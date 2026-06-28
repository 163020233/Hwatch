/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description:  os board source.
 * Author:
 * Create: 2023-03-09
 */

#include <linux/module.h>
#include <linux/platform_device.h>
#include "arch/regs.h"
#include "arch/cache.h"

#ifdef LOSCFG_DRIVERS_BASE
#include "los_driverbase.h"
#endif
#include "asm/platform.h"
#include "asm/delay.h"
#include "soc/uart.h"
#include "los_task_pri.h"
#include "los_tick_pri.h"
#include "los_percpu_pri.h"

#ifdef LOSCFG_WCFS_SCHEDULER
#include "los_cfsqueue_pri.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

uintptr_t g_usb_mem_addr_start __attribute__((section(".data"))) = 0;
unsigned long g_usb_mem_size __attribute__((section(".data"))) = 0;

#define USBC_SYS_INT_IRQN_NUM (16 + 71)

#ifdef LOSCFG_SHARED_STACK
/*
在此处设置共享栈白名单，名单内的任务参与共享
只有参与共享的任务才能参与压缩
*/
const TaskBlackList g_sharedTask[LOSCFG_BASE_CORE_TSK_LIMIT + 1] = {
    //              taskName               加入共享？                            加入压缩？
    // Example：{   "app_Task",             TRUE,                                TRUE},
};
#endif

#ifdef LOSCFG_DRIVERS_USB
#if defined(LOSCFG_DRIVERS_USB_HOST_XHCI)
struct LosDeviceRegs g_xhci_regs[] = {
    {
        .base = 0x5B000000, /* usb ctrl base */
        .size = 0x10000,
    },
};

static struct LosDevice g_usb_host = {
    .name = "xhci",
    .id = -1,
    .cfg =
        {
            .irqNum = USBC_SYS_INT_IRQN_NUM,
            .numRegs = (sizeof(g_xhci_regs) / sizeof(g_xhci_regs[0])),
            .regs = g_xhci_regs,
        },
};
#endif // LOSCFG_DRIVERS_USB_HOST_XHCI
#if defined(LOSCFG_DRIVERS_USB_GADGET)
/* usb */
struct LosDeviceRegs g_udc3_regs[] = {
    {
        .base = 0x5B000000, /* usb ctrl base */
        .size = 0xC000,
    },
};
static struct LosDevice g_usb_device = {
    .name = "udc3",
    .id = -1,
    .cfg =
        {
            .irqNum = USBC_SYS_INT_IRQN_NUM,
            .numRegs = (sizeof(g_udc3_regs) / sizeof(g_udc3_regs[0])),
            .regs = g_udc3_regs,
        },
};
#endif

int usb_machine_init(void)
{
#if defined(LOSCFG_DRIVERS_USB_HOST_XHCI)
    if (LOS_DeviceRegister(&g_usb_host) != LOS_OK) {
        return LOS_NOK;
    }
#endif // LOSCFG_DRIVERS_USB_HOST_XHCI
#if defined(LOSCFG_DRIVERS_USB_GADGET)
    LOS_DeviceRegister(&g_usb_device);
#endif

    return 0;
}

arch_initcall(usb_machine_init);
#endif

void dma_cache_clean(UINTPTR start, UINTPTR end)
{
    ArchDCacheCleanByAddr(start, end);
}

void dma_cache_inv(UINTPTR start, UINTPTR end)
{
    ArchDCacheInvByAddr(start, end);
}

void OsCacheInit(void)
{
    dsb();
    ArchICacheInvAll();
    ArchDCacheInvAll();
    dsb();

    (void)ArchICacheEnable(CACHE_64KB);
    (void)ArchICachePrefetchEnable(CACHE_PREF_1_LINES);

    (void)ArchDCacheEnable(CACHE_64KB);
    (void)ArchDCachePrefetchEnable(CACHE_PREF_1_LINES, CACHE_PREF_1_LINES);

    return;
}

uint32_t oal_get_sleep_ticks(VOID)
{
    uint32_t intSave;
    intSave = LOS_IntLock();
    uint32_t taskTimeout = OsSortLinkGetNextExpireTime(&(OsPercpuGet()->taskSortLink));
#ifdef LOSCFG_BASE_CORE_SWTMR
    uint32_t swtmrTimeout = OsSortLinkGetNextExpireTime(&(OsPercpuGet()->swtmrSortLink));
    if (swtmrTimeout < taskTimeout) {
        taskTimeout = swtmrTimeout;
    }
#endif
    LOS_IntRestore(intSave);
    return taskTimeout;
}

VOID oal_ticks_restore(uint32_t ticks)
{
    uint32_t intSave;
    intSave = LOS_IntLock();
    g_tickCount[ArchCurrCpuid()] += ticks;
    uint32_t taskTimeout = OsSortLinkGetNextExpireTime(&(OsPercpuGet()->taskSortLink));
    if (taskTimeout > ticks) {
        OsSortLinkUpdateExpireTime(ticks + 1, &(OsPercpuGet()->taskSortLink));
    } else {
        OsSortLinkUpdateExpireTime(taskTimeout, &(OsPercpuGet()->taskSortLink));
    }
#ifdef LOSCFG_BASE_CORE_SWTMR
    uint32_t swtmrTimeout = OsSortLinkGetNextExpireTime(&(OsPercpuGet()->swtmrSortLink));
    if (swtmrTimeout > ticks) {
        OsSortLinkUpdateExpireTime(ticks + 1, &(OsPercpuGet()->swtmrSortLink));
    } else {
        OsSortLinkUpdateExpireTime(swtmrTimeout, &(OsPercpuGet()->swtmrSortLink));
    }
#endif
    LOS_IntRestore(intSave);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
