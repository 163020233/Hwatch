/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: mipi rx kernel export interface definition.
 * Author: AuthorNameMagicTag
 * Create: 2021-10-18
 */

#ifndef __DRV_MIPI_RX_EXT_H__
#define __DRV_MIPI_RX_EXT_H__

#include "drv_mcp_dependency.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_s32 mipi_rx_mod_init(td_void);
td_void mipi_rx_mod_exit(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
