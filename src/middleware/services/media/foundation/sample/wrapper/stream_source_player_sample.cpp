/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: stream source sample wrapper
 * Author: Media Software Group
 * Create: 2026-01-27
 */

#include "stream_source_player_sample.h"

#include <climits>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include "source.h"
#include "player.h"
#include "format.h"
#include "securec.h"

#include "audio_manager.h"
#include "audio_base_type.h"

#include "media_log.h"
#include "media_thread_adapt.h"
#include "stream_file.h"

#ifdef SUPPORT_LWIP
#include "lwip/sockets.h"
#include "lwip/ip_addr.h"
#endif

#ifdef ENABLE_UIKIT
#include "common/graphic_startup.h"
#include "common/screen.h"
#include "common/task_manager.h"
#include "components/root_view.h"
#include "gfx_utils/graphic_log.h"
#include "graphic_config.h"
#include "hals/display_dev.h"
#include "graphic_service.h"
#include "components/root_view.h"
#include "components/ui_lite_surface_view.h"
#include "surface.h"
#endif
#include "media_mem.h"
#include "media_chr_log.h"
#include "stream_source_player_service.h"

#ifdef __cplusplus
extern "C" {
#endif

using namespace OHOS::Media;
using namespace OHOS;

#define MEDIA_FILE_PATH_MAX 260
#define MEDIA_CMD_LEN_MAX 128
#define MEDIA_UNUSED(x) ((void)(x))
#define INTERVAL_AUDIO_FRAME_NUM 3

const static int32_t WAIT_SURFACE_INIT_SUCCESS_US = 100000;
const static int32_t WAIT_SURFACE_INIT_SUCCESS_TIME = 1000000;
const static int32_t WAIT_SURFACE_DEINIT_SUCCESS_US = 100000;
const static int32_t WAIT_SURFACE_DEINIT_SUCCESS_TIME = 1000000;
const static int16_t WIDTH = 454;
const static int16_t HEIGHT = 454;
const int32_t STREAM_SOURCE_FORMAT_MAX_LEN = 10;

const uint32_t STREAM_FLAG_EOS = 4;
const uint32_t STREAM_FLAG_AUDIO_FRAME = 64;
const uint32_t STREAM_FLAG_VIDEO_FRAME = 128;
const uint32_t STREAM_FLAG_VIDEO_KEY_FRAME = 256;
const int32_t STREAM_PROCESS_SLEEP_TIME_US = 33000;
const uint32_t NUMBER_BASE = 10;
const char END_SIGN = '\0';
const uint32_t MAX_CONTINOUS_BUFFER_FAIL_COUNT = 100;

#define MEDIA_PATH_KEY_LEN 50
#define MEDIA_PATH_MAX 260
#define GET_BUFFER_RETRY_FLAG (-7)

#define ARRAY_SIZE(array) (sizeof((array)) / sizeof((array)[0]))
#define MEDIA_BYTE_ALIGN(value, align) (((value) + (align) - 1) & (~((align) - 1)))
#define MEDIA_PATH_MAX 260
#define SAMPLE_RETURN_VAL_IF_NULL(condition)                                           \
    do {                                                                               \
        if ((condition) == nullptr) {                                                  \
            MEDIA_ERR_LOG("is nullptr");    \
            return -1;                                                         \
        }                                                                              \
    } while (0)
#define CHK_FAILED_RETURN(value, target, ret, print) \
do { \
    if ((value) != (target)) { \
        MEDIA_ERR_LOG("%s, %#x != %d ", ((print) == NULL) ? "" : (print), (value), (target)); \
        return (ret); \
    } \
} while (0)
#define CHK_NULL_RETURN(ptr, ret, printfString) \
do { \
    if ((ptr) == NULL) { \
        MEDIA_ERR_LOG("%s :ret:%d", ((printfString) == NULL) ? "" : (printfString), ret); \
        return (ret); \
    } \
} while (0)

typedef struct {
    char audioFormatType[STREAM_SOURCE_FORMAT_MAX_LEN];
    char videoFormatType[STREAM_SOURCE_FORMAT_MAX_LEN];
    char audioFilePath[MEDIA_PATH_MAX];
    char videoFilePath[MEDIA_PATH_MAX];
    uint32_t  sampleRate;
    uint32_t  bitWith;
    uint32_t  channel;
    uint32_t width;
    uint32_t height;
    uint32_t frameRate;
} StreamSourceFormat;

typedef struct StreamSourceContex {
    int32_t readLen;
    StreamFileHandle audioSource = nullptr;
    StreamFileHandle videoSource = nullptr;
    int32_t readFlag = 0;
    bool isReadAudioEos = false;
    bool isReadVideoEos = false;
    bool isSupportStreamSourceAudio;
    bool isSupportStreamSourceVideo;
    bool isRemoteAddr = false;
#ifdef SUPPORT_LWIP
    int32_t clientFd;
    int32_t serverFd;
#endif
} StreamSourceContex;

typedef struct {
#if defined(ENABLE_UIKIT)
    Surface *surface;
    UILiteSurfaceView *surfaceView;
    bool surfaceInited;
#endif
    char filePath[MEDIA_FILE_PATH_MAX];
    StreamSourceFormat format;
    SourceType sourceType;
    std::shared_ptr<StreamSourcePlayerService> ssPlayerSrv;

    MediaMutexHandle mutex;
    bool started;
    MediaThreadIdHandle streamSourceProcess;
    StreamSourceContex streamSourceCtx;
    std::shared_ptr<StreamSourcePlayerCallback> playerCallback;
} StreamSourceServiceContext;

static MediaMutexHandle g_ssServiceMutex = nullptr;
static StreamSourceServiceContext *g_ssServiceCtx = nullptr;

static AudioManager& g_amInstance = AudioManager::GetInstance();

static int32_t StartStreamSource(int32_t argc, const char *argv[]);
static int32_t PauseStreamSource(int32_t argc, const char *argv[]);
static int32_t ResumeStreamSource(int32_t argc, const char *argv[]);
static int32_t StopStreamSource(int32_t argc, const char *argv[]);

typedef struct {
    char cmd[MEDIA_CMD_LEN_MAX];
    int32_t (*func)(int32_t argc, const char *argv[]);
} FuncsMap;

static FuncsMap g_videoPlayFuncs[] = {
    { "start", StartStreamSource },
    { "pause", PauseStreamSource },
    { "resume", ResumeStreamSource },
    { "stop", StopStreamSource },
};

class StreamSourceCallbackImpl : public StreamSourcePlayerCallback {
public:
    StreamSourceCallbackImpl() = default;
    ~StreamSourceCallbackImpl() override {}
    void OnInterrupt(int32_t type, int32_t hint) override
    {
        MEDIA_ERR_LOG("OnInterrupt recv is type[%d], hint[%d].", type, hint);
        if (type == INTERRUPT_TYPE_BEGIN && hint == INTERRUPT_HINT_PAUSE) {
            MEDIA_INFO_LOG("recv INTERRUPT_HINT_PAUSE !");
        } else if (type == INTERRUPT_TYPE_END && hint == INTERRUPT_HINT_RESUME) {
            MEDIA_INFO_LOG("recv INTERRUPT_HINT_RESUME !");
        } else if (type == INTERRUPT_TYPE_BEGIN && hint == INTERRUPT_HINT_STOP) {
            MEDIA_INFO_LOG("recv INTERRUPT_HINT_STOP !");
        }
    }
    void OnError(int32_t errorType, int32_t errorCode) override
    {
        MediaMutexLock(g_ssServiceCtx->mutex);
        g_ssServiceCtx->started = false;
        MediaMutexUnLock(g_ssServiceCtx->mutex);
        MEDIA_ERR_LOG(" errorType[%d]. errorCode[%d]", errorType, errorCode);
    }

    void OnInfo(int32_t type, int32_t extra) override
    {
        MEDIA_ERR_LOG("type[%d]. extra[%d]", type, extra);
    }

    void OnPlaybackComplete() override
    {
        MEDIA_INFO_LOG("recv OnPlaybackComplete !");
    }
};

static int32_t InitStreamSourceServiceContext(void)
{
    MediaInitStaticMutexLock(&g_ssServiceMutex);
    MediaMutexLock(g_ssServiceMutex);
    if (g_ssServiceCtx != nullptr) {
        MediaMutexUnLock(g_ssServiceMutex);
        return 0;
    }
    g_ssServiceCtx = new StreamSourceServiceContext();
    if (g_ssServiceCtx == nullptr) {
        MEDIA_ERR_LOG("malloc failed");
        MediaMutexUnLock(g_ssServiceMutex);
        return -1;
    }
    MediaMutexUnLock(g_ssServiceMutex);
    MEDIA_INFO_LOG("sucess");
    return 0;
}

static void DestoryStreamSourceServiceContext()
{
    if (g_ssServiceCtx == nullptr) {
        MEDIA_ERR_LOG("videoCtx is NULL!");
        return;
    }
    delete g_ssServiceCtx;
    g_ssServiceCtx = nullptr;
    MEDIA_INFO_LOG("success!");
}

static int32_t ReadBufferData(StreamFileHandle handle, IdleBuffer *buffer)
{
    int32_t ret = g_ssServiceCtx->ssPlayerSrv->GetAvailableBuffer(buffer);
    if (ret != 0) {
        return GET_BUFFER_RETRY_FLAG;
    }
    uint8_t *data = g_ssServiceCtx->ssPlayerSrv->GetBufferAddress(buffer->idx);
    if (data == nullptr) {
        MEDIA_ERR_LOG("get buffer null");
        return -1;
    }
    uint32_t frameLen = 0;
    uint8_t keyFrame = 0;
    ret = ReadStreamData(handle, &data, buffer->size, &frameLen, &keyFrame);
    if (ret != 0) {
        MEDIA_ERR_LOG("get stream data failed!");
        return ret;
    }
    if (keyFrame != 0) {
        buffer->flag = STREAM_FLAG_VIDEO_KEY_FRAME;
    }
    return frameLen;
}

static int32_t OpenStreamSourceFile(StreamFileHandle *audioSource,
    StreamFileHandle *videoSource, int32_t *readFlag)
{
    int32_t ret = 0;
    if (g_ssServiceCtx->streamSourceCtx.isSupportStreamSourceAudio) {
        ret = OpenStreamFile(audioSource, g_ssServiceCtx->format.audioFormatType, g_ssServiceCtx->format.audioFilePath);
        if (ret != 0) {
            return MEDIA_ERR;
        }
        MEDIA_INFO_LOG("audio file:%s", g_ssServiceCtx->format.audioFilePath);
        *readFlag = 1;
    }
    if (g_ssServiceCtx->streamSourceCtx.isSupportStreamSourceVideo) {
        ret = OpenStreamFile(videoSource, g_ssServiceCtx->format.videoFormatType, g_ssServiceCtx->format.videoFilePath);
        if (ret != 0) {
            if (g_ssServiceCtx->streamSourceCtx.isSupportStreamSourceAudio) {
                (void)CloseStreamFile(audioSource);
            }
            return MEDIA_ERR;
        }
        MEDIA_INFO_LOG("video file:%s", g_ssServiceCtx->format.videoFilePath);
        *readFlag = INTERVAL_AUDIO_FRAME_NUM;
    }
    return MEDIA_OK;
}

static void CloseStreamSourceFile(StreamFileHandle *audioSource,
    StreamFileHandle *videoSource)
{
    if (g_ssServiceCtx->streamSourceCtx.isSupportStreamSourceAudio) {
        CloseStreamFile(audioSource);
        MEDIA_INFO_LOG("close audio file:%s", g_ssServiceCtx->format.audioFilePath);
    }
    if (g_ssServiceCtx->streamSourceCtx.isSupportStreamSourceVideo) {
        CloseStreamFile(videoSource);
        MEDIA_INFO_LOG("close video file:%s", g_ssServiceCtx->format.videoFilePath);
    }
}

static void StreamSourceProcessEos(int32_t *readFlag, const IdleBuffer &buffer,
    bool isReadAudioEos, bool isReadVideoEos)
{
    if (isReadAudioEos) {
        g_ssServiceCtx->ssPlayerSrv->QueueBuffer(buffer.idx, buffer.offset, 0, 0, STREAM_FLAG_EOS);
        *readFlag = INTERVAL_AUDIO_FRAME_NUM;
    }
    if (isReadVideoEos) {
        g_ssServiceCtx->ssPlayerSrv->QueueBuffer(buffer.idx, buffer.offset, 0, 0, STREAM_FLAG_EOS);
        *readFlag = 0x1;
    }
}

static void ProcessAudioFrame(StreamSourceContex *ctx, IdleBuffer &buffer)
{
    buffer.flag = STREAM_FLAG_AUDIO_FRAME;
    ctx->readLen = ReadBufferData(ctx->audioSource, &buffer);
    if (ctx->isSupportStreamSourceVideo && ctx->readLen > 0) {
        ctx->readFlag++;
    }
    if (ctx->readLen == MEDIA_READ_EOS) {
        ctx->isReadAudioEos = true;
        StreamSourceProcessEos(&ctx->readFlag, buffer, ctx->isReadAudioEos, false);
    }
}

static void ProcessVideoFrame(StreamSourceContex *ctx, IdleBuffer &buffer)
{
    buffer.flag = STREAM_FLAG_VIDEO_FRAME;
    ctx->readLen = ReadBufferData(ctx->videoSource, &buffer);
    if (ctx->isSupportStreamSourceAudio && ctx->readLen > 0) {
        ctx->readFlag = 0x1;
    }
    if (ctx->readLen == MEDIA_READ_EOS) {
        ctx->isReadVideoEos = true;
        StreamSourceProcessEos(&ctx->readFlag, buffer, false, ctx->isReadVideoEos);
    }
}

#ifdef SUPPORT_LWIP

#define SERVER_PORT 80
#define MAX_CLIENT_NUM 5
// 1400 is experience value provided by VDEC sample
#define BUFFER_SIZE 1400

static int32_t ConnectRemoteClient(int32_t *serverFd, int32_t *clientFd)
{
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen;

    *serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (*serverFd < 0) {
        MEDIA_ERR_LOG("Failed to create socket");
        return 1;
    }
    g_ssServiceCtx->streamSourceCtx.serverFd = *serverFd;
    int32_t opt = 1;
    setsockopt(*serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    (void)memset_s(&serverAddr, sizeof(serverAddr), 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(SERVER_PORT);

    void *serverConvertPtr = static_cast<void *>(&serverAddr);
    if (bind(*serverFd, static_cast<struct sockaddr*>(serverConvertPtr), sizeof(serverAddr)) < 0) {
        MEDIA_ERR_LOG("Bind faile");
        closesocket(*serverFd);
        return 1;
    }

    if (listen(*serverFd, MAX_CLIENT_NUM) < 0) {
        MEDIA_ERR_LOG("Listen failed");
        closesocket(*serverFd);
        return 1;
    }

    MEDIA_INFO_LOG("TCP Server started on port %d", SERVER_PORT);
    
    clientAddrLen = sizeof(clientAddr);
    void *clientConvertPtr = static_cast<void *>(&clientAddr);
    *clientFd = accept(*serverFd, static_cast<struct sockaddr*>(clientConvertPtr), &clientAddrLen);
    if (*clientFd < 0) {
        MEDIA_ERR_LOG("Accept failed");
        closesocket(*serverFd);
        return 1;
    }
    // "recv" is blocked when disconnected, fd is recorded for invoking "closesocket" to proactively disconnect and exit.
    g_ssServiceCtx->streamSourceCtx.clientFd = *clientFd;
    g_ssServiceCtx->streamSourceCtx.serverFd = *serverFd;
    MEDIA_INFO_LOG("Client connected");
    return 0;
}

static int32_t ReceiveClientBuffer(uint8_t *recvBuffer, int32_t clientFd, size_t bufferSize)
{
    int32_t bytesReceived = recv(clientFd, recvBuffer, bufferSize, 0);
    if (bytesReceived <= 0) {
        MEDIA_ERR_LOG("Client disconnected or received error, bytesReceived:%d", bytesReceived);
        return MEDIA_ERR;
    }
    return bytesReceived;
}

static int32_t StreamRemoteSourcePrepare(int32_t *serverFd, int32_t *clientFd)
{
    MEDIA_INFO_LOG("Start StreamProc Prepare");
    if (ConnectRemoteClient(serverFd, clientFd) != 0) {
        return MEDIA_ERR;
    }
    if (g_ssServiceCtx->ssPlayerSrv->Start() != 0) {
        MEDIA_ERR_LOG("Start failed");
        return MEDIA_ERR;
    }
    MEDIA_INFO_LOG("Start StreamProc");
    return MEDIA_OK;
}

static void StreamRemoteSourceDeinit(IdleBuffer &buffer, int32_t clientFd, int32_t serverFd)
{
    MEDIA_INFO_LOG("StreamProc exit stop player");
    g_ssServiceCtx->ssPlayerSrv->QueueBuffer(buffer.idx, buffer.offset, 0, 0, STREAM_FLAG_EOS);
    g_ssServiceCtx->ssPlayerSrv->Stop();
    closesocket(serverFd);
    closesocket(clientFd);
    g_ssServiceCtx->streamSourceCtx.clientFd = -1;
    g_ssServiceCtx->streamSourceCtx.serverFd = -1;
    MEDIA_INFO_LOG("StreamProc exit close socket");
}

static void *StreamRemoteSourceProcess(void *arg)
{
    int32_t serverFd;
    int32_t clientFd;
    uint32_t bufferFailedCnt = 0;
    int32_t totalBytes = 0;
    IdleBuffer buffer = {};
    int32_t ret = StreamRemoteSourcePrepare(&serverFd, &clientFd);
    if (ret != 0) {
        goto ERROR;
    }
    MEDIA_INFO_LOG("Start StreamProc");
    buffer.flag = STREAM_FLAG_VIDEO_FRAME;
    while (true) {
        MediaMutexLock(g_ssServiceCtx->mutex);
        if (!g_ssServiceCtx->started) {
            MediaMutexUnLock(g_ssServiceCtx->mutex);
            break;
        }
        MediaMutexUnLock(g_ssServiceCtx->mutex);
        ret = g_ssServiceCtx->ssPlayerSrv->GetAvailableBuffer(&buffer);
        if (ret != 0) {
            if (bufferFailedCnt++ < MAX_CONTINOUS_BUFFER_FAIL_COUNT) {
                usleep(STREAM_PROCESS_SLEEP_TIME_US);
                continue;
            }
            MEDIA_ERR_LOG("get available buffer failed");
            goto ERROR;
        }
        uint8_t *recvBuffer = g_ssServiceCtx->ssPlayerSrv->GetBufferAddress(buffer.idx);
        if (recvBuffer == nullptr) {
            MEDIA_ERR_LOG("get buffer null");
            goto ERROR;
        }
        int32_t bytesReceived = ReceiveClientBuffer(recvBuffer, clientFd, BUFFER_SIZE);
        if (bytesReceived <= 0) {
            MEDIA_ERR_LOG("ReceiveClientBuffer failed");
            goto ERROR;
        }
        g_ssServiceCtx->ssPlayerSrv->QueueBuffer(buffer.idx, buffer.offset, bytesReceived, 0, buffer.flag);
        bufferFailedCnt = 0;
        totalBytes += bytesReceived;
        MEDIA_INFO_LOG("Received %d bytes total: %d bytes", bytesReceived, totalBytes);
        usleep(STREAM_PROCESS_SLEEP_TIME_US);
    }
ERROR:
    StreamRemoteSourceDeinit(buffer, clientFd, serverFd);
    return nullptr;
}
#endif

static void *StreamSourceProcess(void *arg)
{
    StreamSourceContex *ctx = &g_ssServiceCtx->streamSourceCtx;
    int32_t ret = OpenStreamSourceFile(&ctx->audioSource, &ctx->videoSource, &ctx->readFlag);
    CHK_FAILED_RETURN(ret, 0, nullptr, "Open file failed!");
    MEDIA_INFO_LOG("Start StreamProc");
    while (true) {
        MediaMutexLock(g_ssServiceCtx->mutex);
        if (!g_ssServiceCtx->started  || ctx->isReadVideoEos || ctx->isReadAudioEos) {
            MediaMutexUnLock(g_ssServiceCtx->mutex);
            break;
        }
        MediaMutexUnLock(g_ssServiceCtx->mutex);
        IdleBuffer buffer = {};
        if (ctx->readFlag % INTERVAL_AUDIO_FRAME_NUM != 0 && ctx->isSupportStreamSourceAudio &&
            !ctx->isReadAudioEos) {
            ProcessAudioFrame(ctx, buffer);
        } else if (ctx->readFlag % INTERVAL_AUDIO_FRAME_NUM == 0 && ctx->isSupportStreamSourceVideo &&
            !ctx->isReadVideoEos) {
            ProcessVideoFrame(ctx, buffer);
        }
        if (ctx->readLen == GET_BUFFER_RETRY_FLAG || ctx->isReadAudioEos || ctx->isReadVideoEos) {
            usleep(STREAM_PROCESS_SLEEP_TIME_US);
            continue;
        }
        if (ctx->readLen > 0) {
            g_ssServiceCtx->ssPlayerSrv->QueueBuffer(buffer.idx, buffer.offset, (size_t)ctx->readLen, 0, buffer.flag);
        }
        usleep(STREAM_PROCESS_SLEEP_TIME_US);
    }
    CloseStreamSourceFile(&ctx->audioSource, &ctx->videoSource);
    g_ssServiceCtx->ssPlayerSrv->Stop();
    MEDIA_INFO_LOG("StreamProc exit");
    return nullptr;
}

static void PlayerInitAudioStreamSource(Format &formats)
{
    if (strncasecmp(g_ssServiceCtx->format.audioFormatType, "aac", strlen("aac")) == 0) {
        formats.PutStringValue(AUDIO_CODEC_MIME, MIME_AUDIO_AAC);
        MEDIA_INFO_LOG("set aac");
    } else if (strncasecmp(g_ssServiceCtx->format.audioFormatType, "mp3", strlen("mp3")) == 0) {
        formats.PutStringValue(AUDIO_CODEC_MIME, MIME_AUDIO_MP3);
        MEDIA_INFO_LOG("set mp3");
    } else {
        MEDIA_ERR_LOG("audio format:%s not support", g_ssServiceCtx->format.audioFormatType);
        return;
    }
    if (g_ssServiceCtx->format.sampleRate > 0) {
        formats.PutIntValue(SAMPLERATE_MIME, g_ssServiceCtx->format.sampleRate);
    }
    if (g_ssServiceCtx->format.channel > 0) {
        formats.PutIntValue(CHANNEL_MIME, g_ssServiceCtx->format.channel);
    }
}

static void PlayerInitVideoStreamSource(Format &formats)
{
    if (strncasecmp(g_ssServiceCtx->format.videoFormatType, "h264", strlen("h264")) == 0) {
        formats.PutStringValue(VIDEO_CODEC_MIME, MIME_VIDEO_H264);
        MEDIA_INFO_LOG("set h264");
    } else if (strncasecmp(g_ssServiceCtx->format.videoFormatType, "jpeg", strlen("jpeg")) == 0) {
        formats.PutStringValue(VIDEO_CODEC_MIME, MIME_VIDEO_JPEG);
        MEDIA_INFO_LOG("set jpeg");
    } else {
        MEDIA_ERR_LOG("video format:%s not support", g_ssServiceCtx->format.videoFormatType);
        return;
    }
    if (g_ssServiceCtx->format.width > 0) {
        formats.PutIntValue(MIME_VIDEO_WIDTH, g_ssServiceCtx->format.width);
    }
    if (g_ssServiceCtx->format.height > 0) {
        formats.PutIntValue(MIME_VIDEO_HEIGHT, g_ssServiceCtx->format.height);
    }
    if (g_ssServiceCtx->format.frameRate > 0) {
        formats.PutIntValue(MIME_VIDEO_FREAMRATE, g_ssServiceCtx->format.frameRate);
    }
}

static int32_t PlayerSetStreamSource()
{
    if (g_ssServiceCtx->sourceType != SourceType::SOURCE_TYPE_STREAM) {
        MEDIA_ERR_LOG("sourcetype not SOURCE_TYPE_STREAM:%d", g_ssServiceCtx->sourceType);
        return MEDIA_ERR;
    }
    Format formats;
    int32_t ret = 0;
    if (strlen(g_ssServiceCtx->format.audioFormatType) != 0) {
        MEDIA_INFO_LOG("audioFormatType2:%s!", g_ssServiceCtx->format.audioFormatType);
        PlayerInitAudioStreamSource(formats);
    }
    if (strlen(g_ssServiceCtx->format.videoFormatType) != 0) {
        MEDIA_INFO_LOG("video FormatType2:%s!", g_ssServiceCtx->format.videoFormatType);
        PlayerInitVideoStreamSource(formats);
    }
    ret = g_ssServiceCtx->ssPlayerSrv->SetStreamSourceInfo(formats);
    if (ret != 0) {
        MEDIA_ERR_LOG("SetStreamSourceInfo failed!");
        return ret;
    }
    MEDIA_CHR_LOG(MT_MEDIA_CHR_VIDEO_PLAYER_SAMPLE, MEDIA_CHR_CREATE_PLAYER_THREAD,
        MEDIA_CHR_SET_SOURCE, MEDIA_CHR_SET_STREAM_SOURCE);
    return ret;
}

#ifdef ENABLE_UIKIT
static void SurfaceInitEvent(void)
{
    MEDIA_INFO_LOG("in");
    // 1. init surface
    RootView::GetInstance()->SetWidth(Screen::GetInstance().GetWidth());
    RootView::GetInstance()->SetHeight(Screen::GetInstance().GetHeight());
    RootView::GetInstance()->SetPosition(0, 0);
    RootView::GetInstance()->SetStyle(STYLE_BACKGROUND_OPA, 0);

    // 2. create surface view
    UILiteSurfaceView *surfaceView = new UILiteSurfaceView();
    if (surfaceView == nullptr) {
        MEDIA_ERR_LOG("surfaceView is nullptr");
        return;
    }
    surfaceView->SetVideoResizeMode(OHOS::UILiteSurfaceView::VideoResizeMode::CONTAIN);
    surfaceView->SetViewId("sur");
    surfaceView->SetPosition(0, 0, WIDTH, HEIGHT);
    surfaceView->SetTouchable(true);
    surfaceView->SetIntercept(true);

    RootView::GetInstance()->Add(surfaceView);

    Surface *surface = surfaceView->GetSurface();

    surface->SetFormat(PIXEL_FMT_YCBCR_420_SP);

    g_ssServiceCtx->surface = surface;
    g_ssServiceCtx->surfaceView = surfaceView;
    g_ssServiceCtx->surfaceInited = true;

    MEDIA_INFO_LOG("success");
}

static void SurfaceInvalidateEvent(void)
{
    MEDIA_INFO_LOG("in");
    RootView::GetInstance()->Invalidate();
    MEDIA_INFO_LOG("success");
}

static void SurfaceDeinitEvent(void)
{
    MEDIA_INFO_LOG("in");
    RootView::GetInstance()->Remove(g_ssServiceCtx->surfaceView);
    // Flush the full screen to avoid partial black screen caused by partial refresh.
    RootView::GetInstance()->Invalidate();

    if (g_ssServiceCtx->surfaceView != nullptr) {
        delete g_ssServiceCtx->surfaceView;
        g_ssServiceCtx->surfaceView = nullptr;
    }

    g_ssServiceCtx->surfaceInited = false;
    MEDIA_INFO_LOG("success");
}
#endif

static void SurfaceDeinitAndReclaimResource(void)
{
    MEDIA_INFO_LOG("in");
#ifdef ENABLE_UIKIT
    // 1. post surface deinit event to graphic service thread
    GraphicService::GetInstance()->PostGraphicEvent([] {
        MEDIA_INFO_LOG("post deinit in");
        SurfaceDeinitEvent();
        MEDIA_INFO_LOG("post deinit out");
    });

    // 2. loop sleep 100ms for surface deinit complete
    int32_t waitTime = 0;
    while (g_ssServiceCtx->surfaceInited && waitTime < WAIT_SURFACE_DEINIT_SUCCESS_TIME) {
        usleep(WAIT_SURFACE_DEINIT_SUCCESS_US);
        waitTime += WAIT_SURFACE_DEINIT_SUCCESS_US;
    }
    if (waitTime >= WAIT_SURFACE_DEINIT_SUCCESS_TIME) {
        MEDIA_ERR_LOG("wait SurfaceDeinitEvent time exceeds 1s.");
    }
#endif
    MEDIA_INFO_LOG("exit");
}

static void StreamSourceStopAndExit()
{
    g_ssServiceCtx->ssPlayerSrv->Stop();
    SurfaceDeinitAndReclaimResource();
    MEDIA_INFO_LOG("exit");
}

static void DeinitPlayerResources()
{
    if (g_ssServiceCtx->ssPlayerSrv != nullptr) {
        g_ssServiceCtx->ssPlayerSrv.reset();
    }
    if (g_ssServiceCtx->playerCallback != nullptr) {
        g_ssServiceCtx->playerCallback.reset();
    }
    (void)MediaMutexDestroy(&g_ssServiceCtx->mutex);
    MEDIA_INFO_LOG("exit");
}

static bool InitPlayerResources()
{
    g_ssServiceCtx->mutex = MediaMutexCreate(nullptr);
    if (g_ssServiceCtx->mutex == nullptr) {
        MEDIA_ERR_LOG("create mutex failed");
        return false;
    }
    g_ssServiceCtx->ssPlayerSrv = std::make_shared<StreamSourcePlayerService>();
    if (g_ssServiceCtx->ssPlayerSrv.get() == nullptr) {
        MEDIA_ERR_LOG("ssPlayerSrv is nullptr");
        return false;
    }
    g_ssServiceCtx->playerCallback = std::make_shared<StreamSourceCallbackImpl>();
    if (g_ssServiceCtx->playerCallback == nullptr) {
        return false;
    }
    g_ssServiceCtx->ssPlayerSrv->SetPlayerCallback(g_ssServiceCtx->playerCallback);
    MEDIA_INFO_LOG("sucess");
    return true;
}

static int32_t StartStreamSourceProcess()
{
    g_ssServiceCtx->started = true;
    MediaThreadattr attr = {"StreamProc", 0x2000, THREAD_SCHED_INVALID, 80, false};
    if (g_ssServiceCtx->streamSourceCtx.isRemoteAddr) {
#ifdef SUPPORT_LWIP
        g_ssServiceCtx->streamSourceCtx.clientFd = -1;
        g_ssServiceCtx->streamSourceCtx.serverFd = -1;
        g_ssServiceCtx->streamSourceProcess = MediaThreadCreate(StreamRemoteSourceProcess, nullptr, &attr);
#endif
    } else {
        g_ssServiceCtx->streamSourceProcess = MediaThreadCreate(StreamSourceProcess, nullptr, &attr);
    }
    
    if (g_ssServiceCtx->streamSourceProcess == nullptr) {
        MEDIA_ERR_LOG("thread create failed");
        return MEDIA_ERR;
    }
    MEDIA_INFO_LOG("sucess");
    return MEDIA_OK;
}

static void StopStreamSourceProcess()
{
    MEDIA_INFO_LOG("in");
    MediaMutexLock(g_ssServiceCtx->mutex);
    g_ssServiceCtx->started = false;
    MediaMutexUnLock(g_ssServiceCtx->mutex);
    if (g_ssServiceCtx->streamSourceProcess != nullptr) {
#ifdef SUPPORT_LWIP
        if (g_ssServiceCtx->streamSourceCtx.isRemoteAddr &&
            (g_ssServiceCtx->streamSourceCtx.clientFd != -1 || g_ssServiceCtx->streamSourceCtx.serverFd != -1)) {
            closesocket(g_ssServiceCtx->streamSourceCtx.serverFd);
            closesocket(g_ssServiceCtx->streamSourceCtx.clientFd);
            g_ssServiceCtx->streamSourceCtx.clientFd = -1;
            g_ssServiceCtx->streamSourceCtx.serverFd = -1;
        }
#endif
        MediaThreadJoin(&g_ssServiceCtx->streamSourceProcess);
    }
    MEDIA_INFO_LOG("sucess");
}

static int32_t StreamSourcePlay()
{
    bool success = InitPlayerResources();
    if (!success) {
        MEDIA_ERR_LOG("init player resources failed");
        return -1;
    }
    int32_t ret = PlayerSetStreamSource();
    if (ret != 0) {
        return MEDIA_ERR;
    }
    ret = g_ssServiceCtx->ssPlayerSrv->SetAudioStreamType(AUDIO_STREAM_FITNESS_VIDEO);
    if (ret != 0) {
        MEDIA_ERR_LOG("SetAudioStreamType failed");
        return MEDIA_ERR;
    }
#if defined(ENABLE_UIKIT)
    ret = g_ssServiceCtx->ssPlayerSrv->SetVideoSurface(g_ssServiceCtx->surface);
#endif
    if (ret != 0) {
        MEDIA_ERR_LOG("SetVideoSurface failed");
        return MEDIA_ERR;
    }
    if (!g_ssServiceCtx->streamSourceCtx.isRemoteAddr) {
        ret = g_ssServiceCtx->ssPlayerSrv->Start();
        if (ret != 0) {
            MEDIA_ERR_LOG("Start failed");
            return MEDIA_ERR;
        }
    }

    ret = StartStreamSourceProcess();
    if (ret != 0) {
        return MEDIA_ERR;
    }
    MEDIA_INFO_LOG("sucess");
    return ret;
}

static int32_t InitAudioStreamSourceInfo(int32_t argc, const char **argv)
{
    if (argc < 0x6) {
        MEDIA_ERR_LOG("argc:%d error.", argc);
        return -1;
    }
    if (memcpy_s(g_ssServiceCtx->format.audioFormatType, STREAM_SOURCE_FORMAT_MAX_LEN,
        argv[0x2], strlen(argv[0x2]) + 1) != EOK) {
        MEDIA_ERR_LOG("memcpy_s failed");
        return -1;
    }
    MEDIA_INFO_LOG("audioFormatType:%s!", g_ssServiceCtx->format.audioFormatType);
    g_ssServiceCtx->format.sampleRate = strtoul(argv[0x3], NULL, 0xA);
    g_ssServiceCtx->format.channel = strtoul(argv[0x4], NULL, 0xA);
    if (memcpy_s(g_ssServiceCtx->format.audioFilePath, MEDIA_PATH_MAX,
        argv[0x5], strlen(argv[0x5]) + 1) != EOK) {
        MEDIA_ERR_LOG("memcpy_s failed");
        return -1;
    }
    g_ssServiceCtx->streamSourceCtx.isSupportStreamSourceAudio = true;
    return 0;
}

static int32_t InitVideoStreamSourceInfo(int32_t argc, const char **argv)
{
    if (argc < 0x6) {
        MEDIA_ERR_LOG("argc:%d error.", argc);
        return -1;
    }
    if (memcpy_s(g_ssServiceCtx->format.videoFormatType, STREAM_SOURCE_FORMAT_MAX_LEN,
        argv[0x2], strlen(argv[0x2]) + 1) != EOK) {
        MEDIA_ERR_LOG("memcpy_s failed");
        return -1;
    }
    MEDIA_INFO_LOG("videoFormatType:%s!", g_ssServiceCtx->format.videoFormatType);
    g_ssServiceCtx->format.width = strtoul(argv[0x3], NULL, 0x0A);
    g_ssServiceCtx->format.height = strtoul(argv[0x4], NULL, 0x0A);
    g_ssServiceCtx->format.frameRate = strtoul(argv[0x5], NULL, 0x0A);
    if (argc == 0x6) {
        g_ssServiceCtx->streamSourceCtx.isRemoteAddr = true;
        MEDIA_INFO_LOG("remote stream source");
    } else {
        if (memcpy_s(g_ssServiceCtx->format.videoFilePath, MEDIA_PATH_MAX, argv[0x6], strlen(argv[0x6]) + 1) != EOK) {
            MEDIA_ERR_LOG("memcpy_s failed");
            return -1;
        }
        g_ssServiceCtx->streamSourceCtx.isRemoteAddr = false;
        MEDIA_INFO_LOG("local file stream source");
    }

    g_ssServiceCtx->streamSourceCtx.isSupportStreamSourceVideo = true;
    return 0;
}

static int32_t StreamSourceSetStreamInfo(int32_t argc, const char *argv[])
{
    int32_t ret = 0;
    uint32_t sourceType = strtoul(argv[0x1], NULL, 0x10);
    MEDIA_INFO_LOG("sourceType:%u!", sourceType);
    if (sourceType == 0) {
        ret = InitAudioStreamSourceInfo(argc, argv);
    } else if (sourceType == 1) {
        ret = InitVideoStreamSourceInfo(argc, argv);
    } else {
        MEDIA_ERR_LOG("not support streamsource type[%u]!", sourceType);
        return -1;
    }
    g_ssServiceCtx->sourceType = SourceType::SOURCE_TYPE_STREAM;
    return ret;
}

static int32_t StartStreamSource(int32_t argc, const char *argv[])
{
    if (argc < 0x2) {
        MEDIA_ERR_LOG("argc invalid.");
        return -1;
    }
    MediaMutexLock(g_ssServiceMutex);

    if (g_ssServiceCtx->ssPlayerSrv.get() != nullptr) {
        MEDIA_ERR_LOG("already Started");
        MediaMutexUnLock(g_ssServiceMutex);
        return -1;
    }
    int32_t ret = StreamSourceSetStreamInfo(argc, argv);
    if (ret != 0) {
        MediaMutexUnLock(g_ssServiceMutex);
        return -1;
    }

#ifdef ENABLE_UIKIT
    // 2. post surface init event to graphic service thread
    GraphicService::GetInstance()->PostGraphicEvent([] {
        SurfaceInitEvent();
    });

    // 3. loop sleep 100ms for surface init complete
    int32_t waitTime = 0;
    while (!g_ssServiceCtx->surfaceInited && waitTime < WAIT_SURFACE_INIT_SUCCESS_TIME) {
        usleep(WAIT_SURFACE_INIT_SUCCESS_US);
        waitTime += WAIT_SURFACE_INIT_SUCCESS_US;
    }
    if (waitTime >= WAIT_SURFACE_INIT_SUCCESS_TIME) {
        MEDIA_ERR_LOG("wait SurfaceInitEvent time exceeds 1s.");
        return -1;
    }
    // 4. post surface Invalidate event to flush screen
    GraphicService::GetInstance()->PostGraphicEvent([] {
        SurfaceInvalidateEvent();
    });
#endif
    ret = StreamSourcePlay();
    if (ret != 0) {
        MediaMutexUnLock(g_ssServiceMutex);
        return -1;
    }
    MediaMutexUnLock(g_ssServiceMutex);
    return 0;
}

static int32_t PauseStreamSource(int32_t argc, const char *argv[])
{
    MEDIA_UNUSED(argc);
    MEDIA_UNUSED(argv);
    MediaMutexLock(g_ssServiceMutex);
    if (g_ssServiceCtx->ssPlayerSrv.get() == nullptr) {
        MEDIA_ERR_LOG("ssPlayerSrv nullptr");
        MediaMutexUnLock(g_ssServiceMutex);
        return -1;
    }
    int32_t ret = g_ssServiceCtx->ssPlayerSrv->Pause();
    if (ret != 0) {
        MEDIA_ERR_LOG("pause failed");
        MediaMutexUnLock(g_ssServiceMutex);
        return -1;
    }
    MediaMutexUnLock(g_ssServiceMutex);
    return 0;
}

static int32_t ResumeStreamSource(int32_t argc, const char *argv[])
{
    MEDIA_UNUSED(argc);
    MEDIA_UNUSED(argv);
    MediaMutexLock(g_ssServiceMutex);
    if (g_ssServiceCtx->ssPlayerSrv.get() == nullptr) {
        MEDIA_ERR_LOG("ssPlayerSrv nullptr");
        MediaMutexUnLock(g_ssServiceMutex);
        return -1;
    }
    int32_t ret = g_ssServiceCtx->ssPlayerSrv->Resume();
    if (ret != 0) {
        MEDIA_ERR_LOG("resume failed");
        MediaMutexUnLock(g_ssServiceMutex);
        return -1;
    }
    MediaMutexUnLock(g_ssServiceMutex);
    return 0;
}

static int32_t StopStreamSource(int32_t argc, const char *argv[])
{
    MEDIA_UNUSED(argc);
    MEDIA_UNUSED(argv);

    MediaMutexLock(g_ssServiceMutex);
    if (g_ssServiceCtx->ssPlayerSrv.get() == nullptr) {
        MEDIA_ERR_LOG("ssPlayerSrv has already stop");
        MediaMutexUnLock(g_ssServiceMutex);
        return 0;
    }
    StopStreamSourceProcess();
    StreamSourceStopAndExit();
    (void)DeinitPlayerResources();
    DestoryStreamSourceServiceContext();
    MediaMutexUnLock(g_ssServiceMutex);
    MEDIA_INFO_LOG("success");
    return 0;
}

int32_t StreamSourcePlayerSample(int32_t argc, const char *argv[])
{
    MEDIA_INFO_LOG("enter");
    if (argc < 1) {
        MEDIA_ERR_LOG("at least one parameter is required");
        return -1;
    }
    MEDIA_INFO_LOG("input param:%s", argv[0]);

    int32_t ret = InitStreamSourceServiceContext();
    if (ret != 0) {
        MEDIA_ERR_LOG("Init Context failed!");
        return -1;
    }
    for (uint32_t i = 0; i < ARRAY_SIZE(g_videoPlayFuncs); i++) {
        if (strcmp(argv[0], g_videoPlayFuncs[i].cmd) == 0) {
            ret = g_videoPlayFuncs[i].func(argc, argv);
            MEDIA_INFO_LOG("%s execute %s", g_videoPlayFuncs[i].cmd, (ret != 0) ? "failed" : "success");
            break;
        }
    }
    MEDIA_INFO_LOG("exit");
    return 0;
}

#ifdef __cplusplus
};
#endif
