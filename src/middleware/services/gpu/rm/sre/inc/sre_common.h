/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description : sre common head file
 */

#ifndef __SRE_COMMON_H__
#define __SRE_COMMON_H__

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include "securec.h"
#include "sre_api.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define NO_CHANGE_ALPHA_MAX              (96)
#define CHANGE_ALPHA_MIN                 (192)
#define SCENE_CHANGE_WEIGHT_MAX          (128)
#define SCENE_CHANGE_WEIGHT_BIT          (7)
#define SCENE_CHANGE_HIST_SHIFT          (3)

#define NORMALIZE_BIT                    (16)
#define NORMALIZE_SIZE                   (1<<NORMALIZE_BIT)

#define SCENE_CHANGE_ALPHA_MIN           (16)
#define SCENE_CHANGE_ALPHA_MAX           (256)
#define SCENE_CHANGE_ALPHA_BIT           (8)

#define PARAMETER_FIX                    (128.0)
#define PARAMETER_FIX_BIT                (7)

#define POWX_SIZE                  (140)
#define POWX_LEVEL_A               (100)
#define POWX_LEVEL_B               (500)
#define POWX_STEP_B                (10)

#ifdef SRE_DEBUG_PRINT
#define DLOG_E(format, ...) printf("E/%s (%4d): " format "\n", __FUNCTION__, __LINE__, __VA_ARGS__)
#define DLOG_W(format, ...) printf("W/%s (%4d): " format "\n", __FUNCTION__, __LINE__, __VA_ARGS__)
#define DLOG_I(format, ...) printf("I/%s (%4d): " format "\n", __FUNCTION__, __LINE__, __VA_ARGS__)
#define DLOG_D(format, ...) printf("D/%s (%4d): " format "\n", __FUNCTION__, __LINE__, __VA_ARGS__)
#define DLOG_V(format, ...) printf("V/%s (%4d): " format "\n", __FUNCTION__, __LINE__, __VA_ARGS__)
#define PRINT printf
#else
#define DLOG_E(format, ...)
#define DLOG_W(format, ...)
#define DLOG_I(format, ...)
#define DLOG_D(format, ...)
#define DLOG_V(format, ...)
#define PRINT(format, ...)
#endif


int detect_scene_change(contrast_proc_info *p_con_mem, const scene_change_params *pst_sc_params, int bin_max_th,
    int alphamin, int alphamax);

int iir_filter_int(int prevalue, int curvalue, int alpha);

void mean_filter_int(int *in_arr, int *out_arr, int len);
void mean_filter_tmf_int(int *in_arr, int *out_arr, int len, int shiftbit);

void calculate_global_local_var_and_average(contrast_proc_info *p_con_mem);

float linear_inter2f(float v, float x0, float y0, float x1, float y1);
int linear_inter2i(int v, int x0, int y0, int x1, int y1);
int rshft_v0(int x, unsigned int n);
int rshft(int x, unsigned int n);
int lshft(int x, unsigned int n);

void normalize_histograms(contrast_proc_info *p_con_mem);

void fix_sloop_limit_flat_noise12(int sloop[HISTBIN], int *pgbin_usage, int *p_local_hist, int ratio_th, int histsumax,
    int histsumin, int histadd_th, int weightmin, int startpos, int endpos, float histmincoef, float histmaxcoef);

void cut_histogram_add(int *p_hist_in, unsigned char *histw, int *p_hist_out, int cut_hist_th, int add_hist_th,
    int len);

float get_simkin_l(float la);
float simkin_core_new(float simkin_l, float l);
float log_approx(float x);
float exp_approx(float val);
float pow_approx(float a, float b, int use_approximate);
void fna_classfier(contrast_proc_info *p_con_mem);
int find_peak_hist_neighbor(int *p_nor_hist);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
