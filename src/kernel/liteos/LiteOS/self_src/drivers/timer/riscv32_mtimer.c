/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2022. All rights reserved.
 * Description: Timer Implementation
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
#include "asm/register_config.h"
#include "los_tick_pri.h"

#ifdef LOSCFG_KERNEL_SMP
#error "riscv32 timer does not support on SMP mode!"
#endif

#ifdef LOSCFG_BASE_CORE_TICK_HW_TIME
#error "riscv32 timer needs to turn off hw time!"
#endif

#define M_TIMER_INT_NUM                 7

#define UINT32_MASK_NUM                 0xFFFFFFFF
#define UINT64_SHIFT_32_BIT             32

/************* RISCV MTIMER Register offset *************/
#define REG_TIMER_CTRL                  0x0
#define REG_TIMER_DIV                   0x04
#define REG_TIMER_MTIME                 0x8
#define REG_TIMER_MTIME_H               0xC
#define REG_TIMER_MTIMECMP              0x10
#define REG_TIMER_MTIMECMP_H            0x14
#define TIMER_CTRL_CLK                  0x1

static VOID OsUpdateMtimecmpValue(UINT64 cycles)
{
    UINT32 mtimeCmpLo, mtimeCmpHi;
    UINT64 readTime = HalClockGetCycles() + cycles;

    mtimeCmpLo = (UINT32)(readTime & UINT32_MASK_NUM);
    mtimeCmpHi = (UINT32)((readTime >> UINT64_SHIFT_32_BIT) & UINT32_MASK_NUM); /* Take high 32 bits of UINT64 */
    /*
     * The following code sequence sets a 64-bit mtimecmp value without spuriously generating a timer interrupt
     * due to the intermediate value of the comparand.
     */
    WRITE_UINT32(UINT32_MASK_NUM, REG_BASE_MTIMER + REG_TIMER_MTIMECMP);  /* No smaller than old value. */
    WRITE_UINT32(mtimeCmpHi, REG_BASE_MTIMER + REG_TIMER_MTIMECMP_H);     /* No smaller than new value. */
    WRITE_UINT32(mtimeCmpLo, REG_BASE_MTIMER + REG_TIMER_MTIMECMP);       /* New value. */
}

STATIC VOID HalTickEntry(VOID)
{
    OsTickHandler();
    OsUpdateMtimecmpValue(OS_CYCLE_PER_TICK);
}

VOID HalClockInit(VOID)
{
    UINT32 ret = LOS_HwiCreate(M_TIMER_INT_NUM, 0, 0, HalTickEntry, 0);
    if (ret != LOS_OK) {
        PRINT_ERR("%s %d ret:%#x\n", __FUNCTION__, __LINE__, ret);
    }
}

UINT64 HalClockGetCycles(VOID)
{
    UINT32 mtimeLo, mtimeHi, mtimeHiTmp;

    READ_UINT32(mtimeHiTmp, REG_BASE_MTIMER + REG_TIMER_MTIME_H);
    READ_UINT32(mtimeLo, REG_BASE_MTIMER + REG_TIMER_MTIME);
    READ_UINT32(mtimeHi, REG_BASE_MTIMER + REG_TIMER_MTIME_H);

    /* MTIME_H carry means MTIME wrap around, need read new MTIME value */
    if (mtimeHiTmp != mtimeHi) {
        READ_UINT32(mtimeLo, REG_BASE_MTIMER + REG_TIMER_MTIME);
    }
    return mtimeLo + (((UINT64)mtimeHi) << UINT64_SHIFT_32_BIT);
}

VOID HalClockStart(VOID)
{
    UINT32 ret;

    WRITE_UINT32(0, REG_BASE_MTIMER + REG_TIMER_DIV);
    WRITE_UINT32(0, REG_BASE_MTIMER + REG_TIMER_MTIME_H);
    WRITE_UINT32(0, REG_BASE_MTIMER + REG_TIMER_MTIME);
    WRITE_UINT32(0, REG_BASE_MTIMER + REG_TIMER_MTIMECMP_H);
    WRITE_UINT32(OS_CYCLE_PER_TICK, REG_BASE_MTIMER + REG_TIMER_MTIMECMP);
    WRITE_UINT32(TIMER_CTRL_CLK, REG_BASE_MTIMER + REG_TIMER_CTRL);

    ret = HalIrqUnmask(M_TIMER_INT_NUM);
    if (ret != LOS_OK) {
        PRINT_ERR("%s %d ret:%#x\n", __FUNCTION__, __LINE__, ret);
    }
}

VOID HalDelayUs(UINT32 usecs)
{
    UINT64 cycles = (UINT64)usecs * GET_SYS_CLOCK() / OS_SYS_US_PER_SECOND;
    UINT64 deadline = HalClockGetCycles() + cycles;

    while (HalClockGetCycles() < deadline) {
        __asm__ volatile ("nop");
    }
}
