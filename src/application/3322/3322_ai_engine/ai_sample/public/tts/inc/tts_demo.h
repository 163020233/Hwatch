/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 */

#ifndef TTS_DEMO_H
#define TTS_DEMO_H

#define INPUT_SIZE 100

#include <stdio.h>
#include <stdint.h>

int32_t ttsEnvInit(void);
int32_t getPcmData(const int phonemeData[INPUT_SIZE], const short maskData[INPUT_SIZE],
    const char *amModelFile, const char *vocoderModelFile, const char *outputFilePath);
int32_t ttsEnvDestroy(void);

#endif /* TTS_DEMO_H */