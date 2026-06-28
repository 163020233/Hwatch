/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: viu module header
 * Author: AuthorNameMagicTag
 * Create: 2025-06-30
 */

#ifndef __VIU_DEV__
#define __VIU_DEV__

#include "drv_viu_struct.h"

#define viu_dev_check_id_return_val(dev_id)                 \
do {                                                        \
    td_s32 ret_tmp = viu_dev_check_id(dev_id);            \
    if (ret_tmp != TD_SUCCESS) {                          \
        return ret_tmp;                                     \
    }                                                       \
} while (0)

td_s32 viu_dev_check_id(td_s32 dev_id);
td_void viu_dev_mkp_ctx_global_init(td_void);
td_bool viu_dev_check_configed(td_void);

td_s32 viu_dev_set_attr(td_s32 dev_id, const ext_viu_dev_attr *dev_attr);
td_s32 viu_dev_get_attr(td_s32 vdev_id, ext_viu_dev_attr *dev_attr);

td_s32 viu_dev_enable(td_s32 dev_id, td_void *unique);
td_s32 viu_dev_disable(td_s32 dev_id, td_void *unique);

td_s32 viu_dev_attach_pipe(td_u8 dev_id, td_u8 pipe_id);
td_s32 viu_dev_detach_pipe(td_u8 dev_id, td_u8 pipe_id);

td_s32 viu_dev_set_bind_pipe(td_s32 dev_id, const ext_viu_dev_bind_pipe *dev_bind_pipe);
td_s32 viu_dev_get_bind_pipe(td_s32 dev_id, ext_viu_dev_bind_pipe *dev_bind_pipe);

#endif /* __VIU_DEV__ */

