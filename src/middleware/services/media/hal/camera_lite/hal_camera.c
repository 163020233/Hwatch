/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2026. All rights reserved.
 * Description: camera hal adapter
 */

#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include "securec.h"
#include "td_type.h"
#include "soc_vdec_api.h"
#include "drv_vdec_intf.h"
#include "hal_camera_common.h"
#include "hal_camera_config.h"
#include "hal_camera_pipe.h"
#include "hal_camera.h"

#define CAMERA_PRINT_FPS_SUPPORT 1
#define CAMERA_PRINT_FPS_INTERVAL_MS 3000
#define CAMERA_LOCK_DEBUG 0
#define CAMERA_STREAM_WAIT_INTERVAL_MS 10
#define CAMERA_STREAM_PUT_TIMEOUT_MS 30
#define CAMERA_NO_STREAM_LOG_TIMEOUT_MS 3000

typedef enum {
    STREAM_STAT_IDLE,
    STREAM_STAT_CREATED,
    STREAM_STAT_STREAM_ON,
} stream_status;

typedef enum {
    STREAM_BUF_NONE,
    STREAM_BUF_QUEUED,
    STREAM_BUF_FILLED,
} stream_buf_status;

typedef struct {
    ext_vdec_frame_buf bind_buf;
    ext_vdec_frame frame;
} vdec_frame;

typedef struct {
    td_handle handle;
    ext_vdec_stream stream;
    bool got_stream; // got stream from vdec
    vdec_frame frames[CAM_CFG_VDEC_MAX_BUF]; // for output frame
} vdec_information;

typedef struct {
    stream_status stream_stat;
    stream_buf_status buf_stat;
    HalBuffer out_stream_buffer;
    bool skip_frame; // process packet error, all packets in this frame should be skipped.
    vdec_information *vdec_info;
    pthread_mutex_t stream_lock;
#if CAMERA_PRINT_FPS_SUPPORT
    uint32_t frame_count;
    uint32_t frame_count_start_time; // ms
#endif
} cam_stream;

typedef struct {
    bool is_pipe_on;
    bool is_thread_created;
    bool need_thread_exit;
    pthread_t thread;
    pthread_mutex_t dev_lock;
    cam_stream streams[CAM_CFG_STREAM_MAX_NUM];
} cam_dev;

static cam_dev g_cam_dev; // only one device now
static const ImageFormat g_supported_fmts[] = CAM_CFG_STREAM_SUPPORT_FORMATS;

#if CAMERA_LOCK_DEBUG
#define cam_lock(mutex) do { \
    CAM_DBG("%s() lock(%s) enter\n", __func__, #mutex); \
    pthread_mutex_lock(mutex); \
    CAM_DBG("%s() lock(%s) exit\n", __func__, #mutex); \
} while (0)

#define cam_unlock(mutex) do { \
    CAM_DBG("%s() unlock(%s)\n", __func__, #mutex); \
    pthread_mutex_unlock(mutex); \
} while (0)
#else
static inline void cam_lock(pthread_mutex_t *mutex)
{
    int ret = pthread_mutex_lock(mutex);
    if (ret != 0) {
        CAM_ERR("lock err\n");
    }
}

static inline void cam_unlock(pthread_mutex_t *mutex)
{
    int ret = pthread_mutex_unlock(mutex);
    if (ret != 0) {
        CAM_ERR("lock err\n");
    }
}
#endif

static inline int get_stream_id(const cam_stream *stream)
{
    return stream - (const cam_stream *)g_cam_dev.streams;
}

#if CAMERA_PRINT_FPS_SUPPORT
static uint32_t get_time_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint32_t)(ts.tv_sec * 1000UL) + (ts.tv_nsec / 1000000UL); // 1000: s to ms, 1000000: ns to ms
}

static void stat_frame_fps(cam_stream *stream)
{
    uint32_t cur_time = get_time_ms();
    if (stream->frame_count_start_time == 0) {
        stream->frame_count_start_time = cur_time;
        stream->frame_count = 0;
        return;
    }

    stream->frame_count++;
    uint32_t total_time = cur_time - stream->frame_count_start_time;
    if (total_time > CAMERA_PRINT_FPS_INTERVAL_MS) {
        float fps = (float)stream->frame_count * 1000 / total_time; // 1000 s per ms
        CAM_INFO("camera stream[%d] FPS: %.1f\n", get_stream_id(stream), fps);
        stream->frame_count_start_time = cur_time;
        stream->frame_count = 0;
    }
}
#endif

// return copyed length
static int copy_venc_stream_to_buf(uint8_t *dst, uint32_t dst_size, const ext_venc_stream *src)
{
    int copyed_len = 0;
    for (int i = 0; i < EXT_VENC_MAX_STREAM_PACK_NUM; i++) {
        if (src->len[i] == 0) {
            continue;
        }
        IF_FUNC_ERR(memcpy_s(&dst[copyed_len], dst_size - copyed_len, (void*)(uintptr_t)src->vir_addr[i],
            src->len[i]), return 0, "venc memcpy err, %d, %d, %d", dst_size, copyed_len, src->len[i]);
        copyed_len += src->len[i];
    }
    return copyed_len;
}

static int vdec_create(vdec_information **vdec)
{
    vdec_information *vdec_info = malloc(sizeof(vdec_information));
    ERR_IF(vdec_info == NULL, return -1, "malloc vdec info err");
    (void)memset_s(vdec_info, sizeof(vdec_information), 0, sizeof(vdec_information));

    ext_vdec_attr vdec_attr = {};
    IF_FUNC_ERR(uapi_vdec_create(&vdec_info->handle), goto err_free, "vdec create err");
    IF_FUNC_ERR(uapi_vdec_get_attr(vdec_info->handle, &vdec_attr), goto err_destroy, "get vdec attr err");
    vdec_attr.standard = EXT_VDEC_STD_MJPEG;
    vdec_attr.decode_mode = EXT_VDEC_DEC_MODE_IPB;
    vdec_attr.frame_buf_mode = EXT_VDEC_FRM_EXTERNAL;
    vdec_attr.output_order = EXT_VDEC_OUT_ORDER_DISPLAY;
    vdec_attr.frame_layout = EXT_VDEC_FRM_LAYOUT_LINEAR;
    vdec_attr.max_stream_size = CAM_CFG_JPEG_BUF_MAX;
    vdec_attr.max_stream_num = 1;
    vdec_attr.extra_frame_num = 1;
    vdec_attr.error_threshold = 100; // 100%: output all frame even has errors
    IF_FUNC_ERR(uapi_vdec_set_attr(vdec_info->handle, &vdec_attr), goto err_destroy, "set vdec attr err");
    IF_FUNC_ERR(uapi_vdec_start(vdec_info->handle), goto err_destroy, "vdec start err");
    *vdec = vdec_info;
    return 0;

err_destroy:
    LOG_FUNC_ERR(uapi_vdec_destroy(vdec_info->handle), "vdec destroy err");
err_free:
    if (vdec_info != NULL) {
        free(vdec_info);
    }
    return -1;
}

static int vdec_input_stream(vdec_information *vdec_info, const ext_venc_stream *in)
{
    ext_vdec_opt_get_stream stream_opt = {
        .expect_size = CAM_CFG_JPEG_BUF_MAX
    };
    if (!vdec_info->got_stream) {
        (void)memset_s(&vdec_info->stream, sizeof(vdec_info->stream), 0, sizeof(vdec_info->stream));
        IF_FUNC_ERR(uapi_vdec_get_stream(vdec_info->handle, &vdec_info->stream, &stream_opt),
            goto err, "vdec get stream err");
        vdec_info->stream.length = 0;
        vdec_info->got_stream = true;
    }

    int copyed_len = copy_venc_stream_to_buf(&vdec_info->stream.vir_addr[vdec_info->stream.length],
        CAM_CFG_JPEG_BUF_MAX - vdec_info->stream.length, in);
    if (copyed_len <= 0) {
        CAM_ERR("vdec stream memcpy err\n");
        goto err;
    }
    vdec_info->stream.length += copyed_len;
    if (in->is_frame_end) {
        vdec_info->stream.is_end_of_frame = true;
        IF_FUNC_ERR(uapi_vdec_put_stream(vdec_info->handle, &vdec_info->stream), goto err, "vdec put stream err");
        vdec_info->got_stream = false;
    }
    CAM_DBG("vdec put stream ok\n");
    return 0;

err:
    vdec_info->stream.length = 0;
    return -1;
}

static int vdec_destroy(vdec_information **vdec)
{
    vdec_information *vdec_info = *vdec;
    LOG_FUNC_ERR(uapi_vdec_stop(vdec_info->handle), "vdec stop err");
    LOG_FUNC_ERR(uapi_vdec_destroy(vdec_info->handle), "vdec destroy err");
    free(vdec_info);
    *vdec = NULL;
    return 0;
}

static inline bool is_support_stream_format(ImageFormat format)
{
    for (uint32_t i = 0; i < sizeof(g_supported_fmts) / sizeof(g_supported_fmts[0]); i++) {
        if (format == g_supported_fmts[i]) {
            return true;
        }
    }
    return false;
}

static uint32_t get_stream_on_count(void)
{
    uint32_t count = 0;
    for (uint32_t i = 0; i < CAM_CFG_STREAM_MAX_NUM; i++) {
        cam_stream *stream = &g_cam_dev.streams[i];
        cam_lock(&stream->stream_lock);
        if (stream->stream_stat == STREAM_STAT_STREAM_ON) {
            count++;
        }
        cam_unlock(&stream->stream_lock);
    }
    return count;
}

static const char *get_skip_frame_reason(const cam_stream *stream)
{
    if (stream->buf_stat == STREAM_BUF_NONE) {
        return "buf not queued";
    } else if (stream->buf_stat == STREAM_BUF_FILLED) {
        if (stream->vdec_info == NULL) {
            return "wait last buf dequeue";
        }
        ext_vdec_status stat = {};
        LOG_FUNC_ERR(uapi_vdec_get_status(stream->vdec_info->handle, &stat), "get vdec stat failed");
        if (stat.frame_state.wait_output_frame_num == 0) {
            return "wait last frame decode";
        }
        return "wait last buf dequeue";
    }
    return "unknown";
}

static int32_t wait_stream_buf_queue(const cam_stream *stream, int32_t timeout_ms)
{
    int32_t wait_time_ms = 0;
    while (stream->buf_stat != STREAM_BUF_QUEUED) {
        if (wait_time_ms >= timeout_ms) {
            const char *reason = get_skip_frame_reason(stream);
            CAM_WARN("stream[%d] skipped frame, %s\n", get_stream_id(stream), reason);
            CAM_TRACE_LOG("skip frame, %s", reason);
            return -1;
        }
        usleep(CAMERA_STREAM_WAIT_INTERVAL_MS * 1000); // 1000: ms to us
        wait_time_ms += CAMERA_STREAM_WAIT_INTERVAL_MS;
    }
    return 0;
}

static void packet_to_stream(const ext_venc_stream *in, cam_stream *stream, int32_t timeout_ms)
{
    cam_lock(&stream->stream_lock);
    if (stream->stream_stat != STREAM_STAT_STREAM_ON) {
        goto out;
    }

    if (stream->skip_frame) {
        if (in->is_frame_end) {
            stream->skip_frame = false;
        }
        goto out;
    }

    cam_unlock(&stream->stream_lock);
    int32_t ret = wait_stream_buf_queue(stream, timeout_ms);
    cam_lock(&stream->stream_lock);
    if (ret != 0) {
        goto err;
    }

    if (stream->vdec_info == NULL) {
        HalBuffer *hal_buf = &stream->out_stream_buffer;
        uint8_t *dst = (uint8_t *)hal_buf->virAddr;
        int copyed_len = copy_venc_stream_to_buf(&dst[hal_buf->dataLen], hal_buf->size - hal_buf->dataLen, in);
        if (copyed_len <= 0) {
            CAM_ERR("copy mjpeg err\n");
            goto err;
        }
        hal_buf->dataLen += copyed_len;
        CAM_DBG("stream[%d] copyed\n", get_stream_id(stream));
    } else {
        IF_FUNC_ERR(vdec_input_stream(stream->vdec_info, in), goto err, "vdec input stream err");
    }

    if (in->is_frame_end) {
        stream->buf_stat = STREAM_BUF_FILLED;
    }
out:
    cam_unlock(&stream->stream_lock);
    return;

err:
    if (!in->is_frame_end) {
        stream->skip_frame = true;
    }
    cam_unlock(&stream->stream_lock);
}

static void packet_to_streams(const ext_venc_stream *in)
{
    cam_lock(&g_cam_dev.dev_lock);
    if (!g_cam_dev.is_pipe_on) {
        cam_unlock(&g_cam_dev.dev_lock);
        return;
    }
    for (uint32_t i = 0; i < CAM_CFG_STREAM_MAX_NUM; i++) {
        cam_stream *stream = &g_cam_dev.streams[i];
        CAM_TRACE_BEGIN("packet to stream");
        packet_to_stream(in, stream, CAMERA_STREAM_PUT_TIMEOUT_MS);
        CAM_TRACE_END();
    }
    cam_unlock(&g_cam_dev.dev_lock);
}

static void *stream_thread(void *arg)
{
    UNUSED(arg);
    cam_dev *dev = &g_cam_dev;
    ext_venc_stream buf = {};
    int32_t no_stream_time_ms = 0;

    while (!g_cam_dev.need_thread_exit) {
        cam_lock(&dev->dev_lock);
        if (!dev->is_pipe_on) {
            cam_unlock(&dev->dev_lock);
            CAM_DBG("stream loop pipe is off\n");
            usleep(CAMERA_STREAM_WAIT_INTERVAL_MS * 1000); // 1000: ms to us
            continue;
        }
        cam_unlock(&dev->dev_lock);

        if (cam_pipe_get_stream(0, &buf, CAM_CFG_VENC_WAIT_TIMEOUT_MS) != 0) {
            CAM_DBG("pipe get stream err, maybe timeout\n");
            no_stream_time_ms += CAM_CFG_VENC_WAIT_TIMEOUT_MS;
            if (no_stream_time_ms > CAMERA_NO_STREAM_LOG_TIMEOUT_MS) {
                CAM_WARN("no stream output more than %d ms\n", no_stream_time_ms);
                no_stream_time_ms = 0;
            }
            continue;
        }
        CAM_DBG("camera got pipe stream\n");
        no_stream_time_ms = 0;
        packet_to_streams(&buf);
        cam_pipe_put_stream(0, &buf);
    }
    CAM_INFO("stream thread exit\n");
    return NULL;
}

static int32_t vdec_queue_frame(vdec_information *vdec_info, const HalBuffer *buf)
{
    uint32_t addr = (uint32_t)(uintptr_t)buf->virAddr;
    int32_t i;
    CAM_TRACE_BEGIN("VDEC-QBUF");
    for (i = 0; i < CAM_CFG_VDEC_MAX_BUF; i++) {
        if (vdec_info->frames[i].bind_buf.phy_addr == addr) {
            IF_FUNC_ERR(uapi_vdec_release_frame(vdec_info->handle, &vdec_info->frames[i].frame),
                goto err, "vdec release err");
            CAM_TRACE_END();
            return 0;
        }
    }
    // add new bind buffer
    for (i = 0; i < CAM_CFG_VDEC_MAX_BUF; i++) {
        ext_vdec_frame_buf *bind_buf = &vdec_info->frames[i].bind_buf;
        if (bind_buf->phy_addr != 0) {
            continue;
        }
        bind_buf->phy_addr = addr;
        bind_buf->frame_size = buf->size;
        CAM_INFO("bind new buffer to vdec: 0x*%04x, size: %u\n", bind_buf->phy_addr & 0xFFFF, bind_buf->frame_size);
        IF_FUNC_ERR(uapi_vdec_bind_frame_buffer(vdec_info->handle, bind_buf), break, "vdec bind frame err");
        CAM_TRACE_END();
        return 0;
    }
    CAM_ERR("vdec q frame error, no empty buffer position\n");
err:
    CAM_TRACE_END();
    return -1;
}

static int vdec_dequeue_frame(vdec_information *vdec_info, const HalBuffer *buf, int32_t timeout_ms)
{
    ext_vdec_opt_acq_frame acq_opt = {};
    uint32_t addr = (uintptr_t)buf->virAddr;
    acq_opt.time_out = timeout_ms;
    int32_t i;
    for (i = 0; i < CAM_CFG_VDEC_MAX_BUF; i++) {
        if (vdec_info->frames[i].bind_buf.phy_addr != addr) {
            continue;
        }
        CAM_DBG("vdec acquire frame start\n");
        ext_vdec_frame *frame = &vdec_info->frames[i].frame;
        CAM_TRACE_BEGIN("VDEC-DQBUF");
        if (uapi_vdec_acquire_frame(vdec_info->handle, frame, &acq_opt) != 0) {
            CAM_DBG("vdec acquire frame err\n");
            CAM_TRACE_END();
            return -1;
        }
        CAM_TRACE_END();
        CAM_DBG("vdec acquire frame OK\n");
        ERR_IF(frame->frame_addr.start_addr != addr, return -1, "vdec acq frame addr err");
        return 0;
    }
    CAM_ERR("vdec dq frame unknown addr, error!\n");
    return -1;
}

int32_t HalCameraInit(void)
{
    CAM_FW_ENTER();
    int32_t i = 0;
    (void)memset_s(&g_cam_dev, sizeof(g_cam_dev), 0, sizeof(g_cam_dev));
    for (; i < CAM_CFG_STREAM_MAX_NUM; i++) {
        IF_FUNC_ERR(pthread_mutex_init(&g_cam_dev.streams[i].stream_lock, NULL), goto err_stream, "mutex init err");
    }
    IF_FUNC_ERR(pthread_mutex_init(&g_cam_dev.dev_lock, NULL), goto err_stream, "mutex init err");

    IF_FUNC_ERR(cam_pipe_init(), goto err_dev, "cam pipe init err");
    IF_FUNC_ERR(uapi_vdec_module_init(), goto err_pipe, "vdec init err");
    CAM_FW_EXIT();
    return 0;

err_pipe:
    LOG_FUNC_ERR(cam_pipe_deinit(), "cam pipe deinit err");
err_dev:
    LOG_FUNC_ERR(pthread_mutex_destroy(&g_cam_dev.dev_lock), "mutex deinit err");
err_stream:
    for (i--; i >= 0; i--) {
        LOG_FUNC_ERR(pthread_mutex_destroy(&g_cam_dev.streams[i].stream_lock), "mutex deinit err");
    }
    return -1;
}

int32_t HalCameraDeinit(void)
{
    CAM_FW_ENTER();
    LOG_FUNC_ERR(cam_pipe_deinit(), "cam pipe deinit err");
    uapi_vdec_module_exit();

    LOG_FUNC_ERR(pthread_mutex_destroy(&g_cam_dev.dev_lock), "mutex deinit err");
    for (int32_t i = 0; i < CAM_CFG_STREAM_MAX_NUM; i++) {
        LOG_FUNC_ERR(pthread_mutex_destroy(&g_cam_dev.streams[i].stream_lock), "mutex deinit err");
    }
    CAM_FW_EXIT();
    return 0;
}

int32_t HalCameraGetModeNum(uint8_t *num)
{
    CAM_FW_ENTER();
    if (num != NULL) {
        *num = 0;
    }
    CAM_FW_EXIT();
    return 0;
}

int32_t HalCameraSetMode(uint8_t index)
{
    CAM_FW_ENTER();
    UNUSED(index);
    CAM_FW_EXIT();
    return 0;
}

int32_t HalCameraSetDeviceDetectCb(const CameraDetectCb cb)
{
    CAM_FW_ENTER();
    UNUSED(cb);
    CAM_FW_EXIT();
    return 0;
}

int32_t HalCameraGetDeviceNum(uint8_t *num)
{
    CAM_FW_ENTER();
    if (num != NULL) {
        *num = CAM_CFG_CAMERA_NUM;
    }
    CAM_FW_EXIT();
    return 0;
}

int32_t HalCameraGetDeviceList(uint32_t *cameraList, uint8_t listNum)
{
    CAM_FW_ENTER();
    // not support hotplug, so id is fixed
    for (uint32_t i = 0; i < listNum; i++) {
        cameraList[i] = i;
    }
    CAM_FW_EXIT();
    return 0;
}

int32_t HalCameraGetStreamCapNum(uint32_t cameraId, uint32_t *num)
{
    CAM_FW_ENTER();
    UNUSED(cameraId);
    if (num != NULL) {
        *num = sizeof(g_supported_fmts) / sizeof(g_supported_fmts[0]);
    }
    CAM_FW_EXIT();
    return 0;
}

int32_t HalCameraGetStreamCap(uint32_t cameraId, StreamCap *streamCap, uint32_t streamNum)
{
    CAM_FW_ENTER();
    UNUSED(cameraId);
    if (streamCap == NULL) {
        CAM_ERR("parameter error!\n");
        return -1;
    }
    for (uint32_t i = 0; i < streamNum && i < sizeof(g_supported_fmts) / sizeof(g_supported_fmts[0]); i++) {
        streamCap[i].type = CAP_DESC_ENUM;
        (void)strcpy_s(streamCap[i].description, CAMERA_DESC_MAX_LEN, CAM_CFG_CAM_DESC);
        streamCap[i].format = g_supported_fmts[i];
        streamCap[i].u.formatEnum.width = CAM_CFG_OUT_WIDTH;
        streamCap[i].u.formatEnum.height = CAM_CFG_OUT_HEIGHT;
        streamCap[i].u.formatEnum.frame_rate_num = 1;
        streamCap[i].u.formatEnum.frame_rate[0] = CAM_CFG_CAM_FPS;
    }
    CAM_FW_EXIT();
    return 0;
}

int32_t HalCameraDeviceOpen(uint32_t cameraId)
{
    int32_t ret = 0;
    CAM_FW_ENTER();
    cam_lock(&g_cam_dev.dev_lock);
    IF_FUNC_ERR(cam_pipe_create(cameraId), ret = _ret, "cam pipe create failed");
    cam_unlock(&g_cam_dev.dev_lock);
    CAM_FW_EXIT();
    return ret;
}

int32_t HalCameraDeviceClose(uint32_t cameraId)
{
    int32_t ret = 0;
    CAM_FW_ENTER();
    cam_lock(&g_cam_dev.dev_lock);
    IF_FUNC_ERR(cam_pipe_destroy(cameraId), ret = _ret, "cam pipe dectroy failed");
    cam_unlock(&g_cam_dev.dev_lock);
    CAM_FW_EXIT();
    return ret;
}

int32_t HalCameraStreamCreate(uint32_t cameraId, const StreamAttr *streamAttr, uint32_t *streamId)
{
    CAM_FW_ENTER();
    UNUSED(cameraId);
    ERR_IF(streamAttr == NULL || streamId == NULL, return -1, "param null");
    ERR_IF(!is_support_stream_format(streamAttr->format), return -1, "not support format: %d", streamAttr->format);
    ERR_IF(streamAttr->width != CAM_CFG_OUT_WIDTH, return -1, "not support w: %d", streamAttr->width);
    ERR_IF(streamAttr->height != CAM_CFG_OUT_HEIGHT, return -1, "not support h: %d", streamAttr->height);

    uint32_t stream_idx = CAM_CFG_STREAM_MAX_NUM;
    cam_stream *stream = NULL;
    cam_lock(&g_cam_dev.dev_lock);

    for (uint32_t i = 0; i < CAM_CFG_STREAM_MAX_NUM; i++) {
        stream = &g_cam_dev.streams[i];
        cam_lock(&stream->stream_lock);
        if (stream->stream_stat != STREAM_STAT_IDLE) {
            cam_unlock(&stream->stream_lock);
            continue;
        }
        stream_idx = i;
        break;
    }
    ERR_IF(stream_idx == CAM_CFG_STREAM_MAX_NUM, goto err_unlock_dev, "no idle stream");

    if (streamAttr->format == FORMAT_YVU420 && stream->vdec_info == NULL) {
        IF_FUNC_ERR(vdec_create(&stream->vdec_info), goto err_unlock_stream, "vdec create err");
    }
    stream->stream_stat = STREAM_STAT_CREATED;
    cam_unlock(&stream->stream_lock);
    cam_unlock(&g_cam_dev.dev_lock);

    *streamId = stream_idx;
    CAM_INFO("camera stream[%u] created, format: %d\n", stream_idx, streamAttr->format);
    CAM_FW_EXIT();
    return 0;

err_unlock_stream:
    cam_unlock(&stream->stream_lock);
err_unlock_dev:
    cam_unlock(&g_cam_dev.dev_lock);
    return -1;
}

int32_t HalCameraStreamDestroy(uint32_t cameraId, uint32_t streamId)
{
    CAM_FW_ENTER();
    ERR_IF(streamId >= CAM_CFG_STREAM_MAX_NUM, return -1, "stream id err: %u", streamId);
    cam_stream *stream = &g_cam_dev.streams[streamId];

    cam_lock(&g_cam_dev.dev_lock);
    cam_lock(&stream->stream_lock);
    ERR_IF(stream->stream_stat != STREAM_STAT_CREATED, goto err_unlock, "stream %u not create stat: %d",
        streamId, stream->stream_stat);

    if (stream->vdec_info != NULL) {
        vdec_destroy(&stream->vdec_info);
    }
    stream->stream_stat = STREAM_STAT_IDLE;
    cam_unlock(&stream->stream_lock);
    cam_unlock(&g_cam_dev.dev_lock);
    CAM_FW_EXIT();
    return 0;

err_unlock:
    cam_unlock(&stream->stream_lock);
    cam_unlock(&g_cam_dev.dev_lock);
    return -1;
}

int32_t HalCameraGetDeviceId(uint32_t cameraId, uint32_t streamId, uint32_t *deviceId)
{
    CAM_FW_ENTER();
    UNUSED(cameraId);
    UNUSED(deviceId);
    ERR_IF(streamId >= CAM_CFG_STREAM_MAX_NUM, return -1, "stream id err: %u", streamId);
    CAM_FW_EXIT();
    return 0;
}

int32_t HalCameraStreamOn(uint32_t cameraId, uint32_t streamId)
{
    CAM_FW_ENTER();
    bool is_stream_turned_on = false;
    ERR_IF(streamId >= CAM_CFG_STREAM_MAX_NUM, return -1, "stream id err: %u", streamId);

    cam_stream *stream = &g_cam_dev.streams[streamId];
    cam_lock(&g_cam_dev.dev_lock);
    cam_lock(&stream->stream_lock);
    if (stream->stream_stat != STREAM_STAT_CREATED) {
        CAM_ERR("stream %u stat err: %d", stream->stream_stat);
        goto err_unlock;
    }

    if (!g_cam_dev.is_pipe_on) {
        IF_FUNC_ERR(cam_pipe_stream_on(cameraId), goto err_out, "pipe stream on err\n");
        g_cam_dev.is_pipe_on = true;
        is_stream_turned_on = true;
    }
    CAM_DBG("is_thread_created: %d\n", g_cam_dev.is_thread_created);
    if (!g_cam_dev.is_thread_created) {
        g_cam_dev.need_thread_exit = false;
        pthread_attr_t attr;
        IF_FUNC_ERR(pthread_attr_init(&attr), goto err_out, "pthread attr err");
        IF_FUNC_ERR(pthread_attr_setstacksize(&attr, 8192), goto err_out, "pthread attr err"); // 8192: 8k stack size
        IF_FUNC_ERR(pthread_create(&g_cam_dev.thread, &attr, stream_thread, NULL), goto err_out,
            "create stream thread err");
        LOG_FUNC_ERR(pthread_setname_np(g_cam_dev.thread, "cam-hal"), "thread set name err");
        g_cam_dev.is_thread_created = true;
    }
#if CAMERA_PRINT_FPS_SUPPORT
    stream->frame_count = 0;
    stream->frame_count_start_time = 0;
#endif
    stream->skip_frame = true; // skip first frame, avoid get half frame stream
    stream->stream_stat = STREAM_STAT_STREAM_ON;
    cam_unlock(&stream->stream_lock);
    cam_unlock(&g_cam_dev.dev_lock);
    CAM_FW_EXIT();
    return 0;

err_out:
    if (is_stream_turned_on) {
        cam_pipe_stream_off(cameraId);
        g_cam_dev.is_pipe_on = false;
    }
err_unlock:
    cam_unlock(&stream->stream_lock);
    cam_unlock(&g_cam_dev.dev_lock);
    return -1;
}

int32_t HalCameraStreamOff(uint32_t cameraId, uint32_t streamId)
{
    CAM_FW_ENTER();
    ERR_IF(streamId >= CAM_CFG_STREAM_MAX_NUM, return -1, "stream id err: %u", streamId);

    cam_stream *stream = &g_cam_dev.streams[streamId];
    cam_lock(&stream->stream_lock);
    if (stream->stream_stat != STREAM_STAT_STREAM_ON) {
        cam_unlock(&stream->stream_lock);
        CAM_ERR("stream %u not on", streamId);
        return -1;
    }
    stream->stream_stat = STREAM_STAT_CREATED;
    stream->buf_stat = STREAM_BUF_NONE;
    cam_unlock(&stream->stream_lock);

    cam_lock(&g_cam_dev.dev_lock);
    if (get_stream_on_count() == 0) {
        if (g_cam_dev.is_thread_created) {
            g_cam_dev.need_thread_exit = true;
            cam_unlock(&g_cam_dev.dev_lock);
            LOG_FUNC_ERR(pthread_join(g_cam_dev.thread, NULL), "stream thread join err");
            cam_lock(&g_cam_dev.dev_lock);
            g_cam_dev.is_thread_created = false;
        }

        if (g_cam_dev.is_pipe_on) {
            if (cam_pipe_stream_off(cameraId) == 0) {
                g_cam_dev.is_pipe_on = false;
            }
        }
    }

    cam_unlock(&g_cam_dev.dev_lock);
    CAM_FW_EXIT();
    return 0;
}

int32_t HalCameraDequeueBuf(uint32_t cameraId, uint32_t streamId, HalBuffer *buffer)
{
    CAM_FW_ENTER();
    UNUSED(cameraId);
    ERR_IF(streamId >= CAM_CFG_STREAM_MAX_NUM, return -1, "stream id err: %u", streamId);
    ERR_IF(buffer == NULL, return -1, "stream %u buffer null", streamId);

    CAM_TRACE_BEGIN("HAL-DQBUF");
    cam_stream *stream = &g_cam_dev.streams[streamId];
    cam_lock(&stream->stream_lock);
    ERR_IF(stream->stream_stat != STREAM_STAT_STREAM_ON, goto err_unlock, "stream %u not on", streamId);
    ERR_IF(stream->buf_stat == STREAM_BUF_NONE, goto err_unlock, "stream %u no queue buf", streamId);

    if (stream->buf_stat != STREAM_BUF_FILLED) {
        CAM_DBG("stream[%u] buf not ready\n", streamId);
        goto err_unlock;
    }

    if (stream->vdec_info != NULL) {
        if (vdec_dequeue_frame(stream->vdec_info, &stream->out_stream_buffer, CAM_CFG_VDEC_WAIT_TIMEOUT_MS) != 0) {
            CAM_DBG("stream[%u] vdec not decode complete\n", streamId);
            goto err_unlock;
        }
    }

    *buffer = stream->out_stream_buffer;
    stream->buf_stat = STREAM_BUF_NONE;

#if CAMERA_PRINT_FPS_SUPPORT
    stat_frame_fps(stream);
#endif

    cam_unlock(&stream->stream_lock);
    CAM_FW_EXIT();
    CAM_TRACE_END();
    return 0;

err_unlock:
    cam_unlock(&stream->stream_lock);
    CAM_TRACE_END();
    return -1;
}

int32_t HalCameraQueueBuf(uint32_t cameraId, uint32_t streamId, const HalBuffer *buffer)
{
    CAM_FW_ENTER();
    UNUSED(cameraId);
    ERR_IF(streamId >= CAM_CFG_STREAM_MAX_NUM, return -1, "stream id err: %u", streamId);
    ERR_IF(buffer == NULL, return -1, "stream %u buffer null", streamId);
    ERR_IF(buffer->virAddr == NULL, return -1, "stream %u buffer vir addr null", streamId);

    CAM_TRACE_BEGIN("HAL-QBUF");
    cam_stream *stream = &g_cam_dev.streams[streamId];
    cam_lock(&stream->stream_lock);
    IF_FUNC_ERR(stream->buf_stat != STREAM_BUF_NONE, goto err_unlock, "stream %u queued buf already", streamId);

    stream->out_stream_buffer = *buffer;
    stream->out_stream_buffer.dataLen = 0;
    if (stream->vdec_info != NULL) {
        IF_FUNC_ERR(vdec_queue_frame(stream->vdec_info, buffer), goto err_unlock, "vdec queue err");
    }
    stream->buf_stat = STREAM_BUF_QUEUED;
    cam_unlock(&stream->stream_lock);
    CAM_FW_EXIT();
    CAM_TRACE_END();
    return 0;

err_unlock:
    cam_unlock(&stream->stream_lock);
    CAM_TRACE_END();
    return -1;
}

int32_t HalCameraSetBufferCallback(uint32_t cameraId, const BufferAvailable callback)
{
    CAM_FW_ENTER();
    UNUSED(cameraId);
    UNUSED(callback);
    CAM_FW_EXIT();
    return 0;
}

int32_t HalCameraStreamSetInfo(uint32_t cameraId, uint32_t streamId, const StreamInfo *info)
{
    CAM_FW_ENTER();
    UNUSED(cameraId);
    UNUSED(streamId);
    UNUSED(info);
    CAM_FW_EXIT();
    return 0;
}

int32_t HalCameraStreamGetInfo(uint32_t cameraId, uint32_t streamId, StreamInfo *info)
{
    CAM_FW_ENTER();
    UNUSED(cameraId);
    UNUSED(streamId);
    UNUSED(info);
    CAM_FW_EXIT();
    return 0;
}

int32_t HalCameraDeviceSetInfo(uint32_t cameraId, const DeviceInfo *info)
{
    CAM_FW_ENTER();
    UNUSED(cameraId);
    UNUSED(info);
    CAM_FW_EXIT();
    return 0;
}

int32_t HalCameraDeviceGetInfo(uint32_t cameraId, DeviceInfo *info)
{
    CAM_FW_ENTER();
    UNUSED(cameraId);
    UNUSED(info);
    CAM_FW_EXIT();
    return 0;
}

int32_t HalCameraGetAbility(uint32_t cameraId, AbilityInfo *ability)
{
    CAM_FW_ENTER();
    UNUSED(cameraId);
    UNUSED(ability);
    CAM_FW_EXIT();
    return 0;
}

int32_t HalCameraSetResultCb(uint32_t cameraId, CameraResultCb cb)
{
    CAM_FW_ENTER();
    UNUSED(cameraId);
    UNUSED(cb);
    CAM_FW_EXIT();
    return 0;
}