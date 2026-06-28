/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2025. All rights reserved.
 * Description: LiteOS XHCI Controller
 * Author: Huawei LiteOS Team
 * Create: 2013-08-27
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

#include <los_driverbase.h>
#include <hal_hwi.h>
#include "implementation/global_implementation.h"
#include "core/usb_dev.h"
#include "controller/usb_host/xhci.h"
#include "controller/usb_host/xhcireg.h"

static int xhci_remove(struct LosDevice *pdev);

static int
xhci_probe(struct LosDevice *pdev)
{
	device_t dev;
	struct xhci_softc *sc;
	int err = ENXIO;
	const uint8_t usedma32 = 0;

	dev = usb_malloc(sizeof(*dev),
	    M_DEVBUF, M_WAITOK | M_ZERO);

	if (dev == NULL) {
		PRINTK("ehci_hcd root device add fail\n");
		return (ENOMEM);
	}

	TAILQ_INIT(&dev->dev_children);

	pdev->data = dev;

	device_set_desc(dev, "usb-xhci");
	err = snprintf_s(dev->dev_nameunit, sizeof(dev->dev_nameunit), sizeof(dev->dev_nameunit) - 1, "%s", "XHCI");
	if (err <= 0) {
		goto error;
	}

	sc = usb_malloc(sizeof(struct xhci_softc),
	    M_DEVBUF, M_WAITOK | M_ZERO);

	if (sc == NULL) {
		err = ENOMEM;
		goto error;
	}
	dev->dev_sc = sc;
	dev->dev_softc_alloc = 1;

	sc->sc_io_res = (void *)LOS_DeviceRegBaseGet(pdev, 0);
	sc->sc_io_tag = (void *)sc->sc_io_res;
	sc->sc_io_hdl = (uintptr_t)sc->sc_io_res;
	sc->sc_io_size = LOS_DeviceRegSizeGet(pdev, 0);

#ifdef LOSCFG_USB_XHCI_INTERRUPT_INTERVAL
	sc->sc_imod_default = LOSCFG_USB_XHCI_INTERRUPT_INTERVAL;
#endif

	err = usb_request_intr((uint32_t)LOS_DeviceIrqNumGet(pdev), (irq_handler)xhci_interrupt, sc);
	if (err) {
		goto error;
	}

	sc->sc_bus.bdev = device_add_child(dev, "usbus", -1);
	if (sc->sc_bus.bdev == NULL) {
		device_printf(dev, "Could not add USB device\n");
		goto error;
	}
	device_set_ivars(sc->sc_bus.bdev, &sc->sc_bus);

	UsbPhyStartHcd();
	UsbPhyDevice2Host();

	if (xhci_init(sc, dev, usedma32)) {
		device_printf(dev, "Could not initialize softc\n");
		goto error;
	}

	err = xhci_halt_controller(sc);
	if (err == 0)
		err = xhci_start_controller(sc);

	usb_callout_init_mtx(&sc->sc_callout, &sc->sc_bus.bus_mtx, 0);

	if (err == 0)
		err = device_probe_and_attach(sc->sc_bus.bdev);

	if (err) {
		device_printf(dev, "XHCI halt/start/probe failed err=%d\n", err);
		goto error;
	}
	/* Initialise some bus fields */
	dev->dev_attached = 1;

	return (0);
error:
	device_printf(dev, "XHCI probe failed err=%d\n", err);
	(void)xhci_remove(pdev);

	return (err);
}

static int
xhci_suspend(struct LosDevice *pst_dev)
{
	device_t bdev;
	device_t dev = pst_dev->data;

	struct xhci_softc *sc = (struct xhci_softc *)device_get_softc(dev);
	if (sc != NULL && sc->sc_bus.bdev != NULL) {
		bdev = sc->sc_bus.bdev;
		return (DEVICE_SUSPEND(bdev));
	}
	return (0);
}

static int
xhci_resume(struct LosDevice *pst_dev)
{
	device_t dev = pst_dev->data;
	device_t bdev;

	struct xhci_softc *sc = (struct xhci_softc *)device_get_softc(dev);
	if (sc != NULL && sc->sc_bus.bdev != NULL) {
		bdev = sc->sc_bus.bdev;
		return (DEVICE_RESUME(bdev));
	}
	return (0);
}

static int
xhci_remove(struct LosDevice *pdev)
{
	struct xhci_softc *sc;
	device_t bdev;
	uint32_t irq;

	device_t dev = pdev->data;
	if (dev == NULL)
		return (-1);

	sc = device_get_softc(dev);
	if (sc == NULL)
		goto disable;

	if (sc->sc_bus.bdev != NULL) {
		bdev = sc->sc_bus.bdev;
		(void)device_detach(bdev);
		(void)device_delete_child(dev, bdev);
	}
	/* During module unload there are lots of children leftover */
	(void)device_delete_children(dev);

	usb_callout_drain(&sc->sc_callout);
	(void)xhci_halt_controller(sc);

	irq = (uint32_t)LOS_DeviceIrqNumGet(pdev);

	/* Disable irq */
	(void)HalIrqMask(irq);
	(void)usb_release_intr(irq, sc);

	xhci_uninit(sc);
	if (sc != NULL) {
		usb_free(sc, M_DEVBUF);
		((device_t)(pdev->data))->dev_sc = NULL;
	}
	UsbPhyStopHcd();

disable:
	usb_free(dev, M_DEVBUF);
	pdev->data = NULL;
	return (0);
}

static struct LosDriver xhci_driver = {
	.name = "xhci",
	.ops = {
		.probe = xhci_probe,
		.remove = xhci_remove,
	},
	.pmOps = {
		.suspend = xhci_suspend,
		.resume = xhci_resume,
	},
};

int
xhci_hcd_init(void)
{
	return ((int)LOS_DriverRegister(&xhci_driver));
}

int
xhci_hcd_exit(void)
{
	return ((int)LOS_DriverUnregister(&xhci_driver));
}

