/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2021-2022. All rights reserved.
 * Description: osal debug adapt
 * Author:
 * Create: 2022-05-16
 */


#include <los_printf.h>
#include <los_config.h>
#include <los_printf_pri.h>
#include <stdbool.h>
#include "soc_osal.h"
#include "osal_adapt.h"
#include "los_exc.h"
#include "std_def.h"
#include "los_ld_elflib.h"
#include "los_memory.h"
#include "los_sem_pri.h"
#include "los_hwi_pri.h"
#include "los_hwi.h"
#include "los_task.h"
#include "los_ringbuf.h"
#include "platform_core.h"

#ifdef SW_UART_DEBUG
#include "debug_print.h"
#endif
#ifdef TEST_SUITE
#include "test_suite_uart.h"
#endif
#if defined(SW_RTT_DEBUG)
#include "SEGGER_RTT.h"
#endif

#ifndef SW_UART_CHIP_DEFINE
void UartPuts(const char *s, unsigned int len, BOOL isLock)
{
    if ((s == NULL) || (strlen(s) == 0)) {
        return;
    }

    UNUSED(isLock);
#ifdef SW_UART_DEBUG
    print_str(s);
#elif defined(TEST_SUITE)
    test_suite_uart_send(s);
#elif defined(SW_RTT_DEBUG)
    SEGGER_RTT_Write(0, (const char *)s, len);
#else
    UNUSED(s);
    UNUSED(len);
#endif
}
#endif

void osal_dcache_flush_all(void)
{
    ArchDCacheFlush();
}

#ifdef LOSCFG_HWI_PRE_POST_PROCESS
void osal_hwi_prehook_reg(osal_hwi_hook intprehook)
{
    LOS_HwiPreHookReg((HWI_PROC_HOOK)intprehook);
}

void osal_hwi_posthook_reg(osal_hwi_hook intposthook)
{
    LOS_HwiPostHookReg((HWI_PROC_HOOK)intposthook);
}
#endif

void osal_task_switch_hook_reg(osal_task_switch_hook swihook)
{
    LOS_TaskSwitchHookReg(swihook);
}

void osal_reset_caller_ra(void)
{
    LOS_ResetCallerRa();
}

void osal_save_caller_ra(void *caller_ra)
{
    LOS_SaveCallerRa((UINTPTR)caller_ra);
}

bool osal_dyn_mem_pool_set(void *mem_pool)
{
    return LOS_DynMemPoolSet(mem_pool);
}

void *osal_find_sym_by_name(void *handle, char *name)
{
    return LOS_FindSymByName(handle, name);
}

int osal_module_unload(void *handle)
{
    return LOS_ModuleUnload(handle);
}

void *osal_so_load(const char *elf_filename)
{
    return LOS_SoLoad(elf_filename);
}

int osal_bcache_sync_by_name(const char *name)
{
    return LOS_BcacheSyncByName(name);
}

unsigned int osal_taskinfo_get(unsigned int taskId, osal_tsk_info_s *taskInfo)
{
    TSK_INFO_S tmpInfo;
    unsigned int ret;

    if (taskInfo == NULL) {
        return OSAL_FAILURE;
    }

    /* 调用底层接口获取任务原始信息 */
    ret = LOS_TaskInfoGet(taskId, &tmpInfo);
    if (ret != LOS_OK) {
        return OSAL_FAILURE;
    }

    /* 安全拷贝任务名称 */
    ret = (unsigned int)strncpy_s(taskInfo->acName, OSAL_TASK_NAMELEN,
                                  tmpInfo.acName, OSAL_TASK_NAMELEN - 1);
    if (ret != EOK) {
        return OSAL_FAILURE;
    }

    /* 将字段映射到 OSAL 结构体 */
    taskInfo->uwTaskID     = (unsigned int)tmpInfo.uwTaskID;
    taskInfo->usTaskStatus = (unsigned short)tmpInfo.usTaskStatus;
    taskInfo->usTaskPrio   = (unsigned short)tmpInfo.usTaskPrio;
    taskInfo->pTaskSem     = tmpInfo.pTaskSem;
    taskInfo->pTaskMux     = tmpInfo.pTaskMux;
    taskInfo->uwEventMask  = (unsigned int)tmpInfo.uwEventMask;
    taskInfo->uwStackSize  = (unsigned int)tmpInfo.uwStackSize;

    taskInfo->uwTopOfStack    = (void *)tmpInfo.uwTopOfStack;
    taskInfo->uwBottomOfStack = (void *)tmpInfo.uwBottomOfStack;
    taskInfo->uwSP            = (void *)tmpInfo.uwSP;

    taskInfo->uwCurrUsed = (unsigned int)tmpInfo.uwCurrUsed;
    taskInfo->uwPeakUsed = (unsigned int)tmpInfo.uwPeakUsed;
    taskInfo->bOvf       = (bool)tmpInfo.bOvf;

    return OSAL_SUCCESS;
}

unsigned int osal_ringbuf_init(osal_ringbuf *ringbuf, char *fifo,
                               unsigned int size, osal_ringbuf_flag flag)
{
    /* 直接转换枚举类型（底层枚举值一致） */
    RingbufFlag rflag = (RingbufFlag)flag;

    /* 直接复用底层初始化 */
    if (LOS_RingbufInit((Ringbuf *)ringbuf, fifo, size, rflag) != LOS_OK) {
        return OSAL_FAILURE;
    }
    return OSAL_SUCCESS;
}

unsigned int osal_ringbuf_write(osal_ringbuf *ringbuf, const char *buf,
                                unsigned int size)
{
    return LOS_RingbufWrite((Ringbuf *)ringbuf, buf, size);
}

unsigned int osal_ringbuf_read(osal_ringbuf *ringbuf, char *buf,
                               unsigned int size)
{
    return LOS_RingbufRead((Ringbuf *)ringbuf, buf, size);
}