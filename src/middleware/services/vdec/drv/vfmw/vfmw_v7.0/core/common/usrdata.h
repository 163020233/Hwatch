/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __USRDAT_H__
#define __USRDAT_H__

#include "vfmw.h"
#include "dbg.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define USD_OK  0
#define USD_ERR (-1)

#define MAX_USERDATA_NUM 4

td_s32 usd_open(td_void);
td_s32 usd_close(td_void);
td_s32 usd_create(td_s32 chan_id);
td_s32 usd_destroy(td_s32 chan_id);
td_s32 usd_reset(td_s32 chan_id);
vfmw_usrdat *usd_get(td_s32 chan_id);
td_void usd_release(td_s32 chan_id, td_void *usrdata);
td_s32 usd_get_mem_need_size(vfmw_svr_mem_need *mem);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif