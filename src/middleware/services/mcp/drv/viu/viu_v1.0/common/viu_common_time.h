/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2016-2021. All rights reserved.
 * Description: viu_drv.h
 * Author: AuthorNameMagicTag
 * Create: 2016-11-17
 */

#ifndef VIU_COMMON_TIME_H
#define VIU_COMMON_TIME_H

#include "drv_viu_struct.h"
#include "drv_viu.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_s32 viu_comm_time_get_now_str(td_char *now_time, td_u32 len);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif