/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: ppu drv struct
 * Author: AuthorNameMagicTag
 * Create: 2021-9-1
 */


#ifndef __DRV_PPU_STRUCT_H__
#define __DRV_PPU_STRUCT_H__

#include "drv_isp_video_ext.h"
#include "drv_vb_struct.h"
#include "drv_gdc_struct.h"
#include "drv_isp_pq_struct.h"
#include "drv_fsd_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */
#ifdef CONFIG_CHIP_PLATFORM_WEAR
#define PPU_MAX_GRP_NUM             2
#define PPU_MAX_PHY_CHN_NUM         3
#define PPU_MAX_EXT_CHN_NUM         2
#else
#define PPU_MAX_GRP_NUM             16
#define PPU_MAX_PHY_CHN_NUM         3
#define PPU_MAX_EXT_CHN_NUM         8
#endif
#define PPU_MAX_CHN_NUM             (PPU_MAX_PHY_CHN_NUM + PPU_MAX_EXT_CHN_NUM)

#define PPU_IP_NUM                  1
#define PPU_SHARPEN_GAIN_NUM        32
#define PPU_AUTO_ISO_STRENGTH_NUM   16
#define PPU_YUV_SHPLUMA_NUM         32
#define PPU_3DNR_MAX_AUTO_PARAM_NUM 16
#define PPU_INVALID_FRMRATE         (-1)
#define PPU_CHN0                    0
#define PPU_CHN1                    1
#define PPU_CHN2                    2
#define PPU_CHN3                    3
#define PPU_INVALID_CHN             (-1)

#define PPU_COVER_MAX_NUM           8
#define PPU_COVEREX_MAX_NUM         8
#define PPU_OVERLAY_MAX_NUM         0
#define PPU_OVERLAYEX_MAX_NUM       8
#define PPU_MOSAIC_MAX_NUM          4
#define PPU_MAX_EXTBUF_NUM          16
#define PPU_MOSAIC_MAX_REGION_NUM   12
typedef enum {
    EXT_PPU_NR_TYPE_VIDEO         = 0,    // GDC+ 3DNR
    EXT_PPU_NR_TYPE_SNAP          = 1,    // GDC+ 3DNR
    EXT_PPU_NR_TYPE_SNAP_SPATIAL  = 2,    // GDC+ 3DNR + WR
    EXT_PPU_NR_TYPE_SNAP_ENHANCE  = 3,    // GDC+ 3DNR + WR + WNR
    EXT_PPU_NR_TYPE_VIDEO_SPATIAL = 4,
    EXT_PPU_NR_TYPE_VIDEO_ENHANCE = 5,
    EXT_PPU_NR_TYPE_MAX
} ext_ppu_nr_type;

typedef enum {
    EXT_PPU_NR_MOTION_MODE_NORMAL      = 0,
    EXT_PPU_NR_MOTION_MODE_COMPENSATE  = 1,
    EXT_PPU_NR_MOTION_MODE_MAX
} ext_ppu_nr_motion_mode;

typedef struct {
    ext_ppu_nr_type           nr_type;
    ext_isp_compress_mode     compress_mode;   /* RW; reference frame compress mode */
    ext_ppu_nr_motion_mode    nr_motion_mode;  /* RW; NR motion compensate mode. */
} ext_ppu_nr_attr;
typedef enum {
    EXT_PPU_CROP_RATIO_COOR = 0,               /* Ratio coordinate. */
    EXT_PPU_CROP_ABS_COOR                      /* Absolute coordinate. */
} ext_ppu_crop_coordinate;
typedef struct {
    td_bool                  enable;           /* RW; range: [0, 1]; crop enable. */
    ext_ppu_crop_coordinate  crop_coordinate;  /* RW; range: [0, 1]; coordinate mode of the crop start point. */
    ext_isp_rect             crop_rect;        /* CROP rectangular. */
} ext_ppu_crop_info;

typedef struct {
    td_u32                  max_w;           /* RW; range: [64, 2688] width of source image. */
    td_u32                  max_h;           /* RW; range: [64, 2688] height of source image. */
    ext_isp_pixel_format      pixel_format;    /* RW; pixel format of source image. */
    ext_isp_dynamic_range     dynamic_range;   /* RW; dynamic_range of source image. */
    ext_isp_frame_rate_ctrl   frame_rate;      /* grp frame rate contrl. */
    td_bool                 nr_en;           /* RW;range: [0, 1];  NR enable. */
    ext_ppu_nr_attr           nr_attr;         /* RW; NR attr. */
    ext_ppu_crop_info         crop_attr;
    td_bool                 sharpen_en;
    td_bool                 limit_range_en;
} ext_ppu_grp_attr;

typedef enum {
    EXT_PPU_CHN_MODE_USER  = 0,
    EXT_PPU_CHN_MODE_AUTO  = 1
} ext_ppu_chn_mode;

typedef struct {
    ext_ppu_chn_mode         chn_mode;         /* RW; ppu channel's work mode. */
    td_u32                   width;            /* RW; range: [64, 8192] width of target image. */
    td_u32                   height;           /* RW; range: [64, 8192] height of target image. */
    ext_isp_video_format     video_format;     /* RW; video format of target image. */
    ext_isp_pixel_format     pixel_format;     /* RW; pixel format of target image. */
    ext_isp_dynamic_range    dynamic_range;    /* RW; dynamic_range of target image. */
    ext_isp_compress_mode    compress_mode;    /* RW; compression mode of the output. */
    ext_isp_frame_rate_ctrl  frame_rate;       /* frame rate control info */
    td_bool                  mirror;           /* RW; mirror enable. */
    td_bool                  flip;             /* RW; flip enable. */
    td_bool                  use_extbuf;       /* RW; ext out buf enable. */
    td_u32                   depth;            /* RW; range: [0, 8]; user get list depth. */
    ext_isp_aspect_ratio     aspect_ratio;     /* aspect ratio info. */
    ext_ppu_crop_info        crop_attr;
    ext_isp_rotation         rotation;
} ext_ppu_chn_attr;

typedef struct {
    td_u32             count;
    ext_vb_memory_type memory;
    td_u32             buf_size;
    td_s64             mem_handle[PPU_MAX_EXTBUF_NUM];
} ext_ppu_chn_extbuf_attr;

typedef struct {
    td_bool        enable;                   /* RW; range: [0, 1]; whether LDC is enbale */
    ext_isp_ldc_attr attr;
} ext_ppu_ldc_attr;

/* not support */
typedef struct {
    td_bool           enable;                /* RW;whether LDC is enbale */
    ext_isp_ldc_v3_attr attr;
} ext_ppu_ldcv3_attr;

typedef struct {
    td_s32                  bind_chn;             /* RW; range: [0, 2]; channel bind to. */
    td_u32                  width;                /* RW; range: [64, 8192] width of target image. */
    td_u32                  height;               /* RW; range: [64, 8192] height of target image. */
    ext_isp_video_format    video_format;         /* RW; video format of target image. */
    ext_isp_pixel_format    pixel_format;         /* RW; pixel format of target image. */
    ext_isp_dynamic_range   dynamic_range;        /* RW; dynamic range. */
    ext_isp_compress_mode   compress_mode;        /* RW; compression mode of the output. */
    td_bool                 use_extbuf;           /* RW; ext out buf enable. */
    td_u32                  depth;                /* RW; range: [0, 8]; user get list depth. */
    ext_isp_frame_rate_ctrl frame_rate;           /* frame rate control info */
} ext_ppu_extchn_attr;

/* not support */
typedef struct {
    /* RW; range: [0, 4095]; undirectional sharpen strength for texture and detail enhancement */
    td_u16 texture_str[PPU_SHARPEN_GAIN_NUM];
    /* RW; range: [0, 4095]; directional sharpen strength for edge enhancement */
    td_u16 edge_str[PPU_SHARPEN_GAIN_NUM];
    /* RW; range: [0, 4095]; texture frequency adjustment. texture and detail will be finer when it increase */
    td_u16 texture_freq;
    /* RW; range: [0, 4095]; edge frequency adjustment. edge will be narrower and thiner when it increase */
    td_u16 edge_freq;
    /* RW; range: [0, 127]; overshoot_amt */
    td_u8  over_shoot;
    /* RW; range: [0, 127]; undershoot_amt */
    td_u8  under_shoot;
    /* RW; range: [0, 255]; overshoot and undershoot suppression strength,
        the amplitude and width of shoot will be decrease when shoot_sup_st increase */
    td_u8  shoot_sup_str;
    /* RW; range: [0, 255]; different sharpen strength for detail and edge.
        when it is bigger than 128, detail sharpen strength will be stronger than edge. */
    td_u8  detail_ctrl;
    /* RW; Range: [0, 63]; Edge smooth strength. */
    td_u8  edge_filt_str;
    /* RW; Range: [0, 255]; The threshold of DetailCtrl, it is used to distinguish detail and edge. */
    td_u8  detail_ctrl_thr;
} ext_ppu_grp_sharpen_manual_attr;

typedef enum {
    EXT_PPU_CHN_PROC_MODE_VIDEO = 0,
    EXT_PPU_CHN_PROC_MODE_SNAP = 1,
    EXT_PPU_CHN_PROC_MODE_MAX
} ext_ppu_chn_proc_mode;
typedef enum {
    EXT_PPU_WORK_MODE_ONLINE = 0,
    EXT_PPU_WORK_MODE_OFFLINE,
    EXT_PPU_WORK_MODE_SLAVE,
    EXT_PPU_WORK_MODE_MAX
} ext_ppu_work_mode;
typedef struct {
    td_s32 grp_id;
    ext_ppu_work_mode input_mode;
    ext_ppu_work_mode output_mode;
} ext_ppu_work_mode_attr;

typedef struct {
    td_u8  color_y;
    td_u8  color_u;
    td_u8  color_v;
    td_u8  thick;  // line thick
    ext_isp_rect rect;
} ext_ppu_mosaic_rect;

typedef struct {
    td_u8               region_num;
    ext_ppu_mosaic_rect region[PPU_MOSAIC_MAX_REGION_NUM];
} ext_ppu_region_info;

typedef struct {
    td_bool                  enable;
    ext_isp_op_type          op_type; // driver use nae result,to create corner or cover
    ext_ppu_region_info      cover_region_info;
} ext_ppu_cover_info;

typedef struct {
    td_bool                  enable;
    ext_isp_op_type          op_type; // driver use nae result,to create corner or cover
    td_u32                   corner_width;      // only for corner mode
    td_u32                   corner_height;    // only for corner mode
    ext_ppu_region_info      corner_region_info;
} ext_ppu_corner_info;

typedef struct {
    td_bool                   face_enable;
    td_bool                   landmark_enable;
    td_bool                   scene_enable;
} ext_ppu_fsd_attr;
typedef struct {
    ext_ppu_fsd_attr         fsd_attr;
    ext_fsd_face_output      face_result;
    ext_fsd_landmark_output  landmark_result;
    ext_fsd_scene_output     scene_result;
    td_u64                   fsd_receive_pts;
} ext_ppu_fsd_result;
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif
