/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: LiteOS USB Driver UVC Ioctl Source File
 * Author: Huawei LiteOS Team
 * Create: 2021-12-06
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

#include "gadget/usbd_video_ioctl_pri.h"
#include "implementation/usb_list.h"

#ifdef LOSCFG_DRIVERS_USB_UVC_IOCTL
#include <fs/fs.h>

#ifdef USBD_VIDEO_DEV_DEBUG
#define UVD_LOG(x...) dprintf(x)
#else
#define UVD_LOG(x...) do {} while(0)
#endif

static int uvc_open(FAR struct file *filep);
static int uvc_close(FAR struct file *filep);
static int uvc_ioctl(struct file *filep, int cmd, unsigned long data);
static int uvc_poll(struct file *filep, poll_table *table);

static const struct file_operations_vfs g_uvc_fops =
{
  .open   = uvc_open,
  .close  = uvc_close,
  .ioctl  = uvc_ioctl,
#ifndef CONFIG_DISABLE_POLL
  .poll   = uvc_poll,
#endif
};

static struct usbd_uvc_event_queue_node *event_node_zalloc(uint32_t len)
{
  struct usbd_uvc_event_queue_node *event_node = zalloc(sizeof(struct usbd_uvc_event_queue_node));
  if (event_node == NULL)
    {
      usb_err("zalloc event queue node failed\n");
      return NULL;
    }

  event_node->buf_len = len;
  event_node->buf     = zalloc(len);
  if (event_node->buf == NULL)
    {
      free(event_node);
      usb_err("zalloc event queue node buf failed\n");
      return NULL;
    }

  return event_node;
}

static void event_node_free(struct usbd_uvc_event_queue_node *event_node)
{
  free(event_node->buf);
  event_node->buf = NULL;
  free(event_node);
}

static void video_event_queue_free(struct uvc_dev_s *fuvc)
{
  struct usbd_uvc_event_queue_node *node;
  uint32_t flags;

  spin_lock_irqsave(&fuvc->uvc_event_lock, flags);
  while (!LOS_ListEmpty(&fuvc->uvc_event_queue))
    {
      node = LOS_LIST_FIRST_ENTRY(&fuvc->uvc_event_queue, struct usbd_uvc_event_queue_node, event_queue_node);
      LOS_ListDelInit(&node->event_queue_node);
      fuvc->uvc_event_queue_len--;
      event_node_free(node);
    }
  spin_unlock_irqrestore(&fuvc->uvc_event_lock, flags);
}

static void video_event_check_del(struct uvc_dev_s *fuvc)
{
  struct usbd_uvc_event_queue_node *node;

  if (fuvc->uvc_event_queue_len > UVC_EVENT_UPPER_LIMIT)
    {
      usb_warn("events are too much to replace old one\n");
      node = LOS_LIST_FIRST_ENTRY(&fuvc->uvc_event_queue, struct usbd_uvc_event_queue_node, event_queue_node);
      LOS_ListDelInit(&node->event_queue_node);
      fuvc->uvc_event_queue_len--;
      free(node);
    }
}

int queue_video_event(struct uvc_dev_s *fuvc, uint32_t event_type)
{
  uint32_t flags;
  struct usbd_uvc_event_queue_node *event_node;
  struct usbd_uvc_video_event *uvc_event;

  if (!is_ioctl_mode())
    {
      UVD_LOG("function call mode no event queue\n");
      return 0;
    }

  event_node = event_node_zalloc(sizeof(struct usbd_uvc_video_event));
  if (event_node == NULL)
    {
      return UVC_ERROR_MEMORY;
    }

  uvc_event = (struct usbd_uvc_video_event *)event_node->buf;
  uvc_event->event_type = event_type;
  if (event_type == UVC_EVENT_DATA)
    {
      (void)memcpy_s(&uvc_event->event_data.format, sizeof(struct uvc_format_info),
        &fuvc->format_info, sizeof(struct uvc_format_info));
    }

  spin_lock_irqsave(&fuvc->uvc_event_lock, flags);
  video_event_check_del(fuvc);
  LIST_ADD_TAIL(&event_node->event_queue_node, &fuvc->uvc_event_queue);
  fuvc->uvc_event_queue_len++;
  spin_unlock_irqrestore(&fuvc->uvc_event_lock, flags);

  UVD_LOG("event_type %u uvc_event_queue_len %u\n",
          uvc_event->event_type, fuvc->uvc_event_queue_len);

  return 0;
}

static int dequeue_video_event(struct uvc_dev_s *fuvc, struct usbd_uvc_video_event *uvc_event)
{
  uint32_t flags;
  struct usbd_uvc_event_queue_node *event_node = NULL;

  if (uvc_event == NULL)
    {
      return UVC_ERROR_PTR;
    }

  spin_lock_irqsave(&fuvc->uvc_event_lock, flags);
  if (!LOS_ListEmpty(&fuvc->uvc_event_queue))
    {
      event_node = LOS_LIST_FIRST_ENTRY(&fuvc->uvc_event_queue, struct usbd_uvc_event_queue_node, event_queue_node);
      LOS_ListDelInit(&event_node->event_queue_node);
      fuvc->uvc_event_queue_len--;
    }
  spin_unlock_irqrestore(&fuvc->uvc_event_lock, flags);

  if (event_node != NULL)
    {
      (void)memcpy_s(uvc_event, sizeof(struct usbd_uvc_video_event),
        event_node->buf, sizeof(struct usbd_uvc_video_event));

      UVD_LOG("%s event_type %u uvc_event_queue_len %u\n",
              __FUNCTION__, uvc_event->event_type, fuvc->uvc_event_queue_len);
      event_node_free(event_node);
    }
  else
    {
      uvc_event->event_type = UVC_EVENT_FIRST;
    }

  return 0;
}

static int uvc_open(FAR struct file *filep)
{
  if (set_control_mode(MODE_IOCTL))
    {
      usb_warn("uvc control is in function call mode!\n");
      return -1;
    }

  struct uvc_dev_s *fuvc = (struct uvc_dev_s *)filep->f_inode->i_private;
  if (fuvc == NULL)
    {
      usb_err("Invalid private parameter!\n");
      clear_control_mode();
      return -1;
    }

  return 0;
}

static int uvc_close(FAR struct file *filep)
{
  struct uvc_dev_s *fuvc = (struct uvc_dev_s *)filep->f_inode->i_private;
  if (fuvc == NULL)
    {
      usb_err("Invalid private parameter!\n");
      return -1;
    }

  clear_control_mode();

  return 0;
}

static int uvc_ioctl(struct file *filep, int cmd, unsigned long data)
{
  int ret = EOPNOTSUPP;
  struct usbd_uvc_video_event *uvc_event;
  struct uvc_dev_s *fuvc = (struct uvc_dev_s *)filep->f_inode->i_private;
  if (fuvc == NULL)
    {
      UVD_LOG("Invalid private parameter!\n");
      return -1;
    }

  switch (cmd)
    {
    case VIDIOC_STREAMON:
      ret = set_uvc_streamon(fuvc);
      break;

    case VIDIOC_STREAMOFF:
      ret = set_uvc_streamoff(fuvc);
      break;

    case VIDIOC_DQEVENT:
      uvc_event = (struct usbd_uvc_video_event *)data;
      ret = dequeue_video_event(fuvc, uvc_event);
      break;

    default:
      usb_warn("%s invalid cmd %d\n", __FUNCTION__, cmd);
      break;
    }

  return ret;
}

static int uvc_poll(struct file *filep, poll_table *fds)
{
  int revents = 0;
  int flags;
  struct uvc_dev_s *fuvc = (struct uvc_dev_s *)filep->f_inode->i_private;
  if (fuvc == NULL)
    {
      usb_warn("Invalid private parameter!\n");
      return -1;
    }

  if ((fds->key & (POLLIN | POLLRDNORM)) == 0)
    {
      usb_warn("uvc poll only support read!\n");
      return -1;
    }

  spin_lock_irqsave(&fuvc->uvc_event_lock, flags);
  if (fuvc->uvc_event_queue_len > 0)
    {
      revents |= fds->key & (POLLIN | POLLRDNORM);
      UVD_LOG("uvc_event_queue_len %u\n", fuvc->uvc_event_queue_len);
    }
  spin_unlock_irqrestore(&fuvc->uvc_event_lock, flags);

  return revents;
}

int uvc_fops_init(struct uvc_dev_s *fuvc)
{
  UVD_LOG("%s in\n", __FUNCTION__);
  int ret = register_driver(USB_UVC_DEV, &g_uvc_fops, O_RDWR, fuvc);
  if (ret != OK)
    {
      usb_err("uvc_dev register failed! %d\n", ret);
      return ret;
    }

  LOS_ListInit(&fuvc->uvc_event_queue);
  spin_lock_init(&fuvc->uvc_event_lock);

  return ret;
}

int uvc_fops_deinit(struct uvc_dev_s *fuvc)
{
  UVD_LOG("%s in\n", __FUNCTION__);
  video_event_queue_free(fuvc);

  int ret = unregister_driver(USB_UVC_DEV);
  if (ret != OK)
    {
      usb_err("uvc_dev unregister failed! %d\n", ret);
    }

  return ret;
}

#else /* !LOSCFG_DRIVERS_USB_UVC_IOCTL */

int queue_video_event(struct uvc_dev_s *fuvc, uint32_t event_type)
{
  (void)fuvc;
  (void)event_type;
  return 0;
}

#endif /* LOSCFG_DRIVERS_USB_UVC_IOCTL */