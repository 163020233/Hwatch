/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2022. All rights reserved.
 * Description: Cortex-M Hw Exc Implementation
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
    .fpu vfpv4
    .section .text
#endif

    EXTERNAL OsExcHandleEntry
    EXTERNAL g_excTbl
    EXTERNAL g_taskScheduled

    EQUAL OS_EXC_CAUSE_NMI               , 16
    EQUAL OS_EXC_CAUSE_HARDFAULT         , 17
    EQUAL HF_DEBUGEVT                    , 20
    EQUAL HF_VECTBL                      , 21
    EQUAL OS_EXC_FLAG_FAULTADDR_VALID    , 0x10000
    EQUAL OS_EXC_FLAG_IN_HWI             , 0x20000
    EQUAL OS_EXC_FLAG_NO_FLOAT           , 0x10000000
    EQUAL OS_NVIC_FSR                    , 0xE000ED28 /* include BusFault/MemFault/UsageFault State Register */
    EQUAL OS_NVIC_HFSR                   , 0xE000ED2C /* HardFault State Register */
    EQUAL OS_NVIC_BFAR                   , 0xE000ED38
    EQUAL OS_NVIC_MMAR                   , 0xE000ED34
    EQUAL OS_NVIC_ACT_BASE               , 0xE000E300
    EQUAL OS_NVIC_SHCSRS                 , 0xE000ED24
    EQUAL OS_NVIC_SHCSR_MASK             , 0xC00
    EQUAL EXCEPT_FRAME_OFFSET_PC         , 0x06 * 4 /* see cortex-m7 reference manual: chapter 2.3.7 */

    FUNC OsExcNMI
OsExcNMI:
    FNSTART
    CANTUNWIND
    MOV  R0, #OS_EXC_CAUSE_NMI
    MOV  R1, #0
    B    OsExcDispatch
    FNEND

    FUNC OsExcHardFault
OsExcHardFault:
    FNSTART
    CANTUNWIND
    MOV  R0, #OS_EXC_CAUSE_HARDFAULT
    LDR  R2, =OS_NVIC_HFSR
    LDR  R2, [R2]

    MOV  R1, #HF_DEBUGEVT
    ORR  R0, R0, R1, LSL #0x8
    TST  R2, #0x80000000
    BNE  OsExcDispatch /* DEBUGEVT */

    AND  R0, #0x000000FF
    MOV  R1, #HF_VECTBL
    ORR  R0, R0, R1, LSL #0x8
    TST  R2, #0x00000002
    BNE  OsExcDispatch /* VECTBL */

    /* if not DEBUGEVT and VECTBL then is FORCED */
    AND  R0, #0x000000FF

    MRS  R2, MSP
    LDR  R1, [R2, #EXCEPT_FRAME_OFFSET_PC]

    LDR  R2, =OS_NVIC_FSR
    LDR  R2, [R2]

    TST  R2, #0x8000 /* BFARVALID */
    BNE  HFBusFault /* BusFault */

    TST  R2, #0x80 /* MMARVALID */
    BNE  HFMemFault /* MemFault */

    MOV  R12,#0
    B    OsHFExcCommonBMU
    FNEND

HFBusFault:
    FNSTART
    CANTUNWIND
    LDR  R1, =OS_NVIC_BFAR
    LDR  R1, [R1]
    MOV  R12, #OS_EXC_FLAG_FAULTADDR_VALID
    B    OsHFExcCommonBMU
    FNEND

HFMemFault:
    FNSTART
    CANTUNWIND
    LDR  R1, =OS_NVIC_MMAR
    LDR  R1, [R1]
    MOV  R12, #OS_EXC_FLAG_FAULTADDR_VALID
    FNEND

    FUNC OsHFExcCommonBMU
OsHFExcCommonBMU:
    FNSTART
    CANTUNWIND
    CLZ  R2, R2
    LDR  R3, =g_excTbl
    ADD  R3, R3, R2
    LDRB R2, [R3]
    ORR  R0, R0, R2, LSL #0x8
    ORR  R0, R12
    B    OsExcDispatch
    FNEND

    FUNC OsExcSvcCall
OsExcSvcCall:
    FNSTART
    CANTUNWIND
    TST   LR, #0x4
    ITE   EQ
    MRSEQ R0, MSP
    MRSNE R0, PSP
#ifdef LOSCFG_TRUSTZONE
    PUSH  {LR}
    BL    NonSecureSvcHandler
    POP   {LR}
    BX    LR
#else
    LDR   R1, [R0,#24]      /* R1: PC */
    LDRB  R0, [R1,#-2]      /* r0: the SVC number */
    MOV   R1, #0
    B     OsExcDispatch
#endif
    FNEND

    FUNC OsExcBusFault
OsExcBusFault:
    FNSTART
    CANTUNWIND
    LDR  R0, =OS_NVIC_FSR
    LDR  R0, [R0]

    TST  R0, #0x8000 /* BFARVALID */
    BEQ  ExcBusNoAddr
    LDR  R1, =OS_NVIC_BFAR
    LDR  R1, [R1]
    MOV  R12, #OS_EXC_FLAG_FAULTADDR_VALID
    AND  R0, #0x1F00

    B    OsExcCommonBMU
    FNEND

ExcBusNoAddr:
    FNSTART
    CANTUNWIND
    MOV  R12,#0
    B    OsExcCommonBMU
    FNEND

    FUNC OsExcMemFault
OsExcMemFault:
    FNSTART
    CANTUNWIND
    LDR  R0, =OS_NVIC_FSR
    LDR  R0, [R0]

    TST  R0, #0x80 /* MMARVALID */
    BEQ  ExcMemNoAddr
    LDR  R1, =OS_NVIC_MMAR
    LDR  R1, [R1]
    MOV  R12, #OS_EXC_FLAG_FAULTADDR_VALID
    AND  R0, #0x1B

    B    OsExcCommonBMU
    FNEND

ExcMemNoAddr:
    FNSTART
    CANTUNWIND
    MOV  R12,#0
    B    OsExcCommonBMU
    FNEND

    FUNC OsExcUsageFault
OsExcUsageFault:
    LDR  R0, =OS_NVIC_FSR
    LDR  R0, [R0]

    LDR  R1, =0x030F
    LSL  R1, #16
    AND  R0, R1
    MOV  R12, #0

OsExcCommonBMU:
    CLZ  R0, R0
    LDR  R3, =g_excTbl
    ADD  R3, R3, R0
    LDRB R0, [R3]
    ORR  R0, R0, R12

/* R0 -- EXCCAUSE(bit 16 is 1 if EXCADDR valid),  R1 -- EXCADDR */
OsExcDispatch:
    LDR   R2, =OS_NVIC_ACT_BASE
    MOV   R12, #8 /* R12 is hwi check loop counter */

HwiActiveCheck:
    LDR   R3, [R2] /* R3 store active hwi register when exc */
    CMP   R3, #0
    BEQ   HwiActiveCheckNext

    /* exc occurred in IRQ */
    ORR   R0, #OS_EXC_FLAG_IN_HWI
    RBIT  R2, R3
    CLZ   R2, R2
    AND   R12, #1
    ADD   R2, R2, R12, LSL #5 /* calculate R2 (hwi number) as pid, third parameter */

ExcInMSP:
    CMP   LR, #0xFFFFFFED
    BNE   NoFloatInMsp
    ADD   R3, R13, #104
    PUSH  {R3}
    MRS   R12, PRIMASK /* store message-->exc: disable int */
    PUSH  {R4-R12} /* store message-->exc: {R4-R12} */
    VPUSH {D8-D15}
    B     HandleEntry

NoFloatInMsp:
    ADD   R3, R13, #32
    PUSH  {R3} /* save IRQ SP, store message-->exc: MSP(R13) */

    MRS   R12, PRIMASK /* store message-->exc: disable int? */
    PUSH  {R4-R12} /* store message-->exc: {R4-R12} */
    ORR   R0, R0, #OS_EXC_FLAG_NO_FLOAT
    B     HandleEntry

HwiActiveCheckNext:
    ADD   R2, #4 /* next NVIC ACT ADDR */
    SUBS  R12, #1
    BNE   HwiActiveCheck

    /* NMI interrupt excption */
    LDR   R2, =OS_NVIC_SHCSRS
    LDRH  R2,[R2]
    LDR   R3,=OS_NVIC_SHCSR_MASK
    AND   R2, R2,R3
    CMP   R2,#0
    BNE   ExcInMSP

    /* exc occurred in Task or Init or exc reserved for register info from task stack */
    LDR   R2, =g_taskScheduled
    LDR   R2, [R2]
    TST   R2, #1 /*os scheduled */
    BEQ   ExcInMSP /* if exc occurred in Init then branch */


    CMP   LR, #0xFFFFFFED /*auto push floating registers */
    BNE   NoFloatInPsp

    /* exc occurred in Task */
    MOV   R2,  R13
    SUB   R13, #96 /* add 8 Bytes reg(for STMFD) */

    MRS   R3,  PSP
    ADD   R12, R3, #104
    PUSH  {R12} /* save task SP */

    MRS   R12, PRIMASK
    PUSH  {R4-R12}
    VPUSH {D8-D15}

    /* copy auto saved task register */
    LDMFD   R3!, {R4-R11} /* R4-R11 store PSP reg(auto push when exc in task) */
    VLDMIA  R3!, {D8-D15}
    VSTMDB  R2!, {D8-D15}
    STMFD   R2!, {R4-R11}
    B       HandleEntry

NoFloatInPsp:
    MOV   R2,  R13 /*no auto push floating registers */
    SUB   R13, #32 /* add 8 Bytes reg(for STMFD) */

    MRS   R3,  PSP
    ADD   R12, R3, #32
    PUSH  {R12} /* save task SP */

    MRS   R12, PRIMASK
    PUSH  {R4-R12}

    LDMFD R3, {R4-R11} /* R4-R11 store PSP reg(auto push when exc in task) */
    STMFD R2!, {R4-R11}
    ORR   R0, R0, #OS_EXC_FLAG_NO_FLOAT

HandleEntry:
    MOV   R3, R13 /* R13:the 4th param */
    CPSID I
    CPSID F
    B     OsExcHandleEntry

    NOP

#ifdef LOSCFG_TRUSTZONE
#define SVC_ALLOC_SECURE_CONTEXT    0
#define SVC_FREE_SECURE_CONTEXT     1
#define SVC_INIT_SECURE_CONTEXT     2

    FUNC SvcAllocSecContextAsm
/* R0: alloc size , R1: self alloc */
SvcAllocSecContextAsm:
    FNSTART
    CANTUNWIND
    SVC     SVC_ALLOC_SECURE_CONTEXT
    BX      LR
    FNEND

    FUNC SvcFreeSecContextAsm
/* R0: secureContext */
SvcFreeSecContextAsm:
    FNSTART
    CANTUNWIND
    SVC     SVC_FREE_SECURE_CONTEXT
    BX      LR
    FNEND

    FUNC SvcInitSecContextAsm
SvcInitSecContextAsm:
    FNSTART
    CANTUNWIND
    SVC     SVC_INIT_SECURE_CONTEXT
    BX      LR
    FNEND

#endif /* LOSCFG_TRUSTZONE */

#ifdef __IASMARM__
    END
#endif