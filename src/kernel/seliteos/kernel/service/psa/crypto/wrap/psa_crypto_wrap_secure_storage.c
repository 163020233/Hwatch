/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: psa crypto wrap secure storage code. \n
 *
 * History: \n
 * 2025-03-14, Create file. \n
 */

#include "psa_crypto_wrap.h"

#include "psa/psa_crypto.h"
#include "psa_crypto_common.h"
#include "sec_storage.h"

#define PSA_KEY_STORAGE_MAGIC_HEADER "PSA\0KEY"
#define PSA_KEY_STORAGE_MAGIC_HEADER_LENGTH (sizeof(PSA_KEY_STORAGE_MAGIC_HEADER))

typedef struct {
    uint8_t magic[PSA_KEY_STORAGE_MAGIC_HEADER_LENGTH];
    psa_key_lifetime_t lifetime;
    psa_key_type_t type;
    psa_key_bits_t bits;
    psa_key_policy_t policy;
    psa_key_id_t key_id;
    uint32_t data_len;
    uint8_t key_data[0];
} psa_persistent_key_storage_format;

static psa_status_t psa_format_key_data_for_storage(const psa_key_attributes_t *attr, const uint8_t *data,
    size_t data_length, psa_persistent_key_storage_format *format_key)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;

    ret = memcpy_sec(format_key->magic, PSA_KEY_STORAGE_MAGIC_HEADER_LENGTH, PSA_KEY_STORAGE_MAGIC_HEADER,
        PSA_KEY_STORAGE_MAGIC_HEADER_LENGTH);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, PSA_ERR_MEMCPY_S, "memcpy_sec error!\n");

    format_key->data_len = data_length;

    ret = memcpy_sec(format_key->key_data, format_key->data_len, data, data_length);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, PSA_ERR_MEMCPY_S, "memcpy_sec error!\n");

    format_key->lifetime = attr->lifetime;
    format_key->type = attr->type;
    format_key->bits = attr->bits;
    format_key->policy = attr->policy;
    format_key->key_id = attr->key;
    return ret;
}

psa_status_t psa_save_persistent_key(const psa_key_attributes_t *attr, const uint8_t *data, size_t data_length)
{
    volatile_sec size_t format_size;
    psa_persistent_key_storage_format *format_key;
    psa_status_t ret = PSA_INTERNAL_FAILURE;

    format_size = sizeof(psa_persistent_key_storage_format) + data_length;
    format_key = psa_crypto_malloc(format_size);
    psa_crypto_chk_return(format_key == NULL, PSA_ERROR_INSUFFICIENT_MEMORY, "psa_crypto_malloc error!\n");

    ret = psa_format_key_data_for_storage(attr, data, data_length, format_key);
    psa_crypto_chk_goto_with_ret(ret, ret != PSA_INTERNAL_SUCCESS, free, ret, "psa_format_key_data_for_storage error!\n");

    ret = uapi_sec_storage_write((uint16_t)attr->key, (const uint8_t *)format_key, format_size);
    psa_crypto_chk_goto_with_ret(ret, psa_get_wrap_errcode(ret) != PSA_INTERNAL_SUCCESS, free, PSA_ERROR_STORAGE_FAILURE,
        "uapi_sec_storage_write error!\n");

free:
    memset_sec(format_key, format_size, 0, format_size);
    psa_crypto_free(format_key);
    return ret;
}

static psa_status_t psa_parse_key_data_from_storage(const psa_persistent_key_storage_format *format_key,
    uint16_t format_size, psa_key_attributes_t *attr, uint8_t *key_data, size_t data_length)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;

    psa_crypto_param_chk(format_size != (uint16_t)(format_key->data_len + sizeof(psa_persistent_key_storage_format)));

    attr->lifetime = format_key->lifetime;
    attr->type = format_key->type;
    attr->bits = format_key->bits;
    attr->policy = format_key->policy;
    attr->key = format_key->key_id;

    ret = psa_validate_key_attributes(attr);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_validate_key_attributes error!\n");

    /* check magic header */
    ret = memcmp_sec(format_key->magic, PSA_KEY_STORAGE_MAGIC_HEADER, PSA_KEY_STORAGE_MAGIC_HEADER_LENGTH);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, PSA_ERR_MEMCPY_S, "memcmp_sec error!\n");

    ret = memcpy_sec(key_data, data_length, format_key->key_data, format_key->data_len);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, PSA_ERR_MEMCPY_S, "memcpy_sec error!\n");

    return ret;
}

psa_status_t psa_recovery_persistent_key(psa_key_id_t key, psa_key_slot_t **pp_slot)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;
    uint16_t format_size;
    uint16_t nv_back_size;
    psa_persistent_key_storage_format *format_key;
    psa_key_id_t volatile_key_id;      /* unused */
    psa_key_slot_t *p_slot;

    /* get format key size */
    ret = uapi_sec_storage_get_key_length(key, &format_size);
    psa_crypto_chk_return(psa_get_wrap_errcode(ret) != PSA_INTERNAL_SUCCESS, PSA_ERROR_INVALID_HANDLE,
        "uapi_sec_storage_get_key_length error!\n");

    /* check format_size is valid */
    if (format_size > (PSA_MAX_KEY_DATA_LENGTH + sizeof(psa_persistent_key_storage_format))) {
        psa_crypto_print("nv read size is invalid!\n");
        return PSA_ERROR_INVALID_HANDLE;
    }

    format_key = psa_crypto_malloc(format_size);
    psa_crypto_chk_return(format_key == NULL, PSA_ERROR_INVALID_HANDLE, "psa_crypto_malloc error!\n");

    /* load persistent key */
    ret = uapi_sec_storage_read(key, format_size, &nv_back_size, (uint8_t *)format_key);
    psa_crypto_chk_goto_with_ret(ret, psa_get_wrap_errcode(ret) != PSA_INTERNAL_SUCCESS, free_format_key,
        PSA_ERROR_INVALID_HANDLE, "uapi_sec_storage_read error!\n");

    /* double check */
    if ((format_size != (nv_back_size + (sizeof(sec_storage_header_t) + SEC_STORAGE_TAG_LENGTH))) ||
        (format_key->data_len == 0) || (format_key->data_len > PSA_MAX_KEY_DATA_LENGTH)) {
        psa_crypto_print("nv read size error!\n");
        ret = PSA_ERROR_INVALID_HANDLE;
        goto free_format_key;
    }

    /* find free keyslot */
    ret = psa_lock_key_slot(&volatile_key_id, &p_slot);
    psa_crypto_chk_goto_with_ret(ret, ret != PSA_INTERNAL_SUCCESS, free_format_key, ret, "psa_lock_key_slot error!\n");

    /* malloc buffer for key */
    ret = psa_alloc_buffer_to_slot(p_slot, format_key->data_len);
    psa_crypto_chk_goto_with_ret(ret, ret != PSA_INTERNAL_SUCCESS, psa_unlock_key_slot, ret,
        "psa_alloc_buffer_to_slot error!\n");

    /* save attr and key to slot */
    ret = psa_parse_key_data_from_storage(format_key, nv_back_size, &p_slot->attr, p_slot->key.data, p_slot->key.bytes);
    psa_crypto_chk_goto_with_ret(ret, ret != PSA_INTERNAL_SUCCESS, psa_free_buffer_to_slot, ret,
        "psa_parse_key_data_from_storage error!\n");

    *pp_slot = p_slot;
    goto free_format_key;

psa_free_buffer_to_slot:
    memset_sec(p_slot->key.data, p_slot->key.bytes, 0, p_slot->key.bytes);
    psa_crypto_free(p_slot->key.data);
psa_unlock_key_slot:
    psa_unlock_key_slot(p_slot);
free_format_key:
    memset_sec(format_key, format_size, 0, format_size);
    psa_crypto_free(format_key);
    return ret;
}

psa_status_t psa_destroy_persistent_key(psa_key_id_t key)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;

    ret = uapi_sec_storage_delete(key);
    return psa_get_wrap_errcode(ret);
}