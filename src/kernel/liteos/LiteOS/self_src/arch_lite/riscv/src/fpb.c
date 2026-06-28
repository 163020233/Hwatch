/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description : LiteOS riscv flash patch module implementation.
 * Author : Huawei LiteOS Team
 * Create : 2022-12-20
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
 * ---------------------------------------------------------------------------- */

#include "arch/fpb.h"
#include "los_printf.h"
#include "arch/cache.h"
#include "string.h"

/**
 * @ingroup fpb
 * .remap section
 */
#ifndef LITE_OS_SEC_REMAP
#define LITE_OS_SEC_REMAP   __attribute__ ((section(".fpb.remap")))
#endif

#define INS_INVALID         0x00000000
#define LITERAL_REG1_INDEX  0
#define LITERAL_REG2_INDEX  1
#define INS_REG_START       2
#define REG_EN              0x01UL

#define FPB_CTRL_GEN_BIT    0
#define FPB_CTRL_WPT_BIT    1
#define FPB_CTRL_PCO_BIT    2
#define FPB_CTRL_LACEN0_BIT 3

#define FPB                 ((FpbReg *)FPB_BASE)

typedef struct {
    volatile UINT32 ctrl;
    volatile UINT32 remap;
    volatile UINT32 comp[FPB_COMP_MAX];
} FpbReg;

#ifdef RISCV_FPB_SET_PCO
#define FPB_DSP_LEN         8
#define INS_TYPE            UINT64
#define REMAP_ALIGN         0x800
#define FPB_REMAP_SIZE      (FPB_DSP_LEN * FPB_COMP_MAX)

/**
 * Ins1: lui t0, target_addr[31:12]
 * lui:U-imm[31:12] rd 0110111---rd : t0(0b00101)
 * Ins1[11:0] = 0b00101 0110111 = 0x2B7
 */
#define PATCH_INS1          0x000002B7ULL
/**
 * Ins2:jalr x0, t0, target_addr[11:0]
 * jalr:I-imm[11:0] rs1 000 rd 1100111---rs1 : t0(0b00101)  rd : x0(0b00000)
 * Ins2[19:0] = 0b00101 000 00000 1100111= 0x28067
 */
#define PATCH_INS2          0x00028067ULL
#define INS1_OFFSET_MASK    0xFFFFF000
#define INS1_OFFSET_ADJ     0x1000
#define INS1_CHECK_BIT12    0x800
#define INS2_OFFSET_MASK    0x00000FFF
#define INS2_OFFSET         32
#define INS_OFFSET_DATA     20
#else
#define FPB_DSP_LEN         4
#define INS_TYPE            UINT32
#define REMAP_ALIGN         0x400
#define FPB_REMAP_SIZE      (FPB_DSP_LEN * FPB_COMP_MAX)
#define INS_OFFSET_MAX      0xFFFFE
#define INS_OFFSET_MIN      (-0x100000)

/**
 * Ins1: jal x0,I-imm[20:1]
 * jal :imm[20] imm[10:1] imm[11] imm[19:12] rd 1101111 --- rd:x0(0b00000)
 */
#define PATCH_INS1          0x0000006FUL
#define MAKE_INS(tmpInstr, offset)   do {                                                 \
        (tmpInstr) |= ((UINT32)(offset) << 11) & 0x80000000; /* Ins [31] : offset [20] */               \
        (tmpInstr) |= ((UINT32)(offset) << 20) & 0x7FE00000; /* Ins [30:21] : offset [10:1] */          \
        (tmpInstr) |= ((UINT32)(offset) << 9) & 0x00100000; /* Ins [20] : offset [11] */  \
        (tmpInstr) |= ((UINT32)(offset)) & 0x000FF000; /* Ins [19:12] : offset [19:12] */ \
    } while (0)
#endif

LITE_OS_SEC_REMAP INS_TYPE g_fpbRemap[FPB_COMP_MAX] __attribute__ ((aligned (REMAP_ALIGN))) = {0};

#define FPB_REMAP_ARR       ((VOID *)&g_fpbRemap[0])

STATIC UINT32 OsFpbAddPatchInputCheck(UINT32 oldAddr, UINT32 patchValue, FpbCompTypeEnum fpbType)
{
    if (fpbType > FPB_TYPE_LITERAL) {
        PRINT_DEBUG("type is wrong, set fpb patch err!\r\n");
        return FPB_TYPE_ERR;
    }
    if (((VOID *)(UINTPTR)oldAddr == NULL) ||
        (((VOID *)(UINTPTR)patchValue == NULL) && (fpbType == FPB_TYPE_INSTR))) {
        PRINT_DEBUG("oldAddr or patchValue invalid !\r\n ");
        return FPB_ERROR_INPUT_ERR;
    }

    if ((fpbType == FPB_TYPE_INSTR) && (oldAddr & 0x1)) {
        PRINT_DEBUG("address is not aligned to 2!\r\n ");
        return FPB_ADDR_NOT_ALIGN_ERR;
    }

    return FPB_SUCCESS;
}

STATIC UINT32 OsGetInstrRegIndex(UINT32 oldAddr, UINT16 *regComp)
{
    UINT32 regValue;
    UINT16 regCount;
    UINT8  flag = 0;

    for (regCount = INS_REG_START; regCount < FPB_COMP_MAX; regCount++) {
        regValue = FPB->comp[regCount];
        if (regValue & REG_EN) {
            if (regValue == (oldAddr | REG_EN)) {
                PRINT_DEBUG("the old_func_addr already has patched\r\n");
                *regComp = regCount;
                return FPB_COMP_REPEAT_ERR;
            }
        } else if (flag == 0) {
            *regComp = regCount;
            flag = 1;
        }
    }
    if (flag == 0) {
        PRINT_DEBUG("there is no free fpb comp register\r\n");
        return FPB_NO_FREE_COMP_ERR;
    }

    return FPB_SUCCESS;
}

STATIC UINT32 OsFpbComparatorRegConfig(UINT16 regIndex, UINT32 compAddr)
{
    if (regIndex >= FPB_COMP_MAX) {
        PRINT_DEBUG("regIndex:%u is out of range, REG_MAX is %d\r\n", regIndex, FPB_COMP_MAX);
        return FPB_NO_COMP_ERR;
    }

    if ((regIndex == LITERAL_REG1_INDEX) || (regIndex == LITERAL_REG2_INDEX)) {
        FPB->comp[regIndex] = compAddr;
    } else {
        FPB->comp[regIndex] = (compAddr | REG_EN);
    }

    return FPB_SUCCESS;
}

UINT32 OsFpbRedirectLiteral(UINT32 originalLiteralAddr, UINT32 targetLiteral)
{
    UINT16 regIndex;
    UINT32 literalReg1Value = FPB->comp[LITERAL_REG1_INDEX];
    UINT32 literalReg2Value = FPB->comp[LITERAL_REG2_INDEX];

    if ((literalReg1Value == originalLiteralAddr) || (literalReg2Value == originalLiteralAddr)) {
        return FPB_COMP_REPEAT_ERR;
    }

    if ((literalReg1Value != 0) && (literalReg2Value != 0)) {
        return FPB_NO_FREE_COMP_ERR;
    }

    if (literalReg1Value == 0) {
        regIndex = LITERAL_REG1_INDEX;
    } else {
        regIndex = LITERAL_REG2_INDEX;
    }

    (VOID)OsFpbComparatorRegConfig(regIndex, originalLiteralAddr);

    FPB->ctrl |= 1U << (regIndex + FPB_CTRL_LACEN0_BIT);
    *((UINT32 *)(UINTPTR)((FPB->remap) + (regIndex * FPB_DSP_LEN))) = targetLiteral;
    return FPB_SUCCESS;
}

STATIC INS_TYPE OsCalcJumpLinkInstr(UINT32 instrAddr, UINT32 targetAddr)
{
    INS_TYPE tmpInstr;

#if RISCV_FPB_SET_PCO
    UINT32 offset;
    (VOID)instrAddr;

    /**
     * Ins1: lui t0, target_addr[31:12]  as low 32 bit of tmpInstr
     * lui:U-imm[31:12] rd 0110111---rd : t0(0b00101)
     */
    tmpInstr = PATCH_INS1 | (targetAddr & INS1_OFFSET_MASK);
    /* U-imm of Ins1 need add 1 if targetAddr[11] = 1;cause Ins2 use a 12bit I-imm , bit11 = 1 means a negative num */
    if ((targetAddr & INS2_OFFSET_MASK) >= INS1_CHECK_BIT12) {
        tmpInstr += INS1_OFFSET_ADJ;
    }
    /**
     * Ins2:jalr x0, t0, target_addr[11:0]  as high 32 bit of tmpInstr
     * jalr:I-imm[11:0] rs1 000 rd 1100111---rs1 : t0(0b00101)  rd : x0(0b00000)
     */
    offset = (targetAddr & INS2_OFFSET_MASK) << INS_OFFSET_DATA;
    tmpInstr |= (PATCH_INS2 | offset) << INS2_OFFSET;
#else
    int32_t offset = (INT32)(targetAddr - instrAddr);
    /* If offset out of jalr range */
    if ((offset > INS_OFFSET_MAX) || (offset < INS_OFFSET_MIN)) {
        tmpInstr = INS_INVALID;
    } else {
        tmpInstr = PATCH_INS1;
        MAKE_INS(tmpInstr, offset);
    }
#endif

    return tmpInstr;
}

STATIC UINT32 OsGetInstr(UINT32 instrAddr, UINT32 targetAddr, INS_TYPE *newInstr, UINT8 isInstr)
{
    INS_TYPE tmpInstr;

    if (isInstr) {
        tmpInstr = OsCalcJumpLinkInstr(instrAddr, targetAddr);
        if (tmpInstr == INS_INVALID) {
            return FPB_TARGET_ADDR_ERR;
        }
    } else {
        PRINT_DEBUG("condition should be always true!\r\n");
        return FPB_TYPE_ERR;
    }

    *newInstr = tmpInstr;
    return FPB_SUCCESS;
}

STATIC UINT32 OsFpbRedirectInstr(UINT32 oldAddr, UINT32 patchValue)
{
    UINT32 ret;
    INS_TYPE newInstr;
    UINT16 regIndex;

    ret = OsGetInstrRegIndex(oldAddr, &regIndex);
    if (ret != FPB_SUCCESS) {
        return ret;
    }

    ret = OsGetInstr(oldAddr, patchValue, &newInstr, TRUE);
    if (ret != FPB_SUCCESS) {
        return ret;
    }

    ret = OsFpbComparatorRegConfig(regIndex, oldAddr);
    if (ret != FPB_SUCCESS) {
        return ret;
    }

    *((volatile INS_TYPE *)(UINTPTR)((FPB->remap) + (regIndex * FPB_DSP_LEN))) = newInstr;
    return ret;
}

STATIC VOID OsFpbControlEnable(VOID)
{
#if RISCV_FPB_SET_PCO
    FPB->ctrl = (1 << FPB_CTRL_GEN_BIT) | (1 << FPB_CTRL_PCO_BIT);
#else
    FPB->ctrl = (1 << FPB_CTRL_GEN_BIT) | (0 << FPB_CTRL_PCO_BIT);
#endif
}

STATIC VOID OsFpbRemapRegConfig(UINT32 remapTableAddr)
{
    FPB->remap = remapTableAddr;
}

VOID ArchFpbInit(VOID)
{
    UINT32 regCount;

    OsFpbControlEnable();
    OsFpbRemapRegConfig((UINTPTR)FPB_REMAP_ARR);
    (VOID)memset(FPB_REMAP_ARR, 0x00, FPB_REMAP_SIZE);

    for (regCount = 0; regCount < FPB_COMP_MAX; regCount++) {
        FPB->comp[regCount] = 0x0UL;
    }
}

UINT32 ArchFpbAddPatch(UINT32 oldAddr, UINT32 patchValue, FpbCompTypeEnum fpbType)
{
    UINT32 ret;

    if ((FPB->ctrl & (1 << FPB_CTRL_WPT_BIT)) != 0) {
        return FPB_BUSY_ERR;
    }

    ret = OsFpbAddPatchInputCheck(oldAddr, patchValue, fpbType);
    if (ret != FPB_SUCCESS) {
        return ret;
    }

    if (fpbType == FPB_TYPE_LITERAL) {
        ret = OsFpbRedirectLiteral(oldAddr, patchValue);
        if (ret != FPB_SUCCESS) {
            PRINT_ERR("set literal patch err!\r\n");
            return ret;
        }
    } else {
        ret = OsFpbRedirectInstr(oldAddr, patchValue);
        if (ret != FPB_SUCCESS) {
            PRINT_ERR("set instr patch err %x %x:%x!\r\n", ret, oldAddr, patchValue);
            return ret;
        }
    }

    return FPB_SUCCESS;
}

UINT32 ArchFpbDeletePatch(UINT32 oldAddr, FpbCompTypeEnum fpbType)
{
    UINT16 regIndex;
    UINT32 regValue;
    UINT32 ret;

    if ((FPB->ctrl & (1 << FPB_CTRL_WPT_BIT)) != 0) {
        return FPB_BUSY_ERR;
    }

    if (fpbType >= FPB_TYPE_MAX) {
        PRINT_DEBUG("type is wrong, set pfb patch err!\r\n");
        return FPB_TYPE_ERR;
    }

    if (fpbType == FPB_TYPE_LITERAL) {
        regValue = FPB->comp[LITERAL_REG1_INDEX];
        if (oldAddr == regValue) {
            FPB->comp[LITERAL_REG1_INDEX] = 0;
            return FPB_SUCCESS;
        } else {
            regValue = FPB->comp[LITERAL_REG2_INDEX];
            if (oldAddr == regValue) {
                FPB->comp[LITERAL_REG2_INDEX] = 0;
                return FPB_SUCCESS;
            }
        }
    } else {
        ret = OsGetInstrRegIndex(oldAddr, &regIndex);
        if (ret == FPB_COMP_REPEAT_ERR) {
            PRINT_DEBUG("delete comp : %u patch\r\n", regIndex);
            FPB->comp[regIndex] = 0;
            return FPB_SUCCESS;
        }
    }

    PRINT_DEBUG("no patch need to delete\r\n");
    return FPB_NO_COMP_ERR;
}

VOID ArchFpbDisable(VOID)
{
    FPB->ctrl &= ~((1 << FPB_CTRL_GEN_BIT) | (1 << FPB_CTRL_WPT_BIT) | (1 << FPB_CTRL_PCO_BIT));
}

VOID ArchFpbLock(VOID)
{
    /* FPB_CTRL_WPT_BIT can only write once after system power on */
    FPB->ctrl |= (1 << FPB_CTRL_WPT_BIT);
    ArchDCacheFlush();
    ArchICacheFlush();
}
