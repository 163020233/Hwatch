/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __NAND_H__
#define __NAND_H__

#include "nand_common.h"

#define MTD_NANDFLASH 4
#define MTD_WRITEABLE 0x400
#define MTD_CAP_NANDFLASH (MTD_WRITEABLE)

extern struct mtd_info *g_nand_mtd;

#define NAND_BLOCK_SIZE (g_nand_mtd->erasesize)
#define NAND_PAGE_SIZE (g_nand_mtd->writesize)
#define NAND_OOB_SIZE (g_nand_mtd->oobsize)

void nand_register(struct mtd_info *mtd);
int32_t nand_flash_init(void);
struct dpal_resource* get_hifmc100_resources(uint8_t index);
struct dpal_platform_device* get_device_hifmc100_nand(void);

#endif /* End of __NAND_H__ */
