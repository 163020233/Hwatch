/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provide ux cmd mic tx service \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-07-21， Create file. \n
 */
#ifndef UX_CMD_MIC_TX_H
#define UX_CMD_MIC_TX_H

#include <stdint.h>
#include "errcode.h"
#include "ux_api_communication.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
errcode_t ux_cmd_notify_recorder_alg_bypass_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_notify_file_path_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_notify_file_split_size_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_notify_battery_level_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_notify_auto_power_off_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_notify_volume_level_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_notify_low_cut_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_notify_nr_mode_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_notify_eq_mode_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_notify_recorder_state_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_notify_mute_state_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_notify_remain_time_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_notify_sample_rate_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_notify_bit_width_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_notify_file_split_duration_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_notify_file_split_size_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_notify_sdk_version_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_set_hid_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_notify_recovery_result_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_notify_file_format_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_set_auto_power_off_control(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_notify_nr_mode_info(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_notify_volume(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_notify_mute(uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
