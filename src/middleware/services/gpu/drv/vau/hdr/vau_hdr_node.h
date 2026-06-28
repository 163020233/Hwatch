/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: vau hdr node head file
 */

#ifndef DRV_VAU_HDR_NODE_H
#define DRV_VAU_HDR_NODE_H

#include "td_base.h"
#include "soc_osal.h"
#include "drv_vau_type.h"
#include "vau_reg_define.h"
#include "vau_hal_node.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#if defined(GPU_1A22)

typedef struct {
    u_hdr_disp_1_dither_ctrl             hdr_disp_1_dither_ctrl;              /* db8 */
    u_hdr_disp_1_dither_ink_ctrl         hdr_disp_1_dither_ink_ctrl;          /* dbc */
    u_hdr_disp_1_dither_ink_pos          hdr_disp_1_dither_ink_pos;           /* dc0 */
    u_hdr_cbm_bkg1                       hdr_cbm_bkg1;                        /* dc4 */
    u_hdr_cbm_bkalpha                    hdr_cbm_bkalpha;                     /* dc8 */
    u_hdr_cbm_mix1                       hdr_cbm_mix1;                        /* dcc */
    u_hdr_cbm_mix_bypass                 hdr_cbm_mix_bypass;                  /* dd0 */
    u_hdr_cbm_bkg2                       hdr_cbm_bkg2;                        /* dd4 */
    unsigned int                         rsr18;                               /* dd8 */
    u_hdr_cbm_mix2                       hdr_cbm_mix2;                        /* ddc */
    unsigned int                         rsr19[2];                            /* de0 ~ de4 */
    u_hdr_hc_bmp_thd                     hdr_hc_bmp_thd;                      /* de8 */
} u_vau_mix_node3;

typedef struct {
    u_hdr_vau_cvfir_vinfo                hdr_vau_cvfir_vinfo;                 /* c00 */
    u_hdr_vau_cvfir_vsp                  hdr_vau_cvfir_vsp;                   /* c04 */
    u_hdr_vau_cvfir_voffset              hdr_vau_cvfir_voffset;               /* c08 */
    u_hdr_vau_cvfir_vboffset             hdr_vau_cvfir_vboffset;              /* c0c */
    u_hdr_vau_hfir_ctrl                  hdr_vau_hfir_ctrl;                   /* c10 */
    u_src1_hipp_csc_ctrl                 src1_hipp_csc_ctrl;                  /* c14 */
    u_src1_hipp_csc_coef00               src1_hipp_csc_coef00;                /* c18 */
    u_src1_hipp_csc_coef01               src1_hipp_csc_coef01;                /* c1c */
    u_src1_hipp_csc_coef02               src1_hipp_csc_coef02;                /* c20 */
    u_src1_hipp_csc_coef10               src1_hipp_csc_coef10;                /* c24 */
    u_src1_hipp_csc_coef11               src1_hipp_csc_coef11;                /* c28 */
    u_src1_hipp_csc_coef12               src1_hipp_csc_coef12;                /* c2c */
    u_src1_hipp_csc_coef20               src1_hipp_csc_coef20;                /* c30 */
    u_src1_hipp_csc_coef21               src1_hipp_csc_coef21;                /* c34 */
    u_src1_hipp_csc_coef22               src1_hipp_csc_coef22;                /* c38 */
    u_src1_hipp_csc_scale                src1_hipp_csc_scale;                 /* c3c */
    u_src1_hipp_csc_idc0                 src1_hipp_csc_idc0;                  /* c40 */
    u_src1_hipp_csc_idc1                 src1_hipp_csc_idc1;                  /* c44 */
    u_src1_hipp_csc_idc2                 src1_hipp_csc_idc2;                  /* c48 */
    u_src1_hipp_csc_odc0                 src1_hipp_csc_odc0;                  /* c4c */
    u_src1_hipp_csc_odc1                 src1_hipp_csc_odc1;                  /* c50 */
    u_src1_hipp_csc_odc2                 src1_hipp_csc_odc2;                  /* c54 */
    u_src1_hipp_csc_min_y                src1_hipp_csc_min_y;                 /* c58 */
    u_src1_hipp_csc_min_c                src1_hipp_csc_min_c;                 /* c5c */
    u_src1_hipp_csc_max_y                src1_hipp_csc_max_y;                 /* c60 */
    u_src1_hipp_csc_max_c                src1_hipp_csc_max_c;                 /* c64 */
    u_hdr_vdm_ctrl                       hdr_vdm_ctrl;                        /* c68 */
    u_hdr_vdm_para_degmm_addr            hdr_vdm_para_degmm_addr;             /* c6c */
    u_hdr_vdm_para_gmm_addr              hdr_vdm_para_gmm_addr;               /* c70 */
    u_hdr_vdm_para_tmap_addr             hdr_vdm_para_tmap_addr;              /* c74 */
    u_hdr_vdm_csc_ctrl                   hdr_vdm_csc_ctrl;                    /* c78 */
    u_hdr_vdm_csc_coef00                 hdr_vdm_csc_coef00;                  /* c7c */
    u_hdr_vdm_csc_coef01                 hdr_vdm_csc_coef01;                  /* c80 */
    u_hdr_vdm_csc_coef02                 hdr_vdm_csc_coef02;                  /* c84 */
    u_hdr_vdm_csc_coef10                 hdr_vdm_csc_coef10;                  /* c88 */
    u_hdr_vdm_csc_coef11                 hdr_vdm_csc_coef11;                  /* c8c */
    u_hdr_vdm_csc_coef12                 hdr_vdm_csc_coef12;                  /* c90 */
    u_hdr_vdm_csc_coef20                 hdr_vdm_csc_coef20;                  /* c94 */
    u_hdr_vdm_csc_coef21                 hdr_vdm_csc_coef21;                  /* c98 */
    u_hdr_vdm_csc_coef22                 hdr_vdm_csc_coef22;                  /* c9c */
    unsigned int                         rsr17;                               /* ca0 */
    u_src2_yuv2rgb_coef00                src2_yuv2rgb_coef00;                 /* ca4 */
    u_src2_yuv2rgb_coef01                src2_yuv2rgb_coef01;                 /* ca8 */
    u_src2_yuv2rgb_coef02                src2_yuv2rgb_coef02;                 /* cac */
    u_src2_yuv2rgb_coef03                src2_yuv2rgb_coef03;                 /* cb0 */
    u_src2_yuv2rgb_coef04                src2_yuv2rgb_coef04;                 /* cb4 */
    u_src2_yuv2rgb_coef05                src2_yuv2rgb_coef05;                 /* cb8 */
    u_src2_yuv2rgb_coef06                src2_yuv2rgb_coef06;                 /* cbc */
    u_src2_yuv2rgb_coef07                src2_yuv2rgb_coef07;                 /* cc0 */
    u_src2_yuv2rgb_coef08                src2_yuv2rgb_coef08;                 /* cc4 */
    u_src2_yuv2rgb_coef10                src2_yuv2rgb_coef10;                 /* cc8 */
    u_src2_yuv2rgb_coef11                src2_yuv2rgb_coef11;                 /* ccc */
    u_src2_yuv2rgb_coef12                src2_yuv2rgb_coef12;                 /* cd0 */
    u_src2_yuv2rgb_coef13                src2_yuv2rgb_coef13;                 /* cd4 */
    u_src2_yuv2rgb_coef14                src2_yuv2rgb_coef14;                 /* cd8 */
    u_src2_yuv2rgb_coef15                src2_yuv2rgb_coef15;                 /* cdc */
    u_src2_yuv2rgb_coef16                src2_yuv2rgb_coef16;                 /* ce0 */
    u_src2_yuv2rgb_coef17                src2_yuv2rgb_coef17;                 /* ce4 */
    u_src2_yuv2rgb_coef18                src2_yuv2rgb_coef18;                 /* ce8 */
    u_src2_yuv2rgb_scale2p               src2_yuv2rgb_scale2p;                /* cec */
    u_src2_yuv2rgb_in_dc00               src2_yuv2rgb_in_dc00;                /* cf0 */
    u_src2_yuv2rgb_in_dc01               src2_yuv2rgb_in_dc01;                /* cf4 */
    u_src2_yuv2rgb_in_dc02               src2_yuv2rgb_in_dc02;                /* cf8 */
    u_src2_yuv2rgb_out_dc00              src2_yuv2rgb_out_dc00;               /* cfc */
    u_src2_yuv2rgb_out_dc01              src2_yuv2rgb_out_dc01;               /* d00 */
    u_src2_yuv2rgb_out_dc02              src2_yuv2rgb_out_dc02;               /* d04 */
    u_src2_yuv2rgb_in_dc10               src2_yuv2rgb_in_dc10;                /* d08 */
    u_src2_yuv2rgb_in_dc11               src2_yuv2rgb_in_dc11;                /* d0c */
    u_src2_yuv2rgb_in_dc12               src2_yuv2rgb_in_dc12;                /* d10 */
    u_src2_yuv2rgb_out_dc10              src2_yuv2rgb_out_dc10;               /* d14 */
    u_src2_yuv2rgb_out_dc11              src2_yuv2rgb_out_dc11;               /* d18 */
    u_src2_yuv2rgb_out_dc12              src2_yuv2rgb_out_dc12;               /* d1c */
    u_src2_yuv2rgb_min                   src2_yuv2rgb_min;                    /* d20 */
    u_src2_yuv2rgb_max                   src2_yuv2rgb_max;                    /* d24 */
    u_hdr_disp_0_dither_ctrl             hdr_disp_0_dither_ctrl;              /* d28 */
    u_hdr_disp_0_dither_ink_ctrl         hdr_disp_0_dither_ink_ctrl;          /* d2c */
    u_hdr_disp_0_dither_ink_pos          hdr_disp_0_dither_ink_pos;           /* d30 */
    u_hdr_rcbn_ctrl                      hdr_rcbn_ctrl;                       /* d34 */
    u_hdr_rcbn_gmap_size_mode            hdr_rcbn_gmap_size_mode;             /* d38 */
    u_hdr_rcbn_gmap_chn_num              hdr_rcbn_gmap_chn_num;               /* d3c */
    u_hdr_rcbn_gcoeff_rmin1              hdr_rcbn_gcoeff_rmin1;               /* d40 */
    u_hdr_rcbn_gcoeff_rmin2              hdr_rcbn_gcoeff_rmin2;               /* d44 */
    u_hdr_rcbn_gcoeff_rmax               hdr_rcbn_gcoeff_rmax;                /* d48 */
    u_hdr_rcbn_gcoeff_gmin1              hdr_rcbn_gcoeff_gmin1;               /* d4c */
    u_hdr_rcbn_gcoeff_gmin2              hdr_rcbn_gcoeff_gmin2;               /* d50 */
    u_hdr_rcbn_gcoeff_gmax               hdr_rcbn_gcoeff_gmax;                /* d54 */
    u_hdr_rcbn_gcoeff_bmin1              hdr_rcbn_gcoeff_bmin1;               /* d58 */
    u_hdr_rcbn_gcoeff_bmin2              hdr_rcbn_gcoeff_bmin2;               /* d5c */
    u_hdr_rcbn_gcoeff_bmax               hdr_rcbn_gcoeff_bmax;                /* d60 */
    u_hdr_rcbn_kcoeff_r_alte             hdr_rcbn_kcoeff_r_alte;              /* d64 */
    u_hdr_rcbn_kcoeff_r_base             hdr_rcbn_kcoeff_r_base;              /* d68 */
    u_hdr_rcbn_kcoeff_g_alte             hdr_rcbn_kcoeff_g_alte;              /* d6c */
    u_hdr_rcbn_kcoeff_g_base             hdr_rcbn_kcoeff_g_base;              /* d70 */
    u_hdr_rcbn_kcoeff_b_alte             hdr_rcbn_kcoeff_b_alte;              /* d74 */
    u_hdr_rcbn_kcoeff_b_base             hdr_rcbn_kcoeff_b_base;              /* d78 */
    u_hdr_rcbn_weight                    hdr_rcbn_weight;                     /* d7c */
    u_hdr_vdm_csc1_ctrl                  hdr_vdm_csc1_ctrl;                   /* d80 */
    u_hdr_vdm_csc1_coef00                hdr_vdm_csc1_coef00;                 /* d84 */
    u_hdr_vdm_csc1_coef01                hdr_vdm_csc1_coef01;                 /* d88 */
    u_hdr_vdm_csc1_coef02                hdr_vdm_csc1_coef02;                 /* d8c */
    u_hdr_vdm_csc1_coef10                hdr_vdm_csc1_coef10;                 /* d90 */
    u_hdr_vdm_csc1_coef11                hdr_vdm_csc1_coef11;                 /* d94 */
    u_hdr_vdm_csc1_coef12                hdr_vdm_csc1_coef12;                 /* d98 */
    u_hdr_vdm_csc1_coef20                hdr_vdm_csc1_coef20;                 /* d9c */
    u_hdr_vdm_csc1_coef21                hdr_vdm_csc1_coef21;                 /* da0 */
    u_hdr_vdm_csc1_coef22                hdr_vdm_csc1_coef22;                 /* da4 */
    u_hdr_vdm_tmap_luma_coef0            hdr_vdm_tmap_luma_coef0;             /* da8 */
    u_hdr_vdm_tmap_luma_coef1            hdr_vdm_tmap_luma_coef1;             /* dac */
    u_hdr_vdm_tmap_luma_coef2            hdr_vdm_tmap_luma_coef2;             /* db0 */
    u_hdr_vdm_tonemap_alpha              hdr_vdm_tonemap_alpha;               /* db4 */
    u_hdr_disp_1_dither_ctrl             hdr_disp_1_dither_ctrl;              /* db8 */
    u_hdr_disp_1_dither_ink_ctrl         hdr_disp_1_dither_ink_ctrl;          /* dbc */
    u_hdr_disp_1_dither_ink_pos          hdr_disp_1_dither_ink_pos;           /* dc0 */
} u_vau_mix_node4;

typedef union {
    u_vau_mix_node3 hdr_cbm;
    u_vau_mix_node4 hdr_combine;
} u_vau_mix_node_hdr;

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
    u_vau_mix_node_hdr                   hdr;                                 /* 280 ~ 4F0 */
} vau_hdr_hardware_node;

#elif defined(GPU_3322)

typedef struct {
    u_hdr_disp_1_dither_ctrl             hdr_disp_1_dither_ctrl;              /* e74 */
    u_hdr_disp_1_dither_ink_ctrl         hdr_disp_1_dither_ink_ctrl;          /* e78 */
    u_hdr_disp_1_dither_ink_pos          hdr_disp_1_dither_ink_pos;           /* e7c */
    u_hdr_hipp_ctadj_ctrl                hdr_hipp_ctadj_ctrl;                 /* e80 */
    u_hdr_hipp_ctadj_gain                hdr_hipp_ctadj_gain;                 /* e84 */
    u_hdr_hipp_ctadj_offset              hdr_hipp_ctadj_offset;               /* e88 */
    u_hdr_hipp_ctadj_offset_b            hdr_hipp_ctadj_offset_b;             /* e8c */
    u_hdr_hipp_ctadj_min                 hdr_hipp_ctadj_min;                  /* e90 */
    u_hdr_hipp_ctadj_max                 hdr_hipp_ctadj_max;                  /* e94 */
    u_hdr_hipp_ctadj_gain_w              hdr_hipp_ctadj_gain_w;               /* e98 */
    u_hdr_hipp_ctadj_offset_w            hdr_hipp_ctadj_offset_w;             /* e9c */
    u_hdr_hipp_ctadj_min_w               hdr_hipp_ctadj_min_w;                /* ea0 */
    u_hdr_hipp_ctadj_max_w               hdr_hipp_ctadj_max_w;                /* ea4 */
    u_hdr_cbm_bkg1                       hdr_cbm_bkg1;                        /* ea8 */
    u_hdr_cbm_bkalpha                    hdr_cbm_bkalpha;                     /* eac */
    u_hdr_cbm_mix1                       hdr_cbm_mix1;                        /* eb0 */
    u_hdr_cbm_mix_bypass                 hdr_cbm_mix_bypass;                  /* eb4 */
    u_hdr_cbm_bkg2                       hdr_cbm_bkg2;                        /* eb8 */
    unsigned int                         rsr24;                               /* ebc */
    u_hdr_cbm_mix2                       hdr_cbm_mix2;                        /* ec0 */
    unsigned int                         rsr25[2];                            /* ec4 ~ ec8 */
    u_hdr_hc_bmp_thd                     hdr_hc_bmp_thd;                      /* ecc */
} u_vau_mix_node3;

typedef struct {
    u_hdr_vau_cvfir_vinfo                hdr_vau_cvfir_vinfo;                 /* c00 */
    u_hdr_vau_cvfir_vsp                  hdr_vau_cvfir_vsp;                   /* c04 */
    u_hdr_vau_cvfir_voffset              hdr_vau_cvfir_voffset;               /* c08 */
    u_hdr_vau_cvfir_vboffset             hdr_vau_cvfir_vboffset;              /* c0c */
    u_hdr_vau_hfir_ctrl                  hdr_vau_hfir_ctrl;                   /* c10 */
    u_src1_hipp_csc_ctrl                 src1_hipp_csc_ctrl;                  /* c14 */
    u_src1_hipp_csc_coef00               src1_hipp_csc_coef00;                /* c18 */
    u_src1_hipp_csc_coef01               src1_hipp_csc_coef01;                /* c1c */
    u_src1_hipp_csc_coef02               src1_hipp_csc_coef02;                /* c20 */
    u_src1_hipp_csc_coef10               src1_hipp_csc_coef10;                /* c24 */
    u_src1_hipp_csc_coef11               src1_hipp_csc_coef11;                /* c28 */
    u_src1_hipp_csc_coef12               src1_hipp_csc_coef12;                /* c2c */
    u_src1_hipp_csc_coef20               src1_hipp_csc_coef20;                /* c30 */
    u_src1_hipp_csc_coef21               src1_hipp_csc_coef21;                /* c34 */
    u_src1_hipp_csc_coef22               src1_hipp_csc_coef22;                /* c38 */
    u_src1_hipp_csc_scale                src1_hipp_csc_scale;                 /* c3c */
    u_src1_hipp_csc_idc0                 src1_hipp_csc_idc0;                  /* c40 */
    u_src1_hipp_csc_idc1                 src1_hipp_csc_idc1;                  /* c44 */
    u_src1_hipp_csc_idc2                 src1_hipp_csc_idc2;                  /* c48 */
    u_src1_hipp_csc_odc0                 src1_hipp_csc_odc0;                  /* c4c */
    u_src1_hipp_csc_odc1                 src1_hipp_csc_odc1;                  /* c50 */
    u_src1_hipp_csc_odc2                 src1_hipp_csc_odc2;                  /* c54 */
    u_src1_hipp_csc_min_y                src1_hipp_csc_min_y;                 /* c58 */
    u_src1_hipp_csc_min_c                src1_hipp_csc_min_c;                 /* c5c */
    u_src1_hipp_csc_max_y                src1_hipp_csc_max_y;                 /* c60 */
    u_src1_hipp_csc_max_c                src1_hipp_csc_max_c;                 /* c64 */
    u_hdr_vdm_ctrl                       hdr_vdm_ctrl;                        /* c68 */
    u_hdr_vdm_para_degmm_addr            hdr_vdm_para_degmm_addr;             /* c6c */
    u_hdr_vdm_para_gmm_addr              hdr_vdm_para_gmm_addr;               /* c70 */
    u_hdr_vdm_para_tmap_addr             hdr_vdm_para_tmap_addr;              /* c74 */
    u_hdr_vdm_csc_ctrl                   hdr_vdm_csc_ctrl;                    /* c78 */
    u_hdr_vdm_csc_coef00                 hdr_vdm_csc_coef00;                  /* c7c */
    u_hdr_vdm_csc_coef01                 hdr_vdm_csc_coef01;                  /* c80 */
    u_hdr_vdm_csc_coef02                 hdr_vdm_csc_coef02;                  /* c84 */
    u_hdr_vdm_csc_coef10                 hdr_vdm_csc_coef10;                  /* c88 */
    u_hdr_vdm_csc_coef11                 hdr_vdm_csc_coef11;                  /* c8c */
    u_hdr_vdm_csc_coef12                 hdr_vdm_csc_coef12;                  /* c90 */
    u_hdr_vdm_csc_coef20                 hdr_vdm_csc_coef20;                  /* c94 */
    u_hdr_vdm_csc_coef21                 hdr_vdm_csc_coef21;                  /* c98 */
    u_hdr_vdm_csc_coef22                 hdr_vdm_csc_coef22;                  /* c9c */
    u_hdr_vdm_csc_scale                  hdr_vdm_csc_scale;                   /* ca0 */
    u_hdr_vdm_csc_clip_min               hdr_vdm_csc_clip_min;                /* ca4 */
    u_hdr_vdm_csc_clip_max               hdr_vdm_csc_clip_max;                /* ca8 */
    u_hdr_vdm_csc_in_dc0                 hdr_vdm_csc_in_dc0;                  /* cac */
    u_hdr_vdm_csc_in_dc1                 hdr_vdm_csc_in_dc1;                  /* cb0 */
    u_hdr_vdm_csc_in_dc2                 hdr_vdm_csc_in_dc2;                  /* cb4 */
    u_hdr_vdm_csc_out_dc0                hdr_vdm_csc_out_dc0;                 /* cb8 */
    u_hdr_vdm_csc_out_dc1                hdr_vdm_csc_out_dc1;                 /* cbc */
    u_hdr_vdm_csc_out_dc2                hdr_vdm_csc_out_dc2;                 /* cc0 */
    unsigned int                         rsr23;                               /* cc4 */
    u_src2_yuv2rgb_coef00                src2_yuv2rgb_coef00;                 /* cc8 */
    u_src2_yuv2rgb_coef01                src2_yuv2rgb_coef01;                 /* ccc */
    u_src2_yuv2rgb_coef02                src2_yuv2rgb_coef02;                 /* cd0 */
    u_src2_yuv2rgb_coef03                src2_yuv2rgb_coef03;                 /* cd4 */
    u_src2_yuv2rgb_coef04                src2_yuv2rgb_coef04;                 /* cd8 */
    u_src2_yuv2rgb_coef05                src2_yuv2rgb_coef05;                 /* cdc */
    u_src2_yuv2rgb_coef06                src2_yuv2rgb_coef06;                 /* ce0 */
    u_src2_yuv2rgb_coef07                src2_yuv2rgb_coef07;                 /* ce4 */
    u_src2_yuv2rgb_coef08                src2_yuv2rgb_coef08;                 /* ce8 */
    u_src2_yuv2rgb_coef10                src2_yuv2rgb_coef10;                 /* cec */
    u_src2_yuv2rgb_coef11                src2_yuv2rgb_coef11;                 /* cf0 */
    u_src2_yuv2rgb_coef12                src2_yuv2rgb_coef12;                 /* cf4 */
    u_src2_yuv2rgb_coef13                src2_yuv2rgb_coef13;                 /* cf8 */
    u_src2_yuv2rgb_coef14                src2_yuv2rgb_coef14;                 /* cfc */
    u_src2_yuv2rgb_coef15                src2_yuv2rgb_coef15;                 /* d00 */
    u_src2_yuv2rgb_coef16                src2_yuv2rgb_coef16;                 /* d04 */
    u_src2_yuv2rgb_coef17                src2_yuv2rgb_coef17;                 /* d08 */
    u_src2_yuv2rgb_coef18                src2_yuv2rgb_coef18;                 /* d0c */
    u_src2_yuv2rgb_scale2p               src2_yuv2rgb_scale2p;                /* d10 */
    u_src2_yuv2rgb_in_dc00               src2_yuv2rgb_in_dc00;                /* d14 */
    u_src2_yuv2rgb_in_dc01               src2_yuv2rgb_in_dc01;                /* d18 */
    u_src2_yuv2rgb_in_dc02               src2_yuv2rgb_in_dc02;                /* d1c */
    u_src2_yuv2rgb_out_dc00              src2_yuv2rgb_out_dc00;               /* d20 */
    u_src2_yuv2rgb_out_dc01              src2_yuv2rgb_out_dc01;               /* d24 */
    u_src2_yuv2rgb_out_dc02              src2_yuv2rgb_out_dc02;               /* d28 */
    u_src2_yuv2rgb_in_dc10               src2_yuv2rgb_in_dc10;                /* d2c */
    u_src2_yuv2rgb_in_dc11               src2_yuv2rgb_in_dc11;                /* d30 */
    u_src2_yuv2rgb_in_dc12               src2_yuv2rgb_in_dc12;                /* d34 */
    u_src2_yuv2rgb_out_dc10              src2_yuv2rgb_out_dc10;               /* d38 */
    u_src2_yuv2rgb_out_dc11              src2_yuv2rgb_out_dc11;               /* d3c */
    u_src2_yuv2rgb_out_dc12              src2_yuv2rgb_out_dc12;               /* d40 */
    u_src2_yuv2rgb_min                   src2_yuv2rgb_min;                    /* d44 */
    u_src2_yuv2rgb_max                   src2_yuv2rgb_max;                    /* d48 */
    u_hdr_disp_0_dither_ctrl             hdr_disp_0_dither_ctrl;              /* d4c */
    u_hdr_disp_0_dither_ink_ctrl         hdr_disp_0_dither_ink_ctrl;          /* d50 */
    u_hdr_disp_0_dither_ink_pos          hdr_disp_0_dither_ink_pos;           /* d54 */
    u_hdr_local_lighting_cfg             hdr_local_lighting_cfg;              /* d58 */
    u_hdr_local_lighting_cx              hdr_local_lighting_cx;               /* d5c */
    u_hdr_local_lighting_cy              hdr_local_lighting_cy;               /* d60 */
    u_hdr_local_lighting_radius2         hdr_local_lighting_radius2;          /* d64 */
    u_hdr_local_lighting_amplitude       hdr_local_lighting_amplitude;        /* d68 */
    u_hdr_local_lighting_ampli_scale     hdr_local_lighting_ampli_scale;      /* d6c */
    u_hdr_local_lighting_start_angle_rad hdr_local_lighting_start_angle_rad;  /* d70 */
    u_hdr_local_lighting_start_end_angle_rad hdr_local_lighting_start_end_angle_rad; /* d74 */
    u_hdr_local_lighting_end_start_angle_rad hdr_local_lighting_end_start_angle_rad; /* d78 */
    u_hdr_local_lighting_end_angle_rad   hdr_local_lighting_end_angle_rad;    /* d7c */
    u_hdr_local_lighting_triwin2_coeff0  hdr_local_lighting_triwin2_coeff0;   /* d80 */
    u_hdr_local_lighting_triwin2_coeff1  hdr_local_lighting_triwin2_coeff1;   /* d84 */
    u_hdr_local_lighting_amb_amp         hdr_local_lighting_amb_amp;          /* d88 */
    u_hdr_local_lighting_bbox0_p0        hdr_local_lighting_bbox0_p0;         /* d8c */
    u_hdr_local_lighting_bbox0_p1        hdr_local_lighting_bbox0_p1;         /* d90 */
    u_hdr_local_lighting_bbox1_p0        hdr_local_lighting_bbox1_p0;         /* d94 */
    u_hdr_local_lighting_bbox1_p1        hdr_local_lighting_bbox1_p1;         /* d98 */
    u_hdr_local_lighting_img_reso        hdr_local_lighting_img_reso;         /* d9c */
    u_hdr_local_lighting_start_pos       hdr_local_lighting_start_pos;        /* da0 */
    u_hdr_local_lighting_adjust_value    hdr_local_lighting_adjust_value;     /* da4 */
    u_hdr_rcbn_ctrl                      hdr_rcbn_ctrl;                       /* da8 */
    u_hdr_rcbn_gmap_size_mode            hdr_rcbn_gmap_size_mode;             /* dac */
    u_hdr_rcbn_gmap_chn_num              hdr_rcbn_gmap_chn_num;               /* db0 */
    u_hdr_rcbn_gcoeff_rmin1              hdr_rcbn_gcoeff_rmin1;               /* db4 */
    u_hdr_rcbn_gcoeff_rmin2              hdr_rcbn_gcoeff_rmin2;               /* db8 */
    u_hdr_rcbn_gcoeff_rmax               hdr_rcbn_gcoeff_rmax;                /* dbc */
    u_hdr_rcbn_gcoeff_gmin1              hdr_rcbn_gcoeff_gmin1;               /* dc0 */
    u_hdr_rcbn_gcoeff_gmin2              hdr_rcbn_gcoeff_gmin2;               /* dc4 */
    u_hdr_rcbn_gcoeff_gmax               hdr_rcbn_gcoeff_gmax;                /* dc8 */
    u_hdr_rcbn_gcoeff_bmin1              hdr_rcbn_gcoeff_bmin1;               /* dcc */
    u_hdr_rcbn_gcoeff_bmin2              hdr_rcbn_gcoeff_bmin2;               /* dd0 */
    u_hdr_rcbn_gcoeff_bmax               hdr_rcbn_gcoeff_bmax;                /* dd4 */
    u_hdr_rcbn_kcoeff_r_alte             hdr_rcbn_kcoeff_r_alte;              /* dd8 */
    u_hdr_rcbn_kcoeff_r_base             hdr_rcbn_kcoeff_r_base;              /* ddc */
    u_hdr_rcbn_kcoeff_g_alte             hdr_rcbn_kcoeff_g_alte;              /* de0 */
    u_hdr_rcbn_kcoeff_g_base             hdr_rcbn_kcoeff_g_base;              /* de4 */
    u_hdr_rcbn_kcoeff_b_alte             hdr_rcbn_kcoeff_b_alte;              /* de8 */
    u_hdr_rcbn_kcoeff_b_base             hdr_rcbn_kcoeff_b_base;              /* dec */
    u_hdr_rcbn_weight                    hdr_rcbn_weight;                     /* df0 */
    u_hdr_rcbn_clip_min                  hdr_rcbn_clip_min;                   /* df4 */
    u_hdr_rcbn_clip_max                  hdr_rcbn_clip_max;                   /* df8 */
    u_hdr_vdm_csc1_ctrl                  hdr_vdm_csc1_ctrl;                   /* dfc */
    u_hdr_vdm_csc1_coef00                hdr_vdm_csc1_coef00;                 /* e00 */
    u_hdr_vdm_csc1_coef01                hdr_vdm_csc1_coef01;                 /* e04 */
    u_hdr_vdm_csc1_coef02                hdr_vdm_csc1_coef02;                 /* e08 */
    u_hdr_vdm_csc1_coef10                hdr_vdm_csc1_coef10;                 /* e0c */
    u_hdr_vdm_csc1_coef11                hdr_vdm_csc1_coef11;                 /* e10 */
    u_hdr_vdm_csc1_coef12                hdr_vdm_csc1_coef12;                 /* e14 */
    u_hdr_vdm_csc1_coef20                hdr_vdm_csc1_coef20;                 /* e18 */
    u_hdr_vdm_csc1_coef21                hdr_vdm_csc1_coef21;                 /* e1c */
    u_hdr_vdm_csc1_coef22                hdr_vdm_csc1_coef22;                 /* e20 */
    u_hdr_vdm_csc1_scale                 hdr_vdm_csc1_scale;                  /* e24 */
    u_hdr_vdm_csc1_clip_min              hdr_vdm_csc1_clip_min;               /* e28 */
    u_hdr_vdm_csc1_clip_max              hdr_vdm_csc1_clip_max;               /* e2c */
    u_hdr_vdm_csc1_in_dc0                hdr_vdm_csc1_in_dc0;                 /* e30 */
    u_hdr_vdm_csc1_in_dc1                hdr_vdm_csc1_in_dc1;                 /* e34 */
    u_hdr_vdm_csc1_in_dc2                hdr_vdm_csc1_in_dc2;                 /* e38 */
    u_hdr_vdm_csc1_out_dc0               hdr_vdm_csc1_out_dc0;                /* e3c */
    u_hdr_vdm_csc1_out_dc1               hdr_vdm_csc1_out_dc1;                /* e40 */
    u_hdr_vdm_csc1_out_dc2               hdr_vdm_csc1_out_dc2;                /* e44 */
    u_hdr_vdm_tmap_luma_coef0            hdr_vdm_tmap_luma_coef0;             /* e48 */
    u_hdr_vdm_tmap_luma_coef1            hdr_vdm_tmap_luma_coef1;             /* e4c */
    u_hdr_vdm_tmap_luma_coef2            hdr_vdm_tmap_luma_coef2;             /* e50 */
    u_hdr_vdm_tmap_luma_scale            hdr_vdm_tmap_luma_scale;             /* e54 */
    u_hdr_vdm_tmap_coef_scale            hdr_vdm_tmap_coef_scale;             /* e58 */
    u_hdr_vdm_tmap_out_clip_min          hdr_vdm_tmap_out_clip_min;           /* e5c */
    u_hdr_vdm_tmap_out_clip_max          hdr_vdm_tmap_out_clip_max;           /* e60 */
    u_hdr_vdm_tmap_out_dc0               hdr_vdm_tmap_out_dc0;                /* e64 */
    u_hdr_vdm_tmap_out_dc1               hdr_vdm_tmap_out_dc1;                /* e68 */
    u_hdr_vdm_tmap_out_dc2               hdr_vdm_tmap_out_dc2;                /* e6c */
    u_hdr_vdm_tonemap_alpha              hdr_vdm_tonemap_alpha;               /* e70 */
    u_hdr_disp_1_dither_ctrl             hdr_disp_1_dither_ctrl;              /* e74 */
    u_hdr_disp_1_dither_ink_ctrl         hdr_disp_1_dither_ink_ctrl;          /* e78 */
    u_hdr_disp_1_dither_ink_pos          hdr_disp_1_dither_ink_pos;           /* e7c */
} u_vau_mix_node4;

typedef union {
    u_vau_mix_node3 hdr_cbm;
    u_vau_mix_node4 hdr_combine;
} u_vau_mix_node_hdr;

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
    u_vau_model_ctrl                     vau_model_ctrl;                      /* 70 */
    u_vau_model_addr                     vau_model_addr;                      /* 74 */
    u_vau_model_stride                   vau_model_stride;                    /* 78 */
    u_vau_des_ctrl                       vau_des_ctrl;                        /* 7c */
    u_vau_des_addr                       vau_des_addr;                        /* 80 */
    u_vau_des_stride                     vau_des_stride;                      /* 84 */
    u_vau_des_imgsize                    vau_des_imgsize;                     /* 88 */
    u_vau_des_rect_xy                    vau_des_rect_xy;                     /* 8c */
    u_vau_des_rect_wh                    vau_des_rect_wh;                     /* 90 */
    u_vau_des_alpha                      vau_des_alpha;                       /* 94 */
    u_vau_des_htc_online_cmp0            vau_des_htc_online_cmp0;             /* 98 */
    u_vau_cbm_ctrl                       vau_cbm_ctrl;                        /* 9c */
    u_vau_cbm_colorize                   vau_cbm_colorize;                    /* a0 */
    u_vau_cbm_alu_para                   vau_cbm_alu_para;                    /* a4 */
    u_vau_cbm_key_para                   vau_cbm_key_para;                    /* a8 */
    u_vau_cbm_key_min                    vau_cbm_key_min;                     /* ac */
    u_vau_cbm_key_max                    vau_cbm_key_max;                     /* b0 */
    u_vau_cbm_key_mask                   vau_cbm_key_mask;                    /* b4 */
    u_vau_cbm_src1_cbm_para              vau_cbm_src1_cbm_para;               /* b8 */
    u_vau_cbm_src2_cbm_para              vau_cbm_src2_cbm_para;               /* bc */
    u_vau_gradient_typical_color         vau_gradient_typical_color;          /* c0 */
    u_vau_gradient_linear_const_x        vau_gradient_linear_const_x;         /* c4 */
    u_vau_gradient_linear_const_y        vau_gradient_linear_const_y;         /* c8 */
    u_vau_gradient_linear_start          vau_gradient_linear_start;           /* cc */
    u_vau_gradient_const_mode            vau_gradient_const_mode;             /* d0 */
    u_vau_gradient_radial_const_r        vau_gradient_radial_const_r;         /* d4 */
    u_vau_gradient_radial_center         vau_gradient_radial_center;          /* d8 */
    u_vau_gradient_sweep_center          vau_gradient_sweep_center;           /* dc */
    u_vau_gradient_sweep_start_angle     vau_gradient_sweep_start_angle;      /* e0 */
    u_vau_gradient_tile_mode             vau_gradient_tile_mode;              /* e4 */
    u_vau_gradient_stops_offset0         vau_gradient_stops_offset0;          /* e8 */
    u_vau_gradient_stops_offset1         vau_gradient_stops_offset1;          /* ec */
    u_vau_gradient_stops_color0          vau_gradient_stops_color[7];         /* f0~108 */
    u_vau_gradient_color_reci0           vau_gradient_color_reci[6];          /* 10c~120 */
    u_vau_gradient_stops_num             vau_gradient_stops_num;              /* 124 */
    u_local_lighting_cfg                 local_lighting_cfg;                  /* 128 */
    u_local_lighting_cx                  local_lighting_cx;                   /* 12c */
    u_local_lighting_cy                  local_lighting_cy;                   /* 130 */
    u_local_lighting_radius2             local_lighting_radius2;              /* 134 */
    u_local_lighting_amplitude           local_lighting_amplitude;            /* 138 */
    u_local_lighting_ampli_scale         local_lighting_ampli_scale;          /* 13c */
    u_local_lighting_start_angle_rad     local_lighting_start_angle_rad;      /* 140 */
    u_local_lighting_start_end_angle_rad local_lighting_start_end_angle_rad;  /* 144 */
    u_local_lighting_end_start_angle_rad local_lighting_end_start_angle_rad;  /* 148 */
    u_local_lighting_end_angle_rad       local_lighting_end_angle_rad;        /* 14c */
    u_local_lighting_triwin2_coeff0      local_lighting_triwin2_coeff0;       /* 150 */
    u_local_lighting_triwin2_coeff1      local_lighting_triwin2_coeff1;       /* 154 */
    u_local_lighting_amb_amp             local_lighting_amb_amp;              /* 158 */
    u_local_lighting_bbox0_p0            local_lighting_bbox0_p0;             /* 15c */
    u_local_lighting_bbox0_p1            local_lighting_bbox0_p1;             /* 160 */
    u_local_lighting_bbox1_p0            local_lighting_bbox1_p0;             /* 164 */
    u_local_lighting_bbox1_p1            local_lighting_bbox1_p1;             /* 168 */
    u_local_lighting_img_reso            local_lighting_img_reso;             /* 16c */
    u_local_lighting_start_pos           local_lighting_start_pos;            /* 170 */
    u_local_lighting_adjust_value        local_lighting_adjust_value;         /* 174 */
    u_gauss_blur_ctrl                    gauss_blur_ctrl;                     /* 178 */
    u_gauss_blur_a0                      gauss_blur_a0;                       /* 17c */
    u_gauss_blur_a1                      gauss_blur_a1;                       /* 180 */
    u_gauss_blur_a2                      gauss_blur_a2;                       /* 184 */
    u_gauss_blur_b_causala0              gauss_blur_b_causala0;               /* 188 */
    u_gauss_blur_b_causala1              gauss_blur_b_causala1;               /* 18c */
    u_gauss_blur_b_causala2              gauss_blur_b_causala2;               /* 190 */
    u_gauss_blur_b_anticausala0          gauss_blur_b_anticausala0;           /* 194 */
    u_gauss_blur_b_anticausala1          gauss_blur_b_anticausala1;           /* 198 */
    u_gauss_blur_b_anticausala2          gauss_blur_b_anticausala2;           /* 19c */
    u_gauss_blur_causala_ratio           gauss_blur_causala_ratio;            /* 1a0 */
    u_gauss_blur_anticausala_ratio       gauss_blur_anticausala_ratio;        /* 1a4 */
    u_vau_src1_t2r_ctrl                  vau_src1_t2r_ctrl;                   /* 1a8 */
    u_vau_src2_t2r_ctrl                  vau_src2_t2r_ctrl;                   /* 1ac */
    u_vau_des_r2t_ctrl                   vau_des_r2t_ctrl;                    /* 1b0 */
    u_vau_clut_addr                      vau_clut_addr;                       /* 1b4 */
    u_tde_src2_zme_lhaddr                tde_src2_zme_lhaddr;                 /* 1b8 */
    u_tde_src2_zme_lvaddr                tde_src2_zme_lvaddr;                 /* 1bc */
    u_tde_src2_zme_chaddr                tde_src2_zme_chaddr;                 /* 1c0 */
    u_tde_src2_zme_cvaddr                tde_src2_zme_cvaddr;                 /* 1c4 */
    u_vau_node_cfg                       vau_node_cfg;                        /* 1c8 */
    u_vau_node_id                        vau_node_id;                         /* 1cc */
    u_vau_intmask                        vau_intmask;                         /* 1d0 */
    u_vau_pnext                          vau_pnext;                           /* 1d4 */
    u_vau_hdr_sys_ctrl                   vau_hdr_sys_ctrl;                    /* 1d8 */
    u_vau_hdr_sys_ctrl1                  vau_hdr_sys_ctrl1;                   /* 1dc */
    u_vau_src1_gmmu_paddr                vau_src1_gmmu_paddr;                 /* 1e0 */
    u_vau_src2_gmmu_paddr                vau_src2_gmmu_paddr;                 /* 1e4 */
    u_vau_des_gmmu_paddr                 vau_des_gmmu_paddr;                  /* 1e8 */
    u_vau_fpga_tag                       vau_fpga_tag;                        /* 1ec */
    unsigned int                         rsr0[4];                             /* 1f0 ~ 1fc */
    u_vau_mix_node                       mix;                                 /* 200 ~ 27c */
    u_vau_mix_node_hdr                   hdr;                                 /* 280 ~ 4F0 */
} vau_hdr_hardware_node;
#else
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
    unsigned int                         rsr31[2];                            /* fd8 ~ fdc */
} vau_hdr_hardware_node;
#endif

typedef struct {
    vau_hdr_hardware_node hw_node;
    td_handle handle;
    td_u32 phy_addr;
    struct osal_list_head list_head;
} vau_hdr_sw_node;

td_void vau_hal_set_hdr_sys_ctrl(vau_hdr_hardware_node *node, td_bool hdr_en, td_bool hdr_cbm_en, td_bool is_hdr_des);
td_void vau_hal_set_hdr_disp1_dither(vau_hdr_hardware_node *node, td_bool hdr_en);
td_void vau_hal_set_hdr_src1(vau_hdr_hardware_node *node, const drv_vau_surface *surface);
td_void vau_hal_set_hdr_src2(vau_hdr_hardware_node *node, const drv_vau_surface *surface);
td_void vau_hal_set_hdr_dst(vau_hdr_hardware_node *node, const drv_vau_surface *surface);
td_void vau_hal_set_hdr_hipp_csc(vau_hdr_hardware_node *node, const drv_vau_surface *surface,
    const hdr_csc_cfg *y2r_gainmap_cfg);
td_void vau_hal_set_hdr_cvfir(vau_hdr_hardware_node *node, const drv_vau_surface *surface);
td_void vau_hal_set_hdr_hfir(vau_hdr_hardware_node *node, const drv_vau_surface *surface);
td_void vau_hal_set_hdr_cvfir2(vau_hdr_hardware_node *node, const drv_vau_surface *surface);
td_void vau_hal_set_hdr_hfir2(vau_hdr_hardware_node *node);
td_void vau_hal_set_hdr_vdm(vau_hdr_hardware_node *node, const drv_hdr_cfg *hdr_cfg);
td_void vau_hal_set_hdr_rcbn(vau_hdr_hardware_node *node, const hdr_rcbn_cfg *combine_cfg);
td_void vau_hal_set_hdr_lighting(vau_hdr_hardware_node *node, const drv_vau_lighting_opt *lighting_opt,
    const drv_vau_surface *src2);
td_void vau_hal_set_hdr_vhdaccstat(vau_hdr_hardware_node *node, const drv_vau_surface *surface);
td_void vau_hal_set_hdr_cbm(vau_hdr_hardware_node *node, const td_bool sdr_en, const td_bool hdr_en);
td_void vau_hal_set_hdr_ctadj(vau_hdr_hardware_node *node, const hdr_scale_cfg *scale_cfg);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
