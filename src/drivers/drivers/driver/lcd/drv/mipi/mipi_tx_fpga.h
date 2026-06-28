/* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. */

#ifndef MIPI_TX_FPGA_H
#define MIPI_TX_FPGA_H

#include "td_base.h"

#ifdef GPU_1A22
#define MIPI_TX_I2C_DEVICE_ID   I2C_BUS_1
#else
#define MIPI_TX_I2C_DEVICE_ID   I2C_BUS_2
#endif

void mipi_tx_fpga_phy_cfg(void);

#endif