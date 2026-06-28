/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BreathMainPage
 * Create: 2025-5-13
 */
#include <sstream>
#include <sys/time.h>
#include "AppViewIDs.h"
#include "NativeAbility.h"
#include "PageTransitionMgr.h"
#include "SlicePageFactory.h"
#include "TransitionType.h"
#include "UiConfig.h"
#include "gfx_utils/graphic_math.h"
#include "main/LoadImg.h"
#include "ui_resource_breath.h"
#include "ui_view_ids_breath.h"
#include "wearable_log.h"
#ifdef BUILD_IN_LINUX
#include "localtime.h"
#endif
#include "breathexercise/BreathModel.h"
#include "breathexercise/BreathMainPage.h"

namespace OHOS {
static constexpr int16_t BREATH_MAIN_TITLE_X = 97;
static constexpr int16_t BREATH_MAIN_TITLE_Y = 34;
static constexpr int16_t BREATH_MAIN_TITLE_W = 260;
static constexpr int16_t BREATH_MAIN_TITLE_H = 53;
static constexpr int16_t BREATH_MAIN_TITLE_FONT_SIZE = 38;
static constexpr int16_t BREATH_MAIN_DURBTN_X = 16;
static constexpr int16_t BREATH_MAIN_DURBTN_Y = 190;
static constexpr int16_t BREATH_MAIN_DURBTN_W = 74;
static constexpr int16_t BREATH_MAIN_DURBTN_H = 74;
static constexpr int16_t BREATH_MAIN_RHYBTN_X = 364;
static constexpr int16_t BREATH_MAIN_RHYBTN_Y = 190;
static constexpr int16_t BREATH_MAIN_RHYBTN_W = 74;
static constexpr int16_t BREATH_MAIN_RHYBTN_H = 74;
static constexpr int16_t BREATH_MAIN_DURLAB_X = 20;
static constexpr int16_t BREATH_MAIN_DURLAB_Y = 275;
static constexpr int16_t BREATH_MAIN_DURLAB_W = 65;
static constexpr int16_t BREATH_MAIN_DURLAB_H = 30;
static constexpr int16_t BREATH_MAIN_RHYLAB_X = 376;
static constexpr int16_t BREATH_MAIN_RHYLAB_Y = 275;
static constexpr int16_t BREATH_MAIN_RHYLAB_W = 50;
static constexpr int16_t BREATH_MAIN_RHYLAB_H = 30;
static constexpr int16_t BREATH_MAIN_SETTING_FONT_SIZE = 24;
static constexpr int16_t BREATH_MAIN_TIME_FONT_SIZE = 20;
static constexpr uint16_t Y_OFFSET = 300;

REGIST_SLICE_PAGE(VIEW_BREATH, BREATH_PAGE::BREATH_MAIN_VIEW, BreathMainPage, true);

BreathMainPage::BreathMainPage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BreathMainPage::BreathMainPage");
}

BreathMainPage::~BreathMainPage()
{
    if (group_ != nullptr) {
        group_->RemoveAll();
        delete group_;
        group_ = nullptr;
    }
    if (lableSetting_ != nullptr) {
        delete lableSetting_;
        lableSetting_ = nullptr;
    }
    if (buttonStart_ != nullptr) {
        delete buttonStart_;
        buttonStart_ = nullptr;
    }
    if (buttonSetting_ != nullptr) {
        delete buttonSetting_;
        buttonSetting_ = nullptr;
    }
    if (buttonTime_ != nullptr) {
        delete buttonTime_;
        buttonTime_ = nullptr;
    }
    if (labelTitle_ != nullptr) {
        delete labelTitle_;
        labelTitle_ = nullptr;
    }
    if (lableTime_ != nullptr) {
        delete lableTime_;
        lableTime_ = nullptr;
    }
    if (bgImg_ != nullptr) {
        delete bgImg_;
        bgImg_ = nullptr;
    }
    if (center_ != nullptr) {
        delete center_;
        center_ = nullptr;
    }
    ImageCacheManager::GetInstance().UnloadAllInMultiRes(BREATH_IMAGE);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BreathMainPage::~BreathMainPage");
}

void BreathMainPage::OnStart(void *data)
{
    group_ = new UIViewGroup();
    if (group_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new group_ ERROR");
        return;
    }
    group_->SetPosition(0, 0);
    group_->Resize(HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    group_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    group_->SetTouchable(true);
    group_->SetDraggable(true);
    group_->SetOnDragListener(this);

    bgImg_ = new UIImageView();
    if (bgImg_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new Background ERROR");
        return;
    }
    bgImg_->SetPosition(0, 0);
    if (HORIZONTAL_RESOLUTION == 454) { // 454: 表盘分辨率
        LOADIMG::LoadImageViewImg(bgImg_, BREATH_IMAGE, IMAGE_BREATH_BG1);
    } else if (HORIZONTAL_RESOLUTION == 466) { // 466: 表盘分辨率
        LOADIMG::LoadImageViewImg(bgImg_, BREATH_IMAGE, IMAGE_BREATH_BG1_466);
    }
    group_->Add(bgImg_);

    center_ = new UIImageView();
    if (center_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new Center ERROR");
        return;
    }
    LOADIMG::LoadImageViewImg(center_, BREATH_IMAGE, IMAGE_BREATH_CENTER2);
    int x = (HORIZONTAL_RESOLUTION - center_->GetWidth()) / 2;
    int y = (HORIZONTAL_RESOLUTION - center_->GetHeight()) / 2;
    center_->SetPosition(x, y);
    group_->Add(center_);

    labelTitle_ = new UILabel();
    if (labelTitle_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelTitle_ ERROR");
        return;
    }
    labelTitle_->SetPosition(BREATH_MAIN_TITLE_X, BREATH_MAIN_TITLE_Y);
    labelTitle_->Resize(BREATH_MAIN_TITLE_W, BREATH_MAIN_TITLE_H);
    labelTitle_->SetText("呼吸");
    labelTitle_->SetTextColor(Color::White());
    labelTitle_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    labelTitle_->SetFont(BOLD_VECTOR_FONT_FILENAME, BREATH_MAIN_TITLE_FONT_SIZE);
    group_->Add(labelTitle_);

    buttonTime_ = new UIButton();
    if (buttonTime_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new buttonTime_ ERROR");
        return;
    }
    buttonTime_->SetPosition(BREATH_MAIN_DURBTN_X, BREATH_MAIN_DURBTN_Y);
    buttonTime_->Resize(BREATH_MAIN_DURBTN_W, BREATH_MAIN_DURBTN_H);
    buttonTime_->SetViewId(UIVIEW_BREATH_UIBUTTON_TIMESET_BUTTON);
    buttonTime_->SetOnClickListener(this);
    LOADIMG::LoadBtnImage(buttonTime_, BREATH_IMAGE, IMAGE_BREATH_TIME, IMAGE_BREATH_TIME);
    buttonTime_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::PRESSED);
    buttonTime_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    group_->Add(buttonTime_);

    buttonSetting_ = new UIButton();
    if (buttonSetting_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new buttonSetting_ ERROR");
        return;
    }
    buttonSetting_->SetPosition(BREATH_MAIN_RHYBTN_X, BREATH_MAIN_RHYBTN_Y);
    buttonSetting_->Resize(BREATH_MAIN_RHYBTN_W, BREATH_MAIN_RHYBTN_H);
    buttonSetting_->SetViewId(UIVIEW_BREATH_UIBUTTON_RHYTHM_SET_BUTTON);
    buttonSetting_->SetOnClickListener(this);
    LOADIMG::LoadBtnImage(buttonSetting_, BREATH_IMAGE, IMAGE_BREATH_SETTING, IMAGE_BREATH_SETTING);
    buttonSetting_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::PRESSED);
    buttonSetting_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    group_->Add(buttonSetting_);

    buttonStart_ = new UIImageView();
    if (buttonStart_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new buttonStart_ ERROR");
        return;
    }
    LOADIMG::LoadImageViewImg(buttonStart_, BREATH_IMAGE, IMAGE_BREATH_START);
    int posX = (HORIZONTAL_RESOLUTION - buttonStart_->GetWidth()) / 2;
    int posY = HORIZONTAL_RESOLUTION - buttonStart_->GetHeight();
    buttonStart_->SetPosition(posX, posY);
    buttonStart_->SetViewId(UIVIEW_BREATH_UIIMAGEVIEW_START_BUTTON);
    buttonStart_->SetTouchable(true);
    buttonStart_->SetOnClickListener(this);
    buttonStart_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    group_->Add(buttonStart_);

    lableSetting_ = new UILabel();
    if (lableSetting_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new lableSetting_ ERROR");
        return;
    }
    lableSetting_->SetPosition(BREATH_MAIN_RHYLAB_X, BREATH_MAIN_RHYLAB_Y);
    lableSetting_->Resize(BREATH_MAIN_RHYLAB_W, BREATH_MAIN_RHYLAB_H);
    lableSetting_->SetText("设置");
    lableSetting_->SetLineBreakMode(UILabel::LINE_BREAK_ELLIPSIS);
    lableSetting_->SetTextColor(Color::White());
    lableSetting_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    lableSetting_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BREATH_MAIN_SETTING_FONT_SIZE);
    group_->Add(lableSetting_);

    lableTime_ = new UILabel();
    if (lableTime_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new lableTime_ ERROR");
        return;
    }
    lableTime_->SetPosition(BREATH_MAIN_DURLAB_X, BREATH_MAIN_DURLAB_Y);
    lableTime_->Resize(BREATH_MAIN_DURLAB_W, BREATH_MAIN_DURLAB_H);
    lableTime_->SetLineBreakMode(UILabel::LINE_BREAK_ELLIPSIS);
    lableTime_->SetTextColor(Color::White());
    lableTime_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    lableTime_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BREATH_MAIN_TIME_FONT_SIZE);
    int time = BreathModel::GetInstance().GetTrainTime() + 1;
    std::ostringstream oss;
    oss << time << " 分钟";
    std::string timeStr = oss.str();
    lableTime_->SetText(timeStr.c_str());
    group_->Add(lableTime_);

    AddViewToPageContainer(group_);
}

bool BreathMainPage::OnClick(UIView &view, const ClickEvent &event)
{
    if (strcmp(view.GetViewId(), UIVIEW_BREATH_UIBUTTON_TIMESET_BUTTON) == 0) {
        NativeAbility::GetInstance().SwitchPageInSlice(BREATH_PAGE::BREATH_DUR_VIEW,
            TransitionType::TRANSITION_INVALID, false);
    } else if (strcmp(view.GetViewId(), UIVIEW_BREATH_UIBUTTON_RHYTHM_SET_BUTTON) == 0) {
        NativeAbility::GetInstance().SwitchPageInSlice(BREATH_PAGE::BREATH_RHY_VIEW,
            TransitionType::TRANSITION_INVALID, false);
    } else if (strcmp(view.GetViewId(), UIVIEW_BREATH_UIIMAGEVIEW_START_BUTTON) == 0) {
        NativeAbility::GetInstance().SwitchPageInSlice(BREATH_PAGE::BREATH_START_VIEW,
            TransitionType::TRANSITION_INVALID, false);
#ifdef _WIN32
        struct timeval tv;
        gettimeofday(&tv, NULL);
#else
        struct timeval64 tv;
        gettimeofday64(&tv, NULL);
#endif
        uint32_t remTime = static_cast<uint32_t>(tv.tv_sec);
        BreathModel::GetInstance().SetStartTime(remTime);
    } else {
        return false;
    }
    return true;
}

bool BreathMainPage::OnDrag(UIView &view, const DragEvent &event)
{
    return true;
}
 
bool BreathMainPage::OnDragStart(UIView& view, const DragEvent& event)
{
    return true;
}
 
bool BreathMainPage::OnDragEnd(UIView &view, const DragEvent &event)
{
    UNUSED(view);
    if (event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) {
        NativeAbility::GetInstance().ChangeSliceToApplist();
    } else if (event.GetDragDirection() == DragEvent::DIRECTION_BOTTOM_TO_TOP) {
        NativeAbility::GetInstance().SwitchPageInSlice(BREATH_PAGE::BREATH_TRAINING_VIEW,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}
}