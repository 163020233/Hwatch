/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description: LOG MODULE
 *
 * Create:
 */

#ifndef LOG_H
#define LOG_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#ifdef LOG_TRIGGER_SET
void log_set_trigger(bool value);

bool log_get_trigger(void);
#endif

#ifdef ENABLE_LOG_CACHE_FUNCTION
void log_set_cache_flag(bool flag);

bool log_get_cache_flag(void);

void log_force_trigger_log(void);
#endif

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif