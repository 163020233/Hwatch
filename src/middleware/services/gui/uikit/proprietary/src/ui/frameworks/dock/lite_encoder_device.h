/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: encoder event device
 * Author: Hisi Graphic Team
 * Created: 2025-08
 */

#ifndef GRAPHIC_LITE_ENCODER_DEVICE_H
#define GRAPHIC_LITE_ENCODER_DEVICE_H
#include "dock/rotate_input_device.h"

namespace OHOS {
class LiteEncoderDevice : public RotateInputDevice {
public:
    LiteEncoderDevice() {}
    ~LiteEncoderDevice() override {}

    bool Read(DeviceData& data) override;
    bool IsEventEmpty() override;
};
}
#endif