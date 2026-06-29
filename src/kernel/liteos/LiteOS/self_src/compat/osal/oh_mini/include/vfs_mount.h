/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Vfs Mount HeadFile
 * Author: Huawei LiteOS Team
 * Create : 2025-03-01
 */

#ifndef _VFS_MOUNT_H_
#define _VFS_MOUNT_H_

#include "sys/statfs.h"
#include "los_compiler.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct FsMap;
struct MountPoint;

struct MountOps {
    int (*mount)(struct MountPoint *mp, unsigned long mountflags,
                 const void *data);
    int (*umount)(struct MountPoint *mp);
    int (*umount2)(struct MountPoint *mp, int flag);
    int (*statfs)(const char *path, struct statfs *buf);
};

struct MountPoint {
    struct FsMap      *mFs;    /* file system info */
    struct MountPoint *mNext;  /* point to next mount point */
    const char        *mPath;  /* target path, /system, /usr, etc. */
    const char        *mDev;   /* device, "emmc0p0", "emmc0p1", etc. */
    UINT32             mRefs;  /* reference to mount point */
    void              *mData;  /* specific file system handle */
    BOOL               mWriteEnable; /* writable flag */
};

extern struct MountPoint *g_mountPoints;
#define LOS_MP_FOR_EACH_ENTRY(prev) \
        for ((prev) = g_mountPoints; (prev) != NULL; (prev) = (prev)->mNext)

struct MountPoint *VfsMpFind(const char *path, const char **pathInMp);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _VFS_MOUNT_H_ */