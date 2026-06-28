/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: HelloWorld View for MVP architecture
 * Create: 2025-04-24
 */

#ifndef HELLOWORLD_VIEW_H
#define HELLOWORLD_VIEW_H

#include "View.h"

namespace OHOS {
/** Forward declaration for template dependency */
class HelloWorldPresenter;

/**
 * @brief View layer for HelloWorld page
 * Defines the view interface in MVP architecture
 */
class HelloWorldView : public View<HelloWorldPresenter> {};
}

#endif  // HELLOWORLD_VIEW_H
