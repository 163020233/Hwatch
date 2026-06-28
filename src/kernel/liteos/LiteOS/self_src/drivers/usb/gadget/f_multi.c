/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: LiteOS USB Driver Composite Devices
 * Author: Huawei LiteOS Team
 * Create: 2017-04-21
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

#include "gadget/composite.h"
#include "gadget/f_acm.h"
#ifdef LOSCFG_DRIVERS_USB_ETH_SER_GADGET
#include "gadget/rndis.h"
#include "gadget/f_rndis.h"
#endif
#ifdef LOSCFG_DRIVERS_USB_ECM_SER_GADGET
#include "gadget/f_ecm.h"
#endif

#define USB_ETHER_FIRST_INTERFACE_NUM 0
#define USB_SERIAL_FIRST_INTERFACE_NUM 2

/* This struct is aimed at enumerating index for composite device strings. */

enum
{
  /* Rndis Device */

  MULTI_RNDIS_CTRL_IDX = 6,
  MULTI_RNDIS_DATA_IDX,
  MULTI_RNDIS_IAD_IDX,

  /* ACM Device */

  MULTI_ACM_CTRL_IDX = 0xa,
  MULTI_ACM_DATA_IDX,
  MULTI_ACM_IAD_IDX
};

/* Define and initialize multi device descriptor. */

static struct usb_devdesc_s g_fmulti_device_desc =
{
  .len          = sizeof(struct usb_devdesc_s),
  .type         = USB_DESC_TYPE_DEVICE,
  HSETW(.usb, USB_VERSION_BCD), /* USB version */
  .classid      = USB_CLASS_MISC,
  .subclass     = 2,
  .protocol     = 1,
  .mxpacketsize = USB_EP_MPS,
  HSETW(.vendor, 0),   /* Vendor */
  HSETW(.product, 0),  /* Product */
  HSETW(.device, 0x0318), /* Device version */
  .imfgr        = 1,
  .iproduct     = 2,
  .serno        = 3,
  .nconfigs     = 1,
};

#define MULTI_STRING_ID_LEN  4
static const char g_multi_string_id[MULTI_STRING_ID_LEN] =
{
  MULTI_STRING_ID_LEN,
  USB_DESC_TYPE_STRING,
  0x09,
  0x04,
};

#define MULTI_STRING_4_LEN 38
static const char g_dt_string_4[MULTI_STRING_4_LEN] =
{
  MULTI_STRING_4_LEN,
  USB_DESC_TYPE_STRING,
  0x47, 0, 0x61, 0, 0x64, 0, 0x67, 0, 0x65, 0, 0x74, 0, 0x20, 0,
  0x53, 0, 0x65, 0, 0x72, 0, 0x69, 0, 0x61, 0, 0x6c, 0, 0x20, 0,
  0x76, 0, 0x32, 0, 0x2e, 0, 0x34, 0
};

#define DT_STRING_5_LEN 26
static char g_dt_string_5[DT_STRING_5_LEN] =
{
  DT_STRING_5_LEN,
  USB_DESC_TYPE_STRING,
  0x33, 0, 0x61, 0, 0x38, 0, 0x32, 0, 0x64, 0, 0x36, 0, 0x36, 0, 0x36, 0, 0x31, 0, 0x66, 0,
  0x33, 0, 0x61, 0
};

#define DT_STRING_6_LEN 26
static const char g_dt_string_6[DT_STRING_6_LEN] =
{
  DT_STRING_6_LEN,
  USB_DESC_TYPE_STRING,
  0x43, 0, 0x44, 0, 0x43, 0, 0x20, 0, 0x41, 0,
  0x43, 0, 0x4d, 0, 0x20, 0, 0x44, 0, 0x61, 0, 0x74, 0, 0x61, 0
};

#define DT_STRING_7_LEN 22
static const char g_dt_string_7[DT_STRING_7_LEN] =
{
  DT_STRING_7_LEN,
  USB_DESC_TYPE_STRING,
  0x43, 0, 0x44, 0, 0x43, 0, 0x20, 0, 0x53, 0, 0x65, 0,
  0x72, 0, 0x69, 0, 0x61, 0, 0x6c, 0
};

#define MULTI_RNDIS_STRING_LEN 12
static const char g_multi_rndis_string[MULTI_RNDIS_STRING_LEN] =
{
  MULTI_RNDIS_STRING_LEN,
  USB_DESC_TYPE_STRING,
  'R',0,'N',0,'D',0,'I',0,'S',0
};

#define MULTI_SERIAL_STRING_LEN 22
static const char g_multi_serial_string[MULTI_SERIAL_STRING_LEN] =
{
  MULTI_SERIAL_STRING_LEN,
  USB_DESC_TYPE_STRING,
  'C',0,'D',0,'C',0,' ',0,'S',0,'e',0,'i',0,'a',0,'l',0
};

#define FMULTI_DSTR_IDX 11
static struct usbd_string g_fmulti_device_strings[FMULTI_DSTR_IDX] =
{
  { 0,                   g_multi_string_id },
  { 1,                   NULL },
  { 2,                   NULL },
  { 3,                   NULL },
  { 4,                   g_dt_string_4 },
  { 5,                   g_dt_string_5 },
  { 6,                   g_dt_string_6 },
  { 7,                   g_dt_string_7 },
  { MULTI_RNDIS_IAD_IDX, g_multi_rndis_string },
  { MULTI_ACM_IAD_IDX,   g_multi_serial_string },
  USBD_DEVICE_STRINGS_END
};

void multi_get_device_dec_info(struct usbd_string **device_strings, struct usb_devdesc_s **device_desc)
{
  *device_strings = &g_fmulti_device_strings[1];
  *device_desc    = &g_fmulti_device_desc;
}

void multi_mkdevdesc(uint8_t *buf)
{
  errno_t ret;

  if (USB_GETW(g_fmulti_device_desc.vendor) == 0)
    {
      usb_err("VID is not set!\n");
      return;
    }

  ret = memcpy_s(buf, USB_COMP_EP0_BUFSIZ, &g_fmulti_device_desc, sizeof(g_fmulti_device_desc));
  if (ret != EOK)
    {
      usb_err("memcpy_s fail!, ret:%d\n", ret);
      return;
    }
}

int multi_mkstrdesc(uint8_t id, struct usb_strdesc_s *buf)
{
  errno_t ret;
  const char *str;
  int i;

  for (i = 0; g_fmulti_device_strings[i].s != NULL; i++)
    {
      str = g_fmulti_device_strings[i].s;
      if (g_fmulti_device_strings[i].id == id)
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

void multi_get_composite_devdesc(struct composite_devdesc_s *dev)
{
  dev->mkdevdesc = multi_mkdevdesc;
  dev->mkstrdesc = multi_mkstrdesc;
}

int usbdev_multi_initialize(void *handle)
{
  struct composite_devdesc_s dev[USB_COMPOSITE_DEV_NUM];
  int ret;

  if (dev_type_get() == DEV_SER_ETH)
    {
#ifdef LOSCFG_DRIVERS_USB_ETH_SER_GADGET
      rndis_get_composite_devdesc(&dev[0]);
      usbdev_rndis_initialize_sub(&dev[0], USB_ETHER_FIRST_INTERFACE_NUM, DEV_ETHERNET);
#endif
    }
  else
    {
#ifdef LOSCFG_DRIVERS_USB_ECM_SER_GADGET
      ecm_init_macaddr_desc(g_dt_string_5);
      usbdev_ecm_initialize_dev(&dev[0], USB_ETHER_FIRST_INTERFACE_NUM, DEV_ECM);
#endif
    }
  multi_get_composite_devdesc(&dev[0]);

  usbdev_acm_initialize_dev(&dev[1], USB_SERIAL_FIRST_INTERFACE_NUM, DEV_SERIAL);
  multi_get_composite_devdesc(&dev[1]);

  ret = composite_initialize_softc(USB_COMPOSITE_DEV_NUM, dev, handle);
  if (ret < 0)
    {
      PRINTK("  ** multi device initialized failed! **\n");
      return -1;
    }

  PRINTK("  ** multi device initialized successfully! **\n");
  return 0;
}
