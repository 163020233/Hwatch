/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: lite_gesture_device.h
 * Author: Hisi Graphic Team
 * Created: 2025-10
 */

#ifndef GRAPHIC_LITE_LITE_GESTURE_DEVICE_H
#define GRAPHIC_LITE_LITE_GESTURE_DEVICE_H
#include "dock/gesture_input_device.h"
#include "dock/lite_gesture_device.h"

namespace OHOS {
class LiteGestureDevice : public GestureInputDevice {
public:
    LiteGestureDevice() {}
    ~LiteGestureDevice() override {}

    bool Read(DeviceData& data) override;
    bool IsEventEmpty() override;
};
}
#endif
