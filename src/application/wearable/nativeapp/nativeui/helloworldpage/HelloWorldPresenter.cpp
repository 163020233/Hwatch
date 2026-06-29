/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: HelloWorldPresenter
 * Create: 2025-06-06
 */

#include "NativeRegisterManager.h"
#include "helloworldpage/HelloWorldView.h"
#include "helloworldpage/HelloWorldPresenter.h"

namespace OHOS {
#ifdef SDK_DITING_COMMUNITY
REGIST_MENU(VIEW_HELLOWORLD, HelloWorldView, HelloWorldPresenter, EMPTY_ICON, EMPTY_ICON, "HelloWorld");
#endif
}
