/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description: SLE video trans. \n
 * Author: @CompanyNameTag \n
 * History: add new file \n
 * 2025-10-25, Create file. \n
 */

#include "sle_data_trans.h"
#include "at_cmd.h"
#include "sle_connection_manager.h"
#include "bs_device_mgr.h"
#include "ssf_init.h"
#include "ssf_config_stru.h"
#include "ssf_config.h"
#include "ssf_connection.h"
#ifdef CHBA_SUPPORT
#include "sle_chba_manager.h"
#include "sle_chba_transmition.h"
#define CHBA_MODE 0
#endif

/* CHBA中的以太网地址类型包头 */
#define SLE_CHBA_HEADER                           1
/* CHBA中的以太网地址类型包头 */
#define SLE_SEND_DATA_DELAY_SLOT_TIME             10
#define SLE_SEND_DATA_TRY_DELAY_SLOT_TIME         2
#define SLE_SEND_DATA_TRY_TIMES                   5
/* 设备信息 */
#define SLE_DEMO_TRANSMITTER_DEVICE_NAME    "SLETransmiter"
#define SLE_DEMO_TRANSMITTER_ADDR           {0x11, 0x22, 0x33, 0x44, 0x55, 0x66}
#define sample_log_print(msg...)    do { \
                                    printf("[sle transmitter] "); \
                                    printf(msg); \
                                } while (0)

#define SAMPLE_TRANSMITTER_TASK_STACK_SIZE 0x1000
#define SAMPLE_TRANSMITTER_TASK_PRIORITY   23

uint8_t g_local_addr[] = SLE_DEMO_TRANSMITTER_ADDR;
/* sle send data delay time */
uint8_t g_send_data_delay = SLE_SEND_DATA_DELAY_SLOT_TIME;
/* sle connect acb status */
uint8_t g_sle_conn_status = SLE_TRANSMISSION_LINK_IDLE;
/* 上层回调 */
sle_link_status_callback g_link_status_cbk = NULL;

int32_t sle_transmitter_send_task(void *param)
{
    unused(param);
    if (g_link_status_cbk != NULL) {
        g_link_status_cbk(SLE_TRANSMISSION_LINK_READY);
    }
    return 0;
}

errcode_t sle_transmitter_reg_callback(sle_link_status_callback func)
{
    if (func == NULL) {
        return ERRCODE_SLE_POINTER_NULL;
    }
    g_link_status_cbk = func;
    return ERRCODE_SLE_SUCCESS;
}

errcode_t sle_transmitter_send_data(uint8_t *data, uint16_t len)
{
    errcode_t ret = ERRCODE_SLE_SUCCESS;
    if (data == NULL || len == 0) {
        sample_log_print("data param error.\r\n");
        return ERRCODE_SLE_PARAM_ERR;
    }
    if (g_sle_conn_status == SLE_TRANSMISSION_LINK_IDLE) {
        return ERRCODE_SLE_STATUS_ERR;
    }
#ifdef CHBA_SUPPORT
    uint8_t count = 0;
    while (g_sle_conn_status == SLE_TRANSMISSION_LINK_BUSY) {
        osal_msleep(SLE_SEND_DATA_TRY_DELAY_SLOT_TIME);
        if (++count == SLE_SEND_DATA_TRY_TIMES) {
            break;
        }
    }
    /* 当前发端没有依据视频码率的传输速率控制方案，使用延时避免传输速率过快，收端解码异常 */
    osal_msleep(g_send_data_delay);
    /* chba对接lwip时添加了数据校验，首位需为0 */
    uint8_t *chba_data = (uint8_t *)osal_vmalloc(SLE_CHBA_HEADER + len);
    if (chba_data == NULL) {
        sample_log_print("chba data malloc fail.\r\n");
        return ERRCODE_SLE_MALLOC_FAIL;
    }
    *chba_data = 0;
    if (memcpy_s(chba_data + SLE_CHBA_HEADER, len, data, len) != EOK) {
        osal_vfree(chba_data);
        return ERRCODE_SLE_MEMCPY_FAIL;
    }
    ret = sle_chba_netdev_driver_send(chba_data, len + SLE_CHBA_HEADER);
    osal_vfree(chba_data);
#endif
    return ret;
}

