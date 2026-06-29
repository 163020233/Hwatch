/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: LiteOS Patch Management Module Common Private HeadFile
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

#ifndef LOS_LP_COMM_H
#define LOS_LP_COMM_H

#include "los_memory.h"
#include "los_lp.h"
#include "los_sys.h"
#include "los_printf.h"

#define OS_LP_CRC_BYTE_BIT_NUM 8

#define OS_LP_PHASE_UNINIT 0
#define OS_LP_PHASE_INITED 0xad5678da
#define OS_LP_BUF_ADDR_ALIGN 4

#define OS_LP_MAX_UINT_NUM 9999

#define OS_LP_VERSION_LEN 48

#define OS_LP_VER_LEN 32
#define OS_LP_SP_FORMAT_VERSION_LEN 4
#define OS_LP_SP_NAME_LEN 6
#define OS_LP_SP_FLAG_LEN 2

#define OS_LP_CRC16_POLY 0x1021
#define OS_LP_FUNC_SIZE 16
#define OS_LP_CRC_LENGTH 32
#define OS_LP_FIRST_BYTE 0
#define OS_LP_SECOND_BYTE 1
#define OS_LP_THIRD_BYTE 2
#define OS_LP_FORTH_BYTE 3

#define OS_LP_UNIT_INNER_NO_2_INDEX(innerNo) ((innerNo) - 1)

#define OS_LP_SP_PACKET_IDENT "SP"
#define OS_LP_IDENT_LENGTH 2

#define OS_LP_INDEX_UNITOFFSET_OFFSET 4
#define OS_LP_INDEX_UNITSIZE_OFFSET 8

/* Macro Definitions Related to the Static Patch Unit Header */
#define OS_LP_SP_PATCHFILE_FLAG_SIZE 4
#define OS_LP_SP_PATCHFILE_CRC_SIZE 2
#define OS_LP_SP_PATCHFILE_TYPE_SIZE 2
#define OS_LP_SP_BASECODE_VERSION_SIZE OS_LP_VERSION_LEN
#define OS_LP_SP_PATCHUNIT_NUM_SIZE 4
#define OS_LP_SP_PATCHUNIT_INDEX_POS_SIZE 4
#define OS_LP_SP_ENTRY_NUM_SIZE 4
#define OS_LP_SP_RESERVED_SIZE 4

#define OS_LP_SP_PATCHFILE_FLAG_OFFSET 0
#define OS_LP_SP_PATCHFILE_CRC_OFFSET (OS_LP_SP_PATCHFILE_FLAG_OFFSET + OS_LP_SP_PATCHFILE_FLAG_SIZE)
#define OS_LP_SP_PACKAGE_TYPE_OFFSET (OS_LP_SP_PATCHFILE_CRC_OFFSET + OS_LP_SP_PATCHFILE_CRC_SIZE)
#define OS_LP_SP_BASECODE_VERSION_OFFSET (OS_LP_SP_PACKAGE_TYPE_OFFSET + OS_LP_SP_PATCHFILE_TYPE_SIZE)
#define OS_LP_SP_PATCHUNIT_NUM_OFFSET (OS_LP_SP_BASECODE_VERSION_OFFSET + OS_LP_SP_BASECODE_VERSION_SIZE)
#define OS_LP_SP_PATCHUNIT_INDEX_POS_OFFSET (OS_LP_SP_PATCHUNIT_NUM_OFFSET + OS_LP_SP_PATCHUNIT_NUM_SIZE)
#define OS_LP_SP_ENTRY_NUM_OFFSET (OS_LP_SP_PATCHUNIT_INDEX_POS_OFFSET + OS_LP_SP_PATCHUNIT_INDEX_POS_SIZE)
#define OS_LP_SP_RESERVED_OFFSET (OS_LP_SP_ENTRY_NUM_OFFSET + OS_LP_SP_ENTRY_NUM_SIZE)
#define OS_LP_SP_COMPILETIME_OFFSET (OS_LP_SP_RESERVED_OFFSET + OS_LP_SP_RESERVED_SIZE)

#define OS_LP_ADDR_START_END_IN_RANGE(startAddr, size, compStartAddr, compSize) \
    (!(((startAddr) >= ((compStartAddr) + (compSize))) || (((startAddr) + (size)) <= (compStartAddr))))

#define OS_LP_ADDR_START_END_EQUEL(startAddr, size, compStartAddr, compSize) \
    (((startAddr) == (compStartAddr)) && ((size) == (compSize)))


/* Instruction Replacement Macro */
#define OS_LP_INSTRUCTION_GUARD 0xABABABAB
#define OS_LP_FUNC_UNPATCH 0
#define OS_LP_FUNC_PATCHED 1

#define OS_LP_SP_FLAGS "SP"
#define OS_LP_SP_FLAGS_LENGTH 2
#define OS_LP_SP_NAME_LENGTH 6
#define OS_LP_SHIFT_BYTE 8
#define OS_LP_SHIFT_HALF_WORD 16
#define OS_LP_SHIFT_THREE_BYTE 24
#define OS_LP_CRC_TABLE_SIZE 256

#define OS_LP_ALIGN_SZIE  16

#define OS_LP_MEM_ALLOC_ALIGN(len, align) LOS_MemAllocAlign(m_aucSysMem0, (UINT32)(len), (align))
#define OS_LP_MEM_ALLOC(len) OS_LP_MEM_ALLOC_ALIGN(len, OS_LP_ALIGN_SZIE)
#define OS_LP_MEM_FREE(addr) (void)LOS_MemFree(m_aucSysMem0, (void *)(addr))

enum TagLpCpuType {
    OS_LP_BIG_ENDIAN = 0x1,
    OS_LP_LITTLE_ENDIAN = 0x2,
    OS_LP_CT_BUTT
};

/* Date structure definition. */
struct DateStru {
    UINT16 year;     /**< year, the value range is 1970 ~ 2100 */
    UINT8 month;     /**< month, the value range is 1 - 12 */
    UINT8 date;      /**< day, the value range is 1 - 31 */
};

/* Time structure definition. */
struct TimeStru {
    UINT8 hour;      /**< hour, the value range is 0 - 23 */
    UINT8 minute;    /**< minute, the value range is 0 - 59 */
    UINT8 second;    /**< second, the value range is 0 - 59 */
    UINT8 padding;   /**< Reserved for 4 bytes alignment. */
};

#pragma pack(push, 1)
struct TagPatSPFileFormat {
    UINT8 hpFormatVer[OS_LP_SP_FORMAT_VERSION_LEN];
    UINT8 hpFlag[OS_LP_SP_FLAG_LEN];
    UINT8 hpFileName[OS_LP_SP_NAME_LEN];
    UINT16 hpCrc;
    UINT16 independFlag;
    UINT32 patchNo;
    UINT8 progVersion[OS_LP_VERSION_LEN];
    UINT8 patchVersion[OS_LP_VER_LEN];
    struct DateStru createData;
    struct TimeStru  createTime;
    UINT32 netID;
    UINT32 progID;
    UINT64 codeAddr;
    UINT64 codeLength;
    UINT64 shareDataAddr;
    UINT64 shareDataLength;
    UINT64 dataAddr;
    UINT64 dataLength;
    UINT32 funcs;
    UINT8 flag[OS_LP_SP_FLAG_LEN];
    /* the length of CRC used in patch file program */
    UINT16 crcLen;
    /* the total size of seg info tables */
    UINT32 segInfSize;
};

struct TagstTime {
    UINT16 year;
    UINT8 month;
    UINT8 day;
    UINT8 hour;
    UINT8 minute;
    UINT8 second;
    UINT8 tick;
};

struct TagSPPatchPacketHeader {
    UINT8 ident[4];
    UINT16 patchCRC;
    UINT16 packetType;
    UINT8 progVersion[OS_LP_VERSION_LEN];
    UINT32 patchUnitNum;
    UINT32 indexOffset;
    UINT32 entryNum;
    UINT32 reserved;
    struct TagstTime compileTime;
};

struct TagLpUnitIndex {
    UINT32 unitType;
    UINT32 unitOffset;
    UINT32 unitSize;
    UINT32 unitFlag;
    UINT32 reserved;
};
#pragma pack(pop)

extern struct LpImagePubInfo g_lpPubInfo;
extern UINT16 g_mcPatchCrcTable[OS_LP_CRC_TABLE_SIZE];
extern enum TagLpCpuType g_mcCpuType;

extern void OsLpPatFuncReplace(UINTPTR orgAddr, UINTPTR patAddr);
extern void OsLpPatFuncRollback(UINTPTR orgAddr, UINTPTR backup);
extern void OsLpModifyMmuRoAttr(UINTPTR addr);
#endif /* LOS_LP_COMM_H */