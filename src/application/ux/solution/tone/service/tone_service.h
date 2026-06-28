/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provide ux tone service \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-03-08， Create file. \n
 */
#ifndef TONE_SERVICE_H
#define TONE_SERVICE_H
#include "errcode.h"
#include "ux_audio_tone_service.h"
#include "ux_api_audio_tone.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum {
    UX_TURN_UP_VOLUME,
    UX_MAX_VOLUME,
    UX_TURN_DOWN_VOLUME,
    UX_MIN_VOLUME,
} ux_haid_volume_t; /* UX_MASTER_HAID_VOLUMEUP、UX_MASTER_HAID_VOLUMEDOWN 提示音事件对应的扩展标识 */

typedef struct {
    uint16_t id;                        /* tone id */
    uint8_t play_way;                   /* 播放方式 */
    bool is_slave_play;                 /* 副耳是否播放 */
    const char* path;                   /* 音源路径 */
    bool enabled;                       /* 是否支持播放(当前对外协议可设置是否使能所有提示音) */
    int8_t priority;                    /* 播放优先级 */
    ux_api_audio_play_after_callback_t play_after;   /* function 播放相应提示音后对应的处理，默认空 */
    int32_t mix_volume;                 /* 当前提示音播放时音乐或通话的混音音量占比 */
    int32_t max_interrupt_count;        /* 提示音最大打断次数 */
    bool immediate_play_back_complete_callback;  /* 是否播放完成后立刻回调LITE_PLAY_EVENT_COMPLETE事件 */
} tone_info_map_t;

typedef struct {
    ux_api_audio_tone_language_t id;        /* 语言对应的id值,与NV中匹配 */
    const char *language_str;   /* 语言对应的字符串简写,如中文:CN,英文:EN */
} tone_language_map_t;

typedef uint8_t (*tone_info_callback_t)(void);

typedef struct {
    tone_info_callback_t tone_func;
} ux_tone_reg_funcs_t;

static ux_tone_reg_funcs_t g_ux_tone_funcs;

const tone_language_map_t* ux_get_tone_language_map(uint8_t id);
const tone_info_map_t* ux_get_tone_info_by_id(uint16_t id);
uint16_t ux_audio_get_tone_info_max(void);

void ux_audio_play_tone(uint16_t tone_id);
void ux_audio_stop_all_tone(void);
errcode_t ux_audio_set_tone_play_work_state(uint8_t work_state);
void ux_audio_set_tone_forbidden_state(bool state);
bool ux_audio_get_tone_forbidden_state(void);

uint8_t ux_audio_get_tone_info_register_num(void);
errcode_t ux_audio_register_tone_handle(tone_info_callback_t cb);
errcode_t ux_audio_get_tone_info(tone_info_map_t *tone_map, uint8_t tone_map_len);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
