/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: gdc ioctl define
 * Author: AuthorNameMagicTag
 * Create: 2021-8-28
 */

#ifndef __DRV_IOCTL_GDC_H__
#define __DRV_IOCTL_GDC_H__

#include "drv_gdc_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define PI_BITS                    17
#define PI                         (1 << PI_BITS)
#define PID2_BITS                  (PI_BITS - 1)
#define PID2                       (1 << PID2_BITS)
#define PIM2_BITS                  (PI_BITS + 1)
#define PIM2                       (1 << PIM2_BITS)
#define INTP_DECIMAL_BIT           4
#define INTP_COEF_LEN              ((1 << (INTP_DECIMAL_BIT - 1)) + 1)

#define GDC_MAX_LDC_OFFSET         511
#define GDC_MIN_LDC_RATIO          (-300)
#define GDC_MAX_LDC_RATIO          500
#define GDC_MIN_BARREL_LDC_RATIO   0
#define GDC_MAX_SPREAD_COEF        18
#define GDC_PRIVATE_DATA_NUM       4
#define GDC_MAX_OFFSET             511

typedef struct {
    ext_isp_video_frame_info img_in; /* input picture */
    ext_isp_video_frame_info img_out; /* output picture */
    td_u64 private_data[GDC_PRIVATE_DATA_NUM]; /* RW; private data of task */
    td_u64 reserved; /* RW; debug information,state of current picture */
} ioc_gdc_task_attr;

typedef struct {
    td_u32 spread_coef;
    ext_isp_rotation rotation;
} ioc_gdc_pmf_attr;

typedef struct {
    td_s64 pmf_coef[GDC_PMFCOEF_NUM];
} ioc_gdc_pmf;

typedef struct {
    td_u32 handle;
    ioc_gdc_task_attr task;
    ioc_gdc_pmf_attr pmf_fun_attr;
    ioc_gdc_pmf add_pmf;

    ext_gdc_comm_cfg comm_cfg;
    ext_gdc_fw_pmf_param fw_pmf_param;
    ext_gdc_fw_comm_param fw_comm_param;
    ext_gdc_fw_fisheye_param fw_fisheye_param;
} ioc_gdc_add_pmf_arg;

/* fisheye job config */
typedef struct {
    td_u64 len_map_phy_addr; /* LMF coefficient physic addr */
} ioc_gdc_fisheye_job_config;

typedef struct {
    td_u32 handle;
    ioc_gdc_fisheye_job_config fisheye_job_config; /* LMF coefficient physic addr */
} ioc_gdc_job_config_arg;

typedef struct {
    td_u32 handle;
    ioc_gdc_task_attr task;
    td_bool use_xy;
    ext_gdc_fisheye_attr fisheye_attr;
    ext_gdc_comm_cfg comm_cfg;
    td_bool check_mode;
    ext_gdc_fw_pmf_param fw_pmf_param[GDC_FISHEYE_MAX_REGION_NUM];
    ext_gdc_fisheye_cfg fisheye_cfg[GDC_FISHEYE_MAX_REGION_NUM];
    ext_gdc_fw_comm_param fw_comm_param[GDC_FISHEYE_MAX_REGION_NUM];
    ext_gdc_fw_fisheye_param fw_fisheye_param[GDC_FISHEYE_MAX_REGION_NUM];
} ioc_gdc_correction_attr;

typedef struct {
    td_u32 handle;
    ioc_gdc_task_attr task;
    ext_isp_ldc_attr add_ldc;
    ext_isp_rotation rotation;
    td_u32 spread_coef;
    ext_gdc_comm_cfg comm_cfg;
    ext_gdc_fw_pmf_param fw_pmf_param;
    ext_gdc_fw_comm_param fw_comm_param;
    ext_gdc_fw_fisheye_param fw_fisheye_param;
} ioc_gdc_add_ldc_arg;

typedef struct {
    td_u32 handle;
    ioc_gdc_task_attr task;
    ext_isp_rotation rotation;
} ioc_gdc_add_rotation_arg;

typedef struct {
    td_s32 angle; /* [-1<<14 , 1<<14] */
    td_s32 radius; /* [-1<<18 , 1<<18] */
    td_u32 phase; /* [0,3]            */
} ioc_polar_co;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __DRV_IOCTL_GDC_H__ */
