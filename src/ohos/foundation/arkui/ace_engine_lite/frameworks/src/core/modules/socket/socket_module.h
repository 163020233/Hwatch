/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef SOCKET_MODULE_H
#define SOCKET_MODULE_H

#include <map>
#include <string>
#include <mutex>
#include <memory>
#include <functional>

#include "jsi/jsi.h"
#include "js_async_work.h"
#include "non_copyable.h"

#include "lwip_wrapper.h"
#include "socket_module_client.h"

namespace OHOS {
namespace ACELite {
const int32_t ARGS_ONE = 1;
const int32_t ARGS_TWO = 2;
const int32_t ARGS_THREE = 3;
const int32_t ARGS_FOUR = 4;

class SocketModule {
public:
    static JSIValue CreateSocket(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue Socket(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue Connect(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue Send(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue Recv(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue Close(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue GetAddrInfo(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue Destroy(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static void OnTerminate();
    // 回调处理
    static void CheckSocketCallback(SocketAsyncParams *asyncParams);

    // 工具函数
    static bool ParseSockAddr(const JSIValue &addrObj, struct sockaddr_in *addr);

private:
    // Socket客户端管理
    static std::map<JSIValue, void *> socketClients_;
    static std::mutex socketClientsMutex_;
    static constexpr int MAX_SOCKET_CLIENTS = 16;

    // 异步工作函数
    static void SocketAsyncExecute(void *data);

    // 清理函数
    static void CleanupAsyncParams(SocketAsyncParams *params);

    // 辅助函数
    static SocketMessage *CreateGetAddrInfoMessage(
        char *hostname, char *service, JSIValue thisVal, JSIValue callback);
    static void CleanupGetAddrInfoResources(SocketMessage *message, char *hostname, char *service);
    static void StoreClientReference(JSIValue socketContext, SocketModuleClient *client);
    static void SetNamedPropertyIfValid(JSIValue object, const char *name, JSIValue value);
    static void BindSocketMethods(JSIValue socketContext);
    static JSIValue CreateSocketContext(SocketModuleClient *client);
    static SocketModuleClient *CreateAndInitializeClient();
    static void CleanupExistingClient(const JSIValue thisVal);
    static bool CheckClientLimit();
    static void CleanupSendMessage(SocketMessage *message);
    static JSIValue SendSocketMessage(SocketModuleClient *client, SocketMessage *message);
    static bool InitializeSendMessage(
        SocketMessage *message, unsigned char *data, size_t dataLen, JSIValue callback, const JSIValue thisVal);
    static SocketMessage *CreateSendMessage(
        unsigned char *data, size_t dataLen, JSIValue callback, const JSIValue thisVal);
    static JSIValue CreateAndSendMessage(
        unsigned char *data, size_t dataLen, JSIValue callback, void *clientPtr, const JSIValue thisVal);
    static bool ValidateSendParameters(const JSIValue *args, uint8_t argsSize);
};

// 模块初始化
void InitSocketClientModule(JSIValue exports);
}  // namespace ACELite
}  // namespace OHOS
#endif  // SOCKET_MODULE_H