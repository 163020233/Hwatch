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
#ifndef SCALE_ITEM_ADAPTER_H
#define SCALE_ITEM_ADAPTER_H
#include "components/abstract_adapter.h"
#include "gfx_utils/list.h"
#include "gfx_utils/vector.h"

namespace OHOS {
struct ItemDataNode {
    int resId;
    std::string txt;
    uint8_t lineBreakMode;
    ImageInfo* imgInfo;
};

class ScaleItemAdapter : public AbstractAdapter {
public:
    ScaleItemAdapter() : width_(0), height_(0), arrowImg_(nullptr) {}
    ~ScaleItemAdapter() override;

    UIView* GetView(UIView* inView, int16_t index) override;
    uint16_t GetCount() override;
    void SetData(const List<ItemDataNode*>* data);

    void ItemResize(uint16_t w, uint16_t h);
    void SetLabelPosition(int16_t x, int16_t y, uint16_t w, uint16_t h);
    void SetImagePosition(int16_t x, int16_t y, uint16_t w, uint16_t h);
private:
    void ClearData(void);
    void FreeNode(ItemDataNode* node);
    List<ItemDataNode*> nodeList_;
    uint16_t width_;
    uint16_t height_;
    Rect imageRect_;
    Rect labelRect_;
    ImageInfo* arrowImg_;
};
} // namespace OHOS
#endif // SCALE_ITEM_ADAPTER_H
