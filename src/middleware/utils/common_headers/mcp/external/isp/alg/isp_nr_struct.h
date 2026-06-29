/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Function of isp_nr_struct.h
 * Author: AuthorNameMagicTag
 * Create: 2025-05-26
 */

#ifndef __ISP_NR_STRUCT_H__
#define __ISP_NR_STRUCT_H__

#include "mpi_mcp_dependency.h"
#include "drv_isp_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/************************************* nr *******************************************/
#define EXT_ISP_BAYERNR_LUT_LENGTH 33
#define EXT_ISP_BAYERNR_LUT_LENGTH1 32
#define EXT_ISP_BAYERNR_FBRTO_LUT_SIZE 64

typedef struct {
    td_u8 chroma_str[ISP_BAYER_CHN_NUM];  /* Strength of Chrmoa noise reduction for R/Gr/Gb/B channel */
    td_u8 fine_str;                       /* Strength of Luma noise reduction */
    td_u16 coring_wgt;                    /* Strength of reserving the random noise */
    td_u16 coarse_str[ISP_BAYER_CHN_NUM]; /* Coarse Strength of noise reduction */
} ext_isp_nr_manual_attr;

typedef struct {
    td_u8 wdr_frame_str[ISP_WDR_FRAME_MAX];    /* Strength of each frame in wdr mode */
    td_u8 fusion_frame_str[ISP_WDR_FRAME_MAX]; /* Strength of each frame in wdr mode */
} ext_isp_nr_wdr_attr_v1;

typedef struct {
    td_bool low_power_enable;    /* Nr Low Power Enable. */
    td_u8 bnr_lsc_max_gain;      /* Max gain for referring to lens shading */
    td_u16 bnr_lsc_cmp_strength; /* Compare strength for referring to lens shading */

    ext_isp_nr_auto_attr auto_attr;
    ext_isp_nr_manual_attr manual_attr;
    ext_isp_nr_wdr_attr_v1 wdr_attr;
} ext_isp_nr_param_v1;

typedef struct {
    td_u16 sfm0_coarse_strength[ISP_BAYER_CHN_NUM][ISP_AUTO_ISO_STRENGTH_NUM]; /* RW; Range:[0, 864];Format:10.0;
                                                sfm0 filter coarse strength. */
    td_u8 sfm0_detail_prot[ISP_AUTO_ISO_STRENGTH_NUM]; /* RW; Range:[0, 31];Format:5.0; sfm0 filter detail protect rate.
                                                        */
    td_u16 sfm1_strength[ISP_AUTO_ISO_STRENGTH_NUM];   /* RW; Range:[0, 1023];Format:16.0; sfm1 filter strength.
                                                  Not support for Hi3516CV610 */
    td_u8 sfm1_adp_strength[ISP_AUTO_ISO_STRENGTH_NUM]; /* RW; Range:[0, 16];Format:5.0; sfm1 filter adaptive strength.
                                                   Not support for Hi3516CV610 */
    td_u8 sfm6_strength[ISP_AUTO_ISO_STRENGTH_NUM];     /* RW; Range:[0, 64];Format:7.0; sfm6 hybrid filter strength.
                                                   Not support for Hi3516CV610 */
    td_u8 sfm7_strength[ISP_AUTO_ISO_STRENGTH_NUM];     /* RW; Range:[0, 64];Format:7.0; sfm7 hybrid filter strength.
                                                   Not support for Hi3516CV610 */
    td_u8 sth[ISP_AUTO_ISO_STRENGTH_NUM];               /* RW; Range:[0, 255];Format:8.0; hybrid filter threshold.
                                                   Not support for Hi3516CV610 */
    td_u8 tss[ISP_AUTO_ISO_STRENGTH_NUM];               /* RW; Range:[0, 128];Format:8.0; snr ratio in background.
                                                   Not support for Hi3519DV500 */
    td_u8 fine_strength[ISP_AUTO_ISO_STRENGTH_NUM];     /* RW; Range:[0, 128];Format:8.0; denoise rate. */
    td_u16 coring_wgt[ISP_AUTO_ISO_STRENGTH_NUM]; /* RW;Range:[0, 3200];Format:12.0;Strength of reserving the random
                                                     noise. */
    td_u8 coring_mot_ratio[ISP_AUTO_ISO_STRENGTH_NUM]; /* RW; Range:[0, 63];Format:6.0; coring on moving area.
                                                    Not support for Hi3516CV610 */
    td_u8 dst_flten[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8 imp_en[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8 flt_mode[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8 dst_gain_max[ISP_AUTO_ISO_STRENGTH_NUM]; /* RW;Range:[0, 255] */
    td_u8 dst_gain_slope[ISP_AUTO_ISO_STRENGTH_NUM]; /* RW;Range:[0, 255] */
    td_u8 noisesd_lut[EXT_ISP_BAYERNR_LUT_LENGTH1][ISP_AUTO_ISO_STRENGTH_NUM];
    /* RW; Range:[0, 255];Format:8.0; Not support for Hi3519DV500 */
} ext_isp_nr_snr_auto_attr;

typedef struct {
    td_u16 sfm0_coarse_strength[ISP_BAYER_CHN_NUM]; /* RW; Range:[0,864];Format:10.0; sfm0 coarse strength. */
    td_u8 sfm0_detail_prot;                         /* RW; Range:[0, 31];Format:5.0; sfm0 filter detail protect rate. */
    td_u16 sfm1_strength;                           /* RW; Range:[0, 1023];Format:16.0; sfm1 filter strength.
                                                       Not support for Hi3516CV610  */
    td_u8 sfm1_adp_strength;                        /* RW; Range:[0, 16];Format:5.0; sfm1 filter adaptive strength.
                                                       Not support for Hi3516CV610  */
    td_u8 sfm6_strength;                            /* RW; Range:[0, 64];Format:7.0; sfm6 hybrid filter strength.
                                                       Not support for Hi3516CV610  */
    td_u8 sfm7_strength;                            /* RW; Range:[0, 64];Format:7.0; sfm7 hybrid filter strength.
                                                       Not support for Hi3516CV610 */
    td_u8 sth;                                      /* RW; Range:[0, 255];Format:8.0; hybrid filter threshold.
                                                       Not support for Hi3516CV610 */
    td_u8 tss;                                      /* RW; Range:[0, 128];Format:8.0; snr ratio in background.
                                                       Not support for Hi3519DV500 */
    td_u8 fine_strength;                            /* RW; Range:[0, 128];Format:8.0; denoise rate. */
    td_u16 coring_wgt;      /* RW;Range:[0, 3200];Format:12.0;Strength of reserving the random noise. */
    td_u8 coring_mot_ratio; /* RW; Range:[0, 63];Format:6.0; coring on moving area.
                               Not support for Hi3516CV610 */
    td_u8 dst_flten; /* RW;Range:[0, 1] */
    td_u8 imp_en; /* RW;Range:[0, 1] */
    td_u8 flt_mode; /* RW;Range:[0, 2] */
    td_u8 dst_gain_max; /* RW;Range:[0, 255] */
    td_u8 dst_gain_slope; /* RW;Range:[0, 255] */
    td_u8 noisesd_lut[EXT_ISP_BAYERNR_LUT_LENGTH1]; /* RW; Range:[0, 255];Format:8.0; Not support for Hi3519DV500 */
} ext_isp_nr_snr_manual_attr;

typedef struct {
    ext_isp_nr_snr_auto_attr snr_auto;
    ext_isp_nr_snr_manual_attr snr_manual;
} ext_isp_nr_snr_attr;

typedef struct {
    ext_isp_nr_snr_attr snr_cfg;
} ext_isp_nr_param_v2;

typedef struct {
    td_bool enable;        /* Nr Enable */
    td_bool nr_lsc_enable; /* TD_TRUE: Noise reduction refers to lens shading;
                              TD_FALSE: Noise reduction not refers to lens shading; */
    td_u8 nr_lsc_ratio;    /* Ratio of referring to lens shading */
    ext_isp_op_type op_type;
    td_u16 coring_ratio[EXT_ISP_BAYERNR_LUT_LENGTH]; /* Strength of reserving the random noise according to luma */

    union {
        ext_isp_nr_param_v1 nr_v1; /* chip:850 */
        ext_isp_nr_param_v2 nr_v2; /* chip:1A22 */
    };
} ext_isp_nr_attr;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
