/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2022. All rights reserved.
 * Description: Timer implementation
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

#include "hal_timer.h"
#include "los_tick_pri.h"
#include "los_hwi.h"
#include "asm/interrupt_config.h"
#include "asm/register_config.h"
#if defined(LOSCFG_PERF_HW_PMU) && defined(LOSCFG_ARCH_RISCV32)
#include "arch/pmu.h"
#endif

#ifdef LOSCFG_RISCV_HQTIMER_V510
#define MASK_OFFSET                     16
#define TIMER_REG_CFG_VAL(bit)          ((1U << (bit)) | (1U << ((bit) + MASK_OFFSET)))
#define TIMER_CTRL_RESET_VAL            (0xFFFFU << MASK_OFFSET)
#else
#define TIMER_REG_CFG_VAL(bit)          (1U << (bit))
#define TIMER_CTRL_RESET_VAL            0U
#endif

#define TIMER_CLK_CTRL                  0x0
#define CFG_TIMER_CLK_CTRL_BYPASS_BIT   0x0  // select clk 32768Hz
#define CFG_TIMER_CLK_CTRL_BYPASS       TIMER_REG_CFG_VAL(CFG_TIMER_CLK_CTRL_BYPASS_BIT)
#define TIMER_INIT                      0x4
#define TIMER_CONTROL                   0x8
#define TIMER_INT_STATUS                0xC
#define CFG_TIMER_INT_RAW_PENDING       0x2
#define TIMER_INT_CLR                   0x10
#define TIMER_VALUE                     0x14

#define TIMER_ENABLE_BIT                0
#define TIMER_ENABLE                    TIMER_REG_CFG_VAL(TIMER_ENABLE_BIT)
#define TIMER_SIZE_SELECT_BIT           1
#define TIMER_SIZE_SELECT               TIMER_REG_CFG_VAL(TIMER_SIZE_SELECT_BIT)
#define TIMER_RELOAD_SEL_BIT            3
#define TIMER_RELOAD_SEL                TIMER_REG_CFG_VAL(TIMER_RELOAD_SEL_BIT)

/* User can change timer clk source, defaults use config source */
#ifndef TIMER_TICK_CLK_CTLR_VAL
#define TIMER_TICK_CLK_CTLR_VAL         CFG_TIMER_CLK_CTRL_BYPASS
#endif

/** When use smp mode, the cycle timer always uses core0 timer count just as LOS_TickCountGet() did.
 * TIMER0_REG_BASE: represent core0 timer base
 * TIMER_TICK_REG_BASE: represents per-cpu timer base
 * NUM_HAL_INTERRUPT_TIMER: represents per-cpu interrupt number
 */
#ifdef LOSCFG_KERNEL_SMP
#define CORE0_TIMER_BASE                TIMER0_REG_BASE
#else
#define CORE0_TIMER_BASE                TIMER_TICK_REG_BASE
#endif

#ifdef LOSCFG_KERNEL_LOWPOWER
static UINT32 g_reloadSysTickFlag = 0;
#endif

VOID HalClockFreqWrite(UINT32 freq)
{
    (VOID)freq;
    PRINT_WARN("soc timer does not support setting frequency\n");
}

LITE_OS_SEC_TEXT STATIC VOID HalTickEntry(VOID)
{
    /* int clear */
    WRITE_UINT32(1, TIMER_TICK_REG_BASE + TIMER_INT_CLR);
    dsb(); /* Make sure pending bit of int source clear for riscv SoC */

#ifdef LOSCFG_RISCV_HIMIDEERV200_PLIC
    (VOID)HalIrqClear(NUM_HAL_INTERRUPT_TIMER);
#endif

#if defined(LOSCFG_PERF_HW_PMU) && defined(LOSCFG_ARCH_RISCV32)
    OsPerfCheckEventCount();
#endif

    OsTickHandler();
}

VOID HalClockInit(VOID)
{
    (VOID)LOS_HwiCreate(NUM_HAL_INTERRUPT_TIMER, OS_HWI_PRIO_LOWEST, 0, HalTickEntry, 0);
#ifdef LOSCFG_KERNEL_SMP
    (VOID)LOS_HwiSetAffinity(NUM_HAL_INTERRUPT_TIMER, CPUID_TO_AFFI_MASK(ArchCurrCpuid()));
#endif
}

VOID HalClockEnable(VOID)
{
    UINT32 val;
    val = GET_UINT32(TIMER_TICK_REG_BASE + TIMER_CONTROL);

    /* timer enabled [bit 0 set as 1] */
    val |= TIMER_ENABLE;
    WRITE_UINT32(val, TIMER_TICK_REG_BASE + TIMER_CONTROL);
}

LITE_OS_SEC_TEXT_INIT VOID HalClockStart(VOID)
{
    UINT32 temp, ret;

    /* disable first */
    WRITE_UINT32(TIMER_CTRL_RESET_VAL, TIMER_TICK_REG_BASE + TIMER_CONTROL);

    WRITE_UINT32(TIMER_TICK_CLK_CTLR_VAL, TIMER_TICK_REG_BASE + TIMER_CLK_CTRL);

    /* set init value as period */
    WRITE_UINT32(OS_CYCLE_PER_TICK, TIMER_TICK_REG_BASE + TIMER_INIT);

    /*
     * Timing mode: 32bits [bit 1 set as 1]
     * Timer mode: periodic [bit 2 set as 0]
     * Timer reload sel: reload to timer init val [bit 3 set as 1]
     * Interrupt enabled [bit 4 set as 0]
     */
    temp = TIMER_RELOAD_SEL | TIMER_SIZE_SELECT;
    WRITE_UINT32(temp, TIMER_TICK_REG_BASE + TIMER_CONTROL);

    /* clock start */
    HalClockEnable();
    ret = HalIrqUnmask(NUM_HAL_INTERRUPT_TIMER);
    if (ret != LOS_OK) {
        PRINT_ERR("%s %d ret:%#x\n", __FUNCTION__, __LINE__, ret);
    }
}

UINT64 HalClockGetCycles(VOID)
{
    UINT64 swTick;
    UINT64 cycle;
    UINT32 intSta;
    UINT32 hwCycle;
    UINT32 intSave;

    intSave = LOS_IntLock();
    swTick = LOS_TickCountGet();

    READ_UINT32(hwCycle, CORE0_TIMER_BASE + TIMER_VALUE);

    READ_UINT32(intSta, CORE0_TIMER_BASE + TIMER_INT_STATUS);
    if ((intSta & CFG_TIMER_INT_RAW_PENDING) != 0) { /* check whether the tick timer interruption occurs */
        swTick++;
        READ_UINT32(hwCycle, CORE0_TIMER_BASE + TIMER_VALUE);
    }
    cycle = (swTick * OS_CYCLE_PER_TICK) + (OS_CYCLE_PER_TICK - hwCycle);
    LOS_IntRestore(intSave);

    return cycle;
}

VOID HalDelayUs(UINT32 usecs)
{
    UINT64 cycles = (UINT64)usecs * GET_SYS_CLOCK() / OS_SYS_US_PER_SECOND;
    UINT64 deadline = HalClockGetCycles() + cycles;

    while (HalClockGetCycles() < deadline) {
        __asm__ volatile ("nop");
    }
}

#ifdef LOSCFG_KERNEL_LOWPOWER
UINT32 HalClockGetTickTimerCycles(VOID)
{
    UINT32 temp;

    READ_UINT32(temp, TIMER_TICK_REG_BASE + TIMER_VALUE);
    return temp;
}

VOID HalClockTickTimerReload(UINT32 cycles)
{
    WRITE_UINT32(1, TIMER_TICK_REG_BASE + TIMER_INT_CLR);
    WRITE_UINT32(cycles, TIMER_TICK_REG_BASE + TIMER_INIT);
    g_reloadSysTickFlag = 1;
}

VOID HalTickReload(VOID)
{
    if (g_reloadSysTickFlag == 1) {
        WRITE_UINT32(OS_CYCLE_PER_TICK, TIMER_TICK_REG_BASE + TIMER_INIT);
        g_reloadSysTickFlag = 0;
    }
}

UINT32 HalGetRemainingCycles(UINT32 *ticks)
{
    UINT32 intSta;
    UINT32 remainingCycles;

    READ_UINT32(remainingCycles, TIMER_TICK_REG_BASE + TIMER_VALUE);

    READ_UINT32(intSta, TIMER_TICK_REG_BASE + TIMER_INT_STATUS);
    /* If the tick interrupt has arrived, update tick count and sleep ticks */
    if ((intSta & CFG_TIMER_INT_RAW_PENDING) != 0) {
        if (ticks != NULL) {
            (*ticks)--;
        }
        READ_UINT32(remainingCycles, TIMER_TICK_REG_BASE + TIMER_VALUE);
    }

    return remainingCycles;
}

UINT32 HalTickAdjust(UINT32 ticks)
{
    UINT32 remainingCycles;
    UINT64 sleepCycles;

    if (ticks > TIMER_MAX_TICK) {
        return LOS_NOK;
    }

    remainingCycles = HalGetRemainingCycles(&ticks);
    sleepCycles = (UINT64)(ticks - 1) * OS_CYCLE_PER_TICK;
    HalClockTickTimerReload((UINT32)(sleepCycles + remainingCycles));

    return LOS_OK;
}
#endif
