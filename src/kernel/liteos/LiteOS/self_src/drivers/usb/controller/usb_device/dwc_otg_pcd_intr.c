/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2021. All rights reserved.
 * Description : LiteOS USB device pcd intr implementation.
 * Author : Huawei LiteOS Team
 * Create : 2013-5-20
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

#include <nuttx/usb/usb.h>
#include "kal_tick.h"
#include "controller/usb_device/dwc_otgreg.h"
#include "controller/usb_device/dwc_otg_pcd.h"
#include "implementation/usb_dma_cache.h"
#include "phy/usb_phy.h"
#include "controller/usb_device/dwc_otg_pcd_intr.h"

#define EPINTR_UNMSK	0
#define EPINTR_MSK		1
#define USB_STATUS_LEN	2

#define GET_OEP_INTR(epnum, needmsk) ((needmsk) ? \
				(DWC_READ_REG32(DOTG_DOEPINT(epnum)) & DWC_READ_REG32(DOTG_DOEPMSK)) : \
				(DWC_READ_REG32(DOTG_DOEPINT(epnum))))

#define GET_IEP_INTR(epnum, needmsk) ((needmsk) ? \
				(DWC_READ_REG32(DOTG_DIEPINT(epnum)) & DWC_READ_REG32(DOTG_DIEPMSK)) : \
				(DWC_READ_REG32(DOTG_DIEPINT(epnum))))

#define CLEAR_OEP_INTR(epnum, bit)   do { \
			doepint_data_t doepint_t = {0}; doepint_t.b.bit = 1; \
			DWC_WRITE_REG32(DOTG_DOEPINT(epnum), doepint_t.d32); \
		} while (0)

#define CLEAR_IEP_INTR(epnum, bit)   do { \
			diepint_data_t diepint_t = {0}; diepint_t.b.bit = 1; \
			DWC_WRITE_REG32(DOTG_DIEPINT(epnum), diepint_t.d32); \
		} while (0)

static int32_t handle_endpoint0_transfer_complete(dwc_otg_pcd_t *pcd);

static inline struct usbdev_ep_s *
usbd_get_in_ep(dwc_otg_pcd_t *pcd, uint8_t num)
{
	if (num == 0) {
		return (&pcd->usbd_ep0);
	}

	for (uint8_t i = 0; i < pcd->num_in_eps; i++) {
		if (pcd->pcd_in_eps[i].num == num) {
			return (&pcd->usbd_in_eps[i]);
		}
	}

	return (NULL);
}

static inline struct usbdev_ep_s *
usbd_get_out_ep(dwc_otg_pcd_t *pcd, uint8_t num)
{
	if (num == 0) {
		return (&pcd->usbd_ep0);
	}

	for (uint8_t i = 0; i < pcd->num_out_eps; i++) {
		if (pcd->pcd_out_eps[i].num == num) {
			return (&pcd->usbd_out_eps[i]);
		}
	}

	return (NULL);
}

static void
endpoint0_in_startup_status_phase(dwc_otg_pcd_t *pcd)
{
	dwc_otg_pcd_ep_t *ep0 = &pcd->pcd_ep0;
	if (pcd->ep0state == EP0_STALL) {
		return;
	}

	pcd->ep0state = EP0_IN_STATUS_PHASE;
	ep0->xfer_len = 0;
	ep0->xfer_count = 0;
	ep0->is_in = 1;

	dwc_otg_ep_start_transfer(ep0);
}

static void
endpoint0_out_startup_status_phase(dwc_otg_pcd_t *pcd)
{
	dwc_otg_pcd_ep_t *ep0 = &pcd->pcd_ep0;
	doepint_data_t doepint;
	doepint.d32 = DWC_READ_REG32(DOTG_DOEPINT(0));
	if (pcd->ep0state == EP0_STALL) {
		return;
	}
	pcd->ep0state = EP0_OUT_STATUS_PHASE;

	/* Prepare for more SETUP Packets */
	ep0->xfer_len = 0;
	ep0->xfer_count = 0;
	ep0->is_in = 0;

	/* If there is xfercomplete on EP0 OUT do not start OUT Status stage.
	* xfercomplete means that ZLP was already received as EP0 OUT is enabled
	* during IN Data stage
	*/
	if (doepint.b.xfercompl_r == 1) {
		return;
	}

	dwc_otg_ep_start_transfer(ep0);
}

static inline void
endpoint0_in_startup_stall(dwc_otg_pcd_t *pcd)
{
	dwc_otg_pcd_ep_t *ep0 = &pcd->pcd_ep0;

	ep0->is_in = 1;
	dwc_otg_ep_set_stall(pcd, ep0);
	ep0->is_stopped = 1;
	pcd->ep0state = EP0_IDLE;
	endpoint0_startup_receiving_setup(pcd);
}

static inline void
endpoint_clear_halt(dwc_otg_pcd_t *pcd, dwc_otg_pcd_ep_t *ep)
{
#if defined(LOSCFG_DRIVERS_USB_UVC_GADGET) || defined(LOSCFG_DRIVERS_USB_CAMERA_GADGET)
	if (device_is_uvc() && (pcd->driver->ops->disconnect != NULL)) {
		pcd->driver->ops->disconnect(pcd->driver, &pcd->usb_dev);
	}
#endif

	dwc_otg_ep_clear_stall(pcd, ep);

#if defined(LOSCFG_DRIVERS_USB_UVC_GADGET) || defined(LOSCFG_DRIVERS_USB_CAMERA_GADGET)
	if (device_is_uvc()) {
		dwc_otg_endpoint_deactivate(ep);
	}
#endif

	/* Reactive the EP */
	dwc_otg_endpoint_activate(ep);
	if (ep->is_stopped) {
		ep->is_stopped = 0;
	}

#if defined(LOSCFG_DRIVERS_USB_UVC_GADGET) || defined(LOSCFG_DRIVERS_USB_CAMERA_GADGET)
	if (device_is_uvc()) {
		dwc_otg_flush_tx_fifo(ep->fifo_index);
	}
#endif

	/* Start Control Status Phase */
	endpoint0_in_startup_status_phase(pcd);
}

static inline void
endpoint0_set_feature(dwc_otg_pcd_t *pcd, struct usb_ctrlreq_s ctrl)
{
	switch (ctrl.type & USB_REQ_RECIPIENT_MASK) {
	case USB_REQ_RECIPIENT_DEVICE:
		switch (USB_GETW(ctrl.value)) {
			case USB_FEATURE_REMOTEWAKEUP:
				pcd->remote_wakeup_enable = 1;
				break;

			case USB_FEATURE_TESTMODE:
				break;

			default:
				endpoint0_in_startup_stall(pcd);
				return;

		}
		endpoint0_in_startup_status_phase(pcd);
		break;

	default:
		break;
	}
}


static inline void
endpoint0_clear_feature(dwc_otg_pcd_t *pcd, struct usb_ctrlreq_s ctrl)
{
	struct usbdev_ep_s *usbd_ep = NULL;
	uint8_t ep_num;

	switch (ctrl.type & USB_REQ_RECIPIENT_MASK) {
	case USB_REQ_RECIPIENT_DEVICE:
		switch (USB_GETW(ctrl.value)) {
		case USB_FEATURE_REMOTEWAKEUP:
			pcd->remote_wakeup_enable = 0;
			pcd->lx_state = USB_DWC2_STATE_L0;
			break;

		case USB_FEATURE_TESTMODE:
			break;

		default:
			endpoint0_in_startup_stall(pcd);
			return;
		}
		endpoint0_in_startup_status_phase(pcd);
		break;

	case USB_REQ_RECIPIENT_ENDPOINT:
		ep_num = USB_GETW(ctrl.index) & USB_EP_ADDR_NUMBER_MASK;
		if (ep_num >= PCD_MAX_EPS) {
			break;
		}

		if ((USB_GETW(ctrl.index) & USB_EP_ADDR_DIR_MASK) == USB_DIR_IN) {
			usbd_ep = usbd_get_in_ep(pcd, ep_num);
		} else {
			usbd_ep = usbd_get_out_ep(pcd, ep_num);
		}

		if ((usbd_ep != NULL) && (usbd_ep->ep_link != NULL)) {
			endpoint_clear_halt(pcd, (dwc_otg_pcd_ep_t *)usbd_ep->ep_link);
		}
		break;
	default:
		break;
	}
}

static void
handle_setup_phase(dwc_otg_pcd_t *pcd)
{
	dwc_otg_pcd_ep_t *ep0 = &pcd->pcd_ep0;
	struct usb_ctrlreq_s ctrl;
	struct usbdev_ep_s *usbd_ep = &pcd->usbd_ep0;
	int ret;

	/* sync data from mem to cache */
	usb_dma_cache_invalid((void *)pcd->setup_pkt, sizeof(ctrl));
	ret = memcpy_s(&ctrl, sizeof(ctrl), pcd->setup_pkt, sizeof(ctrl));
	if (ret != EOK) {
		return;
	}

	dwc_otg_debug("type:0x%x req:0x%x value:0x%x index:0x%x len:0x%x\n",
	    ctrl.type, ctrl.req, USB_GETW(ctrl.value),
	    USB_GETW(ctrl.index), USB_GETW(ctrl.len));

	pcd->status = 0;
	pcd->request_enable = 0;

	if (ctrl.type & USB_DIR_IN) {
		ep0->is_in = 1;
		pcd->ep0state = EP0_IN_DATA_PHASE;
	} else {
		ep0->is_in = 0;
		pcd->ep0state = EP0_OUT_DATA_PHASE;
	}

	if (USB_GETW(ctrl.len) == 0) {
		ep0->is_in = 1;
		pcd->ep0state = EP0_IN_STATUS_PHASE;
	}

	if ((ctrl.type & USB_REQ_TYPE_MASK) != USB_REQ_TYPE_STANDARD) {
		/* handle non-standard (class/vendor) requests in the gadget driver */
		dwc_otg_core_do_pcd_setup(pcd, &ctrl);
		return;
	}

	switch (ctrl.req) {
	case USB_REQ_GETSTATUS:
		switch (ctrl.type & USB_REQ_RECIPIENT_MASK) {
		case USB_REQ_RECIPIENT_DEVICE:
			pcd->status |= pcd->remote_wakeup_enable << 1;
			break;

		case USB_REQ_RECIPIENT_INTERFACE:
			break;

		default:
			break;
		}
		if (usbd_ep->handle_req) {
			usbd_ep->handle_req->len = USB_STATUS_LEN;
			ret = memcpy_s(usbd_ep->handle_req->buf, USB_COMP_EP0_BUFSIZ, &pcd->status, usbd_ep->handle_req->len);
			if (ret != EOK) {
				usb_err("memcpy_s fail, ret:%d\n", ret);
				return;
			}
			(void)dwc_otg_ep_req_start(usbd_ep, usbd_ep->handle_req);
		}
		break;

	case USB_REQ_CLEARFEATURE:
		endpoint0_clear_feature(pcd, ctrl);
		break;

	case USB_REQ_SETFEATURE:
		endpoint0_set_feature(pcd, ctrl);
		break;

	case USB_REQ_SETADDRESS:
		if (ctrl.type == USB_REQ_RECIPIENT_DEVICE) {
			dcfg_data_t dcfg = {0};
			KAL_Printf("\n ****** NEW USB ADDRESS %03d ****** \n", USB_GETW(ctrl.value));
			dcfg.b.devaddr_rw = USB_GETW(ctrl.value);
			DWC_MODIFY_REG32(DOTG_DCFG, 0, dcfg.d32);
			endpoint0_in_startup_status_phase(pcd);
			return;
		}
		break;

	case USB_REQ_SETINTERFACE:
		/* fall-through */
	case USB_REQ_SETCONFIGURATION:
		pcd->request_config = 1;   /* Configuration changed */
		dwc_otg_core_do_pcd_setup(pcd, &ctrl);
		break;

	default:
		/* Call the Gadget Driver's setup functions */
		dwc_otg_core_do_pcd_setup(pcd, &ctrl);
		break;
	}
}

static void
dwc_otg_epn_continue_transfer(dwc_otg_pcd_t *pcd, dwc_otg_pcd_ep_t *ep)
{
	depctl_data_t depctl;
	deptsiz0_data_t deptsiz;

	if (ep->is_in == 1) {
		depctl.d32 = DWC_READ_REG32(DOTG_DIEPCTL(ep->num));
		deptsiz.d32 = DWC_READ_REG32(DOTG_DIEPTSIZ(ep->num));
		deptsiz.b.xfersize_rw = 0;
		deptsiz.b.pktcnt_rw = 1;
		DWC_WRITE_REG32(DOTG_DIEPTSIZ(ep->num), deptsiz.d32);
		DWC_WRITE_REG32(DOTG_DIEPDMA(ep->num), (uint32_t)ep->dma_addr + USB_DMA_OFFSET);

		/* EP enable, IN data in FIFO */
		depctl.b.cnak_w = 1;
		depctl.b.epena_r_ws_sc = 1;
		DWC_WRITE_REG32(DOTG_DIEPCTL(ep->num), depctl.d32);
	}
}

static uint8_t
endpoint_arg_set(dwc_otg_pcd_t *pcd, dwc_otg_pcd_ep_t *ep, uint32_t bcount)
{
	uint8_t is_last = 0;

	ep->xfer_buff += bcount;
	ep->dma_addr += bcount;
	ep->xfer_count += bcount;

	if (ep->xfer_len < ep->total_len) {
		dwc_otg_ep_start_transfer(ep);
	} else if (ep->sent_zlp) {
		dwc_otg_epn_continue_transfer(pcd, ep);
		ep->sent_zlp = 0;
	} else {
		is_last = 1;
	}
	return (is_last);
}

static void
handle_normal_endpoint(dwc_otg_pcd_t *pcd, dwc_otg_pcd_ep_t *pcd_ep, struct usbdev_ep_s *ep)
{
	deptsiz_data_t deptsiz;
	uint32_t byte_count;
	uint8_t ep_num = pcd_ep->num;
	uint8_t is_last = 0;

	if (pcd_ep->is_in) {
		pcd->xfer_len = pcd_ep->xfer_count;

		deptsiz.d32 = DWC_READ_REG32(DOTG_DIEPTSIZ(ep_num));
		if ((deptsiz.b.xfersize_rw == 0) && (deptsiz.b.pktcnt_rw == 0)) {
			byte_count = pcd_ep->xfer_len - pcd_ep->xfer_count;
			is_last = endpoint_arg_set(pcd, pcd_ep, byte_count);
		}
	} else {
		/* OUT Transfer */
		deptsiz.d32 = DWC_READ_REG32(DOTG_DOEPTSIZ(ep_num));
		byte_count = (pcd_ep->xfer_len - pcd_ep->xfer_count - deptsiz.b.xfersize_rw);

		usb_dma_cache_invalid((void *)pcd_ep->dma_addr, byte_count);
		is_last = endpoint_arg_set(pcd, pcd_ep, byte_count);
	}
	if (is_last) {
		pcd_ep->start_xfer_buff = 0;
		pcd_ep->xfer_buff = 0;
		pcd_ep->xfer_len = 0;
		dwc_otg_core_ep_complete(pcd_ep, ep);
	}
}

static void
dwc_otg_ep0_continue_transfer(dwc_otg_pcd_t *pcd, dwc_otg_pcd_ep_t *ep0)
{
	depctl_data_t depctl;
	deptsiz0_data_t deptsiz;
	if (ep0->is_in == 1) {
		depctl.d32 = DWC_READ_REG32(DOTG_DIEPCTL(0));
		deptsiz.d32 = DWC_READ_REG32(DOTG_DIEPTSIZ(0));
		deptsiz.b.xfersize_rw =
		    (ep0->total_len - ep0->xfer_count) >
		    ep0->maxpacket ? ep0->maxpacket : (ep0->total_len -
		    ep0->xfer_count);
		deptsiz.b.pktcnt_rw = 1;
		ep0->xfer_len = deptsiz.b.xfersize_rw;
		DWC_WRITE_REG32(DOTG_DIEPTSIZ(0), deptsiz.d32);
		DWC_WRITE_REG32(DOTG_DIEPDMA(0), (uint32_t)ep0->dma_addr + USB_DMA_OFFSET);

		/* EP enable, IN data in FIFO */
		depctl.b.cnak_w = 1;
		depctl.b.epena_r_ws_sc = 1;
		DWC_WRITE_REG32(DOTG_DIEPCTL(0), depctl.d32);
	} else {
		depctl.d32 = DWC_READ_REG32(DOTG_DOEPCTL(0));
		deptsiz.d32 = DWC_READ_REG32(DOTG_DOEPTSIZ(0));
		deptsiz.b.xfersize_rw = ep0->maxpacket;
		deptsiz.b.pktcnt_rw = 1;
		DWC_WRITE_REG32(DOTG_DOEPTSIZ(0), deptsiz.d32);
		DWC_WRITE_REG32(DOTG_DOEPDMA(0), (uint32_t)ep0->dma_addr + USB_DMA_OFFSET);

		/* EP enable, IN data in FIFO */
		depctl.b.cnak_w = 1;
		depctl.b.epena_r_ws_sc = 1;
		DWC_WRITE_REG32(DOTG_DOEPCTL(0), depctl.d32);
	}
}

static void
handle_default_endpoint0(dwc_otg_pcd_t *pcd, int is_in)
{
	dwc_otg_pcd_ep_t *ep0 = &pcd->pcd_ep0;
	deptsiz0_data_t deptsiz;
	uint32_t byte_count;

	switch (pcd->ep0state) {
	case EP0_DISCONNECT:
		dwc_otg_debug("EP0 DISCONNECT\n");
		break;

	case EP0_IDLE:
		pcd->request_config = 0;
		dwc_otg_debug("EP0_IDLE\n");
		handle_setup_phase(pcd);
		break;

	case EP0_IN_DATA_PHASE:
		dwc_otg_debug("EP0 IN DATA PHASE\n");
		deptsiz.d32 = DWC_READ_REG32(DOTG_DIEPTSIZ(0));
		byte_count = ep0->xfer_len - deptsiz.b.xfersize_rw;

		ep0->xfer_count += byte_count;
		ep0->xfer_buff += byte_count;
		ep0->dma_addr += byte_count;

		if (ep0->xfer_count < ep0->total_len) {
			dwc_otg_ep0_continue_transfer(pcd, ep0);
		} else if (ep0->sent_zlp) {
			dwc_otg_ep0_continue_transfer(pcd, ep0);
			ep0->sent_zlp = 0;
		} else {
			(void)handle_endpoint0_transfer_complete(pcd);
		}
		break;

	case EP0_OUT_DATA_PHASE:
		dwc_otg_debug("EP0 OUT DATA PHASE\n");
		deptsiz.d32 = DWC_READ_REG32(DOTG_DOEPTSIZ(0));
		byte_count = ep0->maxpacket - deptsiz.b.xfersize_rw;

		ep0->xfer_count += byte_count;
		ep0->xfer_buff += byte_count;
		ep0->dma_addr += byte_count;

		if (ep0->xfer_count < ep0->total_len) {
			dwc_otg_ep0_continue_transfer(pcd, ep0);
		} else if (ep0->sent_zlp) {
			dwc_otg_ep0_continue_transfer(pcd, ep0);
		} else {
			(void)handle_endpoint0_transfer_complete(pcd);
		}
		pcd->cmdtype.in_complete = 1;
		break;

	case EP0_IN_STATUS_PHASE:
		/* fall-through */
	case EP0_OUT_STATUS_PHASE:
		(void)handle_endpoint0_transfer_complete(pcd);
		pcd->ep0state = EP0_IDLE;
		ep0->is_stopped = 1;
		ep0->is_in = 0;  /* OUT for next SETUP */

		/* Prepare for more SETUP Packets */
		endpoint0_startup_receiving_setup(pcd);
		dwc_otg_debug("STATUS_PHASE\n");
		break;

	case EP0_STALL:
		dwc_otg_debug("EP0 STALL\n");
		break;

	default:
		break;
	}

	return;
}

static void
handle_in_ep_timeout_intr(dwc_otg_pcd_t *pcd, dwc_otg_pcd_ep_t *pcd_ep)
{
	dctl_data_t dctl = {0};

	gintmsk_data_t intr_mask = {0};

	/* Disable the NP Tx Fifo Empty Interrupt */
	intr_mask.b.nptxfempty_rw = 1;
	DWC_MODIFY_REG32(DOTG_GINTMSK, intr_mask.d32, 0);

	/* Enable the Global IN NAK Effective Interrupt */
	intr_mask.d32 = 0;
	intr_mask.b.ginnakeff_rw = 1;
	DWC_MODIFY_REG32(DOTG_GINTMSK, 0, intr_mask.d32);

	/* Set Global IN NAK */
	dctl.b.sgnpinnak_w = 1;
	DWC_MODIFY_REG32(DOTG_DCTL, dctl.d32, dctl.d32);
	pcd_ep->is_stopped = 1;
}

static int32_t
handle_usb_enum_done_intr(dwc_otg_pcd_t *pcd)
{
	gusbcfg_data_t gusbcfg;
	depctl_data_t diepctl;
	dctl_data_t dctl = {0};

	dwc_otg_debug("\n\t[SPEED ENUM]\n");

	/* Read the Device Status and Endpoint 0 Control registers */
	diepctl.d32 = DWC_READ_REG32(DOTG_DIEPCTL(0));

	/* Set the MPS of the IN EP based on the enumeration speed */
	diepctl.b.mps_rw = DEP0CTL_MPS_64;
	DWC_WRITE_REG32(DOTG_DIEPCTL(0), diepctl.d32);

	dctl.b.cgnpinnak_w = 1;
	DWC_MODIFY_REG32(DOTG_DCTL, dctl.d32, dctl.d32);

	if (pcd->ep0state == EP0_DISCONNECT) {
		pcd->ep0state = EP0_IDLE;
	} else if (pcd->ep0state == EP0_STALL) {
		pcd->ep0state = EP0_IDLE;
	}

	pcd->ep0state = EP0_IDLE;
	pcd->pcd_ep0.xfer_type = USB_EP_TYPE_CONTROL;

	/* setup endpoit0 to receive SETUP packets */
	endpoint0_startup_receiving_setup(pcd);

	/* Set USB turnaround time based on device speed and PHY interface. */
	gusbcfg.d32 = DWC_READ_REG32(DOTG_GUSBCFG);

	gusbcfg.b.usbtrdtim_rw = 9;

	DWC_WRITE_REG32(DOTG_GUSBCFG, gusbcfg.d32);

	return (1);
}

static int32_t
handle_endpoint0_transfer_complete(dwc_otg_pcd_t *pcd)
{
	deptsiz0_data_t deptsiz;
	dwc_otg_pcd_ep_t *ep = &pcd->pcd_ep0;
	uint8_t is_last = 0;

	if ((pcd->ep0state == EP0_OUT_STATUS_PHASE) ||
	    (pcd->ep0state == EP0_IN_STATUS_PHASE)) {
		is_last = 1;
	} else if (ep->is_in) {
		deptsiz.d32 = DWC_READ_REG32(DOTG_DIEPTSIZ(0));
		if (deptsiz.b.xfersize_rw == 0) {
			/* Is a Zero Len Packet needed? */
			endpoint0_out_startup_status_phase(pcd);
		}
	} else {
		endpoint0_in_startup_status_phase(pcd);
	}

	/* Complete the request */
	if (is_last) {
		ep->start_xfer_buff = 0;
		ep->xfer_buff = 0;
		ep->xfer_len = 0;
		dwc_otg_core_ep_complete(ep, &pcd->usbd_ep0);
		return (1);
	}
	return (0);
}

static int32_t
handle_out_endpoint_transfer_complete(dwc_otg_pcd_t *pcd, dwc_otg_pcd_ep_t *pcd_ep, struct usbdev_ep_s *ep)
{
	uint8_t epnum = pcd_ep->num;
	doepint_data_t doepint = { .d32 = 0 };
	depctl_data_t diepctl0 = { .d32 = 0 };
	diepint_data_t diepint0 = { .d32 = 0 };
	deptsiz0_data_t deptsiz = { .d32 = 0 };
	doepint.d32 = GET_OEP_INTR(epnum, EPINTR_MSK);

	/* Handle Default Zero Endpoint */
	if (epnum == 0) {
		doepint_data_t doepint_temp = { .d32 = 0 };
		doepint_temp.d32 = DWC_READ_REG32(DOTG_DOEPINT(epnum));
		deptsiz.d32 = DWC_READ_REG32(DOTG_DOEPTSIZ(epnum));

		if (((deptsiz.b.xfersize_rw == 64) &&
		    (pcd->ep0state == EP0_OUT_DATA_PHASE) && (doepint_temp.b.stsphsercvd_r_ss_wc)) ||
		    ((deptsiz.b.xfersize_rw == 24) && (pcd->ep0state == EP0_IN_STATUS_PHASE))) {
			CLEAR_OEP_INTR(epnum, xfercompl_r);
			doepint_temp.b.xfercompl_r = 0;
			endpoint0_startup_receiving_setup(pcd);
			goto handle_out;
		}

		/* Handle Setup Phase */
		if (pcd->ep0state == EP0_IDLE) {
			if (doepint_temp.b.sr_r_ss_wc) { /* Received a new setup packet */
				CLEAR_OEP_INTR(epnum, sr_r_ss_wc);
			}

			/* Delay is needed for receiving more setup packets */
			KAL_Udelay(100);
			doepint.d32 = GET_OEP_INTR(0, EPINTR_MSK);

			for (;;) {
				if (doepint.b.setup_r_ss_wc) {
					doepint.b.setup_r_ss_wc = 0;
					/* Already started data stage, clear setup */
					CLEAR_OEP_INTR(epnum, setup_r_ss_wc);
					handle_default_endpoint0(pcd, 0);

					goto handle_out;
				}
				doepint.d32 = GET_OEP_INTR(0, EPINTR_MSK);
				if (!doepint.b.setup_r_ss_wc) {
					/* Prepare for receiving more setup packets */
					endpoint0_startup_receiving_setup(pcd);
					break;
				}
			}
		} else {
			/* Handle Data&Status Phase */
			if ((pcd->ep0state == EP0_IN_DATA_PHASE) ||
			    (pcd->ep0state == EP0_IN_STATUS_PHASE)) {
				diepint0.d32 = DWC_READ_REG32(DOTG_DIEPINT(0));
				if (diepint0.b.xfercompl_r) {
					DWC_WRITE_REG32(DOTG_DIEPINT(0), diepint0.d32);
				}

				diepctl0.d32 = DWC_READ_REG32(DOTG_DIEPCTL(0));
				if (diepctl0.b.epena_r_ws_sc) {
					dwc_otg_wait_disable_in_endpoint(0, (~0));
				}
			}

			doepint_temp.d32 = GET_OEP_INTR(0, EPINTR_MSK);
			if (doepint_temp.b.sr_r_ss_wc) { /* Received a new setup packet */
				CLEAR_OEP_INTR(epnum, sr_r_ss_wc);
				pcd->ep0state = EP0_IDLE;
				if (doepint.b.setup_r_ss_wc) {
					/* Data stage started, clear setup */
					CLEAR_OEP_INTR(epnum, setup_r_ss_wc);
					doepint.b.setup_r_ss_wc = 0;
					handle_default_endpoint0(pcd, 0);

					/* Prepare for setup packets if ep0in was enabled */
					if (pcd->ep0state == EP0_IN_STATUS_PHASE)
						endpoint0_startup_receiving_setup(pcd);
				} else {
					endpoint0_startup_receiving_setup(pcd);
				}
			}
		}
		if (pcd->ep0state != EP0_IDLE) {
			handle_default_endpoint0(pcd, 0);
		}
	} else { /* Handle nonZero Endpoint */
		handle_normal_endpoint(pcd, pcd_ep, ep);
	}
handle_out:
	return (0);
}

