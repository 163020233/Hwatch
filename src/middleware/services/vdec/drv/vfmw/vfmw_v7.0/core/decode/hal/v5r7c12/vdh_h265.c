/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "vdh_h265.h"
#include "dec_hal.h"
#include "dec_chn.h"
#include "dec_dev.h"
#include "dec_ref.h"
#include "dbg.h"
#include "vfmw_define.h"

STATIC td_s32 g_cabac_init_value[3][160] = { /* 3 and 160 are length */
    /*  I  */
    {
        -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1,
        153, 200, 139, 141, 157, 154, -1, -1,
        -1, -1, 184, -1, -1, -1, 184, 63,
        94, 138, 182, 154, 111, 141, 154, 154,
        139, 139, 138, 153, 136, 167, 152, 152,
        110, 110, 124, 125, 140, 153, 125, 127,
        140, 109, 111, 143, 127, 111, 79, 108,
        123, 63, 110, 110, 124, 125, 140, 153,
        125, 127, 140, 109, 111, 143, 127, 111,
        79, 108, 123, 63, 91, 171, 134, 141,
        140, 92, 137, 138, 140, 152, 138, 139,
        153, 74, 149, 92, 139, 107, 122, 152,
        140, 179, 166, 182, 140, 227, 122, 197,
        111, 111, 125, 110, 110, 94, 124, 108,
        124, 107, 125, 141, 179, 153, 125, 107,
        125, 141, 179, 153, 125, 107, 125, 141,
        179, 153, 125, 140, 139, 182, 182, 152,
        136, 152, 136, 153, 136, 139, 111, 136,
        139, 111, -1, 153, 138, 138, -1, -1
    },
    /*  P  */
    {
        79, 110, 122, 95, 79, 63, 31, 31,
        153, 153, 168, 140, 198, -1, -1, -1,
        153, 185, 107, 139, 126, 154, 197, 185,
        201, 149, 154, 139, 154, 154, 154, 152,
        149, 107, 167, 154, 153, 111, 154, 154,
        139, 139, 107, 167, 91, 122, 107, 167,
        125, 110, 94, 110, 95, 79, 125, 111,
        110, 78, 110, 111, 111, 95, 94, 108,
        123, 108, 125, 110, 94, 110, 95, 79,
        125, 111, 110, 78, 110, 111, 111, 95,
        94, 108, 123, 108, 121, 140, 61, 154,
        154, 196, 196, 167, 154, 152, 167, 182,
        182, 134, 149, 136, 153, 121, 136, 137,
        169, 194, 166, 167, 154, 167, 137, 182,
        155, 154, 139, 153, 139, 123, 123, 63,
        153, 166, 183, 140, 136, 153, 154, 166,
        183, 140, 136, 153, 154, 166, 183, 140,
        136, 153, 154, 170, 153, 123, 123, 107,
        121, 107, 121, 167, 151, 183, 140, 151,
        183, 140, -1, 124, 138, 94, -1, -1
    },
    /*  B  */
    {
        79, 154, 137, 95, 79, 63, 31, 31,
        153, 153, 168, 169, 198, -1, -1, -1,
        153, 160, 107, 139, 126, 154, 197, 185,
        201, 134, 154, 139, 154, 154, 183, 152,
        149, 92, 167, 154, 153, 111, 154, 154,
        139, 139, 107, 167, 91, 107, 107, 167,
        125, 110, 124, 110, 95, 94, 125, 111,
        111, 79, 125, 126, 111, 111, 79, 108,
        123, 93, 125, 110, 124, 110, 95, 94,
        125, 111, 111, 79, 125, 126, 111, 111,
        79, 108, 123, 93, 121, 140, 61, 154,
        154, 196, 167, 167, 154, 152, 167, 182,
        182, 134, 149, 136, 153, 121, 136, 122,
        169, 208, 166, 167, 154, 152, 167, 182,
        170, 154, 139, 153, 139, 123, 123, 63,
        124, 166, 183, 140, 136, 153, 154, 166,
        183, 140, 136, 153, 154, 166, 183, 140,
        136, 153, 154, 170, 153, 138, 138, 122,
        121, 122, 121, 167, 151, 183, 140, 151,
        183, 140, -1, 224, 167, 122, -1, -1
    }
};

STATIC td_s32 hal_h265_write_slice_msg(td_u8 *, td_void *, td_void *, td_void *, UADDR);

STATIC td_s32 hal_h265_write_cabac_table(td_u8 *mn_vir_addr)
{
    td_s32 idx = 0;
    td_s32 i, j;
    td_s8 *p = (td_s8 *)mn_vir_addr;
    td_s32 init_value, slope_idx, offset_idx;

    for (i = 0; i < 3; i++) { /* 3 :for loop count */
        j = (i == 0) ? 16 : 0; /* 16 is standard strength */

        for (; j < 160; j++, idx++, p += 2) { /* 160 :for loop count 2 is the vfmw_min step */
            init_value = g_cabac_init_value[i][j];

            if (init_value == -1) {
                p[1] = 0;
                p[0] = 0;
            } else {
                slope_idx = (td_u32)init_value >> 4; /* 4 is bit count */
                offset_idx = (td_u32)init_value & 15; /* 15:for alignment */
                p[1] = slope_idx * 5 - 45;     /*  m  5 is multiplier 45 :deviation */
                p[0] = ((td_u32)offset_idx << 3) - 16; /*  n  3 is bit count 16 :deviation */
            }
        }
    }

    return DEC_OK;
}

STATIC td_s32 hal_h265_ctb_ts_addr_to_rs_addr(dec_pic_param_h265 *pic_param, td_s32 ctb_addr_ts)
{
    td_u32 tile_x, tile_y;
    td_s32 tile_width_in_ctb;
    td_s32 tile_height_in_ctb;
    td_s32 abs_width_in_ctb;
    td_s32 abs_height_in_ctb = 0;
    td_s32 tile_ctb_addr_in_ts;
    td_s32 next_tile_ctb_addr_in_ts = 0;
    td_s32 ctb_addr_rs;
    td_s32 row_height_minus1[HEVC_MAX_TILE_ROWS];
    td_s32 column_width_minus1[HEVC_MAX_TILE_COLUMNS];
    td_s32 pic_width_in_ctb_y = pic_param->pic_width_in_ctbs_y_minus1 + 1;

    if (pic_param->tiles_enabled_flag == 0) {
        ctb_addr_rs = ctb_addr_ts;
        return ctb_addr_rs;
    }

    for (tile_y = 0; tile_y <= pic_param->num_tile_rows_minus1; tile_y++) {
        row_height_minus1[tile_y] = pic_param->tile_pos_y[tile_y][1] - pic_param->tile_pos_y[tile_y][0];
    }
    for (tile_x = 0; tile_x <= pic_param->num_tile_columns_minus1; tile_x++) {
        column_width_minus1[tile_x] = pic_param->tile_pos_x[tile_x][1] - pic_param->tile_pos_x[tile_x][0];
    }

    for (tile_y = 0; tile_y <= pic_param->num_tile_rows_minus1; tile_y++) {
        abs_width_in_ctb = 0;

        for (tile_x = 0; tile_x <= pic_param->num_tile_columns_minus1; tile_x++) {
            tile_ctb_addr_in_ts = next_tile_ctb_addr_in_ts;
            tile_width_in_ctb = column_width_minus1[tile_x] + 1;
            tile_height_in_ctb = row_height_minus1[tile_y] + 1;

            next_tile_ctb_addr_in_ts += tile_width_in_ctb * tile_height_in_ctb;

            if (next_tile_ctb_addr_in_ts > ctb_addr_ts) {
                goto out;
            }

            abs_width_in_ctb += tile_width_in_ctb;
        }

        abs_height_in_ctb += tile_height_in_ctb;
    }
out:
    if (tile_width_in_ctb != 0) {
        abs_width_in_ctb += (ctb_addr_ts - tile_ctb_addr_in_ts) % tile_width_in_ctb;
        abs_height_in_ctb += (ctb_addr_ts - tile_ctb_addr_in_ts) / tile_width_in_ctb;
    }

    ctb_addr_rs = abs_width_in_ctb + pic_width_in_ctb_y * abs_height_in_ctb;

    return ctb_addr_rs;
}

STATIC td_void hal_h265_split_slice_list(dec_pic_param_h265 *pic_param,
    dec_slice_param_h265 *slice_head, td_u16 slice_num, td_void *slice_list)
{
    dec_chan_slice_list *slc_list = (dec_chan_slice_list *)slice_list;
    dec_slice_param_h265 **list_head = (dec_slice_param_h265 **)&slc_list->list_head[0];
    td_u16 *list_len = &slc_list->list_len[0];
    td_u16 *list_num = &slc_list->list_num;

    vfmw_unused(pic_param);

    slc_list->fake_slice = 0;
    if (slice_head->slice_segment_address != 0) {
        slc_list->fake_slice = 1;
        slice_num++;
    }

    *list_head = slice_head;
    *list_len = slice_num;
    *list_num = 1;

    return;
}

STATIC td_s32 hal_h265_write_qmatrix(const td_u32 *qmatrix, td_u32 size, td_u32 *vir_addr)
{
    td_u32 i;

    for (i = 0; i < size; i++) {
        *(vir_addr + i) = *(qmatrix + i);
    }

    return DEC_OK;
}

STATIC td_s32 hal_h265_write_tile_xy(const dec_pic_param_h265 *pic_param, td_u32 *vir_addr)
{
    td_s32 i;
    td_u32 d32;

    /* tile_pos_x */
    for (i = 0; i < HEVC_MAX_TILE_COLUMNS; i++) {
        if (i < HEVC_LOGIC_TILE_COLUMNS_LIMIT) {
            if ((i & 0x01) == 0) {
                d32 = (i == 0) ? (pic_param->tile_pos_x[i][0]) : (pic_param->tile_pos_x[i - 1][1] + 1);
            } else {
                d32 |= ((pic_param->tile_pos_x[i - 1][1] + 1) << 16);   /* 16: bit count */
                *(vir_addr++) = d32;
            }
        }
    }
    d32 =  pic_param->tile_pos_x[i - 1][1] + 1 ; // PicWidthInCtbsXMinus1 + 1
    *(vir_addr++) = d32;

    vir_addr++;

    /* tile_pos_y */
    for (i = 0; i < HEVC_MAX_TILE_ROWS; i++) {
        if (i < HEVC_LOGIC_TILE_ROWS_LIMIT) {
            if ((i & 0x01) == 0) {
                d32 = (i == 0) ? pic_param->tile_pos_y[i][0] : (pic_param->tile_pos_y[i - 1][1] + 1) ;
            } else {
                d32 |= ((pic_param->tile_pos_y[i - 1][1] + 1) << 16);   /* 16: bit count */
                *(vir_addr++) = d32;
            }
        }
    }
    d32 =  pic_param->tile_pos_y[i - 1][1] + 1; // PicWidthInCtbsYMinus1 + 1
    *(vir_addr++) = d32;

    return DEC_OK;
}

STATIC td_s32 hal_h265_write_tile_info(const dec_pic_param_h265 *pic_param, td_u8 *tile_vir_addr)
{
    hal_h265_write_tile_xy(pic_param, (td_u32 *)tile_vir_addr);

    return DEC_OK;
}

STATIC td_s32 hal_h265_write_stream_info(const dec_pic_param_h265 *pic_param, const dec_slice_param_h265 *slice_param,
                                         td_u32 *slot)
{
    UADDR stream_base_addr, stream_start_addr;
    td_u32 d32, i;

    if (slice_param->valid_bitlen[0] == 0) {
        return DEC_ERR;
    }

    for (i = 0; i < 2; i++) { /* 2 :for loop count */
        if (slice_param->valid_bitlen[i] > 0) {
            stream_base_addr = vfmw_align_dn(pic_param->stream_base_addr, 16); /* 16 is standard strength */
            stream_start_addr = vfmw_align_dn(slice_param->bytes_pos[i], 16); /* 16 is standard strength */

            d32 = (td_u32)hw_addr_shift(stream_start_addr - stream_base_addr);
            wr_msgword(slot + 3 * i, d32); /* 3 :deviation */

            d32 = (td_u32)((slice_param->bits_offset[i] +
                ((slice_param->bytes_pos[i] & 0xf) << 3)) & 0x7f); /* 3 is bit count */
            wr_msgword(slot + 3 * i + 1, d32); /* 3 is multiplier */

            d32 = slice_param->valid_bitlen[i];
            wr_msgword(slot + 3 * i + 2, d32); /* 3 is multiplier 2 :deviation */
        } else {
            wr_msgword(slot + 3 * i, 0); /* 3 is multiplier */
            wr_msgword(slot + 3 * i + 1, 0); /* 3 is multiplier */
            wr_msgword(slot + 3 * i + 2, 0); /* 3 :deviation 2 :deviation */
        }
    }

    return DEC_OK;
}

STATIC td_void hal_h265_write_apc_index(const dec_slice_param_h265 *slice_param, td_u32 *slot)
{
    td_u32 i, tmp1;
    td_s32 d32;

    tmp1 = 0;
    d32 = 15; /* 15 is standard strength */

    /*  D15~D16  */
    for (i = 0; i < slice_param->listx_size[0]; i++) {
        tmp1 |= (((td_u32)slice_param->idx_2_apc[0][i]) << (4 * (i & 0x7))); /* 4 is an address */

        if (i % 8 == 7) { /* 8:for alignment 7 is standard strength */
            wr_msgword(slot + (d32), tmp1);
            tmp1 = 0;
            ++d32;
        } else if (i == (slice_param->listx_size[0] - 1)) {
            wr_msgword(slot + (d32), tmp1);
        }
    }

    /*  D17~D18  */
    tmp1 = 0;
    d32 = 17; /* 17 is standard strength */

    for (i = 0; i < slice_param->listx_size[1]; i++) {
        tmp1 |= (((td_u32)slice_param->idx_2_apc[1][i]) << (4 * (i & 0x7))); /* 4 is multiplier */

        if (i % 8 == 7) { /* 8:for alignment 7 is standard strength */
            wr_msgword(slot + (d32), tmp1);
            tmp1 = 0;
            ++d32;
        } else if (i == (slice_param->listx_size[1] - 1)) {
            wr_msgword(slot + (d32), tmp1);
        }
    }
}

STATIC td_void hal_h265_write_wp_info(const dec_pic_param_h265 *pic_param, const dec_slice_param_h265 *slice_param,
    td_u32 *slot)
{
    /*  D0~D31 of burst1, Dn_num = 1*64 + D0 ,luma wp  */
    /*  D0~D31 of burst2, Dn_num = 2*64 + D0 ,luma wp  */
    td_u32 *slot_line0 = slot + 64; /* 64 :deviation */
    td_u32 *slot_line1 = slot + 128; /* 128 :deviation */
    td_u32 d32, i;

    if ((pic_param->weighted_pred_flag && slice_param->slice_type == HEVC_P_SLICE) ||
        (pic_param->weighted_bipred_flag && slice_param->slice_type == HEVC_B_SLICE)) {
        /*  list0  */
        for (i = 0; i < slice_param->listx_size[0]; i++) {
            d32 = (((td_u32)slice_param->luma_offset_l0[i] & 0xff) << 12) | /* 12 is bit count */
                  (((td_u32)slice_param->luma_weight_l0[i] & 0x1ff) << 3) | /* 3 is bit count */
                  ((td_u32)slice_param->luma_log2_weight_denom & 0x7);
            wr_msgword(slot_line0 + i, d32);

            d32 = (((td_u32)slice_param->chroma_offset_l0[i][0] & 0xff) << 12) | /* 12 is bit count */
                  (((td_u32)slice_param->chroma_weight_l0[i][0] & 0x1ff) << 3) | /* 3 is bit count */
                  ((td_u32)slice_param->chroma_log2_weight_denom & 0x7);
            wr_msgword(slot_line1 + i, d32);

            d32 = (((td_u32)slice_param->chroma_offset_l0[i][1] & 0xff) << 12) | /* 12 is bit count */
                  (((td_u32)slice_param->chroma_weight_l0[i][1] & 0x1ff) << 3) | /* 3 is bit count */
                  ((td_u32)slice_param->chroma_log2_weight_denom & 0x7);
            wr_msgword(slot_line1 + (i + 32), d32); /* 32 is bit count */
        }

        /*  list1  */
        if (slice_param->slice_type == HEVC_B_SLICE) {
            for (i = 0; i < slice_param->listx_size[1]; i++) {
                d32 = (((td_u32)slice_param->luma_offset_l1[i] & 0xff) << 12) | /* 12 is bit count */
                      (((td_u32)slice_param->luma_weight_l1[i] & 0x1ff) << 3) | /* 3 is bit count */
                      ((td_u32)slice_param->luma_log2_weight_denom & 0x7);
                wr_msgword(slot_line0 + (i + 16), d32); /* 16 :deviation */

                d32 = (((td_u32)slice_param->chroma_offset_l1[i][0] & 0xff) << 12) | /* 12 is bit count */
                      (((td_u32)slice_param->chroma_weight_l1[i][0] & 0x1ff) << 3) | /* 3 is bit count */
                      ((td_u32)slice_param->chroma_log2_weight_denom & 0x7);
                wr_msgword(slot_line1 + (i + 16), d32); /* 16 :deviation */

                d32 = (((td_u32)slice_param->chroma_offset_l1[i][1] & 0xff) << 12) | /* 12 is bit count */
                      (((td_u32)slice_param->chroma_weight_l1[i][1] & 0x1ff) << 3) | /* 3 is bit count */
                      ((td_u32)slice_param->chroma_log2_weight_denom & 0x7);
                wr_msgword(slot_line1 + (i + 48), d32); /* 48 is bit count */
            }
        }
    }
}

STATIC td_s32 hal_h265_write_fake_slice(td_u8 *slot_vir_addr,
    td_void *pic, td_void *task_mem, td_void *slice, UADDR next_slice_phy_addr)
{
    dec_pic_param_h265 *pic_param = (dec_pic_param_h265 *)pic;
    dec_slice_param_h265 *slice_param = (dec_slice_param_h265 *)slice;
    td_u32 slice_segment_address, slice_tile_address, end_ctb_in_slice_raster, end_ctb_in_slice_tile;
    UADDR bytes_pos[2] = { 0, 0 }; /* 2 is an index */
    td_u8 bits_offset[2] = { 0, 0 }; /* 2 is an index */
    td_u32 valid_bitlen[2] = { 0, 0 }; /* 2 is an index */
    td_u8 dependent_slice_segment_flag;

    if (slice_param == TD_NULL) {
        return DEC_ERR;
    }

    valid_bitlen[0] = slice_param->valid_bitlen[0];
    valid_bitlen[1] = slice_param->valid_bitlen[1];
    bits_offset[0] = slice_param->bits_offset[0];
    bits_offset[1] = slice_param->bits_offset[1];
    bytes_pos[0] = slice_param->bytes_pos[0];
    bytes_pos[1] = slice_param->bytes_pos[1];
    slice_segment_address = slice_param->slice_segment_address;
    slice_tile_address = slice_param->slice_segment_tile_address;
    end_ctb_in_slice_raster = slice_param->end_ctb_in_slice_raster;
    end_ctb_in_slice_tile = slice_param->end_ctb_in_slice_tile;
    dependent_slice_segment_flag = slice_param->dependent_slice_segment_flag;

    slice_param->slice_segment_address = slice_param->slice_segment_tile_address = 0;
    slice_param->end_ctb_in_slice_raster = (td_u32)hal_h265_ctb_ts_addr_to_rs_addr(pic_param, slice_tile_address - 1);
    slice_param->end_ctb_in_slice_tile = slice_tile_address - 1;
    slice_param->valid_bitlen[0] = 1;
    slice_param->valid_bitlen[1] = 0;
    slice_param->bits_offset[0] = 0;
    slice_param->bits_offset[1] = 0;
    slice_param->bytes_pos[0] = pic_param->stream_base_addr;
    slice_param->bytes_pos[1] = 0;
    slice_param->dependent_slice_segment_flag = 0;

    if (hal_h265_write_slice_msg(slot_vir_addr, pic_param, task_mem, slice_param, next_slice_phy_addr) != DEC_OK) {
        return DEC_ERR;
    }

    slice_param->valid_bitlen[0] = valid_bitlen[0];
    slice_param->valid_bitlen[1] = valid_bitlen[1];
    slice_param->bits_offset[0] = bits_offset[0];
    slice_param->bits_offset[1] = bits_offset[1];
    slice_param->bytes_pos[0] = bytes_pos[0];
    slice_param->bytes_pos[1] = bytes_pos[1];
    slice_param->slice_segment_address = slice_segment_address;
    slice_param->slice_segment_tile_address = slice_tile_address;
    slice_param->end_ctb_in_slice_raster = end_ctb_in_slice_raster;
    slice_param->end_ctb_in_slice_tile = end_ctb_in_slice_tile;
    slice_param->dependent_slice_segment_flag = dependent_slice_segment_flag;

    return DEC_OK;
}

/*  interface  */
td_s32 hal_h265_fill_ext_mem(td_void *mem, UADDR base_addr, dec_hal_ext_mem_type type)
{
    UADDR phy_addr = base_addr;
    vdh_ext_mem_info *vdh_mem = TD_NULL;
    td_u8 *h265_mn_vir_addr = TD_NULL;

    vdh_mem = (vdh_ext_mem_info *)mem;

    vfmw_check_ret((vdh_mem != TD_NULL), 0);

    if (type == DEC_HAL_EXT_MEM_DYNAMIC) {
        arrange_ptr(phy_addr, vfmw_align_up(HEVC_SED_TOP_ADDR_LEN, VDH_ADDR_ALIGN_16), vdh_mem->h265_sed_top_addr);
        arrange_ptr(phy_addr, vfmw_align_up(HEVC_PMV_TOP_ADDR_LEN, VDH_ADDR_ALIGN_16), vdh_mem->h265_pmv_top_addr);
        arrange_ptr(phy_addr, vfmw_align_up(HEVC_RCN_TOP_ADDR_LEN, VDH_ADDR_ALIGN_16), vdh_mem->h265_rcn_top_addr);
        arrange_ptr(phy_addr, vfmw_align_up(HEVC_DBLK_TOP_ADDR_LEN, VDH_ADDR_ALIGN_16), vdh_mem->h265_dblk_top_addr);
        arrange_ptr(phy_addr, vfmw_align_up(HEVC_SAO_TOP_ADDR_LEN, VDH_ADDR_ALIGN_16), vdh_mem->h265_sao_top_addr);
    } else {
        arrange_ptr(phy_addr, vfmw_align_up(HEVC_PMV_LEFT_ADDR_LEN, VDH_ADDR_ALIGN_16), vdh_mem->h265_pmv_left_addr);
        arrange_ptr(phy_addr, vfmw_align_up(HEVC_DBLK_LEFT_ADDR_LEN, VDH_ADDR_ALIGN_16), vdh_mem->h265_dblk_left_addr);
        arrange_ptr(phy_addr, vfmw_align_up(HEVC_SAO_LEFT_ADDR_LEN, VDH_ADDR_ALIGN_16), vdh_mem->h265_sao_left_addr);
        arrange_ptr(phy_addr, vfmw_align_up(HEVC_CMP_LEFT_ADDR_LEN, VDH_ADDR_ALIGN_16), vdh_mem->h265_cmp_left_addr);
        if (vdh_mem->base_phy_addr != 0 && vdh_mem->base_vir_addr != TD_NULL) {
            h265_mn_vir_addr = vdh_mem->h265_mn_phy_addr - vdh_mem->base_phy_addr + vdh_mem->base_vir_addr;
            hal_h265_write_cabac_table(h265_mn_vir_addr);
        }
    }

    return (td_s32)(phy_addr - base_addr);
}

td_s32 hal_h265_process_slice(td_void *pic, td_void *head, td_u16 slice_num, td_void *slice_list)
{
    dec_pic_param_h265 *pic_param = (dec_pic_param_h265 *)pic;
    dec_slice_param_h265 *slice_head = (dec_slice_param_h265 *)head;
    dec_slice_param_h265 *p = slice_head;
    td_u16 i;

    vfmw_check_ret((pic_param != TD_NULL), DEC_ERR);
    vfmw_check_ret((slice_head != TD_NULL), DEC_ERR);
    vfmw_check_ret((slice_list != TD_NULL), DEC_ERR);

    for (i = 0; i < slice_num - 1; i++, p = p->next_slice) {
        p->last_slice_in_group = !p->next_slice->dependent_slice_segment_flag;
    }
    p->last_slice_in_group = 1;

    hal_h265_split_slice_list(pic_param, slice_head, slice_num, slice_list);

    return DEC_OK;
}

STATIC td_void hal_h265_split_msg_slot(const td_void *vdh_msg, td_void *list)
{
    dec_chan_slice_list *slice_list = (dec_chan_slice_list *)list;
    const vdh_msg_info *msg = (vdh_msg_info *)vdh_msg;
    td_u32 slice_num;
    td_u32 dn_msg_ofs = 0;
    td_u32 up_msg_ofs = 0;
    td_u32 i;

    slice_list->dn_msg_one_size = msg->slice_msg_one_size;

    for (i = 0; i < slice_list->list_num && i < VDH_BSP_NUM; i++) {
        slice_num = slice_list->list_len[i];

        slice_list->dn_msg_phy_addr[i] = msg->slice_msg_phy_addr + dn_msg_ofs;
        slice_list->dn_msg_vir_addr[i] = msg->slice_msg_vir_addr + dn_msg_ofs;

        slice_list->up_msg_phy_addr[i] = msg->up_msg_phy_addr + up_msg_ofs;
        slice_list->up_msg_vir_addr[i] = msg->up_msg_vir_addr + up_msg_ofs;

        if (i == 0) {
            slice_num += slice_list->fake_slice;
        }

        dn_msg_ofs += msg->slice_msg_one_size * slice_num;
        up_msg_ofs += vfmw_align_up(slice_num * DEC_BASE_UPMSG_SLOT_LEN, DEC_DEV_VDH_UP_MSG_ALIGN);
    }

    return;
}

STATIC td_void hal_h265_write_pub_msg_d2(td_u32 *slot, const dec_pic_param_h265 *pic_param,
    const dec_chan_mem_param *task_mem, dps_info *dps)
{
    td_u32 total_ctb, d32;

#ifdef VFMW_HEVC_SED_ONLY_SUPPORT
    const td_u32 work_mode = dps->sed_state == SED_TRY ? WORK_SEDONLY : WORK_NORMAL;
#else
    const td_u32 work_mode = WORK_NORMAL;
    vfmw_unused(dps);
#endif

    vfmw_unused(task_mem);

    total_ctb = (pic_param->pic_hight_in_ctbs_y_minus1 + 1) * (pic_param->pic_width_in_ctbs_y_minus1 + 1) - 1;

    d32 = 0;
    ((vmsg_com_b0_d2 *)(&d32))->mbamt_to_dec = total_ctb;
    ((vmsg_com_b0_d2 *)(&d32))->marker_bit_detect_en = 0;
    ((vmsg_com_b0_d2 *)(&d32))->ac_last_detect_en = 0;
    ((vmsg_com_b0_d2 *)(&d32))->coef_idx_detect_en = 1;
    ((vmsg_com_b0_d2 *)(&d32))->vop_type_detect_en = 0;
    ((vmsg_com_b0_d2 *)(&d32))->work_mode = work_mode;
    ((vmsg_com_b0_d2 *)(&d32))->luma_sum_en = 1;
    ((vmsg_com_b0_d2 *)(&d32))->luma_histogram_en = 0;
    ((vmsg_com_b0_d2 *)(&d32))->load_qmatrix_flag = pic_param->scaling_list_enabled_flag;
    wr_msgword(slot + 2, d32); /* 2 :deviation */
}

STATIC td_void hal_h265_write_pub_msg_d3(td_u32 *slot, const dec_chan_mem_param *task_mem)
{
    td_u32 d32;

    d32 = 0;
    ((vmsg_com_b0_d3 *)(&d32))->video_standard = 0xD;
    ((vmsg_com_b0_d3 *)(&d32))->vdh_mdma_sel = 0;   /* 0 :vdh */
    ((vmsg_com_b0_d3 *)(&d32))->uv_order_en = task_mem->uv_order_en;
    ((vmsg_com_b0_d3 *)(&d32))->fst_slc_grp = 1;
    ((vmsg_com_b0_d3 *)(&d32))->mv_output_en = 1;
    ((vmsg_com_b0_d3 *)(&d32))->max_slcgrp_num = 3; /* 3 is number of slice group */
    ((vmsg_com_b0_d3 *)(&d32))->line_num_output_en = 0;
    ((vmsg_com_b0_d3 *)(&d32))->vdh_2d_en = task_mem->vdh_2d_en;
    ((vmsg_com_b0_d3 *)(&d32))->compress_en = task_mem->compress_en;

    if (task_mem->pmv_phy_addr == 0) {
        ((vmsg_com_b0_d3 *)(&d32))->mv_output_en = 0;
    }

    wr_msgword(slot + 3, d32); /* 3 :deviation */
}

STATIC td_void hal_h265_write_pub_msg_d21_d30(td_u32 *slot, vdh_ext_mem_info *vdh_mem, dec_chan_mem_param *task_mem)
{
    td_u32 d32;

    if (task_mem->pxp_in_ram == 1) {
        d32 = 0;
    } else {
        d32 = (td_u32)hw_addr_shift(vdh_mem->h265_sed_top_addr);
    }
    wr_msgword(slot + 21, d32); /* 21 :deviation */

    if (task_mem->pxp_in_ram == 1) {
        d32 = (td_u32)hw_addr_shift(HEVC_PMV_TOP_INTER_OFFSET);
    } else {
        d32 = (td_u32)hw_addr_shift(vdh_mem->h265_pmv_top_addr);
    }
    wr_msgword(slot + 22, d32); /* 22 :deviation */

    if (task_mem->pxp_in_ram == 1) {
        d32 = (td_u32)hw_addr_shift(HEVC_RCN_TOP_INTER_OFFSET);
    } else {
        d32 = (td_u32)hw_addr_shift(vdh_mem->h265_rcn_top_addr);
    }
    wr_msgword(slot + 24, d32); /* 24 :deviation */

    d32 = (td_u32)hw_addr_shift(vdh_mem->h265_mn_phy_addr);
    wr_msgword(slot + 25, d32); /* 25 :deviation */

    d32 = (td_u32)hw_addr_shift(vdh_mem->h265_pmv_left_addr);
    wr_msgword(slot + 26, d32); /* 26 :deviation */

    d32 = (td_u32)hw_addr_shift(vdh_mem->h265_sao_left_addr);
    wr_msgword(slot + 27, d32); /* 27 :deviation */

    d32 = (td_u32)hw_addr_shift(vdh_mem->h265_dblk_left_addr);
    wr_msgword(slot + 28, d32); /* 28 :deviation */

    if (task_mem->pxp_in_ram == 1) {
        d32 = (td_u32)hw_addr_shift(HEVC_SAO_TOP_INTER_OFFSET);
    } else {
        d32 = (td_u32)hw_addr_shift(vdh_mem->h265_sao_top_addr);
    }
    wr_msgword(slot + 29, d32); /* 29 :deviation */

    if (task_mem->pxp_in_ram == 1) {
        d32 = (td_u32)hw_addr_shift(HEVC_DBLK_TOP_INTER_OFFSET);
    } else {
        d32 = (td_u32)hw_addr_shift(vdh_mem->h265_dblk_top_addr);
    }
    wr_msgword(slot + 30, d32); /* 30 :deviation */
}

STATIC td_void hal_h265_write_pub_msg(td_u8 *slot_vir_addr, dps_info *dps, td_void *chan_mem,
    const td_void *ext_mem)
{
    dec_pic_param_h265 *pic_param = (dec_pic_param_h265 *)dps->pic_param;
    dec_chan_mem_param *task_mem = (dec_chan_mem_param *)chan_mem;
    vdh_ext_mem_info *vdh_mem = (vdh_ext_mem_info *)ext_mem;
    td_u32 *slot = (td_u32 *)slot_vir_addr;
    td_u32 d32;

    hal_h265_write_pub_msg_d2(slot, pic_param, task_mem, dps);
    hal_h265_write_pub_msg_d3(slot, task_mem);

    d32 = (td_u32)hw_addr_shift(pic_param->stream_base_addr);
    wr_msgword(slot + 6, d32); /* 6 :deviation */

    hal_h265_write_pub_msg_d21_d30(slot, vdh_mem, task_mem);

    d32 = (pic_param->pic_hight_in_ctbs_y_minus1 << 16) | /* 16 is bit count */
          pic_param->pic_width_in_ctbs_y_minus1;
    wr_msgword(slot + 31, d32); /* 31 :deviation */

    d32 = (((td_u32)pic_param->num_tile_rows_minus1 << 16)) | /* 16 is bit count */
          (td_u32)pic_param->num_tile_columns_minus1;
    wr_msgword(slot + 32, d32); /* 32 :deviation */

    d32 = hw_addr_shift(vdh_mem->h265_cmp_left_addr);
    wr_msgword(slot + 34, d32); /* 34 :a number */

    d32 = HEVC_CMP_LEFT_ADDR_LEN;
    wr_msgword(slot + 35, d32); /* 35 :a number */

    return;
}

STATIC td_void hal_h265_write_pic_msg_burst1(td_u32 *slot)
{
    td_u32 d32, i;

    d32 = 1 << 30; /* 30 is bit count */
    wr_msgword(slot + 0, d32);

    d32 = 1 << 27; /* 27 is bit count */
    wr_msgword(slot + 12, d32); /* 12 is d12 */

    /* D23~D63 If shvc is not supported, set this parameters to 0. */
    d32 = 0;
    for (i = 0; i < 41; i++) { /* 41 is length num */
        wr_msgword(slot + 23 + i, d32); /* 23 is D23 */
    }

    return;
}

STATIC td_void hal_h265_write_pic_msg_d0_d3(td_u32 *slot, const dec_pic_param_h265 *pic_param)
{
    const td_u32 colmv_prefetch_mode = 0;
    td_u32 d32;

    /*  burst 0  */
    /*  D0  */
    d32 = (colmv_prefetch_mode << 31) | /* 31 is bit count */
          (pic_param->strong_intra_smoothing_enabled_flag << 25) | /* 25 is bit count */
          (pic_param->scaling_list_enabled_flag << 24) | /* 24 is bit count */
          (pic_param->pcm_loop_filter_disabled_flag << 23) | /* 23 is bit count */
          (pic_param->sample_adaptive_offset_enabled_flag << 22) | /* 22 is bit count */
          (pic_param->amp_enabled_flag << 21) | /* 21 is bit count */
          (pic_param->chroma_format_idc << 19) | /* 19 is bit count */
          (pic_param->pcm_enabled_flag << 18) | /* 18 is bit count */
          (pic_param->pic_hight_in_ctbs_y_minus1 << 9) | /* 9 is bit count */
          pic_param->pic_width_in_ctbs_y_minus1;
    wr_msgword(slot, d32);

    /*  D1  */
    d32 = pic_param->this_poc;
    wr_msgword(slot + 1, d32);

    /*  D2  */
    d32 = (pic_param->log2_parallel_merge_level_minus2 << 29) | /* 29 is bit count */
          (pic_param->weighted_bipred_flag << 28) | /* 28 is bit count */
          (pic_param->weighted_pred_flag << 27) | /* 27 is bit count */
          (pic_param->max_transform_hierarchy_depth_inter << 24) | /* 24 is bit count */
          (pic_param->max_transform_hierarchy_depth_intra << 21) | /* 21 is bit count */
          (pic_param->log2_min_ipcm_cb_size_y << 18) | /* 18 is bit count */
          (pic_param->log2_max_ipcm_cb_size_y << 15) | /* 15 is bit count */
          (pic_param->log2_min_trafo_size << 12) | /* 12 is bit count */
          (pic_param->log2_max_trafo_size << 9) | /* 9 is bit count */
          (pic_param->max_cu_depth << 6) | /* 6 is bit count */
          (pic_param->log2_min_cb_size_y << 3) | /* 3 is bit count */
          pic_param->log2_ctb_size_y;
    wr_msgword(slot + 2, d32); /* 2 :deviation */

    /*  D3  */
    d32 = (pic_param->qp_bd_offset_c << 22) | /* 22 is bit count */
          (pic_param->qp_bd_offset_y << 16) | /* 16 is bit count */
          (pic_param->pcm_bit_depth_c << 12) | /* 12 is bit count */
          (pic_param->pcm_bit_depth_y << 8) | /* 8 is bit count */
          (pic_param->bit_depth_c << 4) | /* 4 is bit count */
          pic_param->bit_depth_y;
    wr_msgword(slot + 3, d32); /* 3 :deviation */
}


STATIC td_void hal_h265_write_pic_msg(td_u8 *slot_vir_addr, const td_void *pic, const td_void *task_mem,
    const td_void *vdh_mem, const td_void *msg)
{
    const dec_pic_param_h265 *pic_param = (dec_pic_param_h265 *)pic;
    vdh_msg_info *vdh_msg = (vdh_msg_info *)msg;
    td_u32 *slot = (td_u32 *)slot_vir_addr;
    td_u32 d32, i;

    vfmw_unused(task_mem);
    vfmw_unused(vdh_mem);

    /* burst 0 */
    hal_h265_write_pic_msg_d0_d3(slot, pic_param);

    /*  D8~D23  */
    for (i = 0; i < pic_param->apc_size; i++) {
        d32 = (td_u32)hw_addr_shift(pic_param->apc_fsp_phy_addr[i]);
        wr_msgword(slot + 8 + i, d32); /* 8 :deviation */
    }

    /*  D26~D41  */
    for (i = 0; i < 16; i++) { /* 16 :for loop count */
        d32 = pic_param->apc_poc[i];
        wr_msgword(slot + 26 + i, d32); /* 26 :deviation */
    }

    /*  D54  */
    /* 16 is bit count */
    d32 = (pic_param->pic_height_in_luma_samples << 16) | pic_param->pic_width_in_luma_samples;
    wr_msgword(slot + 54, d32); /* 54 :deviation */

    /*  D55  */
    if (pic_param->tiles_enabled_flag == 1 &&
        hal_h265_write_tile_info(pic_param, vdh_msg->tile_info_vir_addr) != DEC_OK) {
        return;
    }

    d32 = (td_u32)hw_addr_shift(vdh_msg->tile_info_phy_addr);
    wr_msgword(slot + 55, d32); /* 55 :deviation */

    /*  D60  */
    d32 = (pic_param->loop_filter_across_tiles_enabled_flag << 24) | /* 24 is an index */
          (pic_param->transform_skip_enabled_flag << 23) | /* 23 is bit count */
          (pic_param->sign_data_hiding_enabled_flag << 22) | /* 22 is bit count */
          (pic_param->entropy_coding_sync_enabled_flag << 21) | /* 21 is bit count */
          (pic_param->uniform_spacing_flag << 20) | /* 20 is bit count */
          (pic_param->tiles_enabled_flag << 19) | /* 19 is bit count */
          (pic_param->log2_min_cu_qp_delta_size << 16) | /* 16 is bit count */
          (pic_param->diff_cu_qp_delta_depth << 13) | /* 13 is bit count */
          (pic_param->cu_qp_delta_enabled_flag << 12) | /* 12 is bit count */
          (pic_param->transquant_bypass_enabled_flag << 11) | /* 11 is bit count */
          (pic_param->constrained_intra_pred_flag << 10) | /* 10 is bit count */
          ((((td_u32)(pic_param->pps_cb_qp_offset)) & 0x1f) << 5) | /* 5 is bit count */
         (((td_u32)(pic_param->pps_cr_qp_offset)) & 0x1f);
    wr_msgword(slot + 60, d32); /* 60 :deviation */

    /*  D61  */
    /* 16 is bit count */
    d32 = ((td_u32)(pic_param->num_tile_rows_minus1) << 16) | (td_u32)(pic_param->num_tile_columns_minus1);
    wr_msgword(slot + 61, d32); /* 61 :deviation */

    /*  burst 1  */
    slot = slot + 64; /* 64 is burst length */
    hal_h265_write_pic_msg_burst1(slot);

    if (pic_param->scaling_list_enabled_flag == 1) {
        /*  burst 2 ~ burst 5  */
        hal_h265_write_qmatrix(pic_param->qmatrix, 4 * 64, slot + 64); /* 4 and 64 are multiplier, 64 :deviation */
    }

    return;
}

STATIC VFMW_ACC_SLICE_H265 td_void hal_h265_write_slice_msg_d6_d10(td_u32 *slot,
    const dec_pic_param_h265 *pic_param, const dec_slice_param_h265 *slice_param)
{
    td_u32 d32, slice_addr_x, slice_addr_y;
    const td_u32 s_slice_type_for_pmv[3] = { 2, 1, 0 }; /* 3 :a number */

    /*  D6  */
    d32 = (slice_param->is_low_delay << 27) | /* 27 is bit count */
          (slice_param->dependent_slice_segment_flag << 26) | /* 26 is bit count */
          ((((td_u8)(slice_param->slice_qp_y)) & 0x7f) << 19) | /* 19 is bit count */
          (slice_param->cabac_init_flag << 18) | /* 18 is bit count */
          slice_param->independent_slice_segment_addr;
    wr_msgword(slot + 6, d32); /* 6 :deviation */

    /*  D7  */
    d32 = (slice_param->col_pic_apc << 28) | /* 28 is bit count */
          (slice_param->collocated_from_l0_flag << 27) | /* 27 is bit count */
          (slice_param->max_num_merge_cand << 24) | /* 24 is bit count */
          (slice_param->num_ref_idx_l1_active_minus1 << 20) | /* 20 is bit count */
          (slice_param->num_ref_idx_l0_active_minus1 << 16) | /* 16 is bit count */
          (slice_param->slice_temporal_mvp_enabled_flag << 15) | /* 15 is bit count */
          (slice_param->mvd_l1_zero_flag << 14) | /* 14 is bit count */
          (slice_param->listx_size[1] << 8) | /* 8 is bit count */
          (slice_param->listx_size[0] << 2) | /* 2 is bit count */
          s_slice_type_for_pmv[slice_param->slice_type];
    wr_msgword(slot + 7, d32); /* 7 :deviation */

    /*  D8  */
    d32 = slice_param->slice_segment_address;
    wr_msgword(slot + 8, d32); /* 8 :deviation */

    /*  D9  */
    slice_addr_x = slice_param->slice_segment_address % (pic_param->pic_width_in_ctbs_y_minus1 + 1);
    slice_addr_y = slice_param->slice_segment_address / (pic_param->pic_width_in_ctbs_y_minus1 + 1);
    d32 = (slice_addr_y << 16) | slice_addr_x; /* 16 is bit count */
    wr_msgword(slot + 9, d32); /* 9 :deviation */

    /*  D10  */
    d32 = slice_param->slice_segment_tile_address;
    wr_msgword(slot + 10, d32); /* 10 :deviation */
}

STATIC VFMW_ACC_SLICE_H265 td_void hal_h265_write_slice_msg_d11_d20(td_u32 *slot,
    const dec_pic_param_h265 *pic_param, const dec_slice_param_h265 *slice_param)
{
    td_u32 d32, i;

    vfmw_unused(pic_param);

    /*  D11  */
    d32 = (((td_u32)slice_param->slice_cb_qp_offset & 0x1f) << 8) | /* 8 is bit count */
          ((td_u32)slice_param->slice_cr_qp_offset & 0x1f);
    wr_msgword(slot + 11, d32); /* 11 :deviation */

    /*  D12  */
    d32 = (((td_u32)slice_param->slice_tc_offset_div2 & 0xf) << 16) | /* 16 is bit count */
          (((td_u32)slice_param->slice_beta_offset_div2 & 0xf) << 8) | /* 8 is bit count */
          (slice_param->slice_sao_luma_flag << 3) | /* 3 is bit count */
          (slice_param->slice_sao_chroma_flag << 2) | /* 2 is an index */
          (slice_param->slice_deblocking_filter_disabled_flag << 1) |
          slice_param->slice_loop_filter_across_slices_enabled_flag;
    wr_msgword(slot + 12, d32); /* 12 :deviation */

    /*  D13 list0  */
    d32 = 0;
    for (i = 0; i < slice_param->listx_size[0]; i++) {
        d32 |= (((td_u32)slice_param->is_long_term[0][i]) << i);
    }
    wr_msgword(slot + 13, d32); /* 13 :deviation */

    d32 = 0;
    for (i = 0; i < slice_param->listx_size[1]; i++) {
        d32 |= (((td_u32)slice_param->is_long_term[1][i]) << i);
    }
    wr_msgword(slot + 14, d32); /* 14 :deviation */
}


STATIC VFMW_ACC_SLICE_H265 td_s32 hal_h265_write_slice_msg(td_u8 *slot_vir_addr, td_void *pic, td_void *chan_mem,
    td_void *slice, UADDR next_slice_phy_addr)
{
    dec_pic_param_h265 *pic_param = (dec_pic_param_h265 *)pic;
    dec_slice_param_h265 *slice_param = (dec_slice_param_h265 *)slice;
    dec_chan_mem_param *task_mem = (dec_chan_mem_param *)chan_mem;
    td_u32 *slot = (td_u32 *)slot_vir_addr;
    td_u32 slice_addr_x, slice_addr_y;
    td_u32 d32;
    td_s32 ret;

    /*  burst 0  */
    ret = hal_h265_write_stream_info(pic_param, slice_param, slot);
    if (ret != DEC_OK) {
        return DEC_ERR;
    }

    hal_h265_write_slice_msg_d6_d10(slot, pic_param, slice_param);
    hal_h265_write_slice_msg_d11_d20(slot, pic_param, slice_param);

    hal_h265_write_apc_index(slice_param, slot);

    /*  D23 MvAddr  */
    if (slice_param->apc_2_mv_addr == 0) {
        /*  This is only to avoid VDH bug that may not return interrupt if D23 is zero
         * This bug only exists in VDH
         * and will fix in later revision  */
        slice_param->apc_2_mv_addr = task_mem->luma_phy_addr;
    }
    d32 = (td_u32)hw_addr_shift(slice_param->apc_2_mv_addr);
    wr_msgword(slot + 23, d32); /* 23 :deviation */

    /*  D24  */
    /* config err check and repair online. 1:end of stream err 3:all err 14:qp err */
    d32 = (1 << 14) | /* 14 is bit count */
          ((pic_param->qp_bd_offset_y / 2) << 8) | /* 2 :for half 8 is bit count */
          (1 << 3) | /* 3 is bit count */
          (1 << 1);
    wr_msgword(slot + 24, d32); /* 24 :deviation */

    /*  D25  */
    wr_msgword(slot + 25, 0);
    /*  D26  */
    wr_msgword(slot + 26, 0);

    d32 = slice_param->end_ctb_in_slice_raster;
    wr_msgword(slot + 43, d32); /* 43 :deviation */

    d32 = slice_param->end_ctb_in_slice_tile;
    wr_msgword(slot + 44, d32); /* 44 :deviation */

    /*  D62  */
    slice_addr_x = slice_param->end_ctb_in_slice_raster % (pic_param->pic_width_in_ctbs_y_minus1 + 1);
    slice_addr_y = slice_param->end_ctb_in_slice_raster / (pic_param->pic_width_in_ctbs_y_minus1 + 1);
    d32 = ((td_u32)(slice_param->last_slice_in_group) << 31) | /* 31 is bit count */
          (slice_addr_y << 16) | /* 16 is bit count */
          slice_addr_x;
    wr_msgword(slot + 62, d32); /* 62 :deviation */

    /*  D63  */
    d32 = (td_u32)hw_addr_shift(next_slice_phy_addr);
    wr_msgword(slot + 63, d32); /* 63 :deviation */

    /*  burst 1 and burst 2  */
    hal_h265_write_wp_info(pic_param, slice_param, slot);

    return DEC_OK;
}

#ifdef VFMW_HEVC_SED_ONLY_SUPPORT
STATIC td_s32 hal_h265_set_sed_only(td_void *pic, td_void *task_mem, td_void *list)
{
    dec_pic_param_h265 *pic_param = (dec_pic_param_h265 *)pic;
    dec_chan_slice_list *slice_list = (dec_chan_slice_list *)list;
    dec_slice_param_h265 **list_head = (dec_slice_param_h265 **)&slice_list->list_head[0];
    dec_slice_param_h265 *slice_param = TD_NULL;
    dec_slice_param_h265 *target_slice = TD_NULL;

    td_s32 slice_idx;
    td_s32 indepen_slc_idx = 0;
    td_u16 slice_num = slice_list->list_len[0];
    td_u8 *curr_slot_vir_addr = slice_list->dn_msg_vir_addr[0];
    UADDR next_slot_phy_addr = slice_list->dn_msg_phy_addr[0] + slice_list->dn_msg_one_size;

    if (slice_list->fake_slice == 1) {
        slice_num--;
        dprint(PRN_ERROR, "SedOnly First SliceMb != 0\n");
    }

    slice_param = list_head[0];
    for (slice_idx = 0; slice_idx < slice_num; slice_idx++) {
        if (slice_param->dependent_slice_segment_flag == 0) {
            indepen_slc_idx = slice_idx;
            target_slice = slice_param;
        }
        slice_param = slice_param->next_slice;
    }

    if (target_slice == TD_NULL) {
        return DEC_ERR;
    }

    slice_param = target_slice;
    for (slice_idx = 0; slice_idx < slice_num - indepen_slc_idx; slice_idx++) {
        if (slice_idx >= slice_num - indepen_slc_idx - 1) {
            next_slot_phy_addr = 0;
        }

        if (hal_h265_write_slice_msg(curr_slot_vir_addr, pic_param,
                                     task_mem, slice_param, next_slot_phy_addr) != DEC_OK) {
            dprint(PRN_ERROR, "%s: set slice msg failed!\n", __func__);
            return DEC_ERR;
        }

        slice_param = slice_param->next_slice;
        curr_slot_vir_addr += slice_list->dn_msg_one_size;
        next_slot_phy_addr += slice_list->dn_msg_one_size;
    }

    return DEC_OK;
}
#endif

STATIC VFMW_ACC_SLICE_H265 td_s32 hal_h265_write_slice_list(td_void *pic, td_void *task_mem, td_void *list)
{
    dec_pic_param_h265 *pic_param = (dec_pic_param_h265 *)pic;
    dec_chan_slice_list *slice_list = (dec_chan_slice_list *)list;
    dec_slice_param_h265 **list_head = (dec_slice_param_h265 **)&slice_list->list_head[0];
    dec_slice_param_h265 *slice_param = TD_NULL;

    td_u8 *curr_slot_vir_addr = TD_NULL;
    UADDR next_slot_phy_addr;
    td_u16 *list_len = &slice_list->list_len[0];
    td_u16 list_num = slice_list->list_num;
    td_u32 list_idx, slice_num, slice_idx, slot_full;
    td_u32 slice_cnt = 0;

    curr_slot_vir_addr = slice_list->dn_msg_vir_addr[0];
    next_slot_phy_addr = slice_list->dn_msg_phy_addr[0] + slice_list->dn_msg_one_size;

    if (slice_list->fake_slice != 0) {
        slice_param = list_head[0];
        dprint(PRN_ERROR, "first slice is %d, insert a fake slice!\n", slice_param->slice_segment_address);

        if (hal_h265_write_fake_slice(curr_slot_vir_addr,
                                      pic_param, task_mem, slice_param, next_slot_phy_addr) != DEC_OK) {
            dprint(PRN_ERROR, "DEC insert a fake slice failed!\n");
            return DEC_ERR;
        }

        slice_cnt++;
        curr_slot_vir_addr += slice_list->dn_msg_one_size;
        next_slot_phy_addr += slice_list->dn_msg_one_size;
    }

    for (list_idx = 0; list_idx < list_num; list_idx++) {
        slice_param = list_head[list_idx];
        slice_num = list_len[list_idx] - (list_idx == 0 ? slice_list->fake_slice : 0);

        for (slice_idx = 0; slice_idx < slice_num; slice_idx++, slice_cnt++) {
            slot_full = (slice_cnt == HEVC_MAX_SLICE_NUM - 1) ? 1 : 0;

            if (slice_idx == slice_num - 1 || slot_full) {
                next_slot_phy_addr = 0;
            }

            if (hal_h265_write_slice_msg(curr_slot_vir_addr,
                                         pic_param, task_mem, slice_param, next_slot_phy_addr) != DEC_OK) {
                return DEC_ERR;
            }

            if (slot_full == 1) {
                return DEC_OK;
            }

            slice_param = slice_param->next_slice;
            curr_slot_vir_addr += slice_list->dn_msg_one_size;
            next_slot_phy_addr += slice_list->dn_msg_one_size;
        }
    }

    return DEC_OK;
}

VFMW_ACC_SLICE_H265 td_s32 hal_h265_make_slice_ref(td_u32 chan_id, td_void *pic, td_void *slice)
{
    dec_pic_param_h265 *pic_param = (dec_pic_param_h265 *)pic;
    dec_slice_param_h265 *slice_param = (dec_slice_param_h265 *)slice;

    vfmw_check_ret((pic_param != TD_NULL), DEC_ERR);
    vfmw_check_ret((slice_param != TD_NULL), DEC_ERR);

    dec_ref_make_idx_2_apc(pic_param->apc_size, &pic_param->apc_fsp_id[0], vfmw_min(slice_param->listx_size[0],
        HEVC_APC_SIZE), &slice_param->idx_2_fsp_id[0][0], &slice_param->idx_2_apc[0][0]);
    dec_ref_make_idx_2_apc(pic_param->apc_size, &pic_param->apc_fsp_id[0], vfmw_min(slice_param->listx_size[1],
        HEVC_APC_SIZE), &slice_param->idx_2_fsp_id[1][0], &slice_param->idx_2_apc[1][0]);

    dec_ref_make_col_pic_apc(pic_param->apc_size, &pic_param->apc_fsp_id[0], slice_param->col_pmv_fsp_id,
                             &slice_param->col_pic_apc);
    dec_ref_make_col_pmv_addr(chan_id, slice_param->col_pmv_fsp_id, &slice_param->apc_2_mv_addr);

    return DEC_OK;
}

STATIC td_void dec_hal_write_core_msg_burst5(td_u32 *slot, dec_pic_param_h265 *pic_param,
    dec_chan_mem_param *task_mem)
{
    td_u32 i, data;

    /* burst 5 */
    for (i = 0; i < pic_param->apc_size; i++) {
        data = (td_u32)hw_addr_shift(pic_param->apc_fsp_phy_addr[i]);
        wr_msgword(slot + i, data);
        data = (td_u32)(pic_param->apc_fsp_phy_addr[i] + task_mem->uv_offset * 3 / 2); /* 3 is multiplier 2 :for half */
        data = hw_addr_shift(data);
        wr_msgword(slot + 16 + i, data); /* 16 is an address */
        data = (td_u32)(pic_param->apc_fsp_phy_addr[i] + task_mem->nbit_y_offset);
        data = hw_addr_shift(data);
        wr_msgword(slot + 32 + i, data); /* 32 :deviation */
    }

    for (i = pic_param->apc_size; i < 16; i++) { /* 16 :for loop count */
        data = (td_u32)pic_param->apc_fsp_phy_addr[0];
        data = hw_addr_shift(data);
        wr_msgword(slot + i, data);
        data = (td_u32)(pic_param->apc_fsp_phy_addr[0] + task_mem->uv_offset * 3 / 2); /* 3 is multiplier 2 :for half */
        data = hw_addr_shift(data);
        wr_msgword(slot + 16 + i, data); /* 16 is a number */
        data = (td_u32)(pic_param->apc_fsp_phy_addr[0] + task_mem->nbit_y_offset);
        data = hw_addr_shift(data);
        wr_msgword(slot + 32 + i, data); /* 32 :deviation */
    }

    if (task_mem->vdh_2d_en == 0) {
        data = 0;
    } else {
        if (task_mem->linear_no_gap_en == TD_TRUE) {
            data = pic_param->top_offset | pic_param->bottom_offset << 16; /* 16 is bit count */
        } else {
            data = 0;
        }
    }

    wr_msgword(slot + 48, data); /* 48 :deviation */
    wr_msgword(slot + 49, data); /* 49 :deviation */
    wr_msgword(slot + 50, data); /* 50 :deviation */

    /* D51~D53 */
    data = (pic_param->pic_height_in_luma_samples << 16) | pic_param->pic_width_in_luma_samples; /* 16 is bit count */
    wr_msgword(slot + 51, data); /* 51 :deviation */
    wr_msgword(slot + 52, data); /* 52 :deviation */
    wr_msgword(slot + 53, data); /* 53 :deviation */

    /* D54~D56 uvoffset_1d */
    data = task_mem->uv_offset;
    wr_msgword(slot + 54, data); /* 54 :deviation */
    wr_msgword(slot + 55, data); /* 55 :deviation */
    wr_msgword(slot + 56, data); /* 56 :deviation */

    /* D57~D59 uvoffset_disp */
    if (task_mem->linear_no_gap_en == TD_TRUE) {
        data = task_mem->uv_offset_dis;
    } else {
        data = 0;
    }

    wr_msgword(slot + 57, data); /* 57 :deviation */
    wr_msgword(slot + 58, data); /* 58 :deviation */
    wr_msgword(slot + 59, data); /* 59 :deviation */

    /* D60~D62 uvoffset_2bit */
    data = task_mem->nbit_uv_offset;
    wr_msgword(slot + 60, data); /* 60 :deviation */
    wr_msgword(slot + 61, data); /* 61 :deviation */
    wr_msgword(slot + 62, data); /* 62 :deviation */
}

STATIC td_void hal_h265_write_core_msg(td_u8 *core_msg, td_void *task, td_void *pic)
{
    td_u32 data;
    td_u32 *slot = TD_NULL;
    dec_pic_param_h265 *pic_param = (dec_pic_param_h265 *)pic;
    dec_chan_task *chan_task = (dec_chan_task *)task;
    dec_chan_mem_param *task_mem = (dec_chan_mem_param *)(&chan_task->mem_param);

    slot = (td_u32 *)core_msg;

    slot += 64 * 5; /* 64 is standard strength 5 is multiplier */

    dec_hal_write_core_msg_burst5(slot, pic_param, task_mem);

    data = task_mem->y_stride;
    wr_msgword(slot + 63, data); /* 63 :deviation */
    wr_msgword(slot + 64 * 1 + 0, data); /* 64 :deviation */
    wr_msgword(slot + 64 * 1 + 1, data); /* 64 :deviation */

    data = task_mem->nbit_y_stride;
    wr_msgword(slot + 64 * 1 + 2, data); /* 64 :deviation 2 :deviation */
    wr_msgword(slot + 64 * 1 + 3, data); /* 64 :deviation 3 :deviation */
    wr_msgword(slot + 64 * 1 + 4, data); /* 64 :deviation 4 :deviation */

    /* ystaddr_disp */
    if (task_mem->linear_no_gap_en == TD_TRUE) {
        data = (td_u32)(task_mem->luma_phy_addr + task_mem->uv_offset * 3 / 2); /* 3 is multiplier 2 :for half */
    } else {
        data = (td_u32)(task_mem->luma_phy_addr + task_mem->y_stride * pic_param->pic_height_in_luma_samples);
    }
    data = hw_addr_shift(data);
    wr_msgword(slot + 64 * 1 + 5, data); /* 64 is standard strength 5 :deviation */

    data = task_mem->uv_offset_dis;
    wr_msgword(slot + 64 * 1 + 6, data); /* 64 is standard strength 6 :deviation */

    /* ystaddr_2bit */
    data = (td_u32)(task_mem->luma_phy_addr + task_mem->nbit_y_offset);
    data = hw_addr_shift(data);
    wr_msgword(slot + 64 * 1 + 7, data); /* 64 is standard strength 7 :deviation */

    if (task_mem->vdh_2d_en == 0) {
        data = 0;
    } else {
        if (task_mem->linear_no_gap_en == TD_TRUE) {
            data = pic_param->top_offset | pic_param->bottom_offset << 16; /* 16 is bit count */
        } else {
            data = 0;
        }
    }
    wr_msgword(slot + 64 * 1 + 8, data); /* 64 is standard strength 8 :deviation */

    return;
}

td_s32 hal_h265_check_slice_all_size(const td_void *vdh_msg, td_void *list)
{
    td_u32 i;
    td_u32 slice_num;
    td_u32 slice_msg_dn_need_all_size;
    td_u32 slice_msg_up_need_all_size;
    const vdh_msg_info *msg = (vdh_msg_info *)vdh_msg;
    dec_chan_slice_list *slice_list = (dec_chan_slice_list *)list;

    for (i = 0; i < slice_list->list_num && i < VDH_BSP_NUM; i++) {
        slice_num = slice_list->list_len[i];
        if (i == 0) {
            slice_num += slice_list->fake_slice;
        }

        slice_msg_dn_need_all_size = msg->slice_msg_one_size * slice_num;
        slice_msg_up_need_all_size = vfmw_align_up(slice_num * DEC_BASE_UPMSG_SLOT_LEN, DEC_DEV_VDH_UP_MSG_ALIGN);
        if (msg->slice_msg_all_size < slice_msg_dn_need_all_size) {
            dprint(PRN_ALWS, "%s, %d [dn need size %d, alloc size %d] is err\r\n", __func__, __LINE__,
                slice_msg_dn_need_all_size, msg->slice_msg_all_size);
            return DEC_ERR;
        }

        if (msg->slice_up_msg_all_size < slice_msg_up_need_all_size) {
            dprint(PRN_ALWS, "%s, %d [up need size %d, alloc size %d] is err\r\n", __func__, __LINE__,
                slice_msg_up_need_all_size, msg->slice_up_msg_all_size);
            return DEC_ERR;
        }
    }

    return DEC_OK;
}

td_s32 hal_h265_write_msg(const td_void *vdh_msg, td_void *chan_task, const td_void *dev_vdh)
{
    td_s32 ret;
    const vdh_msg_info *msg = (vdh_msg_info *)vdh_msg;
    const dec_dev_vdh *vdh = (dec_dev_vdh *)dev_vdh;
    dec_chan_task *task = (dec_chan_task *)chan_task;

    vfmw_check_ret((msg != TD_NULL), DEC_ERR);
    vfmw_check_ret((vdh != TD_NULL), DEC_ERR);
    vfmw_check_ret((task != TD_NULL), DEC_ERR);
    vfmw_check_ret((task->dps != TD_NULL), DEC_ERR);
    vfmw_check_ret((task->dps->pic_param != TD_NULL), DEC_ERR);
    vfmw_check_ret((msg->core_msg_vir_addr != TD_NULL), DEC_ERR);
    vfmw_check_ret((msg->pub_msg_vir_addr != TD_NULL), DEC_ERR);
    vfmw_check_ret((msg->pic_msg_vir_addr != TD_NULL), DEC_ERR);

    ret = hal_h265_check_slice_all_size(msg, &task->slice_list);
    vfmw_check_ret((ret == DEC_OK), DEC_ERR);

    hal_h265_split_msg_slot(msg, &task->slice_list);

    dec_hal_write_pub_msg(msg, task, vdh);

    dec_hal_write_core_msg(msg, task, vdh);

    hal_h265_write_core_msg(msg->core_msg_vir_addr, task, task->dps->pic_param);

    hal_h265_write_pub_msg(msg->pub_msg_vir_addr, task->dps,
                           &task->mem_param, &vdh->vdh_ext_mem);

    vfmw_check_ret((task->dps->pic_param != TD_NULL), DEC_ERR);
    hal_h265_write_pic_msg(msg->pic_msg_vir_addr, task->dps->pic_param,
                           &task->mem_param, &vdh->vdh_ext_mem, msg);

#ifdef VFMW_HEVC_SED_ONLY_SUPPORT
    if (task->dps->sed_state == SED_TRY) {
        hal_h265_set_sed_only(task->dps->pic_param, &task->mem_param, &task->slice_list);
        return DEC_OK;
    }
#endif

    vfmw_check_ret((task->dps->pic_param != TD_NULL), DEC_ERR);
    ret = hal_h265_write_slice_list(task->dps->pic_param, &task->mem_param, &task->slice_list);

    return ret;
}

STATIC td_void hal_h265_calc_map(dec_pic_param_h265 *pic_param, h265_tile *tile,
    td_u32 tile_idx_y, td_s32 *idx, td_s32 tile_y)
{
    td_s32 tile_x = 0;
    td_u32 i, j, tile_idx_x;
    td_u32 pic_width_in_ctb, max_width_mb, max_height_mb;

    pic_width_in_ctb = pic_param->pic_width_in_ctbs_y_minus1 + 1;
    max_width_mb = HEVC_MAX_PIX_WIDTH / HEVC_MB_SIZE;
    max_height_mb = HEVC_MAX_PIX_HEIGHT / HEVC_MB_SIZE;

    for (tile_idx_x = 0; tile_idx_x <= pic_param->num_tile_columns_minus1 &&
     tile_idx_x < HEVC_MAX_TILE_COLUMNS; tile_idx_x++) {
        if (tile->tile_num >= HEVC_MAX_TILE_NUM) {
            break;
        }

        tile->tile_group[tile->tile_num][0] = (*idx);

        for (i = 0; tile_idx_y < HEVC_MAX_TILE_ROWS && i < (td_u32)pic_param->row_bd[tile_idx_y] && i < max_height_mb;
            i++) {
            for (j = 0; j < (td_u32)pic_param->column_bd[tile_idx_x] && j < max_width_mb; j++) {
                tile->ts_to_rs_map[(*idx)] = (tile_y + i) * pic_width_in_ctb + tile_x + j;
                (*idx)++;
            }
        }

        tile->tile_group[tile->tile_num][1] = (*idx) - 1;
        tile->tile_num++;
        tile_x += pic_param->column_bd[tile_idx_x];
    }

    return;
}

STATIC td_void hal_h265_calc_tile_map(dec_pic_param_h265 *pic_param, h265_tile *tile)
{
    td_s32 idx = 0;
    td_u32 tile_y = 0;
    td_u32 tile_idx_y;

    tile->tile_num = 0;

    for (tile_idx_y = 0; tile_idx_y <= pic_param->num_tile_rows_minus1 &&
         tile_idx_y < HEVC_MAX_TILE_ROWS; tile_idx_y++) {
        hal_h265_calc_map(pic_param, tile, tile_idx_y, &idx, tile_y);
        tile_y += pic_param->row_bd[tile_idx_y];
    }

    return;
}

STATIC td_s32 hal_h265_tile_decode(const dec_chan_task *task, h265_tile *tile, dec_pic_param_h265 *pic_param)
{
    td_u32 j, k, ss_tile_end_addr;
    td_s32 i, num_slice_para;
    dec_slice_param_h265 *slc_param = TD_NULL;
    dec_slice_param_h265 *next_slc_param = TD_NULL;

    slc_param = (dec_slice_param_h265 *)task->dps->slice_head;
    num_slice_para = task->dps->slice_num;
    if (num_slice_para < 0 || num_slice_para > HEVC_MAX_SLICE_NUM) {
        dprint(PRN_ERROR, "NumSlicePara %d out of range[0 %d).\n", num_slice_para, HEVC_MAX_SLICE_NUM);
        return DEC_ERR;
    }

    for (i = 0; i < num_slice_para; i++) {
        slc_param = (dec_slice_param_h265 *)task->dps->slice_list[i];

        if ((i + 1) < num_slice_para) {
            next_slc_param = (dec_slice_param_h265 *)task->dps->slice_list[i + 1];
            ss_tile_end_addr = next_slc_param->slice_segment_tile_address - 1;
        } else {
            ss_tile_end_addr =
                (pic_param->pic_width_in_ctbs_y_minus1 + 1) * (pic_param->pic_hight_in_ctbs_y_minus1 + 1) - 1;
        }

        if (ss_tile_end_addr >= HEVC_MAX_MB_NUM) {
            dprint(PRN_ERROR, "Slice %d mb address %d out of range[0 %d).\n", i, ss_tile_end_addr, HEVC_MAX_MB_NUM);
            return DEC_ERR;
        }

        for (j = slc_param->slice_segment_tile_address; j <= ss_tile_end_addr; j++) {
            if (tile->is_decode[tile->ts_to_rs_map[j]] != 0) {
                continue;
            }

            for (k = slc_param->slice_segment_tile_address; k <= ss_tile_end_addr; k++) {
                tile->is_decode[tile->ts_to_rs_map[k]] = 0;
            }
            break;
        }
    }

    return DEC_OK;
}

STATIC td_void hal_h265_calc_repair_start_end_mb(dec_repair_param *repair_param, h265_tile *tile)
{
    td_u32 end_repair_mb, start_repair_mb;
    td_s32 i, total_mbn;

    i = 0;
    total_mbn = repair_param->image_width * repair_param->image_height;

    while (i < total_mbn) {
        if (tile->is_decode[i] == 1) {
            i++;
        } else {
            start_repair_mb = i;

            while ((i + 1 < total_mbn) && (tile->is_decode[i + 1] == 0)) {
                i++;
            }

            end_repair_mb = i;
            repair_param->mb_group[repair_param->valid_group_num].start_mbn = start_repair_mb;
            repair_param->mb_group[repair_param->valid_group_num].end_mbn = end_repair_mb;
            repair_param->valid_group_num++;
            i++;
        }

        if (repair_param->valid_group_num > (MAX_UP_MSG_SLICE_NUM - 1)) {
            dprint(PRN_ERROR, "pRepairParam->valid_group_num >(MAX_UP_MSG_SLICE_NUM - 1)\n");
            repair_param->mb_group[0].start_mbn = 0;
            repair_param->mb_group[0].end_mbn = total_mbn - 1;
            repair_param->valid_group_num = 1;
            return;
        }
    }

    return;
}

STATIC td_s32 hal_h265_get_mb_gp_of_tile(dec_repair_param *repair_param,
    dec_report_info *dec_report, const dec_chan_task *task, vdh_msg_info *msg)
{
    td_s32 total_mbn, ret;
    td_u32 i, j, rs_addr;
    h265_tile *tile = TD_NULL;
    dec_pic_param_h265 *pic_param = TD_NULL;

    pic_param = (dec_pic_param_h265 *)task->dps->pic_param;
    if (msg->base_mem_len < sizeof(h265_tile)) {
        dprint(PRN_FATAL, "%s: base_mem_len :0x%x sizeof:0x%x\n", __func__, msg->base_mem_len, sizeof(h265_tile));
        return DEC_ERR;
    }

    tile = (h265_tile *)(msg->base_vir_addr);
    vfmw_check_sec_func(memset_s(tile, sizeof(h265_tile), 0, sizeof(h265_tile)));
    total_mbn = repair_param->image_width * repair_param->image_height;

    repair_param->valid_group_num = 0;
    repair_param->full_repair = 0;

    hal_h265_calc_tile_map(pic_param, tile);

    for (i = 0; i < dec_report->slice_num; i++) {
        for (j = dec_report->slice_mb_range[i][0]; j <= dec_report->slice_mb_range[i][1]; j++) {
            if (dec_report->slice_mb_range[i][0] > (td_u32)total_mbn ||
                dec_report->slice_mb_range[i][1] > (td_u32)total_mbn) {
                continue;
            }

            rs_addr = tile->ts_to_rs_map[j];
            if (rs_addr < HEVC_MAX_MB_NUM) {
                tile->is_decode[rs_addr] = 1;
            }
        }
    }

    ret = hal_h265_tile_decode(task, tile, pic_param);
    if (ret != DEC_OK) {
        return DEC_ERR;
    }

    hal_h265_calc_repair_start_end_mb(repair_param, tile);

    return DEC_OK;
}

STATIC td_s32 hal_h265_get_mb_gp_of_no_tile_repair_param(dec_repair_param *repair_param,
    dec_report_info *dec_report, td_s32 total_mbn, td_s32 i)
{
    if (repair_param->valid_group_num >= MAX_UP_MSG_SLICE_NUM) {
        dprint(PRN_ERROR, "%s:0 valid_group_num(%d) out of range\n", __func__,
            repair_param->valid_group_num);
        return DEC_ERR;
    }

    repair_param->mb_group[repair_param->valid_group_num].start_mbn = dec_report->slice_mb_range[i][1];
    repair_param->mb_group[repair_param->valid_group_num].end_mbn = total_mbn - 1;
    repair_param->valid_group_num++;
    return DEC_OK;
}

STATIC td_s32 hal_h265_get_mb_gp_of_no_tile(dec_repair_param *repair_param, dec_report_info *dec_report)
{
    td_s32 i, ret, total_mbn, old_slice_start_mbn, old_slice_end_mbn, curr_slice_start_mbn, curr_slice_end_mbn;

    repair_param->valid_group_num = 0;
    repair_param->full_repair = 0;
    old_slice_start_mbn = -1;
    old_slice_end_mbn = -1;

    total_mbn = repair_param->image_width * repair_param->image_height;

    dec_report->slice_mb_range[dec_report->slice_num][0] =
        dec_report->slice_mb_range[dec_report->slice_num][1] = total_mbn;

    for (i = 0; i <= dec_report->slice_num; i++) {
        curr_slice_start_mbn = dec_report->slice_mb_range[i][0];
        curr_slice_end_mbn = dec_report->slice_mb_range[i][1];

        if ((curr_slice_start_mbn > total_mbn) || (curr_slice_end_mbn > total_mbn) ||
            (curr_slice_start_mbn > curr_slice_end_mbn)) {
            continue;
        }

        if (old_slice_end_mbn > curr_slice_start_mbn) {
            old_slice_end_mbn = old_slice_start_mbn;
        }

        if ((old_slice_end_mbn + 1) < curr_slice_start_mbn) {
            if (repair_param->valid_group_num > (MAX_UP_MSG_SLICE_NUM - 1)) {
                dprint(PRN_ERROR, "%s:0 valid_group_num(%d) out of range\n", __func__, repair_param->valid_group_num);
                return DEC_ERR;
            }

            repair_param->mb_group[repair_param->valid_group_num].start_mbn =
                old_slice_start_mbn + (old_slice_start_mbn < 0);
            repair_param->mb_group[repair_param->valid_group_num].end_mbn = curr_slice_start_mbn - 1;

            if (old_slice_end_mbn > 100 && old_slice_end_mbn - 100 > /* 100 total_mbn may less than 100 */
                (td_s32)repair_param->mb_group[repair_param->valid_group_num].start_mbn) {
                repair_param->mb_group[repair_param->valid_group_num].start_mbn =
                    (old_slice_end_mbn - 100); /* 100 total_mbn may less than 100 */
            }

            repair_param->valid_group_num++;
        }

        if (i == (dec_report->slice_num - 1)) {
            if (repair_param->valid_group_num > (MAX_UP_MSG_SLICE_NUM - 1)) {
                dprint(PRN_ERROR, "%d, valid_group_num(%d) out of range\n", __LINE__, repair_param->valid_group_num);
                return DEC_ERR;
            }

            if (dec_report->slice_mb_range[i][1] < (td_u32)total_mbn - 1) {
                ret = hal_h265_get_mb_gp_of_no_tile_repair_param(repair_param, dec_report, total_mbn, i);
                return ret;
            }
        }

        old_slice_start_mbn = curr_slice_start_mbn;
        old_slice_end_mbn = curr_slice_end_mbn;
    }

    return DEC_OK;
}

STATIC td_s32 hal_h265_set_param_value(dec_pic_param_h265 *pic_param, dec_repair_param *repair_param)
{
    td_s32 mb_size;

    vfmw_check_ret((pic_param != TD_NULL), DEC_ERR);

    repair_param->ref_image_addr = pic_param->apc_fsp_phy_addr[0];
    repair_param->ctb_size = pic_param->log2_ctb_size_y;
    repair_param->bit_depth = vfmw_max(pic_param->bit_depth_y, pic_param->bit_depth_c);
    mb_size = 1 << (repair_param->ctb_size);
    repair_param->image_width = (pic_param->pic_width_in_luma_samples + mb_size - 1) / mb_size;
    repair_param->image_height = (pic_param->pic_height_in_luma_samples + mb_size - 1) / mb_size;
    repair_param->tiles_enabled_flag = pic_param->tiles_enabled_flag;
    repair_param->image_csp = (0 == pic_param->chroma_format_idc);

    return DEC_OK;
}

td_s32 hal_h265_get_rep_param(td_void *repair, const td_void *chan_task, td_void *report)
{
    td_s32 i;
    dec_dev_info *dev = TD_NULL;
    vdh_msg_info *msg = TD_NULL;
    dec_pic_param_h265 *pic_param = TD_NULL;
    const dec_chan_task *task = TD_NULL;
    dec_report_info *dec_report = TD_NULL;
    dec_repair_param *repair_param = TD_NULL;

    task = (dec_chan_task *)chan_task;
    dec_report = (dec_report_info *)report;
    repair_param = (dec_repair_param *)repair;

    vfmw_check_ret((task != TD_NULL), DEC_ERR);
    vfmw_check_ret((dec_report != TD_NULL), DEC_ERR);
    vfmw_check_ret((repair_param != TD_NULL), DEC_ERR);

    if (dec_dev_get_dev(task->dev_id, &dev) != DEC_OK ||
        dev->ops->get_msg(dev, task->msg_id, &msg) != DEC_OK) {
        return DEC_ERR;
    }

    vfmw_check_ret((task->dps != TD_NULL), DEC_ERR);
    pic_param = (dec_pic_param_h265 *)task->dps->pic_param;
    vfmw_check_ret((pic_param != TD_NULL), DEC_ERR);

    repair_param->image_addr = task->mem_param.luma_phy_addr;
    repair_param->vdh_2d_en = task->mem_param.vdh_2d_en;
    repair_param->is_frame = 1;
    repair_param->pic_type = 0;
    if (hal_h265_set_param_value(pic_param, repair_param) != DEC_OK) {
       return DEC_ERR;
    }

    if (dec_report->is_read_from_reg == TD_TRUE) {
        return DEC_OK;
    }

    if ((dec_report->slice_num > 0) && (dec_report->slice_num <= MAX_UP_MSG_SLICE_NUM)) {
        if (repair_param->tiles_enabled_flag != 0) {
            hal_h265_get_mb_gp_of_tile(repair_param, dec_report, task, msg);
        } else {
            hal_h265_get_mb_gp_of_no_tile(repair_param, dec_report);
        }
    }

    if (repair_param->image_addr == repair_param->ref_image_addr) {
        for (i = 0; i < pic_param->apc_size && i < HEVC_APC_SIZE; i++) {
            if (repair_param->image_addr != pic_param->apc_fsp_phy_addr[i]) {
                repair_param->ref_image_addr = pic_param->apc_fsp_phy_addr[i];
                break;
            }
        }
    }

    return DEC_OK;
}
