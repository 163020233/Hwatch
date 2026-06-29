/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Test group
 * Create: 2025-09-20
 */

#ifndef UI_TEST_GROUP_H
#define UI_TEST_GROUP_H

#include "components/ui_view.h"
#include "gfx_utils/list.h"
#include "ui_test.h"

namespace OHOS {
class UITestGroup {
public:
    static UITestGroup* GetInstance()
    {
        static UITestGroup instance;
        return &instance;
    }
    void SetUpTestCase();
    List<TestCaseInfo>& GetTestCase();
    UITest* GetTestCase(const char* id);
    void TearDownTestCase();
    void AddTestCase(TestCaseInfo testCaseInfo);

private:
    List<TestCaseInfo> *testCaseList_ = nullptr;
};
} // namespace OHOS
#endif