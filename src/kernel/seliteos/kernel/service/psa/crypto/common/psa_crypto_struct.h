/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: psa crypto common struct. \n
 *
 * History: \n
 * 2025-03-14, Create file. \n
 */

#ifndef PSA_CRYPTO_STRUCT_H
#define PSA_CRYPTO_STRUCT_H

#include "psa/psa_crypto.h"

typedef struct {
    uint32_t index;
    uint32_t value;
} psa_crypto_table_item;

typedef struct {
    uint32_t index1;
    uint32_t index2;
    uint32_t value;
} psa_crypto_table_item_pro;

typedef struct {
    uint32_t index1;
    uint32_t index2;
    uint8_t *ptr;
} psa_crypto_table_item_ptr;

#endif