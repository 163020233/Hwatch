/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2019. All rights reserved.
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

#include "disk_pri.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "sys/mount.h"
#include "los_atomic.h"
#include "los_spinlock.h"
#ifdef LOSCFG_FS_FAT_CACHE
#include "bcache.h"
#endif
#ifdef LOSCFG_DRIVERS_MMC
#include "mmc_erase.h"
#endif
#ifdef LOSCFG_FS_FAT_UNI_TRIM
#include "utrim.h"
#endif
#ifdef LOSCFG_FS_FAT_FTL
#include "ftl.h"
#endif

#ifdef LOSCFG_FS_FAT_CACHE
SPIN_LOCK_INIT(g_bcacheHookSpin);
#endif
los_disk g_sysDisk[SYS_MAX_DISK];
los_part g_sysPart[SYS_MAX_PART];

#define CONFIG_FS_FAT_MAX_SECTOR_PER_BLOCK 2048
#if CONFIG_FS_FAT_SECTOR_PER_BLOCK > CONFIG_FS_FAT_MAX_SECTOR_PER_BLOCK
#error CONFIG_FS_FAT_SECTOR_PER_BLOCK should less than 2048!
#endif
UINT32 g_uwFatSectorsPerBlock = CONFIG_FS_FAT_SECTOR_PER_BLOCK;
UINT32 g_uwFatBlockNums = CONFIG_FS_FAT_BLOCK_NUMS;

UINT32 g_usbMode = 0;

static Atomic g_diskInitFlag = 0;

#define MEM_ADDR_ALIGN_BYTE  64
#define RWE_RW_RW            0755

#ifdef LOSCFG_FS_FAT_CACHE
#ifndef LOSCFG_COMPILER_ICCARM
typedef VOID *(*StorageHookFunction)(VOID *);

static UINT32 OsReHookFuncAddDiskRef(StorageHookFunction handler,
    VOID *param) __attribute__((weakref("osReHookFuncAdd")));
static UINT32 OsReHookFuncDelDiskRef(StorageHookFunction handler) __attribute__((weakref("osReHookFuncDel")));
#endif
UINT32 GetFatBlockNums(VOID)
{
    return g_uwFatBlockNums;
}

VOID SetFatBlockNums(UINT32 blockNums)
{
    if (blockNums != 0) {
        g_uwFatBlockNums = blockNums;
    }
}

UINT32 GetFatSectorsPerBlock(VOID)
{
    return g_uwFatSectorsPerBlock;
}

VOID SetFatSectorsPerBlock(UINT32 sectorsPerBlock)
{
    if (((sectorsPerBlock % UNSIGNED_INTEGER_BITS) == 0) && ((sectorsPerBlock & (sectorsPerBlock - 1)) == 0) &&
        (sectorsPerBlock <= CONFIG_FS_FAT_MAX_SECTOR_PER_BLOCK)) {
        g_uwFatSectorsPerBlock = sectorsPerBlock;
    }
}
#endif

INT32 los_alloc_diskid_byname(const CHAR *diskName)
{
    INT32 diskId;
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
        DISK_LOCK(disk);
        if (disk->disk_status == STAT_UNUSED) {
            disk->disk_status = STAT_UNREADY;
            DISK_UNLOCK(disk);
            break;
        }
        DISK_UNLOCK(disk);
    }

    if (diskId == SYS_MAX_DISK) {
        disk->disk_status = STAT_UNUSED;
        PRINT_ERR("los_alloc_diskid_byname failed %d!\n", diskId);
        return VFS_ERROR;
    }

    if (strncpy_s(disk->disk_name, DISK_NAME + 1, diskName, strlen(diskName)) != EOK) {
        disk->disk_status = STAT_UNUSED;
        PRINT_ERR("The strncpy_s failed.\n");
        return VFS_ERROR;
    }

    return diskId;
}

INT32 los_get_diskid_byname(const CHAR *diskName)
{
    INT32 diskId;
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

VOID OsSetUsbStatus(UINT32 diskId)
{
    if (diskId < SYS_MAX_DISK) {
        g_usbMode |= (1u << diskId) & UINT_MAX;
    }
}

VOID OsClearUsbStatus(UINT32 diskId)
{
    if (diskId < SYS_MAX_DISK) {
        g_usbMode &= ~((1u << diskId) & UINT_MAX);
    }
}

#ifdef LOSCFG_FS_FAT_CACHE
static BOOL GetDiskUsbStatus(UINT32 diskId)
{
    return (g_usbMode & (1u << diskId)) ? TRUE : FALSE;
}
#endif

los_disk *get_disk(INT32 id)
{
    if ((id >= 0) && (id < SYS_MAX_DISK)) {
        return &g_sysDisk[id];
    }

    return NULL;
}

los_part *get_part(INT32 id)
{
    if ((id >= 0) && (id < SYS_MAX_PART)) {
        return &g_sysPart[id];
    }

    return NULL;
}

static UINT64 GetFirstPartStart(const los_part *part)
{
    los_part *firstPart = NULL;
    los_disk *disk = get_disk((INT32)part->disk_id);
    firstPart = (disk == NULL) ? NULL : LOS_DL_LIST_ENTRY(disk->head.pstNext, los_part, list);
    return (firstPart == NULL) ? 0 : firstPart->sector_start;
}

static VOID DiskPartAddToDisk(los_disk *disk, los_part *part)
{
    DISK_LOCK(disk);
    part->disk_id = disk->disk_id;
    part->part_no_disk = disk->part_count;
    LOS_ListTailInsert(&disk->head, &part->list);
    disk->part_count++;
    DISK_UNLOCK(disk);
}

static VOID DiskPartDelFromDisk(los_disk *disk, los_part *part)
{
    LOS_ListDelete(&part->list);
    disk->part_count--;
}

static los_part *DiskPartAllocate(struct inode *dev, UINT64 start, UINT64 count, UINT32 diskId)
{
    UINT32 i;
    UINT32 offset = diskId * MAX_DIVIDE_PART_PER_DISK;
    los_part *part = get_part((INT32)offset); /* traversing from the offset of the array */

    for (i = 0; (i < MAX_DIVIDE_PART_PER_DISK) && (part != NULL); i++) {
        if (part->dev == NULL) {
            part->part_id = i + offset;
            part->part_no_mbr = 0;
            part->dev = dev;
            part->type = 0;
            part->sector_start = start;
            part->sector_count = count;
            part->part_name = NULL;
            LOS_ListInit(&part->list);

            return part;
        }
        part++;
    }

    return NULL;
}

static VOID DiskPartRelease(los_part *part)
{
    part->dev = NULL;
    part->part_no_disk = 0;
    part->part_no_mbr = 0;
    if (part->part_name != NULL) {
        free(part->part_name);
        part->part_name = NULL;
    }
}

/*
 * name is a combination of disk_name, 'p' and part_count, such as "/dev/mmcblk0p0"
 * disk_name : DISK_NAME + 1
 * 'p' : 1
 * part_count: 1
 */
#define DEV_NAME_BUFF_SIZE  (DISK_NAME + 3)
static INT32 DiskAddPart(los_disk *disk, UINT64 sectorStart, UINT64 sectorCount, BOOL isValidPart)
{
    struct inode_search_s desc;
    CHAR devName[DEV_NAME_BUFF_SIZE];
    struct inode *diskDev = NULL;
    los_part *part = NULL;

    if ((disk == NULL) || (disk->disk_status == STAT_UNUSED) || (disk->dev == NULL)) {
        return VFS_ERROR;
    }

    if ((sectorCount > disk->sector_count) || ((disk->sector_count - sectorCount) < sectorStart)) {
        PRINT_ERR("DiskAddPart failed: sector start is %llu, sector count is %llu\n", sectorStart, sectorCount);
        return VFS_ERROR;
    }

    diskDev = disk->dev;
    if (isValidPart == TRUE) {
        if (snprintf_s(devName, sizeof(devName), sizeof(devName) - 1, "%s%c%u",
                       disk->disk_name, 'p', disk->part_count) < 0) {
            return VFS_ERROR;
        }

        if (register_blockdriver(devName, diskDev->u.i_bops, RWE_RW_RW, diskDev->i_private)) {
            PRINT_ERR("DiskAddPart : register %s fail!\n", devName);
            return VFS_ERROR;
        }

        SETUP_SEARCH(&desc, devName, false);
        if (inode_find(&desc) < 0) {
            PRINT_ERR("DiskAddPart : find %s fail!\n", devName);
            RELEASE_SEARCH(&desc);
            return VFS_ERROR;
        }
        PRINTK("DiskAddPart : register %s ok!\n", devName);

        part = DiskPartAllocate(desc.node, sectorStart, sectorCount, disk->disk_id);
        inode_release(desc.node);
        RELEASE_SEARCH(&desc);
        if (part == NULL) {
            (VOID)unregister_blockdriver(devName);
            return VFS_ERROR;
        }
    } else {
        part = DiskPartAllocate(diskDev, sectorStart, sectorCount, disk->disk_id);
        if (part == NULL) {
            return VFS_ERROR;
        }
    }

    DiskPartAddToDisk(disk, part);
    if (disk->type == EMMC) {
        part->type = EMMC;
    }

    return (INT32)part->part_id;
}

static INT32 DiskDivide(los_disk *disk, struct disk_divide_info *info)
{
    UINT32 i;
    INT32 ret;

    disk->type = info->part[0].type;
    for (i = 0; i < info->part_count; i++) {
        if (info->sector_count < info->part[i].sector_start) {
            return VFS_ERROR;
        }
        if (info->part[i].sector_count > (info->sector_count - info->part[i].sector_start)) {
            PRINT_ERR("Part[%u] sector_start:%llu, sector_count:%llu, exceed emmc sector_count:%llu.\n", i,
                      info->part[i].sector_start, info->part[i].sector_count,
                      (info->sector_count - info->part[i].sector_start));
            info->part[i].sector_count = info->sector_count - info->part[i].sector_start;
            PRINT_ERR("Part[%u] sector_count change to %llu.\n", i, info->part[i].sector_count);

            ret = DiskAddPart(disk, info->part[i].sector_start, info->part[i].sector_count, TRUE);
            if (ret == VFS_ERROR) {
                return VFS_ERROR;
            }
            break;
        }
        ret = DiskAddPart(disk, info->part[i].sector_start, info->part[i].sector_count, TRUE);
        if (ret == VFS_ERROR) {
            return VFS_ERROR;
        }
    }

    return ENOERR;
}

static CHAR GPTPartitionTypeRecognition(const CHAR *parBuf)
{
    const CHAR *buf = parBuf;
    const CHAR *fsType = "FAT";
    const CHAR *str = "\xEB\x52\x90" "NTFS    "; /* NTFS Boot entry point */

    if (((LD_DWORD_DISK(&buf[BS_FILSYSTEMTYPE32]) & BS_FS_TYPE_MASK) == BS_FS_TYPE_VALUE) ||
        (strncmp(&buf[BS_FILSYSTYPE], fsType, strlen(fsType)) == 0)) {
        return BS_FS_TYPE_FAT;
    } else if (strncmp(&buf[BS_JMPBOOT], str, strlen(str)) == 0) {
        return BS_FS_TYPE_NTFS;
    }

    return ENOERR;
}

static INT32 DiskPartitionMemZalloc(size_t boundary, size_t size, CHAR **gptBuf, CHAR **partitionBuf)
{
    CHAR *buffer1 = NULL;
    CHAR *buffer2 = NULL;

    buffer1 = (CHAR *)memalign(boundary, size);
    if (buffer1 == NULL) {
        PRINT_ERR("%s buffer1 malloc %lu failed! %d\n", __FUNCTION__, size, __LINE__);
        return -ENOMEM;
    }
    buffer2 = (CHAR *)memalign(boundary, size);
    if (buffer2 == NULL) {
        PRINT_ERR("%s buffer2 malloc %lu failed! %d\n", __FUNCTION__, size, __LINE__);
        free(buffer1);
        return -ENOMEM;
    }
    (VOID)memset_s(buffer1, size, 0, size);
    (VOID)memset_s(buffer2, size, 0, size);

    *gptBuf = buffer1;
    *partitionBuf = buffer2;

    return ENOERR;
}

static INT32 GPTInfoGet(struct inode *blkDrv, CHAR *gptBuf)
{
    INT32 ret;

    ret = blkDrv->u.i_bops->read(blkDrv, (UINT8 *)gptBuf, 1, 1); /* Read the device first sector */
    if (ret != 1) { /* Read failed */
        PRINT_ERR("%s %d\n", __FUNCTION__, __LINE__);
        return -EIO;
    }

    if (!VERIFY_GPT(gptBuf)) {
        PRINT_ERR("%s %d\n", __FUNCTION__, __LINE__);
        return VFS_ERROR;
    }

    return ENOERR;
}

static INT32 OsGPTPartitionRecognitionSub(struct disk_divide_info *info, const CHAR *partitionBuf,
                                          UINT32 *partitionCount, UINT64 partitionStart, UINT64 partitionEnd)
{
    CHAR partitionType;

    if (VERIFY_FS(partitionBuf)) {
        partitionType = GPTPartitionTypeRecognition(partitionBuf);
        if (partitionType) {
            if (*partitionCount >= MAX_DIVIDE_PART_PER_DISK) {
                return VFS_ERROR;
            }
            info->part[*partitionCount].type = partitionType;
            info->part[*partitionCount].sector_start = partitionStart;
            info->part[*partitionCount].sector_count = (partitionEnd - partitionStart) + 1;
            (*partitionCount)++;
        } else {
            PRINT_ERR("The partition type is not allowed to use!\n");
        }
    } else {
        PRINT_ERR("Do not support the partition type!\n");
    }
    return ENOERR;
}

static INT32 OsGPTPartitionRecognition(struct inode *blkDrv, struct disk_divide_info *info,
                                       const CHAR *gptBuf, CHAR *partitionBuf, UINT32 *partitionCount)
{
    UINT32 j;
    INT32 ret = ENOERR;
    UINT64 partitionStart, partitionEnd;

    for (j = 0; j < PAR_ENTRY_NUM_PER_SECTOR; j++) {
        if (!VERITY_AVAILABLE_PAR(&gptBuf[j * TABLE_SIZE])) {
            PRINTK("The partition type is ESP or MSR!\n");
            continue;
        }

        if (!VERITY_PAR_VALID(&gptBuf[j * TABLE_SIZE])) {
            return VFS_ERROR;
        }

        partitionStart = LD_QWORD_DISK(&gptBuf[(j * TABLE_SIZE) + GPT_PAR_START_OFFSET]);
        partitionEnd = LD_QWORD_DISK(&gptBuf[(j * TABLE_SIZE) + GPT_PAR_END_OFFSET]);
        if ((partitionStart >= partitionEnd) || (partitionEnd > info->sector_count)) {
            PRINT_ERR("GPT partition %u recognition failed : partitionStart = %llu, partitionEnd = %llu\n",
                      j, partitionStart, partitionEnd);
            return VFS_ERROR;
        }

        (VOID)memset_s(partitionBuf, info->sector_size, 0, info->sector_size);
        ret = blkDrv->u.i_bops->read(blkDrv, (UINT8 *)partitionBuf, partitionStart, 1);
        if (ret != 1) { /* read failed */
            PRINT_ERR("%s %d\n", __FUNCTION__, __LINE__);
            return -EIO;
        }

        ret = OsGPTPartitionRecognitionSub(info, partitionBuf, partitionCount, partitionStart, partitionEnd);
        if (ret != ENOERR) {
            return VFS_ERROR;
        }
    }

    return ret;
}

static INT32 DiskGPTPartitionRecognition(struct inode *blkDrv, struct disk_divide_info *info)
{
    CHAR *gptBuf = NULL;
    CHAR *partitionBuf = NULL;
    UINT32 tableNum, i, index;
    UINT32 partitionCount = 0;
    INT32 ret;
    ret = DiskPartitionMemZalloc(MEM_ADDR_ALIGN_BYTE, info->sector_size, &gptBuf, &partitionBuf);
    if (ret != ENOERR) {
        return ret;
    }

    ret = GPTInfoGet(blkDrv, gptBuf);
    if (ret < 0) {
        goto OUT_WITH_MEM;
    }

    tableNum = LD_DWORD_DISK(&gptBuf[TABLE_NUM_OFFSET]);
    if (tableNum > TABLE_MAX_NUM) {
        tableNum = TABLE_MAX_NUM;
    }

    index = (tableNum % PAR_ENTRY_NUM_PER_SECTOR) ? ((tableNum / PAR_ENTRY_NUM_PER_SECTOR) + 1) :
                                                    (tableNum / PAR_ENTRY_NUM_PER_SECTOR);

    for (i = 0; i < index; i++) {
        (VOID)memset_s(gptBuf, info->sector_size, 0, info->sector_size);
        ret = blkDrv->u.i_bops->read(blkDrv, (UINT8 *)gptBuf, TABLE_START_SECTOR + i, 1);
        if (ret != 1) { /* read failed */
            PRINT_ERR("%s %d\n", __FUNCTION__, __LINE__);
            ret = -EIO;
            goto OUT_WITH_MEM;
        }

        ret = OsGPTPartitionRecognition(blkDrv, info, gptBuf, partitionBuf, &partitionCount);
        if (ret < 0) {
            if (ret == VFS_ERROR) {
                ret = (INT32)partitionCount;
            }
            goto OUT_WITH_MEM;
        }
    }
    ret = (INT32)partitionCount;

OUT_WITH_MEM:
    free(gptBuf);
    free(partitionBuf);
    return ret;
}

static INT32 OsMBRInfoGet(struct inode *blkDrv, CHAR *mbrBuf)
{
    INT32 ret;

    /* read MBR, start from sector 0, length is 1 sector */
    ret = blkDrv->u.i_bops->read(blkDrv, (UINT8 *)mbrBuf, 0, 1);
    if (ret != 1) { /* read failed */
        PRINT_ERR("driver read return error: %d\n", ret);
        return -EIO;
    }

    /* Check boot record signature. */
    if (LD_WORD_DISK(&mbrBuf[BS_SIG55AA]) != BS_SIG55AA_VALUE) {
        return VFS_ERROR;
    }

    return ENOERR;
}

static INT32 OsEBRInfoGet(struct inode *blkDrv, const struct disk_divide_info *info,
                          CHAR *ebrBuf, const CHAR *mbrBuf)
{
    INT32 ret;

    if (VERIFY_FS(mbrBuf)) {
        if (info->sector_count <= LD_DWORD_DISK(&mbrBuf[PAR_OFFSET + PAR_START_OFFSET])) {
            return VFS_ERROR;
        }

        ret = blkDrv->u.i_bops->read(blkDrv, (UINT8 *)ebrBuf,
                                     LD_DWORD_DISK(&mbrBuf[PAR_OFFSET + PAR_START_OFFSET]), 1);
        if ((ret != 1) || (!VERIFY_FS(ebrBuf))) { /* read failed */
            PRINT_ERR("OsEBRInfoGet, verify_fs error, ret = %d\n", ret);
            return -EIO;
        }
    }

    return ENOERR;
}

static INT32 OsPrimaryPartitionRecognition(const CHAR *mbrBuf, struct disk_divide_info *info,
                                           INT32 *extendedPos, INT32 *mbrCount)
{
    INT32 i;
    CHAR mbrPartitionType;
    INT32 extendedFlag = 0;
    INT32 count = 0;

    for (i = 0; i < MAX_PRIMARY_PART_PER_DISK; i++) {
        mbrPartitionType = mbrBuf[PAR_OFFSET + PAR_TYPE_OFFSET + (i * PAR_TABLE_SIZE)];
        if (mbrPartitionType) {
            info->part[i].type = mbrPartitionType;
            info->part[i].sector_start = LD_DWORD_DISK(&mbrBuf[PAR_OFFSET + PAR_START_OFFSET + (i * PAR_TABLE_SIZE)]);
            info->part[i].sector_count = LD_DWORD_DISK(&mbrBuf[PAR_OFFSET + PAR_COUNT_OFFSET + (i * PAR_TABLE_SIZE)]);
            if ((mbrPartitionType == EXTENDED_PAR) || (mbrPartitionType == EXTENDED_8G)) {
                extendedFlag = 1;
                *extendedPos = i;
                continue;
            }
            count++;
        }
    }
    *mbrCount = count;

    return extendedFlag;
}

static INT32 OsLogicalPartitionRecognition(struct inode *blkDrv, struct disk_divide_info *info,
                                           UINT32 extendedAddress, CHAR *ebrBuf, INT32 mbrCount)
{
    INT32 ret;
    UINT32 extendedOffset = 0;
    CHAR ebrPartitionType;
    INT32 ebrCount = 0;

    do {
        (VOID)memset_s(ebrBuf, info->sector_size, 0, info->sector_size);
        if (((UINT64)(extendedAddress) + extendedOffset) >= info->sector_count) {
            PRINT_ERR("extended partition is out of disk range: extendedAddress = %u, extendedOffset = %u\n",
                      extendedAddress, extendedOffset);
            break;
        }
        ret = blkDrv->u.i_bops->read(blkDrv, (UINT8 *)ebrBuf,
                                     (UINT64)extendedAddress + extendedOffset, 1);
        if (ret != 1) { /* read failed */
            PRINT_ERR("driver read return error: %d, extendedAddress = %u, extendedOffset = %u\n", ret,
                      extendedAddress, extendedOffset);
            return -EIO;
        }
        ebrPartitionType = ebrBuf[PAR_OFFSET + PAR_TYPE_OFFSET];
        if (ebrPartitionType && ((mbrCount + ebrCount) < MAX_DIVIDE_PART_PER_DISK)) {
            info->part[MAX_PRIMARY_PART_PER_DISK + ebrCount].type = ebrPartitionType;
            info->part[MAX_PRIMARY_PART_PER_DISK + ebrCount].sector_start = (UINT64)extendedAddress + extendedOffset +
                LD_DWORD_DISK(&ebrBuf[PAR_OFFSET + PAR_START_OFFSET]);
            info->part[MAX_PRIMARY_PART_PER_DISK + ebrCount].sector_count = LD_DWORD_DISK(&ebrBuf[PAR_OFFSET +
                                                                                          PAR_COUNT_OFFSET]);
            ebrCount++;
        }
        extendedOffset = LD_DWORD_DISK(&ebrBuf[PAR_OFFSET + PAR_START_OFFSET + PAR_TABLE_SIZE]);
    } while ((ebrBuf[PAR_OFFSET + PAR_TYPE_OFFSET + PAR_TABLE_SIZE] != 0) &&
             ((mbrCount + ebrCount) < MAX_DIVIDE_PART_PER_DISK));

    return ebrCount;
}

static INT32 DiskPartitionRecognition(struct inode *blkDrv, struct disk_divide_info *info)
{
    INT32 ret;
    INT32 extendedFlag;
    INT32 extendedPos = 0;
    INT32 mbrCount = 0;
    UINT32 extendedAddress;
    CHAR *mbrBuf = NULL;
    CHAR *ebrBuf = NULL;

    if ((blkDrv == NULL) || (blkDrv->u.i_bops == NULL) || (blkDrv->u.i_bops->read == NULL)) {
        return -EINVAL;
    }

    ret = DiskPartitionMemZalloc(MEM_ADDR_ALIGN_BYTE, info->sector_size, &mbrBuf, &ebrBuf);
    if (ret != ENOERR) {
        return ret;
    }

    ret = OsMBRInfoGet(blkDrv, mbrBuf);
    if (ret < 0) {
        goto OUT_WITH_MEM;
    }

    /* The partition type is GPT */
    if (mbrBuf[PARTION_MODE_BTYE] == (CHAR)PARTION_MODE_GPT) {
        ret = DiskGPTPartitionRecognition(blkDrv, info);
        goto OUT_WITH_MEM;
    }

    ret = OsEBRInfoGet(blkDrv, info, ebrBuf, mbrBuf);
    if (ret < 0) {
        ret = 0; /* no mbr */
        goto OUT_WITH_MEM;
    }

    extendedFlag = OsPrimaryPartitionRecognition(mbrBuf, info, &extendedPos, &mbrCount);
    if (extendedFlag) {
        extendedAddress = LD_DWORD_DISK(&mbrBuf[PAR_OFFSET + PAR_START_OFFSET + (extendedPos * PAR_TABLE_SIZE)]);
        ret = OsLogicalPartitionRecognition(blkDrv, info, extendedAddress, ebrBuf, mbrCount);
        if (ret <= 0) {
            goto OUT_WITH_MEM;
        }
    }
    ret += mbrCount;

OUT_WITH_MEM:
    free(ebrBuf);
    free(mbrBuf);
    return ret;
}

INT32 DiskPartitionRegister(los_disk *disk)
{
    INT32 count;
    UINT32 i, partSize;
    los_part *part = NULL;
    struct disk_divide_info parInfo;

    /* Fill disk_divide_info structure to set partition's information. */
    (VOID)memset_s(parInfo.part, sizeof(parInfo.part), 0, sizeof(parInfo.part));
    partSize = sizeof(parInfo.part) / sizeof(parInfo.part[0]);

    parInfo.sector_size = disk->sector_size;
    parInfo.sector_count = disk->sector_count;
    count = DiskPartitionRecognition(disk->dev, &parInfo);
    if (count == VFS_ERROR) {
        part = get_part(DiskAddPart(disk, 0, disk->sector_count, FALSE));
        if (part == NULL) {
            return VFS_ERROR;
        }
        part->part_no_mbr = 0;
        PRINTK("Disk %s doesn't contain a valid partition table.\n", disk->disk_name);
        return ENOERR;
    } else if (count < 0) {
        return VFS_ERROR;
    }

    parInfo.part_count = (UINT32)count;
    if (count == 0) {
        part = get_part(DiskAddPart(disk, 0, disk->sector_count, TRUE));
        if (part == NULL) {
            return VFS_ERROR;
        }
        part->part_no_mbr = 0;

        PRINTK("No MBR detected.\n");
        return ENOERR;
    }

    for (i = 0; i < partSize; i++) {
        /* Read the disk_divide_info structure to get partition's information. */
        if ((parInfo.part[i].type != 0) && (parInfo.part[i].type != EXTENDED_PAR) &&
            (parInfo.part[i].type != EXTENDED_8G)) {
            part = get_part(DiskAddPart(disk, parInfo.part[i].sector_start, parInfo.part[i].sector_count, TRUE));
            if (part == NULL) {
                return VFS_ERROR;
            }
            part->part_no_mbr = i + 1;
            part->filesystem_type = parInfo.part[i].type;
        }
    }

    return ENOERR;
}

#ifdef LOSCFG_FS_FAT_CACHE
static INT32 BlockDevOperation(los_disk *disk, const VOID *writeBuf, VOID *readBuf, UINT64 sector, UINT32 count)
{
    UINT32 len;
    INT32 result = VFS_ERROR;
    OsBcache *bcache = (OsBcache *)disk->bcache;
    BOOL wr = (writeBuf != NULL) ? TRUE : FALSE;

    if ((bcache != NULL) && (!FS_OPERATE_IN_EXC)) {
        if (((UINT64)(bcache->sectorSize) * count) > UINT_MAX) {
            return VFS_ERROR;
        }
        len = bcache->sectorSize * count;

        if (wr) {
            result = BlockCacheWrite(bcache, (const UINT8 *)writeBuf, &len, 0, sector);
        } else {
            result = BlockCacheRead(bcache, (UINT8 *)readBuf, &len, 0, sector);
        }

        if (result != ENOERR) {
            PRINT_ERR("BlockDevOperation(%s) err = %d, sector = %llu, len = %u\n",
                      wr ? "write" : "read", result, sector, len);
        }
    } else {
        if ((disk->dev == NULL) || (disk->dev->u.i_bops == NULL)) {
            return VFS_ERROR;
        }

        if ((writeBuf != NULL) && (disk->dev->u.i_bops->write != NULL)) {
            result = disk->dev->u.i_bops->write(disk->dev, (const UINT8 *)writeBuf, sector, count);
            if (result == (INT32)count) {
                result = ENOERR;
            }
        } else if ((readBuf != NULL) && (disk->dev->u.i_bops->read != NULL)) {
            result = disk->dev->u.i_bops->read(disk->dev, (UINT8 *)readBuf, sector, count);
            if (result == (INT32)count) {
                result = ENOERR;
            }
        }
    }

    return result;
}
#else
static INT32 BlockDevOperation(los_disk *disk, const VOID *writeBuf, VOID *readBuf, UINT64 sector, UINT32 count)
{
    INT32 result = VFS_ERROR;

    if ((disk->dev == NULL) || (disk->dev->u.i_bops == NULL)) {
        return VFS_ERROR;
    }

    if ((writeBuf != NULL) && (disk->dev->u.i_bops->write != NULL)) {
        result = disk->dev->u.i_bops->write(disk->dev, (const UINT8 *)writeBuf, sector, count);
        if (result == (INT32)count) {
            result = ENOERR;
        }
    } else if ((readBuf != NULL) && (disk->dev->u.i_bops->read != NULL)) {
        result = disk->dev->u.i_bops->read(disk->dev, (UINT8 *)readBuf, sector, count);
        if (result == (INT32)count) {
            result = ENOERR;
        }
    }

    return result;
}
#endif

static INT32 DiskOperation(INT32 drvId, const VOID *writeBuf, VOID *readBuf, UINT64 sector, UINT32 count)
{
    INT32 result = VFS_ERROR;
    los_disk *disk = get_disk(drvId);
    if (disk == NULL) {
        return result;
    }

    if (((writeBuf == NULL) && (readBuf == NULL)) || (count == 0)) {
        return result;
    }

    DISK_LOCK(disk);

    if (disk->disk_status != STAT_INUSED) {
        goto ERROR_HANDLE;
    }

    if ((count > disk->sector_count) || ((disk->sector_count - count) < sector)) {
        goto ERROR_HANDLE;
    }

    result = BlockDevOperation(disk, writeBuf, readBuf, sector, count);
    if (result != ENOERR) {
        goto ERROR_HANDLE;
    }

    DISK_UNLOCK(disk);
    return ENOERR;

ERROR_HANDLE:
    DISK_UNLOCK(disk);
    return VFS_ERROR;
}

#ifdef LOSCFG_FS_FAT_FTL
STATIC BOOL DiskCheckFtlDevice(CHAR *diskName)
{
    if (strstr(diskName, "ftl") == NULL) {
        return FALSE;
    }
    return TRUE;
}

INT32 DiskFtlOpen(INT32 drvId)
{
    los_disk *disk = get_disk(drvId);

    if (disk == NULL) {
        return -EINVAL;
    }
    if (DiskCheckFtlDevice(disk->disk_name) == FALSE) {
        return ENOERR;
    }

    if ((disk->dev == NULL) || (disk->dev->u.i_bops == NULL) || (disk->dev->u.i_bops->open == NULL)) {
        return -EINVAL;
    }
    return disk->dev->u.i_bops->open(disk->dev);
}

INT32 DiskFtlClose(INT32 drvId)
{
    los_disk *disk = get_disk(drvId);

    if (disk == NULL) {
        return -EINVAL;
    }
    if (DiskCheckFtlDevice(disk->disk_name) == FALSE) {
        return ENOERR;
    }

    if ((disk->dev == NULL) || (disk->dev->u.i_bops == NULL) || (disk->dev->u.i_bops->close == NULL)) {
        return -EINVAL;
    }
    return disk->dev->u.i_bops->close(disk->dev);
}
#endif

INT32 los_disk_read(INT32 drvId, VOID *buf, UINT64 sector, UINT32 count)
{
    return DiskOperation(drvId, NULL, buf, sector, count);
}

INT32 los_disk_write(INT32 drvId, const VOID *buf, UINT64 sector, UINT32 count)
{
    return DiskOperation(drvId, buf, NULL, sector, count);
}

INT32 los_disk_ioctl(INT32 drvId, INT32 cmd, VOID *buf)
{
    struct geometry info;
    los_disk *disk = get_disk(drvId);
    if (disk == NULL) {
        return VFS_ERROR;
    }

    DISK_LOCK(disk);

    if ((disk->dev == NULL) || (disk->disk_status != STAT_INUSED)) {
        goto ERROR_HANDLE;
    }

    if (cmd == DISK_CTRL_SYNC) {
        DISK_UNLOCK(disk);
        return ENOERR;
    }

    if (buf == NULL) {
        goto ERROR_HANDLE;
    }

    (VOID)memset_s(&info, sizeof(info), 0, sizeof(info));
    if ((disk->dev->u.i_bops == NULL) || (disk->dev->u.i_bops->geometry == NULL) ||
        (disk->dev->u.i_bops->geometry(disk->dev, &info) != 0)) {
        goto ERROR_HANDLE;
    }

    if (cmd == DISK_GET_SECTOR_COUNT) {
        *(UINT64 *)buf = info.geo_nsectors;
        if (info.geo_nsectors == 0) {
            goto ERROR_HANDLE;
        }
    } else if (cmd == DISK_GET_SECTOR_SIZE) {
        *(size_t *)buf = (size_t)info.geo_sectorsize;
    } else if (cmd == DISK_GET_BLOCK_SIZE) { /* Get erase block size in unit of sectors (UINT32) */
        /* Block Num SDHC == 512, SD can be set to 512 or other */
        *(size_t *)buf = DISK_MAX_SECTOR_SIZE / info.geo_sectorsize;
    } else {
        goto ERROR_HANDLE;
    }

    DISK_UNLOCK(disk);
    return ENOERR;

ERROR_HANDLE:
    DISK_UNLOCK(disk);
    return VFS_ERROR;
}

static INT32 PartOperation(INT32 pt, const VOID *writeBuf, VOID *readBuf, UINT64 sector, UINT32 count)
{
    const los_part *part = get_part(pt);
    los_disk *disk = NULL;
    INT32 ret;

    if (part == NULL) {
        return VFS_ERROR;
    }

    disk = get_disk((INT32)part->disk_id);
    if (disk == NULL) {
        return VFS_ERROR;
    }

    DISK_LOCK(disk);
    if ((part->dev == NULL) || (disk->disk_status != STAT_INUSED)) {
        goto ERROR_HANDLE;
    }

    if (count > part->sector_count) {
        PRINT_ERR("PartOperation failed, invalid count, count = %u\n", count);
        goto ERROR_HANDLE;
    }

    /* Read from or write to absolute sector. */
    if (part->type == EMMC) {
        if ((disk->sector_count - part->sector_start) > sector) {
            sector += part->sector_start;
        } else {
            PRINT_ERR("PartOperation failed, invalid sector, sector = %llu\n", sector);
            goto ERROR_HANDLE;
        }
    }

    if ((sector >= GetFirstPartStart(part)) &&
        (((sector + count) > (part->sector_start + part->sector_count)) || (sector < part->sector_start))) {
        PRINT_ERR("PartOperation error, sector = %llu, count = %u, part->sector_start = %llu, "
                  "part->sector_count = %llu\n", sector, count, part->sector_start, part->sector_count);
        goto ERROR_HANDLE;
    }

    ret = DiskOperation((INT32)part->disk_id, writeBuf, readBuf, sector, count);
    if (ret < 0) {
        goto ERROR_HANDLE;
    }

    DISK_UNLOCK(disk);
    return ENOERR;

ERROR_HANDLE:
    DISK_UNLOCK(disk);
    return VFS_ERROR;
}

INT32 los_part_read(INT32 pt, VOID *buf, UINT64 sector, UINT32 count)
{
    return PartOperation(pt, NULL, buf, sector, count);
}

INT32 los_part_write(INT32 pt, const VOID *buf, UINT64 sector, UINT32 count)
{
    return PartOperation(pt, buf, NULL, sector, count);
}

#define TRIM_CMD(cmd)   ((cmd) == DISK_CTRL_TRIM || (cmd) == DISK_CTRL_TRIM_REVERT)

#if defined(LOSCFG_FS_FAT_DIRECT_TRIM) || defined(LOSCFG_FS_FAT_UNI_TRIM)

static INT32 PartIsValid(const los_part *part, UINT64 start, UINT32 count)
{
    if (count > part->sector_count) {
        return VFS_ERROR;
    }

#if defined(LOSCFG_FS_FAT_UNI_TRIM)
    // Non-MBR currently does not support unitrim.
    if (part->type == EMMC) {
        return -EOPNOTSUPP;
    }
#endif

    if ((start >= GetFirstPartStart(part)) &&
        (((start + count) > (part->sector_start + part->sector_count)) || (start < part->sector_start))) {
        return VFS_ERROR;
    }

    return ENOERR;
}

#endif

#if defined(LOSCFG_FS_FAT_DIRECT_TRIM)

static INT32 PartTrim(const los_part *part, UINT64 start, UINT32 count, INT32 cmd)
{
    (VOID)cmd;
    INT32 result;

    if (cmd == DISK_CTRL_TRIM_REVERT) {
        return -EOPNOTSUPP;
    }

    if (PartIsValid(part, start, count) != ENOERR) {
        return VFS_ERROR;
    }

    result = mmcsd_erase(part->dev, start, count);

    return result;
}

#elif defined(LOSCFG_FS_FAT_UNI_TRIM)

static INT32 PartTrim(const los_part *part, UINT64 start, UINT32 count, INT32 cmd)
{
    INT32 result;

    if (part->trimManager == NULL) {
        return VFS_ERROR;
    }

    if (PartIsValid(part, start, count) != ENOERR) {
        return VFS_ERROR;
    }

    if (cmd == DISK_CTRL_TRIM) {
        result = UniTrim(part->trimManager, start, count);
    } else {
        result = UniUntrim(part->trimManager, start, count);
    }

    return result;
}

#else

static INT32 PartTrim(const los_part *part, UINT64 start, UINT32 count, INT32 cmd)
{
    (VOID)part;
    (VOID)start;
    (VOID)count;
    (VOID)cmd;
    return ENOERR;
}

#endif

#define GET_ERASE_BLOCK_SIZE 0x2

INT32 los_part_ioctl(INT32 pt, INT32 cmd, VOID *buf)
{
    UINT64 rt[2];
    struct geometry info;
    los_part *part = get_part(pt);
    los_disk *disk = NULL;

    if ((part == NULL) || ((buf == NULL) && (cmd != DISK_CTRL_SYNC))) {
        return VFS_ERROR;
    }

    disk = get_disk((INT32)part->disk_id);
    if (disk == NULL) {
        return VFS_ERROR;
    }

    DISK_LOCK(disk);
    if ((part->dev == NULL) || (disk->disk_status != STAT_INUSED)) {
        goto ERROR_HANDLE;
    }

    if (cmd == DISK_CTRL_SYNC) {
        DISK_UNLOCK(disk);
        return ENOERR;
    }

    (VOID)memset_s(&info, sizeof(info), 0, sizeof(info));
    if ((part->dev->u.i_bops == NULL) || (part->dev->u.i_bops->geometry == NULL) ||
        (part->dev->u.i_bops->geometry(part->dev, &info) != 0)) {
        goto ERROR_HANDLE;
    }

    if (cmd == DISK_GET_SECTOR_COUNT) {
        *(UINT64 *)buf = part->sector_count;
        if (*(UINT64 *)buf == 0) {
            goto ERROR_HANDLE;
        }
    } else if (cmd == DISK_GET_SECTOR_SIZE) {
        *(size_t *)buf = (size_t)info.geo_sectorsize;
    } else if (cmd == DISK_GET_BLOCK_SIZE) { /* Get erase block size in unit of sectors (UINT32) */
        if ((part->dev->u.i_bops->ioctl == NULL) ||
            (part->dev->u.i_bops->ioctl(part->dev, GET_ERASE_BLOCK_SIZE, (UINTPTR)buf) != 0)) {
            goto ERROR_HANDLE;
        }
    } else if (TRIM_CMD(cmd)) {
        if (part->type == EMMC) {
            // Non-MBR require adding partition address offset.
            rt[0] = ((UINT64 *)buf)[0] + part->sector_start;
            rt[1] = ((UINT64 *)buf)[1] + part->sector_start;
        } else {
            rt[0] = ((UINT64 *)buf)[0];
            rt[1] = ((UINT64 *)buf)[1];
        }
        (VOID)PartTrim(part, rt[0], (UINT32)(rt[1] - rt[0] + 1), cmd);
    } else {
        goto ERROR_HANDLE;
    }

    DISK_UNLOCK(disk);
    return ENOERR;

ERROR_HANDLE:
    DISK_UNLOCK(disk);
    return VFS_ERROR;
}

#ifdef LOSCFG_FS_FAT_CACHE
static VOID DiskCacheThreadInit(UINT32 diskId, OsBcache *bc)
{
    if (GetDiskUsbStatus(diskId) == FALSE) {
#ifdef LOSCFG_FS_FAT_CACHE_SYNC_THREAD
        BcacheSyncThreadInit(bc, (INT32)diskId);
#else
        (VOID)bc;
#endif
    }

	/*
	 * AddHook and DelHook may be thread-unsafe, Generally implementers should ensure
	 * that the interface is thread-safe.
	 */
#ifndef LOSCFG_COMPILER_ICCARM
    if (OsReHookFuncAddDiskRef != NULL) {
        LOS_SpinLock(&g_bcacheHookSpin);
        (VOID)OsReHookFuncAddDiskRef((StorageHookFunction)OsSdSync, (VOID *)diskId);
        LOS_SpinUnlock(&g_bcacheHookSpin);
    }
#endif
}

static OsBcache *DiskCacheInit(UINT32 sectorSize, UINT64 nsectors, struct inode *blkDriver)
{
#define SECTOR_SIZE 512

    OsBcache *bc = NULL;
    UINT32 sectorPerBlock;
    UINT32 div = sectorSize / SECTOR_SIZE;
#ifdef LOSCFG_FS_FAT_FTL
    if ((blkDriver == NULL) || IsFtlBlockDev(blkDriver->i_name)) {
        return NULL;
    }
#endif
    if (div != 0) {
        sectorPerBlock = g_uwFatSectorsPerBlock / div;
        if (sectorPerBlock != 0) {
            bc = BlockCacheInit(blkDriver, sectorSize, sectorPerBlock,
                                g_uwFatBlockNums, nsectors / sectorPerBlock);
        }
    }

    if (bc == NULL) {
        PRINT_ERR("disk_init : disk have not init bcache cache!\n");
        return NULL;
    }
    return bc;
}

static VOID DiskCacheDeinit(los_disk *disk)
{
    UINT32 diskId = disk->disk_id;
    if (GetDiskUsbStatus(diskId) == FALSE) {
#ifdef LOSCFG_FS_FAT_CACHE_SYNC_THREAD
        BcacheSyncThreadDeinit(disk->bcache);
#endif
    }

    BlockCacheDeinit(disk->bcache);
    disk->bcache = NULL;
#ifndef LOSCFG_COMPILER_ICCARM
    if (OsReHookFuncDelDiskRef != NULL) {
        LOS_SpinLock(&g_bcacheHookSpin);
        (VOID)OsReHookFuncDelDiskRef((StorageHookFunction)OsSdSync);
        LOS_SpinUnlock(&g_bcacheHookSpin);
    }
#endif
}
#endif

static VOID DiskStructInit(const CHAR *diskName, INT32 diskId, const struct geometry *diskInfo,
                           struct inode *blkDriver, los_disk *disk)
{
    disk->disk_id = (UINT32)diskId;
    disk->dev = blkDriver;
    disk->type = 0;
    disk->sector_start = 0;
    disk->sector_size = (UINT32)diskInfo->geo_sectorsize;
    disk->sector_count = diskInfo->geo_nsectors;
    if (strncpy_s(disk->disk_name, DISK_NAME + 1, diskName, strlen(diskName)) != EOK) {
        PRINT_ERR("DiskStructInit strncpy_s failed.\n");
        return;
    }
    LOS_ListInit(&disk->head);
}

static INT32 DiskDivideAndPartitionRegister(struct disk_divide_info *info, los_disk *disk)
{
    INT32 ret;

    if (info != NULL) {
        ret = DiskDivide(disk, info);
        if (ret != ENOERR) {
            PRINT_ERR("DiskDivide failed, ret = %d\n", ret);
            return ret;
        }
    } else {
        ret = DiskPartitionRegister(disk);
        if (ret != ENOERR) {
            PRINT_ERR("DiskPartitionRegister failed, ret = %d\n", ret);
            return ret;
        }
    }
    return ENOERR;
}

static INT32 DiskDeinit(los_disk *disk)
{
    los_part *part = NULL;
    CHAR devName[DEV_NAME_BUFF_SIZE];
    INT32 ret;

    if (LOS_ListEmpty(&disk->head) == FALSE) {
        part = LOS_DL_LIST_ENTRY(disk->head.pstNext, los_part, list);
        while (&part->list != &disk->head) {
            ret = snprintf_s(devName, sizeof(devName), sizeof(devName) - 1, "%s%c%d",
                             disk->disk_name, 'p', disk->part_count - 1);
            if (ret < 0) {
                return -ENAMETOOLONG;
            }

#ifdef LOSCFG_FS_FAT_UNI_TRIM
            if (part->trimManager != NULL) {
                UniTrimDeinit(part->trimManager);
                part->trimManager = NULL;
            }
#endif
            DiskPartDelFromDisk(disk, part);
            ret = unregister_blockdriver(devName);
            if (ret != 0) {
                PRINT_ERR("unregister_blockdriver err %x, %s\n", ret, devName);
            }
            DiskPartRelease(part);

            part = LOS_DL_LIST_ENTRY(disk->head.pstNext, los_part, list);
        }
    }

#ifdef LOSCFG_FS_FAT_CACHE
    DiskCacheDeinit(disk);
#endif

    disk->dev = NULL;
    ret = unregister_blockdriver(disk->disk_name);
    if (ret != 0) {
        PRINT_ERR("unregister_blockdriver err %x, %s\n", ret, disk->disk_name);
    }
    DISK_LOCK(disk);
    disk->disk_status = STAT_UNUSED;
    disk->disk_name[0] = '\0';
    DISK_UNLOCK(disk);

    return ENOERR;
}

VOID OsDiskInit(VOID)
{
    INT32 diskId;
    UINT32 ret;
    los_disk *disk = NULL;

    if (LOS_AtomicCmpXchg32bits(&g_diskInitFlag, 1, 0)) {
        return;
    }

    for (diskId = 0; diskId < SYS_MAX_DISK; diskId++) {
        disk = &g_sysDisk[diskId];
        disk->disk_status = STAT_UNUSED;
        disk->disk_mutex = 0xFFFFFFFF; /* give a invalid init value */
        ret = LOS_MuxCreate(&disk->disk_mutex);
        if (ret != LOS_OK) {
            PRINT_ERR("%s %d, mutex created failed, error:%u\n", __FUNCTION__, __LINE__, ret);
        }
    }
}

static INT32 OsDiskInitSub(const CHAR *diskName, INT32 diskId, los_disk *disk, struct inode *blkDriver)
{
    struct geometry diskInfo;

    if ((blkDriver->u.i_bops == NULL) || (blkDriver->u.i_bops->geometry == NULL) ||
        (blkDriver->u.i_bops->geometry(blkDriver, &diskInfo) != 0)) {
        return VFS_ERROR;
    }
    if (diskInfo.geo_sectorsize < DISK_MAX_SECTOR_SIZE) {
        return VFS_ERROR;
    }

#ifdef LOSCFG_FS_FAT_CACHE
    OsBcache *bc = DiskCacheInit((UINT32)diskInfo.geo_sectorsize, (UINT64)diskInfo.geo_nsectors, blkDriver);
    if (bc != NULL) {
        disk->bcache = bc;
        DiskCacheThreadInit((UINT32)diskId, bc);
    }
#endif

    DiskStructInit(diskName, diskId, &diskInfo, blkDriver, disk);

    return ENOERR;
}

INT32 los_disk_init(const CHAR *diskName, const struct block_operations *bops,
                    VOID *priv, INT32 diskId, VOID *info)
{
    struct inode_search_s desc;
    struct inode *blkDriver = NULL;
    los_disk *disk = get_disk(diskId);
    int ret;

    if ((diskName == NULL) || (disk == NULL) || (strnlen(diskName, DISK_NAME + 1) > DISK_NAME)) {
        return VFS_ERROR;
    }

    DISK_LOCK(disk);
    if (disk->disk_status != STAT_UNREADY) {
        DISK_UNLOCK(disk);
        return VFS_ERROR;
    }
    disk->disk_status = STAT_ONGOING;
    DISK_UNLOCK(disk);

    if (register_blockdriver(diskName, bops, RWE_RW_RW, priv) != 0) {
        disk->disk_status = STAT_UNUSED;
        PRINT_ERR("disk_init : register %s fail!\n", diskName);
        return VFS_ERROR;
    }

    SETUP_SEARCH(&desc, diskName, false);
    ret = inode_find(&desc);
    if (ret < 0) {
        PRINT_ERR("disk_init : find %s fail!\n", diskName);
        goto DISK_FIND_ERROR;
    }
    blkDriver = desc.node;

    if (OsDiskInitSub(diskName, diskId, disk, blkDriver) != ENOERR) {
        goto DISK_BLKDRIVER_ERROR;
    }
    inode_release(blkDriver);

    PRINTK("disk_init : register %s ok!\n", diskName);

    if (DiskDivideAndPartitionRegister(info, disk) != ENOERR) {
        (VOID)DiskDeinit(disk);
        return VFS_ERROR;
    }

    disk->disk_status = STAT_INUSED;
    RELEASE_SEARCH(&desc);

    return ENOERR;

DISK_BLKDRIVER_ERROR:
    PRINT_ERR("disk_init : register %s ok but get disk info fail!\n", diskName);
    inode_release(blkDriver);
DISK_FIND_ERROR:
    RELEASE_SEARCH(&desc);
    (VOID)unregister_blockdriver(diskName);
    disk->disk_status = STAT_UNUSED;
    return VFS_ERROR;
}

INT32 los_disk_deinit(INT32 diskId)
{
    los_disk *disk = get_disk(diskId);
    if (disk == NULL) {
        return -EINVAL;
    }

    DISK_LOCK(disk);

    if (disk->disk_status != STAT_INUSED) {
        DISK_UNLOCK(disk);
        return -EINVAL;
    }

    disk->disk_status = STAT_ONGOING;
    DISK_UNLOCK(disk);

    return DiskDeinit(disk);
}

INT32 los_disk_sync(INT32 drvId)
{
    INT32 ret = ENOERR;
    los_disk *disk = get_disk(drvId);
    if (disk == NULL) {
        return EINVAL;
    }

    DISK_LOCK(disk);
    if (disk->disk_status != STAT_INUSED) {
        DISK_UNLOCK(disk);
        return EINVAL;
    }

#ifdef LOSCFG_FS_FAT_CACHE
    if (disk->bcache != NULL) {
        ret = BlockCacheSync(disk->bcache);
    }
#endif

    DISK_UNLOCK(disk);
    return ret;
}

INT32 los_disk_set_bcache(INT32 drvId, UINT32 sectorPerBlock, UINT32 blockNum)
{
#ifdef LOSCFG_FS_FAT_CACHE

    INT32 ret;
    OsBcache *bc = NULL;
    los_disk *disk = get_disk(drvId);
    if ((disk == NULL) || (sectorPerBlock == 0) || (blockNum == 0)) {
        return EINVAL;
    }

    if (((sectorPerBlock % UNSIGNED_INTEGER_BITS) != 0) || (sectorPerBlock > CONFIG_FS_FAT_MAX_SECTOR_PER_BLOCK) ||
        ((sectorPerBlock & (sectorPerBlock - 1)) != 0)) {
        return EINVAL;
    }

    DISK_LOCK(disk);

    if (disk->disk_status != STAT_INUSED) {
        goto ERROR_HANDLE;
    }

    if (disk->bcache != NULL) {
        ret = BlockCacheSync(disk->bcache);
        if (ret != ENOERR) {
            DISK_UNLOCK(disk);
            return ret;
        }
    }

    DiskCacheDeinit(disk);

    g_uwFatBlockNums = blockNum;
    g_uwFatSectorsPerBlock = sectorPerBlock;

    bc = DiskCacheInit(disk->sector_size, disk->sector_count, disk->dev);
    if ((bc == NULL) && (blockNum != 0)) {
        DISK_UNLOCK(disk);
        return ENOMEM;
    }

    if (bc != NULL) {
        disk->bcache = bc;
        DiskCacheThreadInit((UINT32)drvId, bc);
    }

    DISK_UNLOCK(disk);
    return ENOERR;

ERROR_HANDLE:
    DISK_UNLOCK(disk);
    return EINVAL;
#else
    return VFS_ERROR;
#endif
}

INT32 los_disk_bcacheable(INT32 drvId, BOOL bcacheable)
{
#ifdef LOSCFG_FS_FAT_CACHE
    INT32 ret;
    los_disk *disk = get_disk(drvId);
    if (disk == NULL) {
        PRINT_ERR("the disk:%d is not registered.\n", drvId);
        return VFS_ERROR;
    }
    DISK_LOCK(disk);
    if (disk->disk_status != STAT_INUSED) {
        PRINT_ERR("the disk:%d is not in used.\n", drvId);
        DISK_UNLOCK(disk);
        return VFS_ERROR;
    }
    if (bcacheable) {
        if (disk->bcache != NULL) {
            /* No need for repeated revisions */
            goto NOERR_RET;
        }
        disk->bcache = DiskCacheInit(disk->sector_size, disk->sector_count, disk->dev);
        if (disk->bcache == NULL) {
            PRINT_ERR("the disk:%d cache init failed.\n", drvId);
            DISK_UNLOCK(disk);
            return VFS_ERROR;
        } else {
            DiskCacheThreadInit((UINT32)drvId, disk->bcache);
        }
    } else {
        if (disk->bcache == NULL) {
            /* No need for repeated revisions */
            goto NOERR_RET;
        }
        /* There is a lock that is mutually exclusive with read and write operations (DiskOperation). */
        ret = BlockCacheSync(disk->bcache);
        if (ret != ENOERR) {
            PRINT_ERR("the disk:%d sync failed.\n", drvId);
            DISK_UNLOCK(disk);
            return ret;
        }
        DiskCacheDeinit(disk);
    }

NOERR_RET:
    DISK_UNLOCK(disk);
#else
    (void)drvId;
    (void)bcacheable;
#endif
    return ENOERR;
}

static los_part *OsPartFind(los_disk *disk, const struct inode *blkDriver)
{
    los_part *part = NULL;

    DISK_LOCK(disk);
    if ((disk->disk_status != STAT_INUSED) || (LOS_ListEmpty(&disk->head) == TRUE)) {
        goto EXIT;
    }
    part = LOS_DL_LIST_ENTRY(disk->head.pstNext, los_part, list);
    if (disk->dev == blkDriver) {
        goto EXIT;
    }

    while (&part->list != &disk->head) {
        if (part->dev == blkDriver) {
            goto EXIT;
        }
        part = LOS_DL_LIST_ENTRY(part->list.pstNext, los_part, list);
    }
    part = NULL;

EXIT:
    DISK_UNLOCK(disk);
    return part;
}

los_part *los_part_find(const struct inode *blkDriver)
{
    INT32 i;
    los_disk *disk = NULL;
    los_part *part = NULL;

    if (blkDriver == NULL) {
        return NULL;
    }

    for (i = 0; i < SYS_MAX_DISK; i++) {
        disk = &g_sysDisk[i];
        part = OsPartFind(disk, blkDriver);
        if (part != NULL) {
            return part;
        }
    }

    return NULL;
}

static UINT32 GetDiskNameStart(const CHAR *name)
{
    UINT32 i;
    UINT32 off = 0;
    for (i = 0; i < strlen(name); i++) {
        if (name[i] == '/') {
            off = i + 1;
        }
    }
    return off;
}

BOOL IsBlockStatusReady(const struct inode *blkDriver)
{
    los_disk *disk = NULL;
    INT32 i;
    UINT32 off;

    if (blkDriver == NULL) {
        return FALSE;
    }

    for (i = 0; i < SYS_MAX_DISK; i++) {
        disk = &g_sysDisk[i];
        if (strlen(disk->disk_name) == 0) {
            continue;
        }
        off = GetDiskNameStart(disk->disk_name); /* get the diskname removed "/dev/" */
        if (strstr(blkDriver->i_name, disk->disk_name + off) == NULL) {
            continue;
        }
        if (OsPartFind(disk, blkDriver) != NULL) {
            return (disk->disk_status == STAT_INUSED) ? TRUE : FALSE;
        } else {
            return FALSE;
        }
    }

    return TRUE;
}

INT32 los_part_access(const CHAR *dev, mode_t mode)
{
    struct inode_search_s desc;
    los_part *part = NULL;
    int ret;

    (VOID)mode;

    SETUP_SEARCH(&desc, dev, false);
    ret = inode_find(&desc);
    if (ret < 0) {
        RELEASE_SEARCH(&desc);
        return VFS_ERROR;
    }

    part = los_part_find(desc.node);
    inode_release(desc.node);
    if (part == NULL) {
        RELEASE_SEARCH(&desc);
        return VFS_ERROR;
    }

    RELEASE_SEARCH(&desc);
    return ENOERR;
}

INT32 SetDiskPartName(los_part *part, const CHAR *src)
{
    size_t len;
    los_disk *disk = NULL;
    INT32 ret;

    if ((part == NULL) || (src == NULL)) {
        return VFS_ERROR;
    }

    len = strlen(src);
    if ((len == 0) || (len >= DISK_NAME)) {
        return VFS_ERROR;
    }

    disk = get_disk((INT32)part->disk_id);
    if (disk == NULL) {
        return VFS_ERROR;
    }

    DISK_LOCK(disk);
    if (disk->disk_status != STAT_INUSED) {
        goto ERROR_HANDLE;
    }

    part->part_name = (CHAR *)zalloc(len + 1);
    if (part->part_name == NULL) {
        PRINT_ERR("%s[%d] zalloc failure\n", __FUNCTION__, __LINE__);
        goto ERROR_HANDLE;
    }

    ret = strcpy_s(part->part_name, len + 1, src);
    if (ret != EOK) {
        PRINT_ERR("%s[%d] strcpy_s failed, ret: %d\n", __FUNCTION__, __LINE__, ret);
        goto ERROR_HANDLE;
    }

    DISK_UNLOCK(disk);
    return ENOERR;

ERROR_HANDLE:
    DISK_UNLOCK(disk);
    return VFS_ERROR;
}

INT32 add_mmc_partition(struct disk_divide_info *info, size_t sectorStart, size_t sectorCount)
{
    UINT32 index, i;

    if (info == NULL) {
        return VFS_ERROR;
    }

    if ((info->part_count >= MAX_DIVIDE_PART_PER_DISK) || (sectorCount == 0)) {
        return VFS_ERROR;
    }

    if ((sectorCount > info->sector_count) || ((info->sector_count - sectorCount) < sectorStart)) {
        return VFS_ERROR;
    }

    index = info->part_count;
    for (i = 0; i < index; i++) {
        if (sectorStart < (info->part[i].sector_start + info->part[i].sector_count)) {
            return VFS_ERROR;
        }
    }

    info->part[index].sector_start = sectorStart;
    info->part[index].sector_count = sectorCount;
    info->part[index].type = EMMC;
    info->part_count++;

    return ENOERR;
}

VOID show_part(const los_part *part)
{
    if ((part == NULL) || (part->dev == NULL)) {
        PRINT_ERR("part is NULL\n");
        return;
    }

    PRINTK("\npart info :\n");
    PRINTK("disk id          : %u\n", part->disk_id);
    PRINTK("part_id in system: %u\n", part->part_id);
    PRINTK("part no in disk  : %u\n", part->part_no_disk);
    PRINTK("part no in mbr   : %u\n", part->part_no_mbr);
    PRINTK("part filesystem  : %02X\n", part->filesystem_type);
    PRINTK("part dev name    : %s\n", part->dev->i_name);
    PRINTK("part sec start   : %llu\n", part->sector_start);
    PRINTK("part sec count   : %llu\n", part->sector_count);
}

INT32 EraseDiskByID(UINT32 diskId, size_t startSector, UINT32 sectors)
{
    INT32 ret = VFS_ERROR;
#if defined(LOSCFG_DRIVERS_MMC) || defined(LOSCFG_FS_FAT_FTL)
    los_disk *disk = get_disk((INT32)diskId);
    if (disk == NULL) {
        return ret;
    }
    DISK_LOCK(disk);
    if (disk->disk_status != (UINT32)STAT_INUSED) {
        DISK_UNLOCK(disk);
        return ret;
    }

    if (strstr(disk->disk_name, "ftl") != NULL) {
#ifdef LOSCFG_FS_FAT_FTL
        ret = FtlErase(disk->dev, startSector, sectors);
#endif
    } else {
#ifdef LOSCFG_DRIVERS_MMC
        ret = mmcsd_erase(disk->dev, startSector, sectors);
#endif
    }

    DISK_UNLOCK(disk);
#else
    (VOID)diskId;
    (VOID)startSector;
    (VOID)sectors;
#endif
    return ret;
}
