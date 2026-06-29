/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Cortex-M Secure Implementation
 * Author: Huawei LiteOS Team
 * Create: 2022-01-07
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

#include "arch/secure/non_secure_call.h"
#include "arch/secure/secure_heap.h"
#include "arch/secure/secure_pri.h"
#include "asm/platform.h"
#include "arch/cpu.h"

OS_NSC_ENTRY SecureContext *ArchSecureAllocContext(UINT32 size)
{
    SecureContext *secContext = NULL;
    UINT8 *secStackMem = NULL;

    secContext = (SecureContext *)ArchSecureMemAlloc(sizeof(SecureContext));
    if (secContext == NULL) {
        return NULL;
    }

    secStackMem = (UINT8 *)ArchSecureMemAlloc(size);
    if (secStackMem == NULL) {
        (VOID)ArchSecureMemFree(secContext);
        return NULL;
    }

    secContext->stackLimit      = secStackMem;
    secContext->stackTop        = secStackMem + size;
    secContext->curStackPointer = secContext->stackTop;
    return secContext;
}

OS_NSC_ENTRY UINT32 ArchSecureFreeContext(SecureContext *handle)
{
    UINT32 ret = LOS_NOK;

    if (handle == NULL) {
        return ret;
    }

    ret = ArchSecureMemFree(handle->stackLimit);
    if (ret != LOS_OK) {
        return ret;
    }
    ret = ArchSecureMemFree(handle);
    return ret;
}

OS_NSC_ENTRY VOID ArchSecureInitContext(VOID)
{
    /* use PSP and privileged */
    asm volatile("MOV     R0,         #0\n"
                 "MSR     PSPLIM,     R0\n"
                 "MSR     PSP,        R0\n"
                 "MOV     R0,         #2\n"
                 "MSR     CONTROL,    R0\n");
}

OS_NSC_ENTRY VOID ArchSecureSaveContext(SecureContext *secContext)
{
    UINT32 ipsrVal = ArchGetIpsr();
    /* Do nothing when the processor is in thread mode. */
    if (ipsrVal == 0) {
        return;
    }
    SecureSaveContextAsm((void *)secContext);
}

OS_NSC_ENTRY VOID ArchSecureLoadContext(SecureContext *secContext)
{
    UINT32 ipsrVal = ArchGetIpsr();
    /* Do nothing when the processor is in thread mode. */
    if (ipsrVal == 0) {
        return;
    }
    SecureLoadContextAsm((void *)secContext);
}
