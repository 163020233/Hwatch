/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BreathTimeSetPage
 * Create: 2025-5-13
 */
#include "AppViewIDs.h"
#include "NativeAbility.h"
#include "SlicePageFactory.h"
#include "TransitionType.h"
#include "UiConfig.h"
#include "components/root_view.h"
#include "gfx_utils/graphic_math.h"
#include "main/LoadImg.h"
#include "ui_resource_breath.h"
#include "ui_view_ids_breath.h"
#include "wearable_log.h"
#include "breathexercise/BreathView.h"
#include "breathexercise/BreathTimeSetPage.h"

namespace OHOS {
static constexpr uint16_t BREATH_TIME_TITLE_X = 100;
static constexpr uint16_t BREATH_TIME_TITLE_Y = 38;
static constexpr uint16_t BREATH_TIME_TITLE_W = 253;
static constexpr uint16_t BREATH_TIME_TITLE_H = 53;
static constexpr uint16_t BREATH_TIME_TITLE_FONT_SIZE = 38;
static constexpr uint16_t BREATH_TIME_PICKER_X = 64;
static constexpr uint16_t BREATH_TIME_PICKER_Y = 109;
static constexpr uint16_t BREATH_TIME_PICKER_W = 321;
static constexpr uint16_t BREATH_TIME_PICKER_H = 218;
static constexpr uint16_t BREATH_TIME_PICKER_FONT_SIZE = 44;
static constexpr uint16_t BREATH_TIME_PICKER_ITEM_H = 80;
static constexpr uint8_t PICKER_SIZE = 3;
const char *g_timeList[PICKER_SIZE] = {"1 分钟", "2 分钟", "3 分钟"};

REGIST_SLICE_PAGE(VIEW_BREATH, BREATH_PAGE::BREATH_DUR_VIEW, BreathTimeSetPage, false);

BreathTimeSetPage::BreathTimeSetPage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BreathTimeSetPage::BreathTimeSetPage");
}

BreathTimeSetPage::~BreathTimeSetPage()
{
    if (group_ != nullptr) {
        group_->RemoveAll();
        delete group_;
        group_ = nullptr;
    }
    if (labelTitle_ != nullptr) {
        delete labelTitle_;
        labelTitle_ = nullptr;
    }
    if (pickerTime_ != nullptr) {
        delete pickerTime_;
        pickerTime_ = nullptr;
    }
    if (buttonOk_ != nullptr) {
        delete buttonOk_;
        buttonOk_ = nullptr;
    }
    if (pickerImg_ != nullptr) {
        delete pickerImg_;
        pickerImg_ = nullptr;
    }
    ImageCacheManager::GetInstance().UnloadAllInMultiRes(BREATH_IMAGE);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BreathTimeSetPage::~BreathTimeSetPage");
}

void BreathTimeSetPage::OnStart(void *data)
{
    group_ = new UIViewGroup();
    if (group_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new Group ERROR");
        return;
    }
    group_->SetPosition(0, 0);
    group_->Resize(HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    group_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);

    labelTitle_ = new UILabel();
    if (labelTitle_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelTitle_ ERROR");
        return;
    }
    labelTitle_->SetPosition(BREATH_TIME_TITLE_X, BREATH_TIME_TITLE_Y);
    labelTitle_->Resize(BREATH_TIME_TITLE_W, BREATH_TIME_TITLE_H);
    labelTitle_->SetText("时间设置");
    labelTitle_->SetTextColor(Color::White());
    labelTitle_->SetFont(BOLD_VECTOR_FONT_FILENAME, BREATH_TIME_TITLE_FONT_SIZE);
    labelTitle_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    group_->Add(labelTitle_);

    pickerTime_ = new UIPicker();
    if (pickerTime_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new pickerTime_ ERROR");
        return;
    }
    pickerTime_->SetPosition(BREATH_TIME_PICKER_X, BREATH_TIME_PICKER_Y);
    pickerTime_->Resize(BREATH_TIME_PICKER_W, BREATH_TIME_PICKER_H);
    pickerTime_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    pickerTime_->SetBackgroundFont(DEFAULT_VECTOR_FONT_FILENAME, BREATH_TIME_PICKER_FONT_SIZE);
    pickerTime_->SetHighlightFont(DEFAULT_VECTOR_FONT_FILENAME, BREATH_TIME_PICKER_FONT_SIZE);
    pickerTime_->SetItemHeight(BREATH_TIME_PICKER_ITEM_H);
    pickerTime_->SetTextColor(Color::GetColorFromRGBA(0xfa, 0xf3, 0xf3, 0xff),
                              Color::GetColorFromRGBA(0xfa, 0xf3, 0xf3, 0xff));
    uint8_t selectTime = BreathModel::GetInstance().GetTrainTime();
    pickerTime_->SetSelected(selectTime);
    pickerTime_->SetOnDragListener(this);
    pickerTime_->SetLoopState(false);
    pickerTime_->SetValues(g_timeList, PICKER_SIZE);
    GetSlicePageContainer()->SetIntercept(true);
    GetSlicePageContainer()->SetOnDragListener(nullptr);
    group_->Add(pickerTime_);

    pickerImg_ = new UIImageView();
    if (pickerImg_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new pickerImg_ ERROR");
        return;
    }
    pickerImg_->SetPosition(BREATH_TIME_PICKER_X, BREATH_TIME_PICKER_Y);
    pickerImg_->Resize(BREATH_TIME_PICKER_W, BREATH_TIME_PICKER_H);
    LOADIMG::LoadImageViewImg(pickerImg_, BREATH_IMAGE, IMAGE_BREATH_CENTER);
    group_->Add(pickerImg_);

    buttonOk_ = new UIImageView();
    if (buttonOk_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new buttonOk_ ERROR");
        return;
    }
    LOADIMG::LoadImageViewImg(buttonOk_, BREATH_IMAGE, IMAGE_BREATH_OK);
    int posX = (HORIZONTAL_RESOLUTION - buttonOk_->GetWidth()) / 2;
    int posY = HORIZONTAL_RESOLUTION - buttonOk_->GetHeight();
    buttonOk_->SetPosition(posX, posY);
    buttonOk_->SetViewId(UIVIEW_BREATH_UIIMAGEVIEW_TIMESET_OK_BUTTON);
    buttonOk_->SetTouchable(true);
    buttonOk_->SetOnClickListener(this);
    buttonOk_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    group_->Add(buttonOk_);

    AddViewToPageContainer(group_);
}

bool BreathTimeSetPage::OnClick(UIView &view, const ClickEvent &event)
{
    if (strcmp(view.GetViewId(), UIVIEW_BREATH_UIIMAGEVIEW_TIMESET_OK_BUTTON) == 0) {
        BreathModel::GetInstance().SetTrainTime(pickerTime_->GetSelected());
        NativeAbility::GetInstance().SwitchPageInSlice(BREATH_PAGE::BREATH_MAIN_VIEW,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}

bool BreathTimeSetPage::OnDrag(UIView &view, const DragEvent &event)
{
    return true;
}
 
bool BreathTimeSetPage::OnDragStart(UIView& view, const DragEvent& event)
{
    return true;
}
 
bool BreathTimeSetPage::OnDragEnd(UIView &view, const DragEvent &event)
{
    UNUSED(view);
    if (event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) {
        NativeAbility::GetInstance().SwitchPageInSlice(BREATH_PAGE::BREATH_MAIN_VIEW,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}
}