/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description : vau hal node ctrl file
 */

#ifndef DRV_GPU_VAU_HAL_NODE_H
#define DRV_GPU_VAU_HAL_NODE_H

#include "vau_reg_define.h"
#include "vau_hal_type.h"
#include "drv_vau_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef struct {
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
} u_vau_mix_node0;

typedef struct {
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
    u_vau_draw_matrix_20                 vau_draw_matrix_20;                  /* 238 */
    u_vau_draw_matrix_21                 vau_draw_matrix_21;                  /* 23c */
    u_vau_draw_matrix_22                 vau_draw_matrix_22;                  /* 240 */
    u_vau_draw_half_width                vau_draw_half_width;                 /* 244 */
    u_vau_tess_line_num_max              vau_tess_line_num_max;               /* 248 */
    u_vau_draw_rnd_m11                   vau_draw_rnd_m11;                    /* 24c */
    u_vau_draw_rnd_m12                   vau_draw_rnd_m12;                    /* 250 */
    u_vau_draw_rnd_m21                   vau_draw_rnd_m21;                    /* 254 */
    u_vau_draw_rnd_m22                   vau_draw_rnd_m22;                    /* 258 */
    u_vau_draw_rnd_d1                    vau_draw_rnd_d1;                     /* 25c */
    u_vau_draw_rnd_d2                    vau_draw_rnd_d2;                     /* 260 */
    u_vau_draw_rnd_e11                   vau_draw_rnd_e11;                    /* 264 */
    u_vau_draw_rnd_e12                   vau_draw_rnd_e12;                    /* 268 */
    u_vau_draw_rnd_e21                   vau_draw_rnd_e21;                    /* 26c */
    u_vau_draw_rnd_e22                   vau_draw_rnd_e22;                    /* 270 */
    u_vau_draw_threshold_quad            vau_draw_threshold_quad;             /* 274 */
    u_vau_draw_threshold_cubic           vau_draw_threshold_cubic;            /* 278 */
    u_vau_draw_miter_limit               vau_draw_miter_limit;                /* 27c */
} u_vau_mix_node1;

typedef struct {
    u_src2_hsp                           src2_hsp;                            /* 380 */
    u_src2_hloffset                      src2_hloffset;                       /* 384 */
    u_src2_hcoffset                      src2_hcoffset;                       /* 388 */
    u_src2_vsp                           src2_vsp;                            /* 38c */
    u_src2_vsr                           src2_vsr;                            /* 390 */
    u_src2_voffset                       src2_voffset;                        /* 394 */
    u_src2_zmeoreso                      src2_zmeoreso;                       /* 398 */
    u_src2_zmeireso                      src2_zmeireso;                       /* 39c */
    u_src2_yuv2rgb_ctrl                  src2_yuv2rgb_ctrl;                   /* 3a0 */
    u_src2_yuv2rgb_coef00                src2_yuv2rgb_coef00;                 /* ca4 */
    u_src2_yuv2rgb_coef01                src2_yuv2rgb_coef01;                 /* ca8 */
    u_src2_yuv2rgb_coef02                src2_yuv2rgb_coef02;                 /* cac */
    u_src2_yuv2rgb_coef03                src2_yuv2rgb_coef03;                 /* cb0 */
    u_src2_yuv2rgb_coef04                src2_yuv2rgb_coef04;                 /* cb4 */
    u_src2_yuv2rgb_coef05                src2_yuv2rgb_coef05;                 /* cb8 */
    u_src2_yuv2rgb_coef06                src2_yuv2rgb_coef06;                 /* cbc */
    u_src2_yuv2rgb_coef07                src2_yuv2rgb_coef07;                 /* cc0 */
    u_src2_yuv2rgb_coef08                src2_yuv2rgb_coef08;                 /* cc4 */
    u_src2_yuv2rgb_scale2p               src2_yuv2rgb_scale2p;                /* cec */
    u_src2_yuv2rgb_in_dc00               src2_yuv2rgb_in_dc00;                /* cf0 */
    u_src2_yuv2rgb_in_dc01               src2_yuv2rgb_in_dc01;                /* cf4 */
    u_src2_yuv2rgb_in_dc02               src2_yuv2rgb_in_dc02;                /* cf8 */
    u_src2_yuv2rgb_out_dc00              src2_yuv2rgb_out_dc00;               /* cfc */
    u_src2_yuv2rgb_out_dc01              src2_yuv2rgb_out_dc01;               /* d00 */
    u_src2_yuv2rgb_out_dc02              src2_yuv2rgb_out_dc02;               /* d04 */
    u_src2_yuv2rgb_min                   src2_yuv2rgb_min;                    /* d20 */
    u_src2_yuv2rgb_max                   src2_yuv2rgb_max;                    /* d24 */
    unsigned int                         rsr[2];                              /* NO MAP */
    u_vau_hsv_rgb_cfg                    vau_hsv_rgb_cfg;                     /* 374 */
    u_vau_hsv_rgb_adj_h_v                vau_hsv_rgb_adj_h_v;                 /* 378 */
    u_vau_hsv_rgb_adj_s                  vau_hsv_rgb_adj_s;                   /* 37c */
} u_vau_mix_node2;

typedef struct {
    u_vau_sdf_node_ctrl                  vau_sdf_node_ctrl;                   /* f00 */
    u_vau_sdf_node_ctrl1                 vau_sdf_node_ctrl1;                  /* f04 */
    u_vau_sdf_glb_addr                   vau_sdf_glb_addr;                    /* f08 */
    u_vau_sdf_prmt_addr                  vau_sdf_prmt_addr;                   /* f0c */
    u_vau_sdf_gamma_addr                 vau_sdf_gamma_addr;                  /* f10 */
    u_vau_sdf_degamma_addr               vau_sdf_degamma_addr;                /* f14 */
    u_vau_sdf_txtre0_addr                vau_sdf_txtre0_addr;                 /* f18 */
    u_vau_sdf_txtre0_stride              vau_sdf_txtre0_stride;               /* f1c */
    u_vau_sdf_txtre1_addr                vau_sdf_txtre1_addr;                 /* f20 */
    u_vau_sdf_txtre1_stride              vau_sdf_txtre1_stride;               /* f24 */
    u_vau_sdf_txtre2_addr                vau_sdf_txtre2_addr;                 /* f28 */
    u_vau_sdf_txtre2_stride              vau_sdf_txtre2_stride;               /* f2c */
    u_vau_sdf_txtre3_addr                vau_sdf_txtre3_addr;                 /* f30 */
    u_vau_sdf_txtre3_stride              vau_sdf_txtre3_stride;               /* f34 */
    u_vau_sdf_splt_cfg                   vau_sdf_splt_cfg;                    /* f38 */
    u_vau_sdf_partial_start              vau_sdf_partial_start;               /* f3c */
    u_vau_sdf_partial_end                vau_sdf_partial_end;                 /* f40 */
    u_vau_sdf_partial_pad_end            vau_sdf_partial_pad_end;             /* f44 */
    u_vau_sdf_reso                       vau_sdf_reso;                        /* f48 */
    u_vau_sdf_coarse_reso                vau_sdf_coarse_reso;                 /* f4c */
    unsigned int                         rsr20[12];                           /* f50 ~ f7c */
} u_vau_mix_sdf;

typedef union {
    u_vau_mix_node0 mapp;
    u_vau_mix_node1 draw;
    u_vau_mix_node2 semi;
    u_vau_mix_sdf sdf;
} u_vau_mix_node;

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
    u_vau_src1c_addr                     vau_src1c_addr;                      /* 28 */
    u_vau_src1c_stride                   vau_src1c_stride;                    /* 2c */
    u_vau_src2_ctrl                      vau_src2_ctrl;                       /* 30 */
    u_vau_src2_addr                      vau_src2_addr;                       /* 34 */
    u_vau_src2_stride                    vau_src2_stride;                     /* 38 */
    u_vau_src2_imgsize                   vau_src2_imgsize;                    /* 3c */
    u_vau_src2_rect_xy                   vau_src2_rect_xy;                    /* 40 */
    u_vau_src2_rect_wh                   vau_src2_rect_wh;                    /* 44 */
    u_vau_src2_fill                      vau_src2_fill;                       /* 48 */
    u_vau_src2_alpha                     vau_src2_alpha;                      /* 4c */
    u_vau_src2_pix_rgb                   vau_src2_pix_rgb;                    /* 50 */
    u_vau_src2_dcmp_cfg0                 vau_src2_dcmp_cfg0;                  /* 54 */
    u_vau_src2_dcmp_cfg1                 vau_src2_dcmp_cfg1;                  /* 58 */
    u_vau_src2_dcmp_cfg2                 vau_src2_dcmp_cfg2;                  /* 5c */
    u_vau_src2_dcmp_cfg3                 vau_src2_dcmp_cfg3;                  /* 60 */
    u_vau_src2_htc_online_dcmp0          vau_src2_htc_online_dcmp0;           /* 64 */
    u_vau_src2c_addr                     vau_src2c_addr;                      /* 68 */
    u_vau_src2c_stride                   vau_src2c_stride;                    /* 6c */
    u_vau_des_ctrl                       vau_des_ctrl;                        /* 70 */
    u_vau_des_addr                       vau_des_addr;                        /* 74 */
    u_vau_des_stride                     vau_des_stride;                      /* 78 */
    u_vau_des_imgsize                    vau_des_imgsize;                     /* 7c */
    u_vau_des_rect_xy                    vau_des_rect_xy;                     /* 80 */
    u_vau_des_rect_wh                    vau_des_rect_wh;                     /* 84 */
    u_vau_des_alpha                      vau_des_alpha;                       /* 88 */
    u_vau_des_htc_online_cmp0            vau_des_htc_online_cmp0;             /* 8c */
    u_vau_cbm_ctrl                       vau_cbm_ctrl;                        /* 90 */
    u_vau_cbm_colorize                   vau_cbm_colorize;                    /* 94 */
    u_vau_cbm_alu_para                   vau_cbm_alu_para;                    /* 98 */
    u_vau_cbm_key_para                   vau_cbm_key_para;                    /* 9c */
    u_vau_cbm_key_min                    vau_cbm_key_min;                     /* a0 */
    u_vau_cbm_key_max                    vau_cbm_key_max;                     /* a4 */
    u_vau_cbm_key_mask                   vau_cbm_key_mask;                    /* a8 */
    u_vau_cbm_src1_cbm_para              vau_cbm_src1_cbm_para;               /* ac */
    u_vau_cbm_src2_cbm_para              vau_cbm_src2_cbm_para;               /* b0 */
    u_vau_gradient_typical_color         vau_gradient_typical_color;          /* b4 */
    u_vau_gradient_linear_const_x        vau_gradient_linear_const_x;         /* b8 */
    u_vau_gradient_linear_const_y        vau_gradient_linear_const_y;         /* bc */
    u_vau_gradient_linear_start          vau_gradient_linear_start;           /* c0 */
    u_vau_gradient_const_mode            vau_gradient_const_mode;             /* c4 */
    u_vau_gradient_radial_const_r        vau_gradient_radial_const_r;         /* c8 */
    u_vau_gradient_radial_center         vau_gradient_radial_center;          /* cc */
    u_vau_gradient_sweep_center          vau_gradient_sweep_center;           /* d0 */
    u_vau_gradient_sweep_start_angle     vau_gradient_sweep_start_angle;      /* d4 */
    u_vau_gradient_tile_mode             vau_gradient_tile_mode;              /* d8 */
    u_vau_gradient_stops_offset0         vau_gradient_stops_offset0;          /* dc */
    u_vau_gradient_stops_offset1         vau_gradient_stops_offset1;          /* e0 */
    u_vau_gradient_stops_color0          vau_gradient_stops_color[7];         /* e4~fc */
    u_vau_gradient_color_reci0           vau_gradient_color_reci[6];          /* 100~114 */
    u_vau_gradient_stops_num             vau_gradient_stops_num;              /* 118 */
    u_gauss_blur_ctrl                    gauss_blur_ctrl;                     /* 11c */
    u_gauss_blur_a0                      gauss_blur_a0;                       /* 120 */
    u_gauss_blur_a1                      gauss_blur_a1;                       /* 124 */
    u_gauss_blur_a2                      gauss_blur_a2;                       /* 128 */
    u_gauss_blur_b_causala0              gauss_blur_b_causala0;               /* 12c */
    u_gauss_blur_b_causala1              gauss_blur_b_causala1;               /* 130 */
    u_gauss_blur_b_causala2              gauss_blur_b_causala2;               /* 134 */
    u_gauss_blur_b_anticausala0          gauss_blur_b_anticausala0;           /* 138 */
    u_gauss_blur_b_anticausala1          gauss_blur_b_anticausala1;           /* 13c */
    u_gauss_blur_b_anticausala2          gauss_blur_b_anticausala2;           /* 140 */
    u_gauss_blur_causala_ratio           gauss_blur_causala_ratio;            /* 144 */
    u_gauss_blur_anticausala_ratio       gauss_blur_anticausala_ratio;        /* 148 */
    u_hdr_hipp_ctadj_ctrl                hdr_hipp_ctadj_ctrl;                 /* 14c */
    u_hdr_hipp_ctadj_gain                hdr_hipp_ctadj_gain;                 /* 150 */
    u_hdr_hipp_ctadj_offset              hdr_hipp_ctadj_offset;               /* 154 */
    u_hdr_hipp_ctadj_offset_b            hdr_hipp_ctadj_offset_b;             /* 158 */
    u_hdr_hipp_ctadj_min                 hdr_hipp_ctadj_min;                  /* 15c */
    u_hdr_hipp_ctadj_max                 hdr_hipp_ctadj_max;                  /* 160 */
    u_hdr_hipp_ctadj_gain_w              hdr_hipp_ctadj_gain_w;               /* 164 */
    u_hdr_hipp_ctadj_offset_w            hdr_hipp_ctadj_offset_w;             /* 168 */
    u_hdr_hipp_ctadj_min_w               hdr_hipp_ctadj_min_w;                /* 16c */
    u_hdr_hipp_ctadj_max_w               hdr_hipp_ctadj_max_w;                /* 170 */
    u_vau_src1_t2r_ctrl                  vau_src1_t2r_ctrl;                   /* 174 */
    u_vau_src2_t2r_ctrl                  vau_src2_t2r_ctrl;                   /* 178 */
    u_vau_des_r2t_ctrl                   vau_des_r2t_ctrl;                    /* 17c */
    u_vau_clut_addr                      vau_clut_addr;                       /* 180 */
    u_tde_src2_zme_lhaddr                tde_src2_zme_lhaddr;                 /* 184 */
    u_tde_src2_zme_lvaddr                tde_src2_zme_lvaddr;                 /* 188 */
    u_tde_src2_zme_chaddr                tde_src2_zme_chaddr;                 /* 18c */
    u_tde_src2_zme_cvaddr                tde_src2_zme_cvaddr;                 /* 190 */
    u_vau_node_cfg                       vau_node_cfg;                        /* 194 */
    u_vau_node_id                        vau_node_id;                         /* 198 */
    u_vau_intmask                        vau_intmask;                         /* 19c */
    u_vau_pnext                          vau_pnext;                           /* 1a0 */
    u_vau_hdr_sys_ctrl                   vau_hdr_sys_ctrl;                    /* 1a4 */
    u_vau_hdr_sys_ctrl1                  vau_hdr_sys_ctrl1;                   /* 1a8 */
    u_vau_fpga_tag                       vau_fpga_tag;                        /* 1ac */
    unsigned int                         rsr0[20];                            /* 1b0 ~ 1fc */
    u_vau_mix_node                       mix;                                 /* 200 ~ 27c */
} vau_hardware_node;

/* src1 */
td_void vau_hal_node_set_src1_info(vau_hardware_node *node, const drv_vau_surface *surface);
td_void vau_hal_node_set_src1_mode(vau_hardware_node *node, td_u8 mode);
td_void vau_hal_node_set_src1_alpha(vau_hardware_node *node, const drv_vau_surface *surface);

/* src2 */
td_void vau_hal_node_set_src2_info(vau_hardware_node *node, const drv_vau_surface *surface);
td_void vau_hal_node_set_src2_mode(vau_hardware_node *node, td_u32 mode);
td_void vau_hal_node_set_src2_alpha(vau_hardware_node *node, const drv_vau_surface *src2);
td_void vau_hal_node_set_src2_clut(vau_hardware_node *node, const drv_vau_surface *surface);
td_void vau_hal_node_set_affine_info(vau_hardware_node *node, vau_affine_cfg *hal_cfg, td_u32 shift);
td_void vau_hal_node_set_yuv_base_info(vau_hardware_node *node, const drv_vau_surface *surface,
                                       const drv_vau_surface *dst);

/* dst */
td_void vau_hal_node_set_dst_info(vau_hardware_node *node, const drv_vau_surface *surface);

/* cbm */
td_void vau_hal_node_set_cbm(vau_hardware_node *node, td_u32 alu_mode);
td_void vau_hal_node_set_rop(vau_hardware_node *node, const drv_vau_rop_opt *rop_opt);
td_void vau_hal_node_set_colorkey(vau_hardware_node *node, hal_vau_colorkey_opt *colorkey_opt);
td_void vau_hal_node_set_blend_mode(vau_hardware_node *node, td_u32 src1_mode, td_u32 src2_mode);
td_void vau_hal_node_set_blend_alpha_mode(vau_hardware_node *node, td_u32 alpha_from);

/* isr */
td_void vau_hal_node_enable_complete_int(vau_hardware_node *node, td_bool is_offline);

/* next node */
td_void vau_hal_node_add_next_node(vau_hardware_node *node, td_u32 phy_addr);

td_void vau_hal_node_set_alpha_strb(vau_hardware_node *node, td_bool enable);

// mapping and 3d
td_void vau_hal_node_set_mapping(vau_hardware_node *node, const drv_vau_surface *src_surface);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* DRV_GPU_VAU_HAL_NODE_H */
