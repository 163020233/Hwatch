
/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: Provide ux charge interface \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-11-11， Create file. \n
 */

#ifndef UX_API_CHARGE_H
#define UX_API_CHARGE_H
#include "errcode.h"
#include "securec.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief 充电状态枚举
 */
typedef enum {
    UX_API_CHARGE_STATE_NOT_CHARGE,              /**< 非充电模式 */
    UX_API_CHARGE_STATE_CHARGING,                /**< 充电模式 */
    UX_API_CHARGE_STATE_CHARGE_DONE,             /**< 充电完成模式 */
    UX_API_CHARGE_STATE_MAX,                     /**< 最大值 */
} ux_api_charge_state_t;

/**
 * @brief 获取电池当前电量
 * @param [out] val 当前电池电量
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_charger_get_battery_percentage(uint32_t *val);

/**
 * @brief 获取当前充电状态
 * @param [out] state 充电状态
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_charger_get_charge_state(ux_api_charge_state_t *state);

/**
 * @brief 电池电量校准
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_charger_battery_calibration(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif