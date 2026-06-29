/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: intelligent_voice_service.h
 * Create: 2025-11-26
 */

#ifndef INTELLIGENT_VOICE_SERVICE
#define INTELLIGENT_VOICE_SERVICE
#include "intelligent_voice_asr_engine.h"
#include "intelligent_voice_tts_engine.h"
#include "media_thread_adapt.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
#define MSG_QUEUE_LENGTH 20

typedef enum {
    TTS,
    ASR,
    STOP,
} VoiceType;

typedef struct {
    AsrEngineEvent asrEventType;
    TtsEngineEvent ttsEventType;
    VoiceType voiceType;
    void *data;
    uint32_t length;
} IntelligentEngineCallbackInfo;

typedef struct {
    MediaMutexHandle mutex;
    MediaThreadCondHandle cond;
    MediaThreadIdHandle taskHandle;
    AsrEngineInitParams initParams;
    TtsEngineInitParams ttsInitParams;
    bool start;
} IntelligentVoiceServiceContext;

errcode_t IntelligentVoiceSerivicePause(void);
errcode_t IntelligentVoiceSeriviceResume(void);
errcode_t IntelligentVoiceSeriviceStart(void);
errcode_t IntelligentVoiceSeriviceStop(void);

void IntelligentVoiceTTSInit(void);
void IntelligentVoiceTTSDeinit(void);
void IntelligentVoiceTTSStop(void);
void IntelligentVoiceTTSSpeak(const char *textString);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* INTELLIGENT_VOICE_SERVICE */
