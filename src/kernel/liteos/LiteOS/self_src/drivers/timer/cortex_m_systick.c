/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2022. All rights reserved.
 * Description: ARM cortex-m timer
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
/* ----------------------------------------------------------------------------
 * This file has been modified in accordance with Misra-C specifications.
 * ---------------------------------------------------------------------------- */

#include "hal_timer.h"
#include "asm/interrupt_config.h"
#include "hal_hwi.h"
#include "los_hwi.h"
#include "los_tick_pri.h"
#include "los_hwi_pri.h"
#include "nvic.h"

#if defined(LOSCFG_ARCH_CORTEX_M3) || defined(LOSCFG_ARCH_CORTEX_M4) || \
    defined(LOSCFG_ARCH_CORTEX_M7) || defined(LOSCFG_ARCH_CORTEX_M55)
/*
 * SysTick Control and Status Register
 * |31 - 17| 16 | 15 - 3 |    2    |   1    |   0   |
 *    RES    FLAG   RES   CLKSOURCE  TICKINT  ENABLE
 */
#define SYST_CSR                    0xE000E010U
#define SYST_CSR_ENABLE_MASK        1U
#define SYST_CSR_TICKINT_MASK       (1U << 1U)
#define SYST_CSR_CLKSOURCE_MASK     (1U << 2U)

/* SysTick Reload Value Register | 31 - 24 RES | 23 - 0 RELOAD| */
#define SYST_RVR                    0xE000E014U
#define SYST_RVR_MAX                0xFFFFFFU
/* SysTick Current Value Register */
#define SYST_CVR                    0xE000E018U
#endif
#define TICK_INTR_CHECK             0x4000000U

#define M_INT_NUM                   15U /* SysTick_IRQn + OS_SYS_VECTOR_CNT */

#define TIMER_REG32_READ(addr)       (*(volatile UINT32 *)(UINTPTR)(addr))
#define TIMER_REG32_WRITE(addr, val) (*(volatile UINT32 *)(UINTPTR)(addr) = (val))
#define TIMER_REG8_WRITE(addr, val)  (*(volatile UINT8 *)(UINTPTR)(addr) = (val))

#ifdef LOSCFG_KERNEL_LOWPOWER
static UINT32 g_reloadSysTickFlag = 0;
#endif

LITE_OS_SEC_BSS UINT32 g_cyclesPerTick;

STATIC VOID ClockConfig(UINT32 reloadTick)
{
    /*
     * The RELOAD value can be any value in the range 0x00000001-0x00FFFFFF.
     * A start value of 0 is possible, but has no effect because the SysTick
     * exception request and COUNTFLAG are activated when counting from 1 to 0.
     */
    if (reloadTick > SYST_RVR_MAX) {
        PRINT_ERR("invalid reload tick\n");
        return;
    }
    /* set reload reg */
    TIMER_REG32_WRITE(SYST_RVR, reloadTick);
    /* load the systick counter value */
    TIMER_REG32_WRITE(SYST_CVR, 0);
    /* enable systick irq and systick timer and select systick clock source(processor clock). */
    TIMER_REG32_WRITE(SYST_CSR, SYST_CSR_ENABLE_MASK | SYST_CSR_TICKINT_MASK | SYST_CSR_CLKSOURCE_MASK);
}

VOID HalClockInit(VOID)
{
    UINT32 ret = LOS_HwiCreate(M_INT_NUM, 0, 0, OsTickHandler, NULL);
    if (ret != 0) {
        PRINTK("ret of LOS_HwiCreate = %#x\n", ret);
    }
}

VOID HalClockStart(VOID)
{
    if ((KERNEL_TICK_PER_SECOND == 0) || (KERNEL_TICK_PER_SECOND > OS_SYS_CLOCK)) {
        return;
    }

    g_cyclesPerTick = (UINT32)OS_CYCLE_PER_TICK;

    /* reload action on the next clock edge, then counts down on subsequent clocks. */
    ClockConfig(OS_CYCLE_PER_TICK - 1U);

    /* set lowest priority for systick interrupt */
    TIMER_REG8_WRITE(SCB_SHPRx(M_INT_NUM), (UINT8)NVIC_CALC_PRIOVALUE((1U << __NVIC_PRIO_BITS) - 1U));
}

UINT64 HalClockGetCycles(VOID)
{
    UINT64 swTick;
    UINT64 cycle;
    UINT32 hwCycle;
    UINT32 intSave;

    intSave = LOS_IntLock();

    swTick = LOS_TickCountGet();
    hwCycle = TIMER_REG32_READ(SYST_CVR);

    /* tick has come, but may interrupt environment, not counting the Tick interrupt response */
    if ((TIMER_REG32_READ(SCB_ICSR) & TICK_INTR_CHECK) != 0) {
        hwCycle = TIMER_REG32_READ(SYST_CVR);
        swTick++;
    }

    cycle = (swTick * g_cyclesPerTick) + (g_cyclesPerTick - hwCycle);
    LOS_IntRestore(intSave);

    return cycle;
}

VOID HalDelayUs(UINT32 usecs)
{
    UINT64 tmo = LOS_CurrNanosec() + ((UINT64)usecs * OS_SYS_NS_PER_US);

    while (LOS_CurrNanosec() < tmo) {
        NOP();
    }
}

UINT64 hi_sched_clock(VOID)
{
    return LOS_CurrNanosec();
}

#ifdef LOSCFG_KERNEL_LOWPOWER

/* just stub api for tickless */
UINT32 HalClockGetTickTimerCycles(VOID)
{
    return TIMER_REG32_READ(SYST_CVR);
}

/* just stub api for tickless */
VOID HalClockTickTimerReload(UINT32 cycles)
{
    ClockConfig((cycles == 0) ? 0 : (cycles - 1U));
    g_reloadSysTickFlag = 1;
}

VOID HalTickReload(VOID)
{
    if (g_reloadSysTickFlag == 1) {
        ClockConfig(g_cyclesPerTick);
        g_reloadSysTickFlag = 0;
    }
}

UINT32 HalGetRemainingCycles(UINT32 *ticks)
{
    UINT32 hwCycle;

    TIMER_REG32_WRITE(SYST_CSR, 0);

    hwCycle = TIMER_REG32_READ(SYST_CVR);

    /* If the tick interrupt has arrived, update tick count and remaining cycles */
    if ((TIMER_REG32_READ(SCB_ICSR) & TICK_INTR_CHECK) != 0) {
        (*ticks)--;
        hwCycle = TIMER_REG32_READ(SYST_CVR);
    }
    return hwCycle;
}

UINT32 HalTickAdjust(UINT32 ticks)
{
    UINT32 remainingCycles;
    UINT64 sleepCycles;

    if (ticks > TIMER_MAX_TICK) {
        return LOS_NOK;
    }

    remainingCycles = HalGetRemainingCycles(&ticks);
    sleepCycles = (ticks - 1) * g_cyclesPerTick;

    HalClockTickTimerReload((UINT32)(sleepCycles + remainingCycles));
    return LOS_OK;
}
#endif

