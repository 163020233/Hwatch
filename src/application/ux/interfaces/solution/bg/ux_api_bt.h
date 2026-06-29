/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 * Description: Provide ux recovery api \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-04， Create file. \n
 */
 
#ifndef UX_API_BT_H
#define UX_API_BT_H
 
#include "stdint.h"
#include "stdbool.h"
#include "errcode.h"
 
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define UX_API_BT_ADDR_TYPE_PUBLIC 0 /**< 公共地址 */
#define UX_API_BT_ADDR_TYPE_RAMDOM 1 /**< 随机地址，仅ble可选 */
#define UX_API_BT_ADDR_LEN 6         /**< 蓝牙物理地址长度 */
#define DIAG_SPP_DEFAULT_PORT 0x3    /**< diag spp port */

/**
 * @brief 蓝牙地址结构体
 */
typedef struct {
    uint8_t addr[UX_API_BT_ADDR_LEN];   /**< 蓝牙地址 */
    uint8_t type;                       /**< 蓝牙地址类型，0: 公共地址，1: 随机地址 */
} ux_api_bt_addr_t;

/**
 * @brief 蓝牙协议类型枚举
 */
typedef enum {
    UX_API_BT_PROTOCOL_BR = 0,           /**< BR协议 */
    UX_API_BT_PROTOCOL_BLE,              /**< BLE协议 */
    UX_API_BT_PROTOCOL_MAX,              /**< 最大值 */
} ux_api_bt_protocol_t;

/**
 * @brief 蓝牙Profile类型枚举
 */
typedef enum {
    UX_API_BT_PROFILE_BR = 0,        /**< BR协议 */
    UX_API_BT_PROFILE_BLE,           /**< BLE协议 */
    UX_API_BT_PROFILE_SCO,           /**< SCO协议 */
    UX_API_BT_PROFILE_A2DP,          /**< A2DP协议 */
    UX_API_BT_PROFILE_HFP,           /**< HFP协议 */
    UX_API_BT_PROFILE_AVRCP,         /**< AVRCP协议 */
    UX_API_BT_PROFILE_MAX,           /**< 最大值 */
} ux_api_bt_profile_t;

/**
 * @brief 蓝牙Profile连接状态枚举
 */
typedef enum {
    UX_API_BT_PROFILE_STATE_CONNECTING = 0x01,    /*!< @if Eng status of connecting. @else 正在连接。 @endif */
    UX_API_BT_PROFILE_STATE_CONNECTED = 0x02,     /*!< @if Eng status of connected. @else 已连接。 @endif */
    UX_API_BT_PROFILE_STATE_DISCONNECTING = 0x03, /*!< @if Eng status of disconnecting. @else 正在断连。 @endif */
    UX_API_BT_PROFILE_STATE_DISCONNECTED = 0x04,  /*!< @if Eng status of disconnected. @else 已断连。 @endif */
    UX_API_BT_PROFILE_STATE_MAX,     /**< 最大值 */
} ux_api_bt_profile_state_t;

/**
 * @brief 蓝牙角色枚举
 */
typedef enum {
    UX_API_BT_AS_CENTRAL,            /**< 中央设备 */
    UX_API_BT_AS_PERIPHERAL,         /**< 周边设备 */
    UX_API_BT_AS_MAX,                /**< 最大值 */
} ux_api_bt_role_t;

/**
 * @brief 蓝牙配置项枚举
 */
typedef enum {
    UX_API_BT_CONFIG_LOCAL_ADDR = 0,        /**< 配置地址，param为6字节uint8_t数组 */
    UX_API_BT_CONFIG_LOCAL_NAME,            /**< 配置名称，param为字符串，最大长度为32 */
    UX_API_BT_CONFIG_ADV_DATA,              /**< 广播扩展字段，uint8_t数组 */
    UX_API_BT_CONFIG_ADV_SCAN_RSP,          /**< 扫描响应扩展字段，uint8_t数组 */
    UX_API_BT_CONFIG_CONNECT_PARAM,         /**< 更新连接参数，参考 @ref ux_api_bt_conn_param_t */
    UX_API_BT_CONFIG_CONNECT_PHY,           /**< 更新物理层参数，参考 @ref ux_api_bt_phy_t */
    UX_API_BT_CONFIG_PAIR_USE_CTKD,         /**< 开关一键双连，只有在识别对端支持一键双连后才打开 */
    UX_API_BT_CONFIG_PROFILE_CONNECT,       /**< 自动连接服务，@ref ux_api_bt_auto_connect_profile_t */
    UX_API_BT_CONFIG_MAX,
} ux_api_bt_config_t;

/**
 * @brief 蓝牙连接参数结构体
 */
typedef struct {
    ux_api_bt_addr_t addr;       /*!< @if Eng addr @ref ux_api_bt_addr_t
                                      @else   地址 参考 @ref ux_api_bt_addr_t @endif */
    uint16_t interval_min;       /*!< @if Eng min interval,the value range is 6-3200,the unit is 1.25ms
                                      @else   最小间隔,值范围6-3200,单位为1.25ms @endif */
    uint16_t interval_max;       /*!< @if Eng max interval,the value range is 6-3200,the unit is 1.25ms
                                      @else   最大间隔,值范围6-3200,单位为1.25ms @endif */
    uint16_t slave_latency;      /*!< @if Eng slave reply min latency,the value range is 0-499
                                      @else   从设备回复最小间隔，值范围0-499 @endif */
    uint16_t timeout_multiplier; /*!< @if Eng interval for disconnection due to timeout,
                                              the value range is 10-3200,the unit is 10ms
                                      @else   超时断连间隔，值范围10-3200，单位为10ms @endif */
} ux_api_bt_conn_param_t;

/**
 * @brief 蓝牙PHY结构体
 */
typedef struct {
    ux_api_bt_addr_t addr;      /*!< @if Eng addr @ref ux_api_bt_addr_t
                                     @else   地址 参考 @ref ux_api_bt_addr_t @endif */
    uint8_t all_phys;           /*!< @if Eng all phys
                                     @else   所有PHY @endif */
    uint8_t tx_phys;            /*!< @if Eng tx phys
                                     @else   发送PHY @endif */
    uint8_t rx_phys;            /*!< @if Eng rx phys
                                     @else   接收PHY @endif */
    uint16_t phy_options;       /*!< @if Eng phy options
                                     @else   PHY选项 @endif */
} ux_api_bt_phy_t;

/**
 * @brief 蓝牙自动连接Profile结构体
 */
typedef struct {
    bool a2dp_snk : 1; /*!< @if Eng support a2dp sink profile.
                            @else 支持a2dp sink profile。 @endif */
    bool a2dp_src : 1; /*!< @if Eng support a2dp source profile.
                            @else 支持a2dp source profile。 @endif */
    bool avrcp_ct : 1; /*!< @if Eng support avrcp control profile.
                            @else 支持avrcp control profile。 @endif */
    bool avrcp_tg : 1; /*!< @if Eng support avrcp target profile.
                            @else 支持avrcp target profile。 @endif */
    bool hfp_hf : 1;   /*!< @if Eng support hfp hands free profile.
                            @else 支持hfp hands free profile。 @endif */
    bool hfp_ag : 1;   /*!< @if Eng support audio gate free profile.
                            @else 支持hfp audio gate profile。 @endif */
} ux_api_bt_auto_connect_profile_t;

/**
 * @brief 蓝牙本地信息结构体
 */
typedef struct {
    bool enable;           /**< 协议栈状态 */
    ux_api_bt_addr_t addr; /**< 本地地址 */
    char name[32];         /**< 本地名称 */
    uint32_t res[8];       /**< 预留字段 */
} ux_api_bt_local_info_t;

/**
 * @brief 蓝牙设备信息结构体
 */
typedef struct {
    ux_api_bt_addr_t addr;               /**< 地址 */
    ux_api_bt_profile_state_t acl_state; /**< acl连接状态 */
    char name[32];                       /**< 名称 */
    uint32_t device_class;               /**< 设备类型 */
    ux_api_bt_role_t role;               /**< 设备角色 */
    ux_api_bt_protocol_t protocol;       /**< 协议类型 */
    uint32_t res[8];                     /**< 预留字段 */
} ux_api_bt_device_info_t;

/**
 * @brief SPP消息处理函数类型定义
 */
typedef errcode_t (*ux_api_spp_msg_handler)(uint8_t *data, uint16_t data_len);

/**
 * @brief 打开蓝牙
 * @param [in] protocol 协议类型
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bt_enable(ux_api_bt_protocol_t protocol);

/**
 * @brief 关闭蓝牙
 * @param [in] protocol 协议类型
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bt_disable(ux_api_bt_protocol_t protocol);

/**
 * @brief 启动discovery发现，含adv和scan
 * @param [in] role 扫描时设备的角色
 * @param [in] protocol 协议类型
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bt_start_discovery(ux_api_bt_role_t role, ux_api_bt_protocol_t protocol);

/**
 * @brief 停止discovery发现，含adv和scan
 * @param [in] role 停止扫描的设备角色
 * @param [in] protocol 协议类型
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bt_stop_discovery(ux_api_bt_role_t role, ux_api_bt_protocol_t protocol);

/**
 * @brief 启动配对
 * @param [in] addr 待配对设备的地址，随机地址类型为1,固定地址类型为0
 * @param [in] protocol 协议类型
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bt_pair_device(const ux_api_bt_addr_t *addr, ux_api_bt_protocol_t protocol);

/**
 * @brief 连接蓝牙协议栈及扩展的链路(在有配对信息的前提下使用)
 * @param [in] profile 链路类型, 参考 @ref ux_api_bt_profile_t
 * @param [in] addr 待配对设备的地址，随机地址类型为1,固定地址类型为0
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bt_connect(ux_api_bt_profile_t profile, const ux_api_bt_addr_t *addr);

/**
 * @brief 删除配对
 * @param [in] addr 待配对设备的地址，随机地址类型为1,固定地址类型为0
 * @param [in] protocol 协议类型
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bt_remove_device(const ux_api_bt_addr_t *addr, ux_api_bt_protocol_t protocol);

/**
 * @brief 断连蓝牙协议栈及扩展的链路(在有配对信息的前提下使用)
 * @param [in] profile 链路类型, 参考 @ref ux_api_bt_profile_t
 * @param [in] addr 待配对设备的地址，随机地址类型为1,固定地址类型为0
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bt_disconnect(ux_api_bt_profile_t profile, const ux_api_bt_addr_t *addr);

/**
 * @brief 获取蓝牙协议栈及扩展协议的链路状态
 * @param [in] profile 链路类型, 参考 @ref ux_api_bt_profile_t
 * @param [in] addr 待配对设备的地址，随机地址类型为1,固定地址类型为0
 * @param [out] profile_state 链路状态，参考 @ref ux_api_bt_profile_state_t
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bt_get_profile_state(ux_api_bt_profile_t profile,
    const ux_api_bt_addr_t *addr, ux_api_bt_profile_state_t *profile_state);

/**
 * @brief ux_bt配置蓝牙参数
 * @param [in] config 配置项, 参考 @ref ux_api_bt_config_t
 * @param [in] param 配置参数, 参考 @ref ux_api_bt_config_t
 * @param [in] len 配置参数长度, 参考 @ref ux_api_bt_config_t
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bt_config(ux_api_bt_config_t config, void *param, uint16_t len);

/**
 * @brief ux_bt获取蓝牙名称
 * @param [out] name 用于存储获取到名称的缓冲区
 * @param [in] len 缓冲区长度
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bt_get_name(uint8_t *name, uint16_t len);

/**
 * @brief 通过spp服务发送数据
 * @param [in] dev_id 用于指定数据接收方的设备
 * @param [in] data 用于存储发送数据的缓冲区
 * @param [in] data_len 缓冲区长度
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_spp_service_send_data(uint16_t dev_id, uint8_t *data, uint16_t data_len);

/**
 * @brief 注册处理接收到的spp数据
 * @param [in] dev_id 用于指定数据发送方的设备
 * @param [in] handler 用于处理数据的注册函数
 */
void ux_api_spp_service_register_msg_handler(uint16_t dev_id, ux_api_spp_msg_handler handler);


/**
 * @brief 获取与远端蓝牙设备建链的蓝牙地址
 * @param [out] out_addr 用于存储获取到蓝牙地址的缓冲区
 * @param [in] addr_len 设备地址长度为 UX_API_BT_ADDR_LEN
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bt_get_device_connection_addr(uint8_t *out_addr, uint16_t addr_len);

/**
 * @brief 获取本地信息,包含名称、地址、使能状态等
 * @param [in] protocol 协议类型
 * @return ux_api_bt_local_info_t* 本地信息结构体指针
 */
const ux_api_bt_local_info_t* ux_api_bt_get_local_info(ux_api_bt_protocol_t protocol);

/**
 * @brief 获取远端设备信息, 按设备信息类型查找, 不需要地址
 * @param [in] role 设备角色
 * @param [in] protocol 协议类型
 * @param [out] device_num 设备个数
 * @return ux_api_bt_device_info_t* 指向个数为device_num的设备信息数组首地址
 */
const ux_api_bt_device_info_t* ux_api_bt_get_device_info(ux_api_bt_role_t role, ux_api_bt_protocol_t protocol,
    uint16_t *device_num);

/**
 * @brief 按地址获取远端设备信息
 * @param [in] addr 地址
 * @param [in] protocol 协议类型
 * @return ux_api_bt_device_info_t* 设备信息结构体指针
 */
const ux_api_bt_device_info_t* ux_api_bt_get_device_info_by_addr(ux_api_bt_addr_t *addr, ux_api_bt_protocol_t protocol);

/**
 * @brief 按地址获取远端设备信息
 * @param [in] device_id 设备编号
 * @param [out] device_info 设备信息结构体指针
 * @return errcode_t UX_ERR_SUCCESS 成功, Other 失败，参考 @ref ux_errcode.h
 */
errcode_t ux_api_bt_get_device_info_by_id(uint8_t device_id, ux_api_bt_device_info_t *device_info);

/**
 * @brief 开关多设备特性
 * @param [in] enable true开启, false关闭
 * @return void
 */
void ux_api_bt_switch_multiple_devices(bool enable);

/**
 * @brief 查询多设备特性是否开启
 * @param [in] void
 * @return bool true开启, false关闭
 */
bool ux_api_bt_is_multiple_device_open(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif