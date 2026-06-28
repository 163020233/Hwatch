/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: header file for profile module
 * Author: audio
 */

#ifndef __COMMON_PROFILE_H__
#define __COMMON_PROFILE_H__

#include "audio_type.h"
#include "sys_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define PROFILE_TAG_LEN 16

#ifdef SAP_MEM_STATISTIC
td_void mem_stat_add_block(td_void *addr, td_u32 size, const td_char *tag);
td_void mem_stat_remove_block(td_void *addr);
#else
#define mem_stat_add_block(fmt...)
#define mem_stat_remove_block(fmt...)
#endif /* SAP_MEM_STATISTIC */

#ifdef SAP_MCPS_STATISTIC
td_void cps_stat_period_proc(td_u32 total_cps, td_u32 interval_ms);
td_void cps_stat_idle_start(td_void);
td_void cps_stat_idle_stop(td_void);
td_void cps_stat_msg_start(td_void);
td_void cps_stat_msg_stop(td_void);
td_void cps_stat_driver_start(td_void);
td_void cps_stat_driver_stop(td_void);
#else
#define cps_stat_period_proc(fmt...)
#define cps_stat_idle_start()
#define cps_stat_idle_stop()
#define cps_stat_msg_start()
#define cps_stat_msg_stop()
#define cps_stat_driver_start()
#define cps_stat_driver_stop()
#endif /* SAP_MCPS_STATISTIC */

td_void cps_stat_query_total(sys_proc_item *proc);
td_void mem_stat_query_detail(sys_proc_item *proc);
td_void shm_stat_query_detail(sys_proc_item *proc);

td_s32 common_profile_init(td_void);
td_void common_profile_deinit(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
