/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: host platform pm dfr header file
 * Create: 2025-01
 */

#ifndef __PLAT_PM_DFR_H__
#define __PLAT_PM_DFR_H__

#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* DFR STATUS */
#define DFR_STATUS_IDLE         0
#define DFR_STATUS_PROCESSING   1

void plat_dfr_lock(void);
void plat_dfr_unlock(void);
uint32_t plat_dfr_status_get(void);
void plat_dfr_status_set(uint32_t status);
errcode_t plat_exception_init(void);
void plat_exception_exit(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __PLAT_PM_DFR_H__ */

