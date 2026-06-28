/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2022. All rights reserved.
 * Description: Cortex-M Dispatch Implementation
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

#include "arch/macro.h"

#ifdef __IASMARM__
    PRESERVE8
    SECTION    .text:CODE(2)
    THUMB
#else /* __GNUC__ */
    .syntax unified
#ifdef LOSCFG_ARCH_CORTEX_M0
    .arch armv6-m
#elif defined(LOSCFG_ARCH_CORTEX_M55)
    .arch armv8.1-m.main
#else
    .arch armv7e-m
#endif
    .thumb
    .section .text
#endif

    EXTERNAL OsSchedTaskSwitch
    EXTERNAL g_oldTask
    EXTERNAL g_runTask

    EQUAL OS_NVIC_INT_CTRL,   0xE000ED04
    EQUAL OS_NVIC_SYSPRI2,    0xE000ED20     /* bit[31-24]:SysTick, bit[23-16]:PendSV */
    EQUAL OS_NVIC_PENDSV_PRI, 0xF0F00000
    EQUAL OS_NVIC_PENDSVSET,  0x10000000

    FUNC ArchStartToRun
/* R0:first task */
ArchStartToRun:
    FNSTART
    CANTUNWIND
    ldr     r4, =OS_NVIC_SYSPRI2
    ldr     r5, =OS_NVIC_PENDSV_PRI
    str     r5, [r4]

    ldr     r1, =g_oldTask
    str     r0, [r1]

    ldr     r1, =g_runTask
    str     r0, [r1]

    movs    r1, #2
    msr     CONTROL, r1             /* thread mode, PSP */

#ifdef LOSCFG_ARCH_CORTEX_M0
    ldr     r3, [r0]
    adds    r3, r3, #36

    ldmfd   r3!, {r0-r2}
    adds    r3, r3, #4
    ldmfd   r3!, {R4-R7}

    msr     psp, r3
    subs    r3, r3, #20
    ldr     r3,  [r3]
#else
    ldr     r11, [r0]

#ifdef LOSCFG_ARCH_ARM_V8M
    ldmfd   r11!, {r2}
    msr     PSPLIM, r2              /* Set the PSPLIM_NS about current task. */

#ifdef LOSCFG_TRUSTZONE
    ldmfd   r11!, {r0, r3}          /* secureContextSp, lr */
#endif
#endif

    add     r11, r11, #36
    /* __VFP_FP__ is set by -mfpu; __SOFTFP__ is set by -mfloat-abi=soft. */
#if (defined(__VFP_FP__) && !defined(__SOFTFP__)) || defined(__ARMVFP__)
    add     r11, r11, #4
#endif

    ldmfd   r11!, {R0-R7}           /* R0~R3, R12, LR, PC, XPSR */

    msr     psp, r11

#ifdef LOSCFG_ARCH_FPU_ENABLE
    vpush   {s0};
    vpop    {s0};
#endif

#endif /* LOSCFG_ARCH_CORTEX_M0 */

    mov     lr, r5

    cpsie   I

#ifdef LOSCFG_TRUSTZONE
    push    {lr}
    bl      ArchInitSecureContext   /* set PSP_S, PSPLIM_S, CONTROL_S */
    pop     {lr}
#endif

    bx      r6
    FNEND

    FUNC ArchIntLock
ArchIntLock:
    FNSTART
    CANTUNWIND
    mrs     r0, PRIMASK
    cpsid   I
    bx      lr
    FNEND

    FUNC ArchIntUnlock
ArchIntUnlock:
    FNSTART
    CANTUNWIND
    mrs     r0, PRIMASK
    cpsie   I
    bx      lr
    FNEND

    FUNC ArchIntRestore
ArchIntRestore:
    FNSTART
    CANTUNWIND
    msr     PRIMASK, r0
    bx      lr
    FNEND

    FUNC ArchTaskSchedule
ArchTaskSchedule:
    FNSTART
    CANTUNWIND
    ldr     r2, =OS_NVIC_INT_CTRL
    ldr     r3, =OS_NVIC_PENDSVSET
    str     r3, [r2]
    /* if in thread mode, save the PRIMASK, if in handle mode, no need to save */
    mrs     r3, IPSR
    cbnz    r3, OsTaskSchedule_exit
    mrs     r0, PRIMASK
    cpsie   i
    msr     PRIMASK, r0
OsTaskSchedule_exit:
    bx      lr
    FNEND

    FUNC OsPendSV
OsPendSV:
    FNSTART
    CANTUNWIND
TaskSwitch:
    mrs     r2, psp

#ifdef LOSCFG_TRUSTZONE
    ldr     r1, =g_secureContext
    ldr     r0, [r1]
    cbz     r0, SaveNsContext       /* if r0 = 0, indicates that the current task does not have a security stack. */

    push    {r0-r2, lr}
    bl      ArchSecureSaveContext   /* parame: r0 = g_secureContext */
    pop     {r0-r2, lr}

    lsls    r1, lr, #25     /* r1 = LR<<25. Bit[6] of EXC_RETURN is 1 if SP_S was used to restore stack frame. */
    bpl     SaveNsContext   /* r1 >= 0, Bit[6] of EXC_RETURN(LR) is 0, SP_NS was used to restore stack frame. */
    b       SaveSecureInfo

SaveNsContext:
#endif /* LOSCFG_TRUSTZONE */

#ifdef LOSCFG_ARCH_CORTEX_M0
    subs    r2, #36
    stmia   r2!, {r4-r7}
    mov     r3, r8
    mov     r4, r9
    mov     r5, r10
    mov     r6, r11
    mov     r7, r12
    stmia   r2!, {r3 - r7}

    subs    r2, #36
#else
#if (defined(__VFP_FP__) && !defined(__SOFTFP__)) || defined(__ARMVFP__)
    /* when enter the exc or interrupt, lr's value is EXC_RETURN. If FPCA = 1, the 4's bit of EXC_RETURN is 0. */
    tst     lr, #0x10
    it      eq
    vstmdbeq  r2!, {d8-d15}         /* push VFP registers. */
    stmfd     r2!, {r14}
#endif
    stmfd   r2!, {r4-r12}

#ifdef LOSCFG_ARCH_ARM_V8M

#ifdef LOSCFG_TRUSTZONE
SaveSecureInfo:
    stmfd   r2!, {r0, lr}           /* store g_secureContext, LR */
#endif
    mrs     r1, PSPLIM
    stmfd   r2!, {r1}
#endif

#endif /* LOSCFG_ARCH_CORTEX_M0 */

    ldr     r0, =g_oldTask
    ldr     r1, [r0]
    str     r2, [r1]

SwitchToNewTask:
    ldr     r2, =g_runTask
#ifdef LOSCFG_SCHED_LATENCY
#if defined(LOSCFG_BASE_CORE_TSK_MONITOR) || defined(LOSCFG_KERNEL_TRACE) || \
    (defined(LOSCFG_KERNEL_PERF) && defined(LOSCFG_PERF_SW_PMU))
    push    {r0, r2, lr}
    ldr     r0, [r2]
    bl      OsSchedTaskSwitch       /* r0 = newTask; r1 =  runTask */
    pop     {r0, r2, lr}
#endif
#endif
    ldr     r2, [r2]
    str     r2, [r0]                /* g_oldTask = g_runTask */
    ldr     r1, [r2]

#ifdef LOSCFG_ARCH_ARM_V8M
    ldmfd   r1!, {r2}
    msr     PSPLIM, r2
#ifdef LOSCFG_TRUSTZONE
    ldmfd   r1!, {r0, lr}           /* get from stack: r0 = g_secureContext, r3 = LR */
    ldr     r2, =g_secureContext
    str     r0, [r2]                /* restore g_secureContext */

    cbz     r0, RestoreNsContext
    push    {r1, lr}
    bl      ArchSecureLoadContext
    pop     {r1, lr}

    lsls    r2, lr, #25
    bpl     RestoreNsContext        /* r2 >= 0, Bit[6] of EXC_RETURN(LR) is 0, indicates that stack is PSP_NS. */
    msr     PSP, r1
    bx      lr

RestoreNsContext:
#endif /* LOSCFG_TRUSTZONE */
#endif

#ifdef LOSCFG_ARCH_CORTEX_M0
    adds    r1,   #16
    ldmfd   r1!, {r3-r7}

    mov     r8, r3
    mov     r9, r4
    mov     r10, r5
    mov     r11, r6
    mov     r12, r7
    subs    r1,  #36
    ldmfd   r1!, {r4-r7}

    adds    r1,   #20
#else
    ldmfd   r1!, {r4-r12}
#endif

#if (defined(__VFP_FP__) && !defined(__SOFTFP__)) || defined(__ARMVFP__)
    ldmfd   r1!, {r14}
    tst     r14, #0x10
    it      eq
    vldmiaeq r1!, {d8-d15}
#endif

    msr     psp,  r1
    bx      lr
    FNEND

#ifdef __IASMARM__
    END
#endif