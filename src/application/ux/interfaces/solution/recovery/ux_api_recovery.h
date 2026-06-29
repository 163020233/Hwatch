/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 * Description: Provide ux recovery api \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-04， Create file. \n
 */
 
#ifndef UX_API_RECOVERY_H
#define UX_API_RECOVERY_H
 
#include "stdint.h"
#include "stdbool.h"
#include "errcode.h"
#include "nv.h"
 
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief 恢复出厂设置参数结构体
 */
typedef struct {
    uint16_t bitmap;     /**< 恢复区域位图 */
    bool need_format;    /**< 是否需要格式化 */
} ux_api_recovery_param_t;

/**
 * @brief 设置设备开始恢复出厂设置
 * @param [in] region_mode 指定需要恢复出厂设置的区域，用16个bool值分别指代16个区域是否要恢厂
 * @param [in] need_format 是否需要格式化
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_recovery_partial_process(bool region_mode[KEY_ID_REGION_MAX_NUM], bool need_format);
 
#ifdef __cplusplus
}
#endif /* __cplusplus */
 
#endif