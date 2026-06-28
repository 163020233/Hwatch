/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provide api functions for at \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-09-25， Create file. \n
 */
#ifndef UX_AT_SPP_TEST_H
#define UX_AT_SPP_TEST_H
#include "stdint.h"
#include "errcode.h"
#include "at.h"
#include "ux_at_manager.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

errcode_t ux_at_test_spp_set_process(const char *func_name, const char *device, const char *param,
    const char *sub_param);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
