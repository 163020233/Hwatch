/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: gdc ext define
 * Author: AuthorNameMagicTag
 * Create: 2025-06-30
 */

#ifndef __DRV_GDC_EXT_H__
#define __DRV_GDC_EXT_H__

#include "drv_mcp_dependency.h"
#include "drv_dis_struct.h"
#include "drv_gdc_struct.h"
#include "drv_vb_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef td_s32 gdc_chn;

#define GDC_TASK_PRIVATE_DATA_NUM        8
#define GDC_TASK_INNER_DATA_NUM          3
#define GDC_JOB_PRIVATE_DATA_NUM         4
#define GDC_MAX_GYRO_SAMPLE_PER_FRAME    144 /* max_gyro_rate/min_frame_rate : 3500/25 */
#define GDC_STRIPING_NUM                 3
#define GDC_LUT_NUM                      2

typedef enum {
    EXT_GDC_JOB_TYPE_BYPASS = 0,
    EXT_GDC_JOB_TYPE_NORMAL = 1,
    EXT_GDC_JOB_TYPE_MAX
} ext_gdc_job_type;

typedef enum {
    EXT_GDC_JOB_FNSH_STAT_OK = 1,               /* JOB已正确完成(表示其下的所有task已正常完成) */
    EXT_GDC_JOB_FNSH_STAT_FAIL = 2,             /* JOB执行出现异常或未完成 */
    EXT_GDC_JOB_FNSH_STAT_CANCELED = 3,         /* JOB执行出现异常或未完成 */

    EXT_GDC_JOB_FNSH_STAT_MAX
} ext_gdc_job_finish_state;

typedef enum {
    EXT_GDC_TASK_FNSH_STAT_OK = 1,              /* task已正确完成 */
    EXT_GDC_TASK_FNSH_STAT_FAIL = 2,            /* 非低延时task执行出现异常或未完成 */
    EXT_GDC_TASK_FNSH_STAT_CANCELED = 3,        /* 被cancel掉 */
    EXT_GDC_TASK_FNSH_STAT_LOWDELAY = 4,        /* 低延时job中的task处于低延时中断处理中 */
    EXT_GDC_TASK_FNSH_STAT_LOWDELAY_FIN = 5,    /* 低延时job中的task处于链表中断处理中 */
    EXT_GDC_TASK_FNSH_STAT_LOWDELAY_ERR = 6,    /* 低延时job中的task出现异常或未完成 */

    EXT_GDC_TASK_FNSH_STAT_MAX
} ext_gdc_task_finish_state;

typedef struct {
    td_bool enable;
    td_u32 line_cnt; /* RW; range: [32, 16384]; low delay shoreline. */
} ext_gdc_low_delay_info;

typedef struct  {
    ext_gdc_fw_comm_param comm_params;
    ext_gdc_fw_fisheye_param fisheye_params;
    ext_gdc_fw_pmf_param pmf_params;
    ext_gdc_fw_fifo_param fifo_params;
} ext_gdc_fw_param;

/*
 * GDC操作数据信息结构，包括输入、输出图像信息及调用者，回调函数信息。
 * GDC任务完成后，每个操作以回调函数形式通知用户，该结构作为回调函数参数
 * 返回该操作对应的数据信息
 */
typedef struct gdc_task_data {
    ext_isp_video_frame_info img_in;
    ext_isp_video_frame_info img_out;
    td_u64 private_data[GDC_TASK_PRIVATE_DATA_NUM];
    td_u32 inner_data[GDC_TASK_INNER_DATA_NUM];
    td_void (*call_back)(mcp_soc_mod_id call_mod_id, td_s32 call_dev_id, td_s32 call_chn_id,
         const struct gdc_task_data *task);
    mcp_soc_mod_id call_mod_id;
    td_s32 call_dev_id;
    td_s32 call_chn_id;
    ext_gdc_task_finish_state finish_state;
    td_u32 reserved;                      /* save current picture's state while debug */
} ext_gdc_task_data;

/* GDC JOB信息结构，一个GDC JOB完成后通知用户时返回给用户的信息 */
typedef struct gdc_job_data {
    td_u32 private_data[GDC_JOB_PRIVATE_DATA_NUM];
    ext_gdc_job_finish_state job_finish_state;
    ext_gdc_job_type job_type;
    ext_gdc_low_delay_info low_delay_info;
    td_void (*job_call_back)(mcp_soc_mod_id call_mod_id, td_s32 call_dev_id, td_s32 call_chn_id,
        const struct gdc_job_data *job_data);
} ext_gdc_job_data;

typedef struct {
    td_u32 jobs_canceled; /* 成功cancel多少个job */
    td_u32 jobs_left;     /* 还剩余多少个job */
} ext_gdc_cancel_state;

typedef struct {
    td_u32 hmax_times;
    td_u32 vmax;
    td_u32 exposure_time;
} ext_gdc_dis_info;

typedef struct {
    td_bool lmf_enable;
    td_bool lmf_modified;
    td_u16 lmf_coef[GDC_FISHEYE_LMFCOEF_NUM]; /* LMF coefficient of gdc len */

    td_bool user_node;
    td_u64 len_map_phy_addr;

    td_bool bg_color_enable;
    td_u32 bg_color;

    td_s32 hor_offset; /* the horizontal offset between image center and physical center of len */
    td_s32 ver_offset; /* the vertical offset between image center and physical center of len */

    td_u32 trapezoid_coef;
    td_s32 fan_strength;
    ext_gdc_fisheye_mount_mode mount_mode;
} ext_gdc_correction_common;

typedef struct {
    td_bool enable;
    ext_gdc_correction_common gdc_comm_attr;

    ext_gdc_sdk_phyaddr sdk_phy_addr;
    ext_gdc_fw_comm_param fw_comm_param;
    ext_gdc_fw_pmf_param fw_pmf_param;
    ext_gdc_fw_fisheye_param fw_fisheye_param;
    ext_gdc_fw_fifo_param fw_fifo_param;
    ext_gdc_fisheye_region_attr gdc_region_attr;
} ext_gdc_correction_opt;

typedef struct {
    td_bool enable;
    ext_isp_rotation rotation;
    td_bool dis_enable;
    td_bool pmf_enable;

    ext_isp_ldc_attr ldc_attr;
    ext_isp_ldc_v2_attr ldc_v2_attr;

    ext_gdc_fw_pmf_param fw_pmf_param;
    ext_gdc_fw_comm_param fw_comm_param;
    ext_gdc_fw_fisheye_param fw_fisheye_param;
    ext_gdc_fw_fifo_param fw_fifo_param;
} ext_gdc_ldc_opt;

typedef enum {
    EXT_GDC_LUT_MODE_GDC_LUT = 0,
    EXT_GDC_LUT_MODE_USR_LUT = 1,
    EXT_GDC_LUT_MODE_USR_LUT_FUSION = 2,
    EXT_GDC_LUT_MODE_USR_LUT_CONNECT_USR_LUT = 3,
    EXT_GDC_LUT_MODE_GDC_LUT_CONNECT_USR_LUT = 4,
    EXT_GDC_LUT_MODE_USR_LUT_CONNECT_GDC_LUT = 5,
    EXT_GDC_LUT_MODE_BUTT
} ext_gdc_lut_intp_mode;

typedef struct {
    ext_isp_size im_size;
    ext_isp_size mesh_size;
    ext_isp_size cell_size;

    td_u32 coor_weight_norm;
    td_u32 coor_weight_norm_right;
    td_u32 coor_weight_norm_bottom;
    td_u32 coor_weight_norm_rb;
    td_u32 cell_log_h;
    td_u32 cell_log_v;
} ext_gdc_global_mesh;

typedef struct {
    ext_isp_size blk_mesh_size;
    ext_isp_point *mesh_point;
    ext_isp_rect  blk_rect;
    ext_isp_rect  rect_roi;
} ext_gdc_blk_lut_cfg;

typedef struct {
    /* regular mesh */
    ext_gdc_lut_intp_mode lut_intp_mode;
    td_u32  mesh_intp_ratio;
    td_u32 pos_dec_bit;      /* pixel position decimal bits */

    td_u32 interp_coeff_norm_bit;
    td_u32 interp_phase_bit;
    td_u32 coeff_tap_h;
    td_u32 coeff_tap_v;
    td_s32 *coeff_h;
    td_s32 *coeff_v;
    ext_gdc_global_mesh glbl_mesh_param[GDC_LUT_NUM];
    ext_isp_point *mesh_point;
    ext_isp_point *mesh_point_next;

    td_u8 coor_weight_norm_bit;
    ext_gdc_blk_lut_cfg blk_mesh_param[GDC_STRIPING_NUM];
} ext_gdc_fw_mesh_param;

typedef struct {
    td_bool pmf_en;
    td_bool scale_enable;
    ext_gdc_comm_cfg comm_cfg;

    ext_gdc_fw_pmf_param fw_pmf_param;
    ext_gdc_fw_comm_param fw_comm_param;
    ext_gdc_fw_fifo_param fw_fifo_param;
    ext_gdc_fw_mesh_param fw_mesh_param;
} ext_gdc_pmf_opt;

typedef struct {
    td_bool lmf_enable;
    td_u64 len_map_phy_addr;
} ext_gdc_lmf_opt;

typedef struct {
    ext_gdc_fw_comm_param comm_params;
    ext_gdc_fw_gyrodis_param gyrodis_param;
    ext_gdc_fw_fifo_param fifo_params;
} ext_gdc_fw_localdis_param;

typedef struct {
    td_bool enable;
    td_s64 pmf[GDC_PMFCOEF_NUM];
    ext_isp_size dest_size;
} ext_gdc_pmf_cfg;

typedef struct {
    td_u64 angle_vel_phy_addr;
    ext_gdc_fw_localdis_param gdc_gyro_param;
} ext_gyro_dis_attr;

typedef struct {
    td_bool gyro_enable;
    ext_gyro_dis_attr gyro_attr;
} ext_gyro_dis_opt;

typedef struct {
    td_u32 width;
    td_u32 height;

    ext_isp_pixel_format pix_format;
    ext_isp_compress_mode compress_mode;
    ext_isp_dynamic_range dynamic_range;
} ext_gdc_chn_attr;

typedef struct {
    mcp_soc_mod_id mod_id;
    td_s32 call_dev_id;
    td_s32 call_chn_id;
    gdc_chn channel;
    td_u32 align;
} ext_gdc_mod_call_state;

typedef struct {
    td_bool config;
    td_bool dis_enable;
    td_bool dis_scale;
    td_bool spread_enable;
    ext_isp_rotation rotation;
    td_bool rotation_ex_enable;
    td_bool ldc_enable;
    td_bool ldc_v2_enable;  /* 与ldc_enable不能同时使用 */
    td_bool bypass_enable;
} ext_gdc_door_bell;

typedef struct {
    ext_gdc_spread_attr spread_attr;
    ext_isp_ldc_v2_attr ldcv2_attr;
    ext_dis_config dis_config;
    ext_dis_attr dis_attr;
    ext_gdc_dis_info isp_info;
    ext_gdc_low_delay_info low_delay_info;
    td_bool mirror_enable;
    td_bool flip_enable;
} ext_gdc_mpi_attr;

typedef struct {
    td_bool config;
    ext_gdc_ldc_opt ldc_opt;
    ext_gdc_pmf_opt pmf_opt;
    ext_gdc_pmf_opt pmf_opt_spread;
    ext_gdc_pmf_opt pmf_opt_rotation;
    ext_gyro_dis_opt gdc_gyro_opt;
} ext_gdc_chn_opt_ctx;

typedef td_s32 fn_gdc_dev_init(td_void);

typedef td_s32 fn_gdc_dev_deinit(td_void);

typedef td_s32 fn_gdc_begin_job(td_u32 *handle, ext_mpp_chn *mpp_chn, ext_gdc_job_data *job_data);

typedef td_s32 fn_gdc_end_job(td_u32 handle);

typedef td_s32 fn_gdc_cancel_job(td_u32 handle);

typedef td_s32 fn_gdc_cancel_job_by_mod_dev(ext_mpp_chn *mpp_chn, ext_gdc_cancel_state *cancel_state);

typedef td_s32 fn_gdc_add_correction_task(td_u32 handle, ext_gdc_task_data *task,
    ext_gdc_correction_opt *correction_opt);

typedef td_s32 fn_gdc_correction_proc(ext_gdc_task_data *task, ext_gdc_fisheye_drv_attr *gdc_attr);

typedef td_s32 fn_gdc_ldc_proc(ext_gdc_task_data *task, ext_gdc_ldc_opt *ldc_opt);

typedef td_s32 fn_gdc_check_correction_info(const ext_gdc_chn_attr *src_chn_attr,
    const ext_gdc_chn_attr *dst_chn_attr, const ext_gdc_fisheye_attr *gdc_attr, const td_bool use_xy,
    const td_bool check_mode);

typedef td_s32 fn_gdc_check_ldc_info(const ext_gdc_chn_attr *chn_attr, const ext_isp_ldc_attr *ldc_attr);

typedef td_s32 fn_gdc_check_spread_info(const ext_gdc_spread_attr *spread_attr);

typedef td_s32 fn_gdc_check_rotation_ex_info(const ext_gdc_chn_attr *chn_attr,
    const ext_isp_rotation_ex *rotation_ex);

typedef td_s32 fn_gdc_pmf_proc(ext_gdc_task_data *submit_task, ext_gdc_pmf_opt *pmf_opt);

typedef td_s32 fn_gdc_pmf_and_rotation_cfg(const ext_gdc_comm_cfg *comm_cfg, const ext_gdc_pmf_cfg *pmf_cfg,
    const ext_isp_rotation gdc_rot_type, ext_gdc_fw_comm_param *fw_comm_param, ext_gdc_fw_pmf_param *fw_pmf_param);

typedef td_s32 fn_gdc_fisheye_fifo_calculate(const ext_gdc_comm_cfg *comm_cfg,
    const ext_gdc_fisheye_cfg *fisheye_cfg, ext_gdc_fw_param *fw_param, ext_gdc_fw_fifo_param *fw_fifo_param);

typedef td_s32 fn_gdc_dst_size_cfg(const ext_isp_rotation gdc_rot_type, const ext_gdc_pmf_cfg *pmf_cfg,
    ext_gdc_fw_comm_param *fw_comm_param);

typedef td_s32 fn_gdc_get_chn_opt_ctx(gdc_chn channel, ext_gdc_chn_opt_ctx **gdc_opt_ctx);

typedef ext_gdc_task_data *fn_gdc_get_free_task(td_bool fisheye);

typedef td_void fn_gdc_put_free_task(td_bool fisheye, ext_gdc_task_data *gdc_task_mode);

typedef td_s32 fn_gdc_all_function_proc(ext_gdc_task_data *gdc_task_mode, ext_gdc_mod_call_state *mod_state,
    ext_gdc_door_bell *door_bell, ext_dis_task_data *dis_task_out, ext_gdc_mpi_attr *mpi_attr);

typedef td_s32 fn_gdc_create_chn(gdc_chn *channel);

typedef td_s32 fn_gdc_destroy_chn(gdc_chn channel);

typedef td_s32 fn_gdc_add_gyro_task(td_u32 gdc_handle, ext_gdc_task_data *task, ext_gyro_dis_opt *gyro_opt);

typedef struct {
    fn_gdc_dev_init *pfn_gdc_dev_init;
    fn_gdc_dev_deinit *pfn_gdc_dev_deinit;
    fn_gdc_begin_job *pfn_gdc_begin_job;
    fn_gdc_end_job *pfn_gdc_end_job;
    fn_gdc_cancel_job *pfn_gdc_cancel_job;
    fn_gdc_cancel_job_by_mod_dev *pfn_gdc_cancel_job_by_mod_dev;
    fn_gdc_add_correction_task *pfn_gdc_add_correction_task;
    fn_gdc_correction_proc *pfn_gdc_correction_proc;
    fn_gdc_ldc_proc *pfn_gdc_ldc_proc;
    fn_gdc_check_correction_info *pfn_gdc_check_correction_info;
    fn_gdc_check_ldc_info *pfn_gdc_check_ldc_info;
    fn_gdc_check_spread_info *pfn_gdc_check_spread_info;
    fn_gdc_check_rotation_ex_info *pfn_gdc_check_rotation_ex_info;
    fn_gdc_pmf_proc *pfn_gdc_pmf_proc;
    fn_gdc_pmf_and_rotation_cfg *pfn_gdc_pmf_and_rotation_cfg;
    fn_gdc_fisheye_fifo_calculate *pfn_gdc_fishey_fifo_calc;
    fn_gdc_dst_size_cfg *pfn_gdc_dst_size_cfg;
    fn_gdc_get_chn_opt_ctx *pfn_gdc_get_chn_opt_ctx;
    fn_gdc_get_free_task *pfn_gdc_get_free_task;
    fn_gdc_put_free_task *pfn_gdc_put_free_task;
    fn_gdc_all_function_proc *pfn_gdc_all_function_proc;
    fn_gdc_create_chn *pfn_gdc_create_chn;
    fn_gdc_destroy_chn *pfn_gdc_destroy_chn;
    fn_gdc_add_gyro_task *pfn_gdc_add_gyro_task;
} gdc_export_func;

td_s32 drv_gdc_mod_init(td_void);
td_void drv_gdc_mod_exit(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __DRV_GDC_EXT_H__ */
