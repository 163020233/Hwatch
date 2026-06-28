/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: log flow control
 */

#ifndef LOG_LIMIT_H
#define LOG_LIMIT_H

#include <stdint.h>
#include <stdbool.h>
#include "errcode.h"
#include "diag_config.h"

#define LOG_LIMIT_SET_ALL       ((uint16_t)0xFFFF)
#ifndef LOG_LIMIT_RESET_TIMEOUT
#define LOG_LIMIT_RESET_TIMEOUT (30 * 60 * 1000)  /* 30 minutes */
#endif

typedef struct {
    uint16_t module_id;         /*!< 要限流的模块ID。
                                     使用extend格式的日志时，低8位[7~0]为log_moudle_id_t，最高3bit[15~13]为CORE ID。
                                     使用其他格式日志时，只有log_moudle_id_t */
    bool    enable;             /*!< 该模块限流是否开启 */
    uint8_t group;              /*!< 组号。一般情况下每个模块group不同，当多个模块使用相同的group时，
                                     这些模块共享限流总数（这些模块的max_count之和）。 */
    uint32_t max_count;         /*!< 该模块在一个限流周期内允许的最大日志数。 */
    uint32_t available_count;   /*!< 该模块在当前限流周期内允许的剩余日志数。 */
    uint32_t printed_count;     /*!< 该模块在当前限流周期内已打印的日志数。 */
    uint32_t lost_count;        /*!< 该模块在当前限流周期内丢弃的日志数。 */
    uint32_t all_printed_count; /*!< 该模块累计打印的日志数。 */
    uint32_t all_lost_count;    /*!< 该模块累计丢弃的日志数。 */
} log_limit_t;

typedef void (*log_limit_info_cb_t)(uint8_t core, const log_limit_t *log_limits, uint32_t log_limit_num);

errcode_t uapi_log_limit_init(log_limit_t *log_limits, uint32_t log_limit_num);

void uapi_log_limit_reset(void);

void uapi_log_limit_set_enable(uint8_t core, bool enable);

void uapi_log_limit_set_mod_enable(uint8_t core, uint16_t module_id, bool enable);

#if (defined(CORE) && defined(APPS) && (CORE == APPS))
void uapi_log_limit_get_info(uint8_t core, log_limit_info_cb_t callback);
#endif

bool log_limit_hook(uint16_t module_id, uint8_t level);

#endif /* LOG_LIMIT_H */
