/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: adapt sdt
 * This file should be changed only infrequently and with great care.
 * Author: audio
 * Create: 2025-10-17
 */

#ifndef DIAG_AUDIO_H
#define DIAG_AUDIO_H
#include <stdint.h>
#include "errcode.h"
#include "diag.h"

#define NV_AUDIO_VQE_PARAM_SIZE 53
#define NV_AUDIO_SWS_PARAM_SIZE 162
#define NV_AUDIO_HVS_PARAM_SIZE 604
#define NV_AUDIO_USER_PARAM_SIZE    56
#define NV_NR_PARAM_SIZE            8
#define NV_AUDIO_AGC_PARAM_SIZE     8

typedef struct {
    uint32_t size;
    uint32_t data[NV_AUDIO_VQE_PARAM_SIZE];
} audio_vqe_param_t;

typedef struct {
    uint32_t size;
    uint8_t data[NV_AUDIO_SWS_PARAM_SIZE];
} audio_sws_param_t;

typedef struct {
    uint32_t size;
    uint8_t data[NV_AUDIO_HVS_PARAM_SIZE];
} audio_hvs_param_t;

typedef struct {
    uint8_t mod_id;
    uint8_t res;
    uint16_t mode;
} diag_nr_cmd_param_header;

typedef struct {
    diag_nr_cmd_param_header head;
    uint32_t data_type;
    uint8_t params[];
} diag_nr_cmd_param_struct;

typedef struct {
    uint32_t size;
    uint32_t data[NV_AUDIO_USER_PARAM_SIZE];
} audio_ahe_user_param_t;

typedef struct {
    uint32_t size;
    uint32_t data[NV_NR_PARAM_SIZE];
} nr_mic4stream_param_t;

typedef struct {
    uint32_t size;
    uint32_t data[NV_AUDIO_AGC_PARAM_SIZE];
} audio_agc_param_t;

errcode_t diag_see_param_save(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option);
errcode_t diag_see_param_trans(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option);
errcode_t diag_sws_param_save(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option);
errcode_t diag_aef_param_trans(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option);
errcode_t diag_hvs_param_trans(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option);
errcode_t diag_hvs_param_save(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option);
errcode_t diag_ahe_param_trans(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option);
errcode_t diag_ahe_param_save(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option);
errcode_t diag_ahe_mode_trans(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option);
#endif