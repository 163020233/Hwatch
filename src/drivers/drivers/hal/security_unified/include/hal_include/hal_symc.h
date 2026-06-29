/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: hal symc header. \n
 *
 * History: \n
 * 2023-03-22, Create file. \n
 */
#ifndef HAL_SYMC_H
#define HAL_SYMC_H

#include "crypto_symc_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/*
 * Internal interface. The caller must ensure that the input parameters meet the following constraints:
 * 1) All pointer parameters are valid and cannot be null.
 * 2) symc_alg must be one of CRYPTO_SYMC_ALG_[AES|SM4].
 * 3) work_mode must be one of CRYPTO_SYMC_WORK_MODE_[ECB|CBC|CTR|OFB|CFB].
 * 4) symc_bit_width must be one of CRYPTO_SYMC_BIT_WIDTH_[1BIT|8BIT|128BIT], only works in CFB mode.
 * 5) data_phys_addr must be 4-byte aligned(only ws53/ws63 have this restriction.).
 * 6) data_len must be 16-byte aligned for ECB/CBC/OFB/CFB128 mode.
 *    In other cases, there is no requirement for length alignment.
 * 7) key_len only support 16/24/32 for aes, 16 for sm4.
 * 8) keyslot_chn_num must be created by the hal_keyslot_clear_key_create interface.
 * 9) is_decrypt, 0 means Encrypt, other means Decrypt.
 */
int hal_cipher_symc_crypt(crypto_symc_alg symc_alg, crypto_symc_work_mode work_mode,
    crypto_symc_bit_width symc_bit_width, unsigned long long src_phys_addr, unsigned long long dst_phys_addr,
    unsigned int data_len, unsigned char iv[16], const unsigned char *key, unsigned int key_len,
    unsigned int keyslot_chn_num, td_bool is_decrypt);

/*
 * Internal interface. The caller must ensure that the input parameters meet the following constraints:
 * 1) All pointer parameters are valid and cannot be null.
 * 2) symc_alg support AES/SM4.
 * 3) data_phys_addr must be 4-byte aligned(only ws53/ws63 have this restriction.).
 * 4) data_len must be 16-byte aligned.
 * 5) key_len only support 16/24/32.
 * 6) keyslot_chn_num must be created by the hal_keyslot_clear_key_create interface.
 */
int hal_cipher_symc_gcm_ghash(crypto_symc_alg symc_alg, unsigned long long data_phys_addr, unsigned int data_len,
    unsigned char ghash[16], const unsigned char *key, unsigned int key_len, unsigned int keyslot_chn_num);

/*
 * Internal interface. The caller must ensure that the input parameters meet the following constraints:
 * 1) All pointer parameters are valid and cannot be null.
 * 2) symc_alg must be one of CRYPTO_SYMC_ALG_[AES|SM4].
 * 3) data_phys_addr must be 4-byte aligned(only ws53/ws63 have this restriction.).
 * 4) data_len must be 16-byte aligned.
 * 5) key_len only support 16/24/32.
 * 6) keyslot_chn_num must be created by the hal_keyslot_clear_key_create interface.
 */
int hal_cipher_symc_cbc_mac(crypto_symc_alg symc_alg, unsigned long long data_phys_addr, unsigned int data_len,
    unsigned char mac[16], const unsigned char *key, unsigned int key_len, unsigned int keyslot_chn_num);

/*
 * Internal interface. The caller must ensure that the input parameters meet the following constraints:
 * 1) All pointer parameters are valid and cannot be null.
 * 2) symc_alg only support CRYPTO_SYMC_ALG_AES.
 * 3) data_phys_addr must be 4-byte aligned(only ws53/ws63 have this restriction.).
 * 4) key_len only support 16/24/32.
 * 5) keyslot_chn_num must be created by the hal_keyslot_clear_key_create interface.
 */
int hal_cipher_symc_cmac(crypto_symc_alg symc_alg,
    unsigned long long data_phys_addr, unsigned int data_len,
    unsigned char mac[16], const unsigned char *key, unsigned int key_len, unsigned int keyslot_chn_num);

void hal_spacc_interrupt_enable(bool enable);

void hal_symc_interrupt_enable(uint32_t chn_num, bool enable);

bool hal_symc_done_try_special(uint32_t chn_num, bool is_noout);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
