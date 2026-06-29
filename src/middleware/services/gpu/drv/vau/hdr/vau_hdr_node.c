/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: hdr process file
 */

#include "vau_hdr_node.h"
#include <securec.h>
#include "gpu_mem_config.h"
#include "common_def.h"

#define PQ_COEF_SIZE 64
#define GPU_HDR_SINGLE_COEF_SIZE (PQ_COEF_SIZE * 4)
#define GPU_HDR_GAMMA_ADDR       (GPU_HDR_COEF_ADDR)
#define GPU_HDR_DEGAMMA_ADDR     (GPU_HDR_COEF_ADDR + GPU_HDR_SINGLE_COEF_SIZE)
#define GPU_HDR_TMAP_ADDR        (GPU_HDR_DEGAMMA_ADDR + GPU_HDR_SINGLE_COEF_SIZE)

#if defined(GPU_3322) || defined(GPU_1A22)
STATIC td_void vau_hdr_node_set_hdr_sys_ctrl(vau_hdr_hardware_node *node, td_bool hdr_en, td_bool hdr_cbm_en)
{
    node->vau_hdr_sys_ctrl.bits.hdr_cbm_g0_alpha = 0xFF;
    node->vau_hdr_sys_ctrl.bits.hdr_cbm_g1_alpha = 0xFF;

    node->vau_hdr_sys_ctrl.bits.hdr_en = hdr_en;
    node->vau_hdr_sys_ctrl.bits.hdr_cbm_en = hdr_cbm_en;
    node->vau_hdr_sys_ctrl.bits.start_dly_cycle = 0x10;
    node->vau_hdr_sys_ctrl.bits.hdr_mode = 0x0;
}

STATIC td_void vau_hdr_node_set_hdr_rbcn(vau_hdr_hardware_node *node, const hdr_rcbn_cfg *cfg)
{
    node->hdr.hdr_combine.hdr_rcbn_ctrl.bits.rcbn_en = cfg->cmb_en;
    node->hdr.hdr_combine.hdr_rcbn_ctrl.bits.pow_en = cfg->pow_en;
    node->hdr.hdr_combine.hdr_rcbn_gmap_size_mode.bits.gmap_size_mode = cfg->gainmap_size_mode;
    node->hdr.hdr_combine.hdr_rcbn_gmap_chn_num.bits.gmap_chn_num = cfg->gainmap_components;

    node->hdr.hdr_combine.hdr_rcbn_gcoeff_rmax.u32 = (td_u32)*(td_s32 *)(&cfg->gain_max[0x0]);
    node->hdr.hdr_combine.hdr_rcbn_gcoeff_gmax.u32 = (td_u32)*(td_s32 *)(&cfg->gain_max[0x1]);
    node->hdr.hdr_combine.hdr_rcbn_gcoeff_bmax.u32 = (td_u32)*(td_s32 *)(&cfg->gain_max[0x2]);
    node->hdr.hdr_combine.hdr_rcbn_gcoeff_rmin1.u32 = (td_u32)*(td_s32 *)(&cfg->gain_min1[0x0]);
    node->hdr.hdr_combine.hdr_rcbn_gcoeff_gmin1.u32 = (td_u32)*(td_s32 *)(&cfg->gain_min1[0x1]);
    node->hdr.hdr_combine.hdr_rcbn_gcoeff_bmin1.u32 = (td_u32)*(td_s32 *)(&cfg->gain_min1[0x2]);
    node->hdr.hdr_combine.hdr_rcbn_gcoeff_rmin2.u32 = (td_u32)*(td_s32 *)(&cfg->gain_min2[0x0]);
    node->hdr.hdr_combine.hdr_rcbn_gcoeff_gmin2.u32 = (td_u32)*(td_s32 *)(&cfg->gain_min2[0x1]);
    node->hdr.hdr_combine.hdr_rcbn_gcoeff_bmin2.u32 = (td_u32)*(td_s32 *)(&cfg->gain_min2[0x2]);

    node->hdr.hdr_combine.hdr_rcbn_kcoeff_r_base.u32 = (td_u32)*(td_s32 *)(&cfg->k_base[0x0]);
    node->hdr.hdr_combine.hdr_rcbn_kcoeff_g_base.u32 = (td_u32)*(td_s32 *)(&cfg->k_base[0x1]);
    node->hdr.hdr_combine.hdr_rcbn_kcoeff_b_base.u32 = (td_u32)*(td_s32 *)(&cfg->k_base[0x2]);

    node->hdr.hdr_combine.hdr_rcbn_kcoeff_r_alte.u32 = (td_u32)*(td_s32 *)(&cfg->k_alternate[0x0]);
    node->hdr.hdr_combine.hdr_rcbn_kcoeff_g_alte.u32 = (td_u32)*(td_s32 *)(&cfg->k_alternate[0x1]);
    node->hdr.hdr_combine.hdr_rcbn_kcoeff_b_alte.u32 = (td_u32)*(td_s32 *)(&cfg->k_alternate[0x2]);
    node->hdr.hdr_combine.hdr_rcbn_weight.u32        = (td_u32)*(td_s32 *)(&cfg->weight);

#ifdef GPU_3322
    node->hdr.hdr_combine.hdr_rcbn_clip_min.bits.clip_min = cfg->clip_min;
    node->hdr.hdr_combine.hdr_rcbn_clip_max.bits.clip_max = cfg->clip_max;
    node->hdr.hdr_combine.hdr_rcbn_clip_min.bits.clip_min = cfg->clip_min;
    node->hdr.hdr_combine.hdr_rcbn_clip_max.bits.clip_max = cfg->clip_max;
#endif
}

STATIC td_void vau_hdr_node_set_hdr_hipp_csc(vau_hdr_hardware_node *node, const hdr_csc_cfg *y2r_gainmap_cfg)
{
    node->hdr.hdr_combine.src1_hipp_csc_ctrl.bits.hipp_csc_en = y2r_gainmap_cfg->csc_en;
    node->hdr.hdr_combine.src1_hipp_csc_coef00.bits.hipp_csc_coef00 = y2r_gainmap_cfg->ac_coef[0][0];
    node->hdr.hdr_combine.src1_hipp_csc_coef01.bits.hipp_csc_coef01 = y2r_gainmap_cfg->ac_coef[0][1];
    node->hdr.hdr_combine.src1_hipp_csc_coef02.bits.hipp_csc_coef02 = y2r_gainmap_cfg->ac_coef[0][2]; // 2:index
    node->hdr.hdr_combine.src1_hipp_csc_coef10.bits.hipp_csc_coef10 = y2r_gainmap_cfg->ac_coef[1][0];
    node->hdr.hdr_combine.src1_hipp_csc_coef11.bits.hipp_csc_coef11 = y2r_gainmap_cfg->ac_coef[1][1];
    node->hdr.hdr_combine.src1_hipp_csc_coef12.bits.hipp_csc_coef12 = y2r_gainmap_cfg->ac_coef[1][2]; // 2:index
    node->hdr.hdr_combine.src1_hipp_csc_coef20.bits.hipp_csc_coef20 = y2r_gainmap_cfg->ac_coef[2][0]; // 2:index
    node->hdr.hdr_combine.src1_hipp_csc_coef21.bits.hipp_csc_coef21 = y2r_gainmap_cfg->ac_coef[2][1]; // 2:index
    node->hdr.hdr_combine.src1_hipp_csc_coef22.bits.hipp_csc_coef22 = y2r_gainmap_cfg->ac_coef[2][2]; // 2:index
    node->hdr.hdr_combine.src1_hipp_csc_scale.bits.hipp_csc_scale = y2r_gainmap_cfg->scale;
    node->hdr.hdr_combine.src1_hipp_csc_idc0.bits.hipp_csc_idc0 = y2r_gainmap_cfg->dc_coef_in[0];
    node->hdr.hdr_combine.src1_hipp_csc_idc1.bits.hipp_csc_idc1 = y2r_gainmap_cfg->dc_coef_in[1];
    node->hdr.hdr_combine.src1_hipp_csc_idc2.bits.hipp_csc_idc2 = y2r_gainmap_cfg->dc_coef_in[2]; // 2:index

    node->hdr.hdr_combine.src1_hipp_csc_odc0.bits.hipp_csc_odc0 = y2r_gainmap_cfg->dc_coef_out[0];
    node->hdr.hdr_combine.src1_hipp_csc_odc1.bits.hipp_csc_odc1 = y2r_gainmap_cfg->dc_coef_out[1];
    node->hdr.hdr_combine.src1_hipp_csc_odc2.bits.hipp_csc_odc2 = y2r_gainmap_cfg->dc_coef_out[2]; // 2:index
    node->hdr.hdr_combine.src1_hipp_csc_min_y.bits.hipp_csc_min_y = y2r_gainmap_cfg->clip_min;
    node->hdr.hdr_combine.src1_hipp_csc_min_c.bits.hipp_csc_min_c = y2r_gainmap_cfg->clip_min;
    node->hdr.hdr_combine.src1_hipp_csc_max_y.bits.hipp_csc_max_y = y2r_gainmap_cfg->clip_max;
    node->hdr.hdr_combine.src1_hipp_csc_max_c.bits.hipp_csc_max_c = y2r_gainmap_cfg->clip_max;
}

STATIC td_void vau_hdr_node_set_hdr_cvfir(vau_hdr_hardware_node *node, const drv_vau_surface *surface)
{
    node->hdr.hdr_combine.hdr_vau_cvfir_vinfo.bits.out_pro = 1;
    node->hdr.hdr_combine.hdr_vau_cvfir_vinfo.bits.out_fmt = 1;
    node->hdr.hdr_combine.hdr_vau_cvfir_vinfo.bits.out_height = surface->height - 1;
    node->hdr.hdr_combine.hdr_vau_cvfir_vsp.bits.cvfir_en = 1;
    node->hdr.hdr_combine.hdr_vau_cvfir_vsp.bits.cvmid_en = 0;
    node->hdr.hdr_combine.hdr_vau_cvfir_vsp.bits.cvfir_mode = 1;
    node->hdr.hdr_combine.hdr_vau_cvfir_vsp.bits.vratio = 0x1000;
}

STATIC td_void vau_hdr_node_set_hdr_hfir(vau_hdr_hardware_node *node)
{
    node->hdr.hdr_combine.hdr_vau_hfir_ctrl.bits.hfir_mode = 1;
    node->hdr.hdr_combine.hdr_vau_hfir_ctrl.bits.hfir_en = 1;
}

static td_void write_coef(td_u32 addr, td_u32 *result, td_u32 result_size)
{
    for (td_u32 i = 0; i < result_size; i++) {
        uapi_reg_write(addr + (sizeof(td_u32) * i), result[i]);
    }
}

#define LINE_SIZE 16
static td_void set_coef(td_u32 addr, const td_u32 *data, td_u32 data_size, td_u32 bit_len)
{
    td_u32 i;
    td_u32 j = 0;
    td_u32 used_len = 0;
    td_u32 tmp_addr = addr;
    td_u32 result[0x4] = { 0 };

    for (i = 0; i < data_size; i++) {
        result[j] |= (data[i] << (used_len % 32)); /* unsigned int contains 32 bits */
        if ((used_len % 32 + bit_len) >= 32) {     /* unsigned int contains 32 bits */
            j++;
        }
        if ((used_len % 32 + bit_len) > 32 && (j < 0x4)) {    /* unsigned int contains 32 bits */
            result[j] |= (data[i] >> (32 - (used_len % 32))); /* unsigned int contains 32 bits */
        }

        used_len += bit_len;

        if (used_len + bit_len > 128 || i == (data_size - 1)) { /* 4 x 32 bits, line size is 128 bits */
            write_coef(tmp_addr, result, LINE_SIZE / sizeof(td_u32));
            memset_s(result, sizeof(result), 0, LINE_SIZE);
            j = 0;
            used_len = 0;
            tmp_addr += LINE_SIZE;
        }
    }
}

STATIC td_void vau_hdr_node_set_hdr_vdm_y2r(vau_hdr_hardware_node *node, const hdr_csc_cfg *y2r_cfg)
{
    node->mix.semi.src2_yuv2rgb_ctrl.bits.vdm_y2r_en    = y2r_cfg->csc_en;
    node->mix.semi.src2_yuv2rgb_ctrl.bits.vdm_v0_csc_en   = y2r_cfg->csc_en;
    node->mix.semi.src2_yuv2rgb_ctrl.bits.vdm_v0_gamma_en = y2r_cfg->csc_en;

    node->hdr.hdr_combine.src2_yuv2rgb_coef00.bits.vdm_00_yuv2rgb          = y2r_cfg->ac_coef[0][0];
    node->hdr.hdr_combine.src2_yuv2rgb_coef01.bits.vdm_01_yuv2rgb          = y2r_cfg->ac_coef[0][1];
    node->hdr.hdr_combine.src2_yuv2rgb_coef02.bits.vdm_02_yuv2rgb          = y2r_cfg->ac_coef[0][2]; // 2:index
    node->hdr.hdr_combine.src2_yuv2rgb_coef03.bits.vdm_03_yuv2rgb          = y2r_cfg->ac_coef[1][0];
    node->hdr.hdr_combine.src2_yuv2rgb_coef04.bits.vdm_04_yuv2rgb          = y2r_cfg->ac_coef[1][1];
    node->hdr.hdr_combine.src2_yuv2rgb_coef05.bits.vdm_05_yuv2rgb          = y2r_cfg->ac_coef[1][2]; // 2:index
    node->hdr.hdr_combine.src2_yuv2rgb_coef06.bits.vdm_06_yuv2rgb          = y2r_cfg->ac_coef[2][0]; // 2:index
    node->hdr.hdr_combine.src2_yuv2rgb_coef07.bits.vdm_07_yuv2rgb          = y2r_cfg->ac_coef[2][1]; // 2:index
    node->hdr.hdr_combine.src2_yuv2rgb_coef08.bits.vdm_08_yuv2rgb          = y2r_cfg->ac_coef[2][2]; // 2:index
    node->hdr.hdr_combine.src2_yuv2rgb_scale2p.bits.vdm_yuv2rgbscale2p     = y2r_cfg->scale;
    node->hdr.hdr_combine.src2_yuv2rgb_in_dc00.bits.vdm_yuv2rgb_00_in_dc   = y2r_cfg->dc_coef_in[0];
    node->hdr.hdr_combine.src2_yuv2rgb_in_dc01.bits.vdm_yuv2rgb_01_in_dc   = y2r_cfg->dc_coef_in[1];
    node->hdr.hdr_combine.src2_yuv2rgb_in_dc02.bits.vdm_yuv2rgb_02_in_dc   = y2r_cfg->dc_coef_in[2]; // 2:index

    node->hdr.hdr_combine.src2_yuv2rgb_out_dc00.bits.vdm_yuv2rgb_00_out_dc = y2r_cfg->dc_coef_out[0];
    node->hdr.hdr_combine.src2_yuv2rgb_out_dc01.bits.vdm_yuv2rgb_01_out_dc = y2r_cfg->dc_coef_out[1];
    node->hdr.hdr_combine.src2_yuv2rgb_out_dc02.bits.vdm_yuv2rgb_02_out_dc = y2r_cfg->dc_coef_out[2]; // 2:index
    node->hdr.hdr_combine.src2_yuv2rgb_min.bits.vdm_yuv2rgb_clip_min       = y2r_cfg->clip_min;
    node->hdr.hdr_combine.src2_yuv2rgb_max.bits.vdm_yuv2rgb_clip_max       = y2r_cfg->clip_max;
}

STATIC td_void vau_hdr_node_set_hdr_vdm_gm1(vau_hdr_hardware_node *node, const hdr_csc_cfg *gm1_cfg)
{
    node->hdr.hdr_combine.hdr_vdm_csc_ctrl.bits.vdm_csc_en           = gm1_cfg->csc_en;
    node->hdr.hdr_combine.hdr_vdm_csc_coef00.bits.vdm_coef00_csc     = gm1_cfg->ac_coef[0][0];
    node->hdr.hdr_combine.hdr_vdm_csc_coef01.bits.vdm_coef01_csc     = gm1_cfg->ac_coef[0][1];
    node->hdr.hdr_combine.hdr_vdm_csc_coef02.bits.vdm_coef02_csc     = gm1_cfg->ac_coef[0][2]; // 2:index
    node->hdr.hdr_combine.hdr_vdm_csc_coef10.bits.vdm_coef10_csc     = gm1_cfg->ac_coef[1][0];
    node->hdr.hdr_combine.hdr_vdm_csc_coef11.bits.vdm_coef11_csc     = gm1_cfg->ac_coef[1][1];
    node->hdr.hdr_combine.hdr_vdm_csc_coef12.bits.vdm_coef12_csc     = gm1_cfg->ac_coef[1][2]; // 2:index
    node->hdr.hdr_combine.hdr_vdm_csc_coef20.bits.vdm_coef20_csc     = gm1_cfg->ac_coef[2][0]; // 2:index
    node->hdr.hdr_combine.hdr_vdm_csc_coef21.bits.vdm_coef21_csc     = gm1_cfg->ac_coef[2][1]; // 2:index
    node->hdr.hdr_combine.hdr_vdm_csc_coef22.bits.vdm_coef22_csc     = gm1_cfg->ac_coef[2][2]; // 2:index

#ifdef GPU_3322
    node->hdr.hdr_combine.hdr_vdm_csc_scale.bits.vdm_csc_scale       = gm1_cfg->scale;
    node->hdr.hdr_combine.hdr_vdm_csc_clip_min.bits.vdm_csc_clip_min = gm1_cfg->clip_min;
    node->hdr.hdr_combine.hdr_vdm_csc_clip_max.bits.vdm_csc_clip_max = gm1_cfg->clip_max;
    node->hdr.hdr_combine.hdr_vdm_csc_in_dc0.bits.vdm_csc_0_in_dc    = gm1_cfg->dc_coef_in[0];
    node->hdr.hdr_combine.hdr_vdm_csc_in_dc1.bits.vdm_csc_1_in_dc    = gm1_cfg->dc_coef_in[1];
    node->hdr.hdr_combine.hdr_vdm_csc_in_dc2.bits.vdm_csc_2_in_dc    = gm1_cfg->dc_coef_in[2];  // 2:index
    node->hdr.hdr_combine.hdr_vdm_csc_out_dc0.bits.vdm_csc_0_out_dc  = gm1_cfg->dc_coef_out[0];
    node->hdr.hdr_combine.hdr_vdm_csc_out_dc1.bits.vdm_csc_1_out_dc  = gm1_cfg->dc_coef_out[1];
    node->hdr.hdr_combine.hdr_vdm_csc_out_dc2.bits.vdm_csc_2_out_dc  = gm1_cfg->dc_coef_out[2]; // 2:index
#endif
}

STATIC td_void vau_hdr_node_set_hdr_vdm_gm2(vau_hdr_hardware_node *node, const hdr_csc_cfg *gm2_cfg)
{
    node->hdr.hdr_combine.hdr_vdm_csc1_ctrl.bits.vdm_csc1_en           = gm2_cfg->csc_en;
    node->hdr.hdr_combine.hdr_vdm_csc1_coef00.bits.vdm_coef00_csc1     = gm2_cfg->ac_coef[0][0];
    node->hdr.hdr_combine.hdr_vdm_csc1_coef01.bits.vdm_coef01_csc1     = gm2_cfg->ac_coef[0][1];
    node->hdr.hdr_combine.hdr_vdm_csc1_coef02.bits.vdm_coef02_csc1     = gm2_cfg->ac_coef[0][2]; // 2:index
    node->hdr.hdr_combine.hdr_vdm_csc1_coef10.bits.vdm_coef10_csc1     = gm2_cfg->ac_coef[1][0];
    node->hdr.hdr_combine.hdr_vdm_csc1_coef11.bits.vdm_coef11_csc1     = gm2_cfg->ac_coef[1][1];
    node->hdr.hdr_combine.hdr_vdm_csc1_coef12.bits.vdm_coef12_csc1     = gm2_cfg->ac_coef[1][2]; // 2:index
    node->hdr.hdr_combine.hdr_vdm_csc1_coef20.bits.vdm_coef20_csc1     = gm2_cfg->ac_coef[2][0]; // 2:index
    node->hdr.hdr_combine.hdr_vdm_csc1_coef21.bits.vdm_coef21_csc1     = gm2_cfg->ac_coef[2][1]; // 2:index
    node->hdr.hdr_combine.hdr_vdm_csc1_coef22.bits.vdm_coef22_csc1     = gm2_cfg->ac_coef[2][2]; // 2:index

#ifdef GPU_3322
    node->hdr.hdr_combine.hdr_vdm_csc1_scale.bits.vdm_csc1_scale       = gm2_cfg->scale;
    node->hdr.hdr_combine.hdr_vdm_csc1_clip_min.bits.vdm_csc1_clip_min = gm2_cfg->clip_min;
    node->hdr.hdr_combine.hdr_vdm_csc1_clip_max.bits.vdm_csc1_clip_max = gm2_cfg->clip_max;
    node->hdr.hdr_combine.hdr_vdm_csc1_in_dc0.bits.vdm_csc1_0_in_dc    = gm2_cfg->dc_coef_in[0];
    node->hdr.hdr_combine.hdr_vdm_csc1_in_dc1.bits.vdm_csc1_1_in_dc    = gm2_cfg->dc_coef_in[1];
    node->hdr.hdr_combine.hdr_vdm_csc1_in_dc2.bits.vdm_csc1_2_in_dc    = gm2_cfg->dc_coef_in[2]; // 2:index
    node->hdr.hdr_combine.hdr_vdm_csc1_out_dc0.bits.vdm_csc1_0_out_dc  = gm2_cfg->dc_coef_out[0];
    node->hdr.hdr_combine.hdr_vdm_csc1_out_dc1.bits.vdm_csc1_1_out_dc  = gm2_cfg->dc_coef_out[1];
    node->hdr.hdr_combine.hdr_vdm_csc1_out_dc2.bits.vdm_csc1_2_out_dc  = gm2_cfg->dc_coef_out[2]; // 2:index
#endif
}

STATIC td_void vau_hdr_node_set_hdr_vdm_gamma(vau_hdr_hardware_node *node, const hdr_gamma_cfg *gamma_cfg)
{
    node->hdr.hdr_combine.hdr_vdm_ctrl.bits.vdm_gmm_en = gamma_cfg->gamma_en;
    node->hdr.hdr_combine.hdr_vdm_para_gmm_addr.bits.vdm_para_gmm_addr = GPU_HDR_GAMMA_ADDR;

    set_coef(node->hdr.hdr_combine.hdr_vdm_para_gmm_addr.bits.vdm_para_gmm_addr, gamma_cfg->gamma_lut,
        sizeof(gamma_cfg->gamma_lut) / sizeof(td_s32), 12); /* gamma bit gap is 12 */
}

STATIC td_void vau_hdr_node_set_hdr_vdm_degamma(vau_hdr_hardware_node *node, const hdr_degamma_cfg *degamma_cfg)
{
    node->hdr.hdr_combine.hdr_vdm_ctrl.bits.vdm_degmm_en = degamma_cfg->degmma_en;
    node->hdr.hdr_combine.hdr_vdm_para_degmm_addr.bits.vdm_para_degmm_addr = GPU_HDR_DEGAMMA_ADDR;

    set_coef(node->hdr.hdr_combine.hdr_vdm_para_degmm_addr.bits.vdm_para_degmm_addr, degamma_cfg->degamma_lut,
             sizeof(degamma_cfg->degamma_lut) / sizeof(td_s32), 21);  /* degamma bit gap is 21 */
}

STATIC td_void vau_hdr_node_set_hdr_vdm_tmap(vau_hdr_hardware_node *node, const hdr_tmap_cfg *tmap_cfg)
{
    node->hdr.hdr_combine.hdr_vdm_ctrl.bits.vdm_tmap_en                        = tmap_cfg->tmap_en;
    node->hdr.hdr_combine.hdr_vdm_para_tmap_addr.bits.vdm_para_tmap_addr       = GPU_HDR_TMAP_ADDR;
    node->hdr.hdr_combine.hdr_vdm_tmap_luma_coef0.bits.vdm_luma_coef0_tmap     = tmap_cfg->tmap_luma_cal[0];
    node->hdr.hdr_combine.hdr_vdm_tmap_luma_coef1.bits.vdm_luma_coef1_tmap     = tmap_cfg->tmap_luma_cal[1];
    node->hdr.hdr_combine.hdr_vdm_tmap_luma_coef2.bits.vdm_luma_coef2_tmap     = tmap_cfg->tmap_luma_cal[2]; // 2:index
#ifdef GPU_3322
    node->hdr.hdr_combine.hdr_vdm_tmap_luma_scale.bits.vdm_tmap_luma_scale     = tmap_cfg->tmap_luma_cal_scale;
    node->hdr.hdr_combine.hdr_vdm_tmap_coef_scale.bits.vdm_tmap_coef_scale     = tmap_cfg->tmap_coef_scale;
    node->hdr.hdr_combine.hdr_vdm_tmap_out_clip_min.bits.vdm_tmap_out_clip_min = tmap_cfg->tmap_out_clip_min;
    node->hdr.hdr_combine.hdr_vdm_tmap_out_clip_max.bits.vdm_tmap_out_clip_max = tmap_cfg->tmap_out_clip_max;
    node->hdr.hdr_combine.hdr_vdm_tmap_out_dc0.bits.vdm_tmap_out_dc0           = tmap_cfg->tmap_out_dc[0];
    node->hdr.hdr_combine.hdr_vdm_tmap_out_dc1.bits.vdm_tmap_out_dc1           = tmap_cfg->tmap_out_dc[1];
    node->hdr.hdr_combine.hdr_vdm_tmap_out_dc2.bits.vdm_tmap_out_dc2           = tmap_cfg->tmap_out_dc[2]; // 2:index

    node->hdr.hdr_combine.hdr_vdm_tonemap_alpha.bits.v_tmap_scale_mix_alpha    = tmap_cfg->tmap_mix_alpha_scale;

    set_coef(node->hdr.hdr_combine.hdr_vdm_para_tmap_addr.bits.vdm_para_tmap_addr, tmap_cfg->tmap_lut,
             sizeof(tmap_cfg->tmap_lut) / sizeof(td_s32), 16); /* tmap bit gap is 16 */
#else // 1A22 TMP no need set_coef
    memcpy_s((void *)GPU_HDR_TMAP_ADDR, sizeof(tmap_cfg->tmap_lut), tmap_cfg->tmap_lut,
             sizeof(td_u32) * TM_LUT_SIZE);
#endif
    node->hdr.hdr_combine.hdr_vdm_tonemap_alpha.bits.v_tmap_mix_alpha          = tmap_cfg->tmap_mix_alpha;
}

STATIC td_void vau_hdr_node_set_hdr_lighting(vau_hdr_hardware_node *node, const drv_vau_lighting_opt *lighting_opt,
    const drv_vau_surface *src2)
{
#ifdef GPU_1A22
    uapi_unused(node);
    uapi_unused(lighting_opt);
    uapi_unused(src2);
#else
    /* common lighting setting */
    node->hdr.hdr_combine.hdr_local_lighting_cfg.bits.local_lighting_en = 1;
    node->hdr.hdr_combine.hdr_local_lighting_cfg.bits.local_lighting_ck_gt_en = 1;
    node->hdr.hdr_combine.hdr_local_lighting_cfg.bits.lighting_mode = (td_u32)lighting_opt->mode;
    node->hdr.hdr_combine.hdr_local_lighting_cfg.bits.is_bi_dir = lighting_opt->attr.is_bi_dir;

    node->hdr.hdr_combine.hdr_local_lighting_cx.bits.centerx = lighting_opt->attr.cx;
    node->hdr.hdr_combine.hdr_local_lighting_cy.bits.centery = lighting_opt->attr.cy;
    node->hdr.hdr_combine.hdr_local_lighting_radius2.bits.radius2 =
        lighting_opt->attr.radius * lighting_opt->attr.radius;
    node->hdr.hdr_combine.hdr_local_lighting_amplitude.bits.amplitude = lighting_opt->attr.spe_amp;
    node->hdr.hdr_combine.hdr_local_lighting_ampli_scale.bits.amplitude_scale =
        lighting_opt->attr.spe_amp / (lighting_opt->attr.radius * lighting_opt->attr.radius);
    node->hdr.hdr_combine.hdr_local_lighting_amb_amp.bits.amb_amplitude = lighting_opt->attr.amb_amp;

    node->hdr.hdr_combine.hdr_local_lighting_start_angle_rad.bits.start_angle_rad = lighting_opt->attr.start_angle_rad;
    node->hdr.hdr_combine.hdr_local_lighting_start_end_angle_rad.bits.start_end_angle_rad =
        lighting_opt->attr.start_end_angle_rad;
    node->hdr.hdr_combine.hdr_local_lighting_end_start_angle_rad.bits.end_start_angle_rad =
        lighting_opt->attr.end_start_angle_rad;
    node->hdr.hdr_combine.hdr_local_lighting_end_angle_rad.bits.end_angle_rad = lighting_opt->attr.end_angle_rad;

    node->hdr.hdr_combine.hdr_local_lighting_triwin2_coeff0.bits.triwin2_coeff0 =
        1 / (lighting_opt->attr.start_end_angle_rad - lighting_opt->attr.start_angle_rad);
    node->hdr.hdr_combine.hdr_local_lighting_triwin2_coeff1.bits.triwin2_coeff1 =
        1 / (lighting_opt->attr.end_angle_rad - lighting_opt->attr.end_start_angle_rad);

    node->hdr.hdr_combine.hdr_local_lighting_bbox0_p0.bits.bbox0_min_x = 0;
    node->hdr.hdr_combine.hdr_local_lighting_bbox0_p0.bits.bbox0_min_y = 0;
    node->hdr.hdr_combine.hdr_local_lighting_bbox0_p1.bits.bbox0_max_x = 599; // 599:max_val
    node->hdr.hdr_combine.hdr_local_lighting_bbox0_p1.bits.bbox0_max_y = 599;  // 599:max_val

    node->hdr.hdr_combine.hdr_local_lighting_bbox1_p0.bits.bbox1_min_x = 0;
    node->hdr.hdr_combine.hdr_local_lighting_bbox1_p0.bits.bbox1_min_y = 0;
    node->hdr.hdr_combine.hdr_local_lighting_bbox1_p1.bits.bbox1_max_x = 599;  // 599:max_val
    node->hdr.hdr_combine.hdr_local_lighting_bbox1_p1.bits.bbox1_max_y = 599;  // 599:max_val

    node->hdr.hdr_combine.hdr_local_lighting_img_reso.bits.image_width = src2->width - 1;
    node->hdr.hdr_combine.hdr_local_lighting_img_reso.bits.image_height = src2->height - 1;

    node->hdr.hdr_combine.hdr_local_lighting_adjust_value.bits.adjust_value = 0.8f;
#endif
}

STATIC td_void vau_hdr_node_set_hdr_cbm(vau_hdr_hardware_node *node, const td_bool sdr_en, const td_bool hdr_en)
{
    node->vau_hdr_sys_ctrl.bits.hdr_cbm_g0_en = sdr_en;
    node->vau_hdr_sys_ctrl.bits.hdr_cbm_g1_en = hdr_en;
    node->vau_hdr_sys_ctrl.bits.hdr_cbm_g0_alpha = 0xFF;
    node->vau_hdr_sys_ctrl.bits.hdr_cbm_g1_alpha = 0xFF;

    /* HDR used as background, SDR used as foreground */
    node->hdr.hdr_cbm.hdr_cbm_mix1.bits.mixer_prio0 = 0x1;
    node->hdr.hdr_cbm.hdr_cbm_mix1.bits.mixer_prio1 = 0x2;
}

STATIC td_void vau_hdr_node_set_hdr_ctadj(vau_hdr_hardware_node *node, const hdr_scale_cfg *scale_cfg)
{
#ifdef GPU_3322
    node->hdr.hdr_cbm.hdr_hipp_ctadj_ctrl.bits.ct_adj_en           = scale_cfg->scale_en;
    node->hdr.hdr_cbm.hdr_hipp_ctadj_gain.bits.ct_adj_gain_r       = scale_cfg->gain[0];
    node->hdr.hdr_cbm.hdr_hipp_ctadj_gain.bits.ct_adj_gain_g       = scale_cfg->gain[1];
    node->hdr.hdr_cbm.hdr_hipp_ctadj_gain.bits.ct_adj_gain_b       = scale_cfg->gain[2]; // 2:index
    node->hdr.hdr_cbm.hdr_hipp_ctadj_gain_w.bits.ct_adj_gain_w     = scale_cfg->gain[3]; // 3:index

    node->hdr.hdr_cbm.hdr_hipp_ctadj_offset.bits.ct_adj_offset_r   = (td_u32)scale_cfg->offset[0];
    node->hdr.hdr_cbm.hdr_hipp_ctadj_offset.bits.ct_adj_offset_g   = (td_u32)scale_cfg->offset[1];
    node->hdr.hdr_cbm.hdr_hipp_ctadj_offset_b.bits.ct_adj_offset_b = (td_u32)scale_cfg->offset[2]; // 2:index
    node->hdr.hdr_cbm.hdr_hipp_ctadj_offset_w.bits.ct_adj_offset_w = (td_u32)scale_cfg->offset[3]; // 3:index

    node->hdr.hdr_cbm.hdr_hipp_ctadj_min.bits.ct_adj_min_r         = scale_cfg->clip_min[0];
    node->hdr.hdr_cbm.hdr_hipp_ctadj_min.bits.ct_adj_min_g         = scale_cfg->clip_min[1];
    node->hdr.hdr_cbm.hdr_hipp_ctadj_min.bits.ct_adj_min_b         = scale_cfg->clip_min[2]; // 2:index
    node->hdr.hdr_cbm.hdr_hipp_ctadj_min_w.bits.ct_adj_min_w       = scale_cfg->clip_min[3]; // 3:index

    node->hdr.hdr_cbm.hdr_hipp_ctadj_max.bits.ct_adj_max_r         = scale_cfg->clip_max[0];
    node->hdr.hdr_cbm.hdr_hipp_ctadj_max.bits.ct_adj_max_g         = scale_cfg->clip_max[1];
    node->hdr.hdr_cbm.hdr_hipp_ctadj_max.bits.ct_adj_max_b         = scale_cfg->clip_max[2]; // 2:index
    node->hdr.hdr_cbm.hdr_hipp_ctadj_max_w.bits.ct_adj_max_w       = scale_cfg->clip_max[3]; // 3:index
#else // 1A22
    node->hdr_hipp_ctadj_ctrl.bits.ct_adj_en                       = scale_cfg->scale_en;
    node->hdr_hipp_ctadj_gain.bits.ct_adj_gain_r                   = scale_cfg->gain[0];
    node->hdr_hipp_ctadj_gain.bits.ct_adj_gain_g                   = scale_cfg->gain[1];
    node->hdr_hipp_ctadj_gain.bits.ct_adj_gain_b                   = scale_cfg->gain[2]; // 2:index
    node->hdr_hipp_ctadj_gain_w.bits.ct_adj_gain_w                 = scale_cfg->gain[3]; // 3:index

    node->hdr_hipp_ctadj_offset.bits.ct_adj_offset_r               = (td_u32)scale_cfg->offset[0];
    node->hdr_hipp_ctadj_offset.bits.ct_adj_offset_g               = (td_u32)scale_cfg->offset[1];
    node->hdr_hipp_ctadj_offset_b.bits.ct_adj_offset_b             = (td_u32)scale_cfg->offset[2]; // 2:index
    node->hdr_hipp_ctadj_offset_w.bits.ct_adj_offset_w             = (td_u32)scale_cfg->offset[3]; // 3:index

    node->hdr_hipp_ctadj_min.bits.ct_adj_min_r                     = scale_cfg->clip_min[0];
    node->hdr_hipp_ctadj_min.bits.ct_adj_min_g                     = scale_cfg->clip_min[1];
    node->hdr_hipp_ctadj_min.bits.ct_adj_min_b                     = scale_cfg->clip_min[2]; // 2:index
    node->hdr_hipp_ctadj_min_w.bits.ct_adj_min_w                   = scale_cfg->clip_min[3]; // 3:index

    node->hdr_hipp_ctadj_max.bits.ct_adj_max_r                     = scale_cfg->clip_max[0];
    node->hdr_hipp_ctadj_max.bits.ct_adj_max_g                     = scale_cfg->clip_max[1];
    node->hdr_hipp_ctadj_max.bits.ct_adj_max_b                     = scale_cfg->clip_max[2]; // 2:index
    node->hdr_hipp_ctadj_max_w.bits.ct_adj_max_w                   = scale_cfg->clip_max[3]; // 3:index
#endif
}

STATIC td_void vau_hdr_node_set_hdr_disp1_dither(vau_hdr_hardware_node *node, td_bool hdr_en)
{
    uapi_unused(hdr_en);
    node->hdr.hdr_combine.hdr_disp_1_dither_ctrl.bits.dither1_en = 0x1;
    node->hdr.hdr_combine.hdr_disp_1_dither_ctrl.bits.i_data_width_dither1 = 0x2;
    node->hdr.hdr_combine.hdr_disp_1_dither_ctrl.bits.o_data_width_dither1 = 0x3;
    node->hdr.hdr_combine.hdr_disp_1_dither_ctrl.bits.t_data_width_dither1 = 0x2;
    node->hdr.hdr_combine.hdr_disp_1_dither_ctrl.bits.dither1_block_mode = 1;
    node->hdr.hdr_combine.hdr_disp_1_dither_ctrl.bits.dither1_reset = 0x4;
    node->hdr.hdr_combine.hdr_disp_1_dither_ctrl.bits.dither1_diffuse_en = 0x1;
}

td_void vau_hal_set_hdr_sys_ctrl(vau_hdr_hardware_node *node, td_bool hdr_en, td_bool hdr_cbm_en, td_bool is_hdr_des)
{
    uapi_unused(is_hdr_des);
    vau_hdr_node_set_hdr_sys_ctrl(node, hdr_en, hdr_cbm_en);
}

td_void vau_hal_set_hdr_hipp_csc(vau_hdr_hardware_node *node, const drv_vau_surface *surface,
    const hdr_csc_cfg *y2r_gainmap_cfg)
{
    if (surface->fmt == DRV_GFX_FMT_YUV400SP) {
        return;
    }

    vau_hdr_node_set_hdr_hipp_csc(node, y2r_gainmap_cfg);
}

td_void vau_hal_set_hdr_cvfir(vau_hdr_hardware_node *node, const drv_vau_surface *surface)
{
    if (surface->fmt == DRV_GFX_FMT_YUV400SP) {
        return;
    }
    vau_hdr_node_set_hdr_cvfir(node, surface);
}

td_void vau_hal_set_hdr_hfir(vau_hdr_hardware_node *node, const drv_vau_surface *surface)
{
    if (surface->fmt == DRV_GFX_FMT_YUV400SP) {
        return;
    }

    vau_hdr_node_set_hdr_hfir(node);
}

td_void vau_hal_set_hdr_vdm(vau_hdr_hardware_node *node, const drv_hdr_cfg *hdr_cfg)
{
    node->hdr.hdr_combine.hdr_vdm_ctrl.bits.vdm_en = 1;
    // change to csc,HDR_VDM_CSC1_COEF00
    vau_hdr_node_set_hdr_vdm_y2r(node, &hdr_cfg->y2r_base_cfg);
    // 0xc78
    vau_hdr_node_set_hdr_vdm_gm1(node, &hdr_cfg->gmap1_cfg);
    // 0xdfc
    vau_hdr_node_set_hdr_vdm_gm2(node, &hdr_cfg->gmap2_cfg);
    // 0xc68
    vau_hdr_node_set_hdr_vdm_degamma(node, &hdr_cfg->degamma_cfg);
    // 0xc70
    vau_hdr_node_set_hdr_vdm_gamma(node, &hdr_cfg->gamma_cfg);
    // 0xe48
    vau_hdr_node_set_hdr_vdm_tmap(node, &hdr_cfg->tmap_cfg);
}

td_void vau_hal_set_hdr_rcbn(vau_hdr_hardware_node *node, const hdr_rcbn_cfg *combine_cfg)
{
    vau_hdr_node_set_hdr_rbcn(node, combine_cfg);
}

td_void vau_hal_set_hdr_lighting(vau_hdr_hardware_node *node, const drv_vau_lighting_opt *lighting_opt,
    const drv_vau_surface *src2)
{
    vau_hdr_node_set_hdr_lighting(node, lighting_opt, src2);
}

td_void vau_hal_set_hdr_cbm(vau_hdr_hardware_node *node, const td_bool sdr_en, const td_bool hdr_en)
{
    vau_hdr_node_set_hdr_cbm(node, sdr_en, hdr_en);
}

td_void vau_hal_set_hdr_ctadj(vau_hdr_hardware_node *node, const hdr_scale_cfg *scale_cfg)
{
    vau_hdr_node_set_hdr_ctadj(node, scale_cfg);
}

td_void vau_hal_set_hdr_disp1_dither(vau_hdr_hardware_node *node, td_bool hdr_en)
{
    vau_hdr_node_set_hdr_disp1_dither(node, hdr_en);
}
#else
STATIC td_void vau_hdr_node_set_hdr_sys_ctrl(vau_hdr_hardware_node *node, const td_bool is_hdr_des)
{
    node->vau_hdr_sys_ctrl.bits.hdr_1mux2_sel   = is_hdr_des;
    node->vau_hdr_sys_ctrl.bits.hdr_1_1mux2_sel = 1;
    node->vau_hdr_sys_ctrl.bits.hdr_2mux1_sel   = 1;
    node->vau_hdr_sys_ctrl.bits.start_dly_cycle = 0x8F;
}

STATIC td_void vau_hdr_node_set_hdr_src1_base_info(vau_hdr_hardware_node *node, const drv_vau_surface *surface)
{
    node->hdr_vau_hdr_src1_ctrl.bits.src1_en = 1;
    node->hdr_vau_hdr_src1_ctrl.bits.src1_fmt = surface->fmt;
    node->hdr_vau_hdr_src1_ctrl.bits.src1_cbcr_order = 0;
    node->hdr_vau_hdr_src1_ctrl.bits.src1_mode = 0;

    node->hdr_vau_hdr_src1_addr.bits.src1_addr = surface->phy_addr;
    node->hdr_vau_hdr_src1_stride.bits.src1_stride = surface->stride;
    node->hdr_vau_hdr_src1c_addr.bits.src1c_addr = surface->phy_addr + surface->uv_offset;
    node->hdr_vau_hdr_src1c_stride.bits.src1c_stride = surface->stride;
    node->hdr_vau_hdr_src1_imgsize.bits.src1_width = surface->width - 1;
    node->hdr_vau_hdr_src1_imgsize.bits.src1_height = surface->height - 1;
    node->hdr_vau_hdr_src1_rect_xy.bits.src1_rect_x = (td_u32)surface->rect.x;
    node->hdr_vau_hdr_src1_rect_xy.bits.src1_rect_y = (td_u32)surface->rect.y;
    node->hdr_vau_hdr_src1_rect_wh.bits.src1_rect_width = surface->rect.width - 1;
    node->hdr_vau_hdr_src1_rect_wh.bits.src1_rect_height = surface->rect.height - 1;
}

STATIC td_void vau_hdr_node_set_hdr_src2_base_info(vau_hdr_hardware_node *node, const drv_vau_surface *surface)
{
    node->hdr_vau_hdr_src2_ctrl.bits.src2_en = 1;
    node->hdr_vau_hdr_src2_ctrl.bits.src2_fmt = surface->fmt;
    node->hdr_vau_hdr_src2_ctrl.bits.src2_cbcr_order = 0;
    node->hdr_vau_hdr_src2_ctrl.bits.src2_mode = 0;

    node->hdr_vau_hdr_src2_addr.bits.src2_addr = surface->phy_addr;
    node->hdr_vau_hdr_src2_stride.bits.src2_stride = surface->stride;
    node->hdr_vau_hdr_src2c_addr.bits.src2c_addr = surface->phy_addr + surface->uv_offset;
    node->hdr_vau_hdr_src2c_stride.bits.src2c_stride = surface->stride;
    node->hdr_vau_hdr_src2_imgsize.bits.src2_width = surface->width - 1;
    node->hdr_vau_hdr_src2_imgsize.bits.src2_height = surface->height - 1;
    node->hdr_vau_hdr_src2_rect_xy.bits.src2_rect_x = (td_u32)surface->rect.x;
    node->hdr_vau_hdr_src2_rect_xy.bits.src2_rect_y = (td_u32)surface->rect.y;
    node->hdr_vau_hdr_src2_rect_wh.bits.src2_rect_width = surface->rect.width - 1;
    node->hdr_vau_hdr_src2_rect_wh.bits.src2_rect_height = surface->rect.height - 1;
}

STATIC td_void vau_hdr_node_set_hdr_dst_base_info(vau_hdr_hardware_node *node, const drv_vau_surface *surface)
{
    node->hdr_vau_hdr_des_ctrl.bits.des_en = 1;
    node->hdr_vau_hdr_des_ctrl.bits.des_fmt = surface->fmt;
    node->hdr_vau_hdr_des_ctrl.bits.des_argb_order = surface->argb_order;

    node->hdr_vau_hdr_des_addr.bits.des_addr = surface->phy_addr;
    node->hdr_vau_hdr_des_stride.bits.des_stride = surface->stride;
    node->hdr_vau_hdr_des_imgsize.bits.des_width = surface->width - 1;
    node->hdr_vau_hdr_des_imgsize.bits.des_height = surface->height - 1;

    node->hdr_vau_hdr_des_rect_xy.bits.des_rect_x = (td_u32)surface->rect.x;
    node->hdr_vau_hdr_des_rect_xy.bits.des_rect_y = (td_u32)surface->rect.y;
    node->hdr_vau_hdr_des_rect_wh.bits.des_rect_width = surface->rect.width - 1;
    node->hdr_vau_hdr_des_rect_wh.bits.des_rect_height = surface->rect.height - 1;

    // output hdr dst, need sdr rect config
    node->vau_des_rect_xy.bits.des_rect_x = (td_u32)surface->rect.x;
    node->vau_des_rect_xy.bits.des_rect_y = (td_u32)surface->rect.y;
    node->vau_des_rect_wh.bits.des_rect_width = surface->rect.width - 1;
    node->vau_des_rect_wh.bits.des_rect_height = surface->rect.height - 1;
}

STATIC td_void vau_hdr_node_set_hdr_rbcn(vau_hdr_hardware_node *node, const hdr_rcbn_cfg *cfg)
{
    const td_u32 shift = 8; // 8:shift value

    node->hdr_rcbn_ctrl.bits.rcbn_en = cfg->cmb_en;
    node->hdr_rcbn_ctrl.bits.pow_en = cfg->pow_en;
    node->hdr_rcbn_gmap_size_mode.bits.gmap_size_mode = cfg->gainmap_size_mode;
    node->hdr_rcbn_gmap_chn_num.bits.gmap_chn_num = cfg->gainmap_components;

    node->hdr_rcbn_gcoeff_rmax.bits.gcoeff_r_max = ((td_s32)*(td_s32 *)(&cfg->gain_max[0x0])) >> shift;
    node->hdr_rcbn_gcoeff_gmax.bits.gcoeff_g_max = ((td_s32)*(td_s32 *)(&cfg->gain_max[0x1])) >> shift;
    node->hdr_rcbn_gcoeff_bmax.bits.gcoeff_b_max = ((td_s32)*(td_s32 *)(&cfg->gain_max[0x2])) >> shift;
    
    node->hdr_rcbn_gcoeff_rmin1.bits.gcoeff_r_min_1 = ((td_s32)*(td_s32 *)(&cfg->gain_min1[0x0])) >> shift;
    node->hdr_rcbn_gcoeff_gmin1.bits.gcoeff_g_min_1 = ((td_s32)*(td_s32 *)(&cfg->gain_min1[0x1])) >> shift;
    node->hdr_rcbn_gcoeff_bmin1.bits.gcoeff_b_min_1 = ((td_s32)*(td_s32 *)(&cfg->gain_min1[0x2])) >> shift;
    
    node->hdr_rcbn_gcoeff_rmin2.bits.gcoeff_r_min_2 = ((td_s32)*(td_s32 *)(&cfg->gain_min2[0x0])) >> shift;
    node->hdr_rcbn_gcoeff_gmin2.bits.gcoeff_g_min_2 = ((td_s32)*(td_s32 *)(&cfg->gain_min2[0x1])) >> shift;
    node->hdr_rcbn_gcoeff_bmin2.bits.gcoeff_b_min_2 = ((td_s32)*(td_s32 *)(&cfg->gain_min2[0x2])) >> shift;
    
    node->hdr_rcbn_kcoeff_r_base.bits.kcoeff_r_base = ((td_s32)*(td_s32 *)(&cfg->k_base[0x0])) >> shift;
    node->hdr_rcbn_kcoeff_g_base.bits.kcoeff_g_base = ((td_s32)*(td_s32 *)(&cfg->k_base[0x1])) >> shift;
    node->hdr_rcbn_kcoeff_b_base.bits.kcoeff_b_base = ((td_s32)*(td_s32 *)(&cfg->k_base[0x2])) >> shift;

    node->hdr_rcbn_kcoeff_r_alte.bits.kcoeff_r_alte = ((td_s32)*(td_s32 *)(&cfg->k_alternate[0x0])) >> shift;
    node->hdr_rcbn_kcoeff_g_alte.bits.kcoeff_g_alte = ((td_s32)*(td_s32 *)(&cfg->k_alternate[0x1])) >> shift;
    node->hdr_rcbn_kcoeff_b_alte.bits.kcoeff_b_alte = ((td_s32)*(td_s32 *)(&cfg->k_alternate[0x2])) >> shift;
    node->hdr_rcbn_weight.bits.weight               = ((td_s32)*(td_s32 *)(&cfg->weight)) >> shift;

    node->hdr_rcbn_clip_min.bits.clip_min = cfg->clip_min;
    node->hdr_rcbn_clip_max.bits.clip_max = cfg->clip_max;
}

STATIC td_void vau_hdr_node_set_hdr_hipp_csc(vau_hdr_hardware_node *node, const hdr_csc_cfg *y2r_gainmap_cfg)
{
    node->hdr_vau_hipp_csc_ctrl.bits.hipp_csc_en = y2r_gainmap_cfg->csc_en;
    node->hdr_vau_hipp_csc_coef00.bits.hipp_csc_coef00 = y2r_gainmap_cfg->ac_coef[0][0];
    node->hdr_vau_hipp_csc_coef01.bits.hipp_csc_coef01 = y2r_gainmap_cfg->ac_coef[0][1];
    node->hdr_vau_hipp_csc_coef02.bits.hipp_csc_coef02 = y2r_gainmap_cfg->ac_coef[0][2]; // 2:index
    node->hdr_vau_hipp_csc_coef10.bits.hipp_csc_coef10 = y2r_gainmap_cfg->ac_coef[1][0];
    node->hdr_vau_hipp_csc_coef11.bits.hipp_csc_coef11 = y2r_gainmap_cfg->ac_coef[1][1];
    node->hdr_vau_hipp_csc_coef12.bits.hipp_csc_coef12 = y2r_gainmap_cfg->ac_coef[1][2]; // 2:index
    node->hdr_vau_hipp_csc_coef20.bits.hipp_csc_coef20 = y2r_gainmap_cfg->ac_coef[2][0]; // 2:index
    node->hdr_vau_hipp_csc_coef21.bits.hipp_csc_coef21 = y2r_gainmap_cfg->ac_coef[2][1]; // 2:index
    node->hdr_vau_hipp_csc_coef22.bits.hipp_csc_coef22 = y2r_gainmap_cfg->ac_coef[2][2]; // 2:index
    node->hdr_vau_hipp_csc_scale.bits.hipp_csc_scale = y2r_gainmap_cfg->scale;
    node->hdr_vau_hipp_csc_idc0.bits.hipp_csc_idc0 = y2r_gainmap_cfg->dc_coef_in[0];
    node->hdr_vau_hipp_csc_idc1.bits.hipp_csc_idc1 = y2r_gainmap_cfg->dc_coef_in[1];
    node->hdr_vau_hipp_csc_idc2.bits.hipp_csc_idc2 = y2r_gainmap_cfg->dc_coef_in[2]; // 2:index
    node->hdr_vau_hipp_csc_odc0.bits.hipp_csc_odc0 = y2r_gainmap_cfg->dc_coef_out[0];
    node->hdr_vau_hipp_csc_odc1.bits.hipp_csc_odc1 = y2r_gainmap_cfg->dc_coef_out[1];
    node->hdr_vau_hipp_csc_odc2.bits.hipp_csc_odc2 = y2r_gainmap_cfg->dc_coef_out[2]; // 2:index
    node->hdr_vau_hipp_csc_min_y.bits.hipp_csc_min_y = y2r_gainmap_cfg->clip_min;
    node->hdr_vau_hipp_csc_min_c.bits.hipp_csc_min_c = y2r_gainmap_cfg->clip_min;
    node->hdr_vau_hipp_csc_max_y.bits.hipp_csc_max_y = y2r_gainmap_cfg->clip_max;
    node->hdr_vau_hipp_csc_max_c.bits.hipp_csc_max_c = y2r_gainmap_cfg->clip_max;
}

STATIC td_void vau_hdr_node_set_hdr_cvfir(vau_hdr_hardware_node *node, const drv_vau_surface *surface)
{
    node->hdr_vau_cvfir_vinfo.bits.out_pro = 1;
    node->hdr_vau_cvfir_vinfo.bits.out_fmt = 1;
    node->hdr_vau_cvfir_vinfo.bits.out_height = surface->height - 1;
    node->hdr_vau_cvfir_vsp.bits.cvfir_en = 1;
    node->hdr_vau_cvfir_vsp.bits.cvmid_en = 0;
    node->hdr_vau_cvfir_vsp.bits.cvfir_mode = 1;
    node->hdr_vau_cvfir_vsp.bits.vratio = 0x1000;
}

STATIC td_void vau_hdr_node_set_hdr_hfir(vau_hdr_hardware_node *node)
{
    node->hdr_vau_hfir_ctrl.bits.hfir_mode = 1;
    node->hdr_vau_hfir_ctrl.bits.hfir_en = 1;
}

STATIC td_void vau_hdr_node_set_hdr_cvfir2(vau_hdr_hardware_node *node, const drv_vau_surface *surface)
{
    node->hdr_vau_cvfir2_vinfo.bits.out_pro = 1;
    node->hdr_vau_cvfir2_vinfo.bits.out_fmt = 1;
    node->hdr_vau_cvfir2_vinfo.bits.out_height = surface->height - 1;
    node->hdr_vau_cvfir2_vsp.bits.cvfir_en = 1;
    node->hdr_vau_cvfir2_vsp.bits.cvmid_en = 0;
    node->hdr_vau_cvfir2_vsp.bits.cvfir_mode = 1;
    node->hdr_vau_cvfir2_vsp.bits.vratio = 0x1000;
}

STATIC td_void vau_hdr_node_set_hdr_hfir2(vau_hdr_hardware_node *node)
{
    node->hdr_vau_hfir2_ctrl.bits.hfir_mode = 1;
    node->hdr_vau_hfir2_ctrl.bits.hfir_en = 1;
}

static td_void write_coef(td_u32 addr, td_u32 *result, td_u32 result_size)
{
    for (td_u32 i = 0; i < result_size; i++) {
        uapi_reg_write(addr + (sizeof(td_u32) * i), result[i]);
    }
}

#define LINE_SIZE 16
static td_void set_coef(td_u32 addr, const td_u32 *data, td_u32 data_size, td_u32 bit_len)
{
    td_u32 i;
    td_u32 j = 0;
    td_u32 used_len = 0;
    td_u32 result[0x4] = { 0 };
    td_u32 tmp_addr = addr;

    for (i = 0; i < data_size; i++) {
        result[j] |= (data[i] << (used_len % 32)); /* unsigned int contains 32 bits */
        if ((used_len % 32 + bit_len) >= 32) {     /* unsigned int contains 32 bits */
            j++;
        }
        if ((used_len % 32 + bit_len) > 32 && (j < 0x4)) {    /* unsigned int contains 32 bits */
            result[j] |= (data[i] >> (32 - (used_len % 32))); /* unsigned int contains 32 bits */
        }

        used_len += bit_len;

        if (used_len + bit_len > 128 || i == (data_size - 1)) { /* 4 x 32 bits, line size is 128 bits */
            write_coef(tmp_addr, result, LINE_SIZE / sizeof(td_u32));
            memset_s(result, sizeof(result), 0, LINE_SIZE);
            j = 0;
            used_len = 0;
            tmp_addr += LINE_SIZE;
        }
    }
}

STATIC td_void vau_hdr_node_set_hdr_vdm_y2r(vau_hdr_hardware_node *node, const hdr_csc_cfg *y2r_cfg)
{
    node->hdr_vdm_yuv2rgb_ctrl.bits.vdm_y2r_en      = y2r_cfg->csc_en;
    node->hdr_vdm_yuv2rgb_ctrl.bits.vdm_v0_csc_en   = y2r_cfg->csc_en;
    node->hdr_vdm_yuv2rgb_ctrl.bits.vdm_v0_gamma_en = y2r_cfg->csc_en;

    node->hdr_vdm_yuv2rgb_coef00.bits.vdm_00_yuv2rgb          = y2r_cfg->ac_coef[0][0];
    node->hdr_vdm_yuv2rgb_coef01.bits.vdm_01_yuv2rgb          = y2r_cfg->ac_coef[0][1];
    node->hdr_vdm_yuv2rgb_coef02.bits.vdm_02_yuv2rgb          = y2r_cfg->ac_coef[0][2]; // 2:index
    node->hdr_vdm_yuv2rgb_coef03.bits.vdm_03_yuv2rgb          = y2r_cfg->ac_coef[1][0];
    node->hdr_vdm_yuv2rgb_coef04.bits.vdm_04_yuv2rgb          = y2r_cfg->ac_coef[1][1];
    node->hdr_vdm_yuv2rgb_coef05.bits.vdm_05_yuv2rgb          = y2r_cfg->ac_coef[1][2]; // 2:index
    node->hdr_vdm_yuv2rgb_coef06.bits.vdm_06_yuv2rgb          = y2r_cfg->ac_coef[2][0]; // 2:index
    node->hdr_vdm_yuv2rgb_coef07.bits.vdm_07_yuv2rgb          = y2r_cfg->ac_coef[2][1]; // 2:index
    node->hdr_vdm_yuv2rgb_coef08.bits.vdm_08_yuv2rgb          = y2r_cfg->ac_coef[2][2]; // 2:index
    node->hdr_vdm_yuv2rgb_scale2p.bits.vdm_yuv2rgbscale2p     = y2r_cfg->scale;
    node->hdr_vdm_yuv2rgb_in_dc00.bits.vdm_yuv2rgb_00_in_dc   = y2r_cfg->dc_coef_in[0];
    node->hdr_vdm_yuv2rgb_in_dc01.bits.vdm_yuv2rgb_01_in_dc   = y2r_cfg->dc_coef_in[1];
    node->hdr_vdm_yuv2rgb_in_dc02.bits.vdm_yuv2rgb_02_in_dc   = y2r_cfg->dc_coef_in[2]; // 2:index
    node->hdr_vdm_yuv2rgb_out_dc00.bits.vdm_yuv2rgb_00_out_dc = y2r_cfg->dc_coef_out[0];
    node->hdr_vdm_yuv2rgb_out_dc01.bits.vdm_yuv2rgb_01_out_dc = y2r_cfg->dc_coef_out[1];
    node->hdr_vdm_yuv2rgb_out_dc02.bits.vdm_yuv2rgb_02_out_dc = y2r_cfg->dc_coef_out[2]; // 2:index
    node->hdr_vdm_yuv2rgb_min.bits.vdm_yuv2rgb_clip_min       = y2r_cfg->clip_min;
    node->hdr_vdm_yuv2rgb_max.bits.vdm_yuv2rgb_clip_max       = y2r_cfg->clip_max;
}

STATIC td_void vau_hdr_node_set_hdr_vdm_gm1(vau_hdr_hardware_node *node, const hdr_csc_cfg *gm1_cfg)
{
    node->hdr_vdm_csc_ctrl.bits.vdm_csc_en           = gm1_cfg->csc_en;
    node->hdr_vdm_csc_clip_min.bits.vdm_csc_clip_min = gm1_cfg->clip_min;
    node->hdr_vdm_csc_clip_max.bits.vdm_csc_clip_max = gm1_cfg->clip_max;

    node->hdr_vdm_csc_coef00.bits.vdm_coef00_csc     = gm1_cfg->ac_coef[0][0];
    node->hdr_vdm_csc_coef01.bits.vdm_coef01_csc     = gm1_cfg->ac_coef[0][1];
    node->hdr_vdm_csc_coef02.bits.vdm_coef02_csc     = gm1_cfg->ac_coef[0][2]; // 2:index
    node->hdr_vdm_csc_coef10.bits.vdm_coef10_csc     = gm1_cfg->ac_coef[1][0];
    node->hdr_vdm_csc_coef11.bits.vdm_coef11_csc     = gm1_cfg->ac_coef[1][1];
    node->hdr_vdm_csc_coef12.bits.vdm_coef12_csc     = gm1_cfg->ac_coef[1][2]; // 2:index
    node->hdr_vdm_csc_coef20.bits.vdm_coef20_csc     = gm1_cfg->ac_coef[2][0]; // 2:index
    node->hdr_vdm_csc_coef21.bits.vdm_coef21_csc     = gm1_cfg->ac_coef[2][1]; // 2:index
    node->hdr_vdm_csc_coef22.bits.vdm_coef22_csc     = gm1_cfg->ac_coef[2][2]; // 2:index
    node->hdr_vdm_csc_scale.bits.vdm_csc_scale       = gm1_cfg->scale;
    node->hdr_vdm_csc_in_dc0.bits.vdm_csc_0_in_dc    = gm1_cfg->dc_coef_in[0];
    node->hdr_vdm_csc_in_dc1.bits.vdm_csc_1_in_dc    = gm1_cfg->dc_coef_in[1];
    node->hdr_vdm_csc_in_dc2.bits.vdm_csc_2_in_dc    = gm1_cfg->dc_coef_in[2];  // 2:index
    node->hdr_vdm_csc_out_dc0.bits.vdm_csc_0_out_dc  = gm1_cfg->dc_coef_out[0];
    node->hdr_vdm_csc_out_dc1.bits.vdm_csc_1_out_dc  = gm1_cfg->dc_coef_out[1];
    node->hdr_vdm_csc_out_dc2.bits.vdm_csc_2_out_dc  = gm1_cfg->dc_coef_out[2]; // 2:index
}

STATIC td_void vau_hdr_node_set_hdr_vdm_gm2(vau_hdr_hardware_node *node, const hdr_csc_cfg *gm2_cfg)
{
    node->hdr_vdm_csc1_ctrl.bits.vdm_csc_en           = gm2_cfg->csc_en;
    node->hdr_vdm_csc1_clip_min.bits.vdm_csc_clip_min = gm2_cfg->clip_min;
    node->hdr_vdm_csc1_clip_max.bits.vdm_csc_clip_max = gm2_cfg->clip_max;

    node->hdr_vdm_csc1_coef00.bits.vdm_coef00_csc     = gm2_cfg->ac_coef[0][0];
    node->hdr_vdm_csc1_coef01.bits.vdm_coef01_csc     = gm2_cfg->ac_coef[0][1];
    node->hdr_vdm_csc1_coef02.bits.vdm_coef02_csc     = gm2_cfg->ac_coef[0][2]; // 2:index
    node->hdr_vdm_csc1_coef10.bits.vdm_coef10_csc     = gm2_cfg->ac_coef[1][0];
    node->hdr_vdm_csc1_coef11.bits.vdm_coef11_csc     = gm2_cfg->ac_coef[1][1];
    node->hdr_vdm_csc1_coef12.bits.vdm_coef12_csc     = gm2_cfg->ac_coef[1][2]; // 2:index
    node->hdr_vdm_csc1_coef20.bits.vdm_coef20_csc     = gm2_cfg->ac_coef[2][0]; // 2:index
    node->hdr_vdm_csc1_coef21.bits.vdm_coef21_csc     = gm2_cfg->ac_coef[2][1]; // 2:index
    node->hdr_vdm_csc1_coef22.bits.vdm_coef22_csc     = gm2_cfg->ac_coef[2][2]; // 2:index
    node->hdr_vdm_csc1_scale.bits.vdm_csc_scale       = gm2_cfg->scale;
    node->hdr_vdm_csc1_in_dc0.bits.vdm_csc_0_in_dc    = gm2_cfg->dc_coef_in[0];
    node->hdr_vdm_csc1_in_dc1.bits.vdm_csc_1_in_dc    = gm2_cfg->dc_coef_in[1];
    node->hdr_vdm_csc1_in_dc2.bits.vdm_csc_2_in_dc    = gm2_cfg->dc_coef_in[2]; // 2:index
    node->hdr_vdm_csc1_out_dc0.bits.vdm_csc_0_out_dc  = gm2_cfg->dc_coef_out[0];
    node->hdr_vdm_csc1_out_dc1.bits.vdm_csc_1_out_dc  = gm2_cfg->dc_coef_out[1];
    node->hdr_vdm_csc1_out_dc2.bits.vdm_csc_2_out_dc  = gm2_cfg->dc_coef_out[2]; // 2:index
}

STATIC td_void vau_hdr_node_set_hdr_vdm_gamma(vau_hdr_hardware_node *node, const hdr_gamma_cfg *gamma_cfg)
{
    node->hdr_vdm_ctrl.bits.vdm_gmm_en = gamma_cfg->gamma_en;
    node->hdr_vdm_para_gmm_addr.bits.vdm_para_gmm_addr = GPU_HDR_GAMMA_ADDR;

    set_coef(node->hdr_vdm_para_gmm_addr.bits.vdm_para_gmm_addr, gamma_cfg->gamma_lut,
        sizeof(gamma_cfg->gamma_lut) / sizeof(td_s32), 12); /* gamma bit gap is 12 */
}

STATIC td_void vau_hdr_node_set_hdr_vdm_degamma(vau_hdr_hardware_node *node, const hdr_degamma_cfg *degamma_cfg)
{
    node->hdr_vdm_ctrl.bits.vdm_degmm_en = degamma_cfg->degmma_en;
    node->hdr_vdm_para_degmm_addr.bits.vdm_para_degmm_addr = GPU_HDR_DEGAMMA_ADDR;

    set_coef(node->hdr_vdm_para_degmm_addr.bits.vdm_para_degmm_addr, degamma_cfg->degamma_lut,
             sizeof(degamma_cfg->degamma_lut) / sizeof(td_s32), 21);  /* degamma bit gap is 21 */
}

STATIC td_void vau_hdr_node_set_hdr_vdm_tmap(vau_hdr_hardware_node *node, const hdr_tmap_cfg *tmap_cfg)
{
    node->hdr_vdm_ctrl.bits.vdm_tmap_en = tmap_cfg->tmap_en;
    node->hdr_vdm_para_tmap_addr.bits.vdm_para_tmap_addr = GPU_HDR_TMAP_ADDR;
    node->hdr_vdm_tmap_luma_coef0.bits.vdm_luma_coef0_tmap = tmap_cfg->tmap_luma_cal[0];
    node->hdr_vdm_tmap_luma_coef1.bits.vdm_luma_coef1_tmap = tmap_cfg->tmap_luma_cal[1];
    node->hdr_vdm_tmap_luma_coef2.bits.vdm_luma_coef2_tmap = tmap_cfg->tmap_luma_cal[2]; // 2:index
    node->hdr_vdm_tmap_luma_scale.bits.vdm_tmap_luma_scale = tmap_cfg->tmap_luma_cal_scale;
    node->hdr_vdm_tonemap_alpha.bits.v_tmap_mix_alpha = tmap_cfg->tmap_mix_alpha;
    node->hdr_vdm_tonemap_alpha.bits.v_tmap_scale_mix_alpha = tmap_cfg->tmap_mix_alpha_scale;
    node->hdr_vdm_tmap_out_clip_min.bits.vdm_tmap_out_clip_min = tmap_cfg->tmap_out_clip_min;
    node->hdr_vdm_tmap_out_clip_max.bits.vdm_tmap_out_clip_max = tmap_cfg->tmap_out_clip_max;
    node->hdr_vdm_tmap_coef_scale.bits.vdm_tmap_coef_scale = tmap_cfg->tmap_coef_scale;
    node->hdr_vdm_tmap_out_dc0.bits.vdm_tmap_out_dc0 = tmap_cfg->tmap_out_dc[0];
    node->hdr_vdm_tmap_out_dc1.bits.vdm_tmap_out_dc1 = tmap_cfg->tmap_out_dc[1];
    node->hdr_vdm_tmap_out_dc2.bits.vdm_tmap_out_dc2 = tmap_cfg->tmap_out_dc[2]; // 2:index

    set_coef(node->hdr_vdm_para_tmap_addr.bits.vdm_para_tmap_addr, tmap_cfg->tmap_lut,
             sizeof(tmap_cfg->tmap_lut) / sizeof(td_s32), 16); /* tmap bit gap is 16 */
}

STATIC td_void vau_hdr_node_set_hdr_lighting(vau_hdr_hardware_node *node, const drv_vau_lighting_opt *lighting_opt,
    const drv_vau_surface *src2)
{
    /* common lighting setting */
    node->hdr_local_lighting_cfg.bits.local_lighting_en = 1;
    node->hdr_local_lighting_cfg.bits.local_lighting_ck_gt_en = 1;
    node->hdr_local_lighting_cfg.bits.lighting_mode = (td_u32)lighting_opt->mode;
    node->hdr_local_lighting_cfg.bits.is_bi_dir = lighting_opt->attr.is_bi_dir;

    node->hdr_local_lighting_cx.bits.centerx = lighting_opt->attr.cx;
    node->hdr_local_lighting_cy.bits.centery = lighting_opt->attr.cy;
    node->hdr_local_lighting_radius2.bits.radius2 =
        lighting_opt->attr.radius * lighting_opt->attr.radius;
    node->hdr_local_lighting_amplitude.bits.amplitude = lighting_opt->attr.spe_amp;
    node->hdr_local_lighting_ampli_scale.bits.amplitude_scale =
        lighting_opt->attr.spe_amp / (lighting_opt->attr.radius * lighting_opt->attr.radius);
    node->hdr_local_lighting_amb_amp.bits.amb_amplitude = lighting_opt->attr.amb_amp;

    node->hdr_local_lighting_start_angle_rad.bits.start_angle_rad = lighting_opt->attr.start_angle_rad;
    node->hdr_local_lighting_start_end_angle_rad.bits.start_end_angle_rad =
        lighting_opt->attr.start_end_angle_rad;
    node->hdr_local_lighting_end_start_angle_rad.bits.end_start_angle_rad =
        lighting_opt->attr.end_start_angle_rad;
    node->hdr_local_lighting_end_angle_rad.bits.end_angle_rad = lighting_opt->attr.end_angle_rad;

    node->hdr_local_lighting_triwin2_coeff0.bits.triwin2_coeff0 =
        1 / (lighting_opt->attr.start_end_angle_rad - lighting_opt->attr.start_angle_rad);
    node->hdr_local_lighting_triwin2_coeff1.bits.triwin2_coeff1 =
        1 / (lighting_opt->attr.end_angle_rad - lighting_opt->attr.end_start_angle_rad);

    node->hdr_local_lighting_bbox0_p0.bits.bbox0_min_x = 0;
    node->hdr_local_lighting_bbox0_p0.bits.bbox0_min_y = 0;
    node->hdr_local_lighting_bbox0_p1.bits.bbox0_max_x = 599; // 599:max_val
    node->hdr_local_lighting_bbox0_p1.bits.bbox0_max_y = 599;  // 599:max_val

    node->hdr_local_lighting_bbox1_p0.bits.bbox1_min_x = 0;
    node->hdr_local_lighting_bbox1_p0.bits.bbox1_min_y = 0;
    node->hdr_local_lighting_bbox1_p1.bits.bbox1_max_x = 599;  // 599:max_val
    node->hdr_local_lighting_bbox1_p1.bits.bbox1_max_y = 599;  // 599:max_val

    node->hdr_local_lighting_img_reso.bits.image_width = src2->width - 1;
    node->hdr_local_lighting_img_reso.bits.image_height = src2->height - 1;

    node->hdr_local_lighting_adjust_value.bits.adjust_value = 0.8f;
}

STATIC td_void vau_hdr_node_set_hdr_vhdaccstat(vau_hdr_hardware_node *node, const drv_vau_surface *surface)
{
    node->hdr_vhdaccstatctrl.bits.acc_stat_en = 1;
    node->hdr_vhdaccstathpos.bits.acc_stat_hstart = 0;
    node->hdr_vhdaccstathpos.bits.acc_stat_hend = surface->width - 1;
    node->hdr_vhdaccstatvpos.bits.acc_stat_vstart = 0;
    node->hdr_vhdaccstatvpos.bits.acc_stat_vend = surface->height - 1;
    node->hdr_vhdaccshiftd.bits.acc_stat_shiftd1 = 0xF;
    node->hdr_vhdaccshiftd.bits.acc_stat_shiftd2 = 0x3;
    node->hdr_vhdaccshiftd.bits.acc_stat_shifthend = surface->width - 1;
    node->hdr_vhdaccdnsam.bits.acc_stat_hdnsample = 1;
    node->hdr_vhdaccdnsam.bits.acc_stat_vdnsample = 1;
    node->hdr_vhdaccsatcfg.bits.acc_stat_lowsat_bin = 0x3;
}

STATIC td_void vau_hdr_node_set_hdr_cbm(vau_hdr_hardware_node *node, const td_bool sdr_en, const td_bool hdr_en)
{
    node->vau_hdr_sys_ctrl.bits.hdr_cbm_g0_en = sdr_en;
    node->vau_hdr_sys_ctrl.bits.hdr_cbm_g1_en = hdr_en;
    node->vau_hdr_sys_ctrl.bits.hdr_cbm_g0_alpha = 0xFF;
    node->vau_hdr_sys_ctrl.bits.hdr_cbm_g1_alpha = 0xFF;

    /* HDR used as background, SDR used as foreground */
    node->hdr_cbm_mix1.bits.mixer_prio0 = 0x2;
    node->hdr_cbm_mix1.bits.mixer_prio1 = 0x1;
}

STATIC td_void vau_hdr_node_set_hdr_ctadj(vau_hdr_hardware_node *node, const hdr_scale_cfg *scale_cfg)
{
    node->hdr_hipp_ctadj_ctrl.bits.ct_adj_en           = scale_cfg->scale_en;
    node->hdr_hipp_ctadj_gain.bits.ct_adj_gain_r       = scale_cfg->gain[0];
    node->hdr_hipp_ctadj_gain.bits.ct_adj_gain_g       = scale_cfg->gain[1];
    node->hdr_hipp_ctadj_gain.bits.ct_adj_gain_b       = scale_cfg->gain[2]; // 2:index
    node->hdr_hipp_ctadj_gain_w.bits.ct_adj_gain_w     = scale_cfg->gain[3]; // 3:index

    node->hdr_hipp_ctadj_offset.bits.ct_adj_offset_r   = (td_u32)scale_cfg->offset[0];
    node->hdr_hipp_ctadj_offset.bits.ct_adj_offset_g   = (td_u32)scale_cfg->offset[1];
    node->hdr_hipp_ctadj_offset_b.bits.ct_adj_offset_b = (td_u32)scale_cfg->offset[2]; // 2:index
    node->hdr_hipp_ctadj_offset_w.bits.ct_adj_offset_w = (td_u32)scale_cfg->offset[3]; // 3:index

    node->hdr_hipp_ctadj_min.bits.ct_adj_min_r         = scale_cfg->clip_min[0];
    node->hdr_hipp_ctadj_min.bits.ct_adj_min_g         = scale_cfg->clip_min[1];
    node->hdr_hipp_ctadj_min.bits.ct_adj_min_b         = scale_cfg->clip_min[2]; // 2:index
    node->hdr_hipp_ctadj_min_w.bits.ct_adj_min_w       = scale_cfg->clip_min[3]; // 3:index

    node->hdr_hipp_ctadj_max.bits.ct_adj_max_r         = scale_cfg->clip_max[0];
    node->hdr_hipp_ctadj_max.bits.ct_adj_max_g         = scale_cfg->clip_max[1];
    node->hdr_hipp_ctadj_max.bits.ct_adj_max_b         = scale_cfg->clip_max[2]; // 2:index
    node->hdr_hipp_ctadj_max_w.bits.ct_adj_max_w       = scale_cfg->clip_max[3]; // 3:index
}

STATIC td_void vau_hdr_node_set_hdr_disp1_dither(vau_hdr_hardware_node *node)
{
    node->hdr_disp_1_dither_ctrl.bits.dither_en = 1;
    node->hdr_disp_1_dither_ctrl.bits.i_data_width_dither = 0x2;
    node->hdr_disp_1_dither_ctrl.bits.o_data_width_dither = 0x3;
    node->hdr_disp_1_dither_ctrl.bits.t_data_width_dither = 0x2;
    node->hdr_disp_1_dither_ctrl.bits.dither_block_mode = 1;
}

td_void vau_hal_set_hdr_sys_ctrl(vau_hdr_hardware_node *node, td_bool hdr_en, td_bool hdr_cbm_en, td_bool is_hdr_des)
{
    uapi_unused(hdr_en);
    uapi_unused(hdr_cbm_en);
    vau_hdr_node_set_hdr_sys_ctrl(node, is_hdr_des);
}

td_void vau_hal_set_hdr_src1(vau_hdr_hardware_node *node, const drv_vau_surface *surface)
{
    vau_hdr_node_set_hdr_src1_base_info(node, surface);
}

td_void vau_hal_set_hdr_src2(vau_hdr_hardware_node *node, const drv_vau_surface *surface)
{
    vau_hdr_node_set_hdr_src2_base_info(node, surface);
}

td_void vau_hal_set_hdr_dst(vau_hdr_hardware_node *node, const drv_vau_surface *surface)
{
    vau_hdr_node_set_hdr_dst_base_info(node, surface);
}

td_void vau_hal_set_hdr_hipp_csc(vau_hdr_hardware_node *node, const drv_vau_surface *surface,
    const hdr_csc_cfg *y2r_gainmap_cfg)
{
    if (surface->fmt == DRV_GFX_FMT_YUV400SP) {
        return;
    }

    vau_hdr_node_set_hdr_hipp_csc(node, y2r_gainmap_cfg);
}

td_void vau_hal_set_hdr_cvfir(vau_hdr_hardware_node *node, const drv_vau_surface *surface)
{
    if (surface->fmt == DRV_GFX_FMT_YUV400SP) {
        return;
    }
    vau_hdr_node_set_hdr_cvfir(node, surface);
}

td_void vau_hal_set_hdr_hfir(vau_hdr_hardware_node *node, const drv_vau_surface *surface)
{
    if (surface->fmt == DRV_GFX_FMT_YUV400SP) {
        return;
    }

    vau_hdr_node_set_hdr_hfir(node);
}

td_void vau_hal_set_hdr_cvfir2(vau_hdr_hardware_node *node, const drv_vau_surface *surface)
{
    vau_hdr_node_set_hdr_cvfir2(node, surface);
}

td_void vau_hal_set_hdr_hfir2(vau_hdr_hardware_node *node)
{
    vau_hdr_node_set_hdr_hfir2(node);
}

td_void vau_hal_set_hdr_vdm(vau_hdr_hardware_node *node, const drv_hdr_cfg *hdr_cfg)
{
    node->hdr_vdm_ctrl.bits.vdm_en = 1;
    vau_hdr_node_set_hdr_vdm_y2r(node, &hdr_cfg->y2r_base_cfg);
    vau_hdr_node_set_hdr_vdm_gm1(node, &hdr_cfg->gmap1_cfg);
    vau_hdr_node_set_hdr_vdm_gm2(node, &hdr_cfg->gmap2_cfg);
    vau_hdr_node_set_hdr_vdm_degamma(node, &hdr_cfg->degamma_cfg);
    vau_hdr_node_set_hdr_vdm_gamma(node, &hdr_cfg->gamma_cfg);
    vau_hdr_node_set_hdr_vdm_tmap(node, &hdr_cfg->tmap_cfg);
}

td_void vau_hal_set_hdr_rcbn(vau_hdr_hardware_node *node, const hdr_rcbn_cfg *combine_cfg)
{
    vau_hdr_node_set_hdr_rbcn(node, combine_cfg);
}

td_void vau_hal_set_hdr_lighting(vau_hdr_hardware_node *node, const drv_vau_lighting_opt *lighting_opt,
    const drv_vau_surface *src2)
{
    vau_hdr_node_set_hdr_lighting(node, lighting_opt, src2);
}

td_void vau_hal_set_hdr_vhdaccstat(vau_hdr_hardware_node *node, const drv_vau_surface *surface)
{
    vau_hdr_node_set_hdr_vhdaccstat(node, surface);
}

td_void vau_hal_set_hdr_cbm(vau_hdr_hardware_node *node, const td_bool sdr_en, const td_bool hdr_en)
{
    vau_hdr_node_set_hdr_cbm(node, sdr_en, hdr_en);
}

td_void vau_hal_set_hdr_ctadj(vau_hdr_hardware_node *node, const hdr_scale_cfg *scale_cfg)
{
    vau_hdr_node_set_hdr_ctadj(node, scale_cfg);
}

td_void vau_hal_set_hdr_disp1_dither(vau_hdr_hardware_node *node, td_bool hdr_en)
{
    uapi_unused(hdr_en);
    vau_hdr_node_set_hdr_disp1_dither(node);
}
#endif

