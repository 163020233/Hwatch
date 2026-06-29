/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Custom IOMMU Plus PMP Implementation.
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

#include "los_dynshm.h"
#include "los_printf.h"
#include "securec.h"
#include "los_exc.h"
#include "hal_iommu.h"

#define IOMMU_PTE_VALID_MASK  1
#define IOMMU_PTE_IS_INVALID  0  /* bit0 = 0: invalid, bit0 = 1: valid */

STATIC INLINE BOOL IommuPteIsValid(UINTPTR pte)
{
    if (((*(UINT32 *)pte) & IOMMU_PTE_VALID_MASK) == IOMMU_PTE_IS_INVALID) {
        PRINT_DEBUG("Invalid pte, %#x:%#x\n", pte, (*(UINT32 *)pte));
        return FALSE;
    }
    return TRUE;
}

STATIC UADDR IommuDecodePte(UADDR addr, UADDR desc, UINT32 pageSize)
{
    UADDR pageBaseAddr, pageOffset;
    const UINT32 shiftBit = (UINT32)(31 - CLZ(pageSize)); /* 31: 4K -> 12 bits */

    /*
     * Page Table Entry Format:
     * |31               8|7                2|1        |0
     * |PA[35:12](24bits) |Buffer ID(6 bits) |Reserved |Page valid
     */
    pageBaseAddr = (desc >> 8) << shiftBit; /* 8, refer to comment above */
    pageOffset = SHM_ALIGN_OFFSET(addr, 1U << shiftBit);

    PRINT_DEBUG("[%s %d] pageBaseAddr=%#llx, offset=%#llx\n", __FUNCTION__, __LINE__,
        pageBaseAddr, pageOffset);
    return pageBaseAddr + pageOffset;
}

STATIC INLINE BOOL IommuAddrIsOverlapped(UADDR addr, UINT32 size)
{
    /* [__text_start, g_sys_mem_addr_end) is LiteOS's Memory Space */
    if ((addr >= g_sys_mem_addr_end) || ((addr + size) <= (UINTPTR)&__text_start)) {
        return FALSE;
    }
    return TRUE;
}

UINT32 IommuCopyPageTable(DynShmConfig *userCfg, UADDR outerVirAddr, UINTPTR innerVirAddr, UINT32 size, BOOL isSec)
{
    UADDR phyMemAddr;
    UINT32 innerTableBaseAddr, outerTableBaseAddr;
    UINT32 innerTableSize, outerTableSize;
    UINT32 innerPte, outerPte;
    UINT32 innerPteOffset, outerPteOffset;
    UINT32 leftPteSize, copyPteSize;
    INT32 ret;

    if (isSec) {
        innerTableBaseAddr = userCfg->innerPageTable.sec.baseAddr;
        innerTableSize = userCfg->innerPageTable.sec.tableSize;
        outerTableBaseAddr = userCfg->outerPageTable.sec.baseAddr;
        outerTableSize = userCfg->outerPageTable.sec.tableSize;
    } else {
        innerTableBaseAddr = userCfg->innerPageTable.nonsec.baseAddr;
        innerTableSize = userCfg->innerPageTable.nonsec.tableSize;
        outerTableBaseAddr = userCfg->outerPageTable.nonsec.baseAddr;
        outerTableSize = userCfg->outerPageTable.nonsec.tableSize;
    }

    innerPteOffset = IommuGetPteOffset((UADDR)innerVirAddr, userCfg->pageSize);
    outerPteOffset = IommuGetPteOffset(outerVirAddr, userCfg->pageSize);
    if ((innerPteOffset >= innerTableSize) || (outerPteOffset >= outerTableSize)) {
        PRINT_ERR("[%s, %d] InnerTableSize: %#x, InnerPteOffset: %#x, OuterTableSize: %#x, OuterPteOffset: %#x\n",
            __FUNCTION__, __LINE__, innerTableSize, innerPteOffset, outerTableSize, outerPteOffset);
        return LOS_NOK;
    }

    innerPte = innerPteOffset + innerTableBaseAddr;
    outerPte = outerPteOffset + outerTableBaseAddr;
    PRINT_INFO("[%s %d] outer ttbr:%#x, src %#x:%#x, inner ttbr:%#x, dst %#x:%#x\n",
        __FUNCTION__, __LINE__, outerTableBaseAddr, outerPte, *(UINT32 *)(UINTPTR)outerPte,
        innerTableBaseAddr, innerPte, *(UINT32 *)(UINTPTR)innerPte);

    if (IommuPteIsValid(innerPte) == TRUE || IommuPteIsValid(outerPte) == FALSE) {
        PRINT_ERR("[%s, %d] pte valid bit is wrong\n", __FUNCTION__, __LINE__);
        return LOS_NOK;
    }
    phyMemAddr = IommuDecodePte(outerVirAddr, (UADDR)*(UINT32 *)(UINTPTR)outerPte, userCfg->pageSize);
    if (IommuAddrIsOverlapped(phyMemAddr, size) == TRUE) {
        PRINT_ERR("[%s, %d] Overlapped with OS memory\n", __FUNCTION__, __LINE__);
        return LOS_NOK;
    }

    leftPteSize = innerTableBaseAddr + innerTableSize - innerPte;
    copyPteSize = IommuGetPteOffset((UADDR)size, userCfg->pageSize);
    ret = memcpy_s((VOID *)(UINTPTR)innerPte, leftPteSize, (VOID *)(UINTPTR)outerPte, copyPteSize);
    if (ret != EOK) {
        PRINT_ERR("[%s, %d] memcpy_s failed errno:%d, leftPteSize:%#x, copyPteSize:%#x\n", __FUNCTION__, __LINE__,
            ret, leftPteSize, copyPteSize);
        return LOS_NOK;
    }

    if (userCfg->flushTlbHandler != NULL) {
        userCfg->flushTlbHandler(FALSE);
    }

    PRINT_INFO("[%s %d] outer ttbr:%#x, src %#x:%#x, inner ttbr:%#x, dst %#x:%#x, size = %#x\n",
        __FUNCTION__, __LINE__, outerTableBaseAddr, outerPte, *(UINT32 *)(UINTPTR)outerPte,
        innerTableBaseAddr, innerPte, *(UINT32 *)(UINTPTR)innerPte, IommuGetPteOffset(size, userCfg->pageSize));

    return LOS_OK;
}

UINT32 IommuCleanPageTable(DynShmConfig *userCfg, UINTPTR virAddr, UINT32 size, UINT32 isSec)
{
    UINT32 tableBaseAddr, tableSize;
    UINT32 pte, pteOffset;
    UINT32 leftPteSize, clrPteSize;
    INT32 ret;

    if (isSec) {
        tableSize = userCfg->innerPageTable.sec.tableSize;
        tableBaseAddr = userCfg->innerPageTable.sec.baseAddr;
    } else {
        tableSize = userCfg->innerPageTable.nonsec.tableSize;
        tableBaseAddr = userCfg->innerPageTable.nonsec.baseAddr;
    }

    pteOffset = IommuGetPteOffset((UADDR)virAddr, userCfg->pageSize);
    if (pteOffset >= tableSize) {
        PRINT_ERR("[%s, %d] Invalid input addr\n", __FUNCTION__, __LINE__);
        return LOS_NOK;
    }

    pte = pteOffset + tableBaseAddr;

    PRINT_INFO("[%s %d] ttbr:%#x, clearing pte %#x:%#x to 0, size = %#x\n",
        __FUNCTION__, __LINE__, tableBaseAddr, pte, *(UINT32 *)(UINTPTR)pte,
        IommuGetPteOffset(size, userCfg->pageSize));

    leftPteSize = tableBaseAddr + tableSize - pte;
    clrPteSize = IommuGetPteOffset((UADDR)size, userCfg->pageSize);

    ret = memset_s((VOID *)(UINTPTR)(pte), leftPteSize, 0, clrPteSize);
    if (ret != EOK) {
        PRINT_ERR("[%s, %d] memcpy_s failed errno:%d, leftPteSize:%#x, clrPteSize:%#x\n", __FUNCTION__, __LINE__,
            ret, leftPteSize, clrPteSize);
        return LOS_NOK;
    }

    if (userCfg->flushTlbHandler != NULL) {
        userCfg->flushTlbHandler(TRUE);
    }
    PRINT_INFO("[%s %d] ttbr:%#x, pte %#x:%#x cleared to 0, size = %#x\n",
        __FUNCTION__, __LINE__, tableBaseAddr, pte, *(UINT32 *)(UINTPTR)pte,
        IommuGetPteOffset(size, userCfg->pageSize));

    return LOS_OK;
}

UINT32 IommuVir2PhyAddr(PageTableConfig *pt, UADDR inAddr, UADDR *outAddr, UINT32 pageSize)
{
    UADDR desc;
    UINT32 tableBaseAddr, tableSize, offset, pte;

    tableBaseAddr = pt->baseAddr;
    tableSize = pt->tableSize;

    offset = IommuGetPteOffset(inAddr, pageSize);
    if (offset >= tableSize) {
        return LOS_ERRNO_DYN_SHM_INPUT_INVALID;
    }
    pte = tableBaseAddr + offset;
    desc = (UADDR)*(UINT32 *)(UINTPTR)pte;

    PRINT_DEBUG("[%s %d] inAddr:%#llx, ttbr %#x, pte %#x:%#llx\n", __FUNCTION__, __LINE__,
        inAddr, tableBaseAddr, pte, desc);

    *outAddr = IommuDecodePte(inAddr, desc, pageSize);
    return LOS_OK;
}
