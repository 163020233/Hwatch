/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SamplePage — template for native app development
 * Create: 2025-06-08
 */

#include "UiConfig.h"
#include "SlicePageFactory.h"
#include "wearable_log.h"
#include "AppViewIDs.h"
#include "NativeAbility.h"
#include "samplepage/SamplePresenter.h"
#include "samplepage/SamplePage.h"

namespace OHOS {
static constexpr uint16_t SAMPLE_PAGE = 1;

REGIST_SLICE_PAGE(VIEW_APP, SAMPLE_PAGE, SamplePage, true);

SamplePage::SamplePage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SamplePage::SamplePage");
}

SamplePage::~SamplePage()
{
    if (container_ != nullptr) {
        container_->RemoveAll();
        delete container_;
        container_ = nullptr;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SamplePage::~SamplePage");
}

void SamplePage::OnStart(void* data)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SamplePage OnStart");
    container_ = new UIViewGroup();
    if (container_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SamplePage container_ new fail");
        return;
    }
    container_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    container_->SetOnDragListener(this);
    container_->SetDraggable(true);
    container_->SetTouchable(true);

    /* TODO: add your UI components here */

    AddViewToPageContainer(container_);
}

void SamplePage::OnResume()
{
    /* TODO: resume logic (e.g. restart timers, refresh data) */
}

void SamplePage::OnPause(PauseReason reason)
{
    /* TODO: pause logic (e.g. stop timers, save state) */
    (void)reason;
}

void SamplePage::OnStop()
{
    /* TODO: cleanup logic */
}

bool SamplePage::OnDrag(UIView& view, const DragEvent& event)
{
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        NativeAbility::GetInstance().ChangeSliceToApplist();
    }
    return false;
}
}
