/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "components/ui_view.h"
#include <cstdio>
#include "common/screen.h"
#include "components/root_view.h"
#include "core/render_manager.h"
#include "dfx/ui_view_bounds.h"
#include "dock/focus_manager.h"
#include "draw/draw_utils.h"
#if ENABLE_VGU_ENGINE && CAPABILITY_HW_GAUSSIAN_BLUR_BY_MASK
#include "draw/hw_draw_utils.h"
#endif
#include "engines/gfx/gfx_engine_manager.h"
#include "engines/gfx/lite_m_gfx_engine.h"
#include "imgdecode/image_load.h"
#include "gfx_utils/graphic_log.h"
#include "gfx_utils/mem_api.h"
#include "hals/display_dev.h"
#if ENABLE_VGU_ENGINE && CAPABILITY_HW_GAUSSIAN_BLUR_BY_MASK
#include "hals/gralloc_engines.h"
#endif
#include "securec.h"
#include "themes/theme_manager.h"

namespace OHOS {
UIView::UIView()
    : touchable_(false),
      visible_(true),
      draggable_(false),
      dragParentInstead_(true),
      isViewGroup_(false),
      needRedraw_(false),
      styleAllocFlag_(false),
      isIntercept_(false),
      isAlignEnable_(true),
      isSizeChange_(false),
#if ENABLE_FOCUS_MANAGER
      focusable_(false),
#endif
      fullScreenBlur_(false),
      opaScale_(OPA_OPAQUE),
      index_(0),
      zIndex_(0),
      pPrevWidth_(0),
      pPrevHeight_(0),
      id_(nullptr),
      parent_(nullptr),
      nextSibling_(nullptr),
      nextRenderSibling_(nullptr),
      alignType_(ALIGN_DEFAULT),
      style_(nullptr),
      transMap_(nullptr),
      onClickListener_(nullptr),
      onLongPressListener_(nullptr),
      onDragListener_(nullptr),
      onTouchListener_(nullptr),
#if ENABLE_FOCUS_MANAGER
      onFocusListener_(nullptr),
#endif
#if ENABLE_ROTATE_INPUT
      onRotateListener_(nullptr),
#endif
      viewExtraMsg_(nullptr),
      rect_(0, 0, -1, -1),
      visibleRect_(nullptr),
#if ENABLE_VGU_ENGINE && CAPABILITY_HW_GAUSSIAN_BLUR_BY_MASK
      blurMask_(nullptr),
      blurPath_(nullptr),
      isBlurPathChange_(false),
      blurPathCmdNum_(0),
      blurPathDataNum_(0),
      blurBorderRadius_(0),
      blurQuality_(0),
#endif
#if ENABLE_VGU_ENGINE && CAPABILITY_HW_LIGHT
      lightStyle_(nullptr),
#endif
      blurRadius_(0.0f)
{
    SetupThemeStyles();
}

UIView::~UIView()
{
#if ENABLE_ROTATE_INPUT
    ClearFocus();
#endif
#if ENABLE_VGU_ENGINE && CAPABILITY_HW_GAUSSIAN_BLUR_BY_MASK
    ClearBlurPath();
    ClearBlurMask();
#endif
    ClearBackgroundBlur();
    if (parent_ != nullptr) {
        GRAPHIC_LOGE("UIView::~UIView failed, parent is not nullptr! Need to remove from parent component first");
    }
    if (transMap_ != nullptr) {
        delete transMap_;
        transMap_ = nullptr;
    }
    if (visibleRect_ != nullptr) {
        delete visibleRect_;
        visibleRect_ = nullptr;
    }
    if (styleAllocFlag_) {
        delete style_;
        style_ = nullptr;
        styleAllocFlag_ = false;
    }
#if ENABLE_VGU_ENGINE && CAPABILITY_HW_LIGHT
    if (lightStyle_) {
        delete lightStyle_;
        lightStyle_ = nullptr;
    }
#endif
}

Rect UIView::GetInnerRect(const Rect &rect, int16_t topRadius, int16_t bottomRadius) const
{
    int16_t minR = MATH_MIN(rect.GetHeight(), rect.GetWidth()) / 2;  // 2: half
    topRadius = MATH_MIN(topRadius, minR);
    bottomRadius = MATH_MIN(bottomRadius, minR);
    topRadius = (topRadius + 1) / 2;        // 2: half
    bottomRadius = (bottomRadius + 1) / 2;  // 2: half

    int16_t x = rect.GetX();
    int16_t y = rect.GetY();
    Rect innerRect;
    if (topRadius >= bottomRadius) {
        innerRect.SetLeft(x + topRadius);
        innerRect.SetTop(y + topRadius);
        innerRect.SetWidth(rect.GetWidth() - 2 * topRadius);  // 2: half
    } else {
        innerRect.SetLeft(x + bottomRadius);
        innerRect.SetTop(y + topRadius);
        innerRect.SetWidth(rect.GetWidth() - 2 * bottomRadius);  // 2: half
    }
    innerRect.SetHeight(rect.GetHeight() - topRadius - bottomRadius);

    return innerRect;
}

bool UIView::OnPreDraw(Rect& invalidatedArea) const
{
    Rect rect(GetRect());
    int16_t topRadius = style_->GetStyle(STYLE_TOP_BORDER_RADIUS);
    int16_t bottomRadius = style_->GetStyle(STYLE_BOTTOM_BORDER_RADIUS);
    if (topRadius == bottomRadius && topRadius == COORD_MAX) {
        return true;
    }
    if (topRadius != 0 || bottomRadius != 0) {
        rect = GetInnerRect(rect, topRadius, bottomRadius);
    }
    if (rect.IsContains(invalidatedArea)) {
        return true;
    }
    invalidatedArea.Intersect(invalidatedArea, rect);
    return false;
}

#if ENABLE_VGU_ENGINE && CAPABILITY_HW_GAUSSIAN_BLUR_BY_MASK
static void* AllocBufferForMask(uint32_t size)
{
    AllocInfo cmdInfo;
    cmdInfo.expectedSize = size;
    cmdInfo.usage = HBM_USE_ASSIGN_SIZE | HBM_USE_MEM_MULTI_IMG;
    GrallocBuffer buffer;
    if (!GrallocEngines::GetInstance()->AllocBuffer(cmdInfo, buffer)) {
        GRAPHIC_LOGE("Alloc buffer for data failed.");
        return nullptr;
    }
    return buffer.virAddr;
}
#endif

void UIView::HandleGaussianBlur(BufferInfo& gfxDstBuffer)
{
    if (RootView::GetInstance()->GetBlurView() && RootView::GetInstance()->GetBlurView() != this) {
        return;
    }
    if (visible_ && (!FloatEqual(blurRadius_, 0.0f))) {
        BufferInfo gfxTargetBuffer = gfxDstBuffer;
        Rect mask = {0, 0, gfxTargetBuffer.width - 1, gfxTargetBuffer.height - 1};
        if (!fullScreenBlur_) {
            Rect rect = GetRect();
            mask.Intersect(rect, mask);
            if (!mask.IsValid()) {
                return;
            }
        }
        gfxTargetBuffer.rect = mask;
#if ENABLE_VGU_ENGINE && CAPABILITY_HW_GAUSSIAN_BLUR_BY_MASK
        if (blurPath_ && !fullScreenBlur_) {
            if (isBlurPathChange_ || !blurMask_) {
                CreateBlurMask();
                LiteMGfxEngine::GetInstance()->DrawBlurMaskByPath(*blurMask_, GetRect(), *blurPath_);
                isBlurPathChange_ = false;
            }
            LiteMGfxEngine::GetInstance()->Blur(gfxTargetBuffer, blurRadius_, blurQuality_, blurMask_);
        } else if (style_->borderRadius_ != 0 && style_->borderRadius_ < COORD_MAX && !fullScreenBlur_) {
            if ((style_->borderRadius_ != blurBorderRadius_) || !blurMask_ || !(blurMaskRect_ == GetRect())) {
                blurMaskRect_ = GetRect();
                CreateBlurMask();
                LiteMGfxEngine::GetInstance()->DrawBlurMaskByRect(*blurMask_, mask, blurMaskRect_, *style_);
                blurBorderRadius_ = style_->borderRadius_;
            }
            LiteMGfxEngine::GetInstance()->Blur(gfxTargetBuffer, blurRadius_, blurQuality_, blurMask_);
        } else {
            LiteMGfxEngine::GetInstance()->Blur(gfxTargetBuffer, blurRadius_, blurQuality_);
        }
        return;
#endif
        BaseGfxEngine::GetInstance()->Blur(gfxTargetBuffer, blurRadius_);
    }
}

#if ENABLE_VGU_ENGINE && CAPABILITY_HW_LIGHT
void UIView::HandleLighting(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea)
{
    Rect rect = lightRect_;
    if (visible_ && isEnableLight_ && lightStyle_) {
        if (rect.Intersect(lightRect_, invalidatedArea)) {
            BufferInfo gfxTargetBuffer = gfxDstBuffer;
            gfxTargetBuffer.rect = rect;
            LiteMGfxEngine::GetInstance()->Lighting(gfxTargetBuffer, *lightStyle_);
        }
    }
}
#endif

void UIView::OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea)
{
    uint8_t opa = GetMixOpaScale();
    BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, GetOrigRect(), invalidatedArea, *style_, opa);
}

void UIView::OnPostDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea)
{
    DrawViewBounds(gfxDstBuffer, invalidatedArea);
}

void UIView::DrawViewBounds(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea)
{
#if ENABLE_DEBUG
    if (!UIViewBounds::GetInstance()->GetShowState()) {
        return;
    }
    Style* style = new Style();
    style->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    style->SetStyle(STYLE_BORDER_COLOR, Color::Red().full);
    style->SetStyle(STYLE_BORDER_WIDTH, 1);
    style->SetStyle(STYLE_BORDER_OPA, OPA_OPAQUE / 2); // 2: half opacity
    Rect viewRect(GetRect());
    BaseGfxEngine* baseGfxEngine = BaseGfxEngine::GetInstance();
    baseGfxEngine->DrawRect(gfxDstBuffer, viewRect, invalidatedArea, *style, OPA_OPAQUE);

    style->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    style->SetStyle(STYLE_BACKGROUND_COLOR, Color::Blue().full);
    style->SetStyle(STYLE_BORDER_WIDTH, 0);
    Rect tmpRect(viewRect);
    int16_t length = 10; // 10: corner length

    // left top corner
    tmpRect.SetRight(viewRect.GetLeft() + length);
    tmpRect.SetBottom(viewRect.GetTop());
    baseGfxEngine->DrawRect(gfxDstBuffer, tmpRect, invalidatedArea, *style, OPA_OPAQUE);
    tmpRect.SetRight(viewRect.GetLeft());
    tmpRect.SetBottom(viewRect.GetTop() + length);
    baseGfxEngine->DrawRect(gfxDstBuffer, tmpRect, invalidatedArea, *style, OPA_OPAQUE);

    // left bottom corner
    tmpRect.SetLeft(viewRect.GetLeft());
    tmpRect.SetTop(viewRect.GetBottom() - length);
    tmpRect.SetRight(viewRect.GetLeft());
    tmpRect.SetBottom(viewRect.GetBottom());
    baseGfxEngine->DrawRect(gfxDstBuffer, tmpRect, invalidatedArea, *style, OPA_OPAQUE);
    tmpRect.SetTop(viewRect.GetBottom());
    tmpRect.SetRight(viewRect.GetLeft() + length);
    baseGfxEngine->DrawRect(gfxDstBuffer, tmpRect, invalidatedArea, *style, OPA_OPAQUE);

    // right top corner
    tmpRect.SetLeft(viewRect.GetRight() - length);
    tmpRect.SetTop(viewRect.GetTop());
    tmpRect.SetRight(viewRect.GetRight());
    tmpRect.SetBottom(viewRect.GetTop());
    baseGfxEngine->DrawRect(gfxDstBuffer, tmpRect, invalidatedArea, *style, OPA_OPAQUE);
    tmpRect.SetLeft(viewRect.GetRight());
    tmpRect.SetBottom(viewRect.GetTop() + length);
    baseGfxEngine->DrawRect(gfxDstBuffer, tmpRect, invalidatedArea, *style, OPA_OPAQUE);

    // right bottom corner
    tmpRect = viewRect;
    tmpRect.SetLeft(viewRect.GetRight());
    tmpRect.SetTop(viewRect.GetBottom() - length);
    baseGfxEngine->DrawRect(gfxDstBuffer, tmpRect, invalidatedArea, *style, OPA_OPAQUE);
    tmpRect.SetLeft(viewRect.GetRight() - length);
    tmpRect.SetTop(viewRect.GetBottom());
    baseGfxEngine->DrawRect(gfxDstBuffer, tmpRect, invalidatedArea, *style, OPA_OPAQUE);
    delete style;
#endif // ENABLE_DEBUG
}

void UIView::SetupThemeStyles()
{
    Theme* theme = ThemeManager::GetInstance().GetCurrent();
    if (theme != nullptr) {
        style_ = &(theme->GetMainStyle());
    } else {
        style_ = &(StyleDefault::GetDefaultStyle());
    }
}

void UIView::SetStyle(Style& style)
{
    if (styleAllocFlag_) {
        delete style_;
        styleAllocFlag_ = false;
    }
    style_ = &style;
}

void UIView::SetStyle(uint8_t key, int64_t value)
{
    if (!styleAllocFlag_) {
        style_ = new Style(*style_);
        if (style_ == nullptr) {
            GRAPHIC_LOGE("new Style fail");
            return;
        }
        styleAllocFlag_ = true;
    }
    int16_t width = GetWidth();
    int16_t height = GetHeight();
    int16_t x = GetX();
    int16_t y = GetY();
    style_->SetStyle(key, value);
    Rect rect(x, y, x + width - 1, y + height - 1);
    UpdateRectInfo(key, rect);
}

void UIView::UpdateRectInfo(uint8_t key, const Rect& rect)
{
    switch (key) {
        case STYLE_BORDER_WIDTH: {
            SetWidth(rect.GetWidth());
            SetHeight(rect.GetHeight());
            break;
        }
        case STYLE_PADDING_LEFT:
        case STYLE_PADDING_RIGHT: {
            SetWidth(rect.GetWidth());
            break;
        }
        case STYLE_PADDING_TOP:
        case STYLE_PADDING_BOTTOM: {
            SetHeight(rect.GetHeight());
            break;
        }
        case STYLE_MARGIN_LEFT: {
            SetX(rect.GetX());
            break;
        }
        case STYLE_MARGIN_TOP: {
            SetY(rect.GetY());
            break;
        }
        default:
            break;
    }
}

int64_t UIView::GetStyle(uint8_t key) const
{
    return style_->GetStyle(key);
}

const Style& UIView::GetStyleConst() const
{
    return *style_;
}

void UIView::SetOpaScale(uint8_t opaScale)
{
    opaScale_ = opaScale;
}

uint8_t UIView::GetOpaScale() const
{
    return opaScale_;
}

UIView::ViewExtraMsg* UIView::GetExtraMsg()
{
    return viewExtraMsg_;
}

void UIView::SetExtraMsg(ViewExtraMsg* extraMsg)
{
    viewExtraMsg_ = extraMsg;
}

#if ENABLE_SWEEP_CLOCK
void UIView::Rotate(float angle, const Vector2<float>& pivot)
#else
void UIView::Rotate(int16_t angle, const Vector2<float>& pivot)
#endif
{
    Vector3<float> pivotStart3D = Vector3<float>(pivot.x_, pivot.y_, 0);
    Vector3<float> pivotEnd3D = Vector3<float>(pivot.x_, pivot.y_, 1.0f);
    Rotate(angle, pivotStart3D, pivotEnd3D);
}

#if ENABLE_SWEEP_CLOCK
void UIView::Rotate(float angle, const Vector3<float>& pivotStart, const Vector3<float>& pivotEnd)
#else
void UIView::Rotate(int16_t angle, const Vector3<float>& pivotStart, const Vector3<float>& pivotEnd)
#endif
{
#if !ENABLE_MAP_BUFFER
    if (!CheckIsNeedMapBufferToTrans(__PRETTY_FUNCTION__)) {
        return;
    }
#endif
    if (transMap_ == nullptr) {
        ReMeasure();
        transMap_ = new TransformMap();
    }
    bool firstTrans = transMap_->IsInvalid();
    Rect joinRect = transMap_->GetBoxRect();
    transMap_->SetTransMapRect(GetOrigRect());
    transMap_->Rotate(angle, pivotStart, pivotEnd);
    if (firstTrans) {
        joinRect = transMap_->GetBoxRect();
        joinRect.Join(joinRect, GetOrigRect());
    } else {
        joinRect.Join(joinRect, transMap_->GetBoxRect());
    }
    InvalidateRect(joinRect);
}

void UIView::Scale(const Vector2<float>& scale, const Vector2<float>& pivot)
{
    Vector3<float> scale3D = Vector3<float>(scale.x_, scale.y_, 1.0f);
    Vector3<float> pivot3D = Vector3<float>(pivot.x_, pivot.y_, 0);
    Scale(scale3D, pivot3D);
}

void UIView::Scale(const Vector3<float>& scale, const Vector3<float>& pivot)
{
#if !ENABLE_MAP_BUFFER
    if (!CheckIsNeedMapBufferToTrans(__PRETTY_FUNCTION__)) {
        return;
    }
#endif
    if (transMap_ == nullptr) {
        ReMeasure();
        transMap_ = new TransformMap();
    }
    bool firstTrans = transMap_->IsInvalid();
    Rect joinRect = transMap_->GetBoxRect();
#if HW_RECT_ALIGN_RESTRICTION
#if ENABLE_VGU_ENGINE
    joinRect.SetX(joinRect.GetX() - 1);
    joinRect.SetY(joinRect.GetY() - 1);
    joinRect.SetWidth(joinRect.GetWidth() + 5); // 5: align size
    joinRect.SetHeight(joinRect.GetHeight() + 5); // 5: align size
#endif
#endif
    transMap_->SetTransMapRect(GetOrigRect());
    transMap_->Scale(scale, pivot);
    if (firstTrans) {
        joinRect = transMap_->GetBoxRect();
        joinRect.Join(joinRect, GetOrigRect());
    } else {
        joinRect.Join(joinRect, transMap_->GetBoxRect());
    }
    InvalidateRect(joinRect);
}

void UIView::Shear(const Vector2<float>& shearX, const Vector2<float>& shearY, const Vector2<float>& shearZ)
{
#if !ENABLE_MAP_BUFFER
    if (!CheckIsNeedMapBufferToTrans(__PRETTY_FUNCTION__)) {
        return;
    }
#endif
    if (transMap_ == nullptr) {
        ReMeasure();
        transMap_ = new TransformMap();
        if (transMap_ == nullptr) {
            return;
        }
    }
    bool firstTrans = transMap_->IsInvalid();
    Rect joinRect = transMap_->GetBoxRect();
    transMap_->SetTransMapRect(GetOrigRect());
    transMap_->Shear(shearX, shearY, shearZ);
    if (firstTrans) {
        joinRect = transMap_->GetBoxRect();
        joinRect.Join(joinRect, GetOrigRect());
    } else {
        joinRect.Join(joinRect, transMap_->GetBoxRect());
    }
    InvalidateRect(joinRect);
}

void UIView::Translate(const Vector2<int16_t>& trans)
{
    Vector3<int16_t> trans3D = Vector3<int16_t>(trans.x_, trans.y_, 0);
    Translate(trans3D);
}

void UIView::Translate(const Vector3<int16_t>& trans)
{
#if !ENABLE_MAP_BUFFER
    if (!CheckIsNeedMapBufferToTrans(__PRETTY_FUNCTION__)) {
        return;
    }
#endif
    if (transMap_ == nullptr) {
        ReMeasure();
        transMap_ = new TransformMap();
    }
    bool firstTrans = transMap_->IsInvalid();
    Rect joinRect = transMap_->GetBoxRect();
    transMap_->SetTransMapRect(GetOrigRect());
    transMap_->Translate(trans);
    if (firstTrans) {
        joinRect = transMap_->GetBoxRect();
        joinRect.Join(joinRect, GetOrigRect());
    } else {
        joinRect.Join(joinRect, transMap_->GetBoxRect());
    }
    InvalidateRect(joinRect);
}

bool UIView::IsTransInvalid()
{
    if (transMap_ == nullptr) {
        return true;
    }
    return transMap_->IsInvalid();
}

void UIView::SetCameraDistance(int16_t distance)
{
    if (transMap_ == nullptr) {
        ReMeasure();
        transMap_ = new TransformMap();
    }
    Rect joinRect = transMap_->GetBoxRect();
    transMap_->SetTransMapRect(GetOrigRect());
    transMap_->SetCameraDistance(distance);
    joinRect.Join(joinRect, transMap_->GetBoxRect());
    joinRect.Join(joinRect, GetOrigRect());
    InvalidateRect(joinRect);
}

void UIView::SetCameraPosition(const Vector2<float>& position)
{
    if (transMap_ == nullptr) {
        ReMeasure();
        transMap_ = new TransformMap();
    }
    Rect joinRect = transMap_->GetBoxRect();
    transMap_->SetTransMapRect(GetOrigRect());
    transMap_->SetCameraPosition(position);
    joinRect.Join(joinRect, transMap_->GetBoxRect());
    joinRect.Join(joinRect, GetOrigRect());
    InvalidateRect(joinRect);
}

void UIView::ResetTransParameter()
{
    if (transMap_ != nullptr) {
#if HW_RECT_ALIGN_RESTRICTION
#if ENABLE_VGU_ENGINE
        Rect joinRect = transMap_->GetBoxRect();
        joinRect.SetX(joinRect.GetX() - 1);
        joinRect.SetY(joinRect.GetY() - 1);
        joinRect.SetWidth(joinRect.GetWidth() + 5); // 5: align size
        joinRect.SetHeight(joinRect.GetHeight() + 5); // 5: align size
        delete transMap_;
        transMap_ = nullptr;
        joinRect.Join(joinRect, GetOrigRect());
        InvalidateRect(joinRect);
        return;
#endif
#endif
        delete transMap_;
        transMap_ = nullptr;
        Invalidate();
    }
}

#if ENABLE_VGU_ENGINE && CAPABILITY_HW_LIGHT
void UIView::EnableLight(bool enable)
{
    isEnableLight_ = enable;
    RootView::GetInstance()->Invalidate();
}

void UIView::SetSpeLightStyle(const LightSpeStyle& speLightStyle)
{
    if (lightStyle_ == nullptr) {
        lightStyle_ = new LightStyle{{LightType::LIGHT_BUTT, 0, 0, 0, 0, 0, 0, false}, 1.0};
    }
    lightStyle_->speStyle = speLightStyle;
    lightRect_ = GetRect();
    if (lightStyle_->ambAmp == 1.0) {
        HWDrawUtils::GetInstance()->CalLightRect(lightRect_, lightStyle_->speStyle);
    }
    InvalidateRect(lightRect_);
}

LightSpeStyle UIView::GetSpeLightStyle()
{
    return lightStyle_->speStyle;
}

void UIView::SetAmbLightStyle(float ambAmp)
{
    if (lightStyle_ == nullptr) {
        lightStyle_ = new LightStyle{{LightType::LIGHT_BUTT, 0, 0, 0, 0, 0, 0, false}, 1.0};
    }
    lightStyle_->ambAmp = ambAmp;
    lightRect_ = GetRect();
    InvalidateRect(lightRect_);
}

float UIView::GetAmbLightStyle()
{
    return lightStyle_->ambAmp;
}
#endif

void UIView::SetFullScreenBlur(bool isFull)
{
    if (isFull) {
        if (!RootView::GetInstance()->SetBlurView(this)) {
            fullScreenBlur_ = false;
            return;
        }
    } else {
        RootView::GetInstance()->ClearBlurView(this);
    }
    fullScreenBlur_ = isFull;
}

void UIView::SetBackgroundBlur(float radius, uint32_t quality)
{
    if (FloatEqual(radius, 0.0f)) {
        RootView::GetInstance()->ClearBlurView(this);
        blurRadius_ = 0.0f;
#if ENABLE_VGU_ENGINE && CAPABILITY_HW_GAUSSIAN_BLUR_BY_MASK
        ClearBlurMask();
#endif
    } else if ((!fullScreenBlur_ && !RootView::GetInstance()->GetBlurView()) || 
               (fullScreenBlur_)) {
        blurRadius_ = radius;
#if ENABLE_VGU_ENGINE && CAPABILITY_HW_GAUSSIAN_BLUR_BY_MASK
        blurQuality_ = quality;
#endif
    } else {
        GRAPHIC_LOGE("FullScreenBlurView already exists. Can not set normal blurView.");
    }
    RootView::GetInstance()->Invalidate();
}
 
void UIView::ClearBackgroundBlur()
{
    if (!FloatEqual(blurRadius_, 0.0f)) {
        if (fullScreenBlur_ && RootView::GetInstance()->GetBlurView() == this) {
            RootView::GetInstance()->ClearBlurView(this);
        }
        blurRadius_ = 0.0f;
#if ENABLE_VGU_ENGINE && CAPABILITY_HW_GAUSSIAN_BLUR_BY_MASK
        ClearBlurPath();
        ClearBlurMask();
#endif
        RootView::GetInstance()->Invalidate();
    }
}

#if ENABLE_VGU_ENGINE && CAPABILITY_HW_GAUSSIAN_BLUR_BY_MASK
void UIView::CreateBlurMask()
{
    if (blurMask_ == nullptr) {
        blurMask_ = static_cast<BufferInfo*>(UIMalloc(sizeof(BufferInfo)));
        uint32_t bufSize = ALIGN_BYTE(static_cast<uint32_t>(Screen::GetInstance().GetWidth()), BYTE_ALIGNMENT) *
                                                            Screen::GetInstance().GetHeight() * 1; // 1: A8 bytes
        blurMask_->phyAddr = AllocBufferForMask(bufSize);
        blurMask_->width = Screen::GetInstance().GetWidth();
        blurMask_->height = Screen::GetInstance().GetHeight();
        blurMask_->stride = DisplayDev::GetInstance()->CalcStride(Screen::GetInstance().GetWidth(), PIXEL_FMT_A8);
        blurMask_->mode = A8;
        blurMask_->compressMode = COMPRESS_MODE_NONE;
        blurMask_->rotType = 0;
    }
}

void UIView::ClearBlurMask()
{
    if (blurMask_ != nullptr) {
        LiteMGfxEngine::GetInstance()->SyncHwDraw();
        GrallocEngines::GetInstance()->FreeBuffer((uint8_t*)blurMask_->phyAddr);
        UIFree((uint8_t*)blurMask_);
        blurMask_ = nullptr;
    }
}

void UIView::SetBlurPath(uint32_t cmdNum, const uint8_t* cmd, uint32_t dataNum, const float* data)
{
    if (blurPath_ == nullptr) {
        blurPath_ = static_cast<Path*>(UIMalloc(sizeof(Path)));
        memset_s(blurPath_, sizeof(Path), 0, sizeof(Path));
    }

    if (cmdNum > blurPathCmdNum_) {
        if (blurPath_->cmds != nullptr) {
            UIFree((uint8_t*)blurPath_->cmds);
        }
        blurPath_->cmds = static_cast<uint8_t*>(UIMalloc(sizeof(uint8_t) * cmdNum));
        blurPathCmdNum_ = cmdNum;
    }
    blurPath_->cmdNum = cmdNum;
    if (memcpy_s(blurPath_->cmds, sizeof(uint8_t) * blurPath_->cmdNum, cmd, sizeof(uint8_t) * cmdNum) != EOK) {
        UIFree((uint8_t*)blurPath_->cmds);
        blurPath_->cmdNum = 0;
        blurPathCmdNum_ = 0;
        GRAPHIC_LOGE("memcpy for cmds failed.");
        return;
    }

    if (dataNum > blurPathDataNum_) {
        if (blurPath_->data != nullptr) {
            UIFree((float*)blurPath_->data);
        }
        blurPath_->data = static_cast<float*>(UIMalloc(sizeof(float) * dataNum));
        blurPathDataNum_ = dataNum;
    }
    blurPath_->dataNum = dataNum;
    if (memcpy_s(blurPath_->data, sizeof(float) * blurPath_->dataNum, data, sizeof(float) * dataNum) != EOK) {
        UIFree((float*)blurPath_->data);
        blurPath_->dataNum = 0;
        blurPathDataNum_ = 0;
        GRAPHIC_LOGE("memcpy for data failed.");
        return;
    }
    isBlurPathChange_ = true;
}

void UIView::ClearBlurPath()
{
    if (blurPath_ != nullptr) {
        if (blurPath_->cmds != nullptr) {
            UIFree((uint8_t*)blurPath_->cmds);
            blurPathCmdNum_ = 0;
        }
        if (blurPath_->data != nullptr) {
            UIFree((float*)blurPath_->data);
            blurPathDataNum_ = 0;
        }
        UIFree((uint8_t*)blurPath_);
        blurPath_ = nullptr;
        ClearBlurMask();
    }
}
#endif

#if ENABLE_ROTATE_INPUT
void UIView::RequestFocus()
{
    FocusManager::GetInstance()->RequestFocus(this);
}

void UIView::ClearFocus()
{
    if (FocusManager::GetInstance()->GetFocusedView() == this) {
        FocusManager::GetInstance()->ClearFocus();
    }
}
#endif

void UIView::Invalidate()
{
    UIView* view = this;
    while (view != nullptr) {
        if (view->transMap_ != nullptr && !view->GetTransformMap().IsInvalid() && view != this) {
            InvalidateRect(view->GetRect(), view);
            return;
        }
        view = view->parent_;
    }
    InvalidateRect(GetRect());
}

void UIView::InvalidateRect(const Rect& invalidatedArea, UIView* view)
{
    if (!visible_) {
        if (needRedraw_) {
            needRedraw_ = false;
        } else {
            return;
        }
    }

    if (view == nullptr) {
        view = this;
    }
    Rect trunc(invalidatedArea);
    bool isIntersect = true;
    UIView* par = view->parent_;
    UIView* cur = view;

    while (par != nullptr) {
        if (!par->visible_) {
            return;
        }

        isIntersect = trunc.Intersect(par->GetContentRect(), trunc);
        if (!isIntersect) {
            break;
        }

        cur = par;
        par = par->parent_;
    }

    if (isIntersect && (cur->GetViewType() == UI_ROOT_VIEW)) {
        RootView* rootView = reinterpret_cast<RootView*>(cur);
        if (!visible_) {
#if LOCAL_RENDER
            rootView->RemoveViewFromInvalidMap(this);
#endif
            rootView->AddInvalidateRectWithLock(trunc, parent_);
        } else {
            rootView->AddInvalidateRectWithLock(trunc, view);
        }
    }
}

bool UIView::OnLongPressEvent(const LongPressEvent& event)
{
    if (onLongPressListener_ != nullptr) {
        /* To ensure version compatibility, the listeners of both versions are invoked. */
        bool isConsumed = onLongPressListener_->OnLongPress(*this, event);
        return isConsumed;
    }
    return isIntercept_;
}

bool UIView::OnDragStartEvent(const DragEvent& event)
{
    if (onDragListener_ != nullptr) {
        /* To ensure version compatibility, the listeners of both versions are invoked. */
        bool isConsumed = onDragListener_->OnDragStart(*this, event);
        return isConsumed;
    }
    return isIntercept_;
}

bool UIView::OnDragEvent(const DragEvent& event)
{
    if (onDragListener_ != nullptr) {
        /* To ensure version compatibility, the listeners of both versions are invoked. */
        bool isConsumed = onDragListener_->OnDrag(*this, event);
        return isConsumed;
    }
    return isIntercept_;
}

bool UIView::OnDragEndEvent(const DragEvent& event)
{
    if (onDragListener_ != nullptr) {
        /* To ensure version compatibility, the listeners of both versions are invoked. */
        bool isConsumed = onDragListener_->OnDragEnd(*this, event);
        return isConsumed;
    }
    return isIntercept_;
}

bool UIView::OnClickEvent(const ClickEvent& event)
{
    if (onClickListener_ != nullptr) {
        /* To ensure version compatibility, the listeners of both versions are invoked. */
        bool isConsumed = onClickListener_->OnClick(*this, event);
        return isConsumed;
    }
    return isIntercept_;
}

bool UIView::OnPressEvent(const PressEvent& event)
{
    if (onTouchListener_ != nullptr) {
        /* To ensure version compatibility, the listeners of both versions are invoked. */
        bool isConsumed = onTouchListener_->OnPress(*this, event);
        return isConsumed;
    }
    return isIntercept_;
}

bool UIView::OnReleaseEvent(const ReleaseEvent& event)
{
    if (onTouchListener_ != nullptr) {
        /* To ensure version compatibility, the listeners of both versions are invoked. */
        bool isConsumed = onTouchListener_->OnRelease(*this, event);
        return isConsumed;
    }
    return isIntercept_;
}

bool UIView::OnCancelEvent(const CancelEvent& event)
{
    if (onTouchListener_ != nullptr) {
        /* To ensure version compatibility, the listeners of both versions are invoked. */
        bool isConsumed = onTouchListener_->OnCancel(*this, event);
        return isConsumed;
    }
    return isIntercept_;
}

void UIView::SetOnDragListener(OnDragListener* onDragListener)
{
    onDragListener_ = onDragListener;
}

UIView::OnDragListener*& UIView::GetOnDragListener()
{
    return onDragListener_;
}

void UIView::SetOnClickListener(OnClickListener* onClickListener)
{
    onClickListener_ = onClickListener;
}

UIView::OnClickListener*& UIView::GetOnClickListener()
{
    return onClickListener_;
}

void UIView::SetOnLongPressListener(OnLongPressListener* onLongPressListener)
{
    onLongPressListener_ = onLongPressListener;
}

UIView::OnLongPressListener*& UIView::GetOnLongPressListener()
{
    return onLongPressListener_;
}

void UIView::SetOnTouchListener(OnTouchListener* onTouchListener)
{
    onTouchListener_ = onTouchListener;
}

UIView::OnTouchListener*& UIView::GetTouchListener()
{
    return onTouchListener_;
}

void UIView::SetParent(UIView* parent)
{
    parent_ = parent;
}

UIView* UIView::GetParent() const
{
    return parent_;
}

void UIView::SetNextSibling(UIView* sibling)
{
    nextSibling_ = sibling;
}

UIView* UIView::GetNextSibling() const
{
    return nextSibling_;
}

void UIView::SetNextRenderSibling(UIView* renderSibling)
{
    nextRenderSibling_ = renderSibling;
}

UIView* UIView::GetNextRenderSibling() const
{
    return nextRenderSibling_;
}

void UIView::SetVisible(bool visible)
{
    if (visible_ != visible) {
        visible_ = visible;
        needRedraw_ = true;
        Invalidate();
    }
}

bool UIView::IsVisible() const
{
    return visible_;
}

void UIView::SetTouchable(bool touch)
{
    touchable_ = touch;
}

bool UIView::IsTouchable() const
{
    return touchable_;
}

void UIView::SetDraggable(bool draggable)
{
    draggable_ = draggable;
    dragParentInstead_ = !draggable;
}

bool UIView::IsDraggable() const
{
    return draggable_;
}

void UIView::SetDragParentInstead(bool dragParentInstead)
{
    dragParentInstead_ = dragParentInstead;
}

bool UIView::IsDragParentInstead() const
{
    return dragParentInstead_;
}

#if ENABLE_ROTATE_INPUT
bool UIView::OnRotateStartEvent(const RotateEvent& event)
{
    if (onRotateListener_ != nullptr) {
        return onRotateListener_->OnRotateStart(*this, event);
    }
    return false;
}

bool UIView::OnRotateEvent(const RotateEvent& event)
{
    if (onRotateListener_ != nullptr) {
        return onRotateListener_->OnRotate(*this, event);
    }
    return isIntercept_;
}

bool UIView::OnRotateEndEvent(const RotateEvent& event)
{
    if (onRotateListener_ != nullptr) {
        return onRotateListener_->OnRotateEnd(*this, event);
    }
    return false;
}

void UIView::SetOnRotateListener(OnRotateListener* onRotateListener)
{
    onRotateListener_ = onRotateListener;
}

#endif

void UIView::GetTargetView(const Point& point, UIView** last)
{
    if (last == nullptr) {
        return;
    }
    UIView* par = parent_;
    Rect rect = GetRect();

    if (par != nullptr) {
        rect.Intersect(par->GetContentRect(), rect);
    }

    if (visible_ && touchable_ && rect.IsContains(point)) {
        *last = this;
    }
}

void UIView::GetTargetView(const Point& point, UIView** current, UIView** target)
{
    if (current == nullptr) {
        return;
    }
    UIView* par = parent_;
    Rect rect = GetRect();

    if (par != nullptr) {
        rect.Intersect(par->GetOrigContentRect(), rect);
    }

    if (visible_ && rect.IsContains(point)) {
        if (touchable_) {
            *current = this;
        }
        *target = this;
    }
}

#if ENABLE_FOCUS_MANAGER
void UIView::SetFocusable(bool focusable)
{
    focusable_ = focusable;
}

bool UIView::IsFocusable() const
{
    return focusable_;
}

void UIView::Focus()
{
    if (focusable_ && onFocusListener_ != nullptr) {
        onFocusListener_->OnFocus(*this);
    }
}

void UIView::Blur()
{
    if (onFocusListener_ != nullptr) {
        onFocusListener_->OnBlur(*this);
    }
}

void UIView::SetOnFocusListener(OnFocusListener* onFocusListener)
{
    onFocusListener_ = onFocusListener;
}

UIView::OnFocusListener* UIView::GetOnFocusListener() const
{
    return onFocusListener_;
}

#endif

Rect UIView::GetRect() const
{
    if ((transMap_ != nullptr) && !transMap_->IsInvalid()) {
        Rect r = transMap_->GetBoxRect();
        Rect origRect = GetOrigRect();
        r.SetX(r.GetX() + origRect.GetX() - transMap_->GetTransMapRect().GetX());
        r.SetY(r.GetY() + origRect.GetY() - transMap_->GetTransMapRect().GetY());
        return r;
    }
    return GetOrigRect();
}

Rect UIView::GetContentRect()
{
    if ((transMap_ != nullptr) && !transMap_->IsInvalid()) {
        Rect r = transMap_->GetBoxRect();
        Rect origRect = GetOrigRect();
        r.SetX(r.GetX() + origRect.GetX() - transMap_->GetTransMapRect().GetX());
        r.SetY(r.GetY() + origRect.GetY() - transMap_->GetTransMapRect().GetY());
        return r;
    }

    Rect contentRect = GetRect();
    contentRect.SetX(contentRect.GetX() + style_->paddingLeft_ + style_->borderWidth_);
    contentRect.SetY(contentRect.GetY() + style_->paddingTop_ + style_->borderWidth_);
    contentRect.SetWidth(GetWidth());
    contentRect.SetHeight(GetHeight());
    return contentRect;
}

Rect UIView::GetOrigContentRect()
{
    Rect contentRect = GetOrigRect();
    contentRect.SetX(contentRect.GetX() + style_->paddingLeft_ + style_->borderWidth_);
    contentRect.SetY(contentRect.GetY() + style_->paddingTop_ + style_->borderWidth_);
    contentRect.SetWidth(GetWidth());
    contentRect.SetHeight(GetHeight());
    return contentRect;
}

Rect UIView::GetOrigRect() const
{
    int16_t x = rect_.GetX();
    int16_t y = rect_.GetY();
    UIView* par = parent_;
    while (par != nullptr) {
        x += par->GetRelativeRect().GetX() + par->GetStyle(STYLE_PADDING_LEFT) + par->GetStyle(STYLE_BORDER_WIDTH);
        y += par->GetRelativeRect().GetY() + par->GetStyle(STYLE_PADDING_TOP) + par->GetStyle(STYLE_BORDER_WIDTH);
        par = par->parent_;
    }
    return Rect(x, y, x + rect_.GetWidth() - 1, y + rect_.GetHeight() - 1);
}

Rect UIView::GetMaskedRect() const
{
    Rect mask;
    if (visibleRect_ != nullptr) {
        mask.Intersect(GetRect(), GetVisibleRect());
    } else {
        mask = GetRect();
    }
    return mask;
}

Rect UIView::GetVisibleRect() const
{
    if (visibleRect_ == nullptr) {
        return GetRect();
    }
    Rect absoluteRect;
    int16_t x = visibleRect_->GetX();
    int16_t y = visibleRect_->GetY();
    UIView* par = parent_;
    while (par != nullptr) {
        x += par->GetX();
        y += par->GetY();
        par = par->parent_;
    }
    absoluteRect.SetX(x);
    absoluteRect.SetY(y);
    absoluteRect.SetWidth(visibleRect_->GetWidth());
    absoluteRect.SetHeight(visibleRect_->GetHeight());
    return absoluteRect;
}

void UIView::SetTransformMap(const TransformMap& transMap)
{
#if !ENABLE_MAP_BUFFER
    if (!CheckIsNeedMapBufferToTrans(__PRETTY_FUNCTION__)) {
        return;
    }
#endif
    if ((transMap_ != nullptr) && (*transMap_ == transMap)) {
        return;
    }

    if (transMap_ == nullptr) {
        transMap_ = new TransformMap();
    }
    Rect preRect = GetRect();
#if HW_RECT_ALIGN_RESTRICTION
#if ENABLE_VGU_ENGINE
    preRect.SetX(preRect.GetX() - 1);
    preRect.SetY(preRect.GetY() - 1);
    preRect.SetWidth(preRect.GetWidth() + 5); // 5: align size
    preRect.SetHeight(preRect.GetHeight() + 5); // 5: align size
#endif
#endif
    *transMap_ = transMap;
    transMap_->SetTransMapRect(GetOrigRect());

    Rect joinRect;
    joinRect.Join(preRect, transMap_->GetBoxRect());
    InvalidateRect(joinRect);
}

void UIView::SetWidth(int16_t width)
{
    if (GetWidth() != width) {
        int16_t newWidth = width + style_->paddingLeft_ + style_->paddingRight_ +
                           (style_->borderWidth_ * 2); /* 2: left and right border */
        rect_.SetWidth(newWidth);
        isSizeChange_ = true;
    }
}

void UIView::SetWidthPercent(float widthPercent)
{
    if (IsInvalid(widthPercent)) {
        return;
    }
    if ((GetParent() != nullptr) && (GetParent()->GetWidth() > 1)) {
        int16_t newWidth = static_cast<int16_t>(GetParent()->GetWidth() * widthPercent);
        SetWidth(newWidth);
    }
}

int16_t UIView::GetWidth()
{
    return rect_.GetWidth() - (style_->paddingLeft_ + style_->paddingRight_) -
           (style_->borderWidth_ * 2); /* 2: left and right border */
}

void UIView::SetHeight(int16_t height)
{
    if (GetHeight() != height) {
        int16_t newHeight = height + style_->paddingTop_ + style_->paddingBottom_ +
                            (style_->borderWidth_ * 2); /* 2: top and bottom border */
        rect_.SetHeight(newHeight);
        isSizeChange_ = true;
    }
}

void UIView::SetHeightPercent(float heightPercent)
{
    if (IsInvalid(heightPercent)) {
        return;
    }
    if ((GetParent() != nullptr) && (GetParent()->GetHeight() > 1)) {
        int16_t newHeight = static_cast<int16_t>(GetParent()->GetHeight() * heightPercent);
        SetHeight(newHeight);
    }
}

int16_t UIView::GetHeight()
{
    return rect_.GetHeight() - (style_->paddingTop_ + style_->paddingBottom_) -
           (style_->borderWidth_ * 2); /* 2: top and bottom border */
}

void UIView::Resize(int16_t width, int16_t height)
{
    SetWidth(width);
    SetHeight(height);
}

void UIView::ResizePercent(float widthPercent, float heightPercent)
{
    if (IsInvalid(widthPercent) || IsInvalid(heightPercent)) {
        return;
    }
    if ((GetParent() != nullptr) && (GetParent()->GetWidth() > 1) && (GetParent()->GetHeight() > 1)) {
        int16_t newWidth = static_cast<int16_t>(GetParent()->GetWidth() * widthPercent);
        int16_t newHeight = static_cast<int16_t>(GetParent()->GetHeight() * heightPercent);
        Resize(newWidth, newHeight);
    }
}

void UIView::SetX(int16_t x)
{
    if (GetX() != x) {
        if (isAlignEnable_) {
            alignType_ = ALIGN_DEFAULT;
        }
        rect_.SetX(x + GetStyle(STYLE_MARGIN_LEFT));
    }
}

void UIView::SetXPercent(float xPercent)
{
    if (IsInvalid(xPercent)) {
        return;
    }
    if ((GetParent() != nullptr) && (GetParent()->GetWidth() > 1)) {
        int16_t newX = static_cast<int16_t>(GetParent()->GetWidth() * xPercent);
        SetX(newX);
    }
}

int16_t UIView::GetX() const
{
    return rect_.GetX() - GetStyle(STYLE_MARGIN_LEFT);
}

void UIView::SetY(int16_t y)
{
    if (GetY() != y) {
        if (isAlignEnable_) {
            alignType_ = ALIGN_DEFAULT;
        }
        rect_.SetY(y + GetStyle(STYLE_MARGIN_TOP));
    }
}

void UIView::SetYPercent(float yPercent)
{
    if (IsInvalid(yPercent)) {
        return;
    }
    if ((GetParent() != nullptr) && (GetParent()->GetHeight() > 1)) {
        int16_t newY = static_cast<int16_t>(GetParent()->GetHeight() * yPercent);
        SetY(newY);
    }
}

int16_t UIView::GetY() const
{
    return rect_.GetY() - GetStyle(STYLE_MARGIN_TOP);
}

void UIView::SetPosition(int16_t x, int16_t y)
{
    SetX(x);
    SetY(y);
}

void UIView::SetPositionPercent(float xPercent, float yPercent)
{
    if (IsInvalid(xPercent) || IsInvalid(yPercent)) {
        return;
    }
    if ((GetParent() != nullptr) && (GetParent()->GetWidth() > 1) && (GetParent()->GetHeight() > 1)) {
        int16_t newX = static_cast<int16_t>(GetParent()->GetWidth() * xPercent);
        int16_t newY = static_cast<int16_t>(GetParent()->GetHeight() * yPercent);
        SetPosition(newX, newY);
    }
}

void UIView::SetPosition(int16_t x, int16_t y, int16_t width, int16_t height)
{
    SetPosition(x, y);
    SetWidth(width);
    SetHeight(height);
}

void UIView::SetPositionPercent(float xPercent, float yPercent, float widthPercent, float heightPercent)
{
    if (IsInvalid(xPercent) || IsInvalid(yPercent) || IsInvalid(widthPercent) || IsInvalid(heightPercent)) {
        return;
    }
    if ((GetParent() != nullptr) && (GetParent()->GetWidth() > 1) && (GetParent()->GetHeight() > 1)) {
        int16_t newX = static_cast<int16_t>(GetParent()->GetWidth() * xPercent);
        int16_t newY = static_cast<int16_t>(GetParent()->GetHeight() * yPercent);
        int16_t newWidth = static_cast<int16_t>(GetParent()->GetWidth() * widthPercent);
        int16_t newHeight = static_cast<int16_t>(GetParent()->GetHeight() * heightPercent);
        SetPosition(newX, newY, newWidth, newHeight);
    }
}

bool UIView::IsViewGroup() const
{
    return isViewGroup_;
}

void UIView::SetIntercept(bool isIntercept)
{
    isIntercept_ = isIntercept;
}

bool UIView::IsIntercept()
{
    return isIntercept_;
}

bool UIView::IsInvalid(float percent)
{
    if ((percent < 1) && (percent > 0)) {
        return false;
    }
    return true;
}

TransformMap& UIView::GetTransformMap()
{
    if (transMap_ == nullptr) {
        transMap_ = new TransformMap();
    }
    return *transMap_;
}

UIView* UIView::GetChildById(const char* id) const
{
    return nullptr;
}

void UIView::SetViewId(const char* id)
{
    id_ = id;
}

const char* UIView::GetViewId() const
{
    return id_;
}

void UIView::SetViewIndex(int16_t index)
{
    index_ = index;
}

int16_t UIView::GetViewIndex() const
{
    return index_;
}

UIViewType UIView::GetViewType() const
{
    return UI_NUMBER_MAX;
}

void UIView::LayoutCenterOfParent(int16_t xOffset, int16_t yOffset)
{
    if (parent_ == nullptr) {
        return;
    }

    int16_t topMargin = style_->marginTop_;
    int16_t leftMargin = style_->marginLeft_;
    int16_t rightMargin = style_->marginRight_;
    int16_t bottomMargin = style_->marginBottom_;
    // 2: half
    int16_t posX = parent_->GetWidth() / 2 - (rect_.GetWidth() - leftMargin + rightMargin) / 2 + xOffset;
    // 2: half
    int16_t posY = parent_->GetHeight() / 2 - (rect_.GetHeight() - topMargin + bottomMargin) / 2 + yOffset;
    SetPosition(posX, posY);
}

void UIView::LayoutLeftOfParent(int16_t offset)
{
    if (parent_ == nullptr) {
        return;
    }

    int16_t leftMargin = style_->marginLeft_;
    SetPosition(leftMargin + offset, GetY());
}

void UIView::LayoutRightOfParent(int16_t offset)
{
    if (parent_ == nullptr) {
        return;
    }

    int16_t rightMargin = style_->marginRight_;
    SetPosition(parent_->GetWidth() - offset - rect_.GetWidth() - rightMargin, GetY());
}

void UIView::LayoutTopOfParent(int16_t offset)
{
    if (parent_ == nullptr) {
        return;
    }

    int16_t topMargin = style_->marginTop_;
    SetPosition(GetX(), topMargin + offset);
}

void UIView::LayoutBottomOfParent(int16_t offset)
{
    if (parent_ == nullptr) {
        return;
    }

    int16_t bottomMargin = style_->marginBottom_;
    SetPosition(GetX(), parent_->GetHeight() - offset - rect_.GetHeight() - bottomMargin);
}

void UIView::AlignLeftToSibling(const char* id, int16_t offset)
{
    if (parent_ == nullptr) {
        return;
    }
    UIView* sib = parent_->GetChildById(id);
    if (sib != nullptr) {
        int16_t margin = sib->style_->marginLeft_ - style_->marginLeft_;
        SetPosition(sib->GetX() - margin + offset, GetY());
    }
}

void UIView::AlignRightToSibling(const char* id, int16_t offset)
{
    if (parent_ == nullptr) {
        return;
    }
    UIView* sib = parent_->GetChildById(id);
    if (sib != nullptr) {
        int16_t margin = sib->style_->marginRight_ - style_->marginRight_;
        SetPosition(sib->GetX() + sib->rect_.GetWidth() - rect_.GetWidth() - offset + margin, GetY());
    }
}

void UIView::AlignTopToSibling(const char* id, int16_t offset)
{
    if (parent_ == nullptr) {
        return;
    }
    UIView* sib = parent_->GetChildById(id);
    if (sib != nullptr) {
        int16_t margin = sib->style_->marginTop_ - style_->marginTop_;
        SetPosition(GetX(), sib->GetY() + offset - margin);
    }
}

void UIView::AlignBottomToSibling(const char* id, int16_t offset)
{
    if (parent_ == nullptr) {
        return;
    }
    UIView* sib = parent_->GetChildById(id);
    if (sib != nullptr) {
        int16_t margin = sib->style_->marginBottom_ - style_->marginBottom_;
        SetPosition(GetX(), sib->GetY() + sib->rect_.GetHeight() - rect_.GetHeight() - offset + margin);
    }
}

void UIView::AlignHorCenterToSibling(const char* id, int16_t offset)
{
    if (parent_ == nullptr) {
        return;
    }
    UIView* sib = parent_->GetChildById(id);
    if (sib != nullptr) {
        int16_t margin =
            (sib->style_->marginRight_ - sib->style_->marginLeft_ - style_->marginRight_ + style_->marginLeft_) /
            2; // 2 : half
        SetPosition(sib->GetX() + sib->rect_.GetWidth() / 2 - rect_.GetWidth() / 2 + margin + offset, GetY());
    }
}
void UIView::AlignVerCenterToSibling(const char* id, int16_t offset)
{
    if (parent_ == nullptr) {
        return;
    }
    UIView* sib = parent_->GetChildById(id);
    if (sib != nullptr) {
        int16_t margin =
            (sib->style_->marginBottom_ - sib->style_->marginTop_ - style_->marginBottom_ + style_->marginTop_) /
            2; // 2 : half
        SetPosition(GetX(), sib->GetY() + sib->rect_.GetHeight() / 2 - rect_.GetHeight() / 2 + margin + offset);
    }
}

void UIView::UpdatePos()
{
    // 如果没有设置过对齐关系,或者为外部对齐 或者父容器不存在，不处理
    if (alignType_ == ALIGN_DEFAULT || alignType_ >= ALIGN_OUT_TOP_LEFT || parent_ == nullptr) {
        return;
    }

    int16_t rx = parent_->GetX();
    int16_t ry = parent_->GetY();
    int16_t rw = parent_->GetWidth();
    int16_t rh = parent_->GetHeight();

    // 如果父容器尺寸和自身尺寸没有发生变化，不需要处理
    if (IsParentSizeStabled(rw, rh) && !isSizeChange_) {
        return;
    }
    isSizeChange_ = false;

    int16_t x = 0;
    int16_t y = 0;
    int16_t w = GetWidth();
    int16_t h = GetHeight();

    // 提取对齐位置 (高低各占4位)
    uint8_t alignHigh = (alignType_ & ALIGN_UP_MASK) >> 4;
    uint8_t alignLow = (alignType_ & ALIGN_DOWUN_MASK);
    
    x = alignLow * (rw - w) / 2;
    y = alignHigh * (rh - h) / 2;
    
    // 计算偏移量
    x += ofsX_;
    y += ofsY_;
    SetPositionInner(x, y);
}

void UIView::AlignToReference(AlignmentType alignType, UIView* referView, int16_t ofsX, int16_t ofsY)
{
    if (referView == nullptr && parent_ == nullptr) {
        return;
    }

    // 实际使用的参考控件
    UIView* actualReferView = (referView != nullptr) ? referView : parent_;

    if (actualReferView == parent_) {
        if (alignType >= ALIGN_OUT_TOP_LEFT) {
            GRAPHIC_LOGE("UIView::AlignToReference, alignment type set invalid");
            return;
        } else {
            pPrevWidth_ = parent_->GetWidth();
            pPrevHeight_ = parent_->GetHeight();
        }
    }
    isSizeChange_ = false;
    alignType_ = alignType;
    // 获取参考控件的几何信息
    int16_t rx = actualReferView->GetX();
    int16_t ry = actualReferView->GetY();
    int16_t rw = actualReferView->GetWidth();
    int16_t rh = actualReferView->GetHeight();

    // 获取待对齐控件的几何信息
    int16_t x = 0;
    int16_t y = 0;
    int16_t w = GetWidth();
    int16_t h = GetHeight();

    bool isOuter = (alignType & ALIGN_OUTER_MASK) != 0;
    
    // 提取对齐位置 (高低各占4位)
    uint8_t alignHigh = (alignType & ALIGN_UP_MASK) >> 4;
    uint8_t alignLow = (alignType & ALIGN_DOWUN_MASK);

    // 基础对齐计算
    if (!isOuter) {
        x = alignLow * (rw - w) / 2;
        y = alignHigh * (rh - h) / 2;
    } else {
        // 外部对齐计算
        if (alignHigh <= 1) {
            x = rx + alignLow * (rw - w) / 2;
            y = ry + (alignHigh * (rh + h)) - h;
        } else {
            x = rx + ((alignHigh - 2) * (rw + w)) - w;
            y = ry + alignLow * (rh - h) / 2;
        }
    }

    ofsX_ = ofsX;
    ofsY_ = ofsY;
 
    // 计算偏移量
    x += ofsX;
    y += ofsY;
    SetPositionInner(x, y);
}

void UIView::LayoutLeftToSibling(const char* id, int16_t offset)
{
    if (parent_ == nullptr) {
        return;
    }
    UIView* sib = parent_->GetChildById(id);
    if (sib != nullptr) {
        int16_t margin = sib->style_->marginLeft_ + style_->marginRight_;
        SetPosition(sib->GetX() - offset - rect_.GetWidth() - margin, GetY());
    }
}

void UIView::LayoutRightToSibling(const char* id, int16_t offset)
{
    if (parent_ == nullptr) {
        return;
    }
    UIView* sib = parent_->GetChildById(id);
    if (sib != nullptr) {
        int16_t margin = sib->style_->marginRight_ + style_->marginLeft_;
        SetPosition(sib->GetX() + sib->rect_.GetWidth() + offset + margin, GetY());
    }
}

void UIView::LayoutTopToSibling(const char* id, int16_t offset)
{
    if (parent_ == nullptr) {
        return;
    }
    UIView* sib = parent_->GetChildById(id);
    if (sib != nullptr) {
        int16_t margin = sib->style_->marginTop_ + style_->marginBottom_;
        SetPosition(GetX(), sib->GetY() - offset - rect_.GetHeight() - margin);
    }
}

void UIView::LayoutBottomToSibling(const char* id, int16_t offset)
{
    if (parent_ == nullptr) {
        return;
    }
    UIView* sib = parent_->GetChildById(id);
    if (sib != nullptr) {
        int16_t margin = sib->style_->marginBottom_ + style_->marginTop_;
        SetPosition(GetX(), sib->GetY() + sib->rect_.GetHeight() + offset + margin);
    }
}

uint8_t UIView::GetMixOpaScale() const
{
    uint8_t opaMix = opaScale_;
    UIView* parent = parent_;
    while (parent != nullptr) {
        uint8_t opaParent = parent->GetOpaScale();
        // 8: Shift right 8 bits
        opaMix = (opaParent == OPA_OPAQUE) ? opaMix : ((static_cast<uint16_t>(opaParent) * opaMix) >> 8);
        parent = parent->GetParent();
    }
    return opaMix;
}

bool UIView::GetBitmap(ImageInfo& imageInfo, ColorMode colorMode)
{
    UIView* tempRenderSibling = nextRenderSibling_;
    nextRenderSibling_ = nullptr;
    UIView* tempParent = parent_;
    parent_ = nullptr;
    int16_t tempX = rect_.GetX();
    int16_t tempY = rect_.GetY();
    rect_.SetPosition(0, 0);

    Rect screenRect(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    Rect mask = GetRect();
    mask.Intersect(mask, screenRect);
    BufferInfo bufInfo{mask, 0, nullptr, nullptr, 0, 0, colorMode, 0};
    bufInfo.width = mask.GetWidth();
    bufInfo.height = mask.GetHeight();

#if ENABLE_FB_COMPRESS
    imageInfo.header.compressMode = COMPRESS_MODE_HFBC_3X;
#else
    imageInfo.header.compressMode = COMPRESS_MODE_NONE;
#endif
    bufInfo.compressMode = imageInfo.header.compressMode;

#ifdef VERSION_IOT
    bufInfo.stride = DisplayDev::GetInstance()->CalcStride(bufInfo.width,
        PIXEL_FMT_RGB_888, imageInfo.header.compressMode);
#else
    bufInfo.stride = bufInfo.width * DrawUtils::GetByteSizeByColorMode(bufInfo.mode);
#endif
    bufInfo.rotType = 0;

    BaseGfxEngine::GetInstance()->AdjustLineStride(bufInfo);
    imageInfo.header.colorMode = bufInfo.mode;
    imageInfo.header.width = bufInfo.width;
    imageInfo.header.height = bufInfo.height;
    imageInfo.header.reserved = 0;
#ifdef VERSION_IOT
    imageInfo.dataSize = DisplayDev::GetInstance()->CalcSize(bufInfo.stride, imageInfo.header.height, imageInfo.header.compressMode);
#else
    imageInfo.dataSize = bufInfo.stride * bufInfo.height;
#endif
    bufInfo.virAddr = ImageCacheMalloc(imageInfo);
    if (bufInfo.virAddr == nullptr) {
        GRAPHIC_LOGE("GetBitmap buffer alloc failed.");
        nextRenderSibling_ = tempRenderSibling;
        parent_ = tempParent;
        rect_.SetPosition(tempX, tempY);
        return false;
    }
    imageInfo.data = reinterpret_cast<uint8_t*>(bufInfo.virAddr);
    if (memset_s(bufInfo.virAddr, imageInfo.dataSize, 0, imageInfo.dataSize) != EOK) {
        GRAPHIC_LOGE("GetBitmap buffer memset failed.");
        ImageCacheFree(imageInfo);
        imageInfo.data = nullptr;
        return false;
    }
    bufInfo.phyAddr = bufInfo.virAddr;
    if ((DrawUtils::GetMixOpacity(opaScale_, style_->bgOpa_) != OPA_OPAQUE) ||
        (style_->borderRadius_ != 0)) {
        LiteMGfxEngine::GetInstance()->Fill(bufInfo, Rect(0, 0, bufInfo.width - 1, bufInfo.height - 1),
            Color::Black(), OPA_OPAQUE);
    }

    RootView* rootView = RootView::GetInstance();
    rootView->SaveDrawContext();
    rootView->UpdateBufferInfo(&bufInfo);
    rootView->SetSnapshotFlag(true);
    rootView->MeasureView(this);
    rootView->DrawTop(this, mask);
#if ENABLE_VGU_ENGINE
    LiteMGfxEngine::GetInstance()->SyncHwDraw();
#endif
    rootView->RestoreDrawContext();
    rootView->SetSnapshotFlag(false);
    nextRenderSibling_ = tempRenderSibling;
    parent_ = tempParent;
    rect_.SetPosition(tempX, tempY);
    return true;
}


bool UIView::IsOnViewTree()
{
    UIView* par = this;
    while (par->GetParent() != nullptr) {
        par = par->GetParent();
    }
    if (par->GetViewType() != UI_ROOT_VIEW) {
        return false;
    }
    return true;
}

int16_t UIView::GetWidthWithMargin()
{
    return GetRelativeRect().GetWidth() + GetStyle(STYLE_MARGIN_LEFT) + GetStyle(STYLE_MARGIN_RIGHT);
}

int16_t UIView::GetHeightWithMargin()
{
    return GetRelativeRect().GetHeight() + GetStyle(STYLE_MARGIN_TOP) + GetStyle(STYLE_MARGIN_BOTTOM);
}

Rect UIView::GetRelativeRect() const
{
    return rect_;
}

void UIView::ResizeVisibleArea(int16_t x, int16_t y, int16_t width, int16_t height)
{
    if (visibleRect_ == nullptr) {
        visibleRect_ = new Rect();
        if (visibleRect_ == nullptr) {
            GRAPHIC_LOGE("new Rect fail");
            return;
        }
    }
    visibleRect_->SetWidth(width);
    visibleRect_->SetHeight(height);
    visibleRect_->SetPosition(x, y);
}

void UIView::SetZIndex(int16_t zIndex)
{
    if (zIndex_ == zIndex) {
        return;
    }

    zIndex_ = zIndex;
    if (parent_ != nullptr) {
        reinterpret_cast<UIViewGroup*>(parent_)->UpdateRenderView(this); // wait_check_again
    }
}

int16_t UIView::GetZIndex()
{
    return zIndex_;
}

bool UIView::IsVideoContained()
{
    if (GetViewType() == UI_LITE_SURFACE_VIEW) {
        return true;
    }
    UIViewGroup* group = dynamic_cast<UIViewGroup*>(this);
    if (group == nullptr) {
        return false;
    }

    UIView* view = group->GetChildrenHead();
    while (view != nullptr) {
        if (view->IsVideoContained()) {
            return true;
        }
        view = view->GetNextSibling();
    }
    return false;
}

void UIView::PrintImgInfo(const ImageInfo* info)
{
    if (info == nullptr) {
        printf("    ImgInfo[%p]\n", info);
    } else {
        printf("    ImgInfo[%p]: size = [%d, %d], fmt = %d, data = %p, dataSize = %d, file = %s, resId = %u\n",
            info, info->header.width, info->header.height, info->header.colorMode,
            info->data, info->dataSize, info->file, info->resId);
    }
}

void UIView::DumpImgInfoRecursive()
{
    DumpImgInfo();

    if (IsViewGroup()) {
        UIView* child = static_cast<UIViewGroup*>(this)->GetChildrenHead();
        while (child != nullptr) {
            child->DumpImgInfoRecursive();
            child = child->GetNextSibling();
        }
    }
}

std::string UIView::GetGuiInfo() const
{
    return "";
}
 
#if !ENABLE_MAP_BUFFER
bool UIView::CheckIsNeedMapBufferToTrans(const char* funcName)
{
    UIViewType type = GetViewType();
    if ((type != UI_IMAGE_VIEW) && (type != UI_TEXTURE_MAPPER) && (type != UI_CANVAS_EXT) && (type != UI_MAP_VIEW) &&
        (type != UI_MENU_ITEM) && (type != UI_LABEL_EXT) && (type != UI_LABEL) && (type != UI_TRANSFORM_GROUP) &&
        (type != UI_IMAGE_ANIMATOR_VIEW) && (type != UI_SLIDER) && (type != UI_LOTT_VIEW) && ( type != UI_POST_EFFECT_IMAGE_VIEW) &&
        (type != UI_TRANSFORM_BUTTON) && (type != UI_BARCODE)) {
        GRAPHIC_LOGE("%s is not supported for view type %d if ENABLE_MAP_BUFFER is 0", funcName, type);
        return false;
    }
    return true;
}
#endif
} // namespace OHOS
