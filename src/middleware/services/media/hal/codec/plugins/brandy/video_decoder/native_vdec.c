/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2025-2025. All rights reserved.
 * Description: vdec plugin
 * Author: Media Software Group
 * Create: 2025-03-08
 */

#include "native_vdec.h"
#include "media_hal_common.h"
#include "media_hal_thread_adapt.h"
#include "limits.h"

#include "vdec_common.h"

#include "soc_vdec_api.h"
#include "drv_vdec_intf.h"
#if defined(ENABLE_UIKIT) || defined(ENABLE_LVGL)
#include "graphic_service_wrapper.h"
#endif
#include <stdio.h>
#define MODULE_NAME "plugin_vdec"

#define MAX_BUFFER_NUM 5

typedef struct {
    bool inited;
    uintptr_t addr;
    OutputInfo outBuf;
    ext_vdec_frame* vdecFrame;
} BufferDesp;

typedef struct {
    BufferDesp bufs[MAX_BUFFER_NUM];
    uint32_t usedSize;
    MediaHalMutexHandle vectorMutex;
} BufferVector;

typedef struct {
    bool receiveEos;
    bool changedEventReceived;
    bool isStreamSource;
    td_handle vdecHandle;
    CodecCallback callback;
    uintptr_t callbackInstance;
    BufferVector externalBufferVec;
} NativeVdecContext;

#define DEC_WIDTH_MAX 600
#define DEC_HEIGHT_MAX 600
#define COMPRESSION_RATIO (10)
#define INPUT_BUFFER_MIN (5)

#define DEC_WIDTH_MIN 8
#define DEC_HEIGHT_MIN 8
#define FLUSH_THRESHOLD 2

static const CodecCapbility g_vdecCapEntries[] = {
    {
        MEDIA_MIMETYPE_IMAGE_JPEG, VIDEO_DECODER, {ALGIN_LEVEL_2, ALGIN_LEVEL_2},
        {DEC_WIDTH_MIN, DEC_HEIGHT_MIN}, {DEC_WIDTH_MAX, DEC_HEIGHT_MAX},
        VID_BITRATE_LEVEL1, VID_BITRATE_LEVEL2, {{INVALID_PROFILE}, 1}, {{INVALID_LEVEL}, 1},
        {{YVU_SEMIPLANAR_420}, 1}, 1, 1, ALLOCATE_INPUT_BUFFER_USER | ALLOCATE_OUTPUT_BUFFER_CODEC, 0
    },
    {
        MEDIA_MIMETYPE_IMAGE_MJPEG, VIDEO_DECODER, {ALGIN_LEVEL_2, ALGIN_LEVEL_2},
        {DEC_WIDTH_MIN, DEC_HEIGHT_MIN}, {DEC_WIDTH_MAX, DEC_HEIGHT_MAX},
        VID_BITRATE_LEVEL1, VID_BITRATE_LEVEL2, {{INVALID_PROFILE}, 1}, {{INVALID_LEVEL}, 1},
        {{YVU_SEMIPLANAR_420}, 1}, 1, 1, ALLOCATE_INPUT_BUFFER_USER | ALLOCATE_OUTPUT_BUFFER_CODEC, 0
    },
    {
        MEDIA_MIMETYPE_VIDEO_AVC, VIDEO_DECODER, {ALGIN_LEVEL_2, ALGIN_LEVEL_2},
        {DEC_WIDTH_MIN, DEC_HEIGHT_MIN}, {DEC_WIDTH_MAX, DEC_HEIGHT_MAX},
        VID_BITRATE_LEVEL1, VID_BITRATE_LEVEL2, {{INVALID_PROFILE}, 1}, {{INVALID_LEVEL}, 1},
        {{YVU_SEMIPLANAR_420}, 1}, 1, 1, ALLOCATE_INPUT_BUFFER_USER | ALLOCATE_OUTPUT_BUFFER_CODEC, 0
    },
};

static int32_t NativeVdecReset(void *nativeVdecHdl);

static void DeinitBufferVector(BufferVector *vec)
{
    for (int32_t i = 0; i < MAX_BUFFER_NUM; i++) {
        if (vec->bufs[i].outBuf.buffers != NULL) {
            MediaHalFree(vec->bufs[i].outBuf.buffers);
            vec->bufs[i].outBuf.buffers = NULL;
        }

        if (vec->bufs[i].vdecFrame != NULL) {
            MediaHalFree(vec->bufs[i].vdecFrame);
            vec->bufs[i].vdecFrame = NULL;
        }

        vec->bufs[i].inited = false;
    }

    vec->usedSize = 0;
    return;
}

static int32_t InitBufferVector(BufferVector *vec)
{
    for (int32_t i = 0; i < MAX_BUFFER_NUM; i++) {
        if (memset_s(&(vec->bufs[i].outBuf), sizeof(OutputInfo), 0, sizeof(OutputInfo)) != EOK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "memset_s failed");
        }
        CodecBufferInfo *inf = (CodecBufferInfo *)MediaHalMalloc(sizeof(CodecBufferInfo));
        if (inf == NULL) {
            MEDIA_HAL_LOGE(MODULE_NAME, "malloc failed");
            goto ERR;
        }
        if (memset_s(inf, sizeof(CodecBufferInfo), 0, sizeof(CodecBufferInfo)) != EOK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "memset_s failed");
        }
        vec->bufs[i].outBuf.buffers = inf;

        vec->bufs[i].vdecFrame = (ext_vdec_frame *)MediaHalMalloc(sizeof(ext_vdec_frame));
        if (vec->bufs[i].vdecFrame == NULL) {
            MEDIA_HAL_LOGE(MODULE_NAME, "malloc failed");
            goto ERR;
        }
        if (memset_s(vec->bufs[i].vdecFrame, sizeof(ext_vdec_frame), 0, sizeof(ext_vdec_frame)) != EOK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "memset_s failed");
        }
        vec->bufs[i].inited = true;
    }

    vec->usedSize = 0;
    return 0;
ERR:
    DeinitBufferVector(vec);
    return -1;
}

static void CopyOutputInfo(OutputInfo *dst, OutputInfo *src)
{
    // 注意这里有陷阱，不能直接结构体赋值，否则会将队列元素中malloc的指针覆盖
    // 此处采用逐项赋值方式，稳妥
    dst->bufferCnt = src->bufferCnt;
    dst->buffers[0] = src->buffers[0];
    dst->buffers[0].addr = src->buffers[0].addr;
    dst->buffers[0].offset = src->buffers[0].offset;
    dst->buffers[0].uvOffset = src->buffers[0].uvOffset;

    dst->flag = src->flag;
    dst->sequence = src->sequence;
    dst->timeStamp = src->timeStamp;
    dst->type = src->type;
    dst->vendorPrivate = src->vendorPrivate;
    dst->width = src->width;
    dst->height = src->height;
    dst->stride = src->stride;
}

static uint32_t Align16(uint32_t value)
{
    return (value + 0xf) & ~0xf;
}

static uint32_t Align64(uint32_t value)
{
    return (value + 0x3f) & ~0x3f;
}

static int32_t SetVdecAttr(NativeVdecContext *vdecCtx, const Param *attr, int32_t len)
{
    ext_vdec_attr vdecAttr = {0};
    int32_t ret = uapi_vdec_get_attr(vdecCtx->vdecHandle, &vdecAttr);
    CHK_FAILED_RETURN(ret, 0, MEDIA_HAL_ERR, "uapi_vdec_get_attr failed.");

    uint32_t height = 0;
    uint32_t width = 0;
    for (int32_t i = 0; i < len; i++) {
        if (attr[i].key == KEY_MIMETYPE) {
            if (attr[i].size != sizeof(AvCodecMime) || attr[i].val == NULL) {
                break;
            }
            AvCodecMime upperMime = *((AvCodecMime *)(attr[i].val));
            if (upperMime == MEDIA_MIMETYPE_VIDEO_AVC) {
                MEDIA_HAL_LOGI(MODULE_NAME, "set format EXT_VDEC_STD_H264.");
                vdecAttr.standard = EXT_VDEC_STD_H264;
            } else if (upperMime == MEDIA_MIMETYPE_VIDEO_HEVC) {
                MEDIA_HAL_LOGI(MODULE_NAME, "set format EXT_VDEC_STD_H265.");
                vdecAttr.standard = EXT_VDEC_STD_H265;
            } else if (upperMime == MEDIA_MIMETYPE_IMAGE_MJPEG) {
                MEDIA_HAL_LOGI(MODULE_NAME, "set format EXT_VDEC_STD_MJPEG.");
                vdecAttr.standard = EXT_VDEC_STD_MJPEG;
            }
        } else if (attr[i].key == KEY_HEIGHT) {
            if (attr[i].size == sizeof(uint32_t) && attr[i].val != NULL) {
                height = *((uint32_t *)(attr[i].val));
            }
        } else if (attr[i].key == KEY_WIDTH) {
            if (attr[i].size == sizeof(uint32_t) && attr[i].val != NULL) {
                width = *((uint32_t *)(attr[i].val));
            }
        }
    }
    vdecAttr.decode_mode = EXT_VDEC_DEC_MODE_IPB;
    vdecAttr.frame_buf_mode = EXT_VDEC_FRM_EXTERNAL;
    vdecAttr.output_order = EXT_VDEC_OUT_ORDER_DISPLAY;
    vdecAttr.frame_layout = EXT_VDEC_FRM_LAYOUT_LINEAR;
    uint32_t maxYUVSize = height * width * 3 / 2; // 3, 2 : YUV420 SIZE
    // 调小buffer大小,按照最小10倍压缩率 及最小buffer个数计算
    vdecAttr.max_stream_size = maxYUVSize / COMPRESSION_RATIO * INPUT_BUFFER_MIN;
    vdecAttr.max_stream_num = 100; /* 100 is nmax stream num */
    vdecAttr.extra_frame_num = 1; /* 2 is num extern frame num */
    vdecAttr.error_threshold = 100; /* 100 is num */
    vdecAttr.max_dec_w_h.is_max_dec_w_h_en = TD_TRUE;
    if (vdecAttr.standard == EXT_VDEC_STD_H264) {
        vdecAttr.max_dec_w_h.max_dec_width = Align16(width);
        vdecAttr.max_dec_w_h.max_dec_height = Align16(height);
    } else if (vdecAttr.standard == EXT_VDEC_STD_H265) {
        vdecAttr.max_dec_w_h.max_dec_width = Align64(width);
        vdecAttr.max_dec_w_h.max_dec_height = Align64(height);
    } else {
        vdecAttr.max_dec_w_h.max_dec_width = width;
        vdecAttr.max_dec_w_h.max_dec_height = height;
    }
    MEDIA_HAL_LOGI(MODULE_NAME, "set res %u * %u.max_stream_size:%u max_stream_num:%u",
        vdecAttr.max_dec_w_h.max_dec_width, vdecAttr.max_dec_w_h.max_dec_height, vdecAttr.max_stream_size, vdecAttr.max_stream_num);

    ret = uapi_vdec_set_attr(vdecCtx->vdecHandle, &vdecAttr);
    return ret;
}

static int32_t NativeVdecCreate(void **vdecHdl, const Param *attr, int32_t len)
{
    MEDIA_HAL_LOGI(MODULE_NAME, "create vdec.");
    CHK_NULL_RETURN(vdecHdl, MEDIA_HAL_ERR, "vdecHdl null");
    MEDIA_HAL_UNUSED(attr);
    MEDIA_HAL_UNUSED(len);

    NativeVdecContext *vdecCtx = (NativeVdecContext *)MediaHalMalloc(sizeof(NativeVdecContext));
    CHK_NULL_RETURN(vdecCtx, MEDIA_HAL_ERR, "malloc failed");

    int32_t ret = memset_s(vdecCtx, sizeof(NativeVdecContext), 0, sizeof(NativeVdecContext));
    if (ret != 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memset_s failed. ret = %x", ret);
        goto FREE_CONTEXT;
    }

    ret = InitBufferVector(&vdecCtx->externalBufferVec);
    if (ret != 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi_vdec_create failed. ret = %x", ret);
        goto FREE_BUFFER_VEC;
    }

    uapi_vdec_module_init();

    ret = uapi_vdec_create(&vdecCtx->vdecHandle);
    if (ret != 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi_vdec_create failed. ret = %x", ret);
        goto FREE_BUFFER_VEC;
    }

    ret = SetVdecAttr(vdecCtx, attr, len);
    if (ret != 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "SetVdecAttr failed. ret = %x", ret);
        goto DESTROY_VDEC;
    }

    td_u64 eventMap = EXT_VDEC_EVT_SIZE_CHG | EXT_VDEC_EVT_EOS;
    ret = uapi_vdec_register_event(vdecCtx->vdecHandle, eventMap);
    if (ret != 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi_vdec_register_event failed. ret = %x", ret);
        goto DESTROY_VDEC;
    }

    MediaHalMutexAttr mutexAttr = { false };
    vdecCtx->externalBufferVec.vectorMutex = MediaHalMutexCreate(&mutexAttr);

    *vdecHdl = vdecCtx;
    return MEDIA_HAL_OK;
DESTROY_VDEC:
    uapi_vdec_destroy(vdecCtx->vdecHandle);
FREE_BUFFER_VEC:
    uapi_vdec_module_exit();
    DeinitBufferVector(&vdecCtx->externalBufferVec);
FREE_CONTEXT:
    MediaHalFree(vdecCtx);
    return MEDIA_HAL_ERR;
}

static int32_t NativeVdecDestroy(void *nativeVdecHdl)
{
    MEDIA_HAL_LOGI(MODULE_NAME, "destroy vdec.");
    CHK_NULL_RETURN(nativeVdecHdl, MEDIA_HAL_ERR, "vdec handle is NULL");

    NativeVdecContext *vdecCtx = (NativeVdecContext *)nativeVdecHdl;

    int32_t ret = uapi_vdec_destroy(vdecCtx->vdecHandle);
    CHK_FAILED_NO_RETURN(ret, 0, "uapi_vdec_destroy failed.");

    DeinitBufferVector(&vdecCtx->externalBufferVec);

    MediaHalMutexDestroy(&vdecCtx->externalBufferVec.vectorMutex);

    MediaHalFree(vdecCtx);

    uapi_vdec_module_exit();

    MEDIA_HAL_LOGI(MODULE_NAME, "vdec destroyed.");
    return MEDIA_HAL_OK;
}

static int32_t NativeVdecStart(void *nativeVdecHdl)
{
    MEDIA_HAL_LOGI(MODULE_NAME, "vdec start.");
    CHK_NULL_RETURN(nativeVdecHdl, MEDIA_HAL_ERR, "vdecHdl null");
    NativeVdecContext *vdecCtx = (NativeVdecContext *)nativeVdecHdl;
    CHK_NULL_RETURN(vdecCtx->callback.OnEvent, MEDIA_HAL_ERR, "vdecCtx->callback null");

    int32_t ret = uapi_vdec_start(vdecCtx->vdecHandle);
    CHK_FAILED_RETURN(ret, 0, MEDIA_HAL_ERR, "uapi_vdec_start failed.");
    MEDIA_HAL_LOGI(MODULE_NAME, "vdec started.");
    return ret;
}

static int32_t NativeVdecStop(void *nativeVdecHdl)
{
    MEDIA_HAL_LOGI(MODULE_NAME, "vdec stop.");
    NativeVdecContext *vdecCtx = (NativeVdecContext *)nativeVdecHdl;
    CHK_NULL_RETURN(vdecCtx, MEDIA_HAL_ERR, "vdecCtx null");

    int32_t ret = uapi_vdec_stop(vdecCtx->vdecHandle);
    CHK_FAILED_RETURN(ret, 0, MEDIA_HAL_ERR, "uapi_vdec_stop failed.");

    MEDIA_HAL_LOGI(MODULE_NAME, "vdec stoped.");
    return MEDIA_HAL_OK;
}

static void CheckVdecEvent(NativeVdecContext *vdecCtx)
{
    ext_vdec_event vdecEvent = {};
    ext_vdec_opt_listen eventOption = {
        .time_out = 0,
    };
    vdecEvent.event_id = EXT_VDEC_EVT_SIZE_CHG;
    int32_t ret = uapi_vdec_listen_event(vdecCtx->vdecHandle, &vdecEvent, &eventOption);
    if (ret == 0 && vdecEvent.event_id == EXT_VDEC_EVT_SIZE_CHG) {
        FormatChange formatChangeEvent = {
            .direct = OUTPUT_TYPE,
            .bufferNum = vdecEvent.event_param.size_change.new_frame_num,
            .width = vdecEvent.event_param.size_change.new_width,
            .height = vdecEvent.event_param.size_change.new_height,
            .widthStride = vdecEvent.event_param.size_change.new_stride,
            .heightStride = vdecEvent.event_param.size_change.new_height,
            .bufferSize = vdecEvent.event_param.size_change.new_frame_size,
            .flag = 0,
        };
        if (vdecEvent.event_param.size_change.new_splice_info.type == EXT_VDEC_SPLICE_AYUV_TAB) {
            formatChangeEvent.flag |= STREAM_FLAG_SPLICE_AYUV_TAB;
        }
        vdecCtx->callback.OnEvent((uintptr_t)NULL, vdecCtx->callbackInstance, EVENT_OUT_FORMAT_CHANGED, 0, 0,
            (uintptr_t)&formatChangeEvent);
        vdecCtx->changedEventReceived = true;
    }
}

static int32_t NativeVdecSendStream(void *nativeVdecHdl, const InputInfo *vdecStream, uint32_t timeout)
{
    NativeVdecContext *vdecCtx = (NativeVdecContext *)nativeVdecHdl;
    CHK_NULL_RETURN(vdecCtx, MEDIA_HAL_ERR, "vdecCtx null");
    CHK_NULL_RETURN(vdecStream, MEDIA_HAL_ERR, "vdecStream null");
    MEDIA_HAL_UNUSED(timeout);

    CheckVdecEvent(vdecCtx);

    if (vdecStream->flag == STREAM_FLAG_EOS) {
        vdecCtx->receiveEos = true;
        MEDIA_HAL_LOGI(MODULE_NAME, "received input eos");
        uapi_vdec_send_eos(vdecCtx->vdecHandle);
        return MEDIA_HAL_OK;
    }

    ext_vdec_stream freeInputStream = {};
    ext_vdec_opt_get_stream opt = {
        .expect_size = vdecStream->buffers->length,
    };
    int32_t ret = 0;
    if (vdecCtx->isStreamSource) {
        ext_vdec_status status = {};
        ret = uapi_vdec_get_status(vdecCtx->vdecHandle, &status);
        CHK_FAILED_RETURN(ret, 0, MEDIA_HAL_ERR, "uapi_vdec_get_status failed.");
        if (vdecCtx->changedEventReceived &&
            status.stream_state.undecode_stream_buffer_size >= status.stream_state.total_stream_buffer_size / FLUSH_THRESHOLD) {
            NativeVdecReset(vdecCtx);
        }
        MEDIA_HAL_LOGD(MODULE_NAME, "status total_size:%u free_size:%u used_size:%u undecode_size:%u",
            status.stream_state.total_stream_buffer_size, status.stream_state.free_stream_buffer_size,
            status.stream_state.used_stream_buffer_size, status.stream_state.undecode_stream_buffer_size);
    }

    ret = uapi_vdec_get_stream(vdecCtx->vdecHandle, &freeInputStream, &opt);
    if (ret != 0) {
        return VDEC_ERR_STREAM_BUF_FULL;
    }

    ret = memcpy_s(freeInputStream.vir_addr, freeInputStream.length,
        vdecStream->buffers->addr, vdecStream->buffers->length);
    CHK_FAILED_NO_RETURN(ret, 0, "copy input stream failed.");
    freeInputStream.pts = vdecStream->pts;
    ret = uapi_vdec_put_stream(vdecCtx->vdecHandle, &freeInputStream);
    CHK_FAILED_RETURN(ret, 0, MEDIA_HAL_ERR, "uapi_vdec_put_stream failed.");
    return MEDIA_HAL_OK;
}

static int32_t NativeVdecGetStatus(void *nativeVdecHdl, AvVdecStatus *vdecStatus)
{
    MEDIA_HAL_UNUSED(nativeVdecHdl);
    MEDIA_HAL_UNUSED(vdecStatus);
    return MEDIA_HAL_OK;
}

static int32_t NativeVdecReset(void *nativeVdecHdl)
{
    MEDIA_HAL_LOGI(MODULE_NAME, "vdec reset.");
    CHK_NULL_RETURN(nativeVdecHdl, MEDIA_HAL_ERR, "vdecHdl null");
    NativeVdecContext *vdecCtx = (NativeVdecContext *)nativeVdecHdl;
    int32_t ret = uapi_vdec_stop(vdecCtx->vdecHandle);
    CHK_FAILED_RETURN(ret, 0, MEDIA_HAL_ERR, "uapi_vdec_stop failed.");
    ext_vdec_opt_flush option = {};
    option.type = EXT_VDEC_FLUSH_ALL;
    ret = uapi_vdec_flush(vdecCtx->vdecHandle, &option);
    CHK_FAILED_RETURN(ret, 0, MEDIA_HAL_ERR, "uapi_vdec_flush failed.");
    ret = uapi_vdec_start(vdecCtx->vdecHandle);
    CHK_FAILED_RETURN(ret, 0, MEDIA_HAL_ERR, "uapi_vdec_start failed.");
    vdecCtx->receiveEos = false;
    return MEDIA_HAL_OK;
}

static int32_t NativeVdecSetWorkScene(void *nativeVdecHdl, void *arg1, void *arg2)
{
    MEDIA_HAL_UNUSED(arg2);
    CHK_NULL_RETURN(nativeVdecHdl, MEDIA_HAL_INVALID_PARAM, "vdecCtx null");
    CHK_NULL_RETURN(arg1, MEDIA_HAL_INVALID_PARAM, "arg1 null");
    NativeVdecContext *vdecCtx = (NativeVdecContext *)nativeVdecHdl;
    bool isStreamSource = *(bool *)arg1;
    vdecCtx->isStreamSource = isStreamSource;
    MEDIA_HAL_LOGI(MODULE_NAME, "set workscene isStreamSource:%d", isStreamSource);
    return MEDIA_HAL_OK;
}

static int32_t NativeVdecInvoke(void *nativeVdecHdl, uint32_t cmd, void *arg1, void *arg2)
{
    switch (cmd) {
        case VDEC_CMD_SETSCENE:
            return NativeVdecSetWorkScene(nativeVdecHdl, arg1, arg2);
        default:
            MEDIA_HAL_LOGD(MODULE_NAME, "invalid cmd %d", cmd);
    }
    return MEDIA_HAL_OK;
}

static int32_t NativeVdecQueueOutBuf(void *nativeVdecHdl, OutputInfo *outInfo, int32_t releaseFenceFd)
{
    MEDIA_HAL_UNUSED(releaseFenceFd);
    NativeVdecContext *vdecCtx = (NativeVdecContext *)nativeVdecHdl;
    CHK_NULL_RETURN(vdecCtx, MEDIA_HAL_INVALID_PARAM, "vdecCtx null");
    CHK_NULL_RETURN(outInfo, MEDIA_HAL_INVALID_PARAM, "outInfo null");
    // 这里要求 buffers 所指向的内存由调用方分配
    CHK_NULL_RETURN(outInfo->buffers, MEDIA_HAL_ERR, "out buf is NULL");
    // 外部vector里面没有的buffer，就保存到vector中并bind到vdec，否则调用release

    int32_t bufferIndex = -1;
    MediaHalMutexLock(vdecCtx->externalBufferVec.vectorMutex);
    BufferVector *vec = &vdecCtx->externalBufferVec;
    for (uint32_t j = 0; j < vec->usedSize && j < MAX_BUFFER_NUM; j++) {
        if ((uint8_t *)vec->bufs[j].addr == outInfo->buffers[0].addr) {
            bufferIndex = j;
            break;
        }
    }
    MediaHalMutexUnLock(vdecCtx->externalBufferVec.vectorMutex);
    if (bufferIndex != -1) {
        int32_t ret = uapi_vdec_release_frame(vdecCtx->vdecHandle, vec->bufs[bufferIndex].vdecFrame);
        CHK_FAILED_RETURN(ret, 0, MEDIA_HAL_ERR, "uapi_vdec_release_frame failed.");
    } else {
        if (vec->usedSize >= MAX_BUFFER_NUM) {
            MEDIA_HAL_LOGE(MODULE_NAME, "can't bind more buffer to vdec.");
            return MEDIA_HAL_ERR;
        }
        ext_vdec_frame_buf buffer = {
            .phy_addr = (uintptr_t)outInfo->buffers[0].addr,
            .frame_size = outInfo->buffers[0].length,
        };
        MEDIA_HAL_LOGI(MODULE_NAME, "vdec bind buffer. start");
        int32_t ret = uapi_vdec_bind_frame_buffer(vdecCtx->vdecHandle, &buffer);
        CHK_FAILED_RETURN(ret, 0, MEDIA_HAL_ERR, "uapi_vdec_bind_frame_buffer failed.");
        CopyOutputInfo(&vec->bufs[vec->usedSize].outBuf, outInfo);
        vec->bufs[vec->usedSize].addr = (uintptr_t)outInfo->buffers[0].addr;
        vec->usedSize++;
    }
    return MEDIA_HAL_OK;
}

static int32_t NativeVdecDeqOutBuf(void *nativeVdecHdl, OutputInfo *outInfo, uint32_t timeout, int32_t *acquireFd)
{
    MEDIA_HAL_UNUSED(acquireFd);
    MEDIA_HAL_UNUSED(timeout);
    NativeVdecContext *vdecCtx = (NativeVdecContext *)nativeVdecHdl;
    CHK_NULL_RETURN(vdecCtx, MEDIA_HAL_ERR, "vdecCtx null");

    ext_vdec_event vdecEvent = {};
    ext_vdec_opt_listen eventOption = {
        .time_out = 0,
    };
    vdecEvent.event_id = EXT_VDEC_EVT_EOS;
    int32_t ret = uapi_vdec_listen_event(vdecCtx->vdecHandle, &vdecEvent, &eventOption);
    if (ret == 0 && vdecEvent.event_id == EXT_VDEC_EVT_EOS) {
        MEDIA_HAL_LOGI(MODULE_NAME, "receive eos event from vdec.");
        return VDEC_FRAME_BUF_EOS;
    }

    CHK_NULL_RETURN(outInfo, MEDIA_HAL_ERR, "outInfo null");
    // 这里要求 buffers 所指向的内存由调用方分配
    CHK_NULL_RETURN(outInfo->buffers, false, "out buf is NULL");

    ext_vdec_frame frame = {};
    ext_vdec_opt_acq_frame acquireOpt = {
        .time_out = 0,
    };

    ret = uapi_vdec_acquire_frame(vdecCtx->vdecHandle, &frame, &acquireOpt);
    if (ret != 0) {
        MEDIA_HAL_LOGD(MODULE_NAME, "no frame data in vdec.");
        return MEDIA_HAL_ERR;
    }

    // 匹配外部buffervector中的地址，并记录下ext_vdec_frame的地址，以便后续release
    MediaHalMutexLock(vdecCtx->externalBufferVec.vectorMutex);
    BufferVector *vec = &vdecCtx->externalBufferVec;
    for (uint32_t i = 0; i < vec->usedSize && i < MAX_BUFFER_NUM; i++) {
        if (vec->bufs[i].addr == frame.frame_addr.start_addr) {
            *(vec->bufs[i].vdecFrame) = frame;
            vec->bufs[i].outBuf.buffers[0].uvOffset = frame.frame_addr.offset_c;
            vec->bufs[i].outBuf.timeStamp = frame.pts;
            vec->bufs[i].outBuf.width = frame.width;
            vec->bufs[i].outBuf.height = frame.height;
            vec->bufs[i].outBuf.stride = frame.frame_addr.stride_y;
            CopyOutputInfo(outInfo, &vec->bufs[i].outBuf);
            MEDIA_HAL_LOGI(MODULE_NAME, "vdec match addr = %p, surface addr = %p, bufferCnt = %d splice type = %d",
                vec->bufs[i].addr, vec->bufs[i].outBuf.vendorPrivate, vec->bufs[i].outBuf.bufferCnt,
                frame.splice_info.type);
            break;
        }
    }
    MediaHalMutexUnLock(vdecCtx->externalBufferVec.vectorMutex);
    return MEDIA_HAL_OK;
}

static int32_t NativeVdecSetCallback(void *nativeVdecHdl, const CodecCallback *cb, uintptr_t instance)
{
    NativeVdecContext *vdecCtx = (NativeVdecContext *)nativeVdecHdl;
    CHK_NULL_RETURN(cb, MEDIA_HAL_ERR, "callback null");

    vdecCtx->callback = *cb;
    vdecCtx->callbackInstance = instance;

    return MEDIA_HAL_OK;
}

void *VDecoderGetImpl(void)
{
    AvVideoDecoder *decoder = (AvVideoDecoder *)MediaHalMalloc(sizeof(AvVideoDecoder));
    if (decoder == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "malloc failed");
        return NULL;
    }
    if (memset_s(decoder, sizeof(AvVideoDecoder), 0x00, sizeof(AvVideoDecoder)) != EOK) {
        MediaHalFree(decoder);
        MEDIA_HAL_LOGE(MODULE_NAME, "memset_s failed");
        return NULL;
    }

    decoder->pluginCommon.pfnCreate = NativeVdecCreate;
    decoder->pluginCommon.isHardwarePlugin = true;
    decoder->pluginCommon.pfnDestroy = NativeVdecDestroy;
    decoder->pluginCommon.pfnStart = NativeVdecStart;
    decoder->pluginCommon.pfnStop = NativeVdecStop;
    decoder->pfnQueueInput = NativeVdecSendStream;
    decoder->pfnDequeueOutput = NativeVdecDeqOutBuf;
    decoder->pfnQueueOutput = NativeVdecQueueOutBuf;
    decoder->pfnGetStatus = NativeVdecGetStatus;
    decoder->pluginCommon.pfnReset = NativeVdecReset;
    decoder->pluginCommon.pfnInvoke = NativeVdecInvoke;
    decoder->pluginCommon.pfnSetCodecCallback = NativeVdecSetCallback;

    int32_t validCapNum = sizeof(g_vdecCapEntries) / sizeof(g_vdecCapEntries[0]);
    decoder->pluginCommon.validCapbilityNum = validCapNum;
    for (int32_t i = 0; i < validCapNum; i++) {
        decoder->pluginCommon.capbilites[i] = g_vdecCapEntries[i];
    }

    return decoder;
}
