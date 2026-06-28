/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "vfmw_intf.h"
#include "dbg.h"
#include "vfmw_osal.h"
#include "vfmw_adpt.h"

vfmw_export_fun g_vfmw_export_funcs = {
    .fn_vfmw_mod_init = vfmw_mod_init,
    .fn_vfmw_mod_exit = vfmw_mod_exit,
    .fn_vfmw_init = vfmw_init,
    .fn_vfmw_exit = vfmw_exit,
    .fn_vfmw_suspend = vfmw_suspend,
    .fn_vfmw_resume = vfmw_resume,
    .fn_vfmw_get_frame = vfmw_get_image,
    .fn_vfmw_rels_frame = vfmw_release_image,
    .fn_vfmw_control = vfmw_control,
};

td_s32 vfmw_init(td_void *args)
{
    td_s32 ret;

    ret = vadpt_init(args);

    return ret;
}

td_s32 vfmw_exit(td_void *args)
{
    td_s32 ret;

    ret = vadpt_exit(args);

    return ret;
}

td_s32 vfmw_suspend(td_void *args)
{
    td_s32 ret;

    ret = vadpt_suspend(args);

    return ret;
}

td_s32 vfmw_resume(td_void *args)
{
    td_s32 ret;

    ret = vadpt_resume(args);

    return ret;
}

td_s32 vfmw_get_image(td_s32 chan_id, td_void *image, td_void *ext_frm_info)
{
    td_s32 ret;

    ret = vadpt_get_frame(chan_id, image, ext_frm_info);

    return ret;
}

td_s32 vfmw_release_image(td_s32 chan_id, const td_void *image)
{
    td_s32 ret;

    ret = vadpt_release_frame(chan_id, image);

    return ret;
}

td_s32 vfmw_control(td_s32 chan_id, td_s32 cmd, td_void *args, td_u32 len)
{
    td_s32 ret;

    ret = vadpt_control(chan_id, cmd, args, len);

    return ret;
}

/* os will check if Init */
td_s32 vfmw_mod_init(td_void)
{
    td_s32 ret;

    ret = vadpt_open();

    return ret;
}

td_void vfmw_mod_exit(td_void)
{
    vadpt_close();

    return;
}
