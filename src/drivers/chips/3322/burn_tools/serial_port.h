/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Provides burn uart port template \n
 * History: \n
 * 2022-06-06， Create file. \n
 */
#ifndef SERIAL_PORT_H
#define SERIAL_PORT_H

#include "burn_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct burn_io {
    void *user;
    int (*open)(u32 baud);
    int (*configure)(u32 baud);
    int (*close)(void *user);
    int (*write)(void *user, const u8 *data, u32 len);
    int (*read)(void *user, u8 *data, u32 maxlen, u32 timeout_ms);
    u64 (*now_ms)(void);
    void (*sleep_ms)(u32 ms);
} burn_io_t;

typedef struct burn_host_info {
    void *handle;
    char port_name[BURN_MAX_PATH];
} burn_host_info_t;

void serial_register_uart_func(burn_host_info_t *info, burn_io_t *io);

#ifdef __cplusplus
}
#endif

#endif /* SERIAL_PORT_H */
