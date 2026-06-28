/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: mpi layer functions that are exposed
 * Author: AuthorNameMagicTag
 * Create: 2021-7-13
 */

#ifndef __MPI_VB_EXT_H__
#define __MPI_VB_EXT_H__

#include "drv_ioctl_vb.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

vb_pool ext_mpi_vb_create_pool(ext_vb_pool_config *vb_pool_cfg);
td_s32 ext_mpi_vb_destroy_pool(vb_pool pool);

vb_blk ext_mpi_vb_get_block(vb_pool pool, td_u64 blk_size);
td_s32 ext_mpi_vb_release_block(vb_blk block);

vb_blk ext_mpi_vb_phys_addr_to_handle(td_u64 phy_addr);
td_u64 ext_mpi_vb_handle_to_phys_addr(vb_blk block);
vb_pool ext_mpi_vb_handle_to_pool_id(vb_blk block);

td_s32 ext_mpi_vb_inquire_user_cnt(vb_blk block);

td_s32 ext_mpi_vb_get_supplement_addr(vb_blk block, ext_isp_video_supplement *supplement);
td_s32 ext_mpi_vb_set_supplement_config(const ext_vb_supplement_config *supplement_config);
td_s32 ext_mpi_vb_get_supplement_config(ext_vb_supplement_config *supplement_config);

td_s32 ext_mpi_vb_init(td_void);
td_s32 ext_mpi_vb_exit(td_void);
td_s32 ext_mpi_vb_set_config(const ext_vb_config *vb_config);
td_s32 ext_mpi_vb_get_config(ext_vb_config *vb_config);

td_s32 ext_mpi_vb_mmap_pool(vb_pool pool);
td_s32 ext_mpi_vb_munmap_pool(vb_pool pool);

td_s32 ext_mpi_vb_get_block_vir_addr(vb_pool pool, td_u64 phy_addr, td_void **vir_addr);

td_s32 ext_mpi_vb_init_mod_comm_pool(ext_vb_uid vb_uid);
td_s32 ext_mpi_vb_exit_mod_comm_pool(ext_vb_uid vb_uid);

td_s32 ext_mpi_vb_set_mod_pool_config(ext_vb_uid vb_uid, const ext_vb_config *vb_config);
td_s32 ext_mpi_vb_get_mod_pool_config(ext_vb_uid vb_uid, ext_vb_config *vb_config);

td_void* ext_mpi_vb_mmap(td_u64 phy_addr, td_u32 size);
td_s32 ext_mpi_vb_munmap(td_void *vir_addr, td_u32 size);

td_s32 ext_mpi_vb_calc_buffer_size(ext_vb_in_params *in_params, ext_vb_out_params *out_params);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __MPI_VB_EXT_H__ */
