/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: vau hal node ctrl file
 */

#include "vau_hal_node.h"
#include "vau_dfx.h"
#ifdef GPU_ZME_SUPPORT
#include "vau_zme_alg.h"
#endif

td_void vau_hal_node_set_src1_info(vau_hardware_node *node, const drv_vau_surface *surface)
{
    node->vau_src1_ctrl.bits.src1_en            = 1;
    node->vau_src1_ctrl.bits.src1_fmt           = surface->fmt;
    node->vau_src1_ctrl.bits.src1_argb_order    = surface->argb_order;
    node->vau_src1_ctrl.bits.src1_rgb_exp       = DRV_VAU_RGB_EXP_HIGH_BITS;
    node->vau_src1_ctrl.bits.src1_alpha_range   = surface->alpha_info.is_alpha_max_128;
#if (!defined(GPU_1A21)) && (!defined(GPU_3322))
    node->vau_src1_ctrl.bits.online_dcmp_ck_gt_en = 1;
#endif

#ifdef GPU_3322
    node->vau_src1_ctrl.bits.src1_cbcr_order    = 0x0;
    node->vau_src1c_addr.bits.src1c_addr        = surface->phy_addr + surface->uv_offset;
    node->vau_src1c_stride.bits.src1c_stride    = surface->stride;
#endif
    node->vau_src1_addr.bits.src1_addr          = surface->phy_addr;
    node->vau_src1_stride.bits.src1_stride      = surface->stride;
    node->vau_src1_imgsize.bits.src1_width      = surface->width - 1;
    node->vau_src1_imgsize.bits.src1_height     = surface->height - 1;

    if (surface->is_color_surface == TD_TRUE) {
        node->vau_src1_fill.bits.src1_color_fill = surface->color;
    }

    /* fdr crop */
    node->vau_src1_rect_xy.bits.src1_rect_x      = (td_u32)surface->rect.x;
    node->vau_src1_rect_xy.bits.src1_rect_y      = (td_u32)surface->rect.y;
    node->vau_src1_rect_wh.bits.src1_rect_width  = surface->rect.width - 1;
    node->vau_src1_rect_wh.bits.src1_rect_height = surface->rect.height - 1;

    if (surface->fmt == DRV_GFX_FMT_A1 || surface->fmt == DRV_GFX_FMT_A1B) {
        node->vau_src1_pix_rgb.bits.src1_pix_rgb = surface->color;
        node->vau_src1_alpha.bits.src1_alpha0 = 0x00; /* decide A1 alpha pix=0 */
        node->vau_src1_alpha.bits.src1_alpha1 = 0xff; /* decide A1 alpha pix=1 */
    } else if (surface->fmt == DRV_GFX_FMT_A8) {
        node->vau_src1_pix_rgb.bits.src1_pix_rgb = surface->color;
    }

    drv_gfx_compress_mode compress_mode = surface->compress_info.compress_mode;
    if ((compress_mode == DRV_GFX_COMPRESS_HFBC_2X) || (compress_mode == DRV_GFX_COMPRESS_HFBC_3X)) {
        node->vau_src1_ctrl.bits.src1_ice_en   = 1;
        node->vau_src1_ctrl.bits.src1_cmp_mode = 1; // online mode
        if (surface->fmt == DRV_GFX_FMT_RGB888) {
            node->vau_src1_htc_online_dcmp0.bits.src1_dcmp_pix_format = 0;
            node->vau_src1_htc_online_dcmp0.bits.src1_block_size =
                (compress_mode == DRV_GFX_COMPRESS_HFBC_2X) ? 24 : 16; // 16,24：block_size
        } else {
            node->vau_src1_htc_online_dcmp0.bits.src1_dcmp_pix_format = 1;
            node->vau_src1_htc_online_dcmp0.bits.src1_block_size =
                (compress_mode == DRV_GFX_COMPRESS_HFBC_2X) ? 32 : 24; // 32,24：block_size
        }

        node->vau_src1_htc_online_dcmp0.bits.src1_dcmp_bit_depth  = 0;
        node->vau_src1_htc_online_dcmp0.bits.src1_dcmp_mpde       = 0;
        node->vau_src1_htc_online_dcmp0.bits.src1_dcmp_pq_sel     = (compress_mode == DRV_GFX_COMPRESS_HFBC_2X) ? 0 : 1;
    }
}

td_void vau_hal_node_set_src1_mode(vau_hardware_node *node, td_u8 mode)
{
    node->vau_src1_ctrl.bits.src1_mode = mode;
}

td_void vau_hal_node_set_src1_alpha(vau_hardware_node *node, const drv_vau_surface *surface)
{
    if (surface->fmt == DRV_GFX_FMT_ARGB1555) {
        node->vau_src1_alpha.bits.src1_alpha0 = surface->alpha_info.alpha0;
        node->vau_src1_alpha.bits.src1_alpha1 = surface->alpha_info.alpha1;
    }

    /* s1_blend_mode config in cbm */
    node->vau_cbm_src1_cbm_para.bits.s1_multi_global_en = surface->alpha_info.golbal_premult_en;
    node->vau_cbm_src1_cbm_para.bits.s1_premult_en      = surface->alpha_info.premult_en;
    node->vau_cbm_src1_cbm_para.bits.s1_palpha_en       = surface->alpha_info.pixel_alpha_en;
    node->vau_cbm_src1_cbm_para.bits.s1_galpha_en       = surface->alpha_info.global_alpha_en;
}

#ifdef GPU_ZME_SUPPORT
STATIC td_void vau_hal_node_zme_info_cfg(vau_hardware_node *node, const drv_vau_surface *surface,
                                         const drv_vau_surface *dst)
{
    if (surface->fmt < DRV_GFX_FMT_YUV400SP || surface->fmt > DRV_GFX_FMT_YUV444SP) {
        return;
    }

    node->vau_src2c_addr.u32   = surface->phy_addr + surface->uv_offset;
    node->vau_src2c_stride.bits.src2c_stride =
        (surface->fmt == DRV_GFX_FMT_YUV444SP) ? surface->stride * 2 : surface->stride; // 2:uv stride is double

#ifdef GPU_1A22
    if (surface->clut_addr != 0) { // AYUV
        node->vau_src1_ctrl.bits.src1c_a8_en     = 0x1;
        node->vau_src1c_addr.bits.src1c_addr     = surface->clut_addr;
        node->vau_src1c_stride.bits.src1c_stride = surface->stride;
    }
#endif

    // zme config // 0x380 ~ 0x39c
    node->mix.semi.src2_hsp.bits.hlmsc_en  = 1; // 水平亮度缩放使能
    node->mix.semi.src2_hsp.bits.hchmsc_en = 1; // 水平色度缩放使能
    node->mix.semi.src2_hsp.bits.hratio    = surface->rect.width * 0x100000 / dst->rect.width;

    node->mix.semi.src2_vsp.bits.vlmsc_en  = 1; // 垂直亮度缩放使能
    node->mix.semi.src2_vsp.bits.vchmsc_en = 1; // 垂直色度缩放使能
    // 缩放输入数据格式 0：422；1：420；3：ARGB
    if (surface->fmt == DRV_GFX_FMT_YUV422SP) {
        node->mix.semi.src2_vsp.bits.zme_in_fmt = 0;
        node->mix.semi.src2_vsp.bits.zme_out_fmt = 2; // 0:YUV422, 2：444；3：ARGB
    } else if (surface->fmt == DRV_GFX_FMT_YUV420SP) {
        node->mix.semi.src2_vsp.bits.zme_in_fmt = 1;
        node->mix.semi.src2_vsp.bits.zme_out_fmt = 2; // 0:YUV422, 2：444；3：ARGB
    } else {
        node->mix.semi.src2_vsp.bits.zme_in_fmt = 3; // 3:ARGB YUV444
        node->mix.semi.src2_vsp.bits.zme_out_fmt = 3; // 0:YUV422, 2：444；3：ARGB
    }

    node->mix.semi.src2_vsr.u32 = surface->rect.height * 0x1000 / dst->rect.height;

    node->mix.semi.src2_voffset.bits.vluma_offset   = 0; // 垂直亮度偏移，补码表示
    node->mix.semi.src2_voffset.bits.vchroma_offset = 0; // 垂直色度偏移，补码表示

    node->mix.semi.src2_zmeoreso.bits.ow = dst->rect.width - 1;  // 输出分辨率
    node->mix.semi.src2_zmeoreso.bits.oh = dst->rect.height - 1; // 输出分辨率
    node->mix.semi.src2_zmeireso.bits.iw = surface->rect.width - 1;   // 输入分辨率
    node->mix.semi.src2_zmeireso.bits.ih = surface->rect.height - 1;  // 输入分辨率

    // csc config  0x3a0
    node->mix.semi.src2_yuv2rgb_ctrl.bits.vdm_y2r_en       = 1; // yuv2rgb总使能信号
    node->mix.semi.src2_yuv2rgb_ctrl.bits.vdm_v0_csc_en    = 1; // 视频0 yuv2rgb使能信号

    // 默认使用滤波模式
    node->mix.semi.src2_hsp.bits.hafir_en = 1;
    node->mix.semi.src2_hsp.bits.hlfir_en = 1;
    node->mix.semi.src2_hsp.bits.hchfir_en = 1;

    node->mix.semi.src2_vsp.bits.vafir_en = 1;
    node->mix.semi.src2_vsp.bits.vlfir_en = 1;
    node->mix.semi.src2_vsp.bits.vchfir_en = 1;

    vau_set_zme_scale_coeff(node, surface);
}
#endif

td_void vau_hal_node_set_yuv_base_info(vau_hardware_node *node, const drv_vau_surface *surface,
                                       const drv_vau_surface *dst)
{
    node->vau_node_cfg.bits.node_mode = 0x2;
#if defined(GPU_1A21)
    uapi_unused(dst);
    node->mix.semi.vau_src2c_addr.u32   = surface->phy_addr + surface->uv_offset;
    node->mix.semi.vau_src2c_stride.bits.src2c_stride =
        (surface->fmt == DRV_GFX_FMT_YUV444SP) ? surface->stride * 2 : surface->stride; // 2:uv stride is double

    /* cvfir/hfir/csc ip en control */
    node->mix.semi.vau_cvfir_vsp.bits.cvfir_en  = (surface->fmt == DRV_GFX_FMT_YUV444SP) ? 0x0 : 0x1;
    node->mix.semi.vau_hfir_ctrl.bits.hfir_mode = 0x1;
    node->mix.semi.vau_hipp_csc_ctrl.bits.hipp_csc_en = 0x1;

    /* cvfir */
    node->mix.semi.vau_cvfir_vinfo.bits.vzme_ck_gt_en = 0x0;
    node->mix.semi.vau_cvfir_vinfo.bits.out_pro = 0x1;
    node->mix.semi.vau_cvfir_vinfo.bits.out_fmt = 0x1;
    node->mix.semi.vau_cvfir_vinfo.bits.out_height = surface->height - 1;
    node->mix.semi.vau_cvfir_vsp.bits.vratio = (1 << 12); /* 12:vratio */

    /* hfir */
    node->mix.semi.vau_hfir_ctrl.bits.ck_gt_en = 0x0;
#else
#ifdef GPU_ZME_SUPPORT
    vau_hal_node_zme_info_cfg(node, surface, dst);
#endif
#endif
}

STATIC td_void vau_hal_node_set_online_cmp_info(vau_hardware_node *node, const drv_vau_surface *surface)
{
    node->vau_src2_ctrl.bits.src2_ice_en       = 1;
    node->vau_src2_ctrl.bits.src2_cmp_mode     = 1; // online mode
    if (surface->fmt == DRV_GFX_FMT_RGB888) {
        node->vau_src2_htc_online_dcmp0.bits.src2_dcmp_pix_format = 0;
        node->vau_src2_htc_online_dcmp0.bits.src2_block_size =
            (surface->compress_info.compress_mode == DRV_GFX_COMPRESS_HFBC_2X) ? 24 : 16; // 24,16: block_size
    } else {
        node->vau_src2_htc_online_dcmp0.bits.src2_dcmp_pix_format = 1;
        node->vau_src2_htc_online_dcmp0.bits.src2_block_size =
            (surface->compress_info.compress_mode == DRV_GFX_COMPRESS_HFBC_2X) ? 32 : 24; // 24,32: block_size
    }

    node->vau_src2_htc_online_dcmp0.bits.src2_dcmp_bit_depth  = 0;
    node->vau_src2_htc_online_dcmp0.bits.src2_dcmp_mpde       = 0;
    node->vau_src2_htc_online_dcmp0.bits.src2_dcmp_pq_sel     =
            (surface->compress_info.compress_mode == DRV_GFX_COMPRESS_HFBC_2X) ? 0 : 1;
}

STATIC td_void vau_hal_node_set_hsv_info(vau_hardware_node *node, const drv_vau_surface *surface)
{
    node->mix.semi.vau_hsv_rgb_cfg.bits.hsv_rgb_en  = 1;
    node->mix.semi.vau_hsv_rgb_cfg.bits.mask_en = surface->hsv_info.mask_en;
    node->mix.semi.vau_hsv_rgb_cfg.bits.mask_threshold = surface->hsv_info.mask_threshold;
    node->mix.semi.vau_hsv_rgb_cfg.bits.mask_threshold_en = (surface->hsv_info.mask_threshold != 0) ? 1 : 0;
    node->mix.semi.vau_hsv_rgb_cfg.bits.hsv_rgb_ck_gt_en = 1;

    // h_ratio = 392964 / 359
    node->mix.semi.vau_hsv_rgb_adj_h_v.bits.hsv_rgb_h_adj  = surface->hsv_info.h_adj * 392964 / 359;
    // v_ratio = 255 / 100
    node->mix.semi.vau_hsv_rgb_adj_h_v.bits.hsv_rgb_v_adj  = surface->hsv_info.v_adj * 255 / 100;
    // s_ratio = 65535 / 100
    node->mix.semi.vau_hsv_rgb_adj_s.bits.hsv_rgb_s_adj    = surface->hsv_info.s_adj * 65535 / 100;
}

td_void vau_hal_node_set_src2_info(vau_hardware_node *node, const drv_vau_surface *surface)
{
    node->vau_src2_ctrl.bits.src2_en            = 1;
    node->vau_src2_ctrl.bits.src2_fmt           = surface->fmt;
    node->vau_src2_ctrl.bits.src2_argb_order    = (surface->fmt == DRV_GFX_FMT_CLUT4 ||
        surface->fmt == DRV_GFX_FMT_CLUT8) ? DRV_GFX_FMT_ORDER_ABGR : surface->argb_order;
    node->vau_src2_ctrl.bits.src2_rgb_exp       = DRV_VAU_RGB_EXP_HIGH_BITS;
    node->vau_src2_ctrl.bits.src2_alpha_range   = surface->alpha_info.is_alpha_max_128;

    node->vau_src2_addr.bits.src2_addr          = surface->phy_addr;
    node->vau_src2_stride.bits.src2_stride      = surface->stride;
#ifdef GPU_3322
    node->vau_src2c_addr.bits.src2c_addr        = surface->phy_addr + surface->uv_offset;
    node->vau_src2_ctrl.bits.src2_cbcr_order    = 0x0;
    node->vau_src2c_stride.bits.src2c_stride    = surface->stride;
#endif
    node->vau_src2_imgsize.bits.src2_width      = surface->width - 1;
    node->vau_src2_imgsize.bits.src2_height     = surface->height - 1;

    if (surface->is_color_surface == TD_TRUE) {
        node->vau_src2_fill.bits.src2_color_fill = surface->color;
    }

    /* fdr crop */
    node->vau_src2_rect_xy.bits.src2_rect_x      = (td_u16)surface->rect.x;
    node->vau_src2_rect_xy.bits.src2_rect_y      = (td_u16)surface->rect.y;
    node->vau_src2_rect_wh.bits.src2_rect_width  = surface->rect.width - 1;
    node->vau_src2_rect_wh.bits.src2_rect_height = surface->rect.height - 1;

    /* cmp info config */
    if (surface->compress_info.compress_mode == DRV_GFX_COMPRESS_HIMC) {
        td_s32 ret;
        node->vau_src2_ctrl.bits.src2_ice_en       = 1;
        node->vau_src2_ctrl.bits.src2_cmp_mode     = 0; // offline mode
        ret = memcpy_s((td_void *)&node->vau_src2_dcmp_cfg0, sizeof(node->vau_src2_dcmp_cfg0) * 4, // 4:index
                       (td_void *)(uintptr_t)surface->cmp_addr, DRV_VAU_CMP_HEAD_SIZE);
        if (ret != EOK) {
            gpu_err("compress_info copy fail!\n");
        }
    }

    if (surface->fmt == DRV_GFX_FMT_A1 || surface->fmt == DRV_GFX_FMT_A1B || surface->fmt == DRV_GFX_FMT_A2 ||
        surface->fmt == DRV_GFX_FMT_A4 || surface->fmt == DRV_GFX_FMT_A2B || surface->fmt == DRV_GFX_FMT_A4B) {
        node->vau_src2_pix_rgb.bits.src2_pix_rgb = surface->color;
        node->vau_src2_alpha.bits.src2_alpha0 = 0x00; /* decide A1 alpha pix=0 */
        node->vau_src2_alpha.bits.src2_alpha1 = 0xff; /* decide A1 alpha pix=1 */
    } else if (surface->fmt == DRV_GFX_FMT_A8) {
        node->vau_src2_pix_rgb.bits.src2_pix_rgb = surface->color;
    }

    if ((surface->compress_info.compress_mode == DRV_GFX_COMPRESS_HFBC_2X) ||
        (surface->compress_info.compress_mode == DRV_GFX_COMPRESS_HFBC_3X)) {
        vau_hal_node_set_online_cmp_info(node, surface);
    }

    if (surface->hsv_info.hvs_en) {
        vau_hal_node_set_hsv_info(node, surface);
    }
}

td_void vau_hal_node_set_src2_mode(vau_hardware_node *node, td_u32 mode)
{
    node->vau_src2_ctrl.bits.src2_mode = mode;
}

td_void vau_hal_node_set_src2_alpha(vau_hardware_node *node, const drv_vau_surface *src2)
{
    if (src2->fmt == DRV_GFX_FMT_ARGB1555) {
        node->vau_src2_alpha.bits.src2_alpha0 = src2->alpha_info.alpha0;
        node->vau_src2_alpha.bits.src2_alpha1 = src2->alpha_info.alpha1;
    }

    if (src2->attr.affine.affine_en) {
        /* matrix */
        node->vau_src2_ctrl.bits.src2_premult_en        = src2->alpha_info.premult_en;
    } else {
        /* s2_blend_mode config in cbm */
        node->vau_cbm_src2_cbm_para.bits.s2_premult_en  = src2->alpha_info.premult_en;
    }
    node->vau_cbm_src2_cbm_para.bits.s2_palpha_en       = src2->alpha_info.pixel_alpha_en;
    node->vau_cbm_src2_cbm_para.bits.s2_galpha          = src2->alpha_info.global_alpha;
    node->vau_cbm_src2_cbm_para.bits.s2_multi_global_en = src2->alpha_info.golbal_premult_en;
    node->vau_cbm_src2_cbm_para.bits.s2_galpha_en       = src2->alpha_info.global_alpha_en;
}

td_void vau_hal_node_set_src2_clut(vau_hardware_node *node, const drv_vau_surface *surface)
{
    if ((surface->fmt >= DRV_GFX_FMT_CLUT1) && (surface->fmt <= DRV_GFX_FMT_CLUT4B)) {
        node->vau_clut_addr.u32 = surface->clut_addr;
    }
}

td_void vau_hal_node_set_affine_info(vau_hardware_node *node, vau_affine_cfg *hal_cfg, td_u32 shift)
{
    node->vau_src2_ctrl.bits.src2_mapping_en = 1;
    node->mix.mapp.vau_mapping_ctrl.bits.mapping_en = 1;
    node->mix.mapp.vau_mapping_ctrl.bits.mapping_mode = hal_cfg->transform_mode;
    node->mix.mapp.vau_mapping_ctrl.bits.req_tile_sel = 1; // 16x4 tile for improving performance
    node->mix.mapp.vau_mapping_ctrl.bits.alpha_mapping_mode = 1;
    node->mix.mapp.vau_mapping_ctrl.bits.tile_linear_sel    = 0;
    node->mix.mapp.vau_mapping_ctrl.bits.pattern_cfg  = hal_cfg->pattern_mode;
    node->mix.mapp.vau_mapping_pattern_fill_color.u32 = hal_cfg->pattern_argb;

    node->mix.mapp.vau_mapping_ctrl.bits.func_sel = 1;

    if (hal_cfg->pattern_mode == DRV_VAU_PATTERN_REPEAT) {
        hal_cfg->inv_m[0][0] = hal_cfg->inv_m[0][0] / (hal_cfg->src_width  << shift);
        hal_cfg->inv_m[0][1] = hal_cfg->inv_m[0][1] / (hal_cfg->src_width  << shift);
        hal_cfg->inv_m[0][2] = hal_cfg->inv_m[0][2] / (hal_cfg->src_width  << shift); /* 2:index */
        hal_cfg->inv_m[1][0] = hal_cfg->inv_m[1][0] / (hal_cfg->src_height << shift);
        hal_cfg->inv_m[1][1] = hal_cfg->inv_m[1][1] / (hal_cfg->src_height << shift);
        hal_cfg->inv_m[1][2] = hal_cfg->inv_m[1][2] / (hal_cfg->src_height << shift); /* 2:index */
    } else if (hal_cfg->pattern_mode == DRV_VAU_PATTERN_REFLECT) {
        hal_cfg->inv_m[0][0] = hal_cfg->inv_m[0][0] / (hal_cfg->src_width  << (1 + shift));
        hal_cfg->inv_m[0][1] = hal_cfg->inv_m[0][1] / (hal_cfg->src_width  << (1 + shift));
        hal_cfg->inv_m[0][2] = hal_cfg->inv_m[0][2] / (hal_cfg->src_width  << (1 + shift)); /* 2:index */
        hal_cfg->inv_m[1][0] = hal_cfg->inv_m[1][0] / (hal_cfg->src_height << (1 + shift));
        hal_cfg->inv_m[1][1] = hal_cfg->inv_m[1][1] / (hal_cfg->src_height << (1 + shift));
        hal_cfg->inv_m[1][2] = hal_cfg->inv_m[1][2] / (hal_cfg->src_height << (1 + shift)); /* 2:index */
    }

    node->mix.mapp.vau_mapping_coef00.bits.mapping_coef00 = hal_cfg->inv_m[0][0];
    node->mix.mapp.vau_mapping_coef01.bits.mapping_coef01 = hal_cfg->inv_m[0][1];
    node->mix.mapp.vau_mapping_coef02.bits.mapping_coef02 = hal_cfg->inv_m[0][2]; /* 2:index */
    node->mix.mapp.vau_mapping_coef10.bits.mapping_coef10 = hal_cfg->inv_m[1][0];
    node->mix.mapp.vau_mapping_coef11.bits.mapping_coef11 = hal_cfg->inv_m[1][1];
    node->mix.mapp.vau_mapping_coef12.bits.mapping_coef12 = hal_cfg->inv_m[1][2]; /* 2:index */
    node->mix.mapp.vau_mapping_coef20.bits.mapping_coef20 = hal_cfg->inv_m[2][0] / (1  << shift); /* 2:index */
    node->mix.mapp.vau_mapping_coef21.bits.mapping_coef21 = hal_cfg->inv_m[2][1] / (1  << shift); /* 2:index */
    node->mix.mapp.vau_mapping_coef22.bits.mapping_coef22 = hal_cfg->inv_m[2][2] / (1  << shift); /* 2:index */

    return;
}

td_void vau_hal_node_set_dst_info(vau_hardware_node *node, const drv_vau_surface *surface)
{
    node->vau_des_ctrl.bits.des_en             = 1;
    node->vau_des_ctrl.bits.des_fmt            = surface->fmt;
    node->vau_des_ctrl.bits.des_argb_order     = surface->argb_order;
    node->vau_des_ctrl.bits.des_alpha_range    = surface->alpha_info.is_alpha_max_128;
    node->vau_des_ctrl.bits.des_rgb_trunc      = (surface->fmt == DRV_GFX_FMT_RGB565) ? 1 : 0;

    node->vau_des_addr.bits.des_addr           = surface->phy_addr;
    node->vau_des_stride.bits.des_stride       = surface->stride;
    node->vau_des_imgsize.bits.des_width       = surface->width - 1;
    node->vau_des_imgsize.bits.des_height      = surface->height - 1;

    node->vau_des_alpha.bits.des_alpha_thd     = surface->alpha_info.alpha_thd;

    /* dst rect */
    node->vau_des_rect_xy.bits.des_rect_x      = (td_u32)surface->rect.x;
    node->vau_des_rect_xy.bits.des_rect_y      = (td_u32)surface->rect.y;
    node->vau_des_rect_wh.bits.des_rect_width  = surface->rect.width - 1;
    node->vau_des_rect_wh.bits.des_rect_height = surface->rect.height - 1;

    if ((surface->compress_info.compress_mode == DRV_GFX_COMPRESS_HFBC_2X) ||
        (surface->compress_info.compress_mode == DRV_GFX_COMPRESS_HFBC_3X)) {
        node->vau_des_ctrl.bits.des_ice_en = 1;
        if (surface->fmt == DRV_GFX_FMT_RGB888) {
            node->vau_des_htc_online_cmp0.bits.pix_format = 0;
            node->vau_des_htc_online_cmp0.bits.des_block_size =
                (surface->compress_info.compress_mode == DRV_GFX_COMPRESS_HFBC_2X) ? 24 : 16; // 24,16：block_size
        } else {
            node->vau_des_htc_online_cmp0.bits.pix_format = 1;
            node->vau_des_htc_online_cmp0.bits.des_block_size =
                (surface->compress_info.compress_mode == DRV_GFX_COMPRESS_HFBC_2X) ? 32 : 24; // 24,32：block_size
        }

        node->vau_des_htc_online_cmp0.bits.bypass_en  = 0;
        node->vau_des_htc_online_cmp0.bits.bit_depth  = 0;
        node->vau_des_htc_online_cmp0.bits.cmp_pq_sel =
            (surface->compress_info.compress_mode == DRV_GFX_COMPRESS_HFBC_2X) ? 0 : 1;
    }
}

td_void vau_hal_node_set_cbm(vau_hardware_node *node, td_u32 alu_mode)
{
    if (node->vau_src1_ctrl.bits.src1_mode == DRV_VAU_MODE_DMA) {
        return;
    }

    node->vau_cbm_ctrl.bits.cbm_en   = 1;
    node->vau_cbm_ctrl.bits.alu_mode = alu_mode;
}

td_void vau_hal_node_set_rop(vau_hardware_node *node, const drv_vau_rop_opt *rop_opt)
{
    node->vau_cbm_alu_para.bits.blend_rop_en = rop_opt->rop_enable;
    node->vau_cbm_alu_para.bits.a_rop        = rop_opt->alpha_rop_mode;
    node->vau_cbm_alu_para.bits.rgb_rop      = rop_opt->color_rop_mode;
}

td_void vau_hal_node_set_colorkey(vau_hardware_node *node, hal_vau_colorkey_opt *colorkey_opt)
{
    node->vau_cbm_key_para.bits.key_en  = colorkey_opt->key_en;
    node->vau_cbm_key_para.bits.key_sel = colorkey_opt->key_sel;

    node->vau_cbm_key_para.bits.key_a_mode = colorkey_opt->component[HAL_VAU_COMPONENT_ALPHA].key_mode;
    node->vau_cbm_key_min.bits.key_a_min   = colorkey_opt->component[HAL_VAU_COMPONENT_ALPHA].key_min;
    node->vau_cbm_key_max.bits.key_a_max   = colorkey_opt->component[HAL_VAU_COMPONENT_ALPHA].key_max;
    node->vau_cbm_key_mask.bits.key_a_mask = colorkey_opt->component[HAL_VAU_COMPONENT_ALPHA].key_mask;

    node->vau_cbm_key_para.bits.key_r_mode = colorkey_opt->component[HAL_VAU_COMPONENT_RED].key_mode;
    node->vau_cbm_key_min.bits.key_r_min   = colorkey_opt->component[HAL_VAU_COMPONENT_RED].key_min;
    node->vau_cbm_key_max.bits.key_r_max   = colorkey_opt->component[HAL_VAU_COMPONENT_RED].key_max;
    node->vau_cbm_key_mask.bits.key_r_mask = colorkey_opt->component[HAL_VAU_COMPONENT_RED].key_mask;

    node->vau_cbm_key_para.bits.key_g_mode = colorkey_opt->component[HAL_VAU_COMPONENT_GREEN].key_mode;
    node->vau_cbm_key_min.bits.key_g_min   = colorkey_opt->component[HAL_VAU_COMPONENT_GREEN].key_min;
    node->vau_cbm_key_max.bits.key_g_max   = colorkey_opt->component[HAL_VAU_COMPONENT_GREEN].key_max;
    node->vau_cbm_key_mask.bits.key_g_mask = colorkey_opt->component[HAL_VAU_COMPONENT_GREEN].key_mask;

    node->vau_cbm_key_para.bits.key_b_mode = colorkey_opt->component[HAL_VAU_COMPONENT_BLUE].key_mode;
    node->vau_cbm_key_min.bits.key_b_min   = colorkey_opt->component[HAL_VAU_COMPONENT_BLUE].key_min;
    node->vau_cbm_key_max.bits.key_b_max   = colorkey_opt->component[HAL_VAU_COMPONENT_BLUE].key_max;
    node->vau_cbm_key_mask.bits.key_b_mask = colorkey_opt->component[HAL_VAU_COMPONENT_BLUE].key_mask;
}

td_void vau_hal_node_set_blend_mode(vau_hardware_node *node, td_u32 src1_mode, td_u32 src2_mode)
{
    node->vau_cbm_src1_cbm_para.bits.s1_blend_mode = src1_mode;
    node->vau_cbm_src2_cbm_para.bits.s2_blend_mode = src2_mode;
}

td_void vau_hal_node_set_blend_alpha_mode(vau_hardware_node *node, td_u32 alpha_from)
{
    node->vau_cbm_alu_para.bits.alpha_from = alpha_from;
}

td_void vau_hal_node_enable_complete_int(vau_hardware_node *node, td_bool is_offline)
{
    if (is_offline) {
        node->vau_intmask.bits.list_end_mask           = 0x1;
    } else {
        node->vau_intmask.bits.node_end_mask           = 0x1;
        node->vau_intmask.bits.msubnode_end_mask       = 0x1;
    }

    node->vau_intmask.bits.timeout_mask            = 0x1;
    node->vau_intmask.bits.bus_err_mask            = 0x1;
#ifndef CONFIG_VAU_DEAL_INT_ONCE
    node->vau_intmask.bits.draw_err_mask           = 0x1;
    node->vau_intmask.bits.et_oversize_mask        = 0x1;
    node->vau_intmask.bits.draw_oversize_mask      = 0x1;
    node->vau_intmask.bits.dcmp_cfg_err_mask       = 0x1;
    node->vau_intmask.bits.dcmp_bs_err_mask        = 0x1;
#else
    node->vau_intmask.bits.once_draw_err_mask      = 0x1;
    node->vau_intmask.bits.once_et_oversize_mask   = 0x1;
    node->vau_intmask.bits.once_draw_oversize_mask = 0x1;
    node->vau_intmask.bits.once_dcmp_cfg_err_mask  = 0x1;
    node->vau_intmask.bits.once_dcmp_bs_err_mask   = 0x1;
#endif
}

td_void vau_hal_node_add_next_node(vau_hardware_node *node, td_u32 phy_addr)
{
    node->vau_pnext.bits.p_next = phy_addr;
}

td_void vau_hal_node_set_alpha_strb(vau_hardware_node *node, td_bool enable)
{
    node->vau_des_ctrl.bits.alpha_strb_en = enable;
    return;
}

STATIC td_void hal_node_set_mapping_ctrl(vau_hardware_node *node, const drv_vau_surface *src_surface)
{
    node->vau_src2_ctrl.bits.src2_mapping_en = 0x1;
    node->mix.mapp.vau_mapping_ctrl.bits.mapping_en = 0x1;

    if (src_surface->threed_attr.mapping_color == DRV_SAMPLING_BILINEAR) {
        node->mix.mapp.vau_mapping_ctrl.bits.mapping_mode = 0x1;
    }

    if (src_surface->threed_attr.mapping_alpha == DRV_SAMPLING_DIRECT) {
        node->mix.mapp.vau_mapping_ctrl.bits.alpha_mapping_mode = 0;
    }

    node->mix.mapp.vau_mapping_ctrl.bits.upsampling_inter_sel_flag = src_surface->threed_attr.model_flag;
    node->mix.mapp.vau_mapping_ctrl.bits.upsampling_inter_sel_pos = src_surface->threed_attr.model_pos;

    node->mix.mapp.vau_mapping_ctrl.bits.pattern_cfg = src_surface->attr.pattern.mode;

    switch (src_surface->mapping_attr.mapping_mode) {
        case DRV_MAPPING_OPT_MATRIX:
            node->mix.mapp.vau_mapping_ctrl.bits.func_sel = 0x1;
            break;
        case DRV_MAPPING_OPT_MESH:
            node->mix.mapp.vau_mapping_ctrl.bits.func_sel = 0x2;
            break;
        case DRV_MAPPING_OPT_SPHERE:
            node->mix.mapp.vau_mapping_ctrl.bits.func_sel = 0x3;
            break;
        case DRV_MAPPING_OPT_PLANE:
            node->mix.mapp.vau_mapping_ctrl.bits.func_sel = 0x4;
            break;
        default:
            break;
    }
    return;
}

