/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
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

#include "Ability.h"
#include "AppViewIDs.h"
#include "graphic_service.h"
#include "abilityms_slite_client.h"
#include "common/input_device_manager.h"
#include "animator/animator.h"
#include "components/root_view.h"
#include "common/screen.h"
#include "common/graphic_utils.h"
#include "PageTransitionMgr.h"
#include "gfx_utils/list.h"
#include "AbilitySlice.h"
#include "settings/model/SettingDesktopModel.h"
#include "NativeAbility.h"
#include "notification_manager.h"
#include "power_display_service.h"
#include "notification_manager.h"

namespace OHOS {
void BaseAbility::AddSliceProxy(const uint16_t sliceId, SliceProxy* proxy)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "BaseAbility AddSliceProxy sliceId = %d", sliceId);
    if (sliceId <= gSliceInvalid || proxy == nullptr) {
        return;
    }

    if (Find(sliceId) != nullptr) {
        return;
    }

    BaseAbility::ProxyStruct proxyPair = {sliceId, proxy};
    allSlicesProxy_.PushBack(proxyPair);
}

void BaseAbility::StartSlice(uint32_t targetId, void* data)
{
    if (targetId <= gSliceInvalid) {
        return;
    }
    uint16_t sliceId = static_cast<uint16_t>(targetId & SLICE_MASK);
    BaseAbility::ProxyStruct* proxyStruct = Find(sliceId);
    if (proxyStruct == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "no slice %d was found", sliceId);
        return;
    }
    InputDeviceManager::GetInstance()->ClearInputDeviceState();

    Slice* newSlice = proxyStruct->proxy->CreateSlice(targetId);
    if (newSlice == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "create slice %u failed", sliceId);
        return;
    }

    /* 存储跳转前栈顶的ID */
    if (allSlices_.Size() > 0) {
        transitionPrevTargetId_ = allSlices_[allSlices_.Size() - 1];
    } else {
        transitionPrevTargetId_ = gSliceInvalid;
    }

    if (sliceId == defaultTargetId_) {
        allSlices_.Clear(); // clear slice history when return to main slice
        PageTransitionMgr::GetInstance().ClearSlideBackHistory();
        allSlices_.PushBack(defaultTargetId_);
    } else if (PageTransitionMgr::GetInstance().IsBackTransitionRunning()) {
        /* 返回类型的操作，出栈到目标 */
        int16_t popCnt = PopToTarget(targetId);
        PageTransitionMgr::GetInstance().SetSlicePopCnt(popCnt);
    } else {
        if (allSlices_.Size() > 31) { // 31 : max slice number
            allSlices_.Erase(0);
        }
        allSlices_.PushBack(targetId);
    }

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "BaseAbility StartSlice %d page %d size:%d", sliceId,
        targetId >> PAGE_OFFSET, allSlices_.Size());
    nextSlice_.sliceId = sliceId;
    nextSlice_.slice = newSlice;
    newSlice->OnStart(data);
}

BaseAbility::ProxyStruct* BaseAbility::Find(const uint16_t sliceId)
{
    if (sliceId <= gSliceInvalid) {
        return nullptr;
    }

    if (allSlicesProxy_.IsEmpty()) {
        return nullptr;
    }

    for (int i = 0; i < allSlicesProxy_.Size(); i++) {
        if (allSlicesProxy_[i].sliceId == sliceId) {
            return &allSlicesProxy_[i];
        }
    }
    return nullptr;
}

SlicePageManager* BaseAbility::GetCurSlicePageManager()
{
    if (PageTransitionMgr::GetInstance().IsTransitionRunning()) {
        WEARABLE_LOGW(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "No valid SlicePageManager during slice switching");
        return nullptr;
    }
    AbilitySlice* slice = dynamic_cast<AbilitySlice*>(curSlice_.slice);
    if (slice == nullptr) {
        WEARABLE_LOGW(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "No valid SlicePageManager when js is running");
        return nullptr;
    }
    return slice->GetSlicePageManger();
}

void BaseAbility::UpdateCurSlicePageId(uint16_t pageId)
{
    if (allSlices_.IsEmpty()) {
        return;
    }
    uint32_t sliceId = allSlices_[allSlices_.Size() - 1] & SLICE_MASK;
    allSlices_[allSlices_.Size() - 1] = sliceId | (static_cast<uint32_t>(pageId) << PAGE_OFFSET);
}

bool BaseAbility::SwitchPageInSlice(uint16_t pageId, TransitionType type, bool canBack, void* data, uint16_t dataLength)
{
    if (pageId == 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "page id should be valid but no zero");
        return false;
    }
    if (PageTransitionMgr::GetInstance().IsTransitionRunning()) {
        WEARABLE_LOGW(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "page can't be switched during transition");
        return false;
    }
    if (!GraphicService::IsNativeUITask()) {
        WEARABLE_LOGW(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "SwitchPageInSlice is called not in ui task");
        return false;
    }
    AbilitySlice* slice = dynamic_cast<AbilitySlice*>(curSlice_.slice);
    if (slice == nullptr) {
        WEARABLE_LOGW(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "page can't be switched because current slice is nullptr");
        return false;
    }
    void* sliceData = nullptr;
    if (data != nullptr) {
        sliceData = static_cast<void*>(UIMalloc(dataLength));
        memcpy_s(sliceData, dataLength, data, dataLength);
    }
    const power_display_svr_api_t *display_api = power_display_svr_get_api();
    if (display_api != nullptr) {
        display_api->turn_on_screen();
    }
    GraphicService::GetInstance()->PostGraphicEvent([slice, pageId, sliceData, type, canBack]() {
        slice->ChangeSlicePage(pageId, sliceData, type, canBack);
        if (sliceData != nullptr) {
            UIFree(sliceData);
        }
    });
    return true;
}

bool BaseAbility::IsCachedSlicePage(uint16_t pageId)
{
    if (PageTransitionMgr::GetInstance().IsTransitionRunning()) {
        WEARABLE_LOGW(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "Transition is running. Please check later.");
        return false;
    }

    AbilitySlice* slice = dynamic_cast<AbilitySlice*>(curSlice_.slice);
    if (slice == nullptr) {
        WEARABLE_LOGW(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "current slice is nullptr");
        return false;
    }

    return slice->IsCachedSlicePage(pageId);
}

void BaseAbility::OnRotationChanged()
{
    if (!GraphicService::GetInstance()->IsNativeRunning()) {
        GRAPHIC_LOGE("cannot rotate in js!");
        return;
    }

    AbilitySlice *slice = dynamic_cast<AbilitySlice*>(curSlice_.slice);
    if (slice == nullptr) {
        GRAPHIC_LOGE("slice is null when rotate!");
        return;
    }

    if (NotificationManager::GetInstance()->HasNotifyShowing()) {
        GRAPHIC_LOGE("has notify, pending rotation");
        slice->EnablePendingRotation();
        return;
    }

    if (PageTransitionMgr::GetInstance().IsTransitionRunning()) {
        if (PageTransitionMgr::GetInstance().IsTransAnimRunning()) {
            AbilitySlice *next = dynamic_cast<AbilitySlice*>(nextSlice_.slice);
            if (next != nullptr) {
                next->EnablePendingRotation();
            } else {
                GRAPHIC_LOGE("next slice is null when rotation!");
            }
        } else {
            GRAPHIC_LOGE("This logic should not be entered.");
        }
        return;
    }

    SlicePageManager *manager = GetCurSlicePageManager();
    if (manager == nullptr) {
        GRAPHIC_LOGE("cur manager is null when rotate!");
        return;
    }
    if (manager != nullptr && manager->IsPageBeingSwitched()) {
        if (manager->IsAnimatorRunning()) {
            manager->EnablePendingRotation();
        } else {
            GRAPHIC_LOGE("This logic should not be entered.");
        }
        return;
    }

    if (slice->IsBackDragging()) {
        slice->EnablePendingRotation();
        return;
    }

    if (manager->IsBackDragging()) {
        manager->EnablePendingRotation();
        return;
    }

    uint32_t targetId = GetCurTargetId();
    if (RotationManager::GetInstance().UpdateDeviceRotation(targetId & SLICE_MASK, targetId >> PAGE_OFFSET)) {
        slice->OnOrientationChanged(RotationManager::GetInstance().GetCurContentRotation());
    }
}

bool BaseAbility::IsAodSlice(uint32_t targetId) const
{
    return (targetId & SLICE_MASK) == VIEW_AOD;
}

bool BaseAbility::BackToPrevSlicePage(TransitionType type)
{
    if (PageTransitionMgr::GetInstance().IsTransitionRunning()) {
        WEARABLE_LOGW(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "page can't be switched during transition");
        return false;
    }
    if (!GraphicService::IsNativeUITask()) {
        WEARABLE_LOGW(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "BackToPrevSlicePage is not called  in ui task");
        return false;
    }
    AbilitySlice* slice = dynamic_cast<AbilitySlice*>(curSlice_.slice);
    if (slice == nullptr) {
        WEARABLE_LOGW(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "page can't be switched because current slice is nullptr");
        return false;
    }
    const power_display_svr_api_t *display_api = power_display_svr_get_api();
    if (display_api != nullptr) {
        display_api->turn_on_screen();
    }
    GraphicService::GetInstance()->PostGraphicEvent([slice, type]() {
        slice->BackToPrevSlicePage(type);
    });
    return true;
}

bool BaseAbility::BackToCachedSlicePage(uint16_t pageId, TransitionType type)
{
    if (PageTransitionMgr::GetInstance().IsTransitionRunning()) {
        WEARABLE_LOGW(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "page can't be switched during transition");
        return false;
    }
    if (!GraphicService::IsNativeUITask()) {
        WEARABLE_LOGW(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "BackToCachedSlicePage is not called in ui task");
        return false;
    }
    AbilitySlice* slice = dynamic_cast<AbilitySlice*>(curSlice_.slice);
    if (slice == nullptr) {
        WEARABLE_LOGW(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "page can't be switched because current slice is nullptr");
        return false;
    }
    const power_display_svr_api_t *display_api = power_display_svr_get_api();
    if (display_api != nullptr) {
        display_api->turn_on_screen();
    }
    GraphicService::GetInstance()->PostGraphicEvent([slice, pageId, type]() {
        slice->BackToCachedSlicePage(pageId, type);
    });
    return true;
}

void BaseAbility::SwitchSlice(uint16_t sliceId, uint16_t pageId, TransitionType type, bool enableSlideBack)
{
    ExtralConfig config = {pageId, type, enableSlideBack, false};
    SwitchSliceWithData(sliceId, nullptr, 0, config);
}

void BaseAbility::SwitchSliceWithCache(uint16_t sliceId, uint16_t pageId, TransitionType type, bool enableSlideBack)
{
    ExtralConfig config = {pageId, type, enableSlideBack, true};
    SwitchSliceWithData(sliceId, nullptr, 0, config);
}

void BaseAbility::SwitchSliceWithData(uint16_t sliceId, void* data, uint16_t dataLength, const ExtralConfig& config)
{
    if (sliceId == VIEW_AOD) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "cannot enter aod by switch slice!");
        return;
    }
    if (Find(sliceId) == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "slice %d is not registered", sliceId);
        return;
    }
    const power_display_svr_api_t *display_api = power_display_svr_get_api();
    if (display_api != nullptr) {
        display_api->turn_on_screen();
    }
#ifdef JS_ENABLE
    ElementName *elementName = OHOS::AbilitySlite::AbilityMsClient::GetInstance().GetTopAbility();
    if (elementName != nullptr && strcmp(elementName->bundleName, "com.ohos.launcher") != 0) {
        targetIdFromJS_ = sliceId | (static_cast<uint32_t>(config.pageId) << PAGE_OFFSET);
        OHOS::AbilitySlite::AbilityMsClient::GetInstance().ForceStop(elementName->bundleName, (uint8_t)config.type);
        FreeElement(elementName);
        return;
    }
    FreeElement(elementName);
#endif
    TransitionTarget target;
    target.sliceInfo.sliceAndPageId = sliceId | (static_cast<uint32_t>(config.pageId) << PAGE_OFFSET);
    void* sliceData = nullptr;
    if (dataLength != 0 && data != nullptr) {
        sliceData = static_cast<void*>(UIMalloc(dataLength));
        memcpy_s(sliceData, dataLength, data, dataLength);
    }
    target.sliceInfo.data = sliceData;
    target.sliceInfo.dataLength = dataLength;
    bool runImmed = PageTransitionMgr::GetInstance().BeginTransition(TransitionCaller::CHANGE_SLICE, target,
        config.type, config.enableSlideBack, config.enableCache);
    if (!runImmed) {
        printf("pending change slice id:%u type:%u enableSlideBack:%d\n",
            target.sliceInfo.sliceAndPageId, (uint32_t)config.type, config.enableSlideBack);
        return;
    }
    GraphicService::GetInstance()->PostGraphicEvent([this, target, config]() {
        AbilitySlice* slice = dynamic_cast<AbilitySlice*>(this->curSlice_.slice);
        if (slice != nullptr) {
            if (slice->IsPageBeingSwitched()) {
                slice->StopSlicePageSwitchAnimator();
            }
        }
        this->PostChangeSlice(target.sliceInfo.sliceAndPageId, config.type, target.sliceInfo.data, config.enableCache);
    });
}

void BaseAbility::ChangeSlice(uint16_t sliceId, TransitionType type,
    const uint16_t priority, bool enableSlideBack)
{
    (void)(priority);
    SwitchSlice(sliceId, gPageInvalid, type, enableSlideBack);
}

void BaseAbility::ChangeSliceToApplist()
{
    if (SettingDesktopModel::GetInstance().GetDesktopStyle() == HEXAGONS_STYLE) {
        NativeAbility::GetInstance().ChangeSlice(VIEW_APPLIST, TransitionType::TRANSITION_BACK_TO_HEXAGONS);
    } else if (SettingDesktopModel::GetInstance().GetDesktopStyle() == WATERFALL_STYLE) {
        NativeAbility::GetInstance().ChangeSlice(VIEW_APPLIST, TransitionType::TRANSITION_BACK_TO_WATERFALL);
    } else {
        NativeAbility::GetInstance().ChangeSlice(VIEW_APPLIST);
    }
}

void BaseAbility::EnterAod()
{
#ifdef JS_ENABLE
    ElementName *elementName = OHOS::AbilitySlite::AbilityMsClient::GetInstance().GetTopAbility();
    if (elementName != nullptr && strcmp(elementName->bundleName, "com.ohos.launcher") != 0) {
        targetIdFromJS_ = VIEW_AOD | (static_cast<uint32_t>(gPageInvalid) << PAGE_OFFSET);
        OHOS::AbilitySlite::AbilityMsClient::GetInstance().ForceStop(elementName->bundleName);
        FreeElement(elementName);
        return;
    }
    FreeElement(elementName);
#endif
    TransitionTarget target;
    target.sliceInfo.sliceAndPageId = VIEW_AOD | (static_cast<uint32_t>(gPageInvalid) << PAGE_OFFSET);
    target.sliceInfo.data = nullptr;
    target.sliceInfo.dataLength = 0;
    bool runImmed = PageTransitionMgr::GetInstance().BeginTransition(
        TransitionCaller::CHANGE_SLICE, target, TransitionType::TRANSITION_INVALID, false);
    if (!runImmed) {
        printf("pending change slice id:%u type:%d enableSlideBack:%d\n",
            target.sliceInfo.sliceAndPageId, static_cast<int>(TransitionType::TRANSITION_INVALID), false);
        return;
    }
    PauseSlice(PauseReason::ENTER_AOD);
    prevAodSlice_ = curSlice_;
    curSlice_.slice = nullptr;
    curSlice_.sliceId = gSliceInvalid;
    ImageCacheManager::GetInstance().EnterAod();
    StartSlice(VIEW_AOD);
    ResumeSlice();
    PageTransitionMgr::GetInstance().EndTransition(true);
}

void BaseAbility::ExitAod(uint32_t targetId)
{
    TransitionTarget target;
    target.sliceInfo.sliceAndPageId = targetId;
    target.sliceInfo.data = nullptr;
    target.sliceInfo.dataLength = 0;
    bool runImmed = PageTransitionMgr::GetInstance().BeginTransition(
        TransitionCaller::CHANGE_SLICE, target, TransitionType::TRANSITION_INVALID, false);
    if (!runImmed) {
        printf("pending change slice id:%u type:%d enableSlideBack:%d\n",
            target.sliceInfo.sliceAndPageId, static_cast<int>(TransitionType::TRANSITION_INVALID), false);
        return;
    }
    allSlices_.PopBack();
    PauseSlice();
    StopSlice();
    ImageCacheManager::GetInstance().ExitAod();
    if ((targetId & SLICE_MASK) == prevAodSlice_.sliceId) {  // back to prev slice
        if (prevAodSlice_.slice != nullptr) {
            nextSlice_ = prevAodSlice_;
            prevAodSlice_.slice = nullptr;
            prevAodSlice_.sliceId = gSliceInvalid;
        }
        InputDeviceManager::GetInstance()->ClearInputDeviceState();
        // 退出到AOD前一个应用，不会调用前一个应用的OnStart，因此需要显式更新下屏幕方向
        RotationManager::GetInstance().UpdateDeviceRotation(targetId & SLICE_MASK, targetId >> PAGE_OFFSET);
        AbilitySlice *slice = dynamic_cast<AbilitySlice*>(nextSlice_.slice);
        if (slice != nullptr && slice->GetCurRotation() != RotationManager::GetInstance().GetCurContentRotation()) {
            slice->OnOrientationChanged(RotationManager::GetInstance().GetCurContentRotation());
        }
    } else {
        if (prevAodSlice_.slice != nullptr) {
            WEARABLE_LOGI(
                WEARABLE_LOG_MODULE_LAUNCHER_FWK, "Ability StopSlice when exits aod %u", prevAodSlice_.sliceId);
            prevAodSlice_.slice->OnStop();
            prevAodSlice_.sliceId = gSliceInvalid;
            delete prevAodSlice_.slice;
            prevAodSlice_.slice = nullptr;
        }
        StartSlice(targetId);
    }
    ResumeSlice();
    PageTransitionMgr::GetInstance().EndTransition(true);
}

void BaseAbility::PostChangeSlice(uint32_t targetId, TransitionType type, void* data, bool needCacheSlice)
{
    if (curSlice_.sliceId == VIEW_AOD) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "cannot exit aod by switch slice!");
        PageTransitionMgr::GetInstance().EndTransition(true);
        if (data != nullptr) {
            UIFree(data);
        }
        return;
    }
    if ((prevSlice_.sliceId != (targetId & SLICE_MASK)) || (!PageTransitionMgr::GetInstance().IsBackTransitionRunning())) {
        ClearCacheSlice();
    }
    PauseSlice();
    // 目标内容方向与当前内容方向不符，需要停止跳转动效
    AbilitySlice *curSlice = dynamic_cast<AbilitySlice *>(curSlice_.slice);
    if (curSlice == nullptr) {
        GRAPHIC_LOGE("cur slice is nullptr!");
        return;
    }
    isSameRotation_ =
        curSlice->GetCurRotation() ==
        RotationManager::GetInstance().GetExpectedRotation(targetId & SLICE_MASK, targetId >> PAGE_OFFSET);

    PageTransitionMgr::GetInstance().SaveCurrentSourceIfNecessary(curSlice_.slice->GetSliceContainer());
    if (!PageTransitionMgr::GetInstance().IsTransAnimNeeded() ||
        PageTransitionMgr::GetInstance().IsCurrentSnapshotEnabled() || !isSameRotation_) {
        if (!needCacheSlice) {
            StopSlice();
        } else {
            UpdateCacheSlice();
        }
    }
    if ((prevSlice_.sliceId == (targetId & SLICE_MASK)) && (PageTransitionMgr::GetInstance().IsBackTransitionRunning())) {
        transitionPrevTargetId_ = GetCurSliceId();
        nextSlice_ = prevSlice_;
        prevSlice_.slice = nullptr;
        prevSlice_.sliceId = gSliceInvalid;
        int16_t popCnt = PopToTarget(targetId);
        PageTransitionMgr::GetInstance().SetSlicePopCnt(popCnt);
        PageTransitionMgr::GetInstance().ClearTopSlideBackImage();
    } else  {
        StartSlice(targetId, data);
    }

    if (!PageTransitionMgr::GetInstance().IsTransAnimNeeded() || !isSameRotation_) {
        ResumeSliceWithoutJS(needCacheSlice);
    } else {
        if (prevSlice_.slice != nullptr) {
            prevSlice_.slice->GetSliceContainer()->SetVisible(false);
        }
        PageTransitionMgr::GetInstance().SaveTargetSource(nextSlice_.slice->GetSliceContainer());
        PageTransitionMgr::GetInstance().StartTransAnim(std::bind(&BaseAbility::ResumeSliceWithoutJS, this, needCacheSlice));
    }
    if (data != nullptr) {
        UIFree(data);
    }
}

void BaseAbility::ResumeSliceWithoutJS(bool needCacheSlice)
{
    if (PageTransitionMgr::GetInstance().IsTransAnimNeeded() &&
        !PageTransitionMgr::GetInstance().IsCurrentSnapshotEnabled() && isSameRotation_) {
        if (!needCacheSlice) {
            StopSlice();
        } else {
            UpdateCacheSlice();
        }
    }
    ResumeSlice();
    if (prevSlice_.slice != nullptr) {
        prevSlice_.slice->GetSliceContainer()->SetVisible(true);
    }
    PageTransitionMgr::GetInstance().EndTransition(true);
    isSameRotation_ = true;
}

void BaseAbility::ResumeSlice()
{
    // if new slice was created, set is as current slice and resume it
    if (nextSlice_.slice != nullptr) {
        if (curSlice_.slice != nullptr) {
            GRAPHIC_LOGE("Both slice %u and %u is valid during resume", curSlice_.sliceId, nextSlice_.sliceId);
        }
        curSlice_.sliceId = nextSlice_.sliceId;
        curSlice_.slice = nextSlice_.slice;
        nextSlice_.sliceId = gSliceInvalid;
        nextSlice_.slice = nullptr;
    }

    // save listener before resume, resume will set new listener
    RootView::OnKeyActListener* keyActListener = RootView::GetInstance()->GetOnKeyActListener();
    RootView::OnGestureListener* gestureListener = RootView::GetInstance()->GetGestureListener();

    if (curSlice_.slice != nullptr) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "BaseAbility ResumeSlice %u", curSlice_.sliceId);
        curSlice_.slice->OnResume();
    } else {
        GRAPHIC_LOGE("No valid slice to resume");
    }
    if (NotificationManager::GetInstance()->HasNotifyShowing() && NotificationManager::GetInstance()->IsCurNotifyNeedPause()) {
        PauseSlice();
        // listener will be cleared after slice pause, so saved listener will be set again
        RootView::GetInstance()->SetOnKeyActListener(keyActListener);
        RootView::GetInstance()->SetGestureListener(gestureListener);
    }
}

void BaseAbility::PauseSlice(PauseReason reason)
{
    if (curSlice_.slice != nullptr) {
        InputDeviceManager::GetInstance()->ClearInputDeviceState();
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "BaseAbility PauseSlice %d", curSlice_.sliceId);
        curSlice_.slice->OnPause(reason);
    } else {
        GRAPHIC_LOGE("No valid slice to pause");
    }
}

void BaseAbility::StopSlice()
{
    if (curSlice_.slice != nullptr) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "BaseAbility StopSlice %u", curSlice_.sliceId);
        curSlice_.slice->OnStop();
        curSlice_.sliceId = gSliceInvalid;
        delete curSlice_.slice;
        curSlice_.slice = nullptr;
        curSlicePriority_ = gSliceDefaultPriority;
    } else {
        GRAPHIC_LOGE("No valid slice to stop");
    }
}

void BaseAbility::StopSliceTransition()
{
    PageTransitionMgr::GetInstance().SyncStopAnimator();
    AbilitySlice* slice = dynamic_cast<AbilitySlice*>(curSlice_.slice);
    if (slice != nullptr) {
        slice->StopSlicePageSwitchAnimator();
    }
}

void BaseAbility::BackToPreSlice(TransitionType type)
{
    uint32_t targetId = GetPreTargetId();
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "BaseAbility BackToPreSlice %u", targetId);
    if (targetId == gSliceInvalid) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "previous slice is invalid");
        return;
    }
    TransitionTarget target;
    target.sliceInfo.sliceAndPageId = targetId;
    target.sliceInfo.data = nullptr;
    target.sliceInfo.dataLength = 0;
    uint16_t sliceId = static_cast<uint16_t>(targetId & SLICE_MASK);
    uint16_t pageId = static_cast<uint16_t>(targetId >> PAGE_OFFSET);
    if (!PageTransitionMgr::GetInstance().BeginBackTransition(TransitionCaller::CHANGE_SLICE, target)) {
        GRAPHIC_LOGE("BeginBackTransition Failed!!");
        return;
    }
    SwitchSlice(sliceId, pageId, type);
}

void BaseAbility::BackToSliceInStack(uint32_t targetId, TransitionType type)
{
    if (targetId == gSliceInvalid) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "targetId is invalid");
        return;
    }

    if (!IsSliceInStack(targetId)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "targetId: %u is not cached!", targetId);
        return;
    }

    TransitionTarget target;
    target.sliceInfo.sliceAndPageId = targetId;
    target.sliceInfo.data = nullptr;
    target.sliceInfo.dataLength = 0;
    uint16_t sliceId = static_cast<uint16_t>(targetId & SLICE_MASK);
    uint16_t pageId = static_cast<uint16_t>(targetId >> PAGE_OFFSET);

    if (!PageTransitionMgr::GetInstance().BeginBackTransition(TransitionCaller::CHANGE_SLICE, target)) {
        return;
    }
    SwitchSlice(sliceId, pageId, type);
}

bool BaseAbility::IsSliceInStack(uint32_t targetId)
{
    for (int i = 0; i < allSlices_.Size(); i++) {
        if (allSlices_[i] == targetId) {
            return true;
        }
    }
    return false;
}

int16_t BaseAbility::PopToTarget(uint32_t targetId)
{
    int16_t allSliceSize = allSlices_.Size();
    int16_t index = allSliceSize - 1;
    bool isCached = false;
    for (; index >= 0; index--) {
        if (allSlices_[index] == targetId) {
            isCached = true;
            break;
        }
    }
    if (!isCached) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "target %d is not in back queue", targetId);
        return 0;
    }

    int16_t popNum = allSliceSize - 1 - index;
    for (int16_t i = 0; i < popNum; i++) {
        allSlices_.PopBack();
    }
    return popNum;
}

void BaseAbility::ClearCacheSlice()
{
    if (prevSlice_.slice != nullptr) {
        PageTransitionMgr::GetInstance().UpdateCacheSliceBackSnapShot(prevSlice_.slice->GetSliceContainer());
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "Ability Clear Cache Slice %u", prevSlice_.sliceId);
        prevSlice_.slice->OnStop();
        prevSlice_.sliceId = gSliceInvalid;
        delete prevSlice_.slice;
        prevSlice_.slice = nullptr;
    }
}

void BaseAbility::UpdateCacheSlice()
{
    if (curSlice_.slice != nullptr) {
        if (prevSlice_.slice != nullptr) {
            GRAPHIC_LOGE("Cache slice %u need clear before update", prevSlice_.sliceId);
            return;
        }
        prevSlice_.sliceId = curSlice_.sliceId;
        prevSlice_.slice = curSlice_.slice;
    } else {
        GRAPHIC_LOGE("Cur slice is nullptr during update cache Slice");
    }
}
} // namespace OHOS
