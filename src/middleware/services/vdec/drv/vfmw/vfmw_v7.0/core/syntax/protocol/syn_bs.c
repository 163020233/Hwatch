/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "syn_bs.h"
#include "vfmw_osal.h"
#include "dbg.h"

#ifdef VT_SUPPORT
#include "vt_ioctl.h"
#endif

static td_u8 g_bs_clac_zero_num[256] = { /* 256 :a number */
    8, 7, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

VFMW_ACC_BASE td_u32 bs_zero_ms_32(td_u32 data)
{
    td_u32 i;
    td_u32 zero_num = 0;
    td_u32 tmp_zero_num;
    td_u32 tmp_data;
    td_u32 data_in = data;

    for (i = 4; i > 0; i--) { /* 4 :a number */
        tmp_data = (data_in & 0xff000000) >> 24; /* 24 :a number */
        if (tmp_data >= 256) { /* 256 is a number */
            return 0;
        }
            tmp_zero_num = (td_u32)g_bs_clac_zero_num[tmp_data];
        zero_num += tmp_zero_num;

        if (tmp_zero_num != 8) { /* 8 :a number */
            break;
        }

        data_in = data_in << 8; /* 8 :a number */
    }

    return zero_num;
}

VFMW_ACC_BASE td_void bs_init(bs_info *bs, const td_u8 *input, td_s32 length)
{
    td_u32 data;
    const td_ulong align_word = 0x3;

    vfmw_check(bs != TD_NULL);
    vfmw_check(input != TD_NULL);

    bs->head = (td_u8 *)uint64_ptr(((td_uintptr_t)input) & (~align_word));
    bs->tail = bs->head + 8; /* 8 :a number */
    bs->bs_len = length;

    if (bs->head == TD_NULL) {
        return;
    }

    data = *(td_u32 *)bs->head;
    bs->buf_a = ENDIAN32(data);
    data = *(td_u32 *)(bs->head + 4); /* 4 :a number */
    bs->buf_b = ENDIAN32(data);

    bs->buf_pos = (td_s32)((((td_uintptr_t)input) & 0x3) << 3); /* 3 is bit ofs */
    bs->total_pos = 0;

    return;
}

VFMW_ACC_BASE td_s32 bs_pos(const bs_info *bs)
{
    vfmw_check_ret(bs != TD_NULL, 0);

    return (td_s32)(bs->total_pos);
}

VFMW_ACC_BASE __inline td_s32 bs_get(bs_info *bs, td_s32 nbits)
{
    td_u32 data;

    vfmw_check_ret(bs != TD_NULL, 0);

    data = (td_u32)bs_show(bs, nbits);
    bs_skip(bs, nbits);

    return (td_s32)(data);
}

static __inline td_void bs_rand(td_u32 *data, td_s32 nbits)
{
#ifdef VT_SUPPORT
    if (g_vt_rand_bs_err != 0) {
        if ((os_get_rand() % 100) < g_vt_rand_bs_err) { /* 100 :a number */
            *data = (nbits < 32) ? (os_get_rand() % (1 << nbits)) : os_get_rand(); /* 32, a number */
        }
    }
#else
    vfmw_unused(data);
    vfmw_unused(nbits);
#endif

    return;
}

#define bs_show_assert(bs, nbits, pos) \
    do { \
        if (((bs) == TD_NULL) || (((td_u32)(pos)) >= 0x20) || \
            ((nbits) == 0) || ((nbits) > 0x20)) { \
            return 0; \
        } \
    } while (0)

VFMW_ACC_BASE __inline td_s32 bs_show(const bs_info *bs, td_s32 nbits)
{
    td_u32 data, data1;

    bs_show_assert(bs, nbits, bs->buf_pos);

    data1 = (((td_u32)(bs->buf_pos)) == 0) ? 0 : (bs->buf_b >> (0x20 - ((td_u32)(bs->buf_pos))));
    data = (nbits + bs->buf_pos > 0x20) ?
        ((data1 | (bs->buf_a << ((td_u32)(bs->buf_pos)))) >> ((td_u32)(0x20 - nbits))) :
        ((bs->buf_a << ((td_u32)(bs->buf_pos))) >> (td_u32)(0x20 - nbits));
    bs_rand(&data, nbits);
    return (td_s32)(data);
}

#define bs_skip_marco_process(buf_pos, buf_a, buf_b, tail, nbits) \
    if (((nbits) + (buf_pos)) >= 32) { /* 32 :a number */   \
        (buf_pos) = ((nbits) + (buf_pos)) - 32; /* 32 :a number */ \
        (buf_a) = (buf_b);  \
        (buf_b) = ENDIAN32(*(td_u32 *)(tail));  \
        (tail) += 4; /* 4 :a number */  \
    } else {    \
        (buf_pos) += (nbits);   \
    }   \

VFMW_ACC_BASE __inline td_s32 bs_skip(bs_info *bs, td_s32 nbits)
{
    vfmw_check_ret(bs != TD_NULL, 0);
    vfmw_check_ret(bs->tail != TD_NULL, 0);

    bs->total_pos = bs->total_pos + (td_u32)nbits;
    if ((td_s32)bs->total_pos < (bs->bs_len * 8)) { /* 8 :a number */
        bs_skip_marco_process(bs->buf_pos, bs->buf_a, bs->buf_b, bs->tail, nbits);
    }

    return (nbits);
}

td_u32 bs_back(bs_info *bs, td_u32 nbits)
{
    td_s32 pred_pos;
    td_u32 data1;
    td_u32 nbits_tmp = nbits;

    vfmw_check_ret(bs != TD_NULL, 0);

    if (nbits_tmp > bs->total_pos) {
        nbits_tmp = bs->total_pos;
    }

    pred_pos = bs->buf_pos - (td_s32)nbits_tmp;
    bs->total_pos -= nbits_tmp;

    if (pred_pos >= 0) {
        bs->buf_pos = pred_pos;
    } else {
        bs->tail -= 4; /* 4 :a number */
        bs->buf_b = bs->buf_a;
        vfmw_check_ret(bs->tail - 8 != 0, 0); /* 8 :a number */
        data1 = *(td_u32 *)(bs->tail - 8); /* 8 :a number */
        bs->buf_a = ENDIAN32(data1);

        bs->buf_pos = pred_pos + 32; /* 32 :a number */
    }

    return (nbits_tmp);
}

td_s32 bs_next_byte(const bs_info *bs)
{
    td_s32 skip_len, align_pos;

    vfmw_check_ret(bs != TD_NULL, 0);

    align_pos = (bs->total_pos + 7) & 0xfffffff8; /* 7 :a number */
    skip_len = align_pos - (td_s32)bs->total_pos;

    return skip_len;
}

VFMW_ACC_BASE td_s32 bs_skip_to_next_byte(bs_info *bs)
{
    td_s32 skip_len, align_pos;

    vfmw_check_ret(bs != TD_NULL, 0);

    align_pos = (bs->total_pos + 7) & 0xfffffff8; /* 7 :a number */
    skip_len = align_pos - (td_s32)bs->total_pos;

    bs_skip(bs, skip_len);

    return skip_len;
}

td_s32 bs_residual_bits(const bs_info *bs)
{
    vfmw_check_ret(bs != TD_NULL, 0);

    return (8 * bs->bs_len - (td_s32)bs->total_pos); /* 8 :a number */
}

td_s32 bs_is_byte_aligned(const bs_info *bs)
{
    td_u32 opreator;

    vfmw_check_ret(bs != TD_NULL, 0);

    opreator = (td_u32)(bs->total_pos);

    return ((opreator & 7) != 0) ? 0 : 1; /* 7 :a number */
}

td_s32 bs_next_bits_byte_aligned(bs_info *bs, td_s32 nbits)
{
    td_u32 bits_to_byte, data;
    td_u32 opreator;

    vfmw_check_ret(bs != TD_NULL, 0);

    opreator = (td_u32)(bs->total_pos);

    /* 'nbits' should <= 24, otherwise may be wrong. */
    bits_to_byte = 8 - (opreator & 7); /* 8 :a number 7 :a number */

    bs_skip(bs, (td_s32)bits_to_byte);
    data = (td_u32)bs_show(bs, nbits);
    bs_back(bs, bits_to_byte);

    return (td_s32)(data);
}

/* 该跳跃只保证计数器的准确性,不能保证bufa和bufb中读得有效数据(当发生在包边界时) */
td_s32 bs_long_skip(bs_info *bs, td_s32 nbits)
{
    td_u32 bits, words;
    td_s32 nbits_tmp = nbits;

    if (nbits_tmp == 0 || bs == TD_NULL) {
        return 0;
    }

    if (nbits_tmp <= 64) { /* 64 :a number */
        if (nbits_tmp > 32) { /* 32 :a number */
            bs_skip(bs, 32); /* 32 :a number */
            nbits_tmp -= 32; /* 32 :a number */
        }

        bs_skip(bs, nbits_tmp);
    } else {
        bits = (td_u32)(32 - bs->buf_pos); /* 32 :a number */
        bs->total_pos += bits;
        bs->buf_pos = 0;
        nbits_tmp -= (td_s32)bits;
        words = (td_u32)nbits_tmp / 32; /* 32 :a number */
        bits = (td_u32)nbits_tmp & 0x1f;
        bs->tail += 4 * (words - 1); /* 4 :a number */
        vfmw_check_ret(bs->tail != TD_NULL, 0);
        bs->buf_a = ENDIAN32(*(td_u32 *)bs->tail);
        bs->tail += 4; /* 4 :a number */
        vfmw_check_ret(bs->tail != TD_NULL, 0);
        bs->buf_b = ENDIAN32(*(td_u32 *)bs->tail);
        bs->tail += 4; /* 4 :a number */
        vfmw_check_ret(words < VFMW_MAX_UINT32, 0);
        bs->total_pos += (words * 32); /* 32 :a number */

        if (bits > 0) {
            bs_skip(bs, (td_s32)bits);
        }
    }

    return nbits_tmp;
}

/* 类似于BS_Skip,只是不在totalpos中计数,便于非32位对齐的码流包进行位置调整 */
/* 而调整的bit不记入消耗.因为配置bsinit是按32位对齐来实现的,可能读入了已消耗的bit */
td_s32 bs_skip_without_count(bs_info *bs, td_s32 nbits)
{
    td_s32 ab_pos;
    td_u32 data1;

    vfmw_check_ret(bs != TD_NULL, 0);

    ab_pos = nbits + bs->buf_pos;

    if (ab_pos >= 32) { /* 32 :a number */
        bs->buf_pos = ab_pos - 32; /* 32 :a number */

        bs->buf_a = bs->buf_b;
        if (bs->tail == TD_NULL) {
            return 0;
        }
        data1 = *(td_u32 *)bs->tail;
        bs->buf_b = ENDIAN32(data1);

        bs->tail += 4; /* 4 :a number */
    } else {
        bs->buf_pos += nbits;
    }

    return (nbits);
}

td_u8 *bs_get_next_byte_ptr(const bs_info *bs)
{
    td_u8 *ptr = TD_NULL;
    td_s32 bis_in_bufa;

    vfmw_check_ret(bs != TD_NULL, TD_NULL);

    bis_in_bufa = (32 - bs->buf_pos); /* 32 :a number */
    ptr = ((td_u8 *)(bs->tail)) - (4 + bis_in_bufa / 8); /* 4 :a number 8 :a number */

    return (ptr);
}
