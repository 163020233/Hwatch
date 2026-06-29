/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * File          ux_bg_config.h
 * Description:  provide ux bg config header file.
 */
#ifndef UX_BG_CONFIG_H
#define UX_BG_CONFIG_H

#include "errcode.h"
#include "stddef.h"
#include "stdbool.h"
#include "ux_common_define.h"
#include "ux_errcode.h"

#ifdef DEMO_WATCH
#include "bs_br_service_api.h"
#include "ux_types.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define BG_ADDR_INDEX_0 0
#define BG_ADDR_INDEX_1 1
#define BG_ADDR_INDEX_2 2
#define BG_ADDR_INDEX_3 3
#define BG_ADDR_INDEX_4 4
#define BG_ADDR_INDEX_5 5

typedef enum {
    UX_WORK_PROTOCOL_BT,        /* 支持蓝牙协议 */
    UX_WORK_PROTOCOL_SLE,       /* 支持星闪协议 */
    UX_WORK_PROTOCOL_COMBO,     /* 支持星闪和蓝牙协议协同 */
    UX_WORK_PROTOCOL_BR,        /* 支持经典蓝牙 */
    UX_WORK_PROTOCOL_BLE,       /* 支持BLE协议 */
    UX_WORK_PROTOCOL_MAX,
} ux_work_protocol_t;

#ifndef DEMO_WATCH
#define UX_BG_MAX_REMOTE_DEVICE 8
#if defined(MELODY_DEMO_DONGLE)
#define UX_BG_DONGLE_MAX_PDL_NUM 2
#elif defined(MELODY_DEMO_SOUNDBAR)
#define UX_BG_DONGLE_MAX_PDL_NUM 1
#endif
#ifdef MELODY_DEMO_BMIC_TX
#define UX_BG_MAX_CONNECTION_NUM 3
#else
#define UX_BG_MAX_CONNECTION_NUM 3
#endif
#define UX_BG_INVALID_CONN_ID 0xffff

typedef enum {
    UX_DEVICE_ROLE_MASTER,
    UX_DEVICE_ROLE_SLAVE,

    UX_DEVICE_ROLE_MAX,
} ux_device_role_t;

#ifdef CONFIG_SUPPORT_BS_SLE_L3
typedef enum {
    UX_PAIR_TOPO_STATE_NONE,
    UX_PAIR_TOPO_STATE_SINGLE,
    UX_PAIR_TOPO_STATE_DOUBLE,

    UX_PAIR_TOPO_STATE_MAX,
} ux_pair_topology_state_t;  // 区分是单耳连接还是双耳连接

typedef struct {
    uint8_t len;   /*!< @if Eng device name length.
                        @else  名称长度，包括结束符\0。 @endif */
    uint8_t *name; /*!< @if Eng device name.
                        @else  设备名称。 @endif */
} ux_sle_device_name_t;

typedef enum {
    UX_PAIR_MODE_PAIRING,    // 配对模式
    UX_PAIR_MODE_RECONNECT,  // 回连模式

    UX_PAIR_MODE_MAX,
} ux_pair_mode_t;
#endif

typedef enum {
    UX_PAIR_STATE_UNPAIRED,
    UX_PAIR_STATE_PAIRED,
    UX_PAIR_STATE_PAIRING,

    UX_PAIR_STATE_MAX,
} ux_pair_state_t;

typedef enum {
    UX_CONNECT_STATE_DISCONNECTED,
    UX_CONNECT_STATE_CONNECTING,
    UX_CONNECT_STATE_CONNECTED,

    UX_CONNECT_STATE_MAX,
} ux_conn_state_t;

typedef struct {
    ux_device_role_t role;
#ifdef CONFIG_SUPPORT_BS_SLE_L3
    ux_pair_topology_state_t topo_state;
    ux_sle_device_name_t *device_name;
#endif
    ux_pair_state_t pair_state;
    ux_conn_state_t conn_state;
    uint16_t nv_id;
    uint16_t conn_id;
} ux_bg_table_t;

typedef struct {
    uint8_t addr[UX_BG_ADDR_LEN];
    uint16_t device_id;
    ux_conn_state_t conn_state;
} ux_bg_conn_info_t;

/**
 * 从整机状态管理器中获取指定device的工作角色
 * @param device_id 指定设备的device_id
 * @param role 工作角色
 * @return 错误码
 */
errcode_t ux_bs_table_get_role(uint16_t device_id, ux_device_role_t *role);

/**
 * 向整机状态管理器中设置指定device的工作角色
 * @param device_id 指定设备的device_id
 * @param role 工作角色
 * @return 错误码
 */
errcode_t ux_bs_table_set_role(uint16_t device_id, ux_device_role_t role);

/**
 * 向整机状态管理器中设置指定device的连接号
 * @param addr 地址
 * @param addr_len 地址长度
 * @param role 工作角色
 * @return 错误码
 */
errcode_t ux_bs_table_set_role_by_addr(uint8_t *addr, uint32_t addr_len, ux_device_role_t role);

/**
 * 从整机状态管理器中获取指定device的配对状态
 * @param device_id 指定设备的device_id
 * @param pair_state 配对状态
 * @return 错误码
 */
errcode_t ux_bs_table_get_pair_state(uint16_t device_id, ux_pair_state_t *pair_state);

/**
 * 向整机状态管理器中设置指定device的配对状态
 * @param device_id 指定设备的device_id
 * @param pair_state 配对状态
 * @return 错误码
 */
errcode_t ux_bs_table_set_pair_state(uint16_t device_id, ux_pair_state_t pair_state);

/**
 * 向整机状态管理器中设置指定device的连接号
 * @param addr 地址
 * @param addr_len 地址长度
 * @param pair_state 配对状态
 * @return 错误码
 */
errcode_t ux_bs_table_set_pair_state_by_addr(uint8_t *addr, uint32_t addr_len, ux_pair_state_t pair_state);

/**
 * 从整机状态管理器中获取指定device的连接状态
 * @param device_id 指定设备的device_id
 * @param conn_state 连接状态
 * @return 错误码
 */
errcode_t ux_bs_table_get_conn_state(uint16_t device_id, ux_conn_state_t *conn_state);

/**
 * 向整机状态管理器中设置指定device的连接状态
 * @param device_id 指定设备的device_id
 * @param conn_state 连接状态
 * @return 错误码
 */
errcode_t ux_bs_table_set_conn_state(uint16_t device_id, ux_conn_state_t conn_state);

/**
 * 根据星闪地址从整机状态管理器中查询指定device的连接状态
 * @param addr 地址
 * @param addr_len 地址长度
 * @param conn_state 连接状态
 * @return 错误码
 */
errcode_t ux_bs_table_get_conn_state_by_addr(uint8_t *addr, uint32_t addr_len, ux_conn_state_t *conn_state);

/**
 * 根据星闪地址向整机状态管理器中设置指定device的连接状态
 * @param addr 地址
 * @param addr_len 地址长度
 * @param conn_state 连接状态
 * @return 错误码
 */
errcode_t ux_bs_table_set_conn_state_by_addr(uint8_t *addr, uint32_t addr_len, ux_conn_state_t conn_state);

/**
 * 从整机状态管理器中获取指定device的连接号
 * @param device_id 指定设备的device_id
 * @param conn_id 连接号
 * @return 错误码
 */
errcode_t ux_bs_table_get_conn_id(uint16_t device_id, uint16_t *conn_id);

/**
 * 向整机状态管理器中设置指定device的连接号
 * @param device_id 指定设备的device_id
 * @param conn_id 连接号
 * @return 错误码
 */
errcode_t ux_bs_table_set_conn_id(uint16_t device_id, uint16_t conn_id);

/**
 * 从整机状态管理器中获取指定device的连接号
 * @param addr 地址
 * @param addr_len 地址长度
 * @param conn_id 连接号
 * @return 错误码
 */
errcode_t ux_bs_table_get_conn_id_by_addr(uint8_t *addr, uint32_t addr_len, uint16_t *conn_id);

/**
 * 向整机状态管理器中设置指定device的连接号
 * @param addr 地址
 * @param addr_len 地址长度
 * @param conn_id 连接号
 * @return 错误码
 */
errcode_t ux_bs_table_set_conn_id_by_addr(uint8_t *addr, uint32_t addr_len, uint16_t conn_id);

/**
 * 从整机状态管理器中获取指定device的nv id
 * @param device_id 指定设备的device_id
 * @param nv_id nv key
 * @return 错误码
 */
errcode_t ux_bs_table_get_nv_id(uint16_t device_id, uint16_t *nv_id);

/**
 * 从整机状态管理器中获取指定device的nv id
 * @param addr 地址
 * @param addr_len 地址长度
 * @param nv_id nv key
 * @return 错误码
 */
errcode_t ux_bs_table_get_nv_id_by_addr(const uint8_t *addr, uint32_t addr_len, uint16_t *nv_id);

/**
 * 向整机状态管理器中设置指定device的nv id
 * @param device_id 指定设备的device_id
 * @param nv_id nv key
 * @return 错误码
 */
errcode_t ux_bs_table_set_nv_id(uint16_t device_id, uint16_t nv_id);

/**
 * 向整机状态管理器中设置指定device的nv id
 * @param addr 地址
 * @param addr_len 地址长度
 * @param nv_id nv key
 * @return 错误码
 */
errcode_t ux_bs_table_set_nv_id_by_addr(uint8_t *addr, uint32_t addr_len, uint16_t nv_id);

/**
 * 设置最新连接的对端设备的device_id
 * @param device_id device_id
 * @return 错误码
 */
errcode_t ux_bs_set_latest_connected_device_id(const uint16_t device_id);

/**
 * 获取最新连接的对端设备的device_id
 * @param device_id device_id
 * @return 错误码
 */
errcode_t ux_bs_get_latest_connected_device_id(uint16_t *device_id);

/**
 * 设置最新连接的对端设备的conn_id
 * @param conn_id conn_id
 * @return 错误码
 */
errcode_t ux_bs_set_latest_conn_id(const uint16_t conn_id);

/**
 * 获取最新连接的对端设备的conn_id
 * @param conn_id conn_id
 * @return 错误码
 */
errcode_t ux_bs_get_latest_conn_id(uint16_t *conn_id);

/**
 * 获取配对状态
 * @param pair_state 配对状态
 * @return 错误代码
 */
errcode_t ux_bs_set_local_pair_state(ux_pair_state_t pair_state);

/**
 * 获取配对状态
 * @param pair_state 配对状态指针
 * @return 错误代码
 */
errcode_t ux_bs_get_local_pair_state(ux_pair_state_t *pair_state);

/**
 * 获取配对状态
 * @param connect_state 连接状态
 * @return 错误代码
 */
errcode_t ux_bs_set_local_connect_state(ux_conn_state_t connect_state);

/**
 * 获取配对状态
 * @param connect_state 连接状态指针
 * @return 错误代码
 */
errcode_t ux_bs_get_local_connect_state(ux_conn_state_t *connect_state);

#endif

#ifdef DEMO_WATCH
/* 星闪蓝牙地址在系统中的存储 */
errcode_t ux_bg_load_addr(uint16_t key, uint8_t *data, uint16_t max_len);
errcode_t ux_bs_set_local_addr(const ux_work_protocol_t protocol, uint8_t *addr, uint16_t len);
errcode_t ux_bg_str_addr_to_dec(const char *str_addr, uint16_t str_len, ux_bd_addr little_endian_addr,
                                ux_bd_addr big_endian_addr);
errcode_t ux_bt_set_name(uint8_t *name, uint16_t len);
errcode_t ux_bt_set_addr(uint8_t *addr, uint16_t len);
bool ux_bt_ctkd_is_open(void);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif