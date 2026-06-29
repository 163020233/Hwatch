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

#ifndef WEBSOCKET_MODULE_CALLBACK_H
#define WEBSOCKET_MODULE_CALLBACK_H

#include "jsi.h"
#include "net_websocket_type.h"
#include "http_errcode.h"
#include "websocket_common.h"

namespace OHOS {
namespace ACELite {

struct WsResult {
    int resCode;
    char* reason;
    char* data;
    size_t length;
    bool isBinary;
    int clientId;
    WsEventId eventId;
};

class WsCallback final : public MemoryHeap {
public:
    WsCallback() = default;
    ~WsCallback() = default;

    static void DispatchStateInfo(int resCode, const char* reason, WsEventId eventId, int clientId);
    static void DispatchMessage(const char *data, size_t length, bool isBinary, int clientId);

private:
    static void SendStateInfo(void *param);
    static void SendMessage(void *param);

private:
};

}  // namespace ACELite
}  // namespace OHOS

#endif