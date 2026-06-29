/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: psa crypto common code. \n
 *
 * History: \n
 * 2025-03-14, Create file. \n
 */

#include "psa_crypto_common.h"

int32_t psa_crypto_get_value_by_index(const psa_crypto_table_item *table, uint32_t table_size,
    uint32_t index, uint32_t *value)
{
    const psa_crypto_table_item *item = NULL;
    uint32_t i;

    psa_crypto_chk_return(table == NULL, PSA_ERROR_INVALID_ARGUMENT, "table is NULL\n");
    psa_crypto_chk_return(value == NULL, PSA_ERROR_INVALID_ARGUMENT, "value is NULL\n");

    for (i = 0; i < table_size; i++) {
        item = &table[i];
        if (item->index == index) {
            *value = item->value;
            return PSA_INTERNAL_SUCCESS;
        }
    }
    return PSA_ERROR_INVALID_ARGUMENT;
}

int32_t psa_crypto_get_value_by_2index(const psa_crypto_table_item_pro *table, uint32_t table_size,
    uint32_t index1, uint32_t index2, uint32_t *value)
{
    const psa_crypto_table_item_pro *item = NULL;
    uint32_t i;

    psa_crypto_chk_return(table == NULL, PSA_ERROR_INVALID_ARGUMENT, "table is NULL\n");
    psa_crypto_chk_return(value == NULL, PSA_ERROR_INVALID_ARGUMENT, "value is NULL\n");

    for (i = 0; i < table_size; i++) {
        item = &table[i];
        if (item->index1 == index1 && item->index2 == index2) {
            *value = item->value;
            return PSA_INTERNAL_SUCCESS;
        }
    }
    return PSA_ERROR_INVALID_ARGUMENT;
}

int32_t psa_crypto_get_ptr_by_2index(const psa_crypto_table_item_ptr *table, uint32_t table_size,
    uint32_t index1, uint32_t index2, uint8_t **value)
{
    const psa_crypto_table_item_ptr *item = NULL;
    uint32_t i;

    psa_crypto_chk_return(table == NULL, PSA_ERROR_INVALID_ARGUMENT, "table is NULL\n");
    psa_crypto_chk_return(value == NULL, PSA_ERROR_INVALID_ARGUMENT, "value is NULL\n");

    for (i = 0; i < table_size; i++) {
        item = &table[i];
        if (item->index1 == index1 && item->index2 == index2) {
            *value = item->ptr;
            return PSA_INTERNAL_SUCCESS;
        }
    }
    return PSA_ERROR_INVALID_ARGUMENT;
}

int32_t psa_crypto_check_match_by_2index(const psa_crypto_table_item *table, uint32_t table_size,
    uint32_t index1, uint32_t index2)
{
    const psa_crypto_table_item *item = NULL;
    uint32_t i;

    psa_crypto_chk_return(table == NULL, PSA_ERROR_INVALID_ARGUMENT, "table is NULL\n");

    for (i = 0; i < table_size; i++) {
        item = &table[i];
        if (item->index == index1 && item->value == index2) {
            return PSA_INTERNAL_SUCCESS;
        }
    }
    return PSA_ERROR_INVALID_ARGUMENT;
}

int32_t psa_crypto_check_match_by_3index(const psa_crypto_table_item_pro *table, uint32_t table_size,
    uint32_t index1, uint32_t index2, uint32_t index3)
{
    const psa_crypto_table_item_pro *item = NULL;
    uint32_t i;

    psa_crypto_chk_return(table == NULL, PSA_ERROR_INVALID_ARGUMENT, "table is NULL\n");

    for (i = 0; i < table_size; i++) {
        item = &table[i];
        if (item->index1 == index1 && item->index2 == index2 && item->value == index3) {
            return PSA_INTERNAL_SUCCESS;
        }
    }
    return PSA_ERROR_INVALID_ARGUMENT;
}

#define BYTES_IN_ONE_LINE 16
#define MAX_DUMP_LENGHT 512
void psa_crypto_dump_data(const char *name, const uint8_t *data, uint32_t data_len)
{
    uint32_t i;
    uint32_t dump_len = data_len;
    psa_unused(name);
    psa_unused(data);
    if (data == NULL) {
        psa_crypto_print("data is NULL\n");
        return;
    }

    psa_crypto_print("%s:\n", name);
    dump_len = data_len > MAX_DUMP_LENGHT ? MAX_DUMP_LENGHT : data_len;
    for (i = 0; i < dump_len; i++) {
        psa_crypto_print("%02x ", data[i]);
        if ((i + 1) % BYTES_IN_ONE_LINE == 0) {
            psa_crypto_print("\n");
        }
    }
    if (dump_len % BYTES_IN_ONE_LINE != 0) {
        psa_crypto_print("\n");
    }
}
