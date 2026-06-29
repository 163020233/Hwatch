/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: vfs file HeadFile
 * Author: Huawei LiteOS Team
 * Create : 2024-04-14
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

#ifndef _VFS_FILES_H
#define _VFS_FILES_H
#include "dirent.h"
#include "sys/stat.h"
#include "unistd.h"
#include "los_compiler.h"
#include "los_memory_internal.h"
#include "los_memory_pri.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#ifndef LOSCFG_MAX_OPEN_DIRS
#define LOSCFG_MAX_OPEN_DIRS 10
#endif

#ifndef FF_VOLUMES
#define FF_VOLUMES      LOSCFG_FS_FAT_VOLUMES
#endif

struct FileOps;
struct File;
struct Dir;
struct MountPoint;

struct FileOps {
    int     (*open)(struct File *, const char *, int);
    int     (*close)(struct File *);
    ssize_t (*read)(struct File *, char *, size_t);
    ssize_t (*write)(struct File *, const char *, size_t);
    off_t   (*lseek)(struct File *, off_t, int);
    int     (*stat)(struct MountPoint *, const char *, struct stat *);
    int     (*truncate)(struct File *, off_t);
    int     (*unlink)(struct MountPoint *, const char *);
    int     (*rename)(struct MountPoint *, const char *, const char *);
    int     (*ioctl)(struct File *, int, unsigned long);
    int     (*sync)(struct File *);
    int     (*opendir)(struct Dir *, const char *);
    int     (*readdir)(struct Dir *, struct dirent *);
    int     (*closedir)(struct Dir *);
    int     (*mkdir)(struct MountPoint *, const char *);
    int     (*rmdir)(struct MountPoint *, const char *);
};

struct File {
    const struct FileOps *fFops;
    UINT32 fFlags;
    UINT32 fStatus;
    off_t fOffset;
    INT32 fOwner;
    struct MountPoint *fMp;
    void *fData; /* file system opreations handle, fatfs FIL, etc. */
    const char *fullPath;
};

struct Dir {
    struct MountPoint *dMp;
    struct dirent      dDent;
    off_t              dOffset;
    void              *dData;
};

void LOS_FsUnlock(void);
int LOS_FsLock(void);
void FS_LockInit(void);
int LOS_PartitionFormat(const char *partName, char *fsType, void *data);
int LOS_DiskPartition(const char *dev, const char *fsType, int *lengthArray, int *addrArray,
                      int partNum);
extern const struct block_operations *mmcsd_block_get_bops(void);
extern struct mmcsd_disk_part disk_info;

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif