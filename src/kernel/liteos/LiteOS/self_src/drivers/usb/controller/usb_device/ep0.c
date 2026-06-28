/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2021. All rights reserved.
 * Description : LiteOS USB device ep implementation.
 * Author : Huawei LiteOS Team
 * Create : 2013-10-1
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

#include "controller/usb_device/os_defs.h"
#include "controller/usb_device/hw.h"
#include "controller/usb_device/pcd.h"
#include "controller/usb_device/dev.h"
#include "controller/usb_device/os_dev.h"
#include "controller/usb_device/cil.h"

#define dwc3_ep0_show(x...) dprintf(x)
#ifdef DWC3_EP0_DEBUG
#define dwc3_ep0_debug(x...) dwc3_ep0_show(x)
#else
#define dwc3_ep0_debug(x...) do {} while(0)
#endif

/*
 * This routine starts the data stage of a 3-stage control command.
 * pcd->ep0state must be set to USB_EP0_OUT_DATA_PHASE or USB_EP0_IN_DATA_PHASE, and
 * pcd->ep0->dwc_ep.is_in must be set to 0 or 1 before calling this routine.
 * For IN, the data to be sent must be placed in pcd->ep0_status_buf before
 * the call.
 */
void
dwc_usb3_pcd_ep0_data_stage(usb_dwc_pcd_t *pcd, int length)
{
	pcd->ep0_req->dwc_req.buf[0] = (char *)pcd->ep0_status_buf;
	pcd->ep0_req->dwc_req.bufdma[0] = pcd->ep0_status_buf_dma;
	pcd->ep0_req->dwc_req.length = length;
	pcd->ep0_req->dwc_req.actual = 0;
	pcd->ep0_status_pending = 1;
	pcd->ep0->dwc_ep.send_zlp = 0;
	dwc_usb3_pcd_ep0_start_transfer(pcd, pcd->ep0_req);
}

/*
 * This routine processes the SET_ADDRESS Setup Commands.
 */
static void
dwc_do_set_address(usb_dwc_pcd_t *pcd)
{
	struct usb_ctrlreq_s req = pcd->usb_ep0_setup_pkt->req;

	PRINTK("\n ****** NEW USB ADDRESS %03d ******\n", USB_GETW(req.value));

	if (req.type == USB_REQ_RECIPIENT_DEVICE) {
		dwc_usb3_set_address(pcd, USB_GETW(req.value));
		pcd->ep0->dwc_ep.is_in = 1;
		pcd->ep0state = USB_EP0_IN_WAIT_NRDY;
		if (USB_GETW(req.value))
			pcd->state = USB_DWC_STATE_ADDRESSED;
		else
			pcd->state = USB_DWC_STATE_DEFAULT;
	}
}

/*
 * This routine stalls EP0.
 */
static void
dwc_ep0_do_stall(usb_dwc_pcd_t *pcd, int err_val)
{
	usb_dwc_pcd_ep_t *ep = pcd->ep0;

	ep->dwc_ep.is_in = 0;
	dwc_usb3_pcd_ep_set_stall(pcd, ep);
	ep->dwc_ep.stopped = 1;
	pcd->ep0state = USB_EP0_IDLE;
	dwc_usb3_pcd_ep0_out_start(pcd);
}

/*
 * Clear the EP halt (STALL), and if there are pending requests start the transfer.
 */
static void
dwc_do_clear_halt(usb_dwc_pcd_t *pcd, usb_dwc_pcd_ep_t *ep)
{
	usb_dwc_dev_ep_regs_t *ep_regs = NULL;
	usb_dwc_pcd_ep_t *ep_t = ep;

	dwc3_ep0_debug("%s()\n", __FUNCTION__);

	if (ep_t->dwc_ep.stall_clear_flag == 0) {
		if (ep_t != pcd->ep0) {
			dwc_usb3_pcd_ep_clear_stall(pcd, ep_t);
		}

		if (ep_t->dwc_ep.stopped) {
			ep_t->dwc_ep.stopped = 0;

			if ((ep_t != pcd->ep0) && ep_t->dwc_ep.is_in) {
				dwc_usb3_gadget_start_next_request(pcd, ep_t);
			}
		}
	} else {
		dwc_usb3_dis_usb2_suspend(pcd);

		if (ep_t->dwc_ep.is_in) {
			ep_regs = ep_t->dwc_ep.in_ep_reg;
			(void)dwc_usb3_dep_cfg(pcd, ep_regs, ep_t->dwc_ep.param0in,
			    ep_t->dwc_ep.param1in, 0);
		} else {
			ep_regs = ep_t->dwc_ep.out_ep_reg;
			(void)dwc_usb3_dep_cfg(pcd, ep_regs, ep_t->dwc_ep.param0out,
			    ep_t->dwc_ep.param1out, 0);
		}

		dwc_usb3_ena_usb2_suspend(pcd);
	}

	/* Then Start Control Status Phase */
	pcd->ep0->dwc_ep.is_in = 1;
	pcd->ep0state = USB_EP0_IN_WAIT_NRDY;
}

/*
 * This routine handles the Get Descriptor request for the BOS descriptor
 * and the OTG descriptor, and passes all other requests to the Gadget driver.
 */
static void
dwc_do_get_descriptor(usb_dwc_pcd_t *pcd)
{
	struct usb_ctrlreq_s ctrl = pcd->usb_ep0_setup_pkt->req;
	uint8_t desc_type = USB_GETW(ctrl.value) >> 8;
	int ret;

	dwc3_ep0_debug("%s desc_type=%x\n", __FUNCTION__, desc_type);

	switch (desc_type) {
	case USB_DESC_TYPE_BOS:
		dwc3_ep0_debug("\nGET_DESCRIPTOR(BOS)\n\n");
		if ((pcd->speed != (u8)USB_SPEED_SUPER) &&
		    pcd->usb3_dev->core_params->nobos) {
			dwc_ep0_do_stall(pcd, -DWC3_USB_E_NOT_SUPPORTED);
			return;
		}
		/* fall-through */

	default:
		/* Call the Gadget driver's setup routine */
		ret = dwc_usb3_gadget_setup(pcd, &ctrl);
		if (ret < 0)
			dwc_ep0_do_stall(pcd, ret);
		break;
	}
}

/*
 * This routine processes the GET_STATUS Setup Commands.
 */
static void
dwc_do_get_status(usb_dwc_pcd_t *pcd)
{
	struct usb_ctrlreq_s req = pcd->usb_ep0_setup_pkt->req;
	uint8_t *status_buf = pcd->ep0_status_buf;
	usb_dwc_pcd_ep_t *ep = NULL;
	int length;

	if (USB_GETW(req.len) != 2) {
		dwc_ep0_do_stall(pcd, -DWC3_USB_E_NOT_SUPPORTED);
		return;
	}

	switch (req.type & USB_REQ_RECIPIENT_MASK) {
	case USB_REQ_RECIPIENT_DEVICE:
		*status_buf = 1; /* Self powered */

		if (pcd->speed == USB_SPEED_SUPER) {
			if (pcd->state == USB_DWC_STATE_CONFIGURED) {
				if (dwc_usb3_u1_enabled(pcd))
					*status_buf |= 1 << 2;

				if (dwc_usb3_u2_enabled(pcd))
					*status_buf |= 1 << 3;

				*status_buf |= pcd->ltm_enable << 4;
			}
		} else {
			*status_buf |= pcd->remote_wakeup_enable << 1;
		}

		dwc3_ep0_debug("GET_STATUS(Device)=%02x\n", *status_buf);
		*(status_buf + 1) = 0;
		break;

	case USB_REQ_RECIPIENT_INTERFACE:
		*status_buf = 0;
		if (pcd->usb3_dev->core_params->wakeup)
			*status_buf |= 1;
		*status_buf |= pcd->remote_wakeup_enable << 1;
		dwc3_ep0_debug("GET_STATUS(Interface %u)=%02x\n",
		    USB_GETW(req.index), *status_buf);
		*(status_buf + 1) = 0;
		break;

	case USB_REQ_RECIPIENT_ENDPOINT:
		ep = dwc_usb3_pcd_get_ep_by_addr(pcd, USB_GETW(req.index));

		/* @todo check for EP stall */
		*status_buf = ep->dwc_ep.stopped;
		dwc3_ep0_debug("GET_STATUS(Endpoint %u)=%02x\n",
		    USB_GETW(req.index), *status_buf);
		*(status_buf + 1) = 0;
		break;

	default:
		dwc_ep0_do_stall(pcd, -DWC3_USB_E_NOT_SUPPORTED);
		return;
	}

	length = 2;
	dwc_usb3_pcd_ep0_data_stage(pcd, length);
}

/*
 * This routine processes the SET_FEATURE Setup Commands.
 */
static void
dwc_do_set_feature(usb_dwc_pcd_t *pcd)
{
	struct usb_ctrlreq_s req = pcd->usb_ep0_setup_pkt->req;
	usb_dwc_pcd_ep_t *ep = NULL;
	int ret;

	switch (req.type & USB_REQ_RECIPIENT_MASK) {
	case USB_REQ_RECIPIENT_DEVICE:
		switch (USB_GETW(req.value)) {
		case USB_FEATURE_REMOTEWAKEUP:
			pcd->remote_wakeup_enable = 1;
			break;

		case USB_FEATURE_TESTMODE:
			pcd->test_mode = USB_GETW(req.index) >> 8;
			break;

		case USB_FEATURE_BHNPENABLE:
			dwc3_ep0_debug("SET_FEATURE: USB_DEVICE_B_HNP_ENABLE\n");
			break;

		case USB_OTG_NTF_HOST_REL:
			dwc3_ep0_debug("SET_FEATURE: USB_NTF_HOST_REL\n");
			break;

		case USB_OTG_B3_RSP_ENABLE:
			dwc3_ep0_debug("SET_FEATURE: USB_B3_RSP_ENABLE\n");
			break;

		case USB_FEATURE_AHNPSUPPORT:
			dwc3_ep0_debug("SET_FEATURE: USB_DEVICE_A_HNP_SUPPORT\n");
			break;

		case USB_FEATURE_AALTHNPSUPPORT:
			dwc3_ep0_debug("SET_FEATURE: USB_DEVICE_A_ALT_HNP_SUPPORT\n");
			break;

		case USB_FEATURE_U1ENABLE:
			dwc3_ep0_debug("SET_FEATURE: USB_FEATURE_U1ENABLE\n");
			if ((pcd->speed != USB_SPEED_SUPER) ||
			    (pcd->state != USB_DWC_STATE_CONFIGURED)) {
				dwc_ep0_do_stall(pcd, -DWC3_USB_E_NOT_SUPPORTED);
				return;
			}

			if (pcd->usb3_dev->core_params->pwrctl & 1)
				dwc_usb3_enable_u1(pcd);
			break;

		case USB_FEATURE_U2ENABLE:
			dwc3_ep0_debug("SET_FEATURE: USB_FEATURE_U2ENABLE\n");
			if ((pcd->speed != USB_SPEED_SUPER) ||
			    (pcd->state != USB_DWC_STATE_CONFIGURED)) {
				dwc_ep0_do_stall(pcd, -DWC3_USB_E_NOT_SUPPORTED);
				return;
			}

			if (pcd->usb3_dev->core_params->pwrctl & 2)
				dwc_usb3_enable_u2(pcd);
			break;

		case USB_FEATURE_LTMENABLE:
			dwc3_ep0_debug("SET_FEATURE: USB_FEATURE_LTMENABLE\n");
			if ((pcd->speed != USB_SPEED_SUPER) ||
			    (pcd->state != USB_DWC_STATE_CONFIGURED) ||
			    (USB_GETW(req.index) != 0)) {
				dwc_ep0_do_stall(pcd, -DWC3_USB_E_NOT_SUPPORTED);
				return;
			}

			pcd->ltm_enable = 1;
			pcd->send_lpm = 1;
			break;

		default:
			dwc_ep0_do_stall(pcd, -DWC3_USB_E_NOT_SUPPORTED);
			return;
		}

		break;

	case USB_REQ_RECIPIENT_INTERFACE:
		if (USB_GETW(req.value) == 0) {
			if (USB_GETW(req.index) >> 8 & 2)
				pcd->remote_wakeup_enable = 1;
			else
				pcd->remote_wakeup_enable = 0;
			break;
		}

		ret = dwc_usb3_gadget_setup(pcd, &req);
		if (ret < 0)
			dwc_ep0_do_stall(pcd, ret);
		return;

	case USB_REQ_RECIPIENT_ENDPOINT:
		ep = dwc_usb3_pcd_get_ep_by_addr(pcd, USB_GETW(req.index));
		if (USB_GETW(req.value) != USB_FEATURE_ENDPOINTHALT) {
			dwc_ep0_do_stall(pcd, -DWC3_USB_E_NOT_SUPPORTED);
			return;
		}

		ep->dwc_ep.stopped = 1;
		dwc_usb3_pcd_ep_set_stall(pcd, ep);
		break;

	default:
		break;
	}

	pcd->ep0->dwc_ep.is_in = 1;
	pcd->ep0state = USB_EP0_IN_WAIT_NRDY;
}

/*
 * This routine processes the CLEAR_FEATURE Setup Commands.
 */
static void
dwc_do_clear_feature(usb_dwc_pcd_t *pcd)
{
	struct usb_ctrlreq_s req = pcd->usb_ep0_setup_pkt->req;
	usb_dwc_pcd_ep_t *ep = NULL;

	switch (req.type & USB_REQ_RECIPIENT_MASK) {
	case USB_REQ_RECIPIENT_DEVICE:
		switch (USB_GETW(req.value)) {
		case USB_FEATURE_REMOTEWAKEUP:
			pcd->remote_wakeup_enable = 0;
			break;

		case USB_FEATURE_TESTMODE:
			break;

		case USB_FEATURE_U1ENABLE:
			dwc3_ep0_debug("CLEAR_FEATURE: USB_FEATURE_U1ENABLE\n");
			if ((pcd->speed != USB_SPEED_SUPER) ||
			    (pcd->state != USB_DWC_STATE_CONFIGURED)) {
				dwc_ep0_do_stall(pcd, -DWC3_USB_E_NOT_SUPPORTED);
				return;
			}

			dwc_usb3_disable_u1(pcd);
			break;

		case USB_FEATURE_U2ENABLE:
			dwc3_ep0_debug("CLEAR_FEATURE: USB_FEATURE_U2ENABLE\n");
			if ((pcd->speed != USB_SPEED_SUPER) ||
			    (pcd->state != USB_DWC_STATE_CONFIGURED)) {
				dwc_ep0_do_stall(pcd, -DWC3_USB_E_NOT_SUPPORTED);
				return;
			}

			dwc_usb3_disable_u2(pcd);
			break;

		case USB_FEATURE_LTMENABLE:
			dwc3_ep0_debug("CLEAR_FEATURE: USB_FEATURE_LTMENABLE\n");
			if ((pcd->speed != USB_SPEED_SUPER) ||
			    (pcd->state != USB_DWC_STATE_CONFIGURED) ||
			    (USB_GETW(req.index) != 0)) {
				dwc_ep0_do_stall(pcd, -DWC3_USB_E_NOT_SUPPORTED);
				return;
			}

			pcd->ltm_enable = 0;
			pcd->send_lpm = 1;
			break;

		default:
			dwc_ep0_do_stall(pcd, -DWC3_USB_E_NOT_SUPPORTED);
			return;
		}
		break;

	case USB_REQ_RECIPIENT_INTERFACE:
		if (USB_GETW(req.value) == 0) {
			if (USB_GETW(req.index) >> 8 & 2) {
				pcd->remote_wakeup_enable = 0;
			}
			break;
		}

		dwc_ep0_do_stall(pcd, -DWC3_USB_E_NOT_SUPPORTED);
		return;

	case USB_REQ_RECIPIENT_ENDPOINT:
		ep = dwc_usb3_pcd_get_ep_by_addr(pcd, USB_GETW(req.index));
		if (USB_GETW(req.value) != USB_FEATURE_ENDPOINTHALT) {
			dwc_ep0_do_stall(pcd, -DWC3_USB_E_NOT_SUPPORTED);
			return;
		}

		dwc_do_clear_halt(pcd, ep);
		return;

	default:
		break;
	}

	pcd->ep0->dwc_ep.is_in = 1;
	pcd->ep0state = USB_EP0_IN_WAIT_NRDY;
}

