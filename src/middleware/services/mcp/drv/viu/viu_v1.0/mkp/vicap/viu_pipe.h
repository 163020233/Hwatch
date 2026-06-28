/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: viu module header
 * Author: AuthorNameMagicTag
 * Create: 2025-06-30
 */

#ifndef __VIU_PIPE__
#define __VIU_PIPE__

#include "drv_viu_struct.h"
#include "viu_extern.h"

#define viu_check_vir_pipe_id_return(pipe_id)               \
do {                                                        \
    td_s32 ret_tmp = viu_check_vir_pipe_id(pipe_id);      \
    if (ret_tmp != TD_SUCCESS) {                          \
        return ret_tmp;                                     \
    }                                                       \
} while (0)

#define viu_check_phy_pipe_id_return(pipe_id)               \
do {                                                        \
    td_s32 ret_tmp = viu_check_phy_pipe_id(pipe_id);      \
    if (ret_tmp != TD_SUCCESS) {                          \
        return ret_tmp;                                     \
    }                                                       \
} while (0)

td_s32 viu_check_pipe_id(td_s32 pipe_id);
td_s32 viu_check_phy_pipe_id(td_s32 pipe_id);
td_s32 viu_check_vir_pipe_id(td_s32 pipe_id);

td_s32 viu_pipe_set_work_mode(td_u8 pipe_id, td_bool pipe_online, td_bool chn_online);
td_s32 viu_pipe_get_work_mode(td_u8 pipe_id, td_bool *pipe_online, td_bool *chn_online);

td_s32 viu_pipe_create(td_s32 pipe_id, const ext_viu_pipe_attr *pipe_attr, td_void *unique);
td_s32 viu_pipe_destroy(td_s32 pipe_id);

td_s32 viu_pipe_set_attr(td_s32 pipe_id, const ext_viu_pipe_attr *pipe_attr);
td_s32 viu_pipe_get_attr(td_s32 pipe_id, ext_viu_pipe_attr *pipe_attr);

td_s32 viu_pipe_start(td_s32 pipe_id);
td_s32 viu_pipe_stop(td_s32 pipe_id);

td_s32 viu_pipe_get_status(td_s32 pipe_id, ext_viu_pipe_status *pipe_status);

td_s32 viu_pipe_set_fpn_attr(td_s32 pipe_id, ext_viu_fpn_attr *fpn_attr);
td_s32 viu_pipe_get_fpn_attr(td_s32 pipe_id, ext_viu_fpn_attr *fpn_attr);

td_s32 viu_pipe_set_nrx_param(td_s32 pipe_id, ext_viu_pipe_nrx_param *nr_x_param);
td_s32 viu_pipe_get_nrx_param(td_s32 pipe_id, ext_viu_pipe_nrx_param *nr_x_param);

/* 内核态导出函数     */
td_bool viu_pipe_is_online_mode(td_s32 pipe_id);
td_bool viu_pipe_is_ppu_online(td_s32 pipe_id);

td_s32 viu_pipe_get_wdr_attr(td_s32 pipe_id, ext_viu_pipe_wdr_attr *wdr_attr);
td_s32 viu_pipe_get_hdr_attr(td_s32 pipe_id, ext_viu_pipe_hdr_attr *hdr_attr);

td_s32 viu_pipe_get_isp_frame_int_attr(td_s32 pipe_id, ext_isp_frame_interrupt_attr *frame_int_attr);

td_s32 viu_pipe_set_bind(td_u8 pipe_id, td_s32 dst_mod, td_s32 dst_id);

#endif /* __VIU_PIPE__ */

