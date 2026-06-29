/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2020. All rights reserved.
 * Description: System Init Implementation
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

#include "los_config.h"
#include "los_init_pri.h"
#include "stdio.h"
#include "los_sys.h"
#include "los_tick_pri.h"
#include "los_task_pri.h"
#include "los_printf.h"
#include "los_swtmr_pri.h"
#include "los_sched_pri.h"
#include "los_memory_pri.h"
#include "los_sem_pri.h"
#include "los_mux_pri.h"
#include "los_queue_pri.h"
#include "los_hwi_pri.h"
#include "los_spinlock.h"
#include "los_mp_pri.h"
#include "los_printf_pri.h"
#include "soc/uart.h"
#include "los_init.h"

#ifdef LOSCFG_FS_VFS
#include "los_fs.h"
#endif

#ifdef LOSCFG_KERNEL_TRACE
#include "los_trace.h"
#include "los_trace_pri.h"
#endif

#ifdef LOSCFG_KERNEL_CPUP
#include "los_cpup_pri.h"
#endif

#ifdef LOSCFG_KERNEL_DYNLOAD
#include "los_ld_initlib_pri.h"
#endif

#ifdef LOSCFG_KERNEL_RUNSTOP
#include "lowpower/los_runstop_pri.h"
#endif

#ifdef LOSCFG_KERNEL_LMS
#include "los_lms_pri.h"
#endif

#ifdef LOSCFG_DRIVERS_RANDOM
#include <sys/cdefs.h>
#endif
#ifdef LOSCFG_SHELL_DMESG
#include "dmesg_pri.h"
#endif
#ifdef LOSCFG_SHELL_LK
#include "shell_pri.h"
#endif

#ifdef LOSCFG_TEST
#include "los_test_pri.h"
#endif
#ifdef LOSCFG_DRIVERS_BASE
#include "los_driverbase_pri.h"
#endif

#include "arch/exception.h"

#ifdef LOSCFG_BASE_IPC_RWSEM
#include "los_rwsem_pri.h"
#endif

#ifdef LOSCFG_MEM_TASK_STAT
#include "los_memstat_pri.h"
#endif

#ifdef LOSCFG_HWI_STACK_CHECK
#include "los_hwi_dfx_pri.h"
#endif

__attribute__((section(".data.init"))) UINTPTR g_sys_mem_addr_end;

#ifdef LOSCFG_EXC_INTERACTION
__attribute__((section(".data.init"))) UINTPTR g_excInteractMemSize = 0;
#endif

STATIC LosTaskCB  g_taskCBArray[LOSCFG_BASE_CORE_TSK_LIMIT + 1];

#ifdef LOSCFG_TASK_STACK_STATIC_ALLOCATION
STATIC UINT8      g_idleTaskStack[LOSCFG_KERNEL_CORE_NUM][LOSCFG_BASE_CORE_TSK_IDLE_STACK_SIZE] LITE_OS_ATTR_ALIGN(LOSCFG_STACK_POINT_ALIGN_SIZE);
#endif

#ifdef LOSCFG_BASE_IPC_QUEUE
STATIC LosQueueCB g_allQueue[LOSCFG_BASE_IPC_QUEUE_LIMIT];
#endif

#ifdef LOSCFG_BASE_IPC_MUX
STATIC LosMuxCB   g_allMux[LOSCFG_BASE_IPC_MUX_LIMIT];
#endif

#ifdef LOSCFG_BASE_IPC_SEM
STATIC LosSemCB   g_allSem[LOSCFG_BASE_IPC_SEM_LIMIT];
#endif

#ifdef LOSCFG_BASE_IPC_RWSEM
STATIC OsRwsemCB  g_allRwsem[LOSCFG_BASE_IPC_RWSEM_LIMIT];
#endif

#ifdef LOSCFG_BASE_CORE_SWTMR
STATIC LosSwtmrCB g_swtmrCBArray[LOSCFG_BASE_CORE_SWTMR_LIMIT];
LITE_OS_SEC_BSS STATIC UINTPTR g_expirybuf[LOSCFG_KERNEL_CORE_NUM][LOSCFG_BASE_CORE_SWTMR_LIMIT];
#ifndef LOSCFG_BASE_CORE_SWTMR_IN_ISR
#ifdef LOSCFG_TASK_STACK_STATIC_ALLOCATION
STATIC UINT8 g_swtmrTaskStack[LOSCFG_KERNEL_CORE_NUM][LOSCFG_BASE_CORE_TSK_SWTMR_STACK_SIZE] LITE_OS_ATTR_ALIGN(LOSCFG_STACK_POINT_ALIGN_SIZE);
#endif
#endif
#endif

#ifdef LOSCFG_HARDWARE_STACK_CHECK_ENABLE
UINTPTR   g_irq_topOfStack;
#endif
/* temp task blocks for booting procedure */
LITE_OS_SEC_BSS LosTaskCB                g_mainTask[LOSCFG_KERNEL_CORE_NUM];

#ifdef LOSCFG_KENREL_BULITIN_ROM
static const HalClockOps g_romClockOps = {
    .clockInit       = HalClockInit,
    .clockStart      = HalClockStart,
    .clockGetCycles  = HalClockGetCycles,
    .delayUs         = HalDelayUs,
};

STATIC VOID OsRegisterExcOps(VOID)
{
#ifdef LOSCFG_ARCH_RISCV32
    g_excOps.osExcTrapHandle = TrapHandle;
#endif
}

STATIC VOID OsRegisterTick(VOID)
{
    OsHalClockOpsReg(&g_romClockOps);
}
#endif

#ifdef LOSCFG_MEM_TASK_STAT

#ifdef LOSCFG_KERNEL_MEM_SHARED_HEAP
#define MEM_STAT_TASK_NUM 128
#else
#define MEM_STAT_TASK_NUM LOSCFG_BASE_CORE_TSK_LIMIT
#endif

TaskMemUsedInfo g_taskMemstats[MEM_STAT_TASK_NUM + 1] = {0};
#endif

#if (defined(LOSCFG_MEM_DFX_SHOW_CALLER_RA) || defined(LOSCFG_MEM_DFX_DOUBLE_FREE_CHECK))
UINTPTR g_callerRA[LOSCFG_BASE_CORE_TSK_LIMIT + 1] = {0};
#endif

STATIC VOID OsRegisterBackTraceOps(VOID)
{
#if defined(LOSCFG_BACKTRACE_FP) || defined(LOSCFG_BACKTRACE_SP)
    g_backtraceOps.osBackTrace = ArchBackTrace;
    g_backtraceOps.osTaskBackTrace = ArchBackTraceWithStackPointer;
#endif
}

STATIC VOID OsRegisterCpupOps(VOID)
{
#ifdef LOSCFG_KERNEL_CPUP
    g_cpupOps.cpupCBIrqCreate = OsCpupCBIrqCreate;
    g_cpupOps.cpupCBTaskCreate = OsCpupCBTaskCreate;
    g_cpupOps.cpupCBTaskDelete = OsCpupCBTaskDelete;
    g_cpupOps.taskCycleEndStart = OsTaskCycleEndStart;
#ifdef LOSCFG_CPUP_INCLUDE_IRQ
    g_cpupOps.cpupIrqStart = OsCpupIrqStart;
    g_cpupOps.cpupIrpEnd = OsCpupIrqEnd;
#endif
#endif
}

STATIC VOID OsRegisterPrintOps(VOID)
{
    g_printOps.osPrintFunc = (VOID (*)(const CHAR *, UINT32, BOOL))UartPuts;
    g_printOps.osVprintFunc = vsnprintf_s;
}

