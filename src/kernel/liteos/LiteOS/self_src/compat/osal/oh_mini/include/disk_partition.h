/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: MBR multi-partition
 * Author: Huawei LiteOS Team
 * Create: 2025-02-16
 */
#ifndef __DISK_PARTITION_H
#define __DISK_PARTITION_H

#include <nuttx/fs/fs.h>
#include "los_typedef.h"

INT32 LOS_DiskPartition(const CHAR *dev, const CHAR *fsType, INT32 *lengthArray, const INT32 *addrArray, INT32 partNum);
extern const struct block_operations *mmcsd_block_get_bops(void);

#endif
