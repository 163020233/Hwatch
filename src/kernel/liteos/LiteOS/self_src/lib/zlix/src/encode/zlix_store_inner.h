/**
 * @file zlix_store_inner.h
 * @copyright Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * @brief zlix_store 基础操作内部头文件
 * @details zlix_store 基础操作内部头文件，包括获取store块起始地址、store块输出操作等
 * @author Anonym
 * @date 2024-04-12
 * @version v0.1.0
 * *******************************************************************************************
 * @par 修改日志：
 * <table>
 * <tr><th>Date        <th>Version  <th>Author       <th>Description
 * <tr><td>2024-04-12  <td>0.1.0    <td>Anonym       <td>Initial Version
 * </table>
 * *******************************************************************************************
 */

#ifndef ZLIX_STORE_INNER_H
#define ZLIX_STORE_INNER_H

#include "drb_zlix.h"
#include "zlix_deflate_public_inner.h"

#ifdef __cplusplus
extern "C" {
#endif

const uint8_t *ZlixDeflateGetStoreSrc(ZlixStream *stream, ZLIX_BOOL dyn,
    uint64_t dynLen, uint64_t staticLen, uint32_t totalAvailIn);

void ZlixDeflateBlockStore(ZlixStream *stream, const uint8_t *src, uint32_t last);

/* 2个字节所能存储的最大数值，用来表示一个store块所能接受的最大长度 */
#define ZLIX_DEFLATE_STORE_MAX_LEN 65535
void ZlixDeflateEasyStore(ZlixStream *stream);

#ifdef __cplusplus
}
#endif

#endif
