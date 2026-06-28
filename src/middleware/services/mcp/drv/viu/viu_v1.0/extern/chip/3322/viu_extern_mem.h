/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: viu_extern_mem.h
 * Author: AuthorNameMagicTag
 * Create: 2025-06-30
 */

#ifndef VIU_EXTERN_MEM_H
#define VIU_EXTERN_MEM_H

#include "drv_mcp_dependency.h"

td_s32 vi_mem_init(td_void);
td_s32 vi_mem_deinit(td_void);

td_s32 vi_mem_create_private_pool(td_u32 blk_cnt, td_u64 blk_size);
td_s32 vi_mem_destroy_private_pool(td_void);

vb_blk_handle viu_mem_get_blk_by_size(td_u64 blk_size, td_u32 uid);
vb_blk_handle viu_mem_phy_to_handle(td_u64 phy_addr);
td_u64 viu_mem_handle_to_kern(vb_blk_handle handle);
td_u64 viu_mem_handle_to_phys(vb_blk_handle handle);
td_bool viu_mem_is_blk_valid(td_u32 pool_id, td_phys_addr_t phys_addr);
td_void viu_mem_get_calc_cfg(const ext_vb_base_info *vb_info, ext_isp_vb_cal_config *vb_calc_cfg);

td_s32 viu_mem_user_add(td_u32 pool_id, td_u64 phy_addr, td_u32 uid);
td_s32 viu_mem_user_sub(td_u32 pool_id, td_u64 phy_addr, td_u32 uid);

#endif

