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


#include "lwip_public.h"
#include "network_listener.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Defines the callback function net connection status change.
 *
 * @param networkType network type, refer to #NetworkType.
 * @param netConnectState net connection status, refer to #ConnectionState.
 */
typedef void (*NetConnectionCallback)(const uint8_t networkType, const uint8_t netConnectState);

/**
 * @brief This function register network connection status listener.
 * @param[in] networkType network type, refer to #NetworkType.
 * @param[in] callback   net connection callback.
 * @return 0 if success; non-0 otherwise.
 */
int RegisterConnectListener(uint8_t networkType, NetConnectionCallback callback);

/**
 * @brief Defines the callback unregister function for listening net connection status change.
 * @param[in] networkType network type, refer to #NetworkType.
 * @param[in] callback   net connection callback.
 * @return 0 if success; non-0 otherwise.
 */
void UnregisterConnectListener(uint8_t networkType, NetConnectionCallback callback);

/**
 * @brief This function get current network connection status.
 * @param[in] networkType network type, refer to #NetworkType.
 * @return netConnectState net connection status, refer to #ConnectionState.
 */
uint8_t GetNetConnectState(uint8_t networkType);

void NetConnectChange(uint8_t networkType, uint8_t netConnectState);

#ifdef __cplusplus
}
#endif