STATIC VOID OsRegisterStackCheckOps(VOID)
{
#ifdef LOSCFG_BASE_CORE_TSK_MONITOR
    g_stackcheckOps.osTaskSwitchCheck = OsTaskSwitchCheck;
    g_stackcheckOps.osExcStackCheck = OsExcStackCheck;
#endif
}

STATIC VOID OsRegisterConsoleIDSetOps(VOID)
{
#ifdef LOSCFG_KERNEL_CONSOLE
    g_consoleidsetOps.osConsoleIDSet = OsSetConsoleID;
#endif
}

VOID *OsGetMainTask(VOID)
{
    return (g_mainTask + ArchCurrCpuid());
}

LITE_OS_SEC_TEXT_INIT_LEVEL_ONE VOID OsSetMainTask(VOID)
{
    UINT32 i;
    for (i = 0; i < LOSCFG_KERNEL_CORE_NUM; i++) {
        g_mainTask[i].taskStatus = OS_TASK_STATUS_UNUSED;
        g_mainTask[i].taskId = LOSCFG_BASE_CORE_TSK_LIMIT;
        g_mainTask[i].priority = LOS_TASK_PRIORITY_LOWEST + 1;
        g_mainTask[i].taskName = "osMain";
#ifdef LOSCFG_KERNEL_HPL_TASK_LOCK_FREE
        g_mainTask[i].hpl = NULL;
#endif
#ifdef LOSCFG_KERNEL_LOCKDEP
    UINT32 j;
    for (j = 0; j < LOCK_TYPE_MAX; j++) {
        g_mainTask[i].lockDep[j].waitLock = NULL;
        g_mainTask[i].lockDep[j].lockDepth = 0;
    }
#endif
    }
}

LITE_OS_SEC_TEXT_INIT_LEVEL_ONE STATIC UINT32 OsRegister(VOID)
{
    UINT32 i;

    OsRegisterPrintOps();
    OsRegisterBackTraceOps();
    OsRegisterCpupOps();
    OsRegisterStackCheckOps();
    OsRegisterConsoleIDSetOps();
#ifdef LOSCFG_KENREL_BULITIN_ROM
    OsRegisterExcOps();
    OsRegisterTick();
#endif

#ifdef LOSCFG_LIB_CONFIGURABLE
    g_osSysClock            = OS_SYS_CLOCK_CONFIG;
    g_taskLimit             = LOSCFG_BASE_CORE_TSK_LIMIT;
    g_taskMinStkSize        = LOSCFG_BASE_CORE_TSK_MIN_STACK_SIZE;
    g_taskIdleStkSize       = LOSCFG_BASE_CORE_TSK_IDLE_STACK_SIZE;
    g_taskDfltStkSize       = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    g_taskSwtmrStkSize      = LOSCFG_BASE_CORE_TSK_SWTMR_STACK_SIZE;
#ifdef LOSCFG_HWI_BOTTOM_HALF
    g_hwiBottomHalfWorkLimit = LOSCFG_HWI_BOTTOM_HALF_WORK_LIMIT;
#endif
#ifdef LOSCFG_BASE_CORE_SWTMR
    g_swtmrLimit            = LOSCFG_BASE_CORE_SWTMR_LIMIT;
#endif
#ifdef LOSCFG_BASE_IPC_SEM
    g_semLimit              = LOSCFG_BASE_IPC_SEM_LIMIT;
#endif
#ifdef LOSCFG_BASE_IPC_RWSEM
    g_rwsemLimit            = LOSCFG_BASE_IPC_RWSEM_LIMIT;
#endif
#ifdef LOSCFG_BASE_IPC_MUX
    g_muxLimit              = LOSCFG_BASE_IPC_MUX_LIMIT;
#endif
#ifdef LOSCFG_BASE_IPC_QUEUE
    g_queueLimit            = LOSCFG_BASE_IPC_QUEUE_LIMIT;
#endif
#ifdef LOSCFG_BASE_CORE_TIMESLICE
    g_timeSliceTimeOut      = LOSCFG_BASE_CORE_TIMESLICE_TIMEOUT;
#endif
#endif

    g_osTaskCBArray = g_taskCBArray;

#ifdef LOSCFG_BASE_IPC_QUEUE
    g_osAllQueue = g_allQueue;
#endif

#ifdef LOSCFG_BASE_IPC_MUX
    g_osAllMux = g_allMux;
#endif

#ifdef LOSCFG_BASE_IPC_SEM
    g_osAllSem = g_allSem;
#endif

#ifdef LOSCFG_BASE_IPC_RWSEM
    g_osAllRwsem = g_allRwsem;
#endif

#ifdef LOSCFG_MEM_TASK_STAT
    g_osTaskMemstats = g_taskMemstats;
#endif

#if (defined(LOSCFG_MEM_DFX_SHOW_CALLER_RA) || defined(LOSCFG_MEM_DFX_DOUBLE_FREE_CHECK))
    g_osCallerRA = g_callerRA;
#endif

#ifdef LOSCFG_BASE_CORE_SWTMR
    g_osSwtmrCBArray = g_swtmrCBArray; /* First address in Timer memory space */
    for (i = 0; i < LOSCFG_KERNEL_CORE_NUM; i++) {
        g_percpu[i].expiryList.buf = g_expirybuf[i];
    }
#endif
    for (i = 0; i < LOSCFG_KERNEL_CORE_NUM; i++) {
#ifdef LOSCFG_BASE_CORE_SWTMR
#ifndef LOSCFG_BASE_CORE_SWTMR_IN_ISR
#ifdef LOSCFG_TASK_STACK_STATIC_ALLOCATION
        g_osSwtmrTaskStack[i] = g_swtmrTaskStack[i];
#endif
#endif
#endif
#ifdef LOSCFG_TASK_STACK_STATIC_ALLOCATION
        g_osIdleTaskStack[i] = g_idleTaskStack[i];
#endif
    }

    g_tickPerSecond = LOSCFG_BASE_CORE_TICK_PER_SECOND;

    SET_SYS_CLOCK(OS_SYS_CLOCK);

#ifdef LOSCFG_KERNEL_NX
    LOS_SET_NX_CFG(true);
#else
    LOS_SET_NX_CFG(false);
#endif
    LOS_SET_DL_NX_HEAP_BASE(LOS_DL_HEAP_BASE);
    LOS_SET_DL_NX_HEAP_SIZE(LOS_DL_HEAP_SIZE);

    return LOS_OK;
}
LOS_SYS_INIT(OsRegister, SYS_INIT_LEVEL_AHEAD, SYS_INIT_SYNC_0);

LITE_OS_SEC_TEXT_INIT_LEVEL_ONE VOID OsStart(VOID)
{
    LosTaskCB *taskCB = NULL;
    UINT32 cpuid = ArchCurrCpuid();

    OsTickStart();

    OsSchedLock();
    taskCB = OsGetTopTask();

#ifdef LOSCFG_KERNEL_SMP
    /*
     * attention: current cpu needs to be set, in case first task deletion
     * may fail because this flag mismatch with the real current cpu.
     */
    taskCB->currCpu = (UINT16)cpuid;
#endif

    PRINTK("cpu %u entering scheduler\n", cpuid);
    OS_SCHEDULER_SET(cpuid);

    taskCB->taskStatus = OS_TASK_STATUS_RUNNING;

#ifdef LOSCFG_KERNEL_CPUP
    /* When the CPUP calculates the irq time separately, the irq time is subtracted from the task running time.
       Therefore, ensure that the start time of the task is earlier than the interrupt time. */
    OsCpupStartToRun(taskCB->taskId);
#endif
#ifdef LOSCFG_SHARED_STACK
    if (taskCB->usrStack == 0 && taskCB->cachePtr == NULL && taskCB->isShared) {
        ContextInitOnly(taskCB);
    }
#endif
#ifdef LOSCFG_HARDWARE_STACK_CHECK_ENABLE
    g_newTask_topOfStack = taskCB->topOfStack;
    g_irq_topOfStack = INTERRUPT_STACK_BOTTOM - INTERRUPT_STACK_SIZE;
#endif
    ArchStartToRun(taskCB);
}

#ifdef LOSCFG_HWI_STACK_CHECK
LITE_OS_SEC_TEXT_INIT_LEVEL_ONE STATIC UINT32 OsHwiStackCheckInit(VOID)
{
    OsHwiStackProtectInit();
    return 0;
}
LOS_SYS_INIT(OsHwiStackCheckInit, SYS_INIT_LEVEL_KERNEL, SYS_INIT_SYNC_1);
#endif

LITE_OS_SEC_TEXT_INIT_LEVEL_ONE STATIC UINT32 OsIpcInit(VOID)
{
    UINT32 ret = LOS_OK;
#ifdef LOSCFG_BASE_IPC_SEM
    ret = OsSemInit();
    if (ret != LOS_OK) {
        return ret;
    }
#endif

#ifdef LOSCFG_BASE_IPC_RWSEM
    ret = OsRwsemInit();
    if (ret != LOS_OK) {
        return ret;
    }
#endif

#ifdef LOSCFG_BASE_IPC_MUX
    ret = OsMuxInit();
    if (ret != LOS_OK) {
        return ret;
    }
#endif

#ifdef LOSCFG_BASE_IPC_QUEUE
    ret = OsQueueInit();
    if (ret != LOS_OK) {
        return ret;
    }
#endif
    return ret;
}
LOS_SYS_INIT(OsIpcInit, SYS_INIT_LEVEL_KERNEL, SYS_INIT_SYNC_1);

#ifdef LOSCFG_PLATFORM_OSAPPINIT
#ifdef LOSCFG_TASK_STACK_STATIC_ALLOCATION
STATIC LITE_OS_SEC_BSS UINT8 g_defaultTaskStack[LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE] LITE_OS_ATTR_ALIGN(LOSCFG_STACK_POINT_ALIGN_SIZE);
#endif
LITE_OS_SEC_TEXT_INIT_LEVEL_ONE STATIC UINT32 OsAppTaskCreate(VOID)
{
    UINT32 ret = LOS_OK;
    UINT32 taskId;
    TSK_INIT_PARAM_S appTask = {0};

    appTask.pfnTaskEntry = (TSK_ENTRY_FUNC)app_init;
    appTask.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    appTask.pcName = "app_Task";
    appTask.usTaskPrio = LOSCFG_BASE_CORE_TSK_DEFAULT_PRIO;
    appTask.uwResved = LOS_TASK_STATUS_DETACHED;
#ifdef LOSCFG_KERNEL_SMP
    appTask.usCpuAffiMask = CPUID_TO_AFFI_MASK(ArchCurrCpuid());
#endif
#ifdef LOSCFG_TASK_STACK_STATIC_ALLOCATION
    ret = LOS_TaskCreateStatic(&taskId, &appTask, g_defaultTaskStack);
#else
	ret = LOS_TaskCreate(&taskId, &appTask);
#endif
    if (ret != LOS_OK) {
        PRINT_ERR("OsAppTaskCreate err.\n");
        return ret;
    }

    return LOS_OK;
}
LOS_SYS_INIT(OsAppTaskCreate, SYS_INIT_LEVEL_APP, SYS_INIT_SYNC_0);
#endif /* LOSCFG_PLATFORM_OSAPPINIT */

#ifdef LOSCFG_SHELL_LK
LITE_OS_SEC_TEXT_INIT_LEVEL_ONE STATIC UINT32 DoOsLkLoggerInit(VOID)
{
    OsLkLoggerInit(NULL);
    return LOS_OK;
}
LOS_SYS_INIT(DoOsLkLoggerInit, SYS_INIT_LEVEL_COMPONENT, SYS_INIT_SYNC_0);
#endif

