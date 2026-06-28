/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: GICv3 is initialized at the EL3 level.
 * Author: Huawei LiteOS Team
 * Create: 2022-05-18
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

#include "asm/register_config.h"
#include "arch/asm.h"

.arch armv8-a
.text
.align 5

.macro GIC_INTERRUPT_GROUP_SET igrpmodr igroupr group tmp
    mov     \tmp, #0xffffffff
    /* G1NS */
    tbz     \group, #1, 1f
    str     wzr, [\igrpmodr], #0x4
    str     \tmp, [\igroupr], #0x4
1:  /* G1S */
    tbz     \group, #0, 2f
    str     \tmp, [\igrpmodr], #0x4
    str     wzr, [\igroupr], #0x4
2:  /* G0 */
    cbnz    \group, 3f
    str     wzr, [\igrpmodr], #0x4
    str     wzr, [\igroupr], #0x4
3:
.endm

.global config_gicr_el3
.type   config_gicr_el3,function
config_gicr_el3:
    GET_CORE_ID x1, x2, x3

    ldr     x1, =GIC_BASE_ADDR
    add     x1, x1, #GICR_OFFSET
    mov     x2, #GICR_STRIDE
    mul     x2, x2, x3
    add     x1, x1, x2          /* GICR_base */

    /* GICR_PWRR supports only GIC600 and GIC700. GIC500 does not have this register. */
    mov     w2, #0x2
    str     w2, [x1, #0x24]     /* GICR_PWRR */

    mov     w2, #~0x2
    ldr     w3, [x1, #0x14]
    and     w3, w3, w2
    str     w3, [x1, #0x14]     /* GICR_WAKER */
    dsb     st
    isb
1:  ldr     w2, [x1, #0x14]
    and     w3, w2, #0x4
    tbnz    w3, #0x4, 1b

    add     x1, x1, #(1 << 16)  /* SGI_Base */
    add     x2, x1, #0xd00      /* GICR_IGRPMOD0 */
    add     x3, x1, #0x80       /* GICR_IGROUPR0 */
#ifndef LOSCFG_ARM_SIC
    GIC_INTERRUPT_GROUP_SET x2, x3, x0, w1
#else
    ldr     x4, =GIC_MIN_SPI_NUM
    lsr     x4, x4, #5
loop:
    GIC_INTERRUPT_GROUP_SET x2, x3, x0, w1
    sub     x4, x4, #1
    cbnz    x4, loop
#endif
    isb
    dsb     sy
    ret

.global config_gicc_el3
.type   config_gicc_el3,function
config_gicc_el3:
    /**
     * ICC_SRE_ELx
     * bit3: Enable. Enable lower ICC_SRE_ELx
     * bit2: DIB (disable IRQ bypass)
     * bit1: DFB (disable FIQ bypass)
     * bit0: SRE (system register enable)
     */
    mrs     x1, CurrentEL
    cmp     x1, #(0b01 << 2)
    beq     1f

    mrs     x0, S3_6_C12_C12_5  /* ICC_SRE_EL3 */
    orr     x0, x0, #0xF
    msr     S3_6_C12_C12_5, x0

    mrs     x1, scr_el3
    orr     x0, x1, #1          /* ICC_SRE_EL2 must set when NS=1 */
    msr     scr_el3, x0

    mrs     x0, S3_4_C12_C9_5   /* ICC_SRE_EL2 */
    orr     x0, x0, #0xF
    msr     S3_4_C12_C9_5, x0

    msr     scr_el3, x1         /* Restore NS */

    /* ICC_SRE_EL1 will set when GIC init */
1:
    mov     x0, #0xff
    msr     s3_0_c4_c6_0, x0    /* ICC_PMR_EL1 set priority threshold to max */
    isb
    ret

.global config_gicd_el3
.type   config_gicd_el3,function
config_gicd_el3:
    ldr     x1, =GIC_BASE_ADDR
    add     x1, x1, #GICD_OFFSET
    mov     w2, #0x30           /* enable ARE_S and ARE_NS first */
    str     w2, [x1]            /* GICD_CTLR */
0:  ldr     w2, [x1]
    and     w2, w2, #(1 << 31)  /* RWP */
    cbnz    w2, 0b
    mov     w2, #0x37           /* EnableGrp0 | EnableGrp1NS | EnableGrp1S | ARE_S | ARE_NS */
    str     w2, [x1]            /* GICD_CTLR */
    ldr     w3, [x1, #0x04]     /* GICD_TYPER */
    and     w2, w3, #0x1f       /* ITLinesNumber */
    cbz     w2, gicd_config_exit

#ifndef LOSCFG_ARM_SIC
    add     x3, x1, (0xd00+4)   /* GICD_IGRPMODR */
    add     x4, x1, (0x80+4)    /* GICD_IGROUPR */
#else
    ldr     x5, =GIC_MIN_SPI_NUM
    lsr     x5, x5, #3          /* (GIC_MIN_SPI_NUM << 5) * 4 */
    add     x3, x5, #0xd00
    add     x4, x5, 0x80
    add     x3, x1, x3    /* GICD_IGRPMODR */
    add     x4, x1, x4    /* GICD_IGROUPR */
#endif
set_interrupt_mode:
    GIC_INTERRUPT_GROUP_SET x3, x4, x0, w1
    sub     w2, w2, #0x1
    cbnz    w2, set_interrupt_mode
gicd_config_exit:
    ret
