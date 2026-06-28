/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2017-2022. All rights reserved.
 * Description: LiteOS USB Driver UVC Protocol
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
#ifdef LOSCFG_DRIVERS_USB3_DEVICE_CONTROLLER
#include "controller/usb_device/pcd.h"
#endif
#ifdef LOSCFG_DRIVERS_USB2_DEVICE_CONTROLLER
#include "controller/usb_device/dwc_otg_pcd.h"
#endif
#include "gadget/f_uvc_pri.h"
#include "gadget/camera_control_pri.h"
#ifdef LOSCFG_DRIVERS_USB_UVC_IOCTL
#include "gadget/usbd_video_ioctl_pri.h"
#endif

#ifdef F_UVC_DEBUG
#define FUVC_DEBUG(x...) PRINTK(x)
#else
#define FUVC_DEBUG(x...) do {} while (0)
#endif

/*
 * Protect the changes of frame info dose not affect
 * the index of g_fuvc_descriptors_array.
 */

SPIN_LOCK_INIT(g_uvc_frame_spinlock);

/* Sequentially Map Format Indexes */

static int g_format_index_map[UVC_FORMAT_COUNT];

static int usbclass_uvc_bind(struct usbdevclass_driver_s *driver, struct usbdev_s *dev);
static int usbclass_uvc_unbind(struct usbdevclass_driver_s *driver, struct usbdev_s *dev);
static int usbclass_uvc_setup(struct usbdevclass_driver_s *driver, struct usbdev_s *dev,
                              const struct usb_ctrlreq_s *ctrl, uint8_t *dataout, size_t outlen);
static void usbclass_uvc_disconnect(struct usbdevclass_driver_s *driver,
                                    struct usbdev_s *dev);

/* USB driver operations */

static const struct usbdevclass_driverops_s g_uvc_driverops =
{
  usbclass_uvc_bind,
  usbclass_uvc_unbind,
  usbclass_uvc_setup,
  usbclass_uvc_disconnect,
  NULL,
  NULL
};

static const char g_fuvc_str_lang[] =
{
  4, USB_DESC_TYPE_STRING,
  0x09, 0x04
};

#define UVC_STR_IDX_CONFIG   5
static const char g_fuvc_str_config[] =
{
  22, USB_DESC_TYPE_STRING,
  'U', 0, 'V', 0, 'C', 0, ' ', 0, 'C', 0, 'A', 0,
  'M', 0, 'E', 0, 'R', 0, 'A', 0
};

static const char g_fuvc_str_video[] =
{
  12, USB_DESC_TYPE_STRING,
  'V', 0, 'i', 0, 'd', 0, 'e', 0, 'o', 0
};

#define UVC_STR_IDX_INTERFACE 6
static const char g_fuvc_str_interface[] =
{
  32, USB_DESC_TYPE_STRING,
  'V', 0, 'i', 0, 'd', 0, 'e', 0, 'o', 0, ' ', 0, 'S', 0, 't', 0,
  'r', 0, 'e', 0, 'a', 0, 'm', 0, 'i', 0, 'n', 0, 'g', 0
};

DEFINE_UVC_VS_INPUT_HEADER_DESC(UVC_FORMAT_COUNT, 1);
DEFINE_UVC_VC_INTERFACE_HEADER_DESC(1);
DEFINE_UVC_FRAME_DESCRIPTOR(uncompressed, 1);
DEFINE_UVC_FRAME_DESCRIPTOR(mjpeg, 1);

/* USB Video Class, Device Descriptor, for the details,
 * please refer to USB 2.0 Specification, section 9.6.1 and
 * USB Video Example 1.5, section 2.3.1
 */

static struct usb_devdesc_s g_fuvc_device_desc =
{
  .len          = sizeof(struct usb_devdesc_s),
  .type         = USB_DESC_TYPE_DEVICE, /* Constant for device descriptor */
  HSETW(.usb, USB_VERSION_BCD),         /* USB version required: 2.0 */
  .classid      = USB_CLASS_MISC,       /* Miscellaneous Device Class */
  .subclass     = 0x02,                 /* Common Class */
  .protocol     = 0x01,                 /* Interface Association Descriptor */
  .mxpacketsize = USB_EP_MPS,           /* Control Endpoint packet size */
  HSETW(.vendor,  0),                   /* Vendor ID */
  HSETW(.product, 0),                   /* Product ID */
  HSETW(.device, 0x0318),               /* Device release code */
  .imfgr        = 1,                    /* Manufacturer name, string index */
  .iproduct     = 2,                    /* Product name, string index */
  .serno        = 3,                    /* Device serial number, string index */
  .nconfigs     = 1                     /* One Configuration */
};

/* USB Video Class, Configuration Descriptor, for the details,
 * please refer to USB 2.0 Specification, section 9.6.3 and
 * USB Video Example 1.5, section 2.3.2
 */

static struct usb_cfgdesc_s g_fuvc_config_desc =
{
  .len         = sizeof(struct usb_cfgdesc_s),
  .type        = USB_DESC_TYPE_CONFIG,
  HSETW(.totallen, 0),                   /* Size of all descriptors, set later */
  .ninterfaces = 0x2,                    /* Two Interfaces */
  .cfgvalue    = 0x1,                    /* ID of this configuration */
  .icfg        = 0x4,                    /* Index of string descriptor */
  .attr        = 0x80 | USB_CONFIG_ATTR_SELFPOWER, /* Self-powered */
  .mxpower     = 1                       /* Maximum power consumption from the bus */
};

/* USB Interface Association Descriptor, for the details,
 * please refer to USB Video Class specification 1.5, section 3.6 or
 * USB Video Example 1.5, section 2.3.3
 */

static struct usb_iaddesc_s g_fuvc_iad =
{
  .len       = sizeof(struct usb_iaddesc_s),
  .type      = USB_DESC_TYPE_INTERFACEASSOCIATION,
  .firstif   = 0,   /* Interface number of VideoControl interface */
  .nifs      = 2,   /* Number of contiguous Video interfaces */
  .classid   = USB_CLASS_VIDEO,
  .subclass  = USB_UVC_SC_VIDEO_INTERFACE_COLLECTION,
  .protocol  = USB_UVC_PC_PROTOCOL_UNDEFINED,
  .ifunction = UVC_STR_IDX_CONFIG  /* Index of string descriptor */
};

/* USB VideoControl Interface Descriptor, for the details,
 * please refer to USB Video Class specification 1.5, section 3.7.1
 * or USB Video Example 1.5, section 2.3.4.1
 */

static struct usb_ifdesc_s g_fuvc_vc_intf_desc =
{
  .len      = sizeof(struct usb_ifdesc_s),
  .type     = USB_DESC_TYPE_INTERFACE,
  .ifno     = 0,   /* Index number of this interface */
  .alt      = 0,   /* Index of this settings */
  .neps     = 1,   /* One endpoint */
  .classid  = USB_CLASS_VIDEO,
  .subclass = USB_UVC_SC_VIDEOCONTROL,
  .protocol = USB_UVC_PC_PROTOCOL_UNDEFINED,
  .iif      = UVC_STR_IDX_CONFIG   /* Index of string descriptor */
};

/* UVC Class-specific VC interface header descriptor, UVC Example 1.5, section 2.3.4.2 */

static struct uvc_vc_interface_header_desc1 g_fuvc_vc_head_desc =
{
  .bLength              = sizeof(struct uvc_vc_interface_header_desc1),
  .bDescriptorType      = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype   = UVC_VC_HEADER,
  .bcdUVC               = UVC_VERSION,
  .wTotalLength         = 0,          /* Total len, currently not set */
  .dwClockFrequency     = 48000000,   /* Clock frequency */
  .bInCollection        = 0x1,        /* Number of streaming interfaces */
  .baInterfaceNr        = {0x1}       /* Associated Video Streaming Interface */
};

/* UVC input terminal descriptor, camera; UVC Example 1.5, section 2.3.4.3 */

static const struct uvc_ct_descriptor g_fuvc_vc_camera =
{
  .len              = sizeof(struct uvc_ct_descriptor),
  .type             = USB_UVC_CS_INTERFACE, /* CS_INTERFACE */
  .subtype          = UVC_VC_INPUT_TERMINAL,
  .terminalid       = UNIT_ID_CAMERA,       /* ID of this input terminal */
  .terminaltype     = USB_UVC_ITT_CAMERA,
  .assocterminal    = 0,                    /* No Association */
  .terminal         = 0,                    /* Not used */
  .objfocallenmin   = 0,                    /* No optical zoom used */
  .objfocallenmax   = 0,                    /* No optical zoom used */
  .ocularfocallen   = 0,                    /* No optical zoom used */
  .controlsize      = 3,                    /* Last Three bytes */
  .controls         = { 0xff, 0xff, 0x00 }
};

/* UVC Example 1.5, section 2.3.4.7 */

static const struct uvc_processing_descriptor g_fuvc_proc_desc =
{
  .len             = sizeof(struct uvc_processing_descriptor),
  .type            = USB_UVC_CS_INTERFACE,  /* CS_INTERFACE */
  .subtype         = UVC_VC_PROCESSING_UNIT,
  .unitid          = UNIT_ID_PROCESSING,    /* ID of this unit */
  .sourceid        = 0x01,                  /* Input pin is connected to the output pin of unit 1 */
  .maxmultiplier   = 16 * 1024,
  .controlsize     = 0x3,                   /* Sizeof controls fields */
  .controls        = { 0xff, 0xff, 0x00 },
  .processing      = 0,                     /* No string index descriptor */
#ifndef UVC_VERSION_1_0
  .videostandards  = 0                      /* Ignored */
#endif
};

/* UVC Example 1.5, section 2.3.4.5 */

static const struct uvc_ot_descriptor g_fuvc_vc_output_desc =
{
  .len             = sizeof(struct uvc_ot_descriptor),
  .type            = USB_UVC_CS_INTERFACE,  /* CS_INTERFACE */
  .subtype         = UVC_VC_OUTPUT_TERMINAL,
  .terminalid      = UNIT_ID_OUTPUT,        /* ID of this output terminal */
  .terminaltype    = USB_UVC_TT_STREAMING,
  .assocterminal   = 0,                     /* No Association */
  .sourceid        = UNIT_ID_PROCESSING,    /* Input pin connected to the output pin of unit 2 */
  .terminal        = 0                      /* Not used */
};

#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_H264
static const struct uvc_extension_unit_descriptor1x2 g_fuvc_h264_ext_desc =
{
  .len                 = sizeof(struct uvc_extension_unit_descriptor1x2),
  .type                = USB_UVC_CS_INTERFACE,
  .subtype             = UVC_VC_EXTENSION_UNIT,
  .unitid              = UNIT_ID_H264_EXTENSION,
  .guidextensioncode   =
    {
      /* Refer UVC spec. 1.1, "USB_Video_Payload_H 264_1 0", Appendix-A */

      0x41, 0x76, 0x9e, 0xa2, 0x04, 0xde, 0xe3, 0x47,
      0x8b, 0x2b, 0xf4, 0x34, 0x1a, 0xff, 0x00, 0x3b
    },
  .numcontrols         = 15,
  .nrinpins            = 0x1,
  .sourceid            = {0x02},
  .controlsize         = 2,
  .controls            = { 0xff, 0xff },
  .extension           = 0
};
#endif

static const struct uvc_extension_unit_descriptor1x2 g_fuvc_camera_ext_desc =
{
  .len                 = sizeof(struct uvc_extension_unit_descriptor1x2),
  .type                = USB_UVC_CS_INTERFACE,
  .subtype             = UVC_VC_EXTENSION_UNIT,
  .unitid              = UNIT_ID_HICAMERA_EXTENSION,
  .guidextensioncode   =
    {
      /* Refer UVC spec. 1.1, "USB_Video_Payload_H 264_1 0", Appendix-A */

      0x91, 0x72, 0x1e, 0x9a, 0x43, 0x68, 0x83, 0x46,
      0x6d, 0x92, 0x39, 0xbc, 0x79, 0x06, 0xee, 0x49
    },
  .numcontrols         = 0x15,
  .nrinpins            = 0x1,
  .sourceid            = {0x01},
  .controlsize         = 2,
  .controls            = { 0xff, 0xff },
  .extension           = 0
};

#if UVC_USE_CTRL_EP
static struct usb_epdesc_s g_fuvc_ctrl_ep_desc =
{
  .len      = sizeof(struct usb_epdesc_s),
  .type     = USB_DESC_TYPE_ENDPOINT,

  /* Hi3516ev200 platform needs to specify endpoint number, otherwise the camera audio works abnormally.
   * This way is compatible with other platforms.
   */

  .addr     = USB_DIR_IN | 0x3,
  .attr     = USB_EP_ATTR_XFER_INT,
  HSETW(.mxpacketsize, 16),
  .interval = 8
};
#endif

#ifdef USB3_DEVICE_CONTROLLER
static const struct usb_ss_comp_epdesc_s g_fuvc_ctrl_ss_comp_desc =
{
  .len      = sizeof(struct usb_ss_comp_epdesc_s),
  .type     = 0x30,
  .maxburst = 0,
  .attr     = 0,
  HSETW(.interval, 0x0010)
};
#endif

static const struct uvc_vc_int_endpoint_descriptor g_fuvc_int_ep_desc =
{
  .len             = sizeof(struct uvc_vc_int_endpoint_descriptor),
  .type            = USB_UVC_CS_ENDPOINT,
  .subtype         = UVC_EP_INTERRUPT,
  .maxtransfersize = 16
};

static struct usb_ifdesc_s g_fuvc_vs_intf_alt0 =
{
  .len      = sizeof(struct usb_ifdesc_s),
  .type     = USB_DESC_TYPE_INTERFACE,
  .ifno     = 0x1,
  .alt      = 0,
#if FUVC_BULK_TO_ISO_CONVERT
  .neps     = 0,  /* ISO must be 0 */
#else
  .neps     = 1,  /* BULK must be 1 */
#endif
  .classid  = USB_CLASS_VIDEO,
  .subclass = USB_UVC_SC_VIDEOSTREAMING,
  .protocol = USB_UVC_PC_PROTOCOL_UNDEFINED,
  .iif      = UVC_STR_IDX_INTERFACE
};

/* UVC Example 1.5, section 2.3.5.1.2 */

static struct uvc_vs_input_header_desc_n_1 g_fuvc_vs_head_desc =
{
  .bLength             = sizeof(struct uvc_vs_input_header_desc_n_1),
  .bDescriptorType     = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype  = USB_UVC_VS_INPUT_HEADER,
  .bNumFormats         = UVC_FORMAT_COUNT,
  .wTotalLength        = 0,                 /* Total len, set later */
  .bmInfo              = 0,                 /* Do not support dynamic format change */
  .bTerminalLink       = UNIT_ID_OUTPUT,    /* Terminal ID */
  .bStillCaptureMethod = 0,                 /* Do not support still image capture */
  .bTriggerSupport     = 0,                 /* Do not support trigger */
  .bTriggerUsage       = 0,                 /* Ignored */
  .bControlSize        = 1,
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_YUYV
  .bmControls[UVC_VFF_YUY2][0] = 0,
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_NV12
  .bmControls[UVC_VFF_NV12][0] = 0,
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_NV21
  .bmControls[UVC_VFF_NV21][0] = 0,
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_MJPEG
  .bmControls[UVC_VFF_MJPEG][0] = 4,
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_H264
  .bmControls[UVC_VFF_H264][0] = 0,
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_H265
  .bmControls[UVC_VFF_H265][0] = 0,
#endif
};

#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_YUYV
static struct uvc_uncompressed_format_descriptor g_fuvc_format_yuv =
{
  .bLength              = sizeof(struct uvc_uncompressed_format_descriptor),
  .bDescriptorType      = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype   = USB_UVC_VS_FORMAT_UNCOMPRESSED,
  .bFormatIndex         = 0x1,                /* Currently support only one format */
  .bNumFrameDescriptors = 0x7,                /* Only one frame descriptor follows */
  .guidFormat           =
    {
      'Y', 'U', 'Y', '2', 0x00, 0x00, 0x10, 0x00, 0x80,
      0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71
    },
  .bBitsPerPixel        = 16,                 /* Two pixels share 4 bytes? */
  .bDefaultFrameIndex   = 1,                  /* Optimum Frame Index */
  .bAspectRatioX        = 0,                  /* No Aspect Ratio */
  .bAspectRatioY        = 0,
  .bmInterlaceFlags     = 0,                  /* No interlacing */
  .bCopyProtect         = 0                   /* No copy protect */
};

static struct uvc_uncompressed_frame_descriptor1 g_fuvc_frame_yuv_144p =
{
  .bLength                   = sizeof(struct uvc_uncompressed_frame_descriptor1),
  .bDescriptorType           = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype        = USB_UVC_VS_FRAME_UNCOMPRESSED,
  .bFrameIndex               = 1, /* Only one frame descriptor */
  .bmCapabilities            = 0,
  .wWidth                    = 176,
  .wHeight                   = 144,
  .dwMinBitRate              = 55296000,
  .dwMaxBitRate              = 55296000,
  .dwMaxVideoFrameBufferSize = 50688,
  .dwDefaultFrameInterval    = 333333,
  .bFrameIntervalType        = 1,
  .dwFrameInterval           = {333333}
};

static struct uvc_uncompressed_frame_descriptor1 g_fuvc_frame_yuv_240p =
{
  .bLength                   = sizeof(struct uvc_uncompressed_frame_descriptor1),
  .bDescriptorType           = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype        = USB_UVC_VS_FRAME_UNCOMPRESSED,
  .bFrameIndex               = 2, /* Only one frame descriptor */
  .bmCapabilities            = 0,
  .wWidth                    = 320,
  .wHeight                   = 240,
  .dwMinBitRate              = 55296000,
  .dwMaxBitRate              = 55296000,
  .dwMaxVideoFrameBufferSize = 153600,
  .dwDefaultFrameInterval    = 333333,
  .bFrameIntervalType        = 1,
  .dwFrameInterval           = {333333}
};

static struct uvc_uncompressed_frame_descriptor1 g_fuvc_frame_yuv_288p =
{
  .bLength                   = sizeof(struct uvc_uncompressed_frame_descriptor1),
  .bDescriptorType           = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype        = USB_UVC_VS_FRAME_UNCOMPRESSED,
  .bFrameIndex               = 3, /* Only one frame descriptor */
  .bmCapabilities            = 0,
  .wWidth                    = 352,
  .wHeight                   = 288,
  .dwMinBitRate              = 55296000,
  .dwMaxBitRate              = 55296000,
  .dwMaxVideoFrameBufferSize = 202752,
  .dwDefaultFrameInterval    = 333333,
  .bFrameIntervalType        = 1,
  .dwFrameInterval           = {333333}
};

static struct uvc_uncompressed_frame_descriptor1 g_fuvc_frame_yuv_360p =
{
  .bLength                   = sizeof(struct uvc_uncompressed_frame_descriptor1),
  .bDescriptorType           = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype        = USB_UVC_VS_FRAME_UNCOMPRESSED,
  .bFrameIndex               = 4, /* Only one frame descriptor */
  .bmCapabilities            = 0,
  .wWidth                    = 640,
  .wHeight                   = 360,
  .dwMinBitRate              = 55296000,
  .dwMaxBitRate              = 55296000,
  .dwMaxVideoFrameBufferSize = 460800,
  .dwDefaultFrameInterval    = 333333,
  .bFrameIntervalType        = 1,
  .dwFrameInterval           = {333333}
};

static struct uvc_uncompressed_frame_descriptor1 g_fuvc_frame_yuv_480p =
{
  .bLength                   = sizeof(struct uvc_uncompressed_frame_descriptor1),
  .bDescriptorType           = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype        = USB_UVC_VS_FRAME_UNCOMPRESSED,
  .bFrameIndex               = 5, /* Only one frame descriptor */
  .bmCapabilities            = 0,
  .wWidth                    = 640,
  .wHeight                   = 480,
  .dwMinBitRate              = 55296000,
  .dwMaxBitRate              = 55296000,
  .dwMaxVideoFrameBufferSize = 614400,
  .dwDefaultFrameInterval    = 333333,
  .bFrameIntervalType        = 1,
  .dwFrameInterval           = {333333}
};

static struct uvc_uncompressed_frame_descriptor1 g_fuvc_frame_yuv_720p =
{
  .bLength                   = sizeof(struct uvc_uncompressed_frame_descriptor1),
  .bDescriptorType           = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype        = USB_UVC_VS_FRAME_UNCOMPRESSED,
  .bFrameIndex               = 6, /* Only one frame descriptor */
  .bmCapabilities            = 0,
  .wWidth                    = 1280,
  .wHeight                   = 720,
  .dwMinBitRate              = 29491200,
  .dwMaxBitRate              = 29491200,
  .dwMaxVideoFrameBufferSize = 1843200,
  .dwDefaultFrameInterval    = 333333,
  .bFrameIntervalType        = 1,
  .dwFrameInterval           = {333333}
};

static struct uvc_uncompressed_frame_descriptor1 g_fuvc_frame_yuv_1080p =
{
  .bLength                   = sizeof(struct uvc_uncompressed_frame_descriptor1),
  .bDescriptorType           = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype        = USB_UVC_VS_FRAME_UNCOMPRESSED,
  .bFrameIndex               = 7, /* Only one frame descriptor */
  .bmCapabilities            = 0,
  .wWidth                    = 1920,
  .wHeight                   = 1080,
  .dwMinBitRate              = 29491200,
  .dwMaxBitRate              = 29491200,
  .dwMaxVideoFrameBufferSize = 4147200,
  .dwDefaultFrameInterval    = 333333,
  .bFrameIntervalType        = 1,
  .dwFrameInterval           = {333333}
};
#endif

#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_NV12
static struct uvc_uncompressed_format_descriptor g_fuvc_format_nv12 =
{
  .bLength              = sizeof(struct uvc_uncompressed_format_descriptor),
  .bDescriptorType      = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype   = USB_UVC_VS_FORMAT_UNCOMPRESSED,
  .bFormatIndex         = 0x2,                /* Currently support only one format */
  .bNumFrameDescriptors = 0x7,                /* Only one frame descriptor follows */
  .guidFormat           =
    {
      'N', 'V', '1', '2', 0x00, 0x00, 0x10, 0x00, 0x80,
      0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71
    },
  .bBitsPerPixel        = 12,                 /* Two pixels share 3 bytes? */
  .bDefaultFrameIndex   = 1,                  /* Optimum Frame Index */
  .bAspectRatioX        = 0,                  /* No Aspect Ratio */
  .bAspectRatioY        = 0,
  .bmInterlaceFlags     = 0,                  /* No interlacing */
  .bCopyProtect         = 0                   /* No copy protect */
};

static struct uvc_uncompressed_frame_descriptor1 g_fuvc_frame_nv12_144p =
{
  .bLength                   = sizeof(struct uvc_uncompressed_frame_descriptor1),
  .bDescriptorType           = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype        = USB_UVC_VS_FRAME_UNCOMPRESSED,
  .bFrameIndex               = 1, /* Only one frame descriptor */
  .bmCapabilities            = 0,
  .wWidth                    = 176,
  .wHeight                   = 144,
  .dwMinBitRate              = 55296000,
  .dwMaxBitRate              = 55296000,
  .dwMaxVideoFrameBufferSize = 38016,
  .dwDefaultFrameInterval    = 333333,
  .bFrameIntervalType        = 1,
  .dwFrameInterval           = {333333}
};

static struct uvc_uncompressed_frame_descriptor1 g_fuvc_frame_nv12_240p =
{
  .bLength                   = sizeof(struct uvc_uncompressed_frame_descriptor1),
  .bDescriptorType           = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype        = USB_UVC_VS_FRAME_UNCOMPRESSED,
  .bFrameIndex               = 2, /* Only one frame descriptor */
  .bmCapabilities            = 0,
  .wWidth                    = 320,
  .wHeight                   = 240,
  .dwMinBitRate              = 55296000,
  .dwMaxBitRate              = 55296000,
  .dwMaxVideoFrameBufferSize = 115200,
  .dwDefaultFrameInterval    = 333333,
  .bFrameIntervalType        = 1,
  .dwFrameInterval           = {333333}
};

static struct uvc_uncompressed_frame_descriptor1 g_fuvc_frame_nv12_288p =
{
  .bLength                   = sizeof(struct uvc_uncompressed_frame_descriptor1),
  .bDescriptorType           = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype        = USB_UVC_VS_FRAME_UNCOMPRESSED,
  .bFrameIndex               = 3, /* Only one frame descriptor */
  .bmCapabilities            = 0,
  .wWidth                    = 352,
  .wHeight                   = 288,
  .dwMinBitRate              = 55296000,
  .dwMaxBitRate              = 55296000,
  .dwMaxVideoFrameBufferSize = 152064,
  .dwDefaultFrameInterval    = 333333,
  .bFrameIntervalType        = 1,
  .dwFrameInterval           = {333333}
};

static struct uvc_uncompressed_frame_descriptor1 g_fuvc_frame_nv12_360p =
{
  .bLength                   = sizeof(struct uvc_uncompressed_frame_descriptor1),
  .bDescriptorType           = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype        = USB_UVC_VS_FRAME_UNCOMPRESSED,
  .bFrameIndex               = 4, /* Only one frame descriptor */
  .bmCapabilities            = 0,
  .wWidth                    = 640,
  .wHeight                   = 360,
  .dwMinBitRate              = 55296000,
  .dwMaxBitRate              = 55296000,
  .dwMaxVideoFrameBufferSize = 345600,
  .dwDefaultFrameInterval    = 333333,
  .bFrameIntervalType        = 1,
  .dwFrameInterval           = {333333}
};

static struct uvc_uncompressed_frame_descriptor1 g_fuvc_frame_nv12_480p =
{
  .bLength                   = sizeof(struct uvc_uncompressed_frame_descriptor1),
  .bDescriptorType           = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype        = USB_UVC_VS_FRAME_UNCOMPRESSED,
  .bFrameIndex               = 5, /* Only one frame descriptor */
  .bmCapabilities            = 0,
  .wWidth                    = 640,
  .wHeight                   = 480,
  .dwMinBitRate              = 55296000,
  .dwMaxBitRate              = 55296000,
  .dwMaxVideoFrameBufferSize = 460800,
  .dwDefaultFrameInterval    = 333333,
  .bFrameIntervalType        = 1,
  .dwFrameInterval           = {333333}
};

static struct uvc_uncompressed_frame_descriptor1 g_fuvc_frame_nv12_720p =
{
  .bLength                   = sizeof(struct uvc_uncompressed_frame_descriptor1),
  .bDescriptorType           = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype        = USB_UVC_VS_FRAME_UNCOMPRESSED,
  .bFrameIndex               = 6, /* Only one frame descriptor */
  .bmCapabilities            = 0,
  .wWidth                    = 1280,
  .wHeight                   = 720,
  .dwMinBitRate              = 29491200,
  .dwMaxBitRate              = 29491200,
  .dwMaxVideoFrameBufferSize = 1382400,
  .dwDefaultFrameInterval    = 333333,
  .bFrameIntervalType        = 1,
  .dwFrameInterval           = {333333}
};

static struct uvc_uncompressed_frame_descriptor1 g_fuvc_frame_nv12_1080p =
{
  .bLength                   = sizeof(struct uvc_uncompressed_frame_descriptor1),
  .bDescriptorType           = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype        = USB_UVC_VS_FRAME_UNCOMPRESSED,
  .bFrameIndex               = 7, /* Only one frame descriptor */
  .bmCapabilities            = 0,
  .wWidth                    = 1920,
  .wHeight                   = 1080,
  .dwMinBitRate              = 29491200,
  .dwMaxBitRate              = 29491200,
  .dwMaxVideoFrameBufferSize = 3110400,
  .dwDefaultFrameInterval    = 333333,
  .bFrameIntervalType        = 1,
  .dwFrameInterval           = {333333}
};
#endif

#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_NV21
static struct uvc_uncompressed_format_descriptor g_fuvc_format_nv21 =
{
  .bLength              = sizeof(struct uvc_uncompressed_format_descriptor),
  .bDescriptorType      = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype   = USB_UVC_VS_FORMAT_UNCOMPRESSED,
  .bFormatIndex         = 0x3,                /* Currently support only one format */
  .bNumFrameDescriptors = 0x7,                /* Only one frame descriptor follows */
  .guidFormat           =
    {
      'N', 'V', '2', '1', 0x00, 0x00, 0x10, 0x00, 0x80,
      0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71
    },
  .bBitsPerPixel        = 12,                 /* Two pixels share 3 bytes? */
  .bDefaultFrameIndex   = 1,                  /* Optimum Frame Index */
  .bAspectRatioX        = 0,                  /* No Aspect Ratio */
  .bAspectRatioY        = 0,
  .bmInterlaceFlags     = 0,                  /* No interlacing */
  .bCopyProtect         = 0                   /* No copy protect */
};

static struct uvc_uncompressed_frame_descriptor1 g_fuvc_frame_nv21_144p =
{
  .bLength                   = sizeof(struct uvc_uncompressed_frame_descriptor1),
  .bDescriptorType           = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype        = USB_UVC_VS_FRAME_UNCOMPRESSED,
  .bFrameIndex               = 1, /* Only one frame descriptor */
  .bmCapabilities            = 0,
  .wWidth                    = 176,
  .wHeight                   = 144,
  .dwMinBitRate              = 55296000,
  .dwMaxBitRate              = 55296000,
  .dwMaxVideoFrameBufferSize = 38016,
  .dwDefaultFrameInterval    = 333333,
  .bFrameIntervalType        = 1,
  .dwFrameInterval           = {333333}
};

static struct uvc_uncompressed_frame_descriptor1 g_fuvc_frame_nv21_240p =
{
  .bLength                   = sizeof(struct uvc_uncompressed_frame_descriptor1),
  .bDescriptorType           = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype        = USB_UVC_VS_FRAME_UNCOMPRESSED,
  .bFrameIndex               = 2, /* Only one frame descriptor */
  .bmCapabilities            = 0,
  .wWidth                    = 320,
  .wHeight                   = 240,
  .dwMinBitRate              = 55296000,
  .dwMaxBitRate              = 55296000,
  .dwMaxVideoFrameBufferSize = 115200,
  .dwDefaultFrameInterval    = 333333,
  .bFrameIntervalType        = 1,
  .dwFrameInterval           = {333333}
};

static struct uvc_uncompressed_frame_descriptor1 g_fuvc_frame_nv21_288p =
{
  .bLength                   = sizeof(struct uvc_uncompressed_frame_descriptor1),
  .bDescriptorType           = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype        = USB_UVC_VS_FRAME_UNCOMPRESSED,
  .bFrameIndex               = 3, /* Only one frame descriptor */
  .bmCapabilities            = 0,
  .wWidth                    = 352,
  .wHeight                   = 288,
  .dwMinBitRate              = 55296000,
  .dwMaxBitRate              = 55296000,
  .dwMaxVideoFrameBufferSize = 152064,
  .dwDefaultFrameInterval    = 333333,
  .bFrameIntervalType        = 1,
  .dwFrameInterval           = {333333}
};

static struct uvc_uncompressed_frame_descriptor1 g_fuvc_frame_nv21_360p =
{
  .bLength                   = sizeof(struct uvc_uncompressed_frame_descriptor1),
  .bDescriptorType           = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype        = USB_UVC_VS_FRAME_UNCOMPRESSED,
  .bFrameIndex               = 4, /* Only one frame descriptor */
  .bmCapabilities            = 0,
  .wWidth                    = 640,
  .wHeight                   = 360,
  .dwMinBitRate              = 55296000,
  .dwMaxBitRate              = 55296000,
  .dwMaxVideoFrameBufferSize = 345600,
  .dwDefaultFrameInterval    = 333333,
  .bFrameIntervalType        = 1,
  .dwFrameInterval           = {333333}
};

static struct uvc_uncompressed_frame_descriptor1 g_fuvc_frame_nv21_480p =
{
  .bLength                   = sizeof(struct uvc_uncompressed_frame_descriptor1),
  .bDescriptorType           = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype        = USB_UVC_VS_FRAME_UNCOMPRESSED,
  .bFrameIndex               = 5, /* Only one frame descriptor */
  .bmCapabilities            = 0,
  .wWidth                    = 640,
  .wHeight                   = 480,
  .dwMinBitRate              = 55296000,
  .dwMaxBitRate              = 55296000,
  .dwMaxVideoFrameBufferSize = 460800,
  .dwDefaultFrameInterval    = 333333,
  .bFrameIntervalType        = 1,
  .dwFrameInterval           = {333333}
};

static struct uvc_uncompressed_frame_descriptor1 g_fuvc_frame_nv21_720p =
{
  .bLength                   = sizeof(struct uvc_uncompressed_frame_descriptor1),
  .bDescriptorType           = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype        = USB_UVC_VS_FRAME_UNCOMPRESSED,
  .bFrameIndex               = 6, /* Only one frame descriptor */
  .bmCapabilities            = 0,
  .wWidth                    = 1280,
  .wHeight                   = 720,
  .dwMinBitRate              = 29491200,
  .dwMaxBitRate              = 29491200,
  .dwMaxVideoFrameBufferSize = 1382400,
  .dwDefaultFrameInterval    = 333333,
  .bFrameIntervalType        = 1,
  .dwFrameInterval           = {333333}
};

static struct uvc_uncompressed_frame_descriptor1 g_fuvc_frame_nv21_1080p =
{
  .bLength                   = sizeof(struct uvc_uncompressed_frame_descriptor1),
  .bDescriptorType           = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype        = USB_UVC_VS_FRAME_UNCOMPRESSED,
  .bFrameIndex               = 7, /* Only one frame descriptor */
  .bmCapabilities            = 0,
  .wWidth                    = 1920,
  .wHeight                   = 1080,
  .dwMinBitRate              = 29491200,
  .dwMaxBitRate              = 29491200,
  .dwMaxVideoFrameBufferSize = 3110400,
  .dwDefaultFrameInterval    = 333333,
  .bFrameIntervalType        = 1,
  .dwFrameInterval           = {333333}
};
#endif

static const struct uvc_color_matching_descriptor g_fuvc_color_matching_descriptor =
{
  .len                  = sizeof(struct uvc_color_matching_descriptor),
  .type                 = USB_UVC_CS_INTERFACE,
  .subtype              = USB_UVC_VS_COLOR_FORMAT,
  .colorprimaries       = UVC_COLOR_BT709_SRGB,
  .transferchars        = UVC_COLOR_BT709_SRGB,
  .matrixcoefficients   = UVC_COLOR_MC_SMPTE_170M
};

#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_H264
static struct uvc_frame_based_format_descriptor g_fuvc_format_h264 =
{
  .len                 = sizeof(struct uvc_frame_based_format_descriptor),
  .type                = USB_UVC_CS_INTERFACE,
  .subtype             = USB_UVC_VS_FORMAT_FRAME_BASED,
  .formatindex         = 5,
  .numframedesc        = 7,
  .guidformat          =
    {
      'H', '2', '6', '4', 0x00, 0x00, 0x10, 0x00, 0x80,
      0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71
    },
  .bitsperpixel        = 16,
  .dframeindex         = 1,
  .aspectratiox        = 0,
  .aspectratioy        = 0,
  .interlaceflags      = 0,
  .copyprotect         = 0,
  .variablesize        = 1
};

static struct uvc_frame_based_frame_descriptor1 g_fuvc_frame_h264_144p =
{
  .len                 = sizeof(struct uvc_frame_based_frame_descriptor1),
  .type                = USB_UVC_CS_INTERFACE,
  .subtype             = USB_UVC_VS_FRAME_FRAME_BASED,
  .frameindex          = 1,
  .capabilities        = 0,
  .width               = 176,
  .height              = 144,
  .minbitrate          = 29491200,
  .maxbitrate          = 29491200,
  .dframeinterval      = 333333,
  .frameintervaltype   = 1, /* Number of supported Frame intervals */
  .bytesperline        = 0, /* Must be 0 */
  .frameinterval       = {333333}
};

static struct uvc_frame_based_frame_descriptor1 g_fuvc_frame_h264_240p =
{
  .len                 = sizeof(struct uvc_frame_based_frame_descriptor1),
  .type                = USB_UVC_CS_INTERFACE,
  .subtype             = USB_UVC_VS_FRAME_FRAME_BASED,
  .frameindex          = 2,
  .capabilities        = 0,
  .width               = 320,
  .height              = 240,
  .minbitrate          = 29491200,
  .maxbitrate          = 29491200,
  .dframeinterval      = 333333,
  .frameintervaltype   = 1, /* Number of supported Frame intervals */
  .bytesperline        = 0, /* Must be 0 */
  .frameinterval       = {333333}
};

static struct uvc_frame_based_frame_descriptor1 g_fuvc_frame_h264_288p =
{
  .len                 = sizeof(struct uvc_frame_based_frame_descriptor1),
  .type                = USB_UVC_CS_INTERFACE,
  .subtype             = USB_UVC_VS_FRAME_FRAME_BASED,
  .frameindex          = 3,
  .capabilities        = 0,
  .width               = 352,
  .height              = 288,
  .minbitrate          = 29491200,
  .maxbitrate          = 29491200,
  .dframeinterval      = 333333,
  .frameintervaltype   = 1, /* Number of supported Frame intervals */
  .bytesperline        = 0, /* Must be 0 */
  .frameinterval       = {333333}
};

static struct uvc_frame_based_frame_descriptor1 g_fuvc_frame_h264_360p =
{
  .len                 = sizeof(struct uvc_frame_based_frame_descriptor1),
  .type                = USB_UVC_CS_INTERFACE,
  .subtype             = USB_UVC_VS_FRAME_FRAME_BASED,
  .frameindex          = 4,
  .capabilities        = 0,
  .width               = 640,
  .height              = 360,
  .minbitrate          = 10240000,
  .maxbitrate          = 10240000,
  .dframeinterval      = 333333,
  .frameintervaltype   = 1, /* Number of supported Frame intervals */
  .bytesperline        = 0, /* Must be 0 */
  .frameinterval       = {333333}
};

static struct uvc_frame_based_frame_descriptor1 g_fuvc_frame_h264_480p =
{
  .len                 = sizeof(struct uvc_frame_based_frame_descriptor1),
  .type                = USB_UVC_CS_INTERFACE,
  .subtype             = USB_UVC_VS_FRAME_FRAME_BASED,
  .frameindex          = 5,
  .capabilities        = 0,
  .width               = 640,
  .height              = 480,
  .minbitrate          = 10240000,
  .maxbitrate          = 10240000,
  .dframeinterval      = 333333,
  .frameintervaltype   = 1, /* Number of supported Frame intervals */
  .bytesperline        = 0, /* Must be 0 */
  .frameinterval       = {333333}
};

static struct uvc_frame_based_frame_descriptor1 g_fuvc_frame_h264_720p =
{
  .len                 = sizeof(struct uvc_frame_based_frame_descriptor1),
  .type                = USB_UVC_CS_INTERFACE,
  .subtype             = USB_UVC_VS_FRAME_FRAME_BASED,
  .frameindex          = 6,
  .capabilities        = 0,
  .width               = 1280,
  .height              = 720,
  .minbitrate          = 20480000,
  .maxbitrate          = 20480000,
  .dframeinterval      = 333333,
  .frameintervaltype   = 1, /* Number of supported Frame intervals */
  .bytesperline        = 0, /* Must be 0 */
  .frameinterval       = {333333}
};

static struct uvc_frame_based_frame_descriptor1 g_fuvc_frame_h264_1080p =
{
  .len                 = sizeof(struct uvc_frame_based_frame_descriptor1),
  .type                = USB_UVC_CS_INTERFACE,
  .subtype             = USB_UVC_VS_FRAME_FRAME_BASED,
  .frameindex          = 7,
  .capabilities        = 0,
  .width               = UVC_VIDEO_WIDTH_DEF,
  .height              = UVC_VIDEO_HEIGHT_DEF,
  .minbitrate          = 40960000,
  .maxbitrate          = 40960000,
  .dframeinterval      = 333333,
  .frameintervaltype   = 1, /* Number of supported Frame intervals */
  .bytesperline        = 0, /* Must be 0 */
  .frameinterval       = {333333}
};
#endif

#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_H265
static struct uvc_frame_based_format_descriptor g_fuvc_format_h265 =
{
  .len                 = sizeof(struct uvc_frame_based_format_descriptor),
  .type                = USB_UVC_CS_INTERFACE,
  .subtype             = USB_UVC_VS_FORMAT_FRAME_BASED,
  .formatindex         = 6,
  .numframedesc        = 7,
  .guidformat          =
    {
      'H', '2', '6', '5', 0x00, 0x00, 0x10, 0x00, 0x80,
      0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71
    },
  .bitsperpixel        = 16,
  .dframeindex         = 1,
  .aspectratiox        = 0,
  .aspectratioy        = 0,
  .interlaceflags      = 0,
  .copyprotect         = 0,
  .variablesize        = 1
};

static struct uvc_frame_based_frame_descriptor1 g_fuvc_frame_h265_144p =
{
  .len                 = sizeof(struct uvc_frame_based_frame_descriptor1),
  .type                = USB_UVC_CS_INTERFACE,
  .subtype             = USB_UVC_VS_FRAME_FRAME_BASED,
  .frameindex          = 1,
  .capabilities        = 0,
  .width               = 176,
  .height              = 144,
  .minbitrate          = 29491200,
  .maxbitrate          = 29491200,
  .dframeinterval      = 333333,
  .frameintervaltype   = 1, /* Number of supported Frame intervals */
  .bytesperline        = 0, /* Must be 0 */
  .frameinterval       = {333333}
};

static struct uvc_frame_based_frame_descriptor1 g_fuvc_frame_h265_240p =
{
  .len                 = sizeof(struct uvc_frame_based_frame_descriptor1),
  .type                = USB_UVC_CS_INTERFACE,
  .subtype             = USB_UVC_VS_FRAME_FRAME_BASED,
  .frameindex          = 2,
  .capabilities        = 0,
  .width               = 320,
  .height              = 240,
  .minbitrate          = 29491200,
  .maxbitrate          = 29491200,
  .dframeinterval      = 333333,
  .frameintervaltype   = 1, /* Number of supported Frame intervals */
  .bytesperline        = 0, /* Must be 0 */
  .frameinterval       = {333333}
};

static struct uvc_frame_based_frame_descriptor1 g_fuvc_frame_h265_288p =
{
  .len                 = sizeof(struct uvc_frame_based_frame_descriptor1),
  .type                = USB_UVC_CS_INTERFACE,
  .subtype             = USB_UVC_VS_FRAME_FRAME_BASED,
  .frameindex          = 3,
  .capabilities        = 0,
  .width               = 352,
  .height              = 288,
  .minbitrate          = 29491200,
  .maxbitrate          = 29491200,
  .dframeinterval      = 333333,
  .frameintervaltype   = 1, /* Number of supported Frame intervals */
  .bytesperline        = 0, /* Must be 0 */
  .frameinterval       = {333333}
};

static struct uvc_frame_based_frame_descriptor1 g_fuvc_frame_h265_360p =
{
  .len                 = sizeof(struct uvc_frame_based_frame_descriptor1),
  .type                = USB_UVC_CS_INTERFACE,
  .subtype             = USB_UVC_VS_FRAME_FRAME_BASED,
  .frameindex          = 4,
  .capabilities        = 0,
  .width               = 640,
  .height              = 360,
  .minbitrate          = 29491200,
  .maxbitrate          = 29491200,
  .dframeinterval      = 333333,
  .frameintervaltype   = 1, /* Number of supported Frame intervals */
  .bytesperline        = 0, /* Must be 0 */
  .frameinterval       = {333333}
};

static struct uvc_frame_based_frame_descriptor1 g_fuvc_frame_h265_480p =
{
  .len                 = sizeof(struct uvc_frame_based_frame_descriptor1),
  .type                = USB_UVC_CS_INTERFACE,
  .subtype             = USB_UVC_VS_FRAME_FRAME_BASED,
  .frameindex          = 5,
  .capabilities        = 0,
  .width               = 640,
  .height              = 480,
  .minbitrate          = 10240000,
  .maxbitrate          = 10240000,
  .dframeinterval      = 333333,
  .frameintervaltype   = 1, /* Number of supported Frame intervals */
  .bytesperline        = 0, /* Must be 0 */
  .frameinterval       = {333333}
};

static struct uvc_frame_based_frame_descriptor1 g_fuvc_frame_h265_720p =
{
  .len                 = sizeof(struct uvc_frame_based_frame_descriptor1),
  .type                = USB_UVC_CS_INTERFACE,
  .subtype             = USB_UVC_VS_FRAME_FRAME_BASED,
  .frameindex          = 6,
  .capabilities        = 0,
  .width               = 1280,
  .height              = 720,
  .minbitrate          = 29491200,
  .maxbitrate          = 29491200,
  .dframeinterval      = 333333,
  .frameintervaltype   = 1, /* Number of supported Frame intervals */
  .bytesperline        = 0, /* Must be 0 */
  .frameinterval       = {333333}
};

static struct uvc_frame_based_frame_descriptor1 g_fuvc_frame_h265_1080p =
{
  .len                 = sizeof(struct uvc_frame_based_frame_descriptor1),
  .type                = USB_UVC_CS_INTERFACE,
  .subtype             = USB_UVC_VS_FRAME_FRAME_BASED,
  .frameindex          = 7,
  .capabilities        = 0,
  .width               = UVC_VIDEO_WIDTH_DEF,
  .height              = UVC_VIDEO_HEIGHT_DEF,
  .minbitrate          = 29491200,
  .maxbitrate          = 29491200,
  .dframeinterval      = 333333,
  .frameintervaltype   = 1, /* Number of supported Frame intervals */
  .bytesperline        = 0, /* Must be 0 */
  .frameinterval       = {333333}
};
#endif

#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_MJPEG
static struct uvc_mjpeg_format_descriptor g_fuvc_format_mjpeg =
{
  .bLength              = sizeof(struct uvc_mjpeg_format_descriptor),
  .bDescriptorType      = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype   = USB_UVC_VS_FORMAT_MJPEG,
  .bFormatIndex         = 2,
  .bNumFrameDescriptors = 7,
  .bmFlags              = 0,
  .bDefaultFrameIndex   = 1,
  .bAspectRatioX        = 0,
  .bAspectRatioY        = 0,
  .bmInterlaceFlags     = 0,
  .bCopyProtect         = 0
};

static struct uvc_mjpeg_frame_descriptor1 g_fuvc_frame_mjpeg_144p =
{
  .bLength                   = sizeof(struct uvc_mjpeg_frame_descriptor1),
  .bDescriptorType           = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype        = USB_UVC_VS_FRAME_MJPEG,
  .bFrameIndex               = 1,
  .bmCapabilities            = 0,
  .wWidth                    = 176,
  .wHeight                   = 144,
  .dwMinBitRate              = 55296000,
  .dwMaxBitRate              = 55296000,
  .dwMaxVideoFrameBufferSize = 25344,
  .dwDefaultFrameInterval    = 333333,
  .bFrameIntervalType        = 1,
  .dwFrameInterval           = {333333}
};

static struct uvc_mjpeg_frame_descriptor1 g_fuvc_frame_mjpeg_240p =
{
  .bLength                   = sizeof(struct uvc_mjpeg_frame_descriptor1),
  .bDescriptorType           = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype        = USB_UVC_VS_FRAME_MJPEG,
  .bFrameIndex               = 2,
  .bmCapabilities            = 0,
  .wWidth                    = 320,
  .wHeight                   = 240,
  .dwMinBitRate              = 55296000,
  .dwMaxBitRate              = 55296000,
  .dwMaxVideoFrameBufferSize = 76800,
  .dwDefaultFrameInterval    = 333333,
  .bFrameIntervalType        = 1,
  .dwFrameInterval           = {333333}
};

static struct uvc_mjpeg_frame_descriptor1 g_fuvc_frame_mjpeg_288p =
{
  .bLength                   = sizeof(struct uvc_mjpeg_frame_descriptor1),
  .bDescriptorType           = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype        = USB_UVC_VS_FRAME_MJPEG,
  .bFrameIndex               = 3,
  .bmCapabilities            = 0,
  .wWidth                    = 352,
  .wHeight                   = 288,
  .dwMinBitRate              = 55296000,
  .dwMaxBitRate              = 55296000,
  .dwMaxVideoFrameBufferSize = 101376,
  .dwDefaultFrameInterval    = 333333,
  .bFrameIntervalType        = 1,
  .dwFrameInterval           = {333333}
};

static struct uvc_mjpeg_frame_descriptor1 g_fuvc_frame_mjpeg_360p =
{
  .bLength                   = sizeof(struct uvc_mjpeg_frame_descriptor1),
  .bDescriptorType           = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype        = USB_UVC_VS_FRAME_MJPEG,
  .bFrameIndex               = 4,
  .bmCapabilities            = 0,
  .wWidth                    = 640,
  .wHeight                   = 360,
  .dwMinBitRate              = 10240000,
  .dwMaxBitRate              = 10240000,
  .dwMaxVideoFrameBufferSize = 230400,
  .dwDefaultFrameInterval    = 333333,
  .bFrameIntervalType        = 1,
  .dwFrameInterval           = {333333}
};

static struct uvc_mjpeg_frame_descriptor1 g_fuvc_frame_mjpeg_480p =
{
  .bLength                   = sizeof(struct uvc_mjpeg_frame_descriptor1),
  .bDescriptorType           = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype        = USB_UVC_VS_FRAME_MJPEG,
  .bFrameIndex               = 5,
  .bmCapabilities            = 0,
  .wWidth                    = 640,
  .wHeight                   = 480,
  .dwMinBitRate              = 10240000,
  .dwMaxBitRate              = 10240000,
  .dwMaxVideoFrameBufferSize = 307200,
  .dwDefaultFrameInterval    = 333333,
  .bFrameIntervalType        = 1,
  .dwFrameInterval           = {333333}
};

static struct uvc_mjpeg_frame_descriptor1 g_fuvc_frame_mjpeg_720p =
{
  .bLength                   = sizeof(struct uvc_mjpeg_frame_descriptor1),
  .bDescriptorType           = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype        = USB_UVC_VS_FRAME_MJPEG,
  .bFrameIndex               = 6,
  .bmCapabilities            = 0,
  .wWidth                    = 1280,
  .wHeight                   = 720,
  .dwMinBitRate              = 20480000,
  .dwMaxBitRate              = 20480000,
  .dwMaxVideoFrameBufferSize = 921600,
  .dwDefaultFrameInterval    = 333333,
  .bFrameIntervalType        = 1,
  .dwFrameInterval           = {333333}
};

static struct uvc_mjpeg_frame_descriptor1 g_fuvc_frame_mjpeg_1080p =
{
  .bLength                   = sizeof(struct uvc_mjpeg_frame_descriptor1),
  .bDescriptorType           = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype        = USB_UVC_VS_FRAME_MJPEG,
  .bFrameIndex               = 7,
  .bmCapabilities            = 0,
  .wWidth                    = 1920,
  .wHeight                   = 1080,
  .dwMinBitRate              = 40960000,
  .dwMaxBitRate              = 40960000,
  .dwMaxVideoFrameBufferSize = 2073600,
  .dwDefaultFrameInterval    = 333333,
  .bFrameIntervalType        = 1,
  .dwFrameInterval           = {333333}
};
#endif

#if FUVC_BULK_TO_ISO_CONVERT
static const struct usb_ifdesc_s g_fuvc_isoc_streaming_intf_alt1 =
{
  .len      = sizeof(struct usb_ifdesc_s),
  .type     = USB_DESC_TYPE_INTERFACE,
  .ifno     = 0x1,
  .alt      = 1,
  .neps     = 1,
  .classid  = USB_CLASS_VIDEO,
  .subclass = USB_UVC_SC_VIDEOSTREAMING,
  .protocol = USB_UVC_PC_PROTOCOL_UNDEFINED,
  .iif      = UVC_STR_IDX_INTERFACE
};

struct usb_epdesc_s g_fuvc_hs_isoc_streaming_ep =
{
  .len      = sizeof(struct usb_epdesc_s),
  .type     = USB_DESC_TYPE_ENDPOINT,

  /* Hi3516ev200 platform needs to specify endpoint number, otherwise the camera audio works abnormally.
   * This way is compatible with other platforms.
   */

  .addr     = USB_DIR_IN | 0x2,
  .attr     = USB_EP_ATTR_ASYNC | USB_EP_ATTR_XFER_ISOC,
  HSETW(.mxpacketsize, 0x1400),
  .interval = 1
};

#ifdef USB3_DEVICE_CONTROLLER
static const struct usb_ss_comp_epdesc_s g_fuvc_isoc_streaming_ss_comp_desc =
{
  .len      = sizeof(struct usb_ss_comp_epdesc_s),
  .type     = 0x30,
  .maxburst = 0xf,
  .attr     = 2,
  HSETW(.interval, 0xc000)
};
#endif
#else

/***********************BULK MODE***************************/

struct usb_epdesc_s g_fuvc_hs_bulk_streaming_ep =
{
  .len      = USB_SIZEOF_EPDESC,
  .type     = USB_DESC_TYPE_ENDPOINT,

  /* Hi3516ev200 platform needs to specify endpoint number, otherwise the camera audio works abnormally.
   * This way is compatible with other platforms.
   */

  .addr     = USB_DIR_IN | 0x2,
  .attr     = USB_EP_ATTR_XFER_BULK,

  /* The wMaxPacketSize and bInterval values will be initialized from
   * module parameters.
   */

  HSETW(.mxpacketsize, 0x200),
  .interval = 0
};

#ifdef USB3_DEVICE_CONTROLLER
static const struct usb_ss_comp_epdesc_s g_fuvc_bulk_streaming_ss_comp_desc =
{
  .len      = sizeof(struct usb_ss_comp_epdesc_s),
  .type     = 0x30,
  .maxburst = 0x0,
  .attr     = 0,
  HSETW(.interval, 0)
};
#endif
#endif

/***********************BULK MODE***************************/

static const uint8_t *g_fuvc_descriptors_array[] =
{
  (const uint8_t *)&g_fuvc_config_desc,
  (const uint8_t *)&g_fuvc_iad,

/*******************video control***************************/

  (const uint8_t *)&g_fuvc_vc_intf_desc,
  (const uint8_t *)&g_fuvc_vc_head_desc,
  (const uint8_t *)&g_fuvc_vc_camera,
  (const uint8_t *)&g_fuvc_proc_desc,
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_H264
  (const uint8_t *)&g_fuvc_h264_ext_desc,
#endif
  (const uint8_t *)&g_fuvc_camera_ext_desc,

  (const uint8_t *)&g_fuvc_vc_output_desc,

#if UVC_USE_CTRL_EP
  (const uint8_t *)&g_fuvc_ctrl_ep_desc,
#endif
#ifdef USB3_DEVICE_CONTROLLER
  (const uint8_t *)&g_fuvc_ctrl_ss_comp_desc,
#endif
  (const uint8_t *)&g_fuvc_int_ep_desc,

/*******************video stream***************************/

  (const uint8_t *)&g_fuvc_vs_intf_alt0,

  (const uint8_t *)&g_fuvc_vs_head_desc,

#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_YUYV
  /**************** yuyv format ***************/

  (const uint8_t *)&g_fuvc_format_yuv,
  (const uint8_t *)&g_fuvc_frame_yuv_144p,
  (const uint8_t *)&g_fuvc_frame_yuv_240p,
  (const uint8_t *)&g_fuvc_frame_yuv_288p,
  (const uint8_t *)&g_fuvc_frame_yuv_360p,
  (const uint8_t *)&g_fuvc_frame_yuv_480p,
  (const uint8_t *)&g_fuvc_frame_yuv_720p,
  (const uint8_t *)&g_fuvc_frame_yuv_1080p,
#endif

#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_NV12
  /**************** nv12 format ***************/

  (const uint8_t *)&g_fuvc_format_nv12,
  (const uint8_t *)&g_fuvc_frame_nv12_144p,
  (const uint8_t *)&g_fuvc_frame_nv12_240p,
  (const uint8_t *)&g_fuvc_frame_nv12_288p,
  (const uint8_t *)&g_fuvc_frame_nv12_360p,
  (const uint8_t *)&g_fuvc_frame_nv12_480p,
  (const uint8_t *)&g_fuvc_frame_nv12_720p,
  (const uint8_t *)&g_fuvc_frame_nv12_1080p,
#endif

#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_NV21
  /**************** nv21 format ***************/

  (const uint8_t *)&g_fuvc_format_nv21,
  (const uint8_t *)&g_fuvc_frame_nv21_144p,
  (const uint8_t *)&g_fuvc_frame_nv21_240p,
  (const uint8_t *)&g_fuvc_frame_nv21_288p,
  (const uint8_t *)&g_fuvc_frame_nv21_360p,
  (const uint8_t *)&g_fuvc_frame_nv21_480p,
  (const uint8_t *)&g_fuvc_frame_nv21_720p,
  (const uint8_t *)&g_fuvc_frame_nv21_1080p,
#endif

#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_MJPEG
   /**************** mjpeg format ***************/

  (const uint8_t *)&g_fuvc_format_mjpeg,
  (const uint8_t *)&g_fuvc_frame_mjpeg_144p,
  (const uint8_t *)&g_fuvc_frame_mjpeg_240p,
  (const uint8_t *)&g_fuvc_frame_mjpeg_288p,
  (const uint8_t *)&g_fuvc_frame_mjpeg_360p,
  (const uint8_t *)&g_fuvc_frame_mjpeg_480p,
  (const uint8_t *)&g_fuvc_frame_mjpeg_720p,
  (const uint8_t *)&g_fuvc_frame_mjpeg_1080p,
#endif

#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_H264
  /**************** h264 format ***************/

  (const uint8_t *)&g_fuvc_format_h264,
  (const uint8_t *)&g_fuvc_frame_h264_144p,
  (const uint8_t *)&g_fuvc_frame_h264_240p,
  (const uint8_t *)&g_fuvc_frame_h264_288p,
  (const uint8_t *)&g_fuvc_frame_h264_360p,
  (const uint8_t *)&g_fuvc_frame_h264_480p,
  (const uint8_t *)&g_fuvc_frame_h264_720p,
  (const uint8_t *)&g_fuvc_frame_h264_1080p,
#endif

#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_H265
  /**************** h265 format ***************/

  (const uint8_t *)&g_fuvc_format_h265,
  (const uint8_t *)&g_fuvc_frame_h265_144p,
  (const uint8_t *)&g_fuvc_frame_h265_240p,
  (const uint8_t *)&g_fuvc_frame_h265_288p,
  (const uint8_t *)&g_fuvc_frame_h265_360p,
  (const uint8_t *)&g_fuvc_frame_h265_480p,
  (const uint8_t *)&g_fuvc_frame_h265_720p,
  (const uint8_t *)&g_fuvc_frame_h265_1080p,
#endif

  /* Reserved for copying g_fuvc_descriptors_reserved_array. */

  NULL,
  NULL,
  NULL,
  NULL,
  NULL
};

static const uint8_t *g_fuvc_descriptors_reserved_array[] =
{
  (const uint8_t *)&g_fuvc_color_matching_descriptor,

#if FUVC_BULK_TO_ISO_CONVERT
  (const uint8_t *)&g_fuvc_isoc_streaming_intf_alt1,
  (const uint8_t *)&g_fuvc_hs_isoc_streaming_ep,
#ifdef USB3_DEVICE_CONTROLLER
  (const uint8_t *)&g_fuvc_isoc_streaming_ss_comp_desc,
#endif
#else
  (const uint8_t *)&g_fuvc_hs_bulk_streaming_ep,
#ifdef USB3_DEVICE_CONTROLLER
  (const uint8_t *)&g_fuvc_bulk_streaming_ss_comp_desc,
#endif
#endif

  NULL
};

#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_YUYV
static struct fuvc_frame_info g_fuvc_frames_yuyv[] =
{
  { 176,  144,  { 333333,  0 }, },
  { 320,  240,  { 333333,  0 }, },
  { 352,  288,  { 333333,  0 }, },
  { 640,  360,  { 333333,  0 }, },
  { 640,  480,  { 333333,  0 }, },
  { 1280, 720,  { 333333,  0 }, },
  { 1920, 1080, { 333333,  0 }, },
  { 0,    0,    { 0,         }, }
};
#endif

#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_NV12
static struct fuvc_frame_info g_fuvc_frames_nv12[] =
{
  { 176,  144,  { 333333,  0 }, },
  { 320,  240,  { 333333,  0 }, },
  { 352,  288,  { 333333,  0 }, },
  { 640,  360,  { 333333,  0 }, },
  { 640,  480,  { 333333,  0 }, },
  { 1280, 720,  { 333333,  0 }, },
  { 1920, 1080, { 333333,  0 }, },
  { 0,    0,    { 0,         }, }
};
#endif

#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_NV21
static struct fuvc_frame_info g_fuvc_frames_nv21[] =
{
  { 176,  144,  { 333333,  0 }, },
  { 320,  240,  { 333333,  0 }, },
  { 352,  288,  { 333333,  0 }, },
  { 640,  360,  { 333333,  0 }, },
  { 640,  480,  { 333333,  0 }, },
  { 1280, 720,  { 333333,  0 }, },
  { 1920, 1080, { 333333,  0 }, },
  { 0,    0,    { 0,         }, }
};
#endif

#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_MJPEG
static struct fuvc_frame_info g_fuvc_frames_mjpeg[] =
{
  { 176,  144,  { 333333,  0 }, },
  { 320,  240,  { 333333,  0 }, },
  { 352,  288,  { 333333,  0 }, },
  { 640,  360,  { 333333,  0 }, },
  { 640,  480,  { 333333,  0 }, },
  { 1280, 720,  { 333333,  0 }, },
  { 1920, 1080, { 333333,  0 }, },
  { 0,    0,    { 0,         }, }
};
#endif

#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_H264
static struct fuvc_frame_info g_fuvc_frames_h264[] =
{
  { 176,  144,  { 333333,  0 }, },
  { 320,  240,  { 333333,  0 }, },
  { 352,  288,  { 333333,  0 }, },
  { 640,  360,  { 333333,  0 }, },
  { 640,  480,  { 333333,  0 }, },
  { 1280, 720,  { 333333,  0 }, },
  { 1920, 1080, { 333333,  0 }, },
  { 0,    0,    { 0,         }, }
};
#endif

#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_H265
static struct fuvc_frame_info g_fuvc_frames_h265[] =
{
  { 176,  144,  { 333333,  0 }, },
  { 320,  240,  { 333333,  0 }, },
  { 352,  288,  { 333333,  0 }, },
  { 640,  360,  { 333333,  0 }, },
  { 640,  480,  { 333333,  0 }, },
  { 1280, 720,  { 333333,  0 }, },
  { 1920, 1080, { 333333,  0 }, },
  { 0,    0,    { 0,         }, }
};
#endif

static struct fuvc_format_info g_fuvc_formats[] =
{
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_YUYV
  { V4L2_PIX_FMT_YUYV, g_fuvc_frames_yuyv },
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_NV12
  { V4L2_PIX_FMT_NV12, g_fuvc_frames_nv12 },
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_NV21
  { V4L2_PIX_FMT_NV21, g_fuvc_frames_nv21 },
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_MJPEG
  { V4L2_PIX_FMT_MJPEG, g_fuvc_frames_mjpeg },
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_H264
  { V4L2_PIX_FMT_H264, g_fuvc_frames_h264 },
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_H265
  { V4L2_PIX_FMT_H265, g_fuvc_frames_h265 }
#endif
};

bool uvc_open_format_check(const struct uvc_open_param *param)
{
  switch (param->vid_format)
    {
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_YUYV
    case UVC_VFF_YUY2:
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_NV12
    case UVC_VFF_NV12:
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_NV21
    case UVC_VFF_NV21:
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_MJPEG
    case UVC_VFF_MJPEG:
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_H264
    case UVC_VFF_H264:
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_H265
    case UVC_VFF_H265:
#endif
      return true;

    default:
      return false;
    }
}

bool uvc_format_info_check(const struct uvc_format_info *format_info)
{
  switch (format_info->format)
    {
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_YUYV
    case V4L2_PIX_FMT_YUYV:
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_NV12
    case V4L2_PIX_FMT_NV12:
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_NV21
    case V4L2_PIX_FMT_NV21:
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_MJPEG
    case V4L2_PIX_FMT_MJPEG:
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_H264
    case V4L2_PIX_FMT_H264:
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_H265
    case V4L2_PIX_FMT_H265:
#endif
      return true;

    default:
      return false;
    }
}

void fuvc_frame_descriptors_get_sub(const struct fuvc_format_info *format_info, uint32_t frames_count,
                                    struct fuvc_frame_info *frame_info, uint32_t frames)
{
  errno_t err;
  uint32_t flag;
  uint32_t frame_info_size = sizeof(struct fuvc_frame_info) * frames;

  if (frames_count >= frames)
    {
      usb_err("Frames_count err \n");
      return;
    }

  spin_lock_irqsave(&g_uvc_frame_spinlock, flag);
  (void)memset_s(frame_info, frame_info_size, 0, frame_info_size);
  err = memcpy_s(frame_info, frame_info_size,
                 format_info->frames, (sizeof(struct fuvc_frame_info) * frames_count));
  spin_unlock_irqrestore(&g_uvc_frame_spinlock, flag);
  if (err != EOK)
    {
      usb_err("Memcpy fail! \n");
      return;
    }
}

void fuvc_frame_descriptors_get(const struct fuvc_format_info *format_info, uint32_t frames_count)
{
  uint32_t frames;

  if (format_info == NULL || format_info->frames == NULL || frames_count == 0)
    {
      usb_err("Format_info or frames_count err\n");
      return;
    }

  switch (format_info->fcc)
    {
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_YUYV
    case V4L2_PIX_FMT_YUYV:
      frames = ARRAY_SIZE(g_fuvc_frames_yuyv);
      fuvc_frame_descriptors_get_sub(format_info, frames_count, g_fuvc_frames_yuyv, frames);
      break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_NV12
    case V4L2_PIX_FMT_NV12:
      frames = ARRAY_SIZE(g_fuvc_frames_nv12);
      fuvc_frame_descriptors_get_sub(format_info, frames_count, g_fuvc_frames_nv12, frames);
      break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_NV21
    case V4L2_PIX_FMT_NV21:
      frames = ARRAY_SIZE(g_fuvc_frames_nv21);
      fuvc_frame_descriptors_get_sub(format_info, frames_count, g_fuvc_frames_nv21, frames);
      break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_MJPEG
    case V4L2_PIX_FMT_MJPEG:
      frames = ARRAY_SIZE(g_fuvc_frames_mjpeg);
      fuvc_frame_descriptors_get_sub(format_info, frames_count, g_fuvc_frames_mjpeg, frames);
      break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_H264
    case V4L2_PIX_FMT_H264:
      frames = ARRAY_SIZE(g_fuvc_frames_h264);
      fuvc_frame_descriptors_get_sub(format_info, frames_count, g_fuvc_frames_h264, frames);
      break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_H265
    case V4L2_PIX_FMT_H265:
      frames = ARRAY_SIZE(g_fuvc_frames_h265);
      fuvc_frame_descriptors_get_sub(format_info, frames_count, g_fuvc_frames_h265, frames);
      break;
#endif

    default:
      usb_err("Unknown format\n");
      break;
    }
}

static void frames_descriptors_update(const struct fuvc_format_info *format_info, uint32_t *index)
{
  uint32_t num = *index;
  uint8_t frames = 0;
  const struct fuvc_format_info *format = format_info;

  while (format->frames[frames].height != 0)
    {
      switch (format->frames[frames].height)
        {
        case UVC_FRAME_WIDTH_144:
          switch (format->fcc)
            {
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_YUYV
            case V4L2_PIX_FMT_YUYV:
              g_fuvc_frame_yuv_144p.dwFrameInterval[0]     = format->frames[frames].intervals[0];
              g_fuvc_frame_yuv_144p.dwDefaultFrameInterval = format->frames[frames].intervals[0];
              g_fuvc_frame_yuv_144p.bFrameIndex            = frames + 1;
              g_fuvc_descriptors_array[num++]              = (const uint8_t *)&g_fuvc_frame_yuv_144p;
              break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_NV12
            case V4L2_PIX_FMT_NV12:
              g_fuvc_frame_nv12_144p.dwFrameInterval[0]     = format->frames[frames].intervals[0];
              g_fuvc_frame_nv12_144p.dwDefaultFrameInterval = format->frames[frames].intervals[0];
              g_fuvc_frame_nv12_144p.bFrameIndex            = frames + 1;
              g_fuvc_descriptors_array[num++]               = (const uint8_t *)&g_fuvc_frame_nv12_144p;
              break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_NV21
            case V4L2_PIX_FMT_NV21:
              g_fuvc_frame_nv21_144p.dwFrameInterval[0]     = format->frames[frames].intervals[0];
              g_fuvc_frame_nv21_144p.dwDefaultFrameInterval = format->frames[frames].intervals[0];
              g_fuvc_frame_nv21_144p.bFrameIndex            = frames + 1;
              g_fuvc_descriptors_array[num++]               = (const uint8_t *)&g_fuvc_frame_nv21_144p;
              break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_MJPEG
            case V4L2_PIX_FMT_MJPEG:
              g_fuvc_frame_mjpeg_144p.dwFrameInterval[0]     = format->frames[frames].intervals[0];
              g_fuvc_frame_mjpeg_144p.dwDefaultFrameInterval = format->frames[frames].intervals[0];
              g_fuvc_frame_mjpeg_144p.bFrameIndex            = frames + 1;
              g_fuvc_descriptors_array[num++]                = (const uint8_t *)&g_fuvc_frame_mjpeg_144p;
              break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_H264
            case V4L2_PIX_FMT_H264:
              g_fuvc_frame_h264_144p.frameinterval[0] = format->frames[frames].intervals[0];
              g_fuvc_frame_h264_144p.dframeinterval   = format->frames[frames].intervals[0];
              g_fuvc_frame_h264_144p.frameindex       = frames + 1;
              g_fuvc_descriptors_array[num++]         = (const uint8_t *)&g_fuvc_frame_h264_144p;
              break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_H265
            case V4L2_PIX_FMT_H265:
              g_fuvc_frame_h265_144p.frameinterval[0] = format->frames[frames].intervals[0];
              g_fuvc_frame_h265_144p.dframeinterval   = format->frames[frames].intervals[0];
              g_fuvc_frame_h265_144p.frameindex       = frames + 1;
              g_fuvc_descriptors_array[num++]         = (const uint8_t *)&g_fuvc_frame_h265_144p;
              break;
#endif

            default:
              break;
            }
          break;

        case UVC_FRAME_WIDTH_240:
          switch (format->fcc)
            {
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_YUYV
            case V4L2_PIX_FMT_YUYV:
              g_fuvc_frame_yuv_240p.dwFrameInterval[0]     = format->frames[frames].intervals[0];
              g_fuvc_frame_yuv_240p.dwDefaultFrameInterval = format->frames[frames].intervals[0];
              g_fuvc_frame_yuv_240p.bFrameIndex            = frames + 1;
              g_fuvc_descriptors_array[num++]              = (const uint8_t *)&g_fuvc_frame_yuv_240p;
              break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_NV12
            case V4L2_PIX_FMT_NV12:
              g_fuvc_frame_nv12_240p.dwFrameInterval[0]     = format->frames[frames].intervals[0];
              g_fuvc_frame_nv12_240p.dwDefaultFrameInterval = format->frames[frames].intervals[0];
              g_fuvc_frame_nv12_240p.bFrameIndex            = frames + 1;
              g_fuvc_descriptors_array[num++]               = (const uint8_t *)&g_fuvc_frame_nv12_240p;
              break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_NV21
            case V4L2_PIX_FMT_NV21:
              g_fuvc_frame_nv21_240p.dwFrameInterval[0]     = format->frames[frames].intervals[0];
              g_fuvc_frame_nv21_240p.dwDefaultFrameInterval = format->frames[frames].intervals[0];
              g_fuvc_frame_nv21_240p.bFrameIndex            = frames + 1;
              g_fuvc_descriptors_array[num++]               = (const uint8_t *)&g_fuvc_frame_nv21_240p;
              break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_MJPEG
            case V4L2_PIX_FMT_MJPEG:
              g_fuvc_frame_mjpeg_240p.dwFrameInterval[0]     = format->frames[frames].intervals[0];
              g_fuvc_frame_mjpeg_240p.dwDefaultFrameInterval = format->frames[frames].intervals[0];
              g_fuvc_frame_mjpeg_240p.bFrameIndex            = frames + 1;
              g_fuvc_descriptors_array[num++]                = (const uint8_t *)&g_fuvc_frame_mjpeg_240p;
              break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_H264
            case V4L2_PIX_FMT_H264:
              g_fuvc_frame_h264_240p.frameinterval[0] = format->frames[frames].intervals[0];
              g_fuvc_frame_h264_240p.dframeinterval   = format->frames[frames].intervals[0];
              g_fuvc_frame_h264_240p.frameindex       = frames + 1;
              g_fuvc_descriptors_array[num++]         = (const uint8_t *)&g_fuvc_frame_h264_240p;
              break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_H265
            case V4L2_PIX_FMT_H265:
              g_fuvc_frame_h265_240p.frameinterval[0] = format->frames[frames].intervals[0];
              g_fuvc_frame_h265_240p.dframeinterval   = format->frames[frames].intervals[0];
              g_fuvc_frame_h265_240p.frameindex       = frames + 1;
              g_fuvc_descriptors_array[num++]         = (const uint8_t *)&g_fuvc_frame_h265_240p;
              break;
#endif

            default:
              break;
            }
          break;

        case UVC_FRAME_WIDTH_288:
          switch (format->fcc)
            {
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_YUYV
            case V4L2_PIX_FMT_YUYV:
              g_fuvc_frame_yuv_288p.dwFrameInterval[0]     = format->frames[frames].intervals[0];
              g_fuvc_frame_yuv_288p.dwDefaultFrameInterval = format->frames[frames].intervals[0];
              g_fuvc_frame_yuv_288p.bFrameIndex            = frames + 1;
              g_fuvc_descriptors_array[num++]              = (const uint8_t *)&g_fuvc_frame_yuv_288p;
              break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_NV12
            case V4L2_PIX_FMT_NV12:
              g_fuvc_frame_nv12_288p.dwFrameInterval[0]     = format->frames[frames].intervals[0];
              g_fuvc_frame_nv12_288p.dwDefaultFrameInterval = format->frames[frames].intervals[0];
              g_fuvc_frame_nv12_288p.bFrameIndex            = frames + 1;
              g_fuvc_descriptors_array[num++]               = (const uint8_t *)&g_fuvc_frame_nv12_288p;
              break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_NV21
            case V4L2_PIX_FMT_NV21:
              g_fuvc_frame_nv21_288p.dwFrameInterval[0]     = format->frames[frames].intervals[0];
              g_fuvc_frame_nv21_288p.dwDefaultFrameInterval = format->frames[frames].intervals[0];
              g_fuvc_frame_nv21_288p.bFrameIndex            = frames + 1;
              g_fuvc_descriptors_array[num++]               = (const uint8_t *)&g_fuvc_frame_nv21_288p;
              break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_MJPEG
            case V4L2_PIX_FMT_MJPEG:
              g_fuvc_frame_mjpeg_288p.dwFrameInterval[0]     = format->frames[frames].intervals[0];
              g_fuvc_frame_mjpeg_288p.dwDefaultFrameInterval = format->frames[frames].intervals[0];
              g_fuvc_frame_mjpeg_288p.bFrameIndex            = frames + 1;
              g_fuvc_descriptors_array[num++]                = (const uint8_t *)&g_fuvc_frame_mjpeg_288p;
              break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_H264
            case V4L2_PIX_FMT_H264:
              g_fuvc_frame_h264_288p.frameinterval[0] = format->frames[frames].intervals[0];
              g_fuvc_frame_h264_288p.dframeinterval   = format->frames[frames].intervals[0];
              g_fuvc_frame_h264_288p.frameindex       = frames + 1;
              g_fuvc_descriptors_array[num++]         = (const uint8_t *)&g_fuvc_frame_h264_288p;
              break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_H265
            case V4L2_PIX_FMT_H265:
              g_fuvc_frame_h265_288p.frameinterval[0] = format->frames[frames].intervals[0];
              g_fuvc_frame_h265_288p.dframeinterval   = format->frames[frames].intervals[0];
              g_fuvc_frame_h265_288p.frameindex       = frames + 1;
              g_fuvc_descriptors_array[num++]         = (const uint8_t *)&g_fuvc_frame_h265_288p;
              break;
#endif

            default:
              break;
            }
          break;

        case UVC_FRAME_WIDTH_360:
          switch (format->fcc)
            {
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_YUYV
            case V4L2_PIX_FMT_YUYV:
              g_fuvc_frame_yuv_360p.dwFrameInterval[0]     = format->frames[frames].intervals[0];
              g_fuvc_frame_yuv_360p.dwDefaultFrameInterval = format->frames[frames].intervals[0];
              g_fuvc_frame_yuv_360p.bFrameIndex            = frames + 1;
              g_fuvc_descriptors_array[num++]              = (const uint8_t *)&g_fuvc_frame_yuv_360p;
              break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_NV12
            case V4L2_PIX_FMT_NV12:
              g_fuvc_frame_nv12_360p.dwFrameInterval[0]     = format->frames[frames].intervals[0];
              g_fuvc_frame_nv12_360p.dwDefaultFrameInterval = format->frames[frames].intervals[0];
              g_fuvc_frame_nv12_360p.bFrameIndex            = frames + 1;
              g_fuvc_descriptors_array[num++]               = (const uint8_t *)&g_fuvc_frame_nv12_360p;
              break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_NV21
            case V4L2_PIX_FMT_NV21:
              g_fuvc_frame_nv21_360p.dwFrameInterval[0]     = format->frames[frames].intervals[0];
              g_fuvc_frame_nv21_360p.dwDefaultFrameInterval = format->frames[frames].intervals[0];
              g_fuvc_frame_nv21_360p.bFrameIndex            = frames + 1;
              g_fuvc_descriptors_array[num++]               = (const uint8_t *)&g_fuvc_frame_nv21_360p;
              break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_MJPEG
            case V4L2_PIX_FMT_MJPEG:
              g_fuvc_frame_mjpeg_360p.dwFrameInterval[0]     = format->frames[frames].intervals[0];
              g_fuvc_frame_mjpeg_360p.dwDefaultFrameInterval = format->frames[frames].intervals[0];
              g_fuvc_frame_mjpeg_360p.bFrameIndex            = frames + 1;
              g_fuvc_descriptors_array[num++]                = (const uint8_t *)&g_fuvc_frame_mjpeg_360p;
              break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_H264
            case V4L2_PIX_FMT_H264:
              g_fuvc_frame_h264_360p.frameinterval[0] = format->frames[frames].intervals[0];
              g_fuvc_frame_h264_360p.dframeinterval   = format->frames[frames].intervals[0];
              g_fuvc_frame_h264_360p.frameindex       = frames + 1;
              g_fuvc_descriptors_array[num++]         = (const uint8_t *)&g_fuvc_frame_h264_360p;
              break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_H265
            case V4L2_PIX_FMT_H265:
              g_fuvc_frame_h265_360p.frameinterval[0] = format->frames[frames].intervals[0];
              g_fuvc_frame_h265_360p.dframeinterval   = format->frames[frames].intervals[0];
              g_fuvc_frame_h265_360p.frameindex       = frames + 1;
              g_fuvc_descriptors_array[num++]         = (const uint8_t *)&g_fuvc_frame_h265_360p;
              break;
#endif

            default:
              break;
            }
          break;

        case UVC_FRAME_WIDTH_480:
          switch (format->fcc)
            {
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_YUYV
            case V4L2_PIX_FMT_YUYV:
              g_fuvc_frame_yuv_480p.dwFrameInterval[0]     = format->frames[frames].intervals[0];
              g_fuvc_frame_yuv_480p.dwDefaultFrameInterval = format->frames[frames].intervals[0];
              g_fuvc_frame_yuv_480p.bFrameIndex            = frames + 1;
              g_fuvc_descriptors_array[num++]              = (const uint8_t *)&g_fuvc_frame_yuv_480p;
              break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_NV12
            case V4L2_PIX_FMT_NV12:
              g_fuvc_frame_nv12_480p.dwFrameInterval[0]     = format->frames[frames].intervals[0];
              g_fuvc_frame_nv12_480p.dwDefaultFrameInterval = format->frames[frames].intervals[0];
              g_fuvc_frame_nv12_480p.bFrameIndex            = frames + 1;
              g_fuvc_descriptors_array[num++]               = (const uint8_t *)&g_fuvc_frame_nv12_480p;
              break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_NV21
            case V4L2_PIX_FMT_NV21:
              g_fuvc_frame_nv21_480p.dwFrameInterval[0]     = format->frames[frames].intervals[0];
              g_fuvc_frame_nv21_480p.dwDefaultFrameInterval = format->frames[frames].intervals[0];
              g_fuvc_frame_nv21_480p.bFrameIndex            = frames + 1;
              g_fuvc_descriptors_array[num++]               = (const uint8_t *)&g_fuvc_frame_nv21_480p;
              break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_MJPEG
            case V4L2_PIX_FMT_MJPEG:
              g_fuvc_frame_mjpeg_480p.dwFrameInterval[0]     = format->frames[frames].intervals[0];
              g_fuvc_frame_mjpeg_480p.dwDefaultFrameInterval = format->frames[frames].intervals[0];
              g_fuvc_frame_mjpeg_480p.bFrameIndex            = frames + 1;
              g_fuvc_descriptors_array[num++]                = (const uint8_t *)&g_fuvc_frame_mjpeg_480p;
              break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_H264
            case V4L2_PIX_FMT_H264:
              g_fuvc_frame_h264_480p.frameinterval[0] = format->frames[frames].intervals[0];
              g_fuvc_frame_h264_480p.dframeinterval   = format->frames[frames].intervals[0];
              g_fuvc_frame_h264_480p.frameindex       = frames + 1;
              g_fuvc_descriptors_array[num++]         = (const uint8_t *)&g_fuvc_frame_h264_480p;
              break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_H265
            case V4L2_PIX_FMT_H265:
              g_fuvc_frame_h265_480p.frameinterval[0] = format->frames[frames].intervals[0];
              g_fuvc_frame_h265_480p.dframeinterval   = format->frames[frames].intervals[0];
              g_fuvc_frame_h265_480p.frameindex       = frames + 1;
              g_fuvc_descriptors_array[num++]         = (const uint8_t *)&g_fuvc_frame_h265_480p;
              break;
#endif

            default:
              break;
            }
          break;

        case UVC_FRAME_WIDTH_720:
          switch (format->fcc)
            {
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_YUYV
            case V4L2_PIX_FMT_YUYV:
              g_fuvc_frame_yuv_720p.dwFrameInterval[0]     = format->frames[frames].intervals[0];
              g_fuvc_frame_yuv_720p.dwDefaultFrameInterval = format->frames[frames].intervals[0];
              g_fuvc_frame_yuv_720p.bFrameIndex            = frames + 1;
              g_fuvc_descriptors_array[num++]              = (const uint8_t *)&g_fuvc_frame_yuv_720p;
              break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_NV12
            case V4L2_PIX_FMT_NV12:
              g_fuvc_frame_nv12_720p.dwFrameInterval[0]     = format->frames[frames].intervals[0];
              g_fuvc_frame_nv12_720p.dwDefaultFrameInterval = format->frames[frames].intervals[0];
              g_fuvc_frame_nv12_720p.bFrameIndex            = frames + 1;
              g_fuvc_descriptors_array[num++]               = (const uint8_t *)&g_fuvc_frame_nv12_720p;
              break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_NV21
            case V4L2_PIX_FMT_NV21:
              g_fuvc_frame_nv21_720p.dwFrameInterval[0]     = format->frames[frames].intervals[0];
              g_fuvc_frame_nv21_720p.dwDefaultFrameInterval = format->frames[frames].intervals[0];
              g_fuvc_frame_nv21_720p.bFrameIndex            = frames + 1;
              g_fuvc_descriptors_array[num++]               = (const uint8_t *)&g_fuvc_frame_nv21_720p;
              break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_MJPEG
            case V4L2_PIX_FMT_MJPEG:
              g_fuvc_frame_mjpeg_720p.dwFrameInterval[0]     = format->frames[frames].intervals[0];
              g_fuvc_frame_mjpeg_720p.dwDefaultFrameInterval = format->frames[frames].intervals[0];
              g_fuvc_frame_mjpeg_720p.bFrameIndex            = frames + 1;
              g_fuvc_descriptors_array[num++]                = (const uint8_t *)&g_fuvc_frame_mjpeg_720p;
              break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_H264
            case V4L2_PIX_FMT_H264:
              g_fuvc_frame_h264_720p.frameinterval[0] = format->frames[frames].intervals[0];
              g_fuvc_frame_h264_720p.dframeinterval   = format->frames[frames].intervals[0];
              g_fuvc_frame_h264_720p.frameindex       = frames + 1;
              g_fuvc_descriptors_array[num++]         = (const uint8_t *)&g_fuvc_frame_h264_720p;
              break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_H265
            case V4L2_PIX_FMT_H265:
              g_fuvc_frame_h265_720p.frameinterval[0] = format->frames[frames].intervals[0];
              g_fuvc_frame_h265_720p.dframeinterval   = format->frames[frames].intervals[0];
              g_fuvc_frame_h265_720p.frameindex       = frames + 1;
              g_fuvc_descriptors_array[num++]         = (const uint8_t *)&g_fuvc_frame_h265_720p;
              break;
#endif

            default:
              break;
            }
          break;

        case UVC_FRAME_WIDTH_1080:
          switch (format->fcc)
            {
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_YUYV
            case V4L2_PIX_FMT_YUYV:
              g_fuvc_frame_yuv_1080p.dwFrameInterval[0]     = format->frames[frames].intervals[0];
              g_fuvc_frame_yuv_1080p.dwDefaultFrameInterval = format->frames[frames].intervals[0];
              g_fuvc_frame_yuv_1080p.bFrameIndex            = frames + 1;
              g_fuvc_descriptors_array[num++]               = (const uint8_t *)&g_fuvc_frame_yuv_1080p;
              break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_NV12
            case V4L2_PIX_FMT_NV12:
              g_fuvc_frame_nv12_1080p.dwFrameInterval[0]     = format->frames[frames].intervals[0];
              g_fuvc_frame_nv12_1080p.dwDefaultFrameInterval = format->frames[frames].intervals[0];
              g_fuvc_frame_nv12_1080p.bFrameIndex            = frames + 1;
              g_fuvc_descriptors_array[num++]                = (const uint8_t *)&g_fuvc_frame_nv12_1080p;
              break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_NV21
            case V4L2_PIX_FMT_NV21:
              g_fuvc_frame_nv21_1080p.dwFrameInterval[0]     = format->frames[frames].intervals[0];
              g_fuvc_frame_nv21_1080p.dwDefaultFrameInterval = format->frames[frames].intervals[0];
              g_fuvc_frame_nv21_1080p.bFrameIndex            = frames + 1;
              g_fuvc_descriptors_array[num++]                = (const uint8_t *)&g_fuvc_frame_nv21_1080p;
              break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_MJPEG
            case V4L2_PIX_FMT_MJPEG:
              g_fuvc_frame_mjpeg_1080p.dwFrameInterval[0]     = format->frames[frames].intervals[0];
              g_fuvc_frame_mjpeg_1080p.dwDefaultFrameInterval = format->frames[frames].intervals[0];
              g_fuvc_frame_mjpeg_1080p.bFrameIndex            = frames + 1;
              g_fuvc_descriptors_array[num++]                 = (const uint8_t *)&g_fuvc_frame_mjpeg_1080p;
              break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_H264
            case V4L2_PIX_FMT_H264:
              g_fuvc_frame_h264_1080p.frameinterval[0] = format->frames[frames].intervals[0];
              g_fuvc_frame_h264_1080p.dframeinterval   = format->frames[frames].intervals[0];
              g_fuvc_frame_h264_1080p.frameindex       = frames + 1;
              g_fuvc_descriptors_array[num++]          = (const uint8_t *)&g_fuvc_frame_h264_1080p;
              break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_H265
            case V4L2_PIX_FMT_H265:
              g_fuvc_frame_h265_1080p.frameinterval[0] = format->frames[frames].intervals[0];
              g_fuvc_frame_h265_1080p.dframeinterval   = format->frames[frames].intervals[0];
              g_fuvc_frame_h265_1080p.frameindex       = frames + 1;
              g_fuvc_descriptors_array[num++]          = (const uint8_t *)&g_fuvc_frame_h265_1080p;
              break;
#endif

            default:
              break;
            }
          break;

        default:
          usb_err("Unknown resolution\n");
        }

      frames++;
    }

  *index = num;
}

