/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
* Description: decoder
* Author: AuthorNameMagicTag
* Create: 2019-04-22
 */
#ifndef __VIR_PP_H__
#define __VIR_PP_H__

#include "vfmw.h"
#include "vfmw_proc.h"
#include "drv_video_ext.h"
#include "vfmw_osal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef VFMW_FSP_MAX_FRAME_NUM
#define VPP_MAX_PRIVATE_NODE_NUM VFMW_FSP_MAX_FRAME_NUM
#define VPP_PVR_MAX_LIST_LEN     VFMW_FSP_MAX_FRAME_NUM
#else
#define VPP_MAX_PRIVATE_NODE_NUM 32
#define VPP_PVR_MAX_LIST_LEN     32
#endif

typedef enum {
    INVALID_STATE = 0,
    START_STATE = 1,
    STOP_STATE,
    STATE_BUTT
} vir_pp_chan_state;

typedef enum {
    THREA_IDLE = 0,
    THREA_RUNNING = 1,
    THREA_SLEEP,
    THREA_BUTT
} vpp_thread_state;

typedef td_s32 (*fn_vpp_get_image)(td_s32 chan_id, td_void *image);
typedef td_s32 (*fn_vpp_rls_image)(td_s32 chan_id, const td_void *image);
typedef td_s32 (*fn_vpp_chk_image)(td_s32 chan_id, td_void *image);
typedef td_s32 (*fn_vpp_chk_eos)(td_s32 chan_id, td_s32 *flag);

typedef struct {
    td_s32 image_id;
    td_s32 image_id_1;
    vfmw_vid_std std;
    td_bool is_3d;
    UADDR phy_addr;
    td_u32 width;
    td_u32 height;
    UADDR phy_addr_1;
    UADDR meta_data_addr;
    td_u32 share_index;
} vpp_pvr_image_info;

typedef struct {
    vfmw_private_frm_info vdec_private;
    td_bool is_use;
} vpp_private_block;

typedef struct {
    td_bool created;
    td_s32 chan_id;
    td_s32 failed_time;
    vir_pp_chan_state state;
    vfmw_vpp_work_mode work_mode;
    vfmw_std_ext std_ext;
    vfmw_frm_alloc_type alloc_type;
    ext_drv_source source;
    fun_vfmw_event_report event_report;
    td_s32 receive_try;
    td_s32 receive_ok;
    td_s32 release_try;
    td_s32 release_ok;
    td_s32 err_thr;
    vpp_private_block *vpp_private_node[VPP_MAX_PRIVATE_NODE_NUM];
    vfmw_dec_mode dec_mode;
    td_u8 single_i_frame;
    vfmw_image last_image;
    td_u64 evt_map;
    td_bool frm_fence_enable;
    td_s32 chan_lowly_enable;
} vpp_chan;

typedef struct {
    td_bool inited;
    os_sema sema;
    os_event event;
    td_void *thread;
    td_u32 init_time;
    td_u32 wait_time;
    fn_vpp_get_image get_image;
    fn_vpp_rls_image rls_image;
    fn_vpp_chk_image chk_image;
    fn_vpp_chk_eos chk_eos;
    vpp_thread_state thread_state;
    vpp_chan *chan[VFMW_CHAN_NUM];
    td_s32 bypass_chan;
    td_bool print_frm;
    td_bool print_buf;

    /* for test */
    td_u32 no_recv_frm;
    td_u32 no_rels_frm;
} vpp_entry;

typedef struct {
    fn_vpp_get_image get_image;
    fn_vpp_rls_image rls_image;
    fn_vpp_chk_image chk_image;
    fn_vpp_chk_eos chk_eos;
} vpp_init_param;

typedef struct {
    vfmw_vpp_work_mode work_mode;
    fun_vfmw_event_report event_report;
} vpp_create_param;

/* ----------------------------EXTERNAL FUNCTION------------------------------- */
vpp_entry *vpp_get_entry(td_void);
vpp_chan *vpp_get_chan(td_s32 chan_id);
td_s32 vpp_init(vpp_init_param *param);
td_s32 vpp_deinit(td_void);
td_s32 vpp_create(td_s32 chan_id, vpp_create_param *param);
td_s32 vpp_destroy(td_s32 chan_id);
td_s32 vpp_start(td_s32 chan_id);
td_s32 vpp_stop(td_s32 chan_id);
td_s32 vpp_reset(td_s32 chan_id);
td_s32 vpp_config(td_s32 chan_id, vfmw_chan_cfg *cfg);
td_s32 vpp_receive_frame(td_s32 chan_id, td_void *frame);
td_s32 vpp_release_frame(td_s32 chan_id, const td_void *frame);
td_s32 vpp_get_chan_status(td_s32 chan_id, vfmw_chan_info *status);
td_s32 vpp_set_evt_map(td_s32 chan_id, td_u64 evt_map);
td_s32 vpp_check_frame_state(td_s32 chan_id, const td_void *frame, td_u32 size);
td_void vpp_sema_down(td_void);
td_void vpp_sema_up(td_void);
td_void vpp_thread_wakeup(td_s32 chan_id);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
