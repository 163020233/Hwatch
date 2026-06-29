/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#include "fuzz_demo.h"
#define NLU_RESULT_LEN 106
// 文件路径根据用户环境决定，需要用户将files下的文件传入指定路径中
static const char *g_asrctcModelFile = "/user/ai/sample/ops/asr/ASRCTC.exeom";              // asrctc模型文件
static const char *g_asrctcModelOffsetPath = "/user/ai/sample/ops/asr/ASRCTC_offset.bin";   // asrctc模型偏移文件
static const char *g_nluModelFile = "/user/ai/sample/ops/nlu/NLU.exeom";                    // nlu模型文件
static const char *g_posembFile = "/user/ai/sample/posemb.bin";                             // 音频位置文件
static const char *g_testAudioFile = "/user/ai/sample/OpenHeartrate.pcm";                   // 测试用音频文件

static const char *audioResult[NLU_RESULT_LEN] = {
    "NULL", "NULL", "setalarm", "settimer", "others", "call", "startoutdoorcycling", "startindoorcycling",
    "startclimbing", "startoutdoorrunning", "startindoorwalking", "startwalking", "startsport", "startopenwaterswimming",
    "startpoolswimming", "startfreetraining", "startindoorrunning", "starttrailrunning", "startskiing", "startrowing",
    "startgolf", "startelliptical", "starttreadmill", "starttriathlon", "startrunningcourse", "openpai", "playmusic",
    "openspo2", "openaltimeter", "openheartrate", "openbarometer", "openwallet", "opensportrecord", "openpowersave",
    "openmusicapp", "timingnodisturb", "openpressure", "openelectrocardio", "deactivatepowersave", "playwatchmusic",
    "opencalendar", "smartnodisturb", "opensleep", "setnodisturb", "openmessage", "playallmusic", "playphonemusic",
    "openbuscard", "stopmusic", "openweather", "increasevolume", "opentraningstatus", "opencallrecords", "openneteasymusic",
    "deactivatesteadyscreen", "deactivatenodisturb", "opentimer", "checknotification", "openalarmapp", "opencompass",
    "opensetting", "openaccesscard", "decreasevolume", "openwatchface", "opensport", "openmute", "opentheater", "pausemusic",
    "resumemusic", "opensteadyscreen", "opennodisturb", "deactivatetheater", "addsport", "deactivateflashlight",
    "increasebrightness", "startlastsport", "openactivatyrecord", "openfavourites", "openbreathtrain", "openremotephoto",
    "decreasebrightness", "openwechatpay", "openalipay", "openflashlight", "opentelephone", "lockscreen", "playsongnext",
    "stopsport", "setring", "returnhome", "playmycollection", "playrunningmusic", "playsongpre", "pausesport", "resumesport",
    "refuse", "openstopwatch", "setbrightness", "answer", "findphone", "playrepeatcurrent", "playshuffle", "playrepeatlist",
    "connectphone", "xiaoai", "xyxy"
};

static void printResult(int32_t index)
{
    if (index < 0 || index >= NLU_RESULT_LEN) {
        printf("Index error\n");
        return;
    }
    printf("Index: %d\n", index);
    printf("Result: %s\n", audioResult[index]);
}

int32_t fuzzyCommandWordMain()
{
    State state;
    int32_t index;
    Hyps hyps = {0};
    if (fuzzEnvInit(g_asrctcModelFile, g_asrctcModelOffsetPath, g_nluModelFile, g_posembFile) != 0) {
        goto DESTROY;
    }
    do {
        state = audioFileProcess(g_testAudioFile, &hyps);
    } while (state == ASRCTC);
    if (state == FAILED) {
        printf("Audio process error\n");
        goto DESTROY;
    }
    if (state == NLU || state == FINISH) {
        index = getNluResultIndex(&hyps);
        printResult(index);
    }

DESTROY:
    fuzzEnvFinalize();
    printf("Finish\n");
    return 0;
}