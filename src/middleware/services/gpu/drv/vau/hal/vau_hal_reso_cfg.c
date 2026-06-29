/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: vau_hal_reso_cfg file
 */

#include "vau_hal_reso_cfg.h"
#include "vau_dfx.h"

#ifdef GPU_RESO_CFG_SUPPORT
STATIC td_u32 vau_hal_get_tran2_tile(drv_vau_surface *sur)
{
    td_u32 tile_value = 0;

    if ((sur->compress_info.compress_mode == DRV_GFX_COMPRESS_HFBC_2X) ||
        (sur->compress_info.compress_mode == DRV_GFX_COMPRESS_HFBC_2X) ||
        (sur->mapping_attr.mapping_en == TD_TRUE)) {
        tile_value = 0;
    } else if (sur->compress_info.compress_mode == DRV_GFX_COMPRESS_HIMC) {
        if (sur->compress_info.tile_width == 4) {          /* 4x4 tile */
            tile_value = 0; /* mode 0 */
        } else if (sur->compress_info.tile_width == 6) {   /* 6x4 tile */
            tile_value = 1; /* mode 1 */
        } else if (sur->compress_info.tile_width == 8) {   /* 8x4 tile */
            tile_value = 2; /* mode 2 */
        } else if (sur->compress_info.tile_width == 16) {  /* 16x4 tile */
            tile_value = 3; /* mode 3 */
        }
    }

    return tile_value;
}

STATIC td_u32 vau_hal_get_algin_value(drv_vau_surface *sur)
{
    td_u32 align_value;

    switch (sur->compress_info.compress_mode) {
        case DRV_GFX_COMPRESS_NONE:
            align_value = 1;
            break;
        case DRV_GFX_COMPRESS_HIMC:
            align_value = sur->compress_info.tile_width;
            break;
        case DRV_GFX_COMPRESS_HFBC_2X:
        case DRV_GFX_COMPRESS_HFBC_3X:
            align_value = 4; /* 4: 4x4 tile */
            break;
        default:
            align_value = 1;
            break;
    }

    if (align_value == 6) { /* 6x4:tile */
        align_value = 12;   /* support input 6x4 tile output:4x4 tile. 12:align */
    }

    return align_value;
}

STATIC td_void vau_hal_get_align_rect(const ext_rect *cur_rect, ext_rect *align_rect, td_u32 align_value)
{
    if (uapi_is_byte_align(cur_rect->x, align_value) != TD_TRUE) {
        align_rect->x = (cur_rect->x / align_value) * align_value; /* down align */
    }

    align_rect->width += (cur_rect->x - align_rect->x);
    align_rect->width = uapi_byte_align(align_rect->width, align_value);

    if (uapi_is_byte_align(cur_rect->y, align_value) != TD_TRUE) {
        align_rect->y = (cur_rect->y / align_value) * align_value; /* down align */
    }

    align_rect->height += (cur_rect->y - align_rect->y);
    align_rect->height = uapi_byte_align(align_rect->height, align_value);

    return;
}
#endif

#ifdef GPU_RESO_CFG_SUPPORT
STATIC td_void vau_hal_update_src2_add_pix_info(vau_hardware_node *hw_node, drv_vau_surface *src1,
    drv_vau_surface *src2, ext_rect *src1_algin_rect)
{
    // 配置ADD_PIX
    hw_node->vau_src2_add_pix_ctrl.bits.src2_add_pix_en = 1;
    hw_node->vau_src2_add_pix_ctrl.bits.src2_add_pix_cfg_en = 1;
    hw_node->vau_src2_add_pix_small.bits.src2_add_pix_small_width  = src2->rect.width - 1;
    hw_node->vau_src2_add_pix_small.bits.src2_add_pix_small_height = src2->rect.height - 1;
    hw_node->vau_src2_add_pix_big.bits.src2_add_pix_big_width  = src1_algin_rect->width - 1;
    hw_node->vau_src2_add_pix_big.bits.src2_add_pix_big_height = src1_algin_rect->height - 1;
    hw_node->vau_src2_add_pix_rect.bits.src2_add_pix_rect_x = src1->rect.x - src1_algin_rect->x;
    hw_node->vau_src2_add_pix_rect.bits.src2_add_pix_rect_y = src1->rect.y - src1_algin_rect->y;
    hw_node->vau_src2_add_pix_tile.bits.src2_add_pix_tile_width  = 0;
    hw_node->vau_src2_add_pix_tile.bits.src2_add_pix_tile_height = 0;

    return;
}

STATIC td_void vau_hal_update_t2r0_ctrl_info(vau_hardware_node *hw_node, ext_rect *src1_algin_rect)
{
        // 配置R2T
    hw_node->vau_t2r0_ctrl.bits.t2r0_en       = 1;
    hw_node->vau_t2r0_ctrl.bits.t2r0_ck_gt_en = 1;
    hw_node->vau_t2r0_ctrl.bits.t2r0_cfg_en   = 1;
    hw_node->vau_t2r0_ctrl.bits.t2r0_mode_sel = 1;
    hw_node->vau_t2r0_ctrl.bits.t2r0_tile_wth = 4; // 4:tile_wth
    hw_node->vau_t2r0_wh.bits.t2r0_width  = src1_algin_rect->width  - 1;
    hw_node->vau_t2r0_wh.bits.t2r0_height = src1_algin_rect->height - 1;

    return;
}

STATIC td_void vau_hal_update_mapping_crop_info(vau_hardware_node *hw_node, drv_vau_surface *src2,
    ext_rect *src1_algin_rect)
{
    // 配置mapping输入与输出分辨率
    hw_node->mapping_src_crop_xy.bits.mapping_src_crop_x = 0;
    hw_node->mapping_src_crop_xy.bits.mapping_src_crop_y = 0;
    hw_node->mapping_src_crop_wh.bits.mapping_src_crop_w = src2->width;
    hw_node->mapping_src_crop_wh.bits.mapping_src_crop_h = src2->height;
    hw_node->vau_mapping_crop_xy.bits.mapping_crop_x = src1_algin_rect->x;
    hw_node->vau_mapping_crop_xy.bits.mapping_crop_y = src1_algin_rect->y;
    hw_node->vau_mapping_crop_wh.bits.mapping_crop_w = src1_algin_rect->width;
    hw_node->vau_mapping_crop_wh.bits.mapping_crop_h = src1_algin_rect->height;

    return;
}

STATIC td_void vau_hal_update_trans_info(vau_hardware_node *hw_node, drv_vau_surface *src1,
    drv_vau_surface *src2, ext_rect *src1_algin_rect, ext_rect *src2_algin_rect)
{
    // 配置trans
    hw_node->vau_trans_ctrl.bits.tile_trans_en    = 1;
    hw_node->vau_trans_ctrl.bits.trans_ck_gt_en   = 1;
    hw_node->vau_trans_ctrl.bits.trans_cfg_en     = 1;
    hw_node->vau_trans_ctrl.bits.trans_fill_mode  = 0;
    hw_node->vau_trans_ctrl.bits.trans1_tile_size = 0;
    hw_node->vau_trans_ctrl.bits.trans2_tile_size = vau_hal_get_tran2_tile(src2);

    hw_node->vau_trans_src1_pos.bits.trans1_xfpos   = src1->rect.x - src1_algin_rect->x;
    hw_node->vau_trans_src1_pos.bits.trans1_yfpos   = src1->rect.y - src1_algin_rect->y;
    hw_node->vau_trans_src1_reso.bits.trans1_width  = src1_algin_rect->width  - 1;
    hw_node->vau_trans_src1_reso.bits.trans1_height = src1_algin_rect->height - 1;

    hw_node->vau_trans_src2_reso.bits.trans2_width  = src2_algin_rect->width  - 1;
    hw_node->vau_trans_src2_reso.bits.trans2_height = src2_algin_rect->height - 1;

    hw_node->vau_trans_src2_crop_xy.bits.trans2_crop_x = src2->rect.x - src2_algin_rect->x;
    hw_node->vau_trans_src2_crop_xy.bits.trans2_crop_y = src2->rect.y - src2_algin_rect->y;
    hw_node->vau_trans_src2_crop_wh.bits.trans2_crop_w = src2->rect.width - 1;
    hw_node->vau_trans_src2_crop_wh.bits.trans2_crop_h = src2->rect.width - 1;
    
    return;
}

STATIC td_void vau_hal_update_reso_cbm_info(vau_hardware_node *hw_node, ext_rect *src1_algin_rect)
{
    // 配置CBM
    hw_node->vau_reso_cbm.bits.cbm_width  = src1_algin_rect->width  - 1;
    hw_node->vau_reso_cbm.bits.cbm_height = src1_algin_rect->height - 1;
    
    return;
}
#endif

td_void vau_hal_software_reso_cfg(vau_hardware_node *hw_node, drv_vau_surface_list *surface_list)
{
#ifdef GPU_RESO_CFG_SUPPORT
    drv_vau_surface *src1 = TD_NULL;
    drv_vau_surface *src2 = TD_NULL;

    uapi_reg_write(HAL_VAU_REG_BASEADDR + 0x538, 0x690);

    if ((surface_list->src_surface_cnt != 2) || /* 2:index */
        (surface_list->src_surface[0].phy_addr != surface_list->dst_surface->phy_addr) ||
        (surface_list->src_surface[0].compress_info.compress_mode == DRV_GFX_COMPRESS_NONE)) {
        return;
    }

    // 软件计算分辨率使能
    hw_node->vau_node_cfg.bits.reso_cfg_en = 1;

    src1 = &(surface_list->src_surface[0]);
    src2 = &(surface_list->src_surface[1]);

    td_u32 src1_align_value = vau_hal_get_algin_value(src1);
    td_u32 src2_align_value = vau_hal_get_algin_value(src2);

    ext_rect src1_algin_rect = src1->rect;
    ext_rect src2_algin_rect = src2->rect;
    vau_hal_get_align_rect(&src1->rect, &src1_algin_rect, src1_align_value);
    vau_hal_get_align_rect(&src2->rect, &src2_algin_rect, src2_align_value);

    if (src2_align_value == 1) {
        // 配置ADD_PIX
        vau_hal_update_src2_add_pix_info(hw_node, src1, src2, &src1_algin_rect);

        // 配置R2T使能
        if (src2->attr.vector.enable == TD_TRUE) {
            hw_node->vau_des_ctrl.bits.draw_t2r_bind_en = 1;
        } else {
            hw_node->vau_src2_ctrl.bits.src2_t2r_bind_en = 1;
        }
        // 配置R2T
        vau_hal_update_t2r0_ctrl_info(hw_node, &src1_algin_rect);

        // 配置CBM
        vau_hal_update_reso_cbm_info(hw_node, &src1_algin_rect);
    } else {
        // 配置mapping输入与输出分辨率
        if (src2->mapping_attr.mapping_en) {
            vau_hal_update_mapping_crop_info(hw_node, src2, &src1_algin_rect);
        }

        // 配置trans
        vau_hal_update_trans_info(hw_node, src1, src2, &src1_algin_rect, &src2_algin_rect);

        // 配置CBM
        vau_hal_update_reso_cbm_info(hw_node, &src1_algin_rect);

        src2->rect = src2_algin_rect;
    }

    src1->rect = src1_algin_rect;
    surface_list->dst_surface->rect = src1_algin_rect;
#else
    uapi_unused(hw_node);
    uapi_unused(surface_list);
#endif

    return;
}
