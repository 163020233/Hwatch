/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Test application
 * Create: 2025-09-20
 */

#ifndef UI_TEST_APP_H
#define UI_TEST_APP_H

#include "components/ui_list.h"
#include "test_case_list_adapter.h"

namespace OHOS {
class UITestApp {
public:
    static UITestApp* GetInstance();
    void Start();
    void Init();

private:
    UITestApp() {}
    ~UITestApp();

    UITestApp(const UITestApp&) = delete;
    UITestApp& operator=(const UITestApp&) = delete;
    UITestApp(UITestApp&&) = delete;
    UITestApp& operator=(UITestApp&&) = delete;

    UIList* mainList_ = nullptr;
    TestCaseListAdapter* adapter_ = nullptr;
};
} // namespace OHOS
#endif
