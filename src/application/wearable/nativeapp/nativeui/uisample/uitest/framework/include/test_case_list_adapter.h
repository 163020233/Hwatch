/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: test_case_list_adapter.h
 * Create: 2025-06-09
 */

#ifndef TEST_CASE_LIST_ADAPTER_H
#define TEST_CASE_LIST_ADAPTER_H

#include "components/abstract_adapter.h"
#include "components/root_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"

namespace OHOS {
class TestCaseListAdapter : public AbstractAdapter {
public:
    TestCaseListAdapter() {}
    ~TestCaseListAdapter()
    {
        if (listener_ != nullptr) {
            delete listener_;
            listener_ = nullptr;
        }
    }
    UIView* GetView(UIView* inView, int16_t index) override;
    int16_t GetItemWidthWithMargin(int16_t index) override;
    int16_t GetItemHeightWithMargin(int16_t index) override;
    uint16_t GetCount() override;
private:
    UIView::OnClickListener* listener_{nullptr};
};
} // namespace OHOS
#endif // TEST_CASE_LIST_ADAPTER_H