void sle_transmitter_set_send_data_delay_ms(uint8_t delay_ms)
{
    g_send_data_delay = delay_ms;
}

 /*
 * sle connect 回调注册
 */
static void sle_transmitter_connect_state_changed_cbk(bs_sle_connection_result_t *result)
{
    sample_log_print("connect state cbk device_id:0x%02x, conn_state:0x%x, encrypt_state:0x%x, reason:0x%x, \
        role:0x%x\r\n", result->dev_id, result->conn_state, result->encrypt_state, result->reason, result->local_role);
    sample_log_print("addr:%02x:**:**:**:%02x:%02x\n", result->addr.addr[INDEX_0], result->addr.addr[INDEX_4], result->addr.addr[INDEX_5]);
    if (result->conn_state == SLE_CONNECT_CONNECTED && result->encrypt_state == SLE_ENCRYPT_SUCCESS) {
        bs_sle_stop_discovery(SLE_DISCOVERY_PERIPHERAL);
#ifdef CHBA_SUPPORT
        errcode_t ret = sle_chba_netdev_add_link(result->dev_id, (const sle_addr_t *)&result->addr);
        sample_log_print("chba add link, ret: %d\r\n", ret);
#endif
        osal_task *task = osal_kthread_create(sle_transmitter_send_task,
            (void *)(uintptr_t)SLE_TRANSMISSION_LINK_READY,
            "sle_trans_send_task", SAMPLE_TRANSMITTER_TASK_STACK_SIZE);
        if (task == NULL) {
            sample_log_print("create task fail\n");
            return;
        }
        osal_kthread_set_priority(task, SAMPLE_TRANSMITTER_TASK_PRIORITY);
    } else if (result->conn_state == SLE_CONNECT_DISCONNECTED) {
        g_sle_conn_status = SLE_TRANSMISSION_LINK_IDLE;
        if (g_link_status_cbk != NULL) {
            g_link_status_cbk(SLE_TRANSMISSION_LINK_IDLE);
        }
#ifdef CHBA_SUPPORT
        sle_chba_netdev_del_link(result->dev_id, (const sle_addr_t *)&result->addr);
#endif
        if (result->reason != SLE_DISCONNECT_BY_LOCAL_HOST) {
            bs_sle_start_discovery(SLE_DISCOVERY_PERIPHERAL);
        }
    }
}

void sle_transmitter_discovery_result_info_cbk(const sle_addr_t *addr,
    bs_sle_discovery_result_t *discovery_result, uint8_t *data, uint16_t data_len)
{
    unused(addr);
    unused(discovery_result);
    unused(data);
    unused(data_len);
    sample_log_print("discovery_result_info_cbk...\r\n");
}

void sle_transmitter_discovery_state_cbk(uint8_t type, uint8_t state)
{
    sample_log_print("discovery state, type:0x%02x, state:0x%02x.\r\n", type, state);
}

static void sle_transmitter_pair_complete_cbk(uint16_t device_id, const sle_addr_t *addr,
    const bs_sle_pair_info_t *pair_info)
{
    unused(pair_info);
    sample_log_print("pair complete device_id:%02x, addr:%02x:**:**:**:%02x:%02x\r\n",
        device_id, addr->addr[INDEX_0], addr->addr[INDEX_4], addr->addr[INDEX_5]);
}

errcode_t sle_transmitter_cbk_register(void)
{
    bs_sle_callbacks_t cbk = {
        .pair_request_cb = NULL,
        .connect_state_cb = sle_transmitter_connect_state_changed_cbk,
        .member_connect_state_cb = NULL,
        .pair_complete_cb = sle_transmitter_pair_complete_cbk,
        .pair_remove_cb = NULL,
        .discovery_result_cb = sle_transmitter_discovery_result_info_cbk,
        .discovery_state_cb = sle_transmitter_discovery_state_cbk,
        .auto_conn_state_cb = NULL,
    };

    return bs_sle_register_callbacks(0, &cbk);
}

#ifdef CHBA_SUPPORT
errcode_t sle_chba_trans_wake_queue_cb(void)
{
    g_sle_conn_status = SLE_TRANSMISSION_LINK_READY;
    return ERRCODE_SUCC;
}

errcode_t sle_chba_trans_stop_queue_cb(void)
{
    g_sle_conn_status = SLE_TRANSMISSION_LINK_BUSY;
    return ERRCODE_SUCC;
}

errcode_t sle_chba_trans_set_link_up_cb(void)
{
    sample_log_print("chba set link up\r\n");
    return ERRCODE_SUCC;
}

errcode_t sle_chba_trans_set_link_down_cb(void)
{
    sample_log_print("chba set link down\r\n");
    return ERRCODE_SUCC;
}

errcode_t sle_chba_trans_netdev_input_cb(uint8_t *data, uint16_t len)
{
    (void)len;
    if (data != NULL) {
        sample_log_print("ack data: 0x%04x.\r\n", *(uint16_t *)(data + SLE_CHBA_HEADER));
    }
    return ERRCODE_SUCC;
}

errcode_t sle_transmitter_chba_cbk_register(void)
{
    sle_chba_netdev_callbacks_t chba_cbk = {0};
    chba_cbk.stop_queue_cb = sle_chba_trans_stop_queue_cb;
    chba_cbk.wake_queue_cb = sle_chba_trans_wake_queue_cb;
    chba_cbk.set_link_up_cb = sle_chba_trans_set_link_up_cb;
    chba_cbk.set_link_down_cb = sle_chba_trans_set_link_down_cb;
    chba_cbk.netdev_input_cb = sle_chba_trans_netdev_input_cb;
    errcode_t ret = sle_chba_netdev_register_callbacks(&chba_cbk);
    if (ret != ERRCODE_SLE_SUCCESS) {
        sample_log_print("chba cbk reg fail, ret = %x.\r\n", ret);
    }
    return ret;
}
#endif

 /*
 * 使能回调注册，回调成功时注册prot
 */
static void sle_transmitter_state_cbk(uint8_t status)
{
    errcode_t ret = ERRCODE_SLE_SUCCESS;
    sample_log_print("sle stack state: 0x%x.\n", status);
    if (status == SLE_STATE_ENABLED) {
        ret |= sle_transmitter_cbk_register();
#ifdef CHBA_SUPPORT
        ret |= sle_transmitter_chba_cbk_register();
        ret |= sle_chba_netdev_create(CHBA_ROLE_STA, CHBA_MODE);
#endif
        if (ret == ERRCODE_SLE_SUCCESS) {
            bs_sle_start_discovery(SLE_DISCOVERY_PERIPHERAL);
        }
    }
}

errcode_t sle_transmitter_set_addr(uint8_t *addr, uint8_t len)
{
    if (addr == NULL || len != SLE_ADDR_LEN) {
        return ERRCODE_SLE_PARAM_ERR;
    }
    if (memcpy_s(g_local_addr, SLE_ADDR_LEN, addr, SLE_ADDR_LEN) != EOK) {
        return ERRCODE_SLE_MEMCPY_FAIL;
    }
    return ERRCODE_SLE_SUCCESS;
}

errcode_t sle_transmitter_init(void)
{
    errcode_t ret = ERRCODE_SUCC;
    char dev_name[] = SLE_DEMO_TRANSMITTER_DEVICE_NAME;
    uint8_t role = SLE_CONFIG_ROLE_PERIPHERAL;
    bs_sle_stack_state_callbacks_t cbk = {
        .stack_state_cbk = sle_transmitter_state_cbk,
    };
    bs_sle_register_stack_state_callbacks(0, &cbk);

    ret |= bs_config(SLE_CONFIG_COMMON_ROLE, &role, sizeof(uint8_t));
    ret |= bs_config(SLE_CONFIG_ADV_SHORT_NAME, dev_name, sizeof(dev_name));
    ret |= bs_config(SLE_CONFIG_COMMON_LOCAL_ADDR, g_local_addr, SLE_ADDR_LEN);
    ret |= bs_sle_enable();

    return ret;
}

errcode_t sle_transmitter_deinit(void)
{
    g_link_status_cbk = NULL;
    g_sle_conn_status = SLE_TRANSMISSION_LINK_IDLE;
#ifdef CHBA_SUPPORT
    sle_chba_netdev_destroy();
#endif
    errcode_t ret = bs_sle_disable();
    if (ret != ERRCODE_SLE_SUCCESS) {
        sample_log_print("disable sle fail!\n");
    }
    return ret;
}