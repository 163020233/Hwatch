/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: vau adp node ctrl file
 */

#include "vau_hal.h"
#include "platform_core.h"
#include "vau_dfx.h"
#include "vau_hal_type.h"
#include "vau_hal_apb.h"
#include "vau_hal_affine.h"
#include "vau_mem.h"
#include "vau_hal_vector_draw.h"
#include "gpu_crg_intf.h"

static drv_vau_blend_mode g_vau_src1_blend_cmd_mode[DRV_VAU_BLENDCMD_MAX] = {
    DRV_VAU_BLEND_INVSRC2ALPHA,
    DRV_VAU_BLEND_ZERO,
    DRV_VAU_BLEND_ZERO,
    DRV_VAU_BLEND_INVSRC2ALPHA,
    DRV_VAU_BLEND_ONE,
    DRV_VAU_BLEND_ZERO,
    DRV_VAU_BLEND_SRC2ALPHA,
    DRV_VAU_BLEND_ZERO,
    DRV_VAU_BLEND_INVSRC2ALPHA,
    DRV_VAU_BLEND_INVSRC2ALPHA,
    DRV_VAU_BLEND_SRC2ALPHA,
    DRV_VAU_BLEND_ONE,
    DRV_VAU_BLEND_INVSRC2ALPHA,
    DRV_VAU_BLEND_ONE,
    0
};

static drv_vau_blend_mode g_vau_src2_blend_cmd_mode[DRV_VAU_BLENDCMD_MAX] = {
    DRV_VAU_BLEND_SRC2ALPHA,
    DRV_VAU_BLEND_ZERO,
    DRV_VAU_BLEND_ONE,
    DRV_VAU_BLEND_ONE,
    DRV_VAU_BLEND_INVSRC1ALPHA,
    DRV_VAU_BLEND_SRC1ALPHA,
    DRV_VAU_BLEND_ZERO,
    DRV_VAU_BLEND_INVSRC1ALPHA,
    DRV_VAU_BLEND_ZERO,
    DRV_VAU_BLEND_SRC1ALPHA,
    DRV_VAU_BLEND_INVSRC1ALPHA,
    DRV_VAU_BLEND_ONE,
    DRV_VAU_BLEND_INVSRC1ALPHA,
    DRV_VAU_BLEND_ZERO,
    0
};

STATIC td_void hal_trans_colorkey_component_drv2hal(const drv_vau_colorkey_component *drv_component,
                                                    hal_vau_colorkey_component *hal_component)
{
    if (drv_component->is_component_ignore == TD_TRUE) {
        hal_component->key_mode = HAL_VAU_COLORKEY_VALUE_IGNORE;
    } else {
        if (drv_component->is_component_out == TD_TRUE) {
            hal_component->key_mode = HAL_VAU_COLORKEY_VALUE_OUT;
        } else {
            hal_component->key_mode = HAL_VAU_COLORKEY_VALUE_IN;
        }
    }

    hal_component->key_min  = drv_component->component_min;
    hal_component->key_max  = drv_component->component_max;
    hal_component->key_mask = 0xFF; /* default all bits used for key decision */
}

STATIC td_void hal_trans_colorkey_opt_drv2hal(const drv_vau_colorkey_opt *drv_colorkey,
                                              hal_vau_colorkey_opt *hal_colorkey)
{
    hal_colorkey->key_en = drv_colorkey->colorkey_enable;

    switch (drv_colorkey->colorkey_mode) {
        case DRV_VAU_COLORKEY_NONE:
        case DRV_VAU_COLORKEY_BACKGROUND:
            hal_colorkey->key_sel = HAL_VAU_COLORKEY_BACKGROUND;
            break;
        case DRV_VAU_COLORKEY_FOREGROUND:
            hal_colorkey->key_sel = HAL_VAU_COLORKEY_FOREGROUND_AFTER_CLUT;
            break;
        default:
            hal_colorkey->key_sel = HAL_VAU_COLORKEY_BACKGROUND;
    }

    hal_trans_colorkey_component_drv2hal(&drv_colorkey->colorkey_value.alpha,
                                         &hal_colorkey->component[HAL_VAU_COMPONENT_ALPHA]);
    hal_trans_colorkey_component_drv2hal(&drv_colorkey->colorkey_value.red,
                                         &hal_colorkey->component[HAL_VAU_COMPONENT_RED]);
    hal_trans_colorkey_component_drv2hal(&drv_colorkey->colorkey_value.green,
                                         &hal_colorkey->component[HAL_VAU_COMPONENT_GREEN]);
    hal_trans_colorkey_component_drv2hal(&drv_colorkey->colorkey_value.blue,
                                         &hal_colorkey->component[HAL_VAU_COMPONENT_BLUE]);
}

td_void vau_hal_set_src1(vau_hardware_node *node, const drv_vau_surface *src1)
{
    td_u8 src_mode;

    vau_hal_node_set_src1_info(node, src1);

    if (src1->is_dma == TD_TRUE) {
        src_mode = src1->is_color_surface ? DRV_VAU_MODE_REG_DMA : DRV_VAU_MODE_DMA;
    } else {
        src_mode = src1->is_color_surface ? DRV_VAU_MODE_REG : DRV_VAU_MODE_MEM;
        vau_hal_node_set_src1_alpha(node, src1);
    }
    vau_hal_node_set_src1_mode(node, src_mode);
}

td_void vau_hal_set_src2(vau_hardware_node *node, const drv_vau_surface *src2, const drv_vau_surface *dst)
{
    if (src2->attr.vector.enable == TD_TRUE) {
        return; // draw path support in vau_draw_directly
    }

    vau_hal_node_set_src2_info(node, src2);
    vau_hal_node_set_src2_clut(node, src2);
    vau_hal_node_set_src2_alpha(node, src2);

    if (src2->attr.affine.affine_en) {
        vau_hal_affine_config(node, src2, dst);
    }

    if (src2->fmt >= DRV_GFX_FMT_YUV400SP && src2->fmt <= DRV_GFX_FMT_YUV444SP) {
        vau_hal_node_set_yuv_base_info(node, src2, dst);
    }

    if (src2->is_color_surface) {
        vau_hal_node_set_src2_mode(node, DRV_VAU_MODE_REG);
    }
}

td_void vau_hal_set_mapping(vau_hardware_node *node, const drv_vau_surface *src_surface)
{
    vau_hal_node_set_mapping(node, src_surface);
}

td_void vau_hal_set_dst(vau_hardware_node *node, const drv_vau_surface *dst)
{
    vau_hal_node_set_dst_info(node, dst);
}

td_void vau_hal_set_blend_mode(vau_hardware_node *node, drv_vau_blit_opt *opt)
{
    drv_vau_blend_mode s1_mode;
    drv_vau_blend_mode s2_mode;

    if (opt->blend_opt.blend_cmd == DRV_VAU_BLENDCMD_CONFIG) {
        vau_hal_node_set_blend_mode(node, opt->blend_opt.background_blend_mode,
                                    opt->blend_opt.foreground_blend_mode);
    } else {
        s1_mode = g_vau_src1_blend_cmd_mode[(td_u32)opt->blend_opt.blend_cmd % DRV_VAU_BLENDCMD_MAX];
        s2_mode = g_vau_src2_blend_cmd_mode[(td_u32)opt->blend_opt.blend_cmd % DRV_VAU_BLENDCMD_MAX];
        vau_hal_node_set_blend_mode(node, s1_mode, s2_mode);
    }

    vau_hal_node_set_blend_alpha_mode(node, opt->blend_opt.out_alpha_mode);
}

td_void vau_hal_set_gaussian(vau_hardware_node *node, drv_vau_blit_opt *opt)
{
    node->gauss_blur_ctrl.bits.gauss_blur_ck_gt_en = 1;
    node->gauss_blur_ctrl.bits.gauss_blur_en       = opt->blur_opt.blur_en;
    node->gauss_blur_ctrl.bits.gauss_blur_mask_en  = opt->blur_opt.mask_en;
    node->gauss_blur_ctrl.bits.gauss_blur_alpha_en = opt->blur_opt.alpha_en;

    if (opt->blur_opt.mask_en) { // adapt for gauss ip mirror write
        node->vau_src1_ctrl.bits.src1_h_scan_ord = 1;
    }
    node->gauss_blur_a0.bits.gauss_blur_a0                              = opt->blur_opt.a[0];
    node->gauss_blur_a1.bits.gauss_blur_a1                              = opt->blur_opt.a[1];
    node->gauss_blur_a2.bits.gauss_blur_a2                              = opt->blur_opt.a[2];         // 2：index
    node->gauss_blur_b_causala0.bits.gauss_blur_b_causal0               = opt->blur_opt.b_fcausal[0];
    node->gauss_blur_b_causala1.bits.gauss_blur_b_causal1               = opt->blur_opt.b_fcausal[1];
    node->gauss_blur_b_causala2.bits.gauss_blur_b_causal2               = opt->blur_opt.b_fcausal[2]; // 2：index
    node->gauss_blur_b_anticausala0.bits.gauss_blur_b_anticausal0       = opt->blur_opt.b_bcausal[0];
    node->gauss_blur_b_anticausala1.bits.gauss_blur_b_anticausal1       = opt->blur_opt.b_bcausal[1];
    node->gauss_blur_b_anticausala2.bits.gauss_blur_b_anticausal2       = opt->blur_opt.b_bcausal[2]; // 2：index
    node->gauss_blur_causala_ratio.bits.gauss_blur_causal_ratio         = opt->blur_opt.f_ratio;
    node->gauss_blur_anticausala_ratio.bits.gauss_blur_anticausal_ratio = opt->blur_opt.b_ratio;

    return;
}

td_void vau_hal_set_set_rop(vau_hardware_node *node, const drv_vau_rop_opt *rop_opt)
{
    vau_hal_node_set_rop(node, rop_opt);
}

td_void vau_hal_set_colorkey(vau_hardware_node *node, const drv_vau_colorkey_opt *drv_colorkey)
{
    hal_vau_colorkey_opt hal_colorkey = {0};

    hal_trans_colorkey_opt_drv2hal(drv_colorkey, &hal_colorkey);
    vau_hal_node_set_colorkey(node, &hal_colorkey);

    return;
}

td_void vau_hal_set_colorrize(vau_hardware_node *node, td_bool colorize_en, td_u32 color)
{
    node->vau_cbm_colorize.bits.colorize_en = colorize_en;
    node->vau_cbm_colorize.bits.colorize_r  = (color >> 16) & 0xff; // 16: offset
    node->vau_cbm_colorize.bits.colorize_g  = (color >> 8) & 0xff; // 8: offset
    node->vau_cbm_colorize.bits.colorize_b  = color & 0xff;
}

td_void vau_hal_set_lighting(vau_hardware_node *node, const drv_vau_lighting_opt *lighting_opt)
{
#ifdef GPU_1A22
    uapi_unused(node);
    uapi_unused(lighting_opt);
#else
    node->local_lighting_cfg.bits.local_lighting_en = 1;
    node->local_lighting_cfg.bits.local_lighting_ck_gt_en = 1;
    node->local_lighting_cfg.bits.lighting_mode = (td_u32)lighting_opt->mode;
    node->local_lighting_cfg.bits.is_bi_dir = lighting_opt->attr.is_bi_dir;

    node->local_lighting_cx.bits.centerx = lighting_opt->attr.cx;
    node->local_lighting_cy.bits.centery = lighting_opt->attr.cy;
    node->local_lighting_radius2.bits.radius2 = lighting_opt->attr.radius * lighting_opt->attr.radius;
    node->local_lighting_amplitude.bits.amplitude = lighting_opt->attr.spe_amp;
    node->local_lighting_ampli_scale.bits.amplitude_scale =
        lighting_opt->attr.spe_amp / (lighting_opt->attr.radius * lighting_opt->attr.radius);
    node->local_lighting_amb_amp.bits.amb_amplitude = lighting_opt->attr.amb_amp;

    node->local_lighting_start_angle_rad.bits.start_angle_rad         = lighting_opt->attr.start_angle_rad;
    node->local_lighting_start_end_angle_rad.bits.start_end_angle_rad = lighting_opt->attr.start_end_angle_rad;
    node->local_lighting_end_start_angle_rad.bits.end_start_angle_rad = lighting_opt->attr.end_start_angle_rad;
    node->local_lighting_end_angle_rad.bits.end_angle_rad             = lighting_opt->attr.end_angle_rad;
#ifdef GPU_3322
    node->local_lighting_adjust_value.u32 = (td_u32)*(td_s32 *)(&lighting_opt->attr.adjust_value);
#endif
    node->local_lighting_triwin2_coeff0.bits.triwin2_coeff0 =
        1 / (lighting_opt->attr.start_end_angle_rad - lighting_opt->attr.start_angle_rad);
    node->local_lighting_triwin2_coeff1.bits.triwin2_coeff1 =
        1 / (lighting_opt->attr.end_angle_rad - lighting_opt->attr.end_start_angle_rad);

    node->local_lighting_bbox0_p0.bits.bbox0_min_x = 0;
    node->local_lighting_bbox0_p0.bits.bbox0_min_y = 0;
    node->local_lighting_bbox0_p1.bits.bbox0_max_x = 599; // 599:max_val
    node->local_lighting_bbox0_p1.bits.bbox0_max_y = 599;  // 599:max_val

    node->local_lighting_bbox1_p0.bits.bbox1_min_x = 0;
    node->local_lighting_bbox1_p0.bits.bbox1_min_y = 0;
    node->local_lighting_bbox1_p1.bits.bbox1_max_x = 599;  // 599:max_val
    node->local_lighting_bbox1_p1.bits.bbox1_max_y = 599;  // 599:max_val
#endif
}

td_void vau_hal_set_cbm(vau_hardware_node *node, td_u32 alu_mode)
{
    vau_hal_node_set_cbm(node, alu_mode);
}

td_void vau_hal_add_next_node(vau_hardware_node *node, td_u32 phy_addr)
{
    vau_hal_node_add_next_node(node, phy_addr);
}

td_void vau_hal_enable_complete_int(vau_hardware_node *node, td_bool is_offline)
{
    vau_hal_node_enable_complete_int(node, is_offline);
}

/* APB SET */
PLT_SEC_ITCM_T td_void vau_hal_init(const vau_clk_scene_info *scene_info)
{
    gpu_crg_init(GPU_ID_VAU, (td_void*)scene_info);
    vau_hal_apb_init_queue();
    vau_hal_vector_draw_mem_init();
    vau_hal_apb_vector_draw_init();
}

td_void vau_hal_deinit(const vau_clk_scene_info *scene_info)
{
    gpu_crg_deinit(GPU_ID_VAU, (td_void*)scene_info);
}

td_void vau_hal_suspend(const vau_clk_scene_info *scene_info)
{
    vau_hal_apb_clear_int_state(0xFF); /* 0xF: clear all int bits */
    vau_hal_deinit(scene_info);
}

PLT_SEC_ITCM_T td_void vau_hal_resume(const vau_clk_scene_info *scene_info)
{
    vau_hal_init(scene_info);
}

td_bool vau_hal_check_is_idle(td_void)
{
    td_u32 i;
    /* get the state of tde one more time ,make sure it's idle */
    for (i = 0; i < 10; i++) {  /* 10:index */
        if (!vau_hal_apb_is_idle()) {
            return TD_FALSE;
        }
    }
    return TD_TRUE;
}

STATIC td_void hal_hsv_draw_disable(td_void)
{
#ifdef GPU_1A21
    td_u32 val;
    uapi_reg_read(HAL_VAU_HSV_CTRL, val);
    if ((val & 0x1) != 0) {
        uapi_reg_write(HAL_VAU_HSV_CTRL, 0x0);
    }

    uapi_reg_read(HAL_VAU_DRAW_CTRL, val);
    if ((val & 0x80000000) != 0) {
        uapi_reg_write(HAL_VAU_DRAW_CTRL, 0x10000);
    }
#endif
}

td_s32 vau_hal_execute_node(td_u32 start_node)
{
    if (vau_hal_check_is_idle()) {
        hal_hsv_draw_disable();
        vau_hal_apb_clear_int_state(0xFF); /* 0xF: clear all int bits */
        vau_hal_apb_set_start_node(start_node);
        vau_hal_apb_set_start_up();
    } else {
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

td_u32 vau_hal_get_current_node(td_void)
{
    return vau_hal_apb_get_current_ndoe();
}

td_u32 vau_hal_get_int_state(td_void)
{
    return vau_hal_apb_get_int_state();
}

td_void vau_hal_clear_int_state(td_u32 mask)
{
    return vau_hal_apb_clear_int_state(mask);
}

td_void vau_hal_set_start_up(td_void)
{
    return vau_hal_apb_set_start_up();
}

td_void vau_hal_free_vector_mem(vau_hardware_node *node)
{
    td_bool inner_cmd;
    td_bool inner_data;
    td_u32 start, end;

    if (node->mix.draw.vau_draw_ctrl.bits.draw_en != 1) {
        return;
    }

    vau_mem_get_range(VAU_MEM_TYPE_PATH_CMD, &start, &end);
    inner_cmd = (node->mix.draw.vau_draw_cmd_addr.u32 < end) && (node->mix.draw.vau_draw_cmd_addr.u32 >= start);

    vau_mem_get_range(VAU_MEM_TYPE_PATH_DATA, &start, &end);
    inner_data = (node->mix.draw.vau_draw_data_addr.u32 < end) && (node->mix.draw.vau_draw_data_addr.u32 >= start);

    if (inner_data) {
        vau_mem_ref_dec(VAU_MEM_TYPE_PATH_DATA, (td_uchar *)(uintptr_t)node->mix.draw.vau_draw_data_addr.u32);
    }
    if (inner_cmd) {
        vau_mem_ref_dec(VAU_MEM_TYPE_PATH_CMD, (td_uchar *)(uintptr_t)node->mix.draw.vau_draw_cmd_addr.u32);
    }

    return;
}

td_void vau_hal_free_mesh_mem(vau_hardware_node *node)
{
#ifdef GPU_1A22
    uapi_unused(node);
#else
    if (node->vau_model_addr.u32 == 0) {
        gpu_err("node[%#x] set null mesh model addr!\n", (td_u32)(uintptr_t)node);
        return;
    }

    vau_psram_free((td_uchar *)(uintptr_t)node->vau_model_addr.u32);
#endif
    return;
}

td_void vau_hal_set_alpha_strb(vau_hardware_node *node, td_bool enable)
{
    vau_hal_node_set_alpha_strb(node, enable);
    return;
}
