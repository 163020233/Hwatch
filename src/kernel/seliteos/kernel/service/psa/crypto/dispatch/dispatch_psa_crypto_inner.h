/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: psa crypto dispatch inner header. \n
 *
 * History: \n
 * 2025-03-14, Create file. \n
 */

#ifndef DISPATCH_PSA_CRYPTO_INNER
#define DISPATCH_PSA_CRYPTO_INNER

#include "psa/psa_crypto.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    bool is_lock;
    psa_key_attributes_t attr;
    bool is_set_iv;
    uint8_t iv[16];
    struct key_data {
        uint8_t *data;
        size_t bytes;
    } key;
} psa_key_slot_t;

psa_status_t psa_lock_key_slot(psa_key_id_t *volatile_key_id, psa_key_slot_t **pp_slot);

void psa_unlock_key_slot(psa_key_slot_t *p_slot);

psa_status_t psa_get_key_slot(psa_key_id_t key, psa_key_slot_t **pp_slot);

psa_status_t psa_alloc_buffer_to_slot(psa_key_slot_t *p_slot, size_t data_length);

psa_status_t psa_get_and_lock_key_slot_with_policy(psa_key_id_t key, psa_key_slot_t **pp_slot,
    psa_key_usage_t usage_to_check, psa_algorithm_t alg_to_check);

psa_status_t tee_psa_import_key(psa_key_attributes_t *attributes, const uint8_t *data, size_t data_length,
    psa_key_id_t *key);

psa_status_t psa_key_usage_algorithm_type_permits(psa_key_usage_t usage, psa_algorithm_t alg, psa_key_type_t type);

psa_status_t psa_validate_key_attributes(const psa_key_attributes_t *attributes);

#ifdef __cplusplus
}
#endif

#endif