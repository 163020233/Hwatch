/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provide ux cmd mic rx service \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-07-21， Create file. \n
 */
#ifndef UX_CMD_MIC_RX_H
#define UX_CMD_MIC_RX_H

#include <stdint.h>
#include "errcode.h"
#include "ux_api_communication.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

errcode_t ux_cmd_set_volume_level_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_set_low_cut_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_set_mute_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_set_noise_reduction_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_set_eq_mode_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_set_recorder_state_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_remain_time_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_set_recorder_bit_width_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_set_recorder_sample_rate_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_set_recorder_file_split_duration_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_set_recorder_file_split_size_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_set_recorder_file_split_size_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_set_recorder_file_path_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_set_file_format_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_set_power_off_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_set_power_reset_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_set_power_off_cancel_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_set_power_reset_cancel_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_set_auto_power_off_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_auto_power_off_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_set_sys_time_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_set_recovery_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_set_recorder_alg_bypass_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_recorder_state_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_recorder_bit_width_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_recorder_sample_rate_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_recorder_file_split_duration_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_recorder_file_split_size_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_recorder_file_path_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_low_cut_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_mute_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_volume_level_path_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_nr_mode_path_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_eq_mode_path_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_sdk_version_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
