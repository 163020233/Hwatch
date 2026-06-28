/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: ux api at data structure.\n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-06-06, Create file. \n
 */
#ifndef UX_API_AT_H
#define UX_API_AT_H

#include <stdint.h>
#include "at.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @brief 自定义AT命令参数结构体,用于存储AT命令的参数映射和参数值
 */
typedef struct {
    uint32_t para_map;              /**< 参数映射标志位，用于标识哪些参数有效 */
    const char *para1;              /**< 第一个参数 */
    const char *para2;              /**< 第二个参数 */
    const char *para3;              /**< 第三个参数 */
    const char *para4;              /**< 第四个参数 */
    const char *para5;              /**< 第五个参数 */
} ux_api_at_cust_args_t;

/**
 * @brief 自定义AT命令设置处理函数
 * @param [in] args 命令参数
 * @return AT_RET_OK 命令执行成功
 * @return Other 失败，参考 @ref at_ret_t
 */
typedef at_ret_t (*ux_api_at_set_handler)(const ux_api_at_cust_args_t *args);

/**
 * @brief 自定义AT命令查询处理函数
 * @param [in] args 命令参数
 * @return AT_RET_OK 命令执行成功
 * @return Other 失败，参考 @ref at_ret_t
 */
typedef at_ret_t (*ux_api_at_query_handler)(const ux_api_at_cust_args_t *args);

/**
 * @brief 自定义AT命令读取处理函数
 * @return AT_RET_OK 命令执行成功
 * @return Other 失败，参考 @ref at_ret_t
 */
typedef at_ret_t (*ux_api_at_read_handler)(void);

/**
 * 自定义AT命令处理函数结构体
 * 包含设置、查询和读取三种处理函数
 */
typedef struct {
    const char *name;                               /**< 自定义AT命令名 */
    ux_api_at_set_handler cust_set_handler;         /**< 自定义设置处理函数 */
    ux_api_at_query_handler cust_query_handler;     /**< 自定义查询处理函数 */
    ux_api_at_read_handler cust_read_handler;       /**< 自定义读取处理函数 */
} ux_api_at_cust_handler_t;

/**
 * @brief 注册自定义AT命令处理函数
 * @param [in] handlers 自定义AT命令处理函数结构体，包含设置、查询和读取处理函数
 * @return ERRCODE_SUCC 成功；Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_at_register_cust_handler(const ux_api_at_cust_handler_t *handlers);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
