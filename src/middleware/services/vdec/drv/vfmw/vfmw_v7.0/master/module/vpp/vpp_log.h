/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
* Description: decoder
* Author: AuthorNameMagicTag
* Create: 2019-04-22
 */

#ifndef __VPP_LOG_H__
#define __VPP_LOG_H__

#include "vfmw_proc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
td_s32 vpp_log_read(td_void *buf, td_s32 len, td_s32 *used);
td_s32 vpp_log_write(cmd_str_ptr buffer, td_u32 count);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif