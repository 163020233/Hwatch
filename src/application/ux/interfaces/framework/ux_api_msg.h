/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: ux api msg data structure.\n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-06-06, Create file. \n
 */
#ifndef UX_API_MSG_H
#define UX_API_MSG_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define NOTIFY_MAIL_DATA_LENGTH  6                    /**< 通知邮件数据长度 */

/**
 * @brief 应用类型枚举
 */
typedef enum {
    UX_API_APP_BUTTON,     /**< 按键应用 */
    UX_API_APP_TOUCH,      /**< 触摸应用 */
    UX_API_APP_MAX,        /**< 最大值 */
} ux_api_app_type;

/**
 * @brief 通信邮件结构体
 */
typedef struct {
    uint8_t event;                           /**< 事件 */
    uint8_t sub_event;                       /**< 子事件，发给ux时需要填写 */
    uint8_t res[2];                          /**< 预留字段 */
    ux_api_app_type app;                     /**< 应用类型 */
} ux_api_comm_mail_t;

/**
 * @brief 通用通知邮件结构体
 */
typedef struct {
    ux_api_comm_mail_t comm;                /**< 通信邮件 */
    uint32_t data[NOTIFY_MAIL_DATA_LENGTH]; /**< 数据 */
    void* pri;                               /**< 私有数据 */
    void* config_buff;                       /**< 配置缓冲区 */
    uint16_t config_len;                     /**< 配置长度 */
} ux_api_common_notify_mail_t;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
