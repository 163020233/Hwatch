/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description: low power control interface header
 * Author:
 * Create:
 */
#ifndef LIB_PUBLIC_LOW_POWER_CONTROL_H
#define LIB_PUBLIC_LOW_POWER_CONTROL_H

#include "core.h"

typedef enum {
    // !< XIP control
    LOW_POWER_XIP_ENABLE = 0,   // !< Not support now.
    LOW_POWER_XIP_DISABLE = 1,  // !< Disable xip funciton, it is dangereous, make sure all run in ram.

    // !< Power control
    LOW_POWER_AUXLDO0_POWER_ON = 2,       // !< AUXLDO0 power on.
    LOW_POWER_AUXLDO0_POWER_OFF = 3,      // !< AUXLDO0 power off.
    LOW_POWER_AUXLDO1_POWER_ON = 4,       // !< AUXLDO1 power on.
    LOW_POWER_AUXLDO1_POWER_OFF = 5,      // !< AUXLDO1 power off.
    LOW_POWER_AUXLDO2_POWER_ON = 6,       // !< AUXLDO2 power on, xip flash is here.
    LOW_POWER_AUXLDO2_POWER_OFF = 7,      // !< AUXLDO2 power off, xip flash is here.
    LOW_POWER_CODEC_POWER_ON = 8,         // !< CODEC power on.
    LOW_POWER_CODEC_POWER_OFF = 9,        // !< CODEC power off.
    LOW_POWER_BT_POWER_UP_AND_RUN = 10,   // !< Power up and run bt
    LOW_POWER_BT_POWER_OFF = 11,          // !< Power off bt.
    LOW_POWER_DSP_POWER_UP_AND_RUN = 12,  // !< Only run dsp 0
    LOW_POWER_DSP1_POWER_UP_AND_RUN = 13, // !< Run all cores, should run dsp0 before the operation.
    LOW_POWER_DSP_POWER_OFF = 14,         // !< DSP0 power off.
    LOW_POWER_DSP1_POWER_OFF = 15,        // !< DSP1 power off.

    // !< Clock control
    LOW_POWER_CLK_OUT0_ON = 30,           // !< enable clk out0.
    LOW_POWER_CLK_OUT0_OFF = 31,          // !< disable clk out0.
    LOW_POWER_CLK_OUT1_ON = 32,           // !< enable clk out1.
    LOW_POWER_CLK_OUT1_OFF = 33,          // !< disable clk out1.
    LOW_POWER_CLK_OUT2_ON = 34,           // !< enable clk out2.
    LOW_POWER_CLK_OUT2_OFF = 35,          // !< disable clk out2.
    LOW_POWER_RESERVED_1 = 36,            // !< Reserved.
    LOW_POWER_RESERVED_2 = 37,            // !< Reserved.
    LOW_POWER_I2S_CLK_SOURCE = 38,        // !< Config I2s clock source.
    LOW_POWER_I2S_CLK_SOURCE_CODEC = 39,  // !< Config I2s clock source selece codec.
    LOW_POWER_I2S_PLL_OFF = 40,           // !< No need I2S clock, can be remove pll_i2s_veto.

    LOW_POWER_END,
} low_power_control_e;

typedef enum {
    CODEC_UNUSED   = 0,
    CODEC_POWEROFF = 1,
    CODEC_POWERON  = 2,
} codec_state_e;

// used for smart wear run mode
typedef enum {
    PRODUCT_AW_MODE = 0,
    PRODUCT_GT_MODE,
    PRODUCT_INVALID_MODE,
} product_mode_e;

// used for smart wear diff board type
typedef enum {
    BOARD_TYPE_BEFORE_V3 = 0,
    BOARD_TYPE_V3,
    BOARD_TYPE_INVALID,
} board_type_e;

/**
  * @brief  Get product AW&GT mode
  * @return aw or gt mode
  */
product_mode_e get_aw_gt_mode(void);

/**
  * @brief  Get board type v3 or before v3
  * @return boart type
  */
board_type_e get_board_type(void);

/**
 * @brief  Control DSP power on.
 */
void low_power_dsp_power_on(void);

/**
 * @brief  Return codec is power on or off.
 * @return true is power on and false is power off.
 */
bool low_power_codec_is_poweron(void);

/**
 * @}
 */
#endif
