/*----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2024. All rights reserved.
 * Description: Aarch64 Cache Implementation
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
 *---------------------------------------------------------------------------*/

#include "arch/asm.h"

#define CACHE_LINE 64

.macro cache_range_op, cache op
    bic     x3, x0, #(CACHE_LINE-1) /* align the start with a cache line */
.Lcache_range_op_loop\@:
    \cache  \op, x3
    add     x3, x3, #CACHE_LINE
    cmp     x3, x1
    blo     .Lcache_range_op_loop\@
    dsb     sy
.endm

/*
 * x0  clidr_el1
 * x1  used to traverse cache level
 * x2  levelNum
 * x3  Ctype<n>   --> L
 * x4  set level  -->  ccsidr_el1 --> WayNum
 * x5  SetNum
 * x6  (32 - A)
 * x7  wayNum
 * x8  set level
 * x9  set level + set Set
 * x10  set level + set Set + set Way
 *
 * CLIDR_EL1:
 * |31 |26  24|23  21|20   18|...|3(n-1)+2   3(n-1)|...|2      0|
 * |   |LoC   |LoUIS |Ctype7 |...|Ctype<n>         |...|Ctype1
 *
 * CSSELR_EL1:
 * |31   4|3      1|0
 * |RES0  |Level   |InD
 *
 * CCSIDR_EL1:
 * |31   |27       13|12             3|2        0|
 * |     |NumSets    |Associativity   |LineSize
 *
 * DCCISW:
 * |31   (32-A)|   B|B-1   L|(L-1)  4|3      1|0
 * |Way        |    |Set    |RES0    |Level   |RES0
 *
 * A = Log2(ASSOCIATIVITY), L =Log2(LINELEN), B = (L + S), S = Log2(NSETS).
 */
.macro cache_setway_op, cache op
    dsb     sy
    mrs     x0, clidr_el1
    ubfx    x2, x0, #24, #3      /* clidr_el1.LoC  */
    cmp     x2, #0
    b.eq    arm64_flush_dcache_end\@
    mov     x1, #0               /* x1 = level */

arm64_flush_dcache_by_level\@:
    add     x3, x1, x1, lsl #1   /* x3 = x1*3, Ctype<n> 3bit */
    lsr     x3, x0, x3
    and     x3, x3, #7
    cmp     x3, #2               /* 2:Data cache only. */
    b.lt    arm64_flush_dcache_next_level\@

    lsl     x4, x1, #1
    msr     csselr_el1, x4       /* set cache level */
    isb

    mrs     x4, ccsidr_el1
    and     x3, x4, #7           /* (Log2(LINELEN)) - 4. */
    add     x3, x3, #4           /* L, line length offset */
    ubfx    x5, x4, #13, #15     /* (Number of sets in cache) - 1 */
    ubfx    x4, x4, #3, #10      /* (Associativity/way of cache) - 1 */
    clz     w6, w4               /* (32-A), bit position of #ways */
    lsl     x8, x1, #1           /* set level */

arm64_flush_dcache_by_set\@:
    mov     x7, x4
    lsl     x11, x5, x3          /* set Set */
    orr     x9, x8, x11
arm64_flush_dcache_by_way\@:
    lsl     x11, x7, x6          /* set Way */
    orr     x10, x9, x11
    \cache  \op, x10             /* Cache Maintenance */
    subs    x7, x7, #1           /* way-- */
    b.ge    arm64_flush_dcache_by_way\@
    subs    x5, x5, #1           /* set-- */
    b.ge    arm64_flush_dcache_by_set\@

arm64_flush_dcache_next_level\@:
    add     x1, x1, #1           /* level++ */
    cmp     x2, x1
    b.gt    arm64_flush_dcache_by_level\@

    mov     x0, #0
    msr     csselr_el1, x0       /* restore csselr_el1 */
arm64_flush_dcache_end\@:
    dsb     sy
    isb
.endm

FUNCTION(ArchDCacheInvByAddr)
    cache_range_op dc ivac         /* invalidate dcache to PoC by MVA */
    ret

FUNCTION(ArchDCacheCleanByAddr)
    cache_range_op dc cvac         /* clean cache to PoC by MVA */
    ret

FUNCTION(ArchDCacheFlushByAddr)
    cache_range_op dc civac        /* Clean and invalidate dcache to PoC by MVA */
    ret

#ifndef LOSCFG_ARCH_LINXCORE_610
FUNCTION(ArchDCacheFlush)
    cache_setway_op dc cisw        /* Clean and invalidate dcache by Set/Way */
    ret

FUNCTION(ArchDCacheClean)
    cache_setway_op dc csw         /* Clean dcache by Set/Way */
    ret

FUNCTION(ArchDCacheInvalid)
    cache_setway_op dc isw         /* Invalid dcache by Set/Way */
    ret
#endif

/*
 *  ArchICacheFlushByAddr(start,end)
 *
 *  Ensure that the I and D caches are coherent within specified region.
 *  This is typically used when code has been written to a memory region,
 *  and will be executed.
 *
 *  - start   - virtual start address of region
 *  - end     - virtual end address of region
 */
FUNCTION(ArchICacheFlushByAddr)
    mrs     x3, ctr_el0
    ubfx    x3, x3, #16, #4
    mov     x2, #0x4

    lsl     x2, x2, x3
    sub     x3, x2, #0x1
    bic     x4, x0, x3
fir1 :
    dc      cvau, x4
    add     x4, x4, x2
    cmp     x4, x1
    b.cc    fir1
    dsb     ish
    mrs     x3, ctr_el0
    and     x3, x3, #0xf
    mov     x2, #0x4

    lsl     x2, x2, x3
    sub     x3, x2, #0x1
    bic     x4, x0, x3
fir2 :
    ic      ivau, x4
    add     x4, x4, x2
    cmp     x4, x1
    b.cc    fir2
    dsb     ish
    isb
    mov     x0, #0x0

    ret
