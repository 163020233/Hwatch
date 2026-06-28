/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: LingLong reloc handle for aslr
 * Author: Huawei LiteOS Team
 * Create: 2024-07-24
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
#include "los_ld_reloc_pri.h"
#include "arch/dynload.h"
#include "asm/platform.h"
#include "asm/memmap_config.h"

#define BITS_PER_BYTE                           8
#define BYTE_MASK                               0xFF

union AslrInfo {
    struct {
        UINT32 type         : 8;                /* [0:7]  : reloc type defined by compiler */
        UINT32 encodeIdx    : 8;                /* [8:15] : encoding type defined by laslr script */
        UINT32 mainWordIdx  : 2;                /* [15:17]: main word offset (slots) */
        UINT32 extWordIdx   : 6;                /* [18:24]: ext word offset (bytes) */
    };
    UINT32 value;
};

LITE_OS_SEC_LASLR_DATA const STATIC InstEncoder g_encoders[] = {
    /* ----,----,xxx-,----,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,x--- */
    [TRG__X__U32_D__X__F48] = {8, 3, 0, 29, 0},
    /* ----,----,----,----,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx */
    [TRG__X__U32__X__F48] = {0, 0, 0, 32, 0},
    /* ----,----,--x-,----,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxx- */
    [TRG__X__U32_B__X__F48] = {10, 1, 0, 31, 0},
    /* ----,---x,xxx-,----,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxx-,---- */
    [U31_SHIFT1_B__X__F48] = {7, 4, 0, 27, 1},
    /* ----,----,xxx-,----,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxx-,---- */
    [U30_SHIFT2_B__X__F48] = {8, 3, 0, 27, 2},
    /* ----,--xx,xxx-,----,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxx-,---- */
    [U32_B__X__F48] = {6, 5, 0, 27, 0},
    /* ----,--xx,xxx-,----,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxx-,---- */
    [M32__X__F48] = {6, 5, 0, 27, 0},
    /* ----,-xxx,xxxx,xx-- */
    [TRG__X__REL_S9__X__F16] = {5, 9, 0, 0, 0},
    /* ----,---x,xxxx,x---,xxxx,xxxx */
    [TRG__X__REL_S14__X__F24] = {7, 6, 0, 8, 0},
    /* ----,--xx,xxxx,----,xxxx,xxxx,xxxx,xxxx */
    [TRG__X__REL_S22__X__F32] = {6, 6, 0, 16, 0},
    /* ----,--xx,xxxx,x---,xxx-,---- */
    [TRG__X__REL_S10_C__X__F24] = {6, 7, 0, 3, 0},
    /* ----,---x,xxxx,----,xxxx,xxxx,xxx-,---- */
    [TRG__X__REL_S16_C__X__F32] = {7, 5, 0, 11, 0},
    /* ----,----,----,----,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx */
    [TRG__X__REL_S32__X__F48] = {0, 0, 0, 32, 0},
    /* ----,----,----,----,----,xxxx,xxxx,xxxx */
    [TRG__X__REL_U12__X__F32] = {0, 0, 4, 12, 0},
    /* ----,----,----,----,----,----,----,----,---x,xxxx,xxxx,xxxx */
    [TRG__X__REL_U13__X__F48] = {0, 0, 19, 13, 0},
    /* ----,----,----,----,----,xxxx,xxxx,xxx-,----,----,----,---- */
    [REL_HWL_U11_BEG__X__F48] = {0, 0, 4, 11, 0},
    /* ----,----,----,----,----,----,----,---x,xxxx,xxxx,xx--,---- */
    [REL_HWL_U11_END__X__F48] = {0, 0, 15, 11, 0},
    /* ----,----,----,----,-xxx,xxxx,xxxx,----,----,----,----,---- */
    [REL_HWL_U11_B_BEG__X__F48] = {0, 0, 1, 11, 0},
    /* ----,----,----,----,----,----,----,xxxx,xxxx,xxx-,----,---- */
    [REL_HWL_U11_B_END__X__F48] = {0, 0, 12, 11, 0},
};

