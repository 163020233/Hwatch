/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides RISCV pmp register \n
 *
 * History: \n
 * 2022-09-24, Create file. \n
 */
#ifndef HAL_PMP_RISCV70_REGS_DEF_H
#define HAL_PMP_RISCV70_REGS_DEF_H

#include <stdint.h>
#include "hal_pmp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_pmp_riscv70_regs_def PMP RISCV70 Regs Definition
 * @ingroup  drivers_hal_pmp
 * @{
 */

#define PMPXCFG_NUM_PER_REG     4
#if defined SUPPORT_RISCV70_PMP_NUM_64
#define PMP_REGION_NUM          64
#elif defined SUPPORT_RISCV70_PMP_NUM_32
#define PMP_REGION_NUM          32
#else
#define PMP_REGION_NUM          16
#endif
#define PMPCFG0_INDEX    0
#define PMPCFG1_INDEX    1
#define PMPCFG2_INDEX    2
#define PMPCFG3_INDEX    3
#define PMPCFG4_INDEX    4
#define PMPCFG5_INDEX    5
#define PMPCFG6_INDEX    6
#define PMPCFG7_INDEX    7
#define PMPCFG8_INDEX    8
#define PMPCFG9_INDEX    9
#define PMPCFG10_INDEX   10
#define PMPCFG11_INDEX   11
#define PMPCFG12_INDEX   12
#define PMPCFG13_INDEX   13
#define PMPCFG14_INDEX   14
#define PMPCFG15_INDEX   15

#define PMPADDR0_INDEX    0
#define PMPADDR1_INDEX    1
#define PMPADDR2_INDEX    2
#define PMPADDR3_INDEX    3
#define PMPADDR4_INDEX    4
#define PMPADDR5_INDEX    5
#define PMPADDR6_INDEX    6
#define PMPADDR7_INDEX    7
#define PMPADDR8_INDEX    8
#define PMPADDR9_INDEX    9
#define PMPADDR10_INDEX   10
#define PMPADDR11_INDEX   11
#define PMPADDR12_INDEX   12
#define PMPADDR13_INDEX   13
#define PMPADDR14_INDEX   14
#define PMPADDR15_INDEX   15
#define PMPADDR16_INDEX   16
#define PMPADDR17_INDEX   17
#define PMPADDR18_INDEX   18
#define PMPADDR19_INDEX   19
#define PMPADDR20_INDEX   20
#define PMPADDR21_INDEX   21
#define PMPADDR22_INDEX   22
#define PMPADDR23_INDEX   23
#define PMPADDR24_INDEX   24
#define PMPADDR25_INDEX   25
#define PMPADDR26_INDEX   26
#define PMPADDR27_INDEX   27
#define PMPADDR28_INDEX   28
#define PMPADDR29_INDEX   29
#define PMPADDR30_INDEX   30
#define PMPADDR31_INDEX   31
#define PMPADDR32_INDEX   32
#define PMPADDR33_INDEX   33
#define PMPADDR34_INDEX   34
#define PMPADDR35_INDEX   35
#define PMPADDR36_INDEX   36
#define PMPADDR37_INDEX   37
#define PMPADDR38_INDEX   38
#define PMPADDR39_INDEX   39
#define PMPADDR40_INDEX   40
#define PMPADDR41_INDEX   41
#define PMPADDR42_INDEX   42
#define PMPADDR43_INDEX   43
#define PMPADDR44_INDEX   44
#define PMPADDR45_INDEX   45
#define PMPADDR46_INDEX   46
#define PMPADDR47_INDEX   47
#define PMPADDR48_INDEX   48
#define PMPADDR49_INDEX   49
#define PMPADDR50_INDEX   50
#define PMPADDR51_INDEX   51
#define PMPADDR52_INDEX   52
#define PMPADDR53_INDEX   53
#define PMPADDR54_INDEX   54
#define PMPADDR55_INDEX   55
#define PMPADDR56_INDEX   56
#define PMPADDR57_INDEX   57
#define PMPADDR58_INDEX   58
#define PMPADDR59_INDEX   59
#define PMPADDR60_INDEX   60
#define PMPADDR61_INDEX   61
#define PMPADDR62_INDEX   62
#define PMPADDR63_INDEX   63

/**
 * @brief  This union represents the bit fields in the pmpaddr register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the region config using the <i>b</i> elements.
 */
typedef union hal_pmpcfg {
    uint32_t d32;                           /*!< Raw register data. */
    hal_pmpx_config_t b[PMPXCFG_NUM_PER_REG];  /*!< PMP regions configurations. */
} hal_pmp_riscv70_cfg_t;

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif