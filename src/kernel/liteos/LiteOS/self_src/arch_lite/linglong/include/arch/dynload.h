/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: LingLong Dynamic Load HeadFile
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

#ifndef _ARCH_DYNLOAD_H
#define _ARCH_DYNLOAD_H

#include "los_typedef.h"
#include "unistd.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define DYNLOAD_ALIGN_SIZE                  16
#define SECTION_ALIGN_SIZE                  16
#define UINTMAX                             UINT32_MAX
#define RESERVE_NUM                         7
#define LD_EI_NIDENT                        16

#define LD_ELF_R_SYM(info)                  ((info) >> 8)
#define LD_ELF_R_TYPE(info)                 ((info) & 0xFFU)
#define LD_ELF_R_INFO(sym, type)            (((sym) << 8) + (UINT8)(type))

#define LD_ELF_SYM                          LDElf32Sym
#define LD_ELF_SHDR                         LDElf32Shdr
#define LD_ELF_PHDR                         LDElf32Phdr
#define LD_ELF_REL                          LDElf32Rel
#define LD_ELF_RELA                         LDElf32Rela
#define LD_ELF_EHDR                         LDElf32Ehdr
#define LD_ELF_DYN                          LDElf32Dyn

/* Relocation Type Definitions */
/* Relocation For Data */
#define R_LINGLONG_32                       1   /* S + A */
/* Relocation For Program */
#define R_LINGLONG_PCREL                    2   /* S + A - P */
#define R_LINGLONG_LOOP_START               3   /* S + A - P */
#define R_LINGLONG_LOOP_END                 4   /* S + A - P */
#define R_LINGLONG_ABS32_SLOT0              5   /* S + A */
#define R_LINGLONG_ABS32_SLOT1              6   /* S + A */
#define R_LINGLONG_ABS32_SLOT2              7   /* S + A */
#define R_LINGLONG_ABS32_SLOT3              8   /* S + A */

/* Reloc format names. Please refers to the linglong core xml. */
#define TRG__X__U32_D__X__F48                   0
#define TRG__X__U32__X__F48                     1
#define TRG__X__U32_B__X__F48                   2
#define U31_SHIFT1_B__X__F48                    3
#define U30_SHIFT2_B__X__F48                    4
#define U32_B__X__F48                           5
#define M32__X__F48                             6
#define TRG__X__REL_S9__X__F16                  7
#define TRG__X__REL_S14__X__F24                 8
#define TRG__X__REL_S22__X__F32                 9
#define TRG__X__REL_S10_C__X__F24               10
#define TRG__X__REL_S16_C__X__F32               11
#define TRG__X__REL_S32__X__F48                 12
#define TRG__X__REL_U12__X__F32                 13
#define TRG__X__REL_U13__X__F48                 14
#define REL_HWL_U11_BEG__X__F48                 15
#define REL_HWL_U11_END__X__F48                 16
#define REL_HWL_U11_B_BEG__X__F48               17
#define REL_HWL_U11_B_END__X__F48               18

/* VLIW constant definitions */
#define SLOTS_MAX                               8  /* PPA 8, LC 4 */
#define MAIN_WORD_LEN                           2

#define INST_F16_EXT_WORD_LEN                   0
#define INST_F24_EXT_WORD_LEN                   1
#define INST_F32_EXT_WORD_LEN                   2
#define INST_F48_EXT_WORD_LEN                   4

#define INST_EXT_WORD_MAX_LEN                   INST_F48_EXT_WORD_LEN
#define INST_MAX_LEN                            (MAIN_WORD_LEN + INST_EXT_WORD_MAX_LEN)

/* EI_CLASS */
#define LD_ELF_CLASS32                      1
#define LD_ELF_CLASS                        LD_ELF_CLASS32

/* e_machine */
#define LD_EM_LINGLONG                      292    /* LingLong  */
#define LD_EM_TYPE                          LD_EM_LINGLONG

/* Symbol table */
typedef struct {
    UINT32 stName;      /* Symbol table name (string tbl index) */
    UINT32 stValue;     /* Symbol table value */
    UINT32 stSize;      /* Symbol table size */
    UINT8 stInfo;       /* Symbol table type and binding */
    UINT8 stOther;      /* Symbol table visibility */
    UINT16 stShndx;     /* Section table index */
} LDElf32Sym;

/* Section header */
typedef struct {
    UINT32 shName;      /* Section name (string tbl index) */
    UINT32 shType;      /* Section type */
    UINT32 shFlags;     /* Section flags */
    UINT32 shAddr;      /* Section virtual addr at execution */
    UINT32 shOffset;    /* Section file offset */
    UINT32 shSize;      /* Section size in bytes */
    UINT32 shLink;      /* Link to another section */
    UINT32 shInfo;      /* Additional section information */
    UINT32 shAddrAlign; /* Section alignment */
    UINT32 shEntSize;   /* Entry size if section holds table */
} LDElf32Shdr;

/* Program Header */
typedef struct {
    UINT32 type;        /* Segment type */
    UINT32 offset;      /* Segment file offset */
    UINT32 vAddr;       /* Segment virtual address */
    UINT32 phyAddr;     /* Segment physical address */
    UINT32 fileSize;    /* Segment size in file */
    UINT32 memSize;     /* Segment size in memory */
    UINT32 flags;       /* Segment flags */
    UINT32 align;       /* Segment alignment */
} LDElf32Phdr;

/* Elf header */
typedef struct {
    UINT8  elfIdent[LD_EI_NIDENT];  /* Magic number and other info */
    UINT16 elfType;         /* Object file type */
    UINT16 elfMachine;      /* Architecture */
    UINT32 elfVersion;      /* Object file version */
    UINT32 elfEntry;        /* Entry point virtual address */
    UINT32 elfPhoff;        /* Program header table file offset */
    UINT32 elfShoff;        /* Section header table file offset */
    UINT32 elfFlags;        /* Processor-specific flags */
    UINT16 elfHeadSize;     /* ELF header size in bytes */
    UINT16 elfPhSize;       /* Program header table entry size */
    UINT16 elfPhNum;        /* Program header table entry count */
    UINT16 elfShEntSize;    /* Section header table entry size */
    UINT16 elfShNum;        /* Section header table entry count */
    UINT16 elfShStrIndex;   /* Section header string table index */
} LDElf32Ehdr;

/* Dynamic */
typedef struct {
    UINT32 dynTag;  /* Dynamic entry type */
    union {
        UINT32 val; /* Integer value */
        UINT32 ptr; /* Address value */
    } dyn;
} LDElf32Dyn;

/* Relocation */
typedef struct {
    UINT32 offset;  /* Address */
    UINT32 info;    /* Relocation type and symbol index */
} LDElf32Rel;

typedef struct {
    UINT32 offset;  /* Address */
    UINT32 info;    /* Relocation type and symbol index */
    INT32 addend;   /* Addend */
} LDElf32Rela;

/* These are GNU extensions to enable C++ vtable garbage collection. */
typedef struct {
    UINT32 relocType;
    UINT32 shType;          /* REL section or RELA section. */
    UINTPTR position;       /* the address of the place being relocated. */
    UINTPTR symAdd;         /* the address of the symbol. */
    ssize_t addend;         /* the addend for the relocation. */
    UINTPTR baseSegment;    /* the addressing origin of the output segment defining the symbol S. */
    UINTPTR gotAddr;        /* the address of the Global Offset Table. */
    /*
     * T is 1 if the target symbol S has type STT_FUNC and
     * the symbol addresses a Thumb instruction; it is 0 otherwise.
     */
    UINT8 thumbFunc;
    UINT8 reserved[RESERVE_NUM];
} LDRelocParam;

typedef struct {
    UINTPTR relTable;       /* address of relocation table. */
    UINTPTR relTableSize;   /* total size of relocation table. */
    UINTPTR relEntrySize;   /* size of every relocation entry. */
} RelocationInfo;

typedef struct {
    UINT8 *mainWord;
    UINT8 *extWord;
    UINT32 extWordVal;      /* linglong extWord max 4 bytes */
    UINT16 mainWordVal;     /* linglong mainWord always 2 bytes */
    UINT8 extWordLen;
} InstWord;

typedef union ExtWord {
    UINT32 val;
    UINT8 bytes[INST_EXT_WORD_MAX_LEN];
} ExtWord;

typedef union MainWord {
    UINT16 val;
    UINT8 bytes[MAIN_WORD_LEN];
} MainWord;

typedef struct {
    UINT8 mainStart;
    UINT8 mainWidth;
    UINT8 extStart;
    UINT8 extWidth;
    UINT8 shift : 7;
    UINT8 hasSign : 1;
} InstEncoder;

extern UINT32 ArchSegRelocImp(const LD_ELF_RELA *rela, const LDRelocParam *relocParam);
extern UINT32 ArchObjRelocImp(const LD_ELF_RELA *relocCmd, const LDRelocParam *relocParam);
extern UINT32 ArchSegRelocTblCheck(const RelocationInfo *relocInfo);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ARCH_DYNLOAD_H */
