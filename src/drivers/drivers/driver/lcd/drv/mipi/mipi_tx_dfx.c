/* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. */

#include "mipi_tx_dfx.h"
#include "systick.h"
#include "mipi_tx.h"
#include "mipi_tx_hal.h"

STATIC mipi_tx_dfx_info g_mipi_tx_dfx_info = {0};

mipi_tx_dfx_info *mipi_tx_dfx_get_info(td_void)
{
    return &g_mipi_tx_dfx_info;
}

td_void uapi_mipi_tx_dfx_set_exit_ulps_cnt_ext(mipi_tx_exit_ulps_ext_opt opt)
{
    if (opt == MIPI_TX_EXIT_ULPS_CNT_ADD) {
        g_mipi_tx_dfx_info.exit_ulps_cnt_ext++;
        return;
    }

    if (opt == MIPI_TX_EXIT_ULPS_CNT_SUB) {
        if (g_mipi_tx_dfx_info.exit_ulps_cnt_ext > 0) {
            g_mipi_tx_dfx_info.exit_ulps_cnt_ext--;
        } else {
            mipi_tx_err("mipi exit_ulps_cnt_ext is already 0!\n");
        }
    }
}

td_void mipi_tx_dfx_record_cmd_in_data_trans(mipi_tx_cmd_info cmd_info)
{
    uint32_t index = (g_mipi_tx_dfx_info.cmd_in_data_trans_cnt - 1) % CMD_IN_DATA_TRANS_CMD_MAX_NUM;
    g_mipi_tx_dfx_info.cmd_in_data_trans[index].cmd = cmd_info.cmd;
    g_mipi_tx_dfx_info.cmd_in_data_trans[index].cmd_type = cmd_info.cmd_type;
}

td_void uapi_mipi_tx_suspend_status_err_det(void)
{
    td_bool is_err = TD_FALSE;
    uint32_t ulps_exit_cnt = mipi_tx_hal_get_ulps_exit_count();
    if (ulps_exit_cnt != 0) {
        g_mipi_tx_dfx_info.sus_sta_err_cnt++;
        is_err = TD_TRUE;
    }

    if (g_mipi_tx_dfx_info.exit_ulps_cnt_ext != 0) {
        g_mipi_tx_dfx_info.sus_sta_err_cnt_ext++;
        is_err = TD_TRUE;
    }
    if (is_err) {
#ifdef MIPI_TX_STA_DET_DFX
        if ((g_mipi_tx_dfx_info.sus_sta_err_cnt <= MIPI_TX_STA_ERR_PRINT_MAX_CNT) &&
            (g_mipi_tx_dfx_info.sus_sta_err_cnt_ext <= MIPI_TX_STA_ERR_PRINT_MAX_CNT)) {
            mipi_tx_err("mipi sus err! ulps_exit_cnt %d ext %d \n", ulps_exit_cnt,
                g_mipi_tx_dfx_info.exit_ulps_cnt_ext);
        }
#endif
    }
}

td_void uapi_mipi_tx_ulps_proc(void)
{
#ifdef MIPI_TX_DFX_READ_REG
    mipi_tx_lane_state_t clk_lane = {};
    mipi_tx_lane_state_t data_lane0 = {};
    uint32_t ulps_exit_cnt = mipi_tx_hal_get_ulps_exit_count();
    mipi_tx_hal_get_lane_state(MIPI_TX_LANE_DATA0, &data_lane0);
    mipi_tx_hal_get_lane_state(MIPI_TX_LANE_CLK, &clk_lane);
    uint32_t clk_stat = mipi_tx_get_lane_state(&clk_lane);
    uint32_t data0_stat = mipi_tx_get_lane_state(&data_lane0);
    mipi_proc_print("ulps_exit_cnt %d clk_stat: %d data0_stat: %d\r\n", ulps_exit_cnt, clk_stat, data0_stat);
#endif
    mipi_tx_dfx_info *dfx_info = mipi_tx_dfx_get_info();
    mipi_proc_print("exit_ulps_cnt_ext %d\r\n", dfx_info->exit_ulps_cnt_ext);
}