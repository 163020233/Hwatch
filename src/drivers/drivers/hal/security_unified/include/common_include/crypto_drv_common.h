/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides cipher driver common utils. \n
 *
 * History: \n
 * 2023-03-22, Create file. \n
 */
#ifndef CRYPTO_DRV_COMMON_H
#define CRYPTO_DRV_COMMON_H

#include "crypto_type.h"
#include "crypto_osal_lib.h"
#include "crypto_common_struct.h"
#include "crypto_pke_struct.h"
#include "crypto_hash_struct.h"
#include "crypto_symc_struct.h"
#include "crypto_km_struct.h"

#include "security_unified_porting.h"

#include "crypto_reg_utils.h"
#include "crypto_log_utils.h"
#include "hal_common.h"
#include "securec.h"

/* align check */
#ifndef crypto_addr_align_check
#define crypto_addr_align_check(addr)       ((((addr) % 4) != 0) ? TD_FALSE : TD_TRUE)
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/*
 * crypto service preprocess and postprocess.
 * redefine in crypto_osal_lib.h if need.
 */
#ifndef crypto_service_preprocess
#define crypto_service_preprocess(...)
#endif

#ifndef crypto_service_postprocess
#define crypto_service_postprocess(...)
#endif

/*
 * crypto atomic.
 * redefine in crypto_osal_lib.h if need.
 */
#define crypto_common_pri_atomic                int32_t
int32_t crypto_common_pri_atomic_read(crypto_common_pri_atomic *atomic);
void crypto_common_pri_atomic_inc(crypto_common_pri_atomic *atomic);
void crypto_common_pri_atomic_dec(crypto_common_pri_atomic *atomic);
int32_t crypto_common_pri_atomic_inc_return(crypto_common_pri_atomic *atomic);
int32_t crypto_common_pri_atomic_dec_return(crypto_common_pri_atomic *atomic);

#ifndef crypto_atomic
#define crypto_atomic                           crypto_common_pri_atomic
#define crypto_atomic_read                      crypto_common_pri_atomic_read
#define crypto_atomic_inc                       crypto_common_pri_atomic_inc
#define crypto_atomic_dec                       crypto_common_pri_atomic_dec
#define crypto_atomic_inc_return                crypto_common_pri_atomic_inc_return
#define crypto_atomic_dec_return                crypto_common_pri_atomic_dec_return
#endif

/*
 * crypto mutex.
 * redefine in crypto_osal_lib.h if need.
 */
#ifndef crypto_mutex
#define crypto_mutex        uint32_t
#define crypto_mutex_init(...)
#define crypto_mutex_destroy(...)
#define crypto_mutex_lock(...)
#define crypto_mutex_unlock(...)
#endif

/*
 * crypto get_phys_addr.
 * redefine in crypto_osal_lib.h if need.
 */
#ifndef crypto_get_phys_addr
#define crypto_get_phys_addr(ptr)       (uintptr_t)(ptr)
#endif

/**
 * crypto resume&suspend.
 */
void crypto_drv_resume(unsigned int module_id);
void crypto_drv_suspend(unsigned int module_id);

typedef void *(*malloc_func_t)(uint32_t size);
typedef void (*free_func_t)(void *ptr);
typedef crypto_cpu_type (*get_cpu_type_func_t)(void);
typedef void (*cache_flush_func_t)(uintptr_t base_addr, uint32_t size);
typedef void (*cache_inv_func_t)(uintptr_t base_addr, uint32_t size);
typedef void (*udelay_func_t)(uint32_t useconds);

void crypto_register_mem_func(malloc_func_t malloc_func, free_func_t free_func);
 
void crypto_register_cache_flush_func(cache_flush_func_t func);

void crypto_register_cache_inv_func(cache_inv_func_t func);

void crypto_register_get_cpu_type_func(get_cpu_type_func_t func);

void crypto_register_udelay_func(udelay_func_t func);

void *crypto_malloc_coherent(uint32_t size);

void *crypto_malloc_mmz(uint32_t size);

void crypto_free_coherent(void *ptr);

void crypto_udelay(uint32_t useconds);

crypto_cpu_type crypto_get_cpu_type(void);

void crypto_cache_flush(uintptr_t base_addr, uint32_t size);

void crypto_cache_inv(uintptr_t base_addr, uint32_t size);

void crypto_cache_all(void);

td_bool crypto_sec_vote_enable(void);

typedef struct {
    uint32_t index;
    uint32_t value;
} crypto_table_item;

td_s32 crypto_get_value_by_index(const crypto_table_item *table, uint32_t table_size,
    uint32_t index, uint32_t *value);
td_s32 crypto_get_index_by_value(const crypto_table_item *table, uint32_t table_size,
    uint32_t value, uint32_t *index);

/* wait function mode control. */
void crypto_wait_func_config(td_bool is_enable);

td_bool crypto_wait_func_is_support(void);

void crypto_common_lock(void);

void crypto_common_unlock(void);

void crypto_hash_lock(void);

void crypto_hash_unlock(void);

/* Check the Validation of crypto data. */
bool crypto_data_check(const uint64_t phys_addr, uint32_t length);

/**
 * @brief open module clock.
 *
 * @param module_id module id value.
 */
void drv_cipher_open_clk(unsigned int module_id);

/**
 * @brief close module clock.
 *
 * @param module_id module id value.
 */
void drv_cipher_close_clk(unsigned int module_id);

void drv_cipher_close_all_clk(void);

int32_t crypto_km_common_get_hard_key_type_item(crypto_keyslot_hard_key_type hard_key_type,
    uint32_t *rkp_cmd_cfg_val, uint32_t *kl_com_ctrl_val);

int memcmp_se(const void *cs, const void *ct, unsigned int count);

int memset_se(void *dest, unsigned int dest_max, unsigned char c, unsigned int count);

int memcpy_se(void *dest, unsigned int dest_max, const void *src, unsigned int count);

void random_delay(void);

/* reverse bits in one byte. */
void reverse_bits_in_byte(uint8_t input_data, uint8_t *out_data);

/* reverse whole bits in stream byte. */
int reverse_stream_bits(uint8_t *data, uint32_t data_len);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif