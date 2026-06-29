/* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: broadcast feature implement
 * Author: CompanyName
 * Create: 2021-11-11
 */

#include <cstdio>
#include "securec.h"
#include "adapter.h"
#include "ohos_init.h"
#include "samgr_lite.h"
#include "memory_adapter.h"
#include "wearable_log.h"
#include "service_id_define.h"
#include "broadcast_service.h"
#include "alarm/AlarmClockModel.h"
#include "alarm/AlarmPopUpPresenter.h"
#include "phonemenu/PhoneMenuCallerLogModel.h"
#include "notification_manager.h"
#include "NativeAbility.h"
#include "abilityms_slite_client.h"
#include "element_name.h"
#include "want.h"
#include "msg_center.h"
#include "msg_center_msg.h"
#include "msg_center_protocol.h"
#ifdef FEATURE_MODULE_BAIDU
#include "xiaodu_navi_vector.h"
#include "baidumap.h"
#endif
#include "common_def.h"
#include "keyinput.h"
#include "key/KeyInputListener.h"
#include "main/VolumeSettingView.h"
#include "main/VolumeSettingPresenter.h"
#include "main/VolumeSettingView.h"
#include "adapter_bt_br.h"
#include "cJSON.h"
#include "heartrate/HeartRateModel.h"
#include "phonemenu/PhoneMenuView.h"
#include "msg_center_device.h"

namespace OHOS {

#ifdef __cplusplus
extern "C" {
#endif
#include "adapter_bt_br.h"

/* 订阅事件设定优先级 */
const PriorityMapper PRI_MAPPER[] = {
    {TOPIC_EVENT_ALARM_RING, PRIORITY_TWO},
    {TOPIC_EVENT_NEXTDAY_UPDATE, PRIORITY_TWO},
    {TOPIC_EVENT_HFP_INCOMMING, PRIORITY_ONE},
    {TOPIC_EVENT_HFP_FINISHED, PRIORITY_ONE},
    {TOPIC_EVENT_HFP_OUTGOING, PRIORITY_ONE},
    {TOPIC_EVENT_HFP_PHONE_ACTIVE, PRIORITY_ONE},
    {TOPIC_EVENT_HFP_WATCH_ACTIVE, PRIORITY_ONE},
    {TOPIC_EVENT_HFP_WATCH_TO_PHONE, PRIORITY_ONE},
    {TOPIC_EVENT_HFP_PHONE_TO_WATCH, PRIORITY_ONE},
    {TOPIC_DEMO_ALL_MSG, PRIORITY_TWO},
    {TOPIC_EVENT_STEPDATA_UPDATE, PRIORITY_TWO},
    {TOPIC_EVENT_HEARTRATE_UPDATA, PRIORITY_TWO},
    {TOPIC_EVENT_BLOODOXYGEN_UPDATA, PRIORITY_TWO},
    {TOPIC_EVENT_SPORT_DISTANCE, PRIORITY_TWO},
    {TOPIC_EVENT_CALENDAR_NOTIFY, PRIORITY_TWO},
    {TOPIC_EVENT_BT_CONNECTED, PRIORITY_TWO},
    {TOPIC_EVENT_BT_DISCONNECTED, PRIORITY_TWO},
    {TOPIC_EVENT_TWS_CARD_POP, PRIORITY_TWO},
    {TOPIC_EVENT_TWS_CARD_CONNECT, PRIORITY_TWO},
};
const uint32 devNameLen = 64;
typedef struct {
    char devName[devNameLen];
    uint32 batterylevel;
    uint32 volume;
} TwsEventData;
bool g_pressFlag = false;
bool g_longPressFlag = false;
char twsName[devNameLen];
uint32 twsBattery;
uint32 twsVolume;
CalendarEvent *g_tmpevent = nullptr;
TwsEventData *pEventData;
static bool CheckPriority(const Topic topic)
{
    uint16_t mapperSize = sizeof(PRI_MAPPER) / sizeof(PRI_MAPPER[0]);
    for (uint16_t i = 0; i < mapperSize; i++) {
        if (topic == PRI_MAPPER[i].topicId) {
            if (PRI_MAPPER[i].prId <= OHOS::NativeAbility::GetInstance().GetCurSlicePriority() &&
                PRI_MAPPER[i].prId <= OHOS::NotificationManager::GetInstance()->GetNotifyPriority()) {
                return true;
            } else {
                return false;
            }
        }
    }
    return false;
}

void MsgCenterNotifyConnectProc(const Topic topic, Request* req)
{
    if (req == nullptr) {
        return;
    }
    ux_bt_connect_msg_t *connect_msg = (ux_bt_connect_msg_t *)(req->data);
    bd_addr_t bdAddr;
    (void)memcpy_s((void *)(bdAddr.addr), sizeof(bdAddr.addr),
        (const void *)(connect_msg->addr.addr), sizeof(connect_msg->addr.addr));
    bdAddr.type = (uint8_t)connect_msg->addr.type;
    
    GraphicService::GetInstance()->PostGraphicEvent(
        std::bind(&SettingBluetoothModel::SettingConnectMsgProcess, SettingBluetoothModel::GetInstance(), bdAddr, connect_msg->reason,
            connect_msg->state, connect_msg->device_class));
}

void MsgCenterNotifyDisconnectProc(const Topic topic, Request* req)
{
    ux_bt_connect_msg_t *disconnect_msg = (ux_bt_connect_msg_t *)(req->data);
    bd_addr_t bdAddr;
    (void)memcpy_s((void *)(bdAddr.addr), sizeof(bdAddr.addr), (const void *)(disconnect_msg->addr.addr), sizeof(disconnect_msg->addr.addr));

    GraphicService::GetInstance()->PostGraphicEvent(
        std::bind(&SettingBluetoothModel::SettingDisconnectMsgProcess, SettingBluetoothModel::GetInstance(), bdAddr, disconnect_msg->reason,
            disconnect_msg->state, disconnect_msg->device_class));
}

/* 订阅事件可定制 */
void MsgCenterSubscribe()
{
    MsgCenterSubscribeTopic(TOPIC_EVENT_ALARM_RING);
    MsgCenterSubscribeTopic(TOPIC_EVENT_NEXTDAY_UPDATE);
    MsgCenterSubscribeTopic(TOPIC_EVENT_HFP_INCOMMING);
    MsgCenterSubscribeTopic(TOPIC_EVENT_HFP_PHONE_ACTIVE);
    MsgCenterSubscribeTopic(TOPIC_EVENT_HFP_WATCH_ACTIVE);
    MsgCenterSubscribeTopic(TOPIC_EVENT_HFP_FINISHED);
    MsgCenterSubscribeTopic(TOPIC_EVENT_HFP_OUTGOING);
    MsgCenterSubscribeTopic(TOPIC_EVENT_HFP_PHONE_TO_WATCH);
    MsgCenterSubscribeTopic(TOPIC_EVENT_HFP_WATCH_TO_PHONE);
    MsgCenterSubscribeTopic(TOPIC_EVENT_STEPDATA_UPDATE);
    MsgCenterSubscribeTopic(TOPIC_EVENT_HEARTRATE_UPDATA);
    MsgCenterSubscribeTopic(TOPIC_EVENT_BLOODOXYGEN_UPDATA);
    MsgCenterSubscribeTopic(TOPIC_EVENT_SPORT_DISTANCE);
    MsgCenterSubscribeTopic(TOPIC_EVENT_CALENDAR_NOTIFY);
    MsgCenterSubscribeTopic(TOPIC_EVENT_BT_CONNECTED);
    MsgCenterSubscribeTopic(TOPIC_EVENT_BT_DISCONNECTED);
    MsgCenterSubscribeTopic(TOPIC_EVENT_TWS_CARD_POP);
    MsgCenterSubscribeTopic(TOPIC_EVENT_TWS_CARD_CONNECT);
}

static OHOS::UIViewGroup* ShowAlarmNotify(void)
{
    return OHOS::CreateAlarmView();
}

static OHOS::UIViewGroup* ShowPhoneCallNotify(void)
{
    return OHOS::CreatePhoneCallView();
}

static OHOS::UIViewGroup* ShowPhoneCallNotifyOutGoing(void)
{
    return OHOS::CreatePhoneCallViewOutGoing();
}

static OHOS::UIViewGroup* ShowCallingNotify(void)
{
    return OHOS::CreateCallingView();
}

static OHOS::UIViewGroup* ShowMultiPhoneCallNotify(void)
{
    return OHOS::CreateMultiplePhoneView();
}

static OHOS::UIViewGroup* ShowCalendarNotify(void)
{
    // only show one notify
    if (!g_tmpevent) {
        return nullptr;
    }
    OHOS::UIViewGroup* group = OHOS::CreateCalendarView(g_tmpevent);
    if (g_tmpevent) {
        free(g_tmpevent);
        g_tmpevent = nullptr;
    }
    return group;
}

static OHOS::UIViewGroup* ShowMsgNotify(void)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "ShowMsgNotify");
    return OHOS::PhoneMsgView::CreatePhoneMsgView();
}

static OHOS::UIViewGroup *ShowHeadsetNotify(void)
{
    return OHOS::CreateHeadsetConnView(twsName, twsBattery, twsVolume);
}

static OHOS::UIViewGroup *ShowHeadsetPopNotify(void)
{
    return OHOS::CreateHeadsetPopView(twsName, twsBattery, twsVolume);
}


void MsgCenterNotifyAlarmProc(const Topic topic, Request* req)
{
    unused(req);
    if (CheckPriority(topic)) {
        AlarmClockModel::GetInstance()->PopInterPlayMusic();
        NotificationManager::GetInstance()->SetNotifyPriority(PRIORITY_TWO);
        (void)NotificationManager::GetInstance()->ShowNotify(ShowAlarmNotify, static_cast<uint16_t>(topic), true);
    } else {
        // just delay the alarm
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "just delay the alarm");
        AlarmClockModel::GetInstance()->AlarmGetClockNum();
        AlarmPopUpPresenter::GetInstance()->DelayAlarmRing();
    }
}

void MsgCenterNotifyCalendarProc(const Topic topic, Request* req)
{
    if (g_tmpevent != nullptr) {
        free(g_tmpevent);
        g_tmpevent = nullptr;
    }
    
    g_tmpevent = (CalendarEvent *)malloc(sizeof(CalendarEvent));
    if (g_tmpevent == nullptr) {
        return;
    }
    memcpy_s(g_tmpevent, sizeof(CalendarEvent), req->data, sizeof(CalendarEvent));
    // The alarm notification is not displayed if the incoming call notification is already being processed
    if (CheckPriority(topic)) {
        NotificationManager::GetInstance()->SetNotifyPriority(PRIORITY_TWO);
        (void)NotificationManager::GetInstance()->ShowNotify(ShowCalendarNotify, static_cast<uint16_t>(topic), true);
    }
}

void MsgCenterNotifyNextDayProc(const Topic topic, Request* req)
{
    unused(topic);
    unused(req);
    AlarmClockModel::GetInstance()->NextDayNotify();
}

void MsgCenterNotifyHfpIncomingProc(const Topic topic, Request* req)
{
    PhoneHfpCallInfo callData = {
    .bd_addr = { .addr = {0}, .type = 0 },
    .id = 0,
    .state = 0,
    .number = {0},
    .number_len = 0,
    .uuid = { .uuid_len = 0, .uuid = {0} },
    .multi_party = false,
    .outgoing = false,
    .in_band_ring = false,
    .creation_time = 0
    };
    int32 ret = memcpy_s(&callData, sizeof(PhoneHfpCallInfo), req->data, req->len);
    if (ret != EOK) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "MsgCenterNotify memcpy_s request data fail!");
    } else {
    // Prevent this from happening:
    // When the incoming call view is displayed, the calling number is not set in CallerLogModel.
        PhoneMenuCallerLogModel::GetInstance()->SetoperateNumber(callData.number, callData.number_len);
    }
    NotificationManager::GetInstance()->SetNotifyPriority(PRIORITY_ONE);
    PhoneMenuCallerLogModel::GetInstance()->SetNotifyFlag(true);
    if (GetCurrCallNum() == 1) {
        (void)NotificationManager::GetInstance()->ShowNotify(ShowPhoneCallNotify, static_cast<uint16_t>(topic), true);
    } else {
        EnableWaitingView();
    }
    NotificationManager::GetInstance()->SetWatchCallStatus(true);
}

void MsgCenterNotifyHfpOutGoingProc(const Topic topic, Request* req)
{
    PhoneHfpCallInfo callData = {
    .bd_addr = { .addr = {0}, .type = 0 },
    .id = 0,
    .state = 0,
    .number = {0},
    .number_len = 0,
    .uuid = { .uuid_len = 0, .uuid = {0} },
    .multi_party = false,
    .outgoing = false,
    .in_band_ring = false,
    .creation_time = 0
    };
    int32 ret = memcpy_s(&callData, sizeof(PhoneHfpCallInfo), req->data, req->len);
    if (ret != EOK) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "MsgCenterNotify memcpy_s request data fail!");
    } else {
    // Prevent this from happening:
    // When the incoming call view is displayed, the calling number is not set in CallerLogModel.
        PhoneMenuCallerLogModel::GetInstance()->SetoperateNumber(callData.number, callData.number_len);
    }
    NotificationManager::GetInstance()->SetNotifyPriority(PRIORITY_ONE);
    PhoneMenuCallerLogModel::GetInstance()->SetNotifyFlag(true);
#ifndef SUPPORT_BIKE
    if (GetCurrCallNum() == 0) {
        (void)NotificationManager::GetInstance()->ShowNotify(ShowPhoneCallNotifyOutGoing, static_cast<uint16_t>(topic),
            true);
    } else {
        EnableWaitingView();
    }
    NotificationManager::GetInstance()->SetWatchCallStatus(true);
#else
    MotorCycleModel::GetInstance().MsgCalling(true);
#endif
}

void MsgCenterNotifyHfpPhoneProc(const Topic topic, Request* req)
{
    unused(req);
    // finish a single-channel call, or switch to phone, stop the notify
    if (PhoneMenuCallerLogModel::GetInstance()->GetNotifyFlag()) {
        DisableWatingView();
        if ((topic == TOPIC_EVENT_HFP_FINISHED && GetCurrCallNum() == 0) ||
        topic == TOPIC_EVENT_HFP_PHONE_ACTIVE || topic == TOPIC_EVENT_HFP_WATCH_TO_PHONE) {
            NotificationManager::GetInstance()->StopNotify();
            NotificationManager::GetInstance()->SetWatchCallStatus(false);
        }
    }
}

void MsgCenterNotifyHfpWatchProc(const Topic topic, Request* req)
{
    unused(req);
    if (PhoneMenuCallerLogModel::GetInstance()->GetNotifyFlag()) {
        NotificationManager::GetInstance()->SetNotifyPriority(PRIORITY_ONE);
        // default not mute
        SetMicMute(false);
        if (PhoneMenuCallerLogModel::GetInstance()->IsMulitplePhoneStatus()) {
            (void)NotificationManager::GetInstance()->ShowNotify(ShowMultiPhoneCallNotify, static_cast<uint16_t>(topic),
                true);
        } else {
            (void)NotificationManager::GetInstance()->ShowNotify(ShowCallingNotify, static_cast<uint16_t>(topic), true);
        }
        NotificationManager::GetInstance()->SetWatchCallStatus(true);
    }
}

void MsgCenterNotifyMsgWatchProc(const Topic topic, Request* req)
{
    std::string msgContent((char *)(req->data),  req->len);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "MsgCenterNotifyMsgWatchProc msgContent %s\r\n", msgContent.c_str());
    cJSON *msgJson = cJSON_Parse(msgContent.c_str());
    if (msgJson == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "phone msg_result cJSON_Parse, fail");
        return;
    }
    cJSON *appNameJson = cJSON_GetObjectItem(msgJson, "appName");
    if (appNameJson == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Missing 'appName' field in JSON");
        cJSON_Delete(msgJson);
        return;
    }

    cJSON *packageNameJson =  cJSON_GetObjectItem(msgJson, "packageName");
    if (packageNameJson == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Missing 'packageName' field in JSON");
        cJSON_Delete(msgJson);
        return;
    }

    cJSON *titleJson = cJSON_GetObjectItem(msgJson, "title");
    if (titleJson == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Missing 'title' field in JSON");
        cJSON_Delete(msgJson);
        return;
    }

    cJSON *contentJson = cJSON_GetObjectItem(msgJson, "content");
    if (contentJson == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Missing 'content' field in JSON");
        cJSON_Delete(msgJson);
        return;
    }

    char *appName = appNameJson->valuestring;
    char *packageName = packageNameJson->valuestring;
    char *title = titleJson->valuestring;
    char *content = contentJson->valuestring;
    if (appName == NULL || packageName == NULL || title == NULL || content == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Invalid string values in JSON");
        cJSON_Delete(msgJson);
        return;
    }
    
    PhoneMsgView::UpDatePhoneMsg(appName, title, content);
    if (CheckPriority(topic)) {
        NotificationManager::GetInstance()->SetNotifyPriority(PRIORITY_TWO);
        (void)NotificationManager::GetInstance()->ShowNotify(ShowMsgNotify, static_cast<uint16_t>(topic), true);
    }

    cJSON_Delete(msgJson);
}

void MsgCenterNotifyHeartRateProc(const Topic topic, Request* req)
{
    unused(topic);
    if (req && req->data) {
        HeartDatas* heart_data = (HeartDatas*)req->data;
        msg_center_set_heartrate(heart_data->perSec);
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "MsgCenterNotifyHeartRateProc per heartrate: %d\n", msg_center_get_heartrate());
    }
}

void MsgCenterNotifyStepDataProc(const Topic topic, Request* req)
{
    unused(topic);
    uint32_t step = 0;
    if (req && req->data) {
        step = *(uint32_t*)req->data;
        msg_center_set_stepdata(msg_center_get_stepdata() + step);
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "MsgCenterNotifyStepDataProc step:%d, totalStep:%d\n", step, msg_center_get_stepdata());
    }
}

void MsgCenterNotifyBloodOxygenProc(const Topic topic, Request* req)
{
    unused(topic);
    if (req && req->data) {
        msg_center_set_bloodoxygen(*(uint8_t*)req->data);
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "MsgCenterNotifyBloodOxygenProc bloodoxygen:%d\n", msg_center_get_bloodoxygen());
    }
}

void MsgCenterNotifyDistanceProc(const Topic topic, Request* req)
{
    unused(topic);
    uint32_t distance = 0;
    if (req && req->data) {
        distance = *(uint32_t*)req->data;
        msg_center_set_distance(msg_center_get_distance() + distance);
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "MsgCenterNotifyDistanceProc distance:%d\n", msg_center_get_distance());
    }
}

void MsgCenterNotifyHeadsetProc(const Topic topic, Request *req)
{
    pEventData = (TwsEventData *)req->data;
    twsBattery = pEventData->batterylevel;
    twsVolume = pEventData->volume;
    memset_s(twsName, devNameLen, 0, devNameLen);
    memcpy_s(twsName, devNameLen, pEventData->devName, strlen(pEventData->devName));
    NotificationManager::GetInstance()->SetNotifyPriority(PRIORITY_TWO);
    (void)NotificationManager::GetInstance()->ShowNotify(ShowHeadsetPopNotify, static_cast<uint16_t>(topic), true);
}

void MsgCenterNotifyHeadsetPopProc(const Topic topic, Request *req)
{
    pEventData = (TwsEventData *)req->data;
    twsBattery = pEventData->batterylevel;
    twsVolume = pEventData->volume;
    memset_s(twsName, devNameLen, 0, devNameLen);
    memcpy_s(twsName, devNameLen, pEventData->devName, strlen(pEventData->devName));
    NotificationManager::GetInstance()->SetNotifyPriority(PRIORITY_TWO);
    (void)NotificationManager::GetInstance()->ShowNotify(ShowHeadsetNotify, static_cast<uint16_t>(topic), true);
}

/* 订阅事件通知逻辑可定制 */
static MsgCenterNotifyTable g_msgCenterNotifyTable[] = {
    {TOPIC_EVENT_ALARM_RING,         MsgCenterNotifyAlarmProc},
    {TOPIC_EVENT_NEXTDAY_UPDATE,     MsgCenterNotifyNextDayProc},
    {TOPIC_EVENT_HFP_INCOMMING,      MsgCenterNotifyHfpIncomingProc},
    {TOPIC_EVENT_HFP_OUTGOING,       MsgCenterNotifyHfpOutGoingProc},
    {TOPIC_EVENT_HFP_PHONE_ACTIVE,   MsgCenterNotifyHfpPhoneProc},
    {TOPIC_EVENT_HFP_FINISHED,       MsgCenterNotifyHfpPhoneProc},
    {TOPIC_EVENT_HFP_WATCH_TO_PHONE, MsgCenterNotifyHfpPhoneProc},
    {TOPIC_EVENT_HFP_WATCH_ACTIVE,   MsgCenterNotifyHfpWatchProc},
    {TOPIC_EVENT_HFP_PHONE_TO_WATCH, MsgCenterNotifyHfpWatchProc},
    {TOPIC_DEMO_ALL_MSG,             MsgCenterNotifyMsgWatchProc},
    {TOPIC_EVENT_HEARTRATE_UPDATA,   MsgCenterNotifyHeartRateProc},
    {TOPIC_EVENT_STEPDATA_UPDATE,    MsgCenterNotifyStepDataProc},
    {TOPIC_EVENT_BLOODOXYGEN_UPDATA, MsgCenterNotifyBloodOxygenProc},
    {TOPIC_EVENT_SPORT_DISTANCE,     MsgCenterNotifyDistanceProc},
    {TOPIC_EVENT_CALENDAR_NOTIFY,    MsgCenterNotifyCalendarProc},
    {TOPIC_EVENT_BT_CONNECTED,       MsgCenterNotifyConnectProc},
    {TOPIC_EVENT_BT_DISCONNECTED,    MsgCenterNotifyDisconnectProc},
    {TOPIC_EVENT_TWS_CARD_POP, MsgCenterNotifyHeadsetPopProc},
    {TOPIC_EVENT_TWS_CARD_CONNECT, MsgCenterNotifyHeadsetProc},
};

static MsgCenterNotifyTable *MsgCenterGetNotifyTblCb(uint8_t idx)
{
    return &g_msgCenterNotifyTable[idx];
}

static uint8_t MsgCenterGetNotifyTblSize(void)
{
    return sizeof(g_msgCenterNotifyTable) / sizeof(g_msgCenterNotifyTable[0]);
}

static void CallVolumeDipSwitchControl(uint16_t notifyId, uint8_t states)
{
    VolumeSettingPresenter *volumeSliderEventListener = nullptr;
    if (notifyId == TOPIC_EVENT_HFP_WATCH_ACTIVE ||
            notifyId == TOPIC_EVENT_HFP_PHONE_TO_WATCH) {
        if (NotificationManager::GetInstance()->GetVolumeScreenStatus()) {
            VolumeSettingView *settingView = GetvolumeSetting();
            if (settingView == nullptr) {
                return;
            }
            volumeSliderEventListener = settingView->GetVolumeEventListener();
            if (volumeSliderEventListener == nullptr) {
                return;
            }
            volumeSliderEventListener->ChangeVolume(static_cast<VolumeChangeStatus>(states));
        } else {
            HonrsViewChange();
            NotificationManager::GetInstance()->SetVolumeScreenStatus(true);
        }
    }
    return;
}

static void OnKeyLongPress(uint16_t keyID)
{
    g_longPressFlag = true; // Long press the logo
    if (keyID == static_cast<uint16_t>(ZliteKeyCode::ZLITE_KEY_POWER)) { // Power button
        // After StopNotify, a restart card will pop up first
        uint16_t notifyId = NotificationManager::GetInstance()->GetShowingNotifyId();
        if (notifyId == TOPIC_EVENT_ALARM_RING) {
            NativeAbility::GetInstance().ChangeSlice(VIEW_REBOOT);
            NotificationManager::GetInstance()->SetNativeUIRunning(true);
            NotificationManager::GetInstance()->StopNotify();
        }
    } else if (keyID == static_cast<uint16_t>(ZliteKeyCode::ZLITE_KEY_FUNC)) { // Function buttons
        // Switch to user configured shortcut card
    }
    return;
}

static void HandleKeyPress(uint16_t notifyId)
{
    if (notifyId == TOPIC_EVENT_ALARM_RING) {
        AlarmClockModel::GetInstance()->AlarmGetClockNum();
        AlarmPopUpPresenter::GetInstance()->DelayAlarmRing();
        NotificationManager::GetInstance()->StopNotify();
    }
}

static bool PressKeyEvent(uint16_t keyID, uint8_t state)
{
    switch (state) {
        case InputDevice::STATE_PRESS:
            g_pressFlag = true;
            return true;
        case InputDevice::STATE_LONG_PRESS:
            OnKeyLongPress(keyID);
            return true;
        case InputDevice::STATE_RELEASE:
            if (g_longPressFlag) {
                g_pressFlag = false;
                g_longPressFlag = false;
                return true;
            }
            break;
        default:
            break;
    }
    return false;
}

static bool KeyNotifyCallback(uint16_t keyID, uint8_t state)
{
    if (!OHOS::NotificationManager::GetInstance()->HasNotifyShowing()) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "Notify does not exist!");
        OHOS::ACELite::KeyInputModule::SetNotifyKeyListener(nullptr);
        g_pressFlag = false;
        g_longPressFlag = false;
        return false;
    }
    if (PressKeyEvent(keyID, state)) {
        return true;
    }
    uint16_t notifyId = NotificationManager::GetInstance()->GetShowingNotifyId();
    if (keyID == static_cast<uint16_t>(ZliteKeyCode::ZLITE_KEY_POWER)) { // Power button
        if (NotificationManager::GetInstance()->GetWatchCallStatus()) { // Volume card
            // volume+
            CallVolumeDipSwitchControl(notifyId, VOLUMEUP);
        } else {
            HandleKeyPress(notifyId);
        }
    }
    else if (keyID == static_cast<uint16_t>(ZliteKeyCode::ZLITE_KEY_FUNC)) { // Function buttons
        if (NotificationManager::GetInstance()->GetWatchCallStatus()) { // Volume card
            // volume-
            CallVolumeDipSwitchControl(notifyId, VOLUMEDOWN);
        } else {
            // Switch to user configured shortcut card
        }
    }
    g_pressFlag = false;
    g_longPressFlag = false;
    return true;
}

bool MsgCenterNotifyProc(const Topic topic, Request* req)
{
    uint8_t i;
    uint8_t tblSize = MsgCenterGetNotifyTblSize();
    OHOS::ACELite::KeyInputModule::SetNotifyKeyListener(KeyNotifyCallback);
    for (i = 0; i < tblSize; i++) {
        MsgCenterNotifyTable *item = MsgCenterGetNotifyTblCb(i);
        if (item != nullptr && item->topic == topic && item->cb != nullptr) {
            item->cb(topic, req);
            return true;
        }
    }

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "MsgCenterNotifyProc:: Can not find the cb!");
    return false;
}

#ifdef JS_ENABLE
void StartJsApp(char *bundleName)
{
    if (bundleName == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "bundleName is null");
        return;
    }
    ElementName* topAbility = OHOS::AbilitySlite::AbilityMsClient::GetInstance().GetTopAbility();
    if (topAbility == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "topAbility is null");
        return;
    }

    if (strcmp(topAbility->bundleName, bundleName) == 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "%s is active", bundleName);
        FreeElement(topAbility);
        return;
    }
    FreeElement(topAbility);
    // If the alarm notification is being processed, we need to disable the alarm notification first.
    if (OHOS::NotificationManager::GetInstance()->GetNotifyPriority() == PRIORITY_TWO &&
        OHOS::NotificationManager::GetInstance()->HasNotifyShowing()) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "Alarm notification needs to be disabled first.");
        AlarmClockModel::GetInstance()->AlarmGetClockNum();
        OHOS::AlarmPopUpPresenter::GetInstance()->DelayAlarmRing();
        OHOS::NotificationManager::GetInstance()->StopNotify();
    }
    Want *want = new Want();
    memset_s(want, sizeof(Want), 0, sizeof(Want));

    ElementName startElement = {};
    startElement.abilityName = (char *)"default";
    startElement.bundleName = bundleName;
    startElement.deviceId = nullptr;
    SetWantElement(want, startElement);

    StartAbility(want);
    ClearWant(want);
    delete want;
}

void StopJsApp(char *bundleName)
{
    ElementName* topAbility = OHOS::AbilitySlite::AbilityMsClient::GetInstance().GetTopAbility();
    if (topAbility == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "topAbility is null");
        return;
    }

    if (strcmp(topAbility->bundleName, bundleName) != 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "%s is not top active", bundleName);
        FreeElement(topAbility);
        return;
    }
    FreeElement(topAbility);
    int token = 0;
    OHOS::AbilitySlite::AbilityMsClient::GetInstance().TerminateAbility(token);
}

void MsgCenterReportBtStatusToJS(uint16_t msgId)
{
    ElementName *elementName = OHOS::AbilitySlite::AbilityMsClient::GetInstance().GetTopAbility();
    if (!(elementName != nullptr && strcmp(elementName->bundleName, "com.ohos.launcher") != 0)) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "JS is not on top, ignore report");
        FreeElement(elementName);
        return;
    }
    FreeElement(elementName);
    if (msgId == BT_DISCONNECTED_TO_MSGCENTER) {
#ifdef FEATURE_MODULE_BAIDU
        BaidumapSendMsgToJS("0", MSGCENTER_TYPE_ID_VECTOR_NAVI_BT_DISCONNECTED);
#endif
    } else if (msgId == BT_CONNECTED_TO_MSGCENTER) {
#ifdef FEATURE_MODULE_BAIDU
        BaidumapSendMsgToJS("1", MSGCENTER_TYPE_ID_VECTOR_NAVI_BT_CONNECTED);
#endif
    } else {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "unexpected msgId %d", msgId);
    }
}
#endif

#ifdef __cplusplus
}
#endif
}
