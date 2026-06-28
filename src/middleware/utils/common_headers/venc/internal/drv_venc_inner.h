/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description:
 * Author: Hisilicon multimedia software group
 * Create: 2025/05/15
 * Notes:
 * History:
 */
#ifndef DRV_VENC_INNER_H
#define DRV_VENC_INNER_H

#include "soc_venc_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef td_s32 fn_ext_venc_online_finish(td_handle src_handle, td_u32 state);

typedef struct {
    td_handle src_handle;
    fn_ext_venc_online_finish *pfn_venc_online_finish;
} ext_venc_online_callback;

td_s32 uapi_venc_inner_send_task(td_handle handle, const ext_venc_task *task);
td_s32 uapi_venc_inner_online_reset(td_handle handle);
td_s32 uapi_venc_inner_online_register(td_handle handle, ext_venc_online_callback *callback);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
