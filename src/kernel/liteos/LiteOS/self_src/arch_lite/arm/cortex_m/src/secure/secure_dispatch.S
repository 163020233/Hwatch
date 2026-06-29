/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Cortex-M Secure dispatch Implementation
 * Author: Huawei LiteOS Team
 * Create: 2022-01-07
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

.syntax unified
.arch armv8.1-m.main
.thumb

.section .text
.thumb

.type SecureLoadContextAsm, %function
.global SecureLoadContextAsm
/* R0 : SecureContext */
SecureLoadContextAsm:
    .fnstart
    .cantunwind
    ldmia   R0!, {R1, R2}   /* R1 = SecureContext.curStackPointer, R2 = SecureContext.stackLimit */
    MSR     PSPLIM, R2
    MSR     PSP, R1
    BX      LR
    .fnend


.type SecureSaveContextAsm, %function
.global SecureSaveContextAsm
/* R0 : SecureContext */
SecureSaveContextAsm:
    .fnstart
    .cantunwind
    MRS     R1, PSP
    STR     R1, [R0]        /* SecureContext.curStackPointer = PSP_S */
    MOVS    R1, #0
    MSR     PSPLIM, R1
    MSR     PSP, R1         /* no stack for thread mode until new task's context load. */
    BX      LR
    .fnend

.type ArchSecureIntLock, %function
.global ArchSecureIntLock
ArchSecureIntLock:
    .fnstart
    .cantunwind
    mrs     r0, PRIMASK
    cpsid   I
    bx      lr
    .fnend

.type ArchSecureIntUnlock, %function
.global ArchSecureIntUnlock
ArchSecureIntUnlock:
    .fnstart
    .cantunwind
    mrs     r0, PRIMASK
    cpsie   I
    bx      lr
    .fnend

.type ArchSecureIntRestore, %function
.global ArchSecureIntRestore
ArchSecureIntRestore:
    .fnstart
    .cantunwind
    msr     PRIMASK, r0
    bx      lr
    .fnend
