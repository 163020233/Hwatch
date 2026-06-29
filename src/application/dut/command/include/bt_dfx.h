/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * File          bt_dfx.h
 * Description:  Audio bt device log code define and log interface
 */
#ifndef BT_DFX_H
#define BT_DFX_H

#include "stdint.h"
#include "errcode.h"

typedef enum {
    BT_ANT_CHANNEL_0,
    BT_ANT_CHANNEL_1,
    BT_ANT_CHANNEL_2,
    BT_ANT_CHANNEL_3,
} bt_ant_status;

errcode_t bt_dfx_capture_datamem(uint8_t *para);

errcode_t bt_dfx_capture_datablock(uint8_t *para);

errcode_t bt_dfx_capture_dataloop_config(uint8_t *para);

errcode_t bt_dfx_capturedatareg(uint8_t *para);

uint32_t bt_dfx_get_calibration_param(uint8_t *para);

errcode_t bt_dfx_capture_dataloop(uint8_t *para);

errcode_t bt_dfx_antenable(uint8_t *param);

errcode_t bt_dfx_get_ant_status(uint8_t *para);

#endif
