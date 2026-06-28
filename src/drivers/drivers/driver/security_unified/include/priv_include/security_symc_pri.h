/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: Provides symc driver pri header \n

 */
#ifndef SECURITY_SYMC_PRI_H
#define SECURITY_SYMC_PRI_H

#include "security_symc.h"
#include "security_mac.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

errcode_t uapi_drv_cipher_symc_set_key(cipher_handle_t ctx, const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle);
errcode_t uapi_drv_cipher_symc_set_alg(cipher_handle_t ctx, uint32_t alg);
errcode_t uapi_drv_cipher_symc_set_crypt_mode(cipher_handle_t ctx, bool is_encrypt);
errcode_t uapi_drv_cipher_cmac_ctx_init(mac_handle_t ctx, uint32_t ctx_size,
    uapi_drv_cipher_symc_alg_t alg, const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle);
errcode_t uapi_drv_cipher_cmac_ctx_deinit(mac_handle_t ctx, uint32_t ctx_size);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif