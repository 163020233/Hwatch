/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: diag data store adapt
 * This file should be changed only infrequently and with great care.
 */

#ifndef DIAG_DATA_STORE_ADAPT_H
#define DIAG_DATA_STORE_ADAPT_H

#include "diag_data_store.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 获取非关键诊断队列驱动的实例
 *
 * @return 返回指向静态驱动实例的指针
 */
diag_data_store_driver_t *diag_data_store_queue_get_non_critical_driver(void);

/**
 * @brief 获取关键诊断队列驱动的实例
 *
 * @return 返回指向静态驱动实例的指针
 */
diag_data_store_driver_t *diag_data_store_queue_get_critical_driver(void);

#ifdef __cplusplus
}
#endif

#endif // __DIAG_DATA_STORE_QUEUE_H__