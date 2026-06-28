/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: AARCH32 reloc handle for aslr
 * Author: Huawei LiteOS Team
 * Create: 2022-07-05
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

#include "sys/types.h"
#include "los_typedef.h"
#include "arch/dynload.h"
#include "los_ld_reloc_pri.h"

#if defined(LOSCFG_LASLR_GOT)
STATIC LITE_OS_SEC_LASLR_TEXT UINT32 AslrArmRelativeReloc(const LDRelocParam *relocParam)
{
    INTPTR addend;
    UINTPTR position = relocParam->position;

    if (relocParam->shType == LD_SHT_RELA) {
        addend = relocParam->addend;
    } else {
        addend = *(INTPTR *)position;
    }

    /* B(S) + A */
    *(UINTPTR *)position = relocParam->baseSegment + (UINTPTR)addend;
    return LOS_OK;
}

#elif defined(LOSCFG_LASLR_GSPR)

STATIC LITE_OS_SEC_LASLR_TEXT UINT32 AslrArmAbs32Reloc(const LDRelocParam *relocParam)
{
#define ABS32_BIT_FROM  0U
#define ABS32_BIT_TO    0U
#define ABS32_BIT_WIDE 32U

    INTPTR addend = 0;
    UINTPTR operations;
    UINTPTR position = relocParam->position;
    UINTPTR symAddr = relocParam->symAdd;

    if (relocParam->shType == LD_SHT_RELA) {
        addend = relocParam->addend;
        operations = (symAddr + (UINTPTR)addend) | relocParam->thumbFunc;
    } else {
        operations = symAddr | relocParam->thumbFunc;
    }

    LD_MoveBitsArea32(operations, (UINT32 *)position, ABS32_BIT_TO, ABS32_BIT_FROM, ABS32_BIT_WIDE);

    return LOS_OK;
}

STATIC LITE_OS_SEC_LASLR_TEXT UINT32 AslrArmMovwOrMovtReloc(const LDRelocParam *relocParam)
{
#define ARM_MOV_INS1_BIT_FROM  0U
#define ARM_MOV_INS1_BIT_TO    0U
#define ARM_MOV_INS1_BIT_WIDE  12U
#define ARM_MOV_INS2_BIT_FROM  16U
#define ARM_MOV_INS2_BIT_TO    12U
#define ARM_MOV_INS2_BIT_WIDE  4U
#define ARM_MOV_BIT_SIGN       16U
#define ARM_MOV_OPERATION_MASK  0x0000ffffU
#define ARM_MOVT_OPERATION_MASK 0xffff0000U

    INTPTR addend = 0;
    UINTPTR operations;
    UINTPTR position = relocParam->position;
    UINTPTR symAddr = relocParam->symAdd;
    UINT8 bitsLowOrHigh;

    if (relocParam->shType == LD_SHT_RELA) {
        addend = relocParam->addend;
    }

    addend = (INTPTR)OS_SIGN_EXTEND(addend, ARM_MOV_BIT_SIGN); /* sign extend */
    operations = (UINTPTR)addend + symAddr;

    if (relocParam->relocType == OS_R_ARM_MOVT_ABS) {
        operations = operations & ARM_MOVT_OPERATION_MASK;
        bitsLowOrHigh = 1;
    } else {
        operations = operations & ARM_MOV_OPERATION_MASK;
        bitsLowOrHigh = 0;
    }
    LD_MoveBitsArea32(operations, (UINT32 *)position, ARM_MOV_INS1_BIT_TO +
                      (ARM_MOV_BIT_SIGN * bitsLowOrHigh), ARM_MOV_INS1_BIT_FROM, ARM_MOV_INS1_BIT_WIDE);
    LD_MoveBitsArea32(operations, (UINT32 *)position, ARM_MOV_INS2_BIT_TO +
                      (ARM_MOV_BIT_SIGN * bitsLowOrHigh), ARM_MOV_INS2_BIT_FROM, ARM_MOV_INS2_BIT_WIDE);

    return LOS_OK;
}
#endif

LITE_OS_SEC_LASLR_TEXT UINT32 ArchAslrRelocImp(const LD_ELF_RELA *rela, const LDRelocParam *relocParam)
{
    UINT32 ret = LOS_NOK;
    switch (LD_ELF_R_TYPE(rela->info)) {
#ifdef LOSCFG_LASLR_GOT
        case OS_R_ARM_RELATIVE:
            ret = AslrArmRelativeReloc(relocParam);
            break;
        case OS_R_ARM_NONE: /* None */
            break;
#elif defined(LOSCFG_LASLR_GSPR)
        case OS_R_ARM_ABS32: /* Word32, S + A */
            ret = AslrArmAbs32Reloc(relocParam);
            break;
        case OS_R_ARM_MOVW_ABS_NC:
        case OS_R_ARM_MOVT_ABS:
            ret = AslrArmMovwOrMovtReloc(relocParam);
            break;
#endif
        default:
            break;
    }
    return ret;
}

