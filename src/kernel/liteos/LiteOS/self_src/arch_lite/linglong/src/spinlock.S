/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2022. All rights reserved.
 * Description: Linglong SpinLock Implementation
 * Author: Huawei LiteOS Team
 * Create: 2022-01-05
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

/* TODO: support low power */

.type ArchSpinLock, %function
.global ArchSpinLock
ArchSpinLock:
    mov.val.i   a2, 1
trylock:
    ld.a32.ex   a1, a0
    b.neqz      a1, trylock
    st.a32.ex   a2, a0
    b.cf        trylock
    ret

.type ArchSpinUnlock, %function
.global ArchSpinUnlock
ArchSpinUnlock:
    mov.val.i   a1, 0
    st.a32.i    a1, a0, 0
    ret

.type ArchSpinTrylock, %function
.global ArchSpinTrylock
ArchSpinTrylock:
    mov.val.i   a2, 1
    ld.a32.ex   a1, a0
    b.neqz      a1, 1f
    st.a32.ex   a2, a0
    mov.a.t     a1
    /* reverse result to 0 or 1 */
    not         a1, a1
    zero.ext.i  a1, a1, 1
1:
    mov         a0, a1
    ret
