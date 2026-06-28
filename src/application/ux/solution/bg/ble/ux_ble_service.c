/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: Provides ux ble service\n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-09-16， Create file. \n
 */
#include <stdint.h>
#include "securec.h"
#include "ux_event_define.h"
#include "bts_gatt_stru.h"
#include "common_def.h"
#include "bs_ble_gatt_server.h"
#include "bs_ble_service_config.h"
#include "bs_device_mgr.h"
#include "ux_errcode.h"
#include "ux_common_log.h"
#include "ux_fwk_msg.h"
#include "ux_hal_memory.h"
#include "ux_fwk_process.h"
#include "ux_api_ble.h"
#include "bs_ble_gatt_stru.h"
#include "ux_ble_service.h"

#define BLE_CHARA_VALUE_LEN 517
#define BLE_VALUE_LEN 2
#define BT_UUID_LEN   2
static uint8_t g_sample_character_val[BLE_CHARA_VALUE_LEN] = {0x00, 0x00};
/* 特征信息添加成功后特征值修改为0x01 */
static uint8_t g_sample_descriptor_val[2] = {0x01, 0x00};
static uint8_t g_mobile_server_id = 0;
static uint8_t g_mobile_inst_id = 0;
static ux_api_ble_msg_handler g_ble_msg_handler;
static uint16_t g_device_id;

static bt_uuid_t g_ble_app_uuid = {0x02, {0x12, 0x34}}; /* UX使用的app uuid */
static uint8_t g_ble_server_id = 0;

uint8_t ux_bs_get_ble_service_id(void)
{
    return g_ble_server_id;
}

/* 数传通道数据接收回调 */
static void ux_ble_data_received_callback(uint16_t dev_id, uint8_t server_id,
    bs_gatts_req_write_cb_t *write_cb_para, errcode_t status)
{
    unused(server_id);
    unused(status);
    g_device_id = dev_id;
    ux_cmd_msg_t msg = { 0 };
    msg.mail.comm.event = UX_EVENT_TYPE_BLE_HOST;
    msg.mail.config_len = 0;
    msg.mail.config_buff = NULL;
    if (write_cb_para->value != NULL && write_cb_para->value_len != 0) {
        msg.mail.config_buff = (void *)ux_hal_memory_acquire(UX_MEM_MAIL, write_cb_para->value_len);
        if (msg.mail.config_buff == NULL) {
            ux_log_err("[ux][bs_ble]malloc failed.\r\n");
            return;
        }
        msg.mail.config_len = write_cb_para->value_len;
        if (memcpy_s(msg.mail.config_buff, msg.mail.config_len,
            write_cb_para->value, write_cb_para->value_len) != UX_ERR_SUCCESS) {
            ux_log_err("[ux][bs_ble]memset_s failed.\r\n");
            return;
        }
    }

    if (sys_send_msg(SYS_MODULE_APP_UX, &msg) != ERRCODE_SUCC) {
        if (msg.mail.config_buff != NULL) {
            ux_hal_memory_release(UX_MEM_MAIL, msg.mail.config_buff);
            msg.mail.config_buff = NULL;
        }
        ux_log_err("[ux][bs_ble]sys_send_msg failed\r\n");
    }
}

static const ux_bs_gatt_service_t g_gatt_service = {
    .uuid = {BT_UUID_LEN, {0xAA, 0x00}},
    .is_primary = 1,
    .attribute_count = BLE_MOBILE_CHARACTER_COUNT,
    .cbks.start_service_cb = NULL,
    .cbks.delete_service_cb = NULL,
    .cbks.read_request_cb = NULL,
    .cbks.write_request_cb = ux_ble_data_received_callback,
    .cbks.indicate_cfm_cb = NULL,
    .cbks.mtu_changed_cb = NULL,
    .attribute_array = {
        [MOBILE_CHARA] = {
            .type = BS_GATTS_CHARAC_TYPE,
            .character = {
                .uuid = {BT_UUID_LEN, {0xFF, 0x01}},
                .permissions = BS_GATT_ATTRIBUTE_PERMISSION_READ | BS_GATT_ATTRIBUTE_PERMISSION_WRITE,
                .properties = BS_GATT_CHARACTER_PROPERTY_BIT_NOTIFY | BS_GATT_CHARACTER_PROPERTY_BIT_WRITE_NO_RSP |
                              BS_GATT_CHARACTER_PROPERTY_BIT_READ,
                .value_len = BLE_CHARA_VALUE_LEN,
                .value = g_sample_character_val,
            },
        },
        [MOBILE_DESC] = {
            .type = BS_GATTS_DESC_TYPE,
            .descriptor = {
                .uuid = {BT_UUID_LEN, {0x29, 0x02}},
                .permissions = BS_GATT_ATTRIBUTE_PERMISSION_READ | BS_GATT_ATTRIBUTE_PERMISSION_WRITE,
                .value_len = BLE_VALUE_LEN,
                .value = g_sample_descriptor_val,
            },
        },
    },
};

static errcode_t process_ble_service_msg(uint8_t sub_event, uint8_t *data, uint16_t data_len)
{
    unused(sub_event);
    if (data == NULL || data_len == 0) {
        ux_log_err("[process_ble_service_msg]spp data msg null");
        return ERRCODE_INVALID_PARAM;
    }

    ux_log_info("data_len:%d", data_len);
    ux_log_info("data[0]:0x%x ", data[0]);
    ux_log_info("data[%d]:0x%x ", data_len - 1, data[data_len - 1]);

    if (g_ble_msg_handler != NULL) {
        return g_ble_msg_handler(data, data_len);
    }
    ux_log_err("[process_ble_service_msg]g_ble_msg_handler is null");
    return ERRCODE_FAIL;
}

errcode_t ux_bs_ble_service_init(void)
{
    errcode_t ret = ERRCODE_SUCC;
    ret = bs_gatts_register(&g_ble_app_uuid, &g_ble_server_id);
    if (ret != ERRCODE_BT_SUCCESS) {
        ux_log_err("[ux_bs_ble_service_init]bs_gatts_register failed 0x%x!", ret);
        return ret;
    }
    ux_api_fwk_module_register(UX_EVENT_TYPE_BLE_HOST, process_ble_service_msg);
    ret =  bs_gatts_service_instance_init(ux_bs_get_ble_service_id(), (bs_gatt_service_template_t *)&g_gatt_service,
        &g_mobile_inst_id);
    if (ret != EOK) {
        ux_log_err("[ux_bs_ble_service_init]bs_gatts_service_instance_init failed!");
        return ERRCODE_FAIL;
    }
    g_mobile_server_id = ux_bs_get_ble_service_id();
    return ret;
}

static uint16_t ux_bs_ble_get_config_type(ux_api_ble_config_type_t type)
{
    switch (type) {
        case UX_API_BLE_CONFIG_ADV_DATA:
            return BLE_CONFIG_ADV_DATA;
        case UX_API_BLE_CONFIG_SCAN_RSP_ADV_DATA:
            return BLE_CONFIG_ADV_SCAN_RSP;
        default:
            return UX_API_BLE_CONFIG_TYPE_MAX;
    }
}

errcode_t ux_api_ble_set_config(ux_api_ble_config_type_t type, void *data, uint16_t data_len)
{
    errcode_t ret = ERRCODE_SUCC;
    if (data == NULL || data_len == 0) {
        ux_log_err("[ux][ble]config invalid para\r\n");
        return ERRCODE_INVALID_PARAM;
    }
    uint16_t config_type = ux_bs_ble_get_config_type(type);
    if (config_type == UX_API_BLE_CONFIG_TYPE_MAX) {
        ux_log_err("[ux][ble]config fail, not support type:0x%x \r\n", type);
        return ERRCODE_INVALID_PARAM;
    }
    ret = bs_config(config_type, data, data_len);
    if (ret != ERRCODE_SUCC) {
        ux_log_err("[ux][ble]config type:0x%x fail, ret=0x%x \r\n", type, ret);
        return ret;
    }
    ux_log_info("[ux][ble]config type:0x%x data_len:%u success \r\n", type, data_len);
    return ret;
}

errcode_t ux_api_ble_service_send_data(uint16_t dev_id, uint8_t *data, uint16_t data_len)
{
    unused(dev_id);
    errcode_t ret = ERRCODE_SUCC;
    if (data == NULL || data_len == 0) {
        return ERRCODE_BT_FAIL;
    }
    bs_gatts_ntf_ind_t param = {
        .instance_id = g_mobile_inst_id,
        .attribute_id = BLE_MOBILE_RW_CHARACTER_IDX,
        .value_len = data_len,
        .value = data,
    };
    ret = bs_gatts_notify_indicate(g_device_id, g_mobile_server_id, &param);
    if (ret != EOK) {
        return ERRCODE_BT_FAIL;
    }
    return ret;
}

/* ble 注册回调 */
void ux_api_ble_service_register_msg_handler(uint16_t dev_id, ux_api_ble_msg_handler handler)
{
    unused(dev_id);
    g_ble_msg_handler = handler;
}