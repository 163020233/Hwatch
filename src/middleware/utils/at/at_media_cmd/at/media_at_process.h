/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description: Provides at cmd register func \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-02-26， Create file. \n
 */
#ifndef MEDIA_AT_SERVICE_H
#define MEDIA_AT_SERVICE_H

#include <stdint.h>
#include <stdio.h>
#include "errcode.h"
#include "soc_osal.h"
#include "errcode.h"
#include "diag_service.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef int32_t (*media_cmd_process_func)(const int32_t argc, const char **argv);

typedef struct {
    uint8_t id;
    media_cmd_process_func func;
} media_cmd_process_type;

typedef struct {
    uint8_t id;
    media_cmd_process_type *funcMap;
    uint32_t funcMapSize;
} MediaSceneProcessType;

typedef enum {
    AUDIO_PLAY,
    AUDIO_RECORD,
    AUDIO_SCENE_INTERACTION,
    VIDEO_PLAY,
    CAMERA_PREVIEW,
    VOICE_CALL_VOLTE,
    AUDIO_MANAGER,
    MEDIA_INTELLIGENT_VOICE,
    MEDIA_STREAM_SOURCE_PLAY,
    MEDIA_SCENE_MAX,
} media_audio_scene;

typedef enum {
    AUDIO_PLAY_START,             // 00
    AUDIO_PLAY_PAUSE,             // 01
    AUDIO_PLAY_RESUME,            // 02
    AUDIO_PLAY_STOP,              // 03
    AUDIO_PLAY_ALBUM_INFO,        // 04
    AUDIO_PLAY_SET_VOLUME,        // 05
    AUDIO_PLAY_GET_MUSIC_VOLUME,  // 06
    AUDIO_PLAY_SET_BT_SCO_VOLUME, // 07
    AUDIO_PLAY_GET_BT_SCO_VOLUME, // 08
    AUDIO_PLAY_SINGLE_LOOP,       // 09
    AUDIO_PLAY_SWITCH_SONG,       // 0A
    AUDIO_PLAY_CURRENT_STATE,     // 0B
    NOTIFY_PLAY_START,            // 0C
    AUDIO_PLAY_SERVICE,           // 0D
    STREAM_SOURCE_PLAY,           // 0F
#if (CONFIG_MEDIAAW_AUDIO_ENABLE == FEATURE_ON)
    AW,
#endif
    AUDIO_PLAY_MUTE,
    AUDIO_MAX_PLAY_ID,
} media_audio_play_cmd;

typedef enum {
    AUDIO_RECORD_START,
    AUDIO_RECORD_STOP,
    AUDIO_MAX_RECORD_ID,
} media_audio_record_cmd;

typedef enum {
    AUDIO_SCENE_INTERACTION_001 = 1,
    AUDIO_SCENE_INTERACTION_002,
    AUDIO_SCENE_INTERACTION_003,
    AUDIO_SCENE_INTERACTION_004,
    AUDIO_SCENE_INTERACTION_005,
    AUDIO_SCENE_INTERACTION_006,
    AUDIO_SCENE_INTERACTION_007,
} media_audio_scene_interaction_id;

typedef enum {
    VIDEO_PLAY_START,         // 00
    VIDEO_PLAY_PAUSE,         // 01
    VIDEO_PLAY_RESUME,        // 02
    VIDEO_PLAY_STOP,          // 03
    VIDEO_PLAY_SINGLE_LOOP,   // 04
    VIDEO_PLAY_SET_VOLUME,    // 05
    VIDEO_PLAY_GET_VOLUME,    // 06
    VIDEO_PLAY_CURRENT_STATE, // 07
    VIDEO_PLAY_MUTE,          // 08
    VIDEO_MAX_PLAY_ID,
} media_video_play_cmd;

typedef enum {
    CAMERA_PREVIEW_START,    // 00
    CAMERA_PREVIEW_STOP,     // 01
} media_camera_preview_cmd;

typedef enum {
    VOICE_CALL_VOLTE_HANDLE,   // 01
} media_voice_call_volte_cmd;

typedef enum {
    A2DP_SINK_AEF,             // 00
    AUDIO_MANAGER_DUMP,        // 01
    AUDIO_MANAGER_SEA_LIB,     // 02
} media_audio_manager_cmd;

void media_at_process(media_audio_scene scene, uint8_t cmd, const int32_t argc, const char **argv);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
