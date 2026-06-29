/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: sdf reg define file
 */

#ifndef _VAU_SDF_REG_DEFINE_H_
#define _VAU_SDF_REG_DEFINE_H_

#include <stdio.h>
#include "vau_reg_define.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* Define the union vau_sdf_prim_para0 */
typedef union {
    /* Define the struct bits */
    struct {
        float prim_rotate_matrix0;      /* [31:8] valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_sdf_prim_para0;

/* Define the union vau_sdf_prim_para1 */
typedef union {
    /* Define the struct bits */
    struct {
        float prim_rotate_matrix1;      /* [31:8] valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_sdf_prim_para1;

/* Define the union vau_sdf_prim_para2 */
typedef union {
    /* Define the struct bits */
    struct {
        float prim_rotate_matrix2;      /* [31:8] valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_sdf_prim_para2;

/* Define the union vau_sdf_prim_para3 */
typedef union {
    /* Define the struct bits */
    struct {
        float prim_rotate_matrix3;      /* [31:8] valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_sdf_prim_para3;

/* Define the union vau_sdf_prim_para4 */
typedef union {
    /* Define the struct bits */
    struct {
        float prim_rotate_matrix4;      /* [31:8] valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_sdf_prim_para4;

/* Define the union vau_sdf_prim_para5 */
typedef union {
    /* Define the struct bits */
    struct {
        float prim_rotate_matrix5;      /* [31:8] valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_sdf_prim_para5;

/* Define the union vau_sdf_prim_para6 */
typedef union {
    /* Define the struct bits */
    struct {
        float prim_rotate_matrix6;      /* [31:8] valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_sdf_prim_para6;

/* Define the union vau_sdf_prim_para7 */
typedef union {
    /* Define the struct bits */
    struct {
        float prim_rotate_matrix7;      /* [31:8] valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_sdf_prim_para7;

/* Define the union vau_sdf_prim_para8 */
typedef union {
    /* Define the struct bits */
    struct {
        float prim_rotate_matrix8;      /* [31:8] valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_sdf_prim_para8;

/* Define the union vau_sdf_prim_para9 */
typedef union {
    /* Define the struct bits */
    struct {
        float bool_threshold0;      /* [31:8] valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_sdf_prim_para9;

/* Define the union vau_sdf_prim_para10 */
typedef union {
    /* Define the struct bits */
    struct {
        float bool_threshold1;      /* [31:8] valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_sdf_prim_para10;

/* Define the union vau_sdf_prim_para11 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int prim_center0 : 16;      /* [15:0]  f16 */
        unsigned int prim_center1 : 16;      /* [31:16] f16 */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_sdf_prim_para11;

/* Define the union vau_sdf_prim_para12 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int prim_center2 : 16;      /* [15:0]  f16 */
        unsigned int prim_rot_center_scale0 : 16;      /* [31:16] f16 */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_sdf_prim_para12;

/* Define the union vau_sdf_prim_para13 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int prim_rot_center_scale1 : 16;      /* [15:0]  f16 */
        unsigned int prim_rot_center_scale2 : 16;      /* [31:16] f16 */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_sdf_prim_para13;

/* Define the union vau_sdf_prim_para14 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int prim_len_xyz0 : 16;      /* [15:0]  f16 */
        unsigned int prim_len_xyz1 : 16;      /* [31:16] f16 */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_sdf_prim_para14;

/* Define the union vau_sdf_prim_para15 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int prim_len_xyz2 : 16;      /* [15:0]  f16 */
        unsigned int prim_chamfer0 : 16;      /* [31:16] f16 */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_sdf_prim_para15;

/* Define the union vau_sdf_prim_para16 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int prim_chamfer1 : 16;      /* [15:0]  f16 */
        unsigned int prim_chamfer2 : 16;      /* [31:16] f16 */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_sdf_prim_para16;

/* Define the union vau_sdf_prim_para17 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int prim_hightlight : 10;      /* [9:0] */
        unsigned int prim_meta_rough0 : 10;      /* [19:10] */
        unsigned int prim_meta_rough1 : 10;      /* [29:20] */
        unsigned int reserved : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_sdf_prim_para17;

/* Define the union vau_sdf_prim_para18 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int prim_color0 : 8;       /* [7:0] */
        unsigned int prim_color1 : 8;       /* [15:8] */
        unsigned int prim_color2 : 8;       /* [23:16] */
        unsigned int reserved : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_sdf_prim_para18;

/* Define the union vau_sdf_prim_para19 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int prim_first_bracket_id : 6;       /* [5:0] */
        unsigned int reserved0 : 2;       /* [7:6] */
        unsigned int prim_bracket_status : 6;       /* [13:8] */
        unsigned int reserved1 : 2;       /* [15:14] */
        unsigned int prim_tex_id : 3;       /* [18:16] */
        unsigned int reserved2 : 1;       /* [19] */
        unsigned int prim_matrix_status : 1;       /* [20] */
        unsigned int bool_scale0 : 1;       /* [21] */
        unsigned int bool_scale1 : 1;       /* [22] */
        unsigned int bool_scale2 : 1;       /* [23] */
        unsigned int prim_mirror0 : 1;       /* [24] */
        unsigned int prim_mirror1 : 1;       /* [25] */
        unsigned int prim_mirror2 : 1;       /* [26] */
        unsigned int reserved3 : 5;       /* [31:27] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_sdf_prim_para19;

/* Define the union vau_sdf_prim_para20 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int prim_rot_cen0 : 16;      /* [15:0]  f16 */
        unsigned int prim_rot_cen1 : 16;      /* [31:16] f16 */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_sdf_prim_para20;

/* Define the union vau_sdf_prim_para21 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int prim_rot_cen2 : 16;      /* [15:0]  f16 */
        unsigned int prim_scale0 : 16;       /* [31:16] f16 */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_sdf_prim_para21;

/* Define the union vau_sdf_prim_para22 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int prim_scale1 : 16;      /* [15:0]  f16 */
        unsigned int prim_scale2 : 16;      /* [31:16] f16 */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_sdf_prim_para22;

/* Define the union vau_sdf_diff_prim_mask */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int diff_prim_mask : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_sdf_prim_para23;

typedef struct {
    u_vau_sdf_prim_para0                 para0;      /* f24 */
    u_vau_sdf_prim_para1                 para1;      /* f24 */
    u_vau_sdf_prim_para2                 para2;      /* f24 */
    u_vau_sdf_prim_para3                 para3;      /* f24 */
    u_vau_sdf_prim_para4                 para4;      /* f24 */
    u_vau_sdf_prim_para5                 para5;      /* f24 */
    u_vau_sdf_prim_para6                 para6;      /* f24 */
    u_vau_sdf_prim_para7                 para7;      /* f24 */
    u_vau_sdf_prim_para8                 para8;      /* f24 */
    u_vau_sdf_prim_para9                 para9;      /* f24 */
    u_vau_sdf_prim_para10                para10;     /* f24 */
    u_vau_sdf_prim_para11                para11;     /* f16 */
    u_vau_sdf_prim_para12                para12;     /* f16 */
    u_vau_sdf_prim_para13                para13;     /* f16 */
    u_vau_sdf_prim_para14                para14;     /* f16 */
    u_vau_sdf_prim_para15                para15;     /* f16 */
    u_vau_sdf_prim_para16                para16;     /* f16 */
    u_vau_sdf_prim_para17                para17;     /* int */
    u_vau_sdf_prim_para18                para18;     /* int */
    u_vau_sdf_prim_para19                para19;     /* int */
} vau_sdf_prim_paras;

typedef struct {
    // add prim
    u_vau_sdf_prim_para20                para20;     /* int */
    u_vau_sdf_prim_para21                para21;     /* int */
    u_vau_sdf_prim_para22                para22;     /* int */
    u_vau_sdf_prim_para23                para23;     /* int */
} vau_sdf_add_prim_paras;

typedef struct {
    u_vau_sdf_ctrl                       sdf_ctrl;                        /* f80 */
    u_vau_sdf_ctrl0                      sdf_ctrl0;                       /* f84 */
    u_vau_sdf_ctrl1                      sdf_ctrl1;                       /* f88 */
    u_vau_sdf_ctrl2                      sdf_ctrl2;                       /* f8c */
    u_vau_sdf_ctrl3                      sdf_ctrl3;                       /* f90 */
    u_vau_sdf_shad_vld_mask0             sdf_shad_vld_mask0;              /* f94 */
    u_vau_sdf_shad_vld_mask1             sdf_shad_vld_mask1;              /* f98 */
    u_vau_sdf_glb_shad_mask0             sdf_glb_shad_mask0;              /* f9c */
    u_vau_sdf_glb_shad_mask1             sdf_glb_shad_mask1;              /* fa0 */
    u_vau_sdf_glb_fg_mask0               sdf_glb_fg_mask0;                /* fa4 */
    u_vau_sdf_glb_fg_mask1               sdf_glb_fg_mask1;                /* fa8 */
    u_vau_sdf_ssaa_grad_thd              sdf_ssaa_grad_thd;               /* fac */
    u_vau_sdf_ssaa_offset0               sdf_ssaa_offset0;                /* fb0 */
    u_vau_sdf_ssaa_offset1               sdf_ssaa_offset1;                /* fb4 */
    u_vau_sdf_ssaa_offset2               sdf_ssaa_offset2;                /* fb8 */
    u_vau_sdf_ssaa_offset3               sdf_ssaa_offset3;                /* fbc */
    u_vau_sdf_shape_eps                  sdf_shape_eps;                   /* fc0 */
    u_vau_sdf_shad_eps                   sdf_shad_eps;                    /* fc4 */
    u_vau_sdf_shad_alpha                 sdf_shad_alpha;                  /* fc8 */
    u_vau_sdf_shad_grd_h                 sdf_shad_grd_h;                  /* fcc */
    u_vau_sdf_shad_grd_x                 sdf_shad_grd_x;                  /* fd0 */
    u_vau_sdf_shad_grd_y                 sdf_shad_grd_y;                  /* fd4 */
    u_vau_sdf_shad_grd_z                 sdf_shad_grd_z;                  /* fd8 */
    u_vau_sdf_norm_x0                    sdf_norm_x0;                     /* fdc */
    u_vau_sdf_norm_x1                    sdf_norm_x1;                     /* fe0 */
    u_vau_sdf_norm_x2                    sdf_norm_x2;                     /* fe4 */
    u_vau_sdf_norm_x3                    sdf_norm_x3;                     /* fe8 */
    u_vau_sdf_norm_y0                    sdf_norm_y0;                     /* fec */
    u_vau_sdf_norm_y1                    sdf_norm_y1;                     /* ff0 */
    u_vau_sdf_norm_y2                    sdf_norm_y2;                     /* ff4 */
    u_vau_sdf_norm_y3                    sdf_norm_y3;                     /* ff8 */
    u_vau_sdf_norm_z0                    sdf_norm_z0;                     /* ffc */
    u_vau_sdf_norm_z1                    sdf_norm_z1;                     /* 1000 */
    u_vau_sdf_norm_z2                    sdf_norm_z2;                     /* 1004 */
    u_vau_sdf_norm_z3                    sdf_norm_z3;                     /* 1008 */
    u_vau_sdf_lgt_mtl_coef0              sdf_lgt_mtl_coef0;               /* 100c */
    u_vau_sdf_lgt_mtl_coef1              sdf_lgt_mtl_coef1;               /* 1010 */
    u_vau_sdf_lgt_mtl_coef2              sdf_lgt_mtl_coef2;               /* 1014 */
    u_vau_sdf_lgt_mtl_coef3              sdf_lgt_mtl_coef3;               /* 1018 */
    u_vau_sdf_tex_wh0                    sdf_tex_wh0;                     /* 101c */
    u_vau_sdf_tex_wh1                    sdf_tex_wh1;                     /* 1020 */
    u_vau_sdf_tex_wh2                    sdf_tex_wh2;                     /* 1024 */
    u_vau_sdf_tex_wh3                    sdf_tex_wh3;                     /* 1028 */
    u_vau_sdf_tex_uv_crop0               sdf_tex_uv_crop0;                /* 102c */
    u_vau_sdf_tex_uv_crop1               sdf_tex_uv_crop1;                /* 1030 */
    u_vau_sdf_tex_uv_crop2               sdf_tex_uv_crop2;                /* 1034 */
    u_vau_sdf_tex_uv_crop3               sdf_tex_uv_crop3;                /* 1038 */
    u_vau_sdf_tex_uv_crop4               sdf_tex_uv_crop4;                /* 103c */
    u_vau_sdf_tex_uv_crop5               sdf_tex_uv_crop5;                /* 1040 */
    u_vau_sdf_tex_uv_crop6               sdf_tex_uv_crop6;                /* 1044 */
    u_vau_sdf_tex_uv_crop7               sdf_tex_uv_crop7;                /* 1048 */
    u_vau_sdf_tex_uv_crop8               sdf_tex_uv_crop8;                /* 104c */
    u_vau_sdf_tex_uv_crop9               sdf_tex_uv_crop9;                /* 1050 */
    u_vau_sdf_tex_uv_crop10              sdf_tex_uv_crop10;               /* 1054 */
    u_vau_sdf_tex_uv_crop11              sdf_tex_uv_crop11;               /* 1058 */
    u_vau_sdf_tex_uv_crop12              sdf_tex_uv_crop12;               /* 105c */
    u_vau_sdf_tex_uv_crop13              sdf_tex_uv_crop13;               /* 1060 */
    u_vau_sdf_tex_uv_crop14              sdf_tex_uv_crop14;               /* 1064 */
    u_vau_sdf_tex_uv_crop15              sdf_tex_uv_crop15;               /* 1068 */
    u_vau_sdf_tex_uv_crop_inv0           sdf_tex_uv_crop_inv0;            /* 106c */
    u_vau_sdf_tex_uv_crop_inv1           sdf_tex_uv_crop_inv1;            /* 1070 */
    u_vau_sdf_tex_uv_crop_inv2           sdf_tex_uv_crop_inv2;            /* 1074 */
    u_vau_sdf_tex_uv_crop_inv3           sdf_tex_uv_crop_inv3;            /* 1078 */
    u_vau_sdf_tex_uv_crop_inv4           sdf_tex_uv_crop_inv4;            /* 107c */
    u_vau_sdf_tex_uv_crop_inv5           sdf_tex_uv_crop_inv5;            /* 1080 */
    u_vau_sdf_tex_uv_crop_inv6           sdf_tex_uv_crop_inv6;            /* 1084 */
    u_vau_sdf_tex_uv_crop_inv7           sdf_tex_uv_crop_inv7;            /* 1088 */
    u_vau_sdf_tex_mask_00                sdf_tex_mask_00;                 /* 108c */
    u_vau_sdf_tex_mask_01                sdf_tex_mask_01;                 /* 1090 */
    u_vau_sdf_tex_mask_10                sdf_tex_mask_10;                 /* 1094 */
    u_vau_sdf_tex_mask_11                sdf_tex_mask_11;                 /* 1098 */
    u_vau_sdf_tex_mask_20                sdf_tex_mask_20;                 /* 109c */
    u_vau_sdf_tex_mask_21                sdf_tex_mask_21;                 /* 10a0 */
    u_vau_sdf_tex_mask_30                sdf_tex_mask_30;                 /* 10a4 */
    u_vau_sdf_tex_mask_31                sdf_tex_mask_31;                 /* 10a8 */
    u_vau_sdf_tex_rept0                  sdf_tex_rept0;                   /* 10ac */
    u_vau_sdf_tex_rept1                  sdf_tex_rept1;                   /* 10b0 */
    u_vau_sdf_reserve_0                  sdf_reserve_0;                   /* 10b4 */
} vau_sdf_global_config;

typedef struct {
    u_vau_sdf_tex_st_x0                  sdf_tex_st_x0;                   /* 1200 */
    u_vau_sdf_tex_st_x1                  sdf_tex_st_x1;                   /* 1204 */
    u_vau_sdf_tex_st_y0                  sdf_tex_st_y0;                   /* 1208 */
    u_vau_sdf_tex_st_y1                  sdf_tex_st_y1;                   /* 120c */
    u_vau_sdf_tex_st_z0                  sdf_tex_st_z0;                   /* 1210 */
    u_vau_sdf_tex_st_z1                  sdf_tex_st_z1;                   /* 1214 */
    u_vau_sdf_tex_ed_x0                  sdf_tex_ed_x0;                   /* 1218 */
    u_vau_sdf_tex_ed_x1                  sdf_tex_ed_x1;                   /* 121c */
    u_vau_sdf_tex_ed_y0                  sdf_tex_ed_y0;                   /* 1220 */
    u_vau_sdf_tex_ed_y1                  sdf_tex_ed_y1;                   /* 1224 */
    u_vau_sdf_tex_ed_z0                  sdf_tex_ed_z0;                   /* 1228 */
    u_vau_sdf_tex_ed_z1                  sdf_tex_ed_z1;                   /* 122c */
    u_vau_sdf_tex_2d_st_x0               sdf_tex_2d_st_x0;                /* 1230 */
    u_vau_sdf_tex_2d_st_x1               sdf_tex_2d_st_x1;                /* 1234 */
    u_vau_sdf_tex_2d_st_y0               sdf_tex_2d_st_y0;                /* 1238 */
    u_vau_sdf_tex_2d_st_y1               sdf_tex_2d_st_y1;                /* 123c */
    u_vau_sdf_tex_2d_ed_x0               sdf_tex_2d_ed_x0;                /* 1240 */
    u_vau_sdf_tex_2d_ed_x1               sdf_tex_2d_ed_x1;                /* 1244 */
    u_vau_sdf_tex_2d_ed_y0               sdf_tex_2d_ed_y0;                /* 1248 */
    u_vau_sdf_tex_2d_ed_y1               sdf_tex_2d_ed_y1;                /* 124c */
    u_vau_sdf_3d_bb_mask0_0              sdf_3d_bb_mask0_0;               /* 1250 */
    u_vau_sdf_3d_bb_mask0_1              sdf_3d_bb_mask0_1;               /* 1254 */
    u_vau_sdf_3d_bb_mask1_0              sdf_3d_bb_mask1_0;               /* 1258 */
    u_vau_sdf_3d_bb_mask1_1              sdf_3d_bb_mask1_1;               /* 125c */
    u_vau_sdf_3d_bb_mask2_0              sdf_3d_bb_mask2_0;               /* 1260 */
    u_vau_sdf_3d_bb_mask2_1              sdf_3d_bb_mask2_1;               /* 1264 */
    u_vau_sdf_3d_bb_mask3_0              sdf_3d_bb_mask3_0;               /* 1268 */
    u_vau_sdf_3d_bb_mask3_1              sdf_3d_bb_mask3_1;               /* 126c */
    u_vau_sdf_3d_bb_mask4_0              sdf_3d_bb_mask4_0;               /* 1270 */
    u_vau_sdf_3d_bb_mask4_1              sdf_3d_bb_mask4_1;               /* 1274 */
    u_vau_sdf_3d_bb_mask5_0              sdf_3d_bb_mask5_0;               /* 1278 */
    u_vau_sdf_3d_bb_mask5_1              sdf_3d_bb_mask5_1;               /* 127c */
    u_vau_sdf_3d_bb_mask6_0              sdf_3d_bb_mask6_0;               /* 1280 */
    u_vau_sdf_3d_bb_mask6_1              sdf_3d_bb_mask6_1;               /* 1284 */
    u_vau_sdf_3d_bb_mask7_0              sdf_3d_bb_mask7_0;               /* 1288 */
    u_vau_sdf_3d_bb_mask7_1              sdf_3d_bb_mask7_1;               /* 128c */
    u_vau_sdf_3d_bb_mask8_0              sdf_3d_bb_mask8_0;               /* 1290 */
    u_vau_sdf_3d_bb_mask8_1              sdf_3d_bb_mask8_1;               /* 1294 */
    u_vau_sdf_3d_bb_mask9_0              sdf_3d_bb_mask9_0;               /* 1298 */
    u_vau_sdf_3d_bb_mask9_1              sdf_3d_bb_mask9_1;               /* 129c */
    u_vau_sdf_3d_bb_mask10_0             sdf_3d_bb_mask10_0;              /* 12a0 */
    u_vau_sdf_3d_bb_mask10_1             sdf_3d_bb_mask10_1;              /* 12a4 */
    u_vau_sdf_3d_bb_mask11_0             sdf_3d_bb_mask11_0;              /* 12a8 */
    u_vau_sdf_3d_bb_mask11_1             sdf_3d_bb_mask11_1;              /* 12ac */
    u_vau_sdf_3d_bb_mask12_0             sdf_3d_bb_mask12_0;              /* 12b0 */
    u_vau_sdf_3d_bb_mask12_1             sdf_3d_bb_mask12_1;              /* 12b4 */
    u_vau_sdf_3d_bb_mask13_0             sdf_3d_bb_mask13_0;              /* 12b8 */
    u_vau_sdf_3d_bb_mask13_1             sdf_3d_bb_mask13_1;              /* 12bc */
    u_vau_sdf_3d_bb_mask14_0             sdf_3d_bb_mask14_0;              /* 12c0 */
    u_vau_sdf_3d_bb_mask14_1             sdf_3d_bb_mask14_1;              /* 12c4 */
    u_vau_sdf_3d_bb_mask15_0             sdf_3d_bb_mask15_0;              /* 12c8 */
    u_vau_sdf_3d_bb_mask15_1             sdf_3d_bb_mask15_1;              /* 12cc */
    u_vau_sdf_3d_bb_st_x01               sdf_3d_bb_st_x01;                /* 12d0 */
    u_vau_sdf_3d_bb_st_x23               sdf_3d_bb_st_x23;                /* 12d4 */
    u_vau_sdf_3d_bb_st_x45               sdf_3d_bb_st_x45;                /* 12d8 */
    u_vau_sdf_3d_bb_st_x67               sdf_3d_bb_st_x67;                /* 12dc */
    u_vau_sdf_3d_bb_st_x89               sdf_3d_bb_st_x89;                /* 12e0 */
    u_vau_sdf_3d_bb_st_x1011             sdf_3d_bb_st_x1011;              /* 12e4 */
    u_vau_sdf_3d_bb_st_x1213             sdf_3d_bb_st_x1213;              /* 12e8 */
    u_vau_sdf_3d_bb_st_x1415             sdf_3d_bb_st_x1415;              /* 12ec */
    u_vau_sdf_3d_bb_st_y01               sdf_3d_bb_st_y01;                /* 12f0 */
    u_vau_sdf_3d_bb_st_y23               sdf_3d_bb_st_y23;                /* 12f4 */
    u_vau_sdf_3d_bb_st_y45               sdf_3d_bb_st_y45;                /* 12f8 */
    u_vau_sdf_3d_bb_st_y67               sdf_3d_bb_st_y67;                /* 12fc */
    u_vau_sdf_3d_bb_st_y89               sdf_3d_bb_st_y89;                /* 1300 */
    u_vau_sdf_3d_bb_st_y1011             sdf_3d_bb_st_y1011;              /* 1304 */
    u_vau_sdf_3d_bb_st_y1213             sdf_3d_bb_st_y1213;              /* 1308 */
    u_vau_sdf_3d_bb_st_y1415             sdf_3d_bb_st_y1415;              /* 130c */
    u_vau_sdf_3d_bb_st_z01               sdf_3d_bb_st_z01;                /* 1310 */
    u_vau_sdf_3d_bb_st_z23               sdf_3d_bb_st_z23;                /* 1314 */
    u_vau_sdf_3d_bb_st_z45               sdf_3d_bb_st_z45;                /* 1318 */
    u_vau_sdf_3d_bb_st_z67               sdf_3d_bb_st_z67;                /* 131c */
    u_vau_sdf_3d_bb_st_z89               sdf_3d_bb_st_z89;                /* 1320 */
    u_vau_sdf_3d_bb_st_z1011             sdf_3d_bb_st_z1011;              /* 1324 */
    u_vau_sdf_3d_bb_st_z1213             sdf_3d_bb_st_z1213;              /* 1328 */
    u_vau_sdf_3d_bb_st_z1415             sdf_3d_bb_st_z1415;              /* 132c */
    u_vau_sdf_3d_bb_ed_x01               sdf_3d_bb_ed_x01;                /* 1330 */
    u_vau_sdf_3d_bb_ed_x23               sdf_3d_bb_ed_x23;                /* 1334 */
    u_vau_sdf_3d_bb_ed_x45               sdf_3d_bb_ed_x45;                /* 1338 */
    u_vau_sdf_3d_bb_ed_x67               sdf_3d_bb_ed_x67;                /* 133c */
    u_vau_sdf_3d_bb_ed_x89               sdf_3d_bb_ed_x89;                /* 1340 */
    u_vau_sdf_3d_bb_ed_x1011             sdf_3d_bb_ed_x1011;              /* 1344 */
    u_vau_sdf_3d_bb_ed_x1213             sdf_3d_bb_ed_x1213;              /* 1348 */
    u_vau_sdf_3d_bb_ed_x1415             sdf_3d_bb_ed_x1415;              /* 134c */
    u_vau_sdf_3d_bb_ed_y01               sdf_3d_bb_ed_y01;                /* 1350 */
    u_vau_sdf_3d_bb_ed_y23               sdf_3d_bb_ed_y23;                /* 1354 */
    u_vau_sdf_3d_bb_ed_y45               sdf_3d_bb_ed_y45;                /* 1358 */
    u_vau_sdf_3d_bb_ed_y67               sdf_3d_bb_ed_y67;                /* 135c */
    u_vau_sdf_3d_bb_ed_y89               sdf_3d_bb_ed_y89;                /* 1360 */
    u_vau_sdf_3d_bb_ed_y1011             sdf_3d_bb_ed_y1011;              /* 1364 */
    u_vau_sdf_3d_bb_ed_y1213             sdf_3d_bb_ed_y1213;              /* 1368 */
    u_vau_sdf_3d_bb_ed_y1415             sdf_3d_bb_ed_y1415;              /* 136c */
    u_vau_sdf_3d_bb_ed_z01               sdf_3d_bb_ed_z01;                /* 1370 */
    u_vau_sdf_3d_bb_ed_z23               sdf_3d_bb_ed_z23;                /* 1374 */
    u_vau_sdf_3d_bb_ed_z45               sdf_3d_bb_ed_z45;                /* 1378 */
    u_vau_sdf_3d_bb_ed_z67               sdf_3d_bb_ed_z67;                /* 137c */
    u_vau_sdf_3d_bb_ed_z89               sdf_3d_bb_ed_z89;                /* 1380 */
    u_vau_sdf_3d_bb_ed_z1011             sdf_3d_bb_ed_z1011;              /* 1384 */
    u_vau_sdf_3d_bb_ed_z1213             sdf_3d_bb_ed_z1213;              /* 1388 */
    u_vau_sdf_3d_bb_ed_z1415             sdf_3d_bb_ed_z1415;              /* 138c */
    u_vau_sdf_2d_bb_uv_st                sdf_2d_bb_uv_st;                 /* 1390 */
    u_vau_sdf_2d_bb_uv_ed                sdf_2d_bb_uv_ed;                 /* 1394 */
    u_vau_sdf_2d_bb_shad_uv_st           sdf_2d_bb_shad_uv_st;            /* 1398 */
    u_vau_sdf_2d_bb_shad_uv_ed           sdf_2d_bb_shad_uv_ed;            /* 139c */
    u_vau_sdf_lgt_ao_0                   sdf_lgt_ao_0;                    /* 13a0 */
    u_vau_sdf_lgt_ao_1                   sdf_lgt_ao_1;                    /* 13a4 */
    u_vau_sdf_self_lgt                   sdf_self_lgt;                    /* 13a8 */
    u_vau_sdf_lgt_power_01               sdf_lgt_power_01;                /* 13ac */
    u_vau_sdf_lgt_power_23               sdf_lgt_power_23;                /* 13b0 */
    u_vau_sdf_lgt_color_0                sdf_lgt_color_0;                 /* 13b4 */
    u_vau_sdf_lgt_color_1                sdf_lgt_color_1;                 /* 13b8 */
    u_vau_sdf_lgt_color_2                sdf_lgt_color_2;                 /* 13bc */
    u_vau_sdf_lgt_color_3                sdf_lgt_color_3;                 /* 13c0 */
    u_vau_sdf_lgt_color_4                sdf_lgt_color_4;                 /* 13c4 */
    u_vau_sdf_lgt_color_5                sdf_lgt_color_5;                 /* 13c8 */
    u_vau_sdf_lgt_color_6                sdf_lgt_color_6;                 /* 13cc */
    u_vau_sdf_lgt_color_7                sdf_lgt_color_7;                 /* 13d0 */
    u_vau_sdf_lgt_color_8                sdf_lgt_color_8;                 /* 13d4 */
    u_vau_sdf_lgt_color_9                sdf_lgt_color_9;                 /* 13d8 */
    u_vau_sdf_lgt_color_10               sdf_lgt_color_10;                /* 13dc */
    u_vau_sdf_lgt_color_11               sdf_lgt_color_11;                /* 13e0 */
    u_vau_sdf_lgt_pos_01                 sdf_lgt_pos_01;                  /* 13e4 */
    u_vau_sdf_lgt_pos_23                 sdf_lgt_pos_23;                  /* 13e8 */
    u_vau_sdf_lgt_pos_45                 sdf_lgt_pos_45;                  /* 13ec */
    u_vau_sdf_lgt_pos_67                 sdf_lgt_pos_67;                  /* 13f0 */
    u_vau_sdf_lgt_pos_89                 sdf_lgt_pos_89;                  /* 13f4 */
    u_vau_sdf_lgt_pos_1011               sdf_lgt_pos_1011;                /* 13f8 */
    u_vau_sdf_ro_x                       sdf_ro_x;                        /* 13fc */
    u_vau_sdf_ro_y                       sdf_ro_y;                        /* 1400 */
    u_vau_sdf_ro_z                       sdf_ro_z;                        /* 1404 */
    u_vau_sdf_ro_dot                     sdf_ro_dot;                      /* 1408 */
    u_vau_sdf_start_loc_x                sdf_start_loc_x;                 /* 140c */
    u_vau_sdf_start_loc_y                sdf_start_loc_y;                 /* 1410 */
    u_vau_sdf_start_loc_z                sdf_start_loc_z;                 /* 1414 */
    u_vau_sdf_delta_u_x                  sdf_delta_u_x;                   /* 1418 */
    u_vau_sdf_delta_u_y                  sdf_delta_u_y;                   /* 141c */
    u_vau_sdf_delta_u_z                  sdf_delta_u_z;                   /* 1420 */
    u_vau_sdf_delta_v_x                  sdf_delta_v_x;                   /* 1424 */
    u_vau_sdf_delta_v_y                  sdf_delta_v_y;                   /* 1428 */
    u_vau_sdf_delta_v_z                  sdf_delta_v_z;                   /* 142c */
    u_vau_sdf_tex_rot_mat_0_0            sdf_tex_rot_mat_0_0;             /* 1430 */
    u_vau_sdf_tex_rot_mat_0_1            sdf_tex_rot_mat_0_1;             /* 1434 */
    u_vau_sdf_tex_rot_mat_0_2            sdf_tex_rot_mat_0_2;             /* 1438 */
    u_vau_sdf_tex_rot_mat_0_3            sdf_tex_rot_mat_0_3;             /* 143c */
    u_vau_sdf_tex_rot_mat_0_4            sdf_tex_rot_mat_0_4;             /* 1440 */
    u_vau_sdf_tex_rot_mat_0_5            sdf_tex_rot_mat_0_5;             /* 1444 */
    u_vau_sdf_tex_rot_mat_0_6            sdf_tex_rot_mat_0_6;             /* 1448 */
    u_vau_sdf_tex_rot_mat_0_7            sdf_tex_rot_mat_0_7;             /* 144c */
    u_vau_sdf_tex_rot_mat_0_8            sdf_tex_rot_mat_0_8;             /* 1450 */
    u_vau_sdf_tex_rot_mat_1_0            sdf_tex_rot_mat_1_0;             /* 1454 */
    u_vau_sdf_tex_rot_mat_1_1            sdf_tex_rot_mat_1_1;             /* 1458 */
    u_vau_sdf_tex_rot_mat_1_2            sdf_tex_rot_mat_1_2;             /* 145c */
    u_vau_sdf_tex_rot_mat_1_3            sdf_tex_rot_mat_1_3;             /* 1460 */
    u_vau_sdf_tex_rot_mat_1_4            sdf_tex_rot_mat_1_4;             /* 1464 */
    u_vau_sdf_tex_rot_mat_1_5            sdf_tex_rot_mat_1_5;             /* 1468 */
    u_vau_sdf_tex_rot_mat_1_6            sdf_tex_rot_mat_1_6;             /* 146c */
    u_vau_sdf_tex_rot_mat_1_7            sdf_tex_rot_mat_1_7;             /* 1470 */
    u_vau_sdf_tex_rot_mat_1_8            sdf_tex_rot_mat_1_8;             /* 1474 */
    u_vau_sdf_tex_rot_mat_2_0            sdf_tex_rot_mat_2_0;             /* 1478 */
    u_vau_sdf_tex_rot_mat_2_1            sdf_tex_rot_mat_2_1;             /* 147c */
    u_vau_sdf_tex_rot_mat_2_2            sdf_tex_rot_mat_2_2;             /* 1480 */
    u_vau_sdf_tex_rot_mat_2_3            sdf_tex_rot_mat_2_3;             /* 1484 */
    u_vau_sdf_tex_rot_mat_2_4            sdf_tex_rot_mat_2_4;             /* 1488 */
    u_vau_sdf_tex_rot_mat_2_5            sdf_tex_rot_mat_2_5;             /* 148c */
    u_vau_sdf_tex_rot_mat_2_6            sdf_tex_rot_mat_2_6;             /* 1490 */
    u_vau_sdf_tex_rot_mat_2_7            sdf_tex_rot_mat_2_7;             /* 1494 */
    u_vau_sdf_tex_rot_mat_2_8            sdf_tex_rot_mat_2_8;             /* 1498 */
    u_vau_sdf_tex_rot_mat_3_0            sdf_tex_rot_mat_3_0;             /* 149c */
    u_vau_sdf_tex_rot_mat_3_1            sdf_tex_rot_mat_3_1;             /* 14a0 */
    u_vau_sdf_tex_rot_mat_3_2            sdf_tex_rot_mat_3_2;             /* 14a4 */
    u_vau_sdf_tex_rot_mat_3_3            sdf_tex_rot_mat_3_3;             /* 14a8 */
    u_vau_sdf_tex_rot_mat_3_4            sdf_tex_rot_mat_3_4;             /* 14ac */
    u_vau_sdf_tex_rot_mat_3_5            sdf_tex_rot_mat_3_5;             /* 14b0 */
    u_vau_sdf_tex_rot_mat_3_6            sdf_tex_rot_mat_3_6;             /* 14b4 */
    u_vau_sdf_tex_rot_mat_3_7            sdf_tex_rot_mat_3_7;             /* 14b8 */
    u_vau_sdf_tex_rot_mat_3_8            sdf_tex_rot_mat_3_8;             /* 14bc */
    u_vau_sdf_tex_rot_cen_0_0            sdf_tex_rot_cen_0_0;             /* 14c0 */
    u_vau_sdf_tex_rot_cen_0_1            sdf_tex_rot_cen_0_1;             /* 14c4 */
    u_vau_sdf_tex_rot_cen_1_0            sdf_tex_rot_cen_1_0;             /* 14c8 */
    u_vau_sdf_tex_rot_cen_1_1            sdf_tex_rot_cen_1_1;             /* 14cc */
    u_vau_sdf_tex_rot_cen_2_0            sdf_tex_rot_cen_2_0;             /* 14d0 */
    u_vau_sdf_tex_rot_cen_2_1            sdf_tex_rot_cen_2_1;             /* 14d4 */
    u_vau_sdf_tex_rot_cen_3_0            sdf_tex_rot_cen_3_0;             /* 14d8 */
    u_vau_sdf_tex_rot_cen_3_1            sdf_tex_rot_cen_3_1;             /* 14dc */
    u_vau_sdf_tex_cen_0_0                sdf_tex_cen_0_0;                 /* 14e0 */
    u_vau_sdf_tex_cen_0_1                sdf_tex_cen_0_1;                 /* 14e4 */
    u_vau_sdf_tex_cen_1_0                sdf_tex_cen_1_0;                 /* 14e8 */
    u_vau_sdf_tex_cen_1_1                sdf_tex_cen_1_1;                 /* 14ec */
    u_vau_sdf_tex_cen_2_0                sdf_tex_cen_2_0;                 /* 14f0 */
    u_vau_sdf_tex_cen_2_1                sdf_tex_cen_2_1;                 /* 14f4 */
    u_vau_sdf_tex_cen_3_0                sdf_tex_cen_3_0;                 /* 14f8 */
    u_vau_sdf_tex_cen_3_1                sdf_tex_cen_3_1;                 /* 14fc */
    vau_sdf_prim_paras                   sdf_prim_paras[64];              /* 20*64 */
    vau_sdf_add_prim_paras               sdf_add_prim_paras[64];          /* 4*64 */
} vau_sdf_hw_prim;

typedef struct {
    u_vau_sdf_node_ctrl                  sdf_node_ctrl;                   /* f00 */
    u_vau_sdf_node_ctrl1                 sdf_node_ctrl1;                  /* f04 */
    u_vau_sdf_glb_addr                   sdf_glb_addr;                    /* f08 */
    u_vau_sdf_prmt_addr                  sdf_prmt_addr;                   /* f0c */
    u_vau_sdf_gamma_addr                 sdf_gamma_addr;                  /* f10 */
    u_vau_sdf_degamma_addr               sdf_degamma_addr;                /* f14 */
    u_vau_sdf_txtre0_addr                sdf_txtre0_addr;                 /* f18 */
    u_vau_sdf_txtre0_stride              sdf_txtre0_stride;               /* f1c */
    u_vau_sdf_txtre1_addr                sdf_txtre1_addr;                 /* f20 */
    u_vau_sdf_txtre1_stride              sdf_txtre1_stride;               /* f24 */
    u_vau_sdf_txtre2_addr                sdf_txtre2_addr;                 /* f28 */
    u_vau_sdf_txtre2_stride              sdf_txtre2_stride;               /* f2c */
    u_vau_sdf_txtre3_addr                sdf_txtre3_addr;                 /* f30 */
    u_vau_sdf_txtre3_stride              sdf_txtre3_stride;               /* f34 */
    u_vau_sdf_splt_cfg                   sdf_splt_cfg;                    /* f38 */
    u_vau_sdf_partial_start              sdf_partial_start;               /* f3c */
    u_vau_sdf_partial_end                sdf_partial_end;                 /* f40 */
    u_vau_sdf_partial_pad_end            sdf_partial_pad_end;             /* f44 */
    u_vau_sdf_reso                       sdf_reso;                        /* f48 */
    u_vau_sdf_coarse_reso                sdf_coarse_reso;                 /* f4c */
}vau_sdf_apb_config;

typedef struct {
    vau_sdf_apb_config apb_addr;
    vau_sdf_hw_prim *hw_prim;
    vau_sdf_hw_prim *tmp_prim;
    vau_sdf_global_config *global_addr;
}vau_sdf_hw_config;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
