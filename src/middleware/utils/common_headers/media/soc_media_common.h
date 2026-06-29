/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: meida common
 * Author: Hisilicon multimedia software group
 * Create: 2025/05/17
 * History:
 */
#ifndef SOC_MEDIA_COMMON_H
#define SOC_MEDIA_COMMON_H

#include "td_type.h"
#include "td_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif

#define EXT_MAX_COLOR_COMPONENT      3
#define EXT_GPS_EXPRESS_NUM          3
#define EXT_GPS_GRP_NUM              2
#define EXT_EXIF_DESCRIPTION_LENGTH  32
#define EXT_EXIF_CAPTURE_TIME_LENGTH 20
#define EXT_ISP_WB_GAIN_NUM          4
#define EXT_ISP_EXPOSURE_RATIO_NUM   3
#define EXT_ISP_CAP_CCM_NUM          9  /* 3*3=9 matrix */
#define EXT_ISP_WDR_MAX_FRAME_NUM    4
#define EXT_LOW_DELAY_RESERVE1       26
#define EXT_LOW_DELAY_RESERVE2       26
#define EXT_LOW_DELAY_RESERVE3       60

typedef enum  {
    EXT_VIDEO_FIELD_TOP = 1,
    EXT_VIDEO_FIELD_BOTTOM,
    EXT_VIDEO_FIELD_INTERLACED,
    EXT_VIDEO_FIELD_FRAME,

    EXT_VIDEO_FIELD_MAX
} ext_video_field;

typedef enum  {
    EXT_VIDEO_FORMAT_LINEAR = 0,
    EXT_VIDEO_FORMAT_TILE_64X16,
    EXT_VIDEO_FORMAT_TILE_16X8,
    EXT_VIDEO_FORMAT_TILE_32X4,

    EXT_VIDEO_FORMAT_MAX
} ext_video_format;

typedef enum  {
    EXT_COMPRESS_MODE_NONE = 0,      /* no compress */
    EXT_COMPRESS_MODE_SEG,           /* compress unit is 256x1 bytes as a segment. */
    EXT_COMPRESS_MODE_SEG_COMPACT,   /* compact compress unit is 256x1 bytes as a segment. */
    EXT_COMPRESS_MODE_TILE,          /* compress unit is a tile. */
    EXT_COMPRESS_MODE_LINE,          /* compress unit is the whole line. */
    EXT_COMPRESS_MODE_FRAME,         /* compress unit is the whole frame. */

    EXT_COMPRESS_MODE_MAX
} ext_compress_mode;

typedef enum  {
    EXT_CS_BT601 = 0,
    EXT_CS_BT709,
    EXT_CS_BT2020,

    EXT_CS_MAX
} ext_color_space;

typedef enum {
    EXT_PIXEL_FORMAT_RGB_444 = 0,
    EXT_PIXEL_FORMAT_RGB_555,
    EXT_PIXEL_FORMAT_RGB_565,
    EXT_PIXEL_FORMAT_RGB_888,
    EXT_PIXEL_FORMAT_RGB_10BPP,
    EXT_PIXEL_FORMAT_RGB_12BPP,
    EXT_PIXEL_FORMAT_RGB_14BPP,
    EXT_PIXEL_FORMAT_RGB_16BPP,

    EXT_PIXEL_FORMAT_BGR_444,
    EXT_PIXEL_FORMAT_BGR_555,
    EXT_PIXEL_FORMAT_BGR_565,
    EXT_PIXEL_FORMAT_BGR_888,

    EXT_PIXEL_FORMAT_ARGB_1555,
    EXT_PIXEL_FORMAT_ARGB_4444,
    EXT_PIXEL_FORMAT_ARGB_8565,
    EXT_PIXEL_FORMAT_ARGB_8888,
    EXT_PIXEL_FORMAT_ARGB_2BPP,
    EXT_PIXEL_FORMAT_ARGB_CLUT2,
    EXT_PIXEL_FORMAT_ARGB_CLUT4,
    EXT_PIXEL_FORMAT_ABGR_1555,
    EXT_PIXEL_FORMAT_ABGR_4444,
    EXT_PIXEL_FORMAT_ABGR_8565,
    EXT_PIXEL_FORMAT_ABGR_8888,

    EXT_PIXEL_FORMAT_RGB_BAYER_8BPP,
    EXT_PIXEL_FORMAT_RGB_BAYER_10BPP,
    EXT_PIXEL_FORMAT_RGB_BAYER_12BPP,
    EXT_PIXEL_FORMAT_RGB_BAYER_14BPP,
    EXT_PIXEL_FORMAT_RGB_BAYER_16BPP,

    EXT_PIXEL_FORMAT_YVU_PLANAR_422,
    EXT_PIXEL_FORMAT_YVU_PLANAR_420,
    EXT_PIXEL_FORMAT_YVU_PLANAR_444,

    EXT_PIXEL_FORMAT_YVU_SEMIPLANAR_422,
    EXT_PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    EXT_PIXEL_FORMAT_YVU_SEMIPLANAR_444,

    EXT_PIXEL_FORMAT_YUV_SEMIPLANAR_422,
    EXT_PIXEL_FORMAT_YUV_SEMIPLANAR_420,
    EXT_PIXEL_FORMAT_YUV_SEMIPLANAR_444,

    EXT_PIXEL_FORMAT_YUYV_PACKAGE_422,
    EXT_PIXEL_FORMAT_YVYU_PACKAGE_422,
    EXT_PIXEL_FORMAT_UYVY_PACKAGE_422,
    EXT_PIXEL_FORMAT_VYUY_PACKAGE_422,
    EXT_PIXEL_FORMAT_YYUV_PACKAGE_422,
    EXT_PIXEL_FORMAT_YYVU_PACKAGE_422,
    EXT_PIXEL_FORMAT_UVYY_PACKAGE_422,
    EXT_PIXEL_FORMAT_VUYY_PACKAGE_422,
    EXT_PIXEL_FORMAT_VY1UY0_PACKAGE_422,

    EXT_PIXEL_FORMAT_YUV_400,
    EXT_PIXEL_FORMAT_UV_420,

    EXT_PIXEL_FORMAT_MAX
} ext_pixel_format;

typedef enum  {
    EXT_DYNAMIC_RANGE_SDR8 = 0,
    EXT_DYNAMIC_RANGE_SDR10,
    EXT_DYNAMIC_RANGE_HDR10,
    EXT_DYNAMIC_RANGE_HLG,
    EXT_DYNAMIC_RANGE_SLF,

    EXT_DYNAMIC_RANGE_MAX
} ext_dynamic_range;

typedef enum  {
    EXT_BIT_DEPTH_8  = 8,
    EXT_BIT_DEPTH_10 = 10,
    EXT_BIT_DEPTH_12 = 12,
    EXT_BIT_DEPTH_14 = 14,
    EXT_BIT_DEPTH_16 = 16,
    EXT_BIT_DEPTH_18 = 18,

    EXT_BIT_DEPTH_MAX,
} ext_bit_depth;

typedef enum  {
    EXT_FRAME_FLAG_SNAP_FLASH   = 0x1 << 0,
    EXT_FRAME_FLAG_SNAP_CUR     = 0x1 << 1,
    EXT_FRAME_FLAG_SNAP_REF     = 0x1 << 2,
    EXT_FRAME_FLAG_SNAP_END     = 0x1 << 3,
    EXT_FRAME_FLAG_MIRROR       = 0x1 << 4,
    EXT_FRAME_FLAG_FLIP         = 0x1 << 5,
    EXT_FRAME_FLAG_ROTATION_90  = 0x1 << 6,
    EXT_FRAME_FLAG_ROTATION_270 = 0x1 << 7,

    EXT_FRAME_FLAG_MAX
} ext_frame_flag;

typedef enum  {
    EXT_ROTATION_0   = 0,
    EXT_ROTATION_90  = 1,
    EXT_ROTATION_180 = 2,
    EXT_ROTATION_270 = 3,

    EXT_ROTATION_MAX
} ext_rotation;

typedef struct {
    td_u64 exif_phy_addr;
    td_u64 exif_vir_addr;
    td_u64 isp_info_phy_addr;
    td_u64 isp_info_vir_addr;
    td_u64 low_delay_phy_addr;
    td_u64 low_delay_vir_addr;
    td_u64 motion_data_phy_addr;
    td_u64 motion_data_vir_addr;
    td_u64 frame_dng_phy_addr;
    td_u64 frame_dng_vir_addr;
    td_u64 sharpen_phy_addr;
    td_u64 sharpen_vir_addr;
    td_u64 jpeg_dcf_phy_addr;
    td_u64 jpeg_dcf_vir_addr;
    td_u64 fsd_phy_addr;
    td_u64 fsd_vir_addr;
    td_u64 reg_phy_addr;
    td_u64 reg_vir_addr;
    td_u32 reg_size;
    td_u32 pool_id;
} ext_video_supplement;

typedef struct {
    td_u32 width;
    td_u32 height;
    ext_video_field field;
    ext_pixel_format pixel_format;
    ext_video_format video_format;
    ext_compress_mode compress_mode;
    ext_dynamic_range dynamic_range;
    ext_color_space color_space;
    ext_bit_depth bit_depth;

    td_u32 header_stride[EXT_MAX_COLOR_COMPONENT];
    td_u64 header_phy_addr[EXT_MAX_COLOR_COMPONENT];
    td_u64 header_vir_addr[EXT_MAX_COLOR_COMPONENT];

    td_u32 stride[EXT_MAX_COLOR_COMPONENT];
    td_u64 phy_addr[EXT_MAX_COLOR_COMPONENT];
    td_u64 vir_addr[EXT_MAX_COLOR_COMPONENT];

    /* offset of show area */
    td_u16 offset_top;
    td_u16 offset_bottom;
    td_u16 offset_left;
    td_u16 offset_right;

    td_u32 time_ref;
    td_u64 pts;
    td_u32 frame_flag;
    ext_video_supplement supplement;
} ext_video_frame;

typedef struct {
     /* GPS latitude_ref indicates whether the latitude is north or south latitude, 'N'/'S', default 'N' */
    td_char gps_latitude_ref;

    /* GPS latitude is expressed as degrees, minutes and seconds, a typical format
      like "dd/1, mm/1, ss/1", default 0/0, 0/0, 0/0 */
    td_u32 gps_latitude[EXT_GPS_EXPRESS_NUM][EXT_GPS_GRP_NUM];

    /* GPS longitude_ref indicates whether the longitude is east or west longitude, 'E'/'W', default 'E' */
    td_char gps_longitude_ref;

    /* GPS longitude is expressed as degrees, minutes and seconds, a typical format like "dd/1, mm/1, ss/1",
     default 0/0, 0/0, 0/0 */
    td_u32 gps_longitude[EXT_GPS_EXPRESS_NUM][EXT_GPS_GRP_NUM];

    /* GPS altitude_ref indicates the reference altitude used, 0 - above sea level, 1 - below sea level default 0 */
    td_u8 gps_altitude_ref;

    /* GPS altitude_ref indicates the altitude based on the reference gps_altitude_ref,
     the reference unit is meters, default 0/0 */
    td_u32 gps_altitude[EXT_GPS_GRP_NUM];
} ext_gps_info;

typedef struct {
    td_u8 image_description[EXT_EXIF_DESCRIPTION_LENGTH];
    /* shows manufacturer of digital cameras */
    td_u8 make[EXT_EXIF_DESCRIPTION_LENGTH];

    /* shows model number of digital cameras */
    td_u8 model[EXT_EXIF_DESCRIPTION_LENGTH];

    /* shows firmware (internal software of digital cameras) version number */
    td_u8 software[EXT_EXIF_DESCRIPTION_LENGTH];

    /* light source, actually this means white balance setting. '0' means unknown, '1' daylight,
     '2' fluorescent, '3' tungsten, '10' flash, '17' standard light A, '18' standard light B,
     '19' standard light C, '20' D55, '21' D65, '22' D75, '255' other. */
    td_u8 light_source;

    /* focal length of lens used to take image. unit is millimeter */
    td_u32 focal_length;

    /* indicates the type of scene. value '0x01' means that the image was directly photographed. */
    td_u8 scene_type;

    /*  indicates the use of special processing on image data, such as rendering geared to output.
      0 = normal process, 1 = custom process. */
    td_u8 custom_rendered;

    /* indicates the equivalent focal length assuming a 35mm film camera, in mm */
    td_u8 focal_length_in35mm_film;

    /* indicates the type of scene that was shot. 0 = standard, 1 = landscape, 2 = portrait, 3 = night scene. */
    td_u8 scene_capture_type;

    /* indicates the degree of overall image gain adjustment. 0 = none, 1 = low gain up, 2 = high gain up,
      3 = low gain down, 4 = high gain down. */
    td_u8 gain_control;
    /* indicates the direction of contrast processing applied by the camera when the image was shot.
      0 = normal, 1 = soft, 2 = hard. */
    td_u8 contrast;

    /* indicates the direction of saturation processing applied by the camera when the image was shot.
      0 = normal, 1 = low saturation, 2 = high saturation. */
    td_u8 saturation;

    /* indicates the direction of sharpness processing applied by the camera when the image was shot.
     0 = normal, 1 = soft, 2 = hard. */
    td_u8 sharpness;

    /* * exposure metering method. '0' means unknown, '1' average, '2' center weighted average, '3' spot,
      '4' multi-spot, '5' multi-segment, '6' partial, '255' other. */
    td_u8 metering_mode;
} ext_exif_const_info;

typedef struct {
    /* CCD sensitivity equivalent to ag-hr film speedrate */
    td_u32 iso_speed_ratings;

    /* exposure time (reciprocal of shutter speed). */
    td_u32 exposure_time;

    /* exposure bias (compensation) value of taking picture */
    td_u32 exposure_bias_value;

    /* exposure program that the camera used when image was taken. '1' means manual control,
      '2' program normal, '3' aperture priority, '4' shutter priority, '5' program creative (slow program),
      '6' program action(high-speed program), '7' portrait mode, '8' landscape mode. */
    td_u8 exposure_program;

    /* the actual F-number (F-stop) of lens when the image was taken */
    td_u32 f_number;

    td_u32 max_aperture_value;

    /* indicates the exposure mode set when the image was shot.
      0 = auto exposure, 1 = manual exposure, 2 = auto bracket */
    td_u8 exposure_mode;

    /* indicates the white balance mode set when the image was shot.
       0 = auto white balance, 1 = manual white balance.  */
    td_u8 white_balance;
} ext_exif_update_info;

typedef struct {
    /* the date and time when the picture data was generated */
    td_u8 capture_time[EXT_EXIF_CAPTURE_TIME_LENGTH];

    /* whether the picture is captured when a flash lamp is on */
    td_bool flash;

    /* indicates the digital zoom ratio when the image was shot.
     if the numerator of the recorded value is 0, this indicates that digital zoom was not used */
    td_u32 digital_zoom_ratio;
    ext_exif_const_info  const_info;
    ext_exif_update_info update_info;
} ext_exif_info;

typedef struct {
    td_u32 iso;               /* ISP internal ISO : again*dgain*isp_gain */
    td_u32 exposure_time;     /* exposure time (reciprocal of shutter speed), unit is us */
    td_u32 isp_dgain;
    td_u32 again;             /* gain value, analog gain */
    td_u32 dgain;             /* gain value, digit gain */
    td_u32 ratio[EXT_ISP_EXPOSURE_RATIO_NUM]; /* default exposure ratio of two adjacent frames in WDR mode */
    td_u32 isp_nr_strength;   /* reserved byte, the default value is 0 */
    td_u32 f_number;          /* the actual F-number (F-stop) of lens when the image was taken */
    td_u32 sensor_id;         /* which sensor is used */
    td_u32 sensor_mode;       /* sequence mode currently used by the sensor */
    td_u32 hmax_times;        /* sensor hmax_times, unit is ns */
    td_u32 vmax;              /* sensor vmax, unit is line */
    td_u32 vc_num;            /* when dump wdr frame, which is long or short exposure frame. */
} ext_isp_frame_info;

/* low delay struct, 128 byte */
typedef struct {
    td_u16 y_height;
    td_u32 ysize;
    td_u8 reserve1[EXT_LOW_DELAY_RESERVE1];
    td_u16 c_height;
    td_u32 csize;
    td_u8 reserve2[EXT_LOW_DELAY_RESERVE2];

    td_u8 frame_ok;
    td_u8 tunl_frame;
    td_u8 frame_finish;
    td_u8 one_buffer;

    td_u8 reserve3[EXT_LOW_DELAY_RESERVE3];
} __attribute__((packed)) ext_low_delay_info;

typedef struct {
    td_bool enable;
    ext_rect rect;
} ext_crop_info;

typedef struct {
    td_u32 width;
    td_u32 height;
} ext_rect_size;

typedef struct {
    td_s32 x;
    td_s32 y;
} ext_point;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* SOC_MEDIA_COMMON_H */
