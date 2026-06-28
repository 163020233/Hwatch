/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * File          ux_api_display.h
 * Description:  provide ux display file.
 */
 
#ifndef UX_DISPLAY_H
#define UX_DISPLAY_H

#include "errcode.h"
#include "stddef.h"
#include "ux_errcode.h"
#include "ux_api_display.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
/**
 * @brief 注册设置事件的回调函数
 * @param event 自定义设置事件
 * @param cb 回调函数
 * @return 返回错误码 @ref ux_errcode.h
 */
errcode_t ux_disp_register_callback(ux_api_disp_event_t event, ux_api_disp_callback cb);

/**
 * @brief 调用通知事件的回调函数
 * @param event 自定义通知事件
 * @param device_id 设备ID
 * @param param 参数
 * @param len 参数长度
 * @return 返回错误码 @ref ux_errcode.h
 */
errcode_t ux_disp_notify(ux_api_disp_notify_event_t event, uint16_t device_id, void* param, uint16_t len);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
