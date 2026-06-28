/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: hal pke romable header. \n
 *
 * History: \n
 * 2025-03-13, Create file. \n
 */
#ifndef HAL_PKE_ROMABLE_H
#define HAL_PKE_ROMABLE_H

#include <stdint.h>

#include "hal_pke_ram_addr.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

int32_t hal_pke_lock_romable(void);

void hal_pke_unlock_romable(void);

int32_t hal_pke_lock_wait_done_romable(void);

void hal_pke_set_mont_para_romable(const uint32_t mont_param[4]);

void hal_pke_set_work_len_romable(uint32_t work_len);

void hal_pke_set_ram_romable(uint32_t ram_section, const uint8_t *data, uint32_t data_len);

void hal_pke_set_ram_word_all_romable(uint32_t ram_section, uint32_t data_len, uint32_t word);

void hal_pke_set_ram_word_romable(uint32_t ram_section, uint32_t data_len, int32_t word_pos, uint32_t word);

void hal_pke_get_ram_romable(uint32_t ram_section, uint8_t *data, uint32_t data_len);

int32_t hal_pke_batch_compute_romable(uint32_t instr_addr, uint32_t instr_num);

int32_t hal_pke_single_compute_romable(uint32_t single_instr);

/* key1 can be NULL, which means key1 is const_0 */
int32_t hal_pke_loop_batch_compute_romable(uint32_t loop_num,
    const unsigned char *key0, const unsigned char *key1,
    unsigned int klen, const rom_lib *batch_instr[4]);

int32_t hal_pke_wait_done_romable(void);

void hal_pke_debug_romable(void);

void hal_pke_batch_trace_romable(uint32_t instr_addr, uint32_t instr_num);

typedef int32_t (*pke_wait_done_func)(void);
typedef void (*pke_debug_func)(void);
typedef void (*pke_batch_trace_func)(uint32_t instr_addr, uint32_t instr_num);
typedef int32_t (*pke_lock_func)(void);
typedef void (*pke_unlock_func)(void);

typedef struct {
    pke_wait_done_func      lock_wait_done_func;
    pke_wait_done_func      wait_done_func;
    pke_debug_func          debug_func;
    pke_batch_trace_func    batch_trace_func;
    pke_lock_func           lock;
    pke_unlock_func         unlock;
} hal_pke_func_table_t;

void hal_pke_register_romable(const hal_pke_func_table_t *func_table);

void hal_pke_debug_default(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
