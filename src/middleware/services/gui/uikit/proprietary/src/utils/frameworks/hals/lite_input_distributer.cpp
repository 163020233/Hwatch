/*
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies Co., Ltd. All rights reserved
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

#include "hals/lite_input_distributer.h"
#include "hals/display_dev.h"
#include "gfx_utils/graphic_log.h"
#include "dfx/dfx_record.h"
#include "common/low_power_manager.h"
#ifdef CONFIG_SUPPORTED_POWERMGR
#include "power_mgr_policy.h"
#include "power_mgr_service.h"
#include "samgr_lite.h"
#include "commu_feature.h"
#endif
namespace OHOS {
namespace {
static const uint8_t MAX_ENCODER_LIST_SIZE = 2;
}
static DeviceData g_lastData = {{{0}}};
LiteInputDistributer::LiteInputDistributer()
    : deviceData_({}) {}

LiteInputDistributer::~LiteInputDistributer() {}

void LiteInputDistributer::SendRawEvent(const RawEvent& rawEvent)
{
#if !defined(_WIN32)
    RawEvent event = rawEvent;
    if (event.type == InputDevType::INDEV_TYPE_TOUCH) {
        LayerRotateType rotateType = DisplayDev::GetInstance()->GetLayerRotateType(LAYER_0);
        if (rotateType == LayerRotateType::LAYER_ROTATE_90) {
            event.x = rawEvent.y;
            event.y = LAYER_RESOLUTION_WIDTH - rawEvent.x;
        } else if (rotateType == LayerRotateType::LAYER_ROTATE_180) {
            event.x = LAYER_RESOLUTION_WIDTH - rawEvent.x;
            event.y = LAYER_RESOLUTION_HEIGHT - rawEvent.y;
        } else if (rotateType == LayerRotateType::LAYER_ROTATE_270) {
            event.x = LAYER_RESOLUTION_HEIGHT - rawEvent.y;
            event.y = rawEvent.x;
        }
    }
#ifdef CONFIG_SUPPORTED_POWERMGR
    SendEventToPM(event);
#endif
    SetEventData(event);
#endif
}

void LiteInputDistributer::GetEventData(DeviceData& data, InputDevType type)
{
    eventLock_.Lock();
    if (type == InputDevType::INDEV_TYPE_TOUCH) {
        if (IsShowPoint() && (deviceData_.timestamp != g_lastData.timestamp)) {
            DfxTPGetEvent();
        }
        g_lastData = deviceData_;
        DeviceData curData;
        if (!tpList_.empty()) {
            curData = tpList_.front();
            tpList_.pop_front();
        } else {
            curData = deviceData_;
        }
        data.point.x = curData.point.x;
        data.point.y = curData.point.y;
        data.state = curData.state;
        data.timestamp = curData.timestamp;

        eventLock_.Unlock();
        return;
    }

    if (type == InputDevType::INDEV_TYPE_KEY) {
        if (keyList_.empty()) {
            data.keyId = INVALID_KEY_ID;
            eventLock_.Unlock();
            return;
        }
        DeviceData tmp = keyList_.front();
        keyList_.pop_front();
        data.keyId = tmp.keyId;
        data.state = tmp.state;
        data.timestamp = tmp.timestamp;
    }

    if (type == InputDevType::INDEV_TYPE_ENCODER) {
        if (encoderList_.empty()) {
            data.rotate = 0;
            eventLock_.Unlock();
            return;
        }
        DeviceData tmp = encoderList_.front();
        encoderList_.pop_front();
        data.rotate = tmp.rotate;
    }

    if (type == InputDevType::INDEV_TYPE_GESTURE) {
        if (gestureList_.empty()) {
            eventLock_.Unlock();
            return;
        }
        DeviceData gestureTmp = gestureList_.front();
        gestureList_.pop_front();
        data.validFlag = gestureTmp.validFlag;
        data.activationFlag = gestureTmp.activationFlag;
        data.pred = gestureTmp.pred;
        data.timestamp = gestureTmp.timestamp;
        data.state = gestureTmp.state;
    }
    eventLock_.Unlock();
}

void LiteInputDistributer::SetEventData(const RawEvent& event)
{
#if ENABLE_DYNAMIC_FRAME_RATE
    LowPowerManager::GetInstance()->ExitLowPower();
#endif
    eventLock_.Lock();
    if (event.type == InputDevType::INDEV_TYPE_TOUCH) {
        if (IsShowPoint()) {
            DfxTPSetEvent(event.timestamp - deviceData_.timestamp);
        }
        deviceData_.point.x = event.x;
        deviceData_.point.y = event.y;
        deviceData_.state = event.state;
        deviceData_.timestamp = event.timestamp;
        while (tpList_.size() >= MAX_TP_LIST_SIZE) {
            tpList_.pop_front();
        }
        tpList_.push_back(deviceData_);
        eventLock_.Unlock();
        return;
    }
    if (event.type == InputDevType::INDEV_TYPE_KEY) {
        DeviceData data;
        data.keyId = event.keyId;
        data.state = event.state;
        data.timestamp = event.timestamp;
        while (keyList_.size() >= MAX_KEY_LIST_SIZE) {
            keyList_.pop_front();
        }
        keyList_.push_back(data);
    }

    if (event.type == InputDevType::INDEV_TYPE_GESTURE) {
        DeviceData gestureData;
        gestureData.validFlag = event.validFlag;
        gestureData.pred = event.pred;
        gestureData.activationFlag = event.activationFlag;
        gestureData.state = event.state;
        gestureData.timestamp = event.timestamp;
        while (gestureList_.size() >= MAX_GESTURE_LIST_SIZE) {
            gestureList_.pop_front();
        }
        gestureList_.push_back(gestureData);
    }

    if (event.type == InputDevType::INDEV_TYPE_ENCODER) {
        DeviceData data;
        data.rotate = event.rotate;
        while (encoderList_.size() >= MAX_ENCODER_LIST_SIZE) {
            encoderList_.pop_front();
        }
        encoderList_.push_back(data);
    }
    eventLock_.Unlock();
}

bool LiteInputDistributer::IsEventEmpty(InputDevType type)
{
    if (type == InputDevType::INDEV_TYPE_TOUCH) {
        return ((g_lastData.state == 0) && (deviceData_.state == 0) && (tpList_.empty()));
    } else if (type == InputDevType::INDEV_TYPE_KEY) {
        return keyList_.empty();
    } else if (type == InputDevType::INDEV_TYPE_ENCODER) {
        return encoderList_.empty();
    } else if (type == InputDevType::INDEV_TYPE_GESTURE) {
        return gestureList_.empty();
    }
    return true;
}

void LiteInputDistributer::ClearEvent(InputDevType type)
{
    eventLock_.Lock();
    if (type == InputDevType::INDEV_TYPE_TOUCH) {
        tpList_.clear();
        deviceData_.state = 0;
    } else if (type == InputDevType::INDEV_TYPE_KEY) {
        keyList_.clear();
    } else if (type == InputDevType::INDEV_TYPE_ENCODER) {
        encoderList_.clear();
    } else if (type == InputDevType::INDEV_TYPE_GESTURE) {
        gestureList_.clear();
    }
    eventLock_.Unlock();
}

#ifdef CONFIG_SUPPORTED_POWERMGR
void LiteInputDistributer::SendEventToPM(const RawEvent& event)
{
    IUnknown *api = SAMGR_GetInstance()->GetDefaultFeatureApiById(POWER_SVR_ID);
    if (api == nullptr) {
        return;
    }

    uint16_t msgId = POWERMSG_USER_EVENT;
    uint8_t userEvent;
    if (event.type == InputDevType::INDEV_TYPE_TOUCH) {
        userEvent = USER_EVENT_TOUCH;
    }
    if (event.type == InputDevType::INDEV_TYPE_KEY) {
        userEvent = USER_EVENT_KEY;
    }
    uint32_t msgValue = (POWER_DISPLAY_POLICY << EVENT_TYPE_BITS_SIZE) | userEvent;

    Request *request = nullptr;
    request = SAMGR_AllocRequestMsg(0, CORES_APPLICATION_CORE, POWER_SVR_ID, 0);
    if (request == nullptr) {
        return;
    }
    request->serviceId = UI_SVR_ID;
    request->msgId = msgId;
    request->msgValue = msgValue;

    CommuFeature *feature = GET_OBJECT(api, CommuFeature, iUnknown);
    if (feature == nullptr) {
        SAMGR_FreeRequestMsg(request);
        return;
    }
    Identity identity = feature->id;
    identity.featureId = -1;
    int32 ret = SAMGR_SendRequest(&identity, request, nullptr);
    if (ret != 0) {
        SAMGR_FreeRequestMsg(request);
        return;
    }
}
#endif
}
