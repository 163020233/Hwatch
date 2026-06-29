/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __IMG_INFO_H__
#define __IMG_INFO_H__

#include "vfmw.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ----------------------------EXTERNAL FUNCTION------------------------------- */
td_void img_info_init(td_void);
td_void img_info_exit(td_void);
td_void img_info_convert(td_s32 chan_id, vfmw_image *image);
td_void img_info_destroy(td_s32 chan_id);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
