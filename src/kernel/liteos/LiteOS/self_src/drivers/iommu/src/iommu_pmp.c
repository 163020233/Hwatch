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

#include "arch/pmp.h"
#include "los_dynshm.h"

enum {
/*
 * Region Configuration:
 * NC -- NON-CACHABLE
 * C  -- CACHEABLE
 * NS -- NON-SECURE
 * S  -- SECURE
 */
    PMP_REGION_NS_PAGE_TABLE_START = PMP_REGION_MMU_START,
#ifdef LOSCFG_KERNEL_DYNSHM_SECURE_ENBALE
    PMP_REGION_S_PAGE_TABLE_START,
#endif
    PMP_REGION_IOMMU_NS_NC_START,
    PMP_REGION_IOMMU_NS_NC_END,
    PMP_REGION_IOMMU_NS_C_END,
    PMP_REGION_IOMMU_S_NC_END,
    PMP_REGION_IOMMU_S_C_END,
};

STATIC UINT32 PmpGetConfigFromType(DynShmMemType type, PMP_ENTRY_INFO *pmpEntry)
{
    UINT32 ret = LOS_OK;

    switch (type) {
        case LOS_VA_SPACE_NOSEC_NOCACHE:
            pmpEntry->memAttr = MEM_ATTR_NORM_NON_CA_BUF;
            pmpEntry->seCtl = SEC_CONTRLO_NOSECURE_MMU;
            pmpEntry->ucNumber = PMP_REGION_IOMMU_NS_NC_END;
            break;
        case LOS_VA_SPACE_NOSEC_CACHE:
            pmpEntry->memAttr = MEM_ATTR_WB_RW_ALLOC;
            pmpEntry->seCtl = SEC_CONTRLO_NOSECURE_MMU;
            pmpEntry->ucNumber = PMP_REGION_IOMMU_NS_C_END;
            break;
        case LOS_VA_SPACE_SEC_NOCACHE:
            pmpEntry->memAttr = MEM_ATTR_NORM_NON_CA_BUF;
            pmpEntry->seCtl = SEC_CONTRLO_SECURE_MMU;
            pmpEntry->ucNumber = PMP_REGION_IOMMU_S_NC_END;
            break;
        case LOS_VA_SPACE_SEC_CACHE:
            pmpEntry->memAttr = MEM_ATTR_WB_RW_ALLOC;
            pmpEntry->seCtl = SEC_CONTRLO_SECURE_MMU;
            pmpEntry->ucNumber = PMP_REGION_IOMMU_S_C_END;
            break;
        default:
            PRINT_ERR("%s type %d invalid\n", __FUNCTION__, type);
            ret = LOS_NOK;
            break;
    }

    if (ret == LOS_NOK) {
        return ret;
    }

    pmpEntry->ucAddressMatch = PMP_RGN_ADDR_MATCH_TOR;
    pmpEntry->accPermission.readAcc = E_MEM_RD_ACC_RD;
    pmpEntry->accPermission.writeAcc = E_MEM_WR_ACC_WR;
    pmpEntry->accPermission.excuteAcc = E_MEM_EX_ACC_NON_EX;
    pmpEntry->bLocked = FALSE;
    pmpEntry->uwRegionSize = 0;

    return ret;
}

UINT32 IommuSetPtPmpRegion(SysPageTable *pt)
{
    UINT32 ret;
    PMP_ENTRY_INFO pmpRegion = {0};

    pmpRegion.accPermission.readAcc = E_MEM_RD_ACC_RD;
    pmpRegion.accPermission.writeAcc = E_MEM_WR_ACC_NON_WR;
    pmpRegion.accPermission.excuteAcc = E_MEM_EX_ACC_NON_EX;
    pmpRegion.memAttr = MEM_ATTR_DEV_NON_BUF;
    pmpRegion.bLocked = FALSE;
    pmpRegion.ucAddressMatch = PMP_RGN_ADDR_MATCH_NAPOT;

    /* Configure nonsecure pagetable */
    pmpRegion.ucNumber = PMP_REGION_NS_PAGE_TABLE_START;
    pmpRegion.uwBaseAddress = pt->nonsec.baseAddr;
    pmpRegion.uwRegionSize = pt->nonsec.tableSize;
    pmpRegion.seCtl = SEC_CONTRLO_NOSECURE_NOMMU;

    PRINT_DEBUG("[%s %d] pmp[%u] mode:%u, baseAddress:%#x, size:%#x\n", __FUNCTION__, __LINE__,
        pmpRegion.ucNumber, pmpRegion.ucAddressMatch, pt->nonsec.baseAddr, pmpRegion.uwRegionSize);

    ret = ArchProtectionRegionSet(&pmpRegion);
    if (ret != LOS_OK) {
        PRINT_ERR("[%s %d] ret = 0x%x failed!!\n", __FUNCTION__, __LINE__, ret);
        return LOS_NOK;
    }

#ifdef LOSCFG_KERNEL_DYNSHM_SECURE_ENBALE
    /* Configure secure pagetable */
    pmpRegion.ucNumber = PMP_REGION_S_PAGE_TABLE_START;
    pmpRegion.uwBaseAddress = pt->sec.baseAddr;
    pmpRegion.uwRegionSize = pt->sec.tableSize;
    pmpRegion.seCtl = SEC_CONTRLO_SECURE_NOMMU;
    ret = ArchProtectionRegionSet(&pmpRegion);
    if (ret != LOS_OK) {
        PRINT_ERR("[%s %d] ret = 0x%x failed!!\n", __FUNCTION__, __LINE__, ret);
        return LOS_NOK;
    }
#endif

    return LOS_OK;
}

UINT32 IommuSetPmpRgnStart(UINT32 startAddr)
{
    UINT32 ret;
    PMP_ENTRY_INFO pmpRegion = { 0 };

    pmpRegion.ucNumber = PMP_REGION_IOMMU_NS_NC_START;
    pmpRegion.ucAddressMatch = PMP_RGN_ADDR_MATCH_OFF;
    pmpRegion.uwBaseAddress = startAddr;
    pmpRegion.bLocked = FALSE;

    PRINT_DEBUG("[%s %d] pmp[%u] mode:%u, baseAddress:%#x, size:%#x\n", __FUNCTION__, __LINE__,
        pmpRegion.ucNumber, pmpRegion.ucAddressMatch, startAddr, pmpRegion.uwRegionSize);
    ret = ArchProtectionRegionSet(&pmpRegion);
    if (ret != LOS_OK) {
        PRINT_ERR("[%s %d] ret = 0x%x failed!!\n", __FUNCTION__, __LINE__, ret);
        return LOS_NOK;
    }

    return LOS_OK;
}

UINT32 IommuSetPmpRgnEnd(DynShmMemType type, UINT32 endAddr)
{
    UINT32 ret;
    PMP_ENTRY_INFO pmpRegion = { 0 };

    ret = PmpGetConfigFromType(type, &pmpRegion);
    if (ret != LOS_OK) {
        PRINT_ERR("[%s %d] type %d, ret = 0x%x failed!!\n",
            __FUNCTION__, __LINE__, type, ret);
        return LOS_NOK;
    }

    pmpRegion.uwBaseAddress = endAddr;
    PRINT_DEBUG("[%s %d] pmp[%u] mode:%u, baseAddress:%#x, size:%#x\n", __FUNCTION__, __LINE__,
        pmpRegion.ucNumber, pmpRegion.ucAddressMatch, endAddr, pmpRegion.uwRegionSize);

    ret = ArchProtectionRegionSet(&pmpRegion);
    if (ret != LOS_OK) {
        PRINT_ERR("[%s %d] ret = 0x%x failed!!\n", __FUNCTION__, __LINE__, ret);
        return LOS_NOK;
    }

    return LOS_OK;
}
