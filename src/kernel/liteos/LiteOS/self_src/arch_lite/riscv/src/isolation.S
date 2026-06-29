/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 * Description : LiteOS Kernel Isolation Module Implementation..
 * Author : Huawei LiteOS Team
 * Create : 2026-06-09
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
 */

#include "arch/stack_ops.h"

.extern g_isolation_swap_sp
.extern g_kernelCommonStack
.extern g_newTask_topOfStack
.extern g_osIsolationContext
.extern g_currentIsolationTask

.global SYS_ISO_CALL
.align 4
SYS_ISO_CALL:
    la      t0, g_isolation_swap_sp
    lw      t0, 0(t0)
    bnez    t0, run_func

    addi    sp, sp, -40
    sw      ra, 0(sp)
    sw      a0, 4(sp)
    sw      a1, 8(sp)
    sw      a2, 12(sp)
    sw      a3, 16(sp)
    sw      a4, 20(sp)
    sw      a5, 24(sp)
    sw      a6, 28(sp)
    sw      a7, 32(sp)
    sw      s0, 36(sp)

    la      t0, g_isolation_swap_sp
    sw      sp, 0(t0)
    
    la      a0, g_osIsolationContext
    jal     OsIsolationContextSave

    lw      t0, 4(sp)
    lw      a0, 8(sp)
    lw      a1, 12(sp)
    lw      a2, 16(sp)
    lw      a3, 20(sp)
    lw      a4, 24(sp)
    lw      a5, 28(sp)
    lw      a6, 32(sp)

#ifdef LOSCFG_HARDWARE_STACK_CHECK_ENABLE
    csrw    msplimit, zero
#endif
    /*切栈*/

    la      t1, g_commonTopOfStack
    lw      t1, 0(t1)
    mv      sp, t1

#ifdef LOSCFG_HARDWARE_STACK_CHECK_ENABLE
    SET_MSPLIMIT g_newTask_topOfStack, t0
#endif

    jalr    t0
    mv      s0,a0

    /*恢复权限*/
    la      a0, g_osIsolationContext
    jal     OsIsolationContextRestore

#ifdef LOSCFG_HARDWARE_STACK_CHECK_ENABLE
    csrw    msplimit, zero
#endif
    la      t0, g_isolation_swap_sp
    lw      sp, 0(t0)
#ifdef LOSCFG_HARDWARE_STACK_CHECK_ENABLE
    SET_MSPLIMIT g_newTask_topOfStack, t0
#endif
    mv      a0, s0 
    lw      ra, 0(sp)
    lw      s0, 36(sp)

    addi    sp,sp,40

    la      t0, g_isolation_swap_sp
    sw      x0, 0(t0)
    ret

run_func:
    addi    sp, sp, -4
    sw      ra, 0(sp)
    mv      t0,a0
    mv      a0,a1
    mv      a1,a2
    mv      a2,a3
    mv      a3,a4
    mv      a4,a5
    mv      a5,a6
    mv      a6,a7
    jalr    t0
    lw      ra, 0(sp)
    addi    sp, sp, 4
    ret