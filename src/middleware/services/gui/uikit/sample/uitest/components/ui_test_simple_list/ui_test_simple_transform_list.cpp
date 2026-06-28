/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: List with built-in adapter sample
 * Author: Hisi Team
 * Created: 2025-07
 */

#include "ui_test_simple_transform_list.h"
#include "common/screen.h"
#include "components/ui_menu_item.h"
#include "components/ui_swipe_view_nested.h"
#include "components/ui_texture_mapper.h"
#include "components/ui_sweep_clock.h"
#include "components/ui_checkbox.h"
#include "components/ui_toggle_button.h"
#include "components/ui_label_button_ext.h"
#include "ui_test_list_fps/ui_list_fps_res.h"
#include "common/image_cache_manager.h"
#include "graphic_config.h"
#include "animator/animator_manager.h"
#include "sys/time.h"
namespace OHOS {
static constexpr int16_t DEFAULT_RADIUS = 10;
static constexpr int16_t LIST_WIDTH = 454;
static constexpr int16_t LIST_HEIGHT = 454;
static constexpr uint16_t ITEM_WIDTH = 400;
static constexpr uint16_t ITEM_HEIGHT = 100;
static constexpr uint16_t IMAGE_WIDTH = 50;
static constexpr uint16_t IMAGE_HEIGHT = 50;
static constexpr uint16_t LABEL_WIDTH = 200;
static constexpr uint16_t LABEL_HEIGHT = 100;
static constexpr uint16_t SCALE_FACTOR = 400;
static constexpr uint16_t IMAGE_NUM = 3;
static constexpr uint16_t POINT_POSITION_X = 202;
static constexpr uint16_t POINT_POSITION_Y = 16;
static constexpr uint16_t SECOND_POINT_POSITION_Y = 16;
static constexpr uint16_t POINT_CENTER_X = 25;
static constexpr uint16_t POINT_CENTER_Y = 211;
static constexpr uint16_t BIGCIRCLE_POSITION_X = 202;
static constexpr uint16_t BIGCIRCLE_POSITION_Y = 202;
static constexpr uint16_t MEDCIRCLE_POSITION_X = 202;
static constexpr uint16_t MEDCIRCLE_POSITION_Y = 202;
static constexpr uint16_t SMALLCIRCLE_POSITION_X = 202;
static constexpr uint16_t SMALLCIRCLE_POSITION_Y = 202;
constexpr const char *CLOCK_IMAGES = RES_PATH"/clock.bin";
const int CLOCK_MAIN_FACE_IMAGE = 0x100007;
const int CLOCK_SECONDHAND = 0x10000d;
const int CLOCK_MINUTEHAND = 0x100009;
const int CLOCK_HOURHAND = 0x100006;
const int CLOCK_BIGCIRCLE_IMAGE_PATH = 0x100001;
const int CLOCK_MEDCIRCLE_IMAGE_PATH = 0x100008;
const int CLOCK_CIRCLE_IMAGE_PATH = 0x100002;
// First type can't be 0
enum class ItemType {
    ITEM_TYPE_NONE,
    ITEM_TYPE_IMAGE_ANIMATOR_AND_SLIDER,
    ITEM_TYPE_IMAGE_AND_LABEL,
    ITEM_TYPE_IMAGE_AND_LABEL_AND_CHECKBOX,
    ITEM_TYPE_IMAGE_AND_LABEL_AND_TOGGLE_BT,
    ITEM_TYPE_IMAGE_AND_LABEL_BT,
    ITEM_TYPE_SWEEP_VIEW,
    ITEM_TYPE_TEXTURE_MAPPER,
    ITEM_TYPE_SWEEP_CLOCK_IMAGE,
    ITEM_TYPE_SWEEP_CLOCK_LINE,
};

struct ImageAndLabelRes {
    uint32_t resId;
    const char* txt;
};

struct ItemNode {
    ItemType type;
    void* data;
};

struct SwipeViewRes {
    uint32_t cardResID1;
    uint32_t cardResID2;
    uint32_t color;
};

static ImageAndLabelRes g_imageAndLabelData[] = {
    {APPLIST_ALARM, "闹钟"},
    {BLUETOOTH_TITLE, "蓝牙"},
    {APPLIST_CALLER_IMAGE, "通话记录"},
    {APPLIST_COMPASS_IMAGE, "指南针"},
    {APPLIST_SETTING, "设置"},
    {APPLIST_SPO2, "血氧饱和度"},
    {APPLIST_VOCASSIST_IMAGE, "语音助手"},
    {APPLIST_COMPASS_IMAGE, "超长文本................................."},
};

static SwipeViewRes g_swipeViewRes[] = {
    {APPLIST_ALARM, APPLIST_COMPASS_IMAGE, Color::Blue().full},
};

static ItemNode g_itemData[] = {
    {ItemType::ITEM_TYPE_IMAGE_AND_LABEL, &g_imageAndLabelData[1]},
    {ItemType::ITEM_TYPE_IMAGE_AND_LABEL, &g_imageAndLabelData[7]},
    {ItemType::ITEM_TYPE_IMAGE_AND_LABEL, &g_imageAndLabelData[3]},
    {ItemType::ITEM_TYPE_TEXTURE_MAPPER, &g_imageAndLabelData[0]},
    {ItemType::ITEM_TYPE_IMAGE_ANIMATOR_AND_SLIDER, nullptr},
    {ItemType::ITEM_TYPE_IMAGE_AND_LABEL, &g_imageAndLabelData[0]},
    {ItemType::ITEM_TYPE_SWEEP_CLOCK_IMAGE, nullptr},
    {ItemType::ITEM_TYPE_IMAGE_AND_LABEL, &g_imageAndLabelData[1]},
    {ItemType::ITEM_TYPE_IMAGE_AND_LABEL, &g_imageAndLabelData[2]},
    {ItemType::ITEM_TYPE_IMAGE_AND_LABEL, &g_imageAndLabelData[3]},
    {ItemType::ITEM_TYPE_SWEEP_VIEW, &g_swipeViewRes[0]},
    {ItemType::ITEM_TYPE_IMAGE_AND_LABEL_BT, &g_imageAndLabelData[4]},
    {ItemType::ITEM_TYPE_IMAGE_AND_LABEL_AND_CHECKBOX, &g_imageAndLabelData[6]},
    {ItemType::ITEM_TYPE_IMAGE_AND_LABEL_AND_TOGGLE_BT, &g_imageAndLabelData[5]},
};

static const char* g_imagePath[IMAGE_NUM] = {
    RES_PATH"A094_051_8888.bin",
    RES_PATH"A094_051_888.bin",
    RES_PATH"A094_051_565.bin",
};

static ImageAnimatorInfo g_binImageAnimatorInfo[IMAGE_NUM] = {{{0}}};
class ClockDialAnimatorCallback : public AnimatorCallback {
public:
    explicit ClockDialAnimatorCallback(UISweepClock *clock) : sweepClock(clock) {}
    ~ClockDialAnimatorCallback() override {}

    void Callback(UIView *view) override
    {
        struct timeval tv;
        gettimeofday(&tv, nullptr);
        uint64_t second = static_cast<uint64_t>(tv.tv_sec);
        uint64_t uSecond = static_cast<uint64_t>(tv.tv_usec);
        uint64_t minute = second / MIN_TO_HOUR;
        uint64_t hour = minute / MIN_TO_HOUR;
        uint8_t realHour = static_cast<uint8_t>(hour % HOURLY_SYSTEM);
        uint8_t realMinute = static_cast<uint8_t>(minute % MIN_TO_HOUR);
        uint16_t realMilliSecond = static_cast<uint16_t>((second % MIN_TO_HOUR) *
            SEC_TO_MILLI + uSecond / USEC_TO_MILLI);
        sweepClock->SetSweepTime12Hour(realHour, realMinute, realMilliSecond, 1);
    }

private:
    static constexpr int16_t SEC_TO_MILLI = 1000;
    static constexpr int16_t USEC_TO_MILLI = 1000;
    static constexpr int16_t MIN_TO_HOUR = 60;
    static constexpr int16_t HOURLY_SYSTEM = 24;
    static constexpr int16_t INTERVAL = 30;

    UISweepClock *sweepClock{nullptr};
};

class ItemContainer : public UITransformGroup {
public:
    explicit ItemContainer(ItemType type) : type_(type)
    {
        SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
        Resize(ITEM_WIDTH, ITEM_HEIGHT);
    }

    ~ItemContainer()
    {
        TearDown();
        RemoveAll();
    }

    void SetUp();
    void TearDown();
    void Update(void* data);
    UIImageAnimatorView* GetImageAnimatorView()
    {
        return imgAni_;
    }
private:
    void CreateSweepView();
    void DestroySweepView();
    void UpdateSweepView(SwipeViewRes* data);
    void UpdateImageAnimatorAndSlider();
    void UpdateImageAndLabel();
    void UpdateTextureMapper(ImageAndLabelRes* data);

    void CreateSweepClock();
    void FillSweppClock();
    void DestroySweepClock();
    void CreateImageAndLabel(ItemType type);
    bool LoadClockImage(Image *view, uint32_t resId);

    void CreateImageAndLabelBt();
    ItemType type_ = ItemType::ITEM_TYPE_NONE;
    UIImageView* imageView_ = nullptr;
    UILabelExt* label_ = nullptr;
    UILabelButtonExt* labelBt_ = nullptr;
    UICheckBox* checkbox_ = nullptr;

    UIImageAnimatorView* imgAni_ = nullptr;
    UISlider* slider_ = nullptr;

    UISwipeViewNested* swipeView_ = nullptr;
    UIImageView* card1_ = nullptr;
    UIImageView* card2_ = nullptr;
    UIViewGroup* card3_ = nullptr;

    UITextureMapper* textureMapper_ = nullptr;

    UISweepClock* sweepClock_ = nullptr;
    UIImageView* curFace_ = nullptr;
    Image* secondHand_ = nullptr;
    Image* minuteHand_ = nullptr;
    Image* hourHand_ = nullptr;
    Image* bigCircle_ = nullptr;
    Image* medCircle_ = nullptr;
    Image* smallCircle_ = nullptr;

    ClockDialAnimatorCallback *callBack = nullptr;
    Animator *clockAnimator = nullptr;
};

static bool TransformItemBaseOnPosition(UITransformList* list, UIView* item)
{
    if (list == nullptr || item == nullptr) {
        return false;
    }
    if (item->GetViewType() != UI_TRANSFORM_GROUP) {
        return false;
    }
    UITransformGroup* group = dynamic_cast<UITransformGroup*>(item);
    if (group == nullptr) {
        return false;
    }
    int16_t w = group->GetWidth();
    int16_t h = group->GetHeight();
    if (list->GetDirection() == UIList::VERTICAL) {
        int16_t midY = list->GetY() + (list->GetHeight() >> 1);
        int16_t y = group->GetY();
        int offset = (y + (h >> 1) - midY);
        int temp = EasingEquation::SineEaseIn(0, SCALE_FACTOR, abs(offset), SCALE_FACTOR);
        float scale = 1.0 - (static_cast<float>(temp) / SCALE_FACTOR);
        group->Scale(Vector2<float>(scale, scale), Vector2<float>(w >> 1,  h >> 1));
        if ((y <= midY) && (y + group->GetHeight() >= midY)) {
            return true;
        }
    }
    return false;
}

void ItemContainer::CreateSweepView()
{
    swipeView_ = new UISwipeViewNested(UISwipeView::HORIZONTAL);
    if (swipeView_ != nullptr) {
        swipeView_->SetPosition(50, 0, 200, 100); // x: 50 w: 200 h: 100
        swipeView_->SetBlankSize(0);
        Add(swipeView_);

        card1_ = new UIImageView();
        if (card1_ != nullptr) {
            card1_->SetPosition(0, 0, 200, 100); // w: 200 h: 100
            card1_->SetResizeMode(UIImageView::ImageResizeMode::CENTER);
            card1_->SetAutoEnable(false);
            swipeView_->Add(card1_);
        }
        card2_ = new UIImageView();
        if (card2_ != nullptr) {
            card2_->SetPosition(0, 0, 200, 100); // w: 200 h: 100
            card2_->SetResizeMode(UIImageView::ImageResizeMode::CENTER);
            card2_->SetAutoEnable(false);
            swipeView_->Add(card2_);
        }
        card3_ = new UIViewGroup();
        if (card3_ != nullptr) {
            card3_->SetPosition(0, 0, 200, 100); // w: 200 h: 100
            swipeView_->Add(card3_);
        }
    }
}

void ItemContainer::CreateSweepClock()
{
    sweepClock_ = new UISweepClock();
    if (sweepClock_ == nullptr) {
        return;
    }

    curFace_ = new UIImageView();
    if (curFace_ == nullptr) {
        DestroySweepClock();
        return;
    }

    std::string file = CLOCK_IMAGES;
    ImageInfo* image = ImageCacheManager::GetInstance().LoadOneInMultiRes(CLOCK_MAIN_FACE_IMAGE, file);
    if (image == nullptr) {
        DestroySweepClock();
        return;
    }
    curFace_->SetSrc(image);

    secondHand_ = new Image();
    if (secondHand_ == nullptr || !LoadClockImage(secondHand_, CLOCK_SECONDHAND)) {
        DestroySweepClock();
        return;
    }

    minuteHand_ = new Image();
    if (minuteHand_ == nullptr || !LoadClockImage(minuteHand_, CLOCK_MINUTEHAND)) {
        DestroySweepClock();
        return;
    }

    hourHand_ = new Image();
    if (hourHand_ == nullptr || !LoadClockImage(hourHand_, CLOCK_HOURHAND)) {
        DestroySweepClock();
        return;
    }

    bigCircle_ = new Image();
    if (bigCircle_ == nullptr || !LoadClockImage(bigCircle_, CLOCK_BIGCIRCLE_IMAGE_PATH)) {
        DestroySweepClock();
        return;
    }

    medCircle_ = new Image();
    if (medCircle_ == nullptr || !LoadClockImage(medCircle_, CLOCK_MEDCIRCLE_IMAGE_PATH)) {
        DestroySweepClock();
        return;
    }

    smallCircle_ = new Image();
    if (smallCircle_ == nullptr || !LoadClockImage(smallCircle_, CLOCK_CIRCLE_IMAGE_PATH)) {
        DestroySweepClock();
        return;
    }
    FillSweppClock();
}

void ItemContainer::FillSweppClock()
{
    curFace_->SetAutoEnable(false);
    curFace_->SetPosition(0, 0, 454, 454); // w: 454 h:454
    this->Resize(454, 454);  // w: 454 h:454
    sweepClock_->SetPosition(0, 0, 454, 454); // w: 454 h:454
    sweepClock_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Yellow().full);
    sweepClock_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    sweepClock_->SetHandImage(UISweepClock::HandType::SECOND_HAND, *secondHand_,
        { POINT_POSITION_X, SECOND_POINT_POSITION_Y }, { POINT_CENTER_X, POINT_CENTER_Y });
    sweepClock_->SetHandImage(UISweepClock::HandType::MINUTE_HAND, *minuteHand_,
        { POINT_POSITION_X, POINT_POSITION_Y }, { POINT_CENTER_X, POINT_CENTER_Y });
    sweepClock_->SetHandImage(UISweepClock::HandType::HOUR_HAND, *hourHand_, { POINT_POSITION_X, POINT_POSITION_Y },
        { POINT_CENTER_X, POINT_CENTER_Y });
    sweepClock_->SetHandCircleImage(UISweepClock::HandType::SECOND_HAND, *smallCircle_,
        { SMALLCIRCLE_POSITION_X, SMALLCIRCLE_POSITION_Y });
    sweepClock_->SetHandCircleImage(UISweepClock::HandType::MINUTE_HAND, *medCircle_,
        { MEDCIRCLE_POSITION_X, MEDCIRCLE_POSITION_Y });
    sweepClock_->SetHandCircleImage(UISweepClock::HandType::HOUR_HAND, *bigCircle_,
        { BIGCIRCLE_POSITION_X, BIGCIRCLE_POSITION_Y });
    sweepClock_->SetInitTime24Hour(0, 0, 0);
    sweepClock_->SetWorkMode(UISweepClock::WorkMode::NORMAL);
    sweepClock_->Add(curFace_);
    callBack = new ClockDialAnimatorCallback(sweepClock_);
    if (callBack == nullptr) {
        DestroySweepClock();
        return;
    }
    clockAnimator = new Animator(callBack, sweepClock_, 0, true);
    if (clockAnimator == nullptr) {
        DestroySweepClock();
        return;
    }
    clockAnimator->Start();
    Add(sweepClock_);
}

void ItemContainer::CreateImageAndLabel(ItemType type)
{
    imageView_ = new UIImageView();
    if (imageView_ != nullptr) {
        imageView_->SetPosition(0, 25, IMAGE_WIDTH, IMAGE_HEIGHT); // 25:image start pos y
        imageView_->SetResizeMode(UIImageView::ImageResizeMode::CENTER);
        Add(imageView_);
    }

    label_ = new UILabelExt();
    if (label_ != nullptr) {
        label_->SetPosition(IMAGE_WIDTH, 0, LABEL_WIDTH, LABEL_HEIGHT);
        label_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
        label_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 40);  // 40: item font size
        label_->SetLineBreakMode(UILabel::LINE_BREAK_MARQUEE);
        Add(label_);
    }

    if (type == ItemType::ITEM_TYPE_IMAGE_AND_LABEL_AND_CHECKBOX) {
        checkbox_ = new UICheckBox();
        checkbox_->SetPosition(IMAGE_WIDTH + LABEL_WIDTH, 20, 100, 60); // y: 20, w: 100 h: 60
        checkbox_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
        checkbox_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
        Add(checkbox_);
    } else if (type == ItemType::ITEM_TYPE_IMAGE_AND_LABEL_AND_TOGGLE_BT) {
        checkbox_ = new UIToggleButton();
        checkbox_->SetPosition(IMAGE_WIDTH + LABEL_WIDTH, 20, 100, 60); // y: 20, w: 100 h: 60
        checkbox_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
        checkbox_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
        Add(checkbox_);
    }
}

void ItemContainer::CreateImageAndLabelBt()
{
    imageView_ = new UIImageView();
    if (imageView_ != nullptr) {
        imageView_->SetPosition(0, 25, IMAGE_WIDTH, IMAGE_HEIGHT); // 25:image start pos y
        imageView_->SetResizeMode(UIImageView::ImageResizeMode::CENTER);
        Add(imageView_);
    }

    labelBt_ = new UILabelButtonExt();
    if (labelBt_ != nullptr) {
        labelBt_->SetPosition(IMAGE_WIDTH, 0, LABEL_WIDTH, LABEL_HEIGHT);
        labelBt_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
        labelBt_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 40);  // 40: item font size
        Add(labelBt_);
    }
}

void ItemContainer::DestroySweepView()
{
    if (swipeView_ == nullptr) {
        return;
    }
    swipeView_->RemoveAll();
    if (card1_ != nullptr) {
        delete card1_;
        card1_ = nullptr;
    }

    if (card2_ != nullptr) {
        delete card2_;
        card2_ = nullptr;
    }

    if (card3_ != nullptr) {
        delete card3_;
        card3_ = nullptr;
    }
    delete swipeView_;
    swipeView_ = nullptr;
}

void ItemContainer::DestroySweepClock()
{
    if (curFace_ != nullptr) {
        if (sweepClock_ != nullptr) {
            sweepClock_->Remove(curFace_);
        }
        delete curFace_;
        curFace_ = nullptr;
    }

    if (secondHand_ != nullptr) {
        delete secondHand_;
        secondHand_ = nullptr;
    }
    if (minuteHand_ != nullptr) {
        delete minuteHand_;
        minuteHand_ = nullptr;
    }
    if (hourHand_ != nullptr) {
        delete hourHand_;
        hourHand_ = nullptr;
    }
    if (bigCircle_ != nullptr) {
        delete bigCircle_;
        bigCircle_ = nullptr;
    }
    if (medCircle_ != nullptr) {
        delete medCircle_;
        medCircle_ = nullptr;
    }
    if (smallCircle_ != nullptr) {
        delete smallCircle_;
        smallCircle_ = nullptr;
    }
    if (sweepClock_ != nullptr) {
        delete sweepClock_;
        sweepClock_ = nullptr;
    }

    if (callBack != nullptr) {
        delete callBack;
        callBack = nullptr;
    }
    if (clockAnimator != nullptr) {
        delete clockAnimator;
        clockAnimator = nullptr;
    }
}

void ItemContainer::UpdateSweepView(SwipeViewRes* data)
{
    if (swipeView_ == nullptr) {
        return;
    }
    if (card1_ != nullptr) {
        ImageInfo* info = ImageCacheManager::GetInstance().LoadOneInMultiRes(data->cardResID1, LIST_FPS);
        if (info != nullptr) {
            card1_->SetSrc(info);
        }
    }
    if (card2_ != nullptr) {
        ImageInfo* info = ImageCacheManager::GetInstance().LoadOneInMultiRes(data->cardResID2, LIST_FPS);
        if (info != nullptr) {
            card2_->SetSrc(info);
        }
    }
    if (card3_ != nullptr) {
        card3_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
        card3_->SetStyle(STYLE_BACKGROUND_COLOR, data->color);
    }
}

bool ItemContainer::LoadClockImage(Image *view, uint32_t resId)
{
    std::string file = CLOCK_IMAGES;
    ImageInfo* image = ImageCacheManager::GetInstance().LoadOneInMultiRes(resId, file);
    if (image == nullptr) {
        GRAPHIC_LOGE("LoadClockImage %x fail\n", resId);
        return false;
    }
    view->SetSrc(image);
    return true;
}

void ItemContainer::UpdateImageAnimatorAndSlider()
{
    if (imgAni_ != nullptr) {
        imgAni_->SetImageAnimatorSrc(g_binImageAnimatorInfo, IMAGE_NUM, 50); // 50: update time Interval
        imgAni_->SetSizeFixed(true);
        imgAni_->Start();
        imgAni_->Pause();
    }
    if (slider_ != nullptr) {
        slider_->SetValidHeight(40);           // 40: valid height;
        slider_->SetValue(20);                 // 20:  progress bar current value
        slider_->SetDirection(UISlider::Direction::DIR_LEFT_TO_RIGHT);
        slider_->SetSliderColor(Color::Silver(), Color::Blue());
        slider_->SetKnobColor(Color::Red());
        slider_->SetSliderRadius(DEFAULT_RADIUS, DEFAULT_RADIUS);
    }
}

void ItemContainer::UpdateTextureMapper(ImageAndLabelRes* data)
{
    if (textureMapper_ != nullptr) {
        ImageInfo* imageInfo = ImageCacheManager::GetInstance().LoadOneInMultiRes(data->resId, LIST_FPS);
        if (imageInfo != nullptr) {
            textureMapper_->SetSrc(imageInfo);
            textureMapper_->SetScaleStart(1);
            textureMapper_->SetScaleEnd(1);
            textureMapper_->SetDurationTime(1000); // 1000:duration time
            textureMapper_->SetEasingFunc(EasingEquation::LinearEaseNone);
            textureMapper_->SetRotateStart(0);
            textureMapper_->SetRotateEnd(360); // 360: rotate end
            textureMapper_->SetPivot(50, 50); // 50: rotate point
            textureMapper_->Start();
        }
    }
}

static void UpdateImage(UIImageView* imageView, uint32_t resId)
{
    if (imageView == nullptr) {
        return;
    }
    ImageInfo* imageInfo = ImageCacheManager::GetInstance().LoadOneInMultiRes(resId, LIST_FPS);
    if (imageInfo != nullptr) {
        imageView->SetSrc(imageInfo);
    }
}

void ItemContainer::SetUp()
{
    if (type_ == ItemType::ITEM_TYPE_IMAGE_ANIMATOR_AND_SLIDER) {
        imgAni_ = new UIImageAnimatorView();
        if (imgAni_ != nullptr) {
            imgAni_->SetPosition(0, 25, IMAGE_WIDTH, IMAGE_HEIGHT); // 25:image start pos y
            imgAni_->SetResizeMode(UIImageView::ImageResizeMode::CENTER);
            Add(imgAni_);
        }

        slider_ = new UISlider();
        if (slider_ != nullptr) {
            slider_->SetPosition(IMAGE_WIDTH, 0, LABEL_WIDTH, LABEL_HEIGHT);
            Add(slider_);
        }
    } else if (type_ == ItemType::ITEM_TYPE_IMAGE_AND_LABEL ||
        type_ == ItemType::ITEM_TYPE_IMAGE_AND_LABEL_AND_CHECKBOX ||
        type_ == ItemType::ITEM_TYPE_IMAGE_AND_LABEL_AND_TOGGLE_BT) {
        return CreateImageAndLabel(type_);
    } else if (type_ == ItemType::ITEM_TYPE_SWEEP_VIEW) {
        return CreateSweepView();
    } else if (type_ == ItemType::ITEM_TYPE_TEXTURE_MAPPER) {
        textureMapper_ = new UITextureMapper();
        if (textureMapper_ != nullptr) {
            textureMapper_->SetPosition(100, 0, 100, 100); // 100: x, w, h
            Add(textureMapper_);
        }
    } else if (type_ == ItemType::ITEM_TYPE_SWEEP_CLOCK_IMAGE) {
        return CreateSweepClock();
    } else if (type_ == ItemType::ITEM_TYPE_IMAGE_AND_LABEL_BT) {
        return CreateImageAndLabelBt();
    }
}

void ItemContainer::TearDown()
{
    RemoveAll();
    if (type_ == ItemType::ITEM_TYPE_IMAGE_ANIMATOR_AND_SLIDER) {
        if (imgAni_ != nullptr) {
            delete imgAni_;
            imgAni_ = nullptr;
        }
        if (slider_ != nullptr) {
            delete slider_;
            slider_ = nullptr;
        }
    } else if (type_ == ItemType::ITEM_TYPE_IMAGE_AND_LABEL ||
        type_ == ItemType::ITEM_TYPE_IMAGE_AND_LABEL_AND_CHECKBOX ||
        type_ == ItemType::ITEM_TYPE_IMAGE_AND_LABEL_AND_TOGGLE_BT) {
        if (imageView_ != nullptr) {
            delete imageView_;
            imageView_ = nullptr;
        }
        if (label_ != nullptr) {
            delete label_;
            label_ = nullptr;
        }
        if (checkbox_ != nullptr) {
            delete checkbox_;
            checkbox_ = nullptr;
        }
    } else if (type_ == ItemType::ITEM_TYPE_SWEEP_VIEW) {
        return DestroySweepView();
    } else if (type_ == ItemType::ITEM_TYPE_TEXTURE_MAPPER) {
        if (textureMapper_ != nullptr) {
            delete textureMapper_;
            textureMapper_ = nullptr;
        }
    } else if (type_ == ItemType::ITEM_TYPE_SWEEP_CLOCK_IMAGE) {
        return DestroySweepClock();
    }
}

void ItemContainer::Update(void* data)
{
    if (type_ == ItemType::ITEM_TYPE_IMAGE_ANIMATOR_AND_SLIDER) {
        return UpdateImageAnimatorAndSlider();
    } else if (type_ == ItemType::ITEM_TYPE_IMAGE_AND_LABEL ||
        type_ == ItemType::ITEM_TYPE_IMAGE_AND_LABEL_AND_CHECKBOX ||
        type_ == ItemType::ITEM_TYPE_IMAGE_AND_LABEL_AND_TOGGLE_BT) {
        ImageAndLabelRes* resData = static_cast<ImageAndLabelRes*>(data);
        if (resData == nullptr) {
            return;
        }
        UpdateImage(imageView_, resData->resId);
        if (label_ != nullptr) {
            label_->SetText(resData->txt);
            label_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
        }
    } else if (type_ == ItemType::ITEM_TYPE_SWEEP_VIEW) {
        return UpdateSweepView(static_cast<SwipeViewRes*>(data));
    } else if (type_ == ItemType::ITEM_TYPE_TEXTURE_MAPPER) {
        return UpdateTextureMapper(static_cast<ImageAndLabelRes*>(data));
    } else if (type_ == ItemType::ITEM_TYPE_IMAGE_AND_LABEL_BT) {
        ImageAndLabelRes* resData = static_cast<ImageAndLabelRes*>(data);
        if (resData == nullptr) {
            return;
        }
        UpdateImage(imageView_, resData->resId);
        if (labelBt_ != nullptr) {
            labelBt_->SetText(resData->txt);
        }
    }
}

static UIView* CreateViewCb(uint8_t type)
{
    ItemContainer* item = new ItemContainer(static_cast<ItemType>(type));
    if (item == nullptr) {
        return nullptr;
    }
    item->SetUp();
    return item;
}

static void UpdateViewCb(UIView* view, void* data, uint8_t type)
{
    ItemContainer* item = dynamic_cast<ItemContainer*>(view);
    if (item == nullptr) {
        return;
    }
    item->Update(data);
}

void UITestSimpleTransformList::OnItemSelected(int16_t index, UIView* view)
{
    if (view == nullptr) {
        return;
    }
    if (lastSelectedView_ != nullptr) {
        if (lastSelectedView_ != view) {
            ItemContainer* selectedItem = dynamic_cast<ItemContainer *>(lastSelectedView_);
            if (selectedItem->GetImageAnimatorView() != nullptr) {
                selectedItem->GetImageAnimatorView()->Pause();
            }
            lastSelectedView_ = nullptr;
        }
    }
    ItemContainer* item = dynamic_cast<ItemContainer *>(view);
    if (item == nullptr) {
        GRAPHIC_LOGE("View is not ItemContainer!");
        return;
    }
    if (item->GetImageAnimatorView() != nullptr) {
        item->GetImageAnimatorView()->Resume();
        lastSelectedView_ = view;
    }
    return;
}

void UITestSimpleTransformList::SetUp()
{
    LoadImg();
    if (list_ == nullptr) {
        list_ = new UISimpleTransformList();
    }
    list_->SetPosition(0, 0, 454, 454); // w,h: 454
    list_->SetThrowDrag(true);
    list_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    list_->SetScrollBlankSize(LIST_HEIGHT / 2); // 2: divisor
    list_->SetItemTransformFunc(&TransformItemBaseOnPosition);
    list_->SetScrollStateListener(this);
    list_->SetSelectPosition(227); // 227 : middle of screen

    Contents content = {};
    for (uint8_t i = 0; i < (sizeof(g_itemData) / sizeof(ItemNode)); ++i) {
        content.createFunc = CreateViewCb;
        content.updateFunc = UpdateViewCb;
        content.type = static_cast<uint8_t>(g_itemData[i].type);
        content.data = reinterpret_cast<void *>(g_itemData[i].data);
        list_->AddContent(content);
    }
    list_->RefreshList();
}

void UITestSimpleTransformList::TearDown()
{
    ReleaseImg();
    if (list_ != nullptr) {
        delete list_;
        list_ = nullptr;
    }

    if (lastSelectedView_ != nullptr) {
        lastSelectedView_ = nullptr;
    }
}

UIView* UITestSimpleTransformList::GetTestView()
{
    return list_;
}

void UITestSimpleTransformList::LoadImg()
{
    ImageAnimatorInfo baseinfo = {nullptr, {0, 0}, IMAGE_WIDTH, IMAGE_HEIGHT, IMG_SRC_IMAGE_INFO};
    for (int i = 0; i < IMAGE_NUM; i++) {
        ImageInfo* img = ImageCacheManager::GetInstance().LoadSingleRes(g_imagePath[i]);
        if (img == nullptr) {
            GRAPHIC_LOGE("Load image failed: %s", g_imagePath[i]);
            ReleaseImg();
            return;
        }
        g_binImageAnimatorInfo[i] = baseinfo;
        g_binImageAnimatorInfo[i].imageInfo = img;
    }
}

void UITestSimpleTransformList::ReleaseImg()
{
    for (int i = 0; i < IMAGE_NUM; i++) {
        if (g_binImageAnimatorInfo[i].imageInfo != nullptr) {
            ImageCacheManager::GetInstance().UnloadSingleRes(g_imagePath[i]);
            g_binImageAnimatorInfo[i].imageInfo = nullptr;
        }
    }
}
} // namespace OHOS
