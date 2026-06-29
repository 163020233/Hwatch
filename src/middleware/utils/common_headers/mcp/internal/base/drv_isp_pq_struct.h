/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: pq struct define
 * Author: AuthorNameMagicTag
 * Create: 2025-04-17
 */

#ifndef __DRV_PQ_STRUCT_H__
#define __DRV_PQ_STRUCT_H__

#include "drv_isp_video_ext.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
/************************************* 3dnr *******************************************/
typedef struct {
    /* ies0~4 ; range: [0, 255]; the gains of edge and texture enhancement. 0~3 for different frequency response. */
    td_u8 ies0, ies1, ies2, ies3;
    td_u16 iedz : 10, reserved : 6; /* iedz; range: [0, 999]; the threshold to control the generated artifacts. */
} ext_pq_v500_iey;

typedef struct {
    /* spn6; range: [0,   5];  the selection of filters to be mixed for NO.6 filter. */
    /* sfr ; range: [0,  31];  the relative NR strength in the s_fi and s_fk filter. */
    /* sbn6; range: [0,   5];  the selection of filters to be mixed for NO.6 filter. */
    /* pbr6; range: [0,  16];  the mix ratio between spn6 and sbn6. */
    td_u8 spn6 : 3, sfr : 5;
    td_u8 sbn6 : 3, pbr6 : 5;

    /* j_mode;      range: [0,   4]; the selection modes for the blending of spatial filters */
    /* srt0, srt1; range: [0,  16]; the blending ratio of different filters. (used in serial filtering mode (SFM).) */
    /* de_idx;      range: [3,   6]; the selection number of filters that textures and details will be added to. */
    td_u16 srt0 : 5, srt1 : 5, j_mode : 3, de_idx : 3;
    /* de_rate;  range: [0, 255]; the enhancement strength for the SFM (when de_rate > 0, the SFM will be activated) */
    /* sfr6;     range: [0,  31]; the relative NR strength for NO.6 filter. (effective when j_mode = 4) */
    td_u8 de_rate, sfr6[3];

    /* sfs1, sft1, sbr1; range: [0, 255];  the NR strength parameters for NO.1 filter. */
    /* sfs2, sft2, sbr2; range: [0, 255];  the NR strength parameters for NO.2 filter. */
    /* sfs4, sft4, sbr4; range: [0, 255];  the NR strength parameters for NO.3 and NO.4 filters. */
    td_u8 sfs1, sft1, sbr1;
    td_u8 sfs2, sft2, sbr2;
    td_u8 sfs4, sft4, sbr4;

    /* sth1~3; range: [0, 511]; the thresholds for protection of edges from blurring */
    /* nr_y_en ; range: [0,   1]; the NR switches */
    /* sfn0~3; range: [0,   6]; filter selection for different image areas based on sth1~3. */
    /* bwsf4 ; range: [0,   1]; the NR window size for the NO.3 and NO.4 filters.  */
    /* k_mode ; range: [0,   3]; the denoise mode based on image brightness. */
    /* trith ; range: [0,   1]; the switch to choose 3 sth threshold or 2 STH threshold */
    td_u16 sth1 : 9, sfn1 : 3, sfn0 : 3, nr_y_en : 1;
    td_u16 sth2 : 9, sfn2 : 3, bwsf4 : 1, k_mode : 3;
    td_u16 sth3 : 9, sfn3 : 3, tri_th : 1, reserved : 3;

    /* sbs_k[32], sds_k[32]; range [0, 8191];  spatial NR strength based on brightness. */
    td_u16 sbs_k[32], sds_k[32];
} ext_pq_v500_sfy;

typedef struct {
    /* madz0, madz1;     range: [0, 511]; the blending ratio between mai2 and mai1 based on image statistics. */
    /* mai00~02,mai10~12 range: [0,   3]; the three blending results between spatial and temporal filtering. */
    /* mabr0, mabr1;     range: [0, 255]; the blending ratio between mai2 and mai1 based on brightness.  */
    /* bi_path;          range: [0,   1]; the switch for single path or dual path. 0: single path; 1: dual path. */
    td_u16 madz0 : 9, mai00 : 2, mai01 : 2, mai02 : 2, bi_path : 1;
    td_u16 madz1 : 9, mai10 : 2, mai11 : 2, mai12 : 2, reserved : 1;
    td_u8 mabr0, mabr1;

    /* math0,math1;   range: [0, 999]; the theshold for motion detection. */
    /* mate0,mate1;   range: [0,   8]; the motion index for smooth image area. */
    /* matw;   range: [0,   3]; the motion index for prevention of motion ghost. */
    td_u16 math0 : 10, mate0 : 4, matw : 2;
    td_u16 math1 : 10, mate1 : 4, reserved_1 : 2;

    /* masw;   range: [0,  15]; the motion index for low-frequency noises. */
    /* mabw0,mabw1;   range: [0,   9]; the window size for motion detection. */
    td_u8 masw : 4, reserved_2 : 4;
    td_u8 mabw0 : 4, mabw1 : 4;
} ext_pq_v500_mdy;

typedef struct {
    /* tfs0,tfs1;    range: [0,  15]; the NR strength for temporal filtering. */
    /* tdz0,tdz1;    range: [0, 999]; protection of the weak texture area from temporal filtering.  */
    /* tdx0,tdx1;    range: [0,   3]; not for tuning. */
    td_u16 tfs0 : 4, tdz0 : 10, tdx0 : 2;
    td_u16 tfs1 : 4, tdz1 : 10, tdx1 : 2;

    /* sdz0,sdz1;    range: [0, 999]; the threshold of NR control for result mai1. */
    /* str0,str1;    range: [0,  31]; the strength of NR control for result mai1.  */
    /* dz_mode0, dz_mode1; range: [0,   1]; the selection mode for tdz0 and tdz1, respectively.  */
    td_u16 sdz0 : 10, str0 : 5, dz_mode0 : 1;
    td_u16 sdz1 : 10, str1 : 5, dz_mode1 : 1;

    /* tfr0,tfr1;    range: [0,  31]; the temoproal NR strength control for background (static) area. */
    /* tss0,tss1;    range: [0,  15]; the ratio for blending spatial NR with the temproal NR results. */
    /* tsi0,tsi1;    range: [0,   1]; the selection of blending filter for TSS. */
    td_u8 tfr0[6], tss0 : 4, tsi0 : 4;
    td_u8 tfr1[6], tss1 : 4, tsi1 : 4;

    /* t_edge;       range: [0,   3]; NR strength control mode for the updating background. */
    /* rfi;          range: [0,   4]; reference mode. (used in when ext_pq_NR_MOTION_MODE_COMPENSATE are selected). */
    /* ref;          range: [0,   1]; the switch for temproal filtering. */
    td_u8 rfi : 3, t_edge : 2, ref : 1, reserved : 2;
} ext_pq_v500_tfy;

typedef struct {
    /* adv_math;        range: [0,   1]; the switch for advanced motion dection.  */
    /* rfui;            range: [0,   4]; the modes for updating reference for n_ry leve 2,
        (used in when ext_pq_NR_MOTION_MODE_COMPENSATE are selected). */
    td_u16 adv_math : 1, rfdz : 9, reserved : 6;
    /* rfdz;            rnage: [0, 511]; the threshold for rfi0 and rfi1 mode 3 and 4. */
    /* rfslp;           rnage: [0,  31]; the strength for rfi0 and rfi1 mode 3 and 4. */
    td_u8 rfui : 3, rfslp : 5;
} ext_pq_v500_rfs;

typedef struct {
    ext_pq_v500_iey iey;
    ext_pq_v500_sfy sfy;
    td_u8 nr_c_en : 1, reserved : 7;
} ext_pq_v500_nr_c;

typedef struct {
    /* sfc;    range: [0, 255]; spatial NR strength. */
    td_u8 sfc;
    /* tfc;    range: [0,  63]; temporal NR strength relative to spatial NR. */
    /* ctfs;   range: [0,  15]; absolute temporal NR strength. */
    td_u16 ctfs : 4, tfc : 6, reserved : 6;
    /* mode;    range: [0,  1]; The switch for new chroma denoise mode. */
    td_u8 mode : 1, reserved_1 : 7;
    /* presfc;    range: [0,  32]; The strength for chroma pre spatial filter. */
    td_u8 presfc : 6, reserved_2 : 2;
} ext_pq_v500_p_nr_c;

typedef struct {
    td_u8 ies0, ies1, ies2, ies3;   /* ies0~4 ; range: [0, 255]; the gains of edge and texture enhancement.
                                     * 0~3 for different frequency response. */
    td_u16 iedz : 10, reserved : 6; /* iedz   ; range: [0, 999]; the threshold to control the generated artifacts. */
} ext_viu_iey;

typedef struct {
    td_u8 spn6 : 3, sfr : 5;  /* SPN6; range: [0, 5];  the selection of filters to be mixed for NO.6 filter. */
                              /* SFR ; range: [0, 31];  the relative NR strength in the s_fi and s_fk filter. */
    td_u8 sbn6 : 3, pbr6 : 5; /* SBN6; range: [0, 5];  the selection of filters to be mixed for NO.6 filter. */
                              /* PBR6; range: [0, 16];  the mix ratio between SPN6 and SBN6. */
    td_u16 srt0 : 5, srt1 : 5, j_mode : 3, de_idx : 3; /* JMODE;      range: [0, 4]; the selection modes
                                                      for the blending of spatial filters */
                                                       /* srt0, srt1; range: [0, 16]; the blending ratio
                                                        of different filters. (used in serial filtering mode (SFM).) */
                                                       /* de_idx;      range: [3, 6]; the selection number of
                                                        filters that textures and details will be added to. */
    td_u8 de_rate, sfr6[3];                            /* de_rate;     range: [0, 255]; the enhancement strength
                                                       for the SFM (when de_rate > 0, the SFM will be activated) */
                                                       /* SFR6;       range: [0, 31]; the relative NR strength
                                                        for NO.6 filter. (effective when JMODE = 4) */
    td_u8 sfs1, sft1, sbr1;                            /* SFS1, SFT1, SBR1; range: [0, 255];  the NR strength
                                                        parameters for NO.1 filter. */
    td_u8 sfs2, sft2, sbr2;                            /* SFS2, SFT2, SBR2; range: [0, 255];  the NR strength
                                                        parameters for NO.2 filter. */
    td_u8 sfs4, sft4, sbr4;                            /* SFS4, SFT4, SBR4; range: [0, 255];  the NR strength
                                                        parameters for NO.3 and NO.4 filters. */

    td_u16 sth1 : 9, sfn1 : 3, nr_y_en : 1, sfn0 : 3;      /* STH1~3; range: [0, 511]; the thresholds for protection
                                                          of edges from blurring */
                                                           /* n_ry_en ; range: [0, 1]; the NR switches */
    td_u16 sth2 : 9, sfn2 : 3, bw_sf4 : 1, k_mode : 3;     /* SFN0~3; range: [0, 6]; filter selection for different
                                                          image areas based on STH1~3. */
                                                           /* BWSF4 ; range: [0, 1]; the NR window size for the
                                                            NO.3 and NO.4 filters.  */
    td_u16 sth3 : 9, sfn3 : 3, tri_th : 1, reserved_0 : 3; /* k_mode ; range: [0, 3]; the denoise mode based
                                                      on image brightness. */
                                                           /* trith ; range: [0, 1]; the switch to choose 3 STH
                                                            threshold or 2 STH threshold */
} ext_viu_sfy;

typedef struct {
    union {
        ext_pq_v500_iey iey[3];
        ext_viu_iey viu_iey;
    };
    union {
        ext_pq_v500_sfy sfy[3];
        ext_viu_sfy viu_sfy;
    };
    ext_pq_v500_mdy mdy[2];
    ext_pq_v500_rfs rfs;
    ext_pq_v500_tfy tfy[2];
    ext_pq_v500_p_nr_c p_nr_c;
    ext_pq_v500_nr_c nr_c;
} ext_pq_nrx_v2;

typedef struct {
    ext_pq_nrx_v2 nrx_param;
} ext_pq_nrx_param_manual_v2;

typedef struct {
    td_u32 param_num;
    td_u32 iso[EXT_ISP_NRX_AUTO_PARAM_MAX];
    ext_pq_nrx_v2 nrx_param[EXT_ISP_NRX_AUTO_PARAM_MAX];
} ext_pq_nrx_param_auto_v2;

typedef struct {
    ext_isp_operation_mode opt_mode;       /* RW;adaptive NR */
    ext_pq_nrx_param_manual_v2 nrx_manual; /* RW;NRX V2 param for manual video */
    ext_pq_nrx_param_auto_v2 nrx_auto;     /* RW;NRX V2 param for auto video */
} ext_pq_nrx_param_v2;

typedef struct {
    td_u16 coarse_g1;   /* RW; Range: [0, 1023];   The sharpen gain of coarse region for No.1 Sharpen filter. */
    td_u16 fine_g1;     /* RW; Range: [0, 1023];   The sharpen gain of fine region for No.1 Sharpen filter. */
    td_u16 coarse_g2;   /* RW; Range: [0, 1023];   The sharpen gain of coarse region for No.2 Sharpen filter. */
    td_u16 fine_g2;     /* RW; Range: [0, 1023];   The sharpen gain of fine region for No.2 Sharpen filter. */
    td_u16 coarse_g3;   /* RW; Range: [0, 1023];   The sharpen gain of coarse region for No.3 Sharpen filter. */
    td_u16 fine_g3;     /* RW; Range: [0, 1023];   The sharpen gain of fine region for No.3 Sharpen filter. */
    td_u16 coarse_g4;   /* RW; Range: [0, 1023];   The sharpen gain of coarse region for No.4 Sharpen filter. */
    td_u16 fine_g4;     /* RW; Range: [0, 1023];   The sharpen gain of fine region for No.4 Sharpen filter. */
    td_u16 o_sht_b : 8; /* RW; Range: [0, 255];      The gains to control the overshoot for background. */
    td_u16 u_sht_b : 8; /* RW; Range: [0, 255];       The gains to control the undershoot for background. */
    td_u16 o_sht_f : 8; /* RW; Range: [0, 255];      The gains to control the overshoot for foreground. */
    td_u16 u_sht_f : 8; /* RW; Range: [0, 255];      The gains to control the undershoot for foreground. */
    td_u16 sf_bld[17];  /* RW; Range: [0, 63];       The  blending strength of selection filters for No. 7 and No. 8
                           filter. */
    td_u16 shp_ctl_mean[17]; /* RW; Range: [0, 63];    The sharpen gain of coarse region based on  mean difference. */
    td_u16 shp_ctl_dir[17];  /* RW; Range: [0, 63];     The sharpen gain of coarse region based on edge variance. */
    td_u16 sfn7_0;           /* RW; Range: [0,   4]; Filter selection for No. 7 filter. */
    td_u16 sfn7_1;           /* RW; Range: [0,   4]; Filter selection for No. 7 filter. */
    td_u16 sfn8_0;           /* RW; Range: [0,   4]; Filter selection for No. 8 filter. */
    td_u16 sfn8_1;           /* RW; Range: [0,   4]; Filter selection for No. 8 filter. */
    td_u16 sth1_0;           /* RW; Range: [0, 511]; The thresholds for protection of edges from blurring */
    td_u16 sth2_0;           /* RW; Range: [0, 511]; The thresholds for protection of edges from blurring */
    td_u16 sth3_0;           /* RW; Range: [0, 511]; The thresholds for protection of edges from blurring */
    td_u16 sth1_1;           /* RW; Range: [0, 511]; The thresholds for protection of edges from blurring */
    td_u16 sth2_1;           /* RW; Range: [0, 511]; The thresholds for protection of edges from blurring */
    td_u16 sth3_1;           /* RW; Range: [0, 511]; The thresholds for protection of edges from blurring */
    td_u16 sfn0_0 : 4;       /* RW; Range: [0,   8]; Filter selection for different image areas based on sth1_0~3. */
    td_u16 sfn1_0 : 4;       /* RW; Range: [0,   8]; Filter selection for different image areas based on sth1_0~3. */
    td_u16 sfn2_0 : 4;       /* RW; Range: [0,   8]; Filter selection for different image areas based on sth1_0~3. */
    td_u16 sfn3_0 : 4;       /* RW; Range: [0,   8]; Filter selection for different image areas based on sth1_0~3. */
    td_u16 sfn0_1 : 4;       /* RW; Range: [0,   8]; Filter selection for different image areas based on sth1_0~3. */
    td_u16 sfn1_1 : 4;       /* RW; Range: [0,   8]; Filter selection for different image areas based on sth1_0~3. */
    td_u16 sfn2_1 : 4;       /* RW; Range: [0,   8]; Filter selection for different image areas based on sth1_0~3. */
    td_u16 sfn3_1 : 4;       /* RW; Range: [0,   8]; Filter selection for different image areas based on sth1_0~3. */
} ext_pq_nr_v3_pshrp;        /*   nry4  */

typedef struct {
    td_u8 sfs1;     /* RW; Range: [0, 255];  The NR strength parameters for NO.1 filter. */
    td_u8 sbr1;     /* RW; Range: [0, 255];  The NR strength parameters for NO.1 filter. */
    td_u8 sfs2;     /* RW; Range: [0, 255];  The NR strength parameters for NO.2 filter. */
    td_u8 sft2;     /* RW; Range: [0, 255];  The NR strength parameters for NO.2 filter. */
    td_u8 sbr2;     /* RW; Range: [0, 255];  The NR strength parameters for NO.2 filter. */
    td_u8 strf3;    /* RW; Range: [0, 31];  The NR strength parameters of foreground for NO.3 filter. */
    td_u8 strb3;    /* RW; Range: [0, 31];  The NR strength parameters of background for NO.3 filter. */
    td_u8 sfs4;     /* RW; Range: [0, 255];  The NR strength parameters for NO.4 filter. */
    td_u8 sft4;     /* RW; Range: [0, 255];  The NR strength parameters for NO.4 filter. */
    td_u8 sbr4;     /* RW; Range: [0, 255];  The NR strength parameters for NO.4 filter. */
    td_u8 strf4;    /* RW; Range: [0, 31];  The NR strength parameters of foreground for NO.4 filter. */
    td_u8 strb4;    /* RW; Range: [0, 31];  The NR strength parameters of background for NO.4 filters. */
    td_u8 sfs5;     /* RW; Range: [0, 255];  The NR strength parameter for NO.5 filters. */
    td_u16 bld6;    /* RW; Range: [0,   16]; The blending strength  for No. 6 filter. */
    td_u16 sth1_0;  /* RW; Range: [0, 511]; The thresholds for protection of edges from blurring */
    td_u16 sth2_0;  /* RW; Range: [0, 511]; The thresholds for protection of edges from blurring */
    td_u16 sth3_0;  /* RW; Range: [0, 511]; The thresholds for protection of edges from blurring */
    td_u16 sth1_1;  /* RW; Range: [0, 511]; The thresholds for protection of edges from blurring */
    td_u16 sth2_1;  /* RW; Range: [0, 511]; The thresholds for protection of edges from blurring */
    td_u16 sth3_1;  /* RW; Range: [0, 511]; The thresholds for protection of edges from blurring */
    td_u16 sfn0_0;  /* RW; Range: [0,   8]; Filter selection for different image areas based on sth1_0~3. */
    td_u16 sfn1_0;  /* RW; Range: [0,   8]; Filter selection for different image areas based on sth1_0~3. */
    td_u16 sfn2_0;  /* RW; Range: [0,   8]; Filter selection for different image areas based on sth1_0~3. */
    td_u16 sfn3_0;  /* RW; Range: [0,   8]; Filter selection for different image areas based on sth1_0~3. */
    td_u16 sfn0_1;  /* RW; Range: [0,   8]; Filter selection for different image areas based on sth1_0~3. */
    td_u16 sfn1_1;  /* RW; Range: [0,   8]; Filter selection for different image areas based on sth1_0~3. */
    td_u16 sfn2_1;  /* RW; Range: [0,   8]; Filter selection for different image areas based on sth1_0~3. */
    td_u16 sfn3_1;  /* RW; Range: [0,   8]; Filter selection for different image areas based on sth1_0~3. */
    td_u8 sfn6_0;   /* RW; Range: [0,   5]; Filter selection for No. 6 filter. */
    td_u8 sfn6_1;   /* RW; Range: [0,   5]; Filter selection for No. 6 filter. */
    td_u8 sfn7_0;   /* RW; Range: [0,   5]; Filter selection for No. 7 filter. */
    td_u8 sfn7_1;   /* RW; Range: [0,   5]; Filter selection for No. 7 filter. */
    td_u8 sfn8_0;   /* RW; Range: [0,   5]; Filter selection for No. 8 filter. */
    td_u8 sfn8_1;   /* RW; Range: [0,   5]; Filter selection for No. 8 filter. */
} ext_pq_nr_v3_sfy; /*   nry1   nry2   nry3    */

typedef struct {
    td_u8 tfs0 : 4; /* RW;         Range: [0,  15]; The NR strength for temporal filtering. */
    td_u8 tfs1 : 4; /* RW;         Range: [0,  15]; The NR strength for temporal filtering. */
    td_u8 tfs2 : 4; /* RW;         Range: [0,  15]; The NR strength for temporal filtering. */
    td_u8 ref : 1;  /* RW;         Range: [0,  1]; The switch for temporal filtering. */
    td_u8 _rb_ : 3;
    td_u8 tss0;         /* RW;         Range: [0,  31]; The NR strength for spatial filtering. */
    td_u8 tss1;         /* RW;         Range: [0,  31]; The NR strength for spatial filtering. */
    td_u8 tss2;         /* RW;         Range: [0,  31]; The NR strength for spatial filtering. */
    td_u8 tfr0[6];      /* RW; Range: [0,  31]; The temporal NR strength control for background (static) area. */
    td_u8 tfr1[6];      /* RW; Range: [0,  31]; The temporal NR strength control for background (static) area. */
    td_u8 tfs0_mot[17]; /* RW; Range: [0,  16];  */
    td_u8 tfs1_mot[17]; /* RW; Range: [0,  16];  */
    td_u8 tfs_mode;     /* RW; Range: [0,  1];  */
} ext_pq_nr_v3_tfy;     /*  nry1  nry2  */

typedef struct {
    td_u16 math0;    /* RW;   Range: [0, 999]; The threshold for motion detection. */
    td_u16 mate0;    /* RW;   Range: [0,   8]; The motion index for smooth image area. */
    td_u16 math1;    /* RW;   Range: [0, 999]; The threshold for motion detection. */
    td_u16 mate1;    /* RW;   Range: [0,   8]; The motion index for smooth image area. */
    td_u8 mabw0 : 4; /* RW;   Range: [5,   9]; The window size for motion detection. */
    td_u8 mabw1 : 4; /* RW;   Range: [5,   9]; The window size for motion detection. */
} ext_pq_nr_v3_mdy;  /*  nry1  nry2 */

typedef struct {
    td_u16 math;      /* RW;   Range: [0, 999]; The threshold for motion detection. */
    td_u16 mate;      /* RW;   Range: [0,   8]; The motion index for smooth image area. */
    td_u8 mabw;       /* RW;   Range: [0,   4]; The window size for motion detection. */
} ext_pq_nr_v3_mdy0; /* nry0  */

typedef struct {
    td_u8 sf_var_f[17];  /* RW;   Range: [0, 63]; The NR strength of No.3 and No.4 filter for foreground region based on
                             variance.  */
    td_u8 sf_var_b[17];  /* RW;   Range: [0, 63]; The NR strength of No.3 and No.4 filter for background region based on
                             variance.  */
    td_u8 sf_dir_f[17];  /* RW;   Range: [0, 63]; The NR strength of No.3 and No.4 filter for foreground region based on
                            edge variance.  */
    td_u8 sf_dir_b[17];  /* RW;   Range: [0, 63]; The NR strength of No.3 and No.4 filter for background region based on
                            edge variance.  */
    td_u8 sf_bri_f[17];  /* RW;   Range: [0, 63]; The NR strength of No.3 and No.4 filter for foreground region based on
                             brightness.  */
    td_u8 sf_bri_b[17];  /* RW;   Range: [0, 63]; The NR strength of No.3 and No.4 filter for background region based on
                             brightness.  */
    td_u8 sf5_var_f[17]; /* RW;   Range: [0, 63]; The NR strength of No.5 filter for foreground region based on
                            variance.  */

    td_u8 sf5_var_b[17];  /* RW;   Range: [0, 63]; The NR strength of No.5 filter for background region based on
                             variance.  */
    td_u8 sf5_bri_f[17];  /* RW;   Range: [0, 63]; The NR strength of No.5 filter for foreground region based on
                             brightness.  */
    td_u8 sf5_bri_b[17];  /* RW;   Range: [0, 63]; The NR strength of No.5 filter for background region based on
                             brightness.  */
    td_u8 sf5_dir_f[17];  /* RW;   Range: [0, 63]; The NR strength of No.5 filter for foreground region based on edge
                             variance.  */
    td_u8 sf5_dir_b[17];  /* RW;   Range: [0, 63]; The NR strength of No.5 filter for background region based on edge
                             variance.  */
    td_u8 sf5_sad_f[17];  /* RW;   Range: [0, 63];  */
    td_u8 sf_cor[17];     /* RW;   Range: [0, 63];   */
    td_u8 sf5_cor[17];    /* RW;   Range: [0, 63];   */
    td_u8 sf_mot[17];     /* RW;   Range: [0, 16];   */
    td_u8 sf5_mot[17];    /* RW;   Range: [0, 16];   */
    td_u8 var_by_bri[17]; /* RW;   Range: [0, 63];  */
    td_u8 sf_bld[17];     /* RW;   Range: [0, 63]; The blending strength for No.6, No.7 and No.8 filter.  */
} ext_pq_nr_v3_sfy_lut; /*  nry2   nry3  */

typedef struct {
    td_u8 ca_lut[33]; /* RW;   Range: [0, 63];   */
    td_u8 ca_en;      /* RW;   Range: [0, 1]; The switch of post ca.  */
} ext_pq_nr_v3_pp;

typedef struct {
    td_u8 nrc0_tfr_thr_v;
    td_u8 nrc0_tfr_thr_u;
    td_u8 tfs;         /* RW; Range: [0,  15]; The NR strength for temporal filtering. */
    td_u8 tfs_mot[17]; /* RW; Range: [0,  16]; The motion mapping function based on motion map. */
} ext_pq_nr_v3_nrc0;

typedef struct {
    td_u8 pre_sfs; /* RW; Range: [0,  16]; The pre-processing of spacial filter. motion mapping function based on motion
                      map. */
    td_u8 sfs1;    /* RW; Range: [0, 255];  The NR strength parameters for NO.1 filter. */
    td_u8 sfs1_mot[17];  /* RW; Range: [0,  16]; The motion mapping function based on motion map. */
    td_u8 sfs2_coarse;   /* RW; Range: [0, 31];  The total NR strength parameters for NO.2 filter */
    td_u8 sfs2_coarse_f; /* RW; Range: [0, 31];  The background NR strength parameters for NO.2 filter. */
    td_u8 sfs2_fine_f;   /* RW; Range: [0, 16];  The foreground NR strength parameters for NO.2 filter */
    td_u8 sfs2_fine_b;   /* RW; Range: [0, 16];  The background NR strength parameters for NO.2 filter */
    td_u8 sfs2_size;      /* RW; Range: [8,  32]; The split block size for sfs2 when sfs2_mode is 2. */
    td_u16 sfs2_eps;      /* RW; Range: [0,  32767]; The NR strength for sfs2 when sfs2_mode is 2. */
    td_u8 sfs2_mot[16];  /* RW; Range: [0,  16]; The motion mapping function based on motion map. */
    td_u16 sfs2_sat[20]; /* RW; Range: [0,  1023]; The motion mapping function based on motion map. */
    td_u8 sfs2_mode;     /* RW; Range: [0,  2];  */
} ext_pq_nr_v3_nrc1;

typedef struct {
    ext_pq_nr_v3_sfy sfy[2];
    ext_pq_nr_v3_mdy mdy[2];
    ext_pq_nr_v3_tfy tfy[2];
    ext_pq_nr_v3_nrc0 nrc0;
    ext_pq_nr_v3_nrc1 nrc1;
    ext_pq_nr_v3_sfy_lut luty;
    ext_pq_nr_v3_pp pp;
    ext_pq_nr_v3_mdy0 mdy0;

    struct {
        td_u8 nry1_en : 1; /* RW; Range: [0, 1]; The switch for luma denoise. */
        td_u8 nry2_en : 1; /* RW; Range: [0, 1]; The switch for luma denoise. */
        td_u8 nrc_en : 1;  /* RW; Range: [0, 1]; The switch for chroma denoise. */
        td_u8 brefen : 1;
        td_u8 brfren : 1;
        td_u8 delaymode : 1;
        td_u8 nrc_online_gf_en : 1;
        td_u8 nr_mode : 3; /* for driver use */
    };
} ext_pq_nr_v3;

typedef struct {
    ext_pq_nr_v3 nrx_param;
} ext_pq_nr_param_manual_v3;

typedef struct {
    td_u32 param_num;
    td_u32 iso[EXT_ISP_NRX_AUTO_PARAM_MAX];  // 数组小标为8
    ext_pq_nr_v3 nrx_param[EXT_ISP_NRX_AUTO_PARAM_MAX];
} ext_pq_nr_param_auto_v3;

typedef struct {
    ext_isp_operation_mode opt_mode;
    ext_pq_nr_param_manual_v3 nrx_manual;
    ext_pq_nr_param_auto_v3 nrx_auto;
} ext_pq_nrx_param_v3;

typedef enum {
    EXT_PQ_NR_V1 = 1,
    EXT_PQ_NR_V2 = 2,
    EXT_PQ_NR_V3 = 3,
    EXT_PQ_NR_MAX = 4,
} ext_pq_nr_ver;

typedef struct {
    ext_pq_nr_ver nr_ver;
    union {
        ext_pq_nrx_param_v2 nrx_param_v2;  // 850
        ext_pq_nrx_param_v3 nrx_param_v3;  // 610
    };
} ext_pq_grp_nrx_param;
/************************************* sharpen*******************************************/
/************************************* sharpen auto*******************************************/

typedef struct {
    td_u16 shoot_inner_threshold[ISP_AUTO_ISO_STRENGTH_NUM];   /* RW; Range: [0, 511]; Format: 9.0;
                                                                  Start threshold of soft processing  interval */
    td_u16 shoot_outer_threshold[ISP_AUTO_ISO_STRENGTH_NUM];   /* RW: Range: [0, 511]; Format: 9.0;
                                                                  End threshold of sort processing interval */
    td_u16 shoot_protect_threshold[ISP_AUTO_ISO_STRENGTH_NUM]; /* RW: Range: [0, 511]; Format: 9.0;
                                                                  Hard processing of input threshold */
} ext_pq_sharpen_auto_shoot_threshold_attr;

typedef struct {
    td_u16 edge_rly_fine_threshold_low[ISP_AUTO_ISO_STRENGTH_NUM];    /* 1A22 New Added; RW; Range: [0, 1023];
                                                                         Format: 10.0; Low threshold of var 5,
                                                                         Not support for 850 */
    td_u16 edge_rly_fine_threshold_mid[ISP_AUTO_ISO_STRENGTH_NUM];  /* 1A22 New Added; RW; Range: [0, 1023];
                                                                       Format: 10.0; MId threshold of var5,
                                                                       Not support for 850 */
    td_u16 edge_rly_fine_threshold_high[ISP_AUTO_ISO_STRENGTH_NUM];   /* 1A22 New Added; RW; Range: [0, 1023];
                                                                         Format: 10.0; High threshold of var 5,
                                                                         Not support for 850 */
    td_u16 edge_rly_coarse_threshold_low[ISP_AUTO_ISO_STRENGTH_NUM];  /* 1A22 New Added; RW; Range: [0, 1023];
                                                                         Format: 10.0; Low threshold of var 7x9,
                                                                         Not support for 850 */
    td_u16 edge_rly_coarse_threshold_high[ISP_AUTO_ISO_STRENGTH_NUM]; /* 1A22 New Added; RW; Range: [0, 1023];
                                                                         Format: 10.0; High threshold of var 7x9,
                                                                         Not support for 850 */
    td_u8 edge_overshoot[ISP_AUTO_ISO_STRENGTH_NUM];  /* RW; Range: [0, 127]; Format: 7.0; edge overshoot */
    td_u8 edge_undershoot[ISP_AUTO_ISO_STRENGTH_NUM]; /* RW; Range: [0, 127]; Format: 7.0; edge undershoot */
    td_u8 edge_gain_by_rly[ISP_SHARPEN_RLYWGT_NUM][ISP_AUTO_ISO_STRENGTH_NUM];  /* RW; Range: [0, 255]; Format: 3.5;
                                                                                   weight calculated by dirRly */
    td_u8 edge_rly_by_mot[ISP_SHARPEN_STDGAIN_NUM][ISP_AUTO_ISO_STRENGTH_NUM]; /* RW; Range: [0, 63]; Format: 2.4;
                                                                                 stardard gain lut calculated by motion
                                                                               */
    td_u8 edge_rly_by_luma[ISP_SHARPEN_STDGAIN_NUM][ISP_AUTO_ISO_STRENGTH_NUM]; /* RW; Range: [0, 63]; Format: 2.4;
                                                                                   stardard gain lut calculated by luma
                                                                                 */
} ext_pq_sharpen_auto_edge_rly_attr;

typedef struct {
    td_u8 mf_gain_by_mot[ISP_SHARPEN_MOT_NUM][ISP_AUTO_ISO_STRENGTH_NUM];  /* RW; Range: [0, 32];
                                                                            Format: 1.5; mf decline weight by motion */
    td_u8 hf_gain_by_mot[ISP_SHARPEN_MOT_NUM][ISP_AUTO_ISO_STRENGTH_NUM];  /* RW; Range: [0, 32];
                                                                            Format: 1.5; hf decline weight by motion */
    td_u8 lmf_gain_by_mot[ISP_SHARPEN_MOT_NUM][ISP_AUTO_ISO_STRENGTH_NUM]; /* RW; Range: [0, 255];
                                                                            Format: 4.4; lmf gain by mot */
} ext_pq_sharpen_auto_gain_by_mot_attr;

typedef struct {
    td_u8 luma_wgt[ISP_SHARPEN_LUMA_NUM][ISP_AUTO_ISO_STRENGTH_NUM]; /* Adjust the sharpen strength according to luma.
                                                                    Sharpen strength will be weaker when it decrease */
    td_u16 texture_strength[ISP_SHARPEN_GAIN_NUM][ISP_AUTO_ISO_STRENGTH_NUM]; /* Undirectional sharpen strength for
                                                                              texture and detail enhancement */
    td_u16 edge_strength[ISP_SHARPEN_GAIN_NUM][ISP_AUTO_ISO_STRENGTH_NUM];    /* Directional sharpen strength for
                                                                              edge enhancement */
    td_u16 texture_freq[ISP_AUTO_ISO_STRENGTH_NUM];      /* Texture frequency adjustment.
                                                            Texture and detail will be finer when it increase */
    td_u16 edge_freq[ISP_AUTO_ISO_STRENGTH_NUM];         /* Edge frequency adjustment.
                                                            Edge will be narrower and thiner when it increase */
    td_u16 max_gain_adj_by_rly[ISP_AUTO_ISO_STRENGTH_NUM];     /* 1A22 New Added; Range: [0, 4095]; MaxGainAdjByRly */
    td_u8 rly_wgt_only_for_dir_gain[ISP_AUTO_ISO_STRENGTH_NUM]; /* 1A22 New Added; Range: [0, 1]; rlyWgtOnlyForDirGain
                                                                 */
    td_u8 over_shoot[ISP_AUTO_ISO_STRENGTH_NUM];                /* u8OvershootAmt */
    td_u8 under_shoot[ISP_AUTO_ISO_STRENGTH_NUM];               /* u8UndershootAmt */
    td_u8 shoot_sup_strength[ISP_AUTO_ISO_STRENGTH_NUM]; /* overshoot and undershoot suppression strength, the amplitude
                                                      and width of shoot will be decrease when shootSupSt increase */
    td_u8 shoot_sup_adj[ISP_AUTO_ISO_STRENGTH_NUM];      /* overshoot and undershoot suppression adjusting,
                                                              adjust the edge shoot suppression strength */

    td_u8 detail_ctrl_en[ISP_AUTO_ISO_STRENGTH_NUM];     /* 1A22 New Added; RW; Range: [0, 1]; bEnDetailCtrl */
    td_u8 detail_over_shoot[ISP_AUTO_ISO_STRENGTH_NUM];  /* 1A22 New Added; RW; Range: [0, 127]; detailOshtAmt */
    td_u8 detail_under_shoot[ISP_AUTO_ISO_STRENGTH_NUM]; /* 1A22 New Added; RW; Range: [0, 127]; detailUshtAmt */
    td_u8 detail_osht_thr0[ISP_AUTO_ISO_STRENGTH_NUM];   /* 1A22 New Added; RW; Range: [0, 255]; detailOshtThr[0] */
    td_u8 detail_osht_thr1[ISP_AUTO_ISO_STRENGTH_NUM];   /* 1A22 New Added; RW; Range: [0, 255]; detailOshtThr[1] */
    td_u8 detail_usht_thr0[ISP_AUTO_ISO_STRENGTH_NUM];   /* 1A22 New Added; RW; Range: [0, 255]; detailUshtThr[0] */
    td_u8 detail_usht_thr1[ISP_AUTO_ISO_STRENGTH_NUM];   /* 1A22 New Added; RW; Range: [0, 255]; detailUshtThr[1] */
    td_u8 edge_filt_strength[ISP_AUTO_ISO_STRENGTH_NUM]; /* The strength of edge filtering. */

    td_u8 edge_filt_dir_rt0[ISP_AUTO_ISO_STRENGTH_NUM]; /* 1A22 New Added; Range: [0, 31]; dirRt[0] */
    td_u8 edge_filt_dir_rt1[ISP_AUTO_ISO_STRENGTH_NUM]; /* 1A22 New Added; Range: [0, 31]; dirRt[1] */
    td_u8 r_gain[ISP_AUTO_ISO_STRENGTH_NUM];            /* Sharpen Gain for Red Area,
                                                           Range: 850 = [0, 31] | Hi3516CV610 = [0, 63] */
    td_u8 g_gain[ISP_AUTO_ISO_STRENGTH_NUM];            /* Sharpen Gain for Green Area ,Range: [0, 255] */
    td_u8 b_gain[ISP_AUTO_ISO_STRENGTH_NUM];            /* Sharpen Gain for Blue Area ,
                                                           Range: 850 = [0, 31] | Hi3516CV610 = [0, 63] */

    td_u8 skin_ctrl_en[ISP_AUTO_ISO_STRENGTH_NUM];     /* 1A22 New Added; Range: [0, 1]; bEnSkinCtrl */
    td_u8 skin_edge_thd0[ISP_AUTO_ISO_STRENGTH_NUM];   /* 1A22 New Added; Range: [0, 255]; skinEdgeThd[0] */
    td_u8 skin_edge_thd1[ISP_AUTO_ISO_STRENGTH_NUM];   /* 1A22 New Added; Range: [0, 255]; skinEdgeThd[1] */
    td_u8 skin_edge_wgt0[ISP_AUTO_ISO_STRENGTH_NUM];   /* 1A22 New Added; Range: [0, 31]; skinEdgeWgt[0] */
    td_u8 skin_edge_wgt1[ISP_AUTO_ISO_STRENGTH_NUM];   /* 1A22 New Added; Range: [0, 31]; skinEdgeWgt[1] */
    td_u16 max_sharp_gain[ISP_AUTO_ISO_STRENGTH_NUM];  /* Maximum sharpen gain */
    td_u8 weak_detail_gain[ISP_AUTO_ISO_STRENGTH_NUM]; /* just support for 850 */

    ext_pq_sharpen_auto_shoot_threshold_attr shoot_threshold_attr; /* RW; Not support for 850 */
    ext_pq_sharpen_auto_edge_rly_attr edge_rly_attr;               /* RW; Not support for 850 */
    ext_pq_sharpen_auto_gain_by_mot_attr gain_by_mot_attr;         /* RW; Not support for 850 */
} ext_pq_sharpen_auto_attr;

/************************************* sharpen manual*******************************************/
typedef struct {
    td_u16 shoot_inner_threshold;   /* RW; Range: [0, 511]; Format: 9.0; Start threshold of soft processing
                                       interval, Not Support for 850 */
    td_u16 shoot_outer_threshold;   /* RW: Range: [0, 511]; Format: 9.0; End threshold of sort processing
                                       interval, Not Support for 850 */
    td_u16 shoot_protect_threshold; /* RW: Range: [0, 511]; Format: 9.0; Hard processing of input threshold,
                                       Not Support for 850 */
} ext_pq_sharpen_manual_shoot_threshold_attr;

typedef struct {
    td_u16 edge_rly_fine_threshold_low;   /* 1A22 New Added; RW; Range: [0, 1023]; Format: 10.0; Low threshold of var5,
                                             Not support for 850 */
    td_u16 edge_rly_fine_threshold_mid;   /* 1A22 New Added; RW; Range: [0, 1023]; Format: 10.0; MId threshold of var5,
                                             Not support for 850 */
    td_u16 edge_rly_fine_threshold_high;  /* 1A22 New Added; RW; Range: [0, 1023]; Format: 10.0; High threshold of var5,
                                             Not support for 850 */
    td_u16 edge_rly_coarse_threshold_low; /* 1A22 New Added; RW; Range: [0, 1023]; Format: 10.0; Low threshold of
                                             var7x9, Not support for 850 */
    td_u16 edge_rly_coarse_threshold_high; /* 1A22 New Added; RW; Range: [0, 1023]; Format: 10.0; High threshold of
                                              var7x9, Not support for 850 */
    td_u8 edge_overshoot;                  /* RW; Range: [0, 127]; Format: 7.0; edge overshoot, Not support for 850 */
    td_u8 edge_undershoot;                 /* RW; Range: [0, 127]; Format: 7.0; edge undershoot, Not support for 850 */

    td_u8 edge_gain_by_rly[ISP_SHARPEN_RLYWGT_NUM];  /* RW; Range: [0, 255];
Format: 3.5; weight calculated by dirRly, Not support for 850 */
    td_u8 edge_rly_by_mot[ISP_SHARPEN_STDGAIN_NUM];  /* RW; Range: [0, 63];
             Format: 2.4; stardard gain lut calculated by motion, Not support for 850 */
    td_u8 edge_rly_by_luma[ISP_SHARPEN_STDGAIN_NUM]; /* RW; Range: [0, 63];
          Format: 2.4; stardard gain lut calculated by luma, Not support for 850 */
} ext_pq_sharpen_manual_edge_rly_attr;

typedef struct {
    td_u8 mf_gain_by_mot[ISP_SHARPEN_MOT_NUM];  /* RW; Range: [0, 32];
Format: 1.5; mf decline weight by motion, Not support for 850 */
    td_u8 hf_gain_by_mot[ISP_SHARPEN_MOT_NUM];  /* RW; Range: [0, 32];
Format: 1.5; hf decline weight by motion, Not support for 850 */
    td_u8 lmf_gain_by_mot[ISP_SHARPEN_MOT_NUM]; /* RW; Range: [0, 255];
 Format: 4.4; lmf gain by mot, Not support for 850 */
} ext_pq_sharpen_manual_gain_by_mot_attr;

typedef struct {
    td_u8 luma_wgt[ISP_SHARPEN_LUMA_NUM];          /* Adjust the sharpen strength according to luma.
                                                       Sharpen strength will be weaker when it decrease. */
    td_u16 texture_strength[ISP_SHARPEN_GAIN_NUM]; /* Undirectional sharpen strength for texture and detail enhancement
                                                    */
    td_u16 edge_strength[ISP_SHARPEN_GAIN_NUM];    /* Directional sharpen strength for edge enhancement */
    td_u16 texture_freq;        /* Texture frequency adjustment. Texture and detail will be finer when it increase */
    td_u16 edge_freq;           /* Edge frequency adjustment.Edge will be narrower and thiner when it increase */
    td_u16 max_gain_adj_by_rly; /* 1A22 New Added; Range: [0, 4095]; MaxGainAdjByRly */
    td_u8 rly_wgt_only_for_dir_gain; /* 1A22 New Added; Range: [0, 1]; rlyWgtOnlyForDirGain */
    td_u8 over_shoot;                /* u8OvershootAmt */
    td_u8 under_shoot;               /* u8UndershootAmt */
    td_u8 shoot_sup_strength; /* overshoot and undershoot suppression strength, the amplitude and width of shoot will be
                                 decrease when shootSupSt increase */
    td_u8 shoot_sup_adj;  /* overshoot and undershoot suppression adjusting, adjust the edge shoot suppression strength
                           */
    td_u8 detail_ctrl_en; /* 1A22 New Added; RW; Range: [0, 1]; bEnDetailCtrl */
    td_u8 detail_over_shoot;  /* 1A22 New Added; RW; Range: [0, 127]; detailOshtAmt */
    td_u8 detail_under_shoot; /* 1A22 New Added; RW; Range: [0, 127]; detailUshtAmt */
    td_u8 detail_osht_thr0;   /* 1A22 New Added; RW; Range: [0, 255]; detailOshtThr[0] */
    td_u8 detail_osht_thr1;   /* 1A22 New Added; RW; Range: [0, 255]; detailOshtThr[1] */
    td_u8 detail_usht_thr0;   /* 1A22 New Added; RW; Range: [0, 255]; detailUshtThr[0] */
    td_u8 detail_usht_thr1;   /* 1A22 New Added; RW; Range: [0, 255]; detailUshtThr[1] */
    td_u8 edge_filt_strength; /* RW; Range: [0, 63]; Format:6.0;The strength of edge filtering. */
    td_u8 edge_filt_dir_rt0;  /* 1A22 New Added; Range: [0, 31]; dirRt[0] */
    td_u8 edge_filt_dir_rt1;  /* 1A22 New Added; Range: [0, 31]; dirRt[1] */
    td_u8 r_gain;             /* RW; Sharpen Gain for Red Area ,
                                 Range: 850 = [0, 31] | Hi3516CV610 = [0, 63] */
    td_u8 g_gain;             /* RW; Range: [0, 255]; Format:8.0; Sharpen Gain for Green Area */
    td_u8 b_gain;             /* RW; Sharpen Gain for Blue Area,
                                 Range: 850 = [0, 31] | Hi3516CV610 = [0, 63] */
    td_u8 skin_ctrl_en;       /* 1A22 New Added; Range: [0, 1]; bEnSkinCtrl */
    td_u8 skin_edge_thd0;     /* 1A22 New Added; Range: [0, 255]; skinEdgeThd[0] */
    td_u8 skin_edge_thd1;     /* 1A22 New Added; Range: [0, 255]; skinEdgeThd[1] */
    td_u8 skin_edge_wgt0;     /* 1A22 New Added; Range: [0, 31]; skinEdgeWgt[0] */
    td_u8 skin_edge_wgt1;     /* 1A22 New Added; Range: [0, 31]; skinEdgeWgt[1] */
    td_u16 max_sharp_gain;    /* RW; Range: [0, 0x7FF]; Format:8.3; Maximum sharpen gain */
    td_u8 weak_detail_gain;   /* RW; Range: [0, 127]; sharpen Gain for weakdetail. Not support for 1A22 */

    ext_pq_sharpen_manual_shoot_threshold_attr shoot_threshold_attr; /* RW; Not support for 850 */
    ext_pq_sharpen_manual_edge_rly_attr edge_rly_attr;               /* RW; Not support for 850 */
    ext_pq_sharpen_manual_gain_by_mot_attr gain_by_mot_attr;         /* RW; Not support for 850 */
} ext_pq_sharpen_manual_attr;

typedef struct {
    td_bool enable;  /* RW; Range:[0, 1]; Format:1.0;Enable/Disable sharpen module */
    td_u8 skin_umin; /* RW; Range: [0, 255];  Format:8.0; U min value of the range of skin area */
    td_u8 skin_vmin; /* RW; Range: [0, 255];  Format:8.0; V min value of the range of skin area */
    td_u8 skin_umax; /* RW; Range: [0, 255];  Format:8.0; U max value of the range of skin area */
    td_u8 skin_vmax; /* RW; Range: [0, 255];  Format:8.0; V max value of the range of skin area */
    ext_isp_operation_mode op_type;
    ext_pq_sharpen_manual_attr manual_attr;
    ext_pq_sharpen_auto_attr auto_attr;
} ext_pq_sharpen_attr;
typedef struct {
    td_bool wnr_y_en;                   /* RW; Range: [0, 1]; the enabling flag for Luma WNR. */
    td_bool wnr_c_en;                   /* RW; Range: [0, 1]; the enabling flag for Chroma WNR. */
    td_bool wnr_y_layer1_hf_en;         /* RW; Range: [0, 1]; the enabling flag for Luma Layer 1 High Frequency Attenuation. */
    td_bool wnr_y_layer2_iir_en;        /* RW; Range: [0, 1]; the enabling flag for Luma Layer 2 Low Frequency IIR Filtering. */
    td_bool wnr_y_layer2_hf_en;         /* RW; Range: [0, 1]; the enabling flag for Luma Layer 2 High Frequency Attenuation. */
    td_bool wnr_c_layer1_iir_en;        /* RW; Range: [0, 1]; the enabling flag for Chroma Layer 1 Low Frequency IIR Filtering. */
    td_bool wnr_c_layer1_hf_en;         /* RW; Range: [0, 1]; the enabling flag for Chroma Layer 1 High Frequency Attenuation. */
    td_bool wnr_c_layer2_iir_en;        /* RW; Range: [0, 1]; the enabling flag for Chroma Layer 2 Low Frequency IIR Filtering. */
    td_bool wnr_c_layer2_hf_en;         /* RW; Range: [0, 1]; the enabling flag for Chroma Layer 2 High Frequency Attenuation. */
    td_s32   wnr_y_iir_ratio;            /* RW; Range: [-100, 0]; the Strength for Luma Low Frequencey IIR Filtering. 0 means stronglest */
    td_s32   wnr_y_hf_ratio;             /* RW; Range: [-100, 0]; the Strength for Luma High Frequencey Attenuation. 0 means stronglest */
    td_s32   wnr_c_iir_ratio;            /* RW; Range: [-100, 0]; the Strength for Chroma Low Frequencey IIR Filtering. 0 means stronglest */
    td_s32   wnr_c_hf_ratio;             /* RW; Range: [-100, 0]; the Strength for Chroma High Frequencey Attenuationg. 0 means stronglest */
    td_s32   wnr_diffusion_suppress;     /* RW; Range: [0, 10]; the Strength for Edge Protection for IIR Filter. 10 means stronglest */
} ext_pq_wnr_attr;
typedef struct {
    td_u32 param_num;
    td_u32 iso[EXT_ISP_NRX_AUTO_PARAM_MAX];  // 数组小标为8
    ext_pq_wnr_attr wnr_param[EXT_ISP_NRX_AUTO_PARAM_MAX];
} ext_pq_wnr_attr_auto;
typedef struct {
    ext_isp_operation_mode opt_mode;
    ext_pq_wnr_attr wnr_param_manual;
    ext_pq_wnr_attr wnr_param_auto;
    ext_pq_wnr_attr_auto wnr_auto;
} ext_pq_wnr_param;
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __DRV_PQ_STRUCT_H__ */
