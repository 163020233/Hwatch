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

#ifndef HTTP_MODULE_CLIENT_H
#define HTTP_MODULE_CLIENT_H

#include <mutex>
#include <atomic>
#include <functional>
#include "cmsis_os2.h"
#include "http_errcode.h"
#include "httpclient.h"
#include "http_module_callback.h"

namespace OHOS {
namespace ACELite {

enum HttpRequestId {
    HTTP_REQUEST,
    HTTP_REQUESTINSTREAM,
    HTTP_DOWNLOAD,
    HTTP_UPLOAD,
    HTTP_FINISH,
    HTTP_MAP_REQUESTINSTREAM,
};

struct HttpParam {
    const char *url;
    const char *header;
    const char *filePath;
    const char *clientCert;
    char *postBuf;
};

struct HttpMessage {
    HttpRequestId requestId;
    HttpRequestType requestType;
    HttpParam param;
    int dataType;
    JSIValue context;
    JSIValue callback;
    uint32_t mapCallbackId;
};

struct HttpRequest {
    JSIValue request;
    JSIValue requestInStream;
    JSIValue download;
    JSIValue upload;
    JSIValue on;
    JSIValue once;
    JSIValue off;
    JSIValue destroy;
};

struct HttpEvent {
    JSIValue callback;
    bool isOnce;
};

typedef std::function<void(const char* url)> MapCallback;
typedef std::function<void(int32_t id, const char* url)> ManagerMapCallback;

class HttpModuleClient final : public MemoryHeap {
public:
    HttpModuleClient();
    ~HttpModuleClient();

    bool IsRunning(void) const
    {
        return isRunning_.load();
    }

    void InitCallback(HttpCallback* cbkInstance)
    {
        cbkInstance_ = cbkInstance;
    }

    void SetMapCallback(ManagerMapCallback callback)
    {
        mapCallback_ = callback;
    }

    void Request(HttpParam &param, HttpRequestType requestType, int expectDataType, JSIValue context, JSIValue callback);
    void RequestInStream(HttpParam &param, HttpRequestType requestType, int expectDataType, JSIValue context, JSIValue callback);
    void Download(HttpParam &param, JSIValue context, JSIValue callback);
    void Upload(HttpParam &param, JSIValue context, JSIValue callback);
    void RequestInStreamMap(HttpParam &param, HttpRequestType requestType, uint32_t id);

    bool InitClient(void);
    bool CreateHttpThread(void);
    void StopThread(void);
    osStatus_t HttpMessagePut(HttpMessage *message);
    osStatus_t HttpMessageGet(HttpMessage *message);
    HttpErrcode GetErrorCode(int errorCode);

    void SetEventCallback(JSIValue callback, bool isOnce, HttpEventId id);
    JSIValue GetEventCallback(HttpEventId id, bool &isOnce);

    HttpClient client_{};
    HttpClientData clientData_{};
    osMessageQueueId_t httpQueueId_ = nullptr;
    HttpRequest request_;
    HttpEvent httpEvents_[HTTP_EVENT_MAX];
    void ReleaseThread(void);
    void CloseClient(void);
private:
    void ReleaseJSIValue(JSIValue value)
    {
        if (!JSI::ValueIsUndefined(value)) {
            JSI::ReleaseValue(value);
        }
    }

    HttpDataType GetContentType(int expectDataType);
    void CheckSubscription(HttpResult result);

    void ResetClient(void);
    
    static const uint32_t REQ_BUF_SIZE = 2048;
    static const uint32_t RSP_BUF_SIZE = 2048;
    std::atomic<bool> isRunning_;
    void *stackMem_ = nullptr;
    osThreadId_t streamId_ = nullptr;
    HttpCallback* cbkInstance_;
    std::mutex clientMutex_;
    ManagerMapCallback mapCallback_;
};

}  // namespace ACELite
}  // namespace OHOS

#endif  // HTTP_MODULE_CLIENT_H
