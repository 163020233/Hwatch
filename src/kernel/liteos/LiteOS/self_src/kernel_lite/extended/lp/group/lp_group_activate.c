/*----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: LiteOS Activation Patch Management Module Implementation
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

STATIC INLINE VOID OsLpActiveFunc(struct TagSPFuncInfo *func)
{
    OsLpPatFuncReplace(func->origFuncAddr, func->replaceFuncAddr);
}

VOID OsLpDeactiveFunc(struct TagSPFuncInfo *funTable)
{
    OsLpPatFuncRollback(funTable->origFuncAddr, (UINTPTR)funTable->origIns);
}

STATIC INLINE VOID OsLpPatBackupOnePatFunc(struct TagSPFuncInfo *funTable)
{
    UINT32 count;
    UINT32 *origOper = (UINT32 *)(UINTPTR)funTable->origIns;
    UINT32 *origAddr = (UINT32 *)(UINTPTR)funTable->origFuncAddr;

    for (count = 0; count < OS_LP_FUNC_MIN_LENGTH / OS_BYTES_PER_WORD; count++) {
        /* Back up the original commands. */
        *origOper = *origAddr;
        /* Back up new instructions */
        origOper++;
        origAddr++;
    }
}

STATIC INLINE VOID OsLpPatBackupInst(UINT32 interNo)
{
    UINT32 index;
    struct TagLpUnitInfo *info = &g_lpUnitInfo[OS_LP_UNIT_INNER_NO_2_INDEX(interNo)];
    struct TagSPFuncInfo *funTable = info->hpFuncs;
    UINT32 funcNum = info->funcNum;

    for (index = 0; index < funcNum; index++, funTable++) {
        OsLpPatBackupOnePatFunc(funTable);
    }
}

STATIC INLINE VOID OsLpReplaceOnePatchFun(struct TagSPFuncInfo *funTable)
{
    if (funTable->state == OS_LP_FUNC_UNPATCH) {
        OsLpActiveFunc(funTable);
        funTable->guard = OS_LP_INSTRUCTION_GUARD;
        funTable->state = OS_LP_FUNC_PATCHED;
    }
}

STATIC VOID OsLpReplacePatchFun(UINT32 interNo, BOOL active)
{
    UINT32 index;
    struct TagLpUnitInfo *info = &g_lpUnitInfo[OS_LP_UNIT_INNER_NO_2_INDEX(interNo)];
    struct TagSPFuncInfo *funTable = info->hpFuncs;
    UINT32 funcNum = info->funcNum;

    if (!active) {
        OsLpRollbackFun(interNo);
        return;
    }

    for (index = 0; index < funcNum; index++, funTable++) {
        OsLpReplaceOnePatchFun(funTable);
    }
}

STATIC UINT32 OsLpPatReplaceSyncApply(UINT32 interNo, BOOL active)
{
    UINT32 intSave;

    intSave = LOS_IntLock();
    OsLpReplacePatchFun(interNo, active);
    LOS_IntRestore(intSave);

    return LOS_OK;
}

UINT32 OsLpPrimaryReplacePat(UINT32 groupId, UINT32 interNo, BOOL active)
{
    (VOID)groupId;
    return OsLpPatReplaceSyncApply(interNo, active);
}

STATIC INLINE UINT32 OsLpUnitCheckAndUpdateStatus(UINT32 groupId, struct TagLpUnitInfo *patUnitInfo, UINT32 *doActive)
{
    *doActive = 0;
    if ((patUnitInfo->unitState == (UINT16)UNIT_STATE_DEACTIVATED) ||
        (patUnitInfo->unitState == (UINT16)UNIT_STATE_ACTIVATING)) {
        /* Update the patch status. */
        patUnitInfo->unitState = (UINT16)UNIT_STATE_ACTIVATING;
        *doActive = 1;
        return LOS_OK;
    }

    if (patUnitInfo->unitState == (UINT16)UNIT_STATE_ACTIVATED) {
        return LOS_OK;
    }

    return LOS_ERRNO_LP_ACTIVE_FAIL;
}

STATIC UINT32 OsLpUnitActive(UINT32 groupId, UINT32 interNo)
{
    UINT32 doActive = 0;
    UINT32 ret;
    UINT32 intSave;
    struct TagLpUnitInfo *patUnitInfo = &g_lpUnitInfo[OS_LP_UNIT_INNER_NO_2_INDEX(interNo)];

    OS_LP_MCMUTEX_LOCK(OS_LP_LOCK, &g_lpInitGuard, intSave);
    ret = OsLpUnitCheckAndUpdateStatus(groupId, patUnitInfo, &doActive);
    OS_LP_MCMUTEX_UNLOCK(OS_LP_LOCK, &g_lpInitGuard, intSave);
    if ((ret != LOS_OK) || (doActive != 1)) {
        return ret;
    }

    if (patUnitInfo->type == LP_TYPE_SP) {
        OsLpPatBackupInst(interNo);
        /* command replacement */
        ret = OsLpPrimaryReplacePat(groupId, interNo, true);
    }

    OS_LP_MCMUTEX_LOCK(OS_LP_LOCK, &g_lpInitGuard, intSave);

    if (ret != LOS_OK) {
        patUnitInfo->unitState = (UINT16)UNIT_STATE_DEACTIVATED;

        OS_LP_MCMUTEX_UNLOCK(OS_LP_LOCK, &g_lpInitGuard, intSave);
        return ret;
    }

    patUnitInfo->unitState = (UINT16)UNIT_STATE_ACTIVATED;

    OS_LP_MCMUTEX_UNLOCK(OS_LP_LOCK, &g_lpInitGuard, intSave);

    patUnitInfo->procFlag = (UINT16)UNIT_PATCH_LOAD_STATE;
    PRINT_INFO("Patch active unit OK, unitNo:[%u]\n", patUnitInfo->patchNo);

    return ret;
}

STATIC INLINE UINT32 OsLpUnitCheck(UINT32 groupId, UINT32 interNo)
{
    UINT16 unitState = g_lpUnitInfo[OS_LP_UNIT_INNER_NO_2_INDEX(interNo)].unitState;
    if (unitState == (UINT16)UNIT_STATE_IDLE) {
        return LOS_ERRNO_LP_NOT_LOADED;
    }

    if (unitState == (UINT16)UNIT_STATE_ACTIVATED) {
        return LOS_ERRNO_LP_IS_ACTIVE;
    }

    return LOS_OK;
}

UINT32 LOS_LpUnitActive(UINT32 unitNo)
{
    UINT32 ret;
    UINT32 interNo = 0;
    UINT32 groupId = g_kernelLpGroupId;

    PRINT_INFO("Patch active unit start, unitNo:[%u]\n", unitNo);

    ret = OsLpInnerUnitNoGet(groupId, unitNo, &interNo);
    if (ret != LOS_OK) {
        goto HANDLE;
    }

    ret = OsLpUnitCheck(groupId, interNo);
    if (ret != LOS_OK) {
        goto HANDLE;
    }

    ret = OsLpUnitActive(groupId, interNo);
    if (ret != LOS_OK) {
        goto HANDLE;
    }

HANDLE:
    if (ret != LOS_OK) {
        PRINT_ERR("LOS_LPUnitActive failed! [%#x]\n", ret);
    }

    return ret;
}