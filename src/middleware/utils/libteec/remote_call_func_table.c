/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * filename: remote_call_func_table.c
 * Author: @CompanyNameTag
 * Create:2025-02-07
 * Description: The remote call function table
 *
 */

#include "debug_print.h"
#include "efuse.h"
#ifdef CONFIG_NV_FEATURE_SUPPORT
#include "nv.h"
#include "nv_storage.h"
#include "nv_config.h"
#endif
#include "securec.h"
#include "sfc.h"
#include "uart.h"

struct remote_func_addr {
    uintptr_t get_die_id_addr;
    uintptr_t nv_read_addr;
    uintptr_t nv_write_addr;
    uintptr_t nv_delete_addr;
    uintptr_t nv_get_key_length;
    uintptr_t efuse_read_buffer_addr;
    uintptr_t reserved_1;
    uintptr_t reserved_2;
    uintptr_t reserved_3;
};

#ifdef CONFIG_NV_FEATURE_SUPPORT
#define NV_EXTRA_STORE_ID 1
static errcode_t dispatch_uapi_nv_write_attr(uint16_t key, const uint8_t *kvalue, uint16_t kvalue_length, uint32_t attr)
{
    nv_key_attr_t copy_attr = {0};
    errcode_t ret = ERRCODE_SUCC;

    ret = (errcode_t)memcpy_s(&copy_attr, sizeof(nv_key_attr_t), &attr, sizeof(uint32_t));
    if (ret != EOK) {
        PRINT("memcpy_s failed! ret = %#x!\n", ret);
        return ret;
    }
#if defined(CONFIG_MIDDLEWARE_SUPPORT_NV_EXTRA) && (CONFIG_MIDDLEWARE_SUPPORT_NV_EXTRA == NV_YES)
    return uapi_nv_extra_write_with_attr(NV_EXTRA_STORE_ID, key, kvalue, kvalue_length, &copy_attr, NULL);
#else
    return uapi_nv_write_with_attr(key, kvalue, kvalue_length, &copy_attr, NULL);
#endif
}

static errcode_t dispatch_nv_get_key_length(uint16_t key, uint16_t *length)
{
    nv_key_attr_t copy_attr = {0};
#if defined(CONFIG_MIDDLEWARE_SUPPORT_NV_EXTRA) && (CONFIG_MIDDLEWARE_SUPPORT_NV_EXTRA == NV_YES)
    return uapi_nv_extra_get_key_attr(NV_EXTRA_STORE_ID, key, length, &copy_attr);
#else
    return uapi_nv_get_key_attr(key, length, &copy_attr);
#endif
}

static errcode_t dispatch_nv_read(uint16_t key, uint16_t kvalue_max_length, uint16_t *kvalue_length, uint8_t *kvalue)
{
#if defined(CONFIG_MIDDLEWARE_SUPPORT_NV_EXTRA) && (CONFIG_MIDDLEWARE_SUPPORT_NV_EXTRA == NV_YES)
    return uapi_nv_extra_read(NV_EXTRA_STORE_ID, key, kvalue_max_length, kvalue_length, kvalue);
#else
    return uapi_nv_read(key, kvalue_max_length, kvalue_length, kvalue);
#endif
}

static errcode_t dispatch_nv_delete_key(uint16_t key, bool completely)
{
#if defined(CONFIG_MIDDLEWARE_SUPPORT_NV_EXTRA) && (CONFIG_MIDDLEWARE_SUPPORT_NV_EXTRA == NV_YES)
    return uapi_nv_extra_delete_key(NV_EXTRA_STORE_ID, key, completely);
#else
    return uapi_nv_delete_key(key, completely);
#endif
}
#else
static errcode_t dispatch_uapi_nv_write_attr(uint16_t key, const uint8_t *kvalue, uint16_t kvalue_length, uint32_t attr)
{
    unused(key);
    unused(kvalue);
    unused(kvalue_length);
    unused(attr);
    return ERRCODE_NOT_SUPPORT;
}

static errcode_t dispatch_nv_get_key_length(uint16_t key, uint16_t *length)
{
    unused(key);
    unused(length);
    return ERRCODE_NOT_SUPPORT;
}

static errcode_t dispatch_nv_read(uint16_t key, uint16_t kvalue_max_length, uint16_t *kvalue_length, uint8_t *kvalue)
{
    unused(key);
    unused(kvalue_max_length);
    unused(kvalue_length);
    unused(kvalue);
    return ERRCODE_NOT_SUPPORT;
}

static errcode_t dispatch_nv_delete_key(uint16_t key, bool completely)
{
    unused(key);
    unused(completely);
    return ERRCODE_NOT_SUPPORT;
}
#endif

const __attribute__((used, section(".remote_func_reserved_rodata"))) struct remote_func_addr g_remote_func_addr = {
    .get_die_id_addr = (uintptr_t)uapi_efuse_get_die_id,
    .nv_write_addr = (uintptr_t)dispatch_uapi_nv_write_attr,
    .nv_read_addr = (uintptr_t)dispatch_nv_read,
    .nv_delete_addr = (uintptr_t)dispatch_nv_delete_key,
    .nv_get_key_length = (uintptr_t)dispatch_nv_get_key_length,
    .efuse_read_buffer_addr = (uintptr_t)uapi_efuse_read_buffer,
    .reserved_1 = 0,
    .reserved_2 = 0,
    .reserved_3 = 0,
};
