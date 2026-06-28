/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: header file for hearing aid
 * Author: audio
 */

#ifndef __HAID_STRUCT_H__
#define __HAID_STRUCT_H__

#include "td_type.h"
#include "ai_struct.h"
#include "sys_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define HAID_CONFIG_SIZE_MAX 530

#define HAID_VOL_MAX 60     /* max 60 dB */
#define HAID_VOL_MIN (-120) /* min -120 dB */
#define HAID_VOL_STEP_MIN 1 /* 1dB */
#define HAID_VOL_PRECI_COEF 1000     /* 0.125dB is 125 */

#define HAID_CALIBRATION_VOL_MAX 12    /* max 12dB */
#define HAID_CALIBRATION_VOL_MIN (-12) /* min -12dB */

typedef enum {
    HAID_EXIT,
    HAID_RUNNING,
    SINUS_TONE_RUNNING,
    CALIBRATION_RUNNING,
    GET_ENV_NOISE_RUNNING,
} haid_state;

typedef enum {
    HAID_EVENT_SCENE_CHANGE,
    HAID_EVENT_TYPE_MAX,
} haid_event_type;

typedef union {
    td_u32 value;
} haid_event_param;

typedef enum {
    HAID_SCENE_AUTO = 0,
    HAID_SCENE_TRANSPORTATION,
    HAID_SCENE_SOCIALIZING,
    HAID_SCENE_PARTY,
    HAID_SCENE_QUIET,
    HAID_SCENE_EXPAND0,
    HAID_SCENE_EXPAND1,
    HAID_SCENE_TEST,

    HAID_SCENE_MAX,
} haid_scene;

typedef enum {
    HAID_VOLTAGE_1P2 = 0x2,
    HAID_VOLTAGE_1P3 = 0x4,
    HAID_VOLTAGE_1P4 = 0x6,
    HAID_VOLTAGE_1P5 = 0x8,
    HAID_VOLTAGE_1P6 = 0xa,
    HAID_VOLTAGE_MAX = 0x10,
} haid_voltage;

typedef enum {
    HAID_UP_FIFO_0 = 0,
    HAID_UP_FIFO_1,
    HAID_DN_FIFO,

    HAID_FIFO_SEL_MAX,
} haid_fifo_sel;

typedef enum {
    HAID_POST_GAIN,
    HAID_PRE_GAIN,
    HAID_WDRC_GAIN,
    HAID_GAIN_TYPE_MAX,
} haid_volume_type;

typedef enum {
    SINUS_TONE_FREQ_125 = 125,
    SINUS_TONE_FREQ_250 = 250,
    SINUS_TONE_FREQ_500 = 500,
    SINUS_TONE_FREQ_750 = 750,
    SINUS_TONE_FREQ_1000 = 1000,
    SINUS_TONE_FREQ_1500 = 1500,
    SINUS_TONE_FREQ_2000 = 2000,
    SINUS_TONE_FREQ_3000 = 3000,
    SINUS_TONE_FREQ_4000 = 4000,
    SINUS_TONE_FREQ_6000 = 6000,
    SINUS_TONE_FREQ_8000 = 8000,
} sinus_tone_freq;

typedef struct {
    audio_ai_port ai_port;
    ao_port_id ao_port;

    audio_pcm_format in_pcm;
    audio_pcm_format out_pcm;
    haid_voltage voltage;
    ai_amic_sel mic_sel[AI_MIC_NUM_MAX];
} haid_attr;

typedef struct {
    td_u32 version;
    td_u32 release_time;
} haid_vesion;

/* High Precision Gain, ranging from -81dB to 18dB, step 0.125dB */
typedef struct {
    td_s32 integer; /* Interger part of high preicision gain */
    td_s32 decimal; /* decimal part of high preicision gain, if -0.125dB, value is -125 */
    haid_volume_type volume_type;
} haid_volume;

/**
 * @if Eng
 * @brief  hearing aid microphone calibration volume
 * @else
 * @brief  助听麦克风的校准音量
 * @endif
 */
typedef struct {
    td_u32 mic_num;  /*!< @if Eng microphone num used by hearing aid.
                          @else   助听使用的麦克风数量 @endif */
    haid_volume volume[AI_MIC_NUM_MAX]; /*!< @if Eng microphone calibration volume
                                             @else   每个麦克风的音量校准值 @endif */
} haid_cali_volume;

typedef struct {
    td_s8 *payload;
    td_u32 length;
} haid_ioctl_param;

typedef struct {
    td_handle h_mod;
    td_s8 config[HAID_CONFIG_SIZE_MAX];
    td_u32 config_len;
} haid_config;

typedef struct {
    td_handle h_mod;
    td_u32 all_config_addr;
    td_u32 all_config_len;
} haid_all_config;

typedef struct {
    haid_state state;
    haid_volume volume;
    haid_cali_volume cali_volume;
    haid_scene scene;
    haid_scene auto_scene;
    audio_pcm_format in_format;
    audio_pcm_format out_format;

    td_u32 ai_frame_count;
    td_u32 ao_frame_count;

    td_u32 dsp_freq;
    mcps_block mcps_list;
    td_u32 set_scene_fail_cnt;
    haid_vesion version;
} haid_proc_item;

typedef struct {
    td_u32 scene;
    td_u32 auto_scene;
    td_u32 set_scene_fail_cnt;
    haid_vesion release_version;
} haid_status;

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* __HAID_STRUCT_H__ */
