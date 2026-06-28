/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: main process
 * Author: Huawei LiteOS Team
 * Create: 2026-04-10
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

#include "los_hwi.h"
#include "los_tick_pri.h"
#include "tick_timer.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#ifdef LOSCFG_KERNEL_LOWPOWER
STATIC LOWPOWERTIMEHOOK g_lowPowerTimerHook = NULL;
#endif

#define MACHINE_INT_NUM NUM_HAL_INTERRUPT_TIMER

#define MACHINE_TIMER_LOAD_COUNT 0x00
#define MACHINE_TIMER_LOAD_COUNT_L 0x00
#define MACHINE_TIMER_LOAD_COUNT_H 0x04
#define MACHINE_TIMER_CURRENT_VALUE_L 0x08
#define MACHINE_TIMER_CURRENT_VALUE_H 0x0C
#define MACHINE_TIMER_CONTROL_REG 0x10
#define MACHINE_TIMER_EOI 0x14
#define MACHINE_TIMER_INT_STATUS 0x18

#define MACHINE_TIMER_ENABLE_USRMODE 0x03
#define MACHINE_TIMER_RST_TIMER 0x10

#define OS_CYCLE_PER_TICK_PRI ((OS_SYS_CLOCK) / (LOSCFG_BASE_CORE_TICK_PER_SECOND))

#define MACHINE_TIMER_CNT_LOCK_REQ_BIT 5
#define MACHINE_TIMER_CNT_LOCKED_BIT 6
#define MACHINE_TIMER_CNT_LOCK_TIMEOUT 0xFFFF
#define nop() __asm__ __volatile__("nop")
#define writel(addr, d) (*(volatile uint32_t *)(uintptr_t)(addr) = (uint32_t)(d))
#define readl(addr) (*(volatile uint32_t *)(uintptr_t)(addr))

static uint32_t g_machine_timer_addr = 0;

void machine_timer_reister_addr(uintptr_t addr)
{
    g_machine_timer_addr = addr;
}

static inline void machine_timer_init(void)
{
    // before config timer, disable it
    writel((g_machine_timer_addr + MACHINE_TIMER_CONTROL_REG), MACHINE_TIMER_RST_TIMER);

    writel((g_machine_timer_addr + MACHINE_TIMER_LOAD_COUNT), OS_CYCLE_PER_TICK_PRI - 1);
    // user define mode and enable it
    writel((g_machine_timer_addr + MACHINE_TIMER_CONTROL_REG), MACHINE_TIMER_ENABLE_USRMODE);
}

void os_tick_timer_disable(void)
{
    writel((g_machine_timer_addr + MACHINE_TIMER_CONTROL_REG), MACHINE_TIMER_RST_TIMER);
}

void os_tick_timer_enable(void)
{
    writel((g_machine_timer_addr + MACHINE_TIMER_LOAD_COUNT), OS_CYCLE_PER_TICK_PRI - 1);
    // user define mode and enable it
    writel((g_machine_timer_addr + MACHINE_TIMER_CONTROL_REG), MACHINE_TIMER_ENABLE_USRMODE);
}

static inline uint32_t machine_timer_get_cur_count(void)
{
    /* 32bit not need lock timer current count */

    return readl(g_machine_timer_addr + MACHINE_TIMER_CURRENT_VALUE_L);
}

static inline uint32_t machine_timer_get_load_count(void)
{
    return readl(g_machine_timer_addr + MACHINE_TIMER_LOAD_COUNT);
}
#ifdef LOSCFG_KERNEL_LOWPOWER
void lowpower_timer_hookreg(LOWPOWERTIMEHOOK hook)
{
    g_lowPowerTimerHook = hook;
}
#endif
void HalTickEntry(void)
{
    // clear the timer interrupt
    writel(g_machine_timer_addr + MACHINE_TIMER_EOI, 1);
    LOS_HwiClear(MACHINE_INT_NUM);
#ifdef LOSCFG_KERNEL_LOWPOWER
    LOWPOWERTIMEHOOK lowPowerTimerHook;
    lowPowerTimerHook = g_lowPowerTimerHook;
    if (lowPowerTimerHook != NULL) {
        lowPowerTimerHook();
    }
#endif
    OsTickHandler();
}

LITE_OS_SEC_TEXT_INIT void HalClockStart(void)
{
    uint32_t lock_int_save;
    HWI_IRQ_PARAM_S par;

    if ((OS_SYS_CLOCK == 0) || (LOSCFG_BASE_CORE_TICK_PER_SECOND == 0) ||
        (OS_SYS_CLOCK < LOSCFG_BASE_CORE_TICK_PER_SECOND)) {
        return;
    }

    lock_int_save = LOS_IntLock();

    par.swIrq = (int)MACHINE_INT_NUM;
    par.pDevId = NULL;
    par.pName = NULL;
    (void)LOS_HwiCreate(MACHINE_INT_NUM, LOSCFG_HWI_PRIO_LIMIT - 1, 0, HalTickEntry, &par);
    (void)LOS_HwiEnable(MACHINE_INT_NUM);

    machine_timer_init();

    LOS_IntRestore(lock_int_save);
}

void HalClockInit(void)
{
    SET_SYS_CLOCK(OS_SYS_CLOCK);
}

void HalDelayUs(uint32_t usecs)
{
    uint64_t tmo = LOS_CurrNanosec() + (uint64_t)usecs * OS_SYS_NS_PER_US;

    while (LOS_CurrNanosec() < tmo) {
        nop();
    }
}

uint64_t HalClockGetCycles(void)
{
    uint64_t tick_count;
    uint64_t cycle;
    uint32_t cycle_in_tick;
    uint32_t lock_int_save;
    uint32_t load_tick;
    uint32_t cur_tick;

    lock_int_save = LOS_IntLock();
    tick_count = LOS_TickCountGet();
    load_tick = machine_timer_get_load_count();
    cur_tick = machine_timer_get_cur_count();
    uint8_t isPending = 0;
    if ((HalIrqPendingGet(MACHINE_INT_NUM, &isPending) == LOS_OK) && (isPending != 0)) {
        cur_tick = machine_timer_get_cur_count();
        tick_count++;
    }
    cycle_in_tick = load_tick - cur_tick;
    cycle = tick_count * load_tick + cycle_in_tick;
    LOS_IntRestore(lock_int_save);
    return cycle;
}

void pm_os_timer_start(void)
{
    // before config timer, disable it
    writel((g_machine_timer_addr + MACHINE_TIMER_CONTROL_REG), MACHINE_TIMER_RST_TIMER);

    writel((g_machine_timer_addr + MACHINE_TIMER_LOAD_COUNT), OS_CYCLE_PER_TICK_PRI - 1);
    // user define mode and enable it
    writel((g_machine_timer_addr + MACHINE_TIMER_CONTROL_REG), MACHINE_TIMER_ENABLE_USRMODE);
}

void pm_os_timer_stop(void)
{
    // before config timer, disable it
    writel(g_machine_timer_addr + MACHINE_TIMER_CONTROL_REG, MACHINE_TIMER_RST_TIMER);
    writel((g_machine_timer_addr + MACHINE_TIMER_EOI), 1);
    LOS_HwiClear(MACHINE_INT_NUM);
    return;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
