/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "local_proc.h"
#include "vctrl_log.h"
#include "liteos_proc.h"

td_s32 local_proc_read(td_s32 module, td_void *p, td_s32 chan_id)
{
    return vctrl_log_read(module, chan_id, p, PROC_READ_BUF_SIZE);
}

td_s32 local_proc_write(td_s32 module, cmd_str_ptr cmd, td_s32 count)
{
    return vctrl_log_write(module, cmd, count);
}
