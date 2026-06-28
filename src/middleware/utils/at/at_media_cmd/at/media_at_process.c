/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description: Provides media at service. \n
 * Author: CompanyName \n
 * History: \n
 * 2025-02-26， Create file. \n
 */

#include "media_at_process.h"
#include "debug_print.h"
#include "securec.h"
#include "at_cmd_api.h"
#include "common_def.h"
#include "soc_uapi_audio_sys.h"
#ifdef CONFIG_SUPPORT_MEDIA_MEMORY_MINI
#include "tone_player_sample_wrapper.h"
#else
#include "audio_capture_wrapper.h"
#include "audio_scene_interaction.h"
#include "video_player_sample_wrapper.h"
#include "camera_preview_sample_wrapper.h"
#include "voice_call_volte_sample_wrapper.h"
#include "audio_player_test.h"
#include "stream_source_player_sample.h"
#endif
#include "intelligent_voice_engine_test.h"
#include "a2dp_sink_service.h"
#include "audio_manager_service.h"
#include "player_sample_wrapper.h"
#include "td_base.h"

#define ARRAY_COUNT(array) (sizeof((array)) / sizeof(array[0]))

#define MEDIA_APP_REPLY_DATA_LEN 4
#define NUMBER_BASE 10

bool g_media_diag_flag = false;             // 用于控制执行功能后是否回复diag消息
diag_ser_header_t g_media_diag_header;      // 用于保存diag回复包头内容

/* 封装返回结果成Diag数据回发给BTH SPP
param descrip:
   send: 用于控制是否做diag消息回复
   data: 用于传入diag包包头信息
   payload: 回复tlv数据
   pay_len: 回复tlv数据长度
*/
uint32_t send_spp_diag_msg(bool send, diag_ser_header_t *data, uint8_t* payload, uint32_t pay_len)
{
    uint8_t size;
    diag_ser_data_t* diag_data;
    diag_ser_frame_t* frame;
    uint32_t result = ERRCODE_FAIL;

    // send为false，则直接返回
    if (send == false) {
        return ERRCODE_SUCC;
    }

    size = sizeof(diag_ser_data_t) + sizeof(diag_ser_frame_t) + pay_len;
    diag_data = (diag_ser_data_t *)malloc(size);
    if (diag_data == NULL) {
        return ERRCODE_FAIL;
    }

    (void)memcpy_s(&(diag_data->header), sizeof(diag_ser_header_t), data, sizeof(diag_ser_header_t));
    frame = (diag_ser_frame_t*)&(diag_data->payload);
    frame->module_id = data->ser_id;
    frame->cmd_id = data->cmd_id;
    (void)memcpy_s(&(frame->tlv), pay_len, payload, pay_len);
    diag_data->header.crc_en = 1;
    diag_data->header.length = size - sizeof(diag_ser_data_t);
    diag_data->header.dst = DIAG_FRAME_FID_BT_SPP;
    diag_data->header.src = data->dst;
    result = uapi_diag_service_send_data(diag_data);
    free(diag_data);
    return result;
}

void print_at_cmd_ret(int ret)
{
    uint32_t irq_sts = osal_irq_lock();
    if (ret == ERRCODE_SUCC) {
        PRINT("\r\nOK~\r\n");
    } else {
        PRINT("\r\nERR~\r\n");
    }

#ifdef FT_BT_UART
    if (ret == ERRCODE_SUCC) {
        at_cmd_send_abt_result_to_bt((uint8_t *)"\r\nOK~\r\n", strlen("\r\nOK~\r\n"));
    } else {
        at_cmd_send_abt_result_to_bt((uint8_t *)"\r\nERR~\r\n", strlen("\r\nERR~\r\n"));
    }
#endif
    osal_irq_restore(irq_sts);
}

#ifndef CONFIG_SUPPORT_MEDIA_MEMORY_MINI

static int32_t MediaAudioSceneInteraction001(const int32_t argc, const char **argv)
{
    PRINT("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    unused(argc);
    unused(argv);

    (void)AudioSceneInteraction001(0, NULL);

    PRINT("{%s():%d} func out success.\r\n", __FUNCTION__, __LINE__);
    return ERRCODE_SUCC;
}

static int32_t MediaAudioSceneInteraction002(const int32_t argc, const char **argv)
{
    PRINT("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    unused(argc);
    unused(argv);

    (void)AudioSceneInteraction002(0, NULL);

    PRINT("{%s():%d} func out success.\r\n", __FUNCTION__, __LINE__);
    return ERRCODE_SUCC;
}

static int32_t MediaAudioSceneInteraction003(const int32_t argc, const char **argv)
{
    PRINT("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    unused(argc);
    unused(argv);

    (void)AudioSceneInteraction003(0, NULL);

    PRINT("{%s():%d} func out success.\r\n", __FUNCTION__, __LINE__);
    return ERRCODE_SUCC;
}

static int32_t MediaAudioSceneInteraction004(const int32_t argc, const char **argv)
{
    PRINT("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    unused(argc);
    unused(argv);

    (void)AudioSceneInteraction004(0, NULL);

    PRINT("{%s():%d} func out success.\r\n", __FUNCTION__, __LINE__);
    return ERRCODE_SUCC;
}

static int32_t MediaAudioSceneInteraction005(const int32_t argc, const char **argv)
{
    PRINT("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    unused(argc);
    unused(argv);

    (void)AudioSceneInteraction005(0, NULL);

    PRINT("{%s():%d} func out success.\r\n", __FUNCTION__, __LINE__);
    return ERRCODE_SUCC;
}

static int32_t MediaAudioSceneInteraction006(const int32_t argc, const char **argv)
{
    PRINT("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    unused(argc);
    unused(argv);

    (void)AudioSceneInteraction006(0, NULL);

    PRINT("{%s():%d} func out success.\r\n", __FUNCTION__, __LINE__);
    return ERRCODE_SUCC;
}

static int32_t MediaAudioSceneInteraction007(const int32_t argc, const char **argv)
{
    PRINT("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    unused(argc);
    unused(argv);

    (void)AudioSceneInteraction007(0, NULL);

    PRINT("{%s():%d} func out success.\r\n", __FUNCTION__, __LINE__);
    return ERRCODE_SUCC;
}

static int32_t MediaA2dpSinkAef(const int32_t argc, const char **argv)
{
    PRINT("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    int32_t ret;
    char *endPtr;

    if (argc >= 1) {
        uint32_t enable = (uint32_t)strtoul((const char *)argv[0x0], &endPtr, NUMBER_BASE);
        if (*endPtr != '\0') {
            PRINT("{%s():%d} the number of parameters is incorrect.\r\n", __FUNCTION__, __LINE__);
            return -1;
        }
        ret = AudioManagerA2dpSinkSetAefEnable(0x190, enable);
    }
    if (argc == 0x2) {
        uint32_t beat = (uint32_t)strtoul((const char *)argv[0x1], &endPtr, NUMBER_BASE);
        if (*endPtr != '\0') {
            PRINT("{%s():%d} the number of parameters is incorrect.\r\n", __FUNCTION__, __LINE__);
            return -1;
        }
        ret = AudioManagerA2dpSinkSetAefParam(0x190, (char *)&beat, sizeof(beat));
    }
    if (argc < 1 || argc > 0x2) {
        PRINT("{%s():%d} the number of parameters is incorrect.\r\n", __FUNCTION__, __LINE__);
    }

    PRINT("{%s():%d} func out success.\r\n", __FUNCTION__, __LINE__);
    return ret;
}

#define MEDIA_MAX_PARAM_CNT 10

static int32_t MediaAudioManagerSetSeaLib(const int32_t argc, const char **argv)
{
    PRINT("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);
 
    int32_t ret = ERRCODE_FAIL;
 
    if (argc < 1) {
        return -1;
    }
    int32_t inPara = strtol((const char*)argv[0x0], NULL, 10);
    PRINT("{%s():%d} set lib type:%d.\r\n", __FUNCTION__, __LINE__, inPara);
    AudioManagerSetSeaLibId(inPara);
 
    PRINT("{%s():%d} func out success.\r\n", __FUNCTION__, __LINE__);
    return ret;
}

static int32_t MediaIntelligentVoiceProcess(const int32_t argc, const char **argv)
{
    PRINT("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);
    int32_t ret = IntelligentVoiceAsrEngineSample(argc, argv);
    PRINT("{%s():%d} func out success.\r\n", __FUNCTION__, __LINE__);
    return ret;
}
#endif

static int32_t MediaAudioManagerDumpInfo(const int32_t argc, const char **argv)
{
    PRINT("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    unused(argc);
    unused(argv);
    
    AudioManagerDumpInfo();
    
    PRINT("{%s():%d} func out success.\r\n", __FUNCTION__, __LINE__);
    return ERRCODE_SUCC;
}

static const media_cmd_process_type g_mediaAudioPlayFuncs[] = {
    { AUDIO_PLAY_START, PlayerSample },
    { AUDIO_PLAY_PAUSE, PlayerSample },
    { AUDIO_PLAY_RESUME, PlayerSample },
    { AUDIO_PLAY_STOP, PlayerSample },
#ifndef CONFIG_SUPPORT_MEDIA_MEMORY_MINI
    { AUDIO_PLAY_ALBUM_INFO, PlayerSample },
#endif
    { AUDIO_PLAY_SET_VOLUME, PlayerSample },
#ifndef CONFIG_SUPPORT_MEDIA_MEMORY_MINI
    { AUDIO_PLAY_GET_MUSIC_VOLUME, PlayerSample },
    { AUDIO_PLAY_SET_BT_SCO_VOLUME, PlayerSample },
    { AUDIO_PLAY_GET_BT_SCO_VOLUME, PlayerSample },
#endif
    { AUDIO_PLAY_SINGLE_LOOP, PlayerSample },
    { AUDIO_PLAY_SWITCH_SONG, PlayerSample },

    { AUDIO_PLAY_CURRENT_STATE, PlayerSample },
#ifndef CONFIG_SUPPORT_MEDIA_MEMORY_MINI
    { AUDIO_PLAY_MUTE, NULL },
#endif
    { NOTIFY_PLAY_START, TonePlayerSampleTest },
#ifndef CONFIG_SUPPORT_MEDIA_MEMORY_MINI
    { AUDIO_PLAY_SERVICE, AudioPlayerSampleTest },
#endif
};

#ifndef CONFIG_SUPPORT_MEDIA_MEMORY_MINI
static const media_cmd_process_type g_mediaAudioRecordFuncs[] = {
    { AUDIO_RECORD_START, AudioCaptureSample },
    { AUDIO_RECORD_STOP, AudioCaptureSample },
};

static const media_cmd_process_type g_mediaAudioSceneInteractionFuncs[] = {
    { AUDIO_SCENE_INTERACTION_001, MediaAudioSceneInteraction001 },
    { AUDIO_SCENE_INTERACTION_002, MediaAudioSceneInteraction002 },
    { AUDIO_SCENE_INTERACTION_003, MediaAudioSceneInteraction003 },
    { AUDIO_SCENE_INTERACTION_004, MediaAudioSceneInteraction004 },
    { AUDIO_SCENE_INTERACTION_005, MediaAudioSceneInteraction005 },
    { AUDIO_SCENE_INTERACTION_006, MediaAudioSceneInteraction006 },
    { AUDIO_SCENE_INTERACTION_007, MediaAudioSceneInteraction007 },
};

#define MEDIA_VIDEO_PLAY_AT
#ifdef MEDIA_VIDEO_PLAY_AT
static const media_cmd_process_type g_mediaVideoPlayFuncs[] = {
    { VIDEO_PLAY_START, VideoPlayerSample },
    { VIDEO_PLAY_PAUSE, VideoPlayerSample },
    { VIDEO_PLAY_RESUME, VideoPlayerSample },
    { VIDEO_PLAY_STOP, VideoPlayerSample },
    { VIDEO_PLAY_SINGLE_LOOP, VideoPlayerSample },
    { VIDEO_PLAY_SET_VOLUME, NULL },
    { VIDEO_PLAY_GET_VOLUME, NULL },
    { VIDEO_PLAY_CURRENT_STATE, NULL },
    { VIDEO_PLAY_MUTE, NULL },
};
#endif

#define MEDIA_CAMERA_PREVIEW_AT
#ifdef MEDIA_CAMERA_PREVIEW_AT
static const media_cmd_process_type g_mediaCameraPreviewFuncs[] = {
    { CAMERA_PREVIEW_START, CameraPreviewSample },
    { CAMERA_PREVIEW_STOP, CameraPreviewSample },
};
#endif

#ifndef CONFIG_SUPPORT_MEDIA_MEMORY_MINI
static const media_cmd_process_type g_mediaStreamSourceFuncs[] = {
    { STREAM_SOURCE_PLAY, StreamSourcePlayerSample },
};
#endif

#ifdef MEDIA_VOICE_CALL_VOLTE_AT
static const media_cmd_process_type g_mediaVoiceCallVolteFuncs[] = {
    { VOICE_CALL_VOLTE_HANDLE, VoiceCallVolteSample },
};
#endif
#endif

static const media_cmd_process_type g_mediaAudioMangerFuncs[] = {
#ifndef CONFIG_SUPPORT_MEDIA_MEMORY_MINI
    { A2DP_SINK_AEF, MediaA2dpSinkAef },
#endif
    { AUDIO_MANAGER_DUMP, MediaAudioManagerDumpInfo },
    { AUDIO_MANAGER_SEA_LIB, MediaAudioManagerSetSeaLib }
};

#ifndef CONFIG_SUPPORT_MEDIA_MEMORY_MINI
static const media_cmd_process_type g_mediaArsMangerFuncs[] = {
    { MEDIA_INTELLIGENT_VOICE, MediaIntelligentVoiceProcess },
};
#endif

static const MediaSceneProcessType g_mediaAudioFuncs[] = {
    { AUDIO_PLAY, (media_cmd_process_type*)g_mediaAudioPlayFuncs, ARRAY_COUNT(g_mediaAudioPlayFuncs)},
#ifndef CONFIG_SUPPORT_MEDIA_MEMORY_MINI
    { AUDIO_RECORD, (media_cmd_process_type*)g_mediaAudioRecordFuncs, ARRAY_COUNT(g_mediaAudioRecordFuncs) },
    { AUDIO_SCENE_INTERACTION, (media_cmd_process_type*)g_mediaAudioSceneInteractionFuncs,
        ARRAY_COUNT(g_mediaAudioSceneInteractionFuncs) },
#ifdef MEDIA_VIDEO_PLAY_AT
    { VIDEO_PLAY, (media_cmd_process_type*)g_mediaVideoPlayFuncs, ARRAY_COUNT(g_mediaVideoPlayFuncs) },
#endif
#ifdef MEDIA_CAMERA_PREVIEW_AT
    { CAMERA_PREVIEW, (media_cmd_process_type*)g_mediaCameraPreviewFuncs, ARRAY_COUNT(g_mediaCameraPreviewFuncs) },
#endif
#ifdef MEDIA_VOICE_CALL_VOLTE_AT
    { VOICE_CALL_VOLTE, (media_cmd_process_type*)g_mediaVoiceCallVolteFuncs,
        ARRAY_COUNT(g_mediaVoiceCallVolteFuncs) },
#endif
#endif
    { AUDIO_MANAGER, (media_cmd_process_type*)g_mediaAudioMangerFuncs, ARRAY_COUNT(g_mediaAudioMangerFuncs) },
#ifndef CONFIG_SUPPORT_MEDIA_MEMORY_MINI
    { MEDIA_INTELLIGENT_VOICE, (media_cmd_process_type *)g_mediaArsMangerFuncs, ARRAY_COUNT(g_mediaArsMangerFuncs) },
    { MEDIA_STREAM_SOURCE_PLAY, (media_cmd_process_type *)g_mediaStreamSourceFuncs, ARRAY_COUNT(g_mediaStreamSourceFuncs) },
#endif
};

void media_at_process(media_audio_scene scene, uint8_t cmd, const int32_t argc, const char **argv)
{
    PRINT("{%s():%d} scene:%d func in. cmd:%d\r\n", __FUNCTION__, __LINE__, scene, cmd);
    if (argv == NULL) {
        PRINT("{%s():%d} input data is NULL.\r\n", __FUNCTION__, __LINE__);
        return;
    }

    uint8_t audio_scene;
    int32_t ret = ERRCODE_FAIL;

    audio_scene = scene;
    media_cmd_process_type *cmdProcessMap = NULL;
    uint32_t cmdProcessMapSize = 0;
    for (uint32_t i = 0; i < ARRAY_COUNT(g_mediaAudioFuncs); i++) {
        if (audio_scene == g_mediaAudioFuncs[i].id) {
            cmdProcessMap = g_mediaAudioFuncs[i].funcMap;
            cmdProcessMapSize = g_mediaAudioFuncs[i].funcMapSize;
        }
    }

    if (cmdProcessMap == NULL || cmdProcessMapSize <= 0) {
        PRINT("Unsupport scene : %d", scene);
    }

    for (uint32_t i = 0; i < cmdProcessMapSize; i++) {
        if (cmd == cmdProcessMap[i].id) {
            if (cmdProcessMap[i].func == NULL) {
                PRINT("Unsupport cmd : %u, scene : %x", cmdProcessMap[i].id, audio_scene);
                return;
            }
            ret = cmdProcessMap[i].func(argc, argv);
            print_at_cmd_ret(ret);
        }
    }

    // phone remote control, need reply to spp
    if (g_media_diag_flag) {
        uint8_t reply_data[MEDIA_APP_REPLY_DATA_LEN] = {0};
        reply_data[0x0] = 0x8C;
        reply_data[0x1] = 0x82;
        reply_data[0x2] = 0;
        reply_data[0x3] = (uint8_t)ret;
        (void)send_spp_diag_msg(g_media_diag_flag, &g_media_diag_header, reply_data, MEDIA_APP_REPLY_DATA_LEN);
    }
    (void)ret;
    g_media_diag_flag = false;
    PRINT("{%s():%d} func out %s.\r\n", __FUNCTION__, __LINE__, (ret == ERRCODE_SUCC) ? "success" : "failure");
}
