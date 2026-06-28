/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description : vau cs reg define file
 */

#ifndef DRV_GRAPHIC_HAL_VAU_REG_DEFINE_H
#define DRV_GRAPHIC_HAL_VAU_REG_DEFINE_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#if defined(GPU_1A21)
#define HAL_VAU_REG_BASEADDR 0x52080000
#elif defined(GPU_FPGA_3321)
#define HAL_VAU_REG_BASEADDR 0x56010000
#else
#define HAL_VAU_REG_BASEADDR 0x56011000
#endif

#define HAL_VAU_REG_START           (HAL_VAU_REG_BASEADDR + 0x0500)
#define HAL_VAU_REG_INT_STATE       (HAL_VAU_REG_BASEADDR + 0x0504)
#define HAL_VAU_REG_INT_CLR         (HAL_VAU_REG_BASEADDR + 0x0508)
#define HAL_VAU_REG_PNEXT           (HAL_VAU_REG_BASEADDR + 0x01C0)
#define HAL_VAU_REG_STATUS          (HAL_VAU_REG_BASEADDR + 0x4000)
#define HAL_VAU_REG_CURRENT_NODE    (HAL_VAU_REG_BASEADDR + 0x4004)
#define HAL_VAU_REG_MISCELLANEOUS   (HAL_VAU_REG_BASEADDR + 0x0514)
#define HAL_VAU_REG_OUTSTANDING     (HAL_VAU_REG_BASEADDR + 0x0600)
#define HAL_VAU_SOFTRST_STATE       (HAL_VAU_REG_BASEADDR + 0x052C)
#define HAL_VAU_LIST_PFCNT          (HAL_VAU_REG_BASEADDR + 0x40a0)
#define HAL_VAU_START_CNT           (HAL_VAU_REG_BASEADDR + 0x4060)
#define HAL_VAU_LIST_FINISH_CNT     (HAL_VAU_REG_BASEADDR + 0x4064)
#define HAL_VAU_INT_CNT             (HAL_VAU_REG_BASEADDR + 0x4068)
#define HAL_VAU_CHECKSUM            (HAL_VAU_REG_BASEADDR + 0x407c)

#define HAL_VAU_DRAW_PATH_CMD_ADDR  (HAL_VAU_REG_BASEADDR + 0x20C)
#define HAL_VAU_DRAW_PATH_DATA_ADDR (HAL_VAU_REG_BASEADDR + 0x210)
#define HAL_VAU_DRAW_PATH_NUM_ADDR  (HAL_VAU_REG_BASEADDR + 0x21C)

#define HAL_VAU_DRAW_CTRL           (HAL_VAU_REG_BASEADDR + 0x200)
#define HAL_VAU_HSV_CTRL            (HAL_VAU_REG_BASEADDR + 0x374)

#define HAL_VAU_DRAW_THRESHOLD_CUBIC (HAL_VAU_REG_BASEADDR + 0x26C)

/* Define the union vau_src1_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_fmt                : 6;       /* [5:0] */
        unsigned int src1_argb_order         : 5;       /* [10:6] */
        unsigned int src1_premult_en         : 1;       /* [11] */
        unsigned int src1_rgb_exp            : 2;       /* [13:12] */
        unsigned int reserved0               : 2;       /* [15:14] */
        unsigned int src1_t2r_bind_en        : 1;       /* [16] */
        unsigned int reserved1               : 1;       /* [17] */
        unsigned int src1_alpha_range        : 1;       /* [18] */
        unsigned int src1_v_scan_ord         : 1;       /* [19] */
        unsigned int src1_h_scan_ord         : 1;       /* [20] */
        unsigned int reserved2               : 2;       /* [22:21] */
        unsigned int hdr_en                  : 1;       /* [23] */
        unsigned int reserved3               : 2;       /* [25:24] */
        unsigned int src1_cmp_mode           : 1;       /* [26] */
        unsigned int src1_ice_en             : 1;       /* [27] */
        unsigned int src1_fast_req           : 1;       /* [28] */
        unsigned int src1_mode               : 2;       /* [30:29] */
        unsigned int src1_en                 : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src1_ctrl;

/* Define the union vau_src1_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_addr               : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src1_addr;

/* Define the union vau_src1_stride */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_stride             : 20;      /* [19:0] */
        unsigned int reserved                : 12;      /* [31:20] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src1_stride;

/* Define the union vau_src1_imgsize */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_width              : 16;      /* [15:0] */
        unsigned int src1_height             : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src1_imgsize;

/* Define the union vau_src1_rect_xy */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_rect_x             : 16;      /* [15:0] */
        unsigned int src1_rect_y             : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src1_rect_xy;

/* Define the union vau_src1_rect_wh */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_rect_width         : 16;      /* [15:0] */
        unsigned int src1_rect_height        : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src1_rect_wh;

/* Define the union vau_src1_fill */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_color_fill         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src1_fill;

/* Define the union vau_src1_alpha */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_alpha0             : 8;       /* [7:0] */
        unsigned int src1_alpha1             : 8;       /* [15:8] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src1_alpha;

/* Define the union vau_src1_pix_rgb */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_pix_rgb            : 24;      /* [23:0] */
        unsigned int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src1_pix_rgb;

/* Define the union vau_src1_htc_online_dcmp0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_dcmp_pix_format    : 3;       /* [2:0] */
        unsigned int src1_dcmp_bit_depth     : 1;       /* [3] */
        unsigned int src1_block_size         : 6;       /* [9:4] */
        unsigned int src1_dcmp_mpde          : 1;       /* [10] */
        unsigned int src1_dcmp_pq_sel        : 1;       /* [11] */
        unsigned int reserved                : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src1_htc_online_dcmp0;

/* Define the union vau_src2_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_fmt                : 6;       /* [5:0] */
        unsigned int src2_argb_order         : 5;       /* [10:6] */
        unsigned int src2_premult_en         : 1;       /* [11] */
        unsigned int src2_rgb_exp            : 2;       /* [13:12] */
        unsigned int src2_clut_mode          : 1;       /* [14] */
        unsigned int reserved0               : 1;       /* [15] */
        unsigned int src2_t2r_bind_en        : 1;       /* [16] */
        unsigned int reserved1               : 1;       /* [17] */
        unsigned int src2_alpha_range        : 1;       /* [18] */
        unsigned int src2_v_scan_ord         : 1;       /* [19] */
        unsigned int src2_h_scan_ord         : 1;       /* [20] */
        unsigned int src2_color_fill_en      : 1;       /* [21] */
        unsigned int src2_cbcr_order         : 1;       /* [22] */
        unsigned int src2_mapping_perf_en    : 1;       /* [23] */
        unsigned int src2_mapping_en         : 1;       /* [24] */
        unsigned int src2_ccopy_en           : 1;       /* [25] */
        unsigned int src2_cmp_mode           : 1;       /* [26] */
        unsigned int src2_ice_en             : 1;       /* [27] */
        unsigned int src2_fast_req           : 1;       /* [28] */
        unsigned int src2_mode               : 2;       /* [30:29] */
        unsigned int src2_en                 : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src2_ctrl;

/* Define the union vau_src2_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_addr               : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src2_addr;

/* Define the union vau_src2_stride */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_stride             : 20;      /* [19:0] */
        unsigned int reserved                : 12;      /* [31:20] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src2_stride;

/* Define the union vau_src2_imgsize */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_width              : 16;      /* [15:0] */
        unsigned int src2_height             : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src2_imgsize;

/* Define the union vau_src2_rect_xy */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_rect_x             : 16;      /* [15:0] */
        unsigned int src2_rect_y             : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src2_rect_xy;

/* Define the union vau_src2_rect_wh */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_rect_width         : 16;      /* [15:0] */
        unsigned int src2_rect_height        : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src2_rect_wh;

/* Define the union vau_src2_fill */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_color_fill         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src2_fill;

/* Define the union vau_src2_alpha */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_alpha0             : 8;       /* [7:0] */
        unsigned int src2_alpha1             : 8;       /* [15:8] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src2_alpha;

/* Define the union vau_src2_pix_rgb */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_pix_rgb            : 24;      /* [23:0] */
        unsigned int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src2_pix_rgb;

/* Define the union vau_src2_dcmp_cfg0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_dcmp_cfg0          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src2_dcmp_cfg0;

/* Define the union vau_src2_dcmp_cfg1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_dcmp_cfg1          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src2_dcmp_cfg1;

/* Define the union vau_src2_dcmp_cfg2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_dcmp_cfg2          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src2_dcmp_cfg2;

/* Define the union vau_src2_dcmp_cfg3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_dcmp_cfg3          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src2_dcmp_cfg3;

/* Define the union vau_src2_htc_online_dcmp0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_dcmp_pix_format    : 3;       /* [2:0] */
        unsigned int src2_dcmp_bit_depth     : 1;       /* [3] */
        unsigned int src2_block_size         : 6;       /* [9:4] */
        unsigned int src2_dcmp_mpde          : 1;       /* [10] */
        unsigned int src2_dcmp_pq_sel        : 1;       /* [11] */
        unsigned int reserved                : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src2_htc_online_dcmp0;

/* Define the union vau_model_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved                : 31;      /* [30:0] */
        unsigned int model_en                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_model_ctrl;

/* Define the union vau_model_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int model_addr              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_model_addr;

/* Define the union vau_model_stride */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int model_stride            : 20;      /* [19:0] */
        unsigned int reserved                : 12;      /* [31:20] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_model_stride;

/* Define the union vau_des_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int des_fmt                 : 6;       /* [5:0] */
        unsigned int des_argb_order          : 5;       /* [10:6] */
        unsigned int des_premult_en          : 1;       /* [11] */
        unsigned int des_bind_en             : 1;       /* [12] */
        unsigned int des_bind_mode           : 1;       /* [13] */
        unsigned int display_node            : 1;       /* [14] */
        unsigned int tunl_en                 : 1;       /* [15] */
        unsigned int des_t2r_bind_en         : 1;       /* [16] */
        unsigned int des_rgb_round           : 1;       /* [17] */
        unsigned int des_alpha_range         : 1;       /* [18] */
        unsigned int des_v_scan_ord          : 1;       /* [19] */
        unsigned int des_h_scan_ord          : 1;       /* [20] */
        unsigned int alpha_strb_en           : 1;       /* [21] */
        unsigned int draw_t2r_bind_en        : 1;       /* [22] */
        unsigned int des_rgb_trunc           : 1;       /* [23] */
        unsigned int reserved0               : 3;       /* [26:24] */
        unsigned int des_ice_en              : 1;       /* [27] */
        unsigned int des_fast_req            : 1;       /* [28] */
        unsigned int reserved1               : 2;       /* [30:29] */
        unsigned int des_en                  : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_des_ctrl;

/* Define the union vau_des_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int des_addr                : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_des_addr;

/* Define the union vau_des_stride */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int des_stride              : 20;      /* [19:0] */
        unsigned int reserved                : 12;      /* [31:20] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_des_stride;

/* Define the union vau_des_imgsize */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int des_width               : 16;      /* [15:0] */
        unsigned int des_height              : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_des_imgsize;

/* Define the union vau_des_rect_xy */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int des_rect_x              : 16;      /* [15:0] */
        unsigned int des_rect_y              : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_des_rect_xy;

/* Define the union vau_des_rect_wh */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int des_rect_width          : 16;      /* [15:0] */
        unsigned int des_rect_height         : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_des_rect_wh;

/* Define the union vau_des_alpha */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int des_alpha_thd           : 8;       /* [7:0] */
        unsigned int reserved                : 24;      /* [31:8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_des_alpha;

/* Define the union vau_des_htc_online_cmp0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int bypass_en               : 1;       /* [0] */
        unsigned int pix_format              : 3;       /* [3:1] */
        unsigned int bit_depth               : 1;       /* [4] */
        unsigned int cmp_pq_sel              : 1;       /* [5] */
        unsigned int reserved0               : 2;       /* [7:6] */
        unsigned int des_block_size          : 6;       /* [13:8] */
        unsigned int reserved1               : 18;      /* [31:14] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_des_htc_online_cmp0;

/* Define the union vau_cbm_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved0               : 21;      /* [20:0] */
        unsigned int alu_mode                : 4;       /* [24:21] */
        unsigned int reserved1               : 6;       /* [30:25] */
        unsigned int cbm_en                  : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_cbm_ctrl;

/* Define the union vau_cbm_colorize */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int colorize_b              : 8;       /* [7:0] */
        unsigned int colorize_g              : 8;       /* [15:8] */
        unsigned int colorize_r              : 8;       /* [23:16] */
        unsigned int reserved                : 7;       /* [30:24] */
        unsigned int colorize_en             : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_cbm_colorize;

/* Define the union vau_cbm_alu_para */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int rgb_rop                 : 4;       /* [3:0] */
        unsigned int a_rop                   : 4;       /* [7:4] */
        unsigned int reserved0               : 8;       /* [15:8] */
        unsigned int alpha_from              : 2;       /* [17:16] */
        unsigned int right_alpha_border_en   : 1;       /* [18] */
        unsigned int left_alpha_border_en    : 1;       /* [19] */
        unsigned int bottom_alpha_border_en  : 1;       /* [20] */
        unsigned int top_alpha_border_en     : 1;       /* [21] */
        unsigned int reserved1               : 9;       /* [30:22] */
        unsigned int blend_rop_en            : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_cbm_alu_para;

/* Define the union vau_cbm_key_para */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int key_b_mode              : 2;       /* [1:0] */
        unsigned int key_g_mode              : 2;       /* [3:2] */
        unsigned int key_r_mode              : 2;       /* [5:4] */
        unsigned int key_a_mode              : 2;       /* [7:6] */
        unsigned int key_sel                 : 2;       /* [9:8] */
        unsigned int reserved                : 21;      /* [30:10] */
        unsigned int key_en                  : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_cbm_key_para;

/* Define the union vau_cbm_key_min */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int key_b_min               : 8;       /* [7:0] */
        unsigned int key_g_min               : 8;       /* [15:8] */
        unsigned int key_r_min               : 8;       /* [23:16] */
        unsigned int key_a_min               : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_cbm_key_min;

/* Define the union vau_cbm_key_max */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int key_b_max               : 8;       /* [7:0] */
        unsigned int key_g_max               : 8;       /* [15:8] */
        unsigned int key_r_max               : 8;       /* [23:16] */
        unsigned int key_a_max               : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_cbm_key_max;

/* Define the union vau_cbm_key_mask */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int key_b_mask              : 8;       /* [7:0] */
        unsigned int key_g_mask              : 8;       /* [15:8] */
        unsigned int key_r_mask              : 8;       /* [23:16] */
        unsigned int key_a_mask              : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_cbm_key_mask;

/* Define the union vau_cbm_src1_cbm_para */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int s1_galpha_en            : 1;       /* [0] */
        unsigned int s1_palpha_en            : 1;       /* [1] */
        unsigned int s1_premult_en           : 1;       /* [2] */
        unsigned int s1_multi_global_en      : 1;       /* [3] */
        unsigned int s1_blend_mode           : 4;       /* [7:4] */
        unsigned int reserved                : 24;      /* [31:8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_cbm_src1_cbm_para;

/* Define the union vau_cbm_src2_cbm_para */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int s2_galpha_en            : 1;       /* [0] */
        unsigned int s2_palpha_en            : 1;       /* [1] */
        unsigned int s2_premult_en           : 1;       /* [2] */
        unsigned int s2_multi_global_en      : 1;       /* [3] */
        unsigned int s2_blend_mode           : 4;       /* [7:4] */
        unsigned int s2_galpha               : 8;       /* [15:8] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_cbm_src2_cbm_para;

/* Define the union vau_gradient_typical_color */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int typical_color           : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_typical_color;

/* Define the union vau_gradient_linear_const_x */
typedef union {
    /* Define the struct bits */
    struct {
        float linear_const_x;      /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_linear_const_x;

/* Define the union vau_gradient_linear_const_y */
typedef union {
    /* Define the struct bits */
    struct {
        float linear_const_y;      /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_linear_const_y;

/* Define the union vau_gradient_linear_start */
typedef union {
    /* Define the struct bits */
    struct {
        signed int linear_start_y          : 16;      /* [15:0] */
        signed int linear_start_x          : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_linear_start;

/* Define the union vau_gradient_const_mode */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int linear_const_mode       : 2;       /* [1:0] */
        unsigned int reserved                : 30;      /* [31:2] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_const_mode;

/* Define the union vau_gradient_radial_const_r */
typedef union {
    /* Define the struct bits */
    struct {
        float radial_const_r;      /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_radial_const_r;

/* Define the union vau_gradient_radial_center */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int radial_center_y         : 10;      /* [9:0] */
        unsigned int reserved0               : 6;       /* [15:10] */
        unsigned int radial_center_x         : 10;      /* [25:16] */
        unsigned int reserved1               : 6;       /* [31:26] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_radial_center;

/* Define the union vau_gradient_sweep_center */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sweep_center_y          : 10;      /* [9:0] */
        unsigned int reserved0               : 6;       /* [15:10] */
        unsigned int sweep_center_x          : 10;      /* [25:16] */
        unsigned int reserved1               : 6;       /* [31:26] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_sweep_center;

/* Define the union vau_gradient_sweep_start_angle */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sweep_start_angle       : 9;       /* [8:0] */
        unsigned int reserved                : 23;      /* [31:9] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_sweep_start_angle;

/* Define the union vau_gradient_tile_mode */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tile_mode               : 2;       /* [1:0] */
        unsigned int src2_fill_effect        : 4;       /* [5:2] */
        unsigned int reserved                : 26;      /* [31:6] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_tile_mode;

/* Define the union vau_gradient_stops_offset0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int stops_offset0           : 8;       /* [7:0] */
        unsigned int stops_offset1           : 8;       /* [15:8] */
        unsigned int stops_offset2           : 8;       /* [23:16] */
        unsigned int stops_offset3           : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_stops_offset0;

/* Define the union vau_gradient_stops_offset1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int stops_offset4           : 8;       /* [7:0] */
        unsigned int stops_offset5           : 8;       /* [15:8] */
        unsigned int stops_offset6           : 8;       /* [23:16] */
        unsigned int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_stops_offset1;

/* Define the union vau_gradient_stops_color0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int stops_color0            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_stops_color0;

/* Define the union vau_gradient_stops_color1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int stops_color1            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_stops_color1;

/* Define the union vau_gradient_stops_color2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int stops_color2            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_stops_color2;

/* Define the union vau_gradient_stops_color3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int stops_color3            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_stops_color3;

/* Define the union vau_gradient_stops_color4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int stops_color4            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_stops_color4;

/* Define the union vau_gradient_stops_color5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int stops_color5            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_stops_color5;

/* Define the union vau_gradient_stops_color6 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int stops_color6            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_stops_color6;

/* Define the union vau_gradient_color_reci0 */
typedef union {
    /* Define the struct bits */
    struct {
        float color_reci01; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_color_reci0;

/* Define the union vau_gradient_color_reci1 */
typedef union {
    /* Define the struct bits */
    struct {
        float color_reci12; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_color_reci1;

/* Define the union vau_gradient_color_reci2 */
typedef union {
    /* Define the struct bits */
    struct {
        float color_reci23; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_color_reci2;

/* Define the union vau_gradient_color_reci3 */
typedef union {
    /* Define the struct bits */
    struct {
        float color_reci34; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_color_reci3;

/* Define the union vau_gradient_color_reci4 */
typedef union {
    /* Define the struct bits */
    struct {
        float color_reci45; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_color_reci4;

/* Define the union vau_gradient_color_reci5 */
typedef union {
    /* Define the struct bits */
    struct {
        float color_reci56; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_color_reci5;

/* Define the union vau_gradient_stops_num */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int stops_num               : 3;       /* [2:0] */
        unsigned int reserved                : 29;      /* [31:3] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_stops_num;

/* Define the union local_lighting_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int local_lighting_en       : 1;       /* [0] */
        unsigned int local_lighting_ck_gt_en : 1;       /* [1] */
        unsigned int bbox_cfg_en             : 1;       /* [2] */
        unsigned int is_x_major              : 1;       /* [3] */
        unsigned int is_bi_dir               : 1;       /* [4] */
        unsigned int lighting_mode           : 3;       /* [7:5] */
        unsigned int bbox0_bbox1_low_power_en : 1;       /* [8] */
        unsigned int reserved                : 23;      /* [31:9] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_local_lighting_cfg;

/* Define the union local_lighting_cx */
typedef union {
    /* Define the struct bits */
    struct {
        float centerx; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_local_lighting_cx;

/* Define the union local_lighting_cy */
typedef union {
    /* Define the struct bits */
    struct {
        float centery; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_local_lighting_cy;

/* Define the union local_lighting_radius2 */
typedef union {
    /* Define the struct bits */
    struct {
        float radius2; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_local_lighting_radius2;

/* Define the union local_lighting_amplitude */
typedef union {
    /* Define the struct bits */
    struct {
        float amplitude; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_local_lighting_amplitude;

/* Define the union local_lighting_ampli_scale */
typedef union {
    /* Define the struct bits */
    struct {
        float amplitude_scale; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_local_lighting_ampli_scale;

/* Define the union local_lighting_start_angle_rad */
typedef union {
    /* Define the struct bits */
    struct {
        float start_angle_rad; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_local_lighting_start_angle_rad;

/* Define the union local_lighting_start_end_angle_rad */
typedef union {
    /* Define the struct bits */
    struct {
        float start_end_angle_rad; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_local_lighting_start_end_angle_rad;

/* Define the union local_lighting_end_start_angle_rad */
typedef union {
    /* Define the struct bits */
    struct {
        float end_start_angle_rad; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_local_lighting_end_start_angle_rad;

/* Define the union local_lighting_end_angle_rad */
typedef union {
    /* Define the struct bits */
    struct {
        float end_angle_rad; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_local_lighting_end_angle_rad;

/* Define the union local_lighting_triwin2_coeff0 */
typedef union {
    /* Define the struct bits */
    struct {
        float triwin2_coeff0; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_local_lighting_triwin2_coeff0;

/* Define the union local_lighting_triwin2_coeff1 */
typedef union {
    /* Define the struct bits */
    struct {
        float triwin2_coeff1; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_local_lighting_triwin2_coeff1;

/* Define the union local_lighting_amb_amp */
typedef union {
    /* Define the struct bits */
    struct {
        float amb_amplitude; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_local_lighting_amb_amp;

/* Define the union local_lighting_bbox0_p0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int bbox0_min_x             : 16;      /* [15:0] */
        unsigned int bbox0_min_y             : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_local_lighting_bbox0_p0;

/* Define the union local_lighting_bbox0_p1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int bbox0_max_x             : 16;      /* [15:0] */
        unsigned int bbox0_max_y             : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_local_lighting_bbox0_p1;

/* Define the union local_lighting_bbox1_p0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int bbox1_min_x             : 16;      /* [15:0] */
        unsigned int bbox1_min_y             : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_local_lighting_bbox1_p0;

/* Define the union local_lighting_bbox1_p1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int bbox1_max_x             : 16;      /* [15:0] */
        unsigned int bbox1_max_y             : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_local_lighting_bbox1_p1;

/* Define the union local_lighting_img_reso */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int image_width             : 16;      /* [15:0] */
        unsigned int image_height            : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_local_lighting_img_reso;

/* Define the union local_lighting_start_pos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int start_pos_x             : 16;      /* [15:0] */
        unsigned int start_pos_y             : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_local_lighting_start_pos;

/* Define the union gauss_blur_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gauss_blur_en           : 1;       /* [0] */
        unsigned int gauss_blur_alpha_en     : 1;       /* [1] */
        unsigned int gauss_blur_dfx_en       : 1;       /* [2] */
        unsigned int gauss_blur_mask_en      : 1;       /* [3] */
        unsigned int reserved                : 27;      /* [30:4] */
        unsigned int gauss_blur_ck_gt_en     : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gauss_blur_ctrl;

/* Define the union gauss_blur_a0 */
typedef union {
    /* Define the struct bits */
    struct {
        float gauss_blur_a0;      /* [31:1] valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gauss_blur_a0;

/* Define the union gauss_blur_a1 */
typedef union {
    /* Define the struct bits */
    struct {
        float gauss_blur_a1;      /* [31:1] valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gauss_blur_a1;

/* Define the union gauss_blur_a2 */
typedef union {
    /* Define the struct bits */
    struct {
        float gauss_blur_a2;      /* [31:1] valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gauss_blur_a2;

/* Define the union gauss_blur_b_causala0 */
typedef union {
    /* Define the struct bits */
    struct {
        float gauss_blur_b_causal0;      /* [31:1] valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gauss_blur_b_causala0;

/* Define the union gauss_blur_b_causala1 */
typedef union {
    /* Define the struct bits */
    struct {
        float gauss_blur_b_causal1;      /* [31:1] valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gauss_blur_b_causala1;

/* Define the union gauss_blur_b_causala2 */
typedef union {
    /* Define the struct bits */
    struct {
        float gauss_blur_b_causal2;      /* [31:1] valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gauss_blur_b_causala2;

/* Define the union gauss_blur_b_anticausala0 */
typedef union {
    /* Define the struct bits */
    struct {
        float gauss_blur_b_anticausal0;      /* [31:1] valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gauss_blur_b_anticausala0;

/* Define the union gauss_blur_b_anticausala1 */
typedef union {
    /* Define the struct bits */
    struct {
        float gauss_blur_b_anticausal1;      /* [31:1] valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gauss_blur_b_anticausala1;

/* Define the union gauss_blur_b_anticausala2 */
typedef union {
    /* Define the struct bits */
    struct {
        float gauss_blur_b_anticausal2;      /* [31:1] valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gauss_blur_b_anticausala2;

/* Define the union gauss_blur_causala_ratio */
typedef union {
    /* Define the struct bits */
    struct {
        float gauss_blur_causal_ratio;      /* [31:1] valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gauss_blur_causala_ratio;

/* Define the union gauss_blur_anticausala_ratio */
typedef union {
    /* Define the struct bits */
    struct {
        float gauss_blur_anticausal_ratio;      /* [31:1] valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gauss_blur_anticausala_ratio;

/* Define the union vau_t2r0_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int t2r0_en                 : 1;       /* [0] */
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
} u_vau_t2r0_ctrl;

/* Define the union vau_trans_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved                : 24;      /* [23:0] */
        unsigned int trans_ck_gt_en          : 1;       /* [24] */
        unsigned int trans_cfg_en            : 1;       /* [25] */
        unsigned int trans_fill_mode         : 1;       /* [26] */
        unsigned int trans1_tile_size        : 2;       /* [28:27] */
        unsigned int trans2_tile_size        : 2;       /* [30:29] */
        unsigned int tile_trans_en           : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_trans_ctrl;

/* Define the union vau_t2r1_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int t2r1_en                 : 1;       /* [0] */
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
} u_vau_t2r1_ctrl;

/* Define the union vau_clut_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int clut_addr               : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_clut_addr;

/* Define the union vau_node_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int list_mode               : 1;       /* [0] */
        unsigned int reso_cfg_en             : 1;       /* [1] */
        unsigned int src1_cfg_en             : 1;       /* [2] */
        unsigned int node_mode               : 2;       /* [4:3] */
        unsigned int nxt_subnode_mask        : 2;       /* [6:5] */
        unsigned int reserved                : 9;       /* [15:7] */
        unsigned int subnode_num             : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_node_cfg;

/* Define the union vau_node_id */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int node_id                 : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_node_id;

/* Define the union vau_intmask */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int node_end_mask           : 1;       /* [0] */
        unsigned int timeout_mask            : 1;       /* [1] */
        unsigned int bus_err_mask            : 1;       /* [2] */
        unsigned int list_end_mask           : 1;       /* [3] */
        unsigned int display_done_mask       : 1;       /* [4] */
        unsigned int tunl_mask               : 1;       /* [5] */
        unsigned int conflict_mask           : 1;       /* [6] */
        unsigned int draw_err_mask           : 1;       /* [7] */
        unsigned int et_oversize_mask        : 1;       /* [8] */
        unsigned int draw_oversize_mask      : 1;       /* [9] */
        unsigned int dcmp_cfg_err_mask       : 1;       /* [10] */
        unsigned int dcmp_bs_err_mask        : 1;       /* [11] */
        unsigned int once_draw_err_mask      : 1;       /* [12] */
        unsigned int once_et_oversize_mask   : 1;       /* [13] */
        unsigned int once_draw_oversize_mask : 1;       /* [14] */
        unsigned int once_dcmp_cfg_err_mask  : 1;       /* [15] */
        unsigned int once_dcmp_bs_err_mask   : 1;       /* [16] */
        unsigned int msubnode_end_mask       : 1;       /* [17] */
        unsigned int reserved                : 14;      /* [31:18] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_intmask;

/* Define the union vau_pnext */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int p_next                  : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_pnext;

/* Define the union vau_fpga_tag */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int fpga_tag                : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_fpga_tag;

/* Define the union vau_hdr_sys_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hdr_1mux2_sel           : 1;       /* [0] */
        unsigned int hdr_1_1mux2_sel         : 1;       /* [1] */
        unsigned int hdr_2mux1_sel           : 1;       /* [2] */
        unsigned int reserved                : 1;       /* [3] */
        unsigned int hdr_mode                : 2;       /* [5:4] */
        unsigned int hdr_cbm_g0_en           : 1;       /* [6] */
        unsigned int hdr_cbm_g1_en           : 1;       /* [7] */
        unsigned int start_dly_cycle         : 8;       /* [15:8] */
        unsigned int hdr_cbm_g0_alpha        : 8;       /* [23:16] */
        unsigned int hdr_cbm_g1_alpha        : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hdr_sys_ctrl;

/* Define the union vau_hdr_sys_ctrl1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sdr_bit_ext_mode        : 2;       /* [1:0] */
        unsigned int base_bit_ext_mode       : 2;       /* [3:2] */
        unsigned int gmap_bit_ext_mode       : 2;       /* [5:4] */
        unsigned int reserved                : 26;      /* [31:6] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hdr_sys_ctrl1;

/* Define the union vau_draw_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved0               : 12;      /* [11:0] */
        unsigned int join_mode               : 2;       /* [13:12] */
        unsigned int cap_mode                : 2;       /* [15:14] */
        unsigned int bdbx_adj_en             : 1;       /* [16] */
        unsigned int path_return_en          : 1;       /* [17] */
        unsigned int cmd_return_en           : 1;       /* [18] */
        unsigned int dyn_bdbx_en             : 1;       /* [19] */
        unsigned int msaa_mode               : 4;       /* [23:20] */
        unsigned int fill_effect             : 4;       /* [27:24] */
        unsigned int fill_mode               : 1;       /* [28] */
        unsigned int reserved1               : 1;       /* [29] */
        unsigned int stroke_en               : 1;       /* [30] */
        unsigned int draw_en                 : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_ctrl;

/* Define the union vau_draw_st */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int draw_x_start            : 16;      /* [15:0] */
        unsigned int draw_y_start            : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_st;

/* Define the union vau_draw_end */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int draw_x_end              : 16;      /* [15:0] */
        unsigned int draw_y_end              : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_end;

/* Define the union vau_draw_cmd_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cmd_addr                : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_cmd_addr;

/* Define the union vau_draw_data_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int data_addr               : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_data_addr;

/* Define the union vau_draw_cmd_addr_offset */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cmd_addr_offset         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_cmd_addr_offset;

/* Define the union vau_draw_data_addr_offset */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int data_addr_offset        : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_data_addr_offset;

/* Define the union vau_draw_path_num */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cmd_num                 : 16;      /* [15:0] */
        unsigned int data_num                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_path_num;

/* Define the union vau_draw_matrix_00 */
typedef union {
    /* Define the struct bits */
    struct {
        float matrix_00; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_matrix_00;

/* Define the union vau_draw_matrix_01 */
typedef union {
    /* Define the struct bits */
    struct {
        float matrix_01; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_matrix_01;

/* Define the union vau_draw_matrix_02 */
typedef union {
    /* Define the struct bits */
    struct {
        float matrix_02; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_matrix_02;

/* Define the union vau_draw_matrix_10 */
typedef union {
    /* Define the struct bits */
    struct {
        float matrix_10; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_matrix_10;

/* Define the union vau_draw_matrix_11 */
typedef union {
    /* Define the struct bits */
    struct {
        float matrix_11; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_matrix_11;

/* Define the union vau_draw_matrix_12 */
typedef union {
    /* Define the struct bits */
    struct {
        float matrix_12; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_matrix_12;

/* Define the union vau_draw_half_width */
typedef union {
    /* Define the struct bits */
    struct {
        float half_width; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_half_width;

/* Define the union vau_tess_line_num_max */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tess_line_num_max       : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_tess_line_num_max;

/* Define the union vau_draw_rnd_m11 */
typedef union {
    /* Define the struct bits */
    struct {
        float rnd_m11; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_rnd_m11;

/* Define the union vau_draw_rnd_m12 */
typedef union {
    /* Define the struct bits */
    struct {
        float rnd_m12; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_rnd_m12;

/* Define the union vau_draw_rnd_m21 */
typedef union {
    /* Define the struct bits */
    struct {
        float rnd_m21; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_rnd_m21;

/* Define the union vau_draw_rnd_m22 */
typedef union {
    /* Define the struct bits */
    struct {
        float rnd_m22; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_rnd_m22;

/* Define the union vau_draw_rnd_d1 */
typedef union {
    /* Define the struct bits */
    struct {
        float rnd_d1; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_rnd_d1;

/* Define the union vau_draw_rnd_d2 */
typedef union {
    /* Define the struct bits */
    struct {
        float rnd_d2; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_rnd_d2;

/* Define the union vau_draw_rnd_e11 */
typedef union {
    /* Define the struct bits */
    struct {
        float rnd_e11; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_rnd_e11;

/* Define the union vau_draw_rnd_e12 */
typedef union {
    /* Define the struct bits */
    struct {
        float rnd_e12; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_rnd_e12;

/* Define the union vau_draw_rnd_e21 */
typedef union {
    /* Define the struct bits */
    struct {
        float rnd_e21; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_rnd_e21;

/* Define the union vau_draw_rnd_e22 */
typedef union {
    /* Define the struct bits */
    struct {
        float rnd_e22; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_rnd_e22;

/* Define the union vau_draw_threshold_quad */
typedef union {
    /* Define the struct bits */
    struct {
        float threshold_quad; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_threshold_quad;

/* Define the union vau_draw_threshold_cubic */
typedef union {
    /* Define the struct bits */
    struct {
        float threshold_cubic; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_threshold_cubic;

/* Define the union vau_draw_miter_limit */
typedef union {
    /* Define the struct bits */
    struct {
        float miter_limit; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_miter_limit;

/* Define the union vau_mapping_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved                : 16;      /* [15:0] */
        unsigned int upsampling_inter_sel_flag : 2;       /* [17:16] */
        unsigned int upsampling_inter_sel_pos : 2;       /* [19:18] */
        unsigned int func_sel                : 3;       /* [22:20] */
        unsigned int pattern_cfg             : 3;       /* [25:23] */
        unsigned int mapping_cfg_en          : 1;       /* [26] */
        unsigned int req_tile_sel            : 1;       /* [27] */
        unsigned int alpha_mapping_mode      : 1;       /* [28] */
        unsigned int tile_linear_sel         : 1;       /* [29] */
        unsigned int mapping_mode            : 1;       /* [30] */
        unsigned int mapping_en              : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mapping_ctrl;

/* Define the union vau_mapping_coef00 */
typedef union {
    /* Define the struct bits */
    struct {
        float mapping_coef00; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mapping_coef00;

/* Define the union vau_mapping_coef01 */
typedef union {
    /* Define the struct bits */
    struct {
        float mapping_coef01; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mapping_coef01;

/* Define the union vau_mapping_coef02 */
typedef union {
    /* Define the struct bits */
    struct {
        float mapping_coef02; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mapping_coef02;

/* Define the union vau_mapping_coef03 */
typedef union {
    /* Define the struct bits */
    struct {
        float mapping_coef03; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mapping_coef03;

/* Define the union vau_mapping_coef10 */
typedef union {
    /* Define the struct bits */
    struct {
        float mapping_coef10; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mapping_coef10;

/* Define the union vau_mapping_coef11 */
typedef union {
    /* Define the struct bits */
    struct {
        float mapping_coef11; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mapping_coef11;

/* Define the union vau_mapping_coef12 */
typedef union {
    /* Define the struct bits */
    struct {
        float mapping_coef12; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mapping_coef12;

/* Define the union vau_mapping_coef13 */
typedef union {
    /* Define the struct bits */
    struct {
        float mapping_coef13; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mapping_coef13;

/* Define the union vau_mapping_coef20 */
typedef union {
    /* Define the struct bits */
    struct {
        float mapping_coef20; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mapping_coef20;

/* Define the union vau_mapping_coef21 */
typedef union {
    /* Define the struct bits */
    struct {
        float mapping_coef21; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mapping_coef21;

/* Define the union vau_mapping_coef22 */
typedef union {
    /* Define the struct bits */
    struct {
        float mapping_coef22; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mapping_coef22;

/* Define the union vau_mapping_coef23 */
typedef union {
    /* Define the struct bits */
    struct {
        float mapping_coef23; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mapping_coef23;

/* Define the union vau_mapping_coef30 */
typedef union {
    /* Define the struct bits */
    struct {
        float mapping_coef30; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mapping_coef30;

/* Define the union vau_mapping_coef31 */
typedef union {
    /* Define the struct bits */
    struct {
        float mapping_coef31; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mapping_coef31;

/* Define the union vau_mapping_coef32 */
typedef union {
    /* Define the struct bits */
    struct {
        float mapping_coef32; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mapping_coef32;

/* Define the union vau_mapping_coef33 */
typedef union {
    /* Define the struct bits */
    struct {
        float mapping_coef33; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mapping_coef33;

/* Define the union vau_mapping_model_wh */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mapping_model_w         : 16;      /* [15:0] */
        unsigned int mapping_model_h         : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mapping_model_wh;

/* Define the union vau_mapping_pattern_fill_color */
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
} u_vau_mapping_pattern_fill_color;

/* Define the union vau_mapping_shadow_color */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int shadow_color_b          : 8;       /* [7:0] */
        unsigned int shadow_color_g          : 8;       /* [15:8] */
        unsigned int shadow_color_r          : 8;       /* [23:16] */
        unsigned int shadow_color_a          : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mapping_shadow_color;

/* Define the union vau_mapping_bg_color */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int bg_color_b              : 8;       /* [7:0] */
        unsigned int bg_color_g              : 8;       /* [15:8] */
        unsigned int bg_color_r              : 8;       /* [23:16] */
        unsigned int bg_color_a              : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mapping_bg_color;

/* Define the union vau_mapping_scale_x */
typedef union {
    /* Define the struct bits */
    struct {
        float x_scale; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mapping_scale_x;

/* Define the union vau_mapping_scale_y */
typedef union {
    /* Define the struct bits */
    struct {
        float y_scale; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mapping_scale_y;

/* Define the union vau_mapping_offset_x */
typedef union {
    /* Define the struct bits */
    struct {
        float x_offset; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mapping_offset_x;

/* Define the union vau_mapping_offset_y */
typedef union {
    /* Define the struct bits */
    struct {
        float y_offset; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mapping_offset_y;

/* Define the union vau_mapping_src_ratio_x */
typedef union {
    /* Define the struct bits */
    struct {
        float src_x_ratio; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mapping_src_ratio_x;

/* Define the union vau_mapping_src_ratio_y */
typedef union {
    /* Define the struct bits */
    struct {
        float src_y_ratio; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mapping_src_ratio_y;

/* Define the union vau_mapping_src_offset_x */
typedef union {
    /* Define the struct bits */
    struct {
        float src_x_offset; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mapping_src_offset_x;

/* Define the union vau_mapping_src_offset_y */
typedef union {
    /* Define the struct bits */
    struct {
        float src_y_offset; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mapping_src_offset_y;

/* Define the union vau_hsv_rgb_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hsv_rgb_en              : 1;       /* [0] */
        unsigned int hsv_rgb_ck_gt_en        : 1;       /* [1] */
        unsigned int hsv_rgb_alpha_en        : 1;       /* [2] */
        unsigned int mask_en                 : 1;       /* [3] */
        unsigned int mask_threshold_en       : 1;       /* [4] */
        unsigned int mask_threshold          : 8;       /* [12:5] */
        unsigned int reserved                : 19;      /* [31:13] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hsv_rgb_cfg;

/* Define the union vau_hsv_rgb_adj_h_v */
typedef union {
    /* Define the struct bits */
    struct {
        signed int hsv_rgb_v_adj           : 9;       /* [8:0] */
        signed int hsv_rgb_h_adj           : 20;      /* [28:9] */
        signed int reserved                : 3;       /* [31:29] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hsv_rgb_adj_h_v;

/* Define the union vau_hsv_rgb_adj_s */
typedef union {
    /* Define the struct bits */
    struct {
        signed int hsv_rgb_s_adj           : 17;      /* [16:0] */
        signed int reserved                : 15;      /* [31:17] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hsv_rgb_adj_s;

/* Define the union vau_src2c_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2c_addr              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src2c_addr;

/* Define the union vau_src2c_stride */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2c_stride            : 20;      /* [19:0] */
        unsigned int reserved                : 12;      /* [31:20] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src2c_stride;

/* Define the union vau_cvfir_vinfo */
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
} u_vau_cvfir_vinfo;

/* Define the union vau_cvfir_vsp */
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
} u_vau_cvfir_vsp;

/* Define the union vau_cvfir_voffset */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vchroma_offset          : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_cvfir_voffset;

/* Define the union vau_cvfir_vboffset */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vbchroma_offset         : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_cvfir_vboffset;

/* Define the union vau_hfir_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved0               : 1;       /* [0] */
        unsigned int hfir_mode               : 2;       /* [2:1] */
        unsigned int mid_en                  : 1;       /* [3] */
        unsigned int ck_gt_en                : 1;       /* [4] */
        unsigned int reserved1               : 27;      /* [31:5] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hfir_ctrl;

/* Define the union vau_hipp_csc_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hipp_csc_en             : 1;       /* [0] */
        unsigned int hipp_csc_demo_en        : 1;       /* [1] */
        unsigned int hipp_csc_ck_gt_en       : 1;       /* [2] */
        unsigned int reserved                : 29;      /* [31:3] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hipp_csc_ctrl;

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

/* Define the union reserved */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved                : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_reserved;

/* Define the union vau_trans_src1_reso */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int trans1_width            : 16;      /* [15:0] */
        unsigned int trans1_height           : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_trans_src1_reso;

/* Define the union vau_trans_src1_pos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int trans1_xfpos            : 16;      /* [15:0] */
        unsigned int trans1_yfpos            : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_trans_src1_pos;

/* Define the union vau_trans_src2_reso */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int trans2_width            : 16;      /* [15:0] */
        unsigned int trans2_height           : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_trans_src2_reso;

/* Define the union vau_trans_src2_crop_xy */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int trans2_crop_x           : 16;      /* [15:0] */
        unsigned int trans2_crop_y           : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_trans_src2_crop_xy;

/* Define the union vau_trans_src2_crop_wh */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int trans2_crop_w           : 16;      /* [15:0] */
        unsigned int trans2_crop_h           : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_trans_src2_crop_wh;

/* Define the union vau_mapping_crop_xy */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mapping_crop_x          : 16;      /* [15:0] */
        unsigned int mapping_crop_y          : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mapping_crop_xy;

/* Define the union vau_mapping_crop_wh */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mapping_crop_w          : 16;      /* [15:0] */
        unsigned int mapping_crop_h          : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mapping_crop_wh;

/* Define the union mapping_src_crop_xy */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mapping_src_crop_x      : 16;      /* [15:0] */
        unsigned int mapping_src_crop_y      : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mapping_src_crop_xy;

/* Define the union mapping_src_crop_wh */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mapping_src_crop_w      : 16;      /* [15:0] */
        unsigned int mapping_src_crop_h      : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mapping_src_crop_wh;

/* Define the union vau_src1_crop_xy */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_crop_x             : 13;      /* [12:0] */
        unsigned int reserved0               : 3;       /* [15:13] */
        unsigned int src1_crop_y             : 13;      /* [28:16] */
        unsigned int reserved1               : 1;       /* [29] */
        unsigned int src1_crop_cfg_en        : 1;       /* [30] */
        unsigned int src1_crop_en            : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src1_crop_xy;

/* Define the union vau_src1_crop_wh */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_crop_w             : 16;      /* [15:0] */
        unsigned int src1_crop_h             : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src1_crop_wh;

/* Define the union vau_src2_crop_xy */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_crop_x             : 13;      /* [12:0] */
        unsigned int reserved0               : 3;       /* [15:13] */
        unsigned int src2_crop_y             : 13;      /* [28:16] */
        unsigned int reserved1               : 1;       /* [29] */
        unsigned int src2_crop_cfg_en        : 1;       /* [30] */
        unsigned int src2_crop_en            : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src2_crop_xy;

/* Define the union vau_src2_crop_wh */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_crop_w             : 16;      /* [15:0] */
        unsigned int src2_crop_h             : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src2_crop_wh;

/* Define the union vau_des_crop_xy */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int des_crop_x              : 13;      /* [12:0] */
        unsigned int reserved0               : 3;       /* [15:13] */
        unsigned int des_crop_y              : 13;      /* [28:16] */
        unsigned int reserved1               : 1;       /* [29] */
        unsigned int des_crop_cfg_en         : 1;       /* [30] */
        unsigned int des_crop_en             : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_des_crop_xy;

/* Define the union vau_des_crop_wh */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int des_crop_w              : 16;      /* [15:0] */
        unsigned int des_crop_h              : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_des_crop_wh;

/* Define the union vau_t2r0_wh */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int t2r0_width              : 16;      /* [15:0] */
        unsigned int t2r0_height             : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_t2r0_wh;

/* Define the union vau_t2r1_wh */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int t2r1_width              : 16;      /* [15:0] */
        unsigned int t2r1_height             : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_t2r1_wh;

/* Define the union vau_src1_add_pix_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_add_pix_en         : 1;       /* [0] */
        unsigned int src1_add_pix_cfg_en     : 1;       /* [1] */
        unsigned int reserved                : 30;      /* [31:2] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src1_add_pix_ctrl;

/* Define the union vau_src1_add_pix_small */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_add_pix_small_height : 16;      /* [15:0] */
        unsigned int src1_add_pix_small_width : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src1_add_pix_small;

/* Define the union vau_src1_add_pix_big */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_add_pix_big_height : 16;      /* [15:0] */
        unsigned int src1_add_pix_big_width  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src1_add_pix_big;

/* Define the union vau_src1_add_pix_rect */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_add_pix_rect_y     : 16;      /* [15:0] */
        unsigned int src1_add_pix_rect_x     : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src1_add_pix_rect;

/* Define the union vau_src1_add_pix_tile */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_add_pix_tile_height : 2;       /* [1:0] */
        unsigned int src1_add_pix_tile_width : 4;       /* [5:2] */
        unsigned int reserved                : 26;      /* [31:6] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src1_add_pix_tile;

/* Define the union vau_src2_add_pix_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_add_pix_en         : 1;       /* [0] */
        unsigned int src2_add_pix_cfg_en     : 1;       /* [1] */
        unsigned int reserved                : 30;      /* [31:2] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src2_add_pix_ctrl;

/* Define the union vau_src2_add_pix_small */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_add_pix_small_height : 16;      /* [15:0] */
        unsigned int src2_add_pix_small_width : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src2_add_pix_small;

/* Define the union vau_src2_add_pix_big */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_add_pix_big_height : 16;      /* [15:0] */
        unsigned int src2_add_pix_big_width  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src2_add_pix_big;

/* Define the union vau_src2_add_pix_rect */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_add_pix_rect_y     : 16;      /* [15:0] */
        unsigned int src2_add_pix_rect_x     : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src2_add_pix_rect;

/* Define the union vau_src2_add_pix_tile */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_add_pix_tile_height : 2;       /* [1:0] */
        unsigned int src2_add_pix_tile_width : 4;       /* [5:2] */
        unsigned int reserved                : 26;      /* [31:6] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src2_add_pix_tile;

/* Define the union vau_des_add_pix_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int des_add_pix_en          : 1;       /* [0] */
        unsigned int des_add_pix_cfg_en      : 1;       /* [1] */
        unsigned int reserved                : 30;      /* [31:2] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_des_add_pix_ctrl;

/* Define the union vau_des_add_pix_small */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int des_add_pix_small_height : 16;      /* [15:0] */
        unsigned int des_add_pix_small_width : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_des_add_pix_small;

/* Define the union vau_des_add_pix_big */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int des_add_pix_big_height  : 16;      /* [15:0] */
        unsigned int des_add_pix_big_width   : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_des_add_pix_big;

/* Define the union vau_des_add_pix_rect */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int des_add_pix_rect_y      : 16;      /* [15:0] */
        unsigned int des_add_pix_rect_x      : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_des_add_pix_rect;

/* Define the union vau_des_add_pix_tile */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int des_add_pix_tile_height : 2;       /* [1:0] */
        unsigned int des_add_pix_tile_width  : 4;       /* [5:2] */
        unsigned int reserved                : 26;      /* [31:6] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_des_add_pix_tile;

/* Define the union vau_draw_add_pix_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int draw_add_pix_en         : 1;       /* [0] */
        unsigned int draw_add_pix_cfg_en     : 1;       /* [1] */
        unsigned int reserved                : 30;      /* [31:2] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_add_pix_ctrl;

/* Define the union vau_draw_add_pix_small */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int draw_add_pix_small_height : 16;      /* [15:0] */
        unsigned int draw_add_pix_small_width : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_add_pix_small;

/* Define the union vau_draw_add_pix_big */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int draw_add_pix_big_height : 16;      /* [15:0] */
        unsigned int draw_add_pix_big_width  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_add_pix_big;

/* Define the union vau_draw_add_pix_rect */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int draw_add_pix_rect_y     : 16;      /* [15:0] */
        unsigned int draw_add_pix_rect_x     : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_add_pix_rect;

/* Define the union vau_draw_add_pix_tile */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int draw_add_pix_tile_height : 2;       /* [1:0] */
        unsigned int draw_add_pix_tile_width : 4;       /* [5:2] */
        unsigned int reserved                : 26;      /* [31:6] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_add_pix_tile;

/* Define the union vau_reso_src1_crop */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_crop_width         : 16;      /* [15:0] */
        unsigned int src1_crop_height        : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_reso_src1_crop;

/* Define the union vau_reso_src2_crop */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_crop_width         : 16;      /* [15:0] */
        unsigned int src2_crop_height        : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_reso_src2_crop;

/* Define the union vau_reso_des_crop */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int des_crop_width          : 16;      /* [15:0] */
        unsigned int des_crop_height         : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_reso_des_crop;

/* Define the union vau_reso_src2_fill_bdbx_start */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_fill_bdbx_y_start  : 16;      /* [15:0] */
        unsigned int src2_fill_bdbx_x_start  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_reso_src2_fill_bdbx_start;

/* Define the union vau_reso_src2_fill_fill_start */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_fill_fill_y_start  : 16;      /* [15:0] */
        unsigned int src2_fill_fill_x_start  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_reso_src2_fill_fill_start;

/* Define the union vau_reso_src2_fill_bdbx_end */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_fill_bdbx_y_end    : 16;      /* [15:0] */
        unsigned int src2_fill_bdbx_x_end    : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_reso_src2_fill_bdbx_end;

/* Define the union vau_reso_src2_fill_fill_end */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_fill_fill_y_end    : 16;      /* [15:0] */
        unsigned int src2_fill_fill_x_end    : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_reso_src2_fill_fill_end;

/* Define the union vau_reso_cbm */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cbm_width               : 16;      /* [15:0] */
        unsigned int cbm_height              : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_reso_cbm;

/* Define the union vau_reso_gauss */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gauss_width             : 16;      /* [15:0] */
        unsigned int gauss_height            : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_reso_gauss;

/* Define the union vau_reso_patch */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int patch_width             : 15;      /* [14:0] */
        unsigned int patch_height            : 15;      /* [29:15] */
        unsigned int patch_en                : 1;       /* [30] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_reso_patch;

/* Define the union vau_reso_patch_rect_xy */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int patch_rect_x            : 16;      /* [15:0] */
        unsigned int patch_rect_y            : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_reso_patch_rect_xy;

/* Define the union vau_reso_patch_rect_wh */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int patch_rect_w            : 16;      /* [15:0] */
        unsigned int patch_rect_h            : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_reso_patch_rect_wh;

/* Define the union vau_start */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int start                   : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_start;

/* Define the union vau_intstate */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int node_end_state          : 1;       /* [0] */
        unsigned int timeout_state           : 1;       /* [1] */
        unsigned int bus_err                 : 1;       /* [2] */
        unsigned int list_end_state          : 1;       /* [3] */
        unsigned int display_done_state      : 1;       /* [4] */
        unsigned int tunl_state              : 1;       /* [5] */
        unsigned int conflict_state          : 1;       /* [6] */
        unsigned int draw_err                : 1;       /* [7] */
        unsigned int et_oversize_state       : 1;       /* [8] */
        unsigned int draw_oversize           : 1;       /* [9] */
        unsigned int dcmp_cfg_err            : 1;       /* [10] */
        unsigned int dcmp_bs_err             : 1;       /* [11] */
        unsigned int once_draw_err           : 1;       /* [12] */
        unsigned int once_et_oversize        : 1;       /* [13] */
        unsigned int once_draw_oversize      : 1;       /* [14] */
        unsigned int once_dcmp_cfg_err       : 1;       /* [15] */
        unsigned int once_dcmp_bs_err        : 1;       /* [16] */
        unsigned int msubnode_end_state      : 1;       /* [17] */
        unsigned int reserved                : 14;      /* [31:18] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_intstate;

/* Define the union vau_intclr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int node_end_clr            : 1;       /* [0] */
        unsigned int timeout_clr             : 1;       /* [1] */
        unsigned int bus_err_clr             : 1;       /* [2] */
        unsigned int list_end_clr            : 1;       /* [3] */
        unsigned int display_done_clr        : 1;       /* [4] */
        unsigned int tunl_clr                : 1;       /* [5] */
        unsigned int conflict_clr            : 1;       /* [6] */
        unsigned int draw_err_clr            : 1;       /* [7] */
        unsigned int et_oversize_clr         : 1;       /* [8] */
        unsigned int draw_oversize_clr       : 1;       /* [9] */
        unsigned int dcmp_cfg_err_clr        : 1;       /* [10] */
        unsigned int dcmp_bs_err_clr         : 1;       /* [11] */
        unsigned int once_draw_err_clr       : 1;       /* [12] */
        unsigned int once_et_oversize_clr    : 1;       /* [13] */
        unsigned int once_draw_oversize_clr  : 1;       /* [14] */
        unsigned int once_dcmp_cfg_err_clr   : 1;       /* [15] */
        unsigned int once_dcmp_bs_err_clr    : 1;       /* [16] */
        unsigned int msubnode_end_clr        : 1;       /* [17] */
        unsigned int reserved                : 14;      /* [31:18] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_intclr;

/* Define the union vau_rawint */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int raw_node_end            : 1;       /* [0] */
        unsigned int raw_timeout             : 1;       /* [1] */
        unsigned int raw_bus_err             : 1;       /* [2] */
        unsigned int raw_list_end            : 1;       /* [3] */
        unsigned int raw_display_done        : 1;       /* [4] */
        unsigned int raw_tunl                : 1;       /* [5] */
        unsigned int raw_conflict            : 1;       /* [6] */
        unsigned int raw_draw_err            : 1;       /* [7] */
        unsigned int raw_et_oversize         : 1;       /* [8] */
        unsigned int raw_draw_oversize       : 1;       /* [9] */
        unsigned int raw_dcmp_cfg_err        : 1;       /* [10] */
        unsigned int raw_dcmp_bs_err         : 1;       /* [11] */
        unsigned int reserved                : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_rawint;

/* Define the union vau_pfcnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pfcnt                   : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_pfcnt;

/* Define the union vau_miscellaneous */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved0               : 2;       /* [1:0] */
        unsigned int list_cksum_en           : 1;       /* [2] */
        unsigned int src1_cksum_en           : 1;       /* [3] */
        unsigned int src2_cksum_en           : 1;       /* [4] */
        unsigned int src2c_cksum_en          : 1;       /* [5] */
        unsigned int model_cksum_en          : 1;       /* [6] */
        unsigned int des_cksum_en            : 1;       /* [7] */
        unsigned int init_timer              : 16;      /* [23:8] */
        unsigned int ck_gt_en                : 1;       /* [24] */
        unsigned int ck_gt_en_calc           : 1;       /* [25] */
        unsigned int ck_gt_en_cfg            : 1;       /* [26] */
        unsigned int ck_gt_en_axi            : 1;       /* [27] */
        unsigned int reserved1               : 4;       /* [31:28] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_miscellaneous;

/* Define the union vau_timeout */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int timeout                 : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_timeout;

/* Define the union vau_node_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int node_cnt                : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_node_cnt;

/* Define the union vau_memctrl_c28 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved                : 1;       /* [0] */
        unsigned int rfs_tm                  : 1;       /* [1] */
        unsigned int rfs_tra                 : 2;       /* [3:2] */
        unsigned int rfs_trdt                : 1;       /* [4] */
        unsigned int rfs_tselr               : 3;       /* [7:5] */
        unsigned int rfs_tselw               : 2;       /* [9:8] */
        unsigned int rfs_twdt                : 1;       /* [10] */
        unsigned int ras_tm                  : 1;       /* [11] */
        unsigned int ras_tra                 : 2;       /* [13:12] */
        unsigned int ras_trdt                : 1;       /* [14] */
        unsigned int ras_tselr               : 3;       /* [17:15] */
        unsigned int ras_tselw               : 2;       /* [19:18] */
        unsigned int ras_twdt                : 1;       /* [20] */
        unsigned int rft_tselr               : 3;       /* [23:21] */
        unsigned int rft_tselw               : 2;       /* [25:24] */
        unsigned int rft_tmr                 : 1;       /* [26] */
        unsigned int rft_tmw                 : 1;       /* [27] */
        unsigned int rft_twdt                : 1;       /* [28] */
        unsigned int rft_trdt                : 1;       /* [29] */
        unsigned int rft_tra                 : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_memctrl_c28;

/* Define the union vau_memctrl1_c28 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int rft_kp                  : 2;       /* [1:0] */
        unsigned int rat_tselr               : 3;       /* [4:2] */
        unsigned int rat_tselw               : 2;       /* [6:5] */
        unsigned int rat_tselm               : 2;       /* [8:7] */
        unsigned int rat_tmr                 : 1;       /* [9] */
        unsigned int rat_tmw                 : 1;       /* [10] */
        unsigned int rat_twdt                : 1;       /* [11] */
        unsigned int rat_trdt                : 1;       /* [12] */
        unsigned int rat_tra                 : 2;       /* [14:13] */
        unsigned int reserved                : 17;      /* [31:15] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_memctrl1_c28;

/* Define the union vau_soft_rst_state */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int soft_rst_state          : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_soft_rst_state;

/* Define the union vau_memctrl_c1x */
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
} u_vau_memctrl_c1x;

/* Define the union vau_memctrl1_c1x */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int c1x_hcbpsb_test         : 3;       /* [2:0] */
        unsigned int c1x_hcbpsb_teslw        : 2;       /* [4:3] */
        unsigned int c1x_hcbpsb_teslr        : 3;       /* [7:5] */
        unsigned int c1x_hcbpsb_tra          : 2;       /* [9:8] */
        unsigned int c1x_hcbpsb_tselm        : 2;       /* [11:10] */
        unsigned int reserved                : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_memctrl1_c1x;

/* Define the union offline_node_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mask_cfg_en             : 1;       /* [0] */
        unsigned int node_addr_offset        : 15;      /* [15:1] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_offline_node_cfg;

/* Define the union cbm_mask_00_0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cbm_mask_00_0           : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_cbm_mask_00_0;

/* Define the union cbm_mask_00_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cbm_mask_00_1           : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_cbm_mask_00_1;

/* Define the union cbm_mask_00_2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cbm_mask_00_2           : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_cbm_mask_00_2;

/* Define the union cbm_mask_00_3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cbm_mask_00_3           : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_cbm_mask_00_3;

/* Define the union cbm_mask_00_4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cbm_mask_00_4           : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_cbm_mask_00_4;

/* Define the union cbm_mask_01_0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cbm_mask_01_0           : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_cbm_mask_01_0;

/* Define the union cbm_mask_01_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cbm_mask_01_1           : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_cbm_mask_01_1;

/* Define the union cbm_mask_01_2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cbm_mask_01_2           : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_cbm_mask_01_2;

/* Define the union cbm_mask_01_3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cbm_mask_01_3           : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_cbm_mask_01_3;

/* Define the union cbm_mask_01_4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cbm_mask_01_4           : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_cbm_mask_01_4;

/* Define the union draw_mask_00_0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int draw_mask_00_0          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_draw_mask_00_0;

/* Define the union draw_mask_00_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int draw_mask_00_1          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_draw_mask_00_1;

/* Define the union draw_mask_00_2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int draw_mask_00_2          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_draw_mask_00_2;

/* Define the union draw_mask_00_3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int draw_mask_00_3          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_draw_mask_00_3;

/* Define the union draw_mask_00_4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int draw_mask_00_4          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_draw_mask_00_4;

/* Define the union draw_mask_01_0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int draw_mask_01_0          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_draw_mask_01_0;

/* Define the union draw_mask_01_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int draw_mask_01_1          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_draw_mask_01_1;

/* Define the union draw_mask_01_2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int draw_mask_01_2          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_draw_mask_01_2;

/* Define the union draw_mask_01_3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int draw_mask_01_3          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_draw_mask_01_3;

/* Define the union draw_mask_01_4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int draw_mask_01_4          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_draw_mask_01_4;

/* Define the union draw_mask_10_0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int draw_mask_10_0          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_draw_mask_10_0;

/* Define the union draw_mask_10_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int draw_mask_10_1          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_draw_mask_10_1;

/* Define the union draw_mask_10_2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int draw_mask_10_2          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_draw_mask_10_2;

/* Define the union draw_mask_10_3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int draw_mask_10_3          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_draw_mask_10_3;

/* Define the union draw_mask_10_4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int draw_mask_10_4          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_draw_mask_10_4;

/* Define the union vau_mst_outstanding */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mstr_routstanding       : 4;       /* [3:0] */
        unsigned int mstr_woutstanding       : 4;       /* [7:4] */
        unsigned int reserved                : 24;      /* [31:8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mst_outstanding;

/* Define the union vau_mst_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int split_mode              : 4;       /* [3:0] */
        unsigned int arb_mode                : 4;       /* [7:4] */
        unsigned int mid_enable              : 1;       /* [8] */
        unsigned int reserved                : 23;      /* [31:9] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mst_ctrl;

/* Define the union vau_mst_rchn_prio */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int rchn0_prio              : 1;       /* [0] */
        unsigned int rchn1_prio              : 1;       /* [1] */
        unsigned int rchn2_prio              : 1;       /* [2] */
        unsigned int rchn3_prio              : 1;       /* [3] */
        unsigned int rchn4_prio              : 1;       /* [4] */
        unsigned int rchn5_prio              : 1;       /* [5] */
        unsigned int rchn6_prio              : 1;       /* [6] */
        unsigned int rchn7_prio              : 1;       /* [7] */
        unsigned int rchn8_prio              : 1;       /* [8] */
        unsigned int rchn9_prio              : 1;       /* [9] */
        unsigned int rchn10_prio             : 1;       /* [10] */
        unsigned int rchn11_prio             : 1;       /* [11] */
        unsigned int rchn12_prio             : 1;       /* [12] */
        unsigned int rchn13_prio             : 1;       /* [13] */
        unsigned int rchn14_prio             : 1;       /* [14] */
        unsigned int rchn15_prio             : 1;       /* [15] */
        unsigned int rchn16_prio             : 1;       /* [16] */
        unsigned int rchn17_prio             : 1;       /* [17] */
        unsigned int rchn18_prio             : 1;       /* [18] */
        unsigned int rchn19_prio             : 1;       /* [19] */
        unsigned int rchn20_prio             : 1;       /* [20] */
        unsigned int rchn21_prio             : 1;       /* [21] */
        unsigned int rchn22_prio             : 1;       /* [22] */
        unsigned int rchn23_prio             : 1;       /* [23] */
        unsigned int rchn24_prio             : 1;       /* [24] */
        unsigned int rchn25_prio             : 1;       /* [25] */
        unsigned int rchn26_prio             : 1;       /* [26] */
        unsigned int rchn27_prio             : 1;       /* [27] */
        unsigned int rchn28_prio             : 1;       /* [28] */
        unsigned int rchn29_prio             : 1;       /* [29] */
        unsigned int rchn30_prio             : 1;       /* [30] */
        unsigned int rchn31_prio             : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mst_rchn_prio;

/* Define the union vau_mst_wchn_prio */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int wchn0_prio              : 1;       /* [0] */
        unsigned int wchn1_prio              : 1;       /* [1] */
        unsigned int wchn2_prio              : 1;       /* [2] */
        unsigned int wchn3_prio              : 1;       /* [3] */
        unsigned int wchn4_prio              : 1;       /* [4] */
        unsigned int wchn5_prio              : 1;       /* [5] */
        unsigned int wchn6_prio              : 1;       /* [6] */
        unsigned int wchn7_prio              : 1;       /* [7] */
        unsigned int wchn8_prio              : 1;       /* [8] */
        unsigned int wchn9_prio              : 1;       /* [9] */
        unsigned int wchn10_prio             : 1;       /* [10] */
        unsigned int wchn11_prio             : 1;       /* [11] */
        unsigned int wchn12_prio             : 1;       /* [12] */
        unsigned int wchn13_prio             : 1;       /* [13] */
        unsigned int wchn14_prio             : 1;       /* [14] */
        unsigned int wchn15_prio             : 1;       /* [15] */
        unsigned int wchn16_prio             : 1;       /* [16] */
        unsigned int wchn17_prio             : 1;       /* [17] */
        unsigned int wchn18_prio             : 1;       /* [18] */
        unsigned int wchn19_prio             : 1;       /* [19] */
        unsigned int wchn20_prio             : 1;       /* [20] */
        unsigned int wchn21_prio             : 1;       /* [21] */
        unsigned int wchn22_prio             : 1;       /* [22] */
        unsigned int wchn23_prio             : 1;       /* [23] */
        unsigned int wchn24_prio             : 1;       /* [24] */
        unsigned int wchn25_prio             : 1;       /* [25] */
        unsigned int wchn26_prio             : 1;       /* [26] */
        unsigned int wchn27_prio             : 1;       /* [27] */
        unsigned int wchn28_prio             : 1;       /* [28] */
        unsigned int wchn29_prio             : 1;       /* [29] */
        unsigned int wchn30_prio             : 1;       /* [30] */
        unsigned int wchn31_prio             : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mst_wchn_prio;

/* Define the union vau_mst_bus_err_clr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int bus_error_clr           : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mst_bus_err_clr;

/* Define the union vau_mst_bus_err */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mst_r_error             : 1;       /* [0] */
        unsigned int mst_w_error             : 1;       /* [1] */
        unsigned int reserved                : 30;      /* [31:2] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mst_bus_err;

/* Define the union vau_mst_src0_status0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src0_status0            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mst_src0_status0;

/* Define the union vau_mst_src0_status1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src0_status1            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mst_src0_status1;

/* Define the union vau_mst_debug_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int axi_det_enable          : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mst_debug_ctrl;

/* Define the union vau_mst_debug_clr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int axi_det_clr             : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mst_debug_clr;

/* Define the union vau_mst_det_latency */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int axi_mst_latency         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mst_det_latency;

/* Define the union vau_mst_rd_info */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mst_rd_info             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mst_rd_info;

/* Define the union vau_mst_wr_info */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mst_wr_info             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mst_wr_info;

/* Define the union vau_mst_det_latency0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mst_rd_max              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mst_det_latency0;

/* Define the union vau_mst_det_latency1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mst_rd_aver             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mst_det_latency1;

/* Define the union vau_mst_det_latency2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mst_rd_cmd              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mst_det_latency2;

/* Define the union vau_mst_det_latency3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mst_wr_max              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mst_det_latency3;

/* Define the union vau_mst_det_latency4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mst_wr_aver             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mst_det_latency4;

/* Define the union vau_mst_det_latency5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mst_wr_cmd              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mst_det_latency5;

/* Define the union vau_src_cmd_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src_cmd_interval        : 16;      /* [15:0] */
        unsigned int reserved                : 15;      /* [30:16] */
        unsigned int src_cmd_ctrl_en         : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src_cmd_ctrl;

/* Define the union vau_des_cmd_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int des_cmd_interval        : 16;      /* [15:0] */
        unsigned int reserved                : 15;      /* [30:16] */
        unsigned int des_cmd_ctrl_en         : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_des_cmd_ctrl;

/* Define the union vau_des_safe_dist */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int des_safe_dist           : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_des_safe_dist;

/* Define the union vau_des_safe_dist_inverse */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int des_safe_dist_inverse   : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_des_safe_dist_inverse;

/* Define the union vau_des_tunl_dist */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int des_tunl_dist           : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_des_tunl_dist;

/* Define the union vau_cvfir_vcoef0 */
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
} u_vau_cvfir_vcoef0;

/* Define the union vau_cvfir_vcoef1 */
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
} u_vau_cvfir_vcoef1;

/* Define the union vau_cvfir_vcoef2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vccoef13                : 10;      /* [9:0] */
        unsigned int vccoef12                : 10;      /* [19:10] */
        unsigned int reserved                : 12;      /* [31:20] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_cvfir_vcoef2;

/* Define the union vau_hfircoef01 */
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
} u_vau_hfircoef01;

/* Define the union vau_hfircoef23 */
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
} u_vau_hfircoef23;

/* Define the union vau_hfircoef45 */
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
} u_vau_hfircoef45;

/* Define the union vau_hfircoef67 */
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
} u_vau_hfircoef67;

/* Define the union vau_hipp_csc_coef00 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int hipp_csc_coef00           : 12;      /* [11:0] */
        signed int reserved                  : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hipp_csc_coef00;

/* Define the union vau_hipp_csc_coef01 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int hipp_csc_coef01           : 12;      /* [11:0] */
        signed int reserved                  : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hipp_csc_coef01;

/* Define the union vau_hipp_csc_coef02 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int hipp_csc_coef02           : 12;      /* [11:0] */
        signed int reserved                  : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hipp_csc_coef02;

/* Define the union vau_hipp_csc_coef10 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int hipp_csc_coef10           : 12;      /* [11:0] */
        signed int reserved                  : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hipp_csc_coef10;

/* Define the union vau_hipp_csc_coef11 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int hipp_csc_coef11           : 12;      /* [11:0] */
        signed int reserved                  : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hipp_csc_coef11;

/* Define the union vau_hipp_csc_coef12 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int hipp_csc_coef12           : 12;      /* [11:0] */
        signed int reserved                  : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hipp_csc_coef12;

/* Define the union vau_hipp_csc_coef20 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int hipp_csc_coef20           : 12;      /* [11:0] */
        signed int reserved                  : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hipp_csc_coef20;

/* Define the union vau_hipp_csc_coef21 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int hipp_csc_coef21           : 12;      /* [11:0] */
        signed int reserved                  : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hipp_csc_coef21;

/* Define the union vau_hipp_csc_coef22 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int hipp_csc_coef22           : 12;      /* [11:0] */
        signed int reserved                  : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hipp_csc_coef22;

/* Define the union vau_hipp_csc_scale */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hipp_csc_scale          : 4;       /* [3:0] */
        unsigned int reserved                : 28;      /* [31:4] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hipp_csc_scale;

/* Define the union vau_hipp_csc_idc0 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int hipp_csc_idc0             : 11;      /* [10:0] */
        signed int reserved                  : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hipp_csc_idc0;

/* Define the union vau_hipp_csc_idc1 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int hipp_csc_idc1             : 11;      /* [10:0] */
        signed int reserved                  : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hipp_csc_idc1;

/* Define the union vau_hipp_csc_idc2 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int hipp_csc_idc2             : 11;      /* [10:0] */
        signed int reserved                  : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hipp_csc_idc2;

/* Define the union vau_hipp_csc_odc0 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int hipp_csc_odc0             : 11;      /* [10:0] */
        signed int reserved                  : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hipp_csc_odc0;

/* Define the union vau_hipp_csc_odc1 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int hipp_csc_odc1             : 11;      /* [10:0] */
        signed int reserved                  : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hipp_csc_odc1;

/* Define the union vau_hipp_csc_odc2 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int hipp_csc_odc2             : 11;      /* [10:0] */
        signed int reserved                  : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hipp_csc_odc2;

/* Define the union vau_hipp_csc_min_y */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hipp_csc_min_y          : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hipp_csc_min_y;

/* Define the union vau_hipp_csc_min_c */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hipp_csc_min_c          : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hipp_csc_min_c;

/* Define the union vau_hipp_csc_max_y */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hipp_csc_max_y          : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hipp_csc_max_y;

/* Define the union vau_hipp_csc_max_c */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hipp_csc_max_c          : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hipp_csc_max_c;

/* Define the union vau_hipp_csc2_coef00 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int hipp_csc2_coef00          : 16;      /* [15:0] */
        signed int reserved                  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hipp_csc2_coef00;

/* Define the union vau_hipp_csc2_coef01 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int hipp_csc2_coef01         : 16;      /* [15:0] */
        signed int reserved                  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hipp_csc2_coef01;

/* Define the union vau_hipp_csc2_coef02 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int hipp_csc2_coef02          : 16;      /* [15:0] */
        signed int reserved                  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hipp_csc2_coef02;

/* Define the union vau_hipp_csc2_coef10 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int hipp_csc2_coef10          : 16;      /* [15:0] */
        signed int reserved                  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hipp_csc2_coef10;

/* Define the union vau_hipp_csc2_coef11 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int hipp_csc2_coef11          : 16;      /* [15:0] */
        signed int reserved                  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hipp_csc2_coef11;

/* Define the union vau_hipp_csc2_coef12 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int hipp_csc2_coef12          : 16;      /* [15:0] */
        signed int reserved                  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hipp_csc2_coef12;

/* Define the union vau_hipp_csc2_coef20 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int hipp_csc2_coef20          : 16;      /* [15:0] */
        signed int reserved                  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hipp_csc2_coef20;

/* Define the union vau_hipp_csc2_coef21 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int hipp_csc2_coef21          : 16;      /* [15:0] */
        signed int reserved                  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hipp_csc2_coef21;

/* Define the union vau_hipp_csc2_coef22 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int hipp_csc2_coef22          : 16;      /* [15:0] */
        signed int reserved                  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hipp_csc2_coef22;

/* Define the union vau_hipp_csc2_scale */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hipp_csc2_scale         : 4;       /* [3:0] */
        unsigned int reserved                : 28;      /* [31:4] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hipp_csc2_scale;

/* Define the union vau_hipp_csc2_idc0 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int hipp_csc2_idc0            : 11;      /* [10:0] */
        signed int reserved                  : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hipp_csc2_idc0;

/* Define the union vau_hipp_csc2_idc1 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int hipp_csc2_idc1            : 11;      /* [10:0] */
        signed int reserved                  : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hipp_csc2_idc1;

/* Define the union vau_hipp_csc2_idc2 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int hipp_csc2_idc2            : 11;      /* [10:0] */
        signed int reserved                  : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hipp_csc2_idc2;

/* Define the union vau_hipp_csc2_odc0 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int hipp_csc2_odc0            : 11;      /* [10:0] */
        signed int reserved                  : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hipp_csc2_odc0;

/* Define the union vau_hipp_csc2_odc1 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int hipp_csc2_odc1            : 11;      /* [10:0] */
        signed int reserved                  : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hipp_csc2_odc1;

/* Define the union vau_hipp_csc2_odc2 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int hipp_csc2_odc2            : 11;      /* [10:0] */
        signed int reserved                  : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hipp_csc2_odc2;

/* Define the union vau_hipp_csc2_min_y */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hipp_csc2_min_y         : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hipp_csc2_min_y;

/* Define the union vau_hipp_csc2_min_c */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hipp_csc2_min_c         : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hipp_csc2_min_c;

/* Define the union vau_hipp_csc2_max_y */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hipp_csc2_max_y         : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hipp_csc2_max_y;

/* Define the union vau_hipp_csc2_max_c */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hipp_csc2_max_c         : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hipp_csc2_max_c;

/* Define the union vau_hipp_csc_ink_ctrl */
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
} u_vau_hipp_csc_ink_ctrl;

/* Define the union vau_hipp_csc_ink_pos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int x_pos                   : 16;      /* [15:0] */
        unsigned int y_pos                   : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hipp_csc_ink_pos;

/* Define the union vau_hipp_csc_ink_data */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ink_data                : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hipp_csc_ink_data;

/* Define the union vau_hipp_csc_ink_data2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ink_data2               : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hipp_csc_ink_data2;

/* Define the union vau_src1_htc_online_dcmp1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved0               : 8;       /* [7:0] */
        unsigned int src1_2x_transform_skip  : 1;       /* [8] */
        unsigned int src1_2x_iter_protect_csc_en : 1;       /* [9] */
        unsigned int src1_2x_flat_protect_en : 1;       /* [10] */
        unsigned int src1_2x_iter_protect_withqs_en : 1;       /* [11] */
        unsigned int src1_2x_iter_protect_clip_en : 1;       /* [12] */
        unsigned int src1_2x_csc_skip        : 1;       /* [13] */
        unsigned int src1_2x_iter_protect_round_en : 1;       /* [14] */
        unsigned int src1_2x_round_offset    : 16;      /* [30:15] */
        unsigned int reserved1               : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src1_htc_online_dcmp1;

/* Define the union vau_src1_htc_online_dcmp2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved0               : 8;       /* [7:0] */
        unsigned int src1_3x_transform_skip  : 1;       /* [8] */
        unsigned int src1_3x_iter_protect_csc_en : 1;       /* [9] */
        unsigned int src1_3x_flat_protect_en : 1;       /* [10] */
        unsigned int src1_3x_iter_protect_withqs_en : 1;       /* [11] */
        unsigned int src1_3x_iter_protect_clip_en : 1;       /* [12] */
        unsigned int src1_3x_csc_skip        : 1;       /* [13] */
        unsigned int src1_3x_iter_protect_round_en : 1;       /* [14] */
        unsigned int src1_3x_round_offset    : 16;      /* [30:15] */
        unsigned int reserved1               : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src1_htc_online_dcmp2;

/* Define the union vau_src2_htc_online_dcmp1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved0               : 8;       /* [7:0] */
        unsigned int src2_2x_transform_skip  : 1;       /* [8] */
        unsigned int src2_2x_iter_protect_csc_en : 1;       /* [9] */
        unsigned int src2_2x_flat_protect_en : 1;       /* [10] */
        unsigned int src2_2x_iter_protect_withqs_en : 1;       /* [11] */
        unsigned int src2_2x_iter_protect_clip_en : 1;       /* [12] */
        unsigned int src2_2x_csc_skip        : 1;       /* [13] */
        unsigned int src2_2x_iter_protect_round_en : 1;       /* [14] */
        unsigned int src2_2x_round_offset    : 16;      /* [30:15] */
        unsigned int reserved1               : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src2_htc_online_dcmp1;

/* Define the union vau_src2_htc_online_dcmp2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved0               : 8;       /* [7:0] */
        unsigned int src2_3x_transform_skip  : 1;       /* [8] */
        unsigned int src2_3x_iter_protect_csc_en : 1;       /* [9] */
        unsigned int src2_3x_flat_protect_en : 1;       /* [10] */
        unsigned int src2_3x_iter_protect_withqs_en : 1;       /* [11] */
        unsigned int src2_3x_iter_protect_clip_en : 1;       /* [12] */
        unsigned int src2_3x_csc_skip        : 1;       /* [13] */
        unsigned int src2_3x_iter_protect_round_en : 1;       /* [14] */
        unsigned int src2_3x_round_offset    : 16;      /* [30:15] */
        unsigned int reserved1               : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src2_htc_online_dcmp2;

/* Define the union vau_des_htc_online_cmp1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int flat_thresh_2x          : 8;       /* [7:0] */
        unsigned int flat_protect_en_2x      : 1;       /* [8] */
        unsigned int htc_cmp_ck_gt_en_2x     : 1;       /* [9] */
        unsigned int dct_skip_2x             : 1;       /* [10] */
        unsigned int csc_metria_en_2x        : 8;       /* [18:11] */
        unsigned int csc_skip_2x             : 1;       /* [19] */
        unsigned int iter_protect_csc_en_2x  : 1;       /* [20] */
        unsigned int reserved                : 11;      /* [31:21] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_des_htc_online_cmp1;

/* Define the union vau_des_htc_online_cmp2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int flat_thresh_3x          : 8;       /* [7:0] */
        unsigned int flat_protect_en_3x      : 1;       /* [8] */
        unsigned int htc_cmp_ck_gt_en_3x     : 1;       /* [9] */
        unsigned int dct_skip_3x             : 1;       /* [10] */
        unsigned int csc_metria_en_3x        : 8;       /* [18:11] */
        unsigned int csc_skip_3x             : 1;       /* [19] */
        unsigned int iter_protect_csc_en_3x  : 1;       /* [20] */
        unsigned int reserved                : 11;      /* [31:21] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_des_htc_online_cmp2;

/* Define the union local_lighting_adjust_value */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved                : 8;       /* [7:0] */
        unsigned int adjust_value            : 24;      /* [31:8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_local_lighting_adjust_value;

/* Define the union vau_matrix_debug0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pix_cnt_dfx             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_matrix_debug0;

/* Define the union vau_t2r_err_clr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int of_err_clr              : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_t2r_err_clr;

/* Define the union vau_t2r_err_sta */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int of_err                  : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_t2r_err_sta;

/* Define the union vau_t2r_debug1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int t2r_debug1              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_t2r_debug1;

/* Define the union vau_t2r_debug2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int t2r_debug2              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_t2r_debug2;

/* Define the union vau_t2r_debug3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int t2r_debug3              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_t2r_debug3;

/* Define the union vau_t2r_debug4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int t2r_debug4              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_t2r_debug4;

/* Define the union vau_t2r01_err_clr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int t2r_01_of_err_clr       : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_t2r01_err_clr;

/* Define the union vau_t2r01_err_sta */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int t2r_01_of_err           : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_t2r01_err_sta;

/* Define the union vau_t2r01_debug1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int t2r_01_debug1           : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_t2r01_debug1;

/* Define the union vau_t2r01_debug2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int t2r_01_debug2           : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_t2r01_debug2;

/* Define the union vau_t2r01_debug3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int t2r_01_debug3           : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_t2r01_debug3;

/* Define the union vau_t2r01_debug4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int t2r_01_debug4           : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_t2r01_debug4;

/* Define the union vau_draw_sampleradius */
typedef union {
    /* Define the struct bits */
    struct {
        float m_sampleradious; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_sampleradius;

/* Define the union vau_draw_rndr_aa_smpl_x0 */
typedef union {
    /* Define the struct bits */
    struct {
        float sample_x_0; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_rndr_aa_smpl_x0;

/* Define the union vau_draw_rndr_aa_smpl_x1 */
typedef union {
    /* Define the struct bits */
    struct {
        float sample_x_1; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_rndr_aa_smpl_x1;

/* Define the union vau_draw_rndr_aa_smpl_x2 */
typedef union {
    /* Define the struct bits */
    struct {
        float sample_x_2; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_rndr_aa_smpl_x2;

/* Define the union vau_draw_rndr_aa_smpl_x3 */
typedef union {
    /* Define the struct bits */
    struct {
        float sample_x_3; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_rndr_aa_smpl_x3;

/* Define the union vau_draw_rndr_aa_smpl_x4 */
typedef union {
    /* Define the struct bits */
    struct {
        float sample_x_4; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_rndr_aa_smpl_x4;

/* Define the union vau_draw_rndr_aa_smpl_x5 */
typedef union {
    /* Define the struct bits */
    struct {
        float sample_x_5; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_rndr_aa_smpl_x5;

/* Define the union vau_draw_rndr_aa_smpl_x6 */
typedef union {
    /* Define the struct bits */
    struct {
        float sample_x_6; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_rndr_aa_smpl_x6;

/* Define the union vau_draw_rndr_aa_smpl_x7 */
typedef union {
    /* Define the struct bits */
    struct {
        float sample_x_7; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_rndr_aa_smpl_x7;

/* Define the union vau_draw_rndr_aa_smpl_y0 */
typedef union {
    /* Define the struct bits */
    struct {
        float sample_y_0; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_rndr_aa_smpl_y0;

/* Define the union vau_draw_rndr_aa_smpl_y1 */
typedef union {
    /* Define the struct bits */
    struct {
        float sample_y_1; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_rndr_aa_smpl_y1;

/* Define the union vau_draw_rndr_aa_smpl_y2 */
typedef union {
    /* Define the struct bits */
    struct {
        float sample_y_2; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_rndr_aa_smpl_y2;

/* Define the union vau_draw_rndr_aa_smpl_y3 */
typedef union {
    /* Define the struct bits */
    struct {
        float sample_y_3; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_rndr_aa_smpl_y3;

/* Define the union vau_draw_rndr_aa_smpl_y4 */
typedef union {
    /* Define the struct bits */
    struct {
        float sample_y_4; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_rndr_aa_smpl_y4;

/* Define the union vau_draw_rndr_aa_smpl_y5 */
typedef union {
    /* Define the struct bits */
    struct {
        float sample_y_5; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_rndr_aa_smpl_y5;

/* Define the union vau_draw_rndr_aa_smpl_y6 */
typedef union {
    /* Define the struct bits */
    struct {
        float sample_y_6; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_rndr_aa_smpl_y6;

/* Define the union vau_draw_rndr_aa_smpl_y7 */
typedef union {
    /* Define the struct bits */
    struct {
        float sample_y_7; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_rndr_aa_smpl_y7;

/* Define the union vau_draw_cvrg_weight0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int weight3                 : 8;       /* [7:0] */
        unsigned int weight2                 : 8;       /* [15:8] */
        unsigned int weight1                 : 8;       /* [23:16] */
        unsigned int weight0                 : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_cvrg_weight0;

/* Define the union vau_draw_cvrg_weight1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int weight7                 : 8;       /* [7:0] */
        unsigned int weight6                 : 8;       /* [15:8] */
        unsigned int weight5                 : 8;       /* [23:16] */
        unsigned int weight4                 : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_cvrg_weight1;

/* Define the union vau_draw_cmdbuf_start_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cmdbuf_start_addr       : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_cmdbuf_start_addr;

/* Define the union vau_draw_cmdbuf_end_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cmdbuf_end_addr         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_cmdbuf_end_addr;

/* Define the union vau_draw_pathbuf_start_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pathbuf_start_addr      : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_pathbuf_start_addr;

/* Define the union vau_draw_pathbuf_end_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pathbuf_end_addr        : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_pathbuf_end_addr;

/* Define the union vau_draw_rnd_cap_tes_num */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int rnd_tes_num             : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_rnd_cap_tes_num;

/* Define the union vau_draw_tes_angle */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved                : 8;       /* [7:0] */
        unsigned int tes_angle               : 24;      /* [31:8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_tes_angle;

/* Define the union vau_draw_tes_sin */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved                : 8;       /* [7:0] */
        unsigned int sin_value               : 24;      /* [31:8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_tes_sin;

/* Define the union vau_draw_tes_cos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved                : 8;       /* [7:0] */
        unsigned int cos_value               : 24;      /* [31:8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_tes_cos;

/* Define the union vau_draw_et_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tes_et_addr             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_et_addr;

/* Define the union vau_draw_et_bkstart_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tes_et_bkstart_addr     : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_et_bkstart_addr;

/* Define the union vau_draw_et_bkend_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tes_et_bkend_addr       : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_et_bkend_addr;

/* Define the union vau_draw_link_start_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tes_link_start_addr     : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_link_start_addr;

/* Define the union vau_draw_link_end_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tes_link_end_addr       : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_link_end_addr;

/* Define the union vau_draw_bdbx_st */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int draw_bdbx_strx          : 16;      /* [15:0] */
        unsigned int draw_bdbx_stry          : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_bdbx_st;

/* Define the union vau_draw_bdbx_end */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int draw_bdbx_endx          : 16;      /* [15:0] */
        unsigned int draw_bdbx_endy          : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_bdbx_end;

/* Define the union vau_sram_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vau_block_en            : 10;      /* [9:0] */
        unsigned int reserved                : 20;      /* [29:10] */
        unsigned int vau_lp_mode             : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_sram_ctrl;

/* Define the union hdr_vau_hdr_src1_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_fmt                : 6;       /* [5:0] */
        unsigned int src1_argb_order         : 5;       /* [10:6] */
        unsigned int src1_premult_en         : 1;       /* [11] */
        unsigned int src1_rgb_exp            : 2;       /* [13:12] */
        unsigned int src1_clut_mode          : 1;       /* [14] */
        unsigned int reserved0               : 3;       /* [17:15] */
        unsigned int src1_alpha_range        : 1;       /* [18] */
        unsigned int src1_v_scan_ord         : 1;       /* [19] */
        unsigned int src1_h_scan_ord         : 1;       /* [20] */
        unsigned int src1_color_fill_en      : 1;       /* [21] */
        unsigned int src1_cbcr_order         : 1;       /* [22] */
        unsigned int reserved1               : 1;       /* [23] */
        unsigned int src1_mapping_en         : 1;       /* [24] */
        unsigned int src1_ccopy_en           : 1;       /* [25] */
        unsigned int src1_cmp_mode           : 1;       /* [26] */
        unsigned int src1_ice_en             : 1;       /* [27] */
        unsigned int src1_fast_req           : 1;       /* [28] */
        unsigned int src1_mode               : 2;       /* [30:29] */
        unsigned int src1_en                 : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hdr_src1_ctrl;

/* Define the union hdr_vau_hdr_src1_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_addr               : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hdr_src1_addr;

/* Define the union hdr_vau_hdr_src1_stride */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_stride             : 20;      /* [19:0] */
        unsigned int reserved                : 12;      /* [31:20] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hdr_src1_stride;

/* Define the union hdr_vau_hdr_src1c_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1c_addr              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hdr_src1c_addr;

/* Define the union hdr_vau_hdr_src1c_stride */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1c_stride            : 20;      /* [19:0] */
        unsigned int reserved                : 12;      /* [31:20] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hdr_src1c_stride;

/* Define the union hdr_vau_hdr_src1_imgsize */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_width              : 16;      /* [15:0] */
        unsigned int src1_height             : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hdr_src1_imgsize;

/* Define the union hdr_vau_hdr_src1_rect_xy */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_rect_x             : 16;      /* [15:0] */
        unsigned int src1_rect_y             : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hdr_src1_rect_xy;

/* Define the union hdr_vau_hdr_src1_rect_wh */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_rect_width         : 16;      /* [15:0] */
        unsigned int src1_rect_height        : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hdr_src1_rect_wh;

/* Define the union hdr_vau_hdr_src1_fill */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_color_fill         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hdr_src1_fill;

/* Define the union hdr_vau_hdr_src1_alpha */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_alpha0             : 8;       /* [7:0] */
        unsigned int src1_alpha1             : 8;       /* [15:8] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hdr_src1_alpha;

/* Define the union hdr_vau_hdr_src1_pix_rgb */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_pix_rgb            : 24;      /* [23:0] */
        unsigned int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hdr_src1_pix_rgb;

/* Define the union hdr_vau_hdr_src2_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_fmt                : 6;       /* [5:0] */
        unsigned int src2_argb_order         : 5;       /* [10:6] */
        unsigned int src2_premult_en         : 1;       /* [11] */
        unsigned int src2_rgb_exp            : 2;       /* [13:12] */
        unsigned int src2_clut_mode          : 1;       /* [14] */
        unsigned int reserved0               : 3;       /* [17:15] */
        unsigned int src2_alpha_range        : 1;       /* [18] */
        unsigned int src2_v_scan_ord         : 1;       /* [19] */
        unsigned int src2_h_scan_ord         : 1;       /* [20] */
        unsigned int src2_color_fill_en      : 1;       /* [21] */
        unsigned int src2_cbcr_order         : 1;       /* [22] */
        unsigned int reserved1               : 1;       /* [23] */
        unsigned int src2_mapping_en         : 1;       /* [24] */
        unsigned int src2_ccopy_en           : 1;       /* [25] */
        unsigned int src2_cmp_mode           : 1;       /* [26] */
        unsigned int src2_ice_en             : 1;       /* [27] */
        unsigned int src2_fast_req           : 1;       /* [28] */
        unsigned int src2_mode               : 2;       /* [30:29] */
        unsigned int src2_en                 : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hdr_src2_ctrl;

/* Define the union hdr_vau_hdr_src2_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_addr               : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hdr_src2_addr;

/* Define the union hdr_vau_hdr_src2_stride */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_stride             : 20;      /* [19:0] */
        unsigned int reserved                : 12;      /* [31:20] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hdr_src2_stride;

/* Define the union hdr_vau_hdr_src2c_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2c_addr              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hdr_src2c_addr;

/* Define the union hdr_vau_hdr_src2c_stride */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2c_stride            : 20;      /* [19:0] */
        unsigned int reserved                : 12;      /* [31:20] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hdr_src2c_stride;

/* Define the union hdr_vau_hdr_src2_imgsize */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_width              : 16;      /* [15:0] */
        unsigned int src2_height             : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hdr_src2_imgsize;

/* Define the union hdr_vau_hdr_src2_rect_xy */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_rect_x             : 16;      /* [15:0] */
        unsigned int src2_rect_y             : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hdr_src2_rect_xy;

/* Define the union hdr_vau_hdr_src2_rect_wh */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_rect_width         : 16;      /* [15:0] */
        unsigned int src2_rect_height        : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hdr_src2_rect_wh;

/* Define the union hdr_vau_hdr_src2_fill */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_color_fill         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hdr_src2_fill;

/* Define the union hdr_vau_hdr_src2_alpha */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_alpha0             : 8;       /* [7:0] */
        unsigned int src2_alpha1             : 8;       /* [15:8] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hdr_src2_alpha;

/* Define the union hdr_vau_hdr_src2_pix_rgb */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_pix_rgb            : 24;      /* [23:0] */
        unsigned int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hdr_src2_pix_rgb;

/* Define the union hdr_vau_hdr_des_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int des_fmt                 : 6;       /* [5:0] */
        unsigned int des_argb_order          : 5;       /* [10:6] */
        unsigned int des_premult_en          : 1;       /* [11] */
        unsigned int des_bind_en             : 1;       /* [12] */
        unsigned int des_bind_mode           : 1;       /* [13] */
        unsigned int display_node            : 1;       /* [14] */
        unsigned int tunl_en                 : 1;       /* [15] */
        unsigned int reserved0               : 1;       /* [16] */
        unsigned int des_rgb_round           : 1;       /* [17] */
        unsigned int des_alpha_range         : 1;       /* [18] */
        unsigned int des_v_scan_ord          : 1;       /* [19] */
        unsigned int des_h_scan_ord          : 1;       /* [20] */
        unsigned int alpha_strb_en           : 1;       /* [21] */
        unsigned int reserved1               : 5;       /* [26:22] */
        unsigned int des_ice_en              : 1;       /* [27] */
        unsigned int des_fast_req            : 1;       /* [28] */
        unsigned int reserved2               : 2;       /* [30:29] */
        unsigned int des_en                  : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hdr_des_ctrl;

/* Define the union hdr_vau_hdr_des_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int des_addr                : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hdr_des_addr;

/* Define the union hdr_vau_hdr_des_stride */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int des_stride              : 20;      /* [19:0] */
        unsigned int reserved                : 12;      /* [31:20] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hdr_des_stride;

/* Define the union hdr_vau_hdr_des_imgsize */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int des_width               : 16;      /* [15:0] */
        unsigned int des_height              : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hdr_des_imgsize;

/* Define the union hdr_vau_hdr_des_rect_xy */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int des_rect_x              : 16;      /* [15:0] */
        unsigned int des_rect_y              : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hdr_des_rect_xy;

/* Define the union hdr_vau_hdr_des_rect_wh */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int des_rect_width          : 16;      /* [15:0] */
        unsigned int des_rect_height         : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hdr_des_rect_wh;

/* Define the union hdr_vau_hdr_des_alpha */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int des_alpha_thd           : 8;       /* [7:0] */
        unsigned int reserved                : 24;      /* [31:8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hdr_des_alpha;

/* Define the union hdr_vau_cvfir_vinfo */
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
} u_hdr_vau_cvfir_vinfo;

/* Define the union hdr_vau_cvfir_vsp */
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
} u_hdr_vau_cvfir_vsp;

/* Define the union hdr_vau_cvfir_voffset */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vchroma_offset          : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_cvfir_voffset;

/* Define the union hdr_vau_cvfir_vboffset */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vbchroma_offset         : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_cvfir_vboffset;

/* Define the union hdr_vau_hfir_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hfir_en                 : 1;       /* [0] */
        unsigned int hfir_mode               : 2;       /* [2:1] */
        unsigned int mid_en                  : 1;       /* [3] */
        unsigned int ck_gt_en                : 1;       /* [4] */
        unsigned int reserved                : 27;      /* [31:5] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hfir_ctrl;

/* Define the union hdr_vau_hipp_csc_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hipp_csc_en             : 1;       /* [0] */
        unsigned int hipp_csc_demo_en        : 1;       /* [1] */
        unsigned int hipp_csc_ck_gt_en       : 1;       /* [2] */
        unsigned int reserved                : 29;      /* [31:3] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hipp_csc_ctrl;

/* Define the union hdr_vau_hipp_csc_coef00 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int hipp_csc_coef00           : 12;      /* [11:0] */
        signed int reserved                  : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hipp_csc_coef00;

/* Define the union hdr_vau_hipp_csc_coef01 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int hipp_csc_coef01           : 12;      /* [11:0] */
        signed int reserved                  : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hipp_csc_coef01;

/* Define the union hdr_vau_hipp_csc_coef02 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int hipp_csc_coef02           : 12;      /* [11:0] */
        signed int reserved                  : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hipp_csc_coef02;

/* Define the union hdr_vau_hipp_csc_coef10 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int hipp_csc_coef10           : 12;      /* [11:0] */
        signed int reserved                  : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hipp_csc_coef10;

/* Define the union hdr_vau_hipp_csc_coef11 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int hipp_csc_coef11           : 12;      /* [11:0] */
        signed int reserved                  : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hipp_csc_coef11;

/* Define the union hdr_vau_hipp_csc_coef12 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int hipp_csc_coef12           : 12;      /* [11:0] */
        signed int reserved                  : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hipp_csc_coef12;

/* Define the union hdr_vau_hipp_csc_coef20 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int hipp_csc_coef20           : 12;      /* [11:0] */
        signed int reserved                  : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hipp_csc_coef20;

/* Define the union hdr_vau_hipp_csc_coef21 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int hipp_csc_coef21           : 12;      /* [11:0] */
        signed int reserved                  : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hipp_csc_coef21;

/* Define the union hdr_vau_hipp_csc_coef22 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int hipp_csc_coef22           : 12;      /* [11:0] */
        signed int reserved                  : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hipp_csc_coef22;

/* Define the union hdr_vau_hipp_csc_scale */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hipp_csc_scale          : 4;       /* [3:0] */
        unsigned int reserved                : 28;      /* [31:4] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hipp_csc_scale;

/* Define the union hdr_vau_hipp_csc_idc0 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int hipp_csc_idc0             : 11;      /* [10:0] */
        signed int reserved                  : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hipp_csc_idc0;

/* Define the union hdr_vau_hipp_csc_idc1 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int hipp_csc_idc1             : 11;      /* [10:0] */
        signed int reserved                  : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hipp_csc_idc1;

/* Define the union hdr_vau_hipp_csc_idc2 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int hipp_csc_idc2             : 11;      /* [10:0] */
        signed int reserved                  : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hipp_csc_idc2;

/* Define the union hdr_vau_hipp_csc_odc0 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int hipp_csc_odc0             : 11;      /* [10:0] */
        signed int reserved                  : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hipp_csc_odc0;

/* Define the union hdr_vau_hipp_csc_odc1 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int hipp_csc_odc1             : 11;      /* [10:0] */
        signed int reserved                  : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hipp_csc_odc1;

/* Define the union hdr_vau_hipp_csc_odc2 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int hipp_csc_odc2             : 11;      /* [10:0] */
        signed int reserved                  : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hipp_csc_odc2;

/* Define the union hdr_vau_hipp_csc_min_y */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hipp_csc_min_y          : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hipp_csc_min_y;

/* Define the union hdr_vau_hipp_csc_min_c */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hipp_csc_min_c          : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hipp_csc_min_c;

/* Define the union hdr_vau_hipp_csc_max_y */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hipp_csc_max_y          : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hipp_csc_max_y;

/* Define the union hdr_vau_hipp_csc_max_c */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hipp_csc_max_c          : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hipp_csc_max_c;

/* Define the union hdr_vau_hipp_csc2_coef00 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hipp_csc2_coef00        : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hipp_csc2_coef00;

/* Define the union hdr_vau_hipp_csc2_coef01 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hipp_csc2_coef01        : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hipp_csc2_coef01;

/* Define the union hdr_vau_hipp_csc2_coef02 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hipp_csc2_coef02        : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hipp_csc2_coef02;

/* Define the union hdr_vau_hipp_csc2_coef10 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hipp_csc2_coef10        : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hipp_csc2_coef10;

/* Define the union hdr_vau_hipp_csc2_coef11 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hipp_csc2_coef11        : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hipp_csc2_coef11;

/* Define the union hdr_vau_hipp_csc2_coef12 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hipp_csc2_coef12        : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hipp_csc2_coef12;

/* Define the union hdr_vau_hipp_csc2_coef20 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hipp_csc2_coef20        : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hipp_csc2_coef20;

/* Define the union hdr_vau_hipp_csc2_coef21 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hipp_csc2_coef21        : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hipp_csc2_coef21;

/* Define the union hdr_vau_hipp_csc2_coef22 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hipp_csc2_coef22        : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hipp_csc2_coef22;

/* Define the union hdr_vau_hipp_csc2_scale */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hipp_csc2_scale         : 4;       /* [3:0] */
        unsigned int reserved                : 28;      /* [31:4] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hipp_csc2_scale;

/* Define the union hdr_vau_hipp_csc2_idc0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hipp_csc2_idc0          : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hipp_csc2_idc0;

/* Define the union hdr_vau_hipp_csc2_idc1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hipp_csc2_idc1          : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hipp_csc2_idc1;

/* Define the union hdr_vau_hipp_csc2_idc2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hipp_csc2_idc2          : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hipp_csc2_idc2;

/* Define the union hdr_vau_hipp_csc2_odc0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hipp_csc2_odc0          : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hipp_csc2_odc0;

/* Define the union hdr_vau_hipp_csc2_odc1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hipp_csc2_odc1          : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hipp_csc2_odc1;

/* Define the union hdr_vau_hipp_csc2_odc2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hipp_csc2_odc2          : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hipp_csc2_odc2;

/* Define the union hdr_vau_hipp_csc2_min_y */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hipp_csc2_min_y         : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hipp_csc2_min_y;

/* Define the union hdr_vau_hipp_csc2_min_c */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hipp_csc2_min_c         : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hipp_csc2_min_c;

/* Define the union hdr_vau_hipp_csc2_max_y */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hipp_csc2_max_y         : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hipp_csc2_max_y;

/* Define the union hdr_vau_hipp_csc2_max_c */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hipp_csc2_max_c         : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hipp_csc2_max_c;

/* Define the union hdr_vau_cvfir2_vinfo */
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
} u_hdr_vau_cvfir2_vinfo;

/* Define the union hdr_vau_cvfir2_vsp */
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
} u_hdr_vau_cvfir2_vsp;

/* Define the union hdr_vau_cvfir2_voffset */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vchroma_offset          : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_cvfir2_voffset;

/* Define the union hdr_vau_cvfir2_vboffset */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vbchroma_offset         : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_cvfir2_vboffset;

/* Define the union hdr_vau_hfir2_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hfir_en                 : 1;       /* [0] */
        unsigned int hfir_mode               : 2;       /* [2:1] */
        unsigned int mid_en                  : 1;       /* [3] */
        unsigned int ck_gt_en                : 1;       /* [4] */
        unsigned int reserved                : 27;      /* [31:5] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hfir2_ctrl;

/* Define the union hdr_vdm_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vdm_en                  : 1;       /* [0] */
        unsigned int vdm_degmm_en            : 1;       /* [1] */
        unsigned int vdm_tmap_en             : 1;       /* [2] */
        unsigned int vdm_gmm_en              : 1;       /* [3] */
        unsigned int vdm_csc_bind            : 1;       /* [4] */
        unsigned int vdm_tmap_in_bdp         : 1;       /* [5] */
        unsigned int vdm_ck_gt_en            : 1;       /* [6] */
        unsigned int reserved                : 25;      /* [31:7] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_ctrl;

/* Define the union hdr_vdm_para_degmm_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vdm_para_degmm_addr     : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_para_degmm_addr;

/* Define the union hdr_vdm_para_gmm_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vdm_para_gmm_addr       : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_para_gmm_addr;

/* Define the union hdr_vdm_para_tmap_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vdm_para_tmap_addr      : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_para_tmap_addr;

/* Define the union hdr_vdm_csc_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vdm_csc_en              : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_csc_ctrl;

/* Define the union hdr_vdm_csc_coef00 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_coef00_csc            : 16;      /* [15:0] */
        signed int reserved                  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_csc_coef00;

/* Define the union hdr_vdm_csc_coef01 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_coef01_csc            : 16;      /* [15:0] */
        signed int reserved                  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_csc_coef01;

/* Define the union hdr_vdm_csc_coef02 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_coef02_csc            : 16;      /* [15:0] */
        signed int reserved                  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_csc_coef02;

/* Define the union hdr_vdm_csc_coef10 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_coef10_csc            : 16;      /* [15:0] */
        signed int reserved                  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_csc_coef10;

/* Define the union hdr_vdm_csc_coef11 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_coef11_csc            : 16;      /* [15:0] */
        signed int reserved                  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_csc_coef11;

/* Define the union hdr_vdm_csc_coef12 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_coef12_csc            : 16;      /* [15:0] */
        signed int reserved                  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_csc_coef12;

/* Define the union hdr_vdm_csc_coef20 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_coef20_csc            : 16;      /* [15:0] */
        signed int reserved                  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_csc_coef20;

/* Define the union hdr_vdm_csc_coef21 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_coef21_csc            : 16;      /* [15:0] */
        signed int reserved                  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_csc_coef21;

/* Define the union hdr_vdm_csc_coef22 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_coef22_csc            : 16;      /* [15:0] */
        signed int reserved                  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_csc_coef22;

/* Define the union hdr_vdm_csc_scale */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vdm_csc_scale           : 4;       /* [3:0] */
        unsigned int reserved                : 28;      /* [31:4] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_csc_scale;

/* Define the union hdr_vdm_csc_clip_min */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vdm_csc_clip_min        : 21;      /* [20:0] */
        unsigned int reserved                : 11;      /* [31:21] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_csc_clip_min;

/* Define the union hdr_vdm_csc_clip_max */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vdm_csc_clip_max        : 21;      /* [20:0] */
        unsigned int reserved                : 11;      /* [31:21] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_csc_clip_max;

/* Define the union hdr_vdm_csc_in_dc0 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_csc_0_in_dc           : 21;      /* [20:0] */
        signed int reserved                  : 11;      /* [31:21] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_csc_in_dc0;

/* Define the union hdr_vdm_csc_in_dc1 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_csc_1_in_dc           : 21;      /* [20:0] */
        signed int reserved                  : 11;      /* [31:21] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_csc_in_dc1;

/* Define the union hdr_vdm_csc_in_dc2 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_csc_2_in_dc           : 21;      /* [20:0] */
        signed int reserved                  : 11;      /* [31:21] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_csc_in_dc2;

/* Define the union hdr_vdm_csc_out_dc0 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_csc_0_out_dc          : 21;      /* [20:0] */
        signed int reserved                  : 11;      /* [31:21] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_csc_out_dc0;

/* Define the union hdr_vdm_csc_out_dc1 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_csc_1_out_dc          : 21;      /* [20:0] */
        signed int reserved                  : 11;      /* [31:21] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_csc_out_dc1;

/* Define the union hdr_vdm_csc_out_dc2 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_csc_2_out_dc          : 21;      /* [20:0] */
        signed int reserved                  : 11;      /* [31:21] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_csc_out_dc2;

/* Define the union hdr_vdm_yuv2rgb_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vdm_y2r_en              : 1;       /* [0] */
        unsigned int vdm_y2r_cl_en           : 1;       /* [1] */
        unsigned int reserved0               : 2;       /* [3:2] */
        unsigned int vdm_v0_csc_en           : 1;       /* [4] */
        unsigned int vdm_v1_csc_en           : 1;       /* [5] */
        unsigned int vdm_v0_gamma_en         : 1;       /* [6] */
        unsigned int vdm_v1_gamma_en         : 1;       /* [7] */
        unsigned int vdm_demo_en             : 1;       /* [8] */
        unsigned int vdm_demo_mode           : 1;       /* [9] */
        unsigned int reserved1               : 2;       /* [11:10] */
        unsigned int vdm_demo_pos            : 12;      /* [23:12] */
        unsigned int reserved2               : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_yuv2rgb_ctrl;

/* Define the union hdr_vdm_yuv2rgb_coef00 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_00_yuv2rgb            : 16;      /* [15:0] */
        signed int reserved                  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_yuv2rgb_coef00;

/* Define the union hdr_vdm_yuv2rgb_coef01 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_01_yuv2rgb            : 16;      /* [15:0] */
        signed int reserved                  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_yuv2rgb_coef01;

/* Define the union hdr_vdm_yuv2rgb_coef02 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_02_yuv2rgb            : 16;      /* [15:0] */
        signed int reserved                  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_yuv2rgb_coef02;

/* Define the union hdr_vdm_yuv2rgb_coef03 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_03_yuv2rgb            : 16;      /* [15:0] */
        signed int reserved                  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_yuv2rgb_coef03;

/* Define the union hdr_vdm_yuv2rgb_coef04 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_04_yuv2rgb            : 16;      /* [15:0] */
        signed int reserved                  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_yuv2rgb_coef04;

/* Define the union hdr_vdm_yuv2rgb_coef05 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_05_yuv2rgb            : 16;      /* [15:0] */
        signed int reserved                  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_yuv2rgb_coef05;

/* Define the union hdr_vdm_yuv2rgb_coef06 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_06_yuv2rgb            : 16;      /* [15:0] */
        signed int reserved                  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_yuv2rgb_coef06;

/* Define the union hdr_vdm_yuv2rgb_coef07 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_07_yuv2rgb            : 16;      /* [15:0] */
        signed int reserved                  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_yuv2rgb_coef07;

/* Define the union hdr_vdm_yuv2rgb_coef08 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_08_yuv2rgb            : 16;      /* [15:0] */
        signed int reserved                  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_yuv2rgb_coef08;

/* Define the union hdr_vdm_yuv2rgb_coef10 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_10_yuv2rgb            : 16;      /* [15:0] */
        signed int reserved                  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_yuv2rgb_coef10;

/* Define the union hdr_vdm_yuv2rgb_coef11 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_11_yuv2rgb            : 16;      /* [15:0] */
        signed int reserved                  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_yuv2rgb_coef11;

/* Define the union hdr_vdm_yuv2rgb_coef12 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_12_yuv2rgb            : 16;      /* [15:0] */
        signed int reserved                  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_yuv2rgb_coef12;

/* Define the union hdr_vdm_yuv2rgb_coef13 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_13_yuv2rgb            : 16;      /* [15:0] */
        signed int reserved                  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_yuv2rgb_coef13;

/* Define the union hdr_vdm_yuv2rgb_coef14 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_14_yuv2rgb            : 16;      /* [15:0] */
        signed int reserved                  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_yuv2rgb_coef14;

/* Define the union hdr_vdm_yuv2rgb_coef15 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_15_yuv2rgb            : 16;      /* [15:0] */
        signed int reserved                  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_yuv2rgb_coef15;

/* Define the union hdr_vdm_yuv2rgb_coef16 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_16_yuv2rgb            : 16;      /* [15:0] */
        signed int reserved                  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_yuv2rgb_coef16;

/* Define the union hdr_vdm_yuv2rgb_coef17 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_17_yuv2rgb            : 16;      /* [15:0] */
        signed int reserved                  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_yuv2rgb_coef17;

/* Define the union hdr_vdm_yuv2rgb_coef18 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_18_yuv2rgb            : 16;      /* [15:0] */
        signed int reserved                  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_yuv2rgb_coef18;

/* Define the union hdr_vdm_yuv2rgb_scale2p */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vdm_yuv2rgbscale2p      : 4;       /* [3:0] */
        unsigned int reserved                : 28;      /* [31:4] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_yuv2rgb_scale2p;

/* Define the union hdr_vdm_yuv2rgb_in_dc00 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_yuv2rgb_00_in_dc      : 11;      /* [10:0] */
        signed int reserved                  : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_yuv2rgb_in_dc00;

/* Define the union hdr_vdm_yuv2rgb_in_dc01 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_yuv2rgb_01_in_dc      : 11;      /* [10:0] */
        signed int reserved                  : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_yuv2rgb_in_dc01;

/* Define the union hdr_vdm_yuv2rgb_in_dc02 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_yuv2rgb_02_in_dc      : 11;      /* [10:0] */
        signed int reserved                  : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_yuv2rgb_in_dc02;

/* Define the union hdr_vdm_yuv2rgb_out_dc00 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_yuv2rgb_00_out_dc     : 13;      /* [12:0] */
        signed int reserved                  : 19;      /* [31:13] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_yuv2rgb_out_dc00;

/* Define the union hdr_vdm_yuv2rgb_out_dc01 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_yuv2rgb_01_out_dc     : 13;      /* [12:0] */
        signed int reserved                  : 19;      /* [31:13] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_yuv2rgb_out_dc01;

/* Define the union hdr_vdm_yuv2rgb_out_dc02 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_yuv2rgb_02_out_dc     : 13;      /* [12:0] */
        signed int reserved                  : 19;      /* [31:13] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_yuv2rgb_out_dc02;

/* Define the union hdr_vdm_yuv2rgb_in_dc10 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_yuv2rgb_10_in_dc      : 11;      /* [10:0] */
        signed int reserved                  : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_yuv2rgb_in_dc10;

/* Define the union hdr_vdm_yuv2rgb_in_dc11 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_yuv2rgb_11_in_dc      : 11;      /* [10:0] */
        signed int reserved                  : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_yuv2rgb_in_dc11;

/* Define the union hdr_vdm_yuv2rgb_in_dc12 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_yuv2rgb_12_in_dc      : 11;      /* [10:0] */
        signed int reserved                  : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_yuv2rgb_in_dc12;

/* Define the union hdr_vdm_yuv2rgb_out_dc10 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_yuv2rgb_10_out_dc     : 13;      /* [12:0] */
        signed int reserved                  : 19;      /* [31:13] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_yuv2rgb_out_dc10;

/* Define the union hdr_vdm_yuv2rgb_out_dc11 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_yuv2rgb_11_out_dc     : 13;      /* [12:0] */
        signed int reserved                  : 19;      /* [31:13] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_yuv2rgb_out_dc11;

/* Define the union hdr_vdm_yuv2rgb_out_dc12 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_yuv2rgb_12_out_dc     : 13;      /* [12:0] */
        signed int reserved                  : 19;      /* [31:13] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_yuv2rgb_out_dc12;

/* Define the union hdr_vdm_yuv2rgb_min */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vdm_yuv2rgb_clip_min    : 12;      /* [11:0] */
        unsigned int reserved                : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_yuv2rgb_min;

/* Define the union hdr_vdm_yuv2rgb_max */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vdm_yuv2rgb_clip_max    : 12;      /* [11:0] */
        unsigned int reserved                : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_yuv2rgb_max;

/* Define the union hdr_disp_0_dither_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int t_data_width_dither0    : 3;       /* [2:0] */
        unsigned int dither0_round           : 1;       /* [3] */
        unsigned int dither0_mode            : 1;       /* [4] */
        unsigned int dither0_en              : 1;       /* [5] */
        unsigned int dither0_round_unlim     : 1;       /* [6] */
        unsigned int i_data_width_dither0    : 3;       /* [9:7] */
        unsigned int o_data_width_dither0    : 3;       /* [12:10] */
        unsigned int dither0_block_mode      : 1;       /* [13] */
        unsigned int dither0_ck_gt_en        : 1;       /* [14] */
        unsigned int dither0_diffuse_en      : 1;       /* [15] */
        unsigned int dither0_reset           : 12;      /* [27:16] */
        unsigned int dither0_frame_cycle     : 2;       /* [29:28] */
        unsigned int dither0_lut_frameroll   : 1;       /* [30] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_0_dither_ctrl;

/* Define the union hdr_disp_0_dither_ink_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither0_ink_en          : 1;       /* [0] */
        unsigned int dither0_ink_sel         : 1;       /* [1] */
        unsigned int dither0_data_fmt        : 1;       /* [2] */
        unsigned int dither0_cross_enable    : 1;       /* [3] */
        unsigned int dither0_color_mode      : 2;       /* [5:4] */
        unsigned int reserved                : 26;      /* [31:6] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_0_dither_ink_ctrl;

/* Define the union hdr_disp_0_dither_ink_pos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither0_x_pos           : 16;      /* [15:0] */
        unsigned int dither0_y_pos           : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_0_dither_ink_pos;

/* Define the union hdr_local_lighting_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int local_lighting_en       : 1;       /* [0] */
        unsigned int local_lighting_ck_gt_en : 1;       /* [1] */
        unsigned int bbox_cfg_en             : 1;       /* [2] */
        unsigned int is_x_major              : 1;       /* [3] */
        unsigned int is_bi_dir               : 1;       /* [4] */
        unsigned int lighting_mode           : 3;       /* [7:5] */
        unsigned int bbox0_bbox1_low_power_en : 1;       /* [8] */
        unsigned int reserved                : 23;      /* [31:9] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_local_lighting_cfg;

/* Define the union hdr_local_lighting_cx */
typedef union {
    /* Define the struct bits */
    struct {
        float centerx; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_local_lighting_cx;

/* Define the union hdr_local_lighting_cy */
typedef union {
    /* Define the struct bits */
    struct {
        float centery; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_local_lighting_cy;

/* Define the union hdr_local_lighting_radius2 */
typedef union {
    /* Define the struct bits */
    struct {
        float radius2; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_local_lighting_radius2;

/* Define the union hdr_local_lighting_amplitude */
typedef union {
    /* Define the struct bits */
    struct {
        float amplitude; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_local_lighting_amplitude;

/* Define the union hdr_local_lighting_ampli_scale */
typedef union {
    /* Define the struct bits */
    struct {
        float amplitude_scale; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_local_lighting_ampli_scale;

/* Define the union hdr_local_lighting_start_angle_rad */
typedef union {
    /* Define the struct bits */
    struct {
        float start_angle_rad; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_local_lighting_start_angle_rad;

/* Define the union hdr_local_lighting_start_end_angle_rad */
typedef union {
    /* Define the struct bits */
    struct {
        float start_end_angle_rad; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_local_lighting_start_end_angle_rad;

/* Define the union hdr_local_lighting_end_start_angle_rad */
typedef union {
    /* Define the struct bits */
    struct {
        float end_start_angle_rad; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_local_lighting_end_start_angle_rad;

/* Define the union hdr_local_lighting_end_angle_rad */
typedef union {
    /* Define the struct bits */
    struct {
        float end_angle_rad; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_local_lighting_end_angle_rad;

/* Define the union hdr_local_lighting_triwin2_coeff0 */
typedef union {
    /* Define the struct bits */
    struct {
        float triwin2_coeff0; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_local_lighting_triwin2_coeff0;

/* Define the union hdr_local_lighting_triwin2_coeff1 */
typedef union {
    /* Define the struct bits */
    struct {
        float triwin2_coeff1; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_local_lighting_triwin2_coeff1;

/* Define the union hdr_local_lighting_amb_amp */
typedef union {
    /* Define the struct bits */
    struct {
        float amb_amplitude; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_local_lighting_amb_amp;

/* Define the union hdr_local_lighting_bbox0_p0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int bbox0_min_x             : 16;      /* [15:0] */
        unsigned int bbox0_min_y             : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_local_lighting_bbox0_p0;

/* Define the union hdr_local_lighting_bbox0_p1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int bbox0_max_x             : 16;      /* [15:0] */
        unsigned int bbox0_max_y             : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_local_lighting_bbox0_p1;

/* Define the union hdr_local_lighting_bbox1_p0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int bbox1_min_x             : 16;      /* [15:0] */
        unsigned int bbox1_min_y             : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_local_lighting_bbox1_p0;

/* Define the union hdr_local_lighting_bbox1_p1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int bbox1_max_x             : 16;      /* [15:0] */
        unsigned int bbox1_max_y             : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_local_lighting_bbox1_p1;

/* Define the union hdr_local_lighting_img_reso */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int image_width             : 16;      /* [15:0] */
        unsigned int image_height            : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_local_lighting_img_reso;

/* Define the union hdr_local_lighting_start_pos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int start_pos_x             : 16;      /* [15:0] */
        unsigned int start_pos_y             : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_local_lighting_start_pos;

/* Define the union hdr_local_lighting_adjust_value */
typedef union {
    /* Define the struct bits */
    struct {
        float adjust_value; /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_local_lighting_adjust_value;

/* Define the union hdr_rcbn_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int rcbn_en                 : 1;       /* [0] */
        unsigned int bypass_mode             : 1;       /* [1] */
        unsigned int pow_en                  : 1;       /* [2] */
        unsigned int reserved                : 29;      /* [31:3] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_rcbn_ctrl;

/* Define the union hdr_rcbn_gmap_size_mode */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gmap_size_mode          : 2;       /* [1:0] */
        unsigned int reserved                : 30;      /* [31:2] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_rcbn_gmap_size_mode;

/* Define the union hdr_rcbn_gmap_chn_num */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gmap_chn_num            : 2;       /* [1:0] */
        unsigned int reserved                : 30;      /* [31:2] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_rcbn_gmap_chn_num;

/* Define the union hdr_rcbn_gcoeff_rmin1 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int gcoeff_r_min_1          : 24;      /* [23:0] */
        signed int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_rcbn_gcoeff_rmin1;

/* Define the union hdr_rcbn_gcoeff_rmin2 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int gcoeff_r_min_2          : 24;      /* [23:0] */
        signed int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_rcbn_gcoeff_rmin2;

/* Define the union hdr_rcbn_gcoeff_rmax */
typedef union {
    /* Define the struct bits */
    struct {
        signed int gcoeff_r_max            : 24;      /* [23:0] */
        signed int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_rcbn_gcoeff_rmax;

/* Define the union hdr_rcbn_gcoeff_gmin1 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int gcoeff_g_min_1          : 24;      /* [23:0] */
        signed int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_rcbn_gcoeff_gmin1;

/* Define the union hdr_rcbn_gcoeff_gmin2 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int gcoeff_g_min_2          : 24;      /* [23:0] */
        signed int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_rcbn_gcoeff_gmin2;

/* Define the union hdr_rcbn_gcoeff_gmax */
typedef union {
    /* Define the struct bits */
    struct {
        signed int gcoeff_g_max            : 24;      /* [23:0] */
        signed int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_rcbn_gcoeff_gmax;

/* Define the union hdr_rcbn_gcoeff_bmin1 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int gcoeff_b_min_1          : 24;      /* [23:0] */
        signed int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_rcbn_gcoeff_bmin1;

/* Define the union hdr_rcbn_gcoeff_bmin2 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int gcoeff_b_min_2          : 24;      /* [23:0] */
        signed int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_rcbn_gcoeff_bmin2;

/* Define the union hdr_rcbn_gcoeff_bmax */
typedef union {
    /* Define the struct bits */
    struct {
        signed int gcoeff_b_max            : 24;      /* [23:0] */
        signed int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_rcbn_gcoeff_bmax;

/* Define the union hdr_rcbn_kcoeff_r_alte */
typedef union {
    /* Define the struct bits */
    struct {
        signed int kcoeff_r_alte           : 24;      /* [23:0] */
        signed int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_rcbn_kcoeff_r_alte;

/* Define the union hdr_rcbn_kcoeff_r_base */
typedef union {
    /* Define the struct bits */
    struct {
        signed int kcoeff_r_base           : 24;      /* [23:0] */
        signed int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_rcbn_kcoeff_r_base;

/* Define the union hdr_rcbn_kcoeff_g_alte */
typedef union {
    /* Define the struct bits */
    struct {
        signed int kcoeff_g_alte           : 24;      /* [23:0] */
        signed int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_rcbn_kcoeff_g_alte;

/* Define the union hdr_rcbn_kcoeff_g_base */
typedef union {
    /* Define the struct bits */
    struct {
        signed int kcoeff_g_base           : 24;      /* [23:0] */
        signed int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_rcbn_kcoeff_g_base;

/* Define the union hdr_rcbn_kcoeff_b_alte */
typedef union {
    /* Define the struct bits */
    struct {
        signed int kcoeff_b_alte           : 24;      /* [23:0] */
        signed int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_rcbn_kcoeff_b_alte;

/* Define the union hdr_rcbn_kcoeff_b_base */
typedef union {
    /* Define the struct bits */
    struct {
        signed int kcoeff_b_base           : 24;      /* [23:0] */
        signed int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_rcbn_kcoeff_b_base;

/* Define the union hdr_rcbn_weight */
typedef union {
    /* Define the struct bits */
    struct {
        signed int weight                  : 24;      /* [23:0] */
        signed int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_rcbn_weight;

/* Define the union hdr_rcbn_clip_min */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int clip_min                : 21;      /* [20:0] */
        unsigned int reserved                : 11;      /* [31:21] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_rcbn_clip_min;

/* Define the union hdr_rcbn_clip_max */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int clip_max                : 21;      /* [20:0] */
        unsigned int reserved                : 11;      /* [31:21] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_rcbn_clip_max;

/* Define the union hdr_hipp_ctadj_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ct_adj_en               : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_hipp_ctadj_ctrl;

/* Define the union hdr_hipp_ctadj_gain */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ct_adj_gain_r           : 9;       /* [8:0] */
        unsigned int ct_adj_gain_g           : 9;       /* [17:9] */
        unsigned int ct_adj_gain_b           : 9;       /* [26:18] */
        unsigned int reserved                : 5;       /* [31:27] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_hipp_ctadj_gain;

/* Define the union hdr_hipp_ctadj_offset */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ct_adj_offset_r         : 11;      /* [10:0] */
        unsigned int ct_adj_offset_g         : 11;      /* [21:11] */
        unsigned int reserved                : 10;      /* [31:22] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_hipp_ctadj_offset;

/* Define the union hdr_hipp_ctadj_offset_b */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ct_adj_offset_b         : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_hipp_ctadj_offset_b;

/* Define the union hdr_hipp_ctadj_min */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ct_adj_min_r            : 10;      /* [9:0] */
        unsigned int ct_adj_min_g            : 10;      /* [19:10] */
        unsigned int ct_adj_min_b            : 10;      /* [29:20] */
        unsigned int reserved                : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_hipp_ctadj_min;

/* Define the union hdr_hipp_ctadj_max */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ct_adj_max_r            : 10;      /* [9:0] */
        unsigned int ct_adj_max_g            : 10;      /* [19:10] */
        unsigned int ct_adj_max_b            : 10;      /* [29:20] */
        unsigned int reserved                : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_hipp_ctadj_max;

/* Define the union hdr_hipp_ctadj_gain_w */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ct_adj_gain_w           : 9;       /* [8:0] */
        unsigned int reserved                : 23;      /* [31:9] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_hipp_ctadj_gain_w;

/* Define the union hdr_hipp_ctadj_offset_w */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ct_adj_offset_w         : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_hipp_ctadj_offset_w;

/* Define the union hdr_hipp_ctadj_min_w */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ct_adj_min_w            : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_hipp_ctadj_min_w;

/* Define the union hdr_hipp_ctadj_max_w */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ct_adj_max_w            : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_hipp_ctadj_max_w;

/* Define the union hdr_disp_1_dither_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int t_data_width_dither     : 3;       /* [2:0] */
        unsigned int dither_round            : 1;       /* [3] */
        unsigned int dither_mode             : 1;       /* [4] */
        unsigned int dither_en               : 1;       /* [5] */
        unsigned int dither_round_unlim      : 1;       /* [6] */
        unsigned int i_data_width_dither     : 3;       /* [9:7] */
        unsigned int o_data_width_dither     : 3;       /* [12:10] */
        unsigned int dither_block_mode       : 1;       /* [13] */
        unsigned int dither_ck_gt_en         : 1;       /* [14] */
        unsigned int dither_diffuse_en       : 1;       /* [15] */
        unsigned int dither_reset            : 12;      /* [27:16] */
        unsigned int dither_frame_cycle      : 2;       /* [29:28] */
        unsigned int dither_lut_frameroll    : 1;       /* [30] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_1_dither_ctrl;

/* Define the union hdr_disp_1_dither_ink_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ink_en                  : 1;       /* [0] */
        unsigned int ink_sel                 : 1;       /* [1] */
        unsigned int data_fmt                : 1;       /* [2] */
        unsigned int cross_enable            : 1;       /* [3] */
        unsigned int color_mode              : 3;       /* [6:4] */
        unsigned int reserved                : 25;      /* [31:7] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_1_dither_ink_ctrl;

/* Define the union hdr_disp_1_dither_ink_pos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int x_pos                   : 16;      /* [15:0] */
        unsigned int y_pos                   : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_1_dither_ink_pos;

/* Define the union hdr_cbm_bkg1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cbm_bkgcr1              : 10;      /* [9:0] */
        unsigned int cbm_bkgcb1              : 10;      /* [19:10] */
        unsigned int cbm_bkgy1               : 10;      /* [29:20] */
        unsigned int reserved                : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_cbm_bkg1;

/* Define the union hdr_cbm_bkalpha */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mixer_alpha             : 8;       /* [7:0] */
        unsigned int reserved                : 24;      /* [31:8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_cbm_bkalpha;

/* Define the union hdr_cbm_mix1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mixer_prio0             : 4;       /* [3:0] */
        unsigned int mixer_prio1             : 4;       /* [7:4] */
        unsigned int mixer_prio2             : 4;       /* [11:8] */
        unsigned int mixer_prio3             : 4;       /* [15:12] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_cbm_mix1;

/* Define the union hdr_cbm_mix_bypass */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int layer0_premulti         : 1;       /* [0] */
        unsigned int layer0_bypass_en        : 1;       /* [1] */
        unsigned int reserved0               : 13;      /* [14:2] */
        unsigned int mix1_bypass_mode        : 1;       /* [15] */
        unsigned int reserved1               : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_cbm_mix_bypass;

/* Define the union hdr_cbm_bkg2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cbm_bkgcr2              : 10;      /* [9:0] */
        unsigned int cbm_bkgcb2              : 10;      /* [19:10] */
        unsigned int cbm_bkgy2               : 10;      /* [29:20] */
        unsigned int reserved                : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_cbm_bkg2;

/* Define the union hdr_cbm_mix2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mixer_prio0             : 4;       /* [3:0] */
        unsigned int mixer_prio1             : 4;       /* [7:4] */
        unsigned int reserved                : 24;      /* [31:8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_cbm_mix2;

/* Define the union hdr_hc_bmp_thd */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hc_bmp_thd              : 8;       /* [7:0] */
        unsigned int reserved                : 24;      /* [31:8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_hc_bmp_thd;

/* Define the union hdr_vhdaccstatctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int acc_stat_en             : 1;       /* [0] */
        unsigned int acc_stat_ck_gt_en       : 1;       /* [1] */
        unsigned int reserved                : 30;      /* [31:2] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vhdaccstatctrl;

/* Define the union hdr_vhdaccstathpos */
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
} u_hdr_vhdaccstathpos;

/* Define the union hdr_vhdaccstatvpos */
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
} u_hdr_vhdaccstatvpos;

/* Define the union hdr_vhdaccshiftd */
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
} u_hdr_vhdaccshiftd;

/* Define the union hdr_vhdaccdnsam */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int acc_stat_hdnsample      : 5;       /* [4:0] */
        unsigned int acc_stat_vdnsample      : 5;       /* [9:5] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vhdaccdnsam;

/* Define the union hdr_vhdaccsatcfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int acc_stat_lowsat_bin     : 5;       /* [4:0] */
        unsigned int reserved                : 27;      /* [31:5] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vhdaccsatcfg;

/* Define the union hdr_vdm_csc1_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vdm_csc_en              : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_csc1_ctrl;

/* Define the union hdr_vdm_csc1_coef00 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_coef00_csc            : 16;      /* [15:0] */
        signed int reserved                  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_csc1_coef00;

/* Define the union hdr_vdm_csc1_coef01 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_coef01_csc            : 16;      /* [15:0] */
        signed int reserved                  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_csc1_coef01;

/* Define the union hdr_vdm_csc1_coef02 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_coef02_csc            : 16;      /* [15:0] */
        signed int reserved                  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_csc1_coef02;

/* Define the union hdr_vdm_csc1_coef10 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_coef10_csc            : 16;      /* [15:0] */
        signed int reserved                  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_csc1_coef10;

/* Define the union hdr_vdm_csc1_coef11 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_coef11_csc            : 16;      /* [15:0] */
        signed int reserved                  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_csc1_coef11;

/* Define the union hdr_vdm_csc1_coef12 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_coef12_csc            : 16;      /* [15:0] */
        signed int reserved                  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_csc1_coef12;

/* Define the union hdr_vdm_csc1_coef20 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_coef20_csc            : 16;      /* [15:0] */
        signed int reserved                  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_csc1_coef20;

/* Define the union hdr_vdm_csc1_coef21 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_coef21_csc            : 16;      /* [15:0] */
        signed int reserved                  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_csc1_coef21;

/* Define the union hdr_vdm_csc1_coef22 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_coef22_csc            : 16;      /* [15:0] */
        signed int reserved                  : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_csc1_coef22;

/* Define the union hdr_vdm_csc1_scale */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vdm_csc_scale           : 4;       /* [3:0] */
        unsigned int reserved                : 28;      /* [31:4] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_csc1_scale;

/* Define the union hdr_vdm_csc1_clip_min */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vdm_csc_clip_min        : 21;      /* [20:0] */
        unsigned int reserved                : 11;      /* [31:21] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_csc1_clip_min;

/* Define the union hdr_vdm_csc1_clip_max */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vdm_csc_clip_max        : 21;      /* [20:0] */
        unsigned int reserved                : 11;      /* [31:21] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_csc1_clip_max;

/* Define the union hdr_vdm_csc1_in_dc0 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_csc_0_in_dc           : 21;      /* [20:0] */
        signed int reserved                  : 11;      /* [31:21] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_csc1_in_dc0;

/* Define the union hdr_vdm_csc1_in_dc1 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_csc_1_in_dc           : 21;      /* [20:0] */
        signed int reserved                  : 11;      /* [31:21] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_csc1_in_dc1;

/* Define the union hdr_vdm_csc1_in_dc2 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_csc_2_in_dc           : 21;      /* [20:0] */
        signed int reserved                  : 11;      /* [31:21] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_csc1_in_dc2;

/* Define the union hdr_vdm_csc1_out_dc0 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_csc_0_out_dc          : 21;      /* [20:0] */
        signed int reserved                  : 11;      /* [31:21] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_csc1_out_dc0;

/* Define the union hdr_vdm_csc1_out_dc1 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_csc_1_out_dc          : 21;      /* [20:0] */
        signed int reserved                  : 11;      /* [31:21] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_csc1_out_dc1;

/* Define the union hdr_vdm_csc1_out_dc2 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_csc_2_out_dc          : 21;      /* [20:0] */
        signed int reserved                  : 11;      /* [31:21] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_csc1_out_dc2;

/* Define the union hdr_vdm_tmap_luma_coef0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vdm_luma_coef0_tmap     : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_tmap_luma_coef0;

/* Define the union hdr_vdm_tmap_luma_coef1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vdm_luma_coef1_tmap     : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_tmap_luma_coef1;

/* Define the union hdr_vdm_tmap_luma_coef2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vdm_luma_coef2_tmap     : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_tmap_luma_coef2;

/* Define the union hdr_vdm_tmap_luma_scale */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vdm_tmap_luma_scale     : 4;       /* [3:0] */
        unsigned int reserved                : 28;      /* [31:4] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_tmap_luma_scale;

/* Define the union hdr_vdm_tmap_coef_scale */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vdm_tmap_coef_scale     : 4;       /* [3:0] */
        unsigned int reserved                : 28;      /* [31:4] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_tmap_coef_scale;

/* Define the union hdr_vdm_tmap_out_clip_min */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vdm_tmap_out_clip_min   : 21;      /* [20:0] */
        unsigned int reserved                : 11;      /* [31:21] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_tmap_out_clip_min;

/* Define the union hdr_vdm_tmap_out_clip_max */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vdm_tmap_out_clip_max   : 21;      /* [20:0] */
        unsigned int reserved                : 11;      /* [31:21] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_tmap_out_clip_max;

/* Define the union hdr_vdm_tmap_out_dc0 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_tmap_out_dc0          : 21;      /* [20:0] */
        signed int reserved                  : 11;      /* [31:21] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_tmap_out_dc0;

/* Define the union hdr_vdm_tmap_out_dc1 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_tmap_out_dc1          : 21;      /* [20:0] */
        signed int reserved                  : 11;      /* [31:21] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_tmap_out_dc1;

/* Define the union hdr_vdm_tmap_out_dc2 */
typedef union {
    /* Define the struct bits */
    struct {
        signed int vdm_tmap_out_dc2          : 21;      /* [20:0] */
        signed int reserved                  : 11;      /* [31:21] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_tmap_out_dc2;

/* Define the union hdr_vdm_tonemap_alpha */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int v_tmap_mix_alpha        : 8;       /* [7:0] */
        unsigned int v_tmap_scale_mix_alpha  : 1;       /* [8] */
        unsigned int reserved                : 23;      /* [31:9] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_tonemap_alpha;

/* Define the union hdr_vau_cvfir_vcoef0 */
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
} u_hdr_vau_cvfir_vcoef0;

/* Define the union hdr_vau_cvfir_vcoef1 */
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
} u_hdr_vau_cvfir_vcoef1;

/* Define the union hdr_vau_cvfir_vcoef2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vccoef13                : 10;      /* [9:0] */
        unsigned int vccoef12                : 10;      /* [19:10] */
        unsigned int reserved                : 12;      /* [31:20] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_cvfir_vcoef2;

/* Define the union hdr_vau_hfircoef01 */
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
} u_hdr_vau_hfircoef01;

/* Define the union hdr_vau_hfircoef23 */
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
} u_hdr_vau_hfircoef23;

/* Define the union hdr_vau_hfircoef45 */
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
} u_hdr_vau_hfircoef45;

/* Define the union hdr_vau_hfircoef67 */
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
} u_hdr_vau_hfircoef67;

/* Define the union hdr_vau_hipp_csc_ink_ctrl */
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
} u_hdr_vau_hipp_csc_ink_ctrl;

/* Define the union hdr_vau_hipp_csc_ink_pos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int x_pos                   : 16;      /* [15:0] */
        unsigned int y_pos                   : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hipp_csc_ink_pos;

/* Define the union hdr_vau_hipp_csc_ink_data */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ink_data                : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hipp_csc_ink_data;

/* Define the union hdr_vau_hipp_csc_ink_data2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ink_data2               : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_hipp_csc_ink_data2;

/* Define the union hdr_vau_cvfir2_vcoef0 */
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
} u_hdr_vau_cvfir2_vcoef0;

/* Define the union hdr_vau_cvfir2_vcoef1 */
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
} u_hdr_vau_cvfir2_vcoef1;

/* Define the union hdr_vau_cvfir2_vcoef2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vccoef13                : 10;      /* [9:0] */
        unsigned int vccoef12                : 10;      /* [19:10] */
        unsigned int reserved                : 12;      /* [31:20] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vau_cvfir2_vcoef2;

/* Define the union hdr_vau_hfir2coef01 */
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
} u_hdr_vau_hfir2coef01;

/* Define the union hdr_vau_hfir2coef23 */
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
} u_hdr_vau_hfir2coef23;

/* Define the union hdr_vau_hfir2coef45 */
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
} u_hdr_vau_hfir2coef45;

/* Define the union hdr_vau_hfir2coef67 */
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
} u_hdr_vau_hfir2coef67;

/* Define the union hdr_vdm_degmm_ren */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vdm_degamma_rd_en       : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_degmm_ren;

/* Define the union hdr_vdm_degmm_data */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vdm_degamma_para_data   : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_degmm_data;

/* Define the union hdr_vdm_degamma_step */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int v_degmm_x1_step         : 4;       /* [3:0] */
        unsigned int reserved0               : 4;       /* [7:4] */
        unsigned int v_degmm_x2_step         : 4;       /* [11:8] */
        unsigned int reserved1               : 4;       /* [15:12] */
        unsigned int v_degmm_x3_step         : 4;       /* [19:16] */
        unsigned int reserved2               : 4;       /* [23:20] */
        unsigned int v_degmm_x4_step         : 4;       /* [27:24] */
        unsigned int reserved3               : 4;       /* [31:28] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_degamma_step;

/* Define the union hdr_vdm_degamma_step2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int v_degmm_x5_step         : 4;       /* [3:0] */
        unsigned int reserved0               : 4;       /* [7:4] */
        unsigned int v_degmm_x6_step         : 4;       /* [11:8] */
        unsigned int reserved1               : 4;       /* [15:12] */
        unsigned int v_degmm_x7_step         : 4;       /* [19:16] */
        unsigned int reserved2               : 4;       /* [23:20] */
        unsigned int v_degmm_x8_step         : 4;       /* [27:24] */
        unsigned int reserved3               : 4;       /* [31:28] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_degamma_step2;

/* Define the union hdr_vdm_degamma_pos0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int v_degmm_x1_pos          : 12;      /* [11:0] */
        unsigned int reserved0               : 4;       /* [15:12] */
        unsigned int v_degmm_x2_pos          : 12;      /* [27:16] */
        unsigned int reserved1               : 4;       /* [31:28] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_degamma_pos0;

/* Define the union hdr_vdm_degamma_pos1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int v_degmm_x3_pos          : 12;      /* [11:0] */
        unsigned int reserved0               : 4;       /* [15:12] */
        unsigned int v_degmm_x4_pos          : 12;      /* [27:16] */
        unsigned int reserved1               : 4;       /* [31:28] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_degamma_pos1;

/* Define the union hdr_vdm_degamma_pos2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int v_degmm_x5_pos          : 12;      /* [11:0] */
        unsigned int reserved0               : 4;       /* [15:12] */
        unsigned int v_degmm_x6_pos          : 12;      /* [27:16] */
        unsigned int reserved1               : 4;       /* [31:28] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_degamma_pos2;

/* Define the union hdr_vdm_degamma_pos3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int v_degmm_x7_pos          : 12;      /* [11:0] */
        unsigned int reserved0               : 4;       /* [15:12] */
        unsigned int v_degmm_x8_pos          : 12;      /* [27:16] */
        unsigned int reserved1               : 4;       /* [31:28] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_degamma_pos3;

/* Define the union hdr_vdm_degamma_num */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int v_degmm_x1_num          : 6;       /* [5:0] */
        unsigned int reserved0               : 2;       /* [7:6] */
        unsigned int v_degmm_x2_num          : 6;       /* [13:8] */
        unsigned int reserved1               : 2;       /* [15:14] */
        unsigned int v_degmm_x3_num          : 6;       /* [21:16] */
        unsigned int reserved2               : 2;       /* [23:22] */
        unsigned int v_degmm_x4_num          : 6;       /* [29:24] */
        unsigned int reserved3               : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_degamma_num;

/* Define the union hdr_vdm_degamma_num2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int v_degmm_x5_num          : 6;       /* [5:0] */
        unsigned int reserved0               : 2;       /* [7:6] */
        unsigned int v_degmm_x6_num          : 6;       /* [13:8] */
        unsigned int reserved1               : 2;       /* [15:14] */
        unsigned int v_degmm_x7_num          : 6;       /* [21:16] */
        unsigned int reserved2               : 2;       /* [23:22] */
        unsigned int v_degmm_x8_num          : 6;       /* [29:24] */
        unsigned int reserved3               : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_degamma_num2;

/* Define the union hdr_vdm_tonemap_ren */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vdm_tmap_rd_en          : 1;       /* [0] */
        unsigned int vdm_sm_rd_en            : 1;       /* [1] */
        unsigned int reserved                : 30;      /* [31:2] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_tonemap_ren;

/* Define the union hdr_vdm_tonemap_data */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vdm_tmap_para_data      : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_tonemap_data;

/* Define the union hdr_vdm_tonemap_sm_data */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vdm_sm_para_data        : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_tonemap_sm_data;

/* Define the union hdr_vdm_tonemap_step */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int v_tmap_x1_step          : 5;       /* [4:0] */
        unsigned int reserved0               : 3;       /* [7:5] */
        unsigned int v_tmap_x2_step          : 5;       /* [12:8] */
        unsigned int reserved1               : 3;       /* [15:13] */
        unsigned int v_tmap_x3_step          : 5;       /* [20:16] */
        unsigned int reserved2               : 3;       /* [23:21] */
        unsigned int v_tmap_x4_step          : 5;       /* [28:24] */
        unsigned int reserved3               : 3;       /* [31:29] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_tonemap_step;

/* Define the union hdr_vdm_tonemap_pos0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int v_tmap_x1_pos           : 21;      /* [20:0] */
        unsigned int reserved                : 11;      /* [31:21] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_tonemap_pos0;

/* Define the union hdr_vdm_tonemap_pos1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int v_tmap_x2_pos           : 21;      /* [20:0] */
        unsigned int reserved                : 11;      /* [31:21] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_tonemap_pos1;

/* Define the union hdr_vdm_tonemap_num */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int v_tmap_x1_num           : 6;       /* [5:0] */
        unsigned int reserved0               : 2;       /* [7:6] */
        unsigned int v_tmap_x2_num           : 6;       /* [13:8] */
        unsigned int reserved1               : 2;       /* [15:14] */
        unsigned int v_tmap_x3_num           : 6;       /* [21:16] */
        unsigned int reserved2               : 2;       /* [23:22] */
        unsigned int v_tmap_x4_num           : 6;       /* [29:24] */
        unsigned int reserved3               : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_tonemap_num;

/* Define the union hdr_vdm_tonemap_pos2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int v_tmap_x3_pos           : 21;      /* [20:0] */
        unsigned int reserved                : 11;      /* [31:21] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_tonemap_pos2;

/* Define the union hdr_vdm_tonemap_pos3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int v_tmap_x4_pos           : 21;      /* [20:0] */
        unsigned int reserved                : 11;      /* [31:21] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_tonemap_pos3;

/* Define the union hdr_vdm_tonemap_step2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int v_tmap_x5_step          : 5;       /* [4:0] */
        unsigned int reserved0               : 3;       /* [7:5] */
        unsigned int v_tmap_x6_step          : 5;       /* [12:8] */
        unsigned int reserved1               : 3;       /* [15:13] */
        unsigned int v_tmap_x7_step          : 5;       /* [20:16] */
        unsigned int reserved2               : 3;       /* [23:21] */
        unsigned int v_tmap_x8_step          : 5;       /* [28:24] */
        unsigned int reserved3               : 3;       /* [31:29] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_tonemap_step2;

/* Define the union hdr_vdm_tonemap_pos4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int v_tmap_x5_pos           : 21;      /* [20:0] */
        unsigned int reserved                : 11;      /* [31:21] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_tonemap_pos4;

/* Define the union hdr_vdm_tonemap_pos5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int v_tmap_x6_pos           : 21;      /* [20:0] */
        unsigned int reserved                : 11;      /* [31:21] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_tonemap_pos5;

/* Define the union hdr_vdm_tonemap_num2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int v_tmap_x5_num           : 6;       /* [5:0] */
        unsigned int reserved0               : 2;       /* [7:6] */
        unsigned int v_tmap_x6_num           : 6;       /* [13:8] */
        unsigned int reserved1               : 2;       /* [15:14] */
        unsigned int v_tmap_x7_num           : 6;       /* [21:16] */
        unsigned int reserved2               : 2;       /* [23:22] */
        unsigned int v_tmap_x8_num           : 6;       /* [29:24] */
        unsigned int reserved3               : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_tonemap_num2;

/* Define the union hdr_vdm_tonemap_pos6 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int v_tmap_x7_pos           : 21;      /* [20:0] */
        unsigned int reserved                : 11;      /* [31:21] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_tonemap_pos6;

/* Define the union hdr_vdm_tonemap_pos7 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int v_tmap_x8_pos           : 21;      /* [20:0] */
        unsigned int reserved                : 11;      /* [31:21] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_tonemap_pos7;

/* Define the union hdr_vdm_gamma_ren */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vdm_gamma_rd_en         : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_gamma_ren;

/* Define the union hdr_vdm_gammma_data */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vdm_gamma_para_data     : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_gammma_data;

/* Define the union hdr_vdm_gamma_step */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int v_gmm_x1_step           : 5;       /* [4:0] */
        unsigned int reserved0               : 3;       /* [7:5] */
        unsigned int v_gmm_x2_step           : 5;       /* [12:8] */
        unsigned int reserved1               : 3;       /* [15:13] */
        unsigned int v_gmm_x3_step           : 5;       /* [20:16] */
        unsigned int reserved2               : 3;       /* [23:21] */
        unsigned int v_gmm_x4_step           : 5;       /* [28:24] */
        unsigned int reserved3               : 3;       /* [31:29] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_gamma_step;

/* Define the union hdr_vdm_gamma_pos0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int v_gmm_x1_pos            : 21;      /* [20:0] */
        unsigned int reserved                : 11;      /* [31:21] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_gamma_pos0;

/* Define the union hdr_vdm_gamma_pos1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int v_gmm_x2_pos            : 21;      /* [20:0] */
        unsigned int reserved                : 11;      /* [31:21] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_gamma_pos1;

/* Define the union hdr_vdm_gamma_num */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int v_gmm_x1_num            : 6;       /* [5:0] */
        unsigned int reserved0               : 2;       /* [7:6] */
        unsigned int v_gmm_x2_num            : 6;       /* [13:8] */
        unsigned int reserved1               : 2;       /* [15:14] */
        unsigned int v_gmm_x3_num            : 6;       /* [21:16] */
        unsigned int reserved2               : 2;       /* [23:22] */
        unsigned int v_gmm_x4_num            : 6;       /* [29:24] */
        unsigned int reserved3               : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_gamma_num;

/* Define the union hdr_vdm_gamma_pos2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int v_gmm_x3_pos            : 21;      /* [20:0] */
        unsigned int reserved                : 11;      /* [31:21] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_gamma_pos2;

/* Define the union hdr_vdm_gamma_pos3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int v_gmm_x4_pos            : 21;      /* [20:0] */
        unsigned int reserved                : 11;      /* [31:21] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_gamma_pos3;

/* Define the union hdr_vdm_gamma_step2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int v_gmm_x5_step           : 5;       /* [4:0] */
        unsigned int reserved0               : 3;       /* [7:5] */
        unsigned int v_gmm_x6_step           : 5;       /* [12:8] */
        unsigned int reserved1               : 3;       /* [15:13] */
        unsigned int v_gmm_x7_step           : 5;       /* [20:16] */
        unsigned int reserved2               : 3;       /* [23:21] */
        unsigned int v_gmm_x8_step           : 5;       /* [28:24] */
        unsigned int reserved3               : 3;       /* [31:29] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_gamma_step2;

/* Define the union hdr_vdm_gamma_num2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int v_gmm_x5_num            : 6;       /* [5:0] */
        unsigned int reserved0               : 2;       /* [7:6] */
        unsigned int v_gmm_x6_num            : 6;       /* [13:8] */
        unsigned int reserved1               : 2;       /* [15:14] */
        unsigned int v_gmm_x7_num            : 6;       /* [21:16] */
        unsigned int reserved2               : 2;       /* [23:22] */
        unsigned int v_gmm_x8_num            : 6;       /* [29:24] */
        unsigned int reserved3               : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_gamma_num2;

/* Define the union hdr_vdm_gamma_pos4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int v_gmm_x5_pos            : 21;      /* [20:0] */
        unsigned int reserved                : 11;      /* [31:21] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_gamma_pos4;

/* Define the union hdr_vdm_gamma_pos5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int v_gmm_x6_pos            : 21;      /* [20:0] */
        unsigned int reserved                : 11;      /* [31:21] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_gamma_pos5;

/* Define the union hdr_vdm_gamma_pos6 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int v_gmm_x7_pos            : 21;      /* [20:0] */
        unsigned int reserved                : 11;      /* [31:21] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_gamma_pos6;

/* Define the union hdr_vdm_gamma_pos7 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int v_gmm_x8_pos            : 21;      /* [20:0] */
        unsigned int reserved                : 11;      /* [31:21] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vdm_gamma_pos7;

/* Define the union hdr_disp_0_dither_thr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither0_thr_min         : 16;      /* [15:0] */
        unsigned int dither0_thr_max         : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_0_dither_thr;

/* Define the union hdr_disp_0_dither_sed_y0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither0_sed_y0          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_0_dither_sed_y0;

/* Define the union hdr_disp_0_dither_sed_u0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither0_sed_u0          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_0_dither_sed_u0;

/* Define the union hdr_disp_0_dither_sed_v0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither0_sed_v0          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_0_dither_sed_v0;

/* Define the union hdr_disp_0_dither_sed_w0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither0_sed_w0          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_0_dither_sed_w0;

/* Define the union hdr_disp_0_dither_sed_y1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither0_sed_y1          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_0_dither_sed_y1;

/* Define the union hdr_disp_0_dither_sed_u1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither0_sed_u1          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_0_dither_sed_u1;

/* Define the union hdr_disp_0_dither_sed_v1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither0_sed_v1          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_0_dither_sed_v1;

/* Define the union hdr_disp_0_dither_sed_w1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither0_sed_w1          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_0_dither_sed_w1;

/* Define the union hdr_disp_0_dither_sed_y2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither0_sed_y2          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_0_dither_sed_y2;

/* Define the union hdr_disp_0_dither_sed_u2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither0_sed_u2          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_0_dither_sed_u2;

/* Define the union hdr_disp_0_dither_sed_v2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither0_sed_v2          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_0_dither_sed_v2;

/* Define the union hdr_disp_0_dither_sed_w2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither0_sed_w2          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_0_dither_sed_w2;

/* Define the union hdr_disp_0_dither_sed_y3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither0_sed_y3          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_0_dither_sed_y3;

/* Define the union hdr_disp_0_dither_sed_u3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither0_sed_u3          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_0_dither_sed_u3;

/* Define the union hdr_disp_0_dither_sed_v3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither0_sed_v3          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_0_dither_sed_v3;

/* Define the union hdr_disp_0_dither_sed_w3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither0_sed_w3          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_0_dither_sed_w3;

/* Define the union hdr_disp_0_dither_sed_y4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither0_sed_y4          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_0_dither_sed_y4;

/* Define the union hdr_disp_0_dither_sed_u4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither0_sed_u4          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_0_dither_sed_u4;

/* Define the union hdr_disp_0_dither_sed_v4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither0_sed_v4          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_0_dither_sed_v4;

/* Define the union hdr_disp_0_dither_sed_w4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither0_sed_w4          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_0_dither_sed_w4;

/* Define the union hdr_disp_0_dither_sed_y5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither0_sed_y5          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_0_dither_sed_y5;

/* Define the union hdr_disp_0_dither_sed_u5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither0_sed_u5          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_0_dither_sed_u5;

/* Define the union hdr_disp_0_dither_sed_v5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither0_sed_v5          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_0_dither_sed_v5;

/* Define the union hdr_disp_0_dither_sed_w5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither0_sed_w5          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_0_dither_sed_w5;

/* Define the union hdr_disp_0_dither_sed_y6 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither0_sed_y6          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_0_dither_sed_y6;

/* Define the union hdr_disp_0_dither_sed_u6 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither0_sed_u6          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_0_dither_sed_u6;

/* Define the union hdr_disp_0_dither_sed_v6 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither0_sed_v6          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_0_dither_sed_v6;

/* Define the union hdr_disp_0_dither_sed_w6 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither0_sed_w6          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_0_dither_sed_w6;

/* Define the union hdr_disp_0_dither_sed_y7 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither0_sed_y7          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_0_dither_sed_y7;

/* Define the union hdr_disp_0_dither_sed_u7 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither0_sed_u7          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_0_dither_sed_u7;

/* Define the union hdr_disp_0_dither_sed_v7 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither0_sed_v7          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_0_dither_sed_v7;

/* Define the union hdr_disp_0_dither_sed_w7 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither0_sed_w7          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_0_dither_sed_w7;

/* Define the union hdr_disp_0_dither_tapmode */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither0_tap_mode0       : 2;       /* [1:0] */
        unsigned int dither0_tap_mode1       : 2;       /* [3:2] */
        unsigned int dither0_tap_mode2       : 2;       /* [5:4] */
        unsigned int dither0_tap_mode3       : 2;       /* [7:6] */
        unsigned int dither0_tap_mode4       : 2;       /* [9:8] */
        unsigned int dither0_tap_mode5       : 2;       /* [11:10] */
        unsigned int dither0_tap_mode6       : 2;       /* [13:12] */
        unsigned int dither0_tap_mode7       : 2;       /* [15:14] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_0_dither_tapmode;

/* Define the union hdr_disp_0_dither_coef_rd */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither0_coef_rd_en      : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_0_dither_coef_rd;

/* Define the union hdr_disp_0_dither_coef_rdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither0_coef_rdata      : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_0_dither_coef_rdata;

/* Define the union hdr_disp_0_dither_ink_data */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither0_ink_data        : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_0_dither_ink_data;

/* Define the union hdr_disp_0_dither_ink_data2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither0_ink_data2       : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_0_dither_ink_data2;

/* Define the union hdr_disp_0_dither_ink_iy */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither0_ink_iy          : 16;      /* [15:0] */
        unsigned int dither0_ink_ia          : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_0_dither_ink_iy;

/* Define the union hdr_disp_0_dither_ink_ic */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither0_ink_iv          : 16;      /* [15:0] */
        unsigned int dither0_ink_iu          : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_0_dither_ink_ic;

/* Define the union hdr_disp_0_dither_ink_oy */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither0_ink_oy          : 16;      /* [15:0] */
        unsigned int dither0_ink_oa          : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_0_dither_ink_oy;

/* Define the union hdr_disp_0_dither_ink_oc */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither0_ink_ov          : 16;      /* [15:0] */
        unsigned int dither0_ink_ou          : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_0_dither_ink_oc;

/* Define the union hdr_rcbn_dfx */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dfx_gmap_reso_err_flag  : 1;       /* [0] */
        unsigned int dfx_clip_cfg_err_flag   : 1;       /* [1] */
        unsigned int dfx_chn_num_err_flag    : 1;       /* [2] */
        unsigned int reserved0               : 5;       /* [7:3] */
        unsigned int dfx_busy                : 1;       /* [8] */
        unsigned int dfx_busy_bstart_flag    : 1;       /* [9] */
        unsigned int reserved1               : 6;       /* [15:10] */
        unsigned int dfx_dnmlz_gmap_neg_flag : 1;       /* [16] */
        unsigned int dfx_pow2_cal_huge_flag  : 1;       /* [17] */
        unsigned int dfx_gmap_cal_hugeint_flag : 1;       /* [18] */
        unsigned int dfx_gmap_cal_huge_flag  : 1;       /* [19] */
        unsigned int dfx_gmap_cal_invalid_flag : 1;       /* [20] */
        unsigned int dfx_gmap_cal_infinity_flag : 1;       /* [21] */
        unsigned int reserved2               : 2;       /* [23:22] */
        unsigned int dfx_cbn_cal_hugeint_flag : 1;       /* [24] */
        unsigned int dfx_cbn_cal_huge_flag   : 1;       /* [25] */
        unsigned int dfx_cbn_cal_invalid_flag : 1;       /* [26] */
        unsigned int dfx_cbn_cal_infinity_flag : 1;       /* [27] */
        unsigned int reserved3               : 4;       /* [31:28] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_rcbn_dfx;

/* Define the union hdr_disp_1_dither_thr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither1_thr_min         : 16;      /* [15:0] */
        unsigned int dither1_thr_max         : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_1_dither_thr;

/* Define the union hdr_disp_1_dither_sed_y0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither1_sed_y0          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_1_dither_sed_y0;

/* Define the union hdr_disp_1_dither_sed_u0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither1_sed_u0          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_1_dither_sed_u0;

/* Define the union hdr_disp_1_dither_sed_v0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither1_sed_v0          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_1_dither_sed_v0;

/* Define the union hdr_disp_1_dither_sed_w0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither1_sed_w0          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_1_dither_sed_w0;

/* Define the union hdr_disp_1_dither_sed_y1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither1_sed_y1          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_1_dither_sed_y1;

/* Define the union hdr_disp_1_dither_sed_u1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither1_sed_u1          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_1_dither_sed_u1;

/* Define the union hdr_disp_1_dither_sed_v1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither1_sed_v1          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_1_dither_sed_v1;

/* Define the union hdr_disp_1_dither_sed_w1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither1_sed_w1          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_1_dither_sed_w1;

/* Define the union hdr_disp_1_dither_sed_y2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither1_sed_y2          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_1_dither_sed_y2;

/* Define the union hdr_disp_1_dither_sed_u2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither1_sed_u2          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_1_dither_sed_u2;

/* Define the union hdr_disp_1_dither_sed_v2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither1_sed_v2          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_1_dither_sed_v2;

/* Define the union hdr_disp_1_dither_sed_w2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither1_sed_w2          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_1_dither_sed_w2;

/* Define the union hdr_disp_1_dither_sed_y3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither1_sed_y3          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_1_dither_sed_y3;

/* Define the union hdr_disp_1_dither_sed_u3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither1_sed_u3          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_1_dither_sed_u3;

/* Define the union hdr_disp_1_dither_sed_v3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither1_sed_v3          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_1_dither_sed_v3;

/* Define the union hdr_disp_1_dither_sed_w3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither1_sed_w3          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_1_dither_sed_w3;

/* Define the union hdr_disp_1_dither_sed_y4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither1_sed_y4          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_1_dither_sed_y4;

/* Define the union hdr_disp_1_dither_sed_u4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither1_sed_u4          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_1_dither_sed_u4;

/* Define the union hdr_disp_1_dither_sed_v4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither1_sed_v4          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_1_dither_sed_v4;

/* Define the union hdr_disp_1_dither_sed_w4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither1_sed_w4          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_1_dither_sed_w4;

/* Define the union hdr_disp_1_dither_sed_y5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither1_sed_y5          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_1_dither_sed_y5;

/* Define the union hdr_disp_1_dither_sed_u5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither1_sed_u5          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_1_dither_sed_u5;

/* Define the union hdr_disp_1_dither_sed_v5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither1_sed_v5          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_1_dither_sed_v5;

/* Define the union hdr_disp_1_dither_sed_w5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither1_sed_w5          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_1_dither_sed_w5;

/* Define the union hdr_disp_1_dither_sed_y6 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither1_sed_y6          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_1_dither_sed_y6;

/* Define the union hdr_disp_1_dither_sed_u6 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither1_sed_u6          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_1_dither_sed_u6;

/* Define the union hdr_disp_1_dither_sed_v6 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither1_sed_v6          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_1_dither_sed_v6;

/* Define the union hdr_disp_1_dither_sed_w6 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither1_sed_w6          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_1_dither_sed_w6;

/* Define the union hdr_disp_1_dither_sed_y7 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither1_sed_y7          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_1_dither_sed_y7;

/* Define the union hdr_disp_1_dither_sed_u7 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither1_sed_u7          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_1_dither_sed_u7;

/* Define the union hdr_disp_1_dither_sed_v7 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither1_sed_v7          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_1_dither_sed_v7;

/* Define the union hdr_disp_1_dither_sed_w7 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither1_sed_w7          : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_1_dither_sed_w7;

/* Define the union hdr_disp_1_dither_tapmode */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither1_tap_mode0       : 2;       /* [1:0] */
        unsigned int dither1_tap_mode1       : 2;       /* [3:2] */
        unsigned int dither1_tap_mode2       : 2;       /* [5:4] */
        unsigned int dither1_tap_mode3       : 2;       /* [7:6] */
        unsigned int dither1_tap_mode4       : 2;       /* [9:8] */
        unsigned int dither1_tap_mode5       : 2;       /* [11:10] */
        unsigned int dither1_tap_mode6       : 2;       /* [13:12] */
        unsigned int dither1_tap_mode7       : 2;       /* [15:14] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_1_dither_tapmode;

/* Define the union hdr_disp_1_dither_coef_rd */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither1_coef_rd_en      : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_1_dither_coef_rd;

/* Define the union hdr_disp_1_dither_coef_rdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither1_coef_rdata      : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_1_dither_coef_rdata;

/* Define the union hdr_disp_1_dither_ink_data */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither1_ink_data        : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_1_dither_ink_data;

/* Define the union hdr_disp_1_dither_ink_data2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither1_ink_data2       : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_1_dither_ink_data2;

/* Define the union hdr_disp_1_dither_ink_iy */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither1_ink_iy          : 16;      /* [15:0] */
        unsigned int dither1_ink_ia          : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_1_dither_ink_iy;

/* Define the union hdr_disp_1_dither_ink_ic */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither1_ink_iv          : 16;      /* [15:0] */
        unsigned int dither1_ink_iu          : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_1_dither_ink_ic;

/* Define the union hdr_disp_1_dither_ink_oy */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither1_ink_oy          : 16;      /* [15:0] */
        unsigned int dither1_ink_oa          : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_1_dither_ink_oy;

/* Define the union hdr_disp_1_dither_ink_oc */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither1_ink_ov          : 16;      /* [15:0] */
        unsigned int dither1_ink_ou          : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_disp_1_dither_ink_oc;

/* Define the union hdr_cbm_lay0_debug */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mix_layer0_cnt          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_cbm_lay0_debug;

/* Define the union hdr_cbm_lay1_debug */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mix_layer1_cnt          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_cbm_lay1_debug;

/* Define the union hdr_cbm_lay2_debug */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mix_layer2_cnt          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_cbm_lay2_debug;

/* Define the union hdr_cbm_lay3_debug */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mix_layer3_cnt          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_cbm_lay3_debug;

/* Define the union hdr_cbm_lay4_debug */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mix_layer4_cnt          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_cbm_lay4_debug;

/* Define the union hdr_cbm_lay0_last_debug */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mix_layer0_last_cnt     : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_cbm_lay0_last_debug;

/* Define the union hdr_cbm_lay1_last_debug */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mix_layer1_last_cnt     : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_cbm_lay1_last_debug;

/* Define the union hdr_cbm_lay2_last_debug */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mix_layer2_last_cnt     : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_cbm_lay2_last_debug;

/* Define the union hdr_cbm_lay3_last_debug */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mix_layer3_last_cnt     : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_cbm_lay3_last_debug;

/* Define the union hdr_cbm_lay4_last_debug */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mix_layer4_last_cnt     : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_cbm_lay4_last_debug;

/* Define the union hdr_vhdaccydiffd1sum */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int acc_ydiffd1_sum         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vhdaccydiffd1sum;

/* Define the union hdr_vhdaccydiffd2sum */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int acc_ydiffd2_sum         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vhdaccydiffd2sum;

/* Define the union hdr_vhdaccysum */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int acc_y_sum               : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vhdaccysum;

/* Define the union hdr_vhdaccsatsum */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int acc_sat_sum             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vhdaccsatsum;

/* Define the union hdr_vhdaccsathistsum */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int acc_sat_hist_sum        : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vhdaccsathistsum;

/* Define the union hdr_vhdaccsathistcum */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int acc_sat_hist_cum        : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vhdaccsathistcum;

/* Define the union hdr_vhdaccyhist_0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ycnt32_data0            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vhdaccyhist_0;

/* Define the union hdr_vhdaccyhist_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ycnt32_data1            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vhdaccyhist_1;

/* Define the union hdr_vhdaccyhist_2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ycnt32_data2            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vhdaccyhist_2;

/* Define the union hdr_vhdaccyhist_3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ycnt32_data3            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vhdaccyhist_3;

/* Define the union hdr_vhdaccyhist_4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ycnt32_data4            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vhdaccyhist_4;

/* Define the union hdr_vhdaccyhist_5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ycnt32_data5            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vhdaccyhist_5;

/* Define the union hdr_vhdaccyhist_6 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ycnt32_data6            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vhdaccyhist_6;

/* Define the union hdr_vhdaccyhist_7 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ycnt32_data7            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vhdaccyhist_7;

/* Define the union hdr_vhdaccyhist_8 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ycnt32_data8            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vhdaccyhist_8;

/* Define the union hdr_vhdaccyhist_9 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ycnt32_data9            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vhdaccyhist_9;

/* Define the union hdr_vhdaccyhist_10 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ycnt32_data10           : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vhdaccyhist_10;

/* Define the union hdr_vhdaccyhist_11 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ycnt32_data11           : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vhdaccyhist_11;

/* Define the union hdr_vhdaccyhist_12 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ycnt32_data12           : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vhdaccyhist_12;

/* Define the union hdr_vhdaccyhist_13 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ycnt32_data13           : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vhdaccyhist_13;

/* Define the union hdr_vhdaccyhist_14 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ycnt32_data14           : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vhdaccyhist_14;

/* Define the union hdr_vhdaccyhist_15 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ycnt32_data15           : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vhdaccyhist_15;

/* Define the union hdr_vhdaccyhist_16 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ycnt32_data16           : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vhdaccyhist_16;

/* Define the union hdr_vhdaccyhist_17 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ycnt32_data17           : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vhdaccyhist_17;

/* Define the union hdr_vhdaccyhist_18 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ycnt32_data18           : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vhdaccyhist_18;

/* Define the union hdr_vhdaccyhist_19 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ycnt32_data19           : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vhdaccyhist_19;

/* Define the union hdr_vhdaccsathist_0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int satcnt32_data0          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vhdaccsathist_0;

/* Define the union hdr_vhdaccsathist_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int satcnt32_data1          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vhdaccsathist_1;

/* Define the union hdr_vhdaccsathist_2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int satcnt32_data2          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vhdaccsathist_2;

/* Define the union hdr_vhdaccsathist_3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int satcnt32_data3          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vhdaccsathist_3;

/* Define the union hdr_vhdaccsathist_4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int satcnt32_data4          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vhdaccsathist_4;

/* Define the union hdr_vhdaccsathist_5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int satcnt32_data5          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vhdaccsathist_5;

/* Define the union hdr_vhdaccsathist_6 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int satcnt32_data6          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vhdaccsathist_6;

/* Define the union hdr_vhdaccsathist_7 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int satcnt32_data7          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vhdaccsathist_7;

/* Define the union hdr_vhdaccsathist_8 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int satcnt32_data8          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vhdaccsathist_8;

/* Define the union hdr_vhdaccsathist_9 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int satcnt32_data9          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vhdaccsathist_9;

/* Define the union hdr_vhdaccsathist_10 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int satcnt32_data10         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vhdaccsathist_10;

/* Define the union hdr_vhdaccsathist_11 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int satcnt32_data11         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vhdaccsathist_11;

/* Define the union hdr_vhdaccsathist_12 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int satcnt32_data12         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vhdaccsathist_12;

/* Define the union hdr_vhdaccsathist_13 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int satcnt32_data13         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vhdaccsathist_13;

/* Define the union hdr_vhdaccsathist_14 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int satcnt32_data14         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vhdaccsathist_14;

/* Define the union hdr_vhdaccsathist_15 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int satcnt32_data15         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vhdaccsathist_15;

/* Define the union hdr_vhdaccsathist_16 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int satcnt32_data16         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vhdaccsathist_16;

/* Define the union hdr_vhdaccsathist_17 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int satcnt32_data17         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vhdaccsathist_17;

/* Define the union hdr_vhdaccsathist_18 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int satcnt32_data18         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vhdaccsathist_18;

/* Define the union hdr_vhdaccsathist_19 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int satcnt32_data19         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdr_vhdaccsathist_19;

/* Define the union vau_work_state */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vas_work_state          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_work_state;

/* Define the union vau_cur_list_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int p_last                  : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_cur_list_addr;

/* Define the union vau_cur_node_id */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int nodeid_last             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_cur_node_id;

/* Define the union vau_mac_rdy_need */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int out_src1_y_drdy         : 1;       /* [0] */
        unsigned int out_src1_y_dneed        : 1;       /* [1] */
        unsigned int out_src1_c_drdy         : 1;       /* [2] */
        unsigned int out_src1_c_dneed        : 1;       /* [3] */
        unsigned int out_src2_y_drdy         : 1;       /* [4] */
        unsigned int out_src2_y_dneed        : 1;       /* [5] */
        unsigned int out_src2_c_drdy         : 1;       /* [6] */
        unsigned int out_src2_c_dneed        : 1;       /* [7] */
        unsigned int in_des_y_drdy           : 1;       /* [8] */
        unsigned int in_des_y_dneed          : 1;       /* [9] */
        unsigned int in_des_c_drdy           : 1;       /* [10] */
        unsigned int in_des_c_dneed          : 1;       /* [11] */
        unsigned int in_map_model_req_ctrl_drdy : 1;       /* [12] */
        unsigned int in_map_model_req_ctrl_dneed : 1;       /* [13] */
        unsigned int in_map_data_req_ctrl_drdy : 1;       /* [14] */
        unsigned int in_map_data_req_ctrl_dneed : 1;       /* [15] */
        unsigned int mapping_model_drdy      : 1;       /* [16] */
        unsigned int mapping_model_dneed     : 1;       /* [17] */
        unsigned int reserved                : 14;      /* [31:18] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mac_rdy_need;

/* Define the union vau_mac_chn_state */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_ch_en              : 1;       /* [0] */
        unsigned int src2_ch_en              : 1;       /* [1] */
        unsigned int src2c_ch_en             : 1;       /* [2] */
        unsigned int model_ch_en             : 1;       /* [3] */
        unsigned int des_chn_en              : 1;       /* [4] */
        unsigned int read_done_src1          : 1;       /* [5] */
        unsigned int read_done_src2          : 1;       /* [6] */
        unsigned int read_done_src2c         : 1;       /* [7] */
        unsigned int read_done_model         : 1;       /* [8] */
        unsigned int write_done_des          : 1;       /* [9] */
        unsigned int draw_ch_en              : 1;       /* [10] */
        unsigned int draw_cmd_en             : 1;       /* [11] */
        unsigned int draw_path_done          : 1;       /* [12] */
        unsigned int draw_wr_done            : 1;       /* [13] */
        unsigned int reserved                : 18;      /* [31:14] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mac_chn_state;

/* Define the union vau_proc_rdy_need */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int proc2cbm_out_src1_dneed : 1;       /* [0] */
        unsigned int proc2cbm_out_src1_drdy  : 1;       /* [1] */
        unsigned int proc2cbm_out_src2_dneed : 1;       /* [2] */
        unsigned int proc2cbm_out_src2_drdy  : 1;       /* [3] */
        unsigned int proc2cbm_in_des_dneed   : 1;       /* [4] */
        unsigned int proc2cbm_in_des_drdy    : 1;       /* [5] */
        unsigned int reserved                : 26;      /* [31:6] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_proc_rdy_need;

/* Define the union vau_proc_src1_rdy_need */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int in_src1_y_drdy          : 1;       /* [0] */
        unsigned int in_src1_y_dneed         : 1;       /* [1] */
        unsigned int in_src1_c_drdy          : 1;       /* [2] */
        unsigned int in_src1_c_dneed         : 1;       /* [3] */
        unsigned int out_src1_drdy           : 1;       /* [4] */
        unsigned int out_src1_dneed          : 1;       /* [5] */
        unsigned int to_t2r_src1_drdy        : 1;       /* [6] */
        unsigned int to_t2r_src1_dneed       : 1;       /* [7] */
        unsigned int from_t2r_src1_drdy      : 1;       /* [8] */
        unsigned int from_t2r_src1_dneed     : 1;       /* [9] */
        unsigned int src1_crop_en            : 1;       /* [10] */
        unsigned int src1_swap_en            : 1;       /* [11] */
        unsigned int reserved                : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_proc_src1_rdy_need;

/* Define the union vau_proc_src2_rdy_need */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int in_src2_y_drdy          : 1;       /* [0] */
        unsigned int in_src2_y_dneed         : 1;       /* [1] */
        unsigned int in_src2_c_drdy          : 1;       /* [2] */
        unsigned int in_src2_c_dneed         : 1;       /* [3] */
        unsigned int out_src2_drdy           : 1;       /* [4] */
        unsigned int out_src2_dneed          : 1;       /* [5] */
        unsigned int to_t2r_src2_drdy        : 1;       /* [6] */
        unsigned int to_t2r_src2_dneed       : 1;       /* [7] */
        unsigned int from_t2r_src2_drdy      : 1;       /* [8] */
        unsigned int from_t2r_src2_dneed     : 1;       /* [9] */
        unsigned int in_hsv_src1_y_drdy      : 1;       /* [10] */
        unsigned int in_hsv_src1_y_dneed     : 1;       /* [11] */
        unsigned int i_hsv_rgb_y_drdy        : 1;       /* [12] */
        unsigned int i_hsv_rgb_y_dneed       : 1;       /* [13] */
        unsigned int i_hsv_rgb_c_drdy        : 1;       /* [14] */
        unsigned int i_hsv_rgb_c_dneed       : 1;       /* [15] */
        unsigned int in_fill_data_drdy       : 1;       /* [16] */
        unsigned int in_fill_data_dneed      : 1;       /* [17] */
        unsigned int in_add_drdy             : 1;       /* [18] */
        unsigned int in_add_dneed            : 1;       /* [19] */
        unsigned int src2_crop_en            : 1;       /* [20] */
        unsigned int src2_swap_en            : 1;       /* [21] */
        unsigned int add_pix_en              : 1;       /* [22] */
        unsigned int fill_en                 : 1;       /* [23] */
        unsigned int sp_en                   : 1;       /* [24] */
        unsigned int reserved                : 7;       /* [31:25] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_proc_src2_rdy_need;

/* Define the union vau_proc_des_rdy_need */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int in_des_drdy             : 1;       /* [0] */
        unsigned int in_des_dneed            : 1;       /* [1] */
        unsigned int out_des_y_drdy          : 1;       /* [2] */
        unsigned int out_des_y_dneed         : 1;       /* [3] */
        unsigned int out_des_c_drdy          : 1;       /* [4] */
        unsigned int out_des_c_dneed         : 1;       /* [5] */
        unsigned int to_t2r_des_drdy         : 1;       /* [6] */
        unsigned int to_t2r_des_dneed        : 1;       /* [7] */
        unsigned int from_t2r_des_drdy       : 1;       /* [8] */
        unsigned int from_t2r_des_dneed      : 1;       /* [9] */
        unsigned int crop_en                 : 1;       /* [10] */
        unsigned int swap_en                 : 1;       /* [11] */
        unsigned int reserved                : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_proc_des_rdy_need;

/* Define the union vau_cbm_rdy_need */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_drdy               : 1;       /* [0] */
        unsigned int src1_dneed              : 1;       /* [1] */
        unsigned int src2_drdy               : 1;       /* [2] */
        unsigned int src2_dneed              : 1;       /* [3] */
        unsigned int src3_drdy               : 1;       /* [4] */
        unsigned int src3_dneed              : 1;       /* [5] */
        unsigned int src4_drdy               : 1;       /* [6] */
        unsigned int src4_dneed              : 1;       /* [7] */
        unsigned int src5_drdy               : 1;       /* [8] */
        unsigned int src5_dneed              : 1;       /* [9] */
        unsigned int src6_drdy               : 1;       /* [10] */
        unsigned int src6_dneed              : 1;       /* [11] */
        unsigned int src7_drdy               : 1;       /* [12] */
        unsigned int src7_dneed              : 1;       /* [13] */
        unsigned int out_drdy                : 1;       /* [14] */
        unsigned int out_dneed               : 1;       /* [15] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_cbm_rdy_need;

/* Define the union vau_axi_ctrl_signal */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int bready                  : 1;       /* [0] */
        unsigned int bvalid                  : 1;       /* [1] */
        unsigned int wready                  : 1;       /* [2] */
        unsigned int wvalid                  : 1;       /* [3] */
        unsigned int awready                 : 1;       /* [4] */
        unsigned int awvalid                 : 1;       /* [5] */
        unsigned int rready                  : 1;       /* [6] */
        unsigned int rvalid                  : 1;       /* [7] */
        unsigned int arready                 : 1;       /* [8] */
        unsigned int arvalid                 : 1;       /* [9] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_axi_ctrl_signal;

/* Define the union vau_offline_dcmp_dbg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vau_offline_dcmp_dbg    : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_offline_dcmp_dbg;

/* Define the union vau_debug12 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int debug12                 : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_debug12;

/* Define the union vau_debug13 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int debug13                 : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_debug13;

/* Define the union vau_debug14 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int debug14                 : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_debug14;

/* Define the union vau_debug15 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int debug15                 : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_debug15;

/* Define the union vau_debug16 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int debug16                 : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_debug16;

/* Define the union vau_debug17 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int debug17                 : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_debug17;

/* Define the union vau_debug18 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int debug18                 : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_debug18;

/* Define the union vau_debug19 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int debug19                 : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_debug19;

/* Define the union vau_debug20 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int debug20                 : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_debug20;

/* Define the union vau_debug21 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int debug21                 : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_debug21;

/* Define the union vau_gauss_dbg0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int debug22                 : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gauss_dbg0;

/* Define the union vau_gauss_dbg1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vau_gauss_dbg           : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gauss_dbg1;

/* Define the union vau_cas_start_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vau_cas_start_cnt       : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_cas_start_cnt;

/* Define the union vau_eof_end_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vau_eof_end_cnt         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_eof_end_cnt;

/* Define the union vau_vau_int_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vau_vau_int_cnt         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_vau_int_cnt;

/* Define the union vau_online_dcmp_dbg0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vau_online_dcmp_dbg0    : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_online_dcmp_dbg0;

/* Define the union vau_online_dcmp_dbg1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vau_online_dcmp_dbg1    : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_online_dcmp_dbg1;

/* Define the union vau_list_checksum */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vau_list_checksum       : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_list_checksum;

/* Define the union vau_src1_checksum */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vau_src1_checksum       : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src1_checksum;

/* Define the union vau_src2_checksum */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vau_src2_checksum       : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src2_checksum;

/* Define the union vau_src2c_checksum */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vau_src2c_checksum      : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src2c_checksum;

/* Define the union vau_model_checksum */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vau_model_checksum      : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_model_checksum;

/* Define the union vau_des_checksum */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vau_des_checksum        : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_des_checksum;

/* Define the union vau_done_list_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int p_last_done             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_done_list_addr;

/* Define the union vau_done_node_id */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int nodeid_last_done        : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_done_node_id;

/* Define the union vau_pfcnt_list */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pfcnt_list              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_pfcnt_list;

/* Define the union vau_tunl_sta */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tunl_sta                : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_tunl_sta;

/* Define the union vau_tunl_wr_ptr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tunl_wr_ptr             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_tunl_wr_ptr;

/* Define the union vau_tunl_rd_ptr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tunl_rd_ptr             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_tunl_rd_ptr;

/* Define the union vau_done_offline_list_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vau_done_offline_list_cnt : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_done_offline_list_cnt;

/* Define the union vau_hdr_mac_rdy_need */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int out_src1_y_drdy         : 1;       /* [0] */
        unsigned int out_src1_y_dneed        : 1;       /* [1] */
        unsigned int out_src1_c_drdy         : 1;       /* [2] */
        unsigned int out_src1_c_dneed        : 1;       /* [3] */
        unsigned int out_src2_y_drdy         : 1;       /* [4] */
        unsigned int out_src2_y_dneed        : 1;       /* [5] */
        unsigned int out_src2_c_drdy         : 1;       /* [6] */
        unsigned int out_src2_c_dneed        : 1;       /* [7] */
        unsigned int in_des_y_drdy           : 1;       /* [8] */
        unsigned int in_des_y_dneed          : 1;       /* [9] */
        unsigned int in_des_c_drdy           : 1;       /* [10] */
        unsigned int in_des_c_dneed          : 1;       /* [11] */
        unsigned int in_map_model_req_ctrl_drdy : 1;       /* [12] */
        unsigned int in_map_model_req_ctrl_dneed : 1;       /* [13] */
        unsigned int in_map_data_req_ctrl_drdy : 1;       /* [14] */
        unsigned int in_map_data_req_ctrl_dneed : 1;       /* [15] */
        unsigned int mapping_model_drdy      : 1;       /* [16] */
        unsigned int mapping_model_dneed     : 1;       /* [17] */
        unsigned int reserved                : 14;      /* [31:18] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hdr_mac_rdy_need;

/* Define the union vau_hdr_mac_chn_state */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_ch_en              : 1;       /* [0] */
        unsigned int src2_ch_en              : 1;       /* [1] */
        unsigned int src2c_ch_en             : 1;       /* [2] */
        unsigned int model_ch_en             : 1;       /* [3] */
        unsigned int des_chn_en              : 1;       /* [4] */
        unsigned int src1c_ch_en             : 1;       /* [5] */
        unsigned int read_done_src1          : 1;       /* [6] */
        unsigned int read_done_src2          : 1;       /* [7] */
        unsigned int read_done_src2c         : 1;       /* [8] */
        unsigned int read_done_model         : 1;       /* [9] */
        unsigned int write_done_des          : 1;       /* [10] */
        unsigned int read_done_src1c         : 1;       /* [11] */
        unsigned int draw_ch_en              : 1;       /* [12] */
        unsigned int draw_cmd_en             : 1;       /* [13] */
        unsigned int draw_path_done          : 1;       /* [14] */
        unsigned int draw_wr_done            : 1;       /* [15] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hdr_mac_chn_state;

/* Define the union vau_hdr_src1_checksum */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vau_hdr_src1_checksum   : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hdr_src1_checksum;

/* Define the union vau_hdr_src2_checksum */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vau_hdr_src2_checksum   : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hdr_src2_checksum;

/* Define the union vau_hdr_src2c_checksum */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vau_hdr_src2c_checksum  : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hdr_src2c_checksum;

/* Define the union vau_hdr_src1c_checksum */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vau_hdr_model_checksum  : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hdr_src1c_checksum;

/* Define the union vau_hdr_des_checksum */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vau_hdr_des_checksum    : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_hdr_des_checksum;


/*
 * DEFINE GLOBAL STRUCT
 */
typedef struct {
    u_vau_src1_ctrl                      vau_src1_ctrl;                       /* 0 */
    u_vau_src1_addr                      vau_src1_addr;                       /* 4 */
    u_vau_src1_stride                    vau_src1_stride;                     /* 8 */
    u_vau_src1_imgsize                   vau_src1_imgsize;                    /* c */
    u_vau_src1_rect_xy                   vau_src1_rect_xy;                    /* 10 */
    u_vau_src1_rect_wh                   vau_src1_rect_wh;                    /* 14 */
    u_vau_src1_fill                      vau_src1_fill;                       /* 18 */
    u_vau_src1_alpha                     vau_src1_alpha;                      /* 1c */
    u_vau_src1_pix_rgb                   vau_src1_pix_rgb;                    /* 20 */
    u_vau_src1_htc_online_dcmp0          vau_src1_htc_online_dcmp0;           /* 24 */
    unsigned int                         rsr0;                                /* 28 */
    u_vau_src2_ctrl                      vau_src2_ctrl;                       /* 2c */
    u_vau_src2_addr                      vau_src2_addr;                       /* 30 */
    u_vau_src2_stride                    vau_src2_stride;                     /* 34 */
    u_vau_src2_imgsize                   vau_src2_imgsize;                    /* 38 */
    u_vau_src2_rect_xy                   vau_src2_rect_xy;                    /* 3c */
    u_vau_src2_rect_wh                   vau_src2_rect_wh;                    /* 40 */
    u_vau_src2_fill                      vau_src2_fill;                       /* 44 */
    u_vau_src2_alpha                     vau_src2_alpha;                      /* 48 */
    u_vau_src2_pix_rgb                   vau_src2_pix_rgb;                    /* 4c */
    u_vau_src2_dcmp_cfg0                 vau_src2_dcmp_cfg0;                  /* 50 */
    u_vau_src2_dcmp_cfg1                 vau_src2_dcmp_cfg1;                  /* 54 */
    u_vau_src2_dcmp_cfg2                 vau_src2_dcmp_cfg2;                  /* 58 */
    u_vau_src2_dcmp_cfg3                 vau_src2_dcmp_cfg3;                  /* 5c */
    u_vau_src2_htc_online_dcmp0          vau_src2_htc_online_dcmp0;           /* 60 */
    unsigned int                         rsr1;                                /* 64 */
    u_vau_model_ctrl                     vau_model_ctrl;                      /* 68 */
    u_vau_model_addr                     vau_model_addr;                      /* 6c */
    u_vau_model_stride                   vau_model_stride;                    /* 70 */
    u_vau_des_ctrl                       vau_des_ctrl;                        /* 74 */
    u_vau_des_addr                       vau_des_addr;                        /* 78 */
    u_vau_des_stride                     vau_des_stride;                      /* 7c */
    u_vau_des_imgsize                    vau_des_imgsize;                     /* 80 */
    u_vau_des_rect_xy                    vau_des_rect_xy;                     /* 84 */
    u_vau_des_rect_wh                    vau_des_rect_wh;                     /* 88 */
    u_vau_des_alpha                      vau_des_alpha;                       /* 8c */
    u_vau_des_htc_online_cmp0            vau_des_htc_online_cmp0;             /* 90 */
    unsigned int                         rsr2;                                /* 94 */
    u_vau_cbm_ctrl                       vau_cbm_ctrl;                        /* 98 */
    u_vau_cbm_colorize                   vau_cbm_colorize;                    /* 9c */
    u_vau_cbm_alu_para                   vau_cbm_alu_para;                    /* a0 */
    u_vau_cbm_key_para                   vau_cbm_key_para;                    /* a4 */
    u_vau_cbm_key_min                    vau_cbm_key_min;                     /* a8 */
    u_vau_cbm_key_max                    vau_cbm_key_max;                     /* ac */
    u_vau_cbm_key_mask                   vau_cbm_key_mask;                    /* b0 */
    u_vau_cbm_src1_cbm_para              vau_cbm_src1_cbm_para;               /* b4 */
    u_vau_cbm_src2_cbm_para              vau_cbm_src2_cbm_para;               /* b8 */
    u_vau_gradient_typical_color         vau_gradient_typical_color;          /* bc */
    u_vau_gradient_linear_const_x        vau_gradient_linear_const_x;         /* c0 */
    u_vau_gradient_linear_const_y        vau_gradient_linear_const_y;         /* c4 */
    u_vau_gradient_linear_start          vau_gradient_linear_start;           /* c8 */
    u_vau_gradient_const_mode            vau_gradient_const_mode;             /* cc */
    u_vau_gradient_radial_const_r        vau_gradient_radial_const_r;         /* d0 */
    u_vau_gradient_radial_center         vau_gradient_radial_center;          /* d4 */
    u_vau_gradient_sweep_center          vau_gradient_sweep_center;           /* d8 */
    u_vau_gradient_sweep_start_angle     vau_gradient_sweep_start_angle;      /* dc */
    u_vau_gradient_tile_mode             vau_gradient_tile_mode;              /* e0 */
    u_vau_gradient_stops_offset0         vau_gradient_stops_offset0;          /* e4 */
    u_vau_gradient_stops_offset1         vau_gradient_stops_offset1;          /* e8 */
    u_vau_gradient_stops_color0          vau_gradient_stops_color0;           /* ec */
    u_vau_gradient_stops_color1          vau_gradient_stops_color1;           /* f0 */
    u_vau_gradient_stops_color2          vau_gradient_stops_color2;           /* f4 */
    u_vau_gradient_stops_color3          vau_gradient_stops_color3;           /* f8 */
    u_vau_gradient_stops_color4          vau_gradient_stops_color4;           /* fc */
    u_vau_gradient_stops_color5          vau_gradient_stops_color5;           /* 100 */
    u_vau_gradient_stops_color6          vau_gradient_stops_color6;           /* 104 */
    u_vau_gradient_color_reci0           vau_gradient_color_reci0;            /* 108 */
    u_vau_gradient_color_reci1           vau_gradient_color_reci1;            /* 10c */
    u_vau_gradient_color_reci2           vau_gradient_color_reci2;            /* 110 */
    u_vau_gradient_color_reci3           vau_gradient_color_reci3;            /* 114 */
    u_vau_gradient_color_reci4           vau_gradient_color_reci4;            /* 118 */
    u_vau_gradient_color_reci5           vau_gradient_color_reci5;            /* 11c */
    u_vau_gradient_stops_num             vau_gradient_stops_num;              /* 120 */
    u_local_lighting_cfg                 local_lighting_cfg;                  /* 124 */
    u_local_lighting_cx                  local_lighting_cx;                   /* 128 */
    u_local_lighting_cy                  local_lighting_cy;                   /* 12c */
    u_local_lighting_radius2             local_lighting_radius2;              /* 130 */
    u_local_lighting_amplitude           local_lighting_amplitude;            /* 134 */
    u_local_lighting_ampli_scale         local_lighting_ampli_scale;          /* 138 */
    u_local_lighting_start_angle_rad     local_lighting_start_angle_rad;      /* 13c */
    u_local_lighting_start_end_angle_rad local_lighting_start_end_angle_rad;  /* 140 */
    u_local_lighting_end_start_angle_rad local_lighting_end_start_angle_rad;  /* 144 */
    u_local_lighting_end_angle_rad       local_lighting_end_angle_rad;        /* 148 */
    u_local_lighting_triwin2_coeff0      local_lighting_triwin2_coeff0;       /* 14c */
    u_local_lighting_triwin2_coeff1      local_lighting_triwin2_coeff1;       /* 150 */
    u_local_lighting_amb_amp             local_lighting_amb_amp;              /* 154 */
    u_local_lighting_bbox0_p0            local_lighting_bbox0_p0;             /* 158 */
    u_local_lighting_bbox0_p1            local_lighting_bbox0_p1;             /* 15c */
    u_local_lighting_bbox1_p0            local_lighting_bbox1_p0;             /* 160 */
    u_local_lighting_bbox1_p1            local_lighting_bbox1_p1;             /* 164 */
    u_local_lighting_img_reso            local_lighting_img_reso;             /* 168 */
    u_local_lighting_start_pos           local_lighting_start_pos;            /* 16c */
    u_gauss_blur_ctrl                    gauss_blur_ctrl;                     /* 170 */
    u_gauss_blur_a0                      gauss_blur_a0;                       /* 174 */
    u_gauss_blur_a1                      gauss_blur_a1;                       /* 178 */
    u_gauss_blur_a2                      gauss_blur_a2;                       /* 17c */
    u_gauss_blur_b_causala0              gauss_blur_b_causala0;               /* 180 */
    u_gauss_blur_b_causala1              gauss_blur_b_causala1;               /* 184 */
    u_gauss_blur_b_causala2              gauss_blur_b_causala2;               /* 188 */
    u_gauss_blur_b_anticausala0          gauss_blur_b_anticausala0;           /* 18c */
    u_gauss_blur_b_anticausala1          gauss_blur_b_anticausala1;           /* 190 */
    u_gauss_blur_b_anticausala2          gauss_blur_b_anticausala2;           /* 194 */
    u_gauss_blur_causala_ratio           gauss_blur_causala_ratio;            /* 198 */
    u_gauss_blur_anticausala_ratio       gauss_blur_anticausala_ratio;        /* 19c */
    u_vau_t2r0_ctrl                      vau_t2r0_ctrl;                       /* 1a0 */
    u_vau_trans_ctrl                     vau_trans_ctrl;                      /* 1a4 */
    u_vau_t2r1_ctrl                      vau_t2r1_ctrl;                       /* 1a8 */
    unsigned int                         rsr3;                                /* 1ac */
    u_vau_clut_addr                      vau_clut_addr;                       /* 1b0 */
    u_vau_node_cfg                       vau_node_cfg;                        /* 1b4 */
    u_vau_node_id                        vau_node_id;                         /* 1b8 */
    u_vau_intmask                        vau_intmask;                         /* 1bc */
    u_vau_pnext                          vau_pnext;                           /* 1c0 */
    u_vau_fpga_tag                       vau_fpga_tag;                        /* 1c4 */
    u_vau_hdr_sys_ctrl                   vau_hdr_sys_ctrl;                    /* 1c8 */
    u_vau_hdr_sys_ctrl1                  vau_hdr_sys_ctrl1;                   /* 1cc */
    unsigned int                         rsr4[12];                            /* 1d0 ~ 1fc */
    u_vau_draw_ctrl                      vau_draw_ctrl;                       /* 200 */
    u_vau_draw_st                        vau_draw_st;                         /* 204 */
    u_vau_draw_end                       vau_draw_end;                        /* 208 */
    u_vau_draw_cmd_addr                  vau_draw_cmd_addr;                   /* 20c */
    u_vau_draw_data_addr                 vau_draw_data_addr;                  /* 210 */
    u_vau_draw_cmd_addr_offset           vau_draw_cmd_addr_offset;            /* 214 */
    u_vau_draw_data_addr_offset          vau_draw_data_addr_offset;           /* 218 */
    u_vau_draw_path_num                  vau_draw_path_num;                   /* 21c */
    u_vau_draw_matrix_00                 vau_draw_matrix_00;                  /* 220 */
    u_vau_draw_matrix_01                 vau_draw_matrix_01;                  /* 224 */
    u_vau_draw_matrix_02                 vau_draw_matrix_02;                  /* 228 */
    u_vau_draw_matrix_10                 vau_draw_matrix_10;                  /* 22c */
    u_vau_draw_matrix_11                 vau_draw_matrix_11;                  /* 230 */
    u_vau_draw_matrix_12                 vau_draw_matrix_12;                  /* 234 */
    u_vau_draw_half_width                vau_draw_half_width;                 /* 238 */
    u_vau_tess_line_num_max              vau_tess_line_num_max;               /* 23c */
    u_vau_draw_rnd_m11                   vau_draw_rnd_m11;                    /* 240 */
    u_vau_draw_rnd_m12                   vau_draw_rnd_m12;                    /* 244 */
    u_vau_draw_rnd_m21                   vau_draw_rnd_m21;                    /* 248 */
    u_vau_draw_rnd_m22                   vau_draw_rnd_m22;                    /* 24c */
    u_vau_draw_rnd_d1                    vau_draw_rnd_d1;                     /* 250 */
    u_vau_draw_rnd_d2                    vau_draw_rnd_d2;                     /* 254 */
    u_vau_draw_rnd_e11                   vau_draw_rnd_e11;                    /* 258 */
    u_vau_draw_rnd_e12                   vau_draw_rnd_e12;                    /* 25c */
    u_vau_draw_rnd_e21                   vau_draw_rnd_e21;                    /* 260 */
    u_vau_draw_rnd_e22                   vau_draw_rnd_e22;                    /* 264 */
    u_vau_draw_threshold_quad            vau_draw_threshold_quad;             /* 268 */
    u_vau_draw_threshold_cubic           vau_draw_threshold_cubic;            /* 26c */
    u_vau_draw_miter_limit               vau_draw_miter_limit;                /* 270 */
    unsigned int                         rsr5[35];                            /* 274 ~ 2fc */
    u_vau_mapping_ctrl                   vau_mapping_ctrl;                    /* 300 */
    u_vau_mapping_coef00                 vau_mapping_coef00;                  /* 304 */
    u_vau_mapping_coef01                 vau_mapping_coef01;                  /* 308 */
    u_vau_mapping_coef02                 vau_mapping_coef02;                  /* 30c */
    u_vau_mapping_coef03                 vau_mapping_coef03;                  /* 310 */
    u_vau_mapping_coef10                 vau_mapping_coef10;                  /* 314 */
    u_vau_mapping_coef11                 vau_mapping_coef11;                  /* 318 */
    u_vau_mapping_coef12                 vau_mapping_coef12;                  /* 31c */
    u_vau_mapping_coef13                 vau_mapping_coef13;                  /* 320 */
    u_vau_mapping_coef20                 vau_mapping_coef20;                  /* 324 */
    u_vau_mapping_coef21                 vau_mapping_coef21;                  /* 328 */
    u_vau_mapping_coef22                 vau_mapping_coef22;                  /* 32c */
    u_vau_mapping_coef23                 vau_mapping_coef23;                  /* 330 */
    u_vau_mapping_coef30                 vau_mapping_coef30;                  /* 334 */
    u_vau_mapping_coef31                 vau_mapping_coef31;                  /* 338 */
    u_vau_mapping_coef32                 vau_mapping_coef32;                  /* 33c */
    u_vau_mapping_coef33                 vau_mapping_coef33;                  /* 340 */
    u_vau_mapping_model_wh               vau_mapping_model_wh;                /* 344 */
    u_vau_mapping_pattern_fill_color     vau_mapping_pattern_fill_color;      /* 348 */
    u_vau_mapping_shadow_color           vau_mapping_shadow_color;            /* 34c */
    u_vau_mapping_bg_color               vau_mapping_bg_color;                /* 350 */
    u_vau_mapping_scale_x                vau_mapping_scale_x;                 /* 354 */
    u_vau_mapping_scale_y                vau_mapping_scale_y;                 /* 358 */
    u_vau_mapping_offset_x               vau_mapping_offset_x;                /* 35c */
    u_vau_mapping_offset_y               vau_mapping_offset_y;                /* 360 */
    u_vau_mapping_src_ratio_x            vau_mapping_src_ratio_x;             /* 364 */
    u_vau_mapping_src_ratio_y            vau_mapping_src_ratio_y;             /* 368 */
    u_vau_mapping_src_offset_x           vau_mapping_src_offset_x;            /* 36c */
    u_vau_mapping_src_offset_y           vau_mapping_src_offset_y;            /* 370 */
    u_vau_hsv_rgb_cfg                    vau_hsv_rgb_cfg;                     /* 374 */
    u_vau_hsv_rgb_adj_h_v                vau_hsv_rgb_adj_h_v;                 /* 378 */
    u_vau_hsv_rgb_adj_s                  vau_hsv_rgb_adj_s;                   /* 37c */
    u_vau_src2c_addr                     vau_src2c_addr;                      /* 380 */
    u_vau_src2c_stride                   vau_src2c_stride;                    /* 384 */
    u_vau_cvfir_vinfo                    vau_cvfir_vinfo;                     /* 388 */
    u_vau_cvfir_vsp                      vau_cvfir_vsp;                       /* 38c */
    u_vau_cvfir_voffset                  vau_cvfir_voffset;                   /* 390 */
    u_vau_cvfir_vboffset                 vau_cvfir_vboffset;                  /* 394 */
    u_vau_hfir_ctrl                      vau_hfir_ctrl;                       /* 398 */
    u_vau_hipp_csc_ctrl                  vau_hipp_csc_ctrl;                   /* 39c */
    u_rnd_crop_lut_rd_en                 rnd_crop_lut_rd_en;                  /* 3a0 */
    u_rnd_crop_lut_rdata                 rnd_crop_lut_rdata;                  /* 3a4 */
    unsigned int                         rsr6[22];                            /* 3a8 ~ 3fc */
    u_reserved                           reserved;                            /* 400 */
    u_vau_trans_src1_reso                vau_trans_src1_reso;                 /* 404 */
    u_vau_trans_src1_pos                 vau_trans_src1_pos;                  /* 408 */
    u_vau_trans_src2_reso                vau_trans_src2_reso;                 /* 40c */
    u_vau_trans_src2_crop_xy             vau_trans_src2_crop_xy;              /* 410 */
    u_vau_trans_src2_crop_wh             vau_trans_src2_crop_wh;              /* 414 */
    u_vau_mapping_crop_xy                vau_mapping_crop_xy;                 /* 418 */
    u_vau_mapping_crop_wh                vau_mapping_crop_wh;                 /* 41c */
    u_mapping_src_crop_xy                mapping_src_crop_xy;                 /* 420 */
    u_mapping_src_crop_wh                mapping_src_crop_wh;                 /* 424 */
    u_vau_src1_crop_xy                   vau_src1_crop_xy;                    /* 428 */
    u_vau_src1_crop_wh                   vau_src1_crop_wh;                    /* 42c */
    u_vau_src2_crop_xy                   vau_src2_crop_xy;                    /* 430 */
    u_vau_src2_crop_wh                   vau_src2_crop_wh;                    /* 434 */
    u_vau_des_crop_xy                    vau_des_crop_xy;                     /* 438 */
    u_vau_des_crop_wh                    vau_des_crop_wh;                     /* 43c */
    u_vau_t2r0_wh                        vau_t2r0_wh;                         /* 440 */
    u_vau_t2r1_wh                        vau_t2r1_wh;                         /* 444 */
    u_vau_src1_add_pix_ctrl              vau_src1_add_pix_ctrl;               /* 448 */
    u_vau_src1_add_pix_small             vau_src1_add_pix_small;              /* 44c */
    u_vau_src1_add_pix_big               vau_src1_add_pix_big;                /* 450 */
    u_vau_src1_add_pix_rect              vau_src1_add_pix_rect;               /* 454 */
    u_vau_src1_add_pix_tile              vau_src1_add_pix_tile;               /* 458 */
    u_vau_src2_add_pix_ctrl              vau_src2_add_pix_ctrl;               /* 45c */
    u_vau_src2_add_pix_small             vau_src2_add_pix_small;              /* 460 */
    u_vau_src2_add_pix_big               vau_src2_add_pix_big;                /* 464 */
    u_vau_src2_add_pix_rect              vau_src2_add_pix_rect;               /* 468 */
    u_vau_src2_add_pix_tile              vau_src2_add_pix_tile;               /* 46c */
    u_vau_des_add_pix_ctrl               vau_des_add_pix_ctrl;                /* 470 */
    u_vau_des_add_pix_small              vau_des_add_pix_small;               /* 474 */
    u_vau_des_add_pix_big                vau_des_add_pix_big;                 /* 478 */
    u_vau_des_add_pix_rect               vau_des_add_pix_rect;                /* 47c */
    u_vau_des_add_pix_tile               vau_des_add_pix_tile;                /* 480 */
    u_vau_draw_add_pix_ctrl              vau_draw_add_pix_ctrl;               /* 484 */
    u_vau_draw_add_pix_small             vau_draw_add_pix_small;              /* 488 */
    u_vau_draw_add_pix_big               vau_draw_add_pix_big;                /* 48c */
    u_vau_draw_add_pix_rect              vau_draw_add_pix_rect;               /* 490 */
    u_vau_draw_add_pix_tile              vau_draw_add_pix_tile;               /* 494 */
    u_vau_reso_src1_crop                 vau_reso_src1_crop;                  /* 498 */
    u_vau_reso_src2_crop                 vau_reso_src2_crop;                  /* 49c */
    u_vau_reso_des_crop                  vau_reso_des_crop;                   /* 4a0 */
    u_vau_reso_src2_fill_bdbx_start      vau_reso_src2_fill_bdbx_start;       /* 4a4 */
    u_vau_reso_src2_fill_fill_start      vau_reso_src2_fill_fill_start;       /* 4a8 */
    u_vau_reso_src2_fill_bdbx_end        vau_reso_src2_fill_bdbx_end;         /* 4ac */
    u_vau_reso_src2_fill_fill_end        vau_reso_src2_fill_fill_end;         /* 4b0 */
    u_vau_reso_cbm                       vau_reso_cbm;                        /* 4b4 */
    u_vau_reso_gauss                     vau_reso_gauss;                      /* 4b8 */
    u_vau_reso_patch                     vau_reso_patch;                      /* 4bc */
    u_vau_reso_patch_rect_xy             vau_reso_patch_rect_xy;              /* 4c0 */
    u_vau_reso_patch_rect_wh             vau_reso_patch_rect_wh;              /* 4c4 */
    unsigned int                         rsr7[14];                            /* 4c8 ~ 4fc */
    u_vau_start                          vau_start;                           /* 500 */
    u_vau_intstate                       vau_intstate;                        /* 504 */
    u_vau_intclr                         vau_intclr;                          /* 508 */
    u_vau_rawint                         vau_rawint;                          /* 50c */
    u_vau_pfcnt                          vau_pfcnt;                           /* 510 */
    u_vau_miscellaneous                  vau_miscellaneous;                   /* 514 */
    unsigned int                         rsr8;                                /* 518 */
    u_vau_timeout                        vau_timeout;                         /* 51c */
    u_vau_node_cnt                       vau_node_cnt;                        /* 520 */
    u_vau_memctrl_c28                    vau_memctrl_c28;                     /* 524 */
    u_vau_memctrl1_c28                   vau_memctrl1_c28;                    /* 528 */
    u_vau_soft_rst_state                 vau_soft_rst_state;                  /* 52c */
    u_vau_memctrl_c1x                    vau_memctrl_c1x;                     /* 530 */
    u_vau_memctrl1_c1x                   vau_memctrl1_c1x;                    /* 534 */
    u_offline_node_cfg                   offline_node_cfg;                    /* 538 */
    u_cbm_mask_00_0                      cbm_mask_00_0;                       /* 53c */
    u_cbm_mask_00_1                      cbm_mask_00_1;                       /* 540 */
    u_cbm_mask_00_2                      cbm_mask_00_2;                       /* 544 */
    u_cbm_mask_00_3                      cbm_mask_00_3;                       /* 548 */
    u_cbm_mask_00_4                      cbm_mask_00_4;                       /* 54c */
    u_cbm_mask_01_0                      cbm_mask_01_0;                       /* 550 */
    u_cbm_mask_01_1                      cbm_mask_01_1;                       /* 554 */
    u_cbm_mask_01_2                      cbm_mask_01_2;                       /* 558 */
    u_cbm_mask_01_3                      cbm_mask_01_3;                       /* 55c */
    u_cbm_mask_01_4                      cbm_mask_01_4;                       /* 560 */
    u_draw_mask_00_0                     draw_mask_00_0;                      /* 564 */
    u_draw_mask_00_1                     draw_mask_00_1;                      /* 568 */
    u_draw_mask_00_2                     draw_mask_00_2;                      /* 56c */
    u_draw_mask_00_3                     draw_mask_00_3;                      /* 570 */
    u_draw_mask_00_4                     draw_mask_00_4;                      /* 574 */
    u_draw_mask_01_0                     draw_mask_01_0;                      /* 578 */
    u_draw_mask_01_1                     draw_mask_01_1;                      /* 57c */
    u_draw_mask_01_2                     draw_mask_01_2;                      /* 580 */
    u_draw_mask_01_3                     draw_mask_01_3;                      /* 584 */
    u_draw_mask_01_4                     draw_mask_01_4;                      /* 588 */
    u_draw_mask_10_0                     draw_mask_10_0;                      /* 58c */
    u_draw_mask_10_1                     draw_mask_10_1;                      /* 590 */
    u_draw_mask_10_2                     draw_mask_10_2;                      /* 594 */
    u_draw_mask_10_3                     draw_mask_10_3;                      /* 598 */
    u_draw_mask_10_4                     draw_mask_10_4;                      /* 59c */
    unsigned int                         rsr9[24];                            /* 5a0 ~ 5fc */
    u_vau_mst_outstanding                vau_mst_outstanding;                 /* 600 */
    u_vau_mst_ctrl                       vau_mst_ctrl;                        /* 604 */
    unsigned int                         rsr10[2];                            /* 608 ~ 60c */
    u_vau_mst_rchn_prio                  vau_mst_rchn_prio;                   /* 610 */
    unsigned int                         rsr11;                               /* 614 */
    u_vau_mst_wchn_prio                  vau_mst_wchn_prio;                   /* 618 */
    unsigned int                         rsr12[9];                            /* 61c ~ 63c */
    u_vau_mst_bus_err_clr                vau_mst_bus_err_clr;                 /* 640 */
    u_vau_mst_bus_err                    vau_mst_bus_err;                     /* 644 */
    unsigned int                         rsr13[2];                            /* 648 ~ 64c */
    u_vau_mst_src0_status0               vau_mst_src0_status0;                /* 650 */
    u_vau_mst_src0_status1               vau_mst_src0_status1;                /* 654 */
    unsigned int                         rsr14[6];                            /* 658 ~ 66c */
    u_vau_mst_debug_ctrl                 vau_mst_debug_ctrl;                  /* 670 */
    u_vau_mst_debug_clr                  vau_mst_debug_clr;                   /* 674 */
    unsigned int                         rsr15[2];                            /* 678 ~ 67c */
    u_vau_mst_det_latency                vau_mst_det_latency;                 /* 680 */
    unsigned int                         rsr16[3];                            /* 684 ~ 68c */
    u_vau_mst_rd_info                    vau_mst_rd_info;                     /* 690 */
    u_vau_mst_wr_info                    vau_mst_wr_info;                     /* 694 */
    unsigned int                         rsr17[6];                            /* 698 ~ 6ac */
    u_vau_mst_det_latency0               vau_mst_det_latency0;                /* 6b0 */
    u_vau_mst_det_latency1               vau_mst_det_latency1;                /* 6b4 */
    u_vau_mst_det_latency2               vau_mst_det_latency2;                /* 6b8 */
    u_vau_mst_det_latency3               vau_mst_det_latency3;                /* 6bc */
    u_vau_mst_det_latency4               vau_mst_det_latency4;                /* 6c0 */
    u_vau_mst_det_latency5               vau_mst_det_latency5;                /* 6c4 */
    unsigned int                         rsr18[14];                           /* 6c8 ~ 6fc */
    u_vau_src_cmd_ctrl                   vau_src_cmd_ctrl;                    /* 700 */
    u_vau_des_cmd_ctrl                   vau_des_cmd_ctrl;                    /* 704 */
    u_vau_des_safe_dist                  vau_des_safe_dist;                   /* 708 */
    u_vau_des_safe_dist_inverse          vau_des_safe_dist_inverse;           /* 70c */
    u_vau_des_tunl_dist                  vau_des_tunl_dist;                   /* 710 */
    u_vau_cvfir_vcoef0                   vau_cvfir_vcoef0;                    /* 714 */
    u_vau_cvfir_vcoef1                   vau_cvfir_vcoef1;                    /* 718 */
    u_vau_cvfir_vcoef2                   vau_cvfir_vcoef2;                    /* 71c */
    u_vau_hfircoef01                     vau_hfircoef01;                      /* 720 */
    u_vau_hfircoef23                     vau_hfircoef23;                      /* 724 */
    u_vau_hfircoef45                     vau_hfircoef45;                      /* 728 */
    u_vau_hfircoef67                     vau_hfircoef67;                      /* 72c */
    u_vau_hipp_csc_coef00                vau_hipp_csc_coef00;                 /* 730 */
    u_vau_hipp_csc_coef01                vau_hipp_csc_coef01;                 /* 734 */
    u_vau_hipp_csc_coef02                vau_hipp_csc_coef02;                 /* 738 */
    u_vau_hipp_csc_coef10                vau_hipp_csc_coef10;                 /* 73c */
    u_vau_hipp_csc_coef11                vau_hipp_csc_coef11;                 /* 740 */
    u_vau_hipp_csc_coef12                vau_hipp_csc_coef12;                 /* 744 */
    u_vau_hipp_csc_coef20                vau_hipp_csc_coef20;                 /* 748 */
    u_vau_hipp_csc_coef21                vau_hipp_csc_coef21;                 /* 74c */
    u_vau_hipp_csc_coef22                vau_hipp_csc_coef22;                 /* 750 */
    u_vau_hipp_csc_scale                 vau_hipp_csc_scale;                  /* 754 */
    u_vau_hipp_csc_idc0                  vau_hipp_csc_idc0;                   /* 758 */
    u_vau_hipp_csc_idc1                  vau_hipp_csc_idc1;                   /* 75c */
    u_vau_hipp_csc_idc2                  vau_hipp_csc_idc2;                   /* 760 */
    u_vau_hipp_csc_odc0                  vau_hipp_csc_odc0;                   /* 764 */
    u_vau_hipp_csc_odc1                  vau_hipp_csc_odc1;                   /* 768 */
    u_vau_hipp_csc_odc2                  vau_hipp_csc_odc2;                   /* 76c */
    u_vau_hipp_csc_min_y                 vau_hipp_csc_min_y;                  /* 770 */
    u_vau_hipp_csc_min_c                 vau_hipp_csc_min_c;                  /* 774 */
    u_vau_hipp_csc_max_y                 vau_hipp_csc_max_y;                  /* 778 */
    u_vau_hipp_csc_max_c                 vau_hipp_csc_max_c;                  /* 77c */
    u_vau_hipp_csc2_coef00               vau_hipp_csc2_coef00;                /* 780 */
    u_vau_hipp_csc2_coef01               vau_hipp_csc2_coef01;                /* 784 */
    u_vau_hipp_csc2_coef02               vau_hipp_csc2_coef02;                /* 788 */
    u_vau_hipp_csc2_coef10               vau_hipp_csc2_coef10;                /* 78c */
    u_vau_hipp_csc2_coef11               vau_hipp_csc2_coef11;                /* 790 */
    u_vau_hipp_csc2_coef12               vau_hipp_csc2_coef12;                /* 794 */
    u_vau_hipp_csc2_coef20               vau_hipp_csc2_coef20;                /* 798 */
    u_vau_hipp_csc2_coef21               vau_hipp_csc2_coef21;                /* 79c */
    u_vau_hipp_csc2_coef22               vau_hipp_csc2_coef22;                /* 7a0 */
    u_vau_hipp_csc2_scale                vau_hipp_csc2_scale;                 /* 7a4 */
    u_vau_hipp_csc2_idc0                 vau_hipp_csc2_idc0;                  /* 7a8 */
    u_vau_hipp_csc2_idc1                 vau_hipp_csc2_idc1;                  /* 7ac */
    u_vau_hipp_csc2_idc2                 vau_hipp_csc2_idc2;                  /* 7b0 */
    u_vau_hipp_csc2_odc0                 vau_hipp_csc2_odc0;                  /* 7b4 */
    u_vau_hipp_csc2_odc1                 vau_hipp_csc2_odc1;                  /* 7b8 */
    u_vau_hipp_csc2_odc2                 vau_hipp_csc2_odc2;                  /* 7bc */
    u_vau_hipp_csc2_min_y                vau_hipp_csc2_min_y;                 /* 7c0 */
    u_vau_hipp_csc2_min_c                vau_hipp_csc2_min_c;                 /* 7c4 */
    u_vau_hipp_csc2_max_y                vau_hipp_csc2_max_y;                 /* 7c8 */
    u_vau_hipp_csc2_max_c                vau_hipp_csc2_max_c;                 /* 7cc */
    u_vau_hipp_csc_ink_ctrl              vau_hipp_csc_ink_ctrl;               /* 7d0 */
    u_vau_hipp_csc_ink_pos               vau_hipp_csc_ink_pos;                /* 7d4 */
    u_vau_hipp_csc_ink_data              vau_hipp_csc_ink_data;               /* 7d8 */
    u_vau_hipp_csc_ink_data2             vau_hipp_csc_ink_data2;              /* 7dc */
    u_vau_src1_htc_online_dcmp1          vau_src1_htc_online_dcmp1;           /* 7e0 */
    u_vau_src1_htc_online_dcmp2          vau_src1_htc_online_dcmp2;           /* 7e4 */
    u_vau_src2_htc_online_dcmp1          vau_src2_htc_online_dcmp1;           /* 7e8 */
    u_vau_src2_htc_online_dcmp2          vau_src2_htc_online_dcmp2;           /* 7ec */
    u_vau_des_htc_online_cmp1            vau_des_htc_online_cmp1;             /* 7f0 */
    u_vau_des_htc_online_cmp2            vau_des_htc_online_cmp2;             /* 7f4 */
    unsigned int                         rsr19;                               /* 7f8 */
    u_local_lighting_adjust_value        local_lighting_adjust_value;         /* 7fc */
    u_vau_matrix_debug0                  vau_matrix_debug0;                   /* 800 */
    unsigned int                         rsr20[63];                           /* 804 ~ 8fc */
    u_vau_t2r_err_clr                    vau_t2r_err_clr;                     /* 900 */
    u_vau_t2r_err_sta                    vau_t2r_err_sta;                     /* 904 */
    unsigned int                         rsr21[2];                            /* 908 ~ 90c */
    u_vau_t2r_debug1                     vau_t2r_debug1;                      /* 910 */
    u_vau_t2r_debug2                     vau_t2r_debug2;                      /* 914 */
    u_vau_t2r_debug3                     vau_t2r_debug3;                      /* 918 */
    u_vau_t2r_debug4                     vau_t2r_debug4;                      /* 91c */
    u_vau_t2r01_err_clr                  vau_t2r01_err_clr;                   /* 920 */
    u_vau_t2r01_err_sta                  vau_t2r01_err_sta;                   /* 924 */
    unsigned int                         rsr22[2];                            /* 928 ~ 92c */
    u_vau_t2r01_debug1                   vau_t2r01_debug1;                    /* 930 */
    u_vau_t2r01_debug2                   vau_t2r01_debug2;                    /* 934 */
    u_vau_t2r01_debug3                   vau_t2r01_debug3;                    /* 938 */
    u_vau_t2r01_debug4                   vau_t2r01_debug4;                    /* 93c */
    unsigned int                         rsr23[48];                           /* 940 ~ 9fc */
    u_vau_draw_sampleradius              vau_draw_sampleradius;               /* a00 */
    u_vau_draw_rndr_aa_smpl_x0           vau_draw_rndr_aa_smpl_x0;            /* a04 */
    u_vau_draw_rndr_aa_smpl_x1           vau_draw_rndr_aa_smpl_x1;            /* a08 */
    u_vau_draw_rndr_aa_smpl_x2           vau_draw_rndr_aa_smpl_x2;            /* a0c */
    u_vau_draw_rndr_aa_smpl_x3           vau_draw_rndr_aa_smpl_x3;            /* a10 */
    u_vau_draw_rndr_aa_smpl_x4           vau_draw_rndr_aa_smpl_x4;            /* a14 */
    u_vau_draw_rndr_aa_smpl_x5           vau_draw_rndr_aa_smpl_x5;            /* a18 */
    u_vau_draw_rndr_aa_smpl_x6           vau_draw_rndr_aa_smpl_x6;            /* a1c */
    u_vau_draw_rndr_aa_smpl_x7           vau_draw_rndr_aa_smpl_x7;            /* a20 */
    u_vau_draw_rndr_aa_smpl_y0           vau_draw_rndr_aa_smpl_y0;            /* a24 */
    u_vau_draw_rndr_aa_smpl_y1           vau_draw_rndr_aa_smpl_y1;            /* a28 */
    u_vau_draw_rndr_aa_smpl_y2           vau_draw_rndr_aa_smpl_y2;            /* a2c */
    u_vau_draw_rndr_aa_smpl_y3           vau_draw_rndr_aa_smpl_y3;            /* a30 */
    u_vau_draw_rndr_aa_smpl_y4           vau_draw_rndr_aa_smpl_y4;            /* a34 */
    u_vau_draw_rndr_aa_smpl_y5           vau_draw_rndr_aa_smpl_y5;            /* a38 */
    u_vau_draw_rndr_aa_smpl_y6           vau_draw_rndr_aa_smpl_y6;            /* a3c */
    u_vau_draw_rndr_aa_smpl_y7           vau_draw_rndr_aa_smpl_y7;            /* a40 */
    u_vau_draw_cvrg_weight0              vau_draw_cvrg_weight0;               /* a44 */
    u_vau_draw_cvrg_weight1              vau_draw_cvrg_weight1;               /* a48 */
    u_vau_draw_cmdbuf_start_addr         vau_draw_cmdbuf_start_addr;          /* a4c */
    u_vau_draw_cmdbuf_end_addr           vau_draw_cmdbuf_end_addr;            /* a50 */
    u_vau_draw_pathbuf_start_addr        vau_draw_pathbuf_start_addr;         /* a54 */
    u_vau_draw_pathbuf_end_addr          vau_draw_pathbuf_end_addr;           /* a58 */
    u_vau_draw_rnd_cap_tes_num           vau_draw_rnd_cap_tes_num;            /* a5c */
    u_vau_draw_tes_angle                 vau_draw_tes_angle;                  /* a60 */
    u_vau_draw_tes_sin                   vau_draw_tes_sin;                    /* a64 */
    u_vau_draw_tes_cos                   vau_draw_tes_cos;                    /* a68 */
    u_vau_draw_et_addr                   vau_draw_et_addr;                    /* a6c */
    u_vau_draw_et_bkstart_addr           vau_draw_et_bkstart_addr;            /* a70 */
    u_vau_draw_et_bkend_addr             vau_draw_et_bkend_addr;              /* a74 */
    u_vau_draw_link_start_addr           vau_draw_link_start_addr;            /* a78 */
    u_vau_draw_link_end_addr             vau_draw_link_end_addr;              /* a7c */
    u_vau_draw_bdbx_st                   vau_draw_bdbx_st;                    /* a80 */
    u_vau_draw_bdbx_end                  vau_draw_bdbx_end;                   /* a84 */
    unsigned int                         rsr24[30];                           /* a88 ~ afc */
    u_vau_sram_ctrl                      vau_sram_ctrl;                       /* b00 */
    unsigned int                         rsr25[63];                           /* b04 ~ bfc */
    u_hdr_vau_hdr_src1_ctrl              hdr_vau_hdr_src1_ctrl;               /* c00 */
    u_hdr_vau_hdr_src1_addr              hdr_vau_hdr_src1_addr;               /* c04 */
    u_hdr_vau_hdr_src1_stride            hdr_vau_hdr_src1_stride;             /* c08 */
    u_hdr_vau_hdr_src1c_addr             hdr_vau_hdr_src1c_addr;              /* c0c */
    u_hdr_vau_hdr_src1c_stride           hdr_vau_hdr_src1c_stride;            /* c10 */
    u_hdr_vau_hdr_src1_imgsize           hdr_vau_hdr_src1_imgsize;            /* c14 */
    u_hdr_vau_hdr_src1_rect_xy           hdr_vau_hdr_src1_rect_xy;            /* c18 */
    u_hdr_vau_hdr_src1_rect_wh           hdr_vau_hdr_src1_rect_wh;            /* c1c */
    u_hdr_vau_hdr_src1_fill              hdr_vau_hdr_src1_fill;               /* c20 */
    u_hdr_vau_hdr_src1_alpha             hdr_vau_hdr_src1_alpha;              /* c24 */
    u_hdr_vau_hdr_src1_pix_rgb           hdr_vau_hdr_src1_pix_rgb;            /* c28 */
    u_hdr_vau_hdr_src2_ctrl              hdr_vau_hdr_src2_ctrl;               /* c2c */
    u_hdr_vau_hdr_src2_addr              hdr_vau_hdr_src2_addr;               /* c30 */
    u_hdr_vau_hdr_src2_stride            hdr_vau_hdr_src2_stride;             /* c34 */
    u_hdr_vau_hdr_src2c_addr             hdr_vau_hdr_src2c_addr;              /* c38 */
    u_hdr_vau_hdr_src2c_stride           hdr_vau_hdr_src2c_stride;            /* c3c */
    u_hdr_vau_hdr_src2_imgsize           hdr_vau_hdr_src2_imgsize;            /* c40 */
    u_hdr_vau_hdr_src2_rect_xy           hdr_vau_hdr_src2_rect_xy;            /* c44 */
    u_hdr_vau_hdr_src2_rect_wh           hdr_vau_hdr_src2_rect_wh;            /* c48 */
    u_hdr_vau_hdr_src2_fill              hdr_vau_hdr_src2_fill;               /* c4c */
    u_hdr_vau_hdr_src2_alpha             hdr_vau_hdr_src2_alpha;              /* c50 */
    u_hdr_vau_hdr_src2_pix_rgb           hdr_vau_hdr_src2_pix_rgb;            /* c54 */
    u_hdr_vau_hdr_des_ctrl               hdr_vau_hdr_des_ctrl;                /* c58 */
    u_hdr_vau_hdr_des_addr               hdr_vau_hdr_des_addr;                /* c5c */
    u_hdr_vau_hdr_des_stride             hdr_vau_hdr_des_stride;              /* c60 */
    u_hdr_vau_hdr_des_imgsize            hdr_vau_hdr_des_imgsize;             /* c64 */
    u_hdr_vau_hdr_des_rect_xy            hdr_vau_hdr_des_rect_xy;             /* c68 */
    u_hdr_vau_hdr_des_rect_wh            hdr_vau_hdr_des_rect_wh;             /* c6c */
    u_hdr_vau_hdr_des_alpha              hdr_vau_hdr_des_alpha;               /* c70 */
    u_hdr_vau_cvfir_vinfo                hdr_vau_cvfir_vinfo;                 /* c74 */
    u_hdr_vau_cvfir_vsp                  hdr_vau_cvfir_vsp;                   /* c78 */
    u_hdr_vau_cvfir_voffset              hdr_vau_cvfir_voffset;               /* c7c */
    u_hdr_vau_cvfir_vboffset             hdr_vau_cvfir_vboffset;              /* c80 */
    u_hdr_vau_hfir_ctrl                  hdr_vau_hfir_ctrl;                   /* c84 */
    u_hdr_vau_hipp_csc_ctrl              hdr_vau_hipp_csc_ctrl;               /* c88 */
    u_hdr_vau_hipp_csc_coef00            hdr_vau_hipp_csc_coef00;             /* c8c */
    u_hdr_vau_hipp_csc_coef01            hdr_vau_hipp_csc_coef01;             /* c90 */
    u_hdr_vau_hipp_csc_coef02            hdr_vau_hipp_csc_coef02;             /* c94 */
    u_hdr_vau_hipp_csc_coef10            hdr_vau_hipp_csc_coef10;             /* c98 */
    u_hdr_vau_hipp_csc_coef11            hdr_vau_hipp_csc_coef11;             /* c9c */
    u_hdr_vau_hipp_csc_coef12            hdr_vau_hipp_csc_coef12;             /* ca0 */
    u_hdr_vau_hipp_csc_coef20            hdr_vau_hipp_csc_coef20;             /* ca4 */
    u_hdr_vau_hipp_csc_coef21            hdr_vau_hipp_csc_coef21;             /* ca8 */
    u_hdr_vau_hipp_csc_coef22            hdr_vau_hipp_csc_coef22;             /* cac */
    u_hdr_vau_hipp_csc_scale             hdr_vau_hipp_csc_scale;              /* cb0 */
    u_hdr_vau_hipp_csc_idc0              hdr_vau_hipp_csc_idc0;               /* cb4 */
    u_hdr_vau_hipp_csc_idc1              hdr_vau_hipp_csc_idc1;               /* cb8 */
    u_hdr_vau_hipp_csc_idc2              hdr_vau_hipp_csc_idc2;               /* cbc */
    u_hdr_vau_hipp_csc_odc0              hdr_vau_hipp_csc_odc0;               /* cc0 */
    u_hdr_vau_hipp_csc_odc1              hdr_vau_hipp_csc_odc1;               /* cc4 */
    u_hdr_vau_hipp_csc_odc2              hdr_vau_hipp_csc_odc2;               /* cc8 */
    u_hdr_vau_hipp_csc_min_y             hdr_vau_hipp_csc_min_y;              /* ccc */
    u_hdr_vau_hipp_csc_min_c             hdr_vau_hipp_csc_min_c;              /* cd0 */
    u_hdr_vau_hipp_csc_max_y             hdr_vau_hipp_csc_max_y;              /* cd4 */
    u_hdr_vau_hipp_csc_max_c             hdr_vau_hipp_csc_max_c;              /* cd8 */
    u_hdr_vau_hipp_csc2_coef00           hdr_vau_hipp_csc2_coef00;            /* cdc */
    u_hdr_vau_hipp_csc2_coef01           hdr_vau_hipp_csc2_coef01;            /* ce0 */
    u_hdr_vau_hipp_csc2_coef02           hdr_vau_hipp_csc2_coef02;            /* ce4 */
    u_hdr_vau_hipp_csc2_coef10           hdr_vau_hipp_csc2_coef10;            /* ce8 */
    u_hdr_vau_hipp_csc2_coef11           hdr_vau_hipp_csc2_coef11;            /* cec */
    u_hdr_vau_hipp_csc2_coef12           hdr_vau_hipp_csc2_coef12;            /* cf0 */
    u_hdr_vau_hipp_csc2_coef20           hdr_vau_hipp_csc2_coef20;            /* cf4 */
    u_hdr_vau_hipp_csc2_coef21           hdr_vau_hipp_csc2_coef21;            /* cf8 */
    u_hdr_vau_hipp_csc2_coef22           hdr_vau_hipp_csc2_coef22;            /* cfc */
    u_hdr_vau_hipp_csc2_scale            hdr_vau_hipp_csc2_scale;             /* d00 */
    u_hdr_vau_hipp_csc2_idc0             hdr_vau_hipp_csc2_idc0;              /* d04 */
    u_hdr_vau_hipp_csc2_idc1             hdr_vau_hipp_csc2_idc1;              /* d08 */
    u_hdr_vau_hipp_csc2_idc2             hdr_vau_hipp_csc2_idc2;              /* d0c */
    u_hdr_vau_hipp_csc2_odc0             hdr_vau_hipp_csc2_odc0;              /* d10 */
    u_hdr_vau_hipp_csc2_odc1             hdr_vau_hipp_csc2_odc1;              /* d14 */
    u_hdr_vau_hipp_csc2_odc2             hdr_vau_hipp_csc2_odc2;              /* d18 */
    u_hdr_vau_hipp_csc2_min_y            hdr_vau_hipp_csc2_min_y;             /* d1c */
    u_hdr_vau_hipp_csc2_min_c            hdr_vau_hipp_csc2_min_c;             /* d20 */
    u_hdr_vau_hipp_csc2_max_y            hdr_vau_hipp_csc2_max_y;             /* d24 */
    u_hdr_vau_hipp_csc2_max_c            hdr_vau_hipp_csc2_max_c;             /* d28 */
    unsigned int                         rsr26[2];                            /* d2c ~ d30 */
    u_hdr_vau_cvfir2_vinfo               hdr_vau_cvfir2_vinfo;                /* d34 */
    u_hdr_vau_cvfir2_vsp                 hdr_vau_cvfir2_vsp;                  /* d38 */
    u_hdr_vau_cvfir2_voffset             hdr_vau_cvfir2_voffset;              /* d3c */
    u_hdr_vau_cvfir2_vboffset            hdr_vau_cvfir2_vboffset;             /* d40 */
    u_hdr_vau_hfir2_ctrl                 hdr_vau_hfir2_ctrl;                  /* d44 */
    u_hdr_vdm_ctrl                       hdr_vdm_ctrl;                        /* d48 */
    u_hdr_vdm_para_degmm_addr            hdr_vdm_para_degmm_addr;             /* d4c */
    u_hdr_vdm_para_gmm_addr              hdr_vdm_para_gmm_addr;               /* d50 */
    u_hdr_vdm_para_tmap_addr             hdr_vdm_para_tmap_addr;              /* d54 */
    u_hdr_vdm_csc_ctrl                   hdr_vdm_csc_ctrl;                    /* d58 */
    u_hdr_vdm_csc_coef00                 hdr_vdm_csc_coef00;                  /* d5c */
    u_hdr_vdm_csc_coef01                 hdr_vdm_csc_coef01;                  /* d60 */
    u_hdr_vdm_csc_coef02                 hdr_vdm_csc_coef02;                  /* d64 */
    u_hdr_vdm_csc_coef10                 hdr_vdm_csc_coef10;                  /* d68 */
    u_hdr_vdm_csc_coef11                 hdr_vdm_csc_coef11;                  /* d6c */
    u_hdr_vdm_csc_coef12                 hdr_vdm_csc_coef12;                  /* d70 */
    u_hdr_vdm_csc_coef20                 hdr_vdm_csc_coef20;                  /* d74 */
    u_hdr_vdm_csc_coef21                 hdr_vdm_csc_coef21;                  /* d78 */
    u_hdr_vdm_csc_coef22                 hdr_vdm_csc_coef22;                  /* d7c */
    u_hdr_vdm_csc_scale                  hdr_vdm_csc_scale;                   /* d80 */
    u_hdr_vdm_csc_clip_min               hdr_vdm_csc_clip_min;                /* d84 */
    u_hdr_vdm_csc_clip_max               hdr_vdm_csc_clip_max;                /* d88 */
    u_hdr_vdm_csc_in_dc0                 hdr_vdm_csc_in_dc0;                  /* d8c */
    u_hdr_vdm_csc_in_dc1                 hdr_vdm_csc_in_dc1;                  /* d90 */
    u_hdr_vdm_csc_in_dc2                 hdr_vdm_csc_in_dc2;                  /* d94 */
    u_hdr_vdm_csc_out_dc0                hdr_vdm_csc_out_dc0;                 /* d98 */
    u_hdr_vdm_csc_out_dc1                hdr_vdm_csc_out_dc1;                 /* d9c */
    u_hdr_vdm_csc_out_dc2                hdr_vdm_csc_out_dc2;                 /* da0 */
    u_hdr_vdm_yuv2rgb_ctrl               hdr_vdm_yuv2rgb_ctrl;                /* da4 */
    u_hdr_vdm_yuv2rgb_coef00             hdr_vdm_yuv2rgb_coef00;              /* da8 */
    u_hdr_vdm_yuv2rgb_coef01             hdr_vdm_yuv2rgb_coef01;              /* dac */
    u_hdr_vdm_yuv2rgb_coef02             hdr_vdm_yuv2rgb_coef02;              /* db0 */
    u_hdr_vdm_yuv2rgb_coef03             hdr_vdm_yuv2rgb_coef03;              /* db4 */
    u_hdr_vdm_yuv2rgb_coef04             hdr_vdm_yuv2rgb_coef04;              /* db8 */
    u_hdr_vdm_yuv2rgb_coef05             hdr_vdm_yuv2rgb_coef05;              /* dbc */
    u_hdr_vdm_yuv2rgb_coef06             hdr_vdm_yuv2rgb_coef06;              /* dc0 */
    u_hdr_vdm_yuv2rgb_coef07             hdr_vdm_yuv2rgb_coef07;              /* dc4 */
    u_hdr_vdm_yuv2rgb_coef08             hdr_vdm_yuv2rgb_coef08;              /* dc8 */
    u_hdr_vdm_yuv2rgb_coef10             hdr_vdm_yuv2rgb_coef10;              /* dcc */
    u_hdr_vdm_yuv2rgb_coef11             hdr_vdm_yuv2rgb_coef11;              /* dd0 */
    u_hdr_vdm_yuv2rgb_coef12             hdr_vdm_yuv2rgb_coef12;              /* dd4 */
    u_hdr_vdm_yuv2rgb_coef13             hdr_vdm_yuv2rgb_coef13;              /* dd8 */
    u_hdr_vdm_yuv2rgb_coef14             hdr_vdm_yuv2rgb_coef14;              /* ddc */
    u_hdr_vdm_yuv2rgb_coef15             hdr_vdm_yuv2rgb_coef15;              /* de0 */
    u_hdr_vdm_yuv2rgb_coef16             hdr_vdm_yuv2rgb_coef16;              /* de4 */
    u_hdr_vdm_yuv2rgb_coef17             hdr_vdm_yuv2rgb_coef17;              /* de8 */
    u_hdr_vdm_yuv2rgb_coef18             hdr_vdm_yuv2rgb_coef18;              /* dec */
    u_hdr_vdm_yuv2rgb_scale2p            hdr_vdm_yuv2rgb_scale2p;             /* df0 */
    u_hdr_vdm_yuv2rgb_in_dc00            hdr_vdm_yuv2rgb_in_dc00;             /* df4 */
    u_hdr_vdm_yuv2rgb_in_dc01            hdr_vdm_yuv2rgb_in_dc01;             /* df8 */
    u_hdr_vdm_yuv2rgb_in_dc02            hdr_vdm_yuv2rgb_in_dc02;             /* dfc */
    u_hdr_vdm_yuv2rgb_out_dc00           hdr_vdm_yuv2rgb_out_dc00;            /* e00 */
    u_hdr_vdm_yuv2rgb_out_dc01           hdr_vdm_yuv2rgb_out_dc01;            /* e04 */
    u_hdr_vdm_yuv2rgb_out_dc02           hdr_vdm_yuv2rgb_out_dc02;            /* e08 */
    u_hdr_vdm_yuv2rgb_in_dc10            hdr_vdm_yuv2rgb_in_dc10;             /* e0c */
    u_hdr_vdm_yuv2rgb_in_dc11            hdr_vdm_yuv2rgb_in_dc11;             /* e10 */
    u_hdr_vdm_yuv2rgb_in_dc12            hdr_vdm_yuv2rgb_in_dc12;             /* e14 */
    u_hdr_vdm_yuv2rgb_out_dc10           hdr_vdm_yuv2rgb_out_dc10;            /* e18 */
    u_hdr_vdm_yuv2rgb_out_dc11           hdr_vdm_yuv2rgb_out_dc11;            /* e1c */
    u_hdr_vdm_yuv2rgb_out_dc12           hdr_vdm_yuv2rgb_out_dc12;            /* e20 */
    u_hdr_vdm_yuv2rgb_min                hdr_vdm_yuv2rgb_min;                 /* e24 */
    u_hdr_vdm_yuv2rgb_max                hdr_vdm_yuv2rgb_max;                 /* e28 */
    u_hdr_disp_0_dither_ctrl             hdr_disp_0_dither_ctrl;              /* e2c */
    u_hdr_disp_0_dither_ink_ctrl         hdr_disp_0_dither_ink_ctrl;          /* e30 */
    u_hdr_disp_0_dither_ink_pos          hdr_disp_0_dither_ink_pos;           /* e34 */
    u_hdr_local_lighting_cfg             hdr_local_lighting_cfg;              /* e38 */
    u_hdr_local_lighting_cx              hdr_local_lighting_cx;               /* e3c */
    u_hdr_local_lighting_cy              hdr_local_lighting_cy;               /* e40 */
    u_hdr_local_lighting_radius2         hdr_local_lighting_radius2;          /* e44 */
    u_hdr_local_lighting_amplitude       hdr_local_lighting_amplitude;        /* e48 */
    u_hdr_local_lighting_ampli_scale     hdr_local_lighting_ampli_scale;      /* e4c */
    u_hdr_local_lighting_start_angle_rad hdr_local_lighting_start_angle_rad;  /* e50 */
    u_hdr_local_lighting_start_end_angle_rad hdr_local_lighting_start_end_angle_rad; /* e54 */
    u_hdr_local_lighting_end_start_angle_rad hdr_local_lighting_end_start_angle_rad; /* e58 */
    u_hdr_local_lighting_end_angle_rad   hdr_local_lighting_end_angle_rad;    /* e5c */
    u_hdr_local_lighting_triwin2_coeff0  hdr_local_lighting_triwin2_coeff0;   /* e60 */
    u_hdr_local_lighting_triwin2_coeff1  hdr_local_lighting_triwin2_coeff1;   /* e64 */
    u_hdr_local_lighting_amb_amp         hdr_local_lighting_amb_amp;          /* e68 */
    u_hdr_local_lighting_bbox0_p0        hdr_local_lighting_bbox0_p0;         /* e6c */
    u_hdr_local_lighting_bbox0_p1        hdr_local_lighting_bbox0_p1;         /* e70 */
    u_hdr_local_lighting_bbox1_p0        hdr_local_lighting_bbox1_p0;         /* e74 */
    u_hdr_local_lighting_bbox1_p1        hdr_local_lighting_bbox1_p1;         /* e78 */
    u_hdr_local_lighting_img_reso        hdr_local_lighting_img_reso;         /* e7c */
    u_hdr_local_lighting_start_pos       hdr_local_lighting_start_pos;        /* e80 */
    u_hdr_local_lighting_adjust_value    hdr_local_lighting_adjust_value;     /* e84 */
    unsigned int                         rsr27[2];                            /* e88 ~ e8c */
    u_hdr_rcbn_ctrl                      hdr_rcbn_ctrl;                       /* e90 */
    u_hdr_rcbn_gmap_size_mode            hdr_rcbn_gmap_size_mode;             /* e94 */
    u_hdr_rcbn_gmap_chn_num              hdr_rcbn_gmap_chn_num;               /* e98 */
    u_hdr_rcbn_gcoeff_rmin1              hdr_rcbn_gcoeff_rmin1;               /* e9c */
    u_hdr_rcbn_gcoeff_rmin2              hdr_rcbn_gcoeff_rmin2;               /* ea0 */
    u_hdr_rcbn_gcoeff_rmax               hdr_rcbn_gcoeff_rmax;                /* ea4 */
    u_hdr_rcbn_gcoeff_gmin1              hdr_rcbn_gcoeff_gmin1;               /* ea8 */
    u_hdr_rcbn_gcoeff_gmin2              hdr_rcbn_gcoeff_gmin2;               /* eac */
    u_hdr_rcbn_gcoeff_gmax               hdr_rcbn_gcoeff_gmax;                /* eb0 */
    u_hdr_rcbn_gcoeff_bmin1              hdr_rcbn_gcoeff_bmin1;               /* eb4 */
    u_hdr_rcbn_gcoeff_bmin2              hdr_rcbn_gcoeff_bmin2;               /* eb8 */
    u_hdr_rcbn_gcoeff_bmax               hdr_rcbn_gcoeff_bmax;                /* ebc */
    u_hdr_rcbn_kcoeff_r_alte             hdr_rcbn_kcoeff_r_alte;              /* ec0 */
    u_hdr_rcbn_kcoeff_r_base             hdr_rcbn_kcoeff_r_base;              /* ec4 */
    u_hdr_rcbn_kcoeff_g_alte             hdr_rcbn_kcoeff_g_alte;              /* ec8 */
    u_hdr_rcbn_kcoeff_g_base             hdr_rcbn_kcoeff_g_base;              /* ecc */
    u_hdr_rcbn_kcoeff_b_alte             hdr_rcbn_kcoeff_b_alte;              /* ed0 */
    u_hdr_rcbn_kcoeff_b_base             hdr_rcbn_kcoeff_b_base;              /* ed4 */
    u_hdr_rcbn_weight                    hdr_rcbn_weight;                     /* ed8 */
    u_hdr_rcbn_clip_min                  hdr_rcbn_clip_min;                   /* edc */
    u_hdr_rcbn_clip_max                  hdr_rcbn_clip_max;                   /* ee0 */
    u_hdr_hipp_ctadj_ctrl                hdr_hipp_ctadj_ctrl;                 /* ee4 */
    u_hdr_hipp_ctadj_gain                hdr_hipp_ctadj_gain;                 /* ee8 */
    u_hdr_hipp_ctadj_offset              hdr_hipp_ctadj_offset;               /* eec */
    u_hdr_hipp_ctadj_offset_b            hdr_hipp_ctadj_offset_b;             /* ef0 */
    u_hdr_hipp_ctadj_min                 hdr_hipp_ctadj_min;                  /* ef4 */
    u_hdr_hipp_ctadj_max                 hdr_hipp_ctadj_max;                  /* ef8 */
    u_hdr_hipp_ctadj_gain_w              hdr_hipp_ctadj_gain_w;               /* efc */
    u_hdr_hipp_ctadj_offset_w            hdr_hipp_ctadj_offset_w;             /* f00 */
    u_hdr_hipp_ctadj_min_w               hdr_hipp_ctadj_min_w;                /* f04 */
    u_hdr_hipp_ctadj_max_w               hdr_hipp_ctadj_max_w;                /* f08 */
    u_hdr_disp_1_dither_ctrl             hdr_disp_1_dither_ctrl;              /* f0c */
    u_hdr_disp_1_dither_ink_ctrl         hdr_disp_1_dither_ink_ctrl;          /* f10 */
    u_hdr_disp_1_dither_ink_pos          hdr_disp_1_dither_ink_pos;           /* f14 */
    unsigned int                         rsr28[2];                            /* f18 ~ f1c */
    u_hdr_cbm_bkg1                       hdr_cbm_bkg1;                        /* f20 */
    u_hdr_cbm_bkalpha                    hdr_cbm_bkalpha;                     /* f24 */
    u_hdr_cbm_mix1                       hdr_cbm_mix1;                        /* f28 */
    u_hdr_cbm_mix_bypass                 hdr_cbm_mix_bypass;                  /* f2c */
    u_hdr_cbm_bkg2                       hdr_cbm_bkg2;                        /* f30 */
    unsigned int                         rsr29;                               /* f34 */
    u_hdr_cbm_mix2                       hdr_cbm_mix2;                        /* f38 */
    unsigned int                         rsr30[2];                            /* f3c ~ f40 */
    u_hdr_hc_bmp_thd                     hdr_hc_bmp_thd;                      /* f44 */
    u_hdr_vhdaccstatctrl                 hdr_vhdaccstatctrl;                  /* f48 */
    u_hdr_vhdaccstathpos                 hdr_vhdaccstathpos;                  /* f4c */
    u_hdr_vhdaccstatvpos                 hdr_vhdaccstatvpos;                  /* f50 */
    u_hdr_vhdaccshiftd                   hdr_vhdaccshiftd;                    /* f54 */
    u_hdr_vhdaccdnsam                    hdr_vhdaccdnsam;                     /* f58 */
    u_hdr_vhdaccsatcfg                   hdr_vhdaccsatcfg;                    /* f5c */
    u_hdr_vdm_csc1_ctrl                  hdr_vdm_csc1_ctrl;                   /* f60 */
    u_hdr_vdm_csc1_coef00                hdr_vdm_csc1_coef00;                 /* f64 */
    u_hdr_vdm_csc1_coef01                hdr_vdm_csc1_coef01;                 /* f68 */
    u_hdr_vdm_csc1_coef02                hdr_vdm_csc1_coef02;                 /* f6c */
    u_hdr_vdm_csc1_coef10                hdr_vdm_csc1_coef10;                 /* f70 */
    u_hdr_vdm_csc1_coef11                hdr_vdm_csc1_coef11;                 /* f74 */
    u_hdr_vdm_csc1_coef12                hdr_vdm_csc1_coef12;                 /* f78 */
    u_hdr_vdm_csc1_coef20                hdr_vdm_csc1_coef20;                 /* f7c */
    u_hdr_vdm_csc1_coef21                hdr_vdm_csc1_coef21;                 /* f80 */
    u_hdr_vdm_csc1_coef22                hdr_vdm_csc1_coef22;                 /* f84 */
    u_hdr_vdm_csc1_scale                 hdr_vdm_csc1_scale;                  /* f88 */
    u_hdr_vdm_csc1_clip_min              hdr_vdm_csc1_clip_min;               /* f8c */
    u_hdr_vdm_csc1_clip_max              hdr_vdm_csc1_clip_max;               /* f90 */
    u_hdr_vdm_csc1_in_dc0                hdr_vdm_csc1_in_dc0;                 /* f94 */
    u_hdr_vdm_csc1_in_dc1                hdr_vdm_csc1_in_dc1;                 /* f98 */
    u_hdr_vdm_csc1_in_dc2                hdr_vdm_csc1_in_dc2;                 /* f9c */
    u_hdr_vdm_csc1_out_dc0               hdr_vdm_csc1_out_dc0;                /* fa0 */
    u_hdr_vdm_csc1_out_dc1               hdr_vdm_csc1_out_dc1;                /* fa4 */
    u_hdr_vdm_csc1_out_dc2               hdr_vdm_csc1_out_dc2;                /* fa8 */
    u_hdr_vdm_tmap_luma_coef0            hdr_vdm_tmap_luma_coef0;             /* fac */
    u_hdr_vdm_tmap_luma_coef1            hdr_vdm_tmap_luma_coef1;             /* fb0 */
    u_hdr_vdm_tmap_luma_coef2            hdr_vdm_tmap_luma_coef2;             /* fb4 */
    u_hdr_vdm_tmap_luma_scale            hdr_vdm_tmap_luma_scale;             /* fb8 */
    u_hdr_vdm_tmap_coef_scale            hdr_vdm_tmap_coef_scale;             /* fbc */
    u_hdr_vdm_tmap_out_clip_min          hdr_vdm_tmap_out_clip_min;           /* fc0 */
    u_hdr_vdm_tmap_out_clip_max          hdr_vdm_tmap_out_clip_max;           /* fc4 */
    u_hdr_vdm_tmap_out_dc0               hdr_vdm_tmap_out_dc0;                /* fc8 */
    u_hdr_vdm_tmap_out_dc1               hdr_vdm_tmap_out_dc1;                /* fcc */
    u_hdr_vdm_tmap_out_dc2               hdr_vdm_tmap_out_dc2;                /* fd0 */
    u_hdr_vdm_tonemap_alpha              hdr_vdm_tonemap_alpha;               /* fd4 */
    unsigned int                         rsr31[10];                           /* fd8 ~ ffc */
    u_hdr_vau_cvfir_vcoef0               hdr_vau_cvfir_vcoef0;                /* 1000 */
    u_hdr_vau_cvfir_vcoef1               hdr_vau_cvfir_vcoef1;                /* 1004 */
    u_hdr_vau_cvfir_vcoef2               hdr_vau_cvfir_vcoef2;                /* 1008 */
    u_hdr_vau_hfircoef01                 hdr_vau_hfircoef01;                  /* 100c */
    u_hdr_vau_hfircoef23                 hdr_vau_hfircoef23;                  /* 1010 */
    u_hdr_vau_hfircoef45                 hdr_vau_hfircoef45;                  /* 1014 */
    u_hdr_vau_hfircoef67                 hdr_vau_hfircoef67;                  /* 1018 */
    u_hdr_vau_hipp_csc_ink_ctrl          hdr_vau_hipp_csc_ink_ctrl;           /* 101c */
    u_hdr_vau_hipp_csc_ink_pos           hdr_vau_hipp_csc_ink_pos;            /* 1020 */
    u_hdr_vau_hipp_csc_ink_data          hdr_vau_hipp_csc_ink_data;           /* 1024 */
    u_hdr_vau_hipp_csc_ink_data2         hdr_vau_hipp_csc_ink_data2;          /* 1028 */
    u_hdr_vau_cvfir2_vcoef0              hdr_vau_cvfir2_vcoef0;               /* 102c */
    u_hdr_vau_cvfir2_vcoef1              hdr_vau_cvfir2_vcoef1;               /* 1030 */
    u_hdr_vau_cvfir2_vcoef2              hdr_vau_cvfir2_vcoef2;               /* 1034 */
    u_hdr_vau_hfir2coef01                hdr_vau_hfir2coef01;                 /* 1038 */
    u_hdr_vau_hfir2coef23                hdr_vau_hfir2coef23;                 /* 103c */
    u_hdr_vau_hfir2coef45                hdr_vau_hfir2coef45;                 /* 1040 */
    u_hdr_vau_hfir2coef67                hdr_vau_hfir2coef67;                 /* 1044 */
    unsigned int                         rsr32[10];                           /* 1048 ~ 106c */
    u_hdr_vdm_degmm_ren                  hdr_vdm_degmm_ren;                   /* 1070 */
    u_hdr_vdm_degmm_data                 hdr_vdm_degmm_data;                  /* 1074 */
    u_hdr_vdm_degamma_step               hdr_vdm_degamma_step;                /* 1078 */
    u_hdr_vdm_degamma_step2              hdr_vdm_degamma_step2;               /* 107c */
    u_hdr_vdm_degamma_pos0               hdr_vdm_degamma_pos0;                /* 1080 */
    u_hdr_vdm_degamma_pos1               hdr_vdm_degamma_pos1;                /* 1084 */
    u_hdr_vdm_degamma_pos2               hdr_vdm_degamma_pos2;                /* 1088 */
    u_hdr_vdm_degamma_pos3               hdr_vdm_degamma_pos3;                /* 108c */
    u_hdr_vdm_degamma_num                hdr_vdm_degamma_num;                 /* 1090 */
    u_hdr_vdm_degamma_num2               hdr_vdm_degamma_num2;                /* 1094 */
    u_hdr_vdm_tonemap_ren                hdr_vdm_tonemap_ren;                 /* 1098 */
    u_hdr_vdm_tonemap_data               hdr_vdm_tonemap_data;                /* 109c */
    u_hdr_vdm_tonemap_sm_data            hdr_vdm_tonemap_sm_data;             /* 10a0 */
    u_hdr_vdm_tonemap_step               hdr_vdm_tonemap_step;                /* 10a4 */
    u_hdr_vdm_tonemap_pos0               hdr_vdm_tonemap_pos0;                /* 10a8 */
    u_hdr_vdm_tonemap_pos1               hdr_vdm_tonemap_pos1;                /* 10ac */
    u_hdr_vdm_tonemap_num                hdr_vdm_tonemap_num;                 /* 10b0 */
    u_hdr_vdm_tonemap_pos2               hdr_vdm_tonemap_pos2;                /* 10b4 */
    u_hdr_vdm_tonemap_pos3               hdr_vdm_tonemap_pos3;                /* 10b8 */
    u_hdr_vdm_tonemap_step2              hdr_vdm_tonemap_step2;               /* 10bc */
    u_hdr_vdm_tonemap_pos4               hdr_vdm_tonemap_pos4;                /* 10c0 */
    u_hdr_vdm_tonemap_pos5               hdr_vdm_tonemap_pos5;                /* 10c4 */
    u_hdr_vdm_tonemap_num2               hdr_vdm_tonemap_num2;                /* 10c8 */
    u_hdr_vdm_tonemap_pos6               hdr_vdm_tonemap_pos6;                /* 10cc */
    u_hdr_vdm_tonemap_pos7               hdr_vdm_tonemap_pos7;                /* 10d0 */
    unsigned int                         rsr33;                               /* 10d4 */
    u_hdr_vdm_gamma_ren                  hdr_vdm_gamma_ren;                   /* 10d8 */
    u_hdr_vdm_gammma_data                hdr_vdm_gammma_data;                 /* 10dc */
    u_hdr_vdm_gamma_step                 hdr_vdm_gamma_step;                  /* 10e0 */
    u_hdr_vdm_gamma_pos0                 hdr_vdm_gamma_pos0;                  /* 10e4 */
    u_hdr_vdm_gamma_pos1                 hdr_vdm_gamma_pos1;                  /* 10e8 */
    u_hdr_vdm_gamma_num                  hdr_vdm_gamma_num;                   /* 10ec */
    u_hdr_vdm_gamma_pos2                 hdr_vdm_gamma_pos2;                  /* 10f0 */
    u_hdr_vdm_gamma_pos3                 hdr_vdm_gamma_pos3;                  /* 10f4 */
    u_hdr_vdm_gamma_step2                hdr_vdm_gamma_step2;                 /* 10f8 */
    u_hdr_vdm_gamma_num2                 hdr_vdm_gamma_num2;                  /* 10fc */
    u_hdr_vdm_gamma_pos4                 hdr_vdm_gamma_pos4;                  /* 1100 */
    u_hdr_vdm_gamma_pos5                 hdr_vdm_gamma_pos5;                  /* 1104 */
    u_hdr_vdm_gamma_pos6                 hdr_vdm_gamma_pos6;                  /* 1108 */
    u_hdr_vdm_gamma_pos7                 hdr_vdm_gamma_pos7;                  /* 110c */
    u_hdr_disp_0_dither_thr              hdr_disp_0_dither_thr;               /* 1110 */
    u_hdr_disp_0_dither_sed_y0           hdr_disp_0_dither_sed_y0;            /* 1114 */
    u_hdr_disp_0_dither_sed_u0           hdr_disp_0_dither_sed_u0;            /* 1118 */
    u_hdr_disp_0_dither_sed_v0           hdr_disp_0_dither_sed_v0;            /* 111c */
    u_hdr_disp_0_dither_sed_w0           hdr_disp_0_dither_sed_w0;            /* 1120 */
    u_hdr_disp_0_dither_sed_y1           hdr_disp_0_dither_sed_y1;            /* 1124 */
    u_hdr_disp_0_dither_sed_u1           hdr_disp_0_dither_sed_u1;            /* 1128 */
    u_hdr_disp_0_dither_sed_v1           hdr_disp_0_dither_sed_v1;            /* 112c */
    u_hdr_disp_0_dither_sed_w1           hdr_disp_0_dither_sed_w1;            /* 1130 */
    u_hdr_disp_0_dither_sed_y2           hdr_disp_0_dither_sed_y2;            /* 1134 */
    u_hdr_disp_0_dither_sed_u2           hdr_disp_0_dither_sed_u2;            /* 1138 */
    u_hdr_disp_0_dither_sed_v2           hdr_disp_0_dither_sed_v2;            /* 113c */
    u_hdr_disp_0_dither_sed_w2           hdr_disp_0_dither_sed_w2;            /* 1140 */
    u_hdr_disp_0_dither_sed_y3           hdr_disp_0_dither_sed_y3;            /* 1144 */
    u_hdr_disp_0_dither_sed_u3           hdr_disp_0_dither_sed_u3;            /* 1148 */
    u_hdr_disp_0_dither_sed_v3           hdr_disp_0_dither_sed_v3;            /* 114c */
    u_hdr_disp_0_dither_sed_w3           hdr_disp_0_dither_sed_w3;            /* 1150 */
    u_hdr_disp_0_dither_sed_y4           hdr_disp_0_dither_sed_y4;            /* 1154 */
    u_hdr_disp_0_dither_sed_u4           hdr_disp_0_dither_sed_u4;            /* 1158 */
    u_hdr_disp_0_dither_sed_v4           hdr_disp_0_dither_sed_v4;            /* 115c */
    u_hdr_disp_0_dither_sed_w4           hdr_disp_0_dither_sed_w4;            /* 1160 */
    u_hdr_disp_0_dither_sed_y5           hdr_disp_0_dither_sed_y5;            /* 1164 */
    u_hdr_disp_0_dither_sed_u5           hdr_disp_0_dither_sed_u5;            /* 1168 */
    u_hdr_disp_0_dither_sed_v5           hdr_disp_0_dither_sed_v5;            /* 116c */
    u_hdr_disp_0_dither_sed_w5           hdr_disp_0_dither_sed_w5;            /* 1170 */
    u_hdr_disp_0_dither_sed_y6           hdr_disp_0_dither_sed_y6;            /* 1174 */
    u_hdr_disp_0_dither_sed_u6           hdr_disp_0_dither_sed_u6;            /* 1178 */
    u_hdr_disp_0_dither_sed_v6           hdr_disp_0_dither_sed_v6;            /* 117c */
    u_hdr_disp_0_dither_sed_w6           hdr_disp_0_dither_sed_w6;            /* 1180 */
    u_hdr_disp_0_dither_sed_y7           hdr_disp_0_dither_sed_y7;            /* 1184 */
    u_hdr_disp_0_dither_sed_u7           hdr_disp_0_dither_sed_u7;            /* 1188 */
    u_hdr_disp_0_dither_sed_v7           hdr_disp_0_dither_sed_v7;            /* 118c */
    u_hdr_disp_0_dither_sed_w7           hdr_disp_0_dither_sed_w7;            /* 1190 */
    u_hdr_disp_0_dither_tapmode          hdr_disp_0_dither_tapmode;           /* 1194 */
    u_hdr_disp_0_dither_coef_rd          hdr_disp_0_dither_coef_rd;           /* 1198 */
    u_hdr_disp_0_dither_coef_rdata       hdr_disp_0_dither_coef_rdata;        /* 119c */
    unsigned int                         rsr34[2];                            /* 11a0 ~ 11a4 */
    u_hdr_disp_0_dither_ink_data         hdr_disp_0_dither_ink_data;          /* 11a8 */
    u_hdr_disp_0_dither_ink_data2        hdr_disp_0_dither_ink_data2;         /* 11ac */
    u_hdr_disp_0_dither_ink_iy           hdr_disp_0_dither_ink_iy;            /* 11b0 */
    u_hdr_disp_0_dither_ink_ic           hdr_disp_0_dither_ink_ic;            /* 11b4 */
    u_hdr_disp_0_dither_ink_oy           hdr_disp_0_dither_ink_oy;            /* 11b8 */
    u_hdr_disp_0_dither_ink_oc           hdr_disp_0_dither_ink_oc;            /* 11bc */
    u_hdr_rcbn_dfx                       hdr_rcbn_dfx;                        /* 11c0 */
    u_hdr_disp_1_dither_thr              hdr_disp_1_dither_thr;               /* 11c4 */
    u_hdr_disp_1_dither_sed_y0           hdr_disp_1_dither_sed_y0;            /* 11c8 */
    u_hdr_disp_1_dither_sed_u0           hdr_disp_1_dither_sed_u0;            /* 11cc */
    u_hdr_disp_1_dither_sed_v0           hdr_disp_1_dither_sed_v0;            /* 11d0 */
    u_hdr_disp_1_dither_sed_w0           hdr_disp_1_dither_sed_w0;            /* 11d4 */
    u_hdr_disp_1_dither_sed_y1           hdr_disp_1_dither_sed_y1;            /* 11d8 */
    u_hdr_disp_1_dither_sed_u1           hdr_disp_1_dither_sed_u1;            /* 11dc */
    u_hdr_disp_1_dither_sed_v1           hdr_disp_1_dither_sed_v1;            /* 11e0 */
    u_hdr_disp_1_dither_sed_w1           hdr_disp_1_dither_sed_w1;            /* 11e4 */
    u_hdr_disp_1_dither_sed_y2           hdr_disp_1_dither_sed_y2;            /* 11e8 */
    u_hdr_disp_1_dither_sed_u2           hdr_disp_1_dither_sed_u2;            /* 11ec */
    u_hdr_disp_1_dither_sed_v2           hdr_disp_1_dither_sed_v2;            /* 11f0 */
    u_hdr_disp_1_dither_sed_w2           hdr_disp_1_dither_sed_w2;            /* 11f4 */
    u_hdr_disp_1_dither_sed_y3           hdr_disp_1_dither_sed_y3;            /* 11f8 */
    u_hdr_disp_1_dither_sed_u3           hdr_disp_1_dither_sed_u3;            /* 11fc */
    u_hdr_disp_1_dither_sed_v3           hdr_disp_1_dither_sed_v3;            /* 1200 */
    u_hdr_disp_1_dither_sed_w3           hdr_disp_1_dither_sed_w3;            /* 1204 */
    u_hdr_disp_1_dither_sed_y4           hdr_disp_1_dither_sed_y4;            /* 1208 */
    u_hdr_disp_1_dither_sed_u4           hdr_disp_1_dither_sed_u4;            /* 120c */
    u_hdr_disp_1_dither_sed_v4           hdr_disp_1_dither_sed_v4;            /* 1210 */
    u_hdr_disp_1_dither_sed_w4           hdr_disp_1_dither_sed_w4;            /* 1214 */
    u_hdr_disp_1_dither_sed_y5           hdr_disp_1_dither_sed_y5;            /* 1218 */
    u_hdr_disp_1_dither_sed_u5           hdr_disp_1_dither_sed_u5;            /* 121c */
    u_hdr_disp_1_dither_sed_v5           hdr_disp_1_dither_sed_v5;            /* 1220 */
    u_hdr_disp_1_dither_sed_w5           hdr_disp_1_dither_sed_w5;            /* 1224 */
    u_hdr_disp_1_dither_sed_y6           hdr_disp_1_dither_sed_y6;            /* 1228 */
    u_hdr_disp_1_dither_sed_u6           hdr_disp_1_dither_sed_u6;            /* 122c */
    u_hdr_disp_1_dither_sed_v6           hdr_disp_1_dither_sed_v6;            /* 1230 */
    u_hdr_disp_1_dither_sed_w6           hdr_disp_1_dither_sed_w6;            /* 1234 */
    u_hdr_disp_1_dither_sed_y7           hdr_disp_1_dither_sed_y7;            /* 1238 */
    u_hdr_disp_1_dither_sed_u7           hdr_disp_1_dither_sed_u7;            /* 123c */
    u_hdr_disp_1_dither_sed_v7           hdr_disp_1_dither_sed_v7;            /* 1240 */
    u_hdr_disp_1_dither_sed_w7           hdr_disp_1_dither_sed_w7;            /* 1244 */
    u_hdr_disp_1_dither_tapmode          hdr_disp_1_dither_tapmode;           /* 1248 */
    u_hdr_disp_1_dither_coef_rd          hdr_disp_1_dither_coef_rd;           /* 124c */
    u_hdr_disp_1_dither_coef_rdata       hdr_disp_1_dither_coef_rdata;        /* 1250 */
    unsigned int                         rsr35[2];                            /* 1254 ~ 1258 */
    u_hdr_disp_1_dither_ink_data         hdr_disp_1_dither_ink_data;          /* 125c */
    u_hdr_disp_1_dither_ink_data2        hdr_disp_1_dither_ink_data2;         /* 1260 */
    u_hdr_disp_1_dither_ink_iy           hdr_disp_1_dither_ink_iy;            /* 1264 */
    u_hdr_disp_1_dither_ink_ic           hdr_disp_1_dither_ink_ic;            /* 1268 */
    u_hdr_disp_1_dither_ink_oy           hdr_disp_1_dither_ink_oy;            /* 126c */
    u_hdr_disp_1_dither_ink_oc           hdr_disp_1_dither_ink_oc;            /* 1270 */
    u_hdr_cbm_lay0_debug                 hdr_cbm_lay0_debug;                  /* 1274 */
    u_hdr_cbm_lay1_debug                 hdr_cbm_lay1_debug;                  /* 1278 */
    u_hdr_cbm_lay2_debug                 hdr_cbm_lay2_debug;                  /* 127c */
    u_hdr_cbm_lay3_debug                 hdr_cbm_lay3_debug;                  /* 1280 */
    u_hdr_cbm_lay4_debug                 hdr_cbm_lay4_debug;                  /* 1284 */
    u_hdr_cbm_lay0_last_debug            hdr_cbm_lay0_last_debug;             /* 1288 */
    u_hdr_cbm_lay1_last_debug            hdr_cbm_lay1_last_debug;             /* 128c */
    u_hdr_cbm_lay2_last_debug            hdr_cbm_lay2_last_debug;             /* 1290 */
    u_hdr_cbm_lay3_last_debug            hdr_cbm_lay3_last_debug;             /* 1294 */
    u_hdr_cbm_lay4_last_debug            hdr_cbm_lay4_last_debug;             /* 1298 */
    u_hdr_vhdaccydiffd1sum               hdr_vhdaccydiffd1sum;                /* 129c */
    u_hdr_vhdaccydiffd2sum               hdr_vhdaccydiffd2sum;                /* 12a0 */
    u_hdr_vhdaccysum                     hdr_vhdaccysum;                      /* 12a4 */
    u_hdr_vhdaccsatsum                   hdr_vhdaccsatsum;                    /* 12a8 */
    u_hdr_vhdaccsathistsum               hdr_vhdaccsathistsum;                /* 12ac */
    u_hdr_vhdaccsathistcum               hdr_vhdaccsathistcum;                /* 12b0 */
    u_hdr_vhdaccyhist_0                  hdr_vhdaccyhist_0;                   /* 12b4 */
    u_hdr_vhdaccyhist_1                  hdr_vhdaccyhist_1;                   /* 12b8 */
    u_hdr_vhdaccyhist_2                  hdr_vhdaccyhist_2;                   /* 12bc */
    u_hdr_vhdaccyhist_3                  hdr_vhdaccyhist_3;                   /* 12c0 */
    u_hdr_vhdaccyhist_4                  hdr_vhdaccyhist_4;                   /* 12c4 */
    u_hdr_vhdaccyhist_5                  hdr_vhdaccyhist_5;                   /* 12c8 */
    u_hdr_vhdaccyhist_6                  hdr_vhdaccyhist_6;                   /* 12cc */
    u_hdr_vhdaccyhist_7                  hdr_vhdaccyhist_7;                   /* 12d0 */
    u_hdr_vhdaccyhist_8                  hdr_vhdaccyhist_8;                   /* 12d4 */
    u_hdr_vhdaccyhist_9                  hdr_vhdaccyhist_9;                   /* 12d8 */
    u_hdr_vhdaccyhist_10                 hdr_vhdaccyhist_10;                  /* 12dc */
    u_hdr_vhdaccyhist_11                 hdr_vhdaccyhist_11;                  /* 12e0 */
    u_hdr_vhdaccyhist_12                 hdr_vhdaccyhist_12;                  /* 12e4 */
    u_hdr_vhdaccyhist_13                 hdr_vhdaccyhist_13;                  /* 12e8 */
    u_hdr_vhdaccyhist_14                 hdr_vhdaccyhist_14;                  /* 12ec */
    u_hdr_vhdaccyhist_15                 hdr_vhdaccyhist_15;                  /* 12f0 */
    u_hdr_vhdaccyhist_16                 hdr_vhdaccyhist_16;                  /* 12f4 */
    u_hdr_vhdaccyhist_17                 hdr_vhdaccyhist_17;                  /* 12f8 */
    u_hdr_vhdaccyhist_18                 hdr_vhdaccyhist_18;                  /* 12fc */
    u_hdr_vhdaccyhist_19                 hdr_vhdaccyhist_19;                  /* 1300 */
    u_hdr_vhdaccsathist_0                hdr_vhdaccsathist_0;                 /* 1304 */
    u_hdr_vhdaccsathist_1                hdr_vhdaccsathist_1;                 /* 1308 */
    u_hdr_vhdaccsathist_2                hdr_vhdaccsathist_2;                 /* 130c */
    u_hdr_vhdaccsathist_3                hdr_vhdaccsathist_3;                 /* 1310 */
    u_hdr_vhdaccsathist_4                hdr_vhdaccsathist_4;                 /* 1314 */
    u_hdr_vhdaccsathist_5                hdr_vhdaccsathist_5;                 /* 1318 */
    u_hdr_vhdaccsathist_6                hdr_vhdaccsathist_6;                 /* 131c */
    u_hdr_vhdaccsathist_7                hdr_vhdaccsathist_7;                 /* 1320 */
    u_hdr_vhdaccsathist_8                hdr_vhdaccsathist_8;                 /* 1324 */
    u_hdr_vhdaccsathist_9                hdr_vhdaccsathist_9;                 /* 1328 */
    u_hdr_vhdaccsathist_10               hdr_vhdaccsathist_10;                /* 132c */
    u_hdr_vhdaccsathist_11               hdr_vhdaccsathist_11;                /* 1330 */
    u_hdr_vhdaccsathist_12               hdr_vhdaccsathist_12;                /* 1334 */
    u_hdr_vhdaccsathist_13               hdr_vhdaccsathist_13;                /* 1338 */
    u_hdr_vhdaccsathist_14               hdr_vhdaccsathist_14;                /* 133c */
    u_hdr_vhdaccsathist_15               hdr_vhdaccsathist_15;                /* 1340 */
    u_hdr_vhdaccsathist_16               hdr_vhdaccsathist_16;                /* 1344 */
    u_hdr_vhdaccsathist_17               hdr_vhdaccsathist_17;                /* 1348 */
    u_hdr_vhdaccsathist_18               hdr_vhdaccsathist_18;                /* 134c */
    u_hdr_vhdaccsathist_19               hdr_vhdaccsathist_19;                /* 1350 */
    unsigned int                         rsr36[2859];                         /* 1354 ~ 3ffc */
    u_vau_work_state                     vau_work_state;                      /* 4000 */
    u_vau_cur_list_addr                  vau_cur_list_addr;                   /* 4004 */
    u_vau_cur_node_id                    vau_cur_node_id;                     /* 4008 */
    u_vau_mac_rdy_need                   vau_mac_rdy_need;                    /* 400c */
    u_vau_mac_chn_state                  vau_mac_chn_state;                   /* 4010 */
    u_vau_proc_rdy_need                  vau_proc_rdy_need;                   /* 4014 */
    u_vau_proc_src1_rdy_need             vau_proc_src1_rdy_need;              /* 4018 */
    u_vau_proc_src2_rdy_need             vau_proc_src2_rdy_need;              /* 401c */
    u_vau_proc_des_rdy_need              vau_proc_des_rdy_need;               /* 4020 */
    u_vau_cbm_rdy_need                   vau_cbm_rdy_need;                    /* 4024 */
    u_vau_axi_ctrl_signal                vau_axi_ctrl_signal;                 /* 4028 */
    u_vau_offline_dcmp_dbg               vau_offline_dcmp_dbg;                /* 402c */
    u_vau_debug12                        vau_debug12;                         /* 4030 */
    u_vau_debug13                        vau_debug13;                         /* 4034 */
    u_vau_debug14                        vau_debug14;                         /* 4038 */
    u_vau_debug15                        vau_debug15;                         /* 403c */
    u_vau_debug16                        vau_debug16;                         /* 4040 */
    u_vau_debug17                        vau_debug17;                         /* 4044 */
    u_vau_debug18                        vau_debug18;                         /* 4048 */
    u_vau_debug19                        vau_debug19;                         /* 404c */
    u_vau_debug20                        vau_debug20;                         /* 4050 */
    u_vau_debug21                        vau_debug21;                         /* 4054 */
    u_vau_gauss_dbg0                     vau_gauss_dbg0;                      /* 4058 */
    u_vau_gauss_dbg1                     vau_gauss_dbg1;                      /* 405c */
    u_vau_cas_start_cnt                  vau_cas_start_cnt;                   /* 4060 */
    u_vau_eof_end_cnt                    vau_eof_end_cnt;                     /* 4064 */
    u_vau_vau_int_cnt                    vau_vau_int_cnt;                     /* 4068 */
    unsigned int                         rsr37[2];                            /* 406c ~ 4070 */
    u_vau_online_dcmp_dbg0               vau_online_dcmp_dbg0;                /* 4074 */
    u_vau_online_dcmp_dbg1               vau_online_dcmp_dbg1;                /* 4078 */
    u_vau_list_checksum                  vau_list_checksum;                   /* 407c */
    u_vau_src1_checksum                  vau_src1_checksum;                   /* 4080 */
    u_vau_src2_checksum                  vau_src2_checksum;                   /* 4084 */
    u_vau_src2c_checksum                 vau_src2c_checksum;                  /* 4088 */
    u_vau_model_checksum                 vau_model_checksum;                  /* 408c */
    u_vau_des_checksum                   vau_des_checksum;                    /* 4090 */
    unsigned int                         rsr38;                               /* 4094 */
    u_vau_done_list_addr                 vau_done_list_addr;                  /* 4098 */
    u_vau_done_node_id                   vau_done_node_id;                    /* 409c */
    u_vau_pfcnt_list                     vau_pfcnt_list;                      /* 40a0 */
    u_vau_tunl_sta                       vau_tunl_sta;                        /* 40a4 */
    u_vau_tunl_wr_ptr                    vau_tunl_wr_ptr;                     /* 40a8 */
    u_vau_tunl_rd_ptr                    vau_tunl_rd_ptr;                     /* 40ac */
    u_vau_done_offline_list_cnt          vau_done_offline_list_cnt;           /* 40b0 */
    u_vau_hdr_mac_rdy_need               vau_hdr_mac_rdy_need;                /* 40b4 */
    u_vau_hdr_mac_chn_state              vau_hdr_mac_chn_state;               /* 40b8 */
    u_vau_hdr_src1_checksum              vau_hdr_src1_checksum;               /* 40bc */
    u_vau_hdr_src2_checksum              vau_hdr_src2_checksum;               /* 40c0 */
    u_vau_hdr_src2c_checksum             vau_hdr_src2c_checksum;              /* 40c4 */
    u_vau_hdr_src1c_checksum             vau_hdr_src1c_checksum;              /* 40c8 */
    u_vau_hdr_des_checksum               vau_hdr_des_checksum;                /* 40cc */
} vau_regs_type;

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* DRV_GRAPHIC_HAL_VAU_REG_DEFINE_H */
