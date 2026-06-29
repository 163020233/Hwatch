/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __PDT_COM_H__
#define __PDT_COM_H__

#include "vfmw_pdt.h"
#include "vfmw_sys.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

vdh_glb_reg_info *pdt_get_glb_reg(td_void);
td_s32 pdt_get_seg_buf_size(const td_void *args);
td_s32 pdt_get_dec_cap(vfmw_cap *cap);
td_s32 pdt_calc_seg_buf_size(td_s32 chan_id);
td_void pdt_get_max_width_height(td_s32 chan_id, td_u32 *dec_max_width, td_u32 *dec_max_height);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
