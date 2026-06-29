/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __DPS_OPS_H__
#define __DPS_OPS_H__

#include "vfmw.h"
#include "vfmw_osal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    td_u16 ref_size;
    td_u16 cur_ref_size;
    td_u32 *ref_fsp_id;
    td_s32 *apc_poc;
    UADDR *apc_2_fsp_phy_addr;
    UADDR *apc_2_fsp_disp_addr;
    td_u32 *cur_ref_fsp_id;
    UADDR *cur_ref_fsp_phy_addr;
    UADDR *cur_ref_fsp_disp_addr;
    UADDR *cur_ref_fsp_pmv_addr;
    td_u32 cur_fsp_id;
    td_u8 *ref_pic_type;
} dps_ref_info;

typedef struct {
    td_s32 vid_std;

    td_s32 (*get_max_slice_num) (td_void);
    td_s32 (*get_pic_param_size) (td_void);
    td_s32 (*get_slice_param_size)(td_void);

    td_s32 (*make_dps_pic) (td_void *syn_ctx, td_void *param);
    td_s32 (*make_dps_slice) (td_void *syn_ctx, td_void *param, td_void *prev_param);

    td_s32 (*get_ref_info) (const td_void *param, dps_ref_info *dps_ref);
    td_s32 (*release_seg) (td_s32 chan_id, const td_void *param);
} dps_ops;

dps_ops *dps_get_ops(td_s32 vid_std);
td_s32 dps_get_max_param_size(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __DPS_OPS_H__ */
