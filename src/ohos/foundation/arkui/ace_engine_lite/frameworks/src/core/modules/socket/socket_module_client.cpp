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

#include "socket_module_client.h"
#include "socket_module.h"
#include "ace_log.h"
#include <cstring>
#include <cerrno>
#include <fcntl.h>
#include <algorithm>

namespace OHOS {
namespace ACELite {
constexpr ssize_t RECV_BUFF_SIZE = 8 * 1024;

// 设置socket接收超时
static int SetSocketTimeout(int sockfd, int seconds)
{
    struct timeval timeout;
    timeout.tv_sec = seconds;
    timeout.tv_usec = 0;

    // 设置接收超时
    if (lwip_setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        HILOG_WARN(HILOG_MODULE_ACE, "setsockopt(SO_RCVTIMEO) failed: %d", errno);
    }

    // 设置发送超时
    if (lwip_setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0) {
        HILOG_WARN(HILOG_MODULE_ACE, "setsockopt(SO_SNDTIMEO) failed: %d", errno);
    }

    return 0;
}

SocketModuleClient::SocketModuleClient()
    : socketQueueId_(nullptr), stackMem_(nullptr), threadId_(nullptr), isRunning_(false), shouldExit_(false), socketFd_(-1)
{
}

SocketModuleClient::~SocketModuleClient()
{
    CloseSocketThread();
}

bool SocketModuleClient::CreateSocketThread()
{
    // 重置退出标志
    shouldExit_.store(false);

    // 创建消息队列
    socketQueueId_ = osMessageQueueNew(SOCKET_QUEUE_LENGTH, sizeof(SocketMessage *), nullptr);
    if (socketQueueId_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Create SocketMessage queue failed!");
        return false;
    }

    // 分配线程栈内存
    stackMem_ = memalign(LOSCFG_STACK_POINT_ALIGN_SIZE, SOCKET_TASK_STACK_SIZE);
    if (stackMem_ == nullptr) {
        osMessageQueueDelete(socketQueueId_);
        socketQueueId_ = nullptr;
        HILOG_ERROR(HILOG_MODULE_ACE, "Malloc memory for SocketThread failed!");
        return false;
    }

    // 设置线程属性
    osThreadAttr_t attr;
    (void)memset_s(&attr, sizeof(attr), 0, sizeof(attr));
    attr.name = "SocketThread";
    attr.attr_bits = osThreadJoinable;
    attr.cb_mem = nullptr;
    attr.cb_size = 0U;
    attr.stack_mem = stackMem_;
    attr.stack_size = SOCKET_TASK_STACK_SIZE;
    attr.priority = osPriorityAboveNormal;

    isRunning_.store(true);
    threadId_ = osThreadNew((osThreadFunc_t)SocketThread, static_cast<void *>(this), &attr);
    if (threadId_ == nullptr) {
        osMessageQueueDelete(socketQueueId_);
        socketQueueId_ = nullptr;
        free(stackMem_);
        stackMem_ = nullptr;
        isRunning_.store(false);
        HILOG_ERROR(HILOG_MODULE_ACE, "Create SocketThread failed");
        return false;
    }

    HILOG_INFO(HILOG_MODULE_ACE, "CreateSocketThread success");
    return true;
}

// 清理单个消息
static void CleanupSingleMessageHelper(SocketMessage *msg)
{
    if (msg == nullptr) {
        return;
    }

    // 根据消息类型清理资源
    switch (msg->messageId) {
        case SOCKET_CREATE:
        case SOCKET_CONNECT:
        case SOCKET_CLOSE:
            break;
        case SOCKET_SEND:
        case SOCKET_RECV:
            if (msg->sendRecvParams.data != nullptr) {
                free(msg->sendRecvParams.data);
                msg->sendRecvParams.data = nullptr;
                msg->sendRecvParams.dataLen = 0;
            }
            break;
        case SOCKET_GETADDRINFO:
            if (msg->addrInfoParams.nodename != nullptr) {
                free(msg->addrInfoParams.nodename);
                msg->addrInfoParams.nodename = nullptr;
            }
            if (msg->addrInfoParams.servname != nullptr) {
                free(msg->addrInfoParams.servname);
                msg->addrInfoParams.servname = nullptr;
            }
            if (msg->addrInfoResult != nullptr) {
                lwip_freeaddrinfo(msg->addrInfoResult);
                msg->addrInfoResult = nullptr;
            }
            break;
        case SOCKET_FINISH:
            // SOCKET_FINISH没有需要清理的特殊资源
            break;
        default:
            HILOG_WARN(HILOG_MODULE_ACE, "CleanupSingleMessageHelper: unknown messageId %d", msg->messageId);
            break;
    }
    // 释放错误信息
    if (msg->errorMsg != nullptr) {
        free(msg->errorMsg);
        msg->errorMsg = nullptr;
    }
    // 释放JSI值
    if (!JSI::ValueIsUndefined(msg->callback)) {
        JSI::ReleaseValue(msg->callback);
    }
    if (!JSI::ValueIsUndefined(msg->context)) {
        JSI::ReleaseValue(msg->context);
    }

    // 释放消息本身
    delete msg;
}

void SocketModuleClient::CleanupMessageQueue()
{
    if (socketQueueId_ == nullptr) {
        return;
    }

    SocketMessage *msg = nullptr;

    // 清空队列中的所有消息
    while (osMessageQueueGet(socketQueueId_, &msg, nullptr, 0) == 0) {
        CleanupSingleMessageHelper(msg);
    }

    HILOG_INFO(HILOG_MODULE_ACE, "Message queue cleanup completed");
}

void SocketModuleClient::CloseSocketThread()
{
    HILOG_INFO(HILOG_MODULE_ACE, "CloseSocketThread called, isRunning=%d", isRunning_.load());

    if (!isRunning_.load() && threadId_ == nullptr) {
        HILOG_INFO(HILOG_MODULE_ACE, "SocketThread is not running");
        return;
    }

    // 设置退出标志
    shouldExit_.store(true);
    isRunning_.store(false);

    HILOG_INFO(HILOG_MODULE_ACE, "Waiting for SocketThread to exit...");

    // 发送一个唤醒消息（如果线程在等待消息）
    if (socketQueueId_ != nullptr) {
        SocketMessage *wakeupMsg = new (std::nothrow) SocketMessage();
        if (wakeupMsg != nullptr) {
            wakeupMsg->messageId = SOCKET_FINISH;
            (void)osMessageQueuePut(socketQueueId_, &wakeupMsg, 0, 50);  // 超时时间50ms
            HILOG_INFO(HILOG_MODULE_ACE, "Sent finish message to socket thread");
        }
    }

    // 等待线程结束
    if (threadId_ != nullptr) {
        int32_t joinStatus = osThreadJoin(threadId_);
        if (joinStatus == 0) {
            HILOG_INFO(HILOG_MODULE_ACE, "SocketThread joined successfully");
        } else {
            HILOG_ERROR(HILOG_MODULE_ACE, "Failed to join SocketThread: %d", joinStatus);
            // 尝试强制终止线程（作为最后手段）
            osThreadTerminate(threadId_);
            HILOG_WARN(HILOG_MODULE_ACE, "Force terminated socket thread");
        }
        threadId_ = nullptr;
    }

    // 清空消息队列
    CleanupMessageQueue();

    // 清理队列资源
    if (socketQueueId_ != nullptr) {
        osMessageQueueDelete(socketQueueId_);
        socketQueueId_ = nullptr;
        HILOG_INFO(HILOG_MODULE_ACE, "Message queue deleted");
    }

    // 清理栈内存
    if (stackMem_ != nullptr) {
        free(stackMem_);
        stackMem_ = nullptr;
    }

    // 关闭当前客户端的socket
    if (socketFd_ >= 0) {
        HILOG_INFO(HILOG_MODULE_ACE, "Closing socket %d during cleanup", socketFd_);
        (void)lwip_close(socketFd_);
        socketFd_ = -1;
        HILOG_INFO(HILOG_MODULE_ACE, "Socket closed");
    }

    HILOG_INFO(HILOG_MODULE_ACE, "CloseSocketThread completed");
}

bool SocketModuleClient::IsRunning() const
{
    return isRunning_.load() && !shouldExit_.load();
}

int32_t SocketModuleClient::SocketMessagePut(SocketMessage *message)
{
    if (socketQueueId_ == nullptr || message == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "SocketMessagePut: invalid parameters");
        if (message != nullptr) {
            delete message;
        }
        return osErrorParameter;
    }

    if (!isRunning_.load() || shouldExit_.load()) {
        HILOG_ERROR(HILOG_MODULE_ACE, "SocketMessagePut: thread is not running or exiting");
        delete message;
        return osErrorResource;
    }

    int32_t status = osMessageQueuePut(socketQueueId_, &message, 0, 500);  // 500ms超时
    if (status != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "SocketMessagePut failed: %d", status);
        delete message;
    } else {
        HILOG_DEBUG(HILOG_MODULE_ACE, "SocketMessagePut success");
    }

    return status;
}

int32_t SocketModuleClient::SocketMessageGet(SocketMessage **message, uint32_t timeoutMs)
{
    if (socketQueueId_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "SocketMessageGet: queue not initialized");
        return osErrorParameter;
    }

    if (shouldExit_.load()) {
        HILOG_INFO(HILOG_MODULE_ACE, "SocketMessageGet: thread should exit");
        return osErrorResource;
    }

    SocketMessage *msgPtr = nullptr;
    int32_t status = osMessageQueueGet(socketQueueId_, &msgPtr, nullptr, timeoutMs);
    if (status == 0) {
        *message = msgPtr;
        HILOG_DEBUG(HILOG_MODULE_ACE, "SocketMessageGet success");
    } else if (status == osErrorTimeout) {
        // 超时是正常的，用于检查退出标志
        *message = nullptr;
    } else {
        *message = nullptr;
        HILOG_ERROR(HILOG_MODULE_ACE, "SocketMessageGet failed: %d", status);
    }

    return status;
}

void SocketModuleClient::SocketThread(void *argument)
{
    SocketModuleClient *client = static_cast<SocketModuleClient *>(argument);
    if (client == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "SocketThread: client is null");
        return;
    }

    while (client->IsRunning()) {
        SocketMessage *message = nullptr;
        int32_t ret = client->SocketMessageGet(&message, 100);  // 100ms超时
        if (ret == osErrorTimeout) {
            // 超时，检查是否应该退出
            continue;
        } else if (ret != 0) {
            HILOG_ERROR(HILOG_MODULE_ACE, "SocketMessageGet failed: %d", ret);
            break;
        }

        if (message == nullptr) {
            HILOG_WARN(HILOG_MODULE_ACE, "Received null message");
            continue;
        }

        // 处理消息
        switch (message->messageId) {
            case SOCKET_CREATE:
            case SOCKET_CONNECT:
            case SOCKET_SEND:
            case SOCKET_RECV:
            case SOCKET_CLOSE:
            case SOCKET_GETADDRINFO:
                client->ProcessMessage(message);
                break;
            case SOCKET_FINISH:
                HILOG_INFO(HILOG_MODULE_ACE, "Received finish message, exiting thread");
                CleanupSingleMessageHelper(message);
                HILOG_INFO(HILOG_MODULE_ACE, "SocketThread exiting normally");
                return;
            default:
                HILOG_ERROR(HILOG_MODULE_ACE, "Invalid messageId: %d", message->messageId);
                CleanupSingleMessageHelper(message);
                break;
        }
    }

    HILOG_INFO(HILOG_MODULE_ACE, "SocketThread exited (shouldExit=%d)", client->shouldExit_.load());
}

// 处理接收操作的成功结果
static void FillRecvSuccessResult(JSIValue resultObj, SocketAsyncParams *asyncParams)
{
    if (asyncParams == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "FillRecvSuccessResult: asyncParams is null");
        return;
    }

    if (JSI::ValueIsTypedArray(asyncParams->typedArray) && asyncParams->rawDataLen > 0) {
        JSI::SetNamedProperty(resultObj, "data", asyncParams->typedArray);
    }
}

// 处理地址解析操作的成功结果
static void FillGetAddrInfoSuccessResult(JSIValue resultObj, SocketAsyncParams *asyncParams)
{
    if (asyncParams == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "FillGetAddrInfoSuccessResult: asyncParams is null");
        return;
    }

    if (asyncParams->addrInfoResult != nullptr) {
        char ipStr[INET_ADDRSTRLEN] = {0};
        if (inet_ntop(AF_INET, &asyncParams->addrInfo.sin_addr, ipStr, sizeof(ipStr)) != nullptr) {
            // 使用JSI::CreateString创建字符串，避免悬垂指针
            JSIValue ipStrVal = JSI::CreateString(ipStr);
            if (!JSI::ValueIsUndefined(ipStrVal)) {
                JSI::SetNamedProperty(resultObj, "address", ipStrVal);
                JSI::ReleaseValue(ipStrVal);
            }
            JSI::SetNumberProperty(resultObj, "port", ntohs(asyncParams->addrInfo.sin_port));
        } else {
            HILOG_WARN(HILOG_MODULE_ACE, "FillGetAddrInfoSuccessResult: inet_ntop failed");
        }
    }
}

// 创建成功结果
JSIValue SocketModuleClient::CreateSuccessResult(SocketAsyncParams *asyncParams)
{
    if (asyncParams == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "CreateSuccessResult: asyncParams is null");
        return JSI::CreateUndefined();
    }

    JSIValue resultObj;
    switch (asyncParams->operationType) {
        case SOCKET_CREATE:
        case SOCKET_CONNECT:
        case SOCKET_SEND:
        case SOCKET_CLOSE:
            resultObj = JSI::CreateBoolean(true);
            break;

        case SOCKET_RECV:
            resultObj = JSI::CreateObject();
            FillRecvSuccessResult(resultObj, asyncParams);
            break;

        case SOCKET_GETADDRINFO:
            resultObj = JSI::CreateObject();
            FillGetAddrInfoSuccessResult(resultObj, asyncParams);
            break;

        default:
            resultObj = JSI::CreateUndefined();
            HILOG_WARN(HILOG_MODULE_ACE, "CreateSuccessResult: unknown operation type %d", asyncParams->operationType);
            break;
    }

    return resultObj;
}

// 创建错误结果
void SocketModuleClient::CreateErrorResult(SocketAsyncParams *asyncParams, int errorCode, const char *errorMsg)
{
    if (asyncParams == nullptr) {
        return;
    }

    // 创建错误对象
    JSIValue errObj = JSI::CreateObject();
    if (!JSI::ValueIsUndefined(errObj)) {
        JSI::SetNumberProperty(errObj, "code", errorCode);
        if (errorMsg != nullptr) {
            JSI::SetStringProperty(errObj, "message", errorMsg);
        }
        asyncParams->err = errObj;
    } else {
        asyncParams->err = JSI::CreateUndefined();
    }

    // 创建结果对象
    asyncParams->result = JSI::CreateUndefined();
}

void SocketModuleClient::HandleSocketCreate(
    SocketMessage *message, SocketAsyncParams *asyncParams, int &errorCode, const char *&errorMsg, bool &hasError)
{
    asyncParams->typeName = "socket";
    message->result =
        SocketCreate(message->createParams.domain, message->createParams.type, message->createParams.protocol);
    if (message->result < 0) {
        errorCode = errno;
        errorMsg = "socket creation failed";
        hasError = true;
    }
}

void SocketModuleClient::HandleSocketConnect(
    SocketMessage *message, SocketAsyncParams *asyncParams, int &errorCode, const char *&errorMsg, bool &hasError)
{
    if (socketFd_ < 0) {
        HILOG_WARN(HILOG_MODULE_ACE, "HandleSocketConnect: no socket available in client");
        errorCode = EBADF;
        errorMsg = "no socket available";
        hasError = true;
        return;
    }
    asyncParams->typeName = "connect";
    message->result = SocketConnect(&message->connectParams.addr);

    // 检查socket类型
    int socktype = 0;
    socklen_t socktype_len = sizeof(socktype);
    int getsockoptRet = lwip_getsockopt(socketFd_, SOL_SOCKET, SO_TYPE, &socktype, &socktype_len);
    if (getsockoptRet < 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "HandleSocketConnect: getsockopt failed, errno=%d", errno);
        errorCode = errno;
        errorMsg = "getsockopt failed";
        hasError = true;
        return;
    }

    if (socktype == SOCK_DGRAM) {
        // UDP套接字，connect只是设置默认地址
        message->result = 0;  // 强制返回成功
    } else if (message->result < 0) {
        // TCP套接字连接失败
        errorCode = errno;
        errorMsg = "connection failed";
        hasError = true;
    }
}

void SocketModuleClient::HandleSocketSend(SocketMessage *message, SocketAsyncParams *asyncParams, int &errorCode,
    const char *&errorMsg, bool &hasError)
{
    if (socketFd_ < 0) {
        HILOG_WARN(HILOG_MODULE_ACE, "HandleSocketSend: no socket available in client");
        errorCode = EBADF;
        errorMsg = "no socket available";
        hasError = true;
        return;
    }
    asyncParams->typeName = "send";

    // 获取socket类型，检查返回值
    int socktype = 0;
    socklen_t socktype_len = sizeof(socktype);
    if (lwip_getsockopt(socketFd_, SOL_SOCKET, SO_TYPE, &socktype, &socktype_len) != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "HandleSocketSend: getsockopt failed, errno=%d", errno);
        errorCode = errno;
        errorMsg = "getsockopt failed";
        hasError = true;
        return;
    }

    HILOG_INFO(HILOG_MODULE_ACE, "ProcessSocketSend: socktype=%d, dataLen=%zu",
        socktype, message->sendRecvParams.dataLen);

    HandleSendBySocketType(message, socktype, errorCode, errorMsg, hasError);
    if (!hasError) {
        asyncParams->rawDataLen = static_cast<size_t>(message->bytesResult);
        HILOG_INFO(HILOG_MODULE_ACE, "Send successful, sent %ld bytes", message->bytesResult);
    }
}

// 处理发送操作后的错误
static void HandleSendErrorResult(int &errorCode, const char *&errorMsg, bool &hasError)
{
    int savedErrno = errno;
    errorCode = savedErrno;

    // 根据错误类型提供更具体的错误信息
    // 注意：在lwip中，EWOULDBLOCK和EAGAIN是同一个值，不能重复处理
    switch (savedErrno) {
        case ECONNRESET:
            errorMsg = "Connection reset by peer";
            break;
        case EPIPE:
            errorMsg = "Broken pipe";
            break;
        case EWOULDBLOCK:  // EAGAIN在lwip中与EWOULDBLOCK相同
            errorMsg = "Socket would block";
            break;
        case ECONNREFUSED:
            errorMsg = "Connection refused";
            break;
        case ETIMEDOUT:
            errorMsg = "Connection timed out";
            break;
        default:
            errorMsg = "Send failed";
            break;
    }

    HILOG_ERROR(HILOG_MODULE_ACE, "SocketSend failed: errno=%d, %s", errorCode, strerror(errorCode));
    hasError = true;
}

// 处理不同socket类型的发送
static void HandleUdpSendError(int sockfd, int savedErrno, int &errorCode, const char *&errorMsg, bool &hasError)
{
    switch (savedErrno) {
        case ENOTCONN:
            HILOG_ERROR(HILOG_MODULE_ACE, "UDP socket %d not connected, cannot send", sockfd);
            errorCode = ENOTCONN;
            errorMsg = "UDP socket not connected";
            break;
        case EBADF:
            HILOG_ERROR(HILOG_MODULE_ACE, "UDP socket %d is not a valid socket descriptor", sockfd);
            errorCode = EBADF;
            errorMsg = "Invalid socket descriptor";
            break;
        case EINTR:
            HILOG_ERROR(HILOG_MODULE_ACE, "UDP socket %d operation interrupted by signal", sockfd);
            errorCode = EINTR;
            errorMsg = "Socket operation interrupted";
            break;
        case EFAULT:
            HILOG_ERROR(HILOG_MODULE_ACE, "UDP socket %d address parameter not accessible", sockfd);
            errorCode = EFAULT;
            errorMsg = "Invalid address parameter";
            break;
        case EINVAL:
            HILOG_ERROR(HILOG_MODULE_ACE, "UDP socket %d invalid address length", sockfd);
            errorCode = EINVAL;
            errorMsg = "Invalid address length";
            break;
        default: {
            HILOG_ERROR(HILOG_MODULE_ACE, "UDP socket %d getpeername failed: errno=%d (%s)",
                sockfd, savedErrno, strerror(savedErrno));
            errorCode = savedErrno;
            // 使用strerror获取错误描述
            const char *errStr = strerror(savedErrno);
            if (errStr != nullptr) {
                static char errorBuffer[64];  // 最大打印64字节错误日志
                int ret = snprintf_s(
                    errorBuffer, sizeof(errorBuffer), sizeof(errorBuffer) - 1, "UDP socket error: %s", errStr);
                if (ret >= 0) {
                    errorMsg = errorBuffer;
                } else {
                    errorMsg = "UDP socket error";
                }
            } else {
                errorMsg = "UDP socket error";
            }
            break;
        }
    }
    hasError = true;
}

bool SocketModuleClient::CheckUdpConnState(int socktype, int &errorCode, const char *&errorMsg, bool &hasError)
{
    if (socktype == SOCK_STREAM) {
        return true;
    }

    // 检查UDP socket是否已连接
    struct sockaddr_in peerAddr;
    socklen_t addrLen = sizeof(peerAddr);
    if (lwip_getpeername(socketFd_, (struct sockaddr *)&peerAddr, &addrLen) != 0) {
        // 检查具体错误原因
        int savedErrno = errno;
        HandleUdpSendError(socketFd_, savedErrno, errorCode, errorMsg, hasError);
        return false;
    }
    return true;
}

// 处理不同socket类型的发送
void SocketModuleClient::HandleSendBySocketType(
    SocketMessage *message, int socktype, int &errorCode, const char *&errorMsg, bool &hasError)
{
    if (!CheckUdpConnState(socktype, errorCode, errorMsg, hasError)) {
        return;
    }
    message->bytesResult =  lwip_send(socketFd_, message->sendRecvParams.data + message->bytesResult,
        message->sendRecvParams.dataLen - message->bytesResult, MSG_DONTWAIT);
    HILOG_INFO(HILOG_MODULE_ACE, "send %ld bytes, errno=%d (%s)", message->bytesResult, errno,
        (message->bytesResult < 0) ? strerror(errno) : "success");
    if (message->bytesResult <= 0) {
        // 处理发送后的错误
        HandleSendErrorResult(errorCode, errorMsg, hasError);
    }
}

static void DataToArrayBuffer(uint8_t *ptr, size_t ptrLen, SocketAsyncParams *asyncParams)
{
    asyncParams->rawDataLen = ptrLen;
    uint8_t *jsDataPtr = nullptr;
    asyncParams->arrayBuffer = JSI::CreateArrayBuffer(ptrLen, jsDataPtr);
    if (jsDataPtr == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to create array of size: %zu", ptrLen);
        asyncParams->arrayBuffer = JSI::CreateUndefined();
        return;
    }
    errno_t err = memcpy_s(jsDataPtr, ptrLen, ptr, ptrLen);
    if (err != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to memcpy recv data");
        JSI::ReleaseValue(asyncParams->arrayBuffer);
        asyncParams->arrayBuffer = JSI::CreateUndefined();
        return;
    }
    asyncParams->typedArray =
        JSI::CreateTypedArray(TypedArrayType::JSI_UINT8_ARRAY, ptrLen, asyncParams->arrayBuffer, 0);
    if (JSI::ValueIsUndefined(asyncParams->typedArray)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to create typed array");
        JSI::ReleaseValue(asyncParams->arrayBuffer);
        asyncParams->arrayBuffer = JSI::CreateUndefined();
        return;
    }
}

void SocketModuleClient::HandleSocketRecv(
    SocketMessage *message, SocketAsyncParams *asyncParams, int &errorCode, const char *&errorMsg, bool &hasError)
{
    if (socketFd_ < 0) {
        HILOG_WARN(HILOG_MODULE_ACE, "HandleSocketRecv: no socket available in client");
        errorCode = EBADF;
        errorMsg = "no socket available";
        hasError = true;
        return;
    }
    asyncParams->typeName = "recv";
    uint8_t *ptr = static_cast<uint8_t *>(malloc(RECV_BUFF_SIZE));
    if (ptr == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to allocate recv buffer of size %zu", RECV_BUFF_SIZE);
        return;
    }
    message->bytesResult = SocketRecv(ptr, RECV_BUFF_SIZE);
    if (message->bytesResult > 0) {
        DataToArrayBuffer(ptr, static_cast<size_t>(message->bytesResult), asyncParams);
    } else if (message->bytesResult == 0) {
        // 连接正常关闭或没有数据
        HILOG_DEBUG(HILOG_MODULE_ACE, "SocketRecv: connection closed or no data");
        asyncParams->rawDataLen = 0;
    } else if (message->bytesResult < 0) {
        errorCode = errno;
        errorMsg = "receive failed";
        hasError = true;
        HILOG_ERROR(HILOG_MODULE_ACE, "SocketRecv failed: errno=%d", errorCode);
    }
    free(ptr);
}

void SocketModuleClient::HandleSocketClose(
    SocketMessage *message, SocketAsyncParams *asyncParams, int &errorCode, const char *&errorMsg, bool &hasError)
{
    asyncParams->typeName = "close";
    message->result = SocketClose();
    if (message->result < 0) {
        errorCode = errno;
        errorMsg = "close failed";
        hasError = true;
    }
}

void SocketModuleClient::HandleSocketGetAddrInfo(
    SocketMessage *message, SocketAsyncParams *asyncParams, int &errorCode, const char *&errorMsg, bool &hasError)
{
    asyncParams->typeName = "getaddrinfo";
    struct addrinfo *res = nullptr;
    message->result = SocketGetAddrInfo(message->addrInfoParams.nodename, message->addrInfoParams.servname,
        &message->addrInfoParams.hints, &res);
    if (message->result == 0 && res != nullptr) {
        asyncParams->addrInfoResult = res;

        // 转换第一个地址为sockaddr_in存储
        struct addrinfo *p = res;
        while (p != nullptr) {
            if (p->ai_family == AF_INET && p->ai_addr != nullptr) {
                // 验证地址长度是否足够
                if (p->ai_addrlen < sizeof(struct sockaddr_in)) {
                    HILOG_ERROR(HILOG_MODULE_ACE, "HandleSocketGetAddrInfo: ai_addrlen too small: %u, expected %zu",
                        p->ai_addrlen, sizeof(struct sockaddr_in));
                    errorCode = -1;
                    errorMsg = "invalid address length";
                    hasError = true;
                    break;
                }
                int32_t ret = memcpy_s(
                    &asyncParams->addrInfo, sizeof(struct sockaddr_in), p->ai_addr, sizeof(struct sockaddr_in));
                if (ret != 0) {
                    HILOG_ERROR(HILOG_MODULE_ACE, "memcpy_s failed for address, ret=%d", ret);
                    errorCode = ret;
                    errorMsg = "address conversion failed";
                    hasError = true;
                }
                break;
            }
            p = p->ai_next;
        }
    } else {
        errorCode = message->result;
        errorMsg = "address resolution failed";
        hasError = true;
        if (res != nullptr) {
            lwip_freeaddrinfo(res);
        }
    }
}

void SocketModuleClient::ProcessMessage(SocketMessage *message)
{
    if (message == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "ProcessMessage: message is null");
        return;
    }

    // 创建异步参数
    SocketAsyncParams *asyncParams = new (std::nothrow) SocketAsyncParams();
    if (asyncParams == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Create SocketAsyncParams failed!");
        CleanupSingleMessageHelper(message);
        return;
    }

    // 设置操作类型
    asyncParams->operationType = message->messageId;

    // 保存回调引用
    asyncParams->callback = JSI::AcquireValue(message->callback);
    asyncParams->context = JSI::AcquireValue(message->context);

    int errorCode = 0;
    const char *errorMsg = nullptr;
    bool hasError = false;

    // 处理不同类型消息
    switch (message->messageId) {
        case SOCKET_CREATE:
            HandleSocketCreate(message, asyncParams, errorCode, errorMsg, hasError);
            break;

        case SOCKET_CONNECT:
            HandleSocketConnect(message, asyncParams, errorCode, errorMsg, hasError);
            break;

        case SOCKET_SEND:
            HandleSocketSend(message, asyncParams, errorCode, errorMsg, hasError);
            break;

        case SOCKET_RECV:
            HandleSocketRecv(message, asyncParams, errorCode, errorMsg, hasError);
            break;

        case SOCKET_CLOSE:
            HandleSocketClose(message, asyncParams, errorCode, errorMsg, hasError);
            break;

        case SOCKET_GETADDRINFO:
            HandleSocketGetAddrInfo(message, asyncParams, errorCode, errorMsg, hasError);
            break;

        default:
            errorCode = -1;
            errorMsg = "invalid operation";
            hasError = true;
            HILOG_ERROR(HILOG_MODULE_ACE, "Invalid operation: %d", message->messageId);
            break;
    }

    if (hasError) {
        CreateErrorResult(asyncParams, errorCode, errorMsg);
    } else {
        asyncParams->err = JSI::CreateUndefined();
        asyncParams->result = CreateSuccessResult(asyncParams);
    }

    // 传递完整的asyncParams给CheckSocketCallback
    SocketModule::CheckSocketCallback(asyncParams);

    // 清理消息资源
    CleanupSingleMessageHelper(message);
}

int SocketModuleClient::SocketCreate(int domain, int type, int protocol)
{
    int sockfd = lwip_socket(domain, type, protocol);
    if (sockfd >= 0) {
        // 保存socket描述符到当前客户端实例
        socketFd_ = sockfd;

        // 为TCP socket设置SO_REUSEADDR选项
        if (type == SOCK_STREAM) {
            int reuse = 1;
            (void)lwip_setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

            // 设置socket接收超时（5秒）
            (void)SetSocketTimeout(sockfd, 5);

            // 设置TCP_NODELAY选项，禁用Nagle算法，减少延迟
            int nodelay = 1;
            (void)lwip_setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay));

            // 设置接收缓冲区大小
            int recvBufSize = SOCKET_QUEUE_LENGTH * 1024;  // 32 * 1024：32KB
            (void)lwip_setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &recvBufSize, sizeof(recvBufSize));

            // 设置发送缓冲区大小
            int sendBufSize = SOCKET_QUEUE_LENGTH * 1024;  // 32 * 1024：32KB
            (void)lwip_setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &sendBufSize, sizeof(sendBufSize));
        }

        // 为UDP socket设置广播选项
        if (type == SOCK_DGRAM) {
            int broadcast = 1;
            (void)lwip_setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));

            // 设置socket接收超时（10秒）
            (void)SetSocketTimeout(sockfd, 10);

            // 设置接收缓冲区大小
            int recvBufSize = SOCKET_QUEUE_LENGTH * 1024;  // 32 * 1024：32KB
            (void)lwip_setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &recvBufSize, sizeof(recvBufSize));
        }
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to create socket: %d", errno);
    }
    return sockfd;
}

int SocketModuleClient::SocketConnect(const struct sockaddr_in *addr)
{
    if (socketFd_ < 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "SocketConnect: no socket available in client");
        errno = EBADF;
        return -1;
    }

    int socktype = 0;
    socklen_t socktype_len = sizeof(socktype);
    int ret = -1;

    // 获取socket类型
    ret = lwip_getsockopt(socketFd_, SOL_SOCKET, SO_TYPE, &socktype, &socktype_len);
    if (ret < 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "SocketConnect: getsockopt failed: %s", strerror(errno));
        return -1;
    }

    // 记录要连接的目标地址和端口
    char ipStr[INET_ADDRSTRLEN] = {0};
    inet_ntop(AF_INET, &addr->sin_addr, ipStr, sizeof(ipStr));
    int port = ntohs(addr->sin_port);
    HILOG_INFO(HILOG_MODULE_ACE, "Connecting to %s:%d, socktype=%d", ipStr, port, socktype);

    if (socktype == SOCK_DGRAM) {
        // UDP套接字连接
        ret = lwip_connect(socketFd_, reinterpret_cast<const struct sockaddr *>(addr), sizeof(struct sockaddr_in));
        if (ret < 0) {
            HILOG_WARN(HILOG_MODULE_ACE,
                "UDP connect failed: %s (UDP is connectionless, setting default address failed)", strerror(errno));
            // 对于UDP，connect失败意味着无法设置默认地址，返回错误让调用者知道
            return ret;
        }

        return ret;
    } else {
        // TCP套接字连接
        ret = lwip_connect(socketFd_, reinterpret_cast<const struct sockaddr *>(addr), sizeof(struct sockaddr_in));
        if (ret < 0) {
            HILOG_ERROR(HILOG_MODULE_ACE, "TCP connect failed: %s", strerror(errno));
            return ret;
        }

        // 连接成功后，设置socket超时
        struct timeval timeout;
        timeout.tv_sec = 5;  // 5秒超时
        timeout.tv_usec = 0;
        (void)lwip_setsockopt(socketFd_, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

        HILOG_INFO(HILOG_MODULE_ACE, "Socket %d connected successfully", socketFd_);
        return ret;
    }
}

bool SocketModuleClient::IsFatalSocketError(int errorCode)
{
    // EINVAL (22) 通常是无效参数错误，可能是socket已经无效
    return (errorCode == ECONNRESET || errorCode == ECONNABORTED || errorCode == ENOTCONN || errorCode == EPIPE ||
            errorCode == EINVAL || errorCode == EBADF);
}

ssize_t SocketModuleClient::PerformSocketRecv(void *buf, size_t len)
{
    // 实际接收数据
    ssize_t ret = lwip_recv(socketFd_, buf, len, MSG_DONTWAIT);
    int savedErrno = errno;  // 立即保存errno

    HILOG_INFO(HILOG_MODULE_ACE, "SocketRecv: received %ld bytes, errno=%d (%s)", ret, savedErrno,
        (ret < 0) ? strerror(savedErrno) : "success");
    if (ret == 0) {
        // TCP连接关闭
        HILOG_INFO(HILOG_MODULE_ACE, "SocketRecv: TCP connection closed by peer, sockfd=%d", socketFd_);
        socketFd_ = -1;
    } else if (ret < 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "SocketRecv failed, sockfd=%d, errno=%d: %s", socketFd_, savedErrno,
            strerror(savedErrno));
        // 如果是致命错误，清理socket
        if (IsFatalSocketError(savedErrno)) {
            // 尝试关闭socket
            (void)lwip_close(socketFd_);
            socketFd_ = -1;
            return -1;
        }
        // 对于EAGAIN/EWOULDBLOCK，返回0表示没有数据
        if (savedErrno == EAGAIN || savedErrno == EWOULDBLOCK) {
            ret = 0;
        }
    }

    return ret;
}

// 检查socket可读性
static int CheckSocketReadability(int sockfd)
{
    // 检查socket是否可读（使用select）
    fd_set readfds;
    struct timeval timeout;
    int selectResult = -1;

    // 设置select超时为100ms
    FD_ZERO(&readfds);
    FD_SET(sockfd, &readfds);
    timeout.tv_sec = 0;
    timeout.tv_usec = 100000;  // 100000 us

    selectResult = lwip_select(sockfd + 1, &readfds, nullptr, nullptr, &timeout);
    if (selectResult < 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "CheckSocketReadability: select error: %d", errno);
        return -1;  // select错误
    } else if (selectResult == 0) {
        // 超时，没有数据可读
        HILOG_DEBUG(HILOG_MODULE_ACE, "CheckSocketReadability: timeout, no data available");
        return 0;  // 超时
    }

    // 检查socket是否真的就绪
    if (!FD_ISSET(sockfd, &readfds)) {
        HILOG_DEBUG(HILOG_MODULE_ACE, "CheckSocketReadability: socket not ready after select");
        return 0;  // socket未就绪
    }

    return 1;  // socket可读
}

// 准备接收数据
ssize_t SocketModuleClient::PrepareForSocketRecv(void *buf, size_t len)
{
    // 检查socket可读性
    int readability = CheckSocketReadability(socketFd_);
    if (readability < 0) {
        return -1;
    } else if (readability == 0) {
        return 0;
    }

    // 确保缓冲区有效
    if (buf == nullptr || len == 0) {
        errno = EINVAL;  // 无效参数
        return -1;
    }

    // 返回1表示准备就绪，可以接收数据
    return 1;
}

ssize_t SocketModuleClient::SocketRecv(void *buf, size_t len)
{
    if (socketFd_ < 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "SocketRecv: no socket available in client");
        errno = EBADF;
        return -1;
    }

    // 检查socket准备状态
    ssize_t prepareResult = PrepareForSocketRecv(buf, len);
    if (prepareResult != 1) {  // 1表示准备就绪，可以接收数据
        return prepareResult;  // 返回-1表示错误，0表示超时或无数据
    }

    // 执行实际的数据接收和处理
    return PerformSocketRecv(buf, len);
}

int SocketModuleClient::SocketClose()
{
    if (socketFd_ < 0) {
        HILOG_WARN(HILOG_MODULE_ACE, "HandleSocketClose: no socket available in client");
        return -1;
    }
    HILOG_INFO(HILOG_MODULE_ACE, "SocketClose: sockfd=%d", socketFd_);
    int ret = lwip_close(socketFd_);
    if (ret == 0) {
        HILOG_INFO(HILOG_MODULE_ACE, "Socket %d closed successfully", socketFd_);
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to close socket %d, errno=%d", socketFd_, errno);
    }
    socketFd_ = -1;
    return ret;
}

int SocketModuleClient::SocketGetAddrInfo(
    const char *nodename, const char *servname, const struct addrinfo *hints, struct addrinfo **res)
{
    HILOG_INFO(HILOG_MODULE_ACE,
        "SocketGetAddrInfo: nodename=%s, servname=%s", nodename ? nodename : "null",
        servname ? servname : "null");
    int ret = lwip_getaddrinfo(nodename, servname, hints, res);
    HILOG_INFO(HILOG_MODULE_ACE, "SocketGetAddrInfo result: %d", ret);
    return ret;
}
}  // namespace ACELite
}  // namespace OHOS
