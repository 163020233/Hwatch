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

#ifndef HTTP_MODULE_H
#define HTTP_MODULE_H

#include <stdint.h>
#include <map>
#include <cstring>
#include "cmsis_os2.h"
#include "jsi.h"
#include "httpclient.h"

namespace OHOS {
namespace ACELite {

class HttpModule final : public MemoryHeap {
public:
    HttpModule() = default;
    ~HttpModule() = default;

    static JSIValue CreateHttp(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue Request(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue RequestInStream(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue Download(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue Upload(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue On(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue Once(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue Off(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue Destroy(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static void OnTerminate();

private:
    static HttpRequestType GetRequestType(const char *method);
    static HttpEventId GetEventId(const char *event);
    static char *HeaderJsonToString(char* headerJson);
};
void InitHttpClientModule(JSIValue exports);

}  // namespace ACELite
}  // namespace OHOS

#endif  // HTTP_MODULE_H