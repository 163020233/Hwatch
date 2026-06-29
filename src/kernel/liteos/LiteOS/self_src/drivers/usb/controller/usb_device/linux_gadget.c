/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2014-2021. All rights reserved.
 * Description: LiteOS USB Gadget Driver
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

#include <ctype.h>  /* for isdigit() */
#include <string.h>
#include <stdlib.h> /* for malloc(..) */
#include <los_sys.h>
#include "controller/usb_device/os_defs.h"
#include "controller/usb_device/hw.h"
#include "controller/usb_device/pcd.h"
#include "controller/usb_device/dev.h"
#include "controller/usb_device/os_dev.h"
#include "controller/usb_device/cil.h"
#include "implementation/usb_list.h"
#include "usb_os_adapt.h"
#include USB3_HEADER_PATH
#include "phy/usb_phy.h"

#define dwc3_lg_show(x...) dprintf(x)

#ifdef DWC3_LINUX_GADGET_DEBUG
#define dwc3_lg_debug(x...) dwc3_lg_show(x)
#else
#define dwc3_lg_debug(x...) do {} while (0)
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(_x) (sizeof(_x) / sizeof(_x[0]))
#endif

static const char g_ep_names[USB_DWC_MAX_EPS * 2 - 1][16] = { "ep0", "ep1out", "ep1in", "ep2out", "ep2in",
	"ep3out", "ep3in", "ep4out", "ep4in", "ep5out", "ep5in", "ep6out", "ep6in", "ep7out", "ep7in", "ep8out",
	"ep8in", "ep9out", "ep9in", "ep10out", "ep10in", "ep11out", "ep11in", "ep12out", "ep12in",
	"ep13out", "ep13in", "ep14out", "ep14in", "ep15out", "ep15in" };

static struct gadget_wrapper {
	usb_dwc_pcd_t *pcd;

	char ep_names[USB_DWC_MAX_EPS * 2 - 1][16];

	struct usbdev_s usb_dev;
	struct usbdevclass_driver_s *driver;

	usb_dwc_pcd_req_t ep0_req;

	usb_dwc_pcd_ep_t ep0;
	usb_dwc_pcd_ep_t out_ep[USB_DWC_MAX_EPS - 1];
	usb_dwc_pcd_ep_t in_ep[USB_DWC_MAX_EPS - 1];
} *gadget_wrapper;

static usb_dwc_pcd_req_t *
dwc_usb3_get_pcd_req(struct usbdev_req_s *usb_req)
{
	struct usb_dwc_pcd_req *req = NULL;

	req = (struct usb_dwc_pcd_req *)malloc(sizeof(struct usb_dwc_pcd_req));
	if (req == NULL) {
		usb_err("dwc3 get pcd req failed!\n");
		return (NULL);
	}
	(void)memset_s(req, sizeof(struct usb_dwc_pcd_req), 0, sizeof(struct usb_dwc_pcd_req));
	req->usb_req = usb_req;
	req->usb_req->dma = DWC_DMA_ADDR_INVALID;

	return (req);
}

static void
dwc_usb3_free_pcd_req(usb_dwc_pcd_ep_t *pcd_ep)
{
	usb_dwc_pcd_req_t *pcd_req = NULL;

	while (!LOS_ListEmpty(&pcd_ep->dwc_ep.queue)) {
		pcd_req = LOS_LIST_FIRST_ENTRY(&pcd_ep->dwc_ep.queue, usb_dwc_pcd_req_t, entry);
		LOS_ListDelInit(&pcd_req->entry);
		free(pcd_req);
	}

	return;
}

static int
gadget_setup(usb_dwc_pcd_t *pcd_t, const void *data)
{
	int retvalue = -DWC3_USB_E_NOT_SUPPORTED;
	struct usbdevclass_driver_s *drvr = NULL;

	dwc3_lg_debug("%s()\n", __FUNCTION__);

	if (gadget_wrapper == NULL) {
		dwc3_lg_show("%s null wrapper!\n", __FUNCTION__);
		return (-DWC3_USB_E_INVALID);
	}
	drvr = gadget_wrapper->driver;
	if ((drvr != NULL) && (drvr->ops != NULL) && (drvr->ops->setup != NULL)) {
		spin_unlock((spinlock_t *)&(pcd_t->lock));
		retvalue = drvr->ops->setup(drvr, &gadget_wrapper->usb_dev, (struct usb_ctrlreq_s *)data, NULL, 0);
		spin_lock((spinlock_t *)&(pcd_t->lock));
		if (retvalue == -EOPNOTSUPP) {
			retvalue = -DWC3_USB_E_NOT_SUPPORTED;
		} else if (retvalue < 0) {
			retvalue = -DWC3_USB_E_INVALID;
		}
	}

	return (retvalue);
}
int
dwc_usb3_gadget_connect(usb_dwc_pcd_t *pcd, int speed)
{
	dwc3_lg_debug("%s()\n", __FUNCTION__);

	if (gadget_wrapper == NULL) {
		dwc3_lg_show("%s null wrapper!\n", __FUNCTION__);
		return (-DWC3_USB_E_INVALID);
	}

	gadget_wrapper->usb_dev.speed = speed;
	pcd->ep0->usb_ep.maxpacket = pcd->ep0->dwc_ep.maxpacket;

	return (0);
}

int
dwc_usb3_gadget_disconnect(usb_dwc_pcd_t *pcd_t)
{
	struct usbdevclass_driver_s *drvr = NULL;
	dwc3_lg_debug("%s()\n", __FUNCTION__);

	if (gadget_wrapper == NULL) {
		dwc3_lg_show("%s null wrapper!\n", __FUNCTION__);
		return (-DWC3_USB_E_INVALID);
	}

	drvr = gadget_wrapper->driver;
	if ((drvr != NULL) && (drvr->ops != NULL) && (drvr->ops->disconnect != NULL)) {
		spin_unlock((spinlock_t *)&(pcd_t->lock));
		drvr->ops->disconnect(drvr, &gadget_wrapper->usb_dev);
		spin_lock((spinlock_t *)&(pcd_t->lock));
	}

	return (0);
}

int
dwc_usb3_gadget_setup(usb_dwc_pcd_t *pcd_t, struct usb_ctrlreq_s *ctrl)
{
	int result;

	dwc3_lg_debug("%s()\n", __FUNCTION__);

	result = gadget_setup(pcd_t, ctrl);
	if (result >= 256 + 999) {
		pcd_t->request_config = 1;
	}

	return (result);
}

int
dwc_usb3_gadget_suspend(usb_dwc_pcd_t *pcd_t)
{
	struct usbdevclass_driver_s *drvr = NULL;
	dwc3_lg_debug("%s()\n", __FUNCTION__);

	if (gadget_wrapper == NULL) {
		dwc3_lg_show("%s null wrapper!\n", __FUNCTION__);
		return (-DWC3_USB_E_INVALID);
	}

	drvr = gadget_wrapper->driver;
	if ((drvr != NULL) && (drvr->ops != NULL) && (drvr->ops->suspend != NULL)) {
		spin_unlock((spinlock_t *)&(pcd_t->lock));
		drvr->ops->suspend(drvr, &gadget_wrapper->usb_dev);
		spin_lock((spinlock_t *)&(pcd_t->lock));
	}

	UsbPhySuspend();

	return (0);
}

int
dwc_usb3_gadget_resume(usb_dwc_pcd_t *pcd_t)
{
	struct usbdevclass_driver_s *drvr = NULL;
	dwc3_lg_debug("%s()\n", __FUNCTION__);

	UsbPhyResume();

	if (gadget_wrapper == NULL) {
		dwc3_lg_show("%s null wrapper!\n", __FUNCTION__);
		return (-DWC3_USB_E_INVALID);
	}

	drvr = gadget_wrapper->driver;
	if ((drvr != NULL) && (drvr->ops != NULL) && (drvr->ops->resume != NULL)) {
		spin_unlock((spinlock_t *)&(pcd_t->lock));
		drvr->ops->resume(drvr, &gadget_wrapper->usb_dev);
		spin_lock((spinlock_t *)&(pcd_t->lock));
	}

	return (0);
}

int
dwc_usb3_gadget_complete(usb_dwc_pcd_t *pcd_t, usb_dwc_pcd_ep_t *pcd_ep, usb_dwc_pcd_req_t *pcd_req_t, int status)
{
	struct usbdev_ep_s *usb_ep = &pcd_ep->usb_ep;
	struct usbdev_req_s *usb_req_t = pcd_req_t->usb_req;
	uint32_t actual = pcd_req_t->dwc_req.actual;

	LOS_ListDelInit(&pcd_req_t->entry);
	pcd_req_t->usb_req = NULL;
	free(pcd_req_t);

	if ((pcd_ep != pcd_t->ep0) && (pcd_ep->dwc_ep.type == USB_EP_ATTR_XFER_ISOC) &&
	    (pcd_ep->dwc_ep.is_in) && (usb_req_t->last_flag == 1)) {
		dwc_usb3_endxfer(pcd_t, pcd_ep);
		pcd_ep->dwc_ep.xfer_started = 0;
	}

	if (usb_req_t->callback != NULL) {
		usb_req_t->is_complete = 1;
		usb_req_t->result = status;
		usb_req_t->xfrd = actual;
		spin_unlock((spinlock_t *)&(pcd_t->lock));
		usb_req_t->callback(usb_ep, usb_req_t);
		spin_lock((spinlock_t *)&(pcd_t->lock));
	}

	if (pcd_t->request_pending > 0)
		--pcd_t->request_pending;

	dwc3_lg_debug("\n###%s,%d\n", __FUNCTION__, __LINE__);
	return (0);
}

void
dwc_usb3_gadget_do_setup(usb_dwc_pcd_t *pcd)
{
	dwc_usb3_pcd_do_setup(pcd);
}

static void
fill_req(usb_dwc_pcd_t *pcd, const usb_dwc_pcd_ep_t *ep,
    usb_dwc_pcd_req_t *req, int numbuf, const char **buf,
    const usb_dwc_dma_t *bufdma, const uint32_t *buflen, uint32_t stream, uint32_t req_flags)
{
	int i;

	dwc3_lg_debug("req_flags=0x%1x\n", req_flags);

	req->dwc_req.length = 0;
	req->dwc_req.numbuf = numbuf;

	for (i = 0; i < numbuf; i++) {
		req->dwc_req.buf[i] = (char *)buf[i];
		req->dwc_req.bufdma[i] = (usb_dwc_dma_t)USB_DMA_VA_TO_PA((void *)bufdma[i]);
		req->dwc_req.buflen[i] = buflen[i];
		req->dwc_req.length += buflen[i];
	}

	req->dwc_req.actual = 0;
	req->dwc_req.stream = stream;
	req->dwc_req.flags = req_flags;

	if ((ep == pcd->ep0) && ep->dwc_ep.is_in) {
		dwc3_lg_debug("%u-OUT ZLP\n", ep->dwc_ep.num);
	}
}

int
dwc_usb3_dev_speed_get(void)
{
	if (gadget_wrapper == NULL) {
		dwc3_lg_show(USB3_DWC "%s null wrapper!\n", __FUNCTION__);
		return (-EINVAL);
	}

	if (gadget_wrapper->usb_dev.speed == USB_SPEED_UNKNOWN) {
		dwc3_lg_show(USB3_DWC "%s, bogus device state!\n", __FUNCTION__);
		return (-ESHUTDOWN);
	}

	return ((int)gadget_wrapper->usb_dev.speed);
}

int
dwc_usb3_dev_maxpacket_get(const struct usbdev_ep_s *usb_ep)
{
	usb_dwc_pcd_ep_t *pcd_ep = NULL;

	if (usb_ep == NULL) {
		dwc3_lg_show(USB3_DWC "%s, bad ep\n", __FUNCTION__);
		return (-EINVAL);
	}
	pcd_ep = dwc_usb3_get_pcd_ep(usb_ep);
	return ((int)pcd_ep->dwc_ep.maxpacket);
}

static int
ep_enable(struct usbdev_ep_s *usb_ep, const struct usb_epdesc_s *usb_desc, bool last)
{
	usb_dwc_pcd_ep_t *pcd_ep_t = NULL;
	usb_dwc_pcd_t *pcd = NULL;
	usb_dwc_dma_desc_t *desc = NULL;
	int ep_type, num_trbs, linked;
	int retvalue;
	uint32_t maxpacket;
	uint32_t flag;
	struct usb_ss_comp_epdesc_s ep_comp = {0};
	uint32_t maxstreams = 0;
	uint32_t mult = 0;
	uint32_t maxburst = 0;

	(void)last;

	if (gadget_wrapper == NULL) {
		dwc3_lg_show(USB3_DWC "%s null wrapper!\n", __FUNCTION__);
		return (-EINVAL);
	}
	if (usb_ep == NULL) {
		dwc3_lg_show(USB3_DWC"%s, bad ep!\n", __FUNCTION__);
		return (-EINVAL);
	}

	if ((usb_desc == NULL) || (usb_desc->type != USB_DESC_TYPE_ENDPOINT)) {
		dwc3_lg_show(USB3_DWC"%s, bad ep or descriptor %u!\n",
		    __FUNCTION__, usb_desc ? usb_desc->type : 0);
		return (-EINVAL);
	}

	if (usb_ep == &gadget_wrapper->ep0.usb_ep) {
		dwc3_lg_show(USB3_DWC "%s called for EP0!\n", __FUNCTION__);
		return (-EINVAL);
	}

	if (USB_GETW(usb_desc->mxpacketsize) == 0) {
		dwc3_lg_show(USB3_DWC "%s, zero mxpacketsize!\n", __FUNCTION__);
		return (-ERANGE);
	}

	if ((gadget_wrapper->driver == NULL) || (gadget_wrapper->usb_dev.speed == USB_SPEED_UNKNOWN)) {
		dwc3_lg_show(USB3_DWC "%s, bogus device state!\n", __FUNCTION__);
		return (-ESHUTDOWN);
	}

	pcd = gadget_wrapper->pcd;
	pcd_ep_t = dwc_usb3_get_pcd_ep(usb_ep);

	if (pcd_ep_t->dwc_ep.phys >= USB_DWC_MAX_PHYS_EP) {
		dwc3_lg_show("%s, bad phys EP # %u!\n", __FUNCTION__, pcd_ep_t->dwc_ep.phys);
		return (-ERANGE);
	}

	dwc_usb3_pcd_trb_free(pcd_ep_t);

	ep_type = usb_desc->attr & USB_EP_ATTR_XFERTYPE_MASK;
	linked = 0;

	switch (ep_type) {
	case USB_EP_ATTR_XFER_INT:
		num_trbs = DWC_USB3_NUM_INTR_TRBS;
		break;
	case USB_EP_ATTR_XFER_ISOC:
		num_trbs = DWC_USB3_NUM_ISOC_TRBS;
		linked = 1;
		break;
	default:
		num_trbs = DWC_USB3_NUM_BULK_TRBS;
		break;
	}

	desc = dwc_usb3_pcd_trb_alloc(pcd_ep_t, num_trbs, ep_type, usb_desc->interval - 1, linked, NULL);
	if (desc == NULL) {
		return (-ENOMEM);
	}

	maxpacket = USB_GETW(usb_desc->mxpacketsize);

	spin_lock_irqsave((spinlock_t *)&(pcd->lock), flag);

	if (gadget_wrapper->usb_dev.speed == USB_SPEED_SUPER) {
		if (usb_ep->comp_desc) {
			if (ep_type == USB_EP_ATTR_XFER_ISOC)
				mult = usb_ep->comp_desc->attr;
			maxburst = usb_ep->comp_desc->maxburst;
			if (ep_type == USB_EP_ATTR_XFER_BULK)
				maxstreams = usb_ep->comp_desc->attr;
		}
	} else {
		if (ep_type == USB_EP_ATTR_XFER_ISOC)
			mult = maxpacket >> 11 & 3;
	}

	if ((mult > 2) || (maxburst > 15) || (maxstreams > 16)) {
		spin_unlock_irqrestore((spinlock_t *)&(pcd->lock), flag);
		dwc_usb3_pcd_trb_free(pcd_ep_t);
		return (-EINVAL);
	}

	ep_comp.attr = USB_SSE_SET_MAX_STREAMS(0, maxstreams);
	ep_comp.attr = USB_SSE_SET_MAX_PACKET_NUM(ep_comp.attr, mult);
	ep_comp.maxburst = maxburst;

	retvalue = dwc_usb3_pcd_ep_enable(pcd, pcd_ep_t, (struct usb_epdesc_s *)usb_desc, &ep_comp);
	if (!retvalue)
		usb_ep->maxpacket = maxpacket;

	if (gadget_wrapper->usb_dev.speed == USB_SPEED_HIGH) {
		if (usb_ep->maxpacket == 5120)
			usb_ep->maxpacket = 3072;
		else if (usb_ep->maxpacket == 3072)
			usb_ep->maxpacket = 2048;
	}

	spin_unlock_irqrestore((spinlock_t *)&(pcd->lock), flag);

	if (retvalue) {
		dwc_usb3_pcd_trb_free(pcd_ep_t);
		retvalue = -ENOMEM;
	}

	return (retvalue);
}

static int
ep_disable(struct usbdev_ep_s *usb_ep)
{
	int retval;
	usb_dwc_pcd_ep_t *pcd_ep = NULL;
	usb_dwc_pcd_t *pcd = NULL;
	uint32_t flags;

	if (gadget_wrapper == NULL) {
		dwc3_lg_show(USB3_DWC "%s null wrapper!\n", __FUNCTION__);
		return (-EINVAL);
	}

	if (usb_ep == NULL) {
		dwc3_lg_show(USB3_DWC "EP not enabled!\n");
		return (-EINVAL);
	}

	pcd = gadget_wrapper->pcd;

	spin_lock_irqsave((spinlock_t *)&(pcd->lock), flags);
	pcd_ep = dwc_usb3_get_pcd_ep(usb_ep);
	retval = dwc_usb3_pcd_ep_disable(pcd, pcd_ep);
	spin_unlock_irqrestore((spinlock_t *)&(pcd->lock), flags);

	/* Free any existing TRB allocation for this EP */
	dwc_usb3_pcd_trb_free(pcd_ep);

	return (retval ? -EINVAL : 0);
}

static int
ep_queue(struct usbdev_ep_s *usb_ep, struct usbdev_req_s *usb_req)
{
	int retval = 0;
	int result;
	uint32_t req_flags = 0;
	usb_dwc_pcd_ep_t *pcd_ep = NULL;
	usb_dwc_pcd_req_t *pcd_req = NULL;
	usb_dwc_pcd_t *pcd = NULL;
	uint32_t flags;
	usb_dwc_pcd_req_t *check_req = NULL;
	uint8_t num_buf;
	uint8_t **buf = NULL;
	uint32_t *buflen = NULL;

	if ((gadget_wrapper == NULL)) {
		dwc3_lg_show(USB3_DWC "%s null wrapper!\n", __FUNCTION__);
		result = -EINVAL;
		goto err;
	}

	if ((usb_req == NULL) || (usb_req->callback == NULL) || (usb_req->buf == NULL)) {
		dwc3_lg_show(USB3_DWC "%s, bad params\n", __FUNCTION__);
		result = -EINVAL;
		goto err;
	}

	if (usb_ep == NULL) {
		dwc3_lg_show(USB3_DWC "%s, bad ep\n", __FUNCTION__);
		result = -EINVAL;
		goto err;
	}

	if ((gadget_wrapper->driver == NULL) || (gadget_wrapper->usb_dev.speed == USB_SPEED_UNKNOWN)) {
		dwc3_lg_show(USB3_DWC "%s, gadget.speed=%u, or bogus device state\n",
		    __FUNCTION__, gadget_wrapper->usb_dev.speed);
		result = -ESHUTDOWN;
		goto err;
	}

	if ((gadget_wrapper->pcd == NULL)) {
		dwc3_lg_show(USB3_DWC"%s, gadget_wrapper->pcd is NULL!\n", __FUNCTION__);
		result = -EINVAL;
		goto err;
	}

	pcd_ep = dwc_usb3_get_pcd_ep(usb_ep);
	pcd_req = dwc_usb3_get_pcd_req(usb_req);
	if (pcd_req == NULL) {
		dwc3_lg_show(USB3_DWC"%s, pcd_req is NULL!\n", __FUNCTION__);
		result = -EINVAL;
		goto err;
	}
	pcd = usb_dwc_pcd_ep_to_pcd(pcd_ep);

	usb_req->result = -EINPROGRESS;
	usb_req->xfrd = 0;
	usb_req->is_complete = 0;

	if (usb_req->zero || ((pcd_ep == pcd->ep0) && pcd_ep->dwc_ep.is_in))
		req_flags |= USB_DWC_PCD_REQ_ZERO;

	if ((usb_dwc_pcd_ep_num(pcd_ep) != 0) && (!pcd_ep->dwc_ep.usb_ep_desc)) {
		dwc3_lg_show("%s, bad ep!\n", __FUNCTION__);
		free(pcd_req);
		result = -EINVAL;
		goto err;
	}

	spin_lock_irqsave((spinlock_t *)&(pcd->lock), flags);

	LOS_ListInit(&pcd_req->entry);

	if (usb_req->sg_flags == 0) {
		num_buf = 1;
		buf = &(usb_req->buf);
		buflen = &(usb_req->len);
	} else {
		num_buf = usb_req->sg_buf->num_buf;
		buf = usb_req->sg_buf->buf;
		buflen = usb_req->sg_buf->len;
	}
	fill_req(pcd, pcd_ep, pcd_req, num_buf, (const char **)buf, (usb_dwc_dma_t *)buf,
	    buflen, usb_req->stream_id, req_flags);

	if (!LOS_ListEmpty(&pcd_ep->dwc_ep.queue)) {
		dwc3_lg_debug(" q not empty %u, stopped %u, avail %d, istart %u\n",
		    pcd->request_pending, pcd_ep->dwc_ep.stopped,
		    pcd_ep->dwc_ep.desc_avail, pcd_ep->dwc_ep.xfer_started);

		check_req = LOS_DL_LIST_ENTRY((&pcd_ep->dwc_ep.queue)->pstPrev, usb_dwc_pcd_req_t, entry);

		if ((!pcd_ep->dwc_ep.stopped) && (pcd_ep->dwc_ep.type == USB_EP_ATTR_XFER_ISOC) &&
		    (pcd_ep->dwc_ep.desc_avail > 0) && pcd_ep->dwc_ep.xfer_started &&
		    (check_req->dwc_req.flags & USB_DWC_PCD_REQ_STARTED)) {
				goto do_start;
			}
	} else if (pcd_ep->dwc_ep.stopped || ((pcd_ep != pcd->ep0) && (pcd_ep->dwc_ep.desc_avail <= 0)) ||
	    ((pcd_ep->dwc_ep.type == USB_EP_ATTR_XFER_ISOC) && (!pcd_ep->dwc_ep.xfer_started))) {
		dwc3_lg_debug(" q empty %u, stopped %u, avail %d, istart %u\n",
		    pcd->request_pending, pcd_ep->dwc_ep.stopped,
		    pcd_ep->dwc_ep.desc_avail, pcd_ep->dwc_ep.xfer_started);
	} else {
do_start:
		dwc_usb3_pcd_fill_trbs(pcd, pcd_ep, pcd_req);
		retval = dwc_usb3_pcd_ep_submit_req(pcd, pcd_ep, pcd_req, req_flags);
	}

	if ((pcd_ep->dwc_ep.type == USB_EP_ATTR_XFER_ISOC) && LOS_ListEmpty(&pcd_ep->dwc_ep.queue) &&
	    pcd_ep->dwc_ep.xfer_pending && pcd_ep->dwc_ep.is_in) {
		pcd_ep->dwc_ep.xfer_pending = 0;
		dwc_usb3_endxfer(pcd, pcd_ep);
	}

	dwc3_lg_debug("%s do_start end retval=%d\n", __FUNCTION__, retval);
	if (!retval) {
		LIST_ADD_TAIL(&pcd_req->entry, &pcd_ep->dwc_ep.queue);
		++pcd->request_pending;
	} else {
		free(pcd_req);
	}

	spin_unlock_irqrestore((spinlock_t *)&(pcd->lock), flags);

	if (retval < 0) {
		result = -EINVAL;
		goto err;
	}

	return (0);

err:
	return (result);
}

static int
ep_dequeue(struct usbdev_ep_s *usb_ep, struct usbdev_req_s *usb_req)
{
	usb_dwc_pcd_req_t *pcd_req = NULL;
	usb_dwc_pcd_ep_t *pcd_ep = NULL;
	usb_dwc_pcd_t *pcd = NULL;
	struct LOS_DL_LIST *list_item = NULL;
	uint32_t flags;

	if (gadget_wrapper == NULL) {
		dwc3_lg_debug(USB3_DWC "%s null wrapper!\n", __FUNCTION__);
		return (-EINVAL);
	}

	if ((usb_ep == NULL) || (usb_req == NULL)) {
		dwc3_lg_show(USB3_DWC "%s, bad argument!\n", __FUNCTION__);
		return (-EINVAL);
	}

	if ((gadget_wrapper->driver == NULL) || (gadget_wrapper->usb_dev.speed == USB_SPEED_UNKNOWN)) {
		dwc3_lg_show(USB3_DWC "%s, bogus device state!\n", __FUNCTION__);
		return (-ESHUTDOWN);
	}

	if (gadget_wrapper->pcd == NULL) {
		dwc3_lg_show(USB3_DWC "%s, gadget_wrapper->pcd is NULL!\n", __FUNCTION__);
		return (-EINVAL);
	}

	pcd = gadget_wrapper->pcd;

	spin_lock_irqsave((spinlock_t *)&(pcd->lock), flags);

	pcd_ep = dwc_usb3_get_pcd_ep(usb_ep);

	if ((usb_dwc_pcd_ep_num(pcd_ep) != 0) && (!pcd_ep->dwc_ep.usb_ep_desc)) {
		pcd->usb3_dev->hiber_cnt--;
		spin_unlock_irqrestore((spinlock_t *)&(pcd->lock), flags);
		return (-EINVAL);
	}

	LOS_DL_LIST_FOR_EACH (list_item, &pcd_ep->dwc_ep.queue) {
		pcd_req = LOS_DL_LIST_ENTRY(list_item, usb_dwc_pcd_req_t, entry);
		if (pcd_req->usb_req == usb_req)
			break;
	}

	if (pcd_req == NULL) {
		pcd->usb3_dev->hiber_cnt--;
		spin_unlock_irqrestore((spinlock_t *)&(pcd->lock), flags);
		return (0);
	}

	dwc_usb3_pcd_ep_cancel_req(pcd, pcd_ep, pcd_req, usb_req->stream_id);

	spin_unlock_irqrestore((spinlock_t *)&(pcd->lock), flags);
	return (0);
}

static void
ep_fifo_flush(struct usbdev_ep_s *usb_ep)
{
	usb_dwc_pcd_ep_t *pcd_ep_t = NULL;
	uint32_t fifo_sel;

	if (usb_ep->ep_link == NULL) {
		dwc3_lg_show(USB3_DWC"%s, bad ep!\n", __FUNCTION__);
		return;
	}

	pcd_ep_t = dwc_usb3_get_pcd_ep(usb_ep);
	if (pcd_ep_t->dwc_ep.phys >= USB_DWC_MAX_PHYS_EP) {
		dwc3_lg_show("%s, bad %x phys EP # %u!\n", __FUNCTION__, usb_ep->eplog, pcd_ep_t->dwc_ep.phys);
		return;
	}

	fifo_sel = pcd_ep_t->dwc_ep.tx_fifo_num;

	if (pcd_ep_t->dwc_ep.is_in) {
		fifo_sel |= 1 << 5; /* 1 TX FIFO, 0 RX FIFO */
	}

	(void)dwc_usb3_flush_fifo((usb_dwc_pcd_t *)usb_ep->ep_link, fifo_sel);

	return;
}

void *
dwc_usb3_gadget_alloc_dma(usb_dwc_pcd_ep_t *ep, int size, usb_dwc_dma_t *mem_dma_ret)
{
	void *mem = dwc_usb3_dma_alloc(size);
	if (mem == NULL) {
		return (NULL);
	}
	*mem_dma_ret = (usb_dwc_dma_t)USB_DMA_VA_TO_PA(mem);
	return (mem);
}

void
dwc_usb3_gadget_free_dma(usb_dwc_pcd_ep_t *ep, int size, void *mem, usb_dwc_dma_t mem_dma)
{
	dwc_usb3_dma_free(mem);
}

usb_dwc_pcd_req_t *
dwc_usb3_gadget_get_request(usb_dwc_pcd_t *pcd, usb_dwc_pcd_ep_t *ep_t)
{
	usb_dwc_pcd_req_t *req = NULL;

	if (LOS_ListEmpty(&ep_t->dwc_ep.queue)) {
		dwc3_lg_debug("%s, ep->dwc_ep.queue empty!\n", __FUNCTION__);
		return (NULL);
	}

	req = LOS_LIST_FIRST_ENTRY(&ep_t->dwc_ep.queue, usb_dwc_pcd_req_t, entry);
	return (req);
}

void
dwc_usb3_gadget_start_next_request(usb_dwc_pcd_t *pcd, usb_dwc_pcd_ep_t *ep_t)
{
	usb_dwc_pcd_req_t *req = NULL;
	struct LOS_DL_LIST *list_item_t = NULL;

	dwc3_lg_debug("%s\n", __FUNCTION__);

	if (LOS_ListEmpty(&ep_t->dwc_ep.queue)) {
		dwc3_lg_debug("start_next EP%u-%s: queue empty\n",
		    ep_t->dwc_ep.num, ep_t->dwc_ep.is_in ? "IN" : "OUT");
		return;
	}

	LOS_DL_LIST_FOR_EACH (list_item_t, &ep_t->dwc_ep.queue) {
		req = LOS_DL_LIST_ENTRY(list_item_t, usb_dwc_pcd_req_t, entry);
		if (!(req->dwc_req.flags & USB_DWC_PCD_REQ_STARTED)) {
			if (ep_t->dwc_ep.desc_avail <= 0) {
				dwc3_lg_debug("start_next EP%u-%s: no TRB avail\n",
				    ep_t->dwc_ep.num, ep_t->dwc_ep.is_in ? "IN" : "OUT");
				return;
			}

			dwc3_lg_debug("start_next EP%u-%s: OK\n",
			    ep_t->dwc_ep.num, ep_t->dwc_ep.is_in ? "IN" : "OUT");

			dwc_usb3_pcd_fill_trbs(pcd, ep_t, req);
			dwc_usb3_pcd_ep_start_transfer(pcd, ep_t, req, 0);
			return;
		}
	}

	dwc3_lg_debug("start_next EP%u-%s: all req active\n",
	    ep_t->dwc_ep.num, ep_t->dwc_ep.is_in ? "IN" : "OUT");
}

void
dwc_usb3_gadget_request_nuke(usb_dwc_pcd_t *pcd, usb_dwc_pcd_ep_t *ep_t)
{
	usb_dwc_pcd_req_t *req = NULL;

	ep_t->dwc_ep.stopped = 1;

	while (!LOS_ListEmpty(&ep_t->dwc_ep.queue)) {
		req = LOS_LIST_FIRST_ENTRY(&ep_t->dwc_ep.queue, usb_dwc_pcd_req_t, entry);
		dwc_usb3_pcd_request_done(pcd, ep_t, req, -DWC3_USB_E_SHUTDOWN);
	}

	if (ep_t != pcd->ep0) {
		ep_t->dwc_ep.desc_idx = 0;
		ep_t->dwc_ep.hiber_desc_idx = 0;
	}
}

void
dwc_usb3_gadget_set_ep_not_started(usb_dwc_pcd_t *pcd, usb_dwc_pcd_ep_t *ep)
{
	usb_dwc_pcd_req_t *req = NULL;
	struct LOS_DL_LIST *list_item_t = NULL;

	LOS_DL_LIST_FOR_EACH (list_item_t, &ep->dwc_ep.queue) {
		req = LOS_DL_LIST_ENTRY(list_item_t, usb_dwc_pcd_req_t, entry);
		if (req->dwc_req.flags & USB_DWC_PCD_REQ_STARTED)
			req->dwc_req.flags &= ~USB_DWC_PCD_REQ_STARTED;
	}
}

void
dwc_usb3_gadget_isoc_ep_start(usb_dwc_pcd_t *pcd, usb_dwc_pcd_ep_t *ep_t, uint32_t event)
{
	usb_dwc_pcd_req_t *ep_req = NULL;
	struct LOS_DL_LIST *list_item_t = NULL;

	if (LOS_ListEmpty(&ep_t->dwc_ep.queue)) {
		dwc3_lg_debug("%s, ep->dwc_ep.queue empty!\n", __FUNCTION__);
		ep_t->dwc_ep.xfer_pending = 1;
		return;
	}

	if (ep_t->dwc_ep.desc_avail <= 0) {
		dwc3_lg_debug("EP%u-%s: no TRB avail!\n",
		    ep_t->dwc_ep.num, ep_t->dwc_ep.is_in ? "IN" : "OUT");
		return;
	}

	LOS_DL_LIST_FOR_EACH (list_item_t, &ep_t->dwc_ep.queue) {
		ep_req = LOS_DL_LIST_ENTRY(list_item_t, usb_dwc_pcd_req_t, entry);
		if (ep_req->dwc_req.flags & USB_DWC_PCD_REQ_STARTED) {
			ep_req = NULL;
		} else {
			break;
		}
	}

	if (ep_req == NULL) {
		dwc3_lg_show("EP%u-%s: no requests to start!\n",
		    ep_t->dwc_ep.num, ep_t->dwc_ep.is_in ? "IN" : "OUT");
		return;
	}

	dwc_usb3_pcd_fill_trbs(pcd, ep_t, ep_req);
	dwc_usb3_pcd_ep_start_transfer(pcd, ep_t, ep_req, event);

	while (!LOS_IS_LIST_LAST(list_item_t, &ep_t->dwc_ep.queue)) {
		list_item_t = list_item_t->pstNext;
		ep_req = LOS_DL_LIST_ENTRY(list_item_t, usb_dwc_pcd_req_t, entry);
		if (!(ep_req->dwc_req.flags & USB_DWC_PCD_REQ_STARTED)) {
			if (ep_t->dwc_ep.desc_avail <= 0) {
				dwc3_lg_debug( "start_next EP%u-%s: no TRB avail!\n",
				    ep_t->dwc_ep.num, ep_t->dwc_ep.is_in ? "IN" : "OUT");
				return;
			}

			dwc_usb3_pcd_fill_trbs(pcd, ep_t, ep_req);
			dwc_usb3_pcd_ep_start_transfer(pcd, ep_t, ep_req, 0);
		}
	}
}

static int
ep_set_halt_sub(const struct usbdev_ep_s *usb_ep, int value)
{
	usb_dwc_pcd_ep_t *pcd_ep = NULL;
	usb_dwc_pcd_t *pcd = NULL;
	uint32_t flags;

	if (gadget_wrapper == NULL) {
		dwc3_lg_show(USB3_DWC "%s null wrapper!\n", __FUNCTION__);
		return (-EINVAL);
	}

	if (usb_ep == NULL) {
		dwc3_lg_show(USB3_DWC "%s, bad usb_ep!\n", __FUNCTION__);
		return (-EINVAL);
	}

	pcd = gadget_wrapper->pcd;

	spin_lock_irqsave((spinlock_t *)&(pcd->lock), flags);

	pcd_ep = dwc_usb3_get_pcd_ep(usb_ep);
	dwc3_lg_debug("epnum=%u is_in=%u\n", usb_dwc_pcd_ep_num(pcd_ep), usb_dwc_pcd_ep_is_in(pcd_ep));

	if (((!pcd_ep->dwc_ep.usb_ep_desc) && (usb_dwc_pcd_ep_num(pcd_ep) != 0)) ||
	    (pcd_ep->dwc_ep.usb_ep_desc && (usb_dwc_pcd_ep_type(pcd_ep) == USB_EP_ATTR_XFER_ISOC))) {
		dwc3_lg_show("%s, bad pcd_ep!\n", __FUNCTION__);
		spin_unlock_irqrestore((spinlock_t *)&(pcd->lock), flags);
		return (-EINVAL);
	}

	if (!LOS_ListEmpty(&pcd_ep->dwc_ep.queue)) {
		dwc3_lg_show("%s, Xfer in process!\n", __FUNCTION__);
		spin_unlock_irqrestore((spinlock_t *)&(pcd->lock), flags);
		return (-EAGAIN);
	}

	dwc_usb3_pcd_ep_set_halt(pcd, pcd_ep, value);

	spin_unlock_irqrestore((spinlock_t *)&(pcd->lock), flags);
	return (0);
}

static int
ep_set_halt(struct usbdev_ep_s *ep, bool resume)
{
	return ep_set_halt_sub(ep, resume);
}

static struct usbdev_req_s *
ep_alloc_req(struct usbdev_ep_s *ep)
{
	struct usbdev_req_s *req = NULL;

	(void)ep;
	req = malloc(sizeof(struct usbdev_req_s));
	if (req == NULL) {
		return (NULL);
	}
	(void)memset_s(req, sizeof(struct usbdev_req_s), 0, sizeof(struct usbdev_req_s));
	return (req);
}

static void
ep_free_req(struct usbdev_ep_s *ep, struct usbdev_req_s *req)
{
	(void)ep;
	if (req == NULL) {
		return;
	}
	free(req);
}

static void *
ep_alloc_buff(struct usbdev_ep_s *ep, uint16_t len)
{
	void *buf = NULL;

	(void)ep;
	buf = memalign(USB_CACHE_ALIGN_SIZE, len);
	if (buf == NULL) {
		return (NULL);
	}
	(void)memset_s(buf, len, 0, len);
	return (buf);
}

static void
ep_free_buff(struct usbdev_ep_s *ep, void *buf)
{
	(void)ep;
	if (buf == NULL) {
		return;
	}
	free(buf);
}

static const struct usbdev_epops_s g_epops = {
	.configure		= ep_enable,
	.disable		= ep_disable,
	.allocreq		= ep_alloc_req,
	.freereq		= ep_free_req,
	.allocbuffer	= ep_alloc_buff,
	.freebuffer		= ep_free_buff,
	.submit			= ep_queue,
	.cancel			= ep_dequeue,
	.stall			= ep_set_halt,
	.flush			= ep_fifo_flush,
};

static struct usbdev_ep_s *
usbd_ep_find(usb_dwc_pcd_ep_t *pcd_ep, uint8_t eplog, struct usb_epdesc_s *desc)
{
	uint32_t i;
	usb_dwc_pcd_ep_t *pcd_ep_tmp = NULL;
	struct usbdev_ep_s *ep = NULL;
	struct usbdev_ep_s *valid_ep = NULL;
	int flag = 0;

	for (i = 0; i < (USB_DWC_MAX_EPS - 1); i++) {
		pcd_ep_tmp = &pcd_ep[i];
		ep = &pcd_ep_tmp->usb_ep;
		if (!ep->eplog) {
			break;
		}

		/* Find the specified endpoint. */
		if ((ep->eplog == eplog) && (!ep->used)) {
			if (valid_ep != NULL) {
				valid_ep->used = 0;
			}
			ep->used = 1;
			desc->addr = ep->eplog;
			return (ep);
		}

		/* Flag is set that indicates that an available endpoint has been found. */
		if (ep->used || flag) {
			continue;
		} else {
			ep->used = 1;
			desc->addr = ep->eplog;
			flag = 1;
			valid_ep = ep;
		}
	}

	if (valid_ep != NULL) {
		return (valid_ep);
	}

	return (NULL);
}

struct usbdev_ep_s *
usbd_ep_alloc(struct usbdev_s *dev, uint8_t eplog, struct usb_epdesc_s *desc)
{
	struct usbdev_ep_s *ep = NULL;
	int epphy;

	if (gadget_wrapper == NULL) {
		return (NULL);
	}

	epphy = USB_EPNO(eplog);
	if (epphy >= USB_DWC_MAX_EPS) {
		usb_err("ep num is too bug!\n");
		return (NULL);
	}

	if ((eplog & USB_DIR_MASK) == USB_DIR_OUT) {
		ep = usbd_ep_find(gadget_wrapper->out_ep, eplog, desc);
	} else if ((eplog & USB_DIR_MASK) == USB_DIR_IN) {
		ep = usbd_ep_find(gadget_wrapper->in_ep, eplog, desc);
	}

	if (ep != NULL) {
		return (ep);
	}

	usb_err("The addr=%x ep is invalid!\n", eplog);
	return (NULL);
}

void
usbd_ep_free(struct usbdev_s *dev, struct usbdev_ep_s *ep)
{
	int epphy;
	if (ep == NULL) {
		usb_err("ep is NULL\n");
		return;
	}

	epphy = USB_EPNO(ep->eplog);
	if (epphy >= USB_DWC_MAX_EPS) {
		usb_err("ep num is too big!\n");
		return;
	}

	if (ep->used) {
		ep->used = 0;
		ep->priv = NULL;
		ep->handle_req = NULL;
		ep->comp_desc = NULL;
	}
}

static const struct usbdev_ops_s g_devops = {
	.allocep		= usbd_ep_alloc,
	.freeep			= usbd_ep_free,
	.getframe		= NULL,
	.wakeup			= NULL,
	.selfpowered	= NULL,
	.pullup			= NULL,
};

static void
gadget_init_ep0(usb_dwc_pcd_t *pcd, struct gadget_wrapper *d)
{
	usb_dwc_pcd_ep_t *pcd_ep = NULL;
	struct usbdev_ep_s *ep0 = NULL;

	d->usb_dev.ep0 = &d->ep0.usb_ep;
	pcd_ep = &d->ep0;
	ep0 = &pcd_ep->usb_ep;

	LOS_ListInit(&pcd_ep->dwc_ep.queue);

	/* Init the usb_ep structure */
	ep0->ops = &g_epops;
	ep0->maxpacket = USB_DWC_MAX_EP0_SIZE;
	ep0->used = 1;
}

static int
gadget_init_eps(usb_dwc_pcd_t *pcd, struct gadget_wrapper *d)
{
	usb_dwc_pcd_ep_t *pcd_ep = NULL;
	struct usbdev_ep_s *ep_t = NULL;
	const char *name = NULL;
	const char *tmp = NULL;
	const char *ttmp = NULL;
	int ep_in;
	int ep_out;
	uint8_t num;
	uint32_t i;

	for (i = 1, ep_in = 0, ep_out = 0; i < ARRAY_SIZE(d->ep_names); i++) {
		name = d->ep_names[i];
		if ((name == NULL) || (!name[0]) || ((ep_in >= IN_EP_NUM) && (ep_out >= OUT_EP_NUM)))
			break;
		/* Find '-' in e.g. "ep1in-bulk" */
		tmp = strrchr(name, '-');
		if (tmp == NULL)
			/* If no '-' then find end of string */
			tmp = name + strlen(name);

		/* If not 0-len string then back up 1 char */
		if (tmp != name)
			tmp--;

		/* Get the EP number */
		num = 0;
		ttmp = tmp;

		if (*tmp == 't') {
			/* If "out", back up to the number before the 'o' */
			if (tmp >= name + 3)
				tmp -= 3;
		} else if (*tmp == 'n') {
			/* If "in", back up to the number before the 'i' */
			if (tmp >= name + 2)
				tmp -= 2;
		}

		if (isdigit(*tmp)) {
			/* If numeric, handle 1 or 2 digits */
			if ((tmp > name) && isdigit(*(tmp - 1)))
				num = (uint8_t)strtol(tmp - 1, NULL, 10);
			else
				num = (uint8_t)strtol(tmp, NULL, 10);
		}

		tmp = ttmp;
		if ((num < 1) || (num >= USB_DWC_MAX_EPS))
			goto bad;

		/* If e.g. "ep1" or "ep1out" then OUT ep */
		if ((isdigit(*tmp) || (*tmp == 't')) && (ep_out < OUT_EP_NUM)) {
			ep_out++;
			pcd_ep = &d->out_ep[ep_out - 1];
			ep_t = &pcd_ep->usb_ep;
			pcd_ep->dwc_ep.num = num;
			LOS_ListInit(&pcd_ep->dwc_ep.queue);
			ep_t->eplog = USB_EPOUT(num);

			/* Init the usb_ep structure */
			ep_t->ops = &g_epops;
			ep_t->maxpacket = USB_DWC_MAX_PACKET_SIZE;
			ep_t->max_streams = 15;
			ep_t->mult = 0;
			ep_t->maxburst = 0;
			ep_t->ep_link = (void *)pcd;
			ep_t->used = 0;
		}

		/* If e.g. "ep1" or "ep1in" then IN ep */
		if ((isdigit(*tmp) || (*tmp == 'n')) && (ep_in < IN_EP_NUM)) {
			ep_in++;
			pcd_ep = &d->in_ep[ep_in - 1];
			ep_t = &pcd_ep->usb_ep;
			pcd_ep->dwc_ep.num = num;
			LOS_ListInit(&pcd_ep->dwc_ep.queue);
			ep_t->eplog = USB_EPIN(num);

			/* Init the usb_ep structure */
			ep_t->ops = &g_epops;
			ep_t->maxpacket = USB_DWC_MAX_PACKET_SIZE;
			ep_t->max_streams = 15;
			ep_t->mult = 0;
			ep_t->maxburst = 0;
			ep_t->ep_link = (void *)pcd;
			ep_t->used = 0;
		}

		if (!isdigit(*tmp) && (*tmp != 't') && (*tmp != 'n')) {
bad:
			dwc3_lg_show("ep%u is bad\n", num);
			return (-EINVAL);
		}
	}

	return (0);
}

static void
gadget_free_ep_allocations(const usb_dwc_pcd_t *pcd, struct gadget_wrapper *d)
{
	usb_dwc_pcd_ep_t *pcd_ep = NULL;
	int index;

	dwc_usb3_free_pcd_req(&d->ep0);

	for (index = 0; index < pcd->num_in_eps; index++) {
		pcd_ep = &d->in_ep[index];
		dwc_usb3_free_pcd_req(pcd_ep);
		dwc_usb3_pcd_trb_free(pcd_ep);
	}

	for (index = 0; index < pcd->num_out_eps; index++) {
		pcd_ep = &d->out_ep[index];
		dwc_usb3_free_pcd_req(pcd_ep);
		dwc_usb3_pcd_trb_free(pcd_ep);
	}
}

int
dwc_usb3_gadget_init(dwc_usb3_device_t *usb3_dev, struct dev_ice *dev)
{
	usb_dwc_pcd_t *pcd = &usb3_dev->pcd;
	int retval = -ENOMEM;
	uint32_t i;

	gadget_wrapper = malloc(sizeof(struct gadget_wrapper));
	if (gadget_wrapper == NULL) {
		goto out1;
	}
	(void)memset_s(gadget_wrapper, sizeof(struct gadget_wrapper), 0, sizeof(*gadget_wrapper));
	gadget_wrapper->pcd = pcd;
	gadget_wrapper->usb_dev.ops = &g_devops;

	for (i = 0; (i < ARRAY_SIZE(g_ep_names)) &&
	    (i < ARRAY_SIZE(gadget_wrapper->ep_names)); i++) {
		(void)strncpy_s(gadget_wrapper->ep_names[i], sizeof(gadget_wrapper->ep_names[i]),
		    g_ep_names[i], sizeof(gadget_wrapper->ep_names[i]) - 1);
		gadget_wrapper->ep_names[i][sizeof(gadget_wrapper->ep_names[i]) - 1] = 0;
	}
	gadget_wrapper->usb_dev.speed = USB_SPEED_UNKNOWN;
	gadget_wrapper->driver = NULL;

	/* Set the PCD's EP0 request pointer to the wrapper's request */
	pcd->ep0_req = &gadget_wrapper->ep0_req;

	/* Set the PCD's EP array pointers to the wrapper's EPs */
	pcd->ep0 = &gadget_wrapper->ep0;
	for (i = 0; i < USB_DWC_MAX_EPS - 1; i++) {
		pcd->out_ep[i] = &gadget_wrapper->out_ep[i];
		pcd->in_ep[i] = &gadget_wrapper->in_ep[i];
	}

	pcd->usb_ep0_setup_pkt = dwc_usb3_dma_alloc(sizeof(*pcd->usb_ep0_setup_pkt) * 5);
	if (pcd->usb_ep0_setup_pkt == NULL) {
		goto out2;
	}

	pcd->ep0_setup_pkt_dma = (usb_dwc_dma_t)USB_DMA_VA_TO_PA(pcd->usb_ep0_setup_pkt);
	if (pcd->ep0_setup_pkt_dma == 0) {
		goto out3;
	}

	pcd->ep0_status_buf = dwc_usb3_dma_alloc(DWC_EVENT_BUF_SIZE);
	if (pcd->ep0_status_buf == NULL) {
		goto out3;
	}

	pcd->ep0_status_buf_dma = (usb_dwc_dma_t)USB_DMA_VA_TO_PA(pcd->ep0_status_buf);
	if (pcd->ep0_status_buf_dma == 0) {
		goto out4;
	}


	pcd->ep0_setup_desc = dwc_usb3_dma_alloc(sizeof(usb_dwc_dma_desc_t));
	if (pcd->ep0_setup_desc == NULL) {
		goto out4;
	}

	pcd->ep0_setup_desc_dma = (usb_dwc_dma_t)USB_DMA_VA_TO_PA(pcd->ep0_setup_desc);
	if (pcd->ep0_setup_desc_dma == 0) {
		goto out5;
	}

	pcd->ep0_in_desc = dwc_usb3_dma_alloc(sizeof(usb_dwc_dma_desc_t));
	if (pcd->ep0_in_desc == NULL) {
		goto out5;
	}

	pcd->ep0_in_desc_dma = (usb_dwc_dma_t)USB_DMA_VA_TO_PA(pcd->ep0_in_desc);
	if (pcd->ep0_in_desc_dma == 0) {
		goto out6;
	}

	pcd->ep0_out_desc = dwc_usb3_dma_alloc(sizeof(usb_dwc_dma_desc_t));
	if (pcd->ep0_out_desc == NULL) {
		goto out6;
	}

	pcd->ep0_out_desc_dma = (usb_dwc_dma_t)USB_DMA_VA_TO_PA(pcd->ep0_out_desc);
	if (pcd->ep0_out_desc_dma == 0) {
		goto out7;
	}

	gadget_init_ep0(pcd, gadget_wrapper);
	retval = gadget_init_eps(pcd, gadget_wrapper);
	if (retval) {
		dwc3_lg_show("%s gadget_init_eps error\n", __FUNCTION__);
		goto out7;
	}

	return (0);

out7:
	dwc_usb3_dma_free(pcd->ep0_out_desc);
	pcd->ep0_out_desc = NULL;
	pcd->ep0_out_desc_dma = 0;
out6:
	dwc_usb3_dma_free(pcd->ep0_in_desc);
	pcd->ep0_in_desc = NULL;
	pcd->ep0_in_desc_dma = 0;
out5:
	dwc_usb3_dma_free(pcd->ep0_setup_desc);
	pcd->ep0_setup_desc = NULL;
	pcd->ep0_setup_desc_dma = 0;
out4:
	dwc_usb3_dma_free(pcd->ep0_status_buf);
	pcd->ep0_status_buf = NULL;
	pcd->ep0_status_buf_dma = 0;
out3:
	dwc_usb3_dma_free(pcd->usb_ep0_setup_pkt);
	pcd->usb_ep0_setup_pkt = NULL;
	pcd->ep0_setup_pkt_dma = 0;
out2:
	free(gadget_wrapper);
	gadget_wrapper = NULL;
out1:
	return (retval);
}

int usbd_gadget_detach_driver(void *context, struct usbdevclass_driver_s *driver);

void
dwc_usb3_gadget_remove(dwc_usb3_device_t *usb3_dev)
{
	usb_dwc_pcd_t *pcd = &usb3_dev->pcd;

	dwc3_lg_debug("%s()\n", __FUNCTION__);

	if (gadget_wrapper == NULL) {
		dwc3_lg_debug("%s null wrapper!\n", __FUNCTION__);
		return;
	}

	/* start with the driver above us */
	if (gadget_wrapper->driver != NULL) {
		/* should have been done already by driver model core */
		(void)usbd_gadget_detach_driver(NULL, gadget_wrapper->driver);
	}

	gadget_free_ep_allocations(pcd, gadget_wrapper);

	dwc_usb3_dma_free(pcd->ep0_out_desc);
	pcd->ep0_out_desc = NULL;
	pcd->ep0_out_desc_dma = 0;

	dwc_usb3_dma_free(pcd->ep0_in_desc);
	pcd->ep0_in_desc = NULL;
	pcd->ep0_in_desc_dma = 0;

	dwc_usb3_dma_free(pcd->ep0_setup_desc);
	pcd->ep0_setup_desc = NULL;
	pcd->ep0_setup_desc_dma = 0;

	dwc_usb3_dma_free(pcd->ep0_status_buf);
	pcd->ep0_status_buf = NULL;
	pcd->ep0_status_buf_dma = 0;

	dwc_usb3_dma_free(pcd->usb_ep0_setup_pkt);
	pcd->usb_ep0_setup_pkt = NULL;
	pcd->ep0_setup_pkt_dma = 0;

	free(gadget_wrapper);
	gadget_wrapper = NULL;
}

int
usbd_gadget_detach_driver(void *context, struct usbdevclass_driver_s *driver)
{
	int ret;

	if ((context == NULL) || (driver == NULL)) {
		ret = -EINVAL;
		goto err;
	}

	if (gadget_wrapper == NULL) {
		ret = -ENODEV;
		goto err;
	}

	if (gadget_wrapper->pcd == NULL) {
		dwc3_lg_show(USB3_DWC "%s Return(%d): pcd==NULL\n", __FUNCTION__, -ENODEV);
		ret = -ENODEV;
		goto err;
	}

	if ((gadget_wrapper->driver == NULL) || (driver != gadget_wrapper->driver)) {
		dwc3_lg_show(USB3_DWC "%s Return(%d): driver?\n",  __FUNCTION__, -EINVAL);
		ret = -EINVAL;
		goto err;
	}

	ret = driver->ops->unbind(driver, &gadget_wrapper->usb_dev);
	if (ret) {
		dwc3_lg_show(USB3_DWC "%s Return(%d): unbind fail\n",  __FUNCTION__, ret);
		goto err;
	}

	gadget_wrapper->driver = NULL;
	gadget_wrapper->pcd->gadget = NULL;

	dwc3_lg_debug(USB3_DWC "unregistered gadget driver!\n");

err:
	return (ret);
}

int usbd_gadget_attach_driver(void *context, struct usbdevclass_driver_s *driver)
{
	usb_dwc_pcd_t *pcd = NULL;
	int retval = -ENODEV;

	pcd = gadget_wrapper->pcd;
	if (pcd == NULL) {
		dwc3_lg_show(USB3_DWC "ENODEV\n");
		goto out;
	}

	/* hook up the driver */
	gadget_wrapper->driver = driver;
	pcd->gadget = &gadget_wrapper->usb_dev;

	if ((driver->ops != NULL) && (driver->ops->bind != NULL)) {
		retval = driver->ops->bind(driver, &gadget_wrapper->usb_dev);
		if (retval) {
			dwc3_lg_show("bind to driver--> error %d\n", retval);
			gadget_wrapper->driver = NULL;
			pcd->gadget = NULL;
			goto out;
		}
	}

	dwc3_lg_debug(USB3_DWC "registered gadget driver!\n");
out:
	return (retval);
}
