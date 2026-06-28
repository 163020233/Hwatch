/**
 * @file zlix_huffman_inner.h
 * @copyright Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * @brief zlix 哈夫曼树操作内部操作头文件
 * @details zlix 哈夫曼树操作内部操作头文件，包括建立建立human树、统计码长操作
 * @author Anonym
 * @date 2021-10-22
 * @version v0.1.0
 * *******************************************************************************************
 * @par 修改日志：
 * <table>
 * <tr><th>Date        <th>Version  <th>Author       <th>Description
 * <tr><td>2021-10-22  <td>0.1.0    <td>Anonym       <td>Initial Version
 * </table>
 * *******************************************************************************************
 */

#ifndef ZLIX_HUFFMAN_INNER_H
#define ZLIX_HUFFMAN_INNER_H

#include "drb_zlix.h"
#include "zlix_deflate_public_inner.h"

#ifdef __cplusplus
extern "C" {
#endif

/* zlix deflate字符编码区间的个数， 从0到255 */
#define ZLIX_DEFLATE_LITERALS_CNT          256

/* zlix deflate长度编码区间的个数， 从257到285 */
#define ZLIX_DEFLATE_LENGTH_CNT            29

/* zlix deflate距离编码区间个数，从0到29 */
#define ZLIX_DEFLATE_DIST_CNT              30

/* zlix deflate cl序列编码区间个数，从0到18 */
#define ZLIX_DEFLATE_CL_CNT                19

/* zlix deflate字符+长度+结尾符编码区间个数，从0到2 */
#define ZLIX_DEFLATE_L_CNT       (ZLIX_DEFLATE_LITERALS_CNT + 1 + ZLIX_DEFLATE_LENGTH_CNT)

/* zlix deflate 字符/len和距离流最大编码长度 */
#define ZLIX_DEFLATE_LIT_MAX_CODE_LEN       15

/* zlix deflate cl序列最大编码长度 */
#define ZLIX_DEFLATE_CL_MAX_CODE_LEN        7

/* zlix deflate 字符/长度静态树大小，2是预留 */
#define ZLIX_DEFLATE_STATIC_L_HUF_ELEM_CNT  (ZLIX_DEFLATE_L_CNT + 2)

/* zlix deflate heap临时节点的大小 */
#define ZLIX_DEFLATE_HEAP_CNT  (ZLIX_DEFLATE_L_CNT << 1)

/* huffman树的大小，考虑到有虚拟节点，申请2 * n + 1个大小 */
#define ZLIX_DEFLATE_HUFFMAN_TREE_SIZE(cnt) \
    (((2 * (cnt)) + 1) * sizeof(ZlixDeflateNodeInfo))

/* 堆中的节点信息 */
typedef struct {
    uint16_t index;                   /* 指向树中的结点位置 */
    uint16_t freq;                    /* 频率值 */
} ZlixDeflateHuffmanMinHeapNode;

/* huffman建树最小堆信息 */
typedef struct {
    ZlixDeflateHuffmanMinHeapNode heap[ZLIX_DEFLATE_HEAP_CNT];     /* 指向树中节点的位置信息 */
} ZlixDeflateHuffmanMinHeap;

/* 树的基础描述信息 */
typedef struct {
    uint8_t *clArray;                        /* 未经过编码的cl序列的临时保存空间 */

    uint16_t maxFreqNoZeroCode;              /* 频率非0的最大编码值 */
    uint16_t clLen;                          /* cl序列的实际长度 */
    uint16_t extraBaseVal;                   /* 需要映射转换关系的起始值，dis映射区间[0-29],
                                                len的映射区间[257-285] */
    uint16_t maxRangeCode;                   /* 树上最大的区间编码值，dist区间是29，literal/len是285 */
    uint8_t maxRangeCodeLen;                 /* huffman树上区间编码值对应的最大编码长度，dynLTree和dynDTree
                                                为15，dynBtree为7 */
} ZlixDeflateBasicDescInfo;

/* huffman编码管理信息 */
typedef struct {
    ZlixDeflateBasicDescInfo dynLBasicDescLInfo;                    /* literal/len树基础描述信息 */
    ZlixDeflateBasicDescInfo dynLBasicDescDInfo;                    /* distance树基础描述信息 */
    ZlixDeflateBasicDescInfo dynLBasicDescCInfo;                    /* cl树基础描述信息 */

    ZlixDeflateNodeInfo *dynLTree;                                  /* 字符、长度的动态huffman树 */
    ZlixDeflateNodeInfo *dynDTree;                                  /* 距离的动态huffman树 */
    ZlixDeflateNodeInfo *dynClTree;                                 /* 序列的动态huffman树 */

#ifndef ZLIX_LOW_COST_BUT_SLOWER
    ZlixDeflateNodeInfo *staticLTree;                               /* 字符、长度的静态huffman树 */
    ZlixDeflateNodeInfo *staticDTree;                               /* 距离的静态huffman树 */
#endif
    ZlixDeflateHuffmanMinHeap *minHeapInfo;                         /* huffman建树所用最小堆信息 */

    uint32_t staticLen;                                             /* 采用静态huffman编码所需的编码长度，单位为比特 */
    uint32_t dynLen;                                                /* 采用动态huffman编码所需的编码长度，单位为比特 */
#ifdef ZLIX_LOW_COST_BUT_SLOWER
    uint8_t useOutbuf;                                              /* 判断是否使用了outbuf临时存储最小堆，以判断后续是否需要释放申请空间 */
#endif
} ZlixDeflateHuffmanCodeInfo;

/* huffman初始化接口 */
ZlixDeflateHuffmanCodeInfo* ZlixHuffmanEntroyCodeInfoInit(ZlixMemFuncs zlixMemOper);

/* huffman去初始化接口 */
void ZlixHuffmanEntroyCodeInfoFini(ZlixDeflateHuffmanCodeInfo **huffmanCodeInfo, ZlixMemFuncs zlixMemOper);

/* 动态huffman编码接口 */
void ZlixHuffmanDynamicCode(ZlixDeflateHuffmanCodeInfo *entroyCode, ZlixDeflateContext *context);

#ifdef __cplusplus
}
#endif

#endif
