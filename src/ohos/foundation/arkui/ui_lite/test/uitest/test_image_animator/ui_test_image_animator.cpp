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

#include "ui_test_image_animator.h"
#include "common/screen.h"

namespace OHOS {
namespace {
static ImageAnimatorInfo g_imageAnimatorInfo[4] = {
    {RES_PATH"image_animator_1.bin", {84, 108}, 116, 116, IMG_SRC_FILE_PATH},
    {RES_PATH"image_animator_2.bin", {84, 108}, 116, 116, IMG_SRC_FILE_PATH},
    {RES_PATH"image_animator_3.bin", {84, 108}, 116, 116, IMG_SRC_FILE_PATH},
    {RES_PATH"image_animator_4.bin", {84, 108}, 116, 116, IMG_SRC_FILE_PATH},
};

static ImageAnimatorInfo g_imageAnimatorInfo2[4] = {
    {RES_PATH"blue.png", {84, 108}, 94, 94, IMG_SRC_FILE_PATH},
    {RES_PATH"red.png", {84, 108}, 94, 94, IMG_SRC_FILE_PATH},
    {RES_PATH"green.png", {84, 108}, 94, 94, IMG_SRC_FILE_PATH},
    {RES_PATH"yellow.png", {84, 108}, 94, 94, IMG_SRC_FILE_PATH},
};
} // namespace

void UITestImageAnimator::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);

        UIViewGroup* uiViewGroup = new UIViewGroup();
        // 270: width; 360: height
        uiViewGroup->SetPosition(0, 0, 270, 360);
        container_->Add(uiViewGroup);
        UILabel* label = new UILabel();
        uiViewGroup->Add(label);
        // 260: width; 48: height
        label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, TEXT_DISTANCE_TO_TOP_SIDE, 260, 48);
        label->SetText("   UIImageAnimator效果");
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        UIViewGroup* uiViewGroupFrame_ = new UIViewGroup();
        uiViewGroup->Add(uiViewGroupFrame_);
        // 250: width; 306: height
        uiViewGroupFrame_->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE2, VIEW_DISTANCE_TO_TOP_SIDE, 250, 306);
        uiViewGroupFrame_->SetStyle(STYLE_BORDER_COLOR, Color::White().full);
        uiViewGroupFrame_->SetStyle(STYLE_BORDER_OPA, HALF_OPA_OPAQUE);
        uiViewGroupFrame_->SetStyle(STYLE_BORDER_WIDTH, VIEW_STYLE_BORDER_WIDTH);
        uiViewGroupFrame_->SetStyle(STYLE_BORDER_RADIUS, VIEW_STYLE_BORDER_RADIUS);
        uiViewGroupFrame_->SetStyle(STYLE_BACKGROUND_OPA, 0);

        imageAnimator_ = new UIImageAnimatorView();
        imageAnimator_->SetPosition(25, 50, 200, 200); // 25 : offset 50 : offset ;200 : offset 200: offset
        imageAnimator_->SetImageAnimatorSrc(g_imageAnimatorInfo, 4, 100); // 4: the number of images, 100: updating time
        imageAnimator_->Start();
        listener_ = new TestAnimatorStopListener(container_);
        listener_->Init();
        uiViewGroupFrame_->Add(imageAnimator_);
        imageAnimator_->LayoutCenterOfParent();
    }

    if (layout_ == nullptr) {
        layout_ = new GridLayout();
        uint16_t height = Screen::GetInstance().GetHeight();
        layout_->SetPosition(270, 0, 250, height); // 270: x 0 : y, 250: width,
        container_->Add(layout_);
        layout_->LayoutTopOfParent(60); // 60: offset
        layout_->SetLayoutDirection(LAYOUT_VER);
        layout_->SetRows(10); // 10 : rows
        layout_->SetCols(2); // 2 : columns
    }
}


void UITestImageAnimator::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
    imageAnimator_ = nullptr;
    layout_ = nullptr;
    delete listener_;
    listener_ = nullptr;
}

const UIView* UITestImageAnimator::GetTestView()
{
    UIKitImageAnimatorTestStart001();
    UIKitImageAnimatorTestStop002();
    UIKitImageAnimatorTestPause003();
    UIKitImageAnimatorTestResume004();
    UIKitImageAnimatorTestSetImageAnimatorSrc005();
    UIKitImageAnimatorTestSetTickOfUpdate006();
    UIKitImageAnimatorTestSetSizeFixed007();
    UIKitImageAnimatorTestSetRepeat008();
    UIKitImageAnimatorTestSetReverse009();
    UIKitImageAnimatorTestSetAnimatorStopListener010();
    UIKitImageAnimatorTestSetRepeatTimes011();
    UIKitImageAnimatorTestSetFillModeTrueForward012();
    UIKitImageAnimatorTestSetFillModeTrueBackward013();
    UIKitImageAnimatorTestSetFillModeFalseForward014();
    UIKitImageAnimatorTestSetFillModeFalseBackward015();

    layout_->LayoutChildren();
    return container_;
}

void UITestImageAnimator::SetUpButton(UILabelButton* btn, const char* title)
{
    if (btn == nullptr) {
        return;
    }
    layout_->Add(btn);
    btn->Resize(BUTTON_WIDHT2, BUTTON_HEIGHT2);
    btn->SetText(title);
    btn->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BUTTON_LABEL_SIZE);
    btn->SetOnClickListener(this);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::RELEASED);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::PRESSED);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::INACTIVE);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::RELEASED);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::PRESSED);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::INACTIVE);
}

void UITestImageAnimator::UIKitImageAnimatorTestStart001()
{
    startBtn_ = new UILabelButton();
    SetUpButton(startBtn_, "Start");
}

void UITestImageAnimator::UIKitImageAnimatorTestStop002()
{
    stopBtn_ = new UILabelButton();
    SetUpButton(stopBtn_, "Stop");
}

void UITestImageAnimator::UIKitImageAnimatorTestPause003()
{
    pauseBtn_ = new UILabelButton();
    SetUpButton(pauseBtn_, "Pause");
}

void UITestImageAnimator::UIKitImageAnimatorTestResume004()
{
    resumeBtn_ = new UILabelButton();
    SetUpButton(resumeBtn_, "Resume");
}

void UITestImageAnimator::UIKitImageAnimatorTestSetImageAnimatorSrc005()
{
    setImageBtn_ = new UILabelButton();
    SetUpButton(setImageBtn_, "设置图片");
}

void UITestImageAnimator::UIKitImageAnimatorTestSetTickOfUpdate006()
{
    setSpeedBtn_ = new UILabelButton();
    SetUpButton(setSpeedBtn_, "设置速度");
}

void UITestImageAnimator::UIKitImageAnimatorTestSetSizeFixed007()
{
    fixedBtn_ = new UILabelButton();
    SetUpButton(fixedBtn_, "图片大小固定");
}

void UITestImageAnimator::UIKitImageAnimatorTestSetRepeat008()
{
    repeatBtn_ = new UILabelButton();
    noRepeatBtn_ = new UILabelButton();
    SetUpButton(repeatBtn_, "循环播放");
    SetUpButton(noRepeatBtn_, "播放一次 ");
}

void UITestImageAnimator::UIKitImageAnimatorTestSetReverse009()
{
    reverseOrderBtn_ = new UILabelButton();
    positiveOrderBtn_ = new UILabelButton();
    SetUpButton(reverseOrderBtn_, "逆序播放");
    SetUpButton(positiveOrderBtn_, "正序播放");
}

void UITestImageAnimator::UIKitImageAnimatorTestSetAnimatorStopListener010()
{
    listenerBtn_ = new UILabelButton();
    SetUpButton(listenerBtn_, "播放结束监听");
}

void UITestImageAnimator::UIKitImageAnimatorTestSetRepeatTimes011()
{
    repeatTimesBtn_ = new UILabelButton();
    SetUpButton(repeatTimesBtn_, "播放次数+");
}

void UITestImageAnimator::UIKitImageAnimatorTestSetFillModeTrueForward012()
{
    fillModeTrueForwardBtn_ = new UILabelButton();
    SetUpButton(fillModeTrueForwardBtn_, "fillMode true 正播");
}

void UITestImageAnimator::UIKitImageAnimatorTestSetFillModeTrueBackward013()
{
    fillModeTrueBackwardBtn_ = new UILabelButton();
    SetUpButton(fillModeTrueBackwardBtn_, "fillMode true 逆播");
}

void UITestImageAnimator::UIKitImageAnimatorTestSetFillModeFalseForward014()
{
    fillModeFalseForwardBtn_ = new UILabelButton();
    SetUpButton(fillModeFalseForwardBtn_, "fillMode false 正播");
}

void UITestImageAnimator::UIKitImageAnimatorTestSetFillModeFalseBackward015()
{
    fillModeFalseBackwardBtn_ = new UILabelButton();
    SetUpButton(fillModeFalseBackwardBtn_, "fillMode false 逆播");
}

void UITestImageAnimator::SubEvent(const UIView& view)
{
    if (&view == startBtn_) {
        imageAnimator_->Start();
    } else if (&view == stopBtn_) {
        imageAnimator_->Stop();
    } else if (&view == pauseBtn_) {
        imageAnimator_->Pause();
    } else if (&view == resumeBtn_) {
        imageAnimator_->Resume();
    } else if (&view == setImageBtn_) {
        imageAnimator_->Stop();
        imageAnimator_->SetImageAnimatorSrc(g_imageAnimatorInfo2, 4); // 4: the number of images
        imageAnimator_->Start();
    } else if (&view == setSpeedBtn_) {
        imageAnimator_->Stop();
        imageAnimator_->SetTimeOfUpdate(10); // 10: set time of update
        imageAnimator_->Start();
    } else if (&view == fixedBtn_) {
        imageAnimator_->SetSizeFixed(true);
        imageAnimator_->SetPosition(50, 50, 200, 200); // 50 : offset 50 : offset 200 : offset 200 ：offset
    } else if (&view == repeatBtn_) {
        imageAnimator_->Stop();
        imageAnimator_->SetRepeat(true);
        imageAnimator_->Start();
    } else if (&view == noRepeatBtn_) {
        imageAnimator_->Stop();
        imageAnimator_->SetRepeat(false);
        imageAnimator_->Start();
    } else if (&view == reverseOrderBtn_) {
        imageAnimator_->Stop();
        imageAnimator_->SetReverse(true);
        imageAnimator_->Start();
    } else if (&view == positiveOrderBtn_) {
        imageAnimator_->Stop();
        imageAnimator_->SetReverse(false);
        imageAnimator_->Start();
    } else if (&view == listenerBtn_) {
        imageAnimator_->SetAnimatorStopListener(listener_);
    } else if (&view == repeatTimesBtn_) {
        imageAnimator_->Stop();
        imageAnimator_->SetRepeatTimes(imageAnimator_->GetRepeatTimes() + 1);
        imageAnimator_->Start();
    }
}

bool UITestImageAnimator::OnClick(UIView& view, const ClickEvent& event)
{
    SubEvent(view);
    if (&view == fillModeTrueForwardBtn_) {
        imageAnimator_->Stop();
        imageAnimator_->SetFillMode(true);
        imageAnimator_->SetRepeat(false);
        imageAnimator_->SetReverse(false);
        imageAnimator_->Start();
    } else if (&view == fillModeTrueBackwardBtn_) {
        imageAnimator_->Stop();
        imageAnimator_->SetFillMode(true);
        imageAnimator_->SetRepeat(false);
        imageAnimator_->SetReverse(true);
        imageAnimator_->Start();
    } else if (&view == fillModeFalseForwardBtn_) {
        imageAnimator_->Stop();
        imageAnimator_->SetFillMode(false);
        imageAnimator_->SetRepeat(false);
        imageAnimator_->SetReverse(false);
        imageAnimator_->Start();
    } else if (&view == fillModeFalseBackwardBtn_) {
        imageAnimator_->Stop();
        imageAnimator_->SetFillMode(false);
        imageAnimator_->SetRepeat(false);
        imageAnimator_->SetReverse(true);
        imageAnimator_->Start();
    }
    return true;
}
} // namespace OHOS
