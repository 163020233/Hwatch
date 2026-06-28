/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: encoder  event device
 * Author: Hisi Graphic Team
 * Created: 2025-08
 */

#include "lite_encoder_device.h"
#include "gfx_utils/input_event_info.h"
#include "lite_input_distributer.h"

namespace OHOS {
bool LiteEncoderDevice::Read(DeviceData& data)
{
    (void)LiteInputDistributer::GetInstance()->GetEventData(data, InputDevType::INDEV_TYPE_ENCODER);
    return false;
}

bool LiteEncoderDevice::IsEventEmpty()
{
    return LiteInputDistributer::GetInstance()->IsEventEmpty(InputDevType::INDEV_TYPE_ENCODER);
}
}