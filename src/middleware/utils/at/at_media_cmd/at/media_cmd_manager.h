/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description: Provides at cmd register func \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-02-26， Create file. \n
 */
#ifndef MEDIA_CMD_MANAGER_H
#define MEDIA_CMD_MANAGER_H

#include <stdint.h>
#include <stdio.h>
#include "errcode.h"
#include "securec.h"
#include "at_cmd_api.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef struct {
    uint32_t para_map;
    char *media_cmd_str;
    char                 *para1;
    char                 *para2;
    char                 *para3;
    char                 *para4;
    char                 *para5;
    char                 *para6;
    char                 *para7;
    char                 *para8;
    char                 *para9;
    char                 *para10;
    char                 *para11;
    char                 *para12;
    char                 *para13;
    char                 *para14;
    char                 *para15;
    char                 *para16;
    char                 *para17;
    char                 *para18;
    char                 *para19;
    char                 *para20;
} media_args;

uint32_t media_at_cmd_handle(media_args *arg);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif