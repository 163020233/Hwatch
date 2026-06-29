/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: isp subsystem commom sturct define
 * Author: AuthorNameMagicTag
 * Create: 2021-08-18
 */

#ifndef __DRV_ISP_VIDEO_EXT_H__
#define __DRV_ISP_VIDEO_EXT_H__

#include "securec.h"
#include "drv_mcp_math.h"
#include "drv_mcp_module.h"
#include "mcp_arch_define.h"
#include "drv_mcp_video.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define EXT_ISP_ALIGN_NUM   4
#define EXT_ISP_ATTRIBUTE   __attribute__((aligned(EXT_ISP_ALIGN_NUM)))

#define is_yuv_sp422(pixel_format)  ((pixel_format) == EXT_ISP_PIXEL_FORMAT_YUV_SEMIPLANAR_422)
#define is_yvu_sp422(pixel_format)  ((pixel_format) == EXT_ISP_PIXEL_FORMAT_YVU_SEMIPLANAR_422)
#define is_yuv_sp420(pixel_format)  ((pixel_format) == EXT_ISP_PIXEL_FORMAT_YUV_SEMIPLANAR_420)
#define is_yvu_sp420(pixel_format)  ((pixel_format) == EXT_ISP_PIXEL_FORMAT_YVU_SEMIPLANAR_420)
#define is_yuv_400(pixel_format)    ((pixel_format) == EXT_ISP_PIXEL_FORMAT_YUV_400)
#define is_sp422(pixel_format)      (is_yuv_sp422(pixel_format) || is_yvu_sp422(pixel_format))
#define is_sp420(pixel_format)      (is_yuv_sp420(pixel_format) || is_yvu_sp420(pixel_format))


typedef struct {
    td_u32 top_width;
    td_u32 bottom_width;
    td_u32 left_width;
    td_u32 right_width;
    td_u32 color;
} ext_isp_border;

typedef struct {
    td_s32 x;
    td_s32 y;
} ext_isp_point;

typedef struct {
    td_u32 width;
    td_u32 height;
} ext_isp_size;

typedef struct {
    td_s32 x;
    td_s32 y;
    td_u32 width;
    td_u32 height;
} ext_isp_rect;

#define ISP_LUMA_STAT_NUM 4
#define ISP_LUMA_REGION_DATA_NUM 3
typedef struct {
    td_bool      face_en;                    /* face area statistics enable */
    td_u32       region_num;                 /* W; count of the region */
    ext_isp_rect region[ISP_LUMA_STAT_NUM];  /* W; region attribute */
} ext_isp_video_region_info;

typedef struct {
    td_s32 milli_sec;
    ext_isp_video_region_info region_info;   /* information of the region */
    td_u32 luma_data[ISP_LUMA_STAT_NUM * ISP_LUMA_REGION_DATA_NUM];    /* luma data of the region */
} ext_isp_get_region_luma;

typedef struct {
    td_bool enable;
    ext_isp_rect rect;
} ext_isp_crop_info;

typedef struct {
    td_s32  src_frame_rate;  /* RW; source frame rate */
    td_s32  dst_frame_rate;  /* RW; dest frame rate */
} ext_isp_frame_rate_ctrl;

typedef enum {
    EXT_ISP_ASPECT_RATIO_NONE = 0,  /* full screen */
    EXT_ISP_ASPECT_RATIO_AUTO = 1,  /* ratio no change, 1:1 */
    EXT_ISP_ASPECT_RATIO_MANUAL = 2,  /* ratio manual set */
    EXT_ISP_ASPECT_RATIO_MAX
} ext_isp_aspect_ratio_type;

typedef struct {
    ext_isp_aspect_ratio_type mode;       /* aspect ratio mode: none/auto/manual */
    td_u32                    bg_color;   /* background color, RGB 888 */
    ext_isp_rect              video_rect; /* valid in ASPECT_RATIO_MANUAL mode */
} ext_isp_aspect_ratio;

/* Defines the format of the input Bayer image */
typedef enum {
    EXT_ISP_BAYER_RGGB    = 0,
    EXT_ISP_BAYER_GRBG    = 1,
    EXT_ISP_BAYER_GBRG    = 2,
    EXT_ISP_BAYER_BGGR    = 3,
    EXT_ISP_BAYER_MAX
} ext_isp_bayer_format;

typedef enum {
    EXT_ISP_VIDEO_DISPLAY_MODE_PREVIEW = 0x0,
    EXT_ISP_VIDEO_DISPLAY_MODE_PLAYBACK = 0x1,
    EXT_ISP_VIDEO_DISPLAY_MODE_MAX
} ext_isp_video_display_mode;

typedef struct {
    td_u64 luma_pix_sum;      /* luma sum of current frame */
    td_u32 luma_pix_average;  /* luma average of current frame */
    td_u64 pts;               /* PTS of current frame  */
} ext_isp_luma_info;

#define EXT_ISP_DCF_DRSCRIPTION_LENGTH  (32)
#define EXT_ISP_DCF_CAPTURE_TIME_LENGTH (20)
typedef struct {
    td_u8       image_description[EXT_ISP_DCF_DRSCRIPTION_LENGTH]; /* describes image */
    td_u8       make[EXT_ISP_DCF_DRSCRIPTION_LENGTH];              /* shows manufacturer of digital cameras */
    td_u8       model[EXT_ISP_DCF_DRSCRIPTION_LENGTH];             /* shows model number of digital cameras */
    td_u8       software[EXT_ISP_DCF_DRSCRIPTION_LENGTH]; /* shows firmware (internal software of digital cameras)
                                                               version number */

    td_u8       light_source; /* light source, actually this means white balance setting. '0' means unknown,
                                 '1' daylight, '2' fluorescent, '3' tungsten, '10' flash, '17' standard light A,
                                 '18' standard light B, '19' standard lightC, '20' D55, '21' D65,
                                 '22' D75, '255' other */
    td_u32      focal_length;   /* focal length of lens used to take image. unit is millimeter */
    td_u8       scene_type;     /* indicates the type of scene. value '0x01' means that the image was directly
                                     photographed. */
    td_u8       custom_rendered;          /* indicates the use of special processing on image data, such as rendering
                                               geared to output. 0 = normal process  1 = custom process */
    td_u8       focal_length_in35mm_film; /* indicates the equivalent focal length assuming a 35mm
                                               film camera, in mm */
    td_u8       scene_capture_type;       /* indicates the type of scene that was shot. 0 = standard,1 = landscape,
                                                2 = portrait,3 = night scene. */
    td_u8       gain_control;             /* indicates the degree of overall image gain adjustment. 0 = none,
                                               1 = low gain up,2 = high gain up,3 = low gain down,4 = high gain down. */
    td_u8       contrast;                 /* indicates the direction of contrast processing applied by the camera when
                                               the image was shot. 0 = normal,1 = soft,2 = hard */
    td_u8       saturation;               /* indicates the direction of saturation process applied by the camera when
                                               the image was shot. 0 = normal,1 = low saturation,2 = high saturation */
    td_u8       sharpness;                /* indicates the direction of sharpness processing applied by the camera
                                               when the image was shot. 0 = normal,1 = soft,2 = hard . */
    td_u8       metering_mode;            /* exposure metering method. '0' means unknown, '1' average,'2' center
                                               weighted average, '3' spot, '4' multi-spot, '5' multi-segment,
                                               '6' partial, '255' other */
} ext_isp_dcf_const_info;

typedef struct {
    td_u32      iso_speed_ratings;       /* CCD sensitivity equivalent to ag-hr film speedrate */
    td_u32      exposure_time;           /* exposure time (reciprocal of shutter speed). */
    td_u32      exposure_bias_value;     /* exposure bias (compensation) value of taking picture */
    td_u8       exposure_program;        /* exposure program that the camera used when image was taken.
                                              '1' means manual control, '2' program normal, '3' aperture priority,
                                              '4' shutter priority, '5' program creative (slow program),
                                              '6' program action(high-speed program),
                                              '7' portrait mode, '8' landscape mode */
    td_u32      f_number;                /* the actual F-number (F-stop) of lens when the image was taken */
    td_u32      max_aperture_value;      /* maximum aperture value of lens. */
    td_u8       exposure_mode;           /* indicates the exposure mode set when the image was shot.
                                              0 = auto exposure,1 = manual exposure, 2 = auto bracket */
    td_u8       white_balance;           /* indicates the white balance mode set when the image was shot.
                                              0 = auto white balance ,1 = manual white balance */
} ext_isp_dcf_update_info;

typedef struct {
    ext_isp_dcf_const_info  isp_dcf_const_info;
    ext_isp_dcf_update_info isp_dcf_update_info;
} ext_isp_dcf_info;

typedef struct {
    td_u8           capture_time[EXT_ISP_DCF_CAPTURE_TIME_LENGTH];  /* the date and time
                                                                         when the picture data was generated */
    td_bool         flash;              /* whether the picture is captured when a flash lamp is on */
    td_u32          digital_zoom_ratio; /* indicates the digital zoom ratio when the image was shot. if the
                                           * numerator of the recorded value is 0, this indicates that digital zoom
                                           * was not used. */
    ext_isp_dcf_info  isp_dcf_info;
} ext_isp_jpeg_dcf;

typedef struct {
    td_u32      iso;               /* ISP internal ISO : again*dgain*isp_gain */
    td_u32      exposure_time;     /* exposure time (reciprocal of shutter speed), unit is us */
    td_u32      isp_dgain;         /* gain value, isp gain */
    td_u32      again;             /* gain value, analog gain */
    td_u32      dgain;             /* gain value, digit gain */
    td_u32      ratio[3];          /* default exposure ratio of two adjacent frames in WDR mode */
    td_u32      isp_nr_strength;   /* reserved byte, the default value is 0 */
    td_u32      f_number;          /* the actual F-number (F-stop) of lens when the image was taken */
    td_u32      sensor_id;         /* which sensor is used */
    td_u32      sensor_mode;       /* sequence mode currently used by the sensor */
    td_u32      hmax_times;        /* sensor hmax_times, unit is ns */
    td_u32      vmax;              /* sensor vmax, unit is line */
    td_u32      vc_num;            /* when dump wdr frame, which is long or short  exposure frame. */
} ext_isp_frame_alg_info;

typedef struct {
    td_u32 numerator;   /* represents the numerator of a fraction, */
    td_u32 denominator; /* the denominator. */
} ext_isp_dng_rational;

/* defines the structure of DNG image dynamic infomation */
#define EXT_ISP_BAYER_CHN      (4)
#define EXT_ISP_CFACOLORPLANE  (3)
#define EXT_ISP_DNG_NP_SIZE    (6)
typedef struct {
    td_u32 black_level[EXT_ISP_BAYER_CHN];                     /* RO;range: [0x0, 0xFFFF]; black level */
    ext_isp_dng_rational as_shot_neutral[EXT_ISP_CFACOLORPLANE]; /* specifies the selected white balance at time of
                                                                    capture, encoded as the coordinates of a perfectly
                                                                    neutral color in linear reference space values. */
    td_double ad_noise_profile[EXT_ISP_DNG_NP_SIZE];           /* RO;describes the amount of noise in a raw image */
} ext_isp_dng_image_dynamic_info;

typedef struct {
    td_u32 color_temp;
    td_u16 ccm[9];
    td_u8  saturation;
} ext_isp_hdr_info;

typedef struct {
    ext_isp_hdr_info isp_hdr;
    td_u32         iso;
    td_u32         init_iso;
    td_u8          sns_wdr_mode;
} ext_isp_attach_info;

typedef enum {
    EXT_ISP_FRAME_FLAG_SNAP_FLASH        = 0x1 << 0,
    EXT_ISP_FRAME_FLAG_SNAP_CUR          = 0x1 << 1,
    EXT_ISP_FRAME_FLAG_SNAP_REF          = 0x1 << 2,
    EXT_ISP_FRAME_FLAG_SNAP_END          = 0x1 << 3,
    EXT_ISP_FRAME_FLAG_MIRROR            = 0x1 << 4,
    EXT_ISP_FRAME_FLAG_FLIP              = 0x1 << 5,
    EXT_ISP_FRAME_FLAG_DGAIN_BYPASS      = 0x1 << 6,
    EXT_ISP_FRAME_FLAG_OUTPUT_NONE       = 0x1 << 7,
    EXT_ISP_FRAME_FLAG_USER_SEND_DISCARD = 0x1 << 8,
    EXT_ISP_FRAME_FLAG_ROTATION_90       = 0x1 << 9,
    EXT_ISP_FRAME_FLAG_ROTATION_270      = 0x1 << 10,
    EXT_ISP_FRAME_FLAG_SVC_FLAG          = 0x1 << 11,
    EXT_ISP_FRAME_FLAG_SNAP_DROP         = 0x1 << 12,
    EXT_ISP_FRAME_FLAG_MAX
} ext_isp_frame_flag;

typedef struct {
    td_u32 iso;
    td_u32 isp_dgain;
    td_u32 exposure_time;
    td_u32 white_balance_gain[EXT_ISP_WB_GAIN_NUM];
    td_u32 color_temperature;
    td_u16 cap_ccm[EXT_ISP_CAP_CCM_NUM];
} ext_isp_config_info;

typedef struct {
    ext_isp_color_gamut color_gamut;
} ext_isp_colorgammut_info;

#define EXT_ISP_DIS_MOTIONDATA_NUM (9)
#define EXT_ISP_DIS_HOR_PART_NUM 3
#define EXT_ISP_DIS_VER_PART_NUM 4
#define EXT_ISP_DIS_REGION_NUM (EXT_ISP_DIS_HOR_PART_NUM * EXT_ISP_DIS_VER_PART_NUM)

typedef struct {
    td_s64 motion_data[EXT_ISP_DIS_MOTIONDATA_NUM];
} ext_isp_dis_motion_data;

typedef struct {
    td_s16 pmf_ctrl_x_end[EXT_ISP_DIS_VER_PART_NUM - 1];
    td_s16 pmf_ctrl_y_end[EXT_ISP_DIS_HOR_PART_NUM - 1];
} ext_isp_dis_multi_region_pmf_info;

typedef struct {
    ext_isp_dis_motion_data data[EXT_ISP_DIS_REGION_NUM];
    td_bool valid;
    td_bool multi_region_enable; /* 分区配准使能 */
    ext_isp_dis_multi_region_pmf_info multi_region_pmf_info;
} ext_isp_dis_motion_data_info;

typedef struct {
    td_u32                  width;
    td_u32                  height;
    td_u64                  phy_addr;
    td_u32                  stride;
} ext_isp_dis_scale_frame;  //  400 format

typedef struct {
    ext_isp_dis_motion_data_info gme_motion_data;
    ext_isp_dis_motion_data_info fliter_motion_data;
    ext_isp_dis_motion_data_info gyro_motion_data;
    ext_isp_dis_motion_data_info wr_motion_data;     // 1 /16 wr smallmotion data
    ext_isp_dis_scale_frame      wr_frame;           // 1 /16 ori frame for 3dnr wr
} ext_isp_motion_data;

typedef struct {
    ext_isp_motion_data motion_data;
} ext_isp_gdc_info;

typedef enum {
    EXT_ISP_ROTATION_0   = 0,
    EXT_ISP_ROTATION_90  = 1,
    EXT_ISP_ROTATION_180 = 2,
    EXT_ISP_ROTATION_270 = 3,
    EXT_ISP_ROTATION_MAX,
} ext_isp_rotation;

typedef struct {
    td_u64             fv; // 在 viproc 给后级模块送帧前，需从 isp 获取清晰度值

    ext_isp_crop_info  viproc_crop_info;
    ext_isp_size       viproc_out_size;
    ext_isp_rotation   viu_rotation;

    td_bool            dynamic_attr_en;
    td_s32             ppu_chn_id;
    ext_isp_crop_info  ppu_crop_info;
    ext_isp_size       ppu_out_size;
    ext_isp_rotation   ppu_rotation;
} ext_isp_private_info;

typedef struct {
    td_u32                  width;
    td_u32                  height;
    ext_isp_video_field     field;
    ext_isp_pixel_format    pixel_format;
    ext_isp_video_format    video_format;
    ext_isp_compress_mode   compress_mode;
    ext_isp_dynamic_range   dynamic_range;
    ext_isp_color_gamut     color_space;
    td_u32                  header_stride[EXT_MAX_COLOR_COMPONENT];
    td_u32                  stride[EXT_MAX_COLOR_COMPONENT];

    td_u64                  header_phy_addr[EXT_MAX_COLOR_COMPONENT];
    td_u64                  header_vir_addr[EXT_MAX_COLOR_COMPONENT];
    td_u64                  phy_addr[EXT_MAX_COLOR_COMPONENT];
    td_u64                  vir_addr[EXT_MAX_COLOR_COMPONENT];
    td_u64                  ext_phy_addr[EXT_MAX_COLOR_COMPONENT];
    td_u64                  ext_vir_addr[EXT_MAX_COLOR_COMPONENT];
    td_u32                  ext_stride[EXT_MAX_COLOR_COMPONENT];

    td_u16                  offset_top;       /* top offset of show area */
    td_u16                  offset_bottom;    /* bottom offset of show area */
    td_u16                  offset_left;      /* left offset of show area */
    td_u16                  offset_right;     /* right offset of show area */

    td_u32                  max_luminance;
    td_u32                  min_luminance;

    td_u32                  time_ref;
    td_u64                  pts;

    td_u64                  private_data;
    td_u32                  frame_flag;       /* frame_flag, can be OR operation. */
    ext_isp_video_supplement  supplement;
    ext_isp_data_bitwidth   bit_depth;
    ext_isp_private_info    private_info;
} ext_isp_video_frame;

typedef struct {
    ext_isp_video_frame v_frame;
    td_u32              pool_id;
    soc_mod_id          mod_id;
} ext_isp_video_frame_info;

typedef struct {
    td_u32 vb_size;

    td_u32 head_stride;
    td_u32 head_size;
    td_u32 head_y_size;

    td_u32 main_stride;
    td_u32 main_size;
    td_u32 main_y_size;

    td_u32 ext_stride;
    td_u32 ext_y_size;
} ext_isp_vb_cal_config;

typedef struct {
    ext_isp_pixel_format pixel_format;  /* bitmap's pixel format */
    td_u32             width;         /* bitmap's width */
    td_u32             height;        /* bitmap's height */
    td_void* EXT_ISP_ATTRIBUTE data;  /* address of bitmap's data */
} ext_isp_bitmap;

typedef struct {
    td_bool aspect;           /* RW;range: [0, 1];whether aspect ration  is keep */
    td_s32 x_ratio;           /* RW; range: [0, 100]; field angle ration of  horizontal,valid when aspect=0. */
    td_s32 y_ratio;           /* RW; range: [0, 100]; field angle ration of  vertical,valid when aspect=0. */
    td_s32 xy_ratio;          /* RW; range: [0, 100]; field angle ration of  all,valid when aspect=1. */
    td_s32 center_x_offset;   /* RW; range: [-511, 511]; horizontal offset of the image distortion center
                                   relative to image center. */
    td_s32 center_y_offset;   /* RW; range: [-511, 511]; vertical offset of the image distortion center relative
                                   to image center. */
    td_s32 distortion_ratio;  /* RW; range: [-300, 500]; LDC distortion ratio.when spread on,distortion_ratio
                                   range should be [0, 500] */
} ext_isp_ldc_attr;

#define SRC_LENS_COEF_SEG       (2)
#define DST_LENS_COEF_SEG       (3)
#define SRC_LENS_COEF_NUM       (4)
#define DST_LENS_COEF_NUM       (4)
#define DST_LENS_COEF_SEG_POINT (DST_LENS_COEF_SEG - 1)
typedef struct {
    td_s32 focal_len_x;                  /* RW; focal length in horizontal direction, with 2 decimal numbers */
    td_s32 focal_len_y;                  /* RW; focal length in vertical direction, with 2 decimal numbers */
    td_s32 coor_shift_x;                 /* RW; coordinate of image center, with 2 decimal numbers */
    td_s32 coor_shift_y;                 /* RW; Y coordinate of image center, with 2 decimal numbers */
    td_s32 src_cali_ratio[SRC_LENS_COEF_SEG][SRC_LENS_COEF_NUM];  /* RW; lens distortion coefficients of the source
                                                                       image, with 5 decimal numbers */
    td_s32 src_jun_pt;                                            /* RW; junction point of the two segments */
    td_s32 dst_cali_ratio[DST_LENS_COEF_SEG][DST_LENS_COEF_NUM];  /* RW; lens distortion coefficients, with 5 decimal
                                                                       numbers */
    td_s32 dst_jun_pt[DST_LENS_COEF_SEG_POINT];                   /* RW; junction point of the three segments */
    td_s32 max_du;                                                /* RW; max undistorted distance before 3rd
                                                                       polynomial drop, with 16bits decimal */
} ext_isp_ldc_v2_attr;

typedef enum {
    EXT_ISP_LDC_VIEW_TYPE_ALL = 0,  /* View scale all but x and y independtly, this will keep both x and y axis ,
                                but corner maybe lost */
    EXT_ISP_LDC_VIEW_TYPE_CROP = 1,  /* Not use view scale, this will lost some side and corner */
    EXT_ISP_LDC_VIEW_TYPE_MAX,
} ext_isp_ldc_view_type;

typedef struct {
    ext_isp_ldc_view_type view_type; /* RW; range: [0, 1], 0: all mode, 1: crop mode. */
    td_s32 center_x_offset;        /* RW; range: ABS(center_x_offset)->[0, min(128, width*0.08)], horizontal offset
                                        of the image distortion center relative to image center. */
    td_s32 center_y_offset;        /* RW; range: ABS(center_x_offset)->[0, min(128, height*0.08)], vertical offset of
                                        the image distortion center relative to image center. */
    td_s32 distortion_ratio;       /* RW; range: [-300, 300], LDC distortion ratio. */
    td_s32 min_ratio;              /* RW; range: [-300, 300], to remove the black region around when performing
                                        pinchusion distortion correction with crop mode. */
} ext_isp_ldc_v3_attr;

typedef enum {
    EXT_ISP_ROTATION_VIEW_TYPE_ALL = 0, /* View all source Image,no lose */
    EXT_ISP_ROTATION_VIEW_TYPE_TYPICAL = 1, /* View from rotation Image with source size,same lose */
    EXT_ISP_ROTATION_VIEW_TYPE_INSIDE = 2, /* View with no black section,all  in dest Image */
    EXT_ISP_ROTATION_VIEW_TYPE_MAX,
} ext_isp_rotation_view_type;

typedef struct {
    ext_isp_rotation_view_type view_type;       /* RW;range: [0, 2];rotation mode */
    td_u32                     angle;           /* RW;range: [0,360];rotation angle:[0,360] */
    td_s32                     center_x_offset; /* RW;range: [-511, 511];horizontal offset of the image
                                                    distortion center relative to image center. */
    td_s32                     center_y_offset; /* RW;range: [-511, 511];vertical offset of the image distortion center
                                                   relative to image center. */
    ext_isp_size               dest_size;       /* RW;dest size of any angle rotation */
} ext_isp_rotation_ex;

#define ISP_SHARPEN_FREQ_CORING_LENGTH      8
#define SHRP_GAIN_LUT_SIZE                  64
#define ISP_SHARPEN_MOT_NUM          16  /* Not support for 850 */
#define ISP_SHARPEN_RLYWGT_NUM       16  /* Not support for 850 */
#define ISP_SHARPEN_STDGAIN_NUM      16  /* Not support for 850 */
#define ISP_AUTO_ISO_STRENGTH_NUM       16
#define ISP_SHARPEN_LUMA_NUM            32
#define ISP_SHARPEN_GAIN_NUM            32
#define ISP_SHAREPEN_PNL_POS_TH_LEN     7
#define ISP_SHAREPEN_PNL_GAMMA_GAIN_LEN 8
#define ISP_SHAREPEN_RLSC_GAIN_LEN      33
#define ISP_SHAREPEN_ROI_NUM_MAX        10

typedef struct {
    td_bool  static_resh;
    td_u8    mf_thd_sel_d;
    td_u8    hf_thd_sel_d;
    td_u8    gain_thd_sel_d;
    td_u8    dir_var_scale;
    td_u8    dir_rly[2];       /* dir rly size 2 */
    td_u8    range_thd1;       /* U8.0 */
    td_u8    range_dir_wgt1;   /* U0.7 */

    td_u8    en_hf_eps;        /* U1.0 */
    td_u8    hf_eps_val0;      /* U0.7 */

    td_u8    max_var_clip_min; /* 8 bit */
    td_u16   o_max_chg;
    td_u16   u_max_chg;
    td_u8    sht_var_wgt1;     //1a22
    td_u8    osht_var_wgt1;    // 850 overshootchrl
    td_u8    usht_var_wgt1;    // 850 undershootctrl
    td_u8    sht_var_diff_wgt0;
    td_u8    osht_var_diff_wgt0;
    td_u8    usht_var_diff_wgt0;
    td_u8    sht_var_thd0;
    td_u8    osht_var_thd0;
    td_u8    usht_var_thd0;
    td_u8    sht_var_sft;
    td_u8    lmt_mf[ISP_SHARPEN_FREQ_CORING_LENGTH];
    td_u8    lmt_hf[ISP_SHARPEN_FREQ_CORING_LENGTH];

    td_u8    skin_src_sel;
    td_u8    skin_cnt_thd[2];   /* skin cnt thd size 2 */
    td_u8    skin_edge_sft;
    td_u8    skin_edge_thd[2];  /* skin edg thd size 2 */
    td_u16   skin_accum_thd[2]; /* skin acc thd size 2 */
    td_u8    skin_accum_wgt[2]; /* skin acc wgt size 2 */
    td_u8    skin_cnt_mul;
    td_s16   skin_accum_mul;

    td_u8    chr_r_var_sft;
    td_u8    rc_bmin;
    td_u8    rc_bmax;
    td_u8    rc_rmin;
    td_u8    rc_rmax;
    td_u8    chr_r_ori_cb;    /* U8.0 */
    td_u8    chr_r_ori_cr;    /* U8.0 */
    td_u8    chr_r_sft[4];    /* chr r sft size 4 */
    td_u8    chr_r_thd[2];    /* chr r thd size 2 */
    td_u8    gc_bmin;
    td_u8    gc_bmax;
    td_u8    gc_rmin;
    td_u8    gc_rmax;
    td_u8    chr_g_ori_cb;    /* U8.0 */
    td_u8    chr_g_ori_cr;    /* U8.0 */
    td_u8    chr_g_sft[4];    /* chr g sft size 4 */
    td_u8    chr_g_thd[2];    /* chr g thd size 2 */
    td_u8    chr_b_var_sft;
    td_u8    bc_bmin;
    td_u8    bc_bmax;
    td_u8    bc_rmin;
    td_u8    bc_rmax;
    td_u8    chr_b_ori_cb;    /* U8.0 */
    td_u8    chr_b_ori_cr;    /* U8.0 */
    td_u8    chr_b_sft[4];    /* chr b sft size 4 */
    td_u8    chr_b_thd[2];    /* chr b thd size 2 */

    td_u8    luma_src_sel;
    td_u8    sht_noise_max;
    td_u8    sht_noise_min;

    /* filter */
    td_s8    lpf_coef_ud[3];  /* S8.0  lpf coef ud size  3  */
    td_s8    lpf_coef_d[3];   /* S8.0  lpf coef  d size  3  */
    td_s8    hsf_coef_ud[3];  /* S8.0  hsf coef ud size  3  */
    td_s8    hsf_coef_d0[13]; /* S8.0  hsf coef d0 size 13  */
    td_s8    hsf_coef_d1[13]; /* S8.0  hsf coef d1 size 13  */
    td_s8    hsf_coef_d2[13]; /* S8.0  hsf coef d2 size 13  */
    td_s8    hsf_coef_d3[13]; /* S8.0  hsf coef d3 size 13  */

    td_u8    lpf_sft_ud;     /* U4.0--[3, 15] */
    td_u8    lpf_sft_d;      /* U4.0--[3, 15] */
    td_u8    hsf_sft_ud;     /* U4.0--[3, 15] */
    td_u8    hsf_sft_d;      /* U4.0--[3, 15] */

    td_u8    en_shp8_dir;

    /* shoot ctrl */
    td_u8    lf_gain_wgt;      // 1A22
    td_u8    hf_gain_sft;
    td_u8    mf_gain_sft;
    td_u8    sht_var_sel;
    td_u8    sht_var5x5_sft;
    td_u8    detail_thd_sel;
    td_u8    detail_thd_sft;

    td_u8    en_var7x9_calc;     // 1A22
} isp_sharpen_static_reg_cfg;

typedef struct {
    td_bool  resh;
    td_u8    mf_thd_sft_d;
    td_u8    mf_thd_sel_ud;
    td_u8    mf_thd_sft_ud;
    td_u8    hf_thd_sft_d;
    td_u8    hf_thd_sel_ud;
    td_u8    hf_thd_sft_ud;
    td_u8    dir_var_sft;
    td_u8    range_thd0;        /* U8.0 */
    td_u8    range_dir_wgt0;    /* U0.7 */

    td_u8    hf_eps_var_thr0;
    td_u8    hf_eps_var_thr1;
    td_u8    hf_eps_val1;

    td_u8    sel_pix_wgt;
    td_u8    osht_var_wgt0;
    td_u8    usht_var_wgt0;
    td_u8    osht_var_diff_thd[2]; /* osht var diff thd size 2 */
    td_u8    osht_var_diff_wgt1;
    td_u8    usht_var_diff_thd[2]; /* usht var diff thd size 2 */
    td_u8    usht_var_diff_wgt1;

    td_u8    rmf_gain_scale;
    td_u8    bmf_gain_scale;
    td_s16   range_dir_mul;       /* convert division to multiplication.            12-bit signed integer  S8.4 */
    td_s16   hf_eps_mul;          /* convert division to multiplication.            12-bit signed integer  S8.4 */
    td_s16   osht_var_diff_mul;   /* convert division to multiplication.            12-bit signed integer  S8.4 */
    td_s16   usht_var_diff_mul;   /* convert division to multiplication.            12-bit signed integer  S8.4 */

    //////////////////////// 1A22 ADD
    td_u8    gain_thd_sft_d;
    td_u8    gain_thd_sel_ud;
    td_u8    gain_thd_sft_ud;

    td_u8    sht_var_wgt0;
    td_u8    sht_var_diff_thd[2]; /* sht var diff thd size 2 */
    td_u8    sht_var_diff_wgt1;

    td_s16   sht_var_diff_mul;    /* convert division to multiplication.            12-bit signed integer    S8.4 */

    td_u8    dir_var_thd;
    td_u8    mot_thd;
    td_u8    std_comb_mode;
    td_u8    std_comb_alpha;
    td_u8    mot_interp_mode;

    /* B3 add */
    td_bool  sharpen_pnl_en;
    td_bool  sharpen_rlsc_en;
    td_bool  sharpen_dir_en;
    td_u8    sharpen_node_cnt;
    td_u8    sharpen_cir_sft;

    td_u16   sharpen_off_center;
    td_u16   sharpen_central_posx;
    td_u16   sharpen_central_posy;

    td_u8   sharpen_gamma_pos[ISP_SHAREPEN_PNL_POS_TH_LEN];
    td_u8   sharpen_gamma_gain[ISP_SHAREPEN_PNL_GAMMA_GAIN_LEN];
    td_u8   sharpen_rlsc_gain[ISP_SHAREPEN_RLSC_GAIN_LEN];
    td_u16  sharpen_pnl_th[ISP_SHAREPEN_PNL_POS_TH_LEN];
    td_u8   sharpen_pnl_str[ISP_SHAREPEN_PNL_GAMMA_GAIN_LEN];
    /* B3 add end */
} isp_sharpen_default_dyna_reg_cfg;

typedef struct {
    td_bool  resh;
    td_bool  pre_reg_new_en;
    td_bool  switch_mode;
    td_u8    buf_id;
    td_u16   mf_gain_d[SHRP_GAIN_LUT_SIZE];
    td_u16   mf_gain_ud[SHRP_GAIN_LUT_SIZE];
    td_u16   hf_gain_d[SHRP_GAIN_LUT_SIZE];
    td_u16   hf_gain_ud[SHRP_GAIN_LUT_SIZE];
    td_u8    osht_amt;
    td_u8    usht_amt;

    td_u8    en_chr_ctrl;
    td_u8    chr_r_gain;       /* U1.5  [0 32] */
    td_u16   chr_g_gain;       /* U1.5  [0 32] */
    td_u16   chr_gmf_gain;     /* U1.5  [0 32] */
    td_u8    chr_b_gain;       /* U1.5  [0 32] */
    td_u8    en_skin_ctrl;
    td_u8    skin_edge_thd[2];  /* skin edg thd size 2 */
    td_u8    skin_edge_wgt[2]; /* U0.5  [0 31] skin edge wgt size 2  */
    td_s16   skin_edge_mul;
    td_u8    skin_max_u;
    td_u8    skin_min_u;
    td_u8    skin_max_v;
    td_u8    skin_min_v;

    td_u8    en_sht_ctrl_by_var;
    td_u8    sht_bld_rt;
    td_u8    sht_var_thd1;            // 1A22
    td_u8    osht_var_thd1;
    td_u8    usht_var_thd1;
    td_u8    en_luma_ctrl;
    td_bool  en_detail_ctrl;
    td_u8    detail_osht_amt;
    td_u8    detail_usht_amt;
    td_u8    luma_wgt[ISP_SHARPEN_LUMA_NUM];
    td_u16   sht_var_mul;     /* convert division to multiplication.            11-bit unsigned integer  U7.4 */ //1A22
    td_u16   osht_var_mul;        /* convert division to multiplication.            11-bit unsigned integer  U7.4 */
    td_u16   usht_var_mul;        /* convert division to multiplication.            11-bit unsigned integer  U7.4 */
    td_s16   detail_osht_mul;
    td_s16   detail_usht_mul;
    td_u8    dir_diff_sft;
    td_u8    dir_rt[2];     /* dir rt size 2 */
    td_s16   chr_r_mul;      /* 9-bit signed integer      S5.4 */
    td_s16   chr_g_mul;      /* 9-bit signed integer      S5.4 */
    td_s16   chr_gmf_mul;    /* 9-bit signed integer      S5.4 */
    td_s16   chr_b_mul;      /* 9-bit signed integer      S5.4 */
    td_u16   o_max_gain;   /* 11bit */
    td_u16   u_max_gain;   /* 11bit */
    td_u8    detail_osht_thr[2]; /* detail osht thr size 2 */
    td_u8    detail_usht_thr[2]; /* detail usht thr size 2 */
    td_u32   update_index;

    ////////////////////////////////////// 1A22 ADD
    td_u16   exluma_thd;     /* 9bit */
    td_u16   exluma_out_thd; /* 9bit */
    td_u16   hard_luma_thd;  /* 9bit */
    td_u8    exluma_mul;     /* 8bit */

    td_u8    en_mot_ctrl;       /* 1bit */
    td_u8    en_std_adj_by_mot; /* 1bit */
    td_u8    en_std_adj_by_y;   /* 1bit */
    td_u16   var5_low_thd;      /* 10bit */
    td_u16   var5_mid_thd;      /* 10bit */
    td_u16   var5_high_thd;     /* 10bit */
    td_u16   var7x9_low_thd;    /* 10bit */
    td_u16   var7x9_high_thd;   /* 10bit */

    td_u8    edge_osht_amt;     /* 10bit */
    td_u8    edge_usht_amt;     /* 10bit */

    td_u8    rly_wgt[ISP_SHARPEN_RLYWGT_NUM];                  /* 8bit */
    td_u8    std_gain_by_mot_lut[ISP_SHARPEN_STDGAIN_NUM];     /* 6bit */
    td_u8    std_offset_by_mot_lut[ISP_SHARPEN_STDGAIN_NUM];   /* 8bit */
    td_u8    std_gain_by_y_lut[ISP_SHARPEN_STDGAIN_NUM];       /* 6bit */
    td_u8    std_offset_by_y_lut[ISP_SHARPEN_STDGAIN_NUM];     /* 8bit */
    td_u8    mf_mot_dec[ISP_SHARPEN_MOT_NUM];                  /* 6bit */
    td_u8    hf_mot_dec[ISP_SHARPEN_MOT_NUM];                  /* 6bit */
    td_u8    lmf_mot_gain[ISP_SHARPEN_MOT_NUM];                /* 8bit */

    // The Following Registers have not be calculated in the current version
    td_u16    max_gain_mf_adj_by_rly;     /* 10bit */
    td_u16    max_gain_hf_adj_by_rly;     /* 10bit */
    td_u16    rly_wgt_only_for_dir_gain;     /* 10bit */

    // Face ROI
    td_u8   roi_region_cnt;  /* 8bit */
    td_u16  roi_x0[ISP_SHAREPEN_ROI_NUM_MAX];      /* 16bit */
    td_u16  roi_y0[ISP_SHAREPEN_ROI_NUM_MAX];      /* 16bit */
    td_u16  roi_x1[ISP_SHAREPEN_ROI_NUM_MAX];      /* 16bit */
    td_u16  roi_y1[ISP_SHAREPEN_ROI_NUM_MAX];      /* 16bit */

    /////////////////////////////////////  1A22 ADD END
} isp_sharpen_mpi_dyna_reg_cfg;

typedef struct {
    isp_sharpen_mpi_dyna_reg_cfg          mpi_dyna_reg_cfg;
    isp_sharpen_default_dyna_reg_cfg      default_dyna_reg_cfg;
} isp_sharpen_dyna_reg_cfg;

typedef struct {
    td_bool  enable;
    td_bool  lut2_stt_en;
    isp_sharpen_dyna_reg_cfg          dyna_reg_cfg;
    isp_sharpen_static_reg_cfg        static_reg_cfg;
} ext_isp_sharpen_reg_cfg;

typedef enum {
    EXT_ISP_DATA_RATE_X1 = 0,         /* RW; output 1 pixel per clock */
    EXT_ISP_DATA_RATE_X2 = 1,         /* RW; output 2 pixel per clock */
    EXT_ISP_DATA_RATE_MAX
} ext_isp_data_rate;

typedef enum {
    EXT_ISP_WDR_MODE_NONE = 0,
    EXT_ISP_WDR_MODE_BUILT_IN,
    EXT_ISP_WDR_MODE_QUDRA,
    EXT_ISP_WDR_MODE_2TO1_LINE,
    EXT_ISP_WDR_MODE_2TO1_FRAME,
    EXT_ISP_WDR_MODE_2TO1_FRAME_FULL_RATE,
    EXT_ISP_WDR_MODE_3TO1_LINE,
    EXT_ISP_WDR_MODE_3TO1_FRAME,
    EXT_ISP_WDR_MODE_3TO1_FRAME_FULL_RATE,
    EXT_ISP_WDR_MODE_4TO1_LINE,
    EXT_ISP_WDR_MODE_4TO1_FRAME,
    EXT_ISP_WDR_MODE_4TO1_FRAME_FULL_RATE,
    EXT_ISP_WDR_MODE_MAX,
} ext_isp_wdr_mode;

typedef enum {
    EXT_ISP_FRAME_INTERRUPT_START,
    EXT_ISP_FRAME_INTERRUPT_EARLY,
    EXT_ISP_FRAME_INTERRUPT_EARLY_END,
    EXT_ISP_FRAME_INTERRUPT_EARLY_ONE_BUF,
    EXT_ISP_FRAME_INTERRUPT_EARLY_END_ONE_BUF,
    EXT_ISP_FRAME_INTERRUPT_MAX,
} ext_isp_frame_interrupt_type;

typedef struct {
    ext_isp_frame_interrupt_type int_type;
    td_u32 early_line;
} ext_isp_frame_interrupt_attr;

/* low delay struct, 80 byte */
typedef struct {
    td_u16 y_height;
    td_u32 ysize;
    td_u8 reserve1[26];
    td_u16 c_height;
    td_u32 csize;
    td_u8 reserve2[26];

    td_u8 frame_ok;
    td_u8 tunl_frame;
    td_u8 frame_finish;
    td_u8 miss_vedu;
    td_u8 miss_jpege;
    td_u32 buf_line;
    td_u8 all_online;
    td_u8 reserve3[6];
} __attribute__((packed)) ext_isp_low_delay_info;

typedef enum {
    EXT_ISP_OPERATION_MODE_AUTO   = 0,
    EXT_ISP_OPERATION_MODE_MANUAL = 1,
    EXT_ISP_OPERATION_MODE_MAX
} ext_isp_operation_mode;

/* 3:seg;34/64/68:coef num;scale系数矩阵，非魔鬼数字 */
#define MCP_SCALE_COEF_NUM 64
typedef struct {
    td_u32 coefficient_lanczos3[MCP_SCALE_COEF_NUM];
    td_u32 coefficient8_cubic[MCP_SCALE_COEF_NUM];
    td_u32 ive_coefficient8_cubic[3][MCP_SCALE_COEF_NUM];

    td_u32 coefficient8_lanczos2_8tap[MCP_SCALE_COEF_NUM];

    td_u32 coefficient8_3_m_a19[MCP_SCALE_COEF_NUM];

    td_u32 coefficient8_2_m_a05[MCP_SCALE_COEF_NUM];

    td_u32 coefficient8_1_5_m_a05[MCP_SCALE_COEF_NUM];

    td_u32 ycoefficient4_5_m_a15[MCP_SCALE_COEF_NUM];

    td_u32 ycoefficient4_cubic[MCP_SCALE_COEF_NUM];

    td_u32 ycoefficient4_4_m_a15[MCP_SCALE_COEF_NUM];

    td_u32 ycoefficient4_2_m_a05[MCP_SCALE_COEF_NUM];
    td_u32 ycoefficient4_1_5_m_a15[MCP_SCALE_COEF_NUM];

    td_u32 coefficient6_6_m_a25[MCP_SCALE_COEF_NUM];

    td_u32 coefficient6_cubic[MCP_SCALE_COEF_NUM];

    td_u32 coefficient6_5_m_a25[MCP_SCALE_COEF_NUM];

    td_u32 coefficient6_4_m_a20[MCP_SCALE_COEF_NUM];

    td_u32 coefficient6_3_m_a15[MCP_SCALE_COEF_NUM];

    td_u32 coefficient6_2_m_a05[MCP_SCALE_COEF_NUM];
    td_u32 ive_coefficient6_2_m_a05[MCP_SCALE_COEF_NUM]; /* 由于IVE需求增加字段 */
    td_u32 coefficient6_1_5_m_a05[MCP_SCALE_COEF_NUM];

    td_u32 ccoefficient4_5_m_a15[MCP_SCALE_COEF_NUM];

    td_u32 ccoefficient4_cubic[MCP_SCALE_COEF_NUM];

    td_u32 ccoefficient4_4_m_a15[MCP_SCALE_COEF_NUM];
    td_u32 ccoefficient4_2_m_a05[MCP_SCALE_COEF_NUM];
    td_u32 ccoefficient4_1_5_m_a15[MCP_SCALE_COEF_NUM];

    td_u32 ive_lcoefficient2_2_m_a05_hor[MCP_SCALE_COEF_NUM]; /* 双线性 由于IVE需求增加字段 */
    td_u32 ive_lcoefficient2_2_m_a05_vel[MCP_SCALE_COEF_NUM]; /* 双线性 由于IVE需求增加字段 */
    td_u32 ive_chcoefficient2_2_m_a05_hor[MCP_SCALE_COEF_NUM]; /* 双线性 由于IVE需求增加字段 */
    td_u32 ive_chcoefficient2_2_m_a05_vel[MCP_SCALE_COEF_NUM]; /* 双线性 由于IVE需求增加字段 */

    td_u32 coefficient8_lanczos2_8tap_dis[MCP_SCALE_COEF_NUM];
    td_u32 coefficient6_4_m_a20_dis[MCP_SCALE_COEF_NUM];

    /* 视频缩放系数 */
    td_u32 hor_luma_coef7_00[MCP_SCALE_COEF_NUM];
    td_u32 hor_luma_coef6_75[MCP_SCALE_COEF_NUM];
    td_u32 hor_luma_coef6_50[MCP_SCALE_COEF_NUM];
    td_u32 hor_luma_coef6_25[MCP_SCALE_COEF_NUM];
    td_u32 hor_luma_coef6_00[MCP_SCALE_COEF_NUM];
    td_u32 hor_luma_coef5_75[MCP_SCALE_COEF_NUM];
    td_u32 hor_luma_coef5_50[MCP_SCALE_COEF_NUM];
    td_u32 hor_luma_coef5_25[MCP_SCALE_COEF_NUM];
    td_u32 hor_luma_coef5_00[MCP_SCALE_COEF_NUM];
    td_u32 hor_luma_coef4_50[MCP_SCALE_COEF_NUM];
    td_u32 hor_luma_coef4_00[MCP_SCALE_COEF_NUM];
    td_u32 hor_luma_coef3_75[MCP_SCALE_COEF_NUM];
    td_u32 hor_luma_coef3_375[MCP_SCALE_COEF_NUM];
    td_u32 hor_luma_coef3_00[MCP_SCALE_COEF_NUM];
    td_u32 hor_luma_coef2_50[MCP_SCALE_COEF_NUM];
    td_u32 hor_luma_coef2_00[MCP_SCALE_COEF_NUM];
    td_u32 hor_luma_coef1_50[MCP_SCALE_COEF_NUM];
    td_u32 hor_luma_coef2_50_a20[MCP_SCALE_COEF_NUM];
    td_u32 hor_luma_coef2_00_a15[MCP_SCALE_COEF_NUM];
    td_u32 hor_luma_coef1_50_a15[MCP_SCALE_COEF_NUM];
    td_u32 hor_luma_coef1_00_a15[MCP_SCALE_COEF_NUM];
    td_u32 hor_luma_coef1_00_a10[MCP_SCALE_COEF_NUM];
    td_u32 hor_luma_coef0_50_a05[MCP_SCALE_COEF_NUM];
    td_u32 hor_luma_coef0_10_a01[MCP_SCALE_COEF_NUM];

    td_u32 hor_chroma_coef7_00[MCP_SCALE_COEF_NUM];
    td_u32 hor_chroma_coef6_75[MCP_SCALE_COEF_NUM];
    td_u32 hor_chroma_coef6_50[MCP_SCALE_COEF_NUM];
    td_u32 hor_chroma_coef6_25[MCP_SCALE_COEF_NUM];
    td_u32 hor_chroma_coef6_00[MCP_SCALE_COEF_NUM];
    td_u32 hor_chroma_coef5_75[MCP_SCALE_COEF_NUM];
    td_u32 hor_chroma_coef5_50[MCP_SCALE_COEF_NUM];
    td_u32 hor_chroma_coef5_25[MCP_SCALE_COEF_NUM];
    td_u32 hor_chroma_coef5_00[MCP_SCALE_COEF_NUM];
    td_u32 hor_chroma_coef4_50[MCP_SCALE_COEF_NUM];
    td_u32 hor_chroma_coef4_00[MCP_SCALE_COEF_NUM];
    td_u32 hor_chroma_coef3_75[MCP_SCALE_COEF_NUM];
    td_u32 hor_chroma_coef3_375[MCP_SCALE_COEF_NUM];
    td_u32 hor_chroma_coef3_00[MCP_SCALE_COEF_NUM];
    td_u32 hor_chroma_coef2_50[MCP_SCALE_COEF_NUM];
    td_u32 hor_chroma_coef2_00[MCP_SCALE_COEF_NUM];
    td_u32 hor_chroma_coef1_50[MCP_SCALE_COEF_NUM];
    td_u32 hor_chroma_coef1_50_a15[MCP_SCALE_COEF_NUM];
    td_u32 hor_chroma_coef1_00_a10[MCP_SCALE_COEF_NUM];
    td_u32 hor_chroma_coef0_50_a05[MCP_SCALE_COEF_NUM];
    td_u32 hor_chroma_coef0_10_a01[MCP_SCALE_COEF_NUM];

    td_u32 ver_luma_coef7_00[MCP_SCALE_COEF_NUM];
    td_u32 ver_luma_coef6_75[MCP_SCALE_COEF_NUM];
    td_u32 ver_luma_coef6_50[MCP_SCALE_COEF_NUM];
    td_u32 ver_luma_coef6_25[MCP_SCALE_COEF_NUM];
    td_u32 ver_luma_coef6_00[MCP_SCALE_COEF_NUM];
    td_u32 ver_luma_coef5_75[MCP_SCALE_COEF_NUM];
    td_u32 ver_luma_coef5_50[MCP_SCALE_COEF_NUM];
    td_u32 ver_luma_coef5_25[MCP_SCALE_COEF_NUM];
    td_u32 ver_luma_coef5_00[MCP_SCALE_COEF_NUM];
    td_u32 ver_luma_coef4_50[MCP_SCALE_COEF_NUM];
    td_u32 ver_luma_coef4_00[MCP_SCALE_COEF_NUM];
    td_u32 ver_luma_coef3_75[MCP_SCALE_COEF_NUM];
    td_u32 ver_luma_coef3_75_a15[MCP_SCALE_COEF_NUM];
    td_u32 ver_luma_coef3_375[MCP_SCALE_COEF_NUM];
    td_u32 ver_luma_coef3_00[MCP_SCALE_COEF_NUM];
    td_u32 ver_luma_coef2_50[MCP_SCALE_COEF_NUM];
    td_u32 ver_luma_coef2_00[MCP_SCALE_COEF_NUM];
    td_u32 ver_luma_coef1_50[MCP_SCALE_COEF_NUM];
    td_u32 ver_luma_coef2_50_a20[MCP_SCALE_COEF_NUM];
    td_u32 ver_luma_coef2_00_a15[MCP_SCALE_COEF_NUM];
    td_u32 ver_luma_coef1_50_a15[MCP_SCALE_COEF_NUM];
    td_u32 ver_luma_coef1_00_a10[MCP_SCALE_COEF_NUM];
    td_u32 ver_luma_coef0_50_a05[MCP_SCALE_COEF_NUM];
    td_u32 ver_luma_coef0_10_a01[MCP_SCALE_COEF_NUM];

    td_u32 ver_chroma_coef7_00[MCP_SCALE_COEF_NUM];
    td_u32 ver_chroma_coef6_75[MCP_SCALE_COEF_NUM];
    td_u32 ver_chroma_coef6_50[MCP_SCALE_COEF_NUM];
    td_u32 ver_chroma_coef6_25[MCP_SCALE_COEF_NUM];
    td_u32 ver_chroma_coef6_00[MCP_SCALE_COEF_NUM];
    td_u32 ver_chroma_coef5_75[MCP_SCALE_COEF_NUM];
    td_u32 ver_chroma_coef5_50[MCP_SCALE_COEF_NUM];
    td_u32 ver_chroma_coef5_25[MCP_SCALE_COEF_NUM];
    td_u32 ver_chroma_coef5_00[MCP_SCALE_COEF_NUM];
    td_u32 ver_chroma_coef4_50[MCP_SCALE_COEF_NUM];
    td_u32 ver_chroma_coef4_00[MCP_SCALE_COEF_NUM];
    td_u32 ver_chroma_coef3_75[MCP_SCALE_COEF_NUM];
    td_u32 ver_chroma_coef3_375[MCP_SCALE_COEF_NUM];
    td_u32 ver_chroma_coef3_00[MCP_SCALE_COEF_NUM];
    td_u32 ver_chroma_coef2_50[MCP_SCALE_COEF_NUM];
    td_u32 ver_chroma_coef2_00[MCP_SCALE_COEF_NUM];
    td_u32 ver_chroma_coef1_50[MCP_SCALE_COEF_NUM];
    td_u32 ver_chroma_coef1_50_a15[MCP_SCALE_COEF_NUM];
    td_u32 ver_chroma_coef1_00_a10[MCP_SCALE_COEF_NUM];
    td_u32 ver_chroma_coef0_50_a05[MCP_SCALE_COEF_NUM];
    td_u32 ver_chroma_coef0_10_a01[MCP_SCALE_COEF_NUM];

    /* for ive 预乘 */
    td_u32 ive_coeff_bilinear_8[MCP_SCALE_COEF_NUM];
    td_u32 ive_coeff_bilinear_6[MCP_SCALE_COEF_NUM];
    td_u32 ive_coeff_bilinear_4[MCP_SCALE_COEF_NUM];
} mcp_scale_coeff;

/* Defines the working mode of ISP : 0 = automatic mode, 1 = manual mode */
typedef enum {
    OP_TYPE_AUTO    = 0,
    OP_TYPE_MANUAL  = 1,
    OP_TYPE_BUTT
} ext_isp_op_type;

#define ISP_3A_FACE_NUM_MAX 4

typedef struct {
    td_u32 y;
    td_u32 u;
    td_u32 v;
} ext_isp_yuv_avg;

typedef struct {
    td_bool valid;
    td_bool yuv_avg_valid;
    td_u8 face_num;
    td_u64 pts;
    td_u32 base_width;
    td_u32 base_height;
    ext_isp_rect face_rect[ISP_3A_FACE_NUM_MAX];
    ext_isp_yuv_avg face_yuv_avg[ISP_3A_FACE_NUM_MAX];
    ext_isp_rotation rotation;
    td_bool flip_en;
    td_bool mirror_en;
} ext_isp_face_stat;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __DRV_ISP_VIDEO_EXT_H__ */