static int32_t
handle_out_endpoint_intr(dwc_otg_pcd_t *pcd)
{
	uint8_t epnum = 0;
	uint32_t ep_intr;
	doepint_data_t doepint = {0};
	dwc_otg_pcd_ep_t *pcd_ep = NULL;
	struct usbdev_ep_s *ep = NULL;
	depctl_data_t depctl;

	dwc_otg_debug("\n\t[EP INTR ----->>OUT]  ep_state:%d   in\n", pcd->ep0state);

	ep_intr = (DWC_READ_REG32(DOTG_DAINT) & DWC_READ_REG32(DOTG_DAINTMSK));
	ep_intr = ((ep_intr & 0xffff0000) >> 16);

	while (ep_intr) {
		if (!(ep_intr & 0x1)) { /* Skip to next endpoint */
			epnum++;
			ep_intr >>= 1;
			continue;
		}
		ep = usbd_get_out_ep(pcd, epnum);
		if ((ep == NULL) || (ep->ep_link == NULL)) {
			return (1);
		}
		pcd_ep = (dwc_otg_pcd_ep_t *)ep->ep_link;

		doepint.d32 = GET_OEP_INTR(epnum, EPINTR_MSK);

		/* Transfer complete */
		if (doepint.b.xfercompl_r) {
			dwc_otg_debug("EP%u OUT Xfer Complete\n", epnum);
			CLEAR_OEP_INTR(epnum, xfercompl_r);
			(void)handle_out_endpoint_transfer_complete(pcd, pcd_ep, ep);
			doepint.d32 = GET_OEP_INTR(epnum, EPINTR_MSK);
			if (doepint.b.setup_r_ss_wc && doepint.b.xfercompl_r) {
				if (!doepint.b.sr_r_ss_wc) {
					epnum++;
					ep_intr >>= 1;
					continue;
				}
			}
		}

		/* Status Phase Received For Control Write */
		if (doepint.b.stsphsercvd_r_ss_wc) {
			CLEAR_OEP_INTR(epnum, stsphsercvd_r_ss_wc);
		}

		/* Setup Phase Done (control EPs) */
		if (doepint.b.setup_r_ss_wc) {
			CLEAR_OEP_INTR(epnum, setup_r_ss_wc);
			handle_default_endpoint0(pcd, 0);
			dwc_otg_debug("b.setup \n");
		}

		/* Endpoint disable  */
		if (doepint.b.epdisabled_r_ss_wc) {
			dwc_otg_debug("EP%u OUT disabled\n", epnum);
			/* Clear the bit in DOEPINTn for this interrupt */
			CLEAR_OEP_INTR(epnum, epdisabled_r_ss_wc);
		}

		if (doepint.b.outtknepdis_r_ss_wc) {
			if (pcd_ep->xfer_type == USB_EP_TYPE_ISOC) {
				pcd_ep->frame_num = pcd->frame_num;
				if (pcd_ep->bInterval > 1) {
					depctl.d32 = DWC_READ_REG32(DOTG_DOEPCTL(epnum));
					if (pcd_ep->frame_num & 0x1) {
						depctl.b.setd1pid_w = 1;
						depctl.b.setd0pid_w = 0;
					} else {
						depctl.b.setd0pid_w = 1;
						depctl.b.setd1pid_w = 0;
					}
					DWC_WRITE_REG32 (DOTG_DOEPCTL(epnum), depctl.d32);
				}
			}
			/* Clear the bit in DOEPINTn for this interrupt */
			CLEAR_OEP_INTR(epnum, outtknepdis_r_ss_wc);
		}

		/* NAK Interrutp */
		if (doepint.b.nak_r_ss_wc) {
			doepmsk_data_t intr_mask = { .d32 = 0 };
			intr_mask.b.nak_r_ss_wc = 1;
			DWC_MODIFY_REG32(DOTG_DOEPMSK, intr_mask.d32, 0);
			CLEAR_OEP_INTR(epnum, nak_r_ss_wc);
			dwc_otg_debug("b.nak \n");
		}

		/* AHB Error */
		if (doepint.b.ahberr_r_ss_wc) {
			dwc_otg_debug("EP%u OUT AHB Error\n", epnum);
			CLEAR_OEP_INTR(epnum, ahberr_r_ss_wc);
		}

		epnum++;
		ep_intr >>= 1;
	}
	return (1);
}

static int32_t
handle_in_endpoint_intr(dwc_otg_pcd_t *pcd)
{
	uint8_t epnum = 0;
	uint32_t ep_intr;
	uint32_t fifoemptymsk;
	diepint_data_t diepint = {0};
	gintsts_data_t gintsts;
	dwc_otg_pcd_ep_t *pcd_ep = NULL;
	struct usbdev_ep_s *ep = NULL;
	depctl_data_t depctl;

	dwc_otg_debug("\n\t[EP INTR ----->>IN]  ep_state:%d   in\n", pcd->ep0state);

	ep_intr = (DWC_READ_REG32(DOTG_DAINT) & DWC_READ_REG32(DOTG_DAINTMSK));
	ep_intr = (ep_intr & 0xffff);

	gintsts.d32 = 0;
	gintsts.b.inepint_r = 1;
	DWC_WRITE_REG32(DOTG_GINTSTS, gintsts.d32);
	DWC_WRITE_REG32(DOTG_DAINT, 0xFFFF);

	while (ep_intr) {
		if (!(ep_intr & 0x1)) { /* Skip to next endpoint */
			epnum++;
			ep_intr >>= 1;
			continue;
		}

		ep = usbd_get_in_ep(pcd, epnum);
		if ((ep == NULL) || (ep->ep_link == NULL)) {
			return (1);
		}
		pcd_ep = (dwc_otg_pcd_ep_t *)ep->ep_link;

		diepint.d32 = GET_IEP_INTR(epnum, EPINTR_MSK);

		/* Transfer complete */
		if (diepint.b.xfercompl_r) {
			fifoemptymsk = 0x1 << epnum;

			/* Disable the Tx FIFO Empty Interrupt for this EP */
			DWC_MODIFY_REG32(DOTG_DTKNQR4, fifoemptymsk, 0);
			CLEAR_IEP_INTR(epnum, xfercompl_r);

			/* Complete the transfer */
			if (epnum == 0) {
				handle_default_endpoint0(pcd, 0);
			} else {
				if ((pcd_ep->xfer_type == USB_EP_TYPE_ISOC) &&
				    (pcd_ep->bInterval > 1)) {
					pcd_ep->frame_num += pcd_ep->bInterval;
					if (pcd_ep->frame_num > 0x3FFF) {
						pcd_ep->frm_overrun = 1;
						pcd_ep->frame_num &= 0x3FFF;
					} else {
						pcd_ep->frm_overrun = 0;
					}
				}
				handle_normal_endpoint(pcd, pcd_ep, ep);
			}
		}
		/* Endpoint disable  */
		if (diepint.b.epdisabled_r_ss_wc) {
			/* Flush the transimit FIFO; */
			dwc_otg_flush_tx_fifo(pcd_ep->fifo_index);
			CLEAR_IEP_INTR(epnum, epdisabled_r_ss_wc);
		}

		/* IN Token received with TxF Empty */
		if (diepint.b.intktxfemp_r_ss_wc) {
			CLEAR_IEP_INTR(epnum, intktxfemp_r_ss_wc);
		}

		/* IN Token Received with EP mismatch */
		if (diepint.b.intknepmis_r_ss_wc) {
			CLEAR_IEP_INTR(epnum, intknepmis_r_ss_wc);
		}

		/* IN Endpoint NAK Effective */
		if (diepint.b.nak_r_ss_wc) {
			if (pcd_ep->xfer_type == USB_EP_TYPE_ISOC) {
				if (pcd_ep->frame_num == 0xFFFFFFFF) {
					pcd_ep->frame_num = pcd->frame_num;
					if (pcd_ep->bInterval > 1) {
						depctl.d32 = 0;
						depctl.d32 = DWC_READ_REG32(DOTG_DIEPCTL(epnum));
						if (pcd_ep->frame_num & 0x1) {
							depctl.b.setd1pid_w = 1;
							depctl.b.setd0pid_w = 0;
						} else {
							depctl.b.setd0pid_w = 1;
							depctl.b.setd1pid_w = 0;
						}
						DWC_WRITE_REG32(DOTG_DIEPCTL(epnum), depctl.d32);
					}
				}
				pcd_ep->frame_num += pcd_ep->bInterval;
				if (pcd_ep->frame_num > 0x3FFF) {
					pcd_ep->frm_overrun = 1;
					pcd_ep->frame_num &= 0x3FFF;
				} else {
					pcd_ep->frm_overrun = 0;
				}
			}
			CLEAR_IEP_INTR(epnum, inepnakeff_r_ss_wc);
		}

		/* AHB Error */
		if (diepint.b.ahberr_r_ss_wc) {
			/* Clear the bit in DIEPINTn for this interrupt */
			CLEAR_IEP_INTR(epnum, ahberr_r_ss_wc);
		}

		/* TimeOUT Handshake (non-ISOC IN EPs) */
		if (diepint.b.timeout_r_ss_wc) {
			handle_in_ep_timeout_intr(pcd, pcd_ep);
			CLEAR_IEP_INTR(epnum, timeout_r_ss_wc);
		}

		if (diepint.b.inepnakeff_r_ss_wc) {
			CLEAR_IEP_INTR(epnum, nak_r_ss_wc);
		}
		epnum++;
		ep_intr >>= 1;
	}
	return (1);
}

static void
handle_usb_suspend_intr(dwc_otg_pcd_t *pcd)
{
	pcgcctl_data_t pcgcctl = { .d32 = 0 };

	dwc_otg_debug("\t[usbsuspend] \n");

	pcgcctl.b.stoppclk_rw = 1;
	DWC_MODIFY_REG32(DOTG_PCGCCTL, 0, pcgcctl.d32);
	KAL_Udelay(10);

	UsbPhySuspend();
	pcd->lx_state = USB_DWC2_STATE_L2;
}

void
endpoint0_startup_receiving_setup(dwc_otg_pcd_t *pcd)
{
	deptsiz0_data_t doeptsize0 = {0};
	depctl_data_t doepctl = {0};

	doepctl.d32 = DWC_READ_REG32(DOTG_DOEPCTL(0));
	if (doepctl.b.epena_r_ws_sc) {
		return;
	}

	doeptsize0.b.supcnt_rw = 3;
	doeptsize0.b.pktcnt_rw = 1;
	doeptsize0.b.xfersize_rw = 8 * 3;

	usb_dma_cache_flush((void *)pcd->setup_pkt, 8);

	DWC_WRITE_REG32(DOTG_DOEPTSIZ(0), doeptsize0.d32);

	/* @todo dma needs to handle multiple setup packets (up to 3) */
	DWC_WRITE_REG32(DOTG_DOEPDMA(0), (uint32_t)(pcd->setup_pkt) + USB_DMA_OFFSET);

	doepctl.d32 = DWC_READ_REG32(DOTG_DOEPCTL(0));
	doepctl.b.epena_r_ws_sc = 1;
	DWC_WRITE_REG32(DOTG_DOEPCTL(0), doepctl.d32);
}

static void
handle_usb_reset_intr(dwc_otg_pcd_t *pcd)
{
	depctl_data_t doepctl = {0};
	daint_data_t daintmsk = {0};
	doepmsk_data_t doepmsk = {0};
	diepmsk_data_t diepmsk = {0};
	dcfg_data_t dcfg = {0};
	depctl_data_t diepctl = {0};
	grstctl_t resetctl = {0};
	dctl_data_t dctl = {0};
	diepint_data_t diepint = { .d32 = 0 };
	pcgcctl_data_t pcgcctl = { .d32 = 0 };
	uint8_t do_try = 0;
	uint8_t ep_num;
	uint8_t i;

	dwc_otg_debug("\n\t[USB RESET]\n");

	if (pcd->lx_state == USB_DWC2_STATE_L2) {
		pcgcctl.b.stoppclk_rw = 1;
		DWC_MODIFY_REG32(DOTG_PCGCCTL, pcgcctl.d32, 0);
		KAL_Udelay(10);
	}

	pcd->lx_state = USB_DWC2_STATE_L0;

	/* Clear the Remote Wakeup Signalling */
	dctl.b.rmtwkupsig_rw = 1;
	DWC_MODIFY_REG32(DOTG_DCTL, dctl.d32, 0);

	/* Set NAK for all Out Endpoints */
	doepctl.d32 = 0;
	doepctl.b.snak_w = 1;
	ep_num = 0;
	for (i = 0; i <= pcd->num_out_eps; i++) {
		if (i) {
			ep_num = pcd->pcd_out_eps[i - 1].num;
		}
		DWC_WRITE_REG32(DOTG_DOEPCTL(ep_num), doepctl.d32);
	}

	/* Disable all active In Endpoints */
	ep_num = 0;
	for (i = 0; i <= pcd->num_in_eps; i++) {
		if (i) {
			ep_num = pcd->pcd_in_eps[i - 1].num;
		}
		diepctl.d32 = DWC_READ_REG32(DOTG_DIEPCTL(ep_num));
		if (diepctl.b.epena_r_ws_sc) {
			diepctl.b.snak_w = 1;
			diepctl.b.epdis_r_ws_sc = 1;
			DWC_WRITE_REG32(DOTG_DIEPCTL(ep_num), diepctl.d32);

			/* Need to wait for ep disable interrupt within 1000us before the endpoint is considered disabled */
			do {
				KAL_Udelay(10);
				diepint.d32 = DWC_READ_REG32(DOTG_DIEPINT(ep_num));
				do_try++;
			} while ((!diepint.b.epdisabled_r_ss_wc) && (do_try < 100));

			diepint.b.epdisabled_r_ss_wc = 1;
			DWC_WRITE_REG32(DOTG_DIEPINT(ep_num), diepint.d32);
		}
	}

	/* 0x10: Flush the NP Tx FIFO */
	dwc_otg_flush_tx_fifo(0x10);

	/* Flush the Learning Queue */
	resetctl.b.intknqflsh_r_ws_sc = 1;
	DWC_WRITE_REG32(DOTG_GRSTCTL, resetctl.d32);

	/* set Default Endpoint Mask */
	daintmsk.b.iep0 = 1;
	daintmsk.b.oep0 = 1;
	DWC_WRITE_REG32(DOTG_DAINTMSK, daintmsk.d32);

	/* set In Endpoint Mask */
	diepmsk.b.xfercompl_r = 1;
	diepmsk.b.timeout_r_ss_wc = 1;
	diepmsk.b.epdisabled_r_ss_wc = 1;
	diepmsk.b.ahberr_r_ss_wc = 1;
	DWC_WRITE_REG32(DOTG_DIEPMSK, diepmsk.d32);

	/* set Out Endpoint Mask */
	doepmsk.b.setup_r_ss_wc = 1;
	doepmsk.b.xfercompl_r = 1;
	doepmsk.b.ahberr_r_ss_wc = 1;
	doepmsk.b.epdisabled_r_ss_wc = 1;
	doepmsk.b.stsphsercvd_r_ss_wc = 1;
	DWC_WRITE_REG32(DOTG_DOEPMSK, doepmsk.d32);

	/* Reset Device Address */
	dcfg.d32 = DWC_READ_REG32(DOTG_DCFG);
	dcfg.b.devaddr_rw = 0;
	DWC_WRITE_REG32(DOTG_DCFG, dcfg.d32);
}

#ifdef LOSCFG_DRIVERS_USB2_DWC_INCOMPISOIN
static int
drop_transfer(uint32_t trgt_fr, uint32_t curr_fr, uint8_t frm_overrun)
{
	int retval = 0;
	if ((!frm_overrun) && (curr_fr >= trgt_fr)) {
		retval = 1;
	} else if (frm_overrun &&
	    ((curr_fr >= trgt_fr) && ((curr_fr - trgt_fr) < 0x3FFF / 2))) {
		retval = 1;
	}
	return (retval);
}

static void
handle_usb_incomplete_isoc_intr(dwc_otg_pcd_t *pcd, gintsts_data_t intsts)
{
	uint8_t i;
	uint8_t epnum;
	uint32_t ctl_reg;
	depctl_data_t epctl;
	dsts_data_t sts;
	gintsts_data_t gintr_status;
	dwc_otg_pcd_ep_t *pcd_ep = NULL;

	(void)intsts; /* not used for now */
	for (i = 0; i < pcd->num_in_eps; ++i) {
		pcd_ep = &pcd->pcd_in_eps[i];
		if (pcd_ep->xfer_type != USB_EP_TYPE_ISOC) {
			continue;
		}

		epnum = pcd_ep->num;
		if (!epnum) {
			continue;
		}
		ctl_reg = DOTG_DIEPCTL(epnum);
		epctl.d32 = DWC_READ_REG32(ctl_reg);

		if (pcd_ep->bInterval == 1) {
			sts.d32 = DWC_READ_REG32(DOTG_DSTS);
			if ((sts.b.soffn_r & 0x1) == epctl.b.dpid_r) {
				if (epctl.b.dpid_r == 1) {
					epctl.b.setd0pid_w = 1;
					epctl.b.setd1pid_w = 0;
				} else if (epctl.b.dpid_r == 0) {
					epctl.b.setd1pid_w = 1;
					epctl.b.setd0pid_w = 0;
				}
				epctl.b.cnak_w = 1;
				DWC_WRITE_REG32(ctl_reg, epctl.d32);
			}
		}

		if (epctl.b.epena_r_ws_sc && (pcd_ep->xfer_type == USB_EP_TYPE_ISOC)) {
			if (drop_transfer(pcd_ep->frame_num, pcd->frame_num, pcd_ep->frm_overrun)) {
				epctl.d32 = DWC_READ_REG32(ctl_reg);
				epctl.b.snak_w = 1;
				epctl.b.epdis_r_ws_sc = 1;
				DWC_WRITE_REG32(ctl_reg, epctl.d32);
			}
		}
	}

	/* Clear interrupt */
	gintr_status.d32 = 0;
	gintr_status.b.incomplisoin_r_ss_wc = 1;
	DWC_WRITE_REG32(DOTG_GINTSTS, gintr_status.d32);
}
#endif

/*
 * Gets the current USB frame number. This is the frame number from the last
 * SOF packet.
 */
uint32_t
dwc_otg_get_frame_number(dwc_otg_pcd_t *pcd)
{
	dsts_data_t dsts;

	dsts.d32 = DWC_READ_REG32(DOTG_DSTS);

	/* read current frame/microframe number from DSTS register */
	return (dsts.b.soffn_r);
}

int
dwc_otg_common_irq(dwc_otg_pcd_t *pcd)
{
	gotgint_data_t gotgint;

	if (DWC_READ_REG32(DOTG_GINTSTS) & 0x1) {
		pcd->frame_num = dwc_otg_get_frame_number(pcd);
	}

	gotgint.d32 = DWC_READ_REG32(DOTG_GOTGINT);
	if (gotgint.d32 == 0) {
		return (0);
	}
	if (gotgint.b.sesreqsucstschng_r_ss_wc) {
		dwc_otg_debug("Session Request Success Status Change\n");
	}
	if (gotgint.b.sesenddet_r_ss_wc) {
		dwc_otg_debug("Session End Detected, Line Disconnected\n");
	}

	DWC_WRITE_REG32(DOTG_GOTGINT, gotgint.d32);
	return (0);
}

#ifdef LOSCFG_DRIVERS_USB_HID_POLLING_REPORTS

/*
 * This function is implemented by the application layer according to the business scenario.
 * NOTE: In high-speed mode, the operation time cannot exceed 125us.
 */
__attribute__((weak)) void
usb_sof_intr_callback(void)
{

}

#endif

int
dwc_otg_pcd_irq(dwc_otg_pcd_t *pcd)
{
	gintsts_data_t  gintr_status;
	gintsts_data_t  gintr_msk;
	pcgcctl_data_t pcgcctl = { .d32 = 0 };

	KAL_SpinLock(&pcd->irq_lock);

	gintr_msk.d32 = DWC_READ_REG32(DOTG_GINTMSK);
	gintr_status.d32 = DWC_READ_REG32(DOTG_GINTSTS);

	if ((gintr_status.d32 & gintr_msk.d32)== 0) {
		dwc_otg_debug("Warning: UDC interrupt is not enable!\n");
		KAL_SpinUnlock(&pcd->irq_lock);
		return (0);
	}

	UsbPhySetDeviceState();

	gintr_status.d32 = gintr_status.d32 & gintr_msk.d32;
	dwc_otg_debug("[irq gintmsk & gintrsts = 0x%08x]\n", gintr_status.d32);

	usb_dma_cache_invalid((void *)pcd->setup_pkt, 8);

#ifdef LOSCFG_DRIVERS_USB_HID_POLLING_REPORTS
	/* Handle usb sof interrupt */
	if (gintr_status.b.sofintr_r_ss_wc) {
		usb_sof_intr_callback();
	}
#endif

	/* Handle usb reset interrupt */
	if (gintr_status.b.usbreset_r_ss_wc) {
		handle_usb_reset_intr(pcd);
		UsbPhyClearDeviceState();
	}

	/* Handle usb enum done interrupt */
	if (gintr_status.b.enumdone_r_ss_wc) {
		(void)handle_usb_enum_done_intr(pcd);
	}

	/* Handle usb in endpoint interrupt */
	if (gintr_status.b.inepint_r) {
		(void)handle_in_endpoint_intr(pcd);
	}

	/* Handle usb out endpoint interrupt */
	if (gintr_status.b.outepintr_r) {
		(void)handle_out_endpoint_intr(pcd);
	}

	/* Handle usb erlysuspend_r_ss_wc interrupt */
	if (gintr_status.b.erlysuspend_r_ss_wc) {
		dwc_otg_debug("\t[erlysuspend]\n");
		UsbPhyClearDeviceState();
	}

	if (gintr_status.b.wkupintr_r_ss_wc) {
		if (pcd->lx_state == USB_DWC2_STATE_L2) {
			dwc_otg_debug("\t[wkupintr]\n");
			UsbPhyResume();

			pcgcctl.b.stoppclk_rw = 1;
			DWC_MODIFY_REG32(DOTG_PCGCCTL, pcgcctl.d32, 0);
			KAL_Udelay(10);
			pcd->lx_state = USB_DWC2_STATE_L0;
		}
	}

	/* Handle usb suspend interrupt */
	if (gintr_status.b.usbsuspend_r_ss_wc) {
		handle_usb_suspend_intr(pcd);
		UsbPhyClearDeviceState();
	}

#ifdef LOSCFG_DRIVERS_USB2_DWC_INCOMPISOIN
	if (gintr_status.b.incomplisoin_r_ss_wc) {
		handle_usb_incomplete_isoc_intr(pcd, gintr_status);
	}
#endif

	if (gintr_status.b.ginnakeff_r) {
		dctl_data_t dctl = { .d32 = 0 };
		dctl.b.cgnpinnak_w = 1;
		DWC_MODIFY_REG32(DOTG_DCTL, dctl.d32, dctl.d32);
	}

	DWC_WRITE_REG32(DOTG_GINTSTS, gintr_status.d32);
	KAL_SpinUnlock(&pcd->irq_lock);
	return (0);
}

uint32_t
dwc_otg_intr_status_get(dwc_otg_pcd_t *pcd)
{
	gintsts_data_t gintr_status;
	gintsts_data_t gintr_msk;
	uint32_t flags;

	KAL_SpinLockSave(&pcd->irq_lock, &flags);
	gintr_msk.d32 = DWC_READ_REG32(DOTG_GINTMSK);
	gintr_status.d32 = DWC_READ_REG32(DOTG_GINTSTS);
	KAL_SpinUnlockRestore(&pcd->irq_lock, flags);

	return (gintr_status.d32 & gintr_msk.d32);
}
