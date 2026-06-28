    /*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: dfx image
 * Author: Hisi Team
 * Created: 2025-12
 */
#include "dfx/dfx_img.h"
#include "common/image_cache_manager.h"
#include "components/ui_view_group.h"
#include "components/root_view.h"

namespace OHOS {
DfxImg* DfxImg::GetInstance()
{
    static DfxImg instance;
    return &instance;
}

void DfxImg::Show(Rect rect, uint32_t resId, std::string file, bool isAutoSize, uint8_t mode)
{
    if (imageView_ == nullptr) {
        imageView_ = new UIImageView();
        if (imageView_ == nullptr) {
            GRAPHIC_CHR_E("show image faild, alloc UIImageView faild");
            return;
        }
    }

    imageView_->SetAutoEnable(isAutoSize);
    if (isAutoSize) {
        imageView_->SetPosition(rect.GetX(), rect.GetY());
    } else {
        imageView_->SetPosition(rect.GetX(), rect.GetY(), rect.GetWidth(), rect.GetHeight());
    }
    imageView_->SetResizeMode(static_cast<UIImageView::ImageResizeMode>(mode));

    if (isLoadImgRes_) {
        if (resId_ != 0) {
            ImageCacheManager::GetInstance().UnloadOneInMultiRes(resId_, file_);
        } else {
            ImageCacheManager::GetInstance().UnloadSingleRes(file_);
        }
        isLoadImgRes_ = false;
    }

    ImageInfo* imageInfo_ = nullptr;
    if (resId != 0) {
        imageInfo_ = ImageCacheManager::GetInstance().LoadOneInMultiRes(resId, file);
    } else {
        imageInfo_ = ImageCacheManager::GetInstance().LoadSingleRes(file);
    }
    if (imageInfo_ == nullptr) {
        GRAPHIC_CHR_E("show image faild, load res faild, file:%s resId:%u", file.c_str(), resId);
        Hide();
        return;
    }

    resId_ = resId;
    file_ = file;
    isLoadImgRes_ = true;
    imageView_->SetSrc(imageInfo_);
    if (imageView_->GetParent() == nullptr) {
        RootView::GetInstance()->Add(imageView_);
    }
    RootView::GetInstance()->Invalidate();
}

void DfxImg::Hide()
{
    if (isLoadImgRes_) {
        if (resId_ != 0) {
            ImageCacheManager::GetInstance().UnloadOneInMultiRes(resId_, file_);
        } else {
            ImageCacheManager::GetInstance().UnloadSingleRes(file_);
        }
        isLoadImgRes_ = false;
    }

    if (imageView_ != nullptr) {
        UIViewGroup* parent = dynamic_cast<UIViewGroup*>(imageView_->GetParent());
        if (parent != nullptr) {
            parent->Remove(imageView_);
        }
        delete imageView_;
        imageView_ = nullptr;
    }
    RootView::GetInstance()->Invalidate();
}
}