/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2017-2022. All rights reserved.
 * Description: LiteOS USB Driver Video Stream
 * Author: Huawei LiteOS Team
 * Create: 2017-04-17
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

#include <los_mux.h>
#include <los_atomic.h>
#include "gadget/usbd_video_ioctl_pri.h"
#include "gadget/f_uvc_pri.h"
#include "gadget/usbd_video.h"
#ifdef LOSCFG_DRIVERS_USB2_DEVICE_CONTROLLER
#include "controller/usb_device/dwc_otgreg.h"
#include "controller/usb_device/dwc_otg_pcd.h"
#endif
#ifdef LOSCFG_DRIVERS_USB3_DEVICE_CONTROLLER
#include "controller/usb_device/pcd.h"
#endif
#include "implementation/usb_list.h"

#define UVC_TASK_PRI      8
#define UVC_HEADER_LEN    2
#define UVC_EVENT_TIMEOUT 100

struct uvc_enabled_dev
{

  /* Local global pointer to the UVC handle structure which is used only in func call mode */

  struct uvc_handle *volatile uvc_hdl;

  /* Local global pointer to the F_UVC structure */

  struct uvc_dev_s *volatile uvc_fuvc;

  /* Mutex to protect global resources and some other resources in struct uvc_dev_s */

  uint32_t uvc_mutex;

  Atomic control_mode_chosen;
};

struct uvc_send_req_s
{
  struct usbdev_req_s *req;
  struct LOS_DL_LIST list;
};

static struct uvc_enabled_dev g_uvc_dev =
{
  .uvc_hdl = (struct uvc_handle *)UVC_INVALID_HANDLE,
  .uvc_fuvc = (struct uvc_dev_s *)UVC_INVALID_HANDLE,
  .uvc_mutex = INVALID_MUX,
  .control_mode_chosen = MODE_NONE,
};

int uvc_acquire_mutex(void)
{
  uint32_t ret = LOS_MuxPend(g_uvc_dev.uvc_mutex, LOS_WAIT_FOREVER);
  if (ret != LOS_OK)
    {
      usb_err("Failed to acquire UVC mutex: %#x\n", ret);
      return UVC_ERROR_VALUE;
    }
  return UVC_OK;
}

void uvc_release_mutex(void)
{
  uint32_t ret = LOS_MuxPost(g_uvc_dev.uvc_mutex);
  if (ret != LOS_OK)
    {
      usb_err("Failed to unlock mutex: %#x\n", ret);
    }
}

bool is_ioctl_mode(void)
{
  return LOS_AtomicRead(&g_uvc_dev.control_mode_chosen) == MODE_IOCTL;
}

bool set_control_mode(int control_mode)
{
  return LOS_AtomicCmpXchg32bits(&g_uvc_dev.control_mode_chosen, control_mode, MODE_NONE);
}

void clear_control_mode(void)
{
  LOS_AtomicSet(&g_uvc_dev.control_mode_chosen, MODE_NONE);
}

static bool fuvc_host_connected(const struct uvc_dev_s *fuvc)
{
  if (fuvc == NULL)
    {
      return 0;
    }

  return fuvc->connected;
}

bool uvc_is_running(void)
{
  return (g_uvc_dev.uvc_hdl == (struct uvc_handle *)UVC_INVALID_HANDLE) ? false : true;
}

/* Check the UVC handle provided by external source */

static int uvc_check_handle(uvc_t hdl)
{
  /* Catch NULL pointer */

  if (hdl == NULL)
    {
      return UVC_ERROR_PTR;
    }

  /* Check whether the provided handle matches the one privated stored */

  if (g_uvc_dev.uvc_hdl == (struct uvc_handle *)UVC_INVALID_HANDLE ||
      hdl != (uvc_t)g_uvc_dev.uvc_hdl)
    {
      return UVC_ERROR_HANDLE;
    }

  return UVC_OK;
}

enum format_switch_status uvc_format_status_check(void)
{
  enum format_switch_status status;
  struct uvc_dev_s *fuvc;

  if (uvc_acquire_mutex() != UVC_OK)
    {
      return FORMAT_SWITCH_FINISH;
    }

  fuvc = g_uvc_dev.uvc_fuvc;
  if (fuvc == (struct uvc_dev_s *)UVC_INVALID_HANDLE)
    {
      uvc_release_mutex();
      usb_err("uvc is not initialization\n");
      return FORMAT_SWITCH_FINISH;
    }

  status = (enum format_switch_status)fuvc->format_info.status;
  uvc_release_mutex();
  return status;
}

int uvc_format_info_get(struct uvc_format_info *info)
{
  struct uvc_format_info *format_info;
  struct uvc_dev_s *fuvc;

  if (info == NULL)
    {
      usb_err("the info is NULL\n");
      return UVC_ERROR_PTR;
    }

  if (uvc_acquire_mutex() != UVC_OK)
    {
      return UVC_ERROR_FATAL;
    }

  fuvc = g_uvc_dev.uvc_fuvc;
  if (fuvc == (struct uvc_dev_s *)UVC_INVALID_HANDLE)
    {
      uvc_release_mutex();
      usb_err("uvc is not initialization\n");
      return UVC_ERROR_PTR;
    }

  format_info = &fuvc->format_info;
  if (format_info->status != FORMAT_SWITCH_PENDING)
    {
      uvc_release_mutex();
      usb_err("the uvc format status is not update\n");
      return UVC_ERROR_NOMATCH;
    }

  if (!uvc_format_info_check(format_info))
    {
      uvc_release_mutex();
      return UVC_ERROR_NOMATCH;
    }

  info->height = format_info->height;
  info->width  = format_info->width;
  info->format = format_info->format;

  fuvc->format_info.status = FORMAT_SWITCH_FINISH;
  uvc_release_mutex();
  return UVC_OK;
}

static int uvc_wait_host_sub(uvc_t hdl, int *connected)
{
  int ret;
  struct uvc_handle *uvc;
  struct uvc_dev_s *fuvc;

  ret = uvc_acquire_mutex();
  if (ret != UVC_OK)
    {
      return ret;
    }

  ret = uvc_check_handle(hdl);
  if (ret != UVC_OK)
    {
      uvc_release_mutex();
      return ret;
    }
  uvc = (struct uvc_handle *)hdl;

  fuvc = (struct uvc_dev_s *)uvc->fuvc;
  if (fuvc == NULL || fuvc != g_uvc_dev.uvc_fuvc)
    {
      uvc_release_mutex();
      return UVC_ERROR_PTR;
    }

  if ((*connected = fuvc_host_connected(fuvc)) != 0)
    {
      if (uvc->state == UVC_STATE_IDLE)
        {
          uvc->state = UVC_STATE_CONN;
        }
    }

  uvc_release_mutex();
  return ret;
}

int uvc_wait_host(uvc_t hdl, int wait_option, int *connected)
{
  int ret;

  if (connected == NULL)
    {
      return UVC_ERROR_PTR;
    }

  switch (wait_option)
    {
    case UVC_WAIT_HOST_NOP:
      ret = uvc_wait_host_sub(hdl, connected);
      break;

    case UVC_WAIT_HOST_FOREVER:
      while (1)
        {
          ret = uvc_wait_host_sub(hdl, connected);
          if (ret != UVC_OK || *connected)
            {
              break;
            }
          (void)LOS_TaskDelay(10);
        }
      break;

    default:
      ret = UVC_ERROR_VALUE;
    }

  return ret;
}

int uvc_open_device(uvc_t *hdl, const struct uvc_open_param *param)
{
  int ret;
  struct uvc_handle *uvc;
  struct uvc_dev_s *fuvc;

  /* NULL pointers are not allowed */

  if (hdl == NULL || param == NULL)
    {
      return UVC_ERROR_PTR;
    }

  /* Check the function input video parameter */

  if (param->vid_width <= 0x1 || param->vid_width > UVC_VIDEO_WIDTH_MAX)
    {
      return UVC_ERROR_VALUE;
    }
  if (param->vid_height <= 0x1 || param->vid_height > UVC_VIDEO_HEIGHT_MAX)
    {
      return UVC_ERROR_VALUE;
    }
  if (!uvc_open_format_check(param))
    {
      return UVC_ERROR_VALUE;
    }

  ret = uvc_acquire_mutex();
  if (ret != UVC_OK)
    {
      return ret;
    }

  uvc = g_uvc_dev.uvc_hdl;
  if (uvc != (struct uvc_handle *)UVC_INVALID_HANDLE)
    {
      uvc_release_mutex();
      return UVC_ERROR_HANDLE; /* Already opened */
    }

  /* Check whether F_UVC module has been initialized */

  fuvc = g_uvc_dev.uvc_fuvc;
  if (fuvc == (struct uvc_dev_s *)UVC_INVALID_HANDLE)
    {
      uvc_release_mutex();
      return UVC_ERROR_VALUE;
    }

  if (set_control_mode(MODE_FUNCCALL))
    {
      usb_warn("uvc device has been opened! %d\n", g_uvc_dev.control_mode_chosen);
      uvc_release_mutex();
      return UVC_ERROR_FATAL;
    }

  /* Malloc memory for uvc_handle structure */

  uvc = malloc(sizeof(struct uvc_handle));
  if (uvc == NULL)
    {
      clear_control_mode();
      uvc_release_mutex();
      return UVC_ERROR_MEMORY;
    }

  /* Initialize the UVC handle */

  uvc->vid_w       = param->vid_width;
  uvc->vid_h       = param->vid_height;
  uvc->vid_f       = param->vid_format;
  uvc->state       = UVC_STATE_IDLE;
  uvc->fuvc        = (void *)fuvc;
  fuvc->uvc_handle = (void *)uvc;

  fuvc->bulk_size = param->bulk_size;
  fuvc->img_size  = param->img_size;

  /* Store a private copy of UVC handle */

  g_uvc_dev.uvc_hdl = uvc;

  /* Return the UVC handle */

  *hdl = (uvc_t)uvc;
  uvc_release_mutex();

  return UVC_OK;
}

/* Local function to stop video stream transmission process
 * return error only when a transmission cannot be stopped
 */

static int uvc_video_tran_stop(struct uvc_handle *uvc)
{
  /* Update UVC handle state */

  if (uvc->fuvc != NULL)
    {
      uvc->state = fuvc_host_connected((struct uvc_dev_s *)uvc->fuvc) ? UVC_STATE_CONN : UVC_STATE_IDLE;
    }
  else
    {
      uvc->state = UVC_STATE_IDLE;
    }
  return UVC_OK;
}

int uvc_video_stop(uvc_t hdl)
{
  int ret;

  ret = uvc_acquire_mutex();
  if (ret != UVC_OK)
    {
      return ret;
    }

  ret = uvc_check_handle(hdl);
  if (ret != UVC_OK)
    {
      uvc_release_mutex();
      return ret;
    }

  ret = uvc_video_tran_stop((struct uvc_handle *)hdl);
  uvc_release_mutex();
  return ret;
}

int uvc_close_device(uvc_t hdl)
{
  int ret;
  struct uvc_handle *uvc;
  struct uvc_dev_s *fuvc;
  uint32_t flags;

  ret = uvc_acquire_mutex();
  if (ret != UVC_OK)
    {
      return ret;
    }

  ret = uvc_check_handle(hdl);
  if (ret != UVC_OK)
    {
      uvc_release_mutex();
      return ret;
    }
  uvc = (struct uvc_handle *)hdl;

  fuvc = (struct uvc_dev_s *)uvc->fuvc;
  if (fuvc == NULL)
    {
      uvc_release_mutex();
      return UVC_ERROR_PTR;
    }

  if (LOS_AtomicCmpXchg32bits(&fuvc->uvc_atomic, CLOSE_STATUS, IDLE_STATUS))
    {
      usb_err("uvc is busy\n");
      uvc_release_mutex();
      return UVC_ERROR_PTR;
    }

  uvc->state = UVC_STATE_CLOSE;
  spin_lock_irqsave(&fuvc->lock, flags);
  fuvc->uvc_handle = (void *)UVC_INVALID_HANDLE;

  /* Set the global handle to NULL, and free the handle structure */

  g_uvc_dev.uvc_hdl = (struct uvc_handle *)UVC_INVALID_HANDLE;
  uvc->fuvc = NULL;
  spin_unlock_irqrestore(&fuvc->lock, flags);

  LOS_AtomicSet(&fuvc->uvc_atomic, IDLE_STATUS);
  free(uvc);
  clear_control_mode();
  uvc_release_mutex();

  return UVC_OK;
}

int uvc_get_state(uvc_t hdl, uint32_t *state)
{
  int ret;
  struct uvc_handle *uvc;

  /* Validate function parameters */

  if (state == NULL)
    {
      return UVC_ERROR_PTR;
    }

  ret = uvc_acquire_mutex();
  if (ret != UVC_OK)
    {
      return ret;
    }

  ret = uvc_check_handle(hdl);
  if (ret != UVC_OK)
    {
      uvc_release_mutex();
      return ret;
    }

  uvc = (struct uvc_handle *)hdl;
  if (uvc->state == UVC_STATE_IDLE && uvc->fuvc &&
      fuvc_host_connected((struct uvc_dev_s *)uvc->fuvc))
    {
      uvc->state = UVC_STATE_CONN;
    }

  *state = uvc->state;
  uvc_release_mutex();
  return UVC_OK;
}

int uvc_recv_pack(const struct uvc_pack *pack)
{
  struct uvc_stream_pack *stream_pack;
  struct uvc_stream_pack *temp_pack;
  struct uvc_dev_s *fuvc = g_uvc_dev.uvc_fuvc;
  uint8_t *buf_addr;
  uint8_t *pack_addr;
  uint32_t flags;
  int ret = UVC_OK;

  if (pack == NULL)
    {
      usb_err("pack is null\n");
      return UVC_ERROR_PTR;
    }

  buf_addr = (uint8_t *)((UINTPTR)pack->buf_vir_addr);
  pack_addr = (uint8_t *)((UINTPTR)pack->pack_vir_addr);
  if (buf_addr == NULL || pack->buf_size == 0 || pack_addr == NULL || pack->pack_len == 0 ||
      pack->pack_vir_addr < pack->buf_vir_addr || pack->pack_vir_addr > pack->buf_vir_addr + pack->buf_size ||
      pack->pack_vir_addr > (UINT64_MAX - pack->pack_len) || pack->buf_vir_addr > (UINT64_MAX - pack->buf_size))
    {
      usb_err("failed, invalid param!\n");
      return UVC_ERROR_PTR;
    }

  if (fuvc == NULL || fuvc == (struct uvc_dev_s *)UVC_INVALID_HANDLE)
    {
      usb_err("fuvc is invalid\n");
      return UVC_ERROR_PTR;
    }

  if (LOS_AtomicCmpXchg32bits(&fuvc->uvc_atomic, RECV_STATUS, IDLE_STATUS))
    {
      usb_err("uvc is uninstalling\n");
      return UVC_ERROR_PTR;
    }

  if (!is_ioctl_mode())
    {
      ret = uvc_check_handle((uvc_t)g_uvc_dev.uvc_hdl);
      if (ret != UVC_OK)
        {
          usb_err("uvc is not open or has been closed!\n");
          goto out;
        }

      /* Check whether UVC state matches */

      if (g_uvc_dev.uvc_hdl->state != UVC_STATE_CONN)
        {
          ret = UVC_ERROR_NOMATCH;
          goto out;
        }
    }

  if (!fuvc->connected)
    {
      ret = UVC_ERROR_PTR;
      usb_err("uvc transfer status is stream off!\n");
      goto out;
    }

  stream_pack = (struct uvc_stream_pack *)malloc(sizeof(struct uvc_stream_pack));
  if (stream_pack == NULL)
    {
      ret = UVC_ERROR_MEMORY;
      goto out;
    }
  stream_pack->pack = (struct uvc_pack *)malloc(sizeof(struct uvc_pack));
  if (stream_pack->pack == NULL)
    {
      free(stream_pack);
      ret = UVC_ERROR_MEMORY;
      goto out;
    }

  if ((pack->pack_vir_addr + pack->pack_len) > (pack->buf_vir_addr + pack->buf_size) &&
      pack->pack_vir_addr < (pack->buf_vir_addr + pack->buf_size))
    {
      temp_pack = (struct uvc_stream_pack *)malloc(sizeof(struct uvc_stream_pack));
      if (temp_pack == NULL)
        {
          free(stream_pack->pack);
          free(stream_pack);
          ret = UVC_ERROR_MEMORY;
          usb_err("malloc uvc_stream_pack failed\n");
          goto out;
        }

      temp_pack->free_flag = 0;
      temp_pack->last = false;
      temp_pack->buf = pack_addr;
      temp_pack->buf_len = pack->buf_vir_addr + pack->buf_size - pack->pack_vir_addr;

      spin_lock_irqsave(&fuvc->pack_list_lock, flags);
      LIST_ADD_TAIL(&temp_pack->list, &fuvc->pack_list);
      spin_unlock_irqrestore(&fuvc->pack_list_lock, flags);

      stream_pack->buf = buf_addr;
      stream_pack->buf_len = pack->pack_len - temp_pack->buf_len;
    }
  else
    {
      if (pack->pack_vir_addr == (pack->buf_vir_addr + pack->buf_size))
        {
          stream_pack->buf = buf_addr;
        }
      else
        {
          stream_pack->buf = pack_addr;
        }
      stream_pack->buf_len = pack->pack_len;
    }

  stream_pack->free_flag = 1;
  stream_pack->last = pack->is_frame_end;
  (void)memcpy_s(stream_pack->pack, sizeof(struct uvc_pack), pack, sizeof(struct uvc_pack));

  spin_lock_irqsave(&fuvc->pack_list_lock, flags);
  LIST_ADD_TAIL(&stream_pack->list, &fuvc->pack_list);
  spin_unlock_irqrestore(&fuvc->pack_list_lock, flags);

  if (pack->is_frame_end)
    {
      (void)LOS_EventWrite(&fuvc->uvc_stream_event, STREAM_STATUS_EVENT);
    }

out:
  LOS_AtomicSet(&fuvc->uvc_atomic, IDLE_STATUS);
  return ret;
}

static void uvc_list_free(LOS_DL_LIST *uvc_list, spinlock_t *lock)
{
  struct uvc_stream_pack *stream_pack;
  uint32_t flags;

  spin_lock_irqsave(lock, flags);
  while (!LOS_ListEmpty(uvc_list))
    {
      stream_pack = LOS_LIST_FIRST_ENTRY(uvc_list, struct uvc_stream_pack, list);
      LOS_ListDelInit(&stream_pack->list);
      spin_unlock_irqrestore(lock, flags);

      if (stream_pack->free_flag)
        {
          if (stream_pack->pack->callback_func != NULL)
            {
              stream_pack->pack->callback_func(stream_pack->pack);
            }
          free(stream_pack->pack);
          stream_pack->pack = NULL;
        }
      free(stream_pack);
      spin_lock_irqsave(lock, flags);
    }
  spin_unlock_irqrestore(lock, flags);
}

static void uvc_release_req(const struct uvc_dev_s *fuvc, struct usbdev_req_s *req)
{
  if (req->sg_flags)
    {
      free(req->sg_buf->buf[0]);
      req->sg_buf->buf[0] = NULL;
      free(req->sg_buf);
      req->sg_buf = NULL;
    }
  EP_FREEREQ(fuvc->vs_ep, req);
}

static void uvc_queue_free(struct uvc_dev_s *fuvc)
{
  struct uvc_send_req_s *send_req;
  struct usbdev_req_s *req;
  while (!LOS_ListEmpty(&fuvc->send_queue))
    {
      send_req = LOS_LIST_FIRST_ENTRY(&fuvc->send_queue, struct uvc_send_req_s, list);
      LOS_ListDelInit(&send_req->list);
      req = send_req->req;
      if (req->sg_flags)
        {
          free(req->sg_buf->buf[0]);
          req->sg_buf->buf[0] = NULL;
          free(req->sg_buf);
          req->sg_buf = NULL;
        }
      EP_FREEREQ(fuvc->vs_ep, req);
      free(send_req);
    }
}

static int uvc_send_data_sub(struct uvc_dev_s *fuvc)
{
  struct uvc_send_req_s *send_req;
  struct usbdev_req_s *req;
  uint32_t int_save;
  int ret;

  while (!LOS_ListEmpty(&fuvc->send_queue))
    {
      int_save = LOS_IntLock();
      LOS_TaskLock();
      send_req = LOS_LIST_FIRST_ENTRY(&fuvc->send_queue, struct uvc_send_req_s, list);
      LOS_ListDelInit(&send_req->list);
      req = send_req->req;
      ret = EP_SUBMIT(fuvc->vs_ep, req);

      /* send_req will not use again after free. */

      free(send_req);
      LOS_TaskUnlock();
      LOS_IntRestore(int_save);
      if (ret != 0)
        {
          if (req->last_flag == 1)
            {
              (void)LOS_EventWrite(&fuvc->uvc_transfer_event, UVC_TRANSFER_EVENT);
            }
          uvc_release_req(fuvc, req);
          uvc_queue_free(fuvc);
          return -1;
        }
    }

  return 0;
}

static struct uvc_send_req_s *uvc_allocreq_sg(struct usbdev_ep_s *ep, struct uvc_dev_s *fuvc, bool last)
{
  uint8_t *usb_head;
  uint32_t head_len;
  struct usb_sg *sg;
  struct uvc_send_req_s *send_req;
  struct usbdev_req_s *req;

  send_req = (struct uvc_send_req_s *)malloc(sizeof(struct uvc_send_req_s));
  if (send_req == NULL)
    {
      usb_err("alloc send req failed!\n");
      return NULL;
    }
  req = EP_ALLOCREQ(ep);
  if (req == NULL)
    {
      usb_err("alloc req failed!\n");
      free(send_req);
      return NULL;
    }

  sg = malloc(sizeof(struct usb_sg));
  if (sg == NULL)
    {
      EP_FREEREQ(ep, req);
      free(send_req);
      usb_err("malloc usb_sg failed\n");
      return NULL;
    }
  req->sg_buf   = sg;
  req->sg_flags = 1;
  req->priv     = req;
  req->callback = fuvc_vs_req_complete;

  head_len = SKB_DATA_ALIGN(UVC_HEADER_LEN);
  usb_head = (uint8_t *)memalign(USB_CACHE_ALIGN_SIZE, head_len);
  if (usb_head == NULL)
    {
      EP_FREEREQ(ep, req);
      free(sg);
      free(send_req);
      usb_err("memalign failed\n");
      return NULL;
    }

  usb_head[0] = UVC_HEADER_LEN;
  usb_head[1] = (uint8_t)(UVC_STREAM_HEADER_EOH | (fuvc->fid & UVC_STREAM_HEADER_FID));
  if (last)
    {
      usb_head[1] |= UVC_STREAM_HEADER_EOF;
      req->last_flag = 1;
      fuvc->fid ^= 1;
    }
  sg->buf[0]    = usb_head;
  sg->len[0]    = UVC_HEADER_LEN;
  sg->num_buf   = 1;
  req->len      = UVC_HEADER_LEN;
  send_req->req = req;

  return send_req;
}

static void fuvc_stream_split_pack(struct usbdev_ep_s *ep, struct uvc_dev_s *fuvc)
{
  bool last;
  uint8_t end_flag;
  struct uvc_send_req_s *send_req;
  struct usbdev_req_s *req;
  struct uvc_stream_pack *stream_pack;
  uint32_t flags;
  uint32_t length;
  uint32_t offset;

  spin_lock_irqsave(&fuvc->pack_list_lock, flags);
  while (!LOS_ListEmpty(&fuvc->pack_list))
    {
      stream_pack = LOS_LIST_FIRST_ENTRY(&fuvc->pack_list, struct uvc_stream_pack, list);
      LOS_ListDelInit(&stream_pack->list);
      spin_unlock_irqrestore(&fuvc->pack_list_lock, flags);

      last = false;
      end_flag = 0;
      length = STREAM_BUF_SIZE - UVC_HEADER_LEN;
      offset = 0;

      while (end_flag == 0)
        {
          if ((offset + STREAM_BUF_SIZE - UVC_HEADER_LEN) >= stream_pack->buf_len)
            {
              length = stream_pack->buf_len - offset;
              last = stream_pack->last;
              end_flag = 1;
            }

          send_req = uvc_allocreq_sg(ep, fuvc, last);
          if (send_req == NULL)
            {
              break;
            }
          req = send_req->req;
          req->sg_buf->buf[1] = &stream_pack->buf[offset];
          req->sg_buf->len[1] = length;
          req->sg_buf->num_buf++;
          req->len += length;
          offset += length;

          LIST_ADD_TAIL(&send_req->list, &fuvc->send_queue);
        }

      spin_lock_irqsave(&fuvc->release_list_lock, flags);
      LIST_ADD_TAIL(&stream_pack->list, &fuvc->release_list);
      spin_unlock_irqrestore(&fuvc->release_list_lock, flags);
      if (stream_pack->last)
        {
          return;
        }

      spin_lock_irqsave(&fuvc->pack_list_lock, flags);
    }
  spin_unlock_irqrestore(&fuvc->pack_list_lock, flags);
}

static void fuvc_transfer_thread(void *para)
{
  int ret;
  uint32_t ret_event;
  struct uvc_dev_s *fuvc = (struct uvc_dev_s *)para;
  struct usbdev_ep_s *ep = fuvc->vs_ep;

  for (;;)
    {
      ret_event = LOS_EventRead(&fuvc->uvc_stream_event,
                                STREAM_STATUS_EVENT | UNBIND_STATUS_EVENT | STREAM_CLEAR_EVENT,
                                LOS_WAITMODE_OR | LOS_WAITMODE_CLR, LOS_WAIT_FOREVER);
      if (ret_event & (STREAM_CLEAR_EVENT | UNBIND_STATUS_EVENT))
        {
          uvc_list_free(&fuvc->release_list, &fuvc->release_list_lock);
          uvc_list_free(&fuvc->pack_list, &fuvc->pack_list_lock);

          if (ret_event & UNBIND_STATUS_EVENT)
            {
              (void)LOS_EventWrite(&fuvc->uvc_stream_event, UVC_EXIT_STATUS_EVENT);
              break;
            }
          continue;
        }

      ret = 0;
      while (!LOS_ListEmpty(&fuvc->pack_list))
        {
          fuvc_stream_split_pack(ep, fuvc);

          ret = uvc_send_data_sub(fuvc);
          if (ret != 0)
            {
              break;
            }

          ret_event = LOS_EventRead(&fuvc->uvc_transfer_event, UVC_TRANSFER_EVENT, LOS_WAITMODE_AND |
                                    LOS_WAITMODE_CLR, UVC_EVENT_TIMEOUT);
          if (ret_event == LOS_ERRNO_EVENT_READ_TIMEOUT)
            {
              /* Transfer timeout occurs when switching resolutions */

              ret = -1;
              usb_warn("Transfer timeout!\n");
              break;
            }

          uvc_list_free(&fuvc->release_list, &fuvc->release_list_lock);
        }
      if (ret != 0)
        {
          usbclass_uvc_stream_ep_reset(fuvc);
          (void)LOS_EventWrite(&fuvc->uvc_stream_event, STREAM_CLEAR_EVENT);
        }
    }
}

int uvc_stream_thread_init(struct uvc_dev_s *fuvc)
{
  uint32_t ret;
  TSK_INIT_PARAM_S param;

  spin_lock_init(&fuvc->pack_list_lock);
  spin_lock_init(&fuvc->release_list_lock);

  LOS_ListInit(&fuvc->pack_list);
  LOS_ListInit(&fuvc->release_list);
  LOS_ListInit(&fuvc->send_queue);

  LOS_AtomicSet(&fuvc->uvc_atomic, IDLE_STATUS);

  (void)LOS_EventInit(&fuvc->uvc_stream_event);

  (void)memset_s(&param, sizeof(TSK_INIT_PARAM_S), 0, sizeof(TSK_INIT_PARAM_S));
  param.pfnTaskEntry = (TSK_ENTRY_FUNC)fuvc_transfer_thread;
  param.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
  param.usTaskPrio = UVC_TASK_PRI;
  param.pcName = "UVC_Stream_Task";
  param.uwResved = LOS_TASK_STATUS_DETACHED;
  LOS_TASK_PARAM_INIT_ARG(param, fuvc);

  ret = LOS_TaskCreate(&fuvc->task_id, &param);
  if (ret != LOS_OK)
    {
      (void)LOS_EventDestroy(&fuvc->uvc_stream_event);
      return -1;
    }
  return 0;
}

