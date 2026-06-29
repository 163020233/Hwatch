/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef OHOS_ACELITE_ARCLIST_COMPONENT_H
#define OHOS_ACELITE_ARCLIST_COMPONENT_H

#include "component.h"
#include "list_adapter.h"
#include "ui_transform_list.h"

namespace OHOS {
namespace ACELite {
class ArcListComponent final : public Component {
public:
    ACE_DISALLOW_COPY_AND_MOVE(ArcListComponent);
    ArcListComponent() = delete;
    ArcListComponent(JSValue options, JSValue children, AppStyleManager *styleManager);
    ~ArcListComponent() override {}

protected:
    bool CreateNativeViews() override;
    void ReleaseNativeViews() override;
    UIView *GetComponentRootView() const override;
    bool ApplyPrivateStyle(const AppStyleItem* style) override;
    bool RegisterPrivateEventListener(uint16_t eventTypeId, JSValue funcValue, bool isStopPropagation) override;
    // ohos begin
    // ace, add progress bar
#ifdef LITEWEARABLE_SUPPORT_ENABLED
    bool SetPrivateAttribute(uint16_t attrKeyId, jerry_value_t attrValue) override;
#endif
    // ohos end
    bool ProcessChildren() override;
    void OnViewAttached() override;
    bool UpdateForView() override;

private:
    UITransformList list_;
    ListAdapter adapter_;
    ListEventListener listEventListener_;
    bool hasSetAdaptor_ = false;
    static const char * const FUNC_SCROLLTO;
    static const char * const INDEX_SCROLLTO;
    // ohos begin
    // ace, add scale list
    int16_t scaleFactor_;
    UIView::ViewExtraMsg scaleMsg_;
    static bool ScaleListItem(UITransformList* list, UIView* item);
    // ace, add progress bar
#ifdef LITEWEARABLE_SUPPORT_ENABLED
    static const char * const FUNC_SCROLLTO_END;
#endif
    // ohos end
    static JSValue ListScrollTo(const JSValue func, const JSValue dom, const JSValue args[], const jerry_length_t size);
    // ohos begin
    // ace, add progress bar
#ifdef LITEWEARABLE_SUPPORT_ENABLED
    static JSValue ListScrollToEnd(
        const JSValue func, const JSValue dom, const JSValue args[], const jerry_length_t size);
    static JSValue ListScroll(const JSValue func, const JSValue dom, const JSValue args[], const jerry_length_t size);
    void ComputeListLength();
#endif
    // ohos end
};
} // namespace ACELite
} // namespace OHOS

#endif // OHOS_ACELITE_ARCLIST_COMPONENT_H
