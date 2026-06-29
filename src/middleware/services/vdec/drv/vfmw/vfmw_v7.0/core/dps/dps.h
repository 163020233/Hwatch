/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __DPS_H__
#define __DPS_H__

#include "vfmw.h"
#include "vfmw_osal.h"
#include "dbg.h"
#include "dps_ops.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define DPS_OK  0
#define DPS_ERR 1

#define DPS_CHAN_NULL 0
#define DPS_CHAN_OPEN 1

#define DPS_CHAN_NUM VFMW_CHAN_NUM
#define DPS_MEM_NUM  (64 * MAX_DPS_NUM)
#define DPS_SLC_NUM  2048

#define INVALID_IDX (-1)

typedef enum {
    DPS_MOD_NULL = 0,
    DPS_MOD_OPEN,
    DPS_MOD_INIT,
    DPS_MOD_EXIT,
    DPS_MOD_MAX
} dps_mod_state;

typedef enum {
    DPS_IDLE = 0,
    DPS_MAKE,
    DPS_WAIT,
    DPS_DEC,
    DPS_MAX
} dps_state;

/* ldy mode */
typedef enum {
    LDY_IDLE = 0,
    LDY_SED,
    LDY_SLC,
    LDY_MULTI_SLC,
    LDY_MAX
} ldy_mode;

/* sed only state */
typedef enum {
    SED_IDLE = 0,
    SED_TRY,
    SED_SUCC,
    SED_MAX
} sed_state;

typedef struct {
    td_void *addr[DPS_MEM_NUM];
    td_s32 len[DPS_MEM_NUM];
} dps_mem;

typedef enum {
    AIR_STATE_FIRST,
    AIR_STATE_CONTINUE,
    AIR_STATE_OVER,
    AIR_STATE_MAX
} air_state;

typedef struct {
    td_u32 air_version;
    td_u32 slice_id;
    td_u32 height_ofs;
    td_u32 air_logic_id;
    air_state air_sta;
} dps_air_info;

typedef struct {
    td_s32 unid;
    td_s32 dps_id;
    dps_state state;
    td_s32 logic_id;
    td_u32 bit_depth;
    td_s32 pic_struct;
    td_s32 ctb_size;
    td_u32 frm_width;
    td_u32 frm_height;
    td_u32 disp_width;
    td_u32 disp_height;
    td_u32 top_offset;
    td_u32 bottom_offset;
    td_s32 ldy_mode;
    td_s32 sed_state;
    td_s32 is_slc_ldy;
    td_s32 pic_ctb_num;
    td_s32 sed_ctb_num;
    td_s32 slice_num;
    td_s32 is_frm_cpy;
    td_s32 make_num;
    dps_ops *ops;
    td_void *pic_param;
    td_void *slice_head;
    td_void *slice_tail;
    td_void *slice_list[DPS_SLC_NUM];
    td_void *mtdt_addr;
    td_u32 mtdt_size;
    td_u32 pic_coding_type;
    td_s32 apply_grain;
    td_u32 frame_mbs_only;
    td_u32 allow_intrabc;
    dps_air_info air_dec_info;
    td_u32 process_cnt;         // current DPS config msg pool count in multi slc ldy mode
    td_bool is_rcv_lst_slc;     // whether the last slice is received. is set based on new pic of next frame.
    td_bool is_get_new_slice;   // avoid repeatedly sending the same slice to VDH message pool in multi slc ldy mode
    td_void *last_slice;        // the last slice received
} dps_info;

typedef struct {
    td_bool has_node_make;
} dps_fault_check_info;

typedef struct {
    td_s32 vid_std;
    td_s32 mem_mode;
    td_s32 max_num;
    td_s32 max_slc_num;
    vfmw_frm_alloc_type alloc_type;
    vfmw_adpt_type adpt_type;
} dps_cfg_info;

typedef struct {
    td_s32 chan_id;
    td_u32 unid;
    td_s32 state;
    td_void *handle;

    td_u32 make_num;
    td_u32 recv_try;
    td_u32 recv_num;
    td_u32 rels_num;
    td_u32 sed_try;
    td_u32 sed_succ;

    td_s32 tail;
    td_s32 head;

    dps_cfg_info cfg;
    dps_mem mem;
    dps_ops *ops;
    dps_info dps[MAX_DPS_NUM];
} dps_chan_info;

typedef struct {
    td_u32 reserve1;

    os_sema sema;
    os_lock lock;
    dps_mod_state state;

    td_u32 ref_cnt;
    td_s32 ser_mode;
    td_s32 dps_num;
    td_u32 ctx_size;
    dps_chan_info *chan[DPS_CHAN_NUM];

    td_u32 reserve2;
    td_u32 no_dps;
} dps_entry_info;

dps_entry_info *dps_get_entry(td_void);
dps_chan_info *dps_get_chan(td_s32 chan_id);

td_s32 dps_open(td_void);
td_s32 dps_close(td_void);
td_s32 dps_init(td_void);
td_s32 dps_exit(td_void);
td_s32 dps_create(td_s32 chan_id);
td_s32 dps_reset(td_s32 chan_id);
td_s32 dps_destroy(td_s32 chan_id);
td_s32 dps_config(td_s32 chan_id, const td_void *args);
td_s32 dps_get_node(td_s32 chan_id, dps_info **dps);
td_s32 dps_reset_node(td_s32 chan_id, dps_info *dps);
td_s32 dps_insert(td_s32 chan_id, dps_info *dps);
td_s32 dps_receive(td_s32 chan_id, dps_info **dps);
td_s32 dps_release(td_s32 chan_id, const dps_info *dps);
td_s32 dps_have_idle(td_s32 chan_id);
td_s32 dps_have_wait(td_s32 chan_id);
td_s32 dps_set_pic_param(td_s32 chan_id, td_void *ctx, dps_info *dps);
td_s32 dps_set_slice_param(td_s32 chan_id, td_void *ctx, dps_info *dps);
td_s32 dps_delete_slice(td_s32 chan_id, td_void *ctx, dps_info *dps);
td_s32 dps_check_valid(td_s32 chan_id, const dps_info *dps);
td_s32 dps_release_seg(td_s32 chan_id, const dps_info *dps);
td_s32 dps_get_fsp_logic_id(td_s32 chan_id, const dps_info *dps);
td_s32 dps_get_pic_struct(td_s32 chan_id, const dps_info *dps);
td_s32 dps_get_ref_info(td_s32 chan_id, dps_info *dps, dps_ref_info *dps_ref);
td_s32 dps_get_mem_need_size(vfmw_svr_mem_need *mem);
td_void *dps_alloc_mem(td_s32 chan_id, td_s32 size);
td_s32 dps_get_unfinish_dps_num(td_s32 chan_id);
td_s32 dps_get_info(td_s32 chan_id, const td_void *args);
td_void dps_sema_down(td_void);
td_void dps_sema_up(td_void);
dps_entry_info *dps_get_entry(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __DPS_H__ */
