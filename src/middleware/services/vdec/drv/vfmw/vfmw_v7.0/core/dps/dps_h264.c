/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "dps_h264.h"
#include "msg_h264.h"
#include "h264.h"
#include "vfmw_define.h"

static const td_u32 g_quant_org[4] = { 0x10101010, 0x10101010, 0x10101010, 0x10101010 }; /* 4 is 2x2 */
static const td_u32 g_quant8_org[16] = { /* 16 is 4x4 */
    0x10101010, 0x10101010, 0x10101010, 0x10101010,
    0x10101010, 0x10101010, 0x10101010, 0x10101010,
    0x10101010, 0x10101010, 0x10101010, 0x10101010,
    0x10101010, 0x10101010, 0x10101010, 0x10101010
};

STATIC td_void dps_h264_write_quant_matrix_default(td_u32 *quant_matrix)
{
    td_s32 i, idx;

    for (i = 0; i < 24; i++) { /* 24 :a number */
        quant_matrix[i] = (((g_quant_org[i % 4] >> 24) & 0xff) << 24) | /* 4 :a number 24 :a number 24 :a number */
                          (((g_quant_org[i % 4] >> 8) & 0xff) << 16) | /* 4 :a number 8 :a number 16 :a number */
                          (((g_quant_org[i % 4] >> 16) & 0xff) << 8) | /* 4 :a number 16 :a number 8 :a number */
                          ((g_quant_org[i % 4]) & 0xff); /* 4 :a number */
    }

    for (i = 0; i < 16; i++) { /* 16 :a number */
        idx = 24 + 2 * i; /* 24 :a number 2 :a number */
        /* 2 :a number 16 :a number 16 :a number 24 :a number */
        quant_matrix[idx] = (((g_quant8_org[(2 * i + 1) % 16] >> 16) & 0xff) << 24) |
                            /* 2 :a number 16 :a number 16 :a number */
                            (((g_quant8_org[(2 * i + 1) % 16]) & 0xff) << 16) |
                            /* 2 :a number 16 :a number 16 :a number 8 :a number */
                            (((g_quant8_org[(2 * i) % 16] >> 16) & 0xff) << 8) |
                            /* 2 :a number 16 :a number */
                            ((g_quant8_org[(2 * i) % 16]) & 0xff);

        idx = 24 + 2 * i + 1; /* 24 :a number 2 :a number */
        /* 2 :a number 16 :a number 24 :a number 24 :a number */
        quant_matrix[idx] = (((g_quant8_org[(2 * i + 1) % 16] >> 24) & 0xff) << 24) |
                            /* 2 :a number 16 :a number 8 :a number 16 :a number */
                            (((g_quant8_org[(2 * i + 1) % 16] >> 8) & 0xff) << 16) |
                            /* 2 :a number 16 :a number 24 :a number 8 :a number */
                            (((g_quant8_org[(2 * i) % 16] >> 24) & 0xff) << 8) |
                            /* 2 :a number 16 :a number 8 :a number */
                            ((g_quant8_org[(2 * i) % 16] >> 8) & 0xff);
    }
}

STATIC td_void dps_h264_write_quant_matrix_from_pps(const h264_pps *pps, td_u32 *quant_matrix)
{
    td_s32 i, idx;

    for (i = 0; i < 24; i++) { /* 24 :a number */
        /* 4 :a number 4 :a number 24 :a number 24 :a number */
        quant_matrix[i] = (((pps->qmatrix4x4[i / 4][i % 4] >> 24) & 0xff) << 24) |
                        /* 4 :a number 4 :a number 8 :a number 16 :a number */
                        (((pps->qmatrix4x4[i / 4][i % 4] >> 8) & 0xff) << 16) |
                        /* 4 :a number 4 :a number 16 :a number 8 :a number */
                        (((pps->qmatrix4x4[i / 4][i % 4] >> 16) & 0xff) << 8) |
                        /* 4 :a number 4 :a number */
                        ((pps->qmatrix4x4[i / 4][i % 4]) & 0xff);
    }

    for (i = 0; i < 16; i++) { /* 16 :a number */
        idx = 24 + 2 * i; /* 24 :a number 2 :a number */
        /* 8 :a number 2 :a number 16 :a number 16 :a number 24 :a number */
        quant_matrix[idx] = (((pps->qmatrix8x8[i / 8][(2 * i + 1) % 16] >> 16) & 0xff) << 24) |
                            /* 8 :a number 2 :a number 16 :a number 16 :a number */
                            (((pps->qmatrix8x8[i / 8][(2 * i + 1) % 16]) & 0xff) << 16) |
                            /* 8 :a number 2 :a number 16 :a number 16 :a number 8 :a number */
                            (((pps->qmatrix8x8[i / 8][(2 * i) % 16] >> 16) & 0xff) << 8) |
                            /* 8 :a number 2 :a number 16 :a number */
                            ((pps->qmatrix8x8[i / 8][(2 * i) % 16]) & 0xff);

        idx = 24 + 2 * i + 1; /* 24 :a number 2 :a number */
        /* 8 :a number 2 :a number 16 :a number 24 :a number 24 :a number */
        quant_matrix[idx] = (((pps->qmatrix8x8[i / 8][(2 * i + 1) % 16] >> 24) & 0xff) << 24) |
                            /* 8 :a number 2 :a number 16 :a number 8 :a number 16 :a number */
                            (((pps->qmatrix8x8[i / 8][(2 * i + 1) % 16] >> 8) & 0xff) << 16) |
                            /* 8 :a number 2 :a number 16 :a number 24 :a number 8 :a number */
                            (((pps->qmatrix8x8[i / 8][(2 * i) % 16] >> 24) & 0xff) << 8) |
                            /* 8 :a number 2 :a number 16 :a number 8 :a number */
                            ((pps->qmatrix8x8[i / 8][(2 * i) % 16] >> 8) & 0xff);
    }
}

STATIC td_void dps_h264_write_quant_matrix_from_sps(const h264_sps *sps, td_u32 *quant_matrix)
{
    td_s32 i, idx;

    for (i = 0; i < 24; i++) { /* 24 :a number */
        /* 4 :a number 4 :a number 24 :a number 24 :a number */
        quant_matrix[i] = (((sps->qmatrix4x4[i / 4][i % 4] >> 24) & 0xff) << 24) |
                        /* 4 :a number 4 :a number 8 :a number 16 :a number */
                        (((sps->qmatrix4x4[i / 4][i % 4] >> 8) & 0xff) << 16) |
                        /* 4 :a number 4 :a number 16 :a number 8 :a number */
                        (((sps->qmatrix4x4[i / 4][i % 4] >> 16) & 0xff) << 8) |
                        /* 4 :a number 4 :a number */
                        ((sps->qmatrix4x4[i / 4][i % 4]) & 0xff);
    }

    for (i = 0; i < 16; i++) { /* 16 :a number */
        idx = 24 + 2 * i; /* 24 :a number 2 :a number */
        /* 8 :a number 2 :a number 16 :a number 16 :a number 24 :a number */
        quant_matrix[idx] = (((sps->qmatrix8x8[i / 8][(2 * i + 1) % 16] >> 16) & 0xff) << 24) |
                            /* 8 :a number 2 :a number 16 :a number 16 :a number */
                            (((sps->qmatrix8x8[i / 8][(2 * i + 1) % 16]) & 0xff) << 16) |
                            /* 8 :a number 2 :a number 16 :a number 16 :a number 8 :a number */
                            (((sps->qmatrix8x8[i / 8][(2 * i) % 16] >> 16) & 0xff) << 8) |
                            /* 8 :a number 2 :a number 16 :a number */
                            ((sps->qmatrix8x8[i / 8][(2 * i) % 16]) & 0xff);

        idx = 24 + 2 * i + 1; /* 24 :a number 2 :a number */
        /* 8 :a number 2 :a number 16 :a number 24 :a number 24 :a number */
        quant_matrix[idx] = (((sps->qmatrix8x8[i / 8][(2 * i + 1) % 16] >> 24) & 0xff) << 24) |
                            /* 8 :a number 2 :a number 16 :a number 8 :a number 16 :a number */
                            (((sps->qmatrix8x8[i / 8][(2 * i + 1) % 16] >> 8) & 0xff) << 16) |
                            /* 8 :a number 2 :a number 16 :a number 24 :a number 8 :a number */
                            (((sps->qmatrix8x8[i / 8][(2 * i) % 16] >> 24) & 0xff) << 8) |
                            /* 8 :a number 2 :a number 16 :a number 8 :a number */
                            ((sps->qmatrix8x8[i / 8][(2 * i) % 16] >> 8) & 0xff);
    }
}

STATIC td_void dps_h264_write_quant_matrix(const h264_ctx *ctx, td_u32 *quant_matrix)
{
    const h264_sps *sps = &ctx->cur_sps;
    const h264_pps *pps = &ctx->cur_pps;

    if (sps->seq_scaling_matrix_present_flag == 0 && pps->pic_scaling_matrix_present_flag == 0) {
        dps_h264_write_quant_matrix_default(quant_matrix);
    } else if (pps->pic_scaling_matrix_present_flag == 1) {
        dps_h264_write_quant_matrix_from_pps(pps, quant_matrix);
    } else {
        dps_h264_write_quant_matrix_from_sps(sps, quant_matrix);
    }
}

VFMW_ACC_SLICE_H264 STATIC td_void dps_h264_make_dps_slice_list_frame(const h264_ctx *ctx,
    dec_slice_param_h264 *slice_param, td_u32 list_idx)
{
    const h264_slice *curr_slice = &ctx->cur_slice;
    h264_store_pic *syn_list = TD_NULL;
    h264_dec_list *dec_list = TD_NULL;
    td_u32 i;

    for (i = 0; i < curr_slice->listx_size[list_idx]; i++) {
        dec_list = &slice_param->declist[list_idx][i];
        syn_list = ctx->listx[list_idx][i];

        dec_list->is_used = syn_list->frame_store->is_used;
        dec_list->frame_is_long_term = syn_list->is_long_term;
        dec_list->frame_structure = syn_list->structure;
        dec_list->list_structure = 0;
    }
}

#ifdef H264_FIELD_SUPPORT
VFMW_ACC_SLICE_H264 STATIC td_void dps_h264_make_dps_slice_list_field(const h264_ctx *ctx,
    dec_slice_param_h264 *slice_param, td_u32 list_idx)
{
    const h264_slice *curr_slice = &ctx->cur_slice;
    h264_store_pic *syn_list = TD_NULL;
    h264_dec_list *dec_list = TD_NULL;
    td_u32 i;

    vfmw_check(list_idx < 2); /* 2 is num */

    for (i = 0; i < curr_slice->listx_size[list_idx]; i++) {
        dec_list = &slice_param->declist[list_idx][i];
        syn_list = ctx->listx[list_idx][i];

        dec_list->is_used = syn_list->frame_store->is_used;
        dec_list->list_structure = syn_list->structure;
        dec_list->frame_structure = syn_list->frame_store->frame.structure;

        if (syn_list->structure == 1) {
            /* top field */
            dec_list->top_structure = syn_list->structure;
            dec_list->top_is_long_term = syn_list->is_long_term;
        } else {
            /* btm field */
            dec_list->btm_structure = syn_list->structure;
            dec_list->btm_is_long_term = syn_list->is_long_term;
        }
    }
}
#endif

VFMW_ACC_SLICE_H264 STATIC td_void dps_h264_make_dps_slice_list_info(const h264_ctx *ctx,
                                                                     dec_slice_param_h264 *slice_param)
{
#if defined(H264_B_FRAME_SUPPORT) || defined(H264_FIELD_SUPPORT)
    const h264_slice *curr_slice = &ctx->cur_slice;
#endif

    if (ctx->cur_pic.structure == 0) { /* frame reference */
        dps_h264_make_dps_slice_list_frame(ctx, slice_param, 0);
#ifdef H264_B_FRAME_SUPPORT
        if (curr_slice->slice_type == B_SLICE) {
            dps_h264_make_dps_slice_list_frame(ctx, slice_param, 1);
        }
#endif
    } else {
#ifdef H264_FIELD_SUPPORT
        /* field reference */
        dps_h264_make_dps_slice_list_field(ctx, slice_param, 0);

        if (curr_slice->slice_type == B_SLICE) {
            dps_h264_make_dps_slice_list_field(ctx, slice_param, 1);
        }
#endif
    }
}

VFMW_ACC_SLICE_H264 STATIC td_void dps_h264_make_dps_slice_wp_info(const h264_ctx *ctx,
                                                                   dec_slice_param_h264 *slice_param)
{
    const h264_slice *curr_slice = &ctx->cur_slice;
    td_u32 i;

    if ((ctx->cur_pps.weighted_pred_flag == 1 && curr_slice->slice_type == P_SLICE) ||
        (ctx->cur_pps.weighted_bipred_idc == 1 && curr_slice->slice_type == B_SLICE)) {
        slice_param->luma_log2_weight_denom = (td_u8)curr_slice->luma_log2_weight_denom;
        slice_param->chroma_log2_weight_denom = (td_u8)curr_slice->chroma_log2_weight_denom;

        /* list0 */
        for (i = 0; i <= curr_slice->num_ref_idx_l0_active_minus1; i++) {
            slice_param->wp_weight_l0_y[i] = (td_s16)curr_slice->wp_weight_l0_y[i];
            slice_param->wp_weight_l0_u[i] = (td_s16)curr_slice->wp_weight_l0_u[i];
            slice_param->wp_weight_l0_v[i] = (td_s16)curr_slice->wp_weight_l0_v[i];

            slice_param->wp_offset_l0_y[i] = (td_s8)curr_slice->wp_offset_l0_y[i];
            slice_param->wp_offset_l0_u[i] = (td_s8)curr_slice->wp_offset_l0_u[i];
            slice_param->wp_offset_l0_v[i] = (td_s8)curr_slice->wp_offset_l0_v[i];
        }
#ifdef H264_B_FRAME_SUPPORT
        /* list1 */
        if (curr_slice->slice_type == B_SLICE) {
            for (i = 0; i <= curr_slice->num_ref_idx_l1_active_minus1; i++) {
                slice_param->wp_weight_l1_y[i] = curr_slice->wp_weight_l1_y[i];
                slice_param->wp_weight_l1_u[i] = curr_slice->wp_weight_l1_u[i];
                slice_param->wp_weight_l1_v[i] = curr_slice->wp_weight_l1_v[i];

                slice_param->wp_offset_l1_y[i] = curr_slice->wp_offset_l1_y[i];
                slice_param->wp_offset_l1_u[i] = curr_slice->wp_offset_l1_u[i];
                slice_param->wp_offset_l1_v[i] = curr_slice->wp_offset_l1_v[i];
            }
        }
#endif
    }
}

td_s32 dps_h264_get_max_slice_num(td_void)
{
    td_u32 max_slc_num;

    max_slc_num = H264_MAX_SLICE_NUM;
#ifdef VFMW_H264_EXT_SLC_NUM
    max_slc_num = vfmw_max(max_slc_num, pdt_get_product_info()->h264_ext_slc_num);
    max_slc_num = vfmw_min(max_slc_num, VFMW_H264_EXT_SLC_NUM);
#endif

    return (td_s32)max_slc_num;
}

td_s32 dps_h264_get_pic_param_size(td_void)
{
    return sizeof(dec_pic_param_h264);
}

td_s32 dps_h264_get_slice_param_size(td_void)
{
    return sizeof(dec_slice_param_h264);
}

STATIC td_s32 dps_h264_calc_slice_end_addr(const dec_pic_param_h264 *pic_param,
                                           dec_slice_param_h264 *slice_head, td_u16 slice_num)
{
    dec_slice_param_h264 *p = slice_head;
    td_u16 i;
    td_u32 pre_last_mb_in_slice = 0;

    for (i = 0; i < slice_num - 1; i++, p = p->next_slice) {
        p->last_mb_in_slice = p->next_slice->first_mb_in_slice - 1;

        if (pre_last_mb_in_slice > p->last_mb_in_slice && (i > 0)) {
            dprint(PRN_ERROR, "slice%d  last_mb_in_slice(%d) < pre_last_mb_in_slice(%d)\n",
                   i, p->last_mb_in_slice, pre_last_mb_in_slice);
            return DPS_ERR;
        }

        pre_last_mb_in_slice = p->last_mb_in_slice;

        if (p->valid_bitlen[0] == 0) {
            dprint(PRN_ERROR, "error,slice%d valid_bitlen[0] is 0\n", i);
            return DPS_ERR;
        }
    }
    vfmw_check_ret(pic_param->pic_width_in_mb * pic_param->pic_height_in_mb < VFMW_MAX_UINT32, DEC_ERR);
    p->last_mb_in_slice = (pic_param->pic_width_in_mb * pic_param->pic_height_in_mb - 1) >> pic_param->mbaff_frame_flag;

    if (p->valid_bitlen[0] == 0) {
        dprint(PRN_ERROR, "error,last slice valid_bitlen[0] is 0\n");
        return DPS_ERR;
    }

    return DPS_OK;
}

