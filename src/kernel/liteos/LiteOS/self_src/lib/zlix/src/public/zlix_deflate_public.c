/**
 * @file zlix_deflate_public.c
 * @copyright Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * @brief \n
 * 提供zlix deflate算法公共数据和操作
 * @author Anonym
 * @date 2021-12-13
 * @version v0.1.0
 * *******************************************************************************************
 * @par 修改日志：
 * <table>
 * <tr><th>Date        <th>Version  <th>Author    <th>Description
 * <tr><td>2023-04-19  <td>0.1.0    <td>Anonym    <td>提供zlix deflate算法公共数据和操作
 * </table>
 *
 * *******************************************************************************************
 */
#include "zlix_deflate_public_inner.h"

/* 长度映射区间的extra bit长度，考虑公式化表达 */
const uint8_t g_lExtraBits[ZLIX_MATCHLEN_CODE_RANGE] = {0, 0, 0, 0, 0, 0, 0,
    0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0};

/* 距离映射区间的extra bit长度，考虑公式化表达 */
const uint8_t g_dExtraBits[ZLIX_DISTANCE_CODE_RANGE] = {0, 0, 0, 0, 1, 1, 2,
    2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13};

/* 游程编码序列 */
const uint8_t g_cclQueue[ZLIX_TREE_CODE_RANGE] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};

/* 游程编码 16 17 18的 映射区间的extra bit长度，考虑公式化表达 */
const uint8_t g_bExtraBits[ZLIX_TREE_CODE_RANGE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 7};

/* 游程编码 16 17 18的 映射区间的基值，考虑公式化表达 */
const uint8_t g_bExtraLenBase[ZLIX_TREE_CODE_RANGE] = {0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 11};

/* 长度的区间节点基值 */
const uint16_t g_baseLength[ZLIX_MATCHLEN_CODE_RANGE] = {3, 4, 5, 6, 7, 8, 9,
    10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258};

/* 距离的区间节点基值 */
const uint16_t g_baseDist[ZLIX_DISTANCE_CODE_RANGE] = {1, 2, 3, 4, 5, 7, 9, 13, 17,
    25, 33, 49, 65, 97, 129, 193, 257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577};

/* 将不同编码长度的区间进行分段，对应区间的编码长度对应 g_staticCodeLen， 2代表起止两个端 */
const uint16_t g_staticCodeRange[ZLIX_STATIC_CODELEN_TYPE][2] = {{256, 279}, /* 256到279的编码长度为7 */
                                                                 {0, 143},   /* 0到143的编码长度为8 */
                                                                 {280, 287}, /* 280到287的编码长度为8 */
                                                                 {144, 255}}; /* 144到255的编码长度为9 */

/* 4个区间的编码长度，分别为 7 8 8 9 */
const uint8_t g_staticCodeLen[ZLIX_STATIC_CODELEN_TYPE] = {7, 8, 8, 9};

/* 4个不同编码长度的分隔区间，按照区间的值大小对区间进行排列 */
const ZLIX_FIXED_SEGMENT g_fixedSeg[ZLIX_STATIC_CODELEN_TYPE] = {{24, 167},   /* 0 - 143的起止下标为24和 167 */
                                                                 {176, 287},  /* 144-255的起止下标为176和287 */
                                                                 {0, 23},     /* 256-279的起止下标为0和23 */
                                                                 {168, 175}}; /* 280-287的起止下标为168和175 */
