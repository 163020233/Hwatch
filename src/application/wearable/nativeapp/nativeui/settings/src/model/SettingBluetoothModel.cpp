/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingSettingBluetoothModel
 * Created: 2025-06-05
 */

#include <cstring>
#include <stdint.h>
#include <stdlib.h>
#include "common_def.h"
#include "tcxo.h"
#include "graphic_service.h"
#include "osal_task.h"
#include "dfx_channel.h"
#include <cstring>
#include "bts_def.h"
#include "bts_a2dp_sink.h"
#include "bts_a2dp_source.h"
#include "bts_gatt_stru.h"
#include "bts_gatt_server.h"
#include "bts_le_gap.h"
#include "bts_avrcp_target.h"
#include "bt_audio_hal_interface.h"
#include "bs_ble_gatt_server.h"
#include "ux_api_bt.h"
#include "adapter_bt_br.h"
#include "a2dp_sink_service.h"
#include "network/network_listener.h"
#if defined(SUPPORT_LWIP) && defined(SUPPORT_BREDR)
#include "lwip_public.h"
#endif
#include "NativeAbility.h"
#include "uiservice/ui_service.h"
#include "msg_center.h"
#include "securec.h"
#include "msg_center_msg.h"
#include "msg_center_protocol.h"
#include "client/msg_center_client.h"
#include "main/PullUpDownView.h"
#include "player/PlayersModel.h"
#include "phoneservice/PhoneAudio.h"
#if defined(SUPPORT_ALIPAY_SEC) || defined(SUPPORT_ALIPAY_HSC32I1)
#include "alipay_feature.h"
#endif
#include "ui_resource_settings.h"
#include "findphone/FindPhoneMainPage.h"
#include "settings/model/SettingBtHeadset.h"
#include "settings/page/SettingConnectionOpen.h"
#include "settings/page/SettingBluetoothPage.h"
#include "settings/model/SettingBluetoothModel.h"

namespace OHOS {
constexpr uint16_t SETTING_BLUETOOTH_CONN_HANDLE = 64;
constexpr uint16_t SETTING_BLUETOOTH_INTERVAL_TOMAX_MIN = 12;
constexpr uint16_t SETTING_BLUETOOTH_INTERVAL_TOMAX_MAX = 12;
constexpr uint16_t SETTING_BLUETOOTH_INTERVAL_TODEFAULT_MIN = 24;
constexpr uint16_t SETTING_BLUETOOTH_INTERVAL_TODEFAULT_MAX = 48;
constexpr uint16_t SETTING_BLUETOOTH_LATENCY = 0;
constexpr uint16_t SETTING_BLUETOOTH_TIMEOUT_MUTIPLIER = 500;
static constexpr uint16_t BLUETOOTH_MAX_NUM = 20;
static constexpr int16_t SETTING_ADDR0 = 0;
static constexpr int16_t SETTING_ADDR1 = 1;
static constexpr int16_t SETTING_ADDR2 = 2;
static constexpr int16_t SETTING_ADDR3 = 3;
static constexpr int16_t SETTING_ADDR4 = 4;
static constexpr int16_t SETTING_ADDR5 = 5;
static constexpr int16_t BLUETOOTH_ADDR_SET = 8;
static constexpr int16_t WRITECB_PARA_LEGNTH_TWO = 2;
static constexpr int16_t WRITECB_PARA_LEGNTH_NINE = 9;
static constexpr int16_t WRITECB_PARA_LEGNTH_TEN = 10;
static constexpr int16_t WAIT_TIME = 50;
#define MEDIA_A2DP_SNK_INTERFACE 1
#define A2DP_STREAM_ACCEPT_TIMER_PERIOD 500

#define GATT_CHARAC_TYPE 0
#define GATT_DESC_TYPE 1

#define GATT_DEFAULT_SERVER_UUID { 0x1F, 0xFF }
#define GATT_DEFAULT_SERVICE_UUID { 0x1F, 0xF0 }
#define GATT_CTKD_CHARACTER_UUID { 0x1F, 0xF1 }
#define GATT_DAIG_CHARACTER_UUID { 0x1F, 0xF2 }
#define GATT_DAIG_DESCRIPT_UUID { 0x29, 0x02 }
#define GATT_UUID_DEFAULT_LEN 2
#define GATT_DEFAULT_CCC_VAL { 0x01, 0x00 }

#define GATT_CTKD_ENABLE  0X01
#define GATT_CTKD_DISABLE 0X00
#define GATT_CTKD_CMD_LEN 1
#define PHONE_DEVICE_CLASS 0x200
uint16_t g_ctkd_gatt_value = 0;
uint16_t g_diag_gatt_val = 0;
uint8_t g_ccc_gatt_val[] = GATT_DEFAULT_CCC_VAL;

AudioFormatConfig g_setting_audio_format_config = {AUDIO_DEFAULT, 0, 0};
static uint16_t g_sample_svr_handle = 0;
static uint16_t g_sample_characteristic_handle = 0;
static uint16_t g_diag_characteristic_handle = 0;
static uint16_t g_diag_desc_handle = 0;
constexpr uint16_t g_sample_adv_id = 2;
static bd_addr_t g_bd_addr = { 0 };
static ux_api_bt_addr_t g_lasted_remote_ble_addr = { 0 };

typedef struct {
    uint8_t bd[BD_ADDR_LEN];
    uint8_t stream_state;
} sample_a2dp_snk_connect_info_t;
static sample_a2dp_snk_connect_info_t g_settings_a2dp_snk_connect_info = {0};

#ifndef GATTS_DEFAULT_SERVER_UUID_ID
#define GATTS_DEFAULT_SERVER_UUID_ID 0xFF1F
#endif
#ifndef GATTS_DEFAULT_SERVICE_ID
#define GATTS_DEFAULT_SERVICE_ID 0xF01F
#endif
static uint16_t g_ble_default_service_id = 0;
static uint8_t g_default_ble_server_id = 0;

SettingBluetoothModel::SettingBluetoothModel()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingBluetoothModel::SettingBluetoothModel");
}

SettingBluetoothModel *SettingBluetoothModel::GetInstance(void)
{
    static SettingBluetoothModel instance;
    return &instance;
}

char* SettingBluetoothModel::GetWatchName(void)
{
    return watchName_;
}

// 扫描设备 btEarPhoneList存在数据 设置为1 否则设置为0
void SettingBluetoothModel::SetEarPhoneListHasData(bool status)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingBluetoothModel EarPhoneListHasData_ =  %d", status);
    EarPhoneListHasData_ = status;
}

uint32_t SettingBluetoothModel::GetEarPhoneListHasData()
{
    return EarPhoneListHasData_;
}

void SettingBluetoothModel::SetDeviceName(std::string deviceName)
{
    deviceName_ = deviceName;
}

std::string SettingBluetoothModel::GetDeviceName()
{
    return deviceName_;
}

void SettingBluetoothModel::SetHeadPhoneConnectStatus(bool status)
{
    headPhoneConnectStatus_ = status;
}

bool SettingBluetoothModel::GetHeadPhoneConnectStatus(void)
{
    return headPhoneConnectStatus_;
}

void SettingBluetoothModel::SetConnectFragmentStatus(bool status)
{
    connectFragmentStatus_ = status;
}

bool SettingBluetoothModel::GetConnectFragmentStatus(void)
{
    return connectFragmentStatus_;
}

bool IsMacAddrSame(const unsigned char addr1[BD_ADDR_LEN], const unsigned char addr2[BD_ADDR_LEN])
{
    for (int i = 0; i < BD_ADDR_LEN; ++i) {
        if (addr1[i] != addr2[i]) {
            return false;
        }
    }
    return true;
}

void SettingBluetoothModel::MoveMatchingDeviceToFront()
{
    std::list<BtCaseInfo> matchedItems;
    std::list<BtCaseInfo> otherItems;
    if (btScansDevicesList_.size() == 0) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "MoveMatchingDeviceToFront btScansDevicesList_ size = 0");
        return;
    }

    bd_addr_t bdAddr = *(GetLastConnedAddr());
    for (auto it = btScansDevicesList_.begin(); it != btScansDevicesList_.end(); ++it) {
        if (IsMacAddrSame(bdAddr.addr, it->addr)) {
            matchedItems.push_back(*it);
        } else {
            otherItems.push_back(*it);
        }
    }

    btScansDevicesList_.clear();
    btScansDevicesList_.insert(btScansDevicesList_.begin(), matchedItems.begin(), matchedItems.end());
    btScansDevicesList_.insert(btScansDevicesList_.end(), otherItems.begin(), otherItems.end());
}

void SettingBluetoothModel::UpdateConnectStatusForDevice()
{
    for (auto& item : btScansDevicesList_) {
        if (std::strncmp(reinterpret_cast<const char *>(item.deviceName), deviceName_.c_str(),
                         BT_DEVICE_NAME_MAX_LEN) == 0) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "UpdateConnectStatusForDevice deviceName: %s", item.deviceName);
            item.connect_status = "连接中...";
        }
    }
}

void SettingBluetoothModel::UpdateDisconnectStatusForDevice(const bd_addr_t &bdAddr)
{
    for (auto it = btScansDevicesList_.begin(); it != btScansDevicesList_.end(); ++it) {
        if (IsMacAddrSame(bdAddr.addr, it->addr)) {
            it->connect_status = "连接已经断开";
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "UpdateDisconnectStatusForDevice addr: ****%02x%02x%02x%02x",
                          it->addr[SETTING_ADDR3], it->addr[SETTING_ADDR2], it->addr[1], it->addr[0]);
        }
    }
}

void SettingBluetoothModel::UpdateConnectedStatusForDevice(const bd_addr_t &bdAddr)
{
    for (auto it = btScansDevicesList_.begin(); it != btScansDevicesList_.end(); ++it) {
        if (IsMacAddrSame(bdAddr.addr, it->addr)) {
            it->connect_status = "已连接";
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "UpdateConnectedStatusForDevice addr: ****%02x%02x%02x%02x",
                          it->addr[SETTING_ADDR3], it->addr[SETTING_ADDR2], it->addr[1], it->addr[0]);
        }
    }
}

bool SettingBluetoothModel::GetConnectResultPage()
{
    return redirct_;
}

void SettingBluetoothModel::HandleUnpairRequest(void)
{
    bool ret;
    unused(ret);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "HandleUnpairRequest lastConnedAddr_: ****%02x%02x%02x%02x",
        selectedDeviceInfo_.addr[SETTING_ADDR3], selectedDeviceInfo_.addr[SETTING_ADDR2],
        selectedDeviceInfo_.addr[1], selectedDeviceInfo_.addr[0]);
    ux_api_bt_addr_t addr = {0};
    (void)memcpy_s(addr.addr, UX_API_BT_ADDR_LEN, selectedDeviceInfo_.addr, UX_API_BT_ADDR_LEN);
    addr.type = UX_API_BT_ADDR_TYPE_PUBLIC;
    ret = ux_api_bt_remove_device(&addr, UX_API_BT_PROTOCOL_BR);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "HandleUnpairRequest gap_remove_pair %d", ret);
}

// 是否需要显示连接成功/失败的页面 在连接新耳机或者断开耳机连接时改变为true
void SettingBluetoothModel::SetConnectResultPage(bool redirct)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingBluetoothModel::SetRedirect :%d", redirct);
    redirct_ = redirct;
}

void SettingBluetoothModel::InitBluetoothInfo(void)
{
    errcode_t ret;
    memset_s(watchName_, sizeof(watchName_), 0, sizeof(watchName_));
    ret = ux_api_bt_get_name((unsigned char *)watchName_, BT_DEVICE_NAME_MAX_LEN);
    unsigned char nameLen = strlen(watchName_);
    // 为了实验室测试需要，客户需要自己修改
    if ((nameLen == strlen("WatchDemo")) && (strcmp("WatchDemo", watchName_) == 0)) {
        uint32_t currentUs = uapi_tcxo_get_us();
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "currentUs 0x%x", currentUs);
        // 设置蓝牙名称
        (void)sprintf_s(watchName_, sizeof(watchName_), "Watch%04x", (currentUs & 0xFFFF));
        ret = ux_api_bt_config(UX_API_BT_CONFIG_LOCAL_NAME, (unsigned char*)watchName_, strlen((char*)watchName_));
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "Init BT name %s ret = %u", watchName_, ret);

        //  设置蓝牙地址
        bd_addr_t addr = {{0x1C, 0x1D, 0x2B, 0x0, 0x0, 0x0}, 0};
        addr.addr[3] = currentUs & 0xFF;
        addr.addr[4] = (currentUs & 0XFF00) >> 8;
        addr.addr[5] = currentUs & 0xFF;
        ret = ux_api_bt_config(UX_API_BT_CONFIG_LOCAL_ADDR, addr.addr, sizeof(addr.addr));
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "Init BT addr [0x%x:0x%x] ret = %u", addr.addr[0], addr.addr[1], ret);
    } else {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "watchName_: %s", watchName_);
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "nameLen: %u", nameLen);
    }
}

void SettingBluetoothModel::Init()
{
#ifndef SUPPORT_AUTO_OTA
    InitBluetoothInfo(); // 初始化BT信息，包括名称，地址
    errcode_t ret = ux_api_bt_enable(UX_API_BT_PROTOCOL_BLE); // 使能蓝牙BLE协议栈
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "ux_api_bt_enable ble ret = %u", ret);
    ret = ux_api_bt_enable(UX_API_BT_PROTOCOL_BR); // 使能蓝牙BR协议栈
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "ux_api_bt_enable br ret = %u", ret);

    ClearScansDevicesList();
#endif
}

void SettingBluetoothModel::ClearScansDevicesList()
{
    btScansDevicesList_.clear();
}

bool SettingBluetoothModel::CheckExistScanListAndUpdateRssi(BtCaseInfo caseInfo)
{
    std::list<BtCaseInfo>::iterator node = btScansDevicesList_.begin();
    if (btScansDevicesList_.empty()) {
        return false;
    }
    while (node != btScansDevicesList_.end()) {
        if (IsMacAddrSame(caseInfo.addr, node->addr)) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "true");
            node->rssi = caseInfo.rssi;
            return true;
        }
        node++;
    }
    return false;
}

std::list<BtCaseInfo>& SettingBluetoothModel::GetScansDevicesList()
{
    return btScansDevicesList_;
}

void SettingBluetoothModel::ScansDevicesListAddCase(BtCaseInfo caseInfo)
{
    SetEarPhoneListHasData(true);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "ScansDevicesListAddCase::caseInfo addr: ****%02x%02x%02x%02x",
        caseInfo.addr[SETTING_ADDR3], caseInfo.addr[SETTING_ADDR2], caseInfo.addr[1], caseInfo.addr[0]);
    if (btScansDevicesList_.size() >= BLUETOOTH_MAX_NUM) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "btScansDevicesList_.size is BLUETOOTH_MAX_NUM");
        return;
    } else {
        btScansDevicesList_.push_back(caseInfo);
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "ScansDevicesListAddCase after size: %d", btScansDevicesList_.size());
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "ScansDevicesListAddCase %s", caseInfo.deviceName);
}

void SettingBluetoothModel::ScansDevicesListDelCase(BtCaseInfo caseInfo)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "ScansDevicesListAddCase::ScansDevicesListDel addr: ****%02x%02x%02x%02x",
        caseInfo.addr[SETTING_ADDR3], caseInfo.addr[SETTING_ADDR2], caseInfo.addr[1], caseInfo.addr[0]);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "ScansDevicesListDel %s", caseInfo.deviceName);
    btScansDevicesList_.remove_if([&caseInfo] (BtCaseInfo &each_info) {
        return !memcmp(each_info.addr, caseInfo.addr, sizeof(caseInfo.addr));
    });
}

void SettingBluetoothModel::SetScansCaseRssi(const bd_addr_t *bdAddr, int rssi)
{
    for (auto it = btScansDevicesList_.begin(); it != btScansDevicesList_.end(); it++) {
        if (IsMacAddrSame(it->addr, bdAddr->addr)) {
            it->rssi = rssi;
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SetScansCaseRssi::rssi:%d deviceName:%s", it->rssi, it->deviceName);
        }
    }
}

bool CmpRssi(const BtCaseInfo& a, const BtCaseInfo& b)
{
    if (a.rssi > b.rssi) {
        return true;
    }
    return false;
}

void SettingBluetoothModel::SortScansCaseRssi()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SortScansCaseRssi");
    btScansDevicesList_.sort(CmpRssi);
}

void SettingBluetoothModel::ResetScanDevicesCase(const BtCaseInfo& caseInfo)
{
    for (auto it = btScansDevicesList_.begin(); it != btScansDevicesList_.end(); it++) {
        if (IsMacAddrSame(it->addr, caseInfo.addr)) {
            (void)memcpy_s(it->deviceName, BD_NAME_LEN, caseInfo.deviceName, BD_NAME_LEN);
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ResetScanDevicesCase deviceName:%s", it->deviceName);
        }
    }
}

/* 已配对设备队列 */
void SettingBluetoothModel::SetPairedDevicesListClear()
{
    btPairedDevicesList_->Clear();
}

void SettingBluetoothModel::SetUpPairedDevicesList()
{
    if (btPairedDevicesList_ == nullptr) {
        btPairedDevicesList_ = new List<BtCaseInfo>();
        if (btPairedDevicesList_ == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "btPairedDevicesList_ new fail");
            return;
        }
    }

    SetPairedDevicesListClear();
    gap_local_device_info_t dev_info = { 0 };
    errcode_t ret = gap_br_get_local_device_info(&dev_info);
    if (ret != ERRCODE_BT_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "get local device fail");
        return;
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "get paired dev info, ret = %d, num = %d", ret, dev_info.paired_devices_num);
    if (dev_info.paired_devices_num == 0) {
        return;
    }
    gap_remote_device_info_t *paired_dev_infos =
        (gap_remote_device_info_t *)malloc(sizeof(gap_remote_device_info_t) * dev_info.paired_devices_num);
    if (paired_dev_infos == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "paired_dev_infos malloc error!\n");
        return;
    }
    ret = gap_br_get_paired_devices_list(paired_dev_infos, &dev_info.paired_devices_num);
    if (ret != ERRCODE_BT_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "gap_br_get_paired_devices_list failed, ret = 0x%x\n", ret);
        free(paired_dev_infos);
        return;
    }

    BtCaseInfo caseInfo{};
    btScansDevicesList_.clear();
    for (int i = 0; i < dev_info.paired_devices_num; ++i) {
        if ((paired_dev_infos[i].device_class & 0x200) != 0) { // 0x200 : Major Device Class = phone
            continue;
        }
        (void)memcpy_s(caseInfo.addr, BD_ADDR_LEN, paired_dev_infos[i].addr.addr, BD_ADDR_LEN);
        (void)memcpy_s(caseInfo.deviceName, BD_NAME_LEN, paired_dev_infos[i].name, BD_NAME_LEN);
        caseInfo.connect_status = "连接已经断开";
        AddPairedDevicesCase(caseInfo);
        btScansDevicesList_.push_back(caseInfo);
    }
    free(paired_dev_infos);
}

void SettingBluetoothModel::AddPairedDevicesCase(BtCaseInfo caseInfo)
{
    if (btPairedDevicesList_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "btPairedDevicesList_ nullptr");
        return;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AddPairedDevicesCase::caseInfo addr: ****%02x%02x%02x%02x",
        caseInfo.addr[SETTING_ADDR3], caseInfo.addr[SETTING_ADDR2], caseInfo.addr[1], caseInfo.addr[0]);
    if (btPairedDevicesList_->Size() >= BLUETOOTH_MAX_NUM) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "btPairedDevicesList_->Size is BLUETOOTH_MAX_NUM");
        return;
    } else {
        btPairedDevicesList_->PushBack(caseInfo);
    }
}

List<BtCaseInfo>& SettingBluetoothModel::GetPairedDevicesCase()
{
    return *btPairedDevicesList_;
}

bool SettingBluetoothModel::GetPhoneConnected(void)
{
    return phoneConnectStatus_;
}

void SettingBluetoothModel::SetPhoneConnected(bool status)
{
    phoneConnectStatus_ = status;
}

void SettingBluetoothModel::UpdateConnectStatus()
{
    if (PullUpDownView::GetInstance() != nullptr) {
        PullUpDownView::GetInstance()->UpdatePhoneAndHeadPhoneConnectStatus();
    }
}

void SettingBluetoothModel::SetSelectedDeviceInfo(BtCaseInfo *caseInfo)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SetSelectedDeviceInfo: ****%02x%02x%02x%02x",
                  caseInfo->addr[SETTING_ADDR3], caseInfo->addr[SETTING_ADDR2], caseInfo->addr[1], caseInfo->addr[0]);
    (void)memcpy_s(selectedDeviceInfo_.addr, BD_ADDR_LEN, caseInfo->addr, BD_ADDR_LEN);
    (void)memcpy_s(selectedDeviceInfo_.deviceName, BD_NAME_LEN, caseInfo->deviceName, BD_NAME_LEN);
}

BtCaseInfo *SettingBluetoothModel::GetSelectedDeviceInfo(void)
{
    return &selectedDeviceInfo_;
}

void SettingBluetoothModel::SetLastConnedAddr(const bd_addr_t *addr)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SetLastConnedAddr: ****%02x%02x%02x%02x",
                  addr->addr[SETTING_ADDR3], addr->addr[SETTING_ADDR2], addr->addr[1], addr->addr[0]);
    (void)memcpy_s(lastConnedAddr_.addr, BD_ADDR_LEN, addr->addr, BD_ADDR_LEN);
}

bd_addr_t *SettingBluetoothModel::GetLastConnedAddr(void)
{
    return &lastConnedAddr_;
}

bool SettingBluetoothModel::CheckExistPairedList(BtCaseInfo caseInfo)
{
    if (btPairedDevicesList_ == nullptr) {
        return false;
    }
    ListNode<BtCaseInfo> *node = btPairedDevicesList_ ->Begin();
    if (node == nullptr) {
        return false;
    } else if ((caseInfo.addr[0] == 0) && (caseInfo.addr[1] == 0) &&(caseInfo.addr[SETTING_ADDR2] == 0) &&
               (caseInfo.addr[SETTING_ADDR3] == 0) && (caseInfo.addr[SETTING_ADDR4] == 0) &&
               (caseInfo.addr[SETTING_ADDR5] == 0)) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "CheckExistPairedList false, addr: 000000000000");
        return true;
    }

    while (node != btPairedDevicesList_ ->End()) {
        if (IsMacAddrSame(caseInfo.addr, node->data_.addr)) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "CheckExistPairedList true");
            return true;
        }
        node = node->next_;
    }
    return false;
}

bool SettingBluetoothModel::GetAdvStartState(void)
{
    return advStartState_;
}
void SettingBluetoothModel::SetAdvStartState(bool state)
{
    advStartState_ = state;
}

int SettingBluetoothModel::ConnectRemoteDevice(const bd_addr_t *addr)
{
    ux_api_bt_profile_state_t acl_state = UX_API_BT_PROFILE_STATE_MAX;
    ux_api_bt_profile_state_t a2dp_state = UX_API_BT_PROFILE_STATE_MAX;
    errcode_t retVal = ERRCODE_SUCC;
    if (reinterpret_cast<const ux_api_bt_addr_t *>(addr) == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "addr failed");
        return OHOS_FAILURE;
    }
    ux_api_bt_get_profile_state(UX_API_BT_PROFILE_BR, reinterpret_cast<const ux_api_bt_addr_t *>(addr), &acl_state);
    ux_api_bt_get_profile_state(UX_API_BT_PROFILE_A2DP, reinterpret_cast<const ux_api_bt_addr_t *>(addr), &a2dp_state);
    if (acl_state != UX_API_BT_PROFILE_STATE_CONNECTED) {
        retVal = ux_api_bt_connect(UX_API_BT_PROFILE_BR, reinterpret_cast<const ux_api_bt_addr_t *>(addr));
    } else if (acl_state == UX_API_BT_PROFILE_STATE_CONNECTED && a2dp_state != UX_API_BT_PROFILE_STATE_CONNECTED) {
        retVal = ux_api_bt_connect(UX_API_BT_PROFILE_A2DP, reinterpret_cast<const ux_api_bt_addr_t *>(addr));
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "ConnectRemoteDevice ux_api_bt_pair_device ret = 0x%x", retVal);
    (void)memcpy_s(&remoteConnectAddr_, sizeof(bd_addr_t), addr, sizeof(bd_addr_t));
    return retVal;
}

bool SettingBluetoothModel::ConnectRemoteCheck(const bd_addr_t *addr)
{
    bool ret;
    if (IsMacAddrSame(remoteConnectAddr_.addr, addr->addr)) {
        ret = true;
        (void)memset_s(&remoteConnectAddr_, sizeof(bd_addr_t), 0, sizeof(bd_addr_t));
    } else {
        ret = false;
    }
    return ret;
}

#ifdef __cplusplus
extern "C" {
#endif
static void LoadScanResult()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "LoadScanResult");
    SettingBluetoothPage* bluetoothPage = SettingBluetoothPage::GetInstance();
    if (bluetoothPage == nullptr) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "bluetoothPage nullptr");
        return;
    }
    bluetoothPage->SetScanStatus(false);

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "LoadScanResult::LoadFindDeviceList");
    bluetoothPage->LoadFindDeviceList();
}

static void ACLConnectionSuss(const bd_addr_t &bdAddr)
{
    // 成功提示
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "ACLConnectionSuss conn success");
    SettingBluetoothModel::GetInstance()->SetLastConnedAddr(&bdAddr);
    SettingBluetoothModel::GetInstance()->UpdateConnectedStatusForDevice(bdAddr);
    if (SettingBluetoothPage::GetInstance() != nullptr) {
        SettingBluetoothPage::GetInstance()->LoadFindDeviceList();
    }
    if ((SettingBluetoothModel::GetInstance()->GetConnectResultPage() == true) &&
        (SettingBluetoothPage::GetInstance() != nullptr)) {
        SettingBluetoothPage::GetInstance()->ShowResultGroup(CONNECT_BLUETOOTH_SUCCESS_FRAGMENT);
    } else {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "GetConnectResultPage is false or GetInstance is nullptr");
    }
    SettingBluetoothModel::GetInstance()->SetConnectResultPage(false);
}

static void ACLConnectionFail(const bd_addr_t &bdAddr)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "ACLConnectionFail conn fail");
    SettingBluetoothModel::GetInstance()->UpdateDisconnectStatusForDevice(bdAddr);
    if (SettingBluetoothPage::GetInstance() != nullptr) {
        SettingBluetoothPage::GetInstance()->LoadFindDeviceList();
    }
    if ((SettingBluetoothModel::GetInstance()->GetConnectResultPage()) == true &&
        (SettingBluetoothPage::GetInstance() != nullptr)) {
        SettingBluetoothPage::GetInstance()->ShowResultGroup(CONNECT_BLUETOOTH_FAIL_FRAGMENT);
    }
    // 失败提示页
    SettingBluetoothModel::GetInstance()->SetConnectResultPage(false);
}

void SettingBluetoothModel::SettingScanCompleteMsgProcess(BtCaseInfo scan_msg, bool need_to_del)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "recv SCAN COMPLETE, dev = %s", scan_msg.deviceName);
    if ((scan_msg.device_class & 0x200) != 0) { // 0x200: Major Device Class = phone
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingScanCompleteMsgProcess: Mobile Phone Device");
        return;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingScanCompleteMsgProcess addr: ****%02x%02x%02x%02x",
        scan_msg.addr[SETTING_ADDR3], scan_msg.addr[SETTING_ADDR2], scan_msg.addr[1], scan_msg.addr[0]);

    unsigned char nameLen = strlen((const char *)(scan_msg.deviceName));
    if (nameLen == 0) { // 过滤未扫描到名字的设备
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "nameLen == 0 return");
        return;
    }
    // 去重操作并更新rssi
    bool tag = SettingBluetoothModel::GetInstance()->CheckExistScanListAndUpdateRssi(scan_msg);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "CheckExistScanList ret = %d", tag);
    if (!tag) {
        SettingBluetoothModel::GetInstance()->ScansDevicesListAddCase(scan_msg); // 增加扫描设备信息
    } else if (need_to_del) {
        SettingBluetoothModel::GetInstance()->ScansDevicesListDelCase(scan_msg); // 剔除扫描设备信息
    }
    SettingBluetoothModel::GetInstance()->SortScansCaseRssi();
    LoadScanResult();
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingScanCompleteMsgProcess end");
}

static inline void EarphoneAclDisconnect(const bd_addr_t &addr)
{
    if (SettingBluetoothModel::GetInstance()->GetHeadPhoneConnectStatus() == false) {
        // 若已经断连, 则不作处理
        return;
    }
    GraphicService::GetInstance()->PostGraphicEvent(std::bind(ACLConnectionFail, addr));
    SettingBluetoothModel::GetInstance()->SetHeadPhoneConnectStatus(false);
    SendMsgToUIService(BT_DISCONNECTED_TO_UI, 0, nullptr, 0);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[A2dpSrcStateChangedProcess] A2DP disconnected!");
}

void SettingBluetoothModel::A2dpSrcStateChangedProcess(const bd_addr_t &bdAddr, uint8_t state)
{
    switch ((ux_api_bt_profile_state_t)state) {
        case UX_API_BT_PROFILE_STATE_CONNECTING:
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[A2dpSrcStateChangedProcess] A2DP connected!");
            break;
        case UX_API_BT_PROFILE_STATE_CONNECTED:
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ConnCtStateChangedCallback addr: ****%02x%02x%02x%02x",
                bdAddr.addr[SETTING_ADDR3], bdAddr.addr[SETTING_ADDR2], bdAddr.addr[1], bdAddr.addr[0]);
            SetAvrcpSrcInfAddr(bdAddr);
            GraphicService::GetInstance()->PostGraphicEvent(std::bind(ACLConnectionSuss, bdAddr));
            SettingBluetoothModel::GetInstance()->SetHeadPhoneConnectStatus(true);
            SendMsgToUIService(BT_CONNECTED_TO_UI, 0, nullptr, 0);
            break;
        case UX_API_BT_PROFILE_STATE_DISCONNECTING:

            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[A2dpSrcStateChangedProcess] A2DP disconnecting!");
            break;
        case UX_API_BT_PROFILE_STATE_DISCONNECTED:
            EarphoneAclDisconnect(bdAddr);
            break;
        default:
            break;
    }
}

void SettingBluetoothModel::SettingConnectMsgProcess(const bd_addr_t &bdAddr, uint8_t reason, uint8_t state,
                                                     uint32_t device_class)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingConnectMsgProcess reason: %x, state: %x, device_class: %d", reason,
                  state, device_class);
    if (state == UX_BT_BLE_CONNECTED) {
        (void)memcpy_s(&g_lasted_remote_ble_addr.addr, UX_API_BT_ADDR_LEN, &bdAddr.addr, BD_ADDR_LEN);
        g_lasted_remote_ble_addr.type = bdAddr.type;
        MsgCenterSendMsg(BT_CONNECTED_TO_MSGCENTER, 0, nullptr, 0);
        msg_center_set_ble_conn_state(GAP_BLE_STATE_CONNECTED);
        // send to ace network
        net_connect_status_change(NETWORK_TYPE_BLUETOOTH_LE, CONNECTION_STATUS_CONNECTED);
    }
    if ((device_class & 0x200) != 0) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingConnectMsgProcess: Mobile Phone Device");
        if (state == UX_BT_BR_CONNECTED) {
            MsgCenterSendMsg(BT_CONNECTED_TO_MSGCENTER, 0, nullptr, 0);
            // send to ace network
            SettingBluetoothModel::GetInstance()->SetPhoneConnected(true);
            SendMsgToUIService(BT_CONNECTED_TO_UI, 0, nullptr, 0);
#if defined(SUPPORT_LWIP) && defined(SUPPORT_BREDR)
            lwip_open_bt_pan();
#endif
            net_connect_status_change(NETWORK_TYPE_BLUETOOTH, CONNECTION_STATUS_CONNECTED);
        }
        return;
    }
    if ((state == UX_BT_BR_CONNECTED) || (state == UX_BT_BLE_CONNECTED)) {
        // 将数据存入设备列表
        BtCaseInfo *info = GetSelectedDeviceInfo();
        bool tag = CheckExistPairedList(*info);
        if ((!tag) && (memcmp(info->addr, bdAddr.addr, BD_ADDR_LEN) == 0)) {
            AddPairedDevicesCase(*info);
        }
        if (SettingBluetoothModel::GetInstance()->ConnectRemoteCheck(&bdAddr)) {
            SetLastConnedAddr(&bdAddr);
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingConnectMsgProcess: earphone connected");
        }
    } else {
        ACLConnectionFail(bdAddr);
    }
}

void SettingBluetoothModel::SettingDisconnectMsgProcess(const bd_addr_t &bdAddr, uint8_t reason, uint8_t state,
                                                        uint32_t device_class)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingDisconnectMsgProcess state: %d, reason: %x, device_class: %d", state,
                  reason, device_class);
    if (state == UX_BT_BLE_DISCONNECTED) { // restart ble adv when disconnected
        /* 通知msg_center，ble状态仍用bt状态，此处不区分 */
        MsgCenterSendMsg(BT_DISCONNECTED_TO_MSGCENTER, 0, nullptr, 0);
        msg_center_set_ble_conn_state(GAP_BLE_STATE_DISCONNECTED);
        // send to ace network
        net_connect_status_change(NETWORK_TYPE_BLUETOOTH_LE, CONNECTION_STATUS_DISCONNECTED);
        // 断开蓝牙连接时，若存在查找手机页面，刷新页面为断开页面
        if (FindPhoneMainPage::GetInstance() != nullptr) {
            FindPhoneMainPage::GetInstance()->ChangeToDisConnect();
        }
    }
    if ((device_class & PHONE_DEVICE_CLASS) != 0) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingDisconnectMsgProcess: Mobile Phone Device");
        if (state == UX_BT_BR_DISCONNECTED) {
            /* 通知msg_center，ble状态仍用bt状态，此处不区分 */
            MsgCenterSendMsg(BT_DISCONNECTED_TO_MSGCENTER, 0, nullptr, 0);
            // send to ace network
            SettingBluetoothModel::GetInstance()->SetPhoneConnected(false);
            SendMsgToUIService(BT_DISCONNECTED_TO_UI, 0, nullptr, 0);
            net_connect_status_change(NETWORK_TYPE_BLUETOOTH, CONNECTION_STATUS_DISCONNECTED);
            // 断开蓝牙连接时，若存在查找手机页面，刷新页面为断开页面
            if (FindPhoneMainPage::GetInstance() != nullptr) {
                FindPhoneMainPage::GetInstance()->ChangeToDisConnect();
            }
#if defined(SUPPORT_LWIP) && defined(SUPPORT_BREDR)
            lwip_close_bt_pan();
#endif
        }
        return;
    } else if (((device_class & PHONE_DEVICE_CLASS) != PHONE_DEVICE_CLASS) && (state == UX_BT_BR_DISCONNECTED)) {
        EarphoneAclDisconnect(bdAddr);
    }

    if ((reason == BT_DISCONNECT_BY_REMOTE_USER) ||
        (reason == BT_DISCONNECT_BY_REMOTE_DUE_TO_LOW_RESOURCE) ||
        (reason == BT_DISCONNECT_BY_REMOTE_DUE_TO_POWER_OFF) ||
        (reason == BT_DISCONNECT_BY_MIC_ERROR) ||
        (reason == BT_DISCONNECT_BY_CONNECT_TIMEOUT)) { // 连接断开
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "The peer end is disconnected.");
    } else if (reason == BT_DISCONNECT_BY_LOCAL_HOST) { // 主动断开连接
        BtCaseInfo *onClickBtCaseInfo = SettingBluetoothModel::GetInstance()->GetSelectedDeviceInfo();
    } else if ((reason == BT_CONNECTION_ALREADY_EXISTS) || (reason > 0x45)) { // 0x45:acl状态最大参数
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "reason is false.");
    }

    ACLConnectionFail(bdAddr);
}

void SettingBluetoothModel::SettingEnableMsgProcess(bs_protocol_type_t protocal)
{
    if (protocal == BS_PROTOCOL_BLE) {
        SettingBtInitGatts();
    } else if (protocal == BS_PROTOCOL_BREDR) {
        SetUpPairedDevicesList();
    } else {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "enable protocol is not support %d", protocal);
    }
}

void ChangeBleConnectToMax()
{
    if (msg_center_get_trans_channel() == DIAG_FRAME_FID_BLE_GATT) {
        ux_api_bt_conn_param_t conn_param = { 0 };
        (void)memcpy_s(&conn_param.addr, sizeof(conn_param.addr), &g_lasted_remote_ble_addr, sizeof(ux_api_bt_addr_t));
        conn_param.interval_min = SETTING_BLUETOOTH_INTERVAL_TOMAX_MIN;
        conn_param.interval_max = SETTING_BLUETOOTH_INTERVAL_TOMAX_MAX;
        conn_param.slave_latency = SETTING_BLUETOOTH_LATENCY;
        conn_param.timeout_multiplier = SETTING_BLUETOOTH_TIMEOUT_MUTIPLIER;
        errcode_t ret = ux_api_bt_config(UX_API_BT_CONFIG_CONNECT_PARAM, &conn_param, sizeof(conn_param));
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "change to max interval ret: %d, latency-min: %u, latency-max: %u\n",
                      ret, conn_param.interval_min, conn_param.interval_max);
    }
}

void ChangeBleConnectToDefault()
{
    if (msg_center_get_trans_channel() == DIAG_FRAME_FID_BLE_GATT) {
        ux_api_bt_conn_param_t conn_param = { 0 };
        (void)memcpy_s(&conn_param.addr, sizeof(conn_param.addr), &g_lasted_remote_ble_addr, sizeof(ux_api_bt_addr_t));
        conn_param.interval_min = SETTING_BLUETOOTH_INTERVAL_TODEFAULT_MIN;
        conn_param.interval_max = SETTING_BLUETOOTH_INTERVAL_TODEFAULT_MAX;
        conn_param.slave_latency = SETTING_BLUETOOTH_LATENCY;
        conn_param.timeout_multiplier = SETTING_BLUETOOTH_TIMEOUT_MUTIPLIER;
        errcode_t ret = ux_api_bt_config(UX_API_BT_CONFIG_CONNECT_PARAM, &conn_param, sizeof(conn_param));
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "change to default interval ret: %d, latency-min: %u, latency-max: %u\n",
                      ret, conn_param.interval_min, conn_param.interval_max);
    }
}

static void SettingGattsServiceStartCallback(uint8_t serverId, uint8_t instanceID, errcode_t status)
{
    if (g_sample_svr_handle == instanceID) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "SettingGattsServiceStartCallback 0x%x 0x%x 0x%x", status, serverId,
                      instanceID);
    }
}

static void SettingGattsServiceStopCallback(uint8_t serverId, uint8_t instanceID, errcode_t status)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "SettingGattsServiceStopCallback 0x%x 0x%x 0x%x",
                  status, serverId, instanceID);
}

static void SettingGattsServiceDeleteCallback(uint8_t serverId, uint8_t instanceID, errcode_t status)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "SettingGattsServiceDeleteCallback 0x%x 0x%x 0x%x",
        status, serverId, instanceID);
}

static void SettingGattsRequestReadCallback(uint16_t connId, uint8_t serverId, bs_gatts_req_read_cb_t *readCbPara,
                                            errcode_t status)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingGattsRequestReadCallback handle = %u", readCbPara->attribute_id);
}

static void SettingGattsRequestWriteCallback(uint16_t connId, uint8_t serverId, bs_gatts_req_write_cb_t *writeCbPara,
                                             errcode_t status)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingGattsRequestWriteCallback id = %u", writeCbPara->attribute_id);
    if (writeCbPara->attribute_id == GATT_DEFAULT_SERVICE_CTKD_CHARACTER) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingGattsRequestWriteCallback id = %u", writeCbPara->attribute_id);
        if ((writeCbPara->value_len == GATT_CTKD_CMD_LEN) && (writeCbPara->value[0] == GATT_CTKD_ENABLE)) {
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "enable ctkd");
            // 一键双连使能（依赖安全连接参数配置）
            bool open_ctkd = true;
            errcode_t ret = ux_api_bt_config(UX_API_BT_CONFIG_PAIR_USE_CTKD, &open_ctkd, sizeof(open_ctkd));
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "ux set ctkd enable ret = %u", ret);
            // 发起ble配对
            ux_api_bt_device_info_t dev_info = {0};
            ret = ux_api_bt_get_device_info_by_id(connId, &dev_info);
            if (ret) {
                WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "ux_api_bt_get_device_info_by_id ret = 0x%x", ret);
                return;
            }
            ret = ux_api_bt_pair_device(&(dev_info.addr), UX_API_BT_PROTOCOL_BLE);
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "ctkd_pair ret=0x%x add:****%02x%02x%02x%02x, type:%u", ret,
                dev_info.addr.addr[SETTING_ADDR3], dev_info.addr.addr[SETTING_ADDR2],
                dev_info.addr.addr[SETTING_ADDR1], dev_info.addr.addr[SETTING_ADDR0],
                dev_info.addr.type);
        } else if ((writeCbPara->value_len == GATT_CTKD_CMD_LEN) && (writeCbPara->value[0] == GATT_CTKD_DISABLE)) {
            // 一键双连去使能
            bool close_ctkd = false;
            errcode_t ret = ux_api_bt_config(UX_API_BT_CONFIG_PAIR_USE_CTKD, &close_ctkd, sizeof(close_ctkd));
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "ux set ctkd disable ret = %u", ret);
        }
    }
}

static void SettingGattsMtuChangedCallback(uint16_t connId, uint8_t serverId, uint16_t mtuSize, errcode_t status)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "SettingGattsMtuChangedCallback connId = %u mtuSize = %u",
                  connId, mtuSize);
}

#if !defined(_WIN32)
GattDefaultServiceTemplate g_default_service = {
    .uuid = { GATT_UUID_DEFAULT_LEN, GATT_DEFAULT_SERVICE_UUID },
    .is_primary = 1,
    .attribute_count = GATT_DEFAULT_SERVICE_CHARA_DESC_MAX,
    .cbks = {
        .start_service_cb = SettingGattsServiceStartCallback,
        .stop_service_cb = SettingGattsServiceStopCallback,
        .delete_service_cb = SettingGattsServiceDeleteCallback,
        .read_request_cb = SettingGattsRequestReadCallback,
        .write_request_cb = SettingGattsRequestWriteCallback,
        .indicate_cfm_cb = NULL,
        .mtu_changed_cb = SettingGattsMtuChangedCallback,
    },
    .attribute_array = {
        {
            .type = GATT_CHARAC_TYPE,
            .character = {
                .uuid = { GATT_UUID_DEFAULT_LEN, GATT_CTKD_CHARACTER_UUID },
                .permissions = BS_GATT_ATTRIBUTE_PERMISSION_WRITE,
                .properties = BS_GATT_CHARACTER_PROPERTY_BIT_WRITE_NO_RSP,
                .value_len = (uint16_t)sizeof(g_ctkd_gatt_value),
                .value = (uint8_t *)(&g_ctkd_gatt_value),
            },
        },
    },
};
#else
bs_gatts_add_chara_info_t character0 = {
    .uuid = { GATT_UUID_DEFAULT_LEN, GATT_CTKD_CHARACTER_UUID },
    .permissions = BS_GATT_ATTRIBUTE_PERMISSION_WRITE,
    .properties = BS_GATT_CHARACTER_PROPERTY_BIT_WRITE_NO_RSP,
    .value_len = (uint16_t)sizeof(g_ctkd_gatt_value),
    .value = (uint8_t *)(&g_ctkd_gatt_value),
};
GattDefaultServiceTemplate g_default_service = {
    .uuid = { GATT_UUID_DEFAULT_LEN, GATT_DEFAULT_SERVICE_UUID },
    .is_primary = 1,
    .attribute_count = GATT_DEFAULT_SERVICE_CHARA_DESC_MAX,
    .cbks = {
        .start_service_cb = SettingGattsServiceStartCallback,
        .stop_service_cb = SettingGattsServiceStopCallback,
        .delete_service_cb = SettingGattsServiceDeleteCallback,
        .read_request_cb = SettingGattsRequestReadCallback,
        .write_request_cb = SettingGattsRequestWriteCallback,
        .indicate_cfm_cb = NULL,
        .mtu_changed_cb = SettingGattsMtuChangedCallback,
    },
    .extend_param = {},
    .attribute_array = {
        {
            .type = GATT_DESC_TYPE,
        },
    }
};
#endif

void SettingBtInitCallBacksEx()
{
    errcode_t ret = ERRCODE_SUCC;
    ret =  bs_gatts_service_instance_init(g_default_ble_server_id,
        (bs_gatt_service_template_t*)(&g_default_service), (uint8_t*)(&g_ble_default_service_id));
    if (ret != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingBtInitCallBacksEx init! ret: %x\n", ret);
        return;
    }
    return;
}

void SettingBtInitGatts(void)
{
    bt_uuid_t server_uuid = { GATT_UUID_DEFAULT_LEN, GATT_DEFAULT_SERVER_UUID };
    errcode_t ret = ERRCODE_SUCC;
    ret = bs_gatts_register(&server_uuid, &g_default_ble_server_id);
    if (ret != ERRCODE_BT_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingBtInitGatts regitster! ret: %x\n", ret);
        return ;
    }
    SettingBtInitCallBacksEx();
    return;
}

void SettingBleAdvStart(void)
{
    errcode_t ret;
    if (SettingBluetoothModel::GetInstance()->GetAdvStartState()) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "SettingBleAdvStart already started");
        ret = ux_api_bt_stop_discovery(UX_API_BT_AS_PERIPHERAL, UX_API_BT_PROTOCOL_BLE);
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "SettingBleAdvStart x_api_bt_stop_discovery  ret = 0x%x", ret);
        osDelay(50);
    }

    ret = ux_api_bt_start_discovery(UX_API_BT_AS_PERIPHERAL, UX_API_BT_PROTOCOL_BLE);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "SettingBleAdvStart x_api_bt_stop_discovery  ret = 0x%x", ret);
}
#ifdef __cplusplus
}
#endif
}
