/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: DeepSleep
 * Author: Huawei LiteOS Team
 * Create: 2023-10-17
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
#include "los_sortlink_pri.h"
#include "los_tick_pri.h"
#include "los_tickless_pri.h"
#include "los_atomic.h"
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

#ifdef LOSCFG_KERNEL_SMP
STATIC INLINE VOID OsTickResume(UINT32 sleepTicks)
{
    UINT32 cpuid = ArchCurrCpuid();
    if (sleepTicks > g_pmMgr.sleepTime[cpuid]) {
        sleepTicks -= g_pmMgr.sleepTime[cpuid];
    } else {
        sleepTicks = 0;
    }
    OsSysTimeUpdate(sleepTicks);
}
#endif

#ifdef LOSCFG_KERNEL_WAKEUPTIMER_CALIBRATION
UINT32 g_remainingCycles = 0;
STATIC VOID OsCalibrateTick(UINT32 sleepCycles)
{
    UINT32 reloadCycles;
    UINT64 sleptTickCycles;
    UINT32 sleptTicks;
    UINT32 cyclesPertick = GET_SYS_CLOCK() / LOSCFG_BASE_CORE_TICK_PER_SECOND;

    /* g_remainingCycles < cyclesPertick */
    sleptTickCycles = GET_SYS_CLOCK() * (UINT64)sleepCycles / g_pmMgr.extTimerFreq +
                      (cyclesPertick - g_remainingCycles);
    sleptTicks = (UINT32)(sleptTickCycles / cyclesPertick);
    reloadCycles = (UINT32)(sleptTickCycles - (UINT64)sleptTicks * cyclesPertick);
    HalClockTickTimerReload(cyclesPertick - reloadCycles);
    if (sleptTicks >= g_pmMgr.sleepTime[0]) {
        OsSysTimeUpdate(g_pmMgr.sleepTime[0]);
        OsTickHandler();
    } else {
        OsSysTimeUpdate(sleptTicks + 1);
    }
#ifdef LOSCFG_KERNEL_TICKLESS
    OsTicklessSleepTickSet(0);
#endif
#ifdef LOSCFG_LOWPOWER_SLEEP_DEBUG
    UINT32 cpuId = ArchCurrCpuid();
    if (!g_pmMgrInfo[cpuId].sleepFlag) {
        return;
    }
    PowerMgrInfoItem powerMgrInfo;
    powerMgrInfo.endTicks = LOS_TickCountGet();
    powerMgrInfo.startTicks = g_pmMgrInfo[cpuId].startTicks;
    powerMgrInfo.intNum = OS_NULL_INT;
    powerMgrInfo.sleepMode = g_pmMgrInfo[cpuId].sleepMode;
    OsPowerMgrInfoPut(&powerMgrInfo, cpuId);
    g_pmMgrInfo[cpuId].sleepFlag = FALSE;
#endif
}

STATIC UINT32 OsCalExtSleepCycles(VOID)
{
    UINT64 intSleepCycles;
    UINT32 extSleepCycles;
    UINT32 cyclesPertick = GET_SYS_CLOCK() / LOSCFG_BASE_CORE_TICK_PER_SECOND;

    /* while it is deep, g_pmMgr.sleepTime[0] > 1 */
    intSleepCycles = (UINT64)cyclesPertick * (g_pmMgr.sleepTime[0] - 1);

    if (intSleepCycles < ((UINT64)g_pmMgr.deltaTicks * cyclesPertick)) {
        LOS_PANIC("[PM] delta ticks must be smaller than min deep time!\n");
    }
    intSleepCycles = intSleepCycles - (UINT64)g_pmMgr.deltaTicks * cyclesPertick;
    extSleepCycles = (UINT32)(intSleepCycles * g_pmMgr.extTimerFreq / GET_SYS_CLOCK());

    return extSleepCycles; /* rtc cycles */
}
#endif

#ifdef LOSCFG_DATA_SECTION_ALWAYSON
__attribute__((section(".alwayson.text"))) VOID OsDeepSleepResume(VOID)
#else
STATIC VOID OsDeepSleepResume(VOID)
#endif
{
    RUNOPS_CALL_FUNC_VOID(systemWakeup);
#ifdef LOSCFG_KERNEL_SMP
    LOS_AtomicSet(&g_pmMgr.resumeSleepCores, OS_MP_CPU_ALL);
#endif

#ifdef LOSCFG_KERNEL_CPUP
    OsCpupSetCycle(0);
#endif

#ifdef LOSCFG_KERNEL_SMP
    release_secondary_cores();
#endif
    OsSRRestoreRegister();
}

#if defined(LOSCFG_KERNEL_SMP) && defined(LOSCFG_KERNEL_TICKLESS)
STATIC VOID OsDeepSleepTickAdjust(VOID)
{
    UINT32 sleepTicks, ret;

    sleepTicks = LOS_SleepTicksGet();
    if (sleepTicks > g_pmMgr.maxSleepTicks) {
        sleepTicks = g_pmMgr.maxSleepTicks;
    }
    ret = HalTickAdjust(sleepTicks);
    PANIC_TICK_ADJUST_RET(ret, LOS_OK);
    OsTicklessSleepTickSet(sleepTicks);
}
#endif

STATIC INLINE VOID OsEnterDeepSleepMainCore(VOID)
{
#ifdef LOSCFG_KERNEL_SMP
    (VOID)LOS_AtomicAdd(&g_pmMgr.deepSleepCores, 1);
    if (g_pmMgr.deepSleepCores == LOSCFG_KERNEL_CORE_NUM) {
#endif
        RUNOPS_CALL_FUNC_VOID(suspendConfig);
        g_resumeFromImg = LOS_COLD_RESET;
        OsSRSaveRegister();

        if (g_resumeFromImg == LOS_COLD_RESET) {
            g_resumeFromImg = LOS_DEEP_SLEEP_RESET;
            ISB();
#ifdef LOSCFG_KERNEL_WAKEUPTIMER_CALIBRATION
            g_remainingCycles = HalGetRemainingCycles(&g_pmMgr.sleepTime[0]);
            RUNOPS_CALL_FUNC_VOID(setWakeUpTimer, OsCalExtSleepCycles());
#endif
#ifdef LOSCFG_KERNEL_RAM_SAVE
            RUNOPS_CALL_FUNC_VOID(contextSave);
#endif
            RUNOPS_CALL_FUNC_VOID(enterDeepSleep);
        }
        RUNOPS_CALL_FUNC_VOID(resumeConfig);
        OsTickStart();
#ifdef LOSCFG_KERNEL_WAKEUPTIMER_CALIBRATION
        UINT32 sleepCycles = 0;
        RUNOPS_CALL_FUNC_RET(withdrawWakeUpTimer, sleepCycles);
        OsCalibrateTick(sleepCycles);
#endif
#ifdef LOSCFG_KERNEL_SMP
    } else {
#ifdef LOSCFG_KERNEL_TICKLESS
        OsDeepSleepTickAdjust();
#endif
#ifdef LOSCFG_KERNEL_LIGHTSLEEP
        RUNOPS_CALL_FUNC_VOID(enterLightSleep);
#else
        OsEnterWFI();
#endif
    }
    (VOID)LOS_AtomicSub(&g_pmMgr.deepSleepCores, 1);
#endif
}

STATIC INLINE VOID OsEnterSleepMode(VOID)
{
#ifdef LOSCFG_KERNEL_SMP
    UINT32 currCpuid = ArchCurrCpuid();
    if (currCpuid == 0) {
#endif
        OsEnterDeepSleepMainCore();
#ifdef LOSCFG_KERNEL_SMP
        return;
    }

    UINT32 cpuMask = 1 << currCpuid;
    (VOID)LOS_AtomicAdd(&g_pmMgr.deepSleepCores, 1);
    OsSRSaveRegister();
    if (g_pmMgr.resumeSleepCores & cpuMask) {
        INT32 val;
        do {
            val = LOS_AtomicRead(&g_pmMgr.resumeSleepCores);
        } while (LOS_AtomicCmpXchg32bits(&g_pmMgr.resumeSleepCores, val & (~cpuMask), val));
        RUNOPS_CALL_FUNC_VOID(otherCoreResume);
        UINT32 sleepTicks = 0;
        RUNOPS_CALL_FUNC_RET(withdrawWakeUpTimer, sleepTicks);
        OsTickResume(sleepTicks);
    } else {
        if (g_pmMgr.deepSleepCores == LOSCFG_KERNEL_CORE_NUM) {
            LOS_MpSchedule(1 << 0);
        }
#ifdef LOSCFG_KERNEL_TICKLESS
        OsDeepSleepTickAdjust();
#endif
#ifdef LOSCFG_KERNEL_LIGHTSLEEP
        g_runOps.enterLightSleep();
#else
        OsEnterWFI();
#endif
    }
    (VOID)LOS_AtomicSub(&g_pmMgr.deepSleepCores, 1);
#endif
}

VOID OsLowpowerDeepSleep(LosSleepMode mode, UINT32 cpuid, UINT32 sleepTicks)
{
#ifdef LOSCFG_LOWPOWER_SLEEP_DEBUG
    g_pmMgrInfo[cpuid].sleepFlag = TRUE;
    g_pmMgrInfo[cpuid].sleepMode = mode;
    g_pmMgrInfo[cpuid].startTicks = LOS_TickCountGet();
#endif
    g_pmMgr.sleepTime[cpuid] = sleepTicks;
    OsEnterSleepMode();
#ifdef LOSCFG_LOWPOWER_SLEEP_USERCONFIG
    RUNOPS_CALL_FUNC_VOID(userPostConfig);
#endif
}

#ifdef LOSCFG_DATA_SECTION_ALWAYSON
__attribute__((section(".alwayson.text"))) VOID OsLowpowerWakeupFromReset(VOID)
#else
VOID OsLowpowerWakeupFromReset(VOID)
#endif
{
#ifdef LOSCFG_KERNEL_RUNSTOP
    if (g_resumeFromImg == LOS_RUN_STOP_RESET) {
        OsSystemWakeup();
        return;
    }
#endif
#ifdef LOSCFG_KERNEL_DEEPSLEEP
    if (g_resumeFromImg == LOS_DEEP_SLEEP_RESET) {
        OsDeepSleepResume();
    }
#endif
}

VOID OsAssignMembers(const PowerMgrRunOps *runOps)
{
    ASSIGN_MEMBER(&g_runOps, runOps, systemWakeup);
    ASSIGN_MEMBER(&g_runOps, runOps, suspendConfig);
    ASSIGN_MEMBER(&g_runOps, runOps, resumeConfig);
#ifdef LOSCFG_KERNEL_SMP
    ASSIGN_MEMBER(&g_runOps, runOps, otherCoreResume);
#endif
    ASSIGN_MEMBER(&g_runOps, runOps, enterDeepSleep);
#ifdef LOSCFG_KERNEL_WAKEUPTIMER_CALIBRATION
    ASSIGN_MEMBER(&g_runOps, runOps, setWakeUpTimer);
    ASSIGN_MEMBER(&g_runOps, runOps, withdrawWakeUpTimer);
#endif
#ifdef LOSCFG_KERNEL_RAM_SAVE
    ASSIGN_MEMBER(&g_runOps, runOps, contextSave);
    ASSIGN_MEMBER(&g_runOps, runOps, contextRestore);
#endif
}
