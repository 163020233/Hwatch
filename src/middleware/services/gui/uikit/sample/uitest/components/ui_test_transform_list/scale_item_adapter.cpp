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

#include "scale_item_adapter.h"
#include "common/image_cache_manager.h"
#include "components/ui_transform_group.h"
#include "components/ui_image_view.h"
#include "components/ui_label_ext.h"
#include "graphic_config.h"
#include "ui_test_list_fps/ui_list_fps_res.h"
#include "securec.h"

namespace OHOS {
class ItemContainer : public UITransformGroup {
public:
    ItemContainer() : leftImg_(nullptr), label_(nullptr), rightImg_(nullptr)
    {
        SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    }

    ~ItemContainer() override
    {
        ClearRes();
    }

    void ClearRes()
    {
        RemoveAll();
        if (leftImg_ != nullptr) {
            delete leftImg_;
            leftImg_ = nullptr;
        }
        if (label_ != nullptr) {
            delete label_;
            label_ = nullptr;
        }
        if (rightImg_ != nullptr) {
            delete rightImg_;
            rightImg_ = nullptr;
        }
    }

    bool InitContainer()
    {
        if (leftImg_ == nullptr) {
            leftImg_ = new UIImageView();
            if (leftImg_ == nullptr) {
                return false;
            }
        }
        leftImg_->SetResizeMode(UIImageView::ImageResizeMode::CENTER);

        if (label_ == nullptr) {
            label_ = new UILabelExt();
            if (label_ == nullptr) {
                ClearRes();
                return false;
            }
        }
        label_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
        label_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 40);  // 40: item font size

        if (rightImg_ == nullptr) {
            rightImg_ = new UIImageView();
            if (rightImg_ == nullptr) {
                ClearRes();
                return false;
            }
        }
        rightImg_->SetResizeMode(UIImageView::ImageResizeMode::CENTER);

        Add(label_);
        Add(leftImg_);
        Add(rightImg_);
        return true;
    }

    UIImageView* GetLeftImageView()
    {
        return leftImg_;
    }

    UILabelExt* GetLabelView()
    {
        return label_;
    }

    UIImageView* GetRightImgView()
    {
        return rightImg_;
    }

    ItemContainer(const ItemContainer &) = delete;
    ItemContainer &operator=(const ItemContainer &) = delete;

private:
    UIImageView* leftImg_;
    UILabelExt* label_;
    UIImageView* rightImg_;
};

ScaleItemAdapter::~ScaleItemAdapter()
{
    ClearData();
}

uint16_t ScaleItemAdapter::GetCount()
{
    return nodeList_.Size();
}

void ScaleItemAdapter::ClearData()
{
    ListNode<ItemDataNode*>* node = nodeList_.Begin();
    while (node != nodeList_.End()) {
        if (node->data_ != nullptr) {
            UIFree(node->data_);
            node->data_ = nullptr;
        }
        node = node->next_;
    }
    ImageCacheManager::GetInstance().UnloadAllInMultiRes(LIST_FPS);
}

void ScaleItemAdapter::FreeNode(ItemDataNode* node)
{
    if (node != nullptr) {
        UIFree(node);
        node = nullptr;
    }
}

void ScaleItemAdapter::SetData(const List<ItemDataNode*>* data)
{
    if (data == nullptr) {
        return;
    }
    if (!nodeList_.IsEmpty()) {
        ClearData();
    }
    ListNode<ItemDataNode*>* node = data->Begin();
    while (node != data->End()) {
        ItemDataNode* tmpNode = static_cast<ItemDataNode *>(UIMalloc(sizeof(ItemDataNode)));
        if (tmpNode == nullptr) {
            return;
        }

        tmpNode->resId = node->data_->resId;
        tmpNode->txt = node->data_->txt;
        tmpNode->lineBreakMode = node->data_->lineBreakMode;
        tmpNode->imgInfo = nullptr;

        nodeList_.PushBack(tmpNode);
        node = node->next_;
    }
}

UIView* ScaleItemAdapter::GetView(UIView* inView, int16_t index)
{
    if (nodeList_.IsEmpty() || index > nodeList_.Size() - 1 || index < 0) {
        return nullptr;
    }

    ListNode<ItemDataNode*>* node = nodeList_.Begin();
    for (uint16_t i = 0; i < index; i++) {
        node = node->next_;
    }
    ItemContainer* item = nullptr;
    if (inView == nullptr) {
        item = new ItemContainer();
        if (item == nullptr) {
            return nullptr;
        }
        if (!item->InitContainer()) {
            delete item;
            return nullptr;
        }
        item->Resize(width_, height_);

        UIImageView* leftImg = item->GetLeftImageView();
        leftImg->SetPosition(imageRect_.GetX(), imageRect_.GetY(), imageRect_.GetWidth(), imageRect_.GetHeight());

        UILabelExt* label = item->GetLabelView();
        label->SetPosition(labelRect_.GetX(), labelRect_.GetY(), labelRect_.GetWidth(), labelRect_.GetHeight());

        UIImageView* rightImg = item->GetRightImgView();
        rightImg->SetPosition(labelRect_.GetX() + labelRect_.GetWidth(),
            imageRect_.GetY(), imageRect_.GetWidth(), imageRect_.GetHeight());
    } else {
        item = static_cast<ItemContainer*>(inView);
    }

    if (item != nullptr) {
        UIImageView* leftImg = item->GetLeftImageView();
        if (leftImg != nullptr) {
            if (node->data_->imgInfo == nullptr) {
                node->data_->imgInfo = ImageCacheManager::GetInstance().LoadOneInMultiRes(node->data_->resId, LIST_FPS);
            }
            leftImg->SetSrc(node->data_->imgInfo);
        }

        UILabelExt* label = item->GetLabelView();
        if (label != nullptr) {
            label->SetText(node->data_->txt.c_str());
            label->SetLineBreakMode(node->data_->lineBreakMode);
            label->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
        }

        UIImageView* rightImg = item->GetRightImgView();
        if (rightImg != nullptr) {
            if (arrowImg_ == nullptr) {
                arrowImg_ = ImageCacheManager::GetInstance().LoadSingleRes(RES_PATH"left_arrow.bin");
            }
            rightImg->SetSrc(arrowImg_);
        }

        item->SetViewIndex(index);
    }
    return item;
}

void ScaleItemAdapter::ItemResize(uint16_t w, uint16_t h)
{
    width_ = w;
    height_ = h;
}

void ScaleItemAdapter::SetLabelPosition(int16_t x, int16_t y, uint16_t w, uint16_t h)
{
    labelRect_.SetPosition(x, y);
    labelRect_.SetWidth(w);
    labelRect_.SetHeight(h);
}

void ScaleItemAdapter::SetImagePosition(int16_t x, int16_t y, uint16_t w, uint16_t h)
{
    imageRect_.SetPosition(x, y);
    imageRect_.SetWidth(w);
    imageRect_.SetHeight(h);
}
} // namespace OHOS