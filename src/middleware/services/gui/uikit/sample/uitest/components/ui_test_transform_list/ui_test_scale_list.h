/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef UI_TEST_SCALEABEL_LIST_H
#define UI_TEST_SCALEABEL_LIST_H

#include "graphic_config.h"
#include "components/ui_transform_list.h"
#include "ui_test.h"
#include "ui_test_transform_list/scale_item_adapter.h"

namespace OHOS {
class UITestScaleList : public UITest {
public:
    UITestScaleList() : list_(nullptr), data_(nullptr), adapter_(nullptr) {}
    ~UITestScaleList() override {}
    void SetUp() override;
    void TearDown() override;
    UIView* GetTestView() override;
    void UIScaleListTest();

private:
    UITransformList* list_;
    List<ItemDataNode*>* data_;
    ScaleItemAdapter* adapter_;
};
} // namespace OHOS
#endif // UI_TEST_SCALEABEL_LIST_H