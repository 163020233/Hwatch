/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved. \n
 *
 * Description: Partition manage module -- seperate configuration for each product \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-09-01, Create file. \n
 */

#include <stdint.h>
#include "partition.h"
#include "memory_config.h"
#include "partition_porting.h"

#define PARTITION_START_ADDRESS PARTITION_INFO_REGION_START
#define uapi_array_size(_array)  (sizeof(_array) / sizeof((_array)[0]))

static const partition_path_map_t g_image_path[] = {
    {PARTITION_BT_IMAGE, "/boot/bt/bt_signed.bin"},
    {PARTITION_DSP_MAIN_IMAGE, "/boot/dsp/dsp_main.bin"},
    {PARTITION_DSP_OVERLAY_IMAGE, "/boot/dsp/dsp_overlay.bin"}
};
uintptr_t partition_get_addr(void)
{
    return PARTITION_START_ADDRESS;
}

uint32_t partition_get_path_map(partition_path_map_t **path_map)
{
    *path_map = (partition_path_map_t *)g_image_path;
    return uapi_array_size(g_image_path);
}
