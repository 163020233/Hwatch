/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Fatfs Extend Interface Implementation
 * Author: Huawei LiteOS Team
 * Create: 2022-8-20
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

#include "fatfs_extend.h"
#include "diskio.h"
#include "disk.h"
#include "fatfs_export.h"
#if (defined(LOSCFG_FS_FAT_INDEPENDENT_MEMORY_POOL) && defined(LOSCFG_BOX))
#include "los_memory.h"
#endif

/* Trim cmd opcode */
#define CTRL_TRIM_REVERT    23

#define BS_FAT32                82  /* FAT32 field in MBR */
#define BS_FILESYSTEM           54  /* Filesystem type field in MBR */
#define FAT32_TYPE_VALUE        0x33544146  /* 3TAF */
#define FAT3_TYPE_VALUE         0x544146    /* TAF */
#define DOS_FAT12_MAX_CULSTER   0xFF5
#define VERIFY_FAT32(ptr)       (ld_dword((ptr) + BS_FAT32) == FAT32_TYPE_VALUE)
#define VERIFY_FAT(ptr)         ((ld_dword((ptr) + BS_FILESYSTEM) & 0xFFFFFF) == FAT3_TYPE_VALUE)

#ifdef LOSCFG_FS_FAT_DCACHE
#define DIR_ATTR                11  /* The dir attribute field of fatfs */
#define DIR_CTIME               14  /* The created time field of fatfs */
#define DIR_MTIME               22  /* The modified time field of fatfs */
#define DIR_FILESIZE            28  /* The file size field of fatfs */

#define FILE_MODE_DATTR         (1UL << 0)
#define FILE_SIZE_DATTR         (1UL << 1)
#define BLOCK_SIZE_DATTR        (1UL << 2)
#define CREATE_TIME_DATTR       (1UL << 3)
#define MODIFY_TIME_DATTR       (1UL << 4)
#define DIR_MODE_DATTR          (1UL << 5)
#define DIR_SIZE_DATTR          (1UL << 6)
#endif

#define EXTEND_PT_OFFSET        8
#define PT_MAX_NUMS             4   /* Standard partition max nums */
#define MBR_TABLE               446 /* MBR partition table offset */
#define PTE_SYSTEM              4   /* PTE: file system id */
#define BS_55AA                 510 /* Signature */
#define SZ_PTE                  16  /* Partition table entry size */

typedef struct {
    BOOL gptPart;
    int extendedBr;
    int extendedPos;
    DWORD extendedBase;
    DWORD extendedOffset;
    BYTE* multiBuf;
    UINT multiBufLen;
} PartExtendParam;

UINT gTimeStatus = SYSTEM_TIME_ENABLE;

static UINT RoundUpPower2(UINT size)
{
    if (size > 1) {
        return 1UL << (32 - CLZ(size - 1)); /* 32 bit shift */
    } else {
        return 1;
    }
}

time_t Fat2Time(DWORD time)
{
    struct tm ftm;
    INT year, mon, day, hour, min, sec;
    WORD mtime;

    mtime = time >> 16;     /* 16 : use high 16 bits */
    day = mtime & 0x1F;     /* bit[4:0] Day(1..31) */
    mtime >>= 5;
    mon = mtime & 0x0F;     /* bit[8:5] Month(1..12) */
    mtime >>= 4;
    year = (mtime & 0x7F) + 1980;   /* bit[15:9] Year since 1980(0..127) */

    mtime = (WORD)time;       /* low 16 bits is seconds field */
    sec = (mtime & 0x1F) * 2; /* bit[4:0] Second/2(0..29) */
    mtime >>= 5;
    min = mtime & 0x3F;       /* bit[10:5] Minute(0..59) */
    mtime >>= 6;
    hour = mtime & 0x1F;      /* bit[15:11] Hour(0..23) */

    (void)memset_s(&ftm, sizeof(ftm), 0, sizeof(ftm));
    ftm.tm_year = year - 1900; /* Years since 1900 */
    ftm.tm_mon  = mon - 1;     /* Months since January: 0-11 */
    ftm.tm_mday = day;         /* Day: 1-31 */
    ftm.tm_hour = hour;        /* Hours: 0-23 */
    ftm.tm_min  = min;         /* Minutes: 0-59 */
    ftm.tm_sec  = sec;         /* Seconds: 0-59 */

    return mktime(&ftm);
}

void ff_set_dentry_mtime(void *dentry, const FILINFO* fno)
{
    struct timespec timeStamp = {0};

    timeStamp.tv_sec = Fat2Time((DWORD)fno->fdate << 16 | fno->ftime); /* bit[32 : 16] date field */
    f_dentry_mtime_update(dentry, &timeStamp);
}

QWORD ff_get_align_ausize(QWORD blkSize)
{
    /* SD card only support up to 64MB AU size */
    if (blkSize == 0 || blkSize > 0x20000) blkSize = 1;
    /*
     * If this partition is SD card, the AU size is used. disk_ioctl may return
     * a size which is not power of 2 (12MB or 24MB for example). We mannully round this up.
     * For eMMC disk_ioctl should expect zero.
     */
    blkSize = RoundUpPower2((UINT)blkSize);
    /*
     * sz_rsv will be set to BPB_RsvdSecCnt(2byte) field that ensure
     * sz_rsv is smaller than 0x10000: avoid 16 bit overflow.
     */
    return  (blkSize > 0x8000) ? 0x8000 : blkSize;
}

void ff_print_mkfsinfo(BYTE fatType, QWORD sectors)
{
    switch (fatType) {
        case FS_FAT12:
            PRINTK("format to FAT12, %llu sectors per cluster.\n", sectors);
            break;
        case FS_FAT16:
            PRINTK("Format to FAT16, %llu sectors per cluster.\n", sectors);
            break;
        case FS_FAT32:
            PRINTK("Format to FAT32, %llu sectors per cluster.\n", sectors);
            break;
        default :
            break;
    }
}

void *ff_create_extended_parm(void)
{
    PartExtendParam *exPart = NULL;

    exPart = ff_memalloc(sizeof(PartExtendParam));
    if (exPart == NULL) {
        return NULL;
    }

    exPart->multiBuf = ff_memalloc(FF_MAX_SS);
    if (exPart->multiBuf == NULL) {
        ff_memfree(exPart);
        return NULL;
    }

    exPart->multiBufLen = FF_MAX_SS;
    exPart->extendedPos = -1;
    exPart->extendedBase = 0;
    exPart->extendedOffset = 0;
    exPart->gptPart = FALSE;

    return exPart;
}

void ff_destroy_extended_parm(void *param)
{
    PartExtendParam *exPart = (PartExtendParam *)param;

    ff_memfree(exPart->multiBuf);
    ff_memfree(exPart);
}

static FRESULT GetPartLocation(PartExtendParam *exPart, BYTE *buf, BYTE ipart, int vol)
{
    UINT i;
    BYTE *pte = NULL;

    pte = buf + (MBR_TABLE + (ipart - 1) * SZ_PTE);
    exPart->extendedBr = ipart - PT_MAX_NUMS;
    if (exPart->extendedBr > 0) {
        for (i = 0; i < PT_MAX_NUMS; i++) {
            pte = &buf[MBR_TABLE + i * SZ_PTE];
            if (pte[PT_MAX_NUMS] == 0x0F) {
                exPart->extendedPos = (int)i;
            }
        }
        pte = &buf[MBR_TABLE + exPart->extendedPos * SZ_PTE];
        exPart->extendedBase = ld_dword(pte + EXTEND_PT_OFFSET);
        if (disk_raw_read(LD2DI(vol), buf, exPart->extendedBase, 1) != RES_OK) {
            return FR_DISK_ERR;
        }
        pte = &buf[MBR_TABLE];
        for (; exPart->extendedBr > 1; --exPart->extendedBr) {
            pte = &buf[MBR_TABLE];
            exPart->extendedOffset = ld_dword(pte + SZ_PTE + EXTEND_PT_OFFSET);
            (void)memset_s(buf, FF_MAX_SS, 0, FF_MAX_SS);
            if (disk_raw_read(LD2DI(vol), buf, exPart->extendedBase + exPart->extendedOffset, 1) != RES_OK) {
                return FR_DISK_ERR;
            }
        }
    }

    if (pte[PT_MAX_NUMS] == 0) {  /* No partition? */
        return FR_MKFS_ABORTED;
    }

    return FR_OK;
}

FRESULT ff_get_extended_volume_info(int vol, BYTE *buf, LBA_t *volBase, LBA_t *volSize, void *param)
{
    FRESULT result;
    BYTE pdrv, ipart;
    PartExtendParam *exPart = NULL;

    exPart = (PartExtendParam *)param;
    pdrv = LD2PD(vol);  /* Physical drive */
    ipart = LD2PT(vol); /* Partition (0:create as new, 1..:get from partition table) */

    if (buf[MBR_TABLE + PTE_SYSTEM] == 0xEE) { /* GPT protective MBR? */
        exPart->gptPart = TRUE;
        *volBase = LD2PS(vol); /* Volume start sector */

        /* Load GPT partition info */
        if (disk_read(pdrv, buf, *volBase, 1) != RES_OK) {
            return FR_DISK_ERR;
        }

        /* Check if GPT partition is valid */
        if (ld_word(buf + BS_55AA) != 0xAA55)  {
            return FR_MKFS_ABORTED;
        }
    } else { /* Get the partition location from MBR partition table */
        result = GetPartLocation(exPart, buf, ipart, vol);
        if (result != FR_OK) {
            return result;
        }
    }

    if (memcpy_s(exPart->multiBuf, exPart->multiBufLen, buf, FF_MAX_SS) != EOK) {
        return FR_MKFS_ABORTED;
    }

    *volBase = LD2PS(vol);  /* Volume start sector */
    *volSize = LD2PC(vol);  /* Volume size */

    return FR_OK;
}

FRESULT ff_update_extended_volume_info(int vol, BYTE sys, void *param)
{
    BYTE ipart, *pte = NULL;
    PartExtendParam *exPart = NULL;
    QWORD n;
    LBA_t bVol;

    ipart = LD2PT(vol); /* Partition (0:create as new, 1..:get from partition table) */
    exPart = (PartExtendParam *)param;

    if (!exPart->gptPart) {
        /* Update system ID in the partition table in MBR or EBR */
        if (ipart > PT_MAX_NUMS) {
            pte = &exPart->multiBuf[MBR_TABLE];
        } else {
            pte = &exPart->multiBuf[MBR_TABLE + (ipart - 1) * SZ_PTE];
        }
        n = (ipart > PT_MAX_NUMS) ? exPart->extendedBase + exPart->extendedOffset : 0;
        pte[PT_MAX_NUMS] = sys;
        if (disk_raw_write(LD2DI(vol), exPart->multiBuf, n, 1) != RES_OK) { /* Write it to the MBR */
            return FR_DISK_ERR;
        }
    } else {
        bVol = LD2PS(vol); /* Volume start sector */
        if (disk_raw_read(LD2DI(vol), exPart->multiBuf, bVol, 1) != RES_OK) {
            return FR_DISK_ERR;
        }
        pte = &exPart->multiBuf[MBR_TABLE];
        pte[PT_MAX_NUMS] = sys;
        if (disk_raw_write(LD2DI(vol), exPart->multiBuf, bVol, 1) != RES_OK) { /* Write it to the MBR */
            return FR_DISK_ERR;
        }
    }

    return FR_OK;
}

UINT ff_dbr_verify_fmt(FATFS *fs, DWORD clustNums)
{
    UINT fmt;

    if (VERIFY_FAT32(fs->win)) {
        fmt = FS_FAT32;
    } else if (VERIFY_FAT(fs->win)) {
        fmt = (clustNums <= DOS_FAT12_MAX_CULSTER) ? FS_FAT12 : FS_FAT16;
    } else {
        return FR_NO_FILESYSTEM;
    }

    return fmt;
}

void ff_trim_revert(FATFS *fs, DWORD preClust, DWORD curClust)
{
    LBA_t rt[2];

    rt[0] = clst2sect(fs, preClust);
    rt[1] = clst2sect(fs, curClust) + fs->csize - 1;

    (void)disk_ioctl(fs->pdrv, CTRL_TRIM_REVERT, rt);
}

#ifdef LOSCFG_FS_FAT_DCACHE

