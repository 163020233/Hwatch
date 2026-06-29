/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: CPU Operations HeadFile
 * Author: Huawei LiteOS Team
 * Create: 2021-11-18
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

#ifndef _ARCH_CPU_H
#define _ARCH_CPU_H

#include "los_typedef.h"
#include "arch/regs.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SR_DIMF_POS         5
#define SR_DI_POS           8
#define SR_DNMI_POS         9
#define SR_BTBEN_POS        17
#define SR_SSOUP_POS        18

typedef struct {
    const UINT32 partNo;
    const CHAR *cpuName;
} CpuVendor;

extern const CHAR *ArchCpuInfo(VOID);

STATIC INLINE UINT32 ArchCurrCpuid(VOID)
{
#ifdef LOSCFG_KERNEL_SMP
    UINT32 coreid;
    __asm__ __volatile__(
        "bextu.ssr.i %0, crthid, 2, 6"
        : "=r"(coreid)
    );
    return coreid;
#else
    return 0;
#endif
}

STATIC INLINE UINT32 ArchSPGet(VOID)
{
    UINT32 sp;
    __asm__ __volatile__("mov.a.ssr %0, sp"
                         : "=r"(sp)
    );
    return sp;
}

STATIC INLINE UINT32 OsMainIDGet(VOID)
{
    return READ_REG(SCU_MP_IDS);
}

STATIC INLINE UINT64 ArchCpuCycleGet(VOID)
{
    UINT64 val;

    __asm__ __volatile__(
        "mcount a0:a1\n"
        "st.a64.i a0:a1, %0, 0"
        :
        : "r"(&val)
        : "memory", "a0", "a1"
    );

    return val;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ARCH_CPU_H */
