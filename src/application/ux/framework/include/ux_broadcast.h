/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved. \n
 *
 * Description: Provide ux broadcast customer \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2026-03-05， Create file. \n
 */
#ifndef UX_BROADCAST_H
#define UX_BROADCAST_H

#include <stdbool.h>
#include "errcode.h"
#include "ux_api_broadcast.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 广播消息函数
 * @param msg 广播消息指针，指向需要广播的消息，msg不可为NULL
 * @retval ERRCODE_SUCC 成功
 * @retval Other 失败，参考 @ref ux_errcode.h
 */
errcode_t ux_broadcast_broadcast_msg(ux_api_broadcast_msg_t* msg);

/**
 * @brief 广播事件函数
 * @param event 广播事件类型
 * @retval ERRCODE_SUCC 成功
 * @retval Other 失败，参考 @ref ux_errcode.h
 */
errcode_t ux_broadcast_broadcast_event(ux_api_broadcast_event_type_t event);
#ifdef __cplusplus
}
#endif

#endif // UX_BROADCAST_H