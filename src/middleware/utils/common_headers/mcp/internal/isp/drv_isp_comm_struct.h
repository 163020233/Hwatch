/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: define for external use
 * Author: AuthorNameMagicTag
 * Create: 2021-09-22
 */

#ifndef __DRV_ISP_COMM_STRUCT_H__
#define __DRV_ISP_COMM_STRUCT_H__

#include "drv_isp_video_ext.h"
#include "mcp_arch_define.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#define ISP_SNS_2M_WIDTH              1600
#define ISP_SNS_2M_HEIGHT             1200
#define ISP_SNS_5M_WIDTH              2592
#define ISP_SNS_5M_HEIGHT             1944

/* arch relative */
#define ISP_MAX_STITCH_NUM 1

#define STATIC_DP_COUNT_NORMAL 2048
#define STATIC_DP_COUNT_MAX (STATIC_DP_COUNT_NORMAL * ISP_STRIPING_MAX_NUM)

#define AWB_ZONE_ORIG_ROW 32
#define AWB_ZONE_ORIG_COLUMN 32
#define AWB_ZONE_BIN 1
#define AWB_ZONE_BIN_MAX 4
#define AWB_ZONE_NUM (AWB_ZONE_ORIG_ROW * AWB_ZONE_ORIG_COLUMN * AWB_ZONE_BIN)
#define AWB_ZONE_CTE_CNT 1024

#define ISP_AUTO_ISO_STRENGTH_NUM 16
#define ISP_SHARPEN_LUMA_NUM 32
#define ISP_SHARPEN_GAIN_NUM 32

#define ISP_MAX_SNS_REGS 32

#define PRO_MAX_FRAME_NUM 5

#define ISP_BAYER_CHN_NUM 4

#define AWB_CURVE_PARA_NUM 6

#define EXT_ISP_CCM_MATRIX_SIZE 9
#define EXT_ISP_CCM_MATRIX_NUM 7

#define EXT_ISP_CCM_DGAIN_TAB_SIZE 65
#define EXT_ISP_DGAIN_POS_NUM          8

#define EXT_ISP_COLOR_SECTOR_EN_DEFAULT 0 /* not support for 850 */
#define EXT_ISP_COLOR_SECTOR_NUM        12  /* not support for 850 */

#define EXT_ISP_LDCI_STAT_WND_X 24
#define EXT_ISP_LDCI_STAT_WND_Y 16

#define EXT_ISP_MAX_SPLIT_NODE_NUM 3U
#define EXT_ISP_MAX_PID_NUM 5
#define EXP_RATIO_NUM 3

#define ISP_AE_ZONE_ROW 15
#define ISP_AE_ZONE_COLUMN 17

#if defined(CONFIG_SOCT_1A22_SUPPORT)
#define ISP_BE_AE_ZONE_ROW 32
#define ISP_BE_AE_ZONE_COLUMN 32

#define ISP_STRIPING_MAX_NUM 1
#else
#define ISP_BE_AE_ZONE_ROW 15
#define ISP_BE_AE_ZONE_COLUMN 17

#define ISP_STRIPING_MAX_NUM 3
#endif

#define ISP_AF_ZONE_ROW 15
#define ISP_AF_ZONE_COLUMN 17
#define ISP_AF_ALPHA 54  // 0.9
#define ISP_AF_BELTA 51  // 0.85
#define ISP_AF_BLEND_SHIFT 6
#define ISP_AF_LUT_LEN 64

#define ISP_HIST_NUM 1024
#define ISP_BAYER_PATTERN_NUM 4
#define ISP_WDR_FRAME_MAX 4 /* The maximum number of wdr frames of historical chips */

#define ISP_SYNC_DRC_PRELUMA_NUM 8

#define EXT_ISP_GDCI_BIN_NUM 256
#define GDCI_METRIC_NUM 3
#define GDCI_SKIN_LUT_MAX_IDX_U 16
#define GDCI_SKIN_LUT_MAX_IDX_V 16
#define GDCI_MESH_LUT01_LEN 273

#if defined(CONFIG_SOCT_1A22_SUPPORT)
#define EXT_ISP_CLUT_LUT_LENGTH 5204
#define CLUT_LUT01_LEN       730
#define CLUT_LUT23_LEN       648
#define CLUT_LUT45_LEN       648
#define CLUT_LUT67_LEN       576
#else
#define EXT_ISP_CLUT_LUT_LENGTH 5508
#endif

typedef struct {
    td_u64 phy_addr;
    td_u64 size;
    td_u64 vir_addr;
} isp_buf_rec;

typedef struct {
    td_u32 bnr_lut;
    td_u32 lsc_lut;
    td_u32 gamma_lut;
    td_u32 dehaze_lut;
    td_u32 ldci_lut;
    td_u32 ca_lut;
    td_u32 cc_lut;
    td_u32 para_lut0;
    td_u32 clut_lut;
    td_u32 gdci_mesh_lut;

    /* 1A22 not support */
    td_u32 pregamma_lut;
} isp_lut_info;

typedef struct {
    td_u32 ae_hist;
    td_u32 ae_aver_r_gr;
    td_u32 ae_aver_gb_b;
    td_u32 awb_stat;
    td_u32 awb_cte;
    td_u32 af_stat;
    td_u32 dehaze_min_stat;
    td_u32 dehaze_max_stat;
    td_u32 ldci_lpf;
    td_u32 gdci_binc;
    td_u32 gdci_biny;
    td_u32 gdci_map;
} isp_stt_info;

typedef struct {
    td_u32 be_cfg;
    td_u32 be_ctrl0;
    td_u32 be_ctrl1;
} isp_be_ctrl;

typedef struct {
    isp_buf_rec be_buf;
    isp_lut_info offset;
    isp_lut_info width_word;
    isp_stt_info stt_offset;
    isp_be_ctrl ctrl;
} isp_cfg_rec;

typedef enum {
    ISP_FSWDR_NORMAL_MODE = 0x0,
    ISP_FSWDR_LONG_FRAME_MODE = 0x1,
    ISP_FSWDR_AUTO_LONG_FRAME_MODE = 0x2, /* Auto long frame mode, only effective in LINE_WDR,  When running in this
                                             mode, normal WDR and long frame mode would auto switch */
    ISP_FSWDR_MODE_BUTT
} ext_isp_fswdr_mode;

/* Defines the structure of ISP control parameters. */
typedef struct {
    td_u32 be_buf_num;       /* RW;Range:[2, 9]; offline be cfg buffer number(Only used for offline mode).
                                Not support changed after isp init */
    td_u32 proc_param;       /* RW;Format:32.0; Indicate the update frequency of ISP_PROC information,
                                No distinction vipipe */
    td_u32 stat_intvl;       /* RW;Range:(0, 0xffffffff];Format:32.0;
                                Indicate the time interval of ISP statistic information */
    td_u32 update_pos;       /* RW;Range:[0, 1]; Format:32.0; Indicate the location of the configuration register of
                                ISP interrupt */
    td_u32 int_time_out;     /* RW;Format:32.0; Indicate the time(unit:ms) of interrupt timeout */
    td_u32 pwm_number;       /* R;Format:32.0; Indicate PWM number */
    td_u32 port_int_delay;   /* RW;Format:32.0; Port intertupt delay value, unit:clk */
    td_bool ldci_tpr_flt_en; /* R;Indicate Enable LDCI temporal filter. Not support changed after isp init */
    td_bool repeat_en;
} ext_isp_ctrl_param;

typedef enum {
    ISP_SCENE_PHOTO_PREVIEW = 0x0,
    ISP_SCENE_PHOTO,
    ISP_SCENE_VIDEO_RECORD_PREVIEW,
    ISP_SCENE_VIDEO_RECORD,
    ISP_SCENE_VIDEO_CALL_PREVIEW,
    ISP_SCENE_VIDEO_CALL,
    ISP_SCENE_MODE_BUTT
} ext_isp_scene_mode;

/* ISP public attribute, contains the public image attribute */
typedef struct {
    ext_isp_rect wnd_rect;      /* RW; start position of the cropping window, image width, and image height */
    ext_isp_size sns_size;      /* RW; width and height of the image output from the sensor */
    td_float f32_frame_rate;    /* RW; range: [0, 0xFFFF]; for frame rate */
    ext_isp_bayer_format bayer; /* RW; range:[0,3];format:2.0;the format of the input bayer image */
    ext_isp_wdr_mode wdr_mode;  /* RW; WDR mode select */
    td_u8 sns_mode;             /* RW; range: [0, 0xFF]; format: 8.0; for special sensor mode switches */
    ext_isp_scene_mode input_scene_mode;
} ext_isp_pub_attr;

typedef struct {
    td_u8 chroma_str[ISP_BAYER_CHN_NUM][ISP_AUTO_ISO_STRENGTH_NUM];  /* Strength of chrmoa noise reduction for
                                                                        R/Gr/Gb/B channel, */
    td_u8 fine_str[ISP_AUTO_ISO_STRENGTH_NUM];                       /* Strength of luma noise reduction */
    td_u16 coring_wgt[ISP_AUTO_ISO_STRENGTH_NUM];                    /* Strength of reserving the random noise */
    td_u16 coarse_str[ISP_BAYER_CHN_NUM][ISP_AUTO_ISO_STRENGTH_NUM]; /* Coarse Strength of noise reduction */
} ext_isp_nr_auto_attr;

typedef struct {
    td_u16 shoot_inner_threshold[ISP_AUTO_ISO_STRENGTH_NUM];   /* RW; Range: [0, 511]; Format: 9.0;
                                                                  Start threshold of soft processing  interval */
    td_u16 shoot_outer_threshold[ISP_AUTO_ISO_STRENGTH_NUM];   /* RW: Range: [0, 511]; Format: 9.0;
                                                                  End threshold of sort processing interval */
    td_u16 shoot_protect_threshold[ISP_AUTO_ISO_STRENGTH_NUM]; /* RW: Range: [0, 511]; Format: 9.0;
                                                                  Hard processing of input threshold */
} ext_isp_sharpen_auto_shoot_threshold_attr;

typedef struct {
    td_u16 edge_rly_fine_threshold_low[ISP_AUTO_ISO_STRENGTH_NUM];      /* 1A22 New Added; RW; Range: [0, 1023]; Format: 10.0; Low threshold of var5, Not support for 850 */
    td_u16 edge_rly_fine_threshold_mid[ISP_AUTO_ISO_STRENGTH_NUM];      /* 1A22 New Added; RW; Range: [0, 1023]; Format: 10.0; MId threshold of var5, Not support for 850 */
    td_u16 edge_rly_fine_threshold_high[ISP_AUTO_ISO_STRENGTH_NUM];     /* 1A22 New Added; RW; Range: [0, 1023]; Format: 10.0; High threshold of var5, Not support for 850 */
    td_u16 edge_rly_coarse_threshold_low[ISP_AUTO_ISO_STRENGTH_NUM];    /* 1A22 New Added; RW; Range: [0, 1023]; Format: 10.0; Low threshold of var7x9, Not support for 850 */
    td_u16 edge_rly_coarse_threshold_high[ISP_AUTO_ISO_STRENGTH_NUM];   /* 1A22 New Added; RW; Range: [0, 1023]; Format: 10.0; High threshold of var7x9, Not support for 850 */
    td_u8 edge_overshoot[ISP_AUTO_ISO_STRENGTH_NUM];             /* RW; Range: [0, 127]; Format: 7.0; edge overshoot */
    td_u8 edge_undershoot[ISP_AUTO_ISO_STRENGTH_NUM];            /* RW; Range: [0, 127]; Format: 7.0; edge undershoot */
    td_u8 edge_gain_by_rly[ISP_SHARPEN_RLYWGT_NUM][ISP_AUTO_ISO_STRENGTH_NUM];  /* RW; Range: [0, 255];
                                                                     Format: 3.5; weight calculated by dirRly */
    td_u8 edge_rly_by_mot[ISP_SHARPEN_STDGAIN_NUM][ISP_AUTO_ISO_STRENGTH_NUM];  /* RW; Range: [0, 63];
                                                  Format: 2.4; stardard gain lut calculated by motion */
    td_u8 edge_rly_by_luma[ISP_SHARPEN_STDGAIN_NUM][ISP_AUTO_ISO_STRENGTH_NUM]; /* RW; Range: [0, 63];
                                                            Format: 2.4; stardard gain lut calculated by luma */
} ext_isp_sharpen_auto_edge_rly_attr;

typedef struct {
    td_u8 mf_gain_by_mot[ISP_SHARPEN_MOT_NUM][ISP_AUTO_ISO_STRENGTH_NUM];  /* RW; Range: [0, 32];
                                                                            Format: 1.5; mf decline weight by motion */
    td_u8 hf_gain_by_mot[ISP_SHARPEN_MOT_NUM][ISP_AUTO_ISO_STRENGTH_NUM];  /* RW; Range: [0, 32];
                                                                            Format: 1.5; hf decline weight by motion */
    td_u8 lmf_gain_by_mot[ISP_SHARPEN_MOT_NUM][ISP_AUTO_ISO_STRENGTH_NUM]; /* RW; Range: [0, 255];
                                                                            Format: 4.4; lmf gain by mot */
} ext_isp_sharpen_auto_gain_by_mot_attr;

typedef struct {
    td_u8  detail_ctrl[ISP_AUTO_ISO_STRENGTH_NUM];    /* Different sharpen strength for detail and edge.
                                     When it is bigger than 128, detail sharpen strength will be stronger than edge. */
    td_u8  detail_ctrl_threshold[ISP_AUTO_ISO_STRENGTH_NUM]; /* The threshold of DetailCtrl,
                                                          it is used to distinguish detail and edge. */
    td_u8  edge_filt_max_cap[ISP_AUTO_ISO_STRENGTH_NUM]; /* The max capacity of edge filtering. */
    td_u8  skin_gain[ISP_AUTO_ISO_STRENGTH_NUM];      /* Sharpen Gain for Skin Area */
    td_u8  weak_detail_gain[ISP_AUTO_ISO_STRENGTH_NUM];  /* just support for 850 */
} ext_isp_sharpen_auto_adv_attr_v1;    /* just support for 850 */

typedef struct {
    td_u16 max_gain_adj_by_rly[ISP_AUTO_ISO_STRENGTH_NUM];          /* 1A22 New Added; Range: [0, 4095]; MaxGainAdjByRly */
    td_u8  rly_wgt_only_for_dir_gain[ISP_AUTO_ISO_STRENGTH_NUM];    /* 1A22 New Added; Range: [0, 1]; rlyWgtOnlyForDirGain */

    td_u8  detail_ctrl_en[ISP_AUTO_ISO_STRENGTH_NUM];        /* 1A22 New Added; RW; Range: [0, 1]; bEnDetailCtrl */
    td_u8  detail_over_shoot[ISP_AUTO_ISO_STRENGTH_NUM];     /* 1A22 New Added; RW; Range: [0, 127]; detailOshtAmt */
    td_u8  detail_under_shoot[ISP_AUTO_ISO_STRENGTH_NUM];    /* 1A22 New Added; RW; Range: [0, 127]; detailUshtAmt */
    td_u8  detail_osht_thr0[ISP_AUTO_ISO_STRENGTH_NUM];      /* 1A22 New Added; RW; Range: [0, 255]; detailOshtThr[0] */
    td_u8  detail_osht_thr1[ISP_AUTO_ISO_STRENGTH_NUM];      /* 1A22 New Added; RW; Range: [0, 255]; detailOshtThr[1] */
    td_u8  detail_usht_thr0[ISP_AUTO_ISO_STRENGTH_NUM];      /* 1A22 New Added; RW; Range: [0, 255]; detailUshtThr[0] */
    td_u8  detail_usht_thr1[ISP_AUTO_ISO_STRENGTH_NUM];      /* 1A22 New Added; RW; Range: [0, 255]; detailUshtThr[1] */

    td_u8  edge_filt_dir_rt0[ISP_AUTO_ISO_STRENGTH_NUM];    /* 1A22 New Added; Range: [0, 31]; dirRt[0] */
    td_u8  edge_filt_dir_rt1[ISP_AUTO_ISO_STRENGTH_NUM];    /* 1A22 New Added; Range: [0, 31]; dirRt[1] */

    td_u8  skin_ctrl_en[ISP_AUTO_ISO_STRENGTH_NUM];       /* 1A22 New Added; Range: [0, 1]; */
    td_u8  skin_max_u[ISP_AUTO_ISO_STRENGTH_NUM];       /* 1A22 New Added; Range: [0, 255]; */
    td_u8  skin_min_u[ISP_AUTO_ISO_STRENGTH_NUM];       /* 1A22 New Added; Range: [0, 255]; */
    td_u8  skin_max_v[ISP_AUTO_ISO_STRENGTH_NUM];       /* 1A22 New Added; Range: [0, 255]; */
    td_u8  skin_min_v[ISP_AUTO_ISO_STRENGTH_NUM];       /* 1A22 New Added; Range: [0, 255]; */
    td_u8  skin_cnt_thd0[ISP_AUTO_ISO_STRENGTH_NUM];    /* 1A22 New Added; Range: [0, 9]; */
    td_u8  skin_cnt_thd1[ISP_AUTO_ISO_STRENGTH_NUM];    /* 1A22 New Added; Range: [0, 9]; */
    td_u8  skin_edge_sft[ISP_AUTO_ISO_STRENGTH_NUM];    /* 1A22 New Added; Range: [0, 12]; */
    td_u8  skin_edge_thd0[ISP_AUTO_ISO_STRENGTH_NUM];     /* 1A22 New Added; Range: [0, 255]; */
    td_u8  skin_edge_thd1[ISP_AUTO_ISO_STRENGTH_NUM];     /* 1A22 New Added; Range: [0, 255]; */
    td_u8  skin_edge_wgt0[ISP_AUTO_ISO_STRENGTH_NUM];     /* 1A22 New Added; Range: [0, 31]; */
    td_u8  skin_edge_wgt1[ISP_AUTO_ISO_STRENGTH_NUM];     /* 1A22 New Added; Range: [0, 31]; */

    td_u8 shoot_ctrl_by_var_en[ISP_AUTO_ISO_STRENGTH_NUM]; /* Range:[0, 1] */
    td_u8 shoot_blend_rate[ISP_AUTO_ISO_STRENGTH_NUM]; /* Range:[0, 15] */
    td_u8 shoot_var_thd0[ISP_AUTO_ISO_STRENGTH_NUM]; /* Range:[0, 255] */
    td_u8 shoot_var_thd1[ISP_AUTO_ISO_STRENGTH_NUM]; /* Range:[0, 255] */
    td_u8 shoot_var_wgt0[ISP_AUTO_ISO_STRENGTH_NUM]; /* Range:[0, 127] */
    td_u8 shoot_var_wgt1[ISP_AUTO_ISO_STRENGTH_NUM]; /* Range:[0, 127] */
    td_u8 shoot_var_diff_thd0[ISP_AUTO_ISO_STRENGTH_NUM]; /* Range:[0, 255] */
    td_u8 shoot_var_diff_thd1[ISP_AUTO_ISO_STRENGTH_NUM]; /* Range:[0, 255] */
    td_u8 shoot_var_diff_wgt0[ISP_AUTO_ISO_STRENGTH_NUM]; /* Range:[0, 127] */
    td_u8 shoot_var_diff_wgt1[ISP_AUTO_ISO_STRENGTH_NUM]; /* Range:[0, 127] */
} ext_isp_sharpen_auto_adv_attr_v2;      /* just support for 1A22 */

typedef struct {
    td_u8 luma_wgt[ISP_SHARPEN_LUMA_NUM][ISP_AUTO_ISO_STRENGTH_NUM]; /* Adjust the sharpen strength according to luma.
                                                                    Sharpen strength will be weaker when it decrease */
    td_u16 texture_strength[ISP_SHARPEN_GAIN_NUM][ISP_AUTO_ISO_STRENGTH_NUM]; /* Undirectional sharpen strength for
                                                                              texture and detail enhancement */
    td_u16 edge_strength[ISP_SHARPEN_GAIN_NUM][ISP_AUTO_ISO_STRENGTH_NUM];    /* Directional sharpen strength for
                                                                              edge enhancement */
    td_u16 texture_freq[ISP_AUTO_ISO_STRENGTH_NUM];  /* Texture frequency adjustment.
                                                         Texture and detail will be finer when it increase */
    td_u16 edge_freq[ISP_AUTO_ISO_STRENGTH_NUM];     /* Edge frequency adjustment.
                                                        Edge will be narrower and thiner when it increase */
    td_u8 over_shoot[ISP_AUTO_ISO_STRENGTH_NUM];                              /* u8OvershootAmt */
    td_u8 under_shoot[ISP_AUTO_ISO_STRENGTH_NUM];                             /* u8UndershootAmt */
    td_u8 shoot_sup_strength[ISP_AUTO_ISO_STRENGTH_NUM]; /* overshoot and undershoot suppression strength,
                                         the amplitude and width of shoot will be decrease when shootSupSt increase */
    td_u8 shoot_sup_adj[ISP_AUTO_ISO_STRENGTH_NUM];      /* overshoot and undershoot suppression adjusting,
                                                              adjust the edge shoot suppression strength */

    td_u8 edge_filt_strength[ISP_AUTO_ISO_STRENGTH_NUM];    /* The strength of edge filtering. */

    td_u8 r_gain[ISP_AUTO_ISO_STRENGTH_NUM];                /* Sharpen Gain for Red Area,
                                                               Range: 850 = [0, 31] | Hi3516CV610 = [0, 63] */
    td_u8 g_gain[ISP_AUTO_ISO_STRENGTH_NUM];                /* Sharpen Gain for Green Area ,Range: [0, 255] */
    td_u8 b_gain[ISP_AUTO_ISO_STRENGTH_NUM];                /* Sharpen Gain for Blue Area ,
                                                               Range: 850 = [0, 31] | Hi3516CV610 = [0, 63] */

    td_u16 max_sharp_gain[ISP_AUTO_ISO_STRENGTH_NUM];       /* Maximum sharpen gain */

    union {
        ext_isp_sharpen_auto_adv_attr_v1       adv_attr_v1;    /* chip:850 */
        ext_isp_sharpen_auto_adv_attr_v2       adv_attr_v2;    /* chip:1A22 */
    };

    ext_isp_sharpen_auto_shoot_threshold_attr  shoot_threshold_attr;  /* just support for 1A22 */
    ext_isp_sharpen_auto_edge_rly_attr         edge_rly_attr;         /* just support for 1A22 */
    ext_isp_sharpen_auto_gain_by_mot_attr      gain_by_mot_attr;      /* just support for 1A22 */
} ext_isp_sharpen_auto_attr;

/*
  Defines the FPN removal type
  0 = Frame mode correction
  1 = Line mode correction
 */
typedef enum {
    ISP_FPN_TYPE_FRAME = 0,
    ISP_FPN_TYPE_LINE = 1,
    ISP_FPN_TYPE_BUTT
} ext_isp_fpn_type;

/* Defines the information about calibrated black frames */
typedef struct {
    td_u32 iso;                                /* RW;Range:[0x64,0xFFFFFFFF];Format:32.0;FPN CALIBRATE ISO */
    td_u32 offset[EXT_ISP_MAX_SPLIT_NODE_NUM]; /* RW;Range:[0, 0xFFF];Format:12.0;
                                             FPN frame u32Offset (agv pixel value) */
    td_u32 frm_size;                           /* RW;FPN frame size (exactly frm size or compress len) */
    ext_isp_video_frame_info fpn_frame;        /* FPN frame info, 8bpp,10bpp,12bpp,16bpp. Compression or not */
} ext_isp_fpn_frame_info;

typedef enum {
    EXT_VIU_FPN_MODE_NONE = 0x0,
    EXT_VIU_FPN_MODE_CORRECTION,
    EXT_VIU_FPN_MODE_CALIBRATE,
    EXT_VIU_FPN_MODE_MAX
} ext_viu_fpn_work_mode;

/* Defines the calibration attribute of the FPN removal module */
typedef struct {
    td_u32 threshold;          /* pix value > threshold means defective pixel */
    td_u32 frame_num;          /* Number of frames to be calibrated.
                                The value range is {1, 2, 4, 8, 16},that is, the integer exponentiation of 2 */
    ext_isp_fpn_type fpn_type; /* frame mode or line mode */
    ext_isp_fpn_frame_info fpn_cali_frame;
} ext_isp_fpn_calibrate_attr;

/* Defines the manual correction attribute of the FPN removal module */
typedef struct {
    td_u32 strength; /* Manual correction strength */
} ext_isp_fpn_manual_attr;

/* Defines the automatic correction attribute of the FPN removal module */
typedef struct {
    td_u32 strength; /* Auto correction strength */
} ext_isp_fpn_auto_attr;

/* Defines the correction attribute of the FPN removal module */
typedef struct {
    td_bool enable;
    ext_isp_op_type op_type; /* manual or auto mode */
    ext_isp_fpn_type fpn_type;
    ext_isp_fpn_frame_info fpn_frm_info;
    ext_isp_fpn_manual_attr manual_attr;
    ext_isp_fpn_auto_attr auto_attr;
} ext_isp_fpn_attr;

typedef struct {
    ext_viu_fpn_work_mode fpn_work_mode;
    union {
        ext_isp_fpn_calibrate_attr calibrate_attr;
        ext_isp_fpn_attr correction_attr;
    };
} ext_viu_fpn_attr;

/* config of statistics structs */
typedef union {
    td_u64 key;
    struct {
        td_u64 bit1_fe_ae_glo_stat : 1;     /* [0] */
        td_u64 bit1_fe_ae_loc_stat : 1;     /* [1] */
        td_u64 bit1_fe_ae_sti_glo_stat : 1; /* [2] */
        td_u64 bit1_fe_ae_sti_loc_stat : 1; /* [3] */
        td_u64 bit1_be_ae_glo_stat : 1;     /* [4] */
        td_u64 bit1_be_ae_loc_stat : 1;     /* [5] */
        td_u64 bit1_be_ae_sti_glo_stat : 1; /* [6] */
        td_u64 bit1_be_ae_sti_loc_stat : 1; /* [7] */
        td_u64 bit1_awb_stat1 : 1;          /* [8] */
        td_u64 bit1_awb_stat2 : 1;          /* [9] */
        td_u64 bit2_rsv0 : 2;               /* [10:11] */
        td_u64 bit1_fe_af_stat : 1;         /* [12] */
        td_u64 bit1_be_af_stat : 1;         /* [13] */
        td_u64 bit1_dp_stat : 1;            /* [14] */
        td_u64 bit1_comm_stat : 1;          /* [15] */
        td_u64 bit1_dehaze : 1;             /* [16] */
        td_u64 bit1_mg_stat : 1;            /* [17] */
        td_u64 bit1_flicker : 1;            /* [18] */
        td_u64 bit1_extend_stats : 1;       /* [19] */
        td_u64 bit12_rsv : 12;              /* [20:31] */
        td_u64 bit32_isr_access : 32;       /* [32:63] */
    };
} ext_isp_statistics_ctrl;

/*
0 = Communication between the sensor and the ISP over the I2C interface
1 = Communication between the sensor and the ISP over the SSP interface
*/
typedef enum {
    ISP_SNS_I2C_TYPE = 0,
    ISP_SNS_SSP_TYPE,

    ISP_SNS_TYPE_BUTT,
} ext_isp_sns_type;

/* sensor communication bus */
typedef union {
    td_s8 i2c_dev;
    struct {
        td_s8 bit4_ssp_dev : 4;
        td_s8 bit4_ssp_cs : 4;
    } ssp_dev;
} ext_isp_sns_commbus;

typedef enum {
    ISP_SNS_NORMAL = 0,
    ISP_SNS_MIRROR = 1,
    ISP_SNS_FLIP = 2,
    ISP_SNS_MIRROR_FLIP = 3,
    ISP_SNS_BUTT
} ext_isp_sns_mirrorflip_type;

typedef struct {
    td_bool update;       /* RW; Range: [0x0, 0x1]; Format:1.0;
                             TD_TRUE: The sensor registers are written,
                             TD_FALSE: The sensor registers are not written */
    td_u8 delay_frm_num;  /* RW; Number of delayed frames for the sensor register */
    td_u8 int_pos;        /* RW;Position where the configuration of the sensor register takes effect */
    td_u8 dev_addr;       /* RW;Sensor device address */
    td_u32 reg_addr;      /* RW;Sensor register address */
    td_u32 addr_byte_num; /* RW;Bit width of the sensor register address */
    td_u32 data;          /* RW;Sensor register data */
    td_u32 data_byte_num; /* RW;Bit width of sensor register data */
} ext_isp_i2c_data;

typedef struct {
    td_bool update;           /* RW; Range: [0x0, 0x1]; Format:1.0;
                                 TD_TRUE: The sensor registers are written,
                                 TD_FALSE: The sensor registers are not written */
    td_u8 delay_frm_num;      /* RW; Number of delayed frames for the sensor register */
    td_u8 int_pos;            /* RW;Position where the configuration of the sensor register takes effect */
    td_u32 dev_addr;          /* RW;Sensor device address */
    td_u32 dev_addr_byte_num; /* RW;Bit width of the sensor device address */
    td_u32 reg_addr;          /* RW;Sensor register address */
    td_u32 reg_addr_byte_num; /* RW;Bit width of the sensor register address */
    td_u32 data;              /* RW;Sensor register data */
    td_u32 data_byte_num;     /* RW;Bit width of sensor register data */
} ext_isp_ssp_data;

typedef struct {
    ext_isp_sns_type sns_type;
    td_u32 reg_num;             /* RW;Number of registers required when exposure results are written to the sensor.
                                   The member value cannot be dynamically changed */
    td_u8 cfg2_valid_delay_max; /* RW;Maximum number of delayed frames from the time when all sensor registers are
                                   configured to the time when configurations take effect, which is used to ensure
                                   the synchronization between sensor registers and ISP registers */
    ext_isp_sns_commbus com_bus;
    union {
        ext_isp_i2c_data i2c_data[ISP_MAX_SNS_REGS];
        ext_isp_ssp_data ssp_data[ISP_MAX_SNS_REGS];
    };

    struct {
        td_bool update;
        td_u8 delay_frm_num;
        td_u32 slave_vs_time; /* RW;time of vsync. Unit: inck clock cycle */
        td_u32 slave_bind_dev;
    } slv_sync;

    td_bool config;
} ext_isp_sns_regs_info;

/*
  DNG cfalayout type
  1 = Rectangular (or square) layout
  2 = Staggered layout A: even columns are offset down by 1/2 row
  3 = Staggered layout B: even columns are offset up by 1/2 row
  4 = Staggered layout C: even rows are offset right by 1/2 column
  5 = Staggered layout D: even rows are offset left by 1/2 column
  6 = Staggered layout E: even rows are offset up by 1/2 row, even columns are offset left by 1/2 column
  7 = Staggered layout F: even rows are offset up by 1/2 row, even columns are offset right by 1/2 column
  8 = Staggered layout G: even rows are offset down by 1/2 row, even columns are offset left by 1/2 column
  9 = Staggered layout H: even rows are offset down by 1/2 row, even columns are offset right by 1/2 column
 */
typedef enum {
    CFALAYOUT_TYPE_RECTANGULAR = 1,
    CFALAYOUT_TYPE_A, /* a,b,c... not support */
    CFALAYOUT_TYPE_B,
    CFALAYOUT_TYPE_C,
    CFALAYOUT_TYPE_D,
    CFALAYOUT_TYPE_E,
    CFALAYOUT_TYPE_F,
    CFALAYOUT_TYPE_G,
    CFALAYOUT_TYPE_H,
    CFALAYOUT_TYPE_BUTT
} ext_isp_dng_cfa_layout_type;

typedef struct {
    td_s32 numerator;   /* represents the numerator of a fraction, */
    td_s32 denominator; /* the denominator. */
} ext_isp_dng_srational;

typedef struct {
    td_u16 blc_repeat_rows;
    td_u16 blc_repeat_cols;
} ext_isp_dng_blc_repeat_dim;

typedef struct {
    ext_isp_dng_srational default_scale_h;
    ext_isp_dng_srational default_scale_v;
} ext_isp_dng_default_scale;

typedef struct {
    td_u16 repeat_pattern_dim_rows;
    td_u16 repeat_pattern_dim_cols;
} ext_isp_dng_repeat_pattern_dim;

/* Defines the structure of dng raw format. */
typedef struct {
    td_u8 bits_per_sample;                        /* RO;Format:8.0; Indicate the bit numbers of raw data */
    td_u8 cfa_plane_color[EXT_ISP_CFACOLORPLANE]; /* Indicate the planer numbers of raw data;
                                                   0:red 1:green 2: blue */
    ext_isp_dng_cfa_layout_type cfa_layout;       /* RO;Range:[1,9]; Describes the spatial layout of the CFA */
    ext_isp_dng_blc_repeat_dim blc_repeat_dim;    /* Specifies repeat pattern size for the BlackLevel */
    td_u32 white_level;                           /* RO;Format:32.0; Indicate the WhiteLevel of the raw data */
    ext_isp_dng_default_scale default_scale;      /* Specifies the default scale factors for each direction to convert
                                                    the image to square pixels */
    ext_isp_dng_repeat_pattern_dim cfa_repeat_pattern_dim; /* Specifies the pixel number of repeat color planer in
                                                             each direction */
    td_u8 cfa_pattern[ISP_BAYER_CHN_NUM];                  /* Indicate the bayer start order; 0:red 1:green 2: blue */
} ext_isp_dng_raw_format;

/* Defines the structure of dng image static infomation. read only */
typedef struct {
    ext_isp_dng_raw_format dng_raw_format;
    ext_isp_dng_srational color_matrix1[EXT_ISP_CCM_MATRIX_SIZE]; /* defines a transformation matrix that converts XYZ
                                                                 values to reference camera native color space values,
                                                                 under the first calibration illuminant. */
    ext_isp_dng_srational color_matrix2[EXT_ISP_CCM_MATRIX_SIZE]; /* defines a transformation matrix that converts XYZ
                                                                 values to reference camera native color space values,
                                                                 under the second calibration illuminant. */
    ext_isp_dng_srational camera_calibration1[EXT_ISP_CCM_MATRIX_SIZE]; /* defines a calibration matrix that transforms
                                                       reference camera native space values to individual camera native
                                                       space values under the first calibration illuminant */
    ext_isp_dng_srational camera_calibration2[EXT_ISP_CCM_MATRIX_SIZE]; /* defines a calibration matrix that transforms
                                                       reference camera native space values to individual camera native
                                                       space values under the second calibration illuminant */
    ext_isp_dng_srational forwad_matrix1[EXT_ISP_CCM_MATRIX_SIZE]; /* defines a matrix that maps white balanced camera
                                                                  colors to XYZ D50 colors */
    ext_isp_dng_srational forwad_matrix2[EXT_ISP_CCM_MATRIX_SIZE]; /* defines a matrix that maps white balanced camera
                                                                  colors to XYZ D50 colors */

    td_u8 calibration_illuminant1;
    td_u8 calibration_illuminant2;
} ext_isp_dng_image_static_info;

typedef enum {
    EXT_SNAP_TYPE_NORMAL, /* ZSL and none-ZSL type */
    EXT_SNAP_TYPE_PRO,    /* Professional type support HDR, AEB, LongExposure */
    EXT_SNAP_TYPE_MAX
} ext_isp_snap_type;

typedef struct {
    td_u32 man_exp_time[PRO_MAX_FRAME_NUM]; /* RW;  Range: [0x0, 0xFFFFFFFF];sensor exposure time (unit: us),
                                                                    it's related to the specific sensor */
    td_u32 man_sysgain[PRO_MAX_FRAME_NUM];  /* RW;  Range: [0x400, 0xFFFFFFFF];system gain (unit: times,
                                                                     10bit precision), it's related to the specific
                                                                     sensor and ISP dgain range */
} ext_isp_snap_pro_manual_param;

typedef struct {
    td_u16 pro_exp_step[PRO_MAX_FRAME_NUM]; /* RW; Range:[0x0, 0xFFFF];Professional exposure step
                                                                    (unit: times, 8bit precision) */
} ext_isp_snap_pro_auto_param;

typedef struct {
    td_bool enable;
    td_u32 param_num;
    ext_isp_nr_auto_attr nr_attr[PRO_MAX_FRAME_NUM];
} ext_isp_pro_nr_param;

typedef struct {
    td_bool enable;
    td_u32 param_num;
    ext_isp_sharpen_auto_attr shp_attr[PRO_MAX_FRAME_NUM];
} ext_isp_pro_sharpen_param;

typedef struct {
    ext_isp_operation_mode operation_mode;
    td_u32 pro_frame_num;
    ext_isp_snap_pro_auto_param auto_param;
    ext_isp_snap_pro_manual_param manual_param;
} isp_pro_param;

typedef struct {
    ext_isp_snap_type snap_type;
    td_s32 preview_pipe_id; /* -1,none snap */
    td_s32 picture_pipe_id; /* -1,none snap */
    isp_pro_param pro_param;
    td_bool load_ccm; /* TD_TRUE:use CCM of snap_isp_info, TD_FALSE: algorithm calculate */
} isp_snap_attr;

/********************************************** copy from isp_arch.h **********************************************/
typedef enum {
    ISP_AWB_AFTER_DG = 0,
    ISP_AWB_AFTER_Expander,
    ISP_AWB_AFTER_DRC,
    ISP_AWB_SWITCH_BUTT
} ext_isp_awb_switch;

typedef struct {
    td_u16 blc[ISP_BAYER_CHN_NUM];
    td_u16 out_blc;
} isp_blc_dyna_cfg;

typedef struct {
    td_bool resh_dyna;
    isp_blc_dyna_cfg lsc_blc;
    isp_blc_dyna_cfg ge_blc;
    isp_blc_dyna_cfg dg_blc;
    isp_blc_dyna_cfg wdr_blc[ISP_WDR_CHN_MAX];
    isp_blc_dyna_cfg expander_blc;
    isp_blc_dyna_cfg wb_blc;
    isp_blc_dyna_cfg wdr_dg_blc[ISP_WDR_CHN_MAX];
    isp_blc_dyna_cfg ae_blc;
    isp_blc_dyna_cfg mg_blc;
    isp_blc_dyna_cfg flicker_blc[ISP_WDR_CHN_MAX];
    isp_blc_dyna_cfg de_blc;
    isp_blc_dyna_cfg bnr_blc;
} isp_be_blc_dyna_cfg;

typedef struct {
    td_u32 ldci_comp;
    td_u8 drc_shp_log;
    td_u32 drc_div_denom_log;
    td_u32 drc_denom_exp;
    td_u32 drc_prev_luma[ISP_SYNC_DRC_PRELUMA_NUM];
    td_u32 isp_dgain[ISP_BAYER_CHN_NUM];
    td_u32 wdr_gain[ISP_WDR_CHN_MAX];
    td_u32 wdr_exp_ratio;
    td_u32 flick_exp_ratio;
    td_u32 wdr_exp_val[ISP_WDR_CHN_MAX];
    td_u32 wdr_blc_comp;
    td_bool wdr_mdt_en;
    td_bool fusion_mode;
    td_u16 short_thr;
    td_u16 long_thr;
    td_u16 wgt_slope;  // 1A22
    td_u32 wdr_max_ratio;
    td_u32 fusion_max_ratio;  // 1A22
    td_u32 wb_gain[ISP_BAYER_CHN_NUM];
    td_u16 ccm[EXT_ISP_CCM_MATRIX_SIZE];
    td_u64 ldci_write_stt_addr[ISP_STRIPING_MAX_NUM];
    td_u64 ldci_read_stt_addr[ISP_STRIPING_MAX_NUM];
    isp_be_blc_dyna_cfg be_blc;
    ext_isp_sharpen_reg_cfg sharpen_reg_cfg;
} isp_be_sync_para;

typedef struct {
    td_u32 pixel_count[ISP_WDR_FRAME_MAX];
    td_u32 pixel_weight[ISP_WDR_FRAME_MAX];
    td_u32 histogram_mem_array[ISP_WDR_FRAME_MAX][ISP_HIST_NUM];
} ext_isp_fe_ae_stat_1;

typedef struct {
    td_u16 global_avg_r[ISP_WDR_FRAME_MAX];
    td_u16 global_avg_gr[ISP_WDR_FRAME_MAX];
    td_u16 global_avg_gb[ISP_WDR_FRAME_MAX];
    td_u16 global_avg_b[ISP_WDR_FRAME_MAX];
} ext_isp_fe_ae_stat_2;

typedef struct {
    td_u16 zone_avg[ISP_WDR_FRAME_MAX][ISP_AE_ZONE_ROW][ISP_AE_ZONE_COLUMN][ISP_BAYER_PATTERN_NUM];
} ext_isp_fe_ae_stat_3;

typedef struct {
    td_u32 pixel_count;
    td_u32 pixel_weight;
    td_u32 histogram_mem_array[ISP_HIST_NUM];
} ext_isp_be_ae_stat_1;

typedef struct {
    td_u16 global_avg_r;
    td_u16 global_avg_gr;
    td_u16 global_avg_gb;
    td_u16 global_avg_b;
} ext_isp_be_ae_stat_2;

typedef struct {
    td_u16 zone_avg[ISP_BE_AE_ZONE_ROW][ISP_BE_AE_ZONE_COLUMN][ISP_BAYER_PATTERN_NUM];
} ext_isp_be_ae_stat_3;

typedef struct {
    td_u16 ae_total_max_aver_stt;
    td_u16 ae_total_min_aver_stt;
    td_u16 ae_total_max2nd_aver_stt;
    td_u16 ae_total_min2nd_aver_stt;
    td_u16 ae_total_mid_aver_stt;
    td_u16 ae_highlight_zone_num_stt;
} ext_isp_be_ae_stat_4;

/* the statistics of awb alg */
typedef struct {
    td_u16 metering_awb_avg_r;
    td_u16 metering_awb_avg_g;
    td_u16 metering_awb_avg_b;
    td_u16 metering_awb_count_all;
} ext_isp_awb_stat_1;

typedef struct {
    td_u16 awb_zone_rg[AWB_ZONE_CTE_CNT];
    td_u16 awb_zone_bg[AWB_ZONE_CTE_CNT];
    td_u16 awb_zone_mired[AWB_ZONE_CTE_CNT];
    td_u16 awb_zone_shift[AWB_ZONE_CTE_CNT];
} ext_isp_awb_cte_stat;

/* ISP debug information */
typedef enum {
    EXT_ISP_DEBUG_AE = 0,
    EXT_ISP_DEBUG_AWB,
    EXT_ISP_DEBUG_AF,
    EXT_ISP_DEBUG_BLC,
    EXT_ISP_DEBUG_BNR,
    EXT_ISP_DEBUG_DG,
    EXT_ISP_DEBUG_LSC,
    EXT_ISP_DEBUG_GE,
    EXT_ISP_DEBUG_DEMOSAIC,
    EXT_ISP_DEBUG_FCR,
    EXT_ISP_DEBUG_GAMMA,
    EXT_ISP_DEBUG_CSC,
    EXT_ISP_DEBUG_CA,
    EXT_ISP_DEBUG_MCDS,
    EXT_ISP_DEBUG_CAC,
    EXT_ISP_DEBUG_DPC,
    EXT_ISP_DEBUG_LDCI,
    EXT_ISP_DEBUG_GDCI,
    EXT_ISP_DEBUG_SHARPEN,
    EXT_ISP_DEBUG_BUTT,
} ext_isp_debug_type;

#define EXT_ISP_DEBUG_ALG_BIT(alg) (1U << (alg))

#define EXT_ISP_DEBUG_ALG_MASK_AE       EXT_ISP_DEBUG_ALG_BIT(EXT_ISP_DEBUG_AE)
#define EXT_ISP_DEBUG_ALG_MASK_AWB      EXT_ISP_DEBUG_ALG_BIT(EXT_ISP_DEBUG_AWB)
#define EXT_ISP_DEBUG_ALG_MASK_AF       EXT_ISP_DEBUG_ALG_BIT(EXT_ISP_DEBUG_AF)
#define EXT_ISP_DEBUG_ALG_MASK_BLC      EXT_ISP_DEBUG_ALG_BIT(EXT_ISP_DEBUG_BLC)
#define EXT_ISP_DEBUG_ALG_MASK_BNR      EXT_ISP_DEBUG_ALG_BIT(EXT_ISP_DEBUG_BNR)
#define EXT_ISP_DEBUG_ALG_MASK_DG       EXT_ISP_DEBUG_ALG_BIT(EXT_ISP_DEBUG_DG)
#define EXT_ISP_DEBUG_ALG_MASK_LSC      EXT_ISP_DEBUG_ALG_BIT(EXT_ISP_DEBUG_LSC)
#define EXT_ISP_DEBUG_ALG_MASK_GE       EXT_ISP_DEBUG_ALG_BIT(EXT_ISP_DEBUG_GE)
#define EXT_ISP_DEBUG_ALG_MASK_DEMOSAIC EXT_ISP_DEBUG_ALG_BIT(EXT_ISP_DEBUG_DEMOSAIC)
#define EXT_ISP_DEBUG_ALG_MASK_FCR      EXT_ISP_DEBUG_ALG_BIT(EXT_ISP_DEBUG_FCR)
#define EXT_ISP_DEBUG_ALG_MASK_GAMMA    EXT_ISP_DEBUG_ALG_BIT(EXT_ISP_DEBUG_GAMMA)
#define EXT_ISP_DEBUG_ALG_MASK_CSC      EXT_ISP_DEBUG_ALG_BIT(EXT_ISP_DEBUG_CSC)
#define EXT_ISP_DEBUG_ALG_MASK_CA       EXT_ISP_DEBUG_ALG_BIT(EXT_ISP_DEBUG_CA)
#define EXT_ISP_DEBUG_ALG_MASK_MCDS     EXT_ISP_DEBUG_ALG_BIT(EXT_ISP_DEBUG_MCDS)
#define EXT_ISP_DEBUG_ALG_MASK_CAC      EXT_ISP_DEBUG_ALG_BIT(EXT_ISP_DEBUG_CAC)
#define EXT_ISP_DEBUG_ALG_MASK_DPC      EXT_ISP_DEBUG_ALG_BIT(EXT_ISP_DEBUG_DPC)
#define EXT_ISP_DEBUG_ALG_MASK_LDCI     EXT_ISP_DEBUG_ALG_BIT(EXT_ISP_DEBUG_LDCI)
#define EXT_ISP_DEBUG_ALG_MASK_GDCI     EXT_ISP_DEBUG_ALG_BIT(EXT_ISP_DEBUG_GDCI)
#define EXT_ISP_DEBUG_ALG_MASK_SHARPEN  EXT_ISP_DEBUG_ALG_BIT(EXT_ISP_DEBUG_SHARPEN)

#define EXT_ISP_DEBUG_ALG_MASK_ALL \
    (EXT_ISP_DEBUG_ALG_MASK_AE | EXT_ISP_DEBUG_ALG_MASK_AWB | EXT_ISP_DEBUG_ALG_MASK_AF | \
     EXT_ISP_DEBUG_ALG_MASK_BLC | EXT_ISP_DEBUG_ALG_MASK_BNR | EXT_ISP_DEBUG_ALG_MASK_DG | \
     EXT_ISP_DEBUG_ALG_MASK_LSC | EXT_ISP_DEBUG_ALG_MASK_GE | EXT_ISP_DEBUG_ALG_MASK_DEMOSAIC | \
     EXT_ISP_DEBUG_ALG_MASK_FCR | EXT_ISP_DEBUG_ALG_MASK_GAMMA | EXT_ISP_DEBUG_ALG_MASK_CSC | \
     EXT_ISP_DEBUG_ALG_MASK_CA | EXT_ISP_DEBUG_ALG_MASK_MCDS | EXT_ISP_DEBUG_ALG_MASK_CAC | \
     EXT_ISP_DEBUG_ALG_MASK_DPC | EXT_ISP_DEBUG_ALG_MASK_LDCI | EXT_ISP_DEBUG_ALG_MASK_GDCI | \
     EXT_ISP_DEBUG_ALG_MASK_SHARPEN)

/* ISP debug information */
typedef struct {
    td_bool debug_en;
    td_u32 alg_mask;  /* RW; bitmask of which algorithms to update (1=update, 0=keep current) */
    td_u32 alg_switch; /* RW; bitmask of algorithm debug switch values */
    td_bool awb_log_en;
    td_bool af_log_en;
} ext_isp_debug_info;

/* ISP debug information */
typedef struct {
    ext_isp_debug_type debug_type; /* RW; type of debug info */
    td_bool debug_en;              /* RW; 1:enable debug, 0:disable debug */
    td_u64 phy_addr;               /* RW; physics address of debug info */
    td_u32 depth;                  /* RW; depth of debug info */
} ext_alg_debug_info;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
