/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: intelligent_voice_service.cpp
 * Create: 2025-11-26
 */

#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include "unistd.h"
#include "securec.h"
#include "audio_manager.h"
#include "audio_base_type.h"
#include "media_log.h"
#include "media_thread_adapt.h"
#include "AppViewIDs.h"
#include "soc_osal.h"
#include "cmsis_os.h"
#include "wearable_log.h"
#include "intelligent_voice_asr_engine.h"
#include "NativeAbility.h"
#include "intelligent_app_control.h"
#include "intelligent_music_control.h"
#include "intelligent_timer_control.h"
#include "intelligent_alarm_control.h"
#include "intelligent_device_control.h"
#include "intelligent/IntelligentVoiceView.h"
#include "intelligent_voice_tts_engine.h"
#include "utf8_to_gbk.h"
#include "intelligent_voice_service.h"
#ifdef SUPPORT_POWER_MANAGER
#include "power_display_service.h"
#endif

namespace OHOS {
#ifdef __cplusplus
extern "C" {
#endif

#define INTELLIGENT_VOICE_SCREEN_ON_TIME 0xFFFFFFFF
#define INTELLIGENT_VOICE_SCREEN_OFF_TIME 15000

osMessageQueueId_t g_intelligent_voice_queue = nullptr;
static IntelligentVoiceServiceContext g_intelligentVoiceCtx = {};

static errcode_t SetToAsrMode(void);
static errcode_t SetToWakeUpMode(void);

static errcode_t IntelligentVoiceMsgQueueInit(void)
{
    osMessageQueueAttr_t m_attr = {0};
    m_attr.name = "INTELLIGENT_VOICE_MSG";
    g_intelligent_voice_queue = osMessageQueueNew(MSG_QUEUE_LENGTH, sizeof(IntelligentEngineCallbackInfo), &m_attr);
    if (g_intelligent_voice_queue == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "create msg queue fail\r\n");
        return ERRCODE_FAIL;
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "create msg queue succ\r\n");
    return ERRCODE_SUCC;
}

static void IntelligentVoiceMsgQueueDel(void)
{
    osStatus_t ret;
    ret = osMessageQueueDelete(g_intelligent_voice_queue);
    if (ret != osOK) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "del pcm_stream queue fail, ret = %u\r\n", ret);
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "del pcm_stream queue succ.\r\n");
    return;
}

static void DispatchTtsEvent(TtsEngineEvent eventType, const void *data, uint32_t length)
{
    switch (eventType) {
        case TTS_ENGINE_EVENT_SPEAK_BEGIN:
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_MAIN_SERVICE, "[DispatchTtsEvent] begin");
            break;
        case TTS_ENGINE_EVENT_SPEAK_PLAY_COMPLETE:
            IntelligentVoiceTTSDeinit();
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_MAIN_SERVICE, "[DispatchTtsEvent] complete");
            break;
        case TTS_ENGINE_EVENT_SPEAK_END:
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_MAIN_SERVICE, "[DispatchTtsEvent] end");
            IntelligentVoiceTTSDeinit();
            break;
        case TTS_ENGINE_EVENT_SPEAK_ERROR:
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "[DispatchTtsEvent] error!");
            IntelligentVoiceTTSDeinit();
            break;
        default:
            IntelligentVoiceTTSDeinit();
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_MAIN_SERVICE, "[DispatchTtsEvent] not support type[%d] !", eventType);
            break;
    }
}

static void TtsEngineListener(TtsEngineEvent eventType, const void *data, uint32_t length)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "[TtsEngineListener] eventType =%d \r\n", eventType);
    IntelligentEngineCallbackInfo msg;
    msg.voiceType = TTS;
    msg.ttsEventType = eventType;
    msg.data = malloc(length);
    memcpy_s(msg.data, length, data, length);
    msg.length = length;
    if (g_intelligent_voice_queue != nullptr) {
        osStatus_t ret = osMessageQueuePut(g_intelligent_voice_queue, &msg, 0, 0);
        if (ret != osOK) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "[TtsEngineListener]Fatal err, ret = %d\r\n", ret);
        }
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_MAIN_SERVICE, "[TtsEngineListener]active send pcm end\r\n");
    }
}

void IntelligentVoiceTTSInit(void)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, " [IntelligentVoiceTTSInit]\r\n");
    (void)IntellVoiceTtsEngineInit(&g_intelligentVoiceCtx.ttsInitParams);
}

void IntelligentVoiceTTSDeinit(void)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, " [IntelligentVoiceTTSDeinit]\r\n");

    IntellVoiceTtsEngineDeInit();
}

void IntelligentVoiceTTSStop(void)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, " [IntelligentVoiceTTSStop]\r\n");

    IntellVoiceTtsEngineStop();
}

void IntelligentVoiceTTSSpeak(const char *textString)
{
    IntelligentVoiceTTSInit();
    char *speakWords = nullptr;
    ConvertUtf8ToGbk(textString, &speakWords);
    const char *targetWords = speakWords;
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "[IntelligentVoiceTTSSpeak] words %s\r\n", targetWords);
    IntellVoiceTtsEngineTextSpeak(0, targetWords, strlen(targetWords));
    free(speakWords);
}

static void DealWithAsrVoiceCommand(const AsrEngineCmdInfo *info)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "[DealWithAsrVoiceCommand] cmdid =%d \r\n", info->cmdId);
    switch (info->cmdId) {
        case ASR_ENGINE_CMD_APP_CONTROL:
            SetToWakeUpMode();                 // Ä£ºýÃüÁî´ÊÊ¶±ðÖ®ºóÇÐ»»µ½¹Ì¶¨ÃüÁî´Ê
            IntelligentAppEventProcess(info);  // ¾ßÌå²Ù×÷
            break;
        case ASR_ENGINE_CMD_MUSIC_CONTROL:
            IntelligentVoiceTTSDeinit();  // Ð¡ÒÕÐ¡ÒÕ»½ÐÑºó¹Ø±ÕTTS,±ÜÃâ²¢·¢
            SetToWakeUpMode();  // Ä£ºýÃüÁî´ÊÊ¶±ðÖ®ºóÇÐ»»µ½¹Ì¶¨ÃüÁî´Ê
            IntelligentMusicEventProcess(info);
            break;
        case ASR_ENGINE_CMD_ALARM_CONTROL:
            SetToWakeUpMode();  // Ä£ºýÃüÁî´ÊÊ¶±ðÖ®ºóÇÐ»»µ½¹Ì¶¨ÃüÁî´Ê
            IntelligentAlarmEventProcess(info);
            break;
        case ASR_ENGINE_CMD_MOTION_CONTROL:
            SetToWakeUpMode();  // Ä£ºýÃüÁî´ÊÊ¶±ðÖ®ºóÇÐ»»µ½¹Ì¶¨ÃüÁî´Ê
            break;
        case ASR_ENGINE_CMD_TELEPHONE_CONTROL:
            SetToWakeUpMode();  // Ä£ºýÃüÁî´ÊÊ¶±ðÖ®ºóÇÐ»»µ½¹Ì¶¨ÃüÁî´Ê
            break;
        case ASR_ENGINE_CMD_TIMER_CONTROL:
            SetToWakeUpMode();  // Ä£ºýÃüÁî´ÊÊ¶±ðÖ®ºóÇÐ»»µ½¹Ì¶¨ÃüÁî´Ê
            IntelligentTimerEventProcess(info);
            break;
        case ASR_ENGINE_CMD_DEVICE_CONTROL:
            IntelligentVoiceTTSDeinit();
            SetToWakeUpMode();  // Ä£ºýÃüÁî´ÊÊ¶±ðÖ®ºóÇÐ»»µ½¹Ì¶¨ÃüÁî´Ê
            IntelligentDeviceEventProcess(info);
            break;
        case ASR_ENGINE_CMD_WAKEUP_KEYWORD: {
#ifdef SUPPORT_POWER_MANAGER
            const power_display_svr_api_t *display_api = power_display_svr_get_api();
            display_api->set_screen_auto_off_timeout(INTELLIGENT_VOICE_SCREEN_ON_TIME);
            display_api->turn_on_screen();
#endif
            IntelligentVoiceTTSDeinit();  // Ð¡ÒÕÐ¡ÒÕ»½ÐÑºó¹Ø±ÕTTS,±ÜÃâ²¢·¢
            SetToAsrMode();               // ¹Ì¶¨ÃüÁî´ÊÊ¶±ðºóÇÐ»»µ½Ä£ºýÃüÁî´Ê
            break;
        }
        default:
            SetToWakeUpMode();
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "[DealWithAsrVoiceCommand] Unknown command id");
            break;
    }
}

static void PrintfResult(const AsrEngineCmdInfo *info)
{
    switch (info->cmdId) {
        case ASR_ENGINE_CMD_APP_CONTROL:
            MEDIA_INFO_LOG("[PrintfResult] APP, result appId = %d, appType = %d",
                info->appControlInfo.appId,
                info->appControlInfo.appControlType);
            break;
        case ASR_ENGINE_CMD_MUSIC_CONTROL:
            MEDIA_INFO_LOG(
                "[PrintfResult] MUSIC, result musicControlType = %d", info->musicControlInfo.musicControlType);
            break;
        case ASR_ENGINE_CMD_ALARM_CONTROL:
            MEDIA_INFO_LOG("[PrintfResult] ALARM, result alarmControlType = %d, day = %d, timePeriod = %d, "
                           "hour = %d, minute = %d",
                info->alarmControlInfo.alarmControlType,
                info->alarmControlInfo.alarmDateTime.day,
                info->alarmControlInfo.alarmDateTime.timePeriod,
                info->alarmControlInfo.alarmDateTime.hour,
                info->alarmControlInfo.alarmDateTime.minute);
            break;
        case ASR_ENGINE_CMD_MOTION_CONTROL:
            MEDIA_INFO_LOG("[PrintfResult] MOTION, result motionControlType = %d, motionTarget = %d, value = %f",
                info->motionControlInfo.motionControlType,
                info->motionControlInfo.motionTarget,
                info->motionControlInfo.value);
            break;
        case ASR_ENGINE_CMD_TELEPHONE_CONTROL:
            MEDIA_INFO_LOG("[PrintfResult] TELEPHONE, result telephoneControlType = %d, telnum = %d",
                info->telephoneControlInfo.telephoneControlType,
                info->telephoneControlInfo.telnum);
            break;
        case ASR_ENGINE_CMD_TIMER_CONTROL:
            MEDIA_INFO_LOG("[PrintfResult] TIMER, result timerControlType = %d, hour = %d, minute = %d, second = %d",
                info->timerControlInfo.timerControlType,
                info->timerControlInfo.hour,
                info->timerControlInfo.minute,
                info->timerControlInfo.second);
            break;
        case ASR_ENGINE_CMD_DEVICE_CONTROL:
            MEDIA_INFO_LOG(
                "[PrintfResult] DEVICE, result deviceControlType = %d", info->deviceControlInfo.deviceControlType);
            break;
        case ASR_ENGINE_CMD_WAKEUP_KEYWORD:
            MEDIA_INFO_LOG("[PrintfResult] WAKE UP, result wakeupKeyword = %d", info->wakeupKeywordInfo.wakeupKeyword);
            break;
        default:
            MEDIA_INFO_LOG("Unknown identification failed!");
            break;
    }
}

static void DispatchAsrEvent(AsrEngineEvent eventType, void *data, uint32_t length)
{
    switch (eventType) {
        case ASR_ENGINE_EVENT_STOP:
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "[DispatchAsrEvent] stop");
            SetToWakeUpMode();
            break;
        case ASR_ENGINE_EVENT_VOICE_COMMAND:
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "[DispatchAsrEvent] command");
            if (data == nullptr) {
                WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "[DispatchAsrEvent] comand data error");
                SetToWakeUpMode();
                return;
            }
            if (length != sizeof(AsrEngineCmdInfo)) {
                WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "[DispatchAsrEvent] comand length error");
                SetToWakeUpMode();
                return;
            }
            PrintfResult((const AsrEngineCmdInfo *)data);
            DealWithAsrVoiceCommand((const AsrEngineCmdInfo *)data);
            break;
        default:
            SetToWakeUpMode();
            WEARABLE_LOGE(
                WEARABLE_LOG_MODULE_MAIN_SERVICE, "[DispatchAsrEvent] not support eventType[%d] !", eventType);
            break;
    }
}

static void AsrEngineListener(AsrEngineEvent eventType, void *data, uint32_t length)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, " [AsrEngineListener] eventType =%d \r\n", eventType);
    IntelligentEngineCallbackInfo msg;
    msg.voiceType = ASR;
    msg.asrEventType = eventType;
    msg.data = malloc(length);
    memcpy_s(msg.data, length, data, length);
    msg.length = length;
    if (g_intelligent_voice_queue != nullptr) {
        osStatus_t ret = osMessageQueuePut(g_intelligent_voice_queue, &msg, 0, 0);
        if (ret != osOK) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "[AsrEngineListener]Fatal err, ret = %d\r\n", ret);
        }
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_MAIN_SERVICE, "[AsrEngineListener]r\n");
    }
}

static errcode_t SetToAsrMode(void)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "[SetToAsrMode]/r/n");
    MediaMutexLock(g_intelligentVoiceCtx.mutex);
    (void)IntellVoiceAsrEngineStopRecognition();
    (void)IntellVoiceAsrEngineDeInit();
    g_intelligentVoiceCtx.initParams.enableNpu = true;
    int32_t ret = IntellVoiceAsrEngineInit(&g_intelligentVoiceCtx.initParams);
    if (ret != MEDIA_OK) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "[SetToAsrMode] init fail");
        MediaMutexUnLock(g_intelligentVoiceCtx.mutex);
        return ERRCODE_FAIL;
    }
    ret = IntellVoiceAsrEngineStartRecognition();
    if (ret != MEDIA_OK) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "[SetToAsrMode] start fail");
        (void)IntellVoiceAsrEngineDeInit();
        MediaMutexUnLock(g_intelligentVoiceCtx.mutex);
        return ERRCODE_FAIL;
    }
    MediaMutexUnLock(g_intelligentVoiceCtx.mutex);
    IntelligentVoiceView::GetInstance()->ShowView();
    return ERRCODE_SUCC;
}

static errcode_t SetToWakeUpMode(void)
{
#ifdef SUPPORT_POWER_MANAGER
    const power_display_svr_api_t *display_api = power_display_svr_get_api();
    display_api->set_screen_auto_off_timeout(INTELLIGENT_VOICE_SCREEN_OFF_TIME);
    display_api->turn_on_screen();
#endif
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "[SetToWakeUpMode]/r/n");
    MediaMutexLock(g_intelligentVoiceCtx.mutex);
    (void)IntellVoiceAsrEngineStopRecognition();
    (void)IntellVoiceAsrEngineDeInit();
    g_intelligentVoiceCtx.initParams.enableNpu = false;
    int32_t ret = IntellVoiceAsrEngineInit(&g_intelligentVoiceCtx.initParams);
    if (ret != MEDIA_OK) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "[SetToWakeUpMode] init fail/r/n");
        MediaMutexUnLock(g_intelligentVoiceCtx.mutex);
        return ERRCODE_FAIL;
    }
    ret = IntellVoiceAsrEngineStartRecognition();
    if (ret != MEDIA_OK) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "[SetToWakeUpMode] start fail/r/n");
        (void)IntellVoiceAsrEngineDeInit();
        MediaMutexUnLock(g_intelligentVoiceCtx.mutex);
        return ERRCODE_FAIL;
    }
    MediaMutexUnLock(g_intelligentVoiceCtx.mutex);
    IntelligentVoiceView::GetInstance()->ExitView();
    return ERRCODE_SUCC;
}

static void *IntellVoiceServiceThread(void *priv)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "[IntellVoiceServiceThread] start\r\n");
    IntelligentVoiceServiceContext *asrContext = (IntelligentVoiceServiceContext *)priv;
    MediaMutexLock(asrContext->mutex);
    int32_t ret = IntellVoiceAsrEngineInit(&asrContext->initParams);
    if (ret != MEDIA_OK) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "[IntellVoiceServiceThread] asr init failed!\r\n");
        MediaMutexUnLock(asrContext->mutex);
        return nullptr;
    }
    ret = IntellVoiceAsrEngineStartRecognition();
    if (ret != MEDIA_OK) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "[IntellVoiceServiceThread] asr start failed!\r\n");
        (void)IntellVoiceAsrEngineDeInit();
        return nullptr;
    }

    MediaMutexUnLock(asrContext->mutex);
    // ´¦ÀíÀ´×ÔAsrEngineµÄ»Øµ÷º¯Êý·¢¹ýÀ´µÄÊÂ¼þ
    IntelligentEngineCallbackInfo msg;
    while (true) {
        osStatus_t ret = osMessageQueueGet(g_intelligent_voice_queue, &msg, nullptr, osWaitForever);
        if (ret != osOK) {
            continue;
        }
        if (msg.voiceType == ASR) {
            DispatchAsrEvent(msg.asrEventType, msg.data, msg.length);
        } else if (msg.voiceType == TTS) {
            DispatchTtsEvent(msg.ttsEventType, msg.data, msg.length);
        } else if (msg.voiceType == STOP) {
            IntelligentVoiceView::GetInstance()->ExitView();
            IntelligentVoiceSerivicePause();
            IntelligentVoiceTTSDeinit();
            MediaMutexDestroy(&g_intelligentVoiceCtx.mutex);
            IntelligentVoiceMsgQueueDel();
            g_intelligentVoiceCtx.start = false;
            break;
        }
        free(msg.data);
    }
    WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "[IntellVoiceServiceThread] thread exit.\r\n");
    return nullptr;
}

// Òòµç»°»òÕßÔÚÏßÒôÀÖ´ò¶Ïµ¼ÖÂÓïÒôÊ¶±ð¹¦ÄÜ¹Ø±Õ»òÕßÃðÆÁµÄÊ±ºò¹Ø±ÕÓïÑÔÊ¶±ð
errcode_t IntelligentVoiceSerivicePause(void)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "[IntelligentVoiceSerivicePause]\r\n");
    (void)IntellVoiceAsrEngineStopRecognition();
    (void)IntellVoiceAsrEngineDeInit();
    return ERRCODE_SUCC;
}

// »Ö¸´ÓïÒôÊ¶±ð¹¦ÄÜ£¬ÏÈ»Ö¸´¹Ì¶¨ÃüÁî´Ê£¬ÁÁÆÁµÄÊ±ºò»Ö¸´ÓïÒôÊ¶±ð£¬»òÕßÔÚÁÁÆÁÎÞ²¢·¢ÒµÎñÊ±ºò»Ö¸´ÓïÒôÊ¶±ð
errcode_t IntelligentVoiceSeriviceResume(void)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "[IntelligentVoiceSeriviceResume]\r\n");
    MediaMutexLock(g_intelligentVoiceCtx.mutex);
    g_intelligentVoiceCtx.initParams.enableNpu = false;
    int32_t ret = IntellVoiceAsrEngineInit(&g_intelligentVoiceCtx.initParams);
    if (ret != MEDIA_OK) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "[IntelligentVoiceSeriviceResume] asr init ret=%x\r\n", ret);
        MediaMutexUnLock(g_intelligentVoiceCtx.mutex);
        return ERRCODE_FAIL;
    }
    ret = IntellVoiceAsrEngineStartRecognition();
    if (ret != MEDIA_OK) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "[IntelligentVoiceSeriviceResume] wake up failed!\r\n");
        (void)IntellVoiceAsrEngineDeInit();
        MediaMutexUnLock(g_intelligentVoiceCtx.mutex);
        return ERRCODE_FAIL;
    }
    MediaMutexUnLock(g_intelligentVoiceCtx.mutex);
    return ERRCODE_SUCC;
}

errcode_t IntelligentVoiceSeriviceStart(void)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "[IntelligentVoiceSeriviceStart] start\r\n");
    if (g_intelligentVoiceCtx.start) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "[IntelligentVoiceSeriviceStart] already start\r\n");
        return ERRCODE_FAIL;
    }
    g_intelligentVoiceCtx.start = true;
    g_intelligentVoiceCtx.initParams.enableNpu = false;  // ÓÅÏÈ¹Ì¶¨ÃüÁî´Ê
    g_intelligentVoiceCtx.initParams.language = ASR_ENGINE_CHINESE;
    g_intelligentVoiceCtx.initParams.workMode = ASR_ENGINE_ALWAYS_WAKE;
    g_intelligentVoiceCtx.initParams.listener = AsrEngineListener;
    g_intelligentVoiceCtx.ttsInitParams.listener = TtsEngineListener;
    g_intelligentVoiceCtx.mutex = MediaMutexCreate(nullptr);
    if (g_intelligentVoiceCtx.mutex == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "[IntelligentVoiceSeriviceStart] mutex error\r\n");
        g_intelligentVoiceCtx.start = false;
        return ERRCODE_FAIL;
    }

    errcode_t ret = IntelligentVoiceMsgQueueInit();
    if (ret != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "[IntelligentVoiceSeriviceStart] queque error\r\n");
        MediaMutexDestroy(&g_intelligentVoiceCtx.mutex);
        g_intelligentVoiceCtx.start = false;
        return ERRCODE_FAIL;
    }
    MediaThreadattr attr = {"IntellVoiceTestThread", 0x2000, THREAD_SCHED_INVALID, 0, false};
    g_intelligentVoiceCtx.taskHandle = MediaThreadCreate(IntellVoiceServiceThread, &g_intelligentVoiceCtx, &attr);
    if (g_intelligentVoiceCtx.taskHandle == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "[IntelligentVoiceSeriviceStart] thread error\r\n");
        MediaMutexDestroy(&g_intelligentVoiceCtx.mutex);
        IntelligentVoiceMsgQueueDel();
        g_intelligentVoiceCtx.start = false;
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

errcode_t IntelligentVoiceSeriviceStop(void)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "[IntelligentVoiceSeriviceStop] start\r\n");
    IntelligentEngineCallbackInfo msg;
    msg.voiceType = STOP;
    if (g_intelligent_voice_queue != nullptr) {
        osStatus_t ret = osMessageQueuePut(g_intelligent_voice_queue, &msg, 0, 0);
        if (ret != osOK) {
            WEARABLE_LOGE(
                WEARABLE_LOG_MODULE_MAIN_SERVICE, "[IntelligentVoiceSeriviceStop] Fatal err, ret = %d\r\n", ret);
            return ERRCODE_FAIL;
        }
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_MAIN_SERVICE, "[IntelligentVoiceSeriviceStop] end\r\n");
    }
    return ERRCODE_SUCC;
}
#ifdef __cplusplus
}
#endif
}  // namespace OHOS