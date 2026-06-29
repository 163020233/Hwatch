/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: LiteOS USB Driver Composite Devices (mass storage & serial)
 * Author: Huawei LiteOS Team
 * Create: 2025-04-07
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

#include "gadget/f_mass_storage_pri.h"
#include "gadget/f_acm.h"
#include "gadget/composite.h"

#if defined(LOSCFG_DRIVERS_USB_SERIAL_COUNT) && (LOSCFG_DRIVERS_USB_SERIAL_COUNT > 2)
#error "Invalid LOSCFG_DRIVERS_USB_SERIAL_COUNT"
#endif

#define USB_MASS_FIRST_INTERFACE_NUM 0
#define USB_SERIAL_FIRST_INTERFACE_NUM 1

/* This struct is aimed at enumerating index for composite device strings. */

/* Define and initialize multi device descriptor. */

static struct usb_devdesc_s g_mass_ser_device_desc =
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

#define MASS_SER_STRING_4_LEN 40
static char g_dt_string_4[MASS_SER_STRING_4_LEN] =
{
  MASS_SER_STRING_4_LEN,
  USB_DESC_TYPE_STRING,
  0x4d, 0, 0x61, 0, 0x73, 0, 0x73, 0, 0x20, 0,
  0x53, 0, 0x74, 0, 0x6f, 0, 0x72, 0, 0x61, 0,
  0x67, 0, 0x65, 0, 0x20, 0, 0x47, 0, 0x61, 0,
  0x64, 0, 0x67, 0, 0x65, 0, 0x74, 0
};

#define DT_STRING_7_LEN 22
static const char g_dt_string_7[DT_STRING_7_LEN] =
{
  DT_STRING_7_LEN,
  USB_DESC_TYPE_STRING,
  0x43, 0, 0x44, 0, 0x43, 0, 0x20, 0, 0x53, 0, 0x65, 0,
  0x72, 0, 0x69, 0, 0x61, 0, 0x6c, 0
};


#define FMULTI_DSTR_IDX 7
static struct usbd_string g_mass_ser_strings[FMULTI_DSTR_IDX] =
{
  { 0,                   g_multi_string_id },
  { 1,                   NULL },
  { 2,                   NULL },
  { 3,                   NULL },
  { 4,                   g_dt_string_4 },
  { 7,                   g_dt_string_7 },
  USBD_DEVICE_STRINGS_END
};

void mass_ser_get_device_dec_info(struct usbd_string **device_strings, struct usb_devdesc_s **device_desc)
{
  *device_strings = &g_mass_ser_strings[1];
  *device_desc    = &g_mass_ser_device_desc;
}

static void mass_ser_mkdevdesc(uint8_t *buf)
{
  errno_t ret;

  if (USB_GETW(g_mass_ser_device_desc.vendor) == 0)
    {
      usb_err("VID is not set!\n");
      return;
    }

  ret = memcpy_s(buf, USB_COMP_EP0_BUFSIZ, &g_mass_ser_device_desc, sizeof(g_mass_ser_device_desc));
  if (ret != EOK)
    {
      usb_err("memcpy_s fail!, ret:%d\n", ret);
    }
}

static int mass_ser_mkstrdesc(uint8_t id, struct usb_strdesc_s *buf)
{
  errno_t ret;
  const char *str;
  int i;

  for (i = 0; g_mass_ser_strings[i].s != NULL; i++)
    {
      str = g_mass_ser_strings[i].s;
      if (g_mass_ser_strings[i].id != id)
        {
          continue;
        }

      ret = memcpy_s(buf, USB_COMP_EP0_BUFSIZ, str, str[0]);
      if (ret != EOK)
        {
          usb_err("memcpy_s failed, ret: %d\n", ret);
          return -1;
        }
        return str[0];
    }

  usb_err("Can not find the id = %u of string\n", id);
  return -1;
}

static void mass_ser_get_composite_devdesc(struct composite_devdesc_s *dev)
{
  dev->mkdevdesc = mass_ser_mkdevdesc;
  dev->mkstrdesc = mass_ser_mkstrdesc;
}

int usbdev_mass_ser_initialize(void *handle)
{
  struct composite_devdesc_s dev[USB_COMPOSITE_DEV_NUM];
  int ret;

  usbdev_mass_initialize_sub(&dev[0], USB_MASS_FIRST_INTERFACE_NUM, DEV_MASS);
  mass_ser_get_composite_devdesc(&dev[0]);

  usbdev_acm_initialize_dev(&dev[1], USB_SERIAL_FIRST_INTERFACE_NUM, DEV_SERIAL);
  mass_ser_get_composite_devdesc(&dev[1]);

  ret = composite_initialize_softc(USB_COMPOSITE_DEV_NUM, dev, handle);
  if (ret < 0)
    {
      PRINTK("  ** mass storage serial device initialized failed! **\n");
      return -1;
    }

  PRINTK("  ** mass storage serial device initialized successfully! **\n");
  return 0;
}
