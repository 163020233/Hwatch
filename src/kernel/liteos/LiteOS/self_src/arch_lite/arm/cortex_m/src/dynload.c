/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: AARCH32 Dynamic Load handle
 * Author: Huawei LiteOS Team
 * Create: 2022-02-25
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
/* ----------------------------------------------------------------------------
 * Notice of Export Control Law
 * ===============================================
 * Huawei LiteOS may be subject to applicable export control laws and regulations, which might
 * include those applicable to Huawei LiteOS of U.S. and the country in which you are located.
 * Import, export and usage of Huawei LiteOS in any manner by you shall be in compliance with such
 * applicable export control laws and regulations.
 * --------------------------------------------------------------------------- */

#include "los_printf.h"
#include "los_ld_reloc_pri.h"

LITE_OS_SEC_TEXT_MINOR UINT32 ELF_ArmPc24Reloc(const LDRelocParam *relocParam)
{
#define PC24_BIT_FROM  0U
#define PC24_BIT_TO    2U
#define PC24_BIT_WIDE  24U
#define PC24_BIT_SIGN  26U
#define PC24_BIT_CHECK 0x3U

    long addend = 0;
    long operations;
    UINTPTR position = relocParam->position;
    UINTPTR symAddr = relocParam->symAdd;

    if (relocParam->shType == LD_SHT_RELA) {
        addend = relocParam->addend;
    } else {
        LD_MoveBitsArea32(*(UINTPTR *)position, (UINT32 *)(VOID *)&addend, PC24_BIT_FROM, PC24_BIT_TO, PC24_BIT_WIDE);
        addend = (long)OS_SIGN_EXTEND(addend, PC24_BIT_SIGN); /* sign extend */
    }

    operations = (long)(((UINTPTR)(symAddr + addend) | relocParam->thumbFunc) - position);
    /*
     * Insane check: R_ARM_PC24 should be in 26bit, and the lowest 2bit must be 0
     * Check if is in 26bit after sign extended
     */
    if (!OS_CHECK_FITS(operations, PC24_BIT_SIGN)) {
        return LOS_NOK;
    }
    /* Check if the lowest 2bit are 0 */
    if ((unsigned long)operations & PC24_BIT_CHECK) {
        return LOS_NOK;
    }

    LD_MoveBitsArea32((UINTPTR)operations, (UINT32 *)position, PC24_BIT_TO, PC24_BIT_FROM, PC24_BIT_WIDE);

    return LOS_OK;
}

LITE_OS_SEC_TEXT_MINOR UINT32 ELF_ArmAbs32Reloc(const LDRelocParam *relocParam)
{
#define ABS32_BIT_FROM  0U
#define ABS32_BIT_TO    0U
#define ABS32_BIT_WIDE 32U

    long addend = 0;
    long operations;
    UINTPTR position = relocParam->position;
    UINTPTR symAddr = relocParam->symAdd;

    if (relocParam->shType == LD_SHT_RELA) {
        addend = relocParam->addend;
    } else {
        LD_MoveBitsArea32(*(UINTPTR *)position, (UINT32 *)(VOID *)&addend,
                          ABS32_BIT_FROM, ABS32_BIT_TO, ABS32_BIT_WIDE);
    }

    operations = (long)((UINTPTR)(symAddr + addend) | relocParam->thumbFunc);

    LD_MoveBitsArea32((UINTPTR)operations, (UINT32 *)position, ABS32_BIT_TO, ABS32_BIT_FROM, ABS32_BIT_WIDE);

    return LOS_OK;
}

LITE_OS_SEC_TEXT_MINOR UINT32 ELF_ArmRel32Reloc(const LDRelocParam *relocParam)
{
#define REL32_BIT_FROM  0U
#define REL32_BIT_TO    0U
#define REL32_BIT_WIDE  32U

    long addend = 0;
    long operations;
    UINTPTR position = relocParam->position;
    UINTPTR symAddr = relocParam->symAdd;

    if (relocParam->shType == LD_SHT_RELA) {
        addend = relocParam->addend;
    } else {
        LD_MoveBitsArea32(*(UINTPTR *)position, (UINT32 *)(VOID *)&addend,
                          REL32_BIT_FROM, REL32_BIT_TO, REL32_BIT_WIDE);
    }

    operations = (long)(((UINTPTR)(symAddr + addend) | relocParam->thumbFunc) - position);

    LD_MoveBitsArea32((UINTPTR)operations, (UINT32 *)position, REL32_BIT_TO, REL32_BIT_FROM, REL32_BIT_WIDE);

    return LOS_OK;
}

#define PC22_INS_BIT1       10U
#define PC22_INS_BIT2       29U
#define PC22_INS_BIT3       27U
#define PC22_INS1_BIT_FROM  16U
#define PC22_INS1_BIT_TO    1U
#define PC22_INS1_BIT_WIDE  11U
#define PC22_INS2_BIT_FROM  0U
#define PC22_INS2_BIT_TO    12U
#define PC22_INS2_BIT_WIDE  10U
#define PC22_INS3_BIT_FROM  0U
#define PC22_INS3_BIT_TO    22U
#define PC22_INS3_BIT_WIDE  1U
#define PC22_INS4_BIT_FROM  0U
#define PC22_INS4_BIT_TO    23U
#define PC22_INS4_BIT_WIDE  1U
#define PC22_INS5_BIT_FROM  0U
#define PC22_INS5_BIT_TO    24U
#define PC22_INS5_BIT_WIDE  1U
#define PC22_BIT_SIGN       24U
#define PC22_BIT_CHECK      1U
#define PC22_OPERATION_MASK 0x01FFFFFEU
LITE_OS_SEC_TEXT_MINOR UINT32 ELF_ThumbPc22Reloc(const LDRelocParam *relocParam)
{
    long addend = 0;
    long operations;
    UINTPTR position = relocParam->position;
    UINTPTR symAddr = relocParam->symAdd;
    UINT32 instruction;
    UINT32 s;
    UINT32 j1;
    UINT32 j2;
    UINT32 i1;
    UINT32 i2 = 0;

    instruction = OS_READ_INSTRUCTION_THUMB32(position);
    if (relocParam->shType == LD_SHT_RELA) {
        addend = relocParam->addend;
    } else {
        /* sign extend: S:I1:I2:imm10:imm11:'0', 32 */
        s = (instruction >> PC22_INS_BIT1) & 1U;
        j1 = (instruction >> PC22_INS_BIT2) & 1U;
        j2 = (instruction >> PC22_INS_BIT3) & 1U;
        i1 = !(j1 ^ s);
        i2 = !(j2 ^ s);
        LD_MoveBitsArea32(instruction, (UINT32 *)(VOID *)&addend,
                          PC22_INS1_BIT_FROM, PC22_INS1_BIT_TO, PC22_INS1_BIT_WIDE); /* im11 */
        LD_MoveBitsArea32(instruction, (UINT32 *)(VOID *)&addend,
                          PC22_INS2_BIT_FROM, PC22_INS2_BIT_TO, PC22_INS2_BIT_WIDE); /* im10 */
        LD_MoveBitsArea32(i1, (UINT32 *)(VOID *)&addend, PC22_INS3_BIT_FROM, PC22_INS3_BIT_TO, PC22_INS3_BIT_WIDE);
        LD_MoveBitsArea32(i2, (UINT32 *)(VOID *)&addend, PC22_INS4_BIT_FROM, PC22_INS4_BIT_TO, PC22_INS4_BIT_WIDE);
        LD_MoveBitsArea32(s, (UINT32 *)(VOID *)&addend, PC22_INS5_BIT_FROM, PC22_INS5_BIT_TO, PC22_INS5_BIT_WIDE);
    }

    addend = (long)OS_SIGN_EXTEND(addend, PC22_BIT_SIGN);
    operations = (long)(((UINTPTR)(symAddr + addend) | relocParam->thumbFunc) - position);
    if (!((unsigned long)operations & PC22_BIT_CHECK) || !OS_CHECK_FITS(operations, PC22_BIT_SIGN)) {
        PRINT_ERR("Only Thumb addresses allowed!\n");
        return LOS_NOK;
    }

    operations = (long)((unsigned long)operations & PC22_OPERATION_MASK);
    s = ((unsigned long)operations >> PC22_INS5_BIT_TO) & 1U;
    j1 = s ^ (~((unsigned long)operations >> PC22_INS4_BIT_TO) & 1U);
    j2 = s ^ (~((unsigned long)operations >> PC22_INS3_BIT_TO) & 1U);
    LD_MoveBitsArea32(s, (UINT32 *)&instruction, PC22_INS5_BIT_FROM, PC22_INS_BIT1, PC22_INS5_BIT_WIDE);
    LD_MoveBitsArea32(j1, (UINT32 *)&instruction, PC22_INS4_BIT_FROM, PC22_INS_BIT2, PC22_INS4_BIT_WIDE);
    LD_MoveBitsArea32(j2, (UINT32 *)&instruction, PC22_INS3_BIT_FROM, PC22_INS_BIT3, PC22_INS3_BIT_WIDE);
    LD_MoveBitsArea32((UINTPTR)operations, (UINT32 *)&instruction,
                      PC22_INS1_BIT_TO, PC22_INS1_BIT_FROM, PC22_INS1_BIT_WIDE);
    LD_MoveBitsArea32((UINTPTR)operations, (UINT32 *)&instruction,
                      PC22_INS2_BIT_TO, PC22_INS2_BIT_FROM, PC22_INS2_BIT_WIDE);

    OS_WRITE_INSTRUCTION_THUMB32(position, instruction);

    return LOS_OK;
}

LITE_OS_SEC_TEXT_MINOR UINT32 ELF_ArmCallReloc(const LDRelocParam *relocParam)
{
#define CALL_BIT_FROM  0U
#define CALL_BIT_TO    2U
#define CALL_BIT_WIDE  24U
#define CALL_BIT_SIGN  26U
#define CALL_BIT_CHECK 0x03U
#define CALL_OPERATION_MASK 0x03fffffeU

    long addend = 0;
    long operations;
    UINTPTR position = relocParam->position;
    UINTPTR symAddr = relocParam->symAdd;

    if (relocParam->shType == LD_SHT_RELA) {
        addend = relocParam->addend;
    } else {
        LD_MoveBitsArea32(*(UINTPTR *)position, (UINT32 *)(VOID *)&addend, CALL_BIT_FROM, CALL_BIT_TO, CALL_BIT_WIDE);
    }

    addend = (long)OS_SIGN_EXTEND(addend, CALL_BIT_SIGN);

    operations = (long)(((UINTPTR)(symAddr + addend) | relocParam->thumbFunc) - position);
    if (!OS_CHECK_FITS(operations, CALL_BIT_SIGN)) {
        return LOS_NOK;
    }
    if (((unsigned long)operations & CALL_BIT_CHECK)) {
        return LOS_NOK;
    }

    LD_MoveBitsArea32((UINTPTR)((unsigned long) operations & CALL_OPERATION_MASK),
                      (UINT32 *)position, CALL_BIT_TO, CALL_BIT_FROM, CALL_BIT_WIDE);
    return LOS_OK;
}

LITE_OS_SEC_TEXT_MINOR UINT32 ELF_ThumbPc9Reloc(const LDRelocParam *relocParam)
{
#define PC9_BIT_FROM  0U
#define PC9_BIT_TO    1U
#define PC9_BIT_WIDE  8U
#define PC9_BIT_SIGN  9U
#define PC9_BIT_CHECK 1U
#define PC9_OPERATION_MASK 0x01FEU

    long addend = 0;
    long operations;
    UINTPTR position = relocParam->position;
    UINTPTR symAddr = relocParam->symAdd;

    if (relocParam->shType == LD_SHT_RELA) {
        addend = relocParam->addend;
    } else {
        LD_MoveBitsArea32(*(UINT16 *)position, (UINT32 *)(VOID *)&addend, PC9_BIT_FROM, PC9_BIT_TO, PC9_BIT_WIDE);
    }

    addend = (long)OS_SIGN_EXTEND(addend, PC9_BIT_SIGN); /* sign extend */
    operations = (long)(symAddr - position) + addend;
    if (!OS_CHECK_FITS(operations, PC9_BIT_SIGN)) {
        return LOS_NOK;
    }
    if ((unsigned long)operations & PC9_BIT_CHECK) {
        return LOS_NOK;
    }

    operations = (long)((unsigned long)operations & PC9_OPERATION_MASK);
    LD_MoveBitsArea16((UINTPTR)operations, (UINT16 *)position, PC9_BIT_TO, PC9_BIT_FROM, PC9_BIT_WIDE);

    return LOS_OK;
}

