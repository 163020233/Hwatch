/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: dfx channel
 * This file should be changed only infrequently and with great care.
 */

#include <stdint.h>
#include "errcode.h"
#include "dfx_adapt_layer.h"
#include "diag.h"
#include "diag_ind_src.h"
#include "diag_channel.h"
#include "diag_adapt_layer.h"
#include "log_uart.h"
#include "uart.h"
#ifdef JS_ENABLE
#include "settings/common/SettingBleChange.h"
#endif
#include "soc_diag_cmd_id.h"
#include "diag_msg.h"
#include "debug_print.h"
#include "bts_br_gap.h"
#include "dfx_channel.h"
#if ENABLE_LOW_POWER == YES
#include "pm_definition.h"
#endif
#include "bs_ble_gatt_server.h"
#include "bs_br_service_port_api.h"
#include "bts_le_gap.h"
#define DFX_GATT_CHARAC_TYPE 0
#define DFX_GATT_DESC_TYPE 1

#define DFX_SPP_SERVICE_SLEEP_TIME 5
#define DFX_GATT_SERVICE_WAITE_TIME 5000
#define DFX_SPP_SERVICE_CYCLE_COUNT 100

#define DFX_GATT_APP_UUID { 0x11, 0x22 }
#define DFX_GATT_SERVER_UUID { 0xFF, 0x02 }
#define DFX_GATT_SERVICE_UUID { 0xFF, 0x00 }
#define DFX_GATT_DAIG_CHARACTER_UUID { 0xFF, 0x01 }
#define DFX_GATT_DAIG_DESCRIPT_UUID { 0x29, 0x02 }
#define DFX_GATT_UUID_LEN_2_BYTE 2
#define DFX_GATT_DEFAULT_CCC_VAL { 0x01, 0x00 }

#ifdef SUPPORT_BREDR
typedef void (*spp_receive_data_callback)(bd_addr_t *bd_addr, uint16_t port, uint8_t *data, uint32_t data_len);
typedef void (*spp_conn_state_changed_callback)(bd_addr_t *bd_addr, uint16_t port, uint8_t state);
typedef void (*spp_receive_connection_request_callback)(bd_addr_t *bd_addr, uint16_t port);

typedef struct {
    spp_receive_data_callback receive_data_cb;
    spp_conn_state_changed_callback conn_state_changed_cb;
    spp_receive_connection_request_callback receive_conn_req_cb;
} spp_callbacks_t;

errcode_t spp_write(uint16_t port, const uint8_t *data, const uint32_t len);
errcode_t spp_register_callbacks_multi(uint8_t app_uid, spp_callbacks_t *func);
#endif

typedef  enum {
    DFX_GATT_SERVICE_DIAG_CHARACTER = 0,
    DFX_GATT_SERVICE_DIAG_DESCRIPT,
    DFX_GATT_SERVICE_CHARA_DESC_MAX,
} GattDefaultServiceCharaDescId;

typedef struct {
    bt_uuid_t uuid;                              /*!< @if Eng uuid of this service.
                                                      @else   该服务uuid。 @endif */
    uint8_t is_primary;                          /*!< @if Eng is primary service tag
                                                      @else   是否是主服务标记 */
    uint8_t attribute_count;                     /*!< @if Eng size of attribue_array.
                                                      @else   attribue_array 中存储的属性个数 */
    bs_gatt_service_callbacks_t cbks;            /*!< @if Eng server callback function.
                                                      @else   服务端回调函数 @endif */
    bs_gatt_service_extend_param_t extend_param; /*!< @if Eng server structure extend param.
                                                      @else   服务端扩展参数 @endif */
    bs_gatts_attribute_info attribue_array[DFX_GATT_SERVICE_CHARA_DESC_MAX]; /*!< @if Eng point array of
                                                                        attribue config.
                                                                        @else   特征/描述符配置数组。 @endif */
} dfx_gatt_service_template_t;

#define DIAG_SER_DATA_MFS        990
#define DIAG_SER_GATT_DATA_MFS   500
#if defined(SUPPORT_ALIPAY_SEC) || defined(SUPPORT_ALIPAY_HSC32I1)
#define DIAG_SER_DATA_MAX_LEN    0x3000
#else
#define DIAG_SER_DATA_MAX_LEN    0x800
#endif

static uint16_t g_diag_gatt_hdl = 0;
static uint8_t g_diag_gatt_server_id = 0;
static uint16_t g_diag_gatt_conn_id = 0;
diag_channel_id_t g_diag_channel_id = DIAG_CHANNEL_ID_INVALID;
static uart_bus_t g_diag_uart_id = UART_BUS_1;
static osal_mutex g_diag_uart_out_mutex = {NULL};

#ifdef JS_ENABLE
static errcode_t diag_gatt_transmit_result_hook(errcode_t result, uintptr_t usr_data);
static errcode_t diag_gatt_transmit_start_hook(uint16_t channel_id, uintptr_t usr_data);

static uint8_t g_diag_gatt_inst_id = 0;
uint16_t g_diag_gatt_val = 0;
uint8_t g_ccc_gatt_val[] = DFX_GATT_DEFAULT_CCC_VAL;

dfx_gatt_service_template_t g_dfx_gatt_service = {
    .uuid = { DFX_GATT_UUID_LEN_2_BYTE, DFX_GATT_SERVICE_UUID },
    .is_primary = 1,
    .attribute_count = DFX_GATT_SERVICE_CHARA_DESC_MAX,
    .cbks.write_request_cb  = diag_gatt_write_req_cbk,
    .attribue_array = {
        [(uint8_t)DFX_GATT_SERVICE_DIAG_CHARACTER] = {
            .type = DFX_GATT_CHARAC_TYPE,
            .character = {
                .uuid = { DFX_GATT_UUID_LEN_2_BYTE, DFX_GATT_DAIG_CHARACTER_UUID },
                .permissions = BS_GATT_ATTRIBUTE_PERMISSION_READ | BS_GATT_ATTRIBUTE_PERMISSION_WRITE,
                .properties = BS_GATT_CHARACTER_PROPERTY_BIT_NOTIFY | BS_GATT_CHARACTER_PROPERTY_BIT_WRITE_NO_RSP |
                            BS_GATT_CHARACTER_PROPERTY_BIT_READ,
                .value_len = (uint16_t)sizeof(g_diag_gatt_val),
                .value = (uint8_t *)(&g_diag_gatt_val),
            },
        },
        [(uint8_t)DFX_GATT_SERVICE_DIAG_DESCRIPT] = {
            .type = DFX_GATT_DESC_TYPE,
            .descriptor = {
                .uuid = { DFX_GATT_UUID_LEN_2_BYTE, DFX_GATT_DAIG_DESCRIPT_UUID },
                .permissions = BS_GATT_ATTRIBUTE_PERMISSION_READ | BS_GATT_ATTRIBUTE_PERMISSION_WRITE,
                .value_len = (uint16_t)sizeof(g_ccc_gatt_val),
                .value = g_ccc_gatt_val,
            },
        },
    },
};

static transmit_callback_t g_dfx_gatt_cb = {
    .result_hook = diag_gatt_transmit_result_hook,
    .start_hook = diag_gatt_transmit_start_hook,
};
#endif

static uint8_t *diag_channel_merge_data(uint8_t *data[], uint16_t len[], uint8_t cnt, uint32_t *merged_len)
{
    uint8_t *merge_data = NULL;
    uint32_t copied_size = 0;
    uint32_t send_len = 0;

    for (uint32_t i = 0; i < cnt; i++) {
        send_len += len[i];
    }
    merge_data = (uint8_t *)dfx_malloc(0, send_len);
    if (merge_data == NULL) {
        return NULL;
    }
    memset_s(merge_data, send_len, 0, send_len);
    for (uint32_t i = 0; i < cnt; i++) {
        if (send_len < copied_size) {
            break;
        }
        if (len[i] == 0) {
            continue;
        }
        if (memcpy_s(merge_data + copied_size, send_len - copied_size, data[i], len[i]) != EOK) {
            dfx_free(0, merge_data);
            return NULL;
        }
        copied_size += len[i];
    }
    *merged_len = send_len;
    return merge_data;
}

#if (CONFIG_DFX_SUPPORT_DIAG_CONNECT_BLE == DFX_YES)
#ifdef SUPPORT_BREDR
uint8_t g_spp_client_id;
static errcode_t diag_channel_rx_bt_data(uint8_t *data, uint16_t size)
{
    (void)uapi_diag_channel_rx_mux_char_data(DIAG_CHANNEL_ID_1, data, size);
    return ERRCODE_SUCC;
}

static inline void process_bt_data(uint8_t *data, uint32_t data_len)
{
    (void)diag_channel_rx_bt_data(data, (uint16_t)data_len);
}
// 不同配置下的回调函数
#ifdef CONFIG_SUPPORT_UX_SPP
static void spp_recv_data_cb(uint16_t dev_id, uint16_t port, uint8_t *data, uint16_t data_len)
{
    unused(dev_id);
    unused(port);
    process_bt_data(data, data_len);
}
static void receive_connection_request_cb(uint16_t dev_id, uint16_t port)
{
    PRINT("diag spp port receive connect req port = %d\n", port);
    unused(dev_id);
    bs_br_port_send_conn_rsp(port, true);
}
#else
static void spp_recv_data_cb(bd_addr_t *bd_addr, uint16_t port, uint8_t *data, uint32_t data_len)
{
    g_spp_client_id = port;
    unused(bd_addr);
    process_bt_data(data, data_len);
}
#endif

static void conn_state_changed_cb(bd_addr_t *bd_addr, uint16_t port, uint8_t state)
{
    unused(bd_addr);
    unused(state);
    g_spp_client_id = port;
#if defined (CONFIG_DFX_CLOUD_REPORT)
    diag_set_report_ctrl(false);
    diag_set_bt_conn_state(state);
#endif
    PRINT("diag port conn_state_changed_cb state = %d\n", state);
}
#endif

static errcode_t diag_channel_bt_output(void *fd, dfx_data_type_t data_type, uint8_t *data[], uint16_t len[],
                                        uint8_t cnt)
{
    uint32_t i;

    unused(fd);
    unused(data_type);

    if (cnt == 0 || data[0] == NULL || len[0] == 0) {
        return ERRCODE_INVALID_PARAM;
    }

#ifdef SUPPORT_BREDR
    uint8_t *send_data = NULL;
    uint32_t send_len = 0;
    // cnt > 1时，使用 diag_channel_merge_data 拼接消息
    send_data = cnt > 1 ? diag_channel_merge_data(data, len, cnt, &send_len) : data[0];
    send_len = cnt > 1 ? send_len : len[0];
    if (send_data == NULL) {
        return ERRCODE_MALLOC;
    }

    for (i = 0; i < DFX_SPP_SERVICE_CYCLE_COUNT; i++) {
#ifdef CONFIG_SUPPORT_UX_SPP
        if (bs_br_port_write_data(0, g_spp_client_id, send_data, send_len) == 0) {
#else
        if (spp_write((int)g_spp_client_id, (const uint8_t*)send_data, (uint16_t)send_len) == 0) {
#endif
            break;
        }

        osal_msleep(DFX_SPP_SERVICE_SLEEP_TIME);
    }

    if (cnt > 1) {
        dfx_free(0, send_data);
    }

    if (i == DFX_SPP_SERVICE_CYCLE_COUNT - 1) {
        PRINT("spp_service_send_data error!\n");
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
#else
    unused(i);
    return ERRCODE_SUCC;
#endif
}
#endif

#if (CONFIG_DFX_SUPPORT_DIAG_CONNECT_BLE == DFX_YES)
void diag_channel_register_bt_callback(void)
{
#ifdef SUPPORT_BREDR
#ifndef CONFIG_SUPPORT_UX_SPP
    spp_callbacks_t callbacks = {
        spp_recv_data_cb,
        conn_state_changed_cb,
        NULL,
    };
    spp_register_callbacks_multi(1, &callbacks);
#endif
#endif
}

#ifdef CONFIG_SUPPORT_UX_SPP
void diag_channel_register_br_port_callback(bt_uuid_t *uuid, uint16_t diag_port)
{
    PRINT("diag spp port creat success, port = %d\n", diag_port);
    g_spp_client_id = diag_port;
    bs_br_port_callbacks_t ux_spp_service_callback_funs = {
        .conn_req_cbk = receive_connection_request_cb,
        .receive_data_cbk = spp_recv_data_cb,
        .conn_state_cbk = conn_state_changed_cb,
    };
    bs_br_port_register_callbacks(uuid, &ux_spp_service_callback_funs);
}
#endif
#endif

#ifndef JS_ENABLE
void diag_gatt_set_handle(uint8_t server_id, uint16_t conn_id, uint16_t handle)
{
    g_diag_gatt_server_id = server_id;
    g_diag_gatt_conn_id = conn_id;
    g_diag_gatt_hdl = handle;
}

void diag_gatt_write_req_cbk(uint8_t server_id, uint16_t conn_id, gatts_req_write_cb_t *write_cb_para,
    errcode_t status)
{
    unused(status);

    diag_gatt_set_handle(server_id, conn_id, write_cb_para->handle);
    (void)uapi_diag_channel_rx_mux_char_data(DIAG_CHANNEL_ID_2, write_cb_para->value, write_cb_para->length);
#if defined (CONFIG_DFX_CLOUD_REPORT)
    if ((write_cb_para->length == 2) && (*(char*)(write_cb_para->value) == 1)) { /* 2:data length */
        diag_set_ble_conn_state(GAP_BLE_STATE_CONNECTED);
        diag_set_report_ctrl(false);
    }
#endif
}

void diag_channel_register_bt_gatt_init(uint32_t service_uuid)
{
#ifndef SUPPORT_OHOSFWK
#define SERVICE_UUID_OFFSET 8
    uint8_t server_id;
    errcode_t ret;
    bt_uuid_t app_uuid = { DFX_GATT_UUID_LEN_2_BYTE, DFX_GATT_APP_UUID };
    bt_uuid_t service_id;
    gatts_callbacks_t cb = {0};

    cb.write_request_cb = diag_gatt_write_req_cbk;

    service_id.uuid_len = DFX_GATT_UUID_LEN_2_BYTE;
    service_id.uuid[0] = (uint8_t)(service_uuid >> SERVICE_UUID_OFFSET);
    service_id.uuid[1] = (uint8_t)service_uuid;

    ble_set_gatt_waitfor_time(DFX_GATT_SERVICE_WAITE_TIME);
    ret = gatts_register_server(&app_uuid, &server_id);
    if (ret != ERRCODE_BT_SUCCESS) {
        PRINT("gatts_register_server error ret: %x\n", ret);
        return;
    }

    PRINT("server_id = %d\n", server_id);

    (void)gatts_register_multi_callbacks(server_id, &service_id, &cb);
#else
    unused(service_uuid);
#endif
}
#else
void diag_gatt_set_handle(uint8_t server_id, uint16_t conn_id, uint16_t handle, uint8_t inst_id)
{
    g_diag_gatt_server_id = server_id;
    g_diag_gatt_conn_id = conn_id;
    g_diag_gatt_hdl = handle;
    g_diag_gatt_inst_id = inst_id;
}

void diag_gatt_write_req_cbk(uint16_t conn_id, uint8_t server_id, bs_gatts_req_write_cb_t *write_cb_para,
    errcode_t status)
{
    unused(status);

    if (write_cb_para->attribute_id == DFX_GATT_SERVICE_DIAG_CHARACTER) {
        diag_gatt_set_handle(server_id, conn_id, write_cb_para->attribute_id, write_cb_para->inst_id);
        (void)uapi_diag_channel_rx_mux_char_data(DIAG_CHANNEL_ID_2, write_cb_para->value, write_cb_para->value_len);
    } else if (write_cb_para->attribute_id == DFX_GATT_SERVICE_DIAG_DESCRIPT) {
        if ((write_cb_para->value_len == 2) && (*(char*)(write_cb_para->value) == 1)) { /* 2:data length */
            diag_gatt_set_handle(server_id, conn_id, write_cb_para->attribute_id, write_cb_para->inst_id);
            msg_center_set_trans_channel(DIAG_FRAME_FID_BLE_GATT);
#if defined (CONFIG_DFX_CLOUD_REPORT)
            diag_set_ble_conn_state(GAP_BLE_STATE_CONNECTED);
            diag_set_report_ctrl(false);
#endif
        } else if ((write_cb_para->value_len == 2) && (*(char*)(write_cb_para->value) == 0)) { /* 2:data length */
            diag_gatt_set_handle(0, 0, 0, 0);
            msg_center_set_trans_channel(DIAG_FRAME_FID_BT_SPP);
        }
    }
}

static errcode_t diag_gatt_transmit_result_hook(errcode_t result, uintptr_t usr_data)
{
    unused(result);
    unused(usr_data);

    ChangeBleConnectToDefault();

    return ERRCODE_SUCC;
}

static errcode_t diag_gatt_transmit_start_hook(uint16_t channel_id, uintptr_t usr_data)
{
    unused(usr_data);

    if (channel_id == DIAG_FRAME_FID_BLE_GATT) {
        msg_center_set_trans_channel(DIAG_FRAME_FID_BLE_GATT);
        ChangeBleConnectToMax();
    }

    return ERRCODE_SUCC;
}

void diag_channel_register_bt_gatt_init(void)
{
    ble_set_gatt_waitfor_time(DFX_GATT_SERVICE_WAITE_TIME);
    bt_uuid_t server_uuid = { DFX_GATT_UUID_LEN_2_BYTE, DFX_GATT_SERVER_UUID};
    errcode_t ret = ERRCODE_SUCC;
    ret = bs_gatts_register(&server_uuid, &g_diag_gatt_server_id);
    if (ret != ERRCODE_BT_SUCCESS) {
        PRINT("bs_gatts_register error ret: %x\n", ret);
        return;
    }

    ret =  bs_gatts_service_instance_init(g_diag_gatt_server_id,
        (bs_gatt_service_template_t*)(&g_dfx_gatt_service), (uint8_t*)(&g_diag_gatt_inst_id));
    if (ret != ERRCODE_SUCC) {
        PRINT("bs_gatts_service_instance_init error ret: %x\n", ret);
        return;
    }

    ret = uapi_transmit_device_register_hook(&g_dfx_gatt_cb);
    if (ret != ERRCODE_SUCC) {
        PRINT("uapi_transmit_device_register_hook error ret: %x\n", ret);
        return;
    }
}
#endif

#ifndef SUPPORT_DEVICE_ONLY
static errcode_t diag_channel_bt_gatt_output(void *fd, dfx_data_type_t data_type, uint8_t *data[], uint16_t len[],
    uint8_t cnt)
{
    errcode_t ret = ERRCODE_FAIL;
    uint8_t *send_data = NULL;
    uint32_t send_len = 0;

    unused(fd);
    unused(data_type);

    if (data[0] == NULL || len[0] == 0 || cnt == 0) {
        return ERRCODE_INVALID_PARAM;
    }

#ifdef JS_ENABLE
    bs_gatts_ntf_ind_t param = {0};
    param.instance_id = g_diag_gatt_inst_id;
    param.attribute_id = g_diag_gatt_hdl;
#else
    gatts_ntf_ind_t param = {0};
    param.attr_handle = g_diag_gatt_hdl;
#endif

    if (cnt > 1) {
        send_data = diag_channel_merge_data(data, len, cnt, &send_len);
        if (send_data == NULL) {
            return ERRCODE_MALLOC;
        }
        param.value_len = send_len;
        param.value = send_data;
    } else {
        param.value_len = len[0];
        param.value = data[0];
    }

#ifdef JS_ENABLE
    ret = bs_gatts_notify_indicate(g_diag_gatt_conn_id, g_diag_gatt_server_id, &param);
#else
    ret = gatts_notify_indicate(g_diag_gatt_server_id, g_diag_gatt_conn_id, &param);
#endif

    if (send_data != NULL) {
        dfx_free(0, send_data);
    }

    return ret;
}
#endif

errcode_t diag_cmd_uart_config(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option)
{
    unused(cmd_param_size);
    errcode_t ret = ERRCODE_FAIL;

    if (g_diag_uart_id >= UART_BUS_MAX_NUM) {
        return ERRCODE_INVALID_PARAM;
    }
    if (cmd_id == DIAG_CMD_CHANGE_UART_CONFIG) {
        uart_attr_t *attr = cmd_param;
        ret = uapi_uart_set_attr(g_diag_uart_id, attr);
        if (ret == ERRCODE_SUCC) {
            PRINT("uart set cfg OK! baud_rate = %d data_bits = %d stop_bits = %d parity = %d\n",
                  attr->baud_rate, attr->data_bits, attr->stop_bits, attr->parity);
            uapi_diag_report_packet(cmd_id, option, (uint8_t *)("success\r\n"),
                (uint16_t)strlen("success\r\n") + 1, true);
        } else {
            PRINT("uart set cfg failed!\r\n");
            uapi_diag_report_packet(cmd_id, option, (uint8_t *)("fail\r\n"), (uint16_t)strlen("fail\r\n") + 1, false);
        }
    } else if (cmd_id == DIAG_CMD_GET_UART_CONFIG) {
        uart_attr_t get_attr = {0};
        ret = uapi_uart_get_attr(g_diag_uart_id, &get_attr);
        uapi_diag_report_packet(cmd_id, option, (uint8_t *)&get_attr, sizeof(get_attr), true);
    }

    return ret;
}

void diag_uart_rx_proc(uint8_t *buffer, uint16_t length)
{
    uapi_diag_channel_rx_mux_char_data(DIAG_CHANNEL_ID_0, buffer, length);
}

static int32_t diag_channel_uart_output(void *fd, dfx_data_type_t data_type, uint8_t *data[], uint16_t len[],
                                        uint8_t cnt)
{
    unused(fd);
    unused(data_type);
#if ENABLE_LOW_POWER == YES
    uapi_pm_add_sleep_veto(PM_ID_SYS);
#endif
    int int_flag = osal_in_interrupt();
    if (int_flag == 0) {
        (void)osal_mutex_lock_timeout(&g_diag_uart_out_mutex, OSAL_MUTEX_WAIT_FOREVER);
    }
    for (uint8_t i = 0; i < cnt; i++) {
        if (len[i] != 0) {
            log_uart_send_buffer(data[i], len[i]);
        }
    }
    if (int_flag == 0) {
        osal_mutex_unlock(&g_diag_uart_out_mutex);
    }
#if ENABLE_LOW_POWER == YES
    uapi_pm_remove_sleep_veto(PM_ID_SYS);
#endif
    return ERRCODE_SUCC;
}

errcode_t diag_channel_uart_tx(uint8_t *data, uint16_t len)
{
    uint8_t *out_data[1];
    uint16_t out_len[1];
    out_data[0] = data;
    out_len[0] = len;
    return (uint32_t)diag_channel_uart_output(0, DFX_DATA_DIAG_PKT, out_data, out_len, 1);
}

errcode_t diag_register_channel(void)
{
    g_diag_channel_id = DIAG_CHANNEL_ID_0;
    if (uapi_diag_channel_init(g_diag_channel_id, DIAG_CHANNEL_ATTR_NEED_RX_BUF) != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    if (uapi_diag_channel_set_connect_hso_addr(g_diag_channel_id, (uint8_t)diag_adapt_get_default_dst()) !=
        ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    if (uapi_diag_channel_set_tx_hook(g_diag_channel_id, diag_channel_uart_output) != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    osal_mutex_init(&g_diag_uart_out_mutex);

#if (CONFIG_DFX_SUPPORT_DIAG_CONNECT_BLE == DFX_YES)
    errcode_t ret;
    /* SPP通道 */
    ret = uapi_diag_channel_init(DIAG_CHANNEL_ID_1, DIAG_CHANNEL_ATTR_NEED_RX_BUF);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    if (uapi_diag_channel_set_tx_hook(DIAG_CHANNEL_ID_1, diag_channel_bt_output) != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    if (uapi_diag_channel_set_mfs(DIAG_CHANNEL_ID_1, DIAG_SER_DATA_MFS, DIAG_SER_DATA_MAX_LEN) != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

#ifdef SUPPORT_AUTO_OTA
    diag_channel_register_bt_callback();
#endif
    /* BLE GATT通道 */
    if (uapi_diag_channel_init(DIAG_CHANNEL_ID_2, DIAG_CHANNEL_ATTR_NEED_RX_BUF) != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    if (uapi_diag_channel_set_mfs(DIAG_CHANNEL_ID_2, DIAG_SER_GATT_DATA_MFS, DIAG_SER_DATA_MAX_LEN) != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

#ifndef SUPPORT_DEVICE_ONLY
    if (uapi_diag_channel_set_tx_hook(DIAG_CHANNEL_ID_2, diag_channel_bt_gatt_output) != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
#endif
#endif /* CONFIG_DFX_SUPPORT_DIAG_CONNECT_BLE */
    return ERRCODE_SUCC;
}
