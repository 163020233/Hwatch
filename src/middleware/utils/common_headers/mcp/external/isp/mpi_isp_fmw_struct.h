/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: Function of mpi_isp_fmw_struct.h
 * Author: AuthorNameMagicTag
 * Create: 2023-01-13
 */

#ifndef __MPI_ISP_FMW_STRUCT_H__
#define __MPI_ISP_FMW_STRUCT_H__

#include "isp_wdr_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

typedef struct {
    td_s32 sensor_id;
} ext_isp_bind_attr;

typedef enum {
    ISP_FMW_STATE_RUN = 0,
    ISP_FMW_STATE_FREEZE,
    ISP_FMW_STATE_BUTT
} ext_isp_fmw_state;

typedef union {
    td_u64 key;
    struct {
        td_u64 bit_bypass_isp_d_gain : 1;        /* RW;[0] */
        td_u64 bit_bypass_anti_false_color : 1;  /* RW;[1] */
        td_u64 bit_bypass_crosstalk_removal : 1; /* RW;[2] */
        td_u64 bit_bypass_dpc : 1;               /* RW;[3] */
        td_u64 bit_bypass_nr : 1;                /* RW;[4] */
        td_u64 bit_bypass_dehaze : 1;            /* RW;[5] */
        td_u64 bit_bypass_wb_gain : 1;           /* RW;[6] */
        td_u64 bit_bypass_mesh_shading : 1;      /* RW;[7] */
        td_u64 bit_bypass_drc : 1;               /* RW;[8] */
        td_u64 bit_bypass_demosaic : 1;          /* RW;[9] */
        td_u64 bit_bypass_color_matrix : 1;      /* RW;[10] */
        td_u64 bit_bypass_gamma : 1;             /* RW;[11] */
        td_u64 bit_bypass_fswdr : 1;             /* RW;[12] */
        td_u64 bit_bypass_ca : 1;                /* RW;[13] */
        td_u64 bit_bypass_csc : 1;               /* RW;[14] */
        td_u64 bit_bypass_radial_crop : 1;       /* RW;[15] */
        td_u64 bit_bypass_sharpen : 1;           /* RW;[16] */
        td_u64 bit_bypass_cac : 1;               /* RW;[17] */
        td_u64 bit_bypass_global_cac : 1;        /* RW;[18] */
        td_u64 bit2_chn_select : 2;              /* RW;[19:20] */
        td_u64 bit_bypass_ldci : 1;              /* RW;[21] */
        td_u64 bit_bypass_pregamma : 1;          /* RW;[22] */
        td_u64 bit_bypass_radial_shading : 1;    /* RW;[23] */
        td_u64 bit_bypass_ae_stat_fe : 1;        /* RW;[24] */
        td_u64 bit_bypass_ae_stat_be : 1;        /* RW;[25] */
        td_u64 bit_bypass_mg_stat : 1;           /* RW;[26] */
        td_u64 bit_bypass_de : 1;                /* RW;[27] */
        td_u64 bit_bypass_af_stat_fe : 1;        /* RW;[28] */
        td_u64 bit_bypass_af_stat_be : 1;        /* RW;[29] */
        td_u64 bit_bypass_awb_stat : 1;          /* RW;[30]; */
        td_u64 bit_bypass_clut : 1;              /* RW;[31] */
        td_u64 bit_bypass_hlc : 1;               /* RW;[32] */
        td_u64 bit_bypass_edge_mark : 1;         /* RW;[33] */
        td_u64 bit_bypass_rgbir : 1;             /* RW;[34] */
        td_u64  bit_bypass_gdci : 1;   /* RW;[35] */
        td_u64  bit_rsv29 : 28;  /* H; [36:63] */
    };
} ext_isp_module_ctrl;

typedef struct {
    td_u16 texture_strength[ISP_SHARPEN_GAIN_NUM]; /* Undirectional sharpen strength for texture and detail enhancement
                                                    */
    td_u16 edge_strength[ISP_SHARPEN_GAIN_NUM];    /* Directional sharpen strength for edge enhancement */
    td_u16 texture_freq;      /* Texture frequency adjustment. Texture and detail will be finer when it increase */
    td_u16 edge_freq;         /* Edge frequency adjustment. Edge will be narrower and thiner when it increase */
    td_u8 over_shoot;         /* u8OvershootAmt */
    td_u8 under_shoot;        /* u8UndershootAmt */
    td_u8 shoot_sup_strength; /* overshoot and undershoot suppression strength,
                             the amplitude and width of shoot will be decrease when shootSupSt increase */

    td_u8 nr_lsc_ratio;                     /* Strength of reserving the random noise according to lumat */
    td_u16 coarse_str[ISP_BAYER_CHN_NUM];   /* Coarse Strength of noise reduction */
    td_u8 wdr_frame_str[ISP_WDR_FRAME_MAX]; /* Coarse strength of each frame in wdr mode */
    td_u8 chroma_str[ISP_BAYER_CHN_NUM];    /* Strength of Chrmoa noise reduction for R/Gr/Gb/B channel */
    td_u8 fine_str;                         /* Strength of Luma noise reduction */
    td_u16 coring_wgt;                      /* Strength of reserving the random noise */

    td_u16 dehaze_strength_actual;                      /* actual dehze strength */
    td_u16 drc_strength_actual;                         /* Strength of dynamic range compression.
                                                           Higher values lead to higher differential gain between
                                                           hadows and highlights. */
    td_u32 wdr_exp_ratio_actual[ISP_WDR_FRAME_MAX - 1]; /* When exp_ratio_type is OP_TYPE_AUTO, exp_ratio is invalid.
                                                           When exp_ratio_type is OP_TYPE_MANUAL, exp_ratio is
                                                           quotient of long exposure time / short exposure time. */
    td_bool wdr_switch_finish;           /* RW; Range:[0, 1];Format:1.0;TD_TRUE: WDR switch is finished */
    td_u8 res_switch_finish;           /* RW; Range:[0, 1];Format:1.0;TD_TRUE: Resolution switch is finished */
    td_u16 bl_actual[ISP_BAYER_CHN_NUM]; /* Actual Black level values that correspond to the black levels of
                                            the R,Gr, Gb, and B components respectively. */
} ext_isp_inner_state_info;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
