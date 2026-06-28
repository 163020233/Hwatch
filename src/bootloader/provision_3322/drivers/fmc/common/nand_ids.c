/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 */
#include "types.h"
#include "debug_print.h"
#include "string.h"
#include "nand_ids.h"
#include "hifmc100.h"
#include "nandflash_ds35x1g.h"

#define BIT_16  16
#define BIT_8   8
#define BIT_4   4
#define BIT_2   2

int32_t ffs(int32_t x)
{
    int32_t r = 1;
    uint32_t tmp = (unsigned int)x;

    if (x == 0) {
        return 0;
    }
    if ((tmp & 0xffff) == 0) {
        tmp >>= BIT_16;
        r += BIT_16;
    }
    if ((tmp & 0xff) == 0) {
        tmp >>= BIT_8;
        r += BIT_8;
    }
    if ((tmp & 0xf) == 0) {
        tmp >>= BIT_4;
        r += BIT_4;
    }
    if ((tmp & 0x3) == 0) {
        tmp >>= BIT_2;
        r += BIT_2;
    }
    if ((tmp & 0x1) == 0) {
        r += 1;
    }
    return r;
}

struct spi_drv spi_driver_general = {
    .qe_enable = spinand_general_qe_enable,
};

struct spi_drv spi_driver_no_qe = {
    .qe_enable = spinand_qe_not_enable,
};

struct spi_drv spi_driver_buf_enable = {
    .qe_enable = spinand_buf_enable,
};

static struct nand_flash_info nand_flash_info_t[] = {
    {       /* SLC 4bit/512 1.8V */
        .name      = "DS35M1GA",
        .id        = {0xE5, 0x21},
        .id_len    = 2,  /* 2 is id len */
        .chipsize  = _128M,
        .pagesize  = _2K,
        .blocksize = _128K,
        .oobsize   = 64,  /* 64 is oobsize */
        .ecctype   = NAND_ECC_0BIT,
        .badblock_pos = BBP_FIRST_PAGE,
        .driver    = &spi_driver_general,
        .oob_format = ds35x1g_oob_format,
    },
    {       /* SLC 4bit/512 1.8V */
        .name      = "FM25LS01",
        .id        = {0xa1, 0xa5},
        .id_len    = 2,  /* 2 is id len */
        .chipsize  = _128M,
        .pagesize  = _2K,
        .blocksize = _128K,
        .oobsize   = 64,  /* 64 is oobsize */
        .ecctype   = NAND_ECC_0BIT,
        .badblock_pos = BBP_FIRST_PAGE,
        .driver    = &spi_driver_general,
    },
    {
        .name      = "XT26Q04D",
        .id        = {0x0B, 0x53},
        .id_len    = 2,  /* 2 is id len */
        .chipsize  = _512M,
        .pagesize  = _4K,
        .blocksize = _256K,
        .oobsize   = 128,  /* 128 is oobsize */
        .ecctype   = NAND_ECC_0BIT,
        .badblock_pos = BBP_FIRST_PAGE,
        .driver    = &spi_driver_general,
    },
    {
        .name      = "XT26Q02D",
        .id        = {0x0B, 0x52},
        .id_len    = 2,  /* 2 is id len */
        .chipsize  = _256M,
        .pagesize  = _2K,
        .blocksize = _128K,
        .oobsize   = 64,  /* 128 is oobsize */
        .ecctype   = NAND_ECC_0BIT,
        .badblock_pos = BBP_FIRST_PAGE,
        .driver    = &spi_driver_general,
    },
    {
        .name      = "DS35M4GM-IB",
        .id        = {0xE5, 0xA4},
        .id_len    = 2,  /* 2 is id len */
        .chipsize  = _512M,
        .pagesize  = _2K,
        .blocksize = _128K,
        .oobsize   = 64,  /* 64 is oobsize */
        .ecctype   = NAND_ECC_0BIT,
        .badblock_pos = BBP_FIRST_PAGE,
        .driver    = &spi_driver_general,
    },
    {
        .name      = "DS35M4GM-IB",
        .id        = {0xE5, 0x64},
        .id_len    = 2,  /* 2 is id len */
        .chipsize  = _512M,
        .pagesize  = _2K,
        .blocksize = _128K,
        .oobsize   = 64,  /* 64 is oobsize */
        .ecctype   = NAND_ECC_0BIT,
        .badblock_pos = BBP_FIRST_PAGE,
        .driver    = &spi_driver_general,
    },
    {
        .name      = "W25N01GW-IE",
        .id        = {0xEF, 0xBA, 0x21},
        .id_len    = 3,  /* 3 is id len */
        .chipsize  = _128M,
        .pagesize  = _2K,
        .blocksize = _128K,
        .oobsize   = 64,  /* 64 is oobsize */
        .ecctype   = NAND_ECC_0BIT,
        .badblock_pos = BBP_FIRST_PAGE,
        .driver    = &spi_driver_buf_enable,
    },
    {
        .name      = "DS35M2GBS-IB",
        .id        = {0xE5, 0x62},
        .id_len    = 2,  /* 2 is id len */
        .chipsize  = _256M,
        .pagesize  = _2K,
        .blocksize = _128K,
        .oobsize   = 64,  /* 64 is oobsize */
        .ecctype   = NAND_ECC_0BIT,
        .badblock_pos = BBP_FIRST_PAGE,
        .driver    = &spi_driver_general,
    },
    {
        .name      = "GD5F2GM7RE",
        .id        = {0xC8, 0x82},
        .id_len    = 2,  /* 2 is id len */
        .chipsize  = _256M,
        .pagesize  = _2K,
        .blocksize = _128K,
        .oobsize   = 64,  /* 64 is oobsize */
        .ecctype   = NAND_ECC_0BIT,
        .badblock_pos = BBP_FIRST_PAGE,
        .driver    = &spi_driver_general,
    },
    {NULL, {0}, 0, 0, 0, 0, 0, 0, 0, NULL, NULL},
};


struct nand_dev_info *nand_get_dev_info_by_id(struct nand_info *nand)
{
    uint8_t *id = nand->dev.id;
    struct nand_dev_info *find = &(nand->dev);
    struct nand_flash_info *cur;

    for (cur = nand_flash_info_t; cur->id_len; cur++) {
        if (cur->id[0] && memcmp(id, cur->id, cur->id_len)) {
            continue;
        }

        if ((cur->id_len == 0x2) && (id[1] != cur->id[1])) {
            continue;
        }

        find->name = cur->name;
        find->id_len = cur->id_len;
        find->chipsize = cur->chipsize;
        find->pagesize = cur->pagesize;
        find->oobsize = cur->oobsize;
        find->blocksize = cur->blocksize;
        find->ecctype = cur->ecctype;
   
        find->page_shift = (uint32_t)ffs((int32_t)find->pagesize) - 1;  // 11
        find->pagemask = find->pagesize - 1;
        find->block_shift = (uint32_t)ffs((int32_t)find->blocksize) - 1; // 17
        find->blockmask = find->blocksize - 1;
        if (find->chipsize & 0xffffffff) {
            find->chip_shift = (uint32_t)ffs((int32_t)find->chipsize) - 1;
        } else {
            find->chip_shift = (uint32_t)ffs((int32_t)(find->chipsize >> 32)) + 31; /* 31 32 is magic */
        }
        find->priv = cur;
        nand->ids_probe(nand);
        return find;
    }

    PRINT("Not found nand flash!!!\n");

    return NULL;
}