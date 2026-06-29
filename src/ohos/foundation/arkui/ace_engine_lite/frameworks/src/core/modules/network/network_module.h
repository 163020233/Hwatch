/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_ACELITE_NETWORK_MODULE_H
#define OHOS_ACELITE_NETWORK_MODULE_H

#include <map>
#include "non_copyable.h"
#include "jsi.h"
#include "js_async_work.h"

namespace OHOS {
namespace ACELite {
#define MAX_STATUS_LENGTH 128

struct NetParam {
    uint8_t net_type = -1;
    uint8_t state;
};

struct NetCallBack {
    JSIValue net_connect_callback = nullptr;
    JSIValue net_connect_thisVal = nullptr;
    NetParam netParam;
};

class NetWorkModule final : public MemoryHeap {
public:
    ACE_DISALLOW_COPY_AND_MOVE(NetWorkModule);
    NetWorkModule() = default;
    ~NetWorkModule() = default;
    static std::map<uint8_t, NetCallBack*> callbackSet;
    static JSIValue RegisterConnectListener(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue UnregisterConnectListener(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue GetConnectState(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue OpenBtPan(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue CloseBtPan(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue GetIpAddress(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static void AsyncExecuteCallback(void *arg);
    static void TriggerCallbackListener(uint8_t type, uint8_t netConnectState);
    static void OnTerminate();
    static void Clean(NetCallBack* callbackToClean);
};

}  // namespace ACELite
}  // namespace OHOS

#endif
