/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description: SLE video trans. \n
 * Author: @CompanyNameTag \n
 * History: add new file \n
 * 2025-10-25, Create file. \n
 */

#include <stdbool.h>
#include <unistd.h>
#include "securec.h"
#include "sle_data_trans.h"
#include "bs_device_mgr.h"
#include "ssf_init.h"
#include "ssf_config_stru.h"
#include "ssf_config.h"
#include "ssf_connection.h"
#include "sle_connection_manager.h"
#ifdef CHBA_SUPPORT
#include "sle_chba_manager.h"
#include "osal_addr.h"
#define CHBA_MODE 0
#endif

#undef THIS_FILE_ID
#define THIS_FILE_ID BTH_GLE_SAMPLE_UUID_CLIENT

#define SLE_DEMO_TRANSMITTER_ADDR        {0x11, 0x22, 0x33, 0x44, 0x55, 0x66}
#define SLE_DEMO_RECEIVER_ADDR           {0xEE, 0xEF, 0x33, 0x9B, 0x8D, 0xBB}
#define SLE_DEMO_RECEIVER_DEVICE_NAME    "sle_chba_video_demo_central"
#define MS_300                           300000
#define SLE_CHBA_HEADER                  1

#define sample_log_print(msg...)    do { \
                                    printf("[sle receiver] "); \
                                    printf(msg); \
                                } while (0)
/* 扫描到的预期对端地址 */
static uint8_t g_peer_addr[SLE_ADDR_LEN] = SLE_DEMO_TRANSMITTER_ADDR;
/* 接收端上层回调 */
sle_receiver_callback_t g_sle_receiver_cbk = {0};

errcode_t sle_receiver_reg_callback(sle_receiver_callback_t *func)
{
    if (func == NULL) {
        return ERRCODE_SLE_POINTER_NULL;
    }

    if (memcpy_s(&g_sle_receiver_cbk, sizeof(g_sle_receiver_cbk),
        func, sizeof(sle_receiver_callback_t)) != EOK) {
        return ERRCODE_SLE_MEMCPY_FAIL;
    }
    return ERRCODE_SLE_SUCCESS;
}

void sle_receiver_discovery_state_cbk(uint8_t type, uint8_t state)
{
    sample_log_print("discovery state, type:0x%02x, state:0x%02x.\r\n", type, state);
    if (type == SLE_DISCOVERY_CENTRAL && state == BS_SLE_COMMON_STATE_DISABLE) {
        usleep(MS_300);
        sle_addr_t addr = { 0 };
        memcpy_s(&addr.addr, SLE_ADDR_LEN, g_peer_addr, SLE_ADDR_LEN);
        bs_sle_start_pair(&addr);
    }
}

void sle_receiver_discovery_result_info_cbk(const sle_addr_t *addr,
    bs_sle_discovery_result_t *discovery_result, uint8_t *data, uint16_t data_len)
{
    unused(discovery_result);
    unused(data);
    unused(data_len);
    if (addr != NULL) {
        sample_log_print("checking MAC address...\r\n");
        if (!memcmp(g_peer_addr, addr->addr, (sizeof(g_peer_addr) / sizeof(g_peer_addr[0])))) {
            sample_log_print("find transmitter device.\r\n");
            bs_sle_stop_discovery(SLE_DISCOVERY_CENTRAL);
        }
    }
}

void sle_receiver_connect_state_changed_cbk(bs_sle_connection_result_t *result)
{
    errcode_sle_t ret = ERRCODE_SLE_SUCCESS;
    sample_log_print("connect state cbk device_id:0x%02x, conn_state:0x%x, encrypt_state:0x%x, reason:0x%x, \
        local role:0x%x\r\n", result->dev_id, result->conn_state, result->encrypt_state, result->reason, result->local_role);
    if (result->conn_state == SLE_CONNECT_CONNECTED && result->encrypt_state == SLE_ENCRYPT_SUCCESS) {
#ifdef CHBA_SUPPORT
        ret = sle_chba_netdev_add_link(result->dev_id, (const sle_addr_t *)&result->addr);
        sample_log_print("chba add link, ret: %x\r\n", ret);
#endif
        if (g_sle_receiver_cbk.link_status_cb != NULL) {
            g_sle_receiver_cbk.link_status_cb(SLE_TRANSMISSION_LINK_READY);
        }
    } else if (result->conn_state == SLE_CONNECT_DISCONNECTED) {
        sample_log_print("SLE disconnect. reason=0x%x\n", result->reason);
        if (g_sle_receiver_cbk.link_status_cb != NULL) {
            g_sle_receiver_cbk.link_status_cb(SLE_TRANSMISSION_LINK_IDLE);
        }
#ifdef CHBA_SUPPORT
        // 此处需注意chab入参使用conn_id，与device_id为不同参数，需要后续重新适配
        sle_chba_netdev_del_link(result->dev_id, (const sle_addr_t *)&result->addr);
#endif
        if (result->reason != SLE_DISCONNECT_BY_LOCAL_HOST) {
            bs_sle_start_discovery(SLE_DISCOVERY_CENTRAL);
        }
    }
}

static void sle_receiver_pair_complete_cbk(uint16_t device_id, const sle_addr_t *addr,
    const bs_sle_pair_info_t *pair_info)
{
    unused(pair_info);
    sample_log_print("pair complete device_id:%02x, addr:%02x:**:**:**:%02x:%02x\r\n",
        device_id, addr->addr[INDEX_0], addr->addr[INDEX_4], addr->addr[INDEX_5]);
}

errcode_t sle_receiver_cbk_register(void)
{
    bs_sle_callbacks_t cbk = {
        .pair_request_cb = NULL,
        .connect_state_cb = sle_receiver_connect_state_changed_cbk,
        .member_connect_state_cb = NULL,
        .pair_complete_cb = sle_receiver_pair_complete_cbk,
        .pair_remove_cb = NULL,
        .discovery_result_cb = sle_receiver_discovery_result_info_cbk,
        .discovery_state_cb = sle_receiver_discovery_state_cbk,
        .auto_conn_state_cb = NULL,
    };

    return bs_sle_register_callbacks(0, &cbk);
}

#ifdef CHBA_SUPPORT
errcode_t sle_chba_netdev_input_cb(uint8_t *chba_data, uint16_t data_len)
{
    uint16_t len = data_len - SLE_CHBA_HEADER;
    uint8_t *data = (uint8_t *)osal_vmalloc(len);
    if (data == NULL) {
        sample_log_print("malloc error.\r\n");
    }
    if (memcpy_s(data, len, chba_data + SLE_CHBA_HEADER, len) != EOK) {
        return ERRCODE_SLE_MEMCPY_FAIL;
    }
    if (g_sle_receiver_cbk.receive_data_cb != NULL) {
        g_sle_receiver_cbk.receive_data_cb(data, len);
        osal_vfree(data);
    }
    return ERRCODE_SUCC;
}

errcode_t sle_receiver_chba_cbk_register(void)
{
    sle_chba_netdev_callbacks_t chba_cbk = {0};
    chba_cbk.netdev_input_cb = sle_chba_netdev_input_cb;
    errcode_t ret = sle_chba_netdev_register_callbacks(&chba_cbk);
    if (ret != ERRCODE_SLE_SUCCESS) {
        sample_log_print("chba cbk reg fail, ret = %x.\r\n", ret);
    }
    return ret;
}
#endif

static void sle_receiver_enable_cbk(uint8_t status)
{
    sample_log_print("enable_cbk status = %x.\n", status);
    errcode_t ret = ERRCODE_SLE_SUCCESS;
    if (status == SLE_STATE_ENABLED) {
        ret |= sle_receiver_cbk_register();
#ifdef CHBA_SUPPORT
        ret |= sle_receiver_chba_cbk_register();
        ret |= sle_chba_netdev_create(CHBA_ROLE_AP, CHBA_MODE);
#endif
        if (ret == ERRCODE_SLE_SUCCESS) {
            bs_sle_start_discovery(SLE_DISCOVERY_CENTRAL);
        }
    }
}

errcode_t sle_receiver_set_addr(uint8_t *addr, uint8_t len)
{
    if (addr == NULL || len != SLE_ADDR_LEN) {
        return ERRCODE_SLE_PARAM_ERR;
    }
    if (memcpy_s(g_peer_addr, SLE_ADDR_LEN, addr, SLE_ADDR_LEN) != EOK) {
        return ERRCODE_SLE_MEMCPY_FAIL;
    }
    return ERRCODE_SLE_SUCCESS;
}

errcode_t sle_receiver_init(void)
{
    errcode_t ret = ERRCODE_SUCC;
    char dev_name[] = SLE_DEMO_RECEIVER_DEVICE_NAME;
    uint8_t local_addr[] = SLE_DEMO_RECEIVER_ADDR;
    uint8_t role = SLE_CONFIG_ROLE_CENTRAL;
    uint32_t appearance = SLE_CONFIG_ACTION_CAMERA;
    bs_sle_stack_state_callbacks_t cbk = {
        .stack_state_cbk = sle_receiver_enable_cbk,
    };
    bs_sle_register_stack_state_callbacks(0, &cbk);

    ret |= bs_config(SLE_CONFIG_COMMON_ROLE, &role, sizeof(uint8_t));
    ret |= bs_config(SLE_CONFIG_ADV_APPEARANCE, &appearance, sizeof(uint32_t));
    ret |= bs_config(SLE_CONFIG_ADV_SHORT_NAME, dev_name, sizeof(dev_name));
    ret |= bs_config(SLE_CONFIG_COMMON_LOCAL_ADDR, local_addr, SLE_ADDR_LEN);
    ret |= bs_sle_enable();

    return ret;
}

errcode_t sle_receiver_deinit(void)
{
    g_sle_receiver_cbk.link_status_cb = NULL;
    g_sle_receiver_cbk.receive_data_cb = NULL;
#ifdef CHBA_SUPPORT
    sle_chba_netdev_destroy();
#endif
    errcode_t ret = bs_sle_disable();
    if (ret != ERRCODE_SLE_SUCCESS) {
        sample_log_print("disable sle fail!\n");
    }
    return ret;
}