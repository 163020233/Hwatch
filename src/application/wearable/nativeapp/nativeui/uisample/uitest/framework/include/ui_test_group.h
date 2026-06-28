/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: ui_test_group.h
 * Create: 2025-08-12
 */

#ifndef UI_TEST_GROUP_H
#define UI_TEST_GROUP_H

#include "components/ui_view.h"
#include "gfx_utils/list.h"

namespace OHOS {
class UITest;
struct TestCaseInfo {
    const char* sliceId;
    UITest* testObj;
};

class UITestGroup : public HeapBase {
public:
    static UITestGroup* GetInstance()
    {
        static UITestGroup instance;
        return &instance;
    }

    void SetUpTestCase();
    void SetUpHSTestCase();
    void SetUpOHTestCase();

    List<TestCaseInfo>*& GetTestCase();
    UITest* GetTestCase(const char* id);
private:
    UITestGroup() {}
    ~UITestGroup();
    UITestGroup(const UITestGroup&) = delete;
    UITestGroup& operator=(const UITestGroup&) = delete;
    void UpdataContents();
    void TearDownTestCase();
    List<TestCaseInfo> *testCaseList_ = nullptr;
};
} // namespace OHOS
#endif