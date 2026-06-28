/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: AARCH64 Dynamic Load HeadFile
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

#ifndef _ARCH_DYNLOAD_H
#define _ARCH_DYNLOAD_H

#include "unistd.h"
#include "los_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define DYNLOAD_ALIGN_PAGE_SIZE 4096
#define DYNLOAD_ALIGN_SIZE      DYNLOAD_ALIGN_PAGE_SIZE
#define UINTMAX                 UINT64_MAX

/* Elf header */
#define LD_EI_NIDENT           16
typedef struct {
    UINT8       elfIdent[LD_EI_NIDENT]; /* Magic number and other info */
    UINT16      elfType;                /* Object file type */
    UINT16      elfMachine;             /* Architecture */
    UINT32      elfVersion;             /* Object file version */
    UINT64      elfEntry;               /* Entry point virtual address */
    UINT64      elfPhoff;               /* Program header table file offset */
    UINT64      elfShoff;               /* Section header table file offset */
    UINT32      elfFlags;               /* Processor-specific flags */
    UINT16      elfHeadSize;            /* ELF header size in bytes */
    UINT16      elfPhSize;              /* Program header table entry size */
    UINT16      elfPhNum;               /* Program header table entry count */
    UINT16      elfShEntSize;           /* Section header table entry size */
    UINT16      elfShNum;               /* Section header table entry count */
    UINT16      elfShStrIndex;          /* Section header string table index */
} LDElf64Ehdr;

#define LD_ELF_EHDR            LDElf64Ehdr

/* Symbol table */
typedef struct {
    UINT32 stName;  /* Symbol table name (string tbl index) */
    UINT8 stInfo;   /* Symbol table type and binding */
    UINT8 stOther;  /* Symbol table visibility */
    UINT16 stShndx; /* Section table index */
    UINT64 stValue; /* Symbol table value */
    UINT64 stSize;  /* Symbol table size */
} LDElf64Sym;

#define LD_ELF_SYM             LDElf64Sym

/* Section header */
typedef struct {
    UINT32 shName;      /* Section name (string tbl index) */
    UINT32 shType;      /* Section type */
    UINT64 shFlags;     /* Section flags */
    UINT64 shAddr;      /* Section virtual addr at execution */
    UINT64 shOffset;    /* Section file offset */
    UINT64 shSize;      /* Section size in bytes */
    UINT32 shLink;      /* Link to another section */
    UINT32 shInfo;      /* Additional section information */
    UINT64 shAddrAlign; /* Section alignment */
    UINT64 shEntSize;   /* Entry size if section holds table */
} LDElf64Shdr;

#define LD_ELF_SHDR            LDElf64Shdr

/* Program Header */
typedef struct {
    UINT32 type;     /* Segment type */
    UINT32 flags;    /* Segment flags */
    UINT64 offset;   /* Segment file offset */
    UINT64 vAddr;    /* Segment virtual address */
    UINT64 phyAddr;  /* Segment physical address */
    UINT64 fileSize; /* Segment size in file */
    UINT64 memSize;  /* Segment size in memory */
    UINT64 align;    /* Segment alignment */
} LDElf64Phdr;
#define LD_ELF_PHDR            LDElf64Phdr

/* EI_CLASS */
#define LD_ELF_CLASS64         2
#define LD_ELF_CLASS LD_ELF_CLASS64

/* e_machine */
#define LD_EM_AARCH64          183   /* ARM AARCH64 */
#define LD_EM_TYPE  LD_EM_AARCH64

/* Dynamic */
typedef struct {
    UINT64 dynTag;  /* Dynamic entry type */
    union {
        UINT64 val; /* Integer value */
        UINT64 ptr; /* Address value */
    } dyn;
} LDElf64Dyn;

#define LD_ELF_DYN             LDElf64Dyn

/* Relocation */
typedef struct {
    UINT64 offset; /* Address */
    UINT64 info;   /* Relocation type and symbol index */
} LDElf64Rel;

typedef struct {
    UINT64 offset; /* Address */
    UINT64 info;   /* Relocation type and symbol index */
    INT64 addend;  /* Addend */
} LDElf64Rela;

#define LD_ELF_REL             LDElf64Rel
#define LD_ELF_RELA            LDElf64Rela

#define LD_ELF_R_SYM(info) ((info) >> 32)
#define LD_ELF_R_TYPE(info) ((info) & 0xFFFFFFFFUL)
#define LD_ELF_R_INFO(sym, type) ((((UINT64)(sym)) << 32) + (type))

#define OS_R_ARM_AARCH64_ABS64              257 /* Direct 64 bit. */
#define OS_R_ARM_AARCH64_PREL32             261
#define OS_R_ARM_AARCH64_ADR_PREL_PG_HI21   275
#define OS_R_ARM_AARCH64_ADD_ABS_LO12_NC    277
#define OS_R_ARM_AARCH64_LDST8_ABS_LO12_NC  278
#define OS_R_ARM_AARCH64_CALL26             283
#define OS_R_ARM_AARCH64_LDST16_ABS_LO12_NC 284
#define OS_R_ARM_AARCH64_LDST32_ABS_LO12_NC 285
#define OS_R_ARM_AARCH64_LDST64_ABS_LO12_NC 286

/* AARCH64 for  Dynamic relocations */
#define OS_R_ARM_AARCH64_COPY               1024
#define OS_R_ARM_AARCH64_GLOB_DAT           1025
#define OS_R_ARM_AARCH64_JUMP_SLOT          1026
#define OS_R_ARM_AARCH64_RELATIVE           1027
#define OS_R_ARM_AARCH64_TLS_DTPREL64       1028
#define OS_R_ARM_AARCH64_TLS_DTPMOD64       1029
#define OS_R_ARM_AARCH64_TLS_TPREL64        1030
#define OS_R_ARM_AARCH64_TLSDESC            1031
#define OS_R_ARM_AARCH64_IRELATIVE          1032

typedef struct {
    UINT32 relocType;
    UINT32 shType;        /* REL section or RELA section. */
    UINTPTR position;     /* the address of the place being relocated. */
    UINTPTR symAdd;       /* the address of the symbol. */
    ssize_t addend;       /* the addend for the relocation. */
    UINTPTR baseSegment;  /* the addressing origin of the output segment defining the symbol S. */
    UINTPTR gotAddr;      /* the address of the Global Offset Table. */
    /*
     * T is 1 if the target symbol S has type STT_FUNC and
     * the symbol addresses a Thumb instruction; it is 0 otherwise.
     */
    UINT8 thumbFunc;
#define RESERVE_NUM 7
    UINT8 reserved[RESERVE_NUM];
} LDRelocParam;

typedef struct {
    UINTPTR relTable;     /* address of relocation table. */
    UINTPTR relTableSize; /* total size of relocation table. */
    UINTPTR relEntrySize; /* size of every relocation entry. */
} RelocationInfo;

extern UINT32 AARCH64_ELF_Call26Reloc(const LDRelocParam *relocParam);
extern UINT32 AARCH64_ELF_Abs64Reloc(const LDRelocParam *relocParam);
extern UINT32 AARCH64_ELF_AdrPrelPgHi21Reloc(const LDRelocParam *relocParam);
extern UINT32 AARCH64_ELF_Ldst32AbsLo12NcReloc(const LDRelocParam *relocParam);
extern UINT32 AARCH64_ELF_AddAbsLo12NcReloc(const LDRelocParam *relocParam);
extern UINT32 AARCH64_ELF_Ldst8AbsLo12NcReloc(const LDRelocParam *relocParam);
extern UINT32 AARCH64_ELF_Ldst64AbsLo12NcReloc(const LDRelocParam *relocParam);
extern UINT32 AARCH64_ELF_Ldst16AbsLo12NcReloc(const LDRelocParam *relocParam);
extern UINT32 AARCH64_ELF_GlobDatReloc(const LDRelocParam *relocParam);
extern UINT32 AARCH64_ELF_JumpSlotReloc(const LDRelocParam *relocParam);
extern UINT32 AARCH64_ELF_RelativeReloc(const LDRelocParam *relocParam);
extern UINT32 ArchSegRelocImp(const LD_ELF_RELA *rela, const LDRelocParam *relocParam);
extern UINT32 ArchObjRelocImp(const LD_ELF_RELA *relocCmd, const LDRelocParam *relocParam);
extern UINT32 ArchSegRelocTblCheck(const RelocationInfo *relocInfo);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ARCH_DYNLOAD_H */
