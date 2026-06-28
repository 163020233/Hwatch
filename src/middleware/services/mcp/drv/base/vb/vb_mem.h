/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
* Description:vb memory header file
* Author: AuthorNameMagicTag
* Create: 2021-8-10
*/

#ifndef __VB_MEM_H__
#define __VB_MEM_H__

#include "drv_mcp_dependency.h"

#define VB_MAX_PID_NUM               5

typedef struct {
    td_mem_handle_t handle;
    td_u64 dma_buf;
} vb_mem_priv;

/* memory interface */
typedef struct {
    td_mem_handle_t handle;
    td_void        *dma_buf;
    td_u32          buf_size;
    td_u8          *vir_addr;
    td_u64          phy_addr;
    vb_mem_priv priv[VB_MAX_PID_NUM];
    td_u32 create_fd_num;
    td_u32 pid[VB_MAX_PID_NUM];
} vb_mem_info;

typedef td_void (*vb_put_umem_ops)(vb_mem_info *);
typedef td_s32  (*vb_get_umem_ops)(vb_mem_info *, td_bool, td_bool);

td_s32 vb_alloc_and_map_buf(const td_char *buf_name, td_u32 buf_size, td_bool is_map_viraddr, td_bool is_cached,
    vb_mem_info *buf_info);
td_void vb_release_and_unmap_buf(const vb_mem_info *buf_info);

td_s32 vb_get_mem_by_mem_handle(vb_mem_info *buf_info, td_bool kmap, td_bool cached);
td_void vb_put_mem_by_mem_handle(vb_mem_info *buf_info);
td_void vb_print_all_mem_node_info(td_void);

#endif
