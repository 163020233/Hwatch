/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: hal_ip of crg
 * Author: AuthorNameMagicTag
 * Create: 2025-05-29
 */

#include "drv_viu_struct.h"

#include "hal_adapt_comm.h"
#include "hal_capability_intf.h"
#include "viu_public.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* #ifdef __cplusplus */

td_void hal_viproc_prepare(viu_proc_id pipe_id)
{
    viu_unused(pipe_id);

    return;
}

td_void hal_viproc_unprepare(viu_proc_id pipe_id)
{
    viu_unused(pipe_id);

    return;
}

td_s32 hal_viproc_init(viu_proc_id proc_id)
{
    viu_unused(proc_id);

    return 0;
}

td_void hal_viproc_deinit(viu_proc_id proc_id)
{
    viu_unused(proc_id);

    return;
}

td_void hal_viproc_start(td_s32 pipe_id, td_bool vicap_online)
{
    viu_unused(pipe_id);
    viu_unused(vicap_online);

    return;
}

td_void hal_viproc_stop(td_s32 pipe_id, td_bool vicap_online)
{
    viu_unused(pipe_id);
    viu_unused(vicap_online);

    return;
}

td_void hal_viproc_work_mode(viu_proc_id proc_id, td_bool online)
{
    viu_unused(proc_id);
    viu_unused(online);

    return;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */