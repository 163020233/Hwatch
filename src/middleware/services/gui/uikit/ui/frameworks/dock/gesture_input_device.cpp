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

#include "components/root_view.h"
#include "gfx_utils/graphic_log.h"
#include "dock/gesture_input_device.h"
#ifdef SUPPORT_OHOSFWK
#include "modules/keyinput/keyinput.h"
#include "graphic_service.h"
#endif

namespace OHOS {
#ifdef SUPPORT_OHOSFWK
static void DispatchEventToKeyinputGesture(const GestureEvent &event)
{
    if (!GraphicService::GetInstance()->IsNativeRunning()) {
        OHOS::ACELite::GestureListener::GetInstance()->OnGestureEvent(event);
    }
}
#endif

void GestureInputDevice::DispatchEvent(const DeviceData& data)
{
    if (data.validFlag == 1 && data.activationFlag == 1 && data.pred < 4) {
        GestureType type;
        if (data.pred == 0) {
            type = GestureType::FLIP_WRIST_ONCE;
        } else if (data.pred == 1) {
            type = GestureType::FLIP_WRIST_TWICE;
        } else if (data.pred == 2) {
            type = GestureType::FIST_ONCE;
        } else if (data.pred == 3) {
            type = GestureType::FIST_TWICE;
        } else {
            return;
        }
        GestureEvent event(type);
        RootView::GetInstance()->OnGestureEvent(event);
#ifdef SUPPORT_OHOSFWK
        DispatchEventToKeyinputGesture(event);
#endif
    }
}
}  // namespace OHOS
