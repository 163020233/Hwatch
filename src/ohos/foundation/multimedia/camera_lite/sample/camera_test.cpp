/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <unistd.h>
#include "camera_state_callback.h"
#include "camera_kit.h"
#include "debug_print.h"
#include "frame_config.h"
#include "frame_state_callback.h"
#include "meta_data.h"

// uikit
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

#include "camera_test.h"

using namespace OHOS;
using namespace OHOS::Media;

static constexpr int32_t CAM_WIDTH = 640;
static constexpr int32_t CAM_HEIGHT = 480;
static constexpr int32_t CAM_PREVIEW_FORMAT = CAM_IMAGE_YUV420;
static constexpr int32_t CAM_PREVIEW_SURFACE_BUFFER_NUM = 3;
static constexpr int32_t CAM_RECORD_SURFACE_BUFFER_NUM = 2;
static constexpr int32_t CAM_CAPTURE_SURFACE_BUFFER_NUM = 1;
static constexpr int32_t CAM_JPEG_BUF_SIZE = CAM_WIDTH * CAM_HEIGHT / 2; // jpeg compression ratio generally 1/5 ~ 1/30

int CameraTest::CameraInit(void) {
    CAM_DBG("APP %s() enter\n", __FUNCTION__);
    CameraKit *camKit = CameraKit::GetInstance();
    if (camKit == nullptr) {
        CAM_ERR("no camera instance\n");
        return -1;
    }

    CAM_DBG("FW CameraKit::GetCameraIds() enter\n");
    std::list<std::string> cameraIds = camKit->GetCameraIds();
    CAM_DBG("FW CameraKit::GetCameraIds() exit\n");
    CAM_INFO("camera num: %u\n", cameraIds.size());
    if (cameraIds.size() == 0) {
        CAM_ERR("no camera\n");
        return -1;
    }

    mCameraId = cameraIds.front();
    CAM_DBG("FW CameraKit::CreateCamera() enter\n");
    camKit->CreateCamera(mCameraId, *this, mEventHandler);
    CAM_DBG("FW CameraKit::CreateCamera() exit\n");
    mCameraKit = camKit;
    usleep(10 * 1000); // wait on create
    if (mCamera == nullptr) {
        CAM_ERR("wait OnCreated timeout!\n");
        return -1;
    }
    CAM_DBG("APP %s() exit\n", __FUNCTION__);
    return 0;
}

void CameraTest::CameraDeinit(void) {
    CAM_DBG("APP %s() enter\n", __FUNCTION__);
    if (mCamera != nullptr) {
        CAM_DBG("FW Camera::Release() enter\n");
        mCamera->Release();
        CAM_DBG("FW Camera::Release() exit\n");
    }
    usleep(10 * 1000); // wait on release
    mCamera = nullptr;
    mCameraKit = nullptr;
    CAM_DBG("APP %s() exit\n", __FUNCTION__);
}

void CameraTest::StartPreview() {
    CAM_DBG("APP %s() enter\n", __FUNCTION__);
    if (mCamera == nullptr) {
        CAM_ERR("not init\n");
        return;
    }
    SetFrameConfig(&mFrameCfgPreview);
    CAM_DBG("FW Camera::TriggerLoopingCapture(FRAME_CONFIG_PREVIEW) enter\n");
    mCamera->TriggerLoopingCapture(mFrameCfgPreview);
    CAM_DBG("FW Camera::TriggerLoopingCapture(FRAME_CONFIG_PREVIEW) exit\n");
    CAM_DBG("APP %s() exit\n", __FUNCTION__);
    return;
}

void CameraTest::StopPreview() {
    CAM_DBG("APP %s() enter\n", __FUNCTION__);
    if (mCamera == nullptr) {
        CAM_ERR("not init\n");
        return;
    }

    CAM_DBG("FW Camera::StopLoopingCapture(FRAME_CONFIG_PREVIEW) enter\n");
    mCamera->StopLoopingCapture(FRAME_CONFIG_PREVIEW);
    CAM_DBG("FW Camera::StopLoopingCapture(FRAME_CONFIG_PREVIEW) exit\n");
    ClearFrameConfig(&mFrameCfgPreview);
    CAM_DBG("APP %s() exit\n", __FUNCTION__);
}

void CameraTest::StartRecord(const char *filePath) {
    CAM_DBG("APP %s() enter\n", __FUNCTION__);
    if (mCamera == nullptr) {
        CAM_ERR("not init\n");
        return;
    }
    if (filePath != NULL) {
        mRecordFp = fopen(filePath, "w+");
        if (mRecordFp == nullptr) {
            CAM_ERR("file open error: %s\n", filePath);
            return;
        }
    }
    SetFrameConfig(&mFrameCfgRecord);
    CAM_DBG("FW Camera::TriggerLoopingCapture(FRAME_CONFIG_CALLBACK) enter\n");
    mCamera->TriggerLoopingCapture(mFrameCfgRecord);
    CAM_DBG("FW Camera::TriggerLoopingCapture(FRAME_CONFIG_CALLBACK) exit\n");
    CAM_DBG("APP %s() exit\n", __FUNCTION__);
}

void CameraTest::StopRecord() {
    CAM_DBG("%s() enter\n", __FUNCTION__);
    if (mCamera == nullptr) {
        CAM_ERR("not init\n");
        return;
    }
    CAM_DBG("FW Camera::StopLoopingCapture(FRAME_CONFIG_CALLBACK) enter\n");
    mCamera->StopLoopingCapture(FRAME_CONFIG_CALLBACK);
    CAM_DBG("FW Camera::StopLoopingCapture(FRAME_CONFIG_CALLBACK) exit\n");
    if (mRecordFp != nullptr) {
        fclose(mRecordFp);
        mRecordFp = nullptr;
    }
    ClearFrameConfig(&mFrameCfgRecord);
    CAM_DBG("%s() exit\n", __FUNCTION__);
}

void CameraTest::Capture(const char *filePath) {
    CAM_DBG("APP %s() enter\n", __FUNCTION__);
    if (mCamera == nullptr) {
        CAM_ERR("not init\n");
        return;
    }
    FILE *fp = fopen(filePath, "w+");
    if (fp == nullptr) {
        CAM_ERR("file open error: %s\n", filePath);
        return;
    }
    SetFrameConfig(&mFrameCfgCapture);
    CAM_DBG("FW Camera::TriggerSingleCapture(FRAME_CONFIG_CAPTURE) enter\n");
    if (mCamera->TriggerSingleCapture(mFrameCfgCapture) == 0) {
        WriteFrame(mFrameCfgCapture, fp);
    } else {
        CAM_ERR("camera capture error\n");
    }
    CAM_DBG("FW Camera::TriggerSingleCapture(FRAME_CONFIG_CAPTURE) exit\n");
    fclose(fp);
    ClearFrameConfig(&mFrameCfgCapture);
    CAM_DBG("APP %s() exit\n", __FUNCTION__);
}

void CameraTest::OnCreated(Camera &c) {
    CAM_INFO("FW CameraStateCallback::OnCreated() enter\n");
    CameraConfig *config = CameraConfig::CreateCameraConfig();
    if (config == nullptr) {
        CAM_ERR("config create error!\n");
        return;
    }
    config->SetFrameStateCallback(this, &mEventHandler);
    c.Configure(*config); // config free in CameraImpl::Release()
    mCamera = &c;
    CAM_INFO("FW CameraStateCallback::OnCreated() exit\n");
}

void CameraTest::OnCreateFailed(const std::string cameraId, int32_t errorCode) {
    CAM_ERR("FW CameraStateCallback::OnCreateFailed()\n");
}

void CameraTest::OnReleased(Camera &c) {
    CAM_INFO("FW CameraStateCallback::OnReleased()\n");
}

void CameraTest::OnConfigured(Camera &c) {
    CAM_DBG("FW CameraStateCallback::OnConfigured()\n");
}

void CameraTest::OnConfigureFailed(const std::string cameraId, int32_t errorCode) {
    CAM_ERR("FW CameraStateCallback::OnConfigureFailed()\n");
}

void CameraTest::OnFrameFinished(Camera &camera, FrameConfig &frameConfig, FrameResult &frameResult) {
    CAM_INFO("FW FrameStateCallback::OnFrameFinished()\n");
}
void CameraTest::OnFrameError(Camera &camera, FrameConfig &frameConfig, int32_t errorCode,
    FrameResult &frameResult) {
    CAM_ERR("FW FrameStateCallback::OnFrameError()\n");
}

void CameraTest::WriteFrame(FrameConfig &config, FILE *fp) {
    std::list<Surface*> surfaceList = config.GetSurfaces();
    if (surfaceList.size() == 0) {
        CAM_WARN("record no surface\n");
        return;
    }

    Surface *surface = surfaceList.front();
    SurfaceBuffer *buf = surface->AcquireBuffer();
    if (buf == nullptr) {
        CAM_WARN("record acquire buffer error\n");
        return;
    }

    if (fp != nullptr) {
        CAM_VER("write 0x*%04x, %d\n", buf->addr & 0xFFFF, buf->len);
        if (fwrite(reinterpret_cast<void *>(static_cast<uintptr_t>(buf->addr)), buf->len, 1, fp) != 1) {
            CAM_ERR("record write file error! len: %d\n", buf->len);
        }
    }

    if (!surface->ReleaseBuffer(buf)) {
        CAM_WARN("record release buffer error\n");
        return;
    }
}

void CameraTest::OnBufferAvailable() {
    WriteFrame(mFrameCfgRecord, mRecordFp);
}

LiteSurface *CameraTest::CreatePreviewSurface(void) {
    UILiteSurfaceView *surfaceView = new UILiteSurfaceView();
    if (surfaceView == nullptr) {
        CAM_ERR("surfaceView is nullptr\n");
        return nullptr;
    }
    LiteSurface *surface = dynamic_cast<LiteSurface *>(surfaceView->GetSurface());
    if (surface == nullptr) {
        CAM_ERR("surfaceView get surface error\n");
        delete surfaceView;
        return nullptr;
    }
    mSurfaceViewList.push_back(surfaceView);
    surfaceView->SetViewId("cam");
    uint16_t screenW = Screen::GetInstance().GetWidth();
    uint16_t screenH = Screen::GetInstance().GetHeight();
    surfaceView->SetPosition(0, 0, screenW, screenH);
    surfaceView->SetTouchable(true);
    surfaceView->SetIntercept(true);

    GraphicService::GetInstance()->PostGraphicEvent([surfaceView, screenW, screenH]() {
        CAM_DBG("Async add surfaceView to RootView\n");
        RootView::GetInstance()->SetWidth(screenW);
        RootView::GetInstance()->SetHeight(screenH);
        RootView::GetInstance()->SetPosition(0, 0);
        RootView::GetInstance()->SetStyle(STYLE_BACKGROUND_OPA, 0);
        RootView::GetInstance()->Add(surfaceView);
    });
    return surface;
}

Surface *CameraTest::CreateSurface(int frameType) {
    LiteSurface *surface = nullptr;
    if (frameType == FRAME_CONFIG_PREVIEW) {
        surface = CreatePreviewSurface();
    } else if (frameType == FRAME_CONFIG_CALLBACK || frameType == FRAME_CONFIG_CAPTURE) {
        surface = new LiteSurface();
    } else {
        CAM_ERR("unsupport frame type: %d\n", frameType);
        return nullptr;
    }
    if (surface == nullptr) {
        CAM_ERR("malloc error\n");
        return nullptr;
    }

    if (frameType == FRAME_CONFIG_PREVIEW) {
        surface->SetFormat(PIXEL_FMT_YCBCR_420_SP);
        surface->SetSize(CAM_WIDTH * CAM_HEIGHT * 3 / 2); // 3/2 SP420 size 
        surface->SetUVOffset(CAM_WIDTH * CAM_HEIGHT);
        surface->SetQueueSize(CAM_PREVIEW_SURFACE_BUFFER_NUM);
    } else if (frameType == FRAME_CONFIG_CALLBACK) {
        // register OnBufferAvailable
        CAM_DBG("register record listener\n");
        surface->RegisterConsumerListener(*this);
        surface->SetFormat(PIXEL_FMT_YCBCR_420_SP); // actual image format is mjpeg
        surface->SetSize(CAM_JPEG_BUF_SIZE);
        surface->SetQueueSize(CAM_RECORD_SURFACE_BUFFER_NUM);
    } else if (frameType == FRAME_CONFIG_CAPTURE) {
        surface->SetFormat(PIXEL_FMT_YCBCR_420_SP); // actual image format is mjpeg
        surface->SetSize(CAM_JPEG_BUF_SIZE);
        surface->SetQueueSize(CAM_CAPTURE_SURFACE_BUFFER_NUM);
    }

    surface->SetWidthAndHeight(CAM_WIDTH, CAM_HEIGHT);
    surface->PrepareBuffers();
    return dynamic_cast<Surface *>(surface);
}

void CameraTest::DestroySurface(int frameType, Surface *surface) {
    if (frameType == FRAME_CONFIG_PREVIEW) {
        CAM_INFO("Destroy preview surface");
        std::list<UILiteSurfaceView *> viewListCopy = mSurfaceViewList; 
        GraphicService::GetInstance()->PostGraphicEvent([viewListCopy]() {
            CAM_INFO("Async del view\n");
            for (UILiteSurfaceView *view : viewListCopy) {
                RootView::GetInstance()->Remove(view);
                CAM_INFO("del view\n");
                delete view;
            }
        });
        mSurfaceViewList.clear();
    } else {
        delete surface;
    }
}

int CameraTest::SetFrameConfig(FrameConfig *fc) {
    int cfgType = fc->GetFrameConfigType();

    std::list<Surface*> surfaceList = fc->GetSurfaces();
    Surface *surface = nullptr;
    if (surfaceList.size() == 0) {
        surface = CreateSurface(cfgType);
        if (surface == nullptr) {
            CAM_ERR("create surface failed!\n");
            return -1;
        }
        fc->AddSurface(*surface);
    }

    if (cfgType == FRAME_CONFIG_PREVIEW) {
        fc->SetParameter(CAM_IMAGE_FORMAT, CAM_PREVIEW_FORMAT);
    }
    fc->SetParameter(CAM_FRAME_FPS, 30);
    fc->SetParameter(CAM_IMAGE_INVERT_MODE, CAM_CENTER_MIRROR);
    return 0;
}

int CameraTest::ClearFrameConfig(FrameConfig *fc) {
    int cfgType = fc->GetFrameConfigType();
    std::list<Surface*> surfaceList = fc->GetSurfaces();
    for (Surface *surface : surfaceList) {
        fc->RemoveSurface(*surface);
        DestroySurface(cfgType, surface);
    }
    return 0;
}