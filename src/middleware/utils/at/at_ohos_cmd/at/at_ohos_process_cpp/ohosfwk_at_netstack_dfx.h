/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved. \n
 *
 * Description: Provides at cmd register func \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2026-02-10， Create file. \n
 */
#ifndef OHOSFWK_AT_NETSTACK_DFX_H
#define OHOSFWK_AT_NETSTACK_DFX_H

#include <stdint.h>
#include <stdio.h>
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#ifdef CONFIG_NETSTACK_TEST
uint32_t OhosfwkATNetstackDfx(uint8_t argc, char *argv[]);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
