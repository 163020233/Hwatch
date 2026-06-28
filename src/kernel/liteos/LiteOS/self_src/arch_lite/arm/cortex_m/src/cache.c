/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: LiteOS cache module implementation.
 * Author: Huawei LiteOS Team
 * Create: 2022-02-25
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
 * ---------------------------------------------------------------------------- */

#include "los_hwi.h"

#if defined(LOSCFG_ARCH_CORTEX_M7) || defined(LOSCFG_ARCH_CORTEX_M55)
#define CACHE_REG32_WRITE(addr, val)     (*(volatile UINT32 *)(UINTPTR)(addr) = (val))
#endif

VOID ArchCleanDCacheByAddress(UINTPTR addr, UINT32 len)
{
#if defined(LOSCFG_ARCH_CORTEX_M7) || defined(LOSCFG_ARCH_CORTEX_M55)
    UINT32 linesize;
    UINT32 align;
    UINT32 start, end;
    UINT32 tmp;

    linesize = LINE_SIZE;
    align = linesize - 1;
    start = (UINT32)addr;
    end = (UINT32)addr + len;
    start = start & (~align);
    for (tmp = start; tmp < end; tmp = tmp + linesize) {
        CACHE_REG32_WRITE(CACHE_DCCMVAC, tmp);
    }
    DSB();
    ISB();
#else
    (VOID)addr;
    (VOID)len;
#endif
}

LITE_OS_SEC_TEXT VOID ArchDCacheFlushByAddr(UINTPTR startAddr, UINTPTR endAddr)
{
#if defined(LOSCFG_ARCH_CORTEX_M7) || defined(LOSCFG_ARCH_CORTEX_M55)
    UINT32 size;

    UINTPTR start = (UINT32)startAddr & ~(LINE_SIZE - 1);
    UINTPTR end = LOS_Align(endAddr, LINE_SIZE);
    size = end - start;

    ArchCleanDCacheByAddress(startAddr, size);
#else
    (VOID)startAddr;
    (VOID)endAddr;
#endif
}

LITE_OS_SEC_TEXT VOID ArchICacheFlush(VOID)
{
#if defined(LOSCFG_ARCH_CORTEX_M7) || defined(LOSCFG_ARCH_CORTEX_M55)
    DSB();
    ISB();
    CACHE_REG32_WRITE(CACHE_ICIALLU, 0UL);
    DSB();
    ISB();
#endif
}
