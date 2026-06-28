/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides HAL CAN-FD \n
 *
 * History: \n
 * 2023-02-25, Create file. \n
 */
#include "common_def.h"
#include "securec.h"
#include "can_porting.h"
#include "hal_can.h"


#define CAN_DLC_NUM 16
static uint32_t g_real_length[CAN_DLC_NUM] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 12, 16, 20, 24, 32, 48, 64};

uintptr_t g_can_regs = 0;
uintptr_t g_can_fd_std_filter = 0;
uintptr_t g_can_fd_ex_filter = 0;
uintptr_t g_can_fd_rx_fifo_0 = 0;
uintptr_t g_can_fd_rx_fifo_1 = 0;
uintptr_t g_can_fd_tx_fifo = 0;
uintptr_t g_can_fd_tx_event_buffer = 0;
uintptr_t g_can_fd_sram = 0;

errcode_t hal_can_regs_init(void)
{
    if (can_porting_get_std_filter_base_addr() == 0 || can_porting_get_reg_base_addr() == 0 ||
        can_porting_get_ex_filter_base_addr() == 0 || can_porting_get_rx_fifo_0_base_addr() == 0 ||
        can_porting_get_rx_fifo_1_base_addr() == 0 || can_porting_get_tx_fifo_base_addr() == 0 ||
        can_porting_get_tx_event_base_addr() == 0) {
        return ERRCODE_CAN_FD_INVALID_ADDR;
    }
    g_can_regs = can_porting_get_reg_base_addr();
    g_can_fd_sram = can_porting_get_sram_base_addr();
    g_can_fd_std_filter = can_porting_get_std_filter_base_addr();
    g_can_fd_ex_filter = can_porting_get_ex_filter_base_addr();
    g_can_fd_rx_fifo_0 = can_porting_get_rx_fifo_0_base_addr();
    g_can_fd_rx_fifo_1 = can_porting_get_rx_fifo_1_base_addr();
    g_can_fd_tx_fifo = can_porting_get_tx_fifo_base_addr();
    g_can_fd_tx_event_buffer = can_porting_get_tx_event_base_addr();
    return ERRCODE_SUCC;
}

void hal_can_regs_deinit(void)
{
    g_can_regs = 0;
    g_can_fd_std_filter = 0;
    g_can_fd_ex_filter = 0;
    g_can_fd_rx_fifo_0 = 0;
    g_can_fd_rx_fifo_1 = 0;
    g_can_fd_tx_fifo = 0;
    g_can_fd_tx_event_buffer = 0;
}

uint8_t hal_can_get_dlc(uint32_t real_length)
{
    uint8_t dlc = 0;
    for (uint8_t i = 0; i < CAN_DLC_NUM; i++) {
        if (g_real_length[i] >= real_length) {
            dlc = i;
            break;
        }
    }
    return dlc;
}

uint32_t hal_can_get_real_length(uint8_t dlc)
{
    return g_real_length[dlc];
}