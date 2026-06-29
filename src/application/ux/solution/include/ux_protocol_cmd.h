/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: ux protocal command and interface definition
 * Author: AuthorNameMagicTag
 * Create: 2023-05-29
 */

#ifndef UX_PROTOCOL_CMD_H
#define UX_PROTOCOL_CMD_H

#include "stdint.h"
#include "errcode.h"
#include "diag_service.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum {
    DEV_QUERY_CMD_BEGIN = 0, // no use
    DEV_QUERY_CONN_PARAM_CMD = 1,
    DEV_QUERY_BATT_PERCENT_CMD = 2,
    DEV_QUERY_EARPHONE_CONN_CMD = 3,
    DEV_QUERY_WEAR_STATUS_CMD = 4,
    DEV_QUERY_DEV_VOL_CMD = 5,
    DEV_QUERY_DEV_RTP_CMD = 6,
    DEV_QUERY_DEV_TIME_CMD = 7,
    DEV_QUERY_DEV_PAIRING_TIMEOUT_CMD = 8,
    DEV_QUERY_SDK_VERSION = 9,
    /* 自定义扩展请在这之后, DEV_QUERY_END之前添加 */
    DEV_QUERY_ANC_PARAM = 10,
    DEV_QUERY_SECTOR_INFO_CMD = 11, // 查询分区表信息
    DEV_QUERY_CMD_END
} dev_query_req_cmd_t;

typedef enum {
    HAID_FIT_CMD_BEGIN = 0,
    HAID_FIT_PASSTHROUGH_CMD = HAID_FIT_CMD_BEGIN,
    HAID_FIT_START_STOP_CMD = 1,
    HAID_FIT_GET_ENV_NOISE_CMD = 2,
    HAID_FIT_SET_CHN_CMD = 3,
    HAID_FIT_CHANGE_MODE_CMD = 4,
    HAID_FIT_SET_MIXED_MEDIA_CMD = 5,
    HAID_FIT_BT_TO_AID_TIME_DELAY_CMD = 6,
    HAID_FIT_POWER_ON_TIME_DELAY_CMD = 7,
    HAID_FIT_AUTO_DETECT_SWITCH_CMD = 8,
    HAID_FIT_PARAM_SAVE_CMD = 9,
    HAID_FIT_SET_MODE_VOL_CMD = 10,
    HAID_FIT_GET_DEV_STATISTIC_CMD = 11,
    HAID_FIT_GET_AID_STATISTIC_CMD = 12,
    HAID_FIT_DEV_FORBID_SWITCH_CMD = 13,
    HAID_FIT_MODE_VOL_SWITCH_CMD = 14,
    HAID_FIT_RPT_PHONE_EVENT_CMD = 15,
    HAID_FIT_QUERY_MODE_CMD = 16,
    HAID_FIT_WDRC_SWITCH_CMD = 17,
    HAID_FIT_MEDICAL_TEST_CMD = 18,
    HAID_FIT_HAID_ENABLE_CMD = 19,
    HAID_FIT_AFC_CALIBRATE_RPT_CMD = 20,
    /* 自定义扩展请在这之后，HAID_FIT_CMD_END之前添加 */
    HAID_FIT_CMD_END
} haid_cmd_id_t;

/* <穿戴类产品对外交互协议> 5.7对应命令字 */
typedef enum {
    DEV_SET_CMD_BEGIN = 0,
    DEV_SET_RESTORE_FACTORY_CMD = 1,
    DEV_SET_EARPHONE_WEAR_STATUS_CMD = 2,
    DEV_SET_VOL_CMD = 3,
    DEV_SET_TOUCH_SLIDE_CMD = 4,
    DEV_SET_FREQ_TONE_CMD = 5,
    DEV_SET_CONTINUE_TONE_CMD = 6,
    DEV_SET_KEY_CONTROL_CMD = 7,
    DEV_SET_MIC_SPK_LOOP_CMD = 8,
    DEV_SET_SPK_CALIB_CMD = 9,
    DEV_SET_SPK_CALIB_SAVE_CMD = 10,
    DEV_SET_MIC_SWITCH_CMD = 11,
    DEV_SET_MIC_CALIB_CMD = 12,
    DEV_SET_MIC_CALIB_SAVE_CMD = 13,
    DEV_SET_DEV_RESET = 14,
    DEV_SET_DEV_NAME_CMD = 15, // 设置DEV NAME
    DEV_SET_DEV_LANGUAGE_CMD = 16,
    DEV_SET_DEV_PAIRING_TIMEOUT_CMD = 17,
    /* 自定义扩展请在这之后, DEV_SET_CMD_END之前添加 */
    DEV_SET_DEV_ASYNC_TEST_CMD = 18, /* 用来模拟异步流程 */
    DEV_SET_DEV_POWEROFF_CMD = 19,
    DEV_SET_DEV_ANCDUT_CMD = 20,
    DEV_SET_DEV_AUDIO_STREAM_CMD = 21,
    DEV_SET_DEV_MICTYPE_CMD = 22,
    DEV_SET_DEV_AUDIO_EQ_CMD = 23,
    DEV_SET_CMD_END
} dev_set_req_cmd_t;

/* <穿戴类产品对外交互协议> 5.9对应命令字 */
typedef enum {
    DEV_BT_CMD_BEGIN = 0, // no use
    DEV_BT_ENABLE_CMD = 1,
    DEV_BT_RF_SINGLETONE_CMD = 2,
    DEV_BT_RF_RX_CMD = 3,
    DEV_BT_RF_TX_CMD = 4,
    /* 自定义扩展请在这之后, DEV_BT_END之前添加 */
    DEV_BT_CMD_END
} dev_bt_req_cmd_t;

/* <穿戴类产品对外交互协议> 5.14对应命令字 */
typedef enum {
    MESH_DEV_CMD_BEGIN = 0,
    MESH_DEV_QUERY_AUDIO_STATUS = 1,
    MESH_DEV_SET_AUDIO_PARAM = 2,
    MESH_DEV_QUERY_DEV_STATUS = 3,
    MESH_DEV_SET_DEV_PARAM = 4,
    MESH_DEV_CMD_END
} mesh_dev_cmd_t;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
