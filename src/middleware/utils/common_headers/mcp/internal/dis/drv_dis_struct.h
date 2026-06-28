/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: dis struct define
 * Author: AuthorNameMagicTag
 * Create: 2021-8-28
 */

#ifndef __DRV_DIS_STRUCT_H__
#define __DRV_DIS_STRUCT_H__

#include "drv_isp_video_ext.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define DIS_PRIVATE_DATA_LEN                8U
#define DIS_MAX_BUFFERNUM                   10
#define DIS_MIN_BUFFERNUM                   5
#define DIS_MAX_CROPRATIO                   98
#define DIS_MIN_CROPRATIO                   50
#define DIS_MAX_ROLLINGSHUTTER              1000
#define DIS_MIN_ROLLINGSHUTTER              0
#define DIS_MAX_SHIFT_LIMIT                 1000
#define DIS_MIN_SHIFT_LIMIT                 0
#define DIS_MAX_MOVINGSUB_LEVEL             6
#define DIS_MIN_MOVINGSUB_LEVEL             0
#define DIS_MAX_TIMEDELAY                   2000000
#define DIS_MIN_TIMEDELAY                   (-2000000)
#define DIS_MAX_VIEWANGLE                   1380
#define DIS_MIN_VIEWANGLE                   100
#define DIS_MAX_STRENGTH                    1024
#define DIS_MAX_LARGE_MOTION_STABLE_COEF    100
#define DIS_MAX_LOW_FREQ_MOTION_PRESERVE    100
#define DIS_MAX_LOW_FREQ_MOTION_FREQ        100
#define DIS_MAX_CHN_NUM                     4
#define DIS_PRIVATE_DATA_LEN                8U
#define DIS_MOTIONBUF_LEN                   32U
#define DIS_GMEDEBUGBUF_LEN                 320U
#define DIS_FPDBUF_LEN                      3072U
#define DIS_GMEFPDBUF_LEN                   9216U
#define DIS_INLIER_INDEX_LEN                320U
#define DIS_PYRAMID_LAYER_NUM               4
#define DIS_MAX_FRAMERATE                   60U
#define DIS_MIN_FRAMERATE                   0U

typedef struct {
    td_bool after_3dnr;         /* 1bit; bool */
    td_bool is_down_sampled;    /* 1bit; bool */
    td_u8 factor;               /* 5bits; u; 5.0 [0,16] */
    td_bool is_lp_filter;       /* 1bit; bool */
    td_bool is_bit_truncation;  /* 1bit; bool */

    td_u32 in_crop_x_start;     /* 13tbis; u; 13.0  [0,512] */
    td_u32 in_crop_x_step;      /* 13bits; u; 13.0  [639,8195] */
    td_u32 in_crop_y_start;     /* 13bits; u; 13.0  [0,4319] */
    td_u32 in_crop_y_step;      /*  13bits; u ; 13.0 [0,4319] */

    td_u32 out_crop_x_start;    /* 13bits; u; 13.0  [0,15] */
    td_u32 out_crop_x_step;     /*  13bits; u ; 13.0 [119,1919] */
    td_u32 out_crop_y_start;    /* 13bits; u; 13.0  [0,1079] */
    td_u32 out_crop_y_step;     /*  13bits; u ; 13.0 [0,1079] */

    td_u32 ds_frame_width;      /* 13bits; u; 13.0  [0,1920] */
    td_u32 ds_frame_height;     /* 13bits; u; 13.0  [0,1080] */
} ext_dis_dsf_hw_cfg;

/* task完成状态 */
typedef enum {
    EXT_DIS_TASK_FNSH_STAT_FAIL = 1,    /* task执行出现异常或未完成**/
    EXT_DIS_TASK_FNSH_STAT_OK = 2,      /* task执行已完成 */
    EXT_DIS_TASK_FNSH_STAT_MAX
} ext_dis_task_finish_state;

/* the type of job */
typedef enum {
    EXT_DIS_TASK_TYPE_BYPASS = 0,       /* BYPASS task will never do DIS */
    EXT_DIS_TASK_TYPE_NORMAL = 1,       /* normal task will  do DIS */
    EXT_DIS_TASK_TYPE_BYPASS_GDC = 2,   /* gme enable, but bypass gdc correction. */
    EXT_DIS_TASK_TYPE_MAX
} ext_dis_task_type;

typedef enum {
    EXT_DIS_MODE_4_DOF_GME = 0,
    EXT_DIS_MODE_6_DOF_GME,
    EXT_DIS_MODE_GYRO,
    EXT_DIS_MODE_DOF_MAX,
} ext_dis_mode;

/* The motion level of camera */
typedef enum {
    EXT_DIS_MOTION_LEVEL_LOW = 0,   /* Low motion level */
    EXT_DIS_MOTION_LEVEL_NORMAL,    /* Normal motion level */
    EXT_DIS_MOTION_LEVEL_HIGH,      /* High motion level */
    EXT_DIS_MOTION_LEVEL_MAX
} ext_dis_motion_level;

/* Different product type used DIS */
typedef enum {
    EXT_DIS_PDT_TYPE_IPC = 0,   /* IPC product type */
    EXT_DIS_PDT_TYPE_DV,        /* DV product type */
    EXT_DIS_PDT_TYPE_DRONE,     /* DRONE product type */
    EXT_DIS_PDT_TYPE_MAX
} ext_dis_pdt_type;

typedef enum {
    EXT_DIS_WORK_MODE_VIDEO = 0,
    EXT_DIS_WORK_MODE_SNAP,
    EXT_DIS_WORK_MODE_MAX,
} ext_dis_work_mode;

typedef enum {
    EXT_DIS_SAMPLE_IIR = 0,
    EXT_DIS_SAMPLE_FIR,
    EXT_DIS_SAMPLE_MAX,
} ext_dis_sample_type;

/* The Config of DIS */
typedef struct {
    ext_dis_mode         mode;                   /* RW; DIS Mode */
    ext_dis_work_mode    work_mode;
    td_u32               snap_num;
    ext_dis_motion_level motion_level;           /* RW; DIS Motion level of the camera */
    ext_dis_pdt_type     pdt_type;               /* RW; DIS product type */
    td_u32             buf_num;                /* RW; Range:[5,10]; Buf num for DIS */
    td_u32             crop_ratio;             /* RW; Range:[50,98]; Crop ratio of output image */
    td_u32             frame_rate;
    td_u32             gyro_output_range;      /* Reserved */
    td_u32             gyro_data_bit_width;    /* Reserved */
    td_bool            camera_steady;          /* RW; The camera is steady or not */
    td_bool            scale;                  /* RW; Scale output image or not */
} ext_dis_config;

typedef struct {
    td_u32 distortion_level;
    td_bool is_ldc;
    td_bool is_lp_filter;
    /* FPD&GME ROI */
    td_u32 roi_offset_x;
    td_u32 roi_offset_y;
    td_u32 roi_width;
    td_u32 roi_height;

    /* FPD&GME shared parameters */
    td_bool use_default_gme_param;
    td_u32 gme_down_sample_factor;
    td_u32 gme_neighbor_min_dist;
    td_u32 gme_max_num_key_pts;

    /* config the feature detector parameter */
    td_u32 det_quality_level;   /* quality_level */
    td_u32 min_eigen_val;

    /* config GME parameter */
    td_u32 trk_max_count;       /* max iteration count */
    td_u32 gme_refresh_thresh;
    td_u32 gme_replenish_thres;
    td_u32 max_mv_norm;
    td_u32 gme_hori_limit;      /* dynamic */
    td_u32 gme_vert_limit;      /* dynamic */
    td_u32 prescreen_trk_thr;
    td_u32 ldc_ratio;

    /* config the motion filter parameter */
    td_u32 motion_anchor;       /* 0:ANCHOR_CORNER 1:ANCHOR_CENTER */

    td_bool use_default_ml_and_drift_param;
    td_bool motion_limit_check;
    td_u32 ml_zoom_in_limit;
    td_u32 ml_zoom_out_limit;
    td_u32 ml_rotate_thresh;
    td_u32 ml_skew_ratio_limit;
    td_u32 ml_skew_angle_limit;

    td_bool drift_param_fix;
    td_u32 drift_param_min_coef;
    td_u32 drift_param_max_coef;
    td_u32 drift_strength;
    td_u32 stab_motion_adjust_coef;

    td_u32 min_inlier_ratio;
    td_bool use_default_filtering_param;
    td_u32 ave_mot_strength;
    td_u32 ave_mot_forget_factor;

    td_bool use_default_rs_limit_param;
    td_u32 rs_hori_min_thresh;
    td_u32 rs_hori_diff_thresh;
    td_u32 rs_vert_min_thresh;
    td_u32 rs_vert_diff_thresh;
    td_u32 rs_correction;   /* rolling shutter correction coeficient */

    td_bool use_default_params;
    td_u32 fuse_corr_thresh_xy;
    td_u32 fuse_corr_thresh_z;
    td_u32 gyro_steady_thre_xy;
    td_u32 gyro_steady_thre_z;
    td_u32 gme_gyro_max_diff_ratio_thre;
    td_u32 gme_reliable_cnt_thr;

    td_bool after_3dnr;
} ext_dis_dbg_param;

typedef struct {
    td_u32 width;
    td_u32 height;
    td_u32 stream_bit_depth;
    td_bool side_by_side;
    td_u32 div_mid_line;          /* the position of the middle line of the overlap region */
    td_u32 div_left_blk_end;      /* coordinate of last pixel in the left block */
    td_u32 div_right_blk_start;   /* coordinate of first pixel in the right block */
    td_bool dis_debug;
    ext_dis_dbg_param debug_param;  /* debug parameters */
} ext_dis_sys_config;

typedef struct {
    /*
     * RW; [0,100],
     * 0: attenuate large motion most in advance,
     * 100: never attenuate large motion;
     * larger value -> better stability but more likely to crop to the border with large motion
     */
    td_u32 large_motion_stable_coef;
    /*
     * RW; [0,100],
     * 0: never preserve the low frequency motion,
     * 100: keep all the low frequency motion;
     * small value -> better stability but more likely to crop to the border even with low level motion
     */
    td_u32 low_freq_motion_preserve;
    /*
     * RW; [0,100],
     * 0: lowest cut frequency,
     * 100: highest cut frequency;
     * small value -> better stability but more likely to crop to the border even with large motion
     */
    td_u32 low_freq_motion_freq;
} ext_dis_param;

/* The Attribute of DIS */
typedef struct {
    td_bool enable;               /* RW; DIS enable */
    td_bool multi_region_enable;  /* RW; Partition registration enable */
    td_bool gdc_bypass;           /* RW; gdc correction process , DIS = GME&GDC correction */
    td_u32  moving_subject_level; /* RW; Range:[0,6]; Moving Subject level */
    td_s32  rolling_shutter_coef; /* RW; Range:[0,1000]; Rolling shutter coefficients */
    td_s32  timelag;              /* RW; Range:[-2000000,2000000]; Timestamp delay between Gyro and Frame PTS */
    td_u32  view_angle;           /* Reserved */
    td_u32  horizontal_limit;     /* RW; Range:[0,1000]; Parameter to limit horizontal drift by large foreground */
    td_u32  vertical_limit;       /* RW; Range:[0,1000]; Parameter to limit vertical drift by large foreground */
    td_bool still_crop;           /* RW; The stabilization not working,but the output image still be cropped */
    td_u32  strength;             /* RW. The DIS strength for different light */
} ext_dis_attr;

/*
 * DIS操作数据信息结构，包括输入、输出图像信息及调用者，回调函数信息。
 * DIS任务完成后，每个操作以回调函数形式通知用户，该结构作为回调函数参数
 * 返回该操作对应的数据信息
 */
typedef struct dis_task_data {
    ext_isp_video_frame_info img_in;                 /* 原始帧信息 */
    ext_isp_video_frame_info dis_pyramid_frm_info;   /* DIS 金字塔信息 */
    ext_dis_task_type task_type;                    /* DIS task 类型, normal or bypass */
    ext_isp_dis_motion_data dis_motion_data;         /* DIS输出的motion data */
    ext_isp_dis_motion_data dis_motion_data_regions[EXT_ISP_DIS_HOR_PART_NUM][EXT_ISP_DIS_VER_PART_NUM];
    td_u32 output_width;                        /* 输出宽高，传给GDC使用 */
    td_u32 output_height;

    td_u64 private_data[DIS_PRIVATE_DATA_LEN];  /* task's private data */
    td_void (*call_back)(soc_mod_id call_mod_id, td_s32 call_dev_id, td_s32 call_chn_id,
         const struct dis_task_data *task_out); /* callback */
    soc_mod_id call_mod_id;              /* module ID */
    td_s32 call_dev_id;                 /* device ID */
    td_s32 call_chn_id;                 /* chnnal ID */
    ext_dis_task_finish_state finish_state; /* output param:task finish status(ok or nok) */
    td_s32 crop_offset_x;
    td_s32 crop_offset_y;
} ext_dis_task_data;

typedef struct {
    td_bool use_default_params;
    td_u32 fuse_corr_thresh_xy;
    td_u32 fuse_corr_thresh_z;
    td_u32 gyro_steady_thre_xy;
    td_u32 gyro_steady_thre_z;
    td_u32 gme_gyro_max_diff_ratio_thre;
    td_u32 gme_reliable_cnt_thr;
} ext_dis_hybrid_fuse_param;

typedef enum {
    EXT_DIS_MF_MOTION_ANCHOR_CORNER = 0,
    EXT_DIS_MF_MOTION_ANCHOR_CENTER = 1,
    EXT_DIS_MF_MOTION_ANCHOR_MAX,
} ext_dis_mf_motion_anchor_mode;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __DRV_DIS_STRUCT_H__ */
