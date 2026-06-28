/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2020. All rights reserved.
 * Description: Tickless
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
#include "los_tickless_pri.h"
#include "los_hwi.h"
#include "los_lowpower_pri.h"

#ifdef LOSCFG_LOWPOWER_SLEEP_DEBUG
#include "los_lowpower_debug_pri.h"
#endif

STATIC volatile UINT32 g_sleepTicks[LOSCFG_KERNEL_CORE_NUM] = {0};

LITE_OS_SEC_TEXT UINT32 OsTicklessSleepTickGet(VOID)
{
    return g_sleepTicks[ArchCurrCpuid()];
}

LITE_OS_SEC_TEXT VOID OsTicklessSleepTickSet(UINT32 sleeptick)
{
    g_sleepTicks[ArchCurrCpuid()] = sleeptick;
}

VOID OsTicklessUpdate(UINT32 irqnum)
{
    UINT32 cycles, ticks;
    UINT32 cyclesPertick;
    UINT32 sleepTicks;
    UINT32 intSave = LOS_IntLock();

    sleepTicks = OsTicklessSleepTickGet();
    if (sleepTicks == 0) {
        LOS_IntRestore(intSave);
        return;
    }

    cyclesPertick = GET_SYS_CLOCK() / LOSCFG_BASE_CORE_TICK_PER_SECOND;
    if (irqnum == OS_TICK_INT_NUM) {
        OsSysTimeUpdate(sleepTicks);
        HalTickReload();
    } else {
        cycles = HalClockGetTickTimerCycles();
        cycles = (sleepTicks * cyclesPertick) - cycles;
        ticks = cycles / cyclesPertick;
        cycles = cycles % cyclesPertick;
        if (ticks < sleepTicks) {
            OsSysTimeUpdate(ticks + 1);
            HalClockTickTimerReload(cyclesPertick - cycles);
        } else {
            /*
             * If ticks is greater or equal to sleepTicks, it means the tick has already
             * arrived, it should compensate with the sleepTicks just as that will be done
             * in tick handler.
             */
            OsSysTimeUpdate(sleepTicks);
        }
    }
#ifdef LOSCFG_LOWPOWER_SLEEP_DEBUG
    UINT32 cpuId = ArchCurrCpuid();
    if (g_pmMgrInfo[cpuId].sleepFlag) {
        PowerMgrInfoItem powerMgrInfo;
        powerMgrInfo.sleepMode = g_pmMgrInfo[cpuId].sleepMode;
        powerMgrInfo.endTicks = LOS_TickCountGet();
        powerMgrInfo.intNum = irqnum;
        powerMgrInfo.startTicks = g_pmMgrInfo[cpuId].startTicks;
        OsPowerMgrInfoPut(&powerMgrInfo, cpuId);
        g_pmMgrInfo[cpuId].sleepFlag = FALSE;
    }
#endif
    OsTicklessSleepTickSet(0);

    LOS_IntRestore(intSave);
}
