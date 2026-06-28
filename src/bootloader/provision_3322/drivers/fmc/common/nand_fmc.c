/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include "types.h"
#include "debug_print.h"
#include "nand.h"
#include "nand_fmc.h"
#include "errno.h"
#include "securec.h"
#include "boot_serial.h"

static bool g_nand_inited = false;

static int32_t nand_write_skip_bad(struct mtd_info *mtd, uint64_t offset, uint32_t length, const char *buffer)
{
    if (mtd == NULL) {
        PRINT("not init g_nand_mtd!!!\n");
        return -ENODEV;
    }

    return mtd->write(mtd, offset, length, buffer);
}

static int32_t nand_read_skip_bad(struct mtd_info *mtd, uint64_t offset, uint32_t length, char *buffer)
{
    uint32_t rw_size = length;

    if (mtd == NULL) {
        PRINT("not init g_nand_mtd!!!\n");
        return -ENODEV;
    }
    return mtd->read(mtd, offset, length, &rw_size, buffer); /* nand_read */
}

int32_t nand_init(void)
{
    return nand_flash_init();
}

int32_t fmc_nand_erase(uint64_t start, uint32_t size)
{
    struct erase_info opts;
    if (g_nand_mtd == NULL) {
        PRINT("not init g_nand_mtd!!!\n");
        return -ENODEV;
    }

    uint32_t erase_size = size / (g_nand_mtd->writesize + g_nand_mtd->oobsize) * g_nand_mtd->writesize;

    memset_s(&opts, sizeof(opts), 0, sizeof(opts));
    opts.addr = (uint64_t)start;
    opts.len = (uint64_t)erase_size;

    return g_nand_mtd->erase(g_nand_mtd, &opts);
}

int32_t fmc_nand_write(void *memaddr, uint64_t start, uint32_t size)
{
    if (g_nand_mtd == NULL) {
        PRINT("not init g_nand_mtd!!!\n");
        return -ENODEV;
    }

    return nand_write_skip_bad(g_nand_mtd, start, size, (const char *)memaddr);
}

int32_t fmc_nand_read(void *memaddr, uint64_t start, uint32_t size)
{
    if (g_nand_mtd == NULL) {
        return -ENODEV;
    }
    return nand_read_skip_bad(g_nand_mtd, start, size, (char *)memaddr);
}

uint32_t get_nandsize_for_burn(void)
{
    if (!g_nand_inited) {
        int32_t ret = nand_init();
        if (ret != 0) {
            boot_msg0("nandflash init fail");
            return 0;
        }
        g_nand_inited = true;
    }
    return g_nand_mtd->size - BOOT_NAND_BLOCK_NUM * g_nand_mtd->erasesize;
}

uint32_t get_erasesize_incl_oob(uint32_t erase_size)
{
    uint32_t size = erase_size / g_nand_mtd->writesize;
    if (erase_size % g_nand_mtd->writesize != 0) {
        size = (g_nand_mtd->writesize + g_nand_mtd->oobsize) * (erase_size / g_nand_mtd->writesize + 1);
    } else {
        size = (g_nand_mtd->writesize + g_nand_mtd->oobsize) * (erase_size / g_nand_mtd->writesize);
    }
    return size;
}