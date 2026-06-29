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


#include "ui_test_arc_label.h"
#include "common/screen.h"
#include "font/ui_font.h"

namespace OHOS {
namespace {
const int16_t GAP = 20;
const uint16_t CENTER_X = 150;
const uint16_t CENTER_Y = 150;
const uint16_t RADIUS = 100;
constexpr int16_t DEG_0 = 0;
constexpr int16_t DEG_270 = 270;
constexpr int16_t DEG_90 = 90;
constexpr int16_t DEG_NEG_180 = -180;
} // namespace

void UITestArcLabel::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->SetThrowDrag(true);
        container_->SetHorizontalScrollState(false);
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
        x_ = 100; // 100: x
        y_ = 0;
    }
}

void UITestArcLabel::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
}

const UIView* UITestArcLabel::GetTestView()
{
    TestDegree();
    TestOrientation();
    TestAlignment();
    return container_;
}

void UITestArcLabel::TestDegree()
{
    const char* title = "内部角度从0到270的左对齐弧形文本";
    TestArcLabelDisplay(title, DEG_0, DEG_270, TextOrientation::INSIDE, TEXT_ALIGNMENT_LEFT);

    const char* title1 = "内部角度从90到-180的左对齐弧形文本";
    TestArcLabelDisplay(title1, DEG_90, DEG_NEG_180, TextOrientation::INSIDE, TEXT_ALIGNMENT_LEFT);
}

void UITestArcLabel::TestOrientation()
{
    const char* title = "外部角度从0到270的左对齐弧形文本";
    TestArcLabelDisplay(title, DEG_0, DEG_270, TextOrientation::OUTSIDE, TEXT_ALIGNMENT_LEFT);

    const char* title1 = "外部角度从90到-180的左对齐弧形文本";
    TestArcLabelDisplay(title1, DEG_90, DEG_NEG_180, TextOrientation::OUTSIDE, TEXT_ALIGNMENT_LEFT);
}

void UITestArcLabel::TestAlignment()
{
    const char* title = "0到90左对齐";
    TestArcLabelDisplay(title, DEG_0, DEG_90, TextOrientation::INSIDE, TEXT_ALIGNMENT_LEFT);

    const char* title1 = "0到90居中对齐";
    TestArcLabelDisplay(title, DEG_0, DEG_90, TextOrientation::INSIDE, TEXT_ALIGNMENT_CENTER);

    const char* title2 = "0到90右对齐";
    TestArcLabelDisplay(title, DEG_0, DEG_90, TextOrientation::INSIDE, TEXT_ALIGNMENT_RIGHT);
}

void UITestArcLabel::TestArcLabelDisplay(const char* title, const int16_t startAngle, const int16_t endAngle,
    const TextOrientation orientation, UITextLanguageAlignment alignment)
{
    if (container_ != nullptr) {
        UIArcLabel* label = new UIArcLabel();
        label->SetPosition(x_, y_);
        label->SetArcTextCenter(CENTER_X, CENTER_Y + y_ + GAP);
        label->SetArcTextRadius(RADIUS);
        label->SetArcTextAngle(startAngle, endAngle);
        label->SetArcTextOrientation(orientation);
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 30); // 30: font size
        label->SetText(title);
        label->SetStyle(STYLE_LETTER_SPACE, 10); // 10: space
        label->SetStyle(STYLE_TEXT_COLOR, Color::Red().full);
        label->SetAlign(alignment);
        container_->Add(label);
        y_ += (RADIUS * 2) + GAP; // 2: diameter
    }
}
}
