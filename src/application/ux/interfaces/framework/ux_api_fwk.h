/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: ux framework message and event process header.\n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-06-06, Create file. \n
 */
#ifndef UX_API_FWK_H
#define UX_API_FWK_H

#include "stdint.h"
#include "errcode.h"
#include "ux_api_msg.h"
#include "ux_event_define.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @brief ux框架优先级枚举
 */
typedef enum {
    UX_API_PRIO_CRITICAL = 0x0,     /**< 关键优先级 */
    UX_API_PRIO_HIGH,               /**< 高优先级 */
    UX_API_PRIO_MEDIUM,             /**< 中优先级 */
    UX_API_PRIO_LOW,                /**< 低优先级 */

    UX_API_PRIORITY_MAX,            /**< 最大值 */
} ux_api_prio_t;

/**
 * @brief 模块函数指针类型
 * @param event 事件类型
 * @param data 数据指针
 * @param data_len 数据长度
 * @return errcode_t 返回操作的状态码
 */
typedef errcode_t (*ux_api_module_func)(uint8_t event, uint8_t *data, uint16_t data_len);

/**
 * @brief ux框架系统资源初始化
 */
void ux_api_fwk_system_init(void);

/**
 * @brief ux框架系统资源反初始化
 */
void ux_api_fwk_system_deinit(void);

/**
 * @brief ux任务消息处理线程
 */
int ux_api_fwk_task(void *data);

/**
 * @brief 发送消息通知ux，含消息全量信息及数据信息
 * @param [in] mail 指向通用的消息配置结构体的指针，含事件类型，子事件类型等。
 * @param [in] data uint8_t类型 数据缓存区。
 * @param [in] data_len 数据缓存区长度。
 * @return errcode_t 返回操作的状态码，表示调用成功或失败的具体原因。
 */
errcode_t ux_api_fwk_push_mail(ux_api_comm_mail_t *mail, uint8_t *data, uint16_t data_len);

/**
 * @brief 发送消息通知ux，含事件及数据信息
 * @param [in] event 事件类型。
 * @param [in] sub_event 子事件类型。
 * @param [in] data uint8_t类型 数据缓存区。
 * @param [in] data_len 数据缓存区长度。
 * @return errcode_t 返回操作的状态码，表示调用成功或失败的具体原因。
 */
errcode_t ux_api_fwk_push_event(uint8_t event, uint8_t sub_event, uint8_t *data, uint16_t data_len);

/**
 * @brief 注册事件处理回调函数，配置默认优先级
 * @param [in] event 事件类型。
 * @param [in] fun 事件对应注册回调函数。
 * @return errcode_t 返回操作的状态码，表示调用成功或失败的具体原因。
 */
errcode_t ux_api_fwk_module_register(ux_event_type_t event, ux_api_module_func fun);

/**
 * @brief 注册事件处理回调函数
 * @param [in] event 事件类型。
 * @param [in] fun 事件对应注册回调函数。
 * @param [in] priority 事件优先级。
 * @return errcode_t 返回操作的状态码，表示调用成功或失败的具体原因。
 */
errcode_t ux_api_fwk_module_register_prio(ux_event_type_t event, ux_api_module_func fun, ux_api_prio_t priority);

/**
 * @brief 事件处理
 * @param [in] event 事件类型。
 * @param [in] sub_event 子事件类型。
 * @param [in] data uint8_t类型 数据缓存区。
 * @param [in] data_len 数据缓存区长度。
 * @return errcode_t 返回操作的状态码，表示调用成功或失败的具体原因。
 */
errcode_t ux_api_fwk_process_event(uint8_t event, uint8_t sub_event, uint8_t *data, uint16_t data_len);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif