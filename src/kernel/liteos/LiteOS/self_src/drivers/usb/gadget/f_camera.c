/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: LiteOS USB Driver Camera
 * Author: Huawei LiteOS Team
 * Create: 2018-05-17
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

#include "gadget/f_uvc_pri.h"
#include "gadget/camera_control_pri.h"
#include "gadget/f_uac_pri.h"
#include "implementation/usb_list.h"
#ifdef LOSCFG_DRIVERS_USB2_DEVICE_CONTROLLER
#include "controller/usb_device/dwc_otg_pcd.h"
#endif

#define CAMERA_STR_LANG   0
static const char g_fcamera_str_lang[] =
{
  4, USB_DESC_TYPE_STRING,
  0x09, 0x04
};

#define CAMERA_STR_IDX_MAN     1
#define CAMERA_STR_IDX_PRODUCT 2
#define CAMERA_STR_IDX_SERIAL  3

#define CAMERA_STR_IDX_CONFIG   5
static const char g_fcamera_str_config[] =
{
  22, USB_DESC_TYPE_STRING,
  'U', 0, 'V', 0, 'C', 0, ' ', 0, 'C', 0, 'A', 0, 'M', 0,
  'E', 0, 'R', 0, 'A', 0
};

#define CAMERA_STR_IDX_VIDEO   4
static const char g_fcamera_str_video[] =
{
  12, USB_DESC_TYPE_STRING,
  'V', 0, 'i', 0, 'd', 0, 'e', 0, 'o', 0
};

#define CAMERA_STR_IDX_INTERFACE   6
static const char g_fcamera_str_interface[] =
{
  32, USB_DESC_TYPE_STRING,
  'V', 0, 'i', 0, 'd', 0, 'e', 0, 'o', 0, ' ', 0, 'S', 0, 't', 0, 'r', 0,
  'e', 0, 'a', 0, 'm', 0, 'i', 0, 'n', 0, 'g', 0
};

#define CAMERA_IN_TERMINAL   8
static const char g_fcamera_input_terminal[] =
{
  46, USB_DESC_TYPE_STRING,
  'C', 0, 'a', 0, 'p', 0, 't', 0, 'u', 0, 'r', 0, 'e', 0, ' ', 0,
  'I', 0, 'n', 0, 'p', 0, 'u', 0, 't', 0, ' ', 0, 't', 0, 'e', 0,
  'r', 0, 'm', 0, 'i', 0, 'n', 0, 'a', 0, 'l', 0
};

#define CAMERA_OUT_TERMINAL   11
static const char g_fcamera_output_terminal[] =
{
  48, USB_DESC_TYPE_STRING,
  'C', 0, 'a', 0, 'p', 0, 't', 0, 'u', 0, 'r', 0, 'e', 0, ' ', 0, 'O', 0,
  'u', 0, 't', 0, 'p', 0, 'u', 0, 't', 0, ' ', 0, 't', 0, 'e', 0, 'r', 0,
  'm', 0, 'i', 0, 'n', 0, 'a', 0, 'l', 0
};

static struct usb_devdesc_s g_fcamera_device_desc =
{
  .len          = sizeof(struct usb_devdesc_s),
  .type         = USB_DESC_TYPE_DEVICE,   /* Constant for device descriptor */
  HSETW(.usb, USB_VERSION_BCD),           /* USB version required: 2.0 */
  .classid      = USB_CLASS_MISC,         /* Miscellaneous Device Class */
  .subclass     = 0x02,                   /* Common Class */
  .protocol     = 0x01,                   /* Interface Association Descriptor */
  .mxpacketsize = USB_EP_MPS,             /* Control Endpoint packet size */
  HSETW(.vendor,  0),                     /* Vendor ID */
  HSETW(.product, 0),                     /* Product ID */
  HSETW(.device,  0x0318),                /* Device release code */
  .imfgr        = CAMERA_STR_IDX_MAN,     /* Manufacturer name, string index */
  .iproduct     = CAMERA_STR_IDX_PRODUCT, /* Product name, string index */
  .serno        = CAMERA_STR_IDX_SERIAL,  /* Device serial number, string index */
  .nconfigs     = 1                       /* One Configuration */
};

static struct usbd_string g_fcamera_device_strings[10] =
{
  { CAMERA_STR_LANG,          g_fcamera_str_lang },
  { CAMERA_STR_IDX_MAN,       NULL },
  { CAMERA_STR_IDX_PRODUCT,   NULL },
  { CAMERA_STR_IDX_SERIAL,    NULL },
  { CAMERA_STR_IDX_VIDEO,     g_fcamera_str_video },
  { CAMERA_STR_IDX_CONFIG,    g_fcamera_str_config },
  { CAMERA_STR_IDX_INTERFACE, g_fcamera_str_interface },
  { CAMERA_IN_TERMINAL,       g_fcamera_input_terminal },
  { CAMERA_OUT_TERMINAL,      g_fcamera_output_terminal },
  USBD_DEVICE_STRINGS_END
};

void camera_get_device_dec_info(struct usbd_string **device_strings, struct usb_devdesc_s **device_desc)
{
  *device_strings = &g_fcamera_device_strings[1];
  *device_desc    = &g_fcamera_device_desc;
}

static void camera_mkdevdesc(uint8_t *buf)
{
  errno_t ret;

  if (USB_GETW(g_fcamera_device_desc.vendor) == 0)
    {
      usb_err("VID is not set!\n");
      return;
    }

  ret = memcpy_s(buf, USB_COMP_EP0_BUFSIZ, &g_fcamera_device_desc, sizeof(g_fcamera_device_desc));
  if (ret != EOK)
    {
      usb_err("memcpy_s fail!, ret:%d\n", ret);
      return;
    }
}

static int camera_mkstrdesc(uint8_t id, struct usb_strdesc_s *buf)
{
  errno_t ret;
  const char *str;
  int i;

  for (i = 0; g_fcamera_device_strings[i].s != NULL; i++)
    {
      str = g_fcamera_device_strings[i].s;
      if (g_fcamera_device_strings[i].id == id)
        {
          ret = memcpy_s(buf, USB_COMP_EP0_BUFSIZ, str, str[0]);
          if (ret != EOK)
            {
              usb_err("memcpy_s failed, ret = %d\n", ret);
              return -1;
            }
          return str[0];
        }
    }

  usb_err("Can not find the id = %u of string\n", id);
  return -1;
}

void camera_get_composite_devdesc(struct composite_devdesc_s *dev)
{
  dev->mkdevdesc = camera_mkdevdesc;
  dev->mkstrdesc = camera_mkstrdesc;
}

#define USB_UVC_FIRST_INTERFACE_NUM 0
#define USB_UAC_FIRST_INTERFACE_NUM 2
int usbdev_camera_initialize(void *handle)
{
  struct composite_devdesc_s dev[USB_COMPOSITE_DEV_NUM];
  int ret;

  usbdev_uvc_initialize_sub(&dev[0], USB_UVC_FIRST_INTERFACE_NUM, DEV_UVC);
  camera_get_composite_devdesc(&dev[0]);
  usbdev_uac_initialize_sub(&dev[1], USB_UAC_FIRST_INTERFACE_NUM, DEV_UAC);
  camera_get_composite_devdesc(&dev[1]);

  ret = composite_initialize_softc(USB_COMPOSITE_DEV_NUM, dev, handle);
  if (ret < 0)
    {
      return -1;
    }

  PRINTK("  ** camera device initialized successfully! **\n");
  return 0;
}
