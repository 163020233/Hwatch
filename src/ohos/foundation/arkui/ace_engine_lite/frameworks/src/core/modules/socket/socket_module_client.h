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

#ifndef SOCKET_MODULE_CLIENT_H
#define SOCKET_MODULE_CLIENT_H

#include <atomic>
#include <map>
#include <mutex>
#include <memory>
#include <condition_variable>
#include "cmsis_os2.h"
#include "jsi/jsi.h"
#include "securec.h"
#include "lwip_wrapper.h"

namespace OHOS {
namespace ACELite {

// Socket消息类型
enum SocketMessageId {
    SOCKET_CREATE = 0,
    SOCKET_CONNECT,
    SOCKET_SEND,
    SOCKET_RECV,
    SOCKET_CLOSE,
    SOCKET_GETADDRINFO,
    SOCKET_FINISH
};

// 二进制数据类型标识
enum DataType {
    DATA_TYPE_TEXT = 0,
    DATA_TYPE_BINARY = 1,
    DATA_TYPE_HEX = 2
};

// Socket消息结构
struct SocketMessage {
    SocketMessageId messageId;

    // 操作参数
    union {
        struct {
            int domain;
            int type;
            int protocol;
        } createParams;

        struct {
            struct sockaddr_in addr;
        } connectParams;

        struct {
            unsigned char *data;  // 发送或接收的数据
            size_t dataLen;       // 数据长度
            DataType dataType;    // 数据类型
        } sendRecvParams;

        struct {
            char *nodename;
            char *servname;
            struct addrinfo hints;
        } addrInfoParams;
    };

    // 回调相关
    JSIValue context;
    JSIValue callback;

    // 结果存储
    int result;
    ssize_t bytesResult;
    struct addrinfo *addrInfoResult;
    char *errorMsg;
    DataType recvDataType;  // 接收数据的数据类型

    SocketMessage()
        : messageId(SOCKET_CREATE), context(JSI::CreateUndefined()), callback(JSI::CreateUndefined()), result(-1),
          bytesResult(-1), addrInfoResult(nullptr), errorMsg(nullptr), recvDataType(DATA_TYPE_TEXT)
    {
        memset_s(&createParams, sizeof(createParams), 0, sizeof(createParams));
        sendRecvParams.data = nullptr;
        sendRecvParams.dataLen = 0;
        addrInfoParams.nodename = nullptr;
        addrInfoParams.servname = nullptr;
    }

    // 禁用拷贝构造函数和拷贝赋值运算符
    SocketMessage(const SocketMessage &) = delete;
    SocketMessage &operator=(const SocketMessage &) = delete;

    // 移动构造函数
    SocketMessage(SocketMessage &&other) noexcept
        : messageId(other.messageId), context(other.context), callback(other.callback), result(other.result),
          bytesResult(other.bytesResult), addrInfoResult(other.addrInfoResult), errorMsg(other.errorMsg),
          recvDataType(other.recvDataType)
    {
        // 根据消息类型复制联合体内容
        switch (other.messageId) {  // 使用源对象的messageId
            case SOCKET_CREATE:
                createParams = other.createParams;
                break;
            case SOCKET_CONNECT:
                connectParams = other.connectParams;
                break;
            case SOCKET_SEND:
            case SOCKET_RECV:
                sendRecvParams.data = other.sendRecvParams.data;
                sendRecvParams.dataLen = other.sendRecvParams.dataLen;
                sendRecvParams.dataType = other.sendRecvParams.dataType;
                break;
            case SOCKET_CLOSE:
                break;
            case SOCKET_GETADDRINFO:
                addrInfoParams.nodename = other.addrInfoParams.nodename;
                addrInfoParams.servname = other.addrInfoParams.servname;
                addrInfoParams.hints = other.addrInfoParams.hints;
                break;
            case SOCKET_FINISH:
                // SOCKET_FINISH 没有联合体数据
                break;
            default:
                // 未知消息类型
                break;
        }

        // 将源对象资源置空，避免双重释放
        other.context = JSI::CreateUndefined();
        other.callback = JSI::CreateUndefined();
        other.result = -1;
        other.bytesResult = -1;
        other.addrInfoResult = nullptr;
        other.errorMsg = nullptr;
        other.recvDataType = DATA_TYPE_TEXT;

        // 清空源对象的指针，避免双重释放
        if (other.sendRecvParams.data) {
            other.sendRecvParams.data = nullptr;
            other.sendRecvParams.dataLen = 0;
        }
        if (other.addrInfoParams.nodename) {
            other.addrInfoParams.nodename = nullptr;
        }
        if (other.addrInfoParams.servname) {
            other.addrInfoParams.servname = nullptr;
        }
    }
    // 移动赋值运算符
    SocketMessage &operator=(SocketMessage &&other) noexcept
    {
        if (this != &other) {
            // 手动释放当前对象资源，而不是调用析构函数，释放errorMsg
            if (errorMsg != nullptr) {
                free(errorMsg);
                errorMsg = nullptr;
            }

            // 释放sendRecvParams.data
            if (sendRecvParams.data != nullptr) {
                free(sendRecvParams.data);
                sendRecvParams.data = nullptr;
            }

            // 释放addrInfoParams中的动态分配内存
            if (messageId == SOCKET_GETADDRINFO) {
                if (addrInfoParams.nodename != nullptr) {
                    free(addrInfoParams.nodename);
                    addrInfoParams.nodename = nullptr;
                }
                if (addrInfoParams.servname != nullptr) {
                    free(addrInfoParams.servname);
                    addrInfoParams.servname = nullptr;
                }
            }

            // 释放addrinfo结果
            if (addrInfoResult != nullptr) {
                lwip_freeaddrinfo(addrInfoResult);
                addrInfoResult = nullptr;
            }

            // 释放JSI值
            if (!JSI::ValueIsUndefined(callback)) {
                JSI::ReleaseValue(callback);
            }
            if (!JSI::ValueIsUndefined(context)) {
                JSI::ReleaseValue(context);
            }

            // 移动资源
            messageId = other.messageId;
            context = other.context;
            callback = other.callback;
            result = other.result;
            bytesResult = other.bytesResult;
            addrInfoResult = other.addrInfoResult;
            errorMsg = other.errorMsg;
            recvDataType = other.recvDataType;

            // 根据源对象的messageId复制联合体内容
            switch (other.messageId) {
                case SOCKET_CREATE:
                    createParams = other.createParams;
                    break;
                case SOCKET_CONNECT:
                    connectParams = other.connectParams;
                    break;
                case SOCKET_SEND:
                case SOCKET_RECV:
                    sendRecvParams.data = other.sendRecvParams.data;
                    sendRecvParams.dataLen = other.sendRecvParams.dataLen;
                    sendRecvParams.dataType = other.sendRecvParams.dataType;
                    break;
                case SOCKET_CLOSE:
                    break;
                case SOCKET_GETADDRINFO:
                    addrInfoParams.nodename = other.addrInfoParams.nodename;
                    addrInfoParams.servname = other.addrInfoParams.servname;
                    addrInfoParams.hints = other.addrInfoParams.hints;
                    break;
                case SOCKET_FINISH:
                    // SOCKET_FINISH没有联合体数据
                    break;
            }

            // 将源对象资源置空
            other.messageId = SOCKET_CREATE;
            other.context = JSI::CreateUndefined();
            other.callback = JSI::CreateUndefined();
            other.result = -1;
            other.bytesResult = -1;
            other.addrInfoResult = nullptr;
            other.errorMsg = nullptr;
            other.recvDataType = DATA_TYPE_TEXT;

            // 清空源对象的指针，避免双重释放
            if (other.sendRecvParams.data) {
                other.sendRecvParams.data = nullptr;
                other.sendRecvParams.dataLen = 0;
            }
            if (other.addrInfoParams.nodename) {
                other.addrInfoParams.nodename = nullptr;
            }
            if (other.addrInfoParams.servname) {
                other.addrInfoParams.servname = nullptr;
            }
        }
        return *this;
    }
    ~SocketMessage()
    {
        // 释放错误信息
        if (errorMsg != nullptr) {
            free(errorMsg);
            errorMsg = nullptr;
        }

        // 释放sendRecvParams.data，不依赖messageId，只检查指针是否非空
        if (sendRecvParams.data != nullptr) {
            free(sendRecvParams.data);
            sendRecvParams.data = nullptr;
            sendRecvParams.dataLen = 0;
        }

        // 释放addrInfoParams中的动态分配内存
        if (addrInfoParams.nodename != nullptr) {
            free(addrInfoParams.nodename);
            addrInfoParams.nodename = nullptr;
        }
        if (addrInfoParams.servname != nullptr) {
            free(addrInfoParams.servname);
            addrInfoParams.servname = nullptr;
        }

        // 释放addrinfo结果
        if (addrInfoResult != nullptr) {
            lwip_freeaddrinfo(addrInfoResult);
            addrInfoResult = nullptr;
        }

        // 释放JSI值
        if (!JSI::ValueIsUndefined(callback)) {
            JSI::ReleaseValue(callback);
        }
        if (!JSI::ValueIsUndefined(context)) {
            JSI::ReleaseValue(context);
        }
    }
};

// Socket异步参数结构体
struct SocketAsyncParams {
    SocketAsyncParams()
        : err(JSI::CreateUndefined()), result(JSI::CreateUndefined()), callback(JSI::CreateUndefined()),
          context(JSI::CreateUndefined()), typedArray(JSI::CreateUndefined()), arrayBuffer(JSI::CreateUndefined()),
          typeName(), rawDataLen(0), errorCode(0), addrInfo(), addrInfoResult(nullptr),
          operationType(SOCKET_CREATE)
    {}

    ~SocketAsyncParams()
    {
        if (addrInfoResult != nullptr) {
            lwip_freeaddrinfo(addrInfoResult);
            addrInfoResult = nullptr;
        }
        // 清理JSI值
        if (!JSI::ValueIsUndefined(err)) {
            JSI::ReleaseValue(err);
            err = JSI::CreateUndefined();  // 重置为undefined
        }
        if (!JSI::ValueIsUndefined(result)) {
            JSI::ReleaseValue(result);
            result = JSI::CreateUndefined();  // 重置为undefined
        }
        if (!JSI::ValueIsUndefined(callback)) {
            JSI::ReleaseValue(callback);
            callback = JSI::CreateUndefined();  // 重置为undefined
        }
        if (!JSI::ValueIsUndefined(context)) {
            JSI::ReleaseValue(context);
            context = JSI::CreateUndefined();  // 重置为undefined
        }
        if (!JSI::ValueIsUndefined(typedArray)) {
            JSI::ReleaseValue(typedArray);
            typedArray = JSI::CreateUndefined();  // 重置为undefined
        }
        if (!JSI::ValueIsUndefined(arrayBuffer)) {
            JSI::ReleaseValue(arrayBuffer);
            arrayBuffer = JSI::CreateUndefined();  // 重置为undefined
        }
    }

    // 禁止拷贝构造和赋值
    SocketAsyncParams(const SocketAsyncParams &) = delete;
    SocketAsyncParams &operator=(const SocketAsyncParams &) = delete;

    JSIValue err;
    JSIValue result;
    JSIValue callback;
    JSIValue context;
    JSIValue typedArray;
    JSIValue arrayBuffer;
    std::string typeName;
    size_t rawDataLen;  // 原始数据长度（字节数）
    int errorCode;
    struct sockaddr_in addrInfo;
    struct addrinfo *addrInfoResult;  // 用于存储getaddrinfo结果
    bool isBinary;                    // 标记是否为二进制数据
    SocketMessageId operationType;    // 操作类型
};

class SocketModuleClient {
public:
    SocketModuleClient();
    ~SocketModuleClient();

    bool CreateSocketThread();
    void CloseSocketThread();
    bool IsRunning() const;

    // 消息队列操作
    int32_t SocketMessagePut(SocketMessage *message);
    int32_t SocketMessageGet(SocketMessage **message, uint32_t timeoutMs = osWaitForever);

private:
    static void SocketThread(void *argument);
    static JSIValue CreateSuccessResult(SocketAsyncParams *asyncParams);
    static void CreateErrorResult(SocketAsyncParams *asyncParams, int errorCode, const char *errorMsg);
    void ProcessMessage(SocketMessage *message);
    void CleanupMessageQueue();

    // 线程和队列
    osMessageQueueId_t socketQueueId_;
    void *stackMem_;
    osThreadId_t threadId_;
    std::atomic<bool> isRunning_;
    std::atomic<bool> shouldExit_;

    // Socket描述符管理 - 每个client实例只管理一个socket
    int socketFd_;

    // 队列参数
    static constexpr uint32_t SOCKET_QUEUE_LENGTH = 32;
    static constexpr uint32_t SOCKET_TASK_STACK_SIZE = 4096;
    static constexpr uint32_t THREAD_EXIT_TIMEOUT_MS = 1000;
    static constexpr uint32_t MESSAGE_TIMEOUT_MS = 500;

    static bool IsFatalSocketError(int errorCode);

    void HandleSocketCreate(
        SocketMessage *message, SocketAsyncParams *asyncParams, int &errorCode, const char *&errorMsg, bool &hasError);
    void HandleSocketConnect(
        SocketMessage *message, SocketAsyncParams *asyncParams, int &errorCode, const char *&errorMsg, bool &hasError);
    void HandleSocketSend(
        SocketMessage *message, SocketAsyncParams *asyncParams, int &errorCode, const char *&errorMsg, bool &hasError);
    void HandleSendBySocketType(
        SocketMessage *message, int socktype, int &errorCode, const char *&errorMsg, bool &hasError);
    void HandleSocketRecv(
        SocketMessage *message, SocketAsyncParams *asyncParams, int &errorCode, const char *&errorMsg, bool &hasError);
    void HandleFallbackConversion(SocketMessage *message, SocketAsyncParams *asyncParams);
    void HandleSocketClose(
        SocketMessage *message, SocketAsyncParams *asyncParams, int &errorCode, const char *&errorMsg, bool &hasError);
    void HandleSocketGetAddrInfo(
        SocketMessage *message, SocketAsyncParams *asyncParams, int &errorCode, const char *&errorMsg, bool &hasError);

    // Socket操作
    int SocketCreate(int domain, int type, int protocol);
    int SocketConnect(const struct sockaddr_in *addr);
    ssize_t SocketRecv(void *buf, size_t len);
    int SocketClose();
    int SocketGetAddrInfo(
        const char *nodename, const char *servname, const struct addrinfo *hints, struct addrinfo **res);
    bool CheckUdpConnState(int socktype, int &errorCode, const char *&errorMsg, bool &hasError);

    ssize_t PrepareForSocketRecv(void *buf, size_t len);
    ssize_t PerformSocketRecv(void *buf, size_t len);
};

}  // namespace ACELite
}  // namespace OHOS

#endif  // SOCKET_MODULE_CLIENT_H
