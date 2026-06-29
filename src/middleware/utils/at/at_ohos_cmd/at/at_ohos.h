/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: At bt header \n
 *
 */

#ifndef __AT_OHOS_H__
#define __AT_OHOS_H__

#include "td_base.h"
#include "at.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    const char* cmd;
    uint32_t (*handler)(uint8_t argc, char** argv);
} at_ohos_cmd;

td_void los_at_ohos_cmd_register(td_void);

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif /* end of at_ohos.h */
