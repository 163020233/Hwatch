/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description:  os adapt api source.
 * Author:
 * Create: 2023-03-09
 */

#include "los_printf.h"
#include "los_task_pri.h"
#include "asm/memmap_config.h"
#include "los_init.h"
#include "hal_hwi.h"

#ifdef LOSCFG_SHELL
#include "soc/uart.h"
#endif

extern char __heap_start[];
extern void *g_intheap_size;
extern char __extend_heap_start;
extern void *g_extend_heap_size;

typedef VOID (*LOWPOWERIDLEHOOK)(VOID);
static LOWPOWERIDLEHOOK g_low_power_hook = NULL;
STATIC VOID OsAppInitHook(VOID) __attribute__((weakref("app_init_ram")));

typedef VOID (*SystemInitFunc)(VOID);
VOID SystemInitArray(UINTPTR start, UINTPTR end)
{
    SystemInitFunc func;
    UINTPTR *it = (UINTPTR *)start;
    UINTPTR *it_end = (UINTPTR *)end;
    while (it < it_end) {
        func = (SystemInitFunc)(*it);
        func();
        it++;
    }
    return;
}

#define MCU_TE_INT 20
/* brandy need change hwi triggle mode*/
LITE_OS_SEC_TEXT_INIT UINT32 OsAdaptInit(VOID)
{
    uint8_t clicintattr = 0;
    /* clicintattr[2:1] = 1 edge-triggered;
       clicintattr[2:1] = 0 level-triggered;
       clicintattr[7:6] = 3 M-mode */
    for (uint8_t index = 0; index < LOSCFG_PLATFORM_HWI_LIMIT; index++) {
        if (index == MCU_TE_INT) { // MCU_TE_INT(中断号4+16)使用边沿中断
            clicintattr = 0;
            clicintattr |= ((1 << 1) | (3 << 6));
            HalIrqSetClicIntAttr(20, clicintattr);
        } else {
            clicintattr = 0;
            clicintattr |= ((0 << 1) | (3 << 6));
            HalIrqSetClicIntAttr(index, clicintattr);
        }
    }
    return LOS_OK;
}
LOS_SYS_INIT(OsAdaptInit, SYS_INIT_LEVEL_APP, SYS_INIT_SYNC_1);

/* brandy need two memory pools,this init another mem pool in osMain */
typedef VOID (*FSMEMPOOLHOOK)(uint32_t *pool, uint32_t *size);
static FSMEMPOOLHOOK g_fs_mem_poll_hook = NULL;
LITE_OS_SEC_TEXT_MINOR VOID OS_FSMemPoolHookReg(FSMEMPOOLHOOK hook)
{
    g_fs_mem_poll_hook = hook;
}
#ifdef LOSCFG_FS_FAT_INDEPENDENT_MEMORY_POOL
extern UINT8 *g_fatMemPool;
#endif

#ifdef LOSCFG_FS_YAFFS_INDEPENDENT_MEMORY_POOL
extern UINT8 *g_yaffsMemPool;
#endif

LITE_OS_SEC_TEXT_INIT UINT32 OsMemAdaptInit(VOID)
{
#ifdef LOSCFG_FS_FAT_INDEPENDENT_MEMORY_POOL
    g_fatMemPool = (uint8_t *)OS_EXTEND_MEM_ADDR;
#endif

#ifdef LOSCFG_FS_YAFFS_INDEPENDENT_MEMORY_POOL
    g_yaffsMemPool = (uint8_t *)OS_EXTEND_MEM_ADDR;
#endif
#if defined(LOSCFG_FS_FAT_INDEPENDENT_MEMORY_POOL) || defined(LOSCFG_FS_YAFFS_INDEPENDENT_MEMORY_POOL)
    uint32_t pool = 0;
    uint32_t size = 0;
    if (g_fs_mem_poll_hook != NULL) {
        g_fs_mem_poll_hook(&pool, &size);
        PRINTK("%s pool:%x,size:%x.\n", __FUNCTION__, pool, size);
    } else {
        return LOS_OK;
    }
    uint32_t ret = LOS_MemInit((uint8_t *)pool, size);
    if (ret != LOS_OK) {
        PRINTK("%s LOS_FatSetMemPool fail\n", __FUNCTION__);
    }
#endif

#ifdef LOSCFG_FS_FAT_INDEPENDENT_MEMORY_POOL
    g_fatMemPool = (uint8_t *)pool;
#endif

#ifdef LOSCFG_FS_YAFFS_INDEPENDENT_MEMORY_POOL
    g_yaffsMemPool = (uint8_t *)pool;
#endif
    return LOS_OK;
}
LOS_SYS_INIT(OsMemAdaptInit, SYS_INIT_LEVEL_KERNEL, SYS_INIT_SYNC_1);

__attribute__((weak)) void app_init(void)
{
    PRINTK("%s start ok\n", __FUNCTION__);
    if (OsAppInitHook != NULL) {
        OsAppInitHook();
    }

    return;
}

#ifndef LOSCFG_KERNEL_LOWPOWER
LITE_OS_SEC_TEXT_MINOR VOID LOS_LowpowerHookReg(LOWPOWERIDLEHOOK hook)
{
    g_low_power_hook = hook;
}
#endif

LITE_OS_SEC_TEXT VOID OsIdleTask(VOID)
{
    while (1) {
        LOS_TaskResRecycle();
        OsIdleHandler();

        if (g_low_power_hook != NULL) {
            g_low_power_hook();
        }
    }
}