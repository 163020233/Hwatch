/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: diag device info
 * This file should be changed only infrequently and with great care.
 */
#ifndef DIAG_DEVICE_INFO_H
#define DIAG_DEVICE_INFO_H

#include "errcode.h"
    
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define DIAG_DEVICE_VERSION_MAX_LEN 64
#define DIAG_DEVICE_SN_MAX_LEN 16
#define DIAG_DEVICE_HARDWARE_MODEL_MAX_LEN 16
#define DIAG_DEVICE_CHIP_MODEL_MAX_LEN 16
#define DIAG_DEVICE_TYPE_MAX_LEN 16

typedef struct {
    char version[DIAG_DEVICE_VERSION_MAX_LEN];                  /* 软件版本 */
    char sn[DIAG_DEVICE_SN_MAX_LEN];                            /* 设备标识 */
    char hardware_model[DIAG_DEVICE_CHIP_MODEL_MAX_LEN];        /* 产品型号 */
    char chip_model[DIAG_DEVICE_HARDWARE_MODEL_MAX_LEN];        /* 芯片型号 */
    char type[DIAG_DEVICE_TYPE_MAX_LEN];                        /* 软件类型(可选，主要用于多mcu的情况) */
} diag_device_model_t;

typedef struct {
    diag_device_model_t device_model;                           /* 设备型号信息 */
    uint16_t reboot_cause;                                      /* 重启原因 */
    uint16_t reboot_count;                                      /* 总重启次数 */
    uint16_t exception_reboot_count;                            /* 异常重启次数 */
} diag_device_info_t;

// 清除设备信息
void diag_device_info_clear(void);

// 设置设备信息
errcode_t diag_device_info_set(const diag_device_model_t* device_model);

// 获取设备信息
errcode_t diag_device_info_get(diag_device_info_t* device_info);

// 上报设备信息至diag存储
errcode_t diag_device_info_report(void);

// 上报上一次crash信息至存储
errcode_t diag_crash_info_report(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif