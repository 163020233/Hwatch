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
#include "at_adapter.h"
#ifdef CHBA_SUPPORT
#include "sle_chba_manager.h"
#include "sle_chba_transmition.h"
#endif

#define TEST_PACKET_LEN     1500
#define TEST_PRINT_INTERVAL 200
#define TEST_PACKET_NUM     20000
#define SLE_CHBA_HEADER     1
#define TEST_ACK_DATA_LEN   16
#define TEST_SEND_DELAY     1

static uint16_t g_rece_data_num = 0;
static uint32_t g_first_data_time = 0;
static uint16_t g_rece_data_ack = 0;
static bool  g_pressure_flag = false;
void sle_transmitter_set_send_data_delay_ms(uint8_t delay_ms);
errcode_t sle_transmitter_set_addr(uint8_t *addr, uint8_t len);
errcode_t sle_receiver_set_addr(uint8_t *addr, uint8_t len);
errcode_t bt_sdk_at_str_to_addr(uint8_t *str, uint8_t *addr);
uint32_t platform_get_ms_passed_new(void);

typedef struct {
    uint32_t        para_map;
    uint8_t         trans_mode;
    const uint8_t*  addr;
} sle_data_trns_at_addr_t;

const at_para_parse_syntax_t sle_data_trns_at_addr_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_FIX_CASE,
        .offset = offsetof(sle_data_trns_at_addr_t, addr)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL,
        .offset = offsetof(sle_data_trns_at_addr_t, trans_mode)
    },
};

void sle_data_trans_receive_data_cbk(uint8_t *data, uint16_t len)
{
    unused(data);
    if (g_rece_data_num == 0) {
        g_first_data_time = platform_get_ms_passed_new();
    }
    g_rece_data_num++;
    if (g_rece_data_num % TEST_PRINT_INTERVAL == 0) {
        uint32_t current_time = platform_get_ms_passed_new();
        uint32_t total_time = current_time - g_first_data_time;
        uint16_t speed = (g_rece_data_num * len * 8) / total_time;
        at_adapter_print("[receive data] packet num: %d, packet len: %d, total_time: %dms, speed: %dkbps\n",
            g_rece_data_num, len, total_time, speed);
        g_rece_data_num = 0;

#ifdef CHBA_SUPPORT
        /* 接收端回复低频数据 */
        uint8_t *chba_data = (uint8_t *)osal_vmalloc(SLE_CHBA_HEADER + TEST_ACK_DATA_LEN);
        if (chba_data == NULL) {
            at_adapter_print("chba data malloc fail.\r\n");
            return;
        }
        *chba_data = 0;
        *(uint16_t *)(chba_data + SLE_CHBA_HEADER) = g_rece_data_ack;
        if (sle_chba_netdev_driver_send(chba_data, SLE_CHBA_HEADER + TEST_ACK_DATA_LEN) != ERRCODE_SUCC) {
            at_adapter_print("ack data send fail.\r\n");
            osal_vfree(chba_data);
            return;
        }
        g_rece_data_ack++;
        osal_vfree(chba_data);
#endif
    }
}

at_ret_t at_sle_receiver_init(const sle_data_trns_at_addr_t *args)
{
    g_rece_data_num = 0;
    sle_receiver_callback_t cbk = {0};
    cbk.receive_data_cb = sle_data_trans_receive_data_cbk;
    sle_receiver_reg_callback(&cbk);
    uint8_t peer_addr[SLE_ADDR_LEN] = { 0 };
    if (bt_sdk_at_str_to_addr((uint8_t *)args->addr, peer_addr) != ERRCODE_SLE_SUCCESS) {
        return AT_RET_PARSE_PARA_ERROR;
    }
    sle_receiver_set_addr(peer_addr, SLE_ADDR_LEN);
    if (sle_receiver_init() !=ERRCODE_SLE_SUCCESS) {
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

void sle_data_trans_send_data(sle_transmission_link_status_t status)
{
    if (status == SLE_TRANSMISSION_LINK_READY) {
        uint8_t *trans_data = (uint8_t *)osal_vmalloc(TEST_PACKET_LEN);
        while (g_pressure_flag) {
            errcode_t ret = sle_transmitter_send_data(trans_data, TEST_PACKET_LEN);
            if (ret == ERRCODE_SLE_STATUS_ERR) {
                at_adapter_print("[trans data] link is idle, transmission stop.\n");
                break;
            } else if (ret != ERRCODE_SLE_SUCCESS) {
                continue;
            }
        }
        osal_vfree(trans_data);
    }
}

at_ret_t at_sle_transmitter_init(const sle_data_trns_at_addr_t *args)
{
    uint8_t local_addr[SLE_ADDR_LEN] = { 0 };
    if (bt_sdk_at_str_to_addr((uint8_t *)args->addr, local_addr) != ERRCODE_SLE_SUCCESS) {
        return AT_RET_PARSE_PARA_ERROR;
    }
    g_pressure_flag = true;
    sle_transmitter_set_send_data_delay_ms(TEST_SEND_DELAY);
    sle_transmitter_reg_callback(sle_data_trans_send_data);
    sle_transmitter_set_addr(local_addr, SLE_ADDR_LEN);
    if (sle_transmitter_init() !=ERRCODE_SLE_SUCCESS) {
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

at_ret_t at_sle_transmitter_deinit(void)
{
    g_pressure_flag = false;
    if (sle_transmitter_deinit() !=ERRCODE_SLE_SUCCESS) {
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

at_ret_t at_sle_receiver_deinit(void)
{
    if (sle_receiver_deinit() !=ERRCODE_SLE_SUCCESS) {
        return AT_RET_SYNTAX_ERROR;
    }
    g_rece_data_num = 0;
    g_first_data_time = 0;
    g_rece_data_ack = 0;
    return AT_RET_OK;
}

const at_cmd_entry_t at_sle_transmitter_register_parse_table[] = {
    {
        "SLETRANSMITTERINIT",
        1,
        0,
        sle_data_trns_at_addr_syntax,
        NULL,
        (at_set_func_t)at_sle_transmitter_init,
        NULL,
        NULL,
    },
    {
        "SLERECEIVERINIT",
        2,
        0,
        sle_data_trns_at_addr_syntax,
        NULL,
        (at_set_func_t)at_sle_receiver_init,
        NULL,
        NULL,
    },
    {
        "SLETRANSMITTERDEINIT",
        3,
        0,
        NULL,
        at_sle_transmitter_deinit,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "SLERECEIVERDEINIT",
        4,
        0,
        NULL,
        at_sle_receiver_deinit,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
};

#define AT_BASE_FUNC_NUM \
    (sizeof(at_sle_transmitter_register_parse_table) / sizeof(at_sle_transmitter_register_parse_table[0]))
#define EXT_AT_BASE_CMD_MAX_LEN 128

void at_sle_data_trans_cmd_register(void)
{
    uint32_t ret = ERRCODE_SUCC;

    ret = uapi_at_cmd_table_register(at_sle_transmitter_register_parse_table,
                                     (uint32_t)AT_BASE_FUNC_NUM, EXT_AT_BASE_CMD_MAX_LEN);
    if (ret != 0) {
        uapi_at_report_to_single_channel(0, "uapi_at_register_cmd \r\n");
    }
}