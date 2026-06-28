/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: FlashLightColorPage
 * Create: 2025-04-24
 */

#include "SlicePageFactory.h"
#include "AppViewIDs.h"
#include "ui_view_ids_flashlight.h"
#include "wearable_log.h"
#include "NativeAbility.h"
#include "main/LoadImg.h"
#include "ui_resource_flashlight.h"
#include "flashlight/FlashLightView.h"
#include "flashlight/FlashLightPresenter.h"
#include "flashlight/FlashLightModel.h"
#include "flashlight/FlashLightColorPage.h"

namespace OHOS {
static constexpr uint16_t COLOR_LABEL_POS_X = 100;
static constexpr uint16_t COLOR_LABEL_POS_Y = 38;
static constexpr uint16_t COLOR_LABEL_WIDTH = 253;
static constexpr uint16_t COLOR_LABEL_HEIGHT = 53;
static constexpr uint16_t COLOR_LABEL_FONT_SIZE = 38;
static constexpr uint16_t CONTAINER_BLANK_SIZE = 100;
// 行坐标
static constexpr uint16_t FIRST_ROW_COLOR_BUTTON_POS_Y = 104;
static constexpr uint16_t SECOND_ROW_COLOR_BUTTON_POS_Y = 225;
static constexpr uint16_t THIRD_ROW_COLOR_BUTTON_POS_Y = 346;
// 列坐标
static constexpr uint16_t FIRST_COLUMN_COLOR_BUTTON_POS_X = 46;
static constexpr uint16_t SECOND_COLUMN_COLOR_BUTTON_POS_X = 169;
static constexpr uint16_t THIRD_COLUMN_COLOR_BUTTON_POS_X = 290;
// 按钮大小
static constexpr uint16_t COLOR_BUTTON_WIDTH = 117;
static constexpr uint16_t COLOR_BUTTON_HEIGHT = 117;
// 选中按钮偏移值
static constexpr uint16_t COLOR_BUTTON_OFFSET = 8;
static constexpr uint32_t g_selectColorButtonRes[] = {
    0,
    IMAGE_FLASHLIGHT_COLOR_WHITE_PRESSBUTTON,
    IMAGE_FLASHLIGHT_COLOR_RED_PRESSBUTTON,
    IMAGE_FLASHLIGHT_COLOR_ORANGE_PRESSBUTTON,
    IMAGE_FLASHLIGHT_COLOR_YELLOW_PRESSBUTTON,
    IMAGE_FLASHLIGHT_COLOR_GREEN_PRESSBUTTON,
    IMAGE_FLASHLIGHT_COLOR_LIGHT_BLUE_PRESSBUTTON,
    IMAGE_FLASHLIGHT_COLOR_DARK_BLUE_PRESSBUTTON,
    IMAGE_FLASHLIGHT_COLOR_PURPLE_PRESSBUTTON,
    IMAGE_FLASHLIGHT_COLOR_MULTI_PRESSBUTTON,
};
static uint32_t g_colorButtonRes[] = {
    0,
    IMAGE_FLASHLIGHT_COLOR_WHITE_BUTTON,
    IMAGE_FLASHLIGHT_COLOR_RED_BUTTON,
    IMAGE_FLASHLIGHT_COLOR_ORANGE_BUTTON,
    IMAGE_FLASHLIGHT_COLOR_YELLOW_BUTTON,
    IMAGE_FLASHLIGHT_COLOR_GREEN_BUTTON,
    IMAGE_FLASHLIGHT_COLOR_LIGHT_BLUE_BUTTON,
    IMAGE_FLASHLIGHT_COLOR_DARK_BLUE_BUTTON,
    IMAGE_FLASHLIGHT_COLOR_PURPLE_BUTTON,
    IMAGE_FLASHLIGHT_COLOR_MULTI_BUTTON,
};

REGIST_SLICE_PAGE(VIEW_FLASHLIGHT, FlashLightAllView::FLASH_LIGHT_COLOR_VIEW, FlashLightColorPage, false);

void FlashLightColorPage::InitWhiteColor(uint16_t selectColor)
{
    // 第一行颜色按钮
    colorWhiteButton_ = new UILabelButtonExt();
    if (colorWhiteButton_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "malloc fail, %s, %d\r\n", __func__, __LINE__);
        return;
    }
    if (selectColor == WHITE_COLOR) {
        colorWhiteButton_->SetPosition(FIRST_COLUMN_COLOR_BUTTON_POS_X - COLOR_BUTTON_OFFSET,
                                       FIRST_ROW_COLOR_BUTTON_POS_Y - COLOR_BUTTON_OFFSET, COLOR_BUTTON_WIDTH,
                                       COLOR_BUTTON_HEIGHT);
    } else {
        colorWhiteButton_->SetPosition(FIRST_COLUMN_COLOR_BUTTON_POS_X, FIRST_ROW_COLOR_BUTTON_POS_Y,
                                       COLOR_BUTTON_WIDTH, COLOR_BUTTON_HEIGHT);
    }
    colorWhiteButton_->SetViewId(UIVIEW_FLASHLIGHT_UILABELBUTTONEXT_WHITE);
    colorWhiteButton_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    colorWhiteButton_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::PRESSED);
    colorWhiteButton_->SetAlign(TEXT_ALIGNMENT_CENTER);
    LOADIMG::LoadBtnImage(colorWhiteButton_, FLASHLIGHT_IMAGE, g_colorButtonRes[WHITE_COLOR],
                          g_colorButtonRes[WHITE_COLOR]);
    container_->Add(colorWhiteButton_);
    colorWhiteButton_->SetVisible(true);
}

void FlashLightColorPage::InitRedColor(uint16_t selectColor)
{
    colorRedButton_ = new UILabelButtonExt();
    if (colorRedButton_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "malloc fail, %s, %d\r\n", __func__, __LINE__);
        return;
    }
    if (selectColor == RED_COLOR) {
        colorRedButton_->SetPosition(SECOND_COLUMN_COLOR_BUTTON_POS_X - COLOR_BUTTON_OFFSET,
                                     FIRST_ROW_COLOR_BUTTON_POS_Y - COLOR_BUTTON_OFFSET, COLOR_BUTTON_WIDTH,
                                     COLOR_BUTTON_HEIGHT);
    } else {
        colorRedButton_->SetPosition(SECOND_COLUMN_COLOR_BUTTON_POS_X, FIRST_ROW_COLOR_BUTTON_POS_Y, COLOR_BUTTON_WIDTH,
                                     COLOR_BUTTON_HEIGHT);
    }
    colorRedButton_->SetViewId(UIVIEW_FLASHLIGHT_UILABELBUTTONEXT_RED);
    colorRedButton_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    colorRedButton_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::PRESSED);
    colorRedButton_->SetAlign(TEXT_ALIGNMENT_CENTER);
    LOADIMG::LoadBtnImage(colorRedButton_, FLASHLIGHT_IMAGE, g_colorButtonRes[RED_COLOR], g_colorButtonRes[RED_COLOR]);
    container_->Add(colorRedButton_);
    colorRedButton_->SetVisible(true);
}

void FlashLightColorPage::InitOrangeColor(uint16_t selectColor)
{
    colorOrangeButton_ = new UILabelButtonExt();
    if (colorOrangeButton_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "malloc fail, %s, %d\r\n", __func__, __LINE__);
        return;
    }
    if (selectColor == ORANGE_COLOR) {
        colorOrangeButton_->SetPosition(THIRD_COLUMN_COLOR_BUTTON_POS_X - COLOR_BUTTON_OFFSET,
                                        FIRST_ROW_COLOR_BUTTON_POS_Y - COLOR_BUTTON_OFFSET, COLOR_BUTTON_WIDTH,
                                        COLOR_BUTTON_HEIGHT);
    } else {
        colorOrangeButton_->SetPosition(THIRD_COLUMN_COLOR_BUTTON_POS_X, FIRST_ROW_COLOR_BUTTON_POS_Y,
                                        COLOR_BUTTON_WIDTH, COLOR_BUTTON_HEIGHT);
    }
    colorOrangeButton_->SetViewId(UIVIEW_FLASHLIGHT_UILABELBUTTONEXT_ORANGE);
    colorOrangeButton_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    colorOrangeButton_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::PRESSED);
    colorOrangeButton_->SetAlign(TEXT_ALIGNMENT_CENTER);
    LOADIMG::LoadBtnImage(colorOrangeButton_, FLASHLIGHT_IMAGE, g_colorButtonRes[ORANGE_COLOR],
                          g_colorButtonRes[ORANGE_COLOR]);
    container_->Add(colorOrangeButton_);
    colorOrangeButton_->SetVisible(true);
}

void FlashLightColorPage::InitYellowColor(uint16_t selectColor)
{
    colorYellowButton_ = new UILabelButtonExt();
    if (colorYellowButton_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "malloc fail, %s, %d\r\n", __func__, __LINE__);
        return;
    }
    if (selectColor == YELLOW_COLOR) {
        colorYellowButton_->SetPosition(FIRST_COLUMN_COLOR_BUTTON_POS_X - COLOR_BUTTON_OFFSET,
                                        SECOND_ROW_COLOR_BUTTON_POS_Y - COLOR_BUTTON_OFFSET, COLOR_BUTTON_WIDTH,
                                        COLOR_BUTTON_HEIGHT);
    } else {
        colorYellowButton_->SetPosition(FIRST_COLUMN_COLOR_BUTTON_POS_X, SECOND_ROW_COLOR_BUTTON_POS_Y,
                                        COLOR_BUTTON_WIDTH, COLOR_BUTTON_HEIGHT);
    }
    colorYellowButton_->SetViewId(UIVIEW_FLASHLIGHT_UILABELBUTTONEXT_YELLOW);
    colorYellowButton_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    colorYellowButton_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::PRESSED);
    colorYellowButton_->SetAlign(TEXT_ALIGNMENT_CENTER);
    LOADIMG::LoadBtnImage(colorYellowButton_, FLASHLIGHT_IMAGE, g_colorButtonRes[YELLOW_COLOR],
                          g_colorButtonRes[YELLOW_COLOR]);
    container_->Add(colorYellowButton_);
    colorYellowButton_->SetVisible(true);
}

void FlashLightColorPage::InitGreenColor(uint16_t selectColor)
{
    colorGreenButton_ = new UILabelButtonExt();
    if (colorGreenButton_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "malloc fail, %s, %d\r\n", __func__, __LINE__);
        return;
    }
    if (selectColor == GREEN_COLOR) {
        colorGreenButton_->SetPosition(SECOND_COLUMN_COLOR_BUTTON_POS_X - COLOR_BUTTON_OFFSET,
                                       SECOND_ROW_COLOR_BUTTON_POS_Y - COLOR_BUTTON_OFFSET, COLOR_BUTTON_WIDTH,
                                       COLOR_BUTTON_HEIGHT);
    } else {
        colorGreenButton_->SetPosition(SECOND_COLUMN_COLOR_BUTTON_POS_X, SECOND_ROW_COLOR_BUTTON_POS_Y,
                                       COLOR_BUTTON_WIDTH, COLOR_BUTTON_HEIGHT);
    }

    colorGreenButton_->SetViewId(UIVIEW_FLASHLIGHT_UILABELBUTTONEXT_GREEN);
    colorGreenButton_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    colorGreenButton_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::PRESSED);
    colorGreenButton_->SetAlign(TEXT_ALIGNMENT_CENTER);
    LOADIMG::LoadBtnImage(colorGreenButton_, FLASHLIGHT_IMAGE, g_colorButtonRes[GREEN_COLOR],
                          g_colorButtonRes[GREEN_COLOR]);
    container_->Add(colorGreenButton_);
    colorGreenButton_->SetVisible(true);
}

void FlashLightColorPage::InitLightBlueColor(uint16_t selectColor)
{
    colorLightBlueButton_ = new UILabelButtonExt();
    if (colorLightBlueButton_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "malloc fail, %s, %d\r\n", __func__, __LINE__);
        return;
    }
    if (selectColor == LIGHT_BLUE_COLOR) {
        colorLightBlueButton_->SetPosition(THIRD_COLUMN_COLOR_BUTTON_POS_X - COLOR_BUTTON_OFFSET,
                                           SECOND_ROW_COLOR_BUTTON_POS_Y - COLOR_BUTTON_OFFSET, COLOR_BUTTON_WIDTH,
                                           COLOR_BUTTON_HEIGHT);
    } else {
        colorLightBlueButton_->SetPosition(THIRD_COLUMN_COLOR_BUTTON_POS_X, SECOND_ROW_COLOR_BUTTON_POS_Y,
                                           COLOR_BUTTON_WIDTH, COLOR_BUTTON_HEIGHT);
    }
    colorLightBlueButton_->SetViewId(UIVIEW_FLASHLIGHT_UILABELBUTTONEXT_LIGHTBLUE);
    colorLightBlueButton_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    colorLightBlueButton_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::PRESSED);
    colorLightBlueButton_->SetAlign(TEXT_ALIGNMENT_CENTER);
    LOADIMG::LoadBtnImage(colorLightBlueButton_, FLASHLIGHT_IMAGE, g_colorButtonRes[LIGHT_BLUE_COLOR],
                          g_colorButtonRes[LIGHT_BLUE_COLOR]);

    container_->Add(colorLightBlueButton_);
    colorLightBlueButton_->SetVisible(true);
}

void FlashLightColorPage::InitDarkBlueColor(uint16_t selectColor)
{
    colorDarkBlueButton_ = new UILabelButtonExt();
    if (colorDarkBlueButton_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "malloc fail, %s, %d\r\n", __func__, __LINE__);
        return;
    }
    if (selectColor == DARK_BLUE_COLOR) {
        colorDarkBlueButton_->SetPosition(FIRST_COLUMN_COLOR_BUTTON_POS_X - COLOR_BUTTON_OFFSET,
                                          THIRD_ROW_COLOR_BUTTON_POS_Y - COLOR_BUTTON_OFFSET, COLOR_BUTTON_WIDTH,
                                          COLOR_BUTTON_HEIGHT);
    } else {
        colorDarkBlueButton_->SetPosition(FIRST_COLUMN_COLOR_BUTTON_POS_X, THIRD_ROW_COLOR_BUTTON_POS_Y,
                                          COLOR_BUTTON_WIDTH, COLOR_BUTTON_HEIGHT);
    }
    colorDarkBlueButton_->SetViewId(UIVIEW_FLASHLIGHT_UILABELBUTTONEXT_DARKBLUE);
    colorDarkBlueButton_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    colorDarkBlueButton_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::PRESSED);
    colorDarkBlueButton_->SetAlign(TEXT_ALIGNMENT_CENTER);
    LOADIMG::LoadBtnImage(colorDarkBlueButton_, FLASHLIGHT_IMAGE, g_colorButtonRes[DARK_BLUE_COLOR],
                          g_colorButtonRes[DARK_BLUE_COLOR]);
    container_->Add(colorDarkBlueButton_);
    colorDarkBlueButton_->SetVisible(true);
}

void FlashLightColorPage::InitPurpleColor(uint16_t selectColor)
{
    colorPurpleButton_ = new UILabelButtonExt();
    if (colorPurpleButton_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "malloc fail, %s, %d\r\n", __func__, __LINE__);
        return;
    }
    if (selectColor == PURPLE_COLOR) {
        colorPurpleButton_->SetPosition(SECOND_COLUMN_COLOR_BUTTON_POS_X - COLOR_BUTTON_OFFSET,
                                        THIRD_ROW_COLOR_BUTTON_POS_Y - COLOR_BUTTON_OFFSET, COLOR_BUTTON_WIDTH,
                                        COLOR_BUTTON_HEIGHT);
    } else {
        colorPurpleButton_->SetPosition(SECOND_COLUMN_COLOR_BUTTON_POS_X, THIRD_ROW_COLOR_BUTTON_POS_Y,
                                        COLOR_BUTTON_WIDTH, COLOR_BUTTON_HEIGHT);
    }
    colorPurpleButton_->SetViewId(UIVIEW_FLASHLIGHT_UILABELBUTTONEXT_PURPLE);
    colorPurpleButton_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    colorPurpleButton_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::PRESSED);
    colorPurpleButton_->SetAlign(TEXT_ALIGNMENT_CENTER);
    LOADIMG::LoadBtnImage(colorPurpleButton_, FLASHLIGHT_IMAGE, g_colorButtonRes[PURPLE_COLOR],
                          g_colorButtonRes[PURPLE_COLOR]);
    container_->Add(colorPurpleButton_);
    colorPurpleButton_->SetVisible(true);
}

void FlashLightColorPage::InitMultiColor(uint16_t selectColor)
{
    colorMultiButton_ = new UILabelButtonExt();
    if (colorMultiButton_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "malloc fail, %s, %d\r\n", __func__, __LINE__);
        return;
    }
    if (selectColor == MULTI_COLOR) {
        colorMultiButton_->SetPosition(THIRD_COLUMN_COLOR_BUTTON_POS_X - COLOR_BUTTON_OFFSET,
                                       THIRD_ROW_COLOR_BUTTON_POS_Y - COLOR_BUTTON_OFFSET, COLOR_BUTTON_WIDTH,
                                       COLOR_BUTTON_HEIGHT);
    } else {
        colorMultiButton_->SetPosition(THIRD_COLUMN_COLOR_BUTTON_POS_X, THIRD_ROW_COLOR_BUTTON_POS_Y,
                                       COLOR_BUTTON_WIDTH, COLOR_BUTTON_HEIGHT);
    }
    colorMultiButton_->SetViewId(UIVIEW_FLASHLIGHT_UILABELBUTTONEXT_IRIDESCENT);
    colorMultiButton_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    colorMultiButton_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::PRESSED);
    colorMultiButton_->SetAlign(TEXT_ALIGNMENT_CENTER);
    LOADIMG::LoadBtnImage(colorMultiButton_, FLASHLIGHT_IMAGE, g_colorButtonRes[MULTI_COLOR],
                          g_colorButtonRes[MULTI_COLOR]);
    container_->Add(colorMultiButton_);
    colorMultiButton_->SetVisible(true);
}

