/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef SM_LOG_H
#define SM_LOG_H

#include "vfmw.h"
#include "vfmw_proc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_s32 stm_log_read(td_void *buf, td_s32 chan_id, td_s32 len);
td_s32 stm_log_write(cmd_str_ptr buffer, td_u32 count);
td_s32 stm_log_need_save_seek(td_void);
td_void stm_log_start_save_raw(td_s32 chan_id);
td_void stm_log_stop_save_raw(td_s32 chan_id);
td_void stm_log_save_raw(td_s32 chan_id, td_void *raw_packet);
td_void stm_log_save_seg(td_s32 chan_id, const td_void *seg_packet);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* SM_LOG_H */
