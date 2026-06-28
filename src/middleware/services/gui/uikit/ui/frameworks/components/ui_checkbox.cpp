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

#include "components/ui_checkbox.h"
#include "default_resource/check_box_res.h"
#include "draw/draw_image.h"
#include "engines/gfx/gfx_engine_manager.h"
#include "imgdecode/cache_manager.h"
#if ENABLE_VGU_ENGINE
#include "engines/gfx/lite_m_gfx_engine.h"
#include "draw/hw_draw_utils.h"
#include "gralloc_engines.h"
#endif
namespace OHOS {
namespace {
constexpr uint8_t DEFAULT_UNSELECT_BG_OPA = 168;     // default background opacity
constexpr float DEFAULT_COEFFICIENT_START_DX = 0.22; // start point: x-cordinate offset
constexpr float DEFAULT_COEFFICIENT_START_DY = 0.5;  // start point: y-cordinate offset
constexpr float DEFAULT_COEFFICIENT_MID_DX = 0.2;    // middle point: y-cordinate offset
constexpr float DEFAULT_COEFFICIENT_MID_DY = 0.38;   // middle point: y-cordinate offset
constexpr int16_t DEFAULT_RATIO_BORDER_RADIUS_LINE_WIDTH = 4;
constexpr uint8_t DEFAULT_BG_RED = 31;
constexpr uint8_t DEFAULT_BG_GREEN = 113;
constexpr uint8_t DEFAULT_BG_BLUE = 255;
#if DEFAULT_ANIMATION
constexpr int16_t DEFAULT_ANIMATOR_TIME = 200;
constexpr float BEZIER_CONTROL_POINT_X_1 = 0.33;
constexpr float BEZIER_CONTROL_POINT_X_2 = 0.67;
#endif
} // namespace
UICheckBox::UICheckBox()
    : state_(UNSELECTED),
      onStateChangeListener_(nullptr),
      width_(DEFAULT_HOT_WIDTH),
      height_(DEFAULT_HOT_HEIGHT),
      borderWidth_(DEFAULT_BORDER_WIDTH),
      backgroundOpacity_(0)
{
    touchable_ = true;
    style_ = &(StyleDefault::GetBackgroundTransparentStyle());
#if defined(ENABLE_DEFAULT_CHECKBOX_IMAGE) && (ENABLE_DEFAULT_CHECKBOX_IMAGE == 1) && !ENABLE_VGU_ENGINE
    image_[UNSELECTED].SetSrc(GetCheckBoxOffInfo());
    image_[SELECTED].SetSrc(GetCheckBoxOnInfo());
    ImageHeader header = {0};
    image_[UNSELECTED].GetHeader(header);
    Resize(header.width, header.height);
#endif
#if DEFAULT_ANIMATION
    runTime_ = 0;
    checkBoxAnimator_ = Animator(this, this, DEFAULT_ANIMATOR_TIME, false);
#endif
    selectedStateColor_ = Color::GetColorFromRGB(DEFAULT_BG_RED, DEFAULT_BG_GREEN, DEFAULT_BG_BLUE);
}

void UICheckBox::SetState(UICheckBoxState state, bool needAnimater)
{
    if (state_ == state) {
        return;
    }
    state_ = state;
    if ((image_[SELECTED].GetSrcType() == IMG_SRC_UNKNOWN) || (image_[UNSELECTED].GetSrcType() == IMG_SRC_UNKNOWN)) {
#if DEFAULT_ANIMATION
        if (needAnimater) {
            checkBoxAnimator_.Start();
            ResetCallback();
        } else {
            backgroundOpacity_ = (state_ == SELECTED) ? OPA_OPAQUE : 0;
        }
#else
        backgroundOpacity_ = (state_ == SELECTED) ? OPA_OPAQUE : 0;
#endif
    }
    if (onStateChangeListener_ != nullptr) {
        onStateChangeListener_->OnChange(state);
    }
    Invalidate();
}

void UICheckBox::ReverseState()
{
    if (state_ == SELECTED) {
        SetState(UNSELECTED, true);
    } else {
        SetState(SELECTED, true);
    }
}

bool UICheckBox::OnClickEvent(const ClickEvent& event)
{
    ReverseState();
    Invalidate();
    return UIView::OnClickEvent(event);
}

void UICheckBox::SetImages(const char* selectedImageSrc, const char* unselectedImageSrc)
{
    image_[SELECTED].SetSrc(selectedImageSrc);
    image_[UNSELECTED].SetSrc(unselectedImageSrc);
}

void UICheckBox::SetImages(const ImageInfo* selectedImageSrc, const ImageInfo* unselectedImageSrc)
{
    image_[SELECTED].SetSrc(selectedImageSrc);
    image_[UNSELECTED].SetSrc(unselectedImageSrc);
}

void UICheckBox::CalculateSize()
{
    int16_t width = GetWidth();
    int16_t height = GetHeight();
    if ((width_ == width) && (height_ == height)) {
        return;
    }
    width_ = width;
    height_ = height;
    int16_t minValue = (width_ > height_) ? height_ : width_;
    borderWidth_ = DEFAULT_BORDER_WIDTH * minValue / DEFAULT_HOT_WIDTH;
}

UICheckBox::~UICheckBox()
{
#if ENABLE_VGU_ENGINE
    if (path_ != nullptr) {
        LiteMGfxEngine::GetInstance()->SyncHwDraw();
        if (path_->cmds != nullptr) {
            GrallocEngines::GetInstance()->FreeBuffer(path_->cmds);
            path_->cmds = nullptr;
        }
        if (path_->data != nullptr) {
            GrallocEngines::GetInstance()->FreeBuffer(reinterpret_cast<uint8_t*>(path_->data));
            path_->data = nullptr;
        }
        delete path_;
        path_ = nullptr;
    }
#endif
}

#if ENABLE_VGU_ENGINE
static void FillTickPath(Path* path, Point& start, Point& mid, Point& end)
{
    if (path != nullptr) {
        path->cmds[0] = CMD_MOVE_TO;
        path->cmds[1] = CMD_LINE_TO;
        path->cmds[2] = CMD_LINE_TO;

        path->data[0] = start.x - 1;
        path->data[1] = start.y - 1;
        path->data[2] = mid.x;
        path->data[3] = mid.y + 1;
        path->data[4] = end.x + 1;
        path->data[5] = end.y - 1;
    }
}

void UICheckBox::DrawForegroundPath(BufferInfo& gfxDstBuffer, Rect trunc,
    VGUStrokeAttr* strokeAttr, VGUFillAttr* fillAttr)
{
    uintptr_t vguPath = LiteMGfxEngine::GetInstance()->InitPath(true);
    LiteMGfxEngine::GetInstance()->SetPathData(vguPath, path_->cmdNum, path_->cmds, path_->dataNum, path_->data);

    if (transMap_ == nullptr) {
        LiteMGfxEngine::GetInstance()->RenderPath(gfxDstBuffer, vguPath, trunc, strokeAttr, fillAttr, nullptr);
    } else {
        VGUMatrix3 mat;
        Matrix4<float> matrix = transMap_->GetOrigTransformMatrix();
        HWDrawUtils::GetInstance()->Matrix4ToVguMatrix3(matrix, mat);
        LiteMGfxEngine::GetInstance()->RenderPath(gfxDstBuffer, vguPath, trunc, strokeAttr, fillAttr, &mat);
    }
    LiteMGfxEngine::GetInstance()->DeinitPath(vguPath);
}

void UICheckBox::AllocPath(uint16_t cmdNum, uint16_t dataNum)
{
    if (path_ != nullptr) {
        return;
    }

    path_ = new Path();
    if (path_ == nullptr) {
        return;
    }
    path_->cmdNum = cmdNum;
    if (!HWDrawUtils::GetInstance()->AllocBufferForCmd(path_->cmdNum, path_->cmds)) {
        delete path_;
        path_ = nullptr;
        return;
    }
    path_->dataNum = dataNum;
    if (!HWDrawUtils::GetInstance()->AllocBufferForData(path_->dataNum, path_->data)) {
        GrallocEngines::GetInstance()->FreeBuffer(path_->cmds);
        path_->cmds = nullptr;
        delete path_;
        path_ = nullptr;
    }
}
#endif
void UICheckBox::SelectedStateSoftwareDrawing(BufferInfo& gfxDstBuffer,
                                              Rect rect,
                                              Rect trunc,
                                              int16_t borderRadius,
                                              int16_t rectLineWidth)
{
    if (backgroundOpacity_ == 0) {
        return;
    }
    Style styleSelect = StyleDefault::GetBackgroundTransparentStyle();
    styleSelect.borderRadius_ = borderRadius;
    styleSelect.bgColor_ = selectedStateColor_;
    styleSelect.bgOpa_ = backgroundOpacity_;
    BaseGfxEngine* baseGfxEngine = BaseGfxEngine::GetInstance();
#if ENABLE_VGU_ENGINE
    LiteMGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, rect, trunc, styleSelect, opaScale_, transMap_);
#else
    baseGfxEngine->DrawRect(gfxDstBuffer, rect, trunc, styleSelect, opaScale_);
#endif
    int16_t dx = static_cast<int16_t>(borderWidth_ * DEFAULT_COEFFICIENT_START_DX);
    int16_t dy = static_cast<int16_t>(borderWidth_ * DEFAULT_COEFFICIENT_START_DY);
    Point start = {static_cast<int16_t>(rect.GetX() + dx), static_cast<int16_t>(rect.GetY() + dy)};
    dx = static_cast<int16_t>(borderWidth_ * DEFAULT_COEFFICIENT_MID_DX);
    Point mid = {static_cast<int16_t>(start.x + dx), static_cast<int16_t>(start.y + dx)};
    dx = static_cast<int16_t>(borderWidth_ * DEFAULT_COEFFICIENT_MID_DY);
    Point end = {static_cast<int16_t>(mid.x + dx), static_cast<int16_t>(mid.y - dx)};
    const int16_t half = 2; // 2 ：half
#if ENABLE_VGU_ENGINE
    if (path_ == nullptr) {
        AllocPath(3, 6); // 3: Circle path command number; 6: Circle path data number;
    }
    if (path_ != nullptr) {
        FillTickPath(path_, start, mid, end);
        VGUStrokeAttr strokeAttr = {VGU_LINECAP_ROUND};
        VGUPaintStyle strokeStyle = {0};
        uint8_t opa = DrawUtils::GetMixOpacity(opaScale_, backgroundOpacity_);
        uint8_t mixedAlpha = ((Color::White().full >> 24) * opa) / 0Xff; // 24 : move bit
        strokeStyle.color = (mixedAlpha << 24) | (Color::White().full & 0x00FFFFFF); // 24 : move bit
        strokeStyle.type = VGU_PAINT_COLOR;
        strokeAttr.width = rectLineWidth * 2;
        strokeAttr.style = &strokeStyle;
        DrawForegroundPath(gfxDstBuffer, trunc, &strokeAttr, nullptr);
    }
#else
    ArcInfo arcInfoLeft = {start,
                           {0, 0},
                           static_cast<uint16_t>(rectLineWidth),
                           SEMICIRCLE_IN_DEGREE + QUARTER_IN_DEGREE / half,
                           QUARTER_IN_DEGREE / half,
                           nullptr};
    ArcInfo arcInfoMid = {mid,
                          {0, 0},
                          static_cast<uint16_t>(rectLineWidth),
                          SEMICIRCLE_IN_DEGREE - QUARTER_IN_DEGREE / half,
                          SEMICIRCLE_IN_DEGREE + QUARTER_IN_DEGREE / half,
                          nullptr};
    ArcInfo arcInfoRight = {end,
                            {0, 0},
                            static_cast<uint16_t>(rectLineWidth),
                            CIRCLE_IN_DEGREE - QUARTER_IN_DEGREE / half,
                            SEMICIRCLE_IN_DEGREE - QUARTER_IN_DEGREE / half,
                            nullptr};
    styleSelect.lineColor_ = Color::White();
    styleSelect.lineOpa_ = backgroundOpacity_;
    uint8_t opa = DrawUtils::GetMixOpacity(opaScale_, backgroundOpacity_);
    baseGfxEngine->DrawArc(gfxDstBuffer, arcInfoLeft, trunc, styleSelect, opaScale_, CapType::CAP_NONE);
    // 2 : double
    baseGfxEngine->DrawLine(gfxDstBuffer, start, mid, trunc, rectLineWidth * 2, Color::White(), opa);
    baseGfxEngine->DrawArc(gfxDstBuffer, arcInfoMid, trunc, styleSelect, opaScale_, CapType::CAP_NONE);
    // 2 : double
    baseGfxEngine->DrawLine(gfxDstBuffer, mid, end, trunc, rectLineWidth * 2, Color::White(), opa);
    baseGfxEngine->DrawArc(gfxDstBuffer, arcInfoRight, trunc, styleSelect, opaScale_, CapType::CAP_NONE);
#endif
}

void UICheckBox::UnSelectedStateSoftwareDrawing(BufferInfo& gfxDstBuffer,
                                                Rect rect,
                                                Rect trunc,
                                                int16_t borderRadius,
                                                int16_t rectLineWidth)
{
    Style styleUnSelect = StyleDefault::GetBackgroundTransparentStyle();
    styleUnSelect.borderWidth_ = rectLineWidth;
    styleUnSelect.borderRadius_ = borderRadius;
    styleUnSelect.borderColor_ = Color::White();
    styleUnSelect.borderOpa_ = DEFAULT_UNSELECT_BG_OPA;
#if ENABLE_VGU_ENGINE
    LiteMGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, rect, trunc, styleUnSelect, opaScale_, transMap_);
#else
    BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, rect, trunc, styleUnSelect, opaScale_);
#endif
}

#if DEFAULT_ANIMATION
void UICheckBox::ResetCallback()
{
    if ((runTime_ != 0) && (checkBoxAnimator_.GetTime() != runTime_)) {
        checkBoxAnimator_.SetRunTime(checkBoxAnimator_.GetTime() - runTime_);
    }
}

void UICheckBox::Callback(UIView* view)
{
    runTime_ = checkBoxAnimator_.GetRunTime();
    float x = static_cast<float>(runTime_) / checkBoxAnimator_.GetTime();
    float coefficient = Interpolation::GetBezierY(x, BEZIER_CONTROL_POINT_X_1, 0, BEZIER_CONTROL_POINT_X_2, 1);
    backgroundOpacity_ = (state_ == SELECTED) ? (static_cast<uint8_t>(coefficient * OPA_OPAQUE)) :
                                                (static_cast<uint8_t>((1 - coefficient) * OPA_OPAQUE));
    Invalidate();
}

void UICheckBox::OnStop(UIView& view)
{
    backgroundOpacity_ = (state_ == SELECTED) ? OPA_OPAQUE : 0;
    Invalidate();
}
#endif

void UICheckBox::OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea)
{
    Rect trunc = invalidatedArea;
#if ENABLE_VGU_ENGINE
    Rect coords = GetOrigRect();
    LiteMGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, coords, invalidatedArea, *style_, opaScale_, transMap_);
#else
    Rect coords = GetContentRect();
    BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, GetRect(), invalidatedArea, *style_, opaScale_);
#endif
    if (!trunc.Intersect(trunc, coords)) {
        return;
    }

    if ((image_[SELECTED].GetSrcType() != IMG_SRC_UNKNOWN) && (image_[UNSELECTED].GetSrcType() != IMG_SRC_UNKNOWN)) {
        ImageHeader header = {0};
        image_[state_].GetHeader(header);
        int16_t imgWidth = header.width;
        int16_t imgHeight = header.height;
        coords.SetWidth(imgWidth);
        coords.SetHeight(imgHeight);
        int16_t offsetLeft = (GetWidth() - imgWidth) / 2;  // 2 : half
        int16_t offsetTop = (GetHeight() - imgHeight) / 2; // 2 : half
        coords.SetX(coords.GetX() + offsetLeft);
        coords.SetY(coords.GetY() + offsetTop);
        image_[state_].DrawImage(gfxDstBuffer, coords, trunc, *style_, opaScale_);
    } else {
        CalculateSize();
        int16_t rectLineWidth = borderWidth_ / DEFAULT_BORDER_WIDTH;
        rectLineWidth = (rectLineWidth == 0) ? 1: rectLineWidth;
        int16_t borderRadius = rectLineWidth * DEFAULT_RATIO_BORDER_RADIUS_LINE_WIDTH;
        int16_t x = coords.GetX() + (width_ - borderWidth_) / 2;  // 2: half
        int16_t y = coords.GetY() + (height_ - borderWidth_) / 2; // 2: half
        Rect rect(x, y, x + borderWidth_, y + borderWidth_);
#if DEFAULT_ANIMATION
        UnSelectedStateSoftwareDrawing(gfxDstBuffer, rect, trunc, borderRadius, rectLineWidth);
        SelectedStateSoftwareDrawing(gfxDstBuffer, rect, trunc, borderRadius, rectLineWidth);
#else
        if (state_ == SELECTED) {
            SelectedStateSoftwareDrawing(gfxDstBuffer, rect, trunc, borderRadius, rectLineWidth);
        } else {
            UnSelectedStateSoftwareDrawing(gfxDstBuffer, rect, trunc, borderRadius, rectLineWidth);
        }
#endif
    }
}

void UICheckBox::SetSelectedStateColor(ColorType color)
{
    selectedStateColor_ = color;
}

ColorType UICheckBox::GetSelectedStateColor() const
{
    return selectedStateColor_;
}
} // namespace OHOS
