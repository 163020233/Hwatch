/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2026. All rights reserved.
 *
 * Description: ipc crypto header.
 *
 * History:
 * 2026-05-08, Create file.
 */

#ifndef IPC_CRYPTO_H
#define IPC_CRYPTO_H

#include "ipc.h"
#include "ipc_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define KE_P256_KEY_LEN 32
#define AES_CMAC_DATA_MAX_LEN 144
#define AES_CMAC_KEY_LEN 16
#define AES_CMAC_OUTPUT_LEN 16
#define AES_128_KEY_LEN 16
#define AES_128_DATA_LEN 16
#define AES_128_IV_LEN 16
#define MULTI_RAND_MAX_LEN 32

enum {
    ECC_GEN_KEY = 0x00,
    ECC_GEN_ECDH_KEY,
    CHECK_DOT_ON_CURVE,
    GET_SEC_RAND,
};

typedef struct {
    uint8_t val[KE_P256_KEY_LEN];
} key_exchange_private_key_t;

typedef struct {
    uint8_t x[KE_P256_KEY_LEN];
    uint8_t y[KE_P256_KEY_LEN];
} key_exchange_public_key_t;

typedef struct {
    uint8_t val[KE_P256_KEY_LEN];
} key_exchange_dh_key_t;

typedef struct {
    uint8_t  ecdh_type;
    uint8_t  curve_length;
    bool     on_curve;
    uint8_t  reserved;    // for 4-byte aligned
    uint32_t curve_type;
    uint32_t rand;
    key_exchange_private_key_t  priv_key;
    key_exchange_public_key_t   public_key;
    key_exchange_dh_key_t       dh_key;
    int32_t return_value;
} ipc_ecdh_payload_t;

typedef struct {
    uint32_t alg;
    uint8_t  src[AES_128_DATA_LEN];
    uint8_t  dst[AES_128_DATA_LEN];
    uint32_t data_len;
    uint8_t  iv[AES_128_IV_LEN];
    uint8_t  key[AES_128_KEY_LEN];
    uint32_t key_len;
    bool     is_encrypt;
    uint8_t  reserved[3];    // 3-byte padding for 4-byte aligned
    int32_t  return_value;
} ipc_aes_payload_t;

typedef struct {
    uint8_t  data[AES_CMAC_DATA_MAX_LEN];
    uint32_t data_len;
    uint8_t  key[AES_CMAC_KEY_LEN];
    uint32_t key_len;
    uint8_t  out_mac[AES_CMAC_OUTPUT_LEN];
    int32_t  return_value;
} ipc_aes_cmac_payload_t;

typedef struct {
    uint8_t rand[MULTI_RAND_MAX_LEN];
    uint32_t rand_len;
    int32_t return_value;
} ipc_multi_rand_payload_t;

void ipc_crypto_init(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif // IPC_ECDH_CLAC_H
