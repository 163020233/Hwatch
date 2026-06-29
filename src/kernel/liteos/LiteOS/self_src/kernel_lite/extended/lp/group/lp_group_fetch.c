/*----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: LiteOS Fetch Patch Management Module Implementation
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

#include "arch/cache.h"
#include "lp_group.h"

STATIC INLINE UINT32 OsLpUnitFetchOutParaCheck(const UINT8 *buf, UINT32 length)
{
    if (buf == NULL) {
        return LOS_ERRNO_LP_INPUT_IS_NULL;
    }

    if (((UINTPTR)buf & (OS_LP_BUF_ADDR_ALIGN - 1)) != 0) {
        return LOS_ERRNO_LP_BUF_ADDR_NOT_ALIGN;
    }

    if (length < sizeof(struct TagSPPatchPacketHeader)) {
        return LOS_ERRNO_LP_BUF_NOT_ENOUGH;
    }

    return LOS_OK;
}

STATIC INLINE UINT32 OsLpUnitFetchInParaCheck(UINT32 groupId)
{
    if (groupId >= g_lpPubInfo.groupNum) {
        return LOS_ERRNO_LP_GROUP_ID_INVALID;
    }

    if (g_lpGroupInfo[groupId].flag != OS_LP_GROUP_INITED) {
        return LOS_ERRNO_LP_MANAGE_UNIT_NO_INIT;
    }

    return LOS_OK;
}

STATIC INLINE UINT32 OsLpUnitFetchCheckPatMemArea(UINT32 groupId, const struct HPAreaInfo *patchAreaInfo,
                                                  UINT32 patUnitNo, UINTPTR codeAddr, UINTPTR shareDataAddr)
{
    size_t dataSize = 0;
    size_t codeSize = 0;
    if (patUnitNo == 1) {
        if ((codeAddr != patchAreaInfo->codeAreaAddr)) {
            return LOS_ERRNO_LP_CODE_ADDR_NOT_SUIT;
        }

        if (shareDataAddr != (patchAreaInfo->dataShareAreaAddr)) {
            return LOS_ERRNO_LP_DATA_ADDR_NOT_SUIT;
        }
    } else {
        OsLpGetLoadSectionLen(groupId, &codeSize, &dataSize);
        if (codeSize + patchAreaInfo->codeAreaAddr > codeAddr) {
            return LOS_ERRNO_LP_CODE_ADDR_NOT_SUIT;
        }

        if (dataSize + patchAreaInfo->dataShareAreaAddr > shareDataAddr) {
            return LOS_ERRNO_LP_DATA_ADDR_NOT_SUIT;
        }
    }
    return LOS_OK;
}

STATIC UINT32 OsLpFetchCheckAddr(UINT32 groupId, UINT32 patUnitNo,
                                 const struct HPAreaInfo *areaInf, UINT32 curPatAreaNo)
{
    UINT32 ret;
    UINTPTR codeAddr = areaInf->codeAreaAddr;
    UINTPTR shareDataAddr = areaInf->dataShareAreaAddr;
    size_t codeLen = areaInf->codeAreaSize;
    size_t shareDataLen = areaInf->dataShareAreaSize;
    struct HPAreaInfo *patchAreaInfo;

    patchAreaInfo = &g_lpGroupInfo[groupId].areaInfo[curPatAreaNo];
    ret = OsLpUnitFetchCheckPatMemArea(groupId, patchAreaInfo, patUnitNo, codeAddr, shareDataAddr);
    if (ret != LOS_OK) {
        return ret;
    }

    if ((codeAddr < patchAreaInfo->codeAreaAddr) ||
        ((codeAddr + codeLen) > (patchAreaInfo->codeAreaAddr + patchAreaInfo->codeAreaSize))) {
        return LOS_ERRNO_LP_CODE_OVER_FLOW;
    }

    if (shareDataAddr != 0) {
        if ((shareDataAddr < patchAreaInfo->dataShareAreaAddr) ||
            ((shareDataAddr + shareDataLen) > (patchAreaInfo->dataShareAreaAddr + patchAreaInfo->dataShareAreaSize))) {
            return LOS_ERRNO_LP_DATA_OVER_FLOW;
        }
    }
    return ret;
}

STATIC UINT32 OsLpFileFlagCheck(const UINT8 *hpFlags)
{
    if (strncmp(OS_LP_SP_FLAGS, (const CHAR *)hpFlags, OS_LP_SP_FLAGS_LENGTH) == 0) {
        return TRUE;
    }

    return FALSE;
}

STATIC VOID OsLpFileCopyBin(UINTPTR baseSrc, UINTPTR baseDest, size_t length)
{
    UINT32 count;
    UINT8 *src = NULL;
    UINT8 *dest = NULL;

    src = (UINT8 *)baseSrc;
    dest = (UINT8 *)baseDest;

    for (count = 0; count < length; count++) {
        *dest = *src;
        src++;
        dest++;
    }

    return;
}

STATIC INLINE UINT32 OsLpPatFetchCheckFile(UINT32 groupId, UINT8 *buf, UINT32 length,
                                           UINT32 patFuncNum, UINT32 patchNo)
{
    struct HPAreaInfo areaInf;
    struct TagPatSPFileFormat *hpHdr = NULL;
    struct TagLPGroupMagInfo *groupMngInfo = &g_lpGroupInfo[groupId];

    if (length < sizeof(struct TagPatSPFileFormat)) {
        return LOS_ERRNO_LP_BUF_NOT_ENOUGH;
    }

    hpHdr = (struct TagPatSPFileFormat *)(VOID *)buf;
    if (OsLpFileFlagCheck(hpHdr->hpFlag) != TRUE) {
        return LOS_ERRNO_LP_SP_FLAG_INVALID;
    }

    if ((patchNo < 1) || (patchNo > g_lpPubInfo.maxLPUnitNum)) {
        return LOS_ERRNO_LP_NO_INVALID;
    }

    if (patFuncNum > groupMngInfo->patMaxFuncNum) {
        return LOS_ERRNO_LP_FUNC_OVERFLOW;
    }
    /* Get Code and Data info */
    areaInf.codeAreaAddr = OsLpFileGetDword((UINT8 *)&hpHdr->codeAddr, g_mcCpuType);
    areaInf.codeAreaSize = OsLpFileGetDword((UINT8 *)&hpHdr->codeLength, g_mcCpuType);

    areaInf.dataShareAreaAddr = OsLpFileGetDword((UINT8 *)&hpHdr->shareDataAddr, g_mcCpuType);
    areaInf.dataShareAreaSize = OsLpFileGetDword((UINT8 *)&hpHdr->shareDataLength, g_mcCpuType);

    areaInf.dataPriAreaAddr = OsLpFileGetDword((UINT8 *)&hpHdr->dataAddr, g_mcCpuType);
    areaInf.dataAreaSize = OsLpFileGetDword((UINT8 *)&hpHdr->dataLength, g_mcCpuType);

    return OsLpFetchCheckAddr(groupId, patchNo, &areaInf, 0);
}

STATIC INLINE UINT32 OsLpPatGetFileInfo(struct TagLpUnitInfo *patUnitInfo,
                                        const UINT8 *buf, struct TagPatSPFileFormat *hpHdr)
{
    UINT32 ret;

    OsLpFileCopyBin((UINTPTR)hpHdr->hpFormatVer,
        (UINTPTR)patUnitInfo->hpFormatVer, sizeof(patUnitInfo->hpFormatVer));

    /* Get function info from patch file. */
    ret = OsLpGetPatFuncInfo(hpHdr, buf, patUnitInfo);
    if (ret != LOS_OK) {
        return ret;
    }

    /* Get Code and Data info */
    patUnitInfo->codeAddr = OsLpFileGetDword((UINT8 *)&hpHdr->codeAddr, g_mcCpuType);
    patUnitInfo->codeLen = OsLpFileGetDword((UINT8 *)&hpHdr->codeLength, g_mcCpuType);

    patUnitInfo->shareDataAddr = OsLpFileGetDword((UINT8 *)&hpHdr->shareDataAddr, g_mcCpuType);
    patUnitInfo->shareDataLength = OsLpFileGetDword((UINT8 *)&hpHdr->shareDataLength, g_mcCpuType);

    patUnitInfo->dataAddr = OsLpFileGetDword((UINT8 *)&hpHdr->dataAddr, g_mcCpuType);
    patUnitInfo->dataLen = OsLpFileGetDword((UINT8 *)&hpHdr->dataLength, g_mcCpuType);
    patUnitInfo->crc = OsLpFileGetShort((UINT8 *)&hpHdr->hpCrc, g_mcCpuType);
    /* Get make Date and Time */
    OsLpFileCopyBin((UINTPTR)&hpHdr->createData,
        (UINTPTR)&patUnitInfo->createDate, sizeof(patUnitInfo->createDate));
    patUnitInfo->createDate.year = OsLpFileGetShort((UINT8 *)&patUnitInfo->createDate.year, g_mcCpuType);

    OsLpFileCopyBin((UINTPTR)&hpHdr->createTime,
        (UINTPTR)&patUnitInfo->createTime, sizeof(patUnitInfo->createTime));
    patUnitInfo->patchNo = OsLpFileGetLong((UINT8 *)&hpHdr->patchNo, g_mcCpuType);
    return LOS_OK;
}

STATIC INLINE UINT32 OsLpPatCpyCodeArea(struct TagLpUnitInfo *patInfo,
                                        UINTPTR baseSrc, UINTPTR baseDest, size_t length)
{
    if (length == 0) {
        return LOS_OK;
    }

    OsLpFileCopyBin(baseSrc, baseDest, length);

    ArchDCacheFlushByAddr(baseDest, baseDest + length);
    ArchICacheFlushByAddr(baseDest, baseDest + length);
    return LOS_OK;
}

STATIC INLINE UINT32 OsLpPatCpyDataArea(struct TagLpUnitInfo *patInfo,
                                        UINTPTR baseSrc, UINTPTR baseDest, size_t length)
{
    if (length == 0) {
        return LOS_OK;
    }

    OsLpFileCopyBin(baseSrc, baseDest, length);

    ArchDCacheFlushByAddr(baseDest, baseDest + length);
    return LOS_OK;
}

STATIC INLINE UINT32 OsLpPatAreaFill(struct TagLpUnitInfo *tmpPatInfo, const UINT8 *buf)
{
    UINTPTR offset;
    UINT32 ret;

    offset = (UINTPTR)((UINTPTR)buf + sizeof(struct TagPatSPFileFormat) +
             (tmpPatInfo->dependCount * sizeof(UINTPTR)) +
             (tmpPatInfo->funcNum * OS_LP_FUNC_SIZE) + sizeof(tmpPatInfo->dependCount));

    ret = OsLpPatCpyCodeArea(tmpPatInfo, offset, tmpPatInfo->codeAddr, tmpPatInfo->codeLen);
    if (ret != LOS_OK) {
        return ret;
    }

    offset += tmpPatInfo->codeLen;
    /* Shared Data Segments */
    return OsLpPatCpyDataArea(tmpPatInfo, offset, tmpPatInfo->shareDataAddr, tmpPatInfo->shareDataLength);
}

STATIC UINT32 OsLpFetchStatPatch(UINT8 *buf, UINT32 interNo)
{
    UINT32 ret;
    struct TagPatSPFileFormat *hpHdr = (struct TagPatSPFileFormat *)(VOID *)buf;
    struct TagLpUnitInfo *patUnitInfo = &g_lpUnitInfo[OS_LP_UNIT_INNER_NO_2_INDEX(interNo)];

    ret = OsLpPatGetFileInfo(patUnitInfo, buf, hpHdr);
    if (ret != LOS_OK) {
        return ret;
    }

    return OsLpPatAreaFill(patUnitInfo, buf);
}

STATIC UINT32 OsLpCheckPatPacketCrc(UINT8 *buf, UINT32 bufLen)
{
    UINT32 indexEntryNo;
    UINT16 tempCrc;
    UINT32 indexPosOffset;
    UINT16 calCrc = 0;

    (VOID)bufLen;

    tempCrc = OsLpFileGetShort((UINT8 *)(buf + OS_LP_SP_PATCHFILE_CRC_OFFSET), g_mcCpuType);
    indexEntryNo = OsLpFileGetLong((UINT8 *)(buf + OS_LP_SP_ENTRY_NUM_OFFSET), g_mcCpuType);

    indexPosOffset = OsLpFileGetLong((UINT8 *)(buf + OS_LP_SP_PATCHUNIT_INDEX_POS_OFFSET), g_mcCpuType);

    /* Index table CRC */
    OsLpMakeCrc((buf + indexPosOffset), (sizeof(struct TagLpUnitIndex) * indexEntryNo), &calCrc);

    /* File header CRC */
    OsLpMakeCrc((buf + OS_LP_SP_PATCHFILE_FLAG_SIZE + OS_LP_SP_PATCHFILE_CRC_SIZE),
                (sizeof(struct TagSPPatchPacketHeader) - (OS_LP_SP_PATCHFILE_FLAG_SIZE + OS_LP_SP_PATCHFILE_CRC_SIZE)),
                &calCrc);

    if (tempCrc != calCrc) {
        return LOS_ERRNO_LP_PACKET_CRC_INVALID;
    }

    return LOS_OK;
}

STATIC VOID OsLpUpdateToDeactive(UINT32 interNo, enum LPRegType type)
{
    struct TagLpUnitInfo *patUnitInfo = NULL;

    patUnitInfo = &(g_lpUnitInfo[OS_LP_UNIT_INNER_NO_2_INDEX(interNo)]);
    patUnitInfo->type = type;
    patUnitInfo->unitState = (UINT16)UNIT_STATE_DEACTIVATED;
}

STATIC UINT32 OsLpPatIndepFetch(UINT32 groupId, UINT32 patchUnitNum, const UINT8 *buf, UINT32 unitIndexOffset)
{
    UINT32 ret;
    UINT32 unitOffset;
    UINT32 unitSize;
    UINT32 patchNo;
    UINT32 patFuncNum;
    UINT32 unitInterNo;
    struct TagLpUnitIndex *hpUnitIndex = NULL;
    struct TagPatSPFileFormat *hpUnitHdr = NULL;

    /* Independent patch package */
    if (patchUnitNum > 1) {
        return LOS_ERRNO_LP_UNIT_NUM_INVALID;
    }

    /* Load a patch unit. */
    hpUnitIndex = (struct TagLpUnitIndex *)(UINTPTR)(buf + unitIndexOffset);

    /* Patch unit offset */
    unitOffset = OsLpFileGetLong((UINT8 *)hpUnitIndex + OS_LP_INDEX_UNITOFFSET_OFFSET, g_mcCpuType);
    /* Size of the patch unit */
    unitSize = OsLpFileGetLong((UINT8 *)hpUnitIndex + OS_LP_INDEX_UNITSIZE_OFFSET, g_mcCpuType);

    hpUnitHdr = (struct TagPatSPFileFormat *)(UINTPTR)(buf + unitOffset);
    patchNo = OsLpFileGetLong((UINT8 *)&hpUnitHdr->patchNo, g_mcCpuType);
    patFuncNum = OsLpFileGetLong((UINT8 *)&hpUnitHdr->funcs, g_mcCpuType);

    ret = OsLpInnerUnitNoGet(groupId, patchNo, &unitInterNo);
    if (ret == LOS_OK) {
        return LOS_ERRNO_LP_LOADED;
    }

    ret = OsLpPatFetchCheckFile(groupId, (UINT8 *)hpUnitHdr, unitSize, patFuncNum, patchNo);
    if (ret != LOS_OK) {
        return ret;
    }

    ret = OsLpUnitSectionAlloc(groupId, patchNo, LP_TYPE_SP, patFuncNum, &unitInterNo);
    if (ret != LOS_OK) {
        return ret;
    }

    /* Patch loading unit */
    ret = OsLpFetchStatPatch((UINT8 *)hpUnitHdr, unitInterNo);
    if (ret != LOS_OK) {
        (VOID)OsLpUnitRemove(groupId, unitInterNo);
        PRINT_ERR("OsLpPatIndepFetch failed! [%#x]\n", ret);
        return ret;
    }

    OsLpUpdateToDeactive(unitInterNo, LP_TYPE_SP);

    return LOS_OK;
}

STATIC UINT32 OsLpSPFetch(UINT32 groupId, UINT8 *buf, UINT32 length)
{
    UINT32 ret;
    UINT32 bufLen;
    UINT32 unitIndexOffset;
    UINT32 patchUnitNum;
    UINT32 entryNum;

    if (length < sizeof(struct TagSPPatchPacketHeader)) {
        return LOS_ERRNO_LP_BUF_NOT_ENOUGH;
    }

    /* Patch file header information */
    unitIndexOffset = OsLpFileGetLong((UINT8 *)(buf + OS_LP_SP_PATCHUNIT_INDEX_POS_OFFSET), g_mcCpuType);
    patchUnitNum = OsLpFileGetLong((UINT8 *)(buf + OS_LP_SP_PATCHUNIT_NUM_OFFSET), g_mcCpuType);
    entryNum = OsLpFileGetLong((UINT8 *)(buf + OS_LP_SP_ENTRY_NUM_OFFSET), g_mcCpuType);

    bufLen = unitIndexOffset + (UINT32)sizeof(struct TagLpUnitIndex) * entryNum;
    if (bufLen != length) {
        return LOS_ERRNO_LP_FILE_IS_INVALID;
    }

    /* Check CRC */
    ret = OsLpCheckPatPacketCrc(buf, length);
    if (ret != LOS_OK) {
        return LOS_ERRNO_LP_PACKET_CRC_INVALID;
    }
    return OsLpPatIndepFetch(groupId, patchUnitNum, buf, unitIndexOffset);
}

UINT32 LOS_LpUnitFetch(const UINT8 *lpBuf, UINT32 length)
{
    UINT32 ret;
    struct TagSPPatchPacketHeader *hdr;
    UINT8 *buf;
    UINT32 groupId = g_kernelLpGroupId;

    PRINT_INFO("Patch fetch unit start\n");

    ret = OsLpUnitFetchOutParaCheck(lpBuf, length);
    if (ret != LOS_OK) {
        goto HANDLE;
    }

    ret = OsLpUnitFetchInParaCheck(groupId);
    if (ret != LOS_OK) {
        goto HANDLE;
    }

    buf = (UINT8 *)lpBuf;
    hdr = (struct TagSPPatchPacketHeader *)buf;
    if (strncmp((CHAR *)(hdr->ident), OS_LP_SP_PACKET_IDENT, OS_LP_IDENT_LENGTH) != LOS_OK) {
        ret = LOS_ERRNO_LP_IDENT_INVALID;
        goto HANDLE;
    }

    ret = OsLpSPFetch(groupId, buf, length);
    if (ret != LOS_OK) {
        goto HANDLE;
    }
    OsLpIncLoadUnitNum();

    PRINT_INFO("Patch fetch unit OK!\n");

HANDLE:
    if (ret != LOS_OK) {
        PRINT_ERR("LOS_LPUnitFetch failed! [%#x]\n", ret);
    }

    return ret;
}
