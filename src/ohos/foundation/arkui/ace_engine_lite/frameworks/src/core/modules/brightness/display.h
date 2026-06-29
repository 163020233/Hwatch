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

#ifndef DISPLAY_MODULE_H
#define DISPLAY_MODULE_H

#include <stdint.h>
#include "jsi.h"

namespace OHOS {
namespace ACELite {
class DisplayModule final : public MemoryHeap {
public:
    /**
     * constructor
     */
    DisplayModule() = default;

    /**
     * desconstructor
     */
    ~DisplayModule() = default;

    /**
     * Display interface
     */
    static JSIValue SetKeepScreenOn(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    /**
     * Display interface
     */
    static JSIValue SetMode(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    /**
     * Display interface
     */
    static JSIValue GetMode(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    /**
     * Display interface
     */
    static JSIValue SetValue(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    /**
     * Display interface
     */
    static JSIValue GetValue(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    /**
     * OnTerminate interface
     */    
    static void OnTerminate();
};

void InitBrightnessModule(JSIValue exports);
}  // namespace ACELite
}  // namespace OHOS

#endif  // DISPLAY_MODULE_H