static void fuvc_descriptors_update(void)
{
  uint32_t index = 0;
  uint32_t desc_array_size;
  uint32_t reserved_array_size;
  uint32_t format_end_index;
  uint8_t frames;
  uint8_t format_index = 0;
  struct fuvc_format_info *format;

  while (g_fuvc_descriptors_array[index] != (const uint8_t *)&g_fuvc_vs_head_desc)
    {
      index++;
    }

  index++;
  desc_array_size = ARRAY_SIZE(g_fuvc_descriptors_array);
  reserved_array_size = ARRAY_SIZE(g_fuvc_descriptors_reserved_array);
  format_end_index = desc_array_size - reserved_array_size;

#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_YUYV
  /**************** yuyv format ***************/

  g_format_index_map[format_index] = UVC_VFF_YUY2;
  frames = 0;
  format = &g_fuvc_formats[UVC_VFF_YUY2];
  while (format->frames[frames].width != 0)
    {
      ++frames;
    }
  g_fuvc_format_yuv.bNumFrameDescriptors = frames;
  g_fuvc_format_yuv.bFormatIndex         = ++format_index;

  g_fuvc_descriptors_array[index++] = (const uint8_t *)&g_fuvc_format_yuv;

  frames_descriptors_update(format, &index);
#endif

#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_NV12
  /**************** nv12 format ***************/

  g_format_index_map[format_index] = UVC_VFF_NV12;
  frames = 0;
  format = &g_fuvc_formats[UVC_VFF_NV12];
  while (format->frames[frames].width != 0)
    {
      ++frames;
    }
  g_fuvc_format_nv12.bNumFrameDescriptors = frames;
  g_fuvc_format_nv12.bFormatIndex         = ++format_index;

  g_fuvc_descriptors_array[index++] = (const uint8_t *)&g_fuvc_format_nv12;

  frames_descriptors_update(format, &index);
#endif

#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_NV21
  /**************** nv21 format ***************/

  g_format_index_map[format_index] = UVC_VFF_NV21;
  frames = 0;
  format = &g_fuvc_formats[UVC_VFF_NV21];
  while (format->frames[frames].width != 0)
    {
      ++frames;
    }
  g_fuvc_format_nv21.bNumFrameDescriptors = frames;
  g_fuvc_format_nv21.bFormatIndex         = ++format_index;

  g_fuvc_descriptors_array[index++] = (const uint8_t *)&g_fuvc_format_nv21;

  frames_descriptors_update(format, &index);
#endif

#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_MJPEG
  /**************** mjpeg format ***************/

  g_format_index_map[format_index] = UVC_VFF_MJPEG;
  frames = 0;
  format = &g_fuvc_formats[UVC_VFF_MJPEG];
  while (format->frames[frames].width != 0)
    {
      ++frames;
    }
  g_fuvc_format_mjpeg.bNumFrameDescriptors = frames;
  g_fuvc_format_mjpeg.bFormatIndex         = ++format_index;

  g_fuvc_descriptors_array[index++] = (const uint8_t *)&g_fuvc_format_mjpeg;

  frames_descriptors_update(format, &index);
#endif

#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_H264
  /**************** h264 format ***************/

  g_format_index_map[format_index] = UVC_VFF_H264;
  frames = 0;
  format = &g_fuvc_formats[UVC_VFF_H264];
  while (format->frames[frames].width != 0)
    {
      ++frames;
    }
  g_fuvc_format_h264.numframedesc = frames;
  g_fuvc_format_h264.formatindex  = ++format_index;

  g_fuvc_descriptors_array[index++] = (const uint8_t *)&g_fuvc_format_h264;

  frames_descriptors_update(format, &index);
#endif

#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_H265
  /**************** h265 format ***************/

  g_format_index_map[format_index] = UVC_VFF_H265;
  frames = 0;
  format = &g_fuvc_formats[UVC_VFF_H265];
  while (format->frames[frames].width != 0)
    {
      ++frames;
    }
  g_fuvc_format_h265.numframedesc = frames;
  g_fuvc_format_h265.formatindex  = ++format_index;

  g_fuvc_descriptors_array[index++] = (const uint8_t *)&g_fuvc_format_h265;

  frames_descriptors_update(format, &index);
#endif

  if (index > format_end_index)
    {
      usb_err("Updated frame descriptor exceeds supported number\n");
      return;
    }

  for (uint8_t i = 0; i < reserved_array_size; i++)
    {
      g_fuvc_descriptors_array[index++] = g_fuvc_descriptors_reserved_array[i];
    }
}

/* Process the descriptors, link them together */

static uint8_t *link_fuvc_descriptors(uint16_t *total_size)
{
  uint32_t i;
  uint32_t index = 0;
  uint32_t flag;
  uint16_t temp_des_len;
  uint16_t cs;
  uint16_t ds = 0;
  errno_t err;
  uint8_t *des;
  uint8_t *temp_des;
  const uint8_t *fuvc_desc;

  spin_lock_irqsave(&g_uvc_frame_spinlock, flag);
  fuvc_descriptors_update();
  spin_unlock_irqrestore(&g_uvc_frame_spinlock, flag);

  /* Add the length of descriptors one by one */

  for (i = 0; g_fuvc_descriptors_array[i] != NULL; ++i)
    {
      if (g_fuvc_descriptors_array[i] == (const uint8_t *)&g_fuvc_vs_head_desc)
        {
          index = i;
        }
      ds += (uint16_t)(*g_fuvc_descriptors_array[i]);
    }

  FUVC_DEBUG("Total Length of descriptors: %u i=%u\n", ds, i);
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

  /* configuration descriptor needs to have the full length of rest of descriptors */

  g_fuvc_config_desc.totallen[0] = (uint8_t)ds;
  g_fuvc_config_desc.totallen[1] = (uint8_t)(ds >> 8);

  /* Add the length of video control descriptors */

  cs  = (uint16_t)g_fuvc_vc_head_desc.bLength;
  cs += (uint16_t)g_fuvc_vc_camera.len;
  cs += (uint16_t)g_fuvc_proc_desc.len;
  cs += (uint16_t)g_fuvc_vc_output_desc.len;

#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_H264
  cs += (uint16_t)g_fuvc_h264_ext_desc.len;
#endif
  cs += (uint16_t)g_fuvc_camera_ext_desc.len;
  g_fuvc_vc_head_desc.wTotalLength = cs;

  ds = 0;
#if FUVC_BULK_TO_ISO_CONVERT
  fuvc_desc = (const uint8_t *)&g_fuvc_isoc_streaming_intf_alt1;
  g_fuvc_vs_head_desc.bEndpointAddress = g_fuvc_hs_isoc_streaming_ep.addr;
#else
  fuvc_desc = (const uint8_t *)&g_fuvc_hs_bulk_streaming_ep;
  g_fuvc_vs_head_desc.bEndpointAddress = g_fuvc_hs_bulk_streaming_ep.addr;
#endif
  for (i = index; g_fuvc_descriptors_array[i] != fuvc_desc; i++)
    {
      ds += (uint16_t)*(g_fuvc_descriptors_array[i]);
    }

  g_fuvc_vs_head_desc.wTotalLength = ds;

  for (i = 0; g_fuvc_descriptors_array[i] != NULL; i++)
    {
      const uint8_t *des_src = g_fuvc_descriptors_array[i];
      uint8_t des_len = *des_src;
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

  return des;
}

static void set_probe_status(struct uvc_dev_s *fuvc, int cs, int req)
{
  if (cs == 0x01)
    {
      switch (req)
        {
        case 0x01:
          fuvc->probe_status.set = 1;
          break;

        case 0x81:
          fuvc->probe_status.get = 1;
          break;

        case 0x82:
          fuvc->probe_status.min = 1;
          break;

        case 0x83:
          fuvc->probe_status.max = 1;
          break;

        default:
          break;
        }
    }
}

static int check_probe_status(struct uvc_dev_s *fuvc)
{
  /* Media negotiation stage, in order to be compatible with more usb hosts */

  if (fuvc->probe_status.get == 1 && fuvc->probe_status.set == 1)
    {
      return 1;
    }

  return 0;
}

static void fuvc_streaming_set(struct uvc_dev_s *fuvc,
                               const struct usbdev_req_s *req,
                               const struct usb_ctrlreq_s *ctrl)
{
  struct uvc_probe_commit_control_dev *target;
  struct uvc_probe_commit_control_dev com_tmp;
  struct uvc_probe_commit_control_dev *com;
  const struct fuvc_format_info *format;
  const struct fuvc_frame_info *frame;
  const uint32_t *interval;
  uint8_t temp_index, format_index, frame_index, frames;

  (void)ctrl;

  switch (fuvc->control)
    {
    case USB_UVC_VS_PROBE_CONTROL:
      if (fuvc->com_flag == 0)
        {
          return;
        }
      fuvc->com_flag = 0;

      target = &fuvc->probe;
      break;

    case USB_UVC_VS_COMMIT_CONTROL:
      target = &fuvc->commit;
      break;

    default:
      usb_err("setting unknown control\n");
      return;
    }

  com = (struct uvc_probe_commit_control_dev *)(req->buf);
  com_tmp.bFormatIndex    = com->bFormatIndex;
  com_tmp.bFrameIndex     = com->bFrameIndex;
  com_tmp.dwFrameInterval = com->dwFrameInterval;

  temp_index = CLAMP((uint32_t)com_tmp.bFormatIndex, 1U, (uint32_t)ARRAY_SIZE(g_fuvc_formats));
  format_index = g_format_index_map[temp_index - 1];

  format = &g_fuvc_formats[format_index];

  frames = 0;

  while (format->frames[frames].width != 0)
    {
      ++frames;
    }

  frame_index = CLAMP((uint32_t)com_tmp.bFrameIndex, 1U, (uint32_t)frames);
  frame       = &format->frames[frame_index - 1];
  interval    = frame->intervals;

  while (interval[0] < com_tmp.dwFrameInterval && interval[1])
    {
      ++interval;
    }

  /* Obtains the index value sent from the host. Verify at the third media negotiation.*/

  target->bFormatIndex = temp_index;
  target->bFrameIndex  = frame_index;

  switch (format->fcc)
    {
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_YUYV
    case V4L2_PIX_FMT_YUYV:
      target->dwMaxVideoFrameSize = frame->width * frame->height * YUYV_FRAME_FACTOR;
      break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_NV12
    case V4L2_PIX_FMT_NV12:
      target->dwMaxVideoFrameSize = frame->width * frame->height * NV12_FRAME_FACTOR;
      break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_NV21
    case V4L2_PIX_FMT_NV21:
      target->dwMaxVideoFrameSize = frame->width * frame->height * NV21_FRAME_FACTOR;
      break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_MJPEG
    case V4L2_PIX_FMT_MJPEG:
      if (fuvc->img_size == 0)
        {
          usb_warn("No image loaded.\n");
        }

      target->dwMaxVideoFrameSize = 1843200;
      break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_H264
    case V4L2_PIX_FMT_H264:
      if (fuvc->img_size == 0)
        {
          usb_warn("No image loaded.\n");
        }

      target->dwMaxVideoFrameSize = 1843200;
      break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_H265
    case V4L2_PIX_FMT_H265:
      if (fuvc->img_size == 0)
        {
          usb_warn("No image loaded.\n");
        }

      target->dwMaxVideoFrameSize = 1843200;
      break;
#endif

    default:
      break;
    }

  target->dwMaxPayloadTransferSize = STREAM_BUF_SIZE;
  target->dwFrameInterval          = interval[0];
  if (fuvc->control == USB_UVC_VS_COMMIT_CONTROL && check_probe_status(fuvc))
    {
      fuvc->format_info.format = format->fcc;
      fuvc->format_info.width  = frame->width;
      fuvc->format_info.height = frame->height;
      fuvc->format_info.status = FORMAT_SWITCH_PENDING;
      fuvc_streaming_on(fuvc, UVC_EVENT_LIST);
#ifdef LOSCFG_DRIVERS_USB_UVC_IOCTL
      queue_video_event(fuvc, UVC_EVENT_DATA);
      queue_video_event(fuvc, UVC_EVENT_STREAMON);
#endif
      fuvc->stream_off_flag = 1;
    }

  if (fuvc->control == USB_UVC_VS_COMMIT_CONTROL)
    {
      (void)memset_s(&fuvc->probe_status, sizeof(fuvc->probe_status), 0, sizeof(fuvc->probe_status));
    }
}

static void fuvc_request_complete(struct usbdev_ep_s *ep, struct usbdev_req_s *req)
{
  struct uvc_dev_s *fuvc;

  (void)ep; /* ignored */

  fuvc = (struct uvc_dev_s *)req->priv;
  switch (fuvc->control)
    {
    case ~0u:
      break;

    case USB_UVC_VS_PROBE_CONTROL:
      fuvc_streaming_set(fuvc, req, NULL);
      fuvc->control = ~0;
      break;

    case USB_UVC_VS_COMMIT_CONTROL:
      fuvc_streaming_set(fuvc, req, NULL);
      if (fuvc->connected)
        {
          usbclass_uvc_stream_ep_reset(fuvc);
        }
      fuvc->control = ~0;
      break;

    default:
      fuvc->control = ~0;
      break;
    }
}

#if UVC_USE_CTRL_EP
static void fuvc_vc_request_complete(struct usbdev_ep_s *ep, struct usbdev_req_s *req)
{
  (void)ep;
  (void)req;
  PRINTK("Entering function [%s]\n", __FUNCTION__);
}
#endif

static void fuvc_fill_streaming_control(struct uvc_probe_commit_control_dev *ctrl,
                                        int frame_index, int format_index)
{
  const struct fuvc_format_info *format;
  const struct fuvc_frame_info *frame;
  int frames;
  int index = frame_index;
  if (format_index < 0)
    {
      format_index = (int)ARRAY_SIZE(g_fuvc_formats) + format_index;
    }

  if (format_index < 0 || format_index >= (int)ARRAY_SIZE(g_fuvc_formats))
    {
      return;
    }

  format = &g_fuvc_formats[g_format_index_map[format_index]];

  frames = 0;

  while (format->frames[frames].width != 0)
    {
      ++frames;
    }

  if (index < 0)
    {
      index = frames + index;
    }

  if (index < 0 || index >= frames)
    {
      return;
    }

  frame = &format->frames[index];

  (void)memset_s(ctrl, sizeof(struct uvc_probe_commit_control_dev),
                 0, sizeof(struct uvc_probe_commit_control_dev));

  ctrl->bmHint          = 1;
  ctrl->bFormatIndex    = format_index + 1; /* 1 is yuv, 2 is mjpeg */
  ctrl->bFrameIndex     = index + 1;  /* 360 1 720 2 */
  ctrl->dwFrameInterval = frame->intervals[0];

  switch (format->fcc)
    {
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_YUYV
    case V4L2_PIX_FMT_YUYV:
      ctrl->dwMaxVideoFrameSize = frame->width * frame->height * YUYV_FRAME_FACTOR;
      break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_NV12
    case V4L2_PIX_FMT_NV12:
      ctrl->dwMaxVideoFrameSize = frame->width * frame->height * NV12_FRAME_FACTOR;
      break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_NV21
    case V4L2_PIX_FMT_NV21:
      ctrl->dwMaxVideoFrameSize = frame->width * frame->height * NV21_FRAME_FACTOR;
      break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_MJPEG
    case V4L2_PIX_FMT_MJPEG:
      ctrl->dwMaxVideoFrameSize = 1843200;
      break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_H264
    case V4L2_PIX_FMT_H264:
      ctrl->dwMaxVideoFrameSize = 1843200;
      break;
#endif
#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_H265
    case V4L2_PIX_FMT_H265:
      ctrl->dwMaxVideoFrameSize = 1843200;
      break;
#endif

    default:
        break;
    }

  ctrl->dwMaxPayloadTransferSize = STREAM_BUF_SIZE;
#ifndef UVC_VERSION_1_0
  ctrl->bmFramingInfo    = 3;
  ctrl->bPreferedVersion = 1;
  ctrl->bMaxVersion      = 1;
#endif
}

static void fuvc_get_default_streaming_ctrl(struct uvc_probe_commit_control_dev *ctrl)
{
  fuvc_fill_streaming_control(ctrl, 0, 0);
}

static void fuvc_control_unit_callback(struct usbdev_ep_s *ep, struct usbdev_req_s *req)
{
  struct uvc_dev_s *fuvc;
  int ret;

  (void)ep;

  fuvc = (struct uvc_dev_s *)req->priv;

  ret = run_cmd_func(req->buf, req->xfrd, fuvc->event_id, UVC_RC_SETCUR, fuvc->unit_id);
  if (ret < 0)
    {
      usb_err("run_cmd_func return error!\n");
    }
}

static int fuvc_handle_camera_control(struct uvc_dev_s *fuvc,
                                      struct usbdev_req_s *req,
                                      const struct usb_ctrlreq_s *ctrl)
{
  uint16_t w_value;
  int ret = 0;

  w_value = USB_GETW(ctrl->value);

  /* The high byte of wValue contains the Control Selector
   * Refer to UVC spec. 1.5, section 4.1.1 & 4.1.2
   */

  switch (ctrl->req)
    {
    case UVC_RC_SETCUR:
      req->callback  = fuvc_control_unit_callback;
      fuvc->event_id = w_value >> 8;
      fuvc->unit_id  = UNIT_ID_CAMERA;
      break;

    default:
      ret = run_cmd_func(req->buf, USB_GETW(ctrl->len), w_value >> 8, ctrl->req, UNIT_ID_CAMERA);
      if (ret < 0)
        {
          usb_err("run_cmd_func return error!\n");
        }
      break;
    }

  return ret;
}

static int fuvc_handle_process_control(struct uvc_dev_s *fuvc,
                                       struct usbdev_req_s *req, const struct usb_ctrlreq_s *ctrl)
{
  uint16_t w_value;
  int ret = 0;

  w_value = USB_GETW(ctrl->value);

  switch (ctrl->req)
    {
    case UVC_RC_SETCUR:
      req->callback  = fuvc_control_unit_callback;
      fuvc->event_id = w_value >> 8;
      fuvc->unit_id  = UNIT_ID_PROCESSING;
      break;

    default:
      ret = run_cmd_func(req->buf, USB_GETW(ctrl->len), w_value >> 8, ctrl->req, UNIT_ID_PROCESSING);
      if (ret < 0)
        {
          usb_err("run_cmd_func return error!\n");
        }
      break;
    }

  return ret;
}

#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_H264
/* We have two UVC specifications, version 1.1 & 1.5
 * The latter added H264 stream format/frame definition,
 * but the former supports H264 stream via extension unit.
 * After careful consideration, I think we'd better support
 * version 1.1, because it's better supported by windows 7
 */

static int fuvc_handle_ext_control(struct uvc_dev_s *fuvc, struct usbdev_req_s *req,
                                   const struct usb_ctrlreq_s *ctrl)
{
  uint16_t w_value;
  int ret = 0;

  w_value = USB_GETW(ctrl->value);

  switch (ctrl->req)
    {
    case UVC_RC_SETCUR:
      req->callback  = fuvc_control_unit_callback;
      fuvc->event_id = w_value >> 8;
      fuvc->unit_id  = UNIT_ID_H264_EXTENSION;
      break;

    default:
      ret = run_cmd_func(req->buf, USB_GETW(ctrl->len), w_value >> 8, ctrl->req, UNIT_ID_H264_EXTENSION);
      if (ret < 0)
        {
          usb_err("run_cmd_func return error!\n");
        }
      break;
    }

  return ret;
}
#endif

static int fuvc_handle_xu_camera_control(struct uvc_dev_s *fuvc, struct usbdev_req_s *req,
                                         const struct usb_ctrlreq_s *ctrl)
{
  uint16_t w_value = USB_GETW(ctrl->value);
  int ret = 0;

  switch (ctrl->req)
    {
    case UVC_RC_SETCUR:
      req->callback  = fuvc_control_unit_callback;
      fuvc->event_id = w_value >> 8;
      fuvc->unit_id  = UNIT_ID_HICAMERA_EXTENSION;
      break;

    default:
      ret = run_cmd_func(req->buf, USB_GETW(ctrl->len), w_value >> 8, ctrl->req, UNIT_ID_HICAMERA_EXTENSION);
      if (ret < 0)
        {
          usb_err("run_cmd_func return error!\n");
        }
      break;
    }

  return ret;
}

static void fuvc_handle_class_setup_control(struct uvc_dev_s *fuvc, struct usbdev_req_s *req,
                                            const struct usb_ctrlreq_s *ctrl)
{
  int retval;
  uint16_t n_len;
  uint16_t w_length = USB_GETW(ctrl->len);
  uint16_t w_index  = USB_GETW(ctrl->index);
  uint16_t w_value  = USB_GETW(ctrl->value);

  retval = 0;
  (void)memset_s(req->buf, USB_COMP_EP0_BUFSIZ, 0, USB_COMP_EP0_BUFSIZ);

  n_len = 32;
  if (n_len > w_length)
    {
      n_len = w_length;
    }

  /* The high byte of wIndex indicates which entity the request addresses.
   * Refer to UVC spec. 1.5, section 4.1.1 & 4.1.2
   */

  switch (w_index >> 8)
    {
    case UNIT_ID_CAMERA:
      retval = fuvc_handle_camera_control(fuvc, req, ctrl);
      break;

    case UNIT_ID_PROCESSING:
      retval = fuvc_handle_process_control(fuvc, req, ctrl);
      break;

#ifdef LOSCFG_DRIVERS_USB_UVC_GADGET_H264
    case UNIT_ID_H264_EXTENSION:
      retval = fuvc_handle_ext_control(fuvc, req, ctrl);
      break;
#endif

    case UNIT_ID_HICAMERA_EXTENSION:
      retval = fuvc_handle_xu_camera_control(fuvc, req, ctrl);
      break;

    default:
      PRINTK("Control Requests not supported, index: %#x, w_value: %#x\n", w_index, w_value);
      break;
    }

  if (retval <= 0)
    {
      req->len = n_len;
    }
  else
    {
      req->len = (uint32_t)retval;
    }
}

static int fuvc_handle_streaming_probe(struct uvc_dev_s *fuvc, struct usbdev_req_s *req,
                                       const struct usb_ctrlreq_s *ctrl)
{
  int retval = 0;
  uint8_t breq;
  errno_t errnum;
  struct uvc_probe_commit_control_dev commit;

  breq = ctrl->req;
  switch (breq)
    {
    case UVC_RC_SETCUR:
      fuvc->com_flag = 1;
      retval         = 1;
      break;

    case UVC_RC_SETCUR_ALL:
      PRINTK("UVC Streaming Probe: %s\n", "UVC_RC_SETCUR_ALL");
      break;

    case UVC_RC_GETINFO:
      PRINTK("UVC Streaming Probe: %s\n", "UVC_RC_GETINFO");
      break;

    case UVC_RC_GETCUR:
      if ((USB_GETW(ctrl->value) >> 8) == USB_UVC_VS_PROBE_CONTROL)
        {
          errnum = memcpy_s(req->buf, USB_COMP_EP0_BUFSIZ,
                            &fuvc->probe, sizeof(struct uvc_probe_commit_control_dev));
        }
      else
        {
          errnum = memcpy_s(req->buf, USB_COMP_EP0_BUFSIZ,
                            &fuvc->commit, sizeof(struct uvc_probe_commit_control_dev));
        }

      if (errnum != EOK)
        {
          usb_err("memcpy fail %d\n", errnum);
          break;
        }

      retval = 0;
      break;

    case UVC_RC_GETMIN:
    case UVC_RC_GETMAX:
    case UVC_RC_GETRES:
    case UVC_RC_GETLEN:
    case UVC_RC_GETDEF:
    case UVC_RC_GETCUR_ALL:
    case UVC_RC_GETMIN_ALL:
    case UVC_RC_GETMAX_ALL:
    case UVC_RC_GETRES_ALL:
    case UVC_RC_GETDEF_ALL:
      fuvc_get_default_streaming_ctrl(&commit); /* The value of fint is unused */
      errnum = memcpy_s(req->buf, USB_COMP_EP0_BUFSIZ,
                        (void *)&commit, sizeof(struct uvc_probe_commit_control_dev));
      if (errnum != EOK)
        {
          usb_err("memcpy fail %d\n", errnum);
          break;
        }

      retval = 0;
      break;

    default:
      usb_err("Fatal Error, un-supported request from host: %#x\n", breq);
      break;
    }

  return retval;
}

static void manual_stream_off_request(struct uvc_dev_s *fuvc);

static int fuvc_handle_class_setup_streaming(struct uvc_dev_s *fuvc,
                                             struct usbdev_req_s *req, const struct usb_ctrlreq_s *ctrl)
{
  int ret;
  errno_t errnum;
  const char *errmsg = NULL;
  uint16_t n_len;
  uint16_t w_len;
  uint16_t w_value;

  ret   = 0;
  n_len = sizeof(struct uvc_probe_commit_control_dev);

  w_value = USB_GETW(ctrl->value);
  w_len   = USB_GETW(ctrl->len);

  switch (w_value >> 8)
    {
    case USB_UVC_VS_PROBE_CONTROL:
      manual_stream_off_request(fuvc);
      fuvc->control = USB_UVC_VS_PROBE_CONTROL;
      ret = fuvc_handle_streaming_probe(fuvc, req, ctrl);
      if (ret)
        {
          errnum = memcpy_s(req->buf, USB_COMP_EP0_BUFSIZ, &fuvc->probe, n_len);
          if (errnum != EOK)
            {
              usb_err("memcpy fail, %d\n", errnum);
              break;
            }
        }

      ret = 1;
      break;

    case USB_UVC_VS_COMMIT_CONTROL:
      fuvc->control = USB_UVC_VS_COMMIT_CONTROL;
      (void)fuvc_handle_streaming_probe(fuvc, req, ctrl);
      ret = 1;
      break;

    case USB_UVC_VS_STILL_PROBE_CONTROL:
      errmsg = "USB_UVC_VS_STILL_PROBE_CONTROL";
      break;

    case USB_UVC_VS_STILL_COMMIT_CONTROL:
      errmsg = "USB_UVC_VS_STILL_COMMIT_CONTROL";
      break;

    case USB_UVC_VS_STILL_IMAGE_TRIGGER_CONTROL:
      errmsg = "USB_UVC_VS_STILL_IMAGE_TRIGGER_CONTROL";
      break;

    case USB_UVC_VS_STREAM_ERROR_CODE_CONTROL:
      errmsg = "USB_UVC_VS_STREAM_ERROR_CODE_CONTROL";
      break;

    case USB_UVC_VS_GENERATE_KEY_FRAME_CONTROL:
      errmsg = "USB_UVC_VS_GENERATE_KEY_FRAME_CONTROL";
      break;

    case USB_UVC_VS_UPDATE_FRAME_SEGMENT_CONTROL:
      errmsg = "USB_UVC_VS_UPDATE_FRAME_SEGMENT_CONTROL";
      break;

    case USB_UVC_VS_SYNCH_DELAY_CONTROL:
      errmsg = "USB_UVC_VS_SYNCH_DELAY_CONTROL";
      break;

    default:
      PRINTK("Invalid streaming control request: %#x\n", w_value);
      break;
    }

  if (errmsg != NULL)
    {
      PRINTK("UVC stream control %s not supported!\n", errmsg);
    }
  if (ret)
    {
      req->len = (n_len > w_len) ? w_len : n_len;
    }
  return ret;
}

static void fuvc_source_free(struct usbdevclass_driver_s *driver, struct usbdev_s *dev)
{
  struct uvc_driver_s *drvr = (struct uvc_driver_s *)driver;
  struct uvc_dev_s *fuvc    = drvr->dev;

  if (fuvc->ctrl_ep != NULL)
    {
      (void)EP_DISABLE(fuvc->ctrl_ep);
      DEV_FREEEP(dev, fuvc->ctrl_ep);
      fuvc->ctrl_ep = NULL;
    }

  if (fuvc->vs_ep != NULL)
    {
      (void)EP_DISABLE(fuvc->vs_ep);
      DEV_FREEEP(dev, fuvc->vs_ep);
      fuvc->vs_ep = NULL;
    }

  (void)LOS_EventDestroy(&fuvc->uvc_transfer_event);
}

static int usbclass_uvc_bind(struct usbdevclass_driver_s *driver, struct usbdev_s *dev)
{
  int ret;
  struct usbdev_ep_s *ep;
  struct uvc_driver_s *uvc_drvr;
  struct uvc_dev_s *uvc_dev;
  struct composite_dev_s *cdev;
  struct composite_devobj_s *devobj;
  struct usbdev_devinfo_s *devinfo;

  if (driver == NULL || dev == NULL)
    {
      return -1;
    }

  cdev = dev->ep0->priv;
  if (cdev == NULL)
    {
      return -1;
    }
  uvc_drvr = (struct uvc_driver_s *)driver;
  uvc_dev  = uvc_drvr->dev;
  if (uvc_dev == NULL)
    {
      return -1;
    }

  uvc_stream_init(uvc_dev);

#ifdef LOSCFG_DRIVERS_USB_UVC_IOCTL
  ret = uvc_fops_init(uvc_dev);
  if (ret != UVC_OK)
   {
     goto fail;
   }
#endif

  devobj = usbclass_devobj_get(cdev, DEV_UVC);
  if (devobj == NULL)
    {
      goto fail;
    }
  devinfo = &devobj->compdesc.devinfo;
  (void)memset_s(&uvc_dev->format_info, sizeof(struct uvc_format_info), 0, sizeof(struct uvc_format_info));

#if UVC_USE_CTRL_EP

  /* Initialize control endpoint */

  ep = DEV_ALLOCEP(dev, g_fuvc_ctrl_ep_desc.addr,
                   (struct usb_epdesc_s *)&g_fuvc_ctrl_ep_desc);
  if (ep == NULL)
    {
      goto fail;
    }
#ifdef USB3_DEVICE_CONTROLLER
  ep->comp_desc = &g_fuvc_ctrl_ss_comp_desc;
#endif
  ep->priv = (void *)uvc_dev;
  (void)memset_s(&(uvc_dev->vc_ctrlreq), sizeof(struct usbdev_req_s), 0, sizeof(struct usbdev_req_s));
  uvc_dev->vc_ctrlreq.callback = fuvc_vc_request_complete;
  uvc_dev->vc_ctrlreq.priv     = (void *)uvc_dev;
  ep->handle_req               = &uvc_dev->vc_ctrlreq;
  uvc_dev->ctrl_ep             = ep;
  devinfo->epno[0]             = ep->eplog;
#endif

  /* Initialize VideoStreaming endpoint */

#if FUVC_BULK_TO_ISO_CONVERT
  ep = DEV_ALLOCEP(dev, g_fuvc_hs_isoc_streaming_ep.addr,
                   (struct usb_epdesc_s *)&g_fuvc_hs_isoc_streaming_ep);
  g_fuvc_vs_head_desc.bEndpointAddress = g_fuvc_hs_isoc_streaming_ep.addr;
#else
  ep = DEV_ALLOCEP(dev, g_fuvc_hs_bulk_streaming_ep.addr,
                   (struct usb_epdesc_s *)&g_fuvc_hs_bulk_streaming_ep);
  g_fuvc_vs_head_desc.bEndpointAddress = g_fuvc_hs_bulk_streaming_ep.addr;
#endif
  if (ep == NULL)
    {
      goto fail;
    }

#ifdef USB3_DEVICE_CONTROLLER
#if FUVC_BULK_TO_ISO_CONVERT
  ep->comp_desc = &g_fuvc_isoc_streaming_ss_comp_desc;
#else
  ep->comp_desc = &g_fuvc_bulk_streaming_ss_comp_desc;
#endif
#endif
  (void)memset_s(&uvc_dev->streamreq, sizeof(struct usbdev_req_s), 0, sizeof(struct usbdev_req_s));
  uvc_dev->streamreq.callback = fuvc_vs_req_complete;
  uvc_dev->streamreq.priv     = (void *)uvc_dev;
  uvc_dev->streamreq.buf      = NULL;

  ep->priv         = (void *)uvc_dev;
  ep->handle_req   = &uvc_dev->streamreq;
  uvc_dev->vs_ep   = ep;
  devinfo->epno[1] = ep->eplog;

  /* Set the descriptors */

  uvc_dev->control    = ~0;
  uvc_dev->uvc_handle = (void *)UVC_INVALID_HANDLE;
  spin_lock_init(&uvc_dev->lock);
  (void)LOS_EventInit(&uvc_dev->uvc_transfer_event);

  ret = uvc_stream_thread_init(uvc_dev);
  if (ret != 0)
    {
      goto fail;
    }

  uvc_dev->connected = false;

  fuvc_fill_streaming_control(&uvc_dev->probe, 0, 0);
  fuvc_fill_streaming_control(&uvc_dev->commit, 0, 0);
  uvc_unit_control_register();

  return 0;

fail:
  (void)usbclass_uvc_unbind(driver, dev);
  return -1;
}

static int usbclass_uvc_unbind(struct usbdevclass_driver_s *driver, struct usbdev_s *dev)
{
  struct composite_dev_s *cdev;
  struct composite_devobj_s *devobj;
  struct usbdev_devinfo_s *devinfo;
  struct uvc_driver_s *drvr;
  struct uvc_dev_s *fuvc;

  if (driver == NULL || dev == NULL)
    {
      usb_err("fuvc_unbind_gadget failed!\n");
      return -1;
    }

  drvr = (struct uvc_driver_s *)driver;
  fuvc = drvr->dev;
  if (fuvc == NULL)
    {
      return -1;
    }

  if (uvc_acquire_mutex() != UVC_OK)
    {
      return -1;
    }

  if (!is_ioctl_mode() && uvc_is_running())
    {
      usb_err("uvc busy, unbind fail\n");
      goto err_out;
    }

  /* Prevent concurrency with uvc_close_device and uvc_recv_pack processes */

  if (LOS_AtomicCmpXchg32bits(&fuvc->uvc_atomic, UNBIND_STATUS, IDLE_STATUS))
    {
      usb_err("uvc is receiving the pack\n");
      goto err_out;
    }

#ifdef LOSCFG_DRIVERS_USB_UVC_IOCTL
  if (uvc_fops_deinit(fuvc) == EBUSY)
    {
      usb_err("uvc ops busy, unbind fail\n");
      LOS_AtomicSet(&fuvc->uvc_atomic, IDLE_STATUS);
      goto err_out;
    }
#endif
  uvc_stream_deinit(fuvc);

  camera_cmd_init();
  usbclass_uvc_disconnect(driver, dev);
  uvc_stream_thread_deinit(fuvc);

  LOS_AtomicSet(&fuvc->uvc_atomic, IDLE_STATUS);

  cdev = dev->ep0->priv;
  if (cdev == NULL)
    {
      goto warn_out;
    }

  devobj = usbclass_devobj_get(cdev, DEV_UVC);
  if (devobj == NULL)
    {
      goto warn_out;
    }

  devinfo = &devobj->compdesc.devinfo;
  (void)memset_s(devinfo, sizeof(struct usbdev_devinfo_s), 0, sizeof(struct usbdev_devinfo_s));

warn_out:
  fuvc_source_free(driver, dev);
  uvc_release_mutex();
  return 0;

err_out:

  /* Return -1 directly when resources may be in used */

  uvc_release_mutex();
  return -1;
}

#if FUVC_BULK_TO_ISO_CONVERT
static int usbclass_uvc_set_alt(struct uvc_dev_s *fuvc, struct usbdev_s *dev,
                                unsigned intf, unsigned alt)
{
  int ret = 0;

  if (fuvc == NULL)
    {
      return -1;
    }

  if (g_fuvc_vc_intf_desc.ifno == intf)
    {
#if UVC_USE_CTRL_EP
      if (fuvc->ctrl_ep->priv != NULL)
        {
          (void)EP_DISABLE(fuvc->ctrl_ep);
          fuvc->ctrl_ep->priv = NULL;
        }

      ret = EP_CONFIGURE(fuvc->ctrl_ep, (const struct usb_epdesc_s *)&g_fuvc_ctrl_ep_desc, 0);
      if (ret)
        {
          usb_err("Error enabling endpoint: %d\n", ret);
          return -1;
        }
      fuvc->ctrl_ep->priv = (void *)fuvc;
#endif
    }
  else if (g_fuvc_vs_intf_alt0.ifno == intf)
    {
      if (alt == 0)
        {
          if (!fuvc->connected)
            {
              return 0;
            }

          if (fuvc->vs_ep != NULL)
            {
              (void)EP_DISABLE(fuvc->vs_ep);
              EP_FLUSH(fuvc->vs_ep);
            }
          fuvc_streaming_off(fuvc);
        }
      else
        {
          if (fuvc->vs_ep->priv != NULL)
            {
              (void)EP_DISABLE(fuvc->vs_ep);
              fuvc->vs_ep->priv = NULL;
            }

          ret = EP_CONFIGURE(fuvc->vs_ep, (const struct usb_epdesc_s *)&g_fuvc_hs_isoc_streaming_ep, 0);
          if (ret)
            {
              usb_err("Error enabling endpoint: %d\n", ret);
              return -1;
            }
          fuvc->vs_ep->priv = (void *)fuvc;

          fuvc_streaming_on(fuvc, 0);
        }
    }
  else
    {
      return -EINVAL;
    }

  return ret;
}

static void manual_stream_off_request(struct uvc_dev_s *fuvc)
{
  (void)fuvc;
}
#else
static int usbclass_uvc_set_alt(struct uvc_dev_s *fuvc, struct usbdev_s *dev)
{
  int ret = 0;

  if (fuvc != NULL)
    {
#if UVC_USE_CTRL_EP
      if (fuvc->ctrl_ep->priv != NULL)
        {
          (void)EP_DISABLE(fuvc->ctrl_ep);
          fuvc->ctrl_ep->priv = NULL;
        }
      ret = EP_CONFIGURE(fuvc->ctrl_ep, (const struct usb_epdesc_s *)&g_fuvc_ctrl_ep_desc, 0);
      if (ret)
        {
          usb_err("Error enabling endpoint: %d\n", ret);
          return -1;
        }
      fuvc->ctrl_ep->priv = (void *)fuvc;
#endif

      if (fuvc->vs_ep->priv != NULL)
        {
          (void)EP_DISABLE(fuvc->vs_ep);
          fuvc->vs_ep->priv = NULL;
        }

      usbd_configep_byspeed(dev, &g_fuvc_hs_bulk_streaming_ep, USB_DESC_TYPE_CONFIG);
      ret = EP_CONFIGURE(fuvc->vs_ep, (const struct usb_epdesc_s *)&g_fuvc_hs_bulk_streaming_ep, 0);
      if (ret)
        {
          usb_err("Error enabling endpoint: %d\n", ret);
          return -1;
        }
      fuvc->vs_ep->priv = (void *)fuvc;

      if (!LOS_ListEmpty(&fuvc->uvc_transfer_event.stEventList))
        {
          (void)LOS_EventWrite(&fuvc->uvc_transfer_event, UVC_EVENT_LIST);
        }

      if (fuvc->connected)
        {
          usbclass_uvc_stream_ep_reset(fuvc);
          fuvc_streaming_off(fuvc);
        }
    }

  return ret;
}
static void manual_stream_off_request(struct uvc_dev_s *fuvc)
{
  if (fuvc->stream_off_flag)
    {
      /* No stream off request in bulk mode */

      fuvc->stream_off_flag = 0;
      fuvc_streaming_off(fuvc);
    }
}
#endif

static int usbclass_uvc_setup(struct usbdevclass_driver_s *driver, struct usbdev_s *dev,
                              const struct usb_ctrlreq_s *ctrl, uint8_t *dataout, size_t outlen)
{
  uint8_t req_type;
  uint8_t new_req = 0;
  uint16_t w_index;
  struct uvc_dev_s *fuvc;
  struct uvc_driver_s *drvr;
  struct usbdev_req_s *req;

  (void)dataout;
  (void)outlen;

  if (driver == NULL || dev == NULL || ctrl == NULL)
    {
      return -1;
    }
  w_index = USB_GETW(ctrl->index);

  drvr = (struct uvc_driver_s *)driver;
  fuvc = drvr->dev;
  if (fuvc == NULL)
    {
      return -1;
    }

  req           = dev->ep0->handle_req;
  req->callback = fuvc_request_complete;
  req->priv     = fuvc;

  req_type = ctrl->type;
  if ((req_type & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_STANDARD)
    {
      switch (ctrl->req)
        {
        case USB_REQ_SETCONFIGURATION:
        case USB_REQ_SETINTERFACE:
#if FUVC_BULK_TO_ISO_CONVERT
          (void)usbclass_uvc_set_alt(fuvc, dev, w_index, USB_GETW(ctrl->value));
#else
          (void)usbclass_uvc_set_alt(fuvc, dev);
#endif
          break;

        default:
          break;
        }
    }
  else
    {
      /* Refer to section UVC spec. 1.5, section 4.1.2 */

      if ((req_type & 0x1f) == 0x2)
        {
          /* Handle request directed to an endpoint */

          PRINTK("Support request for endpoint: %#x\n", req_type);
          return 0;
        }

      /* Make sure that the request is directed to an interface */

      if ((req_type & 0x1f) != 0x1)
        {
          PRINTK("Fatal Error at line: %d, req_type: %#x\n", __LINE__, req_type);
          return 0;
        }

      if ((ctrl->type & USB_REQ_RECIPIENT_MASK) == USB_REQ_RECIPIENT_INTERFACE)
        {
          set_probe_status(fuvc, (USB_GETW(ctrl->value) >> 8), ctrl->req);
        }

      w_index = USB_GETW(ctrl->index);
      switch (w_index & 0xff)
        {
        case USB_UVC_INTERFACE_CONTROL:
          fuvc_handle_class_setup_control(fuvc, req, ctrl);
          new_req++;
          break;

        case USB_UVC_INTERFACE_STREAMING:
          if (fuvc_handle_class_setup_streaming(fuvc, req, ctrl))
            {
              new_req++;
            }
          break;

        default:
          PRINTK("Unknown index in [%s]: %#x\n", __FUNCTION__, w_index);
          break;
        }

      if (new_req)
        {
          (void)EP_SUBMIT(dev->ep0, req);
        }
    }

  return 0;
}

static void usbclass_uvc_disconnect(struct usbdevclass_driver_s *driver,
                                    struct usbdev_s *dev)
{
  struct uvc_driver_s *drvr;
  struct uvc_dev_s *fuvc;

  if (driver == NULL || dev == NULL)
    {
      return;
    }

  drvr = (struct uvc_driver_s *)driver;
  fuvc = drvr->dev;
  if (fuvc == NULL)
    {
      return;
    }

  if (fuvc->vs_ep != NULL)
    {
      (void)EP_DISABLE(fuvc->vs_ep);
      EP_FLUSH(fuvc->vs_ep);
    }

  if (fuvc->connected)
    {
      fuvc_streaming_off(fuvc);
    }

#ifdef USB3_DEVICE_CONTROLLER
#if FUVC_BULK_TO_ISO_CONVERT
  USB_SETW(g_fuvc_hs_isoc_streaming_ep.mxpacketsize, 0x1400);
#else
  USB_SETW(g_fuvc_hs_bulk_streaming_ep.mxpacketsize, 0x200);
#endif
#endif
}

static struct usbd_string g_fuvc_device_strings[8] =
{
  { 0, g_fuvc_str_lang },
  { 1, NULL },
  { 2, NULL },
  { 3, NULL },
  { 4, g_fuvc_str_video },
  { 5, g_fuvc_str_config },
  { 6, g_fuvc_str_interface },
  USBD_DEVICE_STRINGS_END
};

void uvc_get_device_dec_info(struct usbd_string **device_strings, struct usb_devdesc_s **device_desc)
{
  *device_strings = &g_fuvc_device_strings[1];
  *device_desc    = &g_fuvc_device_desc;
}

void uvc_mkdevdesc(uint8_t *buf)
{
  errno_t ret;

  if (USB_GETW(g_fuvc_device_desc.vendor) == 0)
    {
      usb_err("VID is not set!\n");
      return;
    }

  ret = memcpy_s(buf, USB_COMP_EP0_BUFSIZ, &g_fuvc_device_desc, sizeof(g_fuvc_device_desc));
  if (ret != EOK)
    {
      usb_err("memcpy fail\n");
    }
}

#ifdef CONFIG_USBDEV_SUPPORT_CERTIFY
int16_t uvc_mkcfgdesc(uint8_t *buf, struct usbdev_devinfo_s *devinfo,
                      uint8_t speed, uint8_t type)
{
  (void)speed;
  (void)type;
#else
int16_t uvc_mkcfgdesc(uint8_t *buf, struct usbdev_devinfo_s *devinfo)
{
#endif
  uint16_t total_len;
  uint8_t *des;
  int ret;

  g_fuvc_iad.firstif       = devinfo->ifnobase;
  g_fuvc_vc_intf_desc.ifno = devinfo->ifnobase;
  g_fuvc_vs_intf_alt0.ifno = devinfo->ifnobase + 1;

#ifdef USB3_DEVICE_CONTROLLER
  ret = dwc_usb3_dev_speed_get();
  if (ret == USB_SPEED_SUPER)
    {
#if FUVC_BULK_TO_ISO_CONVERT
      USB_SETW(g_fuvc_hs_isoc_streaming_ep.mxpacketsize, 0x400);
#else
      USB_SETW(g_fuvc_hs_bulk_streaming_ep.mxpacketsize, 0x400);
#endif
    }
  else if (ret < 0)
    {
      usb_err("Get speed failed!\n");
      return 0;
    }
#endif

  des = link_fuvc_descriptors(&total_len);
  if (des != NULL)
    {
      ret = memcpy_s(buf, USB_COMP_EP0_BUFSIZ, des, total_len);
      if (ret != EOK)
        {
          usb_err("memcpy fail!, ret:%d\n", ret);
          free(des);
          return 0;
        }
      free(des);
    }

  return total_len;
}

int uvc_mkstrdesc(uint8_t id, struct usb_strdesc_s *buf)
{
  errno_t ret;
  const char *str;
  int i;

  for (i = 0; g_fuvc_device_strings[i].s != NULL; i++)
    {
      str = g_fuvc_device_strings[i].s;
      if (g_fuvc_device_strings[i].id == id)
        {
          ret = memcpy_s(buf, USB_COMP_EP0_BUFSIZ, str, str[0]);
          if (ret != EOK)
            {
              usb_err("memcpy failed, ret = %d\n", ret);
              return -1;
            }
          return str[0];
        }
    }

  usb_err("Can not find the id = %u of string\n", id);
  return -1;
}

#define UVC_NCONFIGS    1
#define UVC_CONFIGID    0
#define UVC_NINTERFACES 2
#define UVC_NSTRIDS     6
#define UVC_NUM_EPS     2
static void uvc_get_composite_devdesc(struct composite_devdesc_s *dev)
{
  (void)memset_s(dev, sizeof(struct composite_devdesc_s), 0, sizeof(struct composite_devdesc_s));

  dev->mkdevdesc  = uvc_mkdevdesc;
  dev->mkconfdesc = uvc_mkcfgdesc;
  dev->mkstrdesc  = uvc_mkstrdesc;

  dev->nconfigs   = UVC_NCONFIGS; /* Number of configurations supported */
  dev->configid   = UVC_CONFIGID; /* The only supported configuration ID */

  dev->devinfo.ninterfaces = UVC_NINTERFACES; /* Number of interfaces in the configuration */
  dev->devinfo.nstrings    = UVC_NSTRIDS;     /* Number of Strings */
  dev->devinfo.nendpoints  = UVC_NUM_EPS;
}

int uvc_classobject(int minor, struct usbdev_devinfo_s *devinfo,
                    struct usbdevclass_driver_s **classdev)
{
  struct uvc_softc *uvc_s;
  struct uvc_dev_s *priv;
  struct uvc_driver_s *drvr;

  (void)minor;
  (void)devinfo;

  /* Allocate the structures needed */

  uvc_s = (struct uvc_softc *)malloc(sizeof(struct uvc_softc));
  if (uvc_s == NULL)
    {
      return -1;
    }

  /* Convenience pointers into the allocated blob */

  priv = &uvc_s->dev;
  drvr = &uvc_s->drvr;

  /* Initialize the USB serial driver structure */

  (void)memset_s(priv, sizeof(struct uvc_dev_s), 0, sizeof(struct uvc_dev_s));

  /* Initialize the USB class driver structure */

  drvr->drvr.speed = USB_SPEED_HIGH;
  drvr->drvr.ops   = &g_uvc_driverops;
  drvr->dev        = priv;

  *classdev = &drvr->drvr;
  return 0;
}

void uvc_uninitialize(struct usbdevclass_driver_s *classdev)
{
  struct uvc_driver_s *uvc_drvr = (struct uvc_driver_s *)classdev;
  struct uvc_dev_s *priv;
  struct uvc_softc *uvc_s;

  dev_str_desc_free(&g_fuvc_device_strings[1]);

  if (uvc_drvr == NULL)
    {
      return;
    }

  priv = uvc_drvr->dev;
  if (priv == NULL)
    {
      return;
    }

  uvc_s = container_of(uvc_drvr, struct uvc_softc, drvr);
  free(uvc_s);
}

void usbdev_uvc_initialize_sub(struct composite_devdesc_s *dev, int ifnobase, int minor)
{
  /* Ask the UVC driver to fill in the constants we didn't
   * know here.
   */

  uvc_get_composite_devdesc(dev);

  /* The callback functions for the UVC class */

  dev->classobject  = uvc_classobject;
  dev->uninitialize = uvc_uninitialize;

  dev->devinfo.ifnobase = ifnobase; /* Offset to Interface-IDs */
  dev->minor            = minor;    /* The minor interface number */

  dev->devinfo.strbase = 0;         /* Offset to String Numbers */
}

int usbdev_uvc_initialize(void *handle)
{
  struct composite_devdesc_s dev;
  int ret;

  usbdev_uvc_initialize_sub(&dev, 0, DEV_UVC);

  ret = composite_initialize_softc(1, &dev, handle);
  if (ret < 0)
    {
      return -1;
    }

  PRINTK("  ** uvc device initialized successfully! **\n");
  return 0;
}