void
dwc_usb3_pcd_do_setup(usb_dwc_pcd_t *pcd)
{
	struct usb_ctrlreq_s req = pcd->usb_ep0_setup_pkt->req;
	usb_dwc_pcd_ep_t *ep = pcd->ep0;
	unsigned short value;
	unsigned short len;
	int ret;

	value = USB_GETW(req.value);
	len = USB_GETW(req.len);

	dwc_usb3_gadget_request_nuke(pcd, ep);
	ep->dwc_ep.stopped = 0;
	ep->dwc_ep.three_stage = 1;

	if (req.type & USB_DIR_IN) {
		ep->dwc_ep.is_in = 1;
		pcd->ep0state = USB_EP0_IN_DATA_PHASE;
	} else {
		ep->dwc_ep.is_in = 0;
		pcd->ep0state = USB_EP0_OUT_DATA_PHASE;
	}

	if (len == 0) {
		ep->dwc_ep.is_in = 1;
		pcd->ep0state = USB_EP0_IN_WAIT_GADGET;
		ep->dwc_ep.three_stage = 0;
	}

	if ((req.type & USB_REQ_TYPE_MASK) != USB_REQ_TYPE_STANDARD) {
		ret = dwc_usb3_gadget_setup(pcd, &req);
		if (ret < 0)
			dwc_ep0_do_stall(pcd, ret);
		return;
	}

	switch (req.req) {
	case USB_REQ_GETSTATUS:
		dwc_do_get_status(pcd);
		break;

	case USB_REQ_CLEARFEATURE:
		dwc_do_clear_feature(pcd);
		break;

	case USB_REQ_SETFEATURE:
		dwc_do_set_feature(pcd);
		break;

	case USB_REQ_SETADDRESS:
		dwc_do_set_address(pcd);
		break;

	case USB_REQ_SETINTERFACE:
		dwc3_ep0_debug("USB_REQ_SET_INTERFACE\n");
		ret = dwc_usb3_gadget_setup(pcd, &req);
		if (ret < 0) {
			dwc_ep0_do_stall(pcd, ret);
			return;
		}
		break;

	case USB_REQ_SETCONFIGURATION:
		dwc3_ep0_debug("USB_REQ_SET_CONFIGURATION\n");

		if (pcd->ltm_enable)
			pcd->send_lpm = 1;

		ret = dwc_usb3_gadget_setup(pcd, &req);
		if (ret >= 0) {
			if (value != 0)
				pcd->state = USB_DWC_STATE_CONFIGURED;
			else
				pcd->state = USB_DWC_STATE_ADDRESSED;
		} else {
			dwc_ep0_do_stall(pcd, ret);
			return;
		}

		if (pcd->usb3_dev->core_params->pwrctl & 1)
			dwc_usb3_accept_u1(pcd);
		if (pcd->usb3_dev->core_params->pwrctl & 2)
			dwc_usb3_accept_u2(pcd);

		pcd->ltm_enable = 0;
		break;

	case USB_REQ_GETDESCRIPTOR:
		dwc_do_get_descriptor(pcd);
		break;

	case USB_REQ_SETSEL:
		dwc3_ep0_debug("USB_REQ_SET_SEL\n");

		dwc_usb3_pcd_ep0_data_stage(pcd, USB_DWC_STATUS_BUF_SIZE < len ?
		    USB_DWC_STATUS_BUF_SIZE : len);
		break;

	case USB_REQ_SETISOCHDELAY:
		dwc3_ep0_debug("USB_REQ_SET_ISOC_DELAY\n");

		ep->dwc_ep.is_in = 1;
		pcd->ep0state = USB_EP0_IN_WAIT_NRDY;
		break;

	default:
		ret = dwc_usb3_gadget_setup(pcd, &req);
		if (ret < 0)
			dwc_ep0_do_stall(pcd, ret);
		break;
	}
}

/*
 * This routine starts the Zero-Length Packet for the IN status phase of a
 * control write transfer.
 */
static void
setup_in_status_phase(usb_dwc_pcd_t *pcd, void *buf, usb_dwc_dma_t dma)
{
	usb_dwc_pcd_ep_t *ep = pcd->ep0;

	dwc3_ep0_debug("%s()\n", __FUNCTION__);

	if (pcd->ep0state == USB_EP0_STALL) {
		dwc3_ep0_show("EP0 STALLED\n");
		return;
	}

	ep->dwc_ep.is_in = 1;
	pcd->ep0state = USB_EP0_IN_STATUS_PHASE;

	dwc3_ep0_debug("EP0 IN ZLP\n");

	pcd->ep0_req->dwc_req.buf[0] = buf;
	pcd->ep0_req->dwc_req.bufdma[0] = dma;
	pcd->ep0_req->dwc_req.length = 0;
	pcd->ep0_req->dwc_req.actual = 0;
	dwc_usb3_pcd_ep0_start_transfer(pcd, pcd->ep0_req);
}

/*
 * This routine starts the Zero-Length Packet for the OUT status phase of a
 * control read transfer.
 */
static void
setup_out_status_phase(usb_dwc_pcd_t *pcd, void *buf, usb_dwc_dma_t dma)
{
	usb_dwc_pcd_ep_t *ep = pcd->ep0;

	dwc3_ep0_debug("%s()\n", __FUNCTION__);

	if (pcd->ep0state == USB_EP0_STALL) {
		dwc3_ep0_show("EP0 STALLED\n");
		return;
	}

	ep->dwc_ep.is_in = 0;
	pcd->ep0state = USB_EP0_OUT_STATUS_PHASE;

	dwc3_ep0_debug("EP0 OUT ZLP\n");

	pcd->ep0_req->dwc_req.buf[0] = buf;
	pcd->ep0_req->dwc_req.bufdma[0] = dma;
	pcd->ep0_req->dwc_req.length = 0;
	pcd->ep0_req->dwc_req.actual = 0;
	dwc_usb3_pcd_ep0_start_transfer(pcd, pcd->ep0_req);
}

/*
 * This routine completes the ep0 control transfer.
 */
static int
ep0_complete_request(usb_dwc_pcd_t *pcd, usb_dwc_pcd_req_t *req,
    const usb_dwc_dma_desc_t *pdesc, int status)
{
	usb_dwc_pcd_ep_t *ep = pcd->ep0;
	int is_last = 0;

	if (pcd->ep0_status_pending && !req) {
		if (ep->dwc_ep.is_in) {
			pcd->ep0->dwc_ep.is_in = 0;
			pcd->ep0state = USB_EP0_OUT_WAIT_NRDY;
		} else {
			pcd->ep0->dwc_ep.is_in = 1;
			pcd->ep0state = USB_EP0_IN_WAIT_NRDY;
		}

		pcd->ep0_status_pending = 0;
		return (1);
	}

	if (req == NULL)
		return (0);

	if ((pcd->ep0state == USB_EP0_OUT_STATUS_PHASE) ||
	    (pcd->ep0state == USB_EP0_IN_STATUS_PHASE)) {
		is_last = 1;

	} else if (ep->dwc_ep.is_in) {
		if (dwc_usb3_get_xfercnt(pdesc) == 0) {
			/* Is a Zero Len Packet needed? */
			if (req->dwc_req.flags & USB_DWC_PCD_REQ_ZERO) {
				req->dwc_req.flags &= ~USB_DWC_PCD_REQ_ZERO;
			}

			pcd->ep0->dwc_ep.is_in = 0;
			pcd->ep0state = USB_EP0_OUT_WAIT_NRDY;
		}
	} else {
		/* Is a Zero Len Packet needed? */
		if (req->dwc_req.flags & USB_DWC_PCD_REQ_ZERO) {
			req->dwc_req.flags &= ~USB_DWC_PCD_REQ_ZERO;
		}

		pcd->ep0->dwc_ep.is_in = 1;
		pcd->ep0state = USB_EP0_IN_WAIT_NRDY;
	}

	/* Complete the request */
	if (is_last) {
		dwc3_ep0_debug("is_last len=%u actual=%u\n",
		    req->dwc_req.length, req->dwc_req.actual);
		dwc_usb3_pcd_request_done(pcd, ep, req, status);
		return (1);
	}

	return (0);
}

/*
 * This routine handles EP0 Control transfers.
 *
 * The state of the control transfers are tracked in <code>ep0state</code>.
 */
static void
dwc_usb3_handle_ep0(usb_dwc_pcd_t *pcd, usb_dwc_pcd_req_t *req, uint32_t event)
{
	usb_dwc_pcd_ep_t *ep0 = pcd->ep0;
	usb_dwc_dma_desc_t *pdesc = NULL;
	uint32_t byte_count;
	uint32_t len;
	uint32_t status;

	(void)event;

	dwc3_ep0_debug("HANDLE EP0 state=%d\n", pcd->ep0state);

	switch (pcd->ep0state) {
	case USB_EP0_IN_DATA_PHASE:
		if (req == NULL) {
			req = pcd->ep0_req;
		}
		pdesc = dwc_usb3_ep0_in_desc(pcd);

		if (dwc_usb3_is_hwo(pdesc)) {
			dwc3_ep0_show("### %s, EP%u-%s HWO bit set 1! ###\n",
			    __FUNCTION__, ep0->dwc_ep.num, ep0->dwc_ep.is_in ? "IN" : "OUT");
			goto out;
		}

		status = dwc_usb3_get_xfersts(pdesc);
		if (status & USB_DWC_TRBRSP_SETUP_PEND) {
			/* Start of a new Control transfer */
			dwc3_ep0_debug("IN SETUP PENDING\n");
			pdesc->status = 0;
		}

		byte_count = req->dwc_req.length - dwc_usb3_get_xfercnt(pdesc);
		req->dwc_req.actual += byte_count;
		req->dwc_req.buf[0] += byte_count;
		req->dwc_req.bufdma[0] += byte_count;
		dwc3_ep0_debug("length=%u byte_count=%u actual=%u\n",
		    req->dwc_req.length, byte_count, req->dwc_req.actual);

		if (req->dwc_req.actual < req->dwc_req.length) {
			dwc3_ep0_debug("IN CONTINUE\n");
			dwc3_ep0_debug("Stall EP0\n");
			ep0->dwc_ep.is_in = 0;
			dwc_usb3_pcd_ep_set_stall(pcd, ep0);
			ep0->dwc_ep.stopped = 1;
			pcd->ep0state = USB_EP0_IDLE;
			dwc_usb3_pcd_ep0_out_start(pcd);

		} else if (ep0->dwc_ep.send_zlp) {
			dwc3_ep0_debug("IN ZLP\n");
			dwc_usb3_pcd_ep0_continue_transfer(pcd, req);
			ep0->dwc_ep.send_zlp = 0;
			dwc3_ep0_debug("CONTINUE TRANSFER\n");

		} else {
			dwc3_ep0_debug("IN COMPLETE\n");
			/* This sets ep0state = EP0_IN/OUT_WAIT_NRDY */
			(void)ep0_complete_request(pcd, req, pdesc, 0);
			dwc3_ep0_debug("COMPLETE TRANSFER\n");
		}
		break;

	case USB_EP0_OUT_DATA_PHASE:
		if (req == NULL) {
			req = pcd->ep0_req;
		}
		pdesc = dwc_usb3_ep0_out_desc(pcd);
		if (dwc_usb3_is_hwo(pdesc)) {
			dwc3_ep0_show(
			    "### %s, EP%u-%s HWO bit set 2! ###\n",
			    __FUNCTION__, ep0->dwc_ep.num, ep0->dwc_ep.is_in ?
			    "IN" : "OUT");
			goto out;
		}

		status = dwc_usb3_get_xfersts(pdesc);
		if (status & USB_DWC_TRBRSP_SETUP_PEND) {
			/* Start of a new Control transfer */
			dwc3_ep0_show("OUT SETUP PENDING\n");
		}

		len = (req->dwc_req.length + ep0->dwc_ep.maxpacket - 1) &
		    ~(ep0->dwc_ep.maxpacket - 1);
		byte_count = len - dwc_usb3_get_xfercnt(pdesc);
		req->dwc_req.actual += byte_count;
		req->dwc_req.buf[0] += byte_count;
		req->dwc_req.bufdma[0] += byte_count;
		dwc3_ep0_debug("length=%u byte_count=%u actual=%u\n",
		    req->dwc_req.length, byte_count, req->dwc_req.actual);

		if (ep0->dwc_ep.send_zlp) {
			dwc3_ep0_debug("OUT ZLP\n");
			dwc_usb3_pcd_ep0_continue_transfer(pcd, req);
			ep0->dwc_ep.send_zlp = 0;
			dwc3_ep0_debug("CONTINUE TRANSFER\n");

		} else {
			dwc3_ep0_debug("OUT COMPLETE\n");
			/* This sets ep0state = EP0_IN/OUT_WAIT_NRDY */
			(void)ep0_complete_request(pcd, req, pdesc, 0);
			dwc3_ep0_debug("COMPLETE TRANSFER\n");
		}
		break;

	case USB_EP0_IN_WAIT_GADGET:
		pcd->ep0state = USB_EP0_IN_WAIT_NRDY;
		break;

	case USB_EP0_OUT_WAIT_GADGET:
		pcd->ep0state = USB_EP0_OUT_WAIT_NRDY;
		break;

	case USB_EP0_IN_WAIT_NRDY:
	case USB_EP0_OUT_WAIT_NRDY:
		if (ep0->dwc_ep.is_in)
			setup_in_status_phase(pcd, dwc_usb3_ep0_setup_pkt(pcd),
			    dwc_usb3_ep0_setup_pkt_dma(pcd));
		else
			setup_out_status_phase(pcd, dwc_usb3_ep0_setup_pkt(pcd),
			    dwc_usb3_ep0_setup_pkt_dma(pcd));
		break;

	case USB_EP0_IN_STATUS_PHASE:
	case USB_EP0_OUT_STATUS_PHASE:
		dwc3_ep0_debug("EP0_IN/OUT_STATUS_PHASE is_in:%u\n", ep0->dwc_ep.is_in);
		if (ep0->dwc_ep.is_in)
			pdesc = dwc_usb3_ep0_in_desc(pcd);
		else
			pdesc = dwc_usb3_ep0_out_desc(pcd);

		(void)ep0_complete_request(pcd, req, pdesc, 0);
		pcd->ep0state = USB_EP0_IDLE;
		ep0->dwc_ep.stopped = 1;
		ep0->dwc_ep.is_in = 0;	/* OUT for next SETUP */

		if (pcd->send_lpm) {
			pcd->send_lpm = 0;
		}

		/* Prepare for more SETUP Packets */
		dwc_usb3_pcd_ep0_out_start(pcd);
		break;

	case USB_EP0_STALL:
		dwc3_ep0_debug("EP0 STALLed, should not get here\n");
		break;

	case USB_EP0_IDLE:
		dwc3_ep0_debug("EP0 IDLE, should not get here\n");
		break;

	default:
		break;
	}
out:
	return;
}

/*
 * This routine handles EP0 transfers.
 *
 * This routine gets the request corresponding to the current EP0 transfer. If
 * EP0 is in IDLE state, it calls dwc_usb3_do_setup() to begin processing
 * the next Setup request, otherwise it calls dwc_usb3_handle_ep0() to handle
 * the next stage of the current transfer.
 */
void
dwc_usb3_handle_ep0_xfer(usb_dwc_pcd_t *pcd, uint32_t event)
{
	usb_dwc_pcd_ep_t *ep0 = pcd->ep0;
	usb_dwc_pcd_req_t *req = NULL;

	req = dwc_usb3_gadget_get_request(pcd, ep0);

	if (pcd->ep0state == USB_EP0_IDLE) {
		pcd->request_config = 0;
		dwc_usb3_gadget_do_setup(pcd);
	} else {
		dwc_usb3_handle_ep0(pcd, req, event);
	}
}
