/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2020. All rights reserved.
 * Description:  Mem Hook Implementation
 * Author: Huawei LiteOS Team
 * Create: 2013-01-01
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
 */

#include "los_hook.h"
#include "los_memory.h"
#include "los_printf.h"

#define MAX_HOOK_ARGS LOS_HOOK_PARAM5

LOS_HookFunc g_los_hook[LOS_HOOK_MAX] = { 0 };

VOID LOS_HookReg(LOS_HOOK_TYPE hookId, LOS_HookFunc pfn)
{
    if (hookId >= LOS_HOOK_MAX) {
        return;
    }
    g_los_hook[hookId] = pfn;
}

INT32 LOS_HookExe(LOS_HOOK_TYPE hookId, int argc, ...)
{
    UINTPTR args[MAX_HOOK_ARGS] = { 0 };
    va_list ap;

    if (argc < LOS_HOOK_PARAM0 || argc > LOS_HOOK_PARAM5) {
        return LOS_NOK;
    }

    if (hookId >= LOS_HOOK_MAX || g_los_hook[hookId] == NULL) {
        return LOS_NOK;
    }

    if (argc > MAX_HOOK_ARGS) {
        return LOS_NOK;
    }

    va_start(ap, argc);
    for (int i = 0; i < argc; i++) {
        args[i] = va_arg(ap, UINTPTR);
    }
    va_end(ap);

    g_los_hook[hookId](args, argc);

    return LOS_OK;
}

static const OsMemPoolOps  *g_osMemPoolOps = NULL;

VOID LOS_MemPoolOpsReg(const OsMemPoolOps *ops)
{
    g_osMemPoolOps = ops;
}

VOID *LOS_MemPoolMemAllocHook(UINT32 poolID, uint32_t size)
{
    if (g_osMemPoolOps != NULL) {
        return g_osMemPoolOps->memPoolAlloc(poolID, size);
    }
    return NULL;
}

bool LOS_MemPoolMemFreeHook(UINT32 poolID, VOID *addr)
{
    if (g_osMemPoolOps != NULL) {
        return g_osMemPoolOps->memPoolFree(poolID, addr);
    }
    return false;
}