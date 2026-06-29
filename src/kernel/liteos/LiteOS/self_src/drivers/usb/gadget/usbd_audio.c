/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: LiteOS USB Driver Audio Stream
 * Author: Huawei LiteOS Team
 * Create: 2017-12-12
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --------------------------------------------------------------------------- */

#include "los_mux.h"
#include "gadget/usbd_audio_pri.h"
#include "gadget/f_uac_pri.h"
#include "implementation/usb_list.h"

#if UAC_GADGET_MICROPHONE
static inline int fuac_host_in_connected(const struct uac_dev_s *dev)
{
  return dev->in_connected;
}
#endif

#if UAC_GADGET_SPEAKER
static inline int fuac_host_out_connected(const struct uac_dev_s *fuac)
{
  return fuac->out_connected;
}
#endif

static int uac_wait_host_sub(bool is_speaker)
{
  int ret = UAC_ERROR_VALUE;
  struct uac_softc *fuac;
  struct uac_dev_s *dev;

  if (LOS_MuxPend(usb_mtx, LOS_WAIT_FOREVER) != LOS_OK)
    {
      return UAC_ERROR_VALUE;
    }
  fuac = get_uac();
  if (fuac == NULL)
    {
      ret = UAC_ERROR_PTR;
      goto out;
    }
  dev = &fuac->dev;

  if (is_speaker)
    {
#if UAC_GADGET_SPEAKER
#ifdef LOSCFG_DRIVERS_USB_UAC_DYNAMIC_SPEAKER
      if (fuac_speaker_num_get() == 0)
        {
          goto out;
        }
#endif /* LOSCFG_DRIVERS_USB_UAC_DYNAMIC_SPEAKER */
      ret = fuac_host_out_connected(dev);
#endif /* UAC_GADGET_SPEAKER */
    }
  else
    {
#if UAC_GADGET_MICROPHONE
      ret = fuac_host_in_connected(dev);
#endif
    }
out:
  (void)LOS_MuxPost(usb_mtx);

  return ret;
}

int uac_wait_host(int wait_option)
{
  int ret;

  bool is_speaker = (bool)((unsigned int)wait_option & 0x2); /* bit 1 -> 2 */
  switch (wait_option)
    {
    case UAC_WAIT_HOST_NOP:
      /* fall through */
    case UAC_WAIT_HOST_OUT_NOP:
      ret = uac_wait_host_sub(is_speaker);
      if (ret == 0) /* Device is not connected. */
        {
          ret = UAC_ERROR_NOMATCH;
        }
      else if (ret == 1) /* Device is connected. */
        {
          ret = UAC_OK;
        }
      break;

    case UAC_WAIT_HOST_FOREVER:
      /* fall through */
    case UAC_WAIT_HOST_OUT_FOREVER:
      while (!(ret = uac_wait_host_sub(is_speaker)))
        {
          (void)LOS_TaskDelay(10);
        }

      if (ret != UAC_ERROR_PTR)
        {
          ret = UAC_OK;
        }
      break;

    default:
      return UAC_ERROR_VALUE;
    }

  return ret;
}

int fuac_reqbuf_init(uint32_t buf_count, uint32_t data_len)
{
  struct uac_softc *fuac;
  struct uac_dev_s *dev;
  uint8_t *buf;
  uint32_t uac_pool_size;
  uint32_t i;
  int ret = UAC_OK;

  if (buf_count == 0 || buf_count > UAC_BUF_COUNT_MAX ||
      data_len == 0 || data_len >= UINT32_MAX / UAC_BUF_COUNT_MAX)
    {
      usb_err("invalid buf_count or invalid data_len. max buf_count:%u max data_len:%u\n",
              UAC_BUF_COUNT_MAX, UINT32_MAX / UAC_BUF_COUNT_MAX);
      return UAC_ERROR_PTR;
    }

  if (LOS_MuxPend(usb_mtx, LOS_WAIT_FOREVER) != LOS_OK)
    {
      return UAC_ERROR_VALUE;
    }

  fuac = get_uac();
  if (fuac == NULL || fuac->dev.data_buf_count != 0)
    {
      ret = UAC_ERROR_VALUE;
      goto error;
    }
  dev = &fuac->dev;

  uac_pool_size = buf_count * data_len;
  buf = memalign(USB_CACHE_ALIGN_SIZE, uac_pool_size);
  if (buf == NULL)
    {
      ret = UAC_ERROR_MEMORY;
      goto error;
    }
  (void)memset_s(buf, uac_pool_size, 0, uac_pool_size);

  for (i = 0; i < buf_count; i++)
    {
      dev->data_buf[i] = buf + i * data_len;
      dev->data_buf_flag[i] = UAC_BUF_IDLE;
    }
  dev->data_buf_head = 0;
  dev->data_buf_tail = 0;
  dev->data_offset = 0;
  dev->buf_use_count = 0;
  dev->data_buf_len = data_len;
  dev->data_buf_count = buf_count;

error:
  (void)LOS_MuxPost(usb_mtx);
  return ret;
}

int fuac_reqbuf_deinit(void)
{
  struct uac_softc *fuac;
  struct uac_dev_s *dev;
  int ret = UAC_OK;

  if (LOS_MuxPend(usb_mtx, LOS_WAIT_FOREVER) != LOS_OK)
    {
      return UAC_ERROR_VALUE;
    }

  fuac = get_uac();
  if (fuac == NULL || fuac->dev.data_buf_count == 0 || fuac->dev.buf_use_count != 0)
    {
      ret = UAC_ERROR_VALUE;
      goto error;
    }
  dev = &fuac->dev;

  if (!LOS_AtomicCmpXchg32bits(&dev->busy_flag, 0, 1))
    {
      (void)LOS_EventRead(&dev->in_complete_event, UAC_COMPLETE_EVENT, LOS_WAITMODE_OR |
                          LOS_WAITMODE_CLR, LOS_MS2Tick(UAC_COMPLETE_EVENT_TIMEOUT));
    }

  dev->data_buf_len = 0;
  dev->data_buf_count = 0;
  free(dev->data_buf[0]);

error:
  (void)LOS_MuxPost(usb_mtx);
  return ret;
}

uint8_t *fuac_reqbuf_get(uint32_t *buf_index)
{
  struct uac_softc *fuac;
  struct uac_dev_s *dev;
  uint8_t *buf;
  uint32_t flags;
  uint32_t tail;
  uint32_t head;
  uint32_t buf_cnt;

  if (buf_index == NULL)
    {
      return NULL;
    }
  if (LOS_MuxPend(usb_mtx, LOS_WAIT_FOREVER) != LOS_OK)
    {
      return NULL;
    }

  fuac = get_uac();
  if (fuac == NULL || fuac->dev.data_buf_count == 0)
    {
      (void)LOS_MuxPost(usb_mtx);
      return NULL;
    }
  dev = &fuac->dev;

  spin_lock_irqsave(&dev->lock, flags);
#if UAC_GADGET_MICROPHONE
  if (fuac_host_in_connected(dev) == 0)
#else
  if (fuac_host_out_connected(dev) == 0)
#endif
    {
      spin_unlock_irqrestore(&dev->lock, flags);
      (void)LOS_MuxPost(usb_mtx);
      return NULL;
    }

  tail = dev->data_buf_tail;
  head = dev->data_buf_head;
  buf_cnt = dev->data_buf_count;
  if ((tail + 1) % buf_cnt == head)
    {
      tail = (head + 1) % buf_cnt;
    }
  buf = dev->data_buf[tail];
  *buf_index = tail;
  dev->buf_use_count++;

  dev->data_buf_tail = (tail + 1) % buf_cnt;
  spin_unlock_irqrestore(&dev->lock, flags);
  (void)LOS_MuxPost(usb_mtx);

  return buf;
}

#if UAC_GADGET_MICROPHONE
void uac_send_data_sub(struct uac_dev_s *sc)
{
  struct uac_dev_s *fuac = sc;
  struct usbdev_req_s *req = fuac->inputreq;
  uint32_t head = fuac->data_buf_head;
  uint32_t len = fuac->data_buf_len - fuac->data_offset;

  req->len = g_uac_iso_data_size;
  LOS_AtomicSet(&fuac->busy_flag, 1);

  if (len < g_uac_iso_data_size)
    {
      req->len = len;
    }

  req->buf = fuac->data_buf[head] + fuac->data_offset;

  fuac->data_offset += req->len;
  (void)EP_SUBMIT(fuac->in_ep, req);
}