void FlashLightColorPage::OnStart(void *data)
{
    uint16_t selectColor = (uint16_t)FlashLightModel::GetInstance().GetViewColorId();
    g_colorButtonRes[selectColor] = g_selectColorButtonRes[selectColor];
    container_ = new UIScrollView();
    if (container_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "malloc fail, %s, %d\r\n", __func__, __LINE__);
        return;
    }
    container_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    container_->SetStyle(STYLE_BACKGROUND_OPA, 0);
    container_->SetScrollBlankSize(CONTAINER_BLANK_SIZE, UIAbstractScroll::Direction::TOP);
    container_->SetScrollBlankSize(CONTAINER_BLANK_SIZE, UIAbstractScroll::Direction::BOTTOM);

    colorLabel_ = new UILabel();
    if (colorLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "malloc fail, %s, %d\r\n", __func__, __LINE__);
        return;
    }
    colorLabel_->SetPosition(COLOR_LABEL_POS_X, COLOR_LABEL_POS_Y, COLOR_LABEL_WIDTH, COLOR_LABEL_HEIGHT);
    colorLabel_->SetText("灯光颜色");
    colorLabel_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    colorLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    colorLabel_->SetFont(BOLD_VECTOR_FONT_FILENAME, COLOR_LABEL_FONT_SIZE);
    container_->Add(colorLabel_);
    colorLabel_->SetVisible(true);

    // 第一行颜色按钮
    InitWhiteColor(selectColor);
    InitRedColor(selectColor);
    InitOrangeColor(selectColor);

    // 第二行颜色按钮
    InitYellowColor(selectColor);
    InitGreenColor(selectColor);
    InitLightBlueColor(selectColor);

    // 第三行颜色按钮
    InitDarkBlueColor(selectColor);
    InitPurpleColor(selectColor);
    InitMultiColor(selectColor);

    // 设置界面组件回调函数
    container_->SetOnDragListener(this);
    // 设置手电筒多个颜色按钮回调函数
    colorOrangeButton_->SetOnClickListener(this);
    colorWhiteButton_->SetOnClickListener(this);
    colorRedButton_->SetOnClickListener(this);
    colorLightBlueButton_->SetOnClickListener(this);
    colorGreenButton_->SetOnClickListener(this);
    colorYellowButton_->SetOnClickListener(this);
    colorMultiButton_->SetOnClickListener(this);
    colorPurpleButton_->SetOnClickListener(this);
    colorDarkBlueButton_->SetOnClickListener(this);
    AddViewToPageContainer(container_);
}

FlashLightColorPage::FlashLightColorPage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "FlashLightColorPage::FlashLightColorPage");
}

FlashLightColorPage::~FlashLightColorPage()
{
    if (container_ != nullptr) {
        container_->RemoveAll();
        delete container_;
        container_ = nullptr;
    }
    if (colorLabel_ != nullptr) {
        delete colorLabel_;
        colorLabel_ = nullptr;
    }
    if (colorWhiteButton_ != nullptr) {
        delete colorWhiteButton_;
        colorWhiteButton_ = nullptr;
    }
    if (colorRedButton_ != nullptr) {
        delete colorRedButton_;
        colorRedButton_ = nullptr;
    }
    if (colorOrangeButton_ != nullptr) {
        delete colorOrangeButton_;
        colorOrangeButton_ = nullptr;
    }
    if (colorYellowButton_ != nullptr) {
        delete colorYellowButton_;
        colorYellowButton_ = nullptr;
    }
    if (colorGreenButton_ != nullptr) {
        delete colorGreenButton_;
        colorGreenButton_ = nullptr;
    }
    if (colorLightBlueButton_ != nullptr) {
        delete colorLightBlueButton_;
        colorLightBlueButton_ = nullptr;
    }
    if (colorDarkBlueButton_ != nullptr) {
        delete colorDarkBlueButton_;
        colorDarkBlueButton_ = nullptr;
    }
    if (colorPurpleButton_ != nullptr) {
        delete colorPurpleButton_;
        colorPurpleButton_ = nullptr;
    }
    if (colorMultiButton_ != nullptr) {
        delete colorMultiButton_;
        colorMultiButton_ = nullptr;
    }
    ImageCacheManager::GetInstance().UnloadSingleRes(FLASHLIGHT_IMAGE);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "FlashLightColorPage::~FlashLightColorPage");
}

bool FlashLightColorPage::OnDrag(UIView &view, const DragEvent &event)
{
    return true;
}

bool FlashLightColorPage::OnDragStart(UIView& view, const DragEvent& event)
{
    return true;
}
 
bool FlashLightColorPage::OnDragEnd(UIView &view, const DragEvent &event)
{
    if (event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) {
        NativeAbility::GetInstance().SwitchPageInSlice(FlashLightAllView::FLASH_LIGHT_SET_VIEW,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}

bool FlashLightColorPage::OnClick(UIView &view, const ClickEvent &event)
{
    if (strcmp(view.GetViewId(), UIVIEW_FLASHLIGHT_UILABELBUTTONEXT_WHITE) == 0) {
        FlashLightModel::GetInstance().SetViewColorId(FlashLightColorAttribute::WHITE_COLOR);
        NativeAbility::GetInstance().SwitchPageInSlice(FlashLightAllView::FLASH_LIGHT_PLAY_VIEW,
            TransitionType::TRANSITION_INVALID, false);
    } else if (strcmp(view.GetViewId(), UIVIEW_FLASHLIGHT_UILABELBUTTONEXT_RED) == 0) {
        FlashLightModel::GetInstance().SetViewColorId(FlashLightColorAttribute::RED_COLOR);
        NativeAbility::GetInstance().SwitchPageInSlice(FlashLightAllView::FLASH_LIGHT_PLAY_VIEW,
            TransitionType::TRANSITION_INVALID, false);
    } else if (strcmp(view.GetViewId(), UIVIEW_FLASHLIGHT_UILABELBUTTONEXT_ORANGE) == 0) {
        FlashLightModel::GetInstance().SetViewColorId(FlashLightColorAttribute::ORANGE_COLOR);
        NativeAbility::GetInstance().SwitchPageInSlice(FlashLightAllView::FLASH_LIGHT_PLAY_VIEW,
            TransitionType::TRANSITION_INVALID, false);
    } else if (strcmp(view.GetViewId(), UIVIEW_FLASHLIGHT_UILABELBUTTONEXT_YELLOW) == 0) {
        FlashLightModel::GetInstance().SetViewColorId(FlashLightColorAttribute::YELLOW_COLOR);
        NativeAbility::GetInstance().SwitchPageInSlice(FlashLightAllView::FLASH_LIGHT_PLAY_VIEW,
            TransitionType::TRANSITION_INVALID, false);
    } else if (strcmp(view.GetViewId(), UIVIEW_FLASHLIGHT_UILABELBUTTONEXT_GREEN) == 0) {
        FlashLightModel::GetInstance().SetViewColorId(FlashLightColorAttribute::GREEN_COLOR);
        NativeAbility::GetInstance().SwitchPageInSlice(FlashLightAllView::FLASH_LIGHT_PLAY_VIEW,
            TransitionType::TRANSITION_INVALID, false);
    } else if (strcmp(view.GetViewId(), UIVIEW_FLASHLIGHT_UILABELBUTTONEXT_LIGHTBLUE) == 0) {
        FlashLightModel::GetInstance().SetViewColorId(FlashLightColorAttribute::LIGHT_BLUE_COLOR);
        NativeAbility::GetInstance().SwitchPageInSlice(FlashLightAllView::FLASH_LIGHT_PLAY_VIEW,
            TransitionType::TRANSITION_INVALID, false);
    } else if (strcmp(view.GetViewId(), UIVIEW_FLASHLIGHT_UILABELBUTTONEXT_DARKBLUE) == 0) {
        FlashLightModel::GetInstance().SetViewColorId(FlashLightColorAttribute::DARK_BLUE_COLOR);
        NativeAbility::GetInstance().SwitchPageInSlice(FlashLightAllView::FLASH_LIGHT_PLAY_VIEW,
            TransitionType::TRANSITION_INVALID, false);
    } else if (strcmp(view.GetViewId(), UIVIEW_FLASHLIGHT_UILABELBUTTONEXT_PURPLE) == 0) {
        FlashLightModel::GetInstance().SetViewColorId(FlashLightColorAttribute::PURPLE_COLOR);
        NativeAbility::GetInstance().SwitchPageInSlice(FlashLightAllView::FLASH_LIGHT_PLAY_VIEW,
            TransitionType::TRANSITION_INVALID, false);
    } else if (strcmp(view.GetViewId(), UIVIEW_FLASHLIGHT_UILABELBUTTONEXT_IRIDESCENT) == 0) {
        FlashLightModel::GetInstance().SetViewColorId(FlashLightColorAttribute::MULTI_COLOR);
        NativeAbility::GetInstance().SwitchPageInSlice(FlashLightAllView::FLASH_LIGHT_PLAY_VIEW,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}
}