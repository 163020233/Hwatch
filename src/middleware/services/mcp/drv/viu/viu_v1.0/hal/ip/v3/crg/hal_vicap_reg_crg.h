/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: head file of crg
 * Author: AuthorNameMagicTag
 * Create: 2025-04-23
 */

#ifndef __HAL_VICAP_REG_CRG_H__
#define __HAL_VICAP_REG_CRG_H__

#include "drv_mcp_dependency.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* #ifdef __cplusplus */

td_void vicap_reg_set_pmc_isp0_reset(td_bool reset);
td_void vicap_reg_set_peri_isp0_cken(td_bool clken);
td_void vicap_reg_get_peri_isp0_cken(td_bool *clken);
td_void vicap_reg_set_ppc_clk(td_u32 clk_sel);

td_void vicap_reg_isp_cksel(td_s32 cap_id, td_u32 clk_sel);
td_void vicap_reg_bus_clk_en(td_bool clk_en);
td_void vicap_reg_bus_reset(td_bool reset);
td_void vicap_reg_ppc_clk_en(td_bool clk_en);
td_void vicap_reg_ppc_reset(td_bool reset);

td_void vicap_reg_mmu_reset(td_s32 cap_id, td_bool reset);
td_void vicap_reg_mmu_clken(td_s32 cap_id, td_bool clken);

td_void vicap_reg_isp_pclk_sel(td_void);
td_void vicap_reg_isp0_aclk_sel(td_void);
td_void vicap_reg_isp1_aclk_sel(td_void);

td_void vicap_reg_ppc_chn_clk_en(td_s32 cap_id, td_bool clk_en);
td_void vicap_reg_isp_clk_en(td_s32 cap_id, td_bool clk_en);
td_void vicap_reg_isp_core_reset(td_s32 cap_id, td_bool reset);
td_void vicap_reg_isp_cfg_reset(td_s32 cap_id, td_bool reset);

td_void vicap_reg_get_online_status(td_s32 cap_id, td_u32 *value);
td_void vicap_reg_proc_online_sel(td_s32 cap_id, td_bool online);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
