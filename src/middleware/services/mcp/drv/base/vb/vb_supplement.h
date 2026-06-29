/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: vb supplement header file
 * Author: AuthorNameMagicTag
 * Create: 2021-08-10
 */


#ifndef __VB_SUPPLEMENT_H__
#define __VB_SUPPLEMENT_H__

#include "drv_mcp_dependency.h"
#include "vb_comm.h"
#include "drv_ioctl_vb.h"

td_s32 vb_check_supplement(const ext_vb_supplement_config *conf);

td_u32 vb_get_supplement_size(ext_vb_supplement_config *conf, td_bool cached);
td_void vb_value_supplement_cached_addr(td_u64 phy, td_void *vir, ext_isp_video_supplement *supplement,
    ext_vb_supplement_config *conf);

td_void vb_value_supplement_no_cache_addr(td_u64 phy, td_void *vir, ext_isp_video_supplement *supplement,
    ext_vb_supplement_config *conf);
#endif
