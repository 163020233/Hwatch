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
#ifdef HILOG_ENABLE
#include "ace_log.h"
#endif
#include "js_app_context.h"
#include "js_async_work.h"
#include "product_adapter.h"
#include <securec.h>
#include "geolocation_module.h"
#include "global.h"

namespace OHOS {
namespace ACELite {

using namespace OHOS::Location;

#ifndef HILOG_ENABLE
const char *HILOG_MODULE_ACE_GEOLOCATION = "ACE_GEOLOCATION";
#define HILOG_DEBUG(mod, fmt, ...)  printf("[%s] [DEBUG] [%s] [L:%d]: " fmt "\n", mod, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define HILOG_INFO(mod, fmt, ...)   printf("[%s] [INFO]  [%s] [L:%d]: " fmt "\n", mod, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define HILOG_ERROR(mod, fmt, ...)  printf("[%s] [ERROR] [%s] [L:%d]: " fmt "\n", mod, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define HILOG_WARN(mod, fmt, ...)   printf("[%s] [WARN]  [%s] [L:%d]: " fmt "\n", mod, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#endif

const double LocationModule::LONGITUDE = 123.456789;
const double LocationModule::LATITUDE = 45.678901;
const double LocationModule::ALTITUDE = 100.0;
const double LocationModule::ACCURACY = 10.0;
const uint32_t LocationModule::TIME = 163456789;
const char *const LocationModule::LONGITUDE_NAME = "longitude";
const char *const LocationModule::LATITUDE_NAME = "latitude";
const char *const LocationModule::ALTITUDE_NAME = "altitude";
const char *const LocationModule::ACCURACY_NAME = "accuracy";
const char *const LocationModule::TIME_NAME = "time";
const char *const LocationModule::COORD_TYPES_NAME = "coordType";
const char *const LocationModule::LOCATIONTYPE_NAME = "types";
const char *const LocationModule::LOCATIONTYPE = "GPS";
const char *const LocationModule::TIMEOUT = "timeout";

typedef struct {
    LocationCoordinateSystemType coordinateSystemType;
    const char *const coordTypeString;
} CoordTypeMap;

LocatorCallback *LocationModule::locatorCallback_ = nullptr;

static CoordTypeMap g_coordTypeMap[] = {
    {WGS84, "WGS84"},
    {GCJ02, "GCJ02"},
};

JSIValue LocationModule::GetSupportCoordTypes(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    std::vector<LocationCoordinateSystemType> coordinateSystemTypes;
    LocationErrCode errCode = Locator::GetInstance().QuerySupportCoordinateSystemType(coordinateSystemTypes);
    if (errCode != ERRCODE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE_GEOLOCATION, "QuerySupportCoordinateSystemType failed:%d", errCode);
        return JSI::CreateUndefined();
    }
    uint32_t length = coordinateSystemTypes.size();
    JSIValue rltArray = JSI::CreateArray(length);
    uint32_t rltArrayCnt = 0;
    for (auto type : coordinateSystemTypes) {
        HILOG_INFO(HILOG_MODULE_ACE_GEOLOCATION, "query type:%d", type);
        for (uint32_t i = 0; i < ARRAY_SIZE(g_coordTypeMap); i++) {
            if (g_coordTypeMap[i].coordinateSystemType == type) {
                JSIValue result = JSI::CreateString(g_coordTypeMap[i].coordTypeString);
                JSI::SetPropertyByIndex(rltArray, rltArrayCnt++, result);
                JSI::ReleaseValue(result);
            }
        }
    }
    return rltArray;
}

JSIValue LocationModule::Unsubscribe(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    ExecuteUnsubscribe();
    return JSI::CreateUndefined();
}

JSIValue LocationModule::Subscribe(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    return ExecuteAsyncWork(thisVal, args, argsNum, ExecuteSubscribe);
}

JSIValue LocationModule::GetLocationType(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    return ExecuteAsyncWork(thisVal, args, argsNum, ExecuteGetLocationType);
}

JSIValue LocationModule::GetLocation(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    return ExecuteAsyncWork(thisVal, args, argsNum, ExecuteGetLocation);
}

JSIValue LocationModule::ExecuteAsyncWork(
    const JSIValue thisVal, const JSIValue *args, uint8_t argsNum, AsyncWorkHandler ExecuteFunc)
{
    JSIValue undefValue = JSI::CreateUndefined();
    if ((args == nullptr) || (argsNum == 0) || (JSI::ValueIsUndefined(args[0]))) {
        HILOG_ERROR(HILOG_MODULE_ACE_GEOLOCATION, "Error in obtaining device information parameters");
        return undefValue;
    }
    LocationParams *params = new (std::nothrow) LocationParams();
    if (params == nullptr) {
        return undefValue;
    }
    params->thisVal = JSI::AcquireValue(thisVal);
    params->args = JSI::AcquireValue(args[0]);
    JsAsyncWork::DispatchAsyncWork(ExecuteFunc, reinterpret_cast<void *>(params));
    return undefValue;
}

void LocatorCallback::OnLocationReport(const std::shared_ptr<OHOS::Location::Location> &location)
{
    JSIValue result = JSI::CreateObject();
    JSI::SetNumberProperty(result, LocationModule::LATITUDE_NAME, location->GetLatitude());
    JSI::SetNumberProperty(result, LocationModule::LONGITUDE_NAME, location->GetLongitude());
    JSI::SetNumberProperty(result, LocationModule::ALTITUDE_NAME, location->GetAltitude());
    JSI::SetNumberProperty(result, LocationModule::ACCURACY_NAME, location->GetAccuracy());
    JSI::SetNumberProperty(result, LocationModule::TIME_NAME, static_cast<double>(location->GetTimeStamp()));
    LocationModule::SuccessCallBack(thisVal_, args_, result);
#ifdef HI3322_PRODUCT_FPGA
    HILOG_INFO(HILOG_MODULE_ACE_GEOLOCATION, "[%s]:%f", LocationModule::LATITUDE_NAME, location->GetLatitude());
    HILOG_INFO(HILOG_MODULE_ACE_GEOLOCATION, "[%s]:%f", LocationModule::LONGITUDE_NAME, location->GetLongitude());
    HILOG_INFO(HILOG_MODULE_ACE_GEOLOCATION, "[%s]:%f", LocationModule::ALTITUDE_NAME, location->GetAltitude());
    HILOG_INFO(HILOG_MODULE_ACE_GEOLOCATION, "[%s]:%f", LocationModule::ACCURACY_NAME, location->GetAccuracy());
#endif
    JSI::ReleaseValueList(result, ARGS_END);
}

void LocatorCallback::OnErrorReport(const int errorCode)
{
    HILOG_INFO(HILOG_MODULE_ACE_GEOLOCATION, "error code:%d", errorCode);
}

void LocatorCallback::OnLocatingStatusChange(const int status)
{
    HILOG_INFO(HILOG_MODULE_ACE_GEOLOCATION, "OnLocatingStatusChange status:%d", status);
}

LocatorCallback::~LocatorCallback()
{
    JSI::ReleaseValueList(args_, thisVal_, ARGS_END);
}

int32_t LocationModule::SubscribeAndStartLocating(void *data)
{
    LocationParams *params = reinterpret_cast<LocationParams *>(data);
    bool isEnabled = false;
    LocationErrCode errCode = Locator::GetInstance().IsLocationEnabled(isEnabled);
    if (errCode != ERRCODE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE_GEOLOCATION, "locator IsLocationEnabled failed:%d", errCode);
        return errCode;
    }
    if (isEnabled) {
        HILOG_ERROR(HILOG_MODULE_ACE_GEOLOCATION, "locator IsLocationEnabled");
        return ERRCODE_SUCCESS;
    }
    errCode = Locator::GetInstance().EnableAbility(true);
    if (errCode != ERRCODE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE_GEOLOCATION, "locator EnableAbility failed:%d", errCode);
        return errCode;
    }
    locatorCallback_ = new (std::nothrow) LocatorCallback(params->args, params->thisVal);
    if (locatorCallback_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE_GEOLOCATION, "locator locatorCallback_ null");
        return ERRCODE_FAILURE;
    }
    RequestConfig requestConfig;
    requestConfig.SetPriority(PRIORITY_ACCURACY);
    errCode = Locator::GetInstance().StartLocating(&requestConfig, locatorCallback_);
    if (errCode != ERRCODE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE_GEOLOCATION, "locator StartLocating failed:%d\n", errCode);
        delete locatorCallback_;
        locatorCallback_ = nullptr;
        return errCode;
    }
    return ERRCODE_SUCCESS;
}

int32_t LocationModule::UnsubscribeAndStopLocating()
{
    bool isEnabled = false;
    LocationErrCode errCode = Locator::GetInstance().IsLocationEnabled(isEnabled);
    if (errCode != ERRCODE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE_GEOLOCATION, "locator IsLocationEnabled failed:%d", errCode);
        return errCode;
    }
    if (!isEnabled) {
        HILOG_WARN(HILOG_MODULE_ACE_GEOLOCATION, "locator Is not LocationEnabled");
        return ERRCODE_FAILURE;
    }
    errCode = Locator::GetInstance().StopLocating(locatorCallback_);
    if (errCode != ERRCODE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE_GEOLOCATION, "locator StopLocating failed:%d", errCode);
    }
    errCode = Locator::GetInstance().EnableAbility(false);
    if (errCode != ERRCODE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE_GEOLOCATION, "locator EnableAbility failed:%d", errCode);
    }
    if (locatorCallback_ != nullptr) {
        delete locatorCallback_;
        locatorCallback_ = nullptr;
    }
    return ERRCODE_SUCCESS;
}

void LocationModule::ExecuteSubscribe(void *data)
{
    LocationParams *params = reinterpret_cast<LocationParams *>(data);
    if (params == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE_GEOLOCATION, "params or locator_ null");
        return;
    }
    JSIValue args = params->args;
    JSIValue thisVal = params->thisVal;
    int32_t ret = SubscribeAndStartLocating(data);
    if (ret != ERRCODE_SUCCESS) {
        FailCallBack(thisVal, args, -1);
    }
    delete params;
}

void LocationModule::ExecuteUnsubscribe()
{
    int32_t ret = UnsubscribeAndStopLocating();
    if (ret != ERRCODE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE_GEOLOCATION, "UnsubscribeAndStopLocating failed:%d", ret);
    }
}

void LocationModule::ExecuteGetLocationType(void *data)
{
    LocationParams *params = reinterpret_cast<LocationParams *>(data);
    if (params == nullptr) {
        return;
    }
    JSIValue args = params->args;
    JSIValue thisVal = params->thisVal;
    JSIValue result = JSI::CreateObject();
    JSIValue LocationType = JSI::CreateString(LOCATIONTYPE);
    JSIValue typeArray = JSI::CreateArray(1);
    JSI::SetPropertyByIndex(typeArray, 0, LocationType);

    JSI::SetNamedProperty(result, "types", typeArray);
    SuccessCallBack(thisVal, args, result);

    JSI::ReleaseValueList(args, thisVal, result, typeArray, LocationType, ARGS_END);
    delete params;
}

void LocationModule::ExecuteGetLocation(void *data)
{
    LocationParams *params = reinterpret_cast<LocationParams *>(data);
    if (params == nullptr) {
        return;
    }
    JSIValue args = params->args;
    JSIValue thisVal = params->thisVal;
    double timeOut = JSI::GetNumberProperty(args, TIMEOUT);
    char *coordType = JSI::GetStringProperty(args, COORD_TYPES_NAME);
    JSIValue result = JSI::CreateObject();
    JSI::SetNumberProperty(result, LONGITUDE_NAME, LONGITUDE);
    JSI::SetNumberProperty(result, LATITUDE_NAME, LATITUDE);
    JSI::SetNumberProperty(result, ALTITUDE_NAME, ALTITUDE);
    JSI::SetNumberProperty(result, ACCURACY_NAME, ACCURACY);
    JSI::SetNumberProperty(result, TIME_NAME, static_cast<double>(TIME));
    SuccessCallBack(thisVal, args, result);
    JSI::ReleaseString(coordType);
    JSI::ReleaseValueList(args, thisVal, result, ARGS_END);
    delete params;
}

void LocationModule::SuccessCallBack(const JSIValue thisVal, const JSIValue args, JSIValue jsiValue)
{
    if (JSI::ValueIsUndefined(args)) {
        return;
    }
    JSIValue success = JSI::GetNamedProperty(args, CB_SUCCESS);
    JSIValue complete = JSI::GetNamedProperty(args, CB_COMPLETE);
    if (!JSI::ValueIsUndefined(success)) {
        if (JSI::ValueIsUndefined(jsiValue)) {
            JSI::CallFunction(success, thisVal, nullptr, 0);
        } else {
            JSI::CallFunction(success, thisVal, &jsiValue, ARGC_ONE);
        }
    }
    if (!JSI::ValueIsUndefined(complete)) {
        JSI::CallFunction(complete, thisVal, nullptr, 0);
    }
    JSI::ReleaseValueList(success, complete, ARGS_END);
}

void LocationModule::FailCallBack(const JSIValue thisVal, const JSIValue args, int ret)
{
    if (JSI::ValueIsUndefined(args)) {
        return;
    }
    JSIValue errInfo = nullptr;
    errInfo = JSI::CreateString("GetDeviceInfo failed");
    JSIValue errCode = JSI::CreateNumber(ret);
    JSIValue fail = JSI::GetNamedProperty(args, CB_FAIL);
    JSIValue complete = JSI::GetNamedProperty(args, CB_COMPLETE);
    JSIValue argv[ARGC_TWO] = {errInfo, errCode};
    if (!JSI::ValueIsUndefined(fail)) {
        JSI::CallFunction(fail, thisVal, argv, ARGC_TWO);
    }
    if (!JSI::ValueIsUndefined(complete)) {
        JSI::CallFunction(complete, thisVal, nullptr, 0);
    }
    JSI::ReleaseValueList(fail, complete, errInfo, errCode, ARGS_END);
}

void LocationModule::OnTerminate()
{
    UnsubscribeAndStopLocating();
}

}  // namespace ACELite
}  // namespace OHOS
