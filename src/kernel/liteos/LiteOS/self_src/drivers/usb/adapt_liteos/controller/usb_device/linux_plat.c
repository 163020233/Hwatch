/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2014-2023. All rights reserved.
 * Description: LiteOS USB Controller
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

#include <los_atomic.h>
#ifdef LOSCFG_DRIVERS_BASE
#include <los_driverbase.h>
#endif
#include <stdlib.h>
#include "controller/usb_device/os_defs.h"
#include "controller/usb_device/hw.h"
#include "controller/usb_device/pcd.h"
#include "controller/usb_device/dev.h"
#include "controller/usb_device/os_dev.h"
#include "controller/usb_device/cil.h"
#include "implementation/_intr_ref.h"
#include "gadget/composite_gadget.h"
#include "phy/usb_phy.h"

#define dwc3_lp_show(x...) dprintf(x)

#ifdef DWC_USB3_LINUX_PLAT_DEBUG
#define dwc3_lp_debug(x...) dwc3_lp_show(x)
#else
#define dwc3_lp_debug(x...) do {} while(0)
#endif

#ifdef LOSCFG_DRIVERS_BASE
#define USB_DEVICE_INTERRUPT_NUM(pdev)	LOS_DeviceIrqNumGet(pdev)
#define USB_DEVICE_REG_SIZE(pdev)	LOS_DeviceRegSizeGet(pdev, 0)
#define USB_DEVICE_REG_BASE(pdev)	LOS_DeviceRegBaseGet(pdev, 0)
#else
#define USB_DEVICE_INTERRUPT_NUM(pdev)	NUM_HAL_INTERRUPT_USB_DEV
#define USB_DEVICE_REG_SIZE(pdev)	DWC_USB3_PORT1_ADDR_OFFSET
#define USB_DEVICE_REG_BASE(pdev)	DWC_USB3_PORT1_BASE_ADDR

static struct gadget_device *g_usb_gadget_dev = NULL;
#endif

/* The flag indicates whether in the interrupt callback */
static Atomic g_intr_active_flag = 0;

/* Enable this to support the Synopsys Xplorer platform */
static int udc3_gadget_deinit(struct gadget_device **gadget_dev);

dwc_usb3_core_params_t dwc_usb3_module_params = {
	.burst			= 1,
	.newcore		= 0,
	.phy			= 2,
	.wakeup			= 0,
	.pwrctl			= 3,
	.lpmctl			= 1,
	.phyctl			= 1,
	.usb2mode		= USB_MODE,
	.hibernate		= 0,
	.hiberdisc		= 1,
	.clkgatingen	= 1,
	.ssdisquirk		= 1,
	.nobos			= 0,
	.loop			= 0,
	.nump			= 16,
	.newcsr			= 1,
	.rxfsz			= 0,
	.txfsz			= { 0, },
	.txfsz_cnt		= 0,
	.besl			= 0,
	.baseline_besl	= 0,
	.deep_besl		= 0,
	.ebc			= 0,
};

/*
 * Hook to override the default Phy configuration in dwc_usb3_pcd_device_init()
 * with a HAPS-specific one
 */
static void
haps_phy_config_hook(struct usb3_dwc_device *dev, int soft_reset, int restore)
{
	dwc_usb3_core_global_regs_t *global_regs = dev->core_global_regs;
	uint32_t temp;

	switch (dev->core_params->phy) {
	case 3:		/*  16-bit UTMI+ SNPS Phy */
		temp = dwc_rd32(dev, &global_regs->gusb2phycfg[0]);
		temp &= ~USB_DWC_USB2PHYCFG_USB_TRD_TIM_BITS;
		temp |= USB_DWC_USB2PHYCFG_16B_PHY_IF_BIT;
		temp |= 5 << USB_DWC_USB2PHYCFG_USB_TRD_TIM_SHIFT;
		dwc_wr32(dev, &global_regs->gusb2phycfg[0], temp);
		break;
	case 2:		/* 8-bit UTMI+ / ULPI TI or SNPS Phy */
	case 1:		/* old 8-bit UTMI+ SNPS Phy */
		temp = dwc_rd32(dev, &global_regs->gusb2phycfg[0]);
		temp &= ~USB_DWC_USB2PHYCFG_USB_TRD_TIM_BITS;
		temp &= ~USB_DWC_USB2PHYCFG_16B_PHY_IF_BIT;
		temp |= 9 << USB_DWC_USB2PHYCFG_USB_TRD_TIM_SHIFT;
		dwc_wr32(dev, &global_regs->gusb2phycfg[0], temp);
		break;
	default:	/* RocketIO Phy */
		if (dev->core_params->usb2mode == 0) {
			/* Set rx-eq, differential swing */
			dwc_wr32(dev, (volatile uint32_t *)(dev->base + dev->gasket_ofs + 8), 0x41);

			dwc_wr32(dev, (volatile uint32_t *)(dev->base + dev->gasket_ofs + 4), 0);
		}
		break;
	}
}

