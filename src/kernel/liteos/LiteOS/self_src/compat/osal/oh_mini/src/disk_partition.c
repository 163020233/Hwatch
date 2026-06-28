/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: LiteOS disk partition.
 * Author: Huawei LiteOS Team
 * Create: 2025-02-16
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
#ifndef CONFIG_PRIVATE_DIRENT_STRUCT
#define CONFIG_PRIVATE_DIRENT_STRUCT
#endif

#if defined(SUPPORT_DISK_PART)
#include "disk.h"
#include "ff.h"
#include "partition.h"
#include "emmc.h"
#include "vfs_files.h"
#include "block.h"

#define DEV_NAME_MAX_SIZE  16
#define EMMC_BLOCK_SIZE    512
#endif

#if defined(SUPPORT_MBR_MULTIPLE_PART)
#include "los_typedef.h"
#include "disk.h"
#include "ff.h"
#include "disk_partition.h"

#define MBR_MAX_SIZE    4096
#define MAX_PERCENTAGE  100
#endif

#if defined(SUPPORT_DISK_PART)
/*
 * This API is compatible with liteos_m interface which used to create partitions by percentage.
 * The argument lengthArray is int type array which represent the percentage of the disk occupied by the partition.
 */
INT32 LOS_DiskPartition(const CHAR *dev, const CHAR *fsType, INT32 *lengthArray, INT32 *addrArray, INT32 partNum)
{
    INT32 diskId;
    INT32 ret = LOS_NOK;
    char devname[DEV_NAME_MAX_SIZE];
    const struct block_operations *ops = mmcsd_block_get_bops();
    CHAR *mbrBuf = NULL;
    LBA_t *transLengthArray = NULL;
    struct disk_divide_info divideInfo = {0};
    struct ptentry mmcDisk;
    UINT32 totalLength = 0;
    struct mmcsd_disk_part diskInfo;

    if ((lengthArray == NULL) || (dev == NULL) || (partNum <= 0)) {
        return LOS_NOK;
    }

    (VOID)fsType;
    (VOID)addrArray;

    transLengthArray = zalloc(partNum * sizeof(LBA_t));
    if (transLengthArray == NULL) {
        PRINTK("disk partion: mem alloc length array failure!\n");
        return LOS_NOK;
    }

    for (UINT32 i = 0; i < partNum; i++) {
        transLengthArray[i] = (LBA_t)lengthArray[i];
        totalLength += transLengthArray[i];
    }

    if (totalLength != 100) {       /* partition summation 100% */
        PRINTK("disk partion: usr length array invalid\n");
        goto OUT_WITH_FREE;
    }

    diskId = los_alloc_diskid_byname(dev);
    if (diskId < 0) {
        PRINTK("disk partion: alloc disk id failed!\n");
        goto OUT_WITH_FREE;
    }

    ret = emmc_set_diskinfo(&diskInfo, &mmcDisk);
    if (ret != ENOERR) {
        goto OUT_WITH_FREE;
    }
    divideInfo.sector_count = 0xFFFFFFFF;
    ret = add_mmc_partition(&divideInfo, 0, diskInfo.part_capacity);
    if (ret != ENOERR) {
        goto OUT_WITH_FREE;
    }

    ret = los_disk_init(dev, ops, (void *)&disk_info, diskId, &divideInfo);
    if (ret != 0) {
        PRINTK("disk partion: all disk init failure!, ret = %x\n", ret);
        goto OUT_WITH_FREE;
    }

    mbrBuf = zalloc(FF_MAX_SS);
    if (mbrBuf == NULL) {
        PRINTK("disk partion: mem alloc failure!\n");
        goto OUT_WITH_DISK_DEINIT;
    }
    /* Create partation in proportion. */
    ret = f_fdisk(0, (LBA_t const *)transLengthArray, mbrBuf);
    if (ret != FR_OK) {
        PRINTK("disk partion: f_fdisk failure! ret %x\n", ret);
        goto OUT;
    }
    ret = LOS_OK;

OUT:
    free(mbrBuf);
OUT_WITH_DISK_DEINIT:
    (VOID)los_disk_deinit(diskId);
OUT_WITH_FREE:
    free(transLengthArray);
    return ret;
}
#endif

#if defined(SUPPORT_MBR_MULTIPLE_PART)
/*
 * The argument lengthArray is int type array which represent the percentage of the disk occupied by the partition.
 * Before calling this API, create partitions by calling los_disk_init to ensure that MBR information can be written
 * from sector 0. The API invokes los_disk_deinit to destroy the disk.
 */
INT32 LOS_DiskPartition(const CHAR *dev, const CHAR *fsType, INT32 *lengthArray, const INT32 *addrArray, INT32 partNum)
{
    INT32 diskId = 0;
    INT32 ret = LOS_NOK;
    CHAR *mbrBuf = NULL;
    LBA_t *transLengthArray = NULL;
    UINT64 totalLength = 0;

    (void)fsType;
    (void)addrArray;

    if ((lengthArray == NULL) || (dev == NULL) || (partNum <= 0)) {
        return LOS_NOK;
    }

    size_t size = (size_t)partNum * sizeof(LBA_t);
    if (size / sizeof(LBA_t) != ((size_t)partNum)) {
        PRINTK("disk partion: overflow check failed!\n");
        return LOS_NOK;
    }

    transLengthArray = zalloc(size);
    if (transLengthArray == NULL) {
        PRINTK("disk partion: mem alloc length array failure!\n");
        return LOS_NOK;
    }

    for (INT32 i = 0; i < partNum; i++) {
        transLengthArray[i] = (LBA_t)lengthArray[i];
        totalLength += transLengthArray[i];
    }

    if (totalLength != MAX_PERCENTAGE) {       /* partition summation 100% */
        PRINTK("disk partion: usr length array invalid\n");
        goto OUT_WITH_FREE;
    }

    diskId = los_get_diskid_byname(dev);
    if (diskId < 0) {
        PRINTK("disk partion: get disk id failed!\n");
        goto OUT_WITH_FREE;
    }

    mbrBuf = zalloc(MBR_MAX_SIZE);
    if (mbrBuf == NULL) {
        PRINTK("disk partion: mem alloc failure!\n");
        goto OUT_WITH_DISK_DEINIT;
    }

    /* Create partation in proportion. */
    ret = f_fdisk(0, (LBA_t const *)transLengthArray, mbrBuf);
    if (ret != FR_OK) {
        PRINTK("disk partion: f_fdisk failure! ret %x\n", ret);
        goto OUT;
    }
#ifdef LOSCFG_FS_FAT_CACHE
    ret = LOS_BcacheSyncByName(dev);
    if (ret != ENOERR) {
        goto OUT;
    }
#endif
    ret = LOS_OK;

OUT:
    free(mbrBuf);
OUT_WITH_DISK_DEINIT:
    (VOID)los_disk_deinit(diskId);
OUT_WITH_FREE:
    free(transLengthArray);
    return ret;
}
#endif
