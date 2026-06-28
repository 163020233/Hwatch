/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: inner efuse file for SELiteOS.
 */

#ifndef __CHIPS_PORTING_H__
#define __CHIPS_PORTING_H__
#include <stdint.h>
#include <stdbool.h>
#include "errcode.h"
#include "tesvec_dispatch.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef SECURITY_ENHANCEMENT
#define volatile_sec volatile
#else
#define volatile_sec
#endif

void tee_pmp_init(void);

void tee_rapc_init(void);

void tee_tzasc_init(void);

void tee_driver_and_service_init(void);

void tee_ready_to_ree(void);

errcode_t tee_efuse_get_lifecycle(uint8_t *lifecycle);

errcode_t tee_efuse_get_chip_version(uint8_t *buffer, uint16_t length);

errcode_t tee_efuse_get_die_id(uint8_t *buffer, uint16_t length);

const tesvec_ioctl_cmd_t *get_chips_porting_dispatch_func_list(void);

uint32_t get_chips_porting_max_nr(void);

typedef int32_t (*verify_func_t)(void);

void safety_function_init(void);

void safety_function_register(const verify_func_t *func_list, uint32_t func_num);

int32_t safety_function_verify(void);

void safety_function_set_verify_flag(bool verify_flag);

bool safety_function_get_verify_flag(void);

void dsensor_init(void);

void dsensor_check_attacked_and_reset(void);

#ifndef CONFIG_NON_TEE_SUPPORT
int memcmp_sec(const void *cs, const void *ct, unsigned int count);

int memset_sec(void *dest, unsigned int dest_max, unsigned char c, unsigned int count);

int memcpy_sec(void *dest, unsigned int dest_max, const void *src, unsigned int count);

void tee_set_sec_storage_fapc(bool enable);

#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CHIPS_PORTING_H__ */
