/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2024. All rights reserved.
 * Description: linglong Atomic HeadFile
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
#ifndef _ARCH_ATOMIC_EXCL_H
#define _ARCH_ATOMIC_EXCL_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern SPIN_LOCK_S g_atomSpin;
#define ATOM_LOCK(intSave) do { LOS_SpinLockSave(&g_atomSpin, (intSave)); } while (0)
#define ATOM_UNLOCK(intSave) do { LOS_SpinUnlockRestore(&g_atomSpin, (intSave)); } while (0)

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
    UINT32 status;

    do {
        __asm__ __volatile__("ld.a32.ex %1, %2\n"
                             "add   %1, %1, %3\n"
                             "st.a32.ex %1, %2\n"
                             "mov.a.not.t %0"
                             : "=&r"(status), "=&r"(val)
                             : "r"(v), "r"(addVal)
                             : "cc");
    } while (__builtin_expect(status, 0));

    return val;
}

STATIC INLINE INT32 ArchAtomicSub(Atomic *v, INT32 subVal)
{
    INT32 val;
    UINT32 status;

    do {
        __asm__ __volatile__("ld.a32.ex %1, %2\n"
                             "sub   %1, %1, %3\n"
                             "st.a32.ex %1, %2\n"
                             "mov.a.not.t %0"
                             : "=&r"(status), "=&r"(val)
                             : "r"(v), "r"(subVal)
                             : "cc");
    } while (__builtin_expect(status, 0));

    return val;
}

STATIC INLINE INT32 ArchAtomicIncRet(Atomic *v)
{
    INT32 val;
    UINT32 status;

    do {
        __asm__ __volatile__("ld.a32.ex %1, %2\n"
                             "add.i  %1, %1, 1\n"
                             "st.a32.ex %1, %2\n"
                             "mov.a.not.t %0"
                             : "=&r"(status), "=&r"(val) // "+m"(*v) not supported yet
                             : "r"(v)
                             : "cc");
    } while (__builtin_expect(status, 0));

    return val;
}

STATIC INLINE VOID ArchAtomicInc(Atomic *v)
{
    (VOID)ArchAtomicIncRet(v);
}

STATIC INLINE INT32 ArchAtomicDecRet(Atomic *v)
{
    INT32 val;
    UINT32 status;

    do {
        __asm__ __volatile__("ld.a32.ex %1, %2\n"
                             "add.i %1, %1, -1\n"
                             "st.a32.ex %1, %2\n"
                             "mov.a.not.t %0"
                             : "=&r"(status), "=&r"(val) // "+m"(*v) not supported yet
                             : "r"(v)
                             : "cc");
    } while (__builtin_expect(status, 0));

    return val;
}

STATIC INLINE VOID ArchAtomicDec(Atomic *v)
{
    (VOID)ArchAtomicDecRet(v);
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
    UINT32 intSave;
    INT64 val;

    ATOM_LOCK(&intSave);
    *v += addVal;
    val = *v;
    ATOM_UNLOCK(intSave);

    return val;
}

STATIC INLINE INT64 ArchAtomic64Sub(Atomic64 *v, INT64 subVal)
{
    UINT32 intSave;
    INT64 val;

    ATOM_LOCK(&intSave);
    *v -= subVal;
    val = *v;
    ATOM_UNLOCK(intSave);

    return val;
}

STATIC INLINE VOID ArchAtomic64Inc(Atomic64 *v)
{
    UINT32 intSave;

    ATOM_LOCK(&intSave);
    *v += 1;
    ATOM_UNLOCK(intSave);
}

STATIC INLINE INT64 ArchAtomic64IncRet(Atomic64 *v)
{
    UINT32 intSave;
    INT64 val;

    ATOM_LOCK(&intSave);
    *v += 1;
    val = *v;
    ATOM_UNLOCK(intSave);

    return val;
}

STATIC INLINE VOID ArchAtomic64Dec(Atomic64 *v)
{
    UINT32 intSave;

    ATOM_LOCK(&intSave);
    *v -= 1;
    ATOM_UNLOCK(intSave);
}

STATIC INLINE INT64 ArchAtomic64DecRet(Atomic64 *v)
{
    UINT32 intSave;
    INT64 val;

    ATOM_LOCK(&intSave);
    *v -= 1;
    val = *v;
    ATOM_UNLOCK(intSave);

    return val;
}

STATIC INLINE INT32 ArchAtomicXchg32bits(Atomic *v, INT32 val)
{
    INT32 prevVal;
    UINT32 status;

    do {
        __asm__ __volatile__("ld.a32.ex   %0, %2\n"
                             "st.a32.ex   %3, %2\n"
                             "mov.a.not.t %1"
                             : "=&r"(prevVal), "=&r"(status) // "+m"(*v) not supported yet
                             : "r"(v), "r"(val)
                             : "cc");
    } while (__builtin_expect(status, 0));

    return prevVal;
}

STATIC INLINE INT64 ArchAtomicXchg64bits(Atomic64 *v, INT64 val)
{
    UINT32 intSave;
    INT64 prevVal;

    ATOM_LOCK(&intSave);
    prevVal = *v;
    *v = val;
    ATOM_UNLOCK(intSave);

    return prevVal;
}

STATIC INLINE BOOL ArchAtomicCmpXchg32bits(Atomic *v, INT32 val, INT32 oldVal)
{
    INT32 prevVal;
    UINT32 status;

    do {
        __asm__ __volatile__("ld.a32.ex %0, %2\n"
                             "mov.i %1, 0\n"
                             "tst.eq %0, %3\n"
                             "b.cf 1f\n"
                             "st.a32.ex %4, %2\n"
                             "mov.a.not.t %1\n"
                             "1:"
                             : "=&r"(prevVal), "=&r"(status) // "+m"(*v) not supported yet
                             : "r"(v), "r"(oldVal), "r"(val)
                             : "cc");
    } while (__builtin_expect(status != 0, 0));

    return (prevVal != oldVal);
}

STATIC INLINE BOOL ArchAtomicCmpXchg64bits(Atomic64 *v, INT64 val, INT64 oldVal)
{
    UINT32 intSave;
    INT64 prevVal;

    ATOM_LOCK(&intSave);
    prevVal = *v;
    if (prevVal == oldVal) {
        *v = val;
    }
    ATOM_UNLOCK(intSave);

    return (prevVal != oldVal);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ARCH_ATOMIC_EXCL_H */
