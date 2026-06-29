/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: UIPostEffectImageView
 * Author: Hisi Graphic Team
 * Created: 2025-5
 */

#include "components/ui_post_effect_image_view.h"
#include "common/image_cache_manager.h"

namespace OHOS {

UIPostEffectImageView::~UIPostEffectImageView()
{
    if (!maskPath_.empty()) {
        ImageCacheManager::GetInstance().UnloadSingleRes(maskPath_);
    }
}

void UIPostEffectImageView::SetHSVInfo(int16_t hAdjust, int8_t sAdjust, int8_t vAdjust, int16_t threshold)
{
    if (hAdjust < MIN_HADJ_VALUE || hAdjust > MAX_HADJ_VALUE || sAdjust < MIN_SADJ_VALUE || sAdjust > MAX_SADJ_VALUE
        || vAdjust < MIN_VADJ_VALUE || vAdjust > MAX_VADJ_VALUE || threshold < MIN_THRESHOLD_VALUE ||
        threshold > MAX_THRESHOLD_VALUE) {
        GRAPHIC_LOGE("hsv adjust value is invalid[%d, %d, %d]", hAdjust, sAdjust, vAdjust);
        return;
    }
    hsvInfo_.hAdjust = hAdjust;
    hsvInfo_.sAdjust = sAdjust;
    hsvInfo_.vAdjust = vAdjust;
    hsvInfo_.threshold = threshold;
}

void UIPostEffectImageView::SetHSVMask(const std::string& mask)
{
    if (!maskPath_.empty() && maskPath_ != mask) {
        ImageCacheManager::GetInstance().UnloadSingleRes(maskPath_);
        maskPath_.clear();
    }

    maskInfo_ = ImageCacheManager::GetInstance().LoadSingleRes(mask);
    if (maskInfo_ == nullptr) {
        GRAPHIC_LOGE("Load mask:%s failed!", mask.c_str());
        return;
    }
    maskPath_ = mask;
}

void UIPostEffectImageView::DrawColorize(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea)
{
    LiteMGfxEngine::GetInstance()->SetColorize(colorize_.full);
    UIImageView::OnDraw(gfxDstBuffer, invalidatedArea);
    LiteMGfxEngine::GetInstance()->ClearColorize();
}

void UIPostEffectImageView::DrawHSVAdjust(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea)
{
    if (maskInfo_ == nullptr || maskInfo_->phyAddr == nullptr) {
        GRAPHIC_LOGE("mask is invalid!");
        return;
    }
    ImageCacheManager::GetInstance().UpdateImageInfoIfNecessary(*maskInfo_);
    if (maskInfo_->data == nullptr) {
        GRAPHIC_LOGE("UpdateImageInfoIfNecessary failed.");
        return;
    }

    LiteMGfxEngine::GetInstance()->SetHSVInfo(hsvInfo_.hAdjust, hsvInfo_.sAdjust, hsvInfo_.vAdjust,
        *maskInfo_, hsvInfo_.threshold);
    UIImageView::OnDraw(gfxDstBuffer, invalidatedArea);
    LiteMGfxEngine::GetInstance()->ClearHSVInfo();
}

void UIPostEffectImageView::OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea)
{
    if (isEnableColorize_) {
        DrawColorize(gfxDstBuffer, invalidatedArea);
        return;
    }

    if (isEnableHSVAdjust_) {
        DrawHSVAdjust(gfxDstBuffer, invalidatedArea);
        return;
    }

    UIImageView::OnDraw(gfxDstBuffer, invalidatedArea);
}

} // namespace OHOS
