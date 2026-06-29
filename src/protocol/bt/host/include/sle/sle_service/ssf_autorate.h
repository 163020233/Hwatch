/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 *
 * Description: SLE Service Framework (SSF) autorate operation APIs
 */
#ifndef SSF_AUTORATE_H
#define SSF_AUTORATE_H

#include <stdbool.h>
#include "sle_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AS_MEMBER_CNT_MAX 0x1F

typedef struct {
    uint8_t group_id;
    uint8_t label_id;
    uint8_t member_cnt;
    uint16_t member_hdl[AS_MEMBER_CNT_MAX];
} sle_iog_phy_param_t;

/**
 * @if Eng
 * @brief Use this function to sets the level to be enabled for the autorate.
 * @param   used_bitmap Level bitmap mapping to be enabled
 * @retval error code. see {@ref errcode_t}
 * @par Dependency:
 * @li  ssf_autorate.h
 * @else
 * @brief  设置autorate需要启用的等级
 * @param  used_bitmap 需要启用的等级位图映射。
 * @retval 执行结果错误码。 参考 {@ref errcode_t}
 * @par 依赖:
 * @li  ssf_autorate.h
 * @endif
 */
errcode_t bs_autorate_set_level_used(uint8_t used_bitmap);

/**
 * @if Eng
 * @brief Use this funtion to reply audio rate change request.
 * @param   rate audio rate
 * @param   accept accept or reject this request
 * @retval error code. see {@ref errcode_t}
 * @par Dependency:
 * @li  ssf_autorate.h
 * @else
 * @brief  回复音频码率更新请求
 * @param  rate 音频码率。
 * @param  accept 接收或拒绝码率更新请求
 * @retval 执行结果错误码。 参考 {@ref errcode_t}
 * @par 依赖:
 * @li  ssf_autorate.h
 * @endif
 */
errcode_t bs_autorate_reply_rate_change_request(uint32_t rate, bool accept);

/**
 * @if Eng
 * @brief Use this funtion to enabling or disabling the autorate.
 * @param   enable The switch is enabled.
 * @retval error code. see {@ref errcode_t}
 * @par Dependency:
 * @li  ssf_autorate.h
 * @else
 * @brief  开启或关闭autorate功能
 * @param  enable 开关使能。
 * @retval 执行结果错误码。 参考 {@ref errcode_t}
 * @par 依赖:
 * @li  ssf_autorate.h
 * @endif
 */
errcode_t bs_autorate_config_enable(bool enable);

/**
 * @if Eng
 * @brief Use this funtion to set autorate default level.
 * @param   level Autorate default level.
 * @retval error code. see {@ref errcode_t}
 * @par Dependency:
 * @li  ssf_autorate.h
 * @else
 * @brief  设置autorate默认档位
 * @param  level autorate默认档位。
 * @retval 执行结果错误码。 参考 {@ref errcode_t}
 * @par 依赖:
 * @li  ssf_autorate.h
 * @endif
 */
errcode_t bs_autorate_config_default_level(uint8_t level);
#ifdef __cplusplus
}
#endif
#endif