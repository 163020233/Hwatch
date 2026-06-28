/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Test group
 * Create: 2025-09-20
 */

#include "ui_test_group.h"
#include "graphic_config.h"
#include "chip_definitions.h"
#if defined(TARGET_CHIP_SOCMN2) && (TARGET_CHIP_SOCMN2 == 1)
#include "ui_socmn2_test_table.h"
#else
#include "ui_wearable_test_table.h"
#endif


namespace OHOS {
void UITestGroup::AddTestCase(TestCaseInfo testCaseInfo)
{
    if (testCaseList_ == nullptr) {
        return;
    }
    testCaseList_->PushBack(testCaseInfo);
}

void UITestGroup::SetUpTestCase()
{
    testCaseList_ = new List<TestCaseInfo>();
    if (testCaseList_ == nullptr) {
        return;
    }
    testCaseList_->Clear();
    SetUpTestCaseToList(testCaseList_);
}

List<TestCaseInfo>& UITestGroup::GetTestCase()
{
    return *testCaseList_;
}

UITest* UITestGroup::GetTestCase(const char* id)
{
    if (id == nullptr) {
        return nullptr;
    }
    ListNode<TestCaseInfo>* node = testCaseList_->Begin();
    while (node != testCaseList_->End()) {
        if (!strcmp(id, node->data_.sliceId)) {
            return node->data_.testObj;
        }
        node = node->next_;
    }
    return nullptr;
}

void UITestGroup::TearDownTestCase()
{
    ListNode<TestCaseInfo>* node = testCaseList_->Begin();
    while (node != testCaseList_->End()) {
        delete node->data_.testObj;
        node->data_.testObj = nullptr;
        node = node->next_;
    }
    testCaseList_->Clear();
}
} // namespace OHOS
