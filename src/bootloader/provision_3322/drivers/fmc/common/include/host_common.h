/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __HOST_COMMON_H__
#define __HOST_COMMON_H__

#define GET_OP                    0
#define SET_OP                    1

/* ECC type macros and enum  */
#define NAND_ECC_0BIT          0
#define NAND_ECC_8BIT_1K       1
#define NAND_ECC_16BIT_1K      2
#define NAND_ECC_24BIT_1K      3
#define NAND_ECC_28BIT_1K      4
#define NAND_ECC_40BIT_1K      5
#define NAND_ECC_64BIT_1K      6

#define NAND_BAD_BLOCK_POS        0
#define NAND_EMPTY_BLOCK_POS        30

struct nand_host {
    struct nand_info *nand;

    void *priv;

    char *regbase;
    char *membase;

    uint32_t cfg;
    uint32_t cfg_ecc0;

    uint32_t pagesize;
    uint32_t oobsize;
    uint32_t ecctype;
    uint32_t addr_cycle;

    // use for dma transfer
    char *buforg;
    char *buffer;
    char *dma_data;
    char *dma_oob;

    char *bbm; /* nand bad block mark */
    char *ebm; /* nand empty block mark */

    uint32_t flags;
    uint16_t version;
};

#endif /* End of __HOST_COMMON_H__ */

