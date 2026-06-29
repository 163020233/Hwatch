/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description:the header file of ppu intf
 * Author: AuthorNameMagicTag
 * Create: 2025-06-30
 */

#ifndef __VIU_IOCTL_H__
#define __VIU_IOCTL_H__

#include "drv_mcp_dependency.h"

td_u32 viu_drv_get_status(td_void);
td_u32 viu_drv_get_open_cnt(td_void);

td_s32 viu_ioctl_drv_init(td_u32 cmd, td_void *arg, td_void *priv_data);
td_s32 viu_ioctl_drv_deinit(td_u32 cmd, td_void *arg, td_void *priv_data);
td_s32 viu_ioctl_dev_set_attr(td_u32 cmd, td_void *arg, td_void *priv_data);
td_s32 viu_ioctl_dev_get_attr(td_u32 cmd, td_void *arg, td_void *priv_data);
td_s32 viu_ioctl_dev_enable(td_u32 cmd, td_void *arg, td_void *priv_data);
td_s32 viu_ioctl_dev_disable(td_u32 cmd, td_void *arg, td_void *priv_data);

td_s32 viu_ioctl_dev_attach_pipe(td_u32 cmd, td_void *arg, td_void *priv_data);
td_s32 viu_ioctl_dev_detach_pipe(td_u32 cmd, td_void *arg, td_void *priv_data);
td_s32 viu_ioctl_set_dev_timing_attr(td_u32 cmd, td_void *arg, td_void *priv_data);
td_s32 viu_ioctl_get_dev_timing_attr(td_u32 cmd, td_void *arg, td_void *priv_data);

td_s32 viu_ioctl_pipe_create(td_u32 cmd, td_void *arg, td_void *priv_data);
td_s32 viu_ioctl_pipe_destroy(td_u32 cmd, td_void *arg, td_void *priv_data);
td_s32 viu_ioctl_pipe_set_attr(td_u32 cmd, td_void *arg, td_void *priv_data);
td_s32 viu_ioctl_pipe_get_attr(td_u32 cmd, td_void *arg, td_void *priv_data);
td_s32 viu_ioctl_pipe_start(td_u32 cmd, td_void *arg, td_void *priv_data);
td_s32 viu_ioctl_pipe_stop(td_u32 cmd, td_void *arg, td_void *priv_data);

td_s32 viu_ioctl_pipe_set_work_mode(td_u32 cmd, td_void *arg, td_void *priv_data);
td_s32 viu_ioctl_pipe_get_work_mode(td_u32 cmd, td_void *arg, td_void *priv_data);

td_s32 viu_ioctl_set_pipe_dump_attr(td_u32 cmd, td_void *arg, td_void *priv_data);
td_s32 viu_ioctl_get_pipe_dump_attr(td_u32 cmd, td_void *arg, td_void *priv_data);
td_s32 viu_ioctl_get_pipe_raw(td_u32 cmd, td_void *arg, td_void *priv_data);

td_s32 viu_ioctl_send_mult_pipe_raw(td_u32 cmd, td_void *arg, td_void *priv_data);
td_s32 viu_ioctl_chn_attach(td_u32 cmd, td_void *arg, td_void *priv_data);
td_s32 viu_ioctl_chn_detach(td_u32 cmd, td_void *arg, td_void *priv_data);

td_s32 viu_ioctl_get_chn_frame(td_u32 cmd, td_void *arg, td_void *priv_data);
td_s32 viu_ioctl_mod_release_frame(td_u32 cmd, td_void *arg, td_void *priv_data);

#endif /* __VIU_IOCTL_H__ */
