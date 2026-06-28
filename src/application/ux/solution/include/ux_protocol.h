/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: ux_protocol.c header file
 * Author: AuthorNameMagicTag
 * Create: 2025-06-09
 */

#ifndef UX_PROTOCOL_H
#define UX_PROTOCOL_H

#include "stdint.h"
#include "errcode.h"
#include "ux_fwk_msg.h"
#include "diag_service.h"
#include "conn_tlv_err_code.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define TYPE_MAX_LEN                  0x12
#define USB_HID_CUSTOM_MSG_LEN     63 // msg_len defined by USB driver, USB will add 1byte report id
#define MAP_PROTOCOL_VOLUME 80
typedef tlv_app_op_code_t (*protocol_msg_proc_fun_cb)(diag_ser_data_t *msg);

/* 联接协议注册结构 */
typedef struct {
    uint8_t cmd_id; // 每个model下的cmd id
    protocol_msg_proc_fun_cb fun; // 对应的具体消息处理函数
} protocol_msg_proc;

void ux_protocol_init(void);
void ux_protocol_deinit(void);
void protocol_cmd_cnt_init(void);
void protocol_cmd_map_init(void);
void protocol_dev_init(void);
protocol_msg_proc_fun_cb ux_protocol_find_cmd_func(uint8_t model_id, uint8_t cmd_id);
void ux_protocol_register_cmd_handle(uint8_t model_id, uint8_t cmd_id, protocol_msg_proc_fun_cb fun);
void protocol_send_err_code(diag_ser_data_t *msg, tlv_app_op_code_t ret);
void ux_protocol_reset_err_code(void);
tlv_app_op_code_t ux_protocol_get_err_code(void);
void ux_protocol_set_err_code(tlv_app_op_code_t err_code);
uint8_t uapi_get_cmd_cnt(diag_ser_id_t ser_id);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
