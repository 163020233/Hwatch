/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __NAND_OPS_H__
#define __NAND_OPS_H__

#include "nand_common.h"

#define page_addr(addr, page_shift) (uint32_t)((addr) >> ((page_shift)))

struct nand_block_operator_pos {
    uint32_t erase_block_start;
    uint32_t read_page_start;
    uint32_t write_page_start;
    uint32_t read_page_offset;
    uint32_t write_page_offset;
};

#define BLOCK_OPERA_START 0x1

int32_t nand_block_markbad(struct nand_info *nand, uint32_t addr);
int32_t nand_block_isbad(struct nand_info *nand, uint32_t addr);
int32_t nand_do_erase_ops(struct mtd_info *mtd, uint32_t addr);
int32_t nand_do_write_ops(struct mtd_info *mtd, uint32_t to, struct mtd_oob_ops *ops);
int32_t nand_do_read_ops(struct nand_info *nand, uint32_t from, struct mtd_oob_ops *ops);

#endif /* End of __NAND_OPS_H__ */