#ifdef LOSCFG_SHELL_DMESG
LOS_SYS_INIT(OsDmesgInit, SYS_INIT_LEVEL_COMPONENT, SYS_INIT_SYNC_0);
#endif

#ifdef LOSCFG_KERNEL_MEM_ALLOC
LITE_OS_SEC_TEXT_INIT_LEVEL_ONE STATIC UINT32 DoOsMemSystemInit(VOID)
{
    UINT32 ret = LOS_OK;
    ret = OsMemSystemInit((UINTPTR)OS_SYS_MEM_ADDR);
    if (ret != LOS_OK) {
        PRINT_ERR("Sys Mem init err.");
        return ret;
    }
#ifdef LOSCFG_LIB_LIBC_MEM_POOL
    ret = LOS_MemInit((VOID *)OS_SYS_LIBC_MEM_ADDR, OS_SYS_LIBC_MEM_POOL_SIZE);
    if (ret != LOS_OK) {
        PRINT_ERR("Libc Mem init err.n");
        return ret;
    }
#endif
    return ret;
}
LOS_SYS_INIT(DoOsMemSystemInit, SYS_INIT_LEVEL_KERNEL, SYS_INIT_SYNC_0);
#endif

#ifdef LOSCFG_EXC_INTERACTION
STATIC UINT32 OsMemExcInteractionDoInit(VOID)
{
    return OsMemExcInteractionInit((UINTPTR)&__exc_heap_start);
}
LOS_SYS_INIT(OsMemExcInteractionDoInit, SYS_INIT_LEVEL_AHEAD, SYS_INIT_SYNC_0);
#endif

/*
 * System init as follow level:
 *   level0: SYS_INIT_LEVEL_EARLY  NONE
 *
 *   level1: SYS_INIT_LEVEL_AHEAD
 *      sync0: OsMemExcInteractionDoInit, OsLmsInit, OsRegister
 *
 *   level2: SYS_INIT_LEVEL_ARCH:
 *      sync0: ArchExcInit
 *
 *   level3: SYS_INIT_LEVEL_KERNEL:
 *      sync0: OsTaskInit, OsTaskMonInit, DoOsMemSystemInit
 *      sync1: OsHwiInit, OsIpcInit
 *      sync2: OsCpupInit, OsTickInit
 *      sync3: OsHwiBottomHalfInit, OsSwtmrInit, OsIdleTaskCreate, OsIdleTaskCreate
 *      sync4: OsMpInit
 *
 *   level4: SYS_INIT_LEVEL_KERNEL_ADDITION:
 *      sync0: OsDynloadInit, OsWowWriteFlashTaskCreate, OsTraceInit
 *
 *   level5: SYS_INIT_LEVEL_COMPONENT:
 *      sync0: HrtimersInit, OsWorkqueueInit, OsDriverBaseInit, OsVfsInit, DoOsLkLoggerInit, OsDmesgInit
 *      sync1: DoInitcalls
 *
 *   level6: SYS_INIT_LEVEL_APP:
 *      sync0: OsTestInit, OsAppTaskCreate
 *
 *   level7: SYS_INIT_LEVEL_RESERVE  NONE
 */
LITE_OS_SEC_TEXT_INIT UINT32 OsMain(VOID)
{
    UINT32 ret;
#ifndef LOSCFG_COMPILER_ICCARM
    SysInitcallFunc *func = NULL;

    for (func = __sysinitcall0_start; func < __sysinitcall_end; func++) {
        ret = (UINT32)(*func)();
        if (ret != LOS_OK) {
            goto OUT;
        }
    }
#else
    DO_SYSINIT_CALL_ALL();
#endif

    return LOS_OK;

OUT:
    PRINT_ERR("OsMain init err %u.\n", ret);
    return ret;
}
