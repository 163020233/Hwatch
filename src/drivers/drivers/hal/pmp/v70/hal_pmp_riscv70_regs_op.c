/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides systick register operation api \n
 *
 * History: \n
 * 2022-09-28, Create file. \n
 */
#include <stdint.h>
#include "common_def.h"
#include "hal_pmp_riscv70_regs_op.h"

#ifdef SUPPORT_RISCV70_PMP_NUM_64
void hal_pmp_riscv70_regs_set_pmpaddr48_to_63(uint32_t idx, uint32_t pmpaddr)
{
    if (idx == PMPADDR48_INDEX) {
        write_csr(pmpaddr48, pmpaddr);
    } else if (idx == PMPADDR49_INDEX) {
        write_csr(pmpaddr49, pmpaddr);
    } else if (idx == PMPADDR50_INDEX) {
        write_csr(pmpaddr50, pmpaddr);
    } else if (idx == PMPADDR51_INDEX) {
        write_csr(pmpaddr51, pmpaddr);
    } else if (idx == PMPADDR52_INDEX) {
        write_csr(pmpaddr52, pmpaddr);
    } else if (idx == PMPADDR53_INDEX) {
        write_csr(pmpaddr53, pmpaddr);
    } else if (idx == PMPADDR54_INDEX) {
        write_csr(pmpaddr54, pmpaddr);
    } else if (idx == PMPADDR55_INDEX) {
        write_csr(pmpaddr55, pmpaddr);
    } else if (idx == PMPADDR56_INDEX) {
        write_csr(pmpaddr56, pmpaddr);
    } else if (idx == PMPADDR57_INDEX) {
        write_csr(pmpaddr57, pmpaddr);
    } else if (idx == PMPADDR58_INDEX) {
        write_csr(pmpaddr58, pmpaddr);
    } else if (idx == PMPADDR59_INDEX) {
        write_csr(pmpaddr59, pmpaddr);
    } else if (idx == PMPADDR60_INDEX) {
        write_csr(pmpaddr60, pmpaddr);
    } else if (idx == PMPADDR61_INDEX) {
        write_csr(pmpaddr61, pmpaddr);
    } else if (idx == PMPADDR62_INDEX) {
        write_csr(pmpaddr62, pmpaddr);
    } else if (idx == PMPADDR63_INDEX) {
        write_csr(pmpaddr63, pmpaddr);
    }
}

void hal_pmp_riscv70_regs_set_pmpaddr32_to_47(uint32_t idx, uint32_t pmpaddr)
{
    if (idx == PMPADDR32_INDEX) {
        write_csr(pmpaddr32, pmpaddr);
    } else if (idx == PMPADDR33_INDEX) {
        write_csr(pmpaddr33, pmpaddr);
    } else if (idx == PMPADDR34_INDEX) {
        write_csr(pmpaddr34, pmpaddr);
    } else if (idx == PMPADDR35_INDEX) {
        write_csr(pmpaddr35, pmpaddr);
    } else if (idx == PMPADDR36_INDEX) {
        write_csr(pmpaddr36, pmpaddr);
    } else if (idx == PMPADDR37_INDEX) {
        write_csr(pmpaddr37, pmpaddr);
    } else if (idx == PMPADDR38_INDEX) {
        write_csr(pmpaddr38, pmpaddr);
    } else if (idx == PMPADDR39_INDEX) {
        write_csr(pmpaddr39, pmpaddr);
    } else if (idx == PMPADDR40_INDEX) {
        write_csr(pmpaddr40, pmpaddr);
    } else if (idx == PMPADDR41_INDEX) {
        write_csr(pmpaddr41, pmpaddr);
    } else if (idx == PMPADDR42_INDEX) {
        write_csr(pmpaddr42, pmpaddr);
    } else if (idx == PMPADDR43_INDEX) {
        write_csr(pmpaddr43, pmpaddr);
    } else if (idx == PMPADDR44_INDEX) {
        write_csr(pmpaddr44, pmpaddr);
    } else if (idx == PMPADDR45_INDEX) {
        write_csr(pmpaddr45, pmpaddr);
    } else if (idx == PMPADDR46_INDEX) {
        write_csr(pmpaddr46, pmpaddr);
    } else if (idx == PMPADDR47_INDEX) {
        write_csr(pmpaddr47, pmpaddr);
    } else {
        hal_pmp_riscv70_regs_set_pmpaddr48_to_63(idx, pmpaddr);
    }
}

void hal_pmp_riscv70_regs_set_memxattr48_to_63(uint32_t idx, uint8_t attr)
{
    if (idx == PMPADDR48_INDEX) {
        write_csr(memattr48, attr);
    } else if (idx == PMPADDR49_INDEX) {
        write_csr(memattr49, attr);
    } else if (idx == PMPADDR50_INDEX) {
        write_csr(memattr50, attr);
    } else if (idx == PMPADDR51_INDEX) {
        write_csr(memattr51, attr);
    } else if (idx == PMPADDR52_INDEX) {
        write_csr(memattr52, attr);
    } else if (idx == PMPADDR53_INDEX) {
        write_csr(memattr53, attr);
    } else if (idx == PMPADDR54_INDEX) {
        write_csr(memattr54, attr);
    } else if (idx == PMPADDR55_INDEX) {
        write_csr(memattr55, attr);
    } else if (idx == PMPADDR56_INDEX) {
        write_csr(memattr56, attr);
    } else if (idx == PMPADDR57_INDEX) {
        write_csr(memattr57, attr);
    } else if (idx == PMPADDR58_INDEX) {
        write_csr(memattr58, attr);
    } else if (idx == PMPADDR59_INDEX) {
        write_csr(memattr59, attr);
    } else if (idx == PMPADDR60_INDEX) {
        write_csr(memattr60, attr);
    } else if (idx == PMPADDR61_INDEX) {
        write_csr(memattr61, attr);
    } else if (idx == PMPADDR62_INDEX) {
        write_csr(memattr62, attr);
    } else if (idx == PMPADDR63_INDEX) {
        write_csr(memattr63, attr);
    }
}

void hal_pmp_riscv70_regs_set_memxattr32_to_47(uint32_t idx, uint8_t attr)
{
    if (idx == PMPADDR32_INDEX) {
        write_csr(memattr32, attr);
    } else if (idx == PMPADDR33_INDEX) {
        write_csr(memattr33, attr);
    } else if (idx == PMPADDR34_INDEX) {
        write_csr(memattr34, attr);
    } else if (idx == PMPADDR35_INDEX) {
        write_csr(memattr35, attr);
    } else if (idx == PMPADDR36_INDEX) {
        write_csr(memattr36, attr);
    } else if (idx == PMPADDR37_INDEX) {
        write_csr(memattr37, attr);
    } else if (idx == PMPADDR38_INDEX) {
        write_csr(memattr38, attr);
    } else if (idx == PMPADDR39_INDEX) {
        write_csr(memattr39, attr);
    } else if (idx == PMPADDR40_INDEX) {
        write_csr(memattr40, attr);
    } else if (idx == PMPADDR41_INDEX) {
        write_csr(memattr41, attr);
    } else if (idx == PMPADDR42_INDEX) {
        write_csr(memattr42, attr);
    } else if (idx == PMPADDR43_INDEX) {
        write_csr(memattr43, attr);
    } else if (idx == PMPADDR44_INDEX) {
        write_csr(memattr44, attr);
    } else if (idx == PMPADDR45_INDEX) {
        write_csr(memattr45, attr);
    } else if (idx == PMPADDR46_INDEX) {
        write_csr(memattr46, attr);
    } else if (idx == PMPADDR47_INDEX) {
        write_csr(memattr47, attr);
    } else {
        hal_pmp_riscv70_regs_set_memxattr48_to_63(idx, attr);
    }
}
#endif

#if defined(SUPPORT_RISCV70_PMP_NUM_32) || defined(SUPPORT_RISCV70_PMP_NUM_64)
void hal_pmp_riscv70_regs_set_pmpaddr16_to_63(uint32_t idx, uint32_t pmpaddr)
{
    /* Because like that pmpaddr0 is RISCV CSR, so cannot be performed using table lookup mode. */
    if (idx == PMPADDR16_INDEX) {
        write_csr(pmpaddr16, pmpaddr);
    } else if (idx == PMPADDR17_INDEX) {
        write_csr(pmpaddr17, pmpaddr);
    } else if (idx == PMPADDR18_INDEX) {
        write_csr(pmpaddr18, pmpaddr);
    } else if (idx == PMPADDR19_INDEX) {
        write_csr(pmpaddr19, pmpaddr);
    } else if (idx == PMPADDR20_INDEX) {
        write_csr(pmpaddr20, pmpaddr);
    } else if (idx == PMPADDR21_INDEX) {
        write_csr(pmpaddr21, pmpaddr);
    } else if (idx == PMPADDR22_INDEX) {
        write_csr(pmpaddr22, pmpaddr);
    } else if (idx == PMPADDR23_INDEX) {
        write_csr(pmpaddr23, pmpaddr);
    } else if (idx == PMPADDR24_INDEX) {
        write_csr(pmpaddr24, pmpaddr);
    } else if (idx == PMPADDR25_INDEX) {
        write_csr(pmpaddr25, pmpaddr);
    } else if (idx == PMPADDR26_INDEX) {
        write_csr(pmpaddr26, pmpaddr);
    } else if (idx == PMPADDR27_INDEX) {
        write_csr(pmpaddr27, pmpaddr);
    } else if (idx == PMPADDR28_INDEX) {
        write_csr(pmpaddr28, pmpaddr);
    } else if (idx == PMPADDR29_INDEX) {
        write_csr(pmpaddr29, pmpaddr);
    } else if (idx == PMPADDR30_INDEX) {
        write_csr(pmpaddr30, pmpaddr);
    } else if (idx == PMPADDR31_INDEX) {
        write_csr(pmpaddr31, pmpaddr);
#ifdef SUPPORT_RISCV70_PMP_NUM_64
    } else {
        hal_pmp_riscv70_regs_set_pmpaddr32_to_47(idx, pmpaddr);
#endif
    }
}

void hal_pmp_riscv70_regs_set_memxattr16_to_63(uint32_t idx, uint8_t attr)
{
    if (idx == PMPADDR16_INDEX) {
        write_csr(memattr16, attr);
    } else if (idx == PMPADDR17_INDEX) {
        write_csr(memattr17, attr);
    } else if (idx == PMPADDR18_INDEX) {
        write_csr(memattr18, attr);
    } else if (idx == PMPADDR19_INDEX) {
        write_csr(memattr19, attr);
    } else if (idx == PMPADDR20_INDEX) {
        write_csr(memattr20, attr);
    } else if (idx == PMPADDR21_INDEX) {
        write_csr(memattr21, attr);
    } else if (idx == PMPADDR22_INDEX) {
        write_csr(memattr22, attr);
    } else if (idx == PMPADDR23_INDEX) {
        write_csr(memattr23, attr);
    } else if (idx == PMPADDR24_INDEX) {
        write_csr(memattr24, attr);
    } else if (idx == PMPADDR25_INDEX) {
        write_csr(memattr25, attr);
    } else if (idx == PMPADDR26_INDEX) {
        write_csr(memattr26, attr);
    } else if (idx == PMPADDR27_INDEX) {
        write_csr(memattr27, attr);
    } else if (idx == PMPADDR28_INDEX) {
        write_csr(memattr28, attr);
    } else if (idx == PMPADDR29_INDEX) {
        write_csr(memattr29, attr);
    } else if (idx == PMPADDR30_INDEX) {
        write_csr(memattr30, attr);
    } else if (idx == PMPADDR31_INDEX) {
        write_csr(memattr31, attr);
#ifdef SUPPORT_RISCV70_PMP_NUM_64
    } else {
        hal_pmp_riscv70_regs_set_memxattr32_to_47(idx, attr);
#endif
    }
}
#endif

void hal_pmp_riscv70_regs_set_pmpxcfg8_to_15(uint32_t idx, hal_pmpx_config_t cfg)
{
    unused(idx);
    unused(cfg);
#ifdef SUPPORT_RISCV70_PMP_NUM_64
    hal_pmp_riscv70_cfg_t pmp_cfg;

    switch (idx / PMPXCFG_NUM_PER_REG) {
        case PMPCFG8_INDEX:
            pmp_cfg.d32 = read_csr(pmpcfg8);
            pmp_cfg.b[idx % PMPXCFG_NUM_PER_REG] = cfg;
            write_csr(pmpcfg8, pmp_cfg.d32);
            break;
        case PMPCFG9_INDEX:
            pmp_cfg.d32 = read_csr(pmpcfg9);
            pmp_cfg.b[idx % PMPXCFG_NUM_PER_REG] = cfg;
            write_csr(pmpcfg9, pmp_cfg.d32);
            break;
        case PMPCFG10_INDEX:
            pmp_cfg.d32 = read_csr(pmpcfg10);
            pmp_cfg.b[idx % PMPXCFG_NUM_PER_REG] = cfg;
            write_csr(pmpcfg10, pmp_cfg.d32);
            break;
        case PMPCFG11_INDEX:
            pmp_cfg.d32 = read_csr(pmpcfg11);
            pmp_cfg.b[idx % PMPXCFG_NUM_PER_REG] = cfg;
            write_csr(pmpcfg11, pmp_cfg.d32);
            break;
        case PMPCFG12_INDEX:
            pmp_cfg.d32 = read_csr(pmpcfg12);
            pmp_cfg.b[idx % PMPXCFG_NUM_PER_REG] = cfg;
            write_csr(pmpcfg12, pmp_cfg.d32);
            break;
        case PMPCFG13_INDEX:
            pmp_cfg.d32 = read_csr(pmpcfg13);
            pmp_cfg.b[idx % PMPXCFG_NUM_PER_REG] = cfg;
            write_csr(pmpcfg13, pmp_cfg.d32);
            break;
        case PMPCFG14_INDEX:
            pmp_cfg.d32 = read_csr(pmpcfg14);
            pmp_cfg.b[idx % PMPXCFG_NUM_PER_REG] = cfg;
            write_csr(pmpcfg14, pmp_cfg.d32);
            break;
        case PMPCFG15_INDEX:
            pmp_cfg.d32 = read_csr(pmpcfg15);
            pmp_cfg.b[idx % PMPXCFG_NUM_PER_REG] = cfg;
            write_csr(pmpcfg15, pmp_cfg.d32);
            break;
    }
#endif
}

void hal_pmp_riscv70_regs_set_pmpaddr(uint32_t idx, uint32_t pmpaddr)
{
    /* Because like that pmpaddr0 is RISCV CSR, so cannot be performed using table lookup mode. */
    if (idx == PMPADDR0_INDEX) {
        write_csr(pmpaddr0, pmpaddr);
    } else if (idx == PMPADDR1_INDEX) {
        write_csr(pmpaddr1, pmpaddr);
    } else if (idx == PMPADDR2_INDEX) {
        write_csr(pmpaddr2, pmpaddr);
    } else if (idx == PMPADDR3_INDEX) {
        write_csr(pmpaddr3, pmpaddr);
    } else if (idx == PMPADDR4_INDEX) {
        write_csr(pmpaddr4, pmpaddr);
    } else if (idx == PMPADDR5_INDEX) {
        write_csr(pmpaddr5, pmpaddr);
    } else if (idx == PMPADDR6_INDEX) {
        write_csr(pmpaddr6, pmpaddr);
    } else if (idx == PMPADDR7_INDEX) {
        write_csr(pmpaddr7, pmpaddr);
    } else if (idx == PMPADDR8_INDEX) {
        write_csr(pmpaddr8, pmpaddr);
    } else if (idx == PMPADDR9_INDEX) {
        write_csr(pmpaddr9, pmpaddr);
    } else if (idx == PMPADDR10_INDEX) {
        write_csr(pmpaddr10, pmpaddr);
    } else if (idx == PMPADDR11_INDEX) {
        write_csr(pmpaddr11, pmpaddr);
    } else if (idx == PMPADDR12_INDEX) {
        write_csr(pmpaddr12, pmpaddr);
    } else if (idx == PMPADDR13_INDEX) {
        write_csr(pmpaddr13, pmpaddr);
    } else if (idx == PMPADDR14_INDEX) {
        write_csr(pmpaddr14, pmpaddr);
    } else if (idx == PMPADDR15_INDEX) {
        write_csr(pmpaddr15, pmpaddr);
#if defined(SUPPORT_RISCV70_PMP_NUM_32) || defined(SUPPORT_RISCV70_PMP_NUM_64)
    } else {
        hal_pmp_riscv70_regs_set_pmpaddr16_to_63(idx, pmpaddr);
#endif
    }
}

void hal_pmp_riscv70_regs_set_memxattr(uint32_t idx, uint8_t attr)
{
    if (idx == PMPADDR0_INDEX) {
        write_csr(memattr0, attr);
    } else if (idx == PMPADDR1_INDEX) {
        write_csr(memattr1, attr);
    } else if (idx == PMPADDR2_INDEX) {
        write_csr(memattr2, attr);
    } else if (idx == PMPADDR3_INDEX) {
        write_csr(memattr3, attr);
    } else if (idx == PMPADDR4_INDEX) {
        write_csr(memattr4, attr);
    } else if (idx == PMPADDR5_INDEX) {
        write_csr(memattr5, attr);
    } else if (idx == PMPADDR6_INDEX) {
        write_csr(memattr6, attr);
    } else if (idx == PMPADDR7_INDEX) {
        write_csr(memattr7, attr);
    } else if (idx == PMPADDR8_INDEX) {
        write_csr(memattr8, attr);
    } else if (idx == PMPADDR9_INDEX) {
        write_csr(memattr9, attr);
    } else if (idx == PMPADDR10_INDEX) {
        write_csr(memattr10, attr);
    } else if (idx == PMPADDR11_INDEX) {
        write_csr(memattr11, attr);
    } else if (idx == PMPADDR12_INDEX) {
        write_csr(memattr12, attr);
    } else if (idx == PMPADDR13_INDEX) {
        write_csr(memattr13, attr);
    } else if (idx == PMPADDR14_INDEX) {
        write_csr(memattr14, attr);
    } else if (idx == PMPADDR15_INDEX) {
        write_csr(memattr15, attr);
#if defined(SUPPORT_RISCV70_PMP_NUM_32) || defined(SUPPORT_RISCV70_PMP_NUM_64)
    } else {
        hal_pmp_riscv70_regs_set_memxattr16_to_63(idx, attr);
#endif
    }
}

void hal_pmp_riscv70_regs_set_pmpxcfg(uint32_t idx, hal_pmpx_config_t cfg)
{
    hal_pmp_riscv70_cfg_t pmp_cfg;

    /* Because like that pmpaddr0 is RISCV CSR, so cannot be performed using table lookup mode. */
    switch (idx / PMPXCFG_NUM_PER_REG) {
        case PMPCFG0_INDEX:
            pmp_cfg.d32 = read_csr(pmpcfg0);
            pmp_cfg.b[idx % PMPXCFG_NUM_PER_REG] = cfg;
            write_csr(pmpcfg0, pmp_cfg.d32);
            break;
        case PMPCFG1_INDEX:
            pmp_cfg.d32 = read_csr(pmpcfg1);
            pmp_cfg.b[idx % PMPXCFG_NUM_PER_REG] = cfg;
            write_csr(pmpcfg1, pmp_cfg.d32);
            break;
        case PMPCFG2_INDEX:
            pmp_cfg.d32 = read_csr(pmpcfg2);
            pmp_cfg.b[idx % PMPXCFG_NUM_PER_REG] = cfg;
            write_csr(pmpcfg2, pmp_cfg.d32);
            break;
        case PMPCFG3_INDEX:
            pmp_cfg.d32 = read_csr(pmpcfg3);
            pmp_cfg.b[idx % PMPXCFG_NUM_PER_REG] = cfg;
            write_csr(pmpcfg3, pmp_cfg.d32);
            break;
#if defined(SUPPORT_RISCV70_PMP_NUM_32) || defined(SUPPORT_RISCV70_PMP_NUM_64)
        case PMPCFG4_INDEX:
            pmp_cfg.d32 = read_csr(pmpcfg4);
            pmp_cfg.b[idx % PMPXCFG_NUM_PER_REG] = cfg;
            write_csr(pmpcfg4, pmp_cfg.d32);
            break;
        case PMPCFG5_INDEX:
            pmp_cfg.d32 = read_csr(pmpcfg5);
            pmp_cfg.b[idx % PMPXCFG_NUM_PER_REG] = cfg;
            write_csr(pmpcfg5, pmp_cfg.d32);
            break;
        case PMPCFG6_INDEX:
            pmp_cfg.d32 = read_csr(pmpcfg6);
            pmp_cfg.b[idx % PMPXCFG_NUM_PER_REG] = cfg;
            write_csr(pmpcfg6, pmp_cfg.d32);
            break;
        case PMPCFG7_INDEX:
            pmp_cfg.d32 = read_csr(pmpcfg7);
            pmp_cfg.b[idx % PMPXCFG_NUM_PER_REG] = cfg;
            write_csr(pmpcfg7, pmp_cfg.d32);
            break;
#endif
        default:
            hal_pmp_riscv70_regs_set_pmpxcfg8_to_15(idx, cfg);
    }
}