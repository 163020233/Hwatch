/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description:  diag report control
 * This file should be changed only infrequently and with great care.
 */

#ifndef __DIAG_DATA_MOCK_H__
#define __DIAG_DATA_MOCK_H__

#include "errcode.h"
#include "diag.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
* @brief: Inject mocked anomaly CPU metrics
* @param permil_measured: Real CPU usage in permil (1000 = 100%)
*/
uint32_t inject_cpu_anomaly(uint32_t permil_measured);

#ifdef __cplusplus
}
#endif

#endif /* __DIAG_DATA_MOCK_H__ */