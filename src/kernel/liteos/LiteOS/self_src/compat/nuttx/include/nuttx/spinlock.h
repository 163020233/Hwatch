/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: LiteOS compat headfile for nuttx spinlock
 * Author: Huawei LiteOS Team
 * Create: 2025-09-03
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

#ifndef __INCLUDE_NUTTX_SPINLOCK_H
#define __INCLUDE_NUTTX_SPINLOCK_H

#include "nuttx/spinlock_type.h"

#undef EXTERN
#if defined(__cplusplus)
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

#ifdef LOSCFG_KERNEL_SMP

static inline uint32_t spin_lock_irqsave_notrace(spinlock_t *lock)
{
    uint32_t flags;
    LOS_SpinLockSave(lock, &flags);
    return flags;
}

static inline void spin_unlock_irqrestore_notrace(spinlock_t *lock, uint32_t flags)
{
    LOS_SpinUnlockRestore(lock, flags);
}

#else

static inline uint32_t spin_lock_irqsave_notrace(spinlock_t *lock)
{
    (void)lock;
    uint32_t flags = LOS_IntLock();
    LOS_TaskLock();
    return flags;
}

static inline void spin_unlock_irqrestore_notrace(spinlock_t *lock, uint32_t flags)
{
    (void)lock;
    LOS_TaskUnlock();
    LOS_IntRestore(flags);
}
#endif

#undef EXTERN
#if defined(__cplusplus)
}
#endif

#endif /* __INCLUDE_NUTTX_SPINLOCK_H */
