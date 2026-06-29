/**
 * @file zlix_static_huffman_inner.h
 * @copyright Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * @brief zlix 提供静态huffman操作接口
 * @details zlix 静态huffman操作头文件，包括建立静态huffman树
 * @author Anonym
 * @date 2024-03-06
 * @version v0.1.0
 * *******************************************************************************************
 * @par 修改日志：
 * <table>
 * <tr><th>Date        <th>Version  <th>Author       <th>Description
 * <tr><td>2024-03-06  <td>0.1.0    <td>Anonym       <td>Initial Version
 * </table>
 * *******************************************************************************************
 */
#ifndef ZLIX_STATIC_HUFFMAN_INNER_H
#define ZLIX_STATIC_HUFFMAN_INNER_H

#include "zlix_huffman_inner.h"

extern void ZlixHuffmanStaticTreeInit(ZlixDeflateNodeInfo *ltree, ZlixDeflateNodeInfo *dtree);

#endif
