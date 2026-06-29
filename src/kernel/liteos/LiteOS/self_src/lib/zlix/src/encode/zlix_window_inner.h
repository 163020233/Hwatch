/**
 * @file zlix_window_inner.h
 * @copyright Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * @brief zlix 内部滑窗相关操作头文件
 * @details zlix_simd 内部滑窗相关操作头头文件
 * @author Anonym
 * @date 2024-08-31
 * @version v0.1.0
 * *******************************************************************************************
 * @par 修改日志：
 * <table>
 * <tr><th>Date        <th>Version  <th>Author       <th>Description
 * <tr><td>2024-07-09  <td>0.1.0    <td>Anonym       <td>Initial Version
 * </table>
 * *******************************************************************************************
 */

#ifndef ZLIX_WINDOW_INNER_H
#define ZLIX_WINDOW_INNER_H

#include "zlix_public_inner.h"
#include "zlix_deflate_inner.h"
#include "zlix_utils_inner.h"
#include "sre_cpu_external.h"

ZLIX_STATIC ZLIX_ALWAYS_INLINE void ZlixShiftHashTableNoSimd(const uint16_t winSize, ZlixDeflateLz77DictInfo *dict)
{
    uint32_t i;
    uint32_t hashSize;
    uint16_t *hashKey;
    uint16_t hashVal;
    /* level 1下不解决冲突 */
    if (dict->compLevel > 1) {
        /* 更新Hash字典数据下标以及平移Chain，保持下标与窗口对应. */
        for (i = 0; i < winSize; i++) {
            dict->hashChain[i] = (dict->hashChain[i] <= winSize) ? 0 : (dict->hashChain[i] - winSize);
        }
    }

    hashSize = 1 << dict->hashBits;
    hashKey = &dict->hash[hashSize];
    /* hash表右侧存放内容移动到左侧时，hash表各下标(hashKey)所存放的地址内容也需要移动到左侧，即减winSize(32K) */
    do {
        hashVal = *--hashKey;
        *hashKey = (hashVal >= winSize) ? (hashVal - winSize) : (0);
        --hashSize;
    } while (hashSize != 0);
}

/* 当滑窗右侧32K达到最后缓冲区时，将右侧滑窗内容移动到左侧，并将hash表和hash链进行更新 */
ZLIX_STATIC ZLIX_ALWAYS_INLINE void ZlixDeflateWindowFill(
    ZlixStream *stream, ZlixDeflateLz77DictInfo *dict, ZLIX_BOOL needCrc, ZLIX_BOOL needAdler)
{
    const uint16_t winSize = 1 << dict->winbits;
    uint32_t dWinSize = winSize << 1;
    uint32_t shiftPos = (winSize << 1) - ZLIX_DEFLATE_LZ77_MIN_LOOKAHEAD;
    /* 在输入长度和滑窗大小之间选择一个较小值作为本次拷贝上限 */
    uint16_t maxCopy = (stream->availIn < winSize) ? (uint16_t)stream->availIn : winSize;
    void *start;
    uint16_t leftLen;
    if (maxCopy == 0) {
        /* 没有数据可拷贝时返回 */
        return;
    }
    /* 当待处理数据首字符超过该地址时，说明此时首字符与滑动窗口中间的地址已经超过了最大偏移距离
       所以此时左侧滑窗内容已经没有作用了，可以将右侧滑窗内容移动到左侧，另外，如果首字符位置为0，
       则说明右侧滑窗已经完全消耗，位置溢出到0,也需要进行窗口复制 */
    if ((dict->startPos >= shiftPos) || (dict->startPos == 0)) {
        /* 右面的窗口的数据覆盖左面，使已经处理过的最近的数据填满左侧窗口 */
        if (memcpy_s(dict->winUnion.winBuff, dWinSize, dict->winUnion.winBuff + winSize, winSize) != EOK) {
            OS_GOTO_SYS_ERROR();
            return;
        }
        ZlixShiftHashTableNoSimd(winSize, dict);
        if (dict->bPrevMatched == ZLIX_TRUE) {
            if (dict->prevMatchStart >= winSize) {
                dict->prevMatchStart -= winSize;
            } else {
                dict->bPrevMatched = ZLIX_FALSE;
            }
        }
        dict->startPos -= winSize;
    }
    /* 新的输入流拷贝至右侧窗口，更新输入流信息和字典信息 */
    start = (void *)((uintptr_t)dict->winUnion.winBuff + dict->startPos + dict->lookahead);
    leftLen = (uint16_t)((uintptr_t)dict->winUnion.winBuff + (dWinSize) - (uintptr_t)start);
    maxCopy = (leftLen < maxCopy) ? leftLen : maxCopy;
    if (memcpy_s(start, dWinSize, stream->nextIn, maxCopy) != EOK) {
        OS_GOTO_SYS_ERROR();
        return;
    }
    if (needCrc) {
        (void)ZlixUtilsCrc32Cal(stream->nextIn, maxCopy, (uint32_t *)(&stream->adler));
    } else if (needAdler) {
        stream->adler = ZlixUtilsAdler32Cal(stream->adler, stream->nextIn, maxCopy);
    }
    stream->availIn -= maxCopy;
    stream->totalIn += maxCopy;
    stream->nextIn = (const uint8_t *)((uintptr_t)stream->nextIn + maxCopy);
    dict->lookahead += maxCopy;
}

#endif
