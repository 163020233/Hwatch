/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description : sre process head file
 */

#ifndef __SRE_PROC_H__
#define __SRE_PROC_H__

#include <string.h>
#include "sre_common.h"
#include "sre_api.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#define ACE_FNAFALG                                                (1)

#define NOISE_NUM_MINTH                                            (6)
#define NOISE_NUM_MAXTH                                            (24)

#define TMF_BUILD_LA_ACE                                           (84)

#define TMF_BUILD_HIST_COEF                                        (125)
#define TMF_BUILD_STEP                                             (0.25)
#define TMF_BUILD_LIMIT_CSLF                                       (2.4)
#define CSLF_SIZE                                                  (2458)
#define TMF_FIX_BIT                                                (10)
#define TMF_DIFF_BIT                                               (5)

// parameters for fix intersection	o¨º?¡§¨°?D¨¨DT??
#define FIX_INTERSECT_HIST                                         (209715) // 3.2*65536
#define FIX_INTERSECT_DARK_WEIGHT                                  (0.3)
#define FIX_INTERSECT_BRIGHT_WEIGHT                                (0.0)
#define INTERSECTION_HIST_SUB                                      (52429) // 0.8*65536
#define INTERSECTION_HIST_ADD                                      (39321) // 0.6*65536
#define INTERSECTION_START_POS                                     (2)
#define INTERSECTION_POS_RANGE                                     (4)
#define INTERSECTION_WEIGHT_STEP                                   (0.1)

#define LIMIT_FLAT_NOISE_ADD_HIST                                  (6554)

// parameters for cut bright histogram
#define CUT_BRIGHT_HIST_LEN                                        (4)
#define CUT_BRIGHT_HIST_RATIO                                      (2048 * 2.2) // 0.1625*65536
#define CUT_BRIGHT_HIST_ADD_POS                                    (5)

// parameters for weight of global histogram
#define GLOBAL_WEIGHT_ADD_MAX                                      (25) // 0.3*128
#define GLOBAL_WEIGHT_ZERO_NUM_MIN                                 (6)
#define GLOBAL_WEIGHT_ZERO_NUM_MAX                                 (16)

// parameters for cut continue histogram
#define CUT_CONTINUE_HIST_LEN                                      (3)
#define CUT_CONTINUE_HIST_ADD_COEF                                 (0.8)

// parameters for cut dark histogram
#define CUT_DARK_HIST_DARK_LEN                                     (6)
#define CUT_DARK_HIST_BRIGHT_LEN                                   (4)
#define CUT_DARK_HIST_ALL_START_POS                                (1)
#define CUT_DARK_HIST_ALL_END_POS                                  (3)
#define CUT_DARK_HIST_ALL_RATIO                                    (4096) // 0.0625*65536

#define CUT_DARK_HIST_DARK_WEIGHT_MIN                              (0.5)
#define CUT_DARK_HIST_DARK_WEIGHT_MAX                              (0.8)
#define CUT_DARK_HIST_LIGHT_WEIGHT_MAX                             (0.7)
#define CUT_DARK_HIST_LIGHT_WEIGHT_MIN                             (0.5)
#define CUT_DARK_HIST_DARK_RATIO_MIN                               (19660) // 0.3*65536
#define CUT_DARK_HIST_DARK_RATIO_MAX                               (26870) // 0.41*65536
#define CUT_DARK_HIST_LIGHT_RATIO_OFFSET                           (1966) // 0.03*65536

#define CUT_DARK_GLOBAL_HIST_VAR_MIN                               (512) // 4*PARAMETER_FIX
#define CUT_DARK_GLOBAL_HIST_VAR_MAX                               (768) // 6*PARAMETER_FIX
#define CUT_DARK_GLOBAL_HIST_AVE_MIN                               (32)
#define CUT_DARK_GLOBAL_HIST_AVE_MAX                               (128)
#define CUT_DARK_GLOBAL_HIST_RATIO_COEF                            (0.8)
#define CUT_DARK_GLOBAL_HIST_LEN_AVE_MIN                           (32)
#define CUT_DARK_GLOBAL_HIST_LEN_AVE_MAX                           (88)
#define CUT_DARK_GLOBAL_HIST_LEN_MAX                               (1280) // 10*PARAMETER_FIX
#define CUT_DARK_GLOBAL_HIST_LEN_MIN                               (384) // 3*PARAMETER_FIX

#define HIGH_LIGHT_PART_RATIO_MIN                                  (0.000001)
#define HIGH_LIGHT_PART_ZERO_RATIO                                 (0.0001)
#define HIGH_LIGHT_PART_DARK_RATIO_MIN                             (0.6)
#define HIGH_LIGHT_PART_DARK_RATIO_MAX                             (0.8)

#define ADD_HIST_VAR_MIN                                           (256) // 2*PARAMETER_FIX
#define ADD_HIST_VAR_MAX                                           (768) // 6*PARAMETER_FIX
#define ADD_HIST_COEF_MIN                                          (1.2)

#define DARKER_MAX_RATIO_MIN                                       (1311) // 0.02*65536
#define DARKER_MAX_RATIO_MAX                                       (5243) // 0.08*65536
#define DARKER_MAX_RATIO_COEF                                      (0.8)

#define DARK_HIST_GLOBAL_WEIGHT                                    (0.2)

#define HIST_LIGHT_START_ADD_MIN                                   (512) // 4*PARAMETER_FIX
#define HIST_DARK_START_ADD_MIN                                    (640) // 5*PARAMETER_FIX

#define DARK_LEN_WEIGHT_MIN                                        (0.2)
#define DARK_LEN_HIST_COEF                                         (1.1)

#define ADD_HIST_LEN                                               (8)
#define HISTHALF                                                   (HISTBIN>>1)
#define BALANCE_HIST_DARK_START                                    (4)
#define BALANCE_HIST_DARK_SUB_END                                  (12)
#define BALANCE_HIST_LIGHT_RATIO_MIN                               (0.43)
#define BALANCE_HIST_LIGHT_RATIO_MAX                               (0.68)
#define BALANCE_HIST_LEFT_RATIO_MAX                                (1.5)
#define BALANCE_HIST_LIGHT_RATIO_SUITABLE                          (26214) // 0.4*65536
#define BALANCE_HIST_LIGHT_HIST_COEF_MIN                           (0.8)
#define BALANCE_HIST_DARK_RATIO_SUITABLE                           (27853) // 0.425*65536
#define BALANCE_HIST_DARK_HIST_COEF_MIN                            (0.9)
#define BALANCE_HIST_LIGHT_HIST_ADD_START                          (20)

#define LIGHT_VAR_ACE_COEF                                         (1.1)
#define LIGHT_VAR_SRE_COEF                                         (1.0)
#define LIGHT_VAR_MIN_COEF                                         (1.5)
#define LIGHT_VAR_MAX_COEF                                         (7.2)
#define SRE_MIN_HIST_LEN                                           (5)

#define LIMIT_TMF_DARK_LEVEL_MIN                                   (-20.0)
#define LIMIT_TMF_DARK_LEVEL_MAX                                   (-4.0)

#define FIXBIT                                    (7)
#define FIXBITHALF                                (1<<(FIXBIT - 1))
#define HISTBINSRC                                (256)

#define SLOPE_HIST_DARK_POS                       (10)
#define SLOPE_HIST_BRIGHT_POS                     (246)
#define SLOPE_HIST_DARK_WEIGHT                    (64)
#define SLOPE_HIST_BRIGHT_WEIGHT                  (64)

#define ZERO_HIST_AVE_PIXEL_MIN                   (40)
#define ZERO_HIST_AVE_PIXEL_MAX                   (80)

#define SLOPE_DARK_LEN                            (52)
#define SLOPE_DARK_RATIO_MIN                      (0.12)
#define SLOPE_DARK_RATIO_MAX                      (0.2)
#define SLOPE_DARK_RATIO_COEF                     (0.4)
#define SLOPE_DARK_RATIO_WEIGHT                   (1.0)

#define SLOPE_BRIGHT_LEN                          (96)
#define SLOPE_BRIGHT_RATIO_MIN                    (0.22)
#define SLOPE_BRIGHT_RATIO_MAX                    (0.35)
#define BRIGHT_POINT_COEF                         (0.5)
#define DARK_POINT_COEF                           (0.5)

#define SGAMMA_LIMIT_NOISE_LEN                    (8)
#define SGAMMA_LIMIT_NOISE_NUM_COEF_MIN           (2.5)
#define SGAMMA_LIMIT_NOISE_NUM_COEF_MAX           (4.0)
#define SGAMMA_SLOPE_MIN                          (0.9)
#define SGAMMA_END_POINT                          (232)
#define SGAMMA_END_POINT_MIN                      (216)
#define ZERO_POSITION_BRIGHT_MAX                  (5)


typedef struct cut_cont_high_hist_params {
    int len;
    int cutstart_th;
    int mid_light_hist_th;
    int pixelnumin_th_max;
    int pixelnumax_th_max;
    int minhistnum_max;
    int pixelnumin_th_min;
    int pixelnumax_th_min;
    int minhistnum_min;

    int avepixelmin;
    int avepixelmax;

    int add_hist_th;
    int avepixel;
    int pixelnumin_th;
    int pixelnumax_th;
    int minhistnum;
} cut_cont_high_hist_params;

typedef struct cut_dark_hist_params {
    int dark_len;
    int darkhist_th;
    int hist_th_len;
    int cut_dark_len;
    int startadd;
    int cutdarkhist_th;
    int starti;
    int endi;
    int cuthistminth;
} cut_dark_hist_params;

typedef struct cut_hist_add_params {
    unsigned char histw_l[32];
    int addhist_th_l;
    int histmax_th_l;
}cut_hist_add_params;


// the tonemapping algorithm for SDR Image
void adaptive_tone_mapping_ace(contrast_proc_info *p_con_mem, contrast_proc_params *st_cp_params, int la,
    int global_alpha);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif