/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: WaterfallDesktopFragment.h
 * Author:
 * Create: 2025-07-04
 */

#ifndef WATERFALL_DESKTOP_FRAGMENT_H
#define WATERFALL_DESKTOP_FRAGMENT_H

#include "UIDesktopFragment.h"
#include "components/ui_waterfall_list.h"
#include "components/ui_transform_group.h"

namespace OHOS {
class WaterfallItemView;
class WaterfallDesktopFragment : public UIDesktopFragment, public UIWaterfallList::OnModeChangeListener {
public:
    bool OnClick(UIView &view, const ClickEvent &event) override;
    void AddAppItemToList(const AppItem &item) override;
    void ClearAppItemToList() override;
    void RefreshAppList() override;
    void OnModeChange(UIWaterfallList* view, UIWaterfallList::WaterfallListMode targetMode) override;

protected:
    void OnCreateView(void* data) override;
    void OnDestroyView() override;
    void OnResumeView() override;
    void OnPauseView() override;

private:
    UIWaterfallList* waterfallList_{nullptr};
    List<WaterfallItemView*> waterfallItemList_;
};

class WaterfallItemView : public UITransformGroup {
public:
    explicit WaterfallItemView() {}
    ~WaterfallItemView();
    bool SetItemInfo(const AppItem &itemInfo);
    const AppItem& GetItemInfo() const;
    AppViewId GetViewId();
    const char *GetAppUid();
    void SetDeleteIconVisible(bool visible);

private:
    WaterfallItemView(const WaterfallItemView &);
    WaterfallItemView &operator = (const WaterfallItemView &);
    AppItem itemInfo_;
    UIImageView itemImg_;
    UIImageView deleteImg_;
};
}

#endif // WATERFALL_DESKTOP_FRAGMENT_H
