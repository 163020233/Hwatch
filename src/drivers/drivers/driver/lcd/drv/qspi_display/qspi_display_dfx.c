/* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved. */

#include "qspi_display_dfx.h"

STATIC qspi_disp_dfx_info g_qspi_disp_dfx_info = {0};

qspi_disp_dfx_info *qspi_disp_dfx_get_info(td_void)
{
    return &g_qspi_disp_dfx_info;
}

td_void qspi_disp_dfx_set_clk_div(td_u32 clk_div)
{
    g_qspi_disp_dfx_info.clk_div = clk_div;
}