LITE_OS_SEC_TEXT_MINOR UINT32 ELF_ThumbPc11Reloc(const LDRelocParam *relocParam)
{
#define PC11_BIT_FROM  0U
#define PC11_BIT_TO    1U
#define PC11_BIT_WIDE  11U
#define PC11_BIT_SIGN  12U
#define PC11_BIT_CHECK 1U
#define PC11_OPERATION_MASK 0x0FFEU

    long addend = 0;
    long operations;
    UINTPTR position = relocParam->position;
    UINTPTR symAddr = relocParam->symAdd;

    if (relocParam->shType == LD_SHT_RELA) {
        addend = relocParam->addend;
    } else {
        LD_MoveBitsArea32(*(UINT16 *)position, (UINT32 *)(VOID *)&addend, PC11_BIT_FROM, PC11_BIT_TO, PC11_BIT_WIDE);
    }

    addend = (long)OS_SIGN_EXTEND(addend, PC11_BIT_SIGN); /* sign extend */
    operations = (long)(symAddr - position) + addend;
    if (!OS_CHECK_FITS(operations, PC11_BIT_SIGN)) {
        return LOS_NOK;
    }
    if ((unsigned long)operations & PC11_BIT_CHECK) {
        return LOS_NOK;
    }

    operations = (long)((unsigned long)operations & PC11_OPERATION_MASK);
    LD_MoveBitsArea16((UINTPTR)operations, (UINT16 *)position, PC11_BIT_TO, PC11_BIT_FROM, PC11_BIT_WIDE);

    return LOS_OK;
}

LITE_OS_SEC_TEXT_MINOR UINT32 ELF_ArmAbs8Reloc(const LDRelocParam *relocParam)
{
#define ABS8_BIT_FROM  0U
#define ABS8_BIT_TO    0U
#define ABS8_BIT_WIDE  9U
#define ABS8_BIT_SIGN  9U
#define ABS8_BIT_CHECK (-128)
#define ABS8_RESULT_BIT_WIDE  8U

    long addend = 0;
    long operations;
    UINTPTR position = relocParam->position;
    UINTPTR symAddr = relocParam->symAdd;

    if (relocParam->shType == LD_SHT_RELA) {
        addend = relocParam->addend;
    } else {
        LD_MoveBitsArea32(position, (UINT32 *)(VOID *)&addend, ABS8_BIT_FROM, ABS8_BIT_TO, ABS8_BIT_WIDE);
    }

    addend = (long)OS_SIGN_EXTEND(addend, ABS8_BIT_SIGN); /* sign extend */
    operations = (long)(symAddr + addend);
    /* -128 <= X <= 255 */
    if (!((OS_CHECK_FITS(operations, ABS8_BIT_SIGN)) && (operations >= ABS8_BIT_CHECK))) {
        return LOS_NOK;
    }

    LD_MoveBitsArea32((UINTPTR)operations, (UINT32 *)position, ABS8_BIT_TO, ABS8_BIT_FROM, ABS8_RESULT_BIT_WIDE);

    return LOS_OK;
}

LITE_OS_SEC_TEXT_MINOR UINT32 ELF_ArmAbs16Reloc(const LDRelocParam *relocParam)
{
#define ABS16_BIT_FROM  0U
#define ABS16_BIT_TO    0U
#define ABS16_BIT_WIDE  17U
#define ABS16_BIT_SIGN  17U
#define ABS16_BIT_CHECK (-32768)
#define ABS16_RESULT_BIT_WIDE  16U

    long addend = 0;
    long operations;
    UINTPTR position = relocParam->position;
    UINTPTR symAddr = relocParam->symAdd;

    if (relocParam->shType == LD_SHT_RELA) {
        addend = relocParam->addend;
    } else {
        LD_MoveBitsArea32(position, (UINT32 *)(VOID *)&addend, ABS16_BIT_FROM, ABS16_BIT_TO, ABS16_BIT_WIDE);
    }

    addend = (long)OS_SIGN_EXTEND(addend, ABS16_BIT_SIGN); /* sign extend */
    operations = (long)(symAddr + addend);
    /* -32768 <= X <= 65535 */
    if (!((OS_CHECK_FITS(operations, ABS16_BIT_SIGN)) && (operations >= ABS16_BIT_CHECK))) {
        return LOS_NOK;
    }

    LD_MoveBitsArea32((UINTPTR)operations, (UINT32 *)position, ABS16_BIT_TO, ABS16_BIT_FROM, ABS16_RESULT_BIT_WIDE);

    return LOS_OK;
}

LITE_OS_SEC_TEXT_MINOR UINT32 ELF_ArmAbs12Reloc(const LDRelocParam *relocParam)
{
#define ABS12_BIT_FROM  0U
#define ABS12_BIT_TO    0U
#define ABS12_BIT_WIDE  12U
#define ABS12_BIT_SIGN  12U

    long addend = 0;
    long operations;
    UINTPTR position = relocParam->position;
    UINTPTR symAddr = relocParam->symAdd;

    if (relocParam->shType == LD_SHT_RELA) {
        addend = relocParam->addend;
    } else {
        LD_MoveBitsArea32(*(UINTPTR *)position, (UINT32 *)(VOID *)&addend,
                          ABS12_BIT_FROM, ABS12_BIT_TO, ABS12_BIT_WIDE);
    }

    addend = (long)OS_SIGN_EXTEND(addend, ABS12_BIT_SIGN); /* sign extend */
    operations = (long)(symAddr + addend);
    if (!OS_CHECK_FITS(operations, ABS12_BIT_SIGN)) {
        return LOS_NOK;
    }

    LD_MoveBitsArea32((UINTPTR)operations, (UINT32 *)position, ABS12_BIT_TO, ABS12_BIT_FROM, ABS12_BIT_WIDE);

    return LOS_OK;
}

LITE_OS_SEC_TEXT_MINOR UINT32 ELF_ThumbAbs5Reloc(const LDRelocParam *relocParam)
{
#define ABS5_BIT_FROM  6U
#define ABS5_BIT_TO    2U
#define ABS5_BIT_WIDE  5U
#define ABS5_BIT_SIGN  7U
#define ABS5_OPERATION_MASK 0x007CU

    long addend = 0;
    long operations;
    UINTPTR position = relocParam->position;
    UINTPTR symAddr = relocParam->symAdd;

    if (relocParam->shType == LD_SHT_RELA) {
        addend = relocParam->addend;
    } else {
        LD_MoveBitsArea32(*(UINT16 *)position, (UINT32 *)(VOID *)&addend, ABS5_BIT_FROM, ABS5_BIT_TO, ABS5_BIT_WIDE);
    }

    operations = (long)(symAddr + addend);
    if (!OS_CHECK_FITS(operations, ABS5_BIT_SIGN)) {
        return LOS_NOK;
    }

    operations = (long)((unsigned long)operations & ABS5_OPERATION_MASK);

    LD_MoveBitsArea16((UINTPTR)operations, (UINT16 *)position, ABS5_BIT_TO, ABS5_BIT_FROM, ABS5_BIT_WIDE);

    return LOS_OK;
}

#define THUMB_MOV_INS1_BIT_FROM  16U
#define THUMB_MOV_INS1_BIT_TO    12U
#define THUMB_MOV_INS1_BIT_WIDE  4U
#define THUMB_MOV_INS2_BIT_FROM  26U
#define THUMB_MOV_INS2_BIT_TO    11U
#define THUMB_MOV_INS2_BIT_WIDE  1U
#define THUMB_MOV_INS3_BIT_FROM  12U
#define THUMB_MOV_INS3_BIT_TO    8U
#define THUMB_MOV_INS3_BIT_WIDE  3U
#define THUMB_MOV_INS4_BIT_FROM  0U
#define THUMB_MOV_INS4_BIT_TO    0U
#define THUMB_MOV_INS4_BIT_WIDE  8U
#define THUMB_MOV_BIT_SIGN       16U
#define THUMB_MOV_OPERATION_MASK  0x0000ffffU
#define THUMB_MOVT_OPERATION_MASK 0xffff0000U
LITE_OS_SEC_TEXT_MINOR UINT32 ELF_ThumbMovwOrMovtReloc(const LDRelocParam *relocParam)
{
    long addend = 0;
    long operations;
    UINTPTR position = relocParam->position;
    UINTPTR symAddr = relocParam->symAdd;
    BOOL bitsLowOrHigh = FALSE;
    UINT32 instruction;

    instruction = OS_READ_INSTRUCTION_THUMB32(position);
    if (relocParam->shType == LD_SHT_RELA) {
        addend = relocParam->addend;
    } else {
        LD_MoveBitsArea32(instruction, (UINT32 *)(VOID *)&addend,
                          THUMB_MOV_INS1_BIT_FROM, THUMB_MOV_INS1_BIT_TO, THUMB_MOV_INS1_BIT_WIDE);
        LD_MoveBitsArea32(instruction, (UINT32 *)(VOID *)&addend,
                          THUMB_MOV_INS2_BIT_FROM, THUMB_MOV_INS2_BIT_FROM, THUMB_MOV_INS2_BIT_WIDE);
        LD_MoveBitsArea32(instruction, (UINT32 *)(VOID *)&addend,
                          THUMB_MOV_INS3_BIT_FROM, THUMB_MOV_INS3_BIT_TO, THUMB_MOV_INS3_BIT_WIDE);
        LD_MoveBitsArea32(instruction, (UINT32 *)(VOID *)&addend,
                          THUMB_MOV_INS4_BIT_FROM, THUMB_MOV_INS4_BIT_TO, THUMB_MOV_INS4_BIT_WIDE);
    }

    addend = (long)OS_SIGN_EXTEND(addend, THUMB_MOV_BIT_SIGN); /* sign extend */
    operations = (long)(addend + symAddr);

    if (relocParam->relocType == OS_ARM_THM_MOVT_ABS) {
        operations = (long)((unsigned long)operations & THUMB_MOVT_OPERATION_MASK);
        bitsLowOrHigh = 1;
    } else {
        operations = (long)((unsigned long)operations & THUMB_MOV_OPERATION_MASK);
        bitsLowOrHigh = 0;
    }

    LD_MoveBitsArea32((UINTPTR)operations, (UINT32 *)&instruction, THUMB_MOV_INS1_BIT_TO +
                      (THUMB_MOV_BIT_SIGN * bitsLowOrHigh), THUMB_MOV_INS1_BIT_FROM, THUMB_MOV_INS1_BIT_WIDE);
    LD_MoveBitsArea32((UINTPTR)operations, (UINT32 *)&instruction, THUMB_MOV_INS2_BIT_TO +
                      (THUMB_MOV_BIT_SIGN * bitsLowOrHigh), THUMB_MOV_INS2_BIT_FROM, THUMB_MOV_INS2_BIT_WIDE);
    LD_MoveBitsArea32((UINTPTR)operations, (UINT32 *)&instruction, THUMB_MOV_INS3_BIT_TO +
                      (THUMB_MOV_BIT_SIGN * bitsLowOrHigh), THUMB_MOV_INS3_BIT_FROM, THUMB_MOV_INS3_BIT_WIDE);
    LD_MoveBitsArea32((UINTPTR)operations, (UINT32 *)&instruction, THUMB_MOV_INS4_BIT_TO +
                      (THUMB_MOV_BIT_SIGN * bitsLowOrHigh), THUMB_MOV_INS4_BIT_FROM, THUMB_MOV_INS4_BIT_WIDE);
    OS_WRITE_INSTRUCTION_THUMB32(position, instruction);

    return LOS_OK;
}

LITE_OS_SEC_TEXT_MINOR UINT32 ELF_ArmMovwOrMovtReloc(const LDRelocParam *relocParam)
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

    long addend = 0;
    long operations;
    UINTPTR position = relocParam->position;
    UINTPTR symAddr = relocParam->symAdd;
    BOOL bitsLowOrHigh = FALSE;

    if (relocParam->shType == LD_SHT_RELA) {
        addend = relocParam->addend;
    } else {
        LD_MoveBitsArea32(*(UINTPTR *)position, (UINT32 *)(VOID *)&addend,
                          ARM_MOV_INS1_BIT_FROM, ARM_MOV_INS1_BIT_TO, ARM_MOV_INS1_BIT_WIDE);
        LD_MoveBitsArea32(*(UINTPTR *)position, (UINT32 *)(VOID *)&addend,
                          ARM_MOV_INS2_BIT_FROM, ARM_MOV_INS2_BIT_TO, ARM_MOV_INS2_BIT_WIDE);
    }

    addend = (long)OS_SIGN_EXTEND(addend, ARM_MOV_BIT_SIGN); /* sign extend */
    operations = (long)(addend + symAddr);

    if (relocParam->relocType == OS_R_ARM_MOVT_ABS) {
        operations = (long)((unsigned long)operations & ARM_MOVT_OPERATION_MASK);
        bitsLowOrHigh = 1;
    } else {
        operations = (long)((unsigned long)operations & ARM_MOV_OPERATION_MASK);
        bitsLowOrHigh = 0;
    }
    LD_MoveBitsArea32((UINTPTR)operations, (UINT32 *)position, ARM_MOV_INS1_BIT_TO +
                      (ARM_MOV_BIT_SIGN * bitsLowOrHigh), ARM_MOV_INS1_BIT_FROM, ARM_MOV_INS1_BIT_WIDE);
    LD_MoveBitsArea32((UINTPTR)operations, (UINT32 *)position, ARM_MOV_INS2_BIT_TO +
                      (ARM_MOV_BIT_SIGN * bitsLowOrHigh), ARM_MOV_INS2_BIT_FROM, ARM_MOV_INS2_BIT_WIDE);

    return LOS_OK;
}

LITE_OS_SEC_TEXT_MINOR UINT32 ELF_GlobDatReloc(const LDRelocParam *relocParam)
{
    long addend;
    long operations;
    UINTPTR position = relocParam->position;
    UINTPTR symAddr = relocParam->symAdd;

    if (relocParam->shType == LD_SHT_RELA) {
        addend = relocParam->addend;
    } else {
        /* Comments: There is no provision for generating an addend for the dynamic entry. */
        addend = 0;
    }

    operations = (long)((UINTPTR)(symAddr + addend) | relocParam->thumbFunc);

    *(UINT32 *)position = (UINT32)operations;
    return LOS_OK;
}

LITE_OS_SEC_TEXT_MINOR UINT32 ELF_JumpSlotReloc(const LDRelocParam *relocParam)
{
    long addend;
    long operations;
    UINTPTR position = relocParam->position;
    UINTPTR symAddr = relocParam->symAdd;

    if (relocParam->shType == LD_SHT_RELA) {
        addend = relocParam->addend;
    } else {
        /* Comments: In a REL form of this relocation the addend, A, is always 0. */
        addend = 0;
    }

    operations = (long)((UINTPTR)(symAddr + addend) | relocParam->thumbFunc);

    *(UINT32 *)position = (UINT32)operations;
    return LOS_OK;
}

LITE_OS_SEC_TEXT_MINOR UINT32 ELF_ArmRelativeReloc(const LDRelocParam *relocParam)
{
    long addend;
    long operations;
    UINTPTR position = relocParam->position;

    if (relocParam->shType == LD_SHT_RELA) {
        addend = relocParam->addend;
    } else {
        addend = (long)(*(UINT32 *)position);
    }

    /* B(S) + A */
    operations = (long)(relocParam->baseSegment + addend);

    *(UINT32 *)position = (UINT32)operations;
    return LOS_OK;
}

LITE_OS_SEC_TEXT_MINOR UINT32 ArchObjRelocImp(const LD_ELF_RELA *relocCmd, const LDRelocParam *relocParam)
{
    UINT32 ret = LOS_OK;
    switch (LD_ELF_R_TYPE(relocCmd->info)) {
        case OS_R_ARM_NONE: /* None */
            break;
        case OS_R_ARM_PC24: /* Word32, S + A - P */
            ret = ELF_ArmPc24Reloc(relocParam);
            break;
        case OS_R_ARM_ABS32: /* Word32, S + A */
            ret = ELF_ArmAbs32Reloc(relocParam);
            break;
        case OS_R_ARM_REL32: /* Word32, S + A - P */
            ret = ELF_ArmRel32Reloc(relocParam);
            break;
        case OS_R_ARM_THM_CALL: /* BL pair, S - P + A */
        case OS_R_ARM_THM_JUMP24:
            ret = ELF_ThumbPc22Reloc(relocParam);
            break;
        case OS_R_ARM_CALL:
            /* a new reloction without options " -mlong-calls ", need to check if it's correct. */
            ret = ELF_ArmCallReloc(relocParam);
            break;
        case OS_R_ARM_THM_JUMP8: /* Low8, S - P + A */
            ret = ELF_ThumbPc9Reloc(relocParam);
            break;
        case OS_R_ARM_THM_JUMP11: /* Low11, S - P + A */
            ret = ELF_ThumbPc11Reloc(relocParam);
            break;
        case OS_R_ARM_ABS16: /* Word16, S + A */
            ret = ELF_ArmAbs16Reloc(relocParam);
            break;
        case OS_R_ARM_ABS12: /* Low12, S + A */
            ret = ELF_ArmAbs12Reloc(relocParam);
            break;
        case OS_R_ARM_ABS8: /* Byte8, S + A */
            ret = ELF_ArmAbs8Reloc(relocParam);
            break;
        case OS_R_ARM_THM_ABS5: /* 6-10 bits, S + A */
            ret = ELF_ThumbAbs5Reloc(relocParam);
            break;
        case OS_ARM_THM_MOVW_ABS_NC:
        case OS_ARM_THM_MOVT_ABS:
            ret = ELF_ThumbMovwOrMovtReloc(relocParam);
            break;
        case OS_R_ARM_MOVW_ABS_NC:
        case OS_R_ARM_MOVT_ABS:
            ret = ELF_ArmMovwOrMovtReloc(relocParam);
            break;
        case OS_R_ARM_TLS_DESC:
        case OS_R_ARM_THM_SWI8:
        case OS_R_ARM_XPC25:
        case OS_R_ARM_THM_XPC22:
        case OS_R_ARM_BASE_PREL:
        case OS_R_ARM_GOT_BREL:
        case OS_R_ARM_LDR_PC_G0: /* Low12 + U - but, S + A - P */
        case OS_R_ARM_THM_PC8: /* Low8, S - P + A */
            ret = LOS_NOK;
            break;
        default:
            PRINT_ERR("Reloc error, no type %lu\n", LD_ELF_R_TYPE(relocCmd->info));
            ret = LOS_NOK;
            break;
    }
    return ret;
}

LITE_OS_SEC_TEXT_MINOR UINT32 ArchSegRelocImp(const LD_ELF_RELA *rela, const LDRelocParam *relocParam)
{
    UINT32 ret = LOS_OK;
    switch (LD_ELF_R_TYPE(rela->info)) {
        case OS_R_ARM_GLOB_DAT:
            ret = ELF_GlobDatReloc(relocParam);
            break;
        case OS_R_ARM_JUMP_SLOT:
            ret = ELF_JumpSlotReloc(relocParam);
            break;
        case OS_R_ARM_RELATIVE:
            ret = ELF_ArmRelativeReloc(relocParam);
            break;
        case OS_R_ARM_ABS32:
            ret = ELF_ArmAbs32Reloc(relocParam);
            break;
        case OS_R_ARM_REL32:
            ret = ELF_ArmRel32Reloc(relocParam);
            break;
        default:
            PRINT_ERR("reloc error, no type %lu\n", LD_ELF_R_TYPE(rela->info));
            ret = LOS_NOK;
            break;
    }

    return ret;
}

LITE_OS_SEC_TEXT_MINOR UINT32 ArchSegRelocTblCheck(const RelocationInfo *relocInfo)
{
    return (relocInfo->relTable != 0) ? LOS_OK : LOS_NOK;
}
