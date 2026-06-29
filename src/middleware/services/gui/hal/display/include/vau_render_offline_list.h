/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description:  Provides vau interface api
 * Author: Hisi Graphic Team
 */

#ifndef VAU_RENDER_OFFLINE_LIST_H
#define VAU_RENDER_OFFLINE_LIST_H
#include "soc_vau_type.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    LOAD_REG_MODE_BASE = 0x0,
    LOAD_REG_MODE_ALTER = 0x1,
    LOAD_REG_MODE_GRADIENT = 0x2,
    LOAD_REG_MODE_FULL = 0x3,
} next_subnode_mask;

/*
检查是否支持离线链表模式
*/
bool uapi_vau_is_off_list_support(void);

/*
获取单个寄存器节点大小
*/
td_s16 uapi_vau_get_node_size(void);

/*
填充一个全量的寄存器节点
*/
td_s32 uapi_vau_set_full_node(td_u8 *node, const ext_vau_surface_list *surface_list, td_u32 subnode_num);

/*
增加一个寄存器节点，并填充其中的path相关参数
*/
td_s32 uapi_vau_append_node_with_path(td_u8 *cur_node, void *pre_node,
    const ext_vau_draw_attr *draw, const ext_vau_matrix *m);

/*
释放驱动内部分配的内存
*/
void uapi_vau_unset_node(td_u8 *node);

/*
修改寄存器节点中的下一个节点的寄存器加载模式
*/
td_s32 uapi_vau_set_node_nxt_subnode_mask(td_u8 *node, next_subnode_mask mask);

/*
修改寄存器节点的子节点数量
*/
td_s32 uapi_vau_set_node_subnode_num(td_u8 *node, td_u32 subnode_num);

/*
修改寄存器节点中的id
*/
td_s32 uapi_vau_set_node_id(td_u8 *node, td_u32 node_id);

/*
修改寄存器节点的目标buffer，只更新地址，仅全量寄存器涉及
*/
td_s32 uapi_vau_set_node_buffer_addr(td_u8 *node, const td_u32 phy_addr);

/*
修改寄存器节点的绘制区域，仅全量寄存器涉及
*/
td_s32 uapi_vau_set_node_rect(td_u8 *node, const ext_rect *rect);

/*
修改寄存器节点的目标buffer，仅全量寄存器涉及
*/
td_s32 uapi_vau_set_node_buffer(td_u8 *node, const ext_vau_surface *dst);

/*
修改寄存器节点的变换矩阵
*/
td_s32 uapi_vau_set_node_matrix(td_u8 *node, const ext_vau_matrix *m);

/*
修改寄存器节点的stroke的圆角处理相关参数
*/
td_s32 uapi_vau_set_node_stroke_rnd(td_u8 *node, const ext_vau_matrix *m);

/*
修改寄存器节点的平移
*/
td_s32 uapi_vau_set_node_translate(td_u8 *node, td_s16 offsetX, td_s16 offsetY);

/*
修改寄存器节点的透明度信息
*/
td_s32 uapi_vau_set_node_opa(td_u8 *node, td_u8 opa);

/*
启动离线链表绘制
*/
td_s32 uapi_vau_render_offline_nodes(td_u8 *start_node, bool sync);

#ifdef __cplusplus
}
#endif
#endif
