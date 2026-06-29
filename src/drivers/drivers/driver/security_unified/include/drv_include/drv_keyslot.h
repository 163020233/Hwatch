/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: cipher driver keyslot header. \n
 *
 * History: \n
 * 2023-03-22, Create file. \n
 */
#ifndef DRV_KEYSLOT_H
#define DRV_KEYSLOT_H

#include <stdint.h>
#include "crypto_km_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

int32_t drv_keyslot_setup(uint32_t *keyslot_handle, crypto_keyslot_type keyslot_type);

int32_t drv_keyslot_teardown(uint32_t keyslot_handle);

int32_t drv_keyslot_set_clear_key(uint32_t keyslot_handle, crypto_klad_engine klad_engine,
    const uint8_t *key, uint32_t key_len);

int32_t drv_keyslot_set_hard_key(uint32_t keyslot_handle,
    crypto_keyslot_hard_key_type hard_key_type, crypto_klad_engine klad_engine,
    const uint8_t *salt, uint32_t salt_len, uint32_t key_len);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif