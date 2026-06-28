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

#include "ui_test_scale_list.h"
#include "ui_test_list_fps/ui_list_fps_res.h"
#include "graphic_config.h"
#include "securec.h"

namespace OHOS {
static const int16_t LIST_WIDTH = 454;
static const int16_t LIST_HEIGHT = 454;
static const uint16_t ITEM_WIDTH = 400;
static const uint16_t ITEM_HEIGHT = 100;
static const uint16_t IMAGE_WIDTH = 50;
static const uint16_t IMAGE_HEIGHT = 50;
static const uint16_t LABEL_WIDTH = 300;
static const uint16_t LABEL_HEIGHT = 100;
static const uint16_t SCALE_FACTOR = 400;
static ItemDataNode g_itemNode[] = {
    {APPLIST_ALARM, "闹钟", UILabel::LINE_BREAK_ELLIPSIS},
    {BLUETOOTH_TITLE, "蓝牙", UILabel::LINE_BREAK_ELLIPSIS},
    {APPLIST_CALLER_IMAGE, "通话记录", UILabel::LINE_BREAK_ELLIPSIS},
    {APPLIST_COMPASS_IMAGE, "指南针", UILabel::LINE_BREAK_ELLIPSIS},
    {APPLIST_SETTING, "设置", UILabel::LINE_BREAK_ELLIPSIS},
    {APPLIST_SPO2, "血氧饱和度", UILabel::LINE_BREAK_ELLIPSIS},
    {APPLIST_VOCASSIST_IMAGE, "语音助手", UILabel::LINE_BREAK_ELLIPSIS},
    {APPLIST_ALARM, "长文本滚动测试ABCD", UILabel::LINE_BREAK_MARQUEE},
    {APPLIST_ALARM, "闹钟", UILabel::LINE_BREAK_ELLIPSIS},
    {BLUETOOTH_TITLE, "蓝牙", UILabel::LINE_BREAK_ELLIPSIS},
    {APPLIST_CALLER_IMAGE, "通话记录", UILabel::LINE_BREAK_ELLIPSIS},
    {APPLIST_COMPASS_IMAGE, "指南针", UILabel::LINE_BREAK_ELLIPSIS}
};

static bool TransformItemBaseOnPosition(UITransformList* list, UIView* item)
{
    if (list == nullptr || item == nullptr) {
        return false;
    }
    if (item->GetViewType() != UI_TRANSFORM_GROUP) {
        return false;
    }
    UITransformGroup* group = dynamic_cast<UITransformGroup*>(item);
    if (group == nullptr) {
        return false;
    }
    int16_t w = group->GetWidth();
    int16_t h = group->GetHeight();
    if (list->GetDirection() == UIList::VERTICAL) {
        int16_t midY = list->GetY() + (static_cast<uint16_t>(list->GetHeight()) >> 1);
        int16_t y = group->GetY();
        int offset = (y + (static_cast<uint16_t>(h) >> 1) - midY);
        int temp = EasingEquation::SineEaseIn(0, SCALE_FACTOR, abs(offset), SCALE_FACTOR);
        float scale = 1.0 - (static_cast<float>(temp) / SCALE_FACTOR);
        group->Scale(Vector2<float>(scale, scale),
                     Vector2<float>(static_cast<uint16_t>(w) >> 1,  static_cast<uint16_t>(h) >> 1));
        if ((y <= midY) && (y + group->GetHeight() >= midY)) {
            return true;
        }
    }
    return false;
}

void UITestScaleList::SetUp()
{
    if (data_ == nullptr) {
        data_ = new List<ItemDataNode*>();
        if (data_ == nullptr) {
            return;
        }
        for (uint8_t i = 0; i < (sizeof(g_itemNode) / sizeof(ItemDataNode)); ++i) {
            data_->PushBack(&g_itemNode[i]);
        }
    }
}

void UITestScaleList::TearDown()
{
    if (data_ != nullptr) {
        data_->Clear();
        delete data_;
        data_ = nullptr;
    }
    if (adapter_ != nullptr) {
        delete adapter_;
        adapter_ = nullptr;
    }
    if (list_ != nullptr) {
        delete list_;
        list_ = nullptr;
    }
}

UIView* UITestScaleList::GetTestView()
{
    UIScaleListTest();
    return list_;
}

void UITestScaleList::UIScaleListTest()
{
    adapter_ = new ScaleItemAdapter();
    if (adapter_ == nullptr) {
        TearDown();
        return;
    }
    adapter_->ItemResize(ITEM_WIDTH, ITEM_HEIGHT);
    adapter_->SetLabelPosition(IMAGE_WIDTH, 0, LABEL_WIDTH, LABEL_HEIGHT);
    adapter_->SetImagePosition(0, 25, IMAGE_WIDTH, IMAGE_HEIGHT); // 25:image start pos y
    adapter_->SetData(data_);

    if (list_ == nullptr) {
        list_ = new UITransformList();
    }
    list_->SetPosition(0, 0, LIST_WIDTH, LIST_HEIGHT);
    list_->SetItemTransformFunc(&TransformItemBaseOnPosition);
    list_->SetAdapter(adapter_);
    list_->SetThrowDrag(true);
    list_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    list_->SetMaxScrollDistance(1000); // 1000: max scroll distance
    list_->SetSwipeACCLevel(200); // 200: compensation distance
    list_->SetDragACCLevel(5); // 5: drag acceleration
    list_->SetYScrollBarVisible(true);

    list_->EnableAutoAlign(false);
    list_->SetSelectPosition(LIST_HEIGHT / 2); // 2: divisor
    list_->EnableCrossDragBack(false);
    list_->SetElastic(true);
    list_->SetReboundSize(LIST_HEIGHT / 2); // 2: divisor
    list_->SetItemSpace(30); // 30: item space
    list_->SetAutoAlignTime(300); // 300: align time
    list_->SetScrollBlankSize(LIST_HEIGHT / 2); // 2: divisor
    list_->SetScrollBlankSize(LIST_HEIGHT / 2 - ITEM_HEIGHT / 2, UIAbstractScroll::Direction::BOTTOM);
    list_->SetReboundSize(0, UIAbstractScroll::Direction::BOTTOM);
    list_->SetDragFunc(EasingEquation::QuadEaseOut);
}
} // namespace OHOS