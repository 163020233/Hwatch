/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: LiteOS USB composite adapt
 * Author: Huawei LiteOS Team
 * Create: 2022-07-22
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

#include "kal_spinlock.h"
#include "gadget/composite_pri.h"
#include "gadget/composite_gadget.h"
#include "implementation/_thread_ref.h"

typedef struct
{
  device_type type;
  int (*callback)(void *handle);
} dev_init_ops;

static dev_init_ops g_usb_dev_init[] =
{
#ifdef LOSCFG_DRIVERS_USB_MASS_STORAGE_GADGET
#ifdef LOSCFG_DRIVERS_USB_MASS_STORAGE_EXT_SER_FUNC
  { DEV_MASS, usbdev_mass_ser_initialize},
#else
  { DEV_MASS, usbdev_mass_initialize },
#endif
#endif

#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET
  { DEV_UVC, usbdev_uvc_initialize },
#endif

#ifdef LOSCFG_DRIVERS_USB_UAC_GADGET
  { DEV_UAC, usbdev_uac_initialize },
#endif

#ifdef LOSCFG_DRIVERS_USB_CAMERA_GADGET
  { DEV_CAMERA, usbdev_camera_initialize },
#endif

#ifdef LOSCFG_DRIVERS_USB_SERIAL_GADGET
  { DEV_SERIAL, usbdev_acm_initialize },
#endif

#ifdef LOSCFG_DRIVERS_USB_ETHERNET_GADGET
  { DEV_ETHERNET, usbdev_rndis_initialize },
#endif

#ifdef LOSCFG_DRIVERS_USB_ETH_SER_GADGET
  { DEV_SER_ETH, usbdev_multi_initialize },
#endif

#ifdef LOSCFG_DRIVERS_USB_ECM_SER_GADGET
  { DEV_SER_ECM, usbdev_multi_initialize },
#endif

#ifdef LOSCFG_DRIVERS_USB_DFU_GADGET
  { DEV_DFU, usbdev_dfu_initialize },
#endif

#ifdef LOSCFG_DRIVERS_USB_HID_GADGET
  { DEV_HID, usbdev_hid_initialize },
#endif

#ifdef LOSCFG_DRIVERS_USB_UAC_HID_GADGET
  { DEV_UAC_HID, usbdev_uac_hid_initialize },
#endif

#ifdef LOSCFG_DRIVERS_USB_ECM_GADGET
  { DEV_ECM, usbdev_ecm_initialize },
#endif

  { DEV_END, NULL }
};

static const struct usb_qualdesc_s g_qualdesc =
{
  USB_SIZEOF_QUALDESC,                               /* len */
  USB_DESC_TYPE_DEVICEQUALIFIER,                    /* type */
  {                                                  /* usb */
     LSBYTE(0x0200),
     MSBYTE(0x0200)
  },
  0,                                             /* classid */
  0,                                            /* subclass */
  0,                                            /* protocol */
  CONFIG_COMPOSITE_EP0MAXPACKET,            /* mxpacketsize */
  COMPOSITE_CONFIGID,                           /* nconfigs */
  0,                                            /* reserved */
};

static KAL_SPIN_LOCK_INIT(g_info_spinlock);

uint32_t composite_spin_lock_irqsave(void)
{
  uint32_t flags;
  KAL_SpinLockSave(&g_info_spinlock, &flags);
  return flags;
}

void composite_spin_unlock_irqrestore(uint32_t flags)
{
  KAL_SpinUnlockRestore(&g_info_spinlock, flags);
}

/* Specific protocol logic processing, such as UAC class setup. */

int composite_classsetup_specific(FAR struct composite_dev_s *priv,
                                  FAR struct usbdev_s *dev,
                                  FAR const struct usb_ctrlreq_s *ctrl,
                                  FAR uint8_t *dataout, size_t outlen)
{
  int i, j;
  int ret = -EOPNOTSUPP;
  uint8_t  interface;
  uint16_t index;
  struct composite_devobj_s *devobj;
  struct usbdev_devinfo_s *devinfo;

  index     = USB_GETW(ctrl->index);
  interface = (uint8_t)(index & 0xff);

  for (i = 0; i < priv->ndevices; i++)
    {
      devobj  = &priv->device[i];
      devinfo = &devobj->compdesc.devinfo;
      for (j = 0; j < USBDEV_MAX_EPNUM; j++)
        {
          if (devinfo->epno[j] == interface)
            {
              ret = CLASS_SETUP(devobj->dev, dev, ctrl, dataout, outlen);
              break;
            }
        }
    }
  return ret;
}

static int modify_config_descriptor(const struct usbdev_s *dev, struct usbdev_req_s *req, uint8_t type)
{
  int ret;
  uint32_t i;
  uint32_t new_config_len;
  uint32_t config_len = req->len;
  uint8_t *config_tmp_buf;
  struct usb_desc_s *descriptors;
  struct usb_cfgdesc_s *config_des;

  if (dev->speed != USB_SPEED_SUPER && dev->speed != USB_SPEED_HIGH && dev->speed != USB_SPEED_FULL)
    {
      usb_err("Device controller speed can not support!\n");
      return -EINVAL;
    }

  if (dev->speed == USB_SPEED_SUPER)
    {
      return 0; /* The configuration descriptor defaults to the superspeed form. */
    }

  config_tmp_buf = zalloc(config_len);
  if (config_tmp_buf == NULL)
    {
      usb_err("malloc failed\n");
      return -ENOMEM;
    }

  descriptors = (struct usb_desc_s *)req->buf;
  i = 0;
  new_config_len = 0;
  while (i < config_len)
    {
      i += descriptors->len;
      if (descriptors->type != USB_DESC_TYPE_ENDPOINT_COMPANION)
        {
          ret = memcpy_s(config_tmp_buf + new_config_len, config_len - new_config_len,
                         descriptors, descriptors->len);
          if (ret != EOK)
            {
              usb_err("memcpy_s failed, %d\n", ret);
              goto err;
            }
          new_config_len += descriptors->len;
        }
      descriptors = (struct usb_desc_s *)((char *)descriptors + descriptors->len);
    }
  ret = memset_s(req->buf, USB_COMP_EP0_BUFSIZ, 0, config_len);
  if (ret != EOK)
    {
      usb_err("memset_s failed, %d\n", ret);
      goto err;
    }

  ret = memcpy_s(req->buf, USB_COMP_EP0_BUFSIZ, config_tmp_buf, new_config_len);
  if (ret != EOK)
    {
      usb_err("memcpy_s failed, %d\n", ret);
      goto err;
    }

  config_des = (struct usb_cfgdesc_s *)req->buf;
  USB_SETW(config_des->totallen, new_config_len);
  config_des->type = type;
  req->len = new_config_len;
  free(config_tmp_buf);
  return 0;

err:
  free(config_tmp_buf);
  return -EINVAL;
}

int composite_get_config_descriptor(const struct usbdev_s *dev,
                                    struct usbdev_req_s *req, unsigned len, uint8_t type)
{
  int ret;
  int retval;
  int i = 0;
  struct usb_desc_s *descriptors;
  struct usb_epdesc_s *ep_des;

  ret = modify_config_descriptor(dev, req, type);
  if (ret < 0)
    {
      return -1;
    }

  retval = min(len, req->len);
  descriptors = (struct usb_desc_s *)req->buf;

  while (i < retval)
    {
      i += descriptors->len;
      if (descriptors->type != USB_DESC_TYPE_ENDPOINT)
        {
          descriptors = (struct usb_desc_s *)((char *)descriptors + descriptors->len);
          continue;
        }

      ep_des = (struct usb_epdesc_s *)descriptors;
      usbd_configep_byspeed(dev, ep_des, type);
      descriptors = (struct usb_desc_s *)((char *)descriptors + descriptors->len);
    }

  req->len = retval;
  return retval;
}

void modify_device_descriptor_byspeed(const struct usbdev_s *dev, uint8_t *buf)
{
  struct usb_devdesc_s *desc = (struct usb_devdesc_s *)buf;
  switch (dev->speed)
    {
    case USB_SPEED_SUPER:
      USB_SETW(desc->usb, 0x0300);
      desc->mxpacketsize = 0x09;
      break;

    case USB_SPEED_HIGH:
      USB_SETW(desc->usb, 0x0200);
      desc->mxpacketsize = 0x40;
      break;
    case USB_SPEED_FULL:
      USB_SETW(desc->usb, 0x0110);
      desc->mxpacketsize = 0x40;
      break;
    default:
      usb_err("invalid speed:%u\n", dev->speed);
      break;
    }
}

void composite_setup_complete(FAR struct usbdev_ep_s *ep,
                              FAR struct usbdev_req_s *req)
{
  (void)ep;
  (void)req;
}

void usbd_configep_byspeed(const struct usbdev_s *dev, struct usb_epdesc_s *ep_desc, uint8_t conftype)
{
  if (ep_desc->attr == USB_EP_ATTR_XFER_BULK)
    {
      switch (dev->speed)
        {
        case USB_SPEED_SUPER:
          USB_SETW(ep_desc->mxpacketsize, USB_DWC_MAX_PACKET_SIZE);
          break;

        case USB_SPEED_HIGH:
        {
          if (conftype == USB_DESC_TYPE_OTHERSPEEDCONFIG)
            {
              USB_SETW(ep_desc->mxpacketsize, USB_FS_MAX_BULK_PACKET_SIZE);
            }
           else
            {
              USB_SETW(ep_desc->mxpacketsize, USB_DWC_U2_MAX_PACKET_SIZE);
            }
          break;
        }
        case USB_SPEED_FULL:
          USB_SETW(ep_desc->mxpacketsize, USB_FS_MAX_BULK_PACKET_SIZE);
          break;

        default:
          usb_err("invalid speed:%u\n", dev->speed);
          break;
        }
    }

  return;
}

static int composite_device_init(struct gadget_device *dev)
{
  device_type type = dev_type_get();

  for (uint32_t i = 0; g_usb_dev_init[i].callback != NULL; i++)
    {
      if (g_usb_dev_init[i].type == type)
        {
          return g_usb_dev_init[i].callback((void *)dev);
        }
    }

  return -1;
}

int composite_attach(struct gadget_device *dev)
{
  struct composite_alloc_s *usb_cdev;
  void *parnet_conext = dev->udc_sc;
  int ret;

  /* Probe and initiate device driver */

  ret = composite_device_init(dev);
  if (ret < 0)
    {
      usb_err("composite device init failed!, ret:%d\n", ret);
      return -1;
    }

  usb_cdev = dev->composite_sc;
  usb_cdev->drvr.dev = &usb_cdev->dev;
  ret = usbd_gadget_attach_driver(parnet_conext, &usb_cdev->drvr.drvr);
  if (ret < 0)
    {
      usb_err("failed, err=%d!\n", ret);
      goto detach;
    }

  return 0; /* Attach success */

detach:
  (void)composite_detach(dev);
  return -1; /* No such device or address */
}

int composite_detach(struct gadget_device *dev)
{
  struct composite_alloc_s *usb_cdev = dev->composite_sc;
  void *parnet_conext = dev->udc_sc;
  int ret;

  ret = usbd_gadget_detach_driver(parnet_conext, &usb_cdev->drvr.drvr);
  if (ret != 0)
    {
      usb_err("failed, err=%d!\n", ret);
      return ret;
    }
  composite_uninitialize(usb_cdev);

  return (0);
}

struct composite_devobj_s *usbclass_devobj_get(struct composite_dev_s *cdev, device_type type)
{
  int i;
  struct composite_devdesc_s *compdesc;

  for (i = 0; i < NUM_DEVICES_TO_HANDLE; i++)
    {
      compdesc = &cdev->device[i].compdesc;
      if (compdesc->minor == (int)type)
        {
          return &cdev->device[i];
        }
    }

  return NULL;
}

void dev_str_desc_free(struct usbd_string *device_strings)
{
  int i;
  uint32_t flags;

  flags = composite_spin_lock_irqsave();

  for (i = 0; i < STRING_DESC_NUM; i++)
    {
      if (device_strings[i].s != NULL)
        {
          free((void *)device_strings[i].s);
          device_strings[i].s = NULL;
        }
    }

  composite_spin_unlock_irqrestore(flags);
}

uint8_t *ep_alloc_buffer(int len)
{
  uint8_t *buf = (uint8_t *)KAL_MemAllocAlign((void *)g_usb_mem_addr_start, len, USB_CACHE_ALIGN_SIZE);
  if (buf == NULL)
    {
      return NULL;
    }
  (void)memset_s(buf, len, 0, len);
  return buf;
}

/****************************************************************************
 * Private Data
 ****************************************************************************/

#define OS_STRING_SIGN_LEN 18
#define OS_STRING_ID       0xEE

static const char g_os_string[OS_STRING_SIGN_LEN] =
{
  OS_STRING_SIGN_LEN,
  USB_DESC_TYPE_STRING,
  'L', 0, 'i', 0, 't', 0, 'e', 0,
  'O', 0, 'S', 0, ' ', 0, 0, 0
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: composite_mkstrdesc
 *
 * Description:
 *   Construct a string descriptor
 *
 ****************************************************************************/

int composite_getstrdesc(FAR struct composite_dev_s *priv, uint8_t id, uint16_t index, FAR uint8_t *buf)
{
  int i;
  int ret;
  int len = -1;
  uint32_t flags;

  flags = composite_spin_lock_irqsave();

  for (i = 0; i < priv->ndevices; i++)
    {
      len = priv->device[i].compdesc.mkstrdesc(id, (struct usb_strdesc_s *)buf);
    }

  composite_spin_unlock_irqrestore(flags);

  if ((len == -1) && (index == 0) && (id == OS_STRING_ID))
    {
      ret = memcpy_s(buf, USB_COMP_EP0_BUFSIZ, (void *)g_os_string, g_os_string[0]);
      if (ret != EOK)
        {
          return -1;
        }
      return g_os_string[0];
    }

  return len;
}

void composite_mkdevdesc(FAR struct composite_dev_s *priv, FAR uint8_t *buf)
{
  int i;

  for (i = 0; i < priv->ndevices; i++)
    {
      priv->device[i].compdesc.mkdevdesc(buf);
    }
}

int16_t composite_mkcfgdesc_sub(uint8_t *buf, uint16_t len)
{
  int i;
  int ret;
  uint8_t *buf_tmp;
  uint16_t new_config_len = 0;
  struct usb_desc_s *desc;

  buf_tmp = (uint8_t *)malloc(len);
  if (buf_tmp == NULL)
    {
      return -1;
    }

  ret = memcpy_s(buf_tmp, len, buf, USB_SIZEOF_CFGDESC);
  if (ret != EOK)
    {
      goto err;
    }
  len -= USB_SIZEOF_CFGDESC;
  new_config_len += USB_SIZEOF_CFGDESC;

  desc = (struct usb_desc_s *)(buf + USB_SIZEOF_CFGDESC);
  i = 0;
  while (i < len)
    {
      i += desc->len;
      if (desc->type == USB_DESC_TYPE_CONFIG || desc->type ==  USB_DESC_TYPE_OTHERSPEEDCONFIG)
        {
          desc = (struct usb_desc_s *)((char *)desc + desc->len);
          continue;
        }
      ret = memcpy_s(buf_tmp + new_config_len, (len + USB_SIZEOF_CFGDESC - new_config_len),
                     desc, desc->len);
      if (ret != EOK)
        {
          goto err;
        }
      new_config_len += desc->len;
      desc = (struct usb_desc_s *)((char *)desc + desc->len);
    }

  ret = memcpy_s(buf, USB_COMP_EP0_BUFSIZ, buf_tmp, new_config_len);
  if (ret != EOK)
    {
      goto err;
    }
  free(buf_tmp);

  return (int16_t)new_config_len;

err:
  free(buf_tmp);
  return -1;
}

/****************************************************************************
 * Name: composite_mkcfgdesc
 *
 * Description:
 *   Construct the configuration descriptor
 *
 ****************************************************************************/

int16_t composite_mkcfgdesc(FAR struct usbdevclass_driver_s *driver,
                            FAR uint8_t *buf,
                            uint8_t speed, uint8_t type)
{
  FAR struct composite_dev_s *priv = ((FAR struct composite_driver_s *)driver)->dev;
  FAR struct usb_cfgdesc_s *config_desc;
  FAR uint8_t *buf_tmp = buf;
  int16_t len;
  int16_t total = 0;
  uint16_t config_len;
  int i;

  /* Copy all contained interface descriptors into the buffer too */

  for (i = 0; i < priv->ndevices; i++)
    {
#ifdef CONFIG_USBDEV_SUPPORT_CERTIFY
      len = priv->device[i].compdesc.mkconfdesc(buf_tmp, &priv->device[i].compdesc.devinfo,
                                                speed, type);
#else
      (void)speed;
      (void)type;
      len = priv->device[i].compdesc.mkconfdesc(buf_tmp, &priv->device[i].compdesc.devinfo);
#endif
      if (len <= 0)
        {
          return -1;
        }
      total   += len;
      buf_tmp += len;
    }

  if (priv->ndevices > 1)
    {
      config_len = (uint16_t)total;
      total      = composite_mkcfgdesc_sub(buf, config_len);
      if (total < 0)
        {
          return -1;
        }

      config_len  = (uint16_t)total;
      config_desc = (struct usb_cfgdesc_s *)buf;
      USB_SETW(config_desc->totallen, config_len);
      config_desc->ninterfaces = priv->ninterfaces;
    }

  return total;
}

int composite_initialize_softc(uint8_t ndevices, FAR struct composite_devdesc_s *pdevices, void *handle)
{
  void *softc;
  struct gadget_device *dev = (struct gadget_device *)handle;

  if (dev == NULL)
    {
      return -1;
    }

  softc = composite_initialize(ndevices, pdevices);
  if (softc == NULL)
    {
      return -1;
    }
  dev->composite_sc = softc;

  return 0;
}

int usbdev_register(FAR struct usbdevclass_driver_s *driver)
{
  (void)driver;
  return 0;
}

int usbdev_unregister(FAR struct usbdevclass_driver_s *driver)
{
  (void)driver;
  return 0;
}

struct usbdev_req_s *composite_get_ctrlreq_by_dev(struct composite_dev_s *dev)
{
  return dev->ctrlreq;
}

/****************************************************************************
 * Name: composite_getqualdesc
 *
 * Description:
 *   Return a pointer to the raw qual descriptor
 *
 ****************************************************************************/

FAR const struct usb_qualdesc_s *composite_getqualdesc(void)
{
  return &g_qualdesc;
}