/*
 * This function is the top level interrupt handler for the Common
 * (Core and Device) interrupts.
 */
static void
dwc_usb3_common_irq(int irq, void *dev)
{
	dwc_usb3_device_t *usb3_dev = dev;

	LOS_AtomicInc(&g_intr_active_flag);

	spin_lock(&usb3_dev->pcd.lock);
	(void)dwc_usb3_irq(usb3_dev, irq);
	spin_unlock(&usb3_dev->pcd.lock);

	LOS_AtomicDec(&g_intr_active_flag);
}

static int
dwc_usb3_intr_request(dwc_usb3_device_t *dev)
{
	int ret;

	/* Install the interrupt handler for the common interrupts. */
	dev->cmn_irq_installed = 1;
	ret = usb_request_intr(dev->irq, dwc_usb3_common_irq, dev);
	if (ret) {
		dev->cmn_irq_installed = 0;
		dwc3_lp_show("request of irq%u failed!, ret:%d\n", dev->irq, ret);
	}

	return (ret);
}

static void
dwc_usb3_intr_release(dwc_usb3_device_t *dev)
{
	uint32_t flags;

	/* Release dwc_usb3 irq. */
	spin_lock_irqsave(&dev->pcd.lock, flags);
	if (dev->cmn_irq_installed) {
		dev->cmn_irq_installed = 0;
		(void)usb_release_intr(dev->irq, NULL);
		dwc3_lp_debug("%s:irq free.\n", __FUNCTION__);
	}
	spin_unlock_irqrestore(&dev->pcd.lock, flags);
}

static int
udc3_attach(struct gadget_device *dev)
{
	if (composite_attach(dev) != 0) {
		dprintf("WARNING: Probe and attach failed!\n");
		return (-1);
	}
	dev->udc_attached = 1;

	dprintf("  ** composite attach success **\n");

	return (0);
}

static int
udc3_detach(struct gadget_device *dev)
{
	int ret = 0;

	if (dev->udc_attached) {
		ret = composite_detach(dev);
		if (ret) {
			return (ret);
		}
		dev->udc_attached = 0;
	}

	return (ret);
}

/*
 * pdev will only be used when LOSCFG_DRIVERS_BASE is enabled, provided by the driver base framework,
 * and the process ensures that pdev is not NULL.
 * when LOSCFG_DRIVERS_BASE is not enabled, pdev will not be used when loading the device,
 * so the input parameter is NULL.
 */
