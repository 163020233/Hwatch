/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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
#include "core_main.h"
#include <ohos_init.h>
#include <samgr_lite.h>
// ohos begin
// adapt wearable support
#ifdef LITEWEARABLE_SUPPORT_ENABLED
#include "system_init.h"

#ifndef JS_ENABLE
#ifdef LITE_NATIVE_ABILITY_FWK_SUPPORT
#include "StartNative.h"
#endif
#endif

#endif
// ohos end

void OHOS_SystemInit(void)
{
    MODULE_INIT(bsp);
    MODULE_INIT(device);
    MODULE_INIT(core);
    SYS_INIT(service);
    SYS_INIT(feature);
    MODULE_INIT(run);
    SAMGR_Bootstrap();
    LiteParamService();

// ohos begin
// adapt wearable support
#ifdef LITEWEARABLE_SUPPORT_ENABLED
#ifndef JS_ENABLE
#ifdef LITE_NATIVE_ABILITY_FWK_SUPPORT
    StartNative();
#endif
#endif
#endif
// ohos end
}
