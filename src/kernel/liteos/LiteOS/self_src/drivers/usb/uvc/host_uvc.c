/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2025. All rights reserved.
 * Description: LiteOS USB Host Video Class driver
 * Author: Huawei LiteOS Team
 * Create: 2022-04-16
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

#include <poll.h>
#include <fs/fs.h>
#include "implementation/global_implementation.h"
#include "implementation/bsd_kernel.h"
#include "implementation/usbdevs.h"
#include "implementation/usb_list.h"
#include "uvc/usb_uvc.h"
#include "uvc/usb_v4l2.h"

#define USB_HOST_UVC_DEV	"/dev/video"
#define UVC_FLAG_ACTIVE	(1 << 0)

#define DRIVER_NAME(sc)	(device_get_nameunit((sc)->dev))
#define BUF_INFO_NAME	"usb-"

#define UVC_FRAME_MAX_COUNT	30
#define UVC_FRAME_TIMEOUT_MS	5000

#define UVC_PACK_NUM	64
#define UVC_IXFER_NUM	3	/* Note: The value must be equal to the number of xfers */
#define UVC_INIR_SIZE	32

/*
 * USB Video Class -- Class-Specific Requests
 * Refer to USB Video Class Specification 1.1, section 4.3.1
 */
#define UVC_SET_INTERFACE	0x21
#define UVC_GET_INTERFACE	0xa1

DEFINE_UVC_VS_INPUT_HEADER_DESC(0, 0);
DEFINE_UVC_VC_INTERFACE_HEADER_DESC(0);
DEFINE_UVC_FRAME_DESCRIPTOR(incomplete, 0);

#define VS_IFACE_NUM		12
#define UVC_VS_FORMAT_NUM	10
#define UVC_FRAME_MAX_LEN	24

#define VALUE_ROUND_UP(n, d)	(((n) + (d) - 1) / (d))

/* Used for USB transfer index */
enum {
	UVC_INTR,
	UVC_XFER1,
	UVC_XFER2,
	UVC_XFER3,
	UVC_TRANSFER_NUM,
};

/* Events used for usb explore task completion */
enum {
	SFMT_EVENT	= 1 << 0,
	SPARM_EVENT	= 1 << 1,
	STREAMON_EVENT	= 1 << 2,
	STREAMOFF_EVENT	= 1 << 3,
};

/* Transfer events of uvc frame data */
enum {
	UVC_FRAME_BUF_EVENT	= 1 << 0,
	UVC_DETACH_EVENT	= 1 << 1,
};

struct uvc_fmt_enum {
	uint32_t pixel_fmt;
	char *name;
};

/*
 * The array uvc_fmt_enums contains the number of uncompressed formats.
 * Note: YUYV index must start from zero.
 */
#define UNCOMPRESSED_FORMAT_NUM 4
struct uvc_fmt_enum uvc_fmt_enums[] = {
	{ V4L2_PIX_FMT_YUYV, "YUYV" },
	{ V4L2_PIX_FMT_YUYV, "YUY2" },
	{ V4L2_PIX_FMT_NV12, "NV12" },
	{ V4L2_PIX_FMT_NV21, "NV21" },
	{ V4L2_PIX_FMT_MJPEG, "MJPEG" },
	{ 0, "UnKNOWN UC Format" }
};

struct host_uvc_mutex {
	struct mtx	uvc_mux;	/* protect uvc_softc resources */
	uint8_t		uvc_attached;	/* 1: device attached; 0: device detached */
};

static struct host_uvc_mutex protection_softc = {
	.uvc_mux = USB_MUTEX_INIT_VAL,
	.uvc_attached = 0,
};

struct uvc_isoc_xfer {
	struct uvc_softc	*sc;
	struct usb_xfer		*xfer;
	uint8_t			index;
	uint8_t			*uvc_buf;
	uint32_t		buf_size;
};

struct uvc_vs_interface_info {
	struct usb_interface	*vs_iface;
	uint8_t			cur_iface_index;
	uint8_t			cur_alt_index;

	uint32_t		endpoint_addr;
	uint32_t		max_pack_size;
	struct uvc_isoc_xfer	isoc_xfer[UVC_IXFER_NUM];
};

struct uvc_format_group {
	struct uvc_uncompressed_format_descriptor	*yuv_format;
	struct uvc_mjpeg_format_descriptor		*mjpeg_format;
	uint32_t					format_type;

	struct uvc_incomplete_frame_descriptor0		*frame_array[UVC_FRAME_MAX_LEN];
	uint32_t					cur_frame_index;
	uint32_t					frame_num;
};

struct uvc_configure_msg {
	struct usb_proc_msg	hdr;
	struct uvc_softc	*sc;
	errno_t			uvc_error;
};

struct frame_buf_info {
	uint8_t			*frame_buf;
	struct v4l2_buffer	buf_info;
	struct LOS_DL_LIST	frame_queue;
};

struct uvc_softc {
	device_t				dev;
	struct usb_device			*uvc_dev;

	/* Save the interface descriptors of VC and VS for interface class matches */
	struct uvc_vc_interface_header_desc0	*vc_header_desc;
	struct uvc_vs_input_header_desc_n_0	*vs_input_desc;
	struct uvc_vs_interface_info		*vs_cur_iface;
	struct uvc_vs_interface_info		vs_iface_coll[VS_IFACE_NUM];

	/* Save streaming parameters,format and frame descriptor info for media negotiation */
	struct uvc_probe_commit_control1_1	uvc_ctrl;
	struct uvc_format_group			vs_format_array[UVC_VS_FORMAT_NUM];
	struct uvc_format_group			*vs_cur_format;
	uint32_t				vs_format_num;

	/* Used for uvc frame data stream transmission */
	uint8_t					*uvc_buf_pool;
	uint32_t				uvc_pool_size;
	uint32_t				uvc_frame_cnt;
	uint32_t				uvc_pack_cnt;
	struct frame_buf_info			uvc_frame_data[UVC_FRAME_MAX_COUNT];
	struct mtx				uvc_frame_mux;	/* Protect frame buffer related operations */
	struct LOS_DL_LIST			in_queue;	/* Buffer to store unfilled uvc frame data */
	struct LOS_DL_LIST			out_queue;	/* Buffer to store filled uvc frame data */
	EVENT_CB_S				frame_event;	/* Event for waiting for filled frame data */
	struct usb_xfer				*sc_xfer[UVC_TRANSFER_NUM];
	uint8_t 				frame_flag;
	uint8_t					uvc_fid;
	uint8_t					stream_flag;	/* Prevent repeated starts of data flow */

	uint8_t					video_flags;	/* Prevent repeated opening and closing */

	/*
	 * Callback function to be processed by the usb explore task.
	 * 2: At most two identical messages are queued in the USB process queue.
	 */
	EVENT_CB_S				uvc_event;		/* USB explore task completed event */
	struct uvc_configure_msg		sc_s_fmt_msg[2];	/* Set the data format */
	struct uvc_configure_msg		sc_s_parm_msg[2];	/* Set streaming parameters */
	struct uvc_configure_msg		sc_streamon_msg[2];	/* Start streaming I/O */
	struct uvc_configure_msg		sc_streamoff_msg[2];	/* Stop streaming I/O */

	/* Private data pointer used by the usb explore callback function */
	struct v4l2_format			*uvc_fmt;
	struct v4l2_streamparm			*uvc_parm;
};

/* Host UVC internal function */
static int	uvc_streamoff(struct uvc_softc *sc, uint32_t *type);

/* Callback function to be processed by the usb explore task */
static usb_proc_callback_t uvc_set_format_process;
static usb_proc_callback_t uvc_set_parm_process;
static usb_proc_callback_t uvc_streamon_process;
static usb_proc_callback_t uvc_streamoff_process;

/* Callback function to receive uvc data */
static usb_callback_t uvc_read_callback;
static usb_callback_t uvc_intr_callback;

/* Callback function for device insertion and removal */
static device_probe_t uvc_probe;
static device_attach_t uvc_attach;
static device_detach_t uvc_detach;

static device_method_t uvc_methods[] = {
	/* Device interface */
	DEVMETHOD(device_probe, uvc_probe),
	DEVMETHOD(device_attach, uvc_attach),
	DEVMETHOD(device_detach, uvc_detach),
	DEVMETHOD_END
};

static driver_t uvc_driver = {
	.name = "USB_VIDEO",
	.methods = uvc_methods,
	.size = sizeof(struct uvc_softc),
};

static devclass_t uvc_devclass;
DRIVER_MODULE(uvc, uhub, uvc_driver, uvc_devclass, 0, 0);

/* Configure parameters for 1 interrupt transfer and 3 isoc transfers */
static struct usb_config uvc_config[UVC_TRANSFER_NUM] = {
	[UVC_INTR] = {
		.type = UE_INTERRUPT,
		.endpoint = UE_ADDR_ANY,
		.direction = UE_DIR_IN,
		.if_index = 0,
		.flags = {.pipe_bof = 1,.short_xfer_ok = 1,.no_pipe_ok = 1,},
		.bufsize = UVC_INIR_SIZE,
		.callback = &uvc_intr_callback,
		.timeout = 0,
		.usb_mode = USB_MODE_HOST,
	},
	[UVC_XFER1] = {
		.type = UE_ISOCHRONOUS,
		.endpoint = UE_ADDR_ANY,
		.direction = UE_DIR_IN,
		.if_index = 1,
		.frames = UVC_PACK_NUM,
		.bufsize = 0,
		.flags = {.short_xfer_ok = 1,.short_frames_ok = 1,.ext_buffer = 1,},
		.callback = &uvc_read_callback,
		.timeout = 0,
		.usb_mode = USB_MODE_HOST,
	},
	[UVC_XFER2] = {
		.type = UE_ISOCHRONOUS,
		.endpoint = UE_ADDR_ANY,
		.direction = UE_DIR_IN,
		.if_index = 1,
		.frames = UVC_PACK_NUM,
		.bufsize = 0,
		.flags = {.short_xfer_ok = 1,.short_frames_ok = 1,.ext_buffer = 1,},
		.callback = &uvc_read_callback,
		.timeout = 0,
		.usb_mode = USB_MODE_HOST,
	},
	[UVC_XFER3] = {
		.type = UE_ISOCHRONOUS,
		.endpoint = UE_ADDR_ANY,
		.direction = UE_DIR_IN,
		.if_index = 1,
		.frames = UVC_PACK_NUM,
		.bufsize = 0,
		.flags = {.short_xfer_ok = 1,.short_frames_ok = 1,.ext_buffer = 1,},
		.callback = &uvc_read_callback,
		.timeout = 0,
		.usb_mode = USB_MODE_HOST,
	},
};

int	video_open(struct file *filep);
int	video_close(struct file *filep);
int	video_ioctl(struct file *filep, int cmd, unsigned long data);
int	video_poll(struct file *filep, poll_table *fds);

static const struct file_operations_vfs video_fops = {
	.open = video_open,
	.close = video_close,
	.ioctl = video_ioctl,
#ifndef CONFIG_DISABLE_POLL
	.poll = video_poll,
#endif
};

static void
uvc_explore_msg_init(struct uvc_softc *sc)
{
	sc->sc_s_fmt_msg[0].hdr.pm_callback = &uvc_set_format_process;
	sc->sc_s_fmt_msg[0].sc = sc;
	sc->sc_s_fmt_msg[1].hdr.pm_callback = &uvc_set_format_process;
	sc->sc_s_fmt_msg[1].sc = sc;

	sc->sc_s_parm_msg[0].hdr.pm_callback = &uvc_set_parm_process;
	sc->sc_s_parm_msg[0].sc = sc;
	sc->sc_s_parm_msg[1].hdr.pm_callback = &uvc_set_parm_process;
	sc->sc_s_parm_msg[1].sc = sc;

	sc->sc_streamon_msg[0].hdr.pm_callback = &uvc_streamon_process;
	sc->sc_streamon_msg[0].sc = sc;
	sc->sc_streamon_msg[1].hdr.pm_callback = &uvc_streamon_process;
	sc->sc_streamon_msg[1].sc = sc;

	sc->sc_streamoff_msg[0].hdr.pm_callback = &uvc_streamoff_process;
	sc->sc_streamoff_msg[0].sc = sc;
	sc->sc_streamoff_msg[1].hdr.pm_callback = &uvc_streamoff_process;
	sc->sc_streamoff_msg[1].sc = sc;
}

static void
uvc_explore_msg_deinit(struct uvc_softc *sc)
{
	usb_proc_explore_lock(sc->uvc_dev);

	usb_proc_explore_mwait(sc->uvc_dev, &sc->sc_s_fmt_msg[0], &sc->sc_s_fmt_msg[1]);
	sc->sc_s_fmt_msg[0].uvc_error = -1;
	(void)LOS_EventWrite(&sc->uvc_event, SFMT_EVENT);

	usb_proc_explore_mwait(sc->uvc_dev, &sc->sc_s_parm_msg[0], &sc->sc_s_parm_msg[1]);
	sc->sc_s_parm_msg[0].uvc_error = -1;
	(void)LOS_EventWrite(&sc->uvc_event, SPARM_EVENT);

	usb_proc_explore_mwait(sc->uvc_dev, &sc->sc_streamon_msg[0], &sc->sc_streamon_msg[1]);
	sc->sc_streamon_msg[0].uvc_error = -1;
	(void)LOS_EventWrite(&sc->uvc_event, STREAMON_EVENT);

	usb_proc_explore_mwait(sc->uvc_dev, &sc->sc_streamon_msg[0], &sc->sc_streamon_msg[1]);
	sc->sc_streamon_msg[0].uvc_error = -1;
	(void)LOS_EventWrite(&sc->uvc_event, STREAMOFF_EVENT);

	usb_proc_explore_unlock(sc->uvc_dev);
}

static usb_error_t
uvc_isoc_xfer_init(struct uvc_softc *sc)
{
	struct uvc_isoc_xfer *isoc_xfer;
	int i;

	for (i = 0; i < UVC_IXFER_NUM; i++) {
		if (sc->sc_xfer[i + 1] == NULL) {
			return (USB_ERR_NOMEM);
		}
		isoc_xfer = &sc->vs_cur_iface->isoc_xfer[i];
		isoc_xfer->sc = sc;
		isoc_xfer->xfer = sc->sc_xfer[i + 1];
		isoc_xfer->index = i;
		isoc_xfer->uvc_buf = memalign(USB_PAGE_SIZE, sc->uvc_pack_cnt * USB_PAGE_SIZE);
		if (isoc_xfer->uvc_buf == NULL) {
			return (USB_ERR_NOMEM);
		}
		isoc_xfer->buf_size = USB_PAGE_SIZE;
	}

	return (USB_ERR_NORMAL_COMPLETION);
}

static void
uvc_isoc_xfer_deinit(struct uvc_softc *sc)
{
	struct uvc_isoc_xfer *isoc_xfer;
	int i;

	for (i = 0; i < UVC_IXFER_NUM; i++) {
		isoc_xfer = &sc->vs_cur_iface->isoc_xfer[i];
		if (isoc_xfer->uvc_buf != NULL) {
			free(isoc_xfer->uvc_buf);
			isoc_xfer->uvc_buf = NULL;
		}

		if (isoc_xfer->xfer != NULL) {
			isoc_xfer->xfer = NULL;
		}
	}
}

static void
uvc_frame_buf_free(struct uvc_softc *sc)
{
	struct frame_buf_info *frame_data;

	mtx_lock(&sc->uvc_frame_mux);
	while (!LOS_ListEmpty(&sc->out_queue)) {
		frame_data = LOS_LIST_FIRST_ENTRY(&sc->out_queue, struct frame_buf_info, frame_queue);
		LOS_ListDelInit(&frame_data->frame_queue);
	}

	while (!LOS_ListEmpty(&sc->in_queue)) {
		frame_data = LOS_LIST_FIRST_ENTRY(&sc->in_queue, struct frame_buf_info, frame_queue);
		LOS_ListDelInit(&frame_data->frame_queue);
	}

	if (sc->uvc_buf_pool != NULL) {
		free(sc->uvc_buf_pool);
		sc->uvc_buf_pool = NULL;
	}
	sc->uvc_frame_cnt = 0;
	mtx_unlock(&sc->uvc_frame_mux);
}

static void
uvc_transfer_close(struct uvc_softc *sc)
{
	usbd_transfer_unsetup(sc->sc_xfer, UVC_TRANSFER_NUM);

	(void)usbd_set_alt_interface_index(sc->uvc_dev,
	    sc->vs_cur_iface->cur_iface_index, 0);

	uvc_isoc_xfer_deinit(sc);
	uvc_frame_buf_free(sc);
	sc->stream_flag = 0;
}

static int
uvc_probe(device_t dev)
{
	struct usb_attach_arg *uaa = device_get_ivars(dev);
	struct usb_interface_descriptor *uvc_iface;

	if ((uaa == NULL) || (uaa->iface == NULL)) {
		return (ENXIO);
	}

	uvc_iface = usbd_get_interface_descriptor(uaa->iface);
	if (uvc_iface == NULL) {
		return (ENXIO);
	}

	if ((uvc_iface->bInterfaceClass == UICLASS_VIDEO) &&
	    (uvc_iface->bInterfaceSubClass == UICLASS_VIDEO_CONTROL)) {
		return (0);
	}

	return (ENXIO);
}

static usb_error_t
uvc_parse_header_desc(struct uvc_softc *sc)
{
	struct usb_descriptor *usb_desc = NULL;
	struct uvc_vs_input_header_desc_n_0 *vs_header = NULL;
	struct uvc_vc_interface_header_desc0 *vc_header = NULL;
	struct usb_config_descriptor *config_desc;
	uint16_t input_header_size = 0;
	uint16_t interface_header_size = 0;

	config_desc = usbd_get_config_descriptor(sc->uvc_dev);
	for (; ;) {
		usb_desc = usb_desc_foreach(config_desc, usb_desc);
		if (usb_desc == NULL) {
			break;
		}
		if ((usb_desc->bDescriptorType != USB_UVC_CS_INTERFACE) ||
		    (usb_desc->bDescriptorSubtype != USB_UVC_VS_INPUT_HEADER) ||
		    (usb_desc->bDescriptorSubtype != UVC_VC_HEADER)) {
			continue;
		}

		if (usb_desc->bLength >= sizeof(struct uvc_vs_input_header_desc_n_0)) {
			vs_header = (struct uvc_vs_input_header_desc_n_0 *)usb_desc;
			input_header_size = vs_header->bControlSize * vs_header->bNumFormats +
			    sizeof(struct uvc_vs_input_header_desc_n_0);
		}

		if (usb_desc->bLength >= sizeof(struct uvc_vc_interface_header_desc0)) {
			vc_header = (struct uvc_vc_interface_header_desc0 *)usb_desc;
			interface_header_size = vc_header->bInCollection +
			    sizeof(struct uvc_vc_interface_header_desc0);
		}

		if ((vs_header != NULL) && (vs_header->bNumFormats != 0) &&
		    (input_header_size == vs_header->bLength)) {
			sc->vs_input_desc = vs_header;
		} else if ((vc_header != NULL) && (vc_header->bInCollection != 0) &&
		    (interface_header_size == vc_header->bLength)) {
			sc->vc_header_desc = vc_header;
		}
	}

	if ((sc->vs_input_desc == NULL) || (sc->vc_header_desc == NULL)) {
		usb_err("VC interface header descriptor or VS input header descriptor not found!\n");
		return (USB_ERR_INVAL);
	}

	return (USB_ERR_NORMAL_COMPLETION);
}

static usb_error_t
uvc_find_frame_desc(struct uvc_softc *sc, struct usb_descriptor *usb_desc,
    struct uvc_format_group *format_group)
{
	struct uvc_common_format_descriptor *format_desc;
	struct usb_config_descriptor *config_desc;
	struct uvc_incomplete_frame_descriptor0 *frame_desc;
	uint32_t frame_index = 0;
	uint8_t default_index;
	uint8_t frame_type;

	format_desc = (struct uvc_common_format_descriptor *)usb_desc;
	if ((format_desc->bNumFrameDescriptors == 0) ||
	    (format_desc->bNumFrameDescriptors > UVC_FRAME_MAX_LEN)) {
		return (USB_ERR_INVAL);
	}

	switch (format_desc->bDescriptorSubtype) {
	case USB_UVC_VS_FORMAT_UNCOMPRESSED:
		frame_type = USB_UVC_VS_FRAME_UNCOMPRESSED;
		default_index = format_group->yuv_format->bDefaultFrameIndex;
		break;
	case USB_UVC_VS_FORMAT_MJPEG:
		frame_type = USB_UVC_VS_FRAME_MJPEG;
		default_index = format_group->mjpeg_format->bDefaultFrameIndex;
		break;

	default:
		return (USB_ERR_INVAL);
	}

	if ((default_index == 0) ||
	    (default_index > format_desc->bNumFrameDescriptors)) {
		default_index = 1;
	}

	config_desc = usbd_get_config_descriptor(sc->uvc_dev);
	for (; ;) {
		usb_desc = usb_desc_foreach(config_desc, usb_desc);
		if ((usb_desc == NULL) || (usb_desc->bDescriptorType != UDESC_CS_INTERFACE) ||
		    (usb_desc->bDescriptorSubtype != frame_type)) {
			break;
		}
		if (frame_index >= format_desc->bNumFrameDescriptors) {
			break;
		}

		frame_desc = (struct uvc_incomplete_frame_descriptor0 *)usb_desc;
		format_group->frame_array[frame_index++] = frame_desc;
		if (default_index == frame_desc->bFrameIndex) {
			format_group->cur_frame_index = default_index - 1;
		}
	}

	if (frame_index == 0) {
		return (USB_ERR_INVAL);
	}
	format_group->frame_num = frame_index;

	return (USB_ERR_NORMAL_COMPLETION);
}

static usb_error_t
uvc_parse_uncompressed_desc(struct uvc_softc *sc, struct usb_descriptor *usb_desc, uint8_t format_index)
{
	struct uvc_uncompressed_format_descriptor *yuv_desc;
	struct uvc_format_group *yuv_group;
	int i;

	yuv_desc = (struct uvc_uncompressed_format_descriptor *)usb_desc;
	yuv_group = &sc->vs_format_array[format_index];

	for (i = 0; i < UNCOMPRESSED_FORMAT_NUM; i++) {
		if (strcmp((const char *)yuv_desc->guidFormat, uvc_fmt_enums[i].name) == 0) {
			break;
		}
	}
	if (i == UNCOMPRESSED_FORMAT_NUM) {
		/* Skip the unsupported formats to ensure that the supported formats are saved */
		usb_warn("Unknown format: %s\n", (const char *)yuv_desc->guidFormat);
		return (USB_ERR_NORMAL_COMPLETION);
	}

	yuv_group->format_type = uvc_fmt_enums[i].pixel_fmt;
	yuv_group->yuv_format = yuv_desc;

	return (uvc_find_frame_desc(sc, usb_desc, yuv_group));
}

static usb_error_t
uvc_parse_mjpeg_desc(struct uvc_softc *sc, struct usb_descriptor *usb_desc, uint8_t format_index)
{
	struct uvc_mjpeg_format_descriptor *mjpeg_desc;
	struct uvc_format_group *mjpeg_group;

	mjpeg_desc = (struct uvc_mjpeg_format_descriptor *)usb_desc;
	mjpeg_group = &sc->vs_format_array[format_index];

	mjpeg_group->format_type = V4L2_PIX_FMT_MJPEG;
	mjpeg_group->mjpeg_format = mjpeg_desc;

	return (uvc_find_frame_desc(sc, usb_desc, mjpeg_group));
}

static usb_error_t
uvc_parse_format_and_frame(struct uvc_softc *sc)
{
	struct usb_descriptor *usb_desc = NULL;
	struct usb_config_descriptor *config_desc;
	uint8_t format_index = 0;
	usb_error_t ret;

	config_desc = usbd_get_config_descriptor(sc->uvc_dev);
	for (; ;) {
		usb_desc = usb_desc_foreach(config_desc, usb_desc);
		if (usb_desc == NULL) {
			break;
		}
		if (usb_desc->bDescriptorType != UDESC_CS_INTERFACE) {
			continue;
		}

		if ((usb_desc->bDescriptorSubtype == USB_UVC_VS_FORMAT_UNCOMPRESSED) &&
		    (usb_desc->bLength == sizeof(struct uvc_uncompressed_format_descriptor))) {
			ret = uvc_parse_uncompressed_desc(sc, usb_desc, format_index);
		} else if ((usb_desc->bDescriptorSubtype == USB_UVC_VS_FORMAT_MJPEG) &&
		    (usb_desc->bLength == sizeof(struct uvc_mjpeg_format_descriptor))) {
			ret = uvc_parse_mjpeg_desc(sc, usb_desc, format_index);
		} else {
			continue;
		}

		if (ret != USB_ERR_NORMAL_COMPLETION) {
			return (ret);
		}

		format_index++;
	}

	if (format_index == 0) {
		return (USB_ERR_INVAL);
	}
	sc->vs_cur_format = sc->vs_format_array;
	sc->vs_format_num = format_index;
	return (USB_ERR_NORMAL_COMPLETION);
}

static usb_error_t
uvc_find_and_save_endpoint(struct uvc_softc *sc, struct uvc_vs_interface_info *iface_info,
    uint8_t iface_index, uint8_t init_flag)
{
	struct usb_interface_descriptor *vs_interface;
	struct usb_endpoint_descriptor *vs_endpoint;
	struct usb_config_descriptor *config_desc;
	struct usb_descriptor *usb_desc = NULL;
	uint32_t payload_size = sc->uvc_ctrl.dwMaxPayloadTransferSize;
	uint32_t bandwidth = UINT_MAX;
	uint32_t pack_size;
	uint8_t flag = 0;

	/*
	 * The endpoint descriptor follows the interface descriptor,
	 * so look up the interface descriptor first and then the endpoint descriptor
	 */
	config_desc = usbd_get_config_descriptor(sc->uvc_dev);
	for (; ;) {
		usb_desc = usb_desc_foreach(config_desc, usb_desc);
		if (usb_desc == NULL) {
			break;
		}

		if ((usb_desc->bDescriptorType != UDESC_INTERFACE) ||
		    (usb_desc->bLength < sizeof(struct usb_interface_descriptor)) ||
		    (((struct usb_interface_descriptor *)usb_desc)->bInterfaceNumber != iface_index) ||
		    (((struct usb_interface_descriptor *)usb_desc)->bNumEndpoints == 0)) {
			continue;
		}
		vs_interface = (struct usb_interface_descriptor *)usb_desc;

		usb_desc = usb_desc_foreach(config_desc, usb_desc);
		if ((usb_desc == NULL) || (usb_desc->bDescriptorType != UDESC_ENDPOINT) ||
		    (UE_GET_DIR(((struct usb_endpoint_descriptor *)usb_desc)->bEndpointAddress) != UE_DIR_IN) ||
		    (UE_GET_XFERTYPE(((struct usb_endpoint_descriptor *)usb_desc)->bmAttributes) != UE_ISOCHRONOUS)) {
			continue;
		}
		vs_endpoint = (struct usb_endpoint_descriptor *)usb_desc;
		pack_size = UE_GET_SIZE(UGETW(vs_endpoint->wMaxPacketSize)) *
		    (1 + UE_GET_TRANS(UGETW(vs_endpoint->wMaxPacketSize)));

		if (init_flag && (iface_info->max_pack_size < (uint32_t)UGETW(vs_endpoint->wMaxPacketSize))) {
			flag = 1;
			iface_info->max_pack_size = UGETW(vs_endpoint->wMaxPacketSize);
			iface_info->vs_iface = &sc->uvc_dev->ifaces[iface_index];
			iface_info->cur_iface_index = iface_index;
			iface_info->cur_alt_index = vs_interface->bAlternateSetting;
			iface_info->endpoint_addr = vs_endpoint->bEndpointAddress;
		} else if (!init_flag && (pack_size >= payload_size) && (pack_size <= bandwidth)) {
			/* Save endpoint information for requesting bandwidth */
			bandwidth = pack_size;
			iface_info->max_pack_size = pack_size;
			iface_info->cur_alt_index = vs_interface->bAlternateSetting;
			iface_info->endpoint_addr = vs_endpoint->bEndpointAddress;
			if (pack_size == payload_size) {
				return (USB_ERR_NORMAL_COMPLETION);
			}
		}
	}

	if (init_flag && !flag) {
		return (USB_ERR_INVAL);
	}

	return (USB_ERR_NORMAL_COMPLETION);
}

static usb_error_t
uvc_parse_endpoint_desc(struct uvc_softc *sc)
{
	uint8_t *interface_nr = sc->vc_header_desc->baInterfaceNr;
	uint8_t i;
	usb_error_t ret;

	if (sc->vc_header_desc->bInCollection >= VS_IFACE_NUM) {
		return (USB_ERR_INVAL);
	}

	for (i = 0; i < sc->vc_header_desc->bInCollection; i++) {
		ret = uvc_find_and_save_endpoint(sc, &sc->vs_iface_coll[i], interface_nr[i], 1);
		if (ret != USB_ERR_NORMAL_COMPLETION) {
			return (ret);
		}
	}

	sc->vs_cur_iface = sc->vs_iface_coll;
	return (USB_ERR_NORMAL_COMPLETION);
}

static usb_error_t
uvc_parse_desc(struct uvc_softc *sc)
{
	usb_error_t ret;

	ret = uvc_parse_header_desc(sc);
	if (ret != USB_ERR_NORMAL_COMPLETION) {
		return (ret);
	}

	ret = uvc_parse_format_and_frame(sc);
	if (ret != USB_ERR_NORMAL_COMPLETION) {
		usb_err("Format descriptor or frame descriptor is invalid!\n");
		return (ret);
	}

	ret = uvc_parse_endpoint_desc(sc);
	if (ret != USB_ERR_NORMAL_COMPLETION) {
		usb_err("Frame descriptor is invalid!\n");
		return (ret);
	}

	ret = usbd_req_set_alt_interface_no(sc->uvc_dev, NULL, sc->vs_cur_iface->cur_iface_index, 0);
	if (ret != USB_ERR_NORMAL_COMPLETION) {
		return (ret);
	}

	return (USB_ERR_NORMAL_COMPLETION);
}

static usb_error_t
uvc_setup_vs_probe(struct uvc_softc *sc, struct uvc_probe_commit_control1_1 *ctrl,
    uint8_t request_type, uint8_t request, uint8_t value)
{
	/*
	 * Refer to USB_Video_Example 1.1 and 1.5, 0x110: UVC version 1.1.
	 * 26: UVC 1.0 control request data size. 34: UVC 1.1 control request data size.
	 * The data size of the control request of the UVC video stream interfacecan are
	 * 26 bytes, 34 bytes, and 48 bytes, which correspond to the UVC 1.0, UVC 1.1 and UVC 1.5 versions.
	 */
	uint8_t length = 26;
	struct usb_device_request uvc_req;
	(void)memset_s(&uvc_req, sizeof(struct usb_device_request), 0, sizeof(struct usb_device_request));

	if (sc->vc_header_desc->bcdUVC == 0x110) {
		length = 34;
	}

	uvc_req.bmRequestType = request_type;
	uvc_req.bRequest = request;
	USETW(uvc_req.wValue, value << 8); /* 8: Shift left by 8 bits. */
	USETW(uvc_req.wIndex, sc->vs_cur_iface->cur_iface_index);
	USETW(uvc_req.wLength, length);
	return (usbd_do_request(sc->uvc_dev, NULL, &uvc_req, ctrl));
}

static usb_error_t
uvc_probe_control(struct uvc_softc *sc)
{
	struct uvc_format_group *uvc_group;
	struct uvc_probe_commit_control1_1 probe_ctrl;

	uvc_group = sc->vs_cur_format;

	(void)memset_s(&probe_ctrl, sizeof(struct uvc_probe_commit_control1_1),
	    0, sizeof(struct uvc_probe_commit_control1_1));
	if (uvc_group->format_type == V4L2_PIX_FMT_MJPEG) {
		probe_ctrl.bFormatIndex = uvc_group->mjpeg_format->bFormatIndex;
	} else {
		probe_ctrl.bFormatIndex = uvc_group->yuv_format->bFormatIndex;
	}
	probe_ctrl.bmHint = 1;
	probe_ctrl.bFrameIndex = uvc_group->frame_array[uvc_group->cur_frame_index]->bFrameIndex;
	probe_ctrl.dwFrameInterval = uvc_group->frame_array[uvc_group->cur_frame_index]->dwDefaultFrameInterval;

	if (uvc_setup_vs_probe(sc, &probe_ctrl, UVC_SET_INTERFACE, UVC_RC_SETCUR, USB_UVC_VS_PROBE_CONTROL) !=
	    USB_ERR_NORMAL_COMPLETION) {
		return (USB_ERR_IOERROR);
	}

	return (USB_ERR_NORMAL_COMPLETION);
}

static int
uvc_attach(device_t dev)
{
	struct usb_attach_arg *uaa = device_get_ivars(dev);
	struct uvc_softc *sc = device_get_softc(dev);
	usb_error_t ret;
	int err;

	if ((uaa == NULL) || (uaa->device == NULL)) {
		return (-EINVAL);
	}

	sc->dev = dev;
	sc->uvc_dev = uaa->device;

	(void)LOS_EventInit(&sc->uvc_event);
	(void)LOS_EventInit(&sc->frame_event);
	LOS_ListInit(&sc->in_queue);
	LOS_ListInit(&sc->out_queue);
	mtx_init(&sc->uvc_frame_mux, NULL, NULL, MTX_RECURSE);

	ret = uvc_parse_desc(sc);
	if (ret != USB_ERR_NORMAL_COMPLETION) {
		err = -ENXIO;
		goto err_out;
	}

	ret = uvc_probe_control(sc);
	if (ret != USB_ERR_NORMAL_COMPLETION) {
		err = -EIO;
		goto err_out;
	}

	err = register_driver(USB_HOST_UVC_DEV, &video_fops, 0666, (FAR void *)sc);
	if (err < 0) {
		usb_err("register_driver() failed: %d\n", err);
		goto err_out;
	}

	uvc_explore_msg_init(sc);
	/* Never del the mutex */
	usb_lock_init(&protection_softc.uvc_mux);
	protection_softc.uvc_attached = 1;

	device_printf(dev, "video connected\n");

	return (0);

err_out:
	mtx_destroy(&sc->uvc_frame_mux);
	(void)LOS_EventDestroy(&sc->frame_event);
	(void)LOS_EventDestroy(&sc->uvc_event);
	return (err);
}

static int
uvc_detach(device_t dev)
{
	int ret;
	struct uvc_softc *sc = device_get_softc(dev);
	uvc_explore_msg_deinit(sc);
	(void)LOS_EventWrite(&sc->frame_event, UVC_DETACH_EVENT);

	(void)usb_mutex_lock(&protection_softc.uvc_mux);

	protection_softc.uvc_attached = 0;
	ret = unregister_driver(USB_HOST_UVC_DEV);
	if (ret < 0) {
		usb_err("unregister_driver() failed: %d\n", ret);
	}

	uvc_transfer_close(sc);

	mtx_destroy(&sc->uvc_frame_mux);
	(void)LOS_EventDestroy(&sc->frame_event);
	(void)LOS_EventDestroy(&sc->uvc_event);
	(void)usb_mutex_unlock(&protection_softc.uvc_mux);
	device_printf(dev, "video disconnected\n");
	return (0);
}

int
video_open(struct file *filep)
{
	struct uvc_softc *sc = (struct uvc_softc *)filep->f_inode->i_private;
	int ret;

	if (sc == NULL) {
		return (-EPERM);
	}

	(void)usb_mutex_lock(&protection_softc.uvc_mux);
	if ((protection_softc.uvc_attached == 0) || (filep->f_inode->i_flags & FSNODEFLAG_DELETED)) {
		ret = -ENODEV;
		goto out;
	}

	if (sc->video_flags & UVC_FLAG_ACTIVE) {
		ret = -EBUSY;
		goto out;
	}

	sc->video_flags |= UVC_FLAG_ACTIVE;
	ret = ENOERR;

out:
	(void)usb_mutex_unlock(&protection_softc.uvc_mux);
	return (ret);
}

int
video_close(struct file *filep)
{
	struct uvc_softc *sc = (struct uvc_softc *)filep->f_inode->i_private;
	uint32_t type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	int ret;

	if (sc == NULL) {
		return (-EPERM);
	}

	(void)usb_mutex_lock(&protection_softc.uvc_mux);
	if ((protection_softc.uvc_attached == 0) || (filep->f_inode->i_flags & FSNODEFLAG_DELETED)) {
		ret = -ENODEV;
		goto out;
	}

	if (!(sc->video_flags & UVC_FLAG_ACTIVE)) {
		ret = -EIO;
		goto out;
	}

	(void)uvc_streamoff(sc, &type);

	sc->video_flags &= ~UVC_FLAG_ACTIVE;
	ret = ENOERR;

out:
	(void)usb_mutex_unlock(&protection_softc.uvc_mux);
	return (ret);
}

static int
uvc_querycap(struct uvc_softc *sc, struct v4l2_capability *data)
{
	errno_t err;

	if (data == NULL) {
		return (-EINVAL);
	}

	(void)memset_s(data, sizeof(struct v4l2_capability), 0, sizeof(struct v4l2_capability));

	err = strncpy_s((char *)data->driver, sizeof(data->driver), DRIVER_NAME(sc), strlen(DRIVER_NAME(sc)));
	if (err != EOK) {
		return (err);
	}

	err = strncpy_s((char *)data->card, sizeof(data->card), uvc_driver.name, strlen(uvc_driver.name));
	if (err != EOK) {
		return (err);
	}

	err = strncpy_s((char *)data->bus_info, sizeof(data->bus_info),
	    BUF_INFO_NAME, sizeof(BUF_INFO_NAME) - 1);
	if (err != EOK) {
		return (err);
	}

	data->version = 1 << 8 | 1 << 0; /* UVC version: 1.1 */
	data->capabilities = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING;

	return (ENOERR);
}

static struct uvc_format_group *
uvc_find_format_by_type(struct uvc_softc *sc, uint32_t type)
{
	uint32_t i;
	struct uvc_format_group *format = NULL;

	for (i = 0; i < sc->vs_format_num; i++) {
		if (sc->vs_format_array[i].format_type == type) {
			format = &sc->vs_format_array[i];
			break;
		}
	}
	return format;
}

static usb_error_t
set_v4l2fmt_by_type(struct v4l2_fmtdesc *fmtdesc, uint32_t pixel_fmt)
{
	errno_t err;
	uint32_t index;

	fmtdesc->pixelformat = 0;

	for (index = 0; index < ARRAY_SIZE(uvc_fmt_enums); index++) {
		if (pixel_fmt == uvc_fmt_enums[index].pixel_fmt) {
			fmtdesc->pixelformat = pixel_fmt;
			break;
		}
	}
	if (index == ARRAY_SIZE(uvc_fmt_enums)) {
		index--;
	}
	err = memcpy_s(fmtdesc->description, sizeof(fmtdesc->description), uvc_fmt_enums[index].name,
	    sizeof(uvc_fmt_enums[index].name));
	if (err != EOK) {
		return (USB_ERR_INVAL);
	}

	return (USB_ERR_NORMAL_COMPLETION);
}

static int
uvc_enum_fmtdesc(struct uvc_softc *sc, struct v4l2_fmtdesc *fmtdesc)
{
	usb_error_t ret;
	uint32_t type;

	if ((fmtdesc == NULL) || (fmtdesc->type != V4L2_BUF_TYPE_VIDEO_CAPTURE) ||
	    (fmtdesc->index >= sc->vs_format_num)) {
		return (-EINVAL);
	}

	type = sc->vs_format_array[fmtdesc->index].format_type;
	if (type == V4L2_PIX_FMT_MJPEG) {
		fmtdesc->flags = V4L2_FMT_FLAG_COMPRESSED;
	} else {
		fmtdesc->flags = V4L2_FMT_FLAG_UNCOMPRESSED;
	}

	ret = set_v4l2fmt_by_type(fmtdesc, type);
	if (ret != USB_ERR_NORMAL_COMPLETION) {
		return (-EINVAL);
	}

	(void)memset_s(fmtdesc->reserved, sizeof(fmtdesc->reserved), 0, sizeof(fmtdesc->reserved));

	return (ENOERR);
}

static int
uvc_enum_framesize(struct uvc_softc *sc, struct v4l2_frmsizeenum *fsizes)
{
	struct uvc_format_group *format;
	struct uvc_incomplete_frame_descriptor0 *frame;

	if (fsizes == NULL) {
		return (-EINVAL);
	}

	format = uvc_find_format_by_type(sc, fsizes->pixel_format);
	if ((format == NULL) || (fsizes->index >= format->frame_num)) {
		return (-EINVAL);
	}

	frame = format->frame_array[fsizes->index];

	fsizes->type = V4L2_FRMSIZE_TYPE_DISCRETE;
	fsizes->discrete.width = frame->wWidth;
	fsizes->discrete.height = frame->wHeight;

	return (ENOERR);
}

static int
uvc_enum_fivals(struct uvc_softc *sc, struct v4l2_frmivalenum *fivals)
{
	int ivals_index = 0;
	uint16_t width;
	uint16_t height;
	uint32_t frame_ivals_size;
	uint32_t i;
	struct uvc_format_group *format;
	struct uvc_incomplete_frame_descriptor0 *frame = NULL;

	if (fivals == NULL) {
		return (-EINVAL);
	}

	format = uvc_find_format_by_type(sc, fivals->pixel_format);
	if (format == NULL) {
		return (-EINVAL);
	}

	/* Find frame by width and height. */
	for (i = 0; i < format->frame_num; i++) {
		width = format->frame_array[i]->wWidth;
		height = format->frame_array[i]->wHeight;
		if ((width == fivals->width) && (height == fivals->height)) {
			frame = format->frame_array[i];
			break;
		}
	}
	if (frame == NULL) {
		return (-EINVAL);
	}

	frame_ivals_size = (frame->bLength - sizeof(struct uvc_incomplete_frame_descriptor0)) / sizeof(uint32_t);
	if (frame_ivals_size == 0) {
		return (-EFAULT);
	}
	/*
	 * Refer to USB_Video_Payload_Frame_Based_1.1 section 3.1.2.
	 * bFrameIntervalType == 0: continuous; 3: continuous frame interival array size.
	 * bFrameIntervalType > 0: discrete, discrete frame interival array size == bFrameIntervalType.
	 */
	if ((frame->bFrameIntervalType == 0) && (frame_ivals_size == 3)) {
		if (fivals->index != 0) {
			return (-EINVAL);
		}
		fivals->type = V4L2_FRMIVAL_TYPE_STEPWISE;
		fivals->stepwise.min.denominator = FRAME_INTERVAL_DENOMINATOR;
		fivals->stepwise.min.numerator = frame->dwFrameInterval[ivals_index++];
		fivals->stepwise.max.denominator = FRAME_INTERVAL_DENOMINATOR;
		fivals->stepwise.max.numerator = frame->dwFrameInterval[ivals_index++];
		fivals->stepwise.step.denominator = FRAME_INTERVAL_DENOMINATOR;
		fivals->stepwise.step.numerator = frame->dwFrameInterval[ivals_index++];
	} else {
		if ((fivals->index >= frame->bFrameIntervalType) || (fivals->index >= frame_ivals_size)) {
			return (-EINVAL);
		}
		fivals->type = V4L2_FRMIVAL_TYPE_DISCRETE;
		fivals->discrete.denominator = FRAME_INTERVAL_DENOMINATOR;
		fivals->discrete.numerator = frame->dwFrameInterval[fivals->index];
	}

	return (ENOERR);
}

static void
uvc_match_format_and_frame(struct uvc_softc *sc, struct uvc_format_group **format,
    uint32_t *frame_index, struct v4l2_format *fmt)
{
	int diff;
	int diff_min = 0;
	uint16_t width;
	uint16_t height;
	uint32_t area;
	uint32_t temp;
	uint32_t i;

	*format = uvc_find_format_by_type(sc, fmt->fmt.pix.pixelformat);
	if (*format == NULL) {
		return;
	}

	area = fmt->fmt.pix.width * fmt->fmt.pix.height;
	for (i = 0; i < (*format)->frame_num; i++) {
		width = (*format)->frame_array[i]->wWidth;
		height = (*format)->frame_array[i]->wHeight;
		/* Match resolution based on width and height. */
		if ((width == fmt->fmt.pix.width) && (height == fmt->fmt.pix.height)) {
			*frame_index = i;
			return;
		}
		/* If don't have the same, find the closest one. */
		temp = width * height;
		diff = abs((int)(temp - area));
		if ((i == 0) || (diff <= diff_min)) {
			diff_min = diff;
			*frame_index = i;
		}
	}
}

static usb_error_t
check_probe_result(struct uvc_softc *sc, struct uvc_probe_commit_control1_1 *probe,
    struct uvc_format_group **format)
{
	uint32_t i;
	uint8_t index;
	struct uvc_incomplete_frame_descriptor0 *frame;

	if ((*format)->format_type == V4L2_PIX_FMT_MJPEG) {
		index = (*format)->mjpeg_format->bFormatIndex;
	} else {
		index = (*format)->yuv_format->bFormatIndex;
	}

	/*
	 * If negotiated bFormatIndex is different from the current one, find format
	 * based on the negotiated bFormatIndex, if not found, return error.
	 */
	if (index != probe->bFormatIndex) {
		*format = uvc_find_format_by_type(sc, probe->bFormatIndex);
		if (*format == NULL) {
			return (USB_ERR_INVAL);
		}
	}

	/*
	 * If negotiated bFrameIndex is different from the current one, find frame
	 * based on the negotiated bFrameIndex, if not found, return error.
	 */
	if (probe->bFrameIndex != (*format)->frame_array[(*format)->cur_frame_index]->bFrameIndex) {
		for (i = 0; i < (*format)->frame_num; i++) {
			if ((*format)->frame_array[i]->bFrameIndex == probe->bFrameIndex) {
				(*format)->cur_frame_index = i;
				break;
			}
		}
		if (i == (*format)->frame_num) {
			return (USB_ERR_INVAL);
		}
	}

	frame = (*format)->frame_array[(*format)->cur_frame_index];
	/*
	 * Calculate dwMaxVideoFrameSize. Some UVC 1.0 device dwMaxVideoFrameSize is 0.
	 * 8: 8 bits per byte. 0x110: UVC 1.1 version.
	 */
	if (frame->bDescriptorSubtype == USB_UVC_VS_FRAME_UNCOMPRESSED) {
		probe->dwMaxVideoFrameSize =
		    frame->wWidth * frame->wHeight * (*format)->yuv_format->bBitsPerPixel / 8;
	} else {
		if ((probe->dwMaxVideoFrameSize == 0) && (sc->vc_header_desc->bcdUVC < 0x110)) {
			probe->dwMaxVideoFrameSize = frame->dwMaxVideoFrameBufferSize;
		}
	}

	return (USB_ERR_NORMAL_COMPLETION);
}

static usb_error_t
uvc_vs_negotiation(struct uvc_softc *sc, struct uvc_probe_commit_control1_1 *probe_ctrl,
    uint32_t frameival, uint8_t check)
{
	usb_error_t ret;
	struct uvc_format_group *format = sc->vs_cur_format;
	uint32_t frame_index = format->cur_frame_index;

	(void)memset_s(probe_ctrl, sizeof(struct uvc_probe_commit_control1_1), 0,
	    sizeof(struct uvc_probe_commit_control1_1));

	probe_ctrl->bmHint = 1;
	if (format->format_type == V4L2_PIX_FMT_MJPEG) {
		probe_ctrl->bFormatIndex = format->mjpeg_format->bFormatIndex;
	} else {
		probe_ctrl->bFormatIndex = format->yuv_format->bFormatIndex;
	}
	probe_ctrl->bFrameIndex = format->frame_array[frame_index]->bFrameIndex;
	probe_ctrl->dwFrameInterval = frameival;

	/* Set Video Probe Control Request. */
	ret = uvc_setup_vs_probe(sc, probe_ctrl, UVC_SET_INTERFACE, UVC_RC_SETCUR, USB_UVC_VS_PROBE_CONTROL);
	if (ret != USB_ERR_NORMAL_COMPLETION) {
		return (ret);
	}

	(void)memset_s(probe_ctrl, sizeof(struct uvc_probe_commit_control1_1), 0,
	    sizeof(struct uvc_probe_commit_control1_1));
	/* Get Video Probe Control Request. */
	ret = uvc_setup_vs_probe(sc, probe_ctrl, UVC_GET_INTERFACE, UVC_RC_GETCUR, USB_UVC_VS_PROBE_CONTROL);
	if (ret != USB_ERR_NORMAL_COMPLETION) {
		return (ret);
	}

	if (check) {
		ret = check_probe_result(sc, probe_ctrl, &format);
		if (ret != USB_ERR_NORMAL_COMPLETION) {
			return (ret);
		}
	}

	/* Set Video Commit Control Request. */
	ret = uvc_setup_vs_probe(sc, probe_ctrl, UVC_SET_INTERFACE, UVC_RC_SETCUR, USB_UVC_VS_COMMIT_CONTROL);
	if (ret != USB_ERR_NORMAL_COMPLETION) {
		return (ret);
	}

	return (USB_ERR_NORMAL_COMPLETION);
}

static void
uvc_set_format_process(struct usb_proc_msg *pm)
{
	errno_t err;
	usb_error_t ret;
	uint16_t width;
	uint16_t height;
	uint32_t cur_frame_index = 0;
	uint32_t prv_frame_index;
	uint32_t frame_ival;
	struct uvc_softc *sc = ((struct uvc_configure_msg *)pm)->sc;
	struct v4l2_format *fmt = sc->uvc_fmt;
	struct uvc_format_group *cur_fmt = NULL;
	struct uvc_format_group *prv_fmt = sc->vs_cur_format;
	struct uvc_probe_commit_control1_1 probe_ctrl;

	prv_frame_index = prv_fmt->cur_frame_index;

	usb_proc_explore_unlock(sc->uvc_dev);

	uvc_match_format_and_frame(sc, &cur_fmt, &cur_frame_index, fmt);
	if (cur_fmt == NULL) {
		err = -EINVAL;
		goto out;
	}

	width = cur_fmt->frame_array[cur_frame_index]->wWidth;
	height = cur_fmt->frame_array[cur_frame_index]->wHeight;

	sc->vs_cur_format = cur_fmt;
	cur_fmt->cur_frame_index = cur_frame_index;
	frame_ival = cur_fmt->frame_array[cur_frame_index]->dwDefaultFrameInterval;

	ret = uvc_vs_negotiation(sc, &probe_ctrl, frame_ival, 1);
	if (ret != USB_ERR_NORMAL_COMPLETION) {
		err = -ENXIO;
		goto err_out;
	}

	err = memcpy_s(&sc->uvc_ctrl, sizeof(struct uvc_probe_commit_control1_1), &probe_ctrl,
	    sizeof(struct uvc_probe_commit_control1_1));
	if (err != EOK) {
		err = -EFAULT;
		goto err_out;
	}
	fmt->fmt.pix.width = width;
	fmt->fmt.pix.height = height;
	fmt->fmt.pix.sizeimage = probe_ctrl.dwMaxVideoFrameSize;
	err = ENOERR;
	goto out;

err_out:
	sc->vs_cur_format = prv_fmt;
	prv_fmt->cur_frame_index = prv_frame_index;
out:
	sc->sc_s_fmt_msg[0].uvc_error = err;
	usb_proc_explore_lock(sc->uvc_dev);
	(void)LOS_EventWrite(&sc->uvc_event, SFMT_EVENT);
}

static int
uvc_set_format(struct uvc_softc *sc, struct v4l2_format *fmt)
{
	if ((fmt == NULL) || (fmt->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)) {
		return (-EINVAL);
	}

	sc->uvc_fmt = fmt;

	(void)usb_proc_explore_msignal(sc->uvc_dev, &sc->sc_s_fmt_msg[0], &sc->sc_s_fmt_msg[1]);
	(void)LOS_EventRead(&sc->uvc_event, SFMT_EVENT,
	    LOS_WAITMODE_OR | LOS_WAITMODE_CLR, LOS_WAIT_FOREVER);

	return (sc->sc_s_fmt_msg[0].uvc_error);
}

static int
uvc_try_format(struct uvc_softc *sc, struct v4l2_format *fmt)
{
	struct uvc_format_group *format = NULL;
	uint32_t frame_index = 0;

	if ((fmt == NULL) || (fmt->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)) {
		return (-EINVAL);
	}

	uvc_match_format_and_frame(sc, &format, &frame_index, fmt);
	if (format == NULL) {
		return (-EINVAL);
	}

	fmt->fmt.pix.width = format->frame_array[frame_index]->wWidth;
	fmt->fmt.pix.height = format->frame_array[frame_index]->wHeight;
	fmt->fmt.pix.sizeimage = sc->uvc_ctrl.dwMaxVideoFrameSize;

	return (ENOERR);
}

static int
uvc_get_format(struct uvc_softc *sc, struct v4l2_format *fmt)
{
	struct uvc_incomplete_frame_descriptor0 *frame;
	struct uvc_format_group *format;

	if ((fmt == NULL) || (fmt->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)) {
		return (-EINVAL);
	}

	format = sc->vs_cur_format;
	frame = format->frame_array[format->cur_frame_index];

	fmt->fmt.pix.pixelformat = format->format_type;
	fmt->fmt.pix.width = frame->wWidth;
	fmt->fmt.pix.height = frame->wHeight;
	fmt->fmt.pix.sizeimage = sc->uvc_ctrl.dwMaxVideoFrameSize;

	return (ENOERR);
}

static uint32_t
uvc_match_frame_ivals(struct uvc_incomplete_frame_descriptor0 *frame, uint32_t frame_ratre)
{
	int ivals_index = 0;
	uint32_t frame_ival = frame->dwDefaultFrameInterval;
	uint32_t frame_ivals_size;
	uint32_t temp;
	uint32_t min;
	uint32_t max;
	uint32_t step;
	uint32_t diff;
	uint32_t current;
	uint32_t i;

	if (frame_ratre == 0) {
		return frame_ival;
	}

	frame_ival = FRAME_INTERVAL_DENOMINATOR / frame_ratre;
	frame_ivals_size = (frame->bLength - sizeof(struct uvc_incomplete_frame_descriptor0)) / sizeof(uint32_t);
	if (frame_ivals_size == 0) {
		return frame_ival;
	}

	/*
	 * Refer to USB_Video_Payload_Frame_Based_1.1 section 3.1.2.
	 * bFrameIntervalType == 0: continuous, 3: continuous frame interival array size.
	 * bFrameIntervalType > 0: discrete, continuous frame interival array size == bFrameIntervalType.
	 */
	if ((frame->bFrameIntervalType == 0) && (frame_ivals_size == 3)) {
		min = frame->dwFrameInterval[ivals_index++];
		max = frame->dwFrameInterval[ivals_index++];
		step = frame->dwFrameInterval[ivals_index++];
		if (frame_ival <= min) {
			frame_ival = min;
		} else if (frame_ival >= max) {
			frame_ival = max;
		} else {
			/* Get the closest frame interival by step. 2: the half of the step. */
			temp = min;
			while (temp + step / 2 < frame_ival) {
				temp += step;
			}
			frame_ival = temp;
		}
	} else if (frame->bFrameIntervalType > 0) {
		min = UINT_MAX;
		current = frame_ival;
		/* Get the frame interival frame array. If not found the same, get the closest one. */
		for (i = 0; i < frame_ivals_size; i++) {
			temp = frame->dwFrameInterval[i];
			diff = (uint32_t)abs((int)(temp - frame_ival));
			if (diff < min) {
				min = diff;
				current = temp;
			}
			if (diff == 0) {
				break;
			}
		}
		frame_ival = current;
	} else {
		frame_ival = frame->dwDefaultFrameInterval;
	}

	return frame_ival;
}

static void
uvc_set_parm_process(struct usb_proc_msg *pm)
{
	errno_t err;
	usb_error_t ret;
	uint32_t frame_rate;
	uint32_t frame_ival;
	uint32_t frame_index;
	struct uvc_softc *sc = ((struct uvc_configure_msg *)pm)->sc;
	struct v4l2_streamparm *parm = sc->uvc_parm;
	struct uvc_format_group *format = sc->vs_cur_format;
	struct uvc_incomplete_frame_descriptor0 *frame;
	struct uvc_probe_commit_control1_1 probe_ctrl;

	usb_proc_explore_unlock(sc->uvc_dev);
	if ((parm->parm.capture.timeperframe.numerator == 0) ||
	    (parm->parm.capture.timeperframe.denominator == 0)) {
		frame_rate = 0;
	} else {
		frame_rate = parm->parm.capture.timeperframe.denominator / parm->parm.capture.timeperframe.numerator;
	}

	frame_index = format->cur_frame_index;
	frame = format->frame_array[frame_index];

	frame_ival = uvc_match_frame_ivals(frame, frame_rate);

	ret = uvc_vs_negotiation(sc, &probe_ctrl, frame_ival, 0);
	if (ret != USB_ERR_NORMAL_COMPLETION) {
		err = -ENXIO;
		goto out;
	}

	err = memcpy_s(&sc->uvc_ctrl, sizeof(struct uvc_probe_commit_control1_1), &probe_ctrl,
	    sizeof(struct uvc_probe_commit_control1_1));
	if (err != EOK) {
		err = -EFAULT;
		goto out;
	}

	err = ENOERR;

out:
	sc->sc_s_parm_msg[0].uvc_error = err;
	usb_proc_explore_lock(sc->uvc_dev);
	(void)LOS_EventWrite(&sc->uvc_event, SPARM_EVENT);
}

static int
uvc_set_parm(struct uvc_softc *sc, struct v4l2_streamparm *parm)
{
	if ((parm == NULL) || (parm->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)) {
		return (-EINVAL);
	}

	sc->uvc_parm = parm;

	(void)usb_proc_explore_msignal(sc->uvc_dev, &sc->sc_s_parm_msg[0], &sc->sc_s_parm_msg[1]);
	(void)LOS_EventRead(&sc->uvc_event, SPARM_EVENT,
	    LOS_WAITMODE_OR | LOS_WAITMODE_CLR, LOS_WAIT_FOREVER);

	return (sc->sc_s_parm_msg[0].uvc_error);
}

static int
uvc_get_parm(struct uvc_softc *sc, struct v4l2_streamparm *parm)
{
	if ((parm == NULL) || (parm->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)) {
		return (-EINVAL);
	}

	parm->parm.capture.capability = V4L2_CAP_TIMEPERFRAME;
	parm->parm.capture.capturemode = 0;
	parm->parm.capture.timeperframe.numerator = sc->uvc_ctrl.dwFrameInterval;
	parm->parm.capture.timeperframe.denominator = FRAME_INTERVAL_DENOMINATOR;

	return (ENOERR);
}

static int
uvc_reqbufs(struct uvc_softc *sc, struct v4l2_requestbuffers *req_buf)
{
	struct frame_buf_info *frame_data;
	uint32_t frame_size = sc->uvc_ctrl.dwMaxVideoFrameSize;
	uint32_t i;

	if ((req_buf == NULL) || (req_buf->count == 0) || (req_buf->memory != V4L2_MEMORY_MMAP)) {
		return (-EINVAL);
	}

	if ((sc->uvc_frame_cnt > 0) || (sc->uvc_buf_pool != NULL) ||
	    (frame_size == 0) || (frame_size >= UINT32_MAX / UVC_FRAME_MAX_COUNT)) {
		return (-EINVAL);
	}

	sc->uvc_frame_cnt = MIN(req_buf->count, UVC_FRAME_MAX_COUNT);
	sc->uvc_pool_size = LOS_Align(sc->uvc_frame_cnt * frame_size, USB_PAGE_SIZE);
	sc->uvc_buf_pool = malloc(sc->uvc_pool_size);
	if (sc->uvc_buf_pool == NULL) {
		sc->uvc_frame_cnt = 0;
		return (-ENOMEM);
	}
	req_buf->count = sc->uvc_frame_cnt;
	sc->frame_flag = 0;

	for (i = 0; i < sc->uvc_frame_cnt; i++) {
		frame_data = &sc->uvc_frame_data[i];
		(void)memset_s(&frame_data->buf_info, sizeof(struct v4l2_buffer),
		    0, sizeof(struct v4l2_buffer));
		frame_data->frame_buf = sc->uvc_buf_pool + i * frame_size;
		frame_data->buf_info.index = i;
		frame_data->buf_info.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		frame_data->buf_info.flags = V4L2_BUF_FLAG_MAPPED;
		frame_data->buf_info.field = V4L2_FIELD_NONE;
		frame_data->buf_info.sequence = 0;
		frame_data->buf_info.memory = V4L2_MEMORY_MMAP;
		frame_data->buf_info.m.offset = i * frame_size;
		frame_data->buf_info.length = frame_size;
	}

	return (ENOERR);
}

static int
uvc_querybuf(struct uvc_softc *sc, struct v4l2_buffer *v4l2_buf)
{
	if ((v4l2_buf == NULL) || (v4l2_buf->type != V4L2_BUF_TYPE_VIDEO_CAPTURE) ||
	    (v4l2_buf->memory != V4L2_MEMORY_MMAP) ||
	    (v4l2_buf->index >= sc->uvc_frame_cnt)) {
		return (-EINVAL);
	}

	mtx_lock(&sc->uvc_frame_mux);
	(void)memcpy_s(v4l2_buf, sizeof(struct v4l2_buffer),
	    &sc->uvc_frame_data[v4l2_buf->index].buf_info, sizeof(struct v4l2_buffer));
	mtx_unlock(&sc->uvc_frame_mux);

	return (ENOERR);
}

