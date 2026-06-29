/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: hal hash header. \n
 *
 * History: \n
 * 2023-03-22, Create file. \n
 */
#ifndef HAL_HASH_H
#define HAL_HASH_H

#include "crypto_hash_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

td_s32 hal_cipher_hash_init(td_void);

td_s32 hal_cipher_hash_deinit(td_void);

td_s32 hal_hash_lock(td_u32 chn_num);

td_s32 hal_hash_unlock(td_u32 chn_num);

td_s32 hal_cipher_hash_config(td_u32 chn_num, crypto_hash_type hash_type, const td_u32 *state);

td_s32 hal_cipher_hash_attach(td_u32 chn_num, td_u32 keyslot_chn_num);

td_s32 hal_cipher_hash_add_in_node(td_u32 chn_num, td_phys_addr_t data_phys, td_u32 data_len,
    in_node_type_e in_node_type);

td_s32 hal_cipher_hash_start(td_u32 chn_num, td_bool is_wait);

td_s32 hal_cipher_hash_start_calc(td_u32 chn_num, td_u8 *data, td_u32 length);

td_s32 hal_cipher_hash_wait_done(td_u32 chn_num, td_u32 *state, td_u32 state_size);

td_s32 hal_cipher_hash_done_notify(td_u32 chn_num);

td_u32 hal_cipher_hash_done_try(td_u32 chn_num);

td_void hal_hash_debug(td_void);

td_void hal_hash_debug_chn(td_u32 chn_num);

td_s32 hal_cipher_hash_wait_func_config(td_u32 chn_num, td_bool is_enable);

/*
 * Internal interface. The caller must ensure that the input parameters meet the following constraints:
 * 1) hash_type must be one of CRYPTO_HASH_TYPE_[SHA1|SHA224|SHA256|SHA384|SHA512|SM3].
 * 2) data_phys_addr must be 4-byte aligned(only ws53/ws63 have this restriction.).
 * 3) data_len must be block-aligned.
 * 4) state and state_len is valid and state is not null.
 *
 */
int hal_cipher_hash_compute(crypto_hash_type hash_type,
    unsigned long long data_phys_addr, unsigned int data_len, unsigned int *state, unsigned int state_len);

int32_t hal_cipher_hmac_compute(crypto_hash_type hash_type, uint64_t data_phys_addr, uint32_t data_len,
    uint32_t keyslot_chn, uint32_t *state, uint32_t state_len);

bool hal_hash_done_try_special(uint32_t chn_num);

void hal_hash_interrupt_enable(uint32_t chn_num, bool enable);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif