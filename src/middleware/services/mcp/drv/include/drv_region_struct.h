/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: region struct define.
 * Author: AuthorNameMagicTag
 * Create: 2021-08-30
 */

#ifndef __DRV_REGION_STRUCT_H__
#define __DRV_REGION_STRUCT_H__

#include "drv_isp_video_ext.h"
#include "drv_vb_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define RGN_COLOR_LUT_NUM              2
#define RGN_MAX_BMP_UPDATE_NUM         16
#define RGN_BATCHHANDLE_MAX            24

#define RGN_ALIGN                      2
#define RGN_MIN_WIDTH                  2
#define RGN_MIN_HEIGHT                 2

#define RGN_COVER_MIN_X                -8192
#define RGN_COVER_MIN_Y                -8192
#define RGN_COVER_MAX_X                8190
#define RGN_COVER_MAX_Y                8190
#define RGN_COVER_MAX_WIDTH            8192
#define RGN_COVER_MAX_HEIGHT           8192
#define RGN_COVER_MIN_THICK            2
#define RGN_COVER_MAX_THICK            8

#define RGN_COVEREX_MIN_X              -8192
#define RGN_COVEREX_MIN_Y              -8192
#define RGN_COVEREX_MAX_X              8190
#define RGN_COVEREX_MAX_Y              8190
#define RGN_COVEREX_MAX_WIDTH          8192
#define RGN_COVEREX_MAX_HEIGHT         8192
#define RGN_COVEREX_MIN_THICK          2
#define RGN_COVEREX_MAX_THICK          8

#define RGN_OVERLAY_MIN_X              0
#define RGN_OVERLAY_MIN_Y              0
#define RGN_OVERLAY_MAX_X              8190
#define RGN_OVERLAY_MAX_Y              8190
#define RGN_OVERLAY_MAX_WIDTH          8192
#define RGN_OVERLAY_MAX_HEIGHT         8192
#define RGN_OVERLAY_MIN_ALPHA          0
#define RGN_OVERLAY_MAX_ALPHA          128

#define RGN_OVERLAYEX_MIN_X            0
#define RGN_OVERLAYEX_MIN_Y            0
#define RGN_OVERLAYEX_MAX_X            8190
#define RGN_OVERLAYEX_MAX_Y            8190
#define RGN_OVERLAYEX_MAX_WIDTH        8192
#define RGN_OVERLAYEX_MAX_HEIGHT       8192
#define RGN_OVERLAYEX_MIN_ALPHA        0
#define RGN_OVERLAYEX_MAX_ALPHA        255

#define RGN_MOSAIC_X_ALIGN             4
#define RGN_MOSAIC_Y_ALIGN             2
#define RGN_MOSAIC_WIDTH_ALIGN         4
#define RGN_MOSAIC_HEIGHT_ALIGN        4

#define RGN_MOSAIC_MIN_X               0
#define RGN_MOSAIC_MIN_Y               0
#define RGN_MOSAIC_MAX_X               8190
#define RGN_MOSAIC_MAX_Y               8190
#define RGN_MOSAIC_MIN_WIDTH           32
#define RGN_MOSAIC_MIN_HEIGHT          32
#define RGN_MOSAIC_MAX_WIDTH           8192
#define RGN_MOSAIC_MAX_HEIGHT          8192

typedef enum {
    EXT_RGN_OVERLAY = 0,
    EXT_RGN_COVER,
    EXT_RGN_COVEREX,
    EXT_RGN_OVERLAYEX,
    EXT_RGN_MOSAIC,
    EXT_RGN_MAX
} ext_rgn_type;

typedef enum {
    /* the lum of the video is less than the lum threshold which is set by u32LumThresh */
    EXT_RGN_LESSTHAN_LUM_THRESH = 0,

    /* the lum of the video is more than the lum threshold which is set by u32LumThresh */
    EXT_RGN_MORETHAN_LUM_THRESH,
    EXT_RGN_INVERT_COLOR_MAX
} ext_rgn_invert_color_mode;

typedef struct {
    td_bool abs_qp;
    td_s32  qp;
    td_bool qp_disable;
} ext_rgn_overlay_qp_info;

typedef struct {
    ext_isp_size inv_col_area;      /* it must be multipe of 16 but not more than 64. */
    td_u32     lum_thresh;        /* the threshold to decide whether invert the OSD's color or not. */
    ext_rgn_invert_color_mode chg_mod;
    td_bool    inv_col_en;        /* the switch of inverting color. */
} ext_rgn_overlay_invert_color;

typedef enum {
    EXT_RGN_ATTACH_JPEG_MAIN = 0,
    EXT_RGN_ATTACH_JPEG_MPF0,
    EXT_RGN_ATTACH_JPEG_MPF1,
    EXT_RGN_ATTACH_JPEG_MAX
} ext_rgn_attach_dest;

typedef struct {
    /* bitmap pixel format,now only support ARGB1555 or ARGB4444 */
    ext_isp_pixel_format pixel_format;

    /* background color, pixel format depends on "pixel_format" */
    td_u32 bg_color;

    /* region size,W:[2,RGN_OVERLAY_MAX_WIDTH],align:2,H:[2,RGN_OVERLAY_MAX_HEIGHT],align:2 */
    ext_isp_size size;
    td_u32 canvas_num;
} ext_rgn_overlay_attr;

typedef struct {
    /* X:[0,OVERLAY_MAX_X_VENC],align:2,Y:[0,OVERLAY_MAX_Y_VENC],align:2 */
    ext_isp_point point;

    /*
     * background an foreground transparence when pixel format is ARGB1555
     * the pixel format is ARGB1555,when the alpha bit is 1 this alpha is value!
     * range:[0,128]
     */
    td_u32 fg_alpha;

    /*
     * background an foreground transparence when pixel format is ARGB1555
     * the pixel format is ARGB1555,when the alpha bit is 0 this alpha is value!
     * range:[0,128]
     */
    td_u32                     bg_alpha;
    td_u32                     layer; /* OVERLAY region layer range:[0,7] */
    ext_rgn_overlay_qp_info      qp_info;
    ext_rgn_overlay_invert_color invert_color;
    ext_rgn_attach_dest          venc_chn;
    td_u16                     color_lut[RGN_COLOR_LUT_NUM];
} ext_rgn_overlay_chn_attr;

typedef enum {
    EXT_RGN_AREA_RECT = 0,
    EXT_RGN_AREA_QUAD_RANGLE,
    EXT_RGN_AREA_MAX
} ext_rgn_area_type;

typedef enum {
    EXT_RGN_ABS_COOR = 0,     /* Absolute coordinate */
    EXT_RGN_RATIO_COOR        /* Ratio coordinate */
} ext_rgn_coordinate;

typedef struct {
    td_bool     solid;      /* whether solid or dashed quadrangle */
    td_u32      thick;      /* line width of quadrangle, valid when dashed quadrangle */
    ext_isp_point point[4];   /* quadrangle has 4 points */
} ext_rgn_quadrangle;

typedef struct {
    ext_rgn_area_type      cover_type;  /* rect or arbitary quadrilateral cover */
    union {
        ext_isp_rect       rect;        /* config of rect */
        ext_rgn_quadrangle quad_rangle; /* config of arbitary quadrilateral cover */
    };
    td_u32               bg_color;
    td_u32               layer;       /* cover region layer */
    ext_rgn_coordinate     coordinate;  /* ratio coordiante or abs coordinate */
} ext_rgn_cover_chn_attr;

typedef struct {
    ext_rgn_area_type      cover_type;  /* rect or arbitary quadrilateral cover */
    union {
        ext_isp_rect       rect;        /* config of rect */
        ext_rgn_quadrangle quad_rangle; /* config of arbitary quadrilateral cover */
    };
    td_u32               bg_color;
    td_u32               layer;       /* coverex region layer range */
} ext_rgn_coverex_chn_attr;

typedef enum {
    EXT_RGN_MOSAIC_BLK_SIZE_8 = 0, /* block size 8*8 of MOSAIC */
    EXT_RGN_MOSAIC_BLK_SIZE_16,    /* block size 16*16 of MOSAIC */
    EXT_RGN_MOSAIC_BLK_SIZE_32,    /* block size 32*32 of MOSAIC */
    EXT_RGN_MOSAIC_BLK_SIZE_64,    /* block size 64*64 of MOSAIC */
    EXT_RGN_MOSAIC_BLK_SIZE_MAX
} ext_rgn_mosaic_blk_size;

typedef struct {
    ext_isp_rect            rect;            /* location of MOSAIC */
    ext_rgn_mosaic_blk_size mosaic_blk_size; /* block size of MOSAIC */
    td_u32                layer;           /* MOSAIC region layer range:[0,3] */
} ext_rgn_mosaic_chn_attr;

typedef struct {
    ext_isp_pixel_format pixel_format;

    /* background color, pixel format depends on "pixel_format" */
    td_u32 bg_color;

    /* region size,W:[2,RGN_OVERLAY_MAX_WIDTH],align:2,H:[2,RGN_OVERLAY_MAX_HEIGHT],align:2 */
    ext_isp_size size;
    td_u32     canvas_num;
} ext_rgn_overlayex_attr;

typedef struct {
    /* X:[0,RGN_OVERLAY_MAX_X],align:2,Y:[0,RGN_OVERLAY_MAX_Y],align:2 */
    ext_isp_point point;

    /*
     * background an foreground transparence when pixel format is ARGB1555
     * the pixel format is ARGB1555,when the alpha bit is 1 this alpha is value!
     * range:[0,255]
     */
    td_u32 fg_alpha;

    /*
     * background an foreground transparence when pixel format is ARGB1555
     * the pixel format is ARGB1555,when the alpha bit is 0 this alpha is value!
     * range:[0,255]
     */
    td_u32 bg_alpha;
    td_u32 layer; /* OVERLAYEX region layer range:[0,15] */
    td_u16 color_lut[RGN_COLOR_LUT_NUM];
} ext_rgn_overlayex_chn_attr;

typedef union {
    ext_rgn_overlay_attr       overlay;        /* attribute of overlay region */
    ext_rgn_overlayex_attr     overlayex;      /* attribute of overlayex region */
} ext_rgn_type_attr;

typedef union {
    ext_rgn_overlay_chn_attr   overlay_chn;    /* attribute of overlay region */
    ext_rgn_cover_chn_attr     cover_chn;      /* attribute of cover region */
    ext_rgn_coverex_chn_attr   coverex_chn;    /* attribute of coverex region */
    ext_rgn_overlayex_chn_attr overlayex_chn;  /* attribute of overlayex region */
    ext_rgn_mosaic_chn_attr    mosaic_chn;     /* attribute of mosic region */
} ext_rgn_type_chn_attr;

typedef struct {
    ext_rgn_type      type;
    ext_rgn_type_attr attr;
} ext_rgn_attr;

typedef struct {
    td_bool             is_show;
    ext_rgn_type          type;
    ext_rgn_type_chn_attr attr;
} ext_rgn_chn_attr;

typedef struct {
    td_u64             phy_addr;
    td_u64             virt_addr;
    ext_isp_size         size;
    td_u32             stride;
    ext_isp_pixel_format pixel_fmt;
} ext_rgn_canvas_info;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __DRV_REGION_STRUCT_H__ */