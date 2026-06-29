/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: ShineOnClickView
 * Create: 2025-11-18
 */

#include "common/image_cache_manager.h"
#include "common/screen.h"
#include "shine_on_click_view.h"

namespace OHOS {
constexpr int16_t DISTURB_IMG_X = 117;
constexpr int16_t DISTURB_IMG_Y = 50;
constexpr int16_t SCREEN_IMG_X = 251;
constexpr int16_t SCREEN_IMG_Y = 50;
constexpr int16_t PHONE_IMG_X = 49;
constexpr int16_t PHONE_IMG_Y = 184;
constexpr int16_t ALARM_IMG_X = 184;
constexpr int16_t ALARM_IMG_Y = 184;
constexpr int16_t SETTING_IMG_X = 319;
constexpr int16_t SETTING_IMG_Y = 184;

constexpr int16_t DISTURB_LABEL_X = 136;
constexpr int16_t DISTURB_LABEL_Y = 145;
constexpr int16_t SCREEN_LABEL_X = 270;
constexpr int16_t SCREEN_LABEL_Y = 145;
constexpr int16_t PHONE_LABEL_X = 68;
constexpr int16_t PHONE_LABEL_Y = 279;
constexpr int16_t ALARM_LABEL_X = 203;
constexpr int16_t ALARM_LABEL_Y = 279;
constexpr int16_t SETTING_LABEL_X = 338;
constexpr int16_t SETTING_LABEL_Y = 279;

constexpr int16_t FONT_DEFAULT_SIZE = 24;
constexpr int16_t TEXT_WIDTH = 86;
constexpr int16_t TEXT_HEIGHT = 30;

static constexpr float DIVISOR_2 = 2.0;
static constexpr float MIN_SCALE = 0.7;

static constexpr const char *DROPDOWN_IAMGES = RES_PATH"dropdown/dropdown.bin";
static constexpr const char *DROPDOWN_MASK_IAMGE_PREFIX = RES_PATH"dropdown/dropdown_mask";

static const int DROPDOWN_DROPDOWN_DN_DISTURB = 0x400002;
static const int DROPDOWN_DROPDOWN_ALARM = 0x400004;
static const int DROPDOWN_DROPDOWN_BRI_SCREEN = 0x400005;
static const int DROPDOWN_DROPDOWN_SETTING = 0x400008;
static const int DROPDOWN_DROPDOWN_FIND_PHONE = 0x40000a;

ShineOnClickView::~ShineOnClickView()
{
    Remove(container_);
    container_->Remove(shineMask_);
    DeinitShineAnimator();
    DeinitShineMask();
    RemoveAndDeleteAllRecursively(container_);
}

void ShineOnClickView::InitViewImg(int16_t x, int16_t y, uint32_t resId, const char* viewId)
{
    UIImageView *imageView = new UIImageView();
    if (imageView == nullptr) {
        return;
    }
    ImageInfo* image = ImageCacheManager::GetInstance().LoadOneInMultiRes(resId, DROPDOWN_IAMGES);
    if (image == nullptr) {
        delete imageView;
        return;
    }
    imageView->SetSrc(image);
    imageView->SetPosition(x, y);
    imageView->SetViewId(viewId);
    imageView->SetTouchable(true);
    imageView->SetOnClickListener(this);
    container_->Add(imageView);
}

void ShineOnClickView::InitViewImg(void)
{
    InitViewImg(DISTURB_IMG_X, DISTURB_IMG_Y, DROPDOWN_DROPDOWN_DN_DISTURB, "0"); // 0: 勿扰
    InitViewImg(SCREEN_IMG_X, SCREEN_IMG_Y, DROPDOWN_DROPDOWN_BRI_SCREEN, "1"); // 1: 亮屏
    InitViewImg(PHONE_IMG_X, PHONE_IMG_Y, DROPDOWN_DROPDOWN_FIND_PHONE, "2"); // 2: 找手机
    InitViewImg(ALARM_IMG_X, ALARM_IMG_Y, DROPDOWN_DROPDOWN_ALARM, "3"); // 3: 闹钟
    InitViewImg(SETTING_IMG_X, SETTING_IMG_Y, DROPDOWN_DROPDOWN_SETTING, "4"); // 4: 设置
}

void ShineOnClickView::InitViewLabel(const Rect& rect, const char* text)
{
    UILabel *label = new UILabel();
    if (label == nullptr) {
        return;
    }
    label->SetPosition(rect.GetLeft(), rect.GetTop(), rect.GetWidth(), rect.GetHeight());
    label->SetText(text);
    label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    label->SetStyle(STYLE_BACKGROUND_COLOR, OPA_TRANSPARENT);
    container_->Add(label);
}

void ShineOnClickView::InitViewLabel(void)
{
    InitViewLabel(Rect(DISTURB_LABEL_X, DISTURB_LABEL_Y, TEXT_WIDTH, TEXT_HEIGHT), "勿扰");
    InitViewLabel(Rect(SCREEN_LABEL_X, SCREEN_LABEL_Y, TEXT_WIDTH, TEXT_HEIGHT), "亮屏");
    InitViewLabel(Rect(PHONE_LABEL_X, PHONE_LABEL_Y, TEXT_WIDTH, TEXT_HEIGHT), "找手机");
    InitViewLabel(Rect(ALARM_LABEL_X, ALARM_LABEL_Y, TEXT_WIDTH, TEXT_HEIGHT), "闹钟");
    InitViewLabel(Rect(SETTING_LABEL_X, SETTING_LABEL_Y, TEXT_WIDTH, TEXT_HEIGHT), "设置");
}

bool ShineOnClickView::InitView()
{
    container_ = new UIViewGroup();
    if (container_ == nullptr) {
        return false;
    }
    container_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    container_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    Add(container_);
    InitViewImg();
    InitViewLabel();
    InitShineMask();
    InitShineAnimator();

    SetTouchable(true);
    SetIntercept(true);

    return true;
}

void ShineOnClickView::InitShineMask()
{
    if (shineMask_ == nullptr) {
        shineMask_ = new UIImageView();
        if (shineMask_ == nullptr) {
            return;
        }
        for (int16_t i = 0; i < BUTTON_COUNT; i++) {
            shineMaskSrc_[i] =
            ImageCacheManager::GetInstance().LoadSingleRes(std::string(DROPDOWN_MASK_IAMGE_PREFIX) +
                std::to_string(i) + std::string(".bin"));
        }
        shineMask_->SetSrc(shineMaskSrc_[0]);
        shineMask_->SetStyle(STYLE_BACKGROUND_COLOR, OPA_TRANSPARENT);
    }
}

void ShineOnClickView::DeinitShineMask()
{
    if (shineMask_ != nullptr) {
        delete shineMask_;
        shineMask_ = nullptr;
        for (int16_t i = 0; i < BUTTON_COUNT; i++) {
            ImageCacheManager::GetInstance().UnloadSingleRes(std::string(std::string(DROPDOWN_MASK_IAMGE_PREFIX) +
                std::to_string(i) + std::string(".bin")));
        }
    }
}

void ShineOnClickView::InitShineAnimator()
{
    if (shineCallBack_ == nullptr) {
        shineCallBack_ = new ShineAnimatorCallback();
        if (shineCallBack_ == nullptr) {
            return;
        }
        shineAnimator_ = new Animator(shineCallBack_, this, SHINE_ANIMATE_TIME, false);
        if (shineAnimator_ == nullptr) {
            delete shineCallBack_;
            shineCallBack_ = nullptr;
        }
    }
}

void ShineOnClickView::DeinitShineAnimator()
{
    if (shineAnimator_ != nullptr) {
        delete shineAnimator_;
        shineAnimator_ = nullptr;
    }
    if (shineCallBack_ != nullptr) {
        delete shineCallBack_;
        shineCallBack_ = nullptr;
    }
}

bool ShineOnClickView::OnClick(UIView &view, const ClickEvent &event)
{
    (void)event;
    if (shineAnimator_ == nullptr) {
        return false;
    }
    if (shineAnimator_->GetState() != Animator::STOP) {
        return true;
    }

    ImageInfo *shineMaskSrc = nullptr;
    std::string curViewId = view.GetViewId();
    for (int16_t i = 0; i < BUTTON_COUNT; i++) {
        if (curViewId == std::to_string(i)) {
            if (shineMaskSrc_[i] == nullptr) {
                return false;
            }
            shineMaskSrc = shineMaskSrc_[i];
            break;
        }
    }
    if (shineMaskSrc == nullptr) {
        return false;
    }

    int16_t curViewCenterX = view.GetX() + static_cast<uint16_t>(view.GetWidth() / DIVISOR_2);
    int16_t curViewCenterY = view.GetY() + static_cast<uint16_t>(view.GetHeight() / DIVISOR_2);
    int16_t shineMaskX = curViewCenterX - static_cast<uint16_t>(shineMaskSrc->header.width / DIVISOR_2);
    int16_t shineMaskY = curViewCenterY - static_cast<uint16_t>(shineMaskSrc->header.height / DIVISOR_2);
    shineMask_->SetSrc(shineMaskSrc);
    shineMask_->SetPosition(shineMaskX, shineMaskY);
    container_->Add(shineMask_);

    clickView_ = &view;
    shineAnimator_->Start();
    return true;
}

void ShineOnClickView::ShineOnClick(uint32_t elapseTime)
{
    uint8_t opaque = 0;
    float scale = 0.0;
    if (elapseTime <= SHINE_ANIMATE_TIME / DIVISOR_2) {
        opaque = static_cast<uint8_t>(OPA_OPAQUE * elapseTime * DIVISOR_2 / SHINE_ANIMATE_TIME);
        scale = 1 - (1 - MIN_SCALE) * elapseTime * DIVISOR_2 / SHINE_ANIMATE_TIME;
    } else if (elapseTime <= SHINE_ANIMATE_TIME) {
        opaque = static_cast<uint8_t>(OPA_OPAQUE * (SHINE_ANIMATE_TIME - elapseTime) * DIVISOR_2 / SHINE_ANIMATE_TIME);
        scale = 1 - (1 - MIN_SCALE) * (SHINE_ANIMATE_TIME - elapseTime) * DIVISOR_2 / SHINE_ANIMATE_TIME;
    } else {
        return;
    }

    shineMask_->SetOpaScale(opaque);
    clickView_->Scale({scale, scale}, {clickView_->GetWidth() / DIVISOR_2, clickView_->GetHeight() / DIVISOR_2});
    Invalidate();
}

void ShineOnClickView::ShineEnd()
{
    clickView_->Scale({1.0, 1.0}, {clickView_->GetWidth() / DIVISOR_2, clickView_->GetHeight() / DIVISOR_2});
    container_->Remove(shineMask_);
    Invalidate();
}

void ShineAnimatorCallback::Callback(UIView *view)
{
    uint32_t elapseTime = 0;
    if (startTime_ == 0) {
        startTime_ = HALTick::GetInstance().GetTime();
    } else {
        elapseTime = HALTick::GetInstance().GetElapseTime(startTime_);
    }
    ShineOnClickView* shineOnClickView = dynamic_cast<ShineOnClickView*>(view);
    shineOnClickView->ShineOnClick(elapseTime);
}

void ShineAnimatorCallback::OnStop(UIView& view)
{
    ShineOnClickView* shineOnClickView = dynamic_cast<ShineOnClickView*>(&view);
    shineOnClickView->ShineEnd();
    startTime_ = 0;
}
}