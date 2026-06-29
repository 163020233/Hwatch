/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BR spp ota demo definitions
 */
#include <stdint.h>
#include "securec.h"
#include "common_def.h"
#include "bts_spp.h"
#include "bts_def.h"
#include "bs_br_service_port_api.h"
#include "ux_errcode.h"
#include "ux_common_log.h"
#include "ux_hal_memory.h"
#include "ux_event_define.h"
#include "ux_fwk_msg.h"
#include "ux_fwk_process.h"
#include "ux_api_bt.h"
#include "dfx_channel.h"
#include "feature_config.h"
#include "ux_spp_service.h"

#if defined (CONFIG_DFX_CLOUD_REPORT)
#include "diag_report_control.h"
#include "diag_data_transport.h"
#endif

#define DIAG_SPP_SERVER_UUID_2_BYTE  0x0111                   /* diag spp uuid */
#ifdef DEMO_WATCH
#define SPP_SERVER_UUID_2_BYTE       0xFF00                   /* 两字节uuid */
#else
#define SPP_SERVER_UUID_2_BYTE       0x0111                   /* 两字节uuid */
#endif
#define SPP_DIAG_SERVER_UUID_2_BYTE   0x0222                   /* diag uuid */

uint8_t SPP_SERVER_uuid_16_byte[SPP_SERVER_UUID_16_BYTE_LEN] = {0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00,
    0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};    /* 16字节uuid */
char spp_server_name[] = "UX_SPP_SERVICE";
uint16_t g_spp_server_port = 0;                                     /* spp 服务端port */
uint16_t g_diag_spp_server_port = 0;                                 /* diag_spp 服务端port */
uint8_t g_spp_server_uuid_len = SPP_SERVER_UUID_2_BYTE_LEN;   /* spp uuid 长度, 默认2字节 */
uint8_t g_spp_service_conn_state = UX_SPP_DISCONNECTED_STATE;
uint8_t g_diag_spp_service_conn_state = UX_SPP_DISCONNECTED_STATE;
static uint8_t g_spp_status = UX_SPP_SERVICE_STATUS_DISABLED;            /* port服务状态 */
ux_api_spp_msg_handler g_spp_msg_handler = NULL;

#ifdef CONFIG_SUPPORT_UX_SPP
static void init_diag_spp_channel(void)
{
    errcode_t ret = 0;
    bs_br_port_create_info_t param = { 0 };
    char *name = "DIAG_SPP";
    uint16_t diag_port = DIAG_SPP_DEFAULT_PORT;

    param.uuid.uuid[0] = (uint8_t)(DIAG_SPP_SERVER_UUID_2_BYTE >> 8); /* 8: octet bit num */
    param.uuid.uuid[1] = (uint8_t)(DIAG_SPP_SERVER_UUID_2_BYTE);
    param.uuid.uuid_len = SPP_SERVER_UUID_2_BYTE_LEN;
    param.name_len = (uint8_t)(strlen(name) + 1);
    param.name = ux_hal_memory_acquire(UX_MEM_SPP, param.name_len);
    if (param.name == NULL) {
        ux_log_err("[ux][bt][spp]init_diag_spp_channel name is null\n");
        return;
    }
    if (strcpy_s(param.name, param.name_len, name) != 0) {
        ux_hal_memory_release(UX_MEM_MAIL, param.name);
        param.name = NULL;
        ux_log_err("[ux][bt][spp]init_diag_spp_channel strcpy_s fail\n");
        return;
    }

    ret = bs_br_port_create_server_port(&param, 0, UX_SPP_SERVICE_SPECIFY_GENERATE, &diag_port);
    if (ret != 0) {
        ux_hal_memory_release(UX_MEM_MAIL, param.name);
        param.name = NULL;
        ux_log_err("[ux][bt][spp]init_diag_spp_channel creat port fail ret = %x\n", ret);
        return;
    }
    diag_channel_register_br_port_callback(&param.uuid, diag_port);
    ux_hal_memory_release(UX_MEM_MAIL, param.name);
    param.name = NULL;
    ux_log_info("[ux][bt][spp]init_diag_spp_channel creat success!\n");
}
#endif

#ifndef FT_DONGLE
/* 服务端收到连接请求回调 */
static void receive_connection_request_cbk(uint16_t dev_id, uint16_t port)
{
    unused(dev_id);
    if (g_spp_service_conn_state == UX_SPP_DISCONNECTED_STATE) {
        bs_br_port_send_conn_rsp(port, true);
    } else {
        bs_br_port_send_conn_rsp(port, false);
    }
}

/* 服务端收到连接状态变化回调 */
static void conn_state_changed_cbk(uint16_t dev_id, uint16_t port, uint8_t state)
{
    unused(dev_id);
    unused(port);
    g_spp_service_conn_state = state;
#if defined (CONFIG_DFX_CLOUD_REPORT)
    diag_set_report_ctrl(false);
    diag_set_bt_conn_state(state);
#endif
}

/* 服务端数据接收回调 */
static void receive_trans_data_cbk(uint16_t dev_id, uint16_t port, uint8_t *data, uint16_t data_len)
{
    unused(dev_id);
    unused(port);
    ux_cmd_msg_t msg = { 0 };
    msg.mail.comm.event = UX_EVENT_TYPE_SPP_HOST;
    msg.mail.config_len = 0;
    msg.mail.config_buff = NULL;
    if (data != NULL && data_len != 0) {
        msg.mail.config_buff = (void *)ux_hal_memory_acquire(UX_MEM_MAIL, data_len);
        if (msg.mail.config_buff == NULL) {
            ux_log_err("[ux][bt][cb]ux_hal_memory_acquire failed.\r\n");
            return;
        }
        msg.mail.config_len = data_len;
        if (memcpy_s(msg.mail.config_buff, data_len, data, data_len) != ERRCODE_SUCC) {
            ux_hal_memory_release(UX_MEM_MAIL, msg.mail.config_buff);
            msg.mail.config_buff = NULL;
            ux_log_err("[ux][bt][cb]memcpy_s failed.\r\n");
            return;
        }
    }
    // 消息发送成功后, msg.mail.config_buff 内存由 ux_fwk_msg.c ux_msg_handle函数进行释放
    if (sys_send_msg(SYS_MODULE_APP_UX, &msg) != ERRCODE_SUCC) {
        if (msg.mail.config_buff != NULL) {
            ux_hal_memory_release(UX_MEM_MAIL, msg.mail.config_buff);
            msg.mail.config_buff = NULL;
        }
        ux_log_err("[ux][bt][cb]sys_send_msg failed\r\n");
    }
}

static errcode_t set_spp_service_param(bs_br_port_create_info_t* param)
{
    param->name = ux_hal_memory_acquire(UX_MEM_SPP, SPP_SERVER_NAME_LEN);
    if (param->name == NULL) {
        ux_log_err("[set_spp_service_param] memory acquire failed\r\n");
        return ERRCODE_MALLOC;
    }
    param->name_len = SPP_SERVER_NAME_LEN;
    (void)memcpy_s(param->name, SPP_SERVER_NAME_LEN, (const char*)(spp_server_name), SPP_SERVER_NAME_LEN);
    if (g_spp_server_uuid_len == SPP_SERVER_UUID_2_BYTE_LEN) {
        param->uuid.uuid[0] = (uint8_t)(SPP_SERVER_UUID_2_BYTE >> 8); /* 8: octet bit num */
        param->uuid.uuid[1] = (uint8_t)(SPP_SERVER_UUID_2_BYTE);
        param->uuid.uuid_len = SPP_SERVER_UUID_2_BYTE_LEN;
    } else if (g_spp_server_uuid_len == SPP_SERVER_UUID_16_BYTE_LEN) {
        (void)memcpy_s(param->uuid.uuid, SPP_SERVER_UUID_16_BYTE_LEN, SPP_SERVER_uuid_16_byte,
            SPP_SERVER_UUID_16_BYTE_LEN);
        param->uuid.uuid_len = SPP_SERVER_UUID_16_BYTE_LEN;
    } else {
        ux_hal_memory_release(UX_MEM_SPP, param->name);
        param->name = NULL;
        ux_log_err("[set_spp_service_param] invalid uuid_len %d\n", g_spp_server_uuid_len);
        return ERRCODE_INVALID_PARAM;
    }
    return UX_ERR_SUCCESS;
}

static errcode_t process_spp_service_msg(uint8_t sub_event, uint8_t *data, uint16_t data_len)
{
    unused(sub_event);
    if (data == NULL || data_len == 0) {
        ux_log_err("[process_spp_service_msg]spp data msg null");
        return ERRCODE_INVALID_PARAM;
    }

    ux_log_info("data_len:%d", data_len);
    ux_log_info("data[0]:0x%x ", data[0]);
    ux_log_info("data[%d]:0x%x ", data_len - 1, data[data_len - 1]);

    if (g_spp_msg_handler != NULL) {
        return g_spp_msg_handler(data, data_len);
    }
    ux_log_err("[process_spp_service_msg]g_spp_msg_handler is null");
    return ERRCODE_FAIL;
}
#endif

bool get_ux_spp_service_connect_state(uint16_t port)
{
    return spp_is_connected(port);
}

uint8_t get_ux_spp_service_enable_state(void)
{
    return g_spp_status;
}

uint16_t get_ux_spp_port_number(void)
{
    return g_spp_server_port;
}

uint16_t get_diag_spp_port_number(void)
{
    return g_diag_spp_server_port;
}

errcode_t ux_spp_service_disconnect(void)
{
    errcode_t ret = 0;
    if (g_spp_status == UX_SPP_SERVICE_STATUS_DISABLED) {
        ux_log_err("[ux_spp_service_disconnect]please enable spp profile first!\r\n");
        return ERRCODE_FAIL;
    }

    ret = bs_br_port_disconnect(0, g_spp_server_port);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux_spp_service_disconnect]spp profile disconnect fail, ret 0x%08x, spp_port: %u\r\n",
            ret, g_spp_server_port);
        return ret;
    }

    return UX_ERR_SUCCESS;
}

errcode_t ux_spp_service_destroy(void)
{
    errcode_t ret = 0;
    if (g_spp_status == UX_SPP_SERVICE_STATUS_DISABLED) {
        ux_log_err("[ux_spp_service_destroy]please enable spp profile first!\r\n");
        return ERRCODE_FAIL;
    }

    ret = bs_br_port_destroy(g_spp_server_port);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux_spp_service_destroy]spp profile server destory fail, ret 0x%08x\r\n", ret);
        return ret;
    }

    return UX_ERR_SUCCESS;
}

#if defined(MELODY_DEMO_EARPHONE) || defined(MELODY_DEMO_HA) || (defined(MELODY_DEMO_SOUND_BOX) && defined(CONFIG_SUPPORT_BLE_CHANNEL))
static void diag_receive_connection_request_cbk(uint16_t dev_id, uint16_t port)
{
    unused(dev_id);
    if (g_diag_spp_service_conn_state == UX_SPP_DISCONNECTED_STATE) {
        bs_br_port_send_conn_rsp(port, true);
    } else {
        bs_br_port_send_conn_rsp(port, false);
    }
}

static void diag_conn_state_changed_cbk(uint16_t dev_id, uint16_t port, uint8_t state)
{
    unused(dev_id);
    unused(port);
    g_diag_spp_service_conn_state = state;
#if defined (CONFIG_DFX_CLOUD_REPORT)
    diag_set_report_ctrl(false);
    diag_set_bt_conn_state(state);
#endif
}

static void diag_spp_service_init(void)
{
    errcode_t ret;
    bs_br_port_create_info_t param = { 0 };
    char *name = "DIAG_SPP_SERVER";
    param.name_len = (uint8_t)(strlen(name) + 1);
    param.name = ux_hal_memory_acquire(UX_MEM_SPP, param.name_len);
    if (param.name == NULL) {
        ux_log_err("[ux][diag_spp]diag_spp_service_init name is null\n");
        return;
    }
    if (strcpy_s(param.name, param.name_len, name) != 0) {
        ux_hal_memory_release(UX_MEM_MAIL, param.name);
        param.name = NULL;
        ux_log_err("[ux][diag_spp]diag_spp_service_init strcpy_s fail\n");
        return;
    }
    param.uuid.uuid_len = SPP_SERVER_UUID_2_BYTE_LEN;
    param.uuid.uuid[0] = (uint8_t)(SPP_DIAG_SERVER_UUID_2_BYTE >> 8); /* 8: octet bit num */
    param.uuid.uuid[1] = (uint8_t)(SPP_DIAG_SERVER_UUID_2_BYTE);
    ret = bs_br_port_create_server_port(&param, 0, UX_SPP_SERVICE_AUTO_GENERATE, &g_diag_spp_server_port);
    if (ret != ERRCODE_SUCC) {
        ux_log_err("[ux][diag_spp]br diag spp create server port fail, ret 0x%08x\r\n", ret);
        ux_hal_memory_release(UX_MEM_MAIL, param.name);
        param.name = NULL;
        return;
    }
    bs_br_port_callbacks_t ux_diag_spp_service_callback_funs = {
        .conn_req_cbk = diag_receive_connection_request_cbk,
        .receive_data_cbk = spp_receive_trans_data_cbk,
        .conn_state_cbk = diag_conn_state_changed_cbk,
    };
    ret = bs_br_port_register_callbacks(&param.uuid, &ux_diag_spp_service_callback_funs);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][ota_spp]register callbacks failed, ret 0x%08x\r\n", ret);
    }
    ux_hal_memory_release(UX_MEM_MAIL, param.name);
    param.name = NULL;
}
#endif

errcode_t ux_spp_service_init(void)
{
    bs_br_port_enable();
#ifdef CONFIG_SUPPORT_UX_SPP
    init_diag_spp_channel();
#endif
#if defined(MELODY_DEMO_EARPHONE) || defined(MELODY_DEMO_HA) || (defined(MELODY_DEMO_SOUND_BOX) && defined(CONFIG_SUPPORT_BLE_CHANNEL))
    diag_spp_service_init();
#endif
#ifndef FT_DONGLE
    errcode_t ret = 0;
    bs_br_port_create_info_t param = { 0 };
    ux_api_fwk_module_register(UX_EVENT_TYPE_SPP_HOST, process_spp_service_msg);
    if (g_spp_status == UX_SPP_SERVICE_STATUS_STARTED) {
        ux_log_warn("[spp_service] enable\r\n");
        return ERRCODE_FAIL;
    }
    g_spp_status = UX_SPP_SERVICE_STATUS_STARTING;
    ret = set_spp_service_param(&param);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux_spp_service_init]set spp service param fail, ret 0x%08x\r\n", ret);
        return ret;
    }
    ret = bs_br_port_create_server_port(&param, SPP_SERVER_INVALID_CHANNEL, UX_SPP_SERVICE_AUTO_GENERATE,
        &g_spp_server_port);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux_spp_service_init]br spp create server port fail, ret 0x%08x\r\n", ret);
        ux_hal_memory_release(UX_MEM_SPP, param.name);
        param.name = NULL;
        return ret;
    }
    bs_br_port_callbacks_t ux_spp_service_callback_funs = {
        .conn_req_cbk = receive_connection_request_cbk,
        .receive_data_cbk = receive_trans_data_cbk,
        .conn_state_cbk = conn_state_changed_cbk,
    };
    ret = bs_br_port_register_callbacks(&param.uuid, &ux_spp_service_callback_funs);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux_spp_service_init]register callbacks failed, ret 0x%08x\r\n", ret);
        if (param.name != NULL) {
            ux_hal_memory_release(UX_MEM_SPP, param.name);
            param.name = NULL;
        }
        return ret;
    }
    g_spp_status = UX_SPP_SERVICE_STATUS_STARTED;
    ux_hal_memory_release(UX_MEM_SPP, param.name);
    param.name = NULL;
#endif
    return UX_ERR_SUCCESS;
}

errcode_t ux_spp_service_deinit(void)
{
    errcode_t ret = 0;
    if (g_spp_status != UX_SPP_SERVICE_STATUS_STARTED) {
        ux_log_err("[ux_spp_service_deinit]spp profile is not enable!\r\n");
        return ERRCODE_FAIL;
    }

    ret = bs_br_port_disable();
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux_spp_service_deinit]spp profile disable fail, ret 0x%08x\r\n", ret);
        return ret;
    }

    g_spp_status = UX_SPP_SERVICE_STATUS_DISABLED;

    return UX_ERR_SUCCESS;
}

errcode_t ux_api_spp_service_send_data(uint16_t dev_id, uint8_t *data, uint16_t data_len)
{
    unused(dev_id);
    errcode_t ret = 0;
    if (g_spp_status == UX_SPP_SERVICE_STATUS_DISABLED) {
        ux_log_err("[ux_api_spp_service_send_data]please enable spp profile first!\r\n");
        return ERRCODE_FAIL;
    }

    if (data == NULL || data_len == 0) {
        ux_log_err("[ux_api_spp_service_send_data]send data is wrong.\r\n");
        return ERRCODE_FAIL;
    }

    ret = bs_br_port_write_data(SPP_SERVER_INVALID_DEV_ID, g_spp_server_port, data, data_len);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux_api_spp_service_send_data]send data fail, ret 0x%08x\r\n", ret);
        return ret;
    }

    return UX_ERR_SUCCESS;
}

/* spp 注册回调 */
void ux_api_spp_service_register_msg_handler(uint16_t dev_id, ux_api_spp_msg_handler handler)
{
    unused(dev_id);
    g_spp_msg_handler = handler;
}