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

#ifndef CAMERA_TEST_H
#define CAMERA_TEST_H

#include <cstdio>
#include <string>
#include <list>
#include "camera_kit.h"
#include "media_log.h"
#include "frame_config.h"
#include "camera_state_callback.h"
#include "frame_state_callback.h"
#include "components/ui_lite_surface_view.h"

#define CAM_ERR(fmt, arg...) printf(fmt, ##arg)
#define CAM_WARN(fmt, arg...) printf(fmt, ##arg)
#define CAM_INFO(fmt, arg...) printf(fmt, ##arg)
#define CAM_DBG(fmt, arg...) printf(fmt, ##arg)
#define CAM_VER(fmt, arg...)

namespace OHOS {
namespace Media {

class CameraTest : private CameraStateCallback, private FrameStateCallback, private IBufferConsumerListener {
public:
    CameraTest() = default;
    ~CameraTest() = default;

    int CameraInit(void);
    void CameraDeinit(void);
    void StartPreview();
    void StopPreview();
    void StartRecord(const char *filePath);
    void StopRecord();
    void Capture(const char *filePath);

private:
    // CameraStateCallback
    virtual void OnCreated(Camera &c) override;
    virtual void OnCreateFailed(const std::string cameraId, int32_t errorCode) override;
    virtual void OnReleased(Camera &c) override;
    virtual void OnConfigured(Camera &c) override;
    virtual void OnConfigureFailed(const std::string cameraId, int32_t errorCode) override;

    // FrameStateCallback
    virtual void OnFrameFinished(Camera &camera, FrameConfig &frameConfig, FrameResult &frameResult) override;
    virtual void OnFrameError(Camera &camera, FrameConfig &frameConfig, int32_t errorCode,
        FrameResult &frameResult) override;

    // surface IBufferConsumerListener for record
    virtual void OnBufferAvailable() override;

    // CameraTest inner
    void WriteFrame(FrameConfig &config, FILE *fp);
    LiteSurface *CreatePreviewSurface(void);
    Surface *CreateSurface(int frameType);
    void DestroySurface(int frameType, Surface *surface);
    int SetFrameConfig(FrameConfig *fc);
    int ClearFrameConfig(FrameConfig *fc);

    // private varible
    EventHandler mEventHandler;
    CameraKit *mCameraKit;
    Camera *mCamera;
    std::string mCameraId;
    FrameConfig mFrameCfgPreview {FRAME_CONFIG_PREVIEW};
    FrameConfig mFrameCfgRecord {FRAME_CONFIG_CALLBACK};
    FrameConfig mFrameCfgCapture {FRAME_CONFIG_CAPTURE};
    std::list<UILiteSurfaceView *> mSurfaceViewList;
    FILE *mRecordFp;
}; // class CameraTest

} // namespace Media
} // namespace OHOS
#endif
