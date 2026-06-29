/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 * Description: Provide ux recovery api \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-10-25， Create file. \n
 */

#ifndef UX_API_PBAP_H
#define UX_API_PBAP_H
#include <stdio.h>
#include "ux_api_bt.h"
#if defined(__cplusplus) && __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief PBAP文件类型枚举
 */
typedef enum {
    UX_API_BT_PBAP_FILE_PHONE_BOOK = 0, /**< 电话簿 */
    UX_API_BT_PBAP_FILE_CALL_LOG,       /**< 通话记录 */
    UX_API_BT_PBAP_FILE_MAX,            /**< 最大值 */
} ux_api_bt_pbap_file_type_t;

/**
 * @brief 打开PBAP文件
 * @param vcf_file out 文件句柄指针
 * @param file_type in 文件类型，参考@ref ux_api_bt_pbap_file_type_t
 * @return errcode_t 成功返回ERRCODE_SUCC，失败返回具体错误码
 */
errcode_t ux_api_bt_pbap_open_file(FILE **vcf_file, ux_api_bt_pbap_file_type_t file_type);

/**
 * @brief 关闭PBAP文件
 * @param vcf_file in 文件句柄
 * @return errcode_t 成功返回ERRCODE_SUCC，失败返回具体错误码
 */
errcode_t ux_api_bt_pbap_close_file(FILE *vcf_file);

/**
 * @brief 从远端设备拉取PBAP文件
 * @param addr in 远端蓝牙设备地址
 * @param file_type in 文件类型，参考@ref ux_api_bt_pbap_file_type_t
 * @return errcode_t 成功返回ERRCODE_SUCC，失败返回具体错误码
 */
errcode_t ux_api_bt_pbap_pull_remote_file(const ux_api_bt_addr_t *addr, ux_api_bt_pbap_file_type_t file_type);

#if defined(__cplusplus) && __cplusplus
}
#endif /* __cplusplus */
#endif