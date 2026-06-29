/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2021-2021. All rights reserved.
* Description: models define
* Author: Media Software Group
* Create: 2021-12-07
*/


#ifndef MOD_DEFS_H
#define MOD_DEFS_H

#include <sys/param.h>
#include <time.h>
#include <limits.h>
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define SEC_TO_USEC 1000000
#define SEC_UNIT 1000

/** middleware module id */
typedef enum {
    APPID_DEMUXER = 0x28,     /** < demuxer */
    APPID_BUTT = 0xFF
} AppModIdE;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* __MOD_DEFS_H__ */
