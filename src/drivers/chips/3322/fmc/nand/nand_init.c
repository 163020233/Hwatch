/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include <sys/mount.h>
#include "dpal.h"
#include "mtd_common.h"
#include "hifmc100.h"
#include "fmc_porting.h"
#include "nand.h"
#include "dpal_driverbase.h"
#include "mtd_partition.h"
#include "yaffsfs.h"

#define YF_VOLUMES_NUM 3
#define YF_NAME_STRS "/boot/", "/system/", "/user/"
uint32_t yf_volume_percent[YF_VOLUMES_NUM] = {5, 25, 70};

/* 预留block0 用于boot启动 */
#define BOOT_NAND_BLOCK_NUM 1
int last_part_start_sector = BOOT_NAND_BLOCK_NUM;

static const char * const g_volume_names[YF_VOLUMES_NUM] = { YF_NAME_STRS };

static struct dpal_resource g_hifmc100_resources[] = {
    {
        .start  = HIFMC100_REG_BASE,
        .end    = HIFMC100_REG_BASE + 0x140,
        .flags  = IORESOURCE_MEM,
    },
    {
        .start  = HIFMC100_MEM_BASE,
        .end    = HIFMC100_MEM_BASE + _2K,
        .flags  = IORESOURCE_MEM,
    },
};

struct dpal_platform_device g_device_hifmc100_nand = {
    .name       = "fmc100_nand",
    .id         = -1,
    .resource   = g_hifmc100_resources,
    .num_resources = sizeof(g_hifmc100_resources) / sizeof((g_hifmc100_resources)[0]),
};

static int32_t machine_init(struct dpal_platform_device* device)
{
    (void)dpal_platform_device_register(device);
    return 0;
}

static int formate_filesystem(bool part_boot)
{
    int ret;
    size_t part_start_sector = (size_t)last_part_start_sector;
    size_t part_count_sector = 0;
    uint32_t total_sector_num = 0;
    char dev_name[64] = { 0 };

    total_sector_num = g_nand_mtd->size / g_nand_mtd->erasesize;

    for (uint32_t index = 0; index < YF_VOLUMES_NUM; ++index) {
        if (part_boot && index > 0) {
            break;
        }
        if (!part_boot && index == 0) {
            continue;
        }
        part_count_sector = (total_sector_num * yf_volume_percent[index]) / 0x64;
        /* The number of blocks in the last area is calculated separately. */
        if (!part_boot && (index == (YF_VOLUMES_NUM - 1))) {
            part_count_sector = total_sector_num - part_start_sector;
        }

        ret = add_mtd_partition("nand", (uint32_t)(part_start_sector * g_nand_mtd->erasesize),
                                (uint32_t)(part_count_sector * g_nand_mtd->erasesize), index);
        if (ret != 0) {
            ERR_MSG("add partition %d fail ret = %d\n", index, ret);
            return -EIO;
        }
        ret = sprintf_s(dev_name, sizeof(dev_name), "/dev/nandblk%u", index);
        if (ret <= 0) {
            ERR_MSG("formate dev %d name fail \n", index);
            return -EIO;
        }

        ret = mount(dev_name, g_volume_names[index], "yaffs", 0, NULL);
        INFO_MSG("formate filesystem devinfo: index = %u , dev_name = %s, folder_name = %s, \
                 part_start_sector = %d, part_count_sector = %d\n", index, dev_name,
                 g_volume_names[index], part_start_sector, part_count_sector);
        if (ret != 0) {
            return ret;
        }
        part_start_sector += part_count_sector;
        last_part_start_sector = (int)part_start_sector;
    }
    return ret;
}

int32_t nand_flash_init(void)
{
    fmc_port_init();
    machine_init(&g_device_hifmc100_nand);
    nand_fmc100_init();
    return 0;
}

int32_t nandflash_filesystem_init(bool part_boot)
{
    /* flash need to be initialized firstly before
     * boot partition mounted as first partition */
    if (part_boot) {
        nand_flash_init();
    }
    formate_filesystem(part_boot);
    return 0;
}

int32_t nandflash_format(void)
{
    struct erase_info opts;
    memset_s(&opts, sizeof(opts), 0, sizeof(opts));
    opts.addr = (uint64_t)g_nand_mtd->erasesize * BOOT_NAND_BLOCK_NUM;
    opts.len = (uint64_t)g_nand_mtd->size - g_nand_mtd->erasesize * BOOT_NAND_BLOCK_NUM;
    g_nand_mtd->erase(g_nand_mtd, &opts);
    return 0;
}

void nandflash_yaffs_sync(void)
{
#ifdef YAFFS_EXTEND_SUPPORT
    for (uint8_t index = 0; index < YF_VOLUMES_NUM; index++) {
        int32_t ret = yaffs_sync(g_volume_names[index]);
        if (ret != 0) {
            ERR_MSG("[%d] yaffs_sync fail, ret = %d", index, ret);
        }
    }
#endif
}
