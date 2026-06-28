/**
 * @file zlix_static_huffman.c
 * @copyright Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * @brief \n
 * 提供静态huffman操作接口
 * @author Anonym
 * @date 2024-03-06
 * @version v0.1.0
 * *******************************************************************************************
 * @par 修改日志：
 * <table>
 * <tr><th>Date        <th>Version  <th>Author    <th>Description
 * <tr><td>2024-03-06  <td>0.1.0    <td>Anonym    <td>静态huffman树操作接口
 * </table>
 *
 * *******************************************************************************************
 */
#include "zlix_static_huffman_inner.h"

void ZlixHuffmanStaticTreeInit(ZlixDeflateNodeInfo *ltree, ZlixDeflateNodeInfo *dtree)
{
    uint32_t i;
    uint32_t j;
    uint16_t clCode = 0;
    uint16_t firstCode[ZLIX_STATIC_LIT_MAX_LEN + 1];
    uint16_t dCode = 0;
    uint16_t bl_cnt[ZLIX_STATIC_LIT_MAX_LEN + 1] = {0};

    for (i = 0; i < ZLIX_STATIC_CODELEN_TYPE; i++) {
        bl_cnt[g_staticCodeLen[i]] += g_staticCodeRange[i][1] - g_staticCodeRange[i][0] + 1;
    }

    for (i = 7; i <= ZLIX_STATIC_LIT_MAX_LEN; i++) {  /* 7为静态长度/距离树的最小码长 */
        clCode = ((uint16_t)(clCode + bl_cnt[i - 1])) << 1;
        firstCode[i] = (uint16_t)clCode;
    }

    for (j = 0; j < ZLIX_STATIC_CODELEN_TYPE; j++) {
        for (i = g_staticCodeRange[j][0]; i <= g_staticCodeRange[j][1]; i++) {
            ltree[i].len.rangeCode = g_staticCodeLen[j];
            ltree[i].code.nodeCode = (uint16_t)ZlixInflateReverse(firstCode[g_staticCodeLen[j]], g_staticCodeLen[j]);
            firstCode[g_staticCodeLen[j]]++;
        }
    }

    /* 0-29的编码长度都为5 */
    for (i = 0; i < ZLIX_DISTANCE_CODE_RANGE; i++) {
        dtree[i].len.rangeCode = ZLIX_STATIC_DIST_MAX_LEN;
        dtree[i].code.nodeCode = (uint16_t)ZlixInflateReverse(dCode, ZLIX_STATIC_DIST_MAX_LEN);
        dCode++;
    }

    return;
}
