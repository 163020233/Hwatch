/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: SEC TRNG
 * Author:
 * Create: 2020-01-20
 */

#ifndef SEC_TRNG_SLAVE_H
#define SEC_TRNG_SLAVE_H

#include "sec_trng_shared.h"

/**
 * @brief  Tnitializate random buffer, when the BT core restarts.
 */
void uapi_sec_trng_random_buffer_slave_init(void);

/**
 * @brief  Get random data.
 * @param  trng_buffer        The trng buffer to store random numbers.
 * @param  trng_buffer_length The length of the random number required.
 * @return SEC_TRNG_RET_OK if success, otherwise false.
 */
sec_trng_ret_t uapi_sec_trng_random_get(uint8_t *trng_buffer, uint32_t trng_buffer_length);
#endif
