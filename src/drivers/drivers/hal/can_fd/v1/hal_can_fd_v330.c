/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides CAN-FD HAL source \n
 *
 * History: \n
 * 2023-02-25, Create file. \n
 */
#include <stdint.h>
#include <stdbool.h>
#include "can.h"
#include "securec.h"
#include "common_def.h"
#include "hal_can_fd_v330_regs_op.h"
#include "hal_can.h"
#include "hal_can_fd_v330.h"

#define HAL_CAN_FD_RX_FIFO_MAX_NUM 0x2
#define FIFO_1_IR_MASK 0x4
#define HAL_CAN_INT_DEFAULT_ENABLE 0x2800077
#define HAL_CAN_INT_DEFAULT_LINE_1 0x2800066
#define HAL_CAN_V330_MAX_INT_NUM 30
#define CAN_V330_MAX_LINE_NUM 2

#define CAN_V330_NSJW_MAX 0x8
#define CAN_V330_DSJW_MAX 0x4
#define CAN_V330_NTSEG1_MAX 0xFF
#define CAN_V330_DTSEG1_MAX 0x1F
#define CAN_V330_SEG1_SUB 0x2
#define CAN_V330_SEG2_SUB 0x1
#define CAN_V330_SYNC_PROG 0x3

typedef void (*hal_can_callback_t)(void);

static uint32_t g_hal_can_fd_opreation_mode = 0;

uint8_t g_hal_can_fd_recv_buffer[HAL_DATA_BUFF_LENGTH] = {0};

static hal_can_int_callback_t g_hal_can_fd_int_callback;

static hal_can_callback_t g_hal_int_handler[CAN_V330_MAX_IRQ_NUM];

static uint32_t get_int0_sts(void)
{
    uint32_t ir = hal_can_fd_regs_get_ir();
    uint32_t ie = hal_can_fd_regs_get_ie();
    uint32_t int_sts = ir & ie;
    uint32_t int_line = hal_can_fd_regs_get_ils();
    return (int_sts & (~int_line));
}

static uint32_t get_int1_sts(void)
{
    uint32_t ir = hal_can_fd_regs_get_ir();
    uint32_t ie = hal_can_fd_regs_get_ie();
    uint32_t int_sts = ir & ie;
    uint32_t int_line = hal_can_fd_regs_get_ils();
    return (int_sts & int_line);
}

static void hal_can_fd_clear_sram(void)
{
    (void)memset_s((uint32_t *)g_can_fd_std_filter, sizeof(std_filter_array_t), 0, sizeof(std_filter_array_t));
    (void)memset_s((uint32_t *)g_can_fd_ex_filter, sizeof(ex_filter_array_t), 0, sizeof(ex_filter_array_t));
    (void)memset_s((uint32_t *)g_can_fd_rx_fifo_0, sizeof(rx_fifo_0_t), 0, sizeof(rx_fifo_0_t));
    (void)memset_s((uint32_t *)g_can_fd_rx_fifo_1, sizeof(rx_fifo_1_t), 0, sizeof(rx_fifo_1_t));
    (void)memset_s((uint32_t *)g_can_fd_tx_fifo,  sizeof(tx_buffer_t), 0, sizeof(tx_buffer_t));
    (void)memset_s((uint32_t *)g_can_fd_tx_event_buffer, sizeof(tx_event_buffer_t), 0, sizeof(tx_event_buffer_t));
}

static void hal_can_fd_send_cancel(uint32_t idx)
{
    uint32_t failed_mask = 1 << idx;
    hal_can_fd_regs_cancel(failed_mask);
    while ((hal_can_fd_regs_get_txbcf() & failed_mask) != failed_mask) {
        ;
    }
}

static void hal_can_fd_get_rx_fifo_0_element(hal_can_msg_t *msg, uint8_t *data, uint32_t *length)
{
    uint32_t get_idx = hal_can_fd_regs_get_rx_fifo_0_get_idx();
    hal_can_fd_get_rx_0_element(get_idx, msg, data, length);
    hal_can_fd_regs_ack_fifo_0_element(get_idx);
}

static void hal_can_fd_get_rx_fifo_1_element(hal_can_msg_t *msg, uint8_t *data, uint32_t *length)
{
    uint32_t get_idx = hal_can_fd_regs_get_rx_fifo_1_get_idx();
    hal_can_fd_get_rx_1_element(get_idx, msg, data, length);
    hal_can_fd_regs_ack_fifo_1_element(get_idx);
}

static void hal_can_fd_get_rx_fifo_0(void)
{
    uint32_t fill_level = hal_can_fd_regs_get_rx_fifo_0_fill_level();
    hal_can_msg_t can_msg = {0};
    uint32_t length = 0;
    for (uint32_t i = 0; i < fill_level; i++) {
        (void)memset_s(g_hal_can_fd_recv_buffer, HAL_DATA_BUFF_LENGTH, 0, HAL_DATA_BUFF_LENGTH);
        hal_can_fd_get_rx_fifo_0_element(&can_msg, g_hal_can_fd_recv_buffer, &length);
        g_hal_can_fd_int_callback(HAL_CAN_FRAME_RECEIVED_INT, &can_msg, g_hal_can_fd_recv_buffer, length);
    }
}

static void hal_can_fd_get_rx_fifo_1(void)
{
    uint32_t fill_level = hal_can_fd_regs_get_rx_fifo_1_fill_level();
    hal_can_msg_t can_msg = {0};
    uint32_t length = 0;
    for (uint32_t i = 0; i < fill_level; i++) {
        (void)memset_s(g_hal_can_fd_recv_buffer, HAL_DATA_BUFF_LENGTH, 0, HAL_DATA_BUFF_LENGTH);
        hal_can_fd_get_rx_fifo_1_element(&can_msg, g_hal_can_fd_recv_buffer, &length);
        g_hal_can_fd_int_callback(HAL_CAN_FRAME_RECEIVED_INT, &can_msg, g_hal_can_fd_recv_buffer, length);
    }
}

static void hal_can_fd_error_passive(void)
{
    g_hal_can_fd_int_callback(HAL_CAN_ERROR_PASSIVE_INT, NULL, NULL, 0);
}

static void hal_can_fd_bus_off(void)
{
    g_hal_can_fd_int_callback(HAL_CAN_BUS_OFF_INT, NULL, NULL, 0);
}

static void hal_can_fd_init_default_int(void)
{
    hal_can_fd_regs_int_enable(HAL_CAN_INT_DEFAULT_ENABLE);
    hal_can_fd_regs_int_select_line_1(HAL_CAN_INT_DEFAULT_LINE_1);
    hal_can_fd_enable_all_int_line();
    g_hal_int_handler[RF0N] = hal_can_fd_get_rx_fifo_0;
    g_hal_int_handler[RF0W] = hal_can_fd_get_rx_fifo_0;
    g_hal_int_handler[RF0F] = hal_can_fd_get_rx_fifo_0;
    g_hal_int_handler[RF1N] = hal_can_fd_get_rx_fifo_1;
    g_hal_int_handler[RF1W] = hal_can_fd_get_rx_fifo_1;
    g_hal_int_handler[RF1F] = hal_can_fd_get_rx_fifo_1;
    g_hal_int_handler[EP] = hal_can_fd_error_passive;
    g_hal_int_handler[BO] = hal_can_fd_bus_off;
}

static void hal_can_fd_set_nbtp(hal_can_global_config_t *global_config)
{
    uint32_t ntseg = global_config->nseg1 + global_config->nseg2;
    uint32_t nbrp = global_config->nseg1 / CAN_V330_NTSEG1_MAX;
    /* seg1 = tseg1 + Sync_Seg(1tq) + 1(default in hardware) */
    ntseg = ntseg / (nbrp + 1);
    uint32_t ntseg1 = global_config->nseg1 / (nbrp + 1);
    uint32_t ntseg2 = ntseg - ntseg1;
    uint32_t nsjw = ntseg2 >= CAN_V330_NSJW_MAX ? CAN_V330_NSJW_MAX : ntseg2;
    hal_can_fd_regs_set_nbtp(nsjw, nbrp, ntseg1 - CAN_V330_SEG1_SUB, ntseg2 - CAN_V330_SEG2_SUB);
}

static void hal_can_fd_set_dbtp(hal_can_global_config_t *global_config)
{
    uint32_t dtseg = global_config->dseg1 + global_config->dseg2;
    uint32_t dbrp = global_config->dseg1 / CAN_V330_DTSEG1_MAX;
    /* seg1 = tseg1 + Sync_Seg(1tq) + 1(default in hardware) */
    dtseg = dtseg / (dbrp + 1);
    uint32_t dtseg1 = global_config->dseg1 / (dbrp + 1);
    uint32_t dtseg2 = dtseg - dtseg1;
    uint32_t dsjw = dtseg2 >= CAN_V330_DSJW_MAX ? CAN_V330_DSJW_MAX : (dtseg2 - 1);
    hal_can_fd_regs_set_tdcr(dtseg1 - 1);
    hal_can_fd_regs_set_dbtp(dsjw, dbrp, dtseg1 - CAN_V330_SEG1_SUB, dtseg2 - CAN_V330_SEG2_SUB);
}

#pragma weak hal_can_init = hal_can_fd_v330_init
errcode_t hal_can_fd_v330_init(hal_can_global_config_t *global_config,
                               hal_can_filter_list_t *std_filter, hal_can_filter_list_t *ex_filter)
{
    errcode_t ret = hal_can_regs_init();
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    hal_can_fd_regs_configuration_change_enable();
    hal_can_fd_clear_sram();
    /* Set baud rate. */
    /* 经计算检验，该IP下所有时钟场景皆可通过内部分频进行配置，无非法值 */
    hal_can_fd_set_nbtp(global_config);
    hal_can_fd_set_dbtp(global_config);
    hal_can_fd_set_buffer_configuration();

    /* 滤波器配置，NULL为合法参数 */
    if (std_filter != NULL) {
        for (uint32_t i = 0; i < std_filter->filter_num; i++) {
            hal_can_fd_regs_set_std_filter(i, std_filter->filters[i]);
        }
    }
    if (ex_filter != NULL) {
        for (uint32_t i = 0; i < ex_filter->filter_num; i++) {
            hal_can_fd_regs_set_ex_filter(i, ex_filter->filters[i]);
        }
    }
    hal_can_fd_regs_set_filter_configuration();

    /* FIFO config. */
    hal_can_fd_regs_set_rx_fifo_configuration();
    hal_can_fd_regs_set_tx_fifo_configuration();
    hal_can_fd_init_default_int();

    /* CAN-FD starts to work */
    g_hal_can_fd_opreation_mode = global_config->mode;
    hal_can_fd_regs_configuration_change_disable(global_config->mode);

    return ERRCODE_SUCC;
}

#pragma weak hal_can_deinit = hal_can_fd_v330_deinit
void hal_can_fd_v330_deinit(void)
{
    hal_can_fd_regs_configuration_disable();
    hal_can_regs_deinit();
}

#pragma weak hal_can_register_int_callback = hal_can_fd_v330_register_int_callback
void hal_can_fd_v330_register_int_callback(hal_can_int_callback_t callback)
{
    g_hal_can_fd_int_callback = callback;
}

#pragma weak hal_can_unregister_int_callback = hal_can_fd_v330_unregister_int_callback
void hal_can_fd_v330_unregister_int_callback(void)
{
    g_hal_can_fd_int_callback = NULL;
}

#pragma weak hal_can_send_msg = hal_can_fd_v330_send_msg
errcode_t hal_can_fd_v330_send_msg(hal_can_msg_t *can_msg, uint8_t *data, uint32_t length)
{
    uint32_t put_idx = hal_can_fd_regs_get_tx_put_idx();
    uint32_t request_mask = 1 << put_idx;
    errcode_t ret = hal_can_fd_set_tx_element(put_idx, can_msg, data, length);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    hal_can_fd_regs_add_request(request_mask);
    /* Normal mode. */
    if (likely(g_hal_can_fd_opreation_mode == 0)) {
        uint64_t cur_time = can_porting_get_cur_time();
        uint64_t end_time = cur_time + can_porting_get_timeout_duration();
        uint32_t txbto = hal_can_fd_regs_get_txbto();
        while (((txbto & request_mask) != request_mask) && (cur_time < end_time)) {
            txbto = hal_can_fd_regs_get_txbto();
            cur_time = can_porting_get_cur_time();
            if (hal_can_fd_regs_get_bus_off_sts() == 1) {
                return ERRCODE_CAN_BUS_OFF;
            }
        }
        txbto = hal_can_fd_regs_get_txbto();
        if ((txbto & request_mask) == request_mask) {
            return ERRCODE_SUCC;
        }
        hal_can_fd_send_cancel(put_idx);
        /* 总线冲突时，取消被pending的发送请求会执行一次发送操作，需判断该发送是否成功。 */
        txbto = hal_can_fd_regs_get_txbto();
        if ((txbto & request_mask) != request_mask) {
            return ERRCODE_CAN_FD_SEND_TIMEOUT;
        }
    }
    /* Loopback mode. */
    return ERRCODE_SUCC;
}

#pragma weak hal_can_receive_msg = hal_can_fd_v330_receive_msg
errcode_t hal_can_fd_v330_receive_msg(uint32_t fifo_index, hal_can_msg_t * receive_msg,
                                      uint8_t *data, uint32_t *length)
{
    uint32_t fill_level = 0;
    if (fifo_index == 0) {
        fill_level = hal_can_fd_regs_get_rx_fifo_0_fill_level();
        if (fill_level != 0) {
            hal_can_fd_get_rx_fifo_0_element(receive_msg, data, length);
        } else {
            return ERRCODE_CAN_FD_RX_FIFO_EMPTY;
        }
    } else {
        fill_level = hal_can_fd_regs_get_rx_fifo_1_fill_level();
        if (fill_level != 0) {
            hal_can_fd_get_rx_fifo_1_element(receive_msg, data, length);
        } else {
            return ERRCODE_CAN_FD_RX_FIFO_EMPTY;
        }
    }
    return ERRCODE_SUCC;
}

#pragma weak hal_can_get_controller_sts = hal_can_fd_v330_get_error_sts
uint32_t hal_can_fd_v330_get_error_sts(void)
{
    uint32_t err_sts = hal_can_fd_regs_get_bus_off_sts();
    if (err_sts == 1) {
        return CAN_STS_BUS_OFF;
    }
    err_sts = hal_can_fd_regs_get_error_passive_sts();
    if (err_sts == 1) {
        return CAN_STS_ERROR_PASSIVE;
    }
    return CAN_STS_ERROR_ACTIVE;
}

#pragma weak hal_can_get_transmit_error_count = hal_can_v330_get_transmit_error_count
uint8_t hal_can_v330_get_transmit_error_count(void)
{
    return hal_can_fd_regs_get_tec();
}

#pragma weak hal_can_get_receive_error_count = hal_can_v330_get_receive_error_count
uint8_t hal_can_v330_get_receive_error_count(void)
{
    return hal_can_fd_regs_get_rec();
}

#pragma weak hal_can_get_rx_sts = hal_can_v330_get_rx_sts
uint8_t hal_can_v330_get_rx_sts(void)
{
    return hal_can_fd_regs_get_rx_sts();
}

#pragma weak hal_can_fd_config_tdc = hal_can_fd_v330_config_tdc
errcode_t hal_can_fd_v330_config_tdc(uint8_t enable, uint8_t offset)
{
    hal_can_fd_regs_configuration_change_enable();
    if (enable) {
        dbtp_data_t dbtp;
        dbtp.d32 = hal_can_fd_regs_get_dbtp();
        uint8_t dbrp = dbtp.b.dbrp + 1;
        uint8_t mtq_offset = offset * dbrp;
        uint8_t mtq_dbit_time = (dbtp.b.dtseg1 + dbtp.b.dtseg2 + CAN_V330_SYNC_PROG) * dbrp;
        if (mtq_offset > mtq_dbit_time) {
            hal_can_fd_regs_configuration_disable();
            return ERRCODE_INVALID_PARAM;
        }
        hal_can_fd_regs_set_tdc_enable(0);
        hal_can_fd_regs_set_tdcr(mtq_offset);
        hal_can_fd_regs_set_tdc_enable(1);
    } else {
        hal_can_fd_regs_set_tdc_enable(0);
    }
    hal_can_fd_regs_configuration_disable();
    return ERRCODE_SUCC;
}

void hal_can_fd_v330_irq_handler(uint32_t int_line)
{
    uint32_t int_sts = 0;
    if (int_line == 0) {
        int_sts = get_int0_sts();
    } else {
        int_sts = get_int1_sts();
    }
    hal_can_fd_regs_clr_ir(int_sts);
    for (int i = 0; i < HAL_CAN_V330_MAX_INT_NUM; i++) {
        if (((int_sts & 1) != 0) && (g_hal_int_handler[i] != NULL)) {
            g_hal_int_handler[i]();
        }
        int_sts = int_sts >> 1;
        if (int_sts == 0) {
            break;
        }
    }
}