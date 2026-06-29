/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: Function of mpi_isp_alg_struct.h
 * Author: AuthorNameMagicTag
 * Create: 2023-01-13
 */

#ifndef __MPI_ISP_ALG_STRUCT_H__
#define __MPI_ISP_ALG_STRUCT_H__

#include "isp_acs_struct.h"
#include "isp_ae_struct.h"
#include "isp_af_struct.h"
#include "isp_antifalsecolor_struct.h"
#include "isp_blc_struct.h"
#include "isp_ca_struct.h"
#include "isp_clut_struct.h"
#include "isp_csc_struct.h"
#include "isp_dehaze_struct.h"
#include "isp_demosaic_struct.h"
#include "isp_dp_struct.h"
#include "isp_drc_struct.h"
#include "isp_flicker_struct.h"
#include "isp_gamma_struct.h"
#include "isp_gcac_struct.h"
#include "isp_ge_struct.h"
#include "isp_lcac_struct.h"
#include "isp_ldci_struct.h"
#include "isp_lsc_struct.h"
#include "isp_nr_struct.h"
#include "isp_pregamma_struct.h"
#include "isp_sharpen_struct.h"
#include "isp_wb_struct.h"
#include "isp_wdr_struct.h"
#include "isp_de_struct.h"
#include "isp_cac_struct.h"
#include "isp_gdci_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

typedef struct {
    ext_isp_statistics_ctrl      ctrl;
    ext_isp_statistics_ctrl      update;
    ext_isp_ae_statistics_cfg    ae_cfg;
    ext_isp_wb_statistics_cfg    wb_cfg;
    ext_isp_focus_statistics_cfg focus_cfg;
} ext_isp_statistics_cfg;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
