/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: AARCH64 Dynamic Load handle
 * Author: Huawei LiteOS Team
 * Create: 2020-07-30
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

#include "los_printf.h"
#include "los_ld_reloc_pri.h"

UINT32 AARCH64_ELF_Call26Reloc(const LDRelocParam *relocParam)
{
#define CALL26_BIT_FROM  0U
#define CALL26_BIT_TO    2U
#define CALL26_BIT_WIDE  26U
#define CALL26_BIT_SIGN  28U
#define CALL26_BIT_CHECK 0x03U

    long addend = 0;
    long operations;
    UINTPTR position = relocParam->position;
    UINTPTR symAddr = relocParam->symAdd;

    if (relocParam->shType == LD_SHT_RELA) {
        addend = relocParam->addend;
    } else {
        LD_MoveBitsArea64(*(UINTPTR *)position, (UINTPTR *)(VOID *)&addend,
                          CALL26_BIT_FROM, CALL26_BIT_TO, CALL26_BIT_WIDE);
    }

    operations = (long)(symAddr - position) + addend;
    operations = OS_SIGN_EXTEND(operations, CALL26_BIT_SIGN);
    /* Need to check -2e27 <= operations < 2e27 */
    if (!OS_CHECK_FITS(operations, CALL26_BIT_SIGN)) {
        PRINT_ERR("out of 2^28 range.\n");
        return LOS_NOK;
    }
    if ((UINTPTR)operations & CALL26_BIT_CHECK) {
        PRINT_ERR("the offset should be 4 byte aligned.\n");
        return LOS_NOK;
    }

    LD_MoveBitsArea32((UINTPTR)operations, (UINT32 *)position, CALL26_BIT_TO, CALL26_BIT_FROM, CALL26_BIT_WIDE);
    return LOS_OK;
}

UINT32 AARCH64_ELF_Abs64Reloc(const LDRelocParam *relocParam)
{
    long addend;
    long operations;
    UINTPTR position = relocParam->position;
    UINTPTR symAddr = relocParam->symAdd;

    if (relocParam->shType == LD_SHT_RELA) {
        addend = relocParam->addend;
    } else {
        addend = *(UINTPTR *)position;
    }

    operations = (long)(symAddr + addend);

    *(UINTPTR *)position = (UINTPTR)operations;
    return LOS_OK;
}


UINT32 AARCH64_ELF_Prel32Reloc(const LDRelocParam *relocParam)
{
#define PREL32_BIT_FROM  0U
#define PREL32_BIT_TO    0U
#define PREL32_BIT_WIDE  32U
#define PREL32_BIT_BOUND_LOWER  0xFFFFFFFF80000000 // (-0x80000000)
#define PREL32_BIT_BOUND_UPER   0x100000000

    long addend = 0;
    long operations;
    UINTPTR position = relocParam->position;
    UINTPTR symAddr = relocParam->symAdd;

    if (relocParam->shType == LD_SHT_RELA) {
        addend = relocParam->addend;
    } else {
        LD_MoveBitsArea64(*(UINTPTR *)position, (UINTPTR *)(VOID *)&addend,
                          PREL32_BIT_FROM, PREL32_BIT_TO, PREL32_BIT_WIDE);
    }

    operations = (long)((UINTPTR)(symAddr + addend) - position);
    /* Need to check -2^31 <= operations < 2^32 */
    if ((operations >= (long)PREL32_BIT_BOUND_UPER) || (operations < (long)PREL32_BIT_BOUND_LOWER)) {
        PRINT_ERR("out of range -2^31 <= operations < 2^32. operations = %lx.\n", operations);
        return LOS_NOK;
    }

    LD_MoveBitsArea64((UINTPTR)operations, (UINTPTR *)position,
                      PREL32_BIT_TO, PREL32_BIT_FROM, PREL32_BIT_WIDE);
    return LOS_OK;
}

