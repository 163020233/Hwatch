/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: define for external use
 * Author: AuthorNameMagicTag
 * Create: 2021-09-22
 */

#ifndef __MPI_SNAP_EXT_H__
#define __MPI_SNAP_EXT_H__

#include "drv_ioctl_viu.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

td_s32 ext_mpi_snap_set_pipe_attr(td_s32 viu_pipe_id, const ext_snap_attr *snap_attr);
td_s32 ext_mpi_snap_get_pipe_attr(td_s32 viu_pipe_id, ext_snap_attr *snap_attr);
td_s32 ext_mpi_snap_enable_pipe(td_s32 viu_pipe_id);
td_s32 ext_mpi_snap_disable_pipe(td_s32 viu_pipe_id);
td_s32 ext_mpi_snap_trigger_pipe(td_s32 viu_pipe_id);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __MPI_SNAP_EXT_H__ */