void uvc_stream_thread_deinit(struct uvc_dev_s *fuvc)
{
  (void)LOS_EventWrite(&fuvc->uvc_stream_event, UNBIND_STATUS_EVENT);
  (void)LOS_EventRead(&fuvc->uvc_stream_event, UVC_EXIT_STATUS_EVENT,
                      LOS_WAITMODE_OR | LOS_WAITMODE_CLR, LOS_WAIT_FOREVER);
  (void)LOS_EventDestroy(&fuvc->uvc_stream_event);
}

void fuvc_vs_req_complete(struct usbdev_ep_s *ep, struct usbdev_req_s *req)
{
  struct uvc_dev_s *fuvc = (struct uvc_dev_s *)ep->priv;

  if (req->result != 0)
    {
      uvc_release_req(fuvc, req);
      return;
    }

  if (req->last_flag == 1)
    {
      (void)LOS_EventWrite(&fuvc->uvc_transfer_event, UVC_TRANSFER_EVENT);
    }
  uvc_release_req(fuvc, req);
}

void fuvc_streaming_on(struct uvc_dev_s *fuvc, uint32_t events)
{
  fuvc->connected = true;

  if (events == UVC_EVENT_LIST && !LOS_ListEmpty(&fuvc->uvc_transfer_event.stEventList))
    {
      (void)LOS_EventWrite(&fuvc->uvc_transfer_event, events);
    }

  return;
}

void fuvc_streaming_off(struct uvc_dev_s *fuvc)
{
  (void)queue_video_event(fuvc, UVC_EVENT_STREAMOFF);

  fuvc->connected = false;
  (void)LOS_EventWrite(&fuvc->uvc_stream_event, STREAM_CLEAR_EVENT);
}

void usbclass_uvc_stream_ep_reset(const struct uvc_dev_s *fuvc)
{
  (void)EP_DISABLE(fuvc->vs_ep);
  EP_FLUSH(fuvc->vs_ep);
#if FUVC_BULK_TO_ISO_CONVERT
  (void)EP_CONFIGURE(fuvc->vs_ep, (const struct usb_epdesc_s *)&g_fuvc_hs_isoc_streaming_ep, 0);
#else
  (void)EP_CONFIGURE(fuvc->vs_ep, (const struct usb_epdesc_s *)&g_fuvc_hs_bulk_streaming_ep, 0);
#endif
}

void uvc_stream_init(struct uvc_dev_s *fuvc)
{
  uint32_t ret;

  /* Never del the mutex */

  if (g_uvc_dev.uvc_mutex == INVALID_MUX)
    {
      ret = LOS_MuxCreate(&g_uvc_dev.uvc_mutex);
      if (ret != LOS_OK)
        {
          usb_err("Mutex create failed, ret:%u\n", ret);
        }
    }
  g_uvc_dev.uvc_fuvc = fuvc;
}

int set_uvc_streamon(struct uvc_dev_s *fuvc)
{
  int ret = uvc_acquire_mutex();
  if (ret != UVC_OK)
    {
      return ret;
    }
  g_uvc_dev.uvc_fuvc = fuvc;
  uvc_release_mutex();
  return 0;
}

int set_uvc_streamoff(struct uvc_dev_s *fuvc)
{
  int ret = uvc_acquire_mutex();
  if (ret != UVC_OK)
    {
      return ret;
    }

  if (g_uvc_dev.uvc_fuvc != fuvc)
    {
      usb_warn("stream has been off\n");
      uvc_release_mutex();
      return 0;
    }

  g_uvc_dev.uvc_fuvc = (struct uvc_dev_s *)UVC_INVALID_HANDLE;
  uvc_release_mutex();
  return 0;
}

void uvc_stream_deinit(struct uvc_dev_s *fuvc)
{
  if (g_uvc_dev.uvc_fuvc != fuvc)
    {
      usb_err("stream has been deinit\n");
      return;
    }
  g_uvc_dev.uvc_fuvc = (struct uvc_dev_s *)UVC_INVALID_HANDLE;
}