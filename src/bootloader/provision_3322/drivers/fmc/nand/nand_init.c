/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 */
#include "types.h"
#include <sys/mount.h>
#include "hifmc_common.h"
#include "hifmc100.h"
#include "fmc_porting.h"
#include "nand.h"
#include "securec.h"
#include "debug_print.h"
#include "tcxo.h"
#include "nand_fmc.h"

uint32_t dpal_device_register(struct dpal_device *dev)
{
    uint32_t ret = ERRCODE_SUCC;
    if (dev == NULL || dev->name == NULL) {
        return ERRCODE_FAIL;
    }

    if (dev->is_registered) {
        return ERRCODE_FAIL;
    }

    dev->drv = NULL;

    dev->is_registered = true;

    return ret;
}

int32_t dpal_plat_device_register(struct dpal_platform_device *dev)
{
    if (dev == NULL || dev->name == NULL) {
        return -EINVAL;
    }

    uint32_t ret = dpal_device_register(&dev->dev.dpal_dev);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

static struct dpal_resource g_hifmc100_resources[] = {
    {
        .start = HIFMC100_REG_BASE,
        .end = HIFMC100_REG_BASE + 0x140,
        .flags = IORESOURCE_MEM,
    },
    {
        .start = HIFMC100_MEM_BASE,
        .end = HIFMC100_MEM_BASE + _2K,
        .flags = IORESOURCE_MEM,
    },
};

struct dpal_platform_device g_device_hifmc100_nand = {
    .name = "fmc100_nand",
    .id = -1,
    .resource = g_hifmc100_resources,
    .num_resources = sizeof(g_hifmc100_resources) / sizeof((g_hifmc100_resources)[0]),
};

struct dpal_platform_device* get_device_hifmc100_nand(void)
{
    return &g_device_hifmc100_nand;
}

struct dpal_resource* get_hifmc100_resources(uint8_t index)
{
    return &g_hifmc100_resources[index];
}

static int32_t machine_init(struct dpal_platform_device *device)
{
    (void)dpal_plat_device_register(device);
    return 0;
}

int32_t nand_flash_init(void)
{
    fmc_port_init();
    machine_init(&g_device_hifmc100_nand);
    uapi_tcxo_delay_ms(10); // delay 10ms
    nand_fmc100_init();
    return 0;
}