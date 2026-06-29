/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef BLE_MODULE_COMMON_H
#define BLE_MODULE_COMMON_H

#include <stdint.h>
#include <map>
#include <vector>
#include <string>
#include "jsi.h"
#include "errcode.h"
#include "non_copyable.h"
#include "js_async_work.h"
#include "bs_ble_gatt_client.h"
#include "bs_ble_gatt_server.h"
#include "bs_ble_service_api.h"
#include "bs_ble_service_config.h"
#include "bs_device_mgr.h"

namespace OHOS {
namespace ACELite {

#define MAX_CHARACTERISTICVALUE_LEN 128
#define MAX_BLE_NAME_LEN 10
#define MIN_BLE_INTERVEL_LEN 48
#define MAX_BLE_INTERVEL_LEN 96
#define MAX_ADV_DATA_LEN 31
#define NOMAL_MTU_VALUE 512
#define NOMAL_ADV_ID 3
#define MAX_DEVICE_NUM 16

#define BLE_ADDR_LEN 6
#define BLE_INVALID_CHARACTERISTIC_HANDLE 65530
#define BLE_INVALID_DESCRIPTOR_HANDLE 65531
#define BLE_SCAN_INTERVAL_COEF 10
#define MS_TO_US(ms)  ((ms) * 1000UL)
#define BLE_DEFAULT_SCAN_INTERVAL 480
#define BLE_MAX_SCAN_RESULT 20

// 蓝牙广播数据类型定义
#define BLE_AD_TYPE_FLAGS 0x01
#define BLE_AD_TYPE_16BIT_SERVICE_UUID 0x03
#define BLE_AD_TYPE_32BIT_SERVICE_UUID 0x05
#define BLE_AD_TYPE_128BIT_SERVICE_UUID 0x07
#define BLE_AD_TYPE_COMPLETE_LOCAL_NAME 0x09
#define BLE_AD_TYPE_MANUFACTURER_SPECIFIC_DATA 0xFF
#define BLE_AD_TYPE_SERVICE_DATA_16BIT 0x16
#define BLE_AD_TYPE_SERVICE_DATA_32BIT 0x20
#define BLE_AD_TYPE_SERVICE_DATA_128BIT 0x21
// 广播数据类型常量定义
constexpr const char* ADVERTISING_DATA = "advertisingData";
constexpr const char* ADVERTISING_RESPONSE = "advertisingResponse";
constexpr const char* SERVICE_UUIDS = "serviceUuids";
constexpr const char* MANUFACTURE_DATA = "manufactureData";
constexpr const char* SERVICE_DATA = "serviceData";
constexpr const char* MANUFACTURE_ID = "manufactureId";
constexpr const char* MANUFACTURE_VALUE = "manufactureValue";
constexpr const char* SERVICE_UUID = "serviceUuid";
constexpr const char* SERVICE_VALUE = "serviceValue";
constexpr const char* INCLUDE_DEVICE_NAME = "includeDeviceName";

constexpr uint8_t ARGC_FOUR = 4;

struct EventCallback {
    JSIValue callback;
    JSIValue context;
};

enum ErrNum {
    BLE_SUCCESS = 0,
    BLE_PERMISSION_DENIED = 201,
    BLE_INVALID_PARAMETER = 401,
    BLE_UNSUPPORT = 801,
    BLE_SERVICE_STOP = 2900001,
    BLE_SERVICE_NOT_FOUND = 2900002,
    BLE_READ_FORBID = 2901000,
    BLE_OPRATION_FAILED = 2900099,
};

enum class MatchType {
    NONE,
    CHARACTERISTIC,
    DESCRIPTOR
};

enum ChannelMap {
    BLE_ONE_CHANEL = 0x1,
    BLE_THREE_CHANEL = 0x7,
};

enum ScanDuty {
    SCAN_MODE_LOW_POWER = 0,
    SCAN_MODE_BALANCED = 1,
    SCAN_MODE_LOW_LATENCY = 2,
    SCAN_MODE_NONE,
};

enum MatchMode {
    MATCH_MODE_AGGRESSIVE = 1,
    MATCH_MODE_STICKY = 2,
    MATCH_MODE_NONE,
};

enum PhyType {
    PHY_LE_1M = 1,
    PHY_LE_ALL_SUPPORTED = 255,
    PHY_LE_NONE,
};

enum ScanReportMode {
    SCAN_REPORT_MODE_NORMAL = 1,
    SCAN_REPORT_MODE_NONE,
};

enum DeviceType {
    BLE_CLIENT = 0,
    BLE_SERVER,
};

struct BLEDescriptor {
    std::string serviceUuid;
    std::string characteristicUuid;
    std::string descriptorUuid;
    std::vector<uint8_t> descriptorValue;
    bs_gattc_discovery_descriptor_result_t descriptor;
};

struct GattProperties {
    bool write;
    bool writeNoResponse;
    bool read;
    bool notify;
    bool indicate;
};

struct BLECharacteristic {
    std::string serviceUuid;
    std::string characteristicUuid;
    std::vector<uint8_t> characteristicValue;
    std::vector<BLEDescriptor> descriptors;
    GattProperties properties;
    bs_gattc_discovery_character_result_t character;
    uint16_t service_start_hdl;
    uint16_t service_end_hdl;
};

struct CallFuncAsyncParams : public MemoryHeap {
    ACE_DISALLOW_COPY_AND_MOVE(CallFuncAsyncParams);
    CallFuncAsyncParams() : err(nullptr), result(nullptr), callback(nullptr), context(nullptr),
        type(""), connId(0), characteristicArgs(nullptr) {}

    JSIValue err;
    JSIValue result;
    JSIValue callback;
    JSIValue context;
    std::string type;
    uint16_t connId;
    BLECharacteristic *characteristicArgs;
};

struct BleScanFilter {
    std::string deviceId;
    std::string name;
    std::string serviceUuid;
    std::string serviceUuidMask;
    std::string serviceSolicitationUuid;
    std::string serviceSolicitationUuidMask;
    std::vector<uint8_t> serviceData;
    std::vector<uint8_t> serviceDataMask;
    uint16_t manufactureId = 0;
    std::vector<uint8_t> manufactureData;
    std::vector<uint8_t> manufactureDataMask;
};

struct ScanOptions {
    double interval;
    ScanDuty dutyMode;
    MatchMode matchMode;
    PhyType phyType;
    ScanReportMode reportMode;
};

struct ScanResult {
    std::string deviceId;
    int8_t rssi;
    std::vector<uint8_t> data;
    std::string deviceName;
    bool connectable;
};

struct AddrInfo {
    std::string deviceName;
    uint8_t addrType;
};

struct BleParsedResult {
    std::vector<std::string> serviceUuids;                     // 服务UUID
    std::vector<std::string> serviceSolicitationUuids;         // 服务请求UUID
    std::vector<std::pair<std::string, std::vector<uint8_t>>> serviceDatas; // 服务数据
    uint16_t manufactureId = 0;                                // 厂商ID
    std::vector<uint8_t> manufactureData;                      // 厂商数据
};

struct ServiceData {
    std::string serviceUuid;
    std::vector<uint8_t> serviceValue;
};

struct ManufactureData {
    uint16_t manufactureId;
    std::vector<uint8_t> manufactureValue;
};

struct AdvertiseSetting {
    double interval;
    double txPower;
    bool connectable;
};

struct AdvertiseData {
    std::vector<std::string> serviceUuids;
    std::vector<ManufactureData> manufactureData;
    std::vector<ServiceData> serviceData;
    bool includeDeviceName;
};

struct AdvertisingParams {
    AdvertiseSetting advertisingSettings;
    AdvertiseData advertisingData;
    AdvertiseData advertisingResponse;
    uint32_t duration;
};

enum ProfileConnectionState {
    STATE_DISCONNECTED = 0,
    STATE_CONNECTING = 1,
    STATE_CONNECTED = 2,
    STATE_DISCONNECTING = 3,
};

enum GattWriteType {
    WRITE = 1,
    WRITE_NO_RESPONSE = 2,
};

enum AdvertisingState {
    STARTED = 1, // advertising started.
    ENABLED = 2, // advertising temporarily enabled.
    DISABLED = 3, // advertising temporarily disabled.
    STOPPED = 4, // advertising stopped.
};

struct AdvertisingStateChangeInfo {
    uint32_t advertisingId;
    AdvertisingState state;
};

struct BLEConnectionChangeState {
    std::string deviceId;
    ProfileConnectionState state;
};

struct GattService {
    std::string serviceUuid;
    bool isPrimary;
    std::vector<BLECharacteristic> characteristics;
    std::vector<GattService> includeServices;
    bs_gattc_discovery_service_result_t service;
    GattService() : isPrimary(true)
    {
        (void)memset_s(&service, sizeof(bs_gattc_discovery_service_result_t),
            0, sizeof(bs_gattc_discovery_service_result_t));
    }
};

// 发现过程上下文，管理单个连接的服务发现状态
struct DiscoveryContext {
    uint16_t connId;
    JSIValue callback;
    JSIValue thisVal;
    uint32_t currentServiceIndex;
    uint32_t currentCharIndex;
    uint16_t expectedDescCount;   // 预期描述符总数
    uint16_t receivedDescCount;   // 已接收描述符计数

    DiscoveryContext() : connId(0), callback(JSI::CreateUndefined()), thisVal(JSI::CreateUndefined()),
        expectedDescCount(0), receivedDescCount(0) {}
};

// 读写请求上下文，用于匹配请求和回调结果
struct GattOperationContext {
    enum Type {
        READ_CHARACTERISTIC,
        WRITE_CHARACTERISTIC,
        READ_DESCRIPTOR,
        WRITE_DESCRIPTOR,
        READ_RSSI
    };

    Type type;
    uint16_t connId; // 关联的连接ID
    std::string serviceUuid;
    std::string charUuid;
    std::string descUuid;
    uint16_t charHandle;
    uint16_t descHandle;
    JSIValue callback;
    JSIValue thisVal;
    GattWriteType writeType; // 仅用于写操作
};

struct DiscoveryData {
    uint16_t connId;
    JSIValue callback;
    JSIValue thisVal;

    ~DiscoveryData()
    {
        JSI::ReleaseValueList(callback, thisVal);
    }
};

struct DispatchAsync {
    JSIValue err;
    JSIValue result;
    JSIValue callback;
    JSIValue thisVal;
    std::string type;
    uint16_t connId;
    BLECharacteristic characteristicArgs;
};

// 服务构建上下文
struct ServiceBuildContext {
    GattService* service;    // 包含句柄的结果服务
    JSIValue callback;
    JSIValue thisVal;
};

// 删除服务上下文
struct RemoveServiceContext {
    JSIValue callback;
    JSIValue thisVal;
    std::string serviceUuid;
    uint16_t serviceHandle;
};

// 通知特征值结构
struct NotifyCharacteristicContext {
    std::string deviceId;
    std::string serviceUuid;
    std::string characteristicUuid;
    std::vector<uint8_t> characteristicValue;
    bool confirm; // true: indicate, false: notify
    JSIValue callback;
    JSIValue thisVal;
};

struct ReadWriteRequestContext {
    std::string deviceId;
    uint16_t transId;
    std::vector<uint8_t> value;
    std::string serviceUuid;
    std::string characteristicUuid;
    std::string descriptorUuid;
    bool needRsp;
};

// 句柄到特征值信息的映射
struct CharacteristicInfo {
    std::string serviceUuid;
    std::string characteristicUuid;
};

struct DescriptorInfo {
    std::string serviceUuid;
    std::string characteristicUuid;
    std::string descriptorUuid;
};

struct DeviceInfo {
    uint32_t mtu;
    uint16_t connId;
    ProfileConnectionState connState;
    DeviceType deviceType;
};

bool CompareHexStrings(const std::string& str1, const std::string& str2);
errcode_t DecodeUuidFromStr(const char *uuidStr, uint8_t *uuid, uint32_t *cnt);
std::string FormatMacAddress(const uint8_t addr[BLE_ADDR_LEN]);
std::string GetUUidStr(const bt_uuid_t &uuid);
void GetAttValueData(uint8_t *dataVec, int dataVecLen, bool enable, bool indication);
errcode_t SetJsiBLECharacteristic(JSIValue jsiBleCharacteristic, uint16_t connId,
    BLECharacteristic *bleCharacteristicArgs);
void BleDispatchAsyncWork(const DispatchAsync &asyncParameter);
void GapBleStackStateCbk(uint8_t state);
void GapBleAdvStateCbk(uint8_t advId, uint8_t status);
void GattcIndicationCbk(uint16_t connId, uint8_t client_id, bs_gattc_handle_value_t *data, errcode_t status);
void GattcNotificationdCbk(uint16_t connId, uint8_t client_id, bs_gattc_handle_value_t *data, errcode_t status);
void GattsStartServiceCbk(uint8_t server_id, uint8_t handle, errcode_t status);
void GattsStopServiceCbk(uint8_t server_id, uint8_t handle, errcode_t status);
void GattsReadRequestCbk(uint16_t connId, uint8_t serverId, bs_gatts_req_read_cb_t *readCbPara, errcode_t status);
void GattsWriteRequestCbk(uint16_t conn_id, uint8_t server_id, bs_gatts_req_write_cb_t *write_cb_para, errcode_t status);
void GattMtuChangedCbk(uint16_t conn_id, uint8_t server_id, uint16_t mtu_size, errcode_t status);
void GattcWriteCfmCbk(uint16_t conn_id, uint8_t client_id, uint16_t handle, uint16_t status);
void GattcReadCfmCbk(uint16_t conn_id, uint8_t client_id, bs_gattc_handle_value_t *read_result, uint16_t status);
void GattcGetServicesCbk(uint16_t conn_id, uint8_t client_id, uint8_t service_count,
    bs_gattc_find_service_result_node_t *service, errcode_t status);
void GapBleConnStateChangeCbk(bs_ble_connection_result_t *result);
void GapBleScanResultCbk(const bs_ble_discovery_result_t *discovery_result, const uint8_t *data, uint16_t len);
void WorkerThreadFunc(void *argument);
void GetCharacteristic(const JSIValue *args, BLECharacteristic &bleCharacteristicArgs, bool isRead);
bool ValidateWriteParameters(uint8_t argsSize, const JSIValue* args);
void GattcReadRssiCbk(uint16_t connId, int8_t rssi, uint8_t status);
// 解析参数函数
errcode_t ParseWriteArguments(const JSIValue* args, BLECharacteristic& bleCharacteristicArgs,
    GattWriteType& writeType);
std::string ParseDeviceId(const JSIValue args);
void GetProperties(uint8_t properties, GattProperties &props);
void CreateGattServices(JSIValue *rltArray, const std::vector<GattService> &services);
JSIValue ConstructJSIResult(const bs_gattc_handle_value_t *read_result, const BLECharacteristic *charFound,
    const BLEDescriptor *descFound, const std::string &serviceUuid, const std::string &charUuid,
    const std::string &descUuid);
BleParsedResult ParseMixedAdvertisingData(const std::vector<uint8_t> &data);
bool ReadMatchFilter(const BleScanFilter& filter, const BleParsedResult& parsedResult,
    const ScanResult& scanResultTmp);
void UpdateScanResult(const ScanResult &scanResultTmp, std::vector<ScanResult>::iterator existingIt,
    std::vector<ScanResult> &scanResultList);

// gatts
JSIValue CreateJsiGattService(const GattService& service);
void RemoveAllCharacteristicHandlesByService(const std::string& serviceUuid);

}  // namespace ACELite
}  // namespace OHOS
#endif  // BLE_MODULE_COMMON_H