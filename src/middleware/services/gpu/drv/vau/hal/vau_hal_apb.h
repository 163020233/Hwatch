/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description : dpu reg common file
 */

#ifndef DRV_GRAPHIC_HAL_VAU_HAL_APB_H
#define DRV_GRAPHIC_HAL_VAU_HAL_APB_H

#include "td_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_void vau_hal_apb_init_queue(td_void);
td_bool vau_hal_apb_is_idle(td_void);
td_u32 vau_hal_apb_get_current_ndoe(td_void);
td_u32 vau_hal_apb_get_int_state(td_void);
td_void vau_hal_apb_clear_int_state(td_u32 mask);
td_void vau_hal_apb_set_start_node(td_u32 start_node);
td_void vau_hal_apb_set_start_up(td_void);
td_void vau_hal_apb_set_cubic_threshold(td_float cubic_threshold);
td_void vau_hal_apb_vector_draw_init(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif