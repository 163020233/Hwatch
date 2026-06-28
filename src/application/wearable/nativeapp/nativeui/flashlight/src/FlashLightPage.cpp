/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: FlashLightPage
 * Create: 2025-04-24
 */

#include <string>
#include "SlicePageFactory.h"
#include "AppViewIDs.h"
#include "ui_view_ids_flashlight.h"
#include "wearable_log.h"
#include "main/LoadImg.h"
#include "ui_resource_flashlight.h"
#include "flashlight/FlashLightView.h"
#include "flashlight/FlashLightPresenter.h"
#include "flashlight/FlashLightModel.h"
#include "flashlight/FlashLightPage.h"

namespace OHOS {
static constexpr uint16_t SWITCH_BUTTON_POS_X = 168;
static constexpr uint16_t SWITCH_BUTTON_POS_Y = 168;
static constexpr uint16_t SWITCH_BUTTON_WIDTH = 119;
static constexpr uint16_t SWITCH_BUTTON_HEIGHT = 119;
static constexpr uint16_t SWITCH_LABEL_POS_X = 168;
static constexpr uint16_t SWITCH_LABEL_POS_Y = 308;
static constexpr uint16_t SWITCH_LABEL_WIDTH = 128;
static constexpr uint16_t SWITCH_LABEL_HEIGHT = 46;
static constexpr uint16_t SWITCH_LABEL_FONT_SIZE = 32;
static constexpr uint16_t TIME_LABEL_FONT_SIZE = 38;
static constexpr uint16_t IMAGE_VIEW_POS_X = 213;
static constexpr uint16_t IMAGE_VIEW_POS_Y = 414;
static constexpr uint16_t TIME_TEXT_POS_X = 151;
static constexpr uint16_t TIME_TEXT_POS_Y = 31;
static constexpr uint16_t TIME_TEXT_WIDTH = 153;
static constexpr uint16_t TIME_TEXT_HEIGHT = 53;
static constexpr uint16_t Y_OFFSET = 300;

REGIST_SLICE_PAGE(VIEW_FLASHLIGHT, FlashLightAllView::FLASH_LIGHT_VIEW, FlashLightPage, true);
void FlashLightPage::OnStart(void *data)
{
    container_ = new UIScrollView();
    if (container_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "malloc fail, %s, %d\r\n", __func__, __LINE__);
        return;
    }
    container_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);

    tipsLabel_ = new UILabel();
    if (tipsLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "malloc fail, %s, %d\r\n", __func__, __LINE__);
        return;
    }
    tipsLabel_->SetPosition(SWITCH_LABEL_POS_X, SWITCH_LABEL_POS_Y, SWITCH_LABEL_WIDTH, SWITCH_LABEL_HEIGHT);
    tipsLabel_->SetText("点击照明");
    tipsLabel_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    tipsLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SWITCH_LABEL_FONT_SIZE);
    tipsLabel_->SetStyle(STYLE_BACKGROUND_COLOR, OPA_OPAQUE);
    tipsLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    tipsLabel_->SetVisible(true);
    container_->Add(tipsLabel_);

    flashLightButtonOn_ = new UILabelButton();
    if (flashLightButtonOn_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "malloc fail, %s, %d\r\n", __func__, __LINE__);
        return;
    }
    flashLightButtonOn_->SetPosition(SWITCH_BUTTON_POS_X, SWITCH_BUTTON_POS_Y,
                                     SWITCH_BUTTON_WIDTH, SWITCH_BUTTON_HEIGHT);
    flashLightButtonOn_->SetViewId(UIVIEW_FLASHLIGHT_UILABELBUTTON_LIGHTING);
    flashLightButtonOn_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    flashLightButtonOn_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::PRESSED);
    LOADIMG::LoadBtnImage(flashLightButtonOn_, FLASHLIGHT_IMAGE, IMAGE_FLASHLIGHT_FLASHLIGHTON,
                          IMAGE_FLASHLIGHT_FLASHLIGHTON);
    flashLightButtonOn_->SetVisible(true);
    container_->Add(flashLightButtonOn_);

    // 创建时间显示组件
    systemTime_ = new UILabel();
    if (systemTime_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "malloc fail, %s, %d\r\n", __func__, __LINE__);
        return;
    }
    systemTime_->SetPosition(TIME_TEXT_POS_X, TIME_TEXT_POS_Y, TIME_TEXT_WIDTH, TIME_TEXT_HEIGHT);
    systemTime_->SetText("12:00");
    systemTime_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    systemTime_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, TIME_LABEL_FONT_SIZE);
    systemTime_->SetStyle(STYLE_BACKGROUND_COLOR, OPA_OPAQUE);
    systemTime_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    systemTime_->SetVisible(true);
    container_->Add(systemTime_);

    timeCallback_ = new TimeAnimatorCallback();
    if (timeCallback_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "malloc fail, %s, %d\r\n", __func__, __LINE__);
        return;
    }

    timeAnimator_ = new Animator(timeCallback_, systemTime_, 0, true);
    if (timeAnimator_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "malloc fail, %s, %d\r\n", __func__, __LINE__);
        return;
    }
    timeAnimator_->Start();

    imageArrow_ = new UIImageView();
    if (imageArrow_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "malloc fail, %s, %d\r\n", __func__, __LINE__);
        return;
    }
    imageArrow_->SetPosition(IMAGE_VIEW_POS_X, IMAGE_VIEW_POS_Y);
    container_->Add(imageArrow_);
    imageArrow_->SetVisible(true);
    LOADIMG::LoadImageViewImg(imageArrow_, FLASHLIGHT_IMAGE, IMAGE_FLASHLIGHT_DOWNARROW);

    flashLightButtonOn_->SetOnClickListener(this);
    container_->SetOnDragListener(this);
    AddViewToPageContainer(container_);
}

FlashLightPage::FlashLightPage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "FlashLightPage::FlashLightPage");
}

FlashLightPage::~FlashLightPage()
{
    if (container_ != nullptr) {
        container_->RemoveAll();
        delete container_;
        container_ = nullptr;
    }
    if (flashLightButtonOn_ != nullptr) {
        delete flashLightButtonOn_;
        flashLightButtonOn_ = nullptr;
    }
    if (systemTime_ != nullptr) {
        delete systemTime_;
        systemTime_ = nullptr;
    }
    if (tipsLabel_ != nullptr) {
        delete tipsLabel_;
        tipsLabel_ = nullptr;
    }
    if (imageArrow_ != nullptr) {
        delete imageArrow_;
        imageArrow_ = nullptr;
    }
    if (timeCallback_ != nullptr) {
        delete timeCallback_;
        timeCallback_ = nullptr;
    }
    if (timeAnimator_ != nullptr) {
        delete timeAnimator_;
        timeAnimator_ = nullptr;
    }
    ImageCacheManager::GetInstance().UnloadSingleRes(FLASHLIGHT_IMAGE);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "FlashLightPage::~FlashLightPage");
}

bool FlashLightPage::OnDrag(UIView &view, const DragEvent &event)
{
    return true;
}
 
bool FlashLightPage::OnDragStart(UIView& view, const DragEvent& event)
{
    return true;
}
 
bool FlashLightPage::OnDragEnd(UIView &view, const DragEvent &event)
{
    if (event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) {
        NativeAbility::GetInstance().ChangeSliceToApplist();
    } else if (event.GetDragDirection() == DragEvent::DIRECTION_BOTTOM_TO_TOP) {
        NativeAbility::GetInstance().SwitchPageInSlice(FlashLightAllView::FLASH_LIGHT_SET_VIEW,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}

bool FlashLightPage::OnClick(UIView &view, const ClickEvent &event)
{
    if (strcmp(view.GetViewId(), UIVIEW_FLASHLIGHT_UILABELBUTTON_LIGHTING) == 0) {
        NativeAbility::GetInstance().SwitchPageInSlice(FlashLightAllView::FLASH_LIGHT_PLAY_VIEW,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}

void FlashLightPage::TimeAnimatorCallback::Callback(UIView *view)
{
    UILabel *timeLabel = dynamic_cast<UILabel *>(view);
    if (timeLabel == nullptr) {
        return;
    }
    std::string currTime;
    FlashLightModel::GetInstance().GetSysCurrentTime(currTime);
    timeLabel->SetText(currTime.c_str());
    timeLabel->Invalidate();
}
}
