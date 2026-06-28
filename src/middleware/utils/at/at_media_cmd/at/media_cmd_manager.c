/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description: Provides at cmd manager source\n
 * Author: CompanyName \n
 * History: \n
 * 2025-02-26， Create file. \n
 */

#include "media_at_process.h"
#include "debug_print.h"
#include "soc_osal.h"
#include "media_cmd_manager.h"

typedef struct {
    char *str_cmd;
    media_audio_scene scene;
    uint8_t cmd;
} media_cmd_table_t;

static const media_cmd_table_t g_media_cmd2scene[] = {
    { "MUSICPLAY",                   AUDIO_PLAY,              AUDIO_PLAY_START},
    { "MUSICPAUSE",                  AUDIO_PLAY,              AUDIO_PLAY_PAUSE},
    { "MUSICRESUME",                 AUDIO_PLAY,              AUDIO_PLAY_RESUME},
    { "MUSICSTOP",                   AUDIO_PLAY,              AUDIO_PLAY_STOP},
    { "MUSICABLUMINFO",              AUDIO_PLAY,              AUDIO_PLAY_ALBUM_INFO},
    { "SETMUSICVOLUME",              AUDIO_PLAY,              AUDIO_PLAY_SET_VOLUME},
    { "GETMUSICVOLUME",              AUDIO_PLAY,              AUDIO_PLAY_GET_MUSIC_VOLUME},
    { "SETBTSCOVOLUME",              AUDIO_PLAY,              AUDIO_PLAY_SET_BT_SCO_VOLUME},
    { "GETBTSCOVOLUME",              AUDIO_PLAY,              AUDIO_PLAY_GET_BT_SCO_VOLUME},
    { "MUSICSINGLELOOP",             AUDIO_PLAY,              AUDIO_PLAY_SINGLE_LOOP},
    { "MUSICSWITCHSONG",             AUDIO_PLAY,              AUDIO_PLAY_SWITCH_SONG},
    { "GETCURRENTMUSICSTATE",        AUDIO_PLAY,              AUDIO_PLAY_CURRENT_STATE},
    { "NOTIFYPLAY",                  AUDIO_PLAY,              NOTIFY_PLAY_START},
    { "AUDIOPLAYSERVICE",            AUDIO_PLAY,              AUDIO_PLAY_SERVICE},
    { "AUDIORECORD",                 AUDIO_RECORD,            AUDIO_RECORD_START},
    { "AUDIORECORDSTOP",             AUDIO_RECORD,            AUDIO_RECORD_STOP},
    { "SCENEINTERACTION001",         AUDIO_SCENE_INTERACTION, AUDIO_SCENE_INTERACTION_001},
    { "SCENEINTERACTION002",         AUDIO_SCENE_INTERACTION, AUDIO_SCENE_INTERACTION_002},
    { "SCENEINTERACTION003",         AUDIO_SCENE_INTERACTION, AUDIO_SCENE_INTERACTION_003},
    { "SCENEINTERACTION004",         AUDIO_SCENE_INTERACTION, AUDIO_SCENE_INTERACTION_004},
    { "SCENEINTERACTION005",         AUDIO_SCENE_INTERACTION, AUDIO_SCENE_INTERACTION_005},
    { "SCENEINTERACTION006",         AUDIO_SCENE_INTERACTION, AUDIO_SCENE_INTERACTION_006},
    { "SCENEINTERACTION007",         AUDIO_SCENE_INTERACTION, AUDIO_SCENE_INTERACTION_007},
    { "VIDEOPLAY",                   VIDEO_PLAY,              VIDEO_PLAY_START},
    { "VIDEOPAUSE",                  VIDEO_PLAY,              VIDEO_PLAY_PAUSE},
    { "VIDEORESUME",                 VIDEO_PLAY,              VIDEO_PLAY_RESUME},
    { "VIDEOSTOP",                   VIDEO_PLAY,              VIDEO_PLAY_STOP},
    { "VIDEOSINGLELOOP",             VIDEO_PLAY,              VIDEO_PLAY_SINGLE_LOOP},
    { "CAMERAPREVIEWSTART",          CAMERA_PREVIEW,          CAMERA_PREVIEW_START},
    { "CAMERAPREVIEWSTOP",           CAMERA_PREVIEW,          CAMERA_PREVIEW_STOP},
    { "VOICECALLVOLTE",              VOICE_CALL_VOLTE,        VOICE_CALL_VOLTE_HANDLE},
    { "A2DPSINKAEF",                 AUDIO_MANAGER,           A2DP_SINK_AEF},
    { "AUDIOMANAGERDUMP",            AUDIO_MANAGER,           AUDIO_MANAGER_DUMP},
    { "MEDIAVOICE",                  MEDIA_INTELLIGENT_VOICE, MEDIA_INTELLIGENT_VOICE },
    { "STREAMSOURCEPLAY",            MEDIA_STREAM_SOURCE_PLAY, STREAM_SOURCE_PLAY },
    { "MEDIASETSEALIB",              AUDIO_MANAGER,           AUDIO_MANAGER_SEA_LIB},
};

static int32_t at_media_convert_bin_to_dec(int32_t pbin)
{
    int32_t result = 0;
    int32_t temp = pbin;

    while (temp != 0) {
        result += temp % 2; // 2 div
        temp /= 2; // 2 div
    }

    return result;
}

uint32_t media_at_cmd_handle(media_args *arg)
{
    PRINT("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);
    PRINT("at_media_at_entrance in, media_cmd_str = %s\n", arg->media_cmd_str);

    media_audio_scene scene = MEDIA_SCENE_MAX;
    uint8_t cmd = -1;
    for (uint8_t i = 0; i < sizeof(g_media_cmd2scene) / sizeof(media_cmd_table_t); i++) {
        if (strcmp(arg->media_cmd_str, g_media_cmd2scene[i].str_cmd) == 0) {
            scene = g_media_cmd2scene[i].scene;
            cmd = g_media_cmd2scene[i].cmd;
        }
    }
    PRINT("at_media_at_entrance scene = %d, cmd = %u\n", scene, cmd);
    if (scene == MEDIA_SCENE_MAX || cmd == (uint8_t)-1) {
        return -1;
    }

    int32_t argc = at_media_convert_bin_to_dec(arg->para_map);
    char *argv[] = {arg->para1, arg->para2, arg->para3, arg->para4, arg->para5, arg->para6, arg->para7, arg->para8,
        arg->para9, arg->para10, arg->para11, arg->para12, arg->para13, arg->para14, arg->para15, arg->para16,
        arg->para17, arg->para18, arg->para19, arg->para20};
    PRINT("AT+MEDIA=\n");
    for (int32_t i = 0; i < argc - 1; i++) {
        printf("param[%d] = %s\n", i, argv[i]);
    }
    PRINT("\n");
    media_at_process(scene, cmd, argc - 1, (const char **)argv);
    PRINT("{%s():%d} func out success.\r\n", __FUNCTION__, __LINE__);
    return ERRCODE_SUCC;
}
