/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides net sample.
 * Author: HiSilicon
 * History:
 * 2022-07-20， Create file.
 */
#ifndef NET_AT_PROCESS_H
#define NET_AT_PROCESS_H

#include <stdint.h>
#include <stdio.h>
#include "errcode.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

uint32_t NetATPing(uint8_t argc, char *argv[]);
uint32_t NetATIperf(uint8_t argc, char *argv[]);
#ifdef AT_COMMAND_HTTP
uint32_t NetATHttp(uint8_t argc, char *argv[]);
uint32_t NetATHttpDownLoad(uint8_t argc, char *argv[]);
#endif
#ifdef NET_AT_TEST_NETSTACK
#ifdef AT_COMMAND_HTTP
uint32_t NetATHttpRequest(uint8_t argc, char *argv[]);
uint32_t NetATHttpSetAnythingHeader(uint8_t argc, char *argv[]);
uint32_t NetATHttpSetJsonHeader(uint8_t argc, char *argv[]);
uint32_t NetATHttpSetTextHeader(uint8_t argc, char *argv[]);
uint32_t NetATHttpPost(uint8_t argc, char *argv[]);
uint32_t NetATHttpPut(uint8_t argc, char *argv[]);
uint32_t NetATHttpDelete(uint8_t argc, char *argv[]);
#endif
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
