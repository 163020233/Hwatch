/* -----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2019. All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/
 * or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 * ---------------------------------------------------------------------------- */

#include "virpart.h"
#include "errno.h"
#include "fatfs.h"
#include "errcode_fat.h"
#include "disk.h"
#ifdef LOSCFG_FS_FAT_CACHE
#include "bcache.h"
#endif

#ifdef LOSCFG_FS_FAT_VIRTUAL_PARTITION

#include "virpartff.h"

char DevPartName[DISK_NAME + 1] = {0};

static INT fatfs_checkpercent(const virpartinfo *virtualinfo)
{
	double virtpercent = 0.0;
	INT partcount;

	for (partcount = 0; partcount < virtualinfo->virpartnum; partcount++) {
		if (virtualinfo->virpartpercent[partcount] <= _FLOAT_ACC ||
			virtualinfo->virpartpercent[partcount] >= (1 - _FLOAT_ACC)) {
			set_errno(VIRERR_PARMPERCENTERR);
			FatVirPart.isparamset = FALSE;
			return -1;
		}
		virtpercent += virtualinfo->virpartpercent[partcount];
	}

	if (virtpercent >= (1 + _FLOAT_ACC)) {
		set_errno(VIRERR_PARMPERCENTERR);
		FatVirPart.isparamset = FALSE;
		return -1;
	}

	return 0;
}

static INT fatfs_checkname(const virpartinfo *virtualinfo)
{
	INT partcount, loopcount;
	size_t len;
	FRESULT res;

	for (partcount = 0; partcount < virtualinfo->virpartnum; partcount++) {
		len = strnlen(virtualinfo->virpartname[partcount], _MAX_ENTRYLENGTH + 1);
		if (len == 0 || len > _MAX_ENTRYLENGTH) {
			set_errno(VIRERR_PARMNAMEERR);
			FatVirPart.isparamset = FALSE;
			return -1;
		}

		res = f_checkname(virtualinfo->virpartname[partcount]);
		if (res != FR_OK) {
			set_errno(VIRERR_PARMNAMEERR);
			FatVirPart.isparamset = FALSE;
			return -1;
		}
	}

	for (partcount = 0; partcount < virtualinfo->virpartnum; partcount++) {
		for (loopcount = partcount + 1; loopcount < virtualinfo->virpartnum; loopcount++) {
			if ((memcmp(virtualinfo->virpartname[partcount], virtualinfo->virpartname[loopcount],
						_MAX_ENTRYLENGTH)) == 0) {
				set_errno(VIRERR_PARMNAMEERR);
				FatVirPart.isparamset = FALSE;
				return -1;
			}
		}
	}

	return 0;
}

INT los_set_virpartparam (virpartinfo virtualinfo)
{
	INT ret;
	UINT min_size;
	UINT dst_name_size;
	char devhead[DISK_NAME + 1] = "/dev/";

	FatVirPart.virtualinfo.devpartpath = DevPartName;

	if (FatVirPart.isparamset == TRUE) {
		set_errno(VIRERR_PARMLOCKED);
		return -1;
	}

	if (virtualinfo.devpartpath == NULL) {
		set_errno(VIRERR_NOPARAM);
		return -1;
	}

	FatVirPart.isparamset = TRUE;

	if (virtualinfo.virpartnum < 1 || virtualinfo.virpartnum > _MAX_VIRVOLUMES) {
		set_errno(VIRERR_PARMNUMERR);
		FatVirPart.isparamset = FALSE;
		return -1;
	}

	ret = fatfs_checkpercent(&virtualinfo);
	if (ret != 0) {
		return ret;
	}

	ret = fatfs_checkname(&virtualinfo);
	if (ret != 0) {
		return ret;
	}

	if (strlen(virtualinfo.devpartpath) <= strlen(devhead)) {
		set_errno(VIRERR_PARMDEVERR);
		FatVirPart.isparamset = FALSE;
		return -1;
	}

	if (memcmp(virtualinfo.devpartpath, devhead, strlen(devhead)) != 0) {
		set_errno(VIRERR_PARMDEVERR);
		FatVirPart.isparamset = FALSE;
		return -1;
	}

	dst_name_size = sizeof(DevPartName);
	min_size = dst_name_size < strlen(virtualinfo.devpartpath) ? dst_name_size : strlen(virtualinfo.devpartpath);
	ret = strncpy_s(FatVirPart.virtualinfo.devpartpath, dst_name_size, virtualinfo.devpartpath, min_size);
	if (ret != EOK) {
		set_errno(VIRERR_PARMNAMEERR);
		FatVirPart.isparamset = FALSE;
		return -1;
	}
	FatVirPart.virtualinfo.devpartpath[dst_name_size - 1] = '\0';

	(void)memcpy_s(FatVirPart.virtualinfo.virpartname, sizeof(FatVirPart.virtualinfo.virpartname),
				   virtualinfo.virpartname, sizeof(virtualinfo.virpartname));
	(void)memcpy_s(FatVirPart.virtualinfo.virpartpercent, sizeof(FatVirPart.virtualinfo.virpartpercent),
				   virtualinfo.virpartpercent, sizeof(virtualinfo.virpartpercent));
	FatVirPart.virtualinfo.virpartnum = virtualinfo.virpartnum;

	return 0;
}

static INT fatfs_disablepart (void *handle)
{
	FATFS *fs = (FATFS *)handle;
	return fatfs_2_vfs(f_disvirfs(fs));
}

/*
 * fatfs_scanfat:
 * Scan the FAT inside the boundary of CHILD FATFS limit, and update the free cluster and last cluster
 * for all CHILD FATFS.
 * Acceptable Return Value:
 * - FR_OK              : Successfully scanned the FAT and update field.
 * Others Return Value:
 * - FR_INVAILD_FATFS   : The FATFS object has error or the info in it has been occuried
 * - FR_DENIED          : The virtual partition feature has been shut down by switcher
 * - FR_DISK_ERR        : A disk error happened
 */
static INT fatfs_scanfat (void *handle)
{
	FATFS *fat = (FATFS *)handle;
	UINT i;
	INT ret;

	for (i = 0; i < fat->vir_amount; i++) {
		/* Assert error will not abort the scanning process */
		ret = f_scanfat(CHILDFS(fat, i));
		if (ret != FR_OK) {
			break;
		}
	}

	return fatfs_2_vfs(ret);
}

/*
 * fatfs_scanclear:
 * Scan the root directory clean completely, regardless of the virtual partition entry.
 * Acceptable Return Value:
 * - FR_OK          : The root directory is completely clean.
 * - FR_NOTCLEAR    : The root dircotory is not clean.
 * Others Return Value:
 * Followed the by the lower API.
 */
static FRESULT fatfs_scanclear (INT vol)
{
	FRESULT ret;
	DIR dir;
	FILINFO fno;
	CHAR path[MAX_LFNAME_LENTH];
	INT num;
	INT res;

	/* num : for the amount of all item in root directory */
	num = 0;

	(void)memset_s(path, sizeof(path), 0, sizeof(path));

	res = snprintf_s(path, MAX_LFNAME_LENTH, MAX_LFNAME_LENTH - 1, "%d:/", vol);
	if (res < 0) {
		return FR_INVALID_NAME;
	}

	ret = f_opendir(&dir, path);
	if (ret != FR_OK) {
		return ret;
	}

	/* Scan all entry for searching virtual partition directory and others in root directory */
	while (1) {
		(void)memset_s(&fno, sizeof(FILINFO), 0, sizeof(FILINFO));
		ret = f_readdir(&dir, &fno);
		/* Reach the end of directory, or the root directory is empty, abort the scanning operation */
		if (fno.fname[0] == 0x00 || fno.fname[0] == (TCHAR)0xFF) {
			break;
		}

		if (ret != FR_OK) {
			(void)f_closedir(&dir);
			return ret;
		}
		num++;
	}

	/* Close the directory */
	ret = f_closedir(&dir);

	if ((ret == FR_OK) && (num != 0)) {
		return FR_NOTCLEAR;
	}

	return ret;
}

/*
 * fatfs_buildentry:
 * Scan virtual partition entry in root directory, and try to rebuild the
 * error virtual partition, according to the scanning result.
 * Acceptable Return Value:
 * - FR_OK          : The root directory is completely clean.
 * - FR_OCCUPIED    : The virtual partition entry has been occupied by the same name file.
 * - FR_CHAIN_ERR   : The virtual partition entry has been rebuilt along the invalid cluster
 *                    chain.
 * Others Return Value:
 * Followed the by the lower API
 */
static FRESULT fatfs_buildentry (const FATFS *fat, INT vol)
{
	UINT i;
	CHAR path[MAX_LFNAME_LENTH];
	FRESULT ret = FR_OK;
	DIR dir;
	INT res;

	for (i = 0; i < fat->vir_amount; i++) {
		res = snprintf_s(path, MAX_LFNAME_LENTH, MAX_LFNAME_LENTH - 1, "%d:%s", vol, CHILDFS(fat, i)->namelabel);
		if (res < 0) {
			return FR_INVALID_NAME;
		}

		ret = f_mkdir(path);
		if (ret == FR_EXIST) {
			(void)memset_s(&dir, sizeof(dir), 0, sizeof(dir));
			ret = f_opendir(&dir, path);
			if (ret == FR_NO_DIR) {
				return FR_OCCUPIED;
			}
			if (ret == FR_OK) {
				ret = f_boundary(CHILDFS(fat, i), dir.obj.sclust);
				if (ret != FR_OK) {
					(void)f_closedir(&dir);
					return ret;
				}
			} else {
				return ret;
			}
			ret = f_closedir(&dir);
			if (ret != FR_OK) {
				return ret;
			}
		} else if (ret != FR_OK) {
			return ret;
		}
	}

	return FR_OK;
}

/*
 * fatfs_unbind_vir_part:
 * Uninitialized the CHILD FATFS object
 *
 * Acceptable Return Value:
 * - ENOERR : Successfully initialized the CHILD FATFS object.
 */
INT fatfs_unbind_vir_part (void *handle)
{
	INT ret;
	FATFS *fat = (FATFS *)handle;
	ret = f_unregvirfs(fat);
	return fatfs_2_vfs(ret);
}

/*
 * fatfs_bind_vir_part:
 * Bind the virtual partition
 *
 * Acceptable Return Value:
 * - FR_OK : Finished the virtual partition.
 * The virtual partition result can refer by API
 */
INT fatfs_bind_vir_part (void *handle, INT vol)
{
	INT ret;
	FATFS *fat = (FATFS *)handle;
	char path[MAX_LFNAME_LENTH] = {0};
#ifdef LOSCFG_FS_FAT_CACHE
	los_part *part = NULL;
#endif

	if (fat == NULL) {
		return -EINVAL;
	}

	ret = snprintf_s(path, sizeof(path), sizeof(path) - 1, "%d:/", vol);
	if (ret < 0) {
		return -EINVAL;
	}

	/* Detect the information in the reserve sector and try to set virtual partition by sector info */
	ret = f_checkvirpart(fat, path, vol);
	if (ret == FR_NOVIRPART) {
		/* Failed to use the External SD setting, try to use the current setting to external SD and rebuild entries */
		/* Check the environment whether it is fit for virtual partition */
		ret = fatfs_scanclear(vol);
		if (ret != FR_OK) {
			/* A error happened in the file operation */
			(void)fatfs_disablepart(fat);
			return fatfs_2_vfs(ret);
		}
		/* Ready to fit the SD card to virtual partition featrue */
		ret = f_makevirpart(fat, path, vol);
		if (ret != FR_OK) {
			(void)fatfs_disablepart(fat);
			return fatfs_2_vfs(ret);
		}
	} else if (ret != FR_OK) {
		return fatfs_2_vfs(ret);
	}
	/* Try to build the virtual entry */
	ret = fatfs_buildentry(fat, vol);
	if (ret != FR_OK) {
		(void)fatfs_disablepart(fat);
		return fatfs_2_vfs(ret);
	}
#ifdef LOSCFG_FS_FAT_CACHE
	part = get_part((int)fat->pdrv);
	if (part != NULL) {
		ret = OsSdSync(part->disk_id);
		if (ret != ENOERR) {
			ret = -EIO;
		}
	} else {
		return -ENODEV;
	}
#endif
	if (ret == FR_OK) {
		ret = fatfs_scanfat(fat);
		if (ret != FR_OK) {
			(void)fatfs_disablepart(fat);
			return fatfs_2_vfs(ret);
		}
	}
	return fatfs_2_vfs(ret);
}

/*
 * fatfs_make_vir_part:
 * Bind the virtual partition
 *
 * Acceptable Return Value:
 * - FR_OK : Finished the virtual partition.
 * The virtual partition result can refer by API
 */
INT fatfs_make_vir_part (void *handle, INT vol)
{
	INT ret;
	FATFS *fat = (FATFS *)handle;
	char path[MAX_LFNAME_LENTH] = {0};

	if (fat == NULL) {
		return -EINVAL;
	}

	ret = snprintf_s(path, sizeof(path), sizeof(path) - 1, "%d:/", vol);
	if (ret < 0) {
		return -EINVAL;
	}

	/* Ready to fit the SD card to virtual partition featrue */
	ret = f_makevirpart(fat, path, vol);
	if (ret != FR_OK) {
		(void)fatfs_disablepart(fat);
		return fatfs_2_vfs(ret);
	}

	/* Try to build the virtual entry */
	ret = fatfs_buildentry(fat, vol);
	if (ret != FR_OK) {
		(void)fatfs_disablepart(fat);
		return fatfs_2_vfs(ret);
	}

	return fatfs_2_vfs(ret);
}

INT fatfs_virstatfs_internel (const struct inode *mountpt, const char *relpath, struct statfs *buf)
{
	char drive[MAX_LFNAME_LENTH];
	DWORD fre_clust, all_clust;
	FATFS *fat = NULL;
	INT result, vol;

	fat = (FATFS *)mountpt->i_private;
	FAT_CHECK(fat);

	if (fat->vir_flag != FS_PARENT) {
		return -EINVAL;
	}

	vol = fatfs_get_vol(fat);
	if (vol < 0 || vol > FF_VOLUMES) {
		return -ENOENT;
	}

	if (strlen(relpath) > MAX_LFNAME_LENTH) {
		return -EFAULT;
	}

	result = snprintf_s(drive, sizeof(drive), sizeof(drive) - 1, "%d:%s", vol, relpath);
	if (result < 0) {
		return -EINVAL;
	}

	result = f_getvirfree(drive, &fre_clust, &all_clust);
	if (result != FR_OK) {
		result = fatfs_2_vfs(result);
		return result;
	}

	(void)memset_s((void *)buf, sizeof(struct statfs), 0, sizeof(struct statfs));
	buf->f_type = MSDOS_SUPER_MAGIC;
	buf->f_bfree = fre_clust;
	buf->f_bavail = fre_clust;
	buf->f_blocks = all_clust;
#if FF_MAX_SS != FF_MIN_SS
	buf->f_bsize = fat->ssize * fat->csize;
#else
	buf->f_bsize = FF_MIN_SS * fat->csize;
#endif
#if FF_USE_LFN
	buf->f_namelen = FF_MAX_LFN; /* Maximum length of filenames */
#else
	/*
	 * Maximum length of filenames,'8' is the effective length, '1' is the terminator,
	 * and '3' is the special length
	 */
	buf->f_namelen = (8 + 1 + 3);
#endif

	return result;
}

#endif
