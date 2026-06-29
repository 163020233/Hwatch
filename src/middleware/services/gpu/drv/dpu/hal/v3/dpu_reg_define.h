/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description : dpu reg define file
 * version : A50 99NL
 */

#ifndef DRV_GRAPHIC_HAL_DPU_REG_DEFINE_H
#define DRV_GRAPHIC_HAL_DPU_REG_DEFINE_H

#include "td_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define DPU_REG_BASEADDR                0x53580000

#define HAL_DPU_SOFTRST_STATE           (DPU_REG_BASEADDR + 0x70) // DPU软复位状态 查询为1表示处于复位状态
#define HAL_DPU_SOFTRST_VAL             1

#define dpu_reg_read(addr)              (*(volatile unsigned int *)(uintptr_t)(addr))
#define dpu_reg_write(addr, val)        (*(volatile unsigned int *)(uintptr_t)(addr) = (val))

/*
 * DEFINE REGISTER UNION
 */
/* Define the union voctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved0               : 19;      /* [18:0] */
        unsigned int clk_axi_ck_gt_en        : 1;       /* [19] */
        unsigned int test_pin_en             : 1;       /* [20] */
        unsigned int test_pin_sel            : 2;       /* [22:21] */
        unsigned int clk_ppc_para_en         : 1;       /* [23] */
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
        unsigned int dhd_vtthd1_int          : 1;       /* [0] */
        unsigned int dhd_vtthd2_int          : 1;       /* [1] */
        unsigned int dhd_vtthd3_int          : 1;       /* [2] */
        unsigned int dhd_uf_int              : 1;       /* [3] */
        unsigned int reserved0               : 8;       /* [11:4] */
        unsigned int bus_err_int             : 1;       /* [12] */
        unsigned int reserved1               : 7;       /* [19:13] */
        unsigned int dhd_te_int              : 1;       /* [20] */
        unsigned int dhd_frm_end_int         : 1;       /* [21] */
        unsigned int reserved2               : 1;       /* [22] */
        unsigned int wbc_dhd_sta_int         : 1;       /* [23] */
        unsigned int reserved3               : 1;       /* [24] */
        unsigned int online_dcmp_bs_err_int  : 1;       /* [25] */
        unsigned int online_dcmp_cfg_err_int : 1;       /* [26] */
        unsigned int reserved4               : 5;       /* [31:27] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vointsta;

/* Define the union vomskintsta */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_vtthd1_clr          : 1;       /* [0] */
        unsigned int dhd_vtthd2_clr          : 1;       /* [1] */
        unsigned int dhd_vtthd3_clr          : 1;       /* [2] */
        unsigned int dhd_uf_clr              : 1;       /* [3] */
        unsigned int reserved0               : 8;       /* [11:4] */
        unsigned int bus_err_clr             : 1;       /* [12] */
        unsigned int reserved1               : 7;       /* [19:13] */
        unsigned int dhd_te_clr              : 1;       /* [20] */
        unsigned int dhd_frm_end_clr         : 1;       /* [21] */
        unsigned int reserved2               : 1;       /* [22] */
        unsigned int wbc_dhd_sta_clr         : 1;       /* [23] */
        unsigned int reserved3               : 1;       /* [24] */
        unsigned int online_dcmp_bs_err_clr  : 1;       /* [25] */
        unsigned int online_dcmp_cfg_err_clr : 1;       /* [26] */
        unsigned int reserved4               : 5;       /* [31:27] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vomskintsta;

/* Define the union vointmsk */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_vtthd1_intmask      : 1;       /* [0] */
        unsigned int dhd_vtthd2_intmask      : 1;       /* [1] */
        unsigned int dhd_vtthd3_intmask      : 1;       /* [2] */
        unsigned int dhd_uf_intmask          : 1;       /* [3] */
        unsigned int reserved0               : 8;       /* [11:4] */
        unsigned int bus_err_intmask         : 1;       /* [12] */
        unsigned int reserved1               : 7;       /* [19:13] */
        unsigned int dhd_te_intmask          : 1;       /* [20] */
        unsigned int dhd_frm_end_intmask     : 1;       /* [21] */
        unsigned int reserved2               : 1;       /* [22] */
        unsigned int wbc_dhd_sta_intmask     : 1;       /* [23] */
        unsigned int reserved3               : 1;       /* [24] */
        unsigned int online_dcmp_bs_err_intmask : 1;       /* [25] */
        unsigned int online_dcmp_cfg_err_intmask : 1;       /* [26] */
        unsigned int reserved4               : 5;       /* [31:27] */
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
        unsigned int dhd_cmd_int             : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vocmd_intsta;

/* Define the union vocmd_mskintsta */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_cmd_clr             : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vocmd_mskintsta;

/* Define the union vocmd_intmsk */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_cmd_intmask         : 1;       /* [0] */
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

/* Define the union dpu_test_pin */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dpu_test_pin            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_test_pin;

/* Define the union dpu_frm_num_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int frm_num_cnt             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_frm_num_cnt;

/* Define the union vosrststa */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int srst_sta                : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vosrststa;

/* Define the union dpu2mipi_info_0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dpu2mipi_info_0         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu2mipi_info_0;

/* Define the union dpu2mipi_info_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dpu2mipi_info_1         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu2mipi_info_1;

/* Define the union dpu2mipi_info_2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dpu2mipi_info_2         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu2mipi_info_2;

/* Define the union dpu2mipi_info_3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dpu2mipi_info_3         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu2mipi_info_3;

/* Define the union dpu2mipi_info_4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dpu2mipi_info_4         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu2mipi_info_4;

/* Define the union dpu2mipi_info_5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dpu2mipi_info_5         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu2mipi_info_5;

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
        unsigned int reserved1               : 1;       /* [4] */
        unsigned int bitext_mode             : 2;       /* [6:5] */
        unsigned int reso_cfg_en             : 1;       /* [7] */
        unsigned int reserved2               : 23;      /* [30:8] */
        unsigned int g0_bypass_dhd0_en       : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_link_ctrl;

/* Define the union vid_link_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vid_g0_bypass_dhd0_en   : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vid_link_ctrl;

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

/* Define the union vid_g0_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vid_surface_en          : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vid_g0_ctrl;

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

/* Define the union g0_add_pix_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int add_pix_en              : 1;       /* [0] */
        unsigned int add_pix_cfg_en          : 1;       /* [1] */
        unsigned int reserved                : 30;      /* [31:2] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g0_add_pix_ctrl;

/* Define the union g0_add_pix_small */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int small_height            : 16;      /* [15:0] */
        unsigned int small_width             : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g0_add_pix_small;

/* Define the union g0_add_pix_big */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int big_height              : 16;      /* [15:0] */
        unsigned int big_width               : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g0_add_pix_big;

/* Define the union g0_add_pix_rect */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int rect_y                  : 16;      /* [15:0] */
        unsigned int rect_x                  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g0_add_pix_rect;

/* Define the union g0_add_pix_tile */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tile_height             : 2;       /* [1:0] */
        unsigned int tile_width              : 4;       /* [5:2] */
        unsigned int reserved                : 26;      /* [31:6] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g0_add_pix_tile;

/* Define the union g0_pbi_crop0_wh */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pbi_crop_width          : 16;      /* [15:0] */
        unsigned int pbi_crop_height         : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g0_pbi_crop0_wh;

/* Define the union g0_pbi_crop0_xy */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pbi_crop_x              : 13;      /* [12:0] */
        unsigned int reserved0               : 3;       /* [15:13] */
        unsigned int pbi_crop_y              : 13;      /* [28:16] */
        unsigned int reserved1               : 1;       /* [29] */
        unsigned int pbi_crop_cfg_en         : 1;       /* [30] */
        unsigned int pbi_crop_en             : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g0_pbi_crop0_xy;

/* Define the union g0_pbi_crop0_in_wh */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pbi_crop_in_width       : 16;      /* [15:0] */
        unsigned int pbi_crop_in_height      : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g0_pbi_crop0_in_wh;

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

/* Define the union vid_g1_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vid_surface_en          : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vid_g1_ctrl;

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

/* Define the union g1_add_pix_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int add_pix_en              : 1;       /* [0] */
        unsigned int add_pix_cfg_en          : 1;       /* [1] */
        unsigned int reserved                : 30;      /* [31:2] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_add_pix_ctrl;

/* Define the union g1_add_pix_small */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int small_height            : 16;      /* [15:0] */
        unsigned int small_width             : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_add_pix_small;

/* Define the union g1_add_pix_big */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int big_height              : 16;      /* [15:0] */
        unsigned int big_width               : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_add_pix_big;

/* Define the union g1_add_pix_rect */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int rect_y                  : 16;      /* [15:0] */
        unsigned int rect_x                  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_add_pix_rect;

/* Define the union g1_add_pix_tile */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tile_height             : 2;       /* [1:0] */
        unsigned int tile_width              : 4;       /* [5:2] */
        unsigned int reserved                : 26;      /* [31:6] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_add_pix_tile;

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
        unsigned int pack_mode               : 1;       /* [12] */
        unsigned int reserved0               : 15;      /* [27:13] */
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

/* Define the union dhd_1_csc_ctrl */
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
} u_dhd_1_csc_ctrl;

/* Define the union dhd_1_csc_coef00 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_coef00              : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_1_csc_coef00;

/* Define the union dhd_1_csc_coef01 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_coef01              : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_1_csc_coef01;

/* Define the union dhd_1_csc_coef02 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_coef02              : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_1_csc_coef02;

/* Define the union dhd_1_csc_coef10 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_coef10              : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_1_csc_coef10;

/* Define the union dhd_1_csc_coef11 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_coef11              : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_1_csc_coef11;

/* Define the union dhd_1_csc_coef12 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_coef12              : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_1_csc_coef12;

/* Define the union dhd_1_csc_coef20 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_coef20              : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_1_csc_coef20;

/* Define the union dhd_1_csc_coef21 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_coef21              : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_1_csc_coef21;

/* Define the union dhd_1_csc_coef22 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_coef22              : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_1_csc_coef22;

/* Define the union dhd_1_csc_scale */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_scale               : 4;       /* [3:0] */
        unsigned int reserved                : 28;      /* [31:4] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_1_csc_scale;

/* Define the union dhd_1_csc_idc0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_idc0                : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_1_csc_idc0;

/* Define the union dhd_1_csc_idc1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_idc1                : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_1_csc_idc1;

/* Define the union dhd_1_csc_idc2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_idc2                : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_1_csc_idc2;

/* Define the union dhd_1_csc_odc0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_odc0                : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_1_csc_odc0;

/* Define the union dhd_1_csc_odc1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_odc1                : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_1_csc_odc1;

/* Define the union dhd_1_csc_odc2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_odc2                : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_1_csc_odc2;

/* Define the union dhd_1_csc_min_y */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_min_y               : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_1_csc_min_y;

/* Define the union dhd_1_csc_min_c */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_min_c               : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_1_csc_min_c;

/* Define the union dhd_1_csc_max_y */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_max_y               : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_1_csc_max_y;

/* Define the union dhd_1_csc_max_c */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_max_c               : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_1_csc_max_c;

/* Define the union dhd_1_csc2_coef00 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_coef00             : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_1_csc2_coef00;

/* Define the union dhd_1_csc2_coef01 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_coef01             : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_1_csc2_coef01;

/* Define the union dhd_1_csc2_coef02 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_coef02             : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_1_csc2_coef02;

/* Define the union dhd_1_csc2_coef10 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_coef10             : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_1_csc2_coef10;

/* Define the union dhd_1_csc2_coef11 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_coef11             : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_1_csc2_coef11;

/* Define the union dhd_1_csc2_coef12 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_coef12             : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_1_csc2_coef12;

/* Define the union dhd_1_csc2_coef20 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_coef20             : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_1_csc2_coef20;

/* Define the union dhd_1_csc2_coef21 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_coef21             : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_1_csc2_coef21;

/* Define the union dhd_1_csc2_coef22 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_coef22             : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_1_csc2_coef22;

/* Define the union dhd_1_csc2_scale */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_scale              : 4;       /* [3:0] */
        unsigned int reserved                : 28;      /* [31:4] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_1_csc2_scale;

/* Define the union dhd_1_csc2_idc0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_idc0               : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_1_csc2_idc0;

/* Define the union dhd_1_csc2_idc1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_idc1               : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_1_csc2_idc1;

/* Define the union dhd_1_csc2_idc2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_idc2               : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_1_csc2_idc2;

/* Define the union dhd_1_csc2_odc0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_odc0               : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_1_csc2_odc0;

/* Define the union dhd_1_csc2_odc1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_odc1               : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_1_csc2_odc1;

/* Define the union dhd_1_csc2_odc2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_odc2               : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_1_csc2_odc2;

/* Define the union dhd_1_csc2_min_y */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_min_y              : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_1_csc2_min_y;

/* Define the union dhd_1_csc2_min_c */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_min_c              : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_1_csc2_min_c;

/* Define the union dhd_1_csc2_max_y */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_max_y              : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_1_csc2_max_y;

/* Define the union dhd_1_csc2_max_c */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_max_c              : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_1_csc2_max_c;

/* Define the union dhd_1_csc_ink_ctrl */
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
} u_dhd_1_csc_ink_ctrl;

/* Define the union dhd_1_csc_ink_pos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int x_pos                   : 16;      /* [15:0] */
        unsigned int y_pos                   : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_1_csc_ink_pos;

/* Define the union dhd_1_csc_ink_data */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ink_data                : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_1_csc_ink_data;

/* Define the union dhd_1_csc_ink_data2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ink_data2               : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_1_csc_ink_data2;

/* Define the union dhd_2_csc_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_2_csc_en            : 1;       /* [0] */
        unsigned int dhd_2_csc_demo_en       : 1;       /* [1] */
        unsigned int dhd_2_csc_ck_gt_en      : 1;       /* [2] */
        unsigned int reserved                : 29;      /* [31:3] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_2_csc_ctrl;

/* Define the union dhd_2_csc_coef00 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_2_csc_coef00        : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_2_csc_coef00;

/* Define the union dhd_2_csc_coef01 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_2_csc_coef01        : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_2_csc_coef01;

/* Define the union dhd_2_csc_coef02 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_2_csc_coef02        : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_2_csc_coef02;

/* Define the union dhd_2_csc_coef10 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_2_csc_coef10        : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_2_csc_coef10;

/* Define the union dhd_2_csc_coef11 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_2_csc_coef11        : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_2_csc_coef11;

/* Define the union dhd_2_csc_coef12 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_2_csc_coef12        : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_2_csc_coef12;

/* Define the union dhd_2_csc_coef20 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_2_csc_coef20        : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_2_csc_coef20;

/* Define the union dhd_2_csc_coef21 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_2_csc_coef21        : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_2_csc_coef21;

/* Define the union dhd_2_csc_coef22 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_2_csc_coef22        : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_2_csc_coef22;

/* Define the union dhd_2_csc_scale */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_2_csc_scale         : 4;       /* [3:0] */
        unsigned int reserved                : 28;      /* [31:4] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_2_csc_scale;

/* Define the union dhd_2_csc_idc0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_2_csc_idc0          : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_2_csc_idc0;

/* Define the union dhd_2_csc_idc1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_2_csc_idc1          : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_2_csc_idc1;

/* Define the union dhd_2_csc_idc2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_2_csc_idc2          : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_2_csc_idc2;

/* Define the union dhd_2_csc_odc0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_2_csc_odc0          : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_2_csc_odc0;

/* Define the union dhd_2_csc_odc1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_2_csc_odc1          : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_2_csc_odc1;

/* Define the union dhd_2_csc_odc2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_2_csc_odc2          : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_2_csc_odc2;

/* Define the union dhd_2_csc_min_y */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_2_csc_min_y         : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_2_csc_min_y;

/* Define the union dhd_2_csc_min_c */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_2_csc_min_c         : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_2_csc_min_c;

/* Define the union dhd_2_csc_max_y */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_2_csc_max_y         : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_2_csc_max_y;

/* Define the union dhd_2_csc_max_c */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_2_csc_max_c         : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_2_csc_max_c;

/* Define the union dhd_2_csc2_coef00 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_2_csc2_coef00       : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_2_csc2_coef00;

/* Define the union dhd_2_csc2_coef01 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_2_csc2_coef01       : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_2_csc2_coef01;

/* Define the union dhd_2_csc2_coef02 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_2_csc2_coef02       : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_2_csc2_coef02;

/* Define the union dhd_2_csc2_coef10 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_2_csc2_coef10       : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_2_csc2_coef10;

/* Define the union dhd_2_csc2_coef11 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_2_csc2_coef11       : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_2_csc2_coef11;

/* Define the union dhd_2_csc2_coef12 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_2_csc2_coef12       : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_2_csc2_coef12;

/* Define the union dhd_2_csc2_coef20 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_2_csc2_coef20       : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_2_csc2_coef20;

/* Define the union dhd_2_csc2_coef21 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_2_csc2_coef21       : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_2_csc2_coef21;

/* Define the union dhd_2_csc2_coef22 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_2_csc2_coef22       : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_2_csc2_coef22;

/* Define the union dhd_2_csc2_scale */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_2_csc2_scale        : 4;       /* [3:0] */
        unsigned int reserved                : 28;      /* [31:4] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_2_csc2_scale;

/* Define the union dhd_2_csc2_idc0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_2_csc2_idc0         : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_2_csc2_idc0;

/* Define the union dhd_2_csc2_idc1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_2_csc2_idc1         : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_2_csc2_idc1;

/* Define the union dhd_2_csc2_idc2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_2_csc2_idc2         : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_2_csc2_idc2;

/* Define the union dhd_2_csc2_odc0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_2_csc2_odc0         : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_2_csc2_odc0;

/* Define the union dhd_2_csc2_odc1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_2_csc2_odc1         : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_2_csc2_odc1;

/* Define the union dhd_2_csc2_odc2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_2_csc2_odc2         : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_2_csc2_odc2;

/* Define the union dhd_2_csc2_min_y */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_2_csc2_min_y        : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_2_csc2_min_y;

/* Define the union dhd_2_csc2_min_c */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_2_csc2_min_c        : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_2_csc2_min_c;

/* Define the union dhd_2_csc2_max_y */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_2_csc2_max_y        : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_2_csc2_max_y;

/* Define the union dhd_2_csc2_max_c */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_2_csc2_max_c        : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_2_csc2_max_c;

/* Define the union dhd_2_csc_ink_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_2_csc_ink_en        : 1;       /* [0] */
        unsigned int dhd_2_csc_ink_sel       : 1;       /* [1] */
        unsigned int dhd_2_csc_data_fmt      : 1;       /* [2] */
        unsigned int dhd_2_csc_cross_enable  : 1;       /* [3] */
        unsigned int dhd_2_csc_color_mode    : 2;       /* [5:4] */
        unsigned int reserved                : 26;      /* [31:6] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_2_csc_ink_ctrl;

/* Define the union dhd_2_csc_ink_pos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_2_csc_x_pos         : 16;      /* [15:0] */
        unsigned int dhd_2_csc_y_pos         : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_2_csc_ink_pos;

/* Define the union dhd_2_csc_ink_data */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_2_csc_ink_data      : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_2_csc_ink_data;

/* Define the union dhd_2_csc_ink_data2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_2_csc_ink_data2     : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_2_csc_ink_data2;

/* Define the union dhd_arsr_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int arsr_en                 : 1;       /* [0] */
        unsigned int arsr_ck_gt_en           : 1;       /* [1] */
        unsigned int in_format               : 2;       /* [3:2] */
        unsigned int no_intpl_en             : 1;       /* [4] */
        unsigned int skin_ctrl_en            : 1;       /* [5] */
        unsigned int shoot_detect_en         : 1;       /* [6] */
        unsigned int texture_analysis_en     : 1;       /* [7] */
        unsigned int sharpen_en              : 1;       /* [8] */
        unsigned int dir_intpl_en            : 1;       /* [9] */
        unsigned int corner_detect_en        : 1;       /* [10] */
        unsigned int dir_sharpen_sel         : 1;       /* [11] */
        unsigned int dir_graph_detect_en     : 1;       /* [12] */
        unsigned int hf_select               : 1;       /* [13] */
        unsigned int strong_hv_intpl         : 1;       /* [14] */
        unsigned int maxmin_sel              : 1;       /* [15] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_arsr_cfg;

/* Define the union dhd_arsr_lut_rd_en */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int arsr_lut_rd_en          : 1;       /* [0] */
        unsigned int para_load_error_clr     : 1;       /* [1] */
        unsigned int reserved                : 30;      /* [31:2] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_arsr_lut_rd_en;

/* Define the union dhd_arsr_lut_rdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int arsr_lut_rdata          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_arsr_lut_rdata;

/* Define the union dhd_arsr_reso_in */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int in_width                : 16;      /* [15:0] */
        unsigned int in_height               : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_arsr_reso_in;

/* Define the union dhd_arsr_reso_out */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int out_width               : 16;      /* [15:0] */
        unsigned int out_height              : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_arsr_reso_out;

/* Define the union dhd_arsr_h_inc */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ih_inc                  : 19;      /* [18:0] */
        unsigned int reserved                : 13;      /* [31:19] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_arsr_h_inc;

/* Define the union dhd_arsr_v_inc */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int iv_inc                  : 19;      /* [18:0] */
        unsigned int reserved                : 13;      /* [31:19] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_arsr_v_inc;

/* Define the union dhd_arsr_in_h_left */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ih_left                 : 17;      /* [16:0] */
        unsigned int reserved                : 15;      /* [31:17] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_arsr_in_h_left;

/* Define the union dhd_arsr_in_h_right */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ih_right                : 28;      /* [27:0] */
        unsigned int reserved                : 4;       /* [31:28] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_arsr_in_h_right;

/* Define the union dhd_arsr_in_v_top */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int iv_top                  : 17;      /* [16:0] */
        unsigned int reserved                : 15;      /* [31:17] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_arsr_in_v_top;

/* Define the union dhd_arsr_in_v_bottom */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int iv_bottom               : 28;      /* [27:0] */
        unsigned int reserved                : 4;       /* [31:28] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_arsr_in_v_bottom;

/* Define the union dhd_arsr_expect_value */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int y_expect_value          : 10;      /* [9:0] */
        unsigned int u_expect_value          : 10;      /* [19:10] */
        unsigned int v_expect_value          : 10;      /* [29:20] */
        unsigned int reserved                : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_arsr_expect_value;

/* Define the union dhd_arsr_thres_l */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int y_thres_l               : 10;      /* [9:0] */
        unsigned int u_thres_l               : 10;      /* [19:10] */
        unsigned int v_thres_l               : 10;      /* [29:20] */
        unsigned int reserved                : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_arsr_thres_l;

/* Define the union dhd_arsr_thres_h */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int y_thres_h               : 10;      /* [9:0] */
        unsigned int u_thres_h               : 10;      /* [19:10] */
        unsigned int v_thres_h               : 10;      /* [29:20] */
        unsigned int reserved                : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_arsr_thres_h;

/* Define the union dhd_arsr_slop_0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int y_slop                  : 12;      /* [11:0] */
        unsigned int u_slop                  : 12;      /* [23:12] */
        unsigned int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_arsr_slop_0;

/* Define the union dhd_arsr_slop_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int v_slop                  : 12;      /* [11:0] */
        unsigned int reserved0               : 4;       /* [15:12] */
        unsigned int clip_data               : 8;       /* [23:16] */
        unsigned int reserved1               : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_arsr_slop_1;

/* Define the union dhd_arsr_shoot_min */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int shoot_min_l             : 10;      /* [9:0] */
        unsigned int shoot_min_h             : 10;      /* [19:10] */
        unsigned int reserved                : 12;      /* [31:20] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_arsr_shoot_min;

/* Define the union dhd_arsr_shoot_max */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int shoot_max_l             : 10;      /* [9:0] */
        unsigned int shoot_max_h             : 10;      /* [19:10] */
        unsigned int shoot_shp_ratio         : 5;       /* [24:20] */
        unsigned int reserved                : 7;       /* [31:25] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_arsr_shoot_max;

/* Define the union dhd_arsr_shoot_slop */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int shoot_min_slop          : 11;      /* [10:0] */
        unsigned int shoot_max_slop          : 11;      /* [21:11] */
        unsigned int reserved                : 10;      /* [31:22] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_arsr_shoot_slop;

/* Define the union dhd_arsr_diff_low_high */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int diff_low                : 7;       /* [6:0] */
        unsigned int diff_high               : 7;       /* [13:7] */
        unsigned int reserved                : 18;      /* [31:14] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_arsr_diff_low_high;

/* Define the union dhd_arsr_corner_thres */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int corner_thres_l          : 10;      /* [9:0] */
        unsigned int corner_thres_h          : 10;      /* [19:10] */
        unsigned int reserved                : 12;      /* [31:20] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_arsr_corner_thres;

/* Define the union dhd_arsr_dir_dif_max */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dir_dif_max_thres_l     : 10;      /* [9:0] */
        unsigned int dir_dif_max_thres_h     : 10;      /* [19:10] */
        unsigned int dir_dif_max_slop        : 11;      /* [30:20] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_arsr_dir_dif_max;

/* Define the union dhd_arsr_dir_hv_dif_min */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dir_hv_dif_min_thres_l  : 10;      /* [9:0] */
        unsigned int dir_hv_dif_min_thres_h  : 10;      /* [19:10] */
        unsigned int dir_hv_dif_min_slop     : 10;      /* [29:20] */
        unsigned int reserved                : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_arsr_dir_hv_dif_min;

/* Define the union dhd_arsr_dir_hv_dif_max */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dir_hv_dif_max_thres_l  : 10;      /* [9:0] */
        unsigned int dir_hv_dif_max_thres_h  : 10;      /* [19:10] */
        unsigned int dir_hv_dif_max_slop     : 10;      /* [29:20] */
        unsigned int reserved                : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_arsr_dir_hv_dif_max;

/* Define the union dhd_arsr_sharp_level */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sharp_level_hf          : 3;       /* [2:0] */
        unsigned int sharp_level_mf          : 3;       /* [5:3] */
        unsigned int hv_prob_inc_ratio       : 3;       /* [8:6] */
        unsigned int reserved                : 23;      /* [31:9] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_arsr_sharp_level;

/* Define the union dhd_arsr_sharp_gain_0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int shp_dir_gain_hf         : 5;       /* [4:0] */
        unsigned int shp_dir_gain_mf         : 5;       /* [9:5] */
        unsigned int shp_gain_var_low_0      : 10;      /* [19:10] */
        unsigned int shp_gain_var_low_1      : 10;      /* [29:20] */
        unsigned int reserved                : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_arsr_sharp_gain_0;

/* Define the union dhd_arsr_sharp_gain_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int shp_gain_var_high_0     : 10;      /* [9:0] */
        unsigned int shp_gain_var_high_1     : 10;      /* [19:10] */
        unsigned int reserved                : 12;      /* [31:20] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_arsr_sharp_gain_1;

/* Define the union dhd_arsr_sharp_gain_2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int shp_gain_low_mf         : 11;      /* [10:0] */
        unsigned int shp_gain_mid_mf         : 11;      /* [21:11] */
        unsigned int reserved                : 10;      /* [31:22] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_arsr_sharp_gain_2;

/* Define the union dhd_arsr_sharp_gain_3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int shp_gain_high_mf        : 11;      /* [10:0] */
        unsigned int shp_gain_low_hf         : 11;      /* [21:11] */
        unsigned int reserved                : 10;      /* [31:22] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_arsr_sharp_gain_3;

/* Define the union dhd_arsr_sharp_gain_4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int shp_gain_mid_hf         : 11;      /* [10:0] */
        unsigned int shp_gain_high_hf        : 11;      /* [21:11] */
        unsigned int reserved                : 10;      /* [31:22] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_arsr_sharp_gain_4;

/* Define the union dhd_arsr_gain_ctrl_slop_l_mf */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gain_ctrl_slop_l_mf     : 21;      /* [20:0] */
        unsigned int reserved                : 11;      /* [31:21] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_arsr_gain_ctrl_slop_l_mf;

/* Define the union dhd_arsr_gain_ctrl_slop_h_mf */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gain_ctrl_slop_h_mf     : 21;      /* [20:0] */
        unsigned int reserved                : 11;      /* [31:21] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_arsr_gain_ctrl_slop_h_mf;

/* Define the union dhd_arsr_gain_ctrl_slop_l_hf */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gain_ctrl_slop_l_hf     : 21;      /* [20:0] */
        unsigned int reserved                : 11;      /* [31:21] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_arsr_gain_ctrl_slop_l_hf;

/* Define the union dhd_arsr_gain_ctrl_slop_h_hf */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gain_ctrl_slop_h_hf     : 21;      /* [20:0] */
        unsigned int reserved                : 11;      /* [31:21] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_arsr_gain_ctrl_slop_h_hf;

/* Define the union dhd_arsr_sharp_gain_mf */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mf_lmt                  : 10;      /* [9:0] */
        unsigned int gain_mf_l               : 11;      /* [20:10] */
        unsigned int gain_mf_h               : 11;      /* [31:21] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_arsr_sharp_gain_mf;

/* Define the union dhd_arsr_sharp_gain_hf */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hf_lmt                  : 10;      /* [9:0] */
        unsigned int gain_hf_l               : 11;      /* [20:10] */
        unsigned int gain_hf_h               : 11;      /* [31:21] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_arsr_sharp_gain_hf;

/* Define the union dhd_arsr_mf_b */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mf_b                    : 22;      /* [21:0] */
        unsigned int reserved                : 10;      /* [31:22] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_arsr_mf_b;

/* Define the union dhd_arsr_hf_b */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hf_b                    : 22;      /* [21:0] */
        unsigned int reserved                : 10;      /* [31:22] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_arsr_hf_b;

/* Define the union dhd_arsr_shp_tocut_high */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int shp_tocuthigh0          : 10;      /* [9:0] */
        unsigned int shp_tocuthigh1          : 10;      /* [19:10] */
        unsigned int reserved                : 12;      /* [31:20] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_arsr_shp_tocut_high;

/* Define the union dhd_arsr_skin_thres */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int skin_thres_l            : 5;       /* [4:0] */
        unsigned int skin_thres_h            : 5;       /* [9:5] */
        unsigned int sharp_coring            : 7;       /* [16:10] */
        unsigned int reserved                : 15;      /* [31:17] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_arsr_skin_thres;

/* Define the union dhd_arsr_linear_sh_ratio */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int linear_sh_ratio_l       : 8;       /* [7:0] */
        unsigned int linear_sh_ratio_h       : 8;       /* [15:8] */
        unsigned int blend_sh_shoot_ctrl     : 7;       /* [22:16] */
        unsigned int reserved                : 9;       /* [31:23] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_arsr_linear_sh_ratio;

/* Define the union dhd_arsr_shoot_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sharp_shoot_ctrl_l      : 9;       /* [8:0] */
        unsigned int sharp_shoot_ctrl_h      : 9;       /* [17:9] */
        unsigned int intpl_shoot_ctrl        : 7;       /* [24:18] */
        unsigned int reserved                : 7;       /* [31:25] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_arsr_shoot_ctrl;

/* Define the union dhd_arsr_dir_diff_avg_thres01 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dir_diff_avg_thres0     : 10;      /* [9:0] */
        unsigned int dir_diff_avg_thres1     : 10;      /* [19:10] */
        unsigned int reserved                : 12;      /* [31:20] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_arsr_dir_diff_avg_thres01;

/* Define the union dhd_arsr_dir_diff_avg_thres23 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dir_diff_avg_thres2     : 10;      /* [9:0] */
        unsigned int dir_diff_avg_thres3     : 10;      /* [19:10] */
        unsigned int reserved                : 12;      /* [31:20] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_arsr_dir_diff_avg_thres23;

/* Define the union dhd_arsr_coef_ar_shp_mf */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int coef_ar_shp03_mf        : 8;       /* [7:0] */
        unsigned int coef_ar_shp45_mf        : 8;       /* [15:8] */
        unsigned int coef_ar_shp67_mf        : 8;       /* [23:16] */
        unsigned int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_arsr_coef_ar_shp_mf;

/* Define the union dhd_arsr_coef_ar_shp_hf */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int coef_ar_shp03_hf        : 8;       /* [7:0] */
        unsigned int coef_ar_shp45_hf        : 8;       /* [15:8] */
        unsigned int coef_ar_shp67_hf        : 8;       /* [23:16] */
        unsigned int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_arsr_coef_ar_shp_hf;

/* Define the union dhd_arsr_dfx_para_load */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int para_load_error         : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_arsr_dfx_para_load;

/* Define the union vhdaccctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int acc_en                  : 1;       /* [0] */
        unsigned int acc_ck_gt_en            : 1;       /* [1] */
        unsigned int input_full_range        : 1;       /* [2] */
        unsigned int output_full_range       : 1;       /* [3] */
        unsigned int color_comp_en           : 1;       /* [4] */
        unsigned int color_comp_all_gain_en  : 1;       /* [5] */
        unsigned int reserved                : 26;      /* [31:6] */
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

/* Define the union vhdacc_all_gain */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int color_comp_all_gain_pos : 9;       /* [8:0] */
        unsigned int reserved0               : 7;       /* [15:9] */
        unsigned int color_comp_all_gain_neg : 9;       /* [24:16] */
        unsigned int reserved1               : 7;       /* [31:25] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdacc_all_gain;

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

/* Define the union vhdacc_gain_pos_lut1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gain_pos_lut0           : 9;       /* [8:0] */
        unsigned int gain_pos_lut1           : 9;       /* [17:9] */
        unsigned int gain_pos_lut2           : 9;       /* [26:18] */
        unsigned int reserved                : 5;       /* [31:27] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdacc_gain_pos_lut1;

/* Define the union vhdacc_gain_pos_lut2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gain_pos_lut3           : 9;       /* [8:0] */
        unsigned int gain_pos_lut4           : 9;       /* [17:9] */
        unsigned int gain_pos_lut5           : 9;       /* [26:18] */
        unsigned int reserved                : 5;       /* [31:27] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdacc_gain_pos_lut2;

/* Define the union vhdacc_gain_pos_lut3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gain_pos_lut6           : 9;       /* [8:0] */
        unsigned int gain_pos_lut7           : 9;       /* [17:9] */
        unsigned int gain_pos_lut8           : 9;       /* [26:18] */
        unsigned int reserved                : 5;       /* [31:27] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdacc_gain_pos_lut3;

/* Define the union vhdacc_gain_pos_lut4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gain_pos_lut9           : 9;       /* [8:0] */
        unsigned int gain_pos_lut10          : 9;       /* [17:9] */
        unsigned int gain_pos_lut11          : 9;       /* [26:18] */
        unsigned int reserved                : 5;       /* [31:27] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdacc_gain_pos_lut4;

/* Define the union vhdacc_gain_pos_lut5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gain_pos_lut12          : 9;       /* [8:0] */
        unsigned int gain_pos_lut13          : 9;       /* [17:9] */
        unsigned int gain_pos_lut14          : 9;       /* [26:18] */
        unsigned int reserved                : 5;       /* [31:27] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdacc_gain_pos_lut5;

/* Define the union vhdacc_gain_pos_lut6 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gain_pos_lut15          : 9;       /* [8:0] */
        unsigned int gain_pos_lut16          : 9;       /* [17:9] */
        unsigned int gain_pos_lut17          : 9;       /* [26:18] */
        unsigned int reserved                : 5;       /* [31:27] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdacc_gain_pos_lut6;

/* Define the union vhdacc_gain_pos_lut7 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gain_pos_lut18          : 9;       /* [8:0] */
        unsigned int gain_pos_lut19          : 9;       /* [17:9] */
        unsigned int gain_pos_lut20          : 9;       /* [26:18] */
        unsigned int reserved                : 5;       /* [31:27] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdacc_gain_pos_lut7;

/* Define the union vhdacc_gain_pos_lut8 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gain_pos_lut21          : 9;       /* [8:0] */
        unsigned int gain_pos_lut22          : 9;       /* [17:9] */
        unsigned int gain_pos_lut23          : 9;       /* [26:18] */
        unsigned int reserved                : 5;       /* [31:27] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdacc_gain_pos_lut8;

/* Define the union vhdacc_gain_pos_lut9 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gain_pos_lut24          : 9;       /* [8:0] */
        unsigned int gain_pos_lut25          : 9;       /* [17:9] */
        unsigned int gain_pos_lut26          : 9;       /* [26:18] */
        unsigned int reserved                : 5;       /* [31:27] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdacc_gain_pos_lut9;

/* Define the union vhdacc_gain_pos_lut10 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gain_pos_lut27          : 9;       /* [8:0] */
        unsigned int gain_pos_lut28          : 9;       /* [17:9] */
        unsigned int gain_pos_lut29          : 9;       /* [26:18] */
        unsigned int reserved                : 5;       /* [31:27] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdacc_gain_pos_lut10;

/* Define the union vhdacc_gain_pos_lut11 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gain_pos_lut30          : 9;       /* [8:0] */
        unsigned int gain_pos_lut31          : 9;       /* [17:9] */
        unsigned int gain_pos_lut32          : 9;       /* [26:18] */
        unsigned int reserved                : 5;       /* [31:27] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdacc_gain_pos_lut11;

/* Define the union vhdacc_gain_neg_lut1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gain_neg_lut0           : 9;       /* [8:0] */
        unsigned int gain_neg_lut1           : 9;       /* [17:9] */
        unsigned int gain_neg_lut2           : 9;       /* [26:18] */
        unsigned int reserved                : 5;       /* [31:27] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdacc_gain_neg_lut1;

/* Define the union vhdacc_gain_neg_lut2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gain_neg_lut3           : 9;       /* [8:0] */
        unsigned int gain_neg_lut4           : 9;       /* [17:9] */
        unsigned int gain_neg_lut5           : 9;       /* [26:18] */
        unsigned int reserved                : 5;       /* [31:27] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdacc_gain_neg_lut2;

/* Define the union vhdacc_gain_neg_lut3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gain_neg_lut6           : 9;       /* [8:0] */
        unsigned int gain_neg_lut7           : 9;       /* [17:9] */
        unsigned int gain_neg_lut8           : 9;       /* [26:18] */
        unsigned int reserved                : 5;       /* [31:27] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdacc_gain_neg_lut3;

/* Define the union vhdacc_gain_neg_lut4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gain_neg_lut9           : 9;       /* [8:0] */
        unsigned int gain_neg_lut10          : 9;       /* [17:9] */
        unsigned int gain_neg_lut11          : 9;       /* [26:18] */
        unsigned int reserved                : 5;       /* [31:27] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdacc_gain_neg_lut4;

/* Define the union vhdacc_gain_neg_lut5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gain_neg_lut12          : 9;       /* [8:0] */
        unsigned int gain_neg_lut13          : 9;       /* [17:9] */
        unsigned int gain_neg_lut14          : 9;       /* [26:18] */
        unsigned int reserved                : 5;       /* [31:27] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdacc_gain_neg_lut5;

/* Define the union vhdacc_gain_neg_lut6 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gain_neg_lut15          : 9;       /* [8:0] */
        unsigned int gain_neg_lut16          : 9;       /* [17:9] */
        unsigned int gain_neg_lut17          : 9;       /* [26:18] */
        unsigned int reserved                : 5;       /* [31:27] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdacc_gain_neg_lut6;

/* Define the union vhdacc_gain_neg_lut7 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gain_neg_lut18          : 9;       /* [8:0] */
        unsigned int gain_neg_lut19          : 9;       /* [17:9] */
        unsigned int gain_neg_lut20          : 9;       /* [26:18] */
        unsigned int reserved                : 5;       /* [31:27] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdacc_gain_neg_lut7;

/* Define the union vhdacc_gain_neg_lut8 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gain_neg_lut21          : 9;       /* [8:0] */
        unsigned int gain_neg_lut22          : 9;       /* [17:9] */
        unsigned int gain_neg_lut23          : 9;       /* [26:18] */
        unsigned int reserved                : 5;       /* [31:27] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdacc_gain_neg_lut8;

/* Define the union vhdacc_gain_neg_lut9 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gain_neg_lut24          : 9;       /* [8:0] */
        unsigned int gain_neg_lut25          : 9;       /* [17:9] */
        unsigned int gain_neg_lut26          : 9;       /* [26:18] */
        unsigned int reserved                : 5;       /* [31:27] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdacc_gain_neg_lut9;

/* Define the union vhdacc_gain_neg_lut10 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gain_neg_lut27          : 9;       /* [8:0] */
        unsigned int gain_neg_lut28          : 9;       /* [17:9] */
        unsigned int gain_neg_lut29          : 9;       /* [26:18] */
        unsigned int reserved                : 5;       /* [31:27] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdacc_gain_neg_lut10;

/* Define the union vhdacc_gain_neg_lut11 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gain_neg_lut30          : 9;       /* [8:0] */
        unsigned int gain_neg_lut31          : 9;       /* [17:9] */
        unsigned int gain_neg_lut32          : 9;       /* [26:18] */
        unsigned int reserved                : 5;       /* [31:27] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdacc_gain_neg_lut11;

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

/* Define the union vhdacc_power_lut1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int acc_power_lut0          : 8;       /* [7:0] */
        unsigned int acc_power_lut1          : 8;       /* [15:8] */
        unsigned int acc_power_lut2          : 8;       /* [23:16] */
        unsigned int acc_power_lut3          : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdacc_power_lut1;

/* Define the union vhdacc_power_lut2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int acc_power_lut4          : 8;       /* [7:0] */
        unsigned int acc_power_lut5          : 8;       /* [15:8] */
        unsigned int acc_power_lut6          : 8;       /* [23:16] */
        unsigned int acc_power_lut7          : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdacc_power_lut2;

/* Define the union vhdacc_power_lut3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int acc_power_lut8          : 8;       /* [7:0] */
        unsigned int reserved                : 24;      /* [31:8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdacc_power_lut3;

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

/* Define the union vhdacc_total_y_sum */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int acc_total_y_sum         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdacc_total_y_sum;

/* Define the union vhdacc_total_pw_sum */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int acc_total_pw_sum        : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vhdacc_total_pw_sum;

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

/* Define the union dhd_crop0_wh */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_crop_width          : 16;      /* [15:0] */
        unsigned int dhd_crop_height         : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_crop0_wh;

/* Define the union dhd_crop0_xy */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_crop_x              : 13;      /* [12:0] */
        unsigned int reserved0               : 3;       /* [15:13] */
        unsigned int dhd_crop_y              : 13;      /* [28:16] */
        unsigned int reserved1               : 1;       /* [29] */
        unsigned int dhd_crop_cfg_en         : 1;       /* [30] */
        unsigned int dhd_crop_en             : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_crop0_xy;

/* Define the union dhd_crop0_in_wh */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_crop_in_width       : 16;      /* [15:0] */
        unsigned int dhd_crop_in_height      : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd_crop0_in_wh;

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
        unsigned int intf_pcnt_en            : 1;       /* [12] */
        unsigned int reserved1               : 4;       /* [16:13] */
        unsigned int mipi_en                 : 1;       /* [17] */
        unsigned int cbar_mode               : 1;       /* [18] */
        unsigned int reserved2               : 10;      /* [28:19] */
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

/* Define the union dhd0_intf_pcnt_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int intf_pcnt_cnt           : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_intf_pcnt_cnt;

/* Define the union dhd0_intf_pcnt_cnt_real */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int intf_pcnt_cnt_real      : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_intf_pcnt_cnt_real;

/* Define the union dhd0_intf_pcnt_real */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int intf_pcnt_real          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_intf_pcnt_real;

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

