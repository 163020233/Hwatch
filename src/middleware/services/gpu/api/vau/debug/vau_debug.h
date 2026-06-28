/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description : vau debug file
 */

#ifndef API_VAU_DEBUG_H
#define API_VAU_DEBUG_H

#include "gpu_dfx.h"
#include "soc_vau_type.h"
#include "drv_vau_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

td_void vau_debug_print_surface_list(const ext_vau_surface_list *surface_list);
td_void vau_debug_print_drv_surface_list(const drv_vau_surface_list *surface_list);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* API_VAU_DEBUG_H */
