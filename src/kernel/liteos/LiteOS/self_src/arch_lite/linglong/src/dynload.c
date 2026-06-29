/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: LingLong Dynamic Load handle
 * Author: Huawei LiteOS Team
 * Create: 2022-08-31
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
#include <stdint.h>
#include "los_base.h"
#include "los_ld_reloc_pri.h"

#define INST_F16_SUFFIX_MASK                    0x4000U
#define INST_F24_SUFFIX_MASK                    0x6000U
#define INST_F32_SUFFIX_MASK                    0x7000U
#define INST_F48_SUFFIX_MASK                    0x7800U

#define INST_F16_SUFFIX                         0x0000U
#define INST_F24_SUFFIX                         0x4000U
#define INST_F32_SUFFIX                         0x6000U
#define INST_F48_SUFFIX                         0x7000U

#define INST_EOV_MASK                           0x8000U

#define ARRAY_SIZE(arr)                         ((UINT32)sizeof((arr)) / (UINT32)sizeof((arr)[0]))
#define BITS_PER_BYTE                           8
#define BYTE_MASK                               0xFF

typedef struct {
    UINT8 code[INST_MAX_LEN];
    UINT8 mask[INST_MAX_LEN];
    UINT8 encoderDesc;
} InstRelocInfo;


typedef struct {
    const InstRelocInfo *relTbl;
    UINT32 tblLen;
    UINT32 slot;
    UINTPTR val;
    UINT32 type;
} RelocAuxParam;

#define IMM_UNSIGNED            0
#define IMM_SIGNED              1

/**
 * bit0              EOV                                    bit47
 *  ↓                 ↓                                       ↓
 *  ----,----,xxx-,----,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,x---
 * Relocate encodings. The value should set to bits indicated by x, the
 * other bits should be left as-is. Please refers to the linglong core xml
 * for more information.
 */
STATIC const InstEncoder g_encoders[] = {
    /* ----,----,xxx-,----,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,x--- */
    [TRG__X__U32_D__X__F48] = {8, 3, 0, 29, 0, IMM_UNSIGNED},
    /* ----,----,----,----,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx */
    [TRG__X__U32__X__F48] = {0, 0, 0, 32, 0, IMM_UNSIGNED},
    /* ----,----,--x-,----,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxx- */
    [TRG__X__U32_B__X__F48] = {10, 1, 0, 31, 0, IMM_UNSIGNED},
    /* ----,---x,xxx-,----,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxx-,---- */
    [U31_SHIFT1_B__X__F48] = {7, 4, 0, 27, 1, IMM_UNSIGNED},
    /* ----,----,xxx-,----,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxx-,---- */
    [U30_SHIFT2_B__X__F48] = {8, 3, 0, 27, 2, IMM_UNSIGNED},
    /* ----,--xx,xxx-,----,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxx-,---- */
    [U32_B__X__F48] = {6, 5, 0, 27, 0, IMM_UNSIGNED},
    /* ----,--xx,xxx-,----,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxx-,---- */
    [M32__X__F48] = {6, 5, 0, 27, 0, IMM_UNSIGNED},
    /* ----,-xxx,xxxx,xx-- */
    [TRG__X__REL_S9__X__F16] = {5, 9, 0, 0, 0, IMM_SIGNED},
    /* ----,---x,xxxx,x---,xxxx,xxxx */
    [TRG__X__REL_S14__X__F24] = {7, 6, 0, 8, 0, IMM_SIGNED},
    /* ----,--xx,xxxx,----,xxxx,xxxx,xxxx,xxxx */
    [TRG__X__REL_S22__X__F32] = {6, 6, 0, 16, 0, IMM_SIGNED},
    /* ----,--xx,xxxx,x---,xxx-,---- */
    [TRG__X__REL_S10_C__X__F24] = {6, 7, 0, 3, 0, IMM_SIGNED},
    /* ----,---x,xxxx,----,xxxx,xxxx,xxx-,---- */
    [TRG__X__REL_S16_C__X__F32] = {7, 5, 0, 11, 0, IMM_SIGNED},
    /* ----,----,----,----,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx */
    [TRG__X__REL_S32__X__F48] = {0, 0, 0, 32, 0, IMM_SIGNED},
    /* ----,----,----,----,----,xxxx,xxxx,xxxx */
    [TRG__X__REL_U12__X__F32] = {0, 0, 4, 12, 0, IMM_UNSIGNED},
    /* ----,----,----,----,----,----,----,----,---x,xxxx,xxxx,xxxx */
    [TRG__X__REL_U13__X__F48] = {0, 0, 19, 13, 0, IMM_UNSIGNED},
    /* ----,----,----,----,----,xxxx,xxxx,xxx-,----,----,----,---- */
    [REL_HWL_U11_BEG__X__F48] = {0, 0, 4, 11, 0, IMM_UNSIGNED},
    /* ----,----,----,----,----,----,----,---x,xxxx,xxxx,xx--,---- */
    [REL_HWL_U11_END__X__F48] = {0, 0, 15, 11, 0, IMM_UNSIGNED},
    /* ----,----,----,----,-xxx,xxxx,xxxx,----,----,----,----,---- */
    [REL_HWL_U11_B_BEG__X__F48] = {0, 0, 1, 11, 0, IMM_UNSIGNED},
    /* ----,----,----,----,----,----,----,xxxx,xxxx,xxx-,----,---- */
    [REL_HWL_U11_B_END__X__F48] = {0, 0, 12, 11, 0, IMM_UNSIGNED},
};

STATIC const InstRelocInfo g_abs32RelocInfoTbl[] = {
    /* call.intra.val@pcu.f48 0000,010-,---0,1110,----,----,----,----,----,----,----,---- */
    {{0x20, 0x70, 0x00, 0x00, 0x00, 0x00}, {0x7f, 0x78, 0x00, 0x00, 0x00, 0x00}, TRG__X__U32_D__X__F48},
    /* call.intra.val@pcu.f48 0000,0010,---0,1110,----,----,----,----,----,----,----,---- */
    {{0x40, 0x70, 0x00, 0x00, 0x00, 0x00}, {0xff, 0x78, 0x00, 0x00, 0x00, 0x00}, TRG__X__U32_D__X__F48},
    /* call.val@pcu.f48 0000,011-,---0,1110,----,----,----,----,----,----,----,---- */
    {{0x60, 0x70, 0x00, 0x00, 0x00, 0x00}, {0x7f, 0x78, 0x00, 0x00, 0x00, 0x00}, TRG__X__U32_D__X__F48},
    /* call.val@pcu.f48 0000,0011,---0,1110,----,----,----,----,----,----,----,---- */
    {{0xc0, 0x70, 0x00, 0x00, 0x00, 0x00}, {0xff, 0x78, 0x00, 0x00, 0x00, 0x00}, TRG__X__U32_D__X__F48},
    /* j@pcu.f48 0000,0000,1110,1110,----,----,----,----,----,----,----,---- */
    {{0x00, 0x77, 0x00, 0x00, 0x00, 0x00}, {0xff, 0x7f, 0x00, 0x00, 0x00, 0x00}, TRG__X__U32__X__F48},
    /* j.cf@pcu.f48 0000,0000,0110,1110,----,----,----,----,----,----,----,---- */
    {{0x00, 0x76, 0x00, 0x00, 0x00, 0x00}, {0xff, 0x7f, 0x00, 0x00, 0x00, 0x00}, TRG__X__U32__X__F48},
    /* j.ct@pcu.f48 0000,0000,1000,1110,----,----,----,----,----,----,----,---- */
    {{0x00, 0x71, 0x00, 0x00, 0x00, 0x00}, {0xff, 0x7f, 0x00, 0x00, 0x00, 0x00}, TRG__X__U32__X__F48},
    /* j.intra.cf@pcu.f48 0000,0000,1010,1110,----,----,----,----,----,----,----,---- */
    {{0x00, 0x75, 0x00, 0x00, 0x00, 0x00}, {0xff, 0x7f, 0x00, 0x00, 0x00, 0x00}, TRG__X__U32__X__F48},
    /* j.intra.ct@pcu.f48 0000,0000,1100,1110,----,----,----,----,----,----,----,---- */
    {{0x00, 0x73, 0x00, 0x00, 0x00, 0x00}, {0xff, 0x7f, 0x00, 0x00, 0x00, 0x00}, TRG__X__U32__X__F48},
    /* lbeginit@pcu.f48 0000,0001,00-0,1110,----,----,----,----,----,----,----,---- */
    {{0x80, 0x70, 0x00, 0x00, 0x00, 0x00}, {0xff, 0x7b, 0x00, 0x00, 0x00, 0x00}, TRG__X__U32_B__X__F48},
    /* ld.a16.u16.aa@agx.f48 0000,110-,---0,1110,----,----,----,----,----,----,----,---- */
    {{0x30, 0x70, 0x00, 0x00, 0x00, 0x00}, {0x7f, 0x78, 0x00, 0x00, 0x00, 0x00}, U31_SHIFT1_B__X__F48},
    /* ld.a32.aa@agx.f48 0000,1001,---0,1110,----,----,----,----,----,----,----,---- */
    {{0x90, 0x70, 0x00, 0x00, 0x00, 0x00}, {0xff, 0x78, 0x00, 0x00, 0x00, 0x00}, U30_SHIFT2_B__X__F48},
    /* ld.a8.u8.aa@agx.f48 0001,00--,---0,1110,----,----,----,----,----,----,----,---- */
    {{0x08, 0x70, 0x00, 0x00, 0x00, 0x00}, {0x3f, 0x78, 0x00, 0x00, 0x00, 0x00}, U32_B__X__F48},
    /* lendinit@pcu.f48 0000,0001,01-0,1110,----,----,----,----,----,----,----,---- */
    {{0x80, 0x72, 0x00, 0x00, 0x00, 0x00}, {0xff, 0x7b, 0x00, 0x00, 0x00, 0x00}, TRG__X__U32_B__X__F48},
    /* mov.i@agx.f48 0001,01--,---0,1110,----,----,----,----,----,----,----,---- */
    {{0x28, 0x70, 0x00, 0x00, 0x00, 0x00}, {0x3f, 0x78, 0x00, 0x00, 0x00, 0x00}, M32__X__F48},
    /* mov.i@sau.f48 0010,00--,---0,1110,----,----,----,----,----,----,----,---- */
    {{0x04, 0x70, 0x00, 0x00, 0x00, 0x00}, {0x3f, 0x78, 0x00, 0x00, 0x00, 0x00}, M32__X__F48},
    /* st.a16.aa@agx.f48 0000,111-,---0,1110,----,----,----,----,----,----,----,---- */
    {{0x70, 0x70, 0x00, 0x00, 0x00, 0x00}, {0x7f, 0x78, 0x00, 0x00, 0x00, 0x00}, U31_SHIFT1_B__X__F48},
    /* st.a32.aa@agx.f48 0000,1010,---0,1110,----,----,----,----,----,----,----,---- */
    {{0x50, 0x70, 0x00, 0x00, 0x00, 0x00}, {0xff, 0x78, 0x00, 0x00, 0x00, 0x00}, U30_SHIFT2_B__X__F48},
    /* st.a8.aa@agx.f48 0001,10--,---0,1110,----,----,----,----,----,----,----,---- */
    {{0x18, 0x70, 0x00, 0x00, 0x00, 0x00}, {0x3f, 0x78, 0x00, 0x00, 0x00, 0x00}, U32_B__X__F48},
};

STATIC const InstRelocInfo g_pcRelInfoTbl[] = {
    /* b@pcu.f16 0010,1---,----,--00 */
    {{0x14, 0x00, 0x00, 0x00, 0x00, 0x00}, {0x1f, 0x40, 0x00, 0x00, 0x00, 0x00}, TRG__X__REL_S9__X__F16},
    /* b@pcu.f24 0000,101-,----,-010,----,---- */
    {{0x50, 0x40, 0x00, 0x00, 0x00, 0x00}, {0x7f, 0x60, 0x00, 0x00, 0x00, 0x00}, TRG__X__REL_S14__X__F24},
    /* b@pcu.f32 0001,11--,----,0110,----,----,----,---- */
    {{0x38, 0x60, 0x00, 0x00, 0x00, 0x00}, {0x3f, 0x70, 0x00, 0x00, 0x00, 0x00}, TRG__X__REL_S22__X__F32},
    /* b.cf@pcu.f16 0000,1---,----,--00 */
    {{0x10, 0x00, 0x00, 0x00, 0x00, 0x00}, {0x1f, 0x40, 0x00, 0x00, 0x00, 0x00}, TRG__X__REL_S9__X__F16},
    /* b.cf@pcu.f24 0000,001-,----,-010,----,---- */
    {{0x40, 0x40, 0x00, 0x00, 0x00, 0x00}, {0x7f, 0x60, 0x00, 0x00, 0x00, 0x00}, TRG__X__REL_S14__X__F24},
    /* b.cf@pcu.f32 0000,11--,----,0110,----,----,----,---- */
    {{0x30, 0x60, 0x00, 0x00, 0x00, 0x00}, {0x3f, 0x70, 0x00, 0x00, 0x00, 0x00}, TRG__X__REL_S22__X__F32},
    /* b.ct@pcu.f16 0001,0---,----,--00 */
    {{0x08, 0x00, 0x00, 0x00, 0x00, 0x00}, {0x1f, 0x40, 0x00, 0x00, 0x00, 0x00}, TRG__X__REL_S9__X__F16},
    /* b.ct@pcu.f24 0000,010-,----,-010,----,---- */
    {{0x20, 0x40, 0x00, 0x00, 0x00, 0x00}, {0x7f, 0x60, 0x00, 0x00, 0x00, 0x00}, TRG__X__REL_S14__X__F24},
    /* b.ct@pcu.f32 0001,00--,----,0110,----,----,----,---- */
    {{0x08, 0x60, 0x00, 0x00, 0x00, 0x00}, {0x3f, 0x70, 0x00, 0x00, 0x00, 0x00}, TRG__X__REL_S22__X__F32},
    /* b.eqz@pcu.f24 0001,00--,----,-010,----,---- */
    {{0x08, 0x40, 0x00, 0x00, 0x00, 0x00}, {0x3f, 0x60, 0x00, 0x00, 0x00, 0x00}, TRG__X__REL_S10_C__X__F24},
    /* b.eqz@pcu.f32 0000,001-,----,0110,----,----,----,---- */
    {{0x40, 0x60, 0x00, 0x00, 0x00, 0x00}, {0x7f, 0x70, 0x00, 0x00, 0x00, 0x00}, TRG__X__REL_S16_C__X__F32},
    /* b.intra.cf@pcu.f16 0001,1---,----,--00 */
    {{0x18, 0x00, 0x00, 0x00, 0x00, 0x00}, {0x1f, 0x40, 0x00, 0x00, 0x00, 0x00}, TRG__X__REL_S9__X__F16},
    /* b.intra.cf@pcu.f24 0000,011-,----,-010,----,---- */
    {{0x60, 0x40, 0x00, 0x00, 0x00, 0x00}, {0x7f, 0x60, 0x00, 0x00, 0x00, 0x00}, TRG__X__REL_S14__X__F24},
    /* b.intra.cf@pcu.f32 0001,01--,----,0110,----,----,----,---- */
    {{0x28, 0x60, 0x00, 0x00, 0x00, 0x00}, {0x3f, 0x70, 0x00, 0x00, 0x00, 0x00}, TRG__X__REL_S22__X__F32},
    /* b.intra.ct@pcu.f16 0010,0---,----,--00 */
    {{0x04, 0x00, 0x00, 0x00, 0x00, 0x00}, {0x1f, 0x40, 0x00, 0x00, 0x00, 0x00}, TRG__X__REL_S9__X__F16},
    /* b.intra.ct@pcu.f24 0000,100-,----,-010,----,---- */
    {{0x10, 0x40, 0x00, 0x00, 0x00, 0x00}, {0x7f, 0x60, 0x00, 0x00, 0x00, 0x00}, TRG__X__REL_S14__X__F24},
    /* b.intra.ct@pcu.f32 0001,10--,----,0110,----,----,----,---- */
    {{0x18, 0x60, 0x00, 0x00, 0x00, 0x00}, {0x3f, 0x70, 0x00, 0x00, 0x00, 0x00}, TRG__X__REL_S22__X__F32},
    /* b.intra.eqz@pcu.f24 0001,01--,----,-010,----,---- */
    {{0x28, 0x40, 0x00, 0x00, 0x00, 0x00}, {0x3f, 0x60, 0x00, 0x00, 0x00, 0x00}, TRG__X__REL_S10_C__X__F24},
    /* b.intra.eqz@pcu.f32 0000,010-,----,0110,----,----,----,---- */
    {{0x20, 0x60, 0x00, 0x00, 0x00, 0x00}, {0x7f, 0x70, 0x00, 0x00, 0x00, 0x00}, TRG__X__REL_S16_C__X__F32},
    /* b.intra.neqz@pcu.f24 0001,10--,----,-010,----,---- */
    {{0x18, 0x40, 0x00, 0x00, 0x00, 0x00}, {0x3f, 0x60, 0x00, 0x00, 0x00, 0x00}, TRG__X__REL_S10_C__X__F24},
    /* b.intra.neqz@pcu.f32 0000,011-,----,0110,----,----,----,---- */
    {{0x60, 0x60, 0x00, 0x00, 0x00, 0x00}, {0x7f, 0x70, 0x00, 0x00, 0x00, 0x00}, TRG__X__REL_S16_C__X__F32},
    /* b.neqz@pcu.f24 0001,11--,----,-010,----,---- */
    {{0x38, 0x40, 0x00, 0x00, 0x00, 0x00}, {0x3f, 0x60, 0x00, 0x00, 0x00, 0x00}, TRG__X__REL_S10_C__X__F24},
    /* b.neqz@pcu.f32 0000,100-,----,0110,----,----,----,---- */
    {{0x10, 0x60, 0x00, 0x00, 0x00, 0x00}, {0x7f, 0x70, 0x00, 0x00, 0x00, 0x00}, TRG__X__REL_S16_C__X__F32},
    /* call@pcu.f24 0000,111-,----,-010,----,---- */
    {{0x70, 0x40, 0x00, 0x00, 0x00, 0x00}, {0x7f, 0x60, 0x00, 0x00, 0x00, 0x00}, TRG__X__REL_S14__X__F24},
    /* call@pcu.f32 0010,01--,----,0110,----,----,----,---- */
    {{0x24, 0x60, 0x00, 0x00, 0x00, 0x00}, {0x3f, 0x70, 0x00, 0x00, 0x00, 0x00}, TRG__X__REL_S22__X__F32},
    /* call@pcu.f48 0000,0000,0100,1110,----,----,----,----,----,----,----,---- */
    {{0x00, 0x72, 0x00, 0x00, 0x00, 0x00}, {0xff, 0x7f, 0x00, 0x00, 0x00, 0x00}, TRG__X__REL_S32__X__F48},
    /* call.intra@pcu.f24 0000,110-,----,-010,----,---- */
    {{0x30, 0x40, 0x00, 0x00, 0x00, 0x00}, {0x7f, 0x60, 0x00, 0x00, 0x00, 0x00}, TRG__X__REL_S14__X__F24},
    /* call.intra@pcu.f32 0010,00--,----,0110,----,----,----,---- */
    {{0x04, 0x60, 0x00, 0x00, 0x00, 0x00}, {0x3f, 0x70, 0x00, 0x00, 0x00, 0x00}, TRG__X__REL_S22__X__F32},
    /* call.intra@pcu.f48 0000,0000,0010,1110,----,----,----,----,----,----,----,---- */
    {{0x00, 0x74, 0x00, 0x00, 0x00, 0x00}, {0xff, 0x7f, 0x00, 0x00, 0x00, 0x00}, TRG__X__REL_S32__X__F48},
    /* lbreak.cf@pcu.f32 0000,0000,0000,0110,0001,----,----,---- */
    {{0x00, 0x60, 0x08, 0x00, 0x00, 0x00}, {0xff, 0x7f, 0x0f, 0x00, 0x00, 0x00}, TRG__X__REL_U12__X__F32},
    /* lbreak.ct@pcu.f32 0000,0000,0000,0110,0010,----,----,---- */
    {{0x00, 0x60, 0x04, 0x00, 0x00, 0x00}, {0xff, 0x7f, 0x0f, 0x00, 0x00, 0x00}, TRG__X__REL_U12__X__F32},
    /* lcont.cf@pcu.f32 0000,0000,0000,0110,0011,----,----,---- */
    {{0x00, 0x60, 0x0c, 0x00, 0x00, 0x00}, {0xff, 0x7f, 0x0f, 0x00, 0x00, 0x00}, TRG__X__REL_U12__X__F32},
    /* lcont.ct@pcu.f32 0000,0000,0000,0110,0100,----,----,---- */
    {{0x00, 0x60, 0x02, 0x00, 0x00, 0x00}, {0xff, 0x7f, 0x0f, 0x00, 0x00, 0x00}, TRG__X__REL_U12__X__F32},
    /* lskip.eqz@pcu.f48 0000,0000,0000,1110,0000,0000,0000,0000,000-,----,----,---- */
    {{0x00, 0x70, 0x00, 0x00, 0x00, 0x00}, {0xff, 0x7f, 0xff, 0xff, 0x07, 0x00}, TRG__X__REL_U13__X__F48},
    /* lskip.leqz@pcu.f48 0000,0000,0000,1110,0000,0000,0000,0000,001-,----,----,---- */
    {{0x00, 0x70, 0x00, 0x00, 0x04, 0x00}, {0xff, 0x7f, 0xff, 0xff, 0x07, 0x00}, TRG__X__REL_U13__X__F48},
};

STATIC const InstRelocInfo g_hwloopBegInfoTbl[] = {
    /* lallinit.a@pcu.f48 0000,0000,0000,1110,0001,----,----,----,----,----,----,---- */
    {{0x00, 0x70, 0x08, 0x00, 0x00, 0x00}, {0xff, 0x7f, 0x0f, 0x00, 0x00, 0x00}, REL_HWL_U11_BEG__X__F48},
    /* lallinit.i@pcu.f48 0000,0000,0000,1110,1---,----,----,----,----,----,----,---- */
    {{0x00, 0x70, 0x01, 0x00, 0x00, 0x00}, {0xff, 0x7f, 0x01, 0x00, 0x00, 0x00}, REL_HWL_U11_B_BEG__X__F48},
};

STATIC const InstRelocInfo g_hwloopEndInfoTbl[] = {
    /* lallinit.a@pcu.f48 0000,0000,0000,1110,0001,----,----,----,----,----,----,---- */
    {{0x00, 0x70, 0x08, 0x00, 0x00, 0x00}, {0xff, 0x7f, 0x0f, 0x00, 0x00, 0x00}, REL_HWL_U11_END__X__F48},
    /* lallinit.i@pcu.f48 0000,0000,0000,1110,1---,----,----,----,----,----,----,---- */
    {{0x00, 0x70, 0x01, 0x00, 0x00, 0x00}, {0xff, 0x7f, 0x01, 0x00, 0x00, 0x00}, REL_HWL_U11_B_END__X__F48},
};

STATIC INLINE UINT16 ReverseByteOrderU16(UINT16 val)
{
    return ((UINT16)(((UINT32)(val) << BITS_PER_BYTE) | ((UINT32)(val) >> BITS_PER_BYTE)));
}

STATIC INLINE BOOL IsOddAddr(const UINT8 *ptr)
{
    return (((UINTPTR)ptr & 0x01) == 0x01) ? TRUE : FALSE;
}

STATIC INLINE UINT16 ArchParityEncodeRead(const UINT8 *src)
{
    UINT16 val = (UINT16)((src[1] << BITS_PER_BYTE) | src[0]);
    if (IsOddAddr(src) == TRUE) {
        val = ReverseByteOrderU16(val);
    }
    return val;
}

STATIC INLINE VOID ArchParityEncodeWrite(UINT8 *dest, UINT16 val)
{
    if (IsOddAddr(dest) == TRUE) {
        val = ReverseByteOrderU16(val);
    }
    dest[0] = val & BYTE_MASK;
    dest[1] = (val >> BITS_PER_BYTE) & BYTE_MASK;
}

STATIC INLINE UINT32 ArchRotateEncodeRead(const UINT8 *src)
{
    ExtWord extWord;
    UINT32 first = (UINT32)(UINTPTR)src % INST_F48_EXT_WORD_LEN;
    for (UINT32 i = 0; i < INST_F48_EXT_WORD_LEN; i++) {
        extWord.bytes[(first + i) % INST_F48_EXT_WORD_LEN] = src[i];
    }
    return extWord.val;
}

STATIC INLINE VOID ArchRotateEncodeWrite(UINT8 *dest, UINT32 val)
{
    UINT32 first = (UINT32)(UINTPTR)dest % INST_F48_EXT_WORD_LEN;
    ExtWord extWord = {.val = val};
    for (UINT32 i = 0; i < INST_F48_EXT_WORD_LEN; i++) {
        dest[i] = extWord.bytes[(first + i) % INST_F48_EXT_WORD_LEN];
    }
}

STATIC INLINE UINT16 ArchInstMainWordRead(const InstWord *inst)
{
    return inst->mainWordVal;
}

STATIC VOID ArchInstMainWordWrite(InstWord *inst, UINT16 val)
{
    inst->mainWordVal = val;
    ArchParityEncodeWrite(inst->mainWord, val);
}

STATIC INLINE UINT32 ArchExtWordRead(const UINT8 *src, UINT32 len)
{
    switch (len) {
        case INST_F16_EXT_WORD_LEN:
            return 0;
        case INST_F24_EXT_WORD_LEN:
            return *src;
        case INST_F32_EXT_WORD_LEN:
            return ArchParityEncodeRead(src);
        case INST_F48_EXT_WORD_LEN:
            return ArchRotateEncodeRead(src);
        default:
            return 0;
    }
}

STATIC INLINE UINT32 ArchInstExtWordRead(const InstWord *inst)
{
    return inst->extWordVal;
}

STATIC VOID ArchInstExtWordWrite(InstWord *inst, UINT32 val)
{
    inst->extWordVal = val;
    switch (inst->extWordLen) {
        case INST_F24_EXT_WORD_LEN:
            *inst->extWord = (UINT8)val;
            break;
        case INST_F32_EXT_WORD_LEN:
            ArchParityEncodeWrite(inst->extWord, (UINT16)val);
            break;
        case INST_F48_EXT_WORD_LEN:
            ArchRotateEncodeWrite(inst->extWord, val);
            break;
        default:
            LOS_ASSERT(FALSE);
    }
}

#define MAX_UNSIGNED(bits)      (UINTPTR)(((UINTPTR)1 << (bits)) - 1)
#define MAX_SIGNED(bits)        (INTPTR)(MAX_UNSIGNED((bits) - 1))
#define MIN_SIGNED(bits)        (INTPTR)(~(MAX_UNSIGNED((bits) - 1)))

STATIC INLINE BOOL ArchCheckBitsFit(UINTPTR val, UINT32 bits, BOOL hasSign)
{
    LOS_ASSERT(bits > (hasSign ? 1 : 0));

    /* The bits width we passed maybe equals to the bit width of UINTPTR, which may leads to undefined behavier
     * when using shift operand below. Check it here to aviod any problems. */
    if (bits >= (sizeof(UINTPTR) * BITS_PER_BYTE)) {
        return TRUE;
    }

    if (hasSign) {
        return ((INTPTR)val >= MIN_SIGNED(bits)) && ((INTPTR)val <= MAX_SIGNED(bits));
    } else {
        return (val <= MAX_UNSIGNED(bits));
    }
}

STATIC INLINE UINT32 ArchInstReloc(InstWord *inst, const InstEncoder *info, UINTPTR val)
{
    UINT16 mainWord;
    UINT32 extWord;

    UINT32 bitSize = info->mainWidth + info->extWidth;
    UINTPTR valShifted = val >> info->shift;
    if (!ArchCheckBitsFit(valShifted, bitSize, info->hasSign)) {
        return LOS_NOK;
    }

    if (info->mainWidth > 0) {
        mainWord = ArchInstMainWordRead(inst);
        LD_MoveBitsArea16(valShifted, &mainWord, 0, info->mainStart, info->mainWidth);
        ArchInstMainWordWrite(inst, mainWord);
    }

    if (info->extWidth > 0) {
        extWord = ArchInstExtWordRead(inst);
        LD_MoveBitsArea32(valShifted, &extWord, info->mainWidth, info->extStart, info->extWidth);
        ArchInstExtWordWrite(inst, extWord);
    }

    return LOS_OK;
}

STATIC UINT32 ArchInstWordLenGet(UINT8 *mainWord, UINT32 *len)
{
    UINT32 val = ArchParityEncodeRead(mainWord);
    if ((val & INST_F16_SUFFIX_MASK) == INST_F16_SUFFIX) {
        *len = INST_F16_EXT_WORD_LEN;
    } else if ((val & INST_F24_SUFFIX_MASK) == INST_F24_SUFFIX) {
        *len = INST_F24_EXT_WORD_LEN;
    } else if ((val & INST_F32_SUFFIX_MASK) == INST_F32_SUFFIX) {
        *len = INST_F32_EXT_WORD_LEN;
    } else if ((val & INST_F48_SUFFIX_MASK) == INST_F48_SUFFIX) {
        *len = INST_F48_EXT_WORD_LEN;
    } else {
        return LOS_NOK;
    }

    return LOS_OK;
}

STATIC INLINE BOOL ArchCheckEov(UINT8 *mainWord)
{
    UINT32 val = ArchParityEncodeRead(mainWord);
    return ((val & INST_EOV_MASK) != 0) ? TRUE : FALSE;
}

/** LingLong Instraction Encoding:
  * bit0                bitN
  * ┌──────────┬───────────┐
  * │main word │extend word│
  * └──────────┴───────────┘
  * LingLong instructions can splits into two parts: main word and extend word.
  * Main word are always 16bits long, extend word length can be 0bits,
  * 8bits, 16bits and 32bits, corresponds to f16, f24, f32 and f48 format,
  * length can be read from main word by different encoding.
  * Multiple instructions can form a VLIW and split into two parts. The last
  * instruction in VLIW is marked by the EOV bit.
  *
  * bit0             EOV↓                    bitN
  * ┌─────────┬─────────┬───────────┬───────────┐
  * │main word│main word│extend word│extend word│
  * └─────────┴─────────┴───────────┴───────────┘
  *
  * Encoding:
  * For 16bits data (eg, main word), use parity encoding, which byte 0 always on
  * even address and byte 1 always on odd address.
  * For 32bits data (eg, f48 extend word), use rotate encoding, which byte 0/1/2/3
  * always on the address aligned 4 + 0/1/2/3.
  */
STATIC UINT32 ArchInstWordGet(InstWord *inst, UINT8 *vliw, UINT32 slot)
{
    UINT32 extWordLen[SLOTS_MAX];
    UINT32 mainWordPos = 0;
    UINT32 extWordPos = 0;
    INT32 slotCnt = -1;
    UINT32 ret;

    /* parse VLIW length */
    do {
        ++slotCnt;
        ret = ArchInstWordLenGet(vliw + slotCnt * MAIN_WORD_LEN, &extWordLen[slotCnt]);
        if (ret != LOS_OK) {
            return ret;
        }
    } while ((slotCnt < (SLOTS_MAX - 1)) && !ArchCheckEov(vliw + slotCnt * MAIN_WORD_LEN));

    ++slotCnt;
    if (slot >= (UINT32)slotCnt) {
        return LOS_NOK;
    }

    /* relative offset of mainWord and extWord */
    for (UINT32 i = 0; i < slot; i++) {
        mainWordPos += MAIN_WORD_LEN;
        extWordPos += extWordLen[i];
    }

    inst->mainWord = vliw + mainWordPos;
    inst->extWord = vliw + slotCnt * MAIN_WORD_LEN + extWordPos;
    inst->extWordLen = (UINT8)extWordLen[slot];
    inst->mainWordVal = ArchParityEncodeRead(inst->mainWord);
    inst->extWordVal = ArchExtWordRead(inst->extWord, inst->extWordLen);

    return LOS_OK;
}

STATIC BOOL ArchCheckInstMatch(const InstRelocInfo *op, InstWord *inst)
{
    MainWord mainWord;
    mainWord.val = ArchInstMainWordRead(inst);
    ExtWord extWord;
    extWord.val = ArchInstExtWordRead(inst);

    for (UINT32 i = 0; i < MAIN_WORD_LEN; i++) {
        if ((mainWord.bytes[i] & op->mask[i]) != op->code[i]) {
            return FALSE;
        }
    }

    for (UINT32 i = 0; i < inst->extWordLen; i++) {
        if ((extWord.bytes[i] & op->mask[i + MAIN_WORD_LEN]) != op->code[i + MAIN_WORD_LEN]) {
            return FALSE;
        }
    }

    return TRUE;
}

STATIC INLINE UINT32 CalcRel32(const LDRelocParam *relocParam, UINT32 *res)
{
    INT64 val = (INT64)relocParam->symAdd + (INT64)relocParam->addend - (INT64)relocParam->position;
    if ((val < INT32_MIN) || (val > INT32_MAX)) {
        return LOS_NOK;
    }
    *res = (UINT32)val;
    return LOS_OK;
}

STATIC INLINE UINT32 CalcAbs32(const LDRelocParam *relocParam, UINT32 *res)
{
    INT64 val = (INT64)relocParam->symAdd + (INT64)relocParam->addend;
    if ((val < 0) || (val > UINT32_MAX)) {
        return LOS_NOK;
    }
    *res = (UINT32)val;
    return LOS_OK;
}

STATIC INLINE UINT32 ObjRelocAuxParamGet(const LDRelocParam *relocParam, RelocAuxParam *auxParam)
{
    UINT32 ret;
    UINT32 val;
    UINT32 slot;
    UINT32 tblLen;
    const InstRelocInfo *relTbl;

    switch (relocParam->relocType) {
        case R_LINGLONG_PCREL:
            slot = 0;
            relTbl = g_pcRelInfoTbl;
            tblLen = ARRAY_SIZE(g_pcRelInfoTbl);
            ret = CalcRel32(relocParam, &val);
            break;
        case R_LINGLONG_ABS32_SLOT0:
        case R_LINGLONG_ABS32_SLOT1:
        case R_LINGLONG_ABS32_SLOT2:
        case R_LINGLONG_ABS32_SLOT3:
            slot = relocParam->relocType - R_LINGLONG_ABS32_SLOT0;
            relTbl = g_abs32RelocInfoTbl;
            tblLen = ARRAY_SIZE(g_abs32RelocInfoTbl);
            ret = CalcAbs32(relocParam, &val);
            break;
        case R_LINGLONG_LOOP_START:
            slot = 0;
            relTbl = g_hwloopBegInfoTbl;
            tblLen = ARRAY_SIZE(g_hwloopBegInfoTbl);
            ret = CalcRel32(relocParam, &val);
            break;
        case R_LINGLONG_LOOP_END:
            slot = 0;
            relTbl = g_hwloopEndInfoTbl;
            tblLen = ARRAY_SIZE(g_hwloopEndInfoTbl);
            ret = CalcRel32(relocParam, &val);
            break;
        default:
            PRINT_ERR("Unsupported reloc type %x\n", LD_ELF_R_TYPE(relocParam->relocType));
            return LOS_NOK;
    }

    if (ret != LOS_OK) {
        return LOS_NOK;
    }

    auxParam->type = relocParam->relocType;
    auxParam->relTbl = relTbl;
    auxParam->slot = slot;
    auxParam->tblLen = tblLen;
    auxParam->val = val;

    return LOS_OK;
}

