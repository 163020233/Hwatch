/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * File          ux_bs_manager.h
 * Description:  provide ux bs manager header file.
 */

#ifndef UX_BS_MANAGER_H
#define UX_BS_MANAGER_H

#include <stdint.h>
#include "errcode.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 启动回连模式
 * @return 错误码
 */
errcode_t ux_bs_manager_start_connect(void);

/**
 * @brief 停止连接模式
 * @return 错误码
 */
errcode_t ux_bs_manager_stop_connect(void);

/**
 * @brief 启动配对模式
 * @return 错误码
 */
errcode_t ux_bs_manager_start_discovery(void);

/**
 * @brief 停止配对模式
 * @return 错误码
 */
errcode_t ux_bs_manager_stop_discovery(void);

/**
 * @brief 初始化设备配置参数
 * @return 错误码
 */
errcode_t ux_bs_manager_config_init(void);

/**
 * @brief 初始化协议栈服务
 * @return 错误码
 */
errcode_t ux_bs_manager_enable(void);

/**
 * @brief 去初始化协议栈服务
 * @return 错误码
 */
errcode_t ux_bs_manager_disable(void);

/**
 * @brief 协议栈服务下电
 * @return 错误码
 */
errcode_t ux_bs_manager_acc_off(void);

/**
 * @brief 断开连接的所有设备
 * @return 错误码
 */
errcode_t ux_bs_manager_disconnect(void);

/**
 * @brief 启动对耳配对模式
 * @return 错误码
 */
errcode_t ux_bs_manager_start_discovery_peer(void);

/**
 * @brief 断开 BLE 连接
 * @return 错误码
 */
errcode_t ux_bs_manager_ble_disconnect(void);

/**
 * @brief 启动 BLE 广播
 * @return 错误码
 */
errcode_t ux_bs_manager_ble_start_discovery(void);

/**
 * @brief 停止 BLE 广播
 * @return 错误码
 */
errcode_t ux_bs_manager_ble_stop_discovery(void);

/**
 * @brief 配置 BLE 广播数据
 * @param add_ble_end_val 是否添加 BLE 结束标识值
 * @return 错误码
 */
errcode_t ux_bs_manager_ble_config_adv_data(bool add_ble_end_val);

/**
 * @brief 耳机放入盒中
 * @return 错误码
 */
errcode_t ux_bs_manager_enter_box(void);

/**
 * @brief 耳机从盒子取出
 * @return 错误码
 */
errcode_t ux_bs_manager_exit_box(void);

/**
 * @brief 耳机佩戴入耳
 * @return 错误码
 */
errcode_t ux_bs_manager_wear_in(void);

/**
 * @brief 耳机摘下出耳
 * @return 错误码
 */
errcode_t ux_bs_manager_wear_out(void);

/**
 * @brief 通知对端耳机电池电量
 * @return 错误码
 */
errcode_t ux_bs_manager_notify_battery_volume(void);

/**
 * @brief 切换佩戴检测功能的启用/禁用状态
 * @return 错误码
 */
errcode_t ux_bs_manager_switch_wear_detection(void);

/**
 * @brief 切换低延迟模式的启用/禁用状态
 * @return 错误码
 */
errcode_t ux_bs_manager_switch_low_latency(void);

/**
 * @brief 切换双连接模式的启用/禁用状态
 * @return 错误码
 */
errcode_t ux_bs_manager_switch_dual_connection(void);

/**
 * @brief 配置设备名称
 * @return 错误码
 */
errcode_t ux_bs_manager_config_device_name(void);


#ifdef __cplusplus
}
#endif
#endif // UX_BS_MANAGER_H
