/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: Low-power Framework.
 * Author: Huawei LiteOS Team
 * Create: 2020-09-19
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

#include "los_lowpower_pri.h"
#include "los_hwi.h"
#include "los_sortlink_pri.h"
#include "los_swtmr_pri.h"
#include "los_task_pri.h"
#include "los_tick_pri.h"
#include "los_tickless_pri.h"
#include "los_atomic.h"
#include "los_mp_pri.h"
#include "los_exc.h"
#ifdef LOSCFG_KERNEL_RUNSTOP
#include "los_runstop_pri.h"
#endif

#ifdef LOSCFG_KERNEL_CPUP
#include "los_cpup_pri.h"
#endif

#ifdef LOSCFG_LOWPOWER_SLEEP_DEBUG
#include "los_lowpower_debug_pri.h"
#endif

#if defined(LOSCFG_KERNEL_RUNSTOP) || defined(LOSCFG_KERNEL_DEEPSLEEP)
/* Is system is up from the memory image, then this flag should be 1; else 0 */
#ifdef LOSCFG_DATA_SECTION_ALWAYSON
__attribute__((section(".alwayson.data"))) INTPTR g_resumeFromImg = LOS_COLD_RESET;
__attribute__((section(".alwayson.data"))) INTPTR g_otherCoreResume = 0;
#else
__attribute__((section(".data"))) INTPTR g_resumeFromImg = LOS_COLD_RESET;
__attribute__((section(".data"))) INTPTR g_otherCoreResume = 0;
#endif

#ifdef LOSCFG_AARCH64
/*
 * 16: The number of aligned memory,
 */
LITE_OS_SEC_DATA_MINOR __attribute__((aligned(16))) UINT64 g_saveSRContext[OS_TASK_REG_NUM];
LITE_OS_SEC_DATA_MINOR UINT64 g_saveAR[OS_UNI_REG_NUM];
#elif defined LOSCFG_ARCH_LINXCORE_131
LITE_OS_SEC_DATA_MINOR UINT32 g_saveSRContext[OS_TASK_REG_NUM];
#elif defined LOSCFG_ARCH_LINX_M
LITE_OS_SEC_DATA_MINOR UINT32 g_saveCpuReg[OS_CPU_REG_NUM];
#ifdef LOSCFG_APC_ENABLE
LITE_OS_SEC_DATA_MINOR UINT32 g_savePmpReg[OS_PMP_REG_NUM];
#endif
#elif defined(LOSCFG_ARCH_LINGLONG)
LITE_OS_SEC_DATA_MINOR UINT32 g_saveCpuReg[OS_CPU_REG_NUM];
#else
LITE_OS_SEC_DATA_MINOR UINT32 g_saveSRContext[LOSCFG_KERNEL_CORE_NUM][OS_TASK_REG_NUM];
LITE_OS_SEC_DATA_MINOR UINT32 g_saveAR[OS_UNI_REG_NUM];
LITE_OS_SEC_DATA_MINOR LosTaskCB *g_saveTsk[LOSCFG_KERNEL_CORE_NUM];
UINT32 g_regBackupBuf[INTERMIT_REGISTER_NUM];
#endif /* LOSCFG_AARCH64 */
#endif /* LOSCFG_KERNEL_RUNSTOP || LOSCFG_KERNEL_DEEPSLEEP */

__attribute__((section(".data"))) PowerMgr g_pmMgr = {
#ifdef LOSCFG_LOWPOWER_SLEEP_VETO
    .requestSleepMode = {0},
#ifdef LOSCFG_LOWPOWER_SLEEP_DEBUG
    .requestSleepTbl = {{0}},
#endif
#endif
#ifdef LOSCFG_KERNEL_DEEPSLEEP
    .sleepTime = {0},
#endif
#ifdef LOSCFG_KERNEL_SMP
    .deepSleepCores = 0,
    .resumeSleepCores = 0,
#endif
#ifdef LOSCFG_KERNEL_DYN_FREQ
    .freq = 1,
    .freqPending = 0,
#endif
};

#ifdef LOSCFG_KERNEL_DYN_FREQ
enum { LOCK_OFF = 0, LOCK_ON };
#endif

#ifdef LOSCFG_KERNEL_DYN_FREQ
STATIC VOID OsChangeFreq(VOID);
#endif

#ifdef LOSCFG_KERNEL_LIGHTSLEEP
STATIC VOID OsLightSleepDefault(VOID)
{
    OsEnterWFI();
}
#endif

#ifdef LOSCFG_KERNEL_POWERMGR
#ifdef LOSCFG_DATA_SECTION_ALWAYSON
__attribute__((section(".alwayson.data"))) PowerMgrRunOps g_runOps = {
#else
PowerMgrRunOps g_runOps = {
#endif
#ifdef LOSCFG_KERNEL_DYN_FREQ
    .changeFreq = NULL,
#endif
#if defined (LOSCFG_KERNEL_LIGHTSLEEP) || defined (LOSCFG_KERNEL_DEEPSLEEP)
    .getSleepMode = NULL,
#endif
#ifdef LOSCFG_KERNEL_LIGHTSLEEP
    .enterLightSleep = OsLightSleepDefault,
#endif
};
#endif

#ifdef LOSCFG_LOWPOWER_SLEEP_VETO
STATIC LosSleepMode OsSelectVetoSleepMode(LosSleepMode tickSleepMode)
{
    /* if sleep normal mode has been voted, just return */
    if ((g_pmMgr.requestSleepMode[0] != 0) || (tickSleepMode == LOS_SLEEP_NORMAL)) {
        return LOS_SLEEP_NORMAL;
    } else {
        /* else, select least sleep mode: light sleep vetoed, not sleep. */
        for (LosSleepMode mode = LOS_SLEEP_LIGHT; mode <= tickSleepMode; mode++) {
            if (g_pmMgr.requestSleepMode[mode] != 0) {
                return mode - 1;
            }
        }
    }
    return tickSleepMode;
}
#endif

#ifdef LOSCFG_LOWPOWER_SLEEP_USERCONFIG
STATIC LosSleepMode OsSelectSleepModeAdjustment(LosSleepMode tickSleepMode, UINT32 *sleepTicks)
{
    if (tickSleepMode >= LOS_SLEEP_DEEP) {
        return tickSleepMode;
    }
    RUNOPS_CALL_FUNC_VOID(userPostConfig);
#ifdef LOSCFG_LOWPOWER_SLEEP_BY_TICK
    UINT32 newSleepTicks = LOS_SleepTicksGet();
    if (newSleepTicks >= *sleepTicks) {
        return tickSleepMode;
    }
    *sleepTicks = newSleepTicks;
#ifdef LOSCFG_KERNEL_LIGHTSLEEP
    if (newSleepTicks < g_pmMgr.minLightSleepTicks) {
        return LOS_SLEEP_NORMAL;
    }
#endif
#else
    (VOID)sleepTicks;
#endif
    return tickSleepMode;
}
#endif /* LOSCFG_LOWPOWER_SLEEP_USERCONFIG */

STATIC LosSleepMode OsSelectSleepMode(UINT32 *sleepTicks)
{
    LosSleepMode tickSleepMode = LOS_SLEEP_MAX;
    UINT32 ret = LOS_SLEEP_MAX;

#ifdef LOSCFG_LOWPOWER_SLEEP_USERCONFIG
    RUNOPS_CALL_FUNC_VOID(userPreConfig);
#endif

#if defined (LOSCFG_KERNEL_LIGHTSLEEP) || defined (LOSCFG_KERNEL_DEEPSLEEP)
    RUNOPS_CALL_FUNC_RET(getSleepMode, ret);
#endif

#ifdef LOSCFG_LOWPOWER_SLEEP_BY_TICK
    *sleepTicks = LOS_SleepTicksGet();
#if defined (LOSCFG_LOWPOWER_SLEEP_DEBUG) && defined (LOSCFG_KERNEL_DEEPSLEEP)
    if ((ret == LOS_SLEEP_DEEP) && (*sleepTicks < g_pmMgr.minDeepSleepTicks)) {
        OsSwtmrTaskBlockInfoStat();
    }
#endif
    if (*sleepTicks > g_pmMgr.maxSleepTicks) {
        *sleepTicks = g_pmMgr.maxSleepTicks;
    }
    if (*sleepTicks <= 1) {
        return LOS_SLEEP_NORMAL;
    }
#ifdef LOSCFG_KERNEL_DEEPSLEEP
    else if (*sleepTicks >= g_pmMgr.minDeepSleepTicks) {
        tickSleepMode = LOS_SLEEP_DEEP;
    }
#endif
#ifdef LOSCFG_KERNEL_LIGHTSLEEP
    else if (*sleepTicks >= g_pmMgr.minLightSleepTicks) {
        tickSleepMode = LOS_SLEEP_LIGHT;
    }
#endif
    else {
        tickSleepMode = LOS_SLEEP_NORMAL;
    }
#endif

    if (ret < tickSleepMode) {
        tickSleepMode = ret;
    }

#ifdef LOSCFG_LOWPOWER_SLEEP_VETO
#if defined (LOSCFG_LOWPOWER_SLEEP_DEBUG) && defined (LOSCFG_KERNEL_DEEPSLEEP)
    if (ret == LOS_SLEEP_DEEP) {
        OsVetoBlockInfoStat();
    }
#endif
    tickSleepMode = OsSelectVetoSleepMode(tickSleepMode);
#endif

#ifdef LOSCFG_LOWPOWER_SLEEP_USERCONFIG
    tickSleepMode = OsSelectSleepModeAdjustment(tickSleepMode, sleepTicks);
#endif
    /* return lightest sleep mode */
    return tickSleepMode;
}

#ifdef LOSCFG_KERNEL_LIGHTSLEEP
STATIC VOID OsLowpowerLightSleep(LosSleepMode mode, UINT32 cpuid, UINT32 sleepTicks)
{
#ifndef LOSCFG_LOWPOWER_SLEEP_DEBUG
    (VOID)mode;
    (VOID)cpuid;
#endif
#ifdef LOSCFG_KERNEL_TICKLESS
    (VOID)sleepTicks;
#endif

#ifdef LOSCFG_KERNEL_TICKLESS
    if (g_runOps.getSleepMode != NULL) {
        sleepTicks = LOS_SleepTicksGet();
    }
#endif
#ifdef LOSCFG_LOWPOWER_SLEEP_DEBUG
    g_pmMgrInfo[cpuid].sleepFlag = TRUE;
    g_pmMgrInfo[cpuid].sleepMode = mode;
    g_pmMgrInfo[cpuid].startTicks = LOS_TickCountGet();
#endif
#ifdef LOSCFG_KERNEL_TICKLESS
    UINT32 ret = HalTickAdjust(sleepTicks);
    PANIC_TICK_ADJUST_RET(ret, LOS_OK);
    OsTicklessSleepTickSet(sleepTicks);
#endif
    g_runOps.enterLightSleep();
}
#endif

STATIC VOID OsLowpowerSleepNormal(UINT32 cpuid, UINT32 sleepTicks)
{
#ifdef LOSCFG_LOWPOWER_SLEEP_DEBUG
    g_pmMgrInfo[cpuid].sleepFlag = TRUE;
    g_pmMgrInfo[cpuid].sleepMode = LOS_SLEEP_NORMAL;
    g_pmMgrInfo[cpuid].startTicks = LOS_TickCountGet();
#else
    (VOID)cpuid;
#endif
#ifdef LOSCFG_KERNEL_TICKLESS
    if (sleepTicks > 1) {
        UINT32 ret = HalTickAdjust(sleepTicks);
        PANIC_TICK_ADJUST_RET(ret, LOS_OK);
        OsTicklessSleepTickSet(sleepTicks);
    }
#else
    (VOID)sleepTicks;
#endif
    OsEnterWFI();
}

STATIC VOID OsLowpowerProcess(VOID)
{
#ifdef LOSCFG_LOWPOWER_SLEEP_DEBUG
    g_lowpowerEnterFlag[ArchCurrCpuid()] = g_lowpowerFlag;
    if (!g_lowpowerFlag) {
        wfi();
        return;
    }
#endif

#ifdef LOSCFG_KERNEL_RUNSTOP
    if (OsWowSysDoneFlagGet() == OS_STORE_SYSTEM) {
        OsStoreSystemInfoBeforeSuspend();
    }
#endif
    /* Change frequency is pended, need to change the freq here. */
#ifdef LOSCFG_KERNEL_DYN_FREQ
    if (g_runOps.changeFreq != NULL) {
        OsChangeFreq();
    }
#endif

    UINT32 intSave = LOS_IntLock();
    LOS_TaskLock();
    UINT32 cpuid = ArchCurrCpuid();
    UINT32 sleepTicks;
    LosSleepMode mode = OsSelectSleepMode(&sleepTicks);
    switch (mode) {
        case LOS_SLEEP_NORMAL:
            OsLowpowerSleepNormal(cpuid, sleepTicks);
            break;
#ifdef LOSCFG_KERNEL_LIGHTSLEEP
        case LOS_SLEEP_LIGHT:
            OsLowpowerLightSleep(mode, cpuid, sleepTicks);
            break;
#endif
#ifdef LOSCFG_KERNEL_DEEPSLEEP
        case LOS_SLEEP_DEEP:
            OsLowpowerDeepSleep(mode, cpuid, sleepTicks);
            break;
#endif
        default:
            break;
    }
    LOS_TaskUnlock();
    LOS_IntRestore(intSave);
}

#ifdef LOSCFG_KERNEL_RAM_SAVE
VOID OsContextRestore(VOID)
{
    RUNOPS_CALL_FUNC_VOID(contextRestore);
}
#endif

#ifdef LOSCFG_KERNEL_DYN_FREQ
STATIC VOID OsChangeFreq(VOID)
{
    LosFreqMode freq;
    BOOL ret;

    do {
        ret = LOS_AtomicCmpXchg32bits(&g_pmMgr.freeLock, LOCK_ON, LOCK_OFF);
        if (ret) {
            return;
        }
        freq = (UINT32)g_pmMgr.freqPending;
        if (freq != (UINT32)g_pmMgr.freq) {
            UINT32 flag = LOS_OK;
            RUNOPS_CALL_FUNC_RET(changeFreq, flag, freq);
            if (flag == LOS_OK) {
                LOS_AtomicSet(&g_pmMgr.freq, freq);
            } else {
                PRINT_ERR("\r\n [PM] change freq failed.\n");
            }
        }
        LOS_AtomicSet(&g_pmMgr.freeLock, LOCK_OFF);
    } while (FreqHigher(g_pmMgr.freqPending, freq));
}

VOID LOS_PowerMgrChangeFreq(LosFreqMode freq)
{
    if (g_runOps.changeFreq == NULL) {
        PRINT_ERR("\r\n [PM] freq function is null.\n");
        return;
    }

    if (freq >= LOS_SYS_FREQ_MAX) {
        PRINT_ERR("\r\n [PM] freq(%d) is invalid.\n", freq);
        return;
    }

    /* The OsChangeFreq procedure deals with the latest setting only. */
    LOS_AtomicSet(&g_pmMgr.freqPending, (INT32)freq);

    /* We get a high frequency request, then change it */
    if (FreqHigher(g_pmMgr.freqPending, g_pmMgr.freq) && (g_runOps.changeFreq != NULL)) {
        OsChangeFreq();
    }
}
#endif

#ifdef LOSCFG_LOWPOWER_SLEEP_VETO
UINT32 LOS_PowerMgrSleepLock(LosSleepMode mode, UINT32 requestId)
{
    UINT32 intSave;
    if ((mode >= LOS_SLEEP_MAX) || (requestId >= MAX_SLEEP_VETO_SRC)) {
        return OS_INVALID;
    }

    LOS_SpinLockSave(&g_pmMgr.requestSleepLock, &intSave);
#ifdef LOSCFG_LOWPOWER_SLEEP_DEBUG
    if (g_pmMgr.requestSleepTbl[mode][requestId] == UINT32_MAX) {
        LOS_PANIC("The pm lock exceed upper limit!\n");
        return OS_INVALID;
    }
    g_pmMgr.requestSleepTbl[mode][requestId]++;
#endif
    if (g_pmMgr.requestSleepMode[mode] == UINT32_MAX) {
        LOS_PANIC("The pm lock exceed upper limit!\n");
        return OS_INVALID;
    }
    g_pmMgr.requestSleepMode[mode]++;

    LOS_SpinUnlockRestore(&g_pmMgr.requestSleepLock, intSave);
    return LOS_OK;
}

UINT32 LOS_PowerMgrSleepUnLock(LosSleepMode mode, UINT32 requestId)
{
    UINT32 intSave;
    if ((mode >= LOS_SLEEP_MAX) || (requestId >= MAX_SLEEP_VETO_SRC)) {
        return OS_INVALID;
    }

    LOS_SpinLockSave(&g_pmMgr.requestSleepLock, &intSave);
    if (g_pmMgr.requestSleepMode[mode] == 0) {
        LOS_PANIC("The pm unlock exceed lower limit!\n");
        return OS_INVALID;
    }
    g_pmMgr.requestSleepMode[mode]--;
#ifdef LOSCFG_LOWPOWER_SLEEP_DEBUG
    if (g_pmMgr.requestSleepTbl[mode][requestId] == 0) {
        LOS_PANIC("The pm unlock exceed lower limit!\n");
        return OS_INVALID;
    }
    g_pmMgr.requestSleepTbl[mode][requestId]--;
#endif

    LOS_SpinUnlockRestore(&g_pmMgr.requestSleepLock, intSave);
    return LOS_OK;
}
#endif

STATIC VOID OsPowerMgrWakeUpFromInterrupt(UINT32 intNum)
{
#ifdef LOSCFG_LOWPOWER_SLEEP_DEBUG
    if (!g_lowpowerEnterFlag[ArchCurrCpuid()]) {
        return;
    }
#endif
#ifdef LOSCFG_KERNEL_TICKLESS
    OsTicklessUpdate(intNum);
#endif
}

#ifdef LOSCFG_LOWPOWER_SLEEP_DEBUG
STATIC VOID OsLowpowerDebugInit(VOID)
{
    OsPowerMgrInfoInit();
    OsSleepBlockCBInit();
}
#endif

UINT32 LOS_PowerMgrInit(const PowerMgrParameter *para)
{
    if (para == NULL) {
        return LOS_ERRNO_LPM_INIT_NULL;
    }
#ifdef LOSCFG_KERNEL_POWERMGR
    const PowerMgrRunOps *runOps = &para->runOps;
#endif
    g_pmMgr.maxSleepTicks = LOSCFG_MAX_SLEEP_TIME;
#ifdef LOSCFG_KERNEL_DEEPSLEEP
    OsAssignMembers(runOps);
#ifdef LOSCFG_KERNEL_WAKEUPTIMER_CALIBRATION
    g_pmMgr.extTimerFreq = LOSCFG_EXT_TIMER_FREQ;
    g_pmMgr.deltaTicks = LOSCFG_DELTA_TICKS;
#endif
    g_pmMgr.minDeepSleepTicks = LOSCFG_MIN_DEEP_SLEEP_TIME;
#endif
#ifdef LOSCFG_KERNEL_LIGHTSLEEP
    g_pmMgr.minLightSleepTicks = LOSCFG_MIN_LIGHT_SLEEP_TIME;
#endif

#ifdef LOSCFG_LOWPOWER_SLEEP_VETO
    LOS_SpinInit(&g_pmMgr.requestSleepLock);
#endif
#if defined (LOSCFG_KERNEL_SMP) && defined (LOSCFG_KERNEL_DEEPSLEEP)
    LOS_AtomicSet(&g_pmMgr.resumeSleepCores, 0);
#endif
#ifdef LOSCFG_KERNEL_DYN_FREQ
    LOS_AtomicSet(&g_pmMgr.freeLock, LOCK_OFF);
#endif

#ifdef LOSCFG_KERNEL_DYN_FREQ
    ASSIGN_MEMBER(&g_runOps, runOps, changeFreq);
#endif
#ifdef LOSCFG_KERNEL_LIGHTSLEEP
    ASSIGN_MEMBER(&g_runOps, runOps, enterLightSleep);
#endif
#if defined (LOSCFG_KERNEL_LIGHTSLEEP) || defined (LOSCFG_KERNEL_DEEPSLEEP)
    ASSIGN_MEMBER(&g_runOps, runOps, getSleepMode);
#endif
#ifdef LOSCFG_LOWPOWER_SLEEP_USERCONFIG
    ASSIGN_MEMBER(&g_runOps, runOps, userPreConfig);
    ASSIGN_MEMBER(&g_runOps, runOps, userPostConfig);
#endif
#ifdef LOSCFG_LOWPOWER_SLEEP_WFI
    ASSIGN_MEMBER(&g_runOps, runOps, enterWFI);
#endif

    LOS_LowpowerHookReg(OsLowpowerProcess);
    LOS_HwiPreHookReg(OsPowerMgrWakeUpFromInterrupt);

#ifdef LOSCFG_LOWPOWER_SLEEP_DEBUG
    OsLowpowerDebugInit();
#endif
    return LOS_OK;
}

LITE_OS_SEC_TEXT UINT32 LOS_SleepTicksGet(VOID)
{
    UINT32 tskSortLinkTicks, sleepTicks;

    UINT32 intSave = LOS_IntLock();
    LOS_SpinLock(&g_taskSpin);
    tskSortLinkTicks = OsSortLinkGetNextExpireTime(&OsPercpuGet()->taskSortLink);
    LOS_SpinUnlock(&g_taskSpin);

#ifdef LOSCFG_BASE_CORE_SWTMR
    UINT32 swtmrSortLinkTicks;
    LOS_SpinLock(&g_swtmrSpin);
    swtmrSortLinkTicks = OsSortLinkGetNextExpireTime(&OsPercpuGet()->swtmrSortLink);
    LOS_SpinUnlock(&g_swtmrSpin);
    sleepTicks = (tskSortLinkTicks < swtmrSortLinkTicks) ? tskSortLinkTicks : swtmrSortLinkTicks;
#else
    sleepTicks = tskSortLinkTicks;
#endif

    LOS_IntRestore(intSave);
    return sleepTicks;
}

#if defined (LOSCFG_KERNEL_DEEPSLEEP) || defined (LOSCFG_KERNEL_TICKLESS)
LITE_OS_SEC_TEXT VOID OsSysTimeUpdate(UINT32 sleepTicks)
{
    UINT32 intSave;

    if (sleepTicks == 0) {
        return;
    }

    intSave = LOS_IntLock();
    g_tickCount[ArchCurrCpuid()] += (sleepTicks - 1);
    LOS_SpinLock(&g_taskSpin);
    OsSortLinkUpdateExpireTime(sleepTicks, &OsPercpuGet()->taskSortLink);
    LOS_SpinUnlock(&g_taskSpin);

#ifdef LOSCFG_BASE_CORE_SWTMR
    LOS_SpinLock(&g_swtmrSpin);
    OsSortLinkUpdateExpireTime(sleepTicks, &OsPercpuGet()->swtmrSortLink);
    LOS_SpinUnlock(&g_swtmrSpin);
#endif

    LOS_IntRestore(intSave);
}
#endif
