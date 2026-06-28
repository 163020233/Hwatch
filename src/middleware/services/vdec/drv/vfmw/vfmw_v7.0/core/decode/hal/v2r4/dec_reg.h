/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2006-2024. All rights reserved.
 * Description: dec hal
 * Author: AuthorNameMagicTag
 * Create: 2006-01-01
 */

#ifndef __DEC_REG_H__
#define __DEC_REG_H__

#include "dec_base.h"

#ifdef __cplusplus
extern "C" {
#endif

#define  VREG_OFS_VDH_START                0x000
#define  VREG_OFS_VDH_REPAIR               0x004
#define  VREG_OFS_BASIC_CFG0               0x008
#define  VREG_OFS_BASIC_CFG1               0x00c
#define  VREG_OFS_AVM_ADDR                 0x010
#define  VREG_OFS_VAM_ADDR                 0x014
#define  VREG_OFS_STREAM_BASE_ADDR         0x018
#define  VREG_OFS_VDH_STATE                0x01c
#define  VREG_OFS_INT_STATE                0x020
#define  VREG_OFS_INT_MASK                 0x024
#define  VREG_OFS_SED_TO                   0x03c
#define  VREG_OFS_ITRANS_TO                0x040
#define  VREG_OFS_PMV_TO                   0x044
#define  VREG_OFS_PRC_TO                   0x048
#define  VREG_OFS_RCN_TO                   0x04c
#define  VREG_OFS_DBLK_TO                  0x050
#define  VREG_OFS_PPFD_TO                  0x054
#define  VREG_OFS_DEC_LEVEL                0x05c
#define  VREG_OFS_YSTADDR_1D               0x060
#define  VREG_OFS_YSTRIDE_1D               0x064
#define  VREG_OFS_UVOFFSET_1D              0x068
#define  VREG_OFS_HEAD_INF_OFFSET          0x06c
#define  VREG_OFS_LINE_NUM_STADDR          0x070
#define  VREG_OFS_YSTRIDE_2BIT             0x074
#define  VREG_OFS_YOFFSET_2BIT             0x078
#define  VREG_OFS_UVOFFSET_2BIT            0x07C
#define  VREG_OFS_PPFD_BUF_ADDR            0x080
#define  VREG_OFS_PPFD_BUF_LEN             0x084
#define  VREG_OFS_REF_PIC_TYPE             0x094
#define  VREG_OFS_FF_APT_EN                0x098
#define  VREG_OFS_VDH_CYCLEPERPIC          0x0B0
#define  VREG_OFS_MB0_QP_IN_CURR_PIC       0x0D0
#define  VREG_OFS_SWITCH_ROUNDING          0x0D4
#define  VREG_OFS_SED_STATE1_0             0x114
#define  VREG_OFS_VDH_LUMA_PIXEL_SUM       0x914
#define  VREG_OFS_VDH_CHROMA_PIXEL_SUM     0x918
#define  VREG_OFS_EMAR_ID                  0xc04
#define  VREG_OFS_VDH_SELRST               0xc08

typedef struct {
    td_u32 mbamt_to_dec :                         20;
    td_u32 uv_order_en :                          1;
    td_u32 reserved :                             1;
    td_u32 marker_bit_detect_en :                 1;
    td_u32 ac_last_detect_en :                    1;
    td_u32 coef_idx_detect_en :                   1;
    td_u32 vop_type_detect_en :                   1;
    td_u32 work_mode :                            2;
    td_u32 luma_sum_en :                          1;
    td_u32 luma_histogram_en :                    1;
    td_u32 load_qmatrix_flag :                    1;
    td_u32 vdh_safe_flag :                        1;
} VREG_BASIC_CFG0;

typedef struct {
    td_u32 video_standard :                       4;
    td_u32 ddr_stride :                           10;
    td_u32 fst_slc_grp :                          1;
    td_u32 mv_output_en :                         1;
    td_u32 max_slcgrp_num :                       12;
    td_u32 line_num_oen :                         1;
    td_u32 dblk_1d_en :                           1;
    td_u32 compress_en :                          1;
    td_u32 ppfd_en :                              1;
} VREG_BASIC_CFG1;

typedef struct {
    td_u32 av_msg_addr :                          32;
} VREG_AVM_ADDR;

typedef struct {
    td_u32 va_msg_addr :                          32;
} VREG_VAM_ADDR;

typedef struct {
    td_u32 stream_base_addr :                     32;
} VREG_STREAM_BASE_ADDR;

typedef struct {
    td_u32 ystaddr_1d :                           32;
} VREG_YSTADDR_1D;

typedef struct {
    td_u32 ystride_1d :                           32;
} VREG_YSTRIDE_1D;

typedef struct {
    td_u32 uvoffset_1d :                          32;
} VREG_UVOFFSET_1D;

typedef struct {
    td_u32 ystride_2bit :                         32;
} VREG_YSTRIDE_2BIT;

typedef struct {
    td_u32 yoffset_2bit :                         32;
} VREG_YOFFSET_2BIT;

typedef struct {
    td_u32 uvoffset_2bit :                        32;
} VREG_UVOFFSET_2BIT;


#ifdef __cplusplus
}
#endif

#endif
