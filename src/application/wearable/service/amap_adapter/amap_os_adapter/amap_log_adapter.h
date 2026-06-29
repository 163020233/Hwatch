/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Create Date : 2025-11
 */

#ifndef AMAP_LOG_ADAPTER
#define AMAP_LOG_ADAPTER

#include <stdint.h>

#ifndef AMAP_LOG_LEVEL
#define AMAP_LOG_LEVEL 3  // 0=全关, 1=Error, 2=+Warn, 3=+Info, 4=+Debug
#endif

typedef enum {
    AMAP_LOG_LEVEL_NONE = 0,
    AMAP_LOG_LEVEL_ERROR,
    AMAP_LOG_LEVEL_WARN,
    AMAP_LOG_LEVEL_INFO,
    AMAP_LOG_LEVEL_DEBUG
} AmapLogLevel;

#if AMAP_LOG_LEVEL >= 1
#define AMAP_LOGE(fmt, ...) amap_log_output(AMAP_LOG_LEVEL_ERROR, "[%s] " fmt, __func__, ##__VA_ARGS__)
#else
#define AMAP_LOGE(fmt, ...)
#endif

#if AMAP_LOG_LEVEL >= 2
#define AMAP_LOGW(fmt, ...) amap_log_output(AMAP_LOG_LEVEL_WARN, "[%s] " fmt, __func__, ##__VA_ARGS__)
#else
#define AMAP_LOGW(fmt, ...)
#endif

#if AMAP_LOG_LEVEL >= 3
#define AMAP_LOGI(fmt, ...) amap_log_output(AMAP_LOG_LEVEL_INFO, "[%s] " fmt, __func__, ##__VA_ARGS__)
#else
#define AMAP_LOGI(fmt, ...)
#endif

#if AMAP_LOG_LEVEL >= 4
#define AMAP_LOGD(fmt, ...) amap_log_output(AMAP_LOG_LEVEL_DEBUG, "[%s] " fmt, __func__, ##__VA_ARGS__)
#else
#define AMAP_LOGD(fmt, ...)
#endif
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @brief 获取当前系统时间（毫秒）
 */
uint64_t amap_get_system_time(void);
/**
 * @brief 高德sdk日志打印函数
 */
int amap_log_printf(const char *fmt, ...);
/**
 * @brief 内部日志输出函数（带级别）
 */
int amap_log_output(AmapLogLevel level, const char *fmt, ...);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  // AMAP_LOG_ADAPTER