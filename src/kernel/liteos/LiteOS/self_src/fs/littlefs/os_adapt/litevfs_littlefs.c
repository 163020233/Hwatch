/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Litevfs littlefs adpat layer
 * Author: Huawei RTOS_Lite Team
 * Create: 2025-04-15
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
#include "stdint.h"
#include "malloc.h"
#include "stdio.h"
#include "securec.h"
#include "stdlib.h"
#include "unistd.h"
#include "fcntl.h"
#include "dirent.h"
#include "sys/mount.h"
#include "sys/stat.h"
#include "sys/errno.h"
#include "lfs.h"
#include "lfs_util.h"
#include "litevfs_vfs.h"
#include "vfs_littlefs.h"

typedef lfs_file_t lfs_file_t;
typedef lfs_dir_t lfs_dir_t;

typedef struct {
    struct lfs_config cfg;
    lfs_t *lfs;
    const void *data;
    uint32_t startAddr;
} VfsLfs;

static int RetToErrno(int result)
{
    int error = 0;
    switch (result) {
        case LFS_ERR_OK:
            return 0;
        case LFS_ERR_IO:
            error = EIO;
            break;
        case LFS_ERR_NOENT:
            error = ENOENT;
            break;
        case LFS_ERR_EXIST:
            error = EEXIST;
            break;
        case LFS_ERR_NOTDIR:
            error = ENOTDIR;
            break;
        case LFS_ERR_ISDIR:
            error = EISDIR;
            break;
        case LFS_ERR_NOTEMPTY:
            error = ENOTEMPTY;
            break;
        case LFS_ERR_BADF:
            error = EBADF;
            break;
        case LFS_ERR_INVAL:
            error = EINVAL;
            break;
        case LFS_ERR_NOSPC:
            error = ENOSPC;
            break;
        case LFS_ERR_NOMEM:
            error = ENOMEM;
            break;
        case LFS_ERR_CORRUPT:
            error = EFAULT;
            break;
        default:
            error = EIO;
            break;
    }

    return -error;
}


static int LittlefsFlagsGet(int oflags)
{
    int flags = 0;
    switch (oflags & O_ACCMODE) {
        case O_RDONLY:
            flags |= LFS_O_RDONLY;
            break;
        case O_WRONLY:
            flags |= LFS_O_WRONLY;
            break;
        case O_RDWR:
            flags |= LFS_O_RDWR;
            break;
        default:
            break;
    }

    if (oflags & O_CREAT) {
        flags |= LFS_O_CREAT;
    }

    if ((oflags & O_EXCL)) {
        flags |= LFS_O_EXCL;
    }

    if (oflags & O_TRUNC) {
        flags |= LFS_O_TRUNC;
    }

    if (oflags & O_APPEND) {
        flags |= LFS_O_APPEND;
    }

    return flags;
}

static int LittlefsFileOpen(struct File *file, const char *pathInMp, int flags)
{
    int ret = 0;
    VfsLfs *vfsLfs = (VfsLfs *)file->fileMp->mountData;
    lfs_t *lfs = vfsLfs->lfs;
    lfs_file_t *filep = (lfs_file_t *)malloc(sizeof(lfs_file_t));
    if (filep == NULL) {
        return -ENOMEM;
    }
    (void)memset_s(filep, sizeof(lfs_file_t), 0, sizeof(lfs_file_t));
    ret = lfs_file_open(lfs, filep, pathInMp, LittlefsFlagsGet(flags));
    if (ret == LFS_ERR_OK) {
        file->fileData = (void *)filep;
    } else {
        free(filep);
    }

    return RetToErrno(ret);
}

static int LittlefsFileClose(struct File *file)
{
    VfsLfs *vfsLfs = (VfsLfs *)file->fileMp->mountData;
    lfs_t *lfs = vfsLfs->lfs;
    lfs_file_t *filep = (lfs_file_t *)file->fileData;
    int ret = lfs_file_close(lfs, filep);
    if (ret == LFS_ERR_OK) {
        free(filep);
    }
    return RetToErrno(ret);
}

static ssize_t LittlefsFileRead(struct File *file, char *buf, size_t bytes)
{
    VfsLfs *vfsLfs = (VfsLfs *)file->fileMp->mountData;
    lfs_t *lfs = vfsLfs->lfs;
    lfs_file_t *filep = (lfs_file_t *)file->fileData;
    lfs_ssize_t ret = lfs_file_read(lfs, filep, buf, (lfs_size_t)bytes);
    if (ret < 0) {
        return RetToErrno((int)ret);
    }
    file->fileOffset = filep->pos;
    return (ssize_t)ret;
}

