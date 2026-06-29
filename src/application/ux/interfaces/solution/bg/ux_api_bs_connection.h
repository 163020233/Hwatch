/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 * Description: Provide ux bs connection-related functions, such as addresses, names, etc. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-04， Create file. \n
 */

#ifndef UX_API_BS_CONNECTION_H
#define UX_API_BS_CONNECTION_H

#include "stdint.h"
#include "errcode.h"
#include "ux_common_define.h"
#include "ux_api_bs_config.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define UX_API_BS_CONNECTED_DEV_MAX 8              /**< 最大已连接设备数量 */
#define UX_API_BS_NAME_MAX_LEN    32               /**< 设备名称最大长度 */
#define UX_API_BS_PD_NUM_MAX     (10)              /**< 配对设备最大数量 */

/**
 * @brief 星闪连接状态枚举
 */
typedef enum {
    UX_API_BS_CONN_STATE_CONNECTING = 0x01,    /*!< @if Eng status of connecting. @else 正在连接。 @endif */
    UX_API_BS_CONN_STATE_CONNECTED = 0x02,     /*!< @if Eng status of connected. @else 已连接。 @endif */
    UX_API_BS_CONN_STATE_DISCONNECTING = 0x03, /*!< @if Eng status of disconnecting. @else 正在断连。 @endif */
    UX_API_BS_CONN_STATE_DISCONNECTED = 0x04,  /*!< @if Eng status of disconnected. @else 已断连。 @endif */
    UX_API_BS_CONN_STATE_MSG_CHANNEL_READY = 0x05,  /**< BLE消息通道已ready */
    UX_API_BS_CONN_STATE_MAX,                  /**< 最大值 */
} ux_api_bs_conn_state_t;

/**
 * @brief 星闪设备状态枚举
 */
typedef enum {
    UX_API_BS_DEVICE_STATUS_PAIRING_INIT = 0,       /**< 配对初始化 */
    UX_API_BS_DEVICE_STATUS_START_PAIR,             /**< 耳机进入配对模式 */
    UX_API_BS_DEVICE_STATUS_STOP_PAIR,              /**< 耳机停止配对模式 */
    UX_API_BS_DEVICE_STATUS_START_CONNECT,          /**< 耳机进入回连模式 */
    UX_API_BS_DEVICE_STATUS_STOP_CONNECT,           /**< 耳机停止回连模式 */
    UX_API_BS_DEVICE_STATUS_START_DISCOVERY,        /**< 耳机处于广播模式(星闪/BLE) */
    UX_API_BS_DEVICE_STATUS_STOP_DISCOVERY,         /**< 耳机停止广播模式(星闪/BLE) */
    UX_API_BS_DEVICE_STATUS_DISCONNECT,             /**< 耳机进入断连模式 */

    UX_API_BS_DEVICE_STATUS_MAX
} ux_api_bs_device_state_t;

/**
 * 服务状态枚举类型定义
 * 定义了主机服务的各种状态
 */
typedef enum {
    UX_API_BS_HOST_SERVICE_STATE_NOT_READY,         /**< 服务未就绪状态 */
    UX_API_BS_HOST_SERVICE_STATE_IDLE,              /**< 服务空闲状态 */
    UX_API_BS_HOST_SERVICE_STATE_INCOMING_CALL,     /**< 来电状态 */
    UX_API_BS_HOST_SERVICE_STATE_CALL_ACTIVE,       /**< 通话进行中状态 */
    UX_API_BS_HOST_SERVICE_STATE_AUDIO_ACTIVE,      /**< 音频激活状态 */
    UX_API_BS_HOST_SERVICE_STATE_OUTGOING_CALL,     /**< 呼出状态 */
    UX_API_BS_HOST_SERVICE_STATE_HELD_CALL,         /**< 挂起通话状态 */
    UX_API_BS_HOST_SERVICE_STATE_RINGING_CALL,      /**< 来电响铃 */
    UX_API_BS_HOST_SERVICE_STATE_MAX                /**< 状态数量上限 */
} ux_api_bs_host_service_state_t;

/**
 * @brief 已连接设备列表结构体
 */
typedef struct {
    ux_api_bs_addr_t devices_list[UX_API_BS_CONNECTED_DEV_MAX]; /**< 已连接设备地址列表 */
    uint8_t number; /**< 实际已连接的设备个数 */
} ux_api_bs_connected_dev_list_t;

/**
 * 已配对设备信息结构体
 * 用于存储已配对设备的详细信息
 */
typedef struct {
    ux_api_bs_addr_t addr; /**< 已配对设备地址 */
    char device_name[UX_API_BS_NAME_MAX_LEN]; /**< 设备名称 */
    uint16_t device_id; /**< 设备ID */
} ux_api_bs_paired_device_info_t;


/**
 * 配对状态枚举类型
 * 定义了设备配对的各种状态
 */
typedef enum {
    UX_API_BS_PAIR_STATE_UNPAIRED,    /**< 未配对状态 */
    UX_API_BS_PAIR_STATE_PAIRING,     /**< 配对中状态 */
    UX_API_BS_PAIR_STATE_PAIRED,      /**< 配对结束状态 */

    UX_API_BS_PAIR_STATE_MAX,
} ux_api_bs_pair_state_t;

/**
 * 连接信息结构体
 * 用于存储设备连接的相关信息
 */
typedef struct {
    ux_api_bs_addr_t addr;                  /**< 设备地址 */
    uint16_t device_id;                     /**< 设备ID */
    ux_api_bs_conn_state_t conn_state;      /**< 连接状态 */
} ux_api_bs_conn_info_t;

/**
 * 配对模式枚举类型
 * 定义了设备配对的不同模式
 */
typedef enum {
    UX_API_BS_PAIRING_MODE_DEFAULT,     /**< 默认配对模式 */
    UX_API_BS_PAIRING_MODE_AIR,         /**< 空中配对模式 */
    UX_API_BS_PAIRING_MODE_WHITELIST,   /**< 白名单配对模式 */

    UX_API_BS_PAIRING_MODE_MAX,
} ux_api_bs_pairing_mode_t;

/**
 * 设备角色枚举类型
 * 定义了设备在连接中的角色
 */
typedef enum {
    UX_API_BS_ROLE_CENTRAL = 0,     /**< 中心设备角色 */
    UX_API_BS_ROLE_PERIPHERAL,      /**< 边缘设备角色 */
    UX_API_BS_ROLE_MAX,
} ux_api_bs_role_t;

/**
 * 连接顺序结构体
 * 用于存储蓝牙重连的设备顺序
 */
typedef struct {
    uint8_t connect_order[UX_API_BS_PD_NUM_MAX]; /**< 蓝牙重连顺序数组 */
} ux_api_bs_connect_order_t;

/**
 * @brief 设备重连阶段枚举
 */
typedef enum {
    UX_API_BS_DEVICE_RECONN_PHASE_ALL = 0,                   /* 同时回连对耳、设备A 设备B(若支持) */
    UX_API_BS_DEVICE_RECONN_PHASE_PEER,                       /* 只回连对耳 */
    UX_API_BS_DEVICE_RECONN_PHASE_FINISH,
    UX_API_BS_DEVICE_RECONN_PHASE_MAX,
} ux_api_bs_device_reconn_phase_t;

/**
 * @brief 获取主机服务状态
 * @return 当前主机服务的状态值, @ref ux_api_bs_host_service_state_t
 */
ux_api_bs_host_service_state_t ux_api_bs_get_host_service_state(void);

/**
 * @brief 获取与手机设备的连接状态
 * @param [in] protocol 协议类型
 * @return 连接状态, @ref ux_api_bs_conn_state_t
 */
ux_api_bs_conn_state_t ux_api_bs_get_connect_state(ux_api_bs_protocol_t protocol);

/**
 * @brief 获取对耳与手机设备的连接状态
 * @return ux_api_bs_conn_state_t 返回对耳与手机设备的连接状态
 */
ux_api_bs_conn_state_t ux_api_bs_get_peer_device_connect_state(void);

/**
 * @brief 获取与对耳设备的连接状态(SPP链路连接状态)
 * @return ux_api_bs_conn_state_t 返回双耳间SPP连接的状态
 */
ux_api_bs_conn_state_t ux_api_bs_get_peer_connect_state(void);

/**
 * @brief 获取目标设备连接状态
 * @param [in] device_id 设备ID
 * @return 连接状态 {@ref ux_api_bs_conn_state_t}
 */
ux_api_bs_conn_state_t ux_api_bs_get_target_device_connect_state(uint16_t device_id);

/**
 * @brief 获取指定协议类型的连接数量
 * @param [in] protocol 指定的协议类型
 * @return 返回指定协议类型的已连接数量
 */
uint16_t ux_api_bs_get_connected_nums(ux_api_bs_protocol_t protocol);

/**
 * @brief 获取对耳连接的设备数量
 * @return uint16_t 返回已连接设备的数量
 */
uint16_t ux_api_bs_get_peer_device_connected_nums(void);

/**
 * @brief 获取设备配对回连状态
 * @param [in] protocol 协议类型
 * @return 设备状态, @ref ux_api_bs_device_state_t
 */
ux_api_bs_device_state_t ux_api_bs_get_device_state(ux_api_bs_protocol_t protocol);

/**
 * @brief 获取对耳设备配对回连状态
 * @param [in] protocol 协议类型
 * @return 配对状态 {@ref ux_api_bs_device_state_t}
 */
ux_api_bs_device_state_t ux_api_bs_get_peer_device_state(ux_api_bs_protocol_t protocol);

/**
 * @defgroup 支持产品 [livemic_rx]
 * @brief 获取配对状态
 * @return 配对状态 {@ref ux_api_bs_pair_state_t}
 */
ux_api_bs_pair_state_t ux_api_bs_get_pair_state(void);

/**
 * @brief 获取远程设备连接信息
 * @defgroup 支持产品 [livemic_rx]
 * @param [out] conn_info_list 连接信息列表
 * @param [in] expect_num 期望获取的数量
 * @param [out] actual_num 实际获取的数量
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bs_get_remote_connect_info(ux_api_bs_conn_info_t *conn_info_list, uint8_t expect_num,
                                            uint8_t *actual_num);

/**
 * @brief 开始发现模式
 * @param [in] mode 配对模式 {@ref ux_api_bs_pairing_mode_t}
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bs_start_discovery(ux_api_bs_pairing_mode_t mode);

/**
 * @brief 关闭发现模式
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bs_stop_discovery(void);

/**
 * @brief 发起回连
 * @param [in] time 回连时间，取值范围[30s, 1800s]
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bs_start_connect(uint32_t time);

/**
 * @brief 停止回连
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bs_stop_connect(void);

/**
 * @brief 开始音频模式
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bs_start_audio_mode(void);

/**
 * @brief 停止音频模式
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bs_stop_audio_mode(void);

/**
 * @brief 开始对耳发现
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bs_start_peer_discovery(void);

/**
 * @brief 分离对耳连接
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bs_start_peer_separate(void);

/**
 * @brief 断开所有设备连接
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bs_disconnect(void);

/**
 * @brief 断开指定设备连接
 * @param [in] protocol 协议类型
 * @param [in] addr 设备地址
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bs_disconnect_by_addr(ux_api_bs_protocol_t protocol, ux_api_bs_addr_t addr);

/**
 * @brief 获取已配对设备数量
 * @return uint8_t 已配对设备数量
 */
uint8_t ux_api_bs_get_paired_nums(ux_api_bs_protocol_t protocol);

/**
 * @brief 获取已配对设备列表
 * @param [out] pdl_list 已配对设备信息列表
 * @param [in] pdl_num 期望获取的设备数量
 * @param [out] actual_num 实际获取的设备数量
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bs_get_paired_device_list(ux_api_bs_protocol_t protocol, ux_api_bs_paired_device_info_t *pdl_list,
                                           uint8_t pdl_num, uint8_t *actual_num);

/**
 * @brief 获取已连接设备的地址列表
 * @param [in] protocol 协议类型
 * @param [out] devices 存储获取到的设备地址列表的结构体指针
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bs_get_connected_device_addr_list(ux_api_bs_protocol_t protocol,
                                                   ux_api_bs_connected_dev_list_t *devices);

/**
 * @brief 获取连接顺序
 * @param [out] order 连接顺序结构体指针
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bs_get_connect_order(ux_api_bs_connect_order_t *order);

/**
 * @brief 获取设备重连阶段
 * @return 设备重连阶段 {@ref ux_api_bs_device_reconn_phase_t}
 */
ux_api_bs_device_reconn_phase_t ux_api_bs_get_device_reconnect_phase(void);

/**
 * @brief 获取多点连接主设备地址
 * @param [out] addr 设备地址
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bs_get_multipoint_main_addr(ux_api_bs_addr_t *addr);

/**
 * @brief 获取多点连接非主设备地址
 * @param [out] addr 设备地址
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bs_get_multipoint_other_addr(ux_api_bs_addr_t *addr);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
