/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "stm_intf.h"
#include "stm_base.h"
#include "stm_ctrl.h"

#include "dbg.h"

td_s32 stm_get_mem_need_size(vfmw_svr_mem_need *mem)
{
    td_s32 ret;
    stm_ops *ops = TD_NULL;

    ops = stm_ctrl_get_ops();
    ret = ops->get_mem_need_size(mem);

    return ret;
}

td_s32 stm_open(td_void)
{
    td_s32 ret;
    stm_ops *ops = TD_NULL;

    ops = stm_ctrl_get_ops();
    ret = ops->open();

    return ret;
}

td_s32 stm_close(td_void)
{
    td_s32 ret;
    stm_ops *ops = TD_NULL;

    ops = stm_ctrl_get_ops();
    ret = ops->close();

    return ret;
}

td_s32 stm_suspend(const td_void *opt)
{
    td_s32 ret;
    stm_ops *ops = TD_NULL;

    vfmw_unused(opt);

    ops = stm_ctrl_get_ops();
    ret = ops->suspend();

    return ret;
}

td_s32 stm_resume(const td_void *opt)
{
    td_s32 ret;
    stm_ops *ops = TD_NULL;

    vfmw_unused(opt);

    ops = stm_ctrl_get_ops();
    ret = ops->resume();

    return ret;
}

td_s32 stm_wake_up(td_void *opt)
{
    td_s32 ret;
    stm_ops *ops = TD_NULL;

    vfmw_unused(opt);

    ops = stm_ctrl_get_ops();

    ret = ops->wakeup();

    return ret;
}

td_s32 stm_recvive_seg(td_s32 chan_id, stm_seg_info *seg)
{
    td_s32 ret;
    stm_ops *ops = TD_NULL;

    ops = stm_ctrl_get_ops();

    ret = ops->receive_seg(chan_id, seg);

    return ret;
}

td_s32 stm_release_seg(td_s32 chan_id, td_s32 seg_id)
{
    td_s32 ret;
    stm_ops *ops = TD_NULL;

    ops = stm_ctrl_get_ops();

    ret = ops->release_seg(chan_id, seg_id);

    return ret;
}

td_s32 stm_set_ref_cnt(td_s32 chan_id, td_s32 seg_id, td_s32 ref_cnt)
{
    td_s32 ret;
    stm_ops *ops = TD_NULL;

    ops = stm_ctrl_get_ops();

    ret = ops->set_ref_cnt(chan_id, seg_id, ref_cnt);

    return ret;
}

td_s32 stm_control(td_s32 chan_id, td_s32 cmd, td_void *arg)
{
    td_s32 ret;
    stm_ops *ops = TD_NULL;

    ops = stm_ctrl_get_ops();
    ret = ops->control(chan_id, cmd, arg);

    return ret;
}

td_s32 stm_process(td_void)
{
    return stm_ctrl_process();
}
