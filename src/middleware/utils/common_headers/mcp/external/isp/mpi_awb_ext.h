/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2021. All rights reserved.
 * Description: Function of mpi_awb_ext.h
 * Author: AuthorNameMagicTag
 * Create: 2012-06-28
 */

#ifndef __MPI_AWB_EXT_H__
#define __MPI_AWB_EXT_H__

#include "mpi_isp_sns_struct.h"
#include "mpi_isp_alg_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_s32 ext_mpi_awb_set_attr(td_s32 pipe_id, const ext_isp_wb_attr *wb_attr);
td_s32 ext_mpi_awb_get_attr(td_s32 pipe_id, ext_isp_wb_attr *wb_attr);

td_s32 ext_mpi_awb_set_ccm_attr(td_s32 pipe_id, const ext_isp_color_matrix_attr *ccm_attr);
td_s32 ext_mpi_awb_get_ccm_attr(td_s32 pipe_id, ext_isp_color_matrix_attr *ccm_attr);

td_s32 ext_mpi_awb_set_ccm_dgain_attr(td_s32 pipe_id, const ext_isp_ccm_dgain_builtin_attr *dgain_attr);
td_s32 ext_mpi_awb_get_ccm_dgain_attr(td_s32 pipe_id, ext_isp_ccm_dgain_builtin_attr *dgain_attr);

td_s32 ext_mpi_awb_set_saturation_attr(td_s32 pipe_id, const ext_isp_saturation_attr *sat_attr);
td_s32 ext_mpi_awb_get_saturation_attr(td_s32 pipe_id, ext_isp_saturation_attr *sat_attr);

td_s32 ext_mpi_awb_query_info(td_s32 pipe_id, ext_isp_wb_info *wb_info);
td_s32 ext_mpi_awb_cal_gain_by_temp(td_s32 pipe_id, const ext_isp_wb_attr *wb_attr,
    td_u16 color_temp, td_s16 shift, td_u16 *awb_gain);

td_s32 ext_mpi_isp_set_awb_attr_ex(td_s32 pipe_id, const ext_isp_awb_attr_ex *awb_attr_ex);
td_s32 ext_mpi_isp_get_awb_attr_ex(td_s32 pipe_id, ext_isp_awb_attr_ex *awb_attr_ex);

td_s32 ext_mpi_isp_set_color_sector_attr(td_s32 pipe_id, const ext_isp_color_sector_attr *sector_attr);
td_s32 ext_mpi_isp_get_color_sector_attr(td_s32 pipe_id, ext_isp_color_sector_attr *sector_attr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
