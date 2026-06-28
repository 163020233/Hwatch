/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: HelloWorld Page Implementation
 * Create: 2025-04-24
 */
#ifndef HELLOWORLD_PAGE_H
#define HELLOWORLD_PAGE_H

#include "components/ui_view_group.h"
#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "SlicePage.h"
#include "HelloWorldPresenter.h"

namespace OHOS {
/**
 * @brief HelloWorldPage class that manages the UI components and page lifecycle
 *        Inherits from SlicePage with corresponding HelloWorldPresenter
 */
class HelloWorldPage : public SlicePage<HelloWorldPresenter> {
public:
    /** @brief Constructor */
    HelloWorldPage();

    /** @brief Destructor */
    ~HelloWorldPage();

    /**
     * @brief Override the OnStart lifecycle method
     * @param data Pointer to initialization data
     */
    void OnStart(void* data) override;

private:
    // Disable copy and move semantics to avoid unexpected behavior
    HelloWorldPage(const HelloWorldPage &) = delete;
    HelloWorldPage &operator=(const HelloWorldPage &) = delete;
    HelloWorldPage(HelloWorldPage &&) = delete;
    HelloWorldPage &operator=(HelloWorldPage &&) = delete;

    UIViewGroup *container_ = nullptr;      /** Root container for UI components */
    UILabel *helloWorldLabel_ = nullptr;   /** Label used to display HelloWorld text */
};
}  // namespace OHOS

#endif /* HELLOWORLD_PAGE_H */
