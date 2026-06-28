/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: LinxCore610 series cache maintenance implementation.
 * Author: Huawei LiteOS Team
 * Create: 2025-01-17
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

#include "los_typedef.h"
#include "los_init.h"
#include "arch/cpu.h"
#include "arch/barrier.h"
#include "asm/platform.h"

#include "arch/cache.h"

#define CACHE_SET_POS                   6
#define CACHE_WAY_POS                   29

USED STATIC UINT32 g_nSet;
USED STATIC UINT32 g_nAssociativity;

/* targets can override this configuartion to a constant to improve performance. */
#ifndef ARCH_CACHE_ASSOCIATIVITY
#define ARCH_CACHE_ASSOCIATIVITY        (g_nAssociativity)
#endif

#ifndef ARCH_CACHE_NSET
#define ARCH_CACHE_NSET                 (g_nSet)
#endif

#define ID_AA64MMFR2_EL1                S3_0_C0_C7_2
#define FEAT_CCIDX_GET(val)             ((UINT32)(((val) >> 20) & 0xF))
#define CCIDX_32BIT                     0
#define CCIDX_64BIT                     1

#define CCSIDR_EL1                      S3_1_C0_C0_0
#define CCSIDR_NASSOC_32BIT(val)        ((UINT32)(((val) >> 3) & 0x3FF))
#define CCSIDR_NSET_32BIT(val)          ((UINT32)(((val) >> 13) & 0x7FFF))
#define CCSIDR_NASSOC_64BIT(val)        ((UINT32)(((val) >> 3) & 0x1FFFFF))
#define CCSIDR_NSET_64BIT(val)          ((UINT32)(((val) >> 32) & 0xFFFFFF))

#define CLIDR_EL1                       S3_1_C0_C0_1
#define CLIRDR_LOC(val)                 ((UINT32)(((val) >> 24) & 0xF))
#define LOC_2                           2

#define CSSELR_EL1                      S3_2_C0_C0_0
#define L2_CACHE                        (0x1 << 1)

/* for linxcore610, flush L2-cache automatically clean and invalidate L1-cache when use set/way. */
#define CACHE_OP_ALL_DEFINE(cacheOp) do {                                        \
    UINT64 opCode;                                                               \
    DSB();                                                                       \
    AARCH64_SYSREG_WRITE(CSSELR_EL1, L2_CACHE);                                  \
    for (UINT32 set = 0; set < ARCH_CACHE_NSET; set++) {                         \
        for (UINT32 way = 0; way < ARCH_CACHE_ASSOCIATIVITY; way++) {            \
            opCode = L2_CACHE | (way << CACHE_WAY_POS) | (set << CACHE_SET_POS); \
            __asm__(cacheOp ", %x0" : : "r"(opCode));                            \
        }                                                                        \
    }                                                                            \
    DSB();                                                                       \
    ISB();                                                                       \
} while (0)

VOID ArchDCacheClean(VOID)
{
    CACHE_OP_ALL_DEFINE("dc csw");
}

/* DO NOT use this unless you know what you are doing. */
VOID ArchDCacheInvalid(VOID)
{
    CACHE_OP_ALL_DEFINE("dc isw");
}

VOID ArchDCacheFlush(VOID)
{
    CACHE_OP_ALL_DEFINE("dc cisw");
}

STATIC UINT32 ArchCacheInit(VOID)
{
    AARCH64_SYSREG_WRITE(CSSELR_EL1, L2_CACHE);

    UINT64 mmfr2 = AARCH64_SYSREG_READ(ID_AA64MMFR2_EL1);
    UINT64 ccsidr = AARCH64_SYSREG_READ(CCSIDR_EL1);
    UINT32 ccidx = FEAT_CCIDX_GET(mmfr2);
    if (ccidx == CCIDX_64BIT) {
        g_nAssociativity = CCSIDR_NASSOC_64BIT(ccsidr) + 1;
        g_nSet = CCSIDR_NSET_64BIT(ccsidr) + 1;
    } else if (ccidx == CCIDX_32BIT) {
        g_nAssociativity = CCSIDR_NASSOC_32BIT(ccsidr) + 1;
        g_nSet = CCSIDR_NSET_32BIT(ccsidr) + 1;
    } else {
        return LOS_NOK; /* not supported extension */
    }

    if ((g_nAssociativity == 0) || (g_nSet == 0)) {
        return LOS_NOK; /* invalid cache information */
    }

    if ((g_nAssociativity != ARCH_CACHE_ASSOCIATIVITY) || (g_nSet != ARCH_CACHE_NSET)) {
        return LOS_NOK; /* configuation error */
    }

    return LOS_OK;
}
SYS_INITCALL(ArchCacheInit, SYS_INIT_LEVEL_ARCH, SYS_INIT_SYNC_0);
