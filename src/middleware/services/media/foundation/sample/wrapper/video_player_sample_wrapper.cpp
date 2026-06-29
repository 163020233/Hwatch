/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2025. All rights reserved.
 * Description: video play sample wrapper
 * Author: Media Software Group
 * Create: 2022-10-17
 */

#include "video_player_sample_wrapper.h"

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
#include "video_player_service.h"
#elif defined(ENABLE_LVGL)
#include "lv_surface_view.h"
#include "lv_font_loader_ext.h"
#include "graphic_service_wrapper.h"
#include "misc/lv_color.h"
#include "misc/lv_timer.h"
#include "lv_label.h"
#endif
#include "media_mem.h"
#include "media_chr_log.h"

#ifdef __cplusplus
extern "C" {
#endif

using OHOS::Media::Player;
using OHOS::Media::PlayerSeekMode;
using OHOS::Media::Source;
using OHOS::Media::Format;
using namespace OHOS::Media;
using namespace::Audio;
using Audio::AudioManager;
using namespace OHOS;

#define MEDIA_FILE_PATH_MAX 260
#define MEDIA_CMD_LEN_MAX 64
#define MEDIA_UNUSED(x) ((void)(x))

const static int32_t WAIT_SURFACE_INIT_SUCCESS_US = 100000;
const static int32_t WAIT_SURFACE_DEINIT_SUCCESS_US = 100000;
const static int16_t WIDTH = 454;
const static int16_t HEIGHT = 454;
const static int32_t MINS_PER_HOUR = 60;
const static int32_t HOURS_PER_DAY = 24;
const static int32_t LABEL_WIDTH = 200;
const static int32_t LABEL_HEIGHT = 100;
const static int32_t LABEL_POS_X = 127;
const static int32_t LABEL_POS_Y = 50;
const static int32_t BUTTON_WIDTH = 227;
const static int32_t BUTTON_HEIGHT = 1010;
const static int32_t BUTTON_POS_X = 227;
const static int32_t BUTTON_POS_Y = 350;
const static int32_t LABEL_FRONT_SIZE = 40;
const static int32_t BUTTON_FRONT_SIZE = 30;
const static int32_t WAIT_TIMER = 1000;

const uint32_t REQUEST_BUFFER_RETRY_WAIT_TIME_US = 10000;
const uint32_t MAX_CONTINOUS_REQUEST_SURFACE_BUFFER_FAIL_COUNT = 100;
const uint32_t NUMBER_BASE = 10;
const char END_SIGN = '\0';

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
    std::shared_ptr<Player> adapter;
    MediaMutexHandle mutex;
    MediaThreadCondHandle cond;
    char    filePath[MEDIA_PATH_MAX];
} TestSample;

typedef struct {
    bool surfaceInited;
    AudioSession sessionId;
    AudioInterrupt interrupt;
    shared_ptr<Player> player;
    bool playError;
    bool interruptHintStop;
    bool isPlaybackCompleted;
    bool isPlaybackStopped;
    char filePath[MEDIA_FILE_PATH_MAX];
#if defined(ENABLE_UIKIT)
    Surface *surface;
    UILiteSurfaceView *surfaceView;
#elif defined(ENABLE_LVGL)
    LvSurfaceView *surface;
#endif
    TestSample *sample;
    MediaMutexHandle mutex;
    MediaThreadCondHandle cond;
    MediaThreadIdHandle taskHandle;
} VideoPlaySampleContext;

static MediaMutexHandle g_videoPlayContextMutex = nullptr;
static VideoPlaySampleContext *g_videoPlayContext = nullptr;
#if defined(ENABLE_LVGL)
static bool g_surfaceInited = false;
static bool g_pause = false;
static bool g_isPlayed = false;
static LvSurfaceView *g_surface = nullptr;
static lv_obj_t *g_label1 = nullptr;
static lv_obj_t* g_mask = nullptr;
#endif
static bool g_isNeedLoop = false;

static AudioManager& g_amInstance = AudioManager::GetInstance();

static int32_t StartVideoPlay(int32_t argc, const char *argv[]);
static int32_t PauseVideoPlay(int32_t argc, const char *argv[]);
static int32_t ResumeVideoPlay(int32_t argc, const char *argv[]);
static int32_t StopVideoPlay(int32_t argc, const char *argv[]);
static int32_t VideoPlayLoopOn(int32_t argc, const char *argv[]);
static int32_t VideoPlayLoopOff(int32_t argc, const char *argv[]);
static int32_t VideoPlayDumpInfo(int32_t argc, const char *argv[]);
static int32_t VideoPlayDumpInfo(int32_t argc, const char *argv[]);
static int32_t SetSpeedVideoPlay(int32_t argc, const char *argv[]);

typedef struct {
    char cmd[MEDIA_CMD_LEN_MAX];
    int32_t (*func)(int32_t argc, const char *argv[]);
} FuncsMap;

static FuncsMap g_videoPlayFuncs[] = {
    { "start", StartVideoPlay },
    { "pause", PauseVideoPlay },
    { "resume", ResumeVideoPlay },
    { "stop", StopVideoPlay },
    { "loopon", VideoPlayLoopOn },
    { "loopoff", VideoPlayLoopOff },
    { "dumpinfo", VideoPlayDumpInfo },
    { "tplay", SetSpeedVideoPlay },
};

class VideoPlayerInterruptListener : public InterruptListener {
public:
    VideoPlayerInterruptListener(VideoPlaySampleContext *in)
    {
        context = in;
    }
    ~VideoPlayerInterruptListener() override {};

    void OnInterrupt(int32_t type, int32_t hint) override
    {
        MEDIA_INFO_LOG("OnInterrupt, type:%d, hint:%d", type, hint);

        if (type == INTERRUPT_TYPE_BEGIN && hint == INTERRUPT_HINT_PAUSE) {
            MEDIA_INFO_LOG("player pause signal one");
        }
        if (type == INTERRUPT_TYPE_END && hint == INTERRUPT_HINT_RESUME) {
            MEDIA_INFO_LOG("player resume signal one");
        }
        if (type == INTERRUPT_TYPE_BEGIN && hint == INTERRUPT_HINT_STOP) {
            MEDIA_INFO_LOG("player stop begin signal one");
            MediaMutexLock(context->mutex);
            MediaThreadSetDetach(context->taskHandle, true);
            context->interruptHintStop = true;
            MediaThreadCondSignal(context->cond);
            MediaMutexUnLock(context->mutex);
        }
        if (type == INTERRUPT_TYPE_END && hint == INTERRUPT_HINT_STOP) {
            MEDIA_INFO_LOG("player stop end signal one");
        }
    }

private:
    VideoPlaySampleContext *context;
};

class VideoPlayCallback : public PlayerCallback {
public:
    VideoPlayCallback(VideoPlaySampleContext &in)
    {
        context = &in;
    }

    ~VideoPlayCallback() override {}

    void OnPlaybackComplete() override
    {
        MEDIA_INFO_LOG("payback complete signal one");
        MediaMutexLock(context->mutex);
        MediaThreadSetDetach(context->taskHandle, true);
        context->isPlaybackCompleted = true;
        MediaThreadCondSignal(context->cond);
        MediaMutexUnLock(context->mutex);
    }

    void OnError(int32_t errorType, int32_t errorCode) override
    {
        MEDIA_INFO_LOG("error type:%d, error code:%d", errorType, errorCode);
        if (errorType != PlayerErrorType::PLAYER_ERROR_UNKNOWN) {
            MEDIA_INFO_LOG("unsupport error type:%d", errorType);
            return;
        }
        if (errorCode == PlayerErrorCode::PLAYER_ERROR_CODE_AUD_PLAY_FAIL) {
            MEDIA_INFO_LOG("playback error signal one");
            MediaMutexLock(context->mutex);
            context->playError = true;
            MediaThreadCondSignal(context->cond);
            MediaMutexUnLock(context->mutex);
        }
    }

    void OnInfo(int32_t type, int32_t extra) override
    {
        MEDIA_UNUSED(type);
        MEDIA_UNUSED(extra);
        MEDIA_INFO_LOG("OnInfo test");
    }

    void OnRewindToComplete() override
    {
        MEDIA_INFO_LOG("OnRewindToComplete test");
    }

private:
    VideoPlaySampleContext *context;
};

#if defined(ENABLE_LVGL)
static void CreateSurface(void *userData)
{
    MEDIA_INFO_LOG("surface init start");
    if (g_surfaceInited) {
        MEDIA_INFO_LOG("surface has been init success");
        return;
    }
    MEDIA_INFO_LOG("surface init 2");
    lv_obj_t *obj = LvSurfaceViewCreate(static_cast<lv_obj_t *>(userData));
    LvSurfaceView *surface = reinterpret_cast<LvSurfaceView *>(obj);
    // 454: width and height
    LvInitSurfaceView(reinterpret_cast<lv_obj_t *>(surface), (lv_point_t) { 0, 0 }, WIDTH, HEIGHT);
    LvSetSurfaceColorkey(obj, lv_palette_main(LV_PALETTE_YELLOW));
    g_surface = surface;
    g_surfaceInited = true;

    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(obj, lv_color_black(), 0);
    lv_obj_set_style_radius(obj, 0, 0);

    MEDIA_INFO_LOG("surface init success");
}
#endif

#ifdef ENABLE_UIKIT
static void SurfaceDeinitEvent(void)
{
    RootView::GetInstance()->Remove(g_videoPlayContext->surfaceView);
    // remove后刷新全屏,避免局部刷新导致局部黑屏
    RootView::GetInstance()->Invalidate();

    if (g_videoPlayContext->surfaceView != nullptr) {
        delete g_videoPlayContext->surfaceView;
        g_videoPlayContext->surfaceView = nullptr;
    }

    g_videoPlayContext->surfaceInited = false;
}
#endif

static void SurfaceDeinitAndReclaimResource(void)
{
#ifdef ENABLE_UIKIT
    // 1. post surface deinit event to graphic service thread
    GraphicService::GetInstance()->PostGraphicEvent([] {
        SurfaceDeinitEvent();
    });

    // 2. loop sleep 100ms for surface deinit complete
    while (g_videoPlayContext->surfaceInited) {
        usleep(WAIT_SURFACE_DEINIT_SUCCESS_US);
    }
#endif
}

static void DestoryVideoPlayContext(VideoPlaySampleContext *videoCtx)
{
    if (videoCtx == NULL) {
        MEDIA_ERR_LOG("videoCtx is NULL!");
        return;
    }
    if (videoCtx->sample != nullptr) {
        MediaMutexDestroy(&videoCtx->sample->mutex);
        delete videoCtx->sample;
        videoCtx->sample = nullptr;
    }
    delete videoCtx;
    videoCtx = nullptr;
    g_videoPlayContext = nullptr;
    MEDIA_INFO_LOG("DestoryVideoPlayContext success!");
}

