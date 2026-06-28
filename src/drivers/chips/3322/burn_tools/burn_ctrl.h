/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Provides burn uart port template \n
 * History: \n
 * 2022-06-06， Create file. \n
 */

#ifndef BURN_CTRL_H
#define BURN_CTRL_H

#include "burn_types.h"
#include "debug_print.h"
#include "serial_port.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*burn_log_fn)(void *user, const char *msg);

typedef struct burn_serial_cfg {
    const char *port;
    u32 baud;
    int databits;
    int stopbits;
    int parity;
    int flowctrl;
} burn_serial_cfg_t;

typedef struct burn_context {
    burn_io_t *io;
    u8 rx_buf[BURN_RX_BUF_SIZE];
    u32 rx_len;
    u32 start_flag;
    u32 ymodem_packet_size;
    int erase_mode;
    int only_erase_all;
    u32 timeout_handshake_ms;
    u32 timeout_ack_ms;
    u32 timeout_c_ms;
    burn_log_fn log_fn;
    void *log_user;
} burn_context_t;

void burn_context_init(burn_context_t *ctx, burn_io_t *io);
void burn_context_set_log(burn_context_t *ctx, burn_log_fn fn, void *user);

int burn_open(burn_context_t *ctx, const burn_serial_cfg_t *cfg);
void burn_close(burn_context_t *ctx);
int burn_handshake(burn_context_t *ctx, const burn_serial_cfg_t *cfg);

int burn_execute(burn_context_t *ctx, const bin_info_t *bins, u32 bin_count, u32 radio);
void reset_2131_cat1_module(void);
void dump_burn_uart_buffer(void);
#ifdef __cplusplus
}
#endif

#endif /* BURN_CTRL_H */
