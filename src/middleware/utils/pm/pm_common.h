/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: Provides power management api \n
 *
 * History: \n
 * 2025-07-23， Create file. \n
 */

#ifndef PM_COMMON_H
#define PM_COMMON_H

#include "common_def.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup middleware_utils_pm_common API
 * @ingroup  middleware_utils_pm
 * @{
 */

/**
 * @if Eng
 * @brief  The adapter layer interface for the malloc function.
 * @param  [in]  len the length of data.
 * @retval NULL           Failure.
 * @retval Other          Success, the address of allocated memory
 * @else
 * @brief  malloc函数的适配层接口。
 * @retval NULL           失败.
 * @retval Other          成功, 分配的内存地址。
 * @endif
 */
__attribute__((weak)) void *pm_kmalloc(uint32_t len);

/**
 * @if Eng
 * @brief  The adapter layer interface for the zalloc function.
 * @param  [in]  len the length of data.
 * @retval NULL           Failure.
 * @retval Other          Success, the address of allocated memory
 * @else
 * @brief  zalloc函数的适配层接口。
 * @retval NULL           失败.
 * @retval Other          成功, 分配的内存地址。
 * @endif
 */
__attribute__((weak)) void *pm_kzalloc(uint32_t len);

/**
 * @if Eng
 * @brief  The adapter layer interface for the free function.
 * @param  [in]  addr the address of allocated memory.
 * @else
 * @brief  free函数的适配层接口。
 * @param  [in]  addr 分配的内存地址.
 * @endif
 */
__attribute__((weak)) void pm_free(void *addr);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif