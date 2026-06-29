/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: source file for drv sync
 * Author: audio
 */

#include "securec.h"
#include "audio_osal.h"
#include "audio_debug.h"
#include "drv_proc_sync.h"
#include "drv_msg_sync.h"
#include "drv_sync.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SAP_PROC_SUPPORT
td_s32 sync_read_proc(td_void *file, td_void *private)
{
    td_s32 ret;
    sync_proc_item *proc = TD_NULL;

    audio_unused(private);

    proc = (sync_proc_item *)audio_osal_kmalloc(sizeof(sync_proc_item));
    if (proc == TD_NULL) {
        return ERR_SYS_MEM_ALLOC;
    }

    (td_void)memset_s(proc, sizeof(*proc), 0, sizeof(*proc));

    ret = drv_msg_sync_get_proc_info(0, proc);
    if (ret != AUDIO_SUCCESS) {
        audio_osal_kfree(proc);
        sap_err_log_fun(drv_msg_sync_get_proc_info, ret);
        return ret;
    }

    ret = drv_proc_sync_show(file, proc);
    if (ret != AUDIO_SUCCESS) {
        audio_osal_kfree(proc);
        sap_err_log_fun(drv_proc_sync_show, ret);
        return ret;
    }

    audio_osal_kfree(proc);
    return AUDIO_SUCCESS;
}
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */
