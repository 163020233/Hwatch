/*----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: LiteOS Deactivation Patch Management Module Implementation
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

UINT32 OsLpDeactiveProc(UINT32 groupId, UINT32 interNo)
{
    UINT32 ret = LOS_OK;
    struct TagLpUnitInfo *patUnitInfo = NULL;
    UINT32 intSave;

    patUnitInfo = &g_lpUnitInfo[OS_LP_UNIT_INNER_NO_2_INDEX(interNo)];

    OS_LP_MCMUTEX_LOCK(OS_LP_LOCK, &g_lpInitGuard, intSave);

    if ((patUnitInfo->unitState == (UINT16)UNIT_STATE_ACTIVATED) ||
        (patUnitInfo->unitState == (UINT16)UNIT_STATE_DEACTIVATING)) {
        patUnitInfo->unitState = (UINT16)UNIT_STATE_DEACTIVATING;
    } else if (patUnitInfo->unitState == (UINT16)UNIT_STATE_DEACTIVATED) {
        OS_LP_MCMUTEX_UNLOCK(OS_LP_LOCK, &g_lpInitGuard, intSave);
        return LOS_OK;
    } else {
        OS_LP_MCMUTEX_UNLOCK(OS_LP_LOCK, &g_lpInitGuard, intSave);
        return LOS_ERRNO_LP_DEACTIVE_FAIL;
    }

    OS_LP_MCMUTEX_UNLOCK(OS_LP_LOCK, &g_lpInitGuard, intSave);

    if (patUnitInfo->type == LP_TYPE_SP) {
        ret = OsLpDeactivePatch(groupId, interNo, patUnitInfo->type);
    }

    OS_LP_MCMUTEX_LOCK(OS_LP_LOCK, &g_lpInitGuard, intSave);

    if (ret != LOS_OK) {
        patUnitInfo->unitState = (UINT16)UNIT_STATE_ACTIVATED;
    } else {
        patUnitInfo->unitState = (UINT16)UNIT_STATE_DEACTIVATED;
    }

    OS_LP_MCMUTEX_UNLOCK(OS_LP_LOCK, &g_lpInitGuard, intSave);

    return ret;
}

UINT32 OsLpDeactivePatch(UINT32 groupId, UINT32 interNo, enum LPRegType type)
{
    (VOID)type;
    return OsLpPrimaryReplacePat(groupId, interNo, false);
}

STATIC UINT32 OsLpUnitDeactive(UINT32 groupId, UINT32 interNo)
{
    UINT32 ret;

    if (groupId >= g_lpPubInfo.groupNum) {
        return LOS_ERRNO_LP_GROUP_ID_INVALID;
    }

    if (g_lpGroupInfo[groupId].flag != OS_LP_GROUP_INITED) {
        return LOS_ERRNO_LP_MANAGE_UNIT_NO_INIT;
    }

    ret = OsLpDeactiveProc(groupId, interNo);
    if (ret != LOS_OK) {
        return ret;
    }

    PRINT_INFO("Patch deactivate unit OK, unitNo:[%u]\n", g_lpUnitInfo[OS_LP_UNIT_INNER_NO_2_INDEX(interNo)].patchNo);

    return ret;
}

UINT32 LOS_LpUnitDeactive(UINT32 unitNo)
{
    UINT32 ret;
    UINT32 interNo;
    UINT16 unitState;
    UINT32 groupId = g_kernelLpGroupId;

    PRINT_INFO("Patch deactivate unit start, unitNo:[%u]\n", unitNo);
    ret = OsLpInnerUnitNoGet(groupId, unitNo, &interNo);
    if (ret != LOS_OK) {
        goto HANDLE;
    }

    unitState = OsLpGetStateByPatchNo(interNo);
    if (unitState == (UINT16)UNIT_STATE_IDLE) {
        ret = LOS_ERRNO_LP_NOT_LOADED;
        goto HANDLE;
    } else if (unitState == (UINT16)UNIT_STATE_DEACTIVATED) {
        ret = LOS_ERRNO_LP_IS_DEACTIVE;
        goto HANDLE;
    }

    ret = OsLpUnitDeactive(groupId, interNo);
    if (ret != LOS_OK) {
        goto HANDLE;
    }

HANDLE:
    if (ret != LOS_OK) {
        PRINT_ERR("LOS_LPUnitDeactive failed! [%#x]\n", ret);
    }

    return ret;
}
