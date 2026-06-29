/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: gdc struct define
 * Author: AuthorNameMagicTag
 * Create: 2021-8-27
 */

#ifndef __DRV_GDC_STRUCT_H__
#define __DRV_GDC_STRUCT_H__

#include "drv_isp_video_ext.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define GDC_MAX_FANSTRENGTH                760
#define GDC_FISHEYE_LMFCOEF_NUM            128

#define GDC_PMFCOEF_NUM                    9

#define GDC_FISHEYE_MAX_REGION_NUM         4
#define GDC_FISHEYE_MIN_IN_IMAGE_WIDTH     1920
#define GDC_FISHEYE_MIN_IN_IMAGE_HEIGHT    1080
#define GDC_FISHEYE_MIN_OUT_IMAGE_WIDTH    640
#define GDC_FISHEYE_MIN_OUT_IMAGE_HEIGHT   360
#define GDC_FISHEYE_MAX_OUT_IMAGE_WIDTH    4608
#define GDC_FISHEYE_MAX_OUT_IMAGE_HEIGHT   4608

#define GDC_LDC_MIN_IMAGE_WIDTH            640
#define GDC_LDC_MIN_IMAGE_HEIGHT           480

#define GDC_SPREAD_MIN_IMAGE_WIDTH         640
#define GDC_SPREAD_MIN_IMAGE_HEIGHT        480

#define GDC_PMF_MIN_IMAGE_WIDTH            480
#define GDC_PMF_MIN_IMAGE_HEIGHT           360

#define GDC_ROTATION_EX_MIN_IMAGE_WIDTH    480
#define GDC_ROTATION_EX_MIN_IMAGE_HEIGHT   360

#define GDC_MAX_IMAGE_WIDTH                8192
#define GDC_MAX_IMAGE_HEIGHT               8192
#define GDC_IP_NUM                         1
#define GDC_FUSION_NUM                     9

typedef enum {
    EXT_GDC_FISHEYE_VIEW_NORMAL = 2,
    EXT_GDC_FISHEYE_NO_TRANSFORMATION,

    EXT_GDC_FISHEYE_VIEW_MODE_MAX
} ext_gdc_fisheye_view_mode;

/* Mount mode of device */
typedef enum {
    EXT_GDC_FISHEYE_DESKTOP_MOUNT = 0,
    EXT_GDC_FISHEYE_CEILING_MOUNT = 1,
    EXT_GDC_FISHEYE_WALL_MOUNT = 2,

    EXT_GDC_FISHEYE_MOUNT_MODE_MAX
} ext_gdc_fisheye_mount_mode;

typedef enum {
    EXT_GDC_VIEW_NORMAL = 2,
    EXT_GDC_VIEW_LDC,        /* lens distoration cerrection. */
    EXT_GDC_VIEW_PMF,        /* PMF. */
    EXT_GDC_VIEW_ROTATION,   /* 360 degree rotation mode */
    EXT_GDC_VIEW_GYRO_DIS,   /* GYRO DIS mode */

    GDC_VIEW_MAX,
} ext_gdc_view_mode;

/* spread correction attribute */
typedef struct {
    /* RW; range: [0, 1];whether enable spread or not, when spread on,ldc distortion_ratio range should be [0, 500] */
    td_bool enable;
    td_u32 spread_coef;   /* RW; range: [0, 18];strength coefficient of spread correction */
    ext_isp_size dest_size; /* RW; dest size of spread */
} ext_gdc_spread_attr;

/* fisheye correction configuration */
typedef struct {
    td_bool enable;
    td_bool lmf;    /* RW, whether gdc len's LMF coefficient is from user config or from default linear config */
    ext_gdc_fisheye_view_mode view_mode;
    ext_gdc_fisheye_mount_mode mount_mode;

    /* RW,  LMF coefficients of fisheye lens (lenght = GDC_FISHEYE_LMFCOEF_NUM) */
    td_u16 lmf_coef[GDC_FISHEYE_LMFCOEF_NUM];
    td_u64 len_map_addr;

    td_u32 in_radius;     /* RW, inner radius of gdc correction region */
    td_u32 out_radius;    /* RW, out radius of gdc correction region */
    /* RW, range: [-511, 511], the horizontal offset between image center and physical center of lens */
    td_s32 hor_offset;
    /* RW, range: [-511, 511], the vertical offset between image center and physical center of lens */
    td_s32 ver_offset;

    td_u32 pan;           /* RW, range: [0, 360] */
    td_u32 tilt;          /* RW, range: [0, 360] */
    td_u32 hor_zoom;      /* RW, range: [1, 4095] */
    td_u32 ver_zoom;      /* RW, range: [1, 4095] */

    td_bool is_use_roixy; /* RW, range: [0,1] */
    /* RW, range: [0,8191], the x position of ROI center with normal mode */
    td_u32 roi_center_x;
    /* RW, range: [0,8191], the y position of ROI center with normal mode */
    td_u32 roi_center_y;

    td_u32 trapezoid_coef; /* RW, range: [0, 32], strength coefficient of trapezoid correction */
    td_s32 fan_strength;   /* RW, range: [-760, 760], strength coefficient of fan correction */

    ext_isp_size dest_size;  /* RW, dst size of spread */
} ext_gdc_fisheye_cfg;

#define LUMA_INTP_COEF_LEN    ((1 << (4 - 1)) + 1)
#define LUMA_INTP_HORI_TAP 6
#define LUMA_INTP_VERT_TAP 4

/* commom parameters */
typedef struct {
    ext_gdc_view_mode view_mode; /* !(U,4,0) <common register> [0 4]  [0,4]  0:360 1:180 2:normal 3:LDC 4:PMF */

    td_u32 width;           /* !(U,14,0) <common register> [512,16383] 14bit u32 */
    td_u32 height;          /* !(U,14,0) <common register> [512,16383] 14bit u32 */
    td_u32 out_width;       /* !(U,14,0) <common register> [512,16383] 14bit u32 */
    td_u32 out_height;      /* !(U,14,0) <common register> [512,16383] 14bit u32 */
    td_u32 width_b4_pmf;    /* !(U,14,0) <common register> [512,16383] 14bit u32 */
    td_u32 height_b4_pmf;   /* !(U,14,0) <common register> [512,16383] 14bit u32 */

    /* !(U,14,0) <common register> [0,16383] 14bit unsigned projection certer
     * in image plane coordinate system (IPCS) */
    td_u32 center_src_h;
    /* !(U,14,0) <common register>  [0,16383] 14bit unsigned projection certer
     * in image plane coordinate system (IPCS) */
    td_u32 center_src_v;
    /* !(U,14,0) <common register> [0,16383] 14bit unsigned projection certer
     * in image plane coordinate system (IPCS) */
    td_u32 center_dst_h;
    /* !(U,14,0) <common register> [0,16383] 14bit unsigned projection certer
     * in image plane coordinate system (IPCS) */
    td_u32 center_dst_v;
    /* (B, 1, 0) <SW parameter> 0: applying 4x6 filter; 1: applying bilinear kernel */
    td_bool is_bilinear_intp;
    td_s8 luma_intp_hori6_tap_coef[LUMA_INTP_COEF_LEN * LUMA_INTP_HORI_TAP];     /* (S,7,0) [-64, 64] 8bits signed */
    td_s8 luma_intp_vert4_tap_coef[LUMA_INTP_COEF_LEN * LUMA_INTP_VERT_TAP];     /* (S,10,0)[-512, 512] 11bits signed */
    td_s8 chroma_intp_hori6_tap_coef[LUMA_INTP_COEF_LEN * LUMA_INTP_HORI_TAP];   /* (S,7,0) [-64, 64] 8bits signed */
    td_s8 chroma_intp_vert4_tap_coef[LUMA_INTP_COEF_LEN * LUMA_INTP_VERT_TAP];   /* (S,10,0)[-512, 512] 11bits signed */
    td_u8 intp_hori_norm_bit;            /* (U,3,0) [1,6] 3bits u8 */
    td_u8 intp_vert_norm_bit;            /* (U,4,0) [1,9] 4bits u8 */
    td_bool is_round_aft_intp;           /* (B,1,0) [0,1] 1bit td_bool */
} ext_gdc_fw_comm_param;

/* compat commom parameters */
typedef struct {
    ext_gdc_view_mode view_mode;
    td_u32 width;
    td_u32 height;
    td_u32 out_width;
    td_u32 out_height;
    td_u32 width_b4_pmf;
    td_u32 height_b4_pmf;
    td_u32 center_src_h;
    td_u32 center_src_v;
    td_u32 center_dst_h;
    td_u32 center_dst_v;
    td_bool is_bilinear_intp;
    td_u32 luma_intp_hori6_tap_coef;
    td_u32 luma_intp_vert4_tap_coef;
    td_u32 chroma_intp_hori6_tap_coef;
    td_u32 chroma_intp_vert4_tap_coef;
    td_u8 intp_hori_norm_bit;
    td_u8 intp_vert_norm_bit;
    td_bool is_round_aft_intp;
} ext_gdc_compat_fw_comm_param;

/* fisheye correction parameters */
typedef struct {
    /* (U, 2,0) <common register> [0 2] [0,2] 0:floor 1:up  2:wall */
    ext_gdc_fisheye_mount_mode load_mode;
    td_u32 radius;       /* !(U,18,0)  <common register> [1,16383] ldc: [1,262143] -- 18bit u32 */
    td_s32 pan;          /* !(S,17,0) <common register> [-PI,PI]=[-131071,131071] -- 18bit signed PTZ pan. */
    td_s32 tilt;         /* s(S,17,0) <common register> [-PI,PI]=[-131071,131071] -- 18bit signed  PTZ pan. */
    td_u32 scale_h;      /* !!(U,20,0) <common register> -- [0,127*1024] scaler coefficient for horizontal */
    td_u32 scale_v;      /* (U,15,0) <common register> -- [0,31*1024] scaler coefficient for vertical */
    td_bool is_lmf;      /* (U,2,0) <common register> [0,1] [0,1] */
    /* (U,4,10) <common register> 4bit integer and 10bit decimal. */
    td_s16 lmf[GDC_FISHEYE_LMFCOEF_NUM + 1];

                           /* for pano 360 correction */
    td_s32 min;          /* !(S,13,0) <mode360 register>[0,8191) [-8191,8191] */
    td_u32 lmf_coef;     /* !(U,14,0) <mode360 register> -- 12bit u32 lmf coeff. */

    /* for pano 180 correction */
    td_s32 s32tilt0;     /* (S,10,0) <mode180 register> -- [-1*1024 1*1024] */
    td_s32 s32tilt1;     /* (U,10,0) <mode180 register> -- [0 1*1024] */
    td_u32 virt_y;       /* !!(U,20,0) <mode180 register> -- 20bit unsighed */
    td_s32 offset180;    /* !(U,18,0) <mode180 register> [0,(PIM2-1)]=[0,262143] -- 18bit u32 */
    td_s32 fan_strength; /* !(S,10,0) <mode180 register> [-760,760] [-760,760] */

    /* for rectlinear correction */
    td_u32 ldc_type;     /* (U,1,0) <LDC use register>[0,1] [0.1] 0:barrel distortion 1:pincushion distortion */
    td_s32 detla_xu;     /* (S,16,0) <normal register> -- [-34*1024, 34*1024] */
    td_s32 detla_yu;     /* (S,16,0) <normal register> -- [-34*1024, 34*1024] */
    td_s32 detla_zu;     /* (S,16,0) <normal register> -- [-34*1024, 34*1024] */
    td_s32 detla_xv;     /* (S,16,0) <normal register> -- [-34*1024, 34*1024] */
    td_s32 detla_yv;     /* (S,16,0) <normal register> -- [-34*1024, 34*1024] */
    td_s32 detla_zv;     /* (S,16,0) <normal register> -- [-34*1024, 34*1024] */
    td_s32 start_x;      /* !!(S,20,0) <normal register> -- 8191 */
    td_s32 start_y;      /* !!(S,22,0) <normal register> -- 32767 */
    td_s32 start_z;      /* !!(S,20,0) <normal register> -- 8191 */

    /* temporal firmware parameters */
    td_u32 hor_zoom;
    td_u32 ver_zoom;
    td_s32 distortion_ratio;
} ext_gdc_fw_fisheye_param;

/* FIFO parameters */
typedef struct {
    /* (U,10,0) <common register> -- [0,1023] projection certer in image plane coordinate system (IPCS) */
    td_u32 out_fifo_width;
    /* (U,10,0) <common register> -- [0,1023] projection certer in image plane coordinate system (IPCS) */
    td_u32 out_fifo_height;
    td_u32 extend_org;           /* (U,15,0) <common register> -- [0~15] */
    td_u32 extend_minxy;         /* (U,15,0) <common register> -- [0~15] */
    td_u32 extend_width;         /* (U,15,0) <common register> -- [0~15] */
    td_u32 extend_height;        /* (U,15,0) <common register> -- [0~15] */
    td_s32 bit_depth_in;         /* (U,5,0) <common register> [8,30]  -- input bitdepth */
    /* (B,1,0) 0:use 4 corner points to decide; 1: use block intesecting image to decide */
    td_bool fifo_blk_out_mode;
} ext_gdc_fw_fifo_param;

/* PMF parameters */
typedef struct {
    td_bool pmf_en;              /* (U,1,0) <common register> [0,1] [0,1] */
    /* !(S,23,0) h00,h01,h10,h11,h20,h21 !(S,33,0) h02,h12,h22 <common register>  h00,h01,h10,h11,h20,h21
     * 24bit sign(24bit include sign)  h02,h12,h22 34bit sign(34bit incluse sign perspective mapping. */
    /*  !! change from 32bit to 34bit  */
    td_s64 pmf[GDC_PMFCOEF_NUM];
    td_bool is_affine;           /* (B,1,0) [0,1] 1bit td_bool */
    td_u8 pmf_norm_bits;         /* (U,6,0) [1,34] 6bits u32 */
} ext_gdc_fw_pmf_param;

/* phy_addr configurations */
typedef struct {
    td_u64 len_map_phy_addr;
    td_u64 interpolate_phy_addr;
    td_u64 angle_vel_phy_addr;
} ext_gdc_sdk_phyaddr;

typedef struct {
    ext_isp_size src_img_size;
    td_u32 bit_depth_in;
} ext_gdc_comm_cfg;

/* fisheye region correction attribute */
typedef struct {
    ext_gdc_fisheye_view_mode view_mode;    /* RW; range: [0, 3];gdc view mode */
    td_u32 in_radius;                     /* RW; inner radius of gdc correction region */
    td_u32 out_radius;                    /* RW; out radius of gdc correction region */
    td_u32 pan;                           /* RW; range: [0, 360] */
    td_u32 tilt;                          /* RW; range: [0, 360] */
    td_u32 hor_zoom;                      /* RW; range: [1, 4095] */
    td_u32 ver_zoom;                      /* RW; range: [1, 4095] */
    ext_isp_rect out_rect;                  /* RW; out imge rectangle attribute */
} ext_gdc_fisheye_region_attr;

/* fisheye all regions correction attribute */
typedef struct {
    td_bool enable;                       /* RW; range: [0, 1];whether enable fisheye correction or not */
    td_bool lmf_enable;                   /* RW; range: [0, 1];whether gdc len's LMF coefficient is from
                                               user config or from default linear config */
    td_bool bg_color_enable;              /* RW; range: [0, 1];whether use background color or not */
    td_u32  bg_color;                     /* RW; range: [0,0xffffff];the background color RGB888 */
    td_s32  hor_offset;                   /* RW; range: [-511, 511];the horizontal offset between image
                                               center and physical center of len */
    td_s32  ver_offset;                   /* RW; range: [-511, 511]; the vertical offset between image
                                               center and physical center of len */
    td_u32  trapezoid_coef;               /* RW; range: [0, 32];strength coefficient of trapezoid correction */
    td_s32  fan_strength;                 /* RW; range: [-760, 760];strength coefficient of fan correction */

    ext_gdc_fisheye_mount_mode mount_mode;  /* RW; range: [0, 2];gdc mount mode */

    td_u32 region_num;                    /* RW; range: [1, 4]; gdc correction region number */
     /* RW; attribution of gdc correction region */
    ext_gdc_fisheye_region_attr fisheye_region_attr[GDC_FISHEYE_MAX_REGION_NUM];
} ext_gdc_fisheye_attr;

typedef struct {
    ext_gdc_fisheye_cfg fisheye_cfg[GDC_FISHEYE_MAX_REGION_NUM];
    ext_gdc_fw_comm_param fw_comm_param[GDC_FISHEYE_MAX_REGION_NUM];
    /* angle config of gdc correction region */
    ext_gdc_fw_fisheye_param fw_fisheye_param[GDC_FISHEYE_MAX_REGION_NUM];
    ext_gdc_fw_fifo_param fw_fifo_param[GDC_FISHEYE_MAX_REGION_NUM];
    ext_gdc_fw_pmf_param pmf_param[GDC_FISHEYE_MAX_REGION_NUM];

    ext_gdc_sdk_phyaddr sdk_phy_addr;
    ext_gdc_comm_cfg comm_cfg;
    ext_gdc_fisheye_attr fisheye_attr;
} ext_gdc_fisheye_drv_attr;

/* fisheye config */
typedef struct {
    td_u16 lmf_coef[GDC_FISHEYE_LMFCOEF_NUM]; /* RW;  LMF coefficient of gdc len */
} ext_gdc_fisheye_config;

typedef struct {
    /* (8bits, u, 8.0 [1,140]) valid gyro sample size per frame */
    td_u32 gyro_sample_sz;
    /* (34bits, s, 1.32, precision is 32bits decimal) time stamp of the first row of the current frame */
    td_s64 frame_pts;
    /* (19bits, s, 0.18, precision is 32bits decimal, max_dt = (1/25fps)/720rows * 2^32) */
    td_s64 row_dt;
    /* rolling shutter time inverval between two consecutive rows */
    /* (22bits, s, 1.20, precision is 20bits decimal), gyro samples PTS used in one frame */
    /* td_s32 angle_vel_pts[GDC_MAX_GYRO_SAMPLE_PER_FRAME] */
    /* (17bits, s, 3.13, precision is 13bits decimal), gyro samples used in one frame */
    /* td_s32 angle_vel_data[3 * CONFIG_SOCT_GDC_MAX_GYRO_SAMPLE_PER_FRAME] */
    /* (22bits, s, 1.20, precision is 20bits decimal), gyro samples PTS used in one frame */
    /* td_s32 *angle_vel_pts; */
    /* (17bits, s, 3.13, precision is 13bits decimal), gyro samples used in one frame */
    /* td_s32 *angle_vel_data; */
    /* (22bits, s, 1.20, precision is 20bits decimal), stable motion of current frame in rotation matrix */
    td_s32 stab_rot_mat[3][3]; /* 3x3 matrix */
    /* (2bits, s, 1.0), permutation matrix */
    td_s32 t[3][3]; /* 3x3 matrix */
    /* (22bits ,s, 1,20, precision is 20bits decimal),original motion of the first row of current frame */
    td_s32 ori_quat[4]; /* 4 quaternion element */

    /* lens K matrix
     * K[0]    0       K[2]
     * 0       K[1]    K[3]
     * 0       0       1
     */
    td_s32 k[4];  /* K[0],K[4]: 17btis, u, 15.2; K[2],K[5]: 14bits, u, 12.2, */

    /* inverse of K matrix
     * kinv[0]     0           kinv[2]
     * 0           kinv[1]     kinv[3]
     * 0           0           1
     */
    /* kinv[0],kinv[4]: 20bits, u, 0.20, precion 26bits decimal (max value is 1/96), inverse of K matrix */
    td_s32 kinv[4];
    /* kinv[2],kinv[5]: 29bits, s(always negative), 3.26, precion 26bits decimal (max value is 5.2) */
    td_s32 kc_src[3];  /* kc_src[0]: 14bits, s, 0.13, [-0.7~1] -> [-5700,8191], precion 13bits decimal */
    /* kc_src[1]: 13bits, s, 0.12, [-0.5~0.5] -> [-4095,4095], precision 13 bits decimal */
    /* kc_src[2]: 11bits, s, 0.10, [-0.1~0.1] -> [-820,820], precision 13 bits decimal */
    /* lens radial  distortion coeficients of the input image (calibration is needed) */
    td_s32 kc_dst[3][4];  /* (18bits, s, 4.13, precision is 13bits decimal) , range (-16,16) */
    /* lens distortion coeficients of the output image (needed to be defined) */
    /* (24bits, u, 9.15, precision is 15bits decimal), square value of junction points of the destination image */
    td_s32 jun_pt_opt_sqr[2];
    /* (18bits, s, 4.13, precision is 13bits decimal) , range (-16,16),
     * 2nd segmentation of the lens distortion (calibration is needed) */
    td_s32 kc_src2nd_seg[4]; /* 4 segmentations  */
    /* 23bits, u, 11.12, precision is 12bits decimal), square value of junction points of the destination image */
    td_s32 jun_pt_opt_sqr_src;
    /* 14bits, u, 1.13, [0.5~1] -> [4096~8192], precision is 13 bits decimal,
     * scale ratio of the image in horizontal direction */
    td_s32 scale_ratio_x;
    /* 14bits, u, 1.13, [0.5~1] -> [4096~8192], precision is 13 bits decimal,
     * scale ratio of the image in vertical direction */
    td_s32 scale_ratio_y;

    td_bool is_enable_itpl_mot_of_rows;  /* 1bits, is enable the per row ops (interpolation motion of rows) */
    /* (22bits, s, 1.20, precision is 20bits decimal), corrected motion if is_enable_itpl_mot_of_rows == FALSE */
    td_s32 rot_mat_mul[3][3]; /* 3x3 matrics */

    td_bool is_remove_distortion;  /* 1bit, bool */
    td_bool is_stabilization;  /* 1bit, bool */
    td_bool is_apply_distortion;  /* 1bit, bool */
} ext_gdc_fw_gyrodis_param;

typedef struct {
    ext_gdc_fw_comm_param fw_comm_param;
    ext_gdc_fw_fisheye_param fw_fisheye_param;
    ext_gdc_fw_fifo_param fw_fifo_param;
} ext_gdc_firmware_attr;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __DRV_GDC_STRUCT_H__ */
