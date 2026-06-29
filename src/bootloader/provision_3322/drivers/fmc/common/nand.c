/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include "types.h"
#include "errno.h"
#include "stdio.h"
#include "stdlib.h"
#include "common_def.h"
#include "watchdog.h"
#include "nand.h"
#include "nand_ops.h"
#include "debug_print.h"
#include "securec.h"
#include "nand_fmc.h"
#include "boot_serial.h"

#define DPAL_MTD_FAIL_ADDR_UNKNOWN (-1LL)
#define NAND_PAGE_MAX_SIZE 4096
#define NAND_OOB_MAX_SIZE 128
#define NAND_PAGE_MAX_BUFFER_SIZE (NAND_PAGE_MAX_SIZE + NAND_OOB_MAX_SIZE)
#define ERASE_ALL_TIMES 10

struct mtd_info *g_nand_mtd = NULL;
static uint8_t g_nand_page_write_buffer[NAND_PAGE_MAX_BUFFER_SIZE];
static uint8_t g_nand_page_read_buffer[NAND_PAGE_MAX_BUFFER_SIZE];
static uint32_t g_nand_page_write_count = 0;
static uint32_t g_nand_write_addr = 0;

void set_nand_write_pos(void)
{
    g_nand_write_addr = 0;
}

static int32_t nand_erase(struct mtd_info *mtd, struct erase_info *ops)
{
    int32_t ret = 0;
    struct nand_info *nand = mtd->priv;
    uint32_t addr = (uint32_t)ops->addr;
    uint32_t len = (uint32_t)ops->len;
    ops->fail_addr = DPAL_MTD_FAIL_ADDR_UNKNOWN;

    /* Start address must align on block boundary */
    if (addr & (mtd->erasesize - 1)) {
        boot_msg0("addr & (mtd->erasesize - 1)!");
        return -EINVAL;
    }

    if ((len + addr) > mtd->size) {
        boot_msg0("(len + addr) > mtd->size!");
        boot_msg4("addr,len,mtd->size,addr+len!", addr, len, mtd->size, addr + len);
        return -EINVAL;
    }

    if (addr < BOOT_NAND_BLOCK_NUM * nand->dev.blocksize) {
        boot_msg0("addr < BOOT_NAND_BLOCK_NUM * nand->dev.blocksize!");
        return -EINVAL;
    }

    for (; addr < ops->addr + len; addr += mtd->erasesize) {
        uint32_t times = 0;

        ret = nand_do_erase_ops(mtd, addr);
        while (times < ERASE_ALL_TIMES && ret != 0) {
            boot_msg1("erase block fail try again ;block addr:", addr);
            ret = nand_do_erase_ops(mtd, addr);
            times++;
        }
        uapi_watchdog_kick();
    }
    return 0;
}

static uint32_t get_len_incl_bad(struct mtd_info *mtd, uint32_t addr, const uint32_t length)
{
    uint32_t len_incl_bad = 0;
    uint32_t len_excl_bad = 0;
    struct nand_info *nand = mtd->priv;
    uint32_t block_len = 0;
    uint32_t offset = addr;
    while (len_excl_bad < length) {
        block_len = nand->dev.blocksize - (offset & (nand->dev.blocksize - 1));
        if (!nand_block_isbad(nand, offset & ~((uint32_t)nand->dev.blocksize - 1))) {
            len_excl_bad += block_len;
        }
        len_incl_bad += block_len;
        offset += block_len;
        if (offset > mtd->size) {
            break;
        }
    }
    return len_incl_bad;
}

static int32_t write_param_check(struct mtd_info *mtd, uint64_t addr, uint32_t len)
{
    if (addr < BOOT_NAND_BLOCK_NUM * mtd->erasesize) {
        boot_msg2("write param error!", addr, mtd->erasesize);
        return -EINVAL;
    }
    return 0;
}

static uint32_t nand_size_get(uint32_t len, uint32_t remain)
{
    uint32_t size;

    if (len < remain) {
        size = len;
    } else {
        size = remain;
    }

    return size;
}

static bool nand_oob_check(const uint8_t *buf, size_t ooblen)
{
    uint32_t i;
    for (i = 0; i < ooblen; i++) {
        if (buf[i] != 0xff) {
            break;
        }
    }
    return (i == ooblen);
}

static int32_t nand_write_check(struct mtd_info *mtd, struct mtd_oob_ops *ops)
{
    struct nand_info *nand = mtd->priv;
    if (nand_do_write_ops(mtd, g_nand_write_addr, ops)) {
        boot_msg1("nand_do_write_ops fail!", g_nand_write_addr);
        return -EIO;
    }
    uint8_t *nand_page_read_buffer = g_nand_page_read_buffer;
    ops->datbuf = (const char *)nand_page_read_buffer;
    ops->oobbuf = (const char *)(nand_page_read_buffer + mtd->writesize);
    if (nand_do_read_ops(nand, g_nand_write_addr, ops)) {
        boot_msg1("nand_do_read_ops fail!", g_nand_write_addr);
        return -EIO;
    }
    if (memcmp(nand_page_read_buffer, g_nand_page_write_buffer, NAND_PAGE_SIZE + NAND_OOB_SIZE) != 0) {
        boot_msg0("nand write fail!\n");
        g_nand_write_addr += mtd->writesize;
    }
    return 0;
}

static int32_t nand_write_block(struct mtd_info *mtd, uint32_t *write_len, const char **buf)
{
    uint32_t page_total_size = mtd->writesize + mtd->oobsize;
    struct mtd_oob_ops ops = {0};
    uint32_t len = *write_len;
    do {
        if ((len + g_nand_page_write_count) < page_total_size) {
            memcpy_s(g_nand_page_write_buffer + g_nand_page_write_count, len, *buf, len);
            g_nand_page_write_count += len;
            len = 0;
        }
        if (((len + g_nand_page_write_count) >= page_total_size)) {
            memcpy_s(g_nand_page_write_buffer + g_nand_page_write_count, page_total_size - g_nand_page_write_count, *buf,
                     page_total_size - g_nand_page_write_count);
            ops.datbuf = (const char *)g_nand_page_write_buffer;
            ops.oobbuf = (const char *)(g_nand_page_write_buffer + mtd->writesize);
            ops.len = mtd->writesize;
            ops.ooblen = mtd->oobsize;
            if (nand_oob_check((const uint8_t *)(g_nand_page_write_buffer + mtd->writesize), mtd->oobsize)) {
                PRINT("skip page addr:%x, len:%x!\n", g_nand_write_addr, ops.len);
            } else if (nand_write_check(mtd, &ops)) {
                return -EIO;
            }
            len -= (page_total_size - g_nand_page_write_count);
            *buf += (page_total_size - g_nand_page_write_count);
            g_nand_page_write_count = 0;
            g_nand_write_addr += mtd->writesize;
        }
    } while ((g_nand_write_addr & (mtd->erasesize - 1)) && len > 0);
    *write_len = len;
    return 0;
}

static int32_t nand_do_write(struct mtd_info *mtd, uint32_t len, const char *buf)
{
    while (len > 0) {
        uint32_t block_offset = g_nand_write_addr & (mtd->erasesize - 1);
        if (nand_block_isbad(mtd->priv, g_nand_write_addr & ~((uint32_t)mtd->erasesize - 1))) {
            g_nand_write_addr += mtd->erasesize - block_offset;
            continue;
        }
        if (nand_write_block(mtd, &len, &buf) != 0) {
            return -EIO;
        }
    }
    return 0;
}

static int32_t nand_write_oob(struct mtd_info *mtd, int64_t addr, uint32_t len, const char *buffer)
{
    const char *buf = buffer;
    uint32_t len_incl_bad, len_data;

    if (write_param_check(mtd, (uint32_t)addr, len)) {
        boot_msg1("addr error ", addr);
        return -EINVAL;
    }
    if (g_nand_write_addr == 0) {
        g_nand_write_addr = (uint32_t)addr;
    }

    len_data = len;
    len_incl_bad = get_len_incl_bad(mtd, g_nand_write_addr, len_data);
    if ((g_nand_write_addr + len_incl_bad) > mtd->size) {
        boot_msg1("mtd size ", mtd->size);
        boot_msg1("write addr ", g_nand_write_addr);
        boot_msg1("len_incl_bad ", len_incl_bad);
        return -EINVAL;
    }

    if (nand_do_write(mtd, len, buf) != 0) {
        return -EIO;
    }
    return 0;
}

static int32_t read_oob_param_check(struct mtd_info *mtd, uint64_t addr, uint32_t len)
{
    if (addr & (mtd->writesize - 1)) {
        boot_msg1("Attempt to read non page aligned data, from", (uint32_t)addr);
        return -EINVAL;
    }
    if ((len % (mtd->writesize + mtd->oobsize)) != 0) {
        boot_msg1("Attempt to read non aligned data, length", (uint32_t)len);
        boot_msg2("pagesize, oobsize ", mtd->writesize, mtd->oobsize);
        return -EINVAL;
    }

    return 0;
}

static int32_t nand_read_oob(struct mtd_info *mtd, int64_t addr, uint32_t len, uint32_t *retlen, char *buf)
{
    unused(retlen);
    struct nand_info *nand = mtd->priv;
    struct mtd_oob_ops ops = {0};
    uint32_t left_to_read, len_incl_bad, len_data;
    uint32_t from = (uint32_t)addr;

    if (read_oob_param_check(mtd, addr, len)) {
        return -EINVAL;
    }

    len_data = len / (mtd->writesize + mtd->oobsize) * mtd->writesize;
    len_incl_bad = get_len_incl_bad(mtd, from, len_data);
    if ((from + len_incl_bad) > mtd->size) {
        return -EINVAL;
    }

    if (len_incl_bad == len_data) {
        for (uint32_t i = 0; i < len_data / mtd->writesize; i++) {
            ops.datbuf = (const char *)buf + i * (mtd->writesize + mtd->oobsize);
            ops.oobbuf = (const char *)buf + i * (mtd->writesize + mtd->oobsize) + mtd->writesize;
            ops.len = mtd->writesize;
            ops.ooblen = mtd->oobsize;

            if (nand_do_read_ops(nand, from + i * mtd->writesize, &ops)) {
                return -EIO;
            }
        }

        return 0;
    }

    left_to_read = len_data;
    while (left_to_read > 0) {
        uint32_t block_from = from & (mtd->erasesize - 1);
        uint32_t read_size;

        if (nand_block_isbad(nand, from & ~((uint32_t)mtd->erasesize - 1))) {
            from += mtd->erasesize - block_from;
            continue;
        }

        read_size = nand_size_get(left_to_read, mtd->erasesize - block_from);

        for (uint32_t i = 0; i < read_size / mtd->writesize; i++) {
            ops.datbuf = (const char *)buf;
            ops.oobbuf = (const char *)buf + mtd->writesize;
            ops.len = mtd->writesize;
            ops.ooblen = mtd->oobsize;

            if (nand_do_read_ops(nand, from, &ops)) {
                return -EIO;
            }
            buf += mtd->writesize + mtd->oobsize;
            left_to_read -= mtd->writesize;
            from += mtd->writesize;
        }
    }

    return 0;
}

static int32_t block_isbad(struct mtd_info *mtd, int64_t ofs)
{
    struct nand_info *nand = mtd->priv;
    return nand_block_isbad(nand, (uint32_t)ofs);
}

static int32_t block_markbad(struct mtd_info *mtd, int64_t ofs)
{
    struct nand_info *nand = mtd->priv;
    return nand_block_markbad(nand, (uint32_t)ofs);
}

void nand_register(struct mtd_info *mtd)
{
    struct nand_info *nand;

    nand = mtd->priv;

    mtd->size = nand->dev.chipsize;
    mtd->erasesize = nand->dev.blocksize;
    mtd->writesize = nand->dev.pagesize;
    mtd->oobsize = nand->dev.oobsize;
    mtd->name = "nand";
    mtd->type = MTD_NANDFLASH;
    mtd->flags = MTD_CAP_NANDFLASH;

    mtd->erase = nand_erase;
    mtd->read = nand_read_oob;
    mtd->write = nand_write_oob;
    mtd->block_isbad = block_isbad;
    mtd->block_markbad = block_markbad;
}