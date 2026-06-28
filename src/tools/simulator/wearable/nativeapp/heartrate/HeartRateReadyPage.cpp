/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: HeartRateReadyPage
 * Create: 2025-06
 */
#include <cmath>
#include <string>
#include "ui_test_group.h"
#include "main/LoadImg.h"
#include "ui_resource_heartRate.h"
#include "heartrate/HeartRatePresenter.h"
#include "heartrate/HeartRateReadyPage.h"

namespace OHOS {
static constexpr uint16_t HEARTRATE_BUTTON_POSITION_X = 188;
static constexpr uint16_t HEARTRATE_BUTTON_WIDTH = 96;
static constexpr uint16_t HEARTRATE_BUTTON_HEIGHT = 96;
static constexpr uint16_t HEARTRATE_BUTTON_TWO_POSITION_Y = 288;
static constexpr uint16_t HEARTRATE_BUTTON_TWO_WIDTH = 120;
static constexpr uint16_t HEARTRATE_LABEL3_POSITION_X = 65;
static constexpr uint16_t HEARTRATE_LABEL3_POSITION_Y = 150;
static constexpr uint16_t HEARTRATE_LABEL3_POSITION_WIDTH = 365;
static constexpr uint16_t HEARTRATE_LABEL3_POSITION_HEIGHT = 50;
static constexpr uint16_t HEARTRATE_LABLETIPSSATRT_POSITION_X = 110;
static constexpr uint16_t HEARTRATE_LABLETIPSSATRT_POSITION_Y = 200;
static constexpr uint16_t HEARTRATE_LABLETIPSSATRT_WIDTH = 424;
static constexpr uint16_t HEARTRATE_LABLETIPSSATRT_HEIGHT = 424;
static constexpr uint16_t HEARTRATE_LABLETIPSSATRT_FONT_SIZE = 36;
static constexpr uint16_t HEARTRATE_LABLEBACK_POSITION_X = 218;
static constexpr uint16_t HEARTRATE_LABLEBACK_POSITION_Y = 318;
static constexpr uint16_t HEARTRATE_LABLEBACK_WIDTH = 50;
static constexpr uint16_t HEARTRATE_LABLEBACK_HEIGHT = 50;
static constexpr uint16_t HEARTRATE_LABLEBACK_FONT_SIZE = 30;
static constexpr uint16_t HEARTRATE_PLAYBACK_POSITION_X = 140;
static constexpr uint16_t HEARTRATE_PLAYBACK_POSITION_Y = 250;
static constexpr uint16_t HEARTRATE_PLAYBACK_WIDTH = 172;
static constexpr uint16_t HEARTRATE_PLAYBACK_HEIGHT = 172;
static constexpr uint16_t HEARTRATE_PLAYBACK_VENTER_POSITION = 86;
static constexpr uint32_t HEARTRATE_PLAYBACK_RANGE = 100000;
static constexpr uint16_t HEARTRATE_PLAYBACK_RADIUS = 48;
static constexpr uint16_t HEARTRATE_PLAYBACK_START_ANGLE = 360;
static constexpr uint16_t HEARTRATE_PLAYBACK_STYLE_LINE_WIDTH = 12;
static constexpr uint16_t HEARTRATE_TIME_NUM = 4;
static constexpr uint16_t HEARTRATE_PROGRESS_TIME = 3;
static constexpr uint16_t HEARTRATE_TIME_USEC = 1000;

REGIST_SLICE_PAGE(VIEW_HEARTRATE, HeartRateAllView::HEARTRATE_READY_PAGE, HeartRateReadyPage, false);

HeartRateReadyPage::HeartRateReadyPage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "HeartRateReadyPage::HeartRateReadyPage");
}

HeartRateReadyPage::~HeartRateReadyPage()
{
    if (group_ != nullptr) {
        group_->RemoveAll();
        delete group_;
        group_ = nullptr;
    }

    if (button_ != nullptr) {
        delete button_;
        button_ = nullptr;
    }

    if (mainImg_ != nullptr) {
        delete mainImg_;
        mainImg_ = nullptr;
    }

    if (labelTipsStart_ != nullptr) {
        delete labelTipsStart_;
        labelTipsStart_ = nullptr;
    }

    if (playbackProgress_ != nullptr) {
        delete playbackProgress_;
        playbackProgress_ = nullptr;
    }

    if (labelBackProgress_ != nullptr) {
        delete labelBackProgress_;
        labelBackProgress_ = nullptr;
    }

    if (animator_ != nullptr) {
        delete animator_;
        animator_ = nullptr;
    }

    if (callBack_ != nullptr) {
        delete callBack_;
        callBack_ = nullptr;
    }

    ImageCacheManager::GetInstance().UnloadAllInMultiRes(HEART_RATE_IMAGE);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "HeartRateReadyPage::~HeartRateReadyPage");
}

void HeartRateReadyPage::OnStart(void *data)
{
    group_ = new UIViewGroup();
    if (group_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "HeartRateReadyPage::[OnStart] new UIViewGroup fail");
        return;
    }
    group_->SetPosition(0, 0);
    group_->Resize(VERTICAL_RESOLUTION, HORIZONTAL_RESOLUTION);
    group_->SetOnDragListener(this);
    group_->SetViewId("startPage");
    group_->SetDraggable(true);
    group_->SetTouchable(true);
    group_->SetOnDragListener(this);

    mainImg_ = new UIImageView();
    if (mainImg_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "HeartRateReadyPage::[OnStart] new UIImageView fail");
        return;
    }
    mainImg_->SetPosition(0, 0);
    LOADIMG::LoadImageViewImg(mainImg_, HEART_RATE_IMAGE, IMAGE_CONVERTER_TOOL_HEART_00037);
    mainImg_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    group_->Add(mainImg_);

    button_ = new UILabelButton();
    if (button_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "HeartRateReadyPage::[OnStart] new UILabelButton fail");
        return;
    }
    button_->SetPosition(HEARTRATE_BUTTON_POSITION_X, HEARTRATE_BUTTON_TWO_POSITION_Y);
    button_->Resize(HEARTRATE_BUTTON_WIDTH, HEARTRATE_BUTTON_HEIGHT);
    LOADIMG::LoadBtnImage(button_, HEART_RATE_IMAGE, IMAGE_CONVERTER_TOOL_HEART_00036,
                          IMAGE_CONVERTER_TOOL_HEART_00036);
    button_->SetViewId("start");
    button_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    button_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::PRESSED);
    button_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    button_->SetVisible(true);
    button_->SetOnClickListener(this);

    labelTipsStart_ = new UILabel();
    if (labelTipsStart_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "HeartRateReadyPage::[OnStart] new UILabel fail");
        return;
    }
    labelTipsStart_->SetPosition(HEARTRATE_LABLETIPSSATRT_POSITION_X, HEARTRATE_LABLETIPSSATRT_POSITION_Y,
                                 HEARTRATE_LABLETIPSSATRT_WIDTH, HEARTRATE_LABLETIPSSATRT_HEIGHT);
    labelTipsStart_->SetText("请佩戴手表测量");
    labelTipsStart_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, HEARTRATE_LABLETIPSSATRT_FONT_SIZE);
    labelTipsStart_->SetVisible(true);

    SetUpCircleProgress();

    SetupAnimator();

    group_->Add(button_);
    group_->Add(labelTipsStart_);
    AddViewToPageContainer(group_);
}

void HeartRateReadyPage::SetupAnimator()
{
    callBack_ = new HeartAnimatorCallback(this);
    animator_ = new Animator(callBack_, nullptr, 0, true);
}

int HeartRateReadyPage::SetUpCircleProgress()
{
    labelBackProgress_ = new UILabel();
    if (labelBackProgress_ == nullptr) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "HeartRateReadyPage::[SetUpCircleProgress] new UILabel fail");
        return OHOS_FAILURE;
    }
    labelBackProgress_->SetPosition(HEARTRATE_LABLEBACK_POSITION_X, HEARTRATE_LABLEBACK_POSITION_Y,
                                    HEARTRATE_LABLEBACK_WIDTH, HEARTRATE_LABLEBACK_HEIGHT);
    labelBackProgress_->SetText("3");
    labelBackProgress_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, HEARTRATE_LABLEBACK_FONT_SIZE);
    labelBackProgress_->SetVisible(true);
    labelBackProgress_->SetVisible(false);
    group_->Add(labelBackProgress_);

    playbackProgress_ = new UICircleProgress();
    if (playbackProgress_ == nullptr) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "HeartRateReadyPage::[SetUpCircleProgress] new UICircleProgress fail");
        return OHOS_FAILURE;
    }
    playbackProgress_->SetPosition(HEARTRATE_PLAYBACK_POSITION_X, HEARTRATE_PLAYBACK_POSITION_Y,
                                   HEARTRATE_PLAYBACK_WIDTH, HEARTRATE_PLAYBACK_HEIGHT);
    playbackProgress_->SetCenterPosition(HEARTRATE_PLAYBACK_VENTER_POSITION, HEARTRATE_PLAYBACK_VENTER_POSITION);
    playbackProgress_->SetRange(HEARTRATE_PLAYBACK_RANGE, 0);
    playbackProgress_->SetRadius(HEARTRATE_PLAYBACK_RADIUS);
    playbackProgress_->SetStartAngle(HEARTRATE_PLAYBACK_START_ANGLE);
    playbackProgress_->SetEndAngle(0);
    playbackProgress_->SetValue(0);
    playbackProgress_->SetForegroundStyle(STYLE_LINE_COLOR, 0xFF491010);
    playbackProgress_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    playbackProgress_->SetBackgroundStyle(STYLE_LINE_COLOR, 0xFFD72C2E);
    playbackProgress_->SetBackgroundStyle(STYLE_LINE_WIDTH, HEARTRATE_PLAYBACK_STYLE_LINE_WIDTH);
    playbackProgress_->SetForegroundStyle(STYLE_LINE_WIDTH, HEARTRATE_PLAYBACK_STYLE_LINE_WIDTH);
    playbackProgress_->SetVisible(false);
    group_->Add(playbackProgress_);
    return OHOS_SUCCESS;
}

void HeartRateReadyPage::HeartAnimatorCallback::Callback(UIView *view)
{
    (void)view;
    static int count = 0;
    struct timeval current_time;
    gettimeofday(&current_time, nullptr);
    long seconds = current_time.tv_sec - page_->start_time.tv_sec;
    long micros = current_time.tv_usec - page_->start_time.tv_usec;
    long time = (seconds * HEARTRATE_TIME_USEC) + (micros / HEARTRATE_TIME_USEC);

    page_->SetLabelProgressTime(time);
    if ((time / HEARTRATE_TIME_USEC) == HEARTRATE_TIME_NUM) {
        page_->ShowOnTest();
        page_->SetFirstFlag(true);
        page_->StopAnimator();
        NativeAbility::GetInstance().SwitchSlice(VIEW_HEARTRATE, HeartRateAllView::HEARTRATE_MEASUREMENT_PAGE);
    }
}

bool HeartRateReadyPage::GetFirstFlag()
{
    return isFirst;
}

void HeartRateReadyPage::SetFirstFlag(bool value)
{
    isFirst = value;
}

void HeartRateReadyPage::StopAnimator()
{
    animator_->Stop();
}

void HeartRateReadyPage::SetLabelProgressTime(int time)
{
    int progressTime = HEARTRATE_PROGRESS_TIME - (time / HEARTRATE_TIME_USEC);
    std::string strTime = std::to_string(progressTime);
    labelBackProgress_->SetText(strTime.c_str());
    double percentage = static_cast<double>(time) / (HEARTRATE_TIME_NUM * HEARTRATE_TIME_USEC);
    double value = round(percentage * 100000);
    playbackProgress_->SetValue(value);
}

void HeartRateReadyPage::ShowOnTest()
{
    button_->SetVisible(false);
    labelTipsStart_->SetVisible(false);
    playbackProgress_->SetVisible(false);
    labelBackProgress_->SetVisible(false);
}

bool HeartRateReadyPage::OnDrag(UIView &view, const DragEvent &event)
{
    UNUSED(view);
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        StopAnimator();
        SetFirstFlag(true);
        NativeAbility::GetInstance().SwitchSlice(VIEW_HEARTRATE, HeartRateAllView::HEARTRATE_MAIN_PAGE);
    }
    return false;
}

bool HeartRateReadyPage::OnClick(UIView &view, const ClickEvent &event)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "HeartRateReadyPage::OnClick ");
    UNUSED(event);
    if (view.GetViewId() == "start") {
        labelTipsStart_->SetVisible(false);
        labelTipsStart_->SetPosition(HEARTRATE_LABEL3_POSITION_X, HEARTRATE_LABEL3_POSITION_Y);
        labelTipsStart_->SetText("请屏幕朝上偏紧佩戴，                  保持静止");
        labelTipsStart_->SetVisible(true);
        button_->SetVisible(false);
        SetFirstFlag(true);
        animator_->Start();
        playbackProgress_->SetVisible(true);
        labelBackProgress_->SetVisible(true);
        gettimeofday(&start_time, nullptr);
    }
    return true;
}
}