/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description : dpu sre file
 */

#include "dpu_sre.h"
#include "sre_api.h"
#include "dpu_hal.h"
#include "dpu_disp.h"

STATIC td_void dpu_get_sub_acc_hist_32bin(td_u32 hist[8], const td_u32 ycnt32_data[5])
{
    hist[0] = ycnt32_data[0] & 0xFFFFF;
    hist[1] = ((ycnt32_data[1] & 0xFF) << 12) | ((ycnt32_data[0] & 0xFFF00000) >> 20); // 1 index 12 20 offset
    hist[2] = (ycnt32_data[1] & 0xFFFFF00) >> 8; // 2 index 8 offset
    hist[3] = ((ycnt32_data[2] & 0xFFFF) << 4) | ((ycnt32_data[1] & 0xF0000000) >> 28); // 3 index  2 4 28 offset
    hist[4] = ((ycnt32_data[3] & 0xF) << 16) | ((ycnt32_data[2] & 0xFFFF0000) >> 16); // 4 index 3 16 2 offset
    hist[5] = (ycnt32_data[3] & 0xFFFFF0) >> 4; // 5 index 3 4 offset
    hist[6] = ((ycnt32_data[4] & 0xFFF) << 8) | ((ycnt32_data[3] & 0xFF000000) >> 24); // 6 index 4 3 8 24 offset
    hist[7] = (ycnt32_data[4] & 0xFFFFF000) >> 12; // 7 index 4 12 offset
}

STATIC td_void dpu_get_acc_hist_32bin(td_u32 hist[32], const td_u32 ycnt32_data[20])
{
    for (td_u32 i = 0; i < 4; i++) { // 4 num
        td_u32 hist_offset = i * 8;  // 8 offset
        td_u32 ycnt32_data_offset = i * 5;  // 5 offset
        dpu_get_sub_acc_hist_32bin(&hist[hist_offset], &ycnt32_data[ycnt32_data_offset]);
    }
}

contrast_proc_params g_sre_proc_para = {
    .st_dre_params.width                           = 466,
    .st_dre_params.height                          = 466,
    .st_dre_params.bit_width                       = 10,
    .st_dre_params.mode                            = 0,
    .st_dre_params.level                           = 0,
    .st_dre_params.do_sre                          = 1,
    .st_dre_params.la_sensor_sre_on_th             = 0,
    .st_dre_params.max_back_light                  = 255,
    .st_dre_params.min_back_light                  = 0,
    .st_dre_params.back_light                      = 102,
    .st_dre_params.global_hist_black_pos           = 16,
    .st_dre_params.global_hist_white_pos           = 240,
    .st_dre_params.global_hist_black_weight        = 51,
    .st_dre_params.global_hist_white_weight        = 51,
    .st_dre_params.global_hist_zero_cut_ratio      = 486,
    .st_dre_params.global_hist_slope_cut_ratio     = 410,
    .st_dre_params.global_gain                     = 32,

    .st_ace_params.local_sloop_based               = 2867,
    .st_ace_params.local_sloop_min_th              = 2253,
    .st_ace_params.local_max_bright_th             = 0,
    .st_ace_params.local_sloop_add_by_ge_min_th    = 2.8,
    .st_ace_params.local_sloop_add_by_ge_max_th    = 3.2,
    .st_ace_params.local_sloop_add_max_th          = 512,
    .st_ace_params.local_sloop_sub_by_ge_min_th    = 1.7,
    .st_ace_params.local_sloop_sub_by_ge_max_th    = 2.2,
    .st_ace_params.local_sloop_sub_max_th          = 410,
    .st_ace_params.global_hist_weight_max_th       = 64,
    .st_ace_params.global_hist_weight_min_th       = 13,
    .st_ace_params.global_hist_weight_by_ge_min_th = 1.8,
    .st_ace_params.global_hist_weight_by_ge_max_th = 2.8,
    .st_ace_params.global_hist_weight_by_le_min_th = 1.6,
    .st_ace_params.limit_flat_noise_max_hist_th    = 44,
    .st_ace_params.limit_flat_noise_min_hist_th    = 31,
    .st_ace_params.limit_flat_noise_min_weight_th  = 51,
    .st_ace_params.limit_flat_noise_min_sloop      = 115,
    .st_ace_params.limit_flat_noise_min_pos_th     = 7,
    .st_ace_params.limit_flat_noise_max_pos_th     = 16,
    .st_ace_params.limit_flat_noise_local_min_hist = 3,
    .st_ace_params.limit_flat_noise_local_max_hist = 6.6,
    .st_ace_params.limit_darker_max_th             = 96,
    .st_ace_params.limit_darker_min_th             = 72,
    .st_ace_params.darker_average_max_th           = 148,
    .st_ace_params.darker_average_min_th           = 90,
    .st_ace_params.limit_lighter_max_ace_th        = 45,
    .st_ace_params.limit_lighter_max_sre_th        = 80,
    .st_ace_params.min_slope_max_by_zero_num_th    = 18,
    .st_ace_params.min_slope_min_by_zero_num_th    = 14,
    .st_ace_params.mid_light_darker_max_th         = 24,
    .st_ace_params.cut_mid_light_hist_min_th       = 256,
    .st_ace_params.contrast_lose_ace_weight_th     = 51,
    .st_ace_params.cut_hist_ratio_max_th           = 448,
    .st_ace_params.cut_hist_ratio_min_th           = 180,
    .st_ace_params.cut_hist_ratio_bin0_max_th      = 384,
    .st_ace_params.cd_hist_th_dark_ratio_max       = 56,
    .st_ace_params.cd_hist_th_dark_ratio_min       = 36,
    .st_ace_params.cd_hist_th_light_ratio_max      = 15,
    .st_ace_params.cd_hist_th_light_ratio_min      = 9,
    .st_ace_params.cd_h_dark_len_ratio_max_th      = 109,
    .st_ace_params.cd_h_dark_len_ratio_min_th      = 64,
    .st_ace_params.cd_h_dark_len_max_th            = 1280,
    .st_ace_params.cd_h_dark_len_min_th            = 512,
    .st_ace_params.cut_con_hist_start_th           = 3,
    .st_ace_params.cut_con_hist_low_th             = 230,
    .st_ace_params.cut_con_hist_high_th            = 384,
    .st_ace_params.cut_con_hist_min_num_th         = 192,
    .st_ace_params.cut_con_hist_min_num_min_th     = 192,
    .st_ace_params.cut_con_hist_low_min_th         = 230,
    .st_ace_params.cut_con_hist_high_min_th        = 384,
    .st_ace_params.cut_con_average_pixel_min_th    = 40,
    .st_ace_params.cut_con_average_pixel_max_th    = 100,
    .st_ace_params.small_hist_max_th               = 128,
    .st_ace_params.cut_dark_hist_min_sre_th        = 64,
    .st_ace_params.cut_dark_hist_local_weight      = 0.5,
    .st_ace_params.gamma_light_max_th              = 20,
    .st_ace_params.gamma_zero_ave_a                = 40,
    .st_ace_params.gamma_zero_ave_b                = 80,
    .st_ace_params.gamma_zero_ave_c                = 130,
    .st_ace_params.gamma_zero_ave_d                = 170,
    .st_ace_params.gamma_zero_pos_a                = 40,
    .st_ace_params.gamma_zero_pos_b                = 60,
    .st_ace_params.gamma_zero_pos_c                = 80,
    .st_ace_params.gamma_zero_pos_d                = 90,
    .st_ace_params.gamma_dark_slope_min_th         = 141,
    .st_ace_params.gamma_dark_slope_max_th         = 151,
    .st_ace_params.gamma_light_slope_min_th        = 146,
    .st_ace_params.gamma_light_slope_max_th        = 156,
    .st_ace_params.scene_info_night_local_sloop    = 2048,
    .st_ace_params.scene_info_night_hist_coef      = 1.0,
    .st_ace_params.scene_info_back_light_hist_coef = 1.0,

    .st_sc_params.global_diff_min_th               = 100,
    .st_sc_params.global_diff_max_th               = 900,
    .st_sc_params.local_diff_min_th                = 600,
    .st_sc_params.local_diff_max_th                = 1600,
    .st_sc_params.no_change_num_min_th             = 1,
    .st_sc_params.no_change_num_max_th             = 12,
    .st_sc_params.ave_alpha_min_th                 = 80,
    .st_sc_params.ave_alpha_max_th                 = 160,
    .st_sre_params.sensor_la_max_ace               = 1000,
    .st_sre_params.sensor_la_min_sre               = 5000,
    .st_sre_params.sensor_la_mid_sre               = 15000,
    .st_sre_params.sensor_la_max_sre               = 40000,
    .st_sre_params.tmf_la_min_sre                  = 4000,
    .st_sre_params.tmf_la_mid_sre                  = 8000,
    .st_sre_params.tmf_la_max_sre                  = 20000,
    .st_sre_params.local_sloop_based_min_sre       = 4096,
    .st_sre_params.local_sloop_based_max_sre       = 8192,
    .st_sre_params.cut_hist_ratio_min_sre          = 384,
    .st_sre_params.contrast_lose_sre_weight_th     = 38,
    .st_sre_params.cslf_gamma                      = 0.45,
    .st_sre_params.delta_max_cslf_gamma            = 0.15,
    .st_sre_params.min_hist_coef                   = 0.75,
    .st_sre_params.sre_iir_fliter                  = 2,    // 时序平滑度配置 典型值 2，配置256关闭时序平滑
};

STATIC contrast_proc_info g_sre_proc_info = {};
STATIC td_void dpu_sre_init(td_void)
{
    g_sre_proc_para.st_dre_params.width = (td_s32)dpu_disp_get_srceen_width();
    g_sre_proc_para.st_dre_params.height = (td_s32)dpu_disp_get_srceen_height();
    contrast_init(&g_sre_proc_para, &g_sre_proc_info);
}

#define DPU_ACC_LUT_NUM (HISTBIN + 1)

td_void dpu_sre_process(td_void)
{
    static td_bool sre_first = TD_TRUE;
    static td_bool is_acc_en_set = TD_FALSE;
    if (dpu_disp_is_la_valid() == TD_FALSE) {
        dpu_hal_set_acc_stat_en(TD_FALSE);
        dpu_hal_set_acc_en(TD_FALSE);
        is_acc_en_set = TD_FALSE;
        sre_first = TD_TRUE;
        return;
    }

    if (sre_first) { // 第一帧开ACC统计, 第二帧开SRE映射
        dpu_sre_init();
        td_u32 screen_width  = dpu_disp_get_srceen_width();
        td_u32 screen_height = dpu_disp_get_srceen_height();
        dpu_hal_set_acc_stat_en(TD_TRUE);
        dpu_hal_set_acc_ctrl(0, screen_width - 1, 0, screen_height - 1);
        dpu_hal_set_acc_stat_ctrl(0, screen_width - 1, 0, screen_height - 1);
        sre_first = TD_FALSE;
        return;
    }

    if (is_acc_en_set == TD_FALSE) {
        dpu_hal_set_acc_en(TD_TRUE);
        is_acc_en_set = TD_TRUE;
    }

    // 获取软算法输入信息
    dpu_hal_acc_info acc_info = {};
    dpu_hal_get_acc_info(&acc_info);

    contrast_proc_inputs inputs = {
        .global_y_sum = acc_info.acc_y_sum,
        .global_y_diff_d1_sum = acc_info.acc_ydiffd1_sum,
        .global_y_diff_d2_sum = acc_info.acc_ydiffd2_sum,
        .global_sat_sum = acc_info.acc_sat_sum,
        .global_sat_hist_sum = acc_info.acc_sat_hist_sum,
        .global_sat_hist_cum = acc_info.acc_sat_hist_cum,
        .la = dpu_disp_get_la()
    };

    dpu_get_acc_hist_32bin((td_u32 *)inputs.global_hist, acc_info.ycnt32_data);

    // 软算法处理
    td_s32 dpu_acc_lut[DPU_ACC_LUT_NUM] = {};
    contrast_process(&inputs, &g_sre_proc_para, &g_sre_proc_info, dpu_acc_lut);

    // 软算法输出映射表配置
    dpu_hal_set_acc_lut(dpu_acc_lut, DPU_ACC_LUT_NUM);
}