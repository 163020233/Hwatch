/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: cis mpi ext define
 * Author: AuthorNameMagicTag
 * Create: 2021-8-28
 */

#ifndef __MPI_CIS_EXT_H__
#define __MPI_CIS_EXT_H__

#include "mpi_mcp_dependency.h"
#include "mpi_isp_sns_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

typedef struct {
    td_u32               width;
    td_u32               height;
    td_float             fps;
    ext_isp_bayer_format pattern;
    ext_isp_pixel_format pix_fmt;
    ext_isp_wdr_mode     wdr_mode;
    td_u32               inclk;
    td_u32               lane_num;
    td_u32               mode;
    ext_isp_data_bitwidth bitwidth;
} ext_isp_sns_feature;

typedef struct {
    td_s32  (*pfn_register_callback)(td_s32 pipe_id);
    td_s32  (*pfn_un_register_callback)(td_s32 pipe_id);
    td_s32  (*pfn_set_bus_info)(td_s32 pipe_id, ext_isp_sns_commbus *sns_bus_info);
    td_void (*pfn_standby)(td_s32 pipe_id);
    td_void (*pfn_restart)(td_s32 pipe_id);
    td_void (*pfn_mirror_flip)(td_s32 pipe_id, ext_isp_sns_mirrorflip_type sns_mirror_flip);
    td_s32  (*pfn_set_init)(td_s32 pipe_id, ext_isp_init_attr *init_attr);
    td_s32  (*pfn_enum_features)(td_s32 pipe_id, ext_isp_sns_feature *feature, td_u32 feature_size);
} ext_isp_sns_obj;

typedef struct {
    td_void (*pfn_cmos_sensor_init)(void);
    td_void (*pfn_cmos_sensor_exit)(void);
    td_s32  (*pfn_get_sensor_features)(ext_isp_sns_feature *feature);
} ext_dvp_sns_obj;

ext_dvp_sns_obj *ext_mpi_get_bf20a6_obj(void);
ext_dvp_sns_obj *ext_mpi_get_gc032a_obj(void);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __MPI_CIS_EXT_H__ */
