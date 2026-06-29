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

#include <new>
#include "ace_log.h"
#include "js_app_context.h"
#include "js_async_work.h"
#include "product_adapter.h"
#include <securec.h>
#include "nfc_module.h"
#include "global.h"
#include "screen.h"

namespace OHOS {
namespace ACELite {

const char *const NfcHceModule::HCE_CMD = "hceCmd";
const uint8_t NfcHceModule::AID_DYNAMIC_NUM = 8;
const uint32_t NfcHceModule::RESPONSE_ARRAY_NUM = 256;
const char *const TYPE = "type";
NfcCallBack NfcHceModule::callback_;
HceStackOnData g_hceStackOnData = {.data = 0, .dataLen = 0, .index = 0};

extern "C" void ApduOnToJs(uint8_t *data, uint16_t dataLen)
{
    if ((data == nullptr) || (dataLen == 0)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "ApduOnToJs invalid data");
        return;
    }
    HceParam *param = new HceParam();
    param->data = malloc(dataLen);
    if (param->data == nullptr) {
        delete (param);
        HILOG_ERROR(HILOG_MODULE_ACE, "ApduOnToJs alloc param data failed");
        return;
    }

    if (memset_s(param->data, dataLen, 0, dataLen) != EOK) {
        free(param->data);
        delete (param);
        HILOG_ERROR(HILOG_MODULE_ACE, "ApduOnToJs memset param data failed");
        return;
    }

    if (memcpy_s(param->data, dataLen, data, dataLen) != EOK) {
        free(param->data);
        delete (param);
        HILOG_ERROR(HILOG_MODULE_ACE, "ApduOnToJs memcpy param data failed");
        return;
    }

    param->len = dataLen;
    JsAsyncWork::DispatchAsyncWork(NfcHceModule::HceApduOnCb, static_cast<void *>(param));
}

static bool ValidJsValue(JSIValue value)
{
    return (value != nullptr) && (!JSI::ValueIsUndefined(value));
}

void NfcHceModule::HceApduOnCb(void *data, int8_t statusCode)
{
    int32_t ret;
    if (data == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "HceApduOnCb invalid parameter");
        return;
    }
    HceParam *param = static_cast<HceParam *>(data);
    uint8_t *ptr = nullptr;
    JSIValue arrayBuffer = JSI::CreateArrayBuffer(param->len, ptr);
    if (ptr == nullptr) {
        JSI::ReleaseValue(arrayBuffer);
        free(param->data);
        delete (param);
        HILOG_ERROR(HILOG_MODULE_ACE, "HceApduOnCb get array buffer failed");
        return;
    }

    ret = memcpy_s(ptr, param->len, param->data, param->len);
    if (ret != EOK) {
        JSI::ReleaseValue(arrayBuffer);
        free(param->data);
        delete (param);
        HILOG_ERROR(HILOG_MODULE_ACE, "HceApduOnCb memcpy buffer failed");
        return;
    }

    JSIValue typedArray = JSI::CreateTypedArray(TypedArrayType::JSI_UINT8_ARRAY, param->len, arrayBuffer, 0);
    if (typedArray == nullptr) {
        JSI::ReleaseValue(arrayBuffer);
        free(param->data);
        delete (param);
        HILOG_ERROR(HILOG_MODULE_ACE, "HceApduOnCb CreateTypedArray failed");
        return;
    }

    if (ValidJsValue(callback_.onCallback)) {
        JSIValue argv[ARGC_ONE] = {typedArray};
        JSI::CallFunction(callback_.onCallback, NfcHceModule::callback_.jsCtx, argv, ARGC_ONE);
    }

    JSI::ReleaseValue(arrayBuffer);
    JSI::ReleaseValue(typedArray);
    free(param->data);
    delete (param);
}

JSIValue NfcHceModule::CreateHceService(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    return JSI::CreateUndefined();
    ;
}

JSIValue NfcHceModule::StartHCE(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    JSIValue undefValue = JSI::CreateUndefined();
    callback_.jsCtx = JSI::AcquireValue(thisVal);
    if ((args == nullptr) || (argsNum == 0) || (JSI::ValueIsUndefined(args[0]))) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Start HceCheckPermission failed");
        return JSI::CreateBoolean(false);
        ;
    }
    uint8_t length = JSI::GetArrayLength(args[0]);
    char *aidDynamic[AID_DYNAMIC_NUM] = {0};
    length = (length > AID_DYNAMIC_NUM) ? AID_DYNAMIC_NUM : length;
    for (uint32_t index = 0; index < length; index++) {
        aidDynamic[index] = JSI::JSIValueToString(JSI::GetPropertyByIndex(args[0], index));
    }
    for (uint32_t index = 0; index < length; index++) {
        JSI::ReleaseString(aidDynamic[index]);
    }
    return JSI::CreateBoolean(true);
}

JSIValue NfcHceModule::StopHCE(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    return JSI::CreateBoolean(true);
}

JSIValue NfcHceModule::On(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    if ((args == nullptr) || (argsNum == 0) || (JSI::ValueIsUndefined(args[0]))) {
        HILOG_ERROR(HILOG_MODULE_ACE, "HceServiceModule register on callback failed");
        return JSI::CreateUndefined();
    }
    char *hceCmd = JSI::JSIValueToString(args[0]);
    if (hceCmd == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "JSIValueToString failed");
        return JSI::CreateUndefined();
    }
    if (strcmp(HCE_CMD, hceCmd) != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "NfcHceModule register on callback failed");
        return JSI::CreateUndefined();
    }
    callback_.onCallback = JSI::AcquireValue(args[1]);
    g_hceStackOnData.dataLen = strlen("testdata");
    if (memcpy_s(g_hceStackOnData.data, sizeof(g_hceStackOnData.data), "testdata", strlen("testdata")) != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "HceServiceModule memcpy_s failed");
        return JSI::CreateUndefined();
    }
    ApduOnToJs(g_hceStackOnData.data, g_hceStackOnData.dataLen);
    return JSI::CreateUndefined();
}

JSIValue NfcHceModule::SendResponse(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    uint32_t length = 0;
    uint8_t responseApdu[RESPONSE_ARRAY_NUM] = {0};
    JSIValue undefValue = JSI::CreateUndefined();
    char *aidDynamic[AID_DYNAMIC_NUM] = {0};
    if ((args == nullptr) || (argsNum == 0) || (JSI::ValueIsUndefined(args[0]))) {
        HILOG_ERROR(HILOG_MODULE_ACE, "SendResponse HceCheckPermission failed");
        return JSI::CreateBoolean(false);
    }
    if (JSI::ValueIsArray(args[0])) {
        length = JSI::GetArrayLength(args[0]);
        length = (length > RESPONSE_ARRAY_NUM) ? RESPONSE_ARRAY_NUM : length;
        for (uint32_t i = 0; i < length; i++) {
            JSIValue j = JSI::GetPropertyByIndex(args[0], i);
            responseApdu[i] = static_cast<uint8_t>(JSI::ValueToNumber(j));
            JSI::ReleaseValue(j);
        }
    }
    return JSI::CreateBoolean(true);
}

}  // namespace ACELite
}  // namespace OHOS
