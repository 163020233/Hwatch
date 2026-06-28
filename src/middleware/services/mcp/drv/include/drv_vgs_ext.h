/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021. All rights reserved.
 * Description: Huanglong vgs ext declaration.
 * Author: AuthorNameMagicTag
 * Create: 2021-08-21
 */

#ifndef __DRV_VGS_EXT_H__
#define __DRV_VGS_EXT_H__

#include "drv_mcp_dependency.h"
#include "drv_vgs_struct.h"
#include "mcp_arch_define.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define MAX_VGS_CHN           3
#define MAX_VGS_COVER         1
#define MAX_VGS_OSD           1
#define MAX_VGS_STITCH        4
#define MAX_VGS_OSD_PER_CHN   24    /* 单通道最大绑定的OSD个数 */
#define MAX_VGS_CORNER_RECT   8

/* grid size used in feature detection, the image is processed with 4x4 grid */
#define VGS_FD_GRID_SZ        4
#define VGS_FD_CELL_NUM       (VGS_FD_GRID_SZ * VGS_FD_GRID_SZ)
#define VGS_PRIVATE_DATA_LEN  6

typedef enum {
    /* BYPASS job,can only contain bypass task */
    EXT_VGS_JOB_TYPE_BYPASS = 0,
    /* normal job,can contain any task except bypass task and lumastat task */
    EXT_VGS_JOB_TYPE_NORMAL = 1,
    EXT_VGS_JOB_TYPE_MAX
} ext_vgs_job_type;

typedef enum {
    EXT_VGS_TASK_FNSH_STAT_OK       = 1,   /* task has been completed correctly */
    EXT_VGS_TASK_FNSH_STAT_FAIL     = 2,   /* task failed because of exception or not completed */
    EXT_VGS_TASK_FNSH_STAT_CANCELED = 3,   /* task has been cancelled */
    EXT_VGS_TASK_FNSH_STAT_MAX
} ext_vgs_task_fnsh_stat;

typedef enum {
    EXT_VGS_JOB_PRI_HIGH   = 0,    /* high priority */
    EXT_VGS_JOB_PRI_NORMAL = 1,    /* normal priority */
    EXT_VGS_JOB_PRI_LOW    = 2,    /* low priority */
    EXT_VGS_JOB_PRI_MAX
} ext_vgs_job_pri;

typedef struct {
    td_u32 jobs_canceled;        /* The count of cancelled job */
    td_u32 jobs_left;            /* The count of the rest job */
} ext_vgs_cancel_stat;

typedef enum {
    EXT_VGS_JOB_FNSH_STAT_OK       = 1,   /* job has been completed correctly */
    EXT_VGS_JOB_FNSH_STAT_FAIL     = 2,   /* job failed because of exception or not completed */
    EXT_VGS_JOB_FNSH_STAT_CANCELED = 3,   /* job has been cancelled */
    EXT_VGS_JOB_FNSH_STAT_MAX
} ext_vgs_job_fnsh_stat;

/*
 * The struct of vgs job.
 * After complete the job,VGS call the callback function to notify the caller with this struct as an parameter.
 */
typedef struct vgs_job_data {
    td_u64 private_data[VGS_PRIVATE_DATA_LEN];   /* private data of job */
    ext_vgs_job_fnsh_stat job_finish_stat;         /* output param:job finish status(ok or nok) */
    ext_vgs_job_type job_type;
    td_void (*job_call_back)(mcp_soc_mod_id call_mod_id, td_s32 call_dev_id, td_s32 call_chn_id,
                          struct vgs_job_data *job_data); /* callback */
} ext_vgs_job_data;

typedef struct {
    td_u64 phy_addr;       /* Header address of a bitmap or the Y component */
    ext_isp_pixel_format color_fmt;
    td_u32 height;         /* Bitmap height */
    td_u32 width;          /* Bitmap width */
    td_u32 stride;         /* Stride of a bitmap or the Y component */
    td_bool alpha_max255;  /* The maximum alpha value of a bitmap is 255 or 128. */
    td_bool alpha_ext1555; /* Whether to enable the alpha extension of an ARGB1555 bitmap. */
    td_u8 alpha0;          /* Values of alpha0 and alpha1, used as the ARGB1555 format */
    td_u8 alpha1;          /* Values of alpha0 and alpha1, used as the ARGB1555 format */
} gfx_surface;

typedef struct vgs_task_data {
    union {
        struct {
            ext_isp_video_frame_info img_in;
            union {
                ext_isp_video_frame_info img_out;
                ext_isp_video_frame_info multi_img_out[MAX_VGS_CHN];
            };
        };
        struct {
            gfx_surface back_ground;
            ext_isp_rect back_ground_rect;
            gfx_surface fore_ground;
            ext_isp_rect fore_ground_rect;
            gfx_surface dst;
            ext_isp_rect dst_rect;
        };
    };
    td_u64 private_data[VGS_PRIVATE_DATA_LEN];
    td_void (*call_back)(mcp_soc_mod_id call_mod_id, td_s32 call_dev_id, td_s32 call_chn_id,
                      const struct vgs_task_data *task);
    mcp_soc_mod_id call_mod_id;
    td_s32 call_dev_id;
    td_s32 call_chn_id;
    union {
        ext_vgs_task_fnsh_stat finish_stat;
        ext_vgs_task_fnsh_stat multi_finish_stat[MAX_VGS_CHN];
    };
    td_u32 reserved;
} ext_vgs_task_data;

typedef struct ext_vgs_task_data_stitch {
    ext_isp_video_frame_info img_in[MAX_VGS_STITCH];
    ext_isp_video_frame_info img_out;
    td_u64 private_data[VGS_PRIVATE_DATA_LEN];
    td_void (*call_back)(mcp_soc_mod_id call_mod_id, td_s32 call_dev_id, td_s32 call_chn_id,
                      const struct ext_vgs_task_data_stitch *task);
    mcp_soc_mod_id call_mod_id;
    td_s32 call_dev_id;
    td_s32 call_chn_id;
    ext_vgs_task_fnsh_stat finish_stat;
    td_u32 reserved;
} ext_vgs_task_data_stitch;

typedef struct {
    td_u64 phy_addr;
    ext_isp_pixel_format pixel_format;
    td_u32 stride;

    /* Alpha bit should be extended by setting alpha0 and Alpha1, when pixel_format is PIXEL_FORMAT_RGB_1555 */
    td_bool alpha_ext1555; /* whether alpha bit should be extended */
    td_u8 alpha0;          /* alpha0 will be valid where alpha bit is 0 */
    td_u8 alpha1;          /* alpha1 will be valid where alpha bit is 1 */
    td_u16 color_lut[CONFIG_SOCT_VGS_CLUT_NUM];
} ext_vgs_osd;

/* The struct of OSD operation */
typedef struct {
    td_bool     osd_en;
    td_u8       global_alpha;
    ext_vgs_osd   osd;
    ext_isp_rect  osd_rect;
    td_bool     revert_en;
    ext_vgs_osd_revert osd_revert;
    td_bool     ghdr_en;
} ext_vgs_osd_opt;

typedef struct {
    ext_isp_rect rect; /* the regions to get lumainfo */
    td_u64 *virt_addr_for_result;
    td_u64 phys_addr_for_result;
    td_u64 *virt_addr_for_user;
} ext_vgs_lumastat_opt;

typedef struct {
    td_bool cover_en;
    ext_vgs_cover_type cover_type;
    union {
        ext_isp_rect dst_rect;                    /* rectangle */
        ext_vgs_quadrangle_cover quad_rangle;     /* arbitrary quadrilateral */
    };
    td_u32 cover_color;
} ext_vgs_cover_opt;

typedef struct {
    ext_isp_rect dest_rect;
} ext_vgs_crop_opt;

typedef struct {
    td_u32 proc_bit_depth;  /* 4bits; u; 4.0 */
    td_bool do_detection;   /* 1bit; bool */

    td_u16 grid_rows;       /* 3bits; u; 3.0 */
    td_u16 grid_cols;       /* 3bits; u; 3.0 */
    td_u16 grid_nums;       /* 5bits; u,; 5.0 */

    td_u8 quality_level;    /* 3bits; u; 3.0 */
    td_u16 min_eigen_val;   /* 16bits; u; 16.0 */
    td_u8 harris_k;         /* 4btis; u; 4.0 */
    td_u16 min_distance;    /* 5bits; u; 5.0 */

    td_u16 cell_row[VGS_FD_GRID_SZ + 1];       /* 9bits; u; 9.0 corner postion of the cells */
    td_u16 cell_col[VGS_FD_GRID_SZ + 1];       /* 9bits; u; 9.0 */
    /* 6bits; u; 6.0 maximum number of keypoints allowed in each block */
    td_u16 pts_num_per_cell[VGS_FD_CELL_NUM];
    td_u16 max_pts_num_in_use;                 /* 9bits; u; 9.0 */
    td_bool set_pts_num_per_cell;              /* 1bit; bool */
} ext_vgs_fpd_hw_cfg;

typedef struct {
    ext_isp_rect video_rect;
    td_u32 bg_color;
} ext_vgs_aspectratio_opt;

typedef enum {
    EXT_VGS_CORNER_RECT_TYPE_CORNER = 0,
    EXT_VGS_CORNER_RECT_TYPE_FULL_LINE,
    EXT_VGS_CORNER_RECT_TYPE_BUTT
} ext_vgs_corner_rect_type;

typedef struct {
    ext_vgs_corner_rect_type corner_rect_type;
    td_u32 color;
} ext_vgs_corner_rect_attr;

typedef struct {
    td_s32 x;
    td_s32 y;
    td_u32 width;
    td_u32 height;
    td_u32 hor_len;
    td_u32 ver_len;
    td_u32 thick;
} ext_vgs_corner_rect;

typedef struct {
    td_u32 num;
    ext_vgs_corner_rect corner_rect[MAX_VGS_CORNER_RECT];
    ext_vgs_corner_rect_attr comm_attr;
} ext_vgs_online_corner_rect_opt;

typedef struct {
    td_bool enable;
    ext_vgs_corner_rect corner_rect;
    ext_vgs_corner_rect_attr comm_attr;
} ext_vgs_corner_rect_opt;

typedef struct {
    /* if enable crop */
    td_bool crop;
    ext_vgs_crop_opt crop_opt;
    /* if enable cover */
    td_bool cover;
    ext_vgs_cover_opt cover_opt[MAX_VGS_COVER];
    /* if enable osd */
    td_bool osd;
    ext_vgs_osd_opt osd_opt[MAX_VGS_OSD];
    /* if enable mirror */
    td_bool mirror;
    /* if enable flip */
    td_bool flip;
    /*
     * whether to force the horizontal direction filtering, it can be
     * set while input and output pic are same size at horizontal direction
     */
    td_bool force_h_filt;
    /*
     * whether to force the vertical direction filtering, it can be
     * set while input and output pic are same size at vertical direction
     */
    td_bool force_v_filt;
    /* whether decrease flicker */
    td_bool deflicker;
    ext_vgs_scale_coef_mode vgs_scl_coef_mode;
    /* the operation is belong to fisheye */
    td_bool gdc;
    ext_isp_rect gdc_rect;
    /* if enable fpd */
    td_bool fpd;
    td_u64 fpd_phy_addr;
    ext_vgs_fpd_hw_cfg fpd_opt;
    td_bool corner;
    ext_vgs_online_corner_rect_opt corner_opt;
    /* if enable LBA */
    td_bool aspect_ratio;
    ext_vgs_aspectratio_opt aspect_ratio_opt;
} ext_vgs_online_opt;

typedef struct {
    td_u32 stitch_num;
} ext_vgs_stitch_opt;

typedef struct {
    td_bool enable;         /* RW; low delay enable. */
    td_u32 line_cnt;       /* RW; range: [16, 8192]; low delay shoreline. */
    td_bool one_buf_en;     /* RW; one buffer for low delay enable. */
} ext_vgs_low_delay_info;

typedef struct {
    td_bool chn_enable[MAX_VGS_CHN];
    ext_isp_crop_info crop_info[MAX_VGS_CHN];
    ext_isp_size zme_out_size[MAX_VGS_CHN]; /* valid when enable chn crop */
    td_bool aspect_ratio[MAX_VGS_CHN];
    ext_vgs_aspectratio_opt aspect_ratio_opt[MAX_VGS_CHN];
    ext_vgs_low_delay_info low_delay_info[MAX_VGS_CHN];
    td_bool mirror[MAX_VGS_CHN];
    td_bool flip[MAX_VGS_CHN];
    ext_vgs_online_opt online_opt;  /* multi chn online opt */
    td_void *mcf_data;

    /* gen mcf stt */
    td_bool mcf_gen_stt_en;
    td_bool mcf_is_big_image;
    td_u64 mcf_stt_phys_addr;
} ext_vgs_multi_chn_opt;

typedef struct {
    td_void *reg_data;
} vgs_gme_opt;

typedef struct {
    ext_isp_rect          rect;           /* OSD坐标宽高信息 */
    td_u32                layer;          /* OSD层次 */
    td_u32                global_alpha;   /* OSD全局alpha */
    td_u32                bg_alpha;       /* alpha0 */
    td_u32                fg_alpha;       /* alpha1 */
    td_u64                phy_addr;       /* OSD图像存储位置 */
    td_u32                bg_color;
    td_u32                stride;         /* OSD图像的行stride */
    ext_isp_pixel_format  pixel_format;
} ext_vgs_osd_attr;

typedef struct {
    td_u32            num;                          /* 通道绑定的OSD个数 */
    td_bool           modify;                       /* OSD信息是否更新 */
    ext_vgs_osd_attr  osd[MAX_VGS_OSD_PER_CHN]; /* 获取的OSD最大attr */
} ext_vgs_drv_osd_info;

typedef td_s32 fn_vgs_dev_init(td_void);

typedef td_s32 fn_vgs_dev_deinit(td_void);

typedef td_s32 fn_vgs_begin_job(vgs_handle *handle, ext_vgs_job_pri pri_level, ext_mpp_chn mpp_chn,
    ext_vgs_job_data *job_data);

typedef td_s32 fn_vgs_end_job(vgs_handle handle, td_bool sort, ext_vgs_job_data *job_data);

typedef td_s32 fn_vgs_end_job_block(vgs_handle handle);

typedef td_s32 fn_vgs_cancel_job(vgs_handle handle);

typedef td_s32 fn_vgs_add_cover_task(vgs_handle handle, ext_vgs_task_data *task, ext_vgs_cover_opt *cover_opt);

typedef td_s32 fn_vgs_add_osd_task(vgs_handle handle, ext_vgs_task_data *task, ext_vgs_osd_opt *osd_opt);

typedef td_s32 fn_vgs_add_online_task(vgs_handle handle, ext_vgs_task_data *task, ext_vgs_online_opt *online_opt);

typedef td_s32 fn_vgs_add_2scale_task(vgs_handle handle, ext_vgs_task_data *task);

typedef td_s32 fn_vgs_add_get_luma_stat_task(vgs_handle handle, ext_vgs_task_data *task,
    ext_vgs_lumastat_opt *luma_info_opt);

typedef td_s32 fn_vgs_add_get_luma_stat_task_user(vgs_handle handle, ext_vgs_task_data *task_attr,
    ext_isp_rect *luma_rect, td_u64 *luma_data, td_u32 array_size);

typedef td_s32 fn_vgs_add_rotation_task(vgs_handle handle, ext_vgs_task_data *task, ext_isp_rotation angle);

typedef td_s32 fn_vgs_add_bypass_task(vgs_handle handle, ext_vgs_task_data *task);

typedef td_s32 fn_vgs_add_gme_task(vgs_handle handle, ext_vgs_task_data *task, vgs_gme_opt *gme_opt);

typedef ext_vgs_task_data *fn_vgs_get_free_task(td_void);

typedef td_void fn_vgs_put_free_task(ext_vgs_task_data *task_data);

typedef td_s32 fn_vgs_get_osd(const ext_mpp_chn *chn, ext_vgs_drv_osd_info *info);

typedef td_s32 fn_vgs_put_osd(const ext_mpp_chn *chn);

typedef td_s32 fn_vgs_add_multi_chn_task(vgs_handle handle, ext_vgs_task_data *task,
    ext_vgs_multi_chn_opt *multi_chn_opt);

typedef td_s32 fn_vgs_add_corner_rect_task(vgs_handle handle, ext_vgs_task_data *task,
    ext_vgs_corner_rect_opt *corner_rect_opt);

typedef struct {
    fn_vgs_dev_init *pfn_vgs_dev_init;
    fn_vgs_dev_deinit *pfn_vgs_dev_deinit;
    fn_vgs_begin_job *pfn_vgs_begin_job;
    fn_vgs_cancel_job *pfn_vgs_cancel_job;
    fn_vgs_end_job *pfn_vgs_end_job;
    fn_vgs_add_cover_task *pfn_vgs_add_cover_task;
    fn_vgs_add_osd_task *pfn_vgs_add_osd_task;
    fn_vgs_add_bypass_task *pfn_vgs_add_bypass_task;
    fn_vgs_add_get_luma_stat_task *pfn_vgs_get_luma_stat_task;
    fn_vgs_add_get_luma_stat_task_user *pfn_vgs_get_luma_stat_task_user;
    fn_vgs_add_online_task *pfn_vgs_add_online_task;
    fn_vgs_add_2scale_task *pfn_vgs_add_2scale_task;
    fn_vgs_add_rotation_task *pfn_vgs_add_rotation_task;
    fn_vgs_add_gme_task *pfn_vgs_add_gme_task;
    fn_vgs_end_job_block *pfn_vgs_end_job_block;
    fn_vgs_get_free_task *pfn_vgs_get_free_task;
    fn_vgs_put_free_task *pfn_vgs_put_free_task;
    fn_vgs_get_osd *pfn_vgs_get_osd;
    fn_vgs_put_osd *pfn_vgs_put_osd;
    fn_vgs_add_multi_chn_task *pfn_vgs_add_multi_chn_task;
    fn_vgs_add_corner_rect_task *pfn_vgs_add_corner_rect_task;
} vgs_export_func;

td_s32 drv_vgs_mod_init(td_void);
td_void drv_vgs_mod_exit(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* __DRV_VGS_EXT_H__ */

