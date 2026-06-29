/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: LingLong Timer Driver Implementation. NOTE: this is ONLY used to support
 * testsuites, DO NOT use in other scenario.
 * Author: Huawei LiteOS Team
 * Create: 2021-11-30
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

#include "arch/cpu.h"
#include "arch/regs.h"
#include "hal_timer.h"
#include "los_tick_pri.h"

/******************** SCU TIMER ********************/
#define SCU_TIMER_OFFSET                            0x3000
#define SCU_TIMER_BASE                              (SCU_TIMER_OFFSET + DSS_PRIVATE_REGS_BASE)
#define SCU_TIMER_N_BASE(n)                         (SCU_TIMER_BASE + 0x80 * (n))

#define SCU_TIMER_N_LOAD_LOW(n)                     (SCU_TIMER_N_BASE(n) + 0x0)
#define SCU_TIMER_N_LOAD_HIGH(n)                    (SCU_TIMER_N_BASE(n) + 0x4)
#define SCU_TIMER_N_CVALUE_LOW(n)                   (SCU_TIMER_N_BASE(n) + 0x10)
#define SCU_TIMER_N_CVALUE_HIGH(n)                  (SCU_TIMER_N_BASE(n) + 0x14)
#define SCU_TIMER_N_CTRL(n)                         (SCU_TIMER_N_BASE(n) + 0x20)
#define SCU_TIMER_N_COMPARE_LOW(n)                  (SCU_TIMER_N_BASE(n) + 0x40)
#define SCU_TIMER_N_COMPARE_HIGH(n)                 (SCU_TIMER_N_BASE(n) + 0x44)

/******************** MP TIMER ********************/
#define MPSCU_TIMER_OFFSET                          0x53000
#define MPSCU_TIMER_BASE                            (MPSCU_TIMER_OFFSET + MPSCU_SOC_REG_BASE)
#define MPSCU_TIMER_N_BASE(n)                       (MPSCU_TIMER_BASE + 0x80 * (n))

#define MPSCU_TIMER_N_LOAD_LOW(n)                   (MPSCU_TIMER_N_BASE(n) + 0x0)
#define MPSCU_TIMER_N_LOAD_HIGH(n)                  (MPSCU_TIMER_N_BASE(n) + 0x4)
#define MPSCU_TIMER_N_CVALUE_LOW(n)                 (MPSCU_TIMER_N_BASE(n) + 0x10)
#define MPSCU_TIMER_N_CVALUE_HIGH(n)                (MPSCU_TIMER_N_BASE(n) + 0x14)
#define MPSCU_TIMER_N_CTRL(n)                       (MPSCU_TIMER_N_BASE(n) + 0x20)
#define MPSCU_TIMER_N_COMPARE_LOW(n)                (MPSCU_TIMER_N_BASE(n) + 0x40)
#define MPSCU_TIMER_N_COMPARE_HIGH(n)               (MPSCU_TIMER_N_BASE(n) + 0x44)

/******************** TIMER_CTRL BIT FIELDS ********************/

#define TIMER_MODE_COMPARE              0
#define TIMER_MODE_LOAD                 1

#define TIMER_SINGLESHOT_CNTDOWN        0
#define TIMER_AUTORELOAD_CNTUP          1

#define TIMER_MODE_FROM_PREV_CNTR       0
#define TIMER_MODE_FROM_GLB_CNTR        1
#define TIMER_MODE_FROM_REG             2

/******************** OTHER ********************/
#define CYCLE_PER_SECONDS               1000000ULL

#ifdef  OS_CYCLE_PER_TICK
#undef  OS_CYCLE_PER_TICK
#define OS_CYCLE_PER_TICK               (CYCLE_PER_SECONDS / LOSCFG_BASE_CORE_TICK_PER_SECOND)
#endif

#define TIMER_FIRST_VAL_GET(low, high, carry, tick) do {    \
    __asm__ __volatile__(   \
        "MCOUNT a0:a1\n"    \
        "addc %2, a0, %3\n" \
        "add a0, a0, %3\n"  \
        "add a1, a1, %2\n"  \
        "{ mov %0, a0 ; mov %1, a1 }"   \
        : "=&r"(low), "=&r"(high), "=&r"(carry) \
        : "r"(tick) \
        : "cc", "a0", "a1"  \
    );  \
} while (0)

typedef union TimerCtrlReg {
    struct {
        UINT32 enabled          : 1;
        UINT32 mode             : 1;
        UINT32 intMask          : 1;
        UINT32                  : 1;
        UINT32 loadMode         : 1;
        UINT32 compareMode      : 2;
    };
    UINT32 val;
} TimerCtrlReg;

static const TimerCtrlReg g_ctrl = {
    .enabled = 1,
    .mode = TIMER_MODE_COMPARE,
    .intMask = 1,
    .compareMode = TIMER_MODE_FROM_GLB_CNTR,
};

LITE_OS_SEC_TEXT STATIC VOID HalTickEntry(VOID)
{
    UINT64 cycle;

    cycle = READ_REG(SCU_TIMER_N_COMPARE_LOW(0));
    cycle |= ((UINT64)READ_REG(SCU_TIMER_N_COMPARE_HIGH(0)) << 32); /* high 32 bits */
    cycle += OS_CYCLE_PER_TICK;

    WRITE_REG(SCU_TIMER_N_CTRL(0), 0);

    OsTickHandler();

    WRITE_REG(SCU_TIMER_N_COMPARE_LOW(0), (UINT32)(cycle & UINT32_MAX));
    WRITE_REG(SCU_TIMER_N_COMPARE_HIGH(0), (UINT32)((cycle >> 32) & UINT32_MAX)); /* high 32 bits */
    WRITE_REG(SCU_TIMER_N_CTRL(0), g_ctrl.val);
}

VOID HalClockInit(VOID)
{
    UINT32 ret;

    ret = LOS_HwiCreate(OS_TICK_INT_NUM, OS_HWI_PRIO_LOWEST, 0, HalTickEntry, 0);
    if (ret != LOS_OK) {
        PRINT_ERR("%s, %d create tick irq failed, ret:0x%x\n", __FUNCTION__, __LINE__, ret);
    }
}

VOID HalClockStart(VOID)
{
    UINT32 cycleHigh;
    UINT32 cycleLow;
    UINT32 carry;
    UINT32 ret;

    TIMER_FIRST_VAL_GET(cycleLow, cycleHigh, carry, (UINT32)OS_CYCLE_PER_TICK);

    WRITE_REG(SCU_TIMER_N_CTRL(0), 0);
    WRITE_REG(SCU_TIMER_N_COMPARE_LOW(0), cycleLow);
    WRITE_REG(SCU_TIMER_N_COMPARE_HIGH(0), cycleHigh);
    WRITE_REG(SCU_TIMER_N_CTRL(0), g_ctrl.val);

    ret = LOS_HwiEnable(OS_TICK_INT_NUM);
    if (ret != LOS_OK) {
        PRINT_ERR("%s, %d enable tick irq failed, ret:0x%x\n", __FUNCTION__, __LINE__, ret);
    }
}

UINT64 HalClockGetCycles(VOID)
{
    return ArchCpuCycleGet();
}

VOID HalDelayUs(UINT32 usecs)
{
    UINT64 cycles = (UINT64)usecs * GET_SYS_CLOCK() / OS_SYS_US_PER_SECOND;
    UINT64 deadline = HalClockGetCycles() + cycles;

    while (HalClockGetCycles() < deadline) {
        __asm__ volatile ("nop");
    }
}
