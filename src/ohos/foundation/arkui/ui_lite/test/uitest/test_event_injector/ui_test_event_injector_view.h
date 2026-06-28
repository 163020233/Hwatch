/*
 * Copyright (c) 2020-2021 HiSilicon (Shanghai) Technologies Co., Ltd.
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

#ifndef UI_TEST_EVENT_INJECTOR_VIEW_H
#define UI_TEST_EVENT_INJECTOR_VIEW_H

#include "graphic_config.h"
#include "components/root_view.h"

namespace OHOS {

class UITestEventInjectorView : public UIView, public RootView::OnKeyActListener {
public:
    UITestEventInjectorView() : label_(nullptr) {}
    virtual ~UITestEventInjectorView() {RootView::GetInstance()->ClearOnKeyActListener();}
    void InitListener() { RootView::GetInstance()->SetOnKeyActListener(this);}
    bool OnLongPressEvent(const LongPressEvent& event) override;
    bool OnDragEvent(const DragEvent& event) override;
    void SetSentence(std::string sentence) {sentence_ = sentence;}
    void SetLabel(UILabel* label) {label_ = label;}
    bool OnClickEvent(const ClickEvent& event) override;
    bool OnPressEvent(const PressEvent& event) override;
    bool OnReleaseEvent(const ReleaseEvent& event) override;
    bool OnCancelEvent(const CancelEvent& event) override;
    bool OnKeyAct(UIView& view, const KeyEvent& event) override;

private:
    UILabel* label_;
    std::string sentence_ = "click";
};
}

#endif // UI_TEST_EVENT_INJECTOR_VIEW_H