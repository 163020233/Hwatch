
/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: chips porting API default implementation for SELiteOS.
 */
#include "los_typedef.h"
#include "los_base.h"
#include "chips_porting.h"

WEAK errcode_t tee_efuse_get_die_id(uint8_t *buffer, uint16_t length)
{
#ifndef CONFIG_NON_TEE_SUPPORT
    return memset_s(buffer, length, 0, length);
#else
    (void)buffer;
    (void)length;
    return ERRCODE_SUCC;
#endif
}

WEAK errcode_t tee_efuse_get_lifecycle(uint8_t *lifecycle)
{
    *lifecycle = 0;

    return ERRCODE_SUCC;
}

WEAK const tesvec_ioctl_cmd_t *get_chips_porting_dispatch_func_list(void)
{
    return NULL;
}

WEAK uint32_t get_chips_porting_max_nr(void)
{
    return 0;
}

static const verify_func_t *g_sf_verify_func_list = NULL;
static uint32_t g_sf_verify_func_num = 0;
static bool g_sf_verify_flag;
static bool g_sf_verify_enable;

WEAK void safety_function_init(void)
{
}

void safety_function_register(const verify_func_t *func_list, uint32_t func_num)
{
    g_sf_verify_func_list = func_list;
    g_sf_verify_func_num = func_num;
}

int32_t safety_function_verify(void)
{
    uint32_t i;

    g_sf_verify_enable = true;
    for (i = 0; i < g_sf_verify_func_num; i++) {
        if (g_sf_verify_func_list[i]() != 0) {
            g_sf_verify_flag = false;
            return -1;
        }
    }
    g_sf_verify_flag = true;
    return 0;
}

void safety_function_set_verify_flag(bool verify_flag)
{
    g_sf_verify_flag = verify_flag;
}

bool safety_function_get_verify_flag(void)
{
    if (g_sf_verify_enable == true) {
        return g_sf_verify_flag;
    }
    return true;
}

WEAK void dsensor_init(void)
{
    return;
}

WEAK void dsensor_check_attacked_and_reset(void)
{
    return;
}

#ifndef CONFIG_NON_TEE_SUPPORT
#ifdef SECURITY_ENHANCEMENT
#include "security_enhancement.h"

int memcmp_sec(const void *cs, const void *ct, unsigned int count)
{
    volatile_sec int ret = LOS_FAILURE;

    tee_random_delay();
    ret = memcmp(cs, ct, count);
    if (ret != 0) {
        return LOS_FAILURE;
    }
    tee_random_delay();
    ret = memcmp(cs, ct, count);
    if (ret != 0) {
        return LOS_FAILURE;
    }
    return LOS_SUCCESS;
}

int memset_sec(void *dest, unsigned int dest_max, unsigned char c, unsigned int count)
{
    volatile_sec int ret = LOS_FAILURE;

    tee_random_delay();
    ret = memset_s(dest, dest_max, c, count);
    if (ret != EOK) {
        return LOS_FAILURE;
    }
    tee_random_delay();
    ret = memset_s(dest, dest_max, c, count);
    if (ret != EOK) {
        return LOS_FAILURE;
    }
    return LOS_SUCCESS;
}

int memcpy_sec(void *dest, unsigned int dest_max, const void *src, unsigned int count)
{
    volatile_sec int ret = LOS_FAILURE;

    tee_random_delay();
    ret = memcpy_s(dest, dest_max, src, count);
    if (ret != EOK) {
        return LOS_FAILURE;
    }
    tee_random_delay();
    ret = memcmp(dest, src, count);
    if (ret != 0) {
        return LOS_FAILURE;
    }
    return LOS_SUCCESS;
}

#else
int memcmp_sec(const void *cs, const void *ct, unsigned int count)
{
    return memcmp(cs, ct, count);
}

int memset_sec(void *dest, unsigned int dest_max, unsigned char c, unsigned int count)
{
    return memset_s(dest, dest_max, c, count);
}

int memcpy_sec(void *dest, unsigned int dest_max, const void *src, unsigned int count)
{
    return memcpy_s(dest, dest_max, src, count);
}
#endif

WEAK void tee_set_sec_storage_fapc(bool enable)
{
    (void)enable;
    return;
}

#endif
