/**
 * @file zlix_lz77_strategy.c
 * @copyright Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * @brief \n
 * 提供lz77操作基础操作，如滑窗移动、最长匹配、贪婪匹配等
 * @author Anonym
 * @date 2021-10-22
 * @version v0.1.0
 * *******************************************************************************************
 * @par 修改日志：
 * <table>
 * <tr><th>Date        <th>Version  <th>Author    <th>Description
 * <tr><td>2021-10-22  <td>0.1.0    <td>Anonym    <td>提供lz77操作基础操作
 * </table>
 *
 * *******************************************************************************************
 */
#include "zlix_public_inner.h"
#include "zlix_deflate_inner.h"
#include "zlix_window_inner.h"
#include "zlix_utils_inner.h"
#include "zlix_lz77_strategy_inner.h"

/* 符合要求的匹配长度 */
uint8_t g_zlixDeflateLz77GoodLen[ZLIX_LEVEL_OPT] = {0, 4, 4, 4, 4, 8, 8, 8, 32, 32};

/* 优秀的匹配长度 */
uint16_t g_zlixDeflateLz77NiceLen[ZLIX_LEVEL_OPT] = {0, 8, 16, 32, 16, 32, 128, 128, 258, 258};

/* 匹配的最大次数 */
uint16_t g_zlixDeflateLz77ChainLen[ZLIX_LEVEL_OPT] = {0, 4, 8, 32, 16, 32, 128, 256, 1024, 4096};

/* 内存水平的基值，决定hash数组的大小 */
#define ZLIX_DEFLATE_MEMLEVEL_HASH_SHIFT 7

/* 快速匹配时使用每8位进行匹配 */
#define ZLIX_DEFLATE_LZ77_QUICK_MATCH_BIT 8

/* 将bit转换为byte需要的右移量 */
#define ZLIX_DEFLATE_LZ77_BIT_TO_BYTE_SHIFT 3

/* 计算hash值时会读取的字节数 */
#define ZLIX_DEFLATE_HASH_READ_BYTES 4

#define ZLIX_DEFLATE_WINMASK_MAX 32767

ZLIX_STATIC ZLIX_ALWAYS_INLINE uint16_t ZlixDeflateLz77GetHashKey(const uint8_t *curr, uint32_t hashBits)
{
    uint32_t key;

    (void)ReadLE32Bit(curr, &key);
    key = (key * 2654435769U) >> (32 - hashBits);   /* 参考Fibonacci散列法, 32为四字节比特数 */

    return (uint16_t)key;
}

/* 获取2个数据的匹配长度，其中curr为当前待匹配字符串，prev为具有相同hashKey的被匹配字符串，
   matchLenMax为匹配的最大长度（限制匹配长度） */
ZLIX_STATIC ZLIX_ALWAYS_INLINE uint16_t ZlixDeflateLz77GetMatchLen(const uint8_t *curr, const uint8_t *prev,
    uint32_t matchLenMax)
{
    uint64_t xorTmp;
    uint32_t matchLen = 0;
    int32_t matchLenMaxLimit = (int32_t)matchLenMax - ZLIX_DEFLATE_LZ77_QUICK_MATCH_BIT;

    while ((int32_t)matchLen <= matchLenMaxLimit) { /* 8字节读取 */
        /* 通过异或来判断是否相等，若异或值为0，则说明数据相等 */
        xorTmp = READ64BIT(curr + matchLen) ^ READ64BIT(prev + matchLen);
        if (xorTmp == 0) {
            matchLen += ZLIX_DEFLATE_LZ77_QUICK_MATCH_BIT;
            continue;
        }
        /* 通过大小端来判断哪边连接着之前的匹配字段，并将连续的异或值为0的结果右移3位，因为一字节有8位，所以一字节相等则会有8个0 */
        matchLen += IsLE() ? ((uint32_t)ZlixCountTailZero64(xorTmp) >> ZLIX_DEFLATE_LZ77_BIT_TO_BYTE_SHIFT)
                           : ((uint32_t)ZlixCountLeadZero64(xorTmp) >> ZLIX_DEFLATE_LZ77_BIT_TO_BYTE_SHIFT);
        return (uint16_t)matchLen;
    }

    if (((matchLen + 4) <= matchLenMax) && (READ32BIT(curr + matchLen) == READ32BIT(prev + matchLen))) { /* 4字节读取 */
        matchLen += 4;
    }

    if (((matchLen + 2) <= matchLenMax) && (READ16BIT(curr + matchLen) == READ16BIT(prev + matchLen))) { /* 2字节读取 */
        matchLen += 2;
    }

    if ((matchLen < matchLenMax) && (curr[matchLen] == prev[matchLen])) {
        matchLen++;
    }

    return (uint16_t)matchLen;
}

/* 寻找当前字符的最长匹配长度 */
ZLIX_STATIC ZLIX_ALWAYS_INLINE void ZlixDeflateLz77LongestMatch(ZlixDeflateLz77DictInfo *dict)
{
    uint16_t hashkey;
    uint16_t now;
    uint16_t match;
    uint16_t best = 0;
    /* hash链表最多可以连续查询几次（查询少，压缩率低，速率快；查询多，压缩率高，速率慢） */
    uint16_t chainLen = g_zlixDeflateLz77ChainLen[dict->compLevel];
    /* 最大匹配长度 */
    uint16_t limitLen = (dict->lookahead < ZLIX_DEFLATE_LZ77_MAX_MATCH_LEN) ?
        dict->lookahead : ZLIX_DEFLATE_LZ77_MAX_MATCH_LEN;
    /* 最优匹配长度，当匹配长度没有达到时，会尝试继续寻找更好的匹配 */
    uint16_t niceLen = (dict->lookahead < g_zlixDeflateLz77NiceLen[dict->compLevel]) ?
        dict->lookahead : g_zlixDeflateLz77NiceLen[dict->compLevel];
    uint16_t minPos = dict->startPos - ((1 << dict->winbits) - ZLIX_DEFLATE_LZ77_MIN_LOOKAHEAD); /* 被匹配字符串首字符最远可出现位置 */
    uint8_t *prev;
    uint8_t *curr = (uint8_t *)((uintptr_t)dict->winUnion.winBuff + dict->startPos);
    uint16_t winMask = (1 << dict->winbits) - 1;

    /* winbits为16时，使用输入作为滑窗 */
    if (ZLIX_DEFLATE_WINBUFF_IS_INPUT(dict->winbits)) {
        winMask = ZLIX_DEFLATE_WINMASK_MAX;
        minPos = (dict->startPos == 0) ? 1 : ((dict->startPos > winMask) ? (dict->startPos - winMask) : 1);
    }

    /* ZlixDeflateLz77GetHashKey中一次读取4字节，达不到4字节时无法继续进行匹配 */
    if (ZLIX_LIKELY(dict->lookahead >= 4)) {
        /* 计算当前地址得到hashKey，将hash表中hashKey所对应的值（上一个计算得出相同key的地址）存储到hash链中，
           hash链的下标为当前地址，因为地址是唯一的（滑动窗口整体平移时，hash链也会一起平移），所以不会产生冲突；
           最后将当前地址存储到hash表中 */
        hashkey = ZlixDeflateLz77GetHashKey(curr, dict->hashBits);
        now = dict->hash[hashkey];
        dict->hashChain[dict->startPos & winMask] = now;
        dict->hash[hashkey] = dict->startPos;
    } else {
        dict->currMatchLen = 0;
        return;
    }

    if ((dict->compLevel == 1) || (dict->prevLength >= g_zlixDeflateLz77GoodLen[dict->compLevel])) {
        chainLen >>= 2;    /* 右移2位，即为原来的四分之一，level 1下只取一次结果 */
    }

    /* 在hash链上不断查找上一个有同一个hashKey的地址，并判断是否匹配及其匹配长度 */
    while (chainLen > 0 && now >= minPos) {
        prev = (uint8_t *)((uintptr_t)dict->winUnion.winBuff + now);
        match = ZlixDeflateLz77GetMatchLen(curr, prev, limitLen);
        /* 当匹配长度达到当前匹配级别的最优匹配长度时，该匹配即为本次需要的匹配；
           否则继续查找，找到在本次查找范围内最好的匹配长度 */
        if (match >= niceLen) {
            dict->currMatchLen = match;
            dict->currMatchStart = now;
            return;
        } else if (match > best) {
            best = match;
            dict->currMatchStart = now;
        }
        now = dict->hashChain[now & winMask];
        chainLen--;
    }

    dict->currMatchLen = best;
    return;
}

/* 因为匹配字符串在之前的函数中只有首字符存入了hash表中，
   所以需要额外更新hash表，将匹配字符串的剩余字符地址也存入hash表中 */
ZLIX_STATIC ZLIX_ALWAYS_INLINE void ZlixDeflateLz77UpdateHash(ZlixDeflateLz77DictInfo *dict, uint16_t match)
{
    uint8_t *curr = (uint8_t *)((uintptr_t)dict->winUnion.winBuff + dict->startPos + 1);
    uint16_t hashkey;
    uint16_t len = 1;
    uint16_t winMask = (1 << dict->winbits) - 1;
    uint16_t start;
    uint16_t matchEnd;
    if (ZLIX_DEFLATE_WINBUFF_IS_INPUT(dict->winbits)) {
        winMask = ZLIX_DEFLATE_WINMASK_MAX;
    }
    start = dict->startPos & winMask;

    if (match + ZLIX_DEFLATE_HASH_READ_BYTES < dict->lookahead) {
        matchEnd = match;
    } else if (dict->lookahead < ZLIX_DEFLATE_HASH_READ_BYTES) {
        matchEnd = 0;
    } else {
        matchEnd = dict->lookahead - ZLIX_DEFLATE_HASH_READ_BYTES;
    }
    while (len < matchEnd) {
        hashkey = ZlixDeflateLz77GetHashKey(curr++, dict->hashBits);
        dict->hashChain[(uint32_t)(start + len) & winMask] = dict->hash[hashkey];
        dict->hash[hashkey] = dict->startPos + len++;
    }

    /* 输出完成后，将滑动窗口匹配查找指针向后移动，以查找接下来的匹配字符串 */
    dict->startPos += match - 1;
    dict->lookahead -= match - 1;
}

/* 前一个字符没有达到匹配长度要求 */
ZLIX_STATIC ZLIX_ALWAYS_INLINE void ZlixDeflateLz77GreedyNoBestMatch(ZlixDeflateLz77DictInfo *dict,
    ZlixDeflateLz77OutInfo *outInfo, ZlixDeflateNodeInfo *ltree)
{
    uint16_t match = dict->currMatchLen;
    uint8_t lvalue;
    /* 当前字符达到匹配长度要求，先将匹配信息进行保存，与下一个字符进行比较判断 */
    if (match >= ZLIX_DEFLATE_LZ77_MIN_MATCH_LEN) {
        dict->bPrevMatched = ZLIX_TRUE;
        dict->prevLength = match;
        dict->prevMatchStart = dict->currMatchStart;
        return;
    }
    /* 仍没达到匹配长度要求，直接输出当前字符 */
    lvalue = *(uint8_t *)((uintptr_t)dict->winUnion.winBuff + dict->startPos);
    ZLIX_DEFLATE_LZ77_WRITE_LITERAL_OUTPUT(outInfo, lvalue, ltree);
}

/* 前一个字符达到了匹配长度要求，与当前字符进行匹配长度比较 */
ZLIX_STATIC ZLIX_ALWAYS_INLINE void ZlixDeflateLz77GreedyBestMatch(ZlixDeflateLz77DictInfo *dict,
    ZlixDeflateLz77OutInfo *outInfo, ZlixDeflateNodeInfo *ltree, ZlixDeflateNodeInfo *dtree, uint32_t *extraLen)
{
    uint16_t match = dict->currMatchLen;
    uint16_t dvalue;
    uint16_t base;
    uint8_t lvalue;
    if (dict->prevLength >= match) {
        /* 保存上一次匹配的结果 */
        lvalue = (uint8_t)(dict->prevLength - ZLIX_DEFLATE_LZ77_MIN_MATCH_LEN);
        dvalue = dict->startPos - dict->prevMatchStart - 1;
        ZLIX_DEFLATE_LZ77_WRITE_LENDIST_OUTPUT(outInfo, lvalue, ltree, dvalue, dtree, *extraLen, base);
        dict->bPrevMatched = ZLIX_FALSE;
        /* 将匹配长度内的hash更新，因为更新hash表是以当前字符作为起点，
           所以到最终字符的长度需要以当前字符为准，需要将前一字符匹配长度减1 */
        ZlixDeflateLz77UpdateHash(dict, dict->prevLength - 1);
        return;
    }
    /* 上一个作为单个字符输出 */
    lvalue = *(uint8_t *)((uintptr_t)dict->winUnion.winBuff + dict->startPos - 1);
    ZLIX_DEFLATE_LZ77_WRITE_LITERAL_OUTPUT(outInfo, lvalue, ltree);
    dict->prevLength = match;
    dict->prevMatchStart = dict->currMatchStart;
}

/* 贪婪匹配，查看当前字符是否比前一个字符有更长的匹配串 */
ZLIX_STATIC ZLIX_ALWAYS_INLINE void ZlixDeflateLz77GreedyMatch(ZlixDeflateLz77DictInfo *dict,
    ZlixDeflateLz77OutInfo *outInfo, ZlixDeflateNodeInfo *ltree, ZlixDeflateNodeInfo *dtree, uint32_t *extraLen)
{
    uint16_t dvalue;
    uint16_t base;
    uint8_t lvalue;

    if (dict->bPrevMatched == ZLIX_FALSE) {
        ZlixDeflateLz77GreedyNoBestMatch(dict, outInfo, ltree);
        return;
    }
    if (dict->currMatchLen >= ZLIX_DEFLATE_LZ77_MIN_MATCH_LEN) {
        /* 与前一个字符的匹配结果做比较 */
        ZlixDeflateLz77GreedyBestMatch(dict, outInfo, ltree, dtree, extraLen);
        return;
    }
    /* 未匹配将上一次结果输出 */
    lvalue = (uint8_t)(dict->prevLength - ZLIX_DEFLATE_LZ77_MIN_MATCH_LEN);
    dvalue = dict->startPos - dict->prevMatchStart - 1;
    ZLIX_DEFLATE_LZ77_WRITE_LENDIST_OUTPUT(outInfo, lvalue, ltree, dvalue, dtree, *extraLen, base);
    dict->bPrevMatched = ZLIX_FALSE;
    /* 将匹配长度内的hash更新 */
    ZlixDeflateLz77UpdateHash(dict, dict->prevLength - 1);
}

/* 不进行贪婪匹配，直接将当前找到的结果进行输出 */
ZLIX_STATIC ZLIX_ALWAYS_INLINE void ZlixDeflateLz77DirectOutput(ZlixDeflateLz77DictInfo *dict,
    ZlixDeflateLz77OutInfo *outInfo, ZlixDeflateNodeInfo *ltree, ZlixDeflateNodeInfo *dtree, uint32_t *extraLen)
{
    uint8_t lvalue;
    uint16_t dvalue;
    uint16_t base;
    uint16_t match = dict->currMatchLen;

    if (match >= ZLIX_DEFLATE_LZ77_MIN_MATCH_LEN) {
        lvalue = (uint8_t)(match - ZLIX_DEFLATE_LZ77_MIN_MATCH_LEN);
        dvalue = dict->startPos - dict->currMatchStart;
        ZLIX_DEFLATE_LZ77_WRITE_LENDIST_OUTPUT(outInfo, lvalue, ltree, dvalue, dtree, *extraLen, base);
        /* 将匹配长度内的hash更新 */
        ZlixDeflateLz77UpdateHash(dict, match);
        return;
    }
    lvalue = *(uint8_t *)((uintptr_t)dict->winUnion.winBuff + dict->startPos);
    ZLIX_DEFLATE_LZ77_WRITE_LITERAL_OUTPUT(outInfo, lvalue, ltree);
}

/* lz77压缩调用接口 */
int32_t ZlixDeflateLz77(ZlixStream *stream, int32_t flush, uint32_t *extraLen, uint32_t *isLast)
{
    ZlixDeflateContext *context = stream->context;
    ZlixDeflateLz77DictInfo *dict = context->dictCode;
    ZlixDeflateLz77OutInfo *outInfo = context->dictCodeOut;
    ZlixDeflateNodeInfo *lTree = context->entroyCode->dynLTree;
    ZlixDeflateNodeInfo *dTree = context->entroyCode->dynDTree;
    uint32_t minAhead = (flush == ZLIX_NO_FLUSH) ? ZLIX_DEFLATE_LZ77_MIN_LOOKAHEAD : 1;
    uint16_t preStartPos; /* 记录匹配之前的起始位置，来判断查找匹配对过程中扫描的长度 */
    ZLIX_BOOL needCrc = (context->headFormat == ZLIX_FORMAT_DEFLATE_GZIP);
    ZLIX_BOOL needAdler = (context->headFormat == ZLIX_FORMAT_DEFLATE_ZLIB);
    ZLIX_BOOL isZeroBytesInput = ((flush != ZLIX_NO_FLUSH) && (stream->availIn == 0));
    ZLIX_BOOL winbuffIsInput = ZLIX_DEFLATE_WINBUFF_IS_INPUT(dict->winbits);
    if (winbuffIsInput == ZLIX_TRUE) {
        ZlixDeflateWindowIsInput(stream, needCrc, needAdler);
    }

    while (1) {
        /* 滑动窗口加载数据 当输入作为滑窗时，滑窗内没有可以判断的数据时表示全部匹配完毕 */
        if ((winbuffIsInput == ZLIX_TRUE) && (dict->lookahead == 0)) {
            break;
        }
        if ((winbuffIsInput == ZLIX_FALSE) && (dict->lookahead < ZLIX_DEFLATE_LZ77_MIN_LOOKAHEAD)) {
            ZlixDeflateWindowFill(stream, dict, needCrc, needAdler);
            /* 所有输入都处理完时，退出循环 */
            if (dict->lookahead < minAhead) {
                break;
            }
            /* NO_FLUSH场景分析：此处逻辑先与zlib保持一致，lookahead小于MIN_LOOKAHEAD时FillWindow，填充后lookahead仍不够就退出，但会导致一些极端情况
               如窗口内剩余数据正好为MIN_LOOKAHEAD - 1，下次传入一字节也会作为一个块压缩 */
        }
        preStartPos = dict->startPos;
        ZlixDeflateLz77LongestMatch(dict);
        if (dict->compLevel <= 4) { /* level 4以下不进行贪婪匹配 */
            ZlixDeflateLz77DirectOutput(dict, outInfo, lTree, dTree, extraLen);
        } else {
            ZlixDeflateLz77GreedyMatch(dict, outInfo, lTree, dTree, extraLen);
        }
        /* 为防止当前startPos加1后达到65536变为0，最后计算错误，所以需要先进行相减计算storeLen */
        context->storeLen += (uint32_t)(dict->startPos - preStartPos) + 1;
        dict->startPos++;
        dict->lookahead--;
        /* 当ldBuf填满时，直接退出 */
        if (outInfo->count >= outInfo->bufSize) {
            break;
        }
    }

    return ZlixDeflateEndCheck(stream, flush, outInfo, isLast, isZeroBytesInput);
}

/* 使用用户输入作为滑窗，只需要申请hash内存即可 */
int32_t ZlixDeflateLz77DictInfoUpdate(ZlixStream *streamInfo)
{
    ZlixDeflateLz77DictInfo *lz77DictInfo = ((ZlixDeflateContext *)streamInfo->context)->dictCode;
    size_t totalSize = sizeof(uint16_t) * streamInfo->availIn + /* hash冲突链内存，因为distance最多用32K来进行保存，所以在查找冲突时不会超过32K */
        sizeof(uint16_t) * (1 << lz77DictInfo->hashBits); /* hash表内存 */
    void *buf = (void *)ZLIX_LABEL_MALLOC(((ZlixDeflateContext *)streamInfo->context)->zlixMemOper,
        ZLIX_DEFLATE_LZ77_BUF_LABEL, totalSize);
    if (buf == NULL) {
        ZLIX_LABEL_FREE(((ZlixDeflateContext *)streamInfo->context)->zlixMemOper,
            ZLIX_DEFLATE_LZ77_DICT_INFO_LABEL, lz77DictInfo);
        ((ZlixDeflateContext *)streamInfo->context)->dictCode = NULL;
        return ZLIX_MEM_OPER_FAILURE;
    }
    (void)memset_s(buf, totalSize, 0, totalSize);
    lz77DictInfo->winUnion.winStream = streamInfo->nextIn;
    lz77DictInfo->hashChain = (uint16_t *)buf; /* hash链 */
    lz77DictInfo->hash = (uint16_t *)(lz77DictInfo->hashChain + streamInfo->availIn); /* hash链之后紧接着就是hash表 */
    lz77DictInfo->startPos = 0;
    lz77DictInfo->lookahead = (uint16_t)streamInfo->availIn; /* 在初始化时已经对输入长度进行了校验，该情况下不会越界 */
    return ZLIX_OK;
}

/* Lz77执行前的信息初始化，进行了滑动窗口、hash链、hash表的构建，以及压缩水平的赋值。
   其中滑动窗口、hash链、hash表的空间构造在一起 */
ZlixDeflateLz77DictInfo *ZlixDeflateLz77DictInfoInit(int32_t winBits, uint8_t level, uint8_t memLevel,
    ZlixMemFuncs zlixMemOper)
{
    size_t totalSize;
    void *buf;
    uint16_t winSize;    /* LZ77 缓冲区长度 (32K by default) */
    uint32_t dWinSize;

    ZlixDeflateLz77DictInfo *lz77DictInfo = (ZlixDeflateLz77DictInfo *)ZLIX_LABEL_MALLOC(zlixMemOper,
        ZLIX_DEFLATE_LZ77_DICT_INFO_LABEL, sizeof(ZlixDeflateLz77DictInfo));
    if (lz77DictInfo == NULL) {
        return NULL;
    }

    lz77DictInfo->hashBits = memLevel + ZLIX_DEFLATE_MEMLEVEL_HASH_SHIFT; /* 用户默认配置8 + 7，即15 */

    /* 一次性申请滑动窗口，hash和hashChain所用的内存，再依次切割 */
    if (!ZLIX_DEFLATE_WINBUFF_IS_INPUT(winBits)) {
        winSize = 1 << winBits;
        dWinSize = winSize << 1;
        /* 无溢出风险 */
        totalSize = sizeof(uint8_t) * (dWinSize) + /* 滑窗内存64K，分为左右2侧 */
            sizeof(uint16_t) * winSize + /* hash冲突链内存，因为distance最多用32K来进行保存，所以在查找冲突时不会超过32K */
            sizeof(uint16_t) * (1 << lz77DictInfo->hashBits); /* hash表内存 */
        buf = (void *)ZLIX_LABEL_MALLOC(zlixMemOper, ZLIX_DEFLATE_LZ77_BUF_LABEL, totalSize);
        if (buf == NULL) {
            ZLIX_LABEL_FREE(zlixMemOper, ZLIX_DEFLATE_LZ77_DICT_INFO_LABEL, lz77DictInfo);
            return NULL;
        }
        (void)memset_s(buf, totalSize, 0, totalSize);
        lz77DictInfo->winUnion.winBuff = (uint8_t *)buf;
        lz77DictInfo->hashChain = (uint16_t *)((uintptr_t)buf + (dWinSize)); /* 滑窗之后紧接着就是hash链 */
        lz77DictInfo->hash = (uint16_t *)(lz77DictInfo->hashChain + winSize); /* hash链之后紧接着就是hash表 */
        lz77DictInfo->startPos = winSize; /* 从右侧窗口开始 */
        lz77DictInfo->lookahead = 0;
    } else {
        lz77DictInfo->hashChain = NULL;
    }

    lz77DictInfo->winbits = (uint16_t)winBits;
    lz77DictInfo->currMatchStart = 0;
    lz77DictInfo->currMatchLen = 0;
    lz77DictInfo->prevMatchStart = 0;
    lz77DictInfo->prevLength = 0;
    lz77DictInfo->bPrevMatched = ZLIX_FALSE;
    lz77DictInfo->compLevel = level;

    return lz77DictInfo;
}

ZlixDeflateLz77OutInfo *ZlixDeflateLz77DictOutInfoInit(uint8_t memLevel, uint8_t *outBuf, uint32_t bufSize,
    ZlixMemFuncs zlixMemOper)
{
    ZlixDeflateLz77OutInfo *lz77Out;

    lz77Out = (ZlixDeflateLz77OutInfo *)ZLIX_LABEL_MALLOC(zlixMemOper,
        ZLIX_DEFLATE_LZ77_OUTINFO_LABEL, sizeof(ZlixDeflateLz77OutInfo));
    if (lz77Out == NULL) {
        return NULL;
    }
    lz77Out->bufSize = 1 << (memLevel + 6);  /* 内部缓冲区的4分之一，即1 << (8 + 6) = 16K */
    lz77Out->count = 0;
    lz77Out->literLenBuf = (uint8_t *)((uintptr_t)outBuf + bufSize - (sizeof(uint8_t) * lz77Out->bufSize));
    lz77Out->disBuf = (uint16_t *)((uintptr_t)lz77Out->literLenBuf - (sizeof(uint16_t) * lz77Out->bufSize));
    return lz77Out;
}

void ZlixDeflateLz77DictInfoFini(ZlixDeflateLz77DictInfo **lz77DictInfo, ZlixMemFuncs zlixMemOper)
{
    ZlixDeflateLz77DictInfo *tmpLz77DictInfo = *lz77DictInfo;

    if (tmpLz77DictInfo == NULL) {
        return;
    }

    if (ZLIX_DEFLATE_WINBUFF_IS_INPUT(tmpLz77DictInfo->winbits)) {
        if (tmpLz77DictInfo->hashChain != NULL) {
            ZLIX_LABEL_FREE(zlixMemOper, ZLIX_DEFLATE_LZ77_BUF_LABEL, tmpLz77DictInfo->hashChain);
        }
    } else {
        if (tmpLz77DictInfo->winUnion.winBuff != NULL) {
            ZLIX_LABEL_FREE(zlixMemOper, ZLIX_DEFLATE_LZ77_BUF_LABEL, tmpLz77DictInfo->winUnion.winBuff);
        }
    }
    ZLIX_LABEL_FREE(zlixMemOper, ZLIX_DEFLATE_LZ77_DICT_INFO_LABEL, tmpLz77DictInfo);

    *lz77DictInfo = NULL;
    return;
}
