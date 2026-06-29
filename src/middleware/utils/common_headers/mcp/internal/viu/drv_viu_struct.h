/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: define for external use
 * Author: AuthorNameMagicTag
 * Create: 2021-09-22
 */

#ifndef __DRV_VIU_STRUCT_H__
#define __DRV_VIU_STRUCT_H__

#include "drv_isp_video_ext.h"
#include "drv_isp_comm_struct.h"
#include "drv_isp_pq_struct.h"
#include "drv_dis_struct.h"
#include "drv_gdc_struct.h"
#include "drv_vb_struct.h"
#include "mcp_arch_define.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define VIU_COMPMASK_NUM                2UL
#define VIU_CMP_PARAM_SIZE              152

// ----------------------------------------------------------------------------------
typedef struct {
    td_bool enable;          /* RW; Low delay enable. */
    td_u32 line_cnt;        /* RW; Range: [32, 16384]; Low delay shoreline. */
} ext_viu_low_delay_info;

/* Information of raw data cmpresss param */
typedef struct {
    td_u8 cmp_param[VIU_CMP_PARAM_SIZE];
} ext_viu_cmp_param;

typedef enum {
    EXT_VIU_USERPIC_MODE_PIC = 0,            /* YUV picture */
    EXT_VIU_USERPIC_MODE_BGC,                /* Background picture only with a color */
    EXT_VIU_USERPIC_MODE_BUTT,
} ext_viu_user_pic_mode;

typedef struct {
    td_u32 bg_color;
} ext_viu_user_pic_bgc;

typedef struct {
    ext_viu_user_pic_mode       usr_pic_mode;  /* User picture mode */
    union {
        ext_isp_video_frame_info   usr_pic_frm;   /* Information about a YUV picture */
        ext_viu_user_pic_bgc    usr_pic_bg;    /* Information about a background picture only with a color */
    } usr_pic;
} ext_viu_user_pic_attr;

/* interface mode of video input */
typedef enum {
    EXT_VIU_MODE_BT656 = 0,              /* ITU-R BT.656 YUV4:2:2 */
    EXT_VIU_MODE_BT656_PACKED_YUV,       /* ITU-R BT.656 packed YUV4:2:2 */
    EXT_VIU_MODE_BT601,                  /* ITU-R BT.601 YUV4:2:2 */
    EXT_VIU_MODE_DIGITAL_CAMERA,         /* digatal camera mode */
    EXT_VIU_MODE_BT1120_STANDARD,        /* BT.1120 progressive mode */
    EXT_VIU_MODE_BT1120_INTERLEAVED,     /* BT.1120 interstage mode */
    EXT_VIU_MODE_MIPI,                   /* MIPI RAW mode */
    EXT_VIU_MODE_MIPI_YUV420_NORMAL,     /* MIPI YUV420 normal mode */
    EXT_VIU_MODE_MIPI_YUV420_LEGACY,     /* MIPI YUV420 legacy mode */
    EXT_VIU_MODE_MIPI_YUV422,            /* MIPI YUV422 mode */
    EXT_VIU_MODE_LVDS,                   /* LVDS mode */
    EXT_VIU_MODE_HISPI,                  /* HiSPi mode */
    EXT_VIU_MODE_SLVS,                   /* SLVS mode */
    EXT_VIU_MODE_MAX
} ext_viu_intf_mode;

/* Input mode */
typedef enum {
    EXT_VIU_INPUT_MODE_BT656 = 0,        /* ITU-R BT.656 YUV4:2:2 */
    EXT_VIU_INPUT_MODE_BT601,            /* ITU-R BT.601 YUV4:2:2 */
    EXT_VIU_INPUT_MODE_DIGITAL_CAMERA,   /* digatal camera mode */
    EXT_VIU_INPUT_MODE_INTERLEAVED,      /* interstage mode */
    EXT_VIU_INPUT_MODE_MIPI,             /* MIPI mode */
    EXT_VIU_INPUT_MODE_LVDS,             /* LVDS mode */
    EXT_VIU_INPUT_MODE_HISPI,            /* HiSPi mode */
    EXT_VIU_INPUT_MODE_SLVS,             /* SLVS mode */
    EXT_VIU_INPUT_MODE_MAX
} ext_viu_input_mode;

/* Work mode */
typedef enum {
    EXT_VIU_WORK_MODE_1Multiplex = 0,    /* 1 Multiplex mode */
    EXT_VIU_WORK_MODE_2Multiplex,        /* 2 Multiplex mode */
    EXT_VIU_WORK_MODE_3Multiplex,        /* 3 Multiplex mode */
    EXT_VIU_WORK_MODE_4Multiplex,        /* 4 Multiplex mode */
    EXT_VIU_WORK_MODE_MAX
} ext_viu_multiplex_mode;

/* whether an input picture is interlaced or progressive */
typedef enum {
    EXT_VIU_SCAN_INTERLACED  = 0,        /* interlaced mode */
    EXT_VIU_SCAN_PROGRESSIVE,            /* progressive mode */
    EXT_VIU_SCAN_MAX
} ext_viu_scan_mode;

/* Sequence of YUV data */
typedef enum {
    EXT_VIU_DATA_SEQ_VUVU = 0,   /* The input sequence of the second component(only contains u and v) in BT.
                                    1120 mode is VUVU */
    EXT_VIU_DATA_SEQ_UVUV,       /* The input sequence of the second component(only contains u and v) in BT.
                                    1120 mode is UVUV */
    EXT_VIU_DATA_SEQ_UYVY,       /* The input sequence of YUV is UYVY */
    EXT_VIU_DATA_SEQ_VYUY,       /* The input sequence of YUV is VYUY */
    EXT_VIU_DATA_SEQ_YUYV,       /* The input sequence of YUV is YUYV */
    EXT_VIU_DATA_SEQ_YVYU,       /* The input sequence of YUV is YVYU */
    EXT_VIU_DATA_SEQ_MAX
} ext_viu_yuv_data_seq;

/* Component mode */
typedef enum {
    EXT_VIU_COMPONENT_MODE_SINGLE = 0,           /* single component mode */
    EXT_VIU_COMPONENT_MODE_DOUBLE,               /* double component mode */
    EXT_VIU_COMPONENT_MODE_MAX
} ext_viu_component_mode;

/* Y/C composite or separation mode */
typedef enum {
    EXT_VIU_COMBINE_COMPOSITE = 0,     /* Composite mode */
    EXT_VIU_COMBINE_SEPARATE,          /* Separate mode */
    EXT_VIU_COMBINE_MAX
} ext_viu_combine_mode;

/* Attribute of the vertical synchronization signal */
typedef enum {
    EXT_VIU_VSYNC_FIELD = 0,  /* Field/toggle mode:a signal reversal means a new frame or a field */
    EXT_VIU_VSYNC_PULSE,      /* Pusle/effective mode:a pusle or an effective signal means a new frame or a field */
    EXT_VIU_VSYNC_MAX
} ext_viu_vsync;

/* Polarity of the vertical synchronization signal */
typedef enum {
    EXT_VIU_VSYNC_NEG_HIGH = 0,    /* if VIU_VSYNC_E = EXT_VIU_VSYNC_FIELD, then the vertical synchronization signal of
                                    even field is high-level,
                                    if VIU_VSYNC_E = EXT_VIU_VSYNC_PULSE,then the vertical synchronization
                                    pulse is positive pulse. */
    EXT_VIU_VSYNC_NEG_LOW,         /* if VIU_VSYNC_E = EXT_VIU_VSYNC_FIELD, then the vertical synchronization signal
                                    of even field is low-level,
                                    if VIU_VSYNC_E = EXT_VIU_VSYNC_PULSE,then the vertical synchronization
                                    pulse is negative pulse. */
    EXT_VIU_VSYNC_NEG_MAX
} ext_viu_vsync_neg;

/* Attribute of the horizontal synchronization signal */
typedef enum {
    EXT_VIU_HSYNC_VALID_SINGNAL = 0,   /* the horizontal synchronization is valid signal mode */
    EXT_VIU_HSYNC_PULSE,               /* the horizontal synchronization is pulse mode, a new pulse means
                                    the beginning of a new line */
    EXT_VIU_HSYNC_MAX
} ext_viu_hsync;

/* Polarity of the horizontal synchronization signal */
typedef enum {
    EXT_VIU_HSYNC_NEG_HIGH = 0,        /* if VIU_HSYNC_E = EXT_VIU_HSYNC_VALID_SINGNAL, then the valid horizontal
                                    synchronization signal is high-level;
                                    if VIU_HSYNC_E = EXT_VIU_HSYNC_PULSE,then the horizontal synchronization
                                    pulse is positive pulse */
    EXT_VIU_HSYNC_NEG_LOW,             /* if VIU_HSYNC_E = EXT_VIU_HSYNC_VALID_SINGNAL, then the valid horizontal
                                    synchronization signal is low-level;
                                    if VIU_HSYNC_E = EXT_VIU_HSYNC_PULSE, then the horizontal synchronization
                                    pulse is negative pulse */
    EXT_VIU_HSYNC_NEG_MAX
} ext_viu_hsync_neg;

/* Attribute of the valid vertical synchronization signal */
typedef enum {
    EXT_VIU_VSYNC_NORM_PULSE = 0, /* the vertical synchronization is pusle mode, a pusle means a new frame or field  */
    EXT_VIU_VSYNC_VALID_SINGAL,   /* the vertical synchronization is effective mode, a effective signal
                                   means a new frame or field */
    EXT_VIU_VSYNC_VALID_MAX
} ext_viu_vsync_valid;

/* Polarity of the valid vertical synchronization signal */
typedef enum {
    EXT_VIU_VSYNC_VALID_NEG_HIGH = 0, /* if VIU_VSYNC_VALID_E=EXT_VIU_VSYNC_NORM_PULSE, a positive pulse means vertical
                                    synchronization pulse;
                                    if VIU_VSYNC_VALID_E = EXT_VIU_VSYNC_VALID_SINGAL, the valid vertical
                                    synchronization signal is high-level */
    EXT_VIU_VSYNC_VALID_NEG_LOW,     /* if VIU_VSYNC_VALID_E = EXT_VIU_VSYNC_NORM_PULSE, a negative pulse
                                        means vertical synchronization pulse;
                                        if VIU_VSYNC_VALID_E = EXT_VIU_VSYNC_VALID_SINGAL, the valid vertical
                                        synchronization signal is low-level */
    EXT_VIU_VSYNC_VALID_NEG_MAX
} ext_viu_vsync_valid_neg;

#define EXT_VIU_BIND_DEST_MAXNUM 64
typedef struct {
    td_u32 num;
    ext_mpp_chn mpp_chn[EXT_VIU_BIND_DEST_MAXNUM];
} ext_mpp_bind_dest;

typedef enum {
    VIU_OFFLINE_PPU_OFFLINE = 0,
    VIU_OFFLINE_PPU_ONLINE,
    VIU_ONLINE_PPU_OFFLINE,
    VIU_ONLINE_PPU_ONLINE
} ext_viu_operating_mode_type;

/* 是否适配到VIU最大的PIPE能力数目？ 比如：ISP_MAX_PIPE_NUM */
#define EXT_VIU_OPERATING_MODE_NUM (16)
typedef struct {
    ext_viu_operating_mode_type mode[EXT_VIU_OPERATING_MODE_NUM];
} ext_viu_operating_mode;

/* Blank information of the input timing */
typedef struct {
    td_u32 hsync_hfb;    /* RW;Horizontal front blanking width */
    td_u32 hsync_act;    /* RW;Horizontal effetive width */
    td_u32 hsync_hbb;    /* RW;Horizontal back blanking width */
    td_u32 vsync_vfb;    /* RW;Vertical front blanking height of one frame or odd-field frame picture */
    td_u32 vsync_vact;   /* RW;Vertical effetive width of one frame or odd-field frame picture */
    td_u32 vsync_vbb;    /* RW;Vertical back blanking height of one frame or odd-field frame picture */
    td_u32 vsync_vbfb;   /* RW;Even-field vertical front blanking height when input mode is interlace
                            (invalid when progressive input mode) */
    td_u32 vsync_vbact;  /* RW;Even-field vertical effetive width when input mode is interlace
                            (invalid when progressive input mode) */
    td_u32 vsync_vbbb;   /* RW;Even-field vertical back blanking height when input mode is interlace
                            (invalid when progressive input mode) */
} ext_viu_timing_blank;

/* synchronization information about the BT.601 or DC timing */
typedef struct {
    ext_viu_vsync              vsync;
    ext_viu_vsync_neg          vsync_neg;
    ext_viu_hsync              hsync;
    ext_viu_hsync_neg          hsync_neg;
    ext_viu_vsync_valid        vsync_valid;
    ext_viu_vsync_valid_neg    vsync_valid_neg;
    ext_viu_timing_blank       timing_blank;
} ext_viu_sync_cfg;

/* Input data type */
typedef enum {
    EXT_VIU_DATA_TYPE_YUV = 0,
    EXT_VIU_DATA_TYPE_RGB,
    EXT_VIU_DATA_TYPE_MAX
} ext_viu_data_type;

/* Attribute of bas type */
typedef enum {
    EXT_VIU_BAS_REPHASE_AUTO = 0,
    EXT_VIU_BAS_REPHASE_MANUAL_OPEN,
    EXT_VIU_BAS_REPHASE_MANUAL_CLOSE,
    EXT_VIU_BAS_REPHASE_MAX
} ext_viu_bas_rephase_mode;

typedef enum {
    EXT_VIU_BAS_GCHN_AUTO = 0,
    EXT_VIU_BAS_GCHN_MANUAL_OPEN,
    EXT_VIU_BAS_GCHN_MANUAL_CLOSE,
    EXT_VIU_BAS_GCHN_MAX
} ext_viu_bas_gchn_mode;

/* Attribute of bas */
typedef struct {
    td_bool enable;
    td_u32 out_width;
    td_u32 out_height;
    ext_isp_bayer_format bayer;
    ext_isp_crop_info bas_crop;

    /* PQT set, default: auto mode */
    ext_viu_bas_rephase_mode rephase;
    ext_viu_bas_gchn_mode gchn;
} ext_viu_bas_attr;

/* Attribute of wdr */
typedef struct {
    ext_isp_wdr_mode  wdr_mode;         /* RW; WDR mode. */
    td_u32            cache_line;       /* RW; WDR cache line. */
} ext_viu_wdr_attr;

/* The attributes of a VI device */
typedef struct {
    ext_viu_intf_mode      intf_mode;                         /* RW;Interface mode */
    ext_viu_multiplex_mode work_mode;                         /* RW;Multiplex mode */
    td_u32                 component_mask[VIU_COMPMASK_NUM];  /* RW;Component mask */
    ext_viu_scan_mode      scan_mode;                         /* RW;Input scanning mode (progressive or interlaced) */
    /* The below members must be configured in BT.601 mode or DC mode and are invalid in other modes */
    ext_viu_yuv_data_seq   data_seq;    /* RW;Input data sequence (only the YUV format is supported) */
    ext_viu_sync_cfg       syn_cfg;     /* RW;Sync timing. This member must be configured in BT.601 mode or DC mode */

    ext_viu_data_type      input_data_type;        /* RW;RGB: CSC-709 or CSC-601, PT YUV444 disable; YUV: default
                                                        yuv CSC coef PT YUV444 enable. */
    td_bool                data_reverse;           /* RW;Data reverse */
    ext_isp_size           size;                   /* RW;Input size */
    ext_viu_bas_attr       bas_attr;               /* RW;Attribute of BAS */
    ext_viu_wdr_attr       wdr_attr;               /* RW;Attribute of WDR */
    ext_isp_data_rate      data_rate;              /* RW;Data rate of device */
} ext_viu_dev_attr;

/* Information of pipe binded to device */
typedef struct {
    td_u32 num;                                     /* RW;Range [1,ISP_MAX_PHY_PIPE_NUM] */
    td_s32 pipe_id[ISP_MAX_PHY_PIPE_NUM];           /* RW;Array of pipe ID */
} ext_viu_dev_bind_pipe;

/* Source of 3DNR reference frame */
typedef enum {
    EXT_VIU_NR_REF_FROM_RFR = 0,                             /* Reference frame from reconstruction frame */
    EXT_VIU_NR_REF_FROM_CHN0,                                /* Reference frame from CHN0's frame */
    EXT_VIU_NR_REF_FROM_MAX
} ext_viu_nr_ref_source;

typedef enum {
    EXT_VIU_PIPE_BYPASS_NONE,
    EXT_VIU_PIPE_BYPASS_FE,
    EXT_VIU_PIPE_BYPASS_BE,
    EXT_VIU_PIPE_BYPASS_MAX
} ext_viu_pipe_bypass_mode;

/* The attributes of 3DNR */
typedef struct {
    ext_isp_pixel_format          pix_fmt;                       /* RW;Pixel format of reference frame */
    ext_isp_data_bitwidth         bit_width;                     /* RW;Bit width of reference frame */
    ext_viu_nr_ref_source         nr_ref_source;                 /* RW;Source of 3DNR reference frame */
    ext_isp_compress_mode         compress_mode;                 /* RW;Reference frame compress mode */
} ext_viu_nr_attr;

/* The attributes of zsl */
typedef struct {
    td_bool             zsl_enable;
    ext_vb_source       vb_source;
    td_u32              vb_pool_id;
    ext_isp_crop_info   zsl_crop;
    td_u32              buf_cnt;
} ext_viu_zsl_attr;

typedef enum {
    EXT_VIU_PIPE_FRAME_SOURCE_DEV = 0, /* RW;Source from dev */
    EXT_VIU_PIPE_FRAME_SOURCE_USER_FE, /* RW;User send to FE */
    EXT_VIU_PIPE_FRAME_SOURCE_USER_BE, /* RW;User send to BE */
    EXT_VIU_PIPE_FRAME_SOURCE_MAX
} ext_viu_pipe_frame_source;

typedef enum {
    EXT_VIU_PIPE_REPEAT_NONE = 0,
    EXT_VIU_PIPE_REPEAT_ONCE = 1,
    EXT_VIU_PIPE_REPEAT_MAX
} ext_viu_pipe_repeat_mode;

/* The attributes of pipe */
typedef struct {
    td_u32                      sns_width;        /* Sensor Out Width */
    td_u32                      sns_height;       /* Sensor Out Height */
    td_u32                      max_w;            /* Vicap Out width */
    td_u32                      max_h;            /* Vicap Out height */

    ext_viu_pipe_bypass_mode    pipe_bypass_mode;
    td_bool                     yuv_skip;         /* YUV skip enable */
    td_bool                     isp_bypass;       /* Range:[0, 1];ISP bypass enable */
    ext_isp_pixel_format        pix_fmt;          /* Pixel format */
    ext_isp_compress_mode       compress_mode;    /* Compress mode. */
    ext_isp_data_bitwidth       bit_width;        /* Bit width */
    ext_isp_frame_rate_ctrl     frame_rate;       /* Out Frame rate Ctrl */
    ext_viu_pipe_frame_source   frame_source;
    ext_isp_bayer_format        bayer_format;
    ext_isp_crop_info           crop_info;

    td_bool                     nr_en;            /* 3DNR enable */
    ext_viu_nr_attr             nr_attr;          /* Attribute of 3DNR */
    td_u32                      vc_number;        /* WDR num_id */
    ext_viu_zsl_attr            zsl_attr;
} ext_viu_pipe_attr;

typedef enum {
    EXT_VIU_CROP_RATIO_COOR = 0,             /* Ratio coordinate */
    EXT_VIU_CROP_ABS_COOR,                   /* Absolute coordinate */
    EXT_VIU_CROP_MAX
} ext_viu_crop_coordinate;

/* information of chn crop */
typedef struct {
    td_bool                  enable;            /* RW;CROP enable */
    ext_viu_crop_coordinate  crop_coordinate;   /* RW;coordinate mode of the crop start point */
    ext_isp_rect             crop_rect;         /* RW;CROP rectangular */
} ext_viu_crop_info;

typedef struct {
    ext_viu_bas_attr            bas_attr;
    ext_isp_crop_info           vicap_crop;

    /* 动态变分辨率，传递给viproc的帧级变化参数 */
    ext_isp_size                viproc_input;
    ext_viu_crop_info           viproc_crop;
    ext_isp_rotation            rotation;       /* chn_attr + ppu_attr rotation */
    td_bool                     snap_en;
    td_u32                      snap_num;

    /* 数字变焦，传递给ppu的帧级变化参数 */
    td_bool                     dynamic_attr_en;
    td_s32                      ppu_chn_id;
    ext_isp_crop_info           ppu_crop;
    ext_isp_size                ppu_zme_out;
} ext_viu_dynamic_attr;

/* the attributes of channel */
typedef struct {
    ext_isp_size              size;             /* RW;channel out put size */
    ext_isp_pixel_format      pixel_format;     /* RW;pixel format */
    ext_isp_dynamic_range     dynamic_range;    /* RW;dynamic range */
    ext_isp_video_format      video_format;     /* RW;video format */
    ext_isp_compress_mode     compress_mode;    /* RW;256B segment compress or no compress. */
    td_bool                   mirror;           /* RW;mirror enable */
    td_bool                   flip;             /* RW;flip enable */
    td_u32                    depth;            /* RW;range [0,8];depth */
    ext_isp_frame_rate_ctrl   frame_rate;       /* RW;frame rate */
    ext_viu_crop_info         crop_info;
    ext_isp_rotation          rotation;
    ext_vb_source             vb_source;
    vb_pool                   vb_pool_id;
} ext_viu_chn_attr;

/* the status of pipe */
typedef struct {
    td_bool enable;                       /* RO;whether this pipe is enabled */
    td_u32  int_cnt;                      /* RO;the video frame interrupt count */
    td_u32  frame_rate;                   /* RO;current frame rate */
    td_u32  lost_frame;                   /* RO;lost frame count */
    td_u32  vb_fail;                      /* RO;video buffer malloc failure */
    ext_isp_size size;                    /* RO;current pipe output size */
} ext_viu_pipe_status;

typedef enum {
    EXT_VIU_EXT_CHN_SOURCE_TAIL,
    EXT_VIU_EXT_CHN_SOURCE_HEAD,
    EXT_VIU_EXT_CHN_SOURCE_MAX
} ext_viu_ext_chn_source;

typedef struct {
    ext_viu_ext_chn_source   source;
    td_s32                   bind_chn;      /* RW;the channel num which extend channel will bind to */
    ext_isp_size             size;          /* RW;channel out put size */
    ext_isp_pixel_format     pix_format;    /* RW;pixel format */
    ext_isp_dynamic_range    dynamic_range; /* RW;dynamic range */
    ext_isp_compress_mode    compress_mode; /* RW;256B segment compress or no compress. */
    td_u32                   depth;         /* RW;range [0,8];depth */
    ext_isp_frame_rate_ctrl  frame_rate;    /* RW;frame rate */
} ext_viu_ext_chn_attr;

/* the attributes of LDC */
typedef struct {
    td_bool enable;
    ext_isp_ldc_attr attr;
    ext_isp_size pic_size;
} ext_viu_ldc_attr;

/* the status of chn */
typedef struct {
    td_bool enable;                    /* RO;whether this channel is enabled */
    td_u32  frame_rate;                /* RO;current frame rate */
    td_u32  lost_frame;                /* RO;lost frame count */
    td_u32  vb_fail;                   /* RO;video buffer malloc failure */
    ext_isp_size size;                   /* RO;chn output size */
} ext_viu_chn_status;

typedef enum {
    EXT_VIU_DUMP_TYPE_RAW = 0,
    EXT_VIU_DUMP_TYPE_YUV = 1,
    EXT_VIU_DUMP_TYPE_IR = 2,
    EXT_VIU_DUMP_TYPE_MAX
} ext_viu_dump_type;

typedef struct {
    td_bool         enable;              /* RW;whether dump is enable */
    td_u32          depth;               /* RW;range [0,8];depth */
    ext_viu_dump_type dump_type;
} ext_viu_dump_attr;

typedef enum {
    EXT_VIU_PIPE_FRAME_TYPE_RAW = 0, /* RW;User send raw to BE */
    EXT_VIU_PIPE_FRAME_TYPE_YUV,     /* RW;User send yuv to BE */
    EXT_VIU_PIPE_FRAME_TYPE_RGB,     /* RW;User send rgb to BE */
    EXT_VIU_PIPE_FRAME_TYPE_MAX
} ext_viu_pipe_frame_type;

/* module params */
typedef struct {
    td_s32       detect_err_frame;
    td_u32       drop_err_frame;
} ext_viu_mod_param;

typedef struct {
    td_bool enable;               /* RW;range:[0,1];whether enable VI generate timing */
    td_u32  frm_rate;             /* RW;range:(0,0xffffff];;generate timing frame rate */
} ext_viu_dev_timing_attr;

typedef struct {
    td_bool enable;
    td_u32 line_cnt;
} ext_viu_early_interrupt;

typedef ext_pq_grp_nrx_param ext_viu_pipe_nrx_param;
typedef ext_pq_nrx_v2 ext_viu_pipe_nrx_param_v2;
typedef ext_pq_nrx_param_manual_v2 ext_viu_nrx_param_manual_v2;
typedef ext_pq_nrx_param_auto_v2 ext_viu_nrx_param_auto_v2;
typedef ext_pq_nrx_param_v2 ext_viu_nrx_param_v2;
typedef ext_pq_nr_ver ext_viu_nr_version;

typedef struct {
    td_s32 viu_dev_id;
    ext_isp_wdr_mode wdr_mode;
    ext_viu_dev_bind_pipe dev_bind_pipe;
    td_bool mast_pipe;
} ext_viu_pipe_wdr_attr;

typedef struct {
    ext_isp_dynamic_range dynamic_range;
    ext_isp_pixel_format format;
    td_bool mirror_en;
} ext_viu_pipe_hdr_attr;

typedef struct {
    ext_isp_rotation rotation;
    td_bool flip_en;
    td_bool mirror_en;
} ext_viu_pipe_transform_attr;

/* Defines the ISP stitch attribute. */
typedef struct {
    td_bool stitch_enable;
    td_bool main_pipe;
    td_u8 stitch_pipe_num;
    td_s8 stitch_bind_id[ISP_MAX_PIPE_NUM];
} ext_viu_stitch_attr;

typedef enum {
    EXT_SNAP_STATE_NULL = 0,
    EXT_SNAP_STATE_CFG = 1,
    EXT_SNAP_STATE_BUTT
} ext_snap_isp_state;

typedef struct {
    ext_isp_operation_mode operation_mode;
    ext_isp_snap_pro_auto_param auto_param;
    ext_isp_snap_pro_manual_param manual_param;
} ext_snap_pro_param;

typedef struct {
    td_u32  frame_cnt;         /* RW;Set capture frame counts */
    td_u32  repeat_send_times;  /* [0, 2]; When FE-BE is offline, the first raw frame that is triggered should be sent
                                                    repeatedly to the BE. */

    /* ZSL struct */
    td_bool zsl;           /* RW;statical attrs */
    td_u32  frame_depth;  /* buffer depth: [1,8] */
    td_u32  rollback_ms;  /* RW;Rollback time, unit(ms), invalid when zsl is TD_FALSE  */
    td_u32  interval;    /* RW;For continuous capture, select frame every intercal frames */
} ext_snap_normal_attr;

typedef struct {
    td_u32  frame_cnt;          /* [1, PRO_MAX_FRAME_NUM] */
    td_u32  repeat_send_times;   /* [0, 2];When FE-BE is offline, the first raw frame that is triggered should be sent
                                                    repeatedly to the BE. */
    ext_snap_pro_param pro_param;
} ext_snap_pro_attr;

typedef struct {
    ext_isp_snap_type snap_type;
    td_bool load_ccm;    /* TD_TRUE:use CCM of snap_isp_info, TD_FALSE: Algorithm calculate */
    union {
        ext_snap_normal_attr     normal_attr;
        ext_snap_pro_attr        pro_attr;
    };
} ext_snap_attr;

typedef struct {
    ext_snap_isp_state snap_state;
    td_u32 pro_index;
    ext_isp_config_info isp_cfg_info;
} ext_isp_snap_info;

typedef struct {
    ext_viu_ldc_attr ldc_attr;
    ext_gdc_firmware_attr gdc_firm_attr;
} viu_gdc_ldc_attr;

typedef struct {
    ext_isp_rotation rotation;
    ext_gdc_fw_pmf_param fw_pmf_param;
    ext_gdc_firmware_attr gdc_firm_attr;
} viu_gdc_rotation_attr;

typedef struct {
    ext_gdc_spread_attr spread_attr;
    ext_gdc_fw_pmf_param fw_pmf_param;
    ext_gdc_firmware_attr gdc_firm_attr;
} viu_gdc_spread_attr;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* End of __DRV_VIU_STRUCT_H__ */
