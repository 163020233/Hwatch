/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: for osal adapt.
*/

#ifndef CRYPTO_OSAL_LIB_H
#define CRYPTO_OSAL_LIB_H

#include "td_type.h"
#include "soc_osal.h"
#include "osal_wait.h"
#include "tcxo.h"
#include "los_mem.h"

#include "crypto_security.h"
#include "crypto_common_macro.h"
#include "crypto_common_struct.h"

#if defined(CONFIG_CRYPTO_PRINT_ENABLE)
#define crypto_print            osal_printk
#endif

/* deep sleep vote & other process. */
void crypto_preprocess(unsigned int module_id);
void crypto_postprocess(unsigned int module_id);
#define crypto_service_preprocess(module_id)     crypto_preprocess(module_id)
#define crypto_service_postprocess(module_id)    crypto_postprocess(module_id)

#endif
