/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 * Description: ta_entry header file.
 */

#ifndef TA_ENTRY_H
#define TA_ENTRY_H

#include "tee_common_def.h"

TEE_Result utee_entry(unsigned long func, unsigned long ta_info,
                      TEE_Param *up, unsigned long cmd_id, uint32_t param_type);

#endif /* TA_ENTRY_H */
