/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: drvbox call for SELiteOS.
 */

#ifndef DRVBOX_CALL_H
#define DRVBOX_CALL_H

#include "security_hash.h"
#include "cipher.h"
#include "security_symc.h"
#include "trng.h"
#include "unified_cipher_pke.h"
#include "trng.h"

#define LITEOS_DRVBOX_CALL_DEF(...)
#define LITEOS_DRVBOX_FUNC_DEF
#include "drvbox_calls_def.h"

/* HASH API */
#define uapi_drv_cipher_sha256           drvbox_uapi_drv_cipher_sha256
#define uapi_drv_cipher_sha256_start     drvbox_uapi_drv_cipher_sha256_start
#define uapi_drv_cipher_sha256_update    drvbox_uapi_drv_cipher_sha256_update
#define uapi_drv_cipher_sha256_finish    drvbox_uapi_drv_cipher_sha256_finish
#define uapi_drv_cipher_sm3              drvbox_uapi_drv_cipher_sm3
#define uapi_drv_cipher_sm3_start        drvbox_uapi_drv_cipher_sm3_start
#define uapi_drv_cipher_sm3_update       drvbox_uapi_drv_cipher_sm3_update
#define uapi_drv_cipher_sm3_finish       drvbox_uapi_drv_cipher_sm3_finish
#define uapi_drv_cipher_sha_destroy      drvbox_uapi_drv_cipher_sha_destroy

/* SYMC API */
#define uapi_drv_cipher_symc_crypt                 drvbox_uapi_drv_cipher_symc_crypt
#define uapi_drv_cipher_symc_gcm_encrypt           drvbox_uapi_drv_cipher_symc_gcm_encrypt
#define uapi_drv_cipher_symc_gcm_decrypt_verify    drvbox_uapi_drv_cipher_symc_gcm_decrypt_verify

/* TRNG API */
#define uapi_drv_cipher_trng_get_random          drvbox_uapi_drv_cipher_trng_get_random
#define uapi_drv_cipher_trng_get_random_bytes    drvbox_uapi_drv_cipher_trng_get_random_bytes

/* PKE API */
#define uapi_drv_cipher_pke_ecc_gen_key         drvbox_uapi_drv_cipher_pke_ecc_gen_key
#define uapi_drv_cipher_pke_ecdsa_sign          drvbox_uapi_drv_cipher_pke_ecdsa_sign
#define uapi_drv_cipher_pke_ecdsa_verify        drvbox_uapi_drv_cipher_pke_ecdsa_verify
#define uapi_drv_cipher_pke_ecc_gen_ecdh_key    drvbox_uapi_drv_cipher_pke_ecc_gen_ecdh_key
#define uapi_drv_cipher_pke_sm2_dsa_hash        drvbox_uapi_drv_cipher_pke_sm2_dsa_hash
#define uapi_drv_cipher_pke_sm2_public_encrypt  drvbox_uapi_drv_cipher_pke_sm2_public_encrypt
#define uapi_drv_cipher_pke_sm2_private_decrypt drvbox_uapi_drv_cipher_pke_sm2_private_decrypt
#define uapi_drv_cipher_pke_sm2_sign            drvbox_uapi_drv_cipher_pke_sm2_sign
#define uapi_drv_cipher_pke_sm2_verify          drvbox_uapi_drv_cipher_pke_sm2_verify
#define uapi_drv_cipher_pke_rsa_sign            drvbox_uapi_drv_cipher_pke_rsa_sign
#define uapi_drv_cipher_pke_rsa_verify          drvbox_uapi_drv_cipher_pke_rsa_verify
#define uapi_drv_cipher_pke_rsa_public_encrypt  drvbox_uapi_drv_cipher_pke_rsa_public_encrypt
#define uapi_drv_cipher_pke_rsa_private_decrypt drvbox_uapi_drv_cipher_pke_rsa_private_decrypt

#endif /* DRVBOX_CALL_H */
