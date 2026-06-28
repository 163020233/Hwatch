/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2024. All rights reserved.
 * Description: vau algorithm file
 */

#include <stdio.h>
#include <math.h>
#include "securec.h"
#include "drv_vau_type.h"
#include "hdr_constants.h"

#include "cuva_api_common.h"
#include "cuva_api.h"

#define DEFAULT_GAMMA 2.2f
#define INTERVAL 10
#define MAX_LUT_IDX 8
#define NORMALIZED_FACTOR 1024

static td_s32 vau_hdr_set_enable(const drv_hdr_param *hdr_param, drv_hdr_cfg *hdr_cfg)
{
    hdr_cfg->y2r_base_cfg.csc_en = 0;
    hdr_cfg->y2r_gainmap_cfg.csc_en = 0;
    hdr_cfg->gmap1_cfg.csc_en = 0;
    hdr_cfg->gmap2_cfg.csc_en = 0;

    hdr_cfg->degamma_cfg.degmma_en = 0;
    hdr_cfg->gamma_cfg.gamma_en = 0;
    hdr_cfg->tmap_cfg.tmap_en = 0;

    hdr_cfg->combine_cfg.cmb_en = 0;
    hdr_cfg->combine_cfg.base_only_en = 0;
    hdr_cfg->combine_cfg.gainmap_only_en = 0;
    hdr_cfg->scale_cfg.scale_en = 1;

    if (hdr_param->hdr_source_type == HDR_SINGLE_LAYER) {
        hdr_cfg->degamma_cfg.degmma_en = 1;
        hdr_cfg->tmap_cfg.tmap_en = 1;
        hdr_cfg->gmap2_cfg.csc_en = 1;
        hdr_cfg->gamma_cfg.gamma_en = 1;
    } else if (hdr_param->hdr_source_type == HDR_STANDARD_DOUBLE_LAYER) {
        hdr_cfg->y2r_base_cfg.csc_en = 1;
        hdr_cfg->y2r_gainmap_cfg.csc_en = 1;
        hdr_cfg->degamma_cfg.degmma_en = 1;
        hdr_cfg->gamma_cfg.gamma_en = 1;
        hdr_cfg->gmap1_cfg.csc_en = 1;
        hdr_cfg->gmap2_cfg.csc_en = 1;
        hdr_cfg->tmap_cfg.tmap_en = 1;
        hdr_cfg->combine_cfg.cmb_en = 1;

        if (hdr_param->img_info_gainmap.gainmap_components == 1) {
            hdr_cfg->y2r_gainmap_cfg.csc_en = 0;
        }
    } else if (hdr_param->hdr_source_type == HDR_NONSTANDARD_DOUBLE_LAYER) {
        hdr_cfg->y2r_base_cfg.csc_en = 1;
        hdr_cfg->combine_cfg.cmb_en = 1;
    }

    return EXT_SUCCESS;
}

static td_s32 vau_hdr_set_y2r(const drv_hdr_param *hdr_param, drv_hdr_cfg *hdr_cfg)
{
    hdr_csc_cfg *y2r_base_cfg = &hdr_cfg->y2r_base_cfg;
    hdr_csc_cfg *y2r_gainmap_cfg = &hdr_cfg->y2r_gainmap_cfg;

    hdr_matrix_coefficients matrix_coefficients_base = hdr_param->img_info_base.cicp_base.matrix_coefficients;
    hdr_matrix_coefficients matrix_coefficients_gainmap =
        hdr_param->img_info_gainmap.cicp_gainmap.matrix_coefficients;

    // Set YUV2RGB base
    y2r_base_cfg->scale = scale_y2r_base;
    y2r_base_cfg->clip_min = clip_min_y2r_base;
    y2r_base_cfg->clip_max = clip_max_y2r_base;

    memcpy_s(y2r_base_cfg->dc_coef_in, sizeof(td_s32) * 0x3, dc_in_y2r_base, sizeof(td_s32) * 0x3);
    memcpy_s(y2r_base_cfg->dc_coef_out, sizeof(td_s32) * 0x3, dc_out_y2r_base, sizeof(td_s32) * 0x3);

    if (matrix_coefficients_base == HDR_MATRIX_COEFFICIENTS_BT601) {
        memcpy_s(y2r_base_cfg->ac_coef, sizeof(td_s32) * 0x9, m33y2r_601_base, sizeof(td_s32) * 0x9);
    } else if (matrix_coefficients_base == HDR_MATRIX_COEFFICIENTS_BT709) {
        memcpy_s(y2r_base_cfg->ac_coef, sizeof(td_s32) * 0x9, m33y2r_709_base, sizeof(td_s32) * 0x9);
    } else if (matrix_coefficients_base == HDR_MATRIX_COEFFICIENTS_BT2020NCL) {
        memcpy_s(y2r_base_cfg->ac_coef, sizeof(td_s32) * 0x9, m33y2r_2020ncl_base, sizeof(td_s32) * 0x9);
    } else {
        memcpy_s(y2r_base_cfg->ac_coef, sizeof(td_s32) * 0x9, m33y2r_709_base, sizeof(td_s32) * 0x9);
        printf("invalid matrix_coefficients_base[%d]\n", matrix_coefficients_base);
    }

    if ((hdr_param->hdr_source_type != HDR_STANDARD_DOUBLE_LAYER) &&
        (hdr_param->hdr_source_type != HDR_NONSTANDARD_DOUBLE_LAYER)) {
        return EXT_SUCCESS;
    }

    // Set YUV2RGB gainmap
    y2r_gainmap_cfg->scale = scale_y2r_gainmap;
    y2r_gainmap_cfg->clip_min = clip_min_y2r_gainmap;
    y2r_gainmap_cfg->clip_max = clip_max_y2r_gainmap;

    memcpy_s(y2r_gainmap_cfg->dc_coef_in, sizeof(td_s32) * 0x3, dc_in_y2r_gainmap, sizeof(td_s32) * 0x3);
    memcpy_s(y2r_gainmap_cfg->dc_coef_out, sizeof(td_s32) * 0x3, dc_out_y2r_gainmap, sizeof(td_s32) * 0x3);

    if (matrix_coefficients_gainmap == HDR_MATRIX_COEFFICIENTS_BT601) {
        // Always Bt601 because the encoder configuration is fixed and will not be saved to the image.
        memcpy_s(y2r_gainmap_cfg->ac_coef, sizeof(td_s32) * 0x9, m33y2r_601_gainmap, sizeof(td_s32) * 0x9);
    } else {
        memcpy_s(y2r_gainmap_cfg->ac_coef, sizeof(td_s32) * 0x9, m33y2r_601_gainmap, sizeof(td_s32) * 0x9);
        printf("invalid matrix_coefficients_gainmap[%d]\n", matrix_coefficients_gainmap);
    }

    return EXT_SUCCESS;
}

static td_s32 vau_hdr_set_degmm(const drv_hdr_param *hdr_param, drv_hdr_cfg *hdr_cfg)
{
    hdr_transfer_characteristics eotf_base = hdr_param->img_info_base.cicp_base.transfer_characteristics;

    if (eotf_base == HDR_TRANS_CHARAC_GAMMA2_2_TYPE) {
        memcpy_s(hdr_cfg->degamma_cfg.degamma_lut, sizeof(td_u32) * DEGMM_LUT_SIZE, de_gmm22_lut,
                 sizeof(td_u32) * DEGMM_LUT_SIZE);
    } else if (eotf_base == HDR_TRANS_CHARAC_PQ_TYPE) {
        memcpy_s(hdr_cfg->degamma_cfg.degamma_lut, sizeof(td_u32) * DEGMM_LUT_SIZE, de_pq_lut,
                 sizeof(td_u32) * DEGMM_LUT_SIZE);
    } else if (eotf_base == HDR_TRANS_CHARAC_HLG_TYPE) {
        memcpy_s(hdr_cfg->degamma_cfg.degamma_lut, sizeof(td_u32) * DEGMM_LUT_SIZE, de_hlg_lut,
                 sizeof(td_u32) * DEGMM_LUT_SIZE);
    } else {
        memcpy_s(hdr_cfg->degamma_cfg.degamma_lut, sizeof(td_u32) * DEGMM_LUT_SIZE, de_gmm22_lut,
                 sizeof(td_u32) * DEGMM_LUT_SIZE);
        printf("invalid eotf_base[%d]\n", eotf_base);
    }

    return EXT_SUCCESS;
}

static td_s32 vau_hdr_set_gmap(const drv_hdr_param *hdr_param, drv_hdr_cfg *hdr_cfg)
{
#ifdef GPU_1A22
    uapi_unused(hdr_param);
    memcpy_s(hdr_cfg->gmap1_cfg.ac_coef, sizeof(td_s32) * 0x9, m33gmap_709_2020ncl, sizeof(td_s32) * 0x9);
    memcpy_s(hdr_cfg->gmap2_cfg.ac_coef, sizeof(td_s32) * 0x9, m33gmap_2020ncl_709, sizeof(td_s32) * 0x9);
#else
    hdr_color_primaries color_primaries_base = hdr_param->img_info_base.cicp_base.color_primaries;
    hdr_color_primaries color_primaries_alternate = hdr_param->img_info_gainmap.cicp_alternate.color_primaries;
    // Set GamutMapping1  Unify the input to the Bt2020 color gamut for combination.
    hdr_cfg->gmap1_cfg.scale = scale_y2r_gmap1;
    hdr_cfg->gmap1_cfg.clip_min = clip_min_y2r_gmap1;
    hdr_cfg->gmap1_cfg.clip_max = clip_max_y2r_gmap1;
    memset_s(hdr_cfg->gmap1_cfg.dc_coef_in, sizeof(td_s32) * 0x3, 0, sizeof(td_s32) * 0x3);
    memset_s(hdr_cfg->gmap1_cfg.dc_coef_out, sizeof(td_s32) * 0x3, 0, sizeof(td_s32) * 0x3);
    if (color_primaries_base == HDR_COLOR_PRIMARIES_BT709) {
        memcpy_s(hdr_cfg->gmap1_cfg.ac_coef, sizeof(td_s32) * 0x9, m33gmap_709_2020ncl, sizeof(td_s32) * 0x9);
    }

    // Set GamutMapping2  Adapts to the screen color gamut. Assume that the screen color gamut is Bt709.
    hdr_cfg->gmap2_cfg.scale = scale_y2r_gmap2;
    hdr_cfg->gmap2_cfg.clip_min = clip_min_y2r_gmap2;
    hdr_cfg->gmap2_cfg.clip_max = clip_max_y2r_gmap2;
    memset_s(hdr_cfg->gmap2_cfg.dc_coef_in, sizeof(td_s32) * 0x3, 0, sizeof(td_s32) * 0x3);
    memset_s(hdr_cfg->gmap2_cfg.dc_coef_out, sizeof(td_s32) * 0x3, 0, sizeof(td_s32) * 0x3);

    if (color_primaries_alternate == HDR_COLOR_PRIMARIES_BT2020) {
        // The default display color space is Bt709.
        memcpy_s(hdr_cfg->gmap2_cfg.ac_coef, sizeof(td_s32) * 0x9, m33gmap_2020ncl_709, sizeof(td_s32) * 0x9);
    } else {
        printf("invalid color_primaries_alternate %d\n", color_primaries_alternate);
    }
#endif

    return EXT_SUCCESS;
}

#ifndef GPU_1A22
static td_s32 cuva_update_tmlut(drv_hdr_param *hdr_param, td_u32 *tm_lut, td_u32 tm_lut_size)
{
    cuva_api_param cuva_in_param;

    memcpy_s(&cuva_in_param.metadata, sizeof(cuva_metadata), &hdr_param->metadata_standard.dynamic_metadata,
             sizeof(hdr_dynamic_metadata));
    cuva_in_param.minlum_display = 500;          /* 500: Parameter */
    cuva_in_param.maxlum_display = 500 * 10000;  /* 500: nit, 10000: Typical Parameters */
    cuva_in_param.maxlum_mastering = 40000000;   /* 40000000: 4000nit * 10000 */
    cuva_in_param.dynamic_metadata_avaliable = 1;
    cuva_in_param.hdr_output_enable = 1; /* 1: hdr output */

    cuva_update_tmlut_core(&cuva_in_param, tm_lut, tm_lut_size);

    return EXT_SUCCESS;
}
#endif

static td_s32 vau_hdr_set_tm(drv_hdr_param *hdr_param, drv_hdr_cfg *hdr_cfg)
{
    td_u32 tm_lut[TM_LUT_SIZE] = {0};
    hdr_tmap_cfg *tmp_cfg = &hdr_cfg->tmap_cfg;
    hdr_transfer_characteristics eotf_alternate =
        hdr_param->img_info_gainmap.cicp_alternate.transfer_characteristics;

    tmp_cfg->tmap_luma_cal_scale = luma_cal_scale;
    memcpy_s(tmp_cfg->tmap_luma_cal, sizeof(td_u32) * 0x3, m3_luma_cal, sizeof(td_u32) * 0x3);

    tmp_cfg->tmap_coef_scale = tm_coef_scale;
    tmp_cfg->tmap_out_clip_min = clip_min_tm;
    tmp_cfg->tmap_out_clip_max = clip_max_tm;

    memset_s(tmp_cfg->tmap_out_dc, sizeof(td_s32) * 0x3, 0, sizeof(td_s32) * 0x3);

#ifdef GPU_1A22
    tmp_cfg->tmap_mix_alpha = 0x3f800000; /* float24 1.0 */
#else
    tmp_cfg->tmap_mix_alpha = 128;     /* 128: Typical Parameters */
#endif
    tmp_cfg->tmap_mix_alpha_scale = 7; /* 7: Typical Parameters */

    if (hdr_param->hdr_source_type == HDR_SINGLE_LAYER) {
        eotf_alternate = hdr_param->img_info_base.cicp_base.transfer_characteristics;
    }
    // The default display feature is gamma2.2.
    if (eotf_alternate == HDR_TRANS_CHARAC_PQ_TYPE) {
        tmp_cfg->tmap_coef_scale = tm_coef_scale_pq2sdr;
        memcpy_s(tm_lut, sizeof(td_u32) * TM_LUT_SIZE, tm_lut_pq2sdr, sizeof(td_u32) * TM_LUT_SIZE);
    } else if (eotf_alternate == HDR_TRANS_CHARAC_HLG_TYPE) {
        tmp_cfg->tmap_coef_scale = tm_coef_scale_hlg2sdr;
        memcpy_s(tm_lut, sizeof(td_u32) * TM_LUT_SIZE, tm_lut_hlg2sdr, sizeof(td_u32) * TM_LUT_SIZE);
    } else if (eotf_alternate == HDR_TRANS_CHARAC_GAMMA2_2_TYPE) {
        tmp_cfg->tmap_coef_scale = tm_coef_scale_linear;
        memcpy_s(tm_lut, sizeof(td_u32) * TM_LUT_SIZE, tm_lut_linear, sizeof(td_u32) * TM_LUT_SIZE);
    } else {
        tmp_cfg->tmap_coef_scale = tm_coef_scale_linear;
        memcpy_s(tm_lut, sizeof(td_u32) * TM_LUT_SIZE, tm_lut_linear, sizeof(td_u32) * TM_LUT_SIZE);
        printf("invalid eotf_alternate[%d]\n", eotf_alternate);
    }

#ifndef GPU_1A22
    cuva_update_tmlut(hdr_param, tm_lut, TM_LUT_SIZE);
#endif
    memcpy_s(tmp_cfg->tmap_lut, sizeof(td_u32) * TM_LUT_SIZE, tm_lut, sizeof(td_u32) * TM_LUT_SIZE);

    return EXT_SUCCESS;
}

static td_s32 vau_hdr_set_gmm(drv_hdr_cfg *hdr_cfg)
{
    // The default display feature is gamma2.2.
    memcpy_s(hdr_cfg->gamma_cfg.gamma_lut, sizeof(td_u32) * GMM_LUT_SIZE, gmm22_lut, sizeof(td_u32) * GMM_LUT_SIZE);

    return EXT_SUCCESS;
}

static td_float vau_hdr_calc_idea_gain(hdr_metadata_nonstandard *metadata, hdr_pic_disp_param *disp_param)
{
    td_s32 idx = 0;
    td_float idea_gain = 0;
    td_float sdr_disp_gain = 3.5904f;
    td_float gamma_display = 0;
    td_float gamma_scene = 0;
    td_float wgt = 0;

    if ((metadata->sdr_display_luma < 0) || (metadata->hdr_luma_ratio < 0)) {
        idea_gain = 1;
        printf("Hdrpicture metadata is invalid.\n");
        return (td_float)EXT_FAILURE;
    }

    gamma_display = disp_param->base_g - log(metadata->sdr_display_luma / disp_param->lum_r) / disp_param->log_r;
    gamma_scene = disp_param->base_g - log(metadata->ae_lv / disp_param->lum_r) / disp_param->log_r;

    if (disp_param->method_idx == 0) {
        if ((td_s32)metadata->ae_lv < lv_table[0]) {
            idea_gain = ratio_table[0];
        } else {
            idx = ((td_s32)metadata->ae_lv - lv_table[0]) / INTERVAL;
            wgt = ((td_s32)metadata->ae_lv * 1.0 - lv_table[0]) / INTERVAL - idx;
            // bilinear interpolation
            idea_gain = ratio_table[uapi_min(uapi_max(idx, 0), MAX_LUT_IDX)] * (1 - wgt) +
                        ratio_table[uapi_min(uapi_max(idx + 1, 0), MAX_LUT_IDX)] * wgt;
        }
    } else if (disp_param->method_idx == 1) {
        idea_gain = powf(disp_param->base, (gamma_display / gamma_scene)) / sdr_disp_gain;
    } else if (disp_param->method_idx == 2) { /* 2: Calculation Mode */
        idea_gain = metadata->hdr_luma_ratio;
    } else {
        printf("Hdrpicture method select err! method_idx = %d\n", disp_param->method_idx);
        return (td_float)EXT_FAILURE;
    }
    return idea_gain;
}

static td_float vau_hdr_calc_hdr_factor(td_float gray_factor_sdr, td_float idea_gain,
    const hdr_pic_disp_param *disp_param)
{
    td_float gray_factor_hdr = 0;
    td_float sdr_scale = 0;
    td_float hdr_scale = 0;

    if (disp_param->gain_min < 1.0f || disp_param->hdr_gamma < 1.0f) { /* 1.0: Max. */
        gray_factor_hdr = gray_factor_sdr;
        printf("vau_hdr_calc_hdr_factor error!  gain_min=%f, hdr_gamma=%f\n", disp_param->gain_min,
               disp_param->hdr_gamma);
        return gray_factor_hdr;
    }

    sdr_scale = powf(gray_factor_sdr, DEFAULT_GAMMA);
    hdr_scale = sdr_scale * uapi_max(idea_gain, disp_param->gain_min);
    if (hdr_scale > 1) {        /* 1: Threshold */
        gray_factor_hdr = 1.0f; /* 1.0f: param */
    } else {
        gray_factor_hdr = powf(hdr_scale, 1 / disp_param->hdr_gamma);
    }

    return gray_factor_hdr;
}

static td_s32 vau_hdr_set_combine_standard(const drv_hdr_param *hdr_param, drv_hdr_cfg *hdr_cfg)
{
    td_s32 i = 0;
    hdr_rcbn_cfg *cmb_cfg = &hdr_cfg->combine_cfg;
    hdr_extend_info extend_info = hdr_param->img_info_gainmap.extend_info;
    hdr_transfer_characteristics eotf_alternate =
        hdr_param->img_info_gainmap.cicp_alternate.transfer_characteristics;

#ifdef GPU_1A22
    td_u8 offset = 0;
#else
    td_u8 offset = 21; /* 21:index */
#endif

    for (i = 0; i < 3; i++) { /* 3: index */
        cmb_cfg->gain_max[i] = extend_info.max_gainmap[i] / (1023.0f);
        cmb_cfg->gain_min1[i] = extend_info.min_gainmap[i] / (1023.0f);
        if (eotf_alternate == HDR_TRANS_CHARAC_PQ_TYPE) {
            cmb_cfg->gain_min2[i] = extend_info.min_gainmap[i] + (-6.643856f);
            cmb_cfg->k_base[i] = extend_info.baseline_image_offset[i] * (1 << offset) * 100; /* 100: default param */
            cmb_cfg->k_alternate[i] = extend_info.alternate_image_offset[i] * (1 << offset);
        } else if (eotf_alternate == HDR_TRANS_CHARAC_HLG_TYPE) {
            cmb_cfg->gain_min2[i] = extend_info.min_gainmap[i] + (-3.321928f);
            cmb_cfg->k_base[i] = extend_info.baseline_image_offset[i] * (1 << offset) * 1000; /* 1000: default param */
            cmb_cfg->k_alternate[i] = extend_info.alternate_image_offset[i] * (1 << offset) * 10; /* 10:index */
        } else {
            printf("vau_hdr_set_combine_standard: eotf_alternate is invalid.\n");
            return EXT_FAILURE;
        }
    }

    return EXT_SUCCESS;
}

static td_s32 vau_hdr_set_combine_nonstandard(const drv_hdr_param *hdr_param, drv_hdr_cfg *hdr_cfg)
{
    td_s32 i = 0;
    hdr_rcbn_cfg *cmb_cfg = &hdr_cfg->combine_cfg;
    hdr_metadata_nonstandard metadata_nonstandard = hdr_param->metadata_nonstandard;

    if (cmb_cfg->pow_en == TD_FALSE) {
        td_float gray_scale_factor_sdr = 0;
        td_float gray_scale_factor_hdr = 0;
        td_float idea_gain = 0;
        hdr_pic_disp_param hdr_pic_disp_param = {50, 10, 2.3, 4, 0, 2.2, 1.0}; /* default param */

        gray_scale_factor_sdr = powf((1.0f / hdr_param->display_headroom), 1.0f / DEFAULT_GAMMA);
        if ((gray_scale_factor_sdr < 0) || (gray_scale_factor_sdr > 1)) {
            printf("Hdrpicture grayscaleFactor is invalid.\n");
            return EXT_FAILURE;
        }

        idea_gain = vau_hdr_calc_idea_gain(&metadata_nonstandard, &hdr_pic_disp_param);
        gray_scale_factor_hdr = vau_hdr_calc_hdr_factor(gray_scale_factor_sdr, idea_gain, &hdr_pic_disp_param);

        for (i = 0; i < 3; i++) {                                    /* 3: index */
            cmb_cfg->gain_max[i] = gray_scale_factor_hdr / 1023.0f;  /* 1023: Typical Parameters */
            cmb_cfg->gain_min1[i] = gray_scale_factor_sdr / 1023.0f; /* 1023: Typical Parameters */
            cmb_cfg->gain_min2[i] = gray_scale_factor_sdr;
#ifdef GPU_1A22
            cmb_cfg->k_base[i] = 0.000001f;
            cmb_cfg->k_alternate[i] = 0.000001f;
#else
            cmb_cfg->k_base[i] = 2.0f;      /* 2: extend_info.baseline_image_offset[i] * (1 << 21); */
            cmb_cfg->k_alternate[i] = 2.0f; /* 2: extend_info.alternate_image_offset[i] * (1 << 21); */
#endif
        }
    }

    return EXT_SUCCESS;
}

static td_s32 vau_hdr_set_combine(drv_hdr_param *hdr_param, drv_hdr_cfg *hdr_cfg)
{
    td_s32 ret;
    hdr_img_info_base img_info_base = hdr_param->img_info_base;
    hdr_img_info_gainmap img_info_gainmap = hdr_param->img_info_gainmap;
    hdr_rcbn_cfg *cmb_cfg = &hdr_cfg->combine_cfg;

    if ((hdr_param->hdr_source_type != HDR_STANDARD_DOUBLE_LAYER) &&
        (hdr_param->hdr_source_type != HDR_NONSTANDARD_DOUBLE_LAYER)) {
        return EXT_SUCCESS;
    }

    cmb_cfg->clip_min = 0;
    cmb_cfg->clip_max = 2097151; /* 2097151: Typical Parameters */
    cmb_cfg->weight = 1;         /* 1: Typical Parameters */
    cmb_cfg->gainmap_components = img_info_gainmap.gainmap_components;

    if (img_info_base.img_width == img_info_gainmap.img_width &&
        img_info_base.img_height == img_info_gainmap.img_height) {
        cmb_cfg->gainmap_size_mode = HDR_GMAP_SIZE_MODE_ONE_TO_ONE;
    } else if (img_info_base.img_width == img_info_gainmap.img_width * 2 && /* 2: Sampling Rate */
               img_info_base.img_height == img_info_gainmap.img_height) {
        cmb_cfg->gainmap_size_mode = HDR_GMAP_SIZE_MODE_ONE_TO_TWO_H;
    } else if (img_info_base.img_width == img_info_gainmap.img_width &&
               img_info_base.img_height == img_info_gainmap.img_height * 2) { /* 2: Sampling Rate */
        cmb_cfg->gainmap_size_mode = HDR_GMAP_SIZE_MODE_ONT_TO_TWO_V;
    } else if (img_info_base.img_width == img_info_gainmap.img_width * 2 &&   /* 2: Sampling Rate */
               img_info_base.img_height == img_info_gainmap.img_height * 2) { /* 2: Sampling Rate */
        cmb_cfg->gainmap_size_mode = HDR_GMAP_SIZE_MODE_ONE_TO_FOUR;
    } else {
        printf("invaild width/height %u x %u \n", img_info_base.img_width, img_info_base.img_height);
        return EXT_FAILURE;
    }

    if (hdr_param->hdr_source_type == HDR_STANDARD_DOUBLE_LAYER) {
        cmb_cfg->pow_en = TD_TRUE;
        ret = vau_hdr_set_combine_standard(hdr_param, hdr_cfg);
    } else {
        cmb_cfg->pow_en = TD_FALSE;
        ret = vau_hdr_set_combine_nonstandard(hdr_param, hdr_cfg);
    }

    return ret;
}

static td_s32 vau_hdr_set_scale(const drv_hdr_param *hdr_param, drv_hdr_cfg *hdr_cfg)
{
    td_s32 i = 0;
    hdr_scale_cfg *scale_cfg = &hdr_cfg->scale_cfg;

    // RGB scale, 10bit->10bit
    for (i = 0; i < 3; i++) {                                                /* 3: index */
        scale_cfg->gain[i] = (td_u32)(255.0f / hdr_param->display_headroom); /* 255: means 1 */
        scale_cfg->offset[i] = 0;                                            /* 0: Typical Parameters */
        scale_cfg->clip_min[i] = 0;                                          /* 0: Typical Parameters */
        scale_cfg->clip_max[i] = 1023;                                       /* 1023: Typical Parameters */
    }

    scale_cfg->gain[3] = 256;     /* 3: index, 256: means 1 */
    scale_cfg->offset[3] = 0;     /* 3: index, 0: Typical Parameters */
    scale_cfg->clip_min[3] = 0;   /* 3: index, 0: Typical Parameters */
    scale_cfg->clip_max[3] = 255; /* 3: index, 255: Typical Parameters */

    return EXT_SUCCESS;
}

#ifdef HDR_ALG_DFX_SUPPORT
static td_void vau_hdr_dfx_y2r(const drv_hdr_cfg *hdr_cfg)
{
    td_s32 i = 0;
    printf("--- y2r_gainmap_cfg ---");
    printf("y2r_gainmap_en = %d \n", (td_s32)hdr_cfg->y2r_gainmap_cfg.csc_en);
    printf("y2r_gainmap_scale = %u \n", hdr_cfg->y2r_gainmap_cfg.scale);
    printf("y2r_gainmap_ac_coef[3][3] = { \n");
    for (i = 0; i < 3; i++) { /* 3: size */
        printf(" %d, %d, %d \n", hdr_cfg->y2r_gainmap_cfg.ac_coef[i][0], hdr_cfg->y2r_gainmap_cfg.ac_coef[i][1],
               hdr_cfg->y2r_gainmap_cfg.ac_coef[i][2]); /* 1,2: index */
    }
    printf("}\n");
    printf("y2r_gainmap_dc_coef_in[3] = { %d, %d, %d }\n", hdr_cfg->y2r_gainmap_cfg.dc_coef_in[0],
           hdr_cfg->y2r_gainmap_cfg.dc_coef_in[1], hdr_cfg->y2r_gainmap_cfg.dc_coef_in[2]); /* 1,2: index */
    printf("y2r_gainmap_dc_coef_out[3] = { %d, %d, %d }\n", hdr_cfg->y2r_gainmap_cfg.dc_coef_out[0],
           hdr_cfg->y2r_gainmap_cfg.dc_coef_out[1], hdr_cfg->y2r_gainmap_cfg.dc_coef_out[2]); /* 1,2: index */
    printf("y2r_gainmap_clip_min = %u \n", hdr_cfg->y2r_gainmap_cfg.clip_min);
    printf("y2r_gainmap_clip_max = %u \n", hdr_cfg->y2r_gainmap_cfg.clip_max);
    printf("Pay attention to the settings of clip_Y and clip_C !!! \n");
    printf("***************************");

    printf("--- y2r_base_cfg ---");
    printf("y2r_base_en = %d \n", (td_s32)hdr_cfg->y2r_base_cfg.csc_en);
    printf("y2r_base_scale = %u \n", hdr_cfg->y2r_base_cfg.scale);
    printf("y2r_base_ac_coef[3][3] = { \n");
    for (i = 0; i < 3; i++) { /* 3: size */
        printf(" %d, %d, %d \n", hdr_cfg->y2r_base_cfg.ac_coef[i][0], hdr_cfg->y2r_base_cfg.ac_coef[i][1],
               hdr_cfg->y2r_base_cfg.ac_coef[i][2]); /* 1,2: index */
    }
    printf("}\n");
    printf("y2r_base_dc_coef_in[3] = { %d, %d, %d }\n", hdr_cfg->y2r_base_cfg.dc_coef_in[0],
           hdr_cfg->y2r_base_cfg.dc_coef_in[1], hdr_cfg->y2r_base_cfg.dc_coef_in[2]); /* 1,2: index */
    printf("y2r_base_dc_coef_out[3] = { %d, %d, %d }\n", hdr_cfg->y2r_base_cfg.dc_coef_out[0],
           hdr_cfg->y2r_base_cfg.dc_coef_out[1], hdr_cfg->y2r_base_cfg.dc_coef_out[2]); /* 1,2: index */
    printf("y2r_base_clip_min = %u \n", hdr_cfg->y2r_base_cfg.clip_min);
    printf("y2r_base_clip_max = %u \n", hdr_cfg->y2r_base_cfg.clip_max);
    printf("***************************");
}

static td_void vau_hdr_dfx_gamut_mapping(const drv_hdr_cfg *hdr_cfg)
{
    td_s32 i = 0;
    printf("--- gmap1_cfg ---");
    printf("gmap1_en = %d \n", hdr_cfg->gmap1_cfg.csc_en);
    printf("gmap1_scale = %u \n", hdr_cfg->gmap1_cfg.scale);
    printf("gmap1_ac_coef[3][3] = { \n");
    for (i = 0; i < 3; i++) { /* 3: size */
        printf(" %d, %d, %d \n", hdr_cfg->gmap1_cfg.ac_coef[i][0], hdr_cfg->gmap1_cfg.ac_coef[i][1],
               hdr_cfg->gmap1_cfg.ac_coef[i][2]); /* 1,2: index */
    }
    printf("}\n");
    printf("gmap1_dc_coef_in[3] = { %d, %d, %d }\n", hdr_cfg->gmap1_cfg.dc_coef_in[0], hdr_cfg->gmap1_cfg.dc_coef_in[1],
           hdr_cfg->gmap1_cfg.dc_coef_in[2]); /* 1,2: index */
    printf("gmap1_dc_coef_out[3] = { %d, %d, %d }\n", hdr_cfg->gmap1_cfg.dc_coef_out[0],
           hdr_cfg->gmap1_cfg.dc_coef_out[1], hdr_cfg->gmap1_cfg.dc_coef_out[2]); /* 1,2: index */
    printf("gmap1_clip_min = %u \n", hdr_cfg->gmap1_cfg.clip_min);
    printf("gmap1_clip_max = %u \n", hdr_cfg->gmap1_cfg.clip_max);
    printf("***************************");

    printf("--- gmap2_cfg ---");
    printf("gmap2_en = %d \n", hdr_cfg->gmap2_cfg.csc_en);
    printf("gmap2_scale = %u \n", hdr_cfg->gmap2_cfg.scale);
    printf("gmap2_ac_coef[3][3] = { \n");
    for (i = 0; i < 3; i++) { /* 3: size */
        printf(" %d, %d, %d \n", hdr_cfg->gmap2_cfg.ac_coef[i][0], hdr_cfg->gmap2_cfg.ac_coef[i][1],
               hdr_cfg->gmap2_cfg.ac_coef[i][2]); /* 1,2: index */
    }
    printf("}\n");
    printf("gmap2_dc_coef_in[3] = { %d, %d, %d }\n", hdr_cfg->gmap2_cfg.dc_coef_in[0], hdr_cfg->gmap2_cfg.dc_coef_in[1],
           hdr_cfg->gmap2_cfg.dc_coef_in[2]); /* 1,2: index */
    printf("gmap2_dc_coef_out[3] = { %d, %d, %d }\n", hdr_cfg->gmap2_cfg.dc_coef_out[0],
           hdr_cfg->gmap2_cfg.dc_coef_out[1], hdr_cfg->gmap2_cfg.dc_coef_out[2]); /* 1,2: index */
    printf("gmap2_clip_min = %u \n", hdr_cfg->gmap2_cfg.clip_min);
    printf("gmap2_clip_max = %u \n", hdr_cfg->gmap2_cfg.clip_max);
    printf("***************************");
}

static td_void vau_hdr_dfx_tone_mapping(const drv_hdr_cfg *hdr_cfg)
{
    td_s32 i = 0, j = 0;
    printf("--- tmap_cfg ---");
    printf("tmap_en = %d \n", hdr_cfg->tmap_cfg.tmap_en);
    printf("tmap_luma_cal_scale = %u \n", hdr_cfg->tmap_cfg.tmap_luma_cal_scale);
    printf("tmap_luma_cal[3] = { %u, %u, %u } \n", hdr_cfg->tmap_cfg.tmap_luma_cal[0],
           hdr_cfg->tmap_cfg.tmap_luma_cal[1], hdr_cfg->tmap_cfg.tmap_luma_cal[2]); /* 1,2: index */
    printf("tmap_mix_alpha = %u \n", hdr_cfg->tmap_cfg.tmap_mix_alpha);
    printf("tmap_mix_alpha_scale = %u \n", hdr_cfg->tmap_cfg.tmap_mix_alpha_scale);
    printf("tmap_coef_scale = %u \n", hdr_cfg->tmap_cfg.tmap_coef_scale);
    printf("tmap_out_clip_min = %u \n", hdr_cfg->tmap_cfg.tmap_out_clip_min);
    printf("tmap_out_clip_max = %u \n", hdr_cfg->tmap_cfg.tmap_out_clip_max);
    printf("tmap_out_dc[3] = { %d, %d, %d } \n", hdr_cfg->tmap_cfg.tmap_out_dc[0], hdr_cfg->tmap_cfg.tmap_out_dc[1],
           hdr_cfg->tmap_cfg.tmap_out_dc[2]); /* 1,2: index */
    printf("tmap_lut[64] = { \n");
    for (i = 0; i < 8; i++) {                                       /* 8: lut size */
        for (j = 0; j < 8; j++) {                                   /* 8: lut size */
            printf(" %u, ", hdr_cfg->tmap_cfg.tmap_lut[i * 8 + j]); /* 8: lut size */
        }
        printf("\n");
    }
    printf("}\n");
    printf("***************************");
}

static td_void vau_hdr_dfx_gamma(const drv_hdr_cfg *hdr_cfg)
{
    td_s32 i = 0, j = 0;
    printf("--- degamma_cfg ---");
    printf("degamma_en = %d \n", (td_s32)hdr_cfg->degamma_cfg.degmma_en);
    printf("degamma_lut[64] = { \n");
    for (i = 0; i < 8; i++) {                                             /* 8: lut size */
        for (j = 0; j < 8; j++) {                                         /* 8: lut size */
            printf(" %u, ", hdr_cfg->degamma_cfg.degamma_lut[i * 8 + j]); /* 8: lut size */
        }
        printf("\n");
    }
    printf("}\n");
    printf("***************************");

    printf("--- gamma_cfg ---");
    printf("gamma_en = %d \n", hdr_cfg->gamma_cfg.gamma_en);
    printf("gamma_lut[64] = { \n");
    for (i = 0; i < 8; i++) {                                         /* 8: lut size */
        for (j = 0; j < 8; j++) {                                     /* 8: lut size */
            printf(" %u, ", hdr_cfg->gamma_cfg.gamma_lut[i * 8 + j]); /* 8: lut size */
        }
        printf("\n");
    }
    printf("}\n");
    printf("***************************");
}

static td_void vau_hdr_dfx_combine(const drv_hdr_cfg *hdr_cfg)
{
    printf("--- combine_cfg ---");
    printf("cmb_en = %d \n", hdr_cfg->combine_cfg.cmb_en);
    printf("base_only_en = %d \n", hdr_cfg->combine_cfg.base_only_en);
    printf("gainmap_only_en = %d \n", hdr_cfg->combine_cfg.gainmap_only_en);
    printf("pow_en = %d \n", hdr_cfg->combine_cfg.pow_en);
    printf("gainmap_size_mode = %d \n", hdr_cfg->combine_cfg.gainmap_size_mode);
    printf("gainmap_components = %u \n", hdr_cfg->combine_cfg.gainmap_components);
    printf("clip_min = %u \n", hdr_cfg->combine_cfg.clip_min);
    printf("clip_max = %u \n", hdr_cfg->combine_cfg.clip_max);
    printf("weight = %f \n", hdr_cfg->combine_cfg.weight);
    printf("gain_min1[3] = { %f, %f, %f } \n", hdr_cfg->combine_cfg.gain_min1[0], hdr_cfg->combine_cfg.gain_min1[1],
           hdr_cfg->combine_cfg.gain_min1[2]); /* 1,2: index */
    printf("gain_min2[3] = { %f, %f, %f } \n", hdr_cfg->combine_cfg.gain_min2[0], hdr_cfg->combine_cfg.gain_min2[1],
           hdr_cfg->combine_cfg.gain_min2[2]); /* 1,2: index */
    printf("gain_max[3] = { %f, %f, %f } \n", hdr_cfg->combine_cfg.gain_max[0], hdr_cfg->combine_cfg.gain_max[1],
           hdr_cfg->combine_cfg.gain_max[2]); /* 1,2: index */
    printf("k_base[3] = { %f, %f, %f } \n", hdr_cfg->combine_cfg.k_base[0], hdr_cfg->combine_cfg.k_base[1],
           hdr_cfg->combine_cfg.k_base[2]); /* 1,2: index */
    printf("k_alternate[3] = { %f, %f, %f } \n", hdr_cfg->combine_cfg.k_alternate[0],
           hdr_cfg->combine_cfg.k_alternate[1], hdr_cfg->combine_cfg.k_alternate[2]); /* 1,2: index */
    printf("gain_min1[3] = { %x, %x, %x } \n", hdr_cfg->combine_cfg.gain_min1[0], hdr_cfg->combine_cfg.gain_min1[1],
           hdr_cfg->combine_cfg.gain_min1[2]); /* 1,2: index */
    printf("gain_min2[3] = { %x, %x, %x } \n", hdr_cfg->combine_cfg.gain_min2[0], hdr_cfg->combine_cfg.gain_min2[1],
           hdr_cfg->combine_cfg.gain_min2[2]); /* 1,2: index */
    printf("gain_max[3] = { %x, %x, %x } \n", hdr_cfg->combine_cfg.gain_max[0], hdr_cfg->combine_cfg.gain_max[1],
           hdr_cfg->combine_cfg.gain_max[2]); /* 1,2: index */
    printf("***************************");
}

static td_void vau_hdr_dfx_scale(const drv_hdr_cfg *hdr_cfg)
{
    printf("--- scale_cfg ---");
    printf("scale_en = %d \n", hdr_cfg->scale_cfg.scale_en);
    printf("gain[3] = { %u, %u, %u, %u } \n", hdr_cfg->scale_cfg.gain[0], hdr_cfg->scale_cfg.gain[1],
           hdr_cfg->scale_cfg.gain[2], hdr_cfg->scale_cfg.gain[3]); /* 1,2,3: index */
    printf("offset[3] = { %d, %d, %d, %d } \n", hdr_cfg->scale_cfg.offset[0], hdr_cfg->scale_cfg.offset[1],
           hdr_cfg->scale_cfg.offset[2], hdr_cfg->scale_cfg.offset[3]); /* 1,2,3: index */
    printf("clip_min[3] = { %u, %u, %u, %u } \n", hdr_cfg->scale_cfg.clip_min[0], hdr_cfg->scale_cfg.clip_min[1],
           hdr_cfg->scale_cfg.clip_min[2],  /* 1,2,3: index */
           hdr_cfg->scale_cfg.clip_min[3]); /* 3: index */
    printf("clip_max[3] = { %u, %u, %u, %u } \n", hdr_cfg->scale_cfg.clip_max[0], hdr_cfg->scale_cfg.clip_max[1],
           hdr_cfg->scale_cfg.clip_max[2],  /* 1,2: index */
           hdr_cfg->scale_cfg.clip_max[3]); /* 3: index */
    printf("***************************");
}
#endif

static td_void vau_hdr_dfx(drv_hdr_cfg *hdr_cfg)
{
#ifdef HDR_ALG_DFX_SUPPORT
    vau_hdr_dfx_y2r(hdr_cfg);
    vau_hdr_dfx_gamut_mapping(hdr_cfg);
    vau_hdr_dfx_tone_mapping(hdr_cfg);
    vau_hdr_dfx_gamma(hdr_cfg);
    vau_hdr_dfx_combine(hdr_cfg);
    vau_hdr_dfx_scale(hdr_cfg);
#else
    uapi_unused(hdr_cfg);
#endif
}

td_s32 vau_hdr_get_cfg(drv_hdr_param *hdr_param, drv_hdr_cfg *hdr_cfg)
{
    td_s32 ret;

    ret = vau_hdr_set_enable(hdr_param, hdr_cfg);

    ret = vau_hdr_set_y2r(hdr_param, hdr_cfg);

    ret = vau_hdr_set_degmm(hdr_param, hdr_cfg);

    ret = vau_hdr_set_gmap(hdr_param, hdr_cfg);

    ret = vau_hdr_set_tm(hdr_param, hdr_cfg);

    ret = vau_hdr_set_gmm(hdr_cfg);

    ret = vau_hdr_set_combine(hdr_param, hdr_cfg);

    ret = vau_hdr_set_scale(hdr_param, hdr_cfg);

    vau_hdr_dfx(hdr_cfg);

    return ret;
}
