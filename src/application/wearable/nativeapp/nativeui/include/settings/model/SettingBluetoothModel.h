/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingBluetoothModel
 * Create: 2025-06-01
 */
#ifndef SETTING_BLUETOOTH_MODEL_H
#define SETTING_BLUETOOTH_MODEL_H

#include <string>
#include <cstring>
#include <list>
#include <cstdio>
#include "gfx_utils/list.h"
#include "bts_def.h"
#include "bts_br_gap.h"
#include "bs_ble_gatt_server.h"
#include "bs_br_service_api.h"
#include "settings/common/SettingUnionItemGroup.h"
#include "settings/common/SettingRefSingleton.h"
#include "samgr_lite.h"

namespace OHOS {
typedef struct {
    unsigned char deviceName[BT_DEVICE_NAME_MAX_LEN]; /* Name of the device record, must be in UTF-8 */
    unsigned char addr[BD_ADDR_LEN];
    std::string connect_status;
    int16_t rssi; // 蓝牙信号强弱
    uint32_t device_class;
} BtCaseInfo;

typedef enum {
    BT_MAIN_PAGE = 0,
    RECONNECT_FRAGMENT,
    RECONNECT_SECOND_CONFIRM_FRAGMENT,
    UNPAIR_CONFIRM_FRAGMENT,
    CONNECT_NEW_PHONE_FRAGMENT,
    CONNECT_OPEN_FRAGMENT,
    CONNECT_BLUETOOTH_SUCCESS_FRAGMENT,
    CONNECT_BLUETOOTH_FAIL_FRAGMENT,
} BtGroupId;

typedef  enum {
    GATT_DEFAULT_SERVICE_CTKD_CHARACTER = 0,
    GATT_DEFAULT_SERVICE_CHARA_DESC_MAX,
} GattDefaultServiceCharaDescId;

typedef struct {
    bt_uuid_t uuid;                              /*!< @if Eng uuid of this service.
                                                      @else   该服务uuid。 @endif */
    uint8_t is_primary;                          /*!< @if Eng is primary service tag
                                                      @else   是否是主服务标记 */
    uint8_t attribute_count;                     /*!< @if Eng size of attribute_array.
                                                      @else   attribute_array 中存储的属性个数 */
    bs_gatt_service_callbacks_t cbks;            /*!< @if Eng server callback function.
                                                      @else   服务端回调函数 @endif */
    bs_gatt_service_extend_param_t extend_param; /*!< @if Eng server structure extend param.
                                                      @else   服务端扩展参数 @endif */
    bs_gatts_attribute_info attribute_array[GATT_DEFAULT_SERVICE_CHARA_DESC_MAX]; /*!< @if Eng point array of
                                                                        attribute config.
                                                                        @else   特征/描述符配置数组。 @endif */
} GattDefaultServiceTemplate;

class SettingBluetoothModel {
public:
    ~SettingBluetoothModel(){};
    static SettingBluetoothModel *GetInstance(void);
    void Init();
    std::list<BtCaseInfo>& GetScansDevicesList(); // 获取扫描到的设备列表
    void ScansDevicesListAddCase(BtCaseInfo caseInfo); // 设备列表新增数据
    void ScansDevicesListDelCase(BtCaseInfo caseInfo); // 扫描列表剔除rssi值低的数据
    void SetScansCaseRssi(const bd_addr_t *bdAddr, int rssi); // 设置设备的信号值
    void SortScansCaseRssi(); // 根据信号值对设备列表进行排序
    void ClearScansDevicesList();
    bool CheckExistScanListAndUpdateRssi(BtCaseInfo caseInfo); // 去重检查并更新rssi
    void ResetScanDevicesCase(const BtCaseInfo& caseInfo); // 刷新名称变化的设备信息
    // 已配对蓝牙设备
    void SetPairedDevicesListClear();
    void AddPairedDevicesCase(BtCaseInfo caseInfo);
    bool CheckExistPairedList(BtCaseInfo caseInfo);
    List<BtCaseInfo>& GetPairedDevicesCase();
    // 被选中的设备信息
    void SetSelectedDeviceInfo(BtCaseInfo *caseInfo);
    BtCaseInfo *GetSelectedDeviceInfo(void);
    // 最后连接的设备地址
    void SetLastConnedAddr(const bd_addr_t *addr);
    bd_addr_t *GetLastConnedAddr(void);
    void SetPhoneConnected(bool status);
    bool GetPhoneConnected(void);
    void UpdateConnectStatus();
    void SettingGapDiscoveryResultCallbakHandle(const bd_addr_t &bdAddr);
    void SettingGapAclStateChangedHandle(gap_acl_state_t state, unsigned int reason, const bd_addr_t &bdAddr);
    void SettingScanCompleteMsgProcess(BtCaseInfo scan_msg, bool need_to_del);
    void SettingConnectMsgProcess(const bd_addr_t &bdAddr, uint8_t reason, uint8_t state, uint32_t device_class);
    void SettingDisconnectMsgProcess(const bd_addr_t &bdAddr, uint8_t reason, uint8_t state, uint32_t device_class);
    void A2dpSrcStateChangedProcess(const bd_addr_t &bdAddr, uint8_t state);
    void SettingEnableMsgProcess(bs_protocol_type_t protocal);
    bool GetAdvStartState(void);
    void SetAdvStartState(bool state);
    int ConnectRemoteDevice(const bd_addr_t *addr);
    bool ConnectRemoteCheck(const bd_addr_t *addr);
    char* GetWatchName(void);
    // 耳机蓝牙连接状态
    void SetHeadPhoneConnectStatus(bool status);
    bool GetHeadPhoneConnectStatus();
    void SetDeviceName(std::string deviceName_);
    std::string GetDeviceName();
    void SetEarPhoneListHasData(bool status);
    uint32_t GetEarPhoneListHasData();
    void MoveMatchingDeviceToFront();
    //  更新UI上设备列表的信息
    void UpdateConnectStatusForDevice();
    void UpdateDisconnectStatusForDevice(const bd_addr_t &bdAddr);
    void UpdateConnectedStatusForDevice(const bd_addr_t &bdAddr);
    // 设置是否展示成功或失败的页面
    void SetConnectResultPage(bool redirct);
    bool GetConnectResultPage();
    // 取消配对
    void HandleUnpairRequest(void);
    void AddNodeFromPairListToScanList(const bd_addr_t &bdAddr);
    void SetConnectFragmentStatus(bool status);
    bool GetConnectFragmentStatus();
    bd_addr_t lastConnedAddr_ = {{0}, 0};
    char watchName_[BT_DEVICE_NAME_MAX_LEN] = { 0 };
    BtCaseInfo selectedDeviceInfo_{};
    std::list<BtCaseInfo> btScansDevicesList_{};
    List<BtCaseInfo> *btPairedDevicesList_{nullptr};
private:
    SettingBluetoothModel();
    void SetUpPairedDevicesList();
    void InitBluetoothInfo(void);
    bool phoneConnectStatus_{false};
    bool headPhoneConnectStatus_{false};
    bd_addr_t remoteConnectAddr_ = {{0}, 0};
    bool advStartState_{false};
    std::string deviceName_;
    uint32_t EarPhoneListHasData_ = 0;
    bool redirct_ = false;
    bool connectFragmentStatus_ = false;
};

#ifdef __cplusplus
extern "C" {
#endif
void SettingGapReadRemoteRssiEventCallback(const bd_addr_t *bd_addr, int8_t rssi, errcode_t status);
void SettingGapDiscoveryStateChangedCallback(uint8_t status);
void SettingBtInitCallBacks(void);
void SettingBtInitCallBacksEx(void);
void SettingBtInitGatts(void);
void SettingBleAdvStart(void);
bd_addr_t* GetBtAddr();
#ifdef __cplusplus
}
#endif
}

#endif // SETTING_BULETOOTH_MODEL_H