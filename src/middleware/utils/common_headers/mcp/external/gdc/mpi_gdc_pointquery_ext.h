/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: gdc mpi pointquery ext interface
 * Author: AuthorNameMagicTag
 * Create: 2021-8-28
 */

#ifndef __MPI_GDC_POINTQUERY_EXT_H__
#define __MPI_GDC_POINTQUERY_EXT_H__

#include "mpi_mcp_dependency.h"
#include "drv_ioctl_gdc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define SCALE_DECIMAL_BIT             10

#define descale(x, n) ((n) <= 0 ? ((x) << (-(n))) : \
    ((x) >= 0 ? (((x) + (1 << ((n) - 1))) >> (n)) : (-(((-(x)) + (1 << ((n) - 1))) >> (n)))))

typedef struct {
    ext_gdc_view_mode view_mode;
    td_u32 width;
    td_u32 height;
    td_u32 out_width;
    td_u32 out_height;
    td_u32 width_b4_pmf;
    td_u32 height_b4_pmf;
    td_u32 center_src_h;
    td_u32 center_src_v;
    td_u32 center_dst_h;
    td_u32 center_dst_v;

    td_bool pmf_en;
    td_s64 pmf[GDC_PMFCOEF_NUM];
    td_bool is_affine;
    td_u8 pmf_norm_bits;

    ext_gdc_fisheye_mount_mode load_mode;
    td_u32 radius;
    td_s32 pan;
    td_s32 tilt;
    td_u32 scale_h;
    td_u32 scale_v;
    td_bool lmf;
    td_s16 lmf_array[GDC_FISHEYE_LMFCOEF_NUM + 1];

    /* for pano 360 correction */
    td_s32 min;
    td_u32 lmf_coef;

    /* for pano 180 correction */
    td_s32 tilt0;
    td_s32 tilt1;
    td_u32 virt_y;
    td_s32 offset180;
    td_s32 fan_strength;

    /* for rectlinear correction */
    td_u32 ldc_type;
    td_s32 detla_xu;
    td_s32 detla_yu;
    td_s32 detla_zu;
    td_s32 detla_xv;
    td_s32 detla_yv;
    td_s32 detla_zv;
    td_s32 start_x;
    td_s32 start_y;
    td_s32 start_z;
} ext_gdc_pointquery_cfg;

/* Description: fisheye point query configuration */
td_void ext_mpi_gdc_fisheye_point_query_cfg(ext_gdc_pointquery_cfg *cfg, const ext_gdc_fw_comm_param *comm_param,
    const ext_gdc_fw_pmf_param *pmf_param, const ext_gdc_fw_fisheye_param *fisheye_param);

/* Description: ldcv1 point query configuration */
td_void ext_mpi_gdc_ldcv1_point_query_cfg(ext_gdc_pointquery_cfg *cfg, const ext_gdc_fw_comm_param *comm_param,
    const ext_gdc_fw_pmf_param *pmf_param, const ext_gdc_fw_fisheye_param *fisheye_param);

/* Description: fisheye point query conver */
td_void ext_mpi_gdc_fisheye_point_query_conver(ext_gdc_pointquery_cfg *cfg,
    const ext_gdc_fisheye_drv_attr *fisheye_drv_attr, td_u32 region_num);

/* Description: point query */
td_s32 ext_mpi_gdc_point_query(const ext_gdc_pointquery_cfg *cfg, const ext_isp_point *dst_point,
    ext_isp_point *src_point, td_bool *point_out);

/* Description: point query backwards */
td_s32 ext_mpi_gdc_point_query_backwards(const ext_gdc_pointquery_cfg *cfg, ext_isp_point *dst_point,
    const ext_isp_point *src_point, td_bool *point_out);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __MPI_GDC_POINTQUERY_EXT_H__ */

