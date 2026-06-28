/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "vfmw_img.h"
#include "dbg.h"
#include "pts_ext.h"

/* ----------------------------EXTERNAL FUNCTION------------------------------- */
td_void img_info_convert(td_s32 chan_id, vfmw_image *image)
{
    td_s32 ret;

#ifdef VFMW_HDR_SUPPORT
    ret = hdr_info_convert(chan_id, image);
    if (ret != EXT_SUCCESS) {
        dprint(PRN_ERROR, "%s convert hdr info failed!\n", __func__);
    }
#endif

    ret = pts_info_convert(chan_id, image);
    if (ret != EXT_SUCCESS) {
        dprint(PRN_ERROR, "%s convert pts info failed!\n", __func__);
    }

    return;
}

td_void img_info_init(td_void)
{
#ifdef VFMW_HDR_SUPPORT
    hdr_info_init();
#endif
    pts_info_init();

    return;
}

td_void img_info_exit(td_void)
{
#ifdef VFMW_HDR_SUPPORT
    hdr_info_exit();
#endif
    pts_info_exit();

    return;
}

td_void img_info_destroy(td_s32 chan_id)
{
    vfmw_unused(chan_id);
#ifdef VFMW_HDR_SUPPORT
    hdr_info_destroy(chan_id);
#endif
    return;
}