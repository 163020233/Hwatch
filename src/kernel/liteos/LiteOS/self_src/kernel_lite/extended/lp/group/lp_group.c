/*----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: LiteOS Patch Status Management Module Implementation
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

#include "los_typedef.h"
#include "lp_group.h"

STATIC INLINE UINT32 OsLpUnitSecAllocSecCtrl(struct TagLpUnitInfo *unitInfo, UINT32 groupId, UINT32 unitNo,
                                             enum LPRegType type, UINT32 patFuncNum)
{
    UINT32 allocSize;
    struct TagSPFuncInfo *hpFuncs = NULL;

    if (type == LP_TYPE_SP) {
        allocSize = (UINT32)sizeof(struct TagSPFuncInfo) * patFuncNum;
        hpFuncs = (struct TagSPFuncInfo *)OS_LP_MEM_ALLOC(allocSize);
        if (hpFuncs == NULL) {
            return LOS_ERRNO_LP_MEM_NOT_ENOUGH;
        }
        (VOID)memset_s(hpFuncs, allocSize, 0, allocSize);
    }

    unitInfo->patchNo = unitNo;
    unitInfo->groupId = groupId;
    unitInfo->type = type;

    unitInfo->funcNum = patFuncNum;
    unitInfo->hpFuncs = hpFuncs;
    return LOS_OK;
}

UINT32 OsLpUnitSectionAlloc(UINT32 groupId, UINT32 unitNo, enum LPRegType type,
                            UINT32 patFuncNum, UINT32 *unitInterNo)
{
    UINT32 ret;
    UINT32 loop;
    UINTPTR intSave;
    struct TagLpUnitInfo *unitInfo = g_lpUnitInfo;

    OS_LP_MCMUTEX_LOCK(OS_LP_LOCK, NULL, intSave);
    for (loop = 0; loop < g_lpPubInfo.maxLPUnitNum; loop++) {
        if ((unitInfo->patchNo != 0) && (unitInfo->unitState != UNIT_STATE_IDLE)) {
            unitInfo++;
            continue;
        }
        ret = OsLpUnitSecAllocSecCtrl(unitInfo, groupId, unitNo, type, patFuncNum);
        if (ret != LOS_OK) {
            OS_LP_MCMUTEX_UNLOCK(OS_LP_LOCK, NULL, intSave);
            return ret;
        }
        OS_LP_MCMUTEX_UNLOCK(OS_LP_LOCK, NULL, intSave);

        *unitInterNo = unitInfo->interNo;

        return LOS_OK;
    }

    OS_LP_MCMUTEX_UNLOCK(OS_LP_LOCK, NULL, intSave);
    return LOS_ERRNO_LP_PATCH_INDEX_ERROR;
}

UINT32 OsLpInnerUnitNoGet(UINT32 groupId, UINT32 patchNo, UINT32 *unitInnerNo)
{
    UINT32 loop;
    struct TagLpUnitInfo *unitInfo = g_lpUnitInfo;

    if (groupId >= g_lpPubInfo.groupNum) {
        return LOS_ERRNO_LP_GROUP_ID_INVALID;
    }

    if (g_lpGroupInfo[groupId].flag != OS_LP_GROUP_INITED) {
        return LOS_ERRNO_LP_MANAGE_UNIT_NO_INIT;
    }

    if ((patchNo == 0) || (patchNo > OS_LP_MAX_UINT_NUM)) {
        return LOS_ERRNO_LP_NO_INVALID;
    }

    for (loop = 0; loop < g_lpPubInfo.maxLPUnitNum; loop++) {
        if ((groupId == unitInfo->groupId) && (patchNo == unitInfo->patchNo) &&
            (unitInfo->unitState != UNIT_STATE_IDLE)) {
            *unitInnerNo = unitInfo->interNo;
            return LOS_OK;
        }

        unitInfo++;
    }

    return LOS_ERRNO_LP_NOT_LOADED;
}

VOID OsLpMakeCrc(const UINT8 *buffer, UINT32 count, UINT16 *crc)
{
    UINT16 temp;
    INT32 index;

    /* Address converted to the cache */
    UINT16 *mcPatchCrcTable = (UINT16 *)((UINTPTR)g_mcPatchCrcTable);

    temp = *crc;

    index = (INT32)count;
    if (index < 0) {
        return;
    }

    while (index--) {
        temp = (UINT16)((temp << OS_LP_SHIFT_BYTE) ^ (mcPatchCrcTable[(temp >> OS_LP_SHIFT_BYTE) ^ *buffer++]));
    }

    *crc = temp;
    return;
}

UINT32 OsLpGetPatFuncInfo(const struct TagPatSPFileFormat *hpHdr, const UINT8 *buf,
                          struct TagLpUnitInfo *tmpPatInfo)
{
    UINT32 funcIdx;
    const UINT8 *bufOff = NULL;
    struct TagSPFuncInfo *tmpFunc = NULL;
    UINTPTR textStart = LP_TEXT_START;
    UINTPTR textEnd = LP_TEXT_END;

    tmpPatInfo->funcNum = hpHdr->funcs;

    tmpFunc = tmpPatInfo->hpFuncs;

    bufOff = (buf + sizeof(struct TagPatSPFileFormat));

    for (funcIdx = 0; funcIdx < hpHdr->funcs; funcIdx++, tmpFunc++) {
        tmpFunc->guard = OS_LP_INSTRUCTION_GUARD;
        tmpFunc->state = 0;
        tmpFunc->origFuncAddr = OsLpFileGetDword(bufOff, g_mcCpuType);
        tmpFunc->replaceFuncAddr = OsLpFileGetDword((bufOff + sizeof(tmpFunc->origFuncAddr)), g_mcCpuType);

        if ((tmpFunc->replaceFuncAddr < OS_LP_SP_CODE_AREA_START) ||
            (tmpFunc->replaceFuncAddr >= OS_LP_SP_CODE_AREA_START + OS_LP_SP_CODE_AREA_LENGTH)) {
            return LOS_ERRNO_LP_PAT_FUCN_ADDR_INVALID;
        }

        if ((tmpFunc->origFuncAddr < textStart) ||
            (tmpFunc->origFuncAddr >= textEnd)) {
            return LOS_ERRNO_LP_PAT_FUCN_ADDR_INVALID;
        }

        tmpFunc->replBytes = 0;
        bufOff += OS_LP_FUNC_SIZE;
    }
    return LOS_OK;
}

UINT16 OsLpGetStateByPatchNo(UINT32 interNo)
{
    struct TagLpUnitInfo *unitInfo = NULL;

    unitInfo = &g_lpUnitInfo[OS_LP_UNIT_INNER_NO_2_INDEX(interNo)];

    return unitInfo->unitState;
}

UINT16 OsLpFileGetShort(const UINT8 *base, enum TagLpCpuType type)
{
    UINT16 result;
    const UINT8 *ptr = NULL;

    ptr = base;

    if (type == OS_LP_BIG_ENDIAN) {
        result = (UINT16)(ptr[OS_LP_SECOND_BYTE] + (ptr[OS_LP_FIRST_BYTE] << OS_LP_SHIFT_BYTE));
    } else {
        result = (UINT16)(ptr[OS_LP_FIRST_BYTE] + (ptr[OS_LP_SECOND_BYTE] << OS_LP_SHIFT_BYTE));
    }

    return result;
}

UINT32 OsLpFileGetLong(const UINT8 *base, enum TagLpCpuType type)
{
    UINT32 result;
    const UINT8 *ptr = NULL;

    ptr = base;

    if (type == OS_LP_BIG_ENDIAN) {
        result = (UINT32)(ptr[OS_LP_FORTH_BYTE] + (ptr[OS_LP_THIRD_BYTE] << OS_LP_SHIFT_BYTE) +
                       (ptr[OS_LP_SECOND_BYTE] << OS_LP_SHIFT_HALF_WORD) +
                       (ptr[OS_LP_FIRST_BYTE] << OS_LP_SHIFT_THREE_BYTE));
    } else {
        result = (UINT32)(ptr[OS_LP_FIRST_BYTE] + (ptr[OS_LP_SECOND_BYTE] << OS_LP_SHIFT_BYTE) +
                       (ptr[OS_LP_THIRD_BYTE] << OS_LP_SHIFT_HALF_WORD) +
                       (ptr[OS_LP_FORTH_BYTE] << OS_LP_SHIFT_THREE_BYTE));
    }

    return result;
}

UINT64 OsLpFileGetDword(const UINT8 *base, enum TagLpCpuType type)
{
    UINT32 index;
    UINT32 byteCnt;
    const UINT8 *ptr = NULL;
    union DwordDef result;

    ptr = base;
    if (type == OS_LP_BIG_ENDIAN) {
        byteCnt = 0;
        for (index = (UINT32)sizeof(UINT64); index > 0; index--) {
            result.byteData[byteCnt] = ptr[index - 1];
            byteCnt++;
        }
    } else {
        for (index = 0; index < sizeof(UINT64); index++) {
            result.byteData[index] = ptr[index];
        }
    }
    return result.dwordata;
}

VOID OsLpGetLoadSectionLen(UINT32 groupId, size_t *codeSize, size_t *dataSize)
{
    UINT32 index;
    UINT32 loadCnt = 0;
    size_t codeLen = 0;
    size_t dataLen = 0;
    struct TagLpUnitInfo *unitInfo = g_lpUnitInfo;

    for (index = 0; index < g_lpPubInfo.maxLPUnitNum; index++, unitInfo++) {
        if (loadCnt == OsLpGetLoadUnitNum()) {
            break;
        }
        if ((unitInfo->patchNo == 0) || (unitInfo->groupId != groupId) ||
            (unitInfo->unitState == UNIT_STATE_IDLE)) {
            continue;
        }
        if ((unitInfo->unitState == (UINT16)UNIT_STATE_DEACTIVATED) ||
            (unitInfo->unitState == (UINT16)UNIT_STATE_ACTIVATED)) {
            codeLen += ALIGN(unitInfo->codeLen, PAGE_SIZE);
            dataLen += ALIGN(unitInfo->shareDataLength, PAGE_SIZE);
            loadCnt++;
        }
    }
    *codeSize = codeLen;
    *dataSize = dataLen;
}

UINT32 LOS_LpUnitStateGet(UINT32 unitNo, enum LpState *unitState)
{
    UINT32 ret;
    UINT32 interNo;
    enum LpState kernelUnitState;
    UINT32 groupId = g_kernelLpGroupId;
    struct TagLpUnitInfo *unitInfo = NULL;

    if (unitState == NULL) {
        return LOS_ERRNO_LP_INPUT_IS_NULL;
    }

    if (g_lpGroupInfo[groupId].flag != OS_LP_GROUP_INITED) {
        return LOS_ERRNO_LP_MANAGE_UNIT_NO_INIT;
    }

    ret = OsLpInnerUnitNoGet(groupId, unitNo, &interNo);
    if (ret != LOS_OK) {
        if (ret == LOS_ERRNO_LP_NOT_LOADED) {
            kernelUnitState = UNIT_STATE_IDLE;
            ret = LOS_OK;
            goto HANDLE;
        }
        return ret;
    }

    unitInfo = &g_lpUnitInfo[OS_LP_UNIT_INNER_NO_2_INDEX(interNo)];
    kernelUnitState = unitInfo->unitState;
HANDLE:
    *unitState = kernelUnitState;
    return ret;
}
