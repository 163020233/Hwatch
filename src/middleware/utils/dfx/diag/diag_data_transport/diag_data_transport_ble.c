/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: diag data transport by BLE
 * This file should be changed only infrequently and with great care.
 */
 
#include "diag_data_transport.h"
#include "dfx_adapt_layer.h"
#include "soc_diag_cmd_id.h"
#include "diag_pkt_router.h"
#include "dfx_channel.h"

static gap_ble_conn_state_t g_ble_conn_state = GAP_BLE_STATE_DISCONNECTED;

void diag_set_ble_conn_state(gap_ble_conn_state_t state)
{
    g_ble_conn_state = state;
}

#if (CONFIG_DFX_DATA_TRANSPORT_BLE == DFX_YES) && (CONFIG_DFX_SUPPORT_DIAG_CONNECT_BLE == DFX_YES)
static bool g_init;

// 回调：初始化diag数据传输ble通道
static errcode_t diag_data_transport_ble_init(void *userdata)
{
    unused(userdata);
    g_init = true;
    return ERRCODE_SUCC;
}


// 回调：反初始化diag数据传输ble通道
static void diag_data_transport_ble_deinit(void)
{
    g_init = false;
}

// 回调：判断ble是否连接
static bool diag_data_transport_ble_is_connected(void)
{
    return g_ble_conn_state == GAP_BLE_STATE_CONNECTED;
}

// 回调：通过ble发送数据
static errcode_t diag_data_transport_ble_data_send(diag_data_item_t *item)
{
    if (!item) {
        return ERRCODE_INVALID_PARAM;
    }
    if (!g_init) {
        return ERRCODE_DIAG_NOT_INIT;
    }

    diag_option_t option_ble;
    errno_t ret;
    uint16_t p_len;
    uint8_t *p_buf;
    option_ble.peer_addr = DIAG_FRAME_FID_BLE_GATT; // 使用BLE通道
    
    if (item->enable_ts) {  // 如果使能时间戳，则在数据前面加上时间戳
        uint32_t raw_len = sizeof(item->ts) + item->len;
        p_len = align_up4(raw_len); // 4字节对齐
        p_buf = (uint8_t *)dfx_malloc(0, p_len);
        if  (p_buf == NULL) {
            return ERRCODE_MALLOC;
        }
        memset_s(p_buf, p_len, 0, p_len);

        ret = memcpy_s(p_buf, p_len, &item->ts, sizeof(item->ts));
        if (ret != EOK) {
            dfx_free(0, p_buf);
            return ERRCODE_MEMCPY;
        }

        ret = memcpy_s(p_buf + sizeof(item->ts), p_len - sizeof(item->ts), item->payload, item->len);
        if (ret != EOK) {
            dfx_free(0, p_buf);
            return ERRCODE_MEMCPY;
        }
    } else {    // 如果不使能时间戳，则直接发送数据
        p_len = item->len;
        p_buf = (uint8_t *)dfx_malloc(0, p_len);
        if (p_buf == NULL) {
            return ERRCODE_MALLOC;
        }

        ret = memcpy_s(p_buf, p_len, item->payload, item->len);
        if (ret != EOK) {
            dfx_free(0, p_buf);
            return ERRCODE_MEMCPY;
        }
    }

    //  使用diag接口通过ble发送数据
    errcode_t err = uapi_diag_report_packet(item->cmd_id, &option_ble, p_buf, p_len, true);
    dfx_free(0, p_buf);
    return err;
}

// 回调：通过ble发送buffer数据
static errcode_t diag_data_transport_ble_data_send_buf(const uint8_t *buf, uint16_t len)
{
    diag_pkt_handle_t pkt;
    diag_pkt_process_param_t process_param = {
        .dst_addr = DIAG_FRAME_FID_BLE_GATT,
        .src_addr = DIAG_FRAME_FID_MCU
    };
    int ret;

    if ((buf == NULL) || (len == 0U)) {
        return ERRCODE_INVALID_PARAM;
    }

    ret = memset_s(&pkt, sizeof(pkt), 0, sizeof(pkt));
    if (ret != EOK) {
        return ERRCODE_MEMSET;
    }
    process_param.cur_proc = DIAG_PKT_PROC_SYNC;
    diag_pkt_handle_init(&pkt, 1);
    diag_pkt_handle_set_data(&pkt, DIAG_PKT_DATA_ID_0, (uint8_t *)buf, len, DIAG_PKT_STACK_DATA);
    return diag_pkt_router(&pkt, &process_param);
}

// diag数据传输ble通道回调配置
diag_data_transport_config_t g_default_diag_data_transport_ble = {
    .callbacks = {
        .init = diag_data_transport_ble_init,
        .deinit = diag_data_transport_ble_deinit,
        .data_send = diag_data_transport_ble_data_send,
        .data_send_buf = diag_data_transport_ble_data_send_buf,
        .is_connected = diag_data_transport_ble_is_connected,
    }
};
#endif