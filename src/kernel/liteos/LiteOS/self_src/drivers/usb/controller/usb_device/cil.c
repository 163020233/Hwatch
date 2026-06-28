/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2014-2021. All rights reserved.
 * Description: LiteOS USB register operation implementation
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

#define dwc3_cil_show(x...) dprintf(x)

#ifdef DWC3_CIL_DEBUG
#define dwc3_cil_debug(x...) dwc3_cil_show(x)
#else
#define dwc3_cil_debug(x...) do {} while (0)
#endif

void
dwc_usb3_fill_desc(usb_dwc_dma_desc_t *pdesc, usb_dwc_dma_t addr,
    uint32_t dma_len, uint32_t stream, uint32_t type,
    uint32_t ctrlbits, int own)
{
	pdesc->bptl = (uint32_t)(addr & 0xffffffffU);
	pdesc->bptl += USB_DMA_OFFSET;
#ifdef DWC_USB3_64B_ARCH
	pdesc->bpth = (uint32_t)(addr >> 32U & 0xffffffffU);
#else
	pdesc->bpth = 0;
#endif
	pdesc->status = dma_len << USB_DWC_DSCSTS_XFRCNT_SHIFT;

	if (type) {
		pdesc->control = type << USB_DWC_DSCCTL_TRBCTL_SHIFT;
	}

	pdesc->control |= stream << USB_DWC_DSCCTL_STRMID_SOFN_SHIFT | ctrlbits;

	if (own) {
		wmb();
		pdesc->control |= USB_DWC_DSCCTL_HWO_BIT;
	}
}

void
dwc_usb3_start_desc_chain(usb_dwc_dma_desc_t *pdesc)
{
	pdesc->control |= USB_DWC_DSCCTL_CHN_BIT;
	pdesc->control &= ~USB_DWC_DSCCTL_IOC_BIT;
}

void
dwc_usb3_end_desc_chain(usb_dwc_dma_desc_t *pdesc)
{
	pdesc->control &= ~USB_DWC_DSCCTL_CHN_BIT;
	pdesc->control |= USB_DWC_DSCCTL_IOC_BIT;
}

/*
 * Enable a DMA descriptor by setting its HWO bit.
 */
void
dwc_usb3_enable_desc(usb_dwc_dma_desc_t *pdesc)
{
	wmb();
	pdesc->control |= USB_DWC_DSCCTL_HWO_BIT;
}

/*
 * Disable a DMA descriptor by clearing its HWO bit.
 * NOTE: This must only be called if it is known that the hardware has finished
 * with the DMA descriptor, but for some reason the hardware has not cleared
 * the HWO bit.
 */
void
dwc_usb3_disable_desc(usb_dwc_dma_desc_t *pdesc)
{
	pdesc->control &= ~USB_DWC_DSCCTL_HWO_BIT;
	wmb();
}

static int
handshake(const volatile dwc_usb3_device_t *pdev, const volatile uint32_t *ptr,
    uint32_t mask, uint32_t done)
{
	int sec = 10000;
	uint32_t ret;

	dwc3_cil_debug("%s()\n", __FUNCTION__);

	do {
		ret = dwc_rd32(pdev, ptr);
		if ((ret & mask) == done) {
			dwc3_cil_debug("after DEPCMD=%08x\n", ret);
			return (1);
		}
		dwc_udelay((void *)pdev, 1);
		sec -= 1;
	} while (sec > 0);

	return (0);
}

/* Wrapper function for dwc_handshake(), shows the source line for any failure */
#define dwc_handshake(_dev_, _ptr_, _mask_, _done_) ({			\
	int _ret_ = handshake(_dev_, _ptr_, _mask_, _done_);	\
	if (!_ret_)							\
		dprintf("dwc_handshake failed in %s():%d\n", __FUNCTION__, __LINE__); \
	_ret_;							\
})

int
dwc_usb3_xmit_fn_remote_wake(usb_dwc_pcd_t *pcd, uint32_t intf)
{
	dwc3_cil_debug("%s()\n", __FUNCTION__);

	/*
	 * Use the generic Transmit Device Notification command if the core
	 * supports it
	 */
	if (pcd->usb3_dev->snpsid >= 0x5533210a) {
		dwc_wr32(pcd->usb3_dev, &pcd->dev_global_regs->dgcmdpar,
		    intf << USB_DWC_DGCMDPAR_DEV_NOTIF_PARAM_SHIFT |
		    USB_DWC_DGCMD_FUNCTION_WAKE_DEV_NOTIF);
		dwc3_cil_debug("DGCMDPAR=%08x\n",
		    intf << USB_DWC_DGCMDPAR_DEV_NOTIF_PARAM_SHIFT |
		    USB_DWC_DGCMD_FUNCTION_WAKE_DEV_NOTIF);

		dwc_wr32(pcd->usb3_dev, &pcd->dev_global_regs->dgcmd,
		    USB_DWC_DGCMD_XMIT_DEV_NOTIF | USB_DWC_DGCMD_ACT_BIT);
		dwc3_cil_debug("DGCMD=%08x\n",
		    USB_DWC_DGCMD_XMIT_DEV_NOTIF | USB_DWC_DGCMD_ACT_BIT);
	} else {
		dwc_wr32(pcd->usb3_dev, &pcd->dev_global_regs->dgcmdpar, intf);
		dwc3_cil_debug("DGCMDPAR=%08x\n", intf);

		dwc_wr32(pcd->usb3_dev, &pcd->dev_global_regs->dgcmd,
		    USB_DWC_DGCMD_XMIT_FUNC_WAKE_DEV_NOTIF | USB_DWC_DGCMD_ACT_BIT);
		dwc3_cil_debug("DGCMD=%08x\n",
		    USB_DWC_DGCMD_XMIT_FUNC_WAKE_DEV_NOTIF | USB_DWC_DGCMD_ACT_BIT);
	}

	dwc_handshake(pcd->usb3_dev, &pcd->dev_global_regs->dgcmd,
	    USB_DWC_DGCMD_ACT_BIT, 0);

	return (0);
}

/*
 * Send LATENCY TOLERANCE MESSAGE DEVICE NOTIFICATION command to Device
 */
int
dwc_usb3_xmit_ltm(usb_dwc_pcd_t *pcd, uint32_t val)
{
	dwc3_cil_debug("%s()\n", __FUNCTION__);

	/*
	 * Use the generic Transmit Device Notification command if the core
	 * supports it. Otherwise this function is a no-op.
	 */
	if (pcd->usb3_dev->snpsid >= 0x5533210a) {
		dwc_wr32(pcd->usb3_dev, &pcd->dev_global_regs->dgcmdpar,
		    val << USB_DWC_DGCMDPAR_DEV_NOTIF_PARAM_SHIFT |
		    USB_DWC_DGCMD_LATENCY_TOL_DEV_NOTIF);
		dwc3_cil_debug("DGCMDPAR=%08x\n",
		    val << USB_DWC_DGCMDPAR_DEV_NOTIF_PARAM_SHIFT |
		    USB_DWC_DGCMD_LATENCY_TOL_DEV_NOTIF);

		dwc_wr32(pcd->usb3_dev, &pcd->dev_global_regs->dgcmd,
		    USB_DWC_DGCMD_XMIT_DEV_NOTIF | USB_DWC_DGCMD_ACT_BIT);
		dwc3_cil_debug("DGCMD=%08x\n",
		    USB_DWC_DGCMD_XMIT_DEV_NOTIF | USB_DWC_DGCMD_ACT_BIT);
	}

	return (0);
}

int
dwc_usb3_set_scratchpad_buf_array(usb_dwc_pcd_t *pcd, usb_dwc_dma_t addr)
{
	dwc3_cil_debug("%s(%lx)\n", __FUNCTION__, (unsigned long)addr);
	addr += USB_DMA_OFFSET;

	dwc_wr32(pcd->usb3_dev, &pcd->dev_global_regs->dgcmdpar, addr & 0xffffffffU);
	dwc3_cil_debug("DGCMDPAR=%08lx\n", (unsigned long)(addr & 0xffffffffU));

	dwc_wr32(pcd->usb3_dev, &pcd->dev_global_regs->dgcmd,
	    USB_DWC_DGCMD_SET_SCRATCHPAD_ARRAY_ADR_LO | USB_DWC_DGCMD_ACT_BIT);
	dwc3_cil_debug("DGCMD=%08x\n",
	    USB_DWC_DGCMD_SET_SCRATCHPAD_ARRAY_ADR_LO | USB_DWC_DGCMD_ACT_BIT);

	return (0);
}

/*
 * Send SELECTED FIFO FLUSH command to Device
 */
int
dwc_usb3_flush_fifo(usb_dwc_pcd_t *pcd, uint32_t fifo_sel)
{
	dwc3_cil_debug("%s()\n", __FUNCTION__);

	dwc_wr32(pcd->usb3_dev, &pcd->dev_global_regs->dgcmdpar, fifo_sel);
	dwc3_cil_debug("DGCMDPAR=%08x\n", fifo_sel);

	dwc_wr32(pcd->usb3_dev, &pcd->dev_global_regs->dgcmd,
	    USB_DWC_DGCMD_SELECTED_FIFO_FLUSH | USB_DWC_DGCMD_ACT_BIT);
	dwc3_cil_debug("DGCMD=%08x\n",
	    USB_DWC_DGCMD_SELECTED_FIFO_FLUSH | USB_DWC_DGCMD_ACT_BIT);

	dwc_handshake(pcd->usb3_dev, &pcd->dev_global_regs->dgcmd,
	    USB_DWC_DGCMD_ACT_BIT, 0);

	return (0);
}

int
dwc_usb3_dep_cfg(const usb_dwc_pcd_t *pcd, usb_dwc_dev_ep_regs_t *ep_reg,
    uint32_t dep_cfg0, uint32_t dep_cfg1, uint32_t dep_cfg2)
{
	dep_cfg1 |= USB_DEPCFG_INTR_NUM; /* intrNum */

	dwc_wr32(pcd->usb3_dev, &ep_reg->depcmdpar2, dep_cfg2);
	dwc3_cil_debug("DEPCFG2=%08x\n", dep_cfg2);

	dwc_wr32(pcd->usb3_dev, &ep_reg->depcmdpar1, dep_cfg1);
	dwc3_cil_debug("DEPCFG1=%08x\n", dep_cfg1);

	dwc_wr32(pcd->usb3_dev, &ep_reg->depcmdpar0, dep_cfg0);
	dwc3_cil_debug("DEPCFG0=%08x\n", dep_cfg0);

	dwc_wr32(pcd->usb3_dev, &ep_reg->depcmd,
	    USB_DWC_EPCMD_SET_EP_CFG | USB_DWC_EPCMD_ACT_BIT);
	dwc3_cil_debug("DEPCMD=%08x\n",
	    USB_DWC_EPCMD_SET_EP_CFG | USB_DWC_EPCMD_ACT_BIT);

	dwc_handshake(pcd->usb3_dev, &ep_reg->depcmd, USB_DWC_EPCMD_ACT_BIT, 0);

	return (0);
}

int
dwc_usb3_dep_xfercfg(const usb_dwc_pcd_t *pcd, usb_dwc_dev_ep_regs_t *ep_reg,
    uint32_t dep_strmcfg)
{
	dwc_wr32(pcd->usb3_dev, &ep_reg->depcmdpar0, dep_strmcfg);
	dwc3_cil_debug("DEPSTRMCFG=%08x\n", dep_strmcfg);

	dwc_wr32(pcd->usb3_dev, &ep_reg->depcmd,
	    USB_DWC_EPCMD_SET_XFER_CFG | USB_DWC_EPCMD_ACT_BIT);
	dwc3_cil_debug("DEPCMD=%08x\n",
	    USB_DWC_EPCMD_SET_XFER_CFG | USB_DWC_EPCMD_ACT_BIT);

	dwc_handshake(pcd->usb3_dev, &ep_reg->depcmd, USB_DWC_EPCMD_ACT_BIT, 0);

	return (0);
}

uint32_t
dwc_usb3_dep_getepstate(const usb_dwc_pcd_t *pcd,
    usb_dwc_dev_ep_regs_t *ep_reg)
{
	uint32_t ret;

	dwc_wr32(pcd->usb3_dev, &ep_reg->depcmd,
	    USB_DWC_EPCMD_GET_EP_STATE | USB_DWC_EPCMD_ACT_BIT);
	dwc3_cil_debug("DEPCMD=%08x\n",
	    USB_DWC_EPCMD_GET_EP_STATE | USB_DWC_EPCMD_ACT_BIT);

	dwc_handshake(pcd->usb3_dev, &ep_reg->depcmd, USB_DWC_EPCMD_ACT_BIT, 0);

	ret = dwc_rd32(pcd->usb3_dev, &ep_reg->depcmdpar2);

	return (ret);
}

int
dwc_usb3_dep_sstall(const usb_dwc_pcd_t *pcd, usb_dwc_dev_ep_regs_t *ep_reg)
{
	dwc_wr32(pcd->usb3_dev, &ep_reg->depcmd,
	    USB_DWC_EPCMD_SET_STALL | USB_DWC_EPCMD_ACT_BIT);
	dwc3_cil_debug("DEPCMD=%08x\n",
	    USB_DWC_EPCMD_SET_STALL | USB_DWC_EPCMD_ACT_BIT);

	dwc_handshake(pcd->usb3_dev, &ep_reg->depcmd, USB_DWC_EPCMD_ACT_BIT, 0);

	return (0);
}

/*
 * Brief: Send DEPCSTALL command to EP
 */
int
dwc_usb3_dep_cstall(const usb_dwc_pcd_t *pcd, usb_dwc_dev_ep_regs_t *ep_reg, int clr_pend)
{
	uint32_t depcmd;

	depcmd = USB_DWC_EPCMD_CLR_STALL | USB_DWC_EPCMD_ACT_BIT;
	if (clr_pend)
		depcmd |= USB_DWC_EPCMD_HP_FRM_BIT;

	dwc_wr32(pcd->usb3_dev, &ep_reg->depcmd, depcmd);
	dwc3_cil_debug("DEPCMD=%08x\n", depcmd);

	dwc_handshake(pcd->usb3_dev, &ep_reg->depcmd, USB_DWC_EPCMD_ACT_BIT, 0);

	return (0);
}

/*
 * Brife: Send DEPSTRTXFER command to EP
 */
int
dwc_usb3_dep_startxfer(const usb_dwc_pcd_t *pcd, usb_dwc_dev_ep_regs_t *ep_reg,
    usb_dwc_dma_t addr, uint32_t stream_or_uf)
{
	uint32_t retval;

	addr += USB_DMA_OFFSET;
	dwc_wr32(pcd->usb3_dev, &ep_reg->depcmdpar1, addr & 0xffffffffU);

	/* Set param 0 */
#ifdef DWC_USB3_64B_ARCH
	dwc_wr32(pcd->usb3_dev, &ep_reg->depcmdpar0, addr >> 32U & 0xffffffffU);
#else
	dwc_wr32(pcd->usb3_dev, &ep_reg->depcmdpar0, 0);
#endif
	dwc_wr32(pcd->usb3_dev, &ep_reg->depcmd,
	    stream_or_uf << USB_DWC_EPCMD_STR_NUM_OR_UF_SHIFT |
	    USB_DWC_EPCMD_START_XFER | USB_DWC_EPCMD_ACT_BIT);
	dmb();
	dwc_handshake(pcd->usb3_dev, &ep_reg->depcmd, USB_DWC_EPCMD_ACT_BIT, 0);

	retval = dwc_rd32(pcd->usb3_dev, &ep_reg->depcmd);
	if (!(retval & USB_DWC_EPCMD_ACT_BIT)) {
		uint32_t cmd_status = (retval >> USB_DWC_DEPEVT_EVENT_STATUS_NUM_SHIFT) & 0x0f;

		switch (cmd_status) {
		case 0:
			break;
		case 1:
			dwc3_cil_debug("%s %d No transfer resource available\n", __FUNCTION__, __LINE__);
			break;
		case 2:
			/* Indicates expiry of the bus time reflected in the Start Transfer command. */
			return (EINVAL);
		default:
			dwc3_cil_debug("Unknown command status\n");
			break;
		}
	}

	return (retval >> USB_DWC_EPCMD_XFER_RSRC_IDX_SHIFT &
	    USB_DWC_EPCMD_XFER_RSRC_IDX_BITS >> USB_DWC_EPCMD_XFER_RSRC_IDX_SHIFT);
}

/*
 * Brife: Send DEPUPDTXFER command to EP
 */
int
dwc_usb3_dep_updatexfer(const usb_dwc_pcd_t *pcd, usb_dwc_dev_ep_regs_t *ep_reg,
    uint32_t tri)
{
	dwc_wr32(pcd->usb3_dev, &ep_reg->depcmd,
	    tri << USB_DWC_EPCMD_XFER_RSRC_IDX_SHIFT |
	    USB_DWC_EPCMD_UPDATE_XFER | USB_DWC_EPCMD_ACT_BIT);
	dwc3_cil_debug("DEPCMD=%08x\n",
	    tri << USB_DWC_EPCMD_XFER_RSRC_IDX_SHIFT |
	    USB_DWC_EPCMD_UPDATE_XFER | USB_DWC_EPCMD_ACT_BIT);

	dwc_handshake(pcd->usb3_dev, &ep_reg->depcmd, USB_DWC_EPCMD_ACT_BIT, 0);

	return (0);
}

/*
 * Brife: Send DEPENDXFER command to EP
 */
int
dwc_usb3_dep_endxfer(const usb_dwc_pcd_t *pcd, usb_dwc_dev_ep_regs_t *ep_reg,
    uint32_t tri, uint32_t flags, void *condition)
{
	(void)condition;
	uint32_t retval;

	retval = tri << USB_DWC_EPCMD_XFER_RSRC_IDX_SHIFT | USB_DWC_EPCMD_END_XFER;
	retval |= USB_DWC_EPCMD_ACT_BIT;
	if (flags & DWC_ENDXFER_FORCE)
		retval |= USB_DWC_EPCMD_HP_FRM_BIT;

	dwc_wr32(pcd->usb3_dev, &ep_reg->depcmd, retval);
	dwc3_cil_debug("DEPCMD=%08x\n", retval);

	dwc_handshake(pcd->usb3_dev, &ep_reg->depcmd, USB_DWC_EPCMD_ACT_BIT, 0);
	if (!(flags & DWC_ENDXFER_NODELAY))
		dwc_udelay(NULL, 100);

	return (0);
}

/*
 * Send DEPSTRTNEWCFG command to EP
 */
int
dwc_usb3_dep_startnewcfg(const usb_dwc_pcd_t *pcd, usb_dwc_dev_ep_regs_t *ep_reg,
    uint32_t rsrcidx)
{
	dwc_wr32(pcd->usb3_dev, &ep_reg->depcmd,
	    rsrcidx << USB_DWC_EPCMD_XFER_RSRC_IDX_SHIFT |
	    USB_DWC_EPCMD_START_NEW_CFG | USB_DWC_EPCMD_ACT_BIT);
	dwc3_cil_debug("DEPCMD=%08x\n",
	    rsrcidx << USB_DWC_EPCMD_XFER_RSRC_IDX_SHIFT |
	    USB_DWC_EPCMD_START_NEW_CFG | USB_DWC_EPCMD_ACT_BIT);

	dwc_handshake(pcd->usb3_dev, &ep_reg->depcmd, USB_DWC_EPCMD_ACT_BIT, 0);

	return (0);
}

/*
 * Brife: Enable an EP in the DALEPENA register.
 */
int
dwc_usb3_enable_ep(usb_dwc_pcd_t *pcd_t, const usb_dwc_pcd_ep_t *ep_t)
{
	uint32_t ep_index, dalepena_val;

	ep_index = ep_t->dwc_ep.num * 2;

	if (ep_t->dwc_ep.is_in)
		ep_index += 1;

	dalepena_val = dwc_rd32(pcd_t->usb3_dev, &pcd_t->dev_global_regs->dalepena);
	if (dalepena_val & 1 << ep_index)
		return (-DWC3_USB_E_BUSY);

	dalepena_val |= 1 << ep_index;
	dwc_wr32(pcd_t->usb3_dev, &pcd_t->dev_global_regs->dalepena, dalepena_val);
	dwc3_cil_debug("enable_ep: DALEPENA=%08x\n", dalepena_val);
	return (0);
}

/*
 * Brife: Disable an EP in the DALEPENA register.
 */
int
dwc_usb3_disable_ep(usb_dwc_pcd_t *pcd_t, const usb_dwc_pcd_ep_t *ep_t)
{
	uint32_t ep_index, dalepena_val;

	ep_index = ep_t->dwc_ep.num * 2;

	if (ep_t->dwc_ep.is_in)
		ep_index += 1U;

	dalepena_val = dwc_rd32(pcd_t->usb3_dev, &pcd_t->dev_global_regs->dalepena);
	if (!(dalepena_val & 1U << ep_index))
		return (-DWC3_USB_E_INVALID);

	dalepena_val &= ~(1U << ep_index);
	dwc_wr32(pcd_t->usb3_dev, &pcd_t->dev_global_regs->dalepena, dalepena_val);
	dwc3_cil_debug("disable_ep: DALEPENA=%08x\n", dalepena_val);
	return (0);
}

/*
 * Brife: Get the device speed from the device status register and convert it to USB speed constant.
 */
int
dwc_usb3_get_device_speed(const usb_dwc_pcd_t *pcd_t)
{
	uint32_t dsts_val;
	int dev_speed = USB_SPEED_UNKNOWN;

	dsts_val = dwc_rd32(pcd_t->usb3_dev, &pcd_t->dev_global_regs->dsts);

	switch (dsts_val >> USB_DWC_DSTS_CONNSPD_SHIFT &
	    USB_DWC_DSTS_CONNSPD_BITS >> USB_DWC_DSTS_CONNSPD_SHIFT) {
	case USB_DWC_SPEED_HS_PHY_30MHZ_OR_60MHZ:
		dwc3_cil_debug("HIGH SPEED\n");
		dev_speed = USB_SPEED_HIGH;
		break;

	case USB_DWC_SPEED_FS_PHY_30MHZ_OR_60MHZ:
	case USB_DWC_SPEED_FS_PHY_48MHZ:
		dwc3_cil_debug("FULL SPEED\n");
		dev_speed = USB_SPEED_FULL;
		break;

	case USB_DWC_SPEED_LS_PHY_6MHZ:
		dwc3_cil_debug("LOW SPEED\n");
		dev_speed = USB_SPEED_LOW;
		break;

	case USB_DWC_SPEED_SS_PHY_125MHZ_OR_250MHZ:
		dwc3_cil_debug("SUPER SPEED\n");
		dev_speed = USB_SPEED_SUPER;
		break;

	default:
		break;
	}

	return (dev_speed);
}

/*
 * Brife: Get the current microframe number.
 */
int
dwc_usb3_get_frame(const usb_dwc_pcd_t *pcd_t)
{
	uint32_t dsts_val;

	dsts_val = dwc_rd32(pcd_t->usb3_dev, &pcd_t->dev_global_regs->dsts);

	return (dsts_val >> USB_DWC_DSTS_SOF_FN_SHIFT &
	    USB_DWC_DSTS_SOF_FN_BITS >> USB_DWC_DSTS_SOF_FN_SHIFT);
}

/*
 * Brife: Get the current link state.
 */
int
dwc_usb3_pcd_get_link_state(const usb_dwc_pcd_t *pcd_t)
{
	uint32_t dsts_val, state;

	dsts_val = dwc_rd32(pcd_t->usb3_dev, &pcd_t->dev_global_regs->dsts);
	state = dsts_val >> USB_DWC_DSTS_USBLNK_STATE_SHIFT &
	    USB_DWC_DSTS_USBLNK_STATE_BITS >> USB_DWC_DSTS_USBLNK_STATE_SHIFT;
	dwc3_cil_debug("status=0x%08x state=%u\n", dsts_val, state);

	return (state);
}

/*
 * Brife: Set state of USB link.
 */
void
dwc_usb3_pcd_set_link_state(usb_dwc_pcd_t *pcd_t, uint32_t state)
{
	uint32_t dctl_val;

	dctl_val = dwc_rd32(pcd_t->usb3_dev, &pcd_t->dev_global_regs->dctl);
	dctl_val &= ~USB_DWC_DCTL_ULST_CHNG_REQ_BITS;
	dwc_wr32(pcd_t->usb3_dev, &pcd_t->dev_global_regs->dctl, dctl_val);

	dctl_val = dwc_rd32(pcd_t->usb3_dev, &pcd_t->dev_global_regs->dctl);
	dctl_val &= ~USB_DWC_DCTL_ULST_CHNG_REQ_BITS;
	dctl_val |= state << USB_DWC_DCTL_ULST_CHNG_REQ_SHIFT;
	dwc_wr32(pcd_t->usb3_dev, &pcd_t->dev_global_regs->dctl, dctl_val);
}

/*
 * Brife: Send a Remote Wakeup to the host.
 */
void
dwc_usb3_pcd_remote_wake(usb_dwc_pcd_t *pcd_t, uint32_t func)
{
	if (pcd_t->speed == USB_SPEED_SUPER)
		(void)dwc_usb3_xmit_fn_remote_wake(pcd_t, func);
}

/*
 * Brife: Set the Device Address.
 */
void
dwc_usb3_set_address(usb_dwc_pcd_t *pcd_t, uint32_t addr)
{
	uint32_t dcfg_val;

	dcfg_val = dwc_rd32(pcd_t->usb3_dev, &pcd_t->dev_global_regs->dcfg);
	dcfg_val &= ~USB_DWC_DCFG_DEVADDR_BITS;
	dcfg_val |= addr << USB_DWC_DCFG_DEVADDR_SHIFT;
	dwc_wr32(pcd_t->usb3_dev, &pcd_t->dev_global_regs->dcfg, dcfg_val);
}

/*
 * Brife: Enable USB2 Phy suspend.
 */
static void
dwc_usb3_ena_usb2_phy_suspend(usb_dwc_pcd_t *pcd)
{
#ifdef LOSCFG_USB_PHY_SUSPEND_NOT_SUPPORTED
	volatile dwc_usb3_device_t *dev = pcd->usb3_dev;
	int hiber = dev->core_params->hibernate &&
	    (dev->hwparams1 & USB_DWC_HWP1_EN_PWROPT_BITS) ==
	    USB_DWC_EN_PWROPT_HIBERNATION << USB_DWC_HWP1_EN_PWROPT_SHIFT;
	int writeit = 0;
	uint32_t usb2phy_cfg;

	if (dev->core_params->phyctl || hiber || dev->core_params->lpmctl) {
		usb2phy_cfg = dwc_rd32(dev,
		    &dev->core_global_regs->gusb2phycfg[0]);

		if ((dev->core_params->phyctl || hiber) &&
		    !(usb2phy_cfg & USB_DWC_USB2PHYCFG_SUS_PHY_BIT)) {
			usb2phy_cfg |= USB_DWC_USB2PHYCFG_SUS_PHY_BIT;
			writeit = 1;
		}

		if (dev->core_params->lpmctl &&
		    !(usb2phy_cfg & USB_DWC_USB2PHYCFG_ENBL_SLP_M_BIT)) {
			usb2phy_cfg |= USB_DWC_USB2PHYCFG_ENBL_SLP_M_BIT;
			writeit = 1;
		}

		if (writeit)
			dwc_wr32(dev, &dev->core_global_regs->gusb2phycfg[0],
			    usb2phy_cfg);
	}
#endif
}

/*
 * Brife: Disable USB2 Phy suspend.
 */
static void
dwc_usb3_dis_usb2_phy_suspend(usb_dwc_pcd_t *pcd)
{
#ifdef LOSCFG_USB_PHY_SUSPEND_NOT_SUPPORTED
	uint32_t usb2phy_cfg;

	usb2phy_cfg = dwc_rd32(pcd->usb3_dev,
	    &pcd->usb3_dev->core_global_regs->gusb2phycfg[0]);

	if (usb2phy_cfg & (USB_DWC_USB2PHYCFG_SUS_PHY_BIT |
	    USB_DWC_USB2PHYCFG_ENBL_SLP_M_BIT)) {
		usb2phy_cfg &= ~USB_DWC_USB2PHYCFG_SUS_PHY_BIT;
		usb2phy_cfg &= ~USB_DWC_USB2PHYCFG_ENBL_SLP_M_BIT;
		dwc_wr32(pcd->usb3_dev,
		    &pcd->usb3_dev->core_global_regs->gusb2phycfg[0],
		    usb2phy_cfg);
	}
#endif
}

/*
 * Brife: Enable USB3 Phy suspend.
 */
static void
dwc_usb3_ena_usb3_phy_suspend(usb_dwc_pcd_t *pcd)
{
#ifdef LOSCFG_USB_PHY_SUSPEND_NOT_SUPPORTED
	uint32_t pipectl_val;

	pipectl_val = dwc_rd32(pcd->usb3_dev,
	    &pcd->usb3_dev->core_global_regs->gusb3pipectl[0]);
	if (!(pipectl_val & USB_DWC_PIPECTL_SUS_PHY_BIT)) {
		pipectl_val |= USB_DWC_PIPECTL_SUS_PHY_BIT;
		dwc_wr32(pcd->usb3_dev,
		    &pcd->usb3_dev->core_global_regs->gusb3pipectl[0],
		    pipectl_val);
	}
#endif
}

/*
 * Brife: Disable USB3 Phy suspend.
 */
static void
dwc_usb3_dis_usb3_phy_suspend(usb_dwc_pcd_t *pcd)
{
#ifdef LOSCFG_USB_PHY_SUSPEND_NOT_SUPPORTED
	uint32_t pipectl_val;

	pipectl_val = dwc_rd32(pcd->usb3_dev,
	    &pcd->usb3_dev->core_global_regs->gusb3pipectl[0]);
	if (pipectl_val & USB_DWC_PIPECTL_SUS_PHY_BIT) {
		pipectl_val &= ~USB_DWC_PIPECTL_SUS_PHY_BIT;
		dwc_wr32(pcd->usb3_dev,
		    &pcd->usb3_dev->core_global_regs->gusb3pipectl[0],
		    pipectl_val);
	}
#endif
}

/*
 * Brife: Enable USB2 Phy suspend if the device is connected at HS or FS.
 */
void
dwc_usb3_ena_usb2_suspend(usb_dwc_pcd_t *pcd_t)
{
	if (pcd_t->speed == USB_SPEED_SUPER)
		return;
	dwc3_cil_debug("%s\n", __FUNCTION__);

	dwc_usb3_ena_usb2_phy_suspend(pcd_t);
}

/*
 * Brife: Disable USB2 Phy suspend if the device is connected at HS or FS.
 */
void
dwc_usb3_dis_usb2_suspend(usb_dwc_pcd_t *pcd_t)
{
	if (pcd_t->speed == USB_SPEED_SUPER)
		return;
	dwc3_cil_debug("%s\n", __FUNCTION__);

	dwc_usb3_dis_usb2_phy_suspend(pcd_t);
}

/*
 * Brife: Enable the Device to accept U1 control commands from the Host.
 */
void
dwc_usb3_accept_u1(usb_dwc_pcd_t *pcd_t)
{
	uint32_t dctl_val;

	dctl_val = dwc_rd32(pcd_t->usb3_dev, &pcd_t->dev_global_regs->dctl);
	dctl_val |= USB_DWC_DCTL_ACCEPT_U1_EN_BIT;
	dwc_wr32(pcd_t->usb3_dev, &pcd_t->dev_global_regs->dctl, dctl_val);
}

/*
 * Brife: Enable the Device to accept U2 control commands from the Host.
 */
void
dwc_usb3_accept_u2(usb_dwc_pcd_t *pcd_t)
{
}

/*
 * Brife: Enable U1 sleep.
 */
void
dwc_usb3_enable_u1(usb_dwc_pcd_t *pcd_t)
{
	uint32_t dctl_val;

	dctl_val = dwc_rd32(pcd_t->usb3_dev, &pcd_t->dev_global_regs->dctl);
	dctl_val |= USB_DWC_DCTL_INIT_U1_EN_BIT;
	dwc_wr32(pcd_t->usb3_dev, &pcd_t->dev_global_regs->dctl, dctl_val);
}

/*
 * Brife: Enable U2 sleep.
 */
void
dwc_usb3_enable_u2(usb_dwc_pcd_t *pcd_t)
{
}

/*
 * Brife: Disable U1 sleep.
 */
void
dwc_usb3_disable_u1(usb_dwc_pcd_t *pcd_t)
{
}

/*
 * Brife: Disable U2 sleep.
 */
void
dwc_usb3_disable_u2(usb_dwc_pcd_t *pcd_t)
{
	uint32_t dctl_val;

	dctl_val = dwc_rd32(pcd_t->usb3_dev, &pcd_t->dev_global_regs->dctl);
	dctl_val &= ~USB_DWC_DCTL_INIT_U2_EN_BIT;
	dwc_wr32(pcd_t->usb3_dev, &pcd_t->dev_global_regs->dctl, dctl_val);
}

/*
 * Brife: Test whether U1 sleep is enabled.
 */
int
dwc_usb3_u1_enabled(usb_dwc_pcd_t *pcd_t)
{
	uint32_t dctl_val;

	dctl_val = dwc_rd32(pcd_t->usb3_dev, &pcd_t->dev_global_regs->dctl);
	return (!!(dctl_val & USB_DWC_DCTL_INIT_U1_EN_BIT));
}

/*
 * Brife: Test whether U2 sleep is enabled.
 */
int
dwc_usb3_u2_enabled(usb_dwc_pcd_t *pcd_t)
{
	uint32_t dctl_val;

	dctl_val = dwc_rd32(pcd_t->usb3_dev, &pcd_t->dev_global_regs->dctl);
	return (!!(dctl_val & USB_DWC_DCTL_INIT_U2_EN_BIT));
}

void
dwc_usb3_clr_eps_enabled(usb_dwc_pcd_t *pcd_t)
{
	usb_dwc_pcd_ep_t *ep_t = NULL;
	uint8_t j;

	pcd_t->eps_enabled = 0;

	for (j = 0; j < pcd_t->num_in_eps; j++) {
		ep_t = pcd_t->in_ep[j];
		if (ep_t != NULL)
			ep_t->dwc_ep.ena_once = 0;
	}

	for (j = 0; j < pcd_t->num_out_eps; j++) {
		ep_t = pcd_t->out_ep[j];
		if (ep_t != NULL)
			ep_t->dwc_ep.ena_once = 0;
	}
}

int
dwc_usb3_pcd_common_init(dwc_usb3_device_t *pdev, const volatile uint8_t *base,
    dwc_usb3_core_params_t *core_params)
{
	usb_dwc_pcd_t *pcd_t = NULL;
	u8 max_ep_num;

	pdev->core_params = core_params;
	pdev->core_global_regs = (dwc_usb3_core_global_regs_t *)
	    (base + USB_DWC_CORE_GLOBAL_REG_OFFSET);

	pcd_t = &pdev->pcd;

	pcd_t->dev_global_regs = (dwc_usb3_dev_global_regs_t *)
	    (base + USB_DWC_DEV_GLOBAL_REG_OFFSET);
	pcd_t->in_ep_regs = (usb_dwc_dev_ep_regs_t *)
	    (base + USB_DWC_DEV_IN_EP_REG_OFFSET);
	pcd_t->out_ep_regs = (usb_dwc_dev_ep_regs_t *)
	    (base + USB_DWC_DEV_OUT_EP_REG_OFFSET);

	/*
	 * Store the contents of the hardware configuration registers here for
	 * easy access later.
	 */
	pdev->hwparams0 = dwc_rd32(pdev, &pdev->core_global_regs->ghwparams0);
	pdev->hwparams1 = dwc_rd32(pdev, &pdev->core_global_regs->ghwparams1);
	pdev->hwparams2 = dwc_rd32(pdev, &pdev->core_global_regs->ghwparams2);
	pdev->hwparams3 = dwc_rd32(pdev, &pdev->core_global_regs->ghwparams3);
	pdev->hwparams4 = dwc_rd32(pdev, &pdev->core_global_regs->ghwparams4);
	pdev->hwparams5 = dwc_rd32(pdev, &pdev->core_global_regs->ghwparams5);
	pdev->hwparams6 = dwc_rd32(pdev, &pdev->core_global_regs->ghwparams6);
	pdev->hwparams7 = dwc_rd32(pdev, &pdev->core_global_regs->ghwparams7);
	pdev->hwparams8 = dwc_rd32(pdev, &pdev->core_global_regs->ghwparams8);

	max_ep_num = (pdev->hwparams3 & USB_DWC_HWP3_MAX_EP_NUMBER) >> USB_DWC_HWP3_MAX_EP_NUMBER_SHIFT;
	if ((IN_EP_NUM + OUT_EP_NUM + CONTROL_EP_NUM) > max_ep_num) {
		usb_err("IN_EP_NUM(%u) + OUT_EP_NUM(%u) + CONTROL_EP_NUM(2) is more then max num of EP %u.\n",
		    IN_EP_NUM, OUT_EP_NUM, max_ep_num);
		return (-DWC3_USB_E_INVALID);
	}

	/* Initialize parameters from Hardware configuration registers. */
	pdev->pcd.num_in_eps = IN_EP_NUM;
	dwc3_cil_debug("\n###%s,%d,dev->pcd.num_in_eps=0x%x\n", __FUNCTION__, __LINE__, pdev->pcd.num_in_eps);
	if (pdev->pcd.num_in_eps > 15) {
		dwc3_cil_debug("Number of IN endpoints too large\n");
		return (-DWC3_USB_E_INVALID);
	}

	pdev->pcd.num_out_eps = OUT_EP_NUM;
	dwc3_cil_debug("\n###%s,%d,dev->pcd.num_out_eps=0x%x\n", __FUNCTION__, __LINE__, pdev->pcd.num_out_eps);
	if (pdev->pcd.num_out_eps > 15) {
		dwc3_cil_debug("Number of OUT endpoints too large\n");
		return (-DWC3_USB_E_INVALID);
	}

	return (0);
}

