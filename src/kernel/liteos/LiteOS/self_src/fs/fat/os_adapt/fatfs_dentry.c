/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Fatfs Dentry Implementation
 * Author: Huawei LiteOS Team
 * Create: 2021-8-20
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

#include "inode/inode.h"
#include "ff.h"
#include "fatfs_dentry.h"

#ifdef LOSCFG_FS_FAT_DCACHE

VOID fatfs_dentry_init(FATFS *fat)
{
    LosDentry *dentry = NULL;

    dentry = DentryInit("/");
    if (dentry != NULL) {
        dentry->mode = S_IFDIR;
    }
    fat->root_dentry = dentry;
}

VOID fatfs_dentry_deinit(const FATFS *fat)
{
    DentryDeinit(fat->root_dentry);
}

INT32 fatfs_dentry_reinit(const CHAR *devName)
{
    struct inode_search_s desc;
    FATFS *fs = NULL;
    INT result;

    SETUP_SEARCH(&desc, devName, false);
    result = inode_find(&desc);
    RELEASE_SEARCH(&desc);
    if (result < 0) {
        return -EINVAL;
    }

    fs = (FATFS *)desc.node->i_private;
    fatfs_dentry_deinit(fs);
    fatfs_dentry_init(fs);
    inode_release(desc.node);
    return ENOERR;
}

VOID fatfs_stat_fill(const LosDentry *dentry, struct stat *buf)
{
    buf->st_dev = 0;
    buf->st_size = dentry->size;
    buf->st_mode = dentry->mode;
    buf->st_blksize = dentry->blkSize;
    buf->st_blocks = (dentry->size + dentry->blkSize - 1) / dentry->blkSize;;
    buf->st_mtime = dentry->mtime.tv_sec;
}

INT32 fatfs_dentry_stat(const FATFS *fat, const CHAR *path, struct stat *buf)
{
    LosDentry *dentry = DentryFind((LosDentry *)fat->root_dentry, path);
    if (dentry != NULL) {
        fatfs_stat_fill(dentry, buf);
        return ENOERR;
    }
    return -ENOENT;
}

STATIC INT32 get_filename(const WCHAR *lfnbuf, CHAR *name, INT32 nameLen)
{
    int i;

    for (i = 0; i < nameLen; i++) {
        name[i] = (CHAR)lfnbuf[i];
        if (name[i] == 0) {
            return ENOERR;
        }
    }

    return -EPERM;
}

LosDentry* f_dentry_new(LosDentry *parent, const WCHAR* lfnbuf)
{
    LosDentry *dentry;
    char name[FF_MAX_LFN + 1];

    if (parent == NULL) {
        return NULL;
    }

    /* lfnbuf type is unsigned short, can't use strcpy */
    if (get_filename(lfnbuf, name, sizeof(name)) != ENOERR) {
        return NULL;
    }
    dentry = DentryLookup(parent, name);
    if (dentry != NULL) {
        return dentry;
    }
    return DentryCreate(parent, name);
}

VOID f_dentry_destroy(LosDentry *dentry)
{
    DentryDestroy(dentry);
}

VOID f_dentry_mode_update(LosDentry *dentry, BYTE fattrib)
{
    if (dentry == NULL) {
        return;
    }
    dentry->mode = S_IFREG | S_IRUSR | S_IRGRP | S_IROTH |
                   S_IWUSR | S_IWGRP | S_IWOTH;
    if (fattrib & AM_RDO)
        dentry->mode &= ~(S_IWUSR | S_IWGRP | S_IWOTH);

    if (fattrib & AM_DIR) {
        dentry->mode &= ~S_IFREG;
        dentry->mode |= S_IFDIR | S_IXUSR | S_IXGRP | S_IXOTH;
    }
}

VOID f_dentry_mtime_update(LosDentry *dentry, const struct timespec *ts)
{
    if (dentry != NULL) {
        DentryTimeUpdate(dentry, ts, MTIME);
    }
}

VOID f_dentry_ctime_update(LosDentry *dentry, const struct timespec *ts)
{
    if (dentry != NULL) {
        DentryTimeUpdate(dentry, ts, CTIME);
    }
}

VOID f_dentry_size_update(LosDentry *dentry, FSIZE_t size)
{
    if (dentry != NULL) {
        DentrySizeUpdate(dentry, size);
    }
}

VOID f_dentry_blocksize_update(LosDentry *dentry, FSIZE_t blksize)
{
    if (dentry != NULL) {
        DentryBlockSizeUpdate(dentry, blksize);
    }
}

BOOL f_dentry_support(VOID)
{
    return TRUE;
}

#else /* !LOSCFG_FS_FAT_DCACHE */

VOID fatfs_dentry_init(FATFS *fat) { }
VOID fatfs_dentry_deinit(const FATFS *fat) { }
INT32 fatfs_dentry_reinit(const CHAR *devName)
{
    return ENOERR;
}

INT32 fatfs_dentry_stat(const FATFS *fat, const char *path, struct stat *buf)
{
    /* DENTRY not support means can't find path in dcache */
    return -EOPNOTSUPP;
}

LosDentry* f_dentry_new(LosDentry *parent, const WCHAR* lfnbuf)
{
    return NULL;
}

VOID f_dentry_destroy(LosDentry *dentry) { }
VOID f_dentry_mode_update(LosDentry *dentry, BYTE fattrib) { }
VOID f_dentry_mtime_update(LosDentry *dentry, const struct timespec *ts) { }
VOID f_dentry_ctime_update(LosDentry *dentry, const struct timespec *ts) { }
VOID f_dentry_size_update(LosDentry *dentry, FSIZE_t size) { }
VOID f_dentry_blocksize_update(LosDentry *dentry, FSIZE_t blksize) { }

BOOL f_dentry_support(VOID)
{
    return FALSE;
}

#endif /* LOSCFG_FS_FAT_DCACHE */

