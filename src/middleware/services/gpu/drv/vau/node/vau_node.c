/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: vau ioctl file
 */

#include "vau_node.h"
#include <securec.h>
#include "vau_mem.h"
#include "vau_list.h"
#include "vau_dfx.h"
#include "vau_hal.h"
#include "vau_hal_apb.h"
#include "platform_core.h"

static td_u32 g_vau_node_id = 0;
static td_bool g_vau_draw_clr_flag = 0;

#ifdef GPU_DRAW_CLR_SUPPORT
static td_bool g_vau_draw_clr_en = 1;
#else
static td_bool g_vau_draw_clr_en = 0;
#endif

PLT_SEC_ITCM_T td_void vau_node_set_draw_flag(td_bool flag)
{
    g_vau_draw_clr_flag = flag;
}

PLT_SEC_ITCM_T td_bool vau_node_get_draw_flag(td_void)
{
    return g_vau_draw_clr_flag;
}

vau_sw_node *vau_node_create(vau_mix_node_mode node_mode)
{
    td_u8 mem_type = VAU_MEM_TYPE_NODE;
    size_t size = sizeof(vau_sw_node) + sizeof(vau_hardware_node);

#ifdef GPU_SDF_SUPPORT
    mem_type = (node_mode == VAU_MIX_NODE_SDF) ? VAU_MEM_TYPE_NODE_SDF : VAU_MEM_TYPE_NODE;
#endif

    vau_sw_node *sw_node = (vau_sw_node *)vau_mem_alloc(mem_type, size);
    if (sw_node == TD_NULL) {
        gpu_err("call vau_mem_alloc failed");
        return TD_NULL;
    }

    (td_void)memset_s(sw_node, size, 0, sizeof(vau_sw_node) + sizeof(vau_hardware_node));

    sw_node->mem_type = mem_type;
    sw_node->hw_node  = (td_void *)((td_u8 *)sw_node + sizeof(vau_sw_node));
    sw_node->phy_addr = (td_u32)(uintptr_t)sw_node->hw_node;
    vau_hardware_node *hw_node = (vau_hardware_node *)sw_node->hw_node;

    g_vau_node_id++;
    hw_node->vau_node_id.u32 = g_vau_node_id;

    /* node default value from logic reg manager */
#ifndef GPU_1A22
    hw_node->local_lighting_cfg.u32    = 0x00000002;
#endif
    hw_node->gauss_blur_ctrl.u32       = 0x80000000;
#ifdef GPU_1A21
    hw_node->vau_t2r0_ctrl.u32         = 0x80001000;
    hw_node->vau_trans_ctrl.u32        = 0x01000000;
    hw_node->vau_t2r1_ctrl.u32         = 0x80001000;
#else
    hw_node->vau_src1_t2r_ctrl.u32     = 0x80001000;
    hw_node->vau_src2_t2r_ctrl.u32     = 0x80001000;
    hw_node->vau_des_r2t_ctrl.u32      = 0x80001000;
    hw_node->vau_cbm_ctrl.u32          = 0x02000000;
#endif
    hw_node->vau_node_cfg.u32          = 0x00000004;
    hw_node->vau_hdr_sys_ctrl.u32      = 0xFFFF1001;

    if (node_mode == VAU_MIX_NODE_MAPP) {
#ifdef GPU_1A22
        hw_node->mix.mapp.vau_mapping_ctrl.u32         = 0x14003000;
#else
        hw_node->mix.mapp.vau_mapping_ctrl.u32         = 0x18000000;
#endif
    } else if (node_mode == VAU_MIX_NODE_DRAW) {
        g_vau_draw_clr_flag = g_vau_draw_clr_en;
        hw_node->mix.draw.vau_draw_ctrl.u32            = 0x00010000;
        hw_node->mix.draw.vau_tess_line_num_max.u32    = 0x00000100;
        hw_node->mix.draw.vau_draw_threshold_quad.u32  = 0x3F983700;
        hw_node->mix.draw.vau_draw_threshold_cubic.u32 = 0x4003D300;
    }

    /* set node mode */
    hw_node->vau_node_cfg.bits.node_mode = (td_u32)node_mode;

    return sw_node;
}

td_s32 vau_node_add(td_handle handle, vau_sw_node *sw_node)
{
    td_s32 ret = vau_list_blit(handle, sw_node);
    if (ret != EXT_SUCCESS) {
        gpu_err("vau_list_blit failed: 0x%x", ret);
        return ret;
    }
    return EXT_SUCCESS;
}

td_s32 vau_node_free(vau_sw_node *sw_node)
{
    vau_list_free_node(sw_node);
    return EXT_SUCCESS;
}

td_void vau_node_set_fast_dma(vau_hardware_node *node, const drv_vau_surface *src_surface,
                              const drv_vau_surface *dst_surface)
{
    td_u32 size;
    td_u32 src_addr = src_surface->phy_addr;

    if (src_surface->compress_info.compress_mode == DRV_GFX_COMPRESS_HIMC) {
        src_addr += 16; /* 16:OFFLINE CMP header */
    }

    if (src_surface->compress_info.compress_mode == DRV_GFX_COMPRESS_NONE) {
        size = src_surface->stride * src_surface->height;
    } else {
        size = src_surface->stride * uapi_byte_align(src_surface->height, 4) / 4; // 4:align val, 4: align 4
    }

    node->vau_src1_ctrl.bits.src1_en       = 1;
    node->vau_src1_ctrl.bits.src1_mode     = 2; /* 2:dma mode */
    node->vau_src1_ctrl.bits.src1_fast_req = 1;
    node->vau_src1_addr.bits.src1_addr     = src_addr;
    node->vau_src1_stride.bits.src1_stride = size;

    node->vau_des_ctrl.bits.des_en         = 1;
    node->vau_des_ctrl.bits.des_fast_req   = 1;
    node->vau_des_addr.bits.des_addr       = dst_surface->phy_addr;
    node->vau_des_stride.bits.des_stride   = size;
}

td_void vau_node_set_src1(vau_hardware_node *node, const drv_vau_surface *surface)
{
    vau_hal_set_src1(node, surface);
}

td_void vau_node_set_src2(vau_hardware_node *node, const drv_vau_surface *src_surface,
                          const drv_vau_surface *dst_surface)
{
    vau_hal_set_src2(node, src_surface, dst_surface);
}

td_void vau_node_set_mapping(vau_hardware_node *node, const drv_vau_surface *src_surface)
{
    vau_hal_set_mapping(node, src_surface);
}

td_void vau_node_set_dst(vau_hardware_node *node, const drv_vau_surface *surface)
{
    vau_hal_set_dst(node, surface);
}

td_void vau_node_set_blend_mode(vau_hardware_node *node, drv_vau_blit_opt *opt)
{
    if (opt == NULL) {
        return;
    }
    vau_hal_set_blend_mode(node, opt);
}

td_void vau_node_set_gaussian(vau_hardware_node *node, drv_vau_blit_opt *opt)
{
    if (opt == NULL) {
        return;
    }
    if (opt->blur_opt.blur_en == TD_TRUE) {
        vau_hal_set_gaussian(node, opt);
    }
}

td_void vau_node_set_cbm(vau_hardware_node *node, const drv_vau_blit_opt *opt, td_u32 color)
{
    drv_vau_alu_mode alu_mode;

    if (opt == TD_NULL) {
        alu_mode = DRV_VAU_ALU_MODE_SRC2_BYPASS;
    } else {
        if (opt->rop_opt.rop_enable) {
            alu_mode = DRV_VAU_ALU_MODE_ROP;
        } else if (opt->blend_opt.blend_enable && (!opt->rain_opt.enable)) {
            alu_mode = DRV_VAU_ALU_MODE_BLEND;
        } else if (opt->blend_opt.blend_enable && opt->rain_opt.enable) {
            alu_mode = DRV_VAU_ALU_MODE_HARD_LIGHT;
        } else {
            alu_mode = DRV_VAU_ALU_MODE_SRC2_BYPASS;
        }

        if (opt->rop_opt.rop_enable == TD_TRUE) {
            vau_hal_set_set_rop(node, &opt->rop_opt);
        }

        if (opt->colorkey_opt.colorkey_enable == TD_TRUE) {
            vau_hal_set_colorkey(node, &opt->colorkey_opt);
        }
        if (opt->lighting_opt.enable == TD_TRUE) {
            vau_hal_set_lighting(node, &opt->lighting_opt);
        }

        if (opt->colorize_en == TD_TRUE) {
            vau_hal_set_colorrize(node, opt->colorize_en, color);
        }
    }

    vau_hal_set_cbm(node, alu_mode);

    return;
}

td_void vau_node_add_next_node(vau_hardware_node *node, td_u32 phy_addr)
{
    vau_hal_add_next_node(node, phy_addr);
}

td_void vau_node_set_finish_isr(vau_hardware_node *node, td_bool is_offline)
{
    g_vau_node_id = 0;
    vau_hal_enable_complete_int(node, is_offline);
}
