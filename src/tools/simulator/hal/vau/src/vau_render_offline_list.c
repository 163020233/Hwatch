/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: the function interface define in vector accelerate unit api source file
 * Create: 2025-7-28
 */

#include "vau_render_offline_list.h"

bool uapi_vau_is_off_list_support(void)
{
    return false;
}

td_s16 uapi_vau_get_node_size(void)
{
    return -1;
}

td_s32 uapi_vau_set_full_node(td_u8 *node, const ext_vau_surface_list *surface_list, td_u32 subnode_num)
{
    return EXT_SUCCESS;
}

td_s32 uapi_vau_append_node_with_path(td_u8 *cur_node, void *pre_node,
    const ext_vau_draw_attr *draw, const ext_vau_matrix *m)
{
    return EXT_SUCCESS;
}

void uapi_vau_unset_node(td_u8 *node)
{
}

td_s32 uapi_vau_set_node_nxt_subnode_mask(td_u8 *node, next_subnode_mask mask)
{
    return EXT_SUCCESS;
}

td_s32 uapi_vau_set_node_subnode_num(td_u8 *node, td_u32 subnode_num)
{
    return EXT_SUCCESS;
}

td_s32 uapi_vau_set_node_id(td_u8 *node, td_u32 node_id)
{
    return EXT_SUCCESS;
}

td_s32 uapi_vau_set_node_buffer_addr(td_u8 *node, const td_u32 phy_addr)
{
    return EXT_SUCCESS;
}

td_s32 uapi_vau_set_node_buffer(td_u8 *node, const ext_vau_surface *dst)
{
    return EXT_SUCCESS;
}

td_s32 uapi_vau_set_node_rect(td_u8 *node, const ext_rect *rect)
{
    return EXT_SUCCESS;
}

td_s32 uapi_vau_set_node_matrix(td_u8 *node, const ext_vau_matrix *m)
{
    return EXT_SUCCESS;
}

td_s32 uapi_vau_set_node_stroke_rnd(td_u8 *node, const ext_vau_matrix *m)
{
    return EXT_SUCCESS;
}

td_s32 uapi_vau_set_node_translate(td_u8 *node, td_s16 offsetX, td_s16 offsetY)
{
    return EXT_SUCCESS;
}

td_s32 uapi_vau_set_node_opa(td_u8 *node, td_u8 opa)
{
    return EXT_SUCCESS;
}

void uapi_vau_wait_for_subnode_done(td_u32 subnode_num)
{
    return;
}

td_s32 uapi_vau_render_offline_nodes(td_u8 *start_node, bool sync)
{
    return EXT_SUCCESS;
}