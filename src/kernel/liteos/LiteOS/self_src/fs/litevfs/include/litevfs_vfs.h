/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: litevfs File System head file
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
#ifndef LITEVFS_H
#define LITEVFS_H

#include "stdint.h"
#include "dirent.h"
#include "sys/stat.h"
#include "stdio.h"
#include "kal_mutex.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif  /* __cplusplus */

#define LITE_VFS_MAX_DIR_NAME_LEN 256
#define LITE_VFS_MAX_FILE_NAME_LEN 32
#define LITE_VFS_MAX_NAME_LEN LITE_VFS_MAX_FILE_NAME_LEN
#define LITE_VFS_MAX_FILES 8

typedef enum {
    VFS_TYPE_FILE   = 0,    /* regular file */
    VFS_TYPE_DIR    = 1,    /* diretory file */
    VFS_TYPE_OTHER  = 2,    /* other file */
} FsFileType;

typedef enum {
    FILE_STATUS_NOT_USED    = 0,    /* 未使用 */
    FILE_STATUS_INITING     = 1,    /* 初始化中 */
    FILE_STATUS_READY       = 2,    /* 就绪态 */
    FILE_STATUS_CLOSING     = 3,    /* 正在关闭 */
} FsFileStatus;

typedef enum {
    FILE_MODE_READ_ONLY     = 0,    /* 只读模式 */
    FILE_MODE_WRITE_ONLY    = 1,    /* 只写模式 */
    FILE_MODE_READ_WRITE    = 2,    /* 读写模式 */
} FsFileMode;

struct FileSystem {
    const char fsName[LITE_VFS_MAX_NAME_LEN];
    struct FileOps *fileOps;
    struct FileSystem *fsNext;
    volatile uint32_t fsRefs;
};

struct MountPoint {
    struct FileSystem *mountFs;
    struct MountPoint *mountNext;
    const char *mountPath;
    volatile uint32_t mountRefs;
    KalMutex mountMutex;
    void *mountData; /* used by fs private data for this mount point (like /sdb1, /sdb2), */
};

struct File {
    struct FileOps *fileOps;
    uint32_t fileFlags;
    FsFileStatus fileStatus;
    uint32_t fileOffset;
    struct MountPoint *fileMp;  /* can get private mount data here */
    uint32_t fileOwner;         /* the task that openned this file */
    void *fileData;             /* specific file pointer */
    const char *fullPath;
};

struct FileOps {
    int     (*open)(struct File *, const char *, int);
    int     (*close)(struct File *);
    ssize_t (*read)(struct File *, char *, size_t);
    ssize_t (*write)(struct File *, const char *, size_t);
    off_t   (*lseek)(struct File *, off_t, int);
    int     (*opendir)(struct dir *, const char *);
    int     (*readdir)(struct dir *, struct dirent *);
    int     (*closedir)(struct dir *);
    int     (*unlink)(struct MountPoint *, const char *);
    int     (*mkdir)(struct MountPoint *, const char *);
    int     (*rmdir) (struct MountPoint *, const char *);
    int     (*rename)(struct MountPoint *, const char *, const char *);
    int     (*stat)(struct MountPoint *, const char *, struct stat *);
    int     (*mount)(struct MountPoint *,  const void *);
    int     (*umount)(struct MountPoint *);
};


void *LittlefsRegister(void);
int LitevfsRegisterFs(struct FileSystem *fs);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif  /* __cplusplus */

#endif  /* LITEVFS_H */