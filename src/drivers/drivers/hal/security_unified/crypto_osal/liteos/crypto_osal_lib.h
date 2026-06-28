/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: for osal adapt.
 *
 * Create: 2023-04-21
*/

#ifndef CRYPTO_OSAL_LIB_H
#define CRYPTO_OSAL_LIB_H

#include "td_type.h"
#include "soc_osal.h"
#include "osal_wait.h"

#include "crypto_security.h"
#include "crypto_common_macro.h"
#include "crypto_common_struct.h"
#ifdef CONFIG_SECURITY_UNIFIED_SUPPORT_DEEP_SLEEP
#include "pm_veto.h"
#else
#define uapi_pm_add_sleep_veto(...)
#define uapi_pm_remove_sleep_veto(...)
#define PM_SECURITY_VETO_ID
#define PM_SECURITY_KM_VETO_ID KM_MODULE_ID
#define PM_SECURITY_PKE_VETO_ID PKE_MODULE_ID
#define PM_SECURITY_SYMC_VETO_ID SYMC_MODULE_ID
#define PM_SECURITY_HASH_VETO_ID HASH_MODULE_ID
#define PM_SECURITY_TRNG_VETO_ID TRNG_MODULE_ID
#endif

#if defined(CONFIG_CRYPTO_PRINT_ENABLE)
#define crypto_print            osal_printk
#endif

/* deep sleep vote & other process. */
void crypto_preprocess(unsigned int module_id);
void crypto_postprocess(unsigned int module_id);

#define crypto_service_preprocess(module_id)     crypto_preprocess(module_id)
#define crypto_service_postprocess(module_id)    crypto_postprocess(module_id)

/* Mutex. */
#define crypto_mutex            osal_mutex
#define crypto_mutex_init       osal_mutex_init
#define crypto_mutex_destroy    osal_mutex_destroy
#define crypto_mutex_lock       osal_mutex_lock
#define crypto_mutex_unlock     osal_mutex_unlock

/* atomic. */
#define crypto_atomic               osal_atomic
#define crypto_atomic_read          osal_atomic_read
#define crypto_atomic_inc           osal_atomic_inc
#define crypto_atomic_dec           osal_atomic_dec
#define crypto_atomic_inc_return    osal_atomic_inc_return
#define crypto_atomic_dec_return    osal_atomic_dec_return

#endif