uint8_t *
uvc_get_frame_buf(struct file *filep, uint32_t index, uint32_t offset)
{
	struct uvc_softc *sc;
	uint8_t *buf = NULL;

	if ((filep == NULL) || (filep->f_inode == NULL) ||
	    (filep->f_inode->i_private == NULL)) {
		return (NULL);
	}

	if (usb_mutex_lock(&protection_softc.uvc_mux) != 0) {
		return (NULL);
	}

	if (protection_softc.uvc_attached == 0) {
		goto out;
	}

	sc = (struct uvc_softc *)filep->f_inode->i_private;
	if ((index >= sc->uvc_frame_cnt) ||
	    (sc->uvc_frame_data[index].buf_info.m.offset != offset)) {
		goto out;
	}

	buf = sc->uvc_frame_data[index].frame_buf;

out:
	(void)usb_mutex_unlock(&protection_softc.uvc_mux);
	return (buf);
}

static int
uvc_qbuf(struct uvc_softc *sc, struct v4l2_buffer *v4l2_buf)
{
	struct frame_buf_info *frame_data;

	if ((v4l2_buf == NULL) || (v4l2_buf->type != V4L2_BUF_TYPE_VIDEO_CAPTURE) ||
	    (v4l2_buf->memory != V4L2_MEMORY_MMAP) ||
	    (v4l2_buf->index >= sc->uvc_frame_cnt)) {
		return (-EINVAL);
	}

	mtx_lock(&sc->uvc_frame_mux);
	frame_data = &sc->uvc_frame_data[v4l2_buf->index];
	if (frame_data->buf_info.flags != V4L2_BUF_FLAG_MAPPED) {
		mtx_unlock(&sc->uvc_frame_mux);
		return (-EBUSY);
	}

	frame_data->buf_info.bytesused = 0;
	frame_data->buf_info.flags &= ~V4L2_BUF_FLAG_MAPPED;
	frame_data->buf_info.flags |= V4L2_BUF_FLAG_QUEUED;
	LIST_ADD_TAIL(&frame_data->frame_queue, &sc->in_queue);
	mtx_unlock(&sc->uvc_frame_mux);

	return (ENOERR);
}

static int
uvc_dqbuf(struct uvc_softc *sc, struct v4l2_buffer *v4l2_buf)
{
	struct frame_buf_info *frame_data;
	uint32_t events;

	if ((v4l2_buf == NULL) || (v4l2_buf->type != V4L2_BUF_TYPE_VIDEO_CAPTURE) ||
	    (v4l2_buf->memory != V4L2_MEMORY_MMAP)) {
		return (-EINVAL);
	}

	mtx_lock(&sc->uvc_frame_mux);
	while (LOS_ListEmpty(&sc->out_queue)) {
		mtx_unlock(&sc->uvc_frame_mux);
		events = LOS_EventRead(&sc->frame_event, UVC_FRAME_BUF_EVENT | UVC_DETACH_EVENT,
		    LOS_WAITMODE_OR | LOS_WAITMODE_CLR, LOS_MS2Tick(UVC_FRAME_TIMEOUT_MS));
		if (events != UVC_FRAME_BUF_EVENT) {
			return (-ENXIO);
		}
		mtx_lock(&sc->uvc_frame_mux);
	}
	frame_data = LOS_LIST_FIRST_ENTRY(&sc->out_queue, struct frame_buf_info, frame_queue);
	LOS_ListDelInit(&frame_data->frame_queue);
	(void)memcpy_s(v4l2_buf, sizeof(struct v4l2_buffer),
	    &frame_data->buf_info, sizeof(struct v4l2_buffer));

	frame_data->buf_info.flags &= ~V4L2_BUF_FLAG_DONE;
	frame_data->buf_info.flags |= V4L2_BUF_FLAG_MAPPED;
	mtx_unlock(&sc->uvc_frame_mux);

	return (ENOERR);
}

static void
uvc_update_interface_index(struct uvc_softc *sc, uint8_t *iface_index, uint8_t iface_len)
{
	struct usb_interface_assoc_descriptor *iface_assoc;
	struct usb_config_descriptor *config_desc;
	struct usb_descriptor *usb_desc = NULL;

	if (iface_len <= 1) {
		return;
	}

	config_desc = usbd_get_config_descriptor(sc->uvc_dev);
	for (; ;) {
		usb_desc = usb_desc_foreach(config_desc, usb_desc);
		if (usb_desc == NULL) {
			break;
		}
		if ((usb_desc->bDescriptorType != UDESC_IFACE_ASSOC) ||
		    (usb_desc->bLength != sizeof(struct usb_interface_assoc_descriptor)) ||
		    (((struct usb_interface_assoc_descriptor *)usb_desc)->bFunctionClass != UICLASS_VIDEO)) {
			continue;
		}

		iface_assoc = (struct usb_interface_assoc_descriptor *)usb_desc;
		iface_index[0] = iface_assoc->bFirstInterface;
		if (iface_assoc->bInterfaceCount > 1) {
			iface_index[1] = iface_assoc->bFirstInterface + 1;
		}
		return;
	}
}

static void
uvc_streamon_process(struct usb_proc_msg *pm)
{
	struct uvc_softc *sc = ((struct uvc_configure_msg *)pm)->sc;
	int i;
	uint32_t num;
	uint8_t iface_index[] = { 0, 1 };
	usb_error_t ret;
	errno_t error = ENOERR;

	usb_proc_explore_unlock(sc->uvc_dev);

	(void)uvc_find_and_save_endpoint(sc, sc->vs_cur_iface, sc->vs_cur_iface->cur_iface_index, 0);
	ret = usbd_set_alt_interface_index(sc->uvc_dev,
	    sc->vs_cur_iface->cur_iface_index, sc->vs_cur_iface->cur_alt_index);
	if (ret != USB_ERR_NORMAL_COMPLETION) {
		error = -ENXIO;
		goto out;
	}
	num = VALUE_ROUND_UP(sc->uvc_ctrl.dwMaxVideoFrameSize, sc->vs_cur_iface->max_pack_size);
	sc->uvc_pack_cnt = MIN(num, UVC_PACK_NUM);

	uvc_update_interface_index(sc, iface_index, sizeof(iface_index));
	ret = usbd_transfer_setup(sc->uvc_dev,
	    iface_index, sc->sc_xfer, uvc_config, UVC_TRANSFER_NUM, sc, &sc->uvc_frame_mux);
	if (ret != USB_ERR_NORMAL_COMPLETION) {
		error = -ENXIO;
		goto error_setup;
	}

	ret = uvc_isoc_xfer_init(sc);
	if (ret != USB_ERR_NORMAL_COMPLETION) {
		error = -ENOMEM;
		goto error_init;
	}

	for (i = UVC_XFER1; i <= UVC_XFER3; i++) {
		usbd_transfer_start(sc->sc_xfer[i]);
	}
	goto out;

error_init:
	uvc_isoc_xfer_deinit(sc);
	usbd_transfer_unsetup(sc->sc_xfer, UVC_TRANSFER_NUM);
error_setup:
	(void)usbd_set_alt_interface_index(sc->uvc_dev,
	    sc->vs_cur_iface->cur_iface_index, 0);
out:
	sc->sc_streamon_msg[0].uvc_error = error;
	usb_proc_explore_lock(sc->uvc_dev);
	(void)LOS_EventWrite(&sc->uvc_event, STREAMON_EVENT);
}

static int
uvc_streamon(struct uvc_softc *sc, uint32_t *type)
{
	if ((type == NULL) || (*type != V4L2_BUF_TYPE_VIDEO_CAPTURE) ||
	    (sc->uvc_frame_cnt == 0) || LOS_ListEmpty(&sc->in_queue)) {
		return (-EINVAL);
	}

	if (sc->uvc_dev->speed != USB_SPEED_HIGH) {
		return (-ENOTSUP);
	}

	if (sc->stream_flag != 0) {
		return (-EBUSY);
	}

	(void)usb_proc_explore_msignal(sc->uvc_dev, &sc->sc_streamon_msg[0], &sc->sc_streamon_msg[1]);
	(void)LOS_EventRead(&sc->uvc_event, STREAMON_EVENT,
	    LOS_WAITMODE_OR | LOS_WAITMODE_CLR, LOS_WAIT_FOREVER);
	if (sc->sc_streamon_msg[0].uvc_error == ENOERR) {
		sc->stream_flag = 1;
	}

	return (sc->sc_streamon_msg[0].uvc_error);
}

static void
uvc_streamoff_process(struct usb_proc_msg *pm)
{
	struct uvc_softc *sc = ((struct uvc_configure_msg *)pm)->sc;

	usb_proc_explore_unlock(sc->uvc_dev);

	uvc_transfer_close(sc);
	sc->sc_streamoff_msg[0].uvc_error = ENOERR;

	usb_proc_explore_lock(sc->uvc_dev);
	(void)LOS_EventWrite(&sc->uvc_event, STREAMOFF_EVENT);
}

static int
uvc_streamoff(struct uvc_softc *sc, uint32_t *type)
{
	if ((type == NULL) || (*type != V4L2_BUF_TYPE_VIDEO_CAPTURE) || (sc->uvc_frame_cnt == 0)) {
		return (-EINVAL);
	}

	(void)usb_proc_explore_msignal(sc->uvc_dev, &sc->sc_streamoff_msg[0], &sc->sc_streamoff_msg[1]);
	(void)LOS_EventRead(&sc->uvc_event, STREAMOFF_EVENT,
	    LOS_WAITMODE_OR | LOS_WAITMODE_CLR, LOS_WAIT_FOREVER);

	return (sc->sc_streamoff_msg[0].uvc_error);
}

static usb_error_t
uvc_frame_data_processing_completed(struct uvc_softc *sc, struct frame_buf_info *frame_data)
{
	/* The bytesused and length of uncompressed formats are equal */
	if ((sc->vs_cur_format->format_type != V4L2_PIX_FMT_MJPEG) &&
	    (frame_data->buf_info.bytesused != frame_data->buf_info.length)) {
		frame_data->buf_info.bytesused = 0;
		return (USB_ERR_BAD_BUFSIZE);
	}

	mtx_lock(&sc->uvc_frame_mux);
	(void)gettimeofday(&frame_data->buf_info.timestamp, NULL);
	LOS_ListDelInit(&frame_data->frame_queue);
	frame_data->buf_info.flags &= ~V4L2_BUF_FLAG_QUEUED;
	frame_data->buf_info.flags |= V4L2_BUF_FLAG_DONE;
	LIST_ADD_TAIL(&frame_data->frame_queue, &sc->out_queue);
	mtx_unlock(&sc->uvc_frame_mux);

	(void)LOS_EventWrite(&sc->frame_event, UVC_FRAME_BUF_EVENT);
	return (USB_ERR_NORMAL_COMPLETION);
}

static usb_error_t
uvc_decode_and_assembly_pack(struct uvc_softc *sc, uint8_t *pack_buf, uint32_t pack_size)
{
	struct frame_buf_info *frame_data;
	struct uvc_payload_header *uvc_header;
	uint32_t len;
	errno_t err;
	usb_error_t ret;

	if (pack_size < sizeof(struct uvc_payload_header)) {
		return (USB_ERR_IOERROR);
	}
	uvc_header = (struct uvc_payload_header *)pack_buf;
	if ((uvc_header->bHeaderLength > UVC_HEADER_MAX_LEN) ||
	    (uvc_header->bHeaderLength < UVC_HEADER_MIN_LEN) ||
	    ((uvc_header->bHeaderLength == pack_size) &&
	    !(uvc_header->bmHeaderInfo & UVC_STREAM_HEADER_EOF))) {
		return (USB_ERR_IOERROR);
	}
	if (uvc_header->bmHeaderInfo & UVC_STREAM_HEADER_ERR) {
		return (USB_ERR_CANCELLED);
	}

	if (sc->frame_flag == 0) {
		sc->frame_flag = 1;
		sc->uvc_fid = uvc_header->bmHeaderInfo & UVC_STREAM_HEADER_FID;
	}

	if (LOS_ListEmpty(&sc->in_queue)) {
		return (USB_ERR_NOMEM);
	}
	frame_data = LOS_LIST_FIRST_ENTRY(&sc->in_queue, struct frame_buf_info, frame_queue);
	if ((sc->uvc_fid != (uvc_header->bmHeaderInfo & UVC_STREAM_HEADER_FID)) &&
	    (frame_data->buf_info.bytesused != 0)) {
		sc->uvc_fid = uvc_header->bmHeaderInfo & UVC_STREAM_HEADER_FID;
		ret = uvc_frame_data_processing_completed(sc, frame_data);
		if (ret == USB_ERR_NORMAL_COMPLETION) {
			if (LOS_ListEmpty(&sc->in_queue)) {
				return (USB_ERR_NOMEM);
			}
			frame_data = LOS_LIST_FIRST_ENTRY(&sc->in_queue, struct frame_buf_info, frame_queue);
		}
	}

	len = pack_size - uvc_header->bHeaderLength;
	if (((frame_data->buf_info.bytesused + len) <= frame_data->buf_info.length) && (len != 0)) {
		err = memcpy_s(frame_data->frame_buf + frame_data->buf_info.bytesused,
		    frame_data->buf_info.length - frame_data->buf_info.bytesused,
		    pack_buf + uvc_header->bHeaderLength, len);
		if (err != EOK) {
			usb_err("memcpy failed! err:%d\n", err);
			return (USB_ERR_INVAL);
		}
		frame_data->buf_info.bytesused += len;
	}

	if (uvc_header->bmHeaderInfo & UVC_STREAM_HEADER_EOF) {
		sc->frame_flag = 0;
		ret = uvc_frame_data_processing_completed(sc, frame_data);
		return (ret);
	}

	return (USB_ERR_NORMAL_COMPLETION);
}

static void
uvc_transfer_setup(struct uvc_softc *sc, struct usb_xfer *xfer, struct uvc_isoc_xfer *isoc_xfer)
{
	uint32_t i;
	uint8_t *pack_buf;

	for (i = 0; i < sc->uvc_pack_cnt; i++) {
		pack_buf = isoc_xfer->uvc_buf + i * isoc_xfer->buf_size;
		usbd_xfer_set_frame_data(xfer, i, pack_buf, isoc_xfer->buf_size);
	}
	xfer->nframes = sc->uvc_pack_cnt;
	usbd_transfer_submit(xfer);
}

static void
uvc_read_callback(struct usb_xfer *xfer, usb_error_t error)
{
	struct uvc_softc *sc = (struct uvc_softc *)usbd_xfer_softc(xfer);
	struct uvc_isoc_xfer *isoc_xfer;
	uint8_t *pack_buf;
	uint32_t pack_size;
	uint32_t i;
	int actlen;
	usb_error_t ret;

	usbd_xfer_status(xfer, &actlen, NULL, NULL, NULL);
	for (i = 0; i < UVC_IXFER_NUM; i++) {
		isoc_xfer = &sc->vs_cur_iface->isoc_xfer[i];
		if (isoc_xfer->xfer == xfer) {
			break;
		}
	}
	if (i == UVC_IXFER_NUM) {
		return;
	}

	switch (USB_GET_STATE(xfer)) {
	case USB_ST_TRANSFERRED:
		if (actlen == 0) {
			uvc_transfer_setup(sc, xfer, isoc_xfer);
			break;
		}

		for (i = 0; i < sc->uvc_pack_cnt; i++) {
			pack_size = xfer->frlengths[i];
			if (pack_size == 0) {
				continue;
			}
			pack_buf = isoc_xfer->uvc_buf + i * isoc_xfer->buf_size;
			ret = uvc_decode_and_assembly_pack(sc, pack_buf, pack_size);
			if (ret == USB_ERR_CANCELLED) {
				break;
			}
		}
		/* fall-through */

	case USB_ST_SETUP:
		uvc_transfer_setup(sc, xfer, isoc_xfer);
		break;

	default:
		if (error != USB_ERR_CANCELLED) {
			usbd_xfer_set_stall(xfer);
			uvc_transfer_setup(sc, xfer, isoc_xfer);
		}
		break;
	}
}

static void
uvc_intr_callback(struct usb_xfer *xfer, usb_error_t error)
{
	int actlen;

	usbd_xfer_status(xfer, &actlen, NULL, NULL, NULL);
	switch (USB_GET_STATE(xfer)) {
	case USB_ST_TRANSFERRED:
	case USB_ST_SETUP:
		usbd_xfer_set_frame_len(xfer, 0, usbd_xfer_max_len(xfer));
		usbd_transfer_submit(xfer);
		break;

	default:
		if (error != USB_ERR_CANCELLED) {
			usbd_xfer_set_stall(xfer);
			usbd_xfer_set_frame_len(xfer, 0, usbd_xfer_max_len(xfer));
			usbd_transfer_submit(xfer);
		}
		break;
	}
}

int
video_ioctl(struct file *filep, int cmd, unsigned long data)
{
	struct uvc_softc *sc = (struct uvc_softc *)filep->f_inode->i_private;
	int ret;

	if (sc == NULL) {
		return (-EPERM);
	}

	(void)usb_mutex_lock(&protection_softc.uvc_mux);
	if ((protection_softc.uvc_attached == 0) || (filep->f_inode->i_flags & FSNODEFLAG_DELETED)) {
		(void)usb_mutex_unlock(&protection_softc.uvc_mux);
		return (-ENODEV);
	}

	switch (cmd) {
	case VIDIOC_QUERYCAP:
		ret = uvc_querycap(sc, (struct v4l2_capability *)data);
		break;
	case VIDIOC_ENUM_FMT:
		ret = uvc_enum_fmtdesc(sc, (struct v4l2_fmtdesc *)data);
		break;
	case VIDIOC_ENUM_FRAMESIZES:
		ret = uvc_enum_framesize(sc, (struct v4l2_frmsizeenum *)data);
		break;
	case VIDIOC_ENUM_FRAMEINTERVALS:
		ret = uvc_enum_fivals(sc, (struct v4l2_frmivalenum *)data);
		break;
	case VIDIOC_S_FMT:
		ret = uvc_set_format(sc, (struct v4l2_format *)data);
		break;
	case VIDIOC_G_FMT:
		ret = uvc_get_format(sc, (struct v4l2_format *)data);
		break;
	case VIDIOC_TRY_FMT:
		ret = uvc_try_format(sc, (struct v4l2_format *)data);
		break;
	case VIDIOC_S_PARM:
		ret = uvc_set_parm(sc, (struct v4l2_streamparm *)data);
		break;
	case VIDIOC_G_PARM:
		ret = uvc_get_parm(sc, (struct v4l2_streamparm *)data);
		break;
	case VIDIOC_REQBUFS:
		ret = uvc_reqbufs(sc, (struct v4l2_requestbuffers *)data);
		break;
	case VIDIOC_QUERYBUF:
		ret = uvc_querybuf(sc, (struct v4l2_buffer *)data);
		break;
	case VIDIOC_QBUF:
		ret = uvc_qbuf(sc, (struct v4l2_buffer *)data);
		break;
	case VIDIOC_DQBUF:
		ret = uvc_dqbuf(sc, (struct v4l2_buffer *)data);
		break;
	case VIDIOC_STREAMON:
		ret = uvc_streamon(sc, (uint32_t *)data);
		break;
	case VIDIOC_STREAMOFF:
		ret = uvc_streamoff(sc, (uint32_t *)data);
		break;
	default:
		ret = ENOTTY;
	}

	(void)usb_mutex_unlock(&protection_softc.uvc_mux);

	return (ret);
}

int
video_poll(struct file *filep, poll_table *fds)
{
	struct uvc_softc *sc = (struct uvc_softc *)filep->f_inode->i_private;
	uint32_t value = fds->key & (POLLIN | POLLRDNORM);
	uint32_t revents = 0;

	if (sc == NULL) {
		return (-EPERM);
	}

	(void)usb_mutex_lock(&protection_softc.uvc_mux);
	if ((protection_softc.uvc_attached == 0) || (filep->f_inode->i_flags & FSNODEFLAG_DELETED)) {
		(void)usb_mutex_unlock(&protection_softc.uvc_mux);
		return (-ENODEV);
	}

	if (value) {
		if (!LOS_ListEmpty(&sc->out_queue)) {
			revents |= value;
		}
	}

	(void)usb_mutex_unlock(&protection_softc.uvc_mux);

	return ((int)revents);
}