static void VideoStopAndExit(VideoPlaySampleContext * videoCtx)
{
    int32_t ret = videoCtx->player->Stop();
    if (ret != 0) {
        MEDIA_ERR_LOG("Stop failed");
    }
    ret = videoCtx->player->Reset();
    if (ret != 0) {
        MEDIA_ERR_LOG("Reset failed");
    }
    ret = videoCtx->player->Release();
    if (ret != 0) {
        MEDIA_ERR_LOG("Release failed");
    }
    MEDIA_CHR_LOG(MT_MEDIA_CHR_VIDEO_PLAYER_SAMPLE, MEDIA_CHR_THREAD_SINGNAL, MEDIA_CHR_THREAD_SINGNAL_STOP, 0);
#if defined(ENABLE_LVGL)
    if ((!videoCtx->isPlaybackCompleted && videoCtx->isPlaybackStopped) ||
        (videoCtx->isPlaybackCompleted && videoCtx->isPlaybackStopped)) {
        LvSurfaceBuffer *buf = LvRequestBuffer(reinterpret_cast<lv_obj_t *>(g_surface), 0);
        uint32_t count = 0;
        while (buf == nullptr) {
            count++;
            if (count == MAX_CONTINOUS_REQUEST_SURFACE_BUFFER_FAIL_COUNT) {
                MEDIA_ERR_LOG("continous request buffer fail count exceed upper limit");
                SurfaceDeinitAndReclaimResource();
                return;
            }
            buf = LvRequestBuffer(reinterpret_cast<lv_obj_t *>(g_surface), 0);
            usleep(REQUEST_BUFFER_RETRY_WAIT_TIME_US);
        }
        buf->format = LV_IMG_CF_UNKNOWN;
        LvFlushBuffer(reinterpret_cast<lv_obj_t *>(g_surface), buf);
    }
#endif
    videoCtx->player = nullptr;
    SurfaceDeinitAndReclaimResource();
    MEDIA_CHR_LOG(MT_MEDIA_CHR_VIDEO_PLAYER_SAMPLE, MEDIA_CHR_DESTORY_SOURCE, 0, 0);
    MEDIA_INFO_LOG("exit");
}

static void DeinitPlayerResources(VideoPlaySampleContext *context)
{
    (void)MediaThreadCondDestroy(&context->cond);
    (void)MediaMutexDestroy(&context->mutex);
}

#if defined(ENABLE_LVGL)
static void SetTextPause(void* userData)
{
    lv_label_set_text(reinterpret_cast<lv_obj_t*>(userData), "pause");
}

static void SetTextPlay(void* userData)
{
    lv_label_set_text(reinterpret_cast<lv_obj_t*>(userData), "play");
}
#endif

static int32_t VideoPlay(VideoPlaySampleContext *context)
{
    std::shared_ptr<PlayerCallback> callback = std::make_shared<VideoPlayCallback>(*context);
    if (callback == nullptr || callback.get() == nullptr) {
        MEDIA_ERR_LOG("callback is nullptr");
        return -1;
    }
    context->player->SetPlayerCallback(callback);
    int32_t ret = context->player->Play();
    if (ret != 0) {
        MEDIA_ERR_LOG("Play failed");
        return -1;
    }
    context->player->EnableSingleLooping(g_isNeedLoop);
    MediaMutexLock(g_videoPlayContextMutex);
#if defined(ENABLE_LVGL)
    g_isPlayed = true;
    if (g_label1 != nullptr) {
        PostGraphicEvent(SetTextPause, g_label1, true);
    }
#endif
    MediaMutexUnLock(g_videoPlayContextMutex);
    MediaMutexLock(context->mutex);
    while (!context->isPlaybackCompleted &&
        !context->isPlaybackStopped &&
        !context->interruptHintStop &&
        !context->playError) {
        MEDIA_CHR_LOG(MT_MEDIA_CHR_VIDEO_PLAYER_SAMPLE, MEDIA_CHR_CTEATE_PLAYER_WAIT, 0, 0);
        MediaThreadCondWait(context->cond, context->mutex);
        MEDIA_CHR_LOG(MT_MEDIA_CHR_VIDEO_PLAYER_SAMPLE, MEDIA_CHR_THREAD_SINGNAL, 0, 0);
    }
    if (context->playError) {
        MEDIA_CHR_LOG(MT_MEDIA_CHR_VIDEO_PLAYER_SAMPLE, MEDIA_CHR_THREAD_SINGNAL, MEDIA_CHR_THREAD_SINGNAL_ERROR, 0);
    }
    MediaMutexUnLock(context->mutex);
    return 0;
}

static bool InitPlayerResources(VideoPlaySampleContext *context)
{
    context->mutex = MediaMutexCreate(nullptr);
    if (context->mutex == nullptr) {
        MEDIA_ERR_LOG("create mutex failed");
        return false;
    }
    context->cond = MediaThreadCondCreate();
    if (context->cond == nullptr) {
        MEDIA_ERR_LOG("create thread cond failed");
        MediaMutexDestroy(&context->mutex);
        return false;
    }
    context->player = std::make_shared<Player>();
    if (context->player == nullptr || context->player.get() == nullptr) {
        MEDIA_ERR_LOG("player is nullptr");
        MediaMutexDestroy(&context->mutex);
        MediaThreadCondDestroy(&context->cond);
        return false;
    }
    return true;
}

static int32_t VideoPlayerSetSource(VideoPlaySampleContext *context)
{
    MEDIA_CHR_LOG(MT_MEDIA_CHR_VIDEO_PLAYER_SAMPLE, MEDIA_CHR_CREATE_PLAYER_THREAD, MEDIA_CHR_SET_SOURCE, 0);
    std::string uri(context->filePath);
    std::map<std::string, std::string> header;
    Source source(uri, header);
    if (context->player->SetSource(source) != 0) {
        MEDIA_ERR_LOG("SetSource failed");
        return -1;
    }
    MEDIA_CHR_LOG(MT_MEDIA_CHR_VIDEO_PLAYER_SAMPLE, MEDIA_CHR_CREATE_PLAYER_THREAD,
        MEDIA_CHR_SET_SOURCE, MEDIA_CHR_SET_LOCAL_SOURCE);
    return 0;
}

static int32_t RunVideoPlay(VideoPlaySampleContext *context)
{
    bool success = InitPlayerResources(context);
    if (!success) {
        MEDIA_ERR_LOG("init player resources failed");
        return -1;
    }
    int32_t ret = 0;
    if (VideoPlayerSetSource(context) != 0) {
        goto DEINIT_PLAYER;
    }
    ret = context->player->Prepare();
    if (ret != 0) {
        MEDIA_ERR_LOG("Prepare failed");
        goto RELEASE_PLAYER;
    }
    ret = context->player->SetAudioSessionId(context->sessionId);
    if (ret != 0) {
        MEDIA_ERR_LOG("SetAudioSessionId failed");
        goto RELEASE_PLAYER;
    }
    ret = context->player->SetAudioStreamType(AUDIO_STREAM_FITNESS_VIDEO);
    if (ret != 0) {
        MEDIA_ERR_LOG("SetAudioStreamType failed");
        goto RELEASE_PLAYER;
    }
#if defined(ENABLE_UIKIT)
    ret = context->player->SetVideoSurface(context->surface);
#elif defined(ENABLE_LVGL)
    ret = context->player->SetVideoSurface(g_surface);
#endif
    if (ret != 0) {
        MEDIA_ERR_LOG("SetVideoSurface failed");
        goto RELEASE_PLAYER;
    }
    ret = VideoPlay(context);
RELEASE_PLAYER:
    VideoStopAndExit(context);
#if defined(ENABLE_LVGL)
    context->isPlaybackCompleted = false;
    MediaMutexLock(g_videoPlayContextMutex);
    g_isPlayed = false;
    g_pause = false;
    MediaMutexUnLock(g_videoPlayContextMutex);
#endif
DEINIT_PLAYER:
    (void)DeinitPlayerResources(context);
    MEDIA_CHR_LOG(MT_MEDIA_CHR_VIDEO_PLAYER_SAMPLE, MEDIA_CHR_SAMPLE_THREAD_EXIT, 0, 0);
    return ret;
}

static bool IsFilePath(const char *param)
{
    bool isFilePath = false;
    char *extension = strrchr((char *)param, '.');
    if (extension != nullptr && strstr(extension + 1, "mp4") != nullptr) {
        isFilePath = true;
    }
    return isFilePath;
}

static bool CheckFilePath(const char *filePath)
{
    if (strlen(filePath) < MEDIA_FILE_PATH_MAX) {
        if (!IsFilePath(filePath)) {
            MEDIA_ERR_LOG("not support file path[%s]!", filePath);
            return false;
        }
        if (memcpy_s(g_videoPlayContext->filePath, MEDIA_FILE_PATH_MAX, filePath, strlen(filePath) + 1) != EOK) {
            MEDIA_ERR_LOG("memcpy_s failed");
            return false;
        }
    } else {
        MEDIA_ERR_LOG("input path too long");
        return false;
    }

    return true;
}

#ifdef ENABLE_UIKIT
static void SurfaceInitEvent(void)
{
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
    surfaceView->SetViewId("sur");
    surfaceView->SetPosition(0, 0, WIDTH, HEIGHT);
    // 当前规避方案：媒体视频播放sample中添加响应触摸事件操作，直接拦截触摸行为不做任何处理
    // 未来正式方案：媒体视频播放sample集成到图形uikit sample中统一管理
    surfaceView->SetTouchable(true);
    surfaceView->SetIntercept(true);

    RootView::GetInstance()->Add(surfaceView);

    Surface *surface = surfaceView->GetSurface();

    surface->SetFormat(PIXEL_FMT_YCBCR_420_SP);

    g_videoPlayContext->surface = surface;
    g_videoPlayContext->surfaceView = surfaceView;
    g_videoPlayContext->surfaceInited = true;

    MEDIA_INFO_LOG("surface init success");
}

static void SurfaceInvalidateEvent(void)
{
    MEDIA_INFO_LOG("in");
    RootView::GetInstance()->Invalidate();
    MEDIA_INFO_LOG("success");
}
#endif

static bool RequsetAudioFocus(VideoPlaySampleContext *context)
{
    bool success = g_amInstance.Initialize();
    if (!success) {
        MEDIA_ERR_LOG("audiomanager init failed");
        return false;
    }

    AudioSession sessionId = g_amInstance.MakeSessionId();
    if (sessionId == AUDIO_SESSION_ID_NONE) {
        MEDIA_ERR_LOG("audio session id invalid");
        return false;
    }

    std::shared_ptr<VideoPlayerInterruptListener> interruptListener =
        std::make_shared<VideoPlayerInterruptListener>(context);
    if (interruptListener == nullptr || interruptListener.get() == nullptr) {
        MEDIA_ERR_LOG("video player interrupt listener is nullptr");
        return false;
    }

    AudioInterrupt interrupt = { AUDIO_STREAM_FITNESS_VIDEO, sessionId, interruptListener };
    if (g_amInstance.ActivateAudioInterrupt(interrupt) == INTERRUPT_FAILED) {
        MEDIA_ERR_LOG("activate audio interrupt failed");
        return false;
    }
    context->sessionId = sessionId;
    context->interrupt = interrupt;
    MEDIA_CHR_LOG(MT_MEDIA_CHR_VIDEO_PLAYER_SAMPLE, MEDIA_CHR_CREATE_PLAYER_THREAD,
        MEDIA_CHR_ACTIVE_AUDIO_INTERRPUT, 0);
    return true;
}

static bool ReleaseAudioFocus(VideoPlaySampleContext *context)
{
    if (g_amInstance.DeactivateAudioInterrupt(context->interrupt) != 0) {
        MEDIA_ERR_LOG("deactivate audio interrupt failed");
        return false;
    }

    return true;
}

static void *VideoPlayThread(void *priv)
{
    VideoPlaySampleContext *context = (VideoPlaySampleContext *)priv;
    int32_t ret = 0;
    bool success = RequsetAudioFocus(context);
    if (!success) {
        MEDIA_ERR_LOG("requset audio focus failed");
        goto EXIT;
    }

    ret = RunVideoPlay(context);
    if (ret != 0) {
        MEDIA_ERR_LOG("run video play failed");
        (void)ReleaseAudioFocus(context);
        goto EXIT;
    }

    success = ReleaseAudioFocus(context);
    if (!success) {
        MEDIA_ERR_LOG("release audio focus failed");
        goto EXIT;
    }
EXIT:
    MediaMutexLock(g_videoPlayContextMutex);
    DestoryVideoPlayContext(context);
#if defined(ENABLE_LVGL)
    if (g_label1 != nullptr) {
        PostGraphicEvent(SetTextPlay, g_label1, true);
    }
    g_isPlayed = false;
    g_pause = false;
#endif
    MediaMutexUnLock(g_videoPlayContextMutex);
    MEDIA_INFO_LOG("VideoPlayThread end!");
    return nullptr;
}

static int32_t StartVideoPlay(int32_t argc, const char *argv[])
{
    if (argc < 0x2) {
        MEDIA_ERR_LOG("The number of input parameters is insufficient.");
        DestoryVideoPlayContext(g_videoPlayContext);
        return -1;
    }
    bool valid = CheckFilePath(argv[1]);
    if (!valid) {
        MEDIA_ERR_LOG("file path is invalid");
        DestoryVideoPlayContext(g_videoPlayContext);
        return -1;
    }

#ifdef ENABLE_UIKIT
    // 2. post surface init event to graphic service thread
    GraphicService::GetInstance()->PostGraphicEvent([] {
        SurfaceInitEvent();
    });
    // 3. loop sleep 100ms for surface init complete
    while (!g_videoPlayContext->surfaceInited) {
        usleep(WAIT_SURFACE_INIT_SUCCESS_US);
    }
    // 4. post surface Invalidate event to flush screen
    GraphicService::GetInstance()->PostGraphicEvent([] {
        SurfaceInvalidateEvent();
    });
#endif
    MEDIA_CHR_LOG(MT_MEDIA_CHR_VIDEO_PLAYER_SAMPLE, MEDIA_CHR_INIT_SOURCE, 0, 0);
    // 5. create video play thread
    MediaThreadattr attr = { "VideoPlayThread", 0x2000, THREAD_SCHED_INVALID, 70, false };
    MediaThreadIdHandle taskHandle = MediaThreadCreate(VideoPlayThread, g_videoPlayContext, &attr);
    if (taskHandle == nullptr) {
        MEDIA_ERR_LOG("create video play thread failed");
        DestoryVideoPlayContext(g_videoPlayContext);
        return -1;
    }
    if (g_videoPlayContext != nullptr) {
        g_videoPlayContext->taskHandle = taskHandle;
    }
    MEDIA_CHR_LOG(MT_MEDIA_CHR_VIDEO_PLAYER_SAMPLE, MEDIA_CHR_CREATE_PLAYER_THREAD, 0, 0);
    return 0;
}

static int32_t PauseVideoPlay(int32_t argc, const char *argv[])
{
    MEDIA_UNUSED(argc);
    MEDIA_UNUSED(argv);
    MediaMutexLock(g_videoPlayContextMutex);
    if (g_videoPlayContext->taskHandle == nullptr) {
        MEDIA_ERR_LOG("can not pause play");
        MediaMutexUnLock(g_videoPlayContextMutex);
        return -1;
    }
    if (g_videoPlayContext->isPlaybackStopped ||
        g_videoPlayContext->isPlaybackCompleted ||
        g_videoPlayContext->interruptHintStop) {
        MEDIA_ERR_LOG("current state is stop, can not pause play");
        MediaMutexUnLock(g_videoPlayContextMutex);
        return -1;
    }
    int32_t ret = g_videoPlayContext->player->Pause();
    if (ret != 0) {
        MEDIA_ERR_LOG("pause failed");
        MediaMutexUnLock(g_videoPlayContextMutex);
        return -1;
    }
#if defined(ENABLE_LVGL)
    g_pause = true;
#endif
    MediaMutexUnLock(g_videoPlayContextMutex);
    return 0;
}

static int32_t SetSpeedVideoPlay(int32_t argc, const char *argv[])
{
    if (argc < 3) {
        return -1;
    }
    char *endPtr;
    int32_t flag = static_cast<int32_t>(strtol(argv[1], &endPtr, NUMBER_BASE));
    if (*endPtr != END_SIGN) {
        MEDIA_ERR_LOG("convert str to number failed!");
        return MEDIA_ERR;
    }
    float speed = strtof(argv[2], &endPtr);
    if ((speed < 0.001 && speed > -0.001) || *endPtr != END_SIGN) {
        MEDIA_ERR_LOG("convert str to number failed or invalid number!");
        return -1;
    }
    speed = flag == 0 ? speed : - speed;
    MediaMutexLock(g_videoPlayContextMutex);
    if (g_videoPlayContext->taskHandle == nullptr) {
        MEDIA_ERR_LOG("can not pause play");
        MediaMutexUnLock(g_videoPlayContextMutex);
        return -1;
    }
    if (g_videoPlayContext->isPlaybackStopped ||
        g_videoPlayContext->isPlaybackCompleted ||
        g_videoPlayContext->interruptHintStop) {
        MEDIA_ERR_LOG("current state is stop, can not pause play");
        MediaMutexUnLock(g_videoPlayContextMutex);
        return -1;
    }
    int32_t ret = g_videoPlayContext->player->SetPlaybackSpeed(speed);
    if (ret != 0) {
        MEDIA_ERR_LOG("pause failed");
        MediaMutexUnLock(g_videoPlayContextMutex);
        return -1;
    }
    MediaMutexUnLock(g_videoPlayContextMutex);
    return 0;
}

static int32_t ResumeVideoPlay(int32_t argc, const char *argv[])
{
    MEDIA_UNUSED(argc);
    MEDIA_UNUSED(argv);
    MediaMutexLock(g_videoPlayContextMutex);
    if (g_videoPlayContext->taskHandle == nullptr) {
        MEDIA_ERR_LOG("can not resume play");
        MediaMutexUnLock(g_videoPlayContextMutex);
        return -1;
    }
    if (g_videoPlayContext->isPlaybackStopped ||
        g_videoPlayContext->isPlaybackCompleted ||
        g_videoPlayContext->interruptHintStop) {
        MEDIA_ERR_LOG("current state is stop, can not resume play");
        MediaMutexUnLock(g_videoPlayContextMutex);
        return -1;
    }
    int32_t ret = g_videoPlayContext->player->Play();
    if (ret != 0) {
        MEDIA_ERR_LOG("resume failed");
        MediaMutexUnLock(g_videoPlayContextMutex);
        return -1;
    }
#if defined(ENABLE_LVGL)
    g_pause = false;
#endif
    MediaMutexUnLock(g_videoPlayContextMutex);
    return 0;
}

static int32_t StopVideoPlay(int32_t argc, const char *argv[])
{
    MEDIA_UNUSED(argc);
    MEDIA_UNUSED(argv);

    MediaMutexLock(g_videoPlayContextMutex);
    if (g_videoPlayContext->taskHandle == nullptr) {
        MEDIA_ERR_LOG("video has already stop");
        MediaMutexUnLock(g_videoPlayContextMutex);
        return 0;
    }

    if (g_videoPlayContext->isPlaybackStopped ||
        g_videoPlayContext->interruptHintStop ||
        g_videoPlayContext->isPlaybackCompleted) {
        MEDIA_ERR_LOG("video play already stopped");
        MediaMutexUnLock(g_videoPlayContextMutex);
        return 0;
    }
    MediaMutexLock(g_videoPlayContext->mutex);
    g_videoPlayContext->isPlaybackStopped = true;
    MediaThreadCondSignal(g_videoPlayContext->cond);
    MediaMutexUnLock(g_videoPlayContext->mutex);
    MediaMutexUnLock(g_videoPlayContextMutex);
    MediaThreadJoin(&g_videoPlayContext->taskHandle);
    MEDIA_ERR_LOG("payback stop signal one");
    return 0;
}

static int32_t VideoPlayLoopOn(int32_t argc, const char *argv[])
{
    MEDIA_UNUSED(argc);
    MEDIA_UNUSED(argv);
    MediaMutexLock(g_videoPlayContextMutex);
    if (g_videoPlayContext->taskHandle == nullptr) {
        MEDIA_ERR_LOG("video play context is null, execute play command first");
        MediaMutexUnLock(g_videoPlayContextMutex);
        return -1;
    }
    if (g_videoPlayContext->player == nullptr ||
        g_videoPlayContext->player.get() == nullptr) {
        MEDIA_ERR_LOG("player is nullptr");
        MediaMutexUnLock(g_videoPlayContextMutex);
        return -1;
    }
    int32_t ret = g_videoPlayContext->player->EnableSingleLooping(true);
    if (ret != 0) {
        MEDIA_ERR_LOG("loop on failed");
        MediaMutexUnLock(g_videoPlayContextMutex);
        return -1;
    }

    MediaMutexUnLock(g_videoPlayContextMutex);
    return 0;
}

static int32_t VideoPlayLoopOff(int32_t argc, const char *argv[])
{
    MEDIA_UNUSED(argc);
    MEDIA_UNUSED(argv);
    MediaMutexLock(g_videoPlayContextMutex);
    if (g_videoPlayContext->taskHandle == nullptr) {
        MEDIA_ERR_LOG("video play context is null, execute play command first");
        MediaMutexUnLock(g_videoPlayContextMutex);
        return -1;
    }
    if (g_videoPlayContext->player == nullptr ||
        g_videoPlayContext->player.get() == nullptr) {
        MEDIA_ERR_LOG("player is nullptr");
        MediaMutexUnLock(g_videoPlayContextMutex);
        return -1;
    }
    int32_t ret = g_videoPlayContext->player->EnableSingleLooping(false);
    if (ret != 0) {
        MEDIA_ERR_LOG("loop off failed");
        MediaMutexUnLock(g_videoPlayContextMutex);
        return -1;
    }

    MediaMutexUnLock(g_videoPlayContextMutex);
    return 0;
}

int32_t VideoPlayDumpInfo(int32_t argc, const char *argv[])
{
    int32_t ret = 0;
    MEDIA_UNUSED(argc);
    MEDIA_UNUSED(argv);
    MediaMutexLock(g_videoPlayContextMutex);
    if (g_videoPlayContext != nullptr &&
        g_videoPlayContext->player != nullptr &&
        g_videoPlayContext->player.get() != nullptr) {
        PlayerDebugInfo *playerInfo = new PlayerDebugInfo();
        ret = g_videoPlayContext->player->DumpInfo(playerInfo);
        if (ret != 0) {
            MEDIA_ERR_LOG("get dump info failed!");
        }
        delete playerInfo;
    }
    MediaMutexUnLock(g_videoPlayContextMutex);
    return ret;
}

static int32_t InitVideoPlayContext(void)
{
    MediaInitStaticMutexLock(&g_videoPlayContextMutex);
    MediaMutexLock(g_videoPlayContextMutex);
    if (g_videoPlayContext == nullptr) {
        g_videoPlayContext = new VideoPlaySampleContext();
        if (g_videoPlayContext == nullptr) {
            MEDIA_ERR_LOG("malloc failed");
            MediaMutexUnLock(g_videoPlayContextMutex);
            return -1;
        }
        g_videoPlayContext->sample = new TestSample();
        if (g_videoPlayContext->sample == nullptr) {
            MEDIA_ERR_LOG("malloc failed");
            MediaMutexUnLock(g_videoPlayContextMutex);
            goto EXIT;
        }
        if (g_videoPlayContext->sample->mutex == NULL) {
            g_videoPlayContext->sample->mutex = MediaMutexCreate(nullptr);
            if (g_videoPlayContext->sample->mutex == nullptr) {
                MEDIA_ERR_LOG("create mutex failed");
                MediaMutexUnLock(g_videoPlayContextMutex);
                goto EXIT1;
            }
        }
    }
    MediaMutexUnLock(g_videoPlayContextMutex);
    return 0;
EXIT1:
    delete g_videoPlayContext->sample;
    g_videoPlayContext->sample = nullptr;
EXIT:
    delete g_videoPlayContext;
    g_videoPlayContext = nullptr;
    return -1;
}

int32_t VideoPlayerSample(int32_t argc, const char *argv[])
{
    MEDIA_INFO_LOG("enter");
    if (argc < 1) {
        MEDIA_ERR_LOG("at least one parameter is required");
        return -1;
    }
    MEDIA_INFO_LOG("input param:%s", argv[0]);

    int32_t ret = InitVideoPlayContext();
    if (ret != 0) {
        MEDIA_ERR_LOG("InitVideoPlayContext failed!");
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

#ifdef ENABLE_LVGL
static void LvStopVideoPlay(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    MediaInitStaticMutexLock(&g_videoPlayContextMutex);
    MediaMutexLock(g_videoPlayContextMutex);
    if (!g_isPlayed) {
        MEDIA_INFO_LOG("not need to stop!!");
        MediaMutexUnLock(g_videoPlayContextMutex);
        return;
    }
    MediaMutexUnLock(g_videoPlayContextMutex);
    StopVideoPlay(nullptr);
}

static void LvStartVideoPlay(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    const char *filePath = (const char *)lv_event_get_user_data(e);
    MediaInitStaticMutexLock(&g_videoPlayContextMutex);
    MediaMutexLock(g_videoPlayContextMutex);
    if (g_isPlayed) {
        if (g_pause) {
            MediaMutexUnLock(g_videoPlayContextMutex);
            int32_t ret = ResumeVideoPlay(nullptr);
            MediaMutexLock(g_videoPlayContextMutex);
            if (ret == 0) {
                lv_label_set_text(obj, "pause");
                MEDIA_INFO_LOG("Resume success!");
            }
        } else {
            MediaMutexUnLock(g_videoPlayContextMutex);
            int32_t ret = PauseVideoPlay(nullptr);
            MediaMutexLock(g_videoPlayContextMutex);
            if (ret == 0) {
                lv_label_set_text(obj, "play");
                MEDIA_INFO_LOG("Pause success!");
            }
        }
    } else {
        MediaMutexUnLock(g_videoPlayContextMutex);
        int32_t ret = StartVideoPlay(filePath);
        MediaMutexLock(g_videoPlayContextMutex);
        if (ret == 0) {
            MEDIA_INFO_LOG("start success!");
        }
    }
    MediaMutexUnLock(g_videoPlayContextMutex);
}

static void LvPauseStartVideoPlay(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    MediaMutexLock(g_videoPlayContextMutex);
    if (!g_isPlayed) {
        MEDIA_INFO_LOG("can not pause player!");
        MediaMutexUnLock(g_videoPlayContextMutex);
        return;
    }
    if (g_pause) {
        MediaMutexUnLock(g_videoPlayContextMutex);
        int32_t ret = ResumeVideoPlay(nullptr);
        MediaMutexLock(g_videoPlayContextMutex);
        if (ret == 0) {
            MEDIA_INFO_LOG("Resume success!");
        }
    } else {
        MediaMutexUnLock(g_videoPlayContextMutex);
        int32_t ret = PauseVideoPlay(nullptr);
        MediaMutexLock(g_videoPlayContextMutex);
        if (ret == 0) {
            MEDIA_INFO_LOG("Pause success!");
        }
    }
    MediaMutexUnLock(g_videoPlayContextMutex);
}

static void TestBtnSetUp(lv_obj_t *parent)
{
    LvInitFontExt();
    static lv_font_t *font = LvFontLoadExt(DEFAULT_VECTOR_FONT_FILENAME, BUTTON_FRONT_SIZE); // 30: size

    g_label1 = lv_label_create(parent);
    lv_obj_set_size(g_label1, BUTTON_WIDTH, BUTTON_HEIGHT);
    lv_obj_set_pos(g_label1, 0, BUTTON_POS_Y);
    lv_obj_add_flag(g_label1, LV_OBJ_FLAG_CLICKABLE);

    lv_label_set_text(g_label1, "play");
    lv_obj_set_style_text_color(g_label1, lv_palette_main(LV_PALETTE_AMBER), 0);
    lv_obj_set_style_text_align(g_label1, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(g_label1, font, 0);

    const char *filePath = "/user/video/panda.mp4";
    lv_obj_add_event_cb(g_label1, LvStartVideoPlay, LV_EVENT_RELEASED, (void *)filePath);

    lv_obj_t *label2 = lv_label_create(parent);
    lv_obj_set_size(label2, BUTTON_WIDTH, BUTTON_HEIGHT);
    lv_obj_set_pos(label2, BUTTON_POS_X, BUTTON_POS_Y);
    lv_obj_add_flag(label2, LV_OBJ_FLAG_CLICKABLE);

    lv_label_set_text(label2, "stop");
    lv_obj_set_style_text_color(label2, lv_palette_main(LV_PALETTE_AMBER), 0);
    lv_obj_set_style_text_align(label2, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(label2, font, 0);

    lv_obj_add_event_cb(label2, LvStopVideoPlay, LV_EVENT_RELEASED, nullptr);

    MEDIA_INFO_LOG("TestBtnSetUp success!");
}

void LvDemoVideoPlay(void)
{
    g_isNeedLoop = false;
    if (g_surfaceInited) {
        MEDIA_INFO_LOG("vedio player start success!");
        return;
    }
    lv_obj_set_style_bg_opa(lv_scr_act(), LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);
    TestBtnSetUp(lv_layer_top());
    CreateSurface(lv_scr_act());
}

static void SetTime(lv_obj_t* label)
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    uint64_t second = static_cast<uint64_t>(tv.tv_sec);
    uint64_t minutes = second / MINS_PER_HOUR; // 60: mins per hour
    uint64_t hour = minutes / MINS_PER_HOUR; // 60: mins per hour
    uint64_t day = hour / HOURS_PER_DAY; // 24: hours per day
    uint64_t realHour = hour % HOURS_PER_DAY; // 24: hours per day
    uint64_t realMinute = minutes % MINS_PER_HOUR; // 60: mins per hour
    uint64_t realMilliSecond = second % MINS_PER_HOUR; // 60: mins per hour
    lv_label_set_text_fmt(label, "%02lld:%02lld:%02lld", realHour, realMinute, realMilliSecond);
}

static void TimeUpdateCb(lv_timer_t* timer)
{
    lv_obj_t* label = static_cast<lv_obj_t*>(timer->user_data);
    SetTime(label);
}

void LvDemoVideoDial(void)
{
    if (g_surfaceInited) {
        MEDIA_INFO_LOG("vedio player start success!");
        return;
    }
    lv_obj_set_style_bg_opa(lv_scr_act(), LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);

    CreateSurface(lv_scr_act());

    LvInitFontExt();
    static lv_font_t *font = LvFontLoadExt(DEFAULT_VECTOR_FONT_FILENAME, LABEL_FRONT_SIZE); // 40: size

    lv_obj_t* label = lv_label_create(lv_layer_top());
    lv_obj_set_size(label, LABEL_WIDTH, LABEL_HEIGHT);
    lv_obj_set_pos(label, LABEL_POS_X, LABEL_POS_Y);
    lv_obj_add_flag(label, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(label, LvPauseStartVideoPlay, LV_EVENT_RELEASED, nullptr);

    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(label, font, 0);
    SetTime(label);

    lv_timer_create(TimeUpdateCb, WAIT_TIMER, static_cast<void*>(label));

    const char *filePath = "/user/video/demo_pure_video_loop.mp4";
    g_isNeedLoop = true;
    StartVideoPlay(filePath);
}
#endif

#ifdef __cplusplus
};
#endif
