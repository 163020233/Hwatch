/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: List with reach detect function
 * Author: Hisi Team
 * Created: 2025-12
 */
#ifndef UI_TEST_SIMPLE_REACH_LIST_H
#define UI_TEST_SIMPLE_REACH_LIST_H

#include "ui_simple_reach_list.h"
#include "ui_test.h"

namespace OHOS {
class UITestSimpleReachList : public UITest, UIView::OnClickListener, ListReachListener {
public:
    UITestSimpleReachList() {}
    ~UITestSimpleReachList() override {}
    void SetUp() override;
    void TearDown() override;
    UIView* GetTestView() override;
    void OnReachEnd() override;

private:
    UISimpleReachList* list_{nullptr};
    List<Contents*> data_;
    uint16_t listCount_{0};
    uint16_t listCurCount_{0};
};
} // namespace OHOS
#endif // UI_TEST_SIMPLE_REACH_LIST_H

