/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2021-2021. All rights reserved.
 * Description: virtual camera impl
 * Author: media_develop team
 * Create: 20211012
 */

#include <sys/prctl.h>
#include <unistd.h>
#include <pthread.h>
#include "hw_camera.h"
#include "securec.h"
#include "demuxer_interface.h"
#include "demuxer_common.h"
#include "protocol_interface.h"
#include "media_hal_common.h"
#include "media_hal_thread_adapt.h"

#ifdef SUPPORT_DMA
#include "asm/dma.h"
#endif
#include "codec_interface.h"
#include "format_type.h"
#include "format_interface.h"
#define MODULE_NAME "virtual_camera"
const uint32_t ONLY_ONE_CAMERA_DEVICE = 1;
const uint32_t READ_FRAME_TIMEOUT = 20;
const uint32_t PARAM_MAX_NUM = 30;
const uint32_t MAX_PROGRAM_NUM = 256;
const uint32_t RETRY_SLEEP_TIME = 10000;
extern void ArchDCacheFlush(void);

typedef enum {
    OPENED,
    CLOSED
} DeviceState;

typedef struct {
    void *demuxHandle;
    void *codecHandle;
    FormatFrame *frame;
    DeviceState devState;
    uint32_t width;
    uint32_t height;
    StreamAttr streamInfo;
    char *url;
    BufferAvailable bufferAvailbleCallback;
    HwCameraVoidPtr userData;
    MediaHalThreadIdHandle callbackThread;
    FileInfo fmtFileInfo;
    AvCodecMime mime;
    uint32_t maxWidth;
    uint32_t maxHeight;
    bool receiveFormatChanged;
    bool cachedFrame;
    bool needSleep;
    CameraEventPkg event;
    OutputInfo outInfo;
} HwCameraContext;

static HwCameraContext g_hwCameraContext = { 0 };
static MediaHalMutexHandle g_mutex = NULL;

int32_t HwCameraInit(void)
{
    return MEDIA_HAL_OK;
}

int32_t HwCameraDeinit(void)
{
    return MEDIA_HAL_OK;
}

int32_t HwCameraGetDeviceNum(uint8_t *num)
{
    if (num == NULL) {
        return MEDIA_HAL_INVALID_PARAM;
    }
    *num = ONLY_ONE_CAMERA_DEVICE;

    return MEDIA_HAL_OK;
}

int32_t HwCameraGetDeviceList(uint32_t *cameraList, uint8_t listNum)
{
    CHK_NULL_RETURN(cameraList, MEDIA_HAL_INVALID_PARAM, "cameraList is null");
    if (listNum != ONLY_ONE_CAMERA_DEVICE) {
        return MEDIA_HAL_INVALID_PARAM;
    }
    *cameraList = ONLY_ONE_CAMERA_DEVICE - 1;

    return MEDIA_HAL_OK;
}

int32_t HwCameraDeviceOpen(uint32_t cameraId)
{
    MEDIA_HAL_UNUSED(cameraId);
    MediaHalMutexAttr attr = {true};
    g_mutex = MediaHalMutexCreate(&attr);
    CHK_NULL_RETURN(g_mutex, MEDIA_HAL_ERR, "mutex create failed");
    MediaHalMutexLock(g_mutex);
    g_hwCameraContext.devState = OPENED;
    g_hwCameraContext.frame = (FormatFrame *)MediaHalCalloc(sizeof(FormatFrame));
    if (g_hwCameraContext.frame == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "g_hwCameraContext.frame is null!");
        MediaHalMutexUnLock(g_mutex);
        MediaHalMutexDestroy(&g_mutex);
        return MEDIA_HAL_ERR;
    }
    MediaHalMutexUnLock(g_mutex);
    g_hwCameraContext.outInfo.buffers = (CodecBufferInfo *)MediaHalCalloc(sizeof(CodecBufferInfo));
    if (g_hwCameraContext.outInfo.buffers == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "g_hwCameraContext.outInfo.buffers is null!");
        MediaHalFree(g_hwCameraContext.frame);
        MediaHalMutexUnLock(g_mutex);
        MediaHalMutexDestroy(&g_mutex);
        return MEDIA_HAL_ERR;
    }

    return MEDIA_HAL_OK;
}

int32_t HwCameraDeviceClose(uint32_t cameraId)
{
    MEDIA_HAL_UNUSED(cameraId);
    MediaHalMutexLock(g_mutex);
    if (g_hwCameraContext.url != NULL) {
        MediaHalFree(g_hwCameraContext.url);
        g_hwCameraContext.url = NULL;
    }
    g_hwCameraContext.devState = CLOSED;
    g_hwCameraContext.receiveFormatChanged = false;
    g_hwCameraContext.cachedFrame = false;
    MediaHalFree(g_hwCameraContext.frame);
    g_hwCameraContext.frame = NULL;
    MediaHalFree(g_hwCameraContext.outInfo.buffers);
    g_hwCameraContext.outInfo.buffers = NULL;
    MediaHalMutexUnLock(g_mutex);
    MediaHalMutexDestroy(&g_mutex);
    return MEDIA_HAL_OK;
}

int32_t HwCameraDeviceSetInfo(uint32_t cameraId, const DeviceInfo *info)
{
    MEDIA_HAL_LOGI(MODULE_NAME, "in");
    MEDIA_HAL_UNUSED(cameraId);
    CHK_NULL_RETURN(info, MEDIA_HAL_INVALID_PARAM, "DeviceInfo is null");
    int32_t ret = MEDIA_HAL_OK;
    if (info->u.privateData == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "privateData is null");
        return MEDIA_HAL_INVALID_PARAM;
    }
    MediaHalMutexLock(g_mutex);
    if (g_hwCameraContext.devState != OPENED) {
        ret = MEDIA_HAL_INVALID_PARAM;
        goto FAIL;
    }
    if (g_hwCameraContext.url != NULL) {
        free(g_hwCameraContext.url);
        g_hwCameraContext.url = NULL;
    }
    g_hwCameraContext.url = strdup(info->u.privateData);
    if (g_hwCameraContext.url == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "strdup failed");
        ret = MEDIA_HAL_INVALID_PARAM;
        goto FAIL;
    }
FAIL:
    MediaHalMutexUnLock(g_mutex);
    return ret;
}

int32_t HwCameraStreamsCreate(uint32_t cameraId,
    const StreamAttr *streamList, uint32_t *streamIdList, uint8_t num)
{
    MEDIA_HAL_UNUSED(cameraId);
    CHK_NULL_RETURN(streamList, MEDIA_HAL_INVALID_PARAM, "streamList is null");
    CHK_NULL_RETURN(streamIdList, MEDIA_HAL_INVALID_PARAM, "streamIdList is null");
    CHK_NULL_RETURN(g_hwCameraContext.url, MEDIA_HAL_INVALID_PARAM, "url is null");
    if (num != ONLY_ONE_CAMERA_DEVICE) {
        return MEDIA_HAL_INVALID_PARAM;
    }
    int32_t ret = MEDIA_HAL_OK;
    MediaHalMutexLock(g_mutex);
    g_hwCameraContext.streamInfo = *streamList;
    *streamIdList = ONLY_ONE_CAMERA_DEVICE - 1;
    FormatSource source = { 0 };
    source.type = SOURCE_TYPE_URI;
    MEDIA_HAL_LOGI(MODULE_NAME, "url:%s", g_hwCameraContext.url);
    if (strcpy_s(source.url, URL_LEN, g_hwCameraContext.url) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "strcpy_s failed");
        ret = MEDIA_HAL_INVALID_PARAM;
        goto FAIL;
    }
    ret = CodecInit();
    if (ret != 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "CodecInit failed");
        goto FAIL;
    }
    ret = FormatDemuxerCreate(&source, &g_hwCameraContext.demuxHandle);
    if (ret != 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "FormatDemuxerCreate failed");
        goto FAIL;
    }
    ret = FormatDemuxerPrepare(g_hwCameraContext.demuxHandle);
    if (ret != 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "FormatDemuxerPrepare failed");
        goto FAIL;
    }
    ret = FormatDemuxerGetFileInfo(g_hwCameraContext.demuxHandle, &g_hwCameraContext.fmtFileInfo);
    if (ret != 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "FormatDemuxerGetFileInfo failed");
        goto FAIL;
    }
FAIL:
    MediaHalMutexUnLock(g_mutex);
    return ret;
}

int32_t HwCameraStreamsDestroy(uint32_t cameraId, const uint32_t *streamIdList, uint8_t num)
{
    (void)cameraId;
    (void)streamIdList;
    (void)num;
    return MEDIA_HAL_OK;
}

static int32_t QueueInputDataToVideoDecoder(void)
{
    InputInfo inputInfo = {0};
    CodecBufferInfo inBufInfo = {0};
    HwCameraContext *devCtx = &g_hwCameraContext;
    CHK_FAILED_RETURN(devCtx->cachedFrame, true, MEDIA_HAL_ERR, "no cachedFrame");
    int32_t ret = CodecDequeInput(devCtx->codecHandle, 0, &inputInfo);
    if (ret != CODEC_SUCCESS) {
        MEDIA_HAL_LOGE(MODULE_NAME, "CodecDequeInput failed");
        return MEDIA_HAL_ERR;
    }
    inBufInfo.addr = devCtx->frame->data;
    inBufInfo.length = devCtx->frame->len;
    inputInfo.bufferCnt = 1;
    inputInfo.buffers = &inBufInfo;
    inputInfo.pts = devCtx->frame->timestampUs;
    inputInfo.flag = (devCtx->frame->data == NULL && devCtx->frame->len == 0) ? STREAM_FLAG_EOS : 0;
    MEDIA_HAL_LOGI(MODULE_NAME, "inputInfo buffers len %d", inputInfo.buffers->length);
    if (CodecQueueInput(devCtx->codecHandle, &inputInfo, 0) < 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "CodecQueueInput failed");
        return MEDIA_HAL_ERR;
    }
    if (devCtx->cachedFrame) {
        if (FormatDemuxerFreeFrame(devCtx->demuxHandle, devCtx->frame) != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "FormatDemuxerFreeFrame failed");
        }
        devCtx->cachedFrame = false;
    }
    return MEDIA_HAL_OK;
}

static int32_t DequeueOutputBuffer(void)
{
    HwCameraContext *devCtx = &g_hwCameraContext;
    if (!devCtx->receiveFormatChanged) {
        MEDIA_HAL_LOGI(MODULE_NAME, "not receiveFormatChanged");
        return MEDIA_HAL_ERR;
    }
    if (memset_s(devCtx->outInfo.buffers, sizeof(CodecBufferInfo), 0, sizeof(CodecBufferInfo)) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memset failed!");
        return MEDIA_HAL_ERR;
    }
    devCtx->outInfo.bufferCnt = 1;
    int32_t ret = CodecDequeueOutput(devCtx->codecHandle, 0, NULL, &devCtx->outInfo);
    if (ret != 0 && ret != CODEC_RECEIVE_EOS) {
        MEDIA_HAL_LOGI(MODULE_NAME, "CodecDequeueOutput failed");
        return ret;
    }
    if (ret == CODEC_RECEIVE_EOS) {
        devCtx->outInfo.bufferCnt = 0;
        devCtx->outInfo.timeStamp = -1;
        devCtx->outInfo.sequence = 0;
        devCtx->outInfo.flag = 0;
        devCtx->outInfo.type = VIDEO_DECODER;
        devCtx->outInfo.vendorPrivate = NULL;
    }
    g_hwCameraContext.event.eventType = ret == CODEC_RECEIVE_EOS ? CAMERA_EOS_EVENT : CAMERA_BUFFER_DONE_EVENT;
    g_hwCameraContext.event.buffer = &devCtx->outInfo;
    if (devCtx->bufferAvailbleCallback != NULL && devCtx->userData != NULL) {
        devCtx->bufferAvailbleCallback(ONLY_ONE_CAMERA_DEVICE - 1, &devCtx->outInfo,
            ONLY_ONE_CAMERA_DEVICE, devCtx->userData, &g_hwCameraContext.event);
    }
    return MEDIA_HAL_OK;
}

static int32_t HwCameraDecode(void)
{
    int32_t ret = QueueInputDataToVideoDecoder();
    CHK_FAILED_RETURN_NO_LOG(ret, MEDIA_HAL_OK, MEDIA_HAL_ERR);
    return DequeueOutputBuffer();
}

static int32_t HwCameraReadFrame(void)
{
    HwCameraContext *devCtx = &g_hwCameraContext;
    if (devCtx->cachedFrame) {
        usleep(RETRY_SLEEP_TIME);
        return MEDIA_HAL_OK;
    }
    int32_t ret = FormatDemuxerReadFrame(devCtx->demuxHandle, devCtx->frame, READ_FRAME_TIMEOUT);
    if (ret == RET_FILE_EOF) {
        MEDIA_HAL_LOGI(MODULE_NAME, "read eos %d", ret);
        devCtx->event.eventType = CAMERA_EOS_EVENT;
        devCtx->event.buffer = NULL;
        if (devCtx->bufferAvailbleCallback != NULL && devCtx->userData != NULL) {
            devCtx->bufferAvailbleCallback(ONLY_ONE_CAMERA_DEVICE - 1, NULL,
                ONLY_ONE_CAMERA_DEVICE, devCtx->userData, &devCtx->event);
        }
        usleep(RETRY_SLEEP_TIME);
        return MEDIA_HAL_ERR;
    } else if (ret < 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "demuxer failed");
        return MEDIA_HAL_ERR;
    }
    devCtx->cachedFrame = true;
    return MEDIA_HAL_OK;
}

static void* AsyncProcess(void *arg)
{
    MEDIA_HAL_LOGI(MODULE_NAME, "in");
    HwCameraContext *devCtx = (HwCameraContext *)arg;
    if (devCtx == NULL || devCtx->demuxHandle == NULL) {
        return NULL;
    }
    prctl(PR_SET_NAME, "virtualCameraProcess", 0, 0, 0);
    while (true) {
        // 1. demux read input frame
        if (devCtx->devState != OPENED) {
            break;
        }
        if (devCtx->needSleep) {
            usleep(RETRY_SLEEP_TIME);
            devCtx->needSleep = false;
        }
        if (HwCameraReadFrame() < 0) {
            break;
        }
        if (HwCameraDecode() != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGI(MODULE_NAME, "docode fail");
            usleep(RETRY_SLEEP_TIME);
            continue;
        }
        usleep(RETRY_SLEEP_TIME);
    }
    MEDIA_HAL_LOGI(MODULE_NAME, "out");
    return NULL;
}

static int32_t OnCodecEvent(void *priv, EventType eventType, uintptr_t eventData)
{
    HwCameraContext *ctx = (HwCameraContext *)priv;
    FormatChange *data = (FormatChange *)eventData;
    ctx->event.fmtChange = (FormatEvent *)MediaMallocz(sizeof(FormatEvent));
    if (ctx->event.fmtChange == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "ctx->event fmtChange malloc failed");
        return MEDIA_HAL_ERR;
    }
    if (eventType == EVENT_OUT_FORMAT_CHANGED) {
        ctx->event.eventType = CAMERA_FORMAT_CHANGE_EVENT;
        ctx->event.fmtChange->bufferNum = data->bufferNum;
        ctx->event.fmtChange->width = data->width;
        ctx->event.fmtChange->height = data->height;
        ctx->event.fmtChange->widthStride = data->widthStride;
        ctx->event.fmtChange->heightStride = data->heightStride;
        ctx->event.fmtChange->bufferSize = data->bufferSize;
        MEDIA_HAL_LOGI(MODULE_NAME, "eventType %d, ctx->event buffernum %u",
            ctx->event.eventType, ctx->event.fmtChange->bufferNum);
        ctx->bufferAvailbleCallback(ONLY_ONE_CAMERA_DEVICE - 1, NULL,
                                    ONLY_ONE_CAMERA_DEVICE, ctx->userData, &ctx->event);
        ctx->receiveFormatChanged = true;
    }
    return MEDIA_HAL_OK;
}

static int32_t CodecOnEvent(uintptr_t comp, uintptr_t appData, EventType event,
                            uint32_t data1, uint32_t data2, uintptr_t eventData)
{
    (void)comp;
    (void)data1;
    (void)data2;
    (void)appData;

    return OnCodecEvent(&g_hwCameraContext, event, eventData);
}

int32_t HwCameraSetBufferCallback(uint32_t cameraId, const BufferAvailable callback, const HwCameraVoidPtr userData)
{
    CHK_NULL_RETURN(callback, MEDIA_HAL_INVALID_PARAM, "callback is NULL");
    CHK_NULL_RETURN(userData, MEDIA_HAL_INVALID_PARAM, "userData is NULL");
    if (cameraId != ONLY_ONE_CAMERA_DEVICE - 1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "cameraId is wrong");
        return MEDIA_HAL_INVALID_PARAM;
    }
    MediaHalMutexLock(g_mutex);
    if (g_hwCameraContext.devState != OPENED) {
        MEDIA_HAL_LOGE(MODULE_NAME, "wrong state");
        MediaHalMutexUnLock(g_mutex);
        return MEDIA_HAL_INVALID_PARAM;
    }
    g_hwCameraContext.bufferAvailbleCallback = callback;
    g_hwCameraContext.userData = (HwCameraVoidPtr)userData;
    MediaHalMutexUnLock(g_mutex);
    return MEDIA_HAL_OK;
}

static AvCodecMime TransCodecFormatToMime(CodecFormat type)
{
    AvCodecMime mime;
    switch (type) {
        case CODEC_H264:
            mime = MEDIA_MIMETYPE_VIDEO_AVC;
            break;
        case CODEC_H265:
            mime = MEDIA_MIMETYPE_VIDEO_HEVC;
            break;
        case CODEC_JPEG:
            mime = MEDIA_MIMETYPE_IMAGE_MJPEG;
            break;
        case CODEC_MJPEG:
            mime = MEDIA_MIMETYPE_IMAGE_JPEG;
            break;
        default:
            mime = MEDIA_MIMETYPE_INVALID;
    }
    return mime;
}

static bool InitCodecParams(Param *param, int32_t maxLen, int32_t *actualLen)
{
    int32_t index = 0;
    g_hwCameraContext.mime = TransCodecFormatToMime(g_hwCameraContext.fmtFileInfo.programInfo->track->vidTrack.format);
    g_hwCameraContext.maxWidth = g_hwCameraContext.fmtFileInfo.programInfo->track->vidTrack.width;
    g_hwCameraContext.maxHeight = g_hwCameraContext.fmtFileInfo.programInfo->track->vidTrack.height;
    static uint32_t bufSize = 0;
    static CodecType type = VIDEO_DECODER;

    param[index].key = KEY_MIMETYPE;
    param[index].val = (void *)(&g_hwCameraContext.mime);
    param[index].size = sizeof(g_hwCameraContext.mime);
    index++;
    param[index].key = KEY_WIDTH;
    param[index].val = (void *)(&g_hwCameraContext.maxWidth);
    param[index].size = sizeof(g_hwCameraContext.maxWidth);
    index++;
    param[index].key = KEY_HEIGHT;
    param[index].val = (void *)(&g_hwCameraContext.maxHeight);
    param[index].size = sizeof(g_hwCameraContext.maxHeight);
    index++;
    param[index].key = KEY_BUFFERSIZE;
    param[index].val = (void *)(&bufSize);
    param[index].size = sizeof(bufSize);
    index++;
    param[index].key = KEY_CODEC_TYPE;
    param[index].val = (void *)(&type);
    param[index].size = sizeof(type);
    index++;
    *actualLen = index;
    if (*actualLen > maxLen) {
        MEDIA_HAL_LOGE(MODULE_NAME, "anylsis vdec param too much");
        return false;
    }
    return true;
}

int32_t DecoderStart(HwCameraContext *ctx)
{
    Param param[PARAM_MAX_NUM];
    if (memset_s(param, PARAM_MAX_NUM * sizeof(Param), 0x00, PARAM_MAX_NUM * sizeof(Param)) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memset_s failed");
        return MEDIA_HAL_ERR;
    }
    int32_t actualLen = 0;
    char *videoName = "codec.avc.soft.decoder";
    if (!InitCodecParams(param, PARAM_MAX_NUM, &actualLen)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "InitCodecParams failed");
        return MEDIA_HAL_ERR;
    }
    int32_t ret = CodecCreate(videoName, param, actualLen, &ctx->codecHandle);
    if (ret != 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "CodecCreate failed, mime %d", *((AvCodecMime *)param[0].val));
        return ret;
    }

    CodecCallback callback = {};
    callback.InputBufferAvailable = NULL;
    callback.OutputBufferAvailable = NULL;
    callback.OnEvent = CodecOnEvent;
    ret = CodecSetCallback(ctx->codecHandle, &callback, (uintptr_t)&ctx);
    if (ret != 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "codec set call back failed");
        return ret;
    }
    ret = CodecStart(ctx->codecHandle);
    if (ret != 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "codec start failed");
        return ret;
    }

    return ret;
}

int32_t HwCameraStreamOn(uint32_t cameraId, uint32_t streamId)
{
    MediaHalMutexLock(g_mutex);
    MEDIA_HAL_LOGI(MODULE_NAME, "in");
    int32_t ret = MEDIA_HAL_OK;
    if (cameraId != ONLY_ONE_CAMERA_DEVICE - 1 || streamId != ONLY_ONE_CAMERA_DEVICE - 1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "cameraId or streamId is wrong");
        ret = MEDIA_HAL_ERR;
        goto FAIL;
    }
    if (g_hwCameraContext.devState != OPENED ||
        g_hwCameraContext.bufferAvailbleCallback == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "wrong state or callback is NULL");
        ret = MEDIA_HAL_ERR;
        goto FAIL;
    }

    ret = FormatDemuxerStart(g_hwCameraContext.demuxHandle);
    if (ret != 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "FormatDemuxerStart failed");
        goto FAIL;
    }
    MediaHalThreadattr attr = { "virtualCameraProcess", 0x4000, THREAD_SCHED_INVALID, 0, false };
    ret = DecoderStart(&g_hwCameraContext);
    g_hwCameraContext.callbackThread = MediaHalThreadCreate(AsyncProcess, &g_hwCameraContext, &attr);
    if (g_hwCameraContext.callbackThread == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "thread create failed");
        MediaHalMutexUnLock(g_mutex);
        ret = MEDIA_HAL_ERR;
        goto FAIL;
    }
FAIL:
    MediaHalMutexUnLock(g_mutex);
    return ret;
}

int32_t HwCameraStreamOff(uint32_t cameraId, uint32_t streamId)
{
    if (cameraId != ONLY_ONE_CAMERA_DEVICE - 1 ||
        streamId != ONLY_ONE_CAMERA_DEVICE - 1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "cameraId or streamId is wrong");
        return MEDIA_HAL_INVALID_PARAM;
    }
    MediaHalMutexLock(g_mutex);
    g_hwCameraContext.devState = CLOSED;
    MediaHalMutexUnLock(g_mutex);
    MediaHalThreadJoin(&g_hwCameraContext.callbackThread);
    
    int32_t ret = FormatDemuxerStop(g_hwCameraContext.demuxHandle);
    if (ret != 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "FormatDemuxerStart failed");
    }
    FormatDemuxerDestroy(g_hwCameraContext.demuxHandle);
    if (ret != 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "FormatDemuxerDestroy failed");
    }
    CodecStop(g_hwCameraContext.codecHandle);
    CodecDestroy(g_hwCameraContext.codecHandle);
    return MEDIA_HAL_OK;
}

int32_t HwCameraQueueBuf(uint32_t cameraId, uint32_t streamId, void *buffer, uint8_t num)
{
    CameraEventPkg *event = (CameraEventPkg *)buffer;
    if (buffer == NULL) {
        MediaHalMutexLock(g_mutex);
        g_hwCameraContext.needSleep = true;
        MediaHalMutexUnLock(g_mutex);
        return MEDIA_HAL_OK;
    }
    int32_t ret = CodecQueueOutput(g_hwCameraContext.codecHandle, (OutputInfo *)event->buffer, 0, -1);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, MEDIA_HAL_ERR, "CodecQueueOutput failed!");
    return MEDIA_HAL_OK;
}
