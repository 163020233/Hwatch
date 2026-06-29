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

#include "lp_group.h"

STATIC VOID OsLpErrHandle(struct TagLPGroupMagInfo *groupInfo)
{
    if (groupInfo->areaInfo != NULL) {
        (VOID)memset_s((VOID *)groupInfo->areaInfo, sizeof(struct HPAreaInfo) * groupInfo->areaNum, 0,
            sizeof(struct HPAreaInfo) * groupInfo->areaNum);
        OS_LP_MEM_FREE((VOID *)groupInfo->areaInfo);
    }

    (VOID)memset_s((VOID *)groupInfo, sizeof(*groupInfo), 0, sizeof(*groupInfo));
}

STATIC UINT32 OsLpSPAreaParaCheck(const struct HPAreaInfo *areaInfo)
{
    if ((areaInfo->codeAreaAddr == 0) || (areaInfo->codeAreaSize == 0)) {
        return LOS_ERRNO_LP_CFG_CODE_INVALID;
    }

    if ((areaInfo->dataShareAreaAddr == 0) || (areaInfo->dataShareAreaSize == 0)) {
        return LOS_ERRNO_LP_CFG_SDATA_INVALID;
    }

    return LOS_OK;
}

STATIC UINT32 OsLpSPAreaFlagTrueCheck(const struct HPAreaInfo *areaInfo)
{
    UINTPTR alignOffset;

    /* cacheline alignment */
    alignOffset = ((areaInfo->codeAreaAddr % OS_LP_AREA_ALGIN) || (areaInfo->codeAreaSize % OS_LP_AREA_ALGIN) ||
            (areaInfo->dataShareAreaAddr % OS_LP_AREA_ALGIN) || (areaInfo->dataShareAreaSize % OS_LP_AREA_ALGIN) ||
            (areaInfo->dataPriAreaAddr % OS_LP_AREA_ALGIN) || (areaInfo->dataAreaSize % OS_LP_AREA_ALGIN));
    if (alignOffset != 0) {
        return LOS_ERRNO_LP_REG_NOT_ALIGN;
    }
    return LOS_OK;
}

STATIC UINT32 OsLpSPAreaFlagCheck(const struct HPAreaInfo *areaInfo)
{
    UINT32 ret;

    if (areaInfo->areaIsShare == TRUE) {
        ret = OsLpSPAreaFlagTrueCheck(areaInfo);
        if (ret != LOS_OK) {
            return ret;
        }
    }
    return LOS_OK;
}

STATIC UINT32 OsLpSPAreaInnerCheck(struct HPAreaInfo *spAreaInfo, UINT32 areaNum, UINT32 groupId)
{
    UINT32 ret;
    UINT32 loop;
    struct HPAreaInfo *areaInfoAddr;

    for (loop = 0; loop < areaNum; loop++) {
        areaInfoAddr = &spAreaInfo[loop];

        ret = OsLpSPAreaParaCheck(areaInfoAddr);
        if (ret != LOS_OK) {
            return ret;
        }

        ret = OsLpSPAreaFlagCheck(areaInfoAddr);
        if (ret != LOS_OK) {
            return ret;
        }

        /* Overlap check */
        if (OS_LP_ADDR_START_END_IN_RANGE(areaInfoAddr->codeAreaAddr, areaInfoAddr->codeAreaSize,
            areaInfoAddr->dataShareAreaAddr, areaInfoAddr->dataShareAreaSize)) {
            return LOS_ERRNO_LP_SP_AREA_OVERLAY_IN_SAME_AREA;
        }
    }

    return LOS_OK;
}

STATIC UINT32 OsLpSectionCheck(struct HPAreaInfo *areaInfo, UINT32 groupId)
{
    UINT32 ret;
    struct TagLPGroupMagInfo *groupInfo = &g_lpGroupInfo[groupId];

    if (groupInfo->patchType == (UINT32)LP_TYPE_SP) {
        ret = OsLpSPAreaInnerCheck(areaInfo, groupInfo->areaNum, groupId);
        if (ret != LOS_OK) {
            return ret;
        }
    }
    return LOS_OK;
}

STATIC INLINE UINT32 OsLpGroupInfoFill(VOID)
{
    g_lpGroupInfo->groupId = 0;
    g_lpGroupInfo->patMaxFuncNum = OS_LP_SP_FUNC_NUM_IN_ONE_UNIT;
    g_lpGroupInfo->areaNum = 1;
    g_lpGroupInfo->patchType = (UINT32)LP_TYPE_SP;

    return LOS_OK;
}

STATIC UINT32 OsLpAllocPatAreaCtrlInfo(const struct HPAreaInfo *areaInfo,
                                       struct TagLPGroupMagInfo *groupMngInfo)
{
    UINT32 allocSize;
    UINT32 loop;
    struct HPAreaInfo *areaInfoAddr = NULL;

    allocSize = (UINT32)sizeof(struct HPAreaInfo) * (groupMngInfo->areaNum);
    areaInfoAddr = (struct HPAreaInfo *)OS_LP_MEM_ALLOC(allocSize);
    if (areaInfoAddr == NULL) {
        return LOS_ERRNO_LP_MEM_NOT_ENOUGH;
    }
    (VOID)memset_s(areaInfoAddr, allocSize, 0, allocSize);

    groupMngInfo->areaInfo = areaInfoAddr;
    for (loop = 0; loop < groupMngInfo->areaNum; loop++) {
        *areaInfoAddr = *areaInfo;
        areaInfoAddr++;
    }
    return LOS_OK;
}

UINT32 OsLpGroupInfoReg(UINT32 *groupId)
{
    UINT32 ret;
    UINT32 intSave;
    struct HPAreaInfo areaInfo = {0};

    OS_LP_MCMUTEX_LOCK(OS_LP_LOCK, &g_lpInitGuard, intSave);
    OsLpGroupInfoFill();

    areaInfo.areaIsShare = true;
    areaInfo.codeAreaAddr = OS_LP_SP_CODE_AREA_START;
    areaInfo.codeAreaSize = OS_LP_SP_CODE_AREA_LENGTH;
    areaInfo.dataShareAreaAddr = OS_LP_SP_DATA_AREA_START;
    areaInfo.dataShareAreaSize = OS_LP_SP_DATA_AREA_LENGTH;

    ret = OsLpSectionCheck(&areaInfo, 0);
    if (ret != LOS_OK) {
        goto ERROR;
    }

    ret = OsLpAllocPatAreaCtrlInfo(&areaInfo, g_lpGroupInfo);
    if (ret != LOS_OK) {
        goto ERROR;
    }

    g_lpGroupInfo->flag = OS_LP_GROUP_INITED;
    *groupId = g_lpGroupInfo->groupId;

    OS_LP_MCMUTEX_UNLOCK(OS_LP_LOCK, &g_lpInitGuard, intSave);
    return LOS_OK;
ERROR:
    OsLpErrHandle(g_lpGroupInfo);

    OS_LP_MCMUTEX_UNLOCK(OS_LP_LOCK, &g_lpInitGuard, intSave);
    return ret;
}
