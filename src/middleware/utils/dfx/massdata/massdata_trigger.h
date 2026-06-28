/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 * Description: MASSDATA TRIGGER MODULE INTERRFACE
 */

#ifndef MASSSATA_TRIGGER_H
#define MASSSATA_TRIGGER_H

/**
 * @addtogroup connectivity_libs_log
 * @{
 */
#include "core.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define CORE_LOGGING APPS

typedef void (*massdata_trigger_callback_t)(void *, uint8_t, uint8_t);

/**
 * @brief  trigger ipc to inform A core to save data from share mem in flash
 * @param  pay_i massdata_memory_region_section_t
 * @param  type 0-event, 1-error
 */
void massdata_trigger(void *pay_i, uint8_t core, uint8_t type);

/**
 * @brief  register mass data save callback
 * @param  callback: should match the type<massdata_trigger_callback_t>.
 */
void register_massdata_trigger(massdata_trigger_callback_t callback);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* MASSSATA_TRIGGER_H */
