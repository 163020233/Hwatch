/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2024. All rights reserved.
 * Description: zme alg file
 */

#include "vau_zme_alg.h"
#include <stdio.h>
#include <string.h>
#include <securec.h>
#include "drv_vau_type.h"
#include "vau_dfx.h"
#include "gpu_mem_config.h"

#define COEF_BUTT         17
#define HSTEP_LSHFT       20
#define VSTEP_LSHFT       12
#define H_COLUMN          8
#define V_COLUMN          4
#define ZOOM_COEF_ROW     9
#define ZOOM_COEF_H_NUM   51
#define ZOOM_COEF_V_NUM   34
#define ZME_COEF_MAX_LEN  0x100

static td_u32 g_vau_zme_yh_coef[ZOOM_COEF_H_NUM];
static td_u32 g_vau_zme_uvh_coef[ZOOM_COEF_H_NUM];
static td_u32 g_vau_zme_yv_coef[ZOOM_COEF_V_NUM];
static td_u32 g_vau_zme_uvv_coef[ZOOM_COEF_V_NUM];

typedef enum {
    Y_HORZ_TYPE  = 0,
    Y_VERT_TYPE  = 1,
    UV_HORZ_TYPE = 2,
    UV_VERT_TYPE = 3,
} drv_gfx_zme_scale_type;

typedef struct {
    td_u32 y_h_ratio;
    td_u32 y_v_ratio;
    td_u32 uv_h_ratio;
    td_u32 uv_v_ratio;
} vau_zme_ratio;

typedef struct {
    td_u32 y_wight_in;
    td_u32 y_height_in;
    td_u32 uv_wight_in;
    td_u32 uv_height_in;
    td_u32 y_wight_out;
    td_u32 y_height_out;
    td_u32 uv_wight_out;
    td_u32 uv_height_out;
} vau_yuv_width_height;

typedef struct {
    td_s16 *y_h_scale_coef;
    td_s16 *y_v_scale_coef;
    td_s16 *uv_h_scale_coef;
    td_s16 *uv_v_scale_coef;
} vau_zme_coef;

static td_s16 g_coef6_6M_h[COEF_BUTT][H_COLUMN] = {
    { 0,  -4,    6,   58,    6,   -4,    2,  0 },
    { 0,  -3,    5,   57,    8,   -5,    2,  0 },
    { 0,  -3,    3,   57,   10,   -5,    2,  0 },
    { 0,  -2,    1,   57,   12,   -6,    2,  0 },
    { 0,  -2,    0,   56,   14,   -6,    2,  0 },
    { 0,  -1,   -1,   54,   15,   -6,    3,  0 },
    { 0,  -1,   -2,   54,   17,   -7,    3,  0 },
    { 0,   0,   -4,   52,   20,   -7,    3,  0 },
    { 0,   0,   -5,   52,   22,   -8,    3,  0 },
    { 0,   0,   -5,   50,   24,   -8,    3,  0 },
    { 0,   1,   -6,   48,   26,   -8,    3,  0 },
    { 0,   1,   -7,   48,   28,   -9,    3,  0 },
    { 0,   1,   -7,   46,   30,   -9,    3,  0 },
    { 0,   2,   -8,   44,   32,   -9,    3,  0 },
    { 0,   2,   -8,   42,   34,   -9,    3,  0 },
    { 0,   2,   -9,   40,   37,   -9,    3,  0 },
    { 0,   2,   -9,   39,   39,   -9,    2,  0 }
};

static td_s16 g_coef6_5M_h[COEF_BUTT][H_COLUMN] = {
    { 0,  -6,   13,   49,   13,   -6,    1,  0 },
    { 0,  -6,   12,   48,   15,   -6,    1,  0 },
    { 0,  -6,   11,   49,   16,   -7,    1,  0 },
    { 0,  -6,    9,   49,   18,   -7,    1,  0 },
    { 0,  -5,    8,   48,   19,   -7,    1,  0 },
    { 0,  -5,    7,   47,   21,   -7,    1,  0 },
    { 0,  -5,    5,   47,   23,   -7,    1,  0 },
    { 0,  -5,    4,   47,   24,   -7,    1,  0 },
    { 0,  -4,    3,   46,   26,   -7,    0,  0 },
    { 0,  -4,    2,   45,   27,   -6,    0,  0 },
    { 0,  -4,    1,   44,   29,   -6,    0,  0 },
    { 0,  -3,    0,   43,   30,   -6,    0,  0 },
    { 0,  -3,   -1,   43,   32,   -6,   -1,  0 },
    { 0,  -3,   -2,   41,   34,   -5,   -1,  0 },
    { 0,  -2,   -2,   40,   34,   -5,   -1,  0 },
    { 0,  -2,   -3,   38,   36,   -4,   -1,  0 },
    { 0,  -2,   -4,   38,   38,   -4,   -2,  0 }
};

static td_s16 g_coef6_2_5M_h[COEF_BUTT][H_COLUMN] = {
    { 0,   4,   16,   25,   16,    4,   -1,  0 },
    { 0,   4,   16,   23,   17,    5,   -1,  0 },
    { 0,   4,   16,   23,   17,    5,   -1,  0 },
    { 0,   4,   15,   23,   18,    5,   -1,  0 },
    { 0,   3,   15,   23,   18,    6,   -1,  0 },
    { 0,   3,   14,   23,   18,    6,    0,  0 },
    { 0,   3,   14,   23,   18,    6,    0,  0 },
    { 0,   3,   13,   23,   18,    7,    0,  0 },
    { 0,   2,   13,   23,   19,    7,    0,  0 },
    { 0,   2,   13,   23,   19,    7,    0,  0 },
    { 0,   2,   12,   22,   20,    8,    0,  0 },
    { 0,   2,   12,   22,   20,    8,    0,  0 },
    { 0,   2,   12,   22,   20,    8,    0,  0 },
    { 0,   1,   11,   22,   21,    9,    0,  0 },
    { 0,   1,   11,   21,   21,    9,    1,  0 },
    { 0,   1,   10,   21,   21,   10,    1,  0 },
    { 0,   1,   10,   21,   21,   10,    1,  0 }
};

static td_s16 g_coef6_area_h[COEF_BUTT][H_COLUMN] = {
    { 0, 0,    0,   32,    32,    0,    0,  0 },
    { 0, 0,    0,   32,    32,    0,    0,  0 },
    { 0, 0,    0,   32,    32,    0,    0,  0 },
    { 0, 0,    0,   32,    32,    0,    0,  0 },
    { 0, 0,    0,   32,    32,    0,    0,  0 },
    { 0, 0,    0,   32,    32,    0,    0,  0 },
    { 0, 0,    0,   32,    32,    0,    0,  0 },
    { 0, 0,    0,   32,    32,    0,    0,  0 },
    { 0, 0,    0,   32,    32,    0,    0,  0 },
    { 0, 0,    0,   32,    32,    0,    0,  0 },
    { 0, 0,    0,   32,    32,    0,    0,  0 },
    { 0, 0,    0,   32,    32,    0,    0,  0 },
    { 0, 0,    0,   32,    32,    0,    0,  0 },
    { 0, 0,    0,   32,    32,    0,    0,  0 },
    { 0, 0,    0,   32,    32,    0,    0,  0 },
    { 0, 0,    0,   32,    32,    0,    0,  0 },
    { 0, 0,    0,   32,    32,    0,    0,  0 }
};

static td_s16 g_coef6_2M_h[COEF_BUTT][H_COLUMN] = {
    { 0,   6,   16,   19,   16,    6,    1,  0 },
    { 0,   6,   15,   20,   15,    7,    1,  0 },
    { 0,   6,   14,   20,   16,    7,    1,  0 },
    { 0,   6,   14,   20,   16,    7,    1,  0 },
    { 0,   5,   14,   20,   17,    7,    1,  0 },
    { 0,   5,   14,   20,   16,    8,    1,  0 },
    { 0,   5,   13,   20,   17,    8,    1,  0 },
    { 0,   5,   13,   19,   17,    8,    2,  0 },
    { 0,   4,   13,   20,   17,    8,    2,  0 },
    { 0,   4,   13,   19,   17,    9,    2,  0 },
    { 0,   4,   12,   19,   18,    9,    2,  0 },
    { 0,   4,   12,   19,   18,    9,    2,  0 },
    { 0,   4,   12,   19,   18,    9,    2,  0 },
    { 0,   3,   12,   19,   18,   10,    2,  0 },
    { 0,   3,   11,   19,   18,   10,    3,  0 },
    { 0,   3,   11,   19,   18,   10,    3,  0 },
    { 0,   3,   11,   18,   18,   11,    3,  0 }
};

static td_s16 g_coef6_1_5M_h[COEF_BUTT][H_COLUMN] = {
    { 0,   8,   14,   18,   14,    8,    2,  0 },
    { 0,   7,   14,   18,   15,    8,    2,  0 },
    { 0,   7,   14,   17,   15,    8,    3,  0 },
    { 0,   7,   14,   17,   15,    8,    3,  0 },
    { 0,   7,   13,   18,   15,    8,    3,  0 },
    { 0,   7,   13,   17,   15,    9,    3,  0 },
    { 0,   6,   13,   18,   15,    9,    3,  0 },
    { 0,   6,   13,   17,   16,    9,    3,  0 },
    { 0,   6,   13,   17,   16,    9,    3,  0 },
    { 0,   6,   13,   17,   16,    9,    3,  0 },
    { 0,   5,   12,   17,   16,   10,    4,  0 },
    { 0,   5,   12,   17,   16,   10,    4,  0 },
    { 0,   5,   12,   17,   16,   10,    4,  0 },
    { 0,   5,   12,   17,   16,   10,    4,  0 },
    { 0,   5,   11,   17,   16,   11,    4,  0 },
    { 0,   5,   11,   17,   16,   11,    4,  0 },
    { 0,   4,   11,   17,   17,   11,    4,  0 }
};

static td_s16 g_coef6_1M_h[COEF_BUTT][H_COLUMN] = {
    { 0,   8,   14,   16,   14,    8,    4,  0 },
    { 0,   8,   13,   16,   14,    9,    4,  0 },
    { 0,   8,   13,   16,   14,    9,    4,  0 },
    { 0,   8,   13,   16,   14,    9,    4,  0 },
    { 0,   8,   13,   16,   14,    9,    4,  0 },
    { 0,   8,   13,   16,   14,    9,    4,  0 },
    { 0,   7,   13,   16,   15,    9,    4,  0 },
    { 0,   7,   13,   16,   14,   10,    4,  0 },
    { 0,   7,   12,   16,   14,   10,    5,  0 },
    { 0,   7,   12,   16,   14,   10,    5,  0 },
    { 0,   7,   12,   15,   15,   10,    5,  0 },
    { 0,   6,   12,   16,   15,   10,    5,  0 },
    { 0,   6,   12,   16,   15,   10,    5,  0 },
    { 0,   6,   12,   15,   15,   11,    5,  0 },
    { 0,   6,   11,   16,   15,   11,    5,  0 },
    { 0,   6,   11,   15,   15,   11,    6,  0 },
    { 0,   6,   11,   15,   15,   11,    6,  0 }
};

static td_s16 g_coef4_6M_v[COEF_BUTT][V_COLUMN] = {
    {    4,   57,    4,   -1 },
    {    3,   56,    6,   -1 },
    {    2,   56,    7,   -1 },
    {    1,   56,    8,   -1 },
    {    0,   56,   10,   -2 },
    {   -1,   55,   12,   -2 },
    {   -1,   53,   14,   -2 },
    {   -2,   52,   16,   -2 },
    {   -3,   51,   18,   -2 },
    {   -3,   50,   20,   -3 },
    {   -3,   48,   22,   -3 },
    {   -4,   46,   25,   -3 },
    {   -4,   44,   27,   -3 },
    {   -4,   43,   29,   -4 },
    {   -4,   41,   31,   -4 },
    {   -4,   38,   34,   -4 },
    {   -4,   36,   36,   -4 }
};

static td_s16 g_coef4_5M_v[COEF_BUTT][V_COLUMN] = {
    {    9,   47,    9,   -1 },
    {    8,   47,   10,   -1 },
    {    7,   47,   12,   -2 },
    {    6,   47,   13,   -2 },
    {    5,   47,   14,   -2 },
    {    4,   46,   16,   -2 },
    {    3,   46,   17,   -2 },
    {    2,   45,   19,   -2 },
    {    2,   44,   20,   -2 },
    {    1,   43,   22,   -2 },
    {    1,   41,   24,   -2 },
    {    0,   41,   25,   -2 },
    {    0,   39,   27,   -2 },
    {   -1,   38,   29,   -2 },
    {   -1,   37,   30,   -2 },
    {   -1,   35,   32,   -2 },
    {   -2,   34,   34,   -2 }
};

static td_s16 g_coef4_4M_v[COEF_BUTT][V_COLUMN] = {
    {   13,   39,   13,   -1 },
    {   12,   40,   13,   -1 },
    {   11,   40,   14,   -1 },
    {   10,   39,   16,   -1 },
    {    9,   39,   17,   -1 },
    {    8,   39,   18,   -1 },
    {    7,   39,   19,   -1 },
    {    6,   38,   20,    0 },
    {    6,   37,   21,    0 },
    {    5,   37,   22,    0 },
    {    4,   36,   24,    0 },
    {    4,   35,   25,    0 },
    {    3,   35,   26,    0 },
    {    3,   33,   27,    1 },
    {    2,   33,   28,    1 },
    {    2,   32,   29,    1 },
    {    2,   30,   30,    2 }
};

static td_s16 g_coef4_area_v[COEF_BUTT][V_COLUMN] = {
    { 0,   32,   32,    0 },
    { 0,   32,   32,    0 },
    { 0,   32,   32,    0 },
    { 0,   32,   32,    0 },
    { 0,   32,   32,    0 },
    { 0,   32,   32,    0 },
    { 0,   32,   32,    0 },
    { 0,   32,   32,    0 },
    { 0,   32,   32,    0 },
    { 0,   32,   32,    0 },
    { 0,   32,   32,    0 },
    { 0,   32,   32,    0 },
    { 0,   32,   32,    0 },
    { 0,   32,   32,    0 },
    { 0,   32,   32,    0 },
    { 0,   32,   32,    0 },
    { 0,   32,   32,    0 }
};

static td_s16 g_coef4_2M_v[COEF_BUTT][V_COLUMN] = {
    {   16,   30,   16,    2 },
    {   15,   30,   17,    2 },
    {   15,   30,   17,    2 },
    {   14,   30,   17,    3 },
    {   13,   30,   18,    3 },
    {   13,   29,   19,    3 },
    {   12,   29,   20,    3 },
    {   11,   29,   20,    4 },
    {   11,   28,   21,    4 },
    {   10,   29,   21,    4 },
    {   10,   28,   22,    4 },
    {    9,   28,   22,    5 },
    {    9,   27,   23,    5 },
    {    8,   27,   23,    6 },
    {    8,   26,   24,    6 },
    {    7,   26,   25,    6 },
    {    7,   25,   25,    7 }
};

static td_s16 g_coef4_1_5M_v[COEF_BUTT][V_COLUMN] = {
    {   16,   29,   16,    3 },
    {   15,   29,   17,    3 },
    {   15,   29,   17,    3 },
    {   14,   29,   18,    3 },
    {   14,   28,   18,    4 },
    {   13,   28,   19,    4 },
    {   13,   28,   19,    4 },
    {   12,   28,   20,    4 },
    {   11,   27,   21,    5 },
    {   11,   27,   21,    5 },
    {   10,   27,   22,    5 },
    {   10,   26,   22,    6 },
    {    9,   26,   23,    6 },
    {    9,   26,   23,    6 },
    {    8,   25,   24,    7 },
    {    8,   25,   24,    7 },
    {    8,   24,   24,    8 }
};

static td_s16 g_coef4_1M_v[COEF_BUTT][V_COLUMN] = {
    {   16,   29,   16,    3 },
    {   16,   28,   17,    3 },
    {   15,   28,   17,    4 },
    {   15,   27,   18,    4 },
    {   14,   28,   18,    4 },
    {   14,   27,   19,    4 },
    {   13,   27,   19,    5 },
    {   12,   27,   20,    5 },
    {   12,   27,   20,    5 },
    {   11,   26,   21,    6 },
    {   11,   26,   21,    6 },
    {   10,   26,   22,    6 },
    {   10,   25,   22,    7 },
    {    9,   25,   23,    7 },
    {    9,   25,   23,    7 },
    {    9,   24,   23,    8 },
    {    8,   24,   24,    8 }
};

static td_s16 *g_zme_coef[ZOOM_COEF_ROW][V_COLUMN] = {
    {&g_coef6_6M_h[0][0],    &g_coef4_6M_v[0][0],   &g_coef6_6M_h[0][0],   &g_coef4_6M_v[0][0]},   // >1
    {&g_coef6_6M_h[0][0],    &g_coef4_6M_v[0][0],   &g_coef6_6M_h[0][0],   &g_coef4_6M_v[0][0]},   // ==1
    {&g_coef6_5M_h[0][0],    &g_coef4_5M_v[0][0],   &g_coef6_5M_h[0][0],   &g_coef4_5M_v[0][0]},   // >=0.75
    {&g_coef6_2_5M_h[0][0],  &g_coef4_4M_v[0][0],   &g_coef6_2_5M_h[0][0], &g_coef4_4M_v[0][0]},   // >0.5
    {&g_coef6_area_h[0][0],  &g_coef4_area_v[0][0], &g_coef6_area_h[0][0], &g_coef4_area_v[0][0]}, // =0.5
    {&g_coef6_2M_h[0][0],    &g_coef4_2M_v[0][0],   &g_coef6_2M_h[0][0],   &g_coef4_2M_v[0][0]},   // >=0.33
    {&g_coef6_2M_h[0][0],    &g_coef4_2M_v[0][0],   &g_coef6_2M_h[0][0],   &g_coef4_2M_v[0][0]},   // >=0.25
    {&g_coef6_1_5M_h[0][0],  &g_coef4_1_5M_v[0][0], &g_coef6_1_5M_h[0][0], &g_coef4_1_5M_v[0][0]}, // >=0.2
    {&g_coef6_1M_h[0][0],    &g_coef4_1M_v[0][0],   &g_coef6_1M_h[0][0],   &g_coef4_1M_v[0][0]},   // else
};

STATIC td_void vau_cal_yuv_wh(const vau_hardware_node *node, const drv_vau_surface *surface,
                              vau_yuv_width_height *yuv_wh)
{
    yuv_wh->y_wight_in    = node->mix.semi.src2_zmeireso.bits.iw + 1;
    yuv_wh->y_height_in   = node->mix.semi.src2_zmeireso.bits.ih + 1;
    yuv_wh->uv_wight_in   = yuv_wh->y_wight_in;
    yuv_wh->uv_height_in  = yuv_wh->y_height_in;

    yuv_wh->y_wight_out   = node->mix.semi.src2_zmeoreso.bits.ow + 1;
    yuv_wh->y_height_out  = node->mix.semi.src2_zmeoreso.bits.oh + 1;
    yuv_wh->uv_wight_out  = yuv_wh->y_wight_out;
    yuv_wh->uv_height_out = yuv_wh->y_height_out;

    if (surface->fmt == DRV_GFX_FMT_YUV420SP) {
        yuv_wh->uv_wight_in   = yuv_wh->y_wight_in / 0x2;
        yuv_wh->uv_height_in  = yuv_wh->y_height_in / 0x2;
    } else if (surface->fmt == DRV_GFX_FMT_YUV400SP) {
        yuv_wh->uv_wight_in   = 0;
        yuv_wh->uv_height_in  = 0;
    } else if (surface->fmt == DRV_GFX_FMT_YUV422SP) {
        yuv_wh->uv_wight_in   = yuv_wh->y_wight_in / 0x2;
    }
}

STATIC td_void vau_cal_zme_ratio(vau_zme_ratio *zme_ration, vau_yuv_width_height *yuv_wh)
{
    zme_ration->y_h_ratio  = (yuv_wh->y_wight_in << HSTEP_LSHFT) / yuv_wh->y_wight_out;
    zme_ration->y_v_ratio  = (yuv_wh->y_height_in << VSTEP_LSHFT) / yuv_wh->y_height_out;
    zme_ration->uv_h_ratio = (yuv_wh->uv_wight_in << HSTEP_LSHFT) / yuv_wh->uv_wight_out;
    zme_ration->uv_v_ratio = (yuv_wh->uv_height_in << VSTEP_LSHFT) / yuv_wh->uv_height_out;
}

STATIC td_s16 *vau_cal_zme_scale_coef(drv_gfx_zme_scale_type scale_type, td_u32 ratio)
{
    td_u32 base_step = VSTEP_LSHFT;

    if (scale_type == Y_HORZ_TYPE || scale_type == UV_HORZ_TYPE) {
        base_step = HSTEP_LSHFT;
    }
    base_step = 1 << base_step;

    if (ratio < base_step) {
        return g_zme_coef[0][scale_type];
    } else if (ratio == base_step) {
        return g_zme_coef[1][scale_type];
    } else if (ratio <= (0x4 * base_step / 0x3)) {
        return g_zme_coef[0x2][scale_type];
    } else if (ratio < 0x2 * base_step) {
        return g_zme_coef[0x3][scale_type];
    } else if (ratio == 0x2 * base_step) {
        return g_zme_coef[0x4][scale_type];
    } else if (ratio <= 0x3 * base_step) {
        return g_zme_coef[0x5][scale_type];
    } else if (ratio <= 0x4 * base_step) {
        return g_zme_coef[0x6][scale_type];
    } else if (ratio <= 0x5 * base_step) {
        return g_zme_coef[0x7][scale_type];
    } else {
        return g_zme_coef[0x8][scale_type];
    }
    return TD_NULL;
}

STATIC td_u16 vau_zme_encode_signed10(td_s16 input_data)
{
    td_u16 temp_data = 0;

    if ((input_data > 511) || (input_data < -512)) { /* 511/-512:10bit有符号最大/小值 */
        gpu_err("input_data error: %d !", input_data);
        return 0;
    }
    if (input_data < 0) {
        temp_data |= 0x200; /* 符号位放在第10bit */
    }
    temp_data |= ((td_u16)input_data & 0x1FF);
    return temp_data;
}

STATIC td_void vau_calc_zme_coef_h(td_s16 *h_scale_coef_table, td_u8 idx)
{
    td_u32 num = 0;
    td_u32 lowBits1;
    td_u32 lowBits2;
    td_u32 lowBits3;

    if (idx == 0) {
        if (memset_s(g_vau_zme_yh_coef, sizeof(g_vau_zme_yh_coef), 0x0, sizeof(g_vau_zme_yh_coef)) != EOK) {
            gpu_err("memset g_vau_zme_yh_coef fail");
            return;
        }
    } else {
        if (memset_s(g_vau_zme_uvh_coef, sizeof(g_vau_zme_uvh_coef), 0x0, sizeof(g_vau_zme_uvh_coef)) != EOK) {
            gpu_err("memset g_vau_zme_uvh_coef fail");
            return;
        }
    }

    td_u32 *base_addr = (idx == 0 ? g_vau_zme_yh_coef : g_vau_zme_uvh_coef);
    for (td_u32 j = 0; j < COEF_BUTT * H_COLUMN; j += 8) { // 8: 步长，每次处理8个数据，紧凑排布为3个数据
        lowBits1 = vau_zme_encode_signed10(h_scale_coef_table[j + 0x0]); // 提取低10位
        lowBits2 = vau_zme_encode_signed10(h_scale_coef_table[j + 0x1]);
        lowBits3 = vau_zme_encode_signed10(h_scale_coef_table[j + 0x2]);
        base_addr[num++] = (lowBits3 << 20) | (lowBits2 << 10) | lowBits1; // 10:左移10, 20:左移20

        lowBits1 = vau_zme_encode_signed10(h_scale_coef_table[j + 0x3]); // 提取低10位
        lowBits2 = vau_zme_encode_signed10(h_scale_coef_table[j + 0x4]);
        lowBits3 = vau_zme_encode_signed10(h_scale_coef_table[j + 0x5]);
        base_addr[num++] = (lowBits3 << 20) | (lowBits2 << 10) | lowBits1; // 10:左移10, 20:左移20

        lowBits1 = vau_zme_encode_signed10(h_scale_coef_table[j + 0x6]); // 提取低10位
        lowBits2 = vau_zme_encode_signed10(h_scale_coef_table[j + 0x7]);
        base_addr[num++] = (lowBits2 << 10) | lowBits1; // 10:左移10
    }
}

STATIC td_void vau_calc_zme_coef_v(td_s16 *v_scale_coef_table, td_u8 idx)
{
    td_u32 num = 0;
    td_u16 high_u;
    td_u16 low_u;

    if (idx == 0) {
        if (memset_s(g_vau_zme_yv_coef, sizeof(g_vau_zme_yv_coef), 0x0, sizeof(g_vau_zme_yv_coef)) != EOK) {
            gpu_err("memset g_vau_zme_yv_coef fail");
            return;
        }
    } else {
        if (memset_s(g_vau_zme_uvv_coef, sizeof(g_vau_zme_uvv_coef), 0x0, sizeof(g_vau_zme_uvv_coef)) != EOK) {
            gpu_err("memset g_vau_zme_uvv_coef fail");
            return;
        }
    }

    td_u32 *base_addr = (idx == 0 ? g_vau_zme_yv_coef : g_vau_zme_uvv_coef);
    for (td_u32 j = 0; j < COEF_BUTT * V_COLUMN; j += 2) { // 2: 步长
        low_u = (td_u16)v_scale_coef_table[j];
        high_u = (td_u16)v_scale_coef_table[j + 1];
        base_addr[num++] = ((td_u32)high_u << 16) | low_u; // 16:左移16
    }
}

STATIC td_void vau_calc_zme_coef(vau_zme_coef *zme_coef)
{
    vau_calc_zme_coef_h(zme_coef->y_h_scale_coef, 0);
    vau_calc_zme_coef_h(zme_coef->uv_h_scale_coef, 1);
    vau_calc_zme_coef_v(zme_coef->y_v_scale_coef, 0);
    vau_calc_zme_coef_v(zme_coef->uv_v_scale_coef, 1);
}

td_void vau_set_zme_coef_soft(td_u32 lhaddr, td_u32 lvaddr, td_u32 chaddr, td_u32 cvaddr)
{
    td_s32 ret;
    ret = memcpy_s((td_void *)(uintptr_t)lhaddr, ZME_COEF_MAX_LEN, g_vau_zme_yh_coef, sizeof(g_vau_zme_yh_coef));
    if (ret != EOK) {
        gpu_err("zme_lh_coef copy fail!\n");
    }
    ret = memcpy_s((td_void *)(uintptr_t)lvaddr, ZME_COEF_MAX_LEN, g_vau_zme_yv_coef, sizeof(g_vau_zme_yv_coef));
    if (ret != EOK) {
        gpu_err("zme_lv_coef copy fail!\n");
    }
    ret = memcpy_s((td_void *)(uintptr_t)chaddr, ZME_COEF_MAX_LEN, g_vau_zme_uvh_coef, sizeof(g_vau_zme_uvh_coef));
    if (ret != EOK) {
        gpu_err("zme_ch_coef copy fail!\n");
    }
    ret = memcpy_s((td_void *)(uintptr_t)cvaddr, ZME_COEF_MAX_LEN, g_vau_zme_uvv_coef, sizeof(g_vau_zme_uvv_coef));
    if (ret != EOK) {
        gpu_err("zme_cv_coef copy fail!\n");
    }
}

td_void vau_get_zme_scale_coef(vau_hardware_node *node, const drv_vau_surface *surface)
{
    vau_yuv_width_height yuv_wh = { 0 };
    vau_zme_ratio zme_ratio = { 0 };
    vau_zme_coef zme_coef = { 0 };

    // 1. 计算y和uv的输入输出分辨率
    vau_cal_yuv_wh(node, surface, &yuv_wh);

    // 2. 计算y 和 uv 的缩放倍率
    vau_cal_zme_ratio(&zme_ratio, &yuv_wh);

    // 3. 根据缩放倍率获取缩放系数
    zme_coef.y_h_scale_coef = vau_cal_zme_scale_coef(Y_HORZ_TYPE, zme_ratio.y_h_ratio);
    zme_coef.y_v_scale_coef = vau_cal_zme_scale_coef(Y_VERT_TYPE, zme_ratio.y_v_ratio);
    zme_coef.uv_h_scale_coef = vau_cal_zme_scale_coef(UV_HORZ_TYPE, zme_ratio.uv_h_ratio);
    zme_coef.uv_v_scale_coef = vau_cal_zme_scale_coef(UV_VERT_TYPE, zme_ratio.uv_v_ratio);

    // 4. 按照规则重新组织缩放系数的内存排布
    // 对水平缩放系数，每行8个数据，每3个数据各取低10bit并拼成一个32bit数据，最后两个也取低10bit拼成U32
    // 对垂直缩放系数，每行4个数据，每2个数据各取低16bit并拼成一个32bit数据
    vau_calc_zme_coef(&zme_coef);
}

td_void vau_set_zme_scale_coeff(vau_hardware_node *node, const drv_vau_surface *surface)
{
    td_u32 lh_addr = GPU_ZME_COEF_ADDR;
    td_u32 lv_addr = lh_addr + ZME_COEF_MAX_LEN;
    td_u32 ch_addr = lv_addr + ZME_COEF_MAX_LEN;
    td_u32 cv_addr = ch_addr + ZME_COEF_MAX_LEN;

    node->tde_src2_zme_lhaddr.u32 = lh_addr;
    node->tde_src2_zme_lvaddr.u32 = lv_addr;
    node->tde_src2_zme_chaddr.u32 = ch_addr;
    node->tde_src2_zme_cvaddr.u32 = cv_addr;

    // 软算法系数
    vau_get_zme_scale_coef(node, surface);
    vau_set_zme_coef_soft(lh_addr, lv_addr, ch_addr, cv_addr);
}
