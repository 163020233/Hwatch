/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __SYN_BP_H__
#define __SYN_BP_H__

#include "vfmw_define.h"
#include "vc1.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define BPD_SIZE (BPD_PLANE_SIZE * 7 + 512)

td_s32 bpd_open(td_void);
td_void bpd_close(td_void);
td_s32 bpd_process(bs_info *p_bs, const vc1_codec_info *info, vc1_syntax *syntax, td_s32 chan_id);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
