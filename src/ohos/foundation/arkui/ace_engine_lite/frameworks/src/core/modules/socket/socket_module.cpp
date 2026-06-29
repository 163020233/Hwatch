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

#include "socket_module.h"
#include "ace_log.h"
#include "securec.h"
#include <cstring>
#include <map>
#include <algorithm>
#include <cctype>

namespace OHOS {
namespace ACELite {

std::map<JSIValue, void *> SocketModule::socketClients_;
std::mutex SocketModule::socketClientsMutex_;
constexpr size_t MAX_SEND_DATA_LEN = 20 * 1024;

// 清理异步参数
void SocketModule::CleanupAsyncParams(SocketAsyncParams *params)
{
    if (params == nullptr) {
        return;
    }
    delete params;
}

// 异步执行函数
void SocketModule::SocketAsyncExecute(void *data)
{
    SocketAsyncParams *params = static_cast<SocketAsyncParams *>(data);
    if (params == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "SocketAsyncExecute: params is null!");
        return;
    }

    // 检查回调是否有效
    bool callbackValid = !JSI::ValueIsUndefined(params->callback);
    bool contextValid = !JSI::ValueIsUndefined(params->context);
    if (callbackValid && contextValid) {
        // 准备参数数组：JS回调期望 (error, result) 格式
        JSIValue argv[ARGS_TWO];
        uint8_t argc = 0;

        // 第一个参数：错误对象
        if (!JSI::ValueIsUndefined(params->err)) {
            argv[0] = params->err;
            argc = 1;
        } else {
            argv[0] = JSI::CreateUndefined();
            argc = 1;
        }

        // 第二个参数：结果对象
        if (!JSI::ValueIsUndefined(params->result)) {
            argv[1] = params->result;
            argc = ARGS_TWO;
        } else {
            argv[1] = JSI::CreateUndefined();
            argc = ARGS_TWO;
        }

        // 调用JS回调
        JSI::CallFunction(params->callback, params->context, argv, argc);

        // 释放创建的值
        JSI::ReleaseValueList(params->err, params->result, params->callback, params->context);
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE,
            "Cannot call JS callback: callback valid=%d, context valid=%d", callbackValid,
            contextValid);
    }

    // 清理资源
    CleanupAsyncParams(params);
}

// 检查并执行Socket回调
void SocketModule::CheckSocketCallback(SocketAsyncParams *asyncParams)
{
    if (asyncParams == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "CheckSocketCallback: asyncParams is null!");
        return;
    }

    // 分发到JS线程执行
    if (!JsAsyncWork::DispatchAsyncWork(SocketAsyncExecute, static_cast<void *>(asyncParams))) {
        HILOG_ERROR(HILOG_MODULE_ACE, "DispatchAsyncWork failed!");
        CleanupAsyncParams(asyncParams);
    }
}

// 检查客户端数量限制
bool SocketModule::CheckClientLimit()
{
    std::lock_guard<std::mutex> lock(socketClientsMutex_);
    if (socketClients_.size() >= MAX_SOCKET_CLIENTS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "The number of Socket clients has exceeded the limit: %d", MAX_SOCKET_CLIENTS);
        return false;
    }
    return true;
}

// 清理已存在的客户端
void SocketModule::CleanupExistingClient(const JSIValue thisVal)
{
    std::lock_guard<std::mutex> lock(socketClientsMutex_);
    auto it = socketClients_.find(thisVal);
    if (it != socketClients_.end()) {
        HILOG_WARN(HILOG_MODULE_ACE, "Socket client already exists, destroying old one");
        SocketModuleClient *client = static_cast<SocketModuleClient *>(it->second);
        if (client != nullptr) {
            client->CloseSocketThread();
            delete client;
        }
        socketClients_.erase(it);
    }
}

// 创建并初始化客户端
SocketModuleClient *SocketModule::CreateAndInitializeClient()
{
    SocketModuleClient *client = new (std::nothrow) SocketModuleClient();
    if (client == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Create SocketModuleClient failed!");
        return nullptr;
    }

    if (!client->CreateSocketThread()) {
        delete client;
        HILOG_ERROR(HILOG_MODULE_ACE, "CreateSocketThread failed!");
        return nullptr;
    }

    return client;
}

// 创建Socket上下文对象
JSIValue SocketModule::CreateSocketContext(SocketModuleClient *client)
{
    JSIValue socketContext = JSI::CreateObject();
    if (JSI::ValueIsUndefined(socketContext)) {
        delete client;
        HILOG_ERROR(HILOG_MODULE_ACE, "Create socket context object failed!");
        return JSI::CreateUndefined();
    }

    BindSocketMethods(socketContext);
    StoreClientReference(socketContext, client);

    return socketContext;
}

// 绑定Socket方法
void SocketModule::BindSocketMethods(JSIValue socketContext)
{
    JSIValue socketFunc = JSI::CreateFunction(Socket);
    JSIValue connectFunc = JSI::CreateFunction(Connect);
    JSIValue sendFunc = JSI::CreateFunction(Send);
    JSIValue recvFunc = JSI::CreateFunction(Recv);
    JSIValue closeFunc = JSI::CreateFunction(Close);
    JSIValue getAddrInfoFunc = JSI::CreateFunction(GetAddrInfo);
    JSIValue destroyFunc = JSI::CreateFunction(Destroy);

    SetNamedPropertyIfValid(socketContext, "socket", socketFunc);
    SetNamedPropertyIfValid(socketContext, "connect", connectFunc);
    SetNamedPropertyIfValid(socketContext, "send", sendFunc);
    SetNamedPropertyIfValid(socketContext, "recv", recvFunc);
    SetNamedPropertyIfValid(socketContext, "close", closeFunc);
    SetNamedPropertyIfValid(socketContext, "getaddrinfo", getAddrInfoFunc);
    SetNamedPropertyIfValid(socketContext, "destroy", destroyFunc);
}

void SocketModule::OnTerminate()
{
    HILOG_INFO(HILOG_MODULE_ACE, "SocketModule::OnTerminate called");

    // 清理所有socket客户端
    std::lock_guard<std::mutex> lock(socketClientsMutex_);

    for (auto it = socketClients_.begin(); it != socketClients_.end();) {
        JSIValue context = it->first;
        SocketModuleClient *client = static_cast<SocketModuleClient *>(it->second);

        if (client != nullptr) {
            HILOG_INFO(HILOG_MODULE_ACE, "Cleaning up socket client for context");
            client->CloseSocketThread();
            delete client;
        }

        // 释放JSI上下文值
        if (!JSI::ValueIsUndefined(context)) {
            JSI::ReleaseValue(context);
        }

        it = socketClients_.erase(it);
    }

    HILOG_INFO(HILOG_MODULE_ACE, "SocketModule::OnTerminate completed, all clients cleaned up");
}

// 设置命名属性（如果有效）
void SocketModule::SetNamedPropertyIfValid(JSIValue object, const char *name, JSIValue value)
{
    if (!JSI::ValueIsUndefined(value)) {
        JSI::SetNamedProperty(object, name, value);
    }
}

// 存储客户端引用
void SocketModule::StoreClientReference(JSIValue socketContext, SocketModuleClient *client)
{
    std::lock_guard<std::mutex> lock(socketClientsMutex_);
    socketClients_[socketContext] = client;
    HILOG_INFO(HILOG_MODULE_ACE, "CreateSocket success, total clients: %zu", socketClients_.size());
}

// CreateSocket API
JSIValue SocketModule::CreateSocket(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    if (!CheckClientLimit()) {
        return JSI::CreateUndefined();
    }

    CleanupExistingClient(thisVal);
    SocketModuleClient *client = CreateAndInitializeClient();
    if (client == nullptr) {
        return JSI::CreateUndefined();
    }

    return CreateSocketContext(client);
}

// Socket API - 创建socket
JSIValue SocketModule::Socket(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    if (args == nullptr || argsSize < ARGS_ONE) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid parameters, argsSize=%d", argsSize);
        return JSI::CreateUndefined();
    }

    std::lock_guard<std::mutex> lock(socketClientsMutex_);
    auto it = socketClients_.find(thisVal);
    if (it == socketClients_.end()) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Socket context not found");
        return JSI::CreateUndefined();
    }

    char *type  = JSI::ValueToString(args[0]);
    if (type == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid type parameters");
        return JSI::CreateUndefined();
    }
    JSIValue callback = (argsSize > ARGS_ONE) ? args[ARGS_ONE] : JSI::CreateUndefined();

    SocketMessage *message = new (std::nothrow) SocketMessage();
    if (message == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Allocate SocketMessage failed!");
        if (!JSI::ValueIsUndefined(callback)) {
            JSI::ReleaseValue(callback);
        }
        JSI::ReleaseString(type);
        return JSI::CreateUndefined();
    }

    message->messageId = SOCKET_CREATE;
    message->createParams.domain = AF_INET;
    if (strcmp(type, "UDP") == 0) {
        message->createParams.type = SOCK_DGRAM;
        message->createParams.protocol = IPPROTO_UDP;
    } else if (strcmp(type, "TCP") == 0) {
        message->createParams.type = SOCK_STREAM;
        message->createParams.protocol = IPPROTO_IP;
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid type parameters");
        JSI::ReleaseString(type);
        delete message;
        return JSI::CreateUndefined();
    }
    message->context = JSI::AcquireValue(thisVal);
    message->callback = JSI::AcquireValue(callback);
    message->result = -1;
    message->bytesResult = -1;
    message->addrInfoResult = nullptr;
    message->errorMsg = nullptr;

    SocketModuleClient *client = static_cast<SocketModuleClient *>(it->second);
    int32_t ret = client->SocketMessagePut(message);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "SocketMessagePut failed! ret=%d", ret);
        if (!JSI::ValueIsUndefined(message->callback)) {
            JSI::ReleaseValue(message->callback);
        }
        if (!JSI::ValueIsUndefined(message->context)) {
            JSI::ReleaseValue(message->context);
        }
        delete message;
    }

    if (argsSize <= ARGS_TWO && !JSI::ValueIsUndefined(callback)) {
        JSI::ReleaseValue(callback);
    }
    JSI::ReleaseString(type);
    return JSI::CreateUndefined();
}

// Connect API
JSIValue SocketModule::Connect(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    if (args == nullptr || argsSize < ARGS_TWO) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid parameters, argsSize=%d", argsSize);
        return JSI::CreateUndefined();
    }

    std::lock_guard<std::mutex> lock(socketClientsMutex_);
    auto it = socketClients_.find(thisVal);
    if (it == socketClients_.end()) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Socket context not found");
        return JSI::CreateUndefined();
    }

    JSIValue addrObj = args[0];
    JSIValue callback = args[ARGS_ONE];

    struct sockaddr_in addr;
    (void)memset_s(&addr, sizeof(addr), 0, sizeof(addr));
    if (!ParseSockAddr(addrObj, &addr)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Parse socket address failed");
        return JSI::CreateUndefined();
    }

    SocketMessage *message = new (std::nothrow) SocketMessage();
    if (message == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Allocate SocketMessage failed!");
        return JSI::CreateUndefined();
    }

    message->messageId = SOCKET_CONNECT;
    int32_t ret = memcpy_s(&message->connectParams.addr, sizeof(struct sockaddr_in), &addr, sizeof(struct sockaddr_in));
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "memcpy_s failed! ret=%d", ret);
        delete message;
        return JSI::CreateUndefined();
    }
    message->context = JSI::AcquireValue(thisVal);
    message->callback = JSI::AcquireValue(callback);
    message->result = -1;
    message->bytesResult = -1;
    message->addrInfoResult = nullptr;
    message->errorMsg = nullptr;

    SocketModuleClient *client = static_cast<SocketModuleClient *>(it->second);
    ret = client->SocketMessagePut(message);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "SocketMessagePut failed! ret=%d", ret);
        if (!JSI::ValueIsUndefined(message->callback)) {
            JSI::ReleaseValue(message->callback);
        }
        if (!JSI::ValueIsUndefined(message->context)) {
            JSI::ReleaseValue(message->context);
        }
        delete message;
    }

    return JSI::CreateUndefined();
}

// 验证发送参数
bool SocketModule::ValidateSendParameters(const JSIValue *args, uint8_t argsSize)
{
    if (args == nullptr || argsSize < ARGS_TWO) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid parameters, argsSize=%d", argsSize);
        return false;
    }
    return true;
}
// Send API - 仅支持二进制数据
JSIValue SocketModule::Send(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    if (!ValidateSendParameters(args, argsSize)) {
        return JSI::CreateUndefined();
    }

    std::lock_guard<std::mutex> lock(socketClientsMutex_);
    auto it = socketClients_.find(thisVal);
    if (it == socketClients_.end() || it->second == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Socket context not found or client is null");
        return JSI::CreateUndefined();
    }

    JSIValue callback = args[ARGS_ONE];
    JSIValue typedArray = args[0];
    if (!JSI::ValueIsTypedArray(typedArray)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Input must be typed Array");
        return JSI::CreateUndefined();
    }
    TypedArrayType type;
    size_t dataLen = 0;
    JSIValue arrayBuffer;
    size_t bytesOffset = 0;

    unsigned char *data = JSI::GetTypedArrayInfo(typedArray, type, dataLen, arrayBuffer, bytesOffset);

    if (type != TypedArrayType::JSI_UINT8_ARRAY) {
        HILOG_ERROR(HILOG_MODULE_ACE, "TypedArray must be of type Uint8Array!");
        JSI::ReleaseValueList(typedArray, arrayBuffer);
        return JSI::CreateUndefined();
    }

    if (data == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Send data is null");
        JSI::ReleaseValueList(typedArray, arrayBuffer);
        return JSI::CreateUndefined();
    }

    if (dataLen > MAX_SEND_DATA_LEN) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Send: dataLen=%zu, out of 20 KB.", dataLen);
        JSI::ReleaseValueList(typedArray, arrayBuffer);
        return JSI::CreateUndefined();
    }

    HILOG_INFO(HILOG_MODULE_ACE, "Send: dataLen=%zu", dataLen);

    // 创建并发送消息
    JSIValue result = CreateAndSendMessage(data, dataLen, callback, it->second, thisVal);
    JSI::ReleaseValueList(typedArray, arrayBuffer);
    typedArray = JSI::CreateUndefined();
    arrayBuffer = JSI::CreateUndefined();
    return result;
}

// 创建并发送消息
JSIValue SocketModule::CreateAndSendMessage(
    unsigned char *data, size_t dataLen, JSIValue callback, void *clientPtr, const JSIValue thisVal)
{
    SocketMessage *message = CreateSendMessage(data, dataLen, callback, thisVal);
    if (message == nullptr) {
        return JSI::CreateUndefined();
    }

    SocketModuleClient *client = static_cast<SocketModuleClient *>(clientPtr);
    return SendSocketMessage(client, message);
}

// 创建发送消息
SocketMessage *SocketModule::CreateSendMessage(
    unsigned char *data, size_t dataLen, JSIValue callback, const JSIValue thisVal)
{
    SocketMessage *message = new (std::nothrow) SocketMessage();
    if (message == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Allocate SocketMessage failed!");
        return nullptr;
    }

    if (!InitializeSendMessage(message, data, dataLen, callback, thisVal)) {
        delete message;
        return nullptr;
    }

    return message;
}

// 初始化发送消息
bool SocketModule::InitializeSendMessage(
    SocketMessage *message, unsigned char *data, size_t dataLen, JSIValue callback, const JSIValue thisVal)
{
    message->messageId = SOCKET_SEND;
    message->sendRecvParams.data = static_cast<unsigned char *>(malloc(dataLen));
    if (message->sendRecvParams.data == nullptr) {
        return false;
    }

    if (memcpy_s(message->sendRecvParams.data, dataLen, data, dataLen) != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "%s, memcpy fail\n", __FUNCTION__);
        return false;
    }

    message->sendRecvParams.dataLen = dataLen;
    message->context = JSI::AcquireValue(thisVal);
    message->callback = JSI::AcquireValue(callback);
    message->result = -1;
    message->bytesResult = 0;
    message->addrInfoResult = nullptr;
    message->errorMsg = nullptr;

    return true;
}

// 发送Socket消息
JSIValue SocketModule::SendSocketMessage(SocketModuleClient *client, SocketMessage *message)
{
    int ret = client->SocketMessagePut(message);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "SocketMessagePut failed! ret=%d", ret);
        CleanupSendMessage(message);
    }

    return JSI::CreateUndefined();
}

// 清理发送消息
void SocketModule::CleanupSendMessage(SocketMessage *message)
{
    if (message->sendRecvParams.data != nullptr) {
        free(message->sendRecvParams.data);
    }
    if (!JSI::ValueIsUndefined(message->callback)) {
        JSI::ReleaseValue(message->callback);
    }
    if (!JSI::ValueIsUndefined(message->context)) {
        JSI::ReleaseValue(message->context);
    }
    delete message;
}

// Recv API
JSIValue SocketModule::Recv(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    if (args == nullptr || argsSize < ARGS_ONE) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid parameters, argsSize=%d", argsSize);
        return JSI::CreateUndefined();
    }

    std::lock_guard<std::mutex> lock(socketClientsMutex_);
    auto it = socketClients_.find(thisVal);
    if (it == socketClients_.end() || it->second == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Socket context not found or client is null");
        return JSI::CreateUndefined();
    }

    JSIValue callback = args[0];
    SocketMessage *message = new (std::nothrow) SocketMessage();
    if (message == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Allocate SocketMessage failed!");
        return JSI::CreateUndefined();
    }

    message->messageId = SOCKET_RECV;
    message->context = JSI::AcquireValue(thisVal);
    message->callback = JSI::AcquireValue(callback);
    message->result = -1;
    message->bytesResult = 0;
    message->addrInfoResult = nullptr;
    message->errorMsg = nullptr;

    SocketModuleClient *client = static_cast<SocketModuleClient *>(it->second);
    int32_t ret = client->SocketMessagePut(message);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "SocketMessagePut failed! ret=%d", ret);
        if (message->sendRecvParams.data != nullptr) {
            free(message->sendRecvParams.data);
        }
        if (!JSI::ValueIsUndefined(message->callback)) {
            JSI::ReleaseValue(message->callback);
        }
        if (!JSI::ValueIsUndefined(message->context)) {
            JSI::ReleaseValue(message->context);
        }
        delete message;
    }

    return JSI::CreateUndefined();
}

// Close API
JSIValue SocketModule::Close(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    if (args == nullptr || argsSize < ARGS_ONE) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid parameters, argsSize=%d", argsSize);
        return JSI::CreateUndefined();
    }

    std::lock_guard<std::mutex> lock(socketClientsMutex_);
    auto it = socketClients_.find(thisVal);
    if (it == socketClients_.end()) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Socket context not found");
        return JSI::CreateUndefined();
    }

    JSIValue callback = args[0];

    SocketMessage *message = new (std::nothrow) SocketMessage();
    if (message == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Allocate SocketMessage failed!");
        return JSI::CreateUndefined();
    }

    message->messageId = SOCKET_CLOSE;
    message->context = JSI::AcquireValue(thisVal);
    message->callback = JSI::AcquireValue(callback);
    message->result = -1;
    message->bytesResult = -1;
    message->addrInfoResult = nullptr;
    message->errorMsg = nullptr;

    SocketModuleClient *client = static_cast<SocketModuleClient *>(it->second);
    int32_t ret = client->SocketMessagePut(message);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "SocketMessagePut failed! ret=%d", ret);
        if (!JSI::ValueIsUndefined(message->callback)) {
            JSI::ReleaseValue(message->callback);
        }
        if (!JSI::ValueIsUndefined(message->context)) {
            JSI::ReleaseValue(message->context);
        }
        delete message;
    }

    HILOG_INFO(HILOG_MODULE_ACE, "Close API exit");
    return JSI::CreateUndefined();
}

// 复制主机名到消息
static bool CopyHostnameToMessage(SocketMessage* message, char* hostname)
{
    if (message == nullptr || hostname == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "CopyHostnameToMessage: Invalid parameters");
        return false;
    }

    size_t hostnameLen = strlen(hostname);
    if (hostnameLen > INT_MAX - 1) {
        HILOG_ERROR(HILOG_MODULE_ACE, "The Length of data is too long!");
        return false;
    }
    message->addrInfoParams.nodename = static_cast<char *>(malloc(hostnameLen + 1));
    if (message->addrInfoParams.nodename == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "CopyHostnameToMessage: malloc failed for nodename!");
        return false;
    }

    int32_t ret = strcpy_s(message->addrInfoParams.nodename, hostnameLen + 1, hostname);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "CopyHostnameToMessage: strcpy_s failed! ret=%d", ret);
        free(message->addrInfoParams.nodename);
        message->addrInfoParams.nodename = nullptr;
        return false;
    }

    return true;
}

// 复制服务名到消息
static bool CopyServiceToMessage(SocketMessage* message, char* service)
{
    if (message == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "CopyServiceToMessage: message is null");
        return false;
    }

    if (service == nullptr) {
        // 服务名为空是允许的
        message->addrInfoParams.servname = nullptr;
        return true;
    }

    size_t serviceLen = strlen(service);
    if (serviceLen > INT_MAX - 1) {
        HILOG_ERROR(HILOG_MODULE_ACE, "The Length of data is too long!");
        return false;
    }
    message->addrInfoParams.servname = static_cast<char *>(malloc(serviceLen + 1));
    if (message->addrInfoParams.servname == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "CopyServiceToMessage: malloc failed for servname!");
        return false;
    }

    int32_t ret = strcpy_s(message->addrInfoParams.servname, serviceLen + 1, service);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "CopyServiceToMessage: strcpy_s failed! ret=%d", ret);
        free(message->addrInfoParams.servname);
        message->addrInfoParams.servname = nullptr;
        return false;
    }

    return true;
}

// 清理地址信息消息资源
static void CleanupAddrInfoMessageResources(SocketMessage* message)
{
    if (message == nullptr) {
        return;
    }

    if (message->addrInfoParams.nodename != nullptr) {
        free(message->addrInfoParams.nodename);
        message->addrInfoParams.nodename = nullptr;
    }

    if (message->addrInfoParams.servname != nullptr) {
        free(message->addrInfoParams.servname);
        message->addrInfoParams.servname = nullptr;
    }
}

// 创建地址信息消息
static SocketMessage* CreateAddrInfoMessageInternal(char* hostname, char* service,
    const struct addrinfo& hints)
{
    // 分配消息结构体
    SocketMessage* message = new (std::nothrow) SocketMessage();
    if (message == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Allocate SocketMessage failed!");
        return nullptr;
    }

    message->messageId = SOCKET_GETADDRINFO;

    // 复制主机名
    bool hostnameCopied = CopyHostnameToMessage(message, hostname);
    if (!hostnameCopied) {
        delete message;
        return nullptr;
    }

    // 复制服务名
    bool serviceCopied = CopyServiceToMessage(message, service);
    if (!serviceCopied) {
        free(message->addrInfoParams.nodename);
        message->addrInfoParams.nodename = nullptr;
        delete message;
        message = nullptr;
        return nullptr;
    }

    // 复制hints结构
    int32_t ret = memcpy_s(&message->addrInfoParams.hints, sizeof(struct addrinfo),
        &hints, sizeof(struct addrinfo));
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "memcpy_s failed for hints! ret=%d", ret);
        CleanupAddrInfoMessageResources(message);
        delete message;
        message = nullptr;
        return nullptr;
    }

    HILOG_INFO(HILOG_MODULE_ACE, "CreateAddrInfoMessageInternal: Message created successfully");
    return message;
}

// GetAddrInfo消息创建
SocketMessage *SocketModule::CreateGetAddrInfoMessage(
    char *hostname, char *service, JSIValue thisVal, JSIValue callback)
{
    // 配置hints参数
    struct addrinfo hints = {
        .ai_flags = MSG_DONTWAIT,
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
        .ai_protocol = IPPROTO_IP,
    };

    // 调用静态函数创建消息结构
    SocketMessage *message = CreateAddrInfoMessageInternal(hostname, service, hints);
    if (message == nullptr) {
        return nullptr;
    }

    // 设置JavaScript相关参数
    message->context = JSI::AcquireValue(thisVal);
    message->callback = JSI::AcquireValue(callback);
    message->result = -1;
    message->bytesResult = -1;
    message->addrInfoResult = nullptr;
    message->errorMsg = nullptr;

    return message;
}

// 清理GetAddrInfo资源
void SocketModule::CleanupGetAddrInfoResources(SocketMessage *message, char *hostname, char *service)
{
    if (message == nullptr) {
        return;
    }

    if (message->addrInfoParams.nodename != nullptr) {
        free(message->addrInfoParams.nodename);
    }
    if (message->addrInfoParams.servname != nullptr) {
        free(message->addrInfoParams.servname);
    }
    if (!JSI::ValueIsUndefined(message->callback)) {
        JSI::ReleaseValue(message->callback);
    }
    if (!JSI::ValueIsUndefined(message->context)) {
        JSI::ReleaseValue(message->context);
    }

    delete message;
}

// GetAddrInfo API
JSIValue SocketModule::GetAddrInfo(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    if (args == nullptr || argsSize < ARGS_THREE) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid parameters, argsSize=%d", argsSize);
        return JSI::CreateUndefined();
    }

    std::lock_guard<std::mutex> lock(socketClientsMutex_);
    auto it = socketClients_.find(thisVal);
    if (it == socketClients_.end()) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Socket context not found");
        return JSI::CreateUndefined();
    }

    char *hostname = JSI::ValueToString(args[0]);
    char *service = JSI::ValueToString(args[1]);
    JSIValue callback = args[ARGS_TWO];

    if (hostname == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Hostname is null");
        return JSI::CreateUndefined();
    }

    SocketMessage *message = CreateGetAddrInfoMessage(hostname, service, thisVal, callback);
    if (message == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "CreateGetAddrInfoMessage failed!");
        JSI::ReleaseString(hostname);
        if (service != nullptr) {
            JSI::ReleaseString(service);
        }
        return JSI::CreateUndefined();
    }

    SocketModuleClient *client = static_cast<SocketModuleClient *>(it->second);
    int32_t ret = client->SocketMessagePut(message);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "SocketMessagePut failed! ret=%d", ret);
        CleanupGetAddrInfoResources(message, hostname, service);
    }

    JSI::ReleaseString(hostname);
    if (service != nullptr) {
        JSI::ReleaseString(service);
    }

    HILOG_INFO(HILOG_MODULE_ACE, "GetAddrInfo API exit");
    return JSI::CreateUndefined();
}

// Destroy API
JSIValue SocketModule::Destroy(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE, "Destroy Socket client");

    std::lock_guard<std::mutex> lock(socketClientsMutex_);
    auto it = socketClients_.find(thisVal);
    if (it == socketClients_.end()) {
        HILOG_INFO(HILOG_MODULE_ACE, "Socket client not found");
        return JSI::CreateUndefined();
    }

    SocketModuleClient *client = static_cast<SocketModuleClient *>(it->second);
    if (client != nullptr) {
        client->CloseSocketThread();
        delete client;
    }

    socketClients_.erase(it);

    HILOG_INFO(HILOG_MODULE_ACE, "Socket client destroyed successfully, remaining clients: %zu", socketClients_.size());
    return JSI::CreateUndefined();
}

// 解析Socket地址
bool SocketModule::ParseSockAddr(const JSIValue &addrObj, struct sockaddr_in *addr)
{
    if (addr == nullptr || JSI::ValueIsUndefined(addrObj)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid parameters");
        return false;
    }

    (void)memset_s(addr, sizeof(struct sockaddr_in), 0, sizeof(struct sockaddr_in));

    char *ipStr = JSI::GetStringProperty(addrObj, "address");
    if (ipStr != nullptr) {
        if (inet_pton(AF_INET, ipStr, &addr->sin_addr) != 1) {
            HILOG_ERROR(HILOG_MODULE_ACE, "Invalid IP address: %s", ipStr);
            JSI::ReleaseString(ipStr);
            return false;
        }
        JSI::ReleaseString(ipStr);
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "address property not found");
        return false;
    }

    JSIValue portValue = JSI::GetNamedProperty(addrObj, "port");
    if (JSI::ValueIsUndefined(portValue)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "port property not found");
        return false;
    }

    int port = static_cast<int>(JSI::ValueToNumber(portValue));
    JSI::ReleaseValue(portValue);

    addr->sin_port = htons(static_cast<uint16_t>(port));
    addr->sin_family = AF_INET;

    HILOG_INFO(
        HILOG_MODULE_ACE, "ParseSockAddr: address=%s, port=%d", inet_ntoa(addr->sin_addr), ntohs(addr->sin_port));

    return true;
}

// 模块初始化函数
void InitSocketClientModule(JSIValue exports)
{
    HILOG_INFO(HILOG_MODULE_ACE, "InitSocketModule called");
    JSI::SetModuleAPI(exports, "createSocket", SocketModule::CreateSocket);
    JSI::SetOnTerminate(exports, SocketModule::OnTerminate);
}
}  // namespace ACELite
}  // namespace OHOS