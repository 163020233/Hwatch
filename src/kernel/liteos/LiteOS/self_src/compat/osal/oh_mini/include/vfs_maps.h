/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Vfs Maps HeadFile
 * Author: Huawei LiteOS Team
 * Create : 2025-03-01
 */

#ifndef _VFS_MAPS_H_
#define _VFS_MAPS_H_

#include "los_compiler.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct MountOps;

struct FsManagement {
    int (*fdisk)(const char *dev, int *lengthArray, int partNum);
    int (*format)(const char *partName, void *data);
};


struct FsMap {
    const char *fsType;
    const struct MountOps *fsMops;
    const struct FileOps *fsFops;
    const struct FsManagement *fsMgt;
    UINT32 fsRefs;
    struct FsMap *next;
};

int OsFsRegister(const char *fsType, const struct MountOps *fsMops,
                 const struct FileOps *fsFops, const struct FsManagement *fsMgt);
struct FsMap *VfsFsMapGet(const char *fsType);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _VFS_MAPS_H_ */
