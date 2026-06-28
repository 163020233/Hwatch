/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * File          ux_sync_commu.h
 * Description:  provide ux sync_commu header file.
 */
#ifndef UX_SYNC_COMMU_H
#define UX_SYNC_COMMU_H
#include "ux_api_communication.h"

/**
 * 初始化同步通信上下文
 * 此函数用于设置同步通信的初始状态
 */
void ux_commu_sync_init(void);

/**
 * 去初始化同步通信上下文
 * 此函数用于清理同步通信的上下文环境
 */
void ux_commu_sync_deinit(void);

/**
 * 获取同步通信数据
 * @param data 指向存储数据的缓冲区的指针
 * @param data_len 要获取的数据长度
 * @return 返回错误代码，0表示成功
 */
errcode_t ux_commu_sync_get_data(uint8_t *data, uint16_t data_len);

/**
 * 发送同步通信消息
 * @param commu_msg 指向通信消息的指针
 * @param msg_len 消息的长度
 * @return 返回错误代码，0表示成功
 */
errcode_t ux_commu_sync_push_commu_msg(ux_api_commu_control_msg_t *commu_msg, uint16_t msg_len);

/**
 * 发送同步通信请求
 * @param cmd 请求的命令
 * @param device_id 设备ID
 * @param data 指向请求数据的指针
 * @param data_len 请求数据的长度
 * @return 返回错误代码，0表示成功
 */
errcode_t ux_commu_sync_request(ux_api_commu_cmd_t cmd, uint16_t device_id, uint8_t *data, uint16_t data_len);

/**
 * 发送同步通信响应
 * @param cmd 响应的命令
 * @param device_id 设备ID
 * @param data 指向响应数据的指针
 * @param data_len 响应数据的长度
 * @param ack_code 执行结果错误码
 * @return 返回错误代码，0表示成功
 */
errcode_t ux_commu_sync_response(ux_api_commu_cmd_t cmd, uint16_t device_id, uint8_t *data, uint16_t data_len,
                                 errcode_t ack_code);
/**
 * 判断是否为立即执行事件
 * @param data 指向事件数据的指针
 * @param data_len 事件数据的长度
 * @return 如果是立即执行事件，返回true，否则返回false
 */
bool ux_commu_is_immediate_exec_event(uint8_t *data, uint16_t data_len);

/**
 * 注册指定通信指令的回调函数
 * @param cmd_type 通信指令
 * @param handler 回调函数
 * @return 返回错误代码，0表示成功
 */
errcode_t ux_commu_register_msg_handler(ux_api_commu_cmd_t cmd_type, ux_api_commu_msg_handler handler);
#endif
