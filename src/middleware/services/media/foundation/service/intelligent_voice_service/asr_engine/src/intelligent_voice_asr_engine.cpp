/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: intelligent voice service asr engine.
 */

#include "intelligent_voice_asr_engine.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include "audio_vqe_typedef.h"
#include "audio_vqe_common_basicop_vec.h"
#include "window_coeff.h"
#include "fp16_convert.h"
#include "ctc_prefix_beamsearch.h"
#include "map_dict.h"
#include "asr.h"
#include "fbank.h"
#include "asr_ctc_model.h"
#include "nlu_model.h"
#include "nlu_slot_convert.h"
#include "nlu_slot_postproc.h"
#include "utils_demo.h"
#include "securec.h"
#include "audio_manager.h"
#include "audio_service.h"
#include "media_log.h"
#include "audio_stream_in.h"
#include "audio_stream_out.h"
#include "media_thread_adapt.h"
#include "media_mem.h"
#include "audio_errors.h"
#include "npu_drv_if.h"
#include "media_chr_log.h"
#include "circle_buffer.h"

using Audio::AudioManager;
using Audio::AudioStreamIn;
using Audio::AudioStreamOut;
using Audio::InterruptListener;
using Audio::AudioInterrupt;

#ifdef __cplusplus
extern "C" {
#endif

enum class IntellVoiceAsrStates : int32_t {
    INTELL_VOICE_ASR_ERR = 0,
    INTELL_VOICE_ASR_IDLE = 1 << 0,
    INTELL_VOICE_ASR_INIT = 1 << 1,
    INTELL_VOICE_ASR_START = 1 << 2,
    INTELL_VOICE_ASR_STOP = 1 << 3,
};

const uint32_t CTC_OUTPUT_SIZE = 20624;
const uint32_t NLU_MASK_SIZE = 5408;
const uint32_t NLU_OUTPUT_SIZE = 212;
const uint32_t FRAME_LAP = 160;
const uint32_t DEFAULT_INPUT_BUFFER_SIZE = 560;
const uint32_t DEFAULT_FFBANK_35_SIZE = 2800;
const uint32_t DEFAULT_POS_EMB_SIZE = 4096;
const uint32_t DEFAULT_LOOP_COUNT = 2;
const uint32_t DEFAULT_ONE_FRAME_LEN = 160;
const uint32_t DEFAULT_HALF_FRAME_LEN = 80;
const uint32_t DEFAULT_RESULT_START_INDEX = 2;
const uint32_t DEFAULT_FRAME_COUNT = 3;
const uint32_t DEFAULT_SLEEP_TIME_10_MS = 10000;
const uint32_t PPS_DATA_TMP_SIZE_Y = 640;
const uint32_t PPS_DATA_TMP_SIZE_X = 20;
const uint32_t COMMAND_106_NUM = 106;
const uint32_t FP16_SIZE = 2;
const uint32_t CHAN_NUM_IN = 1;
const uint32_t DEFAULT_SAMPLE_RATE = 16000;
const uint32_t DEFAULT_RECORD_BUFFER_SIZE = 320;
const uint32_t CAPTURE_ONE_FRAME_TIME = 100;
const int16_t HYPS_CHUNK_CUM = 15;
const uint32_t HYPS_POSEMB = 3;
const uint32_t FRAME_CNT_RESET = 3;
const uint32_t NLU_SLOT_MAX_SIZE = 25;
const uint32_t NLU_POST_MAX_SIZE = 800;
const uint32_t NLU_OUT_SLOT_DATA_SIZE = 11450;
const uint32_t NLU_OUT_SLOT_INFO_SIZE = 500;
const uint32_t PCM_BUFFER_SIZE = 320 * 100;

const char *ASR_MODEL_NAME = "/user/ai/ops/asr_ctc/new_ctc_final_fifo.exeom";
const char *ASR_MODEL_OFFSET_PATH = "/user/ai/ops/asr/offset.bin";
const char *CTC_MODEL_NAME = "/user/ai/ops/ctc/ctc.exeom";
const char *NLU_MODEL_NAME = "/user/ai/ops/nlu/NLU.exeom";
const char *POS_EMB_PE_FILE_PATH = "/user/ai/save_posemb5000x64_uint16.bin";

typedef struct {
    asr_pre_post_para *asrCoeff;
    FILE *file;
    int16_t *inputData;
    uint16_t *ffbankNew;
    uint16_t *posEmb;
    uint32_t frmCnt;
    stru_asr_in *asrInput;
    bool isfirst;
    int chunkNum;
    uint16_t *ffbank;
    void *ctcModelOutput;
    char *nluOutSlotData;
    long_hyps *curHyps;
    float *nluOutVec;
    int16_t ppsDataTmp[PPS_DATA_TMP_SIZE_X][PPS_DATA_TMP_SIZE_Y];
    void *asrCtx;
    int16_t curHypsLastLen;
    int16_t curHypsSameNumLen;
    int16_t curHypsSameNumLenExt;
} AudioProcessContext;

typedef struct {
    MediaMutexHandle produceMutex;
    MediaThreadCondHandle produceCond;
    MediaThreadIdHandle produceTaskHandle;
    bool isThreadActive;
    uint32_t produceSleepTime;
    int64_t produceDataSize;
} IntellVoiceAsrProduceContext;

typedef struct {
    MediaMutexHandle npuMutex;
    MediaThreadCondHandle npuCond;
    MediaThreadIdHandle npuTaskHandle;
    bool isThreadActive;
    uint32_t npuSleepTime;
    int64_t npuDataSize;
} IntellVoiceAsrNpuContext;

typedef struct {
    AudioSession sessionId;
    AudioStreamType streamType;
    AudioInterrupt interrupt;
    std::shared_ptr<AudioStreamIn> asiHandle;
    bool isPause;
    bool isStopped;
    bool isResumed;
    bool enableNpu;
    AsrEngineLanguage language;
    AsrEngineWorkMode workMode;
    AsrEngineListener listener;
    IntellVoiceAsrStates states;
    AudioProcessContext *audioCtx;
    CapturerInputConfig asrInputConfig;
    CacheMemoryInfo pcmBufferInfo;
    IntellVoiceAsrProduceContext *produceContext;
    IntellVoiceAsrNpuContext *npuContext;
} IntellVoiceAsrEngineContext;

typedef struct {
    AudioInterrupt interrupt;
    std::shared_ptr<AudioStreamOut> asoHandle;
    std::shared_ptr<AudioStreamIn> asiHandle;
    bool isStart;
} PickupStreamContext;

typedef void (*GetSlotInfoFunc)(AsrEngineCmdInfo *cmdInfo, const char *info);

typedef struct {
    int32_t cmdId;
    GetSlotInfoFunc invoke;
} IntellVoiceSlotInfo;

static AudioManager &g_amInstance = AudioManager::GetInstance();
static IntellVoiceAsrEngineContext *g_asrCtx = nullptr;
static MediaMutexHandle g_asrMutex = nullptr;
static MediaMutexHandle g_pickupMutex = nullptr;
static char g_nluPostprocIn[NLU_POST_MAX_SIZE];
static int32_t g_nluOutslotData[NLU_SLOT_MAX_SIZE];
static PickupStreamContext g_pickupCtx;

static AsrEngineCmdInfo g_asrEngineCmdInfo[] = {
    {.index = 82, .cmdId = ASR_ENGINE_CMD_ALARM_CONTROL, .alarmControlInfo = {ASR_ENGINE_ALARM_CTRL_SET, {}}},
    {.index = -1, .cmdId = ASR_ENGINE_CMD_TELEPHONE_CONTROL, .telephoneControlInfo = {ASR_ENGINE_TELEPHONE_CTRL_DIAL_NUMBER, 0}},
    {.index = 83, .cmdId = ASR_ENGINE_CMD_TIMER_CONTROL, .timerControlInfo = {ASR_ENGINE_TIMER_CTRL_SET, 0, 0, 0}},
    {.index = -1, .cmdId = ASR_ENGINE_CMD_MUSIC_CONTROL, .musicControlInfo = {ASR_ENGINE_MUSIC_CTRL_PLAY}},
    {.index = 98, .cmdId = ASR_ENGINE_CMD_MOTION_CONTROL,
        .motionControlInfo = {ASR_ENGINE_MOTION_CTRL_START_TRAIL_RUNNING, ASR_ENGINE_MOTION_TARGET_NONE, 0}},
    {.index = 90, .cmdId = ASR_ENGINE_CMD_MOTION_CONTROL,
        .motionControlInfo = {ASR_ENGINE_MOTION_CTRL_START_OUTDOOR_CYCLING, ASR_ENGINE_MOTION_TARGET_NONE, 0}},
    {.index = 97, .cmdId = ASR_ENGINE_CMD_MOTION_CONTROL,
        .motionControlInfo = {ASR_ENGINE_MOTION_CTRL_START_CLIMBING, ASR_ENGINE_MOTION_TARGET_NONE, 0}},
    {.index = 99, .cmdId = ASR_ENGINE_CMD_MOTION_CONTROL,
        .motionControlInfo = {ASR_ENGINE_MOTION_CTRL_START_SKIING, ASR_ENGINE_MOTION_TARGET_NONE, 0}},
    {.index = 89, .cmdId = ASR_ENGINE_CMD_MOTION_CONTROL,
        .motionControlInfo = {ASR_ENGINE_MOTION_CTRL_START_WALKING, ASR_ENGINE_MOTION_TARGET_NONE, 0}},
    {.index = 102, .cmdId = ASR_ENGINE_CMD_MOTION_CONTROL,
        .motionControlInfo = {ASR_ENGINE_MOTION_CTRL_START_GOLF, ASR_ENGINE_MOTION_TARGET_NONE, 0}},
    {.index = 93, .cmdId = ASR_ENGINE_CMD_MOTION_CONTROL,
        .motionControlInfo = {ASR_ENGINE_MOTION_CTRL_START_INDOOR_WALKING, ASR_ENGINE_MOTION_TARGET_NONE, 0}},
    {.index = 92, .cmdId = ASR_ENGINE_CMD_MOTION_CONTROL,
        .motionControlInfo = {ASR_ENGINE_MOTION_CTRL_START_INDOOR_CYCLING, ASR_ENGINE_MOTION_TARGET_NONE, 0}},
    {.index = 101, .cmdId = ASR_ENGINE_CMD_MOTION_CONTROL,
        .motionControlInfo = {ASR_ENGINE_MOTION_CTRL_START_TRIATHLON, ASR_ENGINE_MOTION_TARGET_NONE, 0}},
    {.index = 91, .cmdId = ASR_ENGINE_CMD_MOTION_CONTROL,
        .motionControlInfo = {ASR_ENGINE_MOTION_CTRL_START_TREADMILL, ASR_ENGINE_MOTION_TARGET_NONE, 0}},
    {.index = 100, .cmdId = ASR_ENGINE_CMD_MOTION_CONTROL,
        .motionControlInfo = {ASR_ENGINE_MOTION_CTRL_START_ROWING, ASR_ENGINE_MOTION_TARGET_NONE, 0}},
    {.index = 103, .cmdId = ASR_ENGINE_CMD_MOTION_CONTROL,
        .motionControlInfo = {ASR_ENGINE_MOTION_CTRL_START_FREE_TRAINING, ASR_ENGINE_MOTION_TARGET_NONE, 0}},
    {.index = 96, .cmdId = ASR_ENGINE_CMD_MOTION_CONTROL,
        .motionControlInfo = {ASR_ENGINE_MOTION_CTRL_START_ELLIPTICAL, ASR_ENGINE_MOTION_TARGET_NONE, 0}},
    {.index = 86, .cmdId = ASR_ENGINE_CMD_MOTION_CONTROL,
        .motionControlInfo = {ASR_ENGINE_MOTION_CTRL_START_OUTDOOR_RUNNING, ASR_ENGINE_MOTION_TARGET_NONE, 0}},
    {.index = 94, .cmdId = ASR_ENGINE_CMD_MOTION_CONTROL,
        .motionControlInfo = {ASR_ENGINE_MOTION_CTRL_START_OPENWATER_SWIMMING, ASR_ENGINE_MOTION_TARGET_NONE, 0}},
    {.index = 95, .cmdId = ASR_ENGINE_CMD_MOTION_CONTROL,
        .motionControlInfo = {ASR_ENGINE_MOTION_CTRL_START_POOL_SWIMMING, ASR_ENGINE_MOTION_TARGET_NONE, 0}},
    {.index = 87, .cmdId = ASR_ENGINE_CMD_MOTION_CONTROL,
        .motionControlInfo = {ASR_ENGINE_MOTION_CTRL_START_INDOOR_RUNNING, ASR_ENGINE_MOTION_TARGET_NONE, 0}},
    {.index = 88, .cmdId = ASR_ENGINE_CMD_MOTION_CONTROL,
        .motionControlInfo = {ASR_ENGINE_MOTION_CTRL_START_RUNNING_COURSE, ASR_ENGINE_MOTION_TARGET_NONE, 0}},
    {.index = -1, .cmdId = ASR_ENGINE_CMD_APP_CONTROL, .appControlInfo = {ASR_ENGINE_APP_WALLET, ASR_ENGINE_APP_CTRL_OPEN}},
    {.index = 85, .cmdId = ASR_ENGINE_CMD_MOTION_CONTROL,
        .motionControlInfo = {ASR_ENGINE_MOTION_CTRL_START_SPORT, ASR_ENGINE_MOTION_TARGET_NONE, 0}},
    {.index = 15, .cmdId = ASR_ENGINE_CMD_APP_CONTROL, .appControlInfo = {ASR_ENGINE_APP_MUSIC_APP, ASR_ENGINE_APP_CTRL_OPEN}},
    {.index = 1, .cmdId = ASR_ENGINE_CMD_APP_CONTROL, .appControlInfo = {ASR_ENGINE_APP_HEART_RATE, ASR_ENGINE_APP_CTRL_OPEN}},
    {.index = 3, .cmdId = ASR_ENGINE_CMD_APP_CONTROL, .appControlInfo = {ASR_ENGINE_APP_SPORT_RECORD, ASR_ENGINE_APP_CTRL_OPEN}},
    {.index = 14, .cmdId = ASR_ENGINE_CMD_APP_CONTROL, .appControlInfo = {ASR_ENGINE_APP_WEATHER, ASR_ENGINE_APP_CTRL_OPEN}},
    {.index = -1, .cmdId = ASR_ENGINE_CMD_MUSIC_CONTROL, .musicControlInfo = {ASR_ENGINE_MUSIC_CTRL_PLAY_NEXT}},
    {.index = 18, .cmdId = ASR_ENGINE_CMD_APP_CONTROL, .appControlInfo = {ASR_ENGINE_APP_ALARM_APP, ASR_ENGINE_APP_CTRL_OPEN}},
    {.index = 6, .cmdId = ASR_ENGINE_CMD_APP_CONTROL, .appControlInfo = {ASR_ENGINE_APP_SPO2, ASR_ENGINE_APP_CTRL_OPEN}},
    {.index = 7, .cmdId = ASR_ENGINE_CMD_APP_CONTROL, .appControlInfo = {ASR_ENGINE_APP_SLEEP, ASR_ENGINE_APP_CTRL_OPEN}},
    {.index = -1, .cmdId = ASR_ENGINE_CMD_MUSIC_CONTROL, .musicControlInfo = {ASR_ENGINE_MUSIC_CTRL_PLAY_PRE}},
    {.index = -1, .cmdId = ASR_ENGINE_CMD_MUSIC_CONTROL, .musicControlInfo = {ASR_ENGINE_MUSIC_CTRL_RESUME}},
    {.index = -1, .cmdId = ASR_ENGINE_CMD_MUSIC_CONTROL, .musicControlInfo = {ASR_ENGINE_MUSIC_CTRL_PAUSE}},
    {.index = 19, .cmdId = ASR_ENGINE_CMD_APP_CONTROL, .appControlInfo = {ASR_ENGINE_APP_CALENDAR, ASR_ENGINE_APP_CTRL_OPEN}},
    {.index = 26, .cmdId = ASR_ENGINE_CMD_APP_CONTROL, .appControlInfo = {ASR_ENGINE_APP_BAROMETER, ASR_ENGINE_APP_CTRL_OPEN}},
    {.index = 38, .cmdId = ASR_ENGINE_CMD_DEVICE_CONTROL, .deviceControlInfo = {ASR_ENGINE_DEVICE_CTRL_DECREASE_VOLUME}},
    {.index = 37, .cmdId = ASR_ENGINE_CMD_DEVICE_CONTROL, .deviceControlInfo = {ASR_ENGINE_DEVICE_CTRL_INCREASE_VOLUME}},
    {.index = 8, .cmdId = ASR_ENGINE_CMD_APP_CONTROL, .appControlInfo = {ASR_ENGINE_APP_PRESSURE, ASR_ENGINE_APP_CTRL_OPEN}},
    {.index = 25, .cmdId = ASR_ENGINE_CMD_APP_CONTROL, .appControlInfo = {ASR_ENGINE_APP_ALI_PAY, ASR_ENGINE_APP_CTRL_OPEN}},
    {.index = -1, .cmdId = ASR_ENGINE_CMD_MUSIC_CONTROL, .musicControlInfo = {ASR_ENGINE_MUSIC_CTRL_PLAY_SHUFFLE}},
    {.index = -1, .cmdId = ASR_ENGINE_CMD_APP_CONTROL, .appControlInfo = {ASR_ENGINE_APP_ELECTROCARDIO, ASR_ENGINE_APP_CTRL_OPEN}},
    {.index = -1, .cmdId = ASR_ENGINE_CMD_APP_CONTROL, .appControlInfo = {ASR_ENGINE_APP_PAI, ASR_ENGINE_APP_CTRL_OPEN}},
    {.index = 29, .cmdId = ASR_ENGINE_CMD_APP_CONTROL, .appControlInfo = {ASR_ENGINE_APP_NETEASY_MUSIC, ASR_ENGINE_APP_CTRL_OPEN}},
    {.index = -1, .cmdId = ASR_ENGINE_CMD_MUSIC_CONTROL, .musicControlInfo = {ASR_ENGINE_MUSIC_CTRL_PLAY_LOOP}},
    {.index = 24, .cmdId = ASR_ENGINE_CMD_APP_CONTROL, .appControlInfo = {ASR_ENGINE_APP_WECHAT_PAY, ASR_ENGINE_APP_CTRL_OPEN}},
    {.index = -1, .cmdId = ASR_ENGINE_CMD_MUSIC_CONTROL, .musicControlInfo = {ASR_ENGINE_MUSIC_CTRL_PLAY_REPEAT_LIST}},
    {.index = 23, .cmdId = ASR_ENGINE_CMD_APP_CONTROL, .appControlInfo = {ASR_ENGINE_APP_TIMER, ASR_ENGINE_APP_CTRL_OPEN}},
    {.index = 21, .cmdId = ASR_ENGINE_CMD_APP_CONTROL, .appControlInfo = {ASR_ENGINE_APP_COMPASS, ASR_ENGINE_APP_CTRL_OPEN}},
    {.index = 17, .cmdId = ASR_ENGINE_CMD_APP_CONTROL, .appControlInfo = {ASR_ENGINE_APP_NOTIFICATION, ASR_ENGINE_APP_CTRL_OPEN}},
    {.index = -1, .cmdId = ASR_ENGINE_CMD_APP_CONTROL, .appControlInfo = {ASR_ENGINE_APP_SPORT, ASR_ENGINE_APP_CTRL_OPEN}},
    {.index = 10, .cmdId = ASR_ENGINE_CMD_APP_CONTROL, .appControlInfo = {ASR_ENGINE_APP_CALL_RECORDS, ASR_ENGINE_APP_CTRL_OPEN}},
    {.index = 31, .cmdId = ASR_ENGINE_CMD_APP_CONTROL, .appControlInfo = {ASR_ENGINE_APP_MESSAGE, ASR_ENGINE_APP_CTRL_OPEN}},
    {.index = 5, .cmdId = ASR_ENGINE_CMD_APP_CONTROL, .appControlInfo = {ASR_ENGINE_APP_TRANING_STATUS, ASR_ENGINE_APP_CTRL_OPEN}},
    {.index = 49, .cmdId = ASR_ENGINE_CMD_DEVICE_CONTROL, .deviceControlInfo = {ASR_ENGINE_DEVICE_CTRL_OPEN_POWER_SAVE}},
    {.index = 9, .cmdId = ASR_ENGINE_CMD_APP_CONTROL, .appControlInfo = {ASR_ENGINE_APP_BREATH_TRAIN, ASR_ENGINE_APP_CTRL_OPEN}},
    {.index = -1, .cmdId = ASR_ENGINE_CMD_DEVICE_CONTROL, .deviceControlInfo = {ASR_ENGINE_DEVICE_CTRL_TIMING_NODISTURB}},
    {.index = -1, .cmdId = ASR_ENGINE_CMD_APP_CONTROL, .appControlInfo = {ASR_ENGINE_APP_FAVOURITES, ASR_ENGINE_APP_CTRL_OPEN}},
    {.index = 16, .cmdId = ASR_ENGINE_CMD_APP_CONTROL, .appControlInfo = {ASR_ENGINE_APP_REMOTE_PHOTO, ASR_ENGINE_APP_CTRL_OPEN}},
    {.index = -1, .cmdId = ASR_ENGINE_CMD_MUSIC_CONTROL, .musicControlInfo = {ASR_ENGINE_MUSIC_CTRL_PLAY_WATCH_MUSIC}},
    {.index = -1, .cmdId = ASR_ENGINE_CMD_DEVICE_CONTROL, .deviceControlInfo = {ASR_ENGINE_DEVICE_CTRL_SMART_NODISTURB}},
    {.index = 50, .cmdId = ASR_ENGINE_CMD_DEVICE_CONTROL, .deviceControlInfo = {ASR_ENGINE_DEVICE_CTRL_CLOSE_POWER_SAVE}},
    {.index = 35, .cmdId = ASR_ENGINE_CMD_DEVICE_CONTROL, .deviceControlInfo = {ASR_ENGINE_DEVICE_CTRL_DECREASE_BRIGHTNESS}},
    {.index = 34, .cmdId = ASR_ENGINE_CMD_DEVICE_CONTROL, .deviceControlInfo = {ASR_ENGINE_DEVICE_CTRL_INCREASE_BRIGHTNESS}},
    {.index = 27, .cmdId = ASR_ENGINE_CMD_APP_CONTROL, .appControlInfo = {ASR_ENGINE_APP_ALTIMETER, ASR_ENGINE_APP_CTRL_OPEN}},
    {.index = -1, .cmdId = ASR_ENGINE_CMD_MUSIC_CONTROL, .musicControlInfo = {ASR_ENGINE_MUSIC_CTRL_PLAY_ALL_MUSIC}},
    {.index = -1, .cmdId = ASR_ENGINE_CMD_MUSIC_CONTROL, .musicControlInfo = {ASR_ENGINE_MUSIC_CTRL_STOP}},
    {.index = -1, .cmdId = ASR_ENGINE_CMD_DEVICE_CONTROL, .deviceControlInfo = {ASR_ENGINE_DEVICE_CTRL_SET_NODISTURB}},
    {.index = -1, .cmdId = ASR_ENGINE_CMD_APP_CONTROL, .appControlInfo = {ASR_ENGINE_APP_SETTING, ASR_ENGINE_APP_CTRL_OPEN}},
    {.index = -1, .cmdId = ASR_ENGINE_CMD_DEVICE_CONTROL, .deviceControlInfo = {ASR_ENGINE_DEVICE_CTRL_LOCK_SCREEN}},
    {.index = -1, .cmdId = ASR_ENGINE_CMD_MUSIC_CONTROL, .musicControlInfo = {ASR_ENGINE_MUSIC_CTRL_PLAY_PHONE_MUSIC}},
    {.index = -1, .cmdId = ASR_ENGINE_CMD_DEVICE_CONTROL, .deviceControlInfo = {ASR_ENGINE_DEVICE_CTRL_RETURN_HOME}},
    {.index = 32, .cmdId = ASR_ENGINE_CMD_APP_CONTROL, .appControlInfo = {ASR_ENGINE_APP_WATCH_FACE, ASR_ENGINE_APP_CTRL_OPEN}},
    {.index = 22, .cmdId = ASR_ENGINE_CMD_APP_CONTROL, .appControlInfo = {ASR_ENGINE_APP_STOPWATCH, ASR_ENGINE_APP_CTRL_OPEN}},
    {.index = 78, .cmdId = ASR_ENGINE_CMD_APP_CONTROL, .appControlInfo = {ASR_ENGINE_APP_BUS_CARD, ASR_ENGINE_APP_CTRL_OPEN}},
    {.index = 46, .cmdId = ASR_ENGINE_CMD_DEVICE_CONTROL, .deviceControlInfo = {ASR_ENGINE_DEVICE_CTRL_CLOSE_NODISTURB}},
    {.index = -1, .cmdId = ASR_ENGINE_CMD_TELEPHONE_CONTROL, .telephoneControlInfo = {ASR_ENGINE_TELEPHONE_CTRL_REFUSE_CALL, 0}},
    {.index = 36, .cmdId = ASR_ENGINE_CMD_DEVICE_CONTROL, .deviceControlInfo = {ASR_ENGINE_DEVICE_CTRL_OPEN_MUTE}},
    {.index = 40, .cmdId = ASR_ENGINE_CMD_DEVICE_CONTROL, .deviceControlInfo = {ASR_ENGINE_DEVICE_CTRL_CLOSE_STEADY_SCREEN}},
    {.index = 41, .cmdId = ASR_ENGINE_CMD_DEVICE_CONTROL, .deviceControlInfo = {ASR_ENGINE_DEVICE_CTRL_SET_RING}},
    {.index = -1, .cmdId = ASR_ENGINE_CMD_TELEPHONE_CONTROL, .telephoneControlInfo = {ASR_ENGINE_TELEPHONE_CTRL_ANSWER_CALL, 0}},
    {.index = -1, .cmdId = ASR_ENGINE_CMD_WAKEUP_KEYWORD, .wakeupKeywordInfo = {ASR_ENGINE_WAKEUP_KEYWORD_XIAO_YI} },
    {.index = 45, .cmdId = ASR_ENGINE_CMD_DEVICE_CONTROL, .deviceControlInfo = {ASR_ENGINE_DEVICE_CTRL_OPEN_NODISTURB} },
    {.index = 76, .cmdId = ASR_ENGINE_CMD_DEVICE_CONTROL, .deviceControlInfo = {ASR_ENGINE_DEVICE_CTRL_OPEN_FLASHLIGHT} },
    {.index = 75, .cmdId = ASR_ENGINE_CMD_DEVICE_CONTROL, .deviceControlInfo = {ASR_ENGINE_DEVICE_CTRL_FIND_PHONE} },
    {.index = 79, .cmdId = ASR_ENGINE_CMD_APP_CONTROL, .appControlInfo = {ASR_ENGINE_APP_ACCESS_CARD, ASR_ENGINE_APP_CTRL_OPEN} },
    {.index = 39, .cmdId = ASR_ENGINE_CMD_DEVICE_CONTROL, .deviceControlInfo = {ASR_ENGINE_DEVICE_CTRL_OPEN_STEADY_SCREEN} },
    {.index = 30, .cmdId = ASR_ENGINE_CMD_APP_CONTROL, .appControlInfo = {ASR_ENGINE_APP_TELEPHONE, ASR_ENGINE_APP_CTRL_OPEN} },
    {.index = -1, .cmdId = ASR_ENGINE_CMD_MOTION_CONTROL,
        .motionControlInfo = {ASR_ENGINE_MOTION_CTRL_ADD_SPORT, ASR_ENGINE_MOTION_TARGET_NONE, 0} },
    {.index = -1, .cmdId = ASR_ENGINE_CMD_DEVICE_CONTROL, .deviceControlInfo = {ASR_ENGINE_DEVICE_CTRL_OPEN_THEATER} },
    {.index = 77, .cmdId = ASR_ENGINE_CMD_DEVICE_CONTROL, .deviceControlInfo = {ASR_ENGINE_DEVICE_CTRL_CLOSE_FLASHLIGHT} },
    {.index = -1, .cmdId = ASR_ENGINE_CMD_MOTION_CONTROL,
         .motionControlInfo = {ASR_ENGINE_MOTION_CTRL_START_LAST_SPORT, ASR_ENGINE_MOTION_TARGET_NONE, 0} },
    {.index = -1, .cmdId = ASR_ENGINE_CMD_DEVICE_CONTROL, .deviceControlInfo = {ASR_ENGINE_DEVICE_CTRL_CLOSE_THEATER} },
    {.index = -1, .cmdId = ASR_ENGINE_CMD_MUSIC_CONTROL, .musicControlInfo = {ASR_ENGINE_MUSIC_CTRL_PLAY_RUNNING_MUSIC} },
    {.index = -1, .cmdId = ASR_ENGINE_CMD_MUSIC_CONTROL, .musicControlInfo = {ASR_ENGINE_MUSIC_CTRL_PLAY_MY_COLLECTION} },
    {.index = -1, .cmdId = ASR_ENGINE_CMD_DEVICE_CONTROL, .deviceControlInfo = {ASR_ENGINE_DEVICE_CTRL_SET_BRIGHTNESS} },
    {.index = -1, .cmdId = ASR_ENGINE_CMD_DEVICE_CONTROL, .deviceControlInfo = {ASR_ENGINE_DEVICE_CTRL_CONNECT_PHONE} },
    {.index = -1, .cmdId = ASR_ENGINE_CMD_WAKEUP_KEYWORD, .wakeupKeywordInfo = {ASR_ENGINE_WAKEUP_KEYWORD_XIAO_YI} },
    {.index = 55, .cmdId = ASR_ENGINE_CMD_MOTION_CONTROL,
        .motionControlInfo = {ASR_ENGINE_MOTION_CTRL_PAUSE_SPORT, ASR_ENGINE_MOTION_TARGET_NONE, 0} },
    {.index = 57, .cmdId = ASR_ENGINE_CMD_MOTION_CONTROL,
        .motionControlInfo = {ASR_ENGINE_MOTION_CTRL_RESUME_SPORT, ASR_ENGINE_MOTION_TARGET_NONE, 0} },
    {.index = 56, .cmdId = ASR_ENGINE_CMD_MOTION_CONTROL,
        .motionControlInfo = {ASR_ENGINE_MOTION_CTRL_STOP_SPORT, ASR_ENGINE_MOTION_TARGET_NONE, 0} },
    {.index = -1, .cmdId = ASR_ENGINE_CMD_APP_CONTROL, .appControlInfo = {ASR_ENGINE_APP_ACTIVATY_RECORD, ASR_ENGINE_APP_CTRL_OPEN} },
};

static AsrEngineCmdInfo g_kwsEngineCmdInfo[] = {
    {.index = 1, .cmdId = ASR_ENGINE_CMD_WAKEUP_KEYWORD, .wakeupKeywordInfo = {ASR_ENGINE_WAKEUP_KEYWORD_XIAO_YI} },
    {.index = 2, .cmdId = ASR_ENGINE_CMD_TELEPHONE_CONTROL, .telephoneControlInfo = {ASR_ENGINE_TELEPHONE_CTRL_ANSWER_CALL, 0}},
    {.index = 3, .cmdId = ASR_ENGINE_CMD_TELEPHONE_CONTROL, .telephoneControlInfo = {ASR_ENGINE_TELEPHONE_CTRL_REFUSE_CALL, 0}},
    {.index = 4, .cmdId = ASR_ENGINE_CMD_MUSIC_CONTROL, .musicControlInfo = {ASR_ENGINE_MUSIC_CTRL_PLAY_PRE}},
    {.index = 5, .cmdId = ASR_ENGINE_CMD_MUSIC_CONTROL, .musicControlInfo = {ASR_ENGINE_MUSIC_CTRL_PLAY_NEXT}},
    {.index = 6, .cmdId = ASR_ENGINE_CMD_DEVICE_CONTROL, .deviceControlInfo = {ASR_ENGINE_DEVICE_CTRL_INCREASE_VOLUME}},
    {.index = 7, .cmdId = ASR_ENGINE_CMD_DEVICE_CONTROL, .deviceControlInfo = {ASR_ENGINE_DEVICE_CTRL_DECREASE_VOLUME}},
    {.index = 8, .cmdId = ASR_ENGINE_CMD_MUSIC_CONTROL, .musicControlInfo = {ASR_ENGINE_MUSIC_CTRL_PLAY}},
    {.index = 9, .cmdId = ASR_ENGINE_CMD_MUSIC_CONTROL, .musicControlInfo = {ASR_ENGINE_MUSIC_CTRL_PAUSE}},
    {.index = 10, .cmdId = ASR_ENGINE_CMD_MUSIC_CONTROL, .musicControlInfo = {ASR_ENGINE_MUSIC_CTRL_RESUME}},
    {.index = 11, .cmdId = ASR_ENGINE_CMD_APP_CONTROL, .appControlInfo = {ASR_ENGINE_APP_SPORT, ASR_ENGINE_APP_CTRL_OPEN}},
    {.index = 12, .cmdId = ASR_ENGINE_CMD_APP_CONTROL, .appControlInfo = {ASR_ENGINE_APP_MESSAGE, ASR_ENGINE_APP_CTRL_OPEN}},
    {.index = 13, .cmdId = ASR_ENGINE_CMD_APP_CONTROL, .appControlInfo = {ASR_ENGINE_APP_ALARM_APP, ASR_ENGINE_APP_CTRL_OPEN}},
    {.index = 14, .cmdId = ASR_ENGINE_CMD_APP_CONTROL, .appControlInfo = {ASR_ENGINE_APP_ALI_PAY, ASR_ENGINE_APP_CTRL_OPEN}},
    {.index = 15, .cmdId = ASR_ENGINE_CMD_APP_CONTROL, .appControlInfo = {ASR_ENGINE_APP_WECHAT_PAY, ASR_ENGINE_APP_CTRL_OPEN}},
    {.index = 16, .cmdId = ASR_ENGINE_CMD_APP_CONTROL, .appControlInfo = {ASR_ENGINE_APP_ALI_PAY_SCAN, ASR_ENGINE_APP_CTRL_OPEN}},
    {.index = 17, .cmdId = ASR_ENGINE_CMD_WAKEUP_KEYWORD, .wakeupKeywordInfo = {ASR_ENGINE_WAKEUP_KEYWORD_HELLO_YOYO}},
    {.index = 18, .cmdId = ASR_ENGINE_CMD_WAKEUP_KEYWORD, .wakeupKeywordInfo = {ASR_ENGINE_WAKEUP_KEYWORD_XIAO_WEI}},
    {.index = 19, .cmdId = ASR_ENGINE_CMD_WAKEUP_KEYWORD, .wakeupKeywordInfo = {ASR_ENGINE_WAKEUP_KEYWORD_HI_CELIA}},
};

#define CHK_NULL_RETURN(ptr, ret, printfString) \
do { \
    if ((ptr) == NULL) { \
        MEDIA_ERR_LOG("%s :ret:%d", ((printfString) == NULL) ? "" : (printfString), ret); \
        return (ret); \
    } \
} while (0)
#define CHK_FAILED_RETURN(value, target, ret, print) \
do { \
    if ((value) != (target)) { \
        MEDIA_ERR_LOG("%s, %#x != %d ", ((print) == NULL) ? "" : (print), (value), (target)); \
        return (ret); \
    } \
} while (0)
#define CHK_FAILED_NO_RETURN(value, target, print) \
do { \
    if ((value) != (target)) { \
        MEDIA_ERR_LOG("%s, %#x != %d ", ((print) == NULL) ? "" : (print), (value), (target)); \
    } \
} while (0)

class IntellVoiceAsrEnginInterruptListener : public InterruptListener {
public:
    explicit IntellVoiceAsrEnginInterruptListener(IntellVoiceAsrEngineContext *context)
    {
        asrContext = context;
    };
    ~IntellVoiceAsrEnginInterruptListener() override {};

    void OnInterrupt(int32_t type, int32_t hint) override
    {
        MEDIA_INFO_LOG("type:%d hint:%d", type, hint);
        if (asrContext == nullptr) {
            MEDIA_ERR_LOG("context is nullptr");
            return;
        }
        if (type == INTERRUPT_TYPE_BEGIN && hint == INTERRUPT_HINT_PAUSE) {
            MEDIA_INFO_LOG("ASR INTERRUPT_HINT_PAUSE");
            asrContext->isPause = true;
        } else if (type == INTERRUPT_TYPE_END && hint == INTERRUPT_HINT_RESUME) {
            MEDIA_INFO_LOG("ASR INTERRUPT_HINT_RESUME");
            asrContext->isResumed = true;
        } else if (type == INTERRUPT_TYPE_BEGIN && hint == INTERRUPT_HINT_STOP) {
            MEDIA_INFO_LOG("ASR INTERRUPT_HINT_STOP");
            asrContext->isStopped = true;
        }
    }
private:
    IntellVoiceAsrEngineContext *asrContext = nullptr;
};

class PickupStreamInterruptListener : public InterruptListener {
public:
    PickupStreamInterruptListener() {};
    ~PickupStreamInterruptListener() override {};

    void OnInterrupt(int32_t type, int32_t hint) override
    {
        MEDIA_INFO_LOG("PickupStreamInterruptListener OnInterrupt, type:%d, hint:%d", type, hint);

        if (type == INTERRUPT_TYPE_BEGIN && hint == INTERRUPT_HINT_PAUSE) {
            MEDIA_INFO_LOG("OnInterrupt pause not supported");
        }
        if (type == INTERRUPT_TYPE_END && hint == INTERRUPT_HINT_RESUME) {
            MEDIA_INFO_LOG("OnInterrupt resume not supported");
        }
        if (type == INTERRUPT_TYPE_BEGIN && hint == INTERRUPT_HINT_STOP) {
            MEDIA_INFO_LOG("OnInterrupt stop not supported");
        }
        if (type == INTERRUPT_TYPE_END && hint == INTERRUPT_HINT_STOP) {
            MEDIA_INFO_LOG("OnInterrupt stop not supported");
        }
    }
};

static AsrEngineCmdInfo *IntellVoiceGetAsrResult(int32_t index)
{
    uint32_t size = sizeof(g_asrEngineCmdInfo) / sizeof(g_asrEngineCmdInfo[0]);
    for (uint32_t i = 0; i < size; i++) {
        if (g_asrEngineCmdInfo[i].index > 0 && g_asrEngineCmdInfo[i].index == index) {
            return &g_asrEngineCmdInfo[i];
        }
    }
    return nullptr;
}

static AsrEngineCmdInfo *IntellVoiceGetKwsResult(int32_t index)
{
    uint32_t size = sizeof(g_kwsEngineCmdInfo) / sizeof(g_kwsEngineCmdInfo[0]);
    for (uint32_t i = 0; i < size; i++) {
        if (g_kwsEngineCmdInfo[i].index == index) {
            return &g_kwsEngineCmdInfo[i];
        }
    }
    return nullptr;
}

static void IntellVoiceAsrCallBack(const IntellVoiceAsrEngineContext *asrContext, AsrEngineEvent event,
    void *data, uint32_t length)
{
    if (asrContext == NULL || asrContext->listener == NULL) {
        MEDIA_ERR_LOG("asrContext is NULL or asrContext listener is NULL!");
        return;
    }
    asrContext->listener(event, data, length);
}

static void IntellVoiceGetAlarmSlotInfo(AsrEngineCmdInfo *cmdInfo, const char *info)
{
    int32_t index = 0;
    int32_t slotType = 0;
    int32_t day = ASR_ENGINE_TIME_MAX;
    int32_t timePeriod = ASR_ENGINE_TIME_PERIOD_MAX;
    int32_t hour = 0;
    int32_t minute = 0;
    int32_t ret = sscanf_s((const char *)info, "%d#%d:%d,%d,%d,%d",
        &index, &slotType, &day, &timePeriod, &hour, &minute);
    if (ret == -1) {
        MEDIA_ERR_LOG("sscanf_s get slot param failed!");
        return;
    }
    cmdInfo->alarmControlInfo.alarmDateTime.day = static_cast<AsrEngineTimeDay>(day);
    cmdInfo->alarmControlInfo.alarmDateTime.timePeriod = static_cast<AsrEngineTimePeriod>(timePeriod);
    cmdInfo->alarmControlInfo.alarmDateTime.hour = hour;
    cmdInfo->alarmControlInfo.alarmDateTime.minute = minute;
}

static void IntellVoiceGetMotionSlotInfo(AsrEngineCmdInfo *cmdInfo, const char *info)
{
    int32_t index = 0;
    int32_t slotType = 0;
    float value = 0;
    int32_t type = ASR_ENGINE_TIME_PERIOD_MAX;
    int32_t ret = sscanf_s((const char *)info, "%d#%d:%f,%d",
        &index, &slotType, &value, &type);
    if (ret == -1) {
        MEDIA_ERR_LOG("sscanf_s get slot param failed!");
        return;
    }
    cmdInfo->motionControlInfo.motionTarget = static_cast<AsrEngineMotionTarget>(type);
    cmdInfo->motionControlInfo.value = value;
}

static void IntellVoiceGetTimeSlotInfo(AsrEngineCmdInfo *cmdInfo, const char *info)
{
    int32_t index = 0;
    int32_t slotType = 0;
    int32_t hour = 0;
    int32_t minute = 0;
    int32_t second = 0;
    int32_t ret = sscanf_s((const char *)info, "%d#%d:%d,%d,%d",
        &index, &slotType, &hour, &minute, &second);
    if (ret == -1) {
        MEDIA_ERR_LOG("sscanf_s get slot param failed!");
        return;
    }
    cmdInfo->timerControlInfo.hour = hour;
    cmdInfo->timerControlInfo.minute = minute;
    cmdInfo->timerControlInfo.second = second;
}

static IntellVoiceSlotInfo g_slotInfoInvoke[] = {
    {ASR_ENGINE_CMD_ALARM_CONTROL, IntellVoiceGetAlarmSlotInfo},
    {ASR_ENGINE_CMD_MOTION_CONTROL, IntellVoiceGetMotionSlotInfo},
    {ASR_ENGINE_CMD_TIMER_CONTROL, IntellVoiceGetTimeSlotInfo},
};

static void IntellVoiceGetSlotInfo(AsrEngineCmdInfo *cmdInfo, const char *info)
{
    if (cmdInfo == NULL) {
        MEDIA_ERR_LOG("Invalid cmdInfo!");
        return;
    }
    bool isfound = false;
    size_t size = sizeof(g_slotInfoInvoke) / sizeof(IntellVoiceSlotInfo);
    for (size_t i = 0; i < size; i++) {
        if (g_slotInfoInvoke[i].cmdId == cmdInfo->cmdId) {
            g_slotInfoInvoke[i].invoke(cmdInfo, info);
            isfound = true;
            break;
        }
    }
    if (!isfound) {
        MEDIA_ERR_LOG("not found support slot func,cmdid = %d!", cmdInfo->cmdId);
    }
}

static void IntellVoiceCapturerCallbackHandle(IntellVoiceAsrEngineContext *asrContext,
    AsrEngineEvent eventType, const char *info)
{
    AsrEngineCmdInfo *cmdInfo = nullptr;
    uint32_t size = 0;
    if (asrContext->streamType == AUDIO_STREAM_VOICE_RECOGNITION) {
        cmdInfo = IntellVoiceGetKwsResult(static_cast<uint32_t>(reinterpret_cast<uintptr_t>(info)));
        size = sizeof(AsrEngineCmdInfo);
    } else if (asrContext->streamType == AUDIO_STREAM_VOICE_ASSISTANT) {
        int32_t index = 0;
        int32_t slotType = 0;
        int32_t ret = sscanf_s((const char *)info, "%d#%d:", &index, &slotType);
        if (ret == -1) {
            MEDIA_ERR_LOG("sscanf_s get index and slot type failed!");
            return;
        }
        MEDIA_INFO_LOG("result index = %d, slot type = %d!", index, slotType);
        cmdInfo = IntellVoiceGetAsrResult(index);
        size = sizeof(AsrEngineCmdInfo);
        if (slotType > 0) {
            IntellVoiceGetSlotInfo(cmdInfo, info);
        }
    }
    asrContext->listener(eventType, cmdInfo, size);
}

static int32_t IntellVoiceCapturerCallback(const void *caller, AudioCallbackEventType eventType, const void *info)
{
    CHK_NULL_RETURN(caller, MEDIA_INVALID_PARAM, "audioCtx is Null!");
    IntellVoiceAsrEngineContext *asrContext = (IntellVoiceAsrEngineContext *)caller;
    CHK_NULL_RETURN(asrContext->listener, MEDIA_INVALID_PARAM, "asrContext listener is Null!");
    CHK_NULL_RETURN(info, MEDIA_INVALID_PARAM, "info is Null!");
    switch (eventType) {
        case AUDIO_EVENT_VOICE_BEGIN:
            asrContext->listener(ASR_ENGINE_EVENT_VOICE_BEGIN, nullptr, 0);
            break;
        case AUDIO_EVENT_VOICE_END:
            asrContext->listener(ASR_ENGINE_EVENT_VOICE_END, nullptr, 0);
            break;
        case AUDIO_EVENT_VOICE_COMMAND:
            IntellVoiceCapturerCallbackHandle(asrContext, ASR_ENGINE_EVENT_VOICE_COMMAND, (const char *)info);
            break;
        case AUDIO_EVENT_ERROR_OCCUR:
            IntellVoiceCapturerCallbackHandle(asrContext, ASR_ENGINE_EVENT_VOICE_ERROR, (const char *)info);
            break;
        default:
            asrContext->listener(ASR_ENGINE_EVENT_VOICE_ERROR, nullptr, 0);
            break;
    }
    return MEDIA_OK;
}

static void DeInitNpuContext(IntellVoiceAsrEngineContext *asrContext)
{
    if (asrContext == NULL || asrContext->npuContext == NULL) {
        MEDIA_ERR_LOG("asrContext is NULL!");
        return;
    }
    (void)MediaMutexDestroy(&asrContext->npuContext->npuMutex);
    (void)MediaThreadCondDestroy(&asrContext->npuContext->npuCond);
    if (asrContext->npuContext != nullptr) {
        MediaMemFree(asrContext->npuContext);
        asrContext->npuContext = nullptr;
    }
}

static int32_t InitNpuContext(IntellVoiceAsrEngineContext *asrContext)
{
    asrContext->npuContext = (IntellVoiceAsrNpuContext *)MediaMemCalloc(sizeof(IntellVoiceAsrNpuContext));
    if (asrContext->npuContext == nullptr) {
        MEDIA_ERR_LOG("malloc playContext failed");
        return MEDIA_ERR;
    }
    asrContext->npuContext->npuMutex = MediaMutexCreate(nullptr);
    if (asrContext->npuContext->npuMutex == nullptr) {
        MEDIA_ERR_LOG("create play mutex failed");
        goto EXIT;
    }
    asrContext->npuContext->npuCond = MediaThreadCondCreate();
    if (asrContext->npuContext->npuCond == nullptr) {
        MEDIA_ERR_LOG("create play cond failed");
        goto EXIT;
    }
    asrContext->npuContext->npuTaskHandle = nullptr;
    return MEDIA_OK;
EXIT:
    DeInitNpuContext(asrContext);
    return MEDIA_ERR;
}

static void DeInitProduceContext(IntellVoiceAsrEngineContext *asrContext)
{
    if (asrContext == NULL || asrContext->produceContext == NULL) {
        MEDIA_ERR_LOG("asrContext or produceContext is NULL!");
        return;
    }
    (void)MediaMutexDestroy(&asrContext->produceContext->produceMutex);
    (void)MediaThreadCondDestroy(&asrContext->produceContext->produceCond);
    if (asrContext->produceContext != nullptr) {
        MediaMemFree(asrContext->produceContext);
        asrContext->produceContext = nullptr;
    }
}

static int32_t InitProduceContext(IntellVoiceAsrEngineContext *asrContext)
{
    asrContext->produceContext = (IntellVoiceAsrProduceContext *)MediaMemCalloc(sizeof(IntellVoiceAsrProduceContext));
    if (asrContext->produceContext == nullptr) {
        MEDIA_ERR_LOG("malloc produceContext failed!");
        return MEDIA_ERR;
    }
    asrContext->produceContext->produceMutex = MediaMutexCreate(nullptr);
    if (asrContext->produceContext->produceMutex == nullptr) {
        MEDIA_ERR_LOG("create produce mutex failed");
        goto EXIT;
    }
    asrContext->produceContext->produceCond = MediaThreadCondCreate();
    if (asrContext->produceContext->produceCond == nullptr) {
        MEDIA_ERR_LOG("create produce cond failed!");
        goto EXIT;
    }
    asrContext->produceContext->produceTaskHandle = nullptr;
    asrContext->produceContext->isThreadActive = false;
    asrContext->produceContext->produceSleepTime = 0;
    return MEDIA_OK;
EXIT:
    DeInitProduceContext(asrContext);
    return MEDIA_ERR;
}

static int32_t IntellVoiceAsrContextInit(const AsrEngineInitParams *initParams)
{
    if (initParams == nullptr) {
        MEDIA_INFO_LOG("initParams is null!");
        return MEDIA_OK;
    }
    g_asrCtx = new IntellVoiceAsrEngineContext();
    if (g_asrCtx == nullptr) {
        MEDIA_ERR_LOG("malloc failed");
        return MEDIA_ERR;
    }
    g_asrCtx->sessionId = AUDIO_SESSION_ID_NONE;
    g_asrCtx->asiHandle = nullptr;
    g_asrCtx->isPause = false;
    g_asrCtx->isStopped = false;
    g_asrCtx->isResumed = false;
    g_asrCtx->language = initParams->language;
    g_asrCtx->workMode = initParams->workMode;
    g_asrCtx->enableNpu = initParams->enableNpu;
    g_asrCtx->listener = initParams->listener;
    g_asrCtx->states = IntellVoiceAsrStates::INTELL_VOICE_ASR_IDLE;
    int32_t ret = InitNpuContext(g_asrCtx);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("InitNpuContext failed!");
        goto EXIT2;
    }
    ret = InitProduceContext(g_asrCtx);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("InitProduceContext failed!");
        goto EXIT1;
    }
    g_asrCtx->audioCtx = (AudioProcessContext *)MediaMemCalloc(sizeof(AudioProcessContext));
    if (g_asrCtx->audioCtx == nullptr) {
        MEDIA_ERR_LOG("malloc audioCtx failed");
        goto EXIT0;
    }
    g_asrCtx->audioCtx->asrCtx = g_asrCtx;
    return MEDIA_OK;
EXIT0:
    DeInitProduceContext(g_asrCtx);
EXIT1:
    DeInitNpuContext(g_asrCtx);
EXIT2:
    delete g_asrCtx;
    g_asrCtx = nullptr;
    return MEDIA_ERR;
}

static void IntellVoiceInputConfigInit(IntellVoiceAsrEngineContext *asrContext)
{
    asrContext->asrInputConfig.audioFormat = PCM;
    asrContext->asrInputConfig.streamType = asrContext->streamType;
    asrContext->asrInputConfig.sampleRate = DEFAULT_SAMPLE_RATE;
    asrContext->asrInputConfig.channelCount = 1;
    asrContext->asrInputConfig.linkDir = AUDIO_UP_LINK;
    asrContext->asrInputConfig.disableSEA = false;
    asrContext->asrInputConfig.micType = CAPTURE_MIC_AMIC_0;
    asrContext->asrInputConfig.bitWidth = BIT_WIDTH_16;
    asrContext->asrInputConfig.isAlwaysWake = false;
    if (asrContext->workMode == ASR_ENGINE_ALWAYS_WAKE) {
        asrContext->asrInputConfig.isAlwaysWake = true;
    }
}

static void NpuMoudleDeInit(AudioProcessContext *audioCtx)
{
    if (audioCtx == NULL) {
        return;
    }
    if (audioCtx->asrCoeff != nullptr) {
        if (audioCtx->asrCoeff->pos_emb_pe != nullptr) {
            MediaMemFree(audioCtx->asrCoeff->pos_emb_pe);
            audioCtx->asrCoeff->pos_emb_pe = nullptr;
        }
        if (audioCtx->asrCoeff->ctc_out != nullptr) {
            MediaMemFree(audioCtx->asrCoeff->ctc_out);
            audioCtx->asrCoeff->ctc_out = nullptr;
        }
        MediaMemFree(audioCtx->asrCoeff);
        audioCtx->asrCoeff = nullptr;
    }
    if (audioCtx->curHyps != nullptr) {
        MediaMemFree(audioCtx->curHyps);
        audioCtx->curHyps = nullptr;
    }
    if (audioCtx->asrInput != nullptr) {
        MediaMemFree(audioCtx->asrInput);
        audioCtx->asrInput = nullptr;
    }
    if (audioCtx->inputData != nullptr) {
        MediaMemFree(audioCtx->inputData);
        audioCtx->inputData = nullptr;
    }
    if (audioCtx->nluOutSlotData != nullptr) {
        MediaMemFree(audioCtx->nluOutSlotData);
        audioCtx->nluOutSlotData = nullptr;
    }
    if (audioCtx->ffbankNew != nullptr) {
        MediaMemFree(audioCtx->ffbankNew);
        audioCtx->ffbankNew = nullptr;
    }
    if (audioCtx->posEmb != nullptr) {
        MediaMemFree(audioCtx->posEmb);
        audioCtx->posEmb = nullptr;
    }
    asrctcModelDestroy();
    destroyModelEnv();
}

static int32_t NpuMoudleInitInner(AudioProcessContext *audioCtx)
{
    audioCtx->curHyps = (long_hyps *)MediaMemCalloc(sizeof(long_hyps));
    if (audioCtx->curHyps == nullptr) {
        MEDIA_ERR_LOG("malloc curHyps failed!");
        return MEDIA_ERR;
    }
    audioCtx->ffbankNew = (uint16_t *)MediaMemMalloc(DEFAULT_FFBANK_35_SIZE * sizeof(imedia_uint16));
    if (audioCtx->ffbankNew == nullptr) {
        MEDIA_ERR_LOG("malloc ffbankNew failed!");
        return MEDIA_ERR;
    }
    audioCtx->posEmb = (uint16_t *)MediaMemMalloc(DEFAULT_POS_EMB_SIZE * sizeof(imedia_uint16));
    if (audioCtx->posEmb == nullptr) {
        MEDIA_ERR_LOG("malloc pos_emb failed!");
        return MEDIA_ERR;
    }
    audioCtx->curHyps->candidate[0].n_pb = 0;
    audioCtx->curHyps->candidate[0].n_pnb = NEG_INF_VALUE;
    audioCtx->curHyps->candidate[0].len = 0;
    audioCtx->curHyps->len = 1;
    audioCtx->curHypsLastLen = 0;
    audioCtx->curHypsSameNumLen = 0;
    audioCtx->curHypsSameNumLenExt = 0;
    audioCtx->file = fopen(POS_EMB_PE_FILE_PATH, "rb");
    if (audioCtx->file == nullptr) {
        MEDIA_ERR_LOG("fopen failed");
        return MEDIA_ERR;
    }
    size_t readLen = fread(audioCtx->asrCoeff->pos_emb_pe, sizeof(imedia_uint16),
        POS_EMB_ROW * POS_EMB_COL, audioCtx->file);
    if (readLen != POS_EMB_ROW * POS_EMB_COL) {
        MEDIA_ERR_LOG("posEmbPe read file failed! readLen = %d", readLen);
    }
    (void)fclose(audioCtx->file);
    audioCtx->file = nullptr;
    size_t size = sizeof(audioCtx->ppsDataTmp);
    if (memset_s(audioCtx->ppsDataTmp, size, 0, size) != EOK) {
        MEDIA_ERR_LOG("ppsDataTmp memset_s failed!");
        return MEDIA_ERR;
    }
    return MEDIA_OK;
}

int32_t NpuMoudleInit(AudioProcessContext *audioCtx)
{
    CHK_NULL_RETURN(audioCtx, MEDIA_INVALID_PARAM, "audioCtx is Null!");
    Result ret = initModelEnv();
    if (ret != SUCCESS) {
        MEDIA_ERR_LOG("initModelEnv failed!");
        return MEDIA_ERR;
    }
    int32_t retValue = 0;
    audioCtx->asrCoeff = (asr_pre_post_para *)MediaMemMalloc(sizeof(asr_pre_post_para));
    if (audioCtx->asrCoeff == nullptr) {
        MEDIA_ERR_LOG("MediaMemMalloc failed!");
        goto DESTORY;
    }
    audioCtx->asrInput = (stru_asr_in *)MediaMemMalloc(sizeof(stru_asr_in));
    if (audioCtx->asrInput == nullptr) {
        MEDIA_ERR_LOG("MediaMemMalloc failed!");
        goto DESTORY;
    }
    audioCtx->asrCoeff->pos_emb_pe = (uint16_t *)MediaMemMalloc(POS_EMB_ROW * POS_EMB_COL * sizeof(imedia_uint16) * FRAME_CNT_RESET);
    if (audioCtx->asrCoeff->pos_emb_pe == NULL) {
        MEDIA_ERR_LOG("MediaMemMalloc posEmbPe failed!");
        goto DESTORY;
    }
    retValue = NpuMoudleInitInner(audioCtx);
    if (retValue != MEDIA_OK) {
        MEDIA_ERR_LOG("NpuMoudleInitInner failed!");
        goto DESTORY;
    }
    audioCtx->inputData = (int16_t *)MediaMemMalloc(DEFAULT_INPUT_BUFFER_SIZE * sizeof(imedia_int16));
    if (audioCtx->inputData == nullptr) {
        MEDIA_ERR_LOG("malloc input data buffer failed!");
        goto DESTORY;
    }
    audio_vqe_common_vec_set_int16(audioCtx->inputData, DEFAULT_INPUT_BUFFER_SIZE, 0);
    audioCtx->isfirst = true;
    audioCtx->frmCnt = 0;
    audioCtx->chunkNum = 0;
    return MEDIA_OK;
DESTORY:
    NpuMoudleDeInit(audioCtx);
    return MEDIA_ERR;
}

static int32_t NluInputDataFromCtcModule(AudioProcessContext *audioCtx, nlu_in *nluInput)
{
    CHK_NULL_RETURN(audioCtx, MEDIA_INVALID_PARAM, "audioCtx is Null!");
    CHK_NULL_RETURN(nluInput, MEDIA_INVALID_PARAM, "nluInput is Null!");
    for (int32_t i = 0; i < NLU_INPUT_LENGTH; i++) {
        nluInput->seq_data[i] = 1;
    }
    nluInput->seq_len = 0;
    if ((audioCtx->curHyps->len != 0) && (audioCtx->curHyps->candidate[0].len != 0)) {
        for (int32_t i = 0; i < audioCtx->curHyps->candidate[0].len; i++) {
            nluInput->seq_data[i] = map_asr2nlu[audioCtx->curHyps->candidate[0].prefix[i]];
        }
        nluInput->seq_len = audioCtx->curHyps->candidate[0].len;
        for (int32_t i = audioCtx->curHyps->candidate[0].len; i < NLU_INPUT_LENGTH; i++) {
            nluInput->seq_data[i] = 1;
        }
    }
    return MEDIA_OK;
}

static int32_t AsrMoudleInit(AudioProcessContext *audioCtx)
{
    CHK_NULL_RETURN(audioCtx, MEDIA_INVALID_PARAM, "audioCtx is Null!");
    if (!audioCtx->isfirst) {
        return MEDIA_OK;
    }
    if (asrctcModelInit(ASR_MODEL_NAME, ASR_MODEL_OFFSET_PATH) != SUCCESS) {
        MEDIA_ERR_LOG("asrModelInit failed!");
        return MEDIA_ERR;
    }
    asrctcModelCnnCacheReset();
    audioCtx->isfirst = false;
    MEDIA_INFO_LOG("asrctcModelInit success");
    return MEDIA_OK;
}

static void HandleHypsDetect(AudioProcessContext *audioCtx)
{
    int32_t lenHyps = audioCtx->curHyps->candidate[0].len;
    if (((lenHyps == audioCtx->curHypsLastLen) && (lenHyps != 0))) {
        audioCtx->curHypsSameNumLen += 1;
    } else {
        audioCtx->curHypsSameNumLen = 0;
    }

    if (audioCtx->curHypsSameNumLen >= 0x3) {
        audioCtx->asrInput->ctc_flag = 1;
    }
    audioCtx->curHypsLastLen = lenHyps;
    audioCtx->chunkNum = audioCtx->chunkNum + 1;
    if (lenHyps == 0) {
        audioCtx->curHypsSameNumLenExt += 1;
    } else {
        audioCtx->curHypsSameNumLenExt = 0;
    }

    if (audioCtx->curHypsSameNumLenExt >= HYPS_CHUNK_CUM) {
        asrctcModelDestroy();
        audioCtx->isfirst = true;
        audioCtx->asrInput->ctc_flag = 0;
        audioCtx->asrInput->frm_cnt = FRAME_CNT_RESET;
        audioCtx->curHypsSameNumLenExt = 0;
        audioCtx->chunkNum = 0;
    }
}

static int32_t CtcModuleProcessesAudioData(AudioProcessContext *audioCtx)
{
    CHK_NULL_RETURN(audioCtx, MEDIA_INVALID_PARAM, "audioCtx is Null!");
    audioCtx->ctcModelOutput = MediaMemMalloc(CTC_OUTPUT_SIZE);
    if (audioCtx->ctcModelOutput == nullptr) {
        MEDIA_ERR_LOG("ctcModelOutput malloc failed!");
        goto EXIT;
    }
    if (asrctcModelCopyOutput(audioCtx->ctcModelOutput) != SUCCESS) {
        MediaMemFree(audioCtx->ctcModelOutput);
        audioCtx->ctcModelOutput = nullptr;
        MEDIA_ERR_LOG("ctcModelCopyOutput failed!");
        goto EXIT;
    }
    audioCtx->asrCoeff->ctc_out = (float *)MediaMemMalloc(CTC_CHUNK_SIZE_8 * CTC_CHAR_LEN_1289 * sizeof(float));
    if (audioCtx->asrCoeff->ctc_out == nullptr) {
        MediaMemFree(audioCtx->ctcModelOutput);
        audioCtx->ctcModelOutput = nullptr;
        MEDIA_ERR_LOG("ctc_out malloc failed!");
        goto EXIT;
    }
    for (int32_t i = 0; i < CTC_CHUNK_SIZE_8; i++) {
        for (int32_t j = 0; j < CTC_CHAR_LEN_1289; j++) {
            fp162fp32(&audioCtx->asrCoeff->ctc_out[i * CTC_CHAR_LEN_1289 + j],
                ((int16_t *)audioCtx->ctcModelOutput)[i * CTC_CHAR_LEN_1289 + j]);
        }
    }
    MediaMemFree(audioCtx->ctcModelOutput);
    audioCtx->ctcModelOutput = nullptr;
    ctc_prefix_beamsearch(audioCtx->asrCoeff->ctc_out, audioCtx->curHyps, audioCtx->chunkNum);
    MediaMemFree(audioCtx->asrCoeff->ctc_out);
    audioCtx->asrCoeff->ctc_out = nullptr;
    return MEDIA_OK;
EXIT:
    asrctcModelDestroy();
    return MEDIA_ERR;
}

static int32_t NluModuleExecute(AudioProcessContext *audioCtx, nlu_in *nluInput)
{
    CHK_NULL_RETURN(audioCtx, MEDIA_INVALID_PARAM, "audioCtx is Null!");
    CHK_NULL_RETURN(nluInput, MEDIA_INVALID_PARAM, "nluInput is Null!");
    if (nluModelInit(NLU_MODEL_NAME) != SUCCESS) {
        MEDIA_ERR_LOG("nluModelInit failed!");
        goto EXIT;
    }
    if (nluModelExecute((void *)nluInput->seq_data, nluInput->seq_len) != SUCCESS) {
        nluModelDestroy();
        MEDIA_ERR_LOG("nluModelExecute failed!");
        goto EXIT;
    }
    return MEDIA_OK;
EXIT:
    return MEDIA_ERR;
}

static int32_t CopyNluModuleResultData(AudioProcessContext *audioCtx, float *nluOutVec)
{
    CHK_NULL_RETURN(audioCtx, MEDIA_INVALID_PARAM, "audioCtx is Null!");
    CHK_NULL_RETURN(nluOutVec, MEDIA_INVALID_PARAM, "nluOutVec is Null!");
    short *nluTemp = NULL;
    void *nluModelOutput = MediaMemMalloc(NLU_OUTPUT_SIZE);
    if (nluModelOutput == nullptr) {
        MEDIA_ERR_LOG("nluModelOutput malloc failed!");
        goto EXIT1;
    }
    if (nluModelCopyOutput(nluModelOutput) != SUCCESS) {
        MEDIA_ERR_LOG("nluModelCopyOutput failed!");
        goto EXIT2;
    }
    audioCtx->nluOutSlotData = (char *)MediaMemMalloc(NLU_OUT_SLOT_DATA_SIZE);
    if (nluModelCopyOutputSlot(audioCtx->nluOutSlotData) != SUCCESS) {
        goto EXIT2;
    }
    nluModelDestroy();
    nluTemp = (short *)nluModelOutput;
    for (uint32_t i = 0; i < COMMAND_106_NUM; i++) {
        fp162fp32(&nluOutVec[i], nluTemp[i]);
    }
    MediaMemFree(nluModelOutput);
    return MEDIA_OK;
EXIT2:
    MediaMemFree(nluModelOutput);
EXIT1:
    nluModelDestroy();
    return MEDIA_ERR;
}

static int32_t NluModuleProcessesSlotData(AudioProcessContext *audioCtx, nlu_in *nluInput, uint32_t nluOutIndex)
{
    getSlotMax((float *)audioCtx->nluOutSlotData, nluInput->seq_len, g_nluOutslotData);
    (void)memset_s(g_nluPostprocIn, NLU_POST_MAX_SIZE, 0, NLU_POST_MAX_SIZE);
    int32_t ret = nlu_net_mapto_postproc(nluInput->seq_data, nluOutIndex,
        g_nluOutslotData, nluInput->seq_len, g_nluPostprocIn);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("nlu_net_mapto_postproc failed!");
        return MEDIA_ERR;
    }
    void *handle = NULL;
    ret = slu_slot_convert_init(&handle, 0);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("slu_slot_convert_init failed!");
        return MEDIA_ERR;
    }
    char g_nluOutSlotInfo[NLU_OUT_SLOT_INFO_SIZE] = {};
    (void)nlu_slot_postproc(handle, g_nluPostprocIn, g_nluOutSlotInfo);
    (void)slu_slot_convert_deinit(handle, 0);
    if (audioCtx->nluOutSlotData != nullptr) {
        MediaMemFree(audioCtx->nluOutSlotData);
        audioCtx->nluOutSlotData = nullptr;
    }
    IntellVoiceCapturerCallback(audioCtx->asrCtx, AUDIO_EVENT_VOICE_COMMAND, g_nluOutSlotInfo);
    return MEDIA_OK;
}

static int32_t NluModuleProcessesData(AudioProcessContext *audioCtx, nlu_in *nluInput, int32_t early)
{
    CHK_NULL_RETURN(audioCtx, MEDIA_INVALID_PARAM, "audioCtx is Null!");
    CHK_NULL_RETURN(nluInput, MEDIA_INVALID_PARAM, "nluInput is Null!");
    int32_t ret = NluModuleExecute(audioCtx, nluInput);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("NluModuleExecute failed!");
        return MEDIA_ERR;
    }
    float *nluOutVec = (float *)MediaMemMalloc(COMMAND_106_NUM * sizeof(float));
    if (nluOutVec == nullptr) {
        MEDIA_ERR_LOG("nluOutVec malloc failed!");
        return MEDIA_ERR;
    }
    ret = CopyNluModuleResultData(audioCtx, nluOutVec);
    if (ret != MEDIA_OK) {
        MediaMemFree(nluOutVec);
        MEDIA_ERR_LOG("AudioDataByNluOutput failed!");
        return MEDIA_ERR;
    }
    uint32_t nluOutIndex = DEFAULT_RESULT_START_INDEX;
    float nluMax = nluOutVec[DEFAULT_RESULT_START_INDEX];
    for (uint32_t i = 3; i < COMMAND_106_NUM; i++) {
        if (nluMax < nluOutVec[i]) {
            nluOutIndex = i;
            nluMax = nluOutVec[i];
        }
    }
    audioCtx->chunkNum = 0;
    audioCtx->asrInput->ctc_flag = 0;
    MediaMemFree(nluOutVec);
    MEDIA_INFO_LOG("nluOutIndex = %d!", nluOutIndex);
    return NluModuleProcessesSlotData(audioCtx, nluInput, nluOutIndex);
}

static int32_t SendAudioDataToFFbank(AudioProcessContext *audioCtx)
{
    CHK_NULL_RETURN(audioCtx, MEDIA_INVALID_PARAM, "audioCtx is Null!");
    uint32_t index = DEFAULT_ONE_FRAME_LEN * DEFAULT_LOOP_COUNT;
    audio_vqe_common_vec_copy_int16_to_int16(audioCtx->ppsDataTmp[0],
        DEFAULT_ONE_FRAME_LEN, &audioCtx->inputData[index]);
    imedia_uint16 *ffbank = (imedia_uint16 *)MediaMemMalloc(DEFAULT_HALF_FRAME_LEN * sizeof(imedia_uint16));
    if (ffbank == nullptr) {
        MEDIA_ERR_LOG("ffbank malloc failed!");
        return MEDIA_ERR;
    }
    cal_ffbank(audioCtx->inputData, ffbank);
    get_encoder_input(ffbank, audioCtx->asrInput, audioCtx->asrCoeff);
    MediaMemFree(ffbank);
    return MEDIA_OK;
}

static void DestoryAIMoudleSource(AudioProcessContext *audioCtx)
{
    if (audioCtx == NULL) {
        return;
    }
    if (audioCtx->posEmb != nullptr) {
        MediaMemFree(audioCtx->posEmb);
        audioCtx->posEmb = nullptr;
    }
    if (audioCtx->ffbankNew != nullptr) {
        MediaMemFree(audioCtx->ffbankNew);
        audioCtx->ffbankNew = nullptr;
    }
    if (audioCtx->nluOutSlotData != nullptr) {
        MediaMemFree(audioCtx->nluOutSlotData);
        audioCtx->nluOutSlotData = nullptr;
    }
}

static int32_t NpuModuleProcessesData(AudioProcessContext *audioCtx, int32_t early)
{
    CHK_NULL_RETURN(audioCtx, MEDIA_INVALID_PARAM, "audioCtx is Null!");
    nlu_in nluInput = {};
    if (NluInputDataFromCtcModule(audioCtx, &nluInput) != MEDIA_OK) {
        MEDIA_ERR_LOG("NluInputDataFromCtcModule failed!");
        goto DESTROY;
    }
    if (NluModuleProcessesData(audioCtx, &nluInput, early) != MEDIA_OK) {
        MEDIA_ERR_LOG("NluModuleProcessesData failed!");
        goto DESTROY;
    }
    audioCtx->isfirst = true;
    audioCtx->asrInput->ctc_flag = 0;
    audioCtx->asrInput->frm_cnt = 0;
    if (memset_s(audioCtx->curHyps, sizeof(long_hyps), 0, sizeof(long_hyps)) != EOK) {
        MEDIA_ERR_LOG("memset_s failed!");
    }
    audioCtx->curHyps->candidate[0].n_pb = 0;
    audioCtx->curHyps->candidate[0].n_pnb = NEG_INF_VALUE;
    audioCtx->curHyps->candidate[0].len = 0;
    audioCtx->curHyps->len = 1;
    audioCtx->curHypsLastLen = 0;
    return MEDIA_OK;
DESTROY:
    DestoryAIMoudleSource(audioCtx);
    return MEDIA_ERR;
}

void CopyAudioData(AudioProcessContext *audioCtx, const int16_t *in8, int32_t channels, int32_t channelLen)
{
    int32_t srcIndex = 0;
    for (int32_t i = 0; i < channelLen; i++, srcIndex += channels) {
        for (int32_t j = 0; j < channels; j++) {
            audioCtx->ppsDataTmp[j][i] = in8[srcIndex + channels - 1 - j];
        }
    }
}

int32_t SendAudioDataToNpuMoudle(AudioProcessContext *audioCtx, int16_t *data, uint32_t dataLen)
{
    CHK_NULL_RETURN(audioCtx, MEDIA_INVALID_PARAM, "audioCtx is Null!");
    CHK_NULL_RETURN(data, MEDIA_INVALID_PARAM, "data is Null!");
    int32_t ret = AsrMoudleInit(audioCtx);
    CHK_FAILED_RETURN(ret, MEDIA_OK, ret, "AsrMoudleInit failed!");
    if (dataLen < FRAME_LAP * CHAN_NUM_IN) {
        audioCtx->asrInput->ctc_flag = 0;
        CHK_FAILED_RETURN(ret, MEDIA_OK, ret, "NpuModuleProcessesData failed!");
        return MEDIA_ERR;
    }
    CopyAudioData(audioCtx, (const int16_t *)data, CHAN_NUM_IN, FRAME_LAP);
    if (audioCtx->frmCnt == 0) {
        get_encoder_init(audioCtx->asrInput, audioCtx->ffbankNew, audioCtx->posEmb);
    }
    if (audioCtx->frmCnt < DEFAULT_LOOP_COUNT) { // Collect enough data to reach 25ms.
        uint32_t index = DEFAULT_ONE_FRAME_LEN * audioCtx->frmCnt;
        audio_vqe_common_vec_copy_int16_to_int16(audioCtx->ppsDataTmp[0],
            DEFAULT_ONE_FRAME_LEN, &audioCtx->inputData[index]);
        audioCtx->frmCnt++;
        return MEDIA_OK;
    } else {
        // Collect enough data to reach 25ms, send to ffbank
        ret = SendAudioDataToFFbank(audioCtx);
        CHK_FAILED_RETURN(ret, MEDIA_OK, ret, "SendAudioDataToFFbank failed!");
        // already has 35 frames
        if (audioCtx->asrInput->flag == 1) {
            if (asrctcModelExecute((void *)audioCtx->asrInput->fbank_in,
                (void *)audioCtx->asrInput->pos_emb) != SUCCESS) {
                asrctcModelDestroy();
                MEDIA_ERR_LOG("asrctcModelDestroy failed!");
                return MEDIA_ERR;
            }
            ret = CtcModuleProcessesAudioData(audioCtx);
            CHK_FAILED_RETURN(ret, MEDIA_OK, ret, "CtcModuleProcessesAudioData failed!");
            HandleHypsDetect(audioCtx);
            if (audioCtx->asrInput->ctc_flag == 1) { /* 15chunk asr output ready data for ctc inference */
                asrctcModelDestroy();
                ret = NpuModuleProcessesData(audioCtx, 0);
                CHK_FAILED_RETURN(ret, MEDIA_OK, ret, "NpuModuleProcessesData failed!");
            }
        }
        uint32_t size = DEFAULT_ONE_FRAME_LEN * DEFAULT_LOOP_COUNT;
        audio_vqe_common_vec_copy_int16_to_int16(&audioCtx->inputData[DEFAULT_ONE_FRAME_LEN], size, &audioCtx->inputData[0]);
        audioCtx->frmCnt = audioCtx->frmCnt > UINT_MAX ? DEFAULT_FRAME_COUNT : (audioCtx->frmCnt + 1);
    }
    return MEDIA_OK;
}

static int32_t IntellVoiceAsrSessionInit(IntellVoiceAsrEngineContext *asrContext)
{
    bool success = g_amInstance.Initialize();
    if (!success) {
        MEDIA_ERR_LOG("audio manager init failed");
        return MEDIA_ERR;
    }
    AudioSession sessionID = g_amInstance.MakeSessionId();
    if (sessionID == AUDIO_SESSION_ID_NONE) {
        MEDIA_ERR_LOG("get sessionid fail.\n");
        return MEDIA_ERR;
    }
    asrContext->streamType = AUDIO_STREAM_VOICE_RECOGNITION;
    if (asrContext->enableNpu) {
        asrContext->streamType = AUDIO_STREAM_VOICE_ASSISTANT;
    }
    asrContext->asrInputConfig.sessionID = sessionID;
    IntellVoiceInputConfigInit(asrContext);
    std::shared_ptr<IntellVoiceAsrEnginInterruptListener> listener =
        std::make_shared<IntellVoiceAsrEnginInterruptListener>(asrContext);
    if (listener.get() == nullptr) {
        MEDIA_ERR_LOG("listener is nullptr");
        return MEDIA_ERR;
    }
    asrContext->interrupt = { asrContext->streamType, sessionID, listener };
    int32_t strategy = g_amInstance.ActivateAudioInterrupt(asrContext->interrupt);
    if (strategy == INTERRUPT_FAILED) {
        MEDIA_ERR_LOG("activate audio interrupt faild");
        return MEDIA_ERR;
    }
    if (asrContext->enableNpu) {
        MEDIA_CHR_LOG(MT_MEDIA_CHR_SMART_VOICE_SREVICE, MEDIA_CHR_ARS_ENGINE_INIT,
            MEDIA_CHR_ARS_ENGINE_NPU_INIT, 0);
        uapi_npu_dlog_setlevel(0, 0x3, 1);
        int32_t ret = NpuMoudleInit(asrContext->audioCtx);
        CHK_FAILED_RETURN(ret, MEDIA_OK, ret, "intell voice npu init fail failed!");
    }
    return MEDIA_OK;
}

static int32_t IntellVoiceAsrInit(IntellVoiceAsrEngineContext *asrContext)
{
    int32_t ret = MEDIA_OK;
    asrContext->asiHandle = std::make_shared<AudioStreamIn>();
    if (asrContext->asiHandle.get() == nullptr) {
        MEDIA_ERR_LOG("audio stream out is nullptr");
        goto EXIT;
    }
    ret = asrContext->asiHandle->Init(asrContext->asrInputConfig, IntellVoiceCapturerCallback, asrContext);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("AudioStreamInInit fail \n");
        asrContext->asiHandle->Release();
        goto EXIT;
    }
    return MEDIA_OK;
EXIT:
    g_amInstance.DeactivateAudioInterrupt(asrContext->interrupt);
    return ret;
}

static void IntellVoiceAsrReleaseHandle(IntellVoiceAsrEngineContext *asrContext)
{
    if (asrContext->asiHandle != nullptr) {
        asrContext->asiHandle->Release();
        asrContext->asiHandle.reset();
        asrContext->asiHandle = nullptr;
    }
    g_amInstance.DeactivateAudioInterrupt(asrContext->interrupt);
}

static int32_t IntellVoiceAsrStart(IntellVoiceAsrEngineContext *asrContext)
{
    if (asrContext->asiHandle == nullptr) {
        MEDIA_ERR_LOG("asi handle has been destroyed!");
        return MEDIA_ERR;
    }
    int32_t ret = asrContext->asiHandle->Start();
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("AudioStreamInStart fail.");
        IntellVoiceAsrReleaseHandle(asrContext);
        return ret;
    }
    return MEDIA_OK;
}

static int32_t IntellVoiceAsrDestory(IntellVoiceAsrEngineContext *asrContext)
{
    CHK_NULL_RETURN(asrContext, MEDIA_ERR, "asrContext is NULL!");
    CHK_NULL_RETURN(asrContext->asiHandle, MEDIA_ERR, "asrContext->asiHandle is NULL!");
    int32_t ret = asrContext->asiHandle->Stop();
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("AudioStreamInStop fail.");
    }
    asrContext->asiHandle->Release();
    asrContext->asiHandle.reset();
    asrContext->asiHandle = nullptr;
    return MEDIA_OK;
}

int32_t IntellVoiceAsrEngineInit(AsrEngineInitParams *initParams)
{
    MediaInitStaticMutexLock(&g_asrMutex);
    MediaMutexLock(g_asrMutex);
    if (g_asrCtx != nullptr) {
        MEDIA_ERR_LOG("asr is exist, you need to delete first!");
        MediaMutexUnLock(g_asrMutex);
        return MEDIA_ERR;
    }
    int32_t ret = IntellVoiceAsrContextInit(initParams);
    if (ret != MEDIA_OK) {
        MediaMutexUnLock(g_asrMutex);
        return MEDIA_ERR;
    }
    MEDIA_CHR_LOG(MT_MEDIA_CHR_SMART_VOICE_SREVICE, MEDIA_CHR_ARS_ENGINE_INIT, 0, 0);
    ret = IntellVoiceAsrSessionInit(g_asrCtx);
    if (ret != MEDIA_OK) {
        IntellVoiceAsrEngineDeInit();
        MEDIA_ERR_LOG("session init fail!");
        return MEDIA_ERR;
    }
    g_asrCtx->states = IntellVoiceAsrStates::INTELL_VOICE_ASR_INIT;
    MediaMutexUnLock(g_asrMutex);
    return ret;
}

static void *IntellVoiceNpuThread(void *priv)
{
    IntellVoiceAsrEngineContext *asrContext = (IntellVoiceAsrEngineContext *)priv;
    uint32_t pcmDataSize = DEFAULT_RECORD_BUFFER_SIZE;
    char *pcmData = (char *)MediaMemCalloc(pcmDataSize);
    CHK_NULL_RETURN(pcmData, nullptr, "result malloc failed!");
    uint32_t readLen = 0;
    int32_t ret = 0;
    while (true) {
        MediaMutexLock(asrContext->npuContext->npuMutex);
        if (!asrContext->npuContext->isThreadActive) {
            MediaMutexUnLock(asrContext->npuContext->npuMutex);
            break;
        }
        MediaMutexUnLock(asrContext->npuContext->npuMutex);
        asrContext->npuContext->npuSleepTime = 0;
        uint32_t busySize = CircleBufferQueryBusy(&asrContext->pcmBufferInfo.circleBuffer);
        if (busySize < pcmDataSize) {
            asrContext->npuContext->npuSleepTime = DEFAULT_SLEEP_TIME_10_MS;
            goto CONTINUE;
        }
        readLen = CircleBufferRead(&asrContext->pcmBufferInfo.circleBuffer, (uint8_t *)pcmData, pcmDataSize);
        if (readLen != pcmDataSize) {
            MEDIA_ERR_LOG("CircleBuffer Read pcm data failed! readLen = %d, frameSize = %u", readLen, pcmDataSize);
            IntellVoiceAsrCallBack(asrContext, ASR_ENGINE_EVENT_VOICE_ERROR, nullptr, 0);
            break;
        }
        ret = SendAudioDataToNpuMoudle(asrContext->audioCtx, (int16_t *)pcmData, readLen >> 1);
        if (ret != MEDIA_OK) {
            MEDIA_ERR_LOG("SendAudioDataToNpuMoudle failed:0x%x", ret);
            IntellVoiceAsrCallBack(asrContext, ASR_ENGINE_EVENT_VOICE_ERROR, nullptr, 0);
            break;
        }
    CONTINUE:
        if (asrContext->npuContext->npuSleepTime > 0) {
            MediaMutexLock(asrContext->npuContext->npuMutex);
            MediaThreadCondTimeWait(asrContext->npuContext->npuCond, asrContext->npuContext->npuMutex,
                asrContext->npuContext->npuSleepTime);
            MediaMutexUnLock(asrContext->npuContext->npuMutex);
        }
    }
    MediaMemFree(pcmData);
    MEDIA_CHR_LOG(MT_MEDIA_CHR_SMART_VOICE_SREVICE, MEDIA_CHR_ARS_ENGINE_START,
        MEDIA_CHR_ARS_ENGINE_CREATE_NPU_THREAD, MEDIA_CHR_ARS_DESTROY_NPU_SOURCE);
    NpuMoudleDeInit(asrContext->audioCtx);
    MEDIA_INFO_LOG("IntellVoiceNpuThread exit!");
    return nullptr;
}

static void CacheMemoryInfoDeInit(IntellVoiceAsrEngineContext *asrContext)
{
    if (asrContext->pcmBufferInfo.buf != nullptr) {
        MediaMemFree(asrContext->pcmBufferInfo.buf);
        asrContext->pcmBufferInfo.buf = nullptr;
    }
    if (asrContext->pcmBufferInfo.circleBuffer.lock != nullptr) {
        MediaMutexDestroy(&asrContext->pcmBufferInfo.circleBuffer.lock);
    }
}

static int32_t CacheMemoryInfoInit(IntellVoiceAsrEngineContext *asrContext)
{
    asrContext->pcmBufferInfo.buf = (uint8_t *)MediaMemCalloc(PCM_BUFFER_SIZE);
    if (asrContext->pcmBufferInfo.buf == nullptr) {
        MEDIA_ERR_LOG("pcm buf malloc failed!");
        return MEDIA_ERR;
    }
    asrContext->pcmBufferInfo.read = 0;
    asrContext->pcmBufferInfo.write = 0;
    asrContext->pcmBufferInfo.size = PCM_BUFFER_SIZE;
    CircleBufferinit(&asrContext->pcmBufferInfo.circleBuffer, &asrContext->pcmBufferInfo.write,
        &asrContext->pcmBufferInfo.read, asrContext->pcmBufferInfo.buf, PCM_BUFFER_SIZE);
    asrContext->pcmBufferInfo.circleBuffer.lock = MediaMutexCreate(nullptr);
    if (asrContext->pcmBufferInfo.circleBuffer.lock == nullptr) {
        MEDIA_ERR_LOG("pcmBufferInfo lock create failed!");
        goto EXIT;
    }
    return MEDIA_OK;
EXIT:
    CacheMemoryInfoDeInit(asrContext);
    return MEDIA_ERR;
}

static void IntellVoiceAsrDestroyStream(IntellVoiceAsrEngineContext *asrContext, CapturerBuffer *capBuf)
{
    if (capBuf->data != nullptr) {
        MediaMemFree(capBuf->data);
        capBuf->data = nullptr;
    }
    IntellVoiceAsrDestory(asrContext);
}

static int32_t IntellVoiceAsrCreateStream(IntellVoiceAsrEngineContext *asrContext, CapturerBuffer *capBuf)
{
    MEDIA_CHR_LOG(MT_MEDIA_CHR_SMART_VOICE_SREVICE, MEDIA_CHR_ARS_ENGINE_START,
        MEDIA_CHR_ARS_ENGINE_CREATE_NPU_THREAD, MEDIA_CHR_ARS_CREATE_NPU_STREAM_START);
    int32_t ret = IntellVoiceAsrInit(asrContext);
    CHK_FAILED_RETURN(ret, MEDIA_OK, MEDIA_ERR, "intell voice asr init fail!");
    ret = IntellVoiceAsrStart(asrContext);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("intell voice asr start fail.\n");
        return MEDIA_ERR;
    }
    MEDIA_CHR_LOG(MT_MEDIA_CHR_SMART_VOICE_SREVICE, MEDIA_CHR_ARS_ENGINE_START,
        MEDIA_CHR_ARS_ENGINE_CREATE_NPU_THREAD, MEDIA_CHR_ARS_CREATE_NPU_STREAM_SUCCESS);
    capBuf->size = DEFAULT_RECORD_BUFFER_SIZE;
    capBuf->data = (uint8_t *)MediaMemMalloc(capBuf->size);
    if (capBuf->data == nullptr) {
        MEDIA_ERR_LOG("capBuf.data is NULL.");
        IntellVoiceAsrDestroyStream(asrContext, capBuf);
        return MEDIA_ERR;
    }
    capBuf->frameCount = 1;
    return MEDIA_OK;
}

static void IntellVoiceThreadSleep(IntellVoiceAsrEngineContext *asrContext)
{
    if (asrContext->produceContext->produceSleepTime > 0) {
        MediaMutexLock(asrContext->produceContext->produceMutex);
        MediaThreadCondTimeWait(asrContext->produceContext->produceCond, asrContext->produceContext->produceMutex,
            asrContext->produceContext->produceSleepTime);
        MediaMutexUnLock(asrContext->produceContext->produceMutex);
    }
}

static void *IntellVoiceProduceAudioData(void *priv)
{
    IntellVoiceAsrEngineContext *asrContext = (IntellVoiceAsrEngineContext *)priv;
    CHK_NULL_RETURN(asrContext, nullptr, "ttsContext is NULL!");
    uint32_t len = 0;
    CapturerBuffer capBuf = {};
    int32_t readLen = 0;
    bool cachedBuf = false;
    if (IntellVoiceAsrCreateStream(asrContext, &capBuf) != MEDIA_OK) {
        MEDIA_ERR_LOG("IntellVoiceAsrCreateStream failed!");
        IntellVoiceAsrCallBack(asrContext, ASR_ENGINE_EVENT_VOICE_ERROR, NULL, 0);
        return nullptr;
    }
    while (true) {
        MediaMutexLock(asrContext->produceContext->produceMutex);
        if (!asrContext->produceContext->isThreadActive) {
            break;
        }
        MediaMutexUnLock(asrContext->produceContext->produceMutex);
        asrContext->produceContext->produceSleepTime = 0;
        if (!cachedBuf) {
            readLen = asrContext->asiHandle->ObtainBuffer(capBuf, true);
            if ((uint32_t)readLen == AUDIO_RETRY_READ) {
                asrContext->produceContext->produceSleepTime = DEFAULT_SLEEP_TIME_10_MS;
                goto CONTINUE;
            }
            if (readLen < 0) {
                IntellVoiceAsrCallBack(asrContext, ASR_ENGINE_EVENT_VOICE_ERROR, NULL, 0);
                break;
            }
            cachedBuf = true;
        }
        if (CircleBufferQueryFree(&asrContext->pcmBufferInfo.circleBuffer) < (uint32_t)readLen) {
            asrContext->produceContext->produceSleepTime = DEFAULT_SLEEP_TIME_10_MS;
            goto CONTINUE;
        }
        len = CircleBufferWrite(&asrContext->pcmBufferInfo.circleBuffer, (const uint8_t *)capBuf.data, (uint32_t)readLen);
        if (len != (uint32_t)readLen) {
            MEDIA_ERR_LOG("CircleBufferWrite failed! len = %u, tileSize = %d", len, readLen);
            IntellVoiceAsrCallBack(asrContext, ASR_ENGINE_EVENT_VOICE_ERROR, NULL, 0);
            break;
        }
        cachedBuf = false;
CONTINUE:
        IntellVoiceThreadSleep(asrContext);
    }
    MediaMutexUnLock(asrContext->produceContext->produceMutex);
    IntellVoiceAsrDestroyStream(asrContext, &capBuf);
    MEDIA_INFO_LOG("IntellVoiceProduceAudioData exit!");
    return nullptr;
}

static void IntellVoiceAsrDestoryProduceThread(IntellVoiceAsrEngineContext *asrContext)
{
    if (asrContext == nullptr ||
        asrContext->produceContext->produceTaskHandle == nullptr) {
        return;
    }
    MediaMutexLock(asrContext->produceContext->produceMutex);
    asrContext->produceContext->isThreadActive = false;
    MediaMutexUnLock(asrContext->produceContext->produceMutex);
    MediaThreadCondSignal(asrContext->produceContext->produceCond);
    MediaThreadJoin(&asrContext->produceContext->produceTaskHandle);
    asrContext->produceContext->produceTaskHandle = nullptr;
}

static void IntellVoiceAsrDestoryNpuThread(IntellVoiceAsrEngineContext *asrContext)
{
    if (asrContext == nullptr ||
        asrContext->npuContext->npuTaskHandle == nullptr) {
        return;
    }
    MediaMutexLock(asrContext->npuContext->npuMutex);
    asrContext->npuContext->isThreadActive = false;
    MediaMutexUnLock(asrContext->npuContext->npuMutex);
    MediaThreadCondSignal(asrContext->npuContext->npuCond);
    MediaThreadJoin(&asrContext->npuContext->npuTaskHandle);
    asrContext->npuContext->npuTaskHandle = nullptr;
}

static int32_t IntellVoiceAsrCreateThread(IntellVoiceAsrEngineContext *asrContext)
{
    CHK_NULL_RETURN(asrContext->produceContext, MEDIA_ERR, "produceContext is NULL");
    if (asrContext->produceContext->produceTaskHandle == nullptr) {
        asrContext->produceContext->isThreadActive = true;
        MediaThreadattr attr = { "IntellVoiceCapture", 0x1000, THREAD_SCHED_INVALID, 0, false };
        asrContext->produceContext->produceTaskHandle = MediaThreadCreate(IntellVoiceProduceAudioData,
            asrContext, &attr);
        if (asrContext->produceContext->produceTaskHandle == nullptr) {
            MEDIA_ERR_LOG("create IntellVoiceCapture failed");
            goto EXIT0;
        }
    }
    CHK_NULL_RETURN(asrContext->npuContext, MEDIA_ERR, "npuContext is NULL");
    if (asrContext->npuContext->npuTaskHandle == nullptr) {
        asrContext->npuContext->isThreadActive = true;
        MediaThreadattr attr = { "IntellVoiceNpuThread", 0x1800, THREAD_SCHED_INVALID, 0, false };
        asrContext->npuContext->npuTaskHandle = MediaThreadCreate(IntellVoiceNpuThread, asrContext, &attr);
        if (asrContext->npuContext->npuTaskHandle == nullptr) {
            IntellVoiceAsrDestoryProduceThread(asrContext);
            MEDIA_ERR_LOG("create IntellVoiceAsrDestoryProduceThread failed");
            goto EXIT0;
        }
    }
    return MEDIA_OK;
EXIT0:
    return MEDIA_ERR;
}

int32_t IntellVoiceAsrEngineStartRecognition(void)
{
    MediaInitStaticMutexLock(&g_asrMutex);
    MediaMutexLock(g_asrMutex);
    if (g_asrCtx == nullptr ||
        g_asrCtx->states != IntellVoiceAsrStates::INTELL_VOICE_ASR_INIT) {
        MEDIA_ERR_LOG("initialization failed. Please initialize first.!");
        MediaMutexUnLock(g_asrMutex);
        return MEDIA_ERR;
    }
    MEDIA_CHR_LOG(MT_MEDIA_CHR_SMART_VOICE_SREVICE, MEDIA_CHR_ARS_ENGINE_START, 0, 0);
    if (g_asrCtx->enableNpu) {
        int32_t ret = CacheMemoryInfoInit(g_asrCtx);
        if (ret != MEDIA_OK) {
            MEDIA_ERR_LOG("CacheMemoryInfoInit failed!");
            MediaMutexUnLock(g_asrMutex);
            return MEDIA_ERR;
        }
        ret = IntellVoiceAsrCreateThread(g_asrCtx);
        if (ret != MEDIA_OK) {
            MEDIA_ERR_LOG("IntellVoiceAsrCreateThread failed.!");
            CacheMemoryInfoDeInit(g_asrCtx);
            MediaMutexUnLock(g_asrMutex);
            return MEDIA_ERR;
        }
        MEDIA_CHR_LOG(MT_MEDIA_CHR_SMART_VOICE_SREVICE, MEDIA_CHR_ARS_ENGINE_START,
            MEDIA_CHR_ARS_ENGINE_CREATE_NPU_THREAD, 0);
    } else {
        MEDIA_CHR_LOG(MT_MEDIA_CHR_SMART_VOICE_SREVICE, MEDIA_CHR_ARS_ENGINE_INIT,
            MEDIA_CHR_ARS_ENGINE_FIXED_INIT, 0);
        int32_t ret = IntellVoiceAsrInit(g_asrCtx);
        if (ret != MEDIA_OK) {
            MEDIA_ERR_LOG("intell voice asr init fail.");
            MediaMutexUnLock(g_asrMutex);
            return MEDIA_ERR;
        }
        MEDIA_CHR_LOG(MT_MEDIA_CHR_SMART_VOICE_SREVICE, MEDIA_CHR_ARS_ENGINE_START, MEDIA_CHR_ARS_INIT_FIXED_STREAM, 0);
        ret = IntellVoiceAsrStart(g_asrCtx);
        if (ret != MEDIA_OK) {
            MEDIA_ERR_LOG("intell voice asr start fail.");
            MediaMutexUnLock(g_asrMutex);
            return MEDIA_ERR;
        }
        MEDIA_CHR_LOG(MT_MEDIA_CHR_SMART_VOICE_SREVICE, MEDIA_CHR_ARS_ENGINE_START, MEDIA_CHR_ARS_START_FIXED_STREAM, 0);
    }
    g_asrCtx->states = IntellVoiceAsrStates::INTELL_VOICE_ASR_START;
    MediaMutexUnLock(g_asrMutex);
    return MEDIA_OK;
}

int32_t IntellVoiceAsrEngineStopRecognition(void)
{
    MediaInitStaticMutexLock(&g_asrMutex);
    MediaMutexLock(g_asrMutex);
    MEDIA_CHR_LOG(MT_MEDIA_CHR_SMART_VOICE_SREVICE, MEDIA_CHR_ARS_ENGINE_STOP, MEDIA_CHR_ARS_ENGINE_STOP_START, 0);
    if (g_asrCtx == nullptr ||
        g_asrCtx->states != IntellVoiceAsrStates::INTELL_VOICE_ASR_START) {
        MEDIA_ERR_LOG("No recognition has started, so there is no need to stop.");
        MediaMutexUnLock(g_asrMutex);
        return MEDIA_ERR;
    }
    if (g_asrCtx->enableNpu) {
        IntellVoiceAsrDestoryProduceThread(g_asrCtx);
        IntellVoiceAsrDestoryNpuThread(g_asrCtx);
        CacheMemoryInfoDeInit(g_asrCtx);
    } else {
        IntellVoiceAsrDestory(g_asrCtx);
    }
    g_asrCtx->states = IntellVoiceAsrStates::INTELL_VOICE_ASR_STOP;
    MEDIA_CHR_LOG(MT_MEDIA_CHR_SMART_VOICE_SREVICE, MEDIA_CHR_ARS_ENGINE_STOP, MEDIA_CHR_ARS_ENGINE_STOP_END, 0);
    MediaMutexUnLock(g_asrMutex);
    return MEDIA_OK;
}

int32_t IntellVoiceAsrEngineDeInit(void)
{
    MediaInitStaticMutexLock(&g_asrMutex);
    MediaMutexLock(g_asrMutex);
    MEDIA_CHR_LOG(MT_MEDIA_CHR_SMART_VOICE_SREVICE, MEDIA_CHR_ARS_ENGINE_DEINIT, 0, 0);
    if (g_asrCtx == nullptr) {
        MEDIA_ERR_LOG("The g_asrCtx have been destroyed!");
        MediaMutexUnLock(g_asrMutex);
        return MEDIA_ERR;
    }
    if (g_asrCtx->states != IntellVoiceAsrStates::INTELL_VOICE_ASR_STOP &&
        g_asrCtx->states != IntellVoiceAsrStates::INTELL_VOICE_ASR_IDLE) {
        MediaMutexUnLock(g_asrMutex);
        IntellVoiceAsrEngineStopRecognition();
        MediaMutexLock(g_asrMutex);
    }
    DeInitProduceContext(g_asrCtx);
    DeInitNpuContext(g_asrCtx);
    (void)g_amInstance.DeactivateAudioInterrupt(g_asrCtx->interrupt);
    if (g_asrCtx->audioCtx != NULL) {
        MediaMemFree(g_asrCtx->audioCtx);
        g_asrCtx->audioCtx = NULL;
        MEDIA_CHR_LOG(MT_MEDIA_CHR_SMART_VOICE_SREVICE, MEDIA_CHR_ARS_ENGINE_DEINIT, MEDIA_CHR_ARS_NPU_DEINIT, 0);
    }
    
    MediaMemFree(g_asrCtx);
    g_asrCtx = nullptr;
    MEDIA_CHR_LOG(MT_MEDIA_CHR_SMART_VOICE_SREVICE, MEDIA_CHR_ARS_ENGINE_DEINIT, MEDIA_CHR_ARS_ALL_DEINIT, 0);
    MediaMutexUnLock(g_asrMutex);
    return MEDIA_OK;
}


static void InitAudioStreamConfig(AudioRendererConfig &asoConfig, CapturerInputConfig &asiConfig)
{
    asoConfig.audioFormat = PCM;
    asoConfig.streamType = g_pickupCtx.interrupt.streamType;
    asoConfig.sampleRate = DEFAULT_SAMPLE_RATE;
    asoConfig.sessionID = g_pickupCtx.interrupt.sessionID;
    asoConfig.channelCount = 1;
    asoConfig.linkDir = AUDIO_DOWN_LINK;
    asoConfig.bitWidth = BIT_WIDTH_16;
    asiConfig.audioFormat = PCM;
    asiConfig.streamType = g_pickupCtx.interrupt.streamType;
    asiConfig.sampleRate = DEFAULT_SAMPLE_RATE;
    asiConfig.sessionID = g_pickupCtx.interrupt.sessionID;
    asiConfig.channelCount = 1;
    asiConfig.linkDir = AUDIO_UP_LINK;
    asiConfig.bitWidth = BIT_WIDTH_16;
}


static int32_t PickUpActivateInterrupt()
{
    std::shared_ptr<PickupStreamInterruptListener> listener = std::make_shared<PickupStreamInterruptListener>();
    AudioSession sessionId = g_amInstance.MakeSessionId();
    if (sessionId == AUDIO_SESSION_ID_NONE) {
        MEDIA_ERR_LOG("session is none");
        return MEDIA_ERR;
    }
    MEDIA_INFO_LOG("sessionId:%d", sessionId);
    g_pickupCtx.interrupt = { AUDIO_STREAM_VOICE_PICKUP, sessionId, listener };
    if (g_amInstance.ActivateAudioInterrupt(g_pickupCtx.interrupt) == INTERRUPT_FAILED) {
        MEDIA_ERR_LOG("ActivateAudioInterrupt failed");
        return MEDIA_ERR;
    }
    return MEDIA_OK;
}

static int32_t PickUpStreamInit()
{
    AudioRendererConfig asoConfig = {};
    CapturerInputConfig asiConfig = {};
    InitAudioStreamConfig(asoConfig, asiConfig);
    if (g_pickupCtx.asoHandle->Init(asoConfig) != MEDIA_OK) {
        MEDIA_ERR_LOG("init failed!");
        return MEDIA_ERR;
    }

    if (g_pickupCtx.asiHandle->Init(asiConfig) != MEDIA_OK) {
        MEDIA_ERR_LOG("init failed!");
        return MEDIA_ERR;
    }
    return MEDIA_OK;
}

static int32_t PickUpStreamStart()
{
    if (g_pickupCtx.asoHandle->Play() != MEDIA_OK) {
        MEDIA_ERR_LOG("AudioStreamOut Play failed!");
        return MEDIA_ERR;
    }
    if (g_pickupCtx.asiHandle->Start() != MEDIA_OK) {
        MEDIA_ERR_LOG("AudioStreamIn Start failed!");
        return MEDIA_ERR;
    }
    uint32_t channelId;
    if (g_pickupCtx.asoHandle->GetCurrentChannelId(channelId) != MEDIA_OK) {
        MEDIA_ERR_LOG("AudioStreamOut GetCurrentChannelId fail.");
        return MEDIA_ERR;
    }
    if (g_pickupCtx.asiHandle->AttachBackend(channelId) != MEDIA_OK) {
        MEDIA_ERR_LOG("AudioStreamIn AttachBackend fail.");
        return MEDIA_ERR;
    }
    std::string startCmd("startpickup = ");
    if (g_pickupCtx.asoHandle->SetParam(startCmd.c_str(), startCmd.size()) != MEDIA_OK) {
        MEDIA_ERR_LOG("AudioStreamOut SetParam fail.");
        return MEDIA_ERR;
    }
    return MEDIA_OK;
}

int32_t IntellVoiceAsrPickUpStreamStart()
{
    MediaInitStaticMutexLock(&g_pickupMutex);
    MediaMutexLock(g_pickupMutex);
    if (g_pickupCtx.isStart) {
        MEDIA_ERR_LOG("has already started!");
        MediaMutexUnLock(g_pickupMutex);
        return MEDIA_ERR;
    }

    if (PickUpActivateInterrupt() != MEDIA_OK) {
        MediaMutexUnLock(g_pickupMutex);
        return MEDIA_ERR;
    }

    g_pickupCtx.asoHandle = std::make_shared<AudioStreamOut>();
    g_pickupCtx.asiHandle = std::make_shared<AudioStreamIn>();
    if (g_pickupCtx.asoHandle.get() == nullptr || g_pickupCtx.asiHandle.get() == nullptr) {
        MEDIA_ERR_LOG("init AsoHandle failed");
        goto DEACTIVATE;
    }
    if (PickUpStreamInit() != MEDIA_OK) {
        goto RELEASE;
    }
    if (PickUpStreamStart() != MEDIA_OK) {
        goto STOP;
    }
    
    g_pickupCtx.isStart = true;
    MediaMutexUnLock(g_pickupMutex);
    return MEDIA_OK;
STOP:
    (void)g_pickupCtx.asoHandle->Stop();
    (void)g_pickupCtx.asiHandle->Stop();
RELEASE:
    (void)g_pickupCtx.asoHandle->Release();
    (void)g_pickupCtx.asiHandle->Release();
DEACTIVATE:
    g_pickupCtx.asoHandle.reset();
    g_pickupCtx.asiHandle.reset();
    g_pickupCtx.isStart = false;
    g_amInstance.DeactivateAudioInterrupt(g_pickupCtx.interrupt);
    MediaMutexUnLock(g_pickupMutex);
    return MEDIA_ERR;
}

int32_t IntellVoiceAsrPickUpStreamStop()
{
    MediaInitStaticMutexLock(&g_pickupMutex);
    MediaMutexLock(g_pickupMutex);
    int32_t ret = MEDIA_OK;
    if (!g_pickupCtx.isStart) {
        MEDIA_ERR_LOG("has already stoped!");
        MediaMutexUnLock(g_pickupMutex);
        return MEDIA_ERR;
    }
    
    ret = g_pickupCtx.asoHandle->Stop();
    CHK_FAILED_NO_RETURN(ret, MEDIA_OK, "AudioStreamOut Stop failed!");
    ret = g_pickupCtx.asoHandle->Release();
    CHK_FAILED_NO_RETURN(ret, MEDIA_OK, "AudioStreamOut Release failed!");
    ret = g_pickupCtx.asiHandle->Stop();
    CHK_FAILED_NO_RETURN(ret, MEDIA_OK, "AudioStreamIn Stop failed!");
    ret = g_pickupCtx.asiHandle->Release();
    CHK_FAILED_NO_RETURN(ret, MEDIA_OK, "AudioStreamIn Release failed!");
    g_pickupCtx.asoHandle.reset();
    g_pickupCtx.asiHandle.reset();

    g_amInstance.DeactivateAudioInterrupt(g_pickupCtx.interrupt);
    g_pickupCtx.isStart = false;
    MediaMutexUnLock(g_pickupMutex);
    return MEDIA_OK;
}

#ifdef __cplusplus
}
#endif