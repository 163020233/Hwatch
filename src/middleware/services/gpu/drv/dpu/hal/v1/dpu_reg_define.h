/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description : dpu reg define file
 */

#ifndef DRV_GRAPHIC_HAL_DPU_REG_DEFINE_H
#define DRV_GRAPHIC_HAL_DPU_REG_DEFINE_H

#include "td_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#ifdef GPU_1A21
#define DPU_REG_BASEADDR                0x52090000
#else
#define DPU_REG_BASEADDR                0x56020000
#endif

#define HAL_DPU_SOFTRST_STATE           (DPU_REG_BASEADDR + 0xC) // DPU软复位状态 查询为0表示处于复位状态
#define HAL_DPU_SOFTRST_VAL             0

#define dpu_reg_read(addr)              (*(volatile unsigned int *)(uintptr_t)(addr))
#define dpu_reg_write(addr, val)        (*(volatile unsigned int *)(uintptr_t)(addr) = (val))

/*
 * DEFINE REGISTER UNION
 */
/* Define the union voctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved0               : 24;      /* [23:0] */
        unsigned int clk_ppc_para_ck_gt_en   : 1;       /* [24] */
        unsigned int clk_ppc_ck_gt_en        : 1;       /* [25] */
        unsigned int g1_ck_gt_en             : 1;       /* [26] */
        unsigned int g0_ck_gt_en             : 1;       /* [27] */
        unsigned int reserved1               : 2;       /* [29:28] */
        unsigned int intf_chk_sum_en         : 1;       /* [30] */
        unsigned int vo_ck_gt_en             : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_voctrl;

/* Define the union vointsta */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd0vtthd1_int          : 1;       /* [0] */
        unsigned int dhd0vtthd2_int          : 1;       /* [1] */
        unsigned int dhd0vtthd3_int          : 1;       /* [2] */
        unsigned int dhd0uf_int              : 1;       /* [3] */
        unsigned int reserved0               : 8;       /* [11:4] */
        unsigned int bus_err_int             : 1;       /* [12] */
        unsigned int reserved1               : 7;       /* [19:13] */
        unsigned int dhd0_te_int             : 1;       /* [20] */
        unsigned int dhd0_frm_end_int        : 1;       /* [21] */
        unsigned int reserved2               : 1;       /* [22] */
        unsigned int wbc_dhd_sta_int         : 1;       /* [23] */
        unsigned int reserved3               : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vointsta;

/* Define the union vomskintsta */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd0vtthd1_clr          : 1;       /* [0] */
        unsigned int dhd0vtthd2_clr          : 1;       /* [1] */
        unsigned int dhd0vtthd3_clr          : 1;       /* [2] */
        unsigned int dhd0uf_clr              : 1;       /* [3] */
        unsigned int reserved0               : 8;       /* [11:4] */
        unsigned int bus_err_clr             : 1;       /* [12] */
        unsigned int reserved1               : 7;       /* [19:13] */
        unsigned int dhd0_te_clr             : 1;       /* [20] */
        unsigned int dhd0_frm_end_clr        : 1;       /* [21] */
        unsigned int reserved2               : 1;       /* [22] */
        unsigned int wbc_dhd_sta_clr         : 1;       /* [23] */
        unsigned int reserved3               : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vomskintsta;

/* Define the union vointmsk */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd0vtthd1_intmask      : 1;       /* [0] */
        unsigned int dhd0vtthd2_intmask      : 1;       /* [1] */
        unsigned int dhd0vtthd3_intmask      : 1;       /* [2] */
        unsigned int dhd0uf_intmask          : 1;       /* [3] */
        unsigned int reserved0               : 8;       /* [11:4] */
        unsigned int bus_err_intmask         : 1;       /* [12] */
        unsigned int reserved1               : 7;       /* [19:13] */
        unsigned int dhd0_te_intmask         : 1;       /* [20] */
        unsigned int dhd0_frm_end_intmask    : 1;       /* [21] */
        unsigned int reserved2               : 1;       /* [22] */
        unsigned int wbc_dhd_sta_intmask     : 1;       /* [23] */
        unsigned int reserved3               : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vointmsk;

/* Define the union vodebug */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int rm_en_chn               : 4;       /* [3:0] */
        unsigned int dhd0_ff_info            : 2;       /* [5:4] */
        unsigned int dhd1_ff_info            : 2;       /* [7:6] */
        unsigned int reserved                : 24;      /* [31:8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vodebug;

/* Define the union vocmd_intsta */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd0_vocmd_int          : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vocmd_intsta;

/* Define the union vocmd_mskintsta */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd0_vocmd_clr          : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vocmd_mskintsta;

/* Define the union vocmd_intmsk */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd0_vocmd_intmask      : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vocmd_intmsk;

/* Define the union voversion1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int version1                : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_voversion1;

/* Define the union voversion2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int version2                : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_voversion2;

/* Define the union volowpower_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int c1x_hcspsb_test         : 3;       /* [2:0] */
        unsigned int c1x_hcspsb_teslw        : 2;       /* [4:3] */
        unsigned int c1x_hcspsb_teslr        : 3;       /* [7:5] */
        unsigned int c1x_hcspsb_tra          : 2;       /* [9:8] */
        unsigned int c1x_hcspsb_twa          : 2;       /* [11:10] */
        unsigned int c1x_hcspsb_twac         : 2;       /* [13:12] */
        unsigned int c1x_hdtpwa_test         : 3;       /* [16:14] */
        unsigned int c1x_hdtpwa_teslw        : 2;       /* [18:17] */
        unsigned int c1x_hdtpwa_teslr        : 2;       /* [20:19] */
        unsigned int c1x_hdtpwa_tra          : 2;       /* [22:21] */
        unsigned int c1x_hdtpwa_twa          : 2;       /* [24:23] */
        unsigned int c1x_hdtpwa_twac         : 2;       /* [26:25] */
        unsigned int reserved                : 5;       /* [31:27] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_volowpower_ctrl;

/* Define the union voufsta */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved0               : 8;       /* [7:0] */
        unsigned int g0_uf_sta               : 1;       /* [8] */
        unsigned int g1_uf_sta               : 1;       /* [9] */
        unsigned int reserved1               : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_voufsta;

/* Define the union voufclr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved0               : 8;       /* [7:0] */
        unsigned int g0_uf_clr               : 1;       /* [8] */
        unsigned int g1_uf_clr               : 1;       /* [9] */
        unsigned int reserved1               : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_voufclr;

/* Define the union vointproc_tim */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vointproc_time          : 24;      /* [23:0] */
        unsigned int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vointproc_tim;

/* Define the union vofpgatest */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved                : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vofpgatest;

/* Define the union volowpower_ctrl1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int c1x_hcbpsb_test         : 3;       /* [2:0] */
        unsigned int c1x_hcbpsb_teslw        : 2;       /* [4:3] */
        unsigned int c1x_hcbpsb_teslr        : 3;       /* [7:5] */
        unsigned int c1x_hcbpsb_tra          : 2;       /* [9:8] */
        unsigned int c1x_hcbpsb_twa          : 2;       /* [11:10] */
        unsigned int reserved                : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_volowpower_ctrl1;

/* Define the union dpu_sram_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dpu_block_en            : 10;      /* [9:0] */
        unsigned int reserved                : 20;      /* [29:10] */
        unsigned int dpu_lp_mode             : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_sram_ctrl;

/* Define the union cbm_bkg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cbm_bkgcr               : 8;       /* [7:0] */
        unsigned int cbm_bkgcb               : 8;       /* [15:8] */
        unsigned int cbm_bkgy                : 8;       /* [23:16] */
        unsigned int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_cbm_bkg;

/* Define the union cbm_mix */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mixer_prio0             : 4;       /* [3:0] */
        unsigned int mixer_prio1             : 4;       /* [7:4] */
        unsigned int mixer_prio2             : 4;       /* [11:8] */
        unsigned int reserved                : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_cbm_mix;

/* Define the union cbm_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved0               : 1;       /* [0] */
        unsigned int layer0_bypass_en        : 1;       /* [1] */
        unsigned int reserved1               : 13;      /* [14:2] */
        unsigned int bypass_mode             : 1;       /* [15] */
        unsigned int reserved2               : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_cbm_ctrl;

/* Define the union dpu_partial_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dpu_partial_cfg_vld     : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_partial_ctrl;

/* Define the union dpu_partial_gen_hdr0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int command0_datatype       : 6;       /* [5:0] */
        unsigned int command0_virtualchannel : 2;       /* [7:6] */
        unsigned int command0_wordcount_low  : 8;       /* [15:8] */
        unsigned int command0_wordcount_hi   : 8;       /* [23:16] */
        unsigned int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_partial_gen_hdr0;

/* Define the union dpu_partial_gen_pld_data00 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int command0_pld_b1         : 8;       /* [7:0] */
        unsigned int command0_pld_b2         : 8;       /* [15:8] */
        unsigned int command0_pld_b3         : 8;       /* [23:16] */
        unsigned int command0_pld_b4         : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_partial_gen_pld_data00;

/* Define the union dpu_partial_gen_pld_data01 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int command0_pld_b5         : 8;       /* [7:0] */
        unsigned int command0_pld_b6         : 8;       /* [15:8] */
        unsigned int command0_pld_b7         : 8;       /* [23:16] */
        unsigned int command0_pld_b8         : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_partial_gen_pld_data01;

/* Define the union dpu_partial_gen_hdr1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int command1_datatype       : 6;       /* [5:0] */
        unsigned int command1_virtualchannel : 2;       /* [7:6] */
        unsigned int command1_wordcount_low  : 8;       /* [15:8] */
        unsigned int command1_wordcount_hi   : 8;       /* [23:16] */
        unsigned int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_partial_gen_hdr1;

/* Define the union dpu_partial_gen_pld_data10 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int command1_pld_b1         : 8;       /* [7:0] */
        unsigned int command1_pld_b2         : 8;       /* [15:8] */
        unsigned int command1_pld_b3         : 8;       /* [23:16] */
        unsigned int command1_pld_b4         : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_partial_gen_pld_data10;

/* Define the union dpu_partial_gen_pld_data11 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int command1_pld_b5         : 8;       /* [7:0] */
        unsigned int command1_pld_b6         : 8;       /* [15:8] */
        unsigned int command1_pld_b7         : 8;       /* [23:16] */
        unsigned int command1_pld_b8         : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_partial_gen_pld_data11;

/* Define the union link_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved0               : 2;       /* [1:0] */
        unsigned int format_link             : 2;       /* [3:2] */
        unsigned int mapping_bind            : 1;       /* [4] */
        unsigned int bitext_mode             : 2;       /* [6:5] */
        unsigned int reserved1               : 24;      /* [30:7] */
        unsigned int g0_bypass_dhd0_en       : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_link_ctrl;

/* Define the union para_haddr_vhd_chn00 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int para_haddr_vhd_chn00    : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_para_haddr_vhd_chn00;

/* Define the union para_addr_vhd_chn00 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int para_addr_vhd_chn00     : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_para_addr_vhd_chn00;

/* Define the union para_up_vhd */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int para_up_vhd_chn00       : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_para_up_vhd;

/* Define the union para_conflict_clr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int para_conflict_clr_hd    : 1;       /* [0] */
        unsigned int para_conflict_clr_sd    : 1;       /* [1] */
        unsigned int reserved                : 30;      /* [31:2] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_para_conflict_clr;

/* Define the union para_conflict_sta */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int para_conflict_hd        : 1;       /* [0] */
        unsigned int para_conflict_sd        : 1;       /* [1] */
        unsigned int reserved                : 30;      /* [31:2] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_para_conflict_sta;

/* Define the union dpu_mapping_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved                : 26;      /* [25:0] */
        unsigned int cfg_en                  : 1;       /* [26] */
        unsigned int req_tile_sel            : 1;       /* [27] */
        unsigned int alpha_mapping_mode      : 1;       /* [28] */
        unsigned int tile_linear_sel         : 1;       /* [29] */
        unsigned int mapping_mode            : 1;       /* [30] */
        unsigned int mapping_en              : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_mapping_ctrl;

/* Define the union dpu_mapping_coef00 */
typedef union {
    /* Define the struct bits */
    struct {
        float mapping_coef00; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_mapping_coef00;

/* Define the union dpu_mapping_coef01 */
typedef union {
    /* Define the struct bits */
    struct {
        float mapping_coef01; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_mapping_coef01;

/* Define the union dpu_mapping_coef02 */
typedef union {
    /* Define the struct bits */
    struct {
        float mapping_coef02; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_mapping_coef02;

/* Define the union dpu_mapping_coef03 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved                : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_mapping_coef03;

/* Define the union dpu_mapping_coef10 */
typedef union {
    /* Define the struct bits */
    struct {
        float mapping_coef10; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_mapping_coef10;

/* Define the union dpu_mapping_coef11 */
typedef union {
    /* Define the struct bits */
    struct {
        float mapping_coef11; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_mapping_coef11;

/* Define the union dpu_mapping_coef12 */
typedef union {
    /* Define the struct bits */
    struct {
        float mapping_coef12; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_mapping_coef12;

/* Define the union dpu_mapping_coef13 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved                : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_mapping_coef13;

/* Define the union dpu_mapping_coef20 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved                : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_mapping_coef20;

/* Define the union dpu_mapping_coef21 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved                : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_mapping_coef21;

/* Define the union dpu_mapping_coef22 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved                : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_mapping_coef22;

/* Define the union dpu_mapping_coef23 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved                : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_mapping_coef23;

/* Define the union dpu_mapping_coef30 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved                : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_mapping_coef30;

/* Define the union dpu_mapping_coef31 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved                : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_mapping_coef31;

/* Define the union dpu_mapping_coef32 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved                : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_mapping_coef32;

/* Define the union dpu_mapping_coef33 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved                : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_mapping_coef33;

/* Define the union dpu_mapping_model_wh */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved                : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_mapping_model_wh;

/* Define the union dpu_mapping_pattern_fill_color */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int fill_color_b            : 8;       /* [7:0] */
        unsigned int fill_color_g            : 8;       /* [15:8] */
        unsigned int fill_color_r            : 8;       /* [23:16] */
        unsigned int fill_color_a            : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_mapping_pattern_fill_color;

/* Define the union dpu_mapping_shadow_color */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved                : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_mapping_shadow_color;

/* Define the union dpu_mapping_bg_color */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved                : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_mapping_bg_color;

/* Define the union dpu_mapping_scale_x */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved                : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_mapping_scale_x;

/* Define the union dpu_mapping_scale_y */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved                : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_mapping_scale_y;

/* Define the union dpu_mapping_offset_x */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved                : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_mapping_offset_x;

/* Define the union dpu_mapping_offset_y */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved                : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_mapping_offset_y;

/* Define the union dpu_mapping_src_ratio_x */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved                : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_mapping_src_ratio_x;

/* Define the union dpu_mapping_src_ratio_y */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved                : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_mapping_src_ratio_y;

/* Define the union dpu_mapping_src_offset_x */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved                : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_mapping_src_offset_x;

/* Define the union dpu_mapping_src_offset_y */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved                : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_mapping_src_offset_y;

/* Define the union dpu_mapping_debug0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pix_cnt_dfx             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_mapping_debug0;

/* Define the union dpu_mapping_crop_xy */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mapping_crop_x          : 16;      /* [15:0] */
        unsigned int mapping_crop_y          : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_mapping_crop_xy;

/* Define the union dpu_mapping_crop_wh */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mapping_crop_w          : 16;      /* [15:0] */
        unsigned int mapping_crop_h          : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_mapping_crop_wh;

/* Define the union dpu_add_pix_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int add_pix_en              : 1;       /* [0] */
        unsigned int add_pix_cfg_en          : 1;       /* [1] */
        unsigned int reserved                : 30;      /* [31:2] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_add_pix_ctrl;

/* Define the union dpu_add_pix_small */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int small_height            : 16;      /* [15:0] */
        unsigned int small_width             : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_add_pix_small;

/* Define the union dpu_add_pix_big */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int big_height              : 16;      /* [15:0] */
        unsigned int big_width               : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_add_pix_big;

/* Define the union dpu_add_pix_rect */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int rect_y                  : 16;      /* [15:0] */
        unsigned int rect_x                  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_add_pix_rect;

/* Define the union dpu_add_pix_tile */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tile_height             : 2;       /* [1:0] */
        unsigned int tile_width              : 4;       /* [5:2] */
        unsigned int reserved                : 26;      /* [31:6] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_add_pix_tile;

/* Define the union g0_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int galpha                  : 8;       /* [7:0] */
        unsigned int reserved0               : 19;      /* [26:8] */
        unsigned int depremult               : 1;       /* [27] */
        unsigned int rupd_field              : 1;       /* [28] */
        unsigned int rgup_mode               : 1;       /* [29] */
        unsigned int reserved1               : 1;       /* [30] */
        unsigned int surface_en              : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g0_ctrl;

/* Define the union g0_upd */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int regup                   : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g0_upd;

/* Define the union g0_0reso_read */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ow                      : 16;      /* [15:0] */
        unsigned int oh                      : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g0_0reso_read;

/* Define the union g0_dfpos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int disp_xfpos              : 16;      /* [15:0] */
        unsigned int disp_yfpos              : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g0_dfpos;

/* Define the union g0_dlpos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int disp_xlpos              : 16;      /* [15:0] */
        unsigned int disp_ylpos              : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g0_dlpos;

/* Define the union g0_vfpos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int video_xfpos             : 16;      /* [15:0] */
        unsigned int video_yfpos             : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g0_vfpos;

/* Define the union g0_vlpos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int video_xlpos             : 16;      /* [15:0] */
        unsigned int video_ylpos             : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g0_vlpos;

/* Define the union g0_bk */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved0               : 2;       /* [1:0] */
        unsigned int vbk_cr                  : 8;       /* [9:2] */
        unsigned int reserved1               : 2;       /* [11:10] */
        unsigned int vbk_cb                  : 8;       /* [19:12] */
        unsigned int reserved2               : 2;       /* [21:20] */
        unsigned int vbk_y                   : 8;       /* [29:22] */
        unsigned int reserved3               : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g0_bk;

/* Define the union g0_alpha */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vbk_alpha               : 8;       /* [7:0] */
        unsigned int reserved                : 24;      /* [31:8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g0_alpha;

/* Define the union g0_mute_bk */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mute_cr                 : 10;      /* [9:0] */
        unsigned int mute_cb                 : 10;      /* [19:10] */
        unsigned int mute_y                  : 10;      /* [29:20] */
        unsigned int reserved                : 1;       /* [30] */
        unsigned int mute_en                 : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g0_mute_bk;

/* Define the union g0_t2r0_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int t2r0_t2r_en             : 1;       /* [0] */
        unsigned int t2r0_flip_en            : 1;       /* [1] */
        unsigned int t2r0_copy_en            : 1;       /* [2] */
        unsigned int t2r0_delay_en           : 1;       /* [3] */
        unsigned int t2r0_cfg_en             : 1;       /* [4] */
        unsigned int t2r0_mode_sel           : 1;       /* [5] */
        unsigned int reserved0               : 2;       /* [7:6] */
        unsigned int t2r0_tile_wth           : 8;       /* [15:8] */
        unsigned int reserved1               : 15;      /* [30:16] */
        unsigned int t2r0_ck_gt_en           : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g0_t2r0_ctrl;

/* Define the union g0_t2r0_err_clr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int t2r0_of_err_clr         : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g0_t2r0_err_clr;

/* Define the union g0_t2r0_err_sta */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int t2r0_error              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g0_t2r0_err_sta;

/* Define the union g0_t2r0_debug1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int t2r0_debug1             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g0_t2r0_debug1;

/* Define the union g0_t2r0_debug2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int t2r0_debug2             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g0_t2r0_debug2;

/* Define the union g0_t2r0_debug3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int t2r0_debug3             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g0_t2r0_debug3;

/* Define the union g0_t2r0_debug4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int t2r0_debug4             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g0_t2r0_debug4;

/* Define the union g0_t2r0_wh */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int t2r0_width              : 16;      /* [15:0] */
        unsigned int t2r0_height             : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g0_t2r0_wh;

/* Define the union g0_crop0_wh */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int crop0_width             : 16;      /* [15:0] */
        unsigned int crop0_height            : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g0_crop0_wh;

/* Define the union g0_crop0_xy */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int crop0_x                 : 13;      /* [12:0] */
        unsigned int reserved0               : 3;       /* [15:13] */
        unsigned int crop0_y                 : 13;      /* [28:16] */
        unsigned int reserved1               : 1;       /* [29] */
        unsigned int crop0_cfg_en            : 1;       /* [30] */
        unsigned int crop0_en                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g0_crop0_xy;

/* Define the union g0_crop0_in_wh */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int crop0_in_width          : 16;      /* [15:0] */
        unsigned int crop0_in_height         : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g0_crop0_in_wh;

/* Define the union g1_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int galpha                  : 8;       /* [7:0] */
        unsigned int reserved0               : 19;      /* [26:8] */
        unsigned int depremult               : 1;       /* [27] */
        unsigned int rupd_field              : 1;       /* [28] */
        unsigned int rgup_mode               : 1;       /* [29] */
        unsigned int reserved1               : 1;       /* [30] */
        unsigned int surface_en              : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_ctrl;

/* Define the union g1_upd */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int regup                   : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_upd;

/* Define the union g1_0reso_read */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ow                      : 16;      /* [15:0] */
        unsigned int oh                      : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_0reso_read;

/* Define the union g1_dfpos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int disp_xfpos              : 16;      /* [15:0] */
        unsigned int disp_yfpos              : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_dfpos;

/* Define the union g1_dlpos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int disp_xlpos              : 16;      /* [15:0] */
        unsigned int disp_ylpos              : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_dlpos;

/* Define the union g1_vfpos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int video_xfpos             : 16;      /* [15:0] */
        unsigned int video_yfpos             : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_vfpos;

/* Define the union g1_vlpos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int video_xlpos             : 16;      /* [15:0] */
        unsigned int video_ylpos             : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_vlpos;

/* Define the union g1_bk */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved0               : 2;       /* [1:0] */
        unsigned int vbk_cr                  : 8;       /* [9:2] */
        unsigned int reserved1               : 2;       /* [11:10] */
        unsigned int vbk_cb                  : 8;       /* [19:12] */
        unsigned int reserved2               : 2;       /* [21:20] */
        unsigned int vbk_y                   : 8;       /* [29:22] */
        unsigned int reserved3               : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_bk;

/* Define the union g1_alpha */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vbk_alpha               : 8;       /* [7:0] */
        unsigned int reserved                : 24;      /* [31:8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_alpha;

/* Define the union g1_mute_bk */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mute_cr                 : 10;      /* [9:0] */
        unsigned int mute_cb                 : 10;      /* [19:10] */
        unsigned int mute_y                  : 10;      /* [29:20] */
        unsigned int reserved                : 1;       /* [30] */
        unsigned int mute_en                 : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_mute_bk;

/* Define the union g1_csc_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_en                  : 1;       /* [0] */
        unsigned int csc_demo_en             : 1;       /* [1] */
        unsigned int csc_ck_gt_en            : 1;       /* [2] */
        unsigned int reserved                : 29;      /* [31:3] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_ctrl;

/* Define the union g1_csc_coef00 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_coef00              : 12;      /* [11:0] */
        unsigned int reserved                : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_coef00;

/* Define the union g1_csc_coef01 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_coef01              : 12;      /* [11:0] */
        unsigned int reserved                : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_coef01;

/* Define the union g1_csc_coef02 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_coef02              : 12;      /* [11:0] */
        unsigned int reserved                : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_coef02;

/* Define the union g1_csc_coef10 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_coef10              : 12;      /* [11:0] */
        unsigned int reserved                : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_coef10;

/* Define the union g1_csc_coef11 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_coef11              : 12;      /* [11:0] */
        unsigned int reserved                : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_coef11;

/* Define the union g1_csc_coef12 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_coef12              : 12;      /* [11:0] */
        unsigned int reserved                : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_coef12;

/* Define the union g1_csc_coef20 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_coef20              : 12;      /* [11:0] */
        unsigned int reserved                : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_coef20;

/* Define the union g1_csc_coef21 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_coef21              : 12;      /* [11:0] */
        unsigned int reserved                : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_coef21;

/* Define the union g1_csc_coef22 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_coef22              : 12;      /* [11:0] */
        unsigned int reserved                : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_coef22;

/* Define the union g1_csc_scale */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_scale               : 4;       /* [3:0] */
        unsigned int reserved                : 28;      /* [31:4] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_scale;

/* Define the union g1_csc_idc0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_idc0                : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_idc0;

/* Define the union g1_csc_idc1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_idc1                : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_idc1;

/* Define the union g1_csc_idc2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_idc2                : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_idc2;

/* Define the union g1_csc_odc0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_odc0                : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_odc0;

/* Define the union g1_csc_odc1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_odc1                : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_odc1;

/* Define the union g1_csc_odc2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_odc2                : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_odc2;

/* Define the union g1_csc_min_y */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_min_y               : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_min_y;

/* Define the union g1_csc_min_c */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_min_c               : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_min_c;

/* Define the union g1_csc_max_y */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_max_y               : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_max_y;

/* Define the union g1_csc_max_c */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_max_c               : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_max_c;

/* Define the union g1_csc2_coef00 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_coef00             : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc2_coef00;

/* Define the union g1_csc2_coef01 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_coef01             : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc2_coef01;

/* Define the union g1_csc2_coef02 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_coef02             : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc2_coef02;

/* Define the union g1_csc2_coef10 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_coef10             : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc2_coef10;

/* Define the union g1_csc2_coef11 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_coef11             : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc2_coef11;

/* Define the union g1_csc2_coef12 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_coef12             : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc2_coef12;

/* Define the union g1_csc2_coef20 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_coef20             : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc2_coef20;

/* Define the union g1_csc2_coef21 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_coef21             : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc2_coef21;

/* Define the union g1_csc2_coef22 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_coef22             : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc2_coef22;

/* Define the union g1_csc2_scale */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_scale              : 4;       /* [3:0] */
        unsigned int reserved                : 28;      /* [31:4] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc2_scale;

/* Define the union g1_csc2_idc0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_idc0               : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc2_idc0;

/* Define the union g1_csc2_idc1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_idc1               : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc2_idc1;

/* Define the union g1_csc2_idc2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_idc2               : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc2_idc2;

/* Define the union g1_csc2_odc0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_odc0               : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc2_odc0;

/* Define the union g1_csc2_odc1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_odc1               : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc2_odc1;

/* Define the union g1_csc2_odc2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_odc2               : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc2_odc2;

/* Define the union g1_csc2_min_y */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_min_y              : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc2_min_y;

/* Define the union g1_csc2_min_c */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_min_c              : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc2_min_c;

/* Define the union g1_csc2_max_y */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_max_y              : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc2_max_y;

/* Define the union g1_csc2_max_c */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_max_c              : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc2_max_c;

/* Define the union g1_csc_ink_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ink_en                  : 1;       /* [0] */
        unsigned int ink_sel                 : 1;       /* [1] */
        unsigned int data_fmt                : 1;       /* [2] */
        unsigned int cross_enable            : 1;       /* [3] */
        unsigned int color_mode              : 2;       /* [5:4] */
        unsigned int reserved                : 26;      /* [31:6] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_ink_ctrl;

/* Define the union g1_csc_ink_pos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int x_pos                   : 16;      /* [15:0] */
        unsigned int y_pos                   : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_ink_pos;

/* Define the union g1_csc_ink_data */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ink_data                : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_ink_data;

/* Define the union g1_csc_ink_data2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ink_data2               : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_ink_data2;

/* Define the union g1_cvfir_vinfo */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int out_height              : 16;      /* [15:0] */
        unsigned int out_fmt                 : 2;       /* [17:16] */
        unsigned int out_pro                 : 1;       /* [18] */
        unsigned int vzme_ck_gt_en           : 1;       /* [19] */
        unsigned int reserved                : 12;      /* [31:20] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_cvfir_vinfo;

/* Define the union g1_cvfir_vsp */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vratio                  : 16;      /* [15:0] */
        unsigned int reserved0               : 1;       /* [16] */
        unsigned int reserved1               : 8;       /* [24:17] */
        unsigned int cvfir_mode              : 1;       /* [25] */
        unsigned int reserved2               : 1;       /* [26] */
        unsigned int reserved3               : 1;       /* [27] */
        unsigned int cvmid_en                : 1;       /* [28] */
        unsigned int reserved4               : 1;       /* [29] */
        unsigned int cvfir_en                : 1;       /* [30] */
        unsigned int reserved5               : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_cvfir_vsp;

/* Define the union g1_cvfir_voffset */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vchroma_offset          : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_cvfir_voffset;

/* Define the union g1_cvfir_vboffset */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vbchroma_offset         : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_cvfir_vboffset;

/* Define the union g1_cvfir_vcoef0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vccoef02                : 10;      /* [9:0] */
        unsigned int vccoef01                : 10;      /* [19:10] */
        unsigned int vccoef00                : 10;      /* [29:20] */
        unsigned int reserved                : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_cvfir_vcoef0;

/* Define the union g1_cvfir_vcoef1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vccoef11                : 10;      /* [9:0] */
        unsigned int vccoef10                : 10;      /* [19:10] */
        unsigned int vccoef03                : 10;      /* [29:20] */
        unsigned int reserved                : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_cvfir_vcoef1;

/* Define the union g1_cvfir_vcoef2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vccoef13                : 10;      /* [9:0] */
        unsigned int vccoef12                : 10;      /* [19:10] */
        unsigned int reserved                : 12;      /* [31:20] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_cvfir_vcoef2;

/* Define the union g1_hfir_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hfir_en                 : 1;       /* [0] unused */
        unsigned int hfir_mode               : 2;       /* [2:1] */
        unsigned int mid_en                  : 1;       /* [3] */
        unsigned int ck_gt_en                : 1;       /* [4] */
        unsigned int reserved1               : 27;      /* [31:5] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_hfir_ctrl;

/* Define the union g1_hfircoef01 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int coef0                   : 10;      /* [9:0] */
        unsigned int reserved0               : 6;       /* [15:10] */
        unsigned int coef1                   : 10;      /* [25:16] */
        unsigned int reserved1               : 6;       /* [31:26] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_hfircoef01;

/* Define the union g1_hfircoef23 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int coef2                   : 10;      /* [9:0] */
        unsigned int reserved0               : 6;       /* [15:10] */
        unsigned int coef3                   : 10;      /* [25:16] */
        unsigned int reserved1               : 6;       /* [31:26] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_hfircoef23;

/* Define the union g1_hfircoef45 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int coef4                   : 10;      /* [9:0] */
        unsigned int reserved0               : 6;       /* [15:10] */
        unsigned int coef5                   : 10;      /* [25:16] */
        unsigned int reserved1               : 6;       /* [31:26] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_hfircoef45;

/* Define the union g1_hfircoef67 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int coef6                   : 10;      /* [9:0] */
        unsigned int reserved0               : 6;       /* [15:10] */
        unsigned int coef7                   : 10;      /* [25:16] */
        unsigned int reserved1               : 6;       /* [31:26] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_hfircoef67;

/* Define the union g1_t2r1_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int t2r1_t2r_en             : 1;       /* [0] */
        unsigned int t2r1_flip_en            : 1;       /* [1] */
        unsigned int t2r1_copy_en            : 1;       /* [2] */
        unsigned int t2r1_delay_en           : 1;       /* [3] */
        unsigned int t2r1_cfg_en             : 1;       /* [4] */
        unsigned int t2r1_mode_sel           : 1;       /* [5] */
        unsigned int reserved0               : 2;       /* [7:6] */
        unsigned int t2r1_tile_wth           : 8;       /* [15:8] */
        unsigned int reserved1               : 15;      /* [30:16] */
        unsigned int t2r1_ck_gt_en           : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_t2r1_ctrl;

/* Define the union g1_t2r1_err_clr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int t2r1_of_err_clr         : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_t2r1_err_clr;

/* Define the union g1_t2r1_err_sta */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int t2r1_error              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_t2r1_err_sta;

/* Define the union g1_t2r1_debug1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int t2r1_debug1             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_t2r1_debug1;

/* Define the union g1_t2r1_debug2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int t2r1_debug2             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_t2r1_debug2;

/* Define the union g1_t2r1_debug3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int t2r1_debug3             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_t2r1_debug3;

/* Define the union g1_t2r1_debug4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int t2r1_debug4             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_t2r1_debug4;

/* Define the union g1_t2r1_wh */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int t2r1_width              : 16;      /* [15:0] */
        unsigned int t2r1_height             : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_t2r1_wh;

/* Define the union g1_crop1_wh */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int crop1_width             : 16;      /* [15:0] */
        unsigned int crop1_height            : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_crop1_wh;

/* Define the union g1_crop1_xy */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int crop1_x                 : 13;      /* [12:0] */
        unsigned int reserved0               : 3;       /* [15:13] */
        unsigned int crop1_y                 : 13;      /* [28:16] */
        unsigned int reserved1               : 1;       /* [29] */
        unsigned int crop1_cfg_en            : 1;       /* [30] */
        unsigned int crop1_en                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_crop1_xy;

/* Define the union g1_crop1_in_wh */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int crop1_in_width          : 16;      /* [15:0] */
        unsigned int crop1_in_height         : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_crop1_in_wh;

/* Define the union dhd_glob_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mir_en                  : 1;       /* [0] */
        unsigned int rgb_pos                 : 2;       /* [2:1] */
        unsigned int cmp_data_order          : 1;       /* [3] */
        unsigned int pack_en                 : 1;       /* [4] */
        unsigned int ice_en                  : 1;       /* [5] */
        unsigned int wbc_bind_mode           : 2;       /* [7:6] */
        unsigned int i_dfx_en                : 1;       /* [8] */
        unsigned int i_bit_swap_en           : 1;       /* [9] */
        unsigned int i_byte_swap_en          : 1;       /* [10] */
        unsigned int i_self_mode_en          : 1;       /* [11] */
        unsigned int reserved0               : 16;      /* [27:12] */
        unsigned int rupd_field              : 1;       /* [28] */
        unsigned int rgup_mode               : 1;       /* [29] */
        unsigned int reserved1               : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_glob_ctrl;

/* Define the union dhd_htc_online_cmp0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int bypass_en               : 1;       /* [0] */
        unsigned int pix_format              : 3;       /* [3:1] */
        unsigned int bit_depth               : 1;       /* [4] */
        unsigned int htc_cmp_ck_gt_en        : 1;       /* [5] */
        unsigned int dct_skip                : 1;       /* [6] */
        unsigned int reserved0               : 1;       /* [7] */
        unsigned int des_block_size          : 6;       /* [13:8] */
        unsigned int reserved1               : 2;       /* [15:14] */
        unsigned int csc_metria_en           : 8;       /* [23:16] */
        unsigned int csc_skip                : 1;       /* [24] */
        unsigned int iter_protect_csc_en     : 1;       /* [25] */
        unsigned int reserved2               : 6;       /* [31:26] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_htc_online_cmp0;

/* Define the union dhd_htc_online_cmp1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int flat_thresh             : 8;       /* [7:0] */
        unsigned int flat_protect_en         : 1;       /* [8] */
        unsigned int reserved                : 23;      /* [31:9] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_htc_online_cmp1;

/* Define the union dhd_htc_online_cmp_bs_idx */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int i_bs_ink_idx            : 18;      /* [17:0] */
        unsigned int reserved                : 14;      /* [31:18] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_htc_online_cmp_bs_idx;

/* Define the union dhd_htc_online_cmp_crc */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int bs_crc_result           : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_htc_online_cmp_crc;

/* Define the union dhd_htc_online_cmp_chksum */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int bs_chksum_result        : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_htc_online_cmp_chksum;

/* Define the union dhd_htc_online_cmp_ink_data_l */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int bs_ink_data_l           : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_htc_online_cmp_ink_data_l;

/* Define the union dhd_htc_online_cmp_ink_data_h */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int bs_ink_data_h           : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_htc_online_cmp_ink_data_h;

/* Define the union dhd_htc_online_cmp_sta */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int bs_size_less            : 1;       /* [0] */
        unsigned int bs_data_vld_err         : 1;       /* [1] */
        unsigned int reserved                : 30;      /* [31:2] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_htc_online_cmp_sta;

/* Define the union dhd_htc_online_cmp_debug0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int htc_online_cmp_debug0   : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_htc_online_cmp_debug0;

/* Define the union dhd_htc_online_cmp_debug1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int htc_online_cmp_debug1   : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_htc_online_cmp_debug1;

/* Define the union dhd_dither_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither_tap_mode         : 2;       /* [1:0] */
        unsigned int dither_domain_mode      : 1;       /* [2] */
        unsigned int dither_round            : 1;       /* [3] */
        unsigned int dither_mode             : 1;       /* [4] */
        unsigned int dither_en               : 1;       /* [5] */
        unsigned int dither_round_unlim      : 1;       /* [6] */
        unsigned int i_data_width_dither     : 3;       /* [9:7] */
        unsigned int o_data_width_dither     : 3;       /* [12:10] */
        unsigned int reserved                : 19;      /* [31:13] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_dither_ctrl;

/* Define the union dhd_dither_sed_y0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither_sed_y0           : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_dither_sed_y0;

/* Define the union dhd_dither_sed_u0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither_sed_u0           : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_dither_sed_u0;

/* Define the union dhd_dither_sed_v0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither_sed_v0           : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_dither_sed_v0;

/* Define the union dhd_dither_sed_w0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither_sed_w0           : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_dither_sed_w0;

/* Define the union dhd_dither_sed_y1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither_sed_y1           : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_dither_sed_y1;

/* Define the union dhd_dither_sed_u1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither_sed_u1           : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_dither_sed_u1;

/* Define the union dhd_dither_sed_v1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither_sed_v1           : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_dither_sed_v1;

/* Define the union dhd_dither_sed_w1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither_sed_w1           : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_dither_sed_w1;

/* Define the union dhd_dither_sed_y2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither_sed_y2           : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_dither_sed_y2;

/* Define the union dhd_dither_sed_u2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither_sed_u2           : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_dither_sed_u2;

/* Define the union dhd_dither_sed_v2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither_sed_v2           : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_dither_sed_v2;

/* Define the union dhd_dither_sed_w2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither_sed_w2           : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_dither_sed_w2;

/* Define the union dhd_dither_sed_y3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither_sed_y3           : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_dither_sed_y3;

/* Define the union dhd_dither_sed_u3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither_sed_u3           : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_dither_sed_u3;

/* Define the union dhd_dither_sed_v3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither_sed_v3           : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_dither_sed_v3;

/* Define the union dhd_dither_sed_w3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither_sed_w3           : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_dither_sed_w3;

/* Define the union dhd_dither_thr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither_thr_min          : 16;      /* [15:0] */
        unsigned int dither_thr_max          : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_dither_thr;

/* Define the union rnd_crop_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int rnd_crop_en             : 1;       /* [0] */
        unsigned int rnd_crop_ck_gt_en       : 1;       /* [1] */
        unsigned int reserved                : 30;      /* [31:2] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_rnd_crop_cfg;

/* Define the union rnd_crop_lut_rd_en */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int rnd_crop_lut_rd_en      : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_rnd_crop_lut_rd_en;

/* Define the union rnd_crop_lut_rdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int rnd_crop_lut_rdata      : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_rnd_crop_lut_rdata;

/* Define the union dhd_add_pix_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int add_pix_en              : 1;       /* [0] */
        unsigned int add_pix_cfg_en          : 1;       /* [1] */
        unsigned int reserved                : 30;      /* [31:2] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_add_pix_ctrl;

/* Define the union dhd_add_pix_small */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int small_height            : 16;      /* [15:0] */
        unsigned int small_width             : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_add_pix_small;

/* Define the union dhd_add_pix_big */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int big_height              : 16;      /* [15:0] */
        unsigned int big_width               : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_add_pix_big;

/* Define the union dhd_add_pix_rect */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int rect_y                  : 16;      /* [15:0] */
        unsigned int rect_x                  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_add_pix_rect;

/* Define the union dhd_add_pix_tile */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tile_height             : 2;       /* [1:0] */
        unsigned int tile_width              : 4;       /* [5:2] */
        unsigned int reserved                : 26;      /* [31:6] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_add_pix_tile;

/* Define the union dhd_t2r1_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int t2r_t2r_en              : 1;       /* [0] */
        unsigned int t2r_flip_en             : 1;       /* [1] */
        unsigned int t2r_copy_en             : 1;       /* [2] */
        unsigned int t2r_delay_en            : 1;       /* [3] */
        unsigned int t2r_cfg_en              : 1;       /* [4] */
        unsigned int t2r_mode_sel            : 1;       /* [5] */
        unsigned int reserved0               : 2;       /* [7:6] */
        unsigned int tile_wth                : 8;       /* [15:8] */
        unsigned int reserved1               : 15;      /* [30:16] */
        unsigned int t2r_ck_gt_en            : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_t2r1_ctrl;

/* Define the union dhd_t2r1_err_clr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int t2r_of_err_clr          : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_t2r1_err_clr;

/* Define the union dhd_t2r1_err_sta */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int t2r_error               : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_t2r1_err_sta;

/* Define the union dhd_t2r1_debug1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int t2r_debug1              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_t2r1_debug1;

/* Define the union dhd_t2r1_debug2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int t2r_debug2              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_t2r1_debug2;

/* Define the union dhd_t2r1_debug3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int t2r_debug3              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_t2r1_debug3;

/* Define the union dhd_t2r1_debug4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int t2r_debug4              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_t2r1_debug4;

/* Define the union dhd_t2r1_wh */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int t2r_width               : 16;      /* [15:0] */
        unsigned int t2r_height              : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_t2r1_wh;

/* Define the union vhdaccctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int acc_en                  : 1;       /* [0] */
        unsigned int acc_ck_gt_en            : 1;       /* [1] */
        unsigned int reserved                : 30;      /* [31:2] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccctrl;

/* Define the union vhdacchpos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int acc_hstart              : 12;      /* [11:0] */
        unsigned int reserved0               : 4;       /* [15:12] */
        unsigned int acc_hend                : 12;      /* [27:16] */
        unsigned int reserved1               : 4;       /* [31:28] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdacchpos;

/* Define the union vhdaccvpos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int acc_vstart              : 12;      /* [11:0] */
        unsigned int reserved0               : 4;       /* [15:12] */
        unsigned int acc_vend                : 12;      /* [27:16] */
        unsigned int reserved1               : 4;       /* [31:28] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccvpos;

/* Define the union vhdaccdemo */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int demo_en                 : 1;       /* [0] */
        unsigned int demo_mode               : 1;       /* [1] */
        unsigned int reserved0               : 2;       /* [3:2] */
        unsigned int demo_split_point        : 12;      /* [15:4] */
        unsigned int reserved1               : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccdemo;

/* Define the union vhdacclut1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int table_data0             : 10;      /* [9:0] */
        unsigned int table_data1             : 10;      /* [19:10] */
        unsigned int table_data2             : 10;      /* [29:20] */
        unsigned int reserved                : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdacclut1;

/* Define the union vhdacclut2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int table_data3             : 10;      /* [9:0] */
        unsigned int table_data4             : 10;      /* [19:10] */
        unsigned int table_data5             : 10;      /* [29:20] */
        unsigned int reserved                : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdacclut2;

/* Define the union vhdacclut3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int table_data6             : 10;      /* [9:0] */
        unsigned int table_data7             : 10;      /* [19:10] */
        unsigned int table_data8             : 10;      /* [29:20] */
        unsigned int reserved                : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdacclut3;

/* Define the union vhdacclut4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int table_data9             : 10;      /* [9:0] */
        unsigned int table_data10            : 10;      /* [19:10] */
        unsigned int table_data11            : 10;      /* [29:20] */
        unsigned int reserved                : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdacclut4;

/* Define the union vhdacclut5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int table_data12            : 10;      /* [9:0] */
        unsigned int table_data13            : 10;      /* [19:10] */
        unsigned int table_data14            : 10;      /* [29:20] */
        unsigned int reserved                : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdacclut5;

/* Define the union vhdacclut6 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int table_data15            : 10;      /* [9:0] */
        unsigned int table_data16            : 10;      /* [19:10] */
        unsigned int table_data17            : 10;      /* [29:20] */
        unsigned int reserved                : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdacclut6;

/* Define the union vhdacclut7 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int table_data18            : 10;      /* [9:0] */
        unsigned int table_data19            : 10;      /* [19:10] */
        unsigned int table_data20            : 10;      /* [29:20] */
        unsigned int reserved                : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdacclut7;

/* Define the union vhdacclut8 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int table_data21            : 10;      /* [9:0] */
        unsigned int table_data22            : 10;      /* [19:10] */
        unsigned int table_data23            : 10;      /* [29:20] */
        unsigned int reserved                : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdacclut8;

/* Define the union vhdacclut9 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int table_data24            : 10;      /* [9:0] */
        unsigned int table_data25            : 10;      /* [19:10] */
        unsigned int table_data26            : 10;      /* [29:20] */
        unsigned int reserved                : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdacclut9;

/* Define the union vhdacclut10 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int table_data27            : 10;      /* [9:0] */
        unsigned int table_data28            : 10;      /* [19:10] */
        unsigned int table_data29            : 10;      /* [29:20] */
        unsigned int reserved                : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdacclut10;

/* Define the union vhdacclut11 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int table_data30            : 10;      /* [9:0] */
        unsigned int table_data31            : 10;      /* [19:10] */
        unsigned int table_data32            : 10;      /* [29:20] */
        unsigned int reserved                : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdacclut11;

/* Define the union vhdaccstatctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int acc_stat_en             : 1;       /* [0] */
        unsigned int acc_stat_ck_gt_en       : 1;       /* [1] */
        unsigned int reserved                : 30;      /* [31:2] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccstatctrl;

/* Define the union vhdaccstathpos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int acc_stat_hstart         : 12;      /* [11:0] */
        unsigned int reserved0               : 4;       /* [15:12] */
        unsigned int acc_stat_hend           : 12;      /* [27:16] */
        unsigned int reserved1               : 4;       /* [31:28] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccstathpos;

/* Define the union vhdaccstatvpos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int acc_stat_vstart         : 12;      /* [11:0] */
        unsigned int reserved0               : 4;       /* [15:12] */
        unsigned int acc_stat_vend           : 12;      /* [27:16] */
        unsigned int reserved1               : 4;       /* [31:28] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccstatvpos;

/* Define the union vhdaccshiftd */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int acc_stat_shiftd1        : 5;       /* [4:0] */
        unsigned int acc_stat_shiftd2        : 5;       /* [9:5] */
        unsigned int reserved0               : 2;       /* [11:10] */
        unsigned int acc_stat_shifthend      : 10;      /* [21:12] */
        unsigned int reserved1               : 10;      /* [31:22] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccshiftd;

/* Define the union vhdaccdnsam */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int acc_stat_hdnsample      : 5;       /* [4:0] */
        unsigned int acc_stat_vdnsample      : 5;       /* [9:5] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccdnsam;

/* Define the union vhdaccsatcfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int acc_stat_lowsat_bin     : 5;       /* [4:0] */
        unsigned int reserved                : 27;      /* [31:5] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccsatcfg;

/* Define the union vhdaccydiffd1sum */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int acc_ydiffd1_sum         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccydiffd1sum;

/* Define the union vhdaccydiffd2sum */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int acc_ydiffd2_sum         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccydiffd2sum;

/* Define the union vhdaccysum */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int acc_y_sum               : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccysum;

/* Define the union vhdaccsatsum */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int acc_sat_sum             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccsatsum;

/* Define the union vhdaccsathistsum */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int acc_sat_hist_sum        : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccsathistsum;

/* Define the union vhdaccsathistcum */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int acc_sat_hist_cum        : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccsathistcum;

/* Define the union vhdaccyhist_0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ycnt32_data0            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccyhist_0;

/* Define the union vhdaccyhist_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ycnt32_data1            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccyhist_1;

/* Define the union vhdaccyhist_2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ycnt32_data2            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccyhist_2;

/* Define the union vhdaccyhist_3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ycnt32_data3            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccyhist_3;

/* Define the union vhdaccyhist_4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ycnt32_data4            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccyhist_4;

/* Define the union vhdaccyhist_5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ycnt32_data5            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccyhist_5;

/* Define the union vhdaccyhist_6 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ycnt32_data6            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccyhist_6;

/* Define the union vhdaccyhist_7 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ycnt32_data7            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccyhist_7;

/* Define the union vhdaccyhist_8 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ycnt32_data8            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccyhist_8;

/* Define the union vhdaccyhist_9 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ycnt32_data9            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccyhist_9;

/* Define the union vhdaccyhist_10 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ycnt32_data10           : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccyhist_10;

/* Define the union vhdaccyhist_11 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ycnt32_data11           : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccyhist_11;

/* Define the union vhdaccyhist_12 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ycnt32_data12           : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccyhist_12;

/* Define the union vhdaccyhist_13 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ycnt32_data13           : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccyhist_13;

/* Define the union vhdaccyhist_14 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ycnt32_data14           : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccyhist_14;

/* Define the union vhdaccyhist_15 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ycnt32_data15           : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccyhist_15;

/* Define the union vhdaccyhist_16 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ycnt32_data16           : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccyhist_16;

/* Define the union vhdaccyhist_17 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ycnt32_data17           : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccyhist_17;

/* Define the union vhdaccyhist_18 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ycnt32_data18           : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccyhist_18;

/* Define the union vhdaccyhist_19 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ycnt32_data19           : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccyhist_19;

/* Define the union vhdaccsathist_0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int satcnt32_data0          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccsathist_0;

/* Define the union vhdaccsathist_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int satcnt32_data1          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccsathist_1;

/* Define the union vhdaccsathist_2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int satcnt32_data2          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccsathist_2;

/* Define the union vhdaccsathist_3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int satcnt32_data3          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccsathist_3;

/* Define the union vhdaccsathist_4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int satcnt32_data4          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccsathist_4;

/* Define the union vhdaccsathist_5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int satcnt32_data5          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccsathist_5;

/* Define the union vhdaccsathist_6 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int satcnt32_data6          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccsathist_6;

/* Define the union vhdaccsathist_7 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int satcnt32_data7          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccsathist_7;

/* Define the union vhdaccsathist_8 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int satcnt32_data8          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccsathist_8;

/* Define the union vhdaccsathist_9 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int satcnt32_data9          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccsathist_9;

/* Define the union vhdaccsathist_10 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int satcnt32_data10         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccsathist_10;

/* Define the union vhdaccsathist_11 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int satcnt32_data11         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccsathist_11;

/* Define the union vhdaccsathist_12 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int satcnt32_data12         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccsathist_12;

/* Define the union vhdaccsathist_13 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int satcnt32_data13         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccsathist_13;

/* Define the union vhdaccsathist_14 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int satcnt32_data14         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccsathist_14;

/* Define the union vhdaccsathist_15 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int satcnt32_data15         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccsathist_15;

/* Define the union vhdaccsathist_16 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int satcnt32_data16         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccsathist_16;

/* Define the union vhdaccsathist_17 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int satcnt32_data17         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccsathist_17;

/* Define the union vhdaccsathist_18 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int satcnt32_data18         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccsathist_18;

/* Define the union vhdaccsathist_19 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int satcnt32_data19         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdaccsathist_19;

/* Define the union dhd0_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int regup                   : 1;       /* [0] */
        unsigned int disp_mode               : 3;       /* [3:1] */
        unsigned int iop                     : 1;       /* [4] */
        unsigned int intf_ivs                : 1;       /* [5] */
        unsigned int intf_ihs                : 1;       /* [6] */
        unsigned int intf_idv                : 1;       /* [7] */
        unsigned int reserved0               : 3;       /* [10:8] */
        unsigned int uf_offline_en           : 1;       /* [11] */
        unsigned int reserved1               : 1;       /* [12] */
        unsigned int intf_sel                : 1;       /* [13] */
        unsigned int reserved2               : 4;       /* [17:14] */
        unsigned int cbar_mode               : 1;       /* [18] */
        unsigned int reserved3               : 10;      /* [28:19] */
        unsigned int cbar_sel                : 1;       /* [29] */
        unsigned int cbar_en                 : 1;       /* [30] */
        unsigned int intf_en                 : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_ctrl;

/* Define the union dhd0_vsync1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vact                    : 16;      /* [15:0] */
        unsigned int vbb                     : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_vsync1;

/* Define the union dhd0_vsync2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vfb                     : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_vsync2;

/* Define the union dhd0_hsync1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hact                    : 16;      /* [15:0] */
        unsigned int hbb                     : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_hsync1;

/* Define the union dhd0_hsync2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hfb                     : 16;      /* [15:0] */
        unsigned int hmid                    : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_hsync2;

/* Define the union dhd0_vplus1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int bvact                   : 16;      /* [15:0] */
        unsigned int bvbb                    : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_vplus1;

/* Define the union dhd0_vplus2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int bvfb                    : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_vplus2;

/* Define the union dhd0_pwr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hpw                     : 16;      /* [15:0] */
        unsigned int vpw                     : 8;       /* [23:16] */
        unsigned int reserved0               : 3;       /* [26:24] */
        unsigned int multichn_en             : 2;       /* [28:27] */
        unsigned int reserved1               : 3;       /* [31:29] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_pwr;

/* Define the union dhd0_vtthd3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vtmgthd3                : 13;      /* [12:0] */
        unsigned int reserved0               : 2;       /* [14:13] */
        unsigned int thd3_mode               : 1;       /* [15] */
        unsigned int vtmgthd4                : 13;      /* [28:16] */
        unsigned int reserved1               : 2;       /* [30:29] */
        unsigned int thd4_mode               : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_vtthd3;

/* Define the union dhd0_vtthd */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vtmgthd1                : 13;      /* [12:0] */
        unsigned int reserved0               : 2;       /* [14:13] */
        unsigned int thd1_mode               : 1;       /* [15] */
        unsigned int vtmgthd2                : 13;      /* [28:16] */
        unsigned int reserved1               : 2;       /* [30:29] */
        unsigned int thd2_mode               : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_vtthd;

/* Define the union dhd0_parathd */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int para_thd                : 8;       /* [7:0] */
        unsigned int reserved                : 24;      /* [31:8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_parathd;

/* Define the union dhd0_te_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved0               : 19;      /* [18:0] */
        unsigned int vsync_te_mode           : 2;       /* [20:19] */
        unsigned int vsync_te_from           : 1;       /* [21] */
        unsigned int reserved1               : 10;      /* [31:22] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_te_ctrl;

/* Define the union dhd0_start_pos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int start_pos               : 8;       /* [7:0] */
        unsigned int timing_start_pos        : 8;       /* [15:8] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_start_pos;

/* Define the union dhd0_clk_dv_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int intf_clk_mux            : 1;       /* [0] */
        unsigned int intf_dv_mux             : 1;       /* [1] */
        unsigned int no_active_area_pos      : 16;      /* [17:2] */
        unsigned int reserved                : 14;      /* [31:18] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_clk_dv_ctrl;

/* Define the union dhd0_rgb_fix_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int fix_b                   : 10;      /* [9:0] */
        unsigned int fix_g                   : 10;      /* [19:10] */
        unsigned int fix_r                   : 10;      /* [29:20] */
        unsigned int rgb_fix_mux             : 1;       /* [30] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_rgb_fix_ctrl;

/* Define the union dhd0_intf_chksum_y */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int check_sum0              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_intf_chksum_y;

/* Define the union dhd0_intf_chksum_u */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int check_sum1              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_intf_chksum_u;

/* Define the union dhd0_intf_chksum_v */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int check_sum2              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_intf_chksum_v;

/* Define the union dhd0_intf1_chksum_y */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int check_sum3              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_intf1_chksum_y;

/* Define the union dhd0_intf1_chksum_u */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int check_sum4              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_intf1_chksum_u;

/* Define the union dhd0_intf1_chksum_v */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int check_sum5              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_intf1_chksum_v;

/* Define the union dhd0_intf_chksum_high1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int r0_sum_high             : 8;       /* [7:0] */
        unsigned int g0_sum_high             : 8;       /* [15:8] */
        unsigned int b0_sum_high             : 8;       /* [23:16] */
        unsigned int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_intf_chksum_high1;

/* Define the union dhd0_intf_chksum_high2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int r1_sum_high             : 8;       /* [7:0] */
        unsigned int g1_sum_high             : 8;       /* [15:8] */
        unsigned int b1_sum_high             : 8;       /* [23:16] */
        unsigned int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_intf_chksum_high2;

/* Define the union dhd0_intf_cmdcfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int te_dly_cfg              : 16;      /* [15:0] */
        unsigned int reserved                : 15;      /* [30:16] */
        unsigned int tde_frm_rdy_mode        : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_intf_cmdcfg;

/* Define the union dhd0_tde_frm_rdy */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tde_frm_rdy             : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_tde_frm_rdy;

/* Define the union dhd0_te_timeout */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int te_timeout_thd          : 31;      /* [30:0] */
        unsigned int te_timeout_en           : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_te_timeout;

/* Define the union dhd0_state */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vback_blank             : 1;       /* [0] */
        unsigned int vblank                  : 1;       /* [1] */
        unsigned int bottom_field            : 1;       /* [2] */
        unsigned int vcnt                    : 13;      /* [15:3] */
        unsigned int count_int               : 8;       /* [23:16] */
        unsigned int dhd_even                : 1;       /* [24] */
        unsigned int mipi_rdy_state          : 1;       /* [25] */
        unsigned int reserved                : 6;       /* [31:26] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_state;

/* Define the union dhd0_uf_state */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ud_first_cnt            : 13;      /* [12:0] */
        unsigned int reserved0               : 3;       /* [15:13] */
        unsigned int start_pos_rd            : 8;       /* [23:16] */
        unsigned int reserved1               : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_uf_state;

/* Define the union vo_mux */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mipi_sel                : 4;       /* [3:0] */
        unsigned int lcd_sel                 : 4;       /* [7:4] */
        unsigned int bt_sel                  : 4;       /* [11:8] */
        unsigned int sddate_sel              : 4;       /* [15:12] */
        unsigned int hdmi_sel                : 4;       /* [19:16] */
        unsigned int hdmi1_sel               : 4;       /* [23:20] */
        unsigned int vga_sel                 : 4;       /* [27:24] */
        unsigned int digital_sel             : 4;       /* [31:28] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vo_mux;

/* Define the union vo_mux_sync */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sync_dv                 : 1;       /* [0] */
        unsigned int sync_hsync              : 1;       /* [1] */
        unsigned int sync_vsync              : 1;       /* [2] */
        unsigned int sync_field              : 1;       /* [3] */
        unsigned int reserved                : 27;      /* [30:4] */
        unsigned int sync_test_mode          : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vo_mux_sync;

/* Define the union vo_mux_data */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vomux_data              : 30;      /* [29:0] */
        unsigned int reserved                : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vo_mux_data;

/* Define the union dhd0_vsync_te_state */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vsync_te_start_sta      : 8;       /* [7:0] */
        unsigned int vsync_te_start_sta1     : 8;       /* [15:8] */
        unsigned int vsync_te_end_sta        : 8;       /* [23:16] */
        unsigned int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_vsync_te_state;

/* Define the union dhd0_vsync_te_state1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vsync_te_vfb            : 16;      /* [15:0] */
        unsigned int vsync_te_width          : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_vsync_te_state1;

/* Define the union intf0_sdlp_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sdlp_lp_mode_thd        : 16;      /* [15:0] */
        unsigned int reserved                : 15;      /* [30:16] */
        unsigned int sdlp_lp_mode_en         : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_intf0_sdlp_ctrl;

/* Define the union intf0_sdlp_delay */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sdlp_lp_start_delay     : 16;      /* [15:0] */
        unsigned int sdlp_lp_end_delay       : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_intf0_sdlp_delay;

/* Define the union intf0_sdlp_stop */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int soft_stop_lp_mode       : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_intf0_sdlp_stop;

/* Define the union intf0_sdlp_sta */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mipi_lp_mode_en         : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_intf0_sdlp_sta;

/* Define the union intf_mipi_upd */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int regup_intf              : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_intf_mipi_upd;

/* Define the union intf_mipi_sync_inv */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dv_inv                  : 1;       /* [0] */
        unsigned int hs_inv                  : 1;       /* [1] */
        unsigned int vs_inv                  : 1;       /* [2] */
        unsigned int reserved                : 29;      /* [31:3] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_intf_mipi_sync_inv;

/* Define the union intf_lcd_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved0               : 20;      /* [19:0] */
        unsigned int lcd_format              : 4;       /* [23:20] */
        unsigned int lcd_bit_inv             : 1;       /* [24] */
        unsigned int lcd_comp_order          : 1;       /* [25] */
        unsigned int lcd_serial_perd         : 1;       /* [26] */
        unsigned int dummy_position_sel      : 1;       /* [27] */
        unsigned int reserved1               : 2;       /* [29:28] */
        unsigned int dfir_en                 : 1;       /* [30] */
        unsigned int hdmi_mode               : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_intf_lcd_ctrl;

/* Define the union intf_lcd_upd */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int regup_lcd               : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_intf_lcd_upd;

/* Define the union intf_lcd_sync_inv */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int lcd_dv_inv              : 1;       /* [0] */
        unsigned int lcd_hs_inv              : 1;       /* [1] */
        unsigned int lcd_vs_inv              : 1;       /* [2] */
        unsigned int f_inv                   : 1;       /* [3] */
        unsigned int reserved                : 28;      /* [31:4] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_intf_lcd_sync_inv;

/* Define the union lcd_intf_chksum_y */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int check_sum_lcd0          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_lcd_intf_chksum_y;

/* Define the union lcd_intf_chksum_u */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int check_sum_lcd1          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_lcd_intf_chksum_u;

/* Define the union lcd_intf_chksum_v */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int check_sum_lcd2          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_lcd_intf_chksum_v;

/* Define the union mac_outstanding */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mstr_routstanding       : 4;       /* [3:0] */
        unsigned int mstr_woutstanding       : 4;       /* [7:4] */
        unsigned int reserved                : 24;      /* [31:8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mac_outstanding;

/* Define the union mac_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int split_mode              : 4;       /* [3:0] */
        unsigned int arb_mode                : 4;       /* [7:4] */
        unsigned int mid_enable              : 1;       /* [8] */
        unsigned int reserved0               : 7;       /* [15:9] */
        unsigned int reserved1               : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mac_ctrl;

/* Define the union mac_rchn_prio */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved0               : 9;       /* [8:0] */
        unsigned int g0_prio                 : 1;       /* [9] */
        unsigned int reserved1               : 1;       /* [10] */
        unsigned int g1_prio                 : 1;       /* [11] */
        unsigned int reserved2               : 5;       /* [16:12] */
        unsigned int g3_prio                 : 1;       /* [17] */
        unsigned int reserved3               : 1;       /* [18] */
        unsigned int g2_prio                 : 1;       /* [19] */
        unsigned int reserved4               : 12;      /* [31:20] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mac_rchn_prio;

/* Define the union mac_wchn_prio */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int wbc_prio                : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mac_wchn_prio;

/* Define the union mac_bus_err_clr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int bus_error_clr           : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mac_bus_err_clr;

/* Define the union mac_bus_err */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mst_r_error             : 1;       /* [0] */
        unsigned int mst_w_error             : 1;       /* [1] */
        unsigned int reserved                : 30;      /* [31:2] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mac_bus_err;

/* Define the union mac_src0_status0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src0_status0            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mac_src0_status0;

/* Define the union mac_src0_status1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src0_status1            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mac_src0_status1;

/* Define the union mac_debug_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int axi_det_enable          : 1;       /* [0] */
        unsigned int reserved0               : 3;       /* [3:1] */
        unsigned int fifo_det_mode           : 4;       /* [7:4] */
        unsigned int reserved1               : 24;      /* [31:8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mac_debug_ctrl;

/* Define the union mac_debug_clr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int axi_det_clr             : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mac_debug_clr;

/* Define the union mac_debug_info */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mac_debug_info          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mac_debug_info;

/* Define the union mac_rd_info */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mst_rd_info             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mac_rd_info;

/* Define the union mac_wr_info */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mst_wr_info             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mac_wr_info;

/* Define the union mac_det_latency0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mst_rd_max              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mac_det_latency0;

/* Define the union mac_det_latency1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mst_rd_aver             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mac_det_latency1;

/* Define the union mac_det_latency2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mst_rd_cmd              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mac_det_latency2;

/* Define the union mac_det_latency3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mst_wr_max              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mac_det_latency3;

/* Define the union mac_det_latency4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mst_wr_aver             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mac_det_latency4;

/* Define the union mac_det_latency5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mst_wr_cmd              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mac_det_latency5;

/* Define the union gfx_read_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int read_mode               : 2;       /* [1:0] */
        unsigned int reserved0               : 2;       /* [3:2] */
        unsigned int draw_mode               : 2;       /* [5:4] */
        unsigned int reserved1               : 2;       /* [7:6] */
        unsigned int flip_en                 : 1;       /* [8] */
        unsigned int reserved2               : 1;       /* [9] */
        unsigned int mute_en                 : 1;       /* [10] */
        unsigned int mute_req_en             : 1;       /* [11] */
        unsigned int fdr_ck_gt_en            : 1;       /* [12] */
        unsigned int reserved3               : 1;       /* [13] */
        unsigned int reso_cfg_en             : 1;       /* [14] */
        unsigned int reserved4               : 17;      /* [31:15] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_read_ctrl;

/* Define the union gfx_mac_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int req_ctrl                : 2;       /* [1:0] */
        unsigned int req_len                 : 2;       /* [3:2] */
        unsigned int reserved                : 28;      /* [31:4] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_mac_ctrl;

/* Define the union gfx_out_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int palpha_range            : 1;       /* [0] */
        unsigned int palpha_en               : 1;       /* [1] */
        unsigned int reserved0               : 2;       /* [3:2] */
        unsigned int key_mode                : 1;       /* [4] */
        unsigned int key_en                  : 1;       /* [5] */
        unsigned int reserved1               : 2;       /* [7:6] */
        unsigned int bitext                  : 2;       /* [9:8] */
        unsigned int premulti_en             : 1;       /* [10] */
        unsigned int testpattern_en          : 1;       /* [11] */
        unsigned int mapping_perf_mode       : 1;       /* [12] */
        unsigned int reserved2               : 19;      /* [31:13] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_out_ctrl;

/* Define the union gfx_mute_alpha */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mute_alpha              : 8;       /* [7:0] */
        unsigned int reserved                : 24;      /* [31:8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_mute_alpha;

/* Define the union gfx_mute_bk */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mute_cr                 : 10;      /* [9:0] */
        unsigned int mute_cb                 : 10;      /* [19:10] */
        unsigned int mute_y                  : 10;      /* [29:20] */
        unsigned int reserved                : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_mute_bk;

/* Define the union gfx_1555_alpha */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int alpha_0                 : 8;       /* [7:0] */
        unsigned int alpha_1                 : 8;       /* [15:8] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_1555_alpha;

/* Define the union gfx_src_info */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ifmt                    : 8;       /* [7:0] */
        unsigned int reserved0               : 8;       /* [15:8] */
        unsigned int disp_mode               : 4;       /* [19:16] */
        unsigned int reserved1               : 12;      /* [31:20] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_src_info;

/* Define the union gfx_src_reso */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src_w                   : 16;      /* [15:0] */
        unsigned int src_h                   : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_src_reso;

/* Define the union gfx_src_crop */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src_crop_x              : 16;      /* [15:0] */
        unsigned int src_crop_y              : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_src_crop;

/* Define the union gfx_ireso */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ireso_w                 : 16;      /* [15:0] */
        unsigned int ireso_h                 : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_ireso;

/* Define the union gfx_addr_h */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gfx_addr_h              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_addr_h;

/* Define the union gfx_addr_l */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gfx_addr_l              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_addr_l;

/* Define the union gfx_stride */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int surface_stride          : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_stride;

/* Define the union gfx_addr_h_c */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gfx_addr_h_c            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_addr_h_c;

/* Define the union gfx_addr_l_c */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gfx_addr_l_c            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_addr_l_c;

/* Define the union gfx_stride_c */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int surface_stride_c        : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_stride_c;

/* Define the union gfx_dcmp_addr_h */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dcmp_addr_h             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_dcmp_addr_h;

/* Define the union gfx_dcmp_addr_l */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dcmp_addr_l             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_dcmp_addr_l;

/* Define the union gfx_cmp_reso_debug0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved                : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_cmp_reso_debug0;

/* Define the union gfx_cmp_reso_debug1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved                : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_cmp_reso_debug1;

/* Define the union gfx_cmp_reso_debug2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved                : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_cmp_reso_debug2;

/* Define the union gfx_testpat_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tp_speed                : 10;      /* [9:0] */
        unsigned int reserved0               : 2;       /* [11:10] */
        unsigned int tp_line_w               : 1;       /* [12] */
        unsigned int tp_color_mode           : 1;       /* [13] */
        unsigned int reserved1               : 2;       /* [15:14] */
        unsigned int tp_mode                 : 2;       /* [17:16] */
        unsigned int reserved2               : 14;      /* [31:18] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_testpat_cfg;

/* Define the union gfx_testpat_seed */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tp_seed                 : 30;      /* [29:0] */
        unsigned int reserved                : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_testpat_seed;

/* Define the union gfx_dcmp_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ice_en                  : 1;       /* [0] */
        unsigned int dcmp_mode               : 1;       /* [1] */
        unsigned int dcmp_fmt                : 2;       /* [3:2] */
        unsigned int alpha_bypass            : 1;       /* [4] */
        unsigned int reserved                : 27;      /* [31:5] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_dcmp_ctrl;

/* Define the union gfx_debug_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int checksum_en             : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_debug_ctrl;

/* Define the union gfx_htc_online_dcmp0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pix_format              : 3;       /* [2:0] */
        unsigned int bit_depth               : 1;       /* [3] */
        unsigned int block_size              : 6;       /* [9:4] */
        unsigned int dcmp_mpde               : 1;       /* [10] */
        unsigned int round_offset            : 16;      /* [26:11] */
        unsigned int reserved                : 5;       /* [31:27] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_htc_online_dcmp0;

/* Define the union gfx_htc_online_dcmp1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_metric_enable       : 8;       /* [7:0] */
        unsigned int transform_skip          : 1;       /* [8] */
        unsigned int iter_protect_flag       : 1;       /* [9] */
        unsigned int flat_protect_en         : 1;       /* [10] */
        unsigned int flat_protect_withqs_en  : 1;       /* [11] */
        unsigned int flat_protect_clip_en    : 1;       /* [12] */
        unsigned int csc_skip                : 1;       /* [13] */
        unsigned int flat_protect_round_en   : 1;       /* [14] */
        unsigned int reserved                : 17;      /* [31:15] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_htc_online_dcmp1;

/* Define the union gfx_in_ar_checksum0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ar_checksum0            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_in_ar_checksum0;

/* Define the union gfx_in_ar_checksum1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ar_checksum1            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_in_ar_checksum1;

/* Define the union gfx_in_gb_checksum0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gb_checksum0            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_in_gb_checksum0;

/* Define the union gfx_in_gb_checksum1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gb_checksum1            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_in_gb_checksum1;

/* Define the union gfx_ckey_max */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int key_b_max               : 8;       /* [7:0] */
        unsigned int key_g_max               : 8;       /* [15:8] */
        unsigned int key_r_max               : 8;       /* [23:16] */
        unsigned int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_ckey_max;

/* Define the union gfx_ckey_min */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int key_b_min               : 8;       /* [7:0] */
        unsigned int key_g_min               : 8;       /* [15:8] */
        unsigned int key_r_min               : 8;       /* [23:16] */
        unsigned int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_ckey_min;

/* Define the union gfx_ckey_mask */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int key_b_msk               : 8;       /* [7:0] */
        unsigned int key_g_msk               : 8;       /* [15:8] */
        unsigned int key_r_msk               : 8;       /* [23:16] */
        unsigned int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_ckey_mask;

/* Define the union gfx_htc_online_dcmp_debug0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int htc_dcmp_debug0         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_htc_online_dcmp_debug0;

/* Define the union gfx_htc_online_dcmp_debug1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int htc_dcmp_debug1         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_htc_online_dcmp_debug1;

/* Define the union gfx_htc_online_dcmp_debug2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int htc_dcmp_debug2         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_htc_online_dcmp_debug2;

/* Define the union gfx_htc_online_dcmp_debug3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int htc_dcmp_debug3         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_htc_online_dcmp_debug3;

/* Define the union g1_gfx_read_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int read_mode               : 2;       /* [1:0] */
        unsigned int reserved0               : 1;       /* [2] */
        unsigned int chm_copy_en             : 1;       /* [3] */
        unsigned int draw_mode               : 2;       /* [5:4] */
        unsigned int reserved1               : 2;       /* [7:6] */
        unsigned int flip_en                 : 1;       /* [8] */
        unsigned int reserved2               : 1;       /* [9] */
        unsigned int mute_en                 : 1;       /* [10] */
        unsigned int mute_req_en             : 1;       /* [11] */
        unsigned int fdr_ck_gt_en            : 1;       /* [12] */
        unsigned int reserved3               : 1;       /* [13] */
        unsigned int reso_cfg_en             : 1;       /* [14] */
        unsigned int reserved4               : 17;      /* [31:15] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_read_ctrl;

/* Define the union g1_gfx_mac_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int req_ctrl                : 2;       /* [1:0] */
        unsigned int req_len                 : 2;       /* [3:2] */
        unsigned int reserved                : 28;      /* [31:4] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_mac_ctrl;

/* Define the union g1_gfx_out_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int palpha_range            : 1;       /* [0] */
        unsigned int palpha_en               : 1;       /* [1] */
        unsigned int reserved0               : 2;       /* [3:2] */
        unsigned int key_mode                : 1;       /* [4] */
        unsigned int key_en                  : 1;       /* [5] */
        unsigned int reserved1               : 2;       /* [7:6] */
        unsigned int bitext                  : 2;       /* [9:8] */
        unsigned int premulti_en             : 1;       /* [10] */
        unsigned int testpattern_en          : 1;       /* [11] */
        unsigned int mapping_perf_mode       : 1;       /* [12] */
        unsigned int reserved2               : 19;      /* [31:13] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_out_ctrl;

/* Define the union g1_gfx_mute_alpha */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mute_alpha              : 8;       /* [7:0] */
        unsigned int reserved                : 24;      /* [31:8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_mute_alpha;

/* Define the union g1_gfx_mute_bk */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mute_cr                 : 10;      /* [9:0] */
        unsigned int mute_cb                 : 10;      /* [19:10] */
        unsigned int mute_y                  : 10;      /* [29:20] */
        unsigned int reserved                : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_mute_bk;

/* Define the union g1_gfx_1555_alpha */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int alpha_0                 : 8;       /* [7:0] */
        unsigned int alpha_1                 : 8;       /* [15:8] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_1555_alpha;

/* Define the union g1_gfx_src_info */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ifmt                    : 8;       /* [7:0] */
        unsigned int reserved0               : 8;       /* [15:8] */
        unsigned int disp_mode               : 4;       /* [19:16] */
        unsigned int reserved1               : 12;      /* [31:20] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_src_info;

/* Define the union g1_gfx_src_reso */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src_w                   : 16;      /* [15:0] */
        unsigned int src_h                   : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_src_reso;

/* Define the union g1_gfx_src_crop */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src_crop_x              : 16;      /* [15:0] */
        unsigned int src_crop_y              : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_src_crop;

/* Define the union g1_gfx_ireso */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ireso_w                 : 16;      /* [15:0] */
        unsigned int ireso_h                 : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_ireso;

/* Define the union g1_gfx_addr_h */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gfx_addr_h              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_addr_h;

/* Define the union g1_gfx_addr_l */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gfx_addr_l              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_addr_l;

/* Define the union g1_gfx_stride */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int surface_stride          : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_stride;

/* Define the union g1_gfx_addr_h_c */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gfx_addr_h_c            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_addr_h_c;

/* Define the union g1_gfx_addr_l_c */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gfx_addr_l_c            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_addr_l_c;

/* Define the union g1_gfx_stride_c */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int surface_stride_c        : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_stride_c;

/* Define the union g1_dcmp_addr_h */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dcmp_addr_h             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_dcmp_addr_h;

/* Define the union g1_dcmp_addr_l */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dcmp_addr_l             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_dcmp_addr_l;

/* Define the union g1_gfx_testpat_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tp_speed                : 10;      /* [9:0] */
        unsigned int reserved0               : 2;       /* [11:10] */
        unsigned int tp_line_w               : 1;       /* [12] */
        unsigned int tp_color_mode           : 1;       /* [13] */
        unsigned int reserved1               : 2;       /* [15:14] */
        unsigned int tp_mode                 : 2;       /* [17:16] */
        unsigned int reserved2               : 14;      /* [31:18] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_testpat_cfg;

/* Define the union g1_gfx_testpat_seed */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tp_seed                 : 30;      /* [29:0] */
        unsigned int reserved                : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_testpat_seed;

/* Define the union g1_dcmp_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ice_en                  : 1;       /* [0] */
        unsigned int dcmp_mode               : 1;       /* [1] */
        unsigned int dcmp_fmt                : 2;       /* [3:2] */
        unsigned int alpha_bypass            : 1;       /* [4] */
        unsigned int reserved                : 27;      /* [31:5] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_dcmp_ctrl;

/* Define the union g1_gfx_debug_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int checksum_en             : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_debug_ctrl;

/* Define the union g1_htc_online_dcmp0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pix_format              : 3;       /* [2:0] */
        unsigned int bit_depth               : 1;       /* [3] */
        unsigned int block_size              : 6;       /* [9:4] */
        unsigned int dcmp_mpde               : 1;       /* [10] */
        unsigned int round_offset            : 16;      /* [26:11] */
        unsigned int reserved                : 5;       /* [31:27] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_htc_online_dcmp0;

/* Define the union g1_htc_online_dcmp1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_metric_enable       : 8;       /* [7:0] */
        unsigned int transform_skip          : 1;       /* [8] */
        unsigned int iter_protect_flag       : 1;       /* [9] */
        unsigned int flat_protect_en         : 1;       /* [10] */
        unsigned int flat_protect_withqs_en  : 1;       /* [11] */
        unsigned int flat_protect_clip_en    : 1;       /* [12] */
        unsigned int csc_skip                : 1;       /* [13] */
        unsigned int flat_protect_round_en   : 1;       /* [14] */
        unsigned int reserved                : 17;      /* [31:15] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_htc_online_dcmp1;

/* Define the union g1_gfx_in_ar_checksum0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ar_checksum0            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_in_ar_checksum0;

/* Define the union g1_gfx_in_ar_checksum1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ar_checksum1            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_in_ar_checksum1;

/* Define the union g1_gfx_in_gb_checksum0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gb_checksum0            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_in_gb_checksum0;

/* Define the union g1_gfx_in_gb_checksum1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gb_checksum1            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_in_gb_checksum1;

/* Define the union g1_gfx_ckey_max */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int key_b_max               : 8;       /* [7:0] */
        unsigned int key_g_max               : 8;       /* [15:8] */
        unsigned int key_r_max               : 8;       /* [23:16] */
        unsigned int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_ckey_max;

/* Define the union g1_gfx_ckey_min */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int key_b_min               : 8;       /* [7:0] */
        unsigned int key_g_min               : 8;       /* [15:8] */
        unsigned int key_r_min               : 8;       /* [23:16] */
        unsigned int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_ckey_min;

/* Define the union g1_gfx_ckey_mask */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int key_b_msk               : 8;       /* [7:0] */
        unsigned int key_g_msk               : 8;       /* [15:8] */
        unsigned int key_r_msk               : 8;       /* [23:16] */
        unsigned int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_ckey_mask;

/* Define the union g1_cmp_reso_debug0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved                : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_cmp_reso_debug0;

/* Define the union g1_cmp_reso_debug1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved                : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_cmp_reso_debug1;

/* Define the union g1_cmp_reso_debug2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved                : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_cmp_reso_debug2;

/* Define the union g1_htc_online_dcmp_debug0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int htc_dcmp_debug0         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_htc_online_dcmp_debug0;

/* Define the union g1_htc_online_dcmp_debug1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int htc_dcmp_debug1         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_htc_online_dcmp_debug1;

/* Define the union g1_htc_online_dcmp_debug2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int htc_dcmp_debug2         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_htc_online_dcmp_debug2;

/* Define the union g1_htc_online_dcmp_debug3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int htc_dcmp_debug3         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_htc_online_dcmp_debug3;

/* Define the union wbc_dhd_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int des_fmt                 : 6;       /* [5:0] */
        unsigned int des_argb_order          : 5;       /* [10:6] */
        unsigned int des_premult_en          : 1;       /* [11] */
        unsigned int reserved0               : 5;       /* [16:12] */
        unsigned int des_rgb_round           : 1;       /* [17] */
        unsigned int des_alpha_range         : 1;       /* [18] */
        unsigned int des_v_scan_ord          : 1;       /* [19] */
        unsigned int des_h_scan_ord          : 1;       /* [20] */
        unsigned int reserved1               : 7;       /* [27:21] */
        unsigned int des_fast_req            : 1;       /* [28] */
        unsigned int des_clk_en              : 1;       /* [29] */
        unsigned int reserved2               : 1;       /* [30] */
        unsigned int des_en                  : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_wbc_dhd_ctrl;

/* Define the union wbc_dhd_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int des_addr                : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_wbc_dhd_addr;

/* Define the union wbc_dhd_stride */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int des_stride              : 20;      /* [19:0] */
        unsigned int reserved                : 12;      /* [31:20] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_wbc_dhd_stride;

/* Define the union wbc_dhd_imgsize */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int des_width               : 16;      /* [15:0] */
        unsigned int des_height              : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_wbc_dhd_imgsize;

/* Define the union wbc_dhd_cmd_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int des_cmd_interval        : 16;      /* [15:0] */
        unsigned int reserved                : 15;      /* [30:16] */
        unsigned int des_cmd_ctrl_en         : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_wbc_dhd_cmd_ctrl;

/*
 * DEFINE GLOBAL STRUCT
 */
typedef struct {
    u_voctrl                             voctrl;                              /* 0 */
    u_vointsta                           vointsta;                            /* 4 */
    u_vomskintsta                        vomskintsta;                         /* 8 */
    u_vointmsk                           vointmsk;                            /* c */
    u_vodebug                            vodebug;                             /* 10 */
    u_vocmd_intsta                       vocmd_intsta;                        /* 14 */
    u_vocmd_mskintsta                    vocmd_mskintsta;                     /* 18 */
    u_vocmd_intmsk                       vocmd_intmsk;                        /* 1c */
    u_voversion1                         voversion1;                          /* 20 */
    u_voversion2                         voversion2;                          /* 24 */
    u_volowpower_ctrl                    volowpower_ctrl;                     /* 28 */
    u_voufsta                            voufsta;                             /* 2c */
    u_voufclr                            voufclr;                             /* 30 */
    u_vointproc_tim                      vointproc_tim;                       /* 34 */
    u_vofpgatest                         vofpgatest;                          /* 38 */
    unsigned int                         rsr0[3];                             /* 3c ~ 44 */
    u_volowpower_ctrl1                   volowpower_ctrl1;                    /* 48 */
    unsigned int                         rsr1;                                /* 4c */
    u_dpu_sram_ctrl                      dpu_sram_ctrl;                       /* 50 */
    unsigned int                         rsr2[171];                           /* 54 ~ 2fc */
    u_cbm_bkg                            cbm_bkg;                             /* 300 */
    unsigned int                         rsr3;                                /* 304 */
    u_cbm_mix                            cbm_mix;                             /* 308 */
    u_cbm_ctrl                           cbm_ctrl;                            /* 30c */
    unsigned int                         rsr4[60];                            /* 310 ~ 3fc */
    u_dpu_partial_ctrl                   dpu_partial_ctrl;                    /* 400 */
    u_dpu_partial_gen_hdr0               dpu_partial_gen_hdr0;                /* 404 */
    u_dpu_partial_gen_pld_data00         dpu_partial_gen_pld_data00;          /* 408 */
    u_dpu_partial_gen_pld_data01         dpu_partial_gen_pld_data01;          /* 40c */
    u_dpu_partial_gen_hdr1               dpu_partial_gen_hdr1;                /* 410 */
    u_dpu_partial_gen_pld_data10         dpu_partial_gen_pld_data10;          /* 414 */
    u_dpu_partial_gen_pld_data11         dpu_partial_gen_pld_data11;          /* 418 */
    unsigned int                         rsr5[505];                           /* 41c ~ bfc */
    u_link_ctrl                          link_ctrl;                           /* c00 */
    unsigned int                         rsr6[127];                           /* c04 ~ dfc */
    u_para_haddr_vhd_chn00               para_haddr_vhd_chn00;                /* e00 */
    u_para_addr_vhd_chn00                para_addr_vhd_chn00;                 /* e04 */
    unsigned int                         rsr7[62];                            /* e08 ~ efc */
    u_para_up_vhd                        para_up_vhd;                         /* f00 */
    unsigned int                         rsr8[17];                            /* f04 ~ f44 */
    u_para_conflict_clr                  para_conflict_clr;                   /* f48 */
    u_para_conflict_sta                  para_conflict_sta;                   /* f4c */
    unsigned int                         rsr9[44];                            /* f50 ~ ffc */
    u_dpu_mapping_ctrl                   mapping_ctrl;                        /* 1000 */
    u_dpu_mapping_coef00                 mapping_coef00;                      /* 1004 */
    u_dpu_mapping_coef01                 mapping_coef01;                      /* 1008 */
    u_dpu_mapping_coef02                 mapping_coef02;                      /* 100c */
    u_dpu_mapping_coef03                 mapping_coef03;                      /* 1010 */
    u_dpu_mapping_coef10                 mapping_coef10;                      /* 1014 */
    u_dpu_mapping_coef11                 mapping_coef11;                      /* 1018 */
    u_dpu_mapping_coef12                 mapping_coef12;                      /* 101c */
    u_dpu_mapping_coef13                 mapping_coef13;                      /* 1020 */
    u_dpu_mapping_coef20                 mapping_coef20;                      /* 1024 */
    u_dpu_mapping_coef21                 mapping_coef21;                      /* 1028 */
    u_dpu_mapping_coef22                 mapping_coef22;                      /* 102c */
    u_dpu_mapping_coef23                 mapping_coef23;                      /* 1030 */
    u_dpu_mapping_coef30                 mapping_coef30;                      /* 1034 */
    u_dpu_mapping_coef31                 mapping_coef31;                      /* 1038 */
    u_dpu_mapping_coef32                 mapping_coef32;                      /* 103c */
    u_dpu_mapping_coef33                 mapping_coef33;                      /* 1040 */
    u_dpu_mapping_model_wh               mapping_model_wh;                    /* 1044 */
    u_dpu_mapping_pattern_fill_color     mapping_pattern_fill_color;          /* 1048 */
    u_dpu_mapping_shadow_color           mapping_shadow_color;                /* 104c */
    u_dpu_mapping_bg_color               mapping_bg_color;                    /* 1050 */
    u_dpu_mapping_scale_x                mapping_scale_x;                     /* 1054 */
    u_dpu_mapping_scale_y                mapping_scale_y;                     /* 1058 */
    u_dpu_mapping_offset_x               mapping_offset_x;                    /* 105c */
    u_dpu_mapping_offset_y               mapping_offset_y;                    /* 1060 */
    u_dpu_mapping_src_ratio_x            mapping_src_ratio_x;                 /* 1064 */
    u_dpu_mapping_src_ratio_y            mapping_src_ratio_y;                 /* 1068 */
    u_dpu_mapping_src_offset_x           mapping_src_offset_x;                /* 106c */
    u_dpu_mapping_src_offset_y           mapping_src_offset_y;                /* 1070 */
    u_dpu_mapping_debug0                 mapping_debug0;                      /* 1074 */
    u_dpu_mapping_crop_xy                mapping_crop_xy;                     /* 1078 */
    u_dpu_mapping_crop_wh                mapping_crop_wh;                     /* 107c */
    unsigned int                         rsr10[32];                           /* 1080 ~ 10fc */
    u_dpu_add_pix_ctrl                   add_pix_ctrl;                        /* 1100 */
    u_dpu_add_pix_small                  add_pix_small;                       /* 1104 */
    u_dpu_add_pix_big                    add_pix_big;                         /* 1108 */
    u_dpu_add_pix_rect                   add_pix_rect;                        /* 110c */
    u_dpu_add_pix_tile                   add_pix_tile;                        /* 1110 */
    unsigned int                         rsr11[6075];                         /* 1114 ~ 6ffc */
    u_g0_ctrl                            g0_ctrl;                             /* 7000 */
    u_g0_upd                             g0_upd;                              /* 7004 */
    unsigned int                         rsr12;                               /* 7008 */
    u_g0_0reso_read                      g0_0reso_read;                       /* 700c */
    unsigned int                         rsr13[28];                           /* 7010 ~ 707c */
    u_g0_dfpos                           g0_dfpos;                            /* 7080 */
    u_g0_dlpos                           g0_dlpos;                            /* 7084 */
    u_g0_vfpos                           g0_vfpos;                            /* 7088 */
    u_g0_vlpos                           g0_vlpos;                            /* 708c */
    u_g0_bk                              g0_bk;                               /* 7090 */
    u_g0_alpha                           g0_alpha;                            /* 7094 */
    u_g0_mute_bk                         g0_mute_bk;                          /* 7098 */
    unsigned int                         rsr14[25];                           /* 709c ~ 70fc */
    u_g0_t2r0_ctrl                       g0_t2r0_ctrl;                        /* 7100 */
    u_g0_t2r0_err_clr                    g0_t2r0_err_clr;                     /* 7104 */
    u_g0_t2r0_err_sta                    g0_t2r0_err_sta;                     /* 7108 */
    u_g0_t2r0_debug1                     g0_t2r0_debug1;                      /* 710c */
    u_g0_t2r0_debug2                     g0_t2r0_debug2;                      /* 7110 */
    u_g0_t2r0_debug3                     g0_t2r0_debug3;                      /* 7114 */
    u_g0_t2r0_debug4                     g0_t2r0_debug4;                      /* 7118 */
    u_g0_t2r0_wh                         g0_t2r0_wh;                          /* 711c */
    unsigned int                         rsr15[56];                           /* 7120 ~ 71fc */
    u_g0_crop0_wh                        g0_crop0_wh;                         /* 7200 */
    u_g0_crop0_xy                        g0_crop0_xy;                         /* 7204 */
    u_g0_crop0_in_wh                     g0_crop0_in_wh;                      /* 7208 */
    unsigned int                         rsr16[381];                          /* 720c ~ 77fc */
    u_g1_ctrl                            g1_ctrl;                             /* 7800 */
    u_g1_upd                             g1_upd;                              /* 7804 */
    unsigned int                         rsr17;                               /* 7808 */
    u_g1_0reso_read                      g1_0reso_read;                       /* 780c */
    unsigned int                         rsr18[28];                           /* 7810 ~ 787c */
    u_g1_dfpos                           g1_dfpos;                            /* 7880 */
    u_g1_dlpos                           g1_dlpos;                            /* 7884 */
    u_g1_vfpos                           g1_vfpos;                            /* 7888 */
    u_g1_vlpos                           g1_vlpos;                            /* 788c */
    u_g1_bk                              g1_bk;                               /* 7890 */
    u_g1_alpha                           g1_alpha;                            /* 7894 */
    u_g1_mute_bk                         g1_mute_bk;                          /* 7898 */
    unsigned int                         rsr19[25];                           /* 789c ~ 78fc */
    u_g1_csc_ctrl                        g1_csc_ctrl;                         /* 7900 */
    u_g1_csc_coef00                      g1_csc_coef00;                       /* 7904 */
    u_g1_csc_coef01                      g1_csc_coef01;                       /* 7908 */
    u_g1_csc_coef02                      g1_csc_coef02;                       /* 790c */
    u_g1_csc_coef10                      g1_csc_coef10;                       /* 7910 */
    u_g1_csc_coef11                      g1_csc_coef11;                       /* 7914 */
    u_g1_csc_coef12                      g1_csc_coef12;                       /* 7918 */
    u_g1_csc_coef20                      g1_csc_coef20;                       /* 791c */
    u_g1_csc_coef21                      g1_csc_coef21;                       /* 7920 */
    u_g1_csc_coef22                      g1_csc_coef22;                       /* 7924 */
    u_g1_csc_scale                       g1_csc_scale;                        /* 7928 */
    u_g1_csc_idc0                        g1_csc_idc0;                         /* 792c */
    u_g1_csc_idc1                        g1_csc_idc1;                         /* 7930 */
    u_g1_csc_idc2                        g1_csc_idc2;                         /* 7934 */
    u_g1_csc_odc0                        g1_csc_odc0;                         /* 7938 */
    u_g1_csc_odc1                        g1_csc_odc1;                         /* 793c */
    u_g1_csc_odc2                        g1_csc_odc2;                         /* 7940 */
    u_g1_csc_min_y                       g1_csc_min_y;                        /* 7944 */
    u_g1_csc_min_c                       g1_csc_min_c;                        /* 7948 */
    u_g1_csc_max_y                       g1_csc_max_y;                        /* 794c */
    u_g1_csc_max_c                       g1_csc_max_c;                        /* 7950 */
    u_g1_csc2_coef00                     g1_csc2_coef00;                      /* 7954 */
    u_g1_csc2_coef01                     g1_csc2_coef01;                      /* 7958 */
    u_g1_csc2_coef02                     g1_csc2_coef02;                      /* 795c */
    u_g1_csc2_coef10                     g1_csc2_coef10;                      /* 7960 */
    u_g1_csc2_coef11                     g1_csc2_coef11;                      /* 7964 */
    u_g1_csc2_coef12                     g1_csc2_coef12;                      /* 7968 */
    u_g1_csc2_coef20                     g1_csc2_coef20;                      /* 796c */
    u_g1_csc2_coef21                     g1_csc2_coef21;                      /* 7970 */
    u_g1_csc2_coef22                     g1_csc2_coef22;                      /* 7974 */
    u_g1_csc2_scale                      g1_csc2_scale;                       /* 7978 */
    u_g1_csc2_idc0                       g1_csc2_idc0;                        /* 797c */
    u_g1_csc2_idc1                       g1_csc2_idc1;                        /* 7980 */
    u_g1_csc2_idc2                       g1_csc2_idc2;                        /* 7984 */
    u_g1_csc2_odc0                       g1_csc2_odc0;                        /* 7988 */
    u_g1_csc2_odc1                       g1_csc2_odc1;                        /* 798c */
    u_g1_csc2_odc2                       g1_csc2_odc2;                        /* 7990 */
    u_g1_csc2_min_y                      g1_csc2_min_y;                       /* 7994 */
    u_g1_csc2_min_c                      g1_csc2_min_c;                       /* 7998 */
    u_g1_csc2_max_y                      g1_csc2_max_y;                       /* 799c */
    u_g1_csc2_max_c                      g1_csc2_max_c;                       /* 79a0 */
    unsigned int                         rsr20[19];                           /* 79a4 ~ 79ec */
    u_g1_csc_ink_ctrl                    g1_csc_ink_ctrl;                     /* 79f0 */
    u_g1_csc_ink_pos                     g1_csc_ink_pos;                      /* 79f4 */
    u_g1_csc_ink_data                    g1_csc_ink_data;                     /* 79f8 */
    u_g1_csc_ink_data2                   g1_csc_ink_data2;                    /* 79fc */
    unsigned int                         rsr21[64];                           /* 7a00 ~ 7afc */
    u_g1_cvfir_vinfo                     g1_cvfir_vinfo;                      /* 7b00 */
    u_g1_cvfir_vsp                       g1_cvfir_vsp;                        /* 7b04 */
    u_g1_cvfir_voffset                   g1_cvfir_voffset;                    /* 7b08 */
    u_g1_cvfir_vboffset                  g1_cvfir_vboffset;                   /* 7b0c */
    unsigned int                         rsr22[8];                            /* 7b10 ~ 7b2c */
    u_g1_cvfir_vcoef0                    g1_cvfir_vcoef0;                     /* 7b30 */
    u_g1_cvfir_vcoef1                    g1_cvfir_vcoef1;                     /* 7b34 */
    u_g1_cvfir_vcoef2                    g1_cvfir_vcoef2;                     /* 7b38 */
    unsigned int                         rsr23[49];                           /* 7b3c ~ 7bfc */
    u_g1_hfir_ctrl                       g1_hfir_ctrl;                        /* 7c00 */
    u_g1_hfircoef01                      g1_hfircoef01;                       /* 7c04 */
    u_g1_hfircoef23                      g1_hfircoef23;                       /* 7c08 */
    u_g1_hfircoef45                      g1_hfircoef45;                       /* 7c0c */
    u_g1_hfircoef67                      g1_hfircoef67;                       /* 7c10 */
    unsigned int                         rsr24[59];                           /* 7c14 ~ 7cfc */
    u_g1_t2r1_ctrl                       g1_t2r1_ctrl;                        /* 7d00 */
    u_g1_t2r1_err_clr                    g1_t2r1_err_clr;                     /* 7d04 */
    u_g1_t2r1_err_sta                    g1_t2r1_err_sta;                     /* 7d08 */
    u_g1_t2r1_debug1                     g1_t2r1_debug1;                      /* 7d0c */
    u_g1_t2r1_debug2                     g1_t2r1_debug2;                      /* 7d10 */
    u_g1_t2r1_debug3                     g1_t2r1_debug3;                      /* 7d14 */
    u_g1_t2r1_debug4                     g1_t2r1_debug4;                      /* 7d18 */
    u_g1_t2r1_wh                         g1_t2r1_wh;                          /* 7d1c */
    unsigned int                         rsr25[56];                           /* 7d20 ~ 7dfc */
    u_g1_crop1_wh                        g1_crop1_wh;                         /* 7e00 */
    u_g1_crop1_xy                        g1_crop1_xy;                         /* 7e04 */
    u_g1_crop1_in_wh                     g1_crop1_in_wh;                      /* 7e08 */
    unsigned int                         rsr26[2173];                         /* 7e0c ~ 9ffc */
    u_dhd_glob_ctrl                      dhd_glob_ctrl;                       /* a000 */
    u_dhd_htc_online_cmp0                dhd_htc_online_cmp0;                 /* a004 */
    u_dhd_htc_online_cmp1                dhd_htc_online_cmp1;                 /* a008 */
    u_dhd_htc_online_cmp_bs_idx          dhd_htc_online_cmp_bs_idx;           /* a00c */
    u_dhd_htc_online_cmp_crc             dhd_htc_online_cmp_crc;              /* a010 */
    u_dhd_htc_online_cmp_chksum          dhd_htc_online_cmp_chksum;           /* a014 */
    u_dhd_htc_online_cmp_ink_data_l      dhd_htc_online_cmp_ink_data_l;       /* a018 */
    u_dhd_htc_online_cmp_ink_data_h      dhd_htc_online_cmp_ink_data_h;       /* a01c */
    u_dhd_htc_online_cmp_sta             dhd_htc_online_cmp_sta;              /* a020 */
    u_dhd_htc_online_cmp_debug0          dhd_htc_online_cmp_debug0;           /* a024 */
    u_dhd_htc_online_cmp_debug1          dhd_htc_online_cmp_debug1;           /* a028 */
    unsigned int                         rsr27[53];                           /* a02c ~ a0fc */
    u_dhd_dither_ctrl                    dhd_dither_ctrl;                     /* a100 */
    u_dhd_dither_sed_y0                  dhd_dither_sed_y0;                   /* a104 */
    u_dhd_dither_sed_u0                  dhd_dither_sed_u0;                   /* a108 */
    u_dhd_dither_sed_v0                  dhd_dither_sed_v0;                   /* a10c */
    u_dhd_dither_sed_w0                  dhd_dither_sed_w0;                   /* a110 */
    u_dhd_dither_sed_y1                  dhd_dither_sed_y1;                   /* a114 */
    u_dhd_dither_sed_u1                  dhd_dither_sed_u1;                   /* a118 */
    u_dhd_dither_sed_v1                  dhd_dither_sed_v1;                   /* a11c */
    u_dhd_dither_sed_w1                  dhd_dither_sed_w1;                   /* a120 */
    u_dhd_dither_sed_y2                  dhd_dither_sed_y2;                   /* a124 */
    u_dhd_dither_sed_u2                  dhd_dither_sed_u2;                   /* a128 */
    u_dhd_dither_sed_v2                  dhd_dither_sed_v2;                   /* a12c */
    u_dhd_dither_sed_w2                  dhd_dither_sed_w2;                   /* a130 */
    u_dhd_dither_sed_y3                  dhd_dither_sed_y3;                   /* a134 */
    u_dhd_dither_sed_u3                  dhd_dither_sed_u3;                   /* a138 */
    u_dhd_dither_sed_v3                  dhd_dither_sed_v3;                   /* a13c */
    u_dhd_dither_sed_w3                  dhd_dither_sed_w3;                   /* a140 */
    u_dhd_dither_thr                     dhd_dither_thr;                      /* a144 */
    unsigned int                         rsr28[46];                           /* a148 ~ a1fc */
    u_rnd_crop_cfg                       rnd_crop_cfg;                        /* a200 */
    u_rnd_crop_lut_rd_en                 rnd_crop_lut_rd_en;                  /* a204 */
    u_rnd_crop_lut_rdata                 rnd_crop_lut_rdata;                  /* a208 */
    unsigned int                         rsr29[61];                           /* a20c ~ a2fc */
    u_dhd_add_pix_ctrl                   dhd_add_pix_ctrl;                    /* a300 */
    u_dhd_add_pix_small                  dhd_add_pix_small;                   /* a304 */
    u_dhd_add_pix_big                    dhd_add_pix_big;                     /* a308 */
    u_dhd_add_pix_rect                   dhd_add_pix_rect;                    /* a30c */
    u_dhd_add_pix_tile                   dhd_add_pix_tile;                    /* a310 */
    unsigned int                         rsr30[59];                           /* a314 ~ a3fc */
    u_dhd_t2r1_ctrl                      dhd_t2r1_ctrl;                       /* a400 */
    u_dhd_t2r1_err_clr                   dhd_t2r1_err_clr;                    /* a404 */
    u_dhd_t2r1_err_sta                   dhd_t2r1_err_sta;                    /* a408 */
    u_dhd_t2r1_debug1                    dhd_t2r1_debug1;                     /* a40c */
    u_dhd_t2r1_debug2                    dhd_t2r1_debug2;                     /* a410 */
    u_dhd_t2r1_debug3                    dhd_t2r1_debug3;                     /* a414 */
    u_dhd_t2r1_debug4                    dhd_t2r1_debug4;                     /* a418 */
    u_dhd_t2r1_wh                        dhd_t2r1_wh;                         /* a41c */
    unsigned int                         rsr31[56];                           /* a420 ~ a4fc */
    u_vhdaccctrl                         vhdaccctrl;                          /* a500 */
    u_vhdacchpos                         vhdacchpos;                          /* a504 */
    u_vhdaccvpos                         vhdaccvpos;                          /* a508 */
    u_vhdaccdemo                         vhdaccdemo;                          /* a50c */
    u_vhdacclut1                         vhdacclut1;                          /* a510 */
    u_vhdacclut2                         vhdacclut2;                          /* a514 */
    u_vhdacclut3                         vhdacclut3;                          /* a518 */
    u_vhdacclut4                         vhdacclut4;                          /* a51c */
    u_vhdacclut5                         vhdacclut5;                          /* a520 */
    u_vhdacclut6                         vhdacclut6;                          /* a524 */
    u_vhdacclut7                         vhdacclut7;                          /* a528 */
    u_vhdacclut8                         vhdacclut8;                          /* a52c */
    u_vhdacclut9                         vhdacclut9;                          /* a530 */
    u_vhdacclut10                        vhdacclut10;                         /* a534 */
    u_vhdacclut11                        vhdacclut11;                         /* a538 */
    unsigned int                         rsr32[9];                            /* a53c ~ a55c */
    u_vhdaccstatctrl                     vhdaccstatctrl;                      /* a560 */
    u_vhdaccstathpos                     vhdaccstathpos;                      /* a564 */
    u_vhdaccstatvpos                     vhdaccstatvpos;                      /* a568 */
    u_vhdaccshiftd                       vhdaccshiftd;                        /* a56c */
    u_vhdaccdnsam                        vhdaccdnsam;                         /* a570 */
    u_vhdaccsatcfg                       vhdaccsatcfg;                        /* a574 */
    unsigned int                         rsr33[2];                            /* a578 ~ a57c */
    u_vhdaccydiffd1sum                   vhdaccydiffd1sum;                    /* a580 */
    u_vhdaccydiffd2sum                   vhdaccydiffd2sum;                    /* a584 */
    u_vhdaccysum                         vhdaccysum;                          /* a588 */
    u_vhdaccsatsum                       vhdaccsatsum;                        /* a58c */
    u_vhdaccsathistsum                   vhdaccsathistsum;                    /* a590 */
    u_vhdaccsathistcum                   vhdaccsathistcum;                    /* a594 */
    u_vhdaccyhist_0                      vhdaccyhist_0;                       /* a598 */
    u_vhdaccyhist_1                      vhdaccyhist_1;                       /* a59c */
    u_vhdaccyhist_2                      vhdaccyhist_2;                       /* a5a0 */
    u_vhdaccyhist_3                      vhdaccyhist_3;                       /* a5a4 */
    u_vhdaccyhist_4                      vhdaccyhist_4;                       /* a5a8 */
    u_vhdaccyhist_5                      vhdaccyhist_5;                       /* a5ac */
    u_vhdaccyhist_6                      vhdaccyhist_6;                       /* a5b0 */
    u_vhdaccyhist_7                      vhdaccyhist_7;                       /* a5b4 */
    u_vhdaccyhist_8                      vhdaccyhist_8;                       /* a5b8 */
    u_vhdaccyhist_9                      vhdaccyhist_9;                       /* a5bc */
    u_vhdaccyhist_10                     vhdaccyhist_10;                      /* a5c0 */
    u_vhdaccyhist_11                     vhdaccyhist_11;                      /* a5c4 */
    u_vhdaccyhist_12                     vhdaccyhist_12;                      /* a5c8 */
    u_vhdaccyhist_13                     vhdaccyhist_13;                      /* a5cc */
    u_vhdaccyhist_14                     vhdaccyhist_14;                      /* a5d0 */
    u_vhdaccyhist_15                     vhdaccyhist_15;                      /* a5d4 */
    u_vhdaccyhist_16                     vhdaccyhist_16;                      /* a5d8 */
    u_vhdaccyhist_17                     vhdaccyhist_17;                      /* a5dc */
    u_vhdaccyhist_18                     vhdaccyhist_18;                      /* a5e0 */
    u_vhdaccyhist_19                     vhdaccyhist_19;                      /* a5e4 */
    u_vhdaccsathist_0                    vhdaccsathist_0;                     /* a5e8 */
    u_vhdaccsathist_1                    vhdaccsathist_1;                     /* a5ec */
    u_vhdaccsathist_2                    vhdaccsathist_2;                     /* a5f0 */
    u_vhdaccsathist_3                    vhdaccsathist_3;                     /* a5f4 */
    u_vhdaccsathist_4                    vhdaccsathist_4;                     /* a5f8 */
    u_vhdaccsathist_5                    vhdaccsathist_5;                     /* a5fc */
    u_vhdaccsathist_6                    vhdaccsathist_6;                     /* a600 */
    u_vhdaccsathist_7                    vhdaccsathist_7;                     /* a604 */
    u_vhdaccsathist_8                    vhdaccsathist_8;                     /* a608 */
    u_vhdaccsathist_9                    vhdaccsathist_9;                     /* a60c */
    u_vhdaccsathist_10                   vhdaccsathist_10;                    /* a610 */
    u_vhdaccsathist_11                   vhdaccsathist_11;                    /* a614 */
    u_vhdaccsathist_12                   vhdaccsathist_12;                    /* a618 */
    u_vhdaccsathist_13                   vhdaccsathist_13;                    /* a61c */
    u_vhdaccsathist_14                   vhdaccsathist_14;                    /* a620 */
    u_vhdaccsathist_15                   vhdaccsathist_15;                    /* a624 */
    u_vhdaccsathist_16                   vhdaccsathist_16;                    /* a628 */
    u_vhdaccsathist_17                   vhdaccsathist_17;                    /* a62c */
    u_vhdaccsathist_18                   vhdaccsathist_18;                    /* a630 */
    u_vhdaccsathist_19                   vhdaccsathist_19;                    /* a634 */
    unsigned int                         rsr34[2674];                         /* a638 ~ cffc */
    u_dhd0_ctrl                          dhd0_ctrl;                           /* d000 */
    u_dhd0_vsync1                        dhd0_vsync1;                         /* d004 */
    u_dhd0_vsync2                        dhd0_vsync2;                         /* d008 */
    u_dhd0_hsync1                        dhd0_hsync1;                         /* d00c */
    u_dhd0_hsync2                        dhd0_hsync2;                         /* d010 */
    u_dhd0_vplus1                        dhd0_vplus1;                         /* d014 */
    u_dhd0_vplus2                        dhd0_vplus2;                         /* d018 */
    u_dhd0_pwr                           dhd0_pwr;                            /* d01c */
    u_dhd0_vtthd3                        dhd0_vtthd3;                         /* d020 */
    u_dhd0_vtthd                         dhd0_vtthd;                          /* d024 */
    u_dhd0_parathd                       dhd0_parathd;                        /* d028 */
    u_dhd0_te_ctrl                       dhd0_te_ctrl;                        /* d02c */
    u_dhd0_start_pos                     dhd0_start_pos;                      /* d030 */
    unsigned int                         rsr35[3];                            /* d034 ~ d03c */
    u_dhd0_clk_dv_ctrl                   dhd0_clk_dv_ctrl;                    /* d040 */
    u_dhd0_rgb_fix_ctrl                  dhd0_rgb_fix_ctrl;                   /* d044 */
    unsigned int                         rsr36[4];                            /* d048 ~ d054 */
    u_dhd0_intf_chksum_y                 dhd0_intf_chksum_y;                  /* d058 */
    u_dhd0_intf_chksum_u                 dhd0_intf_chksum_u;                  /* d05c */
    u_dhd0_intf_chksum_v                 dhd0_intf_chksum_v;                  /* d060 */
    u_dhd0_intf1_chksum_y                dhd0_intf1_chksum_y;                 /* d064 */
    u_dhd0_intf1_chksum_u                dhd0_intf1_chksum_u;                 /* d068 */
    u_dhd0_intf1_chksum_v                dhd0_intf1_chksum_v;                 /* d06c */
    u_dhd0_intf_chksum_high1             dhd0_intf_chksum_high1;              /* d070 */
    u_dhd0_intf_chksum_high2             dhd0_intf_chksum_high2;              /* d074 */
    u_dhd0_intf_cmdcfg                   dhd0_intf_cmdcfg;                    /* d078 */
    u_dhd0_tde_frm_rdy                   dhd0_tde_frm_rdy;                    /* d07c */
    u_dhd0_te_timeout                    dhd0_te_timeout;                     /* d080 */
    unsigned int                         rsr37;                               /* d084 */
    u_dhd0_state                         dhd0_state;                          /* d088 */
    u_dhd0_uf_state                      dhd0_uf_state;                       /* d08c */
    u_vo_mux                             vo_mux;                              /* d090 */
    u_vo_mux_sync                        vo_mux_sync;                         /* d094 */
    u_vo_mux_data                        vo_mux_data;                         /* d098 */
    unsigned int                         rsr38;                               /* d09c */
    u_dhd0_vsync_te_state                dhd0_vsync_te_state;                 /* d0a0 */
    u_dhd0_vsync_te_state1               dhd0_vsync_te_state1;                /* d0a4 */
    unsigned int                         rsr39[50];                           /* d0a8 ~ d16c */
    u_intf0_sdlp_ctrl                    intf0_sdlp_ctrl;                     /* d170 */
    u_intf0_sdlp_delay                   intf0_sdlp_delay;                    /* d174 */
    u_intf0_sdlp_stop                    intf0_sdlp_stop;                     /* d178 */
    u_intf0_sdlp_sta                     intf0_sdlp_sta;                      /* d17c */
    unsigned int                         rsr40;                               /* d180 */
    u_intf_mipi_upd                      intf_mipi_upd;                       /* d184 */
    u_intf_mipi_sync_inv                 intf_mipi_sync_inv;                  /* d188 */
    unsigned int                         rsr41[93];                           /* d18c ~ d2fc */
    u_intf_lcd_ctrl                      intf_lcd_ctrl;                       /* d300 */
    u_intf_lcd_upd                       intf_lcd_upd;                        /* d304 */
    u_intf_lcd_sync_inv                  intf_lcd_sync_inv;                   /* d308 */
    unsigned int                         rsr42[57];                           /* d30c ~ d3ec */
    u_lcd_intf_chksum_y                  lcd_intf_chksum_y;                   /* d3f0 */
    u_lcd_intf_chksum_u                  lcd_intf_chksum_u;                   /* d3f4 */
    u_lcd_intf_chksum_v                  lcd_intf_chksum_v;                   /* d3f8 */
    unsigned int                         rsr43[2817];                         /* d3fc ~ fffc */
    u_mac_outstanding                    mac_outstanding;                     /* 10000 */
    u_mac_ctrl                           mac_ctrl;                            /* 10004 */
    unsigned int                         rsr44[2];                            /* 10008 ~ 1000c */
    u_mac_rchn_prio                      mac_rchn_prio;                       /* 10010 */
    unsigned int                         rsr45;                               /* 10014 */
    u_mac_wchn_prio                      mac_wchn_prio;                       /* 10018 */
    unsigned int                         rsr46[9];                            /* 1001c ~ 1003c */
    u_mac_bus_err_clr                    mac_bus_err_clr;                     /* 10040 */
    u_mac_bus_err                        mac_bus_err;                         /* 10044 */
    unsigned int                         rsr47[2];                            /* 10048 ~ 1004c */
    u_mac_src0_status0                   mac_src0_status0;                    /* 10050 */
    u_mac_src0_status1                   mac_src0_status1;                    /* 10054 */
    unsigned int                         rsr48[6];                            /* 10058 ~ 1006c */
    u_mac_debug_ctrl                     mac_debug_ctrl;                      /* 10070 */
    u_mac_debug_clr                      mac_debug_clr;                       /* 10074 */
    unsigned int                         rsr49[2];                            /* 10078 ~ 1007c */
    u_mac_debug_info                     mac_debug_info;                      /* 10080 */
    unsigned int                         rsr50[3];                            /* 10084 ~ 1008c */
    u_mac_rd_info                        mac_rd_info;                         /* 10090 */
    u_mac_wr_info                        mac_wr_info;                         /* 10094 */
    unsigned int                         rsr51[6];                            /* 10098 ~ 100ac */
    u_mac_det_latency0                   mac_det_latency0;                    /* 100b0 */
    u_mac_det_latency1                   mac_det_latency1;                    /* 100b4 */
    u_mac_det_latency2                   mac_det_latency2;                    /* 100b8 */
    u_mac_det_latency3                   mac_det_latency3;                    /* 100bc */
    u_mac_det_latency4                   mac_det_latency4;                    /* 100c0 */
    u_mac_det_latency5                   mac_det_latency5;                    /* 100c4 */
    unsigned int                         rsr52[846];                          /* 100c8 ~ 10dfc */
    u_gfx_read_ctrl                      gfx_read_ctrl;                       /* 10e00 */
    u_gfx_mac_ctrl                       gfx_mac_ctrl;                        /* 10e04 */
    u_gfx_out_ctrl                       gfx_out_ctrl;                        /* 10e08 */
    unsigned int                         rsr53;                               /* 10e0c */
    u_gfx_mute_alpha                     gfx_mute_alpha;                      /* 10e10 */
    u_gfx_mute_bk                        gfx_mute_bk;                         /* 10e14 */
    unsigned int                         rsr54[4];                            /* 10e18 ~ 10e24 */
    u_gfx_1555_alpha                     gfx_1555_alpha;                      /* 10e28 */
    unsigned int                         rsr55[5];                            /* 10e2c ~ 10e3c */
    u_gfx_src_info                       gfx_src_info;                        /* 10e40 */
    u_gfx_src_reso                       gfx_src_reso;                        /* 10e44 */
    u_gfx_src_crop                       gfx_src_crop;                        /* 10e48 */
    u_gfx_ireso                          gfx_ireso;                           /* 10e4c */
    u_gfx_addr_h                         gfx_addr_h;                          /* 10e50 */
    u_gfx_addr_l                         gfx_addr_l;                          /* 10e54 */
    unsigned int                         rsr56[2];                            /* 10e58 ~ 10e5c */
    u_gfx_stride                         gfx_stride;                          /* 10e60 */
    u_gfx_addr_h_c                       gfx_addr_h_c;                        /* 10e64 */
    u_gfx_addr_l_c                       gfx_addr_l_c;                        /* 10e68 */
    u_gfx_stride_c                       gfx_stride_c;                        /* 10e6c */
    u_gfx_dcmp_addr_h                    gfx_dcmp_addr_h;                     /* 10e70 */
    u_gfx_dcmp_addr_l                    gfx_dcmp_addr_l;                     /* 10e74 */
    unsigned int                         rsr57[2];                            /* 10e78 ~ 10e7c */
    u_gfx_cmp_reso_debug0                gfx_cmp_reso_debug0;                 /* 10e80 */
    u_gfx_cmp_reso_debug1                gfx_cmp_reso_debug1;                 /* 10e84 */
    u_gfx_cmp_reso_debug2                gfx_cmp_reso_debug2;                 /* 10e88 */
    unsigned int                         rsr58[9];                            /* 10e8c ~ 10eac */
    u_gfx_testpat_cfg                    gfx_testpat_cfg;                     /* 10eb0 */
    u_gfx_testpat_seed                   gfx_testpat_seed;                    /* 10eb4 */
    u_gfx_dcmp_ctrl                      gfx_dcmp_ctrl;                       /* 10eb8 */
    unsigned int                         rsr59;                               /* 10ebc */
    u_gfx_debug_ctrl                     gfx_debug_ctrl;                      /* 10ec0 */
    unsigned int                         rsr60;                               /* 10ec4 */
    u_gfx_htc_online_dcmp0               gfx_htc_online_dcmp0;                /* 10ec8 */
    u_gfx_htc_online_dcmp1               gfx_htc_online_dcmp1;                /* 10ecc */
    unsigned int                         rsr61[4];                            /* 10ed0 ~ 10edc */
    u_gfx_in_ar_checksum0                gfx_in_ar_checksum0;                 /* 10ee0 */
    u_gfx_in_ar_checksum1                gfx_in_ar_checksum1;                 /* 10ee4 */
    u_gfx_in_gb_checksum0                gfx_in_gb_checksum0;                 /* 10ee8 */
    u_gfx_in_gb_checksum1                gfx_in_gb_checksum1;                 /* 10eec */
    unsigned int                         rsr62[4];                            /* 10ef0 ~ 10efc */
    u_gfx_ckey_max                       gfx_ckey_max;                        /* 10f00 */
    u_gfx_ckey_min                       gfx_ckey_min;                        /* 10f04 */
    u_gfx_ckey_mask                      gfx_ckey_mask;                       /* 10f08 */
    unsigned int                         rsr63[29];                           /* 10f0c ~ 10f7c */
    u_gfx_htc_online_dcmp_debug0         gfx_htc_online_dcmp_debug0;          /* 10f80 */
    u_gfx_htc_online_dcmp_debug1         gfx_htc_online_dcmp_debug1;          /* 10f84 */
    u_gfx_htc_online_dcmp_debug2         gfx_htc_online_dcmp_debug2;          /* 10f88 */
    u_gfx_htc_online_dcmp_debug3         gfx_htc_online_dcmp_debug3;          /* 10f8c */
    unsigned int                         rsr64[28];                           /* 10f90 ~ 10ffc */
    u_g1_gfx_read_ctrl                   g1_gfx_read_ctrl;                    /* 11000 */
    u_g1_gfx_mac_ctrl                    g1_gfx_mac_ctrl;                     /* 11004 */
    u_g1_gfx_out_ctrl                    g1_gfx_out_ctrl;                     /* 11008 */
    unsigned int                         rsr65;                               /* 1100c */
    u_g1_gfx_mute_alpha                  g1_gfx_mute_alpha;                   /* 11010 */
    u_g1_gfx_mute_bk                     g1_gfx_mute_bk;                      /* 11014 */
    unsigned int                         rsr66[4];                            /* 11018 ~ 11024 */
    u_g1_gfx_1555_alpha                  g1_gfx_1555_alpha;                   /* 11028 */
    unsigned int                         rsr67[5];                            /* 1102c ~ 1103c */
    u_g1_gfx_src_info                    g1_gfx_src_info;                     /* 11040 */
    u_g1_gfx_src_reso                    g1_gfx_src_reso;                     /* 11044 */
    u_g1_gfx_src_crop                    g1_gfx_src_crop;                     /* 11048 */
    u_g1_gfx_ireso                       g1_gfx_ireso;                        /* 1104c */
    u_g1_gfx_addr_h                      g1_gfx_addr_h;                       /* 11050 */
    u_g1_gfx_addr_l                      g1_gfx_addr_l;                       /* 11054 */
    unsigned int                         rsr68[2];                            /* 11058 ~ 1105c */
    u_g1_gfx_stride                      g1_gfx_stride;                       /* 11060 */
    u_g1_gfx_addr_h_c                    g1_gfx_addr_h_c;                     /* 11064 */
    u_g1_gfx_addr_l_c                    g1_gfx_addr_l_c;                     /* 11068 */
    u_g1_gfx_stride_c                    g1_gfx_stride_c;                     /* 1106c */
    u_g1_dcmp_addr_h                     g1_dcmp_addr_h;                      /* 11070 */
    u_g1_dcmp_addr_l                     g1_dcmp_addr_l;                      /* 11074 */
    unsigned int                         rsr69[14];                           /* 11078 ~ 110ac */
    u_g1_gfx_testpat_cfg                 g1_gfx_testpat_cfg;                  /* 110b0 */
    u_g1_gfx_testpat_seed                g1_gfx_testpat_seed;                 /* 110b4 */
    u_g1_dcmp_ctrl                       g1_dcmp_ctrl;                        /* 110b8 */
    unsigned int                         rsr70;                               /* 110bc */
    u_g1_gfx_debug_ctrl                  g1_gfx_debug_ctrl;                   /* 110c0 */
    unsigned int                         rsr71;                               /* 110c4 */
    u_g1_htc_online_dcmp0                g1_htc_online_dcmp0;                 /* 110c8 */
    u_g1_htc_online_dcmp1                g1_htc_online_dcmp1;                 /* 110cc */
    unsigned int                         rsr72[4];                            /* 110d0 ~ 110dc */
    u_g1_gfx_in_ar_checksum0             g1_gfx_in_ar_checksum0;              /* 110e0 */
    u_g1_gfx_in_ar_checksum1             g1_gfx_in_ar_checksum1;              /* 110e4 */
    u_g1_gfx_in_gb_checksum0             g1_gfx_in_gb_checksum0;              /* 110e8 */
    u_g1_gfx_in_gb_checksum1             g1_gfx_in_gb_checksum1;              /* 110ec */
    unsigned int                         rsr73[4];                            /* 110f0 ~ 110fc */
    u_g1_gfx_ckey_max                    g1_gfx_ckey_max;                     /* 11100 */
    u_g1_gfx_ckey_min                    g1_gfx_ckey_min;                     /* 11104 */
    u_g1_gfx_ckey_mask                   g1_gfx_ckey_mask;                    /* 11108 */
    unsigned int                         rsr74[861];                          /* 1110c ~ 11e7c */
    u_g1_cmp_reso_debug0                 g1_cmp_reso_debug0;                  /* 11e80 */
    u_g1_cmp_reso_debug1                 g1_cmp_reso_debug1;                  /* 11e84 */
    u_g1_cmp_reso_debug2                 g1_cmp_reso_debug2;                  /* 11e88 */
    unsigned int                         rsr75[61];                           /* 11e8c ~ 11f7c */
    u_g1_htc_online_dcmp_debug0          g1_htc_online_dcmp_debug0;           /* 11f80 */
    u_g1_htc_online_dcmp_debug1          g1_htc_online_dcmp_debug1;           /* 11f84 */
    u_g1_htc_online_dcmp_debug2          g1_htc_online_dcmp_debug2;           /* 11f88 */
    u_g1_htc_online_dcmp_debug3          g1_htc_online_dcmp_debug3;           /* 11f8c */
    unsigned int                         rsr76[412];                          /* 11f90 ~ 125fc */
    u_wbc_dhd_ctrl                       wbc_dhd_ctrl;                        /* 12600 */
    u_wbc_dhd_addr                       wbc_dhd_addr;                        /* 12604 */
    u_wbc_dhd_stride                     wbc_dhd_stride;                      /* 12608 */
    u_wbc_dhd_imgsize                    wbc_dhd_imgsize;                     /* 1260c */
    unsigned int                         rsr77[3];                            /* 12610 ~ 12618 */
    u_wbc_dhd_cmd_ctrl                   wbc_dhd_cmd_ctrl;                    /* 1261c */
} dpu_regs_type;

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* DRV_GRAPHIC_HAL_DPU_REG_DEFINE_H */
