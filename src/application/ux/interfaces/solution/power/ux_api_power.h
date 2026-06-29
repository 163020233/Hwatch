/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: Provide ux power interface \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-11-11， Create file. \n
 */

#ifndef UX_API_POWER_H
#define UX_API_POWER_H
#include "errcode.h"
#include "securec.h"
#include "stdbool.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief 关机原因枚举
 */
typedef enum {
    UX_API_POWER_OFF_NORMAL,             /**< 无需特殊处理的关机 */
    UX_API_POWER_OFF_OTA,                /**< OTA触发关机 */
    UX_API_POWER_OFF_RECOVER,            /**< 恢厂触发关机 */
    UX_API_POWER_OFF_APP,                /**< APP触发关机 */
    UX_API_POWER_OFF_DUT,                /**< DUT触发关机 */
    UX_API_POWER_OFF_BATTERY,            /**< 电量低触发关机 */
    UX_API_POWER_OFF_USER,               /**< 自定义模式关机 */
    UX_API_POWER_OFF_MAX,                  /**< 最大值 */
} ux_api_power_off_reason_t;

/**
 * @brief 关机模式枚举
 */
typedef enum {
    UX_API_POWER_OFF_MODE_HARD,          /**< 切断主控芯片供电 */
    UX_API_POWER_OFF_MODE_SOFT,          /**< 主控芯片进入休眠模式 */
    UX_API_POWER_OFF_MODE_MAX            /**< 关机模式最大值 */
} ux_api_power_off_mode_t;

/**
 * @brief 下电。
 * @param [in] reason 下电原因
 * @param [in] time 延时下电时间，传0即立刻下电
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_power_off(ux_api_power_off_reason_t reason, uint32_t time);

/**
 * @brief 取消延时下电
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_power_cancel_delay_off(void);

/**
 * @brief 重启
 * @param [in] reason 重启原因
 * @param [in] time 延时重启时间，传0即立刻重启
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_power_reset(ux_api_power_off_reason_t reason, uint32_t time);

/**
 * @brief 取消延时重启
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_power_cancel_delay_reset(void);

/**
 * @brief 设置电源关闭原因, 该函数用于设置系统电源关闭的原因，以便后续分析和记录
 * @param [in] reason 电源关闭的原因，类型为ux_api_power_off_reason_t
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_power_set_off_reason(ux_api_power_off_reason_t reason);

/**
 * @brief 获取关机原因
 * @return ux_api_power_off_reason_t 下电原因
 */
ux_api_power_off_reason_t ux_api_power_get_off_reason(void);

/**
 * @brief 启动整机业务
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_power_on(void);

/**
 * @brief 整机上电
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_power_acc_on(void);

/**
 * @brief 整机待机
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_power_standby(void);

/**
 * @brief 整机下电
 * @return 返回错误码，表示操作是否成功
 */
errcode_t ux_api_power_acc_off(void);

/**
 * @brief 设置是否使能自动关机
 * @param [in] enable 是否使能自动关机，true表示使能，false表示不使能
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_power_set_auto_power_off(bool enable);

/**
 * @brief 设置关机模式
 * @param [in] mode 关机模式 {@ref ux_api_power_off_mode_t}
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_power_set_power_off_mode(ux_api_power_off_mode_t mode);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
