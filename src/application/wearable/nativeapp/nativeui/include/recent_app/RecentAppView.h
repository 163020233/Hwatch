/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: RecentAppView.h
 * Create: 2025-08-12
 */

#ifndef RECENT_APP_VIEW_H
#define RECENT_APP_VIEW_H

#include "View.h"
#include "RecentAppPresenter.h"
#include "LoadingFragment.h"
#include "animator/animator.h"

namespace OHOS {
class RecentAppView : public View<RecentAppPresenter> {
public:
    RecentAppView() {}
    ~RecentAppView() override;
    RecentAppView(const RecentAppView &) = delete;
    RecentAppView &operator=(const RecentAppView &) = delete;
    void OnStart() override;
    void OnResume();

private:
    class LoadingAnimCb : public AnimatorCallback {
    public:
        LoadingAnimCb() {}
        ~LoadingAnimCb() override {}
        void Callback(UIView* view) override;
        void SetRecentAppView(RecentAppView* recentAppView)
        {
            recentAppView_ = recentAppView;
        }
    private:
        RecentAppView* recentAppView_;
    };

    void LoadContent();
    bool viewiInitStatus_ = false;
    LoadingAnimCb loadingAnimCb_;
    Animator* loadingAnim_ = nullptr;
    LoadingFragment *loadingFragment_ = nullptr;
    UIFragment *recentAppFragment_ = nullptr;
};
} // namespace OHOS
#endif // RECENT_APP_VIEW_H