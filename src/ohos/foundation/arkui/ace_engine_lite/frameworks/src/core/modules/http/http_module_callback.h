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

#ifndef HTTP_MODULE_CALLBACK_H
#define HTTP_MODULE_CALLBACK_H

#include "jsi.h"
#include "http_errcode.h"
#include "http_callback.h"

namespace OHOS {
namespace ACELite {

class HttpModuleCallback final : public HttpCallback {
public:

    static HttpModuleCallback* GetInstance()
    {
        static HttpModuleCallback instance;
        return &instance;
    }

    void CreateCallback(HttpResult* result) override;
    void CreateSubscription(HttpResult* result) override;

    static void ExecuteCallFunc(void *params);
    static void ExecuteSubscription(void *params);
private:
    HttpModuleCallback() = default;
    ~HttpModuleCallback() = default;

    static void SendData(HttpResult* result);
    static void SendStateInfo(HttpResult* result);
};

}  // namespace ACELite
}  // namespace OHOS

#endif // HTTP_MODULE_CALLBACK_H