int fuac_send_message(const uint8_t *buf, uint32_t len, uint32_t buf_index)
{
  struct uac_softc *fuac;
  struct uac_dev_s *dev;
  uint32_t flags;
  int ret = 0;

  if (LOS_MuxPend(usb_mtx, LOS_WAIT_FOREVER) != LOS_OK)
    {
      return UAC_ERROR_VALUE;
    }
  fuac = get_uac();
  if (fuac == NULL || fuac->dev.data_buf_count == 0 || fuac->dev.buf_use_count == 0 || len != fuac->dev.data_buf_len ||
      buf_index >= fuac->dev.data_buf_count || buf != fuac->dev.data_buf[buf_index])
    {
      ret = UAC_ERROR_PTR;
      goto error;
    }
  dev = &fuac->dev;
  spin_lock_irqsave(&dev->lock, flags);

  dev->buf_use_count--;
  if (fuac_host_in_connected(dev) == 0)
    {
      goto done;
    }

  dev->data_buf_flag[buf_index] = UAC_BUF_USE;
  if (LOS_AtomicRead(&dev->busy_flag))
    {
      goto done;
    }
#if !defined (LOSCFG_DRIVERS_USB_UAC_GADGET_VER_2_0)
  (void)EP_DISABLE(dev->in_ep);
  (void)EP_CONFIGURE(dev->in_ep, (const struct usb_epdesc_s *)&g_fuac_as_in_ep_desc, 0);
#endif

  uac_send_data_sub(dev);

done:
  spin_unlock_irqrestore(&dev->lock, flags);

error:
  (void)LOS_MuxPost(usb_mtx);
  return ret;
}
#endif

#if UAC_GADGET_SPEAKER
struct uac_queue_node *uac_queue_node_alloc(unsigned len)
{
  struct uac_queue_node *queue_node;

  queue_node = zalloc(sizeof(struct uac_queue_node));
  if (queue_node == NULL)
    {
      usb_err("malloc uac queue node failed\n");
      return NULL;
    }

  queue_node->buf_len  = len;
  queue_node->buf_used = 0;
  queue_node->buf      = malloc(len);
  if (queue_node->buf == NULL)
    {
      free(queue_node);
      usb_err("malloc uac queue node buf failed\n");
      return NULL;
    }

  return queue_node;
}

void uac_queue_node_free(struct uac_queue_node *node)
{
  free(node->buf);
  node->buf = NULL;
  free(node);
}

int fuac_receive_message(void *buf, int len, int timeout)
{
  struct uac_dev_s *dev;
  struct uac_softc *fuac;
  struct uac_queue_node *node;
  int copy_size;
  int offset = 0;
  int ret;
  int need_len, remain_len;
  uint32_t err, flags;

  if (buf == NULL || len <= 0 || timeout < 0)
    {
      usb_err("buf is null or len is zero or timeout is less than zero\n");
      return -UAC_ERROR_PTR;
    }
  if (LOS_MuxPend(usb_mtx, LOS_WAIT_FOREVER) != LOS_OK)
    {
      return -UAC_ERROR_VALUE;
    }
#if LOSCFG_DRIVERS_USB_UAC_DYNAMIC_SPEAKER
  if (fuac_speaker_num_get() == 0)
    {
      usb_err("receive message is not avaliable now\n");
      ret = -UAC_ERROR_NOMATCH;
      goto error;
    }
#endif
  fuac = get_uac();
  if (fuac == NULL)
    {
      usb_err("get uac failed\n");
      ret = -UAC_ERROR_PTR;
      goto error;
    }
  dev = &fuac->dev;
  dev->out_start = true;
  if (fuac_host_out_connected(dev) == 0)
    {
      ret = -UAC_ERROR_DISCONNECT;
      goto error;
    }

  err = LOS_EventRead(&dev->out_event, UAC_READ_EVENT, LOS_WAITMODE_OR | LOS_WAITMODE_CLR, timeout);
  if (err != UAC_READ_EVENT)
    {
      ret = (err == LOS_ERRNO_EVENT_READ_TIMEOUT) ? -UAC_ERROR_TIMEOUT : -UAC_ERROR_EVENT;
      usb_err("read event error:0x%x\n", err);
      goto error;
    }

  spin_lock_irqsave(&dev->lock, flags);
  while (!LOS_ListEmpty(&dev->out_queue))
    {
      node = LOS_LIST_FIRST_ENTRY(&dev->out_queue, struct uac_queue_node, irqqueue);
      need_len = len - offset;
      remain_len = node->buf_len - node->buf_used;
      copy_size = (need_len < remain_len) ? need_len : remain_len;
      if (memcpy_s((char*)buf + offset, need_len, node->buf, copy_size) != EOK)
        {
          usb_err("memcpy failed! dst len :%d, src len:%d\n", need_len, copy_size);
          ret = -UAC_ERROR_MEMORY;
          goto error_unlock;
        }
      node->buf_used += copy_size;
      offset += copy_size;
      if (node->buf_used == node->buf_len)
        {
          dev->out_queue_num--;
          LOS_ListDelInit(&node->irqqueue);
          uac_queue_node_free(node);
        }

      if (offset == len)
        {
          break;
        }
    }
  ret = offset;

error_unlock:
  spin_unlock_irqrestore(&dev->lock, flags);

error:
  (void)LOS_MuxPost(usb_mtx);
  return ret;
}
#endif
