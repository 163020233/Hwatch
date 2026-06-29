/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: HelloWorldPage
 * Create: 2025-06-06
 */

#include "UiConfig.h"
#include "main/LoadImg.h"
#include "ui_resource_empty.h"
#include "SlicePageFactory.h"
#include "wearable_log.h"
#include "AppViewIDs.h"
#include "NativeAbility.h"
#include "helloworldpage/HelloWorldPresenter.h"
#include "helloworldpage/HelloWorldPage.h"

namespace OHOS {
static constexpr uint16_t HELLOWORLD_PAGE = 1;
static constexpr uint16_t HELLO_LABEL_X = 121;  // 水平方向大致居中（(400-193)/2 ≈ 104，实取 121 留余量）
static constexpr uint16_t HELLO_LABEL_Y = 161;  // 垂直方向略高于屏幕中央（中央为 180）
static constexpr uint16_t HELLO_LABEL_W = 193;
static constexpr uint16_t HELLO_LABEL_H = 40;
#ifdef SDK_DITING_COMMUNITY
REGIST_SLICE_PAGE(VIEW_HELLOWORLD, HELLOWORLD_PAGE, HelloWorldPage, true);
#endif
void HelloWorldPage::OnStart(void *data)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "HelloWorldPage OnStart");
    bool ret = ImageCacheManager::GetInstance().LoadAllInMultiRes(EMPTY_IMAGE);
    if (ret == false) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "LoadEmptyImage fail");
        // 析构函数负责释放已分配的 container_ 和 helloWorldLabel_
        return;
    }
    container_ = new UIViewGroup();
    if (container_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "HelloWorldPage container_ new fail");
        // 析构函数负责释放已分配的 container_ 和 helloWorldLabel_
        return;
    }
    container_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    container_->SetDraggable(true);
    container_->SetTouchable(true);
    helloWorldLabel_ = new UILabel();
    if (helloWorldLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "HelloWorldPage helloWorldLabel_ new fail");
        // 析构函数负责释放已分配的 container_ 和 helloWorldLabel_
        return;
    }
    helloWorldLabel_->SetPosition(HELLO_LABEL_X, HELLO_LABEL_Y, HELLO_LABEL_W, HELLO_LABEL_H);
    helloWorldLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    helloWorldLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 30);
    helloWorldLabel_->SetText("Hello World");
    container_->Add(helloWorldLabel_);
    AddViewToPageContainer(container_);
}

HelloWorldPage::HelloWorldPage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "HelloWorldPage::HelloWorldPage");
}

HelloWorldPage::~HelloWorldPage()
{
    if (container_ != nullptr) {
        container_->RemoveAll();
        delete container_;
        container_ = nullptr;
    }
    if (helloWorldLabel_ != nullptr) {
        delete helloWorldLabel_;
        helloWorldLabel_ = nullptr;
    }
    ImageCacheManager::GetInstance().UnloadAllInMultiRes(EMPTY_IMAGE);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "HelloWorldPage::~HelloWorldPage");
}
}
