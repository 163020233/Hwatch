/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: SEC TRNG2
 * Author:
 * Create: 2020-01-20
 */

#include "sec_trng_master.h"
#include "trng.h"
#include "ipc.h"
#include "debug_print.h"
#include "ipc_porting.h"
#define SEC_TRNG2_DATA_HALF_LENGTH     (TRNG_DATA_REGION_LENGTH / 2)
#define SEC_TRNG2_DATA_HALF_START      (TRNG_DATA_REGION_START + SEC_TRNG2_DATA_HALF_LENGTH)

static void sec_trng2_random_buffer_init(sec_trng_rand_buf_updata_t updata)
{
    uint32_t trng_data_start;
    uint32_t buffer_length;
    switch (updata) {
        case SEC_TRNG2_RAND_BUF_UPDATA_ALL:
            trng_data_start = TRNG_DATA_REGION_START;
            buffer_length = TRNG_DATA_REGION_LENGTH;
            break;
        case SEC_TRNG2_RAND_BUF_UPDATA_UP_SIDE:
            trng_data_start = TRNG_DATA_REGION_START;
            buffer_length = SEC_TRNG2_DATA_HALF_LENGTH;
            break;
        case SEC_TRNG2_RAND_BUF_UPDATA_DOWN_SIDE:
            trng_data_start = SEC_TRNG2_DATA_HALF_START;
            buffer_length = SEC_TRNG2_DATA_HALF_LENGTH;
            break;
        default:
            return;
    }
    uint32_t rets = uapi_drv_cipher_trng_get_random_bytes((uint8_t *)((uintptr_t)trng_data_start), buffer_length);
    if (rets != ERRCODE_SUCC) {
        PRINT("gener random failed, just use old bytes ! \r\n");
    }
}
static void sec_trng2_updata_handler(uint8_t *payload_addr, uint32_t payload_len)
{
    (void)payload_len;
    sec_updata_event_trng_t *updata_trng_signal = (sec_updata_event_trng_t *)payload_addr;
    switch (updata_trng_signal->updata) {
        case SEC_TRNG2_RAND_BUF_UPDATA_ALL:
            sec_trng2_random_buffer_init(SEC_TRNG2_RAND_BUF_UPDATA_ALL);
            break;
        case SEC_TRNG2_RAND_BUF_UPDATA_UP_SIDE:
            sec_trng2_random_buffer_init(SEC_TRNG2_RAND_BUF_UPDATA_UP_SIDE);
            break;
        case SEC_TRNG2_RAND_BUF_UPDATA_DOWN_SIDE:
            sec_trng2_random_buffer_init(SEC_TRNG2_RAND_BUF_UPDATA_DOWN_SIDE);
            break;
        default:
            return sec_trng2_random_buffer_init(SEC_TRNG2_RAND_BUF_UPDATA_ALL);
    }
}

void uapi_sec_trng_random_buffer_master_init(void)
{
    ipc_rx_handler_info_t handler_info;
    handler_info.msg_id = IPC_MSG_UPDATA_TRNG;
    handler_info.cb = sec_trng2_updata_handler;
    (void)uapi_ipc_register_rx_handler(&handler_info);
}