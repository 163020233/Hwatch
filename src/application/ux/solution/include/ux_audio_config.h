/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * File          ux_audio_config.h
 * Description:  provide ux audio config header file.
 */
#ifndef UX_AUDIO_CONFIG_H
#define UX_AUDIO_CONFIG_H

#include "errcode.h"
#include "stddef.h"
#include "stdbool.h"
#include "ux_errcode.h"
#include "ux_api_audio_effect.h"
#include "ux_api_audio_recorder.h"
#include "ux_audio_recorder.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
    bool record_state;                               /* 录音状态 */
    ux_api_audio_recorder_bit_width_t bit_width;     /* 比特位宽 */
    ux_api_audio_recorder_sample_rate_t sample_rate; /* 采样率 */
    uint32_t split_size;                             /* 分割大小 */
    uint32_t split_duration;                         /* 分割时长 */
    char output_dir[MAX_OUTPUT_PATH_LEN];            /* 录音文件存储路径 */
    uint16_t output_dir_len;                         /* 录音文件存储路径长度 */
    bool alg_bypass;                                 /* 录音音效算法关闭状态 */
} ux_audio_recorder_cfg_t;

typedef struct {
    uint32_t session_id;
    uint16_t conn_id;
    bool low_cut;                     /* 记录当前低切状态 */
    bool mute;                        /* 记录当前mute状态 */
    ux_audio_recorder_cfg_t recorder; /* 记录录音配置状态 */
    ux_api_audio_nr_mode_t nr_mode;   /* 降噪模式 */
    ux_api_audio_eq_mode_t eq_mode;       /* EQ模式 */
    ux_api_audio_nr_mode_t live_mic_nr;   /* 直播麦音频流降噪模式 */
    ux_api_audio_nr_mode_t recorder_nr;   /* 录音音频流降噪模式 */
    int32_t volume;                   /* 音量 */
    int32_t live_mic_vol;             /* 直播麦音量 */
    int32_t recorder_vol;             /* 录音音量 */
    uint64_t remain_time;             /* 剩余录制 */
} ux_audio_table_t;

/**
 * 设置音频设备的静音状态
 * @param device_id 音频设备ID
 * @param mute 静音状态，true表示静音，false表示取消静音
 * @return 错误码，0表示成功，其他值表示失败
 */
errcode_t ux_audio_cfg_set_mute_old(uint16_t device_id, bool mute);

/**
 * 获取音频设备的静音状态
 * @param device_id 音频设备ID
 * @param mute 指向静音状态的指针，true表示静音，false表示未静音
 * @return 错误码，0表示成功，其他值表示失败
 */
errcode_t ux_audio_cfg_get_mute_old(uint16_t device_id, bool *mute);

/**
 * 设置音频设备的低频截止状态
 * @param device_id 音频设备ID
 * @param low_cut 低频截止状态，true表示开启，false表示关闭
 * @return 错误码，0表示成功，其他值表示失败
 */
errcode_t ux_audio_cfg_set_low_cut(uint16_t device_id, bool low_cut);

/**
 * 获取音频设备的低频截止状态
 * @param device_id 音频设备ID
 * @param low_cut 指向低频截止状态的指针，true表示开启，false表示关闭
 * @return 错误码，0表示成功，其他值表示失败
 */
errcode_t ux_audio_cfg_get_low_cut(uint16_t device_id, bool *low_cut);

/**
 * 设置音频设备的录音状态
 * @param device_id 音频设备ID
 * @param record_state 录音状态，true表示开启录音，false表示关闭录音
 * @return 返回errcode_t类型的错误码，0表示成功，其他值表示失败
 */
errcode_t ux_audio_cfg_set_recorder_state(uint16_t device_id, bool record_state);

/**
 * 获取音频设备的录音状态
 * @param device_id 音频设备ID
 * @param record_state 指向bool类型的指针，用于存储录音状态，true表示开启录音，false表示关闭录音
 * @return 返回errcode_t类型的错误码，0表示成功，其他值表示失败
 */
errcode_t ux_audio_cfg_get_recorder_state(uint16_t device_id, bool *record_state);

/**
 * 设置录音设备的位宽
 * @param device_id 设备ID
 * @param bit_width 位宽
 * @return 返回errcode_t类型的错误码，0表示成功，其他值表示失败
 */
errcode_t ux_audio_cfg_set_recorder_bit_width(uint16_t device_id, ux_api_audio_recorder_bit_width_t bit_width);

/**
 * 获取录音设备的位宽
 * @param device_id 设备ID
 * @param bit_width 位宽
 * @return 返回errcode_t类型的错误码，0表示成功，其他值表示失败
 */
errcode_t ux_audio_cfg_get_recorder_bit_width(uint16_t device_id, ux_api_audio_recorder_bit_width_t *bit_width);

/**
 * 设置录音设备的采样率
 * @param device_id 设备ID
 * @param sample_rate 采样率
 * @return 返回errcode_t类型的错误码，0表示成功，其他值表示失败
 */
errcode_t ux_audio_cfg_set_recorder_sample_rate(uint16_t device_id, ux_api_audio_recorder_sample_rate_t sample_rate);

/**
 * 获取录音设备的采样率
 * @param device_id 设备ID
 * @param sample_rate 采样率
 * @return 返回errcode_t类型的错误码，0表示成功，其他值表示失败
 */
errcode_t ux_audio_cfg_get_recorder_sample_rate(uint16_t device_id, ux_api_audio_recorder_sample_rate_t *sample_rate);

/**
 * 设置录音设备的分段大小
 * @param device_id 设备ID
 * @param split_size 分段大小
 * @return 返回errcode_t类型的错误码，0表示成功，其他值表示失败
 */
errcode_t ux_audio_cfg_set_recorder_split_size(uint16_t device_id, uint32_t split_size);

/**
 * 获取录音设备的分段大小
 * @param device_id 设备ID
 * @param split_size 分段大小
 * @return 返回errcode_t类型的错误码，0表示成功，其他值表示失败
 */
errcode_t ux_audio_cfg_get_recorder_split_size(uint16_t device_id, uint32_t *split_size);

/**
 * 设置录音设备的分段时长
 * @param device_id 设备ID
 * @param split_duration 分段时长
 * @return 返回errcode_t类型的错误码，0表示成功，其他值表示失败
 */
errcode_t ux_audio_cfg_set_recorder_split_duration(uint16_t device_id, uint32_t split_duration);

/**
 * 获取录音设备的分段时长
 * @param device_id 设备ID
 * @param split_duration 分段时长
 * @return 返回errcode_t类型的错误码，0表示成功，其他值表示失败
 */
errcode_t ux_audio_cfg_get_recorder_split_duration(uint16_t device_id, uint32_t *split_duration);

/**
 * 设置录音文件存储路径
 * @param device_id 设备ID
 * @param output_dir 录音文件存储路径
 * @param data_len   录音文件存储路径长度
 * @return 返回errcode_t类型的错误码，0表示成功，其他值表示失败
 */
errcode_t ux_audio_cfg_set_recorder_output_dir(uint16_t device_id, char *output_dir, uint16_t data_len);

/**
 * 获取录音文件的存储路径
 * @param device_id 设备ID
 * @param output_dir 录音文件存储路径
 * @param data_len   录音文件存储路径长度
 * @return 返回errcode_t类型的错误码，0表示成功，其他值表示失败
 */
errcode_t ux_audio_cfg_get_recorder_output_dir(uint16_t device_id, char *output_dir, uint16_t data_len);

/**
 * 设置设备的降噪模式
 * @param device_id 设备ID
 * @param nr_mode 降噪模式
 * @return 返回errcode_t类型的错误码，0表示成功，其他值表示失败
 */
errcode_t ux_audio_cfg_set_nr_mode(uint16_t device_id, ux_api_audio_nr_mode_t nr_mode);

/**
 * 获取设备的降噪模式
 * @param device_id 设备ID
 * @param nr_mode 降噪模式
 * @return 返回errcode_t类型的错误码，0表示成功，其他值表示失败
 */
errcode_t ux_audio_cfg_get_nr_mode(uint16_t device_id, ux_api_audio_nr_mode_t *nr_mode);

/**
 * 设置设备的音量值
 * @param device_id 设备ID
 * @param volume 音量值
 * @return 返回errcode_t类型的错误码，0表示成功，其他值表示失败
 */
errcode_t ux_audio_cfg_set_volumn(uint16_t device_id, int32_t volume);

/**
 * 获取设备的音量值
 * @param device_id 设备ID
 * @param volume 音量值
 * @return 返回errcode_t类型的错误码，0表示成功，其他值表示失败
 */
errcode_t ux_audio_cfg_get_volumn(uint16_t device_id, int32_t *volume);

/**
 * 设置设备的EQ模式
 * @param device_id 设备ID
 * @param eq_mode EQ模式
 * @return 返回errcode_t类型的错误码，0表示成功，其他值表示失败
 */
errcode_t ux_audio_cfg_set_eq_mode(uint16_t device_id, ux_api_audio_eq_mode_t eq_mode);

/**
 * 获取设备的EQ模式
 * @param device_id 设备ID
 * @param eq_mode EQ模式
 * @return 返回errcode_t类型的错误码，0表示成功，其他值表示失败
 */
errcode_t ux_audio_cfg_get_eq_mode(uint16_t device_id, ux_api_audio_eq_mode_t *eq_mode);

/**
 * 设置设备的剩余录制时间
 * @param device_id 设备ID
 * @param eq_mode 剩余录制时间
 * @return 返回errcode_t类型的错误码，0表示成功，其他值表示失败
 */
errcode_t ux_audio_cfg_set_remain_time(uint16_t device_id, uint64_t remain_time);

/**
 * 获取设备的剩余录制时间
 * @param device_id 设备ID
 * @param remain_time 剩余录制时间
 * @return 返回errcode_t类型的错误码，0表示成功，其他值表示失败
 */
errcode_t ux_audio_cfg_get_remain_time(uint16_t device_id, uint64_t *remain_time);

errcode_t ux_audio_cfg_set_nr(uint16_t device_id, ux_api_audio_nr_info_t* info);
ux_api_audio_nr_mode_t ux_audio_cfg_get_nr(uint16_t device_id, ux_api_audio_stream_type_t stream);
errcode_t ux_audio_cfg_set_recorder_alg_bypass(uint16_t device_id, bool alg_bypass);
errcode_t ux_audio_cfg_get_recorder_alg_bypass(uint16_t device_id, bool* alg_bypass);
errcode_t ux_audio_cfg_set_volume(uint16_t device_id, ux_api_audio_volume_info_t* control);
errcode_t ux_audio_cfg_get_volume(uint16_t device_id, ux_api_audio_volume_info_t* control);
errcode_t ux_audio_cfg_set_mute(uint16_t device_id, ux_api_audio_mute_info_t* info);
errcode_t ux_audio_cfg_get_mute(uint16_t device_id, ux_api_audio_mute_info_t* info);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif