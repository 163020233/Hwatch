/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2024. All rights reserved.
 * Description : hdr algorithm constant
 */

#ifndef HDR_CONSTANTS_H
#define HDR_CONSTANTS_H

#include "td_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

static const td_s32 lv_table[9] = { 70, 80, 90, 100, 110, 120, 130, 140, 150 };
static const td_float ratio_table[9] = { 4.0, 4.2, 4.4, 4.5, 4.8, 5.0, 5.5, 5.8, 6.6 };

static const td_u32 clip_min_y2r_base = 0;
static const td_u32 clip_max_y2r_base = 4095;

static const td_u32 scale_y2r_base = 11;
static const td_s32 m33y2r_601_base[3][3] = { { 9567, 0, 13113 }, { 9567, -3219, -6679 }, { 9567, 16574, 0 } };
static const td_s32 m33y2r_709_base[3][3] = { { 9567, 0, 14729 }, { 9567, -1752, -4379 }, { 9567, 17356, 0 } };
static const td_s32 m33y2r_2020ncl_base[3][3] = { { 9567, 0, 13792 }, { 9567, -1539, -5344 }, { 9567, 17597, 0 } };

static const td_s32 dc_in_y2r_base[3] = { -64, -512, -512 };
static const td_s32 dc_out_y2r_base[3] = { 0, 0, 0 };

static const td_u32 clip_min_y2r_gainmap = 0;
static const td_u32 clip_max_y2r_gainmap = 1023;

static const td_u32 scale_y2r_gainmap = 8;
static const td_s32 m33y2r_601_gainmap[3][3] = { { 299, 0, 410 }, { 299, -101, -209 }, { 299, 518, 0 } };

static const td_s32 dc_in_y2r_gainmap[3] = { -64, -512, -512 };
static const td_s32 dc_out_y2r_gainmap[3] = { 0, 0, 0 };

static const td_u32 clip_min_y2r_gmap1 = 0;
static const td_u32 clip_max_y2r_gmap1 = 2097151;  // 2^21-1

static const td_u32 clip_min_y2r_gmap2 = 0;
static const td_u32 clip_max_y2r_gmap2 = 2097151;  // 2^21-1

static const td_u32 scale_y2r_gmap1 = 14;
static const td_u32 scale_y2r_gmap2 = 14;

static const td_s32 m33gmap_2020ncl_709[3][3] = {
#ifdef GPU_1A22
    { 0x3FD48A00, 0xBF166F00, 0xBD953200 },
    { 0xBDFF1400, 0x3F910200, 0xBC08CE00 },
    { 0xBC94AF00, 0xBDCDFC00, 0x3F8F3200 }
#else
    { 27206, -9628, -1194 },
    { -2040, 18561, -137 },
    { -297, -1648, 18329 }
#endif
};

static const td_s32 m33gmap_2020ncl_p3[3][3] = { { 22013, -4623, -1006 },
                                                 { -1070, 17626, -172 },
                                                 { 46, -321, 16659 } };

static const td_s32 m33gmap_2020ncl_disp[3][3] = { { 19251, -2346, -521 },
                                                   { -1018, 17045, 357 },
                                                   { 27, -87, 16444 } };

static const td_s32 m33gmap_709_2020ncl[3][3] = {
#ifdef GPU_1A22
    { 0x3F209D00, 0x3EA89700, 0x3D316500 },
    { 0x3D8D8400, 0x3F6B6600, 0x3C3A1F00 },
    { 0x3C864400, 0x3DB43E00, 0x3F654600 }
#else
    { 10279, 5395, 710 },
    { 1132, 15066, 186 },
    { 269, 1442, 14673 }
#endif
};

static const td_s32 m33gmap_p3_2020ncl[3][3] = { { 12351, 3254, 779 }, { 750, 15430, 204 }, { -20, 288, 16116 } };

static const td_u32 luma_cal_scale = 15;
static const td_u32 m3_luma_cal[3] = {
#ifdef GPU_1A22
    0x3e868000, 0x3f2d9100, 0x3d72e400
#else
    8608, 22217, 1943
#endif
};
static const td_u32 clip_min_tm = 0;
static const td_u32 clip_max_tm = 2097151;

static const td_u32 tm_coef_scale = 7;
static const td_u32 tm_coef_scale_pq2sdr = 7;
static const td_u32 tm_coef_scale_hlg2sdr = 12;
static const td_u32 tm_coef_scale_linear = 8;

static const td_u32 tm_lut_pq2sdr[64] = {
#ifdef GPU_1A22
        0x41AFF000, 0x41AFF000, 0x41AFF000, 0x41AFF000,
        0x41AFE000, 0x41AFC000, 0x41AFB000, 0x41AF9000,
        0x41AF7000, 0x41AF6000, 0x41AF4000, 0x41AF2000,
        0x41AEB000, 0x41AE5000, 0x41AD7000, 0x41ACA000,
        0x41ABC000, 0x41AAF000, 0x41A95000, 0x41A7B000,
        0x41A61000, 0x41A48000, 0x41A2F000, 0x419CE000,
        0x41972000, 0x4191B000, 0x418C8000, 0x41879000,
        0x4182E000, 0x417CC000, 0x41744000, 0x416C2001,
        0x41646001, 0x415D0000, 0x41560000, 0x414F4000,
        0x4148E001, 0x41320000, 0x411EE001, 0x410EE001,
        0x41000000, 0x40E38000, 0x40CCC001, 0x40BA4001,
        0x40AAC001, 0x409D8000, 0x40924001, 0x40888000,
        0x40800000, 0x40710000, 0x40638002, 0x40578002,
        0x404D0000, 0x402A8002, 0x40128002, 0x40000000,
        0x3FE40000, 0x3FCD0004, 0x3FBA0000, 0x3FAB0004,
        0x3F9E0000, 0x3F920000, 0x3F890004, 0x3F800000,
#else
    2815, 2815, 2815, 2815, 2814, 2812, 2811, 2809, 2807, 2806, 2804, 2802, 2795, 2789, 2775, 2762,
    2748, 2735, 2709, 2683, 2657, 2632, 2607, 2510, 2418, 2331, 2248, 2169, 2094, 2022, 1954, 1889,
    1827, 1768, 1712, 1658, 1607, 1424, 1271, 1143, 1024, 910,  819,  745,  683,  630,  585,  546,
    512,  482,  455,  431,  410,  341,  293,  256,  228,  205,  186,  171,  158,  146,  137,  128,
#endif
};

static const td_u32 tm_lut_hlg2sdr[64] = {
#ifdef GPU_1A22
    0x3EF7C009, 0x3EF7C009, 0x3F0E3FFF, 0x3F1A3001,
    0x3F234FF8, 0x3F3B2FFD, 0x3F4AA002, 0x3F563001,
    0x3F5F7004, 0x3F673FFB, 0x3F6DEFFA, 0x3F73D007,
    0x3F82F7FD, 0x3F896FFC, 0x3F926003, 0x3F9877FF,
    0x3F9CF7FD, 0x3FA077FF, 0x3FA5C001, 0x3FA9A7FC,
    0x3FACBFFD, 0x3FAF5800, 0x3FB197FF, 0x3FB89803,
    0x3FBDF002, 0x3FC26003, 0x3FC62802, 0x3FC98801,
    0x3FCC7FFE, 0x3FCF2FFD, 0x3FD19FFD, 0x3FD3CFFF,
    0x3FD5D7FE, 0x3FD7B003, 0x3FD95FFE, 0x3FDAEFFE,
    0x3FDC6003, 0x3FE11002, 0x3FE47000, 0x3FE6C804,
    0x3FE85804, 0x3FE94003, 0x3FE99FFD, 0x3FE99000,
    0x3FE91FFF, 0x3FE86801, 0x3FE75FFE, 0x3FE62004,
    0x3FE4AFFF, 0x3FE31002, 0x3FE15001, 0x3FDF6FFC,
    0x3FDD6FFC, 0x3FD4B7FE, 0x3FCB4802, 0x3FC197FF,
    0x3FB80000, 0x3FAEA800, 0x3FA5B003, 0x3F9D27FE,
    0x3F951FFF, 0x3F8D97FF, 0x3F869004, 0x3F800000,
#else
    1982, 1982, 2276, 2467, 2613, 2995, 3242, 3427, 3575, 3700, 3807, 3901, 4191, 4398, 4684, 4879,
    5023, 5135, 5304, 5429, 5528, 5611, 5683, 5907, 6078, 6220, 6341, 6449, 6544, 6630, 6708, 6778,
    6843, 6902, 6956, 7006, 7052, 7202, 7310, 7385, 7435, 7464, 7476, 7474, 7460, 7437, 7404, 7364,
    7318, 7266, 7210, 7150, 7086, 6807, 6505, 6195, 5888, 5589, 5302, 5029, 4772, 4531, 4306, 4096,
#endif
};

static const td_u32 tm_lut_linear[64] = {
#ifdef GPU_1A22
    0x41A00000
#else
    256
#endif
};

static const td_u32 de_pq_lut[64] = {
    0,     0,     0,     0,      0,      1,      1,      2,      3,      4,      5,       6,       6,
    7,     11,    14,    18,     24,     38,     58,     87,     125,    175,    238,     317,     415,
    537,   685,   865,   1082,   1343,   1655,   2025,   2465,   2984,   3596,   4315,    5158,    6143,
    7294,  8635,  10195, 12007,  14110,  16547,  19369,  22633,  26406,  30764,  35793,   41596,   48288,
    56002, 64892, 86935, 116188, 155014, 206576, 275129, 366417, 488226, 651170, 1164118, 2097150,
};

static const td_u32 de_hlg_lut[64] = {
    0,      0,      1,      3,      6,      11,     24,     43,     96,     171,     267,     384,     523,
    683,    1067,   1537,   2092,   2732,   4269,   6147,   8367,   10928,  13831,   17075,   20661,   24588,
    28857,  33467,  38419,  43712,  49347,  55323,  61641,  68300,  75301,  82643,   90327,   98352,   106719,
    115427, 124477, 133868, 143601, 153675, 164091, 174848, 186273, 198741, 212348,  227197,  243403,  261088,
    280389, 301452, 349526, 406781, 474971, 556185, 652910, 768109, 905311, 1068716, 1495116, 2097151,
};

static const td_u32 de_gmm22_lut[64] = {
    0,      0,      1,      2,      6,       11,      26,      49,      118,     223,     364,     544,     764,
    1025,   1674,   2500,   3509,   4708,    7691,    11487,   16124,   21630,   28029,   35340,   43584,   52780,
    62942,  74088,  86232,  99387,  113568,  128785,  145052,  162380,  180780,  200261,  220835,  242511,  265299,
    289207, 314244, 340420, 367742, 396218,  425856,  456664,  488650,  521820,  556182,  591742,  628508,  666486,
    705682, 746103, 830645, 920159, 1014692, 1114289, 1218993, 1328845, 1443887, 1564158, 1820538, 2097151,
};

static const td_u32 gmm22_lut[64] = {
    0,    14,   19,   23,   26,   36,   44,   50,   68,   82,   93,   103,  112,  120,  128,  154,
    175,  240,  289,  329,  396,  451,  500,  543,  582,  619,  653,  685,  715,  744,  771,  798,
    823,  848,  1019, 1162, 1286, 1397, 1498, 1592, 1679, 1762, 1840, 1914, 1985, 2053, 2118, 2181,
    2242, 2301, 2358, 2414, 2623, 2813, 2989, 3153, 3308, 3455, 3594, 3727, 3855, 3978, 4095, 4095,
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* HDR_CONSTANTS_H */
