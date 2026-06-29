/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "fsp.h"
#include "dbg.h"
#include "dec_ctrl.h"
#include "vfmw_define.h"
#include "dec_ref.h"

/* commom function */
VFMW_ACC_BASE td_s32 dec_ref_make_idx_2_apc(td_u32 apc_size, const td_u32 *apc_to_fsp_id,
    td_u16 list_size, const td_u32 *idx_to_fsp_id, td_u8 *idx_to_apc)
{
    td_u32 lid;
    td_u32 i;
    td_u32 j;

    vfmw_check_ret(apc_to_fsp_id != TD_NULL, DEC_ERR);
    vfmw_check_ret(idx_to_apc != TD_NULL, DEC_ERR);
    vfmw_check_ret(idx_to_fsp_id != TD_NULL, DEC_ERR);

    if ((apc_size == 0) || (list_size == 0)) {
        return DEC_OK;
    }

    for (i = 0; i < list_size; i++) {
        lid = idx_to_fsp_id[i];

        for (j = 0; j < apc_size; j++) {
            if (lid == apc_to_fsp_id[j]) {
                idx_to_apc[i] = (td_u8)j;
                break;
            }
        }

        if (j == apc_size) {
            dprint(PRN_ERROR, "DEC one Idx can't make Apc\n");
        }
    }

    return DEC_OK;
}

VFMW_ACC_BASE td_s32 dec_ref_make_col_pmv_addr(td_s32 chan_id, td_s16 col_fsp_id, UADDR *col_pmv_addr)
{
    dec_chan *chan = dec_ctrl_get_chan(chan_id);
    vfmw_check_ret(chan != TD_NULL, DEC_ERR);
    vfmw_check_ret(col_pmv_addr != TD_NULL, DEC_ERR);

    if (col_fsp_id != INVALID_IDX) {
        *col_pmv_addr = fsp_get_pmv(chan_id, col_fsp_id);
        if (*col_pmv_addr == 0) {
            dprint(PRN_ERROR, "DEC PmvAddr return from FSP is 0 for fsp_logic_id %d\n", col_fsp_id);
            return DEC_ERR;
        }
    } else {
        *col_pmv_addr = 0;
    }

    return DEC_OK;
}

VFMW_ACC_BASE td_s32 dec_ref_make_col_pic_apc(td_u32 apc_size, const td_u32 *apc_to_fsp_id,
                                              td_s16 col_fsp_id, td_u8 *col_pic_apc)
{
    td_u32 i;

    vfmw_check_ret(apc_to_fsp_id != TD_NULL, DEC_ERR);
    vfmw_check_ret(col_pic_apc != TD_NULL, DEC_ERR);

    if (col_fsp_id == INVALID_IDX) {
        return DEC_OK;
    }

    for (i = 0; i < apc_size; i++) {
        if (col_fsp_id == (td_s32)apc_to_fsp_id[i]) {
            *col_pic_apc = (td_u8)i;
            break;
        }
    }

    if (i == apc_size) {
        dprint(PRN_ERROR, "DEC can't make ColPicApc\n");
    }

    return DEC_OK;
}

/* interface */
td_s32 dec_ref_check_ref_dec_state(td_s32 chan_id, const dps_ref_info *dps_ref)
{
    td_u16 ref_size;
    td_u32 *fsp_id = TD_NULL;
    fsp_dec_state dec_state;
    td_u16 i;

    vfmw_check_ret(dps_ref != TD_NULL, DEC_ERR);

    ref_size = dps_ref->ref_size;
    fsp_id = dps_ref->ref_fsp_id;
    vfmw_check_ret(fsp_id != TD_NULL, DEC_ERR);
    for (i = 0; i < ref_size; i++) {
        dec_state.data = (td_u8)fsp_get_dec(chan_id, (td_s32)fsp_id[i]);

        if (dec_state.bits.top_state == FSP_DEC_STATE_ERR ||
            dec_state.bits.bot_state == FSP_DEC_STATE_ERR) {
            return DEC_ERR;
        }
    }

    return DEC_OK;
}

VFMW_ACC_BASE td_s32 dec_ref_mark_pic_ref_state(td_s32 chan_id, const dps_ref_info *dps_ref)
{
    td_s32 (*fsp_set_ref[2])(td_s32, td_s32) = {fsp_clear_dec_ref, fsp_set_dec_ref};   /* 2 :a number */
    td_u16 ref_size;
    td_u32 *fsp_id = TD_NULL;
    td_u8 ref_state[FSP_FRAME_NUM] = { 0 };
    td_u8 ref;
    td_s8 i;

    vfmw_check_ret(dps_ref != TD_NULL, DEC_ERR);

    ref_size = dps_ref->ref_size;
    fsp_id = dps_ref->ref_fsp_id;
    vfmw_check_ret(fsp_id != TD_NULL, DEC_ERR);

    for (i = 0; i < ref_size; i++) {
        if (((td_s32)fsp_id[i] != INVALID_IDX) && (fsp_id[i] < FSP_FRAME_NUM)) {
            ref_state[fsp_id[i]] = 1;
        }
    }

    for (i = 0; i < FSP_FRAME_NUM; i++) {
        ref = ref_state[i];
        if (ref != fsp_get_dec_ref(chan_id, i)) {
            fsp_set_ref[ref](chan_id, i);
        }
    }

    return DEC_OK;
}

STATIC td_s32 dec_ref_find_replace_ref(td_s32 chan_id, td_s32 apc_index,
    const dps_ref_info *dps_ref, td_u32 rplc_poc_thrs)
{
#define poc_distance(a, b) (((a) > (b)) ? ((a) - (b)) : ((b) - (a)))

    td_s32 i;
    td_s32 distance = VFMW_INT_MAX;
    td_s32 index = -1;
    td_s32 dest_poc;
    fsp_logic *logic = TD_NULL;

    vfmw_check_ret(dps_ref != TD_NULL, DEC_ERR);

    dest_poc = dps_ref->apc_poc[apc_index];

    for (i = 0; i < dps_ref->ref_size; i++) {
        if (i != apc_index && poc_distance(dps_ref->apc_poc[i], dest_poc) < distance) {
            logic = fsp_get_logic(chan_id, (td_s32)dps_ref->ref_fsp_id[i]);
            if ((logic != TD_NULL) && (logic->dec_fs != TD_NULL) && (logic->err_flag == 0)) {
                index = i;
                distance = poc_distance(dps_ref->apc_poc[i], dest_poc);
            }
        }
    }

    if (index < 0 || (rplc_poc_thrs > 0 && distance > (td_s32)rplc_poc_thrs)) {
        dprint(PRN_ERROR, "Can Not Find ReplaceRef\n");
        return -1;
    }

    return (td_s32)dps_ref->ref_fsp_id[index];
}

VFMW_ACC_BASE STATIC td_bool dec_ref_if_cur_ref(const dps_ref_info *dps_ref, td_u32 fsp_id)
{
    td_u32 i;
    td_u16 cur_ref_size;
    td_u32 *cur_ref_fsp = TD_NULL;

    vfmw_check_ret(dps_ref != TD_NULL, DEC_ERR);

    cur_ref_size = dps_ref->cur_ref_size;
    cur_ref_fsp = dps_ref->cur_ref_fsp_id;

    if ((td_s32)fsp_id == INVALID_IDX ||
        cur_ref_size == 0 ||
        cur_ref_fsp == TD_NULL) {
        return TD_FALSE;
    }

    for (i = 0; i < cur_ref_size; i++) {
        if (cur_ref_fsp[i] == fsp_id) {
            return TD_TRUE;
        }
    }

    return TD_FALSE;
}

STATIC td_void dec_ref_get_disp_addr(td_u16 i, const td_u32 *fsp_id,
    td_s32 chan_id, const dps_ref_info *dps_ref, td_u32 rplc_poc_thrs)
{
    td_s32 tmp_logic_id;
    fsp_logic *curr_logic = fsp_get_logic(chan_id, (td_s32)dps_ref->cur_fsp_id);
    td_s32 rplc_id = dec_ref_find_replace_ref(chan_id, (td_s32)i, dps_ref, rplc_poc_thrs);
    UADDR *fsp_phy_addr = dps_ref->apc_2_fsp_phy_addr;
    UADDR *fsp_disp_addr = dps_ref->apc_2_fsp_disp_addr;

    vfmw_check(fsp_id != TD_NULL);

    if (curr_logic != TD_NULL && rplc_id < 0) {
        curr_logic->ref_err = 1;
    }

    tmp_logic_id = (rplc_id < 0) ? (td_s32)fsp_id[i] : rplc_id;
    fsp_phy_addr[i] = fsp_get_phy(chan_id, tmp_logic_id);
    if (fsp_disp_addr != TD_NULL) {
        fsp_disp_addr[i] = fsp_get_disp_addr(chan_id, tmp_logic_id);
    }
}

VFMW_ACC_BASE STATIC td_s32 dec_ref_get_apc_phy_addr(td_s32 chan_id,
    const dps_ref_info *dps_ref, td_u32 rplc_poc_thrs)
{
    td_u16 i;
    td_s32 ret = DEC_OK;
    td_u16 apc_size;
    td_u32 *fsp_id = TD_NULL;
    UADDR *fsp_phy_addr = TD_NULL;
    UADDR *fsp_disp_addr = TD_NULL;
    fsp_logic *logic = TD_NULL;

    vfmw_check_ret(dps_ref != TD_NULL, DEC_ERR);

    apc_size = dps_ref->ref_size;
    fsp_id = dps_ref->ref_fsp_id;
    fsp_phy_addr = dps_ref->apc_2_fsp_phy_addr;
    fsp_disp_addr = dps_ref->apc_2_fsp_disp_addr;

    for (i = 0; i < apc_size; i++) {
        vfmw_check_ret(fsp_id != TD_NULL, DEC_ERR);
        logic = fsp_get_logic(chan_id, (td_s32)fsp_id[i]);
        if (logic == TD_NULL) {
            continue;
        }

        if ((logic->err_flag == 0) || (dps_ref->apc_poc == TD_NULL) ||
            dec_ref_if_cur_ref(dps_ref, fsp_id[i]) != TD_TRUE) {
            fsp_phy_addr[i] = fsp_get_phy(chan_id, (td_s32)fsp_id[i]);
            if (fsp_disp_addr != TD_NULL) {
                fsp_disp_addr[i] = fsp_get_disp_addr(chan_id, (td_s32)fsp_id[i]);
            }
        } else {
            dec_ref_get_disp_addr(i, fsp_id, chan_id, dps_ref, rplc_poc_thrs);
        }

        if ((fsp_phy_addr[i] == 0) && (fsp_id[i] != dps_ref->cur_fsp_id) &&
            (fsp_disp_addr != TD_NULL && fsp_disp_addr[i] == 0)) {
            dprint(PRN_ERROR, "DEC phy_addr return from FSP is 0 for fsp_logic_id %d\n", fsp_id[i]);
            ret = DEC_ERR;
        }
    }

    return ret;
}

VFMW_ACC_BASE td_s32 dec_ref_make_pic_ref_info(td_s32 chan_id,
    const dps_ref_info *dps_ref, td_u32 rplc_poc_thrs)
{
    td_s32 ret;
    td_u16 i;
    td_u16 single_frame_ref_size;
    td_u32 *fsp_id_single = TD_NULL;
    UADDR *fsp_phy_addr_single = TD_NULL;
    UADDR *fsp_pmv_addr_single = TD_NULL;
    UADDR *fsp_disp_addr_single = TD_NULL;
    td_u32 *fsp_id = TD_NULL;
    UADDR *fsp_phy_addr = TD_NULL;

    vfmw_check_ret(dps_ref != TD_NULL, DEC_ERR);

    single_frame_ref_size = dps_ref->cur_ref_size;
    fsp_id_single = dps_ref->cur_ref_fsp_id;
    fsp_phy_addr_single = dps_ref->cur_ref_fsp_phy_addr;
    fsp_pmv_addr_single = dps_ref->cur_ref_fsp_pmv_addr;
    fsp_disp_addr_single = dps_ref->cur_ref_fsp_disp_addr;
    fsp_id = dps_ref->ref_fsp_id;
    fsp_phy_addr = dps_ref->apc_2_fsp_phy_addr;

    if ((fsp_id == TD_NULL) || (fsp_phy_addr == TD_NULL)) {
        dprint(PRN_ERROR, "%s fsp_id or fsp_phy_addr is null\n", __func__);
        return DEC_ERR;
    }

    ret = dec_ref_get_apc_phy_addr(chan_id, dps_ref, rplc_poc_thrs);

    for (i = 0; i < single_frame_ref_size; i++) {
        vfmw_check_ret(fsp_id_single != TD_NULL, DEC_ERR);
        if ((td_s32)fsp_id_single[i] != INVALID_IDX) {
            if (fsp_phy_addr_single != TD_NULL) {
                fsp_phy_addr_single[i] = fsp_get_phy(chan_id, (td_s32)fsp_id_single[i]);
            }

            if (fsp_disp_addr_single != TD_NULL) {
                fsp_disp_addr_single[i] = fsp_get_disp_addr(chan_id, (td_s32)fsp_id_single[i]);
            }

            if (fsp_pmv_addr_single != TD_NULL) {
                fsp_pmv_addr_single[i] = fsp_get_pmv(chan_id, (td_s32)fsp_id_single[i]);
            }
        }
    }

    return ret;
}

td_s32 dec_ref_update_ref_cnt(td_s32 chan_id, const dps_ref_info *dps_ref, td_s32 flag)
{
    td_s32 i;
    td_u16 apc_size;
    td_u32 *fsp_id = TD_NULL;
    td_u16 single_ref_size;
    td_u32 *fsp_id_single = TD_NULL;

    vfmw_check_ret(dps_ref != TD_NULL, DEC_ERR);

    apc_size = dps_ref->ref_size;
    fsp_id = dps_ref->ref_fsp_id;
    single_ref_size = dps_ref->cur_ref_size;
    fsp_id_single = dps_ref->cur_ref_fsp_id;

    if (fsp_id == TD_NULL) {
        dprint(PRN_ERROR, "%s fsp_id is null\n", __func__);
        return DEC_ERR;
    }

    if (single_ref_size == 0) {
        for (i = 0; i < (td_s32)apc_size; i++) {
            if ((td_s32)fsp_id[i] != INVALID_IDX &&
                fsp_id[i] != dps_ref->cur_fsp_id) {
                fsp_set_ref_cnt(chan_id, (td_s32)fsp_id[i], flag);
            }
        }
    } else {
        for (i = 0; i < (td_s32)single_ref_size; i++) {
            vfmw_check_ret(fsp_id_single != TD_NULL, DEC_ERR);
            if ((td_s32)fsp_id_single[i] != INVALID_IDX &&
                fsp_id_single[i] != dps_ref->cur_fsp_id) {
                fsp_set_ref_cnt(chan_id, (td_s32)fsp_id_single[i], flag);
            }
        }
    }

    return DEC_OK;
}

VFMW_ACC_BASE td_s32 dec_ref_get_pic_store_type(td_s32 chan_id, const dps_ref_info *dps_ref)
{
    td_u16 apc_size;
    td_u32 *fsp_id = TD_NULL;
    td_u8 *fsp_pic_type = TD_NULL;
    td_u16 i;

    vfmw_check_ret(dps_ref != TD_NULL, DEC_ERR);

    apc_size = dps_ref->ref_size;
    fsp_id = dps_ref->ref_fsp_id;
    fsp_pic_type = dps_ref->ref_pic_type;

    if (fsp_pic_type == TD_NULL) {
        return DEC_OK;
    }

    for (i = 0; i < apc_size; i++) {
        vfmw_check_ret(fsp_id != TD_NULL, DEC_ERR);
        if ((td_s32)fsp_id[i] != INVALID_IDX) {
            fsp_pic_type[i] = (td_u8)fsp_get_store_type(chan_id, (td_s32)fsp_id[i]);
        }
    }

    return DEC_OK;
}

VFMW_ACC_BASE td_s32 dec_ref_make_slice_ref_info(td_s32 chan_id,
    const dps_info *dps, const dec_hal_ops *ops)
{
    td_s32 i;

    vfmw_check_ret(dps != TD_NULL, DEC_ERR);
    vfmw_check_ret(ops != TD_NULL, DEC_ERR);
    vfmw_check_ret(dps->pic_param != TD_NULL, DEC_ERR);

    if (ops->make_slice_ref == TD_NULL) {
        return DEC_OK;
    }

    for (i = 0; (i < dps->slice_num) && (i < DPS_SLC_NUM); i++) {
        ops->make_slice_ref(chan_id, dps->pic_param, dps->slice_list[i]);
    }

    return DEC_OK;
}
