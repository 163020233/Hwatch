/**
 * @file zlix_huffman.c
 * @copyright Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * @brief \n
 * 提供huffman操作接口，如huffman建树、编码、生成bit流等
 * @author Anonym
 * @date 2021-10-22
 * @version v0.1.0
 * *******************************************************************************************
 * @par 修改日志：
 * <table>
 * <tr><th>Date        <th>Version  <th>Author    <th>Description
 * <tr><td>2021-10-22  <td>0.1.0    <td>Anonym    <td>huffman操作接口
 * </table>
 *
 * *******************************************************************************************
 */
#include <stdlib.h>
#include "zlix_public_inner.h"
#include "zlix_huffman_inner.h"
#include "zlix_static_huffman_inner.h"
#include "zlix_deflate_inner.h"

#define ZLIX_DEFLATE_HUFFMAN_WRITE_CL_LITERAL(info, clLen, codeval, cclTree) do {                \
    (info)->clArray[(clLen)++] = (uint8_t)(codeval);                                             \
    (cclTree)[codeval].code.nodeFreq++;                                                          \
} while (0)

#define BUCKET_SIZE 16

#ifndef ZLIX_LOW_COST_BUT_SLOWER
ZlixDeflateNodeInfo g_staticLTree[ZLIX_DEFLATE_STATIC_L_HUF_ELEM_CNT];
ZlixDeflateNodeInfo g_staticDTree[ZLIX_DEFLATE_DIST_CNT];
#endif

ZLIX_STATIC ZLIX_ALWAYS_INLINE void InitBlCnt(uint16_t *blCnt, size_t len, uint32_t maxCode, ZlixDeflateNodeInfo *tree)
{
    uint32_t i;
    (void)len;
    for (i = 0; i <= maxCode; i++) {
        blCnt[tree[i].len.rangeCode]++;
    }
}

ZLIX_STATIC void ZlixHuffmanCodeCal(ZlixDeflateNodeInfo *tree, uint32_t maxCodeLen, uint32_t maxCode,
    ZLIX_BOOL isLitTree, ZlixDeflateHuffmanCodeInfo *entroyCode)
{
    uint16_t firstCode[ZLIX_BL_CNT_LEN] = {0}; /* 函数归一，所以CL和CCL临时数组变量长度一律用了15 + 1 */
    uint16_t indexCount[ZLIX_BL_CNT_LEN] = {0};
    uint16_t bl_cnt[ZLIX_BL_CNT_LEN] = {0};
    uint32_t i;
    uint16_t codeLen;
    uint16_t clCode = 0;
#ifndef ZLIX_LOW_COST_BUT_SLOWER
    ZlixDeflateNodeInfo *staticTree;
#endif

    InitBlCnt(bl_cnt, sizeof(bl_cnt), maxCode, tree);

    bl_cnt[0] = 0;
    for (i = 1; i <= maxCodeLen; i++) {
        clCode = (uint16_t)(clCode + bl_cnt[i - 1]) << 1U;
        firstCode[i] = clCode;
    }

#ifndef ZLIX_LOW_COST_BUT_SLOWER
    staticTree = (isLitTree == ZLIX_TRUE) ? entroyCode->staticLTree : entroyCode->staticDTree;
    if (staticTree != NULL) {
        for (i = 0; i <= maxCode; i++) {
            entroyCode->staticLen += (uint32_t)tree[i].code.nodeFreq * (uint32_t)staticTree[i].len.rangeCode;
        }
    }
#else
    if (isLitTree == ZLIX_TRUE) {
        for (i = 0; i <= maxCode; i++) {
            if (i <= 143) { /* 143是g_staticCodeRange中第2个编码区间 */
                entroyCode->staticLen += (uint32_t)tree[i].code.nodeFreq * 8; /* 8是第2个区间的编码长度 */
            } else if (i >= 144 && i <= 255) { /* 144和255是g_staticCodeRange中第4个编码区间 */
                entroyCode->staticLen += (uint32_t)tree[i].code.nodeFreq * 9; /* 9是第4个区间的编码长度 */
            } else if (i >= 256 && i <= 279) {  /* 256和279是g_staticCodeRange中第1个编码区间 */
                entroyCode->staticLen += (uint32_t)tree[i].code.nodeFreq * 7; /* 7是第1个区间的编码长度 */
            } else {  /* g_staticCodeRange中第3个编码区间 */
                entroyCode->staticLen += (uint32_t)tree[i].code.nodeFreq * 8; /* 8是第3个区间的编码长度 */
            }
        }
    } else {
        for (i = 0; i <= maxCode; i++) {
            entroyCode->staticLen += (uint32_t)tree[i].code.nodeFreq * 5; /* 5为静态树存储距离时的编码长度 */
        }
    }
#endif

    for (i = 0; i <= maxCode; i++) {
        codeLen = tree[i].len.rangeCode;
        if (codeLen != 0) {
            entroyCode->dynLen += (uint32_t)tree[i].code.nodeFreq * (uint32_t)codeLen;
            tree[i].code.nodeCode = (uint16_t)ZlixInflateReverse(firstCode[codeLen], codeLen);
            firstCode[codeLen]++;
        }
        indexCount[codeLen]++;
    }
}

ZLIX_STATIC void ZlixHuffmanConbineOverLenNode(uint16_t limit, ZlixDeflateNodeInfo *tree,
    ZlixDeflateHuffmanMinHeapNode *heap, uint32_t heapNum, uint32_t maxCode)
{
    uint32_t i;
    uint32_t j;
    uint16_t index1;
    uint16_t index2;
    uint16_t index3;
    uint16_t len;

    index2 = 0; /* 消除编译告警 */
    for (i = 0; i < heapNum; i++) {
        index1 = (uint16_t)maxCode - heap[i].index;
        if (tree[index1].len.rangeCode <= limit) {
            return;
        } else {
            len = tree[index1].len.rangeCode;
        }
        for (j = i + 1; j < heapNum; j++) {
            /* 找到limit层之上的第一个实际节点 */
            index2 = (uint16_t)maxCode - heap[j].index;
            if (tree[index2].len.rangeCode < limit) {
                break;
            }
        }
        /* 两个节点合并 */
        tree[index2].len.rangeCode += 1;
        tree[index1].len.rangeCode = tree[index2].len.rangeCode;

        /* 将同层最左边的节点往上移 */
        for (j = i + 1; j < heapNum; j++) {
            index3 = (uint16_t)maxCode - heap[j].index;
            if (tree[index3].len.rangeCode != len) {
                break;
            }
        }
        index3 = (uint16_t)maxCode - heap[j - 1].index;
        tree[index3].len.rangeCode--;
    }
}

ZLIX_STATIC void ZlixHuffmanSingleSymbol(uint32_t number, ZlixDeflateNodeInfo *tree, uint32_t maxCode,
    ZlixDeflateBasicDescInfo *info)
{
    uint32_t i;

    info->maxFreqNoZeroCode = (uint16_t)maxCode;

    if (number == 1) {
        tree[maxCode].len.rangeCode = 1;
        tree[maxCode].code.nodeCode = 0;
        return;
    }
    for (i = 0; i <= maxCode; i++) {
        if (tree[i].code.nodeFreq != 0) {
            tree[i].len.rangeCode = 1;
            tree[i].code.nodeCode = 0;
            tree[maxCode].len.rangeCode = 1;
            tree[maxCode].code.nodeCode = 1;
            return;
        }
    }
}

/* 返回值：生成的虚拟节点的个数 */
ZLIX_STATIC uint16_t ZlixHuffmanMinHeapTreeBuild(ZlixDeflateHuffmanMinHeapNode *heap, uint32_t number,
    ZlixDeflateNodeInfo *tree, uint16_t maxCode)
{
    int32_t i;
    int32_t m;
    int32_t n;
    uint16_t freqTotal;
    uint16_t left;
    uint16_t right;
    uint16_t temp;
    uint16_t newNode;
    uint32_t heapNum = number;

    for (i = 1; ; i++) {
        /* 此种方法不能保证相同频率下符号值小的最终码长比符号值大的要小 */
        freqTotal = heap[heapNum - 1].freq + heap[heapNum - 2].freq; /* 合并倒数第1和倒数第2个节点 */
        right = heap[heapNum - 1].index;
        left = heap[heapNum - 2].index;   /* 左节点频率不低于右节点，所以是倒数第2个节点 */
        newNode = (uint16_t)(maxCode + i);
        tree[left].len.nodeParent = newNode;
        tree[right].len.nodeParent = newNode;
        heapNum--;
        if (heapNum <= 1) {
            break;
        }

        for (n = (int32_t)(heapNum - 2); n >= 0; n--) { /* 此时虚拟节点在最后，下标heapNum - 2即为倒数第二个节点 */
            if (heap[n].freq >= freqTotal) {
                break;
            }
        }

        for (m = (int32_t)(heapNum - 2); m >= n + 1; m--) { /* 从倒数第2个节点依次往后移 */
            heap[m + 1].freq = heap[m].freq;
            heap[m + 1].index = heap[m].index;
        }

        /* n >= 0 的退出条件保证不会发生翻转 */
        temp = (uint16_t)(n + 1);
        heap[temp].freq = freqTotal;
        heap[temp].index = newNode;
    }

    return (uint16_t)i;
}

ZLIX_STATIC ZLIX_ALWAYS_INLINE void ZlixHuffmanSortHeap(ZlixDeflateHuffmanMinHeapNode *heap, uint32_t heapNum,
    ZlixDeflateHuffmanMinHeapNode *heapOut)
{
    ZlixDeflateHuffmanMinHeapNode swapNodeInfo;
    uint32_t rank;
    uint32_t pos;
    int32_t index;
    uint16_t symbolCount;
    uint32_t lowRankBucket[BUCKET_SIZE] = {0};
    uint32_t bucketBase[BUCKET_SIZE + 1] = {0};
    uint32_t bucketPos[BUCKET_SIZE + 1] = {0};

    // 把每个符号的频率根据bit位分到桶中
    for (index = 0; index < (int32_t)heapNum; ++index) {
        lowRankBucket[HighBit(heap[index].freq + 1)]++;
    }

    // 计算每个桶的基位置和当前分配的位置
    for (index = BUCKET_SIZE - 1; index >= 0; --index) {
        bucketBase[index] = lowRankBucket[index] + bucketBase[index + 1];
        bucketPos[index] = bucketBase[index];
    }

    for (index = 0; index < (int32_t)heapNum; ++index) {
        symbolCount = heap[index].freq;
        rank = (uint32_t)HighBit(symbolCount + 1) + 1;
        pos = bucketPos[rank]++;
        heapOut[pos].freq = symbolCount;
        heapOut[pos].index = heap[index].index;
        // 此处需要向前找到频率合适的位置，因为后面的频率后放入，但是可能比前面的频率大
        while ((pos > 0) && (pos > bucketBase[rank]) && (heapOut[pos - 1].freq < heapOut[pos].freq)) {
            swapNodeInfo = heapOut[pos];
            heapOut[pos] = heapOut[pos - 1];
            heapOut[pos - 1] = swapNodeInfo;
            pos--;
        }
    }
}

/* 动态Huffman树的构建 */
void ZlixHuffmanDynamicTreeBuild(ZlixDeflateNodeInfo *tree, ZlixDeflateBasicDescInfo *info,
    ZLIX_BOOL isLitTree, ZlixDeflateHuffmanCodeInfo *entroyCode)
{
    int32_t i;
    uint32_t maxCode = 0;
    uint32_t heapNum = 0;
    uint32_t maxCodeLen = 1;
    uint16_t extraNode;
    uint16_t parent;
    ZlixDeflateHuffmanMinHeapNode *heap = entroyCode->minHeapInfo->heap;
    ZlixDeflateHuffmanMinHeapNode *heapHalf = heap + ZLIX_DEFLATE_L_CNT;

    for (i = 0; i <= info->maxRangeCode; i++) {
        if (tree[i].code.nodeFreq == 0) {
            tree[i].len.rangeCode = 0;
        } else {
            heapHalf[heapNum].freq = tree[i].code.nodeFreq;
            heapHalf[heapNum].index = (uint16_t)i;
            maxCode = (uint32_t)i;
            heapNum++;
        }
    }

    if (heapNum == 0) {
        return;
    } else if (heapNum <= 2) {     /* 对只有1个或者2个符号的特殊处理 */
        ZlixHuffmanSingleSymbol(heapNum, tree, maxCode, info);
        return;
    }

    ZlixHuffmanSortHeap(heapHalf, heapNum, heap);

    extraNode = ZlixHuffmanMinHeapTreeBuild(heap, heapNum, tree, (uint16_t)maxCode);
    tree[maxCode + extraNode].len.rangeCode = 0;

    /* 统计节点的层数，后生成的节点在上层，所以从后往前统计 */
    heapNum = 0;
    for (i = (int32_t)(maxCode + extraNode - 1); i >= 0; i--) {
        parent = tree[i].len.nodeParent;
        if (parent > 0) {
            tree[i].len.rangeCode = tree[parent].len.rangeCode + 1;
            if ((uint32_t)i <= maxCode) {
                maxCodeLen = (tree[i].len.rangeCode > maxCodeLen) ? tree[i].len.rangeCode : maxCodeLen;
                heapHalf[heapNum].freq = tree[i].len.rangeCode;  /* 此时freq存的是码长 */
                heapHalf[heapNum++].index = (uint16_t)(maxCode - (uint32_t)i); /* 此时index存的是maxCode - 符号值 */
            }
        }
    }

    /* 处理层数大于最大码长的节点 */
    if (maxCodeLen > info->maxRangeCodeLen) {
        ZlixHuffmanSortHeap(heapHalf, heapNum, heap);
        ZlixHuffmanConbineOverLenNode(info->maxRangeCodeLen, tree, heap, heapNum, maxCode);
        maxCodeLen = info->maxRangeCodeLen;
    }

    /* 统计每层真实节点的个数，计算编码 */
    ZlixHuffmanCodeCal(tree, maxCodeLen, maxCode, isLitTree, entroyCode);

    info->maxFreqNoZeroCode = (uint16_t)maxCode;
}

/* 从tree当前节点开始重复字符的个数，包括tree[0]自身 */
static inline uint32_t ZlixHuffmanRleGetRepeatTime(const ZlixDeflateNodeInfo *tree, uint16_t code,
    uint32_t remain, uint32_t max)
{
    uint32_t i;
    uint32_t loopMax = (max < remain) ? max : remain;

    for (i = 0; i < loopMax; i++) {
        if (tree[i].len.rangeCode != code) {
            break;
        }
    }

    return i;
}

ZLIX_STATIC void ZlixHuffmanDynamicCclArrayWrite(const ZlixDeflateNodeInfo *cclTree,
    ZlixDeflateBasicDescInfo *info, uint32_t *dynLen)
{
    uint32_t i;
    uint32_t maxCode = info->maxFreqNoZeroCode;
    uint32_t cclLen = 0;

    (void)memset_s(info->clArray, sizeof(uint8_t) * ZLIX_TREE_CODE_RANGE, 0, sizeof(uint8_t) * ZLIX_TREE_CODE_RANGE);

    for (i = 0; i <= maxCode; i++) {
        info->clArray[i] = (uint8_t)cclTree[i].len.rangeCode;
    }

    /* 计算ccl的长度 */
    for (i = ZLIX_TREE_CODE_RANGE - 1; i > 0; i--) {
        if (info->clArray[g_cclQueue[i]] != 0) {
            cclLen = i + 1;
            break;
        }
    }
    info->clLen = (uint16_t)cclLen;
    *dynLen += (uint32_t)(cclLen * 3);    /* ccl范围为0-7，占3比特 */
}

/* 生成未经过CCL编码的原始CL序列 */
ZLIX_STATIC void ZlixHuffmanDynamicClArrayWrite(const ZlixDeflateNodeInfo *tree,
    ZlixDeflateBasicDescInfo *info, ZlixDeflateNodeInfo *cclTree)
{
    uint32_t i;
    uint32_t repeat;
    uint32_t maxCode = info->maxFreqNoZeroCode;
    uint32_t maxRangeSize = info->maxRangeCode + 1;
    uint16_t code;
    uint16_t clLen = 0;
    uint16_t prev = 0;

    (void)memset_s(info->clArray, sizeof(uint8_t) * maxRangeSize, 0, sizeof(uint8_t) * maxRangeSize);

    /* 一边进行游程编码的转换并写入输出流一边统计CCL符号的频率 */
    for (i = 0; i <= maxCode; i++) {
        code = tree[i].len.rangeCode;
        if (code == 0) {
            repeat = ZlixHuffmanRleGetRepeatTime(&tree[i], code, maxCode - i + 1, 138); /* 符号0可压缩的最大长度为138 */
            if (repeat < 3) {        /* 重复次数少于3不压缩 */
                ZLIX_DEFLATE_HUFFMAN_WRITE_CL_LITERAL(info, clLen, code, cclTree);
            } else if (repeat < 11) {    /* 小于11即重复次数处于3 - 10 */
                ZLIX_DEFLATE_HUFFMAN_WRITE_CL_LITERAL(info, clLen, 17, cclTree);   /* 该区间的重复标识为17 */
                info->clArray[clLen++] = (uint8_t)(repeat - 3);   /* 减去exrta的base 3 */
                i += repeat - 1;
            } else {    /* 重复次数处于11 - 138 */
                ZLIX_DEFLATE_HUFFMAN_WRITE_CL_LITERAL(info, clLen, 18, cclTree);   /* 该区间的重复标识为18 */
                info->clArray[clLen++] = (uint8_t)(repeat - 11);  /* 减去exrta的base 11 */
                i += repeat - 1;
            }
            prev = 0;
            continue;
        }
        if (code != prev) {
            repeat = ZlixHuffmanRleGetRepeatTime(&tree[i + 1], code, maxCode - i, 6); /* 其他符号可压缩的最大长度为6 */
            ZLIX_DEFLATE_HUFFMAN_WRITE_CL_LITERAL(info, clLen, code, cclTree);
        } else {
            repeat = ZlixHuffmanRleGetRepeatTime(&tree[i], code, maxCode - i + 1, 6); /* 其他符号可压缩的最大长度为6 */
        }
        if (repeat >= 3) {   /* 重复次数少于3不压缩 */
            ZLIX_DEFLATE_HUFFMAN_WRITE_CL_LITERAL(info, clLen, 16, cclTree);   /* 该区间的重复标识为16 */
            info->clArray[clLen++] = (uint8_t)(repeat - 3);   /* 减去exrta的base 3 */
            if (code != prev) {
                i += repeat;
            } else {
                i += repeat - 1;
            }
            prev = code;
        } else {
            if (code == prev) {
                ZLIX_DEFLATE_HUFFMAN_WRITE_CL_LITERAL(info, clLen, code, cclTree);
            }
            prev = 0;
        }
    }

    info->clLen = clLen;
}

ZLIX_STATIC int32_t ZlixHuffmanMinheapInfoInit(ZlixDeflateHuffmanCodeInfo *huffmanCodeInfo, ZlixDeflateContext *context,
    ZlixMemFuncs zlixMemOper)
{
    (void)context;
#ifdef ZLIX_LOW_COST_BUT_SLOWER
    if (context->bufSize - context->pending >= sizeof(ZlixDeflateHuffmanMinHeap)) {
        huffmanCodeInfo->useOutbuf = 1;
        huffmanCodeInfo->minHeapInfo = (ZlixDeflateHuffmanMinHeap *)(context->outBuf + context->pending);
        return ZLIX_OK;
    }
    huffmanCodeInfo->useOutbuf = 0;
#endif
    huffmanCodeInfo->minHeapInfo = (ZlixDeflateHuffmanMinHeap *)ZLIX_LABEL_MALLOC(zlixMemOper,
        ZLIX_DEFLATE_HUFFMAN_SORT_LABEL, sizeof(ZlixDeflateHuffmanMinHeap));
    if (huffmanCodeInfo->minHeapInfo == NULL) {
        return ZLIX_MEM_OPER_FAILURE;
    }
    (void)memset_s(huffmanCodeInfo->minHeapInfo, sizeof(ZlixDeflateHuffmanMinHeap),
        0, sizeof(ZlixDeflateHuffmanMinHeap));

    return ZLIX_OK;
}

