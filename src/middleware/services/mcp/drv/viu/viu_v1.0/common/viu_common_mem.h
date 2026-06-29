/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: viu common header
 * Author: AuthorNameMagicTag
 * Create: 2025-06-30
 */

#ifndef __VIU_COMMON_MEM_H__
#define __VIU_COMMON_MEM_H__

#include "drv_mcp_dependency.h"

#ifdef __cplusplus
extern "C" {
#endif

#define VIU_MEM_NAME_LEN_MAX       16
#define u64_ptr(ptr) ((td_void *)(td_uintptr_t)(ptr))
#define ptr_u64(ptr) ((td_u64)(td_uintptr_t)(ptr))

typedef enum {
    VIU_MEM_TYPE_AUTO = 0,
    VIU_MEM_TYPE_SMMU,
    VIU_MEM_TYPE_MMZ,
    VIU_MEM_TYPE_MAX,
} viu_mem_type;

typedef enum {
    VIU_MEM_OP_ALLOC = 0,
    VIU_MEM_OP_ALLOC_MAP,
    VIU_MEM_OP_ALLOC_MAP_CACHE,
    VIU_MEM_OP_MAX,
} viu_mem_op;

typedef struct {
    td_mem_handle_t handle;
    td_void *dma_buf;
} viu_mem_priv;

typedef struct {
    td_char buf_name[VIU_MEM_NAME_LEN_MAX];
    mcp_osal_mem_type mem_type;
    td_bool is_cache;        /* 是否需要cache */
    td_bool is_map_viraddr;  /* 是否需要映射CPU地址 */
    td_u32 size;

    // output
    td_mem_handle_t handle;
    td_u64 dma_buf;
    td_void *vir_addr;
    td_u64 phy_addr;
} viu_mem_info;

td_s32 viu_mem_alloc_and_map(viu_mem_info *mem_info);
td_void viu_mem_unmap_and_free(viu_mem_info *mem_info);

#ifdef __cplusplus
}
#endif
#endif

