/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
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

#include "tiger_view.h"
#include <string>
#include <unistd.h>
#include "draw/hw_draw_utils.h"
#include "draw/draw_utils.h"
#include "engines/gfx/lite_m_gfx_engine.h"
#include "gfx_utils/file.h"

namespace OHOS {
TigerView::TigerView()
{
    TigerPathInit(&pathInfo_);
}

TigerView::~TigerView()
{
    TigerPathDeinit(&pathInfo_);
    RemoveAll();
    if (tigerAnimator != nullptr) {
        tigerAnimator->Stop();
        delete tigerAnimator;
        tigerAnimator = nullptr;
    }
    if (callBack != nullptr) {
        delete callBack;
        callBack = nullptr;
    }
    if (canvas_ != nullptr) {
        canvas_->Clear();
        delete canvas_;
        canvas_ = nullptr;
    }
    if (pathCacheKey_ != 0) {
        LiteMGfxEngine::GetInstance()->DeinitRenderCache(pathCacheKey_);
        pathCacheKey_ = 0;
    }
}

int TigerView::TigerPathInit(PathInfo *path)
{
#ifdef _WIN32
    int fd = open(RES_PATH"/tiger_path.bin", O_RDONLY | O_BINARY);
#else
    int fd = open(RES_PATH"/tiger_path.bin", O_RDWR, S_IRWXU);
#endif
    if (fd < 0) {
        GRAPHIC_LOGE("open path fail");
        return false;
    }

    if (read(fd, &path->header, sizeof(path->header)) != static_cast<ssize_t>(sizeof(path->header))) {
        GRAPHIC_LOGE("read /user/res/tiger_path.bin fail.");
    }

    path->cmds = static_cast<unsigned char *>(UIMalloc(path->header.cmdsSize));
    if (read(fd, path->cmds, path->header.cmdsSize) != path->header.cmdsSize) {
        GRAPHIC_LOGE("read cmds fail.");
        TigerPathDeinit(path);
        close(fd);
        return -1;
    }
    path->cmdIndex = static_cast<int *>(UIMalloc(path->header.cmdIndexSize));
    if (read(fd, path->cmdIndex, path->header.cmdIndexSize) != path->header.cmdIndexSize) {
        GRAPHIC_LOGE("read cmdIndex fail.");
        TigerPathDeinit(path);
        close(fd);
        return -1;
    }
    path->cmdNum = static_cast<int *>(UIMalloc(path->header.cmdNumSize));
    if (read(fd, path->cmdNum, path->header.cmdNumSize) != path->header.cmdNumSize) {
        GRAPHIC_LOGE("read cmdNum fail.");
        TigerPathDeinit(path);
        close(fd);
        return -1;
    }
    path->data = static_cast<float *>(UIMalloc(path->header.dataSize));
    if (read(fd, path->data, path->header.dataSize) != path->header.dataSize) {
        GRAPHIC_LOGE("read data fail.");
        TigerPathDeinit(path);
        close(fd);
        return -1;
    }
    path->dataIndex = static_cast<int *>(UIMalloc(path->header.dataIndexSize));
    if (read(fd, path->dataIndex, path->header.dataIndexSize) != path->header.dataIndexSize) {
        GRAPHIC_LOGE("read dataIndex fail.");
        TigerPathDeinit(path);
        close(fd);
        return -1;
    }
    path->dataNum = static_cast<int *>(UIMalloc(path->header.dataNumSize));
    if (read(fd, path->dataNum, path->header.dataNumSize) != path->header.dataNumSize) {
        GRAPHIC_LOGE("read dataNum fail.");
        TigerPathDeinit(path);
        close(fd);
        return -1;
    }
    path->color = static_cast<unsigned int *>(UIMalloc(path->header.colorSize));
    if (read(fd, path->color, path->header.colorSize) != path->header.colorSize) {
        GRAPHIC_LOGE("read color fail.");
        TigerPathDeinit(path);
        close(fd);
        return -1;
    }
    close(fd);

    path->cmdCounts = path->header.cmdNumSize / static_cast<int>(sizeof(int));
    return 0;
}

void TigerView::TigerPathDeinit(PathInfo *path)
{
    if (path->cmds != nullptr) {
        UIFree(path->cmds);
        path->cmds = nullptr;
    }
    if (path->cmdIndex != nullptr) {
        UIFree(path->cmdIndex);
        path->cmdIndex = nullptr;
    }
    if (path->cmdNum != nullptr) {
        UIFree(path->cmdNum);
        path->cmdNum = nullptr;
    }
    if (path->data != nullptr) {
        UIFree(path->data);
        path->data = nullptr;
    }
    if (path->dataIndex != nullptr) {
        UIFree(path->dataIndex);
        path->dataIndex = nullptr;
    }
    if (path->dataNum != nullptr) {
        UIFree(path->dataNum);
        path->dataNum = nullptr;
    }
    if (path->color != nullptr) {
        UIFree(path->color);
        path->color = nullptr;
    }
}

void TigerView::LoadTigerPathData()
{
    PaintExt paint;
    paint.SetStyle(Paint::PaintStyle::FILL_STYLE);
    paint.SetOpacity(OPA_OPAQUE);
    paint.SetAntialiased(true);
    for (int i = 0; i < pathInfo_.cmdCounts; i++) {
        unsigned char *cmds = &pathInfo_.cmds[pathInfo_.cmdIndex[i]];
        int cmdNum = pathInfo_.cmdNum[i];
        float *datas = &pathInfo_.data[pathInfo_.dataIndex[i]];
        int dataNum = pathInfo_.dataNum[i];
        uint32_t color = pathInfo_.color[i];
        // 16,8: bit shift
        paint.SetFillColor(Color::GetColorFromRGBA((color & 0x00FF0000) >> 16, (color & 0x0000FF00) >> 8,
            (color & 0x000000FF), (color & 0xFF000000) >> 24)); // 24: bit shift
        canvas_->BeginPath(paint);
        canvas_->SetPathData(cmdNum, cmds, dataNum, datas);
        canvas_->DrawPath();
    }
    canvas_->DrawPath();
}

bool TigerView::InitView()
{
    canvas_ = new UICanvasExt();
    if (canvas_ == nullptr) {
        GRAPHIC_LOGE("Create UICanvasExt Failed.");
        return false;
    }
    if (LiteMGfxEngine::GetInstance()->IsCacheRenderSupport()) {
        pathCacheKey_ = LiteMGfxEngine::GetInstance()->InitRenderCache(pathInfo_.cmdCounts + 1);
        if (pathCacheKey_ == 0) {
            GRAPHIC_LOGE("InitPathCache Failed.");
        } else {
            canvas_->SetVisible(false);
        }
    }
    canvas_->SetPosition(0, 0, 800, 800); // 800: width and hight
    canvas_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    canvas_->Scale({ 1, -1 }, { 0, 0 });
    canvas_->Translate({ 0, 800 }); // 800: width and hight
    LoadTigerPathData();
    Add(canvas_);

    callBack = new TigerAnimatorCallback(canvas_);
    if (callBack == nullptr) {
        GRAPHIC_LOGE("Tiger callBack new fail");
        return false;
    }
    tigerAnimator = new Animator(callBack, canvas_, 0, true);
    if (tigerAnimator == nullptr) {
        GRAPHIC_LOGE("tigerAnimator new fail");
        return false;
    }
    return true;
}

void TigerView::OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea)
{
    if (pathCacheKey_ == 0) {
        UIView::OnDraw(gfxDstBuffer, invalidatedArea);
        return;
    }
    LiteMGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, GetOrigRect(), invalidatedArea, *style_, GetMixOpaScale());
    if (!isCanvasDrawn_) {
        LiteMGfxEngine::GetInstance()->CacheRenderGroupBegin(pathCacheKey_, VGU_RENDER_GROUP_SHARE_GRADIENT,
            VGU_RENDER_GROUP_SHARE_ALTER);
        canvas_->OnDraw(gfxDstBuffer, invalidatedArea);
        LiteMGfxEngine::GetInstance()->CacheRenderGroupEnd(pathCacheKey_);
        isCanvasDrawn_ = true;
    } else {
        VGUMatrix3 mat;
        TransformMap& canvasMap = canvas_->GetTransformMap();
        Rect rect = canvas_->GetOrigRect();
        if (!(canvasMap.GetTransMapRect() == rect)) {
            canvasMap.SetTransMapRect(rect);
        }
        Matrix4<float> viewMatrix = canvasMap.GetTransformMatrix();
        HWDrawUtils::GetInstance()->Matrix4ToVguMatrix3(viewMatrix, mat);

        LiteMGfxEngine::GetInstance()->SetGroupRenderBufferAddr(pathCacheKey_, 0, gfxDstBuffer.phyAddr);
        LiteMGfxEngine::GetInstance()->SetGroupRenderRect(pathCacheKey_, 0, invalidatedArea);
        LiteMGfxEngine::GetInstance()->SetGroupRenderMatrix(pathCacheKey_, 0, &mat);
    }
    LiteMGfxEngine::GetInstance()->DoRenderByCache(pathCacheKey_);
}

bool TigerView::OnReleaseEvent(const ReleaseEvent& event)
{
    if (isScaleStart_) {
        if (tigerAnimator->GetState() == Animator::START) {
            tigerAnimator->Stop();
        }
        isScaleStart_ = false;
    } else if (isNeedReset_) {
        canvas_->ResetTransParameter();
        canvas_->Scale({ 1, -1 }, { 0, 0 });
        canvas_->Translate({ 0, 800 }); // 800: y translate length
        isNeedReset_ = false;
    }
    if (pathCacheKey_ != 0) {
        Invalidate();
    }

    return true;
}

bool TigerView::OnDragEvent(const DragEvent& event)
{
    if (tigerAnimator->GetState() != Animator::START) {
        // When the child is not visible, the drag action does not take effect
        if (pathCacheKey_ != 0) {
            canvas_->SetVisible(true);
        }
        UIScrollView::OnDragEvent(event);
        if (pathCacheKey_ != 0) {
            canvas_->SetVisible(false);
        }
    }
    return true;
}

bool TigerView::OnDragEndEvent(const DragEvent& event)
{
    if (tigerAnimator->GetState() == Animator::START) {
        tigerAnimator->Stop();
    }
    // When the child is not visible, the drag action does not take effect
    if (pathCacheKey_ != 0) {
        canvas_->SetVisible(true);
    }
    UIScrollView::OnDragEvent(event);
    if (pathCacheKey_ != 0) {
        canvas_->SetVisible(false);
    }
    return true;
}

bool TigerView::OnLongPressEvent(const LongPressEvent& event)
{
    if (!isNeedReset_) {
        callBack->startTime_ = HALTick::GetInstance().GetTime();
        callBack->center.x = event.GetCurrentPos().x;
        callBack->center.y = event.GetCurrentPos().y;
        tigerAnimator->Start();
        isScaleStart_ = true;
        isNeedReset_ = true;
    }
    return true;
}
}
