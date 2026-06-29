/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: lite_gesture_device.cpp
 * Author: Hisi Graphic Team
 * Created: 2025-10
 */
#include "dock/gesture_input_device.h"
#include "gfx_utils/input_event_info.h"
#include "lite_input_distributer.h"
#include "lite_gesture_device.h"

namespace OHOS {

bool LiteGestureDevice::Read(DeviceData& data)
{
    (void)LiteInputDistributer::GetInstance()->GetEventData(data, InputDevType::INDEV_TYPE_GESTURE);
    return false;
}

bool LiteGestureDevice::IsEventEmpty()
{
    return LiteInputDistributer::GetInstance()->IsEventEmpty(InputDevType::INDEV_TYPE_GESTURE);
}
}
