/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: SEC TRNG
 * Author:
 * Create: 2020-01-20
 */
#ifndef SEC_TRNG_SHARED_H
#define SEC_TRNG_SHARED_H

#include <stdint.h>

typedef enum {
    SEC_TRNG2_RAND_BUF_UPDATA_ALL,
    SEC_TRNG2_RAND_BUF_UPDATA_UP_SIDE,
    SEC_TRNG2_RAND_BUF_UPDATA_DOWN_SIDE,
} sec_trng_rand_buf_updata_t;

typedef enum {
    SEC_TRNG_RET_OK = 0,
    SEC_TRNG_RET_ERROR = 1,
    SEC_TRNG_RET_TOO_LONG = 2,
} sec_trng_ret_t;

/**
 * @brief  A struct to store the signal of updata trng, 0 opdata all side, 1 updata up side, 2 updata down side
 */
typedef struct {
    uint32_t updata;
} sec_updata_event_trng_t;

#endif