static void ff_update_dentry_attr(FATFS *fs, DIR *dir, FIL *file, void *dentry, UINT flags)
{
    struct timespec timeStamp = {0};

    if (flags & FILE_MODE_DATTR) {
        f_dentry_mode_update(dentry, AM_ARC);
    }

    if (flags & FILE_SIZE_DATTR) {
        f_dentry_size_update(dentry, (file != NULL) ? file->obj.objsize : 0);
    }

    if ((flags & BLOCK_SIZE_DATTR) && (fs != NULL)) {
        f_dentry_blocksize_update(dentry, fs->ssize * fs->csize);
    }

    if ((flags & CREATE_TIME_DATTR) && (gTimeStatus == SYSTEM_TIME_ENABLE)) {
        if (dir != NULL) {
            timeStamp.tv_sec = Fat2Time(ld_dword(dir->dir + DIR_CTIME));
        } else {
            (VOID)clock_gettime(CLOCK_REALTIME, &timeStamp);
        }

        f_dentry_ctime_update(dentry, &timeStamp);
    }

    if ((flags & MODIFY_TIME_DATTR) && (gTimeStatus == SYSTEM_TIME_ENABLE)) {
        if (dir != NULL) {
            timeStamp.tv_sec = Fat2Time(ld_dword(dir->dir + DIR_MTIME));
        } else {
            (VOID)clock_gettime(CLOCK_REALTIME, &timeStamp);
        }

        f_dentry_mtime_update(dentry, &timeStamp);
    }

    if ((flags & DIR_MODE_DATTR) && (dir != NULL)) {
        f_dentry_mode_update(dentry, dir->dir[DIR_ATTR]);
    }

    if ((flags & DIR_SIZE_DATTR) && (dir != NULL)) {
        f_dentry_size_update(dentry, ld_dword(dir->dir + DIR_FILESIZE));
    }
}


void *ff_follow_dentry_path(FATFS *fs, DIR *dir, void *parent)
{
    LosDentry *dentry = NULL;

    dentry = f_dentry_new((LosDentry *)parent, fs->lfnbuf);
    if (dentry != NULL) {
        ff_update_dentry_attr(fs, dir, NULL, dentry, DIR_MODE_DATTR | DIR_SIZE_DATTR |
            BLOCK_SIZE_DATTR | CREATE_TIME_DATTR | MODIFY_TIME_DATTR);
    }

    return dentry;
}

void ff_create_dentry_attr(FIL *fp, void *last, FATFS *fs)
{
    if (fp->dentry == NULL) { /* no file */
        fp->dentry = f_dentry_new(last, fs->lfnbuf);
    }

    if (fp->dentry == NULL) { /* no dentry cache */
        return;
    }

    ff_update_dentry_attr(fs, NULL, NULL, fp->dentry, FILE_MODE_DATTR | FILE_SIZE_DATTR |
        BLOCK_SIZE_DATTR | CREATE_TIME_DATTR | MODIFY_TIME_DATTR);

    return;
}

void ff_sync_dentry_attr(FIL *fp)
{
    ff_update_dentry_attr(NULL, NULL, fp, fp->dentry, FILE_SIZE_DATTR | MODIFY_TIME_DATTR);
}

void ff_del_dentry_attr(void *dentry)
{
    f_dentry_destroy(dentry);
}

#endif

#ifdef LOSCFG_FS_FAT_INDEPENDENT_MEMORY_POOL
UINT8 *g_fatMemPool = NULL;
UINT32 LOS_FatSetMemPool(VOID *pool, UINT32 size)
{
    UINT32 ret;
    ret = LOS_MemInit(pool, size);
    if (ret == LOS_OK) {
        g_fatMemPool = (UINT8 *)pool;
    }
    return ret;
}
#endif

VOID *fat_memzalloc(UINT32 size)
{
#ifdef LOSCFG_FS_FAT_INDEPENDENT_MEMORY_POOL
    VOID *ptr = NULL;
    ptr = LOS_MemAlloc((VOID *)g_fatMemPool, size);
    if (ptr != NULL) {
        (void)memset_s(ptr, size, 0, size);
    }
    return ptr;
#else
    return zalloc(size);
#endif
}

VOID *fat_memalloc(UINT32 size)
{
#ifdef LOSCFG_FS_FAT_INDEPENDENT_MEMORY_POOL
    return LOS_MemAlloc((VOID *)g_fatMemPool, size);
#else
    return malloc(size);
#endif
}

VOID *fat_memalloc_align(UINT32 size, UINT32 align)
{
#ifdef LOSCFG_FS_FAT_INDEPENDENT_MEMORY_POOL
    return LOS_MemAllocAlign((VOID *)g_fatMemPool, size, align);
#else
    return memalign(align, size);
#endif
}

VOID fat_memfree(VOID *ptr)
{
#ifdef LOSCFG_FS_FAT_INDEPENDENT_MEMORY_POOL
    (VOID)LOS_MemFree((VOID *)g_fatMemPool, ptr);
#else
    free(ptr);
#endif
}
