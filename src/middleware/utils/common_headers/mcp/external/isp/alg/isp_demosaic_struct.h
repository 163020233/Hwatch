/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Function of isp_demosaic_struct.h
 * Author: AuthorNameMagicTag
 * Create: 2025-05-26
 */

#ifndef __ISP_DEMOSAIC_STRUCT_H__
#define __ISP_DEMOSAIC_STRUCT_H__

#include "mpi_mcp_dependency.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/************************************* demosaic *******************************************/
/* DM */
typedef struct {
    td_u8   nddm_strength;            /* RW; Range:[0x0, 0xFF]; Format:4.4; Non-direction strength */
    td_u8   nddm_mf_detail_strength; /* Format:3.4; Non-direction medium frequent detail enhance  strength */

    union {
        /*
        * AUTO:OT_CHIP: 850;
        * RW; Range:[0x0, 0x10]; Format:2.2; Non-direction high frequent detail enhance strength
        * Not support for 1a22
        */
        td_u8 nddm_hf_detail_strength;
        /*
        * AUTO:OT_CHIP: 1a22;
        * RW; Range:[0x0, 0x10]; Format:5.0; high frequent detail enhance strength
        * Not support for 850
        */
        td_u8 hf_detail_strength;
    };

    td_u8   detail_smooth_range;    /* Format:4.0; Detail smooth range */
    td_u16  detail_smooth_str;     /* RW;Range:[0x0, 0x100]; Format:9.0; Strength of detail smooth */
    td_u8   color_noise_f_threshold; /* RW;Range:[0x0,0xff]; Format:8.0; Threshold of color noise cancel */
    td_u8   color_noise_f_strength; /* RW;Range:[0x0, 0x8]; Format:4.0; Strength of color noise cancel */
    td_u8   color_noise_y_threshold; /* Format:8.0; Range of color denoise luma, related to luminance and saturation */
    td_u8   color_noise_y_strength; /* Format:8.0;Format:8.0; Strength of color denoise luma */

    //1A22 Add
    td_u8   nddm_dec_gain_sft; /* RW;Range:[0x0, 0xF] */
    td_u8   vhdm_ehc_f0_gain_sft; /* RW;Range:[0x0, 0xF] */
    td_u8   vhdm_dec_f3_gain_sft; /* RW;Range:[0x0, 0xF] */

    /* RW;Range:[0x0, 0x1F] */
    td_u8   nddm_dec_gain0;
    td_u8   nddm_dec_gain1;
    td_u8   nddm_dec_gain2;
    td_u8   nddm_dec_gain3;
    td_u8   nddm_dec_gain4;
    td_u8   nddm_dec_gain5;
    td_u8   nddm_dec_gain6;
    td_u8   nddm_dec_gain7;
    td_u8   nddm_dec_gain8;
    td_u8   nddm_dec_gain9;
    td_u8   nddm_dec_gain10;
    td_u8   nddm_dec_gain11;
    td_u8   nddm_dec_gain12;
    td_u8   nddm_dec_gain13;
    td_u8   nddm_dec_gain14;
    td_u8   nddm_dec_gain15;

   /* RW;Range:[0x0, 0x1F] */
    td_u8   vhdm_ehc_f0_gain0;
    td_u8   vhdm_ehc_f0_gain1;
    td_u8   vhdm_ehc_f0_gain2;
    td_u8   vhdm_ehc_f0_gain3;
    td_u8   vhdm_ehc_f0_gain4;
    td_u8   vhdm_ehc_f0_gain5;
    td_u8   vhdm_ehc_f0_gain6;
    td_u8   vhdm_ehc_f0_gain7;
    td_u8   vhdm_ehc_f0_gain8;
    td_u8   vhdm_ehc_f0_gain9;
    td_u8   vhdm_ehc_f0_gain10;
    td_u8   vhdm_ehc_f0_gain11;
    td_u8   vhdm_ehc_f0_gain12;
    td_u8   vhdm_ehc_f0_gain13;
    td_u8   vhdm_ehc_f0_gain14;
    td_u8   vhdm_ehc_f0_gain15;

   /* RW;Range:[0x0, 0x1F] */
    td_u8   vhdm_dec_f3_gain0;
    td_u8   vhdm_dec_f3_gain1;
    td_u8   vhdm_dec_f3_gain2;
    td_u8   vhdm_dec_f3_gain3;
    td_u8   vhdm_dec_f3_gain4;
    td_u8   vhdm_dec_f3_gain5;
    td_u8   vhdm_dec_f3_gain6;
    td_u8   vhdm_dec_f3_gain7;
    td_u8   vhdm_dec_f3_gain8;
    td_u8   vhdm_dec_f3_gain9;
    td_u8   vhdm_dec_f3_gain10;
    td_u8   vhdm_dec_f3_gain11;
    td_u8   vhdm_dec_f3_gain12;
    td_u8   vhdm_dec_f3_gain13;
    td_u8   vhdm_dec_f3_gain14;
    td_u8   vhdm_dec_f3_gain15;
    //Add End
} ext_isp_demosaic_manual_attr;

typedef struct {
    td_u8   nddm_strength[ISP_AUTO_ISO_STRENGTH_NUM];     /* Non-direction strength */
    td_u8   nddm_mf_detail_strength[ISP_AUTO_ISO_STRENGTH_NUM]; /* Format:3.4; Non-direction medium frequent detail
                                                                   enhance strength */
    union {
        /*
        * AUTO:OT_CHIP: 850;
        * RW; Range:[0x0, 0x10]; Format:2.2; Non-direction high frequent detail enhance strength
        * Not support for 1A22
        */
        td_u8 nddm_hf_detail_strength[ISP_AUTO_ISO_STRENGTH_NUM];
        /*
        * AUTO:OT_CHIP: 1A22;
        * RW; Range: [0x0, 0x10]; Format:5.0; high frequent detail enhance strength
        * Not support for 850
        */
        td_u8 hf_detail_strength[ISP_AUTO_ISO_STRENGTH_NUM];
    };

    td_u8   detail_smooth_range[ISP_AUTO_ISO_STRENGTH_NUM];    /* Detail smooth range */
    td_u16  detail_smooth_str[ISP_AUTO_ISO_STRENGTH_NUM];     /* Strength of detail smooth */
    td_u8   color_noise_f_threshold[ISP_AUTO_ISO_STRENGTH_NUM]; /* hreshold of color noise cancel */
    td_u8   color_noise_f_strength[ISP_AUTO_ISO_STRENGTH_NUM]; /* Strength of color noise cancel,  */
    td_u8   color_noise_y_threshold[ISP_AUTO_ISO_STRENGTH_NUM];  /* denoise luma, related to luminance and saturation */
    td_u8   color_noise_y_strength[ISP_AUTO_ISO_STRENGTH_NUM];  /* Strength of color denoise luma */
    //1A22 Add
    td_u8   nddm_dec_gain_sft[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   vhdm_ehc_f0_gain_sft[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   vhdm_dec_f3_gain_sft[ISP_AUTO_ISO_STRENGTH_NUM];

    td_u8   nddm_dec_gain0[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   nddm_dec_gain1[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   nddm_dec_gain2[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   nddm_dec_gain3[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   nddm_dec_gain4[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   nddm_dec_gain5[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   nddm_dec_gain6[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   nddm_dec_gain7[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   nddm_dec_gain8[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   nddm_dec_gain9[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   nddm_dec_gain10[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   nddm_dec_gain11[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   nddm_dec_gain12[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   nddm_dec_gain13[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   nddm_dec_gain14[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   nddm_dec_gain15[ISP_AUTO_ISO_STRENGTH_NUM];

    td_u8   vhdm_ehc_f0_gain0[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   vhdm_ehc_f0_gain1[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   vhdm_ehc_f0_gain2[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   vhdm_ehc_f0_gain3[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   vhdm_ehc_f0_gain4[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   vhdm_ehc_f0_gain5[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   vhdm_ehc_f0_gain6[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   vhdm_ehc_f0_gain7[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   vhdm_ehc_f0_gain8[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   vhdm_ehc_f0_gain9[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   vhdm_ehc_f0_gain10[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   vhdm_ehc_f0_gain11[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   vhdm_ehc_f0_gain12[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   vhdm_ehc_f0_gain13[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   vhdm_ehc_f0_gain14[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   vhdm_ehc_f0_gain15[ISP_AUTO_ISO_STRENGTH_NUM];

    td_u8   vhdm_dec_f3_gain0[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   vhdm_dec_f3_gain1[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   vhdm_dec_f3_gain2[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   vhdm_dec_f3_gain3[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   vhdm_dec_f3_gain4[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   vhdm_dec_f3_gain5[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   vhdm_dec_f3_gain6[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   vhdm_dec_f3_gain7[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   vhdm_dec_f3_gain8[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   vhdm_dec_f3_gain9[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   vhdm_dec_f3_gain10[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   vhdm_dec_f3_gain11[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   vhdm_dec_f3_gain12[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   vhdm_dec_f3_gain13[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   vhdm_dec_f3_gain14[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   vhdm_dec_f3_gain15[ISP_AUTO_ISO_STRENGTH_NUM];
    //Add End
} ext_isp_demosaic_auto_attr;

typedef struct {
    td_bool enable;        /* RW; Range:[0, 1]; Format:1.0;Enable/Disable demosaic module */
    ext_isp_op_type op_type;
    ext_isp_demosaic_manual_attr manual_attr;
    ext_isp_demosaic_auto_attr   auto_attr;
} ext_isp_demosaic_attr;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
