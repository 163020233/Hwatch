/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: source file of crg
 * Author: AuthorNameMagicTag
 * Create: 2025-04-23
 */

#include "drv_mcp_dependency.h"

#include "mcp_arch_define.h"
#include "viu_base_define.h"
#include "viu_public.h"

#include "vicap_reg.h"
#include "viu_hal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* #ifdef __cplusplus */

#define VIU_CAP_REGS_ADDR           (0x52065000)

volatile vicap_regs_type *g_viu_cap_reg[ISP_VICAP_IP_NUM] = { TD_NULL };

td_s32 hal_comm_init_vicap_reg(viu_cap_id cap_id)
{
    if (g_viu_cap_reg[cap_id] == TD_NULL) {
        g_viu_cap_reg[cap_id] =
            (volatile vicap_regs_type *)osal_ioremap_nocache(VIU_CAP_REGS_ADDR, sizeof(vicap_regs_type));
        if (g_viu_cap_reg[cap_id] == TD_NULL) {
            viu_err_trace("remap vicap reg fail, line: %d. \n", __LINE__);
            return TD_FAILURE;
        }
    }
    viu_hal_set_init_vicap_reg(g_viu_cap_reg[cap_id]);

    return TD_SUCCESS;
}

td_void hal_comm_deinit_vicap_reg(viu_cap_id cap_id)
{
    if (g_viu_cap_reg[cap_id] != TD_NULL) {
        mcp_osal_iounmap((void *)g_viu_cap_reg[cap_id], sizeof(vicap_regs_type));
        g_viu_cap_reg[cap_id] = TD_NULL;
    }
}

volatile vicap_regs_type* hal_comm_get_vicap_reg(viu_cap_id cap_id)
{
    return g_viu_cap_reg[cap_id];
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
