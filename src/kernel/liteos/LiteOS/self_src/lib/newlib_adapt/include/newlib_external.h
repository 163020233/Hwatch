/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: 适配newlib接口模块间头文件。
 * Author: LibCK项目组
 * Create: 2025-04-15
 * History: 2025-04-15 LibCK项目组 创建文件
 */
#ifndef NEWLIB_EXTERNAL_H
#define NEWLIB_EXTERNAL_H

#include "sre_cpu_external.h"

#define SPIN_LOCK_INITIALIZER { 0 }

#define SPIN_LOCK_RECURSIVE_INITIALIZER { SPIN_LOCK_INITIALIZER, -1, 0 }

#define SPIN_LOCK_RECURSIVE_COUNT_UPPER_LIMIT 0x7fffffff

/*
 * @ingroup os_newlib
 * 自旋锁结构体
 */
typedef struct {
    volatile int locked;
} spin_lock_t;

/*
 * @ingroup os_newlib
 * 可重入自旋锁结构体
 */
typedef struct {
    spin_lock_t sl;
    volatile int user;
    volatile int count;
} spin_lock_recursive_t;

#endif /* NEWLIB_EXTERNAL_H */
