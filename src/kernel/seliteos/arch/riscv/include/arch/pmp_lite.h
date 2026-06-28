/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: lite pmp header for SELiteOS.
 */
#ifndef PMP_LITE_H
#define PMP_LITE_H

#include <stdint.h>
#include "arch/regs.h"
#ifdef SECURITY_ENHANCEMENT
#include "security_enhancement.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* pmp csrs */
#define PMPCFG0                     0x3A0
#define PMPCFG1                     0x3A1
#define PMPCFG2                     0x3A2
#define PMPCFG3                     0x3A3
#define PMPCFG4                     0x3A4
#define PMPCFG5                     0x3A5
#define PMPCFG6                     0x3A6
#define PMPCFG7                     0x3A7

#define PMPADDR0                    0x3B0
#define PMPADDR1                    0x3B1
#define PMPADDR2                    0x3B2
#define PMPADDR3                    0x3B3
#define PMPADDR4                    0x3B4
#define PMPADDR5                    0x3B5
#define PMPADDR6                    0x3B6
#define PMPADDR7                    0x3B7
#define PMPADDR8                    0x3B8
#define PMPADDR9                    0x3B9
#define PMPADDR10                   0x3BA
#define PMPADDR11                   0x3BB
#define PMPADDR12                   0x3BC
#define PMPADDR13                   0x3BD
#define PMPADDR14                   0x3BE
#define PMPADDR15                   0x3BF
#define PMPADDR16                   0x3C0
#define PMPADDR17                   0x3C1
#define PMPADDR18                   0x3C2
#define PMPADDR19                   0x3C3
#define PMPADDR20                   0x3C4
#define PMPADDR21                   0x3C5
#define PMPADDR22                   0x3C6
#define PMPADDR23                   0x3C7
#define PMPADDR24                   0x3C8
#define PMPADDR25                   0x3C9
#define PMPADDR26                   0x3CA
#define PMPADDR27                   0x3CB
#define PMPADDR28                   0x3CC
#define PMPADDR29                   0x3CD
#define PMPADDR30                   0x3CE
#define PMPADDR31                   0x3CF

#define MEMATTR0                    0xBC0
#define MEMATTR1                    0xBC1
#define MEMATTR2                    0xBC2
#define MEMATTR3                    0xBC3
#define MEMATTR4                    0xBC4
#define MEMATTR5                    0xBC5
#define MEMATTR6                    0xBC6
#define MEMATTR7                    0xBC7
#define MEMATTR8                    0xBC8
#define MEMATTR9                    0xBC9
#define MEMATTR10                   0xBCA
#define MEMATTR11                   0xBCB
#define MEMATTR12                   0xBCC
#define MEMATTR13                   0xBCD
#define MEMATTR14                   0xBCE
#define MEMATTR15                   0xBCF
#define MEMATTR16                   0xBD0
#define MEMATTR17                   0xBD1
#define MEMATTR18                   0xBD2
#define MEMATTR19                   0xBD3
#define MEMATTR20                   0xBD4
#define MEMATTR21                   0xBD5
#define MEMATTR22                   0xBD6
#define MEMATTR23                   0xBD7
#define MEMATTR24                   0xBD8
#define MEMATTR25                   0xBD9
#define MEMATTR26                   0xBDA
#define MEMATTR27                   0xBDB
#define MEMATTR28                   0xBDC
#define MEMATTR29                   0xBDD
#define MEMATTR30                   0xBDE
#define MEMATTR31                   0xBDF

#define PMPTE0                      0x7F0
#define PMPTE1                      0x7F1

/* pmpcfg macro */
#define PMP_PERMISSION_U_X_P_NA          0x1C
#define PMP_PERMISSION_U_R_P_RW          0x1A
#define PMP_PERMISSION_U_RW_P_RW         0x1E
#define PMP_PERMISSION_U_R_P_NA          0x19
#define PMP_PERMISSION_U_RX_P_NA         0x1D
#define PMP_PERMISSION_U_RW_P_NA         0x1B
#define PMP_PERMISSION_U_RWX_P_NA        0x1F

#define PMP_PERMISSION_U_NA_P_NA         0x98
#define PMP_PERMISSION_U_NA_P_X          0x9C
#define PMP_PERMISSION_U_X_P_X           0x9A
#define PMP_PERMISSION_U_X_P_RX          0x9E
#define PMP_PERMISSION_U_NA_P_R          0x99
#define PMP_PERMISSION_U_NA_P_RX         0x9D
#define PMP_PERMISSION_U_NA_P_RW         0x9B
#define PMP_PERMISSION_U_R_P_R           0x9F

/* mem_attr macro */
#define MEM_ATTR_DEV_NON_BUF             0   /* 4b0000, device non-bufferable */
#define MEM_ATTR_DEV_BUF                 1   /* 4b0001, device bufferable */
#define MEM_ATTR_NORM_NON_CA_NON_BUF     2   /* 4b0010, normal non-cacheable non-bufferable */
#define MEM_ATTR_NORM_NON_CA_BUF         3   /* 4b0011, normal non-cacheable bufferable */
#define MEM_ATTR_WT_NA                   4   /* 4b0100, write-through no-allocate */
#define MEM_ATTR_WT_RA                   6   /* 4b0110, Write-through read-allocate */
#define MEM_ATTR_WB_RD_ALLOC             7   /* 4b0111, Write-back read-allocate */
#define MEM_ATTR_WB_NA                   8   /* 4b1000, Write-back no-allocate */
#define MEM_ATTR_WT_RWA                  14  /* 4b1110, write-through read and write-allocate */
#define MEM_ATTR_WB_RW_ALLOC             15  /* 4b1111, Write-back read- and write-allocate */

#ifndef UINT32_CUT_MASK
#define UINT32_CUT_MASK                  0xFFFFFFFF
#endif
#define PMPADDR_ALIGN_BIT                2
#define SIZE_SHIFT_BIT                   3
#define GET_PMPCFG_OFFSET(pmp_index)     ((uint8_t)((pmp_index) & 0x3) << 3)
#define GET_PMPCFG_NUM(pmp_index)        ((uint8_t)(pmp_index) >> 2)
#define PMPTE_CSR_BITS                   32

static inline void pmp_set_rwx_permission(uint8_t index, uint8_t permission)
{
    uint8_t pmpcfg_index = GET_PMPCFG_NUM(index);
    uint32_t pmpcfg_val = READ_CUSTOM_CSR((PMPCFG0 + pmpcfg_index));
    pmpcfg_val &= (~((0xFF << GET_PMPCFG_OFFSET(index)) & UINT32_CUT_MASK));
    pmpcfg_val |= (permission << GET_PMPCFG_OFFSET(index));
    WRITE_CUSTOM_CSR_VAL((PMPCFG0 + pmpcfg_index), pmpcfg_val);
}

static inline void pmp_set_napot_addr(uint8_t index, uint32_t addr, uint32_t size)
{
    uint32_t pmpaddr_val = (uint32_t)((addr >> PMPADDR_ALIGN_BIT) + (size >> SIZE_SHIFT_BIT) - 1);
    WRITE_CUSTOM_CSR_VAL((PMPADDR0 + index), pmpaddr_val);
}

static inline void pmp_set_mem_attr(uint8_t index, uint8_t mem_attr)
{
    uint32_t mem_attr_val = READ_CUSTOM_CSR((MEMATTR0 + index));
    mem_attr_val &= 0xFFFFFFF0; // 0xFFFFFFF0: clear low 4 bits
    mem_attr_val |= mem_attr;
    WRITE_CUSTOM_CSR_VAL((MEMATTR0 + index), mem_attr_val);
}

static inline void pmp_region_config(uint8_t index, uint8_t permission, uint32_t addr, uint32_t size, uint8_t mem_attr)
{
    pmp_set_rwx_permission(index, permission);
    pmp_set_napot_addr(index, addr, size);
    pmp_set_mem_attr(index, mem_attr);
}

/* pmpte0 controls pmp regions: 0 to 31 */
static inline void pmp_set_pmpte0(uint8_t index, uint8_t tes)
{
    uint32_t tes_val = READ_CUSTOM_CSR(PMPTE0);
    tes_val &= (~((1 << index) & UINT32_CUT_MASK));
    tes_val |= (tes << index);
    WRITE_CUSTOM_CSR_VAL(PMPTE0, tes_val);
#ifdef SECURITY_ENHANCEMENT
    tee_csr_readback_check(PMPTE0, tes_val);
#endif
}

#if defined(LOSCFG_PMP_MAX_SUPPORT) && (LOSCFG_PMP_MAX_SUPPORT == 64)
/* pmpte1 controls pmp regions: 32 to 63 */
static inline void pmp_set_pmpte1(uint8_t index, uint8_t tes)
{
    uint32_t tes_val = READ_CUSTOM_CSR(PMPTE1);
    tes_val &= (~((1 << (index % PMPTE_CSR_BITS)) & UINT32_CUT_MASK));
    tes_val |= (tes << (index % PMPTE_CSR_BITS));
    WRITE_CUSTOM_CSR_VAL(PMPTE1, tes_val);
#ifdef SECURITY_ENHANCEMENT
    tee_csr_readback_check(PMPTE1, tes_val);
#endif
}
#endif

static inline void pmp_set_tes(uint8_t index, uint8_t tes)
{
#if defined(LOSCFG_PMP_MAX_SUPPORT) && (LOSCFG_PMP_MAX_SUPPORT == 64)
    if (index >= PMPTE_CSR_BITS) {
        return pmp_set_pmpte1(index, tes);
    }
#endif
    return pmp_set_pmpte0(index, tes);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PMP_LITE_H */

