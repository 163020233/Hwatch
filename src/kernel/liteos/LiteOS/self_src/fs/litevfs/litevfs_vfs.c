/* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: litevfs File System Implementation
 * Author: Huawei LiteOS Team
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
#include "securec.h"
#include "stdlib.h"
#include "unistd.h"
#include "fcntl.h"
#include "dirent.h"
#include "sys/mount.h"
#include "sys/stat.h"
#include "kal_base.h"
#include "kal_mutex.h"
#include "litevfs_vfs_internal.h"
#include "litevfs_vfs.h"

struct File g_filePool[LITE_VFS_MAX_FILES];
KalMutex g_fsMutex;
struct MountPoint *g_mountPoints = NULL;
struct FileSystem *g_fileSystems = NULL;

static int LitevfsFileToFd(struct File *file)
{
    if ((file < &g_filePool[0]) || (file > &g_filePool[LITE_VFS_MAX_FILES - 1])) {
        return -1;
    }
    return (((uintptr_t)file) - ((uintptr_t)&g_filePool[0])) / sizeof(struct File);
}

static struct File *LitevfsFdToFile(int fd)
{
    if ((fd < 0) || (fd >= LITE_VFS_MAX_FILES)) {
        return NULL;
    }
    return &g_filePool[fd];
}

static struct File *LitevfsAllocFile(void)
{
    uint32_t i;
    struct File *file = NULL;
    for (i = 0; i < LITE_VFS_MAX_FILES; i++) {
        if (g_filePool[i].fileStatus == FILE_STATUS_NOT_USED) {
            file = &g_filePool[i];
            file->fileStatus = FILE_STATUS_INITING;
            file->fileFlags = 0;
            file->fileOps = NULL;
            file->fileData = NULL;
            file->fileMp = NULL;
            file->fileOffset = 0;
            file->fullPath = NULL;
            file->fileOwner = (uint32_t)-1;
            return file;
        }
    }

    return NULL;
}

static int LitevfsFreeFile(struct File *file)
{
    if (file == NULL) {
        return -1;
    }
    if ((file < &g_filePool[0]) || (file > &g_filePool[LITE_VFS_MAX_FILES - 1])) {
        return -1;
    }
    file->fileStatus = FILE_STATUS_NOT_USED;
    return 0;
}

/* attach to a file and then set new status */
static struct File *LitevfsAttachFile(int fd, uint32_t status)
{
    struct File *file = NULL;
    if ((fd < 0) || (fd >= LITE_VFS_MAX_FILES)) {
        return file;
    }
    file = LitevfsFdToFile(fd);
    if ((file == NULL) || (file->fileMp == NULL)) {
        return NULL;
    }
    if (KAL_MutexAcquire(&g_fsMutex) != KAL_OK) {
        return NULL;
    }
    if (file->fileStatus == FILE_STATUS_READY) {
        if (KAL_MutexAcquire(&file->fileMp->mountMutex) != KAL_OK) {
            (void)KAL_MutexRelease(&g_fsMutex);
            return NULL;
        }

        if (status != FILE_STATUS_READY) {
            file->fileStatus = status;
        }
    } else {
        file = NULL;
    }
    (void)KAL_MutexRelease(&g_fsMutex);
    return file;
}

static struct File *LitevfsAttachReadyFile(int fd)
{
    return LitevfsAttachFile(fd, FILE_STATUS_READY);
}

static struct File *LitevfsAttachFileWithStatus(int fd, int status)
{
    return LitevfsAttachFile(fd, (uint32_t)status);
}

static void LitevfsDetachFile(struct File *file)
{
    if ((file == NULL) || (file->fileMp == NULL)) {
        return;
    }
    (void)KAL_MutexRelease(&file->fileMp->mountMutex);
}

static void SetPathtInMp(const char **pathInMp, const char *path)
{
    if (pathInMp != NULL) {
        *pathInMp = path;
    }
}

static struct MountPoint *LitevfsFindMountp(const char *path, const char **pathInMp)
{
    struct MountPoint *mp = g_mountPoints;
    struct MountPoint *bestMp = NULL;
    int bestMatches = 0;
    if (path == NULL) {
        return NULL;
    }
    SetPathtInMp(pathInMp, NULL);
    while ((mp != NULL) && (mp->mountPath != NULL)) {
        const char *mpath = mp->mountPath;
        const char *ipath = path;
        int matches = 0;
        do {
            while ((*mpath == '/') && (*(mpath + 1) != '/')) {
                mpath++;
            }
            while ((*ipath == '/') && (*(ipath + 1) != '/')) {
                ipath++;
            }

            const char *t = strchr(mpath, '/');
            if (t == NULL) {
                t = strchr(mpath, '\0');
            }
            if ((t == mpath) || (t == NULL)) {
                break;
            }
            if (strncmp(mpath, ipath, (size_t)(t - mpath)) != 0) {
                goto next; /* this mount point do not match, check next */
            }

            ipath += (t - mpath);
            if ((*ipath != '\0') && (*ipath != '/')) {
                goto next;
            }

            matches += (t - mpath);
            mpath += (t - mpath);
        } while (*mpath != '\0');

        if (matches > bestMatches) {
            bestMatches = matches;
            bestMp = mp;

            while ((*ipath == '/') && (*(ipath + 1) != '/')) {
                ipath++;
            }
            SetPathtInMp(pathInMp, ipath);
        }
    next:
        mp = mp->mountNext;
    }
    return bestMp;
}

static int MapToPosixRet(int ret)
{
    return ((ret) < 0 ? -1 : (ret));
}

int open(const char *path, int flags, ...)
{
    struct File *file = NULL;
    int fd = LITE_VFS_FAIL;
    const char *pathInMp = NULL;
    struct MountPoint *mountp = NULL;

    if ((path == NULL) || (path[strlen(path) - 1] == '/')) {
        return fd;
    }
    /* prevent fs/mp being removed while opening */
    if (KAL_MutexAcquire(&g_fsMutex) != KAL_OK) {
        return fd;
    }

    file = LitevfsAllocFile();
    mountp = LitevfsFindMountp(path, &pathInMp);
    if ((file == NULL) || (mountp == NULL) || (pathInMp == NULL) || (*pathInMp == '\0') ||
        (mountp->mountFs == NULL) || (mountp->mountFs->fileOps == NULL) || (mountp->mountFs->fileOps->open == NULL)) {
        (void)KAL_MutexRelease(&g_fsMutex);
        LitevfsFreeFile(file);
        return fd;
    }

    if ((KAL_MutexAcquire(&mountp->mountMutex) != KAL_OK)) {
        (void)KAL_MutexRelease(&g_fsMutex);
        LitevfsFreeFile(file);
        return fd;
    }
    (void)KAL_MutexRelease(&g_fsMutex);

    file->fileFlags = (uint32_t)flags;
    file->fileOffset = 0;
    file->fileData = NULL;
    file->fileOps = mountp->mountFs->fileOps;
    file->fileMp = mountp;
    file->fullPath = path;

    if (file->fileOps->open(file, pathInMp, flags) == 0) {
        mountp->mountRefs++;
        fd = LitevfsFileToFd(file);
        file->fileStatus = FILE_STATUS_READY;   /* file now ready to use */
    } else {
        LitevfsFreeFile(file);
    }
    (void)KAL_MutexRelease(&mountp->mountMutex);
    return MapToPosixRet(fd);
}

int close(int fd)
{
    struct File *file;
    int ret = LITE_VFS_FAIL;
    file = LitevfsAttachFileWithStatus(fd, FILE_STATUS_CLOSING);
    if (file == NULL) {
        return ret;
    }

    if ((file->fileOps != NULL) && (file->fileOps->close != NULL)) {
        ret = file->fileOps->close(file);
    }

    if ((ret == 0) && (file->fileMp != NULL)) {
        file->fileMp->mountRefs--;
    }

    LitevfsDetachFile(file);
    ret = LitevfsFreeFile(file);
    if (ret != 0) {
        return LITE_VFS_FAIL;
    }
    return ret;
}

ssize_t read(int fd, void *buf, size_t bytes)
{
    struct File *file;
    ssize_t ret = LITE_VFS_FAIL;
    if (buf == NULL) {
        return ret;
    }
    if (bytes == 0) {
        return LITE_VFS_SUCCESS;
    }
    file = LitevfsAttachReadyFile(fd);
    if (file == NULL) {
        return ret;
    }
    if ((file->fileFlags & O_ACCMODE) == O_WRONLY) {
        ret = LITE_VFS_FAIL;
    } else if ((file->fileOps != NULL) && (file->fileOps->read != NULL)) {
        ret = file->fileOps->read(file, buf, bytes);
    } else {
        ret = LITE_VFS_FAIL;
    }
    LitevfsDetachFile(file);
    return (ssize_t)MapToPosixRet(ret);
}

ssize_t write(int fd, const void *buf, size_t bytes)
{
    struct File *file;
    ssize_t ret = LITE_VFS_FAIL;
    if (buf == NULL) {
        return ret;
    }
    if (bytes == 0) {
        return LITE_VFS_SUCCESS;
    }
    file = LitevfsAttachReadyFile(fd);
    if (file == NULL) {
        return ret;
    }

    if ((file->fileFlags & O_ACCMODE) == O_RDONLY) {
        ret = LITE_VFS_FAIL;
    } else if ((file->fileOps != NULL) && (file->fileOps->write != NULL)) {
        ret = file->fileOps->write(file, buf, bytes);
    } else {
        ret = LITE_VFS_FAIL;
    }

    LitevfsDetachFile(file);
    return (ssize_t)MapToPosixRet(ret);
}

off_t lseek(int fd, off_t offset, int whence)
{
    struct File *file = NULL;
    off_t ret = LITE_VFS_FAIL;

    file = LitevfsAttachReadyFile(fd);
    if (file == NULL) {
        return ret;
    }
    if ((file->fileOps != NULL) && (file->fileOps->lseek != NULL)) {
        ret = file->fileOps->lseek(file, offset, whence);
    } else {
        ret = (off_t)file->fileOffset;
    }

    LitevfsDetachFile(file);
    return (off_t)MapToPosixRet(ret);
}

struct dir *opendir(const char *path)
{
    struct MountPoint *mountp = NULL;
    const char *pathInMp = NULL;
    struct dir *dir = NULL;

    if (path == NULL) {
        return NULL;
    }

    if (KAL_MutexAcquire(&g_fsMutex) != KAL_OK) {
        return NULL;
    }
    mountp = LitevfsFindMountp(path, &pathInMp);
    if ((mountp == NULL) || (pathInMp == NULL) || (*pathInMp == '\0')) {
        (void)KAL_MutexRelease(&g_fsMutex);
        return NULL;
    }

    if (KAL_MutexAcquire(&mountp->mountMutex) != KAL_OK) {
        (void)KAL_MutexRelease(&g_fsMutex);
        return NULL;
    }

    if ((mountp->mountFs == NULL) || (mountp->mountFs->fileOps == NULL) ||
        (mountp->mountFs->fileOps->opendir == NULL)) {
        (void)KAL_MutexRelease(&g_fsMutex);
        (void)KAL_MutexRelease(&mountp->mountMutex);
        return NULL;
    }

    (void)KAL_MutexRelease(&g_fsMutex);
    dir = (struct dir *)malloc(sizeof(struct dir));
    if (dir == NULL) {
        (void)KAL_MutexRelease(&mountp->mountMutex);
        return NULL;
    }
    dir->dirMp = mountp;
    dir->dirOffset = 0;
    
    int ret = mountp->mountFs->fileOps->opendir(dir, pathInMp);
    if (ret == 0) {
        mountp->mountRefs++;
    } else {
        free(dir);
        dir = NULL;
    }
    (void)KAL_MutexRelease(&mountp->mountMutex);
    return dir;
}

struct dirent *readdir(struct dir *dir)
{
    struct MountPoint *mountp = NULL;
    struct dirent *ret = NULL;
    
    if ((dir == NULL) || (dir->dirMp == NULL)) {
        return ret;
    }
    mountp = (struct MountPoint *)dir->dirMp;
    if (KAL_MutexAcquire(&mountp->mountMutex) != KAL_OK) {
        return ret;
    }

    if ((mountp->mountFs != NULL) && (mountp->mountFs->fileOps != NULL) &&
        (mountp->mountFs->fileOps->readdir != NULL)) {
        if (mountp->mountFs->fileOps->readdir(dir, &dir->dirEntry) == 0) {
            ret = &dir->dirEntry;
        }
    }
    (void)KAL_MutexRelease(&mountp->mountMutex);
    return ret;
}

int closedir(struct dir *dir)
{
    struct MountPoint *mountp = NULL;
    int ret = LITE_VFS_FAIL;
    
    if ((dir == NULL) || (dir->dirMp == NULL)) {
        return ret;
    }

    mountp = (struct MountPoint *)dir->dirMp;
    if (KAL_MutexAcquire(&mountp->mountMutex) != KAL_OK) {
        return ret;
    }

    if ((mountp->mountFs != NULL) && (mountp->mountFs->fileOps != NULL) &&
       (mountp->mountFs->fileOps->closedir != NULL)) {
        ret = mountp->mountFs->fileOps->closedir(dir);
    } else {
        ret = LITE_VFS_FAIL;
    }
    if (ret == 0) {
        mountp->mountRefs--;
        free(dir);
        dir = NULL;
    }

    (void)KAL_MutexRelease(&mountp->mountMutex);
    return MapToPosixRet(ret);
}

int mkdir(const char *path, int mode)
{
    struct MountPoint *mountp = NULL;
    const char *pathInMp = NULL;
    int ret = LITE_VFS_FAIL;
    (void)mode;
    if (path == NULL) {
        return ret;
    }

    if (KAL_MutexAcquire(&g_fsMutex) != KAL_OK) {
        return ret;
    }

    mountp = LitevfsFindMountp(path, &pathInMp);
    if ((mountp == NULL) || (pathInMp == NULL) || (*pathInMp == '\0')) {
        (void)KAL_MutexRelease(&g_fsMutex);
        return ret;
    }

    if (KAL_MutexAcquire(&mountp->mountMutex) != KAL_OK) {
        (void)KAL_MutexRelease(&g_fsMutex);
        return LITE_VFS_FAIL;
    }

    (void)KAL_MutexRelease(&g_fsMutex);

    if ((mountp->mountFs != NULL) && (mountp->mountFs->fileOps != NULL) && (mountp->mountFs->fileOps->mkdir != NULL)) {
        ret = mountp->mountFs->fileOps->mkdir(mountp, pathInMp);
    } else {
        ret = LITE_VFS_FAIL;
    }

    (void)KAL_MutexRelease(&mountp->mountMutex) ;
    return MapToPosixRet(ret);
}
int unlink(const char *path)
{
    struct MountPoint *mountp = NULL;
    const char *pathInMp = NULL;
    int ret = LITE_VFS_FAIL;

    if (path == NULL) {
        return ret;
    }
    if (KAL_MutexAcquire(&g_fsMutex) != KAL_OK) {
        return LITE_VFS_FAIL;
    }

    mountp = LitevfsFindMountp(path, &pathInMp);
    if ((mountp == NULL) || (pathInMp == NULL) || (*pathInMp == '\0')) {
        (void)KAL_MutexRelease(&g_fsMutex);
        return ret;
    }
    if ((KAL_MutexAcquire(&mountp->mountMutex) != KAL_OK)) {
        (void)KAL_MutexRelease(&g_fsMutex);
        return ret;
    }
    (void)KAL_MutexRelease(&g_fsMutex);

    if ((mountp->mountFs != NULL) && (mountp->mountFs->fileOps != NULL) && (mountp->mountFs->fileOps->unlink != NULL)) {
        ret = mountp->mountFs->fileOps->unlink(mountp, pathInMp);
    } else {
        ret = LITE_VFS_FAIL;
    }

    (void)KAL_MutexRelease(&mountp->mountMutex);
    return MapToPosixRet(ret);
}

int rename(const char *oldpath, const char *newpath)
{
    struct MountPoint *oldMountp = NULL;
    struct MountPoint *newMountp = NULL;
    const char *oldPathInMp = NULL;
    const char *newPathInMp = NULL;
    int ret = LITE_VFS_FAIL;
    if ((oldpath == NULL) || (newpath == NULL)) {
        return ret;
    }

    if (KAL_MutexAcquire(&g_fsMutex) != KAL_OK) {
        return LITE_VFS_FAIL;
    }

    oldMountp = LitevfsFindMountp(oldpath, &oldPathInMp);
    if (oldPathInMp == NULL) {
        (void)KAL_MutexRelease(&g_fsMutex);
        return ret;
    }

    if ((oldMountp == NULL) || (*oldPathInMp == '\0') || (oldMountp->mountFs == NULL) ||
        (oldMountp->mountFs->fileOps == NULL)) {
        (void)KAL_MutexRelease(&g_fsMutex);
        return ret;
    }

    newMountp = LitevfsFindMountp(newpath, &newPathInMp);
    if ((newMountp == NULL) || (newPathInMp == NULL) || (*newPathInMp == '\0') || (newMountp->mountFs == NULL) ||
        (newMountp->mountFs->fileOps == NULL)) {
        (void)KAL_MutexRelease(&g_fsMutex);
        return ret;
    }

    if (oldMountp != newMountp) {
        (void)KAL_MutexRelease(&g_fsMutex);
        return ret;
    }

    if ((KAL_MutexAcquire(&oldMountp->mountMutex) != KAL_OK)) {
        (void)KAL_MutexRelease(&g_fsMutex);
        return ret;
    }
    (void)KAL_MutexRelease(&g_fsMutex);
    if (oldMountp->mountFs->fileOps->rename != NULL) {
        ret = oldMountp->mountFs->fileOps->rename(oldMountp, oldPathInMp, newPathInMp);
    } else {
        ret = LITE_VFS_FAIL;
    }

    (void)KAL_MutexRelease(&oldMountp->mountMutex);
    return MapToPosixRet(ret);
}

int rmdir(const char *path)
{
    struct MountPoint *mountp = NULL;
    const char *pathInMp = NULL;
    int ret = LITE_VFS_FAIL;
    if (path == NULL) {
        return ret;
    }
    if (KAL_MutexAcquire(&g_fsMutex) != KAL_OK) {
        return LITE_VFS_FAIL;
    }
    mountp = LitevfsFindMountp(path, &pathInMp);
    if ((mountp == NULL) || (pathInMp == NULL) || (*pathInMp == '\0') || (mountp->mountFs == NULL) ||
        (mountp->mountFs->fileOps == NULL)) {
        (void)KAL_MutexRelease(&g_fsMutex);
        return ret;
    }

    if ((KAL_MutexAcquire(&mountp->mountMutex) != KAL_OK)) {
        (void)KAL_MutexRelease(&g_fsMutex);
        return ret;
    }
    (void)KAL_MutexRelease(&g_fsMutex);
    if (mountp->mountFs->fileOps->rmdir != NULL) {
        ret = mountp->mountFs->fileOps->rmdir(mountp, pathInMp);
    } else {
        ret = LITE_VFS_FAIL;
    }
    (void)KAL_MutexRelease(&mountp->mountMutex);
    return MapToPosixRet(ret);
}

int stat(const char *path, struct stat *stat)
{
    struct MountPoint *mountp = NULL;
    const char *pathInMp = NULL;
    int ret = LITE_VFS_FAIL;

    if ((path == NULL) || (stat == NULL)) {
        return ret;
    }
    if (KAL_MutexAcquire(&g_fsMutex) != KAL_OK) {
        return LITE_VFS_FAIL;
    }

    mountp = LitevfsFindMountp(path, &pathInMp);
    if ((mountp == NULL) || (pathInMp == NULL) || (*pathInMp == '\0') || (mountp->mountFs == NULL) ||
        (mountp->mountFs->fileOps == NULL)) {
        (void)KAL_MutexRelease(&g_fsMutex);
        return ret;
    }

    if ((KAL_MutexAcquire(&mountp->mountMutex) != KAL_OK)) {
        (void)KAL_MutexRelease(&g_fsMutex);
        return ret;
    }
    (void)KAL_MutexRelease(&g_fsMutex);
    if (mountp->mountFs->fileOps->stat != NULL) {
        ret = mountp->mountFs->fileOps->stat(mountp, pathInMp, stat);
    } else {
        ret = LITE_VFS_FAIL;
    }
    (void)KAL_MutexRelease(&mountp->mountMutex);
    return MapToPosixRet(ret);
}

static int LitevfsCheckName(const char *name, int maxLen)
{
    char ch;
    int len = 0;
    do {
        ch = *(name++);
        if (ch == '\0') {
            break;
        }

        if (((ch >= 'a') && (ch <= 'z')) || ((ch >= 'A') && (ch <= 'Z')) || ((ch >= '0') && (ch <= '9')) ||
            (ch == '_') || (ch == '-')) {
            len++;
            if (len == maxLen) {
                return LITE_VFS_FAIL;
            }
            continue;
        } else {
            return LITE_VFS_FAIL;
        }
    } while (1);

    return len == 0 ? LITE_VFS_FAIL : LITE_VFS_SUCCESS;
}

static struct FileSystem *LitevfsFindFs(const char *name)
{
    struct FileSystem *fs;

    for (fs = g_fileSystems; fs != NULL; fs = fs->fsNext) {
        if (strncmp(fs->fsName, name, LITE_VFS_MAX_NAME_LEN) == 0) {
            break;
        }
    }
    return fs;
}

/* The caller must ensure that the parameter is not empty. */
static int IsValidPath(const char *path)
{
    if (path[0] != '/') {
        return -1; /* The value must be an absolute path. */
    }

    return LitevfsCheckName((path + 1), PATH_MAX_LEN - 1);
}

int mount(const char *source, const char *target,
    const char *fileSystemType, unsigned long mountflags,
    const void *data)
{
    (void)source;
    (void)mountflags;
    int ret = LITE_VFS_FAIL;
    const char *tmp = NULL;
    if (target == NULL || fileSystemType == NULL) {
        return ret;
    }
    if (IsValidPath(target) != LITE_VFS_SUCCESS) {
        return LITE_VFS_FAIL;
    }
    if (KAL_MutexAcquire(&g_fsMutex) != KAL_OK) {
        return LITE_VFS_FAIL;
    }
    struct FileSystem *fs = LitevfsFindFs(fileSystemType);
    if (fs == NULL) {
        goto free_mutex;
    }
    struct MountPoint *mountp = LitevfsFindMountp(target, &tmp);
    if ((mountp != NULL) && (tmp != NULL) && (*tmp == '\0')) {
        goto free_mutex;
    }
    mountp = malloc(sizeof(struct MountPoint));
    if (mountp == NULL) {
        goto free_mutex;
    }
    (void)memset_s(mountp, sizeof(struct MountPoint), 0, sizeof(struct MountPoint));
    mountp->mountFs = fs;
    mountp->mountPath = target;

    if ((mountp->mountFs->fileOps != NULL) && (mountp->mountFs->fileOps->mount != NULL)) {
        ret = mountp->mountFs->fileOps->mount(mountp, data);
        if (ret != LITE_VFS_SUCCESS) {
            goto free_mountp;
        }
    }

    mountp->mountRefs = 0;
    if (KAL_MutexInit(&mountp->mountMutex) != KAL_OK) {
        goto free_mountp;
    }
    mountp->mountNext = g_mountPoints;
    g_mountPoints = mountp;
    mountp->mountFs->fsRefs++;
    (void)KAL_MutexRelease(&g_fsMutex);
    return LITE_VFS_SUCCESS;

free_mountp:
    free(mountp);
free_mutex:
    (void)KAL_MutexRelease(&g_fsMutex);
    return LITE_VFS_FAIL;
}

int umount(const char *target)
{
    struct MountPoint *prev = NULL;
    const char *tmp = NULL;

    if (target == NULL) {
        return LITE_VFS_FAIL;
    }
    if (IsValidPath(target) != LITE_VFS_SUCCESS) {
        return LITE_VFS_FAIL;
    }
    if (KAL_MutexAcquire(&g_fsMutex) != KAL_OK) {
        return LITE_VFS_FAIL;
    }

    struct MountPoint *mountp = LitevfsFindMountp(target, &tmp);
    if ((mountp == NULL) || (tmp == NULL) || (*tmp != '\0') || (mountp->mountRefs != 0)) {
        (void)KAL_MutexRelease(&g_fsMutex);
        return LITE_VFS_FAIL;
    }
    if ((KAL_MutexAcquire(&mountp->mountMutex) != KAL_OK)) {
        (void)KAL_MutexRelease(&g_fsMutex);
        return LITE_VFS_FAIL;
    }
    if ((mountp->mountFs != NULL) && (mountp->mountFs->fileOps != NULL) &&
        (mountp->mountFs->fileOps->umount != NULL)) {
        int ret = mountp->mountFs->fileOps->umount(mountp);
        if (ret != LITE_VFS_SUCCESS) {
            (void)KAL_MutexRelease(&g_fsMutex);
            (void)KAL_MutexRelease(&mountp->mountMutex);
            return LITE_VFS_FAIL;
        }
    }
    (void)KAL_MutexRelease(&mountp->mountMutex);

    if (g_mountPoints == mountp) {
        g_mountPoints = mountp->mountNext;
    } else {
        for (prev = g_mountPoints; prev != NULL; prev = prev->mountNext) {
            if (prev->mountNext != mountp) {
                continue;
            }
            prev->mountNext = mountp->mountNext;
            break;
        }
    }
    (void)KAL_MutexDeinit(&mountp->mountMutex);
    if (mountp->mountFs != NULL) {
        mountp->mountFs->fsRefs--;
    }
    free(mountp);
    (void)KAL_MutexRelease(&g_fsMutex);
    return LITE_VFS_SUCCESS;
}

char *realpath(const char *path, char *resolved_path)
{
    if (path == NULL) {
        return NULL;
    }
    char *new_path = NULL;
    size_t pathSize = strlen(path) + 1;
    if (resolved_path == NULL) {
        new_path = (char *)malloc(pathSize);
        if (new_path == NULL) {
            return NULL;
        }
        int result = strcpy_s(new_path, pathSize, path);
        if (result != 0) {
            free(new_path);
            return NULL;
        }
        return new_path;
    }

    int result = strcpy_s(resolved_path, pathSize, path);
    if (result != 0) {
        return NULL;
    }
    return resolved_path;
}

int LitevfsRegisterFs(struct FileSystem *fs)
{
    if ((fs == NULL) || (fs->fileOps == NULL)) {
        return LITE_VFS_FAIL;
    }

    if (LitevfsCheckName(fs->fsName, LITE_VFS_MAX_NAME_LEN) != LITE_VFS_SUCCESS) {
        return LITE_VFS_FAIL;
    }

    if (LitevfsFindFs(fs->fsName) != NULL) {
        return LITE_VFS_FAIL;
    }

    if (KAL_MutexInit(&g_fsMutex) != KAL_OK) {
        return LITE_VFS_FAIL;
    }

    fs->fsNext = g_fileSystems;
    g_fileSystems = fs;

    return LITE_VFS_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif  /* __cplusplus */