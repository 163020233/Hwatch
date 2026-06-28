/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __BITSTREAM_HEAD__
#define __BITSTREAM_HEAD__

#include "vfmw.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifndef VFMW_BIG_ENDIAN
#define endian32_(x) (((x) << 24) | (((x) & 0x0000ff00) << 8) | (((x) & 0x00ff0000) >> 8) | (((x) >> 24) & 0x000000ff))
#else
#define endian32_(x) (x)
#endif
#define ENDIAN32 endian32_

typedef struct {
    td_u32 is_sec : 1;
    td_u32 reserved : 31;
} bs_ops;

typedef struct {
    td_u8 *head;
    td_u8 *tail;
    td_u32 buf_a;
    td_u32 buf_b;
    td_s32 bs_len;
    td_s32 buf_pos;
    td_u32 total_pos;
    td_s32 history_pos;
    bs_ops ops;
} bs_info;

td_void bs_init(bs_info *bs, const td_u8 *input, td_s32 length);
td_s32 bs_get(bs_info *bs, td_s32 nbits);
td_s32 bs_show(const bs_info *bs, td_s32 nbits);
td_s32 bs_pos(const bs_info *bs);
td_s32 bs_skip(bs_info *bs, td_s32 nbits);
td_u32 bs_back(bs_info *bs, td_u32 nbits);
td_s32 bs_next_byte(const bs_info *bs);
td_s32 bs_skip_to_next_byte(bs_info *bs);
td_s32 bs_residual_bits(const bs_info *bs);
td_s32 bs_is_byte_aligned(const bs_info *bs);
td_s32 bs_next_bits_byte_aligned(bs_info *bs, td_s32 nbits);
td_s32 bs_long_skip(bs_info *bs, td_s32 nbits);
td_s32 bs_skip_without_count(bs_info *bs, td_s32 nbits);
td_u8 *bs_get_next_byte_ptr(const bs_info *bs);
td_u32 bs_zero_ms_32(td_u32 data);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
