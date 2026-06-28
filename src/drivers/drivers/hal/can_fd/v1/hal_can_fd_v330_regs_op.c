/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides CAN-FD register opreation api \n
 *
 * History: \n
 * 2023-02-25, Create file. \n
 */
#include "securec.h"
#include "common_def.h"
#include "hal_can_fd_v330_regs_op.h"

#define CAN_V330_REJECT_NOMATCH 0x2

void hal_can_fd_regs_configuration_change_disable(uint32_t mode)
{
    cccr_data_t cccr;
    cccr.d32 = ((can_fd_regs_t *)g_can_regs)->cccr;
    cccr.b.fdoe = 1;
    cccr.b.brse = 1;
    /* Enable loopback mode. */
    test_data_t test;
    test.d32 = 0;
    if (mode == 0x1) {
        cccr.b.test = 1;
        test.b.lbck = 1;
    }
    ((can_fd_regs_t *)g_can_regs)->cccr = cccr.d32;
    ((can_fd_regs_t *)g_can_regs)->test = test.d32;
    /* Start normal opreation. */
    cccr.b.cce = 0;
    cccr.b.init = 0;
    ((can_fd_regs_t *)g_can_regs)->cccr = cccr.d32;

    while (((((can_fd_regs_t *)g_can_regs)->cccr) & 1) != 0) {}
}

errcode_t hal_can_fd_set_tx_element(uint32_t put_idx, hal_can_msg_t *msg, uint8_t *data, uint32_t length)
{
    tx_element_config_t tx_element = {0};
    tx_buffer_data_t *tx_ptr = (tx_buffer_data_t *)&(((tx_buffer_t *)g_can_fd_tx_fifo)->element[put_idx]);
    tx_element.d64 = 0;
    tx_element.b.standard_id = msg->std_id;
    if (msg->ide == 1) {
        tx_element.b.extend_id = msg->extend_id;
        tx_element.b.xtd = 1;
    }
    if (msg->rtr == 1) {
        tx_element.b.rtr = 1;
        tx_ptr->config.d64 = tx_element.d64;
        return ERRCODE_SUCC;
    }
    tx_element.b.dlc = hal_can_get_dlc(length);
    tx_element.b.brs = msg->brs;
    tx_element.b.fdf = msg->fdf;
    tx_ptr->config.d64 = tx_element.d64;
    /* RAM四字节对齐访问，必须拷贝两次 */
    uint8_t temp_buf[ELEMENT_MAXIMUM_SIZE] = {0};
    if (memcpy_s((uint8_t *)temp_buf, ELEMENT_MAXIMUM_SIZE, data, length)) {
        return ERRCODE_FAIL;
    }
    (void)memcpy_s((uint8_t *)tx_ptr->data, ELEMENT_MAXIMUM_SIZE, (uint8_t *)temp_buf, ELEMENT_MAXIMUM_SIZE);
    return ERRCODE_SUCC;
}

void hal_can_fd_get_rx_0_element(uint32_t get_idx, hal_can_msg_t *msg, uint8_t *data, uint32_t *length)
{
    rx_fifo_data_t received_msg;
    received_msg = ((rx_fifo_0_t *)g_can_fd_rx_fifo_0)->element[get_idx];
    msg->std_id = received_msg.config.b.standard_id;
    msg->extend_id = received_msg.config.b.extend_id;
    msg->rtr = received_msg.config.b.rtr;
    msg->ide = received_msg.config.b.xtd;
    msg->fdf = received_msg.config.b.fdf;
    msg->brs = received_msg.config.b.brs;
    (void)memcpy_s(data, ELEMENT_MAXIMUM_SIZE, received_msg.data, ELEMENT_MAXIMUM_SIZE);
    *length = hal_can_get_real_length(received_msg.config.b.dlc);
}

void hal_can_fd_get_rx_1_element(uint32_t get_idx, hal_can_msg_t *msg, uint8_t *data, uint32_t *length)
{
    rx_fifo_data_t received_msg;
    received_msg = ((rx_fifo_1_t *)g_can_fd_rx_fifo_1)->element[get_idx];
    msg->std_id = received_msg.config.b.standard_id;
    msg->extend_id = received_msg.config.b.extend_id;
    msg->rtr = received_msg.config.b.rtr;
    msg->ide = received_msg.config.b.xtd;
    msg->fdf = received_msg.config.b.fdf;
    msg->brs = received_msg.config.b.brs;
    (void)memcpy_s(data, ELEMENT_MAXIMUM_SIZE, received_msg.data, ELEMENT_MAXIMUM_SIZE);
    *length = hal_can_get_real_length(received_msg.config.b.dlc);
}

void hal_can_fd_regs_set_filter_configuration(void)
{
    sidfc_data_t sidfc;
    xidfc_data_t xidfc;
    gfc_data_t gfc;
    sidfc.d32 = ((can_fd_regs_t *)g_can_regs)->sidfc;
    xidfc.d32 = ((can_fd_regs_t *)g_can_regs)->xidfc;
    gfc.d32 = ((can_fd_regs_t *)g_can_regs)->gfc;
    sidfc.b.flssa = (g_can_fd_std_filter - g_can_fd_sram) >> CAN_FD_IGNORED_BIT;
    sidfc.b.lss = CONFIG_STANDARD_CAN_FILTER_NUM;
    xidfc.b.flesa = (g_can_fd_ex_filter - g_can_fd_sram) >> CAN_FD_IGNORED_BIT;
    xidfc.b.lse = CONFIG_EXTENDED_CAN_FILTER_NUM;
    gfc.b.anfe = CAN_V330_REJECT_NOMATCH;
    gfc.b.anfs = CAN_V330_REJECT_NOMATCH;
    ((can_fd_regs_t *)g_can_regs)->sidfc = sidfc.d32;
    ((can_fd_regs_t *)g_can_regs)->xidfc = xidfc.d32;
    ((can_fd_regs_t *)g_can_regs)->gfc = gfc.d32;
}

void hal_can_fd_regs_set_rx_fifo_configuration(void)
{
    rxfc_data_t rxf0c;
    rxfc_data_t rxf1c;
    rxf0c.d32 = ((can_fd_regs_t *)g_can_regs)->rxf0c;
    rxf1c.d32 = ((can_fd_regs_t *)g_can_regs)->rxf1c;
    rxf0c.b.fsa = (g_can_fd_rx_fifo_0 - g_can_fd_sram) >> CAN_FD_IGNORED_BIT;
    rxf0c.b.fs = CONFIG_CAN_RX_FIFO0_SIZE;
    rxf0c.b.fom = 1;
    rxf1c.b.fsa = (g_can_fd_rx_fifo_1 - g_can_fd_sram) >> CAN_FD_IGNORED_BIT;
    rxf1c.b.fs = CONFIG_CAN_RX_FIFO1_SIZE;
    rxf1c.b.fom = 1;
    ((can_fd_regs_t *)g_can_regs)->rxf0c = rxf0c.d32;
    ((can_fd_regs_t *)g_can_regs)->rxf1c = rxf1c.d32;
}

void hal_can_fd_regs_set_tx_fifo_configuration(void)
{
    txbc_data_t txbc;
    txefc_data_t txefc;
    txbc.d32 = ((can_fd_regs_t *)g_can_regs)->txbc;
    txefc.d32 = ((can_fd_regs_t *)g_can_regs)->txefc;
    txbc.b.tbsa = (g_can_fd_tx_fifo - g_can_fd_sram) >> CAN_FD_IGNORED_BIT;
    txbc.b.tfqs = CONFIG_CAN_TX_BUFFER_SIZE;
    txbc.b.tfqm = 1;
    txefc.b.efsa = (g_can_fd_tx_event_buffer - g_can_fd_sram) >> CAN_FD_IGNORED_BIT;
    txefc.b.efs = CONFIG_CAN_TX_BUFFER_SIZE;
    ((can_fd_regs_t *)g_can_regs)->txbc = txbc.d32;
    ((can_fd_regs_t *)g_can_regs)->txefc = txefc.d32;
}