td_void vau_hal_node_set_mapping(vau_hardware_node *node, const drv_vau_surface *src_surface)
{
    if (src_surface->mapping_attr.mapping_en == TD_FALSE) {
        return;
    }

    hal_node_set_mapping_ctrl(node, src_surface);

    node->mix.mapp.vau_mapping_pattern_fill_color.u32 = src_surface->attr.pattern.argb;

    node->mix.mapp.vau_mapping_coef00.bits.mapping_coef00 = src_surface->attr.mapping_matrix[0][0];
    node->mix.mapp.vau_mapping_coef01.bits.mapping_coef01 = src_surface->attr.mapping_matrix[0][1];
    node->mix.mapp.vau_mapping_coef02.bits.mapping_coef02 = src_surface->attr.mapping_matrix[0][2]; /* 2：index */
    node->mix.mapp.vau_mapping_coef03.bits.mapping_coef03 = src_surface->attr.mapping_matrix[0][3]; /* 3：index */
    node->mix.mapp.vau_mapping_coef10.bits.mapping_coef10 = src_surface->attr.mapping_matrix[1][0];
    node->mix.mapp.vau_mapping_coef11.bits.mapping_coef11 = src_surface->attr.mapping_matrix[1][1];
    node->mix.mapp.vau_mapping_coef12.bits.mapping_coef12 = src_surface->attr.mapping_matrix[1][2]; /* 2：index */
    node->mix.mapp.vau_mapping_coef13.bits.mapping_coef13 = src_surface->attr.mapping_matrix[1][3]; /* 3：index */
    node->mix.mapp.vau_mapping_coef20.bits.mapping_coef20 = src_surface->attr.mapping_matrix[2][0]; /* 2：index */
    node->mix.mapp.vau_mapping_coef21.bits.mapping_coef21 = src_surface->attr.mapping_matrix[2][1]; /* 2：index */
    node->mix.mapp.vau_mapping_coef22.bits.mapping_coef22 = src_surface->attr.mapping_matrix[2][2]; /* 2：index */
    node->mix.mapp.vau_mapping_coef23.bits.mapping_coef23 = src_surface->attr.mapping_matrix[2][3]; /* 2，3：index */
    node->mix.mapp.vau_mapping_coef30.bits.mapping_coef30 = src_surface->attr.mapping_matrix[3][0]; /* 3：index */
    node->mix.mapp.vau_mapping_coef31.bits.mapping_coef31 = src_surface->attr.mapping_matrix[3][1]; /* 3：index */
    node->mix.mapp.vau_mapping_coef32.bits.mapping_coef32 = src_surface->attr.mapping_matrix[3][2]; /* 2，3：index */
    node->mix.mapp.vau_mapping_coef33.bits.mapping_coef33 = src_surface->attr.mapping_matrix[3][3]; /* 3，3：index */

#ifndef GPU_1A22
    if (src_surface->threed_attr.enable == TD_FALSE) {
        return;
    }

    node->mix.mapp.vau_mapping_shadow_color.u32 = src_surface->threed_attr.shadow_color;
    node->mix.mapp.vau_mapping_bg_color.u32 = src_surface->threed_attr.bg_color;

    node->mix.mapp.vau_mapping_scale_x.bits.x_scale = src_surface->threed_attr.scale_x;
    node->mix.mapp.vau_mapping_scale_y.bits.y_scale = src_surface->threed_attr.scale_y;
    node->mix.mapp.vau_mapping_offset_x.bits.x_offset = src_surface->threed_attr.offset_x;
    node->mix.mapp.vau_mapping_offset_y.bits.y_offset = src_surface->threed_attr.offset_y;
    node->mix.mapp.vau_mapping_src_ratio_x.bits.src_x_ratio = src_surface->threed_attr.src_ration_x;
    node->mix.mapp.vau_mapping_src_ratio_y.bits.src_y_ratio = src_surface->threed_attr.src_ration_y;
    node->mix.mapp.vau_mapping_src_offset_x.bits.src_x_offset = src_surface->threed_attr.src_offset_x;
    node->mix.mapp.vau_mapping_src_offset_y.bits.src_y_offset = src_surface->threed_attr.src_offset_y;

    node->vau_model_ctrl.bits.model_en = src_surface->threed_attr.enable;
    node->vau_model_addr.bits.model_addr = src_surface->threed_attr.model_sur.phy_addr;
    node->vau_model_stride.bits.model_stride = src_surface->threed_attr.model_sur.stride;
    node->mix.mapp.vau_mapping_model_wh.bits.mapping_model_w = src_surface->threed_attr.model_sur.width - 1;
    node->mix.mapp.vau_mapping_model_wh.bits.mapping_model_h = src_surface->threed_attr.model_sur.height - 1;
#endif
    return;
}