/* Define the union dhd0_intf_pcnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int intf_pcnt               : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_intf_pcnt;

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
        unsigned int reserved0               : 4;       /* [7:4] */
        unsigned int bt_sel                  : 4;       /* [11:8] */
        unsigned int reserved1               : 4;       /* [15:12] */
        unsigned int reserved2               : 4;       /* [19:16] */
        unsigned int reserved3               : 4;       /* [23:20] */
        unsigned int reserved4               : 4;       /* [27:24] */
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

/* Define the union intf0_frm_rdy_clr_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int frm_rdy_clr_en          : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_intf0_frm_rdy_clr_ctrl;

/* Define the union intf0_frm_rdy_clr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int frm_rdy_clr             : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_intf0_frm_rdy_clr;

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
        unsigned int burst_type              : 2;       /* [10:9] */
        unsigned int reserved0               : 5;       /* [15:11] */
        unsigned int reserved1               : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mac_ctrl;

/* Define the union mac_rchn_prio */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int rchn0_prio              : 1;       /* [0] */
        unsigned int rchn1_prio              : 1;       /* [1] */
        unsigned int rchn2_prio              : 1;       /* [2] */
        unsigned int reserved                : 29;      /* [31:3] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mac_rchn_prio;

/* Define the union mac_wchn_prio */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int wchn0_prio              : 1;       /* [0] */
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
        unsigned int reserved                : 31;      /* [31:1] */
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

/* Define the union mst_det_latency */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int axi_mst_latency         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mst_det_latency;

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

/* Define the union mac_rd_bus_err_rd_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mst_rd_error_addr       : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mac_rd_bus_err_rd_addr;

/* Define the union mac_rd_bus_err_wr_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mst_wr_error_addr       : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mac_rd_bus_err_wr_addr;

/* Define the union mac_mst_raddr_low */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mst_rd_addr_low         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mac_mst_raddr_low;

/* Define the union mac_mst_raddr_high */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mst_rd_addr_high        : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mac_mst_raddr_high;

/* Define the union mac_mst_waddr_low */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mst_wr_addr_low         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mac_mst_waddr_low;

/* Define the union mac_mst_waddr_high */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mst_wr_addr_high        : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mac_mst_waddr_high;

/* Define the union mac_mst_addr_range_err */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mst_raddr_range_err     : 1;       /* [0] */
        unsigned int mst_waddr_range_err     : 1;       /* [1] */
        unsigned int reserved                : 30;      /* [31:2] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mac_mst_addr_range_err;

/* Define the union gfx_src1_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_fmt                : 6;       /* [5:0] */
        unsigned int src1_argb_order         : 5;       /* [10:6] */
        unsigned int src1_premult_en         : 1;       /* [11] */
        unsigned int src1_rgb_exp            : 2;       /* [13:12] */
        unsigned int reserved0               : 4;       /* [17:14] */
        unsigned int src1_alpha_range        : 1;       /* [18] */
        unsigned int src1_v_scan_ord         : 1;       /* [19] */
        unsigned int src1_h_scan_ord         : 1;       /* [20] */
        unsigned int src1_rotate_en          : 1;       /* [21] */
        unsigned int src1_rotate_angle       : 1;       /* [22] */
        unsigned int src1_perf_en            : 1;       /* [23] */
        unsigned int reserved1               : 2;       /* [25:24] */
        unsigned int src1_cmp_mode           : 1;       /* [26] */
        unsigned int src1_ice_en             : 1;       /* [27] */
        unsigned int src1_fast_req           : 1;       /* [28] */
        unsigned int src1_mode               : 2;       /* [30:29] */
        unsigned int reserved2               : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_src1_ctrl;

/* Define the union gfx_src1_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_addr               : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_src1_addr;

/* Define the union gfx_src1_stride */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_stride             : 20;      /* [19:0] */
        unsigned int reserved                : 12;      /* [31:20] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_src1_stride;

/* Define the union gfx_src1_imgsize */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_width              : 16;      /* [15:0] */
        unsigned int src1_height             : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_src1_imgsize;

/* Define the union gfx_src1_rect_xy */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_rect_x             : 16;      /* [15:0] */
        unsigned int src1_rect_y             : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_src1_rect_xy;

/* Define the union gfx_src1_rect_wh */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_rect_width         : 16;      /* [15:0] */
        unsigned int src1_rect_height        : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_src1_rect_wh;

/* Define the union gfx_src1_fill */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_color_fill         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_src1_fill;

/* Define the union gfx_src1_alpha */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_alpha0             : 8;       /* [7:0] */
        unsigned int src1_alpha1             : 8;       /* [15:8] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_src1_alpha;

/* Define the union gfx_src1_htc_online_dcmp0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_dcmp_pix_format    : 3;       /* [2:0] */
        unsigned int src1_dcmp_bit_depth     : 1;       /* [3] */
        unsigned int src1_block_size         : 6;       /* [9:4] */
        unsigned int src1_dcmp_mode          : 1;       /* [10] */
        unsigned int dcmp_ck_gt_en           : 1;       /* [11] */
        unsigned int reserved                : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_src1_htc_online_dcmp0;

/* Define the union gfx_src1_htc_online_dcmp1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int round_offset            : 16;      /* [15:0] */
        unsigned int transform_skip          : 1;       /* [16] */
        unsigned int iter_protect_clip_en    : 1;       /* [17] */
        unsigned int csc_skip                : 1;       /* [18] */
        unsigned int iterp_protect_qp_en     : 1;       /* [19] */
        unsigned int csc_matrix_replace_en   : 1;       /* [20] */
        unsigned int trans_csc_order         : 1;       /* [21] */
        unsigned int iter_protect_round_en   : 1;       /* [22] */
        unsigned int iter_protect_withqs_en  : 1;       /* [23] */
        unsigned int alpha_qp_minus1_en      : 1;       /* [24] */
        unsigned int iter_protect_clip_offset : 3;       /* [27:25] */
        unsigned int reserved                : 4;       /* [31:28] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_src1_htc_online_dcmp1;

/* Define the union gfx_src1_cmd_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src_cmd_interval        : 16;      /* [15:0] */
        unsigned int reserved                : 15;      /* [30:16] */
        unsigned int src_cmd_ctrl_en         : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_src1_cmd_ctrl;

/* Define the union gfx_src1_out_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_ck_gt_en           : 1;       /* [0] */
        unsigned int checksum_en             : 1;       /* [1] */
        unsigned int reserved0               : 2;       /* [3:2] */
        unsigned int mute_en                 : 1;       /* [4] */
        unsigned int mute_req_en             : 1;       /* [5] */
        unsigned int reserved1               : 2;       /* [7:6] */
        unsigned int palpha_en               : 1;       /* [8] */
        unsigned int reserved2               : 3;       /* [11:9] */
        unsigned int key_en                  : 1;       /* [12] */
        unsigned int key_mode                : 1;       /* [13] */
        unsigned int reserved3               : 2;       /* [15:14] */
        unsigned int testpattern_en          : 1;       /* [16] */
        unsigned int reserved4               : 15;      /* [31:17] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_src1_out_ctrl;

/* Define the union gfx_src1_mute_alpha */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mute_alpha              : 8;       /* [7:0] */
        unsigned int reserved                : 24;      /* [31:8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_src1_mute_alpha;

/* Define the union gfx_src1_mute_bk */
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
} u_gfx_src1_mute_bk;

/* Define the union gfx_src1_checksum */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_chksum             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_src1_checksum;

/* Define the union gfx_src1_ckey_max */
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
} u_gfx_src1_ckey_max;

/* Define the union gfx_src1_ckey_min */
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
} u_gfx_src1_ckey_min;

/* Define the union gfx_src1_ckey_mask */
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
} u_gfx_src1_ckey_mask;

/* Define the union gfx_src1_htc_online_dcmp_debug0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int htc_dcmp_debug0         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_src1_htc_online_dcmp_debug0;

/* Define the union gfx_src1_htc_online_dcmp_debug1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int htc_dcmp_debug1         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_src1_htc_online_dcmp_debug1;

/* Define the union gfx_src1_htc_online_dcmp_debug2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int htc_dcmp_debug2         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_src1_htc_online_dcmp_debug2;

/* Define the union gfx_src1_htc_online_dcmp_debug3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int htc_dcmp_debug3         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_src1_htc_online_dcmp_debug3;

/* Define the union vid_gfx_src1_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vid_src1_fmt            : 6;       /* [5:0] */
        unsigned int vid_src1_argb_order     : 5;       /* [10:6] */
        unsigned int vid_src1_premult_en     : 1;       /* [11] */
        unsigned int vid_src1_rgb_exp        : 2;       /* [13:12] */
        unsigned int reserved0               : 4;       /* [17:14] */
        unsigned int vid_src1_alpha_range    : 1;       /* [18] */
        unsigned int vid_src1_v_scan_ord     : 1;       /* [19] */
        unsigned int vid_src1_h_scan_ord     : 1;       /* [20] */
        unsigned int vid_src1_rotate_en      : 1;       /* [21] */
        unsigned int vid_src1_rotate_angle   : 1;       /* [22] */
        unsigned int vid_src1_perf_en        : 1;       /* [23] */
        unsigned int reserved1               : 2;       /* [25:24] */
        unsigned int vid_src1_cmp_mode       : 1;       /* [26] */
        unsigned int vid_src1_ice_en         : 1;       /* [27] */
        unsigned int reserved2               : 4;       /* [31:28] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vid_gfx_src1_ctrl;

/* Define the union vid_gfxsrc1_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vid_src1_addr           : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vid_gfxsrc1_addr;

/* Define the union vid_gfxsrc1_stride */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vid_src1_stride         : 20;      /* [19:0] */
        unsigned int reserved                : 12;      /* [31:20] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vid_gfxsrc1_stride;

/* Define the union vid_gfxsrc1_imgsize */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vid_src1_width          : 16;      /* [15:0] */
        unsigned int vid_src1_height         : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vid_gfxsrc1_imgsize;

/* Define the union vid_gfxsrc1_rect_xy */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vid_src1_rect_x         : 16;      /* [15:0] */
        unsigned int vid_src1_rect_y         : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vid_gfxsrc1_rect_xy;

/* Define the union vid_gfxsrc1_rect_wh */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vid_src1_rect_width     : 16;      /* [15:0] */
        unsigned int vid_src1_rect_height    : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vid_gfxsrc1_rect_wh;

/* Define the union vid_gfx_src1_out_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vid_checksum_en         : 1;       /* [0] */
        unsigned int vid_mute_en             : 1;       /* [1] */
        unsigned int vid_palpha_en           : 1;       /* [2] */
        unsigned int vid_key_en              : 1;       /* [3] */
        unsigned int reserved                : 28;      /* [31:4] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vid_gfx_src1_out_ctrl;

/* Define the union gfx_src2_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_fmt                : 6;       /* [5:0] */
        unsigned int src2_argb_order         : 5;       /* [10:6] */
        unsigned int src2_premult_en         : 1;       /* [11] */
        unsigned int src2_rgb_exp            : 2;       /* [13:12] */
        unsigned int reserved0               : 4;       /* [17:14] */
        unsigned int src2_alpha_range        : 1;       /* [18] */
        unsigned int src2_v_scan_ord         : 1;       /* [19] */
        unsigned int src2_h_scan_ord         : 1;       /* [20] */
        unsigned int reserved1               : 5;       /* [25:21] */
        unsigned int src2_cmp_mode           : 1;       /* [26] */
        unsigned int src2_ice_en             : 1;       /* [27] */
        unsigned int src2_fast_req           : 1;       /* [28] */
        unsigned int src2_mode               : 2;       /* [30:29] */
        unsigned int reserved2               : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_src2_ctrl;

/* Define the union gfx_src2_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_addr               : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_src2_addr;

/* Define the union gfx_src2_stride */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_stride             : 20;      /* [19:0] */
        unsigned int reserved                : 12;      /* [31:20] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_src2_stride;

/* Define the union gfx_src2_imgsize */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_width              : 16;      /* [15:0] */
        unsigned int src2_height             : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_src2_imgsize;

/* Define the union gfx_src2_rect_xy */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_rect_x             : 16;      /* [15:0] */
        unsigned int src2_rect_y             : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_src2_rect_xy;

/* Define the union gfx_src2_rect_wh */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_rect_width         : 16;      /* [15:0] */
        unsigned int src2_rect_height        : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_src2_rect_wh;

/* Define the union gfx_src2_fill */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_color_fill         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_src2_fill;

/* Define the union gfx_src2_alpha */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_alpha0             : 8;       /* [7:0] */
        unsigned int src2_alpha1             : 8;       /* [15:8] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_src2_alpha;

/* Define the union gfx_src2_htc_online_dcmp0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_dcmp_pix_format    : 3;       /* [2:0] */
        unsigned int src2_dcmp_bit_depth     : 1;       /* [3] */
        unsigned int src2_block_size         : 6;       /* [9:4] */
        unsigned int src2_dcmp_mode          : 1;       /* [10] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_src2_htc_online_dcmp0;

/* Define the union gfx_src2_htc_online_dcmp1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int round_offset            : 16;      /* [15:0] */
        unsigned int transform_skip          : 1;       /* [16] */
        unsigned int iter_protect_clip_en    : 1;       /* [17] */
        unsigned int csc_skip                : 1;       /* [18] */
        unsigned int iterp_protect_qp_en     : 1;       /* [19] */
        unsigned int csc_matrix_replace_en   : 1;       /* [20] */
        unsigned int trans_csc_order         : 1;       /* [21] */
        unsigned int iter_protect_round_en   : 1;       /* [22] */
        unsigned int iter_protect_withqs_en  : 1;       /* [23] */
        unsigned int alpha_qp_minus1_en      : 1;       /* [24] */
        unsigned int iter_protect_clip_offset : 3;       /* [27:25] */
        unsigned int reserved                : 4;       /* [31:28] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_src2_htc_online_dcmp1;

/* Define the union gfx_src2_cmd_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src_cmd_interval        : 16;      /* [15:0] */
        unsigned int reserved                : 15;      /* [30:16] */
        unsigned int src_cmd_ctrl_en         : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_src2_cmd_ctrl;

/* Define the union gfx_src2_out_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_ck_gt_en           : 1;       /* [0] */
        unsigned int checksum_en             : 1;       /* [1] */
        unsigned int reserved0               : 2;       /* [3:2] */
        unsigned int mute_en                 : 1;       /* [4] */
        unsigned int mute_req_en             : 1;       /* [5] */
        unsigned int reserved1               : 2;       /* [7:6] */
        unsigned int palpha_en               : 1;       /* [8] */
        unsigned int reserved2               : 3;       /* [11:9] */
        unsigned int key_en                  : 1;       /* [12] */
        unsigned int key_mode                : 1;       /* [13] */
        unsigned int reserved3               : 2;       /* [15:14] */
        unsigned int testpattern_en          : 1;       /* [16] */
        unsigned int reserved4               : 15;      /* [31:17] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_src2_out_ctrl;

/* Define the union gfx_src2_mute_alpha */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mute_alpha              : 8;       /* [7:0] */
        unsigned int reserved                : 24;      /* [31:8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_src2_mute_alpha;

/* Define the union gfx_src2_mute_bk */
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
} u_gfx_src2_mute_bk;

/* Define the union gfx_src2_checksum */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_chksum             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_src2_checksum;

/* Define the union gfx_src2_ckey_max */
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
} u_gfx_src2_ckey_max;

/* Define the union gfx_src2_ckey_min */
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
} u_gfx_src2_ckey_min;

/* Define the union gfx_src2_ckey_mask */
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
} u_gfx_src2_ckey_mask;

/* Define the union gfx_src2_htc_online_dcmp_debug0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int htc_dcmp_debug0         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_src2_htc_online_dcmp_debug0;

/* Define the union gfx_src2_htc_online_dcmp_debug1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int htc_dcmp_debug1         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_src2_htc_online_dcmp_debug1;

/* Define the union gfx_src2_htc_online_dcmp_debug2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int htc_dcmp_debug2         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_src2_htc_online_dcmp_debug2;

/* Define the union gfx_src2_htc_online_dcmp_debug3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int htc_dcmp_debug3         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_src2_htc_online_dcmp_debug3;

/* Define the union vid_gfx_src2_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vid_src2_fmt            : 6;       /* [5:0] */
        unsigned int vid_src2_argb_order     : 5;       /* [10:6] */
        unsigned int vid_src2_premult_en     : 1;       /* [11] */
        unsigned int vid_src2_rgb_exp        : 2;       /* [13:12] */
        unsigned int reserved0               : 4;       /* [17:14] */
        unsigned int vid_src2_alpha_range    : 1;       /* [18] */
        unsigned int vid_src2_v_scan_ord     : 1;       /* [19] */
        unsigned int vid_src2_h_scan_ord     : 1;       /* [20] */
        unsigned int reserved1               : 5;       /* [25:21] */
        unsigned int vid_src2_cmp_mode       : 1;       /* [26] */
        unsigned int vid_src2_ice_en         : 1;       /* [27] */
        unsigned int reserved2               : 4;       /* [31:28] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vid_gfx_src2_ctrl;

/* Define the union vid_gfx_src2_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vid_src2_addr           : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vid_gfx_src2_addr;

/* Define the union vid_gfx_src2_stride */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vid_src2_stride         : 20;      /* [19:0] */
        unsigned int reserved                : 12;      /* [31:20] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vid_gfx_src2_stride;

/* Define the union vid_gfx_src2_imgsize */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vid_src2_width          : 16;      /* [15:0] */
        unsigned int vid_src2_height         : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vid_gfx_src2_imgsize;

/* Define the union vid_gfx_src2_rect_xy */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vid_src2_rect_x         : 16;      /* [15:0] */
        unsigned int vid_src2_rect_y         : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vid_gfx_src2_rect_xy;

/* Define the union vid_gfx_src2_rect_wh */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vid_src2_rect_width     : 16;      /* [15:0] */
        unsigned int vid_src2_rect_height    : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vid_gfx_src2_rect_wh;

/* Define the union vid_gfx_src2_out_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vid_checksum_en         : 1;       /* [0] */
        unsigned int vid_mute_en             : 1;       /* [1] */
        unsigned int vid_palpha_en           : 1;       /* [2] */
        unsigned int vid_key_en              : 1;       /* [3] */
        unsigned int reserved                : 28;      /* [31:4] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vid_gfx_src2_out_ctrl;

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
        unsigned int reserved1               : 1;       /* [21] */
        unsigned int des_rgb_trunc           : 1;       /* [22] */
        unsigned int des_mute_en             : 1;       /* [23] */
        unsigned int des_hold_mode           : 1;       /* [24] */
        unsigned int des_r4_order            : 1;       /* [25] */
        unsigned int des_r8_order            : 1;       /* [26] */
        unsigned int reserved2               : 1;       /* [27] */
        unsigned int des_fast_req            : 1;       /* [28] */
        unsigned int des_clk_en              : 1;       /* [29] */
        unsigned int reserved3               : 1;       /* [30] */
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

/* Define the union wbc_dhd_chksum_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int des_chksum_en           : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_wbc_dhd_chksum_ctrl;

/* Define the union wbc_dhd_chksum_sum */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int des_chksum              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_wbc_dhd_chksum_sum;

/* Define the union wbc_dhd_dummy_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int des_dummy_en            : 1;       /* [0] */
        unsigned int des_dummy_num           : 4;       /* [4:1] */
        unsigned int reserved                : 27;      /* [31:5] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_wbc_dhd_dummy_ctrl;

/* Define the union wbc_dhd_dummy_value */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int des_dummy_b             : 8;       /* [7:0] */
        unsigned int des_dummy_g             : 8;       /* [15:8] */
        unsigned int des_dummy_r             : 8;       /* [23:16] */
        unsigned int des_dummy_a             : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_wbc_dhd_dummy_value;

/* Define the union wbc_dhd_burst_len_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int burst_len_cfg           : 4;       /* [3:0] */
        unsigned int burst_len_en            : 1;       /* [4] */
        unsigned int reserved                : 27;      /* [31:5] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_wbc_dhd_burst_len_ctrl;

/* Define the union wbc_dhd_mute_bk */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int des_mute_b              : 8;       /* [7:0] */
        unsigned int des_mute_g              : 8;       /* [15:8] */
        unsigned int des_mute_r              : 8;       /* [23:16] */
        unsigned int des_mute_a              : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_wbc_dhd_mute_bk;

/* Define the union wbc_dhd_bist_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved0               : 1;       /* [0] */
        unsigned int colorbar_en             : 1;       /* [1] */
        unsigned int reserved1               : 28;      /* [29:2] */
        unsigned int bist_ck_gt_en           : 1;       /* [30] */
        unsigned int reserved2               : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_wbc_dhd_bist_cfg;

/* Define the union wbc_dhd_bist_colorbar_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved0               : 1;       /* [0] */
        unsigned int bar_num                 : 2;       /* [2:1] */
        unsigned int reserved1               : 1;       /* [3] */
        unsigned int bar_width               : 12;      /* [15:4] */
        unsigned int reserved2               : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_wbc_dhd_bist_colorbar_cfg;

/* Define the union wbc_dhd_bist_color0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int color0_data2            : 10;      /* [9:0] */
        unsigned int color0_data1            : 10;      /* [19:10] */
        unsigned int color0_data0            : 10;      /* [29:20] */
        unsigned int reserved                : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_wbc_dhd_bist_color0;

/* Define the union wbc_dhd_bist_color1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int color1_data2            : 10;      /* [9:0] */
        unsigned int color1_data1            : 10;      /* [19:10] */
        unsigned int color1_data0            : 10;      /* [29:20] */
        unsigned int reserved                : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_wbc_dhd_bist_color1;

/* Define the union wbc_dhd_bist_color2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int color2_data2            : 10;      /* [9:0] */
        unsigned int color2_data1            : 10;      /* [19:10] */
        unsigned int color2_data0            : 10;      /* [29:20] */
        unsigned int reserved                : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_wbc_dhd_bist_color2;

/* Define the union wbc_dhd_bist_color3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int color3_data2            : 10;      /* [9:0] */
        unsigned int color3_data1            : 10;      /* [19:10] */
        unsigned int color3_data0            : 10;      /* [29:20] */
        unsigned int reserved                : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_wbc_dhd_bist_color3;

/* Define the union wbc_dhd_bist_color4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int color4_data2            : 10;      /* [9:0] */
        unsigned int color4_data1            : 10;      /* [19:10] */
        unsigned int color4_data0            : 10;      /* [29:20] */
        unsigned int reserved                : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_wbc_dhd_bist_color4;

/* Define the union wbc_dhd_bist_color5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int color5_data2            : 10;      /* [9:0] */
        unsigned int color5_data1            : 10;      /* [19:10] */
        unsigned int color5_data0            : 10;      /* [29:20] */
        unsigned int reserved                : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_wbc_dhd_bist_color5;

/* Define the union wbc_dhd_bist_color6 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int color6_data2            : 10;      /* [9:0] */
        unsigned int color6_data1            : 10;      /* [19:10] */
        unsigned int color6_data0            : 10;      /* [29:20] */
        unsigned int reserved                : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_wbc_dhd_bist_color6;

/* Define the union wbc_dhd_bist_color7 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int color7_data2            : 10;      /* [9:0] */
        unsigned int color7_data1            : 10;      /* [19:10] */
        unsigned int color7_data0            : 10;      /* [29:20] */
        unsigned int reserved                : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_wbc_dhd_bist_color7;

/* Define the union vid_wbc_dhd_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vid_des_fmt             : 6;       /* [5:0] */
        unsigned int vid_des_argb_order      : 5;       /* [10:6] */
        unsigned int reserved                : 20;      /* [30:11] */
        unsigned int vid_des_en              : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vid_wbc_dhd_ctrl;

/* Define the union vid_wbc_dhd_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vid_des_addr            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vid_wbc_dhd_addr;

/* Define the union vid_wbc_dhd_stride */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vid_des_stride          : 20;      /* [19:0] */
        unsigned int reserved                : 12;      /* [31:20] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vid_wbc_dhd_stride;

/* Define the union vid_wbc_dhd_imgsize */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vid_des_width           : 16;      /* [15:0] */
        unsigned int vid_des_height          : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vid_wbc_dhd_imgsize;

/* Define the union vid_wbc_dhd_chksum_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vid_des_chksum_en       : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vid_wbc_dhd_chksum_ctrl;


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
    unsigned int                         rsr0;                                /* 28 */
    u_voufsta                            voufsta;                             /* 2c */
    u_voufclr                            voufclr;                             /* 30 */
    u_vointproc_tim                      vointproc_tim;                       /* 34 */
    u_vofpgatest                         vofpgatest;                          /* 38 */
    unsigned int                         rsr1[6];                             /* 3c ~ 50 */
    u_dpu_test_pin                       dpu_test_pin;                        /* 54 */
    u_dpu_frm_num_cnt                    dpu_frm_num_cnt;                     /* 58 */
    unsigned int                         rsr2[5];                             /* 5c ~ 6c */
    u_vosrststa                          vosrststa;                           /* 70 */
    unsigned int                         rsr3[3];                             /* 74 ~ 7c */
    u_dpu2mipi_info_0                    dpu2mipi_info_0;                     /* 80 */
    u_dpu2mipi_info_1                    dpu2mipi_info_1;                     /* 84 */
    u_dpu2mipi_info_2                    dpu2mipi_info_2;                     /* 88 */
    u_dpu2mipi_info_3                    dpu2mipi_info_3;                     /* 8c */
    u_dpu2mipi_info_4                    dpu2mipi_info_4;                     /* 90 */
    u_dpu2mipi_info_5                    dpu2mipi_info_5;                     /* 94 */
    unsigned int                         rsr4[154];                           /* 98 ~ 2fc */
    u_cbm_bkg                            cbm_bkg;                             /* 300 */
    unsigned int                         rsr5;                                /* 304 */
    u_cbm_mix                            cbm_mix;                             /* 308 */
    u_cbm_ctrl                           cbm_ctrl;                            /* 30c */
    unsigned int                         rsr6[60];                            /* 310 ~ 3fc */
    u_dpu_partial_ctrl                   dpu_partial_ctrl;                    /* 400 */
    u_dpu_partial_gen_hdr0               dpu_partial_gen_hdr0;                /* 404 */
    u_dpu_partial_gen_pld_data00         dpu_partial_gen_pld_data00;          /* 408 */
    u_dpu_partial_gen_pld_data01         dpu_partial_gen_pld_data01;          /* 40c */
    u_dpu_partial_gen_hdr1               dpu_partial_gen_hdr1;                /* 410 */
    u_dpu_partial_gen_pld_data10         dpu_partial_gen_pld_data10;          /* 414 */
    u_dpu_partial_gen_pld_data11         dpu_partial_gen_pld_data11;          /* 418 */
    unsigned int                         rsr7[505];                           /* 41c ~ bfc */
    u_link_ctrl                          link_ctrl;                           /* c00 */
    u_vid_link_ctrl                      vid_link_ctrl;                       /* c04 */
    unsigned int                         rsr8[127];                           /* c08 ~ e00 */
    u_para_addr_vhd_chn00                para_addr_vhd_chn00;                 /* e04 */
    unsigned int                         rsr9[62];                            /* e08 ~ efc */
    u_para_up_vhd                        para_up_vhd;                         /* f00 */
    unsigned int                         rsr10[17];                           /* f04 ~ f44 */
    u_para_conflict_clr                  para_conflict_clr;                   /* f48 */
    u_para_conflict_sta                  para_conflict_sta;                   /* f4c */
    unsigned int                         rsr11[6188];                         /* f50 ~ 6ffc */
    u_g0_ctrl                            g0_ctrl;                             /* 7000 */
    u_g0_upd                             g0_upd;                              /* 7004 */
    unsigned int                         rsr12;                               /* 7008 */
    u_g0_0reso_read                      g0_0reso_read;                       /* 700c */
    unsigned int                         rsr13[2];                            /* 7010 ~ 7014 */
    u_vid_g0_ctrl                        vid_g0_ctrl;                         /* 7018 */
    unsigned int                         rsr14[25];                           /* 701c ~ 707c */
    u_g0_dfpos                           g0_dfpos;                            /* 7080 */
    u_g0_dlpos                           g0_dlpos;                            /* 7084 */
    u_g0_vfpos                           g0_vfpos;                            /* 7088 */
    u_g0_vlpos                           g0_vlpos;                            /* 708c */
    u_g0_bk                              g0_bk;                               /* 7090 */
    u_g0_alpha                           g0_alpha;                            /* 7094 */
    u_g0_mute_bk                         g0_mute_bk;                          /* 7098 */
    unsigned int                         rsr15[25];                           /* 709c ~ 70fc */
    u_g0_t2r0_ctrl                       g0_t2r0_ctrl;                        /* 7100 */
    u_g0_t2r0_err_clr                    g0_t2r0_err_clr;                     /* 7104 */
    u_g0_t2r0_err_sta                    g0_t2r0_err_sta;                     /* 7108 */
    u_g0_t2r0_debug1                     g0_t2r0_debug1;                      /* 710c */
    u_g0_t2r0_debug2                     g0_t2r0_debug2;                      /* 7110 */
    u_g0_t2r0_debug3                     g0_t2r0_debug3;                      /* 7114 */
    u_g0_t2r0_debug4                     g0_t2r0_debug4;                      /* 7118 */
    u_g0_t2r0_wh                         g0_t2r0_wh;                          /* 711c */
    unsigned int                         rsr16[56];                           /* 7120 ~ 71fc */
    u_g0_crop0_wh                        g0_crop0_wh;                         /* 7200 */
    u_g0_crop0_xy                        g0_crop0_xy;                         /* 7204 */
    u_g0_crop0_in_wh                     g0_crop0_in_wh;                      /* 7208 */
    unsigned int                         rsr17[61];                           /* 720c ~ 72fc */
    u_g0_add_pix_ctrl                    g0_add_pix_ctrl;                     /* 7300 */
    u_g0_add_pix_small                   g0_add_pix_small;                    /* 7304 */
    u_g0_add_pix_big                     g0_add_pix_big;                      /* 7308 */
    u_g0_add_pix_rect                    g0_add_pix_rect;                     /* 730c */
    u_g0_add_pix_tile                    g0_add_pix_tile;                     /* 7310 */
    unsigned int                         rsr18[59];                           /* 7314 ~ 73fc */
    u_g0_pbi_crop0_wh                    g0_pbi_crop0_wh;                     /* 7400 */
    u_g0_pbi_crop0_xy                    g0_pbi_crop0_xy;                     /* 7404 */
    u_g0_pbi_crop0_in_wh                 g0_pbi_crop0_in_wh;                  /* 7408 */
    unsigned int                         rsr19[253];                          /* 740c ~ 77fc */
    u_g1_ctrl                            g1_ctrl;                             /* 7800 */
    u_g1_upd                             g1_upd;                              /* 7804 */
    unsigned int                         rsr20;                               /* 7808 */
    u_g1_0reso_read                      g1_0reso_read;                       /* 780c */
    unsigned int                         rsr21[2];                            /* 7810 ~ 7814 */
    u_vid_g1_ctrl                        vid_g1_ctrl;                         /* 7818 */
    unsigned int                         rsr22[25];                           /* 781c ~ 787c */
    u_g1_dfpos                           g1_dfpos;                            /* 7880 */
    u_g1_dlpos                           g1_dlpos;                            /* 7884 */
    u_g1_vfpos                           g1_vfpos;                            /* 7888 */
    u_g1_vlpos                           g1_vlpos;                            /* 788c */
    u_g1_bk                              g1_bk;                               /* 7890 */
    u_g1_alpha                           g1_alpha;                            /* 7894 */
    u_g1_mute_bk                         g1_mute_bk;                          /* 7898 */
    unsigned int                         rsr23[25];                           /* 789c ~ 78fc */
    u_g1_t2r1_ctrl                       g1_t2r1_ctrl;                        /* 7900 */
    u_g1_t2r1_err_clr                    g1_t2r1_err_clr;                     /* 7904 */
    u_g1_t2r1_err_sta                    g1_t2r1_err_sta;                     /* 7908 */
    u_g1_t2r1_debug1                     g1_t2r1_debug1;                      /* 790c */
    u_g1_t2r1_debug2                     g1_t2r1_debug2;                      /* 7910 */
    u_g1_t2r1_debug3                     g1_t2r1_debug3;                      /* 7914 */
    u_g1_t2r1_debug4                     g1_t2r1_debug4;                      /* 7918 */
    u_g1_t2r1_wh                         g1_t2r1_wh;                          /* 791c */
    unsigned int                         rsr24[56];                           /* 7920 ~ 79fc */
    u_g1_crop1_wh                        g1_crop1_wh;                         /* 7a00 */
    u_g1_crop1_xy                        g1_crop1_xy;                         /* 7a04 */
    u_g1_crop1_in_wh                     g1_crop1_in_wh;                      /* 7a08 */
    unsigned int                         rsr25[61];                           /* 7a0c ~ 7afc */
    u_g1_add_pix_ctrl                    g1_add_pix_ctrl;                     /* 7b00 */
    u_g1_add_pix_small                   g1_add_pix_small;                    /* 7b04 */
    u_g1_add_pix_big                     g1_add_pix_big;                      /* 7b08 */
    u_g1_add_pix_rect                    g1_add_pix_rect;                     /* 7b0c */
    u_g1_add_pix_tile                    g1_add_pix_tile;                     /* 7b10 */
    unsigned int                         rsr26[2363];                         /* 7b14 ~ 9ffc */
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
    u_dhd_1_csc_ctrl                     dhd_1_csc_ctrl;                      /* a200 */
    u_dhd_1_csc_coef00                   dhd_1_csc_coef00;                    /* a204 */
    u_dhd_1_csc_coef01                   dhd_1_csc_coef01;                    /* a208 */
    u_dhd_1_csc_coef02                   dhd_1_csc_coef02;                    /* a20c */
    u_dhd_1_csc_coef10                   dhd_1_csc_coef10;                    /* a210 */
    u_dhd_1_csc_coef11                   dhd_1_csc_coef11;                    /* a214 */
    u_dhd_1_csc_coef12                   dhd_1_csc_coef12;                    /* a218 */
    u_dhd_1_csc_coef20                   dhd_1_csc_coef20;                    /* a21c */
    u_dhd_1_csc_coef21                   dhd_1_csc_coef21;                    /* a220 */
    u_dhd_1_csc_coef22                   dhd_1_csc_coef22;                    /* a224 */
    u_dhd_1_csc_scale                    dhd_1_csc_scale;                     /* a228 */
    u_dhd_1_csc_idc0                     dhd_1_csc_idc0;                      /* a22c */
    u_dhd_1_csc_idc1                     dhd_1_csc_idc1;                      /* a230 */
    u_dhd_1_csc_idc2                     dhd_1_csc_idc2;                      /* a234 */
    u_dhd_1_csc_odc0                     dhd_1_csc_odc0;                      /* a238 */
    u_dhd_1_csc_odc1                     dhd_1_csc_odc1;                      /* a23c */
    u_dhd_1_csc_odc2                     dhd_1_csc_odc2;                      /* a240 */
    u_dhd_1_csc_min_y                    dhd_1_csc_min_y;                     /* a244 */
    u_dhd_1_csc_min_c                    dhd_1_csc_min_c;                     /* a248 */
    u_dhd_1_csc_max_y                    dhd_1_csc_max_y;                     /* a24c */
    u_dhd_1_csc_max_c                    dhd_1_csc_max_c;                     /* a250 */
    u_dhd_1_csc2_coef00                  dhd_1_csc2_coef00;                   /* a254 */
    u_dhd_1_csc2_coef01                  dhd_1_csc2_coef01;                   /* a258 */
    u_dhd_1_csc2_coef02                  dhd_1_csc2_coef02;                   /* a25c */
    u_dhd_1_csc2_coef10                  dhd_1_csc2_coef10;                   /* a260 */
    u_dhd_1_csc2_coef11                  dhd_1_csc2_coef11;                   /* a264 */
    u_dhd_1_csc2_coef12                  dhd_1_csc2_coef12;                   /* a268 */
    u_dhd_1_csc2_coef20                  dhd_1_csc2_coef20;                   /* a26c */
    u_dhd_1_csc2_coef21                  dhd_1_csc2_coef21;                   /* a270 */
    u_dhd_1_csc2_coef22                  dhd_1_csc2_coef22;                   /* a274 */
    u_dhd_1_csc2_scale                   dhd_1_csc2_scale;                    /* a278 */
    u_dhd_1_csc2_idc0                    dhd_1_csc2_idc0;                     /* a27c */
    u_dhd_1_csc2_idc1                    dhd_1_csc2_idc1;                     /* a280 */
    u_dhd_1_csc2_idc2                    dhd_1_csc2_idc2;                     /* a284 */
    u_dhd_1_csc2_odc0                    dhd_1_csc2_odc0;                     /* a288 */
    u_dhd_1_csc2_odc1                    dhd_1_csc2_odc1;                     /* a28c */
    u_dhd_1_csc2_odc2                    dhd_1_csc2_odc2;                     /* a290 */
    u_dhd_1_csc2_min_y                   dhd_1_csc2_min_y;                    /* a294 */
    u_dhd_1_csc2_min_c                   dhd_1_csc2_min_c;                    /* a298 */
    u_dhd_1_csc2_max_y                   dhd_1_csc2_max_y;                    /* a29c */
    u_dhd_1_csc2_max_c                   dhd_1_csc2_max_c;                    /* a2a0 */
    unsigned int                         rsr29[19];                           /* a2a4 ~ a2ec */
    u_dhd_1_csc_ink_ctrl                 dhd_1_csc_ink_ctrl;                  /* a2f0 */
    u_dhd_1_csc_ink_pos                  dhd_1_csc_ink_pos;                   /* a2f4 */
    u_dhd_1_csc_ink_data                 dhd_1_csc_ink_data;                  /* a2f8 */
    u_dhd_1_csc_ink_data2                dhd_1_csc_ink_data2;                 /* a2fc */
    u_dhd_2_csc_ctrl                     dhd_2_csc_ctrl;                      /* a300 */
    u_dhd_2_csc_coef00                   dhd_2_csc_coef00;                    /* a304 */
    u_dhd_2_csc_coef01                   dhd_2_csc_coef01;                    /* a308 */
    u_dhd_2_csc_coef02                   dhd_2_csc_coef02;                    /* a30c */
    u_dhd_2_csc_coef10                   dhd_2_csc_coef10;                    /* a310 */
    u_dhd_2_csc_coef11                   dhd_2_csc_coef11;                    /* a314 */
    u_dhd_2_csc_coef12                   dhd_2_csc_coef12;                    /* a318 */
    u_dhd_2_csc_coef20                   dhd_2_csc_coef20;                    /* a31c */
    u_dhd_2_csc_coef21                   dhd_2_csc_coef21;                    /* a320 */
    u_dhd_2_csc_coef22                   dhd_2_csc_coef22;                    /* a324 */
    u_dhd_2_csc_scale                    dhd_2_csc_scale;                     /* a328 */
    u_dhd_2_csc_idc0                     dhd_2_csc_idc0;                      /* a32c */
    u_dhd_2_csc_idc1                     dhd_2_csc_idc1;                      /* a330 */
    u_dhd_2_csc_idc2                     dhd_2_csc_idc2;                      /* a334 */
    u_dhd_2_csc_odc0                     dhd_2_csc_odc0;                      /* a338 */
    u_dhd_2_csc_odc1                     dhd_2_csc_odc1;                      /* a33c */
    u_dhd_2_csc_odc2                     dhd_2_csc_odc2;                      /* a340 */
    u_dhd_2_csc_min_y                    dhd_2_csc_min_y;                     /* a344 */
    u_dhd_2_csc_min_c                    dhd_2_csc_min_c;                     /* a348 */
    u_dhd_2_csc_max_y                    dhd_2_csc_max_y;                     /* a34c */
    u_dhd_2_csc_max_c                    dhd_2_csc_max_c;                     /* a350 */
    u_dhd_2_csc2_coef00                  dhd_2_csc2_coef00;                   /* a354 */
    u_dhd_2_csc2_coef01                  dhd_2_csc2_coef01;                   /* a358 */
    u_dhd_2_csc2_coef02                  dhd_2_csc2_coef02;                   /* a35c */
    u_dhd_2_csc2_coef10                  dhd_2_csc2_coef10;                   /* a360 */
    u_dhd_2_csc2_coef11                  dhd_2_csc2_coef11;                   /* a364 */
    u_dhd_2_csc2_coef12                  dhd_2_csc2_coef12;                   /* a368 */
    u_dhd_2_csc2_coef20                  dhd_2_csc2_coef20;                   /* a36c */
    u_dhd_2_csc2_coef21                  dhd_2_csc2_coef21;                   /* a370 */
    u_dhd_2_csc2_coef22                  dhd_2_csc2_coef22;                   /* a374 */
    u_dhd_2_csc2_scale                   dhd_2_csc2_scale;                    /* a378 */
    u_dhd_2_csc2_idc0                    dhd_2_csc2_idc0;                     /* a37c */
    u_dhd_2_csc2_idc1                    dhd_2_csc2_idc1;                     /* a380 */
    u_dhd_2_csc2_idc2                    dhd_2_csc2_idc2;                     /* a384 */
    u_dhd_2_csc2_odc0                    dhd_2_csc2_odc0;                     /* a388 */
    u_dhd_2_csc2_odc1                    dhd_2_csc2_odc1;                     /* a38c */
    u_dhd_2_csc2_odc2                    dhd_2_csc2_odc2;                     /* a390 */
    u_dhd_2_csc2_min_y                   dhd_2_csc2_min_y;                    /* a394 */
    u_dhd_2_csc2_min_c                   dhd_2_csc2_min_c;                    /* a398 */
    u_dhd_2_csc2_max_y                   dhd_2_csc2_max_y;                    /* a39c */
    u_dhd_2_csc2_max_c                   dhd_2_csc2_max_c;                    /* a3a0 */
    unsigned int                         rsr30[19];                           /* a3a4 ~ a3ec */
    u_dhd_2_csc_ink_ctrl                 dhd_2_csc_ink_ctrl;                  /* a3f0 */
    u_dhd_2_csc_ink_pos                  dhd_2_csc_ink_pos;                   /* a3f4 */
    u_dhd_2_csc_ink_data                 dhd_2_csc_ink_data;                  /* a3f8 */
    u_dhd_2_csc_ink_data2                dhd_2_csc_ink_data2;                 /* a3fc */
    u_dhd_arsr_cfg                       dhd_arsr_cfg;                        /* a400 */
    u_dhd_arsr_lut_rd_en                 dhd_arsr_lut_rd_en;                  /* a404 */
    u_dhd_arsr_lut_rdata                 dhd_arsr_lut_rdata;                  /* a408 */
    u_dhd_arsr_reso_in                   dhd_arsr_reso_in;                    /* a40c */
    u_dhd_arsr_reso_out                  dhd_arsr_reso_out;                   /* a410 */
    u_dhd_arsr_h_inc                     dhd_arsr_h_inc;                      /* a414 */
    u_dhd_arsr_v_inc                     dhd_arsr_v_inc;                      /* a418 */
    u_dhd_arsr_in_h_left                 dhd_arsr_in_h_left;                  /* a41c */
    u_dhd_arsr_in_h_right                dhd_arsr_in_h_right;                 /* a420 */
    u_dhd_arsr_in_v_top                  dhd_arsr_in_v_top;                   /* a424 */
    u_dhd_arsr_in_v_bottom               dhd_arsr_in_v_bottom;                /* a428 */
    u_dhd_arsr_expect_value              dhd_arsr_expect_value;               /* a42c */
    u_dhd_arsr_thres_l                   dhd_arsr_thres_l;                    /* a430 */
    u_dhd_arsr_thres_h                   dhd_arsr_thres_h;                    /* a434 */
    u_dhd_arsr_slop_0                    dhd_arsr_slop_0;                     /* a438 */
    u_dhd_arsr_slop_1                    dhd_arsr_slop_1;                     /* a43c */
    u_dhd_arsr_shoot_min                 dhd_arsr_shoot_min;                  /* a440 */
    u_dhd_arsr_shoot_max                 dhd_arsr_shoot_max;                  /* a444 */
    u_dhd_arsr_shoot_slop                dhd_arsr_shoot_slop;                 /* a448 */
    u_dhd_arsr_diff_low_high             dhd_arsr_diff_low_high;              /* a44c */
    u_dhd_arsr_corner_thres              dhd_arsr_corner_thres;               /* a450 */
    u_dhd_arsr_dir_dif_max               dhd_arsr_dir_dif_max;                /* a454 */
    u_dhd_arsr_dir_hv_dif_min            dhd_arsr_dir_hv_dif_min;             /* a458 */
    u_dhd_arsr_dir_hv_dif_max            dhd_arsr_dir_hv_dif_max;             /* a45c */
    u_dhd_arsr_sharp_level               dhd_arsr_sharp_level;                /* a460 */
    u_dhd_arsr_sharp_gain_0              dhd_arsr_sharp_gain_0;               /* a464 */
    u_dhd_arsr_sharp_gain_1              dhd_arsr_sharp_gain_1;               /* a468 */
    u_dhd_arsr_sharp_gain_2              dhd_arsr_sharp_gain_2;               /* a46c */
    u_dhd_arsr_sharp_gain_3              dhd_arsr_sharp_gain_3;               /* a470 */
    u_dhd_arsr_sharp_gain_4              dhd_arsr_sharp_gain_4;               /* a474 */
    u_dhd_arsr_gain_ctrl_slop_l_mf       dhd_arsr_gain_ctrl_slop_l_mf;        /* a478 */
    u_dhd_arsr_gain_ctrl_slop_h_mf       dhd_arsr_gain_ctrl_slop_h_mf;        /* a47c */
    u_dhd_arsr_gain_ctrl_slop_l_hf       dhd_arsr_gain_ctrl_slop_l_hf;        /* a480 */
    u_dhd_arsr_gain_ctrl_slop_h_hf       dhd_arsr_gain_ctrl_slop_h_hf;        /* a484 */
    u_dhd_arsr_sharp_gain_mf             dhd_arsr_sharp_gain_mf;              /* a488 */
    u_dhd_arsr_sharp_gain_hf             dhd_arsr_sharp_gain_hf;              /* a48c */
    u_dhd_arsr_mf_b                      dhd_arsr_mf_b;                       /* a490 */
    u_dhd_arsr_hf_b                      dhd_arsr_hf_b;                       /* a494 */
    u_dhd_arsr_shp_tocut_high            dhd_arsr_shp_tocut_high;             /* a498 */
    u_dhd_arsr_skin_thres                dhd_arsr_skin_thres;                 /* a49c */
    u_dhd_arsr_linear_sh_ratio           dhd_arsr_linear_sh_ratio;            /* a4a0 */
    u_dhd_arsr_shoot_ctrl                dhd_arsr_shoot_ctrl;                 /* a4a4 */
    u_dhd_arsr_dir_diff_avg_thres01      dhd_arsr_dir_diff_avg_thres01;       /* a4a8 */
    u_dhd_arsr_dir_diff_avg_thres23      dhd_arsr_dir_diff_avg_thres23;       /* a4ac */
    u_dhd_arsr_coef_ar_shp_mf            dhd_arsr_coef_ar_shp_mf;             /* a4b0 */
    u_dhd_arsr_coef_ar_shp_hf            dhd_arsr_coef_ar_shp_hf;             /* a4b4 */
    u_dhd_arsr_dfx_para_load             dhd_arsr_dfx_para_load;              /* a4b8 */
    unsigned int                         rsr31[17];                           /* a4bc ~ a4fc */
    u_vhdaccctrl                         vhdaccctrl;                          /* a500 */
    u_vhdacchpos                         vhdacchpos;                          /* a504 */
    u_vhdaccvpos                         vhdaccvpos;                          /* a508 */
    u_vhdacc_all_gain                    vhdacc_all_gain;                     /* a50c */
    u_vhdaccdemo                         vhdaccdemo;                          /* a510 */
    u_vhdacclut1                         vhdacclut1;                          /* a514 */
    u_vhdacclut2                         vhdacclut2;                          /* a518 */
    u_vhdacclut3                         vhdacclut3;                          /* a51c */
    u_vhdacclut4                         vhdacclut4;                          /* a520 */
    u_vhdacclut5                         vhdacclut5;                          /* a524 */
    u_vhdacclut6                         vhdacclut6;                          /* a528 */
    u_vhdacclut7                         vhdacclut7;                          /* a52c */
    u_vhdacclut8                         vhdacclut8;                          /* a530 */
    u_vhdacclut9                         vhdacclut9;                          /* a534 */
    u_vhdacclut10                        vhdacclut10;                         /* a538 */
    u_vhdacclut11                        vhdacclut11;                         /* a53c */
    u_vhdacc_gain_pos_lut1               vhdacc_gain_pos_lut1;                /* a540 */
    u_vhdacc_gain_pos_lut2               vhdacc_gain_pos_lut2;                /* a544 */
    u_vhdacc_gain_pos_lut3               vhdacc_gain_pos_lut3;                /* a548 */
    u_vhdacc_gain_pos_lut4               vhdacc_gain_pos_lut4;                /* a54c */
    u_vhdacc_gain_pos_lut5               vhdacc_gain_pos_lut5;                /* a550 */
    u_vhdacc_gain_pos_lut6               vhdacc_gain_pos_lut6;                /* a554 */
    u_vhdacc_gain_pos_lut7               vhdacc_gain_pos_lut7;                /* a558 */
    u_vhdacc_gain_pos_lut8               vhdacc_gain_pos_lut8;                /* a55c */
    u_vhdacc_gain_pos_lut9               vhdacc_gain_pos_lut9;                /* a560 */
    u_vhdacc_gain_pos_lut10              vhdacc_gain_pos_lut10;               /* a564 */
    u_vhdacc_gain_pos_lut11              vhdacc_gain_pos_lut11;               /* a568 */
    u_vhdacc_gain_neg_lut1               vhdacc_gain_neg_lut1;                /* a56c */
    u_vhdacc_gain_neg_lut2               vhdacc_gain_neg_lut2;                /* a570 */
    u_vhdacc_gain_neg_lut3               vhdacc_gain_neg_lut3;                /* a574 */
    u_vhdacc_gain_neg_lut4               vhdacc_gain_neg_lut4;                /* a578 */
    u_vhdacc_gain_neg_lut5               vhdacc_gain_neg_lut5;                /* a57c */
    u_vhdacc_gain_neg_lut6               vhdacc_gain_neg_lut6;                /* a580 */
    u_vhdacc_gain_neg_lut7               vhdacc_gain_neg_lut7;                /* a584 */
    u_vhdacc_gain_neg_lut8               vhdacc_gain_neg_lut8;                /* a588 */
    u_vhdacc_gain_neg_lut9               vhdacc_gain_neg_lut9;                /* a58c */
    u_vhdacc_gain_neg_lut10              vhdacc_gain_neg_lut10;               /* a590 */
    u_vhdacc_gain_neg_lut11              vhdacc_gain_neg_lut11;               /* a594 */
    u_vhdaccstatctrl                     vhdaccstatctrl;                      /* a598 */
    u_vhdaccstathpos                     vhdaccstathpos;                      /* a59c */
    u_vhdaccstatvpos                     vhdaccstatvpos;                      /* a5a0 */
    u_vhdaccshiftd                       vhdaccshiftd;                        /* a5a4 */
    u_vhdaccdnsam                        vhdaccdnsam;                         /* a5a8 */
    u_vhdaccsatcfg                       vhdaccsatcfg;                        /* a5ac */
    u_vhdacc_power_lut1                  vhdacc_power_lut1;                   /* a5b0 */
    u_vhdacc_power_lut2                  vhdacc_power_lut2;                   /* a5b4 */
    u_vhdacc_power_lut3                  vhdacc_power_lut3;                   /* a5b8 */
    u_vhdaccydiffd1sum                   vhdaccydiffd1sum;                    /* a5bc */
    u_vhdaccydiffd2sum                   vhdaccydiffd2sum;                    /* a5c0 */
    u_vhdacc_total_y_sum                 vhdacc_total_y_sum;                  /* a5c4 */
    u_vhdacc_total_pw_sum                vhdacc_total_pw_sum;                 /* a5c8 */
    u_vhdaccysum                         vhdaccysum;                          /* a5cc */
    u_vhdaccsatsum                       vhdaccsatsum;                        /* a5d0 */
    u_vhdaccsathistsum                   vhdaccsathistsum;                    /* a5d4 */
    u_vhdaccsathistcum                   vhdaccsathistcum;                    /* a5d8 */
    u_vhdaccyhist_0                      vhdaccyhist_0;                       /* a5dc */
    u_vhdaccyhist_1                      vhdaccyhist_1;                       /* a5e0 */
    u_vhdaccyhist_2                      vhdaccyhist_2;                       /* a5e4 */
    u_vhdaccyhist_3                      vhdaccyhist_3;                       /* a5e8 */
    u_vhdaccyhist_4                      vhdaccyhist_4;                       /* a5ec */
    u_vhdaccyhist_5                      vhdaccyhist_5;                       /* a5f0 */
    u_vhdaccyhist_6                      vhdaccyhist_6;                       /* a5f4 */
    u_vhdaccyhist_7                      vhdaccyhist_7;                       /* a5f8 */
    u_vhdaccyhist_8                      vhdaccyhist_8;                       /* a5fc */
    u_vhdaccyhist_9                      vhdaccyhist_9;                       /* a600 */
    u_vhdaccyhist_10                     vhdaccyhist_10;                      /* a604 */
    u_vhdaccyhist_11                     vhdaccyhist_11;                      /* a608 */
    u_vhdaccyhist_12                     vhdaccyhist_12;                      /* a60c */
    u_vhdaccyhist_13                     vhdaccyhist_13;                      /* a610 */
    u_vhdaccyhist_14                     vhdaccyhist_14;                      /* a614 */
    u_vhdaccyhist_15                     vhdaccyhist_15;                      /* a618 */
    u_vhdaccyhist_16                     vhdaccyhist_16;                      /* a61c */
    u_vhdaccyhist_17                     vhdaccyhist_17;                      /* a620 */
    u_vhdaccyhist_18                     vhdaccyhist_18;                      /* a624 */
    u_vhdaccyhist_19                     vhdaccyhist_19;                      /* a628 */
    u_vhdaccsathist_0                    vhdaccsathist_0;                     /* a62c */
    u_vhdaccsathist_1                    vhdaccsathist_1;                     /* a630 */
    u_vhdaccsathist_2                    vhdaccsathist_2;                     /* a634 */
    u_vhdaccsathist_3                    vhdaccsathist_3;                     /* a638 */
    u_vhdaccsathist_4                    vhdaccsathist_4;                     /* a63c */
    u_vhdaccsathist_5                    vhdaccsathist_5;                     /* a640 */
    u_vhdaccsathist_6                    vhdaccsathist_6;                     /* a644 */
    u_vhdaccsathist_7                    vhdaccsathist_7;                     /* a648 */
    u_vhdaccsathist_8                    vhdaccsathist_8;                     /* a64c */
    u_vhdaccsathist_9                    vhdaccsathist_9;                     /* a650 */
    u_vhdaccsathist_10                   vhdaccsathist_10;                    /* a654 */
    u_vhdaccsathist_11                   vhdaccsathist_11;                    /* a658 */
    u_vhdaccsathist_12                   vhdaccsathist_12;                    /* a65c */
    u_vhdaccsathist_13                   vhdaccsathist_13;                    /* a660 */
    u_vhdaccsathist_14                   vhdaccsathist_14;                    /* a664 */
    u_vhdaccsathist_15                   vhdaccsathist_15;                    /* a668 */
    u_vhdaccsathist_16                   vhdaccsathist_16;                    /* a66c */
    u_vhdaccsathist_17                   vhdaccsathist_17;                    /* a670 */
    u_vhdaccsathist_18                   vhdaccsathist_18;                    /* a674 */
    u_vhdaccsathist_19                   vhdaccsathist_19;                    /* a678 */
    unsigned int                         rsr32[33];                           /* a67c ~ a6fc */
    u_dhd_add_pix_ctrl                   dhd_add_pix_ctrl;                    /* a700 */
    u_dhd_add_pix_small                  dhd_add_pix_small;                   /* a704 */
    u_dhd_add_pix_big                    dhd_add_pix_big;                     /* a708 */
    u_dhd_add_pix_rect                   dhd_add_pix_rect;                    /* a70c */
    u_dhd_add_pix_tile                   dhd_add_pix_tile;                    /* a710 */
    unsigned int                         rsr33[59];                           /* a714 ~ a7fc */
    u_dhd_t2r1_ctrl                      dhd_t2r1_ctrl;                       /* a800 */
    u_dhd_t2r1_err_clr                   dhd_t2r1_err_clr;                    /* a804 */
    u_dhd_t2r1_err_sta                   dhd_t2r1_err_sta;                    /* a808 */
    u_dhd_t2r1_debug1                    dhd_t2r1_debug1;                     /* a80c */
    u_dhd_t2r1_debug2                    dhd_t2r1_debug2;                     /* a810 */
    u_dhd_t2r1_debug3                    dhd_t2r1_debug3;                     /* a814 */
    u_dhd_t2r1_debug4                    dhd_t2r1_debug4;                     /* a818 */
    u_dhd_t2r1_wh                        dhd_t2r1_wh;                         /* a81c */
    unsigned int                         rsr34[56];                           /* a820 ~ a8fc */
    u_dhd_crop0_wh                       dhd_crop0_wh;                        /* a900 */
    u_dhd_crop0_xy                       dhd_crop0_xy;                        /* a904 */
    u_dhd_crop0_in_wh                    dhd_crop0_in_wh;                     /* a908 */
    unsigned int                         rsr35[2493];                         /* a90c ~ cffc */
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
    unsigned int                         rsr36[3];                            /* d034 ~ d03c */
    u_dhd0_clk_dv_ctrl                   dhd0_clk_dv_ctrl;                    /* d040 */
    u_dhd0_rgb_fix_ctrl                  dhd0_rgb_fix_ctrl;                   /* d044 */
    unsigned int                         rsr37[4];                            /* d048 ~ d054 */
    u_dhd0_intf_chksum_y                 dhd0_intf_chksum_y;                  /* d058 */
    u_dhd0_intf_chksum_u                 dhd0_intf_chksum_u;                  /* d05c */
    u_dhd0_intf_chksum_v                 dhd0_intf_chksum_v;                  /* d060 */
    u_dhd0_intf_pcnt_cnt                 dhd0_intf_pcnt_cnt;                  /* d064 */
    u_dhd0_intf_pcnt_cnt_real            dhd0_intf_pcnt_cnt_real;             /* d068 */
    u_dhd0_intf_pcnt_real                dhd0_intf_pcnt_real;                 /* d06c */
    u_dhd0_intf_chksum_high1             dhd0_intf_chksum_high1;              /* d070 */
    u_dhd0_intf_pcnt                     dhd0_intf_pcnt;                      /* d074 */
    u_dhd0_intf_cmdcfg                   dhd0_intf_cmdcfg;                    /* d078 */
    u_dhd0_tde_frm_rdy                   dhd0_tde_frm_rdy;                    /* d07c */
    u_dhd0_te_timeout                    dhd0_te_timeout;                     /* d080 */
    unsigned int                         rsr38;                               /* d084 */
    u_dhd0_state                         dhd0_state;                          /* d088 */
    u_dhd0_uf_state                      dhd0_uf_state;                       /* d08c */
    u_vo_mux                             vo_mux;                              /* d090 */
    u_vo_mux_sync                        vo_mux_sync;                         /* d094 */
    u_vo_mux_data                        vo_mux_data;                         /* d098 */
    unsigned int                         rsr39;                               /* d09c */
    u_dhd0_vsync_te_state                dhd0_vsync_te_state;                 /* d0a0 */
    u_dhd0_vsync_te_state1               dhd0_vsync_te_state1;                /* d0a4 */
    unsigned int                         rsr40[50];                           /* d0a8 ~ d16c */
    u_intf0_sdlp_ctrl                    intf0_sdlp_ctrl;                     /* d170 */
    u_intf0_sdlp_delay                   intf0_sdlp_delay;                    /* d174 */
    u_intf0_sdlp_stop                    intf0_sdlp_stop;                     /* d178 */
    u_intf0_sdlp_sta                     intf0_sdlp_sta;                      /* d17c */
    unsigned int                         rsr41;                               /* d180 */
    u_intf_mipi_upd                      intf_mipi_upd;                       /* d184 */
    u_intf_mipi_sync_inv                 intf_mipi_sync_inv;                  /* d188 */
    unsigned int                         rsr42[157];                          /* d18c ~ d3fc */
    u_intf0_frm_rdy_clr_ctrl             intf0_frm_rdy_clr_ctrl;              /* d400 */
    u_intf0_frm_rdy_clr                  intf0_frm_rdy_clr;                   /* d404 */
    unsigned int                         rsr43[2814];                         /* d408 ~ fffc */
    u_mac_outstanding                    mac_outstanding;                     /* 10000 */
    u_mac_ctrl                           mac_ctrl;                            /* 10004 */
    u_mac_rchn_prio                      mac_rchn_prio;                       /* 10008 */
    u_mac_wchn_prio                      mac_wchn_prio;                       /* 1000c */
    u_mac_bus_err_clr                    mac_bus_err_clr;                     /* 10010 */
    u_mac_bus_err                        mac_bus_err;                         /* 10014 */
    u_mac_src0_status0                   mac_src0_status0;                    /* 10018 */
    u_mac_src0_status1                   mac_src0_status1;                    /* 1001c */
    u_mac_debug_ctrl                     mac_debug_ctrl;                      /* 10020 */
    u_mac_debug_clr                      mac_debug_clr;                       /* 10024 */
    u_mst_det_latency                    mst_det_latency;                     /* 10028 */
    u_mac_rd_info                        mac_rd_info;                         /* 1002c */
    u_mac_wr_info                        mac_wr_info;                         /* 10030 */
    u_mac_det_latency0                   mac_det_latency0;                    /* 10034 */
    u_mac_det_latency1                   mac_det_latency1;                    /* 10038 */
    u_mac_det_latency2                   mac_det_latency2;                    /* 1003c */
    u_mac_det_latency3                   mac_det_latency3;                    /* 10040 */
    u_mac_det_latency4                   mac_det_latency4;                    /* 10044 */
    u_mac_det_latency5                   mac_det_latency5;                    /* 10048 */
    u_mac_rd_bus_err_rd_addr             mac_rd_bus_err_rd_addr;              /* 1004c */
    u_mac_rd_bus_err_wr_addr             mac_rd_bus_err_wr_addr;              /* 10050 */
    u_mac_mst_raddr_low                  mac_mst_raddr_low;                   /* 10054 */
    u_mac_mst_raddr_high                 mac_mst_raddr_high;                  /* 10058 */
    u_mac_mst_waddr_low                  mac_mst_waddr_low;                   /* 1005c */
    u_mac_mst_waddr_high                 mac_mst_waddr_high;                  /* 10060 */
    u_mac_mst_addr_range_err             mac_mst_addr_range_err;              /* 10064 */
    unsigned int                         rsr44[870];                          /* 10068 ~ 10dfc */
    u_gfx_src1_ctrl                      gfx_src1_ctrl;                       /* 10e00 */
    u_gfx_src1_addr                      gfx_src1_addr;                       /* 10e04 */
    u_gfx_src1_stride                    gfx_src1_stride;                     /* 10e08 */
    u_gfx_src1_imgsize                   gfx_src1_imgsize;                    /* 10e0c */
    u_gfx_src1_rect_xy                   gfx_src1_rect_xy;                    /* 10e10 */
    u_gfx_src1_rect_wh                   gfx_src1_rect_wh;                    /* 10e14 */
    u_gfx_src1_fill                      gfx_src1_fill;                       /* 10e18 */
    u_gfx_src1_alpha                     gfx_src1_alpha;                      /* 10e1c */
    unsigned int                         rsr45[42];                           /* 10e20 ~ 10ec4 */
    u_gfx_src1_htc_online_dcmp0          gfx_src1_htc_online_dcmp0;           /* 10ec8 */
    u_gfx_src1_htc_online_dcmp1          gfx_src1_htc_online_dcmp1;           /* 10ecc */
    unsigned int                         rsr46[12];                           /* 10ed0 ~ 10efc */
    u_gfx_src1_cmd_ctrl                  gfx_src1_cmd_ctrl;                   /* 10f00 */
    u_gfx_src1_out_ctrl                  gfx_src1_out_ctrl;                   /* 10f04 */
    u_gfx_src1_mute_alpha                gfx_src1_mute_alpha;                 /* 10f08 */
    u_gfx_src1_mute_bk                   gfx_src1_mute_bk;                    /* 10f0c */
    u_gfx_src1_checksum                  gfx_src1_checksum;                   /* 10f10 */
    u_gfx_src1_ckey_max                  gfx_src1_ckey_max;                   /* 10f14 */
    u_gfx_src1_ckey_min                  gfx_src1_ckey_min;                   /* 10f18 */
    u_gfx_src1_ckey_mask                 gfx_src1_ckey_mask;                  /* 10f1c */
    unsigned int                         rsr47[24];                           /* 10f20 ~ 10f7c */
    u_gfx_src1_htc_online_dcmp_debug0    gfx_src1_htc_online_dcmp_debug0;     /* 10f80 */
    u_gfx_src1_htc_online_dcmp_debug1    gfx_src1_htc_online_dcmp_debug1;     /* 10f84 */
    u_gfx_src1_htc_online_dcmp_debug2    gfx_src1_htc_online_dcmp_debug2;     /* 10f88 */
    u_gfx_src1_htc_online_dcmp_debug3    gfx_src1_htc_online_dcmp_debug3;     /* 10f8c */
    u_vid_gfx_src1_ctrl                  vid_gfx_src1_ctrl;                   /* 10f90 */
    u_vid_gfxsrc1_addr                   vid_gfxsrc1_addr;                    /* 10f94 */
    u_vid_gfxsrc1_stride                 vid_gfxsrc1_stride;                  /* 10f98 */
    u_vid_gfxsrc1_imgsize                vid_gfxsrc1_imgsize;                 /* 10f9c */
    u_vid_gfxsrc1_rect_xy                vid_gfxsrc1_rect_xy;                 /* 10fa0 */
    u_vid_gfxsrc1_rect_wh                vid_gfxsrc1_rect_wh;                 /* 10fa4 */
    u_vid_gfx_src1_out_ctrl              vid_gfx_src1_out_ctrl;               /* 10fa8 */
    unsigned int                         rsr48[21];                           /* 10fac ~ 10ffc */
    u_gfx_src2_ctrl                      gfx_src2_ctrl;                       /* 11000 */
    u_gfx_src2_addr                      gfx_src2_addr;                       /* 11004 */
    u_gfx_src2_stride                    gfx_src2_stride;                     /* 11008 */
    u_gfx_src2_imgsize                   gfx_src2_imgsize;                    /* 1100c */
    u_gfx_src2_rect_xy                   gfx_src2_rect_xy;                    /* 11010 */
    u_gfx_src2_rect_wh                   gfx_src2_rect_wh;                    /* 11014 */
    u_gfx_src2_fill                      gfx_src2_fill;                       /* 11018 */
    u_gfx_src2_alpha                     gfx_src2_alpha;                      /* 1101c */
    unsigned int                         rsr49[42];                           /* 11020 ~ 110c4 */
    u_gfx_src2_htc_online_dcmp0          gfx_src2_htc_online_dcmp0;           /* 110c8 */
    u_gfx_src2_htc_online_dcmp1          gfx_src2_htc_online_dcmp1;           /* 110cc */
    unsigned int                         rsr50[12];                           /* 110d0 ~ 110fc */
    u_gfx_src2_cmd_ctrl                  gfx_src2_cmd_ctrl;                   /* 11100 */
    u_gfx_src2_out_ctrl                  gfx_src2_out_ctrl;                   /* 11104 */
    u_gfx_src2_mute_alpha                gfx_src2_mute_alpha;                 /* 11108 */
    u_gfx_src2_mute_bk                   gfx_src2_mute_bk;                    /* 1110c */
    u_gfx_src2_checksum                  gfx_src2_checksum;                   /* 11110 */
    u_gfx_src2_ckey_max                  gfx_src2_ckey_max;                   /* 11114 */
    u_gfx_src2_ckey_min                  gfx_src2_ckey_min;                   /* 11118 */
    u_gfx_src2_ckey_mask                 gfx_src2_ckey_mask;                  /* 1111c */
    unsigned int                         rsr51[24];                           /* 11120 ~ 1117c */
    u_gfx_src2_htc_online_dcmp_debug0    gfx_src2_htc_online_dcmp_debug0;     /* 11180 */
    u_gfx_src2_htc_online_dcmp_debug1    gfx_src2_htc_online_dcmp_debug1;     /* 11184 */
    u_gfx_src2_htc_online_dcmp_debug2    gfx_src2_htc_online_dcmp_debug2;     /* 11188 */
    u_gfx_src2_htc_online_dcmp_debug3    gfx_src2_htc_online_dcmp_debug3;     /* 1118c */
    u_vid_gfx_src2_ctrl                  vid_gfx_src2_ctrl;                   /* 11190 */
    u_vid_gfx_src2_addr                  vid_gfx_src2_addr;                   /* 11194 */
    u_vid_gfx_src2_stride                vid_gfx_src2_stride;                 /* 11198 */
    u_vid_gfx_src2_imgsize               vid_gfx_src2_imgsize;                /* 1119c */
    u_vid_gfx_src2_rect_xy               vid_gfx_src2_rect_xy;                /* 111a0 */
    u_vid_gfx_src2_rect_wh               vid_gfx_src2_rect_wh;                /* 111a4 */
    u_vid_gfx_src2_out_ctrl              vid_gfx_src2_out_ctrl;               /* 111a8 */
    unsigned int                         rsr52[1301];                         /* 111ac ~ 125fc */
    u_wbc_dhd_ctrl                       wbc_dhd_ctrl;                        /* 12600 */
    u_wbc_dhd_addr                       wbc_dhd_addr;                        /* 12604 */
    u_wbc_dhd_stride                     wbc_dhd_stride;                      /* 12608 */
    u_wbc_dhd_imgsize                    wbc_dhd_imgsize;                     /* 1260c */
    unsigned int                         rsr53[3];                            /* 12610 ~ 12618 */
    u_wbc_dhd_cmd_ctrl                   wbc_dhd_cmd_ctrl;                    /* 1261c */
    u_wbc_dhd_chksum_ctrl                wbc_dhd_chksum_ctrl;                 /* 12620 */
    u_wbc_dhd_chksum_sum                 wbc_dhd_chksum_sum;                  /* 12624 */
    u_wbc_dhd_dummy_ctrl                 wbc_dhd_dummy_ctrl;                  /* 12628 */
    u_wbc_dhd_dummy_value                wbc_dhd_dummy_value;                 /* 1262c */
    u_wbc_dhd_burst_len_ctrl             wbc_dhd_burst_len_ctrl;              /* 12630 */
    u_wbc_dhd_mute_bk                    wbc_dhd_mute_bk;                     /* 12634 */
    u_wbc_dhd_bist_cfg                   wbc_dhd_bist_cfg;                    /* 12638 */
    u_wbc_dhd_bist_colorbar_cfg          wbc_dhd_bist_colorbar_cfg;           /* 1263c */
    u_wbc_dhd_bist_color0                wbc_dhd_bist_color0;                 /* 12640 */
    u_wbc_dhd_bist_color1                wbc_dhd_bist_color1;                 /* 12644 */
    u_wbc_dhd_bist_color2                wbc_dhd_bist_color2;                 /* 12648 */
    u_wbc_dhd_bist_color3                wbc_dhd_bist_color3;                 /* 1264c */
    u_wbc_dhd_bist_color4                wbc_dhd_bist_color4;                 /* 12650 */
    u_wbc_dhd_bist_color5                wbc_dhd_bist_color5;                 /* 12654 */
    u_wbc_dhd_bist_color6                wbc_dhd_bist_color6;                 /* 12658 */
    u_wbc_dhd_bist_color7                wbc_dhd_bist_color7;                 /* 1265c */
    u_vid_wbc_dhd_ctrl                   vid_wbc_dhd_ctrl;                    /* 12660 */
    u_vid_wbc_dhd_addr                   vid_wbc_dhd_addr;                    /* 12664 */
    u_vid_wbc_dhd_stride                 vid_wbc_dhd_stride;                  /* 12668 */
    u_vid_wbc_dhd_imgsize                vid_wbc_dhd_imgsize;                 /* 1266c */
    u_vid_wbc_dhd_chksum_ctrl            vid_wbc_dhd_chksum_ctrl;             /* 12670 */
} dpu_regs_type;

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* DRV_GRAPHIC_HAL_DPU_REG_DEFINE_H */
