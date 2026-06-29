/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Platform Config HeadFile
 *
 * Create: 2025-03-20
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

#ifndef _SOC_MMC_H
#define _SOC_MMC_H

#include <stdint.h>
#include <stdbool.h>
#include "mmc_porting.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define PRE_FPGA
#define EMMC_BOARD_DEBUG_ON 0

#ifndef BIT
#define BIT(x) (1UL << (uint32_t)(x))
#endif

#define MAX_MMC_NUM 1
#define USE_MMC0 1

#define HISDEMMC_POWER_CONTROL              0x29
#define ARGUMENT                (EMMC_REG_BASE_ADDESS + 0x8)
#define CMD                     (EMMC_REG_BASE_ADDESS + 0xE)
#define RESP01                  (EMMC_REG_BASE_ADDESS + 0x10)
#define RESP23                  (EMMC_REG_BASE_ADDESS + 0x14)
#define RESP45                  (EMMC_REG_BASE_ADDESS + 0x18)
#define RESP67                  (EMMC_REG_BASE_ADDESS + 0x1C)
#define PSTATE_REG              (EMMC_REG_BASE_ADDESS + 0x24)
#define HOST_CTRL1              (EMMC_REG_BASE_ADDESS + 0x28)
#define HIGH_SPEED_EN_BIT       0x2
#define EXT_DAT_XFER_BIT        0x5
#define PWR_CTRL                (EMMC_REG_BASE_ADDESS + 0x29)
#define POWR_ON_1_8V            0xBF
#define CLK_CTRL                (EMMC_REG_BASE_ADDESS + 0x2C)
#define TOUT_CTRL               (EMMC_REG_BASE_ADDESS + 0x2E)
#define SW_RST                  (EMMC_REG_BASE_ADDESS + 0x2F)
#define SW_RST_ALL              BIT(0)
#define SW_RST_CMD              BIT(1)
#define SW_RST_DAT              BIT(2)
#define SW_RE_RST_ALL           (SW_RST_ALL | SW_RST_CMD | SW_RST_DAT)
#define NORMAL_INT_STAT         (EMMC_REG_BASE_ADDESS + 0x30)
#define ERROR_INT_STAT          (EMMC_REG_BASE_ADDESS + 0x32)
#define ALL_NORMAL_ERR_INT      0xffff
#define NORMAL_INT_STAT_EN      (EMMC_REG_BASE_ADDESS + 0x34)
#define ERROR_INT_STAT_EN       (EMMC_REG_BASE_ADDESS + 0x36)
#define NORMAL_ERR_INT_EN       0x7EFF
#define NORMAL_INT_SIGNAL_EN    (EMMC_REG_BASE_ADDESS + 0x38)
#define ERROR_INT_SIGNAL_EN     (EMMC_REG_BASE_ADDESS + 0x3A)
#define HOST_CTRL2              (EMMC_REG_BASE_ADDESS + 0x3E)
#define SAMPLE_CLK_SEL_BIT      0x7
#define SIGNALING_EN_BIT        0x3
#define UHS_MODE_SEL_BIT        0x0
#define UHS_MODE_SEL_LEN        0x3
#define HS_SDR                  0x1
#define HS_DDR                  0x4
#define HS200                   0x3
#define HS400                   0x7
#define MBIIU_CTRL              (EMMC_REG_BASE_ADDESS + DWC_MSHC_PTR_VENDOR1 + 0x10)
#define AT_CTRL                 (EMMC_REG_BASE_ADDESS + DWC_MSHC_PTR_VENDOR1 + 0x40)
#define AT_STAT                 (EMMC_REG_BASE_ADDESS + DWC_MSHC_PTR_VENDOR1 + 0x44)
#define EMMC_CTRL               (EMMC_REG_BASE_ADDESS + DWC_MSHC_PTR_VENDOR1 + 0x2C)
#define LEN_3BIT                0x3
#define LEN_8BIT                0x8

#define SDIO_HOST_CLKEN BIT(0)
#define SDIO_CCLK_ORI_DIV_EN BIT(1)
#define SDIO_CCLK_ORI_LOAD_DIV_EN BIT(2)

#define EMMC_TUNING_BM0_VAL (0x4210)
#define EMMC_TUNING_BM1_VAL (0x707)
#define EMMC_TUNING_COARSE_MODE BIT(13)

#define SDIO_HOST_CLKEN BIT(0)
#define SDIO_CCLK_ORI_DIV_EN BIT(1)
#define SDIO_CCLK_ORI_LOAD_DIV_EN BIT(2)

#define SDIO_EMMC_DIV_NUM_POST_SHIFT 4

#define CONFIG_MMC0_CCLK_MIN 400000 // 400KHz

#define SDIO_EMMC_DIV_NUM_MASK 0xFF0

#define SDIOM_SRC_CLK_LOW_FREQ_VAL 32000000 // 32MHz
#define SDIOM_SRC_CLK_FREQ_LVL_50M_VAL 52000000 // 52MHz
#define SDIOM_SRC_CLK_FREQ_LVL_64M_VAL 64000000 // 64MHz
#define SDIOM_SRC_CLK_FREQ_LVL_49M_VAL 49000000
#define SDIOM_SRC_CLK_FREQ_LVL_98M_VAL 98000000 // 98MHz
#define SDIOM_SRC_CLK_HIGH_FREQ_LVL_VAL 196000000 // 196MHz

#define DRV_PHASE_112P5_DEGREE 10 /* 112.5 degree */
#define DRV_PHASE_258P75_DEGREE 23 /* 258.75 degree */
#define DRV_PHASE_225_DEGREE 20 /* 225 degree */
#define DRV_PHASE_180_DEGREE 16  /* 180 degree */

#define SAMPLE_PHASE 4

#define MSHC_WAIT_INT_PER_MSEC 1

#define CMU_CLOCKS_SDIO_RST_DEVICE_SD_BIT 0

#define reg32(register)                             (*(volatile uint32_t*)(uintptr_t)(register))
#define reg16(register)                             (*(volatile uint16_t*)(uintptr_t)(register))
#define reg8(register)                              (*(volatile uint8_t*)(uintptr_t)(register))

#define reg32_clrbit(register, pos)           (reg32(register) &= ~((uint32_t)((1U) << (uint32_t)(pos))))
#define reg16_clrbit(register, pos)           (reg16(register) &= ~((uint16_t)((1U) << (uint16_t)(pos))))
#define reg8_clrbit(register, pos)            (reg8(register) &= ~((uint8_t)((1U) << (pos))))

#define reg32_setbit(register, pos)           (reg32(register) |= ((uint32_t)((1U) << (uint32_t)(pos))))
#define reg16_setbit(register, pos)           (reg16(register) |= ((uint16_t)((1U) << (uint16_t)(pos))))
#define reg8_setbit(register, pos)            (reg8(register) |= ((uint8_t)((1U) << (pos))))

#define reg16_setbits(r, p, b, v) (reg16(r) = (reg16(r) & \
                                              (uint16_t)(~((((uint32_t)1 << (uint16_t)(b)) - 1) << (uint16_t)(p)))) \
                                              | (uint16_t)((uint32_t)((uint16_t)(v) & \
                                              (((uint32_t)1 << (uint16_t)(b)) - 1)) << (uint16_t)(p)))

/* EMMC and SDIO_HOST peripheral clock gating. */
#define PMU_MTOP4_SDIO_HOST_RST         1
#define PMU_MTOP4_EMMC_RST              0

#define RESERVED_BOOT_SECTOR 2048        /* reserved sector number for boot */
#define SECTOR_NUM_PER_CLUSTER 8         /* cluster size, unit: sector */
#define GB (1024ULL * 1024ULL * 1024ULL)
#define EMMC_CAPCITY_SIZE (4ULL * GB) /* emmc capcity size, unit: bytes */

/**
 * @brief  Describe pheriphral request behavior.
 */
typedef enum {
    HAL_PMU_PERIP_REQUEST_RESET = 0,   // !< Request reset the peripheral, make it in initial status, can't running.
    HAL_PMU_PERIP_REQUEST_DERESET = 1, // !< Request dereset the peripheral, make it can run.
} hal_pmu_perip_request_type_t;

typedef enum {
    HAL_SOFT_RST6_SDIO_CRG,
    HAL_SOFT_RST6_SDIO_LGC,
} hal_glb_sdio_soft_rst_type_t;

/**
 * @brief  Describe mcpu pheriphrals.
 */
typedef enum {
    HAL_PMU_MCPU_PERIP_EMMC         = 0,   // !< The mcpu peripheral EMMC
    HAL_PMU_MCPU_PERIP_SDIO_HOST    = 1,   // !< The mcpu peripheral SDIO HOST
} hal_pmu_mcpu_perips_type_t;

#ifndef udelay
#define udelay(n)   LOS_Udelay(n)
#endif

#ifndef msleep
#define	msleep(n)	do {\
    if (OS_SCHEDULER_ACTIVE) {\
        LOS_Msleep(n); \
    } \
    else {\
        LOS_Mdelay(n); \
    } \
}while(0);
#endif

void emmc_save_info(void);
void emmc_recovery_init(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _MMC_H */

