/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description : Task Low Level Implementations Headfile
 * Author: Huawei LiteOS Team
 * Create : 2022-07-02
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
 * ---------------------------------------------------------------------------- */

/**
 * @defgroup los_task Task
 * @ingroup kernel
 */

#ifndef ARCH_TASK_H
#define ARCH_TASK_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern VOID *g_newTask;
extern VOID *g_runningTask;

#define LOSCFG_STACK_POINT_ALIGN_SIZE 16U

#define TP_INIT_VALUE  0x02020202U
#define SP_INIT_VALUE  0x03030303U
#define S11_INIT_VALUE 0x04040404U
#define S10_INIT_VALUE 0x05050505U
#define S9_INIT_VALUE  0x06060606U
#define S8_INIT_VALUE  0x07070707U
#define S7_INIT_VALUE  0x08080808U
#define S6_INIT_VALUE  0x09090909U
#define S5_INIT_VALUE  0x10101010U
#define S4_INIT_VALUE  0x11111111U
#define S3_INIT_VALUE  0x12121212U
#define S2_INIT_VALUE  0x13131313U
#define S1_INIT_VALUE  0x14141414U
#define FP_INIT_VALUE  0x15151515U
#define T6_INIT_VALUE  0x16161616U
#define T5_INIT_VALUE  0x17171717U
#define T4_INIT_VALUE  0x18181818U
#define T3_INIT_VALUE  0x19191919U
#define A7_INIT_VALUE  0x20202020U
#define A6_INIT_VALUE  0x21212121U
#define A5_INIT_VALUE  0x22222222U
#define A4_INIT_VALUE  0x23232323U
#define A3_INIT_VALUE  0x24242424U
#define A2_INIT_VALUE  0x25252525U
#define A1_INIT_VALUE  0x26262626U
#define A0_INIT_VALUE  0x27272727U
#define T2_INIT_VALUE  0x28282828U
#define T1_INIT_VALUE  0x29292929U
#define T0_INIT_VALUE  0x30303030U

/**
 * @ingroup los_hw
 * Define the type of a task context control block.
 */
typedef struct {
    UINT32 mstatus;
    UINT32 mepc;
    UINT32 mcause;
    UINT32 sp;    /* X2 */

    UINT32 s11;   /* X27 */
    UINT32 s10;   /* X26 */
    UINT32 s9;    /* X25 */
    UINT32 s8;    /* X24 */
    UINT32 s7;    /* X23 */
    UINT32 s6;    /* X22 */
    UINT32 s5;    /* X21 */
    UINT32 s4;    /* X20 */
    UINT32 s3;    /* X19 */
    UINT32 s2;    /* X18 */

    UINT32 s1;    /* X9 */
    UINT32 s0;    /* X8 */

    UINT32 t6;    /* X31 */
    UINT32 t5;    /* X30 */
    UINT32 t4;    /* X29 */
    UINT32 t3;    /* X28 */

    UINT32 a7;    /* X17 */
    UINT32 a6;    /* X16 */
    UINT32 a5;    /* X15 */
    UINT32 a4;    /* X14 */
    UINT32 a3;    /* X13 */
    UINT32 a2;    /* X12 */
    UINT32 a1;    /* X11 */
    UINT32 a0;    /* X10 */

    UINT32 t2;    /* X7 */
    UINT32 t1;    /* X6 */
    UINT32 t0;    /* X5 */

    UINT32 ra;    /* X1 */
} TaskContext;

STATIC INLINE VOID *ArchCurrTaskGet(VOID)
{
    return g_newTask;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
