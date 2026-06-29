/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * File          ux_bs_sle_event_term.h
 * Description:  provide ux bs sle event header file.
 */
#ifndef UX_BS_SLE_EVENT_TERM_H
#define UX_BS_SLE_EVENT_TERM_H

#include <stdint.h>
#include "bs_sdm_common_dpm.h"
#include "errcode.h"
#include "sle_common.h"
#include "ssf_as_stru.h"
#include "ssf_call.h"
#include "ssf_connection.h"
#include "ssf_device.h"
#include "ssf_persistence.h"
#include "stdbool.h"
#include "ux_common_define.h"
#include "ux_event_define.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define UX_BS_SLE_TERM_MAX_STREAM_NUM 8
#define UX_BS_SLE_STREAM_ACCEPT_FLAGS 1
#define UX_BS_SLE_DEFAULT_DEVICE_IDX  0
#define UX_BS_SLE_RESERVED_LEN_2      2
#define UX_BS_SLE_CALL_STATE_IDLE     0

typedef struct {
    sle_obj_t obj;
    uint32_t device_num;
    uint16_t device_id[SSF_PERSISTENCE_MAX_DEVICE_NUM];
    sle_addr_t device_addr[SSF_PERSISTENCE_MAX_DEVICE_NUM];
} sle_obj_device_data_s_t;

typedef struct {
    sle_obj_device_data_s_t object[SSF_PERSISTENCE_MAX_PDL_NUM];
    bs_sle_stream_stru_t stream_stru[UX_BS_SLE_TERM_MAX_STREAM_NUM];
    sle_obj_t active_obj;
    bs_sle_discovery_type_t discovery_type;
    uint8_t object_num;
    uint8_t stream_num;
    uint8_t call_id;
    uint8_t call_state;
    uint8_t media_state;
} ux_bs_sle_term_context;

typedef struct {
    uint16_t device_id;
    uint8_t type;
    uint8_t addr[UX_BS_ADDR_SIZE];
    uint8_t conn_state;
    uint8_t encrypt_state;
    uint8_t reason;     /* sle_disc_reason_t */
} bs_sle_connect_state_msg_s;

typedef struct {
    uint8_t state;      /* bs_sle_common_state_t */
    uint8_t type;       /* bs_sle_discovery_type_t */
    uint8_t reserved[UX_BS_SLE_RESERVED_LEN_2];
} bs_sle_pair_mode_msg_s;

typedef struct {
    uint16_t device_id;
    sle_addr_t sle_addr;
    bs_sle_pair_info_t pair_info;
} bs_sle_pair_info_data_s;

typedef struct {
    sle_obj_t obj;
    uint8_t count;
    bs_sle_call_status_t status[1];
} bs_sle_call_status_msg_s;

typedef struct {
    sle_obj_t obj;
    uint8_t call_id;
    uint8_t reason;
    uint8_t reserved[UX_BS_SLE_RESERVED_LEN_2];
} bs_sle_call_terminate_msg_s;

typedef struct {
    sle_obj_t obj;
    uint8_t type;           /* bs_sle_media_info_type_t */
    uint8_t info_len;
    uint8_t media_info[1];  /* bs_sle_media_play_status_t */
} bs_sle_media_info_msg_s;

typedef struct {
    sle_obj_t obj;
    bs_sle_stream_codec_param_t stream_codec;
    uint8_t stream_num;
} bs_sle_stream_codec_msg_s;

typedef struct {
    sle_obj_t obj;
    bs_sle_audio_stream_state_t state;
    errcode_t result;
    uint8_t stream_num;
    bs_sle_stream_stru_t stream_stru[1];
} bs_sle_stream_state_msg_s;

ux_bs_sle_term_context* ux_bs_sle_term_get_ctx(void);
errcode_t ux_bs_sle_term_register_callback(void);
errcode_t ux_bs_sle_term_register_callback_once(void);
void ux_bs_sle_term_unregister_callback(void);
void ux_bs_sle_term_ctx_init(void);
void ux_bs_sle_term_set_active_obj(sle_obj_t obj);
void ux_bs_sle_term_set_call_id(uint8_t call_id);
void ux_bs_sle_term_set_call_state(uint8_t call_state);
uint8_t ux_bs_sle_term_get_call_state(void);
void ux_bs_sle_term_set_media_state(uint8_t media_state);
uint8_t ux_bs_sle_term_get_media_state(void);
sle_obj_t ux_bs_sle_term_get_active_obj(void);
errcode_t ux_bs_sle_term_get_addr_by_obj(sle_obj_t obj, uint8_t *out_addr, uint16_t addr_len);
bs_sle_discovery_type_t ux_bs_sle_term_get_discovery_type(void);
uint8_t ux_bs_sle_term_get_call_id(void);
uint8_t ux_bs_sle_term_get_config_work_role(void);
uint8_t ux_bs_sle_term_get_config_tws_role(void);
void ux_bs_sle_record_pairing_state(bool is_start);
void ux_bs_sle_term_pdl_data_print(const bs_sle_nv_obj_data_t *data);
void ux_bs_combo_term_pdl_data_print(const bs_sdm_common_pdl_data_t *data);
void ux_bs_sle_event_callback(ux_event_bs_msg_t msg_id, void *param);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  // end UX_BS_SLE_EVENT_TERM_H
