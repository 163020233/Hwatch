/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2014-2021. All rights reserved.
 * Description: LiteOS USB pcd interrupt implementation
 * Author: Huawei LiteOS Team
 * Create: 2014-11-11
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
#include "phy/usb_phy.h"

#define PRINT_HEAD "PCD_INTR:"
#define dwc3_pi_show(x...) dprintf(PRINT_HEAD x)

#ifdef DWC3_PCD_INTR_DEBUG
#define dwc3_pi_debug(x...) dwc3_pi_show(x)
#else
#define dwc3_pi_debug(x...) do {} while (0)
#endif

/*
 * This interrupt indicates that the USB link state has changed to L2, U3, or
 * (if L1 Hibernation is enabled) L1, and software intervention is required.
 */
static int
handle_hiber_req_intr(usb_dwc_pcd_t *usb3_pcd, uint32_t event)
{
	int nsuperspeed;
	uint32_t state;

	dwc3_pi_debug("HIBERNATION REQUEST\n");

	nsuperspeed = !!(event & USB_DWC_DEVT_HIBER_SS_BIT);
	state = event >> USB_DWC_DEVT_HIBER_STATE_SHIFT &
	    USB_DWC_DEVT_HIBER_STATE_BITS >> USB_DWC_DEVT_HIBER_STATE_SHIFT;
	dwc3_pi_debug("state=%x, nsuperspeed=%d\n",
	    state, nsuperspeed);

	if (!((state == USB_DWC_LINK_STATE_U3) ||
	   ((!nsuperspeed) && (state == USB_DWC_LINK_STATE_SLEEP)))) {
		dwc3_pi_debug("HIBERNATION not handled\n");
		return (1);
	}

	if (usb3_pcd->usb3_dev->core_params->hibernate &&
	    ((usb3_pcd->usb3_dev->hwparams1 & USB_DWC_HWP1_EN_PWROPT_BITS) ==
	    (USB_DWC_EN_PWROPT_HIBERNATION << USB_DWC_HWP1_EN_PWROPT_SHIFT))) {

		usb3_pcd->usb3_dev->hibernate = DWC_HIBER_ENTER_SAVE;
		return (1);
	}

	return (0);
}

/*
 * This interrupt indicates that the device has been disconnected.
 */
static int
handle_disconnect_intr(usb_dwc_pcd_t *usb3_pcd)
{
	volatile dwc_usb3_device_t *dev = usb3_pcd->usb3_dev;
	uint32_t temp;

	dwc3_pi_debug("DISCONNECT\n");

	/*
	 * Must set DCTL[8:5] to 5, according to 2.40a and later databook.
	 * Assuming here this will be harmless on earlier cores.
	 */
	temp = dwc_rd32(dev, &usb3_pcd->dev_global_regs->dctl);
	temp &= ~USB_DWC_DCTL_ULST_CHNG_REQ_BITS;
	temp |= USB_DWC_LINK_STATE_REQ_RX_DETECT << USB_DWC_DCTL_ULST_CHNG_REQ_SHIFT;
	dwc_wr32(dev, &usb3_pcd->dev_global_regs->dctl, temp);

	dwc_usb3_clr_eps_enabled(usb3_pcd);
	dwc_usb3_pcd_stop(usb3_pcd);

	return (0);
}

/*
 * This interrupt occurs when a USB Reset is detected. When the USB Reset
 * Interrupt occurs, all transfers are stopped and the device state is set
 * to DEFAULT.
 */
static void
handle_usb_reset_intr(usb_dwc_pcd_t *usb3_pcd)
{
	usb_dwc_pcd_ep_t *usbd_ep = NULL;
	uint8_t i;

	dwc3_pi_debug("USB RESET\n");

	if (usb3_pcd->state != USB_DWC_STATE_UNCONNECTED) {
		dwc3_pi_debug("fake DISCONNECT\n");
		dwc_usb3_clr_eps_enabled(usb3_pcd);
		dwc_usb3_pcd_stop(usb3_pcd);
	} else {
		dwc_usb3_stop_all_xfers(usb3_pcd);
		dwc_usb3_clr_eps_enabled(usb3_pcd);
	}

	for (i = 0; i < usb3_pcd->num_in_eps; i++) {
		usbd_ep = usb3_pcd->in_ep[i];
		if (usbd_ep->dwc_ep.active && usbd_ep->dwc_ep.stopped)
			dwc_usb3_pcd_ep_clear_stall(usb3_pcd, usbd_ep);
	}
	for (i = 0; i < usb3_pcd->num_out_eps; i++) {
		usbd_ep = usb3_pcd->out_ep[i];
		if (usbd_ep->dwc_ep.active && usbd_ep->dwc_ep.stopped)
			dwc_usb3_pcd_ep_clear_stall(usb3_pcd, usbd_ep);
	}

	dwc_usb3_set_address(usb3_pcd, 0);

	usb3_pcd->remote_wakeup_enable = 0;
	usb3_pcd->ltm_enable = 0;
}

/*
 * This interrupt occurs when a Connect Done is detected.
 * Read the device status register and set the device speed in the data
 * structure. Set up EP0 to receive SETUP packets.
 */
#define DWC_USB_SPEED_SUPER	USB_SPEED_SUPER
#define DWC_USB_SPEED_HIGH	USB_SPEED_HIGH
#define DWC_USB_SPEED_FULL	USB_SPEED_FULL
#define DWC_USB_SPEED_LOW	USB_SPEED_LOW
void
dwc_usb3_handle_connect_done_intr(usb_dwc_pcd_t *usb3_pcd)
{
	volatile dwc_usb3_device_t *usb_dev = usb3_pcd->usb3_dev;
	usb_dwc_pcd_ep_t *usb3_ep0 = usb3_pcd->ep0;
	int speeds;
	uint32_t temp;
	uint32_t udiepcfg0, udoepcfg0, udiepcfg1, udoepcfg1;
	usb_dwc_dev_ep_regs_t *usb_ep_reg = NULL;

	if ((!usb_dev->core_params->hibernate) ||
	    ((usb_dev->hwparams1 & USB_DWC_HWP1_EN_PWROPT_BITS) !=
	    (USB_DWC_EN_PWROPT_HIBERNATION << USB_DWC_HWP1_EN_PWROPT_SHIFT)))
		dwc3_pi_debug("CONNECT\n");

	usb3_ep0->dwc_ep.stopped = 0;
	speeds = dwc_usb3_get_device_speed(usb3_pcd);
	usb3_pcd->speed = speeds;

	/* If LPM enable was requested */
	if (usb_dev->core_params->lpmctl) {
		temp = dwc_rd32(usb_dev, &usb3_pcd->dev_global_regs->dctl);

		/* HIRD threshold must be set to 0 when in SS */
		temp &= ~USB_DWC_DCTL_HIRD_THR_BITS;

		/* If not SS */
		if (speeds != USB_SPEED_SUPER) {
			/* If hibernation enabled and non-default threshold */
			if (usb_dev->core_params->hibernate &&
			    ((usb_dev->hwparams1 & USB_DWC_HWP1_EN_PWROPT_BITS) ==
			    (USB_DWC_EN_PWROPT_HIBERNATION << USB_DWC_HWP1_EN_PWROPT_SHIFT)) &&
			    usb_dev->hird_thresh) {
				/* Set requested threshold value */
				temp |= usb_dev->hird_thresh << USB_DWC_DCTL_HIRD_THR_SHIFT &
				    USB_DWC_DCTL_HIRD_THR_BITS;
			} else {
				/* Set default threshold value */
				temp |= 0x1c << USB_DWC_DCTL_HIRD_THR_SHIFT;
			}
		}

		dwc_wr32(usb_dev, &usb3_pcd->dev_global_regs->dctl, temp);
	}

	/* Set the MPS of EP0 based on the connection speed */
	switch (speeds) {
	case DWC_USB_SPEED_SUPER:
		usb3_pcd->ep0->dwc_ep.maxpacket = 512;
		break;

	case DWC_USB_SPEED_HIGH:
	case DWC_USB_SPEED_FULL:
		usb3_pcd->ep0->dwc_ep.maxpacket = 64;
		break;

	case DWC_USB_SPEED_LOW:
		usb3_pcd->ep0->dwc_ep.maxpacket = 8;
		break;

	default:
		break;
	}

	udiepcfg0 = USB_DWC_USB3_EP_TYPE_CONTROL << USB_DWC_EPCFG0_EPTYPE_SHIFT;
	udiepcfg0 |= USB_DWC_CFG_ACTION_MODIFY << USB_DWC_EPCFG0_CFG_ACTION_SHIFT;
	udiepcfg1 = USB_DWC_EPCFG1_XFER_CMPL_BIT | USB_DWC_EPCFG1_XFER_IN_PROG_BIT |
	    USB_DWC_EPCFG1_XFER_NRDY_BIT | USB_DWC_EPCFG1_EP_DIR_BIT;

	udoepcfg0 = USB_DWC_USB3_EP_TYPE_CONTROL << USB_DWC_EPCFG0_EPTYPE_SHIFT;
	udoepcfg0 |= USB_DWC_CFG_ACTION_MODIFY << USB_DWC_EPCFG0_CFG_ACTION_SHIFT;
	udoepcfg1 = USB_DWC_EPCFG1_XFER_CMPL_BIT | USB_DWC_EPCFG1_XFER_IN_PROG_BIT |
	    USB_DWC_EPCFG1_XFER_NRDY_BIT;

	udiepcfg0 |= usb3_pcd->ep0->dwc_ep.maxpacket << USB_DWC_EPCFG0_MPS_SHIFT;
	udoepcfg0 |= usb3_pcd->ep0->dwc_ep.maxpacket << USB_DWC_EPCFG0_MPS_SHIFT;
	udiepcfg0 |= usb3_ep0->dwc_ep.tx_fifo_num << USB_DWC_EPCFG0_TXFNUM_SHIFT;

	dwc_usb3_dis_usb2_suspend(usb3_pcd);


	usb_ep_reg = &usb3_pcd->out_ep_regs[0];
	(void)dwc_usb3_dep_cfg(usb3_pcd, usb_ep_reg, udoepcfg0, udoepcfg1, 0);


	usb_ep_reg = &usb3_pcd->in_ep_regs[0];
	(void)dwc_usb3_dep_cfg(usb3_pcd, usb_ep_reg, udiepcfg0, udiepcfg1, 0);

	dwc_usb3_ena_usb2_suspend(usb3_pcd);

	if (usb3_pcd->state == USB_DWC_STATE_UNCONNECTED)
		usb3_pcd->state = USB_DWC_STATE_DEFAULT;

	/* Inform the gadget of the connection and the speed */
	(void)dwc_usb3_gadget_connect(usb3_pcd, speeds);
	if (usb_dev->hiber_wait_connect) {
		dwc3_pi_debug("Hibernation wait connect\n");
		usb_dev->hiber_wait_connect = 0;
	}
}

/*
 * This interrupt indicates that the USB link state has changed.
 */
static void
handle_link_status_change_intr(usb_dwc_pcd_t *usb3_pcd)
{
	int status;
	int speeds;

	status = dwc_usb3_pcd_get_link_state(usb3_pcd);
	speeds = dwc_usb3_get_device_speed(usb3_pcd);
	usb3_pcd->speed = speeds;
	dwc3_pi_debug("LINK state=%d speed=%d\n", status, speeds);

	switch (status) {
	case USB_DWC_LINK_STATE_U0:
		if (usb3_pcd->usb3_dev->hiber_wait_u0) {
			usb3_pcd->speed = speeds;
			if (usb3_pcd->remote_wakeup_enable)
				dwc_usb3_pcd_remote_wake(usb3_pcd, 0);
			usb3_pcd->usb3_dev->hiber_wait_u0 = 0;
		}

		if (usb3_pcd->link_state == USB_DWC_LINK_STATE_U3) {
			dwc3_pi_debug(
			    "Enabling function remote wake\n");
			usb3_pcd->wkup_rdy = 1;
		} else {
			usb3_pcd->wkup_rdy = 0;
		}

		usb3_pcd->link_state = status;
		break;

	case USB_DWC_LINK_STATE_U3:
		if (usb3_pcd->link_state != USB_DWC_LINK_STATE_U3)
			(void)dwc_usb3_gadget_suspend(usb3_pcd);
		/* fall-through */
	default:
		usb3_pcd->link_state = status;
		usb3_pcd->wkup_rdy = 0;
		break;
	}
}

static void
handle_wakeup_detected_intr(usb_dwc_pcd_t *usb3_pcd)
{
	int state;

	dwc3_pi_debug(
	    "++Resume or Remote Wakeup Detected Interrupt++\n");
	dwc3_pi_debug("DSTS=0x%01x\n",
	    dwc_rd32(usb3_pcd->usb3_dev, &usb3_pcd->dev_global_regs->dsts));
	state = dwc_usb3_pcd_get_link_state(usb3_pcd);

	if (usb3_pcd->link_state != USB_DWC_LINK_STATE_U0)
		(void)dwc_usb3_gadget_resume(usb3_pcd);
	usb3_pcd->link_state = state;
}

/*
 * This interrupt indicates that a U3/L2-L1 Suspend event has occurred.
 */
static void
handle_u3_l2l1_susp_intr(usb_dwc_pcd_t *usb3_pcd)
{
	int status;

	if (usb3_pcd->usb3_dev->snpsid < 0x5533230a)
		return;

	status = dwc_usb3_pcd_get_link_state(usb3_pcd);
	dwc3_pi_debug("LINK state=%d\n", status);

	switch (status) {
	case USB_DWC_LINK_STATE_U0:
		/* If transitioning from 3->0 */
		if (usb3_pcd->link_state == USB_DWC_LINK_STATE_U3) {
			dwc3_pi_debug(
			    "Enabling function remote wake\n");
			usb3_pcd->wkup_rdy = 1;
		} else {
			usb3_pcd->wkup_rdy = 0;
		}

		usb3_pcd->link_state = status;
		break;

	case USB_DWC_LINK_STATE_U3:
		/* If transitioning to 3 */
		if (usb3_pcd->link_state != USB_DWC_LINK_STATE_U3)
			(void)dwc_usb3_gadget_suspend(usb3_pcd);
		/* fall-through */
	default:
		usb3_pcd->link_state = status;
		usb3_pcd->wkup_rdy = 0;
		break;
	}
}

/*
 * This routine handles the SOF Interrupts. At this time the SOF Interrupt
 * is disabled.
 */
static void
handle_sof_intr(usb_dwc_pcd_t *usb3_pcd)
{
	dwc3_pi_debug("SOF\n");
}

/*
 * This interrupt indicates that an EP has a pending interrupt.
 */
void
dwc_usb3_handle_ep_intr(usb_dwc_pcd_t *usb3_pcd, uint32_t physep, uint32_t event)
{
	usb_dwc_pcd_ep_t *usbd_ep = NULL;
	uint32_t ep_num, in;
	char *str = NULL;

	in = physep & 1;
	ep_num = physep >> 1 & 0xf;

	if (in) {
		usbd_ep = dwc_usb3_get_in_ep(usb3_pcd, ep_num);
		str = "IN";
	} else {
		usbd_ep = dwc_usb3_get_out_ep(usb3_pcd, ep_num);
		str = "OUT";
	}

	dwc3_pi_debug("%s EP intr num:%u\n", str, ep_num);

	switch (event & USB_DWC_DEPEVT_INTTYPE_BITS) {
	case USB_DWC_DEPEVT_XFER_CMPL << USB_DWC_DEPEVT_INTTYPE_SHIFT:
		usbd_ep->dwc_ep.xfer_started = 0;
		dwc3_pi_debug("%s %u\n", __FUNCTION__, ep_num);

		if (usbd_ep->dwc_ep.type != USB_EP_ATTR_XFER_ISOC) {
			if (ep_num == 0)
				dwc_usb3_handle_ep0_xfer(usb3_pcd, event);
			else
				dwc_usb3_complete_request(usb3_pcd, usbd_ep, event);
		} else {
			dwc3_pi_debug("[EP%u] %s xfer complete for ISOC EP!\n",
			    ep_num, str);
		}
		break;

	case USB_DWC_DEPEVT_XFER_IN_PROG << USB_DWC_DEPEVT_INTTYPE_SHIFT:
		if (usbd_ep->dwc_ep.type == USB_EP_ATTR_XFER_ISOC) {
			dwc_usb3_complete_request(usb3_pcd, usbd_ep, event);
		} else {
			if (ep_num == 0)
				dwc_usb3_handle_ep0_xfer(usb3_pcd, event);
			else
				dwc_usb3_complete_request(usb3_pcd, usbd_ep, event);
		}
		break;

	case USB_DWC_DEPEVT_XFER_NRDY << USB_DWC_DEPEVT_INTTYPE_SHIFT:
		dwc3_pi_debug("[EP%u] %s xfer not ready\n",
		    ep_num, str);

		if (ep_num == 0) {
			dwc3_pi_debug("[EP%u] pcd->ep0state:%d\n",  ep_num, usb3_pcd->ep0state);
			switch (usb3_pcd->ep0state) {
			case USB_EP0_IN_WAIT_GADGET:
			case USB_EP0_IN_WAIT_NRDY:
				if (in)
					dwc_usb3_handle_ep0_xfer(usb3_pcd, event);
				break;
			case USB_EP0_OUT_WAIT_GADGET:
			case USB_EP0_OUT_WAIT_NRDY:
				if (!in)
					dwc_usb3_handle_ep0_xfer(usb3_pcd, event);
				break;
			default:
				break;
			}
		} else if (usbd_ep->dwc_ep.type == USB_EP_ATTR_XFER_ISOC) {
			dwc3_pi_debug("[EP%u] %s xfer not ready for ISOC EP!\n",
			    ep_num, str);
			if (!usbd_ep->dwc_ep.xfer_started)
				dwc_usb3_gadget_isoc_ep_start(usb3_pcd, usbd_ep, event);
		}
		break;

	case USB_DWC_DEPEVT_FIFOXRUN << USB_DWC_DEPEVT_INTTYPE_SHIFT:
		dwc3_pi_debug("[EP%u] %s FIFO Underrun Error!\n",
		    ep_num, str);
		break;

	case USB_DWC_DEPEVT_EPCMD_CMPL << USB_DWC_DEPEVT_INTTYPE_SHIFT:
		dwc3_pi_debug("[EP%u] %s Command Complete!\n",
		    ep_num, str);
		break;

	default:
		dwc3_pi_debug("[EP%u] %s Unknown event!\n",
		    ep_num, str);
		break;
	}
	(void)str; /* this is for clearing warning */
}

/*
 * PCD interrupt handler.
 *
 * The PCD handles the device interrupts. Many conditions can cause a
 * device interrupt. When an interrupt occurs, the device interrupt
 * service routine determines the cause of the interrupt and
 * dispatches handling to the appropriate routine.
 */
int
dwc_usb3_handle_dev_intr(usb_dwc_pcd_t *usb3_pcd, uint32_t event)
{
	uint32_t udint = event >> USB_DWC_DEVT_SHIFT & USB_DWC_DEVT_BITS >> USB_DWC_DEVT_SHIFT;
	int temp;
	int ret = 0;

	temp = usb3_pcd->usb3_dev->hibernate;
	if ((temp >= DWC_HIBER_SLEEPING) && (temp != DWC_HIBER_WAIT_U0) &&
	    (temp != DWC_HIBER_SS_DIS_QUIRK)) {
		dwc3_pi_debug("Device: got event 0x%08x while hibernating\n", event);
		return (0);
	}

	UsbPhySetDeviceState();

	switch (udint) {
	case USB_DWC_DEVT_DISCONN:
		dwc3_pi_debug("Disconnect\n");
		ret = handle_disconnect_intr(usb3_pcd);
		UsbPhyClearDeviceState();
		break;

	case USB_DWC_DEVT_USBRESET:
		dwc3_pi_debug("USB Reset\n");
		handle_usb_reset_intr(usb3_pcd);
		UsbPhyClearDeviceState();
		break;

	case USB_DWC_DEVT_CONNDONE:
		dwc3_pi_debug("Connect Done\n");
		dwc_usb3_handle_connect_done_intr(usb3_pcd);
		break;

	case USB_DWC_DEVT_ULST_CHNG:
		dwc3_pi_debug("Link Status Change\n");
		handle_link_status_change_intr(usb3_pcd);
		break;

	case USB_DWC_DEVT_WKUP:
		dwc3_pi_debug("Wakeup\n");
		handle_wakeup_detected_intr(usb3_pcd);
		break;

	case USB_DWC_DEVT_HIBER_REQ:
		dwc3_pi_debug("Hibernation Request\n");
		ret = handle_hiber_req_intr(usb3_pcd, event);
		break;

	case USB_DWC_DEVT_U3_L2L1_SUSP:
		dwc3_pi_debug("U3/L2-L1 Suspend Event\n");
		handle_u3_l2l1_susp_intr(usb3_pcd);
		UsbPhyClearDeviceState();
		break;

	case USB_DWC_DEVT_SOF:
		dwc3_pi_debug("Start of Frame\n");
		handle_sof_intr(usb3_pcd);
		break;

	case USB_DWC_DEVT_ERRATICERR:
		dwc3_pi_debug("Erratic Error\n");
		break;

	case USB_DWC_DEVT_CMD_CMPL:
		dwc3_pi_debug("Command Complete\n");
		break;

	case USB_DWC_DEVT_OVERFLOW:
		dwc3_pi_debug("Overflow\n");
		break;

	default:
		dwc3_pi_debug("Unknown event!\n");
		break;
	}

	return (ret);
}
