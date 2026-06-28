/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: mesh_dev_handle.c header file
 * Author: AuthorNameMagicTag
 * Create: 2025-05-23
 */

#ifndef MESH_DEV_HANDLE_H
#define MESH_DEV_HANDLE_H

#include "diag_service.h"
#include "conn_tlv_err_code.h"
#include "conn_tlv_api.h"

#define DEVICE_ID_TYPE              1

typedef enum {
    MESH_DEV_ID_START = 0,
    MESH_DEV_ID_RX = MESH_DEV_ID_START,
    MESH_DEV_ID_TX1,
    MESH_DEV_ID_TX2,
    MESH_DEV_ID_TX3,
    MESH_DEV_ID_TX4,
    MESH_DEV_ID_TX5,
    MESH_DEV_ID_TX6,
    MESH_DEV_ID_TX7,
    MESH_DEV_ID_TX8,

    MESH_DEV_ID_END
} mesh_dev_id_t;

/* 协议5.14.1 type类型定义 */
typedef enum {
    MESH_DEV_QUERY_AUDIO_STATUS_BEGIN = 0,
    MESH_DEV_QUERY_AUDIO_DEVICE_ID = DEVICE_ID_TYPE,
    MESH_DEV_QUERY_ENERGY_VALUE,
    MESH_DEV_QUERY_ANR_MODE,
    MESH_DEV_QUERY_VOLUME_LEVEL,
    MESH_DEV_QUERY_ANC_MODE,
    MESH_DEV_QUERY_MIX_MODE,
    MESH_DEV_QUERY_OUTPUT_DEVICE,
    MESH_DEV_QUERY_EQ_MODE,
    MESH_DEV_QUERY_RECODING_STATE,
    MESH_DEV_QUERY_AUDIO_STATUS_END
} mesh_dev_query_audio_status_type_t;

/* 协议5.14.2 type类型定义 */
typedef enum {
    MESH_DEV_SET_AUDIO_PARAM_BEGIN = 0,
    MESH_DEV_SET_AUDIO_DEVICE_ID = DEVICE_ID_TYPE,
    MESH_DEV_SET_ANR_MODE,
    MESH_DEV_SET_VOLUME_LEVEL,
    MESH_DEV_SET_ANC_MODE,
    MESH_DEV_SET_MIX_MODE,
    MESH_DEV_SET_OUTPUT_DEVICE,
    MESH_DEV_SET_EQ_MODE,
    MESH_DEV_SET_RECODING_STATE,
    MESH_DEV_SET_AUDIO_PARAM_END
} mesh_dev_set_audio_param_type_t;

/* 协议5.14.3 type类型定义 */
typedef enum {
    MESH_DEV_QUERY_DEVICE_STATUS_BEGIN = 0,
    MESH_DEV_QUERY_DEVICE_ID = DEVICE_ID_TYPE,
    MESH_DEV_QUERY_MIC_DIST,
    MESH_DEV_QUERY_AUTO_SHUTDOWN_DURATION,
    MESH_DEV_QUERY_LED_MODE,
    MESH_DEV_QUERY_DEVICE_STATUS_END
} mesh_dev_query_device_status_type_t;

/* 协议5.14.4 type类型定义 */
typedef enum {
    MESH_DEV_SET_DEVICE_PARAM_BEGIN = 0,
    MESH_DEV_SET_DEVICE_ID = DEVICE_ID_TYPE,
    MESH_DEV_SET_MIC_DIST,
    MESH_DEV_SET_AUTO_SHUTDOWN_DURATION,
    MESH_DEV_SET_LED_MODE,
    MESH_DEV_SET_DEVICE_PARAM_END
} mesh_dev_set_device_param_type_t;

typedef uint16_t (*mesh_dev_proc_fun_cb)(mesh_dev_id_t device_id, uint8_t *payload, uint16_t len, uint8_t type);

mesh_dev_proc_fun_cb *get_query_audio_status_map(void);
mesh_dev_proc_fun_cb *get_set_audio_param_map(void);
mesh_dev_proc_fun_cb *get_query_device_status_map(void);
mesh_dev_proc_fun_cb *get_set_device_param_map(void);

/* 5.14.1 */
uint16_t mesh_dev_query_energy_value(mesh_dev_id_t device_id, uint8_t *payload, uint16_t len, uint8_t type);
uint16_t mesh_dev_query_anr_mode(mesh_dev_id_t device_id, uint8_t *payload, uint16_t len, uint8_t type);
uint16_t mesh_dev_query_volume_level(mesh_dev_id_t device_id, uint8_t *payload, uint16_t len, uint8_t type);
uint16_t mesh_dev_query_mix_mode(mesh_dev_id_t device_id, uint8_t *payload, uint16_t len, uint8_t type);
uint16_t mesh_dev_query_output_device(mesh_dev_id_t device_id, uint8_t *payload, uint16_t len, uint8_t type);
uint16_t mesh_dev_query_eq_mode(mesh_dev_id_t device_id, uint8_t *payload, uint16_t len, uint8_t type);
uint16_t mesh_dev_query_recoder_state(mesh_dev_id_t device_id, uint8_t *payload, uint16_t len, uint8_t type);

/* 5.14.2 */
uint16_t mesh_dev_set_anr_mode(mesh_dev_id_t device_id, uint8_t *payload, uint16_t len, uint8_t type);
uint16_t mesh_dev_set_volume_level(mesh_dev_id_t device_id, uint8_t *payload, uint16_t len, uint8_t type);
uint16_t mesh_dev_set_mix_mode(mesh_dev_id_t device_id, uint8_t *payload, uint16_t len, uint8_t type);
uint16_t mesh_dev_set_output_device(mesh_dev_id_t device_id, uint8_t *payload, uint16_t len, uint8_t type);
uint16_t mesh_dev_set_eq_mode(mesh_dev_id_t device_id, uint8_t *payload, uint16_t len, uint8_t type);
uint16_t mesh_dev_set_recoder_state(mesh_dev_id_t device_id, uint8_t *payload, uint16_t len, uint8_t type);

tlv_app_op_code_t ux_protocol_mesh_dev_query_audio_status(diag_ser_data_t *msg);
tlv_app_op_code_t ux_protocol_mesh_dev_set_audio_param(diag_ser_data_t *msg);
tlv_app_op_code_t ux_protocol_mesh_dev_query_device_status(diag_ser_data_t *msg);
tlv_app_op_code_t ux_protocol_mesh_dev_set_device_param(diag_ser_data_t *msg);

#endif
