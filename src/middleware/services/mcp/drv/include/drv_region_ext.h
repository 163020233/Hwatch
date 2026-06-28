/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: region ext header define.
 * Author: AuthorNameMagicTag
 * Create: 2021-08-30
 */

#ifndef __DRV_REGION_EXT_H__
#define __DRV_REGION_EXT_H__

#include "drv_mcp_dependency.h"
#include "drv_region_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define SOC_ID_RGN              SOC_ID_MAX /* just for compile */
#define PIXEL_FORMAT_NUM_MAX    4          /* the max pixel format of region support */
#define RGN_HANDLE_IN_BATCH_MAX 24

typedef td_void (*rgn_detch_chn)(td_s32 dev_id, td_s32 chn_id, td_bool detch_chn);
typedef td_bool (*rgn_check_chn_capacity)(ext_rgn_type type, const ext_mpp_chn *chn);
typedef td_bool (*rgn_check_chn_attr)(ext_rgn_type type, const ext_mpp_chn *chn, const ext_rgn_chn_attr *rgn_chn);

typedef struct {
    td_bool                    is_show;
    ext_rgn_area_type            cover_type;      /* COVER type */
    ext_isp_point                point;           /* start position */
    ext_isp_size                 size;            /* region size(width and height) */
    ext_rgn_quadrangle           quad_rangle;     /* config of  quadrangle */
    td_u32                     layer;           /* layer of region */
    td_u32                     bg_color;        /* background color of region */
    td_u32                     global_alpha;    /* overall perspective ALPHA */
    td_u32                     fg_alpha;        /* foreground ALPHA */
    td_u32                     bg_alpha;        /* background ALPHA */
    td_u64                     phy_addr;        /* physical address of region memory */
    td_u64                     virt_addr;       /* virtual address of region memory */
    td_u32                     stride;          /* stride address of region memory data */
    td_u32                     canvas_num;      /* buffer number of region used */
    ext_isp_pixel_format         pixel_format;    /* pixel formatof region */
    ext_isp_video_field          field;           /* field of region attach */
    ext_rgn_overlay_qp_info      qp_info;         /* QP information */
    ext_rgn_overlay_invert_color inv_col_info;    /* inverse color information */
    ext_rgn_attach_dest          venc_chn;        /* OSD attach JPEG information */
    ext_rgn_mosaic_blk_size      mosaic_blk_size; /* MOSAIC block size */
    ext_rgn_coordinate           coordinate;      /* COVER coordinate */
    td_u16                     color_lut[RGN_COLOR_LUT_NUM];
} ext_rgn_comm;

typedef struct {
    td_u32 buff_index;
    td_u32 handle;
} ext_rgn_buf_info;

typedef struct {
    ext_rgn_buf_info buf_info;
    ext_rgn_comm     *comm;                    /* address of common information of  point array */
} ext_rgn_get_info;

typedef struct {
    td_u32         num;                      /* number of region */
    td_bool        modify;                   /* modify of not */
    ext_rgn_get_info *get_info;                /* address of common information of  point array */
} ext_rgn_info;

typedef struct {
    td_u32         num;                      /* number of region */
    ext_rgn_buf_info *buf_info;
} ext_rgn_put_info;

typedef struct {
    mcp_soc_mod_id    mod_id;
    td_u32      max_dev_cnt;                 /* if no dev id, should set it 1 */
    td_u32      max_chn_cnt;
    rgn_detch_chn pfn_rgn_detch_chn;
} ext_rgn_register_info;

typedef struct {
    td_bool is_comm;
    td_bool spt_reset;
    td_u32  layer_min;
    td_u32  layer_max;
} ext_rgn_capacity_layer;

typedef struct {
    td_bool     is_comm;
    td_bool     spt_reset;
    ext_isp_point point_min;
    ext_isp_point point_max;
    td_u32      start_x_align;  /* X (start position) of pixel align number */
    td_u32      start_y_align;  /* Y (start position) of pixel align number */
} ext_rgn_capacity_point;

typedef struct {
    td_bool    is_comm;
    td_bool    spt_reset;
    ext_isp_size size_min;
    ext_isp_size size_max;
    td_u32     width_align;     /* region width of pixel align number */
    td_u32     height_align;    /* region height of pixel align number */
    td_u32     max_area;        /* maximum area of region */
} ext_rgn_capacity_size;

typedef struct {
    td_bool     is_comm;
    td_bool     spt_reset;
    td_u32      min_thick;
    td_u32      max_thick;
    ext_isp_point point_min;
    ext_isp_point point_max;
    td_u32      start_x_align;  /* X (start position) of pixel align number */
    td_u32      start_y_align;  /* Y (start position) of pixel align number */
} ext_rgn_capacity_quadrangle;

typedef struct {
    td_bool is_comm;
    td_bool spt_reset;
    td_u32  pixl_fmt_num;    /* pixel format number of region */
    /*
     * all pixel format type of region supported,
     * check channel all pixel pormat is same or not.
     */
    ext_isp_pixel_format pixel_format[PIXEL_FORMAT_NUM_MAX];
} ext_rgn_capacity_pixlfmt;

typedef struct {
    td_bool is_comm;
    td_bool spt_reset;
    td_u32  alpha_max;
    td_u32  alpha_min;
} ext_rgn_capacity_alpha;

typedef struct {
    td_bool is_comm;
    td_bool spt_reset;
} ext_rgn_capacity_bgclr;

typedef enum {
    EXT_RGN_SORT_BY_LAYER = 0,
    EXT_RGN_SORT_BY_POSITION,
    EXT_RGN_SORT_MAX
} ext_rgn_sort_key;

typedef struct {
    td_bool        is_sort;
    ext_rgn_sort_key key;
    /* the sort way: 1/true: small to larger;2/true: larger to small; */
    td_bool        small_to_big;
} ext_rgn_capacity_sort;

typedef struct {
    td_bool is_comm;
    td_bool spt_reset;
    td_bool spt_qp_disable;
    td_s32  qp_abs_min;
    td_s32  qp_abs_max;
    td_s32  qp_rel_min;
    td_s32  qp_rel_max;
} ext_rgn_capacity_qp;

typedef enum {
    EXT_RGN_CAPACITY_QUADRANGLE_UNSUPORT = 0x0,
    EXT_RGN_CAPACITY_QUADRANGLE_TYPE_SOLID = 0x1,
    EXT_RGN_CAPACITY_QUADRANGLE_TYPE_UNSOLID = 0x2,
    EXT_RGN_CAPACITY_QUADRANGLE_TYPE_MAX = 0x4,
} ext_rgn_capacity_quadrangle_type;

typedef struct {
    td_bool spt_rect;
    ext_rgn_capacity_quadrangle_type quad_rangle_type;
} ext_rgn_capacity_cover_attr;

typedef struct {
    td_bool is_comm;
    td_bool spt_reset;
    td_u32  msc_blk_sz_max;
    td_u32  msc_blk_sz_min;
} ext_rgn_capacity_mscblksz;

typedef struct {
    ext_rgn_capacity_layer      layer;             /* layer of region */
    ext_rgn_capacity_point      point;             /* start position */
    ext_rgn_capacity_size       size;              /* region size(width and height) */
    ext_rgn_capacity_quadrangle quad_rangle;       /* attribute of quadrangle */
    td_bool                   spt_pixl_fmt;
    ext_rgn_capacity_pixlfmt    pixl_fmt;
    td_bool                   spt_fg_alpha;      /* foreground alpha */
    ext_rgn_capacity_alpha      fg_alpha;
    td_bool                   spt_bg_alpha;      /* background alpha */
    ext_rgn_capacity_alpha      bg_alpha;
    td_bool                   spt_global_alpha;  /* overall perspective alpha */
    ext_rgn_capacity_alpha      global_alpha;
    td_bool                   spt_bg_clr;        /* background color of region */
    ext_rgn_capacity_bgclr      bg_clr;
    td_bool                   spt_sort;
    ext_rgn_capacity_sort       chn_sort;
    td_bool                   spt_qp;
    ext_rgn_capacity_qp         qp;
    ext_rgn_capacity_cover_attr cover_attr;        /* cover attribute */
    td_bool                   spt_msc;
    ext_rgn_capacity_mscblksz   msc_blk_sz;
    td_bool                   spt_bitmap;        /* support bitmap or not */
    td_bool                   spt_overlap;       /* support overlap or not */
    td_u32                    stride;            /* stride align */
    td_u32                    rgn_num_in_chn;    /* the maximumegion of channel */
    rgn_check_chn_capacity pfn_check_chn_capacity; /* check whether channel support a region type */
    rgn_check_chn_attr     pfn_check_attr;         /* check whether attribute is legal */
} ext_rgn_capacity;

typedef struct {
    td_s32 (*pfn_rgn_register_mod)(ext_rgn_type type, const ext_rgn_capacity *capacity,
                                   const ext_rgn_register_info *register_info);
    td_s32 (*pfn_rgn_unregister_mod)(ext_rgn_type type, mcp_soc_mod_id mod_id);
    td_s32 (*pfn_rgn_get_region)(ext_rgn_type type, const ext_mpp_chn *chn, ext_rgn_info *info);
    td_s32 (*pfn_rgn_put_region)(ext_rgn_type type, const ext_mpp_chn *chn, ext_rgn_put_info *info);
    td_s32 (*pfn_rgn_set_modify_false)(ext_rgn_type type, const ext_mpp_chn *chn);
} rgn_export_func;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __DRV_REGION_EXT_H__ */