static int
udc3_gadget_init(void *pdev, struct gadget_device **gadget_dev)
{
	struct gadget_device *dev = NULL;
	dwc_usb3_device_t *usb3_dev = NULL;
	uint32_t addr_ofs;
	int retval = -ENOMEM;

	dev = zalloc(sizeof(struct gadget_device));
	*gadget_dev = dev;
	if (dev == NULL) {
		dprintf("udc3 root device add fail\n");
		return (retval);
	}

	dwc3_lp_debug("Entering function [%s]\n", __FUNCTION__);

	usb3_dev = zalloc(sizeof(dwc_usb3_device_t));
	if (usb3_dev == NULL)
		goto error;

	dev->udc_sc = usb3_dev;

	UsbPhyStartHcd();
	UsbPhyHost2Device();

	usb3_dev->irq = (uint32_t)USB_DEVICE_INTERRUPT_NUM(pdev);
	addr_ofs = (uint32_t)USB_DEVICE_REG_SIZE(pdev);
	usb3_dev->base = (volatile uint8_t *)USB_DEVICE_REG_BASE(pdev);
	if (usb3_dev->base == NULL) {
		dwc3_lp_show("ioremap_nocache() failed!\n");
		goto error;
	}

	if (dwc_usb3_module_params.newcsr)
		usb3_dev->gasket_ofs = 0xf000;
	else
		usb3_dev->gasket_ofs = 0x80000;

	usb3_dev->sysfs_initialized = 0;

	/*
	 * Checks that this device is really a DWC_usb3 controller. Also saves
	 * the SNPSID register value in usb3_dev->snpsid for later use by the
	 * PCD.
	 */
	retval = dwc_usb3_pcd_check_snpsid(usb3_dev, addr_ofs);
	if (retval) {
		dwc3_lp_show("bad value for SNPSID!\n");
		goto error;
	}

	if (dwc_usb3_module_params.newcore && (usb3_dev->snpsid < 0x5533109a))
		usb3_dev->snpsid = 0x5533109a;

	/* Alloc dma for event_buf. */
	usb3_dev->event_buf[0] = dwc_usb3_dma_alloc(DWC_EVENT_BUF_SIZE * sizeof(uint32_t));
	if (usb3_dev->event_buf[0] == NULL) {
		retval = -ENOMEM;
		dwc3_lp_show("allocation of event_buf failed!\n");
		goto error;
	}
	usb3_dev->event_buf_dma[0] = (usb_dwc_dma_t)usb3_dev->event_buf[0];

	/* Add our hook to override the default Phy register setup. */
	usb3_dev->phy_config_hook = haps_phy_config_hook;

	/* Initialize the DWC_usb3 Core. */
	retval = dwc_usb3_pcd_common_init(usb3_dev, usb3_dev->base + addr_ofs, &dwc_usb3_module_params);
	if (retval) {
		dwc3_lp_show("CIL initialization failed!\n");
		goto error;
	}
	usb3_dev->cmn_initialized = 1;

	spin_lock_init(&usb3_dev->pcd.lock);

	/* Initialize the Gadget. */
	retval = dwc_usb3_gadget_init(usb3_dev, NULL);
	if (retval) {
		dwc3_lp_show("gadget initialization failed!\n");
		goto error;
	}
	usb3_dev->gadget_initialized = 1;

	/* Initialize the PCD */
	retval = dwc_usb3_pcd_init(usb3_dev);
	if (retval) {
		dwc3_lp_show("PCD initialization failed!\n");
		goto error;
	}
	usb3_dev->pcd_initialized = 1;
    UsbPhyReleaseCtrl();

	retval = udc3_attach(dev);
	if (retval < 0) {
		goto error;
	}

	retval = dwc_usb3_intr_request(usb3_dev);
	if (retval) {
		goto error;
	}

	return (0);
error:
	(void)udc3_gadget_deinit(gadget_dev);
	return (retval);
}

/*
 * Before uninstalling the protocol, ensure that the interrupt callback has complete,
 * avoiding resource races between the two.
 */
static int
udc3_remove_wait(dwc_usb3_device_t *dev)
{
	/* The longest time to wait for the interrupt callback to complete. */
	const uint32_t timeout = 1000;
	uint32_t count = 0;
	int event_cnt = dwc_usb3_get_eventbuf_count(dev);

	/* Interrupt trigger or interrupt callback is being executed. */
	if (event_cnt || LOS_AtomicRead(&g_intr_active_flag)) {
		/* Waiting to ensure interrupt function call. */
		LOS_Msleep(100);

		/* Waiting for the interrupt callback to complete. */
		while (LOS_AtomicRead(&g_intr_active_flag)) {
			LOS_Msleep(10);
			count += 10;
			if (count == timeout) {
				dwc3_lp_show("%s timeout!\n", __FUNCTION__);
				break;
			}
		}
	}

	return (event_cnt);
}

static int
udc3_gadget_deinit(struct gadget_device **gadget_dev)
{
	struct gadget_device *dev = *gadget_dev;
	dwc_usb3_device_t *usb3_dev = NULL;
	uint32_t *event_buf = NULL;
	int ret;
	int event_cnt = 0;

	if (dev == NULL) {
		return (-1);
	}

	usb3_dev = dev->udc_sc;
	if (usb3_dev == NULL) {
		/* Memory allocation for the usb3_dwc_device failed */
		dwc3_lp_show("%s: usb3_dev NULL\n", __FUNCTION__);
		goto disable;
	}

	(void)LOS_HwiDisable(usb3_dev->irq);

	event_buf = usb3_dev->event_buf[0];
	if (event_buf != NULL) {
		event_cnt = udc3_remove_wait(usb3_dev);
	}

	dwc_usb3_intr_release(usb3_dev);

	ret = udc3_detach(dev);
	if (ret) {
		(void)dwc_usb3_intr_request(usb3_dev);
		return (ret);
	}

	if (usb3_dev->pcd_initialized) {
		usb3_dev->pcd_initialized = 0;
		dwc_usb3_pcd_remove(usb3_dev);
		dwc3_lp_debug("%s:pcd free.\n", __FUNCTION__);
	}

	if (usb3_dev->gadget_initialized) {
		usb3_dev->gadget_initialized = 0;
		dwc_usb3_gadget_remove(usb3_dev);
		dwc3_lp_debug("%s:gadget free.\n", __FUNCTION__);
	}

	if (usb3_dev->cmn_initialized) {
		usb3_dev->cmn_initialized = 0;
		dwc_usb3_pcd_common_remove(usb3_dev);
		dwc3_lp_debug("%s:pcd_common free.\n", __FUNCTION__);
	}

	if (event_buf != NULL) {
		dwc_usb3_update_eventbuf_count(usb3_dev, event_cnt);
		dwc_usb3_dis_flush_eventbuf_intr(usb3_dev, USB_GEVENT_BUF_NUM);
		usb3_dev->event_buf[0] = NULL;
		usb3_dev->event_buf_dma[0] = 0;
		dwc_usb3_dma_free(event_buf);
		dwc3_lp_debug("%s:event_buf free.\n", __FUNCTION__);
	}
	UsbPhyStopHcd();
	UsbPhyClearDeviceState();

	free(usb3_dev);
	dev->udc_sc = NULL;

disable:
	free(dev);
	*gadget_dev = NULL;
	return (0);
}

#ifdef LOSCFG_DRIVERS_BASE
static int
udc3_probe(struct LosDevice *pdev)
{
	return (udc3_gadget_init(pdev, (struct gadget_device **)&pdev->data));
}

static int
udc3_remove(struct LosDevice *pdev)
{
	return (udc3_gadget_deinit((struct gadget_device **)&pdev->data));
}

static int
udc3_suspend(struct LosDevice *pdev)
{
	(void)pdev;
	return (0);
}

static int
udc3_resume(struct LosDevice *pdev)
{
	(void)pdev;
	return (0);
}

static struct LosDriver udc3_driver = {
	.name = "udc3",
	.ops = {
		.probe = udc3_probe,
		.remove = udc3_remove,
	},
	.pmOps = {
		.suspend = udc3_suspend,
		.resume = udc3_resume,
	},
};
#endif

int
udc3_init(void)
{
	int ret;

#ifdef LOSCFG_DRIVERS_BASE
	ret = (int)LOS_DriverRegister(&udc3_driver);
#else
	ret = udc3_gadget_init(NULL, &g_usb_gadget_dev);
#endif

	return (ret);
}

int
udc3_exit(void)
{
	int ret;

#ifdef LOSCFG_DRIVERS_BASE
	ret = (int)LOS_DriverUnregister(&udc3_driver);
#else
	ret = udc3_gadget_deinit(&g_usb_gadget_dev);
#endif

	return (ret);
}