static ssize_t LittlefsFileWrite(struct File *file, const char *buf, size_t bytes)
{
    VfsLfs *vfsLfs = (VfsLfs *)file->fileMp->mountData;
    lfs_t *lfs = vfsLfs->lfs;
    lfs_file_t *filep = (lfs_file_t *)file->fileData;
    lfs_ssize_t ret;
    if ((buf == NULL) || (lfs == NULL) || (filep == NULL)) {
        return -EINVAL;
    }
    ret = lfs_file_write(lfs, filep, buf, (lfs_size_t)bytes);
    if (ret < 0) {
        return RetToErrno((int)ret);
    }

    file->fileOffset = filep->pos;
    return (ssize_t)ret;
}

off_t LittlefsFileLseek(struct File *file, off_t off, int whence)
{
    VfsLfs *vfsLfs = (VfsLfs *)file->fileMp->mountData;
    lfs_t *lfs = vfsLfs->lfs;
    lfs_file_t *filep = (lfs_file_t *)file->fileData;
    lfs_soff_t offset;
    if (filep == NULL) {
        return -EINVAL;
    }
    offset = lfs_file_seek(lfs, filep, (lfs_off_t)off, whence);
    if (offset < 0) {
        return RetToErrno((int)offset);
    }
    file->fileOffset = offset;
    return (off_t)offset;
}

int LittlefsFileFstat(struct MountPoint *mountp, const char *pathInMp, struct stat *stat)
{
    VfsLfs *vfsLfs = (VfsLfs *)mountp->mountData;
    lfs_t *lfs = vfsLfs->lfs;
    struct lfs_info info;
    int ret;
    if (pathInMp[0] == '/') {
        pathInMp++;
    }
    (void)memset_s(stat, sizeof(struct stat), 0, sizeof(struct stat));

    ret = lfs_stat(lfs, pathInMp, &info);
    if (ret) {
        return RetToErrno(ret);
    }
    switch (info.type) {
        case LFS_TYPE_REG:
            stat->st_mode = S_IFREG;
            stat->st_size = (off_t)info.size;
            break;

        case LFS_TYPE_DIR:
            stat->st_mode = S_IFDIR;
            stat->st_size = 0;
            break;

        default:
            stat->st_mode = 0;
            break;
    }
    return LFS_ERR_OK;
}

static int LittlefsOpenDir(struct dir *dir, const char *pathInMp)
{
    int ret;
    VfsLfs *vfsLfs = NULL;
    lfs_dir_t *lfsDir = NULL;
    lfs_t *lfs = NULL;

    if (dir == NULL) {
        return -ENOMEM;
    }

    lfsDir = (lfs_dir_t *)malloc(sizeof(lfs_dir_t));
    if (lfsDir == NULL) {
        return -ENOMEM;
    }
    (void)memset_s(lfsDir, sizeof(lfs_dir_t), 0, sizeof(lfs_dir_t));
    struct MountPoint *dirMp = (struct MountPoint *)dir->dirMp;
    vfsLfs = (VfsLfs *)dirMp->mountData;
    lfs = vfsLfs->lfs;

    ret = lfs_dir_open(lfs, lfsDir, pathInMp);
    if (ret != LFS_ERR_OK) {
        free(lfsDir);
        return RetToErrno(ret);
    }

    dir->dirData = (void *)lfsDir;
    dir->dirOffset = 0;

    return RetToErrno(ret);
}

static int LittlefsReadDir(struct dir *dir, struct dirent *dirent)
{
    int ret;
    struct MountPoint *dirMp = (struct MountPoint *)dir->dirMp;
    VfsLfs *vfsLfs = (VfsLfs *)dirMp->mountData;
    lfs_t *lfs = vfsLfs->lfs;
    lfs_dir_t *lfsDir = (lfs_dir_t *)dir->dirData;
    struct lfs_info info = {0};
    if (lfsDir == NULL) {
        return -EINVAL;
    }

    do {
        ret = lfs_dir_read(lfs, lfsDir, &info);
    } while ((ret >= 0) && ((strcmp(info.name, ".") == 0) || (strcmp(info.name, "..") == 0)));

    if (ret < 0) {
        return -ENOENT;
    }
    ret = strncpy_s((char *)dirent->dirName, MAX_DIR_NAME_LEN, (const char *)info.name,
        LFS_NAME_MAX);
    if (ret != EOK) {
        return -EINVAL;
    }

    dirent->dirName[MAX_DIR_NAME_LEN - 1] = '\0';
    dirent->size = info.size;
    if (info.type == LFS_TYPE_DIR) {
        dirent->type = VFS_TYPE_DIR;
    } else {
        dirent->type = VFS_TYPE_FILE;
    }
    return LFS_ERR_OK;
}

static int LittlefsCloseDir(struct dir *dir)
{
    int ret;
    struct MountPoint *dirMp = (struct MountPoint *)dir->dirMp;
    VfsLfs *vfsLfs = (VfsLfs *)dirMp->mountData;
    lfs_t *lfs = vfsLfs->lfs;
    lfs_dir_t *lfsDir = (lfs_dir_t *)dir->dirData;
    if (lfsDir == NULL) {
        return -EINVAL;
    }

    ret = lfs_dir_close(lfs, lfsDir);
    if (ret == LFS_ERR_OK) {
        free(lfsDir);
        dir->dirData = NULL;
    }
    return RetToErrno(ret);
}

__attribute__((weak)) int UserBlockDeviceRead(const struct lfs_config *cfg, uint32_t block,
    uint32_t off, void *buffer, uint32_t size)
{
    (void)cfg;
    (void)block;
    (void)off;
    (void)buffer;
    (void)size;
    return 0;
}

__attribute__((weak)) int UserBlockDeviceWrite(const struct lfs_config *cfg, uint32_t block,
    uint32_t off, const void *buffer, lfs_size_t size)
{
    (void)cfg;
    (void)block;
    (void)off;
    (void)buffer;
    (void)size;
    return 0;
}

__attribute__((weak)) int UserBlockDeviceErase(const struct lfs_config *cfg, uint32_t block)
{
    (void)cfg;
    (void)block;
    return 0;
}

__attribute__((weak)) int UserBlockDeviceSync(const struct lfs_config *cfg)
{
    (void)cfg;
    return 0;
}

#ifdef LFS_THREADSAFE
__attribute__((weak)) int UsrMutexTake(const struct lfs_config *cfg)
{
    (void)cfg;
    return 0;
}

__attribute__((weak)) int UsrMutexRelease(const struct lfs_config *cfg)
{
    (void)cfg;
    return 0;
}
#endif

__attribute__((weak)) int UserGetBlockSize(const struct lfs_config *cfg)
{
    (void)cfg;
    return 0;
}

__attribute__((weak)) int UserGetBlockCount(const struct lfs_config *cfg)
{
    (void)cfg;
    return 1;
}

// 配置驱动
void LiteInitLfsCfg(struct lfs_config *lfsCfg, const void *data)
{
    // block设备操作
    lfsCfg->read  = UserBlockDeviceRead;
    lfsCfg->prog  = UserBlockDeviceWrite;
    lfsCfg->erase = UserBlockDeviceErase;
    lfsCfg->sync  = UserBlockDeviceSync;
    
#ifdef LFS_THREADSAFE
    lfsCfg->lock   = UsrMutexTake;
    lfsCfg->unlock = UsrMutexRelease;
#endif
    lfsCfg->context = (void *)(uintptr_t)data;
    lfsCfg->read_size = LFS_MIN_CACHE_SIZE;
    lfsCfg->prog_size = LFS_MIN_CACHE_SIZE;
    lfsCfg->block_size = (lfs_size_t)UserGetBlockSize(lfsCfg);
    lfsCfg->block_count = (lfs_size_t)UserGetBlockCount(lfsCfg);
    lfsCfg->cache_size = LFS_CACHE_SIZE;
    lfsCfg->lookahead_size = LFS_LOOKAHEAD_SIZE;
    lfsCfg->block_cycles = LFS_BLOCK_CYCLES;
    lfsCfg->read_buffer = NULL;
    lfsCfg->prog_buffer = NULL;
    lfsCfg->lookahead_buffer = NULL;
    lfsCfg->name_max = 0;
    lfsCfg->file_max = 0;
    lfsCfg->attr_max = 0;
    lfsCfg->metadata_max = 0;
}

int LittlefsMount(struct MountPoint *mountp, const void *data)
{
    int ret = -1;
    VfsLfs *vfsLfs = (VfsLfs *)malloc(sizeof(VfsLfs));
    if (vfsLfs == NULL) {
        return -ENOMEM;
    }
    (void)memset_s(vfsLfs, sizeof(VfsLfs), 0, sizeof(VfsLfs));
    LiteInitLfsCfg(&vfsLfs->cfg, data);
    vfsLfs->lfs = (lfs_t *)malloc(sizeof(lfs_t));
    if (vfsLfs->lfs == NULL) {
        free(vfsLfs);
        return -ENOMEM;
    }
    (void)memset_s(vfsLfs->lfs, sizeof(lfs_t), 0, sizeof(lfs_t));
    mountp->mountData = vfsLfs;
    ret = lfs_mount(vfsLfs->lfs, &vfsLfs->cfg);
    if (ret == LFS_ERR_CORRUPT) {
        ret = lfs_format(vfsLfs->lfs, &vfsLfs->cfg);
        if (ret != LFS_ERR_OK) {
            goto err_free;
        }
        ret = lfs_mount(vfsLfs->lfs, &vfsLfs->cfg);
    }
    if (ret != LFS_ERR_OK) {
        goto err_free;
    }
    return LFS_ERR_OK;

err_free:
    free(vfsLfs->lfs);
    vfsLfs->lfs = NULL;
    free(vfsLfs);
    return RetToErrno(ret);
}

int LittlefsUmount(struct MountPoint *mountp)
{
    VfsLfs *vfsLfs = (VfsLfs *)mountp->mountData;
    if (vfsLfs == NULL) {
        return -EINVAL;
    }
    lfs_t *lfs = vfsLfs->lfs;
    if (lfs == NULL) {
        return -EINVAL;
    }
    
    int ret = lfs_unmount(lfs);
    if (ret != LFS_ERR_OK) {
        return RetToErrno(ret);
    }

    free(lfs);
    vfsLfs->lfs = NULL;
    free(vfsLfs);
    mountp->mountData = NULL;
    return RetToErrno(ret);
}

static int LittlefsMkdir(struct MountPoint *mountp, const char *path)
{
    VfsLfs *vfsLfs = (VfsLfs *)mountp->mountData;
    lfs_t *lfs = vfsLfs->lfs;
    int ret = lfs_mkdir(lfs, path);
    return RetToErrno(ret);
}

int LittlefsUnlink(struct MountPoint *mountp, const char *pathInMp)
{
    VfsLfs *vfsLfs = (VfsLfs *)mountp->mountData;
    lfs_t *lfs = vfsLfs->lfs;
    int ret = lfs_remove(lfs, pathInMp);
    return RetToErrno(ret);
}

int LittlefsRename(struct MountPoint *mountp, const char *pathInMpOld, const char *pathInMpNew)
{
    VfsLfs *vfsLfs = (VfsLfs *)mountp->mountData;
    lfs_t *lfs = vfsLfs->lfs;
    int ret = lfs_rename(lfs, pathInMpOld, pathInMpNew);
    return RetToErrno(ret);
}

int LittlefsRmdir(struct MountPoint *mountp, const char *pathInMp)
{
    return LittlefsUnlink(mountp, pathInMp);
}

static struct FileOps g_littlefsFileOps = {
    .open       = LittlefsFileOpen,
    .close      = LittlefsFileClose,
    .read       = LittlefsFileRead,
    .write      = LittlefsFileWrite,
    .lseek      = LittlefsFileLseek,
    .opendir    = LittlefsOpenDir,
    .readdir    = LittlefsReadDir,
    .closedir   = LittlefsCloseDir,
    .unlink     = LittlefsUnlink,
    .mkdir      = LittlefsMkdir,
    .rmdir      = LittlefsRmdir,
    .rename     = LittlefsRename,
    .stat       = LittlefsFileFstat,
    .mount      = LittlefsMount,
    .umount     = LittlefsUmount,
};

static struct FileSystem g_littlefsFs = { "littlefs", &g_littlefsFileOps, NULL, 0 };
static bool g_LfsRegistered = false;

void *LittlefsRegister(void)
{
    if (g_LfsRegistered) {
        return (void *)&g_littlefsFs;
    }
    
    if (LitevfsRegisterFs(&g_littlefsFs) != 0) {
        return NULL;
    }
    g_LfsRegistered = true;

    return (void *)&g_littlefsFs;
}