/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Aarch64 Hw Privilege Levels Implementation
 * Author: Huawei LiteOS Team
 * Create: 2022-05-07
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

#include "arch/regs.h"

.arch armv8-a
.text
.align 5

/** Switch from EL3 or EL2 to EL1 state.
 * If secure-monitor required, keep state unchanged.
 * Following configuration will be changed:
 * 1. change CurrentEL to EL1
 * 2. disable IRQ/FIQ traps to EL3(scr_el3)
 * 3. enable cpacr_el1, activity monitor and trace access from EL1 (cptr_el2)
 * 4. run in AARCH64 state, virtualization disabled (hcr_el2)
 * 5. enable FPU instructions from EL1 (cpacr_el1)
 * Note: Secure state will NOT changed in this routine, use of EL1s/EL1ns should be decided before calling.
 * Register usage:
 *  x0-x1   Arguments
 */
.global arm64_elX_to_el1
.type   arm64_elX_to_el1,function
arm64_elX_to_el1:
    mrs     x1, CurrentEL
    cmp     x1, #(0b01 << 2)
    bne     notEL1
    /* Already in EL1 */
    ret

notEL1:
    cmp     x1, #(0b10 << 2)
    beq     inEL2

    /* set EL2 to 64bit */
    mrs     x1, scr_el3
    orr     x1, x1, #(1 << 10)
#ifdef LOSCFG_ARCH_SECURE_MONITOR_MODE
    orr     x1, x1, #(1 << 2)  /* FIQ, physical FIQ interrupts are taken to EL3. */
    orr     x1, x1, #(1 << 1)  /* IRQ, physical IRQ interrupts are taken to EL3. */
#else
    bic     x1, x1, #(1 << 2)  /* FIQ, physical FIQ interrupts are not taken to EL3. */
    bic     x1, x1, #(1 << 1)  /* IRQ, physical IRQ interrupts are not taken to EL3. */
#endif
    msr     scr_el3, x1
    msr     elr_el3, x30  /* x30 == lr */
    mov     x1, #((0b1111 << 6) | (0b0101)) /* EL1h runlevel */
    msr     spsr_el3, x1
    b       confEL1

inEL2:
    msr     elr_el2, x30  /* x30 == lr */
    mov     x1, #((0b1111 << 6) | (0b0101)) /* EL1h runlevel */
    msr     spsr_el2, x1

confEL1:
    /* disable EL2 coprocessor traps */
#ifdef LOSCFG_ARCH_ARM_SVE2
    mov     x0, #0x32ff
#else
    mov     x0, #0x33ff
#endif
    msr     cptr_el2, x0

    /* set EL1 to 64bit */
    mov     x0, #(1 << 31)
    msr     hcr_el2, x0

    /* disable EL1 FPU traps */
#ifdef LOSCFG_ARCH_ARM_SVE2
    mov     x0, #(0x330000)
#else
    mov     x0, #(0b11 << 20)
#endif
    msr     cpacr_el1, x0

    /* set up the EL1 bounce interrupt */
    mov     x0, sp
    msr     sp_el1, x0

    isb
    eret

/** Disable traps to EL3 and superiver calls. Call in EL3.
 * All traps should goes to EL1 for secure reason.
 * 1. Disabled Traps to EL3:
 *  a. SCR_EL3.{TWE, TWI, ST)
 *  b. CPTR_EL3.{TAM, TCPAC, TTA, TFP}
 *  c. MDCR_EL3.{TTRF, TDOSA, TDA, TPM}
 * 2. Disable instructions to EL3:
 *  a. SCR_EL3.{HCE, SMD}
 */
.global arm64_disable_el3_traps
.type   arm64_disable_el3_traps,function
arm64_disable_el3_traps:
    mrs     x0, scr_el3
    orr     x0, x0, #(0b1 << 7)     /* SMD */
    bic     x0, x0, #(0b1 << 8)     /* HCE */
    bic     x0, x0, #(0b11 << 12)   /* TWE, TWI */
    orr     x0, x0, #(0b1 << 11)    /* ST */
    msr     scr_el3, x0

    mrs     x0, cptr_el3
    ldr     x1, =0xC0100400         /* TAM, TCPAC, TTA, TFP */
    bic     x0, x0, x1
    msr     cptr_el3, x0

    mrs     x0, mdcr_el3
    ldr     x1, =0x80640            /* TTRF, TDOSA, TDA, TPM */
    bic     x0, x0, x1
    msr     mdcr_el3, x0
    ret
