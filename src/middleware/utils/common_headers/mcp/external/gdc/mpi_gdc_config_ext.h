/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: gdc mpi pointquery ext interface
 * Author: AuthorNameMagicTag
 * Create: 2021-8-28
 */

#ifndef __MPI_GDC_CONFIG_EXT_H__
#define __MPI_GDC_CONFIG_EXT_H__

#include "mpi_mcp_dependency.h"
#include "drv_ioctl_gdc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define MATH_PI           3.1415926 /* (3.1415926536) old:3.1415926 real:3.1415926535897932384626433832795 */

#define LDC_DECIMAL_BIT   12
#define LDC_NOMALIZE_BIT  12
#define NOMALIZE_BIT      13
#define COEFF_DECIMAL_BIT 8

/* Description: fisheye parameters configuration */
td_s32 ext_mpi_gdc_fisheye_configure(const ext_gdc_comm_cfg *comm_cfg, const ext_gdc_fisheye_cfg *fisheye_cfg,
    ext_gdc_fw_comm_param *comm_param, ext_gdc_fw_fisheye_param *fisheye_param);

/* Description: trapzoid parameters configuration */
td_s32 ext_mpi_gdc_trapzoid_cfg(const ext_isp_size *dst_img_size, const td_u32 trapezoid_coef,
    ext_gdc_fw_pmf_param *pmf_param);

/* Description: lens distortion parameters configuration */
td_s32 ext_mpi_gdc_ldc_cfg(const ext_gdc_comm_cfg *comm_cfg, const ext_isp_ldc_attr *ldc_cfg,
    ext_gdc_fw_comm_param *comm_param, ext_gdc_fw_fisheye_param *fisheye_param);

/* Description: spread parameters configuration */
td_s32 ext_mpi_gdc_spread_configure(const ext_gdc_comm_cfg *comm_cfg, const ext_isp_size *dst_img_size,
    const td_u32 spread_coef, ext_gdc_fw_comm_param *comm_param, ext_gdc_fw_pmf_param *pmf_param);

/* Description: free angle rotation (360 deg) parameters configuration */
td_s32 ext_mpi_gdc_free_angle_rotation_cfg(const ext_gdc_comm_cfg *comm_cfg, const ext_isp_rotation_ex *rot_cfg,
    ext_gdc_fw_comm_param *comm_param, ext_gdc_fw_pmf_param *pmf_param);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __MPI_GDC_CONFIG_EXT_H__ */

