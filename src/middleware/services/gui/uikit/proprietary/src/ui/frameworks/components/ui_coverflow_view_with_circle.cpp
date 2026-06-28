/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: Implements the UICoverflowView and provides the selection circle function.
 * Author: Hisi Graphic Team
 * Created: 2026-01
 */
#include "components/ui_coverflow_view_with_circle.h"
#include "draw/hw_draw_utils.h"
#include "engines/gfx/lite_m_gfx_engine.h"
#include "gfx_utils/graphic_math.h"

namespace OHOS {
void CoverflowViewWithCircle::Init()
{
    if (selectCircle_ == nullptr) {
        selectCircle_ = new(std::nothrow) Path();
        if (selectCircle_ == nullptr) {
            GRAPHIC_LOGE("select circle path is nullptr.");
            return;
        }
    }
    if (!HWDrawUtils::GetInstance()->AllocBufferForPath(selectCircle_, 5, 26)) { // 5:cmd num 26: data num
        GRAPHIC_LOGE("select circle alloc path faild.");
        delete selectCircle_;
        selectCircle_ = nullptr;
        return;
    }
}

void CoverflowViewWithCircle::DeInit()
{
    if (selectCircle_ != nullptr) {
        if (selectCircle_->cmds != nullptr || selectCircle_->data != nullptr) {
            LiteMGfxEngine::GetInstance()->SyncHwDraw();
            HWDrawUtils::GetInstance()->FreeBufferForPath(selectCircle_);
        }
        delete selectCircle_;
        selectCircle_ = nullptr;
    }
    if (stopColors_ != nullptr) {
        UIFree(stopColors_);
        stopColors_ = nullptr;
    }
}

void CoverflowViewWithCircle::SetSelectCircleWidth(float width)
{
    if (FloatLess(width, 0.0f)) {
        GRAPHIC_LOGE("SetSelectCircleWidth faild! width less 0.");
        return;
    }
    selectWidth_ = width;
}

void CoverflowViewWithCircle::SetSelectCircleColor(ColorType color)
{
    circleColor_ = color;
    isRenderGradient_ = false;
}

void CoverflowViewWithCircle::SetRadialGradientStops(const ColorStop* stops, uint16_t stopCount)
{
    if (stops == nullptr || stopCount == 0) {
        GRAPHIC_LOGE("Parameter input failed.");
        return;
    }

    if (stopColors_ != nullptr) {
        UIFree(stopColors_);
    }

    stopColors_ = static_cast<Paint::StopAndColor*>(UIMalloc(sizeof(Paint::StopAndColor) * stopCount));
    if (stopColors_ == nullptr) {
        GRAPHIC_LOGE("UIMalloc for stops failed.");
        return;
    }
    if (memcpy_s(stopColors_, sizeof(Paint::StopAndColor) * stopCount, stops,
        sizeof(Paint::StopAndColor) * stopCount) != EOK) {
        GRAPHIC_LOGE("memcpy_s stop failed.");
        UIFree(stopColors_);
        stopColors_ = nullptr;
        return;
    }
    stopCount_ = stopCount;
    isRenderGradient_ = true;
}

bool CoverflowViewWithCircle::OnDragStart(UIView& view, const DragEvent& event)
{
    isNeedDrawSelectCircle_ = false;
    return false;
}

void CoverflowViewWithCircle::OnSwipe(UISwipeView& view)
{
    isNeedDrawSelectCircle_ = true;
    Invalidate();
}

void CoverflowViewWithCircle::HandleStyleAndGradient(VGUPaintStyle& style, VGUGradient& gradient, int16_t& centerX,
    int16_t& centerY, uint16_t radius)
{
    gradient.colorStops = reinterpret_cast<VGUColorStop*>(stopColors_);
    gradient.stopCount = stopCount_;
    gradient.radial.x0 = centerX;
    gradient.radial.y0 = centerY;
    gradient.radial.r0 = radius;
    gradient.radial.x1 = centerX;
    gradient.radial.y1 = centerY;
    gradient.radial.r1 = radius;
    gradient.type = VGUGradientType::VGU_GRADIENT_RADIAL;
    gradient.spread = VGUFillSpread::VGU_SPREAD_PAD;

    style.type = VGUPaintType::VGU_PAINT_GRADIENT;
    style.gradient = &gradient;
}

void CoverflowViewWithCircle::OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea)
{
    UICoverflowView::OnDraw(gfxDstBuffer, invalidatedArea);

    if (!isNeedDrawSelectCircle_) {
        return;
    }
    UIViewGroup* group = dynamic_cast<UIViewGroup*>(GetCurrentView());
    if (group == nullptr) {
        return;
    }
    Rect rect = group->GetChildrenHead()->GetRect();
    uint16_t w = rect.GetWidth();
    uint16_t h = rect.GetHeight();
    int16_t centerX = rect.GetX() + w / 2; // 2: half
    int16_t centerY = rect.GetY() + h / 2; // 2: half
    uint16_t radius = w < h ? static_cast<uint16_t>((h + selectWidth_)  / 2): static_cast<uint16_t>((w + selectWidth_) / 2);  // 2: half
    struct ArcInfo info = {{centerX, centerY}, {0, 0}, radius, 0, CIRCLE_IN_DEGREE, nullptr};
    if (selectCircle_ == nullptr ||
        !HWDrawUtils::GetInstance()->FillCirclePath(info, selectCircle_->cmdNum, selectCircle_->cmds,
            selectCircle_->dataNum, selectCircle_->data)) {
        GRAPHIC_LOGE("fill circle path failed.");
        return;
    }

    VGUGradient gradient = {0};
    VGUPaintStyle strokeStyle = {0};
    if (isRenderGradient_) {
        HandleStyleAndGradient(strokeStyle, gradient, centerX, centerY, radius);
    } else {
        strokeStyle.color = circleColor_.full;
        strokeStyle.type = VGUPaintType::VGU_PAINT_COLOR;
    }
    VGUStrokeAttr strokeAttr = {VGULineCap::VGU_LINECAP_ROUND, VGUJointType::VGU_LINE_JOIN_BEVEL,
        0.0f, selectWidth_, &strokeStyle};

    uintptr_t vguPath = LiteMGfxEngine::GetInstance()->InitPath(false);
    LiteMGfxEngine::GetInstance()->SetPathData(vguPath, selectCircle_->cmdNum, selectCircle_->cmds,
        selectCircle_->dataNum, selectCircle_->data);
    LiteMGfxEngine::GetInstance()->RenderPath(gfxDstBuffer, vguPath, GetOrigRect(), &strokeAttr, nullptr, nullptr);
    LiteMGfxEngine::GetInstance()->DeinitPath(vguPath);
}
} // namespace OHOS