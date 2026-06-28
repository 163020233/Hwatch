/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: LiteOS VPU implementation.
 * Author: Huawei LiteOS Team
 * Create: 2024-7-12
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

#include "arch/cpu.h"
#include "arch/stack_ops.h"
#include "arch/vector.h"

.global VpuSave
.global VpuRestore

.section .itcm.text, "ax"

.global VpuProcess
/* a0 : LosTaskCB *newTask */
VpuProcess:
    /* turn off VPU access */
    csrsi   mstatus, LOS_MSTATUS_VS_OFF

    /* check "newTask" is a vpu task */
    lbu     a1, TASK_CB_VPU_FLAG_OFFSET(a0)
    andi    a1, a1, TASK_CB_VPU_FLAG_MASK
    beqz    a1, VpuProcessDone

    /* turn on VPU access */
    li      a1, LOS_MSTATUS_VS_ON
    csrs    mstatus, a1

    /* newTask == g_vpuOwner ? */
    la      a1, g_vpuOwner
    lw      a2, 0(a1)
    beq     a0, a2, VpuProcessDone

    /* g_vpuOwner == NULL ? */
    beqz    a2, VpuRestore

VpuSave:
    /*
     * a0 : newTask
     * a1 : &g_vpuOwner
     * SWP rs2, rs3, offset(rs1)  {rs3, rs2} Similar to {2t+1, 2t}
     */
    lw      t0, TASK_CB_KERNEL_SP(a2)
    addi    t0, t0, -VPU_CONTEXT_SIZE
    sw      t0, TASK_CB_KERNEL_SP(a2)

    csrrw    t1, CUSTOM_VSTART_0, x0 /* t0:x6 */
    csrrw    t2, CUSTOM_VSTART_1, x0 /* t1:x7 */
    csrr     t3, CSR_VTYPE           /*  */
    csrr     t4, CSR_VL
    csrrw    t5, CSR_VCSR, x0
    vsetivli x0, 0, 0

    swp     t1, t2, 0(t0)
    swp     t3, t4, 8(t0)
    sw      t5, 16(t0)

    addi    t0, t0, VPU_CONTEXT_V
    vs8r.v  v0, (t0)     /* store v0~v7 */
    addi    t0, t0, 128
    vs8r.v  v8, (t0)     /* store v8~v15 */
    addi    t0, t0, 128
    vs8r.v  v16, (t0)     /* store v16~v23 */
    addi    t0, t0, 128
    vs8r.v  v24, (t0)     /* store v24~v31 */

VpuRestore:
    /*
     * a0 : newTask
     * a1 : &g_vpuOwner
     * LWP    rd,  rd2, offset(rs1) {rd2, rd} Similar to {2t+1, 2t}
     */
    sw     a0, 0(a1)
    lw     t0, TASK_CB_KERNEL_SP(a0)

    /* update stack point */
    addi   t1, t0, VPU_CONTEXT_SIZE
    sw     t1, TASK_CB_KERNEL_SP(a0)

    mv          a1, t0
    addi        t0, t0, VPU_CONTEXT_V
    vl8re64.v   v0, (t0)    /* load v0~v7 */
    addi        t0, t0, 128
    vl8re64.v   v8, (t0)    /* load v8~v15 */
    addi        t0, t0, 128
    vl8re64.v   v16, (t0)    /* load v16~v23 */
    addi        t0, t0, 128
    vl8re64.v   v24, (t0)    /* load v24~v31 */

    lwp     t1, t2, 0(a1)    /* t1: vstart0, t2:vstart1 */
    lwp     t3, t4, 8(a1)    /* t3:vtype, t4:vl */
    lw      t5, 16(a1)       /* t5:vcsr */

    csrw    CUSTOM_VSTART_0, t1
    csrw    CUSTOM_VSTART_1, t2
    csrw    CSR_VCSR, t5
    vsetvl  x0, t4, t3

    /* update mstatus */
    lw     t1, VPU_CONTEXT_SIZE(a1)
    ori    t1, t1, LOS_MSTATUS_VS_ON
    sw     t1, VPU_CONTEXT_SIZE(a1)

VpuProcessDone:
    ret

.global CurVpuTaskCheck
/* retval [0: is not a vpu task, >0 : is vpu task] */
CurVpuTaskCheck:
    /* g_newTask must not be empty. */
    la     a0, g_newTask
    lw     a0, 0(a0)
    lw     a0, TASK_CB_VPU_FLAG_OFFSET(a0)
    andi   a0, a0, TASK_CB_VPU_FLAG_MASK
    ret