LITE_OS_SEC_LASLR_DATA const STATIC UINT8 g_extWidthMapping[] = {
    [TRG__X__U32_D__X__F48] = INST_F48_EXT_WORD_LEN,
    [TRG__X__U32__X__F48] = INST_F48_EXT_WORD_LEN,
    [TRG__X__U32_B__X__F48] = INST_F48_EXT_WORD_LEN,
    [U31_SHIFT1_B__X__F48] = INST_F48_EXT_WORD_LEN,
    [U30_SHIFT2_B__X__F48] = INST_F48_EXT_WORD_LEN,
    [U32_B__X__F48] = INST_F48_EXT_WORD_LEN,
    [M32__X__F48] = INST_F48_EXT_WORD_LEN,
    [TRG__X__REL_S9__X__F16] = INST_F16_EXT_WORD_LEN,
    [TRG__X__REL_S14__X__F24] = INST_F24_EXT_WORD_LEN,
    [TRG__X__REL_S22__X__F32] = INST_F32_EXT_WORD_LEN,
    [TRG__X__REL_S10_C__X__F24] = INST_F24_EXT_WORD_LEN,
    [TRG__X__REL_S16_C__X__F32] = INST_F32_EXT_WORD_LEN,
    [TRG__X__REL_S32__X__F48] = INST_F48_EXT_WORD_LEN,
    [TRG__X__REL_U12__X__F32] = INST_F32_EXT_WORD_LEN,
    [TRG__X__REL_U13__X__F48] = INST_F48_EXT_WORD_LEN,
    [REL_HWL_U11_BEG__X__F48] = INST_F48_EXT_WORD_LEN,
    [REL_HWL_U11_END__X__F48] = INST_F48_EXT_WORD_LEN,
    [REL_HWL_U11_B_BEG__X__F48] = INST_F48_EXT_WORD_LEN,
    [REL_HWL_U11_B_END__X__F48] = INST_F48_EXT_WORD_LEN,
};

LITE_OS_SEC_LASLR_TEXT STATIC INLINE UINT32 AslrAbs(INT32 relAddr)
{
    return relAddr >= 0 ? relAddr : -relAddr;
}

LITE_OS_SEC_LASLR_TEXT STATIC INLINE BOOL AslrEncodeSigned(INT32 encodeIdx)
{
    return (encodeIdx >= TRG__X__REL_S9__X__F16) && (encodeIdx <= TRG__X__REL_S32__X__F48);
}

LITE_OS_SEC_LASLR_TEXT STATIC INLINE UINT16 ReverseByteOrderU16(UINT16 val)
{
    return ((UINT16)(((UINT32)(val) << BITS_PER_BYTE) | ((UINT32)(val) >> BITS_PER_BYTE)));
}

LITE_OS_SEC_LASLR_TEXT STATIC INLINE BOOL IsOddAddr(const UINT8 *ptr)
{
    return (((UINTPTR)ptr & 0x01) == 0x01);
}

LITE_OS_SEC_LASLR_TEXT STATIC INLINE UINT16 AslrParityEncodeRead(const UINT8 *src)
{
    UINT16 val = (UINT16)((src[1] << BITS_PER_BYTE) | src[0]);
    if (IsOddAddr(src) == TRUE) {
        val = ReverseByteOrderU16(val);
    }
    return val;
}

LITE_OS_SEC_LASLR_TEXT STATIC INLINE VOID AslrParityEncodeWrite(UINT8 *dest, UINT16 val)
{
    if (IsOddAddr(dest) == TRUE) {
        val = ReverseByteOrderU16(val);
    }
    dest[0] = val & BYTE_MASK;
    dest[1] = (val >> BITS_PER_BYTE) & BYTE_MASK;
}

LITE_OS_SEC_LASLR_TEXT STATIC INLINE UINT32 AslrRotateEncodeRead(const UINT8 *src)
{
    ExtWord extWord;
    UINT32 first = (UINT32)((UINTPTR)src % INST_F48_EXT_WORD_LEN);
    for (UINT32 i = 0; i < INST_F48_EXT_WORD_LEN; i++) {
        extWord.bytes[(first + i) % INST_F48_EXT_WORD_LEN] = src[i];
    }
    return extWord.val;
}

LITE_OS_SEC_LASLR_TEXT STATIC INLINE VOID AslrRotateEncodeWrite(UINT8 *dest, UINT32 val)
{
    UINT32 first = (UINT32)((UINTPTR)dest % INST_F48_EXT_WORD_LEN);
    ExtWord extWord = {.val = val};
    for (UINT32 i = 0; i < INST_F48_EXT_WORD_LEN; i++) {
        dest[i] = extWord.bytes[(first + i) % INST_F48_EXT_WORD_LEN];
    }
}

LITE_OS_SEC_LASLR_TEXT STATIC INLINE UINT32 AslrExtWordRead(const UINT8 *src, UINT32 len)
{
    switch (len) {
        case INST_F24_EXT_WORD_LEN:
            return *src;
        case INST_F32_EXT_WORD_LEN:
            return AslrParityEncodeRead(src);
        case INST_F48_EXT_WORD_LEN:
            return AslrRotateEncodeRead(src);
        default:
            return 0;
    }
}

LITE_OS_SEC_LASLR_TEXT STATIC INLINE VOID AslrExtWordWrite(UINT8 *dest, UINT32 val, UINT32 width)
{
    switch (width) {
        case INST_F24_EXT_WORD_LEN:
            *dest = (UINT8)val;
            break;
        case INST_F32_EXT_WORD_LEN:
            AslrParityEncodeWrite(dest, (UINT16)val);
            break;
        case INST_F48_EXT_WORD_LEN:
            AslrRotateEncodeWrite(dest, val);
            break;
        default:
            return;
    }
}

LITE_OS_SEC_LASLR_TEXT
STATIC INLINE UINT32 AslrMoveBits(UINT32 dest, UINT8 destStart, UINT32 src, UINT8 srcStart, UINT8 width)
{
    /*
     * 1. srcInfo = bitfield(srcStart, width); destInfo = bitfield(destStart, width);
     * 2. result = dest; tmpVal = extract_bitfield(src, srcInfo)
     * 3. insert_bitfield(&result, tmpVal, destInfo)
     */
    UINT32 result;
    UINT32 srcInfo, destInfo, tmpVal;
    __asm__ (
        "{ bext.ctrl.pack %[srcInfo], %[srcStart], %[width] ; bext.ctrl.pack %[destInfo], %[destStart], %[width] }\n"
        "{ mov %[result], %[dest] ; bextu.a %[tmpVal], %[src], %[srcInfo] }\n"
        "bitins.a.i %[result], %[tmpVal], %[destInfo]\n"
        : [result]"=&r"(result), [srcInfo]"=&r"(srcInfo), [destInfo]"=&r"(destInfo), [tmpVal]"=&r"(tmpVal)
        : [src]"r"(src), [srcStart]"r"(srcStart), [destStart]"r"(destStart), [width]"r"(width), [dest]"r"(dest)
    );
    return result;
}

LITE_OS_SEC_LASLR_TEXT
STATIC INLINE VOID AslrInstReloc(UINT16 *mainWord, UINT32 *extWord, const InstEncoder *encode, UINTPTR val)
{
    UINTPTR valShifted = val >> encode->shift;

    if (encode->mainWidth > 0) {
        *mainWord = (UINT16)AslrMoveBits((UINT32)*mainWord, encode->mainStart, valShifted, 0, encode->mainWidth);
    }

    if (encode->extWidth > 0) {
        *extWord = AslrMoveBits(*extWord, encode->extStart, valShifted, encode->mainWidth, encode->extWidth);
    }
}

LITE_OS_SEC_LASLR_TEXT STATIC INLINE VOID CopyMachineCode(UINTPTR dest, UINTPTR src, size_t size)
{
    /* dest/src may not aligned, must use byte-by-byte copy */
    CHAR *destBuf = (CHAR *)dest;
    const CHAR *srcBuf = (const CHAR *)src;
    for (size_t idx = 0; idx < size; idx++) {
        destBuf[idx] = srcBuf[idx];
    }
}

LITE_OS_SEC_LASLR_TEXT STATIC INLINE BOOL AddrInRange(UINTPTR addr, UINTPTR lower, UINTPTR upper)
{
    return (addr >= lower) && (addr < upper);
}

LITE_OS_SEC_LASLR_TEXT STATIC INLINE UINTPTR GetMainWordAddr(UINTPTR base, union AslrInfo info)
{
    return base + MAIN_WORD_LEN * info.mainWordIdx;
}

LITE_OS_SEC_LASLR_TEXT STATIC INLINE UINTPTR GetExtWordAddr(UINTPTR base, union AslrInfo info)
{
    return base + info.extWordIdx;
}

LITE_OS_SEC_LASLR_TEXT STATIC UINT32 GetValue(const LDRelocParam *relocParam, union AslrInfo info)
{
    if (((info.type >= R_LINGLONG_ABS32_SLOT0) && (info.type <= R_LINGLONG_ABS32_SLOT3)) ||
        (info.type == R_LINGLONG_32)) {
        return (UINT32)relocParam->symAdd;
    }

    UINT32 value;
    INT32 relativeAddr = (INT32)(relocParam->symAdd - relocParam->position);
    UINT32 random = (UINT32)relocParam->baseSegment;
    if (AslrEncodeSigned(info.encodeIdx)) {
        value = AslrAbs(relativeAddr) - random;
        if (relativeAddr < 0) {
            value = -value;
        }
    } else {
        value = (UINT32)relativeAddr - random;
    }
    return value;
}

union DWord {
    UINT32 value;
    UINT8 bytes[sizeof(UINT32)];
};

LITE_OS_SEC_LASLR_TEXT STATIC INLINE VOID WriteUnaligned32(UINTPTR address, UINT32 value)
{
    union DWord dwordVal = {.value = value};

    if ((address & (sizeof(UINT32) - 1)) == 0) {
        *(UINT32 *)address = dwordVal.value;
    } else {
        union DWord *dword = (union DWord *)address;
        for (UINT32 idx = 0; idx < sizeof(union DWord); idx++) {
            dword->bytes[idx] = dwordVal.bytes[idx];
        }
    }
}

/** Reloc process:
 * 1. from rela.info get encode info
 * 2. from encode info get inst type (rel or abs)
 * 3. get randomized value using following rule:
 *   a. absolute-address data:      value = value + random
 *   b. absolute-address program:   value = value + random
 *   c. relative-address program:   value = sign * (abs(value - position) - random)
 * 4. read and parse VLIW
 * 5. by using VLIW and encode info, write value to VLIW
 */

__attribute__((no_stack_protector))
LITE_OS_SEC_LASLR_TEXT UINT32 ArchAslrRelocImp(const LD_ELF_RELA *rela, const LDRelocParam *relocParam)
{
    /* cannot not use global variable directly due to aslr, fix address manually before use. */
    UINT32 random = (UINT32)relocParam->baseSegment;
    const InstEncoder *encoders = (InstEncoder *)((UINTPTR)g_encoders + random);
    const UINT8 *extWidthMapping = g_extWidthMapping + random;
    const union AslrInfo info = {.value = rela->info};

    UINT32 value = GetValue(relocParam, info);

    /* tcm does not offset, fix the address */
    UINTPTR position = relocParam->position;
    UINTPTR tcmPos = relocParam->position;
    BOOL isDtcm = FALSE;
    if (position - random >= (UINTPTR)__tcm_start) {
        position = position - random;
        tcmPos = position;
        /* remap position to tcm backup area */
        if (AddrInRange(tcmPos, ITCM_ADDR, ITCM_ADDR + ITCM_SIZE)) {
            position = ITCM_BACKUP_VIRT_ADDR + (tcmPos - ITCM_ADDR);
        } else {
            position = DTCM_BACKUP_VIRT_ADDR + (tcmPos - DTCM_ADDR);
            isDtcm = TRUE;
        }
    }

    /* data type can write directly. */
    if (info.type == R_LINGLONG_32) {
        WriteUnaligned32(position, value);
        if (isDtcm) {
            WriteUnaligned32(tcmPos, value);
        }
        return LOS_OK;
    }

    /* instructions needs more process, handle here */
    UINT8 extWordWidth = extWidthMapping[info.encodeIdx];
    const InstEncoder *encoder = &encoders[info.encodeIdx];
    UINT16 mainWord = AslrParityEncodeRead((UINT8 *)GetMainWordAddr(position, info));
    UINT32 extWord = AslrExtWordRead((UINT8 *)GetExtWordAddr(position, info), extWordWidth);
    AslrInstReloc(&mainWord, &extWord, encoder, value);
    AslrParityEncodeWrite((UINT8 *)GetMainWordAddr(position, info), mainWord);
    AslrExtWordWrite((UINT8 *)GetExtWordAddr(position, info), extWord, extWordWidth);

    /* if target position is dtcm, also write to dtcm so don't needs copy it later. */
    if (isDtcm) {
        CopyMachineCode(GetMainWordAddr(tcmPos, info), GetMainWordAddr(position, info), MAIN_WORD_LEN);
        CopyMachineCode(GetExtWordAddr(tcmPos, info), GetExtWordAddr(position, info), extWordWidth);
    }

    return LOS_OK;
}
