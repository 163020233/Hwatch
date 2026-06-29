/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: Provide ux audio recorder\n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-09-01， Create file. \n
 */
#ifndef UX_AUDIO_RECORDER_H
#define UX_AUDIO_RECORDER_H

#include "errcode.h"
#include "ux_audio_stream.h"
#include "ux_api_audio_recorder.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define DEFAULT_SAMPLE_RATE 48000
#define DEFAULT_BIT_WIDTH 24
#define DEFAULT_FILE_SPLIT_DURATION 60
#define DEFAULT_FILE_SPLIT_SIZE (1024 * 1024 * 1024)
#define DEFAULT_OUTPUT_DIR "/user/recorder"
#define MIN_OUTPUT_PATH_LEN 6
#define MAX_OUTPUT_PATH_LEN 30

typedef struct {
    bool alg_bypass;                            /* 录音音效算法旁路 */
    uint32_t split_size_bytes;                  /* 录音文件分割大小，单位：字节 */
    uint32_t split_duration_seconds;            /* 录音文件分割时长，单位：秒 */
    ux_api_audio_recorder_bit_width_t bit_width; /* 录音文件的位深度 */
    ux_api_audio_recorder_sample_rate_t sampling_rate_hz; /* 采样率，单位：Hz，例如：44100, 48000 */
    char output_directory[MAX_OUTPUT_PATH_LEN]; /* 输出文件目录路径 */
} recorder_info_t;

typedef enum {
    UX_AUDIO_RECORDER_STATUS_START = 0,
    UX_AUDIO_RECORDER_STATUS_STOP,

    UX_AUDIO_RECORDER_STATUS_MAX
} ux_audio_recorder_status_t;


typedef enum {
    UX_AUDIO_RECORDER_ALG_STATE_PRE_PROCESS = 1, /* 支持算法前浮点内录 */
    UX_AUDIO_RECORDER_ALG_STATE_POST_PROCESS = 2, /* 支持算法后浮点内录 */
    UX_AUDIO_RECORDER_ALG_STATE_MAX,
} ux_audio_recorder_alg_state_t;

void ux_audio_recorder_init(void);
errcode_t ux_audio_recorder_start(void);
errcode_t ux_audio_recorder_stop(void);
errcode_t ux_audio_recorder_set_bit_width(ux_api_audio_recorder_bit_width_t bit_width);
errcode_t ux_audio_recorder_get_bit_width(ux_api_audio_recorder_bit_width_t *bit_width);
errcode_t ux_audio_recorder_set_sample_rate(ux_api_audio_recorder_sample_rate_t sample_rate);
errcode_t ux_audio_recorder_get_sample_rate(ux_api_audio_recorder_sample_rate_t *sample_rate);
errcode_t ux_audio_recorder_set_split_by_duration(uint32_t duration);
errcode_t ux_audio_recorder_get_split_by_duration(uint32_t *duration);
errcode_t ux_audio_recorder_set_split_by_size(uint32_t size);
errcode_t ux_audio_recorder_get_split_by_size(uint32_t *size);
errcode_t ux_audio_recorder_set_output_dir(const char *output_dir, uint16_t output_dir_len);
errcode_t ux_audio_recorder_get_output_dir(char *output_dir, uint16_t output_dir_len);
errcode_t ux_audio_recorder_get_remain_time(uint64_t *remain_time, uint16_t data_len);
ux_audio_recorder_status_t ux_audio_get_local_recorder_status(void);
errcode_t ux_audio_recorder_set_alg_bypass(bool bypass);
errcode_t ux_audio_recorder_get_alg_bypass(bool* bypass);
errcode_t ux_audio_recorder_set_frame_length(ux_audio_frame_length_t frame_length);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