/** Instruction relocation procedure.
 *
 *                          ┌─────────────────────┐
 *                          │ g_pcRelInfoTbl      │
 *                          │                     │
 *         ┌─────────┐      │ g_abs32RelocInfoTbl │       ┌─────────────┐     ┌────────────┐
 *         │relocType│ ───► │                     │ ───►  │ encoderDesc │ ──► │ g_encoders │
 *         └─────────┘      │ g_hwloopBegInfoTbl  │       └─────────────┘     └────────────┘
 *                          │                     │
 *                          │ g_hwloopEndInfoTbl  │
 *                          └─────────────────────┘
 *
 * We'll take ld.a8.u8.aa@agx.f48 as an example to illustrate how an instruction relocated.
 *
 * 1. As provided by the compiler, ld.a8.u8.aa will has type R_LINGLONG_ABS32_SLOT<n>,
 * where <n> could be 0-3 in linglong binary, indicates the slot index in a vliw. Unlike
 * ARM, many different instructions can share same relocation type, so we would likes to
 * have a table to store the specific instruction info to identify them, for this example
 * the table is g_abs32RelocInfoTbl.
 *
 * 2. Each instruction has a unique encoding so the core can distinguish them, and the encoding
 * can abstract as a pair of (code, mask), which we're already stored in g_abs32RelocInfoTbl. By
 * matching every pair of (code, mask) we can found a specific encoderDesc in the table.
 *
 * 3. The encoderDesc, for this example is U30_SHIFT2_B__X__F48, indicates an encoding rule which
 * describes how value should be placed in the instruction bits. The rule can be found in g_encoders
 * to write value in instruction.
 *
 * About R_LINGLONG_ABS32_SLOT<n>
 *
 * Maximum 8 instructions can packed into one VLIW, however, <n> can only take 0 to 3. That is
 * because the possible instructions (pcu, agu, sau, axu, in order) is always layout at the
 * beginning of one VLIW and most have 4 of them. As a result, we are safe to say R_LINGLONG_ABS32_SLOT<n>
 * must the nth instruction in a VLIW.
 */

STATIC UINT32 ArchObjRelocInst(const LDRelocParam *relocParam)
{
    UINT32 ret;
    InstWord inst;
    RelocAuxParam auxParam;

    ret = ObjRelocAuxParamGet(relocParam, &auxParam);
    if (ret != LOS_OK) {
        PRINT_ERR("Reloc get aux param failed\n");
        return LOS_NOK;
    }

    ret = ArchInstWordGet(&inst, (UINT8 *)relocParam->position, auxParam.slot);
    if (ret != LOS_OK) {
        PRINT_ERR("Reloc get inst failed\n");
        return LOS_NOK;
    }

    for (UINT32 idx = 0; idx < auxParam.tblLen; idx++) {
        if (ArchCheckInstMatch(&auxParam.relTbl[idx], &inst) == TRUE) {
            ret = ArchInstReloc(&inst, &g_encoders[auxParam.relTbl[idx].encoderDesc], auxParam.val);
            if (ret != LOS_OK) {
                PRINT_ERR("Reloc inst failed\n");
            }
            return ret;
        }
    }

    PRINT_ERR("Reloc no matched inst, inst is %x\n", ArchInstMainWordRead(&inst));

    return LOS_NOK;
}

STATIC UINT32 ArchObjRelocData(const LDRelocParam *relocParam)
{
    UINT32 ret;
    UINT32 val;
    if (relocParam->shType != LD_SHT_RELA) {
        return LOS_NOK;
    }
    ret = CalcAbs32(relocParam, &val);
    if (ret != LOS_OK) {
        return LOS_NOK;
    }
    /*
     * After 'ld -r', the addend will set to the value and be cleared by the linker,
     * therefore the real formula is S + A + Value.
     */
    *(UINT32 *)relocParam->position += val;
    return LOS_OK;
}

UINT32 ArchObjRelocImp(const LD_ELF_RELA *relocCmd, const LDRelocParam *relocParam)
{
    if (relocParam->shType != LD_SHT_RELA) {
        PRINT_ERR("Unsupported shType %u\n", relocParam->shType);
        return LOS_NOK;
    }

    if (LD_ELF_R_TYPE(relocCmd->info) == R_LINGLONG_32) {
        return ArchObjRelocData(relocParam);
    } else {
        return ArchObjRelocInst(relocParam);
    }
}

UINT32 ArchSegRelocImp(const LD_ELF_RELA *rela, const LDRelocParam *relocParam)
{
    (VOID)rela;
    (VOID)relocParam;
    PRINT_ERR("%s not supported\n", __FUNCTION__);
    return LOS_NOK; /* linglong compiler does not support the -pic option */
}

UINT32 ArchSegRelocTblCheck(const RelocationInfo *relocInfo)
{
    return (relocInfo->relTable != 0) ? LOS_OK : LOS_NOK;
}

__attribute__((weak)) UINT32 ArchDynloadPrepare(VOID)
{
    return LOS_OK;
}

__attribute__((weak)) UINT32 ArchDynloadFinish(UINTPTR codeAddrStart, UINTPTR codeAddrEnd,
                                               UINTPTR dataAddrStart, UINTPTR dataAddrEnd)
{
	return LOS_OK;
}
