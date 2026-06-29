/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021. All rights reserved.
 * Description: Huanglong vgs struct declaration.
 * Author: AuthorNameMagicTag
 * Create: 2021-08-21
 */

#ifndef __DRV_VGS_STRUCT_H__
#define __DRV_VGS_STRUCT_H__

#include "drv_isp_video_ext.h"
#include "drv_vb_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define CONFIG_SOCT_VGS_QUAD_POINT_NUM    4
#define CONFIG_SOCT_VGS_CLUT_NUM          2
typedef td_s32 vgs_handle;

typedef enum {
    EXT_VGS_COVER_RECT = 0,    /* Retangle cover */
    EXT_VGS_COVER_QUAD_RANGLE, /* Quadrangle cover */
    EXT_VGS_COVER_MAX
} ext_vgs_cover_type;

typedef enum {
    EXT_VGS_SCALE_COEF_NORMAL = 0,  /* normal scale coefficient */
    EXT_VGS_SCALE_COEF_TAP2 = 1,    /* scale coefficient of 2 tap for IVE */
    EXT_VGS_SCALE_COEF_TAP4 = 2,    /* scale coefficient of 4 tap for IVE */
    EXT_VGS_SCALE_COEF_TAP6 = 3,    /* scale coefficient of 6 tap for IVE */
    EXT_VGS_SCALE_COEF_TAP8 = 4,    /* scale coefficient of 8 tap for IVE */
    EXT_VGS_SCALE_COEF_PYRAMID = 5, /* scale coefficient for DIS pyramid */
    EXT_VGS_SCALE_COEF_MAX
} ext_vgs_scale_coef_mode;

typedef struct {
    td_bool solid;                /* solid or hollow cover */
    td_u32 thick;                 /* RW; range: [2,8]; thick of the hollow quadrangle */
    ext_isp_point point[CONFIG_SOCT_VGS_QUAD_POINT_NUM]; /* four points of the quadrangle */
} ext_vgs_quadrangle_cover;

typedef enum {
    EXT_VGS_COLOR_REVERT_NONE = 0,  /* Not revert */
    EXT_VGS_COLOR_REVERT_RGB,       /* Revert RGB */
    EXT_VGS_COLOR_REVERT_ALPHA,     /* Revert alpha */
    EXT_VGS_COLOR_REVERT_BOTH,      /* Revert RGB and alpha */
    EXT_VGS_COLOR_REVERT_MAX
} ext_vgs_color_revert_mode;

typedef struct {
    ext_isp_rect src_rect;                       /* OSD color revert area */
    ext_vgs_color_revert_mode color_revert_mode; /* OSD color revert mode */
} ext_vgs_osd_revert;

typedef struct {
    ext_isp_pixel_format      pixel_format;    /* OSD region pixel format */
    td_u32                    width;           /* OSD picture width */
    td_u32                    height;          /* OSD picture height */
    td_u32                    bg_color;        /* OSD region background color */
    td_bool                   use_bg_color;    /* if true, OSD will fill with bg_color */
    td_u64                    pic_data;        /* Address of OSD picture data */
} ext_vgs_osd_info;

typedef struct {
    ext_mpp_chn           chn;            /* OSD on which channel */
    td_bool               show;           /* OSD show or not */
    ext_isp_point         coor_point;     /* position of the OSD in the chn frame */
    /*
     * background and foreground transparence when pixel format is ARGB1555
     * the pixel format is ARGB1555, when the alpha bit is 1 this alpha is value!
     * range:[0,255]
     */
    td_u32                fg_alpha;
    /*
     * background and foreground transparence when pixel format is ARGB1555
     * the pixel format is ARGB1555, when the alpha bit is 0 this alpha is value!
     * range:[0,255]
     */
    td_u32                bg_alpha;
} ext_vgs_osd_chn_attr;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* end of __DRV_VGS_STRUCT_H__ */
