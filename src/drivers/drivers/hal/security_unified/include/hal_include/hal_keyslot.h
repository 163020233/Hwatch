/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: hal keyslot header. \n
 *
 * History: \n
 * 2023-03-22, Create file. \n
 */
#ifndef HAL_KEYSLOT_H
#define HAL_KEYSLOT_H

#include "td_type.h"
#include "crypto_km_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

td_s32 hal_keyslot_lock(td_u32 keyslot_num, crypto_keyslot_type keyslot_type);

td_s32 hal_keyslot_unlock(td_u32 keyslot_num, crypto_keyslot_type keyslot_type);

/*
 * Internal interface. The caller must ensure that the input parameters meet the following constraints:
 * 1) All pointer parameters are valid and cannot be null.
 * 2) keyslot_type support CRYPTO_KEYSLOT_TYPE_MCIPHER only.
 * 3) klad_dest support CRYPTO_KLAD_DEST_MCIPHER only.
 * 4) crypto_klad_engine must be on of CRYPTO_KLAD_ENGINE_[AES|SM4].
 */
int hal_keyslot_clear_key_create(unsigned int *keyslot_chn_num, const unsigned char *key, unsigned int key_len,
    crypto_keyslot_type keyslot_type, crypto_klad_dest klad_dest, crypto_klad_engine klad_engine);

/*
 * Internal interface. The caller must ensure that the input parameters meet the following constraints:
 * 1) keyslot_chn_num must be created by the hal_keyslot_clear_key_create interface.
 * 2) keyslot_type support CRYPTO_KEYSLOT_TYPE_MCIPHER only.
 */
int hal_keyslot_destroy(unsigned int keyslot_chn_num, crypto_keyslot_type keyslot_type);

int hal_keyslot_fapc_send_clear_key(const unsigned char *key, unsigned int key_len, crypto_klad_engine klad_engine);

int hal_keyslot_fapc_send_effective_key(const unsigned char *salt, unsigned int salt_length,
    crypto_klad_engine klad_engine);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif