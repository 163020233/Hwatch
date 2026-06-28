/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: header file for overlay_load module
 * Author: audio
 */

#ifndef __CORE_OVERLAY_H__
#define __CORE_OVERLAY_H__

#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include "td_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define OVERLAY_NAME_SIZE  16

typedef struct {
    void *(*malloc)(size_t, const char *);
    void *(*calloc)(size_t, const char *);
    void *(*realloc)(void *, size_t, const char *);
    void (*free)(void *);

    int (*printf)(const char *);
    int (*vsprintf)(char *str, const char *format, va_list arg);

    /* hso log */
    void (*log_event_print0)(td_u32 log_header, td_u32 press_para);
    void (*log_event_print1)(td_u32 log_header, td_u32 press_para, td_u32 p1);
    void (*log_event_print2)(td_u32 log_header, td_u32 press_para, td_u32 p1, td_u32 p2);
    void (*log_event_print3)(td_u32 log_header, td_u32 press_para, td_u32 p1, td_u32 p2, td_u32 p3);
    void (*log_event_print4)(td_u32 log_header, td_u32 press_para, td_u32 p1, td_u32 p2, td_u32 p3, td_u32 p4);

    /* securec functions */
    int (*memset_s)(void *dest, size_t dest_max, int c, size_t count);
    int (*memcpy_s)(void *dest, size_t dest_max, const void *src, size_t count);
    int (*memmove_s)(void *dest, size_t dest_max, const void *src, size_t count);
    int (*vsnprintf_s)(char *str, size_t dest_max, size_t size, const char *format, va_list arg);

    int (*memcmp)(const void *, const void *, size_t);
    void *(*memcpy)(void *, const void *, size_t);
    void *(*memmove)(void *, const void *, size_t);
    void *(*memset)(void *, int, size_t);
    void (*qsort)(void *base, size_t nitems, size_t size, int (*compar)(const void *, const void*));
    void *(*memchr)(const void *buf, int ch, size_t count);

    /* math functions */
    long (*lrintf)(float);
    long (*lrint)(double);
    double (*floor)(double);
    double (*log)(double);
    double (*log10)(double);
    double (*pow)(double, double);
    double (*sqrt)(double);
    int (*rand)(void);
} overlay_func;

typedef struct {
    /* overlay的库名，用于记录log */
    td_char name[OVERLAY_NAME_SIZE];

    /*
     * overlay初始化标志
     * overlay第一次初始化时会清空bss段，初始化heap
     */
    td_bool init;

    /* 将main的系统函数传递到overlay */
    td_void (* set_sys_func)(const overlay_func *func);

    /* 在overlay中寻找目标组建，verify为校验函数 */
    td_void *(* get_component)(td_u32 component_id, td_u32 sub_id,
        td_bool (* verify)(td_void *entry, td_u32 sub_id));
} overlay_contex;

typedef overlay_contex *(*overlay_start_call)(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __CORE_OVERLAY_H__ */
