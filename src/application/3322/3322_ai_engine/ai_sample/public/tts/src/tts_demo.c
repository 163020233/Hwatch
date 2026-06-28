/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 */

#include "tts_demo.h"

// 文件路径根据用户环境决定
static const char *g_amModelFile = "/user/ai/ops/am/am.exeom";
static const char *g_vocoderModelFile = "/user/ai/ops/vocoder/vocoder.exeom";
static const char *g_outputFilePath = "/user/ai/res/tts_res.pcm";

// 音素数据，由用户给出
static const int g_phonemeData[INPUT_SIZE] = {64, 55, 4, 153, 49, 147, 81, 179, 153, 217, 91, 62, 134, 117, 92, 252, 251, 30,
    108, 127, 165, 134, 76, 92, 220, 153, 135, 182, 108, 149, 70, 139, 165, 108, 115, 222, 241, 153, 117, 95, 5, 81,
    182, 155, 70, 56, 181, 239, 153, 117, 165, 138, 30, 155, 70, 56, 82, 70, 156, 81, 236, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static const short g_maskData[INPUT_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

int32_t tts_main(void)
{
    if (ttsEnvInit() != 0) {
        goto DESTROY;
    }
    if (getPcmData(g_phonemeData, g_maskData, g_amModelFile, g_vocoderModelFile, g_outputFilePath) != 0) {
        goto DESTROY;
    }

DESTROY:
    ttsEnvDestroy();
    printf("Finish\n");
    return 0;
}
