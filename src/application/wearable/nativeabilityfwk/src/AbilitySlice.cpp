/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: AbilitySlice
 * Author: Hisi Graphic Team
 * Create: 2025-4
 */

#include "AbilitySlice.h"
#include "components/root_view.h"
#include "dock/focus_manager.h"
#include "wearable_log.h"
#include "PageTransitionMgr.h"
#include "NativeAbility.h"
#include "View.h"
#include "Presenter.h"
#include "key/KeyInputListener.h"
#include "gesture/GestureInputListener.h"
#include "common/image_cache_manager.h"

namespace OHOS {
const uint16_t SLICE_MASK = 0xFFFF;
const uint32_t PAGE_OFFSET = 16;

void AbilitySlice::OnStart(void* data)
{
    if ((presenter_ == nullptr) || (view_ == nullptr)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "Presenter or View  was nullptr for slice %s", sliceId_);
        return;
    }

    // 跳转到新界面更新下屏幕内容方向，防止重复布局
    RotationManager::GetInstance().UpdateDeviceRotation(sliceId_, slicePageMgr_.targetPageId_);
    // if back to current slice, delete slide back ImageView of current slice firstly
    if (PageTransitionMgr::GetInstance().IsBackTransitionRunning()) {
        PageTransitionMgr::GetInstance().ClearTopSlideBackImage();
    }
    presenter_->Attach(view_);
    view_->Attach(presenter_);
    slicePageMgr_.AttachVP(presenter_, view_);

    view_->SetupView();
    view_->OnStart();
    view_->OnStart(data);
    slicePageMgr_.StartSlicePage(data);
    presenter_->OnStart();
    presenter_->OnStart(data);
    state_ = SliceState::START;
}

void AbilitySlice::OnResume()
{
    if ((state_ != SliceState::START) && (state_ != SliceState::PAUSE)) { // avoid repeated resume
        return;
    }
    if ((presenter_ == nullptr) || (view_ == nullptr)) {
        return;
    }

    if (view_->GetRootContainer()->GetParent() == nullptr) {
        RootView::GetInstance()->Add(view_->GetRootContainer());
    }
    // Reset the default key listener
    if (!ImageCacheManager::GetInstance().IsInAod()) {
        RootView::GetInstance()->SetOnKeyActListener(KeyInputListener::GetInstance());
    }
    NativeAbility::GetInstance().SetCurSlicePriority(gSliceDefaultPriority);

    UIView* backView = PageTransitionMgr::GetInstance().GetTopSlideBackImage();
    BackDragListener* backDragListener = view_->GetBackDragListener();
    if ((backView != nullptr) && ((backDragListener != nullptr))) {
        backDragListener->SetCurrentAndTargetView(view_->GetRootContainer(), backView);
        backDragListener->SetParentView(RootView::GetInstance());
        backDragListener->SetDragToTargetAction([]() { NativeAbility::GetInstance().BackToPreSlice(); });
        view_->GetRootContainer()->SetTouchable(true);
        view_->GetRootContainer()->SetDraggable(true);
        view_->GetRootContainer()->SetOnDragListener(backDragListener);
    }

    slicePageMgr_.ResumeSlicePage();
    if (presenter_ != nullptr) {
        GestureInputListener *listner = dynamic_cast<GestureInputListener *>(presenter_);
        if (listner) {
            RootView::GetInstance()->SetGestureListener(listner);
        } else {
            RootView::GetInstance()->SetGestureListener(GestureInputListener::GetInstance());
        }
        presenter_->OnResume();
        view_->Draw();
        state_ = SliceState::RESUME;
    }

    // 延迟生效
    if (hasPendingRotateOperation_) {
        hasPendingRotateOperation_ = false;
        if (RotationManager::GetInstance().UpdateDeviceRotation(sliceId_, slicePageMgr_.targetPageId_)) {
            OnOrientationChanged(RotationManager::GetInstance().GetCurContentRotation());
        }
    }
    slicePageMgr_.SetCurRotation(RotationManager::GetInstance().GetCurContentRotation());
}

void AbilitySlice::OnPause(PauseReason reason)
{
    if (state_ != SliceState::RESUME) { // avoid repeated pause
        return;
    }
    if ((presenter_ == nullptr) || (view_ == nullptr)) {
        return;
    }

    if (slicePageMgr_.IsAnimatorRunning()) {
        slicePageMgr_.StopSwitchAnimator();
    }
    slicePageMgr_.PauseSlicePage(reason);
    RootView::GetInstance()->ClearOnKeyActListener();
    RootView::GetInstance()->ClearGestureListener();
    BackDragListener* backDragListener = view_->GetBackDragListener();
    if (backDragListener != nullptr) {
        backDragListener->ClearDragState();
    }
    view_->GetRootContainer()->SetOnDragListener(nullptr);

    FocusManager::GetInstance()->ClearFocus();
    presenter_->OnPause();
    state_ = SliceState::PAUSE;
}

void AbilitySlice::OnStop()
{
    if ((presenter_ == nullptr) || (view_ == nullptr)) {
        return;
    }

    slicePageMgr_.StopSlicePage();
    view_->TearDownView();
    view_->OnStop();
    delete view_;
    view_ = nullptr;

    presenter_->OnStop();
    delete presenter_;
    presenter_ = nullptr;

    state_ = SliceState::STOP;
}

UIViewGroup* AbilitySlice::GetSliceContainer()
{
    if (view_ != nullptr) {
        return view_->GetRootContainer();
    } else {
        return nullptr;
    }
}

bool AbilitySlice::ChangeSlicePage(uint16_t pageId, void* data, TransitionType type, bool canBack)
{
    return slicePageMgr_.ChangeSlicePage(pageId, data, type, canBack);
}

bool AbilitySlice::BackToPrevSlicePage(TransitionType type)
{
    return slicePageMgr_.BackToPrevSlicePage(type);
}

bool AbilitySlice::BackToCachedSlicePage(uint16_t pageId, TransitionType type)
{
    return slicePageMgr_.BackToCachedSlicePage(pageId, type);
}

bool AbilitySlice::IsCachedSlicePage(uint16_t pageId)
{
    return slicePageMgr_.IsCachedSlicePage(pageId);
}

void AbilitySlice::OnOrientationChanged(AppRotationType rot)
{
    if (view_ == nullptr) {
        GRAPHIC_LOGE("failet to get view when rotate screen");
        return;
    }

    UIViewGroup *group = view_->GetRootContainer();
    if (group == nullptr) {
        GRAPHIC_LOGE("failed to get view container when rotate screen");
        return;
    }
    UIViewGroup *parent = dynamic_cast<UIViewGroup*>(group->GetParent());
    if (parent == nullptr) {
        GRAPHIC_LOGE("root container's parent is null");
        return;
    }
    group->Resize(parent->GetWidth(), parent->GetHeight());
    view_->OnOrientationChanged(rot);
    slicePageMgr_.OnOrientationChanged(rot);
}

bool AbilitySlice::IsBackDragging() const
{
    if (view_ == nullptr) {
        return false;
    }

    BackDragListener *listener = view_->GetBackDragListener();
    if (listener == nullptr) {
        return false;
    }

    return listener->IsDragging();
}

void AbilitySlice::EnablePendingRotation()
{
    hasPendingRotateOperation_ = true;
    if (!IsBackDragging()) {
        return;
    }

    BackDragListener *listener = view_->GetBackDragListener();
    // 跟手期间设置延迟生效
    listener->SetDragToCurrentAction([this]() {
        if (!hasPendingRotateOperation_) {
            return;
        }
        hasPendingRotateOperation_ = false;
        if (RotationManager::GetInstance().UpdateDeviceRotation(sliceId_, slicePageMgr_.targetPageId_)) {
            OnOrientationChanged(RotationManager::GetInstance().GetCurContentRotation());
        }
    });
}

AppRotationType AbilitySlice::GetCurRotation() const
{
    return slicePageMgr_.GetCurRotation();
}

void AbilitySlice::StopSlicePageSwitchAnimator()
{
    if (slicePageMgr_.IsAnimatorRunning()) {
        slicePageMgr_.StopSwitchAnimator();
    }
}

}