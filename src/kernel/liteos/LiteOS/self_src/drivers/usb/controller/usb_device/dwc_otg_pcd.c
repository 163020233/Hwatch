/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2021. All rights reserved.
 * Description: USB Device Controller PCD Core Implementation
 * Author: Huawei LiteOS Team
 * Create: 2013-05-16
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

#include "controller/usb_device/dwc_otg_pcd.h"
#include "controller/usb_device/dwc_otgreg.h"
#include "controller/usb_device/dwc_otg_pcd_intr.h"
#include "implementation/usb_dma_cache.h"
#include "kal_tick.h"
#include "usb_os_adapt.h"
#include USB_HEADER_PATH
#include "phy/usb_phy.h"

#ifdef LOSCFG_USB_DEBUG
#define	PCDSHOW(x...) KAL_Printf(x)
#endif

#define	SETUP_PACKET_NUM_MAX 3 /* Receive the maximum number of setup packages at a time */
#ifdef LOSCFG_DRIVERS_USB2_DWC_OTG_DFIFO_DYNAMIC
#define	CONTROL_EP_NUM 1 /* By default there is only one control endpoint */

/*
 * IN_EP_NUM: The range of this field is 1-15, refer to DesignWare Cores USB 2.0 Hi-Speed On-The-Go
 * Databook(Version 3.30a), section 5.3.3.17~5.3.3.20 for the details. the current platform maximum value is 3.
 * LOSCFG_DRIVERS_USB2_DWC_IN_EP_INDEXn_MPS: used to calculate the value of TX FIFOn, refer to
 * DesignWare Cores USB 2.0 Hi-Speed On-The-Go Databook(Version 3.30a), section 5.3.3.27 for the details.
 */
static uint32_t g_usb_in_ep_mps[IN_EP_NUM] = {
#ifdef LOSCFG_DRIVERS_USB2_DWC_IN_EP_INDEX1_MPS
	LOSCFG_DRIVERS_USB2_DWC_IN_EP_INDEX1_MPS,
#endif
#ifdef LOSCFG_DRIVERS_USB2_DWC_IN_EP_INDEX2_MPS
	LOSCFG_DRIVERS_USB2_DWC_IN_EP_INDEX2_MPS,
#endif
#ifdef LOSCFG_DRIVERS_USB2_DWC_IN_EP_INDEX3_MPS
	LOSCFG_DRIVERS_USB2_DWC_IN_EP_INDEX3_MPS,
#endif
};
#endif

void dwc_otg_ep_set_stall(dwc_otg_pcd_t *pcd, dwc_otg_pcd_ep_t *ep);

static dwc_otg_pcd_ep_t *
uep2dep(struct usbdev_ep_s *ep)
{
	return ((dwc_otg_pcd_ep_t *)ep->ep_link);
}

static struct usbdev_req_s *
usbd_ep_alloc_req(struct usbdev_ep_s *ep)
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
usbd_ep_free_req(struct usbdev_ep_s *ep, struct usbdev_req_s *req)
{
	(void)ep;
	if (req == NULL) {
		return;
	}
	free(req);
}

static void *
usbd_ep_alloc_buff(struct usbdev_ep_s *ep, uint16_t len)
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
usbd_ep_free_buff(struct usbdev_ep_s *ep, void *buf)
{
	(void)ep;
	if (buf == NULL) {
		return;
	}
	free(buf);
}

static int
usbd_ep_enable(struct usbdev_ep_s *ep, const struct usb_epdesc_s *desc, bool last)
{
	(void)last;

	if ((ep == NULL) || (ep->ep_link == NULL) || (desc == NULL)) {
		return (-1);
	}
	return (dwc_otg_endpoint_enable(ep, desc));
}

static int
usbd_ep_disable(struct usbdev_ep_s *ep)
{
	if ((ep == NULL) || (ep->ep_link == NULL)) {
		return (-1);
	}
	return (dwc_otg_endpoint_disable(ep));
}

static int
usbd_ep_queue(struct usbdev_ep_s *ep, struct usbdev_req_s *req)
{
	if ((ep == NULL) || (ep->ep_link == NULL) || (req == NULL)) {
		return (-1);
	}
	return (dwc_otg_ep_req_start(ep, req));
}

static int
usbd_ep_dequeue(struct usbdev_ep_s *ep, struct usbdev_req_s *req)
{
	return (0);
}

static int
usbd_ep_set_halt(struct usbdev_ep_s *ep, bool resume)
{
	if ((ep == NULL) || (ep->ep_link == NULL) || (uep2dep(ep)->private_data == NULL)) {
		return (-1);
	}
	return (dwc_otg_ep_halt(ep, resume));
}

static void
usbd_ep_fifo_flush(struct usbdev_ep_s *ep)
{
	dwc_otg_pcd_ep_t *pcd_ep = NULL;

	if (ep->ep_link == NULL) {
		return;
	}

	pcd_ep = ((dwc_otg_pcd_ep_t *)ep->ep_link);

	if (pcd_ep->is_in) {
		dwc_otg_flush_tx_fifo(pcd_ep->fifo_index);
	} else {
		dwc_otg_flush_rx_fifo();
	}

	return;
}

static const struct usbdev_epops_s g_epops = {
	.configure		= usbd_ep_enable,
	.disable		= usbd_ep_disable,
	.allocreq		= usbd_ep_alloc_req,
	.freereq		= usbd_ep_free_req,
	.allocbuffer	= usbd_ep_alloc_buff,
	.freebuffer		= usbd_ep_free_buff,
	.submit			= usbd_ep_queue,
	.cancel			= usbd_ep_dequeue,
	.stall			= usbd_ep_set_halt,
	.flush			= usbd_ep_fifo_flush,
};

struct usbdev_ep_s *
usbd_ep_alloc(struct usbdev_s *dev, uint8_t eplog, struct usb_epdesc_s *desc)
{
	dwc_otg_pcd_t *pcd = (dwc_otg_pcd_t *)dev;
	struct usbdev_ep_s *eps_array = NULL;
	struct usbdev_ep_s *ep = NULL;
	struct usbdev_ep_s *valid_ep = NULL;
	uint8_t epphy;
	uint8_t epno;
	uint8_t num_eps;
	uint8_t flag = 0;

	epphy = USB_EPNO(eplog);
	if (epphy >= PCD_MAX_EPS) {
		usb_err("ep num is too big!\n");
		return (NULL);
	}

	if (USB_ISEPIN(eplog)) {
		num_eps = pcd->num_in_eps;
		eps_array = pcd->usbd_in_eps;
	} else {
		num_eps = pcd->num_out_eps;
		eps_array = pcd->usbd_out_eps;
	}

	for (epno = 0; epno < num_eps; epno++) {
		ep = eps_array + epno;

		/* Find the specified endpoint. */
		if ((ep->eplog == eplog) && !ep->used) {
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
			if ((ep->eplog & USB_DIR_MASK) == (eplog & USB_DIR_MASK)) {
				ep->used = 1;
				desc->addr = ep->eplog;
				valid_ep = ep;
				flag = 1;
			}
		}
	}

	if (valid_ep != NULL) {
		return (valid_ep);
	}

	usb_err("The addr=%x ep is invalid!\n", eplog);
	return (NULL);
}

void
usbd_ep_free(struct usbdev_s *dev, struct usbdev_ep_s *ep)
{
	uint8_t epphy;

	epphy = USB_EPNO(ep->eplog);
	if (epphy >= PCD_MAX_EPS) {
		usb_err("ep num is too big!\n");
		return;
	}

	if (ep->used) {
		ep->used = 0;
		ep->priv = NULL;
		ep->handle_req = NULL;
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

int
usbd_gadget_attach_driver(void *context, struct usbdevclass_driver_s *driver)
{
	dwc_otg_pcd_t *pcd = NULL;
	int retval;

	pcd = (dwc_otg_pcd_t *)context;
	pcd->driver = driver;

	if ((driver->ops != NULL) && (driver->ops->bind != NULL)) {
		retval = driver->ops->bind(driver, &pcd->usb_dev);
		if (retval) {
			usb_err("bind to driver--> error %d\n", retval);
			pcd->driver = NULL;
			return (retval);
		}
	}
	return (0);
}

int
usbd_gadget_detach_driver(void *context, struct usbdevclass_driver_s *driver)
{
	dwc_otg_pcd_t *pcd = NULL;
	pcgcctl_data_t pcgcctl = { .d32 = 0 };
	int ret = 0;

	if ((context == NULL) || (driver == NULL)) {
		return (-1);
	}

	pcd = (dwc_otg_pcd_t *)context;
	if (pcd->lx_state == USB_DWC2_STATE_L2) {
		pcgcctl.b.stoppclk_rw = 1;
		DWC_MODIFY_REG32(DOTG_PCGCCTL, pcgcctl.d32, 0);
		KAL_Udelay(10);
	}

	if ((driver->ops != NULL) && (driver->ops->unbind != NULL)) {
		ret = driver->ops->unbind(driver, &pcd->usb_dev);
		if (ret) {
			return (ret);
		}
	}

	pcd->driver = NULL;

	return (ret);
}

static inline void
ep0_do_stall(dwc_otg_pcd_t *pcd, const int err_val)
{
	dwc_otg_pcd_ep_t *ep0 = &pcd->pcd_ep0;

	ep0->is_in = 1;
	dwc_otg_ep_set_stall(pcd, ep0);
	ep0->is_in = 0;
	dwc_otg_ep_set_stall(pcd, ep0);
	ep0->is_stopped = 1;
	pcd->ep0state = EP0_IDLE;
	endpoint0_startup_receiving_setup(pcd);
}

void
dwc_otg_core_do_pcd_setup(dwc_otg_pcd_t *pcd, struct usb_ctrlreq_s *ctrl)
{
	int ret = 0;
	struct usbdevclass_driver_s *driver = pcd->driver;
	if ((driver != NULL) && (driver->ops != NULL) && (driver->ops->setup != NULL)) {
		ret = driver->ops->setup(driver, &pcd->usb_dev, ctrl, NULL, 0);
	}
	if (ret < 0) {
		usb_err("setup error!\n");
		ep0_do_stall(pcd, ret);
	}
}

void
dwc_otg_core_ep_complete(dwc_otg_pcd_ep_t *pcd_ep, struct usbdev_ep_s *ep)
{
	struct usbdev_req_s *req = ep->handle_req;

	if (req == NULL) {
		return;
	}

	req->xfrd = pcd_ep->xfer_count;
	req->result = 0;
	req->is_complete = 1;
	if (req->callback != NULL) {
		req->callback(ep, req);
	}
}

void
dwc_otg_core_do_dev_disconnect(dwc_otg_pcd_t *pcd)
{
	struct usbdevclass_driver_s *driver = pcd->driver;
	if ((driver != NULL) && (driver->ops != NULL) && (driver->ops->disconnect != NULL)) {
		driver->ops->disconnect(driver, &pcd->usb_dev);
	}
}

static void
dwc_otg_endpoints_link(struct usbdev_ep_s *ep, dwc_otg_pcd_ep_t *pcd_ep, uint8_t used)
{
	ep->priv = NULL;
	ep->ops = &g_epops;
	ep->ep_link = (void *)pcd_ep;
	ep->used = used;
}

static void
dwc_otg_core_add_endpoints(dwc_otg_pcd_t *pcd)
{
	struct usbdev_ep_s *ep = NULL;
	dwc_otg_pcd_ep_t *pcd_ep = NULL;
	uint8_t i;

	ep = &pcd->usbd_ep0;
	pcd->usb_dev.ep0 = ep;
	pcd_ep = &pcd->pcd_ep0;
	dwc_otg_endpoints_link(ep, pcd_ep, 1);

	for (i = 0; i < pcd->num_in_eps; i++) {
		pcd_ep = &pcd->pcd_in_eps[i];
		ep = &pcd->usbd_in_eps[i];
		ep->eplog = USB_EPIN(pcd_ep->num);
		dwc_otg_endpoints_link(ep, pcd_ep, 0);
	}

	for (i = 0; i < pcd->num_out_eps; i++) {
		pcd_ep = &pcd->pcd_out_eps[i];
		ep = &pcd->usbd_out_eps[i];
		ep->eplog = USB_EPOUT(pcd_ep->num);
		dwc_otg_endpoints_link(ep, pcd_ep, 0);
	}
}

static void
dwc_otg_core_init_ep(dwc_otg_pcd_t *pcd, dwc_otg_pcd_ep_t *ep, int is_in, int dirnum)
{
	ep->num = dirnum;
	ep->is_in = is_in;
	ep->dma_addr = 0;
	ep->start_xfer_buff = NULL;
	ep->xfer_buff = NULL;
	ep->xfer_type = USB_EP_TYPE_CONTROL;
	ep->xfer_len = 0;
	ep->xfer_count = 0;
	ep->maxpacket = 64;
	ep->sent_zlp = 0;
	ep->total_len = 0;
	ep->private_data = (void *)pcd;
}

/*
 * dwc_otg_core_scan_endpoints
 * 1.handle all udc endpoit direction by hwcfg1
 * 2.init pcd_ep address, calc num_in_eps, num_out_eps;
 * 3.init usbd_ep, config ep_ops, link pcd_ep;
 * 4.init all ep registers(EPCTL/EPTSIZ/EPDMA/EPINT)
 */
static int
dwc_otg_core_scan_endpoints(dwc_otg_pcd_t *pcd)
{
	uint8_t i;
	uint8_t num_eps;
	uint8_t num_in_eps = 0;
	uint8_t num_out_eps = 0;
	uint8_t ep_num;
	uint32_t cfg_data;
	hwcfg1_data_t hwcfg1_data;
	hwcfg2_data_t hwcfg2_data;
	depctl_data_t depctl;
	dctl_data_t dctl = { .d32 = 0 };
	gintmsk_data_t gintsts;
	doepint_data_t doepint;

	hwcfg1_data.d32 = DWC_READ_REG32(DOTG_GHWCFG1);
	hwcfg2_data.d32 = DWC_READ_REG32(DOTG_GHWCFG2);
	num_eps = hwcfg2_data.b.num_dev_ep_r;

	cfg_data = hwcfg1_data.d32 >> 2;
	for (i = 1; i <= num_eps; ++i) {
		/*
		 * Each endpoint number corresponds to two bits, and the bit value indicates the endpoint direction:
		 * 2’b00: BIDIR (IN and OUT) endpoint. 2’b01: IN endpoint. 2’b10: OUT endpoint.
		 * Bit[0]:0 indicating that the endpoint number has an OUT direction.
		 * Bit[1]:0 indicating that the endpoint number has an IN direction.
		 * The number of endpoint directions can be obtained by traversing once. refer to DesignWare Cores USB 2.0
		 * Hi-Speed On-The-Go Databook(Version 3.30a), section 5.3.3.17 and Table 5-25 for the details.
		 */
		if ((cfg_data & EP_DIR_OUT) == 0) {
			dwc_otg_core_init_ep(pcd, &pcd->pcd_in_eps[num_in_eps], 1, i);
			num_in_eps++;
		}

		if ((cfg_data & EP_DIR_IN) == 0) {
			dwc_otg_core_init_ep(pcd, &pcd->pcd_out_eps[num_out_eps], 0, i);
			num_out_eps++;
		}

		cfg_data >>= 2;
	}

	dwc_otg_core_init_ep(pcd, &pcd->pcd_ep0, 0, 0);

	pcd->num_in_eps = num_in_eps;
	pcd->num_out_eps = num_out_eps;
	pcd->pcd_num_eps = num_eps;

	ep_num = 0;
	for (i = 0; i <= num_in_eps; i++) {
		if (i) {
			ep_num = pcd->pcd_in_eps[i - 1].num;
		}

		depctl.d32 = DWC_READ_REG32(DOTG_DIEPCTL(ep_num));
		if (depctl.b.epena_r_ws_sc) {
			depctl.d32 = 0;
			depctl.b.epdis_r_ws_sc = 1;
			depctl.b.snak_w = 1;
		} else {
			depctl.d32 = 0;
			depctl.b.snak_w = 1;
		}
		DWC_WRITE_REG32(DOTG_DIEPCTL(ep_num), depctl.d32);
		DWC_WRITE_REG32(DOTG_DIEPTSIZ(ep_num), 0);
		DWC_WRITE_REG32(DOTG_DIEPDMA(ep_num), 0);
		DWC_WRITE_REG32(DOTG_DIEPINT(ep_num), 0xFF);
	}

	ep_num = 0;
	for (i = 0; i <= num_out_eps; i++) {
		if (i) {
			ep_num = pcd->pcd_out_eps[i - 1].num;
		}

		depctl.d32 = DWC_READ_REG32(DOTG_DOEPCTL(ep_num));
		if (depctl.b.epena_r_ws_sc) {
			dctl.b.sgoutnak_w = 1;
			DWC_MODIFY_REG32(DOTG_DCTL, 0, dctl.d32);
			do {
				KAL_Udelay(10);
				gintsts.d32 = DWC_READ_REG32(DOTG_GINTSTS);
			} while (!gintsts.b.goutnakeff_rw);
			gintsts.d32 = 0;
			gintsts.b.goutnakeff_rw = 1;
			DWC_WRITE_REG32(DOTG_GINTSTS, gintsts.d32);

			depctl.d32 = 0;
			depctl.b.epdis_r_ws_sc = 1;
			depctl.b.snak_w = 1;
			DWC_WRITE_REG32(DOTG_DOEPCTL(ep_num), depctl.d32);
			do {
				KAL_Udelay(10);
				doepint.d32 = DWC_READ_REG32(DOTG_DOEPINT(ep_num));
			} while (!doepint.b.epdisabled_r_ss_wc);

			doepint.b.epdisabled_r_ss_wc = 1;
			DWC_WRITE_REG32(DOTG_DOEPINT(ep_num), doepint.d32);

			dctl.d32 = 0;
			dctl.b.cgoutnak_w = 1;
			DWC_MODIFY_REG32(DOTG_DCTL, 0, dctl.d32);
		} else {
			depctl.d32 = 0;
			depctl.b.snak_w = 1;
		}
		DWC_WRITE_REG32(DOTG_DOEPCTL(ep_num), depctl.d32);
		DWC_WRITE_REG32(DOTG_DOEPTSIZ(ep_num), 0);
		DWC_WRITE_REG32(DOTG_DOEPDMA(ep_num), 0);
		DWC_WRITE_REG32(DOTG_DOEPINT(ep_num), 0xFF);
	}
	return (num_in_eps + num_out_eps);
}

int
dwc_otg_initial(dwc_otg_pcd_t *pcd)
{
	gotgctl_data_t gctrldata;
	int32_t snpsid;
	gahbcfg_data_t ahbcfg = { .d32 = 0 };
	uint32_t setup_size = sizeof(struct usb_ctrlreq_s) * SETUP_PACKET_NUM_MAX;

	(void)KAL_SpinInit(&pcd->irq_lock);
	(void)KAL_SpinInit(&pcd->lock);

	pcd->usb_dev.speed = USB_SPEED_HIGH;

	if (pcd->setup_pkt == NULL) {
		pcd->setup_pkt = KAL_MemAllocAlign((void *)g_usb_mem_addr_start, setup_size, USB_CACHE_ALIGN_SIZE);
		if (pcd->setup_pkt == NULL) {
			return (-1);
		}
		(void)memset_s((void *)pcd->setup_pkt, setup_size, 0, setup_size);
	}
	snpsid = DWC_READ_REG32(DOTG_GSNPSID);
	dwc_otg_debug("new DOTG_GSNPSID snpsid = 0x%x\n", snpsid);
	if (!(((snpsid & 0xFFFFF000) == 0x4F542000) || ((snpsid & 0xFFFFF000) == 0x4F543000))) {
		dwc_otg_debug("%s,Bad value for SNPSID: 0x%08x\n", __FUNCTION__, snpsid);
		return (-1);
	}

	/* GOTGCTL */
	gctrldata.d32 = DWC_READ_REG32(DOTG_GOTGCTL);
	if (!gctrldata.b.bsesvld_r) {
		dwc_otg_debug("GOTGCTL=0x%08x,bsesvld=%x\n", gctrldata.d32,gctrldata.b.bsesvld_r);
		return (-1);
	}

	/*
	 * Disable the global interrupt until all the interrupt
	 * handlers are installed.
	 */
	ahbcfg.b.glblintrmsk_rw = 1;   /* Enable interrupts bit */
	DWC_MODIFY_REG32(DOTG_GAHBCFG, ahbcfg.d32, 0);

	pcd->lx_state = USB_DWC2_STATE_L3;

	/*
	 * Initialize the DWC_otg core.
	 */
	if (dwc_otg_core_init(pcd) != 0) {
		return (-1);
	}

	/*
	 * Enable the global interrupt after all the interrupt
	 * handlers are installed.
	 */
	dwc_otg_enable_global_interrupts();

	return (0);
}

int
dwc_otg_close(dwc_otg_pcd_t *pcd)
{
	if (pcd != NULL) {
		if (pcd->setup_pkt != NULL) {
			(void)KAL_MemFree((void *)g_usb_mem_addr_start, pcd->setup_pkt);
			pcd->setup_pkt = NULL;
		}
	}
	return (0);
}

int
dwc_otg_irq(void *dwc_arg)
{
	int ret1;
	int ret2;
	dwc_otg_pcd_t *pcd = (dwc_otg_pcd_t *)dwc_arg;
	ret1 = dwc_otg_common_irq(pcd);
	ret2 = dwc_otg_pcd_irq(pcd);
	return (ret1 + ret2);
}

int
dwc_otg_get_soffn(void)
{
	dsts_data_t dsts;

	dsts.d32 = DWC_READ_REG32(DOTG_DSTS);
	return (dsts.b.soffn_r);
}

int
dwc_otg_remote_wakeup(void)
{
	dsts_data_t dsts;
	dctl_data_t dctl = { .d32 = 0 };
	pcgcctl_data_t pcgcctl = { .d32 = 0 };

	dsts.d32 = DWC_READ_REG32(DOTG_DSTS);
	if (!dsts.b.suspsts_r) {
		return (-1);
	}

	UsbPhyResume();

	pcgcctl.b.stoppclk_rw = 1;
	DWC_MODIFY_REG32(DOTG_PCGCCTL, pcgcctl.d32, 0);
	KAL_Udelay(10);

	/* This bit must be cleared 2ms after setting bit0. */
	dctl.b.rmtwkupsig_rw = 1;
	DWC_MODIFY_REG32(DOTG_DCTL, 0, dctl.d32);
	KAL_Mdelay(2);
	DWC_MODIFY_REG32(DOTG_DCTL, dctl.d32, 0);

	return (0);
}

void
dwc_otg_pullup(int is_on)
{
	if (is_on) {
		/* connect data line */
		DWC_MODIFY_REG32(DOTG_DCTL, 2, 0);
	} else {
		/* disconnect data line */
		DWC_MODIFY_REG32(DOTG_DCTL, 0, 2);
	}
}

int
dwc_otg_core_init(dwc_otg_pcd_t *pcd)
{
	gahbcfg_data_t ahbcfg = { .d32 = 0 };
	gusbcfg_data_t usbcfg = { .d32 = 0 };

	usbcfg.d32 = DWC_READ_REG32(DOTG_GUSBCFG);

	usbcfg.b.ulpi_ext_vbus_drv_rw = 0;
	usbcfg.b.term_sel_dl_pulse_rw = 0;
	DWC_WRITE_REG32(DOTG_GUSBCFG, usbcfg.d32);
	/*
	 * Reset the Controller
	 */
	dwc_otg_core_reset();

	usbcfg.d32 = DWC_READ_REG32(DOTG_GUSBCFG);
	usbcfg.b.srpcap_rw = 0;
	usbcfg.b.hnpcap_rw = 0;

	usbcfg.b.ulpi_utmi_sel_rw = 1;
	usbcfg.b.phyif_rw = 0; /* 16 bit */
	usbcfg.b.ddrsel_rw = 0;
	DWC_WRITE_REG32(DOTG_GUSBCFG, DWC_INTERFACE_CFG);

	dwc_otg_core_reset();
	ahbcfg.b.dma_enable_rw = 0;
	DWC_WRITE_REG32(DOTG_GAHBCFG, ahbcfg.d32);

	/*
	 * Enable common interrupts
	 */
	dwc_otg_enable_common_interrupts();

	/*
	 * Do device or host initialization based on mode during PCD and HCD
	 * initialization
	 */
	if (DWC_READ_REG32(DOTG_GINTSTS) & 0x1) {
		dwc_otg_debug("Host Mode\n");
		return (0);
	} else {
		dwc_otg_debug("Device Mode\n");
		return (dwc_otg_core_dev_init(pcd));
	}
}

/*
 * This function initialized the PCD portion of the driver.
 */
int
dwc_otg_pcd_init(void)
{
	return (0);
}

/*
 * Do core a soft reset of the core.  Be careful with this because it
 * resets all the internal state machines of the core.
 */
void
dwc_otg_core_reset(void)
{
	grstctl_t greset = { .d32 = 0 };
	int count = 0;

	/*
	 * Wait for AHB master IDLE state.
	 */
	do {
		KAL_Udelay(10);
		greset.d32 = DWC_READ_REG32(DOTG_GRSTCTL);
		if (++count > 100000) {
			return;
		}
	} while (greset.b.ahbidle_r == 0);

	/*
	 * Core Soft Reset
	 */
	count = 0;
	greset.b.csftrst_rw = 1;
	DWC_WRITE_REG32(DOTG_GRSTCTL, greset.d32);
	do {
		greset.d32 = DWC_READ_REG32(DOTG_GRSTCTL);
		if (++count > 1000000) {
			break;
		}
	} while (greset.b.csftrst_rw == 1);

	/*
	 * Wait for 3 PHY Clocks
	 */
	KAL_Udelay(10);
}

void
dwc_otg_enable_common_interrupts(void)
{
	gintmsk_data_t intr_mask = {0};
	/* Clear any pending OTG Interrupts */
	DWC_WRITE_REG32(DOTG_GOTGINT, 0xFFFFFFFF);
	/* Clear any pending interrupts */
	DWC_WRITE_REG32(DOTG_GINTSTS, 0xFFFFFFFF);
	/*
	 * Enable the interrupts in the GINTMSK.
	 */
	intr_mask.b.modemismatch_rw = 1;
	intr_mask.b.otgintr_rw = 1;
	intr_mask.b.rxstsqlvl_rw = 1;
	intr_mask.b.conidstschng_rw = 1;
	intr_mask.b.wkupintr_rw = 1;
	intr_mask.b.disconnect_rw = 1;
	intr_mask.b.usbsuspend_rw = 1;
	intr_mask.b.sessreqintr_rw = 1;
	DWC_WRITE_REG32(DOTG_GINTMSK, intr_mask.d32);
}

/*
 * This function enables the Device mode interrupts.
 */
void
dwc_otg_enable_device_interrupts(void)
{
	gintmsk_data_t intr_mask = {0};

	/* Disable all interrupts. */
	DWC_WRITE_REG32(DOTG_GINTMSK, 0);

	/* Clear any pending interrupts */
	DWC_WRITE_REG32(DOTG_GINTSTS, 0xFFFFFFFF);

	/* Enable the common interrupts */
	dwc_otg_enable_common_interrupts();

	/* Enable interrupts */
	intr_mask.b.modemismatch_rw = 1;
	intr_mask.b.otgintr_rw = 1;
	intr_mask.b.erlysuspend_rw = 1;
	intr_mask.b.usbsuspend_rw = 1;
	intr_mask.b.usbreset_rw = 1;
	intr_mask.b.enumdone_rw = 1;
	intr_mask.b.inepintr_rw = 1;
	intr_mask.b.outepintr_rw = 1;
#ifdef LOSCFG_DRIVERS_USB2_DWC_INCOMPISOIN
	intr_mask.b.incomplisoin_rw = 1;
#endif
	intr_mask.b.conidstschng_rw = 1;
	intr_mask.b.sessreqintr_rw = 1;
	intr_mask.b.wkupintr_rw = 1;

#ifdef LOSCFG_DRIVERS_USB_HID_POLLING_REPORTS
	intr_mask.b.sofintr_rw = 1;
#endif

	DWC_WRITE_REG32(DOTG_GINTMSK, intr_mask.d32);
}

/*
 * This function enables the controller's Global Interrupt in the AHB Config register.
 */
void
dwc_otg_enable_global_interrupts(void)
{
	gahbcfg_data_t ahbcfg = {0};
	ahbcfg.b.glblintrmsk_rw = 1; /* Enable interrupts */
	ahbcfg.b.hburstlen_rw = 3;
	ahbcfg.b.dma_enable_rw = 1;
	ahbcfg.b.reserved9_31 = 0x400000;
	DWC_WRITE_REG32(DOTG_GAHBCFG, ahbcfg.d32);
}

/*
 * This function initializes the DWC_otg controller registers for device mode.
 */
int
dwc_otg_core_dev_init(dwc_otg_pcd_t *pcd)
{
	dcfg_data_t dcfg = {0};
	grstctl_t resetctl = {0};
	fifosize_data_t nptxfifosize;
	gdfifocfg_data_t gdfifocfg;
	dctl_data_t dctl = { .d32 = 0 };
	diepmsk_data_t msk = { .d32 = 0 };
	int grxfsiz;

	pcd->usb_dev.ops = &g_devops;

	/* Restart the Phy Clock */
	DWC_WRITE_REG32(DOTG_PCGCCTL, 0);

	/* Device configuration register */
	dcfg.d32 = DWC_READ_REG32(DOTG_DCFG);
	dcfg.b.devspd_rw = 0;
	dcfg.b.descdma_rw = 0;
	dcfg.b.perfrint_rw = DCFG_FRAME_INTERVAL_95;
	DWC_WRITE_REG32(DOTG_DCFG, dcfg.d32);

	/* Configure data FIFO sizes */
	/* Rx FIFO */
#ifdef LOSCFG_DRIVERS_USB2_DWC_OTG_DFIFO_DYNAMIC
	/*
	 * Refer to DesignWare Cores USB 2.0 Hi-Speed On-The-Go Programmer’s Guide(Version 3.30a),
	 * section 2.1.3.1 Method 2 for the details
	 */
	grxfsiz = (5 * CONTROL_EP_NUM + 8) + 2 * ((LOSCFG_DRIVERS_USB2_DWC_OUT_EP_MPS / 4) + 1) +
	    (2 * (CONTROL_EP_NUM + OUT_EP_NUM)) + 1;
#else
	grxfsiz = DWC_READ_REG32(DOTG_GRXFSIZ);
#endif
	DWC_WRITE_REG32(DOTG_GRXFSIZ, grxfsiz);
	dwc_otg_debug("new grxfsiz=%08x\n", DWC_READ_REG32(DOTG_GRXFSIZ));

	/* Non-periodic Tx FIFO */
	nptxfifosize.b.depth_rw  = 64;
	nptxfifosize.b.startaddr_rw = grxfsiz;
	DWC_WRITE_REG32(DOTG_GNPTXFSIZ, nptxfifosize.d32);
	dwc_otg_debug("new gnptxfsiz=%08x\n", DWC_READ_REG32(DOTG_GNPTXFSIZ));

	/*
	 * Configure data TX FIFO sizes:
	 * Refer to DesignWare Cores USB 2.0 Hi-Speed On-The-Go Programmer’s Guide(Version 3.30a),
	 * section 2.1.1.2 for the details
	 */
#ifdef LOSCFG_DRIVERS_USB2_DWC_OTG_DFIFO_DYNAMIC
	fifosize_data_t dieptxfifosize = { .d32 = 0 };
	fifosize_data_t txfifosize;
	hwcfg2_data_t hwcfg2_data;
	hwcfg4_data_t hwcfg4_data;
	uint8_t in_ep_num;
	uint8_t i;

	/* All DIEPTXF registers are cleared to ensure that the searched TX FIFO is valid */
	hwcfg2_data.d32 = DWC_READ_REG32(DOTG_GHWCFG2);
	for (i = 1; i <= hwcfg2_data.b.num_dev_ep_r; i++) {
		DWC_WRITE_REG32(DOTG_DIEPTXF(i), dieptxfifosize.d32);
	}

	hwcfg4_data.d32 = DWC_READ_REG32(DOTG_GHWCFG4);
	in_ep_num = min(hwcfg4_data.b.num_in_eps_r, IN_EP_NUM);
	txfifosize.d32 = nptxfifosize.d32;
	for (i = 1; i <= in_ep_num; i++) {
		/*
		 * Refer to DesignWare Cores USB 2.0 Hi-Speed On-The-Go Programmer’s Guide(Version 3.30a),
		 * section 2.1.3.1 Method 2 for the details
		 */
		dieptxfifosize.b.depth_rw = 2 * g_usb_in_ep_mps[i - 1] / 4;
		dieptxfifosize.b.startaddr_rw = txfifosize.b.startaddr_rw + txfifosize.b.depth_rw;

		DWC_WRITE_REG32(DOTG_DIEPTXF(i), dieptxfifosize.d32);
		txfifosize.d32 = dieptxfifosize.d32;
	}

	gdfifocfg.b.gdfifocfg_rw = GHWCFG3_DFIFODEPTH_GET(DWC_READ_REG32(DOTG_GHWCFG3));
	gdfifocfg.b.epinfobase_rw = txfifosize.b.startaddr_rw + txfifosize.b.depth_rw;

	if (gdfifocfg.b.epinfobase_rw > gdfifocfg.b.gdfifocfg_rw) {
		usb_err("The total FIFO is set too large!\n");
		return (-1);
	}
	DWC_WRITE_REG32(DOTG_GDFIFOCFG, gdfifocfg.d32);
#endif

	/* Flush the FIFOs */
	dwc_otg_flush_tx_fifo(0x10); /* all Tx FIFOs */
	dwc_otg_flush_rx_fifo();

	/* Flush the Learning Queue. */
	resetctl.b.txfflsh_r_ws_sc = 1;
	DWC_WRITE_REG32(DOTG_GRSTCTL, resetctl.d32);

	/* Clear all pending Device Interrupts */
	DWC_WRITE_REG32(DOTG_DIEPMSK, 0);
	DWC_WRITE_REG32(DOTG_DOEPMSK, 0);
	DWC_WRITE_REG32(DOTG_DAINT, 0xFFFFFFFF);
	DWC_WRITE_REG32(DOTG_DAINTMSK, 0);

	if (dwc_otg_core_scan_endpoints(pcd)) {
		dwc_otg_core_add_endpoints(pcd);
	}

	DWC_WRITE_REG32(DOTG_DTHRCTL, 0x800100);

	dwc_otg_enable_device_interrupts();

	msk.d32 = 0;
	msk.b.txfifoundrn_r_ss_wc = 1;
	DWC_MODIFY_REG32(DOTG_DIEPMSK, msk.d32, msk.d32);

	dctl.d32 = 0;
	dctl.d32 = DWC_READ_REG32(DOTG_DCTL);
	dctl.b.sftdiscon_rw = 0;
	DWC_WRITE_REG32(DOTG_DCTL, dctl.d32);
	return (0);
}

/*
 * Flush a Tx FIFO.
 */
void
dwc_otg_flush_tx_fifo(const uint32_t num)
{
	volatile grstctl_t greset = { .d32 = 0 };
	int count = 0;

	dwc_otg_debug("flush tx_fifo: %u\n", num);
	greset.b.txfflsh_r_ws_sc = 1;
	greset.b.txfnum_rw = num;
	DWC_WRITE_REG32(DOTG_GRSTCTL, greset.d32);

	do {
		greset.d32 = DWC_READ_REG32(DOTG_GRSTCTL);
		if (++count > 10000) {
			break;
		}
		KAL_Udelay(1);
	} while (greset.b.txfflsh_r_ws_sc == 1);

	/* Wait for 3 PHY Clocks */
	KAL_Udelay(1);
}

/*
 * Flush a Rx FIFO.
 */
void
dwc_otg_flush_rx_fifo(void)
{
	grstctl_t greset = {0};
	int count = 0;

	dwc_otg_debug("flush rx_fifo\n");
	greset.b.rxfflsh_rw = 1;
	DWC_WRITE_REG32(DOTG_GRSTCTL, greset.d32);

	do {
		greset.d32 = DWC_READ_REG32(DOTG_GRSTCTL);
		if (++count > 10000) {
			break;
		}
	} while (greset.b.rxfflsh_rw == 1);

	/* Wait for 3 PHY Clocks */
	KAL_Udelay(1);
}

static void
dwc_otg_ep0_start_transfer(dwc_otg_pcd_ep_t *ep)
{
	depctl_data_t depctl;
	deptsiz0_data_t deptsiz;
	uint32_t epctl_reg, epctltsize_reg;
	uint32_t ep_mps = ep->maxpacket;
	uint8_t ep_num = ep->num;

	ep->total_len = ep->xfer_len;

	if (ep->is_in) {
		depctl.d32 = DWC_READ_REG32(DOTG_DIEPCTL(0));
		if (depctl.b.epena_r_ws_sc) {
			return;
		}

		epctl_reg = DOTG_DIEPCTL(ep_num);
		epctltsize_reg = DOTG_DIEPTSIZ(ep_num);

		/* If dedicated FIFO every time flush fifo before enable ep */
		dwc_otg_flush_tx_fifo(ep_num);

		depctl.d32 = DWC_READ_REG32(epctl_reg);
		deptsiz.d32 = DWC_READ_REG32(epctltsize_reg);

		/* Zero Length Packet */
		if (ep->xfer_len == 0) {
			deptsiz.b.xfersize_rw = (unsigned)(ep->is_in ? (0) : (ep_mps));
			deptsiz.b.pktcnt_rw = 1;
		} else {
			if (ep->xfer_len > ep->maxpacket) {
				ep->xfer_len = ep->maxpacket;
				deptsiz.b.xfersize_rw = ep->maxpacket;
			} else {
				deptsiz.b.xfersize_rw = ep->xfer_len;
			}
			deptsiz.b.pktcnt_rw = 1;
		}

		usb_dma_cache_flush((void *)ep->dma_addr, ep->xfer_len);
		/* Write the DMA register */
		DWC_WRITE_REG32(epctltsize_reg, deptsiz.d32);
		DWC_WRITE_REG32(DOTG_DIEPDMA(ep_num), ep->dma_addr + USB_DMA_OFFSET);

		/* EP enable */
		depctl.b.cnak_w = 1;
		depctl.b.epena_r_ws_sc = 1;
		DWC_WRITE_REG32 (epctl_reg, depctl.d32);

		if (ep->xfer_len == 0) {
			KAL_Udelay(200);
		}
	} else {
		epctl_reg = DOTG_DOEPCTL(ep_num);
		epctltsize_reg = DOTG_DOEPTSIZ(ep_num);

		depctl.d32 = DWC_READ_REG32(epctl_reg);
		deptsiz.d32 = DWC_READ_REG32(epctltsize_reg);

		deptsiz.b.xfersize_rw = ep->maxpacket;
		deptsiz.b.pktcnt_rw = 1;
		deptsiz.b.supcnt_rw = 3;

		usb_dma_cache_flush((void *)ep->dma_addr, ep->xfer_len);
		DWC_WRITE_REG32(epctltsize_reg, deptsiz.d32);
		DWC_WRITE_REG32(DOTG_DOEPDMA(ep_num), (uint32_t)ep->dma_addr + USB_DMA_OFFSET);

		/* EP enable */
		depctl.b.cnak_w = 1;
		depctl.b.epena_r_ws_sc = 1;
		DWC_WRITE_REG32 (epctl_reg, depctl.d32);
	}
}

void
dwc_otg_epn_start_transfer(dwc_otg_pcd_ep_t *ep)
{
	depctl_data_t depctl;
	deptsiz_data_t deptsiz;
	int epctl_reg;
	int epctltsize_reg;
	int ep_num = ep->num;

	if (ep->is_in) {
		epctl_reg = DOTG_DIEPCTL(ep_num);
		epctltsize_reg = DOTG_DIEPTSIZ(ep_num);

		depctl.d32 = DWC_READ_REG32(epctl_reg);
		deptsiz.d32 = DWC_READ_REG32(epctltsize_reg);

		ep->xfer_len +=
		    (DEPSZ_MAX_PKT_CNT * ep->maxpacket < (ep->total_len - ep->xfer_len)) ?
		    DEPSZ_MAX_PKT_CNT * ep->maxpacket : (ep->total_len - ep->xfer_len);

		/* Zero Length Packet */
		if ((ep->xfer_len - ep->xfer_count) == 0) {
			deptsiz.b.xfersize_rw = 0;
			deptsiz.b.pktcnt_rw = 1;
		} else {
			/*
			 * Program the transfer size and packet count
			 * as follows: xfersize = N * maxpacket +
			 * short_packet pktcnt = N + (short_packet
			 * exist ? 1 : 0)
			 */
			deptsiz.b.xfersize_rw = ep->xfer_len - ep->xfer_count;
			deptsiz.b.pktcnt_rw =
			    (ep->xfer_len - ep->xfer_count - 1 + ep->maxpacket) / ep->maxpacket;
			if (ep->xfer_type == USB_EP_TYPE_ISOC)
				deptsiz.b.mc_rw = deptsiz.b.pktcnt_rw;
		}

		usb_dma_cache_flush((void *)ep->dma_addr, ep->xfer_len);

		/* Write the DMA register */
		if (ep->xfer_type != USB_EP_TYPE_ISOC)
			deptsiz.b.mc_rw = 1;
		DWC_WRITE_REG32(epctltsize_reg, deptsiz.d32);
		DWC_WRITE_REG32(DOTG_DIEPDMA(ep_num), ep->dma_addr + USB_DMA_OFFSET);

		if (ep->xfer_type == USB_EP_TYPE_ISOC) {
			/*
			 * For SYNOPSYS DWC USB OTG isochronous transfer,
			 * we need to update PID0/PID1 according to `soffn in the DSTS
			 */
			dsts_data_t sts;
			uint32_t soffn;
			if (ep->bInterval == 1) {
				sts.d32 = DWC_READ_REG32(DOTG_DSTS);
				soffn = sts.b.soffn_r;
				soffn += 1;

				if (soffn > 0x3FFF) {
					ep->frm_overrun = 1;
					soffn &= 0x3FFF;
				} else {
					ep->frm_overrun = 0;
				}

				if (soffn & 0x1) {
					depctl.b.setd1pid_w = 0x1;
					depctl.b.setd0pid_w = 0x0;
				} else {
					depctl.b.setd1pid_w = 0x0;
					depctl.b.setd0pid_w = 0x1;
				}
			}
		}

		/* EP enable */
		depctl.b.cnak_w = 1;
		depctl.b.epena_r_ws_sc = 1;
		DWC_WRITE_REG32 (epctl_reg, depctl.d32);

	} else {
		epctl_reg = DOTG_DOEPCTL(ep_num);
		epctltsize_reg = DOTG_DOEPTSIZ(ep_num);

		depctl.d32 = DWC_READ_REG32(epctl_reg);
		deptsiz.d32 = DWC_READ_REG32(epctltsize_reg);

		ep->xfer_len += (DEPSZ_MAX_PKT_CNT * ep->maxpacket < (ep->total_len - ep->xfer_len)) ?
		    DEPSZ_MAX_PKT_CNT * ep->maxpacket : (ep->total_len - ep->xfer_len);

		if ((ep->xfer_len - ep->xfer_count) == 0) {
			/* Zero Length Packet */
			deptsiz.b.xfersize_rw = ep->maxpacket;
			deptsiz.b.pktcnt_rw = 1;
		} else {
			deptsiz.b.pktcnt_rw = (ep->xfer_len - ep->xfer_count + (ep->maxpacket - 1)) / ep->maxpacket;
			ep->xfer_len = deptsiz.b.pktcnt_rw * ep->maxpacket + ep->xfer_count;
			deptsiz.b.xfersize_rw = ep->xfer_len - ep->xfer_count;
		}

		usb_dma_cache_flush((void *)ep->dma_addr, ep->xfer_len);
		DWC_WRITE_REG32(epctltsize_reg, deptsiz.d32);
		DWC_WRITE_REG32(DOTG_DOEPDMA(ep_num), (uint32_t)ep->dma_addr + USB_DMA_OFFSET);

		if (ep->xfer_type == USB_EP_TYPE_ISOC) {
			dsts_data_t sts = { .d32 = 0 };
			if (ep->bInterval == 1) {
				sts.d32 = DWC_READ_REG32(DOTG_DSTS);
				ep->frame_num = sts.b.soffn_r + ep->bInterval;
				if (ep->frame_num > 0x3FFF) {
					ep->frm_overrun = 1;
					ep->frame_num &= 0x3FFF;
				} else {
					ep->frm_overrun = 0;
				}

				if (ep->frame_num & 0x1) {
					depctl.b.setd1pid_w = 1;
					depctl.b.setd0pid_w = 0;
				} else {
					depctl.b.setd0pid_w = 1;
					depctl.b.setd1pid_w = 0;
				}
			}
		}

		/* EP enable */
		depctl.b.cnak_w = 1;
		depctl.b.epena_r_ws_sc = 1;
		DWC_WRITE_REG32 (epctl_reg, depctl.d32);
	}
}

void
dwc_otg_ep_start_transfer(dwc_otg_pcd_ep_t *ep)
{
	if (ep->num == 0) {
		dwc_otg_ep0_start_transfer(ep);
	} else {
		dwc_otg_epn_start_transfer(ep);
	}
}

int
dwc_otg_ep_req_start(struct usbdev_ep_s *usbd_ep, struct usbdev_req_s *req)
{
	dwc_otg_pcd_ep_t *ep = NULL;
	dwc_otg_pcd_t *pcd = NULL;
	struct usb_ctrlreq_s *ctrl = NULL;
	int ep_num;
	uint32_t flags;
	uint16_t len;

	if ((usbd_ep == NULL) || (usbd_ep->ep_link == NULL) || (req == NULL)) {
		dwc_otg_debug("%s %d, bad ep, req\n ", __FUNCTION__, __LINE__);
		return (-1);
	}

	ep = (dwc_otg_pcd_ep_t *)usbd_ep->ep_link;
	ep_num = ep->num;
	pcd = (dwc_otg_pcd_t *)ep->private_data;

	if (pcd == NULL) {
		dwc_otg_debug("%s %d, bad pcd\n ", __FUNCTION__, __LINE__);
		return (-1);
	}

	req->is_complete = 0;

	KAL_SpinLockSave(&pcd->lock, &flags);

	/* EP0 Transfer */
	if (ep_num == 0) {
		switch (pcd->ep0state) {
		case EP0_IN_DATA_PHASE:
			break;

		case EP0_OUT_DATA_PHASE:
			if (pcd->request_config) {
				/* Work around for SetConfig cmd Complete STATUS PHASE */
				ep->is_in = 1;
				pcd->ep0state = EP0_IN_STATUS_PHASE;
			}
			break;

		case EP0_IN_STATUS_PHASE:
			break;

		default: {
			usb_err("---> ep0state: %d <----\n", pcd->ep0state);
			KAL_SpinUnlockRestore(&pcd->lock, flags);
			return (-1);
			}
		}

		ep->dma_addr = (uint32_t)req->buf;
		ep->start_xfer_buff = (uint8_t *)req->buf;
		ep->xfer_buff = (uint8_t *)req->buf;
		ep->xfer_len = req->len;
		ep->xfer_count = 0;
		ep->sent_zlp = 0;
		ep->total_len = ep->xfer_len;
		ep->maxpacket = 64;

		ctrl = (struct usb_ctrlreq_s *)pcd->setup_pkt;
		len = USB_GETW(ctrl->len);
		if (ep->is_in && (ep->total_len != 0) && ((ep->total_len % ep->maxpacket) == 0) &&
		    (ep->total_len != len)) {
			ep->sent_zlp = 1;
		}

		dwc_otg_ep_start_transfer(ep);
	} else {
		/* Setup and start the Transfer for Bulk */
		ep->dma_addr = (uint32_t)req->buf; /* user buffer and dma buffer is the same */
		ep->start_xfer_buff = (uint8_t *)req->buf;
		ep->xfer_buff = (uint8_t *)req->buf;
		ep->xfer_len = 0;
		ep->xfer_count = 0;
		ep->sent_zlp = 0;
		ep->total_len = req->len;

		if (req->zero) {
			ep->sent_zlp = 1;
		}
		dwc_otg_ep_start_transfer(ep);
	}

	KAL_SpinUnlockRestore(&pcd->lock, flags);
	return (0);
}

void
dwc_otg_pcd_pullup(dwc_otg_pcd_t *pcd)
{
	depctl_data_t depctl;
	volatile uint32_t depctl_addr;

	depctl.d32 = 0;
	depctl_addr = DOTG_DIEPCTL(3);
	depctl.d32 = DWC_READ_REG32(depctl_addr);
	depctl.b.setd1pid_w = 0;
	depctl.b.setd0pid_w = 1;
	DWC_WRITE_REG32(depctl_addr, depctl.d32);
}

void
dwc_otg_ep_clear_stall(dwc_otg_pcd_t *pcd, dwc_otg_pcd_ep_t *ep)
{
	depctl_data_t depctl;
	volatile uint32_t depctl_addr;

	if (ep->is_in == 1)
		depctl_addr = DOTG_DIEPCTL(ep->num);
	else
		depctl_addr = DOTG_DOEPCTL(ep->num);

	depctl.d32 = DWC_READ_REG32(depctl_addr);

	/* clear the stall bits */
	depctl.b.stall_rw = 0;

	/*
	 * USB Spec 9.4.5: For endpoints using data toggle, regardless
	 * of whether an endpoint has the Halt feature set, a
	 * ClearFeature(ENDPOINT_HALT) request always results in the
	 * data toggle being reinitialized to DATA0.
	 */
	if (ep->xfer_type == USB_EP_TYPE_INTR || ep->xfer_type == USB_EP_TYPE_BULK) {
		depctl.b.setd0pid_w = 1;    /* DATA0 */
	}

	DWC_WRITE_REG32(depctl_addr, depctl.d32);
	return;
}

void
dwc_otg_ep_set_stall(dwc_otg_pcd_t *pcd, dwc_otg_pcd_ep_t *ep)
{
	depctl_data_t depctl;
	volatile uint32_t depctl_addr;

	if (ep->is_in == 1) {
		depctl_addr = DOTG_DIEPCTL(ep->num);
		depctl.d32 = DWC_READ_REG32(depctl_addr);

		/* set the disable and stall bits */
		if (depctl.b.epena_r_ws_sc) {
			depctl.b.epdis_r_ws_sc = 1;
		}

	} else {
		depctl_addr = DOTG_DOEPCTL(ep->num);
		depctl.d32 = DWC_READ_REG32(depctl_addr);
	}
	/* set the stall bit */
	depctl.b.stall_rw = 1;

	DWC_WRITE_REG32(depctl_addr, depctl.d32);
	return;
}

int
dwc_otg_ep_halt(struct usbdev_ep_s *usbd_ep, int value)
{
	dwc_otg_pcd_ep_t *ep = NULL;
	dwc_otg_pcd_t *pcd = NULL;
	uint32_t flags;

	if ((usbd_ep == NULL) || (usbd_ep->ep_link == NULL)) {
		dwc_otg_debug("%d, bad ep\n ", __LINE__);
		return (-1);
	}

	ep = (dwc_otg_pcd_ep_t *)usbd_ep->ep_link;
	pcd = (dwc_otg_pcd_t *)ep->private_data;
	if (pcd == NULL) {
		dwc_otg_debug("%d, bad pcd\n ", __LINE__);
		return (-1);
	}

	KAL_SpinLockSave(&pcd->lock, &flags);

	if (value == 0) {
		dwc_otg_ep_clear_stall(pcd, ep);
	} else if (value == 1) {
		if (ep->num == 0) {
			pcd->ep0state = EP0_STALL;
		}
		ep->is_stopped = 1;
		dwc_otg_ep_set_stall(pcd, ep);
	}

	KAL_SpinUnlockRestore(&pcd->lock, flags);
	return (0);
}

void
dwc_otg_endpoint_activate(dwc_otg_pcd_ep_t *ep)
{
	depctl_data_t depctl = {0};
	daint_data_t daintmsk = {0};
	int epctl;
	uint8_t ep_num = ep->num;

	if (ep->is_in) {
		epctl = DOTG_DIEPCTL(ep_num);
		daintmsk.ep.in = 1 << ep_num;
	} else {
		epctl = DOTG_DOEPCTL(ep_num);
		daintmsk.ep.out = 1 << ep_num;
	}

	depctl.d32 = DWC_READ_REG32(epctl);
	if (!depctl.b.usbactep_r_w_sc) {
		depctl.b.mps_rw = ep->maxpacket;
		depctl.b.eptype_rw = ep->xfer_type;
		depctl.b.setd0pid_w = 1;
		depctl.b.txfnum_rw = ep->fifo_index;
		depctl.b.usbactep_r_w_sc = 1;

		DWC_WRITE_REG32(epctl, depctl.d32);
	}

	DWC_MODIFY_REG32(DOTG_DAINTMSK, 0, daintmsk.d32);
	return;
}

void
dwc_otg_wait_disable_in_endpoint(uint32_t epnum, uint32_t retry)
{
	depctl_data_t diepctl;
	diepctl.d32 = DWC_READ_REG32(DOTG_DIEPCTL(epnum));

	/* Check endpoint is enable status */
	if (diepctl.b.epena_r_ws_sc) {
		uint32_t do_try = 0;
		diepint_data_t diepint = { .d32 = 0 };
		diepctl.b.snak_w = 1;
		DWC_WRITE_REG32(DOTG_DIEPCTL(epnum), diepctl.d32);

		/* Check endpoint NAK */
		do {
			KAL_Udelay(10);
			diepint.d32 = DWC_READ_REG32(DOTG_DIEPINT(epnum));
			do_try++;
		} while ((!diepint.b.inepnakeff_r_ss_wc) && (do_try < retry));

		diepint.b.inepnakeff_r_ss_wc = 1;
		DWC_WRITE_REG32(DOTG_DIEPINT(epnum), diepint.d32);

		diepctl.d32 = 0;
		diepctl.b.epdis_r_ws_sc = 1;
		DWC_WRITE_REG32(DOTG_DIEPCTL(epnum), diepctl.d32);

		do_try = 0;

		/* Check endpoint Disable */
		do {
			KAL_Udelay(10);
			diepint.d32 = DWC_READ_REG32(DOTG_DIEPINT(epnum));
			do_try++;
		} while ((!diepint.b.epdisabled_r_ss_wc) && (do_try < retry));

		diepint.b.epdisabled_r_ss_wc = 1;
		DWC_WRITE_REG32(DOTG_DIEPINT(epnum), diepint.d32);
	}
}

void
dwc_otg_endpoint_deactivate(dwc_otg_pcd_ep_t *ep)
{
	depctl_data_t depctl = { .d32 = 0 };
	daint_data_t daintmsk = { .d32 = 0 };
	dcfg_data_t dcfg;
	int epctl;
	int is_in;
	uint8_t ep_num;

	if (ep == NULL) {
		return;
	}

	is_in = ep->is_in;
	ep_num = ep->num;

	/* Read DEPCTLn register */
	if (is_in) {
		epctl = DOTG_DIEPCTL(ep_num);
		daintmsk.ep.in = 1 << ep_num;
	} else {
		epctl = DOTG_DOEPCTL(ep_num);
		daintmsk.ep.out = 1 << ep_num;
	}

	depctl.d32 = DWC_READ_REG32(epctl);
	depctl.b.usbactep_r_w_sc = 0;

	/* Update nextep_seq array and EPMSCNT in DCFG */
	if (!(depctl.b.eptype_rw & 1) && is_in) {   /* NP EP IN */
		depctl.b.nextep_rw = 0;
		dcfg.d32 = DWC_READ_REG32(DOTG_DCFG);
		dcfg.b.epmscnt_rw--;
		DWC_WRITE_REG32(DOTG_DCFG, dcfg.d32);
	}

	if (is_in)
		depctl.b.txfnum_rw = 0;

	DWC_WRITE_REG32(epctl, depctl.d32);
	depctl.d32 = DWC_READ_REG32(epctl);

	/* CHANGE to disable the IN ep */
	if (depctl.b.epena_r_ws_sc) {
		depctl.d32 = 0;
		if (is_in) {
			dwc_otg_wait_disable_in_endpoint(ep_num, (~0));
		}
	}

	/* Disable the Interrupt for this EP */
	DWC_MODIFY_REG32(DOTG_DAINTMSK, daintmsk.d32, 0);
}

int
dwc_otg_endpoint_disable(struct usbdev_ep_s *ep)
{
	depctl_data_t depctl = {0};
	uint32_t flags;
	dwc_otg_pcd_ep_t *pcd_ep = NULL;
	dwc_otg_pcd_t *pcd = NULL;
	int epctl;
	int is_in;
	int ep_num;

	if ((ep == NULL) || (ep->ep_link == NULL)) {
		return (-1);
	}
	pcd_ep = (dwc_otg_pcd_ep_t *)ep->ep_link;

	if (pcd_ep == NULL) {
		dwc_otg_debug("%s %d, bad pcd_ep\n ", __FUNCTION__, __LINE__);
		return (-1);
	}

	pcd = (dwc_otg_pcd_t *)pcd_ep->private_data;
	if (pcd == NULL) {
		return (-1);
	}

	is_in = pcd_ep->is_in;
	ep_num = pcd_ep->num;

	if (is_in) {
		epctl = DOTG_DIEPCTL(ep_num);
	} else {
		epctl = DOTG_DOEPCTL(ep_num);
		/* Flush the FIFOs */
		dwc_otg_flush_tx_fifo(0x10); /* all Tx FIFOs */
	}

	KAL_SpinLockSave(&pcd->lock, &flags);
	pcd->fifo_map &= ~(1u << pcd_ep->fifo_index);
	pcd_ep->fifo_size = 0;

	depctl.d32 = DWC_READ_REG32(epctl);
	if (depctl.b.epena_r_ws_sc == 1) {
		dwc_otg_wait_disable_in_endpoint(ep_num, 100);
	}

	dwc_otg_endpoint_deactivate(pcd_ep);

	KAL_SpinUnlockRestore(&pcd->lock, flags);
	return (0);
}

static uint32_t
dwc_otg_endpoint_mc_get(dwc_otg_pcd_ep_t *ep, const struct usb_epdesc_s *desc)
{
#define DXEPCTL_MPS_MASK	0x7ff
	uint32_t mps = USB_GETW(desc->mxpacketsize);
	uint32_t mps_val;
	uint32_t ep_num = ep->num;
	uint32_t mc;

	ep->maxpacket = mps;
	if (ep_num != 0) {
		mps_val = mps & DXEPCTL_MPS_MASK;
		if (mps_val > 1024) { /* 1024 is maximum packet size */
			return (0);
		}
		mps_val = ((mps >> 11) & 0x3) + 1; /* Get multi count */
		mc = mps_val;
		if (mc > 3) { /* multi count can not bigger than 3 */
			return (0);
		}
	} else {
		mc = 1;
	}

	return (mc);
}

static int
dwc_otg_endpoint_fifo_index_get(dwc_otg_pcd_ep_t *ep, uint32_t mc)
{
#define FIFO_INVALID_SIZE	0xffffffff
	dwc_otg_pcd_t *pcd = (dwc_otg_pcd_t *)ep->private_data;
	uint32_t fifo_size;
	uint32_t size;
	uint32_t min_fifo_size = FIFO_INVALID_SIZE;
	uint8_t index = 0;
	uint8_t i;
	uint8_t num_eps = pcd->num_in_eps;

	size = ep->maxpacket * mc;
	for (i = 1; i <= num_eps; i++) {
		if (pcd->fifo_map & (1 << i)) {
			continue;
		}

		fifo_size = DWC_READ_REG32(DOTG_DPTXFSIZ(i));
		fifo_size = (fifo_size >> 16) * 4; /* Get fifo size */
		if (fifo_size < size) {
			continue;
		} else {
			if (min_fifo_size > fifo_size) {
				min_fifo_size = fifo_size;
				index = i;
			}
		}
	}

	if (!index) {
		return (-1);
	}

	pcd->fifo_map |= 1 << index;
	ep->fifo_index = index;
	ep->fifo_size = min_fifo_size;
	dwc_otg_debug("fifo_index:%u, fifo_size:%u\n", ep->fifo_index, ep->fifo_size);

	return (0);
}

int
dwc_otg_endpoint_enable(struct usbdev_ep_s *ep, const struct usb_epdesc_s *desc)
{
	dwc_otg_pcd_ep_t *pcd_ep = NULL;
	int is_in;
	uint8_t ep_num;
	depctl_data_t depctl_tmp;
	uint32_t addr_tmp;
	depctl_data_t depctl = {0};
	daint_data_t daintmsk = {0};
	dctl_data_t dctl;
	hwcfg4_data_t hwcfg4_data;
	int epctl;
	uint32_t mc;

	if (ep == NULL) {
		dwc_otg_debug("%s %d, bad ep\n ", __FUNCTION__, __LINE__);
		return (-1);
	}

	pcd_ep = ep->ep_link;

	if ((desc == NULL) || (pcd_ep == NULL)) {
		dwc_otg_debug("%s %d, bad desc or  pcd_ep\n ", __FUNCTION__, __LINE__);
		return (-1);
	}

	is_in = pcd_ep->is_in;
	ep_num = pcd_ep->num;
	pcd_ep->xfer_type = desc->attr & USB_EP_ATTR_XFERTYPE_MASK;
	mc = dwc_otg_endpoint_mc_get(pcd_ep, desc);
	if (!mc) {
		usb_err("endpoint mc get failed!\n");
		return (-1);
	}
	ep->maxpacket = pcd_ep->maxpacket;

	if (pcd_ep->xfer_type == USB_EP_TYPE_ISOC) {
		pcd_ep->bInterval = 1 << (desc->interval - 1);
		pcd_ep->frame_num = 0xFFFFFFFF;
	}

	if (is_in) {
		epctl = DOTG_DIEPCTL(ep_num);
		daintmsk.ep.in = 1 << ep_num;
	} else {
		epctl = DOTG_DOEPCTL(ep_num);
		daintmsk.ep.out = 1 << ep_num;
	}

	/* If the EP is already active don't change the EP Control register. */
	depctl.d32 = DWC_READ_REG32(epctl);
	if (!depctl.b.usbactep_r_w_sc) {
		if (pcd_ep->xfer_type == USB_EP_TYPE_ISOC) {
			addr_tmp = DOTG_DIEPCTL(ep_num);
			depctl_tmp.d32 = DWC_READ_REG32(addr_tmp);
			if (depctl_tmp.b.eptype_rw == 0x1) {
				dctl.d32 = DWC_READ_REG32(DOTG_DCTL);
				dctl.b.ifrmnum = 1;
				DWC_MODIFY_REG32(DOTG_DCTL, dctl.d32, dctl.d32);
			}
		}

		hwcfg4_data.d32 = DWC_READ_REG32(DOTG_GHWCFG4);
		if (hwcfg4_data.b.ded_fifo_en_r && is_in) {
			if (dwc_otg_endpoint_fifo_index_get(pcd_ep, mc)) {
				return (-1);
			}
			depctl.b.txfnum_rw = pcd_ep->fifo_index;
			dwc_otg_flush_tx_fifo(pcd_ep->fifo_index);
		}

		depctl.b.mps_rw = pcd_ep->maxpacket;
		depctl.b.eptype_rw = pcd_ep->xfer_type;
		depctl.b.setd0pid_w = 1;
		depctl.b.usbactep_r_w_sc = 1;

		DWC_WRITE_REG32(epctl, depctl.d32);
	}

	DWC_MODIFY_REG32(DOTG_DAINTMSK, 0, daintmsk.d32);
	return (0);
}

void
dwc_otg_power_off_phy(void)
{
	DWC_WRITE_REG32(DOTG_PCGCCTL, 0xF);
}

void
DWC_WRITE_REG32(uint32_t x, uint32_t v)
{
	KAL_WRITE32_MB(v, x + DWC_REG_BASE);
	dwc_otg_debug("(0x%x,0x%x)\n ", x + DWC_REG_BASE, v);
}

uint32_t
DWC_READ_REG32(uint32_t x)
{
	return (KAL_READ32_MB(x + DWC_REG_BASE));
}

void
DWC_MODIFY_REG32(uint32_t x, uint32_t c, uint32_t s)
{
	uint32_t v = (((KAL_READ32_MB(x + DWC_REG_BASE)) & (~c)) | (s));
	KAL_WRITE32_MB(v, x + DWC_REG_BASE);
	dwc_otg_debug("(0x%x,0x%x)\n ", x + DWC_REG_BASE, v);
}

#ifdef LOSCFG_USB_DEBUG
void
dwc_otg_dump_global_registers(void)
{
	uintptr_t addr;

	PCDSHOW("\tCore Global Registers\n");
	addr = DWC_REG_BASE + DOTG_GOTGCTL;
	PCDSHOW("\tGOTGCTL     @0x%08lX : 0x%08X\n", (unsigned long)addr, DWC_READ_REG32(addr - DWC_REG_BASE));
	addr = DWC_REG_BASE + DOTG_GOTGINT;
	PCDSHOW("\tGOTGINT     @0x%08lX : 0x%08X\n", (unsigned long)addr, DWC_READ_REG32(addr - DWC_REG_BASE));
	addr = DWC_REG_BASE + DOTG_GAHBCFG;
	PCDSHOW("\tGAHBCFG     @0x%08lX : 0x%08X\n", (unsigned long)addr, DWC_READ_REG32(addr - DWC_REG_BASE));
	addr = DWC_REG_BASE + DOTG_GUSBCFG;
	PCDSHOW("\tGUSBCFG     @0x%08lX : 0x%08X\n", (unsigned long)addr, DWC_READ_REG32(addr - DWC_REG_BASE));
	addr = DWC_REG_BASE + DOTG_GRSTCTL;
	PCDSHOW("\tGRSTCTL     @0x%08lX : 0x%08X\n", (unsigned long)addr, DWC_READ_REG32(addr - DWC_REG_BASE));
	addr = DWC_REG_BASE + DOTG_GINTSTS;
	PCDSHOW("\tGINTSTS     @0x%08lX : 0x%08X\n", (unsigned long)addr, DWC_READ_REG32(addr - DWC_REG_BASE));
	addr = DWC_REG_BASE + DOTG_GINTMSK;
	PCDSHOW("\tGINTMSK     @0x%08lX : 0x%08X\n", (unsigned long)addr, DWC_READ_REG32(addr - DWC_REG_BASE));
	addr = DWC_REG_BASE + DOTG_GRXSTSRD;
	PCDSHOW("\tGRXSTSR     @0x%08lX : 0x%08X\n", (unsigned long)addr, DWC_READ_REG32(addr - DWC_REG_BASE));
	addr = DWC_REG_BASE + DOTG_GRXFSIZ;
	PCDSHOW("\tGRXFSIZ     @0x%08lX : 0x%08X\n", (unsigned long)addr, DWC_READ_REG32(addr - DWC_REG_BASE));
	addr = DWC_REG_BASE + DOTG_GNPTXFSIZ;
	PCDSHOW("\tGNPTXFSIZ   @0x%08lX : 0x%08X\n", (unsigned long)addr, DWC_READ_REG32(addr - DWC_REG_BASE));
	addr = DWC_REG_BASE + DOTG_GNPTXSTS;
	PCDSHOW("\tGNPTXSTS    @0x%08lX : 0x%08X\n", (unsigned long)addr, DWC_READ_REG32(addr - DWC_REG_BASE));
	addr = DWC_REG_BASE + DOTG_GI2CCTL;
	PCDSHOW("\tGI2CCTL     @0x%08lX : 0x%08X\n", (unsigned long)addr, DWC_READ_REG32(addr - DWC_REG_BASE));
	addr = DWC_REG_BASE + DOTG_GPVNDCTL;
	PCDSHOW("\tGPVNDCTL    @0x%08lX : 0x%08X\n", (unsigned long)addr, DWC_READ_REG32(addr - DWC_REG_BASE));
	addr = DWC_REG_BASE + DOTG_GGPIO;
	PCDSHOW("\tGGPIO       @0x%08lX : 0x%08X\n", (unsigned long)addr, DWC_READ_REG32(addr - DWC_REG_BASE));
	addr = DWC_REG_BASE + DOTG_GUID;
	PCDSHOW("\tGUID        @0x%08lX : 0x%08X\n", (unsigned long)addr, DWC_READ_REG32(addr - DWC_REG_BASE));
	addr = DWC_REG_BASE + DOTG_GSNPSID;
	PCDSHOW("\tGSNPSID     @0x%08lX : 0x%08X\n", (unsigned long)addr, DWC_READ_REG32(addr - DWC_REG_BASE));
	addr = DWC_REG_BASE + DOTG_GHWCFG1;
	PCDSHOW("\tGHWCFG1     @0x%08lX : 0x%08X\n", (unsigned long)addr, DWC_READ_REG32(addr - DWC_REG_BASE));
	addr = DWC_REG_BASE + DOTG_GHWCFG2;
	PCDSHOW("\tGHWCFG2     @0x%08lX : 0x%08X\n", (unsigned long)addr, DWC_READ_REG32(addr - DWC_REG_BASE));
	addr = DWC_REG_BASE + DOTG_GHWCFG3;
	PCDSHOW("\tGHWCFG3     @0x%08lX : 0x%08X\n", (unsigned long)addr, DWC_READ_REG32(addr - DWC_REG_BASE));
	addr = DWC_REG_BASE + DOTG_GHWCFG4;
	PCDSHOW("\tGHWCFG4     @0x%08lX : 0x%08X\n", (unsigned long)addr, DWC_READ_REG32(addr - DWC_REG_BASE));
	addr = DWC_REG_BASE + DOTG_GLPMCFG;
	PCDSHOW("\tGLPMCFG     @0x%08lX : 0x%08X\n", (unsigned long)addr, DWC_READ_REG32(addr - DWC_REG_BASE));
	addr = DWC_REG_BASE + DOTG_GPWRDN;
	PCDSHOW("\tGPWRDN      @0x%08lX : 0x%08X\n", (unsigned long)addr, DWC_READ_REG32(addr - DWC_REG_BASE));
	addr = DWC_REG_BASE + DOTG_GDFIFOCFG;
	PCDSHOW("\tGDFIFOCFG   @0x%08lX : 0x%08X\n", (unsigned long)addr, DWC_READ_REG32(addr - DWC_REG_BASE));
	addr = DWC_REG_BASE + DOTG_HPTXFSIZ;
	PCDSHOW("\tHPTXFSIZ    @0x%08lX : 0x%08X\n", (unsigned long)addr, DWC_READ_REG32(addr - DWC_REG_BASE));
}

void dwc_otg_dump_dev_registers(void)
{
	int i;
	uintptr_t addr;

	PCDSHOW("\tDevice Global Registers\n");
	addr = DWC_REG_BASE + DOTG_DCFG;
	PCDSHOW("\tDCFG        @0x%08lX : 0x%08X\n", (unsigned long)addr, DWC_READ_REG32(addr - DWC_REG_BASE));
	addr = DWC_REG_BASE + DOTG_DCTL;
	PCDSHOW("\tDCTL        @0x%08lX : 0x%08X\n", (unsigned long)addr, DWC_READ_REG32(addr - DWC_REG_BASE));
	addr = DWC_REG_BASE + DOTG_DSTS;
	PCDSHOW("\tDSTS        @0x%08lX : 0x%08X\n", (unsigned long)addr, DWC_READ_REG32(addr - DWC_REG_BASE));
	addr = DWC_REG_BASE + DOTG_DIEPMSK;
	PCDSHOW("\tDIEPMSK     @0x%08lX : 0x%08X\n", (unsigned long)addr, DWC_READ_REG32(addr - DWC_REG_BASE));
	addr = DWC_REG_BASE + DOTG_DOEPMSK;
	PCDSHOW("\tDOEPMSK     @0x%08lX : 0x%08X\n", (unsigned long)addr, DWC_READ_REG32(addr - DWC_REG_BASE));
	addr = DWC_REG_BASE + DOTG_DAINT;
	PCDSHOW("\tDAINT       @0x%08lX : 0x%08X\n", (unsigned long)addr, DWC_READ_REG32(addr - DWC_REG_BASE));
	addr = DWC_REG_BASE + DOTG_DAINTMSK;
	PCDSHOW("\tDAINTMSK    @0x%08lX : 0x%08X\n", (unsigned long)addr, DWC_READ_REG32(addr - DWC_REG_BASE));
	addr = DWC_REG_BASE + DOTG_DTKNQR1;
	PCDSHOW("\tDTKNQR1     @0x%08lX : 0x%08X\n", (unsigned long)addr, DWC_READ_REG32(addr - DWC_REG_BASE));
	addr = DWC_REG_BASE + DOTG_DVBUSDIS;
	PCDSHOW("\tDVBUSID     @0x%08lX : 0x%08X\n", (unsigned long)addr, DWC_READ_REG32(addr - DWC_REG_BASE));

	addr = DWC_REG_BASE + DOTG_DVBUSPULSE;
	PCDSHOW("\tDVBUSPULSE  @0x%08lX : 0x%08X\n", (unsigned long)addr, DWC_READ_REG32(addr - DWC_REG_BASE));

	addr = DWC_REG_BASE + DOTG_DTHRCTL;
	PCDSHOW("\tDTKNQR3_DTHRCTL @0x%08lX : 0x%08X\n", (unsigned long)addr, DWC_READ_REG32(addr - DWC_REG_BASE));

	addr = DWC_REG_BASE + DOTG_DTKNQR4;
	PCDSHOW("\tFIFOEMPMSK  @0x%08lX : 0x%08X\n", (unsigned long)addr, DWC_READ_REG32(addr - DWC_REG_BASE));

	for (i = 0; i <= 1; i++) {
		PCDSHOW("\tDevice IN EP %d Registers\n", i);
		addr = DWC_REG_BASE + DOTG_DIEPCTL(i);
		PCDSHOW("\tDIEPCTL     @0x%08lX : 0x%08X\n", (unsigned long)addr, DWC_READ_REG32(addr - DWC_REG_BASE));
		addr = DWC_REG_BASE + DOTG_DIEPINT(i);
		PCDSHOW("\tDIEPINT     @0x%08lX : 0x%08X\n", (unsigned long)addr, DWC_READ_REG32(addr - DWC_REG_BASE));
		addr = DWC_REG_BASE + DOTG_DIEPTSIZ(i);
		PCDSHOW("\tDIETSIZ     @0x%08lX : 0x%08X\n", (unsigned long)addr, DWC_READ_REG32(addr - DWC_REG_BASE));
		addr = DWC_REG_BASE + DOTG_DIEPDMA(i);
		PCDSHOW("\tDIEPDMA     @0x%08lX : 0x%08X\n", (unsigned long)addr, DWC_READ_REG32(addr - DWC_REG_BASE));
		addr = DWC_REG_BASE + DOTG_DTXFSTS(i);
		PCDSHOW("\tDTXFSTS     @0x%08lX : 0x%08X\n", (unsigned long)addr, DWC_READ_REG32(addr - DWC_REG_BASE));
		addr = DWC_REG_BASE + DOTG_DIEPDMAB(i);
		PCDSHOW("\tDIEPDMAB    @0x%08lX : 0x%08X\n", (unsigned long)addr, 0);
	}

	for (i = 0; i <= 1; i++) {
		PCDSHOW("\tDevice OUT EP %d Registers\n", i);
		addr = DWC_REG_BASE + DOTG_DOEPCTL(i);
		PCDSHOW("\tDOEPCTL     @0x%08lX : 0x%08X\n", (unsigned long)addr, DWC_READ_REG32(addr - DWC_REG_BASE));
		addr = DWC_REG_BASE + DOTG_DOEPINT(i);
		PCDSHOW("\tDOEPINT     @0x%08lX : 0x%08X\n", (unsigned long)addr, DWC_READ_REG32(addr - DWC_REG_BASE));
		addr = DWC_REG_BASE + DOTG_DOEPTSIZ(i);
		PCDSHOW("\tDOETSIZ     @0x%08lX : 0x%08X\n", (unsigned long)addr, DWC_READ_REG32(addr - DWC_REG_BASE));
		addr = DWC_REG_BASE + DOTG_DOEPDMA(i);
		PCDSHOW("\tDOEPDMA     @0x%08lX : 0x%08X\n", (unsigned long)addr, DWC_READ_REG32(addr - DWC_REG_BASE));
	}
}
#endif
