/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provide ux event wait \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-06-11， Create file. \n
 */
#ifndef UX_FWK_SYNC_EVENT_H
#define UX_FWK_SYNC_EVENT_H
#include "ux_api_fwk.h"
#include "stdbool.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define UX_FWK_SYNC_EVENT_MSG_LEN_MAX 24

typedef enum {
    UX_FWK_SYNC_EVENT_START = 0x0,                   /* 设置指令起始 */
    UX_FWK_SYNC_EVENT_SET_RECORDER = 0x0,            /* 设置录音 */
    UX_FWK_SYNC_EVENT_SET_VOLUME,                    /* 设置音量 */
    UX_FWK_SYNC_EVENT_GET_REMAIN_TIME,
    UX_FWK_SYNC_EVENT_SET_RECORDER_STATE,
    UX_FWK_SYNC_EVENT_SET_UAC_SPEAKER,
    UX_FWK_SYNC_EVENT_MAX,
} ux_fwk_sync_event_cmd_t;

/* 数传通道通信协议定义 */
typedef struct {
    uint16_t device_id;                              /* 设备ID */
    uint8_t param[UX_FWK_SYNC_EVENT_MSG_LEN_MAX];   /* 命令参数 */
    uint16_t param_len;                             /* 命令参数长度 */
    errcode_t ack_code;                             /* 响应错误码 */
} ux_fwk_sync_event_msg_t;

/**
 * @brief 通信消息处理函数指针类型定义
 * @param device_id 源设备ID
 * @param data 接收到的数据
 * @param data_len 接收到的数据长度
 * @return 错误代码
 */
typedef errcode_t (*ux_fwk_sync_event_handler)(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_fwk_sync_event_register_handler(ux_fwk_sync_event_cmd_t event, ux_fwk_sync_event_handler handler);
errcode_t ux_fwk_sync_event_response(uint8_t event_cmd, uint8_t *data, uint16_t data_len, errcode_t ack_code);
errcode_t ux_fwk_sync_event_request(uint8_t event_cmd, uint16_t device_id, uint8_t *data, uint16_t data_len);
void ux_fwk_sync_event_init(void);
void ux_fwk_sync_event_deinit(void);
bool ux_fwk_sync_event_is_ux_task(void);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
