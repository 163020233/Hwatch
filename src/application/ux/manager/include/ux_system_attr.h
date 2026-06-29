/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Define earphone instance attribute \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-03-08， Create file. \n
 */
#ifndef UX_SYSTEM_ATTR_H
#define UX_SYSTEM_ATTR_H
#include <stdint.h>
#include <stdbool.h>
#include "ux_types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define DEV_QUERY_TIME_DATA_LEN 20
#define SDK_VERSION_STRING  "1.1.25.09" /* SDK内部版本号，禁止修改 */

typedef struct {
    uint8_t position;                           /* 耳机位置信息 0: 在盒, 1: 出盒 */
    uint8_t battery_percent;                    /* 耳机电量信息，范围是1~100 */
    uint8_t tws_role;                           /* 记录主副耳信息 */
    uint8_t tws_ear_side;                       /* 记录左右耳信息 */
    uint8_t tws_pair_state;                     /* 记录TWS对耳配对状态 */
    uint8_t charge_state;                       /* 充电状态 0: 未充电, 1: 充电中 */
    uint8_t dev_time[DEV_QUERY_TIME_DATA_LEN];  /* 记录时间 */
} ux_device_info_t;

typedef struct {
    uint8_t ux_bg_state;                        /* 记录bt工作的状态 */
    uint8_t ux_haid_state;                      /* 记录haid工作的状态 */
    uint8_t ux_manager_state;                   /* 记录整机工作的状态 */
    uint8_t ux_manager_pre_state;               /* 记录整机工作的上一个状态 */
} ux_work_state_t;

typedef struct {
    uint8_t ux_power_on_mode;                   /* 记录开机方式 */
    uint8_t ux_power_off_mode;                  /* 记录关机方式，包括按键关机，入盒关机等 */
} ux_power_mode_t;

typedef struct {
    uint8_t button_state;                       /* 记录button状态 */
    uint8_t touchpad_state;                     /* 记录touchpad 状态 */
} ux_ext_device_state_t;

typedef struct {
    uint8_t recover_type;                      /* 记录恢复出场策略 */
} ux_recover_mode_t;

typedef struct {
    uint32_t device_reconn_duration;            /* 回连持续时间 */
    uint8_t peer_pair_state;                    /* 对耳连接状态 */
    uint8_t peer_pair_nums;                     /* 记录对耳配对扫描次数 */
    uint8_t peer_bt_pairing_state;              /* 记录对耳蓝牙是否处于配对中或回连中 */
    uint8_t peer_sle_pairing_state;             /* 记录对耳星闪是否处于配对中或回连中 */
    uint8_t peer_combo_pairing_state;           /* 记录对耳双模是否处于配对中或回连中 */
    uint8_t peer_spp_state;                     /* 对耳spp连接状态 */
    uint8_t device_pair_state;                  /* 设备配对状态 */
    uint8_t bt_device_pair_state;               /* 蓝牙设备配对状态 */
    uint8_t sle_device_pair_state;              /* 星闪设备配对状态 */
    uint8_t slave_trigger_dev_pair_state;       /* 按键触发配对状态 */
    uint8_t peer_device_pair_state;             /* 对耳的设备连接状态 */
    uint8_t peer_device_connect_nums;           /* 对耳的设备连接数量 */
    uint8_t pairing_failed_nums;                /* 记录设备配对失败次数 */
    uint8_t device_disconnect_reason;           /* 记录设备断链原因 */
    uint8_t device_reconn_phase;                /* 记录设备回连阶段 */
    uint8_t device_reconn_type;                 /* 记录设备回连类型 */
    uint8_t peer_device_reconn_type;            /* 对耳的设备回连类型 */
    uint8_t bt_pairing_state;                   /* 蓝牙业务是否进入配对状态 */
    uint8_t sle_pairing_state;                  /* 星闪业务是否进入配对状态 */
    uint8_t ble_pairing_state;                  /* BLE业务是否进入配对状态 */
    uint8_t combo_pairing_state;                /* 双模业务是否进入配对状态 */
    uint8_t connected_protocol_type;            /* 记录最近一次和耳机建连的协议类型 */
    uint8_t peer_connected_protocol_type;       /* 记录对耳最近一次和耳机建连的协议类型 */
} ux_pair_state_info_t;

typedef struct {
    uint8_t tone_state;                        /* 记录提示音状态 */
    uint8_t led_state;                         /* 记录灯语状态 */
} ux_notifica_state_t;

typedef struct {
    uint8_t call_state;                         /* 音频状态 */
    uint8_t haid_state;                         /* 助听状态 */
    uint8_t haid_enable_state;                  /* 助听启用状态 */
    uint8_t wearing_status;                     /* sensor 上报入耳状态 */
    uint8_t lowbattery_state;                   /* 低电量状态 */
    uint8_t ble_conn_state;                     /* ble 连接状态 */
    uint8_t call_sound_state;                   /* 通话中声道的状态 */
    uint8_t bt_standby_state;                   /* 蓝牙待机状态 */
    uint8_t storage_power_state;                /* 记录开关机最终状态，0：无效，保持当前状态，1:开机，2:关机 */
    uint8_t ota_state;                          /* 记录OTA状态 */
    uint8_t pre_fsm_state;                      /* 记录前次整机模式 */
    bool mute_state;                            /* 记录当前mute状态 */
    uint8_t bt_power_off_mode;                  /* 记录蓝牙关机模式 */
    uint8_t recorder_state;                     /* 记录录音状态 */
    uint8_t autorate_mode;                      /* 记录autorate开关模式 */
    bool power_on_haid_delay_state;             /* 开机延时进助听状态 */
    bool wear_in_haid_delay_state;              /* 入耳延时进助听状态 */
    bool bt_init_state;                         /* 记录bt是否进行过初始化 */
    bool acc_on_state;                          /* 记录上电状态 */
    bool device_reconn_state;                   /* 记录主动回连状态 */
    bool switch_pairing_state;                  /* 记录是否主动切换配对状态 */
    int32_t db_val;                             /* 记录当前db值 */
    ux_power_mode_t power_mode;                 /* 记录开关机模式 */
    ux_work_state_t work_state;                 /* 记录工作状态 */
    ux_ext_device_state_t ext_device_state;     /* 记录button 和 touchpad 状态 */
    ux_notifica_state_t notifica_state;         /* 记录提醒功能状态,包括提示音,灯语 */
} ux_system_state_t;

typedef struct {
    uint32_t audio_time;
    uint32_t calling_time;
    uint32_t config_time;
} ux_service_time_count_t;

typedef struct {
    uint8_t long_button;
    uint8_t short_button;
} ux_button_config_t;

typedef struct {
    uint8_t audio_aid_mix;                      /* 混音模式 */
    uint8_t touch_config;                       /* touchpad 是否生效 */
    uint8_t wear_control_status;                /* 佩戴检测模式 */
    uint8_t wear_detection_switch;              /* 佩戴检测开关 */
    uint8_t low_latency_switch;                 /* 音频低时延开关 */
    uint8_t dual_connection_switch;             /* 双设备连接开关 */
    uint8_t tone_enable_status;                 /* 提示音使能或去使能标志位 */
    uint16_t delay_wear_sensor_aidmode;         /* 佩戴检测打开的情况下入耳延时开启助听时间 */
    uint16_t delay_wear_in_aidmode;             /* 强制入耳的情况下入耳延时开启助听时间 */
    uint16_t delay_open_aidmode;                /* 开机延时时间 */
    uint16_t time_switch_aidmode;               /* 音频切助听延时时间 */
    uint16_t scene_mode_status;                 /* 记录当前助听场景模式有效性 */
    ux_button_config_t button_config;           /* button控制，包括长压，短压控制 */
    ux_service_time_count_t service_time;       /* 服务事件统计 */
    uint8_t play_mode;                          /* 记录音频源播放模式 */
} ux_device_nv_content_t;

typedef struct {
    uint8_t battery_percent;                    /* 盒子电量信息，范围是1~100 */
    uint8_t cover_state;                        /* 记录盒盖状态 */
    uint8_t charge_state;                       /* 记录盒盖状态 */
} ux_system_box_info_t;

typedef struct {
    ux_device_info_t local_info;                /* 记录本端设备信息 */
    ux_device_info_t peer_info;                 /* 记录对端设备信息 */
    ux_system_state_t state;                    /* 记录设备状态 */
    ux_device_nv_content_t nv_content;          /* 记录NV值 */
    ux_system_box_info_t box_info;              /* 记录盒子信息 */
    ux_pair_state_info_t pair_state;            /* 记录配对信息 */
    ux_recover_mode_t recover_info;             /* 记录恢复出厂设置信息 */
    bool connect_dongle;                        /* 记录是否连接Dongle */
} ux_system_info_t;

typedef struct {
    uint16_t id;
    ux_bd_addr addr;
    uint8_t bat_level;
    uint8_t rec_bit;
    uint8_t conn_state;
    uint8_t denoise_state;
    uint8_t eq_mode;
    uint8_t rec_state;
    char* sdk_version;
    uint16_t sample_rate;
    uint16_t bit_width;
    uint32_t file_duration;
    uint32_t file_size;
    bool mute_state;
    uint32_t remain_time;
    int32_t db_val;
    bool low_cut_state;
} ux_terminal_node_info_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
