/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: hal common header. \n
 *
 * History: \n
 * 2023-03-22, Create file. \n
 */
#ifndef HAL_COMMON_H
#define HAL_COMMON_H

#include "crypto_type.h"
#include "crypto_symc_struct.h"

#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif
#endif

/**
 * @brief module set for clock open. only need to set once after module reset.
 *
 * @param module_id module id value.
 */
void hal_sec_open_clk(unsigned int module_id);

/**
 * @brief module set for clock close.
 *
 * @param module_id module id value.
 */
void hal_sec_close_clk(unsigned int module_id);

/**
 * @brief close all clock.
 */
void hal_sec_close_all_clk(void);

/**
 * @brief module enable clock vote according to efuse. only need to enable once after module reset.
 *
 * @param module_id module id value.
 */
void hal_sec_vote_enable(unsigned int module_id);

/**
 * @brief module disable clock vote.
 *
 * @param module_id module id value.
 */
void hal_sec_vote_disable(unsigned int module_id);

/**
 * @brief module vote for clock enable or disable.
 *
 * @param enable true: open clock, false: close clock.
 * @param module_id module id value.
 */
void hal_sec_vote(td_bool enable, unsigned int module_id);

/**
 * get cpu type.
 */
crypto_cpu_type hal_sec_get_cpu_type(void);

void hal_trng_set_entropy_source(void);
void hal_trng_open_clk(void);
void hal_trng_close_clk(void);

void get_sec_vote_enable_value(uint8_t *is_sec_vote_disable);

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif