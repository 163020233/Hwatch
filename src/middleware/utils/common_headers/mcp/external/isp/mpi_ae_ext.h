/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2021. All rights reserved.
 * Description: Function of mpi_ae_ext.h
 * Author: AuthorNameMagicTag
 * Create: 2012-06-28
 */

#ifndef __MPI_AE_EXT_H__
#define __MPI_AE_EXT_H__

#include "mpi_isp_sns_struct.h"
#include "mpi_isp_3a_struct.h"
#include "mpi_isp_alg_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_s32 ext_mpi_ae_set_attr(td_s32 pipe_id, const ext_isp_exposure_attr *exp_attr);
td_s32 ext_mpi_ae_get_attr(td_s32 pipe_id, ext_isp_exposure_attr *exp_attr);
td_s32 ext_mpi_ae_set_wdr_attr(td_s32 pipe_id, const ext_isp_wdr_exposure_attr *wdr_exp_attr);
td_s32 ext_mpi_ae_get_wdr_attr(td_s32 pipe_id, ext_isp_wdr_exposure_attr *wdr_exp_attr);

td_s32 ext_mpi_ae_set_route_attr(td_s32 pipe_id, const ext_isp_ae_route *ae_route_attr);
td_s32 ext_mpi_ae_get_route_attr(td_s32 pipe_id, ext_isp_ae_route *ae_route_attr);
td_s32 ext_mpi_ae_set_route_attr_ex(td_s32 pipe_id, const ext_isp_ae_route_ex *ae_route_attr_ex);
td_s32 ext_mpi_ae_get_route_attr_ex(td_s32 pipe_id, ext_isp_ae_route_ex *ae_route_attr_ex);

td_s32 ext_mpi_ae_set_route_sf_attr(td_s32 pipe_id, const ext_isp_ae_route *ae_route_sf_attr);
td_s32 ext_mpi_ae_get_route_sf_attr(td_s32 pipe_id, ext_isp_ae_route *ae_route_sf_attr);
td_s32 ext_mpi_ae_set_route_sf_attr_ex(td_s32 pipe_id, const ext_isp_ae_route_ex *ae_route_sf_attr_ex);
td_s32 ext_mpi_ae_get_route_sf_attr_ex(td_s32 pipe_id, ext_isp_ae_route_ex *ae_route_sf_attr_ex);

td_s32 ext_mpi_ae_query_info(td_s32 pipe_id, ext_isp_exp_info *exp_info);
td_s32 ext_mpi_isp_set_smart_exposure_attr(td_s32 pipe_id,
    const ext_isp_smart_exposure_attr *smart_exp_attr);
td_s32 ext_mpi_isp_get_smart_exposure_attr(td_s32 pipe_id, ext_isp_smart_exposure_attr *smart_exp_attr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
