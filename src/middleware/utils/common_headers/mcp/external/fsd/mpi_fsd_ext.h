/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Function of mpi_fsd_ext.h
 * Author: AuthorNameMagicTag
 * Create: 2025-11-29
 */

#ifndef __MPI_FSD_EXT_H__
#define __MPI_FSD_EXT_H__

#include "drv_ioctl_fsd.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_s32 ext_mpi_fsd_init(ext_fsd_alg_coef *alg_attr);
td_s32 ext_mpi_fsd_begin_job(td_s32 *handle);
td_s32 ext_mpi_fsd_add_task(td_s32 handle, const ext_fsd_task_input_attr *task);
td_s32 ext_mpi_fsd_end_job(td_s32 handle, ext_fsd_task_output_info *result);
td_s32 ext_mpi_fsd_cancel_job(td_s32 handle);
td_s32 ext_mpi_fsd_deinit(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
