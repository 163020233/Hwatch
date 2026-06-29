/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: os platform config files
 */

#ifndef _ASM_PLATFORM_H
#define _ASM_PLATFORM_H

#include "interrupt_config.h"
#include "los_printf.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* 内核心跳时钟主频:50M - FPGA */
#define OS_SYS_CLOCK_CONFIG           50000000
#define OS_SYS_CLOCK                  OS_SYS_CLOCK_CONFIG
#define ENOERR           0
#define ENOSUPP          201
int get_errno(void);
extern void dbguart_printf(const char *fmt, ...);

#define PRINT_DEBUG(fmt, ...)
#define PRINT_INFO(fmt, ...)
#define PRINT_WARN(fmt, ...)

#undef PRINT_ERR
#define PRINT_ERR(fmt, ...)  do {           \
    (dbguart_printf("[ERR] "), dbguart_printf(fmt, ##__VA_ARGS__)); \
} while (0)

#undef PRINTK
#define PRINTK(fmt, ...)            dbguart_printf(fmt, ##__VA_ARGS__)
#define PRINT_EXC(fmt, ...)         dbguart_printf(fmt, ##__VA_ARGS__)
#define LOS_PANIC(fmt, ...)         dbguart_printf(fmt, ##__VA_ARGS__)

extern VOID BoardConfig(VOID);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASM_PLATFORM_H */
