/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: hearing media api
 * Author: AuthorNameMagicTag
 * Create: 2023-02-28
 */

#ifndef HEARING_MEDIA_H
#define HEARING_MEDIA_H

#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"
#include "cmsis_os2.h"
#include "ux_feature_config.h"
#include "key_struct.h"
#ifdef SUPPORT_UX_HAID
#include "haid_manager.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define HAID_MEDIA_FSM_IDLE 0 // 助听未启动
#define HAID_MEDIA_FSM_SCENE 1 // 助听场景
#define HAID_MEDIA_FSM_FIT 2 // 验配阶段
#define HAID_MEDIA_FSM_MEDICAL 3 // 医规测试
#define HAID_MEDIA_FSM_AFC 4 // 啸叫校准

typedef enum {
    HAID_WORK_NORMAL = 0,
    HAID_WORK_MEDICAL = 1,
    HAID_WORK_DISABLE = 2,
    HAID_WOEK_SOUND_MIX = 3,
    HAID_WORK_MODE_BUTT
} haid_work_mode_t;

#define HAID_MEDIA_VOL_EXPAND_COEF 1000 // app下发的音量的放大系数
#define HAID_MEDIA_CALIB_VOL_COEF 1000 // 校准mic\speak音量补偿系数
#define HAID_MEDIA_VOL_DECIMAL_EXPAND_COEF 125 // 音量小数部分必须是125整数倍

#define HAID_MEDIA_VOL_DB_HL_MAX 80  // 芯片支持的最大听力损失等级

#define HAID_MEDIA_MUTE_DB_VOL  (-120000) // 助听MUTE音量值
typedef enum {
    HAID_MODE_CHANGE_LOCAL = 0, // 本耳机调节助听模式
    HAID_MODE_CHANGE_TWS, // 对耳同步助听模式
    HAID_MODE_CHANGE_BUTT
} haid_mode_change_type;

typedef struct {
    uint8_t type; // haid_mode_change_type
    uint8_t mode; // 助听模式序号
} haid_mode_sync_t;

#ifdef SUPPORT_UX_HAID
haid_mode_t *haid_media_get_mode_cfg(void);
HaidVolume haid_media_conv_vol(int32_t vol, int32_t coef);
#endif
haid_vol_t *haid_media_get_vol_cfg(void);
int32_t haid_media_get_local_vol(void);
bool haid_media_get_haid_handle_flag(void);
void haid_media_set_fsm_state(uint8_t state);
uint8_t haid_media_get_fsm_state(void);
void haid_media_init(void);
bool haid_media_check_vol_up_max(void);
bool haid_media_check_vol_down_min(void);
bool haid_media_check_vol_range(const haid_vol_t *vol);
bool haid_media_check_vol_valid(int32_t vol);
void haid_media_update_vol_range(int32_t vol);
bool haid_media_store_vol_cfg(void);
bool haid_media_vol_up(void);
bool haid_media_vol_down(void);
bool haid_media_store_mode_cfg(void);
void haid_media_set_local_mode(uint8_t mode);
uint8_t haid_media_get_local_mode(void);
bool haid_media_check_mode_vaild(uint8_t mode);
bool haid_media_set_mode(uint8_t mode);
bool haid_media_change_mode(uint8_t mode);
bool haid_media_write_param_direct(const uint8_t *data, uint32_t len);
bool haid_media_read_param_direct(const uint8_t *data, uint32_t len);
bool haid_media_set_hw_config(int8_t *config, uint32_t config_len);
bool haid_media_set_allconfig(int8_t *config, uint32_t config_len);
bool haid_media_get_allconfig(int8_t *config, uint32_t *config_len);
bool haid_media_create(void);
bool haid_media_destory(void);
bool haid_media_enable(void);
bool haid_media_disable(void);
bool haid_media_set_config(int8_t *config, uint32_t config_len);
bool haid_media_get_config(int8_t *config, uint32_t *config_len);
bool haid_media_play_tone(uint32_t freq, int32_t vol, uint32_t mask);
bool haid_media_stop_tone(void);
bool haid_media_set_vol(int32_t vol);
bool haid_media_set_vol_by_range(const uint8_t *vol_range, uint16_t len);
osTimerId_t haid_fit_get_tone_timer_handle(void);
void haid_media_set_mic_spk_calib(void);
void haid_update_scene_mode(uint8_t *data, uint8_t datalen);
void haid_media_set_sence_num(uint8_t sence_num);
uint8_t haid_media_get_sence_num(void);
uint8_t haid_media_get_next_mode(uint8_t curr_mode);
errcode_t haid_media_set_vol_type(uint8_t vol_type);
uint8_t haid_media_get_vol_type(void);
uint8_t haid_media_read_vol_type_cfg(void);
void haid_media_set_mix_vol_range(uint16_t vol_range);
uint16_t haid_media_get_mix_vol_range(void);
bool haid_media_store_mix_vol_range(uint16_t vol_range);
void haid_media_read_mix_vol_range(void);
bool haid_media_check_vol_range_valid(uint16_t vol_range);
bool haid_aid_out_pre_handle(void);
bool haid_media_get_haid_hw_param(void);
int32_t haid_media_get_mix_vol(void);
uint8_t haid_media_save_haid_hw_param(void);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
