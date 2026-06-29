/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2016-2023. All rights reserved.
 * Description: LiteOS USB Driver UDC Implementation
 * Author: Huawei LiteOS Team
 * Create: 2016-07-06
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

#ifdef LOSCFG_DRIVERS_USB2_PORT_DISCONNECT
#include <los_swtmr.h>
#endif
#ifdef LOSCFG_DRIVERS_BASE
#include <los_driverbase.h>
#endif
#include "kal_tick.h"
#include "kal_atomic.h"
#include "kal_irq.h"
#include "controller/usb_device/dwc_otg_pcd_intr.h"
#include "gadget/composite_gadget.h"
#include "implementation/_intr_ref.h"
#include "phy/usb_phy.h"
#include "controller/usb_device/udc.h"

/* The flag indicates whether in the interrupt callback */
static KalAtomic intr_active_flag = 0;

#ifdef LOSCFG_DRIVERS_BASE
#define USB_DEVICE_INTERRUPT_NUM(pdev)	LOS_DeviceIrqNumGet(pdev)
#else
#define USB_DEVICE_INTERRUPT_NUM(pdev)	NUM_HAL_INTERRUPT_USB_DEV
#endif

KAL_SPIN_LOCK_INIT(g_gadget_dev_lock);
static struct gadget_device *g_usb_gadget_dev = NULL;

static int udc_gadget_deinit(void *pdev);

static void
notify_dev_refresh(void)
{
	UsbPhyDevice2Host();
	KAL_Msleep(20);
	UsbPhyHost2Device();
	KAL_Msleep(20);
}

static void
udc_interrupt(int irq, void *arg)
{
	KAL_AtomicInc(&intr_active_flag);
	(void)dwc_otg_irq(arg);
	KAL_AtomicDec(&intr_active_flag);
}

#ifdef LOSCFG_DRIVERS_USB2_PORT_DISCONNECT
typedef struct driver_context {
	void *drv_ep0_context;
	void *drv_in_ep_context[PCD_MAX_EPS - 1];
	void *drv_out_ep_context[PCD_MAX_EPS - 1];
} driver_context;

static void
driver_context_push(dwc_otg_pcd_t *pcd, driver_context *context)
{
	uint8_t i;

	context->drv_ep0_context = pcd->usbd_ep0.priv;
	for (i = 1; i <= pcd->pcd_num_eps; i++) {
		if (i <= pcd->num_in_eps) {
			context->drv_in_ep_context[i - 1] = pcd->usbd_in_eps[i - 1].priv;
		}

		if (i <= pcd->num_out_eps) {
			context->drv_out_ep_context[i - 1] = pcd->usbd_out_eps[i - 1].priv;
		}
	}
}

static void
driver_context_pop(dwc_otg_pcd_t *pcd, driver_context *context)
{
	uint8_t i;

	pcd->usbd_ep0.priv = context->drv_ep0_context;
	for (i = 1; i <= pcd->pcd_num_eps; i++) {
		if (i <= pcd->num_in_eps) {
			pcd->usbd_in_eps[i - 1].priv = context->drv_in_ep_context[i - 1];
		}

		if (i <= pcd->num_out_eps) {
			pcd->usbd_out_eps[i - 1].priv = context->drv_out_ep_context[i - 1];
		}
	}
}

void
udc_recover(struct udc_softc *sc)
{
	driver_context context;
	UsbPhyClearDeviceState();
	dwc_otg_core_do_dev_disconnect(&sc->sc_pcd);

	UsbPhyStopHcd();
	UsbPhyStartHcd();

	driver_context_push(&sc->sc_pcd, &context);
	(void)dwc_otg_initial(&sc->sc_pcd);
	driver_context_pop(&sc->sc_pcd, &context);
}

void
udc_check_port_disconnect(void *arg)
{
	struct udc_softc *sc = (struct udc_softc *)arg;
	int first_soffn;
	int diff_soffn = 0;
	int cur_soffn;
	int i;

	if (UsbPhyIsDeviceMode() == true) {
		cur_soffn = dwc_otg_get_soffn();
		sc->sofrec.record[sc->sofrec.cur_record++] = cur_soffn;
		sc->sofrec.cur_record %= MAX_SOF_RECORD_NUM;

		first_soffn = sc->sofrec.record[0];
		for (i = 1; i < MAX_SOF_RECORD_NUM; i++) {
			if ((!first_soffn) || (first_soffn != sc->sofrec.record[i])) {
				diff_soffn = 1;
				break;
			}
		}
		if (!diff_soffn) {
			udc_recover(sc);
		}
	}
}
#endif

static int
udc_intr_request(void *pdev, struct gadget_device *dev)
{
	struct udc_softc *sc = dev->udc_sc;
	int err;

	err = usb_request_intr(USB_DEVICE_INTERRUPT_NUM(pdev), udc_interrupt, &sc->sc_pcd);
	if (err) {
		return (-1);
	}

	return (err);
}

static void
udc_intr_release(void *pdev, struct gadget_device *dev)
{
	uint32_t flags;
	struct udc_softc *sc = dev->udc_sc;
	(void)sc;

	/* Release udc irq. */
	KAL_SpinLockSave(&sc->sc_pcd.irq_lock, &flags);
	(void)usb_release_intr(USB_DEVICE_INTERRUPT_NUM(pdev), NULL);
	KAL_SpinUnlockRestore(&sc->sc_pcd.irq_lock, flags);
}

static int
udc_attach(void *pdev, struct gadget_device *dev)
{
	if (composite_attach(dev) != 0) {
		KAL_Printf("WARNING: Probe and attach failed!\n");
		return (-1);
	}
	dev->udc_attached = 1;

	KAL_Printf("  ** composite attach success **\n");

	if (udc_intr_request(pdev, dev)) {
		return (-1);
	}

	return (0);
}

/*
 * Before uninstalling the protocol, ensure that the interrupt callback has complete,
 * avoiding resource races between the two.
 */
static void
udc_remove_wait(struct udc_softc *sc)
{
	/* The longest time to wait for the interrupt callback to complete. */
	const uint32_t timeout = 1000;
	uint32_t count = 0;
	uint32_t intr_status = dwc_otg_intr_status_get(&sc->sc_pcd);

	/* Interrupt trigger or interrupt callback is being executed. */
	if (intr_status || KAL_AtomicRead(&intr_active_flag)) {

		/* Waiting to ensure interrupt function call. */
		KAL_Msleep(100);

		/* Waiting for the interrupt callback to complete. */
		while (KAL_AtomicRead(&intr_active_flag)) {
			KAL_Msleep(10);
			count += 10;
			if (count == timeout) {
				break;
			}
		}
	}
}

static int
udc_detach(void *pdev, struct gadget_device *dev)
{
	int ret = 0;

	(void)KAL_IrqDisable(USB_DEVICE_INTERRUPT_NUM(pdev));
	udc_remove_wait(dev->udc_sc);
	udc_intr_release(pdev, dev);

	if (dev->udc_attached) {
		ret = composite_detach(dev);
		if (ret) {
			(void)udc_intr_request(pdev, dev);
			return (ret);
		}
		dev->udc_attached = 0;
	}

	notify_dev_refresh();
	UsbPhyDevice2Host();
	UsbPhyClearDeviceState();

	return (ret);
}

/*
 * pdev will only be used when LOSCFG_DRIVERS_BASE is enabled, provided by the driver base framework,
 * and the process ensures that pdev is not NULL.
 * when LOSCFG_DRIVERS_BASE is not enabled, pdev will not be used when loading the device,
 * so the input parameter is NULL.
 */
static int
udc_gadget_init(void *pdev)
{
	struct gadget_device *dev = NULL;
	struct udc_softc *sc = NULL;
	int err;

	dev = zalloc(sizeof(struct gadget_device));
	g_usb_gadget_dev = dev;
	if (dev == NULL) {
		KAL_Printf("udc root device add fail\n");
		goto error;
	}

	sc = zalloc(sizeof(struct udc_softc));
	if (sc == NULL) {
		goto error;
	}
	dev->udc_sc = sc;

	UsbPhyStartHcd();
	err = dwc_otg_initial(&sc->sc_pcd);
	if (err) {
		goto error;
	}

	UsbPhyHost2Device();

#ifdef LOSCFG_DRIVERS_USB2_PORT_DISCONNECT
	if (!device_is_uvc()) {
		if (LOS_SwtmrCreate(LOS_MS2Tick(1000), LOS_SWTMR_MODE_PERIOD, (SWTMR_PROC_FUNC)udc_check_port_disconnect,
		    &sc->timer_id, (uintptr_t)sc) != KAL_OK) {
			goto error;
		}
		(void)LOS_SwtmrStart(sc->timer_id);
		sc->timer_flag = 1;
	}
#endif

	err = udc_attach(pdev, dev);
	if (err < 0) {
		goto error;
	}

	return (0);
error:
	(void)udc_gadget_deinit(pdev);

	return (ENXIO);
}

/*
 * pdev will only be used when LOSCFG_DRIVERS_BASE is enabled, provided by the driver base framework,
 * and the process ensures that pdev is not NULL.
 * when LOSCFG_DRIVERS_BASE is not enabled, pdev will not be used when unloading the device,
 * so the input parameter is NULL.
 */
static int
udc_gadget_deinit(void *pdev)
{
	struct gadget_device *dev = g_usb_gadget_dev;
	struct udc_softc *sc = NULL;
	uint32_t int_save;

	if (dev == NULL) {
		return (-1);
	}

	KAL_SpinLockSave(&g_gadget_dev_lock, &int_save);

	sc = dev->udc_sc;
	if (sc == NULL) {
		goto disable;
	}

	if (udc_detach(pdev, dev) < 0) {
		KAL_SpinUnlockRestore(&g_gadget_dev_lock, int_save);
		return (-1);
	}

#ifdef LOSCFG_DRIVERS_USB2_PORT_DISCONNECT
	if (!device_is_uvc() && sc->timer_flag) {
		(void)LOS_SwtmrDelete(sc->timer_id);
		sc->timer_flag = 0;
	}
#endif

	UsbPhyStopHcd();
	(void)dwc_otg_close(&sc->sc_pcd);
	free(sc);
	dev->udc_sc = NULL;

disable:
	free(dev);
	g_usb_gadget_dev = NULL;
	KAL_SpinUnlockRestore(&g_gadget_dev_lock, int_save);
	return (0);
}

#ifdef LOSCFG_DRIVERS_BASE
static int
udc_probe(struct LosDevice *pdev)
{
	return (udc_gadget_init(pdev));
}

static int
udc_remove(struct LosDevice *pdev)
{
	return (udc_gadget_deinit(pdev));
}

static int
udc_suspend(struct LosDevice *pdev)
{
	(void)pdev;
	return (0);
}

static int
udc_resume(struct LosDevice *pdev)
{
	(void)pdev;
	return (0);
}

static struct LosDriver udc_driver = {
	.name = "udc",
	.ops = {
		.probe = udc_probe,
		.remove = udc_remove,
	},
	.pmOps = {
		.suspend = udc_suspend,
		.resume = udc_resume,
	},
};
#endif

int
udc_init(void)
{
	int ret;

#ifdef LOSCFG_DRIVERS_BASE
	ret = (int)LOS_DriverRegister(&udc_driver);
#else
	ret = udc_gadget_init(NULL);
#endif

	return (ret);
}

int
udc_exit(void)
{
	int ret;

#ifdef LOSCFG_DRIVERS_BASE
	ret = (int)LOS_DriverUnregister(&udc_driver);
#else
	ret = udc_gadget_deinit(NULL);
#endif

	return (ret);
}

bool
usb_device_is_host_suspended(void)
{
	struct udc_softc *sc = NULL;
	bool ret = false;
	uint32_t int_save;

	KAL_SpinLockSave(&g_gadget_dev_lock, &int_save);
	if ((g_usb_gadget_dev != NULL) && (g_usb_gadget_dev->udc_sc != NULL)) {
		sc = g_usb_gadget_dev->udc_sc;
		if (sc->sc_pcd.lx_state == USB_DWC2_STATE_L2) {
			ret = true;
		}
	}
	KAL_SpinUnlockRestore(&g_gadget_dev_lock, int_save);

	return (ret);
}

int
usb_device_remote_wakeup(void)
{
	struct udc_softc *sc = NULL;
	uint32_t int_save;

	KAL_SpinLockSave(&g_gadget_dev_lock, &int_save);
	if ((g_usb_gadget_dev != NULL) && (g_usb_gadget_dev->udc_sc != NULL)) {
		sc = g_usb_gadget_dev->udc_sc;
		if (sc->sc_pcd.remote_wakeup_enable && (sc->sc_pcd.lx_state == USB_DWC2_STATE_L2)) {
			KAL_SpinUnlockRestore(&g_gadget_dev_lock, int_save);
			return (dwc_otg_remote_wakeup());
		}
	}
	KAL_SpinUnlockRestore(&g_gadget_dev_lock, int_save);

	return (-1);
}