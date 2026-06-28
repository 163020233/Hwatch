/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: RecentAppView.cpp
 * Create: 2025-08-12
 */

#include "recent_app/RecentAppView.h"
#include "recent_app/RollerRecentAppFragment.h"
#include "wearable_log.h"
#include "common/image_cache_manager.h"
#include "common/screen.h"
#include "Slice.h"
#include "RecentManager.h"

namespace OHOS {

RecentAppView::~RecentAppView()
{
    if (loadingFragment_ != nullptr) {
        loadingFragment_->DestroyView();
        delete loadingFragment_;
        loadingFragment_ = nullptr;
    }
    if (recentAppFragment_ != nullptr) {
        recentAppFragment_->DestroyView();
        delete recentAppFragment_;
        recentAppFragment_ = nullptr;
    }
    if (loadingAnim_ != nullptr) {
        if (loadingAnim_->GetState() != Animator::STOP) {
            loadingAnim_->Stop();
        }
        delete loadingAnim_;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "RecentAppView::~RecentAppView");
}

void RecentAppView::OnStart(void)
{
    loadingAnimCb_.SetRecentAppView(this);

    if (loadingFragment_ == nullptr) {
        loadingFragment_ = new LoadingFragment();
        if (loadingFragment_ == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "failed to new LoadingFragment!");
            return;
        }
    }
    loadingFragment_->GetFragmentView()->SetPosition(0, 0, GetRootContainer()->GetWidth(),
                                                     GetRootContainer()->GetHeight());
    loadingFragment_->CreateView();
    AddViewToRootContainer(loadingFragment_->GetFragmentView());
}

void RecentAppView::OnResume()
{
    if (viewiInitStatus_) {
        return;
    }

    if (!RecentManager::GetInstance()->IsUpdateCompleted()) {
        loadingAnim_ = new Animator(&loadingAnimCb_, nullptr, 0, true);
        if (loadingAnim_ == nullptr) {
            return;
        }
        loadingAnim_->Start();
    } else {
        LoadContent();
    }
    viewiInitStatus_ = true;
}

void RecentAppView::LoadContent()
{
    if (loadingAnim_ != nullptr) {
        loadingAnim_->Stop();
    }

    if (loadingFragment_ != nullptr) {
        loadingFragment_->GetFragmentView()->SetVisible(false);
    }
    if (recentAppFragment_ == nullptr) {
        recentAppFragment_ = new RollerRecentAppFragment();
    }
    if (recentAppFragment_ != nullptr) {
        recentAppFragment_->GetFragmentView()->SetPosition(0, 0, GetRootContainer()->GetWidth(),
                                                            GetRootContainer()->GetHeight());
        recentAppFragment_->CreateView();
        AddViewToRootContainer(recentAppFragment_->GetFragmentView());
    } else {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new RollerRecentAppFragment failed!");
    }
}

void RecentAppView::LoadingAnimCb::Callback(UIView* view)
{
    if (RecentManager::GetInstance()->IsUpdateCompleted()) {
        if (recentAppView_ != nullptr) {
            recentAppView_->LoadContent();
        } else {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "recentAppView_ is null!");
        }
    }
}
}