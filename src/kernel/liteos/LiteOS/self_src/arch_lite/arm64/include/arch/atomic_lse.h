/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: Aarch64 LSE Atomic HeadFile
 * Author: Huawei LiteOS Team
 * Create: 2023-05-25
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

#ifndef _ARCH_ATOMIC_LSE_H
#define _ARCH_ATOMIC_LSE_H

#include "los_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

STATIC INLINE INT32 ArchAtomicRead(const Atomic *v)
{
    return *(volatile INT32 *)v;
}

STATIC INLINE VOID ArchAtomicSet(Atomic *v, INT32 setVal)
{
    *(volatile INT32 *)v = setVal;
}

STATIC INLINE INT32 ArchAtomicAdd(Atomic *v, INT32 addVal)
{
    INT32 val;

    __asm__ __volatile__("ldadd %w2, %w0, %1\n"
                         "add   %w0, %w0, %w2\n"
                         : "=&r"(val), "+Q"(*v)
                         : "r"(addVal)
                         : "memory");
    return val;
}

STATIC INLINE INT32 ArchAtomicSub(Atomic *v, INT32 subVal)
{
    INT32 val;

    __asm__ __volatile__("neg   %w2, %w2\n"
                         "ldadd %w2, %w0, %1\n"
                         "add   %w0, %w0, %w2\n"
                         : "=&r"(val), "+Q"(*v)
                         : "r"(subVal)
                         : "memory");

    return val;
}

STATIC INLINE VOID ArchAtomicInc(Atomic *v)
{
    (VOID)ArchAtomicAdd(v, 1);
}

STATIC INLINE INT32 ArchAtomicIncRet(Atomic *v)
{
    return ArchAtomicAdd(v, 1);
}

STATIC INLINE VOID ArchAtomicDec(Atomic *v)
{
    (VOID)ArchAtomicAdd(v, -1);
}

STATIC INLINE INT32 ArchAtomicDecRet(Atomic *v)
{
    return ArchAtomicAdd(v, -1);
}

STATIC INLINE INT64 ArchAtomic64Read(const Atomic64 *v)
{
    return *(volatile INT64 *)v;
}

STATIC INLINE VOID ArchAtomic64Set(Atomic64 *v, INT64 setVal)
{
    *(volatile INT64 *)v = setVal;
}

STATIC INLINE INT64 ArchAtomic64Add(Atomic64 *v, INT64 addVal)
{
    INT64 val;

    __asm__ __volatile__("ldadd %2, %0, %1\n"
                         "add   %0, %0, %2\n"
                         : "=&r"(val), "+Q"(*v)
                         : "r"(addVal)
                         : "memory");
    return val;
}

STATIC INLINE INT64 ArchAtomic64Sub(Atomic64 *v, INT64 subVal)
{
    INT64 val;

    __asm__ __volatile__("neg   %2, %2\n"
                         "ldadd %2, %0, %1\n"
                         "add   %0, %0, %2\n"
                         : "=&r"(val), "+Q"(*v)
                         : "r"(subVal)
                         : "memory");

    return val;
}

STATIC INLINE VOID ArchAtomic64Inc(Atomic64 *v)
{
    (VOID)ArchAtomic64Add(v, 1);
}

STATIC INLINE INT64 ArchAtomic64IncRet(Atomic64 *v)
{
    return ArchAtomic64Add(v, 1);
}

STATIC INLINE VOID ArchAtomic64Dec(Atomic64 *v)
{
    (VOID)ArchAtomic64Add(v, -1);
}

STATIC INLINE INT64 ArchAtomic64DecRet(Atomic64 *v)
{
    return ArchAtomic64Add(v, -1);
}

STATIC INLINE INT32 ArchAtomicXchg32bits(Atomic *v, INT32 val)
{
    INT32 prevVal;

    __asm__ __volatile__("swp   %w2, %w0, %1\n"
                         : "=&r"(prevVal), "+Q"(*v)
                         : "r"(val)
                         : "memory");

    return prevVal;
}

STATIC INLINE INT64 ArchAtomicXchg64bits(Atomic64 *v, INT64 val)
{
    INT64 prevVal;

    __asm__ __volatile__("swp   %2, %0, %1\n"
                         : "=&r"(prevVal), "+Q"(*v)
                         : "r"(val)
                         : "memory");

    return prevVal;
}

STATIC INLINE BOOL ArchAtomicCmpXchg32bits(Atomic *v, INT32 val, INT32 oldVal)
{
    INT32 prevVal;

    __asm__ __volatile__("mov   %w0, %w2\n"
                         "cas   %w0, %w3, %1\n"
                         : "=&r"(prevVal), "+Q"(*v)
                         : "r"(oldVal), "r"(val)
                         : "memory");

    return prevVal != oldVal;
}

STATIC INLINE BOOL ArchAtomicCmpXchg64bits(Atomic64 *v, INT64 val, INT64 oldVal)
{
    INT64 prevVal;

    __asm__ __volatile__("mov   %0, %2\n"
                         "cas   %0, %3, %1\n"
                         : "=&r"(prevVal), "+Q"(*v)
                         : "r"(oldVal), "r"(val)
                         : "memory");

    return prevVal != oldVal;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ARCH_ATOMIC_LSE_H */
