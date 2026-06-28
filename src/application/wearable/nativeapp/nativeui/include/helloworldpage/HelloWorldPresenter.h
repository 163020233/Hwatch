/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: HelloWorld Presenter for MVP architecture
 * Create: 2025-04-24
 */

#ifndef HELLOWORLD_PRESENTER_H
#define HELLOWORLD_PRESENTER_H

#include "Presenter.h"

namespace OHOS {
/** Forward declaration of HelloWorldView for template dependency */
class HelloWorldView;

/**
 * @brief Presenter layer for HelloWorld page
 * Implements the presenter component in MVP architecture
 */
class HelloWorldPresenter : public Presenter<HelloWorldView> {};
}

#endif  // HELLOWORLD_PRESENTER_H