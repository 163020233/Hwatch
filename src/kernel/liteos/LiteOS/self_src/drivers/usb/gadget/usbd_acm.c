/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: LiteOS USB Driver Virtual Serial Port transfer
 * Author: Huawei LiteOS Team
 * Create: 2022-12-10
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

#include "f_acm.h"

static Atomic g_magickey;

static int acm_send_in_req(struct acm_data_ctrl *datactrl,
                           struct acm_wrreq_s *wrcontainer)
{
  struct usbdev_ep_s *ep;
  struct usbdev_req_s *req;
  size_t data_size;
  size_t reqlen;
  int ret = 0;
  uint32_t length;

  ep  = datactrl->epbulkin;
  req = wrcontainer->req;

  reqlen = ACM_IN_REQ_LEN;

  while (datactrl->received_char == false)
    {
      /* The data need to submit to host */

      data_size = (RING_BUFFER_SIZE + (wrcontainer->receive_buffer - wrcontainer->send_buffer)) % RING_BUFFER_SIZE;

      /* Stitching the beginning and end of the ringbuffer */

      length = RING_BUFFER_SIZE - (wrcontainer->send_buffer - wrcontainer->data_buffer);
      if (data_size == 0)
        {
          break;
        }
      if (data_size > MAX_PACKET_SIZE)
        {
          data_size = MAX_PACKET_SIZE;
        }
      if (length < data_size)
        {
          ret = memcpy_s(req->buf, reqlen, wrcontainer->send_buffer, length);
          if (ret != EOK)
            {
              usb_err("memcpy_s failed!\n");
              return ret;
            }
          ret = memcpy_s(req->buf + length, reqlen - length, wrcontainer->data_buffer, data_size - length);
          if (ret != EOK)
            {
              usb_err("memcpy_s failed!\n");
              return ret;
            }
          wrcontainer->send_buffer = wrcontainer->data_buffer + data_size - length;
        }
      else
        {
          ret = memcpy_s(req->buf, reqlen, wrcontainer->send_buffer, data_size);
          if (ret != EOK)
            {
              usb_err("memcpy_s failed!\n");
              return ret;
            }
          wrcontainer->send_buffer += data_size;
        }

      datactrl->received_char = true;

      /* Then submit the request to the endpoint. */

      req->zero = 0;
      if (data_size % ep->maxpacket == 0)
        {
          req->zero = 1;
        }

      req->len  = data_size;
      req->priv = wrcontainer;
      ret       = EP_SUBMIT(ep, req);
      if (ret != OK)
        {
          usb_err("submit failed!\n");
          datactrl->received_char = false;
          return ret;
        }
    }
  return ret;
}

/* case 1：
 *    data_buffer    send_buffer          receive_buffer
 *         |_ _ _ _ _ _ _|                    |_ _ _ _ _ _ _
 *         |_ _ _ _ _ _ _|====================|_ _ _ _ _ _ _|
 * 
 * case 2：
 *    data_buffer    receive_buffer       send_buffer
 *         |             |_ _ _ _ _ _ _ _ _ _ |
 *         |=============|_ _ _ _ _ _ _ _ _ _ |=============|
 */

static int acm_receive_buf(struct acm_wrreq_s *wrcontainer,
                           const char *buffer, size_t buflen)
{
  errno_t ret;
  uint32_t remain_length;
  uint32_t min_length;

  remain_length = (wrcontainer->send_buffer == wrcontainer->receive_buffer) ? RING_BUFFER_SIZE :
    (RING_BUFFER_SIZE + (wrcontainer->send_buffer - wrcontainer->receive_buffer)) % RING_BUFFER_SIZE;
  if (buflen >= remain_length)
    {
      return -1;
    }

  /* Determine if the receive_buffer pointer is offset at the end of the ringbuffer */

  const size_t length = RING_BUFFER_SIZE - (wrcontainer->receive_buffer - wrcontainer->data_buffer);

  if (buflen <= length)
    {
      /* There are two cases:
       * 1,In case1, if (buflen <= length), only copy once.
       * 2,In case2, because of (buflen < remain_length <= length), only copy once.
       * And the smaller one is the writable length on the right of receive_buffer.
       */

      min_length = MIN(length, remain_length);
      ret = memcpy_s(wrcontainer->receive_buffer, min_length, buffer, buflen);
      if (ret != EOK)
        {
          usb_err("memcpy_s failed!\n");
          return -1;
        }
      wrcontainer->receive_buffer += buflen;
    }
  else
    {
      /* There are two cases:
       * 1,In case1, if (length == 0), only copy once.
       * 2,In case1, if (length != 0 && buflen > length), need copy twice.
       */

      if (length != 0)
        {
          ret = memcpy_s(wrcontainer->receive_buffer, length, buffer, length);
          if (ret != EOK)
            {
              usb_err("memcpy_s failed!\n");
              return -1;
            }
          buffer += length;
        }

      ret = memcpy_s(wrcontainer->data_buffer, remain_length - length, buffer, buflen - length);
      if (ret != EOK)
        {
          usb_err("memcpy_s failed!\n");
          return -1;
        }
      wrcontainer->receive_buffer = wrcontainer->data_buffer + buflen - length;
    }
  return buflen;
}

static int acm_fill_read_buf(struct acm_data_ctrl *datactrl,
                             const struct acm_rdreq_s *rdcontainer,
                             size_t buflen)
{
  struct usbdev_req_s *req;
  char *req_buf;
  char *read_buf;
  int nbytes;
  int ret;
  uint32_t len;

  req = rdcontainer->req;
  read_buf = rdcontainer->buf;
  req_buf  = (char *)req->buf;

  /* Copy character into read buffer and return the nbytes */

  if (buflen + datactrl->read_offset >= req->xfrd)
    {
      len = req->xfrd - datactrl->read_offset;
    }
  else
    {
      len = buflen;
    }

  ret = memcpy_s(read_buf, buflen, req_buf + datactrl->read_offset, len);
  if (ret != EOK)
    {
      usb_err("memcpy_s failed, %d\n", ret);
      return -1;
    }

  nbytes = (int)len;
  datactrl->read_offset += len;

  if (datactrl->read_offset < req->xfrd)
    {
      ret = (int)LOS_EventWrite(&datactrl->read_event, USB_SERIAL_READ_EVENT);
      if (ret != LOS_OK)
        {
          usb_err("write event failed!\n");
          return -1;
        }
      return nbytes;
    }
  LOS_AtomicSet(&datactrl->send_char, 0);
  datactrl->read_offset = 0;
  return nbytes;
}

int acm_send_out_req(struct acm_data_ctrl *datactrl,
                     struct acm_rdreq_s *rdcontainer)
{
  struct usbdev_req_s *req;
  struct usbdev_ep_s *ep;
  uint32_t flags;
  int ret = 0;

  if (datactrl == NULL || rdcontainer == NULL)
    {
      return -EINVAL;
    }

  spin_lock_irqsave(&datactrl->acm_lock, flags);
  if (LOS_AtomicRead(&datactrl->send_char) == 0)
    {

      LOS_AtomicSet(&datactrl->send_char, 1);

      req      = rdcontainer->req;
      ep       = datactrl->epbulkout;
      req->len = ep->maxpacket;
      ret      = EP_SUBMIT(ep, req);
      if (ret != OK)
        {
          usb_err("epbulkout submit failed!\n");
          LOS_AtomicSet(&datactrl->send_char, 0);
        }
    }
  spin_unlock_irqrestore(&datactrl->acm_lock, flags);
  return ret;
}

static int acm_read_receive_req(struct acm_data_ctrl *datactrl,
                                struct acm_rdreq_s *rdcontainer)
{
  struct usbdev_req_s *req;
  size_t buflen;
  int nbytes = 0;

  if (datactrl == NULL || rdcontainer == NULL)
    {
      usb_err("device unavailable!\n");
      return -ENODEV;
    }

  if (LOS_AtomicRead(&g_magickey) == 1)
    {
      buflen = 1;
    }
  else
    {
      buflen = rdcontainer->buflen;
    }

  if (datactrl->read_pend == true && LOS_AtomicRead(&g_magickey) == 0)
    {
      (void)LOS_EventRead(&datactrl->read_event, USB_SERIAL_READ_EVENT,
                          LOS_WAITMODE_AND | LOS_WAITMODE_CLR, LOS_WAIT_FOREVER);
    }

  req = rdcontainer->req;
  if (req == NULL)
    {
      usb_err("invalid request!\n");
      return -ENODEV;
    }

  switch (req->result)
    {
    case OK:
      {
        nbytes = acm_fill_read_buf(datactrl, rdcontainer, buflen);

        /* Tell host device is ready for next read event */

        (void)acm_send_out_req(datactrl, rdcontainer);
      }
      break;

    case -ESHUTDOWN:
      {
        LOS_AtomicSet(&datactrl->shutdown, 1);
        LOS_AtomicSet(&datactrl->send_char, 0);
      }
      break;

    default:
      break;

    }
  return nbytes;
}

static void acm_clear_check_state(struct acm_dev_s *priv,
                                  struct acm_data_ctrl *datactrl,
                                  enum acm_state_e state)
{
  uint8_t state_flag = 0;
  uint32_t lock_flag;

  spin_lock_irqsave(&priv->state_lock, lock_flag);
  datactrl->acm_flag &= ~state;

  for (int i = 0; i < NUM_SERIAL; i++)
    {
      state_flag |= priv->datactrl[i].acm_flag;
    }
  spin_unlock_irqrestore(&priv->state_lock, lock_flag);
  if (state_flag == ACM_UNBIND_STATE)
    {
      (void)LOS_EventWrite(&priv->unbind_event, ACM_UNBIND_EVENT);
    }
}

static ssize_t serial_read(struct acm_dev_s *priv, struct acm_data_ctrl *datactrl, char *buffer, size_t buflen)
{
  struct acm_rdreq_s *rdcontainer;
  ssize_t nbytes = 0;
  int ret = -1;
  uint32_t lock_flag;

  if (buffer == NULL || buflen == 0)
    {
      return -EINVAL;
    }

  spin_lock_irqsave(&priv->state_lock, lock_flag);
  if (datactrl->acm_flag & ACM_UNBIND_STATE)
    {
      spin_unlock_irqrestore(&priv->state_lock, lock_flag);
      return -EINVAL;
    }

  datactrl->acm_flag |= ACM_READ_BUSY_STATE;
  spin_unlock_irqrestore(&priv->state_lock, lock_flag);

  rdcontainer         = &datactrl->rdreqs;
  rdcontainer->buf    = buffer;
  rdcontainer->buflen = buflen;

  if (datactrl->read_pend != true)
    {
      goto READ_EXIT;
    }

  nbytes = (ssize_t)acm_read_receive_req(datactrl, rdcontainer);

  if (LOS_AtomicRead(&datactrl->shutdown) == 0)
    {
      ret = acm_send_out_req(datactrl, rdcontainer);
      if (ret < 0)
        {
          usb_err("device is not ready for receive next data \n");
          nbytes = ret;
          goto READ_EXIT;
        }
    }

READ_EXIT:
  acm_clear_check_state(priv, datactrl, ACM_READ_BUSY_STATE);
  return nbytes;
}

static ssize_t serial_write(struct acm_dev_s *priv, struct acm_data_ctrl *datactrl,
                            const char *buffer, size_t buflen)
{
  struct acm_wrreq_s *wrcontainer;
  int ret;
  int nbytes = 0;
  uint32_t lock_flag;

  if (buffer == NULL)
    {
      return -1;
    }

  if (buflen == 0)
    {
      return 0;
    }

  spin_lock_irqsave(&priv->state_lock, lock_flag);
  if (datactrl->acm_flag & ACM_UNBIND_STATE)
    {
      spin_unlock_irqrestore(&priv->state_lock, lock_flag);
      return -EINVAL;
    }

  datactrl->acm_flag |= ACM_WRITE_BUSY_STATE;
  spin_unlock_irqrestore(&priv->state_lock, lock_flag);

  wrcontainer = &datactrl->wrreqs;

  nbytes = acm_receive_buf(wrcontainer, buffer, buflen);
  if (nbytes < 0)
    {
      goto WRITE_EXIT;
    }

  spin_lock_irqsave(&datactrl->acm_lock, lock_flag);
  ret = acm_send_in_req(datactrl, wrcontainer);
  spin_unlock_irqrestore(&datactrl->acm_lock, lock_flag);
  if (ret < 0)
    {
      usb_err("file write failed!\n");
      nbytes = ret;
      goto WRITE_EXIT;
    }

WRITE_EXIT:
  acm_clear_check_state(priv, datactrl, ACM_WRITE_BUSY_STATE);
  return nbytes;
}

static int serial_ioctl(struct acm_dev_s *priv, struct acm_data_ctrl *datactrl, int cmd, unsigned long arg)
{
  uint32_t lock_flag;
  int ret = OK;

  spin_lock_irqsave(&priv->state_lock, lock_flag);
  if (datactrl->acm_flag & ACM_UNBIND_STATE)
    {
      spin_unlock_irqrestore(&priv->state_lock, lock_flag);
      return -EINVAL;
    }

  datactrl->acm_flag |= ACM_IOCTL_BUSY_STATE;
  spin_unlock_irqrestore(&priv->state_lock, lock_flag);

  switch (cmd)
    {
    case CONSOLE_CMD_RD_BLOCK_SERIAL:
      {
        if (arg != 0)
          {
            datactrl->read_pend = true;
          }
        else
          {
            datactrl->read_pend = false;
            (void)LOS_EventWrite(&datactrl->read_event, USB_SERIAL_READ_EVENT);
          }
      }
      break;

    default:
      usb_err("Unknown CMD!\n");
      ret = -ENOTTY;
    }

  acm_clear_check_state(priv, datactrl, ACM_IOCTL_BUSY_STATE);
  return ret;
}

#ifdef LOSCFG_DRIVERS_USB_SERIAL_VFS_INTERFACE
static struct acm_dev_s *acm_get_priv_from_file(struct file *filep, int *index)
{
  struct acm_dev_node *cnode = (struct acm_dev_node *)filep->f_inode->i_private;
  if (filep->f_inode->i_flags & FSNODEFLAG_DELETED || cnode == NULL || cnode->dev == NULL)
    {
      usb_err("file get failed!\n");
      return NULL;
    }

  *index = cnode->index;
  return cnode->dev;
}

/****************************************************************************
 * Name: usbclass_acm_read
 *
 * Description:
 *   This function is the external read interface of the serial port device.
 *   It is used to receive the data sent from the host to the serial port.
 *
 ****************************************************************************/

ssize_t usbclass_acm_read(struct file *filep, char *buffer, size_t buflen)
{
  struct acm_dev_s *priv;
  struct acm_data_ctrl *datactrl;
  int index = 0;

  priv = acm_get_priv_from_file(filep, &index);
  if (priv == NULL)
    {
      return -EINVAL;
    }

  datactrl = &priv->datactrl[index];
  return serial_read(priv, datactrl, buffer, buflen);
}

ssize_t usbclass_acm_write(struct file *filep, const char *buffer, size_t buflen)
{
  struct acm_dev_s *priv;
  struct acm_data_ctrl *datactrl;
  int index = 0;

  priv = acm_get_priv_from_file(filep, &index);
  if (priv == NULL)
    {
      return -EINVAL;
    }

  datactrl = &priv->datactrl[index];
  return serial_write(priv, datactrl, buffer, buflen);
}

int usbclass_acm_ioctl(struct file *filep, int cmd, unsigned long arg)
{
  struct acm_dev_s *priv;
  struct acm_data_ctrl *datactrl;
  int index = 0;

  priv = acm_get_priv_from_file(filep, &index);
  if (priv == NULL)
    {
      return -EINVAL;
    }

  datactrl = &priv->datactrl[index];
  return serial_ioctl(priv, datactrl, cmd, arg);
}

int usbclass_acm_open(struct file *filep)
{
  /* Do nothing */

  (void)filep;
  return 0;
}
#else /* LOSCFG_DRIVERS_USB_SERIAL_FUNC_INTERFACE */
static struct acm_dev_s *acm_check_index_and_get_priv(uint32_t index)
{
  struct acm_driver_s *acm_driver = acm_get_acm_driver();
  if (index >= NUM_SERIAL || acm_driver == NULL)
    {
      usb_err("index is error or no device.\n");
      return NULL;
    }

