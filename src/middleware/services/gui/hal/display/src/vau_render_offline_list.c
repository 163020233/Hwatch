/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: the function interface define in vector accelerate unit api source file
 * Create: 2025-7-28
 */

#include "vau_render_offline_list.h"
#include <math.h>
#include <linux/delay.h>
#include "soc_vau_api.h"
#include "vau_hal.h"
#include "vau_hal_node.h"
#include "securec.h"
#include "unistd.h"
#include "rm_math.h"
#include "vau_list.h"

/* HAL_VAU_REG_DRAW_MASK_xx_4 */
#define EXT_ATTR_STROKE_BASE      0x80020000
#define EXT_ATTR_STROKE_MATRIX    0x1FF80000

/* HAL_VAU_REG_DRAW_MASK_xx_1 */
#define BASE_ATTR_TYPICAL_COLOR   0x00010000
/* HAL_VAU_REG_DRAW_MASK_xx_3 */
#define BASE_ATTR_NODE_CFG        0x00040000
#define BASE_ATTR_PATH            0x000000F8

/* HAL_VAU_REG_DRAW_MASK_xx_4 */
#define ALTER_ATTR_DRAW_CTRL      0x00000001
#define ALTER_ATTR_MATRIX         0x00003F00

bool uapi_vau_is_off_list_support(void)
{
#ifdef GPU_3322
    return true;
#else
    return false;
#endif
}

td_s16 uapi_vau_get_node_size(void)
{
#ifdef GPU_3322
    return 640; //  640, register size
#else
    return -1;
#endif
}

td_s32 uapi_vau_set_full_node(td_u8 *node, const ext_vau_surface_list *surface_list, td_u32 subnode_num)
{
    memset_s(node, sizeof(vau_hardware_node), 0, sizeof(vau_hardware_node));
    uapi_vau_get_render_subnode((td_u32)(uintptr_t)node, true, surface_list);

    ext_vau_surface *src2 = (surface_list->src_surface_cnt == 1) ?
                             &surface_list->src_surface[0] : &surface_list->src_surface[1];
    if (src2->matrix != NULL) {
        uapi_vau_set_node_stroke_rnd(node, surface_list->src_surface[1].matrix);
    }

    vau_hardware_node *hw_node = (vau_hardware_node *)node;
    hw_node->vau_node_cfg.bits.list_mode        = 0x1; // 离线链表
    hw_node->vau_node_cfg.bits.subnode_num      = subnode_num;
    hw_node->vau_node_cfg.bits.nxt_subnode_mask = 0;
    return EXT_SUCCESS;
}

td_s32 uapi_vau_append_node_with_path(td_u8 *cur_node, void *pre_node,
    const ext_vau_draw_attr *draw, const ext_vau_matrix *m)
{
    memset_s(cur_node, sizeof(vau_hardware_node), 0, sizeof(vau_hardware_node));
    vau_hardware_node *hw_node = (vau_hardware_node *)cur_node;
    vau_hardware_node *pre_hw_node = (vau_hardware_node *)pre_node;
    hw_node->vau_node_cfg.u32 = pre_hw_node->vau_node_cfg.u32;
    hw_node->vau_node_cfg.bits.nxt_subnode_mask = 0;
    hw_node->mix.draw.vau_draw_ctrl.bits.dyn_bdbx_en = 1;
    hw_node->mix.draw.vau_draw_ctrl.bits.bdbx_adj_en = 1;
    ext_vau_surface_list surface_list = {0};
    ext_vau_surface src = {0};
    surface_list.src_surface_cnt = 1;
    src.draw = draw;
    src.matrix = m;
    surface_list.src_surface = &src;
    return uapi_vau_get_render_subnode((td_u32)(uintptr_t)cur_node, false, &surface_list);
}

void uapi_vau_unset_node(td_u8 *node)
{
    vau_hal_free_vector_mem((vau_hardware_node *)node);
}

td_s32 uapi_vau_set_node_nxt_subnode_mask(td_u8 *node, next_subnode_mask mask)
{
    vau_hardware_node *hw_node = (vau_hardware_node *)node;
    hw_node->vau_node_cfg.bits.nxt_subnode_mask  = mask;
    return EXT_SUCCESS;
}

td_s32 uapi_vau_set_node_subnode_num(td_u8 *node, td_u32 subnode_num)
{
    vau_hardware_node *hw_node = (vau_hardware_node *)node;
    hw_node->vau_node_cfg.bits.subnode_num = subnode_num;
    return EXT_SUCCESS;
}

td_s32 uapi_vau_set_node_id(td_u8 *node, td_u32 node_id)
{
    vau_hardware_node *hw_node = (vau_hardware_node *)node;
    hw_node->vau_node_id.u32 = node_id;
    return EXT_SUCCESS;
}

td_s32 uapi_vau_set_node_buffer_addr(td_u8 *node, const td_u32 phy_addr)
{
    vau_hardware_node *hw_node = (vau_hardware_node *)node;
    hw_node->vau_src1_addr.bits.src1_addr = phy_addr;
    hw_node->vau_des_addr.u32 = phy_addr;

    return EXT_SUCCESS;
}

td_s32 uapi_vau_set_node_rect(td_u8 *node, const ext_rect *rect)
{
    vau_hardware_node *hw_node = (vau_hardware_node *)node;

    hw_node->vau_src1_rect_xy.bits.src1_rect_x      = (td_u16)rect->x;
    hw_node->vau_src1_rect_xy.bits.src1_rect_y      = (td_u16)rect->y;
    hw_node->vau_src1_rect_wh.bits.src1_rect_width  = rect->width - 1;
    hw_node->vau_src1_rect_wh.bits.src1_rect_height = rect->height - 1;

    hw_node->vau_des_rect_xy.bits.des_rect_x = (td_u32)rect->x;
    hw_node->vau_des_rect_xy.bits.des_rect_y = (td_u32)rect->y;
    hw_node->vau_des_rect_wh.bits.des_rect_width = rect->width - 1;
    hw_node->vau_des_rect_wh.bits.des_rect_height = rect->height - 1;

    hw_node->mix.draw.vau_draw_st.bits.draw_x_start = (td_u32)rect->x;
    hw_node->mix.draw.vau_draw_st.bits.draw_y_start = (td_u32)rect->y;
    hw_node->mix.draw.vau_draw_end.bits.draw_x_end = rect->x + rect->width - 1;
    hw_node->mix.draw.vau_draw_end.bits.draw_y_end = rect->y + rect->height - 1;

    return EXT_SUCCESS;
}

td_s32 uapi_vau_set_node_buffer(td_u8 *node, const ext_vau_surface *dst)
{
    vau_hardware_node *hw_node = (vau_hardware_node *)node;
    hw_node->vau_src1_addr.bits.src1_addr           = dst->phy_addr;
    hw_node->vau_src1_stride.bits.src1_stride       = dst->stride;
    hw_node->vau_src1_imgsize.bits.src1_width       = dst->width - 1;
    hw_node->vau_src1_imgsize.bits.src1_height      = dst->height - 1;
    hw_node->vau_src1_rect_xy.bits.src1_rect_x      = (td_u16)dst->rect.x;
    hw_node->vau_src1_rect_xy.bits.src1_rect_y      = (td_u16)dst->rect.y;
    hw_node->vau_src1_rect_wh.bits.src1_rect_width  = dst->rect.width - 1;
    hw_node->vau_src1_rect_wh.bits.src1_rect_height = dst->rect.height - 1;

    hw_node->vau_des_imgsize.bits.des_width = dst->width - 1;
    hw_node->vau_des_imgsize.bits.des_height = dst->height - 1;
    hw_node->vau_des_stride.bits.des_stride = dst->stride;
    hw_node->vau_des_addr.u32 = dst->phy_addr;

    hw_node->vau_des_rect_xy.bits.des_rect_x = (td_u32)dst->rect.x;
    hw_node->vau_des_rect_xy.bits.des_rect_y = (td_u32)dst->rect.y;
    hw_node->vau_des_rect_wh.bits.des_rect_width = dst->rect.width - 1;
    hw_node->vau_des_rect_wh.bits.des_rect_height = dst->rect.height - 1;

    hw_node->mix.draw.vau_draw_st.bits.draw_x_start = (td_u32)dst->rect.x;
    hw_node->mix.draw.vau_draw_st.bits.draw_y_start = (td_u32)dst->rect.y;
    hw_node->mix.draw.vau_draw_end.bits.draw_x_end = dst->rect.x + dst->rect.width - 1;
    hw_node->mix.draw.vau_draw_end.bits.draw_y_end = dst->rect.y + dst->rect.height - 1;

    return EXT_SUCCESS;
}

td_s32 uapi_vau_set_node_matrix(td_u8 *node, const ext_vau_matrix *m)
{
    vau_hardware_node *hw_node = (vau_hardware_node *)node;

    hw_node->mix.draw.vau_draw_matrix_00.bits.matrix_00 = m->matrix[0][0];
    hw_node->mix.draw.vau_draw_matrix_01.bits.matrix_01 = m->matrix[0][1];
    hw_node->mix.draw.vau_draw_matrix_02.bits.matrix_02 = m->matrix[0][2]; /* 0: row, 2: col */
    hw_node->mix.draw.vau_draw_matrix_10.bits.matrix_10 = m->matrix[1][0];
    hw_node->mix.draw.vau_draw_matrix_11.bits.matrix_11 = m->matrix[1][1];
    hw_node->mix.draw.vau_draw_matrix_12.bits.matrix_12 = m->matrix[1][2]; /* 1: row, 2: col */
#ifdef GPU_3322
    hw_node->mix.draw.vau_draw_matrix_20.bits.matrix_20 = m->matrix[2][0]; /* 2: row, 0: col */
    hw_node->mix.draw.vau_draw_matrix_21.bits.matrix_21 = m->matrix[2][1]; /* 2: row, 1: col */
    hw_node->mix.draw.vau_draw_matrix_22.bits.matrix_22 = m->matrix[2][2]; /* 2: row, 2: col */
#endif

    return EXT_SUCCESS;
}

td_s32 uapi_vau_set_node_stroke_rnd(td_u8 *node, const ext_vau_matrix *m)
{
#ifndef GPU_SUPPORT_RM_STROKE
    const td_float tes_angle = (5.0f * 3.1415926 / 180.0f);

    td_float a11 = m->matrix[0][0];
    td_float a12 = m->matrix[0][1];
    td_float a13 = m->matrix[0][2];
    td_float a21 = m->matrix[1][0];
    td_float a22 = m->matrix[1][1];
    td_float a23 = m->matrix[1][2];

    td_float a_determinant = (a11 * a22 - a12 * a21);
    if (vau_math_float_is_zero(a_determinant) == TD_TRUE) {
        return EXT_FAILURE;
    }
    td_float inv_a_determinant = 1.0f / a_determinant;
    td_float cos_value = cosf(-tes_angle);
    td_float sin_value = sinf(-tes_angle);
    td_float b11 = cos_value;
    td_float b12 = -sin_value;
    td_float b21 = sin_value;
    td_float b22 = cos_value;
    td_float c11 = a22 * inv_a_determinant;
    td_float c12 = -a12 * inv_a_determinant;
    td_float c13 = (a12 * a23 - a13 * a22) * inv_a_determinant;
    td_float c21 = -a21 * inv_a_determinant;
    td_float c22 = a11 * inv_a_determinant;
    td_float c23 = (a13 * a21 - a11 * a23) * inv_a_determinant;
    td_float t1  = (b11 * c11 + b12 * c21);
    td_float t2  = (b21 * c11 + b22 * c21);
    td_float t3  = (b11 * c12 + b12 * c22);
    td_float t4  = (b21 * c12 + b22 * c22);
    td_float t5  = (b11 * c13 + b12 * c23);
    td_float t6  = (b21 * c13 + b22 * c23);

    vau_hardware_node *hw_node = (vau_hardware_node *)node;
    hw_node->mix.draw.vau_draw_rnd_m11.bits.rnd_m11 = (a11 * t1 + a12 * t2);
    hw_node->mix.draw.vau_draw_rnd_m12.bits.rnd_m12 = (a11 * t3 + a12 * t4);
    hw_node->mix.draw.vau_draw_rnd_m21.bits.rnd_m21 = (a21 * t1 + a22 * t2);
    hw_node->mix.draw.vau_draw_rnd_m22.bits.rnd_m22 = (a21 * t3 + a22 * t4);
    hw_node->mix.draw.vau_draw_rnd_d1.bits.rnd_d1   = (a11 * t5 + a12 * t6 + a13);
    hw_node->mix.draw.vau_draw_rnd_d2.bits.rnd_d2   = (a21 * t5 + a22 * t6 + a23);
    hw_node->mix.draw.vau_draw_rnd_e11.bits.rnd_e11 = (-a11 * cos_value - a12 * sin_value + a11);
    hw_node->mix.draw.vau_draw_rnd_e12.bits.rnd_e12 = (a11 * sin_value - a12 * cos_value + a12);
    hw_node->mix.draw.vau_draw_rnd_e21.bits.rnd_e21 = (-a21 * cos_value - a22 * sin_value + a21);
    hw_node->mix.draw.vau_draw_rnd_e22.bits.rnd_e22 = (a21 * sin_value - a22 * cos_value + a22);
#endif
    return EXT_SUCCESS;
}

td_s32 uapi_vau_set_node_translate(td_u8 *node, td_s16 offsetX, td_s16 offsetY)
{
    vau_hardware_node *hw_node = (vau_hardware_node *)node;

    hw_node->mix.draw.vau_draw_matrix_02.bits.matrix_02 += offsetX; /* 0: row, 2: col */
#ifdef GPU_3322
    hw_node->mix.draw.vau_draw_matrix_12.bits.matrix_12 += offsetY; /* 2: row, 2: col */
#endif

    return EXT_SUCCESS;
}

td_s32 uapi_vau_set_node_opa(td_u8 *node, td_u8 opa)
{
    vau_hardware_node *hw_node = (vau_hardware_node *)node;

    hw_node->vau_gradient_typical_color.u32 =
        (hw_node->vau_gradient_typical_color.u32 & 0x00FFFFFF) | opa << 24; // 24:AGRB ALPHA SHIFT

    return EXT_SUCCESS;
}

td_s32 uapi_vau_render_offline_nodes(td_u8 *start_node, bool sync)
{
    vau_list_set_offline_finish_done(TD_FALSE);
    vau_hardware_node *hw_node = (vau_hardware_node *)start_node;
    uapi_reg_write(HAL_VAU_REG_OFF_NODE_CFG, 0x0501);

    uapi_reg_write(HAL_VAU_REG_DRAW_MASK_00_1, BASE_ATTR_TYPICAL_COLOR);
    uapi_reg_write(HAL_VAU_REG_DRAW_MASK_00_3, BASE_ATTR_NODE_CFG);
    uapi_reg_write(HAL_VAU_REG_DRAW_MASK_00_4, BASE_ATTR_PATH | ALTER_ATTR_DRAW_CTRL | EXT_ATTR_STROKE_BASE);
    uapi_reg_write(HAL_VAU_REG_DRAW_MASK_01_1, BASE_ATTR_TYPICAL_COLOR);
    uapi_reg_write(HAL_VAU_REG_DRAW_MASK_01_3, BASE_ATTR_NODE_CFG);
    uapi_reg_write(HAL_VAU_REG_DRAW_MASK_01_4, BASE_ATTR_PATH | ALTER_ATTR_DRAW_CTRL | EXT_ATTR_STROKE_BASE |
        ALTER_ATTR_MATRIX | EXT_ATTR_STROKE_MATRIX);

    // apb p_next and start
    uapi_reg_write(HAL_VAU_REG_PNEXT, (td_u32)(uintptr_t)hw_node);
    uapi_reg_write(HAL_VAU_REG_START, 0x1);

    if (sync) {
        uapi_vau_wait_all_done();
    }
    return EXT_SUCCESS;
}