/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description: RISCV32 reloc handle for aslr
 * Author: Huawei LiteOS Team
 * Create: 2022-12-20
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

#include "stdio.h"
#include "los_typedef.h"
#include "arch/dynload.h"
#include "los_ld_reloc_pri.h"

LITE_OS_SEC_LASLR_DATA UINTPTR g_aslrInitFlag = 0;
#if defined(LOSCFG_LASLR_GOT)
STATIC LITE_OS_SEC_LASLR_TEXT UINT32 AslrRiscvRelativeReloc(const LDRelocParam *relocParam)
{
    UINTPTR addend = (UINTPTR)relocParam->addend;
    UINTPTR position = relocParam->position;

    *(UINTPTR *)position = relocParam->baseSegment + addend;

    return LOS_OK;
}
#elif defined(LOSCFG_LASLR_GSPR)
/*
 * 0x800: The successive instruction has a signed 12-bit immediate
 * so the value of the preceding high 20-bit relocation may have 1 added to it.
 */
#define U_TYPE_OFFSET                0x800
#define RISCV_HI20_MASK              0xFFFFF000
#define RISCV_HI20_BIT_FROM          12
#define RISCV_HI20_BIT_TO            12
#define RISCV_HI20_BIT_WIDE          20
#define RISCV_LO12_I_BIT_FROM        0
#define RISCV_LO12_I_BIT_TO          20
#define RISCV_LO12_I_BIT_WIDE        12
#define RISCV_LO12_S1_BIT_FROM       0
#define RISCV_LO12_S1_BIT_TO         7
#define RISCV_LO12_S1_BIT_WIDE       5
#define RISCV_LO12_S2_BIT_FROM       5
#define RISCV_LO12_S2_BIT_TO         25
#define RISCV_LO12_S2_BIT_WIDE       7
#define RSICV_RVC_LUI1_BIT_FROM      17
#define RSICV_RVC_LUI1_BIT_TO        12
#define RSICV_RVC_LUI1_BIT_WIDE      1
#define RSICV_RVC_LUI2_BIT_FROM      12
#define RSICV_RVC_LUI2_BIT_TO        2
#define RSICV_RVC_LUI2_BIT_WIDE      5

STATIC LITE_OS_SEC_LASLR_TEXT UINT32 AslrRiscvRel32Reloc(const LDRelocParam *relocParam)
{
    INT32 addend = relocParam->addend;
    UINTPTR position = relocParam->position;
    UINTPTR symAddr = relocParam->symAdd;
    *(UINTPTR *)position = symAddr + addend;
    return LOS_OK;
}

/* Absolute address such as a global variable or string constant need to be relocated.
 * There are two combinations of the corresponding relocation operations:
 * OS_R_RISCV_HI20/OS_R_RISCV_LO12_S, OS_R_RISCV_HI20/OS_R_RISCV_LO12_I.
 * OS_R_RISCV_HI20 indicates that high 20-bits of the absolute address are relocated using lui instruction.
 * OS_R_RISCV_LO12_S indicates S-Type instructions (store).
 * OS_R_RISCV_LO12_I indicates I-Type instructions (add immediate or load).
 * These two pairs of relocation algorithms are calculated like this:
 * -----------------------------------------
 * |HI20  |  (symbol_address + 0x800) >> 12  |
 *  -----------------------------------------
 * |LO12  |  symbol_address - (HI20 << 12)   |
 *  -----------------------------------------
 * Take OS_R_RISCV_HI20/OS_R_RISCV_LO12_I as an example:
 * lui  a0, %hi(symbol)     # OS_R_RISCV_HI20 (symbol)
 * addi a0, a0, %lo(symbol) # OS_R_RISCV_LO12_I (symbol)
 */
STATIC LITE_OS_SEC_LASLR_TEXT UINT32 AslrRiscvHigh20Reloc(const LDRelocParam *relocParam)
{
    INT32 imm;
    INT32 addend = relocParam->addend;
    UINTPTR symAddr = relocParam->symAdd;
    UINTPTR position = relocParam->position;
 
    imm = (INT32)(symAddr + addend) + U_TYPE_OFFSET;
    /*
     * %pcrel_hi|hi(symbol) (U-Type):
     * |31                               12|11                  |6      |0
     * |imm[31:12]                         |rd                  |opcode
     */
    LD_MoveBitsArea32((UINT32)imm, (UINT32 *)position, RISCV_HI20_BIT_FROM,
                      RISCV_HI20_BIT_TO, RISCV_HI20_BIT_WIDE);

    return LOS_OK;
}
 
STATIC LITE_OS_SEC_LASLR_TEXT UINT32 AslrRiscvLow12SReloc(const LDRelocParam *relocParam)
{
    INT32 imm;
    INT32 addend = relocParam->addend;
    UINTPTR symAddr = relocParam->symAdd;
    UINTPTR position = relocParam->position;

    imm = (INT32)((symAddr + addend) - (((symAddr + addend) + U_TYPE_OFFSET) & RISCV_HI20_MASK));

    /*
     * %pcrel_lo|lo(pcrel_hi20_address) (S-Type):
     * |31     25|24      |19       |14     |11      7|6      |0
     * imm[11:5] |rs2     |rs1      |funct3 |imm[4:0] |opcode
     */
    LD_MoveBitsArea32((UINT32)imm, (UINT32 *)position, RISCV_LO12_S1_BIT_FROM,
                      RISCV_LO12_S1_BIT_TO, RISCV_LO12_S1_BIT_WIDE);
    LD_MoveBitsArea32((UINT32)imm, (UINT32 *)position, RISCV_LO12_S2_BIT_FROM,
                      RISCV_LO12_S2_BIT_TO, RISCV_LO12_S2_BIT_WIDE);

    return LOS_OK;
}

STATIC LITE_OS_SEC_LASLR_TEXT UINT32 AslrRiscvLow12IReloc(const LDRelocParam *relocParam)
{
    INT32 imm;
    INT32 addend = relocParam->addend;
    UINTPTR symAddr = relocParam->symAdd;
    UINTPTR position = relocParam->position;

    imm = (INT32)((symAddr + addend) - (((symAddr + addend) + U_TYPE_OFFSET) & RISCV_HI20_MASK));

    /*
     * %pcrel_lo|lo(pcrel_hi20_address) (I-Type):
     * |31                        20|19    |14     |11      |6      |0
     * |imm[11:0]                   |rs1   |funct3 |rd      |opcode
     */
    LD_MoveBitsArea32((UINT32)imm, (UINT32 *)position, RISCV_LO12_I_BIT_FROM,
                      RISCV_LO12_I_BIT_TO, RISCV_LO12_I_BIT_WIDE);

    return LOS_OK;
}

STATIC LITE_OS_SEC_LASLR_TEXT UINT32 AslrRiscvRvcLuiReloc(const LDRelocParam *relocParam)
{
    INT32 imm;
    INT32 addend = relocParam->addend;
    UINTPTR symAddr = relocParam->symAdd;
    UINTPTR position = relocParam->position;
    imm = (INT32)(symAddr + addend) + U_TYPE_OFFSET;
    /*
     * c.lui rd, imm
     * 15 13 | 12       | 11 7 | 6 2        | 1 0
     * 011   | imm[17]  | rd   | imm[16:12] | 01
     */
    LD_MoveBitsArea16((UINT32)imm, (UINT16 *)position, RSICV_RVC_LUI1_BIT_FROM,
                      RSICV_RVC_LUI1_BIT_TO, RSICV_RVC_LUI1_BIT_WIDE);
    LD_MoveBitsArea16((UINT32)imm, (UINT16 *)position, RSICV_RVC_LUI2_BIT_FROM,
                      RSICV_RVC_LUI2_BIT_TO, RSICV_RVC_LUI2_BIT_WIDE);
    return LOS_OK;
}
#endif

LITE_OS_SEC_LASLR_TEXT UINT32 ArchAslrRelocImp(const LD_ELF_RELA *rela, const LDRelocParam *relocParam)
{
    UINT32 ret = LOS_NOK;
    switch (LD_ELF_R_TYPE(rela->info)) {
#ifdef LOSCFG_LASLR_GOT
        case OS_R_RISCV_RELATIVE:
            ret = AslrRiscvRelativeReloc(relocParam);
            break;
#elif defined(LOSCFG_LASLR_GSPR)
        case OS_R_RISCV_32:
            ret = AslrRiscvRel32Reloc(relocParam);
            break;
        case OS_R_RISCV_GP_HI20:
        case OS_R_RISCV_HI20:
            ret = AslrRiscvHigh20Reloc(relocParam);
            break;
        case OS_R_RISCV_LO12_S:
        case OS_R_RISCV_GP_LO12_S:
            ret = AslrRiscvLow12SReloc(relocParam);
            break;
        case OS_R_RISCV_GP_LO12_I:
        case OS_R_RISCV_LO12_I:
            ret = AslrRiscvLow12IReloc(relocParam);
            break;
        case OS_R_RISCV_RVC_LUI:
            ret = AslrRiscvRvcLuiReloc(relocParam);
            break;
#endif
        default:
            break;
    }
    return ret;
}
