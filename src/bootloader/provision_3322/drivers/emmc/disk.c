/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2019. All rights reserved.
 * Description: LiteOS Disk Module Implementation
 * Author: Huawei LiteOS Team
 * Create: 2013-01-01
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --------------------------------------------------------------------------- */

#include "errcode.h"
#include <string.h>
#include "kal_base.h"
#include "mmc.h"
#include "sdhci.h"
#include "disk_pri.h"

#define DISK_MAX_SECTOR_SIZE 512

los_disk g_sysDisk[SYS_MAX_DISK];
static Atomic g_diskInitFlag = 0;

size_t strnlen(const char *s, size_t maxlen)
{
    size_t len;
    for (len = 0; len < maxlen; len++, s++) {
        if (!*s) {
            break;
        }
    }
    return (len);
}

int los_get_diskid_byname(const char *diskName)
{
    int diskId;
    los_disk *disk = NULL;
    size_t diskNameLen;

    if (diskName == NULL) {
        PRINT_ERR("The parameter diskName is NULL");
        return VFS_ERROR;
    }

    diskNameLen = strnlen(diskName, DISK_NAME + 1);
    if (diskNameLen > DISK_NAME) {
        PRINT_ERR("diskName is too long!\n");
        return VFS_ERROR;
    }

    for (diskId = 0; diskId < SYS_MAX_DISK; diskId++) {
        disk = &g_sysDisk[diskId];
        if (disk->disk_status != STAT_INUSED) {
            continue;
        }
        if (strcmp(diskName, disk->disk_name) == 0) {
            break;
        }
    }
    if (diskId == SYS_MAX_DISK) {
        PRINT_ERR("los_get_diskid_byname failed!\n");
        return VFS_ERROR;
    }
    return diskId;
}

los_disk *get_disk(int id)
{
    if ((id >= 0) && (id < SYS_MAX_DISK)) {
        return &g_sysDisk[id];
    }

    return NULL;
}

int EraseDiskByID(uint32_t diskId, size_t startSector, uint32_t sectors)
{
    if (startSector < RESERVED_BOOT_SECTOR ||
        (startSector + sectors) > (RESERVED_BOOT_SECTOR + EMMC_SECTOR_FOR_BURN)) {
        return 0;
    }
    int ret = ERRCODE_FAIL;
    los_disk *disk = get_disk((int)diskId);
    if (disk == NULL) {
        return ret;
    }
    if (disk->disk_status != (uint32_t)STAT_INUSED) {
        return ret;
    }
    ret = mmcsd_erase(disk->dev, startSector, sectors);
    return ret;
}

int32_t los_alloc_diskid_byname(const char *diskName)
{
    int32_t diskId;
    los_disk *disk = NULL;

    if (diskName == NULL) {
        PRINT_ERR("The parameter disk_name is NULL");
        return VFS_ERROR;
    }

    if (strnlen(diskName, DISK_NAME + 1) > DISK_NAME) {
        PRINT_ERR("diskName is too long!\n");
        return VFS_ERROR;
    }

    for (diskId = 0; diskId < SYS_MAX_DISK; diskId++) {
        disk = &g_sysDisk[diskId];
        if (disk->disk_status == STAT_UNUSED) {
            disk->disk_status = STAT_UNREADY;
            break;
        }
    }

    if (diskId == SYS_MAX_DISK) {
        PRINT_ERR("los_alloc_diskid_byname failed %d!\n", diskId);
        return VFS_ERROR;
    }

    if (strncpy_s(disk->disk_name, DISK_NAME + 1, diskName, strlen(diskName)) != EOK) {
        PRINT_ERR("The strncpy_s failed.\n");
        return VFS_ERROR;
    }

    return diskId;
}

static void DiskStructInit(const char *diskName, int32_t diskId, const struct geometry *diskInfo,
                           struct inode *blkDriver, los_disk *disk)
{
    disk->disk_id = (uint32_t)diskId;
    disk->dev = blkDriver;
    disk->type = 0;
    disk->sector_start = 0;
    disk->sector_size = (uint32_t)diskInfo->geo_sectorsize;
    disk->sector_count = diskInfo->geo_nsectors;
    if (strncpy_s(disk->disk_name, DISK_NAME + 1, diskName, strlen(diskName)) != EOK) {
        PRINT_ERR("DiskStructInit strncpy_s failed.\n");
        return;
    }
}

void nonos_disk_init(void)
{
    int32_t diskId;
    los_disk *disk = NULL;

    if (LOS_AtomicCmpXchg32bits(&g_diskInitFlag, 1, 0)) {
        return;
    }

    for (diskId = 0; diskId < SYS_MAX_DISK; diskId++) {
        disk = &g_sysDisk[diskId];
        disk->disk_status = STAT_UNUSED;
    }
}

static int32_t OsDiskInitSub(const char *diskName, int32_t diskId, los_disk *disk, struct inode *blkDriver)
{
    struct geometry diskInfo;

    if ((blkDriver->u.i_bops == NULL) || (blkDriver->u.i_bops->geometry == NULL) ||
        (blkDriver->u.i_bops->geometry(blkDriver, &diskInfo) != 0)) {
        return VFS_ERROR;
    }
    if (diskInfo.geo_sectorsize < DISK_MAX_SECTOR_SIZE) {
        return VFS_ERROR;
    }

    DiskStructInit(diskName, diskId, &diskInfo, blkDriver, disk);

    return ENOERR;
}

int32_t los_disk_init(const char *diskName, const struct block_operations *bops, void *priv, int32_t diskId, void *info)
{
    struct inode *blkDriver = NULL;
    los_disk *disk = get_disk(diskId);

    if ((diskName == NULL) || (disk == NULL) || (strnlen(diskName, DISK_NAME + 1) > DISK_NAME)) {
        return VFS_ERROR;
    }

    if (disk->disk_status != STAT_UNREADY) {
        return VFS_ERROR;
    }
    disk->disk_status = STAT_ONGOING;

    blkDriver = zalloc(sizeof(struct inode));
    if (blkDriver == NULL) {
        return (-ENOMEM);
    }
    blkDriver->u.i_bops = bops;
    blkDriver->i_private = priv;

    if (OsDiskInitSub(diskName, diskId, disk, blkDriver) != ENOERR) {
        goto DISK_BLKDRIVER_ERROR;
    }

    PRINT("disk_init : register %s ok!\n", diskName);

    disk->disk_status = STAT_INUSED;

    return ENOERR;

DISK_BLKDRIVER_ERROR:
    PRINT_ERR("disk_init : register %s ok but get disk info fail!\n", diskName);
    free(blkDriver);
    disk->disk_status = STAT_UNUSED;
    return VFS_ERROR;
}