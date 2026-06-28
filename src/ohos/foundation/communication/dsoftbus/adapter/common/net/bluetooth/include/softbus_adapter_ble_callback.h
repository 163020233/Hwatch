/*
 * Copyright (c) 2018 Huawei Device Co., Ltd.
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

#ifndef SOFTBUS_ADAPTER_BLE_CALLBACK_H
#define SOFTBUS_ADAPTER_BLE_CALLBACK_H

#include "ohos_bt_def.h"
#include "ohos_bt_gatt.h"
#include "ohos_bt_gatt_client.h"
#include "ohos_bt_gatt_server.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* 以下为softbus ble gatt适配需开放的回调函数 */
void SoftBusAdvEnableCallback(int advId, int status);
void SoftBusAdvDisableCallback(int advId, int status);
void SoftBusAdvDataCallback(int advId, int status);
void SoftBusAdvUpdateCallback(int advId, int status);
void SoftBusScanResultCallback(BtScanResultData *scanResultdata);

/* 以下为softbus ble gatt client适配需开放的回调函数 */
void SoftBusGattcConnectionStateChangedCallback(int clientId, int connectionState, int status);
void SoftBusGattcSearchServiceCompleteCallback(int clientId, int status);
void SoftBusGattcConfigureMtuSizeCallback(int clientId, int mtuSize, int status);
void SoftBusGattcRegisterNotificationCallback(int clientId, int status);
void SoftBusGattcNotificationCallback(int clientId, BtGattReadData *notifyData, int status);

/* 以下为softbus ble gatt server适配需开放的回调函数 */
void SoftBusBleRegisterServerCallback(int status, int serverId, BtUuid *appUuid);
void SoftBusBleConnectServerCallback(int connId, int serverId, const BdAddr *bdAddr);
void SoftBusBleDisconnectServerCallback(int connId, int serverId, const BdAddr *bdAddr);
void SoftBusBleServiceAddCallback(int status, int serverId, BtUuid *uuid, int srvcHandle);
void SoftBusBleCharacteristicAddCallback(int status, int serverId, BtUuid *uuid, int srvcHandle,
    int characteristicHandle);
void SoftBusBleDescriptorAddCallback(int status, int serverId, BtUuid *uuid,
    int srvcHandle, int descriptorHandle);
void SoftBusBleServiceStartCallback(int status, int serverId, int srvcHandle);
void SoftBusBleServiceStopCallback(int status, int serverId, int srvcHandle);
void SoftBusBleServiceDeleteCallback(int status, int serverId, int srvcHandle);
void SoftBusBleRequestReadCallback(BtReqReadCbPara readCbPara);
void SoftBusBleRequestWriteCallback(BtReqWriteCbPara writeCbPara);
void SoftBusBleResponseConfirmationCallback(int status, int handle);
void SoftBusBleIndicationSentCallback(int connId, int status);
void SoftBusBleMtuChangeCallback(int connId, int mtu);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif
