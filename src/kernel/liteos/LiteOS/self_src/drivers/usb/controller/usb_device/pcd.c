/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2014-2021. All rights reserved.
 * Description: LiteOS USB PCD Implementation
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

#include "controller/usb_device/pcd.h"
#include "controller/usb_device/os_defs.h"
#include "controller/usb_device/hw.h"
#include "controller/usb_device/dev.h"
#include "controller/usb_device/os_dev.h"
#include "controller/usb_device/cil.h"
#include "usb_os_adapt.h"

#define dwc3_pcd_show(x...) dprintf(x)

#ifdef DWC3_PCD_DEBUG
#define dwc3_pcd_debug(x...) dwc3_pcd_show(x)
#else
#define dwc3_pcd_debug(x...) do {} while (0)
#endif

#define DWC_USB3_ALIGN_FRAME(uf_tar, intvl, i)	(((uf_tar) + ((intvl) * (i))) & ~((intvl) - 1))

usb_dwc_dma_desc_t *
dwc_usb3_pcd_trb_alloc(usb_dwc_pcd_ep_t *ep, int num_trbs,
    int trb_type, int iso_intvl, int linked, usb_dwc_dma_t *trbs_dma_ret)
{
	usb_dwc_dma_desc_t *trbs = NULL;
	usb_dwc_dma_desc_t *cur_trb = NULL;
	usb_dwc_dma_t trbs_dma;
	int size, i;

	size = num_trbs * 16;
	if (linked)
		size += 16;

	trbs = cur_trb = dwc_usb3_gadget_alloc_dma(ep, size, &trbs_dma);
	if (trbs == NULL)
		return (NULL);

	for (i = 0; i < num_trbs; i++, cur_trb++) {
		if (trb_type == USB_EP_ATTR_XFER_ISOC) {
			dwc_usb3_fill_desc(cur_trb, 0, 0, 0,
			    USB_DWC_DSCCTL_TRBCTL_ISOC_1ST,
			    USB_DWC_DSCCTL_IOC_BIT |
			    USB_DWC_DSCCTL_IMI_BIT |
			    USB_DWC_DSCCTL_CSP_BIT, 0);
		}

		/* For types other than Isoc, the TRBs are initialized just
		 * before the transfer is started.
		 */
	}

	if (linked) {
		dwc_usb3_fill_desc(cur_trb, trbs_dma, 0, 0,
		    USB_DWC_DSCCTL_TRBCTL_LINK, 0, 1);
	}

	ep->dwc_ep.dma_desc = trbs;
	ep->dwc_ep.dma_desc_dma = trbs_dma;
	ep->dwc_ep.desc_size = size;
	ep->dwc_ep.desc_link = linked;
	ep->dwc_ep.num_desc = num_trbs;
	ep->dwc_ep.desc_avail = num_trbs;
	ep->dwc_ep.desc_idx = 0;
	ep->dwc_ep.hiber_desc_idx = 0;

	if (trbs_dma_ret != NULL)
		*trbs_dma_ret = trbs_dma;
	return (trbs);
}

void
dwc_usb3_pcd_trb_free(usb_dwc_pcd_ep_t *ep)
{
	usb_dwc_dma_desc_t *trbs = NULL;
	usb_dwc_dma_t trbs_dma;
	int size;

	if (ep->dwc_ep.dma_desc) {
		trbs = ep->dwc_ep.dma_desc;
		trbs_dma = ep->dwc_ep.dma_desc_dma;
		size = ep->dwc_ep.desc_size;
		ep->dwc_ep.dma_desc = NULL;
		ep->dwc_ep.dma_desc_dma = 0;

		dwc_usb3_gadget_free_dma(ep, size, trbs, trbs_dma);
	}
}

void
dwc_usb3_pcd_fill_trbs(usb_dwc_pcd_t *pcd, usb_dwc_pcd_ep_t *ep,
    usb_dwc_pcd_req_t *req)
{
	usb_dwc_dma_desc_t *desc = NULL;
	usb_dwc_dma_t desc_dma;
	uint32_t length, tlen, packets, ctrl;
	uint32_t ctrl_type;
	int index;

	if (ep == pcd->ep0)
		return;

	if ((ep->dwc_ep.desc_idx + req->dwc_req.numbuf) > ep->dwc_ep.num_desc) {
		ep->dwc_ep.desc_idx = 0;
	}
	desc = ep->dwc_ep.dma_desc + ep->dwc_ep.desc_idx;
	desc_dma = (usb_dwc_dma_t)((unsigned long)ep->dwc_ep.dma_desc_dma +
	    (unsigned long)ep->dwc_ep.desc_idx * 16);

	ep->dwc_ep.desc_idx += req->dwc_req.numbuf;

	req->dwc_req.trb = desc;
	req->dwc_req.trbdma = desc_dma;

	packets = 0;

	if (ep->dwc_ep.is_in) {
		length = req->dwc_req.length;

		if ((ep->dwc_ep.type == USB_EP_ATTR_XFER_ISOC) && (pcd->speed == USB_SPEED_HIGH)) {
			packets = (length + ep->dwc_ep.maxpacket - 1U) / ep->dwc_ep.maxpacket;
			if (packets)
				packets--;
		}
	} else {
		if (((ep->dwc_ep.type == USB_EP_ATTR_XFER_ISOC) || (ep->dwc_ep.type == USB_EP_ATTR_XFER_INT)) &&
		    (ep->dwc_ep.maxpacket != 1024U))
			length = ((req->dwc_req.length + ep->dwc_ep.maxpacket - 1U) / ep->dwc_ep.maxpacket) *
			    ep->dwc_ep.maxpacket;
		else
			length = (req->dwc_req.length + ep->dwc_ep.maxpacket - 1U) & ~(ep->dwc_ep.maxpacket - 1U);

		req->dwc_req.length = length;
	}

	for (index = 0; index < req->dwc_req.numbuf; index++, desc++) {
		ep->dwc_ep.desc_avail--;
		if (ep->dwc_ep.type == USB_EP_ATTR_XFER_ISOC) {
			if (index != req->dwc_req.numbuf - 1) {
				dwc_usb3_start_desc_chain(desc);
				tlen = req->dwc_req.buflen[index];
				length -= tlen;
			} else {
				tlen = length;
			}

			if (index == 0) {
				ctrl = 0;
				tlen |= packets << USB_DWC_DSCSTS_PCM1_SHIFT;
			} else {
				/* Set the TRBCTL of the first TRB in the service interval to USB_DWC_DSCCTL_TRBCTL_ISOC_1ST,
				 * and the TRBCTL of the remaining TRBs to USB_DWC_DSCCTL_TRBCTL_ISOC.
				 */
				ctrl = USB_DWC_DSCCTL_TRBCTL_ISOC << USB_DWC_DSCCTL_TRBCTL_SHIFT;
			}

			dwc_usb3_fill_desc(desc, req->dwc_req.bufdma[index], tlen,
			    0, 0, ctrl, index != 0);
		} else {
			ctrl_type = USB_DWC_DSCCTL_TRBCTL_NORMAL;
			if (index != req->dwc_req.numbuf - 1) {
				ctrl = USB_DWC_DSCCTL_CHN_BIT;
				tlen = req->dwc_req.buflen[index];
				length -= tlen;
			} else {
				ctrl = USB_DWC_DSCCTL_LST_BIT;
				tlen = length;
				/* Set TRBCTL to USB_DWC_DSCCTL_TRBCTL_NORMAL_ZLP for a BULK IN TRB
				 * where a zero-length packet termination is required when completing
				 * the TRB chain(when the transfer size is an exact multiple of MaxPacketSize)
				 */
				if (ep->dwc_ep.is_in && (ep->dwc_ep.type == USB_EP_ATTR_XFER_BULK) && (req->dwc_req.numbuf > 1) &&
				    (req->dwc_req.length % ep->dwc_ep.maxpacket == 0)) {
					ctrl_type = USB_DWC_DSCCTL_TRBCTL_NORMAL_ZLP;
				}
			}

			dwc_usb3_fill_desc(desc, req->dwc_req.bufdma[index], tlen,
			    req->dwc_req.stream,
			    ctrl_type, ctrl,
			    index != 0);
		}
	}

	desc = req->dwc_req.trb;
	dwc_usb3_enable_desc(desc);
}

void
dwc_usb3_ep0_activate(usb_dwc_pcd_t *pcd_t, int restore)
{
	uint32_t diepcfg0, doepcfg0, diepcfg1, doepcfg1;
	uint32_t diepcfg2 = 0;
	uint32_t doepcfg2 = 0;
	usb_dwc_dev_ep_regs_t *ep_reg = NULL;

	dwc3_pcd_debug("%s()\n", __FUNCTION__);

	diepcfg0 = USB_DWC_USB3_EP_TYPE_CONTROL << USB_DWC_EPCFG0_EPTYPE_SHIFT;
	diepcfg1 = USB_DWC_EPCFG1_XFER_CMPL_BIT | USB_DWC_EPCFG1_XFER_IN_PROG_BIT |
	    USB_DWC_EPCFG1_XFER_NRDY_BIT | USB_DWC_EPCFG1_EP_DIR_BIT;

	doepcfg0 = USB_DWC_USB3_EP_TYPE_CONTROL << USB_DWC_EPCFG0_EPTYPE_SHIFT;
	doepcfg1 = USB_DWC_EPCFG1_XFER_CMPL_BIT | USB_DWC_EPCFG1_XFER_IN_PROG_BIT |
	    USB_DWC_EPCFG1_XFER_NRDY_BIT;

	diepcfg0 |= 512 << USB_DWC_EPCFG0_MPS_SHIFT;
	doepcfg0 |= 512 << USB_DWC_EPCFG0_MPS_SHIFT;
	diepcfg0 |= pcd_t->ep0->dwc_ep.tx_fifo_num << USB_DWC_EPCFG0_TXFNUM_SHIFT;

	if (restore) {
		diepcfg0 |= USB_DWC_CFG_ACTION_RESTORE << USB_DWC_EPCFG0_CFG_ACTION_SHIFT;
		diepcfg2 = pcd_t->ep0_in_save_state;
		dwc3_pcd_debug("IN restore state=%08x\n", diepcfg2);
		doepcfg0 |= USB_DWC_CFG_ACTION_RESTORE << USB_DWC_EPCFG0_CFG_ACTION_SHIFT;
		doepcfg2 = pcd_t->ep0_out_save_state;
		dwc3_pcd_debug("OUT restore state=%08x\n", doepcfg2);
	}

	ep_reg = &pcd_t->out_ep_regs[0];
	dwc_usb3_dis_usb2_suspend(pcd_t);

	if ((pcd_t->usb3_dev->snpsid & 0xffff) >= 0x109a) {
		(void)dwc_usb3_dep_startnewcfg(pcd_t, ep_reg, 0);
	}

	(void)dwc_usb3_dep_cfg(pcd_t, ep_reg, doepcfg0, doepcfg1, doepcfg2);
	(void)dwc_usb3_dep_xfercfg(pcd_t, ep_reg, 1);

	ep_reg = &pcd_t->in_ep_regs[0];
	(void)dwc_usb3_dep_cfg(pcd_t, ep_reg, diepcfg0, diepcfg1, diepcfg2);
	(void)dwc_usb3_dep_xfercfg(pcd_t, ep_reg, 1);

	dwc_usb3_ena_usb2_suspend(pcd_t);
	pcd_t->ep0->dwc_ep.active = 1;
}

void
dwc_usb3_ep_activate(usb_dwc_pcd_t *pcd, usb_dwc_pcd_ep_t *ep_t,
    int restore)
{
	usb_dwc_dev_ep_regs_t *ep_reg = NULL;
	usb_dwc_dev_ep_regs_t *ep0_reg_t = NULL;
	uint32_t depcfg0, depcfg1;
	uint32_t depcfg2 = 0;

	dwc3_pcd_debug("%s() EP%u-%s\n", __FUNCTION__, ep_t->dwc_ep.num,
	    (ep_t->dwc_ep.is_in ? "IN" : "OUT"));

	ep_t->dwc_ep.hiber_desc_idx = 0;

	if (ep_t->dwc_ep.is_in)
		ep_reg = ep_t->dwc_ep.in_ep_reg;
	else
		ep_reg = ep_t->dwc_ep.out_ep_reg;

	dwc_usb3_dis_usb2_suspend(pcd);

	if (!pcd->eps_enabled) {
		pcd->eps_enabled = 1;

		depcfg0 = USB_DWC_USB3_EP_TYPE_CONTROL << USB_DWC_EPCFG0_EPTYPE_SHIFT;
		depcfg0 |= USB_DWC_CFG_ACTION_MODIFY << USB_DWC_EPCFG0_CFG_ACTION_SHIFT;
		depcfg1 = USB_DWC_EPCFG1_XFER_CMPL_BIT | USB_DWC_EPCFG1_XFER_IN_PROG_BIT |
		    USB_DWC_EPCFG1_XFER_NRDY_BIT | USB_DWC_EPCFG1_EP_DIR_BIT;

		switch (pcd->speed) {
		case USB_SPEED_SUPER:
			depcfg0 |= 512 << USB_DWC_EPCFG0_MPS_SHIFT;
			break;

		case USB_SPEED_HIGH:
			/* fall-through */
		case USB_SPEED_FULL:
			depcfg0 |= 64 << USB_DWC_EPCFG0_MPS_SHIFT;
			break;

		case USB_SPEED_LOW:
			depcfg0 |= 8 << USB_DWC_EPCFG0_MPS_SHIFT;
			break;
		default:
			break;
		}

		ep0_reg_t = &pcd->in_ep_regs[0];
		(void)dwc_usb3_dep_cfg(pcd, ep0_reg_t, depcfg0, depcfg1, 0);

		if ((pcd->usb3_dev->snpsid & 0xffff) >= 0x109a) {
			ep0_reg_t = &pcd->out_ep_regs[0];
			(void)dwc_usb3_dep_startnewcfg(pcd, ep0_reg_t, 2);
		}
	}

	depcfg0 = ep_t->dwc_ep.type << USB_DWC_EPCFG0_EPTYPE_SHIFT;
	depcfg0 |= ep_t->dwc_ep.maxpacket << USB_DWC_EPCFG0_MPS_SHIFT;

	if (ep_t->dwc_ep.is_in) {
		depcfg0 |= ep_t->dwc_ep.tx_fifo_num << USB_DWC_EPCFG0_TXFNUM_SHIFT;
	}

	if (pcd->usb3_dev->core_params->burst) {
		dwc3_pcd_debug("Setting maxburst to %u\n", ep_t->dwc_ep.maxburst);
		depcfg0 |= ep_t->dwc_ep.maxburst << USB_DWC_EPCFG0_BRSTSIZ_SHIFT;
	}

	if (restore) {
		depcfg0 |= USB_DWC_CFG_ACTION_RESTORE <<
		    USB_DWC_EPCFG0_CFG_ACTION_SHIFT;
		depcfg2 = ep_t->dwc_ep.save_state;
	}

	depcfg1 = ep_t->dwc_ep.num << USB_DWC_EPCFG1_EP_NUM_SHIFT;
	if (ep_t->dwc_ep.is_in)
		depcfg1 |= USB_DWC_EPCFG1_EP_DIR_BIT;

	depcfg1 |= USB_DWC_EPCFG1_XFER_CMPL_BIT;
	depcfg1 |= USB_DWC_EPCFG1_XFER_IN_PROG_BIT;
	depcfg1 |= USB_DWC_EPCFG1_XFER_NRDY_BIT;
	dwc3_pcd_debug("Setting bInterval-1 to %u\n", ep_t->dwc_ep.intvl);
	depcfg1 |= ep_t->dwc_ep.intvl << USB_DWC_EPCFG1_BINTERVAL_SHIFT;

	if (ep_t->dwc_ep.num_streams) {
		dwc3_pcd_debug("Setting stream-capable bit\n");
		depcfg1 |= USB_DWC_EPCFG1_STRM_CAP_BIT;
	}

	if (pcd->usb3_dev->core_params->ebc) {
		if (pcd->speed == USB_SPEED_SUPER ||
		    pcd->speed == USB_SPEED_HIGH) {
			dwc3_pcd_debug("Setting EBC enable bit\n");
			depcfg1 |= USB_DWC_EPCFG1_EBC_MODE_BIT;
		}
	}

	if (ep_t->dwc_ep.is_in) {
		ep_t->dwc_ep.param0in = depcfg0 & ~USB_DWC_EPCFG0_CFG_ACTION_BITS;
		ep_t->dwc_ep.param1in = depcfg1;
	} else {
		ep_t->dwc_ep.param0out = depcfg0 & ~USB_DWC_EPCFG0_CFG_ACTION_BITS;
		ep_t->dwc_ep.param1out = depcfg1;
	}

	(void)dwc_usb3_dep_cfg(pcd, ep_reg, depcfg0, depcfg1, depcfg2);

	if (!ep_t->dwc_ep.ena_once) {
		ep_t->dwc_ep.ena_once = 1;

		dwc3_pcd_debug("Setting 1 stream resource\n");
		(void)dwc_usb3_dep_xfercfg(pcd, ep_reg, 1);
	}

	dwc_usb3_ena_usb2_suspend(pcd);

	(void)dwc_usb3_enable_ep(pcd, ep_t);

	ep_t->dwc_ep.active = 1;
	ep_t->dwc_ep.stall_clear_flag = 0;
}

void
ep_deactivate(usb_dwc_pcd_t *pcd, usb_dwc_pcd_ep_t *ep_t)
{
	usb_dwc_dev_ep_regs_t *ep_reg = NULL;
	uint8_t tri;

	dwc3_pcd_debug("%s() EP%u-%s\n", __FUNCTION__, ep_t->dwc_ep.num,
	    (ep_t->dwc_ep.is_in ? "IN" : "OUT"));

	if (ep_t->dwc_ep.is_in) {
		ep_reg = ep_t->dwc_ep.in_ep_reg;
		tri = ep_t->dwc_ep.tri_in;
		ep_t->dwc_ep.tri_in = 0;
	} else {
		ep_reg = ep_t->dwc_ep.out_ep_reg;
		tri = ep_t->dwc_ep.tri_out;
		ep_t->dwc_ep.tri_out = 0;
	}

	dwc_usb3_dis_usb2_suspend(pcd);

	(void)dwc_usb3_dep_cstall(pcd, ep_reg, 0);

	if (tri) {
		ep_t->dwc_ep.condition = 0;
		(void)dwc_usb3_dep_endxfer(pcd, ep_reg, tri - 1, DWC_ENDXFER_FORCE,
		    &ep_t->dwc_ep.condition);
	}

	dwc_usb3_ena_usb2_suspend(pcd);
	ep_t->dwc_ep.xfer_started = 0;

	(void)dwc_usb3_disable_ep(pcd, ep_t);

	ep_t->dwc_ep.active = 0;
}

void
dwc_usb3_pcd_ep0_out_start(usb_dwc_pcd_t *pcd)
{
	usb_dwc_dev_ep_regs_t *ep_reg_t = NULL;
	usb_dwc_dma_desc_t *desc = NULL;
	usb_dwc_dma_t desc_dma;
	uint8_t tri;

	desc = dwc_usb3_ep0_setup_desc(pcd);
	desc_dma = dwc_usb3_ep0_setup_desc_dma(pcd);

	dwc_usb3_fill_desc(desc, dwc_usb3_ep0_setup_pkt_dma(pcd),
	    pcd->ep0->dwc_ep.maxpacket, 0,
	    USB_DWC_DSCCTL_TRBCTL_SETUP, USB_DWC_DSCCTL_LST_BIT, 1);
	dwc3_pcd_debug("%s() xfercnt=%u bptr=0x%08x:%08x\n",
	    __FUNCTION__, dwc_usb3_get_xfercnt(desc),
	    desc->bpth, desc->bptl);

	ep_reg_t = &pcd->out_ep_regs[0];
	dwc_usb3_dis_usb2_suspend(pcd);

	wmb();
	tri = dwc_usb3_dep_startxfer(pcd, ep_reg_t, desc_dma, 0);
	pcd->ep0->dwc_ep.tri_out = tri + 1;

	dwc_usb3_ena_usb2_suspend(pcd);
}

void
dwc_usb3_pcd_ep0_start_transfer(usb_dwc_pcd_t *pcd,
    usb_dwc_pcd_req_t *req)
{
	usb_dwc_pcd_ep_t *ep_t = pcd->ep0;
	usb_dwc_dev_ep_regs_t *ep_reg = NULL;
	usb_dwc_dma_desc_t *desc_t = NULL;
	usb_dwc_dma_t desc_dma_t;
	uint32_t desc_type, length;
	uint8_t tri;

	dwc3_pcd_debug("%s(): ep%u-%s xfer_len=%u xfer_cnt=%u\n",
	    __FUNCTION__, ep_t->dwc_ep.num, (ep_t->dwc_ep.is_in ? "IN" : "OUT"),
	    req->dwc_req.length, req->dwc_req.actual);

	if (ep_t->dwc_ep.is_in) {
		req->dwc_req.trb = dwc_usb3_ep0_in_desc(pcd);
		req->dwc_req.trbdma = dwc_usb3_ep0_in_desc_dma(pcd);
	} else {
		req->dwc_req.trb = dwc_usb3_ep0_out_desc(pcd);
		req->dwc_req.trbdma = dwc_usb3_ep0_out_desc_dma(pcd);
	}

	desc_t = req->dwc_req.trb;
	desc_dma_t = req->dwc_req.trbdma;
	dwc_usb3_dis_usb2_suspend(pcd);

	if (ep_t->dwc_ep.is_in) {
		ep_reg = ep_t->dwc_ep.in_ep_reg;

		length = req->dwc_req.length;

		dwc3_pcd_debug("IN EP0STATE=%d\n", pcd->ep0state);

		if (pcd->ep0state == USB_EP0_IN_STATUS_PHASE) {
			if (ep_t->dwc_ep.three_stage)
				desc_type = USB_DWC_DSCCTL_TRBCTL_STATUS_3;
			else
				desc_type = USB_DWC_DSCCTL_TRBCTL_STATUS_2;
		} else {
			desc_type = USB_DWC_DSCCTL_TRBCTL_CTLDATA_1ST;
		}

		dwc_usb3_fill_desc(desc_t, req->dwc_req.bufdma[0], length, 0, desc_type, USB_DWC_DSCCTL_LST_BIT, 1);
		dwc3_pcd_debug("IN xferlen=%u bptr=0x%08x:%08x\n",
		    dwc_usb3_get_xfercnt(desc_t), desc_t->bpth, desc_t->bptl);

		wmb();
		tri = dwc_usb3_dep_startxfer(pcd, ep_reg, desc_dma_t, 0);
		ep_t->dwc_ep.tri_in = tri + 1U;
	} else {
		ep_reg = ep_t->dwc_ep.out_ep_reg;

		length = (req->dwc_req.length + ep_t->dwc_ep.maxpacket - 1U) & ~(ep_t->dwc_ep.maxpacket - 1U);

		dwc3_pcd_debug("OUT EP0STATE=%d\n", pcd->ep0state);

		if (pcd->ep0state == USB_EP0_OUT_STATUS_PHASE) {
			if (ep_t->dwc_ep.three_stage)
				desc_type = USB_DWC_DSCCTL_TRBCTL_STATUS_3;
			else
				desc_type = USB_DWC_DSCCTL_TRBCTL_STATUS_2;
		} else {
			desc_type = USB_DWC_DSCCTL_TRBCTL_CTLDATA_1ST;
		}

		dwc_usb3_fill_desc(desc_t, req->dwc_req.bufdma[0], length, 0, desc_type, USB_DWC_DSCCTL_LST_BIT, 1);
		dwc3_pcd_debug("OUT xferlen=%u bptr=0x%08x:%08x\n",
		    dwc_usb3_get_xfercnt(desc_t), desc_t->bpth, desc_t->bptl);

		wmb();
		tri = dwc_usb3_dep_startxfer(pcd, ep_reg, desc_dma_t, 0);
		ep_t->dwc_ep.tri_out = tri + 1;
	}

	dwc_usb3_ena_usb2_suspend(pcd);
}

void
dwc_usb3_pcd_ep0_continue_transfer(usb_dwc_pcd_t *pcd,
    usb_dwc_pcd_req_t *req)
{
	usb_dwc_pcd_ep_t *ep_t = pcd->ep0;
	usb_dwc_dev_ep_regs_t *ep_reg = NULL;
	usb_dwc_dma_desc_t *desc = NULL;
	usb_dwc_dma_t desc_dma;
	uint8_t tri_value;

	dwc3_pcd_debug("%s()\n", __FUNCTION__);

	if (ep_t->dwc_ep.is_in) {
		desc = dwc_usb3_ep0_in_desc(pcd);
		desc_dma = dwc_usb3_ep0_in_desc_dma(pcd);
		ep_reg = ep_t->dwc_ep.in_ep_reg;

		dwc_usb3_fill_desc(desc, req->dwc_req.bufdma[0], 0, 0,
		    USB_DWC_DSCCTL_TRBCTL_NORMAL, USB_DWC_DSCCTL_LST_BIT,1);
		/* Make sure all writes to TRB have completed */
		wmb();

		dwc_usb3_dis_usb2_suspend(pcd);
		tri_value = dwc_usb3_dep_startxfer(pcd, ep_reg, desc_dma, 0);
		ep_t->dwc_ep.tri_in = tri_value + 1;
		dwc_usb3_ena_usb2_suspend(pcd);
	}
}

uint16_t
calc_isoc_frame_number(const usb_dwc_pcd_t *pcd, const usb_dwc_pcd_ep_t *ep_t, uint32_t event)
{
	uint32_t dsts, now;
	uint16_t uf_cur, intvl, maskcode;
	uint16_t uf_tar = 0;
	if (event) {
		uf_cur = event >> USB_DWC_DEPEVT_ISOC_UFRAME_NUM_SHIFT &
		    USB_DWC_DEPEVT_ISOC_UFRAME_NUM_BITS >> USB_DWC_DEPEVT_ISOC_UFRAME_NUM_SHIFT;

		intvl = 1 << ep_t->dwc_ep.intvl;

		maskcode = ~(intvl - 1U);

		dsts = dwc_rd32(pcd->usb3_dev, &pcd->dev_global_regs->dsts);
		now = dsts >> USB_DWC_DSTS_SOF_FN_SHIFT & USB_DWC_DSTS_SOF_FN_BITS >> USB_DWC_DSTS_SOF_FN_SHIFT;
		if (now < (uf_cur & 0x3fff))
			now += 0x4000;
		else if (now > (uf_cur & 0x3fff))
			uf_cur = (uf_cur & ~0x3fff) + now;

		now += uf_cur & 0xc000;

		uf_tar = uf_cur & maskcode;
again:
		if (intvl <= 8)
			uf_tar += intvl << 2;
		else
			uf_tar += intvl << 1;

		dwc3_pcd_debug("tgt:%1x now:%1x tgt-now:%1x\n", uf_tar, now, uf_tar - now);
		if (uf_tar >= (0x8000U + now))
			goto again;

		uf_tar++;
	}
	return (uf_tar);
}

void
dwc_usb3_pcd_epn_continue_transfer(usb_dwc_pcd_t *pcd, usb_dwc_pcd_ep_t *ep_t, usb_dwc_pcd_req_t *req)
{
	usb_dwc_dev_ep_regs_t *ep_reg = NULL;
	usb_dwc_dma_desc_t *desc = NULL;
	usb_dwc_dma_t desc_dma;
	uint8_t tri_value;

	dwc3_pcd_debug("%s()\n", __FUNCTION__);

	if (ep_t->dwc_ep.is_in) {
		desc = ep_t->dwc_ep.dma_desc;
		desc_dma = ep_t->dwc_ep.dma_desc_dma;
		ep_reg = ep_t->dwc_ep.in_ep_reg;

		dwc_usb3_fill_desc(desc, req->dwc_req.bufdma[0], 0, 0,
		    USB_DWC_DSCCTL_TRBCTL_NORMAL, USB_DWC_DSCCTL_LST_BIT, 1);
		/* Make sure all writes to TRB have completed */
		wmb();

		dwc_usb3_dis_usb2_suspend(pcd);
		tri_value = dwc_usb3_dep_startxfer(pcd, ep_reg, desc_dma, 0);
		ep_t->dwc_ep.tri_in = tri_value + 1;
		dwc_usb3_ena_usb2_suspend(pcd);
	}
}

void
realign_frame_numer(const usb_dwc_pcd_t *pcd, usb_dwc_pcd_ep_t *ep_t,
    usb_dwc_dma_t desc_dma, uint16_t uf_tar, uint16_t intvl)
{
	uint8_t i;
	usb_dwc_dev_ep_regs_t *ep_reg;
	uint8_t tri;
	uint32_t dsts;

	if (ep_t->dwc_ep.is_in) {
		ep_reg = ep_t->dwc_ep.in_ep_reg;
	} else {
		ep_reg = ep_t->dwc_ep.out_ep_reg;
	}

	/* If the XferNotReady event arrives early, the calculated frame number may be zero */
	if (!uf_tar) {
		uf_tar = DWC_USB3_ALIGN_FRAME(uf_tar, intvl, 1);
	}

	/*
	 * If future interval time has already passed when
	 * core receives the command, it will realign
	 * the frame number and align it up to 5 times.
	 */
	for (i = 0; i < 5; i++) {
		tri = dwc_usb3_dep_startxfer(pcd, ep_reg, desc_dma, uf_tar - 1);
		if (tri != EINVAL) {
			break;
		}
		dsts = dwc_usb3_get_frame(pcd);
		uf_tar += dsts - (uf_tar & 0x3fff);
		uf_tar = DWC_USB3_ALIGN_FRAME(uf_tar, intvl, i + 1);
		uf_tar++;
	}

	if (ep_t->dwc_ep.is_in) {
		ep_t->dwc_ep.tri_in = tri + 1;
	} else {
		ep_t->dwc_ep.tri_out = tri + 1;
	}
	ep_t->dwc_ep.xfer_started = 1;
}

void
dwc_usb3_pcd_ep_start_transfer(usb_dwc_pcd_t *pcd, usb_dwc_pcd_ep_t *ep_t,
    usb_dwc_pcd_req_t *req, uint32_t event)
{
	usb_dwc_dev_ep_regs_t *ep_reg = NULL;
	usb_dwc_dma_t desc_dma;
	uint16_t intvl;
	uint16_t uf_tar = 0;
	uint8_t tri;
	int i;

	ep_t->dwc_ep.hiber_desc_idx = 0;

	intvl = 1 << ep_t->dwc_ep.intvl;
	uf_tar = calc_isoc_frame_number(pcd, ep_t, event);

	req->dwc_req.flags |= USB_DWC_PCD_REQ_STARTED;
	desc_dma = req->dwc_req.trbdma;
	dwc_usb3_dis_usb2_suspend(pcd);
	wmb();

	if (ep_t->dwc_ep.is_in) {
		ep_reg = ep_t->dwc_ep.in_ep_reg;

		for (i = 0; i < req->dwc_req.numbuf; i++) {
			usb_dma_cache_flush((void *)USB_DMA_PA_TO_VA(req->dwc_req.bufdma[i]), req->dwc_req.buflen[i]);
		}

		if (ep_t->dwc_ep.type == USB_EP_ATTR_XFER_ISOC) {
			dwc3_pcd_debug("ISOC xfer_started=%u\n", ep_t->dwc_ep.xfer_started);
			if (ep_t->dwc_ep.xfer_started) {
				(void)dwc_usb3_dep_updatexfer(pcd, ep_reg, ep_t->dwc_ep.tri_in - 1);
			} else {
				realign_frame_numer(pcd, ep_t, desc_dma, uf_tar, intvl);
			}
		} else {
			dwc3_pcd_debug("Non-ISOC xfer_started=%u\n", ep_t->dwc_ep.xfer_started);
			if (ep_t->dwc_ep.xfer_started) {
				(void)dwc_usb3_dep_updatexfer(pcd, ep_reg, ep_t->dwc_ep.tri_in - 1);
			} else {
				tri = dwc_usb3_dep_startxfer(pcd, ep_reg, desc_dma, req->dwc_req.stream);
				ep_t->dwc_ep.tri_in = tri + 1;
				ep_t->dwc_ep.xfer_started = 1;
			}
		}
	} else {
		ep_reg = ep_t->dwc_ep.out_ep_reg;
		for (i = 0; i < req->dwc_req.numbuf; i++) {
			usb_dma_cache_flush((void *)USB_DMA_PA_TO_VA(req->dwc_req.bufdma[i]), req->dwc_req.buflen[i]);
		}

		if (ep_t->dwc_ep.type == USB_EP_ATTR_XFER_ISOC) {
			if (ep_t->dwc_ep.xfer_started) {
				(void)dwc_usb3_dep_updatexfer(pcd, ep_reg, ep_t->dwc_ep.tri_out - 1);
			} else {
				realign_frame_numer(pcd, ep_t, desc_dma, uf_tar, intvl);
			}
		} else {
			if (ep_t->dwc_ep.xfer_started) {
				(void)dwc_usb3_dep_updatexfer(pcd, ep_reg, ep_t->dwc_ep.tri_out - 1);
			} else {
				tri = dwc_usb3_dep_startxfer(pcd, ep_reg, desc_dma, req->dwc_req.stream);
				ep_t->dwc_ep.tri_out = tri + 1;
				ep_t->dwc_ep.xfer_started = 1;
			}
		}
	}

	dwc_usb3_ena_usb2_suspend(pcd);
}

int
dwc_usb3_pcd_isoc_ep_hiber_restart(usb_dwc_pcd_t *pcd,
    usb_dwc_pcd_ep_t *ep_t)
{
	usb_dwc_dev_ep_regs_t *ep_reg_t = NULL;
	usb_dwc_dma_t desc_dma;
	int ep_owned;
	uint8_t *tri = NULL;

	ep_owned = ep_t->dwc_ep.hiber_desc_idx - 1;
	if (ep_owned < 0)
		return (0);

	if (ep_t->dwc_ep.is_in) {
		ep_reg_t = ep_t->dwc_ep.in_ep_reg;
		tri = &ep_t->dwc_ep.tri_in;
	} else {
		ep_reg_t = ep_t->dwc_ep.out_ep_reg;
		tri = &ep_t->dwc_ep.tri_out;
	}

	dwc3_pcd_debug("Restarting Isoc xfer\n");
	desc_dma = (usb_dwc_dma_t)
	    ((unsigned long)ep_t->dwc_ep.dma_desc_dma + ep_owned * 16);

	dwc_usb3_dis_usb2_suspend(pcd);
	wmb();
	*tri = dwc_usb3_dep_startxfer(pcd, ep_reg_t, desc_dma, 0) + 1;
	dwc_usb3_ena_usb2_suspend(pcd);

	return (1);
}

static
void dwc_usb3_stop_xfer(const usb_dwc_pcd_t *pcd, usb_dwc_pcd_ep_t *ep)
{
	usb_dwc_dev_ep_regs_t *ep_reg = NULL;

	dwc3_pcd_debug("%s()\n", __FUNCTION__);

	if (ep->dwc_ep.is_in) {
		if (ep->dwc_ep.active && ep->dwc_ep.tri_in) {
			ep_reg = ep->dwc_ep.in_ep_reg;
			ep->dwc_ep.condition = 0;
			(void)dwc_usb3_dep_endxfer(pcd, ep_reg, ep->dwc_ep.tri_in - 1,
			    DWC_ENDXFER_FORCE,
			    &ep->dwc_ep.condition);
			ep->dwc_ep.tri_in = 0;
		}
	} else {
		if (ep->dwc_ep.active && ep->dwc_ep.tri_out) {
			ep_reg = ep->dwc_ep.out_ep_reg;
			ep->dwc_ep.condition = 0;
			(void)dwc_usb3_dep_endxfer(pcd, ep_reg, ep->dwc_ep.tri_out - 1,
			    DWC_ENDXFER_FORCE,
			    &ep->dwc_ep.condition);
			ep->dwc_ep.tri_out = 0;
		}
	}
}

void
dwc_usb3_stop_all_xfers(usb_dwc_pcd_t *pcd)
{
	uint8_t index;
	usb_dwc_pcd_ep_t *ep_t = NULL;

	dwc3_pcd_debug("%s()\n", __FUNCTION__);
	dwc_usb3_dis_usb2_suspend(pcd);

	dwc3_pcd_debug("\n####%s,%d,pcd->num_in_eps=0x%x,pcd->num_out_eps=0x%x\n",
	    __FUNCTION__, __LINE__, pcd->num_in_eps, pcd->num_out_eps);
	for (index = pcd->num_in_eps; index > 0; index--) {
		ep_t = pcd->in_ep[index - 1];
		dwc3_pcd_debug("DWC IN EP%u tri-in=%u\n", index, ep_t->dwc_ep.tri_in);

		dwc_usb3_stop_xfer(pcd, ep_t);
		dwc_usb3_gadget_request_nuke(pcd, ep_t);
		ep_t->dwc_ep.xfer_started = 0;
	}

	for (index = pcd->num_out_eps; index > 0; index--) {
		ep_t = pcd->out_ep[index - 1];
		dwc3_pcd_debug("DWC OUT EP%u tri-out=%u\n", index, ep_t->dwc_ep.tri_out);

		dwc_usb3_stop_xfer(pcd, ep_t);
		dwc_usb3_gadget_request_nuke(pcd, ep_t);
		ep_t->dwc_ep.xfer_started = 0;
	}

	dwc_usb3_ena_usb2_suspend(pcd);
}

void
dwc_usb3_endxfer(usb_dwc_pcd_t *pcd, usb_dwc_pcd_ep_t *ep)
{
	usb_dwc_dev_ep_regs_t *ep_reg = NULL;
	uint32_t tri;

	dwc3_pcd_debug("%s()\n", __FUNCTION__);

	ep_reg = ep->dwc_ep.in_ep_reg;
	tri = ep->dwc_ep.tri_in;
	ep->dwc_ep.tri_in = 0;
	dwc_usb3_dis_usb2_suspend(pcd);
	(void)dwc_usb3_dep_endxfer(pcd, ep_reg, tri - 1,
	    DWC_ENDXFER_FORCE |
	    DWC_ENDXFER_NODELAY,
	    NULL);
	dwc_usb3_ena_usb2_suspend(pcd);
}

static int
dwc_usb3_ep_complete_request(usb_dwc_pcd_t *pcd,
    usb_dwc_pcd_ep_t *ep_t, usb_dwc_pcd_req_t *req, uint32_t event)
{
	int blast = 0;
	int retval = 0;
	uint32_t uf_evt, status;
	int uf_cur, differ, intvl, tri, index;
	usb_dwc_dma_desc_t *desc_t = NULL;
	usb_dwc_dev_ep_regs_t *ep_reg = NULL;
	uint32_t byte_num;

	dwc3_pcd_debug("%s()\n", __FUNCTION__);

	desc_t = req->dwc_req.trb;

	if (desc_t == NULL) {
		dwc3_pcd_debug("### %s, EP%u-%s request TRB is NULL! ###\n",
		    __FUNCTION__, ep_t->dwc_ep.num, ep_t->dwc_ep.is_in ?
		    "IN" : "OUT");
		return (retval);
	}

	if (!(req->dwc_req.flags & USB_DWC_PCD_REQ_STARTED)) {
		dwc3_pcd_debug("### %s, EP%u-%s request not started! ###\n",
		    __FUNCTION__, ep_t->dwc_ep.num, ep_t->dwc_ep.is_in ?
		    "IN" : "OUT");
		if ((ep_t->dwc_ep.type == USB_EP_ATTR_XFER_ISOC) &&
		    ((event & USB_DWC_DEPEVT_INTTYPE_BITS) ==
		    (USB_DWC_DEPEVT_XFER_IN_PROG << USB_DWC_DEPEVT_INTTYPE_SHIFT)) &&
		    (ep_t->dwc_ep.xfer_started == 0)) {
			blast = 1;
			goto done;
		}
		return (retval);
	}

	if (dwc_usb3_is_hwo(desc_t)) {
		dwc3_pcd_debug("### %s, EP%u-%s HWO bit set! ###\n",
		    __FUNCTION__, ep_t->dwc_ep.num, ep_t->dwc_ep.is_in ? "IN" : "OUT");
		return (retval);
	}

	if (ep_t->dwc_ep.type == USB_EP_ATTR_XFER_ISOC) {
		uf_cur = dwc_usb3_get_frame(pcd);
		uf_evt = dwc_usb3_get_eventsofn(event);
		dwc3_pcd_debug("ep:%1x evt:%08x frm:%04x now:%04x trb:%1lx len:%04x\n",
		    ep_t->dwc_ep.num, event, uf_evt, uf_cur,
		    ((unsigned long)req->dwc_req.trbdma - (unsigned long)ep_t->dwc_ep.dma_desc_dma) /
		    (req->dwc_req.numbuf * 16), dwc_usb3_get_xfercnt(desc_t));

		status = dwc_usb3_get_xfersts(desc_t);
		if (status & USB_DWC_TRBRSP_MISSED_ISOC_IN) {
			desc_t->status &= ~USB_DWC_DSCSTS_TRBRSP_BITS;

			if (!dwc_usb3_is_ioc(desc_t)) {
				goto no_restart;
			}

			uf_evt &= 0x3fff;
			differ = uf_cur - uf_evt;
			if (differ < 0) {
				differ += 0x4000;
			}
			intvl = 1 << ep_t->dwc_ep.intvl;
			dwc3_pcd_debug("diff %1x intvl %1x\n", differ, intvl);

			if (differ < intvl * 2) {
				goto no_restart;
			}

			dwc3_pcd_debug("Restart!\n");
			if (ep_t->dwc_ep.is_in) {
				ep_reg = ep_t->dwc_ep.in_ep_reg;
				tri = ep_t->dwc_ep.tri_in;
				ep_t->dwc_ep.tri_in = 0;
				dwc3_pcd_debug("IN tri=%1x\n", tri - 1);
			} else {
				ep_reg = ep_t->dwc_ep.out_ep_reg;
				tri = ep_t->dwc_ep.tri_out;
				ep_t->dwc_ep.tri_out = 0;
				dwc3_pcd_debug("OUT tri=%1x\n", tri - 1);
			}

			if (ep_t->dwc_ep.active && tri) {
				ep_t->dwc_ep.condition = 0;
				dwc_usb3_dis_usb2_suspend(pcd);
				(void)dwc_usb3_dep_endxfer(pcd, ep_reg, tri - 1,
				    DWC_ENDXFER_FORCE |
				    DWC_ENDXFER_NODELAY,
				    &ep_t->dwc_ep.condition);
				dwc_usb3_ena_usb2_suspend(pcd);
			}

			ep_t->dwc_ep.xfer_started = 0;

			retval = -1;
no_restart:
			req->dwc_req.actual = 0;
			blast = 1;
			goto done;
		}
	}

	if (ep_t->dwc_ep.is_in) {	/* IN endpoint */
		for (index = 0; index < req->dwc_req.numbuf; index++, desc_t++) {
			if (dwc_usb3_get_xfercnt(desc_t) == 0) {
				req->dwc_req.actual += req->dwc_req.buflen[index];
				if (index == req->dwc_req.numbuf - 1)
					blast = 1;
			} else {
				dwc3_pcd_debug("Incomplete transfer!\n");
			}
		}

		if (index)
			desc_t--;
		dwc3_pcd_debug("IN len=%u cnt=%u rem=%u\n",
		    req->dwc_req.length, req->dwc_req.actual, dwc_usb3_get_xfercnt(desc_t));

	} else {
		/* OUT endpoint */
		for (index = 0; index < req->dwc_req.numbuf; index++, desc_t++) {
			byte_num = req->dwc_req.buflen[index] - dwc_usb3_get_xfercnt(desc_t);
			req->dwc_req.actual += byte_num;
			usb_dma_cache_invalid((void *)USB_DMA_PA_TO_VA(req->dwc_req.bufdma[index]), byte_num);
		}
		if (index)
			desc_t--;
		dwc3_pcd_debug("OUT len=%u cnt=%u rem=%u\n",
		    req->dwc_req.length, req->dwc_req.actual, dwc_usb3_get_xfercnt(desc_t));
		blast = 1;
	}
done:
	if ((event & USB_DWC_DEPEVT_INTTYPE_BITS) == (USB_DWC_DEPEVT_XFER_CMPL << USB_DWC_DEPEVT_INTTYPE_SHIFT)) {
		if (ep_t->dwc_ep.is_in)
			ep_t->dwc_ep.tri_in = 0;
		else
			ep_t->dwc_ep.tri_out = 0;
	}

	if (ep_t->dwc_ep.send_zlp) {
		dwc_usb3_pcd_epn_continue_transfer(pcd, ep_t, req);
		ep_t->dwc_ep.send_zlp = 0;
		return (retval);
	}

	if (blast) {
		dwc_usb3_pcd_request_done(pcd, ep_t, req, 0);
		if ((ep_t->dwc_ep.type != USB_EP_ATTR_XFER_ISOC) || ep_t->dwc_ep.xfer_started)
			dwc_usb3_gadget_start_next_request(pcd, ep_t);
	} else {
		dwc3_pcd_debug("### EP%u-%s is_last not set! ###\n",
		    ep_t->dwc_ep.num, ep_t->dwc_ep.is_in ? "IN" : "OUT");
	}

	return (retval);
}

void
dwc_usb3_complete_request(usb_dwc_pcd_t *pcd, usb_dwc_pcd_ep_t *ep_t,
    uint32_t event)
{
	usb_dwc_pcd_req_t *req = NULL;
	usb_dwc_dma_desc_t *desc = NULL;
	int ret;

	dwc3_pcd_debug("%s()\n", __FUNCTION__);
	dwc3_pcd_debug("Requests %u\n", pcd->request_pending);

	req = dwc_usb3_gadget_get_request(pcd, ep_t);
	if (req == NULL) {
		dwc3_pcd_show("### %s, ep->dwc_ep.queue empty!\n", __FUNCTION__);
		return;
	}
next:
	ret = dwc_usb3_ep_complete_request(pcd, ep_t, req, event);
	dwc3_pcd_debug("dwc_usb3_ep_complete_request() returned %d\n", ret);
	if (!ret)
		return;

	req = dwc_usb3_gadget_get_request(pcd, ep_t);
	if (req == NULL)
		return;

	if (ret < 0) {
		dwc_usb3_gadget_set_ep_not_started(pcd, ep_t);
	} else {
		dwc3_pcd_debug("Requests2 %u\n", pcd->request_pending);
		desc = req->dwc_req.trb;
		if (desc && (req->dwc_req.flags & USB_DWC_PCD_REQ_STARTED) &&
		    !dwc_usb3_is_hwo(desc)) {
			dwc3_pcd_debug("Processing next TRB\n");
			goto next;
		}
	}
}

void
dwc_usb3_pcd_ep_set_stall(usb_dwc_pcd_t *pcd, usb_dwc_pcd_ep_t *ep_t)
{
	usb_dwc_dev_ep_regs_t *ep_reg = NULL;

	dwc3_pcd_debug("%s\n", __FUNCTION__);
	dwc3_pcd_debug("ep_num=%u is_in=%u\n", ep_t->dwc_ep.num, ep_t->dwc_ep.is_in);

	if (ep_t->dwc_ep.is_in)
		ep_reg = ep_t->dwc_ep.in_ep_reg;
	else
		ep_reg = ep_t->dwc_ep.out_ep_reg;

	dwc_usb3_dis_usb2_suspend(pcd);
	(void)dwc_usb3_dep_sstall(pcd, ep_reg);
	dwc_usb3_ena_usb2_suspend(pcd);
}

void
dwc_usb3_pcd_ep_clear_stall(usb_dwc_pcd_t *pcd, usb_dwc_pcd_ep_t *ep_t)
{
	usb_dwc_dev_ep_regs_t *ep_reg = NULL;

	dwc3_pcd_debug("%s()\n", __FUNCTION__);
	dwc3_pcd_debug("ep_num=%u is_in=%u\n", ep_t->dwc_ep.num, ep_t->dwc_ep.is_in);

	if (ep_t->dwc_ep.is_in)
		ep_reg = ep_t->dwc_ep.in_ep_reg;
	else
		ep_reg = ep_t->dwc_ep.out_ep_reg;

	dwc_usb3_dis_usb2_suspend(pcd);
	(void)dwc_usb3_dep_cstall(pcd, ep_reg, 0);
	dwc_usb3_ena_usb2_suspend(pcd);
}

usb_dwc_pcd_ep_t *
dwc_usb3_get_out_ep(const usb_dwc_pcd_t *pcd, uint32_t num)
{
	usb_dwc_pcd_ep_t *ep_t = NULL;

	if (num == 0) {
		ep_t = pcd->ep0;
	} else {
		ep_t = pcd->out_ep[num - 1];
	}

	return (ep_t);
}

usb_dwc_pcd_ep_t *
dwc_usb3_get_in_ep(const usb_dwc_pcd_t *pcd, uint32_t num)
{
	usb_dwc_pcd_ep_t *ep_t = NULL;

	if (num == 0) {
		ep_t = pcd->ep0;
	} else {
		ep_t = pcd->in_ep[num - 1];
	}

	return (ep_t);
}

usb_dwc_pcd_ep_t *
dwc_usb3_pcd_get_ep_by_addr(const usb_dwc_pcd_t *pcd, uint16_t index)
{
	uint32_t num = index & USB_EP_ADDR_NUMBER_MASK;

	if ((index & USB_DIR_MASK) == USB_DIR_IN)
		return (dwc_usb3_get_in_ep(pcd, num));
	else
		return (dwc_usb3_get_out_ep(pcd, num));
}

int
dwc_usb3_pcd_ep_enable(usb_dwc_pcd_t *pcd, usb_dwc_pcd_ep_t *ep_t,
    const struct usb_epdesc_s *ep_desc, const struct usb_ss_comp_epdesc_s *ep_comp)
{
	int ep_num, ep_dir;
	uint16_t maxpacket;
	uint8_t ep_type;

	dwc3_pcd_debug("ep is_in=%u\n", ep_t->dwc_ep.is_in);

	if (ep_t->dwc_ep.usb_ep_desc) {
		dwc3_pcd_show("%s, bad ep or descriptor!\n", __FUNCTION__);
		return (-DWC3_USB_E_INVALID);
	}

	ep_t->dwc_ep.usb_ep_desc = ep_desc;

	ep_t->dwc_ep.stopped = 0;

	ep_num = ep_desc->addr & USB_EP_ADDR_NUMBER_MASK;
	if (ep_t->dwc_ep.num != ep_num) {
		dwc3_pcd_show("%s, EP num mismatch, is %u asked %d!\n",
		    __FUNCTION__, ep_t->dwc_ep.num, ep_num);
	}

	ep_dir = ep_desc->addr & USB_EP_ADDR_DIR_MASK;
	if (ep_t->dwc_ep.is_in != (ep_dir == USB_DIR_IN)) {
		dwc3_pcd_show("%s, EP dir mismatch, is %u asked %u!\n",
		    __FUNCTION__, ep_t->dwc_ep.is_in, ep_dir == USB_DIR_IN);
	}

	ep_type = ep_desc->attr & USB_EP_ATTR_XFERTYPE_MASK;
	maxpacket = USB_GETW(ep_desc->mxpacketsize);

	ep_t->dwc_ep.type = ep_type;
	ep_t->dwc_ep.maxpacket = maxpacket & 0x7ff;
	ep_t->dwc_ep.intvl = 0;
	ep_t->dwc_ep.mult = 0;
	ep_t->dwc_ep.maxburst = 0;
	ep_t->dwc_ep.num_streams = 0;
	ep_t->dwc_ep.xfer_started = 0;

	if ((pcd->speed == USB_SPEED_SUPER) && ep_comp)
		ep_t->dwc_ep.maxburst = ep_comp->maxburst;

	switch (ep_type) {
	case USB_EP_ATTR_XFER_ISOC:
		if (pcd->speed == USB_SPEED_SUPER && ep_comp) {
			ep_t->dwc_ep.mult = USB_SSE_GET_MAX_PACKET_NUM(
			    ep_comp->attr) + 1;
			/* 3 packets at most */
			if (ep_t->dwc_ep.mult > 3)
				return (-DWC3_USB_E_INVALID);
		}
		/* fall-through */
	case USB_EP_ATTR_XFER_INT:	/* no need break */
		if (pcd->speed == USB_SPEED_SUPER) {
			ep_t->dwc_ep.intvl = ep_desc->interval - 1;
			break;
		}

		ep_t->dwc_ep.intvl = ep_desc->interval - 1;
		ep_t->dwc_ep.mult = (maxpacket >> 11 & 3) + 1;

		if (ep_t->dwc_ep.mult > 3)	/* 3 packets at most */
			return (-DWC3_USB_E_INVALID);

		break;

	case USB_EP_ATTR_XFER_BULK:
		if ((pcd->speed == USB_SPEED_SUPER) && ep_comp)
			ep_t->dwc_ep.num_streams =
			    USB_SSE_GET_MAX_STREAMS(ep_comp->attr);

		/* Set initial data PID */
		ep_t->dwc_ep.data_pid_start = 0;
		break;
	default:
		break;
	}

	dwc3_pcd_debug("type=%u maxpkt=%u mult=%u maxbst=%u numstrm=%u\n",
	    ep_type, maxpacket, ep_t->dwc_ep.mult, ep_t->dwc_ep.maxburst,
	    ep_t->dwc_ep.num_streams);

	dwc_usb3_ep_activate(pcd, ep_t, 0);
	return (0);
}

int
dwc_usb3_pcd_ep_disable(usb_dwc_pcd_t *pcd, usb_dwc_pcd_ep_t *ep_t)
{
	dwc3_pcd_debug("ep is_in=%u\n", ep_t->dwc_ep.is_in);

	if (ep_t->dwc_ep.usb_ep_desc == NULL) {
		return (-DWC3_USB_E_INVALID);
	}

	ep_deactivate(ep_t->dwc_ep.pcd, ep_t);
	dwc_usb3_gadget_request_nuke(pcd, ep_t);
	ep_t->dwc_ep.usb_ep_desc = NULL;

	return (0);
}

int
dwc_usb3_pcd_ep_submit_req(usb_dwc_pcd_t *pcd, usb_dwc_pcd_ep_t *ep_t,
    usb_dwc_pcd_req_t *req, uint32_t req_flags)
{
	dwc3_pcd_debug("%s()\n", __FUNCTION__);
	struct usb_ctrlreq_s ctrl_req;
	uint16_t len;

	if (ep_t == pcd->ep0) {
		switch (pcd->ep0state) {
		case USB_EP0_IN_DATA_PHASE:
			dwc3_pcd_debug("%s ep0: USB_EP0_IN_DATA_PHASE\n", __FUNCTION__);
			break;

		case USB_EP0_OUT_DATA_PHASE:
			dwc3_pcd_debug("%s ep0: USB_EP0_OUT_DATA_PHASE\n", __FUNCTION__);
			if (pcd->request_config) {
				ep_t->dwc_ep.is_in = 1;
				pcd->ep0state = USB_EP0_IN_WAIT_NRDY;
				return (1);
			}
			break;

		case USB_EP0_IN_WAIT_GADGET:
			dwc3_pcd_debug("%s ep0: USB_EP0_IN_WAIT_GADGET\n", __FUNCTION__);
			pcd->ep0state = USB_EP0_IN_WAIT_NRDY;
			return (2);

		case USB_EP0_OUT_WAIT_GADGET:
			dwc3_pcd_debug("%s ep0: USB_EP0_OUT_WAIT_GADGET\n", __FUNCTION__);
			pcd->ep0state = USB_EP0_OUT_WAIT_NRDY;
			return (3);

		case USB_EP0_IN_WAIT_NRDY:
			dwc3_pcd_debug("%s ep0: USB_EP0_IN_WAIT_NRDY\n", __FUNCTION__);
			pcd->ep0state = USB_EP0_IN_STATUS_PHASE;
			break;

		case USB_EP0_OUT_WAIT_NRDY:
			dwc3_pcd_debug("%s ep0: USB_EP0_OUT_WAIT_NRDY\n", __FUNCTION__);
			pcd->ep0state = USB_EP0_OUT_STATUS_PHASE;
			break;

		default:
			dwc3_pcd_debug("%s ep0: odd state %d!\n", __FUNCTION__, pcd->ep0state);
			return (-DWC3_USB_E_SHUTDOWN);
		}

		ep_t->dwc_ep.send_zlp = 0;
		ctrl_req = pcd->usb_ep0_setup_pkt->req;
		len = USB_GETW(ctrl_req.len);
		/* Have more package. */
		if ((req_flags & USB_DWC_PCD_REQ_ZERO) && (req->dwc_req.length != 0) &&
		    ((req->dwc_req.length & (ep_t->dwc_ep.maxpacket - 1U)) == 0) &&
		    (req->dwc_req.length != len)) {
			ep_t->dwc_ep.send_zlp = 1;
		}

		dwc_usb3_pcd_ep0_start_transfer(pcd, req);
	} else {
		ep_t->dwc_ep.send_zlp = 0;
		if (req->usb_req->zero) {
			ep_t->dwc_ep.send_zlp = 1;
		}
		dwc_usb3_pcd_ep_start_transfer(pcd, ep_t, req, 0);
	}

	return (0);
}

void
dwc_usb3_pcd_ep_cancel_req(usb_dwc_pcd_t *pcd, usb_dwc_pcd_ep_t *ep,
    usb_dwc_pcd_req_t *req, uint32_t stream)
{
	dwc3_pcd_debug("%s stream %u\n", __FUNCTION__, stream);

	dwc_usb3_dis_usb2_suspend(pcd);
	dwc_usb3_stop_xfer(pcd, ep);
	dwc_usb3_ena_usb2_suspend(pcd);

	ep->dwc_ep.xfer_started = 0;
	dwc_usb3_pcd_request_done(pcd, ep, req, -DWC3_USB_E_DISCONNECT);
}

void
dwc_usb3_pcd_ep_set_halt(usb_dwc_pcd_t *pcd, usb_dwc_pcd_ep_t *ep_t, int value)
{
	dwc3_pcd_debug("%s(%d)\n", __FUNCTION__, value);

	if (value == 0) {
		ep_t->dwc_ep.stall_clear_flag = 0;
		if (ep_t != pcd->ep0)
			dwc_usb3_pcd_ep_clear_stall(pcd, ep_t);

		if (ep_t->dwc_ep.stopped) {
			ep_t->dwc_ep.stopped = 0;

			if ((ep_t != pcd->ep0) && ep_t->dwc_ep.is_in)
				dwc_usb3_gadget_start_next_request(pcd, ep_t);
		}
	} else if (value == 1) {
stall:
		if (ep_t == pcd->ep0) {
			ep_t->dwc_ep.is_in = 0;
			dwc_usb3_pcd_ep_set_stall(pcd, ep_t);
			pcd->ep0state = USB_EP0_STALL;
		} else {
			dwc_usb3_pcd_ep_set_stall(pcd, ep_t);
		}

		ep_t->dwc_ep.stopped = 1;

	} else if (value == 2) {
		ep_t->dwc_ep.stall_clear_flag = 0;
	} else if (value == 3) {
		ep_t->dwc_ep.stall_clear_flag = 1;
		goto stall;
	}
}

void
dwc_usb3_pcd_request_done(usb_dwc_pcd_t *pcd, usb_dwc_pcd_ep_t *ep_t,
    usb_dwc_pcd_req_t *request, int status)
{
	unsigned ep_stopped = ep_t->dwc_ep.stopped;
	int index;
	usb_dwc_dma_desc_t *desc = NULL;

	dwc3_pcd_debug("%s(%d)\n", __FUNCTION__, status);

	if (ep_t != pcd->ep0) {
		request->dwc_req.flags &= ~USB_DWC_PCD_REQ_STARTED;
		desc = request->dwc_req.trb;
		for (index = 0; index < request->dwc_req.numbuf; index++, desc++) {
			if (desc == NULL) {
				break;
			}
			if (ep_t->dwc_ep.type == USB_EP_ATTR_XFER_ISOC) {
				(void)memset_s(desc, sizeof(usb_dwc_dma_desc_t), 0, sizeof(usb_dwc_dma_desc_t));
				dwc_usb3_fill_desc(desc, 0, 0, 0,
					USB_DWC_DSCCTL_TRBCTL_ISOC_1ST,
					USB_DWC_DSCCTL_IOC_BIT |
					USB_DWC_DSCCTL_IMI_BIT |
					USB_DWC_DSCCTL_CSP_BIT, 0);
			} else {
				dwc_usb3_disable_desc(desc);
			}
			ep_t->dwc_ep.desc_avail++;
			request->dwc_req.trb = NULL;
		}
	}

	ep_t->dwc_ep.stopped = 1;
	(void)dwc_usb3_gadget_complete(pcd, ep_t, request, status);

	ep_t->dwc_ep.stopped = ep_stopped;
}

void
dwc_usb3_pcd_stop(usb_dwc_pcd_t *pcd)
{
	usb_dwc_pcd_ep_t *ep_t = NULL;
	uint8_t index;

	dwc3_pcd_debug("%s()\n", __FUNCTION__);

	if (pcd->state == USB_DWC_STATE_UNCONNECTED) {
		dwc3_pcd_debug("%s() Already Disconnected\n", __FUNCTION__);
	} else {
		pcd->state = USB_DWC_STATE_UNCONNECTED;

		(void)dwc_usb3_gadget_disconnect(pcd);
	}

	dwc_usb3_dis_usb2_suspend(pcd);

	for (index = 0; index < pcd->num_in_eps; index++) {
		ep_t = pcd->in_ep[index];
		dwc_usb3_stop_xfer(pcd, ep_t);
		dwc_usb3_gadget_request_nuke(pcd, ep_t);
		ep_t->dwc_ep.xfer_started = 0;
	}

	for (index = 0; index < pcd->num_out_eps; index++) {
		ep_t = pcd->out_ep[index];
		dwc_usb3_stop_xfer(pcd, ep_t);
		dwc_usb3_gadget_request_nuke(pcd, ep_t);
		ep_t->dwc_ep.xfer_started = 0;
	}

	dwc_usb3_ena_usb2_suspend(pcd);
}

int
dwc_usb3_pcd_get_frame_number(const usb_dwc_pcd_t *pcd)
{
	return (dwc_usb3_get_frame(pcd));
}

static void
pcd_epinit(usb_dwc_pcd_t *pcd)
{
	uint32_t num_eps_out = pcd->num_out_eps;
	uint32_t num_eps_in = pcd->num_in_eps;
	uint32_t ep_control;
	usb_dwc_pcd_ep_t *ep_t = NULL;
	usb_dwc_pcd_req_t *ep0_req = NULL;

	dwc3_pcd_debug("num_out_eps=%u\n", num_eps_out);
	dwc3_pcd_debug("num_in_eps=%u\n", num_eps_in);

	ep_t = pcd->ep0;

	ep_t->dwc_ep.dma_desc = NULL;
	ep_t->dwc_ep.dma_desc_dma = 0;
	ep_t->dwc_ep.usb_ep_desc = NULL;
	ep_t->dwc_ep.pcd = pcd;
	ep_t->dwc_ep.stopped = 1;
	ep_t->dwc_ep.is_in = 0;
	ep_t->dwc_ep.active = 0;
	ep_t->dwc_ep.phys = 0;
	ep_t->dwc_ep.num = 0;
	ep_t->dwc_ep.tx_fifo_num = 0;
	ep_t->dwc_ep.out_ep_reg = &pcd->out_ep_regs[0];
	ep_t->dwc_ep.in_ep_reg = &pcd->in_ep_regs[0];

	ep_t->dwc_ep.type = USB_DWC_USB3_EP_TYPE_CONTROL;
	ep_t->dwc_ep.maxpacket = USB_DWC_MAX_EP0_SIZE;
	ep_t->dwc_ep.send_zlp = 0;
	ep_t->dwc_ep.queue_sof = 0;

	ep0_req = pcd->ep0_req;
	ep0_req->dwc_req.buf[0] = NULL;
	ep0_req->dwc_req.bufdma[0] = 0;
	ep0_req->dwc_req.buflen[0] = 0;
	ep0_req->dwc_req.length = 0;
	ep0_req->dwc_req.actual = 0;

	ep_control = 0;

	for (; ep_control < num_eps_out;) {
		ep_t = pcd->out_ep[ep_control];
		ep_control++;

		ep_t->dwc_ep.dma_desc = NULL;
		ep_t->dwc_ep.dma_desc_dma = 0;
		ep_t->dwc_ep.usb_ep_desc = NULL;
		ep_t->dwc_ep.pcd = pcd;
		ep_t->dwc_ep.stopped = 1;
		ep_t->dwc_ep.is_in = 0;
		ep_t->dwc_ep.active = 0;
		ep_t->dwc_ep.phys = ep_control << 1;
		ep_t->dwc_ep.tx_fifo_num = 0;
		ep_t->dwc_ep.out_ep_reg = &pcd->out_ep_regs[ep_control];

		ep_t->dwc_ep.type = USB_DWC_USB3_EP_TYPE_CONTROL;
		ep_t->dwc_ep.maxpacket = USB_DWC_MAX_EP0_SIZE;
		ep_t->dwc_ep.send_zlp = 0;
		ep_t->dwc_ep.queue_sof = 0;
	}

	ep_control = 0;

	for (; ep_control < num_eps_in;) {
		ep_t = pcd->in_ep[ep_control];
		ep_control++;

		ep_t->dwc_ep.dma_desc = NULL;
		ep_t->dwc_ep.dma_desc_dma = 0;
		ep_t->dwc_ep.usb_ep_desc = NULL;
		ep_t->dwc_ep.pcd = pcd;
		ep_t->dwc_ep.stopped = 1;
		ep_t->dwc_ep.is_in = 1;
		ep_t->dwc_ep.active = 0;
		ep_t->dwc_ep.phys = ep_control << 1 | 1;
		ep_t->dwc_ep.tx_fifo_num = ep_control;
		ep_t->dwc_ep.in_ep_reg = &pcd->in_ep_regs[ep_control];

		ep_t->dwc_ep.type = USB_DWC_USB3_EP_TYPE_CONTROL;
		ep_t->dwc_ep.maxpacket = USB_DWC_MAX_EP0_SIZE;
		ep_t->dwc_ep.send_zlp = 0;
		ep_t->dwc_ep.queue_sof = 0;
	}

	pcd->ep0state = USB_EP0_IDLE;
	pcd->ep0->dwc_ep.maxpacket = USB_DWC_MAX_EP0_SIZE;
	pcd->ep0->dwc_ep.type = USB_DWC_USB3_EP_TYPE_CONTROL;
}

int
dwc_usb3_pcd_init(dwc_usb3_device_t *device)
{
	dwc3_pcd_debug("%s()\n", __FUNCTION__);

	device->pcd.usb3_dev = device;
	device->pcd.speed = USB_SPEED_UNKNOWN;

	pcd_epinit(&device->pcd);

	dwc_usb3_pcd_device_init(device, 1, 0);

	return (0);
}

void
dwc_usb3_pcd_remove(dwc_usb3_device_t *device)
{
	dwc3_pcd_debug("%s()\n", __FUNCTION__);

	dwc_usb3_pcd_device_remove(device);
}
