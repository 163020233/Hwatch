/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: audio_calib_service_handle.c header file
 * Author : AuthorNameMagicTag
 * Create : 2023-07-10
 */

#ifndef AUDIO_CALIB_SERVICE_HANDLE_H
#define AUDIO_CALIB_SERVICE_HANDLE_H

#include "conn_tlv_err_code.h"
#include "diag_service.h"

#ifdef __cplusplus
extern "C" {
#endif

tlv_app_op_code_t mic_spk_loop_handle(diag_ser_data_t *msg);
tlv_app_op_code_t spk_calib_handle(diag_ser_data_t *msg);
tlv_app_op_code_t spk_calib_save_handle(diag_ser_data_t *msg);
tlv_app_op_code_t mic_switch_handle(diag_ser_data_t *msg);
tlv_app_op_code_t mic_calib_handle(diag_ser_data_t *msg);
tlv_app_op_code_t mic_calib_save_handle(diag_ser_data_t *msg);

#ifdef __cplusplus
}
#endif

#endif