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

#ifndef OHOS_ACELITE_NETWORK_LISTENER_H
#define OHOS_ACELITE_NETWORK_LISTENER_H

#include "lwip_public.h"

#ifdef __cplusplus
extern "C" {
#endif

// 网络类型枚举
typedef enum {
    // short reach
    NETWORK_TYPE_WIFI = 0,
    NETWORK_TYPE_BLUETOOTH = 1,
    NETWORK_TYPE_BLUETOOTH_LE = 2,
    NETWORK_TYPE_BLUETOOTH_PAN = 3,
    NETWORK_TYPE_SLE = 4,
    NETWORK_TYPE_USB = 5,
    // long reach
    NETWORK_TYPE_CELL_LTE_CAT1 = 30,
    NETWORK_TYPE_CELL_4G = 31,
    // unknown
    NETWORK_TYPE_UNKNOWN = 100
} NetworkType;

// 连接状态枚举
typedef enum {
    // to be connected
    CONNECTION_STATUS_CONNECTING = 0,
    CONNECTION_STATUS_CONNECTED = 1,
    // to be disconnected
    CONNECTION_STATUS_DISCONNECTING = 2,
    CONNECTION_STATUS_DISCONNECTED = 3,
    CONNECTION_STATUS_UNKNOWN = 100
} ConnectionState;

void net_connect_status_change(uint8_t type, uint8_t netConnectState);
#ifdef __cplusplus
};
#endif

#endif