  return &acm_driver->dev;
}

ssize_t usb_serial_read(uint32_t index, char *buffer, size_t buflen)
{
  struct acm_dev_s *priv;
  struct acm_data_ctrl *datactrl;

  priv = acm_check_index_and_get_priv(index);
  if (priv == NULL)
    {
      return -EINVAL;
    }

  datactrl = &priv->datactrl[index];

  return serial_read(priv, datactrl, buffer, buflen);
}

ssize_t usb_serial_write(uint32_t index, const char *buffer, size_t buflen)
{
  struct acm_dev_s *priv;
  struct acm_data_ctrl *datactrl;

  priv = acm_check_index_and_get_priv(index);
  if (priv == NULL)
    {
      return -EINVAL;
    }

  datactrl = &priv->datactrl[index];
  return serial_write(priv, datactrl, buffer, buflen);
}

int usb_serial_ioctl(uint32_t index, int cmd, unsigned long arg)
{
  struct acm_dev_s *priv;
  struct acm_data_ctrl *datactrl;

  priv = acm_check_index_and_get_priv(index);
  if (priv == NULL)
    {
      return -EINVAL;
    }

  datactrl = &priv->datactrl[index];

  return serial_ioctl(priv, datactrl, cmd, arg);
}
#endif /* LOSCFG_DRIVERS_USB_SERIAL_VFS_INTERFACE */

#ifdef LOSCFG_DRIVERS_USB_SERIAL_NOTIFICATION_FUNC
static uint32_t acm_serial_connect_state(const struct acm_dev_s *priv,
                                         struct usbdev_req_s *req,
                                         const void *data,
                                         uint16_t value,
                                         uint32_t len)
{
  struct cdc_notification_element *state_buf;
  void *buf;
  uint32_t ret;

  state_buf = (struct cdc_notification_element *)req->buf;

  state_buf->bmRequestType = USB_REQ_DIR_IN | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
  state_buf->bNotification = ACM_NOTIFY_SERIAL_STATE;
  state_buf->wValue        = value;
  state_buf->wIndex        = priv->ctrl_id;
  state_buf->wLength       = len;

  buf = state_buf + STAE_BUFFER_OFFSET;
  ret = (uint32_t)memcpy_s(buf, sizeof(struct cdc_notification_element), data, len);
  if (ret != EOK)
    {
      usb_err("memcpy_s failed!\n");
      return ret;
    }

  return sizeof(struct cdc_notification_element) + len;
}

int acm_serial_connect(struct acm_dev_s *priv, int index)
{
  struct usbdev_ep_s *ep;
  struct acm_data_ctrl *datactrl;
  struct usbdev_req_s *req;
  uint32_t length;
  int ret;

  if (priv == NULL)
    {
      usb_err("no device can be found!\n");
      return -ENODEV;
    }
  datactrl = &priv->datactrl[index];

  datactrl->serialstate |= ACM_CONNECT_STATE;
  ep = datactrl->epintin;
  if (ep == NULL)
    {
      return -EINVAL;
    }
  req = ep->handle_req;
  if (req == NULL)
    {
      datactrl->waiting = true;
      return 0;
    }

  length = acm_serial_connect_state(priv, req, &datactrl->serialstate, 0,
                                    sizeof(datactrl->serialstate));

  req->priv = priv;
  req->len  = length;

  ret = EP_SUBMIT(ep, req);
  if (ret != OK)
    {
      usb_err("can not send notify data to host\n");
    }
  datactrl->waiting = false;
  return ret;
}

int acm_serial_disconnect(struct acm_dev_s *priv, int index)
{
  struct usbdev_req_s *req;
  struct usbdev_ep_s *ep;
  struct acm_data_ctrl *datactrl;
  uint32_t length;
  int ret;

  datactrl = &priv->datactrl[index];
  userial_mask_set(0);
  datactrl->received_char = false;

  datactrl->serialstate &= ~ACM_CONNECT_STATE;
  ep  = datactrl->epintin;
  if (ep == NULL)
    {
      return -EINVAL;
    }

  req = ep->handle_req;
  if (req == NULL)
    {
      return -EINVAL;
    }

  length = acm_serial_connect_state(priv, req, &datactrl->serialstate,
                                    0, sizeof(datactrl->serialstate));
  req->len = length;

  ret = EP_SUBMIT(ep, req);
  if (ret != OK)
    {
      usb_err("Can not send notify data to host!\n");
    }

  return ret;
}
#else
int acm_serial_connect(struct acm_dev_s *priv, int index)
{
  return 0;
}

int acm_serial_disconnect(struct acm_dev_s *priv, int index)
{
  return 0;
}
#endif

void acm_ep0incomplete_notify_callback(struct usbdev_ep_s *ep,
                                       struct usbdev_req_s *req)
{
  struct acm_dev_node *cnode;
  struct acm_dev_s *priv;
  struct acm_data_ctrl *datactrl;
  int ret;
  uint32_t flags;

  priv = req->priv;
  cnode = (struct acm_dev_node *)ep->priv;
  if (priv == NULL || cnode == NULL)
    {
      usb_err("No device!\n");
      return;
    }
  datactrl = &priv->datactrl[cnode->index];

  if (req->result != -ESHUTDOWN && datactrl->waiting)
    {
      spin_lock_irqsave(&datactrl->acm_lock, flags);
      ret = acm_serial_connect(priv, cnode->index);
      spin_unlock_irqrestore(&datactrl->acm_lock, flags);
      if (ret < 0)
        {
          usb_err("acm device connect failed!\r\n");
        }
    }
}

void acm_rdcomplete_process_req(struct usbdev_ep_s *ep,
                                struct usbdev_req_s *req)
{
  struct acm_dev_node *cnode;
  struct acm_dev_s *priv;
  struct acm_data_ctrl *datactrl;
  struct acm_rdreq_s *rdcontainer;
  uint32_t ret;

  cnode = (struct acm_dev_node *)ep->priv;
  if (cnode == NULL || cnode->dev == NULL)
    {
      usb_err("device is unavailable!\n");
      return;
    }
  priv = cnode->dev;
  datactrl = &priv->datactrl[cnode->index];

  rdcontainer = &datactrl->rdreqs;

  rdcontainer->req      = req;
  rdcontainer->req->len = req->len;

#ifdef LOSCFG_ENABLE_MAGICKEY

  /* Magic key */

  if (CheckMagicKey(*(char *)req->buf))
    {
      LOS_AtomicSet(&g_magickey, 1);
      (void)acm_read_receive_req(datactrl, rdcontainer);

      if (LOS_AtomicRead(&datactrl->shutdown) == 0)
        {
          (void)acm_send_out_req(datactrl, rdcontainer);
        }

      req->buf[0] = 0;
      LOS_AtomicSet(&g_magickey, 0);
      return;
    }
#endif
  ret = LOS_EventWrite(&datactrl->read_event, USB_SERIAL_READ_EVENT);
  if (ret != OK)
    {
      usb_err("write event failed!\n");
    }
}

void acm_wrcomplete_check_send_result(struct usbdev_ep_s *ep,
                                      struct usbdev_req_s *req)
{
  struct acm_dev_node *cnode;
  struct acm_data_ctrl *datactrl;
  struct acm_dev_s *priv;
  struct acm_wrreq_s *wrcontainer;

  cnode = (struct acm_dev_node *)ep->priv;
  if (cnode == NULL || cnode->dev == NULL)
    {
      usb_err("device is unavailable!\n");
      return;
    }
  priv = cnode->dev;
  datactrl = &priv->datactrl[cnode->index];

  spin_lock(&datactrl->acm_lock);

  wrcontainer = (struct acm_wrreq_s *)req->priv;

  datactrl->received_char = false;

  switch (req->result)
    {
    case OK:
      {
        (void)acm_send_in_req(datactrl, wrcontainer);
      }
      break;

    default:
      break;
    }
  spin_unlock(&datactrl->acm_lock);
}
