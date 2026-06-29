/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: set_dev_handle.c header file
 * Author: AuthorNameMagicTag
 * Create: 2025-10-13
 */

#ifndef SET_DEV_HANDLE_H
#define SET_DEV_HANDLE_H

#include "diag_service.h"
#include "conn_tlv_err_code.h"
#include "conn_tlv_api.h"
#include "ux_event_define.h"

/* 协议5.7.1 恢复出厂设置类型定义 */
typedef enum {
    SET_DEV_RESTORE_START = 0,
    SET_DEV_RESTORE_FACTORY,

    SET_DEV_RESTORE_END
} set_dev_restore_factory_type_t;

/* 协议5.7.2 耳机入耳状态设置类型定义 */
typedef enum {
    SET_DEV_WEAR_STATUS_START = 0,
    SET_DEV_WEAR_STATUS,

    SET_DEV_WEAR_STATUS_END
} set_dev_wear_status_type_t;

/* 协议5.7.3 设备音量及效果设置类型定义 */
typedef enum {
    SET_DEV_VOLUME_START = 0,
    SET_DEV_HEARINGAID_VOLUME,
    SET_DEV_MUSIC_VOLUME,

    SET_DEV_VOLUME_END
} set_dev_volume_type_t;

/* 协议5.7.14 设备重启设置类型定义 */
typedef enum {
    SET_DEV_RESET_START = 0,
    SET_DEV_RESET,

    SET_DEV_RESET_END
} set_dev_reset_type_t;

/* 协议5.7.15 设备蓝牙名称设置类型定义 */
typedef enum {
    SET_DEV_NAME_START = 0,
    SET_DEV_BT_NAME,
    SET_DEV_BLE_NAME,

    SET_DEV_NAME_END
} set_dev_name_type_t;

/* 协议5.7.18 自定义章节，模拟异步流程 */
typedef enum {
    SET_DEV_ASYNC_TEST_START = 0,
    SET_DEV_ASYNC_TEST,

    SET_DEV_ASYNC_TEST_END
} set_dev_async_test_type_t;

/* 协议5.7.19 自定义章节，设备关机设置类型定义 */
typedef enum {
    SET_DEV_POWEROFF_START = 0,
    SET_DEV_POWEROFF,

    SET_DEV_POWEROFF_END
} set_dev_poweroff_type_t;

/* 协议5.7.20 自定义章节，ANC主动降噪产测类型定义 */

typedef struct {
    uint32_t gain;
}anc_gain_param_t;

typedef enum {
    SET_DEV_ANCDUT_START = 0,
    SET_DEV_ANCDUT_CONNECT,
    // CONNECT 包含下列三个步骤
    SET_DEV_ANCDUT_INIT,
    SET_DEV_ANCDUT_OPEN,
    SET_DEV_ANCDUT_SET_INITIAL_PARAM,

    SET_DEV_ANCDUT_OFF,

    SET_DEV_ANCDUT_BEGIN_FB,
    SET_DEV_ANCDUT_SET_FB_GAIN,
    SET_DEV_ANCDUT_BEGIN_FF0,
    SET_DEV_ANCDUT_SET_FF0_GAIN,
    SET_DEV_ANCDUT_BURN_GAIN,

    SET_DEV_ANCDUT_BEGIN_FB_FF0,

    SET_DEV_ANCDUT_CLOSE,
    SET_DEV_ANCDUT_DEINIT,

    SET_DEV_ANCDUT_END
} set_dev_ancdut_type_t;


typedef enum {
    ANC_DUT_STATUS_TYPE_INVALID,                        // 未开始
    ANC_DUT_STATUS_TYPE_INITED,                         // 已初始化
    ANC_DUT_STATUS_TYPE_OPENED,                         // 已开启
    ANC_DUT_STATUS_TYPE_OFF,                            // 已关闭
    ANC_DUT_STATUS_TYPE_PASSIVE_TEST_BEGIN,             // 被动测试开始
    ANC_DUT_STATUS_TYPE_PASSIVE_TEST_PASSED,            // 被动测试通过

    ANC_DUT_STATUS_TYPE_CLOSED,                         // 已关闭
    ANC_DUT_STATUS_TYPE_MAX
} anc_status_t;

/* 协议5.7.21 自定义章节，关闭3A算法设置类型定义 */
typedef enum {
    SET_DEV_AUDIOSTRAM_START = 0,
    SET_DEV_SEA_CLOSE,
    SET_DEV_STRAM_STATE,
 
    SET_DEV_AUDIOSTRAM_END,
} set_dev_audio_stream_type_t;
 
/* 协议5.7.22 自定义章节，mic类型设置类型定义 */
typedef enum {
    SET_DEV_MICTYPE_START = 0,
    SET_DEV_MICTYPE,
 
    SET_DEV_MICTYPE_END,
} set_dev_mictype_type_t;
 
/* 协议5.7.23 自定义章节，媒体EQ设置类型定义 */
typedef enum {
    SET_DEV_AUDIO_EQ_START = 0,
    SET_DEV_AUDIO_EQ,
 
    SET_DEV_AUDIO_EQ_END,
} set_dev_audio_eq_type_t;
 
/* mic类型枚举 */
typedef enum {
    UX_MIC_AMIC_0 = 0x1,
    UX_MIC_AMIC_1 = 0x2,
    UX_MIC_AMIC_2 = 0x4,
    UX_MIC_TYPE_MAX,
} ux_mic_type_t;
 
typedef enum {
    UX_CAPTURE_MIC_TYPE_0,
    UX_CAPTURE_MIC_TYPE_1,
    UX_CAPTURE_MIC_TYPE_MAX,
} capturera_mic_type_t;

typedef enum {
    CONFIG_SEA_CLOSED = 1,
    CONFIG_SEA_OPEN = 2,
} set_sea_config_type_t;

typedef enum {
    AUDIO_STREAM_CREATE = 1,
    AUDIO_STREAM_DESTORY = 2,
} set_sudio_stream_type_t;

typedef struct {
    uint8_t anc_mode;
}anc_initial_param_t;

typedef struct {
    uint32_t fb_gain;
}anc_fb_gain_param_t;

typedef struct {
    uint32_t ff_gain;
}anc_ff_gain_param_t;

// #ifdef CONFIG_SUPPORT_ANC
typedef struct {
    anc_status_t            status;                     // 状态机控制
    bool                    anc_inited;                 // 初始化控制，仅初始化一次
    bool                    anc_opened;                 // open控制，仅open一次
    uint16_t                passive_test_loop_cnt;      // 被动测试循环次数
    uint16_t                fb_test_loop_cnt;           // fb测试循环次数
    uint16_t                fb_ff_test_loop_cnt;        // 混合测试循环次数
    uint32_t                fb_gain;                    // uapi_anc_core_coef_rw_param.bq_fb.gain;
    uint32_t                ff0_gain;                   // uapi_anc_core_coef_rw_param.bq_ff0.gain;
    uint32_t                ff1_gain;                   // uapi_anc_core_coef_rw_param.bq_ff1.gain;
} anc_ctrl_t;

anc_ctrl_t *anc_get_ctrl(void);
// #endif /* __CONFIG_SUPPORT_ANC__ */

typedef uint16_t (*set_dev_proc_fun_cb)(uint8_t *payload, uint16_t len, uint8_t type);

set_dev_proc_fun_cb *get_set_dev_restore_factory_map(void);
set_dev_proc_fun_cb *get_set_dev_wear_status_map(void);
set_dev_proc_fun_cb *get_set_dev_volume_map(void);
set_dev_proc_fun_cb *get_set_dev_reset_map(void);
set_dev_proc_fun_cb *get_set_dev_name_map(void);
set_dev_proc_fun_cb *get_set_async_test_map(void);
set_dev_proc_fun_cb *get_set_dev_recovery_map(void);
set_dev_proc_fun_cb *get_set_dev_poweroff_map(void);
set_dev_proc_fun_cb *get_set_dev_ancdut_map(void);
set_dev_proc_fun_cb *get_set_dev_audio_stream_map(void);
set_dev_proc_fun_cb *get_set_dev_mictype_map(void);
set_dev_proc_fun_cb *get_set_dev_audio_eq_map(void);

/* 5.7 */
uint16_t set_dev_set_wearing_status(uint8_t *payload, uint16_t len, uint8_t type);
uint16_t set_dev_set_music_volume(uint8_t *payload, uint16_t len, uint8_t type);
uint16_t set_dev_set_reset(uint8_t *payload, uint16_t len, uint8_t type);
uint16_t set_dev_set_ble_name(uint8_t *payload, uint16_t len, uint8_t type);
uint16_t set_dev_set_async_test(uint8_t *payload, uint16_t len, uint8_t type);
uint16_t set_dev_recovery_partial(uint8_t *payload, uint16_t len, uint8_t type);
uint16_t set_dev_power_off(uint8_t *payload, uint16_t len, uint8_t type);
uint16_t set_dev_enable_sea_close(uint8_t *payload, uint16_t len, uint8_t type);
uint16_t set_dev_set_mic_type(uint8_t *payload, uint16_t len, uint8_t type);
uint16_t set_dev_set_audio_eq(uint8_t *payload, uint16_t len, uint8_t type);
uint16_t set_dev_haid_vol(uint8_t *payload, uint16_t len, uint8_t type);

/* 5.7.20 ANC产测 */
uint16_t set_dev_ancdut_connect(uint8_t *payload, uint16_t len, uint8_t type);
uint16_t set_dev_ancdut_init(uint8_t *payload, uint16_t len, uint8_t type);
uint16_t set_dev_ancdut_open_default(uint8_t *payload, uint16_t len, uint8_t type);
uint16_t set_dev_ancdut_set_initial_param_by_flash(uint8_t *payload, uint16_t len, uint8_t type);
uint16_t set_dev_ancdut_off(uint8_t *payload, uint16_t len, uint8_t type);
uint16_t set_dev_ancdut_begin_fb(uint8_t *payload, uint16_t len, uint8_t type);
uint16_t set_dev_ancdut_set_fb_gain(uint8_t *payload, uint16_t len, uint8_t type);
uint16_t set_dev_ancdut_begin_ff0(uint8_t *payload, uint16_t len, uint8_t type);
uint16_t set_dev_ancdut_set_ff0_gain(uint8_t *payload, uint16_t len, uint8_t type);
uint16_t set_dev_ancdut_begin_fb_ff0(uint8_t *payload, uint16_t len, uint8_t type);

uint16_t set_dev_ancdut_burn_gain(uint8_t *payload, uint16_t len, uint8_t type);
uint16_t set_dev_ancdut_close(uint8_t *payload, uint16_t len, uint8_t type);
uint16_t set_dev_ancdut_deinit(uint8_t *payload, uint16_t len, uint8_t type);

tlv_app_op_code_t ux_protocol_set_dev_parse_process(diag_ser_data_t *msg);

#endif
