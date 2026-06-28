/*
 * Copyright (c) 2020-2021 HiSilicon (Shanghai) Technologies Co., Ltd.
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

#ifndef GRAPHIC_LITE_GESTURE_INPUT_DEVICE_H
#define GRAPHIC_LITE_GESTURE_INPUT_DEVICE_H

#include "dock/input_device.h"

namespace OHOS {
/** @brief A Gesture input device. */
class GestureInputDevice : public InputDevice {
public:
    /**
     * @brief Constructor
     */
    GestureInputDevice() {}
    /** @brief Destructor */
    virtual ~GestureInputDevice() {}
    void ClearGestureViews() {}
protected:
    void DispatchEvent(const DeviceData& data) override;
private:
};
}; // namespace OHOS
#endif // GRAPHIC_LITE_GESTURE_INPUT_DEVICE_H
