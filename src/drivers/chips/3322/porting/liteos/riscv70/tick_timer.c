/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: RISCV70 Machine timer config for LiteOS
 *
 * Create: 2022-5-25
 */
#include "hal_hwi.h"
#include "los_tick_pri.h"
#include "los_sched_pri.h"
#include "chip_io.h"
#include "arch_barrier.h"
#include "platform_core.h"
#include "idle_config.h"
#include "soc_osal.h"
#include "osal_timer.h"
#include "osal_interrupt.h"
#include "debug_print.h"
#include "asm/platform.h"
#include "hal_systick.h"
#include "tcxo.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define MACHINE_INT_NUM                 (TIMER_0_IRQN)
#define MACHINE_TIMER_COUNT             10

#define MACHINE_TIMER_BASE              TICK_TIMER_BASE_ADDR
#define MACHINE_TIMER_LOAD_COUNT        0x00
#define MACHINE_TIMER_LOAD_COUNT_L      0x00
#define MACHINE_TIMER_LOAD_COUNT_H      0x04
#define MACHINE_TIMER_CURRENT_VALUE     0x08
#define MACHINE_TIMER_CONTROL_REG       0x10
#define MACHINE_TIMER_EOI               0x14
#define MACHINE_TIMER_INT_STATUS        0x18
#define MACHINE_TIMER_ENABLE_USRMODE        0x3 // periodic mode
#define MACHINE_TIMER_ENABLE_ONESHOTMODE    0x5 // one shot mode
#define MACHINE_TIMER_CNT_REQ_BIT       5
#define MACHINE_TIMER_CNT_LOCK          6
#define MACHINE_TIMER_LOCK_BASE         TIMER_BASE_ADDR
#define MACHINE_TIMER_UNLOCK_VALUE      0x5a5a5a5a
#define MACHINE_TIMER_LOCK_VALUE        0x22222222

#define SYS_CLICINTIP_DISABLE 0x0
#define MCLICBASE                   0xFBF

#ifndef OS_CYCLE_PER_TICK
#define OS_CYCLE_PER_TICK ((OS_SYS_CLOCK) / (LOSCFG_BASE_CORE_TICK_PER_SECOND))
#endif

#define writel(addr, d)    (*(volatile uint32_t*)(uintptr_t)(addr) = (uint32_t)(d))
#define readl(addr)        (*(volatile uint32_t*)(uintptr_t)(addr))
static int g_timerinited = 0;
static uint64_t g_last_timer_sytick_record = 0;
static uint32_t g_timer_reload_flag = 0;
static uint64_t g_timer_base = 0;
static uint32_t g_timer_err = 0;

uint64_t get_last_timer_systick(void)
{
    return g_last_timer_sytick_record;
}

void clear_last_timer_systick(void)
{
    g_last_timer_sytick_record = 0;
}

uint64_t get_timer_base(void)
{
    return g_timer_base;
}

uint32_t get_timer_err_dfx(void)
{
    return g_timer_err;
}

void clear_timer_err_dfx(void)
{
    g_timer_err = 0;
}

static inline void machine_timer_init(void)
{
    if (g_timerinited == 1) {
        return ;
    }
    writel(MACHINE_TIMER_LOCK_BASE, MACHINE_TIMER_UNLOCK_VALUE);
    // before config timer, disable it
    writel((MACHINE_TIMER_BASE + MACHINE_TIMER_CONTROL_REG), 0);

    writel((MACHINE_TIMER_BASE + MACHINE_TIMER_LOAD_COUNT), OS_CYCLE_PER_TICK);
    // user define mode and enable it
    writel((MACHINE_TIMER_BASE + MACHINE_TIMER_CONTROL_REG), MACHINE_TIMER_ENABLE_USRMODE);

    g_timerinited = 1;
}

static inline uint32_t machine_timer_get_cur_count(void)
{
    uint32_t irq_sts = osal_irq_lock();
    reg_setbit(MACHINE_TIMER_BASE, MACHINE_TIMER_CONTROL_REG, MACHINE_TIMER_CNT_REQ_BIT);
    while (reg_getbits(MACHINE_TIMER_BASE, MACHINE_TIMER_CONTROL_REG, MACHINE_TIMER_CNT_LOCK, 1) != 1) {}
    osal_irq_restore(irq_sts);
    return readl(MACHINE_TIMER_BASE + MACHINE_TIMER_CURRENT_VALUE);
}

void pre_timer_init(void)
{
    machine_timer_init();
}

void HalClockDisable(void)
{
    writel((MACHINE_TIMER_BASE + MACHINE_TIMER_CONTROL_REG), 0);
    int_clear_pending_irq(MACHINE_INT_NUM);
}

void HalClockEnable(void)
{
    writel((MACHINE_TIMER_BASE + MACHINE_TIMER_LOAD_COUNT), OS_CYCLE_PER_TICK);
    // user define mode and enable it
    writel((MACHINE_TIMER_BASE + MACHINE_TIMER_CONTROL_REG), MACHINE_TIMER_ENABLE_USRMODE);
}

void HalClockEnableAndReload(uint32_t value)
{
    writel((MACHINE_TIMER_BASE + MACHINE_TIMER_LOAD_COUNT), value);
    // user define mode and enable it
    writel((MACHINE_TIMER_BASE + MACHINE_TIMER_CONTROL_REG), MACHINE_TIMER_ENABLE_ONESHOTMODE);

    g_timer_reload_flag = 1;
}

static void oal_swtmr_ticks_restore(uint32_t ticks)
{
    uint32_t compensation_ms = ticks;
    uint32_t swtmrTimeout = OsSortLinkGetNextExpireTime(&(OsPercpuGet()->swtmrSortLink));
    while (compensation_ms > 0) {
        if (swtmrTimeout > compensation_ms) {
            g_tickCount[ArchCurrCpuid()] += compensation_ms;
            OsSortLinkUpdateExpireTime(compensation_ms + 1, &(OsPercpuGet()->swtmrSortLink));
            break;
        } else {
            g_tickCount[ArchCurrCpuid()] += swtmrTimeout - 1;
            // 补偿当前timer超时时间
            OsSortLinkUpdateExpireTime(swtmrTimeout, &(OsPercpuGet()->swtmrSortLink));
            // 触发一次tick handler，更新timer链表
            OsTickHandler();
            // 修正补偿时间
            compensation_ms -= swtmrTimeout;
            // 获取下一次timer的超时时间
            swtmrTimeout = OsSortLinkGetNextExpireTime(&(OsPercpuGet()->swtmrSortLink));
            if ((swtmrTimeout == 0) || (swtmrTimeout == 0xFFFFFFFF)) {
                // 当前无剩余timer, 仅更新tick count
                g_tickCount[ArchCurrCpuid()] += compensation_ms;
                break;
            }
        }
    }
}

void HalTickEntry(void)
{
    // clear the timer interrupt
    int_clear_pending_irq(MACHINE_INT_NUM);
    writel(MACHINE_TIMER_BASE + MACHINE_TIMER_EOI, 0x1);

    if (g_timer_reload_flag) {
        // before config timer, disable it
        writel((MACHINE_TIMER_BASE + MACHINE_TIMER_CONTROL_REG), 0);

        writel((MACHINE_TIMER_BASE + MACHINE_TIMER_LOAD_COUNT), OS_CYCLE_PER_TICK);
        // user define mode and enable it
        writel((MACHINE_TIMER_BASE + MACHINE_TIMER_CONTROL_REG), MACHINE_TIMER_ENABLE_USRMODE);
        g_timer_reload_flag = 0;
    }

#if defined(CONFIG_SUPPORT_XO_32K)
    g_last_timer_sytick_record = hal_systick_get_count();

    uint64_t os_tick = g_tickCount[ArchCurrCpuid()] + 1; // current tick
    uint64_t rtc_ms = ((g_last_timer_sytick_record - g_timer_base) * 1000) >> 0xf; // divide 32768
    if (os_tick < rtc_ms) {
        oal_swtmr_ticks_restore((uint32_t)(rtc_ms - os_tick));
    } else if (os_tick > rtc_ms) {
        g_timer_err = (uint32_t)(os_tick - rtc_ms);
    }
#else
    uint64_t rtc_count = hal_systick_get_count();
    if (g_last_timer_sytick_record != 0) {
        uint64_t tick_offset = ((rtc_count - g_last_timer_sytick_record) * 1000) >> 0xF; // div 32768
        if (tick_offset > 1) {
            oal_swtmr_ticks_restore(tick_offset);
        }
    }

    g_last_timer_sytick_record = rtc_count;
#endif

#ifdef LOSCFG_PERF_HW_PMU
    /*
     * The RISCV architecture does not have PMU interrupts.The tick interrupt is used to collect
     * statistics on the number of events. Once the event frequency is higher than the tick interrupt frequency,
     * statistics are lost. Therefore, the PMU statistics frequency of the RISCV architecture
     * cannot be higher than that of the tick interrupt.
     */
    OsPerfCheckEventCount();
#endif
    OsTickHandler();
}

LITE_OS_SEC_TEXT_INIT void HalClockStart(void)
{
    uint32_t lock_int_save;

    if ((OS_SYS_CLOCK == 0) ||
        (LOSCFG_BASE_CORE_TICK_PER_SECOND == 0) ||
        (LOSCFG_BASE_CORE_TICK_PER_SECOND > OS_SYS_CLOCK)) {
        PRINT_ERR("TIMER cfg error.\r\n");
        return;
    }

    lock_int_save = osal_irq_lock();
    osal_irq_request(MACHINE_INT_NUM, (osal_irq_handler)HalTickEntry, NULL, NULL, NULL);
    osal_irq_enable(MACHINE_INT_NUM);
    machine_timer_init();
    g_timer_base = hal_systick_get_count();
    osal_irq_restore(lock_int_save);
    idle_task_config();
}

void HalClockInit(void)
{
    SET_SYS_CLOCK(OS_SYS_CLOCK);
}

void HalDelayUs(unsigned usecs)
{
    uapi_tcxo_delay_us(usecs);
}

uint64_t HalClockGetCycles(void)
{
    uint64_t tick_count;
    uint64_t cycle;
    uint32_t cycle_in_tick;
    uint32_t lock_int_save;
    uint32_t load_tick = OS_CYCLE_PER_TICK;
    uint32_t cur_tick;
    uint8_t isPending = 0;
    if (g_timerinited == 0) {
        machine_timer_init();
    }
    lock_int_save = osal_irq_lock();
    tick_count = osal_get_jiffies();
    cur_tick = machine_timer_get_cur_count();
    if ((HalIrqPendingGet(MACHINE_INT_NUM, &isPending) == OSAL_SUCCESS) && (isPending != 0)) {
        cur_tick = machine_timer_get_cur_count();
        tick_count++;
    }
    cycle_in_tick = load_tick - cur_tick;
    cycle = tick_count * load_tick + cycle_in_tick;
    osal_irq_restore(lock_int_save);
    return cycle;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
