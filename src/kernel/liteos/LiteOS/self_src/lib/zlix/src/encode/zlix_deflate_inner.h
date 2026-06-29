/**
 * @file zlix_deflate_inner.h
 * @copyright Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * @brief zlix deflate算法内部头文件
 * @details zlix deflate算法内部头文件，包括初始化、压缩、去初始化等
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

#ifndef ZLIX_DEFLATE_INNER_H
#define ZLIX_DEFLATE_INNER_H

#include "drb_zlix.h"
#include "zlix_public_inner.h"
#include "zlix_lz77_strategy_inner.h"
#include "zlix_huffman_inner.h"
#include "zlix_store_inner.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 用于长度转换至符号的数组大小，即MAX_MATCH(258) - MIN_MATCH(3) + 1 */
#define ZLIX_DEFLATE_LENGTH_CODE_LEN 256

/* 用于距离转换至符号的数组大小 */
#define ZLIX_DEFLATE_DIST_CODE_LEN 512

/* 按bit进行输出操作的相关宏 */
#define ZLIX_DEFLATE_BIT_BUF_SIZE 48

/* 长度转换至符号值 */
extern uint8_t g_lenToCode[ZLIX_DEFLATE_LENGTH_CODE_LEN];

/* 距离转换至符号值 */
extern uint8_t g_distToCode[ZLIX_DEFLATE_DIST_CODE_LEN];

#define ZLIX_DEFLATE_GET_LENGTH_BASE(length) (g_lenToCode[length])
#define ZLIX_DEFLATE_GET_DIST_BASE(dist) \
    (((dist) < 256) ? (g_distToCode[(dist)]) : (g_distToCode[256 + ((dist) >> 7)]))

/* 将bit缓冲区中的内容按字节输出值pending中 */
#define ZLIX_DEFLATE_FLUSH_BYTE_TO_PENDING(context) do {                                   \
    (context)->outBuf[(context)->pending++] = (uint8_t)((context)->bitBuf & 0xff);         \
    (context)->bitBuf >>= 8;                                                               \
} while (0)

/* 将bit内容输出至bit缓冲区中 */
#define ZLIX_DEFLATE_WRITE_BITS(context, value, bits) do {                                         \
    if ((bits) > 0) {                                                                              \
        (context)->bitBuf |=  ((uint64_t)(value) << (context)->usedBitCnt);                          \
        (context)->usedBitCnt += (bits);                                                           \
        if ((context)->usedBitCnt >= 48) {                                                         \
            ZLIX_DEFLATE_FLUSH_BYTE_TO_PENDING(context);                                           \
            ZLIX_DEFLATE_FLUSH_BYTE_TO_PENDING(context);                                           \
            ZLIX_DEFLATE_FLUSH_BYTE_TO_PENDING(context);                                           \
            ZLIX_DEFLATE_FLUSH_BYTE_TO_PENDING(context);                                           \
            ZLIX_DEFLATE_FLUSH_BYTE_TO_PENDING(context);                                           \
            ZLIX_DEFLATE_FLUSH_BYTE_TO_PENDING(context);                                           \
            (context)->usedBitCnt -= ZLIX_DEFLATE_BIT_BUF_SIZE;                                    \
        }                                                                                          \
    }                                                                                              \
} while (0)

/* bitBuf中剩余内容先按字节对齐输出 */
#define ZLIX_DEFLATE_FLUSH_BITBUF(context) do {                                                           \
    while ((context)->usedBitCnt >= 8) {                                                                  \
        (context)->outBuf[(context)->pending++] = (uint8_t)(((context)->bitBuf) & 0xff);                  \
        (context)->bitBuf >>= 8;                                                                          \
        (context)->usedBitCnt -= 8;                                                                       \
    }                                                                                                     \
    if ((context)->usedBitCnt > 0) {                                                                      \
        (context)->outBuf[(context)->pending++] = (uint8_t)((context)->bitBuf);                           \
    }                                                                                                     \
    (context)->bitBuf = 0;                                                                                \
    (context)->usedBitCnt = 0;                                                                            \
} while (0)

#define ZLIX_DEFLATE_FLUSH_BITBUF_PARTIAL(context) do {                                                   \
    while ((context)->usedBitCnt >= 8) {                                                                  \
        (context)->outBuf[(context)->pending++] = (uint8_t)(((context)->bitBuf) & 0xff);                  \
        (context)->bitBuf >>= 8;                                                                          \
        (context)->usedBitCnt -= 8;                                                                       \
    }                                                                                                     \
} while (0)

/* 定义zlix压缩/解压缩上下文信息 */
struct ZlixDeflateContextInfo {
    ZlixMemFuncs zlixMemOper;                   /* 内存申请函数 */
    ZlixHeadFormat headFormat;                  /* 头格式信息 */
    ZlixStatus status;                          /* 定义压缩状态信息 */
    uint32_t easyStoreMode;                     /* 是否开启纯store模式 */
    int32_t lastFlushType;                      /* 最后flash类型 */
    ZLIX_BOOL matchWithoutStore;                /* 如果前一个block有数据残留，则需要将其记录到store块长度的计算中 */
    uint8_t *outBuf;                            /* 内部缓冲区起始地址 */
    uint32_t pending;                           /* 内部缓冲区中已写入的长度 */
    uint8_t *pendingBuf;                        /* 内部缓冲区中当前未读数据的起始地址 */
    uint32_t bufSize;                           /* 内部缓冲区长度 */
    uint32_t bufLeftLen;                        /* 内部缓冲区未刷新到外部缓冲区的长度 */
    uint64_t bitBuf;                            /* 压缩块时，以bit为单位填充数据，一次可能填充的最大bit数为15(编码长度)，
                                                   使用64bit作为bit流缓冲区, 缓冲区中bit数超过48时，以8bit(1字节)为单位，赋值内部缓冲区 */
    uint16_t usedBitCnt;                        /* bit流缓冲区中已经占用的bit位数(最低有效位开始) */
    /*  压缩策略，参见ZlixEncodeStrategy--
    Z_FILTERED
    用于由filter生成的数据（包含很多小的随机数据），这种情况下，压缩算法能够获得更好的压缩效果。强制更多的哈夫曼编码和更少的lz77字典编码。
    Z_HUFFMAN_ONLY  不做字符匹配，仅进行huffman编码
    Z_RLE           游程编码，distance为1,也就是前面一个字符重复的长度为len ，然后huffman编码
    Z_FIXED         静态树编码，不需要根据统计的频率建动态huffman树，使用固化的Z_DEFAULT_STRATEGY
    LZ77字典编码加上huffman动态树编码
    */
    uint16_t strategy;
    uint32_t storeLen;                          /* 采用store格式所需的编码长度，单位为字节 */
    uint32_t forceStore;                        /* 是否为level 0下强制使用store格式的标志位，字节对齐下占四字节 */
    uint32_t extraLen;                          /* huffman所需的extraBits，单位为比特 */
    ZlixDeflateLz77DictInfo *dictCode;          /* 根据不同的算法，使用时可以被强转为不同的类型，比如deflate的使用
                                                   ZlixDeflateLz77DictInfo */
    ZlixDeflateLz77OutInfo *dictCodeOut;        /* 根据不同的算法，使用时可以被强转为不同的类型，比如deflate的使用
                                                   ZlixDeflateLz77OutInfo */
    ZlixDeflateHuffmanCodeInfo *entroyCode;     /* 根据不同的算法，使用时可以被强转为不同的类型，比如deflate的使用
                                                   ZlixDeflateHuffmanCodeInfo */
};

typedef enum {
    ZLIX_BLOCK_NEED_INPUT,          /* 所有输入已处理，需要更多的输入 */
    ZLIX_BLOCK_NEED_OUTPUT,         /* 输出内存不够，缓冲区中有残留的未输出数据 */
    ZLIX_BLOCK_FINISHED_DONE,       /* 块中所有输入处理完毕并输出 */
} ZlixDeflateBlockState;

void ZlixDeflateBlockReset(ZlixDeflateContext *context);
int32_t ZlixDeflateBlockFlushPending(ZlixStream *stream, ZlixDeflateContext *context);
int32_t ZlixDeflateEndCheck(ZlixStream *stream, int32_t flush, ZlixDeflateLz77OutInfo *outInfo,
    uint32_t *isLast, ZLIX_BOOL isZeroBytesInput);

#ifdef __cplusplus
}
#endif

#endif
