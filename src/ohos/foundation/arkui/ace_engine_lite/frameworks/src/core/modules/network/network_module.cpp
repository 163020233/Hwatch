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

#include <unistd.h>
#include <securec.h>
#include "ace_log.h"
#include "js_async_work.h"
#include "product_adapter.h"
#include "connection_manage.h"
#include "network_listener.h"
#include "network_module.h"

constexpr uint8_t ARGS_NUM_2 = 2;
constexpr uint8_t IP_ADDR_NUM = 2;

namespace OHOS {
namespace ACELite {
std::map<uint8_t, NetCallBack *> NetWorkModule::callbackSet;

void NetWorkModule::TriggerCallbackListener(const uint8_t networkType, const uint8_t netConnectState)
{
    auto it = callbackSet.find(networkType);
    if (it != callbackSet.end()) {
        NetCallBack *callback = it->second;
        callback->netParam.state = netConnectState;
        callback->netParam.net_type = networkType;
        if (!JsAsyncWork::DispatchAsyncWork(AsyncExecuteCallback, const_cast<NetCallBack *>(callback))) {
            HILOG_ERROR(HILOG_MODULE_ACE, "dispatch async work failed.");
        }
    }
}

static bool ValidJsValue(JSIValue value)
{
    return (value != nullptr) && (!JSI::ValueIsUndefined(value));
}

static JSIValue GetNetworkStateString(uint8_t type, uint8_t state)
{
    // 网络类型
    const char *networkType;
    switch (type) {
        case NETWORK_TYPE_WIFI:
            networkType = "WIFI";
            break;
        case NETWORK_TYPE_BLUETOOTH:
            networkType = "Bluetooth";
            break;
        case NETWORK_TYPE_BLUETOOTH_LE:
            networkType = "Bluetooth LE";
            break;
        case NETWORK_TYPE_BLUETOOTH_PAN:
            networkType = "Bluetooth PAN";
            break;
        case NETWORK_TYPE_CELL_LTE_CAT1:
            networkType = "Cell CAT1";
            break;
        default:
            networkType = "Unknown Network Type";
            break;
    }

    // 连接状态
    const char *connectionState;
    switch (state) {
        case CONNECTION_STATUS_CONNECTING:
            connectionState = "Connecting";
            break;
        case CONNECTION_STATUS_CONNECTED:
            connectionState = "Connected";
            break;
        case CONNECTION_STATUS_DISCONNECTED:
            connectionState = "Disconnected";
            break;
        default:
            connectionState = "Unknown State";
            break;
    }

    // 静态字符串缓冲区，用于返回结果
    static char description[MAX_STATUS_LENGTH];
    int ret =
        snprintf_s(description, sizeof(description), sizeof(description) - 1, "%s: %s", networkType, connectionState);
    if (ret <= 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "result buff is too long");
    }
    JSIValue result = JSI::CreateObject();
    JSI::SetNumberProperty(result, "type", type);
    JSI::SetNumberProperty(result, "state", state);
    JSI::SetStringProperty(result, "description", description);
    return result;
}

void NetWorkModule::AsyncExecuteCallback(void *arg)
{
    if (!arg) {
        HILOG_ERROR(HILOG_MODULE_ACE, "arg is invalid");
        return;
    }

    NetCallBack *callback = static_cast<NetCallBack *>(arg);
    if (ValidJsValue(callback->net_connect_callback)) {
        JSIValue result = GetNetworkStateString(callback->netParam.net_type, callback->netParam.state);
        JSIValue argv[ARGC_ONE] = {result};
        JSI::CallFunction(callback->net_connect_callback, callback->net_connect_thisVal, argv, ARGC_ONE);
        JSI::ReleaseValue(result);
    }
}

JSIValue NetWorkModule::RegisterConnectListener(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    uint8_t type;
    JSIValue callbackArg;
    if (argsNum == 1) {
        type = NETWORK_TYPE_BLUETOOTH;
        callbackArg = args[0];
    } else if (argsNum == ARGS_NUM_2) {
        if (args == nullptr || JSI::ValueIsUndefined(args[0])) {
            HILOG_ERROR(HILOG_MODULE_ACE, "Invalid type argument for registering net connection callback");
            return JSI::CreateUndefined();
        }
        double numValue = JSI::ValueToNumber(args[0]);
        type = static_cast<uint8_t>(numValue);
        callbackArg = args[1];
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid number of arguments for registering net connection callback");
        return JSI::CreateUndefined();
    }
    auto it = callbackSet.find(type);
    if (it != callbackSet.end()) {
        NetCallBack *callback = it->second;
        // 从 map 中移除该回调
        callbackSet.erase(it);
        // 如果找到了对应的回调，先清理旧的回调
        Clean(callback);
    }
    NetCallBack *newCallback = new (std::nothrow) NetCallBack();
    if (newCallback == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to allocate memory for NetCallBack");
        return JSI::CreateUndefined();
    }
    // 保存新的回调
    newCallback->net_connect_thisVal = JSI::AcquireValue(thisVal);
    newCallback->net_connect_callback = JSI::AcquireValue(callbackArg);
    if (newCallback->net_connect_callback == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to register net connection callback");
        delete newCallback;
        return JSI::CreateUndefined();
    }
    newCallback->netParam.net_type = type;
    callbackSet[type] = newCallback;
    ::RegisterConnectListener(type, TriggerCallbackListener);
    return JSI::CreateUndefined();
}

JSIValue NetWorkModule::UnregisterConnectListener(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    if (args == nullptr || argsNum == 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid arguments for unregistering net connection callback");
        return JSI::CreateUndefined();
    }

    uint8_t type;
    if (argsNum < 1) {
        type = NETWORK_TYPE_BLUETOOTH;
    } else {
        if (JSI::ValueIsUndefined(args[0])) {
            HILOG_ERROR(HILOG_MODULE_ACE, "Invalid type argument for unregistering net connection callback");
            return JSI::CreateUndefined();
        }
        double numValue = JSI::ValueToNumber(args[0]);
        type = static_cast<uint8_t>(numValue);
    }
    auto it = callbackSet.find(type);
    if (it != callbackSet.end()) {
        NetCallBack *callback = it->second;
        callbackSet.erase(it);
        Clean(callback);
        ::UnregisterConnectListener(type, TriggerCallbackListener);
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "No callback registered for the given type");
    }

    return JSI::CreateUndefined();
}

JSIValue NetWorkModule::GetConnectState(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    uint8_t net_type = NETWORK_TYPE_UNKNOWN;
    if (argsNum < 1) {
        net_type = NETWORK_TYPE_BLUETOOTH;
    } else {
        net_type = (uint8_t)JSI::ValueToNumber(args[0]);
    }
    uint8_t state = GetNetConnectState(net_type);
    return GetNetworkStateString(net_type, state);
}

JSIValue NetWorkModule::OpenBtPan(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    lwip_open_bt_pan();
    return JSI::CreateUndefined();
}
 
JSIValue NetWorkModule::CloseBtPan(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    lwip_close_bt_pan();
    return JSI::CreateUndefined();
}

static int ParseNetifType(const JSIValue arg, lwip_netif_type &type)
{
    int ret = 0;
    NetworkType networkType = static_cast<NetworkType>(JSI::ValueToNumber(arg));
    switch (networkType) {
        case NETWORK_TYPE_BLUETOOTH_PAN:
            type = LWIP_NETIF_BT_PAN;
            break;
        case NETWORK_TYPE_CELL_LTE_CAT1:
            type = LWIP_NETIF_VOLTE;
            break;
        case NETWORK_TYPE_SLE:
            type = LWIP_NETIF_SLE;
            break;
        case NETWORK_TYPE_USB:
            type = LWIP_NETIF_USB;
            break;
        default:
            ret = -1;
    }
    return ret;
}

JSIValue NetWorkModule::GetIpAddress(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    if (argsNum < ARGC_ONE || argsNum > ARGC_TWO) {
        HILOG_ERROR(HILOG_MODULE_ACE, "getIpAddress params_cnt error!");
        return JSI::CreateUndefined();
    }

    JSIValue callback;
    lwip_netif_type type;
    // 参数解析
    if (argsNum == ARGC_ONE) {
        callback = JSI::AcquireValue(args[0]);
        type = LWIP_NETIF_NONE;
    } else {
        callback = JSI::AcquireValue(args[1]);
        if (ParseNetifType(args[0], type) != 0) {
            JSI::ReleaseValue(callback);
            HILOG_ERROR(HILOG_MODULE_ACE, "getIpAddress networkType param error!");
            return JSI::CreateUndefined();
        }
    }

    // 检查callback参数
    if (!JSI::ValueIsFunction(callback)) {
        JSI::ReleaseValue(callback);
        HILOG_ERROR(HILOG_MODULE_ACE, "getIpAddress callback param error!");
        return JSI::CreateUndefined();
    }

    char ip_addr[IP_ADDR_NUM][MAX_IPADDR_LEN];
    memset_s(ip_addr[0], MAX_IPADDR_LEN, 0, MAX_IPADDR_LEN);
    memset_s(ip_addr[1], MAX_IPADDR_LEN, 0, MAX_IPADDR_LEN);

    // 获取本机IP地址
    int ret = lwip_get_ip_addr(type, ip_addr, IP_ADDR_NUM);
    if (ret != 0) {
        JSIValue err = JSI::CreateNumber(ret);
        JSIValue argv[ARGC_TWO] = {err, JSI::CreateUndefined()};
        JSI::CallFunction(callback, thisVal, argv, ARGC_TWO);
        JSI::ReleaseValueList(err, callback);
        return JSI::CreateUndefined();
    }

    // 将IP地址转成JS数据
    JSIValue ipAddrArray = JSI::CreateArray(IP_ADDR_NUM);
    for (int i = 0; i < IP_ADDR_NUM; i++) {
        JSIValue ipAddr = JSI::CreateString(ip_addr[i]);
        JSI::SetPropertyByIndex(ipAddrArray, i, ipAddr);
        JSI::ReleaseValue(ipAddr);
    }

    // 将数据返回应用层
    JSIValue err = JSI::CreateNumber(0);
    JSIValue argv[ARGC_TWO] = {err, ipAddrArray};
    JSI::CallFunction(callback, thisVal, argv, ARGC_TWO);
    JSI::ReleaseValueList(err, callback, ipAddrArray);
    return JSI::CreateUndefined();
}

void NetWorkModule::Clean(NetCallBack *callbackToClean)
{
    if (callbackToClean == nullptr) {
        return;
    }
    // 释放回调函数资源
    if (callbackToClean->net_connect_callback && JSI::ValueIsFunction(callbackToClean->net_connect_callback)) {
        JSI::ReleaseValue(callbackToClean->net_connect_callback);
    }
    // 释放回调上下文资源
    if (callbackToClean->net_connect_thisVal && !JSI::ValueIsUndefined(callbackToClean->net_connect_thisVal)) {
        JSI::ReleaseValue(callbackToClean->net_connect_thisVal);
    }
    // 删除回调对象
    delete callbackToClean;
}

void NetWorkModule::OnTerminate()
{
    for (auto it = callbackSet.begin(); it != callbackSet.end();) {
        NetCallBack *callback = it->second;
        callbackSet.erase(it);
        Clean(callback);
        it = callbackSet.begin();
    }
}

void InitNetWorkModule(JSIValue exports)
{
    JSI::SetModuleAPI(exports, "registerConnectListener", NetWorkModule::RegisterConnectListener);
    JSI::SetModuleAPI(exports, "unregisterConnectListener", NetWorkModule::UnregisterConnectListener);
    JSI::SetModuleAPI(exports, "getConnectState", NetWorkModule::GetConnectState);
    JSI::SetModuleAPI(exports, "openBtPan", NetWorkModule::OpenBtPan);
    JSI::SetModuleAPI(exports, "closeBtPan", NetWorkModule::CloseBtPan);
    JSI::SetModuleAPI(exports, "getIpAddress", NetWorkModule::GetIpAddress);
    JSI::SetOnTerminate(exports, NetWorkModule::OnTerminate);
}

}  // namespace ACELite
}  // namespace OHOS