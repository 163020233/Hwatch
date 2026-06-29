/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: SEC TRNG
 * Author:
 * Create: 2020-01-20
 */

#ifndef SEC_TRNG_MASTER_H
#define SEC_TRNG_MASTER_H

#include "sec_trng_shared.h"
/**
 * @brief  Register callback function for refreshing random numbers.
 */
void uapi_sec_trng_random_buffer_master_init(void);

#endif
