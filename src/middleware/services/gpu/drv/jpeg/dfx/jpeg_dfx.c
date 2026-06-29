/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: jpeg hal
 */

#include "jpeg_dfx.h"

STATIC drv_jpeg_dfx_info g_jpeg_dfx_info = { 0 };
drv_jpeg_dfx_info *jpeg_dfx_get_info(td_void)
{
    return &g_jpeg_dfx_info;
}

td_s32 uapi_jpeg_dfx_proc_read(int argc, const char *argv[])
{
    td_u32 output_color_space = 0;

    uapi_unused(argc);
    uapi_unused(argv);

    output_color_space = (g_jpeg_dfx_info.output_color_space == 1) ? 3 : /* index: 1 == error, 3 == yuv420 */
        g_jpeg_dfx_info.jpeg_color_space;

    gpu_print("---------------------jpeg_proc-------------\n");
    gpu_print("open_times         : %u\n", g_jpeg_dfx_info.open_times);
    gpu_print("close_times        : %u\n", g_jpeg_dfx_info.close_times);
    gpu_print("stream_buf_size    : %u\n", g_jpeg_dfx_info.stream_buf_size);
    gpu_print("data_buf_size      : %u\n", g_jpeg_dfx_info.data_buf_size);
    gpu_print("y_buf_stride       : %u\n", g_jpeg_dfx_info.y_buf_stride);
    gpu_print("y_mcu_height       : %u\n", g_jpeg_dfx_info.y_mcu_height);
    gpu_print("uv_buf_stride      : %u\n", g_jpeg_dfx_info.uv_buf_stride);
    gpu_print("uv_mcu_height      : %u\n", g_jpeg_dfx_info.uv_mcu_height);
    gpu_print("jpeg_color_space   : %u\n", g_jpeg_dfx_info.jpeg_color_space);
    gpu_print("output_color_space : %u\n", output_color_space);
    gpu_print("scale              : %d\n", (0x01 << g_jpeg_dfx_info.scale));
    gpu_print("---------------------time info-------------------------\n");
    gpu_print("init_cost(us)      : %u\n", (td_u32)g_jpeg_dfx_info.init_cost);
    gpu_print("start_cost(us)     : %u\n", (td_u32)g_jpeg_dfx_info.start_cost);
    gpu_print("get_status_cost(us): %u\n", (td_u32)g_jpeg_dfx_info.get_status_cost);

    return EXT_SUCCESS;
}
