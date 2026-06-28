/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __DEC_REG_H__
#define __DEC_REG_H__

#include "dec_base.h"

#ifndef ENV_SOS_KERNEL
#define OFS_BASE1 0x020
#define OFS_BASE2 0x100
#else
#define OFS_BASE1 0x300
#define OFS_BASE2 0x400
#endif

#define VREG_OFS_DOMN_CLK_CNT 0x0
#define VREG_OFS_RESERVED     0x4
#define VREG_OFS_RFS_RAS_EMA  0x8
#define VREG_OFS_RFT_EMA      0xC
#define VREG_OFS_ROM_VRG_EMA  0x10
#define VREG_OFS_RST_EN       0x14
#define VREG_OFS_CORRECT_CNT  0x60

#define VREG_OFS_BSP_INTR_STATE    0x00
#define VREG_OFS_BSP_INTR_MSKSTATE 0x04
#define VREG_OFS_BSP_INTR_MASK     0x08
#define VREG_OFS_BSP_CUR_REG       0x0C
#define VREG_OFS_PXP_INTR_STATE    0x10
#define VREG_OFS_PXP_INTR_MSKSTATE 0x14
#define VREG_OFS_PXP_INTR_MASK     0x18
#define VREG_OFS_PXP_CUR_REG       0x1C
#define VREG_OFS_PXP_SLICE_NUM     0x20
#define VREG_OFS_BSP_CYCLES        0x24
#define VREG_OFS_PXP_CYCLES        0x28

#define VREG_OFS_VDH_LUMA_PIXEL_SUM_LOW     0x18
#define VREG_OFS_VDH_CHROMA_PIXEL_SUM_LOW   0x19
#define VREG_OFS_VDH_LUMA_PIXEL_SUM_HIGH    0x1a
#define VREG_OFS_VDH_CHROMA_PIXEL_SUM_HIGH  0x1a

#define VREG_OFS_MB0_QP_IN_CURR_PIC 0x30D0
#define VREG_OFS_SWITCH_ROUNDING    0x30D4

#define VREG_OFS_BETWEEN_REG 0x20

#define VREG_OFS_DEC_START     0x000
#define VREG_OFS_INFO_STATE    0x004
#define VREG_OFS_AVM_ADDR_CNT  0x008
#define VREG_OFS_PROCESS_STATE 0x00C
#define VREG_OFS_PXP_START     0x010

#define VREG_OFS_DOMN_CLK_BSP  0x4

#define VREG_OFS_VDH_CLKSEL         0xa004
#define VREG_OFS_VDH_SRST_REQ       0xa00c
#define VREG_OFS_VDH_SRST_OK        0xa014

typedef struct {
    td_u32 frame_indicator : 16;
    td_u32 reserved : 1;
    td_u32 intrst_vdh_dec_over : 1;
    td_u32 reserved1 : 14;
} vreg_bsp_intr_state;

typedef struct {
    td_u32 frame_indicator : 16;
    td_u32 reserved : 1;
    td_u32 intrst_vdh_dec_over : 1;
    td_u32 intrst_vdh_dec_err : 1;
    td_u32 intrst_vdh_dec_part : 1;
    td_u32 reserved1 : 4;
    td_u32 pxp_cur_reg : 3;
    td_u32 reserved2 : 5;
} vreg_pxp_intr_state;

typedef struct {
    td_u32 vdh_safe_flag : 1;
    td_u32 reserved1 : 1;
    td_u32 use_core_num : 2;
    td_u32 low_latency_en : 1;
    td_u32 reserved2 : 3;
    td_u32 bsp_internal_ram : 1;
    td_u32 pxp_internal_ram : 1;
    td_u32 reserved3 : 2;
    td_u32 frame_indicator : 16;
    td_u32 dec_cancel : 1;
    td_u32 pxp_state : 1;
    td_u32 bsp_state : 1;
    td_u32 vdec_busy_state : 1;
} vreg_info_state;

typedef struct {
    td_u32 mbamt_to_dec : 20;
    td_u32 reserved1 : 2;
    td_u32 marker_bit_detect_en : 1;
    td_u32 ac_last_detect_en : 1;
    td_u32 coef_idx_detect_en : 1;
    td_u32 vop_type_detect_en : 1;
    td_u32 work_mode : 2;
    td_u32 luma_sum_en : 1;
    td_u32 luma_histogram_en : 1;
    td_u32 load_qmatrix_flag : 1;
    td_u32 reserved2 : 1;
} vmsg_com_b0_d2;

typedef struct {
    td_u32 video_standard : 5;
    td_u32 reserved1 : 3;
    td_u32 cmp_left_neb_en : 1;
    td_u32 vdh_mdma_sel : 1;
    td_u32 water_mark_en : 1;
    td_u32 vpss_info_en : 1;
    td_u32 reserved2 : 1;
    td_u32 uv_order_en : 1;
    td_u32 fst_slc_grp : 1;
    td_u32 mv_output_en : 1;
    td_u32 max_slcgrp_num : 12;
    td_u32 line_num_output_en : 1;
    td_u32 vdh_2d_en : 1;
    td_u32 compress_en : 1;
    td_u32 reserved3 : 1;
} vmsg_com_b0_d3;

#endif
