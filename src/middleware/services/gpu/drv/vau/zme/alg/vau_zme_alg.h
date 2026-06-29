/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2024. All rights reserved.
 * Description: vau algorithm file
 */

#ifndef ZME_ALG_H
#define ZME_ALG_H

#include "drv_vau_type.h"
#include "common_def.h"
#include "vau_hal_node.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_void vau_get_zme_scale_coeff(const drv_vau_surface *surface);
td_void vau_set_zme_scale_coeff(vau_hardware_node *node, const drv_vau_surface *surface);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
