/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: header of hvs aef
 * Author: audio
 */

#ifndef __HVS_AEF_H__
#define __HVS_AEF_H__

#include "td_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AUDIO_HVS_COM_PAD_2 2               // padding array size
#define AUDIO_HVS_COM_PAD_3 3               // padding array size
#define AUDIO_HVS_COM_PAD_4 4               // padding array size
#define AUDIO_HVS_COM_PAD_5 5               // padding array size
#define AUDIO_HVS_COM_PAD_6 6               // padding array size
#define AUDIO_HVS_COM_PAD_7 7               // padding array size
#define AUDIO_HVS_COM_PAD_8 8               // padding array size
#define AUDIO_HVS_COM_PAD_10 10             // padding array size
#define AUDIO_HVS_COM_PAD_11 11             // padding array size
#define AUDIO_HVS_COM_PAD_12 12             // padding array size

/* HVS parameter structure */
typedef struct {
    /* ALG模块 (参数0-7): */
    td_s16 ntm_flag;                                                 /* [0, 1] */
    td_s16 qmf_delay;                                                /* [0, 2048] */
    td_s16 total_gain;                                               /* [-60, 60] */
    td_s16 reserved[AUDIO_HVS_COM_PAD_5];                            /* [0, 1] */

    /* CGN模块(参数8-11): */
    td_s16 cgn_module_flag;                                         /* [0, 1] */
    td_s16 vol_relate;                                              /* [0, 1] */
    td_s16 gain[AUDIO_HVS_COM_PAD_2];                               /* [0, 200] */

    /* LME模块(参数 12-26): */
    td_s16 lme_module_flag;                                         /* [0, 1] */
    td_s16 mce[AUDIO_HVS_COM_PAD_4];                                /* [0, 1000] */
    td_s16 lfe[AUDIO_HVS_COM_PAD_10];                               /* [0, 10000] */

    /* GEQ0模块(参数 27-127): */
    td_s16 geq0_module_flag;                                        /* [0, 1] */
    td_s16 geq0_pre_gain[AUDIO_HVS_COM_PAD_2];                      /* [0, 200] */
    td_s16 geq0_n[AUDIO_HVS_COM_PAD_2];                             /* [0, 12] */
    td_s16 geq0_type[AUDIO_HVS_COM_PAD_2][AUDIO_HVS_COM_PAD_12];    /* [0, 6] */
    td_s16 geq0_g[AUDIO_HVS_COM_PAD_2][AUDIO_HVS_COM_PAD_12];       /* [0, 200] */
    td_s16 geq0_q[AUDIO_HVS_COM_PAD_2][AUDIO_HVS_COM_PAD_12];       /* [100, 10000] */
    td_s16 geq0_fc[AUDIO_HVS_COM_PAD_2][AUDIO_HVS_COM_PAD_12];      /* [20, 20000] */

    /* GEQ1模块(参数 128-228): */
    td_s16 geq1_module_flag;                                        /* [0, 1] */
    td_s16 geq1_pre_gain[AUDIO_HVS_COM_PAD_2];                      /* [0, 200] */
    td_s16 geq1_n[AUDIO_HVS_COM_PAD_2];                             /* [0, 12] */
    td_s16 geq1_type[AUDIO_HVS_COM_PAD_2][AUDIO_HVS_COM_PAD_12];    /* [0, 6] */
    td_s16 geq1_g[AUDIO_HVS_COM_PAD_2][AUDIO_HVS_COM_PAD_12];       /* [0, 200] */
    td_s16 geq1_q[AUDIO_HVS_COM_PAD_2][AUDIO_HVS_COM_PAD_12];       /* [100, 10000] */
    td_s16 geq1_fc[AUDIO_HVS_COM_PAD_2][AUDIO_HVS_COM_PAD_12];      /* [20, 20000] */

    /* LSP模块(参数 229-301): */
    td_s16 lsp_module_flag;                                         /* [0, 1] */
    td_s16 sub_module_flag[AUDIO_HVS_COM_PAD_6];                    /* [0, 1] */
    td_s16 makeup_gain[AUDIO_HVS_COM_PAD_6];                        /* [0,200] */
    td_s16 attack[AUDIO_HVS_COM_PAD_6];                             /* [0,10000] */
    td_s16 release[AUDIO_HVS_COM_PAD_6];                            /* [0,10000] */
    td_s16 thr[AUDIO_HVS_COM_PAD_6];                                /* [-80,10] */
    td_s16 ratio[AUDIO_HVS_COM_PAD_6];                              /* [2,200] */
    td_s16 softknee[AUDIO_HVS_COM_PAD_6];                           /* [0,40] */
    td_s16 p_sm_coeff[AUDIO_HVS_COM_PAD_2];                         /* [-32768,32767] */
    td_s16 pmax[AUDIO_HVS_COM_PAD_2];                               /* [-32768,32767] */
    td_s16 x_u2x_params[AUDIO_HVS_COM_PAD_2][AUDIO_HVS_COM_PAD_11]; /* [-32768,32767] */
    td_s16 x_delay[AUDIO_HVS_COM_PAD_4];                            /* [0,1024] */
} audio_hvs_param_struct;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __HVS_AEF_H__ */
