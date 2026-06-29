/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: ux hearing fit handle
 * Author: AuthorNameMagicTag
 * Create: 2023-02-24
 */

#ifndef HAID_FITTING_H
#define HAID_FITTING_H

#include "stdint.h"
#include "stdbool.h"
#include "errcode.h"
#include "diag_service.h"
#include "conn_tlv_err_code.h"
#include "haid_manager.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef bool (*is_haid_proc_cb)(uint8_t cmd);
/* <穿戴类产品对外交互协议> 5.3对应命令字 */
typedef tlv_app_op_code_t (*haid_fit_handle_fun)(diag_ser_data_t *msg); // 助听业务处理函数

#define HAID_FIT_START_TEST 0x01 // 开始助听验配
#define HAID_FIT_STOP_TEST 0x02 // 停止助听验配

#define HAID_DISABLE_DEV_SWITCH_MODE 1 // 禁止设备侧设置助听模式
#define HAID_ENABLE_DEV_SWITCH_MODE 2 // 使能设备侧设置助听模式

#define HAID_DEV_MODE_NO_SAVE 1 // 设备侧设置助听模式下次重启不生效
#define HAID_DEV_MODE_SAVE 2 // 设备侧设置助听模式下次重启生效

#define HAID_DEV_VOL_NO_SAVE 1 // 设备侧设置助听音量下次重启不生效
#define HAID_DEV_VOL_SAVE 2 // 设备侧设置助听音量下次重启生效

#define HAID_MIX_BT_CALL_AND_MUSIC 1
#define HAID_MIX_NO_BT_CALL_AND_MUSIC 2
#define HAID_MIX_BT_CALL 3
#define HAID_MIX_BT_MUSIC 4

#define WDRC_SWITCH_DISABLE 1 // 不使能WDRC调节音频音量
#define WDRC_SWITCH_ENABLE 2 // 使能WDRC调节音频音量
#define WDRC_STATUS_ERR 0XFF

// 支持最大听损等级
#define HAID_FIT_SINUS_VOL_HL_MAX 80
// 支持最小听损等级
#define HAID_FIT_SINUS_VOL_HL_MIN 0

void haid_fit_init(void);
void haid_fit_set_param_load_flag(bool flag);
bool haid_fit_get_param_load_flag(void);
uint8_t haid_fit_get_mode_save_flag(void);
uint8_t haid_fit_get_vol_save_flag(void);
uint8_t haid_fit_get_dev_cfg_flag(void);
void haid_fit_condition_fun_register(is_haid_proc_cb cb);
bool haid_fit_load_hw_param(void);
bool haid_fit_load_all_param(void);
bool haid_fit_cfg_stop_proc(uint8_t state);
void haid_fit_set_tone_time(uint16_t timeout);
bool haid_fit_play_tone(uint32_t frequence, const HaidVolume *volume);
tlv_app_op_code_t haid_fit_cfg_start_stop_handle(diag_ser_data_t *msg);
tlv_app_op_code_t haid_fit_get_evn_noise_handle(diag_ser_data_t *msg);
tlv_app_op_code_t haid_fit_set_chn_handle(diag_ser_data_t *msg);
tlv_app_op_code_t haid_fit_change_mode_handle(diag_ser_data_t *msg);
tlv_app_op_code_t haid_fit_enable_auto_mode_handle(diag_ser_data_t *msg);
tlv_app_op_code_t haid_fit_param_save_handle(diag_ser_data_t *msg);
tlv_app_op_code_t haid_fit_set_mode_vol_handle(diag_ser_data_t *msg);
tlv_app_op_code_t haid_fit_get_mode_statistic_handle(diag_ser_data_t *msg);
tlv_app_op_code_t haid_fit_dev_cfg_switch_handle(diag_ser_data_t *msg);
tlv_app_op_code_t haid_fit_save_switch_handle(diag_ser_data_t *msg);
tlv_app_op_code_t haid_fit_rpt_phone_state_handle(diag_ser_data_t *msg);
tlv_app_op_code_t haid_fit_get_mode_handle(diag_ser_data_t *msg);
tlv_app_op_code_t haid_fit_wdrc_switch_handle(diag_ser_data_t *msg);
tlv_app_op_code_t haid_fit_transparent_msg_handle(diag_ser_data_t *msg);
bool haid_fit_set_mode(uint8_t mode);

uint8_t get_a2dp_enable_mode_process(void);
uint8_t get_hfp_enable_mode_process(void);

bool haid_fit_stop(void);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
