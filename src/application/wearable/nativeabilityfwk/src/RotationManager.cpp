/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 * Description: RotationManager
 * Author:
 * Create: 2026-02
 */

#include "RotationManager.h"
#include "gfx_utils/vector.h"
#include "wearable_log.h"
#include "common/screen.h"
#include "graphic_service.h"
#include "components/root_view.h"

namespace OHOS {

const static uint8_t PAGE_REGISTRATION_OFFSET = 16;

RotationManager::~RotationManager()
{
}

void RotationManager::RegisterRotationCapability(uint16_t sliceId, uint16_t pageId, uint8_t supportRotations)
{
    if (sliceId == 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Can't register using invalid id %u-%u", sliceId, pageId);
        return;
    }

    uint32_t id = sliceId | (pageId << PAGE_REGISTRATION_OFFSET);
    // pageId为0时，注册整个slice
    if (pageId == 0) {
        id = sliceId;
    }
    if ((pageSupportRotationsMap_.find(id) != pageSupportRotationsMap_.end()) ||
        (pageSupportRotationsMap_.find(sliceId) != pageSupportRotationsMap_.end())) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP,
            "RegisterRotation : Slice %u page:%u has already been registered",
            sliceId,
            pageId);
        return;
    } else {
        pageSupportRotationsMap_.insert({id, supportRotations});
    }
}

void RotationManager::SetDisplayRotation(AppRotationType rotation)
{
#if defined(ROTATE_METHOD) && (ROTATE_METHOD == 2)
    if (Screen::GetInstance().GetWidth() == Screen::GetInstance().GetHeight()) {
        LayerRotateType rot = ConvertToLayerRotation(rotation);
        GraphicService::GetInstance()->PostGraphicEvent([this, rot]() {
            curLayerRotation_ = rot;
            UpdateLayerRotation(rot);
        });
        return;
    }

    if (pageSupportRotationsMap_.size() == 0) {
        GRAPHIC_LOGI("there are no rotation pages!");
        return;
    }

    auto changeRotation = [this, rotation] () {
        displayRotation_ = rotation;
        for (uint16_t i = 0; i < listeners_.Size(); ++i) {
            listeners_[i]->OnRotationChanged();
        }
    };
    GraphicService::GetInstance()->PostGraphicEvent(changeRotation);
#endif
}

bool RotationManager::UpdateDeviceRotation(uint16_t sliceId, uint16_t pageId)
{
#if defined(ROTATE_METHOD) && (ROTATE_METHOD == 2)
    if (Screen::GetInstance().GetWidth() == Screen::GetInstance().GetHeight()) {
        return false;
    }

    if (IsSupportRotation(sliceId, pageId, displayRotation_)) {
        if (ConvertToLayerRotation(displayRotation_) != curLayerRotation_) {
            curLayerRotation_ = ConvertToLayerRotation(displayRotation_);
            UpdateLayerRotation(curLayerRotation_);
            return true;
        }
    } else {
        if (curLayerRotation_ != LayerRotateType::LAYER_ROTATE_NONE) {
            curLayerRotation_ = LayerRotateType::LAYER_ROTATE_NONE;
            UpdateLayerRotation(curLayerRotation_);
            return true;
        }
    }
#endif
    return false;
}

AppRotationType RotationManager::GetDisplayRotation() const
{
    return displayRotation_;
}

AppRotationType RotationManager::GetExpectedRotation(uint16_t sliceId, uint16_t pageId) const
{
    if (IsSupportRotation(sliceId, pageId, displayRotation_)) {
        return displayRotation_;
    }
    return AppRotationType::ROTATION_0;
}

AppRotationType RotationManager::GetCurContentRotation() const
{
    return ConvertToContentRotation(curLayerRotation_);
}

void RotationManager::RegisterRotationListener(OnRotationChangedListener* listener)
{
    if (listener == nullptr) {
        GRAPHIC_LOGE("listener is nullptr");
        return;
    }

    for (uint16_t i = 0; i < listeners_.Size(); ++i) {
        if (listeners_[i] == listener) {
            GRAPHIC_LOGE("has already registered!");
            return;
        }
    }
    listeners_.PushBack(listener);
}

void RotationManager::UnRegisterRotationListener(OnRotationChangedListener* listener)
{
    if (listener == nullptr) {
        GRAPHIC_LOGE("listener is nullptr");
        return;
    }

    for (uint16_t i = 0; i < listeners_.Size(); ++i) {
        if (listeners_[i] == listener) {
            listeners_.Erase(i);
            return;
        }
    }
    GRAPHIC_LOGW("lisener has not been registered.");
}

RotationManager::RotationManager() : defaultLayerRotation_(static_cast<LayerRotateType>(HW_ROTATE_ANGLE)) {}

AppRotationType RotationManager::ConvertToContentRotation(LayerRotateType displayRotation) const
{
    AppRotationType contentRotation = AppRotationType::ROTATION_0;
    switch (displayRotation) {
        case LayerRotateType::LAYER_ROTATE_NONE:
            contentRotation = AppRotationType::ROTATION_0;
            break;
        case LayerRotateType::LAYER_ROTATE_270:
            contentRotation = AppRotationType::ROTATION_90;
            break;
        case LayerRotateType::LAYER_ROTATE_180:
            contentRotation = AppRotationType::ROTATION_180;
            break;
        case LayerRotateType::LAYER_ROTATE_90:
            contentRotation = AppRotationType::ROTATION_270;
            break;
        default:
            contentRotation = AppRotationType::ROTATION_BUTT;
            break;
    }
    return contentRotation;
}

LayerRotateType RotationManager::ConvertToLayerRotation(AppRotationType rotationType)
{
    LayerRotateType layerRotation = LayerRotateType::LAYER_ROTATE_NONE;
    switch (rotationType) {
        case AppRotationType::ROTATION_0:
            layerRotation = LayerRotateType::LAYER_ROTATE_NONE;
            break;
        case AppRotationType::ROTATION_90:
            layerRotation = LayerRotateType::LAYER_ROTATE_270;
            break;
        case AppRotationType::ROTATION_180:
            layerRotation = LayerRotateType::LAYER_ROTATE_180;
            break;
        case AppRotationType::ROTATION_270:
            layerRotation = LayerRotateType::LAYER_ROTATE_90;
            break;
        default:
            layerRotation = LayerRotateType::LAYER_ROTATE_BUTT;
            break;
    }
    return layerRotation;
}

bool RotationManager::IsSupportRotation(uint16_t sliceId, uint16_t pageId, AppRotationType displayRotation) const
{
    uint32_t id = sliceId | (pageId << PAGE_REGISTRATION_OFFSET);
    auto iter1 = pageSupportRotationsMap_.find(id);
    if (iter1 == pageSupportRotationsMap_.end()) {
        iter1 = pageSupportRotationsMap_.find(sliceId);
    }
    return iter1 != pageSupportRotationsMap_.end() ? (static_cast<uint8_t>(displayRotation) & iter1->second) != 0
                                                   : displayRotation == AppRotationType::ROTATION_0;
}

void RotationManager::UpdateLayerRotation(LayerRotateType relativeAngle) const
{
    if (relativeAngle == LayerRotateType::LAYER_ROTATE_BUTT) {
        GRAPHIC_LOGE("update rotation angle is wrong!");
        return;
    }

    LayerRotateType cur = defaultLayerRotation_;
    if (cur == LayerRotateType::LAYER_ROTATE_BUTT) {
        GRAPHIC_LOGE("current layer rotation angle is wrong!");
        return;
    }

    int a0 = static_cast<int>(cur);
    int a1 = static_cast<int>(relativeAngle);
    int mod = 4;
    LayerRotateType rot = static_cast<LayerRotateType>((a0 + a1) % mod);
    DisplayDev::GetInstance()->SetRotation(rot);
}
} // namespace OHOS
