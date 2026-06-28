/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * File          ux_bt_event.h
 * Description: ux bt basic event.
 */
#ifndef UX_BT_EVENT_H
#define UX_BT_EVENT_H

#include <stdint.h>
#include "errcode.h"
#include "bt_service.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 处理BT上报的事件消息
 * @param[in] msg_id 事件消息ID
 * @param[in] param 事件参数指针（类型需与msg_id对应）
 * @return 处理结果错误码
 */
errcode_t ux_bs_bt_event_callback(service_to_ux_msg msg_id, void *param);

#ifdef __cplusplus
}
#endif

#endif

