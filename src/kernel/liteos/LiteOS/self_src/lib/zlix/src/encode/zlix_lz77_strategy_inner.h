/**
 * @file zlix_lz77_strategy.h
 * @copyright Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * @brief zlix_lz77 基础操作内部头文件
 * @details zlix_lz77 基础操作内部头文件，包括滑窗操作、最长匹配等
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

#ifndef ZLIX_LZ77_INNER_H
#define ZLIX_LZ77_INNER_H

#include "drb_zlix.h"
#include "zlix_deflate_public_inner.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ZLIX_DEFLATE_BLOCK_THIS_TIME 0
#define ZLIX_DEFLATE_BLOCK_NEXT_TIME 1

/* 匹配长度的下限 */
#define ZLIX_DEFLATE_LZ77_MIN_MATCH_LEN 3

/* 匹配长度的上限 */
#define ZLIX_DEFLATE_LZ77_MAX_MATCH_LEN 258

/* 窗口中最小剩余数据量 MAX_MATCH(258) + MIN_MATCH(3) + 1 */
#define ZLIX_DEFLATE_LZ77_MIN_LOOKAHEAD 262

/* 新设定，winbits为16时用输入作为滑窗 */
#define ZLIX_DEFLATE_WINBUFF_IS_INPUT(winbits) ((winbits) == 16)

/* 写入字符信息到临时缓冲区中 */
#define ZLIX_DEFLATE_LZ77_WRITE_LITERAL_OUTPUT(outInfo, lvalue, ltree) do {                          \
    (outInfo)->literLenBuf[(outInfo)->count] = (lvalue);                                             \
    (outInfo)->disBuf[(outInfo)->count++] = 0;                                                       \
    (ltree)[lvalue].code.nodeFreq++;                                                                 \
} while (0)

/* 写入长度距离信息到临时缓冲区中 */
#define ZLIX_DEFLATE_LZ77_WRITE_LENDIST_OUTPUT(outInfo, lvalue, ltree, dvalue, dtree, extraLen, base) do { \
    (outInfo)->literLenBuf[(outInfo)->count] = (lvalue);                                                   \
    (base) = ZLIX_DEFLATE_GET_LENGTH_BASE(lvalue);                                                         \
    (ltree)[(base) + ZLIX_LITERALS_EXTRA_BASE].code.nodeFreq++;                                            \
    (extraLen) += g_lExtraBits[base];                                                                      \
    (outInfo)->disBuf[(outInfo)->count++] = (dvalue);                                                      \
    (base) = ZLIX_DEFLATE_GET_DIST_BASE((dvalue) - 1);                                                     \
    (dtree)[base].code.nodeFreq++;                                                                         \
    (extraLen) += g_dExtraBits[base];                                                                      \
} while (0)

typedef struct {
    uint16_t *hashChain; /* 用于解决hash冲突的hash
    chain，长度和查找缓冲区长度一致，注意，HashChain的类型为unsigned short。
    数组下标由当前待处理的数据的首字符在窗口的位置startPos生成，数组值为上一个相同hash码的首字符在窗口的位置 */
    uint16_t *hash; /* hash表数组，数组下标为以当前待处理的数据前3字符计算生成的hash码，
                           数组值为当前待处理的数据的首字符在窗口的位置 */
    union {
        uint8_t *winBuff;    /* 滑窗的起始地址 */
        const uint8_t *winStream; /* 使用外部输入作为滑窗 */
    } winUnion;
    uint16_t winbits;    /* 范围为9 - 15，winSize = 1 << winbits; */
    uint16_t hashBits; /* hash码长度，取决于用户设置的内存使用水平，16位操作系统最大为15，32位或64位操作系统最大为16 */
    uint16_t startPos;         /* 当前待处理的数据的首字符在窗口的位置 */
    uint16_t lookahead;        /* 滑窗内当前待处理的数据的总量 */
    uint16_t compLevel;        /* 压缩水平 */
    uint16_t currMatchStart;   /* 当前字符的最佳匹配字符串的对应字典起始位置 */
    uint16_t currMatchLen;     /* 当前字符串匹配长度 */
    uint16_t prevMatchStart;   /* 当前字符的前一字符的最佳匹配字符串的对应字典起始位置 */
    uint16_t prevLength;       /* 当前字符的前一字符的匹配长度 */
    ZLIX_BOOL bPrevMatched;     /* 当前字符的前一字符是否存在最佳匹配长度 */
} ZlixDeflateLz77DictInfo;

typedef struct  {
    uint16_t *disBuf; /* 写距离数据的缓冲区，距离的取值范围为[0, 32768]，所以需要2个字节 */
    uint8_t *literLenBuf; /* 写字符或者匹配长度数据的缓冲区 */
    uint32_t bufSize; /* buf的长度， literLenBuf的长度和disBuf的长度一致，注意，disBuf的元素是2字节 */
    uint32_t count;   /* buf内目前装载的匹配数据对个数 */
} ZlixDeflateLz77OutInfo;

int32_t ZlixDeflateLz77(ZlixStream *stream, int32_t flush, uint32_t *extraLen, uint32_t *isLast);

int32_t ZlixDeflateLiteralOnly(ZlixStream *stream, int32_t flush, uint32_t *extraLen, uint32_t *isLast);

int32_t ZlixDeflateRleAsMatch(ZlixStream *stream, int32_t flush, uint32_t *extraLen, uint32_t *isLast);

int32_t ZlixDeflateLz77DictInfoUpdate(ZlixStream *streamInfo);

ZlixDeflateLz77DictInfo *ZlixDeflateLz77DictInfoInit(int32_t winBits, uint8_t level, uint8_t memLevel,
    ZlixMemFuncs zlixMemOper);

ZlixDeflateLz77OutInfo *ZlixDeflateLz77DictOutInfoInit(uint8_t memLevel, uint8_t *outBuf, uint32_t bufSize,
    ZlixMemFuncs zlixMemOper);

void ZlixDeflateLz77DictInfoFini(ZlixDeflateLz77DictInfo **lz77DictInfo, ZlixMemFuncs zlixMemOper);

#ifdef __cplusplus
}
#endif

#endif
