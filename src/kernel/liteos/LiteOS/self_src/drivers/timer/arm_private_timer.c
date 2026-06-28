/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2022. All rights reserved.
 * Description: ARM private timer.
 * Author: Huawei LiteOS Team
 * Create: 2018-06-26
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

#include "hal_timer.h"
#include "asm/interrupt_config.h"
#include "los_hwi.h"
#include "los_tick_pri.h"

typedef struct {
    UINT32 load;        /* Private Timer Load Register */
    UINT32 count;       /* Private Timer Counter Register */
    UINT32 control;     /* Private Timer Control Register */
    UINT32 intStatus;   /* Private Timer Interrupt Status Register */
} PrivateTimer;

typedef struct {
    UINT32 low;         /* Global Timer Counter Registers, low bits */
    UINT32 high;        /* Global Timer Counter Registers, high bits */
    UINT32 control;     /* Global Timer Control Register */
    UINT32 intStatus;   /* Global Timer Interrupt Status Register */
    UINT32 compareLow;  /* Comparator Value Registers, low bits */
    UINT32 compareHigh; /* Comparator Value Registers, high bits */
    UINT32 increment;   /* Auto-increment Register */
} GlobalTimer;

/* Declared as a volatile type to ensure that read and write operations are not optimized by the compiler */
volatile PrivateTimer * const g_privateTimer = (volatile PrivateTimer *)PRVTIMER_BASE_ADDR;
volatile GlobalTimer * const g_globalTimer = (volatile GlobalTimer *)GTIMER_BASE_ADDR;

VOID HalClockFreqWrite(UINT32 freq)
{
    PRINT_WARN("private timer does not support setting frequency\n");
}

VOID HalClockStart(VOID)
{
    (void)HalIrqUnmask(PRVTIMER_INT_NUM);

    g_privateTimer->load = OS_CYCLE_PER_TICK;
    g_privateTimer->control = 0x06; /* IAE bits = 110, not enabled yet */
    g_privateTimer->control |= 0x01; /* enable private timer */

    g_globalTimer->control = 0x06;   /* Prescaler bits = 0, Auto-increment off, Comp enabled */
    g_globalTimer->control |= 0x01;  /* enable timer */
}

STATIC VOID HalTickEntry(VOID)
{
    OsTickHandler();

    /* clear private timer */
    g_privateTimer->intStatus = 0x01;
}

VOID HalClockInit(VOID)
{
    UINT32 ret =  LOS_HwiCreate(PRVTIMER_INT_NUM, OS_HWI_PRIO_LOWEST, 0, HalTickEntry, NULL);
    if (ret != LOS_OK) {
        PRINT_ERR("%s, %d create tick irq failed, ret:0x%x\n", __FUNCTION__, __LINE__, ret);
    }
}

UINT64 HalClockGetCycles(VOID)
{
    UINT32 low, high;

    do {
        high = g_globalTimer->high;
        low = g_globalTimer->low;
    } while (g_globalTimer->high != high);

    /* combine high and low into 8 bytes cycles */
    return (((UINT64)high << 32) | low);
}

VOID HalDelayUs(UINT32 usecs)
{
    UINT64 tmo = LOS_CurrNanosec() + (UINT64)usecs * OS_SYS_NS_PER_US;

    while (LOS_CurrNanosec() < tmo) {
        __asm__ volatile ("nop");
    }
}

UINT64 hi_sched_clock(VOID)
{
    return LOS_CurrNanosec();
}

#ifdef LOSCFG_KERNEL_LOWPOWER
UINT32 HalClockGetTickTimerCycles(VOID)
{
    return g_privateTimer->count;
}

VOID HalClockTickTimerReload(UINT32 cycles)
{
    (void)HalIrqUnmask(PRVTIMER_INT_NUM);

    /* set control counter regs to defaults */
    g_privateTimer->load = cycles;
    g_privateTimer->control = 0x06;  /* IAE bits = 110, not enabled yet */
    g_privateTimer->control |= 0x01; /* re-enable private timer */
}

VOID HalTickReload(VOID)
{
}
#endif
