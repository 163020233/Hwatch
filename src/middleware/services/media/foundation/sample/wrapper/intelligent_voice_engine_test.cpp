/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2026. All rights reserved.
 * Description: intelligent_voice_engine_test
 * Author: Media Software Group
 * Create: 2025-05-14
 */

#include "intelligent_voice_engine_test.h"
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include "securec.h"
#include "audio_manager.h"
#include "audio_base_type.h"
#include "media_log.h"
#include "media_thread_adapt.h"
#include "intelligent_voice_asr_engine.h"
#include "intelligent_voice_tts_engine.h"
#include "media_mem.h"
#include "audio_stream_out.h"

#ifdef __cplusplus
extern "C" {
#endif
typedef int32_t (*InvokeFunc)(int32_t argc, const char* argv[]);

typedef struct {
    const char *cmdName;
    InvokeFunc invoke;
} IntelligentVoiceTestInvoke;

typedef struct {
    MediaMutexHandle mutex;
    MediaThreadCondHandle cond;
    MediaThreadIdHandle taskHandle;
    AsrEngineInitParams initParams;
    bool start;
    bool threadStopped;
    bool isWakeUpNpu;
} IntelligentVoiceContext;

typedef struct {
    MediaMutexHandle mutex;
    MediaThreadCondHandle cond;
    MediaThreadIdHandle taskHandle;
    TtsEngineInitParams initParams;
    FILE *pfd;
    char *textBuf;
    uint32_t textId;
    bool start;
    bool active;
    uint32_t sleepTime;
} IntelligentVoiceTtsContext;

const uint32_t MAX_TEXT_ONE_BUF_SIZE = 1024;
const uint32_t DEFAULT_SLEEP_TIME = 10000000;
const uint32_t DEFAULT_READ_COMPLETE_SLEEP_TIME = 100000;

static IntelligentVoiceContext g_intelligentVoiceCtx = {};
static IntelligentVoiceTtsContext g_intelligentVoiceTtsCtx = {};

static void PrintfResult(const AsrEngineCmdInfo *info)
{
    switch (info->cmdId) {
        case ASR_ENGINE_CMD_APP_CONTROL:
            MEDIA_INFO_LOG("ASR_ENGINE_CMD_APP_CONTROL, result appId = %d, appType = %d",
                info->appControlInfo.appId, info->appControlInfo.appControlType);
            break;
        case ASR_ENGINE_CMD_MUSIC_CONTROL:
            MEDIA_INFO_LOG("ASR_ENGINE_CMD_MUSIC_CONTROL, result musicControlType = %d",
                info->musicControlInfo.musicControlType);
            break;
        case ASR_ENGINE_CMD_ALARM_CONTROL:
            MEDIA_INFO_LOG(
                "ASR_ENGINE_CMD_ALARM_CONTROL, result alarmControlType = %d, day = %d, timePeriod = %d, hour = %d, minute = %d",
                info->alarmControlInfo.alarmControlType, info->alarmControlInfo.alarmDateTime.day,
                info->alarmControlInfo.alarmDateTime.timePeriod, info->alarmControlInfo.alarmDateTime.hour,
                info->alarmControlInfo.alarmDateTime.minute);
            break;
        case ASR_ENGINE_CMD_MOTION_CONTROL:
            MEDIA_INFO_LOG("ASR_ENGINE_CMD_MOTION_CONTROL, result motionControlType = %d, motionTarget = %d, value = %f",
                info->motionControlInfo.motionControlType, info->motionControlInfo.motionTarget, info->motionControlInfo.value);
            break;
        case ASR_ENGINE_CMD_TELEPHONE_CONTROL:
            MEDIA_INFO_LOG("ASR_ENGINE_CMD_TELEPHONE_CONTROL, result telephoneControlType = %d, telnum = %d",
                info->telephoneControlInfo.telephoneControlType, info->telephoneControlInfo.telnum);
            break;
        case ASR_ENGINE_CMD_TIMER_CONTROL:
            MEDIA_INFO_LOG("ASR_ENGINE_CMD_TIMER_CONTROL, result timerControlType = %d, hour = %d, minute = %d, second = %d",
                info->timerControlInfo.timerControlType, info->timerControlInfo.hour, info->timerControlInfo.minute,
                info->timerControlInfo.second);
            break;
        case ASR_ENGINE_CMD_DEVICE_CONTROL:
            MEDIA_INFO_LOG("ASR_ENGINE_CMD_DEVICE_CONTROL, result deviceControlType = %d",
                info->deviceControlInfo.deviceControlType);
            break;
        case ASR_ENGINE_CMD_WAKEUP_KEYWORD:
            MEDIA_INFO_LOG("ASR_ENGINE_CMD_WAKEUP_KEYWORD, result wakeupKeyword = %d",
                info->wakeupKeywordInfo.wakeupKeyword);
            if (info->wakeupKeywordInfo.wakeupKeyword == ASR_ENGINE_WAKEUP_KEYWORD_XIAO_YI &&
                !g_intelligentVoiceCtx.initParams.enableNpu) {
                MediaMutexLock(g_intelligentVoiceCtx.mutex);
                g_intelligentVoiceCtx.isWakeUpNpu = true;
                MediaMutexUnLock(g_intelligentVoiceCtx.mutex);
                MediaThreadCondSignal(g_intelligentVoiceCtx.cond);
            }
            break;
        default:
            MEDIA_INFO_LOG("Unknown identification failed!");
            break;
    }
}

static void TtsEngineListenerTest(TtsEngineEvent eventType, const void *data, uint32_t length)
{
    switch (eventType) {
        case TTS_ENGINE_EVENT_SPEAK_BEGIN:
            MEDIA_INFO_LOG("TTS_ENGINE_EVENT_SPEAK_BEGIN !");
            break;
        case TTS_ENGINE_EVENT_SPEAK_PLAY_COMPLETE: {
            if (data == nullptr || length == 0) {
                MEDIA_INFO_LOG("data is NULL!");
                return;
            }
            int32_t textId = (intptr_t)data;
            MEDIA_INFO_LOG("TTS_ENGINE_EVENT_SPEAK_PLAY_COMPLETE !textId = %d", textId);
            break;
        }
        case TTS_ENGINE_EVENT_SPEAK_END:
            MEDIA_INFO_LOG("TTS_ENGINE_EVENT_SPEAK_END !");
            break;
        case TTS_ENGINE_EVENT_SPEAK_ERROR:
            MEDIA_INFO_LOG("TTS_ENGINE_EVENT_SPEAK_ERROR !");
            break;
        default:
            MEDIA_INFO_LOG("not support eventType[%d] !", eventType);
            break;
    }
}

static void AsrEngineListenerTest(AsrEngineEvent eventType, void *data, uint32_t length)
{
    switch (eventType) {
        case ASR_ENGINE_EVENT_STOP:
            MEDIA_INFO_LOG("ASR_ENGINE_EVENT_STOP !");
            break;
        case ASR_ENGINE_EVENT_VOICE_COMMAND:
            MEDIA_INFO_LOG("ASR_ENGINE_EVENT_VOICE_COMMAND !");
            if (data == nullptr) {
                MEDIA_INFO_LOG("data is null!");
                return;
            }
            if (length != sizeof(AsrEngineCmdInfo)) {
                MEDIA_INFO_LOG("length is not right!");
                return;
            }
            PrintfResult((const AsrEngineCmdInfo *)data);
            break;
        case ASR_ENGINE_EVENT_VOICE_BEGIN:
            MEDIA_INFO_LOG("ASR_ENGINE_EVENT_VOICE_BEGIN !");
            break;
        case ASR_ENGINE_EVENT_VOICE_END:
            MEDIA_INFO_LOG("ASR_ENGINE_EVENT_VOICE_END !");
            break;
        case ASR_ENGINE_EVENT_VOICE_ERROR:
            MEDIA_ERR_LOG("ASR_ENGINE_EVENT_VOICE_ERROR !");
            break;
        default:
            MEDIA_INFO_LOG("not support eventType[%d] !", eventType);
            break;
    }
}

static int32_t ProcessAsrInterrupt(IntelligentVoiceContext *asrContext)
{
    if (!asrContext->isWakeUpNpu) {
        return MEDIA_OK;
    }
    (void)IntellVoiceAsrEngineStopRecognition();
    (void)IntellVoiceAsrEngineDeInit();
    asrContext->initParams.enableNpu = true;
    int32_t ret = IntellVoiceAsrEngineInit(&asrContext->initParams);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("IntellVoiceAsrEngineInit failed!");
        return MEDIA_ERR;
    }
    ret = IntellVoiceAsrEngineStartRecognition();
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("IntellVoiceAsrEngineStartRecognition failed!");
        (void)IntellVoiceAsrEngineDeInit();
        return MEDIA_ERR;
    }
    return MEDIA_OK;
}

static void *IntellVoiceTestThread(void *priv)
{
    IntelligentVoiceContext *asrContext = (IntelligentVoiceContext *)priv;
    MediaMutexLock(asrContext->mutex);
    int32_t ret = IntellVoiceAsrEngineInit(&asrContext->initParams);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("IntellVoiceAsrEngineInit failed!");
        MediaMutexUnLock(asrContext->mutex);
        return nullptr;
    }
    ret = IntellVoiceAsrEngineStartRecognition();
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("IntellVoiceAsrEngineStartRecognition failed!");
        (void)IntellVoiceAsrEngineDeInit();
        return nullptr;
    }
    while (true) {
        if (asrContext->threadStopped) {
            (void)IntellVoiceAsrEngineStopRecognition();
            (void)IntellVoiceAsrEngineDeInit();
            break;
        }
        MediaThreadCondWait(asrContext->cond, asrContext->mutex);
        ret = ProcessAsrInterrupt(asrContext);
        if (ret != MEDIA_OK) {
            break;
        }
    }
    MediaMutexUnLock(asrContext->mutex);
    MEDIA_INFO_LOG("IntellVoiceTestThread exit.");
    return nullptr;
}

static void IntelligentVoiceTtsDestoryThread(IntelligentVoiceTtsContext *ttsContext)
{
    if (ttsContext->textBuf != nullptr) {
        MediaMemFree(ttsContext->textBuf);
        ttsContext->textBuf = nullptr;
    }
    if (ttsContext->pfd != nullptr) {
        fclose(ttsContext->pfd);
        ttsContext->pfd = nullptr;
    }
}

static int32_t HandleFreadEmpty(IntelligentVoiceTtsContext *ttsContext)
{
    if (ferror(ttsContext->pfd)) {
        MEDIA_ERR_LOG("read text file data failed! error = %d", errno);
        return MEDIA_ERR;
    }
    if (feof(ttsContext->pfd)) {
        fclose(ttsContext->pfd);
        ttsContext->pfd = nullptr;
        ttsContext->sleepTime = DEFAULT_READ_COMPLETE_SLEEP_TIME;
    }
    return MEDIA_OK;
}

static void *IntellVoiceTtsThread(void *priv)
{
    IntelligentVoiceTtsContext *ttsContext = (IntelligentVoiceTtsContext *)priv;
    uint32_t readLen = 0;
    int32_t ret = 0;
    ttsContext->textBuf = (char *)MediaMemCalloc(MAX_TEXT_ONE_BUF_SIZE);
    if (ttsContext->textBuf == nullptr) {
        MEDIA_ERR_LOG("malloc textBuf failed!");
        goto EXIT;
    }
    while (true) {
        MediaMutexLock(ttsContext->mutex);
        ttsContext->sleepTime = 0;
        if (!ttsContext->active) {
            break;
        }
        if (ttsContext->pfd == nullptr) {
            ttsContext->sleepTime = DEFAULT_READ_COMPLETE_SLEEP_TIME;
            goto CONTINUE;
        }
        readLen = fread(ttsContext->textBuf, 1, MAX_TEXT_ONE_BUF_SIZE, ttsContext->pfd);
        if (readLen == 0) {
            if (HandleFreadEmpty(ttsContext) != MEDIA_OK) {
                break;
            } else {
                goto CONTINUE;
            }
        }
        ttsContext->textId++;
        MEDIA_INFO_LOG("fread data read size = %d!, textId = %d", readLen, ttsContext->textId);
        ret = IntellVoiceTtsEngineTextSpeak(ttsContext->textId, ttsContext->textBuf, readLen);
        if (ret != MEDIA_OK) {
            if (ret == MEDIA_NO_CACHE) {
                ttsContext->sleepTime = DEFAULT_SLEEP_TIME;
                goto CONTINUE;
            }
            MEDIA_ERR_LOG("IntellVoiceTtsEngineTextSpeak failed!");
            break;
        }
 CONTINUE:
        if (ttsContext->sleepTime > 0) {
            MediaThreadCondTimeWait(ttsContext->cond, ttsContext->mutex, ttsContext->sleepTime);
        }
        MediaMutexUnLock(ttsContext->mutex);
    }
    MediaMutexUnLock(ttsContext->mutex);
EXIT:
    IntelligentVoiceTtsDestoryThread(ttsContext);
    MEDIA_ERR_LOG("IntellVoiceTtsThread EXIT!");
    return nullptr;
}

static int32_t IntelligentVoiceTtsInit(int32_t argc, const char *argv[])
{
    g_intelligentVoiceTtsCtx.initParams.listener = TtsEngineListenerTest;
    g_intelligentVoiceTtsCtx.textId = 0;
    g_intelligentVoiceTtsCtx.pfd = fopen(argv[1], "rb");
    if (g_intelligentVoiceTtsCtx.pfd == nullptr) {
        MEDIA_ERR_LOG("open file %s failed", argv[1]);
        return MEDIA_ERR;
    }
    int32_t ret = IntellVoiceTtsEngineInit(&g_intelligentVoiceTtsCtx.initParams);
    if (ret != MEDIA_OK) {
        return ret;
    }
    MediaThreadattr attr = { "IntellVoiceTestThread", 0x1000, THREAD_SCHED_INVALID, 0, false };
    g_intelligentVoiceTtsCtx.cond = MediaThreadCondCreate();
    if (g_intelligentVoiceTtsCtx.cond == nullptr) {
        MEDIA_ERR_LOG("create cond failed");
        goto EXIT1;
    }
    g_intelligentVoiceTtsCtx.active = true;
    g_intelligentVoiceTtsCtx.taskHandle = MediaThreadCreate(IntellVoiceTtsThread, &g_intelligentVoiceTtsCtx, &attr);
    if (g_intelligentVoiceTtsCtx.taskHandle == nullptr) {
        MEDIA_ERR_LOG("create IntellVoiceTestThread failed");
        goto EXIT2;
    }
    return MEDIA_OK;
EXIT2:
    MediaThreadCondDestroy(&g_intelligentVoiceTtsCtx.cond);
EXIT1:
    IntellVoiceTtsEngineDeInit();
    if (g_intelligentVoiceTtsCtx.pfd != nullptr) {
        fclose(g_intelligentVoiceTtsCtx.pfd);
        g_intelligentVoiceTtsCtx.pfd = nullptr;
    }
    return MEDIA_ERR;
}

static int32_t IntelligentVoiceTtsDeInit(void)
{
    IntellVoiceTtsEngineDeInit();
    return MEDIA_OK;
}

static int32_t IntelligentVoiceTtsStart(int32_t argc, const char *argv[])
{
    if (argc <= 0x1) {
        MEDIA_ERR_LOG("The number of input parameters is insufficient. num = %d", argc);
        return MEDIA_ERR;
    }
    int32_t ret = 0;
    MediaInitStaticMutexLock(&g_intelligentVoiceTtsCtx.mutex);
    MediaMutexLock(g_intelligentVoiceTtsCtx.mutex);
    if (!g_intelligentVoiceTtsCtx.start) {
        ret = IntelligentVoiceTtsInit(argc, argv);
        if (ret == MEDIA_OK) {
            g_intelligentVoiceTtsCtx.start = true;
        }
        MediaMutexUnLock(g_intelligentVoiceTtsCtx.mutex);
        return ret;
    }
    if (g_intelligentVoiceTtsCtx.pfd == nullptr) {
        g_intelligentVoiceTtsCtx.pfd = fopen(argv[1], "rb");
        if (g_intelligentVoiceTtsCtx.pfd == nullptr) {
            MEDIA_ERR_LOG("open file %s failed", argv[1]);
            MediaMutexUnLock(g_intelligentVoiceTtsCtx.mutex);
            return MEDIA_ERR;
        }
        MediaMutexUnLock(g_intelligentVoiceTtsCtx.mutex);
        MediaThreadCondSignal(g_intelligentVoiceTtsCtx.cond);
        return MEDIA_OK;
    }
    MEDIA_ERR_LOG("The current text is being played and cannot be processed!");
    MediaMutexUnLock(g_intelligentVoiceTtsCtx.mutex);
    return MEDIA_ERR;
}

static int32_t IntelligentVoiceTtsStop(int32_t argc, const char *argv[])
{
    MEDIA_INFO_LOG("enter");
    MEDIA_UNUSED(argc);
    MEDIA_UNUSED(argv);
    MediaInitStaticMutexLock(&g_intelligentVoiceTtsCtx.mutex);
    MediaMutexLock(g_intelligentVoiceTtsCtx.mutex);
    if (!g_intelligentVoiceTtsCtx.start) {
        MediaMutexUnLock(g_intelligentVoiceTtsCtx.mutex);
        MEDIA_ERR_LOG("Tts Engine not started");
        return MEDIA_ERR;
    }
    IntellVoiceTtsEngineStop();
    g_intelligentVoiceTtsCtx.active = false;
    g_intelligentVoiceTtsCtx.start = false;
    MediaMutexUnLock(g_intelligentVoiceTtsCtx.mutex);
    MediaThreadCondSignal(g_intelligentVoiceTtsCtx.cond);
    MediaThreadJoin(&g_intelligentVoiceTtsCtx.taskHandle);
    g_intelligentVoiceTtsCtx.taskHandle = nullptr;
    MediaMutexDestroy(&g_intelligentVoiceTtsCtx.mutex);
    MediaThreadCondDestroy(&g_intelligentVoiceTtsCtx.cond);
    return MEDIA_OK;
}

static int32_t IntelligentVoiceTtsDestory(int32_t argc, const char *argv[])
{
    MEDIA_INFO_LOG("enter");
    IntelligentVoiceTtsStop(argc, argv);
    IntelligentVoiceTtsDeInit();
    g_intelligentVoiceTtsCtx.textId = 0;
    MEDIA_INFO_LOG("out");
    return MEDIA_OK;
}

static int32_t IntelligentVoiceAsrStart(int32_t argc, const char *argv[])
{
    if (argc <= 0x3) {
        MEDIA_ERR_LOG("The number of input parameters is insufficient. num = %d", argc);
        return MEDIA_ERR;
    }
    if (g_intelligentVoiceCtx.start) {
        MEDIA_ERR_LOG("Executing, please exit first!");
        return MEDIA_ERR;
    }
    g_intelligentVoiceCtx.isWakeUpNpu = false;
    g_intelligentVoiceCtx.threadStopped = false;
    g_intelligentVoiceCtx.start = true;
    uint32_t language = strtoul(argv[0x1], NULL, 0x10);
    uint32_t workMode = strtoul(argv[0x2], NULL, 0xA);
    uint32_t enableStatus = strtoul(argv[0x3], NULL, 0xA);
    bool enableNpu = false;
    if (enableStatus != 0) {
        enableNpu = true;
    }
    g_intelligentVoiceCtx.initParams.enableNpu = enableNpu;
    g_intelligentVoiceCtx.initParams.language = (AsrEngineLanguage)language;
    g_intelligentVoiceCtx.initParams.workMode = (AsrEngineWorkMode)workMode;
    g_intelligentVoiceCtx.initParams.listener = AsrEngineListenerTest;
    g_intelligentVoiceCtx.mutex = MediaMutexCreate(nullptr);
    if (g_intelligentVoiceCtx.mutex == nullptr) {
        MEDIA_ERR_LOG("create mutex failed");
        g_intelligentVoiceCtx.start = false;
        return MEDIA_ERR;
    }
    MediaThreadattr attr = { "IntellVoiceTestThread", 0x2000, THREAD_SCHED_INVALID, 0, false };
    g_intelligentVoiceCtx.cond = MediaThreadCondCreate();
    if (g_intelligentVoiceCtx.cond == nullptr) {
        MEDIA_ERR_LOG("create cond failed");
        goto EXIT1;
    }
    g_intelligentVoiceCtx.taskHandle = MediaThreadCreate(IntellVoiceTestThread, &g_intelligentVoiceCtx, &attr);
    if (g_intelligentVoiceCtx.taskHandle == nullptr) {
        MEDIA_ERR_LOG("create IntellVoiceTestThread failed");
        goto EXIT2;
    }
    return MEDIA_OK;
EXIT2:
    MediaThreadCondDestroy(&g_intelligentVoiceCtx.cond);
EXIT1:
    MediaMutexDestroy(&g_intelligentVoiceCtx.mutex);
    g_intelligentVoiceCtx.start = false;
    return MEDIA_ERR;
}

static int32_t IntelligentVoiceAsrStop(int32_t argc, const char *argv[])
{
    MEDIA_UNUSED(argc);
    MEDIA_UNUSED(argv);
    if (!g_intelligentVoiceCtx.start) {
        MEDIA_ERR_LOG("Can't stop without starting!");
        return MEDIA_ERR;
    }
    MediaMutexLock(g_intelligentVoiceCtx.mutex);
    g_intelligentVoiceCtx.threadStopped = true;
    MediaThreadCondSignal(g_intelligentVoiceCtx.cond);
    MediaMutexUnLock(g_intelligentVoiceCtx.mutex);
    MediaThreadJoin(&g_intelligentVoiceCtx.taskHandle);
    g_intelligentVoiceCtx.start = false;
    MediaThreadCondDestroy(&g_intelligentVoiceCtx.cond);
    MediaMutexDestroy(&g_intelligentVoiceCtx.mutex);
    return MEDIA_OK;
}

static int32_t IntelligentVoiceAsrPickupStart(int32_t argc, const char *argv[])
{
    MEDIA_UNUSED(argc);
    MEDIA_UNUSED(argv);
    return IntellVoiceAsrPickUpStreamStart();
}

static int32_t IntelligentVoiceAsrPickupStop(int32_t argc, const char *argv[])
{
    MEDIA_UNUSED(argc);
    MEDIA_UNUSED(argv);
    return IntellVoiceAsrPickUpStreamStop();
}

static IntelligentVoiceTestInvoke g_asrInvokeFuncList[] = {
    {"asr_start", IntelligentVoiceAsrStart},
    {"tts_start", IntelligentVoiceTtsStart},
    {"tts_stop", IntelligentVoiceTtsStop},
    {"tts_destory", IntelligentVoiceTtsDestory},
    {"asr_stop", IntelligentVoiceAsrStop},
    {"pickup_start", IntelligentVoiceAsrPickupStart},
    {"pickup_stop", IntelligentVoiceAsrPickupStop},
};

int32_t IntelligentVoiceAsrEngineSample(int32_t argc, const char *argv[])
{
    MEDIA_INFO_LOG("enter");
    if (argc < 1) {
        MEDIA_ERR_LOG("at least one parameter is required");
        return MEDIA_INVALID_PARAM;
    }
    for (int32_t i = 0; i < argc; i++) {
        MEDIA_INFO_LOG("argv[%d]: %s\n", i, argv[i]);
    }
    size_t size = sizeof(g_asrInvokeFuncList) / sizeof(IntelligentVoiceTestInvoke);
    InvokeFunc invokeFunc = NULL;
    for (size_t i = 0; i < size; i++) {
        if (strcmp(argv[0], g_asrInvokeFuncList[i].cmdName) == 0) {
            invokeFunc = g_asrInvokeFuncList[i].invoke;
            break;
        }
    }
    if (invokeFunc == NULL) {
        MEDIA_ERR_LOG("not found invoke function!, invokeId = %s", argv[0]);
        return MEDIA_INVALID_PARAM;
    }
    return invokeFunc(argc, argv);
}

#ifdef __cplusplus
};
#endif