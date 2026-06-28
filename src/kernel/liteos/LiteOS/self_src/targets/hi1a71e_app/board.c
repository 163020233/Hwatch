/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: os board source
 * Author: Huawei LiteOS Team
 * Create: 2025-01-15
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
#include "linux/platform_device.h"
#include "asm/platform.h"
#include "asm/interrupt_config.h"
#include "arch/regs.h"
#include "arch/cache.h"
#ifdef LOSCFG_DRIVERS_BASE
#include "los_driverbase.h"
#endif
#include "los_init.h"
#include "los_task_pri.h"
#include "los_tick_pri.h"
#include "los_percpu_pri.h"
#include "chip_io.h"

#ifdef LOSCFG_DRIVERS_USB
#include "soc/usb3.h"
#endif
#include "mmc_porting.h"
#ifdef LOSCFG_SHARED_STACK
#include "los_task_pri.h"
#endif
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

#define LOW_SECTORS_PER_BLOCK 32

VOID dma_cache_clean(UINTPTR start, UINTPTR end)
{
    ArchDCacheCleanByAddr(start, end);
}

VOID dma_cache_inv(UINTPTR start, UINTPTR end)
{
    ArchDCacheInvByAddr(start, end);
}

VOID OsCacheInit(VOID)
{
    dsb();
    ArchICacheInvAll();
    ArchDCacheInvAll();
    dsb();

    (VOID)ArchICacheEnable(CACHE_32KB);
    (VOID)ArchICachePrefetchEnable(CACHE_PREF_1_LINES);

    (VOID)ArchDCacheEnable(CACHE_32KB);
    (VOID)ArchDCachePrefetchEnable(CACHE_PREF_1_LINES, CACHE_PREF_1_LINES);
}

UINT32 oal_get_sleep_ticks(VOID)
{
    UINT32 intSave;
    intSave = LOS_IntLock();
    UINT32 taskTimeout = OsSortLinkGetNextExpireTime(&(OsPercpuGet()->taskSortLink));
#ifdef LOSCFG_BASE_CORE_SWTMR
    UINT32 swtmrTimeout = OsSortLinkGetNextExpireTime(&(OsPercpuGet()->swtmrSortLink));
    if (swtmrTimeout < taskTimeout) {
        taskTimeout = swtmrTimeout;
    }
#endif
    LOS_IntRestore(intSave);
    return taskTimeout;
}

VOID oal_ticks_restore(UINT32 ticks)
{
    UINT32 intSave = LOS_IntLock();
    g_tickCount[ArchCurrCpuid()] += ticks;
    UINT32 taskTimeout = OsSortLinkGetNextExpireTime(&(OsPercpuGet()->taskSortLink));
    if (taskTimeout >= ticks) {
        OsSortLinkUpdateExpireTime(ticks, &(OsPercpuGet()->taskSortLink));
    } else {
        OsSortLinkUpdateExpireTime(taskTimeout, &(OsPercpuGet()->taskSortLink));
    }
#ifdef LOSCFG_BASE_CORE_SWTMR
    UINT32 swtmrTimeout = OsSortLinkGetNextExpireTime(&(OsPercpuGet()->swtmrSortLink));
    if (swtmrTimeout >= ticks) {
        OsSortLinkUpdateExpireTime(ticks, &(OsPercpuGet()->swtmrSortLink));
    } else {
        OsSortLinkUpdateExpireTime(swtmrTimeout, &(OsPercpuGet()->swtmrSortLink));
    }

#endif
    LOS_IntRestore(intSave);
}

#ifdef LOSCFG_DRIVERS_MMC
#ifdef USE_MMC0
static struct resource sdmmc0_resources[] = {
    {
        .start = EMMC_REG_BASE_ADDESS,
        .end   = EMMC_REG_END_ADDRESS,
        .flags = IORESOURCE_MEM,
    },
    {
        .start = EMMC_INTERRUPT_IRQ,
        .end   = EMMC_INTERRUPT_IRQ,
        .flags = IORESOURCE_IRQ,
    },
    {
        .start = GLB_CTL_M_RB_SOFT_RST_6_REG,
        .end   = GLB_CTL_M_RB_SOFT_RST_6_REG,
        .flags = IORESOURCE_REG,
    },
};

struct platform_device device_sdmmc0 = {
    .name          = "sdhci0",
    .id            = 0,
    .resource      = sdmmc0_resources,
    .num_resources = sizeof(sdmmc0_resources) / sizeof(sdmmc0_resources[0]),
};

static UINT32 EmmcDeviceRegister(VOID)
{
    UINT32 ret = 0;
    ret = (UINT32)platform_device_register(&device_sdmmc0);
    if (ret != 0) {
        PRINT_ERR("platform_device_register sdmmc0 error! ret: %d.\n", ret);
        return ret;
    }
    return ret;
}
#endif
#endif
UINT32 MachineInit(void)
{
    return LOS_OK;
}

VOID BoardConfig(VOID)
{
    OsCacheInit();

    OsSetMainTask();
    OsCurrTaskSet(OsGetMainTask());
}

LITE_OS_SEC_TEXT_INIT UINT32 OsAdaptInit(VOID)
{
    UINT8 clicIntattr = 0;

    /*
     * clicIntattr[2:1] = 1 edge-triggered;
     * clicIntattr[2:1] = 0 level-triggered;
     * clicIntattr[7:6] = 3 M-mode
     */
    clicIntattr |= ((0 << 1) | (3 << 6));
    for (UINT8 index = 0; index < LOSCFG_PLATFORM_HWI_LIMIT; index++) {
        HalIrqSetClicIntAttr(index, clicIntattr);
    }
    OsCacheInit();
    return LOS_OK;
}

LOS_SYS_INIT(OsAdaptInit, SYS_INIT_LEVEL_APP, SYS_INIT_SYNC_1);
