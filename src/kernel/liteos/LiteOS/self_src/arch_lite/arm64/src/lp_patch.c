/*----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: LiteOS Patch Management Module Implementation
 * Author: Huawei LiteOS Team
 * Create: 2024-09-25
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
 *---------------------------------------------------------------------------*/

#include "arch/cache.h"

#define PAT_PLT_INS_CODE_LDR     0x5800004fU       /* ldr x15, pc+8 */
#define PAT_PLT_INS_CODE_BR      0xd61f01e0U       /* br x15 */

struct OsLpPatPlt {
    UINT32 ins0;
    UINT32 ins1;
    UINT64 jumpAddr;
};

STATIC VOID OsLpSelfModifyCodeClean(UINTPTR addr, size_t size)
{
    ArchDCacheFlushByAddr(addr, addr + size);
    ArchICacheFlushByAddr(addr, addr + size);
}

STATIC VOID OsLpInsModify(UINTPTR addr, const struct OsLpPatPlt *plt)
{
    *(struct OsLpPatPlt *)addr = *plt;
    OsLpSelfModifyCodeClean(addr, sizeof(struct OsLpPatPlt));
}

VOID OsLpPatFuncReplace(UINTPTR orgAddr, UINTPTR patAddr)
{
    struct OsLpPatPlt plt = {PAT_PLT_INS_CODE_LDR, PAT_PLT_INS_CODE_BR, patAddr};

    OsLpInsModify(orgAddr, &plt);
}

VOID OsLpPatFuncRollback(UINTPTR orgAddr, UINTPTR backup)
{
    OsLpInsModify(orgAddr, (struct OsLpPatPlt *)backup);
}
