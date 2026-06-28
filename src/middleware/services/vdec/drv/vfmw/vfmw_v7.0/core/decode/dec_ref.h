/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __DEC_REF_H__
#define __DEC_REF_H__

#include "dec_base.h"
#include "dec_hal.h"
#include "dps.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_s32 dec_ref_check_ref_dec_state(td_s32 chan_id, const dps_ref_info *dps_ref);
td_s32 dec_ref_mark_pic_ref_state(td_s32 chan_id, const dps_ref_info *dps_ref);
td_s32 dec_ref_make_pic_ref_info(td_s32 chan_id, const dps_ref_info *dps_ref, td_u32 rplc_poc_thrs);
td_s32 dec_ref_get_pic_store_type(td_s32 chan_id, const dps_ref_info *dps_ref);
td_s32 dec_ref_make_slice_ref_info(td_s32 chan_id, const dps_info *dps, const dec_hal_ops *ops);
td_s32 dec_ref_make_col_pmv_addr(td_s32 chan_id, td_s16 col_fsp_id, UADDR *col_pmv_addr);
td_s32 dec_ref_make_idx_2_apc(td_u32 apc_size, const td_u32 *apc_to_fsp_id,
    td_u16 list_size, const td_u32 *idx_to_fsp_id, td_u8 *idx_to_apc);
td_s32 dec_ref_make_col_pic_apc(td_u32 apc_size, const td_u32 *apc_to_fsp_id,
    td_s16 col_fsp_id, td_u8 *col_pic_apc);
td_s32 dec_ref_update_ref_cnt(td_s32 chan_id, const dps_ref_info *dps_ref, td_s32 flag);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
