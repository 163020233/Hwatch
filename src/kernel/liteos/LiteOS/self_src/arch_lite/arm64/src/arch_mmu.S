/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Arm64 Mmu Enable Implementation
 * Author: Huawei LiteOS Team
 * Create: 2022-06-09
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

#include "asm/memmap_config.h"

.arch armv8-a
.align 5
.text

.global MmuEnable
.global PageTableMemInit

/* clear first_page_table */
/* x0: pagetable addr, x1: size */
PageTableMemInit:
    cbz     x1, PageTableLoopDone
PageTableLoop:
    sub     x1, x1, #8
    str     xzr, [x0], #8
    cbnz    x1, PageTableLoop
PageTableLoopDone:
    ret

MmuEnable:
#ifdef LOSCFG_ARCH_SECURE_MONITOR_MODE
    /* Invalidate TLB */
    tlbi    alle3is
    isb
    dsb     sy

    /* Turn on the MMU */
    mrs     x9, sctlr_el3
    orr     x9, x9, #0x1
    msr     sctlr_el3, x9
    isb

    /* Invalidate TLB */
    tlbi    alle3
#else
    /* Invalidate TLB */
    tlbi    vmalle1is
    isb
    dsb     sy

    /* Turn on the MMU */
    mrs     x9, sctlr_el1
    orr     x9, x9, #0x1
    msr     sctlr_el1, x9
    isb

    /* Invalidate TLB */
    tlbi    vmalle1
#endif
    isb
    dsb     sy

    ret
