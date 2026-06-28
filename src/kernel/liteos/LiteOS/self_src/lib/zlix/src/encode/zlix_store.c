/**
 * @file zlix_store.c
 * @copyright Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * @brief \n
 * 提供压缩store模式的关键接口，如获取store块起始地址、进行store块的输出操作等
 * @author Anonym
 * @date 2024-04-12
 * @version v0.1.0
 * *******************************************************************************************
 * @par 修改日志：
 * <table>
 * <tr><th>Date        <th>Version  <th>Author    <th>Description
 * <tr><td>2024-04-12  <td>0.1.0    <td>Anonym    <td>压缩关键接口
 * </table>
 *
 * *******************************************************************************************
 */
#include "zlix_public_inner.h"
#include "zlix_deflate_inner.h"
#include "zlix_store_inner.h"

/* 一个store块的头长度 */
#define ZLIX_DEFLATE_STORE_HEAD_LEN 5

/* storeLen + lookahead为当前输入已经扫描和尚未扫描的大小，
   totalAvailIn为当前输入的总大小，availIn为当前剩余未滑入滑窗的大小，相减得一共滑入滑窗的大小，
   如果小于等于的话，则说明当前已经滑入滑窗的大小包含整个store块，store可以在NextIn中获取 */
#define ZLIX_DEFLATE_STORE_IS_IN_NEXTIN(stream, context, totalAvailIn) \
    ((context)->storeLen + (context)->dictCode->lookahead <= (totalAvailIn) - (stream)->availIn)

/* store块的末尾位置（storeLen为当前块压缩时扫描的大小）即当前startPos的位置，
   所以store块的起始位置为当前滑窗位置前移storeLen，即winBuff + startPos - storeLen，
   要使store块在滑窗内，则需要起始位置在winBuff之后，所以startPos - storeLen要大于等于0，
   即startPos >= storeLen，此时store块可以在滑窗中获取 */
#define ZLIX_DEFLATE_STORE_IS_IN_WINDOW(context) \
    ((context)->dictCode->startPos >= (context)->storeLen)


ZLIX_STATIC const uint8_t *ZlixDeflateBlockStorePending(ZlixDeflateContext *context, const uint8_t *src)
{
    uint32_t len;
    uint32_t times;
    len = context->storeLen; // len最大为65536，所以只需取2个字节即可
    context->outBuf[context->pending++] = (uint8_t)(len & ZLIX_1BYTES_MASK);
    context->outBuf[context->pending++] = (uint8_t)((len >> 8) & ZLIX_1BYTES_MASK); /* 1个字节8比特 */
    context->outBuf[context->pending++] = (uint8_t)((~len) & ZLIX_1BYTES_MASK);
    context->outBuf[context->pending++] = (uint8_t)(((~len) >> 8) & ZLIX_1BYTES_MASK); /* 1个字节8比特 */

    times = len >> 3; /* 右移3位，计算使用八字节输出的次数 */
    len -= (times << 3);  /* 左移3位，计算使用八字节输出后的余数 */
    while (times > 0) {
        context->outBuf[context->pending++] = *src++;
        context->outBuf[context->pending++] = *src++;
        context->outBuf[context->pending++] = *src++;
        context->outBuf[context->pending++] = *src++;
        context->outBuf[context->pending++] = *src++;
        context->outBuf[context->pending++] = *src++;
        context->outBuf[context->pending++] = *src++;
        context->outBuf[context->pending++] = *src++;
        times--;
    }

    while (len > 0) {
        context->outBuf[context->pending++] = *src++;
        len--;
    }

    context->bufLeftLen += context->pending;
    return src; /* 若有多个store块，则需要将下一个store块的起始位置进行更新 */
}

/* 不断输出store块，当超过一个store块最大值时，会进行循环输出 */
void ZlixDeflateBlockStore(ZlixStream *stream, const uint8_t *src, uint32_t last)
{
    ZlixDeflateContext *context = (ZlixDeflateContext *)stream->context;
    int32_t storeLast = 0;
    uint32_t storeTotal = context->storeLen;
    context->dictCodeOut->count = 0;
    while (storeTotal > 0) {
        context->storeLen = (storeTotal > ZLIX_DEFLATE_STORE_MAX_LEN) ? ZLIX_DEFLATE_STORE_MAX_LEN : storeTotal;
        storeTotal -= context->storeLen;

        if ((last == 1) && (storeTotal == 0)) {
            storeLast = 1;
        }
        ZLIX_DEFLATE_WRITE_BITS(context, storeLast, 1);                        /* 标记是否为最后一个块的标志位占1比特 */
        ZLIX_DEFLATE_WRITE_BITS(context, STORED_HEAD_FLAG, 2);            /* 压缩策略标志位占2比特 */
        ZLIX_DEFLATE_FLUSH_BITBUF(context);
        src = ZlixDeflateBlockStorePending(context, src);
    }
}

ZLIX_STATIC ZLIX_ALWAYS_INLINE void ZlixDeflateStoreLz77MatchReset(ZlixDeflateContext *context)
{
    context->dictCode->currMatchStart = 0;
    context->dictCode->currMatchLen = 0;
    context->dictCode->prevMatchStart = 0;
    context->dictCode->prevLength = 0;
    context->dictCode->bPrevMatched = ZLIX_FALSE;
}

/* 获取store块的起始地址，返回值为NULL表示无法获取store块 */
const uint8_t *ZlixDeflateGetStoreSrc(ZlixStream *stream, ZLIX_BOOL dyn,
    uint64_t dynLen, uint64_t staticLen, uint32_t totalAvailIn)
{
    const uint8_t *src;
    uint64_t realStoreLen;
    ZLIX_BOOL store;
    ZlixDeflateContext *context = (ZlixDeflateContext *)stream->context;
    if (context->matchWithoutStore == ZLIX_TRUE) {
        context->matchWithoutStore = ZLIX_FALSE;
        context->storeLen++;
    }
    realStoreLen =
        (((context->storeLen - 1) / ZLIX_DEFLATE_STORE_MAX_LEN) + 1) * ZLIX_DEFLATE_STORE_HEAD_LEN + context->storeLen;
    store = dyn ? (realStoreLen < dynLen) : (realStoreLen < staticLen);
    if (store && ZLIX_DEFLATE_STORE_IS_IN_NEXTIN(stream, context, totalAvailIn)) {
        /* 获取NextIn中store块的起始位置 */
        src = stream->nextIn - context->storeLen - context->dictCode->lookahead;
        ZlixDeflateStoreLz77MatchReset(context);
    } else if (store && ZLIX_DEFLATE_STORE_IS_IN_WINDOW(context)) {
        /* 获取滑窗中store块的起始位置 */
        src = context->dictCode->winUnion.winBuff + context->dictCode->startPos - context->storeLen;
        ZlixDeflateStoreLz77MatchReset(context);
    } else {
        src = NULL;
    }
    /* 如果当前有保留贪心匹配串，且不为store块，则说明尚未打印，有2种情况：
       保存到下一次huffman块中，或者是将其长度记录到store块中。
       当前块若为store块，则会将本次扫描过的内容全部输出，不会保留。 */
    if (context->dictCode->bPrevMatched == ZLIX_TRUE && src == NULL) {
        context->matchWithoutStore = ZLIX_TRUE;
    }
    return src;
}

void ZlixDeflateEasyStore(ZlixStream *stream) // 只压成store块
{
    ZlixDeflateContext *context = (ZlixDeflateContext *)stream->context;
    const uint8_t *src = stream->nextIn;   // 设置store的起点为src
    uint32_t storeTotal = stream->availIn; // 设置store的长度为srcLen
    uint32_t storeLast = 0;
    while (storeTotal > 0) {
        context->storeLen = (storeTotal > ZLIX_DEFLATE_STORE_MAX_LEN) ? ZLIX_DEFLATE_STORE_MAX_LEN : storeTotal;
        storeTotal -= context->storeLen;
        if (storeTotal == 0) { // 最后一块
            storeLast = 1;
        }
        ZLIX_DEFLATE_WRITE_BITS(context, storeLast, 1);          // 标记是否为最后一个块的标志位占1比特
        ZLIX_DEFLATE_WRITE_BITS(context, STORED_HEAD_FLAG, 2);   // 压缩策略标志位占2比特
        ZLIX_DEFLATE_FLUSH_BITBUF(context);
        src = ZlixDeflateBlockStorePending(context, src);
        if ((context->bufLeftLen > 0) && (ZlixDeflateBlockFlushPending(stream, context) == ZLIX_BLOCK_FINISHED_DONE)) {
            ZlixDeflateBlockReset(context);
            continue;
        }
    }
}
