/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: trng.cpp
 */

#include "trng.h"
#include <random>

std::random_device g_rd;
std::mt19937 g_gen(g_rd());
std::uniform_int_distribution<> g_dis(0, std::numeric_limits<uint32_t>::max());

errcode_t uapi_drv_cipher_trng_get_random(uint32_t *randnum)
{
    *randnum = g_dis(g_gen);
    return ERRCODE_SUCC;
}

errcode_t uapi_drv_cipher_trng_get_random_bytes(uint8_t *randnum, uint32_t size)
{
    return ERRCODE_SUCC;
}
