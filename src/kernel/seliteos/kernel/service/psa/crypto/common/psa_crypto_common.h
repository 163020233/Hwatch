/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: psa crypto common header. \n
 *
 * History: \n
 * 2025-03-14, Create file. \n
 */

#ifndef PSA_CRYPTO_COMMON_H
#define PSA_CRYPTO_COMMON_H

#include <stdint.h>
#include <stddef.h>
#include "securec.h"

#include "psa_crypto_macro.h"
#include "psa_crypto_struct.h"
#include "psa_crypto_value.h"
#include "psa_common.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t psa_crypto_get_value_by_index(const psa_crypto_table_item *table, uint32_t table_size, uint32_t index,
    uint32_t *value);
int32_t psa_crypto_get_value_by_2index(const psa_crypto_table_item_pro *table, uint32_t table_size,
    uint32_t index1, uint32_t index2, uint32_t *value);

int32_t psa_crypto_check_match_by_2index(const psa_crypto_table_item *table, uint32_t table_size,
    uint32_t index1, uint32_t index2);
int32_t psa_crypto_check_match_by_3index(const psa_crypto_table_item_pro *table, uint32_t table_size,
    uint32_t index1, uint32_t index2, uint32_t index3);

int32_t psa_crypto_get_ptr_by_2index(const psa_crypto_table_item_ptr *table, uint32_t table_size,
    uint32_t index1, uint32_t index2, uint8_t **value);

void psa_crypto_dump_data(const char *name, const uint8_t *data, uint32_t data_len);

#ifdef __cplusplus
}
#endif

#endif