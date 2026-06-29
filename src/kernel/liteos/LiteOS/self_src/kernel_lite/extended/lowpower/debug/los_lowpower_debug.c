/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Low-power Debug File.
 * Author: Huawei LiteOS Team
 * Create: 2021-11-12
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
#include "los_lowpower_debug_pri.h"
#include "los_typedef.h"
#include "los_printf.h"
#include "los_ringbuf.h"
#include "los_task_pri.h"
#include "los_swtmr_pri.h"
#include "los_sortlink_pri.h"

STATIC const CHAR *g_shellLowpowerMode[LOS_SLEEP_MAX] = {
    "Normal",
    "Light",
    "Deep",
};

UINT32 g_lowpowerFlag = TRUE;
volatile BOOL g_lowpowerEnterFlag[LOSCFG_KERNEL_CORE_NUM] = {FALSE};
VOID LOS_EnableLowPower(VOID)
{
    g_lowpowerFlag = TRUE;
}

VOID LOS_DisableLowPower(VOID)
{
    g_lowpowerFlag = FALSE;
}

UINT32 LOS_GetLowPowerStatus(VOID)
{
    return g_lowpowerFlag;
}

STATIC CHAR g_pmBuff[LOSCFG_KERNEL_CORE_NUM][LOSCFG_LOWPOWER_RECORD_COUNT * sizeof(PowerMgrInfoItem)] = {0};
STATIC CHAR g_pmBuffTakeOut[LOSCFG_LOWPOWER_RECORD_COUNT * sizeof(PowerMgrInfoItem)] = {0};
STATIC Ringbuf g_pmMgrRecord[LOSCFG_KERNEL_CORE_NUM] = {0};
PowerMgrInfo g_pmMgrInfo[LOSCFG_KERNEL_CORE_NUM] = {0};

UINT32 OsPowerMgrInfoInit(VOID)
{
    UINT32 size = LOSCFG_LOWPOWER_RECORD_COUNT * sizeof(PowerMgrInfoItem);
    UINT32 ret;
    for (UINT32 i = 0; i < LOSCFG_KERNEL_CORE_NUM; ++i) {
        ret = LOS_RingbufInit(&g_pmMgrRecord[i], &g_pmBuff[i][0], size, RBUF_OVERWRITE);
        if (ret != LOS_OK) {
            PRINT_ERR("\r\n [PM] PowerMgrInfo init fail.\n");
            return LOS_NOK;
        }
    }

    return ret;
}

VOID OsPowerMgrInfoPut(PowerMgrInfoItem *powerMgrInfo, UINT32 cpuid)
{
    UINT32 ret;
    if (powerMgrInfo->sleepMode == LOS_SLEEP_NORMAL) {
        return;
    }
    ret = LOS_RingbufWrite(&g_pmMgrRecord[cpuid], (const CHAR *)powerMgrInfo, sizeof(PowerMgrInfoItem));
    if (ret != sizeof(PowerMgrInfoItem)) {
        PRINT_ERR("\r\n [PM] PowerMgrInfo ring buf write fail, ret: %u.\n", ret);
        return;
    }
}

VOID OsPowerMgrInfoTitle(VOID)
{
    PRINTK("---------------------------------------------------------------------");
    PRINTK("\n");

    PRINTK("cpuId   sleepMode   startTicks    endTicks   sleepTicks   InterruptNo");
    PRINTK("\n");

    PRINTK("---------------------------------------------------------------------");
    PRINTK("\n");
}

VOID PowerMgrInfoItemPrint(PowerMgrInfoItem *powerMgrInfo, UINT32 cpuId)
{
    PRINTK("%-7u %-7s     %-10llu    %-10llu  %-10llu    %-7u\n",
        cpuId, g_shellLowpowerMode[powerMgrInfo->sleepMode], powerMgrInfo->startTicks, powerMgrInfo->endTicks,
        powerMgrInfo->endTicks - powerMgrInfo->startTicks, powerMgrInfo->intNum);
}

VOID OsPowerMgrInfoDbg(UINT32 cpuId)
{
    Ringbuf *ringbuf = &g_pmMgrRecord[cpuId];
    PowerMgrInfoItem *powerMgrInfo;
    UINT32 actualLen;

    (VOID)memset(g_pmBuffTakeOut, 0, sizeof(g_pmBuffTakeOut));
    actualLen = LOS_RingbufPeek(ringbuf, g_pmBuffTakeOut, LOSCFG_LOWPOWER_RECORD_COUNT * sizeof(PowerMgrInfoItem));
    for (UINT32 i = 0; i < actualLen; i = i + sizeof(PowerMgrInfoItem)) {
        powerMgrInfo = (PowerMgrInfoItem *)(&g_pmBuffTakeOut[i]);
        PowerMgrInfoItemPrint((PowerMgrInfoItem *)powerMgrInfo, cpuId);
    }
}

typedef struct SleepBlockCB {
    SPIN_LOCK_S lock;
    UINT32 taskBlockCount[LOSCFG_BASE_CORE_TSK_LIMIT];
    UINTPTR taskHandler[LOSCFG_BASE_CORE_TSK_LIMIT];
    UINTPTR swtmrHandler[LOSCFG_BASE_CORE_SWTMR_LIMIT];
    UINT32 swtmrBlockCount[LOSCFG_BASE_CORE_SWTMR_LIMIT];
#ifdef LOSCFG_LOWPOWER_SLEEP_VETO
    UINT32 requestBlockCount[LOS_SLEEP_MAX][MAX_SLEEP_VETO_SRC];
#endif
} SleepBlockCB;

typedef enum {
    BLOCK_TASK = 0,
    BLOCK_SWTMR = 1,
} SleepBlockType;

SleepBlockCB g_sleepBlockCB[LOSCFG_KERNEL_CORE_NUM] = {0};

VOID OsSleepBlockCBInit(VOID)
{
    (VOID)memset(&g_sleepBlockCB, 0, sizeof(g_sleepBlockCB));
    for (UINT32 i = 0; i < LOSCFG_KERNEL_CORE_NUM; ++i) {
        LOS_SpinInit(&g_sleepBlockCB[i].lock);
    }
}

STATIC VOID OsAdd2SleepBlockCB(UINT32 cpuId, UINT32 type, UINT32 id, UINTPTR handler)
{
    UINT32 intSave;
    LOS_SpinLockSave(&g_sleepBlockCB[cpuId].lock, &intSave);
    if ((type == BLOCK_TASK) && (g_sleepBlockCB[cpuId].taskBlockCount[id] < UINT32_MAX)) {
        g_sleepBlockCB[cpuId].taskBlockCount[id]++;
        g_sleepBlockCB[cpuId].taskHandler[id] = handler;
    } else if ((type == BLOCK_SWTMR) && (g_sleepBlockCB[cpuId].swtmrBlockCount[id] < UINT32_MAX)) {
        g_sleepBlockCB[cpuId].swtmrBlockCount[id]++;
        g_sleepBlockCB[cpuId].swtmrHandler[id] = handler;
    }
    LOS_SpinUnlockRestore(&g_sleepBlockCB[cpuId].lock, intSave);
    return;
}

VOID OsResetSleepBlockCB(UINT32 cpuId)
{
    UINT32 intSave;

    LOS_SpinLockSave(&g_sleepBlockCB[cpuId].lock, &intSave);
    (VOID)memset(g_sleepBlockCB[cpuId].swtmrBlockCount, 0, sizeof(g_sleepBlockCB[cpuId].swtmrBlockCount));
    (VOID)memset(g_sleepBlockCB[cpuId].taskBlockCount, 0, sizeof(g_sleepBlockCB[cpuId].taskBlockCount));
#ifdef LOSCFG_LOWPOWER_SLEEP_VETO
    (VOID)memset(g_sleepBlockCB[cpuId].requestBlockCount, 0, sizeof(g_sleepBlockCB[cpuId].requestBlockCount));
#endif
    LOS_SpinUnlockRestore(&g_sleepBlockCB[cpuId].lock, intSave);

    return;
}

VOID OsSwtmrTaskBlockInfoStat(VOID)
{
#ifdef LOSCFG_KERNEL_DEEPSLEEP
    UINT32 tskSortLinkTicks;
    UINT32 taskId, swtmrId;
    UINTPTR handler1, handler2;

    UINT32 cpuId = ArchCurrCpuid();
    UINT32 intSave = LOS_IntLock();
    LOS_SpinLock(&g_taskSpin);
    tskSortLinkTicks = OsSortLinkGetNextTaskExpireTime(&taskId, &handler1);
    LOS_SpinUnlock(&g_taskSpin);
    if (tskSortLinkTicks < g_pmMgr.minDeepSleepTicks) {
        OsAdd2SleepBlockCB(cpuId, BLOCK_TASK, taskId, handler1);
    }

#ifdef LOSCFG_BASE_CORE_SWTMR
    UINT32 swtmrSortLinkTicks;
    LOS_SpinLock(&g_swtmrSpin);
    swtmrSortLinkTicks = OsSortLinkGetNextSwtmrExpireTime(&swtmrId, &handler2);
    LOS_SpinUnlock(&g_swtmrSpin);
    if (swtmrSortLinkTicks < g_pmMgr.minDeepSleepTicks) {
        OsAdd2SleepBlockCB(cpuId, BLOCK_SWTMR, swtmrId, handler2);
    }
#endif
    LOS_IntRestore(intSave);
    return;
#endif
}

#ifdef LOSCFG_LOWPOWER_SLEEP_VETO
VOID OsVetoBlockInfoStat(VOID)
{
    UINT32 index;
    UINT32 intSave;
    UINT32 cpuId = ArchCurrCpuid();

    for (LosSleepMode mode = LOS_SLEEP_NORMAL; mode < LOS_SLEEP_MAX; ++mode) {
        for (index = 0; index < MAX_SLEEP_VETO_SRC; ++index) {
            if (g_pmMgr.requestSleepTbl[mode][index] == 0) {
                continue;
            }
            LOS_SpinLockSave(&g_sleepBlockCB[cpuId].lock, &intSave);
            if (g_sleepBlockCB[cpuId].requestBlockCount[mode][index] < UINT32_MAX) {
                g_sleepBlockCB[cpuId].requestBlockCount[mode][index]++;
            }
            LOS_SpinUnlockRestore(&g_sleepBlockCB[cpuId].lock, intSave);
        }
    }
}
#endif

STATIC VOID OsSleepBlockInfoPrintHead(VOID)
{
    PRINTK("deep sleep block info:\n"
        "--------------------------------------------------------\n"
        "cpuId     type      id         blockCount    handler\n"
        "--------------------------------------------------------\n");
}

#ifdef LOSCFG_LOWPOWER_SLEEP_VETO
VOID OsVetoBlockInfoPrintHead(VOID)
{
    PRINTK("user veto block deep sleep info:\n"
        "-----------------------------------\n"
        "sleepMode  requestId    blockCount\n"
        "-----------------------------------\n");
}
#endif

VOID OsSleepBlockInfoPrint(UINT32 cpuId)
{
    UINT32 intSave;
    LOS_SpinLockSave(&g_sleepBlockCB[cpuId].lock, &intSave);
    OsSleepBlockInfoPrintHead();
    for (UINT32 i = 0; i < LOSCFG_BASE_CORE_TSK_LIMIT; i++) {
        if (g_sleepBlockCB[cpuId].taskBlockCount[i] != 0) {
            PRINTK("%-7u   %-7s   %-7u    %-7u       %p\n", cpuId, "task", i,
                g_sleepBlockCB[cpuId].taskBlockCount[i], g_sleepBlockCB[cpuId].taskHandler[i]);
        }
    }
    for (UINT32 i = 0; i < LOSCFG_BASE_CORE_SWTMR_LIMIT; i++) {
        if (g_sleepBlockCB[cpuId].swtmrBlockCount[i] != 0) {
            PRINTK("%-7u   %-7s   %-7u    %-7u       %p\n", cpuId, "swtmr", i,
                g_sleepBlockCB[cpuId].swtmrBlockCount[i], g_sleepBlockCB[cpuId].swtmrHandler[i]);
        }
    }
#ifdef LOSCFG_LOWPOWER_SLEEP_VETO
    OsVetoBlockInfoPrintHead();
    for (LosSleepMode mode = LOS_SLEEP_NORMAL; mode < LOS_SLEEP_MAX; mode++) {
        for (UINT32 i = 0; i < MAX_SLEEP_VETO_SRC; i++) {
            if (g_sleepBlockCB[cpuId].requestBlockCount[mode][i] != 0) {
                PRINTK("%-7s    %-7u      %-7u\n", g_shellLowpowerMode[mode], i,
                    g_sleepBlockCB[cpuId].requestBlockCount[mode][i]);
            }
        }
    }
#endif
    LOS_SpinUnlockRestore(&g_sleepBlockCB[cpuId].lock, intSave);
    return;
}
