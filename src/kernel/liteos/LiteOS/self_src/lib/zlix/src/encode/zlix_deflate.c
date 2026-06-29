/**
 * @file zlix_deflate.c
 * @copyright Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * @brief \n
 * 提供压缩的关键接口，比如压缩初始化、去初始化、压缩、压缩重置以及获取压缩输出缓存区大小等关键接口
 * @author Anonym
 * @date 2021-10-22
 * @version v0.1.0
 * *******************************************************************************************
 * @par 修改日志：
 * <table>
 * <tr><th>Date        <th>Version  <th>Author    <th>Description
 * <tr><td>2021-10-22  <td>0.1.0    <td>Anonym    <td>压缩关键接口
 * </table>
 *
 * *******************************************************************************************
 */
#include "zlix_deflate_inner.h"
#include "zlix_static_huffman_inner.h"
#include "zlix_utils_inner.h"

typedef int32_t (*ZlixDeflatePreCompressor)(ZlixStream *stream, int32_t flush, uint32_t *extraLen, uint32_t *isLast);

ZlixDeflatePreCompressor g_zlixDeflatePreCompressorTbl[ZLIX_STRATEGY_BUTT] = {ZlixDeflateLz77,
    ZlixDeflateLz77, ZlixDeflateLiteralOnly, ZlixDeflateRleAsMatch, ZlixDeflateLz77
};

/* 长度转换至符号值
   使用时直接用长度作为索引，取得对应的范围值，作为索引的长度要比实际长度少3（默认最小匹配长度），
   最后在得到的范围区间的基础上加上257作为最终的结果（257分为256个字符和1个EOB终结符） */
uint8_t g_lenToCode[ZLIX_DEFLATE_LENGTH_CODE_LEN] = {
    0,  1,  2,  3,  4,  5,  6,  7,  8,  8,  9,  9, 10, 10, 11, 11, 12, 12, 12, 12,
    13, 13, 13, 13, 14, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16,
    17, 17, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19,
    19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
    21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22,
    22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 23, 23,
    23, 23, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
    24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
    25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
    25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 26, 26,
    26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
    26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
    27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 28
};

/* 距离转换至符号值
   0-255为一类，直接作为distToCode[]的索引取得对应范围值，计算为g_distToCode[dist]；
   256-32K为一类，计算时去掉最高位（32K只需要15位来表示）和低7位（剩下范围的边界都为2^7的整数倍，即高位相同情况下低7位不同仍范围一致），
   计算为g_distToCode[256 + (dist >> 7)] */
uint8_t g_distToCode[ZLIX_DEFLATE_DIST_CODE_LEN] = {
    0,  1,  2,  3,  4,  4,  5,  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  8,
    8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 10, 10,
    10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
    11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
    12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13,
    13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
    13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
    14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
    14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
    14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,  0,  0, 16, 17,
    18, 18, 19, 19, 20, 20, 20, 20, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 22,
    23, 23, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
    24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
    26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
    26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 27,
    27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
    27, 27, 27, 27, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
    28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
    28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
    28, 28, 28, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
    29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
    29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
    29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29
};

/* deflate算法最大压缩等级 */
#define ZLIX_DEFLATE_MAX_ENCODE_LEVEL  9

/* deflate算法最大内存等级 */
#define ZLIX_DEFLATE_MAX_MEM_LEVEL     9

/* deflate算法最大窗口大小 */
#define ZLIX_DEFLATE_MAX_WINDOW        16

/* deflate算法最小窗口大小 */
#define ZLIX_DEFLATE_MIN_WINDOW        9

/* 双字节的bit位 */
#define ZLIX_DEFLATE_HALF_WORD_BITS     16

#define ZLIX_DEFLATE_MAX_LOOKAHEAD  65535

#define ZLIX_2BYTES_MASK 0xFFFF

/* 计算blcok块的字节数，7用于字节向上对齐 */
#define ZLIX_DEFLATE_BLOCK_BITS_TO_BYTES(bits) (((bits) + 7) >> 3)

/* 用户配置信息check */
#define ZLIX_DEFLATE_CONFIG_IS_INVALID(usrConfig) \
    (((usrConfig)->level > ZLIX_DEFLATE_MAX_ENCODE_LEVEL) || \
        ((usrConfig)->strategy >= ZLIX_STRATEGY_BUTT) || \
         ((usrConfig)->memLevel == 0) || \
         ((usrConfig)->memLevel > ZLIX_DEFLATE_MAX_MEM_LEVEL))

/* 用户配置winbits信息check */
#define ZLIX_DEFLATE_WINBITS_IS_INVALID(winBits) \
    (((winBits) < ZLIX_DEFLATE_MIN_WINDOW) || ((winBits) > ZLIX_DEFLATE_MAX_WINDOW))

#define ZLIX_DEFLATE_BOUND_RAWSIZE_WILD(srcLen) ((srcLen) + (((srcLen) + 7) >> 3) + (((srcLen) + 63) >> 6) + 5)

#define ZLIX_DEFLATE_BOUND_RAWSIZE(srcLen) ((srcLen) + ((srcLen) >> 12) + ((srcLen) >> 14) + ((srcLen) >> 25) + 7)

#define ZLIX_DEFLATE_DATA_REMAINING(streamInfo, dict) (((streamInfo)->availIn > 0) || ((dict)->lookahead > 0))

#define ZLIX_DEFLATE_FLUSH_RANK(flushType) (((flushType) * 2) - ((flushType) > 4 ? 9 : 0))

ZLIX_STATIC ZLIX_ALWAYS_INLINE uint32_t ZlixDefaultHeaderCal(uint16_t level, uint16_t winBits, uint16_t strategy)
{
    uint32_t header = (8 + ((winBits - 8) << 4)) << 8; // 8和4代表偏移位数
    uint32_t levelFlag;
    if (strategy >= ZLIX_DEFLATE_HUFFMAN_ONLY || level < 2) { // 等级2
        levelFlag = 0;
    } else if (level < 6) { // 默认等级6
        levelFlag = 1;
    } else if (level == 6) { // 默认等级6
        levelFlag = 2; // 填充到header二进制2
    } else {
        levelFlag = 3; // 填充到header二进制3
    }
    header |= (levelFlag << 6); // 偏移位数6
    header += 31 - (header % 31); // 整除31
    return header;
}

/* deflate算法入参检查 */
ZLIX_STATIC int32_t ZlixDeflateInputCheck(const ZlixConfigInfo *usrConfigInfo,
                                          int32_t *newWinBits, ZlixHeadFormat *format)
{
    int32_t tmpWinBits;
    ZlixHeadFormat tmpFormat;

    if (ZLIX_DEFLATE_CONFIG_IS_INVALID(usrConfigInfo)) {
        return ZLIX_INPUT_PARA_INVALID;
    }

    if (usrConfigInfo->winBits < 0) {
        tmpWinBits = -usrConfigInfo->winBits;
        tmpFormat = ZLIX_FORMAT_DEFLATE_RAW;
    } else if (usrConfigInfo->winBits > ZLIX_DEFLATE_MAX_WINDOW) {
        tmpWinBits = usrConfigInfo->winBits - ZLIX_DEFLATE_HALF_WORD_BITS;
        tmpFormat = ZLIX_FORMAT_DEFLATE_GZIP;
    } else {
        tmpWinBits = (int32_t)usrConfigInfo->winBits;
        tmpFormat = ZLIX_FORMAT_DEFLATE_ZLIB;
    }

    if (ZLIX_DEFLATE_WINBITS_IS_INVALID(tmpWinBits)) {
        return ZLIX_INPUT_PARA_INVALID;
    }

    *newWinBits = tmpWinBits;
    *format = tmpFormat;

    return ZLIX_OK;
}

int32_t ZlixDeflateBufferInit(const ZlixConfigInfo *usrConfigInfo, ZlixDeflateContext *context,
    uint32_t *innerMallocSizePtr, ZlixMemFuncs zlixMemOper)
{
    const uint32_t innerBufsize = 1 << (usrConfigInfo->memLevel + 8); /* 用户默认配置8 + 8，保证默认64K */
    uint32_t innerMallocSize = innerBufsize + (innerBufsize >> 1);
    context->outBuf = (uint8_t *)ZLIX_LABEL_MALLOC(zlixMemOper, ZLIX_DEFLATE_OUTBUF_LABEL, innerMallocSize);
    if (context->outBuf == NULL) {
        ZLIX_LABEL_FREE(zlixMemOper, ZLIX_DEFLATE_CONTEXT_LABEL, context);
        return ZLIX_MEM_OPER_FAILURE;
    }
    *innerMallocSizePtr = innerMallocSize;

    (void)memset_s(context->outBuf, innerMallocSize, 0, innerMallocSize);

    context->pendingBuf = context->outBuf;
    context->bufSize = innerBufsize;
    context->bufLeftLen = 0;
    return ZLIX_OK;
}

/* deflate算法上下文初始化 */
ZLIX_STATIC ZlixDeflateContext *ZlixDeflateContextInit(const ZlixConfigInfo *usrConfigInfo, int32_t newWinBits,
                                                       ZlixHeadFormat format)
{
    ZlixMemFuncs zlixMemOper;
    uint32_t innerMallocSize;
    ZlixDeflateContext *context;
    uint8_t level;
    int32_t ret = ZlixMemFuncsCheck(usrConfigInfo, &zlixMemOper);
    if (ret != ZLIX_OK) {
        return NULL;
    }
    context = (ZlixDeflateContext *)ZLIX_LABEL_MALLOC(zlixMemOper, ZLIX_DEFLATE_CONTEXT_LABEL,
        sizeof(ZlixDeflateContext));
    if (context == NULL) {
        return NULL;
    }

    (void)memset_s(context, sizeof(ZlixDeflateContext), 0, sizeof(ZlixDeflateContext));
    context->zlixMemOper = zlixMemOper;
    context->matchWithoutStore = ZLIX_FALSE;

    /* step1: 内部缓存区资源初始化 */
    if (ZlixDeflateBufferInit(usrConfigInfo, context, &innerMallocSize, zlixMemOper) != ZLIX_OK) {
        return NULL;
    }

    level = (usrConfigInfo->level == 0) ? 1 : usrConfigInfo->level;

    /* step2: lz77初始化 */
    context->dictCode = ZlixDeflateLz77DictInfoInit(newWinBits, level, usrConfigInfo->memLevel, zlixMemOper);
    if (context->dictCode == NULL) {
        ZLIX_LABEL_FREE(zlixMemOper, ZLIX_DEFLATE_OUTBUF_LABEL, context->outBuf);
        ZLIX_LABEL_FREE(zlixMemOper, ZLIX_DEFLATE_CONTEXT_LABEL, context);
        return NULL;
    }
    context->dictCodeOut = ZlixDeflateLz77DictOutInfoInit(usrConfigInfo->memLevel, context->outBuf, innerMallocSize,
        zlixMemOper);
    if (context->dictCodeOut == NULL) {
        ZlixDeflateLz77DictInfoFini(&context->dictCode, zlixMemOper);
        ZLIX_LABEL_FREE(zlixMemOper, ZLIX_DEFLATE_OUTBUF_LABEL, context->outBuf);
        ZLIX_LABEL_FREE(zlixMemOper, ZLIX_DEFLATE_CONTEXT_LABEL, context);
        return NULL;
    }

    /* step3: huffman熵编码初始化 */
    context->entroyCode = ZlixHuffmanEntroyCodeInfoInit(zlixMemOper);
    if (context->entroyCode == NULL) {
        ZlixDeflateLz77DictInfoFini(&context->dictCode, zlixMemOper);
        ZLIX_LABEL_FREE(zlixMemOper, ZLIX_DEFLATE_OUTBUF_LABEL, context->outBuf);
        ZLIX_LABEL_FREE(zlixMemOper, ZLIX_DEFLATE_LZ77_OUTINFO_LABEL, context->dictCodeOut);
        ZLIX_LABEL_FREE(zlixMemOper, ZLIX_DEFLATE_CONTEXT_LABEL, context);
        return NULL;
    }

    /* step4. 赋值其他用户字段 */
    context->status = ((format == ZLIX_FORMAT_DEFLATE_GZIP) ? ZLIX_STATUS_DEFLATE_GZIP_HEAD :
                      ((format == ZLIX_FORMAT_DEFLATE_ZLIB) ? ZLIX_STATUS_DEFLATE_ZLIB_HEAD : ZLIX_STATUS_INIT));
    context->headFormat = format;
    context->strategy = usrConfigInfo->strategy;
    context->forceStore = (usrConfigInfo->level == 0) ? 1 : 0; /* 标志位预留 */

    return context;
}

/*  deflate算法总初始化接口 */
int32_t ZlixDeflateInit(const ZlixConfigInfo *usrConfigInfo, ZlixStream *streamInfo)
{
    int32_t ret;
    int32_t newWinBits = 0;
    ZlixHeadFormat tmpFormat = ZLIX_FORMAT_DEFLATE_RAW;

    /* step1: 入参校验 */
    ret = ZlixDeflateInputCheck(usrConfigInfo, &newWinBits, &tmpFormat);
    if (ret != ZLIX_OK) {
        return ret;
    }

    /* step2: 上下文初始化 */
    streamInfo->context = (void *)ZlixDeflateContextInit(usrConfigInfo, newWinBits, tmpFormat);
    if (streamInfo->context == NULL) {
        return ZLIX_MEM_OPER_FAILURE;
    }

    streamInfo->method = usrConfigInfo->method;

    return ZLIX_OK;
}

ZLIX_STATIC ZLIX_ALWAYS_INLINE void ZlixDeflateBlockWriteLdClInfo(uint32_t len, ZlixDeflateNodeInfo *clTree,
    ZlixDeflateBasicDescInfo *basicDescInfo, ZlixDeflateContext *context)
{
    uint32_t i = 0;
    uint8_t val;

    while (i < len) {
        val = basicDescInfo->clArray[i];
        ZLIX_DEFLATE_WRITE_BITS(context, clTree[val].code.nodeCode, clTree[val].len.rangeCode);
        if (val >= 16) { /* 16以上为游程编码的重复标识 */
            i++;
            ZLIX_DEFLATE_WRITE_BITS(context, basicDescInfo->clArray[i], g_bExtraBits[val]);
        }
        i++;
    }
}

ZLIX_STATIC void ZlixDeflateBlockWriteClInfo(ZlixDeflateContext *context)
{
    ZlixDeflateHuffmanCodeInfo *entroyCode = context->entroyCode;
    ZlixDeflateBasicDescInfo *cclDescInfo;
    ZlixDeflateNodeInfo *clTree = entroyCode->dynClTree;
    uint32_t i;
    uint32_t cclLen;
    uint32_t lLen;
    uint32_t dLen;

    lLen = entroyCode->dynLBasicDescLInfo.maxFreqNoZeroCode + 1;
    dLen = entroyCode->dynLBasicDescDInfo.maxFreqNoZeroCode + 1;

    ZLIX_DEFLATE_WRITE_BITS(context, lLen - 257, ZLIX_HLIT_LEN); // 257指的是CL1序列的长度
    ZLIX_DEFLATE_WRITE_BITS(context, dLen - 1, ZLIX_HDIST_LEN);

    /* 计算ccl的长度 */
    cclDescInfo = &(entroyCode->dynLBasicDescCInfo);
    cclLen = cclDescInfo->clLen;
    ZLIX_DEFLATE_WRITE_BITS(context, cclLen - 4, ZLIX_HCCL_LEN); // 4指的是CLL序列的长度

    for (i = 0; i < cclLen; i++) {
        ZLIX_DEFLATE_WRITE_BITS(context, cclDescInfo->clArray[g_cclQueue[i]], 3); // 写3bit
    }

    ZlixDeflateBlockWriteLdClInfo(entroyCode->dynLBasicDescLInfo.clLen, clTree, &(entroyCode->dynLBasicDescLInfo),
        context);
    ZlixDeflateBlockWriteLdClInfo(entroyCode->dynLBasicDescDInfo.clLen, clTree, &(entroyCode->dynLBasicDescDInfo),
        context);

    return;
}

ZLIX_STATIC void ZlixDeflateBlockWriteLitLenDistBits(ZLIX_BOOL dyn, ZlixDeflateContext *context)
{
#ifdef ZLIX_LOW_COST_BUT_SLOWER
    (void)dyn;
#endif
    uint32_t i;
    ZlixDeflateLz77OutInfo *outInfo = context->dictCodeOut;
    ZlixDeflateHuffmanCodeInfo *entroyCode = context->entroyCode;
    ZlixDeflateNodeInfo *lTree;
    ZlixDeflateNodeInfo *dTree;
    uint16_t val;
    uint16_t code;
    uint16_t len;
    uint16_t dist;

#ifndef ZLIX_LOW_COST_BUT_SLOWER
    if (dyn) {
#endif
        lTree = entroyCode->dynLTree;
        dTree = entroyCode->dynDTree;
#ifndef ZLIX_LOW_COST_BUT_SLOWER
    } else {
        lTree = entroyCode->staticLTree;
        dTree = entroyCode->staticDTree;
    }
#endif

    for (i = 0; i < outInfo->count; i++) {
        if (outInfo->disBuf[i] == 0) {
            /* 写入单个字符编码 */
            val = outInfo->literLenBuf[i];
            ZLIX_DEFLATE_WRITE_BITS(context, lTree[val].code.nodeCode, lTree[val].len.rangeCode);
        } else {
            /* 依次写入长度、距离的bits和extra bits */
            len = (uint16_t)outInfo->literLenBuf[i] + 3;  /* 最小长度为3 */
            code = ZLIX_DEFLATE_GET_LENGTH_BASE(outInfo->literLenBuf[i]);
            val = code + ZLIX_LITERALS_EXTRA_BASE;
            ZLIX_DEFLATE_WRITE_BITS(context, lTree[val].code.nodeCode, lTree[val].len.rangeCode);
            ZLIX_DEFLATE_WRITE_BITS(context, (uint32_t)len - g_baseLength[code], g_lExtraBits[code]);
            dist = outInfo->disBuf[i];
            val = ZLIX_DEFLATE_GET_DIST_BASE(dist - 1);
            ZLIX_DEFLATE_WRITE_BITS(context, dTree[val].code.nodeCode, dTree[val].len.rangeCode);
            ZLIX_DEFLATE_WRITE_BITS(context, dist - g_baseDist[val], g_dExtraBits[val]);
        }
    }

    /* 写入块结束符 */
    ZLIX_DEFLATE_WRITE_BITS(context,
        lTree[ZLIX_LITERALS_CODE_RANGE].code.nodeCode, lTree[ZLIX_LITERALS_CODE_RANGE].len.rangeCode);
    context->bufLeftLen += context->pending;
}

/* 将pending中的输出块输出到外部buff中 */
int32_t ZlixDeflateBlockFlushPending(ZlixStream *stream, ZlixDeflateContext *context)
{
    uint32_t max = (stream->availOut < context->bufLeftLen) ? stream->availOut : context->bufLeftLen;
    uint32_t temp = max;
    uint8_t *out = stream->nextOut;
    uint8_t *pendingBuf = context->pendingBuf;
    uint32_t times = temp >> 3; /* 八字节输出的次数 */

    temp -= (times << 3);  /* 右移3位，计算八字节输出后的余数 */
    while (times > 0) {
        *out++ = *pendingBuf++;
        *out++ = *pendingBuf++;
        *out++ = *pendingBuf++;
        *out++ = *pendingBuf++;
        *out++ = *pendingBuf++;
        *out++ = *pendingBuf++;
        *out++ = *pendingBuf++;
        *out++ = *pendingBuf++;
        times--;
    }

    while (temp > 0) {
        *out++ = *pendingBuf++;
        temp--;
    }

    context->bufLeftLen -= max;
    stream->nextOut = out;
    stream->availOut -= max;
    stream->totalOut += max;

    if (context->bufLeftLen != 0) {
        context->pendingBuf = pendingBuf;
        context->lastFlushType = -1; // 这个状态意味着上一轮调用deflate没有输出干净
        return ZLIX_BLOCK_NEED_OUTPUT;
    } else {
        return ZLIX_BLOCK_FINISHED_DONE;
    }
}

void ZlixDeflateBlockReset(ZlixDeflateContext *context)
{
    ZlixDeflateLz77OutInfo *outInfo = context->dictCodeOut;
    ZlixDeflateHuffmanCodeInfo *entroyCode = context->entroyCode;
    size_t lTreeLen = ZLIX_DEFLATE_HUFFMAN_TREE_SIZE(ZLIX_DEFLATE_L_CNT);
    size_t dTreeLen = ZLIX_DEFLATE_HUFFMAN_TREE_SIZE(ZLIX_DEFLATE_DIST_CNT);
    size_t clTreeLen = ZLIX_DEFLATE_HUFFMAN_TREE_SIZE(ZLIX_DEFLATE_CL_CNT);

    (void)memset_s(entroyCode->dynLTree, lTreeLen, 0, lTreeLen);
    (void)memset_s(entroyCode->dynDTree, dTreeLen, 0, dTreeLen);
    (void)memset_s(entroyCode->dynClTree, clTreeLen, 0, clTreeLen);

    outInfo->count = 0;
    context->bufLeftLen = 0;
    context->pending = 0;
    context->pendingBuf = context->outBuf;
    entroyCode->staticLen = 0;
    entroyCode->dynLen = 0;
    context->storeLen = 0;
    context->extraLen = 0;
}

/* Gzip头填充，目前仅支持gzhead头为空的情况 */
ZLIX_STATIC void ZlixDeflateWriteGzipHead(ZlixStream *stream, ZlixDeflateContext *context,
    const ZlixGzHeadInfo *gzHeadInfo)
{
    size_t endIndex;
    const uint8_t *curPos = (gzHeadInfo == NULL) ? NULL : (const uint8_t *)gzHeadInfo->fileName;
    size_t pathLen = (gzHeadInfo == NULL || curPos == NULL) ? 0 : gzHeadInfo->fileNameLen;
    stream->adler = 0;

    ZLIX_DEFLATE_WRITE_BITS(context, ZLIX_GZIP_FLAG_ID_MAGIC, 16);    /* 魔术字占16个比特 */
    ZLIX_DEFLATE_WRITE_BITS(context, 8, 8);       /* 8代表gzip使用deflate压缩方法 */

    if (pathLen == 0) {
        ZLIX_DEFLATE_WRITE_BITS(context, 0, 8);      /* FLAG字段占8个比特 */
    } else {
        ZLIX_DEFLATE_WRITE_BITS(context, 8, 8);     /* FLAG第3位表示gzip包含原始名称，对应值为8 */
    }
    ZLIX_DEFLATE_WRITE_BITS(context, 0, 16);     /* MTIME字段占32个比特，由于WRITE_BITS宏的特性分两次填写，每次16比特 */
    ZLIX_DEFLATE_WRITE_BITS(context, 0, 16);     /* MTIME字段占32个比特，由于WRITE_BITS宏的特性分两次填写，每次16比特 */

    ZLIX_DEFLATE_WRITE_BITS(context, 0, 16);     /* 这里实际上为XFL和OS两个字段，各占一字节，先合并一起输入16比特 */

    /* 保存文件名，endIndex初始为3，用来表示每次循环读取的末尾的下标，每次读取4字节 */
    for (endIndex = 3; endIndex < pathLen; endIndex += 4) {
        ZLIX_DEFLATE_WRITE_BITS(context, *curPos, 8);       /* 一次读取8个比特的字符，读取第0字节 */
        ZLIX_DEFLATE_WRITE_BITS(context, *(curPos + 1), 8); /* 一次读取8个比特的字符，读取第1字节 */
        ZLIX_DEFLATE_WRITE_BITS(context, *(curPos + 2), 8); /* 一次读取8个比特的字符，读取第2字节 */
        ZLIX_DEFLATE_WRITE_BITS(context, *(curPos + 3), 8); /* 一次读取8个比特的字符，读取第3字节 */
        curPos += 4;    /* 一次读取4个字节到context中 */
    }

    while (curPos != NULL && *curPos != 0) {
        ZLIX_DEFLATE_WRITE_BITS(context, *curPos, 8);       /* 一次读取8个比特的字符 */
        curPos++;
    }
    if (pathLen != 0) {
        ZLIX_DEFLATE_WRITE_BITS(context, 0, 8); /* 最后8比特为0，表示字符串结束 */
    }
    ZLIX_DEFLATE_FLUSH_BITBUF(context);

    context->bufLeftLen += context->pending;

    context->status = ZLIX_STATUS_ENCODE;
}

/* Gzip尾填充 */
ZLIX_STATIC int32_t ZlixDeflateWriteGzipTail(ZlixStream *stream, ZlixDeflateContext *context)
{
    ZLIX_DEFLATE_WRITE_BITS(context, stream->adler & ZLIX_2BYTES_MASK, 16);       /* 校验和占32个比特，拆成两次填写，每次16比特 */
    ZLIX_DEFLATE_WRITE_BITS(context, (stream->adler >> 16) & ZLIX_2BYTES_MASK, 16);   /* 拆成两次填写，每次16比特 */

    ZLIX_DEFLATE_WRITE_BITS(context, stream->totalIn & ZLIX_2BYTES_MASK, 16);     /* 长度和占32个比特，拆成两次填写，每次16比特 */
    ZLIX_DEFLATE_WRITE_BITS(context, (stream->totalIn >> 16) & ZLIX_2BYTES_MASK, 16);   /* 拆成两次填写，每次16比特 */

    ZLIX_DEFLATE_FLUSH_BITBUF(context);
    context->bufLeftLen += context->pending;

    return ZlixDeflateBlockFlushPending(stream, context);
}

/* Zlib头填充，当前默带预置字典，小端序 */
ZLIX_STATIC void ZlixDeflateWriteZlibHead(ZlixStream *stream, ZlixDeflateContext *context)
{
    uint32_t header = ZlixDefaultHeaderCal(context->dictCode->compLevel,
        context->dictCode->winbits, context->strategy);
    stream->adler = ZlixUtilsAdler32(NULL, 0);

    ZLIX_DEFLATE_WRITE_BITS(context, (header >> 8) & ZLIX_1BYTES_MASK, 8); // 8比特
    ZLIX_DEFLATE_WRITE_BITS(context, header & ZLIX_1BYTES_MASK, 8); // 8比特

    ZLIX_DEFLATE_FLUSH_BITBUF(context);
    context->bufLeftLen += context->pending;
    context->status = ZLIX_STATUS_ENCODE;
}

/* Zlib尾填充，小端序 */
ZLIX_STATIC int32_t ZlixDeflateWriteZlibTail(ZlixStream *stream, ZlixDeflateContext *context)
{
    ZLIX_DEFLATE_WRITE_BITS(context, (stream->adler >> 24) & ZLIX_1BYTES_MASK, 8); // 右移24位取高8比特
    ZLIX_DEFLATE_WRITE_BITS(context, (stream->adler >> 16) & ZLIX_1BYTES_MASK, 8); // 右移16位取高8比特
    ZLIX_DEFLATE_WRITE_BITS(context, (stream->adler >> 8) & ZLIX_1BYTES_MASK, 8); // 右移8位取高8比特
    ZLIX_DEFLATE_WRITE_BITS(context, stream->adler & ZLIX_1BYTES_MASK, 8); // 取最低8比特

    ZLIX_DEFLATE_FLUSH_BITBUF(context);
    context->bufLeftLen += context->pending;

    return ZlixDeflateBlockFlushPending(stream, context);
}

ZLIX_STATIC ZLIX_ALWAYS_INLINE void ZlixDeflateBlockHuffman(ZlixStream *stream, uint32_t last, ZLIX_BOOL dyn)
{
    uint8_t flag;
    ZLIX_BOOL dynAdjust = dyn;
    ZlixDeflateContext *context = (ZlixDeflateContext *)stream->context;
    if (context->strategy == ZLIX_DEFLATE_FIXED) {
        dynAdjust = ZLIX_FALSE;
    }
    flag = (dynAdjust ? DYNAMIC_HUFF_HEAD_FLAG : FIXED_HUFF_HEAD_FLAG);
    ZLIX_DEFLATE_WRITE_BITS(context, last, 1);                        /* 标记是否为最后一个块的标志位占1比特 */
    ZLIX_DEFLATE_WRITE_BITS(context, flag, 2);                        /* 压缩策略标志位占2比特 */

    if (dynAdjust) {
        ZlixDeflateBlockWriteClInfo(context);
#ifdef ZLIX_LOW_COST_BUT_SLOWER
    } else {
        /* 在要使用静态huffman模式时，再构建静态树 */
        ZlixHuffmanStaticTreeInit(context->entroyCode->dynLTree, context->entroyCode->dynDTree);
#endif
    }
    ZlixDeflateBlockWriteLitLenDistBits(dynAdjust, context);
}

ZLIX_STATIC ZLIX_ALWAYS_INLINE int32_t ZlixDeflateBlock(
    ZlixStream *stream, ZlixDeflateContext *context, int32_t flushType)
{
    ZLIX_BOOL dyn;
    const uint8_t *src = NULL;
    uint32_t totalAvailIn = stream->availIn;
    uint32_t last = 0;
    uint64_t staticLen;
    uint64_t dynLen;
    ZlixDeflateHuffmanCodeInfo *entroyCode = context->entroyCode;
    ZlixDeflatePreCompressor preCompressor = g_zlixDeflatePreCompressorTbl[context->strategy];
    /*  此处进行预处理，即将原待压缩数据处理成可以使用Huffman编码或者store编码的格式 */
    if (preCompressor(stream, flushType, &context->extraLen, &last) == ZLIX_DEFLATE_BLOCK_NEXT_TIME) {
        return ZLIX_BLOCK_NEED_INPUT;
    }

    ZlixHuffmanDynamicCode(entroyCode, context);
    staticLen = ZLIX_DEFLATE_BLOCK_BITS_TO_BYTES(entroyCode->staticLen + context->extraLen + ZLIX_HEAD_LEN);
    /* 17 = 3 + 5 + 5 + 4, 为头标志和长度标识总和 */
    dynLen = ZLIX_DEFLATE_BLOCK_BITS_TO_BYTES(entroyCode->dynLen + context->extraLen + 17);
    dyn = (dynLen < staticLen);
    /* 获取store起始地址 */
    src = ZlixDeflateGetStoreSrc(stream, dyn, dynLen, staticLen, totalAvailIn);
    if (src != NULL) {
        ZlixDeflateBlockStore(stream, src, last);
    } else {
        ZlixDeflateBlockHuffman(stream, last, dyn);
    }

    if (last == 1) {
        context->status = ZLIX_STATUS_FINISH;
    }

    return ZlixDeflateBlockFlushPending(stream, context);
}

ZLIX_STATIC int32_t ZlixDeflateFinishFlush(ZlixStream *stream, ZlixDeflateContext *context)
{
    ZLIX_DEFLATE_FLUSH_BITBUF(context);
    context->bufLeftLen += context->pending;
    if ((context->bufLeftLen > 0) && (ZlixDeflateBlockFlushPending(stream, context) != ZLIX_BLOCK_FINISHED_DONE)) {
        return ZLIX_OK;
    }
    ZlixDeflateBlockReset(context);

    if (context->status <= ZLIX_STATUS_FINISH) {
        if (context->headFormat == ZLIX_FORMAT_DEFLATE_ZLIB) {
            (void)ZlixDeflateWriteZlibTail(stream, context);
            context->status = ZLIX_STATUS_DEFLATE_ZLIB_TAIL;
        } else if (context->headFormat == ZLIX_FORMAT_DEFLATE_GZIP) {
            (void)ZlixDeflateWriteGzipTail(stream, context);
            context->status = ZLIX_STATUS_DEFLATE_GZIP_TAIL;
        }
    }

    if (context->bufLeftLen == 0) {
        return ZLIX_STREAM_END;
    }
    return ZLIX_OK;
}

ZLIX_STATIC ZLIX_ALWAYS_INLINE void ZlixDeflateHeadMake(ZlixStream *streamInfo,
    ZlixDeflateContext *context, ZlixGzHeadInfo *gzHeadInfo)
{
    if (context->status == ZLIX_STATUS_DEFLATE_GZIP_HEAD) {
        ZlixDeflateWriteGzipHead(streamInfo, context, gzHeadInfo);
    } else if (context->status == ZLIX_STATUS_DEFLATE_ZLIB_HEAD) {
        ZlixDeflateWriteZlibHead(streamInfo, context);
    } else if (context->status == ZLIX_STATUS_INIT) {
        context->status = ZLIX_STATUS_ENCODE;
    }
}

ZLIX_STATIC ZLIX_ALWAYS_INLINE ZLIX_BOOL ZlixFlushPendingNeedReturn(ZlixStream *stream, ZlixDeflateContext *context)
{
    if (ZlixDeflateBlockFlushPending(stream, context) == ZLIX_BLOCK_NEED_OUTPUT) {
        return ZLIX_TRUE;
    } else {
        ZlixDeflateBlockReset(context);
        return ZLIX_FALSE;
    }
}

/* 当本轮outbuffer不足时，需要重新调用deflate函数,请求更多输出buffer */
#define ZLIX_FLUSH_AND_RETURN_IF_NEEDED(streamInfo, context) do { \
    if (ZlixFlushPendingNeedReturn(streamInfo, context)) { \
        return ZLIX_OK; \
    } \
} while (0)

ZLIX_STATIC ZLIX_ALWAYS_INLINE void ZlixMakePartialMarker(ZlixStream *streamInfo, ZlixDeflateContext *context)
{
    (void)streamInfo;
    ZLIX_DEFLATE_WRITE_BITS(context, (uint64_t)(1 << 1), 3); // 3bits static huffman
    ZLIX_DEFLATE_WRITE_BITS(context, 0, 7); // 7bits
    ZLIX_DEFLATE_FLUSH_BITBUF_PARTIAL(context); // keeping at most 7 bits in it
    context->bufLeftLen += context->pending;
}

ZLIX_STATIC ZLIX_ALWAYS_INLINE void ZlixMakeSyncFlushMarker(ZlixStream *streamInfo, ZlixDeflateContext *context)
{
    if (streamInfo->totalIn == 0) {
        context->bitBuf = 0;
        context->usedBitCnt = 0;
    }
    ZLIX_DEFLATE_WRITE_BITS(context, 0, 3); // 3bits store
    ZLIX_DEFLATE_FLUSH_BITBUF(context);
    ZLIX_DEFLATE_WRITE_BITS(context, 0, 16); // 16bits
    ZLIX_DEFLATE_WRITE_BITS(context, ZLIX_1BYTES_MASK, 8); // 8bits
    ZLIX_DEFLATE_WRITE_BITS(context, ZLIX_1BYTES_MASK, 8); // 8bits
    ZLIX_DEFLATE_FLUSH_BITBUF(context);
    context->bufLeftLen += context->pending;
}

ZLIX_STATIC ZLIX_ALWAYS_INLINE void ZlixMakeFullFlushMarker(ZlixStream *streamInfo, ZlixDeflateContext *context)
{
    ZlixDeflateLz77DictInfo *dictInfo = context->dictCode;
    ZlixMakeSyncFlushMarker(streamInfo, context);
    (void)memset_s(dictInfo->hash,
        sizeof(uint16_t) * (1 << dictInfo->hashBits), 0, sizeof(uint16_t) * (1 << dictInfo->hashBits));
    (void)memset_s(dictInfo->hashChain,
        sizeof(uint16_t) * (1 << dictInfo->winbits), 0, sizeof(uint16_t) * (1 << dictInfo->winbits));
    if (context->dictCode->lookahead == 0) {
        context->dictCode->startPos = (1 << dictInfo->winbits);
        context->dictCode->bPrevMatched = 0;
        context->dictCode->prevMatchStart = 0;
        context->dictCode->prevLength = 0;
        context->dictCode->currMatchStart = 0;
        context->dictCode->currMatchLen = 0;
    }
}

ZLIX_STATIC ZLIX_ALWAYS_INLINE void ZlixDeflateMakeBlockEndMarker(ZlixStream *streamInfo,
    ZlixDeflateContext *context, int32_t flushType)
{
    if (flushType == ZLIX_PARTIAL_FLUSH) {
        ZlixMakePartialMarker(streamInfo, context);
    } else if (flushType == ZLIX_SYNC_FLUSH) {
        ZlixMakeSyncFlushMarker(streamInfo, context);
    } else if (flushType == ZLIX_FULL_FLUSH) {
        ZlixMakeFullFlushMarker(streamInfo, context);
    }
}

ZLIX_STATIC ZLIX_ALWAYS_INLINE int32_t ZlixDeflateEndRetHandle(
    ZlixStream *streamInfo, ZlixDeflateContext *context, int32_t flushType, int32_t endRet)
{
    if (endRet == ZLIX_BLOCK_FINISHED_DONE) {
        ZlixDeflateMakeBlockEndMarker(streamInfo, context, flushType);
        /* flush block endmarker */
        if (context->bufLeftLen != 0) {
            ZLIX_FLUSH_AND_RETURN_IF_NEEDED(streamInfo, context);
        }
    }

    if ((flushType != ZLIX_FINISH) || (endRet == ZLIX_BLOCK_NEED_OUTPUT)) {
        return ZLIX_OK;
    }

    return ZlixDeflateFinishFlush(streamInfo, context);
}

/* 与zlib判断逻辑保持一致，如果传入的待压缩数据长度为0，
   并且不是FINISH模式，同时Flush模式的等级没有提升，则认定为无意义调用返回失败 */
ZLIX_STATIC ZLIX_ALWAYS_INLINE ZLIX_BOOL ZlixDeflateMeaninglessRepeat(
    ZlixStream *streamInfo, int32_t flushType, int32_t oldFlushType)
{
    return ((streamInfo->availIn == 0) &&
        (ZLIX_DEFLATE_FLUSH_RANK(flushType) <= ZLIX_DEFLATE_FLUSH_RANK(oldFlushType)) && (flushType != ZLIX_FINISH)) ?
        ZLIX_TRUE : ZLIX_FALSE;
}

ZLIX_STATIC ZLIX_ALWAYS_INLINE int32_t ZlixDeflateWinbuffAsInputPrepare(
    ZlixStream *streamInfo, ZlixDeflateLz77DictInfo *dict, int32_t flushType)
{
    /* 输入作为滑窗时限制条件，因为输入大小可能在压缩初始化后传入，所以需要在压缩时判断 */
    if (ZLIX_DEFLATE_WINBUFF_IS_INPUT(dict->winbits)) {
        /* winbits为16时需要一次性输入，防止流式压缩时滑窗内容消失 */
        if ((flushType != ZLIX_FINISH) || (streamInfo->availIn > ZLIX_DEFLATE_MAX_LOOKAHEAD)) {
            return ZLIX_STREAM_ERROR;
        }
        return ZlixDeflateLz77DictInfoUpdate(streamInfo);
    }

    return ZLIX_OK;
}

ZLIX_STATIC ZLIX_ALWAYS_INLINE int32_t ZlixDeflateContextCheck(ZlixDeflateContext *context, int32_t flushType)
{
    if ((context->status >= ZLIX_BUTT) ||
        ((context->status >= ZLIX_STATUS_FINISH) && (flushType != ZLIX_FINISH))) {
        return ZLIX_STREAM_ERROR;
    }

    return ZLIX_OK;
}

ZLIX_STATIC ZLIX_ALWAYS_INLINE ZLIX_BOOL ZlixDeflateFlushTypeInValid(int32_t flushType)
{
    return ((flushType < ZLIX_NO_FLUSH) || (flushType > ZLIX_BLOCK)) ? ZLIX_TRUE : ZLIX_FALSE;
}

ZLIX_STATIC ZLIX_ALWAYS_INLINE int32_t ZlixDeflateStreamInfoCheck(ZlixStream *streamInfo)
{
    if ((streamInfo == NULL) || (streamInfo->context == NULL) || (streamInfo->nextOut == NULL) ||
        ((streamInfo->nextIn == NULL) && (streamInfo->availIn != 0))) {
        return ZLIX_STREAM_ERROR;
    }
    if (streamInfo->availOut == 0) {
        return ZLIX_OUTPUT_BUF_INSUFFICENT;
    }

    return ZLIX_OK;
}

int32_t ZlixDeflate(int32_t flushType, ZlixStream *streamInfo, ZlixGzHeadInfo *gzHeadInfo)
{
    int32_t ret;
    ZlixDeflateContext *context;
    ZlixDeflateLz77DictInfo *dict;
    int oldFlushType;

    if (ZlixDeflateFlushTypeInValid(flushType) == ZLIX_TRUE) {
        return ZLIX_STREAM_ERROR;
    }

    ret = ZlixDeflateStreamInfoCheck(streamInfo);
    if (ret != ZLIX_OK) {
        return ret;
    }

    context = (ZlixDeflateContext *)(streamInfo->context);
    ret = ZlixDeflateContextCheck(context, flushType);
    if (ret != ZLIX_OK) {
        return ret;
    }

    oldFlushType = context->lastFlushType;
    context->lastFlushType = flushType;

    if (context->bufLeftLen != 0) { // 说明deflate上一轮有未输出的
        ZLIX_FLUSH_AND_RETURN_IF_NEEDED(streamInfo, context);
    } else if (ZlixDeflateMeaninglessRepeat(streamInfo, flushType, oldFlushType) == ZLIX_TRUE) {
        return ZLIX_OUTPUT_BUF_INSUFFICENT;
    }

    /* 当且仅当用户上一次压缩时指定Fulsh类型为FINISH,并且所有输入数据被压缩完全
       上下文状态才会被置为STATUS_FINISH,之后的接口调用不允许再传入数据进行压缩 */
    if ((context->status >= ZLIX_STATUS_FINISH) && (streamInfo->availIn != 0)) {
        return ZLIX_FINISH_BUF_REDUNDANT;
    }

    /* 对原缓冲区内容进行一次flush操作后，再写入zlib或者gzip头 */
    ZlixDeflateHeadMake(streamInfo, context, gzHeadInfo);
    /* 写完头后需要及时刷出 */
    if (context->bufLeftLen != 0) {
        ZLIX_FLUSH_AND_RETURN_IF_NEEDED(streamInfo, context);
    }

    if (ZLIX_UNLIKELY(context->easyStoreMode == 1) && (streamInfo->availIn != 0)) { // 纯store模式不处理0字节情况
        ZlixDeflateEasyStore(streamInfo);
        // 将输入作为滑窗,更新crc或adler
        ZlixDeflateWindowIsInput(streamInfo, (context->headFormat == ZLIX_FORMAT_DEFLATE_GZIP),
            (context->headFormat == ZLIX_FORMAT_DEFLATE_ZLIB));
        return ZlixDeflateEndRetHandle(streamInfo, context, flushType, ret);
    }

    dict = context->dictCode;
    ret = ZlixDeflateWinbuffAsInputPrepare(streamInfo, dict, flushType);
    if (ret != ZLIX_OK) {
        return ret;
    }

    do {
        if (context->status >= ZLIX_STATUS_FINISH) {
            break;
        }
        ret = ZlixDeflateBlock(streamInfo, context, flushType);
        if (ret != ZLIX_BLOCK_FINISHED_DONE) {
            break;
        } else {
            ZlixDeflateBlockReset(context);
        }
    } while (ZLIX_DEFLATE_DATA_REMAINING(streamInfo, dict));

    return ZlixDeflateEndRetHandle(streamInfo, context, flushType, ret);
}

/* deflate算法context去初始化 */
ZLIX_STATIC void ZlixDeflateContextFini(ZlixDeflateContext **context)
{
    ZlixDeflateContext *tmpContext = *context;
    ZlixMemFuncs zlixMemOper = tmpContext->zlixMemOper;
    if (tmpContext->outBuf != NULL) {
        ZLIX_LABEL_FREE(zlixMemOper, ZLIX_DEFLATE_OUTBUF_LABEL, tmpContext->outBuf);
        tmpContext->outBuf = NULL;
    }

    if (tmpContext->dictCode != NULL) {
        ZlixDeflateLz77DictInfoFini(&tmpContext->dictCode, zlixMemOper);
    }

    if (tmpContext->dictCodeOut != NULL) {
        ZLIX_LABEL_FREE(zlixMemOper, ZLIX_DEFLATE_LZ77_OUTINFO_LABEL, tmpContext->dictCodeOut);
        tmpContext->dictCodeOut = NULL;
    }

    if (tmpContext->entroyCode != NULL) {
        ZlixHuffmanEntroyCodeInfoFini(&tmpContext->entroyCode, zlixMemOper);
    }

    ZLIX_LABEL_FREE(zlixMemOper, ZLIX_DEFLATE_CONTEXT_LABEL, tmpContext);
    *context = NULL;

    return;
}

/* deflate算法总去初始化接口 */
int32_t ZlixDeflateFini(ZlixStream *streamInfo)
{
    /* streamInfo外部已经判空，此处不判空 */
    if (streamInfo->context != NULL) {
        ZlixDeflateContextFini((ZlixDeflateContext **)&streamInfo->context);
    }

    return ZLIX_OK;
}

CONSTRUCTOR void ZlixDeflateModeInit(void)
{
    ZlixStreamOperFuncs *zlixStreamOprTbl = ZlixStreamOperTblGet();
    zlixStreamOprTbl[ZLIX_DEFLATE].encodeResInit = ZlixDeflateInit;
    zlixStreamOprTbl[ZLIX_DEFLATE].encodeResReset = NULL;
    zlixStreamOprTbl[ZLIX_DEFLATE].encodeStream = ZlixDeflate;
    zlixStreamOprTbl[ZLIX_DEFLATE].encodeResFini = ZlixDeflateFini;
    zlixStreamOprTbl[ZLIX_DEFLATE].encodeBound = NULL;
}
