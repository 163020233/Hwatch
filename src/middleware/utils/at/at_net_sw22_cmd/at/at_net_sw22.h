/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: At bt header \n
 *
 */

#ifndef __AT_NET_SW22_H__
#define __AT_NET_SW22_H__

#include "td_base.h"
#include "at.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
    const char* cmd;
    uint32_t (*handler)(uint8_t argc, char** argv);
} at_net_sw22_cmd;

td_void los_at_net_sw22_cmd_register(td_void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* end of at_net_sw22.h */
