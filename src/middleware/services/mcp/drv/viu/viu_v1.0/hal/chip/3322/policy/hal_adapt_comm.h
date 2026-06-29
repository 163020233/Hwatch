/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: head file of crg
 * Author: AuthorNameMagicTag
 * Create: 2025-04-23
 */

#ifndef HAL_ADAPT_REG_H
#define HAL_ADAPT_REG_H

#include "drv_mcp_dependency.h"
#include "vicap_reg.h"

#include "mcp_arch_define.h"
#include "viu_base_define.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* #ifdef __cplusplus */

td_s32 hal_comm_init_vicap_reg(viu_cap_id cap_id);
td_void hal_comm_deinit_vicap_reg(viu_cap_id cap_id);
volatile vicap_regs_type* hal_comm_get_vicap_reg(viu_cap_id cap_id);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
