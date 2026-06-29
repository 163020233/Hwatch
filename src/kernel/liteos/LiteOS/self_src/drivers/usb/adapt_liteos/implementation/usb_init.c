/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2023. All rights reserved.
 * Description: Usb Init File
 * Author: Huawei LiteOS Team
 * Create: 2013-01-01
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

#include "kal_mem.h"
#include "kal_mutex.h"
#include "kal_atomic.h"
#include "kal_base.h"
#include "usb_api_pri.h"
#include "phy/usb_phy.h"
#include "usb_os_adapt.h"
#include "implementation/usb_init.h"
#include "implementation/usb_dma_cache.h"
#include "implementation/_printf_ref.h"
#ifdef LOSCFG_DRIVERS_USB_HOST_DRIVER
#include "implementation/global_implementation.h"
#endif

typedef struct usb_info {
	bool is_initialized;
	controller_type ctype;
	device_type dtype;
} usb_info_t;

#define INVALID_TYPE 0xFFFF
#define USB_IDLE_STATUS      0
#define USB_INIT_STATUS      1

static bool uvc_enable = false;
static usb_info_t usb_info = { false, (controller_type)INVALID_TYPE, (device_type)INVALID_TYPE };
uint32_t usb_mtx = INVALID_MUX;
static KalAtomic usb_init_atomic = USB_IDLE_STATUS;

#ifdef LOSCFG_DRIVERS_USB_HOST_DRIVER
static struct module_data *bsd_driver_module_list[] = {
#ifdef LOSCFG_DRIVERS_USB_HOST_EHCI
	/* xxx controller modules */
	/* usb generial controller modules */
	&bsd_usbus_ehci_driver_mod,
#endif

#if defined (LOSCFG_DRIVERS_USB_HOST_XHCI)
	&bsd_usbus_xhci_driver_mod,
#endif

#ifdef LOSCFG_DRIVERS_USB_HOST_DRIVER
	/* xxx driver modules */
	&bsd_uhub_uhub_driver_mod,
	&bsd_uhub_usbus_driver_mod,
#endif

#ifdef LOSCFG_DRIVERS_USB_MASS_STORAGE
	&bsd_umass_uhub_driver_mod,
#endif

#ifdef LOSCFG_DRIVERS_USB_HOST_UVC_CLASS
	&bsd_uvc_uhub_driver_mod,
#endif

#if LOSCFG_DRIVERS_USB_HOST_UAC_CLASS
	&bsd_uaudio_uhub_driver_mod,
	&bsd_pcm_uaudio_driver_mod,
#endif

#ifdef LOSCFG_DRIVERS_USB_HOST_HID
	&bsd_uhid_uhub_driver_mod,
#endif

#ifdef LOSCFG_DRIVERS_USB_NET_ASIX
	&bsd_axe_uhub_driver_mod,
#endif
	NULL
};
#endif

static void
usbinfo_set(controller_type ctype, device_type dtype)
{
	if ((dtype == DEV_UVC) || (dtype == DEV_CAMERA)) {
		uvc_enable = true;
	}
	usb_info.ctype = ctype;
	usb_info.dtype = dtype;
	usb_info.is_initialized = true;
}

static void
usbinfo_clean(void)
{
	uvc_enable = false;
	usb_info.ctype = (controller_type)INVALID_TYPE;
	usb_info.dtype = (device_type)INVALID_TYPE;
	usb_info.is_initialized = false;
}

device_type
dev_type_get(void)
{
	return (usb_info.dtype);
}

bool
device_is_uvc(void)
{
	return (uvc_enable);
}

#ifdef LOSCFG_DRIVERS_USB_HOST_DRIVER
static void usb_load_host_class_drv(void)
{
	uint32_t i = 0;
	uint32_t driver_module_list_cnt = sizeof(bsd_driver_module_list) / sizeof(bsd_driver_module_list[0]);
	/* init quirk */
	usb_quirk_init(NULL);

	for (i = 0; i < driver_module_list_cnt; i++) {
		if (bsd_driver_module_list[i] == NULL)
			break;

		KAL_Printf("Loading USB Driver Module: %s Over %s\r\n",
					bsd_driver_module_list[i]->mod_name, bsd_driver_module_list[i]->bus_name);
		module_register(bsd_driver_module_list[i]);
	}
	devclass_module_dump();
}

static void usb_unload_host_class_drv(void)
{
	uint32_t i = 0;
	uint32_t driver_module_list_cnt = sizeof(bsd_driver_module_list) / sizeof(bsd_driver_module_list[0]);
	for (i = driver_module_list_cnt; i > 0 ; i--) {
		if (bsd_driver_module_list[i - 1] != NULL) {
			KAL_Printf("Unloading USB Driver Module: %s Over %s\r\n",
						bsd_driver_module_list[i - 1]->mod_name, bsd_driver_module_list[i - 1]->bus_name);
			module_unregister(bsd_driver_module_list[i - 1]);
		}
	}
}
#endif

static uint32_t
usb_loadonce(void)
{
	KAL_Printf("usb %s\n", fetach_usbversion());

#ifdef LOSCFG_DRIVERS_USB

#ifdef LOSCFG_DRIVERS_USB_HOST_STANDALONE_MEM_POOL
	uint32_t mem_pool;
	unsigned long mem_size;
	usb_host_mem_pool_get(&mem_pool, &mem_size);
	uint32_t ret = KAL_MemInit((void *)mem_pool, mem_size);
#else
	/* init dma memory */
	uint32_t ret = KAL_MemInit((void *)g_usb_mem_addr_start, g_usb_mem_size);
#endif
	if (ret != KAL_OK) {
		KAL_Printf("*** usb init memory error 0x%x!! ***\n\n", ret);
		return (1);
	}

#ifdef LOSCFG_DRIVERS_USB_HOST_DRIVER
	usb_load_host_class_drv();
#endif // LOSCFG_DRIVERS_USB_HOST_DRIVER

#endif

	return (0);
}

static void
usb_unloadonce(void)
{
#ifdef LOSCFG_DRIVERS_USB

#ifdef LOSCFG_DRIVERS_USB_HOST_DRIVER
	usb_unload_host_class_drv();
#endif

#ifdef LOSCFG_DRIVERS_USB_HOST_STANDALONE_MEM_POOL
	uint32_t mem_pool;
	unsigned long mem_size;
	usb_host_mem_pool_get(&mem_pool, &mem_size);

#ifdef LOSCFG_MEM_MUL_POOL
	(void)KAL_MemDeInit((void *)mem_pool);
#endif

	(void)memset_s((void *)mem_pool, mem_size, 0, mem_size);
#else // LOSCFG_DRIVERS_USB_HOST_STANDALONE_MEM_POOL

#ifdef LOSCFG_MEM_MUL_POOL
	(void)KAL_MemDeInit((void *)g_usb_mem_addr_start);
#endif

	(void)memset_s((void *)g_usb_mem_addr_start, g_usb_mem_size, 0, g_usb_mem_size);
#endif // LOSCFG_DRIVERS_USB_HOST_STANDALONE_MEM_POOL

#endif // LOSCFG_DRIVERS_USB
}

/*
 * step1: Modify DRIVER_MODULE, register all host driver modules or
 * modify g_usb_dev_init, register callback functions for all device protocols
 * step2: Load host controller(ehci/xhci) or device controlle(dwc2.0/dwc3.0)
 * step3: Load usb host or device protocol
 */
uint32_t
usb_init(controller_type ctype, device_type dtype)
{
	uint32_t ret = KAL_NOK;
    bool usb_loaded = false;

	UsbPhyInit();

	if (KAL_AtomicCmpXchg32(&usb_init_atomic, USB_INIT_STATUS, USB_IDLE_STATUS)) {
		return (LOS_NOK);
	}

	/*
	 * After the mutex resource application is successful, it will not be deleted
	 * because LiteOS does not support global static mutexes.
	 */
	if (usb_mtx == INVALID_MUX) {
		if (KAL_MutexInit(&usb_mtx) != KAL_OK) {
			KAL_AtomicSet(&usb_init_atomic, USB_IDLE_STATUS);
			return (LOS_NOK);
		}
	}
	KAL_AtomicSet(&usb_init_atomic, USB_IDLE_STATUS);
	KAL_Printf("\n******** %s in **********\n", __FUNCTION__);

	if (KAL_MutexAcquire(&usb_mtx) != KAL_OK) {
		return (LOS_NOK);
	}

	if (usb_info.is_initialized) {
		KAL_Printf("\n duplicate usb_init %s, ctype:%d dtype:%d\n", __FUNCTION__, usb_info.ctype, usb_info.dtype);
		(void)KAL_MutexRelease(&usb_mtx);
		return (LOS_NOK);
	}
	usbinfo_set(ctype, dtype);

    if ((usb_loaded == false)) {
		if (usb_loadonce()) {
			goto err;
		}
	}

	if (ctype == DEVICE) {
		if ((dtype == DEV_START) || (dtype >= DEV_END)) {
			usb_err("device type is not supported\n");
			goto err;
		}

#if defined (LOSCFG_DRIVERS_USB2_DEVICE_CONTROLLER)
		ret = (uint32_t)udc_init();
#elif defined (LOSCFG_DRIVERS_USB3_DEVICE_CONTROLLER)
		ret = (uint32_t)udc3_init();
#elif defined (LOSCFG_DRIVERS_HiUSBC3X_CONTROLLER)
		ret = (uint32_t)hiusbc_init();
#endif
	} else if (ctype == HOST) {
#if defined (LOSCFG_DRIVERS_USB_HOST_XHCI)
		usb_lock_init(&Giant);
		ret = (uint32_t)xhci_hcd_init();
#endif
	} else {
		usb_err("controller type %d is error\n", ctype);
		goto err;
	}

	if (ret != KAL_OK) {
#if defined (LOSCFG_DRIVERS_USB_HOST_XHCI)
		if (ctype == HOST) {
			usb_lock_destroy(&Giant);
		}
#endif
		goto err;
	}

	usb_loaded = true;
	(void)KAL_MutexRelease(&usb_mtx);
	KAL_Printf("******** %s ok**********\n\n", __FUNCTION__);
	return (LOS_OK);

err:
	usbinfo_clean();
	(void)KAL_MutexRelease(&usb_mtx);
	if (!usb_loaded) {
		usb_unloadonce();
	}
	KAL_Printf("******** %s fail**********\n\n", __FUNCTION__);

	return (LOS_NOK);
}

uint32_t
usb_deinit(void)
{
	uint32_t ret = KAL_NOK;

	KAL_Printf("******** %s in **********\n\n", __FUNCTION__);
	if (KAL_MutexAcquire(&usb_mtx) != KAL_OK) {
		return (LOS_NOK);
	}

	if (usb_info.is_initialized == false) {
		KAL_Printf("******** %s out, no init **********\n\n", __FUNCTION__);
		goto err;
	}

	if (usb_info.ctype == DEVICE) {
#if defined (LOSCFG_DRIVERS_USB2_DEVICE_CONTROLLER)
		ret = (uint32_t)udc_exit();
#elif defined (LOSCFG_DRIVERS_USB3_DEVICE_CONTROLLER)
		ret = (uint32_t)udc3_exit();
#elif defined (LOSCFG_DRIVERS_HiUSBC3X_CONTROLLER)
		ret = (uint32_t)hiusbc_exit();
#endif
	} else if (usb_info.ctype == HOST) {
#if defined (LOSCFG_DRIVERS_USB_HOST_XHCI)
		usb_lock_destroy(&Giant);
		ret = (uint32_t)xhci_hcd_exit();
#endif
	}
	if (ret) {
		KAL_Printf("******** %s fail, %u **********\n\n", __FUNCTION__, ret);
		goto err;
	}
	usbinfo_clean();
	KAL_Printf(" ** %s success **\n", __FUNCTION__);
	(void)KAL_MutexRelease(&usb_mtx);
	usb_unloadonce();
	return (LOS_OK);

err:
	(void)KAL_MutexRelease(&usb_mtx);
	return (LOS_NOK);
}

bool
usb_is_devicemode(void)
{
	return (UsbPhyIsDeviceMode());
}
