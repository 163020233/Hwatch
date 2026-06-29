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

#ifndef WEBSOCKET_CLIENT_MODULE_H
#define WEBSOCKET_CLIENT_MODULE_H

#include <stdint.h>
#include "jsi.h"
#include <map>
#include <cstring>
#include "websocket_common.h"
#include "websocket_module_callback.h"

namespace OHOS {
namespace ACELite {

struct EventCallback {
    JSIValue callback;
    JSIValue context;
};

class WebSocketModule final : public MemoryHeap {
public:
    WebSocketModule() = default;
    ~WebSocketModule() = default;
    static JSIValue CreateWebSocket(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue Connect(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue Send(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue SendInit(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue SendUpdate(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue SendFinish(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue Close(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue Destroy(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue On(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue Off(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static std::map<std::string, EventCallback *> eventCallbacks;
    static void OnTerminate();
private:
    static WsEventId GetEventId(const char *event);
    static char *HeaderJsonToString(char* headerJson);
};
void InitWebSocketClientModule(JSIValue exports);

}  // namespace ACELite
}  // namespace OHOS

#endif  // WEBSOCKET_CLIENT_MODULE_H