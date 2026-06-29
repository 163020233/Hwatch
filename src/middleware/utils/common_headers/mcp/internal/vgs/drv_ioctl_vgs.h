/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: dis ext define
 * Author: AuthorNameMagicTag
 * Create: 2021-8-31
 */

#ifndef __DRV_IOCTL_VGS_H__
#define __DRV_IOCTL_VGS_H__

#include "drv_vgs_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define VGS_TASK_PRIVATE_DATA_LEN       4
#define MAX_LINE_ARRAY_SIZE             100
#define MAX_COVER_ARRAY_SIZE            100
#define MAX_OSD_ARRAY_SIZE              100
#define MAX_LUMASTAT_ARRAY_SIZE         100
#define VGS_OSD_MAX_ATTACH_CHN          16

typedef struct {
    ext_isp_video_frame_info img_in;
    ext_isp_video_frame_info img_out;
    td_u64 private_data[VGS_TASK_PRIVATE_DATA_LEN];
    td_u32 reserved;          /* RW; debug information,state of current picture */
} ioc_vgs_task_attr;

typedef struct {
    vgs_handle handle;
    ioc_vgs_task_attr task_attr;
    ext_vgs_scale_coef_mode scl_coef_mode;
} ioc_vgs_scale_arg;

typedef struct {
    ext_isp_point start_point;
    ext_isp_point end_point;
    td_u32 thick;         /* RW; width of line */
    td_u32 color;         /* RW; range: [0,0xFFFFFF]; color of line */
} ioc_vgs_draw_line;

typedef struct {
    vgs_handle handle;
    ioc_vgs_task_attr task_attr;
    ioc_vgs_draw_line line;
} ioc_vgs_draw_line_arg;

typedef struct {
    vgs_handle handle;
    ioc_vgs_task_attr task_attr;
    td_u32 array_size;
    ioc_vgs_draw_line line[MAX_LINE_ARRAY_SIZE];
} ioc_vgs_draw_line_grp_arg;

typedef struct {
    ext_vgs_cover_type cover_type;
    union {
        ext_isp_rect dst_rect;                    /* the rectangle area */
        ext_vgs_quadrangle_cover quad_rangle;     /* the quadrangle area */
    };
    td_u32 color;                               /* RW; range: [0,0xFFFFFF]; color of cover */
} ioc_vgs_add_cover;

typedef struct {
    vgs_handle handle;
    ioc_vgs_task_attr task_attr;
    ioc_vgs_add_cover cover;
} ioc_vgs_add_cover_arg;

typedef struct {
    vgs_handle handle;
    ioc_vgs_task_attr task_attr;
    td_u32 array_size;
    ioc_vgs_add_cover cover[MAX_COVER_ARRAY_SIZE];
} ioc_vgs_add_cover_grp_arg;

typedef struct {
    ext_isp_rect rect;                 /* osd area */
    td_u32 bg_color;                 /* RW; background color of osd, depends on pixel format of osd,
                                          ARGB8888:[0,0xFFFFFFFF], ARGB4444:[0,0xFFFF], ARGB1555:[0,0x1FFF] */
    ext_isp_pixel_format pixel_fmt;    /* pixel format of osd */
    td_u64 phy_addr;                 /* RW; physical address of osd */
    td_u32 stride;                   /* RW; range: [32,8192]; stride of osd */
    td_u32 bg_alpha;                 /* RW; range: [0,255]; background alpha of osd */
    td_u32 fg_alpha;                 /* RW; range: [0,255]; foreground alpha of osd */
    td_bool revert_en;               /* RW; enable osd color revert */
    ext_vgs_osd_revert osd_revert;     /* osd color revert information */
    td_u16 color_lut[CONFIG_SOCT_VGS_CLUT_NUM];
} ioc_vgs_add_osd;

typedef struct {
    vgs_handle handle;
    ioc_vgs_task_attr task_attr;
    ioc_vgs_add_osd osd;
} ioc_vgs_add_osd_arg;

typedef struct {
    vgs_handle handle;
    ioc_vgs_task_attr task_attr;
    td_u32 array_size;
    ioc_vgs_add_osd osd[MAX_OSD_ARRAY_SIZE];
} ioc_vgs_add_osd_grp_arg;

typedef struct {
    vgs_handle handle;
    ioc_vgs_task_attr task_attr;
    ext_isp_rotation angle;
} ioc_vgs_rotation_arg;

typedef struct {
    vgs_handle handle;
    td_u32 array_size;
    ioc_vgs_task_attr task_attr;
    ext_isp_rect luma_rect[MAX_LUMASTAT_ARRAY_SIZE];
    td_u64 luma_data_addr;
} ioc_vgs_luma_arg;

typedef struct {
    td_u32            handle;
    ext_vgs_osd_info  osd_info;
} ioc_vgs_osd_info_arg;

typedef struct {
    td_u32           handle;
    ext_mpp_chn      chn;
} ioc_vgs_osd_detach_arg;

typedef struct {
    td_u32                 handle;
    td_u32                 attr_num;
    ext_vgs_osd_chn_attr   chn_attr[VGS_OSD_MAX_ATTACH_CHN];
} ioc_vgs_osd_attr_arg;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* end of __DRV_IOCTL_VGS_H__ */

