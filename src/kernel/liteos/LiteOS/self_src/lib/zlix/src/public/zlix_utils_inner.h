/**
 * @file zlix_utils_inner.h
 * @copyright Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * @brief zlix utils 基础操作内部头文件
 * @details zlix utils 基础操作内部头文件，包括基础的hash操作、字符串比较操作等等
 * @author Anonym
 * @date 2023-03-23
 * @version v0.1.0
 * *******************************************************************************************
 * @par 修改日志：
 * <table>
 * <tr><th>Date        <th>Version  <th>Author       <th>Description
 * <tr><td>2023-04-12  <td>0.1.0    <td>Anonym       <td>Initial Version
 * </table>
 * *******************************************************************************************
 */

#ifndef ZLIX_UTILS_INNER_H
#define ZLIX_UTILS_INNER_H

#include "drb_zlix.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t ZlixUtilsCrc32Cal(const uint8_t *buf, size_t length, uint32_t *crcVal);
uint32_t ZlixUtilsAdler32Cal(uint32_t adlerIn, const uint8_t *buf, size_t lenIn);
uint32_t ZlixUtilsAdler32(const uint8_t *buf, size_t len);

#ifdef __cplusplus
}
#endif

#endif
