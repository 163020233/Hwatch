/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: LiteOS Patch Management Module Private HeadFile
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
 * --------------------------------------------------------------------------- */

#ifndef LOS_LP_GROUP_H
#define LOS_LP_GROUP_H

#include "los_spinlock.h"
#include "lp_comm.h"

#ifndef INLINE
#define INLINE static __inline __attribute__((always_inline))
#endif

#if (LOSCFG_KERNEL_LP_MAX_UNIT_NUM > 1)
#error "Only full patches are supported!"
#endif

#define OS_LP_MAX_GROUP_NUM                   0x1
#define OS_LP_MAX_UNIT_NUM                    LOSCFG_KERNEL_LP_MAX_UNIT_NUM

#define OS_LP_MAG_INFO_TCB_MAGIC_LOW 0x6b43
#define OS_LP_MAG_INFO_TCB_MAGIC_HIGH 0x965a
#define OS_LP_MAG_TCB_MAGIC_HIGH_OFFSET 2

/* Group initialization flag */
#define OS_LP_GROUP_INITED 0x4c504750  // LPGP

#define PAGE_SIZE          0x1000U
#define OS_LP_AREA_ALGIN   PAGE_SIZE

#define OS_LP_SP_CODE_AREA_START                        STATIC_PATCH_AREA_BASE
#define OS_LP_SP_CODE_AREA_LENGTH                       STATIC_PATCH_CODE_AREA_SIZE
#define OS_LP_SP_DATA_AREA_START                        (STATIC_PATCH_AREA_BASE + STATIC_PATCH_CODE_AREA_SIZE)
#define OS_LP_SP_DATA_AREA_LENGTH                       STATIC_PATCH_DATA_AREA_SIZE

#ifndef OS_LP_SP_FUNC_NUM_IN_ONE_UNIT
#define OS_LP_SP_FUNC_NUM_IN_ONE_UNIT                   LOSCFG_KERNEL_LP_MAX_FUNC_NUM_PER_UNIT
#endif

#define OS_LP_TOOL_VER "TOOL LP V0.1"

#define OS_LP_MCMUTEX_LOCK(hwSemID, addr, intSave)       ((VOID)(intSave))
#define OS_LP_MCMUTEX_UNLOCK(hwSemID, addr, intSave)    ((VOID)(intSave))
  
#define OS_LP_LOCK 0

#define LP_TEXT_START         STATIC_PATCH_TEXT_START
#define LP_TEXT_END           STATIC_PATCH_TEXT_END

#define OS_LP_FUNC_MIN_LENGTH 16
struct TagSPFuncInfo {
    UINT32 guard;              /* protect the instruction */
    UINT32 state;              /* the state of function */
    UINT32 replBytes;
    UINTPTR origFuncAddr;      /* source function address */
    UINTPTR replaceFuncAddr;   /* patch function address */
    UINT32 funcLocation;
    UINT8 origIns[OS_LP_FUNC_MIN_LENGTH];
    UINT8 replaceIns[OS_LP_FUNC_MIN_LENGTH];
};

enum PatchUnitProcFlag {
    UNIT_PATCH_NOTPROC_STATE = 0x0,  /* Initial state, not processed by the patch unit */
    UNIT_PATCH_LOAD_STATE = 0x1,     /* Identifies the patch unit that has been loaded. */
    UNIT_PATCH_UNLOAD_STATE = 0x2,   /* Identifies the patch unit that has been uninstalled. */
};

/* Load&Patch Registration Type. */
enum LPRegType {
    LP_TYPE_NULL = 0,     /**< Invalid LP type. */
    LP_TYPE_SP = 1,       /**< Static patch type. */
    LP_TYPE_DL = 2,       /**< Dynamic loading type */
    LP_TYPE_DP = 4,       /**< Dynamic Patch Type */
    LP_TYPE_DP_DL = 6,    /**< Dynamic Patch and Dynamic Loading Type */
};

/* Patch unit control information */
struct TagLpUnitInfo {
    UINT32 groupId;
    UINT8 hpFormatVer[OS_LP_SP_FORMAT_VERSION_LEN];
    UINT16 crc;
    UINT16 procFlag;
    UINT32 patchNo;
    UINT32 interNo;
    enum LPRegType type;
    UINT16 unitState;
    UINT16 resetState;
    UINTPTR codeAddr;
    size_t codeLen;
    UINTPTR shareDataAddr;
    size_t shareDataLength;
    UINTPTR dataAddr;
    size_t dataLen;
    UINT32 funcNum;
    struct TagSPFuncInfo *hpFuncs;
    UINT32 dependCount;
    struct DateStru createDate;
    struct TimeStru createTime;
};

/* Group Management Structure Information */
struct TagLPGroupMagInfo {
    UINT32 flag;
    UINT32 patchType;
    UINT32 groupId;
    UINT32 patMaxFuncNum;
    UINT32 areaNum;
    struct HPAreaInfo *areaInfo;
};

union DwordDef {
    UINT64 dwordata;
    UINT8  byteData[sizeof(UINT64)];
};

/* patch area information. */
struct HPAreaInfo {
    BOOL areaIsShare;           /**< Whether the patch area is in the shared memory. */
    UINTPTR codeAreaAddr;       /**< Start address of the patch code area. */
    size_t codeAreaSize;        /**< Size of the patch code area. */
    UINTPTR dataShareAreaAddr;  /**< Start address of the patch shared data area */
    size_t dataShareAreaSize;   /**< Size of the patch shared data area */
    UINTPTR dataPriAreaAddr;    /**< Start address of the patch private data area */
    size_t dataAreaSize;        /**< Size of the patch data area */
};

/* Unified configuration of static patch mirroring. */
struct LpImagePubInfo {
    UINT32 groupNum;         /**< Maximum number of LP management information groups */
    UINT32 maxLPUnitNum;     /**< Maximum number of units */
};

/*
 * Global variable declaration in the module
 */
extern struct TagLpUnitInfo *g_lpUnitInfo;
extern struct TagLPGroupMagInfo *g_lpGroupInfo;
extern volatile UINT32 g_lpPubInitPhase;
extern volatile UINT32 g_lpInitGuard;
extern UINT32 g_kernelLpGroupId;
extern UINT32 g_lpLoadUnitNum;

/* ************************interior function declare ************************** */
extern VOID OsLpRollbackFun(UINT32 patchNo);
extern UINT32 OsLpRollback(UINT32 groupId, UINT32 interNo, BOOL needSyn);
extern UINT32 OsLpDeactiveProc(UINT32 groupId, UINT32 interNo);
extern UINT32 OsLpReplacePatch(UINT32 groupId, UINT32 interNo, enum LPRegType type);
extern UINT32 OsLpDeactivePatch(UINT32 groupId, UINT32 interNo, enum LPRegType type);

extern VOID OsLpDeactiveFunc(struct TagSPFuncInfo *funTable);
extern UINT32 OsLpUnitRemove(UINT32 groupId, UINT32 interNo);
extern UINT32 OsLpGetPatFuncInfo(const struct TagPatSPFileFormat *hpHdr,
    const UINT8 *buf, struct TagLpUnitInfo *tmpPatInfo);
extern UINT16 OsLpGetStateByPatchNo(UINT32 interNo);
extern UINT32 OsLpInnerUnitNoGet(UINT32 groupId, UINT32 patchNo, UINT32 *unitInnerNo);
extern UINT32 OsLpFileGetLong(const UINT8 *base, enum TagLpCpuType type);
extern UINT16 OsLpFileGetShort(const UINT8 *base, enum TagLpCpuType type);
extern UINT64 OsLpFileGetDword(const UINT8 *base, enum TagLpCpuType type);

extern VOID OsLpMakeCrc(const UINT8 *buffer, UINT32 count, UINT16 *crc);
extern UINT32 OsLpUnitSectionAlloc(UINT32 groupId, UINT32 unitNo,
    enum LPRegType type, UINT32 patFuncNum, UINT32 *unitInterNo);
extern VOID OsLpGetLoadSectionLen(UINT32 groupId, size_t *codeSize, size_t *dataSize);
extern UINT32 OsLpGroupInfoReg(UINT32 *groupId);
extern UINT32 OsLpPrimaryReplacePat(UINT32 groupId, UINT32 interNo, BOOL active);
extern UINT32 OsLpConfigInit(VOID);

#define OS_BYTES_PER_WORD   (sizeof(UINT32))

INLINE VOID OsLpIncLoadUnitNum(VOID)
{
    g_lpLoadUnitNum++;
}

INLINE VOID OsLpDecLoadUnitNum(VOID)
{
    g_lpLoadUnitNum--;
}

INLINE UINT32 OsLpGetLoadUnitNum(VOID)
{
    return g_lpLoadUnitNum;
}
#endif /* LOS_LP_GROUP_H */
