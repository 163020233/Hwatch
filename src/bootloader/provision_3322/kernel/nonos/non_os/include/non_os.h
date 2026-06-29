/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:  NON OS Utils Header
 * Author:
 * Create:
 */

#ifndef NON_OS_H
#define NON_OS_H

#include "core.h"
#include "memory_config.h"

/**
 * @defgroup connectivity_drivers_non_os_utils UTILS
 * @ingroup  connectivity_drivers_non_os
 * @{
 */
#define THREAD_MODE_PRIORITY 4

#ifndef __return_address
#define __return_address() ((uintptr_t)__builtin_return_address(0))
#endif

/**
 * @brief  Enter in a critical section.
 */
void non_os_enter_critical(void);

/**
 * @brief  Exit a critical section.
 */
void non_os_exit_critical(void);

/**
 * @brief  Checks if the system is running on a critical section.
 * @return true if the system is running in a critical section.
 */
bool non_os_is_in_critical_section(void);

/**
 * @brief  Ensures interrupts are enabled and the interrupt nested counter is reset.
 */
void non_os_critical_section_init(void);


/**
 * @}
 */
#endif
