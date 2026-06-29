/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef WEBSOCKET_COMMON_H
#define WEBSOCKET_COMMON_H

#include "ace_mem_base.h"

namespace OHOS {
namespace ACELite {

#define net_malloc ace_malloc
#define net_free ace_free

enum WsEventId {
    WS_EVENT_OPEN = 0,
    WS_EVENT_MESSAGE = 1,
    WS_EVENT_CLOSE = 2,
    WS_EVENT_ERROR = 3,
    WS_EVENT_MAX = 4
};

}  // namespace ACELite
}  // namespace OHOS

#endif