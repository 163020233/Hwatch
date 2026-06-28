/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: jpeg dfx
 */

#ifndef DRV_GRAPHICS_JPEG_DFX_H
#define DRV_GRAPHICS_JPEG_DFX_H

#include "gpu_dfx.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    td_u32 open_times;
    td_u32 close_times;
    td_u32 stream_buf_size;
    td_u64 stream_phy_buf;
    td_u32 data_buf_size;
    td_u64 data_phy_buf;
    td_u64 y_phy_buf;
    td_u32 y_buf_stride;
    td_u32 y_mcu_height;
    td_u64 uv_phy_buf;
    td_u32 uv_buf_stride;
    td_u32 uv_mcu_height;
    td_u32 jpeg_color_space;
    td_u32 output_color_space;
    td_u32 scale;
    td_float start_cost;
    td_float get_status_cost;
    td_float init_cost;
    td_u32 create_cnt;
    td_u32 destroy_cnt;
    td_u32 perf_cycles;
    td_u32 int_type;
} drv_jpeg_dfx_info;

drv_jpeg_dfx_info *jpeg_dfx_get_info(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* DRV_GRAPHICS_JPEG_DFX_H */
