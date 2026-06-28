/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: BluetoothModel view
 * Author:
 * Create: 2021-10-13
 */
#include <cstdint>
#include "bluetooth/BluetoothModel.h"
#if !defined(_WIN32)
#include "bluetooth/BluetoothWrapper.h"
#endif
#include "bluetooth/BluetoothPresenter.h"
#include "connectnewphone/ConnectPhonePresenter.h"
#include "player/BluetoothView.h"
#include "bluetooth/BluetoothItemView.h"
#include "player/PlayersModel.h"
#include "phoneservice/PhoneAudio.h"
#include "graphic_service.h"
#include "msg_center.h"
#include "bts_def.h"
#include "bts_gatt_stru.h"
#include "bts_a2dp_source.h"
#include "bts_gatt_server.h"
#include "bts_le_gap.h"
#include "bts_spp.h"
#if !defined(_WIN32)
#include "osal_task.h"
#include "dfx_channel.h"
#include "tcxo.h"
#else
#include "securec.h"
#include "ui_font_header.h"
#endif
#ifdef SUPPORT_ALIPAY_SEC
#include "alipay_feature.h"
#endif
#include "bts_a2dp_sink.h"
#include "bts_feature.h"
#include "bt_audio_hal_interface.h"
#include "audio_manager_c_wrapper.h"
#include "bts_avrcp_target.h"
#include "msg_center_msg.h"

namespace OHOS {

#define BT_SPP_TASK_STACK_SIZE 2048
#define BT_SPP_TASK_PRI        20
#define TEN                    10

constexpr uint16_t BLUETOOTH_MAX_NUM = 20;

#define MEDIA_A2DP_SNK_INTERFACE 1
#if !defined(_WIN32)
osal_timer g_a2dp_stream_accept_timer = {nullptr, nullptr, 0, 0};
#endif
#define A2DP_STREAM_ACCEPT_TIMER_PERIOD 500
AudioFormatConfig g_audio_format_config = {AUDIO_DEFAULT, 0, 0};
static uint16_t g_sample_svr_handle = 0;
static uint16_t g_sample_characteristic_handle = 0;
static uint16_t g_diag_characteristic_handle = 0;
constexpr uint16_t g_sample_adv_id = 2;
static bd_addr_t g_bd_addr = { 0 };

void BluetoothModel::SetBluetoothCalleridStatus(int16_t value)
{
    callerid = value;
}

int BluetoothModel::GetBluetoothCalleridStatus(void)
{
    return callerid;
}

BluetoothModel::BluetoothModel()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BluetoothModel::BluetoothModel");
}

BluetoothModel *BluetoothModel::GetInstance(void)
{
    static BluetoothModel instance;
    return &instance;
}

char* BluetoothModel::getWatchName(void)
{
    return watchName;
}

void BluetoothModel::InitBluetoothInfo(void)
{
#if !defined(_WIN32)
    errcode_t ret;
    unsigned char nameLen = BD_NAME_LEN;
    memset_s(watchName, sizeof(watchName), 0, sizeof(watchName));
    ret = bluetooth_get_local_name((unsigned char *)watchName, &nameLen);
    // 为了实验室测试需要，客户需要自己修改
    if ((nameLen == strlen("WatchDemo") + 1) && (strcmp("WatchDemo", watchName) == 0)) {
        uint32_t currentUs = uapi_tcxo_get_us();
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "currentUs 0x%x", currentUs);
        // 设置蓝牙名称
        (void)sprintf_s(watchName, sizeof(watchName), "Watch%x", (currentUs & 0xFFFF));
        ret = bluetooth_set_local_name((unsigned char*)watchName, strlen((char*)watchName) + 1);
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "bluetooth_set_local_name %s ret = %u", watchName, ret);

        //  设置蓝牙地址
        bd_addr_t addr = {{0x0, 0x0, 0x33, 0x44, 0x55, 0x66} ,0};
        addr.addr[0] = currentUs & 0xFF;
        addr.addr[1] = (currentUs & 0XFF00) >> 8;
        ret = bluetooth_set_local_addr(addr.addr, BD_ADDR_LEN);
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "bluetooth_set_local_addr [0x%x:0x%x] ret = %u", addr.addr[0], addr.addr[1], ret);
    } else {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "watchName: %s", watchName);
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "nameLen: %u", nameLen);
    }
#endif
}

void BluetoothModel::Init()
{
#if !defined(_WIN32)
    errcode_t ret;

    // 设置自动回连，ios一键双连依赖此配置
    bts_feature_t bts_feature = { 0 };
    bts_feature.bt_auto_reconnect_closed = 0;
    ret = bts_set_features(&bts_feature);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "bt bts_set_features ret = %u", ret);

    ret = enable_bt_stack();
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "bt enable_bt_stack ret = %u", ret);

    // 设置安全连接
    gap_ble_sec_params_t sec_param = { 1, GAP_BLE_IO_CAPABILITY_NOINPUTNOOUTPUT, 1, GAP_BLE_GAP_SECURITY_MODE1_LEVEL3 };
    ret = gap_ble_set_sec_param(&sec_param);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "bt gap_ble_set_sec_param ret = %u", ret);

    InitBluetoothInfo();
    BtInitCallBacks();
    wrapper_ble_register();
#endif
#ifdef SUPPORT_ALIPAY_SEC
    svr_alipay_bluetooth_init();
#endif
    BtInitCallBacksEx();
    BtInitGatts();
    BtInitSpp();

    SetUpListCase();
    SetUpConnedDevicesList();
}

void BluetoothModel::SetCaseListClear()
{
    btScansDevicesList_.clear();
}

bool BluetoothModel::CheckExistScanList(BluetoothCaseInfo caseInfo)
{
    std::list<BluetoothCaseInfo>::iterator node = btScansDevicesList_.begin();
    if (btScansDevicesList_.empty()) {
        return false;
    }
    while (node != btScansDevicesList_.end()) {
        if ((caseInfo.addr[0] == node->addr[0]) && (caseInfo.addr[1] == node->addr[1])
            && (caseInfo.addr[2] == node->addr[2]) && (caseInfo.addr[3] == node->addr[3])
            && (caseInfo.addr[4] == node->addr[4]) && (caseInfo.addr[5] == node->addr[5])) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "true");
            return true;
        }
        node++;
    }
    return false;
}

void BluetoothModel::SetUpListCase()
{
    SetCaseListClear();
}

std::list<BluetoothCaseInfo>& BluetoothModel::GetCase()
{
    return btScansDevicesList_;
}

void BluetoothModel::AddCase(BluetoothCaseInfo caseInfo)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AddCase::caseInfo addr: %02x%02x%02x%02x%02x%02x",
        caseInfo.addr[5], caseInfo.addr[4], caseInfo.addr[3], /* mac下标 0 1 2 3 4 5 */
        caseInfo.addr[2], caseInfo.addr[1], caseInfo.addr[0]);
    if (btScansDevicesList_.size() >= BLUETOOTH_MAX_NUM) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "btScansDevicesList_.size is BLUETOOTH_MAX_NUM");
        return;
    } else {
        btScansDevicesList_.push_back(caseInfo);
    }
}

void BluetoothModel::SetScansCaseRssi(const bd_addr_t *bdAddr, int rssi)
{
    for (auto it = btScansDevicesList_.begin(); it != btScansDevicesList_.end(); it++) {
        if ((it->addr[0] == bdAddr->addr[0]) && (it->addr[1] == bdAddr->addr[1])
            && (it->addr[2] == bdAddr->addr[2]) && (it->addr[3] == bdAddr->addr[3])
            && (it->addr[4] == bdAddr->addr[4]) && (it->addr[5] == bdAddr->addr[5])) {
                it->temp = rssi;
                WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "btScansDevicesList_ rssi:%d deviceName:%s", it->temp, it->deviceName);
        }
    }
}


bool CompBuletoothInfo(const BluetoothCaseInfo& a, const BluetoothCaseInfo& b)
{
    if (a.temp > b.temp) {
        return true;
    }
    return false;
}

void BluetoothModel::SortScansCaseRssi()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BluetoothModel::SortScansCaseRssi");
    btScansDevicesList_.sort(CompBuletoothInfo);
}

void BluetoothModel::ResetScanDevicesCase(const BluetoothCaseInfo& caseInfo)
{
    for (auto it = btScansDevicesList_.begin(); it != btScansDevicesList_.end(); it++) {
        if ((it->addr[0] == caseInfo.addr[0]) && (it->addr[1] == caseInfo.addr[1])
            && (it->addr[2] == caseInfo.addr[2]) && (it->addr[3] == caseInfo.addr[3])
            && (it->addr[4] == caseInfo.addr[4]) && (it->addr[5] == caseInfo.addr[5])) {
                (void)memcpy_s(it->deviceName, BD_NAME_LEN,
                               caseInfo.deviceName, BD_NAME_LEN);
                WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ResetConnedDevicesCase deviceName:%s", it->deviceName);
        }
    }
}

/* 已配对设备队列 */
void BluetoothModel::SetConnedDevicesListClear()
{
    btConnedDevicesList_->Clear();
}

void BluetoothModel::SetUpConnedDevicesList()
{
    if (btConnedDevicesList_ == nullptr) {
        btConnedDevicesList_ = new List<BluetoothCaseInfo>();
        if (btConnedDevicesList_ == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "BluetoothModel btConnedDevicesList_ new fail");
            return;
        }
        SetConnedDevicesListClear();
#if !defined(_WIN32)
        unsigned int connedDevicesNum = 0;
        bool ret = gap_get_paired_devices_num(&connedDevicesNum);
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "Setup GetPariedDevicesNum ret = %d, num = %d", ret, connedDevicesNum);

        gap_paired_device_info_t *devInfo = (gap_paired_device_info_t*)malloc(sizeof(gap_paired_device_info_t) *connedDevicesNum);
        if (devInfo ==  nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "DevInfo malloc failed %u", sizeof(gap_paired_device_info_t) *connedDevicesNum);
            return;
        }
        ret =  gap_get_paired_devices_list(devInfo, (int *)&connedDevicesNum);
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SetUpConnedDevicesList gap_get_paired_devices_list ret = %d", ret);

        BluetoothCaseInfo caseInfo{};
        for (int i = 0; i < static_cast<int>(connedDevicesNum); ++i) {
            if ((gap_get_device_class((const bd_addr_t *)devInfo[i].addr.addr) & 0x200) != 0) {
                continue;
            }
            (void)memcpy_s(caseInfo.addr, BD_ADDR_LEN,
                           devInfo[i].addr.addr, BD_ADDR_LEN);
            (void)memcpy_s(caseInfo.deviceName, BD_NAME_LEN,
                           devInfo[i].device_name, BD_NAME_LEN);
            AddConnedDevicesCase(caseInfo);
        }
        free(devInfo);
#endif
    }
}

void BluetoothModel::AddConnedDevicesCase(BluetoothCaseInfo caseInfo)
{
    if (btConnedDevicesList_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "btConnedDevicesList_ nullptr");
        return;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AddConnedDevicesCase::caseInfo addr: %02x%02x%02x%02x%02x%02x",
        caseInfo.addr[5], caseInfo.addr[4], caseInfo.addr[3], /* mac下标 0 1 2 3 4 5 */
        caseInfo.addr[2], caseInfo.addr[1], caseInfo.addr[0]);
    if (btConnedDevicesList_->Size() >= BLUETOOTH_MAX_NUM) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "btConnedDevicesList_->Size is BLUETOOTH_MAX_NUM");
        return;
    } else {
        btConnedDevicesList_->PushBack(caseInfo);
    }
}

List<BluetoothCaseInfo>& BluetoothModel::GetConnedDevicesCase()
{
    return *btConnedDevicesList_;
}

void BluetoothModel::SetBtStatus(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SetBtStatus tempMacValue: %02x%02x%02x%02x%02x%02x",
        tempMacValue.addr[5], tempMacValue.addr[4], tempMacValue.addr[3], /* macÏÂ±ê 0 1 2 3 4 5 */
        tempMacValue.addr[2], tempMacValue.addr[1], tempMacValue.addr[0]); /* macÏÂ±ê 0 1 2 3 4 5 */
#if !defined(_WIN32)
    aclConnected = bt_is_acl_connected((const bd_addr_t*)&tempMacValue.addr);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "aclConnected %d", aclConnected);
#endif
}

bool BluetoothModel::GetAclConnected(void)
{
    return aclConnected;
}

void BluetoothModel::SetTempMacValue(BluetoothCaseInfo *caseInfo)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SetTempMacValue: %02x%02x%02x%02x%02x%02x",
                  caseInfo->addr[5], caseInfo->addr[4], caseInfo->addr[3], /* macÏÂ±ê 0 1 2 3 4 5 */
                  caseInfo->addr[2], caseInfo->addr[1], caseInfo->addr[0]); /* macÏÂ±ê 0 1 2 3 4 5 */
    (void)memcpy_s(tempMacValue.addr, BD_ADDR_LEN,
                   caseInfo->addr, BD_ADDR_LEN);
    (void)memcpy_s(tempMacValue.deviceName, BD_NAME_LEN,
                   caseInfo->deviceName, BD_NAME_LEN);
}

BluetoothCaseInfo *BluetoothModel::GetTempMacValue(void)
{
    return &tempMacValue;
}

void BluetoothModel::SetLastConnedAddr(const bd_addr_t *addr)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SetLastConnedAddr: %02x%02x%02x%02x%02x%02x",
                  addr->addr[5], addr->addr[4], addr->addr[3], /* macÏÂ±ê 0 1 2 3 4 5 */
                  addr->addr[2], addr->addr[1], addr->addr[0]); /* macÏÂ±ê 0 1 2 3 4 5 */
    (void)memcpy_s(lastConnedAddr.addr, BD_ADDR_LEN, addr->addr, BD_ADDR_LEN);
}

bd_addr_t *BluetoothModel::GetLastConnedAddr(void)
{
    return &lastConnedAddr;
}

void BluetoothModel::SetConnectionView(BluetoothItemView *item)
{
    selectItemView = item;
}

BluetoothItemView *BluetoothModel::GetConnectionView()
{
    return selectItemView;
}


void BluetoothModel::SetTagReconnectJump(bool tag)
{
    tagReconnectJump = tag;
}

bool BluetoothModel::GetTagReconnectJump(void)
{
    return tagReconnectJump;
}

void BluetoothModel::SetCurView(int16_t viewNum)
{
    curView = viewNum;
}

int BluetoothModel::GetCurView(void)
{
    return curView;
}

void BluetoothModel::SetTagConnectView(bool tag)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BluetoothModel::SetTagConnectView:%d", tag);
    tagConnectView = tag;
}

// 判断是否需要跳转连接成功\失败界面（已退出蓝牙模块）
bool BluetoothModel::IsTagConnectView(void)
{
    return tagConnectView;
}

void BluetoothModel::SetTagDisconnectView(bool tag)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BluetoothModel::SetTagDisconnectView:%d", tag);
    tagDisconnectView = tag;
}

// 判断是否需要跳转连接失败界面
bool BluetoothModel::IsTagDisconnectView(void)
{
    return tagDisconnectView;
}

void BluetoothModel::SetTagConnectNew(bool tag)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BluetoothModel::SetTagConnectNew:%d", tag);
    tagConnectNew = tag;
}

void BluetoothModel::SetTagReturnToDeviceListView(bool tag)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BluetoothModel::SetTagReturnToDeviceListView:%d", tag);
    tagReturnToDeviceListView = tag;
}

bool BluetoothModel::IsTagReturnToDeviceListView(void)
{
    return tagReturnToDeviceListView;
}

// 判断断开连接时是否需要马上连接新设备
bool BluetoothModel::IsTagConnectNew(void)
{
    return tagConnectNew;
}

void BluetoothModel::SetTagScanDeviceClicked(bool tag)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BluetoothModel::SetTagScanDeviceClicked:%d", tag);
    tagScanDeviceClicked = tag;
}

// 判断是否已点击设备请求连接
bool BluetoothModel::IsTagScanDeviceClicked(void)
{
    return tagScanDeviceClicked;
}

bool BluetoothModel::CheckExistConnedList(BluetoothCaseInfo caseInfo)
{
    ListNode<BluetoothCaseInfo> *node = btConnedDevicesList_ ->Begin();
    if (node == nullptr) {
        return false;
    } else if ((caseInfo.addr[0] == 0) && (caseInfo.addr[1] == 0)
                && (caseInfo.addr[2] == 0) && (caseInfo.addr[3] == 0)
                && (caseInfo.addr[4] == 0) && (caseInfo.addr[5] == 0)) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "CheckExistConnedList false addr: 000000000000");
        return true;
    }
    while (node != btConnedDevicesList_ ->End()) {
        if ((caseInfo.addr[0] == node->data_.addr[0]) && (caseInfo.addr[1] == node->data_.addr[1])
            && (caseInfo.addr[2] == node->data_.addr[2]) && (caseInfo.addr[3] == node->data_.addr[3])
            && (caseInfo.addr[4] == node->data_.addr[4]) && (caseInfo.addr[5] == node->data_.addr[5])) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "CheckExistConnedList true");
            return true;
        }
        node = node->next_;
    }
    return false;
}

bool BluetoothModel::FindNameByAddr(BluetoothCaseInfo &listInfo, BluetoothCaseInfo *caseInfo)
{
    if ((listInfo.addr[0] == caseInfo->addr[0]) && (listInfo.addr[1] == caseInfo->addr[1]) &&
        (listInfo.addr[2] == caseInfo->addr[2]) && (listInfo.addr[3] == caseInfo->addr[3]) &&
        (listInfo.addr[4] == caseInfo->addr[4]) && (listInfo.addr[5] == caseInfo->addr[5])) {
        (void)memcpy_s(caseInfo->deviceName, BD_NAME_LEN, listInfo.deviceName, BD_NAME_LEN);
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "FindNameFromList success, deviceName:%s", listInfo.deviceName);
        return true;
    } else {
        return false;
    }
}

bool BluetoothModel::FindNameFromList(BluetoothCaseInfo *caseInfo)
{
    ListNode<BluetoothCaseInfo> *node = btConnedDevicesList_->Begin();
    if (!btScansDevicesList_.empty()) {
        for (auto it = btScansDevicesList_.begin(); it != btScansDevicesList_.end(); it++) {
            if (FindNameByAddr(*it, caseInfo)) {
                return true;
            }
        }
    }
    if (node != nullptr) {
        while (node != btConnedDevicesList_->End()) {
            if (FindNameByAddr(node->data_, caseInfo)) {
                return true;
            }
            node = node->next_;
        }
    }
    return false;
}

bool BluetoothModel::GetBluetoothAclState(void)
{
    if ((lastConnedAddr.addr[0] == 0) && (lastConnedAddr.addr[1] == 0) &&
        (lastConnedAddr.addr[2] == 0) && (lastConnedAddr.addr[3] == 0) &&
        (lastConnedAddr.addr[4] == 0) && (lastConnedAddr.addr[5] == 0)) {
        return false;
    } else {
#if !defined(_WIN32)
        return bt_is_acl_connected(&lastConnedAddr);
#else
        return false;
#endif
    }
}

bool BluetoothModel::GetAdvStartState(void)
{
    return advStartState;
}
void BluetoothModel::SetAdvStartState(bool state)
{
    advStartState = state;
}

int BluetoothModel::ConnectRemoteDevice(const bd_addr_t *addr)
{
#if !defined(_WIN32)
    errcode_t retVal = gap_connect_remote_device(addr);
    (void)memcpy_s(&remoteConnectAddr, sizeof(bd_addr_t), addr, sizeof(bd_addr_t));
    return retVal;
#else
    return ERRCODE_SUCC;
#endif
}

bool BluetoothModel::ConnectRemoteCheck(const bd_addr_t *addr)
{
    bool ret;
    if ((remoteConnectAddr.addr[0] == addr->addr[0]) && (remoteConnectAddr.addr[1] == addr->addr[1]) &&
        (remoteConnectAddr.addr[2] == addr->addr[2]) && (remoteConnectAddr.addr[3] == addr->addr[3]) &&
        (remoteConnectAddr.addr[4] == addr->addr[4]) && (remoteConnectAddr.addr[5] == addr->addr[5])) {
        ret = true;
        (void)memset_s(&remoteConnectAddr, sizeof(bd_addr_t), 0, sizeof(bd_addr_t));
    } else {
        ret = false;
    }
    return ret;
}

#ifdef __cplusplus
extern "C" {
#endif

// 回调： gap_read_remote_rssi_value 接口的数据上报
void BtGapReadRemoteRssiEventCallback(const bd_addr_t *bdAddr, int rssi, int status)
{
    UNUSED(status);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BtGapReadRemoteRssiEventCallback addr: %02x%02x%02x%02x%02x%02x",
        bdAddr->addr[5], bdAddr->addr[4], bdAddr->addr[3], /* mac下标 0 1 2 3 4 5 */
        bdAddr->addr[2], bdAddr->addr[1], bdAddr->addr[0]); /* mac下标 0 1 2 3 4 5 */
    BluetoothModel::GetInstance()->SetScansCaseRssi(bdAddr, rssi);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "bt BtGapReadRemoteRssiEventCallback SetSortCaseRssi: %d status: %d", rssi, status);
}

// 回调：发现状态变化上报/扫描结束
void BtGapDiscoveryStateChangedCallback(int status)
{
    UNUSED(status);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "bt BtGapDiscoveryStateChangedCallback %d", status);
}

static void ChangeScanView()
{
    BluetoothEarPhoneView *bluetoothEarPhoneView = BluetoothEarPhoneView::GetInstance();
    if (bluetoothEarPhoneView == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "bluetoothEarPhoneView nullptr");
        return;
    }
    bluetoothEarPhoneView->setScanStatus(false);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "Bt scan one jumpView");
    if (BluetoothModel::GetInstance()->GetBluetoothCalleridStatus() == CALLERPLAYERS) {
#if !defined(_WIN32)
        JumpView *jumpView = JumpView::GetInstance();
        if (jumpView == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "jumpView GetInstance fail");
            return;
        }
        jumpView->ChangeView(JumpView::BluetoothJumpViewNum::BLUETOOTHEARPHONEVIEW,
                             JumpView::BluetoothJumpViewNum::BLUETOOTHEARLISTVIEW);
#endif
    } else if (BluetoothModel::GetInstance()->GetBluetoothCalleridStatus() == CALLERSETTING) {
        MainBluetoothView *mainBluetoothView = MainBluetoothView::GetInstance();
        if (mainBluetoothView == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "mainBluetoothView GetInstance fail");
            return;
        }
        mainBluetoothView->ChangeView(BLUETOOTHEARPHONEVIEW, BLUETOOTHEARLISTVIEW);
    }
    // 暂停动画
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BtAnimatorStop bk");
    bluetoothEarPhoneView->BtAnimatorStop();
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BtAnimatorStop");
}

void BluetoothModel::BtGapDiscoveryResultCallbakHandle(const bd_addr_t &bdAddr)
{
    BluetoothCaseInfo caseInfo{};
    (void)memcpy_s(caseInfo.addr, BD_ADDR_LEN, bdAddr.addr, BD_ADDR_LEN);

    unsigned char nameLen = BD_NAME_LEN;
#if !defined(_WIN32)
    bool getNameRet = gap_get_device_name(&bdAddr, caseInfo.deviceName, &nameLen);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "gap_discovery_result_callback getNameRet(bool): %d nameLen: %d name:%s",
                  getNameRet, nameLen, caseInfo.deviceName);
#endif

    if (nameLen == 0) { // 过滤未扫描到名字的设备
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "nameLen == 0 return");
        return;
    }
    // 去重操作
    bool tag = BluetoothModel::GetInstance()->CheckExistScanList(caseInfo);
    if (!tag) {
        BluetoothModel::GetInstance()->AddCase(caseInfo); // 增加扫描设备信息
    }
#if !defined(_WIN32)
    bool ret = gap_read_remote_rssi_value(&bdAddr); // 获取rssi
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "gap_read_remote_rssi_value ret = %d", ret);
#endif
    BluetoothEarPhoneView *bluetoothEarPhoneView = BluetoothEarPhoneView::GetInstance();
    if (bluetoothEarPhoneView == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "bluetoothEarPhoneView nullptr");
        return;
    }
    tag = bluetoothEarPhoneView->getScanStatus();
    if (tag) { // 跳转
        ChangeScanView();
    } else {
        BluetoothEarListView *bluetoothEarListView = BluetoothEarListView::GetInstance();
        if (bluetoothEarListView == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "bluetoothEarListView nullptr");
            return;
        }
        if (bluetoothEarListView->GetScanUIList() == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "GetScanUIList nullptr");
            return;
        } // 刷新
        bluetoothEarListView->GetScanUIList()->RefreshList();
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "bt BtGapDiscoveryResultCallback end");
}

void BtGapPairStatusChangedCallback(const bd_addr_t *bdAddr, int status)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BtGapPairStatusChangedCallback addr: ****%02x%02x%02x%02x status: %d\n",
        bdAddr->addr[3], bdAddr->addr[2], bdAddr->addr[1], bdAddr->addr[0], status); /* addr下标 0 1 2 3 */
    if (status == GAP_PAIR_PAIRED) {
        if (ConnectPhonePresenter::GetInstance() != nullptr) {
            NativeAbility::GetInstance().ChangeSlice(VIEW_SETTING);
        }
    }
}

void BtGapPairRequestedCallback(const bd_addr_t *bdAddr)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BtGapPairRequestedCallback addr: ****%02x%02x%02x%02x\n",
        bdAddr->addr[3], bdAddr->addr[2], bdAddr->addr[1], bdAddr->addr[0]); /* addr下标 0 1 2 3 */
}

void BtGapPairConfirmedCallback(const bd_addr_t *bdAddr, int req_type, int number)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BtGapPairConfirmedCallback addr: ****%02x%02x%02x%02x req_type: %d number: %d\n",
        bdAddr->addr[3], bdAddr->addr[2], bdAddr->addr[1], bdAddr->addr[0], /* addr下标 0 1 2 3 */
        req_type, number);
}

// 回调：扫描结果处理/发现结果上报
void BtGapDiscoveryResultCallback(const bd_addr_t *bdAddr)
{
#if !defined(_WIN32)
    if ((gap_get_device_class(bdAddr) & DEVICE_MAJOR_CLASS_PHONE) != 0) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BtGapDiscoveryResultCallback: Mobile Phone Device");
        return;
    }
#endif

    if (bdAddr == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "addr nullptr");
        return;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "gap_discovery_result_callback addr: %02x%02x%02x%02x%02x%02x",
        bdAddr->addr[5], bdAddr->addr[4], bdAddr->addr[3], /* mac下标 0 1 2 3 4 5 */
        bdAddr->addr[2], bdAddr->addr[1], bdAddr->addr[0]); /* mac下标 0 1 2 3 4 5 */
    GraphicService::GetInstance()->PostGraphicEvent(std::bind(&BluetoothModel::BtGapDiscoveryResultCallbakHandle,
                                                              BluetoothModel::GetInstance(), *bdAddr));
}

void HandleBeforeChangeView()
{
    int curView = BluetoothModel::GetInstance()->GetCurView();
    if (curView == BLUETOOTHEARPHONEVIEW) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "HandleBeforeChangeView curView == BLUETOOTHEARPHONEVIEW");
        BluetoothEarPhoneView *bluetoothEarPhoneView = BluetoothEarPhoneView::GetInstance();
        if (bluetoothEarPhoneView == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "HandleBeforeChangeView bluetoothEarPhoneView == nullptr");
            return;
        }
        bluetoothEarPhoneView->BtAnimatorStop();
        bluetoothEarPhoneView->StopBtDiscovery();
    } else if (curView == BLUETOOTHEARLISTVIEW) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "HandleBeforeChangeView curView == BLUETOOTHEARLISTVIEW");
        BluetoothEarPhoneView *bluetoothEarPhone = BluetoothEarPhoneView::GetInstance();
        if (bluetoothEarPhone == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "HandleBeforeChangeView bluetoothEarPhoneView == nullptr");
            return;
        }
        bluetoothEarPhone->StopBtDiscovery();
        BluetoothItemView *pbItermView = BluetoothModel::GetInstance()->GetConnectionView();
        if (pbItermView == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "HandleBeforeChangeView The GetConnectionView get failed.");
            return;
        }
        pbItermView->ConnectAnimatorStop();
        BluetoothModel::GetInstance()->SetConnectionView(nullptr);
    } else if (curView == BLUETOOTHRECONNECTVIEW) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "HandleBeforeChangeView curView == BLUETOOTHRECONNECTVIEW");
        BluetoothReconnectView *bluetoothReconnectView = BluetoothReconnectView::GetInstance();
        if (bluetoothReconnectView == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "HandleBeforeChangeView bluetoothReconnectView == nullptr");
            return;
        }
        bluetoothReconnectView->BtReconnectStop();
    }
}

static void ACLConnectionSuss(const bd_addr_t &bdAddr)
{
    // 判断是否已退出蓝牙模块（无需跳转）
    bool tagConnectView = BluetoothModel::GetInstance()->IsTagConnectView();
    if (!tagConnectView) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "GapAclStateChangedCallback tagConnectView fail");
        return;
    }
    // 成功提示
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "GAP_ACL_STATE_CONNECTED");
    HandleBeforeChangeView();
    if (BluetoothModel::GetInstance()->GetBluetoothCalleridStatus() == CALLERPLAYERS) {
#if !defined(_WIN32)
        JumpView *jumpView = JumpView::GetInstance();
        if (jumpView == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "jumpView GetInstance fail");
            return;
        }
        jumpView->ChangeView(BluetoothModel::GetInstance()->GetCurView(),
                             JumpView::BluetoothJumpViewNum::TIPSSUCCMESSAGEVIEW);
#endif
    } else if (BluetoothModel::GetInstance()->GetBluetoothCalleridStatus() == CALLERSETTING) {
        MainBluetoothView *mainBluetoothView = MainBluetoothView::GetInstance();
        if (mainBluetoothView == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "mainBluetoothView GetInstance fail");
            return;
        }
        mainBluetoothView->ChangeView(BluetoothModel::GetInstance()->GetCurView(), TIPSSUCCMESSAGEVIEW);
    }
    TipsSuccMessageView *tipsSuccMessageView = TipsSuccMessageView::GetInstance();
    if (tipsSuccMessageView == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "ACLConnectionSuss tipsSuccMessageView GetInstance fail");
        return;
    }
    tipsSuccMessageView->RefreshNameLabel(bdAddr);
}

static void ACLConnectionFail(const bd_addr_t &bdAddr)
{
    // 判断是否已退出蓝牙模块（无需跳转）
    bool tagConnectView = BluetoothModel::GetInstance()->IsTagConnectView();
    // 判断是否无需跳转到失败界面
    bool tagDisconnectView = BluetoothModel::GetInstance()->IsTagDisconnectView();
    if ((!tagConnectView) || (!tagDisconnectView)) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "GapAclStateChangedCallback tagDisconnectView fail");
        return;
    }
    // 失败提示页
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "conn fail");
    HandleBeforeChangeView();
    if (BluetoothModel::GetInstance()->GetBluetoothCalleridStatus() == CALLERPLAYERS) {
#if !defined(_WIN32)
        JumpView *jumpView = JumpView::GetInstance();
        if (jumpView == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "jumpView GetInstance fail");
            return;
        }
        jumpView->ChangeView(BluetoothModel::GetInstance()->GetCurView(),
                             JumpView::BluetoothJumpViewNum::TIPSCONNFAILMESSAGEVIEW);
#endif
    } else if (BluetoothModel::GetInstance()->GetBluetoothCalleridStatus() == CALLERSETTING) {
        MainBluetoothView *mainBluetoothView = MainBluetoothView::GetInstance();
        if (mainBluetoothView == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "mainBluetoothView GetInstance fail");
            return;
        }
        mainBluetoothView->ChangeView(BluetoothModel::GetInstance()->GetCurView(), TIPSCONNFAILMESSAGEVIEW);
    }
    TipsConnFailMessageView *tipsConnFailMessageView = TipsConnFailMessageView::GetInstance();
    if (tipsConnFailMessageView == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "ACLConnectionFail tipsConnFailMessageView GetInstance fail");
        return;
    }
    tipsConnFailMessageView->RefreshNameLabel(bdAddr);
}

static void DisconnectedHandle()
{
    BluetoothModel::GetInstance()->SetTagDisconnectView(false);
    BluetoothDeviceListView *bluetoothDeviceListView = BluetoothDeviceListView::GetInstance();
    if (bluetoothDeviceListView == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AclStateChangedCallback bluetoothDeviceListView  == nullptr");
        return;
    }
    UIList *deviceList = bluetoothDeviceListView->GetDeviceUIList();
    if (deviceList == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "DisconnectedHandle deviceList nullptr");
        return;
    }
    deviceList->RefreshList();
    BluetoothOptionsView *bluetoothOptionsView = BluetoothOptionsView::GetInstance();
    if (bluetoothOptionsView == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AclStateChangedCallback bluetoothOptionsView  == nullptr");
        return;
    }
    bluetoothOptionsView->RefreshText();
}

void BluetoothModel::BtGapAclStateChangedHandle(gap_acl_state_t state, unsigned int reason, const bd_addr_t &bdAddr)
{
    bool tag = BluetoothModel::GetInstance()->IsTagScanDeviceClicked();
    if ((reason == 0x13) || (reason == 0x14) || (reason == 0x15) || (reason == 0x3D) || (reason == 0x08 && (!tag))) { // 连接断开
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "The peer end is disconnected.");
        DisconnectedHandle();
    } else if (reason == 0x16) { // 主动断开连接
        DisconnectedHandle();
        BluetoothCaseInfo *onClickBtCaseInfo = BluetoothModel::GetInstance()->GetTempMacValue();
        if (BluetoothModel::GetInstance()->IsTagConnectNew()) {
            errcode_t retVal = ConnectRemoteDevice((const bd_addr_t *)&onClickBtCaseInfo->addr);
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ConnectRemoteDevice retVal =  %d", retVal);
        }
    } else if ((reason == 0xb) || (reason > 0x45)) { // 0xb:重复点击连接按钮 0x45:acl状态最大参数
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "reason is false.");
        SetTagDisconnectView(false);
    }

    BluetoothCaseInfo *onClickBtCaseInfo = GetTempMacValue();

    if ((state == GAP_ACL_STATE_CONNECTED) || (state == GAP_ACL_STATE_LE_CONNECTED)) {
        SetLastConnedAddr(&bdAddr);
        // 将数据存入设备列表
        tag = CheckExistConnedList(*onClickBtCaseInfo);
        if (!tag) {
            AddConnedDevicesCase(*onClickBtCaseInfo);
        }
    } else {
        ACLConnectionFail(bdAddr);
    }

    if ((state == GAP_ACL_STATE_CONNECTED) || (state == GAP_ACL_STATE_LE_CONNECTED)) {
        if (BluetoothModel::GetInstance()->ConnectRemoteCheck(&bdAddr)) {
#if !defined(_WIN32)
            int32_t retA2dp = a2dp_src_connect(&bdAddr);
            if (retA2dp) {
                ACLConnectionFail(bdAddr);
            }
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "a2dp_src_connect result 0x%x", retA2dp);
#endif
        } else {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ingore a2dp_src_connect");
        }
    }
}

// 回调：ACL连接状态上报
void GapAclStateChangedCallback(const bd_addr_t *bdAddr, gap_acl_state_t state, unsigned int reason)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "bt GapAclStateChangedCallback state %d, reason %x succ or err", state, reason);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "addr: %02x%02x%02x%02x%02x%02x",
        bdAddr->addr[5], bdAddr->addr[4], bdAddr->addr[3], /* mac下标 0 1 2 3 4 5 */
        bdAddr->addr[2], bdAddr->addr[1], bdAddr->addr[0]); /* mac下标 0 1 2 3 4 5 */
#if !defined(_WIN32)
    if ((gap_get_device_class(bdAddr) & 0x200) != 0) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "GapAclStateChangedCallback: Mobile Phone Device");
        return;
    }
#endif
    GraphicService::GetInstance()->PostGraphicEvent(
        std::bind(&BluetoothModel::BtGapAclStateChangedHandle, BluetoothModel::GetInstance(), state, reason, *bdAddr));
}

// 回调：远端设备名称变化
void BtRemoteNameChangedCallback(const bd_addr_t *bdAddr, const unsigned char *deviceName, unsigned char length)
{
#if !defined(_WIN32)
    if ((gap_get_device_class(bdAddr) & 0x200) != 0) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BtRemoteNameChangedCallback: Mobile Phone Device");
        return;
    }
#endif
    BluetoothCaseInfo caseInfo{};
    if (bdAddr == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "BtRemoteNameChangedCallback addr nullptr");
        return;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BtRemoteNameChangedCallback addr: %02x%02x%02x%02x%02x%02x",
                  bdAddr->addr[5], bdAddr->addr[4], bdAddr->addr[3], /* mac下标 0 1 2 3 4 5 */
                  bdAddr->addr[2], bdAddr->addr[1], bdAddr->addr[0]); /* mac下标 0 1 2 3 4 5 */

    (void)memcpy_s(caseInfo.addr, BD_ADDR_LEN, bdAddr->addr, BD_ADDR_LEN);
    int ret = memcpy_s(caseInfo.deviceName, BD_NAME_LEN, deviceName, length);
    if (ret != EOK) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "BtRemoteNameChangedCallback memcpy fail!");
        return;
    }
    BluetoothModel::GetInstance()->ResetScanDevicesCase(caseInfo);
}

void BtIsAcceptConnOnSafeModeCallback(const bd_addr_t *bdAddr, bool *res)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BtIsAcceptConnOnSafeModeCallback addr: %02x%02x%02x%02x%02x%02x set *res true",
        bdAddr->addr[5], bdAddr->addr[4], bdAddr->addr[3], /* addr下标 3 4 5 */
        bdAddr->addr[2], bdAddr->addr[1], bdAddr->addr[0]); /* addr下标 0 1 2 */
    *res = true;
}

static void A2dpSrcConnectstateChanged(bd_addr_t *bdAddr, int connState)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "a2dp_src_connectStateChangedCallback addr: %02x%02x%02x%02x%02x%02x connState: %x",
        bdAddr->addr[5], bdAddr->addr[4], bdAddr->addr[3], /* 5 4 3 idx */
        bdAddr->addr[2], bdAddr->addr[1], bdAddr->addr[0], connState); /* 2 1 0 idx */
#if !defined(_WIN32)
    if ((gap_get_device_class(bdAddr) & 0x200) != 0) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "a2dp_src_connectstateChangedCb: Mobile Phone Device");
        return;
    }
#endif
    if (connState == PROFILE_STATE_CONNECTED) {
#if !defined(_WIN32)
        avrcp_ct_connect(bdAddr);
#endif
        PhoneAudioManagerSetDeviceConnState(OUT_BLUETOOTH_A2DP, AUDIO_DEVICE_CONNECTED);
        GraphicService::GetInstance()->PostGraphicEvent(std::bind(ACLConnectionSuss, *bdAddr));
    }

    if (connState == PROFILE_STATE_DISCONNECTED) {
        PhoneAudioManagerSetDeviceConnState(OUT_BLUETOOTH_A2DP, AUDIO_DEVICE_DISCONNECTED);
        GraphicService::GetInstance()->PostGraphicEvent(std::bind(ACLConnectionFail, *bdAddr));
    }
}

static void A2dpSrcPlayingStateChanged(bd_addr_t *bdAddr, int playState)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "A2dpSrcPlayingStateChangedCallback addr: %02x%02x%02x%02x%02x%02x playState: %x",
        bdAddr->addr[5], bdAddr->addr[4], bdAddr->addr[3], /* 5 4 3 idx */
        bdAddr->addr[2], bdAddr->addr[1], bdAddr->addr[0], playState); /* 2 1 0 idx */
}

static void A2dpSrcConfigureChanged(bd_addr_t *bdAddr, a2dp_codec_info_t *info)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "A2dpSrcConfigurationChangedCallback addr: %02x%02x%02x%02x%02x%02x ",
        bdAddr->addr[5], bdAddr->addr[4], bdAddr->addr[3], /* 5 4 3 idx */
        bdAddr->addr[2], bdAddr->addr[1], bdAddr->addr[0]); /* 2 1 0 idx */
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "codec_priority: %04x codec_type: %04x sample_rate: %02x bits_per_sample: %02x channel_mode: %02x ",
        info->codec_priority, info->codec_type, info->sample_rate, info->bits_per_sample, info->channel_mode);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "codec_specific1: %04x codec_specific2: %04x codec_specific3: %04x codec_specific4: %04x",
        info->codec_specific1, info->codec_specific2, info->codec_specific3, info->codec_specific4);
}

static void BleConnStateChangeCallback(unsigned short connId, bd_addr_t *addr, gap_ble_conn_state_t connState,
                                       gap_ble_pair_state_t pairState, gap_ble_disc_reason_t discReason)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "BleConnStateChangeCallback connId 0x%x, connState 0x%x, pairState 0x%x",
                  connId, connState, pairState);
    if (GAP_BLE_STATE_DISCONNECTED == connState) { // restart ble adv when disconnected
        /* 通知msg_center，ble状态仍用bt状态，此处不区分 */
#if !defined(_WIN32)
        SendMsgToMsgCenterService(BT_DISCONNECTED_TO_MSGCENTER, 0, nullptr, 0);
#endif
    } else {
#if !defined(_WIN32)
        SendMsgToMsgCenterService(BT_CONNECTED_TO_MSGCENTER, 0, nullptr, 0);
        (void)memcpy_s(&g_bd_addr, sizeof(bd_addr_t), addr, sizeof(bd_addr_t));
#endif
    }
}

static void BleAdvStartCallback(uint8_t advId, adv_status_t status)
{
    if (g_sample_adv_id == advId) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "BleAdvStartCallback advId: %d, status:%d\n", advId, status);
        BluetoothModel::GetInstance()->SetAdvStartState(true);
    }
}

static void BleAdvStopCallback(uint8_t advId, adv_status_t status)
{
    if (g_sample_adv_id == advId) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "BleAdvStopCallback advId: %d, status:%d\n", advId, status);
        BluetoothModel::GetInstance()->SetAdvStartState(false);
    }
}

static void BleAdvTerminateCallback(uint8_t advId, adv_status_t status)
{
    if (g_sample_adv_id == advId) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "BleAdvTerminateCallback advId: %d, status:%d\n", advId, status);
        BluetoothModel::GetInstance()->SetAdvStartState(false);
        // 此处开广播是为了实验室测试需要，客户需要自己修改
        BleAdvStart();
    }
}

static void BtGattsServiceAddCallback(uint8_t serverId, bt_uuid_t *uuid, uint16_t srvcHandle, errcode_t status)
{
    if ((uuid->uuid_len == sizeof(uint16_t)) && (*((uint16_t*)uuid->uuid) == 0x1FF0)) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "BtGattsServiceAddCallback 0x%x 0x%x 0x%x",
            status, serverId, srvcHandle);
        uint16_t sample_id = 0x1FF1;
        uint16_t diag_id = 0x1FF2;
        gatts_add_chara_info_t character;
        uint16_t value = 0;

        memset_s(&character, sizeof(gatts_add_chara_info_t), 0, sizeof(gatts_add_chara_info_t));
        character.chara_uuid.uuid_len = sizeof(sample_id);
        memcpy_s(character.chara_uuid.uuid, sizeof(character.chara_uuid.uuid), (char *)&sample_id, sizeof(sample_id));
        character.permissions = GATT_ATTRIBUTE_PERMISSION_WRITE;
        character.properties = GATT_CHARACTER_PROPERTY_BIT_WRITE_NO_RSP;
        character.value_len = (uint16_t)sizeof(value);
        character.value = (uint8_t *)(&value);
#if !defined(_WIN32)
        int ret = gatts_add_characteristic(serverId, srvcHandle, &character);
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "gatts_add_characteristic sample ret 0x%x", ret);

        memset_s(&character, sizeof(gatts_add_chara_info_t), 0, sizeof(gatts_add_chara_info_t));
        character.chara_uuid.uuid_len = sizeof(diag_id);
        memcpy_s(character.chara_uuid.uuid, sizeof(character.chara_uuid.uuid), (char *)&diag_id, sizeof(diag_id));
        character.permissions = GATT_ATTRIBUTE_PERMISSION_WRITE | GATT_ATTRIBUTE_PERMISSION_READ;
        character.properties = GATT_CHARACTER_PROPERTY_BIT_WRITE | GATT_CHARACTER_PROPERTY_BIT_READ;
        character.value_len = (uint16_t)sizeof(value);
        character.value = (uint8_t *)(&value);

        ret = gatts_add_characteristic(serverId, srvcHandle, &character);
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "gatts_add_characteristic diag ret 0x%x", ret);

        g_sample_svr_handle = srvcHandle;
#endif
    }
}

static void BtGattsCharacteristicAddCallback(uint8_t serverId, bt_uuid_t *uuid, uint16_t srvcHandle, gatts_add_character_result_t *result, errcode_t status)
{
#if !defined(_WIN32)
    if ((uuid->uuid_len == sizeof(uint16_t)) && (*((uint16_t*)uuid->uuid) == 0x1FF1)) {
        g_sample_characteristic_handle = result->value_handle;
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "BtGattsCharacteristicAddCallback 0x%x 0x%x 0x%x 0x%x",
                      status, serverId, srvcHandle, g_sample_characteristic_handle);
    }
    if ((uuid->uuid_len == sizeof(uint16_t)) && (*((uint16_t*)uuid->uuid) == 0x1FF2)) {
        g_diag_characteristic_handle = result->value_handle;
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "BtGattsCharacteristicAddCallback 0x%x 0x%x 0x%x 0x%x",
                      status, serverId, srvcHandle, g_diag_characteristic_handle);
    }
    if (g_sample_characteristic_handle != 0 && g_diag_characteristic_handle !=0) {
        int ret = gatts_start_service(serverId, srvcHandle);
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "gatts_start_service ret 0x%x", ret);
    }
#endif
}

static void BtGattsDescriptorAddCallback(uint8_t serverId, bt_uuid_t *uuid, uint16_t srvcHandle, uint16_t descriptorHandle, errcode_t status)
{
    if (g_sample_svr_handle == srvcHandle) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "BtGattsDescriptorAddCallback 0x%x 0x%x 0x%x 0x%x",
            status, serverId, srvcHandle, descriptorHandle);
    }
}

static void BtGattsServiceStartCallback(uint8_t serverId, uint16_t srvcHandle, errcode_t status)
{
    if (g_sample_svr_handle == srvcHandle) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "BtGattsServiceStartCallback 0x%x 0x%x 0x%x", status, serverId, srvcHandle);
    }
}

static void BtGattsServiceStopCallback(uint8_t serverId, uint16_t srvcHandle, errcode_t status)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "BtGattsServiceStopCallback 0x%x 0x%x 0x%x", status, serverId, srvcHandle);
}

static void BtGattsServiceDeleteCallback(uint8_t serverId, errcode_t status)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "BtGattsServiceDeleteCallback 0x%x 0x%x", status, serverId);
}

static void BtGattsRequestReadCallback(uint8_t serverId, uint16_t connId, gatts_req_read_cb_t *readCbPara, errcode_t status)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "BtGattsRequestReadCallback");
}

static void BtGattsRequestWriteCallback(uint8_t serverId, uint16_t connId, gatts_req_write_cb_t *writeCbPara,  errcode_t status)
{
#if !defined(_WIN32)
    if (g_sample_characteristic_handle == writeCbPara->handle) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "BtGattsRequestWriteCallback");
        if (writeCbPara->length == 9) {
            uint8_t ctkd[] = {'o', 'p', 'e', 'n', ' ', 'c', 't', 'k', 'd'};
            if (memcmp(ctkd, writeCbPara->value, writeCbPara->length) == 0) {
                WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "enable ctkd");
                // 一键双连使能（依赖安全连接参数配置）
                errcode_t ret = gap_ble_set_ctkd_enable(true);
                WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "gap_ble_set_ctkd_enable ret = %u", ret);

                // 发起ble配对
                ret = gap_ble_pair_remote_device(&g_bd_addr);
                WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "gap_ble_pair_remote_device ret = %u ", ret);
            }
        } else if (writeCbPara->length == 10) {
            uint8_t ctkd[] = {'c', 'l', 'o', 's', 'e', ' ', 'c', 't', 'k', 'd'};
            if (memcmp(ctkd, writeCbPara->value, writeCbPara->length) == 0) {
                // 一键双连去使能
                errcode_t ret = gap_ble_set_ctkd_enable(false);
                WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "gap_ble_set_ctkd_disable ret = %u", ret);
            }
        }
    } else if (g_diag_characteristic_handle == writeCbPara->handle) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "diag_gatt_write_req_cbk");
        diag_gatt_write_req_cbk(serverId, connId, writeCbPara, status);
    }
#endif
}

static void BtGattsMtuChangedCallback(uint8_t serverId, uint16_t connId, uint16_t mtuSize, errcode_t status)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "BtGattsMtuChangedCallback connId = %u mtuSize = %u", connId, mtuSize);
}

static void BtA2dpSnkAcceptTimerStop(void)
{
#if !defined(_WIN32)
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "stop timer\n");
    if (g_a2dp_stream_accept_timer.timer != nullptr) {
        (void)osal_timer_stop(&g_a2dp_stream_accept_timer);
        (void)osal_timer_destroy(&g_a2dp_stream_accept_timer);
        g_a2dp_stream_accept_timer.timer = nullptr;
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "stop timer success!\n");
#endif
}

static void BtA2dpSnStartAcceptHandler(unsigned long data)
{
    UNUSED(data);
#if !defined(_WIN32)
    bd_addr_t active_bd_addr = a2dp_snk_get_active_device();
    a2dp_snk_start_accept(&active_bd_addr);
#endif
    BtA2dpSnkAcceptTimerStop();
}

static errcode_t BtA2dpSnkStartAcceptTimer(void)
{
#if !defined(_WIN32)
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "start timer\n");
    if (g_a2dp_stream_accept_timer.timer != nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "timer is running");
        return ERRCODE_SUCC;
    }

    g_a2dp_stream_accept_timer.handler = BtA2dpSnStartAcceptHandler;
    g_a2dp_stream_accept_timer.interval = A2DP_STREAM_ACCEPT_TIMER_PERIOD;

    int32_t timer_ret = osal_timer_init(&g_a2dp_stream_accept_timer);
    if (timer_ret != EOK) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "create timer error! ret: %x\n", timer_ret);
        BtA2dpSnkAcceptTimerStop();
        return ERRCODE_FAIL;
    }
    timer_ret = osal_timer_start(&g_a2dp_stream_accept_timer);
    if (timer_ret != EOK) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "start timer error! ret: %x\n", timer_ret);
        BtA2dpSnkAcceptTimerStop();
        return ERRCODE_FAIL;
    }
#endif
    return ERRCODE_SUCC;
}

static int32_t BtA2dpSnkAudioCreate()
{
#if MEDIA_A2DP_SNK_INTERFACE
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "audioFormat %d channelCount %d sampleRate %d \n",
        g_audio_format_config.audioFormat, g_audio_format_config.channelCount, g_audio_format_config.sampleRate);

#if !defined(_WIN32)
    int32_t ret = AudioManagerA2dpSinkSetParam(g_audio_format_config);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "AudioManagerA2dpSinkSetParam ret %x\n", ret);
    ret = AudioManagerA2dpSinkCreate();
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "AudioManagerA2dpSinkCreate ret %x\n", ret);
#endif
#endif
    /* 起定时器接受音乐开始请求 */
    (void)BtA2dpSnkStartAcceptTimer();
    return 0;
}

static int32_t BtA2dpSnkAudioDestroy()
{
#if MEDIA_A2DP_SNK_INTERFACE
#if !defined(_WIN32)
    int32_t ret = AudioManagerA2dpSinkDestroy();
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "AudioManagerA2dpSinkDestroy ret %x\n", ret);
#endif
#endif
    return 0;
}

static void BtA2dpSnkConnectStateChangedCb(bd_addr_t *bd_addr, int conn_state)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "addr: ****%02x%02x%02x%02x conn_state: %x\n",
        bd_addr->addr[3], bd_addr->addr[2], bd_addr->addr[1], bd_addr->addr[0], conn_state); /* 3 2 1 0 idx */
}

static void BtA2dpSnkStartStreamReqCb(bd_addr_t *bd_addr)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "addr: ****%02x%02x%02x%02x \n",
        bd_addr->addr[3], bd_addr->addr[2], bd_addr->addr[1], bd_addr->addr[0]); /* 3 2 1 0 idx */
    BtA2dpSnkAudioCreate();
}

static void BtA2dpSnkPlayingStateChangedCb(bd_addr_t *bd_addr, int playing_state)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "addr: ****%02x%02x%02x%02x playing_state: %x\n",
        bd_addr->addr[3], bd_addr->addr[2], bd_addr->addr[1], bd_addr->addr[0], playing_state); /* 3 2 1 0 idx */
    if (playing_state == A2DP_NOT_PLAYING) {
        BtA2dpSnkAudioDestroy();
    }
}

static void BtA2dpSnkConfigChangedCb(bd_addr_t *bd_addr, a2dp_snk_codec_info_t *info)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "addr: ****%02x%02x%02x%02x ",
        bd_addr->addr[3], bd_addr->addr[2], bd_addr->addr[1], bd_addr->addr[0]); /* 3 2 1 0 idx */
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP,
        "codec_priority: %04x codec_type: %04x sample_rate: %02x bits_per_sample: %02x channel_mode: %02x",
        info->codec_priority, info->codec_type, info->sample_rate, info->bits_per_sample, info->channel_mode);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP,
        "codec_specific1: %04x codec_specific2: %04x codec_specific3: %04x codec_specific4: %04x\n",
        (unsigned int)info->codec_specific1,
        (unsigned int)info->codec_specific2,
        (unsigned int)info->codec_specific3,
        (unsigned int)info->codec_specific4);
#if MEDIA_A2DP_SNK_INTERFACE
    if (info->codec_type == A2DP_SNK_CODEC_TYPE_AAC) {
        g_audio_format_config.audioFormat = AAC_LC;
    } else {
        g_audio_format_config.audioFormat = SBC;
    }
    /* channel count 1 or 2 */
    g_audio_format_config.channelCount = (info->channel_mode == A2DP_SNK_CODEC_CHANNEL_MODE_NONE ? 1 : 2);

    switch (info->sample_rate) {
        case A2DP_SNK_CODEC_SAMPLE_RATE_48000:
            g_audio_format_config.sampleRate = BT_AUDIO_A2DP_SMAPLE_RATE48000;
            break;
        case A2DP_SNK_CODEC_SAMPLE_RATE_88200:
            g_audio_format_config.sampleRate = BT_AUDIO_A2DP_SMAPLE_RATE88200;
            break;
        case A2DP_SNK_CODEC_SAMPLE_RATE_96000:
            g_audio_format_config.sampleRate = BT_AUDIO_A2DP_SMAPLE_RATE96000;
            break;
        default:
            g_audio_format_config.sampleRate = BT_AUDIO_A2DP_SMAPLE_RATE44100;
            break;
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "audioFormat %d channelCount %d sampleRate %d \n",
        g_audio_format_config.audioFormat, g_audio_format_config.channelCount, g_audio_format_config.sampleRate);
#endif
}

// 注册回调函数
static gap_call_backs_t g_btGapCallBacks{};
static a2dp_src_callbacks_t g_a2dpSrcCallbacks{};
static gap_ble_callbacks_t g_gapBleCallbacks{};
static gatts_callbacks_t g_btGattsCallbacks{};

static a2dp_snk_callbacks_t g_a2dp_snk_callbacks = {
    BtA2dpSnkConnectStateChangedCb,
    BtA2dpSnkStartStreamReqCb,
    BtA2dpSnkPlayingStateChangedCb,
    BtA2dpSnkConfigChangedCb
};

void BtInitCallBacks()
{
#if !defined(_WIN32)
    g_btGapCallBacks.discovery_result_callback = BtGapDiscoveryResultCallback;
    g_btGapCallBacks.discovery_state_changed_callback = BtGapDiscoveryStateChangedCallback;
    g_btGapCallBacks.acl_state_changed_callbak = GapAclStateChangedCallback;
    g_btGapCallBacks.read_remote_rssi_event_callback = BtGapReadRemoteRssiEventCallback;
    g_btGapCallBacks.remote_name_changed_callback = BtRemoteNameChangedCallback;
    g_btGapCallBacks.is_accept_conn_on_safe_mode_callback = BtIsAcceptConnOnSafeModeCallback;
    g_btGapCallBacks.pair_status_changed_callback = BtGapPairStatusChangedCallback;
    g_btGapCallBacks.pair_requested_callback = BtGapPairRequestedCallback;
    g_btGapCallBacks.pair_confiremed_callback = BtGapPairConfirmedCallback;
    gap_register_callbacks(&g_btGapCallBacks);

    g_a2dpSrcCallbacks.connectstate_changed_cb = A2dpSrcConnectstateChanged;
    g_a2dpSrcCallbacks.configuration_changed_cb = A2dpSrcConfigureChanged;
    g_a2dpSrcCallbacks.playing_state_changed_cb = A2dpSrcPlayingStateChanged;
    a2dp_src_register_callbacks(&g_a2dpSrcCallbacks);

    /* a2dp sink */
    a2dp_snk_register_callbacks(&g_a2dp_snk_callbacks);
#endif
}

void BtInitCallBacksEx()
{
#if !defined(_WIN32)
    g_gapBleCallbacks.conn_state_change_cb = BleConnStateChangeCallback;
    g_gapBleCallbacks.start_adv_cb = BleAdvStartCallback;
    g_gapBleCallbacks.stop_adv_cb = BleAdvStopCallback;
    g_gapBleCallbacks.terminate_adv_cb = BleAdvTerminateCallback;
    wrapper_gap_ble_register_callbacks(&g_gapBleCallbacks);

    g_btGattsCallbacks.add_service_cb = BtGattsServiceAddCallback;
    g_btGattsCallbacks.add_characteristic_cb = BtGattsCharacteristicAddCallback;
    g_btGattsCallbacks.add_descriptor_cb = BtGattsDescriptorAddCallback;
    g_btGattsCallbacks.start_service_cb = BtGattsServiceStartCallback;
    g_btGattsCallbacks.stop_service_cb = BtGattsServiceStopCallback;
    g_btGattsCallbacks.delete_service_cb = BtGattsServiceDeleteCallback;
    g_btGattsCallbacks.read_request_cb = BtGattsRequestReadCallback;
    g_btGattsCallbacks.write_request_cb = BtGattsRequestWriteCallback;
    g_btGattsCallbacks.mtu_changed_cb = BtGattsMtuChangedCallback;
    wrapper_gatts_register_callbacks(&g_btGattsCallbacks);
#endif
}

void BtInitGatts(void)
{
    uint16_t id = 0x1FF0;
    uint16_t ser_id = 0x1FFF;
    uint8_t serverId = 0;
    bt_uuid_t uuid;
    int ret;

    uuid.uuid_len = sizeof(ser_id);
    memcpy_s(uuid.uuid, sizeof(uuid.uuid), (char *)&ser_id, sizeof(ser_id));
#if !defined(_WIN32)
    ret = wrapper_gatts_register_server(&uuid, &serverId); /* 注册gatt server */
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "gatts_register_server ret: 0x%x serverId = 0x%x", ret, serverId);


    uuid.uuid_len = sizeof(id);
    memcpy_s(uuid.uuid, sizeof(uuid.uuid), (char *)&id, sizeof(id));
    ret = wrapper_gatts_add_service(serverId, &uuid, true);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "gatts_add_service ret: 0x%x", ret);

    wrapper_gatts_service_start();
#endif
}

static uint8_t Char2Num(int8_t c)
{
    if (c >= '0' && c <= '9') {
        return (uint8_t)(c - '0');
    } else if (c >= 'A' && c <= 'F') {
        return (uint8_t)(c - 'A' + TEN);
    } else if (c >= 'a' && c <= 'f') {
        return (uint8_t)(c - 'a' + TEN);
    } else {
        return 0xFF;
    }
}

static uint32_t GenerateUuidFromString(const char *str, uint8_t len, bt_uuid_t *uuid_out)
{
    int slen = (int)strnlen(str, len);
    bool new_uuid = false;
    if (uuid_out == nullptr) {
        return ERRCODE_INVALID_PARAM;
    }

    uuid_out->uuid_len = (uint8_t)slen / 2; /* 2: 2个字符组成一个字节，因此只取偶数个字符 */
    for (int i = 0; i + 1 < slen; i += 2) { /* 2: 同上 */
        uint8_t num1 = Char2Num(str[i]);
        uint8_t num2 = Char2Num(str[i + 1]);
        if (num1 == UINT8_MAX || num2 == UINT8_MAX) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "invalid input %c%c\n", str[i], str[i + 1]);
            return ERRCODE_INVALID_PARAM;
        }

        uuid_out->uuid[i / 2] = (uint8_t)((num1 << BIT4) | num2); /* 2: 同上 */
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "%02x ", (uint8_t)(uuid_out->uuid[i / 2])); /* 2: 同上 */
    }

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "\n");
    return ERRCODE_SUCC;
}

void BtSppServerCreate(const char *spp_uuid, const char *name, uint8_t name_len)
{
    spp_create_socket_para_t param;
    if (GenerateUuidFromString((const char *)spp_uuid, (uint8_t)strlen(spp_uuid), &param.uuid) != ERRCODE_SUCC) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "get uuid fail\n");
        return;
    }

    param.socket_type = SPP_SOCKET_RFCOMM;
    param.is_encrypt = true;

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "uuid: %s uuid_len: %u\n", spp_uuid, param.uuid.uuid_len);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "socket_type: %x is_encrypt: %d name: %s nameLen: %u\n", param.socket_type,
        param.is_encrypt, name, name_len);

#if !defined(_WIN32)
    int ret = spp_server_create(&param, name, name_len);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "spp_server_create ret: %x\n", ret);
#endif
}

static int BtSppServerAcceptTaskBody(void *data)
{
#if !defined(_WIN32)
    int server_id = (int)(intptr_t)data;
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "server_id: %x\n", server_id);

    int ret = spp_server_accept(server_id);

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "spp_server_accept ret: %x\n", ret);
#endif

    return OHOS_SUCCESS;
}

/* 等待client连接 */
void BtSppServerAccept(uint8_t server_id)
{
#if !defined(_WIN32)
    osal_task *task = osal_kthread_create(BtSppServerAcceptTaskBody, (void *)(uintptr_t)server_id,
        "spp_server_accept_task", BT_SPP_TASK_STACK_SIZE);
    if (task == nullptr) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "create task fail\n");
        return;
    }

    int ret = osal_kthread_set_priority(task, BT_SPP_TASK_PRI);
    if (ret != OHOS_SUCCESS) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "set pri error: %08x\n", ret);
    }

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "create task success.\n");
#endif
}

void BtInitSpp(void)
{
    char uuid[] = "1101";
    char name[] = "COM1";
    uint8_t name_len = strlen(name) + 1;
    uint8_t server_id = 0;

    /* 创建spp */
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "create spp server uuid=%s name=%s name_len=%d\n", uuid, name, name_len);
    BtSppServerCreate(uuid, name, name_len);
    BtSppServerAccept(server_id);
}

void BleAdvStart(void)
{
#if !defined(_WIN32)
    int advId = g_sample_adv_id;
    if (BluetoothModel::GetInstance()->GetAdvStartState()) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "BleAdvStart already started");
        gap_ble_stop_adv(advId);
        osDelay(50);
    }
    gap_ble_adv_params_t adv_para;
    memset_s(&adv_para, sizeof(gap_ble_adv_params_t), 0, sizeof(gap_ble_adv_params_t));
    adv_para.min_interval = 0x30;
    adv_para.max_interval = 0x60;
    adv_para.adv_type = GAP_BLE_ADV_CONN_SCAN_UNDIR;
    adv_para.channel_map = 0x7;
    adv_para.duration = 0;
    gap_ble_set_adv_param(advId, &adv_para);
    gap_ble_set_local_appearance(GAP_BLE_APPEARANCE_TYPE_GENERIC_WATCH);
    errcode_t ret = gap_ble_start_adv(advId);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "BleAdvStart bt gap_ble_start_adv  ret = 0x%x", ret);
#endif
}

#ifdef __cplusplus
}
#endif
}
