/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description : api for sre
 */
 
#ifndef __SRE_API_H__
#define __SRE_API_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define DRE_CFG_MAX_PATH (512)

#define XBLOCKNUM (1)
#define YBLOCKNUM (1)

#define HISTBIT (5)
#define HISTBIN (1 << HISTBIT)
#define HISTSHIFTBIT ((8) - HISTBIT)
#define BITWIDTH (10)
#define GAMMABIT HISTBIT
#define GAMMABIN (1 << GAMMABIT)
#define SHIFTBIT (BITWIDTH - GAMMABIT)

#define HIACE_MAX_CNT 0x7FFFFF        // ((1<<23)-1) //22bit->23bit
#define HIACE_MAX_GLOBAL_CNT 0x7FFFFF // ((1<<23)-1) //23bit
#define HIACE_MAX_LOCAL_CNT 0xFFFFF   // ((1<<20)-1) //20bit

#define HIACE_MAX_DIFF 0xFFFF00     // ((1<<24) - 256)   //24bit
#define HIACE_MAX_SUMY 0x3FFFF00    // ((1<<26) - 256)   //26bit
#define HIACE_MAX_SUMSAT 0x7FFFFE00 // ((1<<31) - 256)   //31bit  if subtract 1, maybe overflow, must subtract 256

#define HIACE_MAX_FNA_MID 0x3FFFF // U18
#define HIACE_MAX_FNA_FINAL 0x3FF // U10

#define HIACE_GHIST_RANK (32)
#define HIACE_LHIST_RANK (16)
#define HIACE_GAMMA_RANK (8)

typedef struct dynamic_range_expansion_params {
    // 消除黑边白边干扰参数
    int global_hist_black_pos;
    int global_hist_white_pos;
    int global_hist_black_weight;
    int global_hist_white_weight;
    int global_hist_zero_cut_ratio;
    int global_hist_slope_cut_ratio;

    // 算法使能变量
    int do_sre;

    // SRE开启的最小环境光
    int la_sensor_sre_on_th;

    int width; // 根据通路上输入每帧的尺寸设置
    int height;
    int bit_width;
    int mode;
    int level; // 视频(0) 图库(1) UI(2)

    int max_back_light;
    int min_back_light;
    int back_light;

    int global_gain;
} dynamic_range_expansion_params;

typedef struct ace_params {
    // parameters for cutoff of sloop
    int local_sloop_based;
    int local_sloop_min_th;
    float local_sloop_add_by_ge_min_th;
    float local_sloop_add_by_ge_max_th;
    int local_sloop_add_max_th;
    float local_sloop_sub_by_ge_min_th;
    float local_sloop_sub_by_ge_max_th;
    int local_sloop_sub_max_th;
    int local_max_bright_th;

    // parameters for weight of global histogram
    int global_hist_weight_max_th;
    int global_hist_weight_min_th;
    float global_hist_weight_by_ge_min_th;
    float global_hist_weight_by_ge_max_th;
    float global_hist_weight_by_le_min_th;

    // parameters for limit noise in flat area
    int limit_flat_noise_max_hist_th;
    int limit_flat_noise_min_hist_th;
    int limit_flat_noise_min_weight_th;
    int limit_flat_noise_min_sloop;
    int limit_flat_noise_min_pos_th;
    int limit_flat_noise_max_pos_th;
    float limit_flat_noise_local_min_hist;
    float limit_flat_noise_local_max_hist;

    // parameters for limit become darker/lighter too much
    int limit_darker_max_th;
    int limit_darker_min_th;
    int darker_average_max_th;
    int darker_average_min_th;
    int limit_lighter_max_ace_th;
    int limit_lighter_max_sre_th;
    int contrast_lose_ace_weight_th;
    int min_slope_max_by_zero_num_th;
    int min_slope_min_by_zero_num_th;
    int mid_light_darker_max_th;
    int cut_mid_light_hist_min_th;
    // parameters for cut histogram
    int cut_hist_ratio_max_th;
    int cut_hist_ratio_min_th;
    int cut_hist_ratio_bin0_max_th;
    // parameters for get threshold of cut histogram
    int cd_hist_th_dark_ratio_max;
    int cd_hist_th_dark_ratio_min;
    int cd_hist_th_light_ratio_max;
    int cd_hist_th_light_ratio_min;
    // parameters for get length of cut histogram
    int cd_h_dark_len_ratio_min_th;
    int cd_h_dark_len_ratio_max_th;
    int cd_h_dark_len_max_th;
    int cd_h_dark_len_min_th;
    // parameters for cut continue histogram
    int cut_con_hist_start_th;
    int cut_con_hist_low_th;
    int cut_con_hist_high_th;
    int cut_con_hist_min_num_th;
    int cut_con_hist_low_min_th;
    int cut_con_hist_high_min_th;
    int cut_con_hist_min_num_min_th;
    int cut_con_average_pixel_min_th;
    int cut_con_average_pixel_max_th;
    int small_hist_max_th;
    int cut_dark_hist_min_sre_th;
    float cut_dark_hist_local_weight;

    // parameters for global fixed gamma
    int gamma_light_max_th;
    int gamma_zero_ave_a;
    int gamma_zero_ave_b;
    int gamma_zero_ave_c;
    int gamma_zero_ave_d;
    int gamma_zero_pos_a;
    int gamma_zero_pos_b;
    int gamma_zero_pos_c;
    int gamma_zero_pos_d;
    int gamma_dark_slope_min_th;
    int gamma_dark_slope_max_th;
    int gamma_light_slope_min_th;
    int gamma_light_slope_max_th;

    int scene_info_night_local_sloop;
    float scene_info_night_hist_coef;
    float scene_info_back_light_hist_coef;
} ace_params;

typedef struct sre_params {
    int sensor_la_max_ace;
    int sensor_la_min_sre;
    int sensor_la_mid_sre;
    int sensor_la_max_sre;

    int tmf_la_min_sre;
    int tmf_la_mid_sre;
    int tmf_la_max_sre;

    int local_sloop_based_min_sre;
    int local_sloop_based_max_sre;

    int cut_hist_ratio_min_sre;

    int contrast_lose_sre_weight_th;
    float cslf_gamma;
    float delta_max_cslf_gamma;
    float min_hist_coef;
    int sre_iir_fliter;
} sre_params;

typedef struct scene_change_params {
    int global_diff_min_th;
    int global_diff_max_th;
    int local_diff_min_th;
    int local_diff_max_th;
    int no_change_num_min_th;
    int no_change_num_max_th;
    int ave_alpha_min_th;
    int ave_alpha_max_th;
} scene_change_params;

typedef struct classifier_features {
    int y_abs_diff_d1;  // d1 (horizontal differences between Y(x,y) ans Y(x+1,y))	 10bit
    int y_abs_diff_d2;  // d1 (horizontal differences between Y(x,y) ans Y(x+Shift,y))	10bit
    int y_av;           // average value of Y channel in block	10bit
} classifier_features;

typedef struct algo_scene_info {
    int night_result;
    int iso;
} algo_scene_info;

typedef struct fna_responds {
    float object_respond;
    float flat_respond;
    float noise_respond;
    float average;
    float max_local_hist;
    float width_hist;
} fna_responds;

typedef struct fna_num {
    int object_num;
    int flat_num;
    int noise_num;
} fna_num;

typedef struct contrast_proc_info {
    // 不是同一张图就不能保留的REG寄存器如下8个:
    int reg_fixbit_y;
    int reg_reciprocal_y;
    int reg_fixbit_x;
    int reg_reciprocal_x;

    int pre_zero_pos;
    int frame_num_th;
    int frame_num;
    int start_flag;
    int alpha_src;
    int pre_la;

    int imode;
    float avezeronum;
    float bs_ratio;
    int global_var;
    int ace_g_dark_len;
    int ace_dark_len;
    int ace_g_dark_hist_th;
    int ace_dark_hist_th;
    int ace_add_hist_th;

    int ace_darker_max_th;
    int ace_darker_min_th;

    int ace_cut_dark_hist_th;
    int lightvar;
    int ace_end_point;

    int ace_light_start_add;
    int ace_dark_start_add;

    int cut_dark_hist_max;
    int cut_dark_hist_min;

    float ace_flat;
    int sum_sat;  // sum of saturation                                 // U26

    int max_hist_bin_neighbor;
    int no_change_num;
    algo_scene_info st_scene_info;
    int backlightflag;
    int nightflag;
    float cut_dark_hist_local_weight;
    int light_sum;

    float entropy[YBLOCKNUM * XBLOCKNUM + 1];
    int zeronum[YBLOCKNUM * XBLOCKNUM + 1];
    int avepixel[YBLOCKNUM * XBLOCKNUM + 1];
    int pre_nor_global_hist[HISTBIN];
    int mod_global_hist[HISTBIN];
    int nor_global_hist[HISTBIN];
    int global_hist[HISTBIN];  // to store global histogram, 32*bit counter * 32bin = 1024bit

    int global_gamma[HISTBIN];

    fna_num st_fna_num;
    classifier_features clss_features;  // features of classifier for each block

    int global_sat_hist_sum;
    int global_sat_hist_cum;

    int do_sre;
    int pre_max_bright_amp;

    int ambientLight;

    /////////////////////////////////////////////////////////////////
    int nor_local_hist[YBLOCKNUM * XBLOCKNUM * HISTBIN];
    int mod_local_hist[YBLOCKNUM * XBLOCKNUM * HISTBIN];
    int local_hist[YBLOCKNUM * XBLOCKNUM * HISTBIN];
    int local_gamma[YBLOCKNUM * XBLOCKNUM * HISTBIN];
    int local_gammaf[YBLOCKNUM * XBLOCKNUM * HISTBIN];
    int pre_nor_local_hist[YBLOCKNUM * XBLOCKNUM * HISTBIN];
} contrast_proc_info;

typedef struct contrast_proc_frame {
    char input_hist_file[255];
    char input_la_file[255];
    char output_file[255];
    int start_frame;
    int frame_num;
} contrast_proc_frame;

// 涉及到调试的参数，需要寄存器开出来
typedef struct contrast_proc_params {
    dynamic_range_expansion_params st_dre_params;
    sre_params st_sre_params;
    ace_params st_ace_params;
    scene_change_params st_sc_params;
} contrast_proc_params;

typedef struct contrast_proc_inputs {
    int global_hist[HISTBIN];
    int global_y_sum;
    int global_y_diff_d1_sum;
    int global_y_diff_d2_sum;
    int global_sat_sum;
    int global_sat_hist_sum;
    int global_sat_hist_cum;

    int la;
} contrast_proc_inputs;

void contrast_init(const contrast_proc_params *st_cp_params, contrast_proc_info *st_cp_info);
void contrast_process(contrast_proc_inputs *st_cp_inputs, contrast_proc_params *st_cp_params,
    contrast_proc_info *st_cp_info, int *st_cp_outputs);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
