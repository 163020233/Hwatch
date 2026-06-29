/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description: mipi tx header file.
 *
 * Create: 2022-04-14
 */
#ifndef __MIPI_TX_H__
#define __MIPI_TX_H__

#include "stdio.h"
#include "soc_mipi_tx.h"
#include "mipi_tx_hal.h"

typedef enum {
    MIPI_TX_LANE_STATE_NON_IDLE = 0,
    MIPI_TX_LANE_STATE_IDLE,
    MIPI_TX_LANE_STATE_SDLP,
    MIPI_TX_LANE_STATE_ULPS,
    MIPI_TX_LANE_STATE_INAVLID
} mipi_tx_lane_status;

void mipi_tx_set_lp_clk_en(uint8_t lp_clk_en);
uint8_t mipi_tx_get_lp_clk_en(void);
uint32_t mipi_tx_get_lane_num(const int16_t lane_id[], uint32_t lane_id_len);

uint32_t mipi_tx_set_colorbar(colorbar_orien_mode orien, colorbar_input_mode mode);
uint32_t mipi_tx_unset_colorbar(void);

uint32_t mipi_tx_cfg_show(void);

void mipi_tx_drv_get_phy_cfg(void);

void mipi_tx_reset(void);

void mipi_tx_drv_set_partial_refresh(combo_dev_cfg_t *dev_cfg);

uint32_t mipi_tx_partial_refresh(sync_info_t *dev_sync, uint32_t pixel_clk);

uint32_t mipi_tx_proc_show(void);

mipi_tx_lane_status mipi_tx_get_lane_state(const mipi_tx_lane_state_t *state);

uint32_t mipi_tx_set_cmd_video_mode(td_u32 module);
#endif
