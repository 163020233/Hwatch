/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: hdr process file
 */

#include "vau_hdr_process.h"
#include "vau_hdr_node.h"
#include "vau_mem.h"
#include "hdr_algorithm.h"
#include "vau_node.h"
#include "vau_hal.h"
#include "vau_dfx.h"
#include "vau_list.h"

STATIC hdr_dynamic_metadata g_default_dynamic_metadata = {
    1, 1, 2076, 3135, 3652, 1, 1,
    { 2770, 2080 }, { 1, 1 }, { 6193, 6193 }, { 24, 24 }, { 729, 727}, { 0, 0 },
    { 10, 10 }, { 1, 1 }, { 1, 1 }, { 1, 1 }, { 6, 6 }, { 6, 6 }, { 1, 1 }, { 1, 1 },
    { { 0, 0 }, { 2, 2 } }, { { 240, 240 }, { 0, 0 } }, { { 39, 39 }, { 1555, 1555 } },
    { { 611, 611 }, { 1004, 1004 } }, { { 695, 692 }, { 1004, 1004 } }, { { 121, 120 }, { 145, 145 } },
    1, 4,
    { 38, 24, 38, 25, 0, 0, 0, 0 },
    0, 0, 0,
};

STATIC td_s32 vau_hdr_get_alg_cfg(const drv_vau_surface *hdr_src1, const drv_vau_surface *hdr_src2,
    const drv_vau_surface *vau_src2, drv_hdr_param* hdr_param, drv_hdr_cfg* hdr_cfg)
{
    hdr_metadata_nonstandard metadata_nonstandard = { 3072, 4096, 196, 87, 3.900000, 200, 1, 2.385862 };
    hdr_image_cicp cicp_gainmap = { HDR_COLOR_PRIMARIES_UNSPECIFIED, HDR_TRANS_CHARAC_UNSPECIFIED,
        HDR_MATRIX_COEFFICIENTS_BT601, 1 };

    hdr_param->display_headroom = 2.0f; /* HDR/SDR brightness ratio */
    hdr_param->metadata_standard.dynamic_metadata = g_default_dynamic_metadata;

    if (hdr_src1 && hdr_src2) {
        hdr_param->hdr_source_type = HDR_STANDARD_DOUBLE_LAYER;
        if (hdr_src1->hdr_attr.is_nonstandard) {
            hdr_param->hdr_source_type = HDR_NONSTANDARD_DOUBLE_LAYER;
            hdr_param->metadata_nonstandard = metadata_nonstandard;
        }
#ifdef GPU_HDR_METADATA_VALID
        hdr_param->metadata_standard.dynamic_metadata = hdr_src1->hdr_attr.metadata_standard.dynamic_metadata;
#endif
    } else if (hdr_src2 && (!hdr_src1) && (!vau_src2)) {
        hdr_param->hdr_source_type = HDR_SINGLE_LAYER;
    } else if (hdr_src2 && (!hdr_src1) && (vau_src2)) {
        hdr_param->hdr_source_type = HDR_BYPASS;
    } else {
        hdr_param->hdr_source_type = HDR_SOURCE_TYPE_MAX;
    }

    if (hdr_src2) {
        hdr_param->img_info_base.img_width  = hdr_src2->width;
        hdr_param->img_info_base.img_height = hdr_src2->height;
        hdr_param->img_info_base.cicp_base  = hdr_src2->hdr_attr.cicp_info;
        hdr_param->img_info_base.cicp_base.matrix_coefficients = HDR_MATRIX_COEFFICIENTS_BT709;
    }

    if (hdr_src1) {
        hdr_param->img_info_gainmap.img_width  = hdr_src1->width;
        hdr_param->img_info_gainmap.img_height = hdr_src1->height;
        hdr_param->img_info_gainmap.gainmap_components = (hdr_src1->fmt == DRV_GFX_FMT_YUV400SP) ? 0x1 : 0x3;
        hdr_param->img_info_gainmap.extend_info    = hdr_src1->hdr_attr.extend_info;
        hdr_param->img_info_gainmap.cicp_gainmap   = cicp_gainmap;
        hdr_param->img_info_gainmap.cicp_alternate = hdr_src1->hdr_attr.cicp_info;
        hdr_param->img_info_gainmap.cicp_alternate.matrix_coefficients = HDR_MATRIX_COEFFICIENTS_BT709;
    }

    return vau_hdr_get_cfg(hdr_param, hdr_cfg);
}

STATIC td_s32 vau_hdr_node_add(td_handle handle, vau_sw_node *sw_node)
{
    td_s32 ret = vau_list_blit(handle, sw_node);
    if (ret != EXT_SUCCESS) {
        gpu_err("vau_list_blit failed: 0x%x", ret);
        return ret;
    }
    return EXT_SUCCESS;
}

#if defined(GPU_3322) || defined(GPU_1A22)
td_void vau_hdr_blit(vau_hdr_hardware_node *node, const drv_vau_surface_list *surface_list)
{
    td_bool hdr_en = TD_FALSE;
    drv_hdr_cfg hdr_cfg = { 0 };
    drv_hdr_param hdr_param = { 0 };
    drv_vau_surface *src1 = TD_NULL, *src2 = TD_NULL;

    if (surface_list->src_surface_cnt == 0x2) {
        src1 = &surface_list->src_surface[0];
        src2 = &surface_list->src_surface[1];
        if ((src1->fmt >= DRV_GFX_FMT_YUV400SP) && (src1->fmt <= DRV_GFX_FMT_YUV444SP)) {
            hdr_en = TD_TRUE;
            vau_hdr_get_alg_cfg(src1, src2, TD_NULL, &hdr_param, &hdr_cfg);
            vau_hal_set_hdr_cvfir(node, src1);
            vau_hal_set_hdr_hfir(node, src1);
            vau_hal_set_hdr_hipp_csc(node, src1, &hdr_cfg.y2r_gainmap_cfg);
            vau_hal_set_hdr_rcbn(node, &hdr_cfg.combine_cfg);
            vau_hal_set_hdr_vdm(node, &hdr_cfg);
        }
    } else {
        if (surface_list->src_surface[0].fmt == DRV_GFX_FMT_ARGB2101010) {
            src1 = &surface_list->src_surface[0];
        } else {
            src2 = &surface_list->src_surface[0];
            if (surface_list->dst_surface->fmt == DRV_GFX_FMT_ARGB2101010) {
                vau_hdr_get_alg_cfg(TD_NULL, src2, TD_NULL, &hdr_param, &hdr_cfg);
                vau_node_set_cbm((vau_hardware_node *)node, surface_list->opt, src2->color);
                vau_hal_set_hdr_ctadj(node, &hdr_cfg.scale_cfg); /* 8->10bit for HDR cbm */
            }
        }
    }

    node->vau_node_cfg.bits.node_mode = hdr_en ? (0x2) : node->vau_node_cfg.bits.node_mode;
    vau_hal_set_hdr_sys_ctrl(node, hdr_en, !hdr_en, TD_FALSE);

    if (src1 != TD_NULL) {
        vau_node_set_src1((vau_hardware_node *)node, src1);
    }

    if (src2 != TD_NULL) {
        vau_node_set_src2((vau_hardware_node *)node, src2, surface_list->dst_surface);
        vau_node_set_mapping((vau_hardware_node *)node, src2);
    }

    vau_node_set_gaussian((vau_hardware_node *)node, surface_list->opt);
    vau_node_set_blend_mode((vau_hardware_node *)node, surface_list->opt);
    vau_hal_set_hdr_cbm(node, (hdr_en ? 0 : (src1 != TD_NULL)), (src2 != TD_NULL));

    if (surface_list->dst_surface->fmt != DRV_GFX_FMT_ARGB2101010) {
        vau_hal_set_hdr_disp1_dither(node, hdr_en);
    }

    vau_node_set_dst((vau_hardware_node *)node, surface_list->dst_surface);
}

STATIC vau_sw_node *vau_hdr_node_create(vau_mix_node_mode node_mode)
{
    static td_u32 vau_hdr_node_id = 0;
    size_t size = sizeof(vau_sw_node) + sizeof(vau_hdr_hardware_node);

    vau_sw_node *sw_node = (vau_sw_node *)vau_mem_alloc(VAU_MEM_TYPE_NODE_HDR, size);
    if (sw_node == TD_NULL) {
        gpu_err("call vau_mem_alloc failed");
        return TD_NULL;
    }

    (td_void)memset_s(sw_node, size, 0, sizeof(vau_sw_node) + sizeof(vau_hdr_hardware_node));

    sw_node->mem_type = (td_u8)VAU_MEM_TYPE_NODE_HDR;
    sw_node->hw_node   = (td_void *)((td_u8 *)sw_node + sizeof(vau_sw_node));
    sw_node->phy_addr  = (td_u32)(uintptr_t)sw_node->hw_node;
    vau_hdr_hardware_node *hw_node = (vau_hdr_hardware_node *)sw_node->hw_node;

    vau_hdr_node_id++;
    hw_node->vau_node_id.u32 = vau_hdr_node_id;

    /* node default value from logic reg manager */
#ifndef GPU_1A22
    hw_node->local_lighting_cfg.u32    = 0x00000002;
#endif
    hw_node->gauss_blur_ctrl.u32       = 0x80000000;

    hw_node->vau_src1_t2r_ctrl.u32     = 0x80001000;
    hw_node->vau_src2_t2r_ctrl.u32     = 0x80001000;
    hw_node->vau_des_r2t_ctrl.u32      = 0x80001000;
    hw_node->vau_cbm_ctrl.u32          = 0x02000000;

    hw_node->vau_node_cfg.u32          = 0x00000004;
    hw_node->vau_hdr_sys_ctrl.u32      = 0xFFFF1001;

    /* set node mode */
    hw_node->vau_node_cfg.bits.node_mode = (td_u32)node_mode;

    return sw_node;
}

td_s32 vau_hdr_process(td_handle handle, drv_vau_surface_list *surface_list)
{
    vau_sw_node *sw_node = vau_hdr_node_create(VAU_MIX_NODE_MAPP);
    if (sw_node == TD_NULL) {
        gpu_err("failed to create hdr node!");
        return DRV_ERR_VAU_NO_MEM;
    }

    vau_hdr_blit((vau_hdr_hardware_node *)sw_node->hw_node, surface_list);

    if (vau_hdr_node_add(handle, sw_node) != EXT_SUCCESS) {
        vau_node_free(sw_node);
        gpu_err("failed to add hdr node");
        return EXT_FAILURE;
    }
    return EXT_SUCCESS;
}
#else
static td_void vau_hdr_blit(vau_hdr_hardware_node *node, const drv_vau_surface_list *surface_list)
{
    drv_vau_surface *hdr_src1 = TD_NULL, *hdr_src2 = TD_NULL, *vau_src2 = TD_NULL;

    drv_hdr_cfg hdr_cfg = { 0 };
    drv_hdr_param hdr_param = { 0 };

    if (surface_list->src_surface_cnt == 2) { // 2:two layer
        if (surface_list->src_surface[0].fmt >= DRV_GFX_FMT_YUV400SP &&
            surface_list->src_surface[0].fmt <= DRV_GFX_FMT_YUV444SP) {
            hdr_src1 = &surface_list->src_surface[0];
        } else {
            vau_src2 = &surface_list->src_surface[0];
        }
        hdr_src2 = &surface_list->src_surface[1];
    } else {
        hdr_src2 = &surface_list->src_surface[0];
    }

    // HDR SOFT ALG
    vau_hdr_get_alg_cfg(hdr_src1, hdr_src2, vau_src2, &hdr_param, &hdr_cfg);

    if (vau_src2 != TD_NULL) { // SDR SRC2
        vau_hal_set_src2((vau_hardware_node *)node, vau_src2, surface_list->dst_surface);
        vau_node_set_cbm((vau_hardware_node *)node, surface_list->opt, vau_src2->color);
        vau_hal_set_hdr_ctadj(node, &hdr_cfg.scale_cfg); /* 8->10bit for HDR cbm */
    }

    if (hdr_src1 != TD_NULL) {
        vau_hal_set_hdr_src1(node, hdr_src1);
        vau_hal_set_hdr_cvfir(node, hdr_src1);
        vau_hal_set_hdr_hfir(node, hdr_src1);
        vau_hal_set_hdr_hipp_csc(node, hdr_src1, &hdr_cfg.y2r_gainmap_cfg);
    }

    // HDR SRC2 CONFIG
    if (hdr_src2->fmt >= DRV_GFX_FMT_YUV400SP && hdr_src2->fmt <= DRV_GFX_FMT_YUV444SP) {
        vau_hal_set_hdr_cvfir2(node, hdr_src2);
        vau_hal_set_hdr_hfir2(node);
    }
    vau_hal_set_hdr_vdm(node, &hdr_cfg);
    vau_hal_set_hdr_src2(node, hdr_src2);

    if ((surface_list->src_surface_cnt == 2) && // only hdr 2 layer need combine
        (hdr_src1 != TD_NULL) && (hdr_src2 != TD_NULL)) {
        vau_hal_set_hdr_rcbn(node, &hdr_cfg.combine_cfg);
    }

    if ((surface_list->opt != TD_NULL) && (surface_list->opt->lighting_opt.enable == TD_TRUE)) {
        vau_hal_set_hdr_lighting(node, &surface_list->opt->lighting_opt, hdr_src2);
    }

    vau_hal_set_hdr_cbm(node, (vau_src2 != TD_NULL), TD_TRUE);

    if (surface_list->dst_surface->fmt == DRV_GFX_FMT_ARGB2101010) { // HDR DST
        vau_hal_set_hdr_sys_ctrl(node, TD_FALSE, TD_FALSE, TD_TRUE);
        vau_hal_set_hdr_dst(node, surface_list->dst_surface);
    } else { // SDR DST
        vau_hal_set_hdr_sys_ctrl(node, TD_FALSE, TD_FALSE, TD_FALSE);
        vau_hal_set_hdr_disp1_dither(node, TD_FALSE);
        vau_hal_set_dst((vau_hardware_node *)node, surface_list->dst_surface);
    }
}

STATIC vau_sw_node *vau_hdr_node_create(vau_mix_node_mode node_mode)
{
    static td_u32 vau_hdr_node_id = 0;
    size_t size = sizeof(vau_sw_node) + sizeof(vau_hdr_hardware_node);

    vau_sw_node *sw_node = (vau_sw_node *)vau_mem_alloc(VAU_MEM_TYPE_NODE_HDR, size);
    if (sw_node == TD_NULL) {
        gpu_err("call vau_mem_alloc failed");
        return TD_NULL;
    }

    (td_void)memset_s(sw_node, size, 0, sizeof(vau_sw_node) + sizeof(vau_hdr_hardware_node));

    sw_node->mem_type = (td_u8)VAU_MEM_TYPE_NODE_HDR;
    sw_node->hw_node   = (td_void *)((td_u8 *)sw_node + sizeof(vau_sw_node));
    sw_node->phy_addr  = (td_u32)(uintptr_t)sw_node->hw_node;
    vau_hdr_hardware_node *hw_node = (vau_hdr_hardware_node *)sw_node->hw_node;

    vau_hdr_node_id++;
    hw_node->vau_node_id.u32 = vau_hdr_node_id;

    /* node default value from logic reg manager */
    hw_node->local_lighting_cfg.u32    = 0x00000002;
    hw_node->gauss_blur_ctrl.u32       = 0x80000000;
    hw_node->vau_t2r0_ctrl.u32         = 0x80001000;
    hw_node->vau_trans_ctrl.u32        = 0x01000000;
    hw_node->vau_t2r1_ctrl.u32         = 0x80001000;
    hw_node->vau_node_cfg.u32          = 0x00000004;
    hw_node->vau_hdr_sys_ctrl.u32      = 0xFFFF1001;

    /* set node mode */
    hw_node->vau_node_cfg.bits.node_mode = (td_u32)node_mode;

    /* HDR enable */
    hw_node->vau_src1_ctrl.bits.hdr_en = 0x1;

    return sw_node;
}

td_s32 vau_hdr_process(td_handle handle, drv_vau_surface_list *surface_list)
{
    vau_sw_node *sw_node = vau_hdr_node_create(VAU_MIX_NODE_MAPP);
    if (sw_node == TD_NULL) {
        gpu_err("failed to create hdr node!");
        return DRV_ERR_VAU_NO_MEM;
    }

    vau_hdr_blit(sw_node->hw_node, surface_list);

    if (vau_hdr_node_add(handle, sw_node) != EXT_SUCCESS) {
        vau_node_free(sw_node);
        gpu_err("failed to add hdr node");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}
#endif

