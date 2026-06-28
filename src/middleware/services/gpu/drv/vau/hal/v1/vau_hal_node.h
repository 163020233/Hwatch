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
    unsigned int                         rsr[3];                              /* 274 ~ 27c */
} u_vau_mix_node1;

typedef struct {
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
    unsigned int                         rsr[19];                             /* 3a8 ~ 3f0 */
    u_vau_hsv_rgb_cfg                    vau_hsv_rgb_cfg;                     /* 374 */
    u_vau_hsv_rgb_adj_h_v                vau_hsv_rgb_adj_h_v;                 /* 378 */
    u_vau_hsv_rgb_adj_s                  vau_hsv_rgb_adj_s;                   /* 37c */
} u_vau_mix_node2;

typedef union {
    u_vau_mix_node0 mapp;
    u_vau_mix_node1 draw;
    u_vau_mix_node2 semi;
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
    u_vau_gradient_stops_color0          vau_gradient_stops_color[7];         /* ec~104 */
    u_vau_gradient_color_reci0           vau_gradient_color_reci[6];          /* 108~11c */
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
    u_vau_mix_node                       mix;                                 /* 200 ~ 27c */
#ifdef GPU_RESO_CFG_SUPPORT
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
#endif
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
