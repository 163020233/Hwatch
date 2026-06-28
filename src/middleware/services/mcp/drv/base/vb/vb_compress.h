/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
* Description:vb queue header file
* Author: AuthorNameMagicTag
* Create: 2025-4-28
*/

#ifndef __VB_COMPRESS_H__
#define __VB_COMPRESS_H__

#include "drv_mcp_dependency.h"
#include "drv_vb_ext.h"

typedef enum {
    OPERATION_MODE_AUTO   = 0,
    OPERATION_MODE_MANUAL = 1,
    OPERATION_MODE_MAX
} operation_mode;

typedef enum {
    COMPRESS_LOSSLESS = 0,
    COMPRESS_LOSSY,
    COMPRESS_MAX
} compress_strategy;

typedef struct {
    compress_strategy comp_strategy; /* RW, compress strategy */
    td_s32 type;
    td_u32 comp_ratio_8_bit;  /* RW, range[0, 100]; lossy compression ratio of 8 bit */
    td_u32 comp_ratio_10_bit; /* RW, range[0, 100]; lossy compression ratio of 10 bit */
    td_u32 comp_ratio_12_bit; /* RW, range[0, 100]; lossy compression ratio of 12 bit */
    td_u32 comp_ratio_14_bit; /* RW, range[0, 100]; lossy compression ratio of 14 bit */
    td_u32 comp_ratio_16_bit; /* RW, range[0, 100]; lossy compression ratio of 16 bit */
} raw_compress_param;

typedef struct {
    compress_strategy comp_strategy; /* RW, compress strategy */
    td_s32 type;
    td_u32 comp_ratio_8_bit;  /* RW, range[0, 100]; lossy compression ratio of 8 bit */
    td_u32 comp_ratio_10_bit; /* RW, range[0, 100]; lossy compression ratio of 10 bit */
} compress_parameter;

typedef struct {
    compress_parameter luma_comp_param;
    compress_parameter chroma_comp_param;
} yuv_compress_param;

typedef struct {
    operation_mode operation_mode;
    raw_compress_param raw_line_comp_param;
    raw_compress_param raw_frame_comp_param;
    yuv_compress_param viu_comp_param;
    yuv_compress_param ppu_comp_param;
    yuv_compress_param vgs_comp_param;
    yuv_compress_param gdc_comp_param;
    yuv_compress_param viu_tile_comp_param;
    yuv_compress_param ppu_tile_comp_param;
    yuv_compress_param nr_comp_param;
} vb_compress_v2_param;

typedef struct {
    int cmp_mode;  /* 1: is raw frame compress mode; 0: is not ... */
    int is_lossless;  /* 1: is lossless compress; 0: is lossy compress */
    int sample_mode;  /* 1:422;2:420 */

    unsigned int frame_width;  /* input frame width */
    unsigned int frame_height;  /* input frame height */
    int dw_mode;  /* pixel bit depth, 0: 10 bit; 1: 8 bit */

    int part_cmp_en;  /* 0: compress total frame; 1: compress partial frame */
    int pcmp_start_hpos_y;  /* luma start horizontal position of partial cmp */
    int pcmp_end_hpos_y;  /* luma end horizontal position of partial cmp */
    int pcmp_start_hpos_c;  /* chroma start horizontal position of partial cmp */
    int pcmp_end_hpos_c;  /* chroma end horizontal position of partial cmp */

    int comp_ratio_y_int;  /* luma compress ratio, (=comp_ratio_y * 1000) */
    int comp_ratio_c_int;  /* chroma compress ratio, (=comp_ratio_c * 1000) */

    int rm_dbg_en;  /* RM debug enable control   0: unable; 1: enable */
    int rc_type_mode;  /* 0: default    1: random; 2: max value; 3: min value */
    td_u32 bit_depth;           /* pixel bit depth; 0: 8 bit, 1: 10 bit */
    td_u32 chroma_en;           /* 0--luma, 1--chroma */
    td_u32 pix_format;          /* 0: 420, 1: 422 */
} cmp_3dnr_input;

typedef struct {
    int frame_width;  /* input frame width */
    int frame_height;  /* input frame height */
    int pix_format;  /* YUV420-- 0; YUV422-- 1 */
    int cmp_mode;  /* 0:cmp 1:bypass 2:esl_mode */
    int esl_qp;
    int is_lossless;  /* 1: is lossless compress; 0: is lossy compress */
    int tile_en;
    int tile_height;
    int mirror_en;
    int chroma_en;
    int comp_ratio_int;  /* luma&chroma compression ratio, (=comp_ratio * 1000) */
    int rc_type_mode;  /* 0: default; 1: random; 2: max value; 3: min value; 4: max/min combination */
} cmp_seg_256x1_input;

/* 压缩率=压缩后/压缩前,压缩比=压缩前/压缩后,压缩算法使用的都是压缩比 */
typedef struct {
    unsigned int frame_width;  /* input frame width */
    unsigned int frame_height;  /* input frame height */
    unsigned int frame_bitdepth;  /* pixel bit depth, 1: 10 bit; 2: 12 bit; 3:14bit */
    unsigned int cmp_mode;  /* 0 -- cmp; 1 -- bypass */
    unsigned int is_lossless;  /* 1: is lossless compress; 0: is lossy compress */
    unsigned int bayer_mode;  /* bayer mode  0:RGGB; 1:GRBG; 2:GBRG; 3:BGGR */
    unsigned int stride_en;  /* 0 -- frame; 1 -- line */
    unsigned int align_mode;  /* 0-- 1burst align; 1-- 2burst align */
    unsigned int comp_ratio_int;  /* luma&chroma compression ratio, (=comp_ratio * 1000) */
    unsigned int rc_type_mode;  /* 0: default; 1: random; 2: max value; 3: min value; 4: max/min combination */
} cmp_bayer_input;

typedef struct {
    td_u32 comp_ratio_10_bit; /* compression ratio of 10 bit */
    td_u32 comp_ratio_12_bit; /* compression ratio of 12 bit */
    td_u32 comp_ratio_14_bit; /* compression ratio of 14 bit */
} vb_raw_frame_compress_param;

td_s32 vb_compress_drv_get_cmp_seg_cfg(ext_vb_global_cmp_input *vb_cmp_in, ext_vb_cmp_para_output *y_cmp_cfg,
    ext_vb_cmp_para_output *c_cmp_cfg);

td_s32 vb_compress_drv_get_cmp_bayer_cfg(ext_vb_global_cmp_input *vb_cmp_in,
    ext_vb_cmp_para_output *bayer_cmp_cfg);

td_s32 vb_compress_drv_get_cmp_3dnr_cfg(ext_vb_global_cmp_input *vb_cmp_in, ext_vb_cmp_para_output *y_cmp_cfg,
    ext_vb_cmp_para_output *c_cmp_cfg);

td_void vb_compress_drv_get_cmp_tile_cfg(ext_vb_tile32x4_cmp_input *para_input, ext_vb_cmp_tile32x4_output *rm_cfg);

td_s32 vb_compress_get_vb_cfg(ext_vb_base_info *info, ext_isp_vb_cal_config *cfg);

td_s32 vb_compress_drv_init(td_void);
td_void vb_compress_drv_exit(td_void);

#ifdef CONFIG_MCP_UT
td_void cmp_generate_cfg_seg128x1(const cmp_seg_256x1_input *input, ext_vb_cmp_seg_256x1_output *output, td_bool cmp_mode);
td_void cmp_generate_cfg_bayer(const cmp_bayer_input *input, ext_vb_cmp_bayer_output *output, td_bool cmp_mode);
td_void cmp_generate_cfg_3dnr(const cmp_3dnr_input *input, ext_vb_cmp_3dnr_output *output, td_bool cmp_mode);
#endif

#endif


