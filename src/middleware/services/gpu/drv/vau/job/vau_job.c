/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: vau job
 */

#include "vau_job.h"
#include "gpu_feature_config.h"
#include "vau_dfx.h"
#include "vau_list.h"
#include "vau_handle.h"
#include "vau_hal.h"
#include "vau_hal_capability.h"
#if (GPU_LOW_POWER_SUPPORT == YES)
#if TARGET_CHIP_BRANDY
#include "pm.h"
#else
#include "pm_veto_porting.h"
#endif
#include "pm_veto.h"
#endif
#include "vau_hal_reso_cfg.h"
#include "vau_debug.h"

STATIC td_void vau_job_revise_src_rect(drv_vau_surface *src_sur, ext_rect *refer_region)
{
    if ((src_sur->attr.affine.affine_en == TD_TRUE) ||
        (src_sur->attr.vector.enable == TD_TRUE) ||
        (src_sur->threed_attr.enable)) {
        return; /* affine / vector / 3D / zme, not need revise */
    }

    if (src_sur->fmt >= DRV_GFX_FMT_YUV400SP && src_sur->fmt <= DRV_GFX_FMT_YUV444SP) {
        return;
    }

    gpu_dbg("src_rect_before:[%d, %d, %u, %u]",
            src_sur->rect.x, src_sur->rect.y, src_sur->rect.width, src_sur->rect.height);

    src_sur->rect.x = src_sur->rect.x + refer_region->x;
    src_sur->rect.y = src_sur->rect.y + refer_region->y;
    src_sur->rect.width  = refer_region->width;
    src_sur->rect.height = refer_region->height;

    gpu_dbg("src_rect_after:[%d, %d, %u, %u]",
            src_sur->rect.x, src_sur->rect.y, src_sur->rect.width, src_sur->rect.height);

    return;
}

STATIC td_void vau_job_revise_dst_rect(drv_vau_surface *dst_sur, ext_rect *refer_region)
{
    td_s32 surface_w, surface_h;
    td_s32 rect_x, rect_y, rect_w, rect_h;

    rect_x = dst_sur->rect.x;
    rect_y = dst_sur->rect.y;
    rect_w = (td_s32)dst_sur->rect.width;  /* for cmpare u32 to s32 */
    rect_h = (td_s32)dst_sur->rect.height; /* for cmpare u32 to s32 */

    gpu_dbg("dst_rect_before:[%d, %d, %u, %u]",
            dst_sur->rect.x, dst_sur->rect.y, dst_sur->rect.width, dst_sur->rect.height);

    surface_w = (td_s32)dst_sur->width;    /* for cmpare u32 to s32 */
    surface_h = (td_s32)dst_sur->height;   /* for cmpare u32 to s32 */

    if (rect_x < 0) {
        rect_w = rect_x + rect_w;
        rect_x = 0;
    }

    if (rect_y < 0) {
        rect_h = rect_y + rect_h;
        rect_y = 0;
    }

    if (rect_x + rect_w > surface_w) {
        rect_w = surface_w - rect_x;
    }

    if (rect_y + rect_h > surface_h) {
        rect_h = surface_h - rect_y;
    }

    /* refer_region decide how to revise src rect */
    refer_region->x = rect_x - dst_sur->rect.x; /* x offset */
    refer_region->y = rect_y - dst_sur->rect.y; /* y offset */
    refer_region->width  = (td_u32)rect_w;              /* revise w */
    refer_region->height = (td_u32)rect_h;              /* revise h */

    dst_sur->rect.x = rect_x;
    dst_sur->rect.y = rect_y;
    dst_sur->rect.width  = (td_u32)rect_w;
    dst_sur->rect.height = (td_u32)rect_h;

    gpu_dbg("dst_rect_after:[%d, %d, %u, %u]",
            dst_sur->rect.x, dst_sur->rect.y, dst_sur->rect.width, dst_sur->rect.height);

    return;
}

STATIC td_void vau_job_revise_rect_in_range(drv_vau_surface_list *surface_list)
{
    ext_rect refer_region = {0};

    vau_job_revise_dst_rect(surface_list->dst_surface, &refer_region);
    vau_job_revise_src_rect(&surface_list->src_surface[0], &refer_region);

    if (surface_list->src_surface_cnt == 2) { /* 2: src cnt */
        vau_job_revise_src_rect(&surface_list->src_surface[1], &refer_region);
    }

    return;
}

STATIC td_s32 vau_check_frg_and_dst_surface(const drv_vau_surface *frg_sur,
                                            const drv_vau_surface *dst_sur)
{
    if ((frg_sur->attr.affine.affine_en == TD_TRUE) ||
        (frg_sur->attr.vector.enable == TD_TRUE) ||
        (frg_sur->threed_attr.enable == TD_TRUE)) {
        return EXT_SUCCESS;
    }

    if (frg_sur->fmt >= DRV_GFX_FMT_YUV400SP && frg_sur->fmt <= DRV_GFX_FMT_YUV444SP) {
        return EXT_SUCCESS;
    }

    if ((frg_sur->rect.width != dst_sur->rect.width) ||
        (frg_sur->rect.height != dst_sur->rect.height)) {
        gpu_err("foreground and dst rect_wh not equal, frg_wh[%u,%u], dst_wh[%u,%u]!",
                frg_sur->rect.width, frg_sur->rect.height,
                dst_sur->rect.width, dst_sur->rect.height);
        return DRV_ERR_VAU_INVLIAD_RECT;
    }

    return EXT_SUCCESS;
}

STATIC td_s32 vau_check_bkg_and_dst_surface(const drv_vau_surface *bkg_sur,
                                            const drv_vau_surface *dst_sur)
{
    td_bool is_same_buf = (bkg_sur->phy_addr == dst_sur->phy_addr) ? TD_TRUE : TD_FALSE;

    if (is_same_buf) {
        if ((bkg_sur->fmt != dst_sur->fmt) ||
            (memcmp(&bkg_sur->rect, &dst_sur->rect, sizeof(ext_rect)) != 0) ||
            (bkg_sur->compress_info.compress_mode != dst_sur->compress_info.compress_mode)) {
            gpu_err("bkg and dst surface not equal!"
                    "bkg:fmt[%d] cmp[%d] rect[%d,%d,%u,%u] dst:fmt[%d] cmp[%d] rect[%d,%d,%u,%u]!",
                    bkg_sur->fmt, bkg_sur->compress_info.compress_mode, bkg_sur->rect.x, bkg_sur->rect.y,
                    bkg_sur->rect.width, bkg_sur->rect.height, dst_sur->fmt, dst_sur->compress_info.compress_mode,
                    dst_sur->rect.x, dst_sur->rect.y, dst_sur->rect.width, dst_sur->rect.height);
            return EXT_FAILURE;
        }
    } else {
        /* if not same buf, src1 xy not support < 0 */
        if ((bkg_sur->rect.x < 0) || (bkg_sur->rect.y < 0) ||
            (bkg_sur->rect.width != dst_sur->rect.width) ||
            (bkg_sur->rect.height != dst_sur->rect.height)) {
            gpu_err("bkg and dst rect_wh not same buf, bkg_xywh[%d,%d,%u,%u], dst_wh[%u,%u]!",
                    bkg_sur->rect.x, bkg_sur->rect.y, bkg_sur->rect.width, bkg_sur->rect.height,
                    dst_sur->rect.width, dst_sur->rect.height);
            return DRV_ERR_VAU_INVLIAD_RECT;
        }
    }

    return EXT_SUCCESS;
}

STATIC td_s32 vau_job_check_every_surface(const drv_vau_surface_list *surface_list)
{
    td_s32 ret;

    if (surface_list->src_surface_cnt == 1) {
        if (surface_list->src_surface[0].is_dma == TD_TRUE) {
            ret = vau_hal_check_src1(&surface_list->src_surface[0]);
        } else {
            ret = vau_hal_check_src2(&surface_list->src_surface[0]);
        }
        if (ret != EXT_SUCCESS) {
            gpu_err("vau check single src failed!");
            return ret;
        }

        /* check foreground and dst */
        ret = vau_check_frg_and_dst_surface(&surface_list->src_surface[0], surface_list->dst_surface);
        if (ret != EXT_SUCCESS) {
            gpu_err("vau_check_frg_and_dst_surface failed!");
            return ret;
        }
    } else { /* src cnt = 2 */
        ret = vau_hal_check_src1(&surface_list->src_surface[0]);
        if (ret != EXT_SUCCESS) {
            gpu_err("call vau_hal_check_src1 failed!");
            return ret;
        }

        ret = vau_hal_check_src2(&surface_list->src_surface[1]);
        if (ret != EXT_SUCCESS) {
            gpu_err("call vau_hal_check_src2 failed!");
            return ret;
        }

        ret = vau_hal_check_blit_opt(surface_list->opt);
        if (ret != EXT_SUCCESS) {
            gpu_err("call vau_hal_check_blit_opt failed!");
            return ret;
        }

        /* check foreground and dst */
        ret = vau_check_frg_and_dst_surface(&surface_list->src_surface[1], surface_list->dst_surface);
        if (ret != EXT_SUCCESS) {
            gpu_err("vau_check_frg_and_dst_surface failed!");
            return ret;
        }

        /* check background and dst */
        ret = vau_check_bkg_and_dst_surface(&surface_list->src_surface[0], surface_list->dst_surface);
        if (ret != EXT_SUCCESS) {
            gpu_err("vau_check_bkg_and_dst_surface failed!");
            return ret;
        }
    }

    ret = vau_hal_check_dst(surface_list->dst_surface);
    if (ret != EXT_SUCCESS) {
        return ret;
    }

    return EXT_SUCCESS;
}

STATIC td_s32 vau_job_check_surface_list(const drv_vau_surface_list *surface_list)
{
    td_s32 ret;

    if ((surface_list == TD_NULL) ||
        (surface_list->src_surface == TD_NULL) || (surface_list->dst_surface == TD_NULL)) {
        gpu_err("surface list or surface is null");
        return EXT_FAILURE;
    }

    /* logic max support 2 layers */
    if ((surface_list->src_surface_cnt != 1) && (surface_list->src_surface_cnt != 2)) {
        gpu_err("surface cnt [%u] is not support", surface_list->src_surface_cnt);
        return EXT_FAILURE;
    }

    ret = vau_job_check_every_surface(surface_list);
    if (ret != EXT_SUCCESS) {
        gpu_err("vau_check_every_surface failed");
        return ret;
    }

    return EXT_SUCCESS;
}

STATIC td_bool vau_job_check_dma2d_onlysrc2mode(const drv_vau_surface_list *surface_list)
{
    if ((surface_list->src_surface[1].compress_info.alpha_value == DRV_GFX_ALPHA_ALL_255) ||
        (surface_list->src_surface[1].compress_info.alpha_value == DRV_GFX_ALPHA_0_255)) {
        return TD_TRUE;
    }

    return TD_FALSE;
}

STATIC td_void vau_job_dma2d_config(vau_hardware_node *node, drv_vau_surface_list *surface_list)
{
    td_bool strb_en = TD_FALSE;
    if (surface_list->opt == TD_NULL) {
        return;
    }
    if ((surface_list->src_surface[1].compress_info.alpha_value == DRV_GFX_ALPHA_ALL_255) ||
        (surface_list->src_surface[1].compress_info.alpha_value == DRV_GFX_ALPHA_0_255)) {
        surface_list->opt->rop_opt.rop_enable = TD_FALSE;
        surface_list->opt->blend_opt.blend_enable = TD_FALSE;
        strb_en = (surface_list->src_surface[1].compress_info.alpha_value == DRV_GFX_ALPHA_0_255) ? TD_TRUE : TD_FALSE;
            vau_hal_set_alpha_strb(node, strb_en);
    } else {
        vau_hal_set_alpha_strb(node, TD_FALSE);
    }

    return;
}

STATIC td_void vau_job_node_config(vau_hardware_node *hw_node, drv_vau_surface_list *surface_list)
{
    if ((surface_list->src_surface[0].is_dma == TD_TRUE) &&
        (surface_list->src_surface[0].is_color_surface == TD_FALSE) &&
        (surface_list->src_surface[0].rect.x == 0) &&
        (surface_list->src_surface[0].rect.y == 0) &&
        (surface_list->src_surface[0].rect.width == surface_list->src_surface[0].width) &&
        (surface_list->src_surface[0].rect.height == surface_list->src_surface[0].height) &&
        (surface_list->dst_surface->rect.x == 0) &&
        (surface_list->dst_surface->rect.y == 0) &&
        (surface_list->dst_surface->rect.width == surface_list->dst_surface->width) &&
        (surface_list->dst_surface->rect.height == surface_list->dst_surface->height)) {
        vau_node_set_fast_dma(hw_node, &surface_list->src_surface[0], surface_list->dst_surface);
        return;
    }

    if (surface_list->src_surface[0].is_dma == TD_TRUE) {
        vau_node_set_src1(hw_node, &surface_list->src_surface[0]);
    } else {
        if (surface_list->src_surface_cnt == 2) { /* 2:double src surface */
            if (vau_job_check_dma2d_onlysrc2mode(surface_list)) {
                vau_job_dma2d_config(hw_node, surface_list);
            } else {
                vau_node_set_src1(hw_node, &surface_list->src_surface[0]);
                vau_node_set_blend_mode(hw_node, surface_list->opt);
            }
            vau_node_set_src2(hw_node, &surface_list->src_surface[1], surface_list->dst_surface);
            vau_node_set_mapping(hw_node, &surface_list->src_surface[1]);
        } else {
            vau_node_set_src2(hw_node, &surface_list->src_surface[0], surface_list->dst_surface);
            vau_node_set_mapping(hw_node, &surface_list->src_surface[0]);
        }
        vau_node_set_gaussian(hw_node, surface_list->opt);
        drv_vau_surface *src2 = (surface_list->src_surface_cnt == 1) ? &surface_list->src_surface[0] :
                                &surface_list->src_surface[1];
        vau_node_set_cbm(hw_node, surface_list->opt, src2->color);
    }

    vau_node_set_dst(hw_node, surface_list->dst_surface);
    return;
}

STATIC td_bool vau_job_foreground_is_zero(drv_vau_surface_list *surface_list)
{
    if ((surface_list->src_surface_cnt == 2) && /* 2: double src */
        (surface_list->src_surface[1].compress_info.alpha_value == DRV_GFX_ALPHA_ALL_0)) {
        if (surface_list->src_surface[0].phy_addr == surface_list->dst_surface->phy_addr) {
            return TD_TRUE; /* no need logic run */
        } else {
            surface_list->src_surface_cnt = 1; /* gon to check dma */
        }
    }

    return TD_FALSE;
}

td_s32 drv_vau_create(td_handle *handle)
{
    td_s32 ret;

    ret = vau_list_create(handle);
    if (ret != EXT_SUCCESS) {
        gpu_err("vau_list_create failed!");
        return ret;
    }

#if (GPU_LOW_POWER_SUPPORT == YES)
    uapi_pm_add_sleep_veto_with_symmetry(PM_ID_VAU);
#endif

    return EXT_SUCCESS;
}

td_s32 drv_vau_destroy(td_handle handle)
{
    td_s32 ret;

    ret = vau_list_destroy(handle);
    if (ret != EXT_SUCCESS) {
        gpu_err("vau_list_destroy failed!");
        return ret;
    }

#if (GPU_LOW_POWER_SUPPORT == YES)
    uapi_pm_remove_sleep_veto_with_symmetry(PM_ID_VAU);
#endif

    return EXT_SUCCESS;
}

td_s32 drv_vau_fill(td_handle handle, drv_vau_surface_list *surface_list)
{
    return drv_vau_blit(handle, surface_list);
}

STATIC td_void vau_check_mesh_mem_set(vau_sw_node *sw_node, drv_vau_surface_list *surface_list)
{
    td_u32 index = surface_list->src_surface_cnt > 1 ? 1 : 0;

    sw_node->is_mesh_mem_set = surface_list->src_surface[index].threed_attr.model_sur.is_mesh_mem_set;
}

STATIC td_s32 drv_vau_blit_node_cfg(td_handle handle, drv_vau_surface_list *surface_list)
{
    vau_sw_node *tmp_sw_node = TD_NULL;
    td_bool draw_flag = vau_node_get_draw_flag();

    if (vau_job_check_surface_list(surface_list) != EXT_SUCCESS) {
        gpu_err("call vau_job_check_surface_list fail!");
        return EXT_FAILURE;
    }

    vau_job_revise_rect_in_range(surface_list);

    if (vau_job_foreground_is_zero(surface_list)) {
        return EXT_SUCCESS;
    }

    if (draw_flag == TD_TRUE) {
        tmp_sw_node = vau_node_create(VAU_MIX_NODE_DRAW);
        if (tmp_sw_node == TD_NULL) {
            gpu_err("vau_node_create tmp_sw_node failed");
            return DRV_ERR_VAU_NO_MEM;
        }

        if (vau_node_add(handle, tmp_sw_node) != EXT_SUCCESS) {
            vau_node_free(tmp_sw_node);
            gpu_err("vau_node_add tmp_sw_node failed");
            return EXT_FAILURE;
        }
    }

    vau_sw_node *sw_node = vau_node_create(VAU_MIX_NODE_MAPP);
    if (sw_node == TD_NULL) {
        gpu_err("call vau_node_create fail!");
        goto out;
    }

    /* software cal reso for supporting fb 1x1 */
    vau_hal_software_reso_cfg((vau_hardware_node *)sw_node->hw_node, surface_list);

    vau_job_node_config((vau_hardware_node *)sw_node->hw_node, surface_list);

    vau_check_mesh_mem_set(sw_node, surface_list);

    if (vau_node_add(handle, sw_node) != EXT_SUCCESS) {
        vau_node_free(sw_node);
        gpu_err("call vau_job_add_node failed");
        goto out;
    }

    if (draw_flag == TD_TRUE) {
        vau_node_set_draw_flag(TD_FALSE);
    }

    return EXT_SUCCESS;

out:
    if (draw_flag == TD_TRUE) {
        vau_node_free(tmp_sw_node);
    }

    return EXT_FAILURE;
}

STATIC td_bool drv_vau_blit_with_split_check(drv_vau_surface_list *surface_list)
{
    td_u32 surface_cnt = surface_list->src_surface_cnt;

    if (surface_cnt == 1) {
        if (surface_list->src_surface[0].attr.affine.affine_en) {
            return TD_TRUE;
        }
    } else {
        if (surface_list->src_surface[0].attr.affine.affine_en ||
            surface_list->src_surface[1].attr.affine.affine_en) {
            return TD_TRUE;
        }
    }

    return TD_FALSE;
}

STATIC td_s32 drv_vau_blit_with_split(td_handle handle, drv_vau_surface_list *surface_list)
{
    td_s32 ret;
    td_u32 surface_cnt = surface_list->src_surface_cnt;
    ext_rect tmp_rect;
    drv_vau_surface *dst_sur = surface_list->dst_surface;
    ext_rect org_dst_rect = dst_sur->rect;
    td_u32 split_cnt = (org_dst_rect.width + DRV_GFX_SPLIT_MAX_WIDTH - 1) / DRV_GFX_SPLIT_MAX_WIDTH;
    td_bool need_split = TD_FALSE;
    td_bool matrix_en = TD_FALSE;

    matrix_en = drv_vau_blit_with_split_check(surface_list);

    for (td_u32 i = 0; i < surface_cnt; i++) {
        drv_vau_surface *tmp_src = &(surface_list->src_surface[i]);
        if (tmp_src->compress_info.compress_mode == DRV_GFX_COMPRESS_HIMC ||
            tmp_src->compress_info.compress_mode == DRV_GFX_COMPRESS_HFBC_2X ||
            tmp_src->compress_info.compress_mode == DRV_GFX_COMPRESS_HFBC_3X ||
            (tmp_src->fmt >= DRV_GFX_FMT_YUV400SP && tmp_src->fmt <= DRV_GFX_FMT_YUV444SP)) {
            need_split = TD_TRUE;
            break;
        }
    }

    if (need_split == TD_FALSE && matrix_en == TD_FALSE) {
        ret = drv_vau_blit_node_cfg(handle, surface_list);
        if (ret != EXT_SUCCESS) {
            gpu_err("call drv_vau_blit_node_cfg failed!");
            return EXT_FAILURE;
        }
        return EXT_SUCCESS;
    }

    if (surface_cnt == 1) {
        drv_vau_surface *src_sur0 = &(surface_list->src_surface[0]);
        ext_rect org_rect0 = src_sur0->rect;

        for (td_u32 i = 0; i < split_cnt; i++) {
            if (i == split_cnt - 1) {
                src_sur0->rect.width = org_rect0.width - (i * DRV_GFX_SPLIT_MAX_WIDTH);
            } else {
                src_sur0->rect.width = DRV_GFX_SPLIT_MAX_WIDTH;
            }
            tmp_rect = src_sur0->rect;
            tmp_rect.x = DRV_GFX_SPLIT_MAX_WIDTH * i + org_dst_rect.x;
            tmp_rect.y = org_dst_rect.y;
            src_sur0->rect.x = DRV_GFX_SPLIT_MAX_WIDTH * i + org_rect0.x;
            dst_sur->rect = tmp_rect;
            if (matrix_en == TD_TRUE) {
                src_sur0->rect = org_rect0;
            }

            if (vau_dfx_check_mask(VAU_DFX_MASK_SURFACE) == TD_TRUE) {
                gpu_print("*********************split %d split_cnt %d*****************************\n", i, split_cnt);
                vau_debug_print_drv_surface_list(surface_list);
            }

            ret = drv_vau_blit_node_cfg(handle, surface_list);
            if (ret != EXT_SUCCESS) {
                gpu_err("drv_vau_blit_node_cfg failed, i=%u, src_cnt=%u", i, surface_cnt);
                return EXT_FAILURE;
            }
        }
    } else {
        drv_vau_surface *src_sur0 = &(surface_list->src_surface[0]);
        drv_vau_surface *src_sur1 = &(surface_list->src_surface[1]);
        ext_rect org_rect1 = src_sur1->rect;

        for (td_u32 i = 0; i < split_cnt; i++) {
            if (i == (split_cnt - 1)) {
                src_sur1->rect.width = org_rect1.width - (i * DRV_GFX_SPLIT_MAX_WIDTH);
            } else {
                src_sur1->rect.width = DRV_GFX_SPLIT_MAX_WIDTH;
            }

            tmp_rect = src_sur1->rect;
            tmp_rect.x = DRV_GFX_SPLIT_MAX_WIDTH * i + org_dst_rect.x;
            tmp_rect.y = org_dst_rect.y;
            src_sur1->rect.x = DRV_GFX_SPLIT_MAX_WIDTH * i + org_rect1.x;
            src_sur0->rect = tmp_rect;
            dst_sur->rect = tmp_rect;
            if (matrix_en == TD_TRUE) {
                src_sur1->rect = org_rect1;
            }

            if (vau_dfx_check_mask(VAU_DFX_MASK_SURFACE) == TD_TRUE) {
                gpu_print("*********************split %d split_cnt %d*****************************\n", i, split_cnt);
                vau_debug_print_drv_surface_list(surface_list);
            }

            ret = drv_vau_blit_node_cfg(handle, surface_list);
            if (ret != EXT_SUCCESS) {
                gpu_err("drv_vau_blit_node_cfg failed, i=%u src_cnt=%u", i, surface_cnt);
                return EXT_FAILURE;
            }
        }
    }

    return EXT_SUCCESS;
}

STATIC td_bool vau_need_split(drv_vau_surface_list *surface_list)
{
    drv_vau_surface *src = (surface_list->src_surface_cnt == 1) ? &surface_list->src_surface[0] : &surface_list->src_surface[1];

    if (src->mapping_attr.mapping_en) {
        return TD_FALSE;
    }

    if (surface_list->opt != TD_NULL && surface_list->opt->blur_opt.blur_en == TD_TRUE) {
        return TD_FALSE;
    }

    if (surface_list->dst_surface->rect.width > DRV_GFX_NOT_SPLIT_MAX_WIDTH) {
        return TD_TRUE;
    }

    return TD_FALSE;
}

td_s32 drv_vau_blit(td_handle handle, drv_vau_surface_list *surface_list)
{
    if (vau_need_split(surface_list) == TD_TRUE) {
        return drv_vau_blit_with_split(handle, surface_list);
    }

    return drv_vau_blit_node_cfg(handle, surface_list);
}

td_s32 drv_vau_compose(td_handle handle, drv_vau_surface_list *surface_list)
{
    td_s32 ret;
    td_u32 surface_cnt;
    drv_vau_surface tmp_sur0 = { 0 };
    drv_vau_surface tmp_sur1 = { 0 };
    drv_vau_surface src_sur  = { 0 };
    drv_vau_surface dst_sur  = { 0 };

    if ((surface_list == NULL) ||
        (surface_list->src_surface == NULL) ||
        (surface_list->dst_surface == NULL) ||
        (surface_list->src_surface_cnt <= 0)) {
        gpu_err("input surface_list or src surface or dst surface is null");
        return EXT_FAILURE;
    }

    surface_cnt = surface_list->src_surface_cnt;
    surface_list->src_surface_cnt = 2; /* every for 2 layers */

    if (surface_cnt == 1) { /* only 1 layers */
        tmp_sur0 = surface_list->src_surface[0];
    } else { /* max to 2 layers */
        tmp_sur0 = surface_list->src_surface[0];
        tmp_sur1 = surface_list->src_surface[1];
    }
    dst_sur = *(surface_list->dst_surface);

    for (td_u32 i = 0; i < surface_cnt; i++) {
        src_sur = surface_list->src_surface[i];
        if (i == 0) {
            dst_sur.rect = tmp_sur0.out_rect;
            surface_list->dst_surface->rect = tmp_sur0.out_rect;
            surface_list->src_surface[0] = dst_sur;
            surface_list->src_surface[1] = tmp_sur0;
        } else if (i == 1) {
            dst_sur.rect = tmp_sur1.out_rect;
            surface_list->dst_surface->rect = tmp_sur1.out_rect;
            surface_list->src_surface[0] = dst_sur;
            surface_list->src_surface[1] = tmp_sur1;
        } else {
            dst_sur.rect = src_sur.out_rect;
            surface_list->dst_surface->rect = src_sur.out_rect;
            surface_list->src_surface[0] = dst_sur;
            surface_list->src_surface[1] = src_sur;
        }

        ret = drv_vau_blit(handle, surface_list);
        if (ret != EXT_SUCCESS) {
            gpu_err("call drv_vau_blit failed");
            return EXT_FAILURE;
        }
    }

    return EXT_SUCCESS;
}

td_s32 drv_vau_submit(td_handle handle, td_bool is_sync, td_u32 time_out)
{
    drv_vau_notify_mode notify_type;
    vau_dfx_info *vau_info = vau_dfx_get_info();

    if ((is_sync == TD_TRUE) && (osal_in_interrupt() != 0)) {
        gpu_err("unsupport in interrupt");
        return DRV_ERR_VAU_UNSUPPORTED_OPERATION;
    }

    vau_info->func_info.func_cnt.submit_cnt++;
    notify_type = (is_sync == TD_TRUE) ? (DRV_VAU_JOB_NOTIFY_SYNC) : (DRV_VAU_JOB_NOTIFY_ASYNC);

    return vau_list_submit(handle, time_out, notify_type);
}

td_s32 drv_vau_wait_for_done(td_handle handle, td_u32 timeout)
{
    return vau_list_wait_for_done(handle, timeout);
}

td_s32 drv_vau_wait_all_done(td_u32 time_out_ms)
{
    td_s32 ret;

    if ((td_bool)osal_in_interrupt()) {
        gpu_err("unsupport in interrupt");
        return DRV_ERR_VAU_UNSUPPORTED_OPERATION;
    }
    ret = vau_list_wait_all_done(time_out_ms);
    if (ret != EXT_SUCCESS) {
        gpu_err("vau_list_wait_all_done failed!");
        return ret;
    }

    return EXT_SUCCESS;
}
