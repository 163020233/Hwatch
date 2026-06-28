/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: data_crypto header
 */

#ifndef DATA_CRYPTO_H
#define DATA_CRYPTO_H

#include <stdbool.h>
#include <errcode.h>

#define DEFAULT_SEC_STORAGE_HEADER_MAGIC 0xEA
#define SEC_STORAGE_HEADER_MAGIC         0xAB
#define SEC_STORAGE_IV_LENGTH            12
#define SEC_STORAGE_HEADER_LENGTH        18
#define SEC_STORAGE_DIE_ID_LENGTH_BYTES  12
#define INVALID_HANDLE                   (0xFFFFFFFF)
#define SEC_STORAGE_KEY_LENGTH           16
#define SALT_LEN                         28
#define SEC_STORAGE_TAG_LENGTH           16
#define SEC_STORAGE_AAD_LENGTH           (SEC_STORAGE_HEADER_LENGTH - SEC_STORAGE_IV_LENGTH)

typedef struct {
    uint8_t magic;
    uint16_t sec_storage_id;
    uint16_t length;
    uint8_t iv[SEC_STORAGE_IV_LENGTH];
} sec_storage_header_t;

typedef struct {
    uint32_t timeLow;
    uint16_t timeMid;
    uint16_t timeHiAndVersion;
    uint8_t clockSeqAndNode[8];
} uuid_t;

errcode_t data_decrypt(uint8_t *src, uint32_t src_len, uint8_t *dest, uint32_t *dest_len);

errcode_t data_encrypt(uint8_t *src, uint32_t src_len, uint8_t *dest, uint32_t *dest_len);

errcode_t data_encrypt_with_flag(uint8_t *src, uint32_t src_len, uint8_t *dest, uint32_t *dest_len, bool is_tee);

errcode_t data_decrypt_with_flag(uint8_t *src, uint32_t src_len, uint8_t *dest, uint32_t *dest_len, bool is_tee);
#endif
