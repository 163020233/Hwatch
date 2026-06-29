/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: 3322 fapc header file for SELiteOS.
 */

#ifndef FAPC_H
#define FAPC_H

#include "stdint.h"
#include "seliteos_memory_config.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SELITEOS_FAPC_REGION_NUM    7
#define FAPC_TEE_MID                (1 << 0)
#define FAPC_REE_MID                (1 << 1)

#define ENC_FLAG_OFFSET             0x148
#define ENC_FLAG_ITCM_ADDR          (TEE_ITCM_ORIGIN + ENC_FLAG_OFFSET)
#define NON_ENC_FLAG                0x3C7896E1
#define SALT_LEN                    0x1C
#define RESERVED_LEN                0x4
#define KEY_IV_LEN                  0x10

#define ERASE_CHIP_CMD              0xC7
#define ERASE_64K_CMD               0xD8
#define ERASE_32K_CMD               0x52
#define ERASE_4K_CMD                0x20
#define BLOCK_SIZE                  0x1000

typedef struct {
    uint32_t start_addr;
    uint32_t end_addr;
    uint16_t read_mid;
    uint16_t write_mid;
} fapc_config_t;

typedef struct {
    uint8_t allow_mid;
    uint8_t instr;
} fapc_lut0_config_t;

typedef struct {
    uint8_t block_cnt; // block size: 4K
    uint8_t instr;
} fapc_lut1_config_t;

typedef struct {
    uint32_t code_enc_flag;    /* 0x3C7896E1: code area not enc, others: enc */
    uint8_t salt[SALT_LEN];
    uint8_t reserved[RESERVED_LEN];
    uint8_t iv[KEY_IV_LEN];
} fapc_image_info_t;

void tee_fapc_get_image_info(void);

void tee_fapc_init(void);

void tee_flash_online_decryption_config(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* FAPC_H */

