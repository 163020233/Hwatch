/**
 * @file zlix_special_strategy.c
 * @copyright Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * @brief \n
 * zlix_rle 特殊编码实现相关逻辑
 * @author Anonym
 * @date 2024-08-31
 * @version v0.1.0
 * *******************************************************************************************
 * @par 修改日志：
 * <table>
 * <tr><th>Date        <th>Version  <th>Author       <th>Description
 * <tr><td>2024-07-09  <td>0.1.0    <td>Anonym       <td>SIMD操作实现
 * </table>
 * *******************************************************************************************
 */
#include "zlix_public_inner.h"
#include "zlix_utils_inner.h"
#include "zlix_window_inner.h"
#include "zlix_deflate_inner.h"
#include "zlix_lz77_strategy_inner.h"

int32_t ZlixDeflateEndCheck(ZlixStream *stream, int32_t flush, ZlixDeflateLz77OutInfo *outInfo,
    uint32_t *isLast, ZLIX_BOOL isZeroBytesInput)
{
    ZlixDeflateLz77DictInfo *dict = ((ZlixDeflateContext *)stream->context)->dictCode;
    /* 判断当前输入和滑窗内数据是否全部输出 */
    if ((stream->availIn == 0) && (dict->lookahead == 0) && (flush == ZLIX_FINISH)) {
        *isLast = 1;
    }

    /* 当flushType为ZLIX_NO_FLUSH时，代表不立刻flush，等到输出缓冲满了以后再进行刷出，以实现流式压缩 */
    /* 当输入为0时，若只输入0字节，则只使用ZLIX_FINISH并且输入总字节为0，进行后续编码；反之则希望更多输入进行lz77匹配对查找 */

    if (((outInfo->count == 0) || (flush == ZLIX_NO_FLUSH && outInfo->count < outInfo->bufSize)) && !isZeroBytesInput) {
        return ZLIX_DEFLATE_BLOCK_NEXT_TIME;
    }

    return ZLIX_DEFLATE_BLOCK_THIS_TIME;
}

/* 不进行LZ77匹配，全部数据视为literal */
int32_t ZlixDeflateLiteralOnly(ZlixStream *stream, int32_t flush, uint32_t *extraLen, uint32_t *isLast)
{
    ZlixDeflateContext *context = stream->context;
    ZlixDeflateLz77DictInfo *dict = context->dictCode;
    ZlixDeflateLz77OutInfo *outInfo = context->dictCodeOut;
    ZlixDeflateNodeInfo *lTree = context->entroyCode->dynLTree;
    ZLIX_BOOL isZeroBytesInput = ((flush != ZLIX_NO_FLUSH) && (stream->availIn == 0));
    ZLIX_BOOL needCrc = (context->headFormat == ZLIX_FORMAT_DEFLATE_GZIP);
    ZLIX_BOOL needAdler = (context->headFormat == ZLIX_FORMAT_DEFLATE_ZLIB);
    ZLIX_BOOL winbuffIsInput = ZLIX_DEFLATE_WINBUFF_IS_INPUT(dict->winbits);
    (void)flush;
    (void)extraLen;
    if (winbuffIsInput == ZLIX_TRUE) {
        ZlixDeflateWindowIsInput(stream, needCrc, needAdler);
    }

    while (1) {
        /* 直接尽可能地将待压缩数据移入滑窗即可，不需要考虑预留LZ77_MIN_LOOKAHEAD长度的数据 */
        if (dict->lookahead == 0) {
            ZlixDeflateWindowFill(stream, dict, needCrc, needAdler);
            if (dict->lookahead == 0) { /* 所有输入都处理完时，退出循环 */
                break;
            }
        }
        /* 全部数据都视为literal */
        ZLIX_DEFLATE_LZ77_WRITE_LITERAL_OUTPUT(outInfo, *(dict->winUnion.winBuff + dict->startPos), lTree);
        context->storeLen++;
        dict->startPos++;
        dict->lookahead--;
        /* 当ldBuf填满时，直接退出 */
        if (outInfo->count >= outInfo->bufSize) {
            break;
        }
    }

    return ZlixDeflateEndCheck(stream, flush, outInfo, isLast, isZeroBytesInput);
}

ZLIX_STATIC ZLIX_ALWAYS_INLINE uint16_t ZlixDeflateRleMatchFind(ZlixStream *stream, ZlixDeflateLz77DictInfo *dict)
{
    uint16_t match = 0;
    uint8_t *cmpByteAddr;
    uint8_t firstByte;
    uint16_t limitLen;

    /* 第一次压缩时不要前向查找 */
    if (stream->totalIn == dict->lookahead) {
        return 0;
    }

    if ((dict->lookahead >= ZLIX_DEFLATE_LZ77_MIN_LOOKAHEAD) && (dict->startPos > 0)) {
        limitLen = (dict->lookahead < ZLIX_DEFLATE_LZ77_MAX_MATCH_LEN) ?
            dict->lookahead : ZLIX_DEFLATE_LZ77_MAX_MATCH_LEN;
        cmpByteAddr = dict->winUnion.winBuff + dict->startPos - 1;
        firstByte = *cmpByteAddr;
        while (match < limitLen) {
            cmpByteAddr++;
            if (firstByte != *cmpByteAddr) {
                break;
            }
            match += 1;
        }
    }
    return match;
}

/* 不进行LZ77匹配，所有Match对仅由RLE对组成 */
int32_t ZlixDeflateRleAsMatch(ZlixStream *stream, int32_t flush, uint32_t *extraLen, uint32_t *isLast)
{
    ZlixDeflateContext *context = stream->context;
    ZlixDeflateLz77DictInfo *dict = context->dictCode;
    ZlixDeflateLz77OutInfo *outInfo = context->dictCodeOut;
    ZlixDeflateNodeInfo *lTree = context->entroyCode->dynLTree;
    ZlixDeflateNodeInfo *dTree = context->entroyCode->dynDTree;
    uint8_t lvalue;
    uint16_t base;
    uint16_t match;
    uint16_t preStartPos;
    uint32_t minAhead = (flush == ZLIX_NO_FLUSH) ? ZLIX_DEFLATE_LZ77_MIN_LOOKAHEAD : 1;
    ZLIX_BOOL needCrc = (context->headFormat == ZLIX_FORMAT_DEFLATE_GZIP);
    ZLIX_BOOL needAdler = (context->headFormat == ZLIX_FORMAT_DEFLATE_ZLIB);
    ZLIX_BOOL isZeroBytesInput = ((flush != ZLIX_NO_FLUSH) && (stream->availIn == 0));
    ZLIX_BOOL winbuffIsInput = ZLIX_DEFLATE_WINBUFF_IS_INPUT(dict->winbits);
    if (winbuffIsInput == ZLIX_TRUE) {
        ZlixDeflateWindowIsInput(stream, needCrc, needAdler);
    }

    while (1) {
        if (dict->lookahead < ZLIX_DEFLATE_LZ77_MIN_LOOKAHEAD) {
            ZlixDeflateWindowFill(stream, dict, needCrc, needAdler);
            if (dict->lookahead < minAhead) { /* 所有输入都处理完时，退出循环 */
                break;
            }
        }

        match = ZlixDeflateRleMatchFind(stream, dict);
        preStartPos = dict->startPos;
        if (match >= ZLIX_DEFLATE_LZ77_MIN_MATCH_LEN) {
            lvalue = (uint8_t)(match - ZLIX_DEFLATE_LZ77_MIN_MATCH_LEN);
            /* RLE模式下的匹配距离恒为1  */
            ZLIX_DEFLATE_LZ77_WRITE_LENDIST_OUTPUT(outInfo, lvalue, lTree, 1, dTree, *extraLen, base);
            dict->startPos += match - 1;
            dict->lookahead -= match - 1;
        } else {
            lvalue = *(uint8_t *)((uintptr_t)dict->winUnion.winBuff + dict->startPos);
            ZLIX_DEFLATE_LZ77_WRITE_LITERAL_OUTPUT(outInfo, lvalue, lTree);
        }
        /* 为防止当前startPos加1后达到65536变为0，最后计算错误，所以需要先进行相减计算storeLen */
        context->storeLen += (uint32_t)(dict->startPos - preStartPos) + 1;
        dict->lookahead--;
        dict->startPos++;
        /* 当ldBuf填满时，直接退出 */
        if (outInfo->count >= outInfo->bufSize) {
            break;
        }
    }

    return ZlixDeflateEndCheck(stream, flush, outInfo, isLast, isZeroBytesInput);
}
