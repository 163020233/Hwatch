/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description:
 * Author: Hisilicon multimedia software group
 * Create: 2025/05/15
 * Notes:
 * History:
 */
#ifndef DRV_VENC_INTF_H
#define DRV_VENC_INTF_H

#include "td_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_s32  uapi_venc_module_init(td_void);
td_void uapi_venc_module_exit(td_void);

td_s32  uapi_venc_register_ipc(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
