/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 * Description: RotationManager
 * Author:
 * Create: 2026-02
 */

#ifndef ROTATION_MANAGER_H
#define ROTATION_MANAGER_H

#include <unordered_map>
#include "gfx_utils/vector.h"
#include "hals/display_dev.h"

namespace OHOS {

enum class AppRotationType {
    ROTATION_0 = 1,
    ROTATION_90 = 1 << 1,
    ROTATION_180 = 1 << 2,
    ROTATION_270 = 1 << 3,
    ROTATION_BUTT = 0
};

typedef struct {
    uint32_t slice;
    uint32_t page;
    uint8_t supportRotations;
} RotationInfo;

class OnRotationChangedListener {
public:
    virtual ~OnRotationChangedListener() = default;
    virtual void OnRotationChanged() {}
};

class RotationManager : public HeapBase {
public:
    static RotationManager& GetInstance()
    {
        static RotationManager instance;
        return instance;
    }
    void RegisterRotationCapability(uint16_t sliceId, uint16_t pageId, uint8_t supportRotations);
    void SetDisplayRotation(AppRotationType rotation);
    bool UpdateDeviceRotation(uint16_t sliceId, uint16_t pageId);
    AppRotationType GetDisplayRotation() const;
    AppRotationType GetExpectedRotation(uint16_t sliceId, uint16_t pageId) const;
    AppRotationType GetCurContentRotation() const;
    void RegisterRotationListener(OnRotationChangedListener* listener);
    void UnRegisterRotationListener(OnRotationChangedListener* listener);

private:
    RotationManager();
    ~RotationManager();
    RotationManager(const RotationManager &) = delete;
    RotationManager &operator=(const RotationManager &) = delete;
    RotationManager(RotationManager &&) = delete;
    RotationManager &operator=(RotationManager &&) = delete;
    AppRotationType ConvertToContentRotation(LayerRotateType displayRotation) const;
    LayerRotateType ConvertToLayerRotation(AppRotationType rotationType);
    bool IsSupportRotation(uint16_t sliceId, uint16_t pageId, AppRotationType displayRotation) const;
    void UpdateLayerRotation(LayerRotateType relativeAngle) const;

    std::unordered_map<uint32_t, uint8_t> pageSupportRotationsMap_;
    AppRotationType displayRotation_ = AppRotationType::ROTATION_0;
    Graphic::Vector<OnRotationChangedListener*> listeners_;
    LayerRotateType curLayerRotation_ = LayerRotateType::LAYER_ROTATE_NONE;
    const LayerRotateType defaultLayerRotation_;
};

class RotationRegister : public HeapBase {
public:
    RotationRegister(uint16_t sliceId, uint16_t pageId, uint8_t supportRotations)
    {
        RotationManager::GetInstance().RegisterRotationCapability(sliceId, pageId, supportRotations);
    }
};
} // namespace OHOS

#define REG_SLICE_ROT_CAP(sliceId, rotation) \
    const static OHOS::RotationRegister (STATIC_VAR##ROT)(sliceId, 0, rotation)

#define REG_SLICE_PAGE_ROT_CAP(sliceId, pageId, rotation) \
    const static OHOS::RotationRegister (STATIC_VAR##ROT)(sliceId, pageId, rotation)
#endif // ROTATION_MANAGER_H