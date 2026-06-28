/*----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: LiteOS Perf Arm Pmu Module HeadFile
 * Author: Huawei LiteOS Team
 * Create: 2022-10-17
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
 *---------------------------------------------------------------------------*/

#ifndef _ARCH_PMU_H
#define _ARCH_PMU_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define PMU_FLAG_MASK                   0xffffffff         /* Mask for writable bits */
#define PMU_OVERFLOWED_MASK             PMU_FLAG_MASK      /* Mask for pmu overflow */
#define PMU_EVTYPE_MASK                 0xc800ffff         /* Mask for writable bits */
#define PMU_READ_MASK                   0x00000000ffffffff /* Mask for read count */
#define PMU_PERIOD_MASK                 0xffffffff00000000 /* Mask for period set */

#define PMCR_E                          (1U << 0) /* Enable all counters */
#define PMCR_P                          (1U << 1) /* Reset all counters */
#define PMCR_C                          (1U << 2) /* Cycle counter reset */
#define PMCR_D                          (1U << 3) /* CCNT counts every 64th cpu cycle */
#define PMCR_X                          (1U << 4) /* Export to ETM */
#define PMCR_DP                         (1U << 5) /* Disable CCNT if non-invasive debug */
#define PMCR_LC                         (1U << 6) /* Overflow on 64 bit cycle counter */
#define PMCR_MASK                       0x7f      /* Mask for writable bits */

#define PMU_MAX_COUNTERS                32
#define PMU_COUNTER_MASK                (PMU_MAX_COUNTERS - 1)

#define PMU_IDX_MAX_COUNTER             7
#define PMU_IDX_CYCLE_COUNTER           0
#define PMU_IDX_COUNTER0                1

#define PMU_IDX2CNT(x)                  (((x) - PMU_IDX_COUNTER0) & PMU_COUNTER_MASK)
#define PMU_CNT2BIT(x)                  (1ULL << (x))

enum PmuEventType {
    PERF_HW_PMNC_SW_INCR               = 0x00,
    PERF_HW_L1_ICACHE_REFILL           = 0x01, /* icache access miss event */
    PERF_HW_ITLB_REFILL                = 0x02,
    PERF_HW_L1_DCACHE_REFILL           = 0x03, /* dcache access miss event */
    PERF_HW_L1_DCACHE_ACCESS           = 0x04, /* dcache access event */
    PERF_HW_DTLB_REFILL                = 0x05,
    PERF_HW_MEM_READ                   = 0x06,
    PERF_HW_MEM_WRITE                  = 0x07,
    PERF_HW_INSTR_EXECUTED             = 0x08, /* instructions event */
    PERF_HW_EXC_TAKEN                  = 0x09,
    PERF_HW_EXC_EXECUTED               = 0x0A,
    PERF_HW_CID_WRITE                  = 0x0B,
    PERF_HW_PC_WRITE                   = 0x0C, /* branch forecast event */
    PERF_HW_PC_IMM_BRANCH              = 0x0D,
    PERF_HW_PC_PROC_RETURN             = 0x0E,
    PERF_HW_MEM_UNALIGNED_ACCESS       = 0x0F,
    PERF_HW_PC_BRANCH_MIS_PRED         = 0x10, /* branch forecast miss event */
    PERF_HW_CLOCK_CYCLES               = 0x11, /* cycles event */
    PERF_HW_PC_BRANCH_PRED             = 0x12,
    PERF_HW_MEM_ACCESS                 = 0x13,
    PERF_HW_L1_ICACHE_ACCESS           = 0x14, /* icache access event */
    PERF_HW_L1_DCACHE_WB               = 0x15,
    PERF_HW_L2_CACHE_ACCESS            = 0x16,
    PERF_HW_L2_CACHE_REFILL            = 0x17,
    PERF_HW_L2_CACHE_WB                = 0x18,
    PERF_HW_BUS_ACCESS                 = 0x19,
    PERF_HW_MEM_ERROR                  = 0x1A,
    PERF_HW_OP_SPEC                    = 0x1B,
    PERF_HW_TTBR_WRITE                 = 0x1C,
    PERF_HW_BUS_CYCLES                 = 0x1D,
    PERF_HW_CHAIN                      = 0x1E,
    PERF_HW_L2D_CACHE_ALLOCATE         = 0x20,
    PERF_HW_BR_RETIRED                 = 0x21,
    PERF_HW_BR__MIS_PRED_RETIRED       = 0x22,
    PERF_HW_STALL_FRONTEND             = 0x23,
    PERF_HW_STALL_BACKEND              = 0x24,
    PERF_HW_L1D_TLB                    = 0x25,
    PERF_HW_L1I_TLB                    = 0x26,
    PERF_HW_L3D_CACHE_ALLOCATE         = 0x29,
    PERF_HW_L3D_CACHE_REFILL           = 0x2A,
    PERF_HW_L3D_CACHE                  = 0x2B,
    PERF_HW_L2D_TLB_REFILL             = 0x2D,
    PERF_HW_L2D_TLB                    = 0x2F,
    PERF_HW_DTLB_WALK                  = 0x34,
    PERF_HW_ITLB_WALK                  = 0x35,
    PERF_HW_LL_CACHE_RD                = 0x36,
    PERF_HW_LL_CACHE_MISS_RD           = 0x37,
    PERF_HW_REMOTE_ACCESS_RD           = 0x38,
    PERF_HW_L1D_CACHE_LD               = 0x40,
    PERF_HW_L1D_CACHE_ST               = 0x41,
    PERF_HW_L1D_CACHE_REFILL_LD        = 0x42,
    PERF_HW_L1D_CACHE_REFILL_ST        = 0x43,
    PERF_HW_L1D_CACHE_REFILL_INNER     = 0x44,
    PERF_HW_L1D_CACHE_REFILL_OUTER     = 0x45,
    PERF_HW_L2D_CACHE_ST               = 0x51,
    PERF_HW_L2D_CACHE_REFILL_LD        = 0x52,
    PERF_HW_L2D_CACHE_REFILL_ST        = 0x53,
    PERF_HW_BUS_ACCESS_LD              = 0x60,
    PERF_HW_BUS_ACCESS_ST              = 0x61,
    PERF_HW_MEM_ACCESS_LD              = 0x66,
    PERF_HW_MEM_ACCESS_ST              = 0x67,
    PERF_HW_LD_SPEC                    = 0x70,
    PERF_HW_ST_SPEC                    = 0x71,
    PERF_HW_LDST_SPEC                  = 0x72,
    PERF_HW_DP_SPEC                    = 0x73,
    PERF_HW_ASE_SPEC                   = 0x74,
    PERF_HW_VFP_SPEC                   = 0x75,
    PERF_HW_PC_WRITE_SPEC              = 0x76,
    PERF_HW_CRYPTO_SPEC                = 0x77,
    PERF_HW_BR_IMMED_SPEC              = 0x78,
    PERF_HW_BR_RETURN_SPEC             = 0x79,
    PERF_HW_BR_INDIRECT_SPEC           = 0x7A,
    PERF_HW_EXC_IRQ                    = 0x86,
    PERF_HW_EXC_FIQ                    = 0x87,
    PERF_HW_L3D_CACHE_RD               = 0xA0,
    PERF_HW_L3D_CACHE_REFILL_RD        = 0xA2,
    PERF_HW_L3D_CACHE_REFILL_PREFETCH  = 0xC0,
    PERF_HW_L2D_CACHE_REFILL_PREFETCH  = 0xC1,
    PERF_HW_L1D_CACHE_REFILL_PREFETCH  = 0xC2,
    PERF_HW_L2D_WS_MODE                = 0xC3,
    PERF_HW_L1D_WS_MODE_ENTRY          = 0xC4,
    PERF_HW_L1D_WS_MODE                = 0xC5,
    PERF_HW_PREDECODE_ERROR            = 0xC6,
    PERF_HW_L3D_WS_MODE                = 0xC7,
    PERF_HW_BR_COND_PRED               = 0xC9,
    PERF_HW_BR_INDIRECT_MIS_PRED       = 0xCA,
    PERF_HW_BR_INDIRECT_ADDR_MIS_PRED  = 0xCB,
    PERF_HW_BR_COND_MIS_PRED           = 0xCC,
    PERF_HW_BR_INDIRECT_ADDR_PRED      = 0xCD,
    PERF_HW_BR_RETURN_ADDR_PRED        = 0xCE,
    PERF_HW_BR_RETURN_ADDR_MIS_PRED    = 0xCF,
    PERF_HW_L2D_LLWALK_TLB             = 0xD0,
    PERF_HW_L2D_LLWALK_TLB_REFILL      = 0xD1,
    PERF_HW_L2D_L2WALK_TLB             = 0xD2,
    PERF_HW_L2D_L2WALK_TLB_REFILL      = 0xD3,
    PERF_HW_L2D_S2_TLB                 = 0xD4,
    PERF_HW_L2D_S2_TLB_REFILL          = 0xD5,
    PERF_HW_L2D_CACHE_STASH_DROPPED    = 0xD6,
    PERF_HW_STALL_FRONTEND_CACHE       = 0xE1,
    PERF_HW_STALL_FRONTEND_TLB         = 0xE2,
    PERF_HW_STALL_FRONTEND_PDERR       = 0xE3,
    PERF_HW_STALL_BACKEND_ILOCK        = 0xE4,
    PERF_HW_STALL_BACKEND_ILOCK_AGU    = 0xE5,
    PERF_HW_STALL_BACKEND_ILOCK_FPU    = 0xE6,
    PERF_HW_STALL_BACKEND_LD           = 0xE7,
    PERF_HW_STALL_BACKEND_ST           = 0xE8,
    PERF_HW_STALL_BACKEND_LD_CACHE     = 0xE9,
    PERF_HW_STALL_BACKEND_LD_TLB       = 0xEA,
    PERF_HW_STALL_BACKEND_ST_STB       = 0xEB,
    PERF_HW_STALL_BACKEND_ST_TLB       = 0xEC,
    PERF_HW_MAX
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ARCH_PMU_H */
