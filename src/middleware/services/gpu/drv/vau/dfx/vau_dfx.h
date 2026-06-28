/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: dfx manage
 */

#ifndef DRV_GRAPHICS_VAU_DFX_H
#define DRV_GRAPHICS_VAU_DFX_H

#include "gpu_dfx.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

enum {
    VAU_DFX_MASK_NONE             = (0 << 0),
    VAU_DFX_MASK_NODE_ALL         = (1 << 0),
    VAU_DFX_MASK_NODE_NOZERO      = (1 << 1),
    VAU_DFX_MASK_PATH             = (1 << 2),
    VAU_DFX_MASK_SURFACE          = (1 << 3),
};

typedef struct {
    td_float create_cost;
    td_float destroy_cost;
    td_float blit_cost;
    td_float compose_cost;
    td_float submit_cost;
    td_float wait_for_done_cost;
    td_float wait_all_done_cost;
} dfx_time_info;

typedef struct {
    td_u32 create_cnt;
    td_u32 cancel_cnt;
    td_u32 destroy_cnt;
    td_u32 blit_cnt;
    td_u32 compose_cnt;
    td_u32 submit_cnt;
    td_u32 submit_usr_cnt;
    td_u32 wait_for_done_cnt;
    td_u32 wait_all_done_cnt;
    td_u32 submit_from_int;
    td_u32 submit_from_user;
    td_u32 isr_cnt;
    td_u32 tasklet_enter_cnt;
    td_u32 tasklet_exit_cnt;
    td_u32 execute_node_cnt;
} dfx_func_cnt;

typedef struct {
    td_u32 isr_zero;
    td_u32 isr_node_end;
    td_u32 isr_timeout;
    td_u32 isr_bus_err;
    td_u32 isr_list_end;
    td_u32 isr_disp_done;
    td_u32 isr_tunl_done;
    td_u32 isr_conflict;
    td_u32 isr_draw_err;
    td_u32 isr_et_oversize;
    td_u32 isr_draw_oversize;
    td_u32 isr_dcmp_cfg_err;
    td_u32 isr_dcmp_bs_err;
} dfx_isr_cnt;

typedef struct {
    dfx_time_info time_info;
    dfx_func_cnt func_cnt;
    dfx_isr_cnt isr_cnt;
} dfx_func_info;

typedef struct {
    td_u32 suspend_cnt;
    td_u32 resume_cnt;
    td_u32 power_err_cnt;
    td_u32 last_handle;
    dfx_func_info func_info;
} vau_dfx_info;

vau_dfx_info *vau_dfx_get_info(td_void);
td_bool vau_dfx_check_mask(td_u32 mask);
td_void vau_dfx_print_reg(td_void);
td_s32 uapi_vau_dfx_proc_read(int argc, const char *argv[]);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* DRV_GRAPHICS_VAU_DFX_H */
