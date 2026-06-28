/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * File          ux_device_manager.h
 * Description:  provide ux device_mgr header file.
 */
#ifndef UX_DEVICE_MANAGER_H
#define UX_DEVICE_MANAGER_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "ux_audio_config.h"
#include "ux_bg_config.h"
#include "ux_api_bs_connection.h"
#include "ux_common_define.h"
#include "ux_system_config.h"
#ifndef DEMO_WATCH
#include "ux_bs_generic.h"
#endif
#ifdef DEMO_WATCH
#include "bs_br_service_api.h"
#include "ux_api_bt.h"
#include "ux_types.h"
#include "bs_device_mgr.h"
#endif
 
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef DEMO_WATCH
#define UX_ROLE_MASTER_MAX_DEVICE 4
#define UX_ROLE_SLAVE_MAX_DEVICE 8
#define UX_MAX_REMOTE_DEVICE 8
#define UX_DEVICE_ID_DEFAULT 0xee
#define UX_DEVICE_ID_INVALID 0xff
#define UX_DEVICE_ID_LEN 1
#define UX_MAX_REMOTE_OBJ 1  // OBJ最大数
#define UX_INVALID_OBJ_ID 0xff

typedef enum {
    UX_DEVICE_ID_TX_START = 0,
    UX_DEVICE_ID_TX_1 = UX_DEVICE_ID_TX_START,
    UX_DEVICE_ID_TX_2,
    UX_DEVICE_ID_TX_3,
    UX_DEVICE_ID_TX_4,
    UX_DEVICE_ID_TX_5,
    UX_DEVICE_ID_TX_6,
    UX_DEVICE_ID_TX_7,
    UX_DEVICE_ID_TX_8,
    UX_DEVICE_ID_TX_END,

    UX_DEVICE_ID_RX_START = 0x10,
    UX_DEVICE_ID_RX_1 = UX_DEVICE_ID_RX_START,
    UX_DEVICE_ID_RX_2,
    UX_DEVICE_ID_RX_3,
    UX_DEVICE_ID_RX_4,
    UX_DEVICE_ID_RX_END,

    UX_DEVICE_ID_MAX,
} ux_device_id_t;

typedef struct {
    uint32_t battery;
} ux_power_table_t;

typedef struct {
    ux_bs_obj_t obj;           // 合作设备集标识
    uint32_t device_num;       // obj中设备的数量
    uint16_t *device_id_list;  // 设备集的设备id列表
} ux_sle_obj_info_t;

typedef struct device_node {
    uint16_t device_id;
    uint8_t bg_addr[UX_BG_ADDR_LEN];
    ux_bg_table_t bg;
    ux_audio_table_t audio;
    ux_power_table_t power;
    ux_system_settings_table_t system_settings;
    struct device_node *next;
} ux_device_mgr_node_t;

typedef struct {
    ux_device_mgr_node_t *head;
} ux_device_mgr_singleton_t;

/**
 * 初始化设备管理器
 * @return 错误码
 */
errcode_t ux_device_mgr_init(void);

/**
 * 去初始化设备管理器
 * @return 错误码
 */
errcode_t ux_device_mgr_deinit(void);

typedef struct {
    uint32_t last_used;
    ux_sle_obj_info_t obj_info;
    bool active;
} ux_device_obj_entry_t;

/**
* 获取当前激活的obj
* @param obj 合作设备集
* @return 错误码
*/
errcode_t ux_device_mgr_get_obj(ux_bs_obj_t *obj);
/**
* 找到当前obj的信息
* @param obj 合作设备集
* @return obj信息
*/
ux_device_obj_entry_t *find_obj(ux_bs_obj_t obj);
/**
* 获取device_id的obj分配状态
* @param device_id 指定设备的device_id
* @return obj信息
*/
ux_device_obj_entry_t *ux_sle_get_obj_alloc_by_device(uint16_t device_id);
/**
* 获取obj的状态信息
* @param obj 合作设备集
* @return 是否激活
*/
bool ux_device_mgr_get_obj_active_state(ux_bs_obj_t obj);
/**
* 获取device_id的激活状态信息
* @param device_id 设备号
* @return 是否激活
*/
bool ux_device_mgr_obj_is_active_by_devid(uint16_t device_id);

/**
* 根据地址设置合作设备集obj的状态
* @param addr 设备地址
* @return 是否设置成功
*/
bool ux_device_mgr_obj_is_active_by_addr(uint8_t *addr);

/**
* 更新obj的临时状态
* @param obj 合作设备集ID
* @param is_active 该设备集是否激活
* @return 错误码
*/
errcode_t ux_device_mgr_set_obj_active(ux_bs_obj_t obj, bool is_active);

/**
* 新增obj信息节点
* @param obj 合作设备集ID
* @param device_num 合作设备集中的设备数量
* @param device_id_list 合作设备集中的设备id数组
* @return 错误码
*/
errcode_t ux_device_mgr_add_obj(ux_bs_obj_t obj, uint32_t device_num, uint16_t *device_id_list);

/**
* 移除obj节点
 * @param obj 合作设备集ID
 * @return 错误码
*/
errcode_t ux_device_mgr_remove_obj(ux_bs_obj_t obj);

/**
* 更新obj节点
 * @param obj 合作设备集ID
 * @param device_num 合作设备集中的设备数量
 * @param device_id_list 合作设备集中的设备id数组
 * @return 错误码
*/
errcode_t ux_device_mgr_update_obj(ux_bs_obj_t obj, uint32_t device_num, uint16_t *device_id_list);

/**
 * 新增设备信息节点
 * @param device_id 指定设备的device_id
 * @return 错误码
 */
errcode_t ux_device_mgr_add(uint16_t device_id, uint8_t *bg_addr, uint32_t bg_addr_len);

/**
 * 删除设备信息节点
 * @param device_id 指定设备的device_id
 * @return 错误码
 */
errcode_t ux_device_mgr_delete(uint16_t device_id);

/**
 * 根据星闪地址删除设备信息节点
 * @param addr 星闪/蓝牙地址
 * @param addr_len 星闪/蓝牙地址长度
 * @return 错误码
 */
errcode_t ux_device_mgr_delete_by_addr(uint8_t *addr, uint16_t addr_len);

/**
 * 通过组网工作角色分配device_id
 * @param role 工作角色
 * @param device_id 指定设备的device_id
 * @return 错误码
 */
errcode_t ux_device_mgr_allocate_device_id(ux_device_role_t role, uint16_t *device_id);

/**
 * 通过组网工作角色回收device_id
 * @param role 工作角色
 * @param device_id 指定设备的device_id
 * @return 错误码
 */
errcode_t ux_device_mgr_recycle_device_id(ux_device_role_t role, uint16_t device_id);

/**
 * 通过组网工作角色激活device_id
 * @param role 工作角色
 * @param device_id 指定设备的device_id
 * @return 错误码
 */
errcode_t ux_device_mgr_active_device_id(ux_device_role_t role, uint16_t device_id);

/**
 * 通过bg_addr获取device_id
 * @param bg_addr 星闪/蓝牙地址
 * @param bg_addr_len 星闪/蓝牙地址长度
 * @param device_id 指定设备的device_id
 * @return 错误码
 */
errcode_t ux_device_mgr_get_id_by_addr(const uint8_t *bg_addr, uint32_t bg_addr_len, uint16_t *device_id);

/**
 * 通过device_id和conn_id获取addr，本接口不可在配对流程中使用，需在设备完成稳定配对之后才可使用
 * @param device_id 指定设备的device_id
 * @param conn_id 指定设备的device_id
 * @param bg_addr 星闪/蓝牙地址
 * @param bg_addr_len 星闪/蓝牙地址长度
 * @return 错误码
 */
errcode_t ux_device_mgr_get_addr_by_id(uint16_t device_id, uint16_t conn_id, uint8_t *bg_addr, uint32_t bg_addr_len);

/**
 * 更新指定bg_addr的device_id
 * @param bg_addr 星闪/蓝牙地址
 * @param bg_addr_len 星闪/蓝牙地址长度
 * @param device_id 更新的device_id
 * @return 错误码
 */
errcode_t ux_device_mgr_update_device_id_by_addr(const uint8_t *bg_addr, uint32_t bg_addr_len,
                                                 const uint16_t device_id);

/**
 * 从nv读取本机device_id
 * @param device_id 指定设备的device_id
 * @return 错误码
 */
errcode_t ux_device_mgr_get_local_device_id(uint16_t *device_id);

/**
 * 将本机device_id写入nv
 * @param device_id 指定设备的device_id
 * @return 错误码
 */
errcode_t ux_device_mgr_save_local_device_id(const uint16_t device_id);

/**
 * 获取已连接的对端设备device_id数组和数量
 * @param device_id_arr 已连接的device_id数组
 * @param device_num 已连接的device_id数量
 * @return 错误码
 */
errcode_t ux_device_mgr_get_remote_connected_device_id(uint16_t *device_id_arr, uint32_t *device_num);

/**
 * 获取已配对的对端设备device_id数组和数量
 * @param device_id_arr 已连接的device_id数组
 * @param device_num 已连接的device_id数量
 * @return 错误码
 */
errcode_t ux_device_mgr_get_remote_paired_device_id(uint16_t *device_id_arr, uint32_t *device_num);

/**
 * @brief 获取远程配对设备信息
 * @param conn_info_arr 连接信息数组，用于存储获取到的设备信息
 * @param expect_num 输入参数，表示要获取的设备数量
 * @param actual_num 输出参数，表示实际获取到的设备数量
 * @return 返回错误码，0表示成功，非0表示失败
 */
errcode_t ux_device_mgr_get_remote_paired_info(ux_api_bs_conn_info_t *conn_info_arr, uint8_t expect_num,
                                               uint8_t *actual_num);

/**
 * @brief 通过连接号查找指定设备的device id
 * @param conn_id 连接号
 * @param device_id 设备的device_id
 * @return 返回错误码，0表示成功，非0表示失败
 */
errcode_t ux_device_mgr_get_device_id_by_conn_id(const uint16_t conn_id, uint16_t *device_id);

errcode_t ux_device_mgr_2_physical_dev_id(uint16_t *device_id);
errcode_t ux_device_mgr_2_logic_dev_id(uint16_t *device_id);

errcode_t ux_device_mgr_get_valid_conn_id(uint16_t device_id, uint16_t *conn_id);
bool ux_device_mgr_check_logic_device_id_valid(uint16_t device_id);
// 获取指定设备的表指针
ux_bg_table_t *ux_device_mgr_get_bg_table(uint16_t device_id);
ux_bg_table_t *ux_device_mgr_get_bg_table_by_addr(uint8_t *addr, uint32_t addr_len);
ux_audio_table_t *ux_device_mgr_get_audio_table(uint16_t device_id);
ux_power_table_t *ux_device_mgr_get_power_table(uint16_t device_id);
ux_system_settings_table_t *ux_device_mgr_get_system_settings_table(uint16_t device_id);
#endif

#ifdef DEMO_WATCH
#define DEFAULT_DEVICE_ID 0xEE
#define UX_BG_MAX_DEVICE_NUM 20

typedef struct ux_bg_device_info_t {
    char name[BS_BR_DEVICE_NAME_MAX_LEN];    // 设备名
    ux_api_bt_addr_t addr;                   // 地址
    uint32_t device_class;                   // 设备类型
    uint16_t id;                             // 对应bs的device_id
    ux_api_bt_protocol_t protocol;           // 协议类型
    ux_api_bt_role_t role;                   // 中心设备或外围设备
    ux_api_bt_profile_state_t connect_state; // 连接状态,包含过渡状态,在回调中调用
    bool is_valid;                           // 当前设备信息是否有效
    int8_t rssi;
    bool enc_state;         // 加密状态
    struct ux_bg_device_info_t *next;
} ux_bg_device_info_t;

typedef struct {
    ux_bg_device_info_t *head;
    uint16_t device_num;
} ux_bg_device_list_t;

typedef bool (*ux_bg_device_operate_rule)(const ux_bg_device_info_t *node, const uint8_t *rule_param, uint16_t len);
// 链表操作获取设备信息
ux_bg_device_info_t *ux_bg_find_device_info_by_addr(ux_bg_device_list_t *device_list,
                                                    const ux_api_bt_addr_t *addr, ux_api_bt_protocol_t protocol);
ux_bg_device_info_t *ux_bg_find_device_info_by_id(ux_bg_device_list_t *device_list, uint16_t id);
ux_bg_device_list_t *ux_bg_get_scan_device_list(void);
ux_bg_device_list_t *ux_bg_get_paired_device_list(void);
ux_bg_device_info_t *ux_bg_append_blank_device_info(ux_bg_device_list_t *device_list);
void ux_bg_clear_device_list(ux_bg_device_list_t *device_list);
void ux_bg_delete_device_info_by_id(ux_bg_device_list_t *device_list, uint16_t device_id);
void ux_bg_delete_device_info_by_addr(ux_bg_device_list_t *device_list, const ux_api_bt_addr_t *addr,
    ux_api_bt_protocol_t protocol);
void ux_bg_delete_device_info_by_protocol(ux_bg_device_list_t *device_list, const ux_api_bt_protocol_t protocol);
ux_bg_device_info_t *ux_bg_get_device_info(ux_bg_device_list_t *device_list,
    const ux_bg_device_operate_rule is_target_device, const uint8_t *rule_param, uint16_t len);
void ux_bg_set_scan_device_count(uint16_t count);
uint16_t ux_bg_get_scan_device_count(void);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */
 
#endif