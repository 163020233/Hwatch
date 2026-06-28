/*----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: LiteOS Patch Management Initialization Module Implementation
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

#include "los_init.h"
#include "lp_group.h"

/* The start address is 128-byte aligned to ensure memory not misalignment. */
UINT16 g_mcPatchCrcTable[OS_LP_CRC_TABLE_SIZE] __attribute__((aligned(128)));
volatile UINT32 g_lpInitGuard;

/* Patch module information structure */
struct TagLPGroupMagInfo *g_lpGroupInfo = NULL;
struct LpImagePubInfo g_lpPubInfo;
enum TagLpCpuType g_mcCpuType;

struct TagLpUnitInfo *g_lpUnitInfo = NULL;

UINT32 g_kernelLpGroupId = 0;
UINT32 g_lpLoadUnitNum = 0;
volatile UINT32 g_lpPubInitPhase = OS_LP_PHASE_UNINIT;

STATIC VOID OsLpCrcTabInit(VOID)
{
    UINT16 indexI;
    UINT16 indexJ;
    UINT16 value;

    for (indexI = 0; indexI < OS_LP_CRC_TABLE_SIZE; indexI++) {
        value = (UINT16)(indexI << OS_LP_SHIFT_BYTE);
        for (indexJ = 0; indexJ < OS_LP_CRC_BYTE_BIT_NUM; indexJ++) {
            /* 0x8000 is used to mask all bits other that the last bit */
            if ((value & 0x8000) != 0) {
                value = (UINT16)((value << 1) ^ OS_LP_CRC16_POLY);
            } else {
                value <<= 1;
            }
        }
        g_mcPatchCrcTable[indexI] = value;
    }
    return;
}

STATIC INLINE UINT32 OsLpGlobalInit(UINTPTR **addr)
{
    size_t totalSize;
    size_t grpInfoSize;
    size_t unitInfoSize;
    size_t magicWordSize = sizeof(UINT32);
    size_t groupNumSize = sizeof(UINT32);
    size_t unitNumSize = sizeof(UINT32);
    UINT32 loop;
    CHAR *toolVersion = NULL;

    g_mcCpuType = OS_LP_LITTLE_ENDIAN;

    grpInfoSize = sizeof(struct TagLPGroupMagInfo) * g_lpPubInfo.groupNum;
    unitInfoSize = sizeof(struct TagLpUnitInfo) * g_lpPubInfo.maxLPUnitNum;

    totalSize = grpInfoSize + unitInfoSize + magicWordSize + groupNumSize + unitNumSize + OS_LP_VERSION_LEN;

    *addr = OS_LP_MEM_ALLOC(totalSize);
    if (*addr == NULL) {
        return LOS_ERRNO_LP_MEM_NOT_ENOUGH;
    }

    (VOID)memset_s(*addr, totalSize, 0, totalSize);

    /* Magic Words */
    *((UINT16 *)((UINTPTR)(*addr))) = OS_LP_MAG_INFO_TCB_MAGIC_LOW;
    (*((UINT16 *)((UINTPTR)(*addr))))++;
    *((UINT16 *)((UINTPTR)(*addr) + OS_LP_MAG_TCB_MAGIC_HIGH_OFFSET)) = OS_LP_MAG_INFO_TCB_MAGIC_HIGH;

    *((UINT32 *)((UINTPTR)(*addr) + magicWordSize)) = g_lpPubInfo.groupNum;
    *((UINT32 *)((UINTPTR)(*addr) + magicWordSize + groupNumSize)) = g_lpPubInfo.maxLPUnitNum;

    /* Tool version information */
    toolVersion = (CHAR *)((UINTPTR)(*addr) + magicWordSize + groupNumSize + unitNumSize);
    (VOID)strcpy_s(toolVersion, OS_LP_VERSION_LEN, OS_LP_TOOL_VER);

    g_lpGroupInfo = (struct TagLPGroupMagInfo *)((UINTPTR)toolVersion + OS_LP_VERSION_LEN);
    g_lpUnitInfo = (struct TagLpUnitInfo *)((UINTPTR)g_lpGroupInfo + grpInfoSize);

    for (loop = 0; loop < g_lpPubInfo.maxLPUnitNum; loop++) {
        /* Internal LP unit number. The base value starts from 1. */
        g_lpUnitInfo[loop].interNo = loop + 1;
        g_lpUnitInfo[loop].unitState = (UINT16)UNIT_STATE_IDLE;
        g_lpUnitInfo[loop].resetState = (UINT16)UNIT_STATE_IDLE;
        g_lpUnitInfo[loop].procFlag = (UINT16)UNIT_PATCH_NOTPROC_STATE;
    }
    return LOS_OK;
}

STATIC UINT32 OsLpMemAllocInit(UINTPTR *allocAddr)
{
    UINTPTR *addr = NULL;
    UINT32 ret;

    ret = OsLpGlobalInit(&addr);
    if (ret != LOS_OK) {
        *allocAddr = (UINTPTR)NULL;
        return ret;
    }

    OsLpCrcTabInit();

    *allocAddr = (UINTPTR)addr;

    return LOS_OK;
}

LITE_OS_SEC_TEXT_INIT_LEVEL_ONE UINT32 OsLpConfigInit(VOID)
{
    UINT32 ret;
    UINT32 groupId;
    UINTPTR allocAddr;

    PRINT_INFO("Patch init start.\n");

    if (g_lpPubInitPhase == OS_LP_PHASE_INITED) {
        return LOS_OK;
    }

    g_lpPubInfo.groupNum = OS_LP_MAX_GROUP_NUM;
    g_lpPubInfo.maxLPUnitNum = OS_LP_MAX_UNIT_NUM;

    ret = OsLpMemAllocInit(&allocAddr);
    if (ret != LOS_OK) {
        return ret;
    }

    PRINT_INFO("Patch memory init ok!\n");

    ret = OsLpGroupInfoReg(&groupId);
    if (ret != LOS_OK) {
        OS_LP_MEM_FREE(allocAddr);
        return ret;
    }
    g_kernelLpGroupId = groupId;
    g_lpPubInitPhase = OS_LP_PHASE_INITED;

    PRINT_INFO("Patch init OK!\n");
    return ret;
}
LOS_SYS_INIT(OsLpConfigInit, SYS_INIT_LEVEL_KERNEL_ADDITION, SYS_INIT_SYNC_0);