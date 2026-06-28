/*
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies Co., Ltd..
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _ICE_V5R1_BLOCK_HTC_INTERFACE_H_
#define _ICE_V5R1_BLOCK_HTC_INTERFACE_H_
#include "td_base.h"
#include "soc_vau_type.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    HI_GFX_FMT_ARGB8888 = 0x00,
    HI_GFX_FMT_KRGB8888 = 0x01,
    HI_GFX_FMT_ARGB4444 = 0x02,
    HI_GFX_FMT_ARGB1555 = 0x03,
    HI_GFX_FMT_ARGB8565 = 0x04,
    HI_GFX_FMT_RGB888 = 0x05,
    HI_GFX_FMT_RGB444 = 0x06,
    HI_GFX_FMT_RGB555 = 0x07,
    HI_GFX_FMT_RGB565 = 0x08,
    HI_GFX_FMT_A1 = 0x09,
    HI_GFX_FMT_A8 = 0x0A,
    HI_GFX_FMT_A1B = 0x0D,
    HI_GFX_FMT_CLUT1 = 0x10,
    HI_GFX_FMT_CLUT2 = 0x11,
    HI_GFX_FMT_CLUT4 = 0x12,
    HI_GFX_FMT_CLUT8 = 0x13,
    HI_GFX_FMT_ACLUT44 = 0x14,
    HI_GFX_FMT_ACLUT88 = 0x15,
    HI_GFX_FMT_CLUT1B = 0x16,
    HI_GFX_FMT_CLUT2B = 0x17,
    HI_GFX_FMT_CLUT4B = 0x18,
    HI_GFX_FMT_MAX
} gfx_fmt;

typedef struct {
    unsigned int width;
    unsigned int height;
    unsigned int stride;
    unsigned int pixel_format; // 0-- RGB888, 1-- ARGB8888, 2-- RGB565
    unsigned int tile_width;   // TILE: 4-- 4x4, 6-- 6x4, 8-- 8x4, 16-- 16x4
    unsigned int mode_alpha;   // 0-- A_BYPASS,   1-- A_MERGE,  2-- A_SEPARATE, 3-- A_CONST,   4-- A_BINARY
    unsigned int mode_rgb;     // 0-- RGB_BYPASS, 1-- RGB_ASTC, 2-- RGB_TABLE,  3-- RGB_CONST ,4-- RGB_BINARY

    unsigned char *red_channel;
    unsigned char *green_channel;
    unsigned char *blue_channel;
    unsigned char *alpha_channel;
} cmp_input;

typedef struct {
    unsigned int version;      // 0:ES, 1:CS
    unsigned int alpha_bypass;  // for ES
    unsigned int width;         // 10bit
    unsigned int height;        // 10bit
    unsigned int stride;        // Size of the TILE row after compression
    unsigned int pixel_format;  // GFX_FMT_RGB565,GFX_FMT_RGB888,GFX_FMT_ARGB8888
    unsigned int tile_size;     // tile size, in 64bits
    unsigned int alpha_value;  // 0:ALPHA rand value; 1:ALPHA all 0; 2:ALPHA all 255; 3:ALPHA 0 and 255
    unsigned int mode;         // The parameter is used for decoding
    unsigned int tile_width;
} ext_vau_cmp_header;

typedef struct {
    unsigned int data_size;
    unsigned char *data;
    unsigned int  subjective_review;
} cmp_output;

typedef struct {
    unsigned int width;
    unsigned int height;
    unsigned int stride;
    unsigned int pixel_format;
    unsigned char *red_channel;
    unsigned char *green_channel;
    unsigned char *blue_channel;
    unsigned char *alpha_channel;
} dcmp_output;

typedef struct {
    unsigned char *red_channel;
    unsigned char *green_channel;
    unsigned char *blue_channel;
    unsigned char *alpha_channel;
} rec_output;

typedef struct {
    unsigned int auto_cmp_mode;       // 0--debug mode 1--auto debug mode
    unsigned int high_cmp_ratio_mode;  // 1--high_cmp_ratio_mode 0--no_high_cmp_ratio_mode
    cmp_input *cmp_in;
} auto_cmp_input;

typedef struct {
    unsigned int width;
    unsigned int height;
    unsigned int stride;
    unsigned int pixel_format;  // 0-- RGB888, 1-- ARGB8888, 2-- RGB565
    unsigned int tile_width;   // TILE: 4-- 4x4, 6-- 6x4, 8-- 8x4, 16-- 16x4
    unsigned int mode_alpha;   // 0-- A_BYPASS,   1-- A_MERGE,  2-- A_SEPARATE, 3-- A_CONST,   4-- A_BINARY
    unsigned int mode_rgb;     // 0-- RGB_BYPASS, 1-- RGB_ASTC, 2-- RGB_TABLE,  3-- RGB_CONST ,4-- RGB_BINARY
} header;

typedef struct cmpBs {
    unsigned char *bs_data;   // head(128bits)+CMP data
    unsigned int data_size;   // in byte
    unsigned int cmp_ratio;   // real cmp ratio
    unsigned int is_valid;    // 0--no valid 1--valid
    unsigned int subjective_review;  // 0--no subjective review  1--subjective  review
    header para;
} BS_DATA;

typedef struct {
    BS_DATA *cmp_bs[4];
    unsigned int auto_cmp_mode;
    unsigned int high_cmp_ratio_mode;  // 1--high_cmp_ratio_mode 0--no_high_cmp_ratio_mode
    unsigned int valid_bs_num;         // number of valid streams
} auto_cmp_output;

typedef struct auto_rec_out_data {
    unsigned char *red_channel;
    unsigned char *green_channel;
    unsigned char *blue_channel;
    unsigned char *alpha_channel;
    unsigned int cmp_ratio;  // real cmp ratio
    unsigned int is_valid;   // 0--no valid 1--valid
    unsigned int subjective_review;  // 0--no subjective review  1--subjective  review
} REC_DATA;

typedef struct {
    REC_DATA *rec_out_data[4];
    unsigned int auto_cmp_mode;
    unsigned int valid_image_num;
} auto_dcmp_output;

__attribute__ ((visibility("default"))) void ice_block_htc_offline_channal_one_to_four(unsigned char *image,
    cmp_input *cmp_in); // one channal to four channal
__attribute__ ((visibility("default"))) void ice_block_htc_offline_channal_four_to_one(unsigned char *image,
    dcmp_output *cmp_out); // four channal to one channal

__attribute__ ((visibility("default"))) int uapi_vau_parse_cmp_header(unsigned char *head, unsigned int size,
    ext_vau_cmp_header *info);
__attribute__ ((visibility("default"))) unsigned int ice_v5r1_block_htc_cmp(cmp_input *cmp_in, cmp_output *cmp_out);
__attribute__ ((visibility("default"))) unsigned int ice_v5r1_block_htc_dcmp(cmp_output *cmp_out,
    dcmp_output *dcmp_out);
__attribute__ ((visibility("default"))) unsigned int ice_v5r1_block_get_buffer_size(cmp_input *cmp_in);
__attribute__ ((visibility("default"))) unsigned int ice_v5r1_block_htc_auto_cmp(auto_cmp_input *auto_cmp_in,
    auto_cmp_output *auto_cmp_out);
__attribute__ ((visibility("default"))) unsigned int ice_v5r1_block_htc_auto_dcmp(auto_cmp_output *auto_cmp_out,
    auto_dcmp_output *auto_dcmp_out);

__attribute__ ((visibility("default"))) unsigned int ice_block_htc_offline_cmp(cmp_input *cmp_in,
    cmp_output *cmp_out);
__attribute__ ((visibility("default"))) unsigned int ice_block_htc_offline_dcmp(cmp_output *cmp_out,
    dcmp_output *dcmp_out);
__attribute__ ((visibility("default"))) unsigned int ice_block_get_offline_buffer_size(cmp_input *cmp_in);
__attribute__ ((visibility("default"))) unsigned int ice_block_htc_offline_auto_cmp(auto_cmp_input *auto_cmp_in,
    auto_cmp_output *auto_cmp_out);
__attribute__ ((visibility("default"))) unsigned int ice_block_htc_offline_auto_dcmp(auto_cmp_output *auto_cmp_out,
    auto_dcmp_output *auto_dcmp_out);
#ifdef __cplusplus
}
#endif

#endif