UINT32 AARCH64_ELF_AdrPrelPgHi21Reloc(const LDRelocParam *relocParam)
{
#define HI21_INS1_BIT_FROM  29U
#define HI21_INS1_BIT_TO    12U
#define HI21_INS1_BIT_WIDE  2U
#define HI21_INS2_BIT_FROM  5U
#define HI21_INS2_BIT_TO    14U
#define HI21_INS2_BIT_WIDE  19U
#define HI21_BIT_SIGN       33U

    long addend = 0;
    long operations;
    UINTPTR position = relocParam->position;
    UINTPTR symAddr = relocParam->symAdd;

    if (relocParam->shType == LD_SHT_RELA) {
        addend = relocParam->addend;
    } else {
        LD_MoveBitsArea64(*(UINTPTR *)position, (UINTPTR *)(VOID *)&addend,
                          HI21_INS1_BIT_FROM, HI21_INS1_BIT_TO, HI21_INS1_BIT_WIDE);
        LD_MoveBitsArea64(*(UINTPTR *)position, (UINTPTR *)(VOID *)&addend,
                          HI21_INS2_BIT_FROM, HI21_INS2_BIT_TO, HI21_INS2_BIT_WIDE);
    }

    operations = (long)(OS_GET_PAGE_ADDR(symAddr + addend) - OS_GET_PAGE_ADDR(position));
    operations = (long)(OS_SIGN_EXTEND(operations, HI21_BIT_SIGN));
    /* Need to check -2e32 <= operations < 2e32 */
    if (!OS_CHECK_FITS(operations, HI21_BIT_SIGN)) {
        PRINT_ERR("out of 2^33 range.\n");
        return LOS_NOK;
    }

    LD_MoveBitsArea32((UINTPTR)operations, (UINT32 *)position,
                      HI21_INS1_BIT_TO, HI21_INS1_BIT_FROM, HI21_INS1_BIT_WIDE);
    LD_MoveBitsArea32((UINTPTR)operations, (UINT32 *)position,
                      HI21_INS2_BIT_TO, HI21_INS2_BIT_FROM, HI21_INS2_BIT_WIDE);
    return LOS_OK;
}

UINT32 AARCH64_ELF_Ldst32AbsLo12NcReloc(const LDRelocParam *relocParam)
{
#define LDST32_BIT_FROM  10U
#define LDST32_BIT_TO    2U
#define LDST32_BIT_WIDE  10U

    long addend = 0;
    long operations;
    UINTPTR position = relocParam->position;
    UINTPTR symAddr = relocParam->symAdd;

    if (relocParam->shType == LD_SHT_RELA) {
        addend = relocParam->addend;
    } else {
        LD_MoveBitsArea64(*(UINTPTR *)position, (UINTPTR *)(VOID *)&addend,
                          LDST32_BIT_FROM, LDST32_BIT_TO, LDST32_BIT_WIDE);
    }

    operations = (long)(symAddr + addend);
    /* ZeroExtend */
    LD_MoveBitsArea32((UINTPTR)operations, (UINT32 *)position, LDST32_BIT_TO, LDST32_BIT_FROM, LDST32_BIT_WIDE);
    return LOS_OK;
}

UINT32 AARCH64_ELF_AddAbsLo12NcReloc(const LDRelocParam *relocParam)
{
#define ADD_ABS_BIT_FROM  10U
#define ADD_ABS_BIT_TO    0U
#define ADD_ABS_BIT_WIDE  12U

    long addend = 0;
    long operations;
    UINTPTR position = relocParam->position;
    UINTPTR symAddr = relocParam->symAdd;

    if (relocParam->shType == LD_SHT_RELA) {
        addend = relocParam->addend;
    } else {
        LD_MoveBitsArea64(*(UINTPTR *)position, (UINTPTR *)(VOID *)&addend,
                          ADD_ABS_BIT_FROM, ADD_ABS_BIT_TO, ADD_ABS_BIT_WIDE);
    }

    operations = (long)(symAddr + addend);
    /* ZeroExtend */
    LD_MoveBitsArea32((UINTPTR)operations, (UINT32 *)position, ADD_ABS_BIT_TO, ADD_ABS_BIT_FROM, ADD_ABS_BIT_WIDE);
    return LOS_OK;
}

UINT32 AARCH64_ELF_Ldst8AbsLo12NcReloc(const LDRelocParam *relocParam)
{
#define LDST8_BIT_FROM  10U
#define LDST8_BIT_TO    0U
#define LDST8_BIT_WIDE  12U

    long addend = 0;
    long operations;
    UINTPTR position = relocParam->position;
    UINTPTR symAddr = relocParam->symAdd;

    if (relocParam->shType == LD_SHT_RELA) {
        addend = relocParam->addend;
    } else {
        LD_MoveBitsArea64(*(UINTPTR *)position, (UINTPTR *)(VOID *)&addend,
                          LDST8_BIT_FROM, LDST8_BIT_TO, LDST8_BIT_WIDE);
    }

    operations = (long)(symAddr + addend);
    /* ZeroExtend */
    LD_MoveBitsArea32((UINTPTR)operations, (UINT32 *)position, LDST8_BIT_TO, LDST8_BIT_FROM, LDST8_BIT_WIDE);
    return LOS_OK;
}

UINT32 AARCH64_ELF_Ldst64AbsLo12NcReloc(const LDRelocParam *relocParam)
{
#define LDST64_BIT_FROM  10U
#define LDST64_BIT_TO    3U
#define LDST64_BIT_WIDE  9U

    long addend = 0;
    long operations;
    UINTPTR position = relocParam->position;
    UINTPTR symAddr = relocParam->symAdd;

    if (relocParam->shType == LD_SHT_RELA) {
        addend = relocParam->addend;
    } else {
        LD_MoveBitsArea64(*(UINTPTR *)position, (UINTPTR *)(VOID *)&addend,
                          LDST64_BIT_FROM, LDST64_BIT_TO, LDST64_BIT_WIDE);
    }

    operations = (long)(symAddr + addend);
    /* ZeroExtend */
    LD_MoveBitsArea32((UINTPTR)operations, (UINT32 *)position, LDST64_BIT_TO, LDST64_BIT_FROM, LDST64_BIT_WIDE);
    return LOS_OK;
}

UINT32 AARCH64_ELF_Ldst16AbsLo12NcReloc(const LDRelocParam *relocParam)
{
#define LDST16_BIT_FROM  10U
#define LDST16_BIT_TO    1U
#define LDST16_BIT_WIDE  11U

    long addend = 0;
    long operations;
    UINTPTR position = relocParam->position;
    UINTPTR symAddr = relocParam->symAdd;

    if (relocParam->shType == LD_SHT_RELA) {
        addend = relocParam->addend;
    } else {
        LD_MoveBitsArea64(*(UINTPTR *)position, (UINTPTR *)(VOID *)&addend,
                          LDST16_BIT_FROM, LDST16_BIT_TO, LDST16_BIT_WIDE);
    }

    operations = (long)(symAddr + addend);
    /* ZeroExtend */
    LD_MoveBitsArea32((UINTPTR)operations, (UINT32 *)position, LDST16_BIT_TO, LDST16_BIT_FROM, LDST16_BIT_WIDE);
    return LOS_OK;
}

UINT32 AARCH64_ELF_GlobDatReloc(const LDRelocParam *relocParam)
{
    long addend;
    long operations;
    UINTPTR position = relocParam->position;
    UINTPTR symAddr = relocParam->symAdd;

    if (relocParam->shType == LD_SHT_RELA) {
        addend = relocParam->addend;
    } else {
        addend = *(UINTPTR *)position;
    }

    operations = (long)(symAddr + addend);

    *(UINTPTR *)position = operations;

    return LOS_OK;
}

UINT32 AARCH64_ELF_JumpSlotReloc(const LDRelocParam *relocParam)
{
    long addend;
    long operations;
    UINTPTR position = relocParam->position;
    UINTPTR symAddr = relocParam->symAdd;

    if (relocParam->shType == LD_SHT_RELA) {
        addend = relocParam->addend;
    } else {
        /*
         * Comments:
         * Because the initial value of the place is not related to the ultimate target
         * of a R_AARCH64_JUMP_SLOT relocation, the addend A of such a REL-type relocation
         * shall be zero rather than the initial content of the place.
         */
        addend = 0;
    }

    operations = (long)(symAddr + addend);

    *(UINTPTR *)position = operations;

    return LOS_OK;
}

UINT32 AARCH64_ELF_RelativeReloc(const LDRelocParam *relocParam)
{
    long addend;
    UINTPTR position = relocParam->position;

    if (relocParam->shType == LD_SHT_RELA) {
        addend = relocParam->addend;
    } else {
        addend = *(UINTPTR *)position;
    }

    *(UINTPTR *)position = relocParam->baseSegment + addend;

    return LOS_OK;
}

LITE_OS_SEC_TEXT_MINOR UINT32 ArchObjRelocImp(const LD_ELF_RELA *relocCmd, const LDRelocParam *relocParam)
{
    UINT32 ret;
    switch (LD_ELF_R_TYPE(relocCmd->info)) {
        case OS_R_ARM_AARCH64_ABS64:
            ret = AARCH64_ELF_Abs64Reloc(relocParam);
            break;
        case OS_R_ARM_AARCH64_PREL32: /* 261 */
            ret = AARCH64_ELF_Prel32Reloc(relocParam);
            break;
        case OS_R_ARM_AARCH64_ADR_PREL_PG_HI21: /* 275 */
            ret = AARCH64_ELF_AdrPrelPgHi21Reloc(relocParam);
            break;
        case OS_R_ARM_AARCH64_ADD_ABS_LO12_NC: /* 277 */
            ret = AARCH64_ELF_AddAbsLo12NcReloc(relocParam);
            break;
        case OS_R_ARM_AARCH64_LDST8_ABS_LO12_NC:
            ret = AARCH64_ELF_Ldst8AbsLo12NcReloc(relocParam);
            break;
        case OS_R_ARM_AARCH64_CALL26:
            ret = AARCH64_ELF_Call26Reloc(relocParam);
            break;
        case OS_R_ARM_AARCH64_LDST16_ABS_LO12_NC:
            ret = AARCH64_ELF_Ldst16AbsLo12NcReloc(relocParam);
            break;
        case OS_R_ARM_AARCH64_LDST32_ABS_LO12_NC:
            ret = AARCH64_ELF_Ldst32AbsLo12NcReloc(relocParam);
            break;
        case OS_R_ARM_AARCH64_LDST64_ABS_LO12_NC:
            ret = AARCH64_ELF_Ldst64AbsLo12NcReloc(relocParam);
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
        case OS_R_ARM_AARCH64_ABS64:  /* S + A */
            ret = AARCH64_ELF_Abs64Reloc(relocParam);
            break;
        case OS_R_ARM_AARCH64_GLOB_DAT:  /* S + A */
            ret = AARCH64_ELF_GlobDatReloc(relocParam);
            break;
        case OS_R_ARM_AARCH64_JUMP_SLOT:  /* S + A */
            ret = AARCH64_ELF_JumpSlotReloc(relocParam);
            break;
        case OS_R_ARM_AARCH64_RELATIVE:
            ret = AARCH64_ELF_RelativeReloc(relocParam);
            break;
        case OS_R_ARM_AARCH64_TLS_DTPREL64:
        case OS_R_ARM_AARCH64_TLS_DTPMOD64:
        case OS_R_ARM_AARCH64_TLS_TPREL64:
        case OS_R_ARM_AARCH64_TLSDESC:
        case OS_R_ARM_AARCH64_IRELATIVE:
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
