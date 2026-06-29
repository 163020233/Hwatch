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

#ifndef HTTP_CALLBACK_H
#define HTTP_CALLBACK_H

#include "jsi.h"
#include "httpclient.h"
#include "http_errcode.h"
#include "ace_mem_base.h"

namespace OHOS {
namespace ACELite {

typedef HTTP_REQUEST_TYPE HttpRequestType;
typedef HTTP_ERRCODE HttpErrcode;
typedef HTTPC_RESULT HttpcResult;

enum HttpDataType {
    DATA_TYPE_STRING = 0,
    DATA_TYPE_OBJECT = 1,
    DATA_TYPE_ARRAY_BUFFER = 2
};

enum HttpEventId {
    HTTP_EVENT_RECEIVE = 0,
    HTTP_EVENT_PROGRESS = 1,
    HTTP_EVENT_END = 2,
    HTTP_EVENT_MAX = 3
};

struct HttpResult {
    HttpErrcode resCode;
    int responseCode;
    const char *errMsg;
    uint8_t *data;
    int dataLen;
    int receiveSize;
    int totalSize;
    bool isMore;
    bool isFinish;
    bool isOnce;
    HttpDataType datatype;
    HttpEventId eventId;
    JSIValue context;
    JSIValue callback;
    void *client;
};

class HttpCallback : public MemoryHeap {
public:
    virtual ~HttpCallback() {}
    virtual void CreateCallback(HttpResult* result) {}
    virtual void CreateSubscription(HttpResult* result) {}
};

}  // namespace ACELite
}  // namespace OHOS

#endif