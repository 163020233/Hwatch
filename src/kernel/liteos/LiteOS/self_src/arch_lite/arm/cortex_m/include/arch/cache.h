/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Cache Operations HeadFile
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

#ifndef _ARCH_CACHE_H
#define _ARCH_CACHE_H

#include "los_typedef.h"

#if defined(LOSCFG_ARCH_CORTEX_M7) || defined(LOSCFG_ARCH_CORTEX_M55)
/* Data cache clean by address to the PoC */
#define CACHE_DCCMVAC   0xE000EF68U
/* Instruction cache invalidate all to the Point of Unification(PoU) */
#define CACHE_ICIALLU   0xE000EF50U
#define LINE_SIZE       32
#endif

/**
 * @ingroup cache
 * @brief flush DCache by start address and end address.
 *
 * @par Description:
 *  This API is used to flush DCache by Va.
 * @attention
 * <ul>
 * <li>The startAddr or endAddr address will be aligned to CACHE_LINE_SIZE if it's not aligned to CACHE_LINE_SIZE.</li>
 * <li>.</li>
 * </ul>
 * @param  startAddr     [IN] The start address need flush.
 * @param  endAddr       [IN] The end address need flush.
 * @par Dependency:
 * <ul><li>cache.h: the header file that contains the API declaration.</li></ul>
 * @since Huawei LiteOS 206.2.0
 */
VOID ArchDCacheFlushByAddr(UINTPTR startAddr, UINTPTR endAddr);
#define ArchDCacheCleanByAddr    ArchDCacheFlushByAddr     /* adapt for los_sec */

/**
 * @ingroup cache
 * @brief flush ICache.
 *
 * @par Description:
 * This API is used to flush ICache.
 * @attention
 * <ul>
 * <li>The API will flush all ICache, the size is 32K.</li>
 * <li>.</li>
 * </ul>
 * @par Dependency:
 * <ul><li>cache.h: the header file that contains the API declaration.</li></ul>
 * @since Huawei LiteOS 206.2.0
 */
VOID ArchICacheFlush(VOID);

#define flush_dcache(start, end) ArchDCacheFlushByAddr(start, end)
#define flush_icache() ArchICacheFlush()
#endif /* _ARCH_CACHE_H */
