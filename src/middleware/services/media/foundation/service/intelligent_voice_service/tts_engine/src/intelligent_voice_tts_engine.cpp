/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: intelligent voice service tts engine.
 */

#include "intelligent_voice_tts_engine.h"
#include <string.h>
#include <unistd.h>
#include <cstdlib>
#include <stdbool.h>
#include <vector>
#include <mutex>
#include <shared_mutex>
#include "media_log.h"
#include "soc_uapi_tts_frontend.h"
#include "media_thread_adapt.h"
#include "securec.h"
#include "audio_manager.h"
#include "audio_stream_out.h"
#include "am_model.h"
#include "vocoder_model.h"
#include "media_mem.h"
#include "npu_drv_if.h"
#include "circle_buffer.h"

using Audio::AudioManager;
using Audio::AudioStreamOut;
using Audio::InterruptListener;
using Audio::AudioInterrupt;

enum class IntellVoiceTtsStates : int32_t {
    INTELL_VOICE_TTS_ERR = 0,
    INTELL_VOICE_TTS_IDLE = 1 << 0,
    INTELL_VOICE_TTS_INIT = 1 << 1,
    INTELL_VOICE_TTS_START = 1 << 2,
    INTELL_VOICE_TTS_STOP = 1 << 3,
};

const int32_t MAX_TTS_INPUT_GBK_ENCODE_SIZE = 100;
const int32_t MAX_AM_OUT_REPEATITION_SIZE = 2;
const int32_t MAX_AM_OUT_MEL_SIZE = 80000;
const int32_t MAX_VOCODER_INPUT_SIZE = 9600;
const int32_t MAX_VOCODER_OUTPUT_SIZE = 24000;
const int32_t MAX_VOCODER_OUTPUT_TILE_SIZE = 30000;
const uint32_t MAX_PCM_BUFFER_SIZE = 25 * 1024;
const uint32_t ONE_SECOND_SAMPLE_RATE = 1000;
const uint32_t ONE_FRAME_TIME = 10;
const uint32_t DEFAULT_SAMPLE_RATE = 8000;
const uint32_t MAX_TEXT_ONE_BUF_SIZE = 1024;
const uint32_t MAX_TEXT_BUF_SIZE = 5 * MAX_TEXT_ONE_BUF_SIZE;
const uint32_t MAX_SHARE_BUFFER_OFFSET = 8;
const uint32_t THREAD_SLEEP_10MS = 10000;
const uint32_t THREAD_SLEEP_10S = 10000000;
const uint32_t DEFAULT_CHINESE_MAX_NUM = 50;
const int32_t AM_OUT_LEN = 500;
const int32_t AM_OUT_FEATURE_LEN = 80;
const int32_t VOCODER_IN_LEN = 60;
const int32_t VOCODER_IN_OVERLAP_LEN = 30;
const int32_t VOCODER_OUT_OVERLAP_LEN = 6000;
const int32_t VOCODER_OUT_REMAIN_LEN = 9000;
const int32_t VOCODER_OUT_DISCARD_LEN = 3000;
const int32_t AM2VOCODER_FACTOR = 200;
const int32_t MAX_OVERLAP_TIMES = 17;
const int32_t FLOAT16_BYTE = 2;
const uint32_t BUFFER_SIZE = 1050;
const uint32_t TEXT_ID_SIZE = 20;
const uint32_t ALL_PCM_SIZE_COEFF = 200;
const uint32_t MAX_VOCODER_OUTPUT_LENGHT = 12000;
const int16_t CROP_LEN = 800;

const char *VOCODER_MODEL_NAME = "/user/ai/ops/vocoder/vocoder.exeom";
const char *AM_MODEL_NAME = "/user/ai/ops/am/am.exeom";

typedef struct {
    uint8_t repeatition[MAX_AM_OUT_REPEATITION_SIZE];
    uint8_t mel[MAX_AM_OUT_MEL_SIZE];
    int16_t validLength;
    int16_t tileNum;
    uint8_t *vocoderInput;
    void *vocoderInputTemp;
    void *vocoderOutput;
    uint16_t *tileBuf;
    MediaMutexHandle produceMutex;
    MediaThreadCondHandle produceCond;
    MediaThreadIdHandle produceTaskHandle;
    bool isThreadActive;
    uint32_t produceSleepTime;
    int64_t produceDataSize;
} IntellVoiceTtsProduceContext;

typedef struct {
    AudioRendererConfig ttsOutputConfig;
    MediaMutexHandle playMutex;
    MediaThreadCondHandle playCond;
    MediaThreadIdHandle playTaskHandle;
    uint8_t *frameBuffer;
    uint32_t frameSize;
    bool isThreadActive;
    uint32_t playSleepTime;
    int64_t playDataSize;
} IntellVoiceTtsPlayContext;

typedef struct {
    uint32_t phonemeData[AUDIO_PIN_YIN_MAX_OUT_SIZE];
    uint16_t phonemeMask[AUDIO_PIN_YIN_MAX_OUT_SIZE];
} TtsPhoneme;

typedef struct {
    int32_t textId;
    uint32_t totalDataSize;
} TextAllDataSize;

typedef struct IntellVoiceTtsEngineContext {
    AudioSession sessionId;
    AudioInterrupt interrupt;
    std::shared_ptr<AudioStreamOut> asoHandle;
    bool isPause;
    bool isStopped;
    bool isResumed;
    TtsEngineListener listener;
    IntellVoiceTtsStates states;
    IntellVoiceTtsPlayContext *playContext;
    IntellVoiceTtsProduceContext *produceContext;
    CacheMemoryInfo textDataInfo;
    CacheMemoryInfo pcmBufferInfo;
    int32_t textId = -1;
    TtsPhoneme phoneme;
    std::vector<TextAllDataSize> pcmDataSize;
    MediaMutexHandle mutex;
#ifdef SUPPORT_DUMP_DATA
    FILE *pfdFront = nullptr;
    FILE *pfdRear = nullptr;
#endif
} IntellVoiceTtsEngineContext;

static AudioManager &g_amInstance = AudioManager::GetInstance();
static IntellVoiceTtsEngineContext *g_ttsCtx = nullptr;
static MediaMutexHandle g_ttsMutex = nullptr;

#define CHK_NULL_RETURN(ptr, ret, printfString) \
do { \
    if ((ptr) == nullptr) { \
        MEDIA_ERR_LOG("%s :ret:%d", ((printfString) == nullptr) ? "" : (printfString), ret); \
        return (ret); \
    } \
} while (0)
#define CHK_FAILED_RETURN(value, target, ret, print) \
do { \
    if ((value) != (target)) { \
        MEDIA_ERR_LOG("%s, %#x != %d ", ((print) == nullptr) ? "" : (print), (value), (target)); \
        return (ret); \
    } \
} while (0)

static void IncreaseDataSizeByTextId(IntellVoiceTtsEngineContext *ttsContext, uint32_t size)
{
    MediaMutexLock(ttsContext->mutex);
    bool found = false;
    for (auto &item : ttsContext->pcmDataSize) {
        if (item.textId == ttsContext->textId) {
            item.totalDataSize += size;
            found = true;
            break;
        }
    }
    if (!found) {
        TextAllDataSize dataSize = { ttsContext->textId, size };
        ttsContext->pcmDataSize.push_back(dataSize);
    }
    MediaMutexUnLock(ttsContext->mutex);
}

static int32_t DecreaseDataSizeByTextId(IntellVoiceTtsEngineContext *ttsContext, uint32_t size, int32_t *textId)
{
    MediaMutexLock(ttsContext->mutex);
    if (ttsContext->pcmDataSize.empty()) {
        MediaMutexUnLock(ttsContext->mutex);
        return MEDIA_ERR;
    }
    // Get the first element
    TextAllDataSize dataSize = ttsContext->pcmDataSize.front();
    if (dataSize.totalDataSize >= size) {
        dataSize.totalDataSize -= size;
        ttsContext->pcmDataSize.front() = dataSize;
    } else {
        // Remove the first element
        (*textId) = dataSize.textId;
        ttsContext->pcmDataSize.erase(ttsContext->pcmDataSize.begin());
        MediaMutexUnLock(ttsContext->mutex);
        return MEDIA_EMPTY;
    }
    MediaMutexUnLock(ttsContext->mutex);
    return MEDIA_OK;
}

class IntellVoiceTtsEnginInterruptListener : public InterruptListener {
public:
    explicit IntellVoiceTtsEnginInterruptListener(IntellVoiceTtsEngineContext *context)
    {
        ttsContext = context;
    };
    ~IntellVoiceTtsEnginInterruptListener() override {};

    void OnInterrupt(int32_t type, int32_t hint) override
    {
        MEDIA_INFO_LOG("type:%d hint:%d", type, hint);
        if (ttsContext == nullptr) {
            MEDIA_ERR_LOG("context is nullptr");
            return;
        }
        if (type == INTERRUPT_TYPE_BEGIN && hint == INTERRUPT_HINT_PAUSE) {
            MEDIA_INFO_LOG("ASR INTERRUPT_HINT_PAUSE");
            ttsContext->isPause = true;
        } else if (type == INTERRUPT_TYPE_END && hint == INTERRUPT_HINT_RESUME) {
            MEDIA_INFO_LOG("ASR INTERRUPT_HINT_RESUME");
            ttsContext->isResumed = true;
        } else if (type == INTERRUPT_TYPE_BEGIN && hint == INTERRUPT_HINT_STOP) {
            MEDIA_INFO_LOG("ASR INTERRUPT_HINT_STOP");
            ttsContext->isStopped = true;
        }
    }
private:
    IntellVoiceTtsEngineContext *ttsContext = nullptr;
};

static void CacheMemoryInfoDeInit(IntellVoiceTtsEngineContext *ttsContext)
{
    if (ttsContext->pcmBufferInfo.buf != nullptr) {
        MediaMemFree(ttsContext->pcmBufferInfo.buf);
        ttsContext->pcmBufferInfo.buf = nullptr;
    }
    if (ttsContext->pcmBufferInfo.circleBuffer.lock != nullptr) {
        MediaMutexDestroy(&ttsContext->pcmBufferInfo.circleBuffer.lock);
    }

    if (ttsContext->textDataInfo.buf != nullptr) {
        MediaMemFree(ttsContext->textDataInfo.buf);
        ttsContext->textDataInfo.buf = nullptr;
    }
    if (ttsContext->textDataInfo.circleBuffer.lock != nullptr) {
        MediaMutexDestroy(&ttsContext->textDataInfo.circleBuffer.lock);
    }
}

static int32_t CacheMemoryInfoInit(IntellVoiceTtsEngineContext *ttsContext)
{
    ttsContext->pcmBufferInfo.buf = (uint8_t *)MediaMemCalloc(MAX_PCM_BUFFER_SIZE + MAX_SHARE_BUFFER_OFFSET);
    if (ttsContext->pcmBufferInfo.buf == nullptr) {
        MEDIA_ERR_LOG("pcm buf malloc failed!");
        return MEDIA_ERR;
    }
    ttsContext->pcmBufferInfo.read = 0;
    ttsContext->pcmBufferInfo.write = 0;
    ttsContext->pcmBufferInfo.size = MAX_PCM_BUFFER_SIZE + MAX_SHARE_BUFFER_OFFSET;
    CircleBufferinit(&ttsContext->pcmBufferInfo.circleBuffer, &ttsContext->pcmBufferInfo.write,
        &ttsContext->pcmBufferInfo.read, ttsContext->pcmBufferInfo.buf,
        MAX_PCM_BUFFER_SIZE + MAX_SHARE_BUFFER_OFFSET);
    ttsContext->pcmBufferInfo.circleBuffer.lock = MediaMutexCreate(nullptr);
    if (ttsContext->pcmBufferInfo.circleBuffer.lock == nullptr) {
        MEDIA_ERR_LOG("pcmBufferInfo lock create failed!");
        goto EXIT;
    }
    ttsContext->textDataInfo.buf = (uint8_t *)MediaMemCalloc(MAX_PCM_BUFFER_SIZE + MAX_SHARE_BUFFER_OFFSET);
    if (ttsContext->textDataInfo.buf == nullptr) {
        MEDIA_ERR_LOG("text buf malloc failed!");
        goto EXIT;
    }
    ttsContext->textDataInfo.read = 0;
    ttsContext->textDataInfo.write = 0;
    ttsContext->textDataInfo.size = MAX_TEXT_BUF_SIZE + MAX_SHARE_BUFFER_OFFSET;
    CircleBufferinit(&ttsContext->textDataInfo.circleBuffer, &ttsContext->textDataInfo.write,
        &ttsContext->textDataInfo.read, ttsContext->textDataInfo.buf,
        MAX_TEXT_BUF_SIZE + MAX_SHARE_BUFFER_OFFSET);
    ttsContext->textDataInfo.circleBuffer.lock = MediaMutexCreate(nullptr);
    if (ttsContext->textDataInfo.circleBuffer.lock == nullptr) {
        MEDIA_ERR_LOG("textDataInfo lock create failed!");
        goto EXIT;
    }
    return MEDIA_OK;
EXIT:
    CacheMemoryInfoDeInit(ttsContext);
    return MEDIA_ERR;
}

static int32_t TtsTextToPhoneme(char *buff, uint32_t bufferSize, TtsPhoneme *phoneme)
{
    MEDIA_UNUSED(bufferSize);
    int32_t ret = MEDIA_OK;
    pin_yin_stru *pinYinHandle = nullptr;
    uint8_t *outBuff;
    uint32_t outBuffSize = (AUDIO_CHS_MAX_OUT_NUM * (sizeof(uint16_t) + sizeof(uint8_t))) + 1;

    outBuff = (uint8_t *)MediaMemCalloc(outBuffSize);
    if (outBuff == nullptr) {
        MEDIA_ERR_LOG("outBuff calloc failed!");
        return MEDIA_ERR;
    }
    ret = audio_pinyin_convert_init((void **)&pinYinHandle, outBuff, outBuffSize);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("audio_pinyin_convert_init failed.ret = %x", ret);
        goto EXIT0;
    }
    ret = audio_pinyin_convert_process(pinYinHandle, buff, strlen(buff));
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("audio_pinyin_convert_process failed.ret = %x", ret);
        goto EXIT1;
    }

    for (int32_t i = 0; i < AUDIO_PIN_YIN_MAX_OUT_SIZE; i++) {
        phoneme->phonemeData[i] = pinYinHandle->out_buffer[i];
        phoneme->phonemeMask[i] = (phoneme->phonemeData[i] == 0);
    }

EXIT1:
    audio_pinyin_convert_deinit(pinYinHandle);
EXIT0:
    MediaMemFree(outBuff);
    return ret;
}

static uint32_t IntellVoiceGetFrameSize(const AudioRendererConfig &ttsOutputConfig)
{
    uint32_t byteNum = ttsOutputConfig.bitWidth / 0x8;
    uint32_t len = ttsOutputConfig.channelCount *
        (ttsOutputConfig.sampleRate / ONE_SECOND_SAMPLE_RATE * ONE_FRAME_TIME) * byteNum;
    return len;
}

static void DeInitAudioPlayContext(IntellVoiceTtsEngineContext *ttsContext)
{
    if (ttsContext == nullptr || ttsContext->playContext == nullptr) {
        MEDIA_ERR_LOG("ttsContext or playContext is NULL!");
        return;
    }
    if (ttsContext->playContext->frameBuffer != nullptr) {
        MediaMemFree(ttsContext->playContext->frameBuffer);
        ttsContext->playContext->frameBuffer = nullptr;
    }
    (void)MediaMutexDestroy(&ttsContext->playContext->playMutex);
    (void)MediaThreadCondDestroy(&ttsContext->playContext->playCond);
    MediaMemFree(ttsContext->playContext);
    ttsContext->playContext = nullptr;
}

static int32_t InitAudioPlayContext(IntellVoiceTtsEngineContext *ttsContext)
{
    ttsContext->playContext = (IntellVoiceTtsPlayContext *)MediaMemCalloc(sizeof(IntellVoiceTtsPlayContext));
    if (ttsContext->playContext == nullptr) {
        MEDIA_ERR_LOG("malloc playContext failed");
        return MEDIA_ERR;
    }
    ttsContext->playContext->ttsOutputConfig.audioFormat = PCM;
    ttsContext->playContext->ttsOutputConfig.streamType = AUDIO_STREAM_TTS;
    ttsContext->playContext->ttsOutputConfig.sampleRate = DEFAULT_SAMPLE_RATE;
    ttsContext->playContext->ttsOutputConfig.channelCount = 1;
    ttsContext->playContext->ttsOutputConfig.linkDir = AUDIO_DOWN_LINK;
    ttsContext->playContext->ttsOutputConfig.bitWidth = BIT_WIDTH_16;
    ttsContext->playContext->frameSize = IntellVoiceGetFrameSize(ttsContext->playContext->ttsOutputConfig);
    ttsContext->playContext->frameBuffer = (uint8_t *)MediaMemCalloc(ttsContext->playContext->frameSize);
    if (ttsContext->playContext->frameBuffer == nullptr) {
        MEDIA_ERR_LOG("malloc frameBuffer failed");
        goto EXIT;
    }
    ttsContext->playContext->playMutex = MediaMutexCreate(nullptr);
    if (ttsContext->playContext->playMutex == nullptr) {
        MEDIA_ERR_LOG("create play mutex failed");
        goto EXIT;
    }
    ttsContext->playContext->playCond = MediaThreadCondCreate();
    if (ttsContext->playContext->playCond == nullptr) {
        MEDIA_ERR_LOG("create play cond failed");
        goto EXIT;
    }
    ttsContext->playContext->playTaskHandle = nullptr;
    return MEDIA_OK;
EXIT:
    DeInitAudioPlayContext(ttsContext);
    return MEDIA_ERR;
}

static void DeInitAudioProduceContext(IntellVoiceTtsEngineContext *ttsContext)
{
    if (ttsContext == NULL) {
        MEDIA_ERR_LOG("ttsContext is NULL!");
        return;
    }
    if (ttsContext->produceContext != nullptr) {
        (void)MediaMutexDestroy(&ttsContext->produceContext->produceMutex);
        (void)MediaThreadCondDestroy(&ttsContext->produceContext->produceCond);
        MediaMemFree(ttsContext->produceContext);
        ttsContext->produceContext = nullptr;
    }
}

static int32_t InitAudioProduceContext(IntellVoiceTtsEngineContext *ttsContext)
{
    ttsContext->produceContext = (IntellVoiceTtsProduceContext *)MediaMemCalloc(sizeof(IntellVoiceTtsProduceContext));
    if (ttsContext->produceContext == nullptr) {
        MEDIA_ERR_LOG("malloc produceContext failed!");
        return MEDIA_ERR;
    }
    ttsContext->produceContext->validLength = 0;
    ttsContext->produceContext->tileNum = 0;
    ttsContext->produceContext->vocoderInput = nullptr;
    ttsContext->produceContext->vocoderInputTemp = nullptr;
    ttsContext->produceContext->vocoderOutput = nullptr;
    ttsContext->produceContext->tileBuf = nullptr;
    ttsContext->produceContext->produceMutex = MediaMutexCreate(nullptr);
    if (ttsContext->produceContext->produceMutex == nullptr) {
        MEDIA_ERR_LOG("create produce mutex failed");
        goto EXIT;
    }
    ttsContext->produceContext->produceCond = MediaThreadCondCreate();
    if (ttsContext->produceContext->produceCond == nullptr) {
        MEDIA_ERR_LOG("create produce cond failed!");
        goto EXIT;
    }
    ttsContext->produceContext->produceTaskHandle = nullptr;
    return MEDIA_OK;
EXIT:
    DeInitAudioProduceContext(ttsContext);
    return MEDIA_ERR;
}

#ifdef SUPPORT_DUMP_DATA
static void IntellVoiceTtsContextOpenDumpFile(IntellVoiceTtsEngineContext *ttsContext)
{
    if (ttsContext->pfdFront == nullptr) {
        ttsContext->pfdFront = fopen("/user/tts_front.pcm", "w");
        if (ttsContext->pfdFront == nullptr) {
            MEDIA_ERR_LOG("fopen front dump file failed!");
        }
    }
    if (ttsContext->pfdRear == nullptr) {
        ttsContext->pfdRear = fopen("/user/tts_rear.pcm", "w");
        if (ttsContext->pfdRear == nullptr) {
            ttsContext->pfdFront = nullptr;
            MEDIA_ERR_LOG("fopen rear dump file failed!");
        }
    }
}
#endif

#ifdef SUPPORT_DUMP_DATA
static void IntellVoiceTtsContextCloseDumpFile(IntellVoiceTtsEngineContext *ttsContext)
{
    if (ttsContext->pfdFront != nullptr) {
        fclose(ttsContext->pfdFront);
        ttsContext->pfdFront = nullptr;
    }
    if (ttsContext->pfdRear != nullptr) {
        fclose(ttsContext->pfdRear);
        ttsContext->pfdRear = nullptr;
    }
}
#endif

static int32_t IntellVoiceTtsContextInit(const TtsEngineInitParams *initParams)
{
    CHK_NULL_RETURN(initParams, MEDIA_ERR, "initParams is NULL!");
    g_ttsCtx = new IntellVoiceTtsEngineContext();
    CHK_NULL_RETURN(g_ttsCtx, MEDIA_ERR, "malloc failed!");
    g_ttsCtx->sessionId = AUDIO_SESSION_ID_NONE;
    g_ttsCtx->asoHandle = nullptr;
    g_ttsCtx->isPause = false;
    g_ttsCtx->isStopped = false;
    g_ttsCtx->isResumed = false;
    g_ttsCtx->textId = -1;
    g_ttsCtx->listener = initParams->listener;
    int32_t ret = 0;
    g_ttsCtx->mutex = MediaMutexCreate(nullptr);
    if (g_ttsCtx->mutex == nullptr) {
        MEDIA_ERR_LOG("create tts mutex failed");
        goto EXIT1;
    }
    ret = InitAudioPlayContext(g_ttsCtx);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("InitAudioPlayContext failed!");
        goto EXIT2;
    }
    ret = InitAudioProduceContext(g_ttsCtx);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("InitAudioProduceContext failed!");
        goto EXIT3;
    }
    g_ttsCtx->states = IntellVoiceTtsStates::INTELL_VOICE_TTS_IDLE;
    return MEDIA_OK;
EXIT3:
    DeInitAudioPlayContext(g_ttsCtx);
EXIT2:
    (void)MediaMutexDestroy(&g_ttsCtx->mutex);
EXIT1:
    delete g_ttsCtx;
    g_ttsCtx = nullptr;
    return MEDIA_ERR;
}

static int32_t IntellVoiceTtsSessionInit(IntellVoiceTtsEngineContext *ttsContext)
{
    bool success = g_amInstance.Initialize();
    if (!success) {
        MEDIA_ERR_LOG("audio manager init failed");
        return MEDIA_ERR;
    }
    AudioSession sessionID = g_amInstance.MakeSessionId();
    if (sessionID == AUDIO_SESSION_ID_NONE) {
        MEDIA_ERR_LOG("get sessionid failed!");
        return MEDIA_ERR;
    }
    ttsContext->playContext->ttsOutputConfig.sessionID = sessionID;
    std::shared_ptr<IntellVoiceTtsEnginInterruptListener> listener =
        std::make_shared<IntellVoiceTtsEnginInterruptListener>(ttsContext);
    if (listener.get() == nullptr) {
        MEDIA_ERR_LOG("listener is nullptr!");
        return MEDIA_ERR;
    }
    ttsContext->interrupt = { AUDIO_STREAM_TTS, sessionID, listener };
    int32_t strategy = g_amInstance.ActivateAudioInterrupt(ttsContext->interrupt);
    if (strategy == INTERRUPT_FAILED) {
        MEDIA_ERR_LOG("activate audio interrupt faild!");
        return MEDIA_ERR;
    }
    return MEDIA_OK;
}

int32_t IntellVoiceTtsEngineInit(TtsEngineInitParams *initParams)
{
    MediaInitStaticMutexLock(&g_ttsMutex);
    MediaMutexLock(g_ttsMutex);
    if (g_ttsCtx != nullptr) {
        MEDIA_ERR_LOG("tts is exist, you need to delete first!");
        MediaMutexUnLock(g_ttsMutex);
        return MEDIA_ERR;
    }
    int32_t ret = IntellVoiceTtsContextInit(initParams);
    if (ret != MEDIA_OK) {
        MediaMutexUnLock(g_ttsMutex);
        return MEDIA_ERR;
    }
    ret = IntellVoiceTtsSessionInit(g_ttsCtx);
    if (ret != MEDIA_OK) {
        IntellVoiceTtsEngineDeInit();
        MEDIA_ERR_LOG("session init fail!");
        MediaMutexUnLock(g_ttsMutex);
        return MEDIA_ERR;
    }
    uapi_npu_dlog_setlevel(0, 0x3, 1);
    ret = initModelEnv();
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("initModelEnv failed!");
        MediaMutexUnLock(g_ttsMutex);
        return MEDIA_ERR;
    }
    g_ttsCtx->states = IntellVoiceTtsStates::INTELL_VOICE_TTS_INIT;
    MediaMutexUnLock(g_ttsMutex);
    return ret;
}

static int32_t IntellVoiceTtsAudioPlayInit(IntellVoiceTtsEngineContext *ttsContext)
{
    int32_t ret = MEDIA_OK;
    ttsContext->asoHandle = std::make_shared<AudioStreamOut>();
    if (ttsContext->asoHandle.get() == nullptr) {
        MEDIA_ERR_LOG("audio stream out is nullptr!");
        goto EXIT;
    }
    ret = ttsContext->asoHandle->Init(ttsContext->playContext->ttsOutputConfig, nullptr, ttsContext);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("AudioStreamOutInit failed!");
        ttsContext->asoHandle->Release();
        goto EXIT;
    }
    return MEDIA_OK;
EXIT:
    g_amInstance.DeactivateAudioInterrupt(ttsContext->interrupt);
    return ret;
}

static void IntellVoiceTtsReleaseHandle(IntellVoiceTtsEngineContext *ttsContext)
{
    if (ttsContext->asoHandle != nullptr) {
        ttsContext->asoHandle->Release();
        ttsContext->asoHandle.reset();
        ttsContext->asoHandle = nullptr;
    }
    g_amInstance.DeactivateAudioInterrupt(ttsContext->interrupt);
}

static int32_t IntellVoiceTtsAudioPlayStart(IntellVoiceTtsEngineContext *ttsContext)
{
    if (ttsContext->asoHandle == nullptr) {
        MEDIA_ERR_LOG("aso handle has been destroyed!");
        return MEDIA_ERR;
    }
    int32_t ret = ttsContext->asoHandle->Play();
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("AudioStreamOutStart fail.");
        IntellVoiceTtsReleaseHandle(ttsContext);
        return ret;
    }
    ttsContext->asoHandle->SetVolume(1);
    return MEDIA_OK;
}

static int32_t IntellVoiceTtsAudioPlayDestroy(IntellVoiceTtsEngineContext *ttsContext)
{
    CHK_NULL_RETURN(ttsContext, MEDIA_INVALID_PARAM, "ttsContext is Null!");
    CHK_NULL_RETURN(ttsContext->asoHandle, MEDIA_INVALID_PARAM, "asoHandle is Null!");
    int32_t ret = ttsContext->asoHandle->Stop();
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("AudioStreamOutStop fail.");
    }
    ttsContext->asoHandle->Release();
    ttsContext->asoHandle.reset();
    ttsContext->asoHandle = nullptr;
    return MEDIA_OK;
}

static int32_t IntellVoiceTtsCallback(const void *caller, AudioCallbackEventType eventType, const void *info)
{
    CHK_NULL_RETURN(caller, MEDIA_INVALID_PARAM, "audioCtx is Null!");
    IntellVoiceTtsEngineContext *ttsContext = (IntellVoiceTtsEngineContext *)caller;
    CHK_NULL_RETURN(ttsContext->listener, MEDIA_INVALID_PARAM, "ttsContext listener is Null!");
    switch (eventType) {
        case AUDIO_EVENT_TTS_START:
            ttsContext->listener(TTS_ENGINE_EVENT_SPEAK_BEGIN, info, 0);
            break;
        case AUDIO_EVENT_TTS_COMPLETED:
            ttsContext->listener(TTS_ENGINE_EVENT_SPEAK_PLAY_COMPLETE, info, sizeof(int32_t));
            break;
        case AUDIO_EVENT_TTS_END:
            ttsContext->listener(TTS_ENGINE_EVENT_SPEAK_ERROR, info, 0);
            break;
        case AUDIO_EVENT_ERROR_OCCUR:
            ttsContext->listener(TTS_ENGINE_EVENT_SPEAK_ERROR, info, 0);
            break;
        default:
            ttsContext->listener(TTS_ENGINE_EVENT_SPEAK_ERROR, info, 0);
            break;
    }
    return MEDIA_OK;
}

static int32_t IntellVoiceTtsPlayAudioDataInner(IntellVoiceTtsEngineContext *ttsContext)
{
    int32_t ret = IntellVoiceTtsAudioPlayInit(ttsContext);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("intell voice audio play init fail.");
        IntellVoiceTtsCallback(ttsContext, AUDIO_EVENT_ERROR_OCCUR, nullptr);
        return ret;
    }
    ret = IntellVoiceTtsAudioPlayStart(ttsContext);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("intell voice audio play start fail.");
        IntellVoiceTtsCallback(ttsContext, AUDIO_EVENT_ERROR_OCCUR, nullptr);
        return ret;
    }
    return MEDIA_OK;
}

static int32_t IntellVoiceTtsReadAudioData(IntellVoiceTtsEngineContext *ttsContext, bool isCacheFrame)
{
    if (isCacheFrame) {
        return MEDIA_OK;
    }
    uint32_t pcmDataSize = ttsContext->playContext->frameSize;
    int32_t textId = -1;
    if (DecreaseDataSizeByTextId(ttsContext, pcmDataSize, &textId) == MEDIA_EMPTY) {
        IntellVoiceTtsCallback(ttsContext, AUDIO_EVENT_TTS_COMPLETED, (void *)(intptr_t)textId);
        ttsContext->playContext->playSleepTime = THREAD_SLEEP_10MS;
        return MEDIA_NO_DATA;
    }
    uint32_t busySize = CircleBufferQueryBusy(&ttsContext->pcmBufferInfo.circleBuffer);
    if (busySize == 0) {
        ttsContext->playContext->playSleepTime = THREAD_SLEEP_10MS;
        return MEDIA_NO_DATA;
    }
    if (busySize < ttsContext->playContext->frameSize) {
        pcmDataSize = busySize;
    }
    uint32_t readLen = CircleBufferRead(&ttsContext->pcmBufferInfo.circleBuffer, ttsContext->playContext->frameBuffer, pcmDataSize);
    if (readLen != pcmDataSize) {
        MEDIA_ERR_LOG("CircleBufferRead data failed! readLen = %d, frameSize = %d",
            readLen, pcmDataSize);
        IntellVoiceTtsCallback(ttsContext, AUDIO_EVENT_ERROR_OCCUR, nullptr);
        return MEDIA_ERR;
    }
    return MEDIA_OK;
}

static int32_t IntellVoiceTtsWriteAudioData(IntellVoiceTtsEngineContext *ttsContext, bool *isCacheFrame)
{
    AudioRendererConfig renderConfig = {};
    uint64_t writtenSize = 0;
    int32_t ret = ttsContext->asoHandle->StreamWrite(ttsContext->playContext->frameBuffer, ttsContext->playContext->frameSize,
        writtenSize, renderConfig);
    if ((uint32_t)writtenSize != ttsContext->playContext->frameSize) {
        ttsContext->playContext->playSleepTime = THREAD_SLEEP_10MS;
        (*isCacheFrame) = true;
        return MEDIA_NO_CACHE;
    }
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("write pcm data failed!");
        IntellVoiceTtsCallback(ttsContext, AUDIO_EVENT_ERROR_OCCUR, nullptr);
        return MEDIA_ERR;
    }
    (*isCacheFrame) = false;
    return MEDIA_OK;
}

static void *IntellVoiceTtsPlayAudioData(void *priv)
{
    IntellVoiceTtsEngineContext *ttsContext = (IntellVoiceTtsEngineContext *)priv;
    CHK_NULL_RETURN(ttsContext, nullptr, "ttsContext is NULL!");
    CHK_NULL_RETURN(ttsContext->playContext, nullptr, "playContext is NULL!");
    int32_t ret = IntellVoiceTtsPlayAudioDataInner(ttsContext);
    CHK_FAILED_RETURN(ret, MEDIA_OK, nullptr, "create auido stream failed!");
    bool isCacheFrame = false;
    while (true) {
        MediaMutexLock(ttsContext->playContext->playMutex);
        if (!ttsContext->playContext->isThreadActive) {
            MediaMutexUnLock(ttsContext->playContext->playMutex);
            break;
        }
        MediaMutexUnLock(ttsContext->playContext->playMutex);
        ttsContext->playContext->playSleepTime = 0;
        ret = IntellVoiceTtsReadAudioData(ttsContext, isCacheFrame);
        if (ret != MEDIA_OK) {
            if (ret == MEDIA_NO_DATA) {
                goto CONTINUE;
            }
            break;
        }
        ret = IntellVoiceTtsWriteAudioData(ttsContext, &isCacheFrame);
        if (ret != MEDIA_OK) {
            if (ret == MEDIA_NO_CACHE) {
                goto CONTINUE;
            }
            break;
        }
#ifdef SUPPORT_DUMP_DATA
        ret = fwrite(ttsContext->playContext->frameBuffer, 1, ttsContext->playContext->frameSize, ttsContext->pfdRear);
        if ((uint32_t)ret != ttsContext->playContext->frameSize) {
            MEDIA_ERR_LOG("write pcm data failed ret = %d!", ret);
        }
#endif
CONTINUE:
        if (ttsContext->playContext->playSleepTime > 0) {
            MediaMutexLock(ttsContext->playContext->playMutex);
            MediaThreadCondTimeWait(ttsContext->playContext->playCond, ttsContext->playContext->playMutex,
                ttsContext->playContext->playSleepTime);
            MediaMutexUnLock(ttsContext->playContext->playMutex);
        }
    }
    IntellVoiceTtsAudioPlayDestroy(ttsContext);
    MEDIA_INFO_LOG("IntellVoiceTtsPlayAudioData exit!");
    return nullptr;
}

static inline int32_t IsChinese(uint8_t c)
{
    return (c & 0x80) != 0;
}

static uint32_t GetCutTextStr(uint8_t *str, char *result, uint32_t resultSize, uint32_t *index, int32_t *textId)
{
    uint32_t chineseNum = 0;
    char charId[TEXT_ID_SIZE] = {};
    int32_t textIdStart = 0;
    // Parse a string containing 50 Chinese characters and less than 1024 bytes.
    while (chineseNum < DEFAULT_CHINESE_MAX_NUM &&
        str[(*index)] &&
        (*index) < resultSize) {
        if (IsChinese(str[(*index)]) && IsChinese(str[(*index) + 1])) {
            if (strncat_s(result, resultSize, (char *)(&str[(*index)]), 0x2) != 0) {
                MEDIA_ERR_LOG("strncat_s failed!");
                return chineseNum;
            }
            chineseNum++;
            (*index) += 0x2;
            continue;
        }
        // Parse the #%d# in the string to obtain the textId.
        if (str[(*index)] == '#') {
            if (textIdStart == 0) {
                textIdStart++;
            } else {
                *textId = strtoul(charId, NULL, 0xA);
                textIdStart = 0;
                MEDIA_INFO_LOG("textId = %d!", *textId);
            }
            (*index)++;
            continue;
        }
        if (textIdStart > 0) {
            if (strncat_s(charId, TEXT_ID_SIZE, (char *)(&str[(*index)]), 0x1) != 0) {
                MEDIA_ERR_LOG("strncat_s failed!");
                return chineseNum;
            }
        } else {
            if (strncat_s(result, resultSize, (char *)(&str[(*index)]), 0x1) != 0) {
                MEDIA_ERR_LOG("strncat_s failed!");
                return chineseNum;
            }
        }
        (*index)++;
    }
    MEDIA_INFO_LOG("get chineseNum = %u!", chineseNum);
    return chineseNum;
}

static int32_t TtsAmProcess(IntellVoiceTtsEngineContext *ttsContext)
{
    int32_t ret = amModelInit(AM_MODEL_NAME);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("amModelInit failed!");
        return MEDIA_ERR;
    }
    ret = amModelExecute(ttsContext->phoneme.phonemeData, ttsContext->phoneme.phonemeMask);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("amModelExecute failed!");
        goto EXIT;
    }

    ret = amModelCopyOutput(ttsContext->produceContext->repeatition, ttsContext->produceContext->mel);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("amModelCopyOutput failed!");
    }
EXIT:
    amModelDestroy();
    return ret;
}

static int32_t TtsAmPostProcess(IntellVoiceTtsEngineContext *ttsContext)
{
    int16_t repeatition = ((int16_t *)ttsContext->produceContext->repeatition)[0];
    uint32_t allPcmDataSize = repeatition * ALL_PCM_SIZE_COEFF * FLOAT16_BYTE;
    MEDIA_INFO_LOG("all pcm data size = %d", allPcmDataSize);
    IncreaseDataSizeByTextId(ttsContext, allPcmDataSize);
    ttsContext->produceContext->tileNum = (repeatition - VOCODER_IN_LEN + VOCODER_IN_OVERLAP_LEN - 1) / VOCODER_IN_OVERLAP_LEN + 1;

    ttsContext->produceContext->validLength = (repeatition - (ttsContext->produceContext->tileNum - 1) * VOCODER_IN_OVERLAP_LEN)
        * AM2VOCODER_FACTOR;
    if (ttsContext->produceContext->tileNum == 1 && ttsContext->produceContext->validLength > CROP_LEN) {
        ttsContext->produceContext->validLength = ttsContext->produceContext->validLength - CROP_LEN;
    } else {
        if (ttsContext->produceContext->validLength <= VOCODER_OUT_DISCARD_LEN + CROP_LEN) {
            ttsContext->produceContext->tileNum = ttsContext->produceContext->tileNum - 1;
            ttsContext->produceContext->validLength = MAX_VOCODER_OUTPUT_LENGHT - CROP_LEN +
            ttsContext->produceContext->validLength - VOCODER_OUT_DISCARD_LEN;
        } else {
            ttsContext->produceContext->validLength = ttsContext->produceContext->validLength - CROP_LEN;
        }
    }

    ttsContext->produceContext->vocoderInput = (uint8_t *)MediaMemMalloc(ttsContext->produceContext->tileNum * MAX_VOCODER_INPUT_SIZE);
    if (ttsContext->produceContext->vocoderInput == nullptr) {
        MEDIA_ERR_LOG("vocoderInput malloc failed!");
        return MEDIA_ERR;
    }
    uint32_t melSliceSize = VOCODER_IN_LEN * AM_OUT_FEATURE_LEN * FLOAT16_BYTE;
    uint32_t melSliceOverlapOffset = VOCODER_IN_OVERLAP_LEN * AM_OUT_FEATURE_LEN * FLOAT16_BYTE;
    for (int32_t i = 0; i < ttsContext->produceContext->tileNum; i++) {
        int32_t ret = memcpy_s(ttsContext->produceContext->vocoderInput + i * melSliceSize, melSliceSize,
            ttsContext->produceContext->mel + i * melSliceOverlapOffset, melSliceSize);
        if (ret != EOK) {
            MEDIA_ERR_LOG("vocoderInput memcpy_s failed!");
            MediaMemFree(ttsContext->produceContext->vocoderInput);
            ttsContext->produceContext->vocoderInput = NULL;
            return MEDIA_ERR;
        }
    }
    return MEDIA_OK;
}

static void Float32Tts(float *__restrict out, const uint16_t in)
{
    uint32_t t1 = in & 0x7fffu;  // Non-sign bits
    uint32_t t2 = in & 0x8000u;  // Sign bit
    uint32_t t3 = in & 0x7c00u;  // Exponent
    t1 <<= 0xD;  // Align mantissa on MSB
    t2 <<= 0x10;  // Shift sign bit into position
    t1 += 0x38000000;         // Adjust bias
    t1 = (t3 == 0 ? 0 : t1);  // Denormals-as-zero
    t1 |= t2;                 // Re-insert sign bit
    *((unsigned int *)out) = t1;
};

static void TtsFloat16ToInt16(const void *floatSrc, uint32_t floatSize, uint16_t *intDst, uint32_t *intSize)
{
    uint32_t size = floatSize / FLOAT16_BYTE;

    for (uint32_t i = 0; i < size; i++) {
        float tmp;
        Float32Tts(&tmp, ((uint16_t *)floatSrc)[i]);
        intDst[i] = (uint16_t)(tmp * 0x7FFF);
    }
    *intSize = size * sizeof(uint16_t);
}

static int32_t TtsSendTile(IntellVoiceTtsEngineContext *ttsContext, int32_t tileIndex)
{
    const void *tileFloatSrc = nullptr;
    uint32_t tileFloatSize = 0;
    uint32_t tileSize = 0;
    if (ttsContext->produceContext->tileNum == 1) {
        tileFloatSrc = ((uint8_t *)ttsContext->produceContext->vocoderOutput);
        tileFloatSize = ttsContext->produceContext->validLength * FLOAT16_BYTE;
    } else if (tileIndex == 0) {
        tileFloatSrc = ((uint8_t *)ttsContext->produceContext->vocoderOutput);
        tileFloatSize = VOCODER_OUT_REMAIN_LEN * FLOAT16_BYTE;
    } else if (tileIndex == ttsContext->produceContext->tileNum - 1) {
        tileFloatSrc = ((uint8_t *)ttsContext->produceContext->vocoderOutput) + VOCODER_OUT_OVERLAP_LEN;
        tileFloatSize = (ttsContext->produceContext->validLength - VOCODER_OUT_DISCARD_LEN) * FLOAT16_BYTE;
    } else {
        tileFloatSrc = ((uint8_t *)ttsContext->produceContext->vocoderOutput) + VOCODER_OUT_OVERLAP_LEN;
        tileFloatSize = VOCODER_OUT_OVERLAP_LEN * FLOAT16_BYTE;
    }
    TtsFloat16ToInt16(tileFloatSrc, tileFloatSize, ttsContext->produceContext->tileBuf, &tileSize);
    while (ttsContext->produceContext->isThreadActive) {
        uint32_t freeSize = CircleBufferQueryFree(&ttsContext->pcmBufferInfo.circleBuffer);
        if (freeSize < tileSize) {
            MediaMutexLock(ttsContext->produceContext->produceMutex);
            MediaThreadCondTimeWait(ttsContext->produceContext->produceCond, ttsContext->produceContext->produceMutex,
                THREAD_SLEEP_10MS);
            MediaMutexUnLock(ttsContext->produceContext->produceMutex);
            continue;
        }
        uint32_t len = CircleBufferWrite(&ttsContext->pcmBufferInfo.circleBuffer,
            (const uint8_t *)ttsContext->produceContext->tileBuf, tileSize);
        if (len != tileSize) {
            MEDIA_ERR_LOG("CircleBufferWrite failed! len = %u, tileSize = %u", len, tileSize);
            return MEDIA_ERR;
        }
#ifdef SUPPORT_DUMP_DATA
        uint32_t ret = fwrite(ttsContext->produceContext->tileBuf, 1, tileSize, ttsContext->pfdFront);
        if (ret != tileSize) {
            MEDIA_ERR_LOG("write pcm front data failed ret = %d!", ret);
        }
#endif
        break;
    }
    return MEDIA_OK;
}

static void TtsVocoderBufDestory(IntellVoiceTtsEngineContext *ttsContext)
{
    if (ttsContext->produceContext->vocoderInput != nullptr) {
        MediaMemFree(ttsContext->produceContext->vocoderInput);
        ttsContext->produceContext->vocoderInput = nullptr;
    }

    if (ttsContext->produceContext->tileBuf != nullptr) {
        MediaMemFree(ttsContext->produceContext->tileBuf);
        ttsContext->produceContext->tileBuf = nullptr;
    }

    if (ttsContext->produceContext->vocoderOutput != nullptr) {
        MediaMemFree(ttsContext->produceContext->vocoderOutput);
        ttsContext->produceContext->vocoderOutput = nullptr;
    }

    if (ttsContext->produceContext->vocoderInputTemp != nullptr) {
        MediaMemFree(ttsContext->produceContext->vocoderInputTemp);
        ttsContext->produceContext->vocoderInputTemp = nullptr;
    }
}

static int32_t TtsVocoderBufCreate(IntellVoiceTtsEngineContext *ttsContext)
{
    int32_t ret = MEDIA_OK;
    ttsContext->produceContext->vocoderInputTemp = MediaMemCalloc(MAX_VOCODER_INPUT_SIZE);
    if (ttsContext->produceContext->vocoderInputTemp == nullptr) {
        MEDIA_ERR_LOG("malloc vocoder Input Temp failed");
        return MEDIA_ERR;
    }
    ttsContext->produceContext->vocoderOutput = MediaMemCalloc(MAX_VOCODER_OUTPUT_SIZE);
    if (ttsContext->produceContext->vocoderOutput == nullptr) {
        MEDIA_ERR_LOG("malloc vocoder output failed");
        goto EXIT;
    }
    ttsContext->produceContext->tileBuf = (uint16_t *)MediaMemCalloc(MAX_VOCODER_OUTPUT_TILE_SIZE);
    if (ttsContext->produceContext->tileBuf == nullptr) {
        MEDIA_ERR_LOG("malloc tile_buf failed");
        goto EXIT;
    }
    return MEDIA_OK;
EXIT:
    TtsVocoderBufDestory(ttsContext);
    return MEDIA_ERR;
}

static int32_t TtsVocoderProcess(IntellVoiceTtsEngineContext *ttsContext)
{
    int32_t ret = vocoderModelInit(VOCODER_MODEL_NAME);
    int32_t tileIndex = 0;
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("vocoderModelInit failed");
        return ret;
    }
    ret = TtsVocoderBufCreate(ttsContext);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("TtsVocoderBufCreate failed");
        goto EXIT;
    }

    for (; tileIndex < ttsContext->produceContext->tileNum && ttsContext->produceContext->isThreadActive; tileIndex++) {
        ret = memcpy_s(ttsContext->produceContext->vocoderInputTemp, MAX_VOCODER_INPUT_SIZE,
            ttsContext->produceContext->vocoderInput + tileIndex * MAX_VOCODER_INPUT_SIZE, MAX_VOCODER_INPUT_SIZE);
        if (ret != EOK) {
            MEDIA_ERR_LOG("memcpy_s failed!");
            break;
        }
        ret = vocoderModelExecute(ttsContext->produceContext->vocoderInputTemp);
        if (ret != MEDIA_OK) {
            MEDIA_ERR_LOG("vocoderModelExecute failed!");
            break;
        }

        ret = vocoderModelCopyOutput(ttsContext->produceContext->vocoderOutput);
        if (ret != MEDIA_OK) {
            MEDIA_ERR_LOG("vocoderModelCopyOutput failed!");
            break;
        }

        ret = TtsSendTile(ttsContext, tileIndex);
        if (ret != MEDIA_OK) {
            MEDIA_ERR_LOG("TtsSendTile failed!");
            break;
        }
    }
    TtsVocoderBufDestory(ttsContext);
EXIT:
    vocoderModelDestroy();
    return ret;
}

static int32_t StartGeneratingPcmData(IntellVoiceTtsEngineContext *ttsContext, uint8_t *str, uint32_t size)
{
    uint32_t tempIndex = 0;
    int32_t ret = MEDIA_OK;
    for (uint32_t index = 0; index < size && ttsContext->produceContext->isThreadActive;) {
        char *result = (char *)MediaMemCalloc(MAX_TEXT_ONE_BUF_SIZE + 1);
        CHK_NULL_RETURN(result, MEDIA_ERR, "result malloc failed!");
        uint32_t chineseNum = GetCutTextStr(str, result, MAX_TEXT_ONE_BUF_SIZE, &index, &ttsContext->textId);
        if (index <= tempIndex || chineseNum == 0) {
            MEDIA_INFO_LOG("No Chinese characters were retrieved!");
            MediaMemFree(result);
            break;
        }
        uint32_t charLen = index - tempIndex;
        tempIndex = index;
        MEDIA_INFO_LOG("index = %u, charLen = %u, ttsContext->textId = %d, result = %s",
            index, charLen, ttsContext->textId, result);
        ret = TtsTextToPhoneme(result, charLen, &ttsContext->phoneme);
        if (ret != MEDIA_OK) {
            MEDIA_ERR_LOG("TtsTextToPhoneme failed!");
            MediaMemFree(result);
            return MEDIA_ERR;
        }
        MediaMemFree(result);
        ret = TtsAmProcess(ttsContext);
        if (ret != MEDIA_OK) {
            MEDIA_ERR_LOG("TtsAmProcess failed!");
            return ret;
        }
        ret = TtsAmPostProcess(ttsContext);
        if (ret != MEDIA_OK) {
            MEDIA_ERR_LOG("TtsAmPostProcess failed!");
            return MEDIA_ERR;
        }
        ret = TtsVocoderProcess(ttsContext);
        if (ret != MEDIA_OK) {
            MEDIA_ERR_LOG("TtsVocoderProcess failed!");
            return MEDIA_ERR;
        }
    }
    MEDIA_INFO_LOG("StartGeneratingPcmData success size = %d!", size);
    return ret;
}

static void *IntellVoiceTtsProduceAudioData(void *priv)
{
    IntellVoiceTtsEngineContext *ttsContext = (IntellVoiceTtsEngineContext *)priv;
    CHK_NULL_RETURN(ttsContext, nullptr, "ttsContext is NULL!");
    CHK_NULL_RETURN(ttsContext->produceContext, nullptr, "produceContext is NULL!");
    uint32_t textSize = MAX_TEXT_ONE_BUF_SIZE;
    uint8_t *textStr = (uint8_t *)MediaMemCalloc(textSize);
    CHK_NULL_RETURN(textStr, nullptr, "result malloc failed!");
    uint32_t readLen = 0;
    int32_t ret = 0;
    while (true) {
        MediaMutexLock(ttsContext->produceContext->produceMutex);
        if (!ttsContext->produceContext->isThreadActive) {
            MediaMutexUnLock(ttsContext->produceContext->produceMutex);
            break;
        }
        MediaMutexUnLock(ttsContext->produceContext->produceMutex);
        ttsContext->produceContext->produceSleepTime = 0;
        uint32_t busySize = CircleBufferQueryBusy(&ttsContext->textDataInfo.circleBuffer);
        if (busySize == 0) {
            ttsContext->produceContext->produceSleepTime = THREAD_SLEEP_10MS;
            goto CONTINUE;
        }
        if (busySize < MAX_TEXT_ONE_BUF_SIZE) {
            textSize = busySize;
        }
        MEDIA_INFO_LOG("busySize = %d, textSize = %d!", busySize, textSize);
        (void)memset_s(textStr, MAX_TEXT_ONE_BUF_SIZE, 0, MAX_TEXT_ONE_BUF_SIZE);
        readLen = CircleBufferRead(&ttsContext->textDataInfo.circleBuffer, textStr, textSize);
        if (readLen != textSize) {
            MEDIA_ERR_LOG("CircleBuffer Read text data failed! readLen = %u, frameSize = %u", readLen, textSize);
            IntellVoiceTtsCallback(ttsContext, AUDIO_EVENT_ERROR_OCCUR, nullptr);
            break;
        }
        if (StartGeneratingPcmData(ttsContext, textStr, textSize) != MEDIA_OK) {
            MEDIA_ERR_LOG("StartGeneratingPcmData failed!");
            IntellVoiceTtsCallback(ttsContext, AUDIO_EVENT_ERROR_OCCUR, nullptr);
            break;
        }
    CONTINUE:
        if (ttsContext->produceContext->produceSleepTime > 0) {
            MediaMutexLock(ttsContext->produceContext->produceMutex);
            MediaThreadCondTimeWait(ttsContext->produceContext->produceCond, ttsContext->produceContext->produceMutex,
                ttsContext->produceContext->produceSleepTime);
            MediaMutexUnLock(ttsContext->produceContext->produceMutex);
        }
    }
    MediaMemFree(textStr);
    MEDIA_INFO_LOG("IntellVoiceTtsProduceAudioData exit!");
    return nullptr;
}

static void IntellVoiceTtsDestoryProduceThread(IntellVoiceTtsEngineContext *ttsContext)
{
    if (ttsContext == nullptr ||
        ttsContext->produceContext->produceTaskHandle == nullptr) {
        return;
    }
    MediaMutexLock(ttsContext->produceContext->produceMutex);
    ttsContext->produceContext->isThreadActive = false;
    MediaMutexUnLock(ttsContext->produceContext->produceMutex);
    MediaThreadCondSignal(ttsContext->produceContext->produceCond);
    MediaThreadJoin(&ttsContext->produceContext->produceTaskHandle);
    ttsContext->produceContext->produceTaskHandle = nullptr;
}

static void IntellVoiceTtsDestoryPlayThread(IntellVoiceTtsEngineContext *ttsContext)
{
    if (ttsContext == nullptr ||
        ttsContext->playContext->playTaskHandle == nullptr) {
        return;
    }
    MediaMutexLock(ttsContext->playContext->playMutex);
    ttsContext->playContext->isThreadActive = false;
    MediaMutexUnLock(ttsContext->playContext->playMutex);
    MediaThreadCondSignal(ttsContext->playContext->playCond);
    MediaThreadJoin(&ttsContext->playContext->playTaskHandle);
    ttsContext->playContext->playTaskHandle = nullptr;
}

static int32_t IntellVoiceTtsCreateThread(IntellVoiceTtsEngineContext *ttsContext)
{
#ifdef SUPPORT_DUMP_DATA
    IntellVoiceTtsContextOpenDumpFile(ttsContext);
#endif
    CHK_NULL_RETURN(g_ttsCtx->produceContext, MEDIA_ERR, "produceContext is NULL");
    if (g_ttsCtx->produceContext->produceTaskHandle == nullptr) {
        g_ttsCtx->produceContext->isThreadActive = true;
        MediaThreadattr attr = { "IntellVoiceTTSProduce", 0x1000, THREAD_SCHED_INVALID, 0, false };
        g_ttsCtx->produceContext->produceTaskHandle = MediaThreadCreate(IntellVoiceTtsProduceAudioData,
            g_ttsCtx, &attr);
        if (g_ttsCtx->produceContext->produceTaskHandle == nullptr) {
            MEDIA_ERR_LOG("create IntellVoiceTtsProduceAudioData failed");
            goto EXIT0;
        }
    }
    CHK_NULL_RETURN(g_ttsCtx->playContext, MEDIA_ERR, "playContext is NULL");
    if (g_ttsCtx->playContext->playTaskHandle == nullptr) {
        g_ttsCtx->playContext->isThreadActive = true;
        MediaThreadattr attr = { "IntellVoiceTTSPlayer", 0x1000, THREAD_SCHED_INVALID, 0, false };
        g_ttsCtx->playContext->playTaskHandle = MediaThreadCreate(IntellVoiceTtsPlayAudioData,
            g_ttsCtx, &attr);
        if (g_ttsCtx->playContext->playTaskHandle == nullptr) {
            IntellVoiceTtsDestoryProduceThread(g_ttsCtx);
            MEDIA_ERR_LOG("create IntellVoiceTtsPlayAudioData failed");
            goto EXIT0;
        }
    }
    return MEDIA_OK;
EXIT0:
#ifdef SUPPORT_DUMP_DATA
    IntellVoiceTtsContextCloseDumpFile(ttsContext);
#endif
    return MEDIA_ERR;
}

int32_t IntellVoiceTtsEngineSendData(uint32_t textId, const char *textString, uint32_t textLen)
{
    uint32_t freeSize = CircleBufferQueryFree(&g_ttsCtx->textDataInfo.circleBuffer);
    if (freeSize < textLen) {
        MEDIA_ERR_LOG("No space available to place the text!");
        return MEDIA_NO_CACHE;
    }
    char buf[BUFFER_SIZE] = {};
    int32_t ret = sprintf_s(buf, BUFFER_SIZE, "#%u#", textId);
    if (ret == -1) {
        MEDIA_ERR_LOG("sprintf_s failed!");
        return MEDIA_ERR;
    }
    if (memcpy_s(buf + ret, BUFFER_SIZE - ret, textString, textLen) != EOK) {
        MEDIA_ERR_LOG("memcpy_s failed !");
        return MEDIA_ERR;
    }
    uint32_t len = CircleBufferWrite(&g_ttsCtx->textDataInfo.circleBuffer, (const uint8_t *)buf, ret + textLen);
    if (len != static_cast<uint32_t>(ret) + textLen) {
        MEDIA_ERR_LOG("failed to cache text! textLen = %u, len = %d", ret + textLen, len);
        return MEDIA_NO_CACHE;
    }
    g_ttsCtx->states = IntellVoiceTtsStates::INTELL_VOICE_TTS_START;
    return MEDIA_OK;
}

int32_t IntellVoiceTtsEngineTextSpeak(int32_t textId, const char *textString, uint32_t textLen)
{
    MediaInitStaticMutexLock(&g_ttsMutex);
    MediaMutexLock(g_ttsMutex);
    if (g_ttsCtx == nullptr ||
        g_ttsCtx->states == IntellVoiceTtsStates::INTELL_VOICE_TTS_IDLE ||
        g_ttsCtx->states == IntellVoiceTtsStates::INTELL_VOICE_TTS_ERR) {
        MEDIA_ERR_LOG("Current status error, unable to recognize and play!");
        MediaMutexUnLock(g_ttsMutex);
        return MEDIA_ERR;
    }
    if (textLen > MAX_TEXT_ONE_BUF_SIZE) {
        MEDIA_ERR_LOG("The text length exceeds 1024 bytes and cannot be parsed!");
        MediaMutexUnLock(g_ttsMutex);
        return MEDIA_ERR;
    }
    int32_t ret = CacheMemoryInfoInit(g_ttsCtx);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("CacheMemoryInfoInit failed!");
        MediaMutexUnLock(g_ttsMutex);
        return MEDIA_ERR;
    }
    ret = IntellVoiceTtsCreateThread(g_ttsCtx);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("IntellVoiceTtsCreateThread failed.!");
        CacheMemoryInfoDeInit(g_ttsCtx);
        MediaMutexUnLock(g_ttsMutex);
        return MEDIA_ERR;
    }
    ret = IntellVoiceTtsEngineSendData(textId, textString, textLen);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("IntellVoiceTtsEngineSendData failed.!");
        IntellVoiceTtsDestoryProduceThread(g_ttsCtx);
        IntellVoiceTtsDestoryPlayThread(g_ttsCtx);
        CacheMemoryInfoDeInit(g_ttsCtx);
    }
    MediaMutexUnLock(g_ttsMutex);
    return ret;
}

int32_t IntellVoiceTtsEngineStop(void)
{
    MediaInitStaticMutexLock(&g_ttsMutex);
    MediaMutexLock(g_ttsMutex);
    if (g_ttsCtx == nullptr ||
        g_ttsCtx->states != IntellVoiceTtsStates::INTELL_VOICE_TTS_START) {
        MEDIA_ERR_LOG("No TTS has started, so there is no need to stop.");
        MediaMutexUnLock(g_ttsMutex);
        return MEDIA_ERR;
    }
    IntellVoiceTtsDestoryProduceThread(g_ttsCtx);
    IntellVoiceTtsDestoryPlayThread(g_ttsCtx);
    CacheMemoryInfoDeInit(g_ttsCtx);
#ifdef SUPPORT_DUMP_DATA
    IntellVoiceTtsContextCloseDumpFile(g_ttsCtx);
#endif
    g_ttsCtx->pcmDataSize.clear();
    g_ttsCtx->states = IntellVoiceTtsStates::INTELL_VOICE_TTS_STOP;
    MediaMutexUnLock(g_ttsMutex);
    return MEDIA_OK;
}

int32_t IntellVoiceTtsEngineDeInit(void)
{
    MediaInitStaticMutexLock(&g_ttsMutex);
    MediaMutexLock(g_ttsMutex);
    if (g_ttsCtx == nullptr) {
        MEDIA_ERR_LOG("Tts Engine is not inited");
        MediaMutexUnLock(g_ttsMutex);
        return MEDIA_ERR;
    }
    if (g_ttsCtx->states != IntellVoiceTtsStates::INTELL_VOICE_TTS_STOP &&
        g_ttsCtx->states != IntellVoiceTtsStates::INTELL_VOICE_TTS_IDLE) {
        MediaMutexUnLock(g_ttsMutex);
        IntellVoiceTtsEngineStop();
        MediaMutexLock(g_ttsMutex);
    }
    DeInitAudioProduceContext(g_ttsCtx);
    DeInitAudioPlayContext(g_ttsCtx);
    (void)MediaMutexDestroy(&g_ttsCtx->mutex);
    (void)g_amInstance.DeactivateAudioInterrupt(g_ttsCtx->interrupt);
    delete g_ttsCtx;
    g_ttsCtx = nullptr;
    MediaMutexUnLock(g_ttsMutex);
    return MEDIA_OK;
}
