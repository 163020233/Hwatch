/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2021-2021. All rights reserved.
 * Description: camera stream operator impl
 * Author: media_develop team
 * Create: 20211117
 */

#include "stream_operator_impl.h"
#include <sys/prctl.h>
#include <unistd.h>
using namespace std;
namespace OHOS {
namespace Camera {
const int STREAM_FPS = 30;
const float YCBCR420SP_BUFFER_SIZE_COEFFICIENT = 1.5;
// Waiting time calculated based on the frame rate of 60, 1000 / 60 = 16ms
const int WAIT_FLUSHBUFFER_TO_SURFACE_TIME = 10000;

StreamOperatorImpl::StreamOperatorImpl(std::shared_ptr<StreamMng> &streamMng)
    : m_streamMng(streamMng), m_isInit(false), m_surfaceReady(false)
{
    m_streamBaseInfoMap.clear();
    m_cachedEventList.clear();
    Init();
}

void StreamOperatorImpl::ReleaseAll()
{
    MediaHalMutexLock(m_infoMutex);
    for (auto baseInfo = m_streamBaseInfoMap.begin(); baseInfo != m_streamBaseInfoMap.end(); baseInfo++) {
        map<const SurfaceBuffer *, HalBuffer *> bufferMap = baseInfo->second->bufferMap;
        for (auto it = bufferMap.begin(); it != bufferMap.end(); it++) {
            delete (it->second);
            it->second = nullptr;
        }
    }
    m_streamBaseInfoMap.clear();
   
    MediaHalMutexUnLock(m_infoMutex);
}

StreamOperatorImpl::~StreamOperatorImpl()
{
    ReleaseAll();
    m_isInit = false;
    MediaHalThreadCondSignal(m_threadCond);
    MediaHalThreadJoin(&m_thread);
    MediaHalThreadCondDestroy(&m_threadCond);
    MediaHalMutexDestroy(&m_infoMutex);
}

CamRetCode StreamOperatorImpl::Init()
{
    MediaHalMutexAttr mutexAttr = { true };
    m_infoMutex = MediaHalMutexCreate(&mutexAttr);
    if (m_infoMutex == nullptr) {
        HAL_LOGE("m_infoMutex is null");
        return INSUFFICIENT_RESOURCES;
    }
    m_threadCond = MediaHalThreadCondCreate();;
    if (m_threadCond == nullptr) {
        HAL_LOGE("fork thread StreamCopyProcess failed");
        MediaHalMutexDestroy(&m_infoMutex);
        return INSUFFICIENT_RESOURCES;
    }
    m_isInit = true;
    MediaHalThreadattr attr = { "CameraBufferLoop", 0x2000, THREAD_SCHED_INVALID, 0, false };
    m_thread = MediaHalThreadCreate(BufferLoopThread, this, &attr);
    if (m_thread == nullptr) {
        HAL_LOGE("fork thread StreamCopyProcess failed");
        MediaHalThreadCondDestroy(&m_threadCond);
        MediaHalMutexDestroy(&m_infoMutex);
        return INSUFFICIENT_RESOURCES;
    }

    return NO_ERROR;
}

CamRetCode StreamOperatorImpl::IsStreamsSupported(
    OperationMode mode,
    const std::shared_ptr<CameraStandard::CameraMetadata> &modeSetting,
    const std::shared_ptr<StreamInfo> &info,
    StreamSupportType &type)
{
    HW_HAL_UNUSED(mode);
    HW_HAL_UNUSED(modeSetting);
    HW_HAL_UNUSED(info);
    type = DYNAMIC_SUPPORTED;
    return NO_ERROR;
}

CamRetCode StreamOperatorImpl::IsStreamsSupported(
    OperationMode mode,
    const std::shared_ptr<CameraStandard::CameraMetadata> &modeSetting,
    const std::vector<std::shared_ptr<StreamInfo>> &info,
    StreamSupportType &type)
{
    HW_HAL_UNUSED(mode);
    HW_HAL_UNUSED(modeSetting);
    HW_HAL_UNUSED(info);
    type = DYNAMIC_SUPPORTED;
    return NO_ERROR;
}

CamRetCode StreamOperatorImpl::CreateStreams(const std::vector<std::shared_ptr<StreamInfo>> &streamInfos)
{
    vector<HalStreamInfo> halStreamInfos;
    for (auto streamInfo : streamInfos) {
        shared_ptr<StreamBaseInfo> streamBaseInfo = make_shared<StreamBaseInfo>();
        streamBaseInfo->captureIds.clear();
        streamBaseInfo->bufferMap.clear();
        streamBaseInfo->bufferQueue = streamInfo->bufferQueue_;
        if (memset_s(&(streamBaseInfo->attr), sizeof(streamBaseInfo->attr), 0, sizeof(streamBaseInfo->attr)) != EOK) {
            HAL_LOGE("memset_s failed");
        }
        streamBaseInfo->attr.width = streamInfo->width_;
        streamBaseInfo->attr.height = streamInfo->height_;
        streamBaseInfo->attr.format = FORMAT_YVU420; // streamInfo->format_;
        streamBaseInfo->attr.type = STREAM_PREVIEW; // streamInfo->intent_;
        streamBaseInfo->attr.fps = STREAM_FPS; // streamInfo->intent_;

        MediaHalMutexLock(m_infoMutex);
        m_streamBaseInfoMap.emplace(streamInfo->streamId_, streamBaseInfo);
        MediaHalMutexUnLock(m_infoMutex);
        HAL_LOGI("create stream id %d w = %d h = %d", streamInfo->streamId_,
            streamInfo->width_, streamInfo->height_);
        HalStreamInfo info;
        info.id = streamInfo->streamId_;
        info.attr = streamBaseInfo->attr;
        halStreamInfos.emplace_back(info);
        CaptureDoneCb captureDoneCb;
        captureDoneCb.cb = CameraEventCallback;
        captureDoneCb.imp = this;
        m_streamMng->SetCaptureDoneCb(streamInfo->streamId_, captureDoneCb);
    }
    m_streamMng->CreateStreams(halStreamInfos);
    return NO_ERROR;
}

CamRetCode StreamOperatorImpl::ReleaseStreams(const std::vector<int> &streamIds)
{
    MediaHalMutexLock(m_infoMutex);
    m_streamMng->ReleaseStreams(streamIds);
    for (auto streamId : streamIds) {
        auto baseInfo = m_streamBaseInfoMap.find(streamId);
        if (baseInfo != m_streamBaseInfoMap.end()) {
            map<const SurfaceBuffer *, HalBuffer *> bufferMap = baseInfo->second->bufferMap;
            for (SurfaceBuffer *sb : m_vdecCachedBuf) {
                baseInfo->second->bufferQueue->CancelBuffer(sb);
            }
            for (auto it = bufferMap.begin(); it != bufferMap.end(); it++) {
                delete (it->second);
                it->second = NULL;
            }
            m_streamBaseInfoMap.erase(streamId);
        }
    }
    MediaHalMutexUnLock(m_infoMutex);
    return NO_ERROR;
}

CamRetCode StreamOperatorImpl::CommitStreams(OperationMode mode,
    const std::shared_ptr<CameraStandard::CameraMetadata> &modeSetting)
{
    HW_HAL_UNUSED(mode);
    HW_HAL_UNUSED(modeSetting);
    return NO_ERROR;
}

CamRetCode StreamOperatorImpl::GetStreamAttributes(std::vector<std::shared_ptr<StreamAttribute>> &attributes)
{
    HW_HAL_UNUSED(attributes);
    return NO_ERROR;
}

CamRetCode StreamOperatorImpl::AttachBufferQueue(int streamId, const OHOS::sptr<OHOS::Surface> &producer)
{
    HW_HAL_UNUSED(streamId);
    HW_HAL_UNUSED(producer);
    return NO_ERROR;
}

CamRetCode StreamOperatorImpl::DetachBufferQueue(int streamId)
{
    HW_HAL_UNUSED(streamId);
    return NO_ERROR;
}

CamRetCode StreamOperatorImpl::Capture(int captureId, const std::shared_ptr<CaptureInfo> &info,  bool isStreaming)
{
    MediaHalMutexLock(m_infoMutex);
    for (auto streamId : info->streamIds_) {
        HAL_LOGI("capture stream id = %d", streamId);
        auto pair = m_streamBaseInfoMap.find(streamId);
        if (pair != m_streamBaseInfoMap.end()) {
            CaptureId capture;
            capture.isStreaming = isStreaming;
            capture.captureId = captureId;
            pair->second->captureIds.emplace_back(capture);
        }
    }
    MediaHalMutexUnLock(m_infoMutex);
    return NO_ERROR;
}

CamRetCode StreamOperatorImpl::CancelCapture(int captureId)
{
    MediaHalMutexLock(m_infoMutex);
    for (auto it = m_streamBaseInfoMap.begin(); it != m_streamBaseInfoMap.end(); it++) {
        for (auto itList = it->second->captureIds.begin(); itList != it->second->captureIds.end();) {
            if (itList->captureId == captureId) {
                it->second->captureIds.erase(itList++);
            } else {
                itList++;
            }
        }
    }
    MediaHalMutexUnLock(m_infoMutex);
    return NO_ERROR;
}

CamRetCode StreamOperatorImpl::ChangeToOfflineStream(const std::vector<int> &streamIds,
    OHOS::sptr<IStreamOperatorCallback> &callback,
    OHOS::sptr<IOfflineStreamOperator> &offlineOperator)
{
    HW_HAL_UNUSED(streamIds);
    HW_HAL_UNUSED(offlineOperator);
    m_callback = callback;
    return NO_ERROR;
}

HalBuffer& StreamOperatorImpl::GetHalBuffer(const SurfaceBuffer *surfaceBuf,
    std::map<const SurfaceBuffer *, HalBuffer *> &bufferMap)
{
    auto it = bufferMap.find(surfaceBuf);
    if (it != bufferMap.end()) {
        return *(it->second);
    }
    HalBuffer *halBuffer = new HalBuffer;
    if (memset_s(halBuffer, sizeof(HalBuffer), 0, sizeof(HalBuffer)) != EOK) {
        HAL_LOGE("memset_s failed");
    }
    halBuffer->virAddr = (void *)(uintptr_t)(surfaceBuf->addr);
    halBuffer->size = static_cast<int32_t>(surfaceBuf->width * surfaceBuf->height * YCBCR420SP_BUFFER_SIZE_COEFFICIENT);
    halBuffer->phyAddr = surfaceBuf->addr;
    bufferMap.emplace(surfaceBuf, halBuffer);
    return *halBuffer;
}

void StreamOperatorImpl::CameraEventCallback(const void *imp, int32_t streamId, void *buffer, const CameraEventPkg *event)
{
    StreamOperatorImpl *obj = (StreamOperatorImpl *)imp;
    MediaHalMutexLock((obj->m_infoMutex));
    (obj->m_cachedEventList).push_back(const_cast<CameraEventPkg *>(event));
    MediaHalMutexUnLock((obj->m_infoMutex));
    MediaHalThreadCondSignal(obj->m_threadCond);
}

void StreamOperatorImpl::SurfaceFlushBuffer(Surface *surf, CameraEventPkg &pkg)
{
    OutputInfo *outInfo = (OutputInfo *)pkg.buffer;
    LiteSurface* liteSurface = dynamic_cast<LiteSurface*>(surf);
    HAL_LOGI("bufferQueue not null, addr %p buffer %p", surf, outInfo->vendorPrivate);
    auto it = std::find(m_vdecCachedBuf.begin(), m_vdecCachedBuf.end(), outInfo->vendorPrivate);
    if (liteSurface != nullptr && outInfo->buffers != nullptr) {
        liteSurface->SetUVOffset(outInfo->buffers[0].uvOffset);
    }
    if (it != m_vdecCachedBuf.end()) {
        m_vdecCachedBuf.erase(it);
    }
    surf->FlushBuffer(static_cast<SurfaceBuffer *>(outInfo->vendorPrivate));
    usleep(WAIT_FLUSHBUFFER_TO_SURFACE_TIME);
}

CamRetCode StreamOperatorImpl::SurfaceRequestBuffer(Surface *surf, CameraEventPkg &pkg, uint32_t bufferNum)
{
    if (surf == nullptr) {
        HAL_LOGE("surf null");
        return INVALID_ARGUMENT;
    }
    if (!m_surfaceReady) {
        uint32_t byteAlign = pow(2, (int)log2(pkg.fmtChange->widthStride - pkg.fmtChange->width) + 1);
        surf->SetStrideAlignment(byteAlign);
        surf->SetWidthAndHeight(pkg.fmtChange->width, pkg.fmtChange->height);
        surf->SetQueueSize(pkg.fmtChange->bufferNum);
        surf->SetSize(pkg.fmtChange->bufferSize);
        HAL_LOGI("vdec format change [%d, %d], stride = %d, byteAlign = %u, bufferSize = %d, bufferNum = %d",
            pkg.fmtChange->width, pkg.fmtChange->height,
            pkg.fmtChange->widthStride, byteAlign, pkg.fmtChange->bufferSize, pkg.fmtChange->bufferNum);
        LiteSurface* liteSurface = dynamic_cast<LiteSurface*>(surf);
        if (liteSurface != nullptr) {
            liteSurface->PrepareBuffers();
        }
        m_surfaceReady = true;
        MediaHalFree(pkg.fmtChange);  
    }
    SurfaceBuffer *surfaceBuf = nullptr;
    for (uint32_t i = 0; i < bufferNum; i++) {
        surfaceBuf = surf->RequestBuffer();
        if (surfaceBuf == nullptr) {
            HAL_LOGI("RequestBuffer fail");
            m_streamMng->Capture(0, NULL);
            usleep(WAIT_FLUSHBUFFER_TO_SURFACE_TIME);
            break;
        }
        m_vdecCachedBuf.push_back(surfaceBuf);
        OutputInfo outInfo = {};
        outInfo.type = VIDEO_DECODER;
        outInfo.bufferCnt = 1;
        outInfo.buffers = (CodecBufferInfo *)MediaHalCalloc(sizeof(CodecBufferInfo));
        if (outInfo.buffers == nullptr) {
            HAL_LOGE("malloc failed");
            return INSUFFICIENT_RESOURCES;
        }
        outInfo.buffers->type = BUFFER_TYPE_VIRTUAL;
        outInfo.buffers->addr = reinterpret_cast<uint8_t *>(surfaceBuf->addr);
        outInfo.buffers[0].addr = reinterpret_cast<uint8_t *>(surfaceBuf->addr);
        outInfo.buffers->length = surf->GetSize();
        outInfo.vendorPrivate = surfaceBuf;
        CameraEventPkg pkg = {
            CAMERA_INVALID_EVENT,
            NULL,
            &outInfo,
        };
        m_streamMng->Capture(0, &pkg);
        MediaHalFree(outInfo.buffers);                
    }
    return NO_ERROR;
}

void *StreamOperatorImpl::BufferLoopThread(void *arg)
{
    HAL_LOGI("enter BufferLoopThread start");
    prctl(PR_SET_NAME, "CameraBufferLoop");
    StreamOperatorImpl *obj = (StreamOperatorImpl *)arg;
    CaptureId capture;
    SurfaceBuffer *surfaceBuf = nullptr;
    while (obj->m_isInit) {
        for (auto it = obj->m_streamBaseInfoMap.begin(); it != obj->m_streamBaseInfoMap.end(); it++) {
            Surface *surf = it->second->bufferQueue;
            CameraEventPkg *event = nullptr;
            MediaHalMutexLock((obj->m_infoMutex));
            if (obj->m_cachedEventList.size() <= 0) {
                MediaHalThreadCondWait(obj->m_threadCond, obj->m_infoMutex);
                MediaHalMutexUnLock((obj->m_infoMutex));
                if (!obj->m_isInit) {
                    break;
                }
                continue;
            }
            event = obj->m_cachedEventList[0];
            obj->m_cachedEventList.erase(obj->m_cachedEventList.begin());
            MediaHalMutexUnLock((obj->m_infoMutex));
            if (event == nullptr && obj->m_isInit) {
                continue;
            }
            switch(event->eventType) {
                case CAMERA_BUFFER_DONE_EVENT:
                    HAL_LOGI("bufferloop CAMERA_BUFFER_DONE_EVENT");
                    obj->SurfaceFlushBuffer(surf, *event);
                    obj->SurfaceRequestBuffer(surf, *event, 1); // ½á¹ûÐ£Ñé
                    break;
                case CAMERA_FORMAT_CHANGE_EVENT:
                    HAL_LOGI("eventType %d, event buffernum %u", event->eventType, event->fmtChange->bufferNum);
                    obj->SurfaceRequestBuffer(surf, *event, event->fmtChange->bufferNum);
                    break;
                case CAMERA_EOS_EVENT:
                case CAMERA_ERROR_EVENT:
                    obj->m_isInit = false;
                    HAL_LOGI("receive eos");
                    obj->m_callback->OnCaptureEnded(0, obj->m_infoVector);
                    break;
                default:
                    HAL_LOGE("invalid event");
            }
        }
    }
    HAL_LOGI("out");
    return nullptr;
}

} // namespace Camera
} // namespace OHOS