/*
 * Brife: This routine frees any allocations made by dwc_usb3_pcd_common_init().
 */
void
dwc_usb3_pcd_common_remove(dwc_usb3_device_t *pdev)
{
}

int
dwc_usb3_pcd_check_snpsid(dwc_usb3_device_t *pdev, uint32_t addr_ofs)
{
	pdev->snpsid = dwc_rd32(pdev, (volatile uint32_t *)
	    (pdev->base + addr_ofs + 0x120));	/* 1018c120 */
	if ((pdev->snpsid & 0xffff0000) != 0x55330000) {
		return (-DWC3_USB_E_INVALID);
	}
	return (0);
}

/*
 * Brife: Set the size of the Tx FIFOs
 */
void
dwc_usb3_set_tx_fifo_size(const dwc_usb3_device_t *pdev, const uint32_t *size)
{
	dwc_usb3_core_global_regs_t *global_regs = pdev->core_global_regs;
	uint32_t ram_width, ram_depth, sz, prev_start, txsz[USB_DWC_MAX_TX_FIFOS];
	uint8_t j;

	ram_width = (pdev->hwparams0 >> USB_DWC_HWP0_MDWIDTH_SHIFT &
	    USB_DWC_HWP0_MDWIDTH_BITS >> USB_DWC_HWP0_MDWIDTH_SHIFT)
	    / 8;
	ram_depth = (pdev->hwparams7 >> USB_DWC_HWP7_RAM1_DEPTH_SHIFT &
	    USB_DWC_HWP7_RAM1_DEPTH_BITS >> USB_DWC_HWP7_RAM1_DEPTH_SHIFT)
	    * ram_width;
	sz = dwc_rd32(pdev, &global_regs->gtxfifosiz[0]);
	prev_start = sz >> USB_DWC_FIFOSZ_STARTADDR_SHIFT &
	    USB_DWC_FIFOSZ_STARTADDR_BITS >> USB_DWC_FIFOSZ_STARTADDR_SHIFT;

	for (j = 0; j < pdev->pcd.num_in_eps + 1; j++) {
		sz = size[j];

		if ((j == 0) && sz && (sz < 512 + 2 * ram_width)) {
			dwc3_cil_debug("Requested Tx FIFO %u size too small\n",
			    j);
			dwc3_cil_debug("Not setting Tx FIFO sizes\n");
			goto err;
		}

		if (!sz) {
			sz = j ? 1024 : 512;
			sz += 2 * ram_width;
		}

		sz = (sz + ram_width - 1) & ~(ram_width - 1);
		dwc3_cil_debug("Tx FIFO %u size = %u bytes out of %u available\n",
		    j, sz, ram_depth);
		if (sz > ram_depth) {
			dwc3_cil_debug("Requested Tx FIFO %u size too large\n",
			    j);
			dwc3_cil_debug("Not setting Tx FIFO sizes\n");
			goto err;
		}

		txsz[j] = sz;
		ram_depth -= sz;
	}

	for (j = 0; j < pdev->pcd.num_in_eps + 1; j++) {
		sz = txsz[j];
		dwc3_cil_debug("Setting GTXFIFOSIZ%u = 0x%08x\n", j,
		    (sz / ram_width) << USB_DWC_FIFOSZ_DEPTH_SHIFT |
		    prev_start << USB_DWC_FIFOSZ_STARTADDR_SHIFT);
		dwc_wr32(pdev, &global_regs->gtxfifosiz[j],
		    (sz / ram_width) << USB_DWC_FIFOSZ_DEPTH_SHIFT |
		    prev_start << USB_DWC_FIFOSZ_STARTADDR_SHIFT);
		dwc3_cil_debug("GTXFIFOSIZ%u = 0x%08x\n", j,
		   dwc_rd32(pdev, &global_regs->gtxfifosiz[j]));
		prev_start += sz / ram_width;
	}
err:
	return;
}

/*
 * Brife: Set the size of the Rx FIFO
 */
void
dwc_usb3_set_rx_fifo_size(const dwc_usb3_device_t *pdev, uint32_t sz)
{
	dwc_usb3_core_global_regs_t *global_regs =
	    pdev->core_global_regs;
	uint32_t size, rxthrcfg;
	uint32_t ram_width, ram_depth, prev_start, cnt, bst;

	ram_width = (pdev->hwparams0 >> USB_DWC_HWP0_MDWIDTH_SHIFT &
	    USB_DWC_HWP0_MDWIDTH_BITS >> USB_DWC_HWP0_MDWIDTH_SHIFT)
	    / 8;
	ram_depth = (pdev->hwparams7 >> USB_DWC_HWP7_RAM2_DEPTH_SHIFT &
	    USB_DWC_HWP7_RAM2_DEPTH_BITS >> USB_DWC_HWP7_RAM2_DEPTH_SHIFT)
	    * ram_width;
	size = dwc_rd32(pdev, &global_regs->grxfifosiz[0]);
	prev_start = size >> USB_DWC_FIFOSZ_STARTADDR_SHIFT &
	    USB_DWC_FIFOSZ_STARTADDR_BITS >> USB_DWC_FIFOSZ_STARTADDR_SHIFT;

	if (sz < 512 + 24 + 16 + (ram_width == 16 ? 24 : 0)) {
		dwc3_cil_debug("Requested Rx FIFO size too small\n");
		dwc3_cil_debug("Not setting Rx FIFO size\n");
		goto rxerr;
	}

	sz = (sz + ram_width - 1) & ~(ram_width - 1);
	if (sz > ram_depth) {
		dwc3_cil_debug("Requested Rx FIFO size too large\n");
		dwc3_cil_debug("Not setting Rx FIFO size\n");
		goto rxerr;
	}

	dwc3_cil_debug("Setting GRXFIFOSIZ0 = 0x%08x\n",
	    (sz / ram_width) << USB_DWC_FIFOSZ_DEPTH_SHIFT |
	    prev_start << USB_DWC_FIFOSZ_STARTADDR_SHIFT);
	dwc_wr32(pdev, &global_regs->grxfifosiz[0],
	    (sz / ram_width) << USB_DWC_FIFOSZ_DEPTH_SHIFT |
	    prev_start << USB_DWC_FIFOSZ_STARTADDR_SHIFT);
	dwc3_cil_debug("GRXFIFOSIZ0 = 0x%08x\n",
	    dwc_rd32(pdev, &global_regs->grxfifosiz[0]));
	dwc3_cil_debug("Rx FIFO size = %u bytes out of %u available\n",
	    sz, ram_depth);

	rxthrcfg = dwc_rd32(pdev, &global_regs->grxthrcfg);
	dwc3_cil_debug("GRXTHRCFG = 0x%08x\n", rxthrcfg);
	if (rxthrcfg & USB_DWC_RXTHRCTL_RX_PKT_CNT_EN_BIT) {
		cnt = (sz - ram_width * 4) / 1024;
		if (cnt > 0) {
			if (cnt > USB_DWC_RXTHRCTL_RX_PKT_CNT_BITS >>
				    USB_DWC_RXTHRCTL_RX_PKT_CNT_SHIFT)
				cnt = USB_DWC_RXTHRCTL_RX_PKT_CNT_BITS >>
				    USB_DWC_RXTHRCTL_RX_PKT_CNT_SHIFT;
			bst = rxthrcfg >>
			    USB_DWC_RXTHRCTL_MAX_RX_BURST_SIZE_SHIFT &
				    USB_DWC_RXTHRCTL_MAX_RX_BURST_SIZE_BITS >>
				    USB_DWC_RXTHRCTL_MAX_RX_BURST_SIZE_SHIFT;
			if (cnt > bst)
				cnt = bst;
			if (cnt < 1)
				goto disable;
			rxthrcfg &= ~USB_DWC_RXTHRCTL_RX_PKT_CNT_BITS;
			rxthrcfg |= cnt << USB_DWC_RXTHRCTL_RX_PKT_CNT_SHIFT;
		} else {
disable:
			rxthrcfg &= ~USB_DWC_RXTHRCTL_RX_PKT_CNT_EN_BIT;
		}
		dwc3_cil_debug("Setting GRXTHRCFG = 0x%08x\n", rxthrcfg);
		dwc_wr32(pdev, &global_regs->grxthrcfg, rxthrcfg);
	}
rxerr:
	return;
}

/*
 * Brife: This routine initializes the DWC_usb3 controller registers.
 */
void
dwc_usb3_pcd_device_init(dwc_usb3_device_t *pdev, int reset,
    int restore)
{
	dwc_usb3_core_global_regs_t *global_regs =
	    pdev->core_global_regs;
	usb_dwc_pcd_t *pcd_t = &pdev->pcd;
	uint32_t temp;
	uint8_t j;
	uint32_t sz;

	dwc3_cil_debug("%s()\n", __FUNCTION__);

	if (pdev->program_gsbuscfg) {	/* No entered(exit the hibernation) */
		dwc3_cil_debug("Programming SBUSCFG0,1 to %08x %08x\n",
		    pdev->gsbuscfg0, pdev->gsbuscfg1);
		dwc_wr32(pdev, &global_regs->gsbuscfg0, pdev->gsbuscfg0);
		dwc_wr32(pdev, &global_regs->gsbuscfg1, pdev->gsbuscfg1);
	}

	if (pdev->soft_reset_hook != NULL) {
		pdev->soft_reset_hook(pdev, reset, restore);
	} else {
		if (reset) {
			temp = dwc_rd32(pdev, &pcd_t->dev_global_regs->dctl);
			temp &= ~USB_DWC_DCTL_RUN_STOP_BIT;
			temp |= USB_DWC_DCTL_CSFT_RST_BIT;
			dwc_wr32(pdev, &pcd_t->dev_global_regs->dctl, temp);

			do {
				dwc_msleep(pdev, 1);
				temp = dwc_rd32(pdev, &pcd_t->dev_global_regs->dctl);
			} while (temp & USB_DWC_DCTL_CSFT_RST_BIT);

			dwc_msleep(pdev, 1);
		}
	}

	if (pdev->program_gsbuscfg) {
		dwc3_cil_debug("Programming SBUSCFG0,1 to %08x %08x\n",
		    pdev->gsbuscfg0, pdev->gsbuscfg1);
		dwc_wr32(pdev, &global_regs->gsbuscfg0, pdev->gsbuscfg0);
		dwc_wr32(pdev, &global_regs->gsbuscfg1, pdev->gsbuscfg1);
	}

	pcd_t->link_state = 0;
	pcd_t->wkup_rdy = 0;

	if (pdev->phy_config_hook != NULL) {
		pdev->phy_config_hook(pdev, reset, restore);
	}

	if (pdev->fifo_sizing_hook != NULL) {
		pdev->fifo_sizing_hook(pdev, reset, restore);
	} else {
		for (j = 0; j < pcd_t->num_in_eps + 1; j++) {
			sz = dwc_rd32(pdev, &global_regs->gtxfifosiz[j]);
			dwc3_cil_debug("Initial GTXFIFOSIZ%u = 0x%08x\n", j, sz);
		}

		if (pdev->core_params->txfsz_cnt)
			dwc_usb3_set_tx_fifo_size(pdev, pdev->core_params->txfsz);

		sz = dwc_rd32(pdev, &global_regs->grxfifosiz[0]);
		dwc3_cil_debug("Initial GRXFIFOSIZ0 = 0x%08x\n", sz);
		sz = pdev->core_params->rxfsz;

		if (sz)
			dwc_usb3_set_rx_fifo_size(pdev, sz);
	}

	if (pdev->gctl_init_hook != NULL) {
		pdev->gctl_init_hook(pdev, reset, restore);
	} else {
		temp = dwc_rd32(pdev, &global_regs->gctl);
		temp &= ~(USB_DWC_GCTL_PRT_CAP_DIR_BITS | USB_DWC_GCTL_SCALE_DOWN_BITS);
		temp |= USB_DWC_GCTL_PRT_CAP_DEVICE << USB_DWC_GCTL_PRT_CAP_DIR_SHIFT;
		temp &= ~USB_DWC_GCTL_PWR_DN_SCALE_BITS;

		switch (pdev->core_params->phy) {
		case 3: /* 16-bit UTMI+ SNPS Phy */
		case 2: /* 8-bit UTMI+ / ULPI TI or SNPS Phy */
			temp |= 0x270 << USB_DWC_GCTL_PWR_DN_SCALE_SHIFT;
			break;
		case 1: /* old 8-bit UTMI+ SNPS Phy */
			dwc_wr32(pdev, &global_regs->gusb3pipectl[0],
			    USB_DWC_PIPECTL_LFPS_FILTER_BIT |
			    1 << USB_DWC_PIPECTL_TX_DEMPH_SHIFT);

			temp |= 0x270 << USB_DWC_GCTL_PWR_DN_SCALE_SHIFT;
			break;
		default: /* RocketIO Phy */
			temp |= 0x1e84 << USB_DWC_GCTL_PWR_DN_SCALE_SHIFT |
			    USB_DWC_GCTL_DEBUG_ATTACH_BIT |
			    USB_DWC_GCTL_DIS_SCRAMBLE_BIT;
		}
		dwc_wr32(pdev, &global_regs->gctl, temp);
	}

	dwc_usb3_init_eventbuf(pdev, USB_GEVENT_BUF_NUM, pdev->event_buf[0], DWC_EVENT_BUF_SIZE,
	    pdev->event_buf_dma[0]);
	pdev->event_ptr[0] = pdev->event_buf[0];

	dwc3_cil_debug("dcfg = 0x%08lx\n", dwc_rd32(pdev, &pcd_t->dev_global_regs->dcfg));
	if (pdev->core_params->usb2mode == 1) {
		temp = dwc_rd32(pdev, &pcd_t->dev_global_regs->dcfg);
		temp &= ~(USB_DWC_DCFG_DEVSPD_BITS << USB_DWC_DCFG_DEVSPD_SHIFT);
		temp |= USB_DWC_SPEED_FS_PHY_30MHZ_OR_60MHZ
		    << USB_DWC_DCFG_DEVSPD_SHIFT;
		dwc_wr32(pdev, &pcd_t->dev_global_regs->dcfg, temp);
	} else if (pdev->core_params->usb2mode == 2) {
		temp = dwc_rd32(pdev, &pcd_t->dev_global_regs->dcfg);
		temp &= ~(USB_DWC_DCFG_DEVSPD_BITS << USB_DWC_DCFG_DEVSPD_SHIFT);
		temp |= USB_DWC_SPEED_HS_PHY_30MHZ_OR_60MHZ
		    << USB_DWC_DCFG_DEVSPD_SHIFT;
		temp |= USB_DEPCFG_INTR_NUM << 12; /* DCFG.intrNum */
		dwc_wr32(pdev, &pcd_t->dev_global_regs->dcfg, temp);
	} else {
		temp = dwc_rd32(pdev, &pcd_t->dev_global_regs->dcfg);
		temp &= ~(USB_DWC_DCFG_DEVSPD_BITS << USB_DWC_DCFG_DEVSPD_SHIFT);

		if ((pdev->hwparams3 & USB_DWC_HWP3_SSPHY_IFC_BITS) == 0)
			temp |= USB_DWC_SPEED_HS_PHY_30MHZ_OR_60MHZ
			    << USB_DWC_DCFG_DEVSPD_SHIFT;
		else
			temp |= USB_DWC_SPEED_SS_PHY_125MHZ_OR_250MHZ
			    << USB_DWC_DCFG_DEVSPD_SHIFT;
		dwc_wr32(pdev, &pcd_t->dev_global_regs->dcfg, temp);
	}

	if (pdev->core_params->lpmctl) {
		temp = dwc_rd32(pdev, &pcd_t->dev_global_regs->dcfg);
		temp |= USB_DWC_DCFG_LPM_CAP_BIT;
		dwc_wr32(pdev, &pcd_t->dev_global_regs->dcfg, temp);

		if (pdev->core_params->lpmctl > 1) {
			temp = dwc_rd32(pdev, &pcd_t->dev_global_regs->dctl);
			temp |= USB_DWC_DCTL_APP_L1_RES_BIT;
			dwc_wr32(pdev, &pcd_t->dev_global_regs->dctl, temp);
		}
	}

	if ((pdev->core_params->nump > 0) && (pdev->core_params->nump <= 16)) {
		temp = dwc_rd32(pdev, &pcd_t->dev_global_regs->dcfg);
		temp &= ~USB_DWC_DCFG_NUM_RCV_BUF_BITS;
		temp |= pdev->core_params->nump << USB_DWC_DCFG_NUM_RCV_BUF_SHIFT;
		dwc_wr32(pdev, &pcd_t->dev_global_regs->dcfg, temp);
	}
	if (pdev->set_address_hook != NULL) {
		pdev->set_address_hook(pdev, reset, restore);
	} else {
		if (!restore)
			dwc_usb3_set_address(pcd_t, 0);
	}

	if (pdev->core_params->hibernate &&
	    ((pdev->hwparams1 & USB_DWC_HWP1_EN_PWROPT_BITS) ==
	    (USB_DWC_EN_PWROPT_HIBERNATION << USB_DWC_HWP1_EN_PWROPT_SHIFT))) {
		temp = dwc_rd32(pdev, &global_regs->gctl);
		temp |= USB_DWC_GCTL_GBL_HIBER_EN_BIT;
		if (pdev->core_params->clkgatingen)
			temp &= ~USB_DWC_GCTL_DSBL_CLCK_GTNG_BIT;
		else
			temp |= USB_DWC_GCTL_DSBL_CLCK_GTNG_BIT;
		dwc_wr32(pdev, &global_regs->gctl, temp);

		if (pdev->core_params->lpmctl) {
			temp = dwc_rd32(pdev, &pcd_t->dev_global_regs->dctl);
			temp &= ~USB_DWC_DCTL_HIRD_THR_BITS;
			if (pdev->hird_thresh)
				temp |= pdev->hird_thresh << USB_DWC_DCTL_HIRD_THR_SHIFT &
				    USB_DWC_DCTL_HIRD_THR_BITS;
			else
				temp |= 0x1c << USB_DWC_DCTL_HIRD_THR_SHIFT;

			temp |= USB_DWC_DCTL_L1_HIBER_EN_BIT;
			dwc_wr32(pdev, &pcd_t->dev_global_regs->dctl, temp);
		}

		if (!restore) {
			(void)dwc_usb3_set_scratchpad_buf_array(pcd_t,
			    pcd_t->hiber_scratchpad_array_dma);
		}

#ifdef LOSCFG_USB_PHY_SUSPEND_NOT_SUPPORTED
		temp = dwc_rd32(pdev, &global_regs->gusb2phycfg[0]);
		temp |= USB_DWC_USB2PHYCFG_SUS_PHY_BIT;
		if (pdev->core_params->lpmctl)
			temp |= USB_DWC_USB2PHYCFG_ENBL_SLP_M_BIT;
		dwc_wr32(pdev, &global_regs->gusb2phycfg[0], temp);

		temp = dwc_rd32(pdev, &global_regs->gusb3pipectl[0]);
		temp |= USB_DWC_PIPECTL_SUS_PHY_BIT;
		dwc_wr32(pdev, &global_regs->gusb3pipectl[0], temp);
#endif
	} else {
		if (pdev->core_params->phyctl) {
			dwc_usb3_ena_usb3_phy_suspend(pcd_t);
			dwc_usb3_ena_usb2_phy_suspend(pcd_t);
		} else {
			dwc_usb3_dis_usb3_phy_suspend(pcd_t);
			dwc_usb3_dis_usb2_phy_suspend(pcd_t);
		}
	}

	dwc_usb3_enable_device_interrupts(pdev);

	dwc_usb3_ep0_activate(pcd_t, restore);

	if (pdev->ep0_start_hook) {
		pdev->ep0_start_hook(pdev, reset, restore);
	} else {
		dwc_usb3_pcd_ep0_out_start(pcd_t);
	}

	dwc_wr32(pdev, &pcd_t->dev_global_regs->dalepena, 3);

	pcd_t->eps_enabled = 0;

	temp = dwc_rd32(pdev, &pcd_t->dev_global_regs->dctl);
	temp |= USB_DWC_DCTL_RUN_STOP_BIT;
	if (pdev->core_params->hibernate &&
	    ((pdev->hwparams1 & USB_DWC_HWP1_EN_PWROPT_BITS) ==
	    (USB_DWC_EN_PWROPT_HIBERNATION << USB_DWC_HWP1_EN_PWROPT_SHIFT)))
		temp |= USB_DWC_DCTL_KEEP_CONNECT_BIT;
	dwc_wr32(pdev, &pcd_t->dev_global_regs->dctl, temp);
}

/*
 * Brife: This routine deinitializes the DWC_usb3 controller registers.
 */
void
dwc_usb3_pcd_device_remove(dwc_usb3_device_t *pdev)
{
	dwc_usb3_core_global_regs_t *global_regs =
	    pdev->core_global_regs;
	usb_dwc_pcd_t *pcd = &pdev->pcd;
	uint32_t temp;

	if (pdev->hibernate >= DWC_HIBER_SLEEPING)
		return;

	temp = dwc_rd32(pdev, &pcd->dev_global_regs->dctl);
	temp &= ~(USB_DWC_DCTL_RUN_STOP_BIT | USB_DWC_DCTL_KEEP_CONNECT_BIT);
	dwc_wr32(pdev, &pcd->dev_global_regs->dctl, temp);

	dwc_wr32(pdev, &pcd->dev_global_regs->devten, 0);

	if (pdev->core_params->hibernate &&
	    ((pdev->hwparams1 & USB_DWC_HWP1_EN_PWROPT_BITS) ==
	    (USB_DWC_EN_PWROPT_HIBERNATION << USB_DWC_HWP1_EN_PWROPT_SHIFT))) {
		temp = dwc_rd32(pdev, &global_regs->gusb3pipectl[0]);
		temp &= ~USB_DWC_PIPECTL_SUS_PHY_BIT;
		dwc_wr32(pdev, &global_regs->gusb3pipectl[0], temp);

		temp = dwc_rd32(pdev, &global_regs->gusb2phycfg[0]);
		temp &= ~USB_DWC_USB2PHYCFG_SUS_PHY_BIT;
		temp &= ~USB_DWC_USB2PHYCFG_ENBL_SLP_M_BIT;
		dwc_wr32(pdev, &global_regs->gusb2phycfg[0], temp);

		temp = dwc_rd32(pdev, &pcd->dev_global_regs->dctl);
		temp &= ~USB_DWC_DCTL_L1_HIBER_EN_BIT;
		dwc_wr32(pdev, &pcd->dev_global_regs->dctl, temp);

		temp = dwc_rd32(pdev, &global_regs->gctl);
		temp &= ~USB_DWC_GCTL_GBL_HIBER_EN_BIT;
		temp |= USB_DWC_GCTL_DSBL_CLCK_GTNG_BIT;
		dwc_wr32(pdev, &global_regs->gctl, temp);
	}
}
