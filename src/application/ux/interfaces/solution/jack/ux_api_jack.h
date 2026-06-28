/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: Provides ux jack api \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-06-16， Create file. \n
 */

#ifndef UX_API_JACK_H
#define UX_API_JACK_H

#include "stdint.h"
#include "errcode.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
/**
 * @brief 插口状态
 */
typedef enum {
    UX_API_JACK_STATE_PLUG_IN = 0,  /**< 插入 */
    UX_API_JACK_STATE_PLUG_OUT,     /**< 拔出 */
    UX_API_JACK_STATE_MAX,          /**< 最大值 */
} ux_api_jack_state_t;

/**
 * @brief 获取插口状态
 * @return ux_api_jack_state_t 插口状态，包括：插入、拔出
 */
ux_api_jack_state_t ux_api_jack_get_state(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
