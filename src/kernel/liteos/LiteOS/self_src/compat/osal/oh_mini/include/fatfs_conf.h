/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Adapt File
 * Author: Huawei LiteOS Team
 */

#ifndef _FATFS_CONF_H
#define _FATFS_CONF_H

#ifndef FAT_MAX_OPEN_FILES
#define FAT_MAX_OPEN_FILES    50
#endif /* FAT_MAX_OPEN_FILES */

/* Format options */
#define FMT_FAT      0x01
#define FMT_FAT32    0x02
#define FMT_ANY      0x07

#endif /* _FATFS_CONF_H */
