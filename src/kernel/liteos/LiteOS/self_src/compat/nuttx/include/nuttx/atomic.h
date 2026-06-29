/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: LiteOS compat headfile for nuttx atomic
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

#ifndef __INCLUDE_NUTTX_ATOMIC_H
#define __INCLUDE_NUTTX_ATOMIC_H

#include "los_atomic.h"

#ifndef ATOMIC_T_DEFINED
typedef Atomic atomic_t;
#define ATOMIC_T_DEFINED
#endif

#define ATOMIC_INIT(x)                  (x)

#define atomic_read(v)                  LOS_AtomicRead(v)

#define atomic_set(v, i)                LOS_AtomicSet(v, i)

static inline int32_t atomic_fetch_add(atomic_t *obj, int32_t val)
{
    return (LOS_AtomicAdd(obj, val) - val);
}

static inline int32_t atomic_fetch_sub(atomic_t *obj, int32_t val)
{
    return (LOS_AtomicSub(obj, val) + val);
}

#endif /* __INCLUDE_NUTTX_ATOMIC_H */
