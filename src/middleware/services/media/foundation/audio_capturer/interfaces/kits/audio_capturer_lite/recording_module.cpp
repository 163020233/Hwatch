/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "recording_module.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <climits>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifdef SUPPORT_SYS_PRCTL
#include <sys/prctl.h>
#endif
#include <string>
#include "securec.h"
#include "media_errors.h"
#include "audio_manager.h"
#include "media_log.h"
#include "media_thread_adapt.h"
#include "xiaodu_voice.h"
#include "msg_center_protocol.h"
#include "cmsis_os2.h"
#include "audio_capturer.h"
#include "js_async_work.h"
#include "ogg.h"
#include "media_mem.h"
#include "media_chr_log.h"

#ifdef __cplusplus
extern "C" {
#endif
using namespace OHOS::ACELite;
using namespace OHOS::Media;
using namespace OHOS::Audio;
using namespace::Audio;
using Audio::InterruptListener;
using Audio::AudioInterrupt;
using Audio::AudioManager;

const uint32_t DIRECTORY_PERMISSION = 0755;
const uint32_t PARAM_NUMBER = 6;
const uint32_t PLAYER_CONTROL_CMD_LEN = 20;
const uint32_t READ_RETRY_TIME_US = 10000;
const uint32_t READ_COUNT_MAX = 1000;
const uint32_t GET_AUDIO_TIME_INTERVAL_TIMES = 10;
const uint32_t MAX_ARGC = 5;
const uint32_t PATH_STR_SIZE = 64;
const uint32_t DEFAULT_SAMPLE_RATE = 16000;
const uint32_t DEFAULT_CHANNEL_CNT = 1;
const AudioBitWidth DEFAULT_BIT_WIDTH = BIT_WIDTH_16;
const size_t DEFAULT_SAMPLE_TIME_MS = 10; // ms
const size_t DEFAULT_SAMPLE_OPUS_TIME_MS = 20; // ms
const uint32_t SECOND_TO_MICROSECOND = 1000;
const uint32_t BYTE_SIZE = 8;
const uint32_t OPUS_HEAD_LEN = 8;
const uint32_t DEFAULT_BUFFER_SIZE = DEFAULT_SAMPLE_RATE *
                                    DEFAULT_BIT_WIDTH / BYTE_SIZE *
                                    DEFAULT_SAMPLE_TIME_MS / SECOND_TO_MICROSECOND *
                                    DEFAULT_CHANNEL_CNT * 2; // equals 2 pcm frames size
const uint32_t DEFAULT_OGG_BUFFER_SIZE = 3 * 1024;
const char XIAODU_VOICE[] = "XiaoduVoice";
const char AUDIO_CAPTURER[] = "AudioCapturer";
const char PROP_SUCCESS[] = "success";
const char PROP_FAIL[] = "fail";
const char PROP_BUFFER_SIZE[] = "framesize";
const char PROP_FORMAT[] = "format";
const char PROP_SAMPLE_RATE[] = "samplerate";
const char PROP_BITEWIDTH[] = "bitwidth";
const char PROP_CHANNEL_CNT[] = "channelcnt";
const char PROP_FILE_DURATION_MS[] = "fileDuration";
const char PROP_FILE_FORMAT[] = "fileFormat";
const char FILE_FORMAT_OGG[] = "ogg";

#define CHK_NULL_RETURN(ptr, ret) \
    do { \
        if ((ptr) == nullptr) { \
            MEDIA_ERR_LOG("ptr null"); \
            return (ret); \
        } \
    } while (0)

// for ogg muxer
const uint32_t MILLSECOND_TO_SECOND = 1000;
const uint32_t HEADER_STRLEN = 9;
const uint32_t OPUS_DELAY_TIME_DIVISOR = 0;
const uint32_t OPUS_DELAY_TIME_DIVIDED = 100;
const uint32_t OPUS_SPLIT_PAGE_SIZE = 100;
const uint32_t OPUS_ID_HEADER_LEN = 19;
const uint32_t OPUS_COMMENT_HEADER_LEN = 25;
const uint32_t OPUS_COMMENT_TAIL_LEN = 4;
const uint32_t OPUS_OPUSTAGS_LEN = 12;
const uint32_t OPUS_SAMPLERATE = 48000;
const uint32_t OPUS_SAMPLE_POINT = 480;
const size_t OPUS_FRMAE_LEN = 30; // byte
const size_t OGG_RESERVED_INFO_LEN = 512; // byte 预留的OPUS 封装成OGG额外的数据大小
const uint32_t FRAME_HEAD_LEN_BIG = 8;

struct SampleAudioCapturerInfo {
    /** Audio source type */
    AudioSourceType inputSource = AUDIO_MIC;
    /** Audio codec format */
    AudioCodecFormat audioFormat = AUDIO_DEFAULT;
    /** Sampling rate */
    int32_t sampleRate = DEFAULT_SAMPLE_RATE;
    /** Bit rate */
    int32_t bitRate = 0;
    /** Number of audio channels */
    int32_t channelCount = DEFAULT_CHANNEL_CNT;
    /** Audio stream type */
    AudioStreamType streamType = TYPE_MEDIA;
    /** Bit width */
    AudioBitWidth bitWidth = DEFAULT_BIT_WIDTH;
    /** Session id */
    AudioSession sessionID = AUDIO_SESSION_ID_NONE;
};

typedef struct OggMuxerContext {
    ogg_stream_state oggStream = {};
    int64_t sampleRate;
    int64_t channels;
    int64_t granulepos;
    int64_t packetCount;
    int32_t serialno;
    uint32_t commentLen;
    bool isInited;
    ogg_page *page;
    ogg_packet *packet;
    unsigned char *idHeader;
    unsigned char *commentHeader;
} OggMuxerContext;

struct NameAudioFormat {
    std::string name;
    AudioCodecFormat audioFormat;
};

typedef struct AudioSourceInput {
    size_t framesize;
    uint8_t *buffer;
    size_t storedBuffersize = DEFAULT_BUFFER_SIZE;
    uint8_t *storedBuffer;
    int32_t writePos;
    AudioCapturer *audioCap;
    SampleAudioCapturerInfo info;
    bool recordOgg;
} AudioSourceInput;

struct JsCaptureParams {
    JSIValue thisVal = JSI::CreateUndefined();
    JSIValue successCb = JSI::CreateUndefined();
    JSIValue failCb = JSI::CreateUndefined();
};

typedef enum AudioCapturerState {
    OFF = 0,
    ON,
} AudioCapturerState;

typedef struct JsCapturerContext {
    char control[PLAYER_CONTROL_CMD_LEN];
    bool needStop;
    AudioCapturerState capturerState;
    AudioCapturerState listenerState;
    AudioInterrupt interrupt;
    MediaThreadIdHandle audioCaptureProcess;
    pcm_stream_queue_msg msg;
} JsCapturerContext;

static NameAudioFormat g_audioCodecFormat[] = {
    {"mp3", MP3},
    {"ape", APE},
    {"msbc", mSBC},
    {"vorbis", VORBIS},
    {"opus", OPUS},
    {"flac", FLAC},
    {"g726", G726},
    {"g711u", G711U},
    {"g711a", G711A},
    {"aaceld", AAC_ELD},
    {"silk", SILK},
    {"pcm", PCM}
};

static bool g_interruptHintStop = false;
static std::shared_ptr<AudioCapturer> g_audioCap;
static MediaMutexHandle g_jsCaptureMutex = nullptr;
class CaptureInterruptListener : public InterruptListener {
public:
    CaptureInterruptListener() {};
    ~CaptureInterruptListener() override {};

    void OnInterrupt(int32_t type, int32_t hint) override
    {
        if (type == INTERRUPT_TYPE_BEGIN && hint == INTERRUPT_HINT_PAUSE) {
            MEDIA_INFO_LOG("record OnInterrupt pause not supported");
        }
        if (type == INTERRUPT_TYPE_END && hint == INTERRUPT_HINT_RESUME) {
            MEDIA_INFO_LOG("record OnInterrupt resume not supported");
        }
        if (type == INTERRUPT_TYPE_BEGIN && hint == INTERRUPT_HINT_STOP) {
            g_interruptHintStop = true;
        }
        if (type == INTERRUPT_TYPE_END && hint == INTERRUPT_HINT_STOP) {
            MEDIA_INFO_LOG("record OnInterrupt stop not supported");
        }
    }
};

static AudioSourceInput g_audioSourceProcessInput;
static AudioManager& g_amInstance = AudioManager::GetInstance();
static JsCapturerContext g_capturerCtx;
static std::shared_ptr<CaptureInterruptListener> g_captureInterruptListener;
static struct JsCaptureParams g_jsCaptureParams;
static OggMuxerContext g_oggMuxerCtx = {};

static int32_t OggMuxerHeaderInit(SampleAudioCapturerInfo &info)
{
    const char *idHeader = "OpusHead";
    const char *commentHeader = "OpusTags";
    const char *encoderName = "Lavf62.3.100";
    const char *encoderInfo = "encoder=Lavf62.3.100";
    uint32_t sampleRate = DEFAULT_SAMPLE_RATE;
    uint32_t preSkip = sampleRate * OPUS_DELAY_TIME_DIVISOR / OPUS_DELAY_TIME_DIVIDED;
    uint32_t commentHeaderLen = strlen(commentHeader); // 0-7
    uint32_t encoderNameLen = strlen(encoderName); // 12-31
    uint32_t encoderInfoLen = strlen(encoderInfo); // 40-67
    uint32_t encoderNamePosition = commentHeaderLen + OPUS_COMMENT_TAIL_LEN;
    uint32_t encoderInfoPosition = commentHeaderLen + encoderNameLen + OPUS_OPUSTAGS_LEN;
    uint32_t encoderInfoLenPostion = encoderNamePosition + strlen(encoderName);
    g_oggMuxerCtx.commentLen = encoderInfoLen + encoderNameLen + commentHeaderLen + OPUS_OPUSTAGS_LEN;
    g_oggMuxerCtx.idHeader = (uint8_t *)(MediaMemCalloc(OPUS_ID_HEADER_LEN));
    CHK_NULL_RETURN(g_oggMuxerCtx.idHeader, MEDIA_ERR);
    if (memcpy_s(g_oggMuxerCtx.idHeader, OPUS_ID_HEADER_LEN, idHeader, strlen(idHeader)) != 0) {
        MEDIA_ERR_LOG("ogg idHeader memcpy failed!");
        goto FREE_ID_HEADER;
    }
    g_oggMuxerCtx.idHeader[0x8] = 0x01;
    g_oggMuxerCtx.idHeader[0x9] = info.channelCount;
    g_oggMuxerCtx.idHeader[0xA] = preSkip & 0xFF;
    g_oggMuxerCtx.idHeader[0xB] = (preSkip >> 0x8);
    g_oggMuxerCtx.idHeader[0xC] = sampleRate & 0xFF;
    g_oggMuxerCtx.idHeader[0xD] = (sampleRate >> 0x8) & 0xFF;
    g_oggMuxerCtx.idHeader[0xE] = (sampleRate >> 0x10) & 0xFF;
    g_oggMuxerCtx.idHeader[0xF] = (sampleRate >> 0x18) & 0xFF;
    g_oggMuxerCtx.commentHeader = (uint8_t *)(MediaMemCalloc(g_oggMuxerCtx.commentLen));
    if (g_oggMuxerCtx.commentHeader == NULL) {
        MEDIA_ERR_LOG("ogg commentHeader malloc failed!");
        goto FREE_ID_HEADER;
    }
    if (memcpy_s(g_oggMuxerCtx.commentHeader, g_oggMuxerCtx.commentLen, commentHeader, strlen(commentHeader)) != 0) {
        MEDIA_ERR_LOG("ogg commentHeader memcpy failed!");
        goto FREE_COMMENT_HEADER;
    }
    g_oggMuxerCtx.commentHeader[0x8] = encoderNameLen & 0xFF;
    g_oggMuxerCtx.commentHeader[0x9] = (encoderNameLen >> 0x8) & 0xFF;
    g_oggMuxerCtx.commentHeader[0xA] = (encoderNameLen >> 0x10) & 0xFF;
    g_oggMuxerCtx.commentHeader[0xB] = (encoderNameLen >> 0x18) & 0xFF;
    if (memcpy_s(g_oggMuxerCtx.commentHeader + encoderNamePosition,
        g_oggMuxerCtx.commentLen - encoderNamePosition, encoderName,
        strlen(encoderName)) != 0) {
        MEDIA_ERR_LOG("ogg commentHeader memcpy failed!");
        goto FREE_COMMENT_HEADER;
    }
    
    g_oggMuxerCtx.commentHeader[encoderInfoLenPostion] = 1;
    g_oggMuxerCtx.commentHeader[encoderInfoLenPostion + 0x1] = 0;
    g_oggMuxerCtx.commentHeader[encoderInfoLenPostion + 0x2] = 0;
    g_oggMuxerCtx.commentHeader[encoderInfoLenPostion + 0x3] = 0;
    g_oggMuxerCtx.commentHeader[encoderInfoLenPostion + 0x4] = encoderInfoLen & 0xFF;
    g_oggMuxerCtx.commentHeader[encoderInfoLenPostion + 0x5] = (encoderInfoLen >> 0x8) & 0xFF;
    g_oggMuxerCtx.commentHeader[encoderInfoLenPostion + 0x6] = (encoderInfoLen >> 0x10) & 0xFF;
    g_oggMuxerCtx.commentHeader[encoderInfoLenPostion + 0x7] = (encoderInfoLen >> 0x18) & 0xFF;
    if (memcpy_s(g_oggMuxerCtx.commentHeader + encoderInfoPosition,
        g_oggMuxerCtx.commentLen - encoderInfoPosition,
        encoderInfo, encoderInfoLen) != 0) {
        MEDIA_ERR_LOG("ogg commentHeader memcpy failed!");
        goto FREE_COMMENT_HEADER;
    }
    return MEDIA_OK;
FREE_COMMENT_HEADER:
    MediaMemFree(g_oggMuxerCtx.commentHeader);
    g_oggMuxerCtx.commentHeader = NULL;
FREE_ID_HEADER:
    MediaMemFree(g_oggMuxerCtx.idHeader);
    g_oggMuxerCtx.idHeader = NULL;
    return MEDIA_ERR;
}

static int32_t OggMuxerStreamFlush()
{
    int32_t writePos = g_audioSourceProcessInput.writePos;
    uint8_t *storedBuffer = g_audioSourceProcessInput.storedBuffer + writePos;
    int32_t storedBuffersize = g_audioSourceProcessInput.storedBuffersize;
    while (ogg_stream_flush(&g_oggMuxerCtx.oggStream, g_oggMuxerCtx.page)) {
        if (memcpy_s(storedBuffer, storedBuffersize - writePos,
            g_oggMuxerCtx.page->header, g_oggMuxerCtx.page->header_len) != EOK) {
            MEDIA_ERR_LOG("header_len:%d, storedBuffersize - writePos:%d", g_oggMuxerCtx.page->header_len, storedBuffersize - writePos);
            return MEDIA_ERR;
        }
        writePos += g_oggMuxerCtx.page->header_len;
        storedBuffer += g_oggMuxerCtx.page->header_len;
        if (memcpy_s(storedBuffer, storedBuffersize - writePos,
            g_oggMuxerCtx.page->body, g_oggMuxerCtx.page->body_len) != EOK) {
            MEDIA_ERR_LOG("body_len:%d, storedBuffersize - writePos:%d", g_oggMuxerCtx.page->body_len, storedBuffersize - writePos);
            return MEDIA_ERR;
        }
        writePos += g_oggMuxerCtx.page->body_len;
        storedBuffer += g_oggMuxerCtx.page->body_len;
    }
    g_audioSourceProcessInput.writePos = writePos;
    return MEDIA_OK;
}

static int32_t OggMuxerStreamInit(SampleAudioCapturerInfo &info)
{
    g_oggMuxerCtx.sampleRate = info.sampleRate;
    g_oggMuxerCtx.channels = info.channelCount;
    g_oggMuxerCtx.granulepos = 0;
    g_oggMuxerCtx.packetCount = 0;
    g_oggMuxerCtx.serialno = 0x1; // unique serialno
    if (ogg_stream_init(&g_oggMuxerCtx.oggStream, g_oggMuxerCtx.serialno) != MEDIA_OK) {
        MEDIA_ERR_LOG("ogg stream init failed!");
        return MEDIA_ERR;
    }
    return MEDIA_OK;
}

static void OggMuxerStreamDeInit(void)
{
    ogg_stream_clear(&g_oggMuxerCtx.oggStream);
}

static void OggMuxerPacketIn(unsigned char *packet, int64_t bytes)
{
    g_oggMuxerCtx.packet->packet = packet;
    g_oggMuxerCtx.packet->bytes = bytes;
    g_oggMuxerCtx.packet->b_o_s = g_oggMuxerCtx.packetCount != 0 ? 0 : 1; // 1 means first page
    g_oggMuxerCtx.packet->e_o_s = 0;
    g_oggMuxerCtx.packet->granulepos = g_oggMuxerCtx.packetCount != 0 ? g_oggMuxerCtx.granulepos : -1; // -1 means first page
    g_oggMuxerCtx.packet->packetno = g_oggMuxerCtx.packetCount++;
    ogg_stream_packetin(&g_oggMuxerCtx.oggStream, g_oggMuxerCtx.packet);
}

static int32_t OggMuxerWriteHeader() 
{
    OggMuxerPacketIn(g_oggMuxerCtx.idHeader, OPUS_ID_HEADER_LEN);
    if (OggMuxerStreamFlush() != MEDIA_OK) {
        MEDIA_ERR_LOG("ogg packet flush failed!");
        return MEDIA_ERR;
    }
    OggMuxerPacketIn(g_oggMuxerCtx.commentHeader, g_oggMuxerCtx.commentLen);
    if (OggMuxerStreamFlush() != MEDIA_OK) {
        MEDIA_ERR_LOG("ogg header flush failed!");
        return MEDIA_ERR;
    }
    return MEDIA_OK;
}

static int32_t OggMuxerWritePage()
{
#ifdef SUPPORT_OGG_SPLIT_PAGE
    if ((g_oggMuxerCtx.packetCount - 2) % OPUS_SPLIT_PAGE_SIZE == 0) { // remove 2 header page
        if (OggMuxerStreamFlush() != MEDIA_OK) {
            MEDIA_ERR_LOG("ogg stream flush fail!");
            return MEDIA_ERR;
        }
        return MEDIA_OK;
    }
#else
    int32_t writePos = g_audioSourceProcessInput.writePos;
    uint8_t *storedBuffer = g_audioSourceProcessInput.storedBuffer + writePos;
    int32_t storedBuffersize = g_audioSourceProcessInput.storedBuffersize;
    if (ogg_stream_pageout(&g_oggMuxerCtx.oggStream, g_oggMuxerCtx.page)) {
        if (memcpy_s(storedBuffer, storedBuffersize - writePos,
            g_oggMuxerCtx.page->header, g_oggMuxerCtx.page->header_len) != EOK) {
            MEDIA_ERR_LOG("errno:%d, errmsg:%s", errno, strerror(errno));
            return MEDIA_ERR;
        }
        writePos += g_oggMuxerCtx.page->header_len;
        storedBuffer += g_oggMuxerCtx.page->header_len;
        if (memcpy_s(storedBuffer, storedBuffersize - writePos,
            g_oggMuxerCtx.page->body, g_oggMuxerCtx.page->body_len) != EOK) {
            MEDIA_ERR_LOG("errno:%d, errmsg:%s", errno, strerror(errno));
            return MEDIA_ERR;
        }
        writePos += g_oggMuxerCtx.page->body_len;
        storedBuffer += g_oggMuxerCtx.page->body_len;
    }
    g_audioSourceProcessInput.writePos = writePos;
#endif
    return MEDIA_OK;
}

static int32_t OggMuxerWritePacket(const int64_t bufferLen)
{
    unsigned char *buffer = g_audioSourceProcessInput.buffer;
    OggMuxerPacketIn(buffer, bufferLen);
    // capture frequency is 10ms for each frame
    g_oggMuxerCtx.granulepos += OPUS_SAMPLERATE * DEFAULT_SAMPLE_TIME_MS / MILLSECOND_TO_SECOND;
    return OggMuxerWritePage();
}

static int32_t OggMuxerWriteEosPacket()
{
    unsigned char emptyData = 0;
    g_oggMuxerCtx.packet->packet = &emptyData;
    g_oggMuxerCtx.packet->bytes = 1;
    g_oggMuxerCtx.packet->b_o_s = 0;
    g_oggMuxerCtx.packet->e_o_s = 1;
    g_oggMuxerCtx.packet->granulepos = g_oggMuxerCtx.granulepos;
    g_oggMuxerCtx.packet->packetno = g_oggMuxerCtx.packetCount;
    ogg_stream_packetin(&g_oggMuxerCtx.oggStream, g_oggMuxerCtx.packet);
    return OggMuxerStreamFlush();
}

static int32_t OggMuxerCtxInit(SampleAudioCapturerInfo &info)
{
    if (g_oggMuxerCtx.isInited) {
        MEDIA_ERR_LOG("Ogg context has inited!");
        goto EXIT;
    }
    g_oggMuxerCtx.packet = (ogg_packet *)MediaMemCalloc(sizeof(ogg_packet));
    if (g_oggMuxerCtx.packet == nullptr) {
        MEDIA_ERR_LOG("ogg context packet malloc failed!");
        goto EXIT;
    }
    g_oggMuxerCtx.page = (ogg_page *)MediaMemCalloc(sizeof(ogg_page));
    if (g_oggMuxerCtx.page == nullptr) {
        MEDIA_ERR_LOG("ogg context page malloc failed!");
        goto FREE_PACKET;
    }

    if (OggMuxerHeaderInit(info) != MEDIA_OK) {
        MEDIA_ERR_LOG("Header init failed!");
        goto FREE_PAGE;
    }
    g_oggMuxerCtx.isInited = true;
    return MEDIA_OK;
FREE_PAGE:
    MediaMemFree(g_oggMuxerCtx.page);
FREE_PACKET:
    MediaMemFree(g_oggMuxerCtx.packet);
EXIT:
    return MEDIA_ERR;
}

static void OggMuxerCtxDeInit()
{
    g_oggMuxerCtx.sampleRate = 0;
    g_oggMuxerCtx.channels = 0;
    g_oggMuxerCtx.granulepos = 0;
    g_oggMuxerCtx.packetCount = 0;
    MediaMemFree(g_oggMuxerCtx.packet);
    g_oggMuxerCtx.packet = nullptr;
    MediaMemFree(g_oggMuxerCtx.page);
    g_oggMuxerCtx.page = nullptr;
    MediaMemFree(g_oggMuxerCtx.idHeader);
    g_oggMuxerCtx.idHeader = nullptr;
    MediaMemFree(g_oggMuxerCtx.commentHeader);
    g_oggMuxerCtx.commentHeader = nullptr;
    g_oggMuxerCtx.isInited = false;
}

static void InitResource()
{
    g_jsCaptureParams.thisVal = JSI::CreateUndefined();
    g_jsCaptureParams.successCb = JSI::CreateUndefined();
    g_jsCaptureParams.failCb = JSI::CreateUndefined();
    g_jsCaptureMutex = MediaMutexCreate(nullptr);
}

static void DestroyResource()
{
    JSI::ReleaseValueList(g_jsCaptureParams.thisVal,
        g_jsCaptureParams.successCb,
        g_jsCaptureParams.failCb);
    g_jsCaptureParams.thisVal = JSI::CreateUndefined();
    g_jsCaptureParams.successCb = JSI::CreateUndefined();
    g_jsCaptureParams.failCb = JSI::CreateUndefined();
    MediaMutexDestroy(&g_jsCaptureMutex);
}

static void AudioCaptureExit()
{
    if (!g_audioCap->Stop()) {
        MEDIA_ERR_LOG("record Stop failed");
    }
    if (!g_audioCap->Release()) {
        MEDIA_ERR_LOG("record Release failed");
    }
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_CAPTURE_JS, MEDIA_CHR_JS_AUDIO_CAPTURE_THREAD,
        MEDIA_CHR_DESTORY_AUDIO_STREAM, 0);
    delete g_audioSourceProcessInput.buffer;
    g_audioSourceProcessInput.buffer = nullptr;

    if (g_amInstance.DeactivateAudioInterrupt(g_capturerCtx.interrupt) != 0) {
        MEDIA_ERR_LOG("record deactivate audio interrupt failed");
    }
    g_audioCap.reset();
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_CAPTURE_JS, MEDIA_CHR_JS_AUDIO_CAPTURE_THREAD,
        MEDIA_CHR_DEACTIVE_AUDIO_INTERRPUT, 0);
    MEDIA_INFO_LOG("record AudioCaptureProcess end");
}

static int32_t AddFrameHeader(uint8_t *bigEndArray, pcm_stream_queue_msg *msg, int32_t bufferLen)
{
    /* 该函数用于填充到8字节大端序帧头00 00 00 14 01 00 00 00 */
    bigEndArray[0] = (bufferLen >> 0x18) & 0xFF;
    bigEndArray[0x1] = (bufferLen >> 0x10) & 0xFF;
    bigEndArray[0x2] = (bufferLen >> 0x8) & 0xFF;
    bigEndArray[0x3] = bufferLen & 0xFF;
    bigEndArray[0x4] = 1;
    bigEndArray[0x5] = 0;
    bigEndArray[0x6] = 0;
    bigEndArray[0x7] = 0;
    int32_t cpyRet = memcpy_s(msg->data, msg->len, bigEndArray, FRAME_HEAD_LEN_BIG);
    if (cpyRet != 0) {
        MEDIA_ERR_LOG("memcpy failed ret:%d", cpyRet);
        return MEDIA_ERR;
    }
    cpyRet = memcpy_s(msg->data + FRAME_HEAD_LEN_BIG, msg->len - FRAME_HEAD_LEN_BIG,
        g_audioSourceProcessInput.buffer, bufferLen);
    if (cpyRet != 0) {
        MEDIA_ERR_LOG("memcpy failed ret:%d", cpyRet);
        return MEDIA_ERR;
    }
    return MEDIA_OK;
}

static void SuccessCallBack(JSIValue *jsiValue)
{
    if (JSI::ValueIsUndefined(g_jsCaptureParams.successCb)) {
        MEDIA_ERR_LOG("success cbk undefined!");
        return;
    }
    if (JSI::ValueIsUndefined(*jsiValue)) {
        JSI::CallFunction(g_jsCaptureParams.successCb, g_jsCaptureParams.thisVal, nullptr, 0);
    } else {
        JSI::CallFunction(g_jsCaptureParams.successCb, g_jsCaptureParams.thisVal, jsiValue, ARGC_ONE);
    }
}

static void FailCallBack(int ret, const char* errLog)
{
    JSIValue errCode = JSI::CreateNumber(ret);
    JSIValue errInfo = JSI::CreateString(errLog);
    JSIValue argv[ARGC_TWO] = {errInfo, errCode};
    if (!JSI::ValueIsUndefined(g_jsCaptureParams.failCb)) {
        JSI::CallFunction(g_jsCaptureParams.failCb, g_jsCaptureParams.thisVal, argv, ARGC_TWO);
    }
    JSI::ReleaseValueList(errInfo, errCode, ARGS_END);
}

static void* AudioCaptureProcess(void* arg)
{
    MEDIA_UNUSED(arg);
    int32_t readCnt = 0;
    Timestamp timeStamp;
	MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_CAPTURE_JS, MEDIA_CHR_JS_AUDIO_CAPTURE_THREAD,
        MEDIA_CHR_GET_CAPTURE_DATA_START, 0);
    g_capturerCtx.msg = {MSGCENTER_TYPE_ID_XIAODU_SEND_PCM_STREAM, nullptr, 0};
    while (!g_capturerCtx.needStop && !g_interruptHintStop) {
        int32_t ret = g_audioCap->Read(g_audioSourceProcessInput.buffer, g_audioSourceProcessInput.framesize, false);
        if (ret <= 0) {
            if (ret == ERR_RETRY_READ) {
                usleep(READ_RETRY_TIME_US);
                continue;
            }
            usleep(READ_RETRY_TIME_US);
            continue;
        }
        g_capturerCtx.msg.len = ret;
        g_capturerCtx.msg.len = ret + FRAME_HEAD_LEN_BIG;
        g_capturerCtx.msg.data = (uint8_t *)malloc(g_capturerCtx.msg.len); /* 消费者释放内存 */
        if (g_capturerCtx.msg.data == nullptr) {
            MEDIA_ERR_LOG("malloc fail. len = %d", g_capturerCtx.msg.len);
            continue;
        }
        uint8_t bigEndArray[0x8] = {}; /* 小度语音和掌禅app需要8个字节大端序的帧头对opus进行解析，dsp原始编码是4个字节的小端序帧头,hal层已将帧头去除 */
        if (AddFrameHeader(bigEndArray, &g_capturerCtx.msg, ret) != MEDIA_OK) {
            free(g_capturerCtx.msg.data);
            g_capturerCtx.msg.data = nullptr;
            break;
        }
        errcode_t res = osMessageQueuePut(GetPcmMsgQueueId(), &g_capturerCtx.msg, 0, 0);
        if (res != osOK) {
            free(g_capturerCtx.msg.data);
            MEDIA_ERR_LOG("put pcm stream fail, len = %d, res = 0x%x", g_capturerCtx.msg.len, res);
            g_capturerCtx.msg.data = nullptr;
            continue;
        }

        readCnt++;
        if (readCnt % GET_AUDIO_TIME_INTERVAL_TIMES == 0) {
            if (g_audioCap->GetAudioTime(timeStamp, Timestamp::Timebase::MONOTONIC)) {
                MEDIA_INFO_LOG("read %d frames timestamp seconds:%ld, nanoseconds:%ld",
                    readCnt, timeStamp.time.tv_sec, timeStamp.time.tv_nsec);
            }
        }
        MEDIA_DEBUG_LOG("audioCap Read readCnt = %d, size = %d", readCnt, ret);
    }
    AudioCaptureExit();
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_CAPTURE_JS, MEDIA_CHR_JS_AUDIO_CAPTURE_THREAD, MEDIA_CHR_THREAD_EXIT, 0);
    return nullptr;
}

static void DisPatchSendBufferToJS(void *arg)
{
    (void)arg;
    int32_t writePos = g_audioSourceProcessInput.writePos;
    uint8_t *arrayPtr = nullptr;
    JSIValue result = JSI::CreateObject();
    JSIValue arrayBuffer = JSI::CreateArrayBuffer(writePos, arrayPtr);
    if (memcpy_s(arrayPtr, writePos,
        g_audioSourceProcessInput.storedBuffer,
        writePos) != 0) {
        JSI::ReleaseValueList(result, arrayBuffer);
        FailCallBack(MEDIA_ERR, "memcpy failed!");
        MEDIA_ERR_LOG("memcpy failed!");
        return;
    }
    JSI::SetNumberProperty(result, "ret", MEDIA_OK);
    JSI::SetNumberProperty(result, "dataLen", writePos);
    JSI::SetNamedProperty(result, "data", arrayBuffer);
    SuccessCallBack(&result);

    // 发送成功，清理storedBuffer
    if (memset_s(g_audioSourceProcessInput.storedBuffer,
        g_audioSourceProcessInput.storedBuffersize, 0, writePos) != 0) {
        FailCallBack(MEDIA_ERR, "memset_s failed!");
        MEDIA_ERR_LOG("memset_s failed!");
        return;
    }
    g_audioSourceProcessInput.writePos = 0;
    JSI::ReleaseValueList(result, arrayBuffer);
    return;
}

static void SendBufferToJS()
{
    JsAsyncWork::DispatchAsyncWork(DisPatchSendBufferToJS, nullptr);
}

static int32_t WriteOggBuffer(const int32_t readLen)
{
    if (OggMuxerWritePacket(readLen) != MEDIA_OK) {
        MEDIA_ERR_LOG("ogg write Packet failed!");
        return MEDIA_ERR;
    }
    return ((g_oggMuxerCtx.packetCount - 2) % 100 == 0 ) ? MEDIA_OK : readLen;
}

// 通过返回值判断是否存入缓存buffer，readLen表示写入成功，不需要后续上报流程，-1 表示写入失败，需要上报后再写入
static int32_t WriteAudioBuffer(const int32_t readLen)
{
    if (g_audioSourceProcessInput.recordOgg) {
        return WriteOggBuffer(readLen);
    }
    // 其他非OGG 上报裸流场景
    int32_t writePos = g_audioSourceProcessInput.writePos;
    if (writePos + readLen > static_cast<int32_t>(g_audioSourceProcessInput.storedBuffersize)) {
        return MEDIA_ERR;
    }

    if (memcpy_s(g_audioSourceProcessInput.storedBuffer + writePos,
        g_audioSourceProcessInput.storedBuffersize - writePos,
        g_audioSourceProcessInput.buffer,
        readLen) != 0) {
        FailCallBack(MEDIA_ERR, "memcpy failed!");
        MEDIA_ERR_LOG("memcpy failed!");
        return MEDIA_ERR;
    }
    g_audioSourceProcessInput.writePos += readLen;
    return readLen;
}

static int32_t PrepareSendOggBuffer()
{
    if (OggMuxerStreamInit(g_audioSourceProcessInput.info) != MEDIA_OK) {
        MEDIA_ERR_LOG("OggMuxerStreamInit failed!");
        return MEDIA_ERR;
    }
    if (OggMuxerWriteHeader() != MEDIA_OK) {
        MEDIA_ERR_LOG("OggMuxerWriteHeader failed!");
        return MEDIA_ERR;
    }
    g_oggMuxerCtx.granulepos = OPUS_SAMPLE_POINT;
    return MEDIA_OK;
}

static int32_t SendAudioBuffer()
{
    if (g_audioSourceProcessInput.recordOgg && g_capturerCtx.needStop) {
        OggMuxerWriteEosPacket();
        OggMuxerStreamDeInit();
    }
    SendBufferToJS();
    return MEDIA_OK;
}

static void* AudioStreamCaptureProcess(void* arg)
{
    g_audioSourceProcessInput.storedBuffer = new uint8_t[g_audioSourceProcessInput.storedBuffersize];
    if (g_audioSourceProcessInput.storedBuffer == nullptr) {
        MEDIA_ERR_LOG("new storedBuffer failed!");
        return nullptr;
    }
    if (g_audioSourceProcessInput.recordOgg &&
        PrepareSendOggBuffer() != MEDIA_OK) {
        delete []g_audioSourceProcessInput.storedBuffer;
        g_audioSourceProcessInput.storedBuffer = nullptr;
        MEDIA_ERR_LOG("PrepareSendOggBuffer failed!");
        return nullptr;
    }
    MEDIA_INFO_LOG("start audio capture");
    while (true) {
        MediaMutexLock(g_jsCaptureMutex);
        if (g_capturerCtx.needStop || g_interruptHintStop) {
            MediaMutexUnLock(g_jsCaptureMutex);
            break;
        }
        int32_t readLen = g_audioCap->Read(g_audioSourceProcessInput.buffer,
            g_audioSourceProcessInput.framesize, false);
        if (readLen <= 0) {
            usleep(READ_RETRY_TIME_US);
            MediaMutexUnLock(g_jsCaptureMutex);
            continue;
        }
        // 采集的音频数据写入 storedBuffer 中，写入成功继续采集数据
        int32_t ret = WriteAudioBuffer(readLen);
        if (ret == readLen) {
            MediaMutexUnLock(g_jsCaptureMutex);
            continue;
        }
        if (SendAudioBuffer() != MEDIA_OK) {
            MediaMutexUnLock(g_jsCaptureMutex);
            MEDIA_ERR_LOG("SendBuffer failed");
            break;
        }
        // 写入本次采集的音频数据
        if (WriteAudioBuffer(readLen) != readLen) {
            MediaMutexUnLock(g_jsCaptureMutex);
            MEDIA_ERR_LOG("WriteAudioBuffer failed");
            continue;
        }
        MediaMutexUnLock(g_jsCaptureMutex);
    }
    SendAudioBuffer();
    delete []g_audioSourceProcessInput.storedBuffer;
    g_audioSourceProcessInput.storedBuffer = nullptr;
    g_audioSourceProcessInput.writePos = 0;
    AudioCaptureExit();
    MEDIA_INFO_LOG("exit audio captrue");
    return nullptr;
}

static int32_t AudioCaptureGetFormat(const char *format, AudioCodecFormat &audioFormat)
{
    std::string inputFormat = format;
    for (size_t i = 0; i < (sizeof(g_audioCodecFormat) / sizeof(g_audioCodecFormat[0])); i++) {
        if (inputFormat == g_audioCodecFormat[i].name) {
            audioFormat = g_audioCodecFormat[i].audioFormat;
            MEDIA_INFO_LOG("record [AudioCaptureSample] set audioFormat %s", g_audioCodecFormat[i].name.c_str());
            return MEDIA_OK;
        }
    }
    MEDIA_ERR_LOG("invalid input format!");
    return MEDIA_INVALID_PARAM;
}

static int32_t AudioStreamCaptureGetInfo(const JSIValue* argv, SampleAudioCapturerInfo &audioCapturerInfo)
{
    JSIValue buffersize = JSI::GetNamedProperty(argv[0], PROP_BUFFER_SIZE);
    if (JSI::ValueIsUndefined(buffersize)) {
        FailCallBack(MEDIA_ERR, "invalid buffer size!");
        JSI::ReleaseValueList(buffersize);
        return MEDIA_ERR;
    }
    size_t size = static_cast<size_t>(JSI::ValueToNumber(buffersize));
    /* 1: buffersize */
    g_audioSourceProcessInput.storedBuffersize = size < DEFAULT_BUFFER_SIZE ? DEFAULT_BUFFER_SIZE : size;
    MEDIA_INFO_LOG("record set capture parameter buffersize:%zu", g_audioSourceProcessInput.storedBuffersize);
    JSIValue format = JSI::GetNamedProperty(argv[0], PROP_FORMAT);
    JSIValue fileFormat = JSI::GetNamedProperty(argv[0], PROP_FILE_FORMAT);
    JSIValue fileDuration = JSI::GetNamedProperty(argv[0], PROP_FILE_DURATION_MS);
    if (!JSI::ValueIsUndefined(format)) {
        /* 2: audioFormat */
        if (AudioCaptureGetFormat(JSI::ValueToString(format), audioCapturerInfo.audioFormat) != MEDIA_OK) {
            FailCallBack(MEDIA_ERR, "invalld format!");
            JSI::ReleaseValueList(buffersize, format, fileFormat, fileDuration);
            return MEDIA_ERR;
        }
        MEDIA_INFO_LOG("record set audioFormat:%d", audioCapturerInfo.audioFormat);
    }
    if (!JSI::ValueIsUndefined(fileFormat)) {
        std::string fileFormatStr = JSI::ValueToString(fileFormat);
        std::string fileFormatOgg = FILE_FORMAT_OGG;
        if (fileFormatStr != fileFormatOgg) {
            FailCallBack(MEDIA_ERR, "only support file format ogg!");
            JSI::ReleaseValueList(buffersize, format, fileFormat, fileDuration);
            return MEDIA_ERR;
        }
        g_audioSourceProcessInput.storedBuffersize = DEFAULT_OGG_BUFFER_SIZE;
        g_audioSourceProcessInput.recordOgg = true;
    }

    JSIValue sampleRate = JSI::GetNamedProperty(argv[0], PROP_SAMPLE_RATE);
    if (!JSI::ValueIsUndefined(sampleRate)) {
        /* 3: samplerate */
        audioCapturerInfo.sampleRate = static_cast<int32_t>(JSI::ValueToNumber(sampleRate));
        MEDIA_INFO_LOG("record set capture parameter sampleRate:%d", audioCapturerInfo.sampleRate);
    }
    JSIValue channelCount = JSI::GetNamedProperty(argv[0], PROP_CHANNEL_CNT);
    if (!JSI::ValueIsUndefined(channelCount)) {
        /* 4: channelCount */
        audioCapturerInfo.channelCount = static_cast<int32_t>(JSI::ValueToNumber(channelCount));
        MEDIA_INFO_LOG("record set capture parameter channelCount:%d", audioCapturerInfo.channelCount);
    }
    JSIValue bitWidth = JSI::GetNamedProperty(argv[0], PROP_BITEWIDTH);
    if (!JSI::ValueIsUndefined(bitWidth)) {
        /* 5: bitWidth */
        audioCapturerInfo.bitWidth = static_cast<AudioBitWidth>((AudioBitWidth)JSI::ValueToNumber(bitWidth));
        MEDIA_INFO_LOG("record set capture parameter bitWidth:%d", audioCapturerInfo.bitWidth);
    }
    JSI::ReleaseValueList(buffersize, format, fileFormat, fileDuration, sampleRate, channelCount, bitWidth);
    return MEDIA_OK;
}

static int32_t AudioCaptureGetInfo(const char **argv, SampleAudioCapturerInfo &audioCapturerInfo, std::string &filePath)
{
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_CAPTURE_JS, MEDIA_CHR_JS_AUDIO_CAPTURE_START, MEDIA_CHR_JS_GET_CAPTURE_INFO, 0);
    AudioCaptureGetFormat(argv[0x1], audioCapturerInfo.audioFormat);
    audioCapturerInfo.sampleRate = atoi(argv[0x2]); /* 2: samplerate */
    MEDIA_INFO_LOG("record set capture parameter sampleRate:%d", audioCapturerInfo.sampleRate);
    audioCapturerInfo.channelCount = atoi(argv[0x3]); /* 3: channel */
    MEDIA_INFO_LOG("record set capture parameter channel:%d", audioCapturerInfo.channelCount);
    audioCapturerInfo.bitWidth = static_cast<AudioBitWidth>(atoi(argv[0x4])); /* 4: bitWidth */
    MEDIA_INFO_LOG("record set capture parameter bitWidth:%d", audioCapturerInfo.bitWidth);
    MEDIA_INFO_LOG("record input argv[5]:%s.", argv[5]); /* 5: filePath */

    if (std::strlen(argv[0x5]) < PATH_MAX) {
        filePath = argv[0x5];
    } else {
        MEDIA_ERR_LOG("record input path too long");
        return -1;
    }
    return 0;
}

static void ConvertCaptureInfo(AudioCapturerInfo &dts, const SampleAudioCapturerInfo &src)
{
    dts.inputSource = src.inputSource;
    dts.audioFormat = src.audioFormat;
    dts.sampleRate = src.sampleRate;
    dts.bitWidth = src.bitWidth;
    dts.sessionID = src.sessionID;
    dts.channelCount = src.channelCount;
    dts.streamType = src.streamType;
}

static int32_t AudioCaptureSetInfo(SampleAudioCapturerInfo audioCapturerInfo)
{
    AudioCapturerInfo info = {};
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_CAPTURE_JS, MEDIA_CHR_JS_AUDIO_CAPTURE_START,
        MEDIA_CHR_JS_SET_CAPTURE_INFO, 0);
    ConvertCaptureInfo(info, audioCapturerInfo);
    if (g_audioCap->SetCapturerInfo(info) != 0) {
        MEDIA_ERR_LOG("record Can't SetCapturerInfo");
        return -1;
    }
    size_t frameCount = g_audioCap->GetFrameCount();
    if (frameCount == 0) {
        MEDIA_ERR_LOG("record Can't GetFrameCount");
        (void)g_audioCap->Release();
        return -1;
    }
    MEDIA_INFO_LOG("GetFrameCount:%d", frameCount);
    g_audioSourceProcessInput.framesize = frameCount * info.channelCount * info.bitWidth / BYTE_SIZE;
    g_audioSourceProcessInput.buffer = new uint8_t[g_audioSourceProcessInput.framesize];
    if (g_audioSourceProcessInput.buffer == nullptr) {
        MEDIA_ERR_LOG("buffer malloc fail");
        (void)g_audioCap->Release();
        return -1;
    }
    if (!g_audioCap->Start()) {
        MEDIA_ERR_LOG("record Can't Start");
        delete g_audioSourceProcessInput.buffer;
        g_audioSourceProcessInput.buffer = nullptr;
        (void)g_audioCap->Release();
        return -1;
    }
    return 0;
}

static int32_t ActivateAudioInterrupt(AudioSession &sessionId, AudioStreamType type)
{
    g_amInstance.Initialize();
    sessionId = g_amInstance.MakeSessionId();
    if (sessionId == AUDIO_SESSION_ID_NONE) {
        MEDIA_ERR_LOG("record session is none");
        return MEDIA_ERR;
    }
    MEDIA_INFO_LOG("sessionId:%d", sessionId);
    g_captureInterruptListener = std::make_shared<CaptureInterruptListener>();
    if (g_captureInterruptListener.get() == nullptr) {
        MEDIA_ERR_LOG("g_captureInterruptListener create failed!");
        return MEDIA_ERR;
    }
    g_capturerCtx.interrupt = { type, sessionId, g_captureInterruptListener };
    if (g_amInstance.ActivateAudioInterrupt(g_capturerCtx.interrupt) == INTERRUPT_FAILED) {
        MEDIA_ERR_LOG("record ActivateAudioInterrupt failed");
        return MEDIA_ERR;
    }
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_CAPTURE_JS, MEDIA_CHR_JS_AUDIO_CAPTURE_START,
        MEDIA_CHR_ACTIVE_AUDIO_INTERRPUT, 0);
    return MEDIA_OK;
}

static int32_t CheckInputParam(int32_t argc, const char **argv)
{
    MEDIA_UNUSED(argv);
    if (argc != 0x6) { // must be 6 parameters
        MEDIA_ERR_LOG("argc:%d is invalid, must be 6 parameters", argc);
        return -1;
    }

    return 0;
}

int MakeAudioDir(const char *dir)
{
    if (strlen(dir) >= PATH_STR_SIZE) {
        return -1;
    }
    char str[PATH_STR_SIZE];
    if (strncpy_s(str, PATH_STR_SIZE, dir, strlen(dir)) != 0) {
        MEDIA_ERR_LOG("copy path error");
        return -1;
    }
    int len = strlen(str);
    for (int i = 0; i < len; i++) {
        if (str[i] == '/') {
            str[i] = '\0';
            if (access(str, F_OK) != 0) {
                (void)mkdir(str, DIRECTORY_PERMISSION);
            }
            str[i] = '/';
        }
    }
    if (len > 0 && access(str, F_OK) != 0) {
        (void)mkdir(str, DIRECTORY_PERMISSION);
    }
    return 0;
}

static int32_t AudioStreamCaptureInit(const JSIValue *argv)
{
    int32_t ret = AudioStreamCaptureGetInfo(argv,
        g_audioSourceProcessInput.info);
    if (ret != 0) {
        return ret;
    }

    AudioSession sessionId = AUDIO_SESSION_ID_NONE;
    ret = ActivateAudioInterrupt(sessionId, AUDIO_STREAM_VOICE_RECORD);
    if (ret != 0) {
        return ret;
    }
    g_audioCap = std::make_shared<AudioCapturer>();
    if (g_audioCap.get() == nullptr) {
        MEDIA_ERR_LOG("audio capture is nullptr");
        (void)g_amInstance.DeactivateAudioInterrupt(g_capturerCtx.interrupt);
        return -1;
    }
    g_audioSourceProcessInput.info.sessionID = sessionId;
    g_audioSourceProcessInput.info.inputSource = AUDIO_MIC;
    g_audioSourceProcessInput.info.streamType = AUDIO_STREAM_VOICE_RECORD;
    ret = AudioCaptureSetInfo(g_audioSourceProcessInput.info);
    if (ret != 0) {
        g_audioCap.reset();
        (void)g_amInstance.DeactivateAudioInterrupt(g_capturerCtx.interrupt);
        FailCallBack(MEDIA_ERR, "set info failed!");
        return ret;
    }
    if (g_audioSourceProcessInput.recordOgg) {
        ret = OggMuxerCtxInit(g_audioSourceProcessInput.info);
        if (ret != MEDIA_OK) {
            MEDIA_ERR_LOG("OggCtx init failed");
            return ret;
        }
    }
    return ret;
}

static int32_t AudioCaptureInit(const char **argv)
{
    SampleAudioCapturerInfo audioCapturerInfo = {};
    std::string filePath = "";
    int32_t ret = AudioCaptureGetInfo(argv, audioCapturerInfo, filePath);
    if (ret != 0) {
        return ret;
    }

    AudioSession sessionId = AUDIO_SESSION_ID_NONE;
    ret = ActivateAudioInterrupt(sessionId, AUDIO_STREAM_VOICE_RECORD);
    if (ret != 0) {
        return ret;
    }
    g_audioCap = std::make_shared<AudioCapturer>();
    if (g_audioCap == nullptr || g_audioCap.get() == nullptr) {
        MEDIA_ERR_LOG("audio capture is nullptr");
        (void)g_amInstance.DeactivateAudioInterrupt(g_capturerCtx.interrupt);
        return -1;
    }

    ret = MakeAudioDir("/user/log/xiaodu");
    if (ret != 0) {
        MEDIA_ERR_LOG("dir path is invalid");
        return -1;
    }

    audioCapturerInfo.sessionID = sessionId;
    audioCapturerInfo.inputSource = AUDIO_MIC;
    audioCapturerInfo.streamType = AUDIO_STREAM_VOICE_RECORD;
    ret = AudioCaptureSetInfo(audioCapturerInfo);
    if (ret != 0) {
        g_audioCap.reset();
        (void)g_amInstance.DeactivateAudioInterrupt(g_capturerCtx.interrupt);
        return ret;
    }
    return ret;
}

int32_t AudioCaptureStartSample(int32_t argc, const char **argv)
{
    for (int32_t i = 0; i < argc; i++) {
        MEDIA_INFO_LOG("argv[%d]:%s", i, argv[i]);
    }
    int32_t ret = CheckInputParam(argc, argv);
    if (ret != 0) {
        return ret;
    }

    g_capturerCtx.needStop = false;
    g_interruptHintStop = false;
    ret = AudioCaptureInit(argv);
    if (ret != 0) {
        return ret;
    }


    /* create pcm stream task for transfer */
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_CAPTURE_JS, MEDIA_CHR_JS_AUDIO_CAPTURE_START, MEDIA_CHR_CREATE_PCM_THREAD, 0);
    ret = pcm_stream_create_task();
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_CAPTURE_JS, MEDIA_CHR_JS_AUDIO_CAPTURE_START, MEDIA_CHR_CREATE_CAPTURE_THREAD, 0);
    MediaThreadattr attr = { "AudioCaptureTask", 0x2000, THREAD_SCHED_INVALID, 0, false };
    g_capturerCtx.audioCaptureProcess = MediaThreadCreate(AudioCaptureProcess, nullptr, &attr);
    if (g_capturerCtx.audioCaptureProcess == nullptr) {
        MEDIA_ERR_LOG("thread create failed");
        return -1;
    }

    /* send record start */
    errcode_t res = msg_center_xiaodu_start_record(MSGCENTER_CMD_XIAODU, MSGCENTER_TYPE_ID_XIAODU_START_RECORD, NULL, 0);
    if (res != ERRCODE_SUCC) {
        MEDIA_ERR_LOG("send start fail.\r\n");
        return res;
    }
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_CAPTURE_JS, MEDIA_CHR_JS_AUDIO_CAPTURE_START, MEDIA_CHR_NOTIFY_PCM_THREAD, 0);
    return 0;
}

static int32_t AudioStreamCaptureStart(const JSIValue *args)
{
    g_capturerCtx.needStop = false;
    g_interruptHintStop = false;
    int32_t ret = AudioStreamCaptureInit(args);
    if (ret != 0) {
        return ret;
    }
    MediaThreadattr attr = { "AudioStreamCaptureProcess", 0x4000, THREAD_SCHED_INVALID, 0, false };
    g_capturerCtx.audioCaptureProcess = MediaThreadCreate(AudioStreamCaptureProcess, nullptr, &attr);
    if (g_capturerCtx.audioCaptureProcess == nullptr) {
        MEDIA_ERR_LOG("thread create failed");
        return -1;
    }
    return 0;
}

#ifdef __cplusplus
};

namespace OHOS {
namespace ACELite {

JSIValue AudioCapturerModule::Start(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    MEDIA_UNUSED(thisVal);
    MEDIA_UNUSED(args);
    MEDIA_UNUSED(argsSize);
    const char *params[PARAM_NUMBER] = { "", "opus", "16000", "1", "16", "/user/log/xiaodu/xiaodu.opus" };
    return JSI::CreateBoolean(AudioCaptureStartSample(PARAM_NUMBER, params));
}

JSIValue AudioCapturerModule::start(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    if (argsSize == 0) {
        const char *params[PARAM_NUMBER] = { "", "opus", "16000", "1", "16", "/user/log/xiaodu/xiaodu.opus" };
        return JSI::CreateBoolean(AudioCaptureStartSample(PARAM_NUMBER, params));
    }
    if (g_capturerCtx.capturerState == ON) {
        FailCallBack(MEDIA_OK, "Already start,no need to start!");
        return JSI::CreateBoolean(false);
    }
    if (args == nullptr || JSI::ValueIsUndefined(args[0])) {
        FailCallBack(MEDIA_ERR, "invalid parameter!");
        return JSI::CreateBoolean(false);
    }
    if (g_capturerCtx.listenerState == OFF) {
        InitResource();
    }
    if (AudioStreamCaptureStart(args) != MEDIA_OK) {
        FailCallBack(MEDIA_ERR, "Start AudioCapturer fail!");
        return JSI::CreateBoolean(false);
    }
    g_capturerCtx.capturerState = ON;
    return JSI::CreateBoolean(true);
}

JSIValue AudioCapturerModule::stop(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    if (g_capturerCtx.capturerState == OFF) {
        FailCallBack(MEDIA_ERR, "Already stop, no need to stop");
        return JSI::CreateBoolean(false);
    }
    MEDIA_UNUSED(thisVal);
    MEDIA_UNUSED(args);
    MEDIA_UNUSED(argsSize);
    MediaMutexLock(g_jsCaptureMutex);
    g_capturerCtx.needStop = true;
    MediaMutexUnLock(g_jsCaptureMutex);
    MediaThreadJoin(&g_capturerCtx.audioCaptureProcess);
    if (g_capturerCtx.listenerState == OFF) {
        DestroyResource();
    }
    if (g_audioSourceProcessInput.recordOgg) {
        OggMuxerCtxDeInit();
        g_audioSourceProcessInput.recordOgg = false;
    }
    g_capturerCtx.capturerState = OFF;
    return JSI::CreateBoolean(true);
}

JSIValue AudioCapturerModule::Stop(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    MEDIA_UNUSED(thisVal);
    MEDIA_UNUSED(args);
    MEDIA_UNUSED(argsSize);
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_CAPTURE_JS, MEDIA_CHR_JS_AUDIO_CAPTURE_STOP,
        MEDIA_CHR_SEND_STOP_SIGNAL, 0);
    g_capturerCtx.needStop = true;
    MediaThreadJoin(&g_capturerCtx.audioCaptureProcess);
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_CAPTURE_JS, MEDIA_CHR_JS_AUDIO_CAPTURE_STOP,
        MEDIA_CHR_WAIT_THREAD_END, 0);	
    return JSI::CreateBoolean(true);
}

JSIValue AudioCapturerModule::ListenerOn(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    if (g_capturerCtx.listenerState == ON) {
        FailCallBack(MEDIA_OK, "No need to turn on!");
        return JSI::CreateBoolean(false);
    }
    if (args == nullptr || argsSize == 0) {
        FailCallBack(MEDIA_OK, "invalid parameter!");
        return JSI::CreateBoolean(false);
    }
    if (!JSI::ValueIsObject(args[0])) {
        FailCallBack(MEDIA_OK, "invalid object!");
        return JSI::CreateBoolean(false);
    }
    if (g_capturerCtx.capturerState == OFF) {
        InitResource();
    }
    MediaMutexLock(g_jsCaptureMutex);
    g_jsCaptureParams.thisVal = JSI::AcquireValue(thisVal);
    g_jsCaptureParams.successCb = JSI::GetNamedProperty(args[0], PROP_SUCCESS);
    g_jsCaptureParams.failCb = JSI::GetNamedProperty(args[0], PROP_FAIL);
    g_capturerCtx.listenerState = ON;
    MediaMutexUnLock(g_jsCaptureMutex);
    return JSI::CreateBoolean(SUCCESS);
}

JSIValue AudioCapturerModule::ListenerOff(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    if (g_capturerCtx.listenerState == OFF) {
        FailCallBack(MEDIA_ERR, "Already off, no need to start!");
        return JSI::CreateBoolean(false);
    }
    MediaMutexLock(g_jsCaptureMutex);
    if (!JSI::ValueIsUndefined(g_jsCaptureParams.successCb)) {
        JSI::ReleaseValueList(g_jsCaptureParams.successCb);
        g_jsCaptureParams.successCb = JSI::CreateUndefined();
    }
    MediaMutexUnLock(g_jsCaptureMutex);
    if (g_capturerCtx.capturerState == OFF) {
        DestroyResource();
    }
    g_capturerCtx.listenerState = OFF;
    return JSI::CreateBoolean(SUCCESS);
}

JSIValue AudioCapturerModule::Cancel(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    MEDIA_UNUSED(thisVal);
    MEDIA_UNUSED(args);
    MEDIA_UNUSED(argsSize);
    msg_center_xiaodu_audio_pcm_clear_res();
    return JSI::CreateBoolean(0);
}

void AudioCapturerModule::ExceptionStop(void)
{
    if (!g_capturerCtx.needStop) {
        g_capturerCtx.needStop = true;
    }
}

void AudioCapturerModule::OnDestroy()
{
    if (g_capturerCtx.listenerState == OFF && g_capturerCtx.capturerState == OFF) {
        return;
    }
    MediaMutexLock(g_jsCaptureMutex);
    g_capturerCtx.needStop = true;
    MediaMutexUnLock(g_jsCaptureMutex);
    MediaThreadJoin(&g_capturerCtx.audioCaptureProcess);
    DestroyResource();
    g_capturerCtx.listenerState = OFF;
    g_capturerCtx.capturerState = OFF;
}

void AudioCapturerModule::OnTerminate()
{
    if (g_capturerCtx.listenerState == OFF && g_capturerCtx.capturerState == OFF) {
        return;
    }
    MediaMutexLock(g_jsCaptureMutex);
    g_capturerCtx.needStop = true;
    MediaMutexUnLock(g_jsCaptureMutex);
    MediaThreadJoin(&g_capturerCtx.audioCaptureProcess);
    DestroyResource();
    g_capturerCtx.listenerState = OFF;
    g_capturerCtx.capturerState = OFF;
}

void InitAudioCapturerModule(JSIValue exports)
{
    JSI::SetModuleAPI(exports, "Start", AudioCapturerModule::Start);
    JSI::SetModuleAPI(exports, "Stop", AudioCapturerModule::Stop);
    JSI::SetModuleAPI(exports, "cancel", AudioCapturerModule::Cancel);
    JSI::SetModuleAPI(exports, "start", AudioCapturerModule::start);
    JSI::SetModuleAPI(exports, "stop", AudioCapturerModule::stop);
    JSI::SetModuleAPI(exports, "on", AudioCapturerModule::ListenerOn);
    JSI::SetModuleAPI(exports, "off", AudioCapturerModule::ListenerOff);
    JSI::SetOnDestroy(exports, AudioCapturerModule::OnDestroy);
    JSI::SetOnTerminate(exports, AudioCapturerModule::OnTerminate);
}

}
}

#endif
