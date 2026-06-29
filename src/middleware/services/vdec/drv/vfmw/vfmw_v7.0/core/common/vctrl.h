/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __VFMW_CTRL_H__
#define __VFMW_CTRL_H__

#include "vfmw.h"
#include "vfmw_osal.h"
#include "voque.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    td_s32 id;
    td_u32 unid;
    td_void *handle;
    os_atomic ref_cnt;
    vfmw_chan_option opt;

    vfmw_chan_cfg cfg;

    td_u32 start_time;
    td_u32 recv_time;
    td_u32 block_time;
} vctrl_chan;

typedef struct {
    td_s32 dec_mode;
    td_s8 err_threshold;
    td_s32 out_order;
    td_s32 fix_frame_num;
    td_s32 fix_frame_width;
    td_s32 fix_frame_height;
    td_s32 cmp_mode;
    td_s32 frm_layout;
    td_s32 sedldy_en;
    td_s8 mod_lowdelay_en;
    td_s32 tunnel_line_num;
    td_s32 linear_no_gap_en;
} vctrl_dflt;

td_s32 vctrl_init(td_void *args);
td_s32 vctrl_exit(const td_void *args);
td_s32 vctrl_open(td_void);
td_s32 vctrl_close(td_void);
td_s32 vctrl_suspend(const td_void *args);
td_s32 vctrl_resume(const td_void *args);
td_s32 vctrl_get_image(td_s32 chan_id, const td_void *image);
td_s32 vctrl_release_image(td_s32 chan_id, const td_void *rls_info);
td_s32 vctrl_check_image(td_s32 chan_id, const td_void *img);
td_s32 vctrl_control(td_s32 chan_id, td_s32 cmd, td_void *args);
vctrl_chan *vctrl_get_chan(td_s32 chan_id);
vctrl_dflt *vctrl_get_dflt(td_void);
td_void vctrl_set_event_interface(fun_vfmw_event_report evt_rpt);
td_s32 vctrl_get_param_len(td_s32 cmd, td_u32 *len);
td_s32 vctrl_stm_process(td_void);
td_s32 vctrl_syn_process(td_void);
td_s32 vctrl_dec_process(td_void);
td_s32 vctrl_get_require_mem(vfmw_svr_mem_need *mem);
td_s32 vctrl_set_voq_ext_intf(const voq_ext_intf *intf);
td_s32 vctrl_check_eos(td_s32 chan_id, td_s32 *flag);
td_void vctrl_img_2_rls_info(const vfmw_image *img, vfmw_image_rls *rls_info);
td_s32 vctrl_get_max_dec_width_height(td_s32 chan_id, td_u32 *dec_max_width, td_u32 *dec_max_height);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __VFMW_CTRL_H__ */