STATIC td_void dps_h264_make_dps_pic_common_info(const h264_ctx *ctx, dec_pic_param_h264 *pic_param)
{
    const h264_sps *sps = &ctx->cur_sps;
    const h264_pps *pps = &ctx->cur_pps;

    pic_param->pic_width_in_mb = sps->pic_width_in_mbs_minus1 + 1;
    pic_param->pic_height_in_mb = ctx->cur_pic.pic_height_in_mb;
    pic_param->chroma_format_idc = (td_u8)sps->chroma_format_idc;
    pic_param->frame_mbs_only_flag = sps->frame_mbs_only_flag;
    pic_param->mb_adaptive_frame_field_flag = sps->mb_adaptive_frame_field_flag;

    vfmw_check((sps->pic_height_in_map_units_minus1 + 1) < VFMW_MAX_UINT32);
    vfmw_check(pic_param->pic_width_in_mb < VFMW_MAX_UINT32);

    pic_param->frame_height = (sps->pic_height_in_map_units_minus1 + 1) * 16 * /* 16 is mb size */
                              (1 + (sps->frame_mbs_only_flag == 0));
    pic_param->disp_width = vfmw_min(sps->disp_width, pic_param->pic_width_in_mb * 16); /* 16 is mb size */
    pic_param->disp_height = vfmw_min(sps->disp_height, pic_param->frame_height);
    pic_param->top_offset = sps->frame_crop_top_offset;
    pic_param->bottom_offset = sps->frame_crop_bottom_offset;

    pic_param->entropy_coding_mode_flag = pps->entropy_coding_mode_flag;
    pic_param->weighted_pred_flag = pps->weighted_pred_flag;
    pic_param->weighted_bipred_idc = (td_u8)pps->weighted_bipred_idc;
    pic_param->constrained_intra_pred_flag = pps->constrained_intra_pred_flag;
    pic_param->transform_8x8_mode_flag = (td_u8)pps->transform_8x8_mode_flag;

    pic_param->nal_ref_idc = (td_u8)ctx->cur_pic.nal_ref_idc;
    pic_param->cabac_qp_err_dec_en = 0;
    pic_param->new_pic_flag = ctx->cur_pic.field_pair_flag == 0 ? 1 : 0;
    pic_param->pic_structure = ctx->cur_pic.structure;
    pic_param->mbaff_frame_flag = ctx->cur_pic.mbaff_frame_flag;

    pic_param->this_poc = ctx->cur_poc.this_poc;
    pic_param->top_poc = ctx->cur_poc.toppoc;
    pic_param->bottom_poc = ctx->cur_poc.bottompoc;
    pic_param->ref_pic_single_size = ctx->ref_pic_single_size;

    pic_param->pic_type = (td_u8)ctx->cur_pic.pic_type;
    pic_param->fsp_logic_param_id = (td_u32)ctx->cur_pic.frame_store->logic_fs_id;
    pic_param->stream_base_addr = ctx->cur_pic.stream_base_addr;
}

STATIC td_void dps_h264_make_dps_pic_apc(const h264_ctx *ctx, dec_pic_param_h264 *pic_param)
{
    td_u32 i;
    td_s32 idx;
    h264_frame_store *h264_fs = TD_NULL;

    /* the apc idx of a frame must keep same in life for h264 */
    pic_param->apc_size = (td_u16)ctx->dpb.size;
    for (i = 0; i < H264_APC_SIZE; i++) {
        pic_param->apc_fsp_id[i] = INVALID_IDX;
        pic_param->ref_pic_single_fsp_id[i] = ctx->ref_pic_single_fsp_id[i];
    }

    for (i = 0; i < ctx->dpb.used_size; i++) {
        h264_fs = ctx->dpb.fs[i];

        if (h264_fs->is_reference > 0) {
            idx = (td_s32)h264_fs->apc_idc;
            vfmw_check(idx < H264_APC_SIZE);
            pic_param->apc_fsp_id[idx] = (td_u32)h264_fs->logic_fs_id;
#ifdef H264_FIELD_SUPPORT
            if (h264_fs->is_used == 1) {
                pic_param->apc_poc[0][idx] = h264_fs->top_field.poc;
                pic_param->apc_poc[1][idx] = h264_fs->top_field.poc;
            } else if (h264_fs->is_used == 2) { /* 2 is btm field */
                pic_param->apc_poc[0][idx] = h264_fs->bottom_field.poc;
                pic_param->apc_poc[1][idx] = h264_fs->bottom_field.poc;
            } else {
                pic_param->apc_poc[0][idx] = h264_fs->top_field.poc;
                pic_param->apc_poc[1][idx] = h264_fs->bottom_field.poc;
            }
#else
            pic_param->apc_poc[0][idx] = h264_fs->top_field.poc;
            pic_param->apc_poc[1][idx] = h264_fs->bottom_field.poc;
#endif
        }
    }
}

STATIC td_s32 dps_h264_update_dps_info(h264_ctx *ctx, const dec_pic_param_h264 *pic_param)
{
    ctx->cur_dps->pic_ctb_num = (td_s32)(pic_param->pic_width_in_mb * pic_param->pic_height_in_mb);

    ctx->cur_dps->logic_id = (td_s32)pic_param->fsp_logic_param_id;
    ctx->cur_dps->pic_struct = pic_param->pic_structure;
    ctx->cur_dps->frm_width = (pic_param->pic_width_in_mb * 16); /* 16 is mb size */
    ctx->cur_dps->frm_height = (pic_param->pic_height_in_mb * 16); /* 16 is mb size */

    ctx->cur_dps->disp_width = pic_param->disp_width;
    ctx->cur_dps->disp_height = pic_param->disp_height;
    ctx->cur_dps->top_offset = pic_param->top_offset;
    ctx->cur_dps->bottom_offset = pic_param->bottom_offset;
    ctx->cur_dps->pic_coding_type = pic_param->pic_type;
    ctx->cur_dps->frame_mbs_only = pic_param->frame_mbs_only_flag;

    if (pic_param->pic_structure != 0) {
        vfmw_check_ret(ctx->cur_dps->frm_height <= VFMW_MAX_SINT32 / 2, DPS_ERR); /* 2 is double field */
        ctx->cur_dps->frm_height *= 2; /* 2 is double field */
    }

    return DPS_OK;
}

td_s32 dps_h264_make_dps_pic(td_void *syn_ctx, td_void *param)
{
    h264_ctx *ctx = (h264_ctx *)syn_ctx;
    dec_pic_param_h264 *pic_param = (dec_pic_param_h264 *)param;
    dec_slice_param_h264 *slice_param = TD_NULL;
    td_s32 ret;

    vfmw_check_ret(ctx != TD_NULL && pic_param != TD_NULL, DPS_ERR);

    dps_h264_make_dps_pic_common_info(ctx, pic_param);

    dps_h264_make_dps_pic_apc(ctx, pic_param);

    dps_h264_write_quant_matrix(ctx, pic_param->qmatrix);

    if ((ctx->cur_dps != TD_NULL) && (ctx->cur_dps->slice_head != TD_NULL)) {
        slice_param = (dec_slice_param_h264 *)ctx->cur_dps->slice_head;
    } else {
        dprint(PRN_ERROR, "%s cur_dps or slice_head is TD_NULL!\n", __func__);
        return DPS_ERR;
    }

    if (slice_param == TD_NULL) {
        dprint(PRN_ERROR, "%s h264_ctx->cur_dps->slice_head is TD_NULL!\n", __func__);
        return DPS_ERR;
    }
    vfmw_check_ret(slice_param != TD_NULL && ctx->extra_data != TD_NULL, DPS_ERR);

    if (ctx->extra_data->dec_mode == VFMW_DEC_MODE_I && slice_param->first_mb_in_slice != 0) {
        dprint(PRN_ERROR, "%s I mode while fisrt mb %d != 0\n", __func__, slice_param->first_mb_in_slice);
        return DPS_ERR;
    }

    /* calc slice address */
    ret = dps_h264_calc_slice_end_addr(pic_param, slice_param, (td_u16)ctx->cur_dps->slice_num);
    if (ret != DPS_OK) {
        dprint(PRN_ERROR, "%s call dps_h264_calc_slice_end_addr failed.\n", __func__);
        return DPS_ERR;
    }

    vfmw_check_ret(pic_param->pic_width_in_mb <= VFMW_MAX_UINT32 / pic_param->pic_height_in_mb, DPS_ERR);
    vfmw_check_ret(pic_param->pic_width_in_mb <= VFMW_MAX_UINT32 / 16, DPS_ERR);  /* 16 is mb size */
    vfmw_check_ret(pic_param->pic_height_in_mb <= VFMW_MAX_UINT32 / 16, DPS_ERR); /* 16 is mb size */

    pic_param->sed_state = ctx->cur_dps->sed_state;

    if (dps_h264_update_dps_info(ctx, pic_param) != DPS_OK) {
        return DPS_ERR;
    }

    return DPS_OK;
}

VFMW_ACC_SLICE_H264 td_void dps_h264_make_dps_slice_stream_info(const h264_ctx *ctx,
    dec_slice_param_h264 *slice_param)
{
    td_u32 i;
    vfmw_check(slice_param != TD_NULL && ctx != TD_NULL && ctx->cur_nal != TD_NULL);

    for (i = ctx->cur_nal->nal_used_segment; ((i < ctx->cur_nal->nal_segment) && (i < 2)); i++) { /* 2 is seg num */
        slice_param->stream_id[i] = ctx->cur_nal->stream[i].stream_id;
        slice_param->bits_offset[i] = (td_u8)ctx->cur_nal->stream[i].bits_offset;
        slice_param->valid_bitlen[i] = ctx->cur_nal->stream[i].valid_bitlen;
        slice_param->bytes_pos[i] = ctx->cur_nal->stream[i].bytes_pos;
    }

    for (i = ctx->cur_nal->nal_segment; i < 2; i++) { /* 2 is seg num */
        slice_param->stream_id[i] = INVALID_IDX;
        slice_param->bits_offset[i] = 0;
        slice_param->valid_bitlen[i] = 0;
        slice_param->bytes_pos[i] = 0;
    }
}

VFMW_ACC_SLICE_H264 STATIC td_void dps_h264_make_ref_single_info_list(h264_ctx *ctx, td_u32 i, td_u32 j)
{
    td_u32 k;

    for (k = 0; (k < H264_APC_SIZE) && (j < MAX_LIST_SIZE); k++) {
        if ((ctx->ref_pic_single_fsp_id[k] == ctx->listx[i][j]->frame_store->logic_fs_id) &&
            (ctx->listx[i][j]->frame_store->logic_fs_id != INVALID_IDX)) {
            break;
        }
    }

    if (k >= H264_APC_SIZE && ctx->ref_pic_single_size < H264_APC_SIZE) {
        ctx->ref_pic_single_fsp_id[ctx->ref_pic_single_size++] =
            ctx->listx[i][j]->frame_store->logic_fs_id;
    }

    return;
}

STATIC VFMW_ACC_SLICE_H264 td_void dps_h264_make_dps_slice_ref_info(h264_ctx *ctx, const h264_slice *curr_slice,
    dec_slice_param_h264 *slice_param)
{
    td_u32 i, j;
    vfmw_check(ctx != TD_NULL && curr_slice != TD_NULL && slice_param != TD_NULL);

    slice_param->list_x_size[0] = (td_u8)curr_slice->listx_size[0];
    slice_param->list_x_size[1] = (td_u8)curr_slice->listx_size[1];

    for (i = 0; i < 2; i++) { /* 2 is vfmw_max num */
        for (j = 0; j < curr_slice->listx_size[i] && j < (MAX_LIST_SIZE - 1); j++) {
            slice_param->idx_2_fsp_id[i][j] = (td_u32)ctx->listx[i][j]->frame_store->logic_fs_id;
        }
    }

    for (i = 0; i < 2; i++) { /* 2 is vfmw_max num */
        for (j = 0; j < curr_slice->listx_size[i] && j < MAX_LIST_SIZE; j++) {
            dps_h264_make_ref_single_info_list(ctx, i, j);
        }
    }

    if (curr_slice->slice_type == B_SLICE) {
        slice_param->col_pmv_fsp_id = (td_s16)ctx->listx[1][0]->frame_store->logic_fs_id;
    } else {
        slice_param->col_pmv_fsp_id = INVALID_IDX;
    }
}

VFMW_ACC_SLICE_H264 td_s32 dps_h264_make_dps_slice(td_void *syn_ctx, td_void *param, td_void *prev_param)
{
    h264_ctx *ctx = (h264_ctx *)syn_ctx;
    h264_sps *sps = TD_NULL;
    h264_pps *pps = TD_NULL;
    dec_slice_param_h264 *slice_param = (dec_slice_param_h264 *)param;
    dec_slice_param_h264 *prev_slice_param = (dec_slice_param_h264 *)prev_param;
    h264_slice *curr_slice = TD_NULL;

    vfmw_check_ret(ctx != TD_NULL && slice_param != TD_NULL, DPS_ERR);
    sps = &ctx->cur_sps;
    pps = &ctx->cur_pps;
    curr_slice = &ctx->cur_slice;

    dps_h264_make_dps_slice_stream_info(ctx, slice_param);

    slice_param->first_mb_in_slice = curr_slice->first_mb_in_slice;
    slice_param->slice_type = curr_slice->slice_type;
    slice_param->cabac_init_idc = (td_u8)curr_slice->cabac_init_idc;
    slice_param->num_ref_idx_l0_active_minus1 = (td_u8)curr_slice->num_ref_idx_l0_active_minus1;
    slice_param->num_ref_idx_l1_active_minus1 = (td_u8)curr_slice->num_ref_idx_l1_active_minus1;
    slice_param->direct_spatial_mv_pred_flag = curr_slice->direct_spatial_mv_pred_flag;
    slice_param->direct_8x8_inference_flag = sps->direct_8x8_inference_flag;

    slice_param->slice_qpy = (td_s8)(pps->pic_init_qp_minus26 + 26 + curr_slice->slice_qp_delta); /* 26 is minus26 */
    slice_param->chroma_qp_index_offset = (td_s8)pps->chroma_qp_index_offset;
    slice_param->second_chroma_qp_index_offset = (td_s8)pps->second_chroma_qp_index_offset;
    slice_param->disable_deblocking_filter_idc = (td_u8)curr_slice->disable_deblocking_filter_idc;
    slice_param->slice_alpha_c0_offset_div2 = (td_s8)curr_slice->slice_alpha_c0_offset_div2;
    slice_param->slice_beta_offset_div2 = (td_s8)curr_slice->slice_beta_offset_div2;

    dps_h264_make_dps_slice_ref_info(ctx, curr_slice, slice_param);

    if (curr_slice->slice_type == B_SLICE) {
        slice_param->weight_flag = (td_u8)pps->weighted_bipred_idc;
    } else if (curr_slice->slice_type == P_SLICE) {
        slice_param->weight_flag = pps->weighted_pred_flag;
    } else {
        slice_param->weight_flag = 0;
    }

    if (curr_slice->slice_type != I_SLICE) {
        dps_h264_make_dps_slice_list_info(ctx, slice_param);

        dps_h264_make_dps_slice_wp_info(ctx, slice_param);
    }

    slice_param->next_slice = TD_NULL;
    if (prev_slice_param != TD_NULL) {
        prev_slice_param->next_slice = slice_param;
    }

    return DPS_OK;
}

td_s32 dps_h264_get_ref_info(const td_void *param, dps_ref_info *dps_ref)
{
    dec_pic_param_h264 *pic_param = (dec_pic_param_h264 *)param;
    vfmw_check_ret(pic_param != TD_NULL && dps_ref != TD_NULL, DPS_ERR);

    dps_ref->ref_size = pic_param->apc_size;
    dps_ref->ref_fsp_id = &pic_param->apc_fsp_id[0];
    dps_ref->apc_2_fsp_phy_addr = &pic_param->apc_fsp_phy_addr[0];
    dps_ref->ref_pic_type = &pic_param->ref_pic_type[0];
    dps_ref->cur_fsp_id = pic_param->fsp_logic_param_id;
    dps_ref->cur_ref_size = (td_u16)pic_param->ref_pic_single_size;
    dps_ref->cur_ref_fsp_id = (td_u32 *)(&pic_param->ref_pic_single_fsp_id[0]);
    dps_ref->cur_ref_fsp_phy_addr = &pic_param->ref_pic_single_phy_addr[0];

    return DPS_OK;
}

td_s32 dps_h264_release_seg(td_s32 chan_id, const td_void *param)
{
    td_s32 i;
    td_s32 j;
    td_s32 seg_id;
    dps_info *dps = TD_NULL;
    dec_slice_param_h264 *slice = TD_NULL;

    dps = (dps_info *)param;
    vfmw_check_ret(dps != TD_NULL, DPS_ERR);

    for (i = 0; (i < dps->slice_num) && (i < DPS_SLC_NUM); i++) {
        slice = (dec_slice_param_h264 *)(dps->slice_list[i]);

        for (j = 0; j < 2; j++) { /* 2 is vfmw_max num */
            seg_id = slice->stream_id[j];
            if (seg_id == INVALID_IDX) {
                continue;
            }

            if (stm_release_seg(chan_id, seg_id) != STM_OK) {
                dprint(PRN_ERROR, "%s failed seg[%d] id %d\n", __func__, j, seg_id);
            }
        }
    }

    return DPS_OK;
}
