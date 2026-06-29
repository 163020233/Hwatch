/*----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: LiteOS Remove Patch Management Module Implementation
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

#include "los_exc.h"
#include "lp_group.h"

STATIC UINT32 OsLpUnitSecRollBack(UINT32 unitInterNo)
{
    UINTPTR intSave;
    struct TagLpUnitInfo *unitInfo = &g_lpUnitInfo[OS_LP_UNIT_INNER_NO_2_INDEX(unitInterNo)];

    OS_LP_MCMUTEX_LOCK(OS_LP_LOCK, NULL, intSave);

    if (unitInfo->hpFuncs != NULL) {
        (VOID)memset_s((VOID *)unitInfo->hpFuncs, sizeof(struct TagSPFuncInfo) * unitInfo->funcNum, 0,
            sizeof(struct TagSPFuncInfo) * unitInfo->funcNum);
        OS_LP_MEM_FREE(unitInfo->hpFuncs);
    }

    unitInfo->interNo = unitInterNo;
    unitInfo->unitState = (UINT16)UNIT_STATE_IDLE;
    unitInfo->resetState = (UINT16)UNIT_STATE_IDLE;

    OS_LP_MCMUTEX_UNLOCK(OS_LP_LOCK, NULL, intSave);

    return LOS_OK;
}

VOID OsLpRollbackFun(UINT32 patchNo)
{
    UINT32 loop;
    UINT32 funcs;
    struct TagSPFuncInfo *funTable = NULL;
    struct TagLpUnitInfo *patUnitInfo = &g_lpUnitInfo[patchNo - 1];

    funTable = patUnitInfo->hpFuncs;
    funcs = patUnitInfo->funcNum;

    for (loop = 0; loop < funcs; loop++, funTable++) {
        if (funTable->state == OS_LP_FUNC_PATCHED) {
            OsLpDeactiveFunc(funTable);
            funTable->state = OS_LP_FUNC_UNPATCH;
        }
    }
}

UINT32 OsLpRollback(UINT32 groupId, UINT32 interNo, BOOL needSyn)
{
    if (!needSyn) {
        return LOS_OK;
    }

    return OsLpDeactiveProc(groupId, interNo);
}

UINT32 OsLpUnitRemove(UINT32 groupId, UINT32 interNo)
{
    UINT32 ret;
    BOOL needSyn = FALSE;
    struct TagLpUnitInfo *patUnitInfo = NULL;

    patUnitInfo = &(g_lpUnitInfo[OS_LP_UNIT_INNER_NO_2_INDEX(interNo)]);
    needSyn = FALSE;

    if (patUnitInfo->unitState == (UINT16)UNIT_STATE_ACTIVATED) {
        needSyn = TRUE;
    }
    ret = OsLpRollback(groupId, interNo, needSyn);
    if (ret != LOS_OK) {
        return ret;
    }

    ret = OsLpUnitSecRollBack(interNo);
    if (ret != LOS_OK) {
        return ret;
    }

    OsLpDecLoadUnitNum();

    patUnitInfo->procFlag = (UINT16)UNIT_PATCH_UNLOAD_STATE;
    PRINT_INFO("Patch remove unit OK, unitNo:[%u]\n", patUnitInfo->patchNo);

    return LOS_OK;
}

STATIC BOOL OsLpUnitCheckIdleStatus(UINT32 groupId, UINT32 interNo)
{
    return (g_lpUnitInfo[OS_LP_UNIT_INNER_NO_2_INDEX(interNo)].unitState == (UINT16)UNIT_STATE_IDLE);
}

UINT32 LOS_LpUnitRemove(UINT32 unitNo)
{
    UINT32 ret;
    UINT32 interNo;
    UINT32 groupId = g_kernelLpGroupId;

    PRINT_INFO("Patch remove unit start, unitNo:[%u]\n", unitNo);

    /* Obtain the internal patch number. */
    ret = OsLpInnerUnitNoGet(groupId, unitNo, &interNo);
    if (ret != LOS_OK) {
        goto HANDLE;
    }

    if (OsLpUnitCheckIdleStatus(groupId, interNo)) {
        return LOS_OK;
    }

    ret = OsLpUnitRemove(groupId, interNo);
    if (ret != LOS_OK) {
        goto HANDLE;
    }

HANDLE:
    if (ret != LOS_OK) {
        PRINT_ERR("LOS_LPUnitRemove failed! [%#x]\n", ret);
    }

    return ret;
}
