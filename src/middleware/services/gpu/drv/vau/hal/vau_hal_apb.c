/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: vau hal ctrl settings
 */
#include "vau_hal_apb.h"
#include "platform_core.h"
#include "crg_reg_define.h"
#include "vau_reg_define.h"

static td_float g_cubic_threshold = 0.25f;

PLT_SEC_ITCM_T td_void vau_hal_apb_init_queue(td_void)
{
    // P_NEXT init
    uapi_reg_write(HAL_VAU_REG_PNEXT, 0x0);

    // Performance and CHKSUM init
    uapi_reg_write(HAL_VAU_REG_MISCELLANEOUS, 0xF0000FF);

    // CSC default coef
#ifdef GPU_1A21
    uapi_reg_write(HAL_VAU_REG_BASEADDR + 0x730, 0x256);
    uapi_reg_write(HAL_VAU_REG_BASEADDR + 0x738, 0x399);
    uapi_reg_write(HAL_VAU_REG_BASEADDR + 0x73c, 0x256);
    uapi_reg_write(HAL_VAU_REG_BASEADDR + 0x740, 0xF93);
    uapi_reg_write(HAL_VAU_REG_BASEADDR + 0x744, 0xEEE);
    uapi_reg_write(HAL_VAU_REG_BASEADDR + 0x748, 0x256);
    uapi_reg_write(HAL_VAU_REG_BASEADDR + 0x74c, 0x43D);
    uapi_reg_write(HAL_VAU_REG_BASEADDR + 0x754, 0x9);
#endif
}

td_bool vau_hal_apb_is_idle(td_void)
{
    volatile u_vau_work_state status;

    uapi_reg_read(HAL_VAU_REG_STATUS, status.u32);

    return ((status.bits.vas_work_state == 0) ? TD_TRUE : TD_FALSE);
}

td_u32 vau_hal_apb_get_current_ndoe(td_void)
{
    volatile u_vau_cur_list_addr current_node;

    uapi_reg_read(HAL_VAU_REG_CURRENT_NODE, current_node.u32);

    return current_node.bits.p_last;
}

td_u32 vau_hal_apb_get_int_state(td_void)
{
    volatile u_vau_intstate int_state;
    uapi_reg_read(HAL_VAU_REG_INT_STATE, int_state.u32);
    return int_state.u32;
}

td_void vau_hal_apb_clear_int_state(td_u32 mask)
{
    volatile u_vau_intclr int_clr;
    int_clr.u32 = mask; /* Write directly without read-modify-write to avoid losing new interrupt */
    uapi_reg_write(HAL_VAU_REG_INT_CLR, int_clr.u32);
    return;
}

td_void vau_hal_apb_set_start_node(td_u32 start_node)
{
    volatile u_vau_pnext next_node;

    uapi_reg_read(HAL_VAU_REG_PNEXT, next_node.u32);
    next_node.bits.p_next = start_node;
    uapi_reg_write(HAL_VAU_REG_PNEXT, next_node.u32);
}

td_void vau_hal_apb_set_start_up(td_void)
{
    volatile u_vau_start start_reg;
    uapi_reg_read(HAL_VAU_REG_START, start_reg.u32);
    start_reg.bits.start = 1;
    uapi_reg_write(HAL_VAU_REG_START, start_reg.u32);
}

td_void vau_hal_apb_vector_draw_init(td_void)
{
    u_vau_draw_threshold_cubic cubic;
    cubic.bits.threshold_cubic = g_cubic_threshold; /* 0.25 : default threshold for cubic split */
    uapi_reg_write(HAL_VAU_DRAW_THRESHOLD_CUBIC, cubic.u32);
}

td_void vau_hal_apb_set_cubic_threshold(td_float cubic_threshold)
{
    u_vau_draw_threshold_cubic cubic;

    cubic.bits.threshold_cubic = cubic_threshold;
    uapi_reg_write(HAL_VAU_DRAW_THRESHOLD_CUBIC, cubic.u32);

    g_cubic_threshold = cubic_threshold;
}
