/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: vfs file HeadFile
 * Author: Huawei LiteOS Team
 * Create : 2025-03-01
 */

#ifndef _VFS_FILES_H
#define _VFS_FILES_H
#include "dirent.h"
#include "sys/stat.h"
#include "unistd.h"
#include "los_compiler.h"
#include "ff.h"
#include "fatfs_conf.h"
#include "los_memory_internal.h"
#include "los_memory_pri.h"
#include "partition.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef LOSCFG_MAX_OPEN_DIRS
#define LOSCFG_MAX_OPEN_DIRS 10
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

#if defined(SUPPORT_DISK_PART)
extern struct mmcsd_disk_part disk_info;
extern int emmc_set_diskinfo(struct mmcsd_disk_part *disk_info_t, struct ptentry *mmc_disk);
#endif
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif