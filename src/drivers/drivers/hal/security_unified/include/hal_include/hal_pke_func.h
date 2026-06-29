/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: hal pke not romable header. \n
 *
 * History: \n
 * 2025-03-13, Create file. \n
 */
#ifndef HAL_PKE_NEW_H
#define HAL_PKE_NEW_H

#include <stdint.h>

#include "hal_pke_ram_addr.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

void hal_pke_set_ram_not_aligned(uint32_t ram_section, const uint8_t *data, uint32_t data_len, uint32_t work_len);

int32_t hal_pke_wait_done_sec(void);

int32_t hal_pke_batch_compute_sec(uint32_t instr_addr, uint32_t instr_num);

int32_t hal_pke_loop_batch_compute_sec(uint32_t loop_num, const unsigned char *key0, const unsigned char *key1,
    unsigned int klen, const rom_lib *batch_instr[4]);

void hal_pke_enable_noise(void);

void hal_pke_disable_noise(void);

int32_t hal_pke_set_dram_mask(void);

void hal_pke_clean_dram_mask(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
