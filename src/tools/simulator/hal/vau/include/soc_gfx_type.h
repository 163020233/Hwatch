 /*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2021. All rights reserved.
 * Description: soc gfx type
 */

#ifndef SOC_GFX_TYPE_H
#define SOC_GFX_TYPE_H

#include "td_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @defgroup gfx_type gfx_type
 * @ingroup  gpu_type
 * @{
 */

/**
 * @if Eng
 * @brief  Pixel Format.
 * @else
 * @brief  像素格式
 * @endif
 */
typedef enum {
    EXT_GFX_FMT_ARGB8888    = 0x00,
    EXT_GFX_FMT_KRGB8888    = 0x01,
    EXT_GFX_FMT_ARGB4444    = 0x02,
    EXT_GFX_FMT_ARGB1555    = 0x03,
    EXT_GFX_FMT_ARGB8565    = 0x04,
    EXT_GFX_FMT_RGB888      = 0x05,
    EXT_GFX_FMT_RGB444      = 0x06,
    EXT_GFX_FMT_RGB555      = 0x07,
    EXT_GFX_FMT_RGB565      = 0x08,
    EXT_GFX_FMT_A1          = 0x09,
    EXT_GFX_FMT_A8          = 0x0A,
    EXT_GFX_FMT_A1B         = 0x0D,
    EXT_GFX_FMT_CLUT1       = 0x10,
    EXT_GFX_FMT_CLUT2       = 0x11,
    EXT_GFX_FMT_CLUT4       = 0x12,
    EXT_GFX_FMT_CLUT8       = 0x13,
    EXT_GFX_FMT_ACLUT44     = 0x14,
    EXT_GFX_FMT_ACLUT88     = 0x15,
    EXT_GFX_FMT_CLUT1B      = 0x16,
    EXT_GFX_FMT_CLUT2B      = 0x17,
    EXT_GFX_FMT_CLUT4B      = 0x18,
    EXT_GFX_FMT_A2          = 0x19,
    EXT_GFX_FMT_A4          = 0x1A,
    EXT_GFX_FMT_A2B         = 0x1B,
    EXT_GFX_FMT_A4B         = 0x1C,
    EXT_GFX_FMT_YUV400SP    = 0x30,
    EXT_GFX_FMT_YUV422SP    = 0x31,
    EXT_GFX_FMT_YUV420SP    = 0x33,
    EXT_GFX_FMT_YUV444SP    = 0x34,
    EXT_GFX_FMT_R8          = 0x35,
    EXT_GFX_FMT_G8          = 0x36,
    EXT_GFX_FMT_B8          = 0x37,
    EXT_GFX_FMT_RGB332      = 0x38,
    EXT_GFX_FMT_ARGB2101010 = 0x39,
    EXT_GFX_FMT_MAX
} ext_gfx_fmt;

/**
 * @if Eng
 * @brief  ARGB Pixel Format Component Layout.
 * @else
 * @brief  ARGB分量排布
 * @endif
 */
typedef enum {
    EXT_GFX_FMT_ORDER_ARGB = 0x00,
    EXT_GFX_FMT_ORDER_ARBG = 0x01,
    EXT_GFX_FMT_ORDER_AGRB = 0x02,
    EXT_GFX_FMT_ORDER_AGBR = 0x03,
    EXT_GFX_FMT_ORDER_ABRG = 0x04,
    EXT_GFX_FMT_ORDER_ABGR = 0x05,

    EXT_GFX_FMT_ORDER_RAGB = 0x06,
    EXT_GFX_FMT_ORDER_RABG = 0x07,
    EXT_GFX_FMT_ORDER_RGAB = 0x08,
    EXT_GFX_FMT_ORDER_RGBA = 0x09,
    EXT_GFX_FMT_ORDER_RBAG = 0x0a,
    EXT_GFX_FMT_ORDER_RBGA = 0x0b,

    EXT_GFX_FMT_ORDER_GRAB = 0x0c,
    EXT_GFX_FMT_ORDER_GRBA = 0x0d,
    EXT_GFX_FMT_ORDER_GARB = 0x0e,
    EXT_GFX_FMT_ORDER_GABR = 0x0f,
    EXT_GFX_FMT_ORDER_GBRA = 0x10,
    EXT_GFX_FMT_ORDER_GBAR = 0x11,

    EXT_GFX_FMT_ORDER_BRGA = 0x12,
    EXT_GFX_FMT_ORDER_BRAG = 0x13,
    EXT_GFX_FMT_ORDER_BGRA = 0x14,
    EXT_GFX_FMT_ORDER_BGAR = 0x15,
    EXT_GFX_FMT_ORDER_BARG = 0x16,
    EXT_GFX_FMT_ORDER_BAGR = 0x17,
    EXT_GFX_FMT_ORDER_MAX
} ext_gfx_argb_order;

/**
 * @if Eng
 * @brief  Compression mode.
 * @else
 * @brief  压缩模式
 * @endif
 */
typedef enum {
    EXT_GFX_COMPRESS_NONE    = 0x0,
    EXT_GFX_COMPRESS_HIMC    = 0x1,
    EXT_GFX_COMPRESS_HFBC_2X = 0x2,
    EXT_GFX_COMPRESS_HFBC_3X = 0x3,
    EXT_GFX_COMPRESS_MAX,
#if TARGET_CHIP_BRANDY
    EXT_GFX_COMPRESS_HFBC         = 0x1,
    EXT_GFX_COMPRESS_HFBC_ABYPASS = 0x2,
#endif
} ext_gfx_compress_mode;

// =============== HDR SUPPORT ============== //
#define GFX_YUV_COMPONENTS 3

typedef enum {
    EXT_GFX_COLOR_PRIMARIES_BT709      = 0x1,
    EXT_GFX_COLOR_PRIMARIES_BT601      = 0x5,
    EXT_GFX_COLOR_PRIMARIES_BT2020     = 0x9,
    EXT_GFX_COLOR_PRIMARIES_MAX
} ext_gfx_color_primaries;

typedef enum {
    EXT_GFX_TRANS_CHARAC_GAMMA2_2_TYPE = 0xd,
    EXT_GFX_TRANS_CHARAC_PQ_TYPE       = 0x10,
    EXT_GFX_TRANS_CHARAC_HLG_TYPE      = 0x12,
    EXT_GFX_TRANS_CHARAC_MAX
} ext_gfx_transfer_characteristics;
 
typedef enum {
    EXT_GFX_MATRIX_COEFFICIENTS_BT709  = 0x1,
    EXT_GFX_MATRIX_COEFFICIENTS_BT601  = 0x5,
    EXT_GFX_MATRIX_COEFFICIENTS_BT2020 = 0x9,
    EXT_GFX_MATRIX_COEFFICIENTS_MAX
} ext_gfx_matrix_coefficients;

typedef struct {
    ext_gfx_color_primaries color_primaries;
    ext_gfx_transfer_characteristics transfer_characteristics;
    ext_gfx_matrix_coefficients matrix_coefficients;
} ext_gfx_image_cicp;

typedef struct {
    td_float base_headroom;
    td_float alternate_headroom;
    td_float min_gainmap[GFX_YUV_COMPONENTS];
    td_float max_gainmap[GFX_YUV_COMPONENTS];
    td_float gamma_values[GFX_YUV_COMPONENTS];
    td_float baseline_image_offset[GFX_YUV_COMPONENTS];
    td_float alternate_image_offset[GFX_YUV_COMPONENTS];
} ext_gfx_extend_info;

typedef struct {
    td_u8 image_cnt;
    td_bool is_base_image;
    td_bool is_nonstandard;
    ext_gfx_image_cicp cicp_info;
    ext_gfx_extend_info extend_info;
} ext_gfx_hdr_attr;

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* SOC_GFX_TYPE_H */
