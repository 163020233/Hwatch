/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Interrupt Operations HeadFile
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

#ifndef _ARCH_INTERRUPT_H
#define _ARCH_INTERRUPT_H

#include "los_typedef.h"
#include "arch/regs.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define IRQ_GEN_REGS_NUM    20

/* saved by software at bottom of irq stack */
typedef struct IrqContext {
    UINT32 sp;
#ifdef LOSCFG_ARCH_SOUP_ENABLE
    UINT32 sr;
    UINT32 sup_uspb;
    UINT32 sup_lspb;
#else
    UINT32 reserved[3];  /* 16 bytes aligned */
#endif
} IrqContext;

typedef struct IrqStack {
    /* saved by software */
    UINT32 genRegs[IRQ_GEN_REGS_NUM]; /* A0-A11, A16-A23 */

    /* automatically pushed when irq inserted */
    UINT32 pc;
    UINT32 sr;
    UINT32 isr2;
    UINT32 emaddr;
} IrqStack;

extern UINT32 ArchIntLock(VOID);
extern UINT32 ArchIntUnlock(VOID);
extern VOID ArchIntRestore(UINT32 intSave);

STATIC INLINE UINT32 ArchIntLocked(VOID)
{
    UINT32 intSave;

    __asm__ __volatile__("bextu.ssr.i %0, sr, 8, 2" :"=r" (intSave) : :"memory", "cc");

    return intSave;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ARCH_INTERRUPT_H */
