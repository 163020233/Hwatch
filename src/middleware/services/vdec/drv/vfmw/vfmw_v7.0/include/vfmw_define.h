/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __VFMW_DEFINE_H__ /*  #ifdef VFMW_BINARY_SUPPORT should be exclusive to v5 */
#define __VFMW_DEFINE_H__

#define H264_ENABLE
#define MVC_ENABLE
#define MPEG2_ENABLE
#define AVS_ENABLE
#define AVS2_ENABLE
#define MPEG4_ENABLE
#define REAL8_ENABLE
#define REAL9_ENABLE
#define VC1_ENABLE
#define DIVX3_ENABLE
#define VP6_ENABLE
#define VP8_ENABLE
#define HEVC_ENABLE
#define VP9_ENABLE

#define VFMW_INT_MAX  ((td_s32)(~0U >> 1))
#define VFMW_INT_MIN  (-VFMW_INT_MAX - 1)
#define VFMW_UINT_MAX (~0U)
#define VFMW_INT16_MAX  32767
#define VFMW_INT16_MIN  (-VFMW_INT16_MAX - 1)

#define vfmw_max(a, b)       (((a) < (b)) ? (b) : (a))
#define vfmw_min(a, b)       ((((a) > (b))) ? (b) : (a))
#define vfmw_abs(x)          (((x) < 0) ? -(x) : (x))
#define vfmw_mdian(a, b, c) ((a) + (b) + (c)-vfmw_min((a), vfmw_min((b), (c))) - vfmw_max((a), vfmw_max((b), (c))))

#define vfmw_clip_1(high, x)      (vfmw_max(vfmw_min((x), high), 0))
#define vfmw_clip_3(low, high, x) (vfmw_max(vfmw_min((x), high), low))
#define vfmw_clip_255(x)          (vfmw_max(vfmw_min((x), 255), 0))

#define vfmw_align_up(val, align) (((val) + (align) - 1) & (~((align) - 1)))
#define vfmw_align_dn(val, align) ((val) & (~((align) - 1)))

#define swap_xor(a, b) do {  \
        (a) ^= (b);    \
        (b) ^= (a);    \
        (a) ^= (b);    \
} while (0)

#define check_sint_plus_overflow(a, b) (((((b) > 0) && ((a) > (VFMW_INT_MAX - (b))))    \
                                        || (((b) < 0) && ((a) < (VFMW_INT_MIN - (b))))) ? 1 : 0)

#define check_uint_plus_overflow(a, b) (((VFMW_UINT_MAX - (a)) < (b)) ? 1 : 0)

#define vfmw_last_img_id(x) ((x) + 2)

#define ADDR_ALIGN_16_MASK (~((td_u64)0xF))

#endif /*   __PUBLIC_H__  */

