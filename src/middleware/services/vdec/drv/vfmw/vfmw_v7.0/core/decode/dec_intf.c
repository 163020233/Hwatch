/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "dec_base.h"
#include "dec_intf.h"

td_s32 dec_get_mem_need_size(vfmw_svr_mem_need *mem)
{
    return dec_ctrl_get_mem_need_size(mem);
}

td_s32 dec_open(td_void)
{
    td_s32 ret;

    ret = dec_ctrl_open();

    return ret;
}

td_s32 dec_close(td_void)
{
    td_s32 ret;

    ret = dec_ctrl_close();

    return ret;
}

td_s32 dec_suspend(const td_void *opt)
{
    td_s32 ret;

    vfmw_unused(opt);
    ret = dec_ctrl_suspend();

    return ret;
}

td_s32 dec_resume(const td_void *opt)
{
    td_s32 ret;

    vfmw_unused(opt);
    ret = dec_ctrl_resume();

    return ret;
}

VFMW_ACC_BASE td_s32 dec_wakeup(td_s32 chan_id)
{
    td_s32 ret;

    ret = dec_ctrl_wakeup(chan_id);

    return ret;
}

td_s32 dec_control(td_s32 chan_id, td_u32 cmd, td_void *args)
{
    td_s32 ret;

    ret = dec_ctrl_control(chan_id, cmd, args);

    return ret;
}

td_s32 dec_process(td_void)
{
    td_s32 ret;

    ret = dec_ctrl_process();

    return ret;
}