void ZlixHuffmanDynamicCode(ZlixDeflateHuffmanCodeInfo *entroyCode, ZlixDeflateContext *context)
{
    (void)context;

    /* 因为最小堆占用的内存可能是临时借用输出缓存，所以需要在每次计算huffman时再判断outBuf空间是否足够 */
#ifdef ZLIX_LOW_COST_BUT_SLOWER
    ZlixHuffmanMinheapInfoInit(entroyCode, context, context->zlixMemOper);
#endif
    entroyCode->dynLTree[ZLIX_LITERALS_CODE_RANGE].code.nodeFreq = 1;
    /* Literals和Length的动态霍夫曼树构建并转换成游程编码 */
    ZlixHuffmanDynamicTreeBuild(entroyCode->dynLTree, &(entroyCode->dynLBasicDescLInfo),
        ZLIX_TRUE, entroyCode);
    ZlixHuffmanDynamicClArrayWrite(entroyCode->dynLTree, &(entroyCode->dynLBasicDescLInfo), entroyCode->dynClTree);

    /* Distance的动态霍夫曼树构建并转换成游程编码 */
    ZlixHuffmanDynamicTreeBuild(entroyCode->dynDTree, &(entroyCode->dynLBasicDescDInfo),
        ZLIX_FALSE, entroyCode);
    ZlixHuffmanDynamicClArrayWrite(entroyCode->dynDTree, &(entroyCode->dynLBasicDescDInfo), entroyCode->dynClTree);

    /* Cl序列的动态霍夫曼树构建并转换成游程编码 */
    ZlixHuffmanDynamicTreeBuild(entroyCode->dynClTree, &(entroyCode->dynLBasicDescCInfo), ZLIX_FALSE, entroyCode);
    ZlixHuffmanDynamicCclArrayWrite(entroyCode->dynClTree, &(entroyCode->dynLBasicDescCInfo), &entroyCode->dynLen);

#ifdef ZLIX_LOW_COST_BUT_SLOWER
    if (entroyCode->useOutbuf == 0) {
        ZLIX_LABEL_FREE(context->zlixMemOper, ZLIX_DEFLATE_HUFFMAN_SORT_LABEL, entroyCode->minHeapInfo);
    }
#endif
}

ZLIX_STATIC int32_t ZlixHuffmanTreeInfoInit(ZlixDeflateHuffmanCodeInfo *huffmanCodeInfo, ZlixMemFuncs zlixMemOper)
{
    size_t totalAllocSize;
    uint8_t *allocAddr;
#ifndef ZLIX_LOW_COST_BUT_SLOWER
    static uint32_t staticInit = 0;
#endif

    /* 动态和静态huffman内存申请
       Note@:
       1.动态huffman树后续在最小堆排序时，会产生虚拟节点，故需要2*n+1;
       2.静态huffman树，不需要排序，直接使用，只需要n；
       3.静态huffman算法不需要将编码长度进行游程编码进而生成CCL序列，故不需要CL树 */
    totalAllocSize = ZLIX_DEFLATE_HUFFMAN_TREE_SIZE(ZLIX_DEFLATE_L_CNT);    /* 动态literal/len huffman树信息 */
    totalAllocSize += ZLIX_DEFLATE_HUFFMAN_TREE_SIZE(ZLIX_DEFLATE_DIST_CNT);    /* 动态distance huffman树信息 */
    totalAllocSize += ZLIX_DEFLATE_HUFFMAN_TREE_SIZE(ZLIX_DEFLATE_CL_CNT);    /* 动态cl huffman树信息 */

    allocAddr = ZLIX_LABEL_MALLOC(zlixMemOper, ZLIX_DEFLATE_HUFFMAN_TREE_LABEL, totalAllocSize);
    if (allocAddr == NULL) {
        return ZLIX_MEM_OPER_FAILURE;
    }

    (void)memset_s(allocAddr, totalAllocSize, 0, totalAllocSize);

    /* 动态Huffman树节点信息初始化构建 */
    huffmanCodeInfo->dynLTree = (ZlixDeflateNodeInfo *)(uintptr_t)allocAddr;
    allocAddr += ZLIX_DEFLATE_HUFFMAN_TREE_SIZE(ZLIX_DEFLATE_L_CNT);
    huffmanCodeInfo->dynDTree = (ZlixDeflateNodeInfo *)(uintptr_t)allocAddr;
    allocAddr += ZLIX_DEFLATE_HUFFMAN_TREE_SIZE(ZLIX_DEFLATE_DIST_CNT);
    huffmanCodeInfo->dynClTree = (ZlixDeflateNodeInfo *)(uintptr_t)allocAddr;

#ifndef ZLIX_LOW_COST_BUT_SLOWER
    huffmanCodeInfo->staticLTree = g_staticLTree;
    huffmanCodeInfo->staticDTree = g_staticDTree;

    if (staticInit == 0) {
        ZlixHuffmanStaticTreeInit(huffmanCodeInfo->staticLTree, huffmanCodeInfo->staticDTree);
        staticInit = 1;
    }
#endif
    return ZLIX_OK;
}

/* 动态Huffman树描述信息初始化 */
ZLIX_STATIC int32_t ZlixHuffmanBaiscDescTreeInfoInit(ZlixDeflateHuffmanCodeInfo *huffmanCodeInfo,
    ZlixMemFuncs zlixMemOper)
{
    uint32_t totalAllocSize = 0;
    uint8_t *allocAddr;
    ZlixDeflateBasicDescInfo *dynBasicInfo;

    totalAllocSize = ZLIX_DEFLATE_L_CNT;
    totalAllocSize += ZLIX_DEFLATE_DIST_CNT;
    totalAllocSize += ZLIX_DEFLATE_CL_CNT;
    allocAddr = (uint8_t *)ZLIX_LABEL_MALLOC(zlixMemOper, ZLIX_DEFLATE_DYN_BASIC_INFO_LABEL, totalAllocSize);
    if (allocAddr == NULL) {
        return ZLIX_MEM_OPER_FAILURE;
    }

    (void)memset_s(allocAddr, totalAllocSize, 0, totalAllocSize);

    /* 初始化literal/len树基础描述信息 */
    dynBasicInfo = &huffmanCodeInfo->dynLBasicDescLInfo;
    dynBasicInfo->maxRangeCode = ZLIX_DEFLATE_L_CNT - 1;
    dynBasicInfo->maxRangeCodeLen = ZLIX_DEFLATE_LIT_MAX_CODE_LEN;
    dynBasicInfo->maxFreqNoZeroCode = 0;
    dynBasicInfo->extraBaseVal = ZLIX_DEFLATE_LITERALS_CNT + 1;

    dynBasicInfo->clArray = allocAddr;
    allocAddr += ZLIX_DEFLATE_L_CNT;

    /* 初始化distance树基础描述信息 */
    dynBasicInfo = &huffmanCodeInfo->dynLBasicDescDInfo;
    dynBasicInfo->maxRangeCode = ZLIX_DEFLATE_DIST_CNT - 1;
    dynBasicInfo->maxRangeCodeLen = ZLIX_DEFLATE_LIT_MAX_CODE_LEN;
    dynBasicInfo->maxFreqNoZeroCode = 0;
    dynBasicInfo->extraBaseVal = 0;

    dynBasicInfo->clArray = allocAddr;
    allocAddr += ZLIX_DEFLATE_DIST_CNT;

    /* 初始化CL树基础描述信息 */
    dynBasicInfo = &huffmanCodeInfo->dynLBasicDescCInfo;
    dynBasicInfo->maxRangeCode = ZLIX_DEFLATE_CL_CNT - 1;
    dynBasicInfo->maxRangeCodeLen = ZLIX_DEFLATE_CL_MAX_CODE_LEN;
    dynBasicInfo->maxFreqNoZeroCode = 0;
    dynBasicInfo->extraBaseVal = 0;

    dynBasicInfo->clArray = allocAddr;

    return ZLIX_OK;
}

/* deflate算法 huffman熵编码初始化 */
ZlixDeflateHuffmanCodeInfo* ZlixHuffmanEntroyCodeInfoInit(ZlixMemFuncs zlixMemOper)
{
    int32_t ret;
    ZlixDeflateHuffmanCodeInfo *huffmanCodeInfo;

    huffmanCodeInfo = (ZlixDeflateHuffmanCodeInfo *)ZLIX_LABEL_MALLOC(zlixMemOper,
        ZLIX_DEFLATE_HUFFMAN_INFO_LABEL, sizeof(ZlixDeflateHuffmanCodeInfo));
    if (huffmanCodeInfo == NULL) {
        return NULL;
    }
    (void)memset_s(huffmanCodeInfo, sizeof(ZlixDeflateHuffmanCodeInfo), 0, sizeof(ZlixDeflateHuffmanCodeInfo));

    /* 后面所有字段都会初始化，此处不先调用ZlixMemset初始化了 */
    huffmanCodeInfo->staticLen = 0;
    huffmanCodeInfo->dynLen = 0;

    /* 1. 初始化动静态huffman树 */
    ret = ZlixHuffmanTreeInfoInit(huffmanCodeInfo, zlixMemOper);
    if (ret != ZLIX_OK) {
        ZLIX_LABEL_FREE(zlixMemOper, ZLIX_DEFLATE_HUFFMAN_INFO_LABEL, huffmanCodeInfo);
        huffmanCodeInfo = NULL;
        return NULL;
    }

    /* 初始化建树所用最小堆 */
#ifndef ZLIX_LOW_COST_BUT_SLOWER
    ret = ZlixHuffmanMinheapInfoInit(huffmanCodeInfo, NULL, zlixMemOper);
    if (ret != ZLIX_OK) {
        /* 释放huffman树申请的资源 */
        ZLIX_LABEL_FREE(zlixMemOper, ZLIX_DEFLATE_HUFFMAN_TREE_LABEL, huffmanCodeInfo->dynLTree);
        ZLIX_LABEL_FREE(zlixMemOper, ZLIX_DEFLATE_HUFFMAN_INFO_LABEL, huffmanCodeInfo);
        huffmanCodeInfo = NULL;
        return NULL;
    }
#endif

    /* 初始化静态描述基础信息 */
    ret = ZlixHuffmanBaiscDescTreeInfoInit(huffmanCodeInfo, zlixMemOper);
    if (ret != ZLIX_OK) {
        /* 释放huffman树申请的资源 */
        ZLIX_LABEL_FREE(zlixMemOper, ZLIX_DEFLATE_HUFFMAN_TREE_LABEL, huffmanCodeInfo->dynLTree);
#ifndef ZLIX_LOW_COST_BUT_SLOWER
        ZLIX_LABEL_FREE(zlixMemOper, ZLIX_DEFLATE_HUFFMAN_SORT_LABEL, huffmanCodeInfo->minHeapInfo);
#endif
        ZLIX_LABEL_FREE(zlixMemOper, ZLIX_DEFLATE_HUFFMAN_INFO_LABEL, huffmanCodeInfo);
        huffmanCodeInfo = NULL;
        return NULL;
    }

    return huffmanCodeInfo;
}

void ZlixHuffmanEntroyCodeInfoFini(ZlixDeflateHuffmanCodeInfo **huffmanCodeInfo, ZlixMemFuncs zlixMemOper)
{
    ZlixDeflateHuffmanCodeInfo *tmpHufCodeInfo = *huffmanCodeInfo;

    if (tmpHufCodeInfo->dynLTree != NULL) {
        ZLIX_LABEL_FREE(zlixMemOper, ZLIX_DEFLATE_HUFFMAN_TREE_LABEL, tmpHufCodeInfo->dynLTree);
        tmpHufCodeInfo->dynLTree = NULL;
    }

#ifndef ZLIX_LOW_COST_BUT_SLOWER
    if (tmpHufCodeInfo->minHeapInfo != NULL) {
        ZLIX_LABEL_FREE(zlixMemOper, ZLIX_DEFLATE_HUFFMAN_SORT_LABEL, tmpHufCodeInfo->minHeapInfo);
        tmpHufCodeInfo->minHeapInfo = NULL;
    }
#endif

    if (tmpHufCodeInfo->dynLBasicDescLInfo.clArray != NULL) {
        ZLIX_LABEL_FREE(zlixMemOper, ZLIX_DEFLATE_DYN_BASIC_INFO_LABEL,
            tmpHufCodeInfo->dynLBasicDescLInfo.clArray);
        tmpHufCodeInfo->dynLBasicDescLInfo.clArray = NULL;
    }

    ZLIX_LABEL_FREE(zlixMemOper, ZLIX_DEFLATE_HUFFMAN_INFO_LABEL, tmpHufCodeInfo);
    *huffmanCodeInfo = NULL;
    return;
}
