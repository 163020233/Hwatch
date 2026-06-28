/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Basic definitions
 * Author: Huawei LiteOS Team
 * Create: 2022-07-01
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

#ifndef _HAL_IOMMU_H
#define _HAL_IOMMU_H

#include "los_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern UINT32 IommuCleanPageTable(DynShmConfig *userCfg, UINTPTR virAddr, UINT32 size, UINT32 isSec);
extern UINT32 IommuCopyPageTable(DynShmConfig *userCfg, UADDR outerVirAddr, UINTPTR innerVirAddr,
    UINT32 size, BOOL isSec);
extern UINT32 IommuVir2PhyAddr(PageTableConfig *pt, UADDR inAddr, UADDR *outAddr, UINT32 pageSize);

extern UINT32 IommuSetPtPmpRegion(SysPageTable *pt);
extern UINT32 IommuSetPmpRgnStart(UINT32 startAddr);
extern UINT32 IommuSetPmpRgnEnd(DynShmMemType type, UINT32 endAddr);

STATIC INLINE UINT32 IommuGetPteOffset(UADDR addr, UINT32 pageSize)
{
    const UINT32 shiftBit = (UINT32)(31 - CLZ(pageSize));  /* 31: 4K -> 12 bits */
    return (UINT32)((addr >> shiftBit) << 2);    /* 12 2: bits */
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _HAL_IOMMU_H */
