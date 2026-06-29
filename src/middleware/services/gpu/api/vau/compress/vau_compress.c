/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: vau compress
 */
#include "vau_compress.h"
#include "vau_debug.h"

#define BYTE_SIZE                8

typedef struct {
    td_u8 *cur_buf_ptr; // potd_s32 to current position of bit stream buffer
    td_s32 left_bits;   // left bits in one byte, which can be filled bits
} vau_block_htc_bs;

STATIC td_u32 g_vau_block_htc_i_mask[33] = { // 33: size of arry
    0x00,
    0x01, 0x03, 0x07, 0x0f,
    0x1f, 0x3f, 0x7f, 0xff,
    0x1ff, 0x3ff, 0x7ff, 0xfff,
    0x1fff, 0x3fff, 0x7fff, 0xffff,
    0x1ffff, 0x3ffff, 0x7ffff, 0xfffff,
    0x1fffff, 0x3fffff, 0x7fffff, 0xffffff,
    0x1ffffff, 0x3ffffff, 0x7ffffff, 0xfffffff,
    0x1fffffff, 0x3fffffff, 0x7fffffff, 0xffffffff
};

STATIC td_bool vau_check_cmp_info(const ext_vau_cmp_header *info)
{
    const td_u32 cmp_w_max = 4096;
    const td_u32 cmp_w_min = 4;
    const td_u32 cmp_h_max = 4096;
    const td_u32 cmp_h_min = 4;

    if ((info->width < cmp_w_min) || (info->width > cmp_w_max) || (info->height < cmp_h_min) ||
        (info->height > cmp_h_max)) {
        gpu_err("invalid w h %u %u\n", info->width, info->height);
        return TD_FALSE;
    }

    if ((info->pixel_format != EXT_GFX_FMT_ARGB8888) &&
        (info->pixel_format != EXT_GFX_FMT_RGB888) &&
        (info->pixel_format != EXT_GFX_FMT_ARGB8565) &&
        (info->pixel_format != EXT_GFX_FMT_RGB565)) {
        gpu_err("invalid pixel format %u\n", info->pixel_format);
        return TD_FALSE;
    }

    return TD_TRUE;
}

static inline ext_gfx_fmt vau_trans_cmp_pixel_fmt(td_u32 fmt)
{
    switch (fmt) {
        case 0:
            return EXT_GFX_FMT_RGB888;
        case 2: /* rgb565 is 2 */
            return EXT_GFX_FMT_RGB565;
        case 3: /* argb8565 is 3 */
            return EXT_GFX_FMT_ARGB8565;
        default:
            return EXT_GFX_FMT_ARGB8888;
    }
}

STATIC td_void vau_block_htc_strminit(vau_block_htc_bs *bs, td_u8 *bs_buf_ptr)
{
    bs->cur_buf_ptr = bs_buf_ptr;
    bs->left_bits = BYTE_SIZE;
}

STATIC td_u32 vau_block_htc_strmshowbits(const vau_block_htc_bs *bs, td_u32 n_bits)
{
    td_u32 i_cache;

    if ((n_bits > 0)) {
        //  3: size of arry 24: bit_width 2: size of arry 16: bit_width
        i_cache = ((bs->cur_buf_ptr[3] << 24) + (bs->cur_buf_ptr[2] << 16) + (bs->cur_buf_ptr[1] << BYTE_SIZE) +
            bs->cur_buf_ptr[0]) >> (td_u32)(BYTE_SIZE - bs->left_bits);
        return (i_cache & g_vau_block_htc_i_mask[n_bits]);
    }

    return 0;
}

STATIC td_void vau_block_htc_strmflushbits(vau_block_htc_bs *bs, td_u32 n_bits)
{
    bs->left_bits = bs->left_bits - (td_s32)n_bits;

    while (bs->left_bits <= 0) {
        bs->cur_buf_ptr++;
        bs->left_bits += BYTE_SIZE;
    }
}

STATIC td_u32 vau_block_htc_strmrdbit(vau_block_htc_bs *bs, td_u32 len)
{
    td_s32 i_shr;
    td_u32 word = 0;
    td_u32 bit_pos = 0;
    td_u32 tmp_len = len;

    while (tmp_len > 0) {
        if ((i_shr = bs->left_bits - (td_s32)tmp_len) >= 0) {
            /* more in the buffer than requested */
            word |= (((td_u32)(*bs->cur_buf_ptr) >> (td_u32)(BYTE_SIZE - bs->left_bits)) &
            g_vau_block_htc_i_mask[tmp_len]) << bit_pos;
            bs->left_bits -= (td_s32)tmp_len;
            if (bs->left_bits == 0) {
                bs->cur_buf_ptr++;
                bs->left_bits = BYTE_SIZE;
            }

            return (word);
        } else {
            word |=
                (((td_u32)(*bs->cur_buf_ptr) >> (td_u32)(BYTE_SIZE - bs->left_bits)) &
                g_vau_block_htc_i_mask[bs->left_bits]) << bit_pos;
            bit_pos += (td_u32)bs->left_bits;
            tmp_len -= (td_u32)bs->left_bits;
            bs->cur_buf_ptr++;
            bs->left_bits = BYTE_SIZE;
        }
    }

    return (word);
}

STATIC td_void vau_parse_cmp_header_es(vau_block_htc_bs *head, ext_vau_cmp_header *info)
{
    td_u32 ratio_rgb, ratio_alpha, pix_format;
    const td_u32 slice_width      = 16;
    const td_u32 slice_height     = 4;
    const td_u32 ratio_no_cmp_base = 1000;
    const td_u32 ratio_cmp_alpha   = 2000;
    const td_u32 ratio_cmp_rgb     = 3000;
    vau_block_htc_bs bs;
    bs.cur_buf_ptr = head->cur_buf_ptr;
    bs.left_bits = head->left_bits;

    info->width        = vau_block_htc_strmrdbit(&bs, 10);  // FrmWth 10: bit_width
    info->height       = vau_block_htc_strmrdbit(&bs, 10);  // FrmHgt 10: bit_width
    pix_format         = vau_block_htc_strmrdbit(&bs, 2);   // 2 bits
    info->pixel_format = vau_trans_cmp_pixel_fmt(pix_format);
    pix_format         = info->pixel_format;
    info->alpha_bypass = vau_block_htc_strmrdbit(&bs, 1);
    info->mode         = vau_block_htc_strmrdbit(&bs, 1);
    info->tile_width   = 16;                                 // 16 tile_width
    ratio_rgb = (info->mode != 0) ? ratio_no_cmp_base : ratio_cmp_rgb;
    ratio_alpha = (info->mode != 0 || info->alpha_bypass != 0) ? ratio_no_cmp_base : ratio_cmp_alpha;

    if (pix_format == EXT_GFX_FMT_RGB888) {
        /* 24 bits for rgb,7 8 for up align */
        info->stride = ((((slice_width * slice_height * 24) * ratio_no_cmp_base / ratio_rgb) + 7) / 8) *
            ((info->width + slice_width - 1) / slice_width);
    } else if (pix_format == EXT_GFX_FMT_ARGB8888) {
        /* 24 bits for rgb, 8 bits, 7 bits */
        info->stride = ((((slice_width * slice_height * 24) * ratio_no_cmp_base / ratio_rgb) +
            ((slice_width * slice_height * 8) * ratio_no_cmp_base / ratio_alpha) + 7) / // 8 bits, 7 bits
            8) * ((info->width + slice_width - 1) / slice_width); // 8 bits
    } else {
        gpu_err("stride calc Error pixel_format\n");
    }
}

STATIC td_void vau_parse_cmp_header_cs_v1(vau_block_htc_bs *head, ext_vau_cmp_header *info)
{
    vau_block_htc_bs bs;
    bs.cur_buf_ptr = head->cur_buf_ptr;
    bs.left_bits = head->left_bits;
    vau_block_htc_strmrdbit(&bs, 5);                          // reserved 5: bit_width
    info->tile_size    = vau_block_htc_strmrdbit(&bs, 6);     // tile_size 6: bit_width
    info->width        = vau_block_htc_strmrdbit(&bs, 10);    // FrmWth 10: bit_width
    info->height       = vau_block_htc_strmrdbit(&bs, 10);    // FrmHgt 10: bit_width
    info->tile_width   = (td_s32)vau_block_htc_strmrdbit(&bs, 4) + 1; // tileWth 4: bit_width
    info->tile_width   = (info->tile_width == 1) ? 0 : info->tile_width;
    info->pixel_format = vau_block_htc_strmrdbit(&bs, 2);     // pixFmt 2: bit_width
    td_u32 pix_bits   = (info->pixel_format == 0) ? 24 : ((info->pixel_format == 1) ? 32 : 16); // 16 24 32: val
    info->pixel_format = vau_trans_cmp_pixel_fmt(info->pixel_format);
    td_u32 mode_alpha  = vau_block_htc_strmrdbit(&bs, 3);     // mode_alpha 3: bit_width
    vau_block_htc_strmrdbit(&bs, 3);                          // mode_rgb 3: bit_width
    td_u32 consta = vau_block_htc_strmrdbit(&bs, BYTE_SIZE);  // CONST_A
    info->width += (vau_block_htc_strmrdbit(&bs, 6) << 10); // 6 10: bits
    info->height += (vau_block_htc_strmrdbit(&bs, 6) << 10); // 6 10: bits
    if (mode_alpha == 3) {                                    // 3: mode_alpha
        if (consta == 0) {
            info->alpha_value = 1;
        } else if (consta == 255) {  // 255: consta value
            info->alpha_value = 2; // 2: alpha_value
        } else {
            info->alpha_value = 0;
        }
    } else if (mode_alpha == 4) { // 4: mode_alpha
        info->alpha_value = 3;  // 3: alpha_value
    } else {
        info->alpha_value = 0;
    }

    if (info->tile_width == 0) {
        info->stride = (info->width + 15) / 16 * 16 * pix_bits / BYTE_SIZE; // 15 16: algin val
    } else {
        info->stride = (td_u32)(((td_s32)info->width + info->tile_width - 1) / info->tile_width) *
                        info->tile_size * 64 / BYTE_SIZE; // byte 64:  algin width
    }

    // not support ARGB8565
    if (info->pixel_format == EXT_GFX_FMT_ARGB8565) {
        gpu_err("!!! Error pixel_format\n");
    }
    // for ES
    info->alpha_bypass = 0;
}

STATIC td_void vau_parse_cmp_header_cs_v2(vau_block_htc_bs *head, ext_vau_cmp_header *info)
{
    vau_block_htc_bs bs;
    bs.cur_buf_ptr = head->cur_buf_ptr;
    bs.left_bits = head->left_bits;
    vau_block_htc_strmrdbit(&bs, 1);                          // reserved 1: bit_width
    info->width        = vau_block_htc_strmrdbit(&bs, 2) << 10; // pixFmt 2: bit_width FrmWth 10: bit_width
    info->height       = vau_block_htc_strmrdbit(&bs, 2) << 10; // pixFmt 2: bit_width FrmWth 10: bit_width

    info->tile_size    = vau_block_htc_strmrdbit(&bs, 6);     // tile_size 6: bit_width
    info->width       |= vau_block_htc_strmrdbit(&bs, 10);    // FrmWth 10: bit_width
    info->height      |= vau_block_htc_strmrdbit(&bs, 10);    // FrmHgt 10: bit_width
    info->tile_width   = (td_s32)vau_block_htc_strmrdbit(&bs, 4) + 1; // tileWth 4: bit_width
    info->tile_width   = (info->tile_width == 1) ? 0 : info->tile_width;
    info->pixel_format = vau_block_htc_strmrdbit(&bs, 2);     // pixFmt 2: bit_width
    info->pixel_format = vau_trans_cmp_pixel_fmt(info->pixel_format);
    td_u32 mode_alpha  = vau_block_htc_strmrdbit(&bs, 3);     // mode_alpha 3: bit_width
    td_u32 mode_rgb    = vau_block_htc_strmrdbit(&bs, 3);     // mode_rgb 3: bit_width
    vau_block_htc_strmrdbit(&bs, BYTE_SIZE);  // CONST_A

    if (mode_alpha == 3 && mode_rgb == 3) { // 3 alpha_value and rgb mode
        gpu_err("!!!unsupported mode\n");
        return;
    }

    if (mode_alpha == 4) { // 4: alpha mode
        info->alpha_value = 3; // 3: alpha_value
    } else {
        info->alpha_value = 0;
    }

    /* in 32bits only if (mode_rgb == 5) for rgb or ((mode_alpha == 1 || mode_alpha == 3) && mode_rgb == 5)
       for argb is true, otherwise in 64bits */
    td_u32 is_rgb_96         = ((info->pixel_format == EXT_GFX_FMT_RGB888 ||
                               info->pixel_format == EXT_GFX_FMT_RGB565) &&
                               mode_rgb == 5) ? 1 : 0;
    td_u32 is_argb_merge_96  = ((info->pixel_format == EXT_GFX_FMT_ARGB8888 ||
                               info->pixel_format == EXT_GFX_FMT_ARGB8565) &&
                               mode_alpha == 1 && mode_rgb == 5) ? 1 : 0;
    td_u32 is_argb_aconst_96 = ((info->pixel_format == EXT_GFX_FMT_ARGB8888 ||
                               info->pixel_format == EXT_GFX_FMT_ARGB8565) &&
                               mode_alpha == 3 && mode_rgb == 5) ? 1 : 0;
    td_u32 tile_align_unit   = (is_rgb_96 || is_argb_merge_96 || is_argb_aconst_96) ? 64 / 2 : 64;

    td_u32 pix_bits = ((info->pixel_format == EXT_GFX_FMT_RGB888) || (info->pixel_format == EXT_GFX_FMT_ARGB8565)) ?
                      24 : ((info->pixel_format == EXT_GFX_FMT_ARGB8888) ? 32 : 16); // 16 24 32: val
    if (info->tile_width == 0) {
        info->stride = (info->width + 15) / 16 * 16 * pix_bits / BYTE_SIZE; // 15 16: algin val
    } else {
        info->stride = (td_u32)(((td_s32)info->width + info->tile_width - 1) / info->tile_width) *
                       info->tile_size * tile_align_unit;
        info->stride = (info->stride + 64 - 1) / 64 * 64 / BYTE_SIZE; // byte 64:  algin width
    }
    // for ES
    info->alpha_bypass = 0;
}

STATIC td_s32 vau_parse_cmp_header_standard_astc_cs(vau_block_htc_bs *head, ext_vau_cmp_header *info)
{
    vau_block_htc_bs bs;
    bs.cur_buf_ptr = head->cur_buf_ptr;
    bs.left_bits = head->left_bits;
    vau_block_htc_strmflushbits(&bs, 32); // 32: bit_width
    td_u8 block_x = (td_u8)vau_block_htc_strmrdbit(&bs, BYTE_SIZE);
    td_u8 block_y = (td_u8)vau_block_htc_strmrdbit(&bs, BYTE_SIZE);
    td_u8 block_z = (td_u8)vau_block_htc_strmrdbit(&bs, BYTE_SIZE);
    td_u32 dim_x = (vau_block_htc_strmrdbit(&bs, BYTE_SIZE) << 0) +
        (vau_block_htc_strmrdbit(&bs, BYTE_SIZE) << BYTE_SIZE) +
        (vau_block_htc_strmrdbit(&bs, BYTE_SIZE) << 16); // 16: bit_width
    td_u32 dim_y = (vau_block_htc_strmrdbit(&bs, BYTE_SIZE) << 0) +
        (vau_block_htc_strmrdbit(&bs, BYTE_SIZE) << BYTE_SIZE) +
        (vau_block_htc_strmrdbit(&bs, BYTE_SIZE) << 16); // 16: bit_width
    td_u32 dim_z = (vau_block_htc_strmrdbit(&bs, BYTE_SIZE) << 0) +
        (vau_block_htc_strmrdbit(&bs, BYTE_SIZE) << BYTE_SIZE) +
        (vau_block_htc_strmrdbit(&bs, BYTE_SIZE) << 16); // 16: bit_width
                                                         // 4: block_y size 4: block_x size
    if (block_y == 4 && block_x == 4 && dim_z == 1 && block_z == 1) {
        // for CS
        info->tile_size    = 2; // 2: tile_size
        info->width        = dim_x;
        info->height       = dim_y;
        info->pixel_format = EXT_GFX_FMT_ARGB8888;
        info->alpha_value  = 0;
        info->tile_width   = 4; // 4: tile_width
        info->stride = (td_u32)(((td_s32)info->width + info->tile_width - 1) / info->tile_width) *
                       info->tile_size * 64 / BYTE_SIZE; // byte 64: algin width
        info->alpha_bypass = 0; // for ES
    } else {
        gpu_err("!!!unsupported ASTC bitstream, only 2D 4x4 is supported!!!\n");
        return -1;
    }
    return 1;
}

td_s32 vau_parse_cmp_header(td_u8 *head, td_u32 size, ext_vau_cmp_header *info)
{
    vau_block_htc_bs bs;

    uapi_unused(size);

    vau_block_htc_strminit(&bs, head);
    td_u32 magic_id;
    magic_id = vau_block_htc_strmshowbits(&bs, 32); // 32: bit_width
    if (magic_id == 0x5CA1AB13) { // standard ASTC   for CS
        td_s32 flag = 0;
        flag = vau_parse_cmp_header_standard_astc_cs(&bs, info);
        if (flag == (td_s32)EXT_FAILURE) {
            return EXT_FAILURE;
        }
    } else if ((magic_id & 0x01ff) == 0x01ff) {          // "HTC"
        vau_block_htc_strmflushbits(&bs, 9);             // 9: bit_width
        info->version = vau_block_htc_strmrdbit(&bs, 4); // info->version 4: bit_width
        if (info->version == 0) {                        // 0: ES
            vau_parse_cmp_header_es(&bs, info);
        } else if (info->version == 1) {                 // 1: v1 v2
            vau_parse_cmp_header_cs_v1(&bs, info);
        } else if (info->version == 2) {                 // 2: v3
            vau_parse_cmp_header_cs_v2(&bs, info);
        } else {
            gpu_err("!!!unsupported HTC info->version!!!\n");
            return EXT_FAILURE;
        }
    } else {
        gpu_err("!!!unrecognized bitstream!!!\n");
        return EXT_FAILURE;
    }

    if (vau_check_cmp_info(info) != TD_TRUE) {
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}
