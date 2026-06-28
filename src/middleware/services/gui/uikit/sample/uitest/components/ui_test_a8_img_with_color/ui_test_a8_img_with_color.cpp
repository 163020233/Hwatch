/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: UITestA8ImgWithColor
 * Author: Hisi Graphic Team
 * Created: 2026-01
 */

#include "ui_test_a8_img_with_color.h"
#include "common/screen.h"
#include "common/image_cache_manager.h"

namespace OHOS {
namespace {
const char *A8_RES = RES_PATH "A8_pack.bin";
constexpr uint32_t A8_IMG_NUM = 7;
constexpr uint32_t RES_IDS[A8_IMG_NUM] = {
    0x100001,
    0x100002,
    0x100003,
    0x100004,
    0x100005,
    0x100006,
    0x100007
};

constexpr uint32_t COLORS[A8_IMG_NUM] = {
    0xffff0000,
    0xff00ff00,
    0xff0000ff,
    0xff00ffff,
    0xffff00ff,
    0xffffff00,
    0xefee0ff0
};
}

void UITestA8ImgWithColor::SetUp()
{
    if (container_ != nullptr) {
        return;
    }

    if (InitRes()) {
        InitViews();
        SetPeriod(1000); // 1000ms
        Init();
    } else {
        DeInitRes();
    }
}

void UITestA8ImgWithColor::TearDown()
{
    if (container_ != nullptr) {
        container_->RemoveAll();
        DeInitRes();
        Deinit();
    }
}

const UIView* UITestA8ImgWithColor::GetTestView()
{
    return container_;
}

void UITestA8ImgWithColor::Callback()
{
    if (container_ == nullptr) {
        return;
    }

    colorIndex_++;
    for (uint32_t i = 0; i < A8_IMG_NUM; ++i) {
        ColorType color;
        color.full = COLORS[(i + colorIndex_) % A8_IMG_NUM];
        imgs_[i]->SetColor(color);
    }
}

bool UITestA8ImgWithColor::InitRes()
{
    container_ = new (std::nothrow) UIScrollView;
    if (container_ == nullptr) {
        return false;
    }

    imgs_ = new (std::nothrow) UIImageView* [A8_IMG_NUM];
    if (imgs_ == nullptr) {
        return false;
    }

    for (uint32_t i = 0; i < A8_IMG_NUM; ++i) {
        imgs_[i] = new (std::nothrow) UIImageView;
        if (imgs_[i] == nullptr) {
            return false;
        }
        ++imgNum_;
        ImageInfo *info = ImageCacheManager::GetInstance().LoadOneInMultiRes(RES_IDS[i], std::string(A8_RES));
        if (info == nullptr) {
            return false;
        }
        info->color = COLORS[i];
        imgs_[i]->SetSrc(info);
    }

    return true;
}

void UITestA8ImgWithColor::DeInitRes()
{
    if (container_ != nullptr) {
        delete container_;
        container_ = nullptr;
    }

    if (imgs_ != nullptr) {
        for (uint32_t i = 0; i < imgNum_; ++i) {
            delete imgs_[i];
        }
        imgNum_ = 0;
        delete []imgs_;
        imgs_ = nullptr;
    }
    ImageCacheManager::GetInstance().UnloadAllInMultiRes(std::string(A8_RES));
}

void UITestA8ImgWithColor::InitViews()
{
    container_->SetThrowDrag(true);
    container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    container_->SetHorizontalScrollState(true);
    container_->SetVerticalScrollState(true);
    const int16_t gap = 20;
    int16_t y = 0;
    for (uint32_t i = 0; i < imgNum_; ++i) {
        imgs_[i]->SetPosition(0, y);
        container_->Add(imgs_[i]);
        y += gap + imgs_[i]->GetHeight();
    }
}
} // namespace OHOS
