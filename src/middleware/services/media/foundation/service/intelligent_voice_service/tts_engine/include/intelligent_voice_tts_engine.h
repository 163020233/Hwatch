/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: intelligent voice service tts engine.
 */
#ifndef INTELLIGENT_VS_TTS_H
#define INTELLIGENT_VS_TTS_H

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    TTS_ENGINE_EVENT_SPEAK_BEGIN,
    TTS_ENGINE_EVENT_SPEAK_PLAY_COMPLETE,
    TTS_ENGINE_EVENT_SPEAK_END,
    TTS_ENGINE_EVENT_SPEAK_ERROR,
    TTS_ENGINE_EVENT_MAX
} TtsEngineEvent;

typedef void (*TtsEngineListener)(TtsEngineEvent event, const void *data, uint32_t length);

typedef struct {
    TtsEngineListener listener;
} TtsEngineInitParams;

int32_t IntellVoiceTtsEngineInit(TtsEngineInitParams *initParams);

int32_t IntellVoiceTtsEngineTextSpeak(int32_t textId, const char *textString, uint32_t textLen);

int32_t IntellVoiceTtsEngineStop(void);

int32_t IntellVoiceTtsEngineDeInit(void);

#ifdef __cplusplus
}
#endif

#endif // SMART_VS_H