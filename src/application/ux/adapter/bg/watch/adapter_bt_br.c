/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: Provide ux bg adapter  \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-10-20， Create file. \n
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "securec.h"
#include "bs_br_service_api.h"
#include "ux_bg_config.h"
#include "common_def.h"
#include "ux_common_log.h"
#include "ux_hal_memory.h"
#include "ux_api_bt.h"
#include "adapter_bt_br.h"
#ifdef JS_ENABLE
#include "uiservice/ui_service.h"
#include "broadcast_service.h"
#include "pub_sub_feature.h"
#include "service_id_define.h"
#include "samgr_lite.h"
/* 利用samgr机制发送广播消息，native app订阅后可以接受消息并处理 */
void ux_samgr_event_publish(int16 topic, void* data, uint16_t data_len)
{
    int32 ret;
    PubSubInterface *broadcast_api = NULL;
    IUnknown *api = NULL;

    void *publish_data = ux_hal_memory_acquire(UX_MEM_BT, (size_t)data_len);
    if (publish_data == NULL) {
        return;
    }
    if (memcpy_s(publish_data, (size_t)data_len, data, (unsigned int)data_len) != 0) {
        ux_hal_memory_release(UX_MEM_BT, publish_data);
        publish_data = NULL;
        return;
    }

    Request request = {
        .msgId = topic,
        .len = (int16_t)data_len,
        .msgValue = 0,
        .data = publish_data,
    };

    /* public request */
    api = SAMGR_GetInstance()->GetFeatureApi(BROADCAST_SERVICE, PUB_SUB_FEATURE);
    if (api == NULL) {
        ux_hal_memory_release(UX_MEM_BT, publish_data);
        publish_data = NULL;
        return;
    }
    ret = api->QueryInterface(api, DEFAULT_VERSION, (void **)&broadcast_api);
    if (ret != 0 || broadcast_api == NULL) {
        ux_hal_memory_release(UX_MEM_BT, publish_data);
        publish_data = NULL;
        return;
    }
    broadcast_api->PublishTopic((IUnknown *)broadcast_api, &request);
    broadcast_api->Release((IUnknown *)broadcast_api);
    return;
}
#endif


void ux_adapter_bt_scan_complete_process(const bs_br_discovery_result_t *scan_result, bool need_to_delete)
{
#ifdef JS_ENABLE
    ux_bt_scan_msg_t scan_msg;
    if (strcpy_s((char *)(scan_msg.deviceName), BS_BR_DEVICE_NAME_MAX_LEN, (const char *)scan_result->name) != 0) {
        ux_log_err("ux_adapter_bt_scan_complete_process strcpy_s failed");
        return;
    }
    scan_msg.device_class = scan_result->device_class;
    if (memcpy_s(scan_msg.addr, sizeof(scan_msg.addr), scan_result->addr.addr, sizeof(scan_result->addr.addr)) != 0) {
        ux_log_err("ux_adapter_bt_scan_complete_process memcpy_s failed");
        return;
    }
    scan_msg.rssi = scan_result->rssi;
    scan_msg.need_to_delete = need_to_delete;

    SendMsgToUIService(BT_SCAN_COMPLETE, 0, &scan_msg, sizeof(ux_bt_scan_msg_t));
#endif
}

void ux_adapter_bt_connect_process(int8_t *name, ux_api_bt_addr_t *addr, uint32_t reason,
    uint8_t connect_state, uint32_t device_class)
{
#ifdef JS_ENABLE
    ux_bt_connect_msg_t connect_msg;
    if (strcpy_s((char *)(connect_msg.deviceName), BS_BR_DEVICE_NAME_MAX_LEN, (const char *)name) != 0) {
        ux_log_err("ux_adapter_bt_connect_process strcpy_s failed");
        return;
    }
    if (memcpy_s(&connect_msg.addr, sizeof(ux_api_bt_addr_t), addr, sizeof(ux_api_bt_addr_t)) != 0) {
        ux_log_err("ux_adapter_bt_connect_process memcpy_s failed");
        return;
    }
    connect_msg.reason = reason;
    connect_msg.state = connect_state;
    connect_msg.device_class = device_class;
    ux_samgr_event_publish(TOPIC_EVENT_BT_CONNECTED, &connect_msg, sizeof(ux_bt_connect_msg_t));
#endif
}

void ux_adapter_bt_disconnect_process(int8_t *name, ux_api_bt_addr_t *addr, uint32_t reason,
    uint8_t connect_state, uint32_t device_class)
{
#ifdef JS_ENABLE
    ux_bt_connect_msg_t disconnect_msg;
    if (strcpy_s((char *)(disconnect_msg.deviceName), BS_BR_DEVICE_NAME_MAX_LEN, (const char *)name) != 0) {
        ux_log_err("ux_adapter_bt_disconnect_process strcpy_s failed");
        return;
    }
    if (memcpy_s(&disconnect_msg.addr, sizeof(disconnect_msg.addr), addr, sizeof(ux_api_bt_addr_t)) != 0) {
        ux_log_err("ux_adapter_bt_disconnect_process memcpy_s failed");
        return;
    }
    disconnect_msg.reason = reason;
    disconnect_msg.state = connect_state;
    disconnect_msg.device_class = device_class;
    ux_samgr_event_publish(TOPIC_EVENT_BT_DISCONNECTED, &disconnect_msg, sizeof(ux_bt_connect_msg_t));
#endif
}

void ux_adapter_bt_enable_process(bs_protocol_type_t protocol)
{
#ifdef JS_ENABLE
    SendMsgToUIService(BT_GAP_ENABLE_SUCC, 0, &protocol, sizeof(bs_protocol_type_t));
#endif
}

void ux_adapter_bt_call_state_process(bd_addr_t *bd_addr, uint8_t call_state, char* number,
    uint8_t number_len, bool outgoing)
{
#ifdef JS_ENABLE
    ux_bt_call_msg_t call_msg;
    if (strcpy_s((char *)call_msg.number, sizeof(call_msg.number), number) != 0) {
        ux_log_err("ux_adapter_bt_call_state_process strcpy_s failed");
        return;
    }
    if (memcpy_s(&(call_msg.bd_addr), sizeof(bd_addr_t), bd_addr, sizeof(bd_addr_t)) != 0) {
        ux_log_err("ux_adapter_bt_call_state_process memcpy_s failed");
        return;
    }
    call_msg.state = call_state;
    call_msg.outgoing = outgoing;
    call_msg.number_len = number_len;
    SendMsgToUIService(BT_CALL_STATE_CHANGE, 0, &call_msg, sizeof(ux_bt_call_msg_t));
#endif
}

void ux_adapter_bt_sco_state_process(bd_addr_t *bd_addr, uint8_t sco_state)
{
#ifdef JS_ENABLE
    ux_bt_sco_msg_t sco_msg;
    sco_msg.sco_state = sco_state;
    if (memcpy_s(&(sco_msg.bd_addr), sizeof(bd_addr_t), bd_addr, sizeof(bd_addr_t)) != 0) {
        ux_log_err("ux_adapter_bt_sco_state_process memcpy_s failed");
        return;
    }
    SendMsgToUIService(BT_SCO_STATE_CHANGE, 0, &sco_msg, sizeof(ux_bt_sco_msg_t));
#endif
}

void ux_adapter_bt_hfp_state_process(bd_addr_t *bd_addr, uint8_t hfp_state)
{
#ifdef JS_ENABLE
    ux_bt_hfp_msg_t hfp_msg;
    hfp_msg.hfp_state = hfp_state;
    if (memcpy_s(&(hfp_msg.bd_addr), sizeof(bd_addr_t), bd_addr, sizeof(bd_addr_t)) != 0) {
        ux_log_err("ux_adapter_bt_hfp_state_process memcpy_s failed");
        return;
    }
    SendMsgToUIService(BT_HFP_STATE_CHANGE, 0, &hfp_msg, sizeof(ux_bt_hfp_msg_t));
#endif
}

void ux_adapter_bt_a2dp_src_state_process(ux_api_bt_addr_t *bd_addr, uint8_t a2dp_state)
{
#ifdef JS_ENABLE
    ux_bt_a2dp_msg_t a2dp_state_msg = { 0 };
    a2dp_state_msg.a2dp_state = a2dp_state;
    if (memcpy_s(&(a2dp_state_msg.bd_addr), sizeof(ux_api_bt_addr_t), bd_addr, sizeof(ux_api_bt_addr_t)) != EOK) {
        ux_log_err("ux_adapter_bt_a2dp_src_state_process memcpy_s failed");
        return;
    }
    SendMsgToUIService(BT_A2DP_SRC_STATE_CHANGE, 0, &a2dp_state_msg, sizeof(ux_bt_a2dp_msg_t));
#endif
}

void ux_adapter_bt_avrcp_tg_play_control_process(ux_api_bt_addr_t *bd_addr, ux_api_bt_control_play_t play_control)
{
#ifdef JS_ENABLE
    ux_bt_avrcp_play_control_msg_t msg = { 0 };
    msg.play_control = play_control;
    if (memcpy_s(&(msg.bd_addr), sizeof(bd_addr_t), bd_addr, sizeof(bd_addr_t)) != EOK) {
        ux_log_err("ux_adapter_bt_avrcp_tg_play_control_process memcpy_s failed");
        return;
    }
    SendMsgToUIService(BT_AVRCP_TG_PLAY_CONTROL, 0, &msg, sizeof(ux_bt_avrcp_play_control_msg_t));
#endif
}

void ux_adapter_bt_avrcp_tg_volume_control_process(bd_addr_t *bd_addr, uint8_t avrcp_volume)
{
    unused(bd_addr);
#ifdef JS_ENABLE
    SendMsgToUIService(BT_AVRCP_TG_VOLUME_CONTROL, 0, &avrcp_volume, sizeof(avrcp_volume));
#endif
}