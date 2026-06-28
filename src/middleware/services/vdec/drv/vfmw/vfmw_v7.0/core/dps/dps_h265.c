/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "dps_h265.h"
#include "vdh_h265.h"
#include "hevc.h"
#include "vfmw_define.h"

STATIC td_void dps_h265_write_qmatrix_4x4(td_u32 matrix_id, hevc_ctx *ctx, td_u32 *qmatrix)
{
    td_u32 d32, i, offset;
    td_u32 *pqm_mat_id_0 = &(ctx->qmatrix[0][matrix_id][0]);
    td_u32 qm_mat_id_0 = *pqm_mat_id_0;
    td_u32 *pqm_mat_id_1 = pqm_mat_id_0 + 1;
    td_u32 qm_mat_id_1 = *pqm_mat_id_1;
    td_u32 *pqm_mat_id_2 = pqm_mat_id_0 + 2; /* id_0  add 2 equal to id_2 */
    td_u32 qm_mat_id_2 = *pqm_mat_id_2;
    td_u32 *pqm_mat_id_3 = pqm_mat_id_0 + 3; /* id_0  add 3 equal to id_3 */
    td_u32 qm_mat_id_3 = *pqm_mat_id_3;

    for (i = 0; i <= 24; i += 8) { /* 24 is vfmw_max num, 8 is add num */
        offset = 0x000000FF << i;
        d32 = (((qm_mat_id_3 & offset) >> i) << 24) | /* 24 is 24bit */
              (((qm_mat_id_1 & offset) >> i) << 16) | /* 16 is 16bit */
              (((qm_mat_id_2 & offset) >> i) << 8) | /* 8 is 8bit */
              ((qm_mat_id_0 & offset) >> i);
        qmatrix[(i / 4) >> 1] = d32; /* 4 is num */
    }

    return;
}

STATIC td_void dps_h265_write_qmatrix_8x8_even(td_u32 *qmatrix, td_u32 **pqm_mat_id_0)
{
    td_u32 d32, i, offset;
    td_u32 *pqm_mat_id_0_tmp = (*pqm_mat_id_0);
    td_u32 qm_mat_id_0 = *pqm_mat_id_0_tmp;
    td_u32 *pqm_mat_id_1 = pqm_mat_id_0_tmp + 4; /* id_0  add 4 equal to id_1 */
    td_u32 qm_mat_id_1 = *pqm_mat_id_1;
    td_u32 *pqm_mat_id_2 = pqm_mat_id_0_tmp + 8; /* id_0  add 8 equal to id_2 */
    td_u32 qm_mat_id_2 = *pqm_mat_id_2;
    td_u32 *pqm_mat_id_3 = pqm_mat_id_0_tmp + 12; /* id_0  add 12 equal to id_3 */
    td_u32 qm_mat_id_3 = *pqm_mat_id_3;

    for (i = 0; i <= 24; i += 8) { /* 24 is vfmw_max num, 8 is add num */
        offset = 0x000000FF << i;
        d32 = (((qm_mat_id_3 & offset) >> i) << 24) | /* 24 is 24bit */
              (((qm_mat_id_2 & offset) >> i) << 16) | /* 16 is 16bit */
              (((qm_mat_id_1 & offset) >> i) << 8) | /* 8 is 8bit */
              ((qm_mat_id_0 & offset) >> i);
        qmatrix[(i / 4)] = d32; /* 4 is num */
    }

    pqm_mat_id_0_tmp = pqm_mat_id_0_tmp + 1;
    qm_mat_id_0 = *pqm_mat_id_0_tmp;
    pqm_mat_id_1 = pqm_mat_id_0_tmp + 4; /* id_0  add 4 equal to id_1 */
    qm_mat_id_1 = *pqm_mat_id_1;
    pqm_mat_id_2 = pqm_mat_id_0_tmp + 8; /* id_0  add 8 equal to id_2 */
    qm_mat_id_2 = *pqm_mat_id_2;
    pqm_mat_id_3 = pqm_mat_id_0_tmp + 12; /* id_0  add 12 equal to id_3 */
    qm_mat_id_3 = *pqm_mat_id_3;

    for (i = 0; i <= 24; i += 8) { /* 24 is vfmw_max num, 8 is add num */
        offset = 0x000000FF << i;
        d32 = (((qm_mat_id_3 & offset) >> i) << 24) | /* 24 is 24bit */
              (((qm_mat_id_2 & offset) >> i) << 16) | /* 16 is 16bit */
              (((qm_mat_id_1 & offset) >> i) << 8) | /* 8 is 8bit */
              ((qm_mat_id_0 & offset) >> i);
        qmatrix[(i / 4) + 8] = d32; /* 4 8 is num */
    }

    (*pqm_mat_id_0) = pqm_mat_id_0_tmp;
}

STATIC td_void dps_h265_write_qmatrix_8x8_odd(td_u32 *qmatrix, td_u32 **pqm_mat_id_0)
{
    td_u32 d32, i, offset;
    td_u32 *pqm_mat_id_0_tmp = (*pqm_mat_id_0) + 1;
    td_u32 qm_mat_id_0 = *pqm_mat_id_0_tmp;
    td_u32 *pqm_mat_id_1 = pqm_mat_id_0_tmp + 4; /* id_0  add 4 equal to id_1 */
    td_u32 qm_mat_id_1 = *pqm_mat_id_1;
    td_u32 *pqm_mat_id_2 = pqm_mat_id_0_tmp + 8; /* id_0  add 8 equal to id_2 */
    td_u32 qm_mat_id_2 = *pqm_mat_id_2;
    td_u32 *pqm_mat_id_3 = pqm_mat_id_0_tmp + 12; /* id_0  add 12 equal to id_3 */
    td_u32 qm_mat_id_3 = *pqm_mat_id_3;

    for (i = 0; i <= 24; i += 8) { /* 24 is vfmw_max num, 8 is add num */
        offset = 0x000000FF << i;
        d32 = (((qm_mat_id_3 & offset) >> i) << 24) | /* 24 is 24bit */
              (((qm_mat_id_2 & offset) >> i) << 16) | /* 16 is 16bit */
              (((qm_mat_id_1 & offset) >> i) << 8) |  /* 8 is 8bit */
              ((qm_mat_id_0 & offset) >> i);
        qmatrix[(i / 4) + 1] = d32; /* 4 is num */
    }

    pqm_mat_id_0_tmp = pqm_mat_id_0_tmp + 1;
    qm_mat_id_0 = *pqm_mat_id_0_tmp;
    pqm_mat_id_1 = pqm_mat_id_0_tmp + 4; /* id_0  add 4 equal to id_1 */
    qm_mat_id_1 = *pqm_mat_id_1;
    pqm_mat_id_2 = pqm_mat_id_0_tmp + 8; /* id_0  add 8 equal to id_2 */
    qm_mat_id_2 = *pqm_mat_id_2;
    pqm_mat_id_3 = pqm_mat_id_0_tmp + 12; /* id_0  add 12 equal to id_3 */
    qm_mat_id_3 = *pqm_mat_id_3;

    for (i = 0; i <= 24; i += 8) { /* 24 is vfmw_max num, 8 is add num */
        offset = 0x000000FF << i;
        d32 = (((qm_mat_id_3 & offset) >> i) << 24) | /* 24 is 24bit */
              (((qm_mat_id_2 & offset) >> i) << 16) | /* 16 is 16bit */
              (((qm_mat_id_1 & offset) >> i) << 8) | /* 8 is 8bit */
              ((qm_mat_id_0 & offset) >> i);
        qmatrix[(i / 4) + 9] = d32; /* 4 9 is num */
    }

    (*pqm_mat_id_0) = pqm_mat_id_0_tmp;
}

STATIC td_void dps_h265_write_qmatrix_8x8(td_u32 matrix_id, hevc_ctx *ctx, td_u32 *qmatrix)
{
    td_u32 *pqm_mat_id_0 = &(ctx->qmatrix[1][matrix_id][0]);

    dps_h265_write_qmatrix_8x8_even(qmatrix, &pqm_mat_id_0);
    dps_h265_write_qmatrix_8x8_odd(qmatrix, &pqm_mat_id_0);

    return;
}

STATIC td_void dps_h265_write_qmatrix_16x16_even(td_u32 *qmatrix, td_u32 **pqm_mat_id_0)
{
    td_u32 d32, i, offset;
    td_u32 *pqm_mat_id_0_tmp = (*pqm_mat_id_0);  /* 2 is array num */
    td_u32 qm_mat_id_0 = *pqm_mat_id_0_tmp;
    td_u32 *pqm_mat_id_1 = pqm_mat_id_0_tmp + 2; /* id_0  add 2 equal to id_1 */
    td_u32 qm_mat_id_1 = *pqm_mat_id_1;
    td_u32 *pqm_mat_id_2 = pqm_mat_id_0_tmp + 4; /* id_0  add 4 equal to id_2 */
    td_u32 qm_mat_id_2 = *pqm_mat_id_2;
    td_u32 *pqm_mat_id_3 = pqm_mat_id_0_tmp + 6; /* id_0  add 6 equal to id_3 */
    td_u32 qm_mat_id_3 = *pqm_mat_id_3;

    for (i = 0; i <= 24; i += 8) { /* 24 is vfmw_max num, 8 is add num */
        offset = 0x000000FF << i;
        d32 = (((qm_mat_id_3 & offset) >> i) << 24) | /* 24 is 24bit */
              (((qm_mat_id_2 & offset) >> i) << 16) | /* 16 is 16bit */
              (((qm_mat_id_1 & offset) >> i) << 8) | /* 8 is 8bit */
              ((qm_mat_id_0 & offset) >> i);
        qmatrix[(i / 4)] = d32; /* 0,2,4,6 */ /* 4 is num */
    }

    pqm_mat_id_0_tmp = pqm_mat_id_0_tmp + 1; /* 1 */
    qm_mat_id_0 = *pqm_mat_id_0_tmp;
    pqm_mat_id_1 = pqm_mat_id_0_tmp + 2; /* id_0  add 2 equal to id_1 */
    qm_mat_id_1 = *pqm_mat_id_1;
    pqm_mat_id_2 = pqm_mat_id_0_tmp + 4; /* id_0  add 4 equal to id_2 */
    qm_mat_id_2 = *pqm_mat_id_2;
    pqm_mat_id_3 = pqm_mat_id_0_tmp + 6; /* id_0  add 6 equal to id_3 */
    qm_mat_id_3 = *pqm_mat_id_3;

    for (i = 0; i <= 24; i += 8) { /* 24 is vfmw_max num, 8 is add num */
        offset = 0x000000FF << i;
        d32 = (((qm_mat_id_3 & offset) >> i) << 24) | /* 24 is 24bit */
              (((qm_mat_id_2 & offset) >> i) << 16) | /* 16 is 16bit */
              (((qm_mat_id_1 & offset) >> i) << 8) | /* 8 is 8bit */
              ((qm_mat_id_0 & offset) >> i);
        qmatrix[(i / 4) + 8] = d32; /* 8,10,12,14 */ /* 4 8 is num */
    }

    (*pqm_mat_id_0) = pqm_mat_id_0_tmp;
}

STATIC td_void dps_h265_write_qmatrix_16x16_odd(td_u32 *qmatrix, td_u32 **pqm_mat_id_0)
{
    td_u32 d32, i, offset;
    td_u32 *pqm_mat_id_0_tmp = (*pqm_mat_id_0) + 7; /* 8 */ /* id_0  add 7 equal to id_0 */
    td_u32 qm_mat_id_0 = *pqm_mat_id_0_tmp;
    td_u32 *pqm_mat_id_1 = pqm_mat_id_0_tmp + 2; /* id_0  add 2 equal to id_1 */
    td_u32 qm_mat_id_1 = *pqm_mat_id_1;
    td_u32 *pqm_mat_id_2 = pqm_mat_id_0_tmp + 4; /* id_0  add 4 equal to id_2 */
    td_u32 qm_mat_id_2 = *pqm_mat_id_2;
    td_u32 *pqm_mat_id_3 = pqm_mat_id_0_tmp + 6; /* id_0  add 6 equal to id_3 */
    td_u32 qm_mat_id_3 = *pqm_mat_id_3;

    for (i = 0; i <= 24; i += 8) { /* 24 is vfmw_max num, 8 is add num */
        offset = 0x000000FF << i;
        d32 = (((qm_mat_id_3 & offset) >> i) << 24) | /* 24 is 24bit */
              (((qm_mat_id_2 & offset) >> i) << 16) | /* 16 is 16bit */
              (((qm_mat_id_1 & offset) >> i) << 8) |  /* 8 is 8bit */
              ((qm_mat_id_0 & offset) >> i);
        qmatrix[(i / 4) + 1] = d32; /* 1, 3, 5, 7 */ /* 4 is num */
    }

    pqm_mat_id_0_tmp = pqm_mat_id_0_tmp + 1; /* 9 */
    qm_mat_id_0 = *pqm_mat_id_0_tmp;
    pqm_mat_id_1 = pqm_mat_id_0_tmp + 2; /* id_0  add 2 equal to id_1 */
    qm_mat_id_1 = *pqm_mat_id_1;
    pqm_mat_id_2 = pqm_mat_id_0_tmp + 4; /* id_0  add 4 equal to id_2 */
    qm_mat_id_2 = *pqm_mat_id_2;
    pqm_mat_id_3 = pqm_mat_id_0_tmp + 6; /* id_0  add 6 equal to id_3 */
    qm_mat_id_3 = *pqm_mat_id_3;

    for (i = 0; i <= 24; i += 8) { /* 24 is vfmw_max num, 8 is add num */
        offset = 0x000000FF << i;
        d32 = (((qm_mat_id_3 & offset) >> i) << 24) | /* 24 is 24bit */
              (((qm_mat_id_2 & offset) >> i) << 16) | /* 16 is 16bit */
              (((qm_mat_id_1 & offset) >> i) << 8) | /* 8 is 8bit */
              ((qm_mat_id_0 & offset) >> i);
        qmatrix[(i / 4) + 9] = d32; /* 9, 11, 13, 15 */ /* 4 9 is num */
    }

    (*pqm_mat_id_0) = pqm_mat_id_0_tmp;
}

STATIC td_void dps_h265_write_qmatrix_16x16(td_u32 matrix_id, hevc_ctx *ctx, td_u32 *qmatrix)
{
    td_u32 *pqm_mat_id_0 = &(ctx->qmatrix[2][matrix_id][0]);  /* 2 is array num */

    dps_h265_write_qmatrix_16x16_even(qmatrix, &pqm_mat_id_0);

    dps_h265_write_qmatrix_16x16_odd(qmatrix, &pqm_mat_id_0);

    return;
}

STATIC td_void dps_h265_write_qmatrix_32x32_even(td_u32 *qmatrix, td_u32 **pqm_mat_id_0)
{
    td_u32 d32, i, offset;
    td_u32 *pqm_mat_id_0_tmp = (*pqm_mat_id_0);
    td_u32 qm_mat_id_0 = *pqm_mat_id_0_tmp;
    td_u32 *pqm_mat_id_1 = pqm_mat_id_0_tmp + 2; /* id_0  add 2 equal to id_1 */
    td_u32 qm_mat_id_1 = *pqm_mat_id_1;
    td_u32 *pqm_mat_id_2 = pqm_mat_id_0_tmp + 4; /* id_0  add 4 equal to id_2 */
    td_u32 qm_mat_id_2 = *pqm_mat_id_2;
    td_u32 *pqm_mat_id_3 = pqm_mat_id_0_tmp + 6; /* id_0  add 6 equal to id_3 */
    td_u32 qm_mat_id_3 = *pqm_mat_id_3;

    for (i = 0; i <= 24; i += 8) { /* 24 is vfmw_max num, 8 is add num */
        offset = 0x000000FF << i;
        d32 = (((qm_mat_id_3 & offset) >> i) << 24) | /* 24 is 24bit */
              (((qm_mat_id_2 & offset) >> i) << 16) | /* 16 is 16bit */
              (((qm_mat_id_1 & offset) >> i) << 8) | /* 8 is 8bit */
              ((qm_mat_id_0 & offset) >> i);
        qmatrix[(i / 4)] = d32; /* 0, 2, 4, 6 */ /* 4 is num */
    }

    pqm_mat_id_0_tmp = pqm_mat_id_0_tmp + 1; /* 1 */
    qm_mat_id_0 = *pqm_mat_id_0_tmp;
    pqm_mat_id_1 = pqm_mat_id_0_tmp + 2; /* id_0  add 2 equal to id_1 */
    qm_mat_id_1 = *pqm_mat_id_1;
    pqm_mat_id_2 = pqm_mat_id_0_tmp + 4; /* id_0  add 4 equal to id_2 */
    qm_mat_id_2 = *pqm_mat_id_2;
    pqm_mat_id_3 = pqm_mat_id_0_tmp + 6; /* id_0  add 6 equal to id_3 */
    qm_mat_id_3 = *pqm_mat_id_3;

    for (i = 0; i <= 24; i += 8) { /* 24 is vfmw_max num, 8 is add num */
        offset = 0x000000FF << i;
        d32 = (((qm_mat_id_3 & offset) >> i) << 24) | /* 24 is 24bit */
              (((qm_mat_id_2 & offset) >> i) << 16) | /* 16 is 16bit */
              (((qm_mat_id_1 & offset) >> i) << 8) | /* 8 is 8bit */
              ((qm_mat_id_0 & offset) >> i);
        qmatrix[(i / 4) + 8] = d32; /* 8, 10, 12, 14 */ /* 4 8 is num */
    }

    (*pqm_mat_id_0) = pqm_mat_id_0_tmp;
}

STATIC td_void dps_h265_write_qmatrix_32x32_odd(td_u32 *qmatrix, td_u32 **pqm_mat_id_0)
{
    td_u32 d32, i, offset;
    td_u32 *pqm_mat_id_0_tmp = (*pqm_mat_id_0) + 7; /* 7 is num */
    td_u32 qm_mat_id_0 = *pqm_mat_id_0_tmp;
    td_u32 *pqm_mat_id_1 = pqm_mat_id_0_tmp + 2; /* id_0  add 2 equal to id_1 */
    td_u32 qm_mat_id_1 = *pqm_mat_id_1;
    td_u32 *pqm_mat_id_2 = pqm_mat_id_0_tmp + 4; /* id_0  add 4 equal to id_2 */
    td_u32 qm_mat_id_2 = *pqm_mat_id_2;
    td_u32 *pqm_mat_id_3 = pqm_mat_id_0_tmp + 6; /* id_0  add 6 equal to id_3 */
    td_u32 qm_mat_id_3 = *pqm_mat_id_3;

    for (i = 0; i <= 24; i += 8) { /* 24 is vfmw_max num, 8 is add num */
        offset = 0x000000FF << i;
        d32 = (((qm_mat_id_3 & offset) >> i) << 24) | /* 24 is 24bit */
              (((qm_mat_id_2 & offset) >> i) << 16) | /* 16 is 16bit */
              (((qm_mat_id_1 & offset) >> i) << 8) | /* 8 is 8bit */
              ((qm_mat_id_0 & offset) >> i);
        qmatrix[(i / 4) + 1] = d32; /* 1, 3, 5, 7 */ /* 4 is num */
    }

    pqm_mat_id_0_tmp = pqm_mat_id_0_tmp + 1; /* 9 */
    qm_mat_id_0 = *pqm_mat_id_0_tmp;
    pqm_mat_id_1 = pqm_mat_id_0_tmp + 2; /* id_0  add 2 equal to id_1 */
    qm_mat_id_1 = *pqm_mat_id_1;
    pqm_mat_id_2 = pqm_mat_id_0_tmp + 4; /* id_0  add 4 equal to id_2 */
    qm_mat_id_2 = *pqm_mat_id_2;
    pqm_mat_id_3 = pqm_mat_id_0_tmp + 6; /* id_0  add 6 equal to id_3 */
    qm_mat_id_3 = *pqm_mat_id_3;

    for (i = 0; i <= 24; i += 8) { /* 24 is vfmw_max num, 8 is add num */
        offset = 0x000000FF << i;
        d32 = (((qm_mat_id_3 & offset) >> i) << 24) | /* 24 is 24bit */
              (((qm_mat_id_2 & offset) >> i) << 16) | /* 16 is 16bit */
              (((qm_mat_id_1 & offset) >> i) << 8) | /* 8 is 8bit */
              ((qm_mat_id_0 & offset) >> i);
        qmatrix[(i / 4) + 9] = d32; /* 9, 11, 13, 15 */ /* 4 9 is num */
    }

    (*pqm_mat_id_0) = pqm_mat_id_0_tmp;
}


STATIC td_void dps_h265_write_qmatrix_32x32(td_u32 matrix_id, hevc_ctx *ctx, td_u32 *qmatrix)
{
    td_u32 *pqm_mat_id_0 = &(ctx->qmatrix[3][matrix_id][0]); /* 3 is array num */

    dps_h265_write_qmatrix_32x32_even(qmatrix, &pqm_mat_id_0);

    dps_h265_write_qmatrix_32x32_odd(qmatrix, &pqm_mat_id_0);

    return;
}

VFMW_ACC_SLICE_H265 STATIC td_s32 dps_h265_ctb_rsaddr_to_tsaddr(const hevc_ctx *ctx, td_s32 ctb_addr_rs)
{
    hevc_pic_param_set *pps = ctx->curr_pps;
    td_s32 pic_width_in_ctbs_y = (td_s32)ctx->curr_sps->ctb_num_width;
    td_s32 ctb_raster_addr_x;
    td_s32 ctb_raster_addr_y;
    td_s32 tile_width;
    td_s32 tile_height;
    td_s32 tile_addr_x;
    td_s32 tile_addr_y = 0;
    td_s32 ctb_addr_ts;
    td_s32 next_ctb_addr_ts = 0;
    td_s32 tile_x;
    td_s32 tile_y;

    ctb_addr_ts = 0;
    tile_addr_x = 0;
    tile_width = 0;
    tile_height = 0;

    if (pps->tiles_enabled_flag == 0) {
        return ctb_addr_rs;
    }

    ctb_raster_addr_x = ctb_addr_rs % pic_width_in_ctbs_y;
    ctb_raster_addr_y = ctb_addr_rs / pic_width_in_ctbs_y;

    for (tile_y = 0; tile_y < pps->num_tile_rows; tile_y++) {
        tile_addr_x = 0;

        for (tile_x = 0; tile_x < pps->num_tile_columns; tile_x++) {
            tile_width = pps->column_bd[tile_x];
            tile_height = pps->row_bd[tile_y];
            ctb_addr_ts = next_ctb_addr_ts;
            next_ctb_addr_ts += tile_width * tile_height;

            if ((tile_addr_x + tile_width) > ctb_raster_addr_x && (tile_addr_y + tile_height) > ctb_raster_addr_y) {
                goto jump;
            }

            tile_addr_x += tile_width;
        }

        tile_addr_y += tile_height;
    }

jump:
    vfmw_check_ret((ctb_raster_addr_y - tile_addr_y) * tile_width < VFMW_MAX_SINT32, 0);
    ctb_addr_ts += (ctb_raster_addr_y - tile_addr_y) * tile_width + (ctb_raster_addr_x - tile_addr_x);

    return ctb_addr_ts;
}

STATIC td_s32 dps_h264_check_tile(td_s32 num_tile_columns, td_s32 num_tile_rows)
{
    if (num_tile_columns <= 0 || num_tile_columns > HEVC_LOGIC_TILE_COLUMNS_LIMIT) {
        dprint(PRN_ERROR, "%s num_tile_columns(%d) out of range(0, %d].\n",
               __func__, num_tile_columns, HEVC_LOGIC_TILE_COLUMNS_LIMIT);
        return DPS_ERR;
    }

    if (num_tile_rows <= 0 || num_tile_rows > HEVC_LOGIC_TILE_ROWS_LIMIT) {
        dprint(PRN_ERROR, "%s num_tile_rows(%d) out of range(0, %d].\n",
               __func__, num_tile_rows, HEVC_LOGIC_TILE_ROWS_LIMIT);
        return DPS_ERR;
    }

    return DPS_OK;
}

STATIC td_s32 dps_h265_calc_tile_x(const hevc_ctx *ctx, td_s32 num_tile_columns, td_u32 tile_idx_shift,
    td_u16 tile_pos_x[HEVC_LOGIC_TILE_COLUMNS_LIMIT][2], /* 2 is array num */
    td_u8 tile_id[2][HEVC_MAX_CTB_NUM_PER_LINE]) /* 2 is array num */
{
    td_s32 tile_x;
    td_u32 start, end;
    td_s32 idx, start_idx, end_idx;

    /* x */
    start = 0;
    end = 0;

    for (tile_x = 0; tile_x < num_tile_columns; tile_x++) {
        end += (td_u32)ctx->curr_pps->column_bd[tile_x];
        tile_pos_x[tile_x][0] = start;
        tile_pos_x[tile_x][1] = end - 1;

        start_idx = start << tile_idx_shift;
        end_idx = end << tile_idx_shift;

        if (start_idx < 0 || end_idx > HEVC_MAX_CTB_NUM_PER_LINE) {
            dprint(PRN_ERROR, "%s idx(%d %d) out of (0 %d).\n",
                   __func__, start_idx, end_idx, HEVC_MAX_CTB_NUM_PER_LINE);
            return DPS_ERR;
        }
        for (idx = start_idx; idx < end_idx; idx++) {
            tile_id[0][idx] = (td_u8)tile_x;
        }

        start = end;
    }

    return DPS_OK;
}

STATIC td_s32 dps_h265_calc_tile_y(const hevc_ctx *ctx, td_s32 num_tile_rows, td_u32 tile_idx_shift,
    td_u16 tile_pos_y[HEVC_LOGIC_TILE_COLUMNS_LIMIT][2], /* 2 is array num */
    td_u8 tile_id[2][HEVC_MAX_CTB_NUM_PER_LINE]) /* 2 is array num */
{
    td_s32 tile_y;
    td_u32 start, end;
    td_s32 idx, start_idx, end_idx;

    /* y */
    start = 0;
    end = 0;

    for (tile_y = 0; tile_y < num_tile_rows; tile_y++) {
        end += (td_u32)ctx->curr_pps->row_bd[tile_y];
        tile_pos_y[tile_y][0] = start;
        tile_pos_y[tile_y][1] = end - 1;

        start_idx = start << tile_idx_shift;
        end_idx = end << tile_idx_shift;

        if (start_idx < 0 || end_idx > HEVC_MAX_CTB_NUM_PER_LINE) {
            dprint(PRN_ERROR, "%s idx(%d %d) out of (0 %d).\n",
                   __func__, start_idx, end_idx, HEVC_MAX_CTB_NUM_PER_LINE);
            return DPS_ERR;
        }
        for (idx = start_idx; idx < end_idx; idx++) {
            tile_id[1][idx] = (td_u8)tile_y;
        }

        start = end;
    }

    return DPS_OK;
}

STATIC td_s32 dps_h265_calc_tile_info(const hevc_ctx *ctx,
                                      td_u8 tile_id[2][HEVC_MAX_CTB_NUM_PER_LINE],  /* 2 is array num */
                                      td_u16 tile_pos_x[HEVC_LOGIC_TILE_COLUMNS_LIMIT][2], /* 2 is array num */
                                      td_u16 tile_pos_y[HEVC_LOGIC_TILE_ROWS_LIMIT][2]) /* 2 is array num */
{
    td_s32 num_tile_columns;
    td_s32 num_tile_rows;
    td_u32 tile_idx_shift;

    num_tile_rows = ctx->curr_pps->num_tile_rows;
    num_tile_columns = ctx->curr_pps->num_tile_columns;

#ifdef HEVC_TILE_IDX_SHIFT
    tile_idx_shift = HEVC_TILE_IDX_SHIFT;
#else
    tile_idx_shift = ctx->curr_sps->log2_ctb_sizey - HEVC_MIN_LOG2_CTB_SIZE;
#endif

    if (dps_h264_check_tile(num_tile_columns, num_tile_rows) != DPS_OK) {
        return DPS_ERR;
    }

    vfmw_check_sec_func(memset_s(tile_id, 2 * HEVC_MAX_CTB_NUM_PER_LINE, 0, /* 2 :a number */
                                 2 * HEVC_MAX_CTB_NUM_PER_LINE)); /* 2 :a number */
    vfmw_check_sec_func(memset_s(tile_pos_x, sizeof(td_u16) * HEVC_LOGIC_TILE_COLUMNS_LIMIT * 2, /* 2 :a number */
                                 0, sizeof(td_u16) * HEVC_LOGIC_TILE_COLUMNS_LIMIT * 2)); /* 2 :a number */
    vfmw_check_sec_func(memset_s(tile_pos_y, sizeof(td_u16) * HEVC_LOGIC_TILE_ROWS_LIMIT * 2, /* 2 :a number */
                                 0, sizeof(td_u16) * HEVC_LOGIC_TILE_ROWS_LIMIT * 2)); /* 2 :a number */

    if (dps_h265_calc_tile_x(ctx, num_tile_columns, tile_idx_shift, tile_pos_x, tile_id) != DPS_OK) {
        return DPS_ERR;
    }

    if (dps_h265_calc_tile_y(ctx, num_tile_rows, tile_idx_shift, tile_pos_y, tile_id) != DPS_OK) {
        return DPS_ERR;
    }

    return DPS_OK;
}

STATIC td_void dps_h265_get_pmv_fsp_id(const hevc_ctx *ctx, dec_slice_param_h265 *slice_param,
    const hevc_slice_segment_header *slice)
{
    if (slice->collocated_ref_idx >= 0 &&
        slice->collocated_ref_idx < HEVC_MAX_LIST_SIZE &&
        ctx->list_x[REF_PIC_LIST_1][slice->collocated_ref_idx] != TD_NULL &&
        ctx->list_x[REF_PIC_LIST_1][slice->collocated_ref_idx]->frame_store != TD_NULL) {
        slice_param->col_pmv_fsp_id =
            ctx->list_x[REF_PIC_LIST_1][slice->collocated_ref_idx]->frame_store->logic_fs_id;
    }
}

STATIC td_void dps_h265_dps_slice_get_pmv_fsp_id(const hevc_ctx *ctx, dec_slice_param_h265 *slice_param)
{
    const hevc_slice_segment_header *slice =
        (ctx->curr_slice.dependent_slice_segment_flag == 0) ? (&ctx->curr_slice) : (&ctx->independent_slice);

    if (slice->slice_temporal_mvp_enable_flag && slice->slice_type != HEVC_I_SLICE) {
        if (slice->slice_type == HEVC_B_SLICE && !slice->collocated_from_l0_flag) {
            dps_h265_get_pmv_fsp_id(ctx, slice_param, slice);
        } else {
            if (slice->collocated_ref_idx >= 0 &&
                slice->collocated_ref_idx < HEVC_MAX_LIST_SIZE &&
                ctx->list_x[REF_PIC_LIST_0][slice->collocated_ref_idx] != TD_NULL &&
                ctx->list_x[REF_PIC_LIST_0][slice->collocated_ref_idx]->frame_store != TD_NULL) {
                slice_param->col_pmv_fsp_id =
                    ctx->list_x[REF_PIC_LIST_0][slice->collocated_ref_idx]->frame_store->logic_fs_id;
            }
        }
    } else {
        slice_param->col_pmv_fsp_id = INVALID_IDX;
    }
}

VFMW_ACC_SLICE_H265 STATIC td_void dps_h265_make_dps_slice_ref_info(const hevc_ctx *ctx,
                                                                    dec_slice_param_h265 *slice_param)
{
    td_u32 i, j;
    td_u8 is_lowdelay = 1;
    const hevc_slice_segment_header *slice =
        (ctx->curr_slice.dependent_slice_segment_flag == 0) ? (&ctx->curr_slice) : (&ctx->independent_slice);

    for (i = 0; i < 2; i++) { /* 2 is list num */
        for (j = 0; j < slice->list_x_size[i]; j++) {
            slice_param->is_long_term[i][j] = ctx->list_x[i][j]->is_long_term;
#ifdef VFMW_MV_HEVC_SUPPORT
            if (ctx->is_multi_layers == 1 && ctx->list_x[i][j]->is_inter_layer == 1 &&
                ctx->curr_slice.view_id != 0) {
                slice_param->idx_2_fsp_id[i][j] = (td_u32)ctx->list_x[i][j]->frame_store->logic_fs_id;

                slice_param->is_long_term[i][j] = (ctx->is_shvc_type == 1) ? 1 : 0;
            } else {
#endif
                slice_param->idx_2_fsp_id[i][j] = (td_u32)ctx->list_x[i][j]->frame_store->logic_fs_id;
#ifdef VFMW_MV_HEVC_SUPPORT
            }
#endif
        }
    }
    dps_h265_dps_slice_get_pmv_fsp_id(ctx, slice_param);

    for (i = 0; i < slice->list_x_size[0]; i++) {
        if (ctx->curr_pic.thispoc < ctx->list_x[0][i]->poc) {
            is_lowdelay = 0;
            break;
        }
    }

    if (slice->slice_type == HEVC_B_SLICE) {
        for (i = 0; i < slice->list_x_size[1] && is_lowdelay; i++) {
            if (ctx->curr_pic.thispoc < ctx->list_x[1][i]->poc) {
                is_lowdelay = 0;
                break;
            }
        }
    }

    slice_param->is_low_delay = is_lowdelay;
}

VFMW_ACC_SLICE_H265 STATIC td_void dps_h265_make_dps_slice_wp_info(const hevc_ctx *ctx,
                                                                   dec_slice_param_h265 *slice_param)
{
    td_s32 i;
    hevc_pic_param_set *curr_pps = TD_NULL;
    const hevc_slice_segment_header *slice = TD_NULL;

    curr_pps = ctx->curr_pps;

    if (ctx->curr_slice.dependent_slice_segment_flag == 0) {
        slice = &ctx->curr_slice;
    } else {
        slice = &ctx->independent_slice;
    }

    if ((curr_pps->weighted_pred_flag && slice->slice_type == HEVC_P_SLICE) ||
        (curr_pps->weighted_bipred_flag && slice->slice_type == HEVC_B_SLICE)) {
        slice_param->luma_log2_weight_denom = slice->pred_weight_table.luma_log2_weight_denom;
        slice_param->chroma_log2_weight_denom = slice->pred_weight_table.chroma_log2_weight_denom;

        /* list0 */
        for (i = 0; i < HEVC_MAX_NUM_REF_PICS; i++) {
            slice_param->luma_weight_l0[i] = slice->pred_weight_table.luma_weight_l0[i];
            slice_param->luma_offset_l0[i] = slice->pred_weight_table.luma_offset_l0[i];
            slice_param->chroma_weight_l0[i][0] = slice->pred_weight_table.chroma_weight_l0[i][0];
            slice_param->chroma_weight_l0[i][1] = slice->pred_weight_table.chroma_weight_l0[i][1];
            slice_param->chroma_offset_l0[i][0] = slice->pred_weight_table.chroma_offset_l0[i][0];
            slice_param->chroma_offset_l0[i][1] = slice->pred_weight_table.chroma_offset_l0[i][1];
        }

        /* list1 */
        if (slice->slice_type == HEVC_B_SLICE) {
            for (i = 0; i < HEVC_MAX_NUM_REF_PICS; i++) {
                slice_param->luma_weight_l1[i] = slice->pred_weight_table.luma_weight_l1[i];
                slice_param->luma_offset_l1[i] = slice->pred_weight_table.luma_offset_l1[i];
                slice_param->chroma_weight_l1[i][0] = slice->pred_weight_table.chroma_weight_l1[i][0];
                slice_param->chroma_weight_l1[i][1] = slice->pred_weight_table.chroma_weight_l1[i][1];
                slice_param->chroma_offset_l1[i][0] = slice->pred_weight_table.chroma_offset_l1[i][0];
                slice_param->chroma_offset_l1[i][1] = slice->pred_weight_table.chroma_offset_l1[i][1];
            }
        }
    }
}

/* interface */
td_s32 dps_h265_get_max_slice_num(td_void)
{
    return HEVC_MAX_SLICE_NUM;
}

td_s32 dps_h265_get_pic_param_size(td_void)
{
    return sizeof(dec_pic_param_h265);
}

td_s32 dps_h265_get_slice_param_size(td_void)
{
    return sizeof(dec_slice_param_h265);
}

STATIC td_s32 dps_h265_ctb_tsaddr_to_rsaddr(const dec_pic_param_h265 *pic_param, td_s32 ctb_addr_ts)
{
    td_s32 pic_width_in_ctb_y = pic_param->pic_width_in_ctbs_y_minus1 + 1;
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

    if (pic_param->tiles_enabled_flag == 0) {
        return ctb_addr_ts;
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
            vfmw_check_ret(tile_width_in_ctb * tile_height_in_ctb < VFMW_MAX_SINT32, 0);
            next_tile_ctb_addr_in_ts += tile_width_in_ctb * tile_height_in_ctb;

            if (next_tile_ctb_addr_in_ts > ctb_addr_ts) {
                goto jump;
            }

            abs_width_in_ctb += tile_width_in_ctb;
        }

        abs_height_in_ctb += tile_height_in_ctb;
    }
jump:
    abs_width_in_ctb += (ctb_addr_ts - tile_ctb_addr_in_ts) % tile_width_in_ctb;
    abs_height_in_ctb += (ctb_addr_ts - tile_ctb_addr_in_ts) / tile_width_in_ctb;
    vfmw_check_ret(pic_width_in_ctb_y * abs_height_in_ctb < VFMW_MAX_SINT32, 0);
    ctb_addr_rs = abs_width_in_ctb + pic_width_in_ctb_y * abs_height_in_ctb;

    return ctb_addr_rs;
}

STATIC td_s32 dps_h265_calc_slice_end_addr_in_tile_mulslcldy(
    hevc_ctx *ctx, const dec_pic_param_h265 *pic_param, dec_slice_param_h265 *slice_head, td_u16 slice_num)
{
    dec_slice_param_h265 *p = slice_head;
    td_u32 i, frm_last_ctb_num;
    td_s32 pre_end_ctb_in_slice_tile = 0;

    vfmw_check_ret((pic_param->pic_width_in_ctbs_y_minus1 + 1) < VFMW_MAX_UINT32, DEC_ERR);
    vfmw_check_ret((pic_param->pic_hight_in_ctbs_y_minus1 + 1) < VFMW_MAX_UINT32, DEC_ERR);

    frm_last_ctb_num = (pic_param->pic_width_in_ctbs_y_minus1 + 1) * (pic_param->pic_hight_in_ctbs_y_minus1 + 1) - 1;

    for (i = 0; i < slice_num; i++, p = p->next_slice) {
        if (p == TD_NULL) {
            dprint(PRN_ERROR, "DPS slicelist  member num(%d) != SliceNum(%d)\n", i + 1, slice_num);
            return DPS_ERR;
        }

        if (p->next_slice == TD_NULL) {
            p->end_ctb_in_slice_tile = ctx->curr_slice.slice_segment_address;
            break;
        }

        p->end_ctb_in_slice_tile = p->next_slice->slice_segment_tile_address - 1;

        /* check slice address valid */
        if ((p->slice_segment_tile_address < (td_u32)pre_end_ctb_in_slice_tile) && (i > 0)) {
            dprint(PRN_ERROR, "Slice%d  slice_segment_tile_address(%d) < pre_end_ctb_in_slice_tile(%d)\n",
                   i, p->slice_segment_tile_address, pre_end_ctb_in_slice_tile);
            return DPS_ERR;
        }
        pre_end_ctb_in_slice_tile = (td_s32)p->end_ctb_in_slice_tile;

        if (p->slice_segment_tile_address > p->end_ctb_in_slice_tile) {
            dprint(PRN_ERROR, "Slice%d  slice_segment_tile_address %d > end_ctb_in_slice_tile %d\n",
                   i, p->slice_segment_tile_address, p->end_ctb_in_slice_tile);
            return DPS_ERR;
        }

        if (p->end_ctb_in_slice_tile > frm_last_ctb_num) {
            dprint(PRN_ERROR, "Slice%d  end_ctb_in_slice_tile %d > FrmLastCtbNum %d\n",
                   i, p->end_ctb_in_slice_tile, frm_last_ctb_num);
            return DPS_ERR;
        }

        if (p->valid_bitlen[0] == 0) {
            dprint(PRN_ERROR, "error,slice%d valid_bitlen[0] is 0\n", i);
            return DPS_ERR;
        }
    }

    if ((ctx->curr_dps->is_rcv_lst_slc == TD_TRUE && p == ctx->curr_dps->last_slice) ||
        (p->end_ctb_in_slice_tile == 0)) {
        p->end_ctb_in_slice_tile = frm_last_ctb_num;

        if (p->valid_bitlen[0] == 0) {
            dprint(PRN_ERROR, "error,last slice valid_bitlen[0] is 0\n");
            return DPS_ERR;
        }
    } else {
        p->end_ctb_in_slice_tile = dps_h265_ctb_rsaddr_to_tsaddr(ctx, p->end_ctb_in_slice_tile) - 1;
    }

    return DPS_OK;
}

STATIC td_s32 dps_h265_calc_slice_end_addr_in_tile_normal(
    const dec_pic_param_h265 *pic_param, dec_slice_param_h265 *slice_head, td_u16 slice_num)
{
    dec_slice_param_h265 *p = slice_head;
    td_u32 i;
    td_u32 frm_last_ctb_num;
    td_s32 pre_end_ctb_in_slice_tile = 0;

    vfmw_check_ret((pic_param->pic_width_in_ctbs_y_minus1 + 1) < VFMW_MAX_UINT32, DEC_ERR);
    vfmw_check_ret((pic_param->pic_hight_in_ctbs_y_minus1 + 1) < VFMW_MAX_UINT32, DEC_ERR);
    vfmw_check_ret(slice_num >= 1, DEC_ERR);

    frm_last_ctb_num = (pic_param->pic_width_in_ctbs_y_minus1 + 1) * (pic_param->pic_hight_in_ctbs_y_minus1 + 1) - 1;

    for (i = 0; i < slice_num - 1; i++, p = p->next_slice) {
        if (p == TD_NULL || p->next_slice == TD_NULL) {
            dprint(PRN_ERROR, "DPS slicelist  member num(%d) != SliceNum(%d)\n", i + 1, slice_num);
            return DPS_ERR;
        }

        p->end_ctb_in_slice_tile = p->next_slice->slice_segment_tile_address - 1;

        /* check slice address valid */
        if ((p->slice_segment_tile_address < (td_u32)pre_end_ctb_in_slice_tile) && (i > 0)) {
            dprint(PRN_ERROR, "Slice%d  slice_segment_tile_address(%d) < pre_end_ctb_in_slice_tile(%d)\n",
                   i, p->slice_segment_tile_address, pre_end_ctb_in_slice_tile);
            return DPS_ERR;
        }
        pre_end_ctb_in_slice_tile = (td_s32)p->end_ctb_in_slice_tile;

        if (p->slice_segment_tile_address > p->end_ctb_in_slice_tile) {
            dprint(PRN_ERROR, "Slice%d  slice_segment_tile_address %d > end_ctb_in_slice_tile %d\n",
                   i, p->slice_segment_tile_address, p->end_ctb_in_slice_tile);
            return DPS_ERR;
        }

        if (p->end_ctb_in_slice_tile > frm_last_ctb_num) {
            dprint(PRN_ERROR, "Slice%d  end_ctb_in_slice_tile %d > FrmLastCtbNum %d\n",
                   i, p->end_ctb_in_slice_tile, frm_last_ctb_num);
            return DPS_ERR;
        }

        if (p->valid_bitlen[0] == 0) {
            dprint(PRN_ERROR, "error,slice%d valid_bitlen[0] is 0\n", i);
            return DPS_ERR;
        }
    }

    p->end_ctb_in_slice_tile = frm_last_ctb_num;

    if (p->valid_bitlen[0] == 0) {
        dprint(PRN_ERROR, "error,last slice valid_bitlen[0] is 0\n");
        return DPS_ERR;
    }

    return DPS_OK;
}

STATIC td_s32 dps_h265_calc_slice_end_addr_in_tile(hevc_ctx *ctx, const dec_pic_param_h265 *pic_param,
                                                   dec_slice_param_h265 *slice_head, td_u16 slice_num)
{
    if (ctx->ldy_mode == LDY_MULTI_SLC) {
        return dps_h265_calc_slice_end_addr_in_tile_mulslcldy(ctx, pic_param, slice_head, slice_num);
    }

    return dps_h265_calc_slice_end_addr_in_tile_normal(pic_param, slice_head, slice_num);
}

STATIC td_void dps_h265_calc_slice_end_addr_in_raster(const dec_pic_param_h265 *pic_param,
                                                      dec_slice_param_h265 *slice_head, td_u16 slice_num)
{
    dec_slice_param_h265 *p = slice_head;
    td_u32 i;

    for (i = 0; i < slice_num; i++, p = p->next_slice) {
        p->end_ctb_in_slice_raster = (td_u32)dps_h265_ctb_tsaddr_to_rsaddr(pic_param, p->end_ctb_in_slice_tile);
    }
}

STATIC td_s32 dps_h265_write_qmatrix(hevc_ctx *ctx, dec_pic_param_h265 *pic_param)
{
    td_s32 i;

    /* burst 1 */
    for (i = 0; i < 4; i++) { /* 4 is vfmw_max num */
        dps_h265_write_qmatrix_8x8(i, ctx, pic_param->qmatrix + 16 * i); /* 16 is size */
    }

    /* burst 2 */
    for (i = 0; i < 2; i++) { /* 2 is vfmw_max num */
        dps_h265_write_qmatrix_8x8(i + 4, ctx, pic_param->qmatrix + 64 * 1 + 16 * i); /* 4 64 16 is size */
    }

    for (i = 0; i < 2; i++) { /* 2 is vfmw_max num */
        dps_h265_write_qmatrix_16x16(i, ctx, pic_param->qmatrix + 64 * 1 + 32 + 16 * i); /* 64 32 16 is size */
    }

    /* burst 3 */
    for (i = 0; i < 4; i++) { /* 4 is vfmw_max num */
        dps_h265_write_qmatrix_16x16(i + 2, ctx, pic_param->qmatrix + 64 * 2 + 16 * i); /* 2 64 16 is size */
    }

    /* burst 4 */
    for (i = 0; i < 2; i++) { /* 2 is vfmw_max num */
        dps_h265_write_qmatrix_32x32(i, ctx, pic_param->qmatrix + 64 * 3 + 16 * i); /* 64 3 16 is size */
    }

    for (i = 0; i < 6; i++) { /* 6 is vfmw_max num */
        dps_h265_write_qmatrix_4x4(i, ctx, pic_param->qmatrix + 64 * 3 + 32 + 4 * i); /* 64 3 32 4 is size */
    }

    pic_param->qmatrix[64 * 3 + 56] = /* 64 3 56 is size */
        ((ctx->scaling_list_dc[2][3]) << 24) | /* 2 3 is array num, 24 is 24bit */
        ((ctx->scaling_list_dc[2][2]) << 16) | /* 2  is array num, 16 is 16bit */
        ((ctx->scaling_list_dc[2][1]) << 8) | /* 2  is array num, 8 is 8bit */
        (ctx->scaling_list_dc[2][0]); /* 2  is array num */

    pic_param->qmatrix[64 * 3 + 57] = /* 64 3 57 is size */
        ((ctx->scaling_list_dc[3][1]) << 24) | /* 3 is array num, 24 is 24bit */
        ((ctx->scaling_list_dc[3][0]) << 16) | /* 3 is array num, 16 is 16bit */
        ((ctx->scaling_list_dc[2][5]) << 8) | /* 2 5 is array num, 8 is 8bit */
        (ctx->scaling_list_dc[2][4]); /* 2 4 is array num */

    return DPS_OK;
}

STATIC td_s32 dps_h265_make_dps_pic_set_xps(const hevc_ctx *ctx, dec_pic_param_h265 *pic_param)
{
    td_s32 copy_size;
    hevc_seq_param_set *curr_sps = ctx->curr_sps;
    hevc_pic_param_set *curr_pps = ctx->curr_pps;

    if ((&pic_param->for_copy_end_1 - &pic_param->scaling_list_enabled_flag) !=
        (&curr_sps->for_copy_end - &curr_sps->scaling_list_enabled_flag)) {
        dprint(PRN_ERROR, "dps_h265_make_dps_pic sps struct copy order Err\n");
        return DPS_ERR;
    }

    copy_size = (td_u8 *)(&pic_param->for_copy_end_1) - (td_u8 *)(&pic_param->scaling_list_enabled_flag);
    vfmw_check_sec_func(memcpy_s(&pic_param->scaling_list_enabled_flag, copy_size,
                                 &curr_sps->scaling_list_enabled_flag, copy_size));

    pic_param->pic_width_in_ctbs_y_minus1--;
    pic_param->pic_hight_in_ctbs_y_minus1--;

    if ((&pic_param->for_copy_end_2 - &pic_param->sign_data_hiding_enabled_flag) !=
        (&curr_pps->for_copy_end - &curr_pps->sign_data_hiding_flag)) {
        dprint(PRN_ERROR, "dps_h265_make_dps_pic PPS struct copy order Err\n");
        return DPS_ERR;
    }

    copy_size = (td_u8 *)(&pic_param->for_copy_end_2) - (td_u8 *)(&pic_param->sign_data_hiding_enabled_flag);
    vfmw_check_sec_func(memcpy_s(&pic_param->sign_data_hiding_enabled_flag, copy_size,
                                 &curr_pps->sign_data_hiding_flag, copy_size));

    pic_param->log2_min_cu_qp_delta_size = curr_sps->log2_ctb_sizey - (td_u32)curr_pps->max_cu_qp_delta_depth;
    pic_param->num_tile_rows_minus1--;
    pic_param->num_tile_columns_minus1--;
    pic_param->log2_parallel_merge_level_minus2 -= 2; /* 2 is minus2 */

    if ((pic_param->tiles_enabled_flag == 1) &&
        (dps_h265_calc_tile_info(ctx, pic_param->tile_id, pic_param->tile_pos_x,
                                 pic_param->tile_pos_y) != DPS_OK)) {
        dprint(PRN_ERROR, "%s call dps_h265_calc_tile_info failed.\n", __func__);
        return DPS_ERR;
    }

    if (ctx->extra_data_ptr->air_version == AIR_DEC_FRAME) {
        pic_param->disp_width = ctx->curr_sps->air_parameters.air_big_width;
        pic_param->disp_height = ctx->curr_sps->air_parameters.air_big_height;
    } else {
        pic_param->disp_width = vfmw_min(curr_sps->disp_width, pic_param->pic_width_in_luma_samples);
        pic_param->disp_height = vfmw_min(curr_sps->disp_height, pic_param->pic_height_in_luma_samples);
    }
    pic_param->top_offset = curr_sps->conf_win_top_offset;
    pic_param->bottom_offset = curr_sps->conf_win_bottom_offset;

    return DPS_OK;
}

STATIC td_void dps_h265_update_dps_info(const hevc_ctx *ctx, const dec_pic_param_h265 *pic_param)
{
    vfmw_check((pic_param->pic_width_in_ctbs_y_minus1 + 1) < VFMW_MAX_UINT32);
    vfmw_check((pic_param->pic_hight_in_ctbs_y_minus1 + 1) < VFMW_MAX_UINT32);

    ctx->curr_dps->pic_ctb_num = (pic_param->pic_width_in_ctbs_y_minus1 + 1) *
        (pic_param->pic_hight_in_ctbs_y_minus1 + 1);

    ctx->curr_dps->logic_id = pic_param->fsp_logic_id;
    ctx->curr_dps->frm_width = pic_param->pic_width_in_luma_samples;
    ctx->curr_dps->frm_height = pic_param->pic_height_in_luma_samples;
    ctx->curr_dps->disp_width = pic_param->disp_width;
    ctx->curr_dps->disp_height = pic_param->disp_height;
    ctx->curr_dps->bit_depth = vfmw_max(pic_param->bit_depth_y, pic_param->bit_depth_c);
    ctx->curr_dps->ctb_size = 1 << pic_param->log2_ctb_size_y;
    ctx->curr_dps->top_offset = pic_param->top_offset;
    ctx->curr_dps->bottom_offset = pic_param->bottom_offset;
    ctx->curr_dps->pic_coding_type = pic_param->pic_type;
    if (ctx->extra_data_ptr->air_version == AIR_DEC_FRAME) {
        ctx->curr_dps->air_dec_info.air_version = AIR_DEC_FRAME;
        ctx->curr_dps->air_dec_info.air_logic_id = ctx->air_logic_id;
        ctx->curr_dps->air_dec_info.air_sta = ctx->air_sta;
        ctx->curr_dps->air_dec_info.slice_id = ctx->curr_pic.air_slice_id;
        ctx->curr_dps->air_dec_info.height_ofs = ctx->curr_sps->pic_height_in_luma_samples -
            ctx->curr_sps->conf_win_top_offset - ctx->curr_sps->conf_win_bottom_offset;
    } else {
        ctx->curr_dps->air_dec_info.air_version = AIR_DEC_UNSUPPORT;
        ctx->curr_dps->air_dec_info.air_logic_id = -1;
    }
}

STATIC td_void dps_h265_get_cur_ref_frm_slice(const dec_slice_param_h265 *p, dec_pic_param_h265 *pic_param, td_u32 idx)
{
    td_u32 j, k;

    for (j = 0; j < p->listx_size[idx]; j++) {
        for (k = 0; k < pic_param->ref_pic_single_size; k++) {
            if ((td_u32)pic_param->ref_pic_single_fsp_id[k] == p->idx_2_fsp_id[idx][j]) {
                break;
            }
        }

        if (k >= pic_param->ref_pic_single_size &&
            pic_param->ref_pic_single_size < pic_param->apc_size) {
            k = pic_param->ref_pic_single_size++;
            pic_param->ref_pic_single_fsp_id[k] = p->idx_2_fsp_id[idx][j];
        }
    }
}

STATIC td_void dps_h265_get_cur_ref(const hevc_ctx *ctx, dec_pic_param_h265 *pic_param,
    const dec_slice_param_h265 *slice_head)
{
    td_u32 i;
    td_u32 slice_num;
    const dec_slice_param_h265 *p = slice_head;

    slice_num = (td_u32)ctx->curr_dps->slice_num;
    pic_param->ref_pic_single_size = 0;

    for (i = 0; i < slice_num; i++, p = p->next_slice) {
        if (pic_param->ref_pic_single_size >= pic_param->apc_size) {
            /* for fast quit */
            break;
        }

        dps_h265_get_cur_ref_frm_slice(p, pic_param, 0);

        if (p->slice_type != HEVC_B_SLICE) {
            continue;
        }

        dps_h265_get_cur_ref_frm_slice(p, pic_param, 1);
    }
}

STATIC td_void dps_h265_calc_ref_cnt(const hevc_ctx *ctx, dec_pic_param_h265 *pic_param)
{
    td_u16 i;
    hevc_frame_store *hevc_fs = TD_NULL;
    td_u16 ref_cnt = 0;
    td_u16 max_ref_num;

    max_ref_num = vfmw_min(ctx->dpb.used_size, HEVC_APC_SIZE);
    for (i = 0; i < max_ref_num; i++) {
        hevc_fs = ctx->dpb.fs[i];
        if (hevc_fs->is_reference && hevc_fs->logic_fs_id != (td_s32)pic_param->fsp_logic_id &&
            ref_cnt < HEVC_APC_SIZE) {
            pic_param->apc_fsp_id[ref_cnt] = (td_u32)hevc_fs->logic_fs_id;
            pic_param->apc_poc[ref_cnt] = hevc_fs->poc;
#ifdef VFMW_MV_HEVC_SUPPORT
            pic_param->base_layer_ref_en[ref_cnt] = 0;

            if (hevc_fs->view_id == 0) {
                pic_param->base_layer_ref_en[ref_cnt] = 1;
            }
#endif
            ref_cnt++;
        }
    }
    pic_param->apc_size = ref_cnt;
}

STATIC td_s32 dps_h265_make_dps_pic_set_pic(hevc_ctx *ctx, dec_pic_param_h265 *pic_param)
{
    dec_slice_param_h265 *slice_param = TD_NULL;
    td_s32 ret;
    const hevc_curr_pic *curr_pic = &ctx->curr_pic;

    pic_param->this_poc = curr_pic->thispoc;
    pic_param->pic_type = curr_pic->pic_type;
    pic_param->fsp_logic_id = (td_u32)ctx->curr_pic.frame_store->logic_fs_id;
    pic_param->stream_base_addr = ctx->curr_pic.stream_base_addr;
#ifdef VFMW_MV_HEVC_SUPPORT
    pic_param->view_id = curr_pic->view_id;
#endif

    dps_h265_calc_ref_cnt(ctx, pic_param);

    if (pic_param->scaling_list_enabled_flag == 1) {
        dps_h265_write_qmatrix(ctx, pic_param);
    }

    /* calc slice address */
    if ((ctx->curr_dps != TD_NULL) && (ctx->curr_dps->slice_head != TD_NULL)) {
        slice_param = (dec_slice_param_h265 *)ctx->curr_dps->slice_head;
    } else {
        dprint(PRN_ERROR, "%s curr_dps or pSliceHead is TD_NULL!\n", __func__);
        return DPS_ERR;
    }
    if (slice_param == TD_NULL) {
        dprint(PRN_ERROR, "%s pHevcCtx->curr_dps->pSliceHead is TD_NULL!\n", __func__);
        return DPS_ERR;
    }

    ret = dps_h265_calc_slice_end_addr_in_tile(ctx, pic_param, slice_param, ctx->curr_dps->slice_num);
    vfmw_check_ret_print(ret == DPS_OK, DPS_ERR, "%s call dps_h265_calc_slice_end_addr_in_tile failed.\n",  __func__);

    dps_h265_calc_slice_end_addr_in_raster(pic_param, slice_param, ctx->curr_dps->slice_num);
    pic_param->sed_state = (td_u32)ctx->curr_dps->sed_state;

    dps_h265_get_cur_ref(ctx, pic_param, slice_param);

    dps_h265_update_dps_info(ctx, pic_param);

    vfmw_check_ret_no_print(ctx->extra_data_ptr->air_version == AIR_DEC_FRAME, DPS_OK);
    if (ctx->curr_dps->air_dec_info.slice_id == ctx->curr_sps->air_parameters.air_slice_num - 1) {
        if (ctx->curr_dps->air_dec_info.height_ofs * ctx->curr_sps->air_parameters.air_slice_num ==
            ctx->curr_sps->air_parameters.air_big_height) {
            pic_param->disp_height = ctx->curr_dps->air_dec_info.height_ofs;
        } else {
            vfmw_check_ret((ctx->curr_sps->air_parameters.air_slice_num - 1) *
                (td_u64)ctx->curr_dps->air_dec_info.height_ofs < VFMW_MAX_UINT32, DPS_ERR);
            pic_param->pic_height_in_luma_samples = ctx->curr_sps->air_parameters.air_big_height -
            (ctx->curr_sps->air_parameters.air_slice_num - 1) * ctx->curr_dps->air_dec_info.height_ofs;
            pic_param->disp_height = pic_param->pic_height_in_luma_samples;
        }
    } else {
        pic_param->disp_height = ctx->curr_dps->air_dec_info.height_ofs;
    }

    return DPS_OK;
}

td_s32 dps_h265_make_dps_pic(td_void *syn_ctx, td_void *param)
{
    hevc_ctx *ctx = (hevc_ctx *)syn_ctx;
    dec_pic_param_h265 *pic_param = (dec_pic_param_h265 *)param;

    vfmw_check_ret(ctx != TD_NULL, DPS_ERR);

    /* avoid memcpy/memmov while decxps/initpic delete currxps */
    /* avoid pcurrxps rewriten by new xps before find new pic */
    /* set xps param while initpic and set pic param in pic end */
    if (ctx->set_dps_xps == 1) {
        goto setxps;
    } else {
        goto setpic;
    }

setxps:

    return dps_h265_make_dps_pic_set_xps(ctx, pic_param);

setpic:

    return dps_h265_make_dps_pic_set_pic(ctx, pic_param);
}

#ifdef VFMW_MV_HEVC_SUPPORT
STATIC td_void dps_265_get_shvc_ref_imag(hevc_ctx *ctx, const dec_slice_param_h265 *slice_param, vfmw_image **img)
{
    td_u32 i;

    if (img != TD_NULL) {
        for (i = 0; i < slice_param->listx_size[0]; i++) {
            if (ctx->list_x[0][i]->frame_store->view_id == 0) {
                *img = &(ctx->list_x[0][i]->frame_store->fs_img);
            }
        }

        for (i = 0; i < slice_param->listx_size[1]; i++) {
            if (ctx->list_x[1][i]->frame_store->view_id == 0) {
                *img = &(ctx->list_x[1][i]->frame_store->fs_img);
            }
        }
    }
}

STATIC td_void dps_h265_make_shvc_img_info(shvc_dec_slice_param *first_slice_param, const vfmw_image *img)
{
    first_slice_param->y_bit_depth = img->bit_depth;
    first_slice_param->cb_bit_depth = img->bit_depth;
    first_slice_param->head_info_size = img->dec_info.head_info_size;
    first_slice_param->ref_frame_width = img->image_width_ref;
    first_slice_param->ref_frame_height = img->image_height_ref;
}

STATIC td_void dps_h265_make_shvc_ref_region_wh(const hevc_pps_extension_set *pps_ext, const vfmw_image *img,
    td_s32 *ref_region_width_y, td_s32 *ref_region_height_y)
{
    vfmw_check(pps_ext->ref_region_left_offset[0] < VFMW_MAX_SINT32);
    vfmw_check(pps_ext->ref_region_right_offset[0] < VFMW_MAX_SINT32);
    vfmw_check(pps_ext->ref_region_top_offset[0] < VFMW_MAX_SINT32);
    vfmw_check(pps_ext->ref_region_bottom_offset[0] < VFMW_MAX_SINT32);
    *ref_region_width_y = img->image_width_ref - pps_ext->ref_region_left_offset[0] * 2 - /* 2 is param of alg */
        pps_ext->ref_region_right_offset[0] * 2;
    *ref_region_height_y = img->image_height_ref - pps_ext->ref_region_top_offset[0] * 2 - /* 2 is param of alg */
        pps_ext->ref_region_bottom_offset[0] * 2;
}

STATIC td_void dps_h265_make_shvc_scale_region_wh(const hevc_ctx *ctx, const hevc_pps_extension_set *pps_ext,
    td_s32 *scale_region_width_y, td_s32 *scale_region_height_y)
{
    vfmw_check(pps_ext->scaled_ref_layer_left_offset[0] < VFMW_MAX_SINT32);
    vfmw_check(pps_ext->scaled_ref_layer_right_offset[0] < VFMW_MAX_SINT32);
    vfmw_check(pps_ext->scaled_ref_layer_top_offset[0] < VFMW_MAX_SINT32);
    vfmw_check(pps_ext->scaled_ref_layer_bottom_offset[0] < VFMW_MAX_SINT32);
    *scale_region_width_y = ctx->curr_pic.pic_width -
        pps_ext->scaled_ref_layer_left_offset[0] * 2 - /* 2 is param of algorithm */
        pps_ext->scaled_ref_layer_right_offset[0] * 2; /* 2 is param of algorithm */
    *scale_region_height_y = ctx->curr_pic.pic_height -
        pps_ext->scaled_ref_layer_top_offset[0] * 2 - /* 2 is param of algorithm */
        pps_ext->scaled_ref_layer_bottom_offset[0] * 2; /* 2 is param of algorithm */
}

STATIC td_void dps_h265_make_shvc_scale_y(shvc_dec_slice_param *first_slice_param,
    td_s32 ref_region_width_y, td_s32 scale_region_width_y, td_s32 ref_region_height_y, td_s32 scale_region_height_y)
{
    first_slice_param->scale_hor_y =
        (td_s32)os_do_div((((td_u64)ref_region_width_y) << 16) + /* 16 is param of algorithm */
            (((td_u64)scale_region_width_y) >> 1), (td_u32)scale_region_width_y);
    first_slice_param->scale_ver_y =
        (td_s32)os_do_div((((td_u64)ref_region_height_y) << 16) + /* 16 is param of algorithm */
            (((td_u64)scale_region_height_y) >> 1), (td_u32)scale_region_height_y);
}

STATIC td_void dps_h265_make_shvc_scale_type(shvc_dec_slice_param *first_slice_param,
    td_s32 curr_width, td_s32 curr_height)
{
    td_s32 scale_type_hor, scale_type_ver;

    if (curr_width <= first_slice_param->ref_frame_width) {
        scale_type_hor = HEVC_SCALE_TYPE_10X;
        if (first_slice_param->scale_hor_y <= (1 << 16)) {  /* 16 is param of algorithm */
            scale_type_hor = HEVC_SCALE_TYPE_15X;
        }
    } else if (curr_width <= (first_slice_param->ref_frame_width * 3 / 2)) {  /* 3, 2 are param of algorithm */
        scale_type_hor = HEVC_SCALE_TYPE_15X;
    } else if (curr_width <= (first_slice_param->ref_frame_width * 2)) {  /* 2 is param of algorithm */
        scale_type_hor = HEVC_SCALE_TYPE_20X;
    } else if (curr_width <= (first_slice_param->ref_frame_width * 3)) {  /* 3 is param of algorithm */
        scale_type_hor = HEVC_SCALE_TYPE_30X;
    } else {
        scale_type_hor = HEVC_SCALE_TYPE_30X;
        dprint(PRN_ERROR, "width is larger than 3! (%d, %d)\n", curr_width, first_slice_param->ref_frame_width);
    }

    if (curr_height <= first_slice_param->ref_frame_height) {
        scale_type_ver = HEVC_SCALE_TYPE_10X;
        if (first_slice_param->scale_ver_y <= (1 << 16)) {  /* 16 is param of algorithm */
            scale_type_ver = HEVC_SCALE_TYPE_15X;
        }
    } else if (curr_height <= (first_slice_param->ref_frame_height * 3 / 2)) {  /* 3, 2 are param of algorithm */
        scale_type_ver = HEVC_SCALE_TYPE_15X;
    } else if (curr_height <= (first_slice_param->ref_frame_height * 2)) {  /* 2 is param of algorithm */
        scale_type_ver = HEVC_SCALE_TYPE_20X;
    } else if (curr_height <= (first_slice_param->ref_frame_height * 3)) {  /* 3 is param of algorithm */
        scale_type_ver = HEVC_SCALE_TYPE_30X;
    } else {
        scale_type_ver = HEVC_SCALE_TYPE_30X;
        dprint(PRN_ERROR, "width is larger than 3! (%d, %d)\n", curr_height, first_slice_param->ref_frame_height);
    }

    first_slice_param->inter_layer_mp_enabled = 1;
    if (scale_type_hor != HEVC_SCALE_TYPE_10X) {
        first_slice_param->scale_type = ((td_u32)scale_type_ver | (td_u32)scale_type_hor);
    }

    first_slice_param->scale_hor_c = first_slice_param->scale_hor_y;
    first_slice_param->scale_ver_c = first_slice_param->scale_ver_y;
}

STATIC td_void dps_h265_make_shvc_phase(shvc_dec_slice_param *first_slice_param, const hevc_pps_extension_set *pps_ext,
    td_s32 ref_region_height_y, td_s32 scale_region_height_y)
{
    if (pps_ext->resample_phase_set_present_flag[0]) {
        first_slice_param->phase_hor_y = pps_ext->phase_hor_luma[0];
        first_slice_param->phase_ver_y = pps_ext->phase_ver_luma[0];
        first_slice_param->phase_hor_c = pps_ext->phase_hor_chroma_plus8[0] - 8; /* 8 is protocal required */
        first_slice_param->phase_ver_c = pps_ext->phase_ver_chroma_plus8[0] - 8; /* 8 is protocal required */
    } else {
        first_slice_param->phase_hor_y = 0;
        first_slice_param->phase_ver_y = 0;
        first_slice_param->phase_hor_c = 0;
        if (ref_region_height_y == 4) { /* 4 is protocal required */
            first_slice_param->phase_ver_c = 0;
        } else {
            first_slice_param->phase_ver_c = (((td_u32)scale_region_height_y << 2) + /* 2 is protocal required */
                ((td_u32)ref_region_height_y >> 1)) / ref_region_height_y - 4; /* 4 is protocal required */
        }
    }
}

STATIC td_void dps_h265_make_shvc_mv_factor(shvc_dec_slice_param *slice_param, td_s32 scale_region_width_y,
    td_s32 scale_region_height_y, td_s32 ref_region_width_y, td_s32 ref_region_height_y)
{
    vfmw_check(slice_param->scale_hor_y * slice_param->phase_hor_y < VFMW_MAX_SINT32);
    vfmw_check(slice_param->scale_ver_y * slice_param->phase_ver_y < VFMW_MAX_SINT32);
    vfmw_check(slice_param->scale_hor_c * slice_param->phase_hor_c < VFMW_MAX_SINT32);
    vfmw_check(slice_param->scale_ver_c * slice_param->phase_ver_c < VFMW_MAX_SINT32);
    slice_param->add_hor_y =
        -((td_u32)((slice_param->scale_hor_y * slice_param->phase_hor_y) + 8) >> 4); /* 8,4 is protocal required */
    slice_param->add_ver_y =
        -((td_u32)((slice_param->scale_ver_y * slice_param->phase_ver_y) + 8) >> 4); /* 8,4 is protocal required */
    slice_param->add_hor_c =
        -((td_u32)((slice_param->scale_hor_c * slice_param->phase_hor_c) + 8) >> 4); /* 8,4 is protocal required */
    slice_param->add_ver_c =
        -((td_u32)((slice_param->scale_ver_c * slice_param->phase_ver_c) + 8) >> 4); /* 8,4 is protocal required */

    if (ref_region_width_y != 0) {
        slice_param->scale_mv_x =
            (((td_u32)scale_region_width_y << 8) + ((td_u32)ref_region_width_y >> 1)) / /* 8 required */
                ref_region_width_y;
    }
    if (ref_region_height_y != 0) {
        slice_param->scale_mv_y =
            (((td_u32)scale_region_height_y << 8) + ((td_u32)ref_region_height_y >> 1)) / /* 8 required */
                ref_region_height_y;
    }
    slice_param->scale_mv_x = vfmw_clip_3(-4096, 4095, slice_param->scale_mv_x); /* 4096 4095 is protocal required */
    slice_param->scale_mv_y = vfmw_clip_3(-4096, 4095, slice_param->scale_mv_y); /* 4096 4095 is protocal required */
}

STATIC td_void dps_h265_calc_shvc_offset(shvc_dec_slice_param *slice_param, const hevc_pps_extension_set *pps_ext)
{
    vfmw_check(pps_ext->scaled_ref_layer_left_offset[0] < VFMW_MAX_SINT32);
    vfmw_check(pps_ext->scaled_ref_layer_top_offset[0] < VFMW_MAX_SINT32);
    slice_param->curr_offset_left_y = pps_ext->scaled_ref_layer_left_offset[0] * 2; /* 2 is protocal required */
    slice_param->curr_offset_top_y = pps_ext->scaled_ref_layer_top_offset[0] * 2; /* 2 is protocal required */
    slice_param->curr_offset_left_c = ((td_u32)(slice_param->curr_offset_left_y)) >> 1;
    slice_param->curr_offset_top_c = ((td_u32)(slice_param->curr_offset_top_y)) >> 1;

    vfmw_check(pps_ext->ref_region_left_offset[0] < VFMW_MAX_SINT32);
    vfmw_check(pps_ext->ref_region_top_offset[0] < VFMW_MAX_SINT32);
    slice_param->ref_offset_left_y = pps_ext->ref_region_left_offset[0] * 2; /* 2 is protocal required */
    slice_param->ref_offset_top_y = pps_ext->ref_region_top_offset[0] * 2; /* 2 is protocal required */
    slice_param->ref_offset_left_c = ((td_u32)(slice_param->ref_offset_left_y)) >> 1;
    slice_param->ref_offset_top_c = ((td_u32)(slice_param->ref_offset_top_y)) >> 1;
}

STATIC td_void dps_h265_make_shvc_slice_ref_ctb_size(const hevc_ctx *ctx, shvc_dec_slice_param *slice_param,
    hevc_seq_param_set *curr_sps, const vfmw_image *img)
{
    const td_u32 ctb_size = 1 << ctx->curr_sps->log2_ctb_sizey;
    slice_param->ref_frame_width_align_ctb = (img->image_width_ref + (ctb_size - 1)) & (~(ctb_size - 1));
    slice_param->ref_frame_height_align_ctb = (img->image_height_ref + (ctb_size - 1)) & (~(ctb_size - 1));
}

STATIC td_s32 dps_h265_make_shvc_dps_slice(hevc_ctx *ctx, dec_slice_param_h265 *slice_param)
{
    td_s32 ref_region_width_y = 0;
    td_s32 ref_region_height_y = 0;
    td_s32 scale_region_width_y = 0;
    td_s32 scale_region_height_y = 0;
    td_s32 curr_width = (td_s32)ctx->curr_pic.pic_width;
    td_s32 curr_height = (td_s32)ctx->curr_pic.pic_height;
    hevc_pps_extension_set *pps_ext = &(ctx->curr_pps->pps_extension);
    dec_slice_param_h265 *first_slc_para = (dec_slice_param_h265 *)(ctx->curr_dps->slice_list[0]);
    shvc_dec_slice_param *first_slice_param = &(first_slc_para->shvc_slice_param);
    shvc_dec_slice_param *curr_slice_param = &(slice_param->shvc_slice_param);
    vfmw_image *img = TD_NULL;

    curr_slice_param->view_id = ctx->curr_slice.view_id;

    if (ctx->is_shvc_type == 0 || ctx->curr_slice.view_id == 0) {
        curr_slice_param->scale_type = HEVC_SCALE_TYPE_10X;
        curr_slice_param->inter_layer_mp_enabled = 0;
        curr_slice_param->col_mv_ref_eqaul_flag = 1;
        return HEVC_DEC_NORMAL;
    }

    if (curr_slice_param == first_slice_param) {
        first_slice_param->scale_type = HEVC_SCALE_TYPE_10X;
        first_slice_param->inter_layer_mp_enabled = 0;
    }

    dps_265_get_shvc_ref_imag(ctx, slice_param, &img);

    if (img != TD_NULL) {
        dps_h265_make_shvc_img_info(first_slice_param, img);
        dps_h265_make_shvc_ref_region_wh(pps_ext, img, &ref_region_width_y, &ref_region_height_y);
        dps_h265_make_shvc_scale_region_wh(ctx, pps_ext, &scale_region_width_y, &scale_region_height_y);

        if (scale_region_width_y == 0 || scale_region_height_y == 0 || ref_region_height_y == 0) {
            dprint(PRN_FATAL, "scale_region_width_y is %d  scale_region_height_y is %d, error",
                scale_region_width_y, scale_region_height_y);
            return HEVC_DEC_ERR;
        }

        dps_h265_make_shvc_scale_y(first_slice_param, ref_region_width_y, scale_region_width_y,
            ref_region_height_y, scale_region_height_y);

        dps_h265_make_shvc_scale_type(first_slice_param, curr_width, curr_height);
        curr_slice_param->inter_layer_mp_enabled = first_slice_param->inter_layer_mp_enabled;

        dps_h265_make_shvc_phase(first_slice_param, pps_ext, ref_region_height_y, scale_region_height_y);

        dps_h265_make_shvc_mv_factor(first_slice_param, scale_region_width_y, scale_region_height_y,
            ref_region_width_y, ref_region_height_y);
        curr_slice_param->scale_mv_x = first_slice_param->scale_mv_x;
        curr_slice_param->scale_mv_y = first_slice_param->scale_mv_y;

        dps_h265_calc_shvc_offset(first_slice_param, pps_ext);

        dps_h265_make_shvc_slice_ref_ctb_size(ctx, first_slice_param, ctx->curr_sps, img);
    }

    return HEVC_DEC_NORMAL;
}
#endif

STATIC td_void dps_h265_make_dps_stream_info(const hevc_ctx *ctx, dec_slice_param_h265 *slice_param)
{
    td_u32 i;

    for (i = ctx->curr_nal->nal_used_segment; ((i < ctx->curr_nal->nal_segment) && (i < 2)); i++) { /* 2 is seg num */
        slice_param->stream_id[i] = ctx->curr_nal->stream[i].stream_id;
        slice_param->bits_offset[i] = ctx->curr_nal->stream[i].bits_offset;
        slice_param->valid_bitlen[i] = ctx->curr_nal->stream[i].valid_bitlen;
        slice_param->bytes_pos[i] = ctx->curr_nal->stream[i].bytes_pos;
    }

    for (i = ctx->curr_nal->nal_segment; i < 2; i++) { /* 2 is seg num */
        slice_param->stream_id[i] = INVALID_IDX;
        slice_param->bits_offset[i] = 0;
        slice_param->valid_bitlen[i] = 0;
        slice_param->bytes_pos[i] = 0;
    }
}

VFMW_ACC_SLICE_H265 td_s32 dps_h265_make_dps_slice(td_void *syn_ctx, td_void *param, td_void *prev_param)
{
    hevc_ctx *ctx = (hevc_ctx *)syn_ctx;
    dec_slice_param_h265 *slice_param = (dec_slice_param_h265 *)param;
    dec_slice_param_h265 *prev_slice_param = (dec_slice_param_h265 *)prev_param;
    hevc_slice_segment_header *curr_slice = TD_NULL;
    hevc_slice_segment_header *slice = TD_NULL;
    td_s32 copy_size;

    vfmw_check_ret(ctx != TD_NULL && ctx->curr_nal != TD_NULL && slice_param != TD_NULL, DPS_ERR);
    curr_slice = &ctx->curr_slice;

    if (ctx->curr_slice.dependent_slice_segment_flag == 0) {
        slice = &ctx->curr_slice;
    } else {
        slice = &ctx->independent_slice;
    }

    dps_h265_make_dps_stream_info(ctx, slice_param);

    slice_param->dependent_slice_segment_flag = curr_slice->dependent_slice_segment_flag;
    slice_param->slice_segment_address = (td_u32)curr_slice->slice_segment_address;

    if ((&slice_param->for_copy_end - &slice_param->slice_sao_luma_flag) !=
        (&slice->for_copy_end - &slice->slice_sao_luma_flag)) {
        dprint(PRN_ERROR, "dps_h265_make_dps_slice copy struct, the struct order changed\n");
        return DPS_ERR;
    }

    /* Improve performance */
    copy_size = (td_u8 *)(&slice_param->for_copy_end) - (td_u8 *)(&slice_param->slice_sao_luma_flag);
    vfmw_check_sec_func(memcpy_s(&slice_param->slice_sao_luma_flag, copy_size,
                                 &slice->slice_sao_luma_flag, copy_size));

    slice_param->slice_segment_tile_address =
        (td_u32)dps_h265_ctb_rsaddr_to_tsaddr(ctx, curr_slice->slice_segment_address);
    slice_param->num_ref_idx_l0_active_minus1 = (td_u8)(vfmw_max((td_s32)slice->num_ref_idx[0] - 1, 0));
    slice_param->num_ref_idx_l1_active_minus1 = (td_u8)(vfmw_max((td_s32)slice->num_ref_idx[1] - 1, 0));

    dps_h265_make_dps_slice_ref_info(ctx, slice_param);

    dps_h265_make_dps_slice_wp_info(ctx, slice_param);
#ifdef VFMW_MV_HEVC_SUPPORT
    if (dps_h265_make_shvc_dps_slice(ctx, slice_param) != HEVC_DEC_NORMAL) {
        return DPS_ERR;
    }
#endif

    slice_param->next_slice = TD_NULL;
    if (prev_slice_param != TD_NULL) {
        prev_slice_param->next_slice = slice_param;
    }

    return DPS_OK;
}

td_s32 dps_h265_get_ref_info(const td_void *param, dps_ref_info *dps_ref)
{
    dec_pic_param_h265 *pic_param = (dec_pic_param_h265 *)param;

    vfmw_check_ret(dps_ref != TD_NULL && pic_param != TD_NULL, DPS_ERR);

    dps_ref->ref_size = pic_param->apc_size;
    dps_ref->ref_fsp_id = &pic_param->apc_fsp_id[0];
    dps_ref->apc_poc = &pic_param->apc_poc[0];
    dps_ref->apc_2_fsp_phy_addr = &pic_param->apc_fsp_phy_addr[0];
    dps_ref->ref_pic_type = TD_NULL;
    dps_ref->cur_fsp_id = pic_param->fsp_logic_id;

    dps_ref->cur_ref_size = pic_param->ref_pic_single_size;
    dps_ref->cur_ref_fsp_id = (td_u32 *)&pic_param->ref_pic_single_fsp_id[0];
    dps_ref->cur_ref_fsp_phy_addr = &pic_param->ref_pic_single_phy_addr[0];

    return DPS_OK;
}

td_s32 dps_h265_get_seg_id(const td_void *param, td_s32 *seg_id)
{
    dec_slice_param_h265 *slice_param = (dec_slice_param_h265 *)param;
    td_s32 i;
    vfmw_check_ret(slice_param != TD_NULL && seg_id != TD_NULL, DPS_ERR);

    for (i = 0; i < 2; i++) { /* 2 is seg num */
        seg_id[i] = slice_param->stream_id[i];
    }

    return DPS_OK;
}

VFMW_ACC_SLICE_H265 td_s32 dps_h265_release_seg(td_s32 chan_id, const td_void *param)
{
    td_s32 i;
    td_s32 j;
    td_s32 seg_id;
    dps_info *dps = TD_NULL;
    dec_slice_param_h265 *slice = TD_NULL;

    vfmw_check_ret(param != TD_NULL, DEC_ERR);

    dps = (dps_info *)param;

    for (i = 0; i < dps->slice_num; i++) {
        slice = (dec_slice_param_h265 *)(dps->slice_list[i]);
        for (j = 0; j < 2; j++) { /* 2 is seg num */
            seg_id = slice->stream_id[j];
            if (seg_id == INVALID_IDX) {
                continue;
            }
            if (stm_release_seg(chan_id, seg_id) != STM_OK) {
                dprint(PRN_ERROR, "%s failed Seg[%d] Id %d\n", __func__, j, seg_id);
            }
        }
    }

    return DPS_OK;
}
