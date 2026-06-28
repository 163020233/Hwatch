/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2019. All rights reserved.
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

#ifdef LOSCFG_FS_FAT
#include "ff.h"
#include "fatfs.h"
#include "errno.h"

#include "fatfs_export.h"
#include "fatfs_extend.h"
#include "disk_pri.h"
#include "los_tables.h"

#ifdef LOSCFG_FS_FAT_VIRTUAL_PARTITION
#include "virpart.h"
#include "errcode_fat.h"
#include "virpartff.h"
#endif

#ifdef LOSCFG_FS_FAT_CACHE
#include "bcache.h"
#endif
#include "fatfs_dentry.h"

#ifdef LOSCFG_FS_FAT_UNI_TRIM
#include "utrim.h"
#endif

#ifdef LOSCFG_FS_FAT_STACK_OVERFLOW_CHECK
#include "los_task.h"
#include "los_task_pri.h"
#endif

#ifdef LOSCFG_FS_FAT_VIRTUAL_PARTITION
FAT_VIR_PART FatVirPart = {0};
#endif
__attribute__((weak)) PARTITION VolToPart[FF_VOLUMES];
static struct inode *DiskDev[FF_VOLUMES];
char FatLabel[LABEL_LEN];

#define DRIVER_NAME_LEN	8

enum {
	FAT_STATUS_UNBIND,
	FAT_STATUS_BIND
};

typedef struct {
	DIR		f_dir;
	FILINFO	fno;
} DIR_FILE;

#define MIN(a, b, c)	(c = (a < b) ? a : b)

#define BOOT_SECTOR_RESERVED1	65
#define BOOT_SECTOR_NUM			0
#define BOOT_SECTOR_CNT			1
#define BOOT_SECTOR_SIZE		512 /* BPB BIOS Parameter block struct just for FAT32 */

BYTE FatMountStatus = FAT_STATUS_UNBIND;

#ifdef LOSCFG_FS_FAT_STACK_OVERFLOW_CHECK
/*
 * Different board consume different stack size, custom implement stack waterlevel in board plartform.h header file.
 * The following default value STACK_WARNING_HIGH_LEVEL is used in interface (open fstat unlink) with LNF buffer
 * which 512 bytes size, The value STACK_WARNING_LOW_LEVEL is used in interface (write read) without LFN buffer.
 */
#ifndef STACK_WARNING_HIGH_LEVEL
#ifdef LOSCFG_FS_FAT_HEAP_LFN
#define STACK_WARNING_HIGH_LEVEL 0x700
#elif LOSCFG_FS_FAT_STACK_LFN
/* LFN use extra 0x200 stack size */
#define STACK_WARNING_HIGH_LEVEL 0x900
#endif
#endif

#ifndef STACK_WARNING_LOW_LEVEL
#define STACK_WARNING_LOW_LEVEL 0x500
#endif

static TSK_INFO_S g_stackInfo;

#define fatfs_stackcheck(stack_size)  do { \
	(void)LOS_TaskInfoGet(OsCurrTaskGet()->taskId, &g_stackInfo); \
	if (g_stackInfo.uwStackSize - g_stackInfo.uwCurrUsed < stack_size) { \
		PRINT_ERR("task [%s:%u, %s] stack warning!!! total %u, used %u, peak size %u\n", OsCurrTaskGet()->taskName, \
			OsCurrTaskGet()->taskId, __func__, g_stackInfo.uwStackSize, g_stackInfo.uwCurrUsed, g_stackInfo.uwPeakUsed); \
	} \
} while (0)
#else
#define fatfs_stackcheck(stack_size)
#endif

static void fatfs_settimestatus(UINT status)
{
	gTimeStatus = status;
}

static FRESULT fatfs_fcheckfat(DIR_FILE* dir_info)
{
	FRESULT res;
	FATFS *fs;
	DWORD val = 0, tcl, clust_size;

	res = validate(&(dir_info->f_dir.obj), &fs);	/* Lock volume */
	if (res == FR_OK) {
		clust_size = (DWORD)fs->csize * SS(fs);	/* Cluster size */
		tcl = (DWORD)(dir_info->fno.fsize / clust_size) + ((dir_info->fno.fsize & (clust_size - 1)) ? 1 : 0);	/* Number of clusters required */
		if (dir_info->fno.fsize == 0) {	/* When set file size to zero, remove entire cluster chain */
			if (dir_info->fno.sclst != 0) {
#if !FF_FS_READONLY
				res = remove_chain(&(dir_info->f_dir.obj), dir_info->fno.sclst, 0);
#else
				res = FR_DENIED;
#endif
			} else
				res = FR_OK;
		} else {			/* When truncate a part of the file, remove remaining clusters */
			val = get_fat(&(dir_info->f_dir.obj), dir_info->fno.sclst + tcl - 1);
			if (val == 0xFFFFFFFF) {
				res = FR_DISK_ERR;
				LEAVE_FF(fs, res);
			}
			if (val == 1) {
				res = FR_INT_ERR;
				LEAVE_FF(fs, res);
			}
			if (val == 0x0FFFFFFF) {
				res = FR_OK;
				LEAVE_FF(fs, res);
			}
			if (val < fs->n_fatent) {
#if !FF_FS_READONLY
				res = remove_chain(&(dir_info->f_dir.obj), val, dir_info->fno.sclst + tcl - 1);
#else
				res = FR_DENIED;
#endif
			}
		}
	}

	LEAVE_FF(fs, res);
}

static FRESULT fatfs_getclustinfo(FIL* fp, DWORD* fclust, DWORD* fcount)
{
	UINT count = 0;
	FATFS *fs;
	FRESULT ret;
	ret = validate(&fp->obj,&fs);
	if (ret != FR_OK) LEAVE_FF(fs,ret);

	count = f_getclustinfo(fp, fclust);
	if (count == 0xFFFFFFFF)
		LEAVE_FF(fs,FR_DENIED);

	*fcount = count;
	LEAVE_FF(fs,FR_OK);
}

static FRESULT fatfs_checkopenlock(int index)	/* check lock entries is empty or not by index. */
{
#if FF_FS_LOCK != 0
	FATFS* fs = FatFs[index];

	if (!lock_volume(fs, 0)) return FR_TIMEOUT;	/* Lock the volume */

#ifdef LOSCFG_FS_FAT_VIRTUAL_PARTITION
	UINT i;
	FRESULT res;

	if (ISVIRPART(fs)) {
		for (i = 0; i < fs->vir_amount; i++) {
			if (!(CHILDFS(fs, i)) || (CHILDFS(fs, i))->namelabel == NULL) {
				continue;
			}
			res = empty_lock(CHILDFS(fs, i));
			if (res != FR_OK) {
				return res;
			}
		}
	}
#endif
	return empty_lock(fs);
#else
	return FR_OK;
#endif
}

static void fatfs_checkopenunlock(int index)
{
	unlock_volume(FatFs[index], FR_OK);
}

/* Returns -1 if there is no space to install fatfs driver, else returns >=0 */
static INT fatfs_get_disk(const struct inode *node)
{
	INT index;

	for (index = 0; index < FF_VOLUMES; index++) {
		if (DiskDev[index] == node) {
			return index;
		}
	}

	return FAT_ERROR;
}

static INT fatfs_direct_sync(const FATFS *fat)
{
#ifdef LOSCFG_FS_FAT_CACHE
	los_part *part = get_part((INT)fat->pdrv);
	if (part == NULL)
		return -ENODEV;

	if (OsSdSync(part->disk_id) != ENOERR)
		return -EIO;
#endif
	return ENOERR;
}

static INT fatfs_sd_sync(unsigned long mountflags, const FATFS *fat)
{
#ifdef LOSCFG_FS_FAT_CACHE
	if (mountflags != MS_NOSYNC) {
		return fatfs_direct_sync(fat);
	}
#endif
	return ENOERR;
}

int fatfs_get_vol(void *fat)
{
	int vol;

	for (vol = 0; vol < FF_VOLUMES; vol ++) {
		if (FatFs[vol] == (FATFS *)fat)
			return vol;
	}

	return -1;
}

INT fatfs_2_vfs(INT result)
{
	INT status = ENOERR;

#ifdef LOSCFG_FS_FAT_VIRTUAL_PARTITION
	if (result < 0 || result >= VIRERR_BASE) {
		return result;
	}
#else
	if (result < 0) {
		return result;
	}
#endif

	/* FatFs errno to Libc errno */
	switch (result) {
	case FR_OK:
		break;

	case FR_NO_FILE:
	case FR_NO_PATH:
	case FR_NO_FILESYSTEM:
		status = -ENOENT;
		break;

	case FR_INVALID_NAME:
		status = -EINVAL;
		break;

	case FR_EXIST:
	case FR_INVALID_OBJECT:
		status = -EEXIST;
		break;

	case FR_DISK_ERR:
	case FR_NOT_READY:
	case FR_INT_ERR:
		status = -EIO;
		break;

	case FR_WRITE_PROTECTED:
		status = -EROFS;
		break;
	case FR_MKFS_ABORTED:
	case FR_INVALID_PARAMETER:
		status = -EINVAL;
		break;

	case FR_NO_SPACE_LEFT:
		status = -ENOSPC;
		break;
	case FR_NO_DIRENTRY:
		status = -ENFILE;
		break;
	case FR_NO_EMPTY_DIR:
		status = -ENOTEMPTY;
		break;
	case FR_IS_DIR:
		status = -EISDIR;
		break;
	case FR_NO_DIR:
		status = -ENOTDIR;
		break;
	case FR_NO_EPERM:
	case FR_DENIED:
		status = -EPERM;
		break;
	case FR_LOCKED:
		status = -EBUSY;
		break;
#ifdef LOSCFG_FS_FAT_VIRTUAL_PARTITION
	case FR_MODIFIED:
		status = VIRERR_MODIFIED;
		break;
	case FR_CHAIN_ERR:
		status = VIRERR_CHAIN_ERR;
		break;
	case FR_OCCUPIED:
		status = VIRERR_OCCUPIED;
		break;
	case FR_NOTCLEAR:
		status = VIRERR_NOTCLEAR;
		break;
	case FR_NOTFIT:
		status = VIRERR_NOTFIT;
		break;
	case FR_INVAILD_FATFS:
		status = VIRERR_INTER_ERR;
		break;
#endif
	default:
		status = FAT_ERROR;
		break;
    }

    return status;
}

static int fatfs_bind_check(struct inode *blkdriver, los_part **partition)
{
	los_part *part = NULL;
	if (blkdriver == NULL || blkdriver->u.i_bops == NULL)
		return -ENODEV;

	if (blkdriver->u.i_bops->open != NULL && blkdriver->u.i_bops->open(blkdriver) != 0)
		return -ENODEV;

	part = los_part_find(blkdriver);
	if (part == NULL)
		return -ENODEV;

	if (part->part_name != NULL)
		return -EACCES;

#if !FF_MULTI_PARTITION
	if (part->part_no_mbr > 1)
		return -EPERM;
#endif

	*partition = part;
	return ENOERR;
}

static int fatfs_stat_set(int fd, unsigned char value)
{
	BYTE sector_buffer[BOOT_SECTOR_SIZE];
	los_part *part = get_part(fd);
	QWORD sector = BOOT_SECTOR_NUM;
	const UINT count = BOOT_SECTOR_CNT;
	INT ret;

	if (part->type != EMMC)
		sector += part->sector_start;

	ret = los_part_read(fd, sector_buffer, sector, count);
	if (ret < 0) {
		PRINTK("%s read ret=%d\n", __FUNCTION__, ret);
		return FR_DISK_ERR;
	}
	sector_buffer[BOOT_SECTOR_RESERVED1] = value;
	ret = los_part_write(fd, sector_buffer, sector, count);
	if (ret < 0) {
		PRINTK("%s ret=%d\n", __FUNCTION__, ret);
		return FR_DISK_ERR;
	}
	return FR_OK;
}

static int fatfs_stat_get(int fd, unsigned char *value)
{
	BYTE sector_buffer[BOOT_SECTOR_SIZE];
	los_part *part = get_part(fd);
	QWORD sector = BOOT_SECTOR_NUM;
	const UINT count = BOOT_SECTOR_CNT;
	INT ret;

	if (part->type != EMMC)
		sector += part->sector_start;

	ret = los_part_read(fd, sector_buffer, sector, count);
	if (ret < 0) {
		PRINTK("%s read ret=%d\n", __FUNCTION__, ret);
		return FR_DISK_ERR;
	}
	*value = sector_buffer[BOOT_SECTOR_RESERVED1];
	return FR_OK;
}

int fatfs_bind(struct inode *blkdriver, const void *data, void **handle, const char *relpath)
{
	FATFS *fat = NULL;
	INT result;
	INT index;
	INT err;
	los_part *part = NULL;
	char drive[DRIVER_NAME_LEN];
#ifdef LOSCFG_FS_FAT_VIRTUAL_PARTITION
	struct inode_search_s desc;
#endif

	fatfs_stackcheck(STACK_WARNING_LOW_LEVEL);

	result = fatfs_bind_check(blkdriver, &part);
	if (result != 0)
		return result;

	err = SetDiskPartName(part, relpath);
	if (err != 0) {
		err = -ENOMEM;
		goto ERROR_WITH_CLOSE;
	}

	index = fatfs_get_disk((struct inode *)NULL);
	if (index >= FF_VOLUMES || index < 0) {
		err = -EBUSY;
		goto ERROUT_WITH_FREE;
	}
	VolToPart[index].di = part->disk_id;
	VolToPart[index].pd = part->part_id;
	VolToPart[index].pt = part->part_no_mbr;
	VolToPart[index].ps = part->sector_start;
	VolToPart[index].pc = part->sector_count;

	fat = (FATFS *)fat_memzalloc(sizeof(FATFS));
	if (fat == NULL) {
		err = -ENOMEM;
		goto ERROUT_WITH_DISK;
	}
	result = snprintf_s(drive, sizeof(drive), sizeof(drive) - 1, "%d:/", index);
	if (result < 0) {
		err = -EINVAL;
		fat_memfree(fat);
		goto ERROUT_WITH_DISK;
	}

	/* save blockdriver inode : driver will use in f_mount */
	DiskDev[index] = blkdriver;
	/* mount fatfs, mmc always 0 logic driver */
	result = f_mount(fat, drive, 1);
	if (result != FR_OK) {
		err = fatfs_2_vfs(result);
		goto ERROUT_WITH_MOUNT;
	}

	DIR *dir = NULL;

	dir = (DIR *)fat_memalloc(sizeof(DIR));
	if (dir == NULL) {
		err = -ENOMEM;
		goto ERROUT_WITH_MOUNT;
	}

#if FF_FS_MINIMIZE <= 1
	/* open the root directory, returns error if fatfs is invalid */
	result = f_opendir(dir, drive);
	if (result != FR_OK) {
		err = fatfs_2_vfs(result);
		fat_memfree(dir);
		goto ERROUT_WITH_MOUNT;
	}
	result = f_closedir(dir);
	if (result != FR_OK) {
		err = fatfs_2_vfs(result);
		fat_memfree(dir);
		goto ERROUT_WITH_MOUNT;
	}
#else
	PRINT_ERR("cannot open root directory to check if fatfs is invalid!\n");
#endif
	/* mount ok! */
	*handle = (void *)fat;
	fat_memfree(dir);
	(void)fatfs_stat_get(part->part_id, &FatMountStatus);
	if (FatMountStatus == FAT_STATUS_UNBIND) {
		(void)fatfs_stat_set(part->part_id, FAT_STATUS_BIND);
#ifdef LOSCFG_FS_FAT_CACHE
		if (OsSdSync(part->disk_id) != ENOERR) {
			err = -EIO;
			goto ERROUT_WITH_MOUNT;
		}
#endif
	}
#ifdef LOSCFG_FS_FAT_VIRTUAL_PARTITION
	/* Parent Partition Mount Success, then process virtual partition mount operation */
	if (result == FR_OK) {
		los_disk *disk = get_disk(part->disk_id);
		if (disk->sector_count > UINT_MAX) {
			return VIRERR_NOTFIT;
		}
		if (fat->fs_type != FS_FAT32)
			return VIRERR_NOTFIT;
		SETUP_SEARCH(&desc, FatVirPart.virtualinfo.devpartpath, false);
		result = inode_find(&desc);
		RELEASE_SEARCH(&desc);
		if (result < 0 || blkdriver != desc.node) {
			return VIRERR_NOPARAM;
		} else {
			result = fatfs_bind_vir_part(fat, index);
			if (result != FR_OK && result < VIRERR_BASE) {
				return VIRERR_INTER_ERR;
			}
		}
	}
#endif

#ifdef  LOSCFG_FS_FAT_UNI_TRIM
	part->trimManager = UniTrimInit(part->dev, fat->csize, part->sector_count);
#endif

	fatfs_dentry_init(fat);

	fat->fsi_check = TRUE;

	return fatfs_2_vfs(result);

ERROUT_WITH_MOUNT:
	(void)f_mount((FATFS *)NULL, drive, 0);
	if (fat->win != NULL)
		fat_memfree(fat->win);
	fat_memfree(fat);

ERROUT_WITH_DISK:
	DiskDev[index] = (struct inode *)NULL;

ERROUT_WITH_FREE:
	free(part->part_name);
	part->part_name = NULL;
ERROR_WITH_CLOSE:
	if (blkdriver->u.i_bops->close != NULL) {
		(void)blkdriver->u.i_bops->close(blkdriver);
	}
	return err;
}

int fatfs_unbind(void *handle, struct inode **blkdriver)
{
	INT index;
	FATFS *fat = NULL;
	INT result;
	los_part *part = NULL;
	char drive[DRIVER_NAME_LEN];
#ifdef LOSCFG_FS_FAT_VIRTUAL_PARTITION
	struct inode_search_s desc;
#endif

	fatfs_stackcheck(STACK_WARNING_LOW_LEVEL);

	fat = (FATFS *)handle;
	if (fat == NULL)
		return -EINVAL;
	index = fatfs_get_vol(fat);
	if (index >= FF_VOLUMES || index < 0)
		return -EINVAL;
	result = snprintf_s(drive, sizeof(drive), sizeof(drive) - 1, "%d:/", index);
	if (result < 0)
		return -EINVAL;

#ifdef LOSCFG_FS_FAT_VIRTUAL_PARTITION
	/* Release virtual partition objects before unbind real partition */
	result = fatfs_unbind_vir_part(fat);
	if (result == FR_OK) {
		part = los_part_find(DiskDev[index]);
		SETUP_SEARCH(&desc, FatVirPart.virtualinfo.devpartpath, false);
		result = inode_find(&desc);
		RELEASE_SEARCH(&desc);
		if (part == NULL || (result >= 0 && part->dev == desc.node)) {
			(void)memset_s(DevPartName, sizeof(DevPartName), 0, sizeof(DevPartName));
			(void)memset_s(&FatVirPart.virtualinfo, sizeof(virpartinfo), 0, sizeof(virpartinfo));
			FatVirPart.isparamset = FALSE;
		}
	} else {
		return fatfs_2_vfs(result);
	}
#endif
	result = f_mount((FATFS *)NULL, drive, 0);
	if (result != FR_OK)
		return fatfs_2_vfs(result);

	VolToPart[index].di = 0;
	VolToPart[index].pd = 0;
	VolToPart[index].pt = 0;
	VolToPart[index].ps = 0;
	VolToPart[index].pc = 0;

	part = los_part_find(DiskDev[index]);
	if (part != NULL) {
		(void)fatfs_stat_set(part->part_id, FatMountStatus);
#ifdef LOSCFG_FS_FAT_CACHE
		if (OsSdSync(part->disk_id) != ENOERR) {
			PRINT_ERR("OsSdSync failed in %s\n", __FUNCTION__);
		}
#endif
		if (part->part_name) {
			free(part->part_name);
			part->part_name = NULL;
		}
#ifdef LOSCFG_FS_FAT_UNI_TRIM
		if (part->trimManager != NULL) {
			UniTrimDeinit(part->trimManager);
			part->trimManager = NULL;
		}
#endif
	}

	fatfs_dentry_deinit(fat);

	if (blkdriver != NULL)
		*blkdriver = DiskDev[index];

	if (DiskDev[index]->u.i_bops->close != NULL) {
		result = DiskDev[index]->u.i_bops->close(DiskDev[index]);
	}
	DiskDev[index] = (struct inode *)NULL;
	if (fat->win != NULL)
		fat_memfree(fat->win);
	fat_memfree(fat);

	return fatfs_2_vfs(result);
}

static BYTE fatfs_get_mode(int oflags)
{
	BYTE f_mode = FA_READ;

	if ((UINT)oflags & O_WRONLY)
		f_mode |= FA_WRITE;
	if (((UINT)oflags & O_ACCMODE) & O_RDWR)
		f_mode |= FA_WRITE;
	/* Creates a new file if the file is not existing, otherwise, just open it. */
	if ((UINT)oflags & O_CREAT) {
		f_mode |= FA_OPEN_ALWAYS;
		/* Creates a new file. If the file already exists, the function shall fail. */
		if ((UINT)oflags & O_EXCL)
			f_mode |= FA_CREATE_NEW;
	}
	/* Creates a new file. If the file already exists, its length shall be truncated to 0. */
	if ((UINT)oflags & O_TRUNC)
		f_mode |= FA_CREATE_ALWAYS;

	return f_mode;
}

int fatfs_open(struct file *filep, const char *relpath, int oflags, mode_t mode)
{
	BYTE f_mode;
	char *f_path = NULL;
	FATFS *fat = NULL;
	FIL *fp = NULL;
	FRESULT result;
	INT vol, ret;

	fatfs_stackcheck(STACK_WARNING_HIGH_LEVEL);

	if (filep == NULL || filep->f_inode == NULL || filep->f_inode->i_private == NULL || relpath == NULL)
		return -EINVAL;

	if ((UINT)oflags & O_DIRECTORY)
		return -EACCES;

	if (strlen(relpath) > MAX_LFNAME_LENTH - VOLUME_CHAR_LENGTH)
		return -ENAMETOOLONG;

	if (filep->f_inode->mountflags == MS_RDONLY && oflags != O_RDONLY)
		return -EACCES;

	fat = (FATFS *)filep->f_inode->i_private;

	vol = fatfs_get_vol(fat);
	if (vol < 0 || vol > FF_VOLUMES)
		return -ENOENT;

	f_path = (char *)fat_memalloc(MAX_LFNAME_LENTH);
	if (f_path == NULL)
		return -ENOMEM;

	ret = snprintf_s(f_path, MAX_LFNAME_LENTH, MAX_LFNAME_LENTH - 1, "%d:%s", vol, relpath);
	if (ret < 0) {
		fat_memfree(f_path);
		return -EINVAL;
	}

	f_mode = fatfs_get_mode(oflags);

	fp = (FIL *)fat_memzalloc(sizeof(FIL));
	if (fp == NULL) {
		fat_memfree(f_path);
		return -ENOMEM;
	}
#if !FF_FS_TINY
	fp->buf = NULL;
#endif

	result = f_open(fp, f_path, f_mode);
	fat_memfree(f_path);
	if (result != FR_OK) {
		/* open failed, release memory and return errno */
		goto ERROUT;
	}
	filep->f_pos  = fp->fptr;

	if ((UINT)oflags & O_APPEND) {
#if FF_FS_MINIMIZE <= 2
		/* seek to the end of file */
		(void)f_lseek(fp, fp->obj.objsize);
		filep->f_pos = fp->fptr;
#else
		PRINT_ERR("f_lseek is not supported, O_APPEND won't take effect!\n");
#endif
	}

	ret = fatfs_sd_sync(filep->f_inode->mountflags, fat);
	if (ret != ENOERR) {
		result = FR_DISK_ERR;
		(void)f_close(fp);
		goto ERROUT;
	}
	filep->f_priv = fp;

	return fatfs_2_vfs(result);

ERROUT:
#if !FF_FS_TINY
	if (fp->buf != NULL)
		fat_memfree(fp->buf);
#endif
	fat_memfree(fp);
	return fatfs_2_vfs(result);
}

int fatfs_close(struct file *filep)
{
	FATFS *fat = NULL;
	FIL *fp = NULL;
	FRESULT result;
	INT vol;
	INT ret;

	fatfs_stackcheck(STACK_WARNING_LOW_LEVEL);

	if (filep == NULL || filep->f_inode == NULL || filep->f_inode->i_private == NULL)
		return -EINVAL;

	fat = (FATFS *)filep->f_inode->i_private;
	vol = fatfs_get_vol(fat);
	if (vol < 0 || vol > FF_VOLUMES)
		return -ENOENT;

	if (!(lock_volume(fat, 0)))
		return -EBUSY;
	if (filep->f_priv == NULL) {
		unlock_volume(fat, FR_OK);
		return -EINVAL;
	}

	fp = (FIL *)(filep->f_priv);
	result = f_close(fp);
	if (result == FR_OK) {
		/* close succeed, release memory */
#if !FF_FS_TINY
		if (fp->buf != NULL)
			fat_memfree(fp->buf);
#endif
		fat_memfree(fp);
		filep->f_priv = NULL;
#ifdef LOSCFG_FS_FAT_CACHE_SYNC_THREAD
		ret = fatfs_sd_sync(filep->f_inode->mountflags, fat);
#else
		ret = fatfs_direct_sync(fat);
#endif
		if (ret != ENOERR)
			PRINTK("WARNNING: sd sync failed, return %d\n", ret);
	}
	unlock_volume(fat, FR_OK);
	return fatfs_2_vfs(result);
}

int fatfs_ioctl(FAR struct file *filep, int cmd, unsigned long arg)
{
	return -ENOSYS;
}

ssize_t fatfs_read(struct file *filep, char *buffer, size_t buflen)
{
	FIL *fp = NULL;
	FRESULT result;
	UINT byte_read;

	fatfs_stackcheck(STACK_WARNING_LOW_LEVEL);

	fp = (FIL *)(filep->f_priv);
	FAT_CHECK(fp);

	result = f_read(fp, buffer, buflen, &byte_read);
	/* file position should be updated */
	filep->f_pos = fp->fptr;

	if (result == FR_OK)
		return byte_read;

	return fatfs_2_vfs(result);
}

ssize_t fatfs_write(struct file *filep, const char *buffer, size_t buflen)
{
#if !FF_FS_READONLY
	FIL *fp = (FIL *)(filep->f_priv);
	FRESULT result;
	UINT byte_write;

	fatfs_stackcheck(STACK_WARNING_LOW_LEVEL);

	if (filep->f_inode->mountflags == MS_RDONLY)
		return -EACCES;

	FAT_CHECK(fp);

	result = f_write(fp, buffer, buflen, &byte_write);
	/* file position should be updated */
	filep->f_pos  = fp->fptr;
#ifdef LOSCFG_FS_FAT_WRITE_WITH_SYNC
	if (f_sync(fp) != FR_OK)
		PRINT_ERR("fatfs_write f_sync error \n");
#endif
	if (result == FR_OK)
		return byte_write;

	return fatfs_2_vfs(result);
#else
	return -ENOSYS;
#endif
}

int fatfs_sync(struct file *filep)
{
#if !FF_FS_READONLY
	FATFS *fat = NULL;
	FIL *fp = NULL;
	FRESULT result;
	INT vol;
#ifdef LOSCFG_FS_FAT_CACHE
	los_part *part = NULL;
#endif

	fatfs_stackcheck(STACK_WARNING_LOW_LEVEL);

	if (filep == NULL || filep->f_inode == NULL || filep->f_inode->i_private == NULL)
		return -EINVAL;

	if (filep->f_inode->mountflags == MS_RDONLY)
		return -EACCES;

	fat = (FATFS *)filep->f_inode->i_private;
	vol = fatfs_get_vol(fat);
	if (vol < 0 || vol > FF_VOLUMES)
		return -ENOENT;

	if (!(lock_volume(fat, 0)))
		return -EBUSY;
	if (filep->f_priv == NULL) {
		unlock_volume(fat, FR_OK);
		return -EINVAL;
	}

	fp = (FIL *)(filep->f_priv);
	result = f_sync(fp);
	unlock_volume(fat, FR_OK);

#ifdef LOSCFG_FS_FAT_CACHE
	if (result == FR_OK) {
		part = get_part((INT)fat->pdrv);
		if (part == NULL)
			return -ENODEV;
		if (OsSdSync(part->disk_id) != ENOERR) {
			return -EIO;
		}
	}
#endif
	return fatfs_2_vfs(result);
#else
	return -ENOSYS;
#endif
}

int fatfs_dup(FAR const struct file *oldp, FAR struct file *newp)
{
	return -ENOSYS;
}

off_t fatfs_seek(struct file *filep, off_t offset, int whence)
{
#if FF_FS_MINIMIZE <= 2
	FRESULT result;
	FIL *fp = (FIL *)(filep->f_priv);

	fatfs_stackcheck(STACK_WARNING_LOW_LEVEL);

	FAT_CHECK(fp);

	switch (whence) {
	case SEEK_SET: /* The offset is set to offset bytes. */
		break;

	case SEEK_CUR: /* The offset is set to its current location plus offset bytes. */
		offset += (off_t)filep->f_pos;
		break;

	case SEEK_END: /* The offset is set to the size of the file plus offset bytes. */
		offset += (off_t)fp->obj.objsize;
		break;

	default:
		return -EINVAL;
	}

	if (offset < 0) {
		return -EINVAL;
	}

	result = f_lseek(fp, offset);
	if (result == FR_OK) {
		/* return current position */
		filep->f_pos = (off_t)fp->fptr;
		return (off_t)fp->fptr;
	}

	return fatfs_2_vfs(result);
#else
	return -ENOSYS;
#endif
}

loff_t fatfs_seek64(struct file *filep, loff_t offset, int whence)
{
#if FF_FS_MINIMIZE <= 2
	FRESULT result;
	FIL *fp = (FIL *)(filep->f_priv);

	fatfs_stackcheck(STACK_WARNING_LOW_LEVEL);

	FAT_CHECK(fp);

	if (offset >= FAT32_MAXSZIE || offset <= -FAT32_MAXSZIE)
		return -EINVAL;

	switch (whence) {
	case SEEK_SET: /* The offset is set to offset bytes. */
		break;

	case SEEK_CUR: /* The offset is set to its current location plus offset bytes. */
		offset += filep->f_pos;
		break;

	case SEEK_END: /* The offset is set to the size of the file plus offset bytes. */
		offset += (loff_t)fp->obj.objsize;
		break;

	default:
		return -EINVAL;
	}

	if (offset < 0)
		return -EINVAL;

	result = f_lseek(fp, offset);
	if (result == FR_OK) {
		/* return current position */
		filep->f_pos = (loff_t)fp->fptr;
		return (loff_t)fp->fptr;
	}

	return fatfs_2_vfs(result);
#else
	return -ENOSYS;
#endif
}

int fatfs_unlink(struct inode *mountpt, const char *relpath)
{
#if FF_FS_MINIMIZE == 0 && !FF_FS_READONLY
	FATFS *fat = NULL;
	FRESULT result;
	INT ret, vol;
	char *f_path = NULL;
	struct stat st;

	fatfs_stackcheck(STACK_WARNING_HIGH_LEVEL);

	if (strlen(relpath) > MAX_LFNAME_LENTH - VOLUME_CHAR_LENGTH)
		return -ENAMETOOLONG;

	if (mountpt->mountflags == MS_RDONLY)
		return -EACCES;

	ret = fatfs_stat(mountpt, relpath, &st);
	if (ret != FR_OK)
		return ret;

	switch (st.st_mode & S_IFMT) {
	case S_IFREG:
		break;
	case S_IFDIR:
		return -EISDIR;
	default:
		/* unknown file type */
		return FAT_ERROR;
	}

	fat = (FATFS *)mountpt->i_private;
	vol = fatfs_get_vol(fat);
	if (vol < 0 || vol > FF_VOLUMES)
		return -ENOENT;

	f_path = (char *)fat_memalloc(MAX_LFNAME_LENTH);
	if (f_path == NULL)
		return -ENOMEM;

	ret = snprintf_s(f_path, MAX_LFNAME_LENTH, MAX_LFNAME_LENTH - 1, "%d:%s", vol, relpath);
	if (ret < 0) {
		fat_memfree(f_path);
		return -EINVAL;
	}

	result = f_unlink(f_path);

	fat_memfree(f_path);

	if (result == FR_OK) {
		return fatfs_sd_sync(mountpt->mountflags, fat);
	}

	return fatfs_2_vfs(result);
#else
	return -ENOSYS;
#endif
}

int fatfs_rename(struct inode *mountpt, const char *oldpath, const char *newpath)
{
#if FF_FS_MINIMIZE == 0 && !FF_FS_READONLY
	FATFS *fat = NULL;
	FRESULT result;
	char *oldfspath = NULL;
	const char *newfspath = NULL;
	INT vol;
	size_t oldlen, newlen, len;
	INT ret;

	fatfs_stackcheck(STACK_WARNING_HIGH_LEVEL);

	if (strlen(oldpath) > MAX_LFNAME_LENTH - VOLUME_CHAR_LENGTH)
		return -ENAMETOOLONG;

	if (mountpt->mountflags == MS_RDONLY)
		return -EACCES;

	oldlen = strlen(oldpath);
	newlen = strlen(newpath);
	if (!oldlen || !newlen)
		return -EPERM;

	len = oldlen < newlen ? oldlen : newlen;
	if (!strncmp(oldpath, newpath, len) && (oldpath[len] == '/' || newpath[len] == '/'))
		return -EPERM;

	fat = (FATFS *)mountpt->i_private;
	vol = fatfs_get_vol(fat);
	if (vol < 0 || vol > FF_VOLUMES)
		return -ENOENT;

	oldfspath = (char *)fat_memalloc(MAX_LFNAME_LENTH);
	if (oldfspath == NULL)
		return -ENOMEM;

	newfspath = newpath;
	ret = snprintf_s(oldfspath, MAX_LFNAME_LENTH, MAX_LFNAME_LENTH - 1, "%d:%s", vol, oldpath);
	if (ret < 0) {
		fat_memfree(oldfspath);
		return -EINVAL;
	}

	result = f_rename(oldfspath, newfspath);

	fat_memfree(oldfspath);

	if (result == FR_OK) {
		return fatfs_sd_sync(mountpt->mountflags, fat);
	}

	return fatfs_2_vfs(result);
#else
	return -ENOSYS;
#endif
}

int fatfs_stat(struct inode *mountpt, const char *path, struct stat *buf)
{
#if FF_FS_MINIMIZE == 0
	FILINFO finfo;
	FRESULT result;
	FATFS *fat = NULL;
	INT vol, ret;
	char *f_path = NULL;

	fatfs_stackcheck(STACK_WARNING_HIGH_LEVEL);

	if (mountpt == NULL || mountpt->i_private == NULL)
		return -EINVAL;

	if (strlen(path) > MAX_LFNAME_LENTH - VOLUME_CHAR_LENGTH)
		return -ENAMETOOLONG;

	fat = (FATFS *)mountpt->i_private;

	if (fatfs_dentry_stat(fat, path, buf) == ENOERR) {
		return ENOERR;
	}

	vol = fatfs_get_vol(fat);
	if (vol < 0 || vol > FF_VOLUMES)
		return -ENOENT;

	f_path = (char *)fat_memalloc(MAX_LFNAME_LENTH);
	if (f_path == NULL)
		return -ENOMEM;

	ret = snprintf_s(f_path, MAX_LFNAME_LENTH, MAX_LFNAME_LENTH - 1, "%d:%s", vol, path);
	if (ret < 0) {
		fat_memfree(f_path);
		return -EINVAL;
	}

	result = f_stat(f_path, &finfo);

	(void)memset_s((void *)buf, sizeof(struct stat), 0, sizeof(struct stat));

	if (result == FR_OK) {
		buf->st_dev = 0;

		buf->st_mode = S_IFREG | S_IRUSR | S_IRGRP | S_IROTH |
                       S_IWUSR | S_IWGRP | S_IWOTH;

		if (finfo.fattrib & AM_RDO)
			buf->st_mode &= ~(S_IWUSR | S_IWGRP | S_IWOTH);

		if (finfo.fattrib & AM_DIR) {
			buf->st_mode &= ~S_IFREG;
			buf->st_mode |= S_IFDIR | S_IXUSR | S_IXGRP | S_IXOTH;
		}

		buf->st_size    = (long long)finfo.fsize;
#if FF_MAX_SS != FF_MIN_SS
		buf->st_blksize = (long)((unsigned long)fat->ssize * fat->csize);
#else
		buf->st_blksize = (unsigned long)FF_MIN_SS * fat->csize;
#endif
        buf->st_blocks = (long long)(buf->st_size + buf->st_blksize - 1) / buf->st_blksize;
		buf->st_mtime = Fat2Time(((DWORD)finfo.fdate << 16 | finfo.ftime));
	}

	if (result == FR_INVALID_NAME) { /* root directory */
		buf->st_dev   = 0;
		buf->st_mode  = S_IFREG | S_IRUSR | S_IRGRP | S_IROTH |
                        S_IWUSR | S_IWGRP | S_IWOTH ;

		buf->st_mode &= ~S_IFREG;
		buf->st_mode |= S_IFDIR | S_IXUSR | S_IXGRP | S_IXOTH;
		buf->st_mtime = 0;
		buf->st_size  = 0;
#if FF_MAX_SS != FF_MIN_SS
		buf->st_blksize = (long)((unsigned long)fat->ssize * fat->csize);
#else
		buf->st_blksize = (unsigned long)FF_MIN_SS * fat->csize;
#endif
        buf->st_blocks = (long long)(buf->st_size + buf->st_blksize - 1) / buf->st_blksize;
		result = FR_OK;
	}

	fat_memfree(f_path);

	return fatfs_2_vfs(result);
#else
	return -ENOSYS;
#endif
}

#define VOL_NAME_LEN 4
int fatfs_statfs(struct inode *mountpt, struct statfs *buf)
{
#if FF_FS_MINIMIZE == 0 && !FF_FS_READONLY
	char drive[VOL_NAME_LEN];
	DWORD fre_clust;
	FATFS *fat = NULL;
	FRESULT result;
	INT vol, ret;

	fatfs_stackcheck(STACK_WARNING_LOW_LEVEL);

	FAT_CHECK(mountpt);
	FAT_CHECK(buf);

	fat = (FATFS *)mountpt->i_private;
	FAT_CHECK(fat);
	vol = fatfs_get_vol(fat);
	if (vol < 0 || vol > FF_VOLUMES)
		return -ENOENT;

	ret = snprintf_s(drive, sizeof(drive), sizeof(drive) - 1, "%d:", vol);
	if (ret < 0)
		return -EINVAL;

	if (fat->fsi_check == TRUE) {
		result = f_setfree(drive, &fre_clust, &fat);
		if (result != FR_OK)
			return fatfs_2_vfs(result);
		fat->fsi_check = FALSE;
	}

	result = f_getfree(drive, &fre_clust, &fat);
	if (result != FR_OK)
		return fatfs_2_vfs(result);

	(void)memset_s((void *)buf, sizeof(struct statfs), 0, sizeof(struct statfs));
	buf->f_type   = MSDOS_SUPER_MAGIC;
	buf->f_bfree  = fre_clust;
	buf->f_bavail = fre_clust;
	buf->f_blocks = fat->n_fatent - 2; /* Cluster #0 and #1 is for VBR, reserve sectors and fat */
#if FF_MAX_SS != FF_MIN_SS
	buf->f_bsize  = fat->ssize * fat->csize;
#else
	buf->f_bsize  = FF_MIN_SS * fat->csize;
#endif

#if FF_USE_LFN
	/* Maximum length of filenames */
	buf->f_namelen = FF_MAX_LFN;
#else
	/* Maximum length of filenames: 8 is the basename length, 1 is the dot, 3 is the extension length */
	buf->f_namelen = (8 + 1 + 3);
#endif

	return 0;
#else
	return -ENOSYS;
#endif
}

/* fdate: bit[15:9] Year since 1980(0..127), bit[8:5] Month(0..11), bit[4:0] Day(1..31) */
/* ftime: bit[15:11] Hour(0..23), bit[10:5] Minute(0..59), bit[4:0] Second/2(0..29) */
#define FAT_YEAR_BASE 80
#define FAT_YEAR_SHIFT 9
#define FAT_MONTH_SHIFT 5
#define FAT_HOUR_SHIFT 11
#define FAT_MINUTE_SHIFT 5
int fatfs_utime(struct inode *mountpt, const char *pathname, const struct tm * set_tm)
{
#if FF_FS_MINIMIZE == 0 && !FF_FS_READONLY
	FILINFO finfo;
	FRESULT result;
	FATFS *fat = NULL;
	INT vol, status;
	char *f_path = NULL;

	fatfs_stackcheck(STACK_WARNING_HIGH_LEVEL);

	if (mountpt == NULL || mountpt->i_private == NULL)
		return -EINVAL;

	if (strlen(pathname) > MAX_LFNAME_LENTH - VOLUME_CHAR_LENGTH)
		return -ENAMETOOLONG;

	if (set_tm->tm_year < 80) { /* year must be lager than 1980 */
		PRINT_ERR("modetime must be lager than 1980-1-1\n");
		return -EINVAL;
	}

	if (mountpt->mountflags == MS_RDONLY)
		return -EACCES;

	fat = (FATFS *)mountpt->i_private;
	vol = fatfs_get_vol(fat);
	if (vol < 0 || vol > FF_VOLUMES)
		return -ENOENT;

	f_path = (char *)fat_memalloc(MAX_LFNAME_LENTH);
	if (f_path == NULL)
		return -ENOMEM;

	status = snprintf_s(f_path, MAX_LFNAME_LENTH, MAX_LFNAME_LENTH - 1, "%d:%s", vol, pathname);
	if (status < 0) {
		fat_memfree(f_path);
		return -EINVAL;
	}

	/*
	 * transform set_tm to be the format what we want,
	 * and write in fatfs through f_utime to set modified
	 * time.
	 */

	/* fdate: bit[15:9] Year since 1980(0..127), bit[8:5] Month(0..11), bit[4:0] Day(1..31) */
    finfo.fdate = (WORD)(
        ((unsigned int)(set_tm->tm_year - FAT_YEAR_BASE) << FAT_YEAR_SHIFT) |
        ((unsigned int)(set_tm->tm_mon + 1) << FAT_MONTH_SHIFT) |
        ((unsigned int)set_tm->tm_mday)
    );
	/* ftime: bit[15:11] Hour(0..23), bit[10:5] Minute(0..59), bit[4:0] Second/2(0..29) */
    finfo.ftime = (WORD)(
        ((unsigned int)set_tm->tm_hour << FAT_HOUR_SHIFT)  |
        ((unsigned int)set_tm->tm_min << FAT_MINUTE_SHIFT) |
        ((unsigned int)set_tm->tm_sec >> 1)
    );
	result = f_utime(f_path, &finfo);
	status = fatfs_2_vfs(result);
	fat_memfree(f_path);
	return status;
#else
	return -ENOSYS;
#endif
}

int fatfs_opendir(struct inode *mountpt, const char *relpath, struct fs_dirent_s **dir)
{
#if FF_FS_MINIMIZE <= 1
	char *f_path = NULL;
	DIR *f_dir = NULL;
	FATFS *fat = NULL;
	FRESULT result;
	INT vol, ret;
	struct fs_dirent_s *dirp = NULL;

	fatfs_stackcheck(STACK_WARNING_HIGH_LEVEL);

	if (strlen(relpath) > MAX_LFNAME_LENTH - VOLUME_CHAR_LENGTH)
		return -ENAMETOOLONG;

	FAT_CHECK(dir);
	fat = (FATFS *)mountpt->i_private;
	FAT_CHECK(fat);
	vol = fatfs_get_vol(fat);
	if (vol < 0 || vol > FF_VOLUMES)
		return -ENOENT;

	f_path = (char *)fat_memalloc(MAX_LFNAME_LENTH);
	if (f_path == NULL)
		return -ENOMEM;

	ret = snprintf_s(f_path, MAX_LFNAME_LENTH, MAX_LFNAME_LENTH - 1, "%d:%s", vol, relpath);
	if (ret < 0) {
		fat_memfree(f_path);
		return -EINVAL;
	}

	/* open directory */
	f_dir = (DIR *)fat_memalloc(sizeof(DIR));
	if (f_dir == NULL) {
		fat_memfree(f_path);
		return -ENOMEM;
	}

	dirp = fat_memalloc(sizeof(struct fs_dirent_s));
	if (dirp == NULL) {
		fat_memfree(f_path);
		fat_memfree(f_dir);
		return -ENOMEM;
	}

	result = f_opendir(f_dir, f_path);
	fat_memfree(f_path);

	if (result != FR_OK) {
		fat_memfree(dirp);
		fat_memfree(f_dir);
		return fatfs_2_vfs(result);
	}

	dirp->fd_data = f_dir;
	*dir = dirp;
	return FR_OK;
#else
	return -ENOSYS;
#endif
}

int fatfs_closedir(struct inode *mountpt, struct fs_dirent_s *dir)
{
#if FF_FS_MINIMIZE <= 1
	DIR *f_dir = NULL;
	FRESULT result;

	fatfs_stackcheck(STACK_WARNING_LOW_LEVEL);

	FAT_CHECK(dir);
	f_dir = (DIR *)(dir->fd_data);
	FAT_CHECK(f_dir);

	result = f_closedir(f_dir);
	if (result == FR_OK) {
		dir->fd_data = NULL;
		fat_memfree(f_dir);
		fat_memfree(dir);
		return FR_OK;
	}

	return fatfs_2_vfs(result);
#else
	return -ENOSYS;
#endif
}

int fatfs_readdir(struct inode *mountpt, struct fs_dirent_s *dir, struct dirent *d)
{
#if FF_FS_MINIMIZE <= 1
	DIR *f_dir = NULL;
	FILINFO fno;
	FRESULT result;

	fatfs_stackcheck(STACK_WARNING_LOW_LEVEL);

	FAT_CHECK(dir);
	f_dir = (DIR *)(dir->fd_data);

	(void)memset_s(&fno, sizeof(FILINFO), 0, sizeof(FILINFO));
	result = f_readdir(f_dir, &fno);
	if (result != FR_OK)
		return fatfs_2_vfs(result);

	/*
	 * 0x00:Reached end of directory.
	 * 0xFF:The directory is empty.
	 */
	if (fno.fname[0] == 0 || fno.fname[0] == (TCHAR)0xFF)
		return -ENOENT;

	if (fno.fattrib & AM_DIR)
		d->d_type = DT_DIR;
	else
		d->d_type = DT_REG;

	d->d_reclen = (WORD)sizeof(struct dirent);

	if (strncpy_s(d->d_name, sizeof(d->d_name), fno.fname, strlen(fno.fname)) != EOK)
		return -ENAMETOOLONG;

	return ENOERR;
#else
	return -ENOSYS;
#endif
}

int fatfs_rewinddir(struct inode *mountpt, struct fs_dirent_s *dir)
{
#if FF_FS_MINIMIZE <= 1
	DIR *f_dir = NULL;
	FRESULT result;

	fatfs_stackcheck(STACK_WARNING_HIGH_LEVEL);

	FAT_CHECK(dir);
	f_dir = (DIR *)(dir->fd_data);
	FAT_CHECK(f_dir);

	result = f_rewinddir(f_dir);
	if (result != FR_OK) {
		return fatfs_2_vfs(result);
	}

	return FR_OK;
#else
	return -ENOSYS;
#endif
}

int fatfs_mkdir(struct inode *mountpt, const char *relpath, mode_t mode)
{
#if FF_FS_MINIMIZE == 0 && !FF_FS_READONLY
	char *f_path = NULL;
	FATFS *fat = NULL;
	FRESULT result;
	INT vol, ret;

	fatfs_stackcheck(STACK_WARNING_HIGH_LEVEL);

	if (mountpt->mountflags == MS_RDONLY)
		return -EACCES;

	if (strlen(relpath) > MAX_LFNAME_LENTH - VOLUME_CHAR_LENGTH)
		return -ENAMETOOLONG;

	fat = (FATFS *)mountpt->i_private;
	FAT_CHECK(fat);
	vol = fatfs_get_vol(fat);
	if (vol < 0 || vol > FF_VOLUMES)
		return -ENOENT;

	f_path = (char *)fat_memalloc(MAX_LFNAME_LENTH);
	if (f_path == NULL)
		return -ENOMEM;

	ret = snprintf_s(f_path, MAX_LFNAME_LENTH, MAX_LFNAME_LENTH - 1, "%d:%s", vol, relpath);
	if (ret < 0) {
		fat_memfree(f_path);
		return -EINVAL;
	}

	result = f_mkdir(f_path);
	fat_memfree(f_path);

	if (result == FR_OK)
		return fatfs_sd_sync(mountpt->mountflags, fat);

	return fatfs_2_vfs(result);
#else
	return -ENOSYS;
#endif
}

int fatfs_rmdir(struct inode *mountpt, const char *relpath)
{
#if FF_FS_MINIMIZE == 0 && !FF_FS_READONLY
	char *f_path = NULL;
	FATFS *fat = NULL;
	FRESULT result;
	INT ret, vol;
	struct stat st;

	fatfs_stackcheck(STACK_WARNING_HIGH_LEVEL);

	if (strlen(relpath) > MAX_LFNAME_LENTH - VOLUME_CHAR_LENGTH)
		return -ENAMETOOLONG;

	if (mountpt->mountflags == MS_RDONLY)
		return -EACCES;

	ret = fatfs_stat(mountpt, relpath, &st);
	if (ret != FR_OK)
		return ret;

	switch (st.st_mode & S_IFMT) {
	case S_IFDIR:
		break;
	case S_IFREG:
		return -ENOTDIR;
	default:
		/* unknown file type */
		return FAT_ERROR;
	}

	fat = (FATFS *)mountpt->i_private;
	FAT_CHECK(fat);
	vol = fatfs_get_vol(fat);
	if (vol < 0 || vol > FF_VOLUMES)
		return -ENOENT;

	f_path = (char *)fat_memalloc(MAX_LFNAME_LENTH);
	if (f_path == NULL)
		return -ENOMEM;

	ret = snprintf_s(f_path, MAX_LFNAME_LENTH, MAX_LFNAME_LENTH - 1, "%d:%s", vol, relpath);
	if (ret < 0) {
		fat_memfree(f_path);
		return -EINVAL;
	}

	result = f_unlink(f_path);
	fat_memfree(f_path);

	if (result == FR_OK) {
		return fatfs_sd_sync(mountpt->mountflags, fat);
	}

	return fatfs_2_vfs(result);
#else
	return -ENOSYS;
#endif
}

static INT fatfs_erase(INT option, const los_part *part)
{
	INT opt = option;

	if ((UINT)opt & FMT_ERASE) {
		opt = (UINT)opt & (~FMT_ERASE);
		if (EraseDiskByID(part->disk_id, part->sector_start, part->sector_count) != 0)
			PRINTK("SD card erase error.\n");
	}

	if (opt != FM_FAT && opt != FM_FAT32)
		opt = FM_ANY;

	return opt;
}

static INT fatfs_mount_empty(const los_part *part, char *drive, INT len, FATFS *fat)
{
	INT result, index;

	index = fatfs_get_vol((FATFS *)NULL);
	if (index == FAT_ERROR) {
		return -EEXIST; /* can't find empty fatfs */
	}

	VolToPart[index].di = part->disk_id;
	VolToPart[index].pd = part->part_id;
	VolToPart[index].pt = part->part_no_mbr;
	VolToPart[index].ps = part->sector_start;
	VolToPart[index].pc = part->sector_count;

	result = snprintf_s(drive, len, len - 1, "%d:/", index);
	if (result < 0) {
		return -EINVAL;
	}

	result = f_mount(fat, drive, 0);
	if (result != FR_OK) {
		return fatfs_2_vfs(result);
	}

	return index;
}

#define DMA_ALLGN  64
static INT fatfs_set_partinfo(INT index, los_part *part)
{
	los_disk *disk = NULL;
	char *mbr_buf = NULL;
	INT result;

	/* if there is no mbr before, the partition info needs to be changed after formatting. */
	if ((part->type != EMMC) && (part->part_no_mbr == 0)) {
		disk = get_disk(part->disk_id);
		if (disk == NULL) {
			return -EIO;
		}
		mbr_buf = (char *)fat_memalloc_align(DMA_ALLGN, disk->sector_size);
		if (mbr_buf == NULL) {
			return -ENOMEM;
		}
		(void)memset_s(mbr_buf, disk->sector_size, 0, disk->sector_size);
		result = los_disk_read(part->disk_id, mbr_buf, 0, 1);
		if (result < 0) {
			fat_memfree(mbr_buf);
			return -EIO;
		}
		part->sector_start = LD_DWORD_DISK(&mbr_buf[PAR_OFFSET + PAR_START_OFFSET]);
		part->sector_count = LD_DWORD_DISK(&mbr_buf[PAR_OFFSET + PAR_COUNT_OFFSET]);
		part->part_no_mbr = 1;
		part->filesystem_type = mbr_buf[PAR_OFFSET + PAR_TYPE_OFFSET];

		VolToPart[index].pt = part->part_no_mbr;
		VolToPart[index].ps = part->sector_start;
		VolToPart[index].pc = part->sector_count;
		fat_memfree(mbr_buf);
	}

	return ENOERR;
}

#ifdef LOSCFG_FS_FAT_VIRTUAL_PARTITION
static INT fatfs_virpart_mkfs(UINT flag, const struct inode *mmc_node, int option, int index)
{
	struct inode_search_s desc;
	INT vir_ret;
	if (flag != 0)
		return VIRERR_NOTMOUNT;

	SETUP_SEARCH(&desc, FatVirPart.virtualinfo.devpartpath, false);
	vir_ret = inode_find(&desc);
	RELEASE_SEARCH(&desc);
	if (vir_ret < 0 || desc.node != mmc_node) {
		return VIRERR_NOPARAM;
	}

	(void)fatfs_unbind_vir_part(f_getfatfs(index));

	if (option != FM_FAT32)
		return VIRERR_NOTFIT;

	vir_ret = fatfs_make_vir_part(f_getfatfs(index), index);
	if (vir_ret < VIRERR_BASE)
		return FR_OK;
	return vir_ret;
}
#endif

#define INDEX_LABEL_LEN 15
static INT fatfs_set_label(int index)
{
	INT result = FR_OK;

#if FF_USE_LABEL
	char label[INDEX_LABEL_LEN] = {0};
	result = snprintf_s(label, sizeof(label), sizeof(label) - 1, "%d:%s", index, FatLabel);
	if (result < 0) {
		PRINTK("setlable snprintf_s failed !\n ");
		return -EINVAL;
	}

	result = f_setlabel(label);
	if (result != FR_OK)
		PRINTK("Set sd card volume label failed !\n ");

#endif

	return result;
}

#ifdef LOSCFG_FS_FAT_FTL
int fatfs_ftl_open(int devId)
{
	return DiskFtlOpen(devId);
}

int fatfs_ftl_close(int devId)
{
	return DiskFtlClose(devId);
}
#else
int fatfs_ftl_open(int devId)
{
	(void)devId;
	return ENOERR;
}

int fatfs_ftl_close(int devId)
{
	(void)devId;
	return ENOERR;
}
#endif
/*
 * return  : 0 : OK; -ENODEV : "/dev/mmc0" driver not ready
 * sectors : 0~128 0:auto seclect; 1:512Byte; 2:1KB; 4 : 2KB; 128:64KB; if secotrs > 128,auto seclect;
 */
int fatfs_mkfs(const char *dev, int sectors, int option)
{
#if FF_USE_MKFS && !FF_FS_READONLY
	INT result, index, err;
	FATFS fat = {0};
	UINT flag = 0;
	struct inode_search_s desc;
#ifdef LOSCFG_FS_FAT_VIRTUAL_PARTITION
	INT vir_ret = 0;
#endif
	los_part *part = NULL;
	char drive[DRIVER_NAME_LEN] = {0};
	char *work = NULL;
	struct inode *mmc_node = NULL;
	MKFS_PARM opt;
	int ret;
	size_t ss;

	fat.win = NULL;

	fatfs_stackcheck(STACK_WARNING_LOW_LEVEL);

	/* if device not register in system,it may cause serious error,so return -ENODEV */
	SETUP_SEARCH(&desc, dev, false);
	result = inode_find(&desc);
	if (result < 0) {
		RELEASE_SEARCH(&desc);
		return -ENODEV; /* "mmc0" driver not ready */
	}

	mmc_node = desc.node;
	part = los_part_find(mmc_node);
	if (part == NULL || part->dev == NULL) {
		inode_release(mmc_node);
		RELEASE_SEARCH(&desc);
		return -ENODEV;
	}

	ret = fatfs_ftl_open((int)part->disk_id);
	if (ret != ENOERR) {
		inode_release(mmc_node);
		RELEASE_SEARCH(&desc);
		return ret;
	}

	ret = los_disk_ioctl((int)part->disk_id, DISK_GET_SECTOR_SIZE, &ss);
	if (ret != ENOERR) {
		(void)fatfs_ftl_close((int)part->disk_id);
		inode_release(mmc_node);
		RELEASE_SEARCH(&desc);
		return ret;
	}
	inode_lock();
	index = fatfs_get_disk(mmc_node);
	/* if not mount, find an empty fatfs to mount */
	if (index == FAT_ERROR) {
		index = fatfs_mount_empty(part, drive, sizeof(drive), &fat);
		if (index < 0) {
			err = index;
			goto ERROUT;
		}
		DiskDev[index] = mmc_node;
		flag |= 1<<1;

	} else {
		if (fatfs_checkopenlock(index) != FR_OK) { /* format is not allowed when a file or directory is opened. */
			fatfs_checkopenunlock(index);
			(void)fatfs_ftl_close((int)part->disk_id);
			inode_release(mmc_node);
			RELEASE_SEARCH(&desc);
			inode_unlock();
			return -EBUSY;
		}
		result = snprintf_s(drive, sizeof(drive), sizeof(drive) - 1, "%d:/", index);
		if (result < 0) {
			fatfs_checkopenunlock(index);
			(void)fatfs_ftl_close((int)part->disk_id);
			inode_release(mmc_node);
			RELEASE_SEARCH(&desc);
			inode_unlock();
			return -EINVAL;
		}

		flag = 0;
		result = fatfs_dentry_reinit(part->part_name);
		if (result != ENOERR) {
			fatfs_checkopenunlock(index);
			(void)fatfs_ftl_close((int)part->disk_id);
			inode_release(mmc_node);
			RELEASE_SEARCH(&desc);
			inode_unlock();
			return result;
		}
	}

	option = fatfs_erase(option, part);
	work = (char *)fat_memalloc(FF_MAX_SS);
	if (work == NULL) {
		err = -ENOMEM;
		goto ERROUT_WITH_MOUNT;
	}
	if (part->type == EMMC)
		option = (UINT)option | FM_SFD;

	opt.fmt = option;
	opt.n_fat = 1;
	opt.au_size = (DWORD)sectors * ss;
	opt.n_root = 0;
	opt.align = 0;
#if FF_MULTI_PARTITION_EXTEND
	opt.extend_data = ff_create_extended_parm();
	if (opt.extend_data == NULL) {
		fat_memfree(work);
		err = -ENOMEM;
		goto ERROUT_WITH_MOUNT;
	}
#endif

	result = f_mkfs(drive, &opt, work, FF_MAX_SS);
	fat_memfree(work);

#if FF_MULTI_PARTITION_EXTEND
	ff_destroy_extended_parm(opt.extend_data);
#endif
	if (result != FR_OK) {
		err = fatfs_2_vfs(result);
		goto ERROUT_WITH_MOUNT;
	}

#ifdef LOSCFG_FS_FAT_VIRTUAL_PARTITION
	los_disk *disk = get_disk(part->disk_id);
	if (disk->sector_count > UINT_MAX) {
		vir_ret = VIRERR_NOTFIT;
	} else{
		vir_ret = fatfs_virpart_mkfs(flag, mmc_node, option, index);
	}
#endif

	result = fatfs_set_label(index);

#ifdef LOSCFG_FS_FAT_CACHE
	if (result == FR_OK) {
		if (OsSdSync(part->disk_id) != ENOERR) {
			err = -EIO;
			goto ERROUT_WITH_MOUNT;
		}
	}
#endif

	err = fatfs_set_partinfo(index, part);

#ifdef LOSCFG_FS_FAT_UNI_TRIM
	if (part->trimManager != NULL) {
		UniTrimDeinit(part->trimManager);
		part->trimManager = UniTrimInit(part->dev, sectors, part->sector_count);
	}
#endif

ERROUT_WITH_MOUNT:
	if (flag & (1 << 1)) {
		(void)f_mount(NULL, drive, 0); /* unmount the tmp fatfs */
		DiskDev[index] = (struct inode *)NULL;
	} else if (flag == 0) {
		fatfs_checkopenunlock(index);
	}

ERROUT:
	(VOID)fatfs_ftl_close((int)part->disk_id);
	inode_release(mmc_node);
	RELEASE_SEARCH(&desc);
	inode_unlock();
	if (fat.win != NULL)
		fat_memfree(fat.win);
#ifdef LOSCFG_FS_FAT_VIRTUAL_PARTITION
	if (err == 0)
		err = vir_ret;
#endif
	return err;
#else
	return -ENOSYS;
#endif
}

int fatfs_chattr(struct inode *mountpt, const char *relpath, mode_t mode)
{
#if FF_FS_MINIMIZE == 0 && !FF_FS_READONLY
	FRESULT result;
	FATFS *fat = NULL;
	INT vol, ret;
	char *f_path = NULL;
	unsigned char attr;

	fatfs_stackcheck(STACK_WARNING_HIGH_LEVEL);

	if (mountpt == NULL || mountpt->i_private == NULL)
		return -EINVAL;

	if (strlen(relpath) > MAX_LFNAME_LENTH - VOLUME_CHAR_LENGTH)
		return -ENAMETOOLONG;

	if (mountpt->mountflags == MS_RDONLY)
		return -EACCES;

	attr = mode & (AM_RDO | AM_HID | AM_SYS | AM_ARC);

	fat = (FATFS *)mountpt->i_private;
	vol = fatfs_get_vol(fat);
	if (vol < 0 || vol > FF_VOLUMES)
		return -ENOENT;

	f_path = (char *)fat_memalloc(MAX_LFNAME_LENTH);
	if (f_path == NULL)
		return -ENOMEM;

	ret = snprintf_s(f_path, MAX_LFNAME_LENTH, MAX_LFNAME_LENTH - 1, "%d:%s", vol, relpath);
	if (ret < 0) {
		fat_memfree(f_path);
		return -EINVAL;
	}

	result = f_chmod(f_path, attr, 0xff);

	fat_memfree(f_path);

	if (result == FR_OK)
		return fatfs_sd_sync(mountpt->mountflags, fat);

	return fatfs_2_vfs(result);

#else
	return -ENOSYS;
#endif
}

int fatfs_getlabel(void *handle, char *label)
{
#if FF_USE_LABEL
	INT index;
	FATFS *fat = NULL;
	FRESULT result;
	los_part *part = NULL;
	char drive[DRIVER_NAME_LEN];
	DWORD *vsn = NULL;
	INT ret;

	fatfs_stackcheck(STACK_WARNING_LOW_LEVEL);

	if (label == NULL)
		return -EFAULT;

	fat = (FATFS *)handle;
	if (fat == NULL)
		return -EINVAL;

	index = fatfs_get_vol(fat);
	if (index >= FF_VOLUMES || index < 0)
		return -EINVAL;

	part = los_part_find(DiskDev[index]);
	if (part == NULL)
		return -ENODEV;

	ret = snprintf_s(drive, sizeof(drive), sizeof(drive) - 1, "%d:/", index);
	if (ret < 0)
		return -EINVAL;

	result = f_getlabel(drive, label, vsn);
	if (result != FR_OK)
		return fatfs_2_vfs(result);

	return OK;
#else
	return -ENOSYS;
#endif
}

int fatfs_fallocate(FAR struct file *filep, int mode, off_t offset, off_t len)
{
#if FF_USE_EXPAND && !FF_FS_READONLY
	FIL *fp = (FIL *)(filep->f_priv);
	FRESULT res;

	fatfs_stackcheck(STACK_WARNING_LOW_LEVEL);

	FAT_CHECK(fp);

	if (len <= 0 || offset < 0)
		return -EINVAL;

	if (mode != FALLOC_FL_KEEP_SIZE)
		return -EINVAL;

	if (filep->f_inode->mountflags == MS_RDONLY)
		return -EACCES;

	res = f_expand_offset(fp, (FSIZE_t)offset, (FSIZE_t)len, FALLOC_FL_KEEP_SIZE);

	if (res == FR_OK)
		res = f_sync(fp);

	return fatfs_2_vfs(res);
#else
	return -ENOSYS;
#endif
}

int fatfs_fallocate64(FAR struct file *filep, int mode, off64_t offset, off64_t len)
{
#if FF_USE_EXPAND && !FF_FS_READONLY
	FIL *fp = (FIL *)(filep->f_priv);
	FRESULT  res;

	fatfs_stackcheck(STACK_WARNING_LOW_LEVEL);

	FAT_CHECK(fp);

	if (len >= FAT32_MAXSZIE || len <= 0 || offset >= FAT32_MAXSZIE || offset < 0)
		return -EINVAL;

	if (mode != FALLOC_FL_KEEP_SIZE)
		return -EINVAL;

	if (filep->f_inode->mountflags == MS_RDONLY)
		return -EACCES;

	res = f_expand_offset(fp, (FSIZE_t)offset, (FSIZE_t)len, FALLOC_FL_KEEP_SIZE);

	if (res == FR_OK)
		res = f_sync(fp);

	return fatfs_2_vfs(res);
#else
	return -ENOSYS;
#endif
}

int los_set_systime_status(BOOL b_status)
{
	if (b_status != FAT_SYSTEM_TIME_ENABLE && b_status != FAT_SYSTEM_TIME_DISABLE)
		return -EINVAL;

	fatfs_settimestatus(b_status);
	return FR_OK;
}

int fatfs_truncate(FAR struct file *filep, off_t length)
{
#if FF_FS_MINIMIZE == 0 && !FF_FS_READONLY
	FIL *fp = (FIL *)(filep->f_priv);
	FRESULT res = FR_OK;
	UINT count;
	DWORD n, fclust;
#ifdef LOSCFG_FS_FAT_CACHE
	FATFS *fat = NULL;
#endif

	fatfs_stackcheck(STACK_WARNING_LOW_LEVEL);

	if (filep->f_inode->mountflags == MS_RDONLY)
		return -EACCES;

	FAT_CHECK(fp);

	if (length < 0)
		return -EINVAL;

	res = fatfs_getclustinfo(fp, &fclust, &count);
	if (res != FR_OK)
		return fatfs_2_vfs(res);

	if (count == 0xFFFFFFFF)
		return -EPERM;

	n = (DWORD)(fp->obj.fs)->csize * SS(fp->obj.fs); /* Cluster size */

	if ((unsigned long)length > count * n) {
#if FF_USE_EXPAND
		res = f_expand_offset(fp, 0, (FSIZE_t)(length), FALLOC_FL_KEEP_SIZE);
#else
		return -ENOSYS;
#endif
	} else if ((unsigned long)length < n * count) {
		res = f_truncate_length(fp, (FSIZE_t)length);
	}
    fp->obj.objsize = (FSIZE_t)length; /* Set file size to length */
	fp->flag |= 0x40; /* Set modified flag */

	if (res == FR_OK)
		res = f_sync(fp);
#ifdef LOSCFG_FS_FAT_CACHE
	if (res == FR_OK) {
		fat = (FATFS *)filep->f_inode->i_private;
		FAT_CHECK(fat);
		return fatfs_sd_sync(filep->f_inode->mountflags, fat);
	}
#endif
	return fatfs_2_vfs(res);
#else
	return -ENOSYS;
#endif
}

int fatfs_truncate64(FAR struct file *filep, off64_t length)
{
#if FF_FS_MINIMIZE == 0 && !FF_FS_READONLY
	FIL *fp = (FIL *)(filep->f_priv);
	FRESULT res = FR_OK;
	UINT count;
	DWORD n, fclust;
#ifdef LOSCFG_FS_FAT_CACHE
	FATFS *fat = NULL;
#endif

	fatfs_stackcheck(STACK_WARNING_LOW_LEVEL);

	FAT_CHECK(fp);

	if (filep->f_inode->mountflags == MS_RDONLY)
		return -EACCES;

	if (length >= FAT32_MAXSZIE || length < 0)
		return -EINVAL;

	res = fatfs_getclustinfo(fp, &fclust, &count);
	if (res != FR_OK)
		return fatfs_2_vfs(res);

	if (count == 0xFFFFFFFF)
		return -EPERM;

	n = (DWORD)(fp->obj.fs)->csize * SS(fp->obj.fs); /* Cluster size */

    if (length > (off64_t)((unsigned long long)count * (unsigned long long)n)) {
#if FF_USE_EXPAND
		res = f_expand_offset(fp, 0, (FSIZE_t)(length), FALLOC_FL_KEEP_SIZE);
#else
		return -ENOSYS;
#endif
    } else if (length < (off64_t)((unsigned long long)count * (unsigned long long)n)) {
		res = f_truncate_length(fp, (FSIZE_t)length);
	}

    fp->obj.objsize = (FSIZE_t)length; /* Set file size to length */
	fp->flag |= 0x40; /* Set modified flag */

	if (res == FR_OK)
		res = f_sync(fp);
#ifdef LOSCFG_FS_FAT_CACHE
	if (res == FR_OK) {
		fat = (FATFS *)filep->f_inode->i_private;
		FAT_CHECK(fat);
		return fatfs_sd_sync(filep->f_inode->mountflags, fat);
	}
#endif
	return fatfs_2_vfs(res);
#else
	return -ENOSYS;
#endif
}

#define DATE_OFFSET 16
#define CHECK_FILE_NUM 3
static INT fatfs_timemin(const DIR_FILE dir_file[])
{
	INT min = 0;
	DWORD tm_min = 0;

	MIN((dir_file[0].fno.fdate << DATE_OFFSET | dir_file[0].fno.ftime),
		(dir_file[1].fno.fdate << DATE_OFFSET | dir_file[1].fno.ftime), tm_min);
	MIN((DWORD)(dir_file[2].fno.fdate << DATE_OFFSET | dir_file[2].fno.ftime), tm_min, tm_min);
	if (tm_min == (DWORD)(dir_file[0].fno.fdate << DATE_OFFSET | dir_file[0].fno.ftime)) {
		min = 0;
	} else if (tm_min == (DWORD)(dir_file[1].fno.fdate << DATE_OFFSET | dir_file[1].fno.ftime)) {
		min = 1;
	} else if (tm_min == (DWORD)(dir_file[2].fno.fdate << DATE_OFFSET | dir_file[2].fno.ftime)) {
		min = 2; /* check the latest three files */
	}

	return min;
}

int fatfs_fscheck(struct inode *mountpt, const char *relpath, struct fs_dirent_s **dir)
{
	DIR *f_dir = NULL;
	FILINFO fno;
	FRESULT result;
	INT count = 0;
	INT time_oldest = 0;
	DIR dir_backup = {0};
	DIR_FILE dir_file[CHECK_FILE_NUM] = {0};
	INT loop = CHECK_FILE_NUM;
#ifdef LOSCFG_FS_FAT_CACHE
	FATFS    *fat = NULL;
	los_part   *part = NULL;
	INT vol;
#endif
	INT ret;

	fatfs_stackcheck(STACK_WARNING_HIGH_LEVEL);

	if (mountpt->mountflags == MS_RDONLY)
		return -EACCES;

	FAT_CHECK(dir);

	ret = fatfs_opendir(mountpt, relpath, dir);
	if (ret != FR_OK)
		return ret;

	f_dir = (DIR *)((*dir)->fd_data);
	if ((f_dir->obj.fs)->fs_type != FS_FAT32) {
		(void)fatfs_closedir(mountpt, *dir);
		return -EINVAL;
	}

	do {
		(void)memcpy_s(&dir_backup, sizeof(DIR), f_dir, sizeof(DIR));
		result = f_readdir(f_dir, &fno);
		if (result != FR_OK)
			break;

		if ((fno.fname[0] == 0 || fno.fname[0] == (TCHAR)0xFF))
			break;

		if (fno.fattrib == AM_DIR)
			continue;

		if (count == 0) {
			(void)memcpy_s(&dir_file[0].f_dir, sizeof(DIR), &dir_backup, sizeof(DIR));
			(void)memcpy_s(&dir_file[0].fno, sizeof(FILINFO), &fno, sizeof(FILINFO));
		} else if (count == 1) {
			(void)memcpy_s(&dir_file[1].f_dir, sizeof(DIR), &dir_backup, sizeof(DIR));
			(void)memcpy_s(&dir_file[1].fno, sizeof(FILINFO), &fno, sizeof(FILINFO));
		} else if (count == 2) { /* check the latest three files */
			(void)memcpy_s(&dir_file[2].f_dir, sizeof(DIR), &dir_backup, sizeof(DIR));
			(void)memcpy_s(&dir_file[2].fno, sizeof(FILINFO), &fno, sizeof(FILINFO));
			time_oldest = fatfs_timemin(dir_file);
		} else {
			if ((fno.fdate << DATE_OFFSET | fno.ftime) >
				(dir_file[time_oldest].fno.fdate << DATE_OFFSET | dir_file[time_oldest].fno.ftime)) {
				(void)memcpy_s(&dir_file[time_oldest].f_dir, sizeof(DIR), &dir_backup, sizeof(DIR));
				(void)memcpy_s(&dir_file[time_oldest].fno, sizeof(FILINFO), &fno, sizeof(FILINFO));
				time_oldest = fatfs_timemin(dir_file);
			}
		}
		count++;
	} while (result == FR_OK);
	if (count < loop)
		loop = count;

	ret = fatfs_closedir(mountpt, *dir);
	if (ret != FR_OK)
		return ret;

	for (count = 0; count < loop; count++) {
		result = fatfs_fcheckfat(&dir_file[count]);
		if (result != FR_OK)
			return fatfs_2_vfs(result);
	}
#ifdef LOSCFG_FS_FAT_CACHE
	fat = (FATFS *)mountpt->i_private;
	vol = fatfs_get_vol(fat);
	if (vol < 0 || vol > FF_VOLUMES)
		return -ENOENT;
	part = get_part((INT)fat->pdrv);
	if (part != NULL) {
		if (OsSdSync(part->disk_id) != ENOERR) {
			return -EIO;
		}
	}
#endif

	return ENOERR;
}

int fatfs_virstatfs(struct inode *mountpt, const char *relpath, struct statfs *buf)
{
#ifdef LOSCFG_FS_FAT_VIRTUAL_PARTITION
	return fatfs_virstatfs_internel(mountpt, relpath, buf);
#else
	return 0;
#endif
}

const struct mountpt_operations fat_operations = {
	fatfs_open,		/* open */
	fatfs_close,		/* close */
	fatfs_read,		/* read */
	fatfs_write,		/* write */
	fatfs_seek,		/* seek */
	fatfs_ioctl,		/* ioctl */
	NULL,			/* mmap */
	fatfs_truncate,	/* truncate */
	fatfs_sync,		/* sync */
	fatfs_dup,		/* dup */
	NULL,			/* fstat */
	NULL,			/* fchstat */

	fatfs_opendir,		/* opendir */
	fatfs_closedir,		/* closedir */
	fatfs_readdir,		/* readdir */
	fatfs_rewinddir,	/* rewinddir */

	fatfs_bind,		/* bind */
	fatfs_unbind,		/* unbind */
	fatfs_statfs,		/* statfs */
	fatfs_virstatfs,	/* virstatfs */
	fatfs_unlink,		/* unlinke */
	fatfs_mkdir,		/* mkdir */
	fatfs_rmdir,		/* rmdir */
	fatfs_rename,		/* rename */
	fatfs_stat,		/* stat */
	NULL,			/* chstat */
	fatfs_utime,		/* utime */
	fatfs_chattr,		/* chattr */
	fatfs_seek64,		/* seek64 */
	fatfs_getlabel,		/* getlabel */
	fatfs_fallocate,	/* fallocate */
	fatfs_fallocate64,	/* fallocate64 */
	fatfs_truncate64,	/* truncate64 */
	fatfs_fscheck		/* fscheck */
};

FSMAP_ENTRY(fat_fsmap, "vfat", fat_operations, FALSE, TRUE);

#endif	/* LOSCFG_FS_FAT */
