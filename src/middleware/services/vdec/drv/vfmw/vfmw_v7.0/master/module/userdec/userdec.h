/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __VFMW_USER_H__
#define __VFMW_USER_H__

#include "vfmw.h"
#include "vfmw_proc.h"

td_s32 user_open(td_void);
td_s32 user_close(td_void);
td_s32 user_init(td_void *args, td_u32 len);
td_s32 user_control(td_s32 chan_id, td_s32 cmd, td_void *args, td_u32 len);
td_s32 user_get_image(td_s32 chan_id, td_void *user_image);
td_s32 user_release_image(td_s32 chan_id, const td_void *user_image);
td_s32 user_info_read(td_void *buf);
td_s32 user_info_write(cmd_str_ptr buffer, td_u32 count);
td_s32 user_set_callback(vfmw_adpt_type type, fun_vfmw_event_report fp_event_report);

#endif
