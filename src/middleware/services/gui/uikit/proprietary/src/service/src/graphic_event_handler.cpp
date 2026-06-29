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

#include "graphic_event_handler.h"
#include "dfx/dfx_record.h"
#include "dfx/dfx_frame_trace.h"
#include "common/low_power_manager.h"
#include "gfx_utils/graphic_log.h"

namespace OHOS {
GraphicEventHandler::GraphicEventHandler() {}

GraphicEventHandler::~GraphicEventHandler() {}

void GraphicEventHandler::Run()
{
    while (true) {
        queueMutex_.Lock();
        while (eventQueue_.empty()) {
#if ENABLE_DYNAMIC_FRAME_RATE
            LowPowerManager::GetInstance()->TryToEnterLowPower();
#endif
            pthreadCond_.Wait(queueMutex_);
        }
        GraphicEvent event = std::move(eventQueue_.front());
        eventQueue_.pop();
        queueMutex_.Unlock();
        EVENT_TRACE_START();
        event();
        EVENT_TRACE_END();
        DfxRecordShow();
    }
}

void GraphicEventHandler::PostGraphicEvent(const GraphicEvent& event, bool exitLowPower)
{
    queueMutex_.Lock();
#if ENABLE_DYNAMIC_FRAME_RATE
    if (exitLowPower) {
        eventQueue_.push([event]() {
            LowPowerManager::GetInstance()->ExitLowPowerInner();
            event();
        });
    } else {
#endif
        eventQueue_.push(event);
#if ENABLE_DYNAMIC_FRAME_RATE
    }
#endif
    pthreadCond_.Signal();
    queueMutex_.Unlock();
}

void GraphicEventHandler::ClearGraphicEvent()
{
    queueMutex_.Lock();
    GRAPHIC_LOGP("ClearGraphicEvent! EventQueue's size: %u", eventQueue_.size());
    while (!eventQueue_.empty()) {
        eventQueue_.pop();
    }
    queueMutex_.Unlock();
}
} // namespace OHOS
