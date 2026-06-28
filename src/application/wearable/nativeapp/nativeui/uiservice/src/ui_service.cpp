/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2025. All rights reserved.
 * Description: ui_service.cpp
 * Create: 2021-01-09
 */

#include "uiservice/ui_service.h"
#include "wearable_log.h"
#include "samgr_lite.h"
#include "ux_api_bt.h"
#include "adapter_bt_br.h"
#include "settings/model/SettingBluetoothModel.h"
#include "settings/model/SettingBtHeadset.h"
#include "settings/page/SettingBluetoothPage.h"
#include "phoneservice/PhoneService.h"
#include "main/UiServersMsg.h"
#include "ohos_init.h"
#include "memory_adapter.h"
#include <cstdio>
#include "service_id_define.h"
#include "bundle_install_msg.h"
#include "graphic_service.h"
#include "applist/ApplistModel.h"
#if defined(SUPPORT_POWER_MANAGER)
#include "power_display_service.h"
#endif
#include "screennotify/ui_screennotify.h"
#include "main/PullUpDownModel.h"
#include "bts_br_gap.h"
#include "bts_le_gap.h"
#include "uiservice/ui_language.h"
#include "errcode.h"
#include "pm_veto.h"
#include "NativeAbility.h"
#include "TransitionType.h"

extern "C" errcode_t uapi_pm_add_sleep_veto(pm_veto_id_t veto_id);
extern "C" errcode_t uapi_pm_remove_sleep_veto(pm_veto_id_t veto_id);

#ifdef __cplusplus
extern "C" {
#endif

static const char *GetServiceName(Service *service);
static BOOL ServiceInitialize(Service *service, Identity identity);
static TaskConfig GetServiceTaskConfig(Service *service);
static BOOL ServiceMessageHandle(Service *service, Request *request);
static void ServiceMsgProcess(Request *request);
static int32_t SendMsg(int32_t msgId, uint32_t msgValue, void *data, int32_t dataLen);

static UiService g_uiService = {
    .GetName = GetServiceName,
    .Initialize = ServiceInitialize,
    .MessageHandle = ServiceMessageHandle,
    .GetTaskConfig = GetServiceTaskConfig,
    DEFAULT_IUNKNOWN_ENTRY_BEGIN,
    .SendMsg = SendMsg,
    DEFAULT_IUNKNOWN_ENTRY_END,
    .identity = { -1, -1, nullptr },
};

const int DATA_DIST_STACK_SIZE = 0xC00;
const int DATA_DIST_MAIL_LENGTH = 10;
const int BYTE_OFFSET = 8;
static uint32_t g_interval = 5;

void ScreenTurnOn(int32_t timeout)
{
    if (OHOS::PullUpDownModel::GetInstance()->GetBriScrStatus() == 1) {
        return;
    }
#if defined(SUPPORT_POWER_MANAGER)
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[ScreenTurn] timeout = %d", timeout);
    g_interval = get_back_to_home_interval();
    set_back_to_home_interval(0xFFFFFFFF);
    const power_display_svr_api_t *display_api = power_display_svr_get_api();
    if (display_api->get_screen_state() != SCREEN_ON) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[ScreenTurn] turn_on_screen");
        display_api->turn_on_screen();
    }
    display_api->set_screen_set_keepon_timeout(timeout);
#endif
    return;
}

void ScreenTurnOff(void)
{
    if (OHOS::PullUpDownModel::GetInstance()->GetBriScrStatus() == 1) {
        return;
    }
#if defined(SUPPORT_POWER_MANAGER)
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[ScreenTurnOff] timeout auto");
    set_back_to_home_interval(g_interval);
    const power_display_svr_api_t *display_api = power_display_svr_get_api();
    display_api->set_screen_set_keepon_timeout(0);
#endif
    return;
}

void ScreenOffEnable(int32_t enable)
{
#if defined(SUPPORT_POWER_MANAGER)
    const power_display_svr_api_t *display_api = power_display_svr_get_api();
    display_api->set_auto_timeout_function(enable);
#endif
}

void ConnectNewPhone(void)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "ConnectNewPhone.\n");
    uint16_t dev_num = 0;
    const ux_api_bt_device_info_t *mobile_ble_info = NULL;
    const ux_api_bt_device_info_t *mobile_br_info = NULL;
    errcode_t err = ERRCODE_SUCC;
    mobile_br_info = ux_api_bt_get_device_info(UX_API_BT_AS_CENTRAL, UX_API_BT_PROTOCOL_BR, &dev_num);
    mobile_ble_info = ux_api_bt_get_device_info(UX_API_BT_AS_CENTRAL, UX_API_BT_PROTOCOL_BLE, &dev_num);
    if (mobile_br_info != NULL) {
        ux_api_bt_remove_device(&mobile_br_info->addr, UX_API_BT_PROTOCOL_BR);
    }
    if (mobile_ble_info != NULL) {
        ux_api_bt_remove_device(&mobile_ble_info->addr, UX_API_BT_PROTOCOL_BLE);
    }
    /* 停扫描, 避免干扰广播 */
    OHOS::SettingBluetoothPage::GetInstance()->StopBtScan();
    err = ux_api_bt_start_discovery(UX_API_BT_AS_PERIPHERAL, UX_API_BT_PROTOCOL_BR);
    if (err != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "ux_api_br_start_discovery ret = 0x%x", err);
    }
    err = ux_api_bt_start_discovery(UX_API_BT_AS_PERIPHERAL, UX_API_BT_PROTOCOL_BLE);
    if (err != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "ux_api_ble_start_discovery ret = 0x%x", err);
    }
}

static void Init()
{
    SamgrLite *sm = SAMGR_GetInstance();
    BOOL result = sm->RegisterService((Service *)&g_uiService);
    sm->RegisterDefaultFeatureApi(UI_SERVICE, GET_IUNKNOWN(g_uiService));
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_UI_SERVICE, "UiService starts %s", result ? "successfully" : "unsuccessfully");
}

SYSEX_SERVICE_INIT_PRI(Init, 2);

const char *GetServiceName(Service *service)
{
    (void)service;
    return UI_SERVICE;
}

BOOL ServiceInitialize(Service *service, Identity identity)
{
    if (service == nullptr) {
        return FALSE;
    }
    UiService *uiService = reinterpret_cast<UiService *>(service);
    uiService->identity = identity;

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_UI_SERVICE, "UiService init serviceId = %d, featureId = %d, queueId = %d",
                  identity.serviceId, identity.featureId, identity.queueId);
    return TRUE;
}

static void ReadScanMsg(ux_bt_scan_msg_t *scan_msg, OHOS::BtCaseInfo *scan_device)
{
    if (strcpy_s((char *)(scan_device->deviceName), sizeof(scan_msg->deviceName),
        (const char *)(scan_msg->deviceName)) != 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "ui_service stcpy scan name fail");
        return;
    }
    (void)memcpy_s(scan_device->addr, BS_ADDR_LEN, scan_msg->addr, BS_ADDR_LEN);
    scan_device->rssi = scan_msg->rssi;
    scan_device->device_class = scan_msg->device_class;
}

void ServiceMsgProcess(Request *request)
{
    ux_bt_hfp_msg_t  *hfp_msg = nullptr;
    ux_bt_sco_msg_t  *sco_msg = nullptr;
    ux_bt_call_msg_t *call_msg = nullptr;
    ux_bt_a2dp_msg_t *a2dp_msg = nullptr;
    ux_bt_scan_msg_t *scan_msg = nullptr;
    OHOS::BtCaseInfo scan_device;
    ux_bt_avrcp_play_control_msg_t *play_control_msg = nullptr;
    errno_t ret;
    switch (request->msgId) {
        case BMS_REFRESH_APPLIST_EVENT_TO_UI:
            GraphicService::GetInstance()->PostGraphicEvent(
                std::bind(&OHOS::ApplistModel::ApplistUpdate, &OHOS::ApplistModel::GetInstance()));
            break;
        case JS_INSTALL_SUCESS_TO_UI:
        case JS_UNINSTALL_SUCESS_TO_UI:
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_UI_SERVICE, "bms msg_id = %d bundleName = %s",
                          request->msgId, ((BundleData *)request->data)->bundleName);
            GraphicService::GetInstance()->PostGraphicEvent(
                std::bind(&OHOS::ApplistModel::ApplistUpdate, &OHOS::ApplistModel::GetInstance()));
            break;
        case BT_CONNECTED_TO_UI:
            GraphicService::GetInstance()->PostGraphicEvent(
                std::bind(&OHOS::SettingBluetoothModel::UpdateConnectStatus,
                    OHOS::SettingBluetoothModel::GetInstance()));
            break;
        case BT_DISCONNECTED_TO_UI:
            GraphicService::GetInstance()->PostGraphicEvent(
                std::bind(&OHOS::SettingBluetoothModel::UpdateConnectStatus,
                    OHOS::SettingBluetoothModel::GetInstance()));
            break;
        case UI_ADD_SLEEP_VETO:
            uapi_pm_add_sleep_veto(PM_ID_SCREEN);
            break;
        case UI_RM_SLEEP_VETO:
            uapi_pm_remove_sleep_veto(PM_ID_SCREEN);
            break;
        case CONNECT_NEW_PHONE:
            ConnectNewPhone();
            break;
        case UI_UPDATE_LANGUAGE:
            OHOS::UILanguage::UpdateLanguage();
            break;
        case UI_OTA_UPDATE:
            power_display_svr_get_api()->turn_on_screen();
            OHOS::NativeAbility::GetInstance().SwitchSlice(VIEW_SETTING, OHOS::SettingPageId::SYSTEM_OTA_PAGE,
                OHOS::TransitionType::TRANSITION_INVALID, true);
            break;
        case BT_HFP_STATE_CHANGE:
            hfp_msg = (ux_bt_hfp_msg_t *)(request->data);
            GraphicService::GetInstance()->PostGraphicEvent(std::bind(ConnStateChangedProcess,
                hfp_msg->bd_addr, hfp_msg->hfp_state));
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "uiservice recv BT_HFP_STATE_CHANGE");
            break;
        case BT_CALL_STATE_CHANGE:
            call_msg = (ux_bt_call_msg_t *)(request->data);
            PhoneHfpCallInfo callinfo;
            ret = strcpy_s((char *)callinfo.number, sizeof(callinfo.number), (char *)call_msg->number);
            if (ret != 0) {
                WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "strcpy_s failed with error code: %d\n", ret);
            }
            callinfo.number_len = call_msg->number_len;
            callinfo.state = call_msg->state;
            callinfo.outgoing = call_msg->outgoing;
            GraphicService::GetInstance()->PostGraphicEvent(std::bind(CallStateChangedProcess, call_msg->bd_addr,
                callinfo));
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "uiservice recv BT_CALL_STATE_CHANGE");
            break;
        case BT_SCO_STATE_CHANGE:
            sco_msg = (ux_bt_sco_msg_t *)(request->data);
            GraphicService::GetInstance()->PostGraphicEvent(std::bind(ScoStateChangedProcess, sco_msg->bd_addr,
                sco_msg->sco_state));
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "uiservice recv BT_SCO_STATE_CHANGE");
            break;
        case BT_GAP_ENABLE_SUCC:
            GraphicService::GetInstance()->PostGraphicEvent(
                std::bind(&OHOS::SettingBluetoothModel::SettingEnableMsgProcess,
                OHOS::SettingBluetoothModel::GetInstance(), *(bs_protocol_type_t *)(request->data)));
            break;
        case BT_SCAN_COMPLETE:
            scan_msg = (ux_bt_scan_msg_t *)(request->data);
            ReadScanMsg(scan_msg, &scan_device);
            GraphicService::GetInstance()->PostGraphicEvent(
                std::bind(&OHOS::SettingBluetoothModel::SettingScanCompleteMsgProcess,
                OHOS::SettingBluetoothModel::GetInstance(), scan_device, scan_msg->need_to_delete));
            break;
        case BT_A2DP_SRC_STATE_CHANGE:
            a2dp_msg = (ux_bt_a2dp_msg_t*)(request->data);
            GraphicService::GetInstance()->PostGraphicEvent(
                std::bind(&OHOS::SettingBluetoothModel::A2dpSrcStateChangedProcess,
                OHOS::SettingBluetoothModel::GetInstance(), a2dp_msg->bd_addr, a2dp_msg->a2dp_state));
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "uiservice recv BT_A2DP_SRC_STATE_CHANGE");
            break;
        case BT_AVRCP_TG_PLAY_CONTROL:
            play_control_msg = (ux_bt_avrcp_play_control_msg_t*)(request->data);
            GraphicService::GetInstance()->PostGraphicEvent(std::bind(AvrcpTgPlayControlProcess,
                play_control_msg->play_control));
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "uiservice recv BT_AVRCP_TG_PLAY_CONTROL");
            break;
        case BT_AVRCP_TG_VOLUME_CONTROL:
            uint8_t *avrcp_volume = (uint8_t*)(request->data);
            GraphicService::GetInstance()->PostGraphicEvent(std::bind(AvrcpTgVolumeControlProcess, *avrcp_volume));
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "uiservice recv BT_AVRCP_TG_VOLUME_CONTROL");
            break;
    }
}

BOOL ServiceMessageHandle(Service *service, Request *request)
{
    (void)service;
    if (request == nullptr) {
        return FALSE;
    }

    ServiceMsgProcess(request);
    return TRUE;
}

TaskConfig GetServiceTaskConfig(Service *service)
{
    (void)service;
    TaskConfig config = {LEVEL_HIGH, PRI_NORMAL, DATA_DIST_STACK_SIZE, DATA_DIST_MAIL_LENGTH, SINGLE_TASK};
    return config;
}

static int32_t SendMsg(int32_t msgId, uint32_t msgValue, void *data, int32_t dataLen)
{
    int32_t ret;
    void *sendData = nullptr;
    if (dataLen > 0) {
        sendData = malloc(dataLen);
        if (sendData == nullptr) {
            return FALSE;
        }
        ret = memcpy_s(sendData, dataLen, data, dataLen);
        if (ret != EOK) {
            free(sendData);
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "uiservice sendMsg memcpy_s fail! ret = %d", ret);
            return FALSE;
        }
    }

    Request request = {
        .msgId = msgId,
        .len = dataLen,
        .data = sendData,
        .msgValue = msgValue,
    };

    ret = SAMGR_SendRequest(IUNKNOWN_GetIdentity(GET_IUNKNOWN(g_uiService)), &request, nullptr);
    if (ret != 0) {
        if (sendData != nullptr) {
            free(sendData);
        }
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_UI_SERVICE, "SendMsg failed ret = %d ", ret);
        return FALSE;
    }

    return TRUE;
}

void SendMsgToUIService(int32_t msgId, uint32_t msgValue, void *data, int32_t dataLen)
{
    UiServiceFeatureApi *comApi = nullptr;
    IUnknown *iUnknown = SAMGR_GetInstance()->GetDefaultFeatureApi(UI_SERVICE);
    if (iUnknown == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_UI_SERVICE, "GetDefaultFeatureApi failed");
        return;
    }
    int result = iUnknown->QueryInterface(iUnknown, DEFAULT_VERSION, (void **)&comApi);
    if (result != 0 || comApi == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_UI_SERVICE, "QueryInterface comApi failed");
        return;
    }
    comApi->SendMsg(msgId, msgValue, data, dataLen);
    comApi->Release((IUnknown *)comApi);
}

#ifdef __cplusplus
}
#endif
