/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2020. All rights reserved.
 * Description: Linglong Dispatch Implementation
 * Author: Huawei LiteOS Team
 * Create: 2022-01-05
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

#include "asm/memmap_config.h"
#include "arch/asm.h"

.extern g_epc

.macro PUSH_SHARED_REGS pair1, pair2
    add.i.sll.4.s -96

    v.mov.2x.v.v128 \pair1, v32:v33;
    { v.mov.2x.v.v128 \pair2, v34:v35; v.st.v128.s \pair1, 0; }
    { v.mov.2x.v.v128 \pair1, v36:v37; v.st.v128.s \pair2, 16; }
    { v.mov.2x.v.v128 \pair2, v38:v39; v.st.v128.s \pair1, 32; }
    { v.mov.2x.v.v128 \pair1, v40:v41; v.st.v128.s \pair2, 48; }
    { v.mov.2x.v.v128 \pair2, v42:v43; v.st.v128.s \pair1, 64; }
    v.st.v128.s \pair2, 80;
.endm

.macro POP_SHARED_REGS pair1, pair2
    v.ld.v128.s \pair1, 0;
    { v.ld.v128.s \pair2, 16; v.mov.2x.v128.v v32:v33, \pair1; }
    { v.ld.v128.s \pair1, 32; v.mov.2x.v128.v v34:v35, \pair2; }
    { v.ld.v128.s \pair2, 48; v.mov.2x.v128.v v36:v37, \pair1; }
    { v.ld.v128.s \pair1, 64; v.mov.2x.v128.v v38:v39, \pair2; }
    { v.ld.v128.s \pair2, 80; v.mov.2x.v128.v v40:v41, \pair1; }
    v.mov.2x.v128.v v42:v43, \pair2;

    add.i.sll.4.s 96
.endm

.macro PUSH_L_REGS
    add.i.sll.4.s -16
    { v.st.l8.s l0, 0; v.st.l8.s l1, 4; }
    { v.st.l8.s l2, 8; v.st.l8.s l3, 12; }
.endm

.macro POP_L_REGS
    { v.ld.l8.s l0, 0; v.ld.l8.s l1, 4; }
    { v.ld.l8.s l2, 8; v.ld.l8.s l3, 12; }
    add.i.sll.4.s 16
.endm

.macro PUSH_VEC_REGS
    v.push.v128 v30:v31
    v.push.v128 v28:v29
    v.push.v128 v26:v27
    v.push.v128 v24:v25
    v.push.v128 v22:v23
    v.push.v128 v20:v21
    v.push.v128 v18:v19
    v.push.v128 v16:v17
    v.push.v128 v14:v15
    v.push.v128 v12:v13
    v.push.v128 v10:v11
    v.push.v128 v8:v9
    v.push.v128 v6:v7
    v.push.v128 v4:v5
    v.push.v128 v2:v3
    v.push.v128 v0:v1
.endm

.macro POP_VEC_REGS
    v.pop.v128 v0:v1
    v.pop.v128 v2:v3
    v.pop.v128 v4:v5
    v.pop.v128 v6:v7
    v.pop.v128 v8:v9
    v.pop.v128 v10:v11
    v.pop.v128 v12:v13
    v.pop.v128 v14:v15
    v.pop.v128 v16:v17
    v.pop.v128 v18:v19
    v.pop.v128 v20:v21
    v.pop.v128 v22:v23
    v.pop.v128 v24:v25
    v.pop.v128 v26:v27
    v.pop.v128 v28:v29
    v.pop.v128 v30:v31
.endm

.macro PUSH_GEN_REGS
    push.a128 a28:a29:a30:a31
    push.a128 a24:a25:a26:a27
    push.a128 a20:a21:a22:a23
    push.a128 a16:a17:a18:a19
    push.a128 a12:a13:a14:a15
    push.a128 a8:a9:a10:a11
    push.a128 a4:a5:a6:a7
    push.a128 a0:a1:a2:a3
.endm

.macro POP_GEN_REGS
    pop.a128 a0:a1:a2:a3
    pop.a128 a4:a5:a6:a7
    pop.a128 a8:a9:a10:a11
    pop.a128 a12:a13:a14:a15
    pop.a128 a16:a17:a18:a19
    pop.a128 a20:a21:a22:a23
    pop.a128 a24:a25:a26:a27
    pop.a128 a28:a29:a30:a31
.endm

.macro PUSH_SSR nextpc
    mov.a.ssr a3, sp

    /* sp, sr, fbr and pc */
    { mov.i a4, \nextpc ; mov.a.ssr a5, fbr }
    { mov.a.ssr a6, sr ; mov.a.ssr a7, sp }

#ifdef LOSCFG_ARCH_SOUP_ENABLE
    /* stack overflow/underflow protection (SOUP) registers */
    { mov.a.ssr a8, sup_uspb ; st.a128.iu a4:a5:a6:a7, a3, -16 }
    mov.a.ssr a9, sup_lspb
#else
    st.a128.iu a4:a5:a6:a7, a3, -16
#endif

    /* hwloop registers & circular buffer registers */
