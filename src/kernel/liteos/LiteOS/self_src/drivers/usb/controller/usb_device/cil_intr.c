/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2014-2021. All rights reserved.
 * Description: LiteOS USB Driver CIL Implementation
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

#ifdef DWC3_CIL_INTR_DEBUG
#define dwc3_ci_debug(x...) dprintf(x)
#else
#define dwc3_ci_debug(x...) do {} while (0)
#endif

/*
 * This routine enables the Event Buffer interrupt.
 */
static void
dwc_ena_eventbuf_intr(dwc_usb3_device_t *dev, int bufno)
{
	uint32_t eventsize;

	eventsize = dwc_rd32(dev, &dev->core_global_regs->geventbuf[bufno].geventsiz);
	eventsize &= ~USB_DWC_EVENTSIZ_INT_MSK_BIT;
	dwc_wr32(dev, &dev->core_global_regs->geventbuf[bufno].geventsiz, eventsize);
}

/*
 * This routine disables the Event Buffer interrupt.
 */
static void
dwc_dis_eventbuf_intr(dwc_usb3_device_t *dev, int bufno)
{
	uint32_t eventsize;

	eventsize = dwc_rd32(dev, &dev->core_global_regs->geventbuf[bufno].geventsiz);
	eventsize |= USB_DWC_EVENTSIZ_INT_MSK_BIT;
	dwc_wr32(dev, &dev->core_global_regs->geventbuf[bufno].geventsiz, eventsize);
}

/*
 * This routine disables the Event Buffer interrupt and flushes any pending
 * events from the buffer.
 */
void
dwc_usb3_dis_flush_eventbuf_intr(dwc_usb3_device_t *pdev, int bufno)
{
	uint32_t count;

	if (pdev->hibernate >= DWC_HIBER_SLEEPING)
		return;

	dwc_dis_eventbuf_intr(pdev, bufno);
	count = dwc_rd32(pdev, &pdev->core_global_regs->geventbuf[bufno].geventcnt);
	dwc_wr32(pdev, &pdev->core_global_regs->geventbuf[bufno].geventcnt, count);
}

/*
 * This routine reads the current Event Buffer count.
 */
static int
dwc_get_eventbuf_count(const dwc_usb3_device_t *dev, int bufno)
{
	uint32_t count;

	count = dwc_rd32(dev, &dev->core_global_regs->geventbuf[bufno].geventcnt);
	return (count & USB_DWC_EVENTCNT_CNT_BITS);
}

int
dwc_usb3_get_eventbuf_count(dwc_usb3_device_t *dev)
{
	int count;
	uint32_t flags;

	spin_lock_irqsave(&dev->pcd.lock, flags);
	count = dwc_get_eventbuf_count(dev, USB_GEVENT_BUF_NUM);
	spin_unlock_irqrestore(&dev->pcd.lock, flags);

	return (count);
}

/*
 * This routine writes the Event Buffer count.
 */
static void
dwc_update_eventbuf_count(dwc_usb3_device_t *dev, int bufno, int count)
{
	dwc_wr32(dev, &dev->core_global_regs->geventbuf[bufno].geventcnt, count);
}

void
dwc_usb3_update_eventbuf_count(dwc_usb3_device_t *dev, int count)
{
	uint32_t flags;

	if (!count) {
		return;
	}

	spin_lock_irqsave(&dev->pcd.lock, flags);
	dwc_update_eventbuf_count(dev, USB_GEVENT_BUF_NUM, count);
	spin_unlock_irqrestore(&dev->pcd.lock, flags);
}

/*
 * This routine fetches the next event from the Event Buffer.
 */
static uint32_t
dwc_get_eventbuf_event(dwc_usb3_device_t *pdev, int bufno, int size)
{
	uint32_t ret;

	ret = *pdev->event_ptr[bufno]++;
	if (pdev->event_ptr[bufno] >= pdev->event_buf[bufno] + size)
		pdev->event_ptr[bufno] = pdev->event_buf[bufno];
	return (ret);
}

/*
 * This routine initializes an Event Buffer.
 */
void
dwc_usb3_init_eventbuf(dwc_usb3_device_t *pdev, int bufno,
    uint32_t *addr, const uint32_t sz, usb_dwc_dma_t dma_addr)
{
	dma_addr += USB_DMA_OFFSET;
	dwc_wr32(pdev, &pdev->core_global_regs->geventbuf[bufno].geventadr_lo, dma_addr & 0xffffffffU);
#ifdef DWC_USB3_64B_ARCH
	dwc_wr32(pdev, &pdev->core_global_regs->geventbuf[bufno].geventadr_hi, dma_addr >> 32U & 0xffffffffU);
#else
	dwc_wr32(pdev, &pdev->core_global_regs->geventbuf[bufno].geventadr_hi, 0);
#endif
	dwc_wr32(pdev, &pdev->core_global_regs->geventbuf[bufno].geventsiz, sz << 2);
	dwc_wr32(pdev, &pdev->core_global_regs->geventbuf[bufno].geventcnt, 0);
}

/*
 * This routine initializes the common interrupts.
 */
void
dwc_usb3_enable_common_interrupts(dwc_usb3_device_t *pdev)
{
	dwc_usb3_dis_flush_eventbuf_intr(pdev, USB_GEVENT_BUF_NUM);

	dwc_ena_eventbuf_intr(pdev, USB_GEVENT_BUF_NUM);
}

/*
 * This routine enables the Device mode interrupts.
 */
void
dwc_usb3_enable_device_interrupts(dwc_usb3_device_t *pdev)
{
	uint32_t devten;

	dwc3_ci_debug("%s()\n", __func__);

	dwc_usb3_enable_common_interrupts(pdev);

	devten = USB_DWC_DEVTEN_DISCONN_BIT | USB_DWC_DEVTEN_CONNDONE_BIT |
	    USB_DWC_DEVTEN_USBRESET_BIT | USB_DWC_DEVTEN_HIBER_REQ_BIT |
	    USB_DWC_DEVTEN_WKUP_BIT | USB_DWC_DEVTEN_ULST_CHNG_BIT;

	/* Enable device interrupts */
	dwc_wr32(pdev, &pdev->pcd.dev_global_regs->devten, devten);

	dwc3_ci_debug("%s() devten=%0x\n", __func__,
	    dwc_rd32(pdev, &pdev->pcd.dev_global_regs->devten));
}

/*
 * This routine handles all interrupt events. It is called by the
 * dwc_usb3_irq() interrupt handler routine, and by the enter_hibernation()
 * routine after clearing the Run/Stop bit and waiting for the Halted bit to
 * be set.
 */
int
dwc_usb3_handle_event(dwc_usb3_device_t *pdev)
{
	usb_dwc_pcd_t *pcd = &pdev->pcd;
	uint32_t event;
	uint32_t cnt, intr, physep, j;
	int retval = 0;
	static int msg_count = 0;

	cnt = dwc_get_eventbuf_count(pdev, USB_GEVENT_BUF_NUM);
	if (cnt)
		dwc3_ci_debug("Interrupt count %u\n", cnt);

	/* Count is too large. */
	if (((cnt & USB_DWC_EVENTCNT_CNT_BITS) ==
	    (0xffffffff & USB_DWC_EVENTCNT_CNT_BITS)) ||
	    (cnt >= DWC_EVENT_BUF_SIZE * 4)) {
		if (msg_count > 100) {
			msg_count = 100;
			dwc3_ci_debug("Too many bad events!!\n");
		} else {
			msg_count++;
			dwc3_ci_debug("Bad event count 0x%01x in dwc_usb3_irq() !!\n", cnt);
		}

		dwc_dis_eventbuf_intr(pdev, USB_GEVENT_BUF_NUM);
		dwc_update_eventbuf_count(pdev, USB_GEVENT_BUF_NUM, cnt);
		cnt = 0;
	}

	if (!cnt)
		goto done;
	retval = 1;

	for (j = 0; j < cnt; j += 4) {
		event = dwc_get_eventbuf_event(pdev, 0, DWC_EVENT_BUF_SIZE);
		dwc_update_eventbuf_count(pdev, USB_GEVENT_BUF_NUM, 4);
		if (event == 0) {
			dwc3_ci_debug("## Null event! ##\n");
			continue;
		}

		/* The event mantissa is 1. */
		if (event & USB_DWC_EVENT_NON_EP_BIT) {
			intr = event & USB_DWC_EVENT_INTTYPE_BITS;
			if (intr == USB_DWC_EVENT_DEV_INT << USB_DWC_EVENT_INTTYPE_SHIFT) {
				dwc3_ci_debug("## Device interrupt 0x%08x ##\n", event);
				retval = dwc_usb3_handle_dev_intr(pcd, event);
				if (retval) {
					retval = 2;
					goto done;
				}
				retval = 1;
			} else {
				dwc3_ci_debug("## Core interrupt 0x%08x ##\n", event);
			}
		} else {
			physep = event >> USB_DWC_DEPEVT_EPNUM_SHIFT &
			    USB_DWC_DEPEVT_EPNUM_BITS >> USB_DWC_DEPEVT_EPNUM_SHIFT;
			dwc3_ci_debug("## Physical EP%u interrupt 0x%08x ##\n", physep, event);
			dwc_usb3_handle_ep_intr(pcd, physep, event);
		}
	}
done:
	return (retval);
}

/*
 * This is the common interrupt handler routine.
 */
int
dwc_usb3_irq(dwc_usb3_device_t *pdev, int irq)
{
	int state, temp, retval;

	if (!pdev->cmn_irq_installed)
		return (0);

	state = pdev->hibernate;

	if ((state != DWC_HIBER_SLEEPING) && pdev->hiber_wait_u0) {
		/* Handle waiting for U0 after requesting link state RECOVERY,
		 * because we don't have the link state change event enabled.
		 * We also do this in dwc_wait_pme_thread() in case an event
		 * doesn't come.
		 */
		temp = dwc_usb3_pcd_get_link_state(&pdev->pcd);
		pdev->pcd.link_state = temp;
		dwc3_ci_debug("intr WAIT_U0 state=%d\n", temp);

		if (temp == USB_DWC_LINK_STATE_U0) {
			dwc3_ci_debug("intr WAIT_U0 setting speed\n");
			pdev->pcd.speed = dwc_usb3_get_device_speed(&pdev->pcd);
			if (pdev->pcd.remote_wakeup_enable)
				dwc_usb3_pcd_remote_wake(&pdev->pcd, 0);
			pdev->hiber_wait_u0 = 0;
		}
	}

	if (state >= DWC_HIBER_SLEEPING) {
		if ((state == DWC_HIBER_WAIT_U0) ||
		    (state == DWC_HIBER_SS_DIS_QUIRK)) {
			pdev->hibernate = DWC_HIBER_AWAKE;
		} else {
			if (pdev->pme_ready) {
				return (1);
			} else {
				if (state != DWC_HIBER_WAIT_LINK_UP)
					dwc3_ci_debug("Intr in hibernate but pme_ready not set\n");
				return (1);
			}
		}
	}

	retval = dwc_usb3_handle_event(pdev);
	if (retval == 2)
		retval = 1;

	return (retval);
}
