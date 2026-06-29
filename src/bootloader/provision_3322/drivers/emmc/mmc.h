/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description:  nonos mmc header.
 * Author: HiSilicon
 * Create: 2025-08-21
 */
#ifndef HI3322_SOC_MMC_H
#define HI3322_SOC_MMC_H

#include "emmc_type.h"

#define DRV_PHASE_112P5_DEGREE 10  /* 112.5 degree */
#define DRV_PHASE_258P75_DEGREE 23 /* 258.75 degree */
#define DRV_PHASE_225_DEGREE 20    /* 225 degree */
#define DRV_PHASE_180_DEGREE 16    /* 180 degree */

/* M_CTL_RB Base address of Module's Register */
#define M_CTL_RB_BASE (0x52000000)

#define EMMC_CCLK_DIV_REG (M_CTL_RB_BASE + 0x0100)
#define EMMC_CCLK_ORI_DIV_NUM_MASK 0x0FF0
#define EMMC_CCLK_ORI_DIV_NUM_2 0x20
#define EMMC_TUNING_BM0_REG (M_CTL_RB_BASE + 0x104)
#define EMMC_TUNING_BM1_REG (M_CTL_RB_BASE + 0x108)
#define SD_HOST_CCLK_DIV_REG (M_CTL_RB_BASE + 0x0110)
#define SDIO_HOST_CCLK_DIV_REG (M_CTL_RB_BASE + 0x014C)

#define EMMC_TUNING_BM0_VAL (0x4210)
#define EMMC_TUNING_BM1_VAL (0x707)
#define EMMC_TUNING_COARSE_MODE BIT(13)

#define MSHC_WAIT_INT_PER_MSEC 1

#define SAMPLE_PHASE 4
#define DEFAULT_PHASE 0x10

#define EMMC_CAPCITY_SIZE get_emmc_capcity_size() /* emmc capcity size, unit: bytes */
#define RESERVED_BOOT_SECTOR 2048                 /* reserved sector number for boot */
#define SECTOR_NUM_PER_CLUSTER 8                  /* cluster size, unit: sector */
#define GB (1024ULL * 1024ULL * 1024ULL)

#define MMC_SECTOR_SIZE 512

#define EMMC_SECTOR_FOR_BURN (EMMC_CAPCITY_SIZE / MMC_SECTOR_SIZE - RESERVED_BOOT_SECTOR)
#define EMMC_SIZE_FOR_BURN (EMMC_SECTOR_FOR_BURN * MMC_SECTOR_SIZE)

int emmc_drv_context_init(void);
uint32_t emmc_raw_write(char *buffer, uint32_t start_sector, uint32_t nsectors);
uint32_t emmc_raw_read(char *buffer, uint32_t start_sector, uint32_t nsectors);
int EraseDiskByID(uint32_t diskId, size_t startSector, uint32_t sectors);
void nonos_disk_init(void);
uint32_t *get_part_start_sector(void);
uint32_t *get_part_count_sector(void);

#endif
