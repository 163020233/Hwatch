/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved. \n
 *
 * Description: Provides at cmd register func \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-07-20， Create file. \n
 */
#ifndef OHOSFWK_AT_PROCESS_H
#define OHOSFWK_AT_PROCESS_H

#include <stdint.h>
#include <stdio.h>
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */
uint32_t OhosfwkATCmdAmExitJsApp(uint8_t argc, char *argv[]);
uint32_t OhosfwkATCmdBmInstall(uint8_t argc, char *argv[]);
uint32_t OhosfwkATCmdBmUninstall(uint8_t argc, char *argv[]);
uint32_t OhosfwkATCmdBmSet(uint8_t argc, char *argv[]);
uint32_t OhosfwkATCmdAmChangeNativeSlice(uint8_t argc, char *argv[]);
uint32_t OhosfwkATCmdAmStartJsApp(uint8_t argc, char *argv[]);
uint32_t OhosfwkATCmdDfxJsMemoryInfo(uint8_t argc, char *argv[]);
uint32_t OhosfwkATCmdChangeLanguage(uint8_t argc, char *argv[]);
uint32_t OhosfwkATCmdBmGetAppInfo(uint8_t argc, char *argv[]);
uint32_t OhosfwkATCmdBmGetAppList(uint8_t argc, char *argv[]);
uint32_t OhosfwkATCmdAmDfx(uint8_t argc, char *argv[]);
uint32_t OhosfwkATStartNotify(uint8_t argc, char *argv[]);
#ifdef SUPPORT_AT_PERMISSION
uint32_t OhosfwkATCmdQueryAppPermission(uint8_t argc, char *argv[]);
uint32_t OhosfwkATCmdCheckAppPermission(uint8_t argc, char *argv[]);
uint32_t OhosfwkATCmdGrantAppPermission(uint8_t argc, char *argv[]);
#endif
uint32_t OhosfwkATCmdTimezoneSet(uint8_t argc, char *argv[]);
uint32_t OhosfwkATCmdTimezoneGet(uint8_t argc, char *argv[]);
#ifdef CONFIG_NATIVEAPP_TEST
uint32_t OhosfwkATSwitchSlicePage(uint8_t argc, char *argv[]);
uint32_t OhosATMonkeyControl(uint8_t argc, char *argv[]);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
