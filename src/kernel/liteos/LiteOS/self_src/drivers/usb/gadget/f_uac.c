/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2017-2023. All rights reserved.
 * Description: LiteOS USB Driver UAC Protocol
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

#include "gadget/f_uac_pri.h"
#include "gadget/usbd_audio_pri.h"
#include "implementation/usb_list.h"

static int usbclass_uac_bind(struct usbdevclass_driver_s *driver, struct usbdev_s *dev);
static int usbclass_uac_unbind(struct usbdevclass_driver_s *driver, struct usbdev_s *dev);
static int usbclass_uac_setup(struct usbdevclass_driver_s *driver, struct usbdev_s *dev,
                              const struct usb_ctrlreq_s *ctrl, uint8_t *dataout, size_t outlen);
static void usbclass_uac_disconnect(struct usbdevclass_driver_s *driver, struct usbdev_s *dev);

/* USB driver operations */

static const struct usbdevclass_driverops_s g_uac_driverops =
{
  usbclass_uac_bind,
  usbclass_uac_unbind,
  usbclass_uac_setup,
  usbclass_uac_disconnect,
  NULL,
  NULL
};

static const char g_fuac_str_lang[] =
{
  4, USB_DESC_TYPE_STRING,
  0x09, 0x04
};

static const char g_fuac_str_ac_if[] =
{
  UAC_AC_IF_STRDESC_LEN, USB_DESC_TYPE_STRING,
  'H', 0, 'i', 0, 's', 0, 'i', 0, 'l', 0, 'i', 0, 'c', 0, 'o', 0, 'n', 0,
  ' ', 0, 'H', 0, 'D', 0, ' ', 0, 'A', 0, 'u', 0, 'd', 0, 'i', 0, 'o', 0,
#ifndef UAC_VER_1_0
  '2', 0, '.', 0, '0', 0
#endif
};

static const char g_fuac_usb_in_it[] =
{
  46, USB_DESC_TYPE_STRING,
  'C', 0, 'a', 0, 'p', 0, 't', 0, 'u', 0, 'r', 0, 'e', 0, ' ', 0, 'I', 0,
  'n', 0, 'p', 0, 'u', 0, 't', 0, ' ', 0, 't', 0, 'e', 0, 'r', 0, 'm', 0,
  'i', 0, 'n', 0, 'a', 0, 'l', 0
};

static const char g_fuac_usb_in_ot[] =
{
  48, USB_DESC_TYPE_STRING,
  'U', 0, 'S', 0, 'B', 0, ' ', 0, 'I', 0, 'n', 0, ' ', 0, ' ', 0, 'O', 0,
  'u', 0, 't', 0, 'p', 0, 'u', 0, 't', 0, ' ', 0, 't', 0, 'e', 0, 'r', 0,
  'm', 0, 'i', 0, 'n', 0, 'a', 0, 'l', 0
};

static const char g_fuac_usb_out_it[] =
{
  48, USB_DESC_TYPE_STRING,
  'U', 0, 'S', 0, 'B', 0, ' ', 0, 'O', 0, 'u', 0, 't', 0, ' ', 0, ' ', 0,
  'I', 0, 'n', 0, 'p', 0, 'u', 0, 't', 0, ' ', 0, 't', 0, 'e', 0, 'r', 0,
  'm', 0, 'i', 0, 'n', 0, 'a', 0, 'l', 0
};

static const char g_fuac_usb_out_ot[] =
{
  48, USB_DESC_TYPE_STRING,
  'C', 0, 'a', 0, 'p', 0, 't', 0, 'u', 0, 'r', 0, 'e', 0, ' ', 0, 'O', 0,
  'u', 0, 't', 0, 'p', 0, 'u', 0, 't', 0, ' ', 0, 't', 0, 'e', 0, 'r', 0,
  'm', 0, 'i', 0, 'n', 0, 'a', 0, 'l', 0
};

size_t g_uac_iso_data_size = 16;
static bool g_uac_notification_init = false;

event_message *fuac_get_notification(void)
{
  if (!g_uac_notification_init)
    {
      usb_warn("notification event has not been initialized\n");
      return NULL;
    }

  struct uac_softc *fuac = get_uac();
  if (fuac == NULL)
    {
      usb_warn("usb device has not been initialized\n");
      return NULL;
    }
  struct uac_dev_s *dev = &fuac->dev;

  return &dev->notification;
}

static void fuac_write_notification_event(uac_event_type type, int32_t val)
{
  uint32_t ret = 0;
  event_message *message = fuac_get_notification();
  if (message == NULL)
    {
      return;
    }

  message->event_types |= type;
  switch (type)
    {
    case UAC_VOLUME_INC:
    case UAC_VOLUME_DEC:
        message->volume = val;
        break;
    case UAC_PSRATE_UPDATE:
        message->psrate = val;
        break;
    case UAC_CSRATE_UPDATE:
        message->csrate = val;
        break;
    case UAC_PSSIZE_UPDATE:
        message->pssize = val;
        break;
    default:
        break;
    }

  ret = LOS_EventWrite(&message->notification_event, UAC_NOTIFICATION_EVENT);
  if (ret != LOS_OK)
    {
      usb_warn("%d event written failed: 0x%x\n", type, ret);
    }
}

static uint8_t fuac_channel_num_get(int channel_mask)
{
  uint8_t num = 0;

  while (channel_mask > 0)
    {
      if (channel_mask & 1)
        {
          num++;
        }
      channel_mask >>= 1;
    }

  return num;
}

static int fuac_check_mask(int c_chmask, int p_chmask)
{
  uint8_t c_channel_num = fuac_channel_num_get(c_chmask);
  uint8_t p_channel_num = fuac_channel_num_get(p_chmask);
  if (c_channel_num > UAC_CHANNEL_NUM || c_channel_num == 0 ||
      p_channel_num > UAC_CHANNEL_NUM || p_channel_num == 0)
    {
      usb_err("c_chmask is invalid %d or p_chmask is invalid %d\n", c_chmask, p_chmask);
      return -UAC_ERROR_VALUE;
    }
   return UAC_OK;
}

#ifdef UAC_VER_2_0
static int g_uac_c_srate[UAC_FREQ_NUM] =
{
  UAC_SAMPLING_RATE_8K, UAC_SAMPLING_RATE_16K, UAC_SAMPLING_RATE_32K, UAC_SAMPLING_RATE_48K,
};  /* Capture sample rate */
static int g_uac_p_srate[UAC_FREQ_NUM] =
{
  UAC_SAMPLING_RATE_8K, UAC_SAMPLING_RATE_16K, UAC_SAMPLING_RATE_32K, UAC_SAMPLING_RATE_48K,
};  /* Playback sample rate. */
#endif

struct uac_opts g_uac_opts =
{
  UAC_DEF_CCHMASK, UAC_DEF_CSRATE, UAC_DEF_CSSIZE,
  UAC_DEF_PCHMASK, UAC_DEF_PSRATE, UAC_DEF_PSSIZE,
  UAC_FREQ_NUM, UAC_FREQ_NUM,
#ifdef LOSCFG_DRIVERS_USB_UAC_GADGET_FU
  UAC_DEFF_MUTE, UAC_DEFF_MIN_DB, UAC_DEFF_MAX_DB, UAC_DEF_RES_DB, UAC_DEFF_DB,
  UAC_DEFF_MUTE, UAC_DEFF_MIN_DB, UAC_DEFF_MAX_DB, UAC_DEF_RES_DB, UAC_DEFF_DB,
#endif
#if LOSCFG_DRIVERS_USB_UAC_DYNAMIC_SPEAKER
  UAC_GADGET_SPEAKER,
#endif
};

int fuac_opts_set(const struct uac_opts *opts)
{
  if (opts == NULL)
    {
      usb_err("opts is NULL\n");
      return -UAC_ERROR_PTR;
    }

  if (fuac_check_mask(opts->c_chmask, opts->p_chmask) != UAC_OK)
    {
      return -UAC_ERROR_VALUE;
    }

  if (opts->c_ssize > UAC_DEF_CSSIZE || opts->c_ssize <= 1 ||
      opts->p_ssize > UAC_DEF_PSSIZE || opts->p_ssize <= 1)
    {
      usb_err("c_ssize is invalid %d or p_ssize is invalid %d\n", opts->c_ssize, opts->p_ssize);
      return -UAC_ERROR_VALUE;
    }

  if (opts->c_srate < UAC_SAMPLING_RATE_8K || opts->c_srate > UAC_SAMPLING_RATE_48K)
    {
      usb_err("c_srate is invalid %d\n", opts->c_srate);
      return -UAC_ERROR_VALUE;
    }

  if (opts->p_srate < UAC_SAMPLING_RATE_8K || opts->p_srate > UAC_SAMPLING_RATE_48K)
    {
      usb_err("p_srate is invalid %d\n", opts->p_srate);
      return -UAC_ERROR_VALUE;
    }

#ifdef LOSCFG_DRIVERS_USB_UAC_GADGET_FU
  if (opts->c_volume_min >= opts->c_volume_max)
    {
      usb_err("c_volume_min is invalid %d or c_volume_max is invalid %d\n", opts->c_volume_min, opts->c_volume_max);
      return -UAC_ERROR_VALUE;
    }
  else if (opts->c_volume_curr < opts->c_volume_min || opts->c_volume_curr > opts->c_volume_max)
    {
      usb_err("c_volume_curr is invalid %d\n", opts->c_volume_curr);
      return -UAC_ERROR_VALUE;
    }
  else if (opts->c_volume_res <= 0 || (opts->c_volume_max - opts->c_volume_min) % opts->c_volume_res != 0)
    {
      usb_err("c_volume_res is invalid %d\n", opts->c_volume_res);
      return -UAC_ERROR_VALUE;
    }

  if (opts->p_volume_min >= opts->p_volume_max)
    {
      usb_err("p_volume_min is invalid %d or p_volume_max is invalid %d\n", opts->p_volume_min, opts->p_volume_max);
      return -UAC_ERROR_VALUE;
    }
  else if (opts->p_volume_curr < opts->p_volume_min || opts->p_volume_curr > opts->p_volume_max)
    {
      usb_err("p_volume_curr is invalid %d\n", opts->p_volume_curr);
      return -UAC_ERROR_VALUE;
    }
  else if (opts->p_volume_res <= 0 || (opts->p_volume_max - opts->p_volume_min) % opts->p_volume_res != 0)
    {
      usb_err("p_volume_res is invalid %d\n", opts->p_volume_res);
      return -UAC_ERROR_VALUE;
    }
#endif
  if (opts->c_freq_num == 0 || opts->c_freq_num > UAC_FREQ_NUM ||
      opts->p_freq_num == 0 || opts->p_freq_num > UAC_FREQ_NUM)
    {
      usb_err("format num is invalid, c_freq_num: %u, p_freq_num: %u\n", opts->c_freq_num, opts->p_freq_num);
      return -UAC_ERROR_VALUE;
    }
#ifdef LOSCFG_DRIVERS_USB_UAC_DYNAMIC_SPEAKER
  if (opts->speaker_num > 1)
    {
      usb_err("speaker_num is invalid %u\n", opts->speaker_num);
      return -UAC_ERROR_VALUE;
    }
#endif

  (void)memcpy_s(&g_uac_opts, sizeof(g_uac_opts), opts, sizeof(struct uac_opts));

  return 0;
}

int fuac_get_opts(struct uac_opts *opts)
{
  if (opts == NULL)
    {
      usb_err("opts is NULL\n");
      return -UAC_ERROR_PTR;
    }

  (void)memcpy_s(opts, sizeof(struct uac_opts), &g_uac_opts, sizeof(g_uac_opts));
  return 0;
}

int fuac_rate_get(void)
{
  return g_uac_opts.p_srate;
}

static struct uac_softc *g_fuac_softc = NULL;

struct uac_softc *get_uac(void)
{
  return g_fuac_softc;
}

#if UAC_GADGET_MICROPHONE
static void uac_set_playback_transfer_data_size(struct uac_dev_s *uac, int rate, int size, int chmask)
{
  uint32_t interval;

  spin_lock(&uac->lock);
  interval = UAC_FACTOR >> (g_fuac_as_in_ep_desc.bInterval - 1);
  g_uac_iso_data_size = DIV_ROUND_UP(rate, interval) * size * fuac_channel_num_get(chmask);
  spin_unlock(&uac->lock);
}
#else
static void uac_set_playback_transfer_data_size(struct uac_dev_s *uac, int rate, int size, int chmask)
{
  (void)uac;
  (void)rate;
  (void)size;
}
#endif

static struct usb_devdesc_s g_fuac_device_desc =
{
  .len          = sizeof(struct usb_devdesc_s),
  .type         = USB_DESC_TYPE_DEVICE, /* Constant for device descriptor */
  HSETW(.usb, USB_VERSION_BCD),         /* USB version required: 2.0 */
  .classid      = USB_CLASS_MISC,       /* Miscellaneous Device Class */
  .subclass     = 0x2,                  /* Common Class */
  .protocol     = 0x1,                  /* Interface Association Descriptor */
  .mxpacketsize = 64,                   /* Control Endpoint packet size */
  HSETW(.vendor,  0),                   /* Vendor ID */
  HSETW(.product, 0),                   /* Product ID */
  HSETW(.device,  0x318),               /* Device release code */
  .imfgr        = 1,                    /* Manufacturer name, string index */
  .iproduct     = 2,                    /* Product name, string index */
  .serno        = 3,                    /* Device serial number, string index */
  .nconfigs     = 1                     /* One Configuration */
};

static struct usb_cfgdesc_s g_fuac_config_desc =
{
  .len         = sizeof(struct usb_cfgdesc_s),
  .type        = USB_DESC_TYPE_CONFIG,
  HSETW(.totallen, 0),                     /* Size of all descriptors, set later */
  .ninterfaces = UAC_NINTERFACES,          /* Number of Interfaces */
  .cfgvalue    = 0x1,                      /* ID of this configuration */
  .icfg        = 0x0,                      /* Index of string descriptor */
  .attr        = 0xc0,                     /* Self-powered */
  .mxpower     = 0x1                       /* Maximum power consumption from the bus */
};

static struct usb_iaddesc_s g_fuac_iad =
{
  .len       = sizeof(struct usb_iaddesc_s),
  .type      = USB_DESC_TYPE_INTERFACEASSOCIATION,
  .firstif   = 0,               /* Interface number of VideoControl interface */
  .nifs      = UAC_NINTERFACES, /* Number of contiguous Video interfaces */
  .classid   = USB_CLASS_AUDIO,
  .subclass  = 0,
  .protocol  = UAC_VERSION,
  .ifunction = STR_AC_IF        /* Index of string descriptor */
};

static struct usb_ifdesc_s g_fuac_ac_intf_desc =
{
  .len      = sizeof(struct usb_ifdesc_s),
  .type     = USB_DESC_TYPE_INTERFACE,
  .ifno     = 0,         /* Index number of this interface */
  .alt      = 0,         /* Index of this settings */
  .neps     = 0,         /* One endpoint */
  .classid  = USB_CLASS_AUDIO,
  .subclass = UAC_AUDIOCONTROL,
  .protocol = UAC_VERSION,
  .iif      = STR_AC_IF  /* Index of string descriptor */
};

static struct uac_ac_header_descriptor g_fuac_ac_head_desc =
{
  .bLength            = sizeof(struct uac_ac_header_descriptor),
  .bDescriptorType    = USB_DESC_TYPE_CSINTERFACE,
  .bDescriptorSubtype = UAC_HEADER,
  .bcdADC             = UAC_SPEC_VERSION,       /* UAC specification version */
  .wTotalLength       = 0,                      /* total length, currently not set */
#ifdef UAC_VER_1_0
  .bIncollection      = UAC_GADGET_MICROPHONE + UAC_GADGET_SPEAKER, /* Number of streaming interfaces */
  .bIfnr              = {0x01}                  /* Associated Audio Streaming Interface */
#else
  .bCategory          = UAC2_AFC_IO_BOX,
  .bmControls         = 0,
#endif
};
#if UAC_GADGET_MICROPHONE
#ifndef UAC_VER_1_0
static const struct uac2_clock_source_descriptor g_fuac2_clk_src_in_desc =
{
  .bLength            = sizeof(struct uac2_clock_source_descriptor),
  .bDescriptorType    = USB_DESC_TYPE_CSINTERFACE,
  .bDescriptorSubtype = UAC2_CLOCK_SOURCE,
  .bClockID           = IN_CLK_ID,
  .bmAttributes       = UAC2_CS_TYPE_INT_PROG,
  .bmControls         = CONTROL_RDWR,
  .bAssocTerminal     = 0,
};
#endif

static struct uac_input_terminal_descriptor g_fuac_in_it_desc =
{
  .bLength            = sizeof(struct uac_input_terminal_descriptor),
  .bDescriptorType    = USB_DESC_TYPE_CSINTERFACE,
  .bDescriptorSubtype = UAC_INPUT_TERMINAL,
  .bTerminalID        = IN_IT_ID,
  .wTerminalType      = UAC_IN_MICROPHONE,
  .bAssocTerminal     = 0,
  .bNrChannels        = UAC_CHANNEL_NUM,
  .bmChannelConfig    = UAC_DEF_PCHMASK,
  .iChannelNames      = 0x10,
  .iTerminal          = UAC_USB_IN_IT,
#ifndef UAC_VER_1_0
  .bCSourceID         = IN_CLK_ID,
  .bmControls         = CONTROL_RDWR,
#endif
};

#ifdef LOSCFG_DRIVERS_USB_UAC_GADGET_FU
static struct uac_feature_unit_descriptor g_fuac_in_fu_desc =
{
  .bLength            = sizeof(struct uac_feature_unit_descriptor),
  .bDescriptorType    = USB_DESC_TYPE_CSINTERFACE,
  .bDescriptorSubtype = UAC_FEATURE_UNIT,
  .bUnitID            = IN_FU_ID,
  .bSourceID          = IN_IT_ID,
#ifdef UAC_VER_1_0
  .bControlSize       = 0x1,
#endif
  .bmaControls[0]     = UAC_FU_MUTE | UAC_FU_VOLUME,
  .iFeature           = 0,
};
#endif

static struct uac_output_terminal_descriptor g_fuac_in_ot_desc =
{
  .bLength            = sizeof(struct uac_output_terminal_descriptor),
  .bDescriptorType    = USB_DESC_TYPE_CSINTERFACE,
  .bDescriptorSubtype = UAC_OUTPUT_TERMINAL,
  .wTerminalType      = UAC_TERMINAL_STREAMING,
  .bAssocTerminal     = 0,
  .iTerminal          = UAC_USB_IN_OT,
  .bTerminalID        = IN_OT_ID,
  .bSourceID          = IN_IT_ID,
#ifndef UAC_VER_1_0
  .bCSourceID         = IN_CLK_ID,
  .bmControls         = CONTROL_RDWR,
#endif
};

static struct usb_ifdesc_s g_fuac_as_in_alt_0_desc =
{
  .len      = sizeof(struct usb_ifdesc_s),
  .type     = USB_DESC_TYPE_INTERFACE,
  .ifno     = 1,
  .alt      = 0,
  .neps     = 0,
  .classid  = USB_CLASS_AUDIO,
  .subclass = UAC_AUDIOSTREAMING,
  .protocol = UAC_VERSION,
  .iif      = 0,
};

static struct usb_ifdesc_s g_fuac_as_in_alt_1_desc =
{
  .len      = sizeof(struct usb_ifdesc_s),
  .type     = USB_DESC_TYPE_INTERFACE,
  .ifno     = 1,
  .alt      = 1,
  .neps     = 1,
  .classid  = USB_CLASS_AUDIO,
  .subclass = UAC_AUDIOSTREAMING,
  .protocol = UAC_VERSION,
  .iif      = 0,
};

static struct uac_as_header_descriptor g_fuac_as_in_head_desc =
{
  .bLength            = sizeof(struct uac_as_header_descriptor),
  .bDescriptorType    = USB_DESC_TYPE_CSINTERFACE,
  .bDescriptorSubtype = UAC_HEADER,
  .bTerminalLink      = IN_OT_ID,
#ifdef UAC_VER_1_0
  .bDelay             = 0,
  .wFormatTag         = 1,
#else
  .bNrChannels        = UAC_CHANNEL_NUM,
  .bmChannelConfig    = UAC_DEF_PCHMASK,
  .bmControls         = 0,
  .bFormatType        = UAC_FORMAT_TYPE_I,
  .bmFormats          = UAC_FORMAT_TYPE_I_PCM,
  .iChannelNames      = 0,
#endif
};

static struct uac_format_type_i_discrete_descriptor g_fuac_as_in_type_i_desc =
{
  .bLength            = sizeof(struct uac_format_type_i_discrete_descriptor),
  .bDescriptorType    = USB_DESC_TYPE_CSINTERFACE,
  .bDescriptorSubtype = UAC_FORMAT_TYPE,
  .bFormatType        = UAC_FORMAT_TYPE_I,
  .bBitResolution     = UAC_BIT_DEPTH,
#ifdef UAC_VER_1_0
  .bNrChannels        = UAC_CHANNEL_NUM,
  .bSubFrameSize      = UAC_BIT_DEPTH / 8,
  .bSamFreqType       = UAC_FREQ_NUM,
  HSETM(.tSamFreq[0], UAC_SAMPLING_RATE_8K),
  HSETM(.tSamFreq[1], UAC_SAMPLING_RATE_16K),
  HSETM(.tSamFreq[2], UAC_SAMPLING_RATE_32K),
  HSETM(.tSamFreq[3], UAC_SAMPLING_RATE_48K)
#else
  .bSubslotSize       = UAC_BIT_DEPTH / 8,
#endif
};

#ifdef UAC_HAS_ALT2
static struct usb_ifdesc_s g_fuac_as_in_alt_2_desc =
{
  .len      = sizeof(struct usb_ifdesc_s),
  .type     = USB_DESC_TYPE_INTERFACE,
  .ifno     = 1,
  .alt      = 2,
  .neps     = 1,
  .classid  = USB_CLASS_AUDIO,
  .subclass = UAC_AUDIOSTREAMING,
  .protocol = UAC_VERSION,
  .iif      = 0,
};

static struct uac_format_type_i_discrete_descriptor g_fuac_as_in_type_i_alt_2_desc =
{
  .bLength            = sizeof(struct uac_format_type_i_discrete_descriptor),
  .bDescriptorType    = USB_DESC_TYPE_CSINTERFACE,
  .bDescriptorSubtype = UAC_FORMAT_TYPE,
  .bFormatType        = UAC_FORMAT_TYPE_I,
  .bBitResolution     = UAC_ALT2_BIT_DEPTH,
#ifdef UAC_VER_1_0
  .bNrChannels        = UAC_CHANNEL_NUM,
  .bSubFrameSize      = UAC_ALT2_BIT_DEPTH / 8,
  .bSamFreqType       = UAC_FREQ_NUM,
  HSETM(.tSamFreq[0], UAC_SAMPLING_RATE_8K),
  HSETM(.tSamFreq[1], UAC_SAMPLING_RATE_16K),
  HSETM(.tSamFreq[2], UAC_SAMPLING_RATE_32K),
  HSETM(.tSamFreq[3], UAC_SAMPLING_RATE_48K)
#else
  .bSubslotSize       = UAC_ALT2_BIT_DEPTH / 8,
#endif
};
#endif

#ifdef UAC_HAS_ALT3
static struct usb_ifdesc_s g_fuac_as_in_alt_3_desc =
{
  .len      = sizeof(struct usb_ifdesc_s),
  .type     = USB_DESC_TYPE_INTERFACE,
  .ifno     = 1,
  .alt      = 3,
  .neps     = 1,
  .classid  = USB_CLASS_AUDIO,
  .subclass = UAC_AUDIOSTREAMING,
  .protocol = UAC_VERSION,
  .iif      = 0,
};

static struct uac_format_type_i_discrete_descriptor g_fuac_as_in_type_i_alt_3_desc =
{
  .bLength            = sizeof(struct uac_format_type_i_discrete_descriptor),
  .bDescriptorType    = USB_DESC_TYPE_CSINTERFACE,
  .bDescriptorSubtype = UAC_FORMAT_TYPE,
  .bFormatType        = UAC_FORMAT_TYPE_I,
  .bBitResolution     = UAC_ALT3_BIT_DEPTH,
#ifdef UAC_VER_1_0
  .bNrChannels        = UAC_CHANNEL_NUM,
  .bSubFrameSize      = UAC_ALT3_BIT_DEPTH / 8,
  .bSamFreqType       = UAC_FREQ_NUM,
  HSETM(.tSamFreq[0], UAC_SAMPLING_RATE_8K),
  HSETM(.tSamFreq[1], UAC_SAMPLING_RATE_16K),
  HSETM(.tSamFreq[2], UAC_SAMPLING_RATE_32K),
  HSETM(.tSamFreq[3], UAC_SAMPLING_RATE_48K)
#else
  .bSubslotSize       = UAC_ALT3_BIT_DEPTH / 8,
#endif
};
#endif

/* Standard ISO IN Endpoint Descriptor */

struct uac_endpoint_descriptor g_fuac_as_in_ep_desc =
{
  .bLength          = sizeof(struct uac_endpoint_descriptor),
  .bDescriptorType  = USB_DESC_TYPE_ENDPOINT,

  /* Hi3516ev200 platform needs to specify endpoint number, otherwise the camera audio works abnormally.
   * This way is compatible with other platforms.
   */

  .bEndpointAddress = USB_DIR_IN | 0x1,
  .bmAttributes     = UAC_EP_TRANSFER_TYPE,
  HSETW(.wMaxPacketSize, ISO_DATA_SIZE),
  .bInterval        = UAC_INTERVAL,
#ifdef UAC_VER_1_0
  .bRefresh         = 0,
  .bSynchAddress    = 0,
#endif
};

static const struct uac_iso_endpoint_descriptor g_fuac_as_iso_in_desc =
{
  .bLength            = sizeof(struct uac_iso_endpoint_descriptor),
  .bDescriptorType    = USB_DESC_TYPE_CSENDPOINT,
  .bDescriptorSubtype = UAC_EP_GENERAL,
  .bLockDelayUnits    = 0,
  .wLockDelay         = 0,
#ifdef UAC_VER_1_0
  .bmAttributes       = 1,
#else
  .bmAttributes       = 0,
  .bmControls         = 0,
#endif
};
#endif


#if UAC_GADGET_SPEAKER
/* USB out for speaker */
#ifndef UAC_VER_1_0
static const struct uac2_clock_source_descriptor g_fuac2_clk_src_out_desc =
{
  .bLength            = sizeof(struct uac2_clock_source_descriptor),
  .bDescriptorType    = USB_DESC_TYPE_CSINTERFACE,
  .bDescriptorSubtype = UAC2_CLOCK_SOURCE,
  .bClockID           = OUT_CLK_ID,
  .bmAttributes       = UAC2_CS_TYPE_INT_PROG,
  .bmControls         = CONTROL_RDWR,
  .bAssocTerminal     = 0,
};
#endif

static struct uac_input_terminal_descriptor g_fuac_out_it_desc =
{
  .bLength            = sizeof(struct uac_input_terminal_descriptor),
  .bDescriptorType    = USB_DESC_TYPE_CSINTERFACE,
  .bDescriptorSubtype = UAC_INPUT_TERMINAL,
  .wTerminalType      = UAC_TERMINAL_STREAMING,
  .bAssocTerminal     = 0,
  .bNrChannels        = UAC_CHANNEL_NUM,
  .bmChannelConfig    = UAC_DEF_CCHMASK,
  .iChannelNames      = 0,
  .iTerminal          = UAC_USB_OUT_IT,
  .bTerminalID        = OUT_IT_ID,
#ifndef UAC_VER_1_0
  .bCSourceID         = OUT_CLK_ID,
  .bmControls         = CONTROL_RDWR,
#endif
};

#ifdef LOSCFG_DRIVERS_USB_UAC_GADGET_FU
static struct uac_feature_unit_descriptor g_fuac_out_fu_desc =
{
  .bLength            = sizeof(struct uac_feature_unit_descriptor),
  .bDescriptorType    = USB_DESC_TYPE_CSINTERFACE,
  .bDescriptorSubtype = UAC_FEATURE_UNIT,
  .bUnitID            = OUT_FU_ID,
  .bSourceID          = OUT_IT_ID,
#ifdef UAC_VER_1_0
  .bControlSize       = 0x1,
#endif
  .bmaControls[0]     = UAC_FU_MUTE | UAC_FU_VOLUME,
  .iFeature           = 0,
};
#endif

static struct uac_output_terminal_descriptor g_fuac_out_ot_desc =
{
  .bLength            = sizeof(struct uac_output_terminal_descriptor),
  .bDescriptorType    = USB_DESC_TYPE_CSINTERFACE,
  .bDescriptorSubtype = UAC_OUTPUT_TERMINAL,
  .wTerminalType      = UAC_OUT_SPEAKER,
  .iTerminal          = UAC_USB_OUT_OT,
  .bSourceID          = OUT_IT_ID,
  .bAssocTerminal     = 0,
  .bTerminalID        = OUT_OT_ID,
#ifndef UAC_VER_1_0
  .bCSourceID         = OUT_CLK_ID,
  .bmControls         = CONTROL_RDWR,
#endif
};

static struct usb_ifdesc_s g_fuac_as_out_alt_0_desc =
{
  .len      = sizeof(struct usb_ifdesc_s),
  .type     = USB_DESC_TYPE_INTERFACE,
  .ifno     = 1,
  .alt      = 0,
  .neps     = 0,
  .classid  = USB_CLASS_AUDIO,
  .subclass = UAC_AUDIOSTREAMING,
  .protocol = UAC_VERSION,
  .iif      = 0,
};

static struct usb_ifdesc_s g_fuac_as_out_alt_1_desc =
{
  .len      = sizeof(struct usb_ifdesc_s),
  .type     = USB_DESC_TYPE_INTERFACE,
  .ifno     = 1,
  .alt      = 1,
  .neps     = 1,
  .classid  = USB_CLASS_AUDIO,
  .subclass = UAC_AUDIOSTREAMING,
  .protocol = UAC_VERSION,
  .iif      = 0,
};

static struct uac_as_header_descriptor g_fuac_as_out_head_desc =
{
  .bLength            = sizeof(struct uac_as_header_descriptor),
  .bDescriptorType    = USB_DESC_TYPE_CSINTERFACE,
  .bDescriptorSubtype = UAC_AS_GENERAL,
  .bTerminalLink      = OUT_IT_ID,
#ifdef UAC_VER_1_0
  .bDelay             = 0,
  .wFormatTag         = 1,
#else
  .bmControls         = 0,
  .bFormatType        = UAC_FORMAT_TYPE_I,
  .bmFormats          = UAC_FORMAT_TYPE_I_PCM,
  .bNrChannels        = UAC_CHANNEL_NUM,
  .bmChannelConfig    = UAC_DEF_CCHMASK,
  .iChannelNames      = 0,
#endif
};

static struct uac_format_type_i_discrete_descriptor g_fuac_as_out_type_i_desc =
{
  .bLength            = sizeof(struct uac_format_type_i_discrete_descriptor),
  .bDescriptorType    = USB_DESC_TYPE_CSINTERFACE,
  .bDescriptorSubtype = UAC_FORMAT_TYPE,
  .bFormatType        = UAC_FORMAT_TYPE_I,
  .bBitResolution     = UAC_BIT_DEPTH,
#ifdef UAC_VER_1_0
  .bNrChannels        = UAC_CHANNEL_NUM,
  .bSubFrameSize      = UAC_BIT_DEPTH / 8,
  .bSamFreqType       = UAC_FREQ_NUM,
  HSETM(.tSamFreq[0], UAC_SAMPLING_RATE_8K),
  HSETM(.tSamFreq[1], UAC_SAMPLING_RATE_16K),
  HSETM(.tSamFreq[2], UAC_SAMPLING_RATE_32K),
  HSETM(.tSamFreq[3], UAC_SAMPLING_RATE_48K),
#else
  .bSubslotSize       = UAC_BIT_DEPTH / 8,
#endif
};

/* Standard ISO OUT Endpoint Descriptor */

static struct uac_endpoint_descriptor g_fuac_as_out_ep_desc =
{
  .bLength            = sizeof(struct uac_endpoint_descriptor),
  .bDescriptorType    = USB_DESC_TYPE_ENDPOINT,
  .bEndpointAddress   = 0,
  .bmAttributes       = USB_EP_ATTR_XFER_ISOC | USB_EP_ATTR_SYNC,
  HSETW(.wMaxPacketSize, ISO_DATA_SIZE),
  .bInterval          = UAC_INTERVAL,
#ifdef UAC_VER_1_0
  .bRefresh           = 0,
  .bSynchAddress      = 0,
#endif
};

static const struct uac_iso_endpoint_descriptor g_fuac_as_iso_out_desc =
{
  .bLength            = sizeof(struct uac_iso_endpoint_descriptor),
  .bDescriptorType    = USB_DESC_TYPE_CSENDPOINT,
  .bDescriptorSubtype = UAC_EP_GENERAL,
  .bLockDelayUnits    = 0,
  .wLockDelay         = 0,
#ifdef UAC_VER_1_0
  .bmAttributes       = 1,
#else
  .bmAttributes       = 0,
  .bmControls         = 0,
#endif
};
#endif

static const uint8_t *g_fuac_ac_head1_desc_array[] =
{
  (const uint8_t *)&g_fuac_config_desc,
  (const uint8_t *)&g_fuac_iad,
  (const uint8_t *)&g_fuac_ac_intf_desc,
  (const uint8_t *)&g_fuac_ac_head_desc,
};

#if UAC_GADGET_SPEAKER
static const uint8_t *g_fuac_ac_out_desc_array[] =
{
#ifndef UAC_VER_1_0
  (const uint8_t *)&g_fuac2_clk_src_out_desc,
#endif
  (const uint8_t *)&g_fuac_out_it_desc,
#ifdef LOSCFG_DRIVERS_USB_UAC_GADGET_FU
  (const uint8_t *)&g_fuac_out_fu_desc,
#endif
  (const uint8_t *)&g_fuac_out_ot_desc,
};
#endif

#if UAC_GADGET_MICROPHONE
static const uint8_t *g_fuac_ac_in_desc_array[] =
{
#ifndef UAC_VER_1_0
  (const uint8_t *)&g_fuac2_clk_src_in_desc,
#endif
  (const uint8_t *)&g_fuac_in_it_desc,
#ifdef LOSCFG_DRIVERS_USB_UAC_GADGET_FU
  (const uint8_t *)&g_fuac_in_fu_desc,
#endif
  (const uint8_t *)&g_fuac_in_ot_desc,
};
#endif

#if UAC_GADGET_SPEAKER
static const uint8_t *g_fuac_as_out_desc_array[] =
{
  (const uint8_t *)&g_fuac_as_out_alt_0_desc,
  (const uint8_t *)&g_fuac_as_out_alt_1_desc,
  (const uint8_t *)&g_fuac_as_out_head_desc,
  (const uint8_t *)&g_fuac_as_out_type_i_desc,
  (const uint8_t *)&g_fuac_as_out_ep_desc,
  (const uint8_t *)&g_fuac_as_iso_out_desc,
};
#endif

#if UAC_GADGET_MICROPHONE
static const uint8_t *g_fuac_as_in_alt01_desc_array[] =
{
  (const uint8_t *)&g_fuac_as_in_alt_0_desc,
  (const uint8_t *)&g_fuac_as_in_alt_1_desc,
  (const uint8_t *)&g_fuac_as_in_head_desc,
  (const uint8_t *)&g_fuac_as_in_type_i_desc,
  (const uint8_t *)&g_fuac_as_in_ep_desc,
  (const uint8_t *)&g_fuac_as_iso_in_desc,
};

#ifdef UAC_HAS_ALT2
static const uint8_t *g_fuac_as_in_alt2_desc_array[] =
{
  (const uint8_t *)&g_fuac_as_in_alt_2_desc,
  (const uint8_t *)&g_fuac_as_in_head_desc,
  (const uint8_t *)&g_fuac_as_in_type_i_alt_2_desc,
  (const uint8_t *)&g_fuac_as_in_ep_desc,
  (const uint8_t *)&g_fuac_as_iso_in_desc,
};
#endif

#ifdef UAC_HAS_ALT3
static const uint8_t *g_fuac_as_in_alt3_desc_array[] =
{
  (const uint8_t *)&g_fuac_as_in_alt_3_desc,
  (const uint8_t *)&g_fuac_as_in_head_desc,
  (const uint8_t *)&g_fuac_as_in_type_i_alt_3_desc,
  (const uint8_t *)&g_fuac_as_in_ep_desc,
  (const uint8_t *)&g_fuac_as_iso_in_desc,
};
#endif
#endif

struct desc_array_s
{
  bool disable;
  uint8_t len;
  const uint8_t **start;
};

#define CREATE_DESC_ARRAY_S(name)                   \
static struct desc_array_s name##_s =               \
{                                                   \
  .disable = false,                                 \
  .len = sizeof(name) / sizeof((name)[0]),            \
  .start = (const uint8_t **)&(name),               \
}

CREATE_DESC_ARRAY_S(g_fuac_ac_head1_desc_array);
#if UAC_GADGET_SPEAKER
CREATE_DESC_ARRAY_S(g_fuac_ac_out_desc_array);
CREATE_DESC_ARRAY_S(g_fuac_as_out_desc_array);
#endif
#if UAC_GADGET_MICROPHONE
CREATE_DESC_ARRAY_S(g_fuac_ac_in_desc_array);
CREATE_DESC_ARRAY_S(g_fuac_as_in_alt01_desc_array);
#ifdef UAC_HAS_ALT2
CREATE_DESC_ARRAY_S(g_fuac_as_in_alt2_desc_array);
#endif
#ifdef UAC_HAS_ALT3
CREATE_DESC_ARRAY_S(g_fuac_as_in_alt3_desc_array);
#endif
#endif

/* Total descriptors
 * Note: The audio control descriptors must be placed before the audio stream descriptors.
 */
struct desc_array_s *g_fuac_descriptors[] =
{
    &g_fuac_ac_head1_desc_array_s,
#if UAC_GADGET_SPEAKER
    &g_fuac_ac_out_desc_array_s,
#endif
#if UAC_GADGET_MICROPHONE
    &g_fuac_ac_in_desc_array_s,
#endif
#if UAC_GADGET_SPEAKER
    &g_fuac_as_out_desc_array_s,
#endif
#if UAC_GADGET_MICROPHONE
    &g_fuac_as_in_alt01_desc_array_s,
#ifdef UAC_HAS_ALT2
    &g_fuac_as_in_alt2_desc_array_s,
#endif
#ifdef UAC_HAS_ALT3
    &g_fuac_as_in_alt3_desc_array_s,
#endif
#endif
  NULL
};

static uint8_t g_fuac_speaker_num = UAC_GADGET_SPEAKER;
static int g_fuac_p_ssize = UAC_DEF_PSSIZE;

uint8_t fuac_speaker_num_get(void)
{
  return g_fuac_speaker_num;
}

static uint16_t fuac_get_total_len(struct desc_array_s* entry)
{
  int i;
  uint16_t len = 0;
  if (entry->disable)
    {
      return 0;
    }
  uint8_t **desc_arr = (uint8_t **)entry->start;
  for (i = 0; i < entry->len; i++)
    {
      len += (uint16_t)(*desc_arr[i]);
    }
  return len;
}

#ifdef CONFIG_USBDEV_SUPPORT_CERTIFY
static uint8_t *link_fuac_descriptors(uint16_t *total_size, uint8_t speed, uint8_t type)
#else
static uint8_t *link_fuac_descriptors(uint16_t *total_size)
#endif
{
  int i, j;
  uint8_t *des;
  uint8_t *temp_des;
  uint16_t cs;
  errno_t err;
  uint16_t temp_des_len;
  uint16_t ds = 0;

#ifdef LOSCFG_DRIVERS_USB_UAC_DYNAMIC_SPEAKER
  bool spk_disable = (g_fuac_speaker_num == 0);
  g_fuac_ac_out_desc_array_s.disable = spk_disable;
  g_fuac_as_out_desc_array_s.disable = spk_disable;
#ifdef UAC_VER_1_0
  g_fuac_ac_head_desc.bIncollection = 1 + g_fuac_speaker_num;
  g_fuac_ac_head_desc.bLength = (uint8_t)(sizeof(struct uac_ac_header_descriptor) - 1 + g_fuac_speaker_num);
#endif
  g_fuac_iad.nifs = 2 + g_fuac_speaker_num;  /* 2 */
  g_fuac_config_desc.ninterfaces = 2 + g_fuac_speaker_num;  /* 2 */
#endif
  g_fuac_p_ssize = g_uac_opts.p_ssize;

#if UAC_GADGET_MICROPHONE
#if UAC_HAS_ALT3
  g_fuac_as_in_alt3_desc_array_s.disable = (g_fuac_p_ssize <= 3);
#endif
#if UAC_HAS_ALT2
  g_fuac_as_in_alt2_desc_array_s.disable = (g_fuac_p_ssize <= 2);
#endif
#endif

  /* Add the length of descriptors one by one */
  for (i = 0; g_fuac_descriptors[i]; i++)
    {
      ds += fuac_get_total_len(g_fuac_descriptors[i]);
    }

  *total_size = ds;

  temp_des_len = SKB_DATA_ALIGN(ds);
  des = memalign(USB_CACHE_ALIGN_SIZE, temp_des_len);
  if (des == NULL)
    {
      usb_err("System out of memory! Descriptors length: %u\n", ds);
      return NULL;
    }
  (void)memset_s(des, temp_des_len, 0, temp_des_len);

  temp_des = des;

  /* Configuration descriptor needs to have the full length of rest of descriptors */

  USB_SETW(g_fuac_config_desc.totallen, ds);

  /* Add the length of audio control descriptors */

  cs = (uint16_t)sizeof(g_fuac_ac_head_desc);
#if UAC_GADGET_SPEAKER
#ifdef LOSCFG_DRIVERS_USB_UAC_GADGET_FU
  g_fuac_out_ot_desc.bSourceID = g_fuac_out_fu_desc.bUnitID;
#endif
  cs += fuac_get_total_len(&g_fuac_ac_out_desc_array_s);
#endif

#if UAC_GADGET_MICROPHONE
#ifdef LOSCFG_DRIVERS_USB_UAC_GADGET_FU
  g_fuac_in_ot_desc.bSourceID = g_fuac_in_fu_desc.bUnitID;
#endif
  cs += fuac_get_total_len(&g_fuac_ac_in_desc_array_s);
#endif

  g_fuac_ac_head_desc.wTotalLength = cs;

  for (i = 0; g_fuac_descriptors[i]; i++)
    {
      if (g_fuac_descriptors[i]->disable)
        {
          continue;
        }
      uint8_t **desc_arr = (uint8_t **)g_fuac_descriptors[i]->start;
      for (j = 0; j < g_fuac_descriptors[i]->len; j++)
        {
          const uint8_t *des_src = desc_arr[j];
          uint8_t des_len = *des_src;
#ifdef CONFIG_USBDEV_SUPPORT_CERTIFY
          struct usb_desc_s *desc = (struct usb_desc_s *)des_src;
          if (desc->type == USB_DESC_TYPE_ENDPOINT && type == USB_DESC_TYPE_OTHERSPEEDCONFIG)
            {
              struct uac_endpoint_descriptor *uac_ep_desc = (struct uac_endpoint_descriptor *)des_src;
              USB_SETW(uac_ep_desc->wMaxPacketSize, USB_DWC_MAX_PACKET_SIZE - 1);
            }
          (void)speed;
#endif
          err = memcpy_s(temp_des, temp_des_len, des_src, des_len);
          if (err != EOK)
            {
              usb_err("memcpy fail!\n");
              free(des);
              return NULL;
            }
          temp_des += des_len;
          temp_des_len -= des_len;
        }
    }

  return des;
}

#if UAC_GADGET_SPEAKER

#define MAX_QUEUE_NUM   8

static void uac_free_out_queue(struct uac_dev_s *uac)
{
  struct uac_queue_node *node;

  while (!LOS_ListEmpty(&uac->out_queue))
    {
      node = LOS_LIST_FIRST_ENTRY(&uac->out_queue, struct uac_queue_node, irqqueue);
      LOS_ListDelInit(&node->irqqueue);
      uac_queue_node_free(node);
    }
  uac->out_queue_num = 0;
}

static void configure_out_endpoint(struct uac_dev_s *uac)
{
  struct usbdev_req_s *req = uac->outputreq;

  if (uac->out_ep_enabled == true)
    {
      uac->out_ep_enabled = false;
      EP_DISABLE(uac->out_ep);
    }
  (void)EP_CONFIGURE(uac->out_ep, (const struct usb_epdesc_s *)&g_fuac_as_out_ep_desc, false);
  uac->out_ep_enabled = true;

  spin_lock(&uac->lock);
  uac_free_out_queue(uac);
  spin_unlock(&uac->lock);

  (void)EP_SUBMIT(uac->out_ep, req);
}

static void uac_receive_data(struct uac_dev_s *uac, const void *buf, int len)
{
  struct uac_queue_node *node;

  if (!uac->out_start)
    {
      return;
    }

  node = uac_queue_node_alloc(len);
  if (node == NULL)
    {
      usb_err("UAC_ERROR_MEMORY\n");
      return;
    }

  (void)memcpy_s(node->buf, len, buf, len);
  if (uac->out_queue_num > MAX_QUEUE_NUM)
    {
      uac_free_out_queue(uac);
    }

  LIST_ADD_TAIL(&node->irqqueue, &uac->out_queue);
  uac->out_queue_num++;
}

static void fuac_output_request_complete(struct usbdev_ep_s *ep,
                                         struct usbdev_req_s *req)
{
  struct uac_dev_s *uac = (struct uac_dev_s *)req->priv;
  uint32_t ret;

  (void)ep;

  if (req->result != 0)
    {
      return;
    }
  spin_lock(&uac->lock);
  if (req->xfrd > 0 && req->xfrd < ISO_DATA_SIZE)
    {
      uac_receive_data(uac, req->buf, req->xfrd);
      ret = LOS_EventWrite(&uac->out_event, UAC_READ_EVENT);
      if (ret != LOS_OK)
        {
          usb_err("write event failed :%u\n", ret);
        }
    }
  spin_unlock(&uac->lock);
  (void)EP_SUBMIT(uac->out_ep, req);
}
#endif

static void fuac_request_sampling_rate_complete(struct usbdev_ep_s *ep, struct usbdev_req_s *req);

#ifdef LOSCFG_DRIVERS_USB_UAC_GADGET_FU
static void fuac_request_cur_complete(struct usbdev_ep_s *ep, struct usbdev_req_s *req)
{
  struct uac_dev_s *uac = (struct uac_dev_s *)req->priv;
  uint8_t entity_id = (uint8_t)uac->dir;
  uint8_t *mute;
  int16_t val;
  int16_t last_val;
  (void)ep;

  switch (uac->control)
    {
    case UAC_FU_MUTE_CONTROL:
      mute = (entity_id == IN_FU_ID) ? (uint8_t *)&g_uac_opts.p_mute : (uint8_t *)&g_uac_opts.c_mute;

      *mute = *(uint8_t *)req->buf;
      fuac_write_notification_event(UAC_MUTE, INVALID);
      break;

    case UAC_FU_VOLUME_CONTROL:
      val = USB_GETW(req->buf);
      if (entity_id == IN_FU_ID)
        {
          last_val = g_uac_opts.p_volume_curr;
          g_uac_opts.p_volume_curr = CLAMP(val, g_uac_opts.p_volume_min, g_uac_opts.p_volume_max);
          val = g_uac_opts.p_volume_curr;
        }
      else
        {
          last_val = g_uac_opts.c_volume_curr;
          g_uac_opts.c_volume_curr = CLAMP(val, g_uac_opts.c_volume_min, g_uac_opts.c_volume_max);
          val = g_uac_opts.c_volume_curr;
        }

      if (last_val < val)
        {
          fuac_write_notification_event(UAC_VOLUME_INC, val);
        }
      else
        {
          fuac_write_notification_event(UAC_VOLUME_DEC, val);
        }
      break;

    default:
      break;
    }
  uac->control = ~0;
  uac->dir = UAC_DIR_INVALID;
}
#endif

static void uac_set_sampling_rate(struct uac_dev_s *uac, uint32_t rate)
{
  if (uac->dir == USB_DIR_IN && g_uac_opts.p_srate != rate)
    {
      g_uac_opts.p_srate = rate;
      uac_set_playback_transfer_data_size(uac, g_uac_opts.p_srate, g_uac_opts.p_ssize, g_uac_opts.p_chmask);
      fuac_write_notification_event(UAC_PSRATE_UPDATE, (int)rate);
      PRINTK("<<<in ep rate:%d, size:%u>>>\n", g_uac_opts.p_srate, g_uac_iso_data_size);
    }
  else if (uac->dir == USB_DIR_OUT && (g_uac_opts.c_srate != rate || uac->out_connected == 0))
    {
#ifdef LOSCFG_DRIVERS_USB_UAC_DYNAMIC_SPEAKER
      if (g_fuac_speaker_num == 0)
        {
          uac->dir = UAC_DIR_INVALID;
          return;
        }
#endif
      g_uac_opts.c_srate = rate;
      fuac_write_notification_event(UAC_CSRATE_UPDATE, (int)rate);
      PRINTK("<<<out ep rate:%d>>>\n", g_uac_opts.c_srate);
    }
  uac->dir = UAC_DIR_INVALID;
}

#ifdef UAC_VER_1_0
static uint32_t uac_get_rate_range(uint8_t *buf, int dir)
{
  uint32_t ret = 0;

  if (dir == USB_DIR_IN)
    {
      buf[0] = (uint8_t)g_uac_opts.p_srate;
      buf[1] = (uint8_t)(g_uac_opts.p_srate >> 8);
      buf[2] = (uint8_t)(g_uac_opts.p_srate >> 16);
      ret = UAC_RATE_BUF_LEN;
    }
  else if (dir == USB_DIR_OUT)
    {
      buf[0] = (uint8_t)g_uac_opts.c_srate;
      buf[1] = (uint8_t)(g_uac_opts.c_srate >> 8);
      buf[2] = (uint8_t)(g_uac_opts.c_srate >> 16);
      ret = UAC_RATE_BUF_LEN;
    }

  return ret;
}

static void uac_set_endpoint_req(struct usbdev_s *dev, struct usbdev_req_s *req,
                                 const struct usb_ctrlreq_s *ctrl)
{
  struct uac_dev_s *uac = (struct uac_dev_s *)req->priv;
  uint8_t request = ctrl->req;

  uac->dir = UAC_DIR_INVALID;
  switch (request)
    {
    case SET_CUR_UAC:
    case SET_MIN_UAC:
    case SET_MAX_UAC:
    case SET_RES_UAC:
      uac->dir = USB_GETW(ctrl->index) & UAC_DIR_MASK;
      req->len = MIN(USB_GETW(ctrl->len), USB_COMP_EP0_BUFSIZ);
      (void)EP_SUBMIT(dev->ep0, req);
      break;

    default:
      usb_warn("Unknown request: %#x\n", request);
      break;
    }
}

static void uac_get_endpoint_req(struct usbdev_s *dev, struct usbdev_req_s *req,
                                   const struct usb_ctrlreq_s *ctrl)
{
  uint8_t request = ctrl->req;
  uint8_t *buf = (uint8_t *)req->buf;

  switch (request)
    {
    case GET_CUR_UAC:
    case GET_MIN_UAC:
    case GET_MAX_UAC:
    case GET_RES_UAC:
      req->len = uac_get_rate_range(buf, USB_GETW(ctrl->index) & UAC_DIR_MASK);
      (void)EP_SUBMIT(dev->ep0, req);
      break;

    case GET_MEM_UAC:
      break;

    default:
      usb_warn("Unknown request: %#x\n", request);
      break;
    }
}

#ifdef LOSCFG_DRIVERS_USB_UAC_GADGET_FU
static int uac_set_interface_req(struct usbdev_s *dev, struct usbdev_req_s *req,
                                 const struct usb_ctrlreq_s *ctrl)
{
  struct uac_dev_s *uac = (struct uac_dev_s *)req->priv;
  uint8_t request = ctrl->req;
  uint8_t entity_id = ctrl->index[1];
  int ret = -1;

  if (entity_id != IN_FU_ID
#if UAC_GADGET_SPEAKER
#ifdef LOSCFG_DRIVERS_USB_UAC_DYNAMIC_SPEAKER
      && g_fuac_speaker_num > 0
#endif
      && entity_id != OUT_FU_ID
#endif
     )
    {
      usb_err("Unknown entity id: %u\n", entity_id);
      return ret;
    }

  switch (request)
    {
    case SET_CUR_UAC:
      uac->dir      = entity_id;
      uac->control  = ctrl->value[1];
      req->callback = fuac_request_cur_complete;
      req->priv     = uac;
      req->len      = MIN(USB_GETW(ctrl->len), USB_COMP_EP0_BUFSIZ);
      (void)EP_SUBMIT(dev->ep0, req);
      ret = 0;
      break;

    default:
      usb_warn("Unknown request: %#x\n", request);
      break;
    }
    return ret;
}

static int uac_fu_req_get_value(struct usbdev_s *dev, struct usbdev_req_s *req,
                                const struct usb_ctrlreq_s *ctrl, int16_t *volume_buf)
{
  uint8_t *buf = (uint8_t *)req->buf;
  uint8_t control_selector = ctrl->value[1];
  errno_t err;
  uint32_t buf_len = MIN(USB_GETW(ctrl->len), USB_COMP_EP0_BUFSIZ);

  if (control_selector != UAC_FU_VOLUME_CONTROL)
    {
      usb_err("Unknown control selector: %u\n", control_selector);
      return -1;
    }

  buf_len = MIN(buf_len, sizeof(int16_t));
  err = memcpy_s(buf, USB_COMP_EP0_BUFSIZ, volume_buf, buf_len);
  if (err != EOK)
    {
      usb_err("memcpy_s fail!, err:%d\n", err);
      return -1;
    }
  req->len = buf_len;
  (void)EP_SUBMIT(dev->ep0, req);

  return 0;
}

static int uac_get_interface_req(struct usbdev_s *dev, struct usbdev_req_s *req,
                                 const struct usb_ctrlreq_s *ctrl)
{
  uint8_t request = ctrl->req;
  uint8_t *buf = (uint8_t *)req->buf;
  uint8_t entity_id = ctrl->index[1];
  int16_t *volume_buf;
  int ret = -1;

  if (entity_id != IN_FU_ID
#if UAC_GADGET_SPEAKER
#ifdef LOSCFG_DRIVERS_USB_UAC_DYNAMIC_SPEAKER
      && g_fuac_speaker_num > 0
#endif
      && entity_id != OUT_FU_ID
#endif
     )
    {
      return ret;
    }

  switch (request)
    {
    case GET_CUR_UAC:
      if (ctrl->value[1] == UAC_FU_MUTE_CONTROL)
        {
          buf[0] = (entity_id == IN_FU_ID) ? g_uac_opts.p_mute : g_uac_opts.c_mute;
          req->len = 1;
          (void)EP_SUBMIT(dev->ep0, req);
          ret = 0;
        }
      else
        {
          volume_buf = (entity_id == IN_FU_ID) ? &g_uac_opts.p_volume_curr : &g_uac_opts.c_volume_curr;
          ret = uac_fu_req_get_value(dev, req, ctrl, volume_buf);
        }
      break;

    case GET_MIN_UAC:
      volume_buf = (entity_id == IN_FU_ID) ? &g_uac_opts.p_volume_min : &g_uac_opts.c_volume_min;
      ret = uac_fu_req_get_value(dev, req, ctrl, volume_buf);
      break;

    case GET_MAX_UAC:
      volume_buf = (entity_id == IN_FU_ID) ? &g_uac_opts.p_volume_max : &g_uac_opts.c_volume_max;
      ret = uac_fu_req_get_value(dev, req, ctrl, volume_buf);
      break;

    case GET_RES_UAC:
      volume_buf = (entity_id == IN_FU_ID) ? &g_uac_opts.p_volume_res : &g_uac_opts.c_volume_res;
      ret = uac_fu_req_get_value(dev, req, ctrl, volume_buf);
      break;

    default:
      break;
    }

    return ret;
}
#endif /* LOSCFG_DRIVERS_USB_UAC_GADGET_FU */

#else  /* UAC_VER_2_0 */

static uint32_t uac2_get_rate_range(uint8_t *buf, uint32_t buf_len, uint8_t clock_id)
{
  errno_t ret;
  uint32_t len;
  uint32_t freq_num;
  int *srates;
  struct ctl_ranges_lay3 rs = {0};
  uint32_t range_len = sizeof(struct ctl_ranges_lay3) - sizeof(struct ctl_range_lay3) * UAC_FREQ_NUM;
  uint8_t channel_num;

  if (clock_id == IN_CLK_ID)
    {
      channel_num = fuac_channel_num_get(g_uac_opts.p_chmask);
      srates = g_uac_p_srate;
      freq_num = g_uac_opts.p_freq_num;
    }
  else if (clock_id == OUT_CLK_ID)
    {
      channel_num = fuac_channel_num_get(g_uac_opts.c_chmask);
      srates = g_uac_c_srate;
      freq_num = g_uac_opts.c_freq_num;
    }
  else
   {
      return 0;
   }

  /* When the number of channels is greater than or equal to 4,
   * the Windows processing logic becomes confusing:
   * the sampling rate within the supported range cannot be selected.
   * To avoid this, only one sampling rate range is set.
   */
  if (channel_num >= 4 || (clock_id == OUT_CLK_ID))
    {
      USB_SETDW(rs.range[0].dMIN, srates[freq_num - 1]);
      USB_SETDW(rs.range[0].dMAX, srates[freq_num - 1]);
      range_len += sizeof(struct ctl_range_lay3);
      USB_SETW(rs.wNumSubRanges, 1);
    }
  else
    {
      for (int i = 0; i < freq_num; i++)
        {
          USB_SETDW(rs.range[i].dMIN, srates[i]);
          USB_SETDW(rs.range[i].dMAX, srates[i]);
        }
      USB_SETW(rs.wNumSubRanges, freq_num);
      range_len += sizeof(struct ctl_range_lay3) * freq_num;
    }

  len = MIN(buf_len, range_len);
  ret = memcpy_s(buf, buf_len, &rs, len);
  if (ret != EOK)
    {
      usb_err("memcpy_s fail!, ret:%d\n", ret);
      return 0;
    }
  return len;
}

static uint32_t uac2_get_rate_curr(uint8_t *buf, uint32_t buf_len, uint8_t entity_id)
{
  errno_t ret;
  uint32_t len;
  uint32_t curr = 0;

  if (entity_id == IN_CLK_ID)
    {
      curr = g_uac_opts.p_srate;
    }
  else if (entity_id == OUT_CLK_ID)
    {
      curr = g_uac_opts.c_srate;
    }

  len = MIN(buf_len, sizeof(curr));
  ret = memcpy_s(buf, buf_len, &curr, len);
  if (ret != EOK)
    {
      usb_err("memcpy_s fail!, ret:%d\n", ret);
      return 0;
    }
  return len;
}

#ifdef LOSCFG_DRIVERS_USB_UAC_GADGET_FU
static uint32_t uac2_get_fu_range(uint8_t *buf, uint32_t buf_len, uint8_t entity_id)
{
  errno_t ret;
  uint32_t len;

  struct ctl_ranges_lay2 rs = {0};

  if (entity_id == IN_FU_ID)
    {
      USB_SETW(rs.range[0].dMIN, g_uac_opts.p_volume_min);
      USB_SETW(rs.range[0].dMAX, g_uac_opts.p_volume_max);
      USB_SETW(rs.range[0].dRES, g_uac_opts.p_volume_res);
    }
  else
    {
      USB_SETW(rs.range[0].dMIN, g_uac_opts.c_volume_min);
      USB_SETW(rs.range[0].dMAX, g_uac_opts.c_volume_max);
      USB_SETW(rs.range[0].dRES, g_uac_opts.c_volume_res);
    }
  USB_SETW(rs.wNumSubRanges, 1);
  len = MIN(buf_len, sizeof(rs));
  ret = memcpy_s(buf, buf_len, &rs, len);
  if (ret != EOK)
    {
      usb_err("memcpy_s fail!, ret:%d\n", ret);
      return 0;
    }
  return len;
}

static uint32_t uac2_get_fu_curr(uint8_t *buf, uint32_t buf_len, uint8_t value, uint8_t entity_id)
{
  errno_t ret;
  uint32_t len;
  uint32_t curr = 0;

  if (entity_id == IN_FU_ID)
    {
      if (value == UAC_FU_MUTE_CONTROL)
        {
          curr = g_uac_opts.p_mute;
        }
      else if (value == UAC_FU_VOLUME_CONTROL)
        {
          curr = g_uac_opts.p_volume_curr;
        }
    }
  else
    {
      if (value == UAC_FU_MUTE_CONTROL)
        {
          curr = g_uac_opts.c_mute;
        }
      else if (value == UAC_FU_VOLUME_CONTROL)
        {
          curr = g_uac_opts.c_volume_curr;
        }
    }
  len = MIN(buf_len, sizeof(curr));
  ret = memcpy_s(buf, buf_len, &curr, len);
  if (ret != EOK)
    {
      usb_err("memcpy_s fail!, ret:%d\n", ret);
      return 0;
    }
  return len;
}
#endif

static int uac_set_interface_req(struct usbdev_s *dev, struct usbdev_req_s *req,
                                  const struct usb_ctrlreq_s *ctrl)
{
  struct uac_dev_s *uac = (struct uac_dev_s *)req->priv;
  uint8_t request = ctrl->req;
  uint8_t entity_id = ctrl->index[1];
  int ret = -1;

  switch (request)
    {
    case UAC2_CS_CUR:
      if (entity_id == OUT_CLK_ID || entity_id == IN_CLK_ID)
        {
          if (ctrl->value[1] == UAC2_CS_CONTROL_SAM_FREQ)
            {
              uac->dir = entity_id == IN_CLK_ID ? USB_DIR_IN : USB_DIR_OUT;
              uac->control  = UAC_SET_SAMPLING_RATE;
              req->len = MIN(USB_GETW(ctrl->len), USB_COMP_EP0_BUFSIZ);
              req->callback = fuac_request_sampling_rate_complete;
              (void)EP_SUBMIT(dev->ep0, req);
              ret = 0;
            }
        }
#ifdef LOSCFG_DRIVERS_USB_UAC_GADGET_FU
      else if (entity_id == OUT_FU_ID || entity_id == IN_FU_ID)
        {
          uac->dir      = entity_id;
          uac->control  = ctrl->value[1];
          req->callback = fuac_request_cur_complete;
          req->priv     = uac;
          req->len      = MIN(USB_GETW(ctrl->len), USB_COMP_EP0_BUFSIZ);
          (void)EP_SUBMIT(dev->ep0, req);
          ret = 0;
        }
#endif
      break;

    default:
      usb_warn("Unknown request: %#x\n", request);
      break;
    }
  return ret;
}

static int uac_get_interface_req(struct usbdev_s *dev, struct usbdev_req_s *req,
                                  const struct usb_ctrlreq_s *ctrl)
{
  uint8_t request = ctrl->req;
  uint8_t *buf = (uint8_t *)req->buf;
  uint32_t buf_len = MIN(USB_GETW(ctrl->len), USB_COMP_EP0_BUFSIZ);
  uint8_t entity_id = ctrl->index[1];
  int ret = 0;

  if (entity_id == IN_CLK_ID || entity_id == OUT_CLK_ID)
  {
  switch (request)
    {
    case UAC2_CS_CUR:
      if (ctrl->value[1] == UAC2_CS_CONTROL_SAM_FREQ)
        {
          req->len = uac2_get_rate_curr(buf, buf_len, entity_id);
          (void)EP_SUBMIT(dev->ep0, req);
        }
      break;
    case UAC2_CS_RANGE:
      if (ctrl->value[1] == UAC2_CS_CONTROL_SAM_FREQ)
        {
          req->len = uac2_get_rate_range(buf, buf_len, entity_id);
          (void)EP_SUBMIT(dev->ep0, req);
        }
      break;

    default:
      ret = -1;
      usb_warn("Unknown request: %#x\n", request);
      break;
    }
  }
#ifdef LOSCFG_DRIVERS_USB_UAC_GADGET_FU
  else if (entity_id == IN_FU_ID || entity_id == OUT_FU_ID)
    {
      switch (request)
        {
          case UAC2_CS_CUR:
            req->len = uac2_get_fu_curr(buf, buf_len, ctrl->value[1], entity_id);
            (void)EP_SUBMIT(dev->ep0, req);
            break;
          case UAC2_CS_RANGE:
            req->len = uac2_get_fu_range(buf, buf_len, entity_id);
            (void)EP_SUBMIT(dev->ep0, req);
            break;
          default:
            ret = -1;
            usb_warn("Unknown request: %#x\n", request);
            break;
    }
  }
#endif
  return ret;
}
#endif

static void uac_free_req(struct usbdev_ep_s *ep, struct usbdev_req_s *req, uint8_t is_free_buf)
{
  if (is_free_buf)
    {
      EP_FREEBUFFER(ep, req->buf);
    }

  EP_FREEREQ(ep, req);
}

static struct usbdev_req_s *uac_alloc_req(struct usbdev_ep_s *ep, uint16_t len)
{
  struct usbdev_req_s *req;

  req = EP_ALLOCREQ(ep);
  if (req == NULL)
    {
      return NULL;
    }

  if (len == 0)
    {
      return req;
    }

  req->len = len;
  req->buf = EP_ALLOCBUFFER(ep, len);
  if (req->buf == NULL)
    {
      EP_FREEREQ(ep, req);
      req = NULL;
    }

  return req;
}

static void fuac_request_sampling_rate_complete(struct usbdev_ep_s *ep, struct usbdev_req_s *req)
{
  struct uac_dev_s *uac = (struct uac_dev_s *)req->priv;
  uint8_t *buf = (uint8_t *)req->buf;
  (void)ep;

  switch (uac->control)
    {
    case UAC_SET_SAMPLING_RATE:
      uac_set_sampling_rate(uac, UAC_GETRATE(buf));
      break;

    default:
      break;
    }
  uac->control = ~0;
}

#if UAC_GADGET_MICROPHONE
void fuac_input_req_complete(struct usbdev_ep_s *ep, struct usbdev_req_s *req)
{
  struct uac_dev_s *uac = (struct uac_dev_s *)req->priv;
  uint32_t head = uac->data_buf_head;
  uint32_t head_next;

  if (LOS_AtomicRead(&uac->busy_flag) == 0)
    {
      (void)LOS_EventWrite(&uac->in_complete_event, UAC_COMPLETE_EVENT);
      return;
    }

  spin_lock(&uac->lock);

  if (uac->data_buf_len != uac->data_offset)
    {
      uac_send_data_sub(uac);
      spin_unlock(&uac->lock);
      return;
    }

  head_next = (head + 1) % uac->data_buf_count;
  if (head_next != uac->data_buf_tail && uac->data_buf_flag[head_next] == UAC_BUF_USE)
    {
      uac->data_offset = 0;
      uac->data_buf_flag[head] = UAC_BUF_IDLE;
      uac->data_buf_head = head_next;
    }

  uac_send_data_sub(uac);
  spin_unlock(&uac->lock);
}
#endif

static void fuac_source_free(const struct usbdevclass_driver_s *driver, struct usbdev_s *dev)
{
  const struct uac_driver_s *drvr;
  struct uac_dev_s *uac;

  drvr = (const struct uac_driver_s *)driver;
  uac  = drvr->dev;
  if (uac == NULL)
    {
      return;
    }

#if UAC_GADGET_SPEAKER
#ifdef LOSCFG_DRIVERS_USB_UAC_DYNAMIC_SPEAKER
  if (g_fuac_speaker_num > 0)
#endif
    {
      if (uac->outputreq != NULL)
        {
          uac_free_req(uac->out_ep, uac->outputreq, 1);
          uac->outputreq = NULL;
        }
      if (uac->out_ep != NULL)
        {
          DEV_FREEEP(dev, uac->out_ep);
          uac->out_ep = NULL;
        }
      (void)LOS_EventDestroy(&uac->out_event);
    }
#endif
#if UAC_GADGET_MICROPHONE
  if (uac->inputreq != NULL)
    {
      uac_free_req(uac->in_ep, uac->inputreq, 0);
      uac->inputreq = NULL;
    }
  if (uac->in_ep != NULL)
    {
      DEV_FREEEP(dev, uac->in_ep);
      uac->in_ep = NULL;
    }

  (void)LOS_EventDestroy(&uac->in_complete_event);
#endif

  event_message *message = &uac->notification;
  message->pssize = INVALID;
  message->psrate = INVALID;
  message->csrate = INVALID;
  message->volume = INVALID;
  (void)LOS_EventDestroy(&message->notification_event);
  g_uac_notification_init = false;
}

static int usbclass_uac_bind(struct usbdevclass_driver_s *driver, struct usbdev_s *dev)
{
  struct usbdev_ep_s *ep;
  struct uac_driver_s *drvr;
  struct composite_dev_s *cdev;
  struct uac_dev_s *uac;
  struct composite_devobj_s *devobj;
  struct usbdev_devinfo_s *devinfo;
  uint32_t ret = 0;
  event_message *message = NULL;

  if (driver == NULL || dev == NULL)
    {
      return -1;
    }

  cdev = dev->ep0->priv;
  if (cdev == NULL)
    {
      return -1;
    }

  drvr = (struct uac_driver_s *)driver;
  uac  = drvr->dev;
  if (uac == NULL)
    {
      return -1;
    }

  uac->busy_flag     = 0;
  uac->dir           = UAC_DIR_INVALID;
  spin_lock_init(&uac->lock);

  devobj = usbclass_devobj_get(cdev, DEV_UAC);
  if (devobj == NULL)
    {
      return -1;
    }
  devinfo = &devobj->compdesc.devinfo;

#if UAC_GADGET_SPEAKER
#ifdef LOSCFG_DRIVERS_USB_UAC_DYNAMIC_SPEAKER
  if (g_fuac_speaker_num == 0)
    {
      goto init_epin;
    }
#endif
  /* Initialize AudioStreaming OUT endpoint */

  LOS_ListInit(&uac->out_queue);
  (void)LOS_EventInit(&uac->out_event);
  uac->out_start     = false;
  uac->out_queue_num = 0;

  ep = DEV_ALLOCEP(dev, g_fuac_as_out_ep_desc.bEndpointAddress,
                   (struct usb_epdesc_s *)&g_fuac_as_out_ep_desc);
  if (ep == NULL)
    {
      (void)LOS_EventDestroy(&uac->out_event);
      return -1;
    }
  ep->priv    = (void *)uac;
  uac->out_ep = ep;

  uac->outputreq = uac_alloc_req(uac->out_ep, ISO_DATA_SIZE);
  if (uac->outputreq == NULL)
    {
      usb_err("Malloc output req failed!\n");
      goto fail;
    }
  uac->outputreq->priv     = (void *)uac;
  uac->outputreq->callback = fuac_output_request_complete;
  ep->handle_req           = uac->outputreq;
  devinfo->epno[0]         = ep->eplog;
#ifdef LOSCFG_DRIVERS_USB_UAC_DYNAMIC_SPEAKER
init_epin:
#endif
#endif

#if UAC_GADGET_MICROPHONE
  /* Initialize AudioStreaming IN endpoint */

  ep = DEV_ALLOCEP(dev, g_fuac_as_in_ep_desc.bEndpointAddress,
                   (struct usb_epdesc_s *)&g_fuac_as_in_ep_desc);
  if (ep == NULL)
    {
      goto fail;
    }

  uac->inputreq = uac_alloc_req(ep, 0);
  if (uac->inputreq == NULL)
    {
      usb_err("Malloc input req failed!\n");
      goto fail;
    }
  uac->inputreq->callback           = fuac_input_req_complete;
  uac->inputreq->priv               = (void *)uac;
  ep->priv                          = (void *)uac;
  ep->handle_req                    = uac->inputreq;
  uac->in_ep                        = ep;
  devinfo->epno[g_fuac_speaker_num] = ep->eplog;
  uac->control                      = ~0;

  (void)LOS_EventInit(&uac->in_complete_event);
#endif

  message = &uac->notification;
  message->pssize = INVALID;
  message->psrate = INVALID;
  message->csrate = INVALID;
  message->volume = INVALID;
  /* Initialize notification event */
  ret = LOS_EventInit(&message->notification_event);
  if (ret != LOS_OK)
    {
      usb_warn("init notification event failed: 0x%x\n", ret);
      return 0;  /* Event initialization failures do not damage UAC function. */
    }
  g_uac_notification_init = true;

  return 0;
fail:
  (void)usbclass_uac_unbind(driver, dev);
  return -1;
}

static int usbclass_uac_unbind(struct usbdevclass_driver_s *driver, struct usbdev_s *dev)
{
  struct composite_dev_s *cdev;
  struct composite_devobj_s *devobj;
  struct usbdev_devinfo_s *devinfo;
  struct uac_dev_s *uac_dev;

  if (driver == NULL || dev == NULL)
    {
      return -1;
    }

  uac_dev = ((struct uac_driver_s *)driver)->dev;
  if (uac_dev->data_buf_count != 0)
    {
      usb_err("uac device is busy! \n");
      return -1;
    }

  usbclass_uac_disconnect(driver, dev);

  cdev = dev->ep0->priv;
  if (cdev == NULL)
    {
      return -1;
    }

  devobj = usbclass_devobj_get(cdev, DEV_UAC);
  if (devobj == NULL)
    {
      return -1;
    }
  devinfo = &devobj->compdesc.devinfo;
  (void)memset_s(devinfo, sizeof(struct usbdev_devinfo_s), 0, sizeof(struct usbdev_devinfo_s));

  fuac_write_notification_event(UAC_UNBIND, INVALID);

  fuac_source_free(driver, dev);
  return 0;
}

#if UAC_GADGET_MICROPHONE
static void uac_start_playback(struct uac_dev_s *dev)
{
  uac_set_playback_transfer_data_size(dev, g_uac_opts.p_srate, g_uac_opts.p_ssize, g_uac_opts.p_chmask);

  if (dev->in_ep_enabled == false)
    {
      (void)EP_CONFIGURE(dev->in_ep, (const struct usb_epdesc_s *)&g_fuac_as_in_ep_desc, 0);
      dev->in_ep_enabled = true;
    }
  dev->in_connected = 1;

  fuac_write_notification_event(UAC_START_PLAYBACK, INVALID);
}

static void uac_stop_playback(struct uac_dev_s *dev)
{
  spin_lock(&dev->lock);
  dev->in_connected = 0;
  LOS_AtomicSet(&dev->busy_flag, 0);
  if (dev->in_ep_enabled == true)
    {
      (void)EP_DISABLE(dev->in_ep);
      EP_FLUSH(dev->in_ep);
      dev->in_ep_enabled = false;
    }

  dev->data_buf_head = 0;
  dev->data_buf_tail = 0;
  dev->data_offset = 0;
  (void)memset_s(dev->data_buf_flag, UAC_BUF_COUNT_MAX, UAC_BUF_IDLE, UAC_BUF_COUNT_MAX);
  spin_unlock(&dev->lock);
}

#ifdef LOSCFG_DRIVERS_USB_UAC_MULTI_ALT_SETTING
static void set_p_ssize_by_alt(unsigned alt)
{
  /* Using the default alt setting, the p_ssize needs to be restored to the value set previously. */
  if (alt == 1)
    {
      g_uac_opts.p_ssize = g_fuac_p_ssize;
      fuac_write_notification_event(UAC_PSSIZE_UPDATE, g_fuac_p_ssize);
      return;
    }

#ifdef UAC_HAS_ALT2
  if (alt == 2 && g_fuac_as_in_alt2_desc_array_s.disable)
    {
      return;
    }
#endif
#ifdef UAC_HAS_ALT3
  if (alt == 3 && g_fuac_as_in_alt3_desc_array_s.disable)
    {
      return;
    }
#endif
  g_uac_opts.p_ssize = (int)alt;
  fuac_write_notification_event(UAC_PSSIZE_UPDATE, alt);
}
#endif
#endif

#if UAC_GADGET_SPEAKER
static void uac_start_capture(struct uac_dev_s *dev)
{
  configure_out_endpoint(dev);
  dev->out_connected = 1;
  fuac_write_notification_event(UAC_START_CAPTURE, INVALID);
  return;
}

static void uac_stop_capture(struct uac_dev_s *dev)
{
  dev->out_connected = 0;
  if (dev->out_ep_enabled == true)
    {
      (void)EP_DISABLE(dev->out_ep);
      EP_FLUSH(dev->out_ep);
      dev->out_ep_enabled = false;
    }

  uint32_t ret = LOS_EventWrite(&dev->out_event, UAC_READ_EVENT);
  if (ret != LOS_OK)
    {
      usb_err("write event failed :%u\n", ret);
    }

  spin_lock(&dev->lock);
  uac_free_out_queue(dev);
  spin_unlock(&dev->lock);

  fuac_write_notification_event(UAC_STOP_CAPTURE, INVALID);
  return;
}
#endif

static int usbclass_uac_set_alt(struct uac_dev_s *dev, unsigned intf, unsigned alt)
{
  if (alt >= UAC_MAX_ALT)
    {
      return -1;
    }

#if UAC_GADGET_MICROPHONE
  /* Currently, only the microphone scene supports pausing and resuming data streaming,
   * but the speaker scene does not. Therefore, if there is a mismatch in the speaker scene,
   * no error will be reported.
   */
  if (intf == g_fuac_as_in_alt_1_desc.ifno)
    {
      if (alt)
        {
#ifdef LOSCFG_DRIVERS_USB_UAC_MULTI_ALT_SETTING
          set_p_ssize_by_alt(alt);
#endif
          uac_start_playback(dev);
        }
      else
        {
          uac_stop_playback(dev);
          fuac_write_notification_event(UAC_STOP_PLAYBACK, INVALID);
        }
    }
#endif

#if UAC_GADGET_SPEAKER
#ifdef LOSCFG_DRIVERS_USB_UAC_DYNAMIC_SPEAKER
  if (g_fuac_speaker_num == 0)
    {
      return 0;
    }
#endif
  if (intf == g_fuac_as_out_alt_1_desc.ifno)
    {
      if (alt)
        {
          uac_start_capture(dev);
        }
      else
        {
          uac_stop_capture(dev);
        }
    }
#endif

  return 0;
}

static void usbclass_uac_set_configuration(struct uac_dev_s *dev)
{
#if UAC_GADGET_MICROPHONE
  uac_stop_playback(dev);
#endif
  fuac_write_notification_event(UAC_SET_CONFIG, INVALID);
#if UAC_GADGET_SPEAKER
#ifdef LOSCFG_DRIVERS_USB_UAC_DYNAMIC_SPEAKER
  if (g_fuac_speaker_num == 0)
    {
      return;
    }
#endif
  dev->out_connected = 0;
  if (dev->out_ep_enabled == true)
    {
      (void)EP_DISABLE(dev->out_ep);
      dev->out_ep_enabled = false;
    }
#endif
}

static uint8_t g_uac_intf_alts[UAC_NINTERFACES];
static int usbclass_uac_setup(struct usbdevclass_driver_s *driver, struct usbdev_s *dev,
                              const struct usb_ctrlreq_s *ctrl, uint8_t *dataout, size_t outlen)
{
  uint8_t req_type;
  uint16_t index, value;
  struct uac_dev_s *uac;
  struct uac_driver_s *drvr;
  struct usbdev_req_s *req;
  int ret = 0;

  (void)dataout;
  (void)outlen;

  if (dev == NULL || driver == NULL || ctrl == NULL)
    {
      return -1;
    }

  drvr = (struct uac_driver_s *)driver;
  uac  = drvr->dev;
  if (uac == NULL)
    {
      return -1;
    }

  req      = dev->ep0->handle_req;
  req_type = ctrl->type;
  value = USB_GETW(ctrl->value);
  index = USB_GETW(ctrl->index);

  if ((req_type & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_STANDARD)
    {
      switch (ctrl->req)
        {
        case USB_REQ_SETCONFIGURATION:
          usbclass_uac_set_configuration(uac);
          break;
        case USB_REQ_SETINTERFACE:
          ret = usbclass_uac_set_alt(uac, index, USB_GETW(ctrl->value));
          if (index < UAC_NINTERFACES)
            {
              g_uac_intf_alts[index] = (uint8_t)value;
            }
          break;
        case USB_REQ_GETINTERFACE:
          if (index >= UAC_NINTERFACES)
            {
              usb_err("invalid interface index:%d!\n", index);
              return -1;
            }
          req->buf[0] = g_uac_intf_alts[index];
          req->len = 1;
          ret = EP_SUBMIT(dev->ep0, req);
          if (ret != 0)
            {
              usb_err("endpoint send fail!\n");
              req->result = 0;
              return -1;
            }

        default:
          break;
        }
    }
  else
    {
      switch (req_type)
        {
#ifdef UAC_VER_1_0
        case USB_DIR_OUT | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_ENDPOINT:
          req->callback = fuac_request_sampling_rate_complete;
          req->priv     = uac;
          uac_set_endpoint_req(dev, req, ctrl);
          uac->control  = UAC_SET_SAMPLING_RATE;
          break;

        case USB_DIR_IN | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_ENDPOINT:
          uac_get_endpoint_req(dev, req, ctrl);
          break;

#ifdef LOSCFG_DRIVERS_USB_UAC_GADGET_FU
        case USB_DIR_OUT | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE:
          req->priv = uac;
          ret = uac_set_interface_req(dev, req, ctrl);
          break;

        case USB_DIR_IN | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE:
          ret = uac_get_interface_req(dev, req, ctrl);
          break;
#endif /* LOSCFG_DRIVERS_USB_UAC_GADGET_FU */
#else  /* UAC_VER_2_0 */
        case USB_DIR_OUT | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE:
          req->priv     = uac;
          ret = uac_set_interface_req(dev, req, ctrl);
          break;

        case USB_DIR_IN | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE:
          ret = uac_get_interface_req(dev, req, ctrl);
          break;
#endif

        default:
          break;
        }
    }
  return ret;
}

static void usbclass_uac_disconnect(struct usbdevclass_driver_s *driver, struct usbdev_s *dev)
{
  struct uac_driver_s *uac_drvr;
  struct uac_dev_s *uac_dev;

  uac_drvr = (struct uac_driver_s *)driver;
  uac_dev  = uac_drvr->dev;
  if (uac_dev == NULL)
    {
      return;
    }
#if UAC_GADGET_MICROPHONE
  uac_stop_playback(uac_dev);
#endif

#if UAC_GADGET_SPEAKER
#ifdef LOSCFG_DRIVERS_USB_UAC_DYNAMIC_SPEAKER
  if (g_fuac_speaker_num == 0)
    {
      return;
    }
#endif
  uac_stop_capture(uac_dev);
#endif
}

static struct usbd_string g_fuac_device_strings[] =
{
  { 0,  g_fuac_str_lang },
  { 1,  NULL },
  { 2,  NULL },
  { 3,  NULL },
  { 4,  g_fuac_str_ac_if },
  { 8,  g_fuac_usb_in_it },
  { 9,  g_fuac_usb_in_ot },
  { 10, g_fuac_usb_out_it },
  { 11, g_fuac_usb_out_ot },
  USBD_DEVICE_STRINGS_END
};

void uac_get_device_dec_info(struct usbd_string **device_strings, struct usb_devdesc_s **device_desc)
{
  *device_strings = &g_fuac_device_strings[1];
  *device_desc    = &g_fuac_device_desc;
}

void uac_mkdevdesc(uint8_t *buf)
{
  errno_t ret;

  if (USB_GETW(g_fuac_device_desc.vendor) == 0)
    {
      usb_err("VID is not set!\n");
      return;
    }

  ret = memcpy_s(buf, USB_COMP_EP0_BUFSIZ, &g_fuac_device_desc, sizeof(g_fuac_device_desc));
  if (ret != EOK)
    {
      usb_err("memcpy_s fail!, ret:%d\n", ret);
      return;
    }
}

static void uac_set_channel_num(void)
{
#ifdef UAC_VER_1_0
#define BMACONTROLS_UNIT 1
#else
#define BMACONTROLS_UNIT 4
#endif

#if UAC_GADGET_MICROPHONE
  uint8_t p_channel_num = fuac_channel_num_get(g_uac_opts.p_chmask);

#ifdef UAC_VER_1_0
  g_fuac_as_in_type_i_desc.bNrChannels  = p_channel_num;
#else
  g_fuac_as_in_head_desc.bNrChannels  = p_channel_num;
  g_fuac_as_in_head_desc.bmChannelConfig = g_uac_opts.p_chmask;
#endif

  g_fuac_in_it_desc.bNrChannels         = p_channel_num;
  g_fuac_in_it_desc.bmChannelConfig     = g_uac_opts.p_chmask;
#ifdef LOSCFG_DRIVERS_USB_UAC_GADGET_FU
  g_fuac_in_fu_desc.bLength = (uint8_t)(sizeof(struct uac_feature_unit_descriptor) -
                                        BMACONTROLS_UNIT * UAC_CHANNEL_NUM + BMACONTROLS_UNIT * p_channel_num);
#endif
#endif

#if UAC_GADGET_SPEAKER
#ifdef LOSCFG_DRIVERS_USB_UAC_DYNAMIC_SPEAKER
  if (g_fuac_speaker_num > 0)
#endif
    {
      uint8_t c_channel_num = fuac_channel_num_get(g_uac_opts.c_chmask);
      g_fuac_out_it_desc.bNrChannels        = c_channel_num;
      g_fuac_out_it_desc.bmChannelConfig    = g_uac_opts.c_chmask;

#ifdef UAC_VER_1_0
      g_fuac_as_out_type_i_desc.bNrChannels = c_channel_num;
#else
      g_fuac_as_out_head_desc.bNrChannels  = c_channel_num;
      g_fuac_as_out_head_desc.bmChannelConfig = g_uac_opts.c_chmask;
#endif

#ifdef LOSCFG_DRIVERS_USB_UAC_GADGET_FU
      g_fuac_out_fu_desc.bLength = (uint8_t)(sizeof(struct uac_feature_unit_descriptor) -
                                             BMACONTROLS_UNIT * UAC_CHANNEL_NUM + BMACONTROLS_UNIT * c_channel_num);
#endif
    }
#endif
}

static void uac_set_type_i_desc(void)
{
#ifdef UAC_VER_1_0
  uint8_t type_i_len = (uint8_t)(sizeof(struct uac_format_type_i_discrete_descriptor) - UAC_FREQ_NUM * UAC_FREQ_SIZE);
#if UAC_HAS_ALT2 || UAC_HAS_ALT3
  uint8_t channel_num = fuac_channel_num_get(g_uac_opts.p_chmask);
#endif
  g_fuac_as_in_type_i_desc.bBitResolution     = g_uac_opts.p_ssize * 8;  /* 8 */
  g_fuac_as_in_type_i_desc.bSubFrameSize      = g_uac_opts.p_ssize;
  g_fuac_as_in_type_i_desc.bSamFreqType       = g_uac_opts.p_freq_num;
  g_fuac_as_in_type_i_desc.bLength            = type_i_len + g_uac_opts.p_freq_num * UAC_FREQ_SIZE;

#if UAC_HAS_ALT2
  g_fuac_as_in_type_i_alt_2_desc.bSamFreqType = g_uac_opts.p_freq_num;
  g_fuac_as_in_type_i_alt_2_desc.bLength      = type_i_len + g_uac_opts.p_freq_num * UAC_FREQ_SIZE;
  g_fuac_as_in_type_i_alt_2_desc.bNrChannels  = channel_num;
#endif
#if UAC_HAS_ALT3
  g_fuac_as_in_type_i_alt_3_desc.bSamFreqType = g_uac_opts.p_freq_num;
  g_fuac_as_in_type_i_alt_3_desc.bLength      = type_i_len + g_uac_opts.p_freq_num * UAC_FREQ_SIZE;
  g_fuac_as_in_type_i_alt_3_desc.bNrChannels  = channel_num;
#endif
#if UAC_GADGET_SPEAKER
#ifdef LOSCFG_DRIVERS_USB_UAC_DYNAMIC_SPEAKER
 if (g_fuac_speaker_num == 0)
    {
      return;
    }
#endif
  g_fuac_as_out_type_i_desc.bBitResolution    = g_uac_opts.c_ssize * 8;  /* 8 */
  g_fuac_as_out_type_i_desc.bSubFrameSize     = g_uac_opts.c_ssize;
  g_fuac_as_out_type_i_desc.bSamFreqType      = g_uac_opts.c_freq_num;
  g_fuac_as_out_type_i_desc.bLength           = type_i_len + g_uac_opts.c_freq_num * UAC_FREQ_SIZE;
#endif

#else /* UAC_VER_2_0 */

#if UAC_GADGET_MICROPHONE
  g_fuac_as_in_type_i_desc.bBitResolution     = g_uac_opts.p_ssize * 8;  /* 8 */
  g_fuac_as_in_type_i_desc.bSubslotSize       = g_uac_opts.p_ssize;
#endif
#if UAC_GADGET_SPEAKER
#ifdef LOSCFG_DRIVERS_USB_UAC_DYNAMIC_SPEAKER
  if (g_fuac_speaker_num == 0)
    {
      return;
    }
#endif
  g_fuac_as_out_type_i_desc.bBitResolution    = g_uac_opts.c_ssize * 8;  /* 8 */
  g_fuac_as_out_type_i_desc.bSubslotSize      = g_uac_opts.c_ssize;
#endif
#endif
}

#ifdef CONFIG_USBDEV_SUPPORT_CERTIFY
int16_t uac_mkcfgdesc(uint8_t *buf, struct usbdev_devinfo_s *devinfo,
                      uint8_t speed, uint8_t type)
#else
int16_t uac_mkcfgdesc(uint8_t *buf, struct usbdev_devinfo_s *devinfo)
#endif
{
  uint16_t total_len;
  uint8_t *des;
  errno_t ret;

  (void)devinfo;

  uac_set_channel_num();
  uac_set_type_i_desc();
#ifdef CONFIG_USBDEV_SUPPORT_CERTIFY
  des = link_fuac_descriptors(&total_len, speed, type);
#else
  des = link_fuac_descriptors(&total_len);
#endif
  if (des != NULL)
    {
      ret = memcpy_s(buf, USB_COMP_EP0_BUFSIZ, des, total_len);
      if (ret != EOK)
        {
          usb_err("memcpy_s fail!, ret:%d\n", ret);
          free(des);
          return 0;
        }
      free(des);
    }

  return (int16_t)total_len;
}

int uac_mkstrdesc(uint8_t id, struct usb_strdesc_s *buf)
{
  errno_t ret;
  const char *str;
  uint32_t i;

  for (i = 0; i < UAC_NSTRIDS; i++)
    {
      str = g_fuac_device_strings[i].s;
      if (g_fuac_device_strings[i].id == id && str != NULL)
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

void uac_get_composite_devdesc(struct composite_devdesc_s *dev)
{
  (void)memset_s(dev, sizeof(struct composite_devdesc_s), 0, sizeof(struct composite_devdesc_s));

  dev->mkdevdesc  = uac_mkdevdesc;
  dev->mkconfdesc = uac_mkcfgdesc;
  dev->mkstrdesc  = uac_mkstrdesc;

  dev->nconfigs = UAC_NCONFIGS;           /* Number of configurations supported */
  dev->configid = UAC_CONFIGID;           /* The only supported configuration ID */

  dev->devinfo.ninterfaces = 2 + g_fuac_speaker_num; /* Number of interfaces in the configuration 2 */
  dev->devinfo.nstrings    = UAC_NSTRIDS;     /* Number of Strings */
  dev->devinfo.nendpoints  = UAC_NUM_EPS;
}

int uac_classobject(int minor, struct usbdev_devinfo_s *devinfo,
                    struct usbdevclass_driver_s **classdev)
{
  struct uac_softc *uac_s;
  struct uac_dev_s *priv;
  struct uac_driver_s *drvr;

  (void)minor;
  (void)devinfo;

  /* Allocate the structures needed */

  uac_s = (struct uac_softc *)malloc(sizeof(struct uac_softc));
  if (uac_s == NULL)
    {
      return -1;
    }
  g_fuac_softc = uac_s;

  /* Convenience pointers into the allocated blob */

  priv = &uac_s->dev;
  drvr = &uac_s->drvr;

  /* Initialize the USB serial driver structure */

  (void)memset_s(priv, sizeof(struct uac_dev_s), 0, sizeof(struct uac_dev_s));

  /* Initialize the USB class driver structure */

  drvr->drvr.speed = USB_SPEED_HIGH;
  drvr->drvr.ops   = &g_uac_driverops;
  drvr->dev        = priv;

  *classdev = &drvr->drvr;
  return 0;
}

void uac_uninitialize(struct usbdevclass_driver_s *classdev)
{
  struct uac_driver_s *uac_drvr = (struct uac_driver_s *)classdev;
  struct uac_dev_s *priv;
  struct uac_softc *uac_s;

  dev_str_desc_free(&g_fuac_device_strings[1]);

  if (uac_drvr == NULL)
    {
      return;
    }

  priv = uac_drvr->dev;
  if (priv == NULL)
    {
      return;
    }

  uac_s = container_of(uac_drvr, struct uac_softc, drvr);
  free(uac_s);
  g_fuac_softc = NULL;
}

void usbdev_uac_initialize_sub(struct composite_devdesc_s *dev, int ifnobase, int minor)
{
#ifdef LOSCFG_DRIVERS_USB_UAC_DYNAMIC_SPEAKER
  g_fuac_speaker_num = g_uac_opts.speaker_num;
#endif

  /* Ask the UAC driver to fill in the constants we didn't
   * know here.
   */
  uac_get_composite_devdesc(dev);

  /* The callback functions for the UAC class */

  dev->classobject  = uac_classobject;
  dev->uninitialize = uac_uninitialize;

  g_fuac_iad.firstif = ifnobase;
  g_fuac_ac_intf_desc.ifno = ifnobase;
#if UAC_GADGET_SPEAKER
#ifdef LOSCFG_DRIVERS_USB_UAC_DYNAMIC_SPEAKER
  if (g_fuac_speaker_num == 1)
#endif
    {
      g_fuac_as_out_alt_0_desc.ifno = ifnobase + 1;
      g_fuac_as_out_alt_1_desc.ifno = ifnobase + 1;
    }

#endif
#if UAC_GADGET_MICROPHONE
  g_fuac_as_in_alt_0_desc.ifno = ifnobase + 1 + g_fuac_speaker_num;
  g_fuac_as_in_alt_1_desc.ifno = ifnobase + 1 + g_fuac_speaker_num;
#ifdef UAC_HAS_ALT2
  g_fuac_as_in_alt_2_desc.ifno = ifnobase + 1 + g_fuac_speaker_num;
#endif
#ifdef UAC_HAS_ALT3
  g_fuac_as_in_alt_3_desc.ifno = ifnobase + 1 + g_fuac_speaker_num;
#endif
#endif
#ifdef UAC_VER_1_0
  g_fuac_ac_head_desc.bIfnr[0] = ifnobase + 1;
  g_fuac_ac_head_desc.bIfnr[g_fuac_speaker_num] = ifnobase + 1 + g_fuac_speaker_num;
#endif

  dev->devinfo.ifnobase = ifnobase; /* Offset to Interface-IDs */
  dev->minor            = minor;    /* The minor interface number */

  dev->devinfo.strbase = 0;         /* Offset to String Numbers */
}

int usbdev_uac_initialize(void *handle)
{
  struct composite_devdesc_s dev;
  int ret;

  usbdev_uac_initialize_sub(&dev, 0, DEV_UAC);

  ret = composite_initialize_softc(1, &dev, handle);
  if (ret < 0)
    {
      return -1;
    }

  PRINTK("  ** uac device initialized successfully! **\n");
  return 0;
}