#ifdef LOSCFG_ARCH_SOUP_ENABLE
    { mov.a.ssr a4, lbeg1 ; st.a128.iu a8:a9:a10:a11, a3, -16 }
#else
    mov.a.ssr a4, lbeg1
#endif
    mov.a.ssr a5, lend1
    mov.a.ssr a6, cbegin
    mov.a.ssr a7, cend

    { mov.a.ssr a8, lcount0 ; st.a128.iu a4:a5:a6:a7, a3, -16 }
    mov.a.ssr a9, lbeg0
    mov.a.ssr a10, lend0
    mov.a.ssr a11, lcount1

    st.a128.iu a8:a9:a10:a11, a3, -16

    mov.ssr.a sp, a3

#ifdef LOSCFG_ARCH_FPU_ENABLE
    push.ssr128 fpctl /* push: fpctl, fpsr, fpieaddr */
#endif

.endm

/* Note: SOUP should be disabled when running this */
.macro POP_SSR

#ifdef LOSCFG_ARCH_FPU_ENABLE
    { nop@sau.f16 ; pop.ssr128 fpctl }  /* pop: fpctl, fpsr, fpieaddr */
#endif

    mov.a.ssr a3, sp
    add.i a3, a3, -16

    ld.a128.iu a4:a5:a6:a7, a3, 16

    /* hwloop registers & circular buffer registers */
    { mov.ssr.a lcount0, a4 ; ld.a128.iu a8:a9:a10:a11, a3, 16 }
    mov.ssr.a lbeg0, a5
    mov.ssr.a lend0, a6
    mov.ssr.a lcount1, a7
#ifdef LOSCFG_ARCH_SOUP_ENABLE
    { mov.ssr.a lbeg1, a8 ; ld.a128.iu a4:a5:a6:a7, a3, 16 }
#else
    mov.ssr.a lbeg1, a8
#endif
    mov.ssr.a lend1, a9
    mov.ssr.a cbegin, a10
    mov.ssr.a cend, a11

#ifdef LOSCFG_ARCH_SOUP_ENABLE
    /* stack overflow/underflow protection (SOUP) registers */
    { mov.late.ssr.a sup_uspb, a4 ; ld.a128.iu a8:a9:a10:a11, a3, 16 }
    mov.late.ssr.a sup_lspb, a5
#else
    ld.a128.iu a8:a9:a10:a11, a3, 16
#endif

    /* sp, sr, fbr and pc */
    { mov.ssr.a fbr, a9 ; add.i a3, a3, 16 }
    mov.ssr.a sp, a3
    mov.ssr.a sr, a10 /* SOUP status restored */
.endm

.macro IRQ_PUSH_REGS
    push.a128 a20:a21:a22:a23
    push.a128 a16:a17:a18:a19
    push.a128 a8:a9:a10:a11
    push.a128 a4:a5:a6:a7
.endm

.macro IRQ_POP_REGS
    pop.a128 a4:a5:a6:a7
    pop.a128 a8:a9:a10:a11
    pop.a128 a16:a17:a18:a19
    pop.a128 a20:a21:a22:a23
.endm

/* Switch to standalone stack. MODIFY: a0-a7 */
.macro SWITCH_STACK stack_start, stack_size
    /* get stack address */
    GET_STACK \stack_start, \stack_size, a2, a3, a0, a1

    { mov.a.ssr a4, sp ; mov.a.ssr a5, sr }
    { mov.a.ssr a6, sup_uspb ; mov.a.ssr a7, sup_lspb }

#ifdef LOSCFG_ARCH_SOUP_ENABLE
    set.sr.ssoup.dis
    mov.late.ssr.a sup_lspb, a0
    mov.late.ssr.a sup_uspb, a1
    mov.ssr.a sp, a1
    mov.ssr.a sr, a5 /* soup status restored */
#else
    mov.ssr.a sp, a1
#endif /* LOSCFG_ARCH_SOUP_ENABLE */

    push.a128 a4:a5:a6:a7
.endm

.macro IRQ_PUSH_SSR
#ifdef LOSCFG_ARCH_FPU_ENABLE
    push.ssr128       fpctl
#endif
    push.ssr128       lbeg0
    push.ssr128       lbeg1
    push.ssr128       cbegin
.endm

.macro IRQ_POP_SSR
    { nop@sau.f16 ; pop.ssr128 cbegin }
    { nop@sau.f16 ; pop.ssr128 lbeg1 }
    { nop@sau.f16 ; pop.ssr128 lbeg0 }
#ifdef LOSCFG_ARCH_FPU_ENABLE
    { nop@sau.f16 ; pop.ssr128 fpctl }
#endif
.endm

.macro IRQ_PUSH_VEC_REGS
    v.push.v128 v0:v1
    v.push.v128 v2:v3
.endm

.macro IRQ_POP_VEC_REGS
    v.pop.v128 v2:v3
    v.pop.v128 v0:v1
.endm

/* Restore from standalone stack. MODIFY: a4-a7 */
.macro RESTORE_STACK
    pop.a128 a4:a5:a6:a7

#ifdef LOSCFG_ARCH_SOUP_ENABLE
    set.sr.ssoup.dis
    mov.ssr.a sp, a4
    mov.late.ssr.a sup_uspb, a6
    mov.late.ssr.a sup_lspb, a7
    mov.ssr.a sr, a5
#else
    mov.ssr.a sp, a4
#endif /* LOSCFG_ARCH_SOUP_ENABLE */
.endm


.type ArchStartToRun, %function
.global ArchStartToRun
ArchStartToRun:
    /* initialize task context */
    mov.ssr.a       gpsr, a0

    b OsLoadNewTask

.type ArchTaskSchedule, %function
.global ArchTaskSchedule
ArchTaskSchedule:
    PUSH_GEN_REGS
    PUSH_SSR OsTaskSchedule_exit
    PUSH_VEC_REGS
    PUSH_SHARED_REGS v0:v1 v2:v3
    PUSH_L_REGS

    /* save top of stack */
    mov.a.ssr a28, sp
    st.a32.i a28, a1, 0

OsLoadNewTask:
#ifdef LOSCFG_ARCH_SOUP_ENABLE
    /* close SR.SOUP to update related registers, SOUP will restored after pop SR */
    set.sr.ssoup.dis
#endif

    /* change sp to new task */
    ld.a32.i a28, a0, 0
    mov.ssr.a sp, a28

    /* a14 a15 can restore by retrr, task pc in a14 */
    POP_L_REGS
    POP_SHARED_REGS v0:v1 v2:v3
    POP_VEC_REGS
    POP_SSR
    POP_GEN_REGS

    add.i.s a14, -(128 + 16)
    ld.a32.iz a14, a14

    btbinv
    rasinv
    j.a a14

OsTaskSchedule_exit:
    retrr

.global OsIrqHandler
OsIrqHandler:
    push.a128 a0:a1:a2:a3
    get.irq a0
    st.lock.isr a0
    { b.cf IrqHandler_exit }
    { st.clear.isr a0, 0 ; sync.str.start }

    ld.a32.i.s a0, 16  /* 16: a0~a3 */

    SAVE_EPC a0, g_epc, a1, a2

    /* Push regs. For O2 optimization, vector registers may be used. */
    IRQ_PUSH_REGS
    IRQ_PUSH_SSR
    IRQ_PUSH_VEC_REGS
    PUSH_SHARED_REGS v0:v1 v2:v3

#ifdef LOSCFG_IRQ_USE_STANDALONE_STACK
    /* switch sp to irq stack */
    SWITCH_STACK __stack, STACK_SIZE
#endif

    /* irq handling */
    call OsIntEntry

#ifdef LOSCFG_IRQ_USE_STANDALONE_STACK
    /* restore sp to task stack */
    RESTORE_STACK
#endif

    /* do task schedule */
    call OsSchedProcSchedFlag

    /* pop regs */
    POP_SHARED_REGS v0:v1 v2:v3
    IRQ_POP_VEC_REGS
    IRQ_POP_SSR
    IRQ_POP_REGS

    /* make sure clean irq operation done */
    sync.str.finish

IrqHandler_exit:
    pop.a128 a0:a1:a2:a3
    rfi

.global DefaultIrqHandler
DefaultIrqHandler:
    b .

.global NmiIrqHandler
NmiIrqHandler:
    push.a128 a0:a1:a2:a3

    ld.a32.i.s a0, 16  /* 16: a0~a3 */
    SAVE_EPC a0, g_epc, a1, a2

    IRQ_PUSH_REGS
    IRQ_PUSH_SSR
    IRQ_PUSH_VEC_REGS
    PUSH_SHARED_REGS v0:v1 v2:v3

#ifdef LOSCFG_IRQ_USE_STANDALONE_STACK
    /* switch to NMI stack */
    SWITCH_STACK __stack, STACK_SIZE
#endif

    /* handle NMI */
    call OsNMIHandler

#ifdef LOSCFG_IRQ_USE_STANDALONE_STACK
    /* switch stack back */
    RESTORE_STACK
#endif

    POP_SHARED_REGS v0:v1 v2:v3
    IRQ_POP_VEC_REGS
    IRQ_POP_SSR
    IRQ_POP_REGS
    pop.a128 a0:a1:a2:a3
    nop /* Restriction H.9 */
    rfi

.type ArchIntLock, %function
.global ArchIntLock
ArchIntLock:
    bextu.ssr.i a0, sr, 8, 1
    set.inter 0x7  /* 0x7: DI=1, DNMI unchanged */
    ret

.type ArchIntUnlock, %function
.global ArchIntUnlock
ArchIntUnlock:
    bextu.ssr.i a0, sr, 8, 1
    set.inter 0x6 /* 0x6: DI=0, DNMI unchanged */
    ret

.type ArchIntRestore, %function
.global ArchIntRestore
ArchIntRestore:
    { mov.a.ssr a14, sr ; mov.i a15, 0xFFFFFEFF }
    and        a14, a15, a14
    sll.i      a15, a0, 8
    or         a14, a15, a14
    mov.ssr.a  sr, a14
    retrr
