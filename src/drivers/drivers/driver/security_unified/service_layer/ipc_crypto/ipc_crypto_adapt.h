/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2026. All rights reserved.
 *
 * Description: ipc crypto adapt header.
 *
 * History:
 * 2026-05-08, Create file.
 */

#ifndef IPC_CRYPTO_ADAPT_H
#define IPC_CRYPTO_ADAPT_H

#include "ipc_crypto.h"
#include "soc_osal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define IPC_CRYPTO_BYTE_IN_BITS  8
#define IPC_CRYPTO_WORD_IN_BYTES 4

#ifdef CONFIG_NON_TEE_SUPPORT
int32_t ipc_uapi_ecc_adapt(ipc_ecdh_payload_t *param);
int32_t ipc_uapi_aes_adapt(ipc_aes_payload_t *param);
int32_t ipc_uapi_aes_cmac_adapt(ipc_aes_cmac_payload_t *param);
int32_t ipc_uapi_multi_rand_adapt(ipc_multi_rand_payload_t *param);
#else
#define psa_chk_return(cond, ret, ...) \
    do {                               \
        if (cond) {                    \
            osal_printk(__VA_ARGS__);        \
            return ret;                \
        }                              \
    } while (0)

#define psa_chk_goto(cond, label, ...) \
    do {                               \
        if (cond) {                    \
            osal_printk(__VA_ARGS__);        \
            goto label;                \
        }                              \
    } while (0)

#define psa_chk_goto_with_ret(cond, label, err_ret, ...) \
    do {                               \
        if (cond) {                    \
            osal_printk(__VA_ARGS__);        \
            ret = err_ret;                 \
            goto label;                \
        }                              \
    } while (0)

#define PSA_VOLATILE_KEY \
    PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(PSA_KEY_PERSISTENCE_VOLATILE, PSA_KEY_LOCATION_LOCAL_STORAGE)
#define PSA_PERSISTENT_KEY \
    PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(PSA_KEY_PERSISTENCE_DEFAULT, PSA_KEY_LOCATION_LOCAL_STORAGE)

int32_t ipc_psa_ecc_adapt(ipc_ecdh_payload_t *param);
int32_t ipc_psa_aes_adapt(ipc_aes_payload_t *param);
int32_t ipc_psa_aes_cmac_adapt(ipc_aes_cmac_payload_t *param);
int32_t ipc_psa_multi_rand_adapt(ipc_multi_rand_payload_t *param);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* IPC_CRYPTO_ADAPT_H */