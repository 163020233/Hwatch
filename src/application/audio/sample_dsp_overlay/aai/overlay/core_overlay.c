/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: source code file for overlay_load module
 * Author: audio
 */

#include "core_overlay.h"
#include "component.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

td_s32 __errno = 0;

extern td_u8 _bss_start[];
extern td_u8 _bss_end[];

static td_void clear_bss(td_void)
{
    td_u8 *c = TD_NULL;

    for (c = _bss_start; c < _bss_end; c++) {
        *c = 0;
    }
}

#ifdef HEAP_IN_OVERLAY

typedef struct {
    td_void *addr;
    size_t size;
} heap_buffer;

extern td_u8 _end[];
extern td_u8 _heap_sentry[];

static td_u8 *g_heap_offset = TD_NULL;

static td_u32 align_up(td_u32 size)
{
    size += 0x10;
    size -= (size & 0xf);
    return size;
}

static inline td_void heap_init(td_void)
{
    td_u32 off;

    off = (td_u32)(uintptr_t)_end;
    off = align_up(off);
    g_heap_offset = (td_u8 *)(uintptr_t)off;
}
#endif

static const overlay_func *g_overlay_func
    __attribute__((__used__)) \
    __attribute__((__section__(".data"))) = TD_NULL;

static td_void overlay_set_sys_func(const overlay_func *func)
{
    g_overlay_func = func;
}

static overlay_contex g_overlay_ctx = {
    .init = TD_FALSE,
    .set_sys_func = overlay_set_sys_func,

    /* component.c提供的组建查找接口 */
    .get_component = get_component,
};

int memcpy_s(void *dest, size_t dest_max, const void *src, size_t count)
{
    return g_overlay_func->memcpy_s(dest, dest_max, src, count);
}

int memset_s(void *dest, size_t dest_max, int c, size_t count)
{
    return g_overlay_func->memset_s(dest, dest_max, c, count);
}

int memmove_s(void *dest, size_t dest_max, const void *src, size_t count)
{
    return g_overlay_func->memmove_s(dest, dest_max, src, count);
}

int memcmp(const void *p1, const void *p2, size_t size)
{
    return g_overlay_func->memcmp(p1, p2, size);
}

void *memcpy(void *to, const void *from, size_t size)
{
    return g_overlay_func->memcpy(to, from, size);
}

void *memmove(void *to, const void *from, size_t size)
{
    return g_overlay_func->memmove(to, from, size);
}

void *memset(void *dest, int c, size_t size)
{
    return g_overlay_func->memset(dest, c, size);
}

void qsort(void *base, size_t nitems, size_t size, int (*compar)(const void *, const void*))
{
    return g_overlay_func->qsort(base, nitems, size, compar);
}

void *memchr(const void *buf, int ch, size_t count)
{
    return g_overlay_func->memchr(buf, ch, count);
}

static void *overlay_malloc(size_t size)
{
    return g_overlay_func->malloc(size, g_overlay_ctx.name);
}

void *overlay_calloc(size_t size, size_t count)
{
    return g_overlay_func->calloc(size * count, g_overlay_ctx.name);
}

static void *overlay_realloc(void *p, size_t size)
{
    return g_overlay_func->realloc(p, size, g_overlay_ctx.name);
}

#ifdef HEAP_IN_OVERLAY
#define HEAP_BUFFER_NUM_MAX 16
static heap_buffer g_heap_buffer_tab[HEAP_BUFFER_NUM_MAX];

static heap_buffer *heap_find_buf(td_void *addr)
{
    td_u32 i;

    if (addr == TD_NULL) {
        return TD_NULL;
    }

    for (i = 0; i < HEAP_BUFFER_NUM_MAX; i++) {
        if (g_heap_buffer_tab[i].addr == addr) {
            return &(g_heap_buffer_tab[i]);
        }
    }

    return TD_NULL;
}

static td_s32 heap_store_buf(td_void *addr, size_t size)
{
    td_u32 i;

    for (i = 0; i < HEAP_BUFFER_NUM_MAX; i++) {
        if (g_heap_buffer_tab[i].addr == TD_NULL) {
            g_heap_buffer_tab[i].addr = addr;
            g_heap_buffer_tab[i].size = size;
            return 0;
        }
    }

    return -1;
}

td_void *malloc(size_t size)
{
    td_void *addr = TD_NULL;
    td_u8 *safe = _heap_sentry;

    if (g_heap_offset + size >= safe) {
        return overlay_malloc(size);
    }

    addr = (td_void *)g_heap_offset;
    if (heap_store_buf(addr, size)) {
        return TD_NULL;
    }

    g_heap_offset += align_up(size);
    return addr;
}

void *calloc(size_t size, size_t count)
{
    td_void *addr = TD_NULL;

    size *= count;

    addr = malloc(size);
    if (addr == TD_NULL) {
        return TD_NULL;
    }

    g_overlay_func->memset(addr, 0, size);
    return addr;
}

void *realloc(void *p, size_t size)
{
    td_void *addr = TD_NULL;
    heap_buffer *heap = TD_NULL;
    td_u8 *safe = _heap_sentry;

    if (p == TD_NULL) {
        return malloc(size);
    }

    heap = heap_find_buf(p);
    if (heap == TD_NULL) {
        return overlay_realloc(p, size);
    }

    if (heap->size >= size) {
        return heap->addr;
    }

    if (g_heap_offset + size >= safe) {
        return TD_NULL;
    }

    addr = (td_void *)g_heap_offset;
    (td_void)memcpy_s(addr, size, heap->addr, heap->size);

    heap->addr = addr;
    heap->size = size;

    g_heap_offset += align_up(size);
    return addr;
}

void free(void *addr)
{
    heap_buffer *heap = TD_NULL;

    if (addr == TD_NULL) {
        return;
    }

    heap = heap_find_buf(addr);
    if (heap == TD_NULL) {
        g_overlay_func->free(addr);
    } else {
        heap->addr = TD_NULL;
        heap->size = 0;
        heap_init();
    }
}

#else

td_void *malloc(size_t size)
{
    return overlay_malloc(size);
}

void *calloc(size_t size, size_t count)
{
    return overlay_calloc(size, count);
}

void *realloc(void *p, size_t size)
{
    return overlay_realloc(p, size);
}

void free(void *addr)
{
    g_overlay_func->free(addr);
}

#endif

int vsprintf(char *str, const char *format, va_list arg)
{
    return g_overlay_func->vsprintf(str, format, arg);
}

long lrintf(float arg)
{
    return g_overlay_func->lrintf(arg);
}

long lrint(double arg)
{
    return g_overlay_func->lrint(arg);
}

double floor(double arg)
{
    return g_overlay_func->floor(arg);
}

double log(double arg)
{
    return g_overlay_func->log(arg);
}

double log10(double arg)
{
    return g_overlay_func->log10(arg);
}

double pow(double arg1, double arg2)
{
    return g_overlay_func->pow(arg1, arg2);
}

double sqrt(double arg)
{
    return g_overlay_func->sqrt(arg);
}

int rand(void)
{
    return g_overlay_func->rand();
}

int vsnprintf_s(char *str, size_t dest_max, size_t size, const char *format, va_list arg)
{
    return g_overlay_func->vsnprintf_s(str, dest_max, size, format, arg);
}

int printf(const char *fmt, ...)
{
    int ret;
    va_list args;
    char str[128] = {'\0'}; /* maximum 128 characters */

    va_start(args, fmt);
    ret = vsnprintf_s(str, sizeof(str) - 1, sizeof(str) - 1, fmt, args);
    va_end(args);
    if (ret < 0) {
        return ret;
    }

    return g_overlay_func->printf(str);
}

td_void log_event_print0(td_u32 log_header, td_u32 press_para)
{
    return g_overlay_func->log_event_print0(log_header, press_para);
}

td_void log_event_print1(td_u32 log_header, td_u32 press_para, td_u32 p1)
{
    return g_overlay_func->log_event_print1(log_header, press_para, p1);
}

td_void log_event_print2(td_u32 log_header, td_u32 press_para, td_u32 p1, td_u32 p2)
{
    return g_overlay_func->log_event_print2(log_header, press_para, p1, p2);
}

td_void log_event_print3(td_u32 log_header, td_u32 press_para, td_u32 p1, td_u32 p2, td_u32 p3)
{
    return g_overlay_func->log_event_print3(log_header, press_para, p1, p2, p3);
}

td_void log_event_print4(td_u32 log_header, td_u32 press_para, td_u32 p1, td_u32 p2, td_u32 p3, td_u32 p4)
{
    return g_overlay_func->log_event_print4(log_header, press_para, p1, p2, p3, p4);
}

overlay_contex *_start(td_void)
{
    if (g_overlay_ctx.init == TD_FALSE) {
        clear_bss();
#ifdef HEAP_IN_OVERLAY
        heap_init();
#endif
        g_overlay_ctx.init = TD_TRUE;
    }

    return &g_overlay_ctx;
}

static overlay_start_call g_overlay_start_call \
    __attribute__((__used__)) \
    __attribute__((__section__(".start"))) = _start;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
