/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: diag usr adapt
 * This file should be changed only infrequently and with great care.
 */

#ifndef DIAG_ADAPT_LAYER_H
#define DIAG_ADAPT_LAYER_H

#include <stdbool.h>
#include "time.h"
#include "errcode.h"
#include "diag_config.h"
#include "diag_common.h"
#include "diag_channel.h"
#include "dfx_adapt_layer.h"
#include "soc_diag_util.h"
#include "log_printf.h"

#define DIAG_HSO_MAX_MUX_PKT_SIZE 512
#define DIAG_HSO_VER_DEFAULT 0
#define DIAG_HSO_THIS_VER 0x11

#define USER_CMD_LIST_NUM 10 /* Maximum number of non-system command lists */

static inline uint32_t diag_adapt_get_msg_time(void)
{
    return dfx_get_cur_second();
}

static inline uint64_t diag_adapt_get_msg_time_ms(void)
{
    uint64_t time_ms = 0;

    osal_timeval cur_time = { 0 };
    osal_gettimeofday(&cur_time);

    time_ms = ((uint64_t)cur_time.tv_sec) * 1000 ;  /* 1 second = 1000 milliseconds */
    time_ms += (uint64_t)(cur_time.tv_usec / 1000); /* 1 milliseconds = 1000 Microseconds */
    return time_ms;
}

static inline uint32_t diag_divu10(uint32_t n)
{
    uint32_t q, r;
    q = (n >> 1) + (n >> 2);  /* shift 2 */
    q = q + (q >> 4);         /* shift 4 */
    q = q + (q >> 8);         /* shift 8 */
    q = q + (q >> 16);        /* shift 16 */
    q = q >> 3;               /* shift 3 */
    r = n - (((q << 2) + q) << 1); /* shift 2 */
    return q + (r > 9);       /* 9 */
}

static inline uint32_t diag_adapt_get_msg_local_time(void)
{
    return diag_divu10((uint32_t)((uapi_systick_get_ms() + get_log_basetime_ms())));
}

static inline errcode_t diag_adapt_sync_tx_sem_wait(void)
{
    return ERRCODE_FAIL;
}

static inline void diag_adapt_sync_tx_sem_signal(void)
{
    return;
}

static inline uint32_t diag_adapt_get_default_dst(void)
{
    return DIAG_FRAME_FID_UART;
}

static inline diag_channel_id_t diag_adapt_addr_2_channel_id(diag_frame_fid_t addr)
{
    if (addr == DIAG_FRAME_FID_UART) {
        return DIAG_CHANNEL_ID_0;
    } else if (addr == DIAG_FRAME_FID_BT_SPP) {
        return DIAG_CHANNEL_ID_1;
    } else if (addr == DIAG_FRAME_FID_BLE_GATT) {
        return DIAG_CHANNEL_ID_2;
    }
    return DIAG_CHANNEL_ID_INVALID;
}

static inline diag_frame_fid_t diag_adapt_channel_id_2_addr(diag_channel_id_t id)
{
    if (id == DIAG_CHANNEL_ID_0) {
        return DIAG_FRAME_FID_UART;
    } else if (id == DIAG_CHANNEL_ID_1) {
        return DIAG_FRAME_FID_BT_SPP;
    } else if (id == DIAG_CHANNEL_ID_2) {
        return DIAG_FRAME_FID_BLE_GATT;
    }
    return DIAG_FRAME_FID_MAX;
}

static inline diag_frame_fid_t diag_adapt_get_local_addr(void)
{
    return DIAG_FRAME_FID_LOCAL;
}

static inline bool diag_adapt_is_in_unblocking_context(void)
{
    return false;
}

#endif /* DIAG_ADAPT_LAYER_H */
