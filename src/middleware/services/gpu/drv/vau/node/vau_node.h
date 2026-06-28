/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: vau_interface
 */

#ifndef DRV_GRAPHIC_NODE_VAU_NODE_H
#define DRV_GRAPHIC_NODE_VAU_NODE_H

#include "drv_vau_type.h"
#include "vau_hal_node.h"
#include "soc_osal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum {
    VAU_MIX_NODE_MAPP = 0x0,
    VAU_MIX_NODE_DRAW = 0x1,
    VAU_MIX_NODE_SEMI = 0x2,
    VAU_MIX_NODE_SDF  = 0x3,
    VAU_MIX_NODE_MAX
} vau_mix_node_mode;

/* Data struct of software list node, total size need align 16 bytes */
typedef struct {
    struct osal_list_head list_head; /* 8 BYTES */
    td_u32 phy_addr;
    td_handle handle;
    td_u8 reserved[10]; /* 10:for algin */
    td_u8 mem_type;
    td_bool is_mesh_mem_set;
    td_void *hw_node;   /* support sdr/hdr hw node */
} vau_sw_node;

td_void vau_node_set_draw_flag(td_bool flag);
td_bool vau_node_get_draw_flag(td_void);

vau_sw_node *vau_node_create(vau_mix_node_mode node_mode);
td_s32 vau_node_add(td_handle handle, vau_sw_node *sw_node);
td_s32 vau_node_free(vau_sw_node *sw_node);

td_void vau_node_set_fast_dma(vau_hardware_node *node, const drv_vau_surface *src_surface,
                              const drv_vau_surface *dst_surface);
td_void vau_node_set_src1(vau_hardware_node *node, const drv_vau_surface *surface);
td_void vau_node_set_src2(vau_hardware_node *node, const drv_vau_surface *src_surface,
                          const drv_vau_surface *dst_surface);
td_void vau_node_set_mapping(vau_hardware_node *node, const drv_vau_surface *src_surface);
td_void vau_node_set_dst(vau_hardware_node *node, const drv_vau_surface *surface);
td_void vau_node_set_blend_mode(vau_hardware_node *node, drv_vau_blit_opt *opt);
td_void vau_node_set_gaussian(vau_hardware_node *node, drv_vau_blit_opt *opt);
td_void vau_node_set_cbm(vau_hardware_node *node, const drv_vau_blit_opt *opt, td_u32 color);
td_void vau_node_add_next_node(vau_hardware_node *node, td_u32 phy_addr);
td_void vau_node_set_finish_isr(vau_hardware_node *node, td_bool is_offline);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
