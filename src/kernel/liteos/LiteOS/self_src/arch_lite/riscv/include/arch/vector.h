/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: Riscv Vector HeadFile
 * Author: Huawei LiteOS Team
 * Create: 2024-07-12
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

#ifndef ARCH_VECTOR_OPS_H
#define ARCH_VECTOR_OPS_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef OS_MEM_ACCESS_BIT_WIDTH
#define OS_MEM_ACCESS_BIT_WIDTH      8U
#define OS_MEM_ACCESS_BIT_WIDTH_MASK 7U
#endif
#define OS_COPY_COUNT_PER_LOOP       128U

#if OS_MEM_ACCESS_BIT_WIDTH > OS_COPY_COUNT_PER_LOOP
#error "size align requirements are too high"
#endif

#define VECTOR_REG_NUM                  32
#define VLENB                           16

/* VpuContext */
#define VPU_CONTEXT_VSTART0     0
#define VPU_CONTEXT_VSTART1     (VPU_CONTEXT_VSTART0 + 0x4)
#define VPU_CONTEXT_VTYPE       (VPU_CONTEXT_VSTART1 + 0x4)
#define VPU_CONTEXT_VL          (VPU_CONTEXT_VTYPE + 0x4)
#define VPU_CONTEXT_VCSR        (VPU_CONTEXT_VL + 0x4)
#define VPU_CONTEXT_V           (VPU_CONTEXT_VCSR + 0x4)

#define VPU_CONTEXT_V_SIZE      (VECTOR_REG_NUM * VLENB)
#define VPU_CONTEXT_SIZE        (VPU_CONTEXT_V + VPU_CONTEXT_V_SIZE)

#ifdef __ASSEMBLER__
.macro  CHECK_ADDR_IS_ALIGN tmp, addr, label
    neg \tmp, \addr
    /* Get unaligned address num */
    andi \tmp, \tmp, OS_MEM_ACCESS_BIT_WIDTH_MASK
    /* unaligned num < OS_MEM_ACCESS_BIT_WIDTH ? */
    beqz \tmp, \label
.endm
#endif

#ifndef __ASSEMBLER__
typedef struct {
    UINT32 vstart0;
    UINT32 vstart1;
    UINT32 vtype;
    UINT32 vl;
    UINT32 vcsr;
    UINT8 v[VECTOR_REG_NUM][VLENB];
} VpuContext;

#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ARCH_VECTOR_OPS_H */
