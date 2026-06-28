/**
 * @file zlix_deflate_public_inner.h
 * @copyright Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * @brief zlix deflate算法内部公共头文件
 * @details zlix deflate算法内部公共头文件，包括压缩与解压通用的数据、结构等
 * @author Anonym
 * @date 2021-12-13
 * @version v0.1.0
 * *******************************************************************************************
 * @par 修改日志：
 * <table>
 * <tr><th>Date        <th>Version  <th>Author       <th>Description
 * <tr><td>2023-04-12  <td>0.1.0    <td>Anonym       <td>Initial Version
 * </table>
 * *******************************************************************************************
 */

#ifndef ZLIX_DEFLATE_PUBLIC_INNER_H
#define ZLIX_DEFLATE_PUBLIC_INNER_H

#include "drb_zlix.h"
#include "zlix_public_inner.h"

#ifdef __cplusplus
extern "C" {
#endif

/* gzip头id魔术字 */
#define ZLIX_GZIP_FLAG_ID_MAGIC 0x8b1f

/* 字符的编码区间映射范围 0 - 255 */
#define ZLIX_LITERALS_CODE_RANGE  256

/* 字符的编码区间映射基础值 */
#define ZLIX_LITERALS_EXTRA_BASE  257

/* 距离的编码区间映射基础值 */
#define ZLIX_DIST_EXTRA_BASE  0

/* 动态编码匹配长度的编码区间范围为257 - 285，静态L树符号范围为0 - 287，对应匹配长度的编码区间范围为257 - 287，取两者较大值 */
#define ZLIX_MATCHLEN_CODE_RANGE  31

/* 字符/长度的huffman树的节点值范围 0-285 */
#define ZLIX_LIT_LEN_CODE_RANGE 286

/* 距离（编码区间）范围 0 - 29 */
#define ZLIX_DISTANCE_CODE_RANGE  30

/* CCL的编码数值范围 0 - 18 */
#define ZLIX_TREE_CODE_RANGE  19

/* 头信息字段长度 */
#define ZLIX_HEAD_LEN 3

/* 头信息字段中，STORED标志为0 */
#define STORED_HEAD_FLAG 0

/* 头信息字段中，静态huffman树标志为1 */
#define FIXED_HUFF_HEAD_FLAG 1

/* 头信息字段中，动态huffman树标志为2 */
#define DYNAMIC_HUFF_HEAD_FLAG 2

/* 头信息字段中，块被破坏的标志为3 */
#define DESTROYED_HEAD_FLAG 3

/* HDIST的字段长度 */
#define ZLIX_HDIST_LEN 5

/* HLIT的字段长度 */
#define ZLIX_HLIT_LEN 5

/* HCCL的字段长度 */
#define ZLIX_HCCL_LEN 4

/* 解压的字典窗口bit上限 */
#define ZLIX_INFLATE_WINBITS_HIGH 15

/* 解压的字典窗口bit下限 */
#define ZLIX_INFLATE_WINBITS_LOW 8

/* 静态huffman树的4种编码长度 */
#define ZLIX_STATIC_CODELEN_TYPE 4

/* 静态huffman 长度/字符树的一级表阈值 */
#define ZLIX_STATIC_LIT_MAX_LEN 9

/* 静态huffman 距离树的一级表阈值 */
#define ZLIX_STATIC_DIST_MAX_LEN 5

/* 静态huffman 长度/字符树的区间长度 */
#define ZLIX_STATIC_LIT_MAX_RANGE 287

/* CL编码长度上限值 */
#define ZLIX_MAX_CODE_BITS_LEN 15

/* CCL编码长度上限值 */
#define ZLIX_MAX_CODE_BITS_LEN_BL 7

/* 统计编码长度的数组长度 */
#define ZLIX_BL_CNT_LEN (ZLIX_MAX_CODE_BITS_LEN + 1)

/* 动态huffman编码的CL1序列和CL2序列的总长度上限 */
#define ZLIX_DYNAMIC_CL1_AND_CL2_LEN (ZLIX_LIT_LEN_CODE_RANGE + ZLIX_DISTANCE_CODE_RANGE)

/* 长度映射区间的extra bit长度，考虑公式化表达 */
extern const uint8_t g_lExtraBits[ZLIX_MATCHLEN_CODE_RANGE];

/* 距离映射区间的extra bit长度，考虑公式化表达 */
extern const uint8_t g_dExtraBits[ZLIX_DISTANCE_CODE_RANGE];

/* 游程编码序列 */
extern const uint8_t g_cclQueue[ZLIX_TREE_CODE_RANGE];

/* 游程编码 16 17 18的 映射区间的extra bit长度，考虑公式化表达 */
extern const uint8_t g_bExtraBits[ZLIX_TREE_CODE_RANGE];

/* 游程编码 16 17 18的 映射区间的基值，考虑公式化表达 */
extern const uint8_t g_bExtraLenBase[ZLIX_TREE_CODE_RANGE];

/* 长度的区间节点基值 */
extern const uint16_t g_baseLength[ZLIX_MATCHLEN_CODE_RANGE];

/* 距离的区间节点基值 */
extern const uint16_t g_baseDist[ZLIX_DISTANCE_CODE_RANGE];

typedef struct tagZlixFixedSeg {
    uint16_t beginPos;
    uint16_t endPos;
} ZLIX_FIXED_SEGMENT;

/* 将不同编码长度的区间进行分段，对应区间的编码长度对应 g_staticCodeLen， 2代表起止两个端 */
extern const uint16_t g_staticCodeRange[ZLIX_STATIC_CODELEN_TYPE][2];

/* 4个区间的编码长度，分别为 7 8 8 9 */
extern const uint8_t g_staticCodeLen[ZLIX_STATIC_CODELEN_TYPE];

/* 4个不同编码长度的分隔区间，按照区间的值大小对区间进行排列 */
extern const ZLIX_FIXED_SEGMENT g_fixedSeg[ZLIX_STATIC_CODELEN_TYPE];

/* 定义huffman树的节点信息: 同时在lz77统计时也可用到 */
typedef struct {
    union {
        uint16_t nodeFreq;             /* 当前区间节点出现的频率 */
        uint16_t nodeCode;             /* 当前区间节点的编码 */
    } code;

    union {
        uint16_t nodeParent;           /* 当前区间编码的父节点 */
        uint16_t rangeCode;            /* 当前区别节点值的编码长度 */
    } len;
} ZlixDeflateNodeInfo;

/*
 * ZlixInflateReverse最大仅支持翻转16比特
 */
ZLIX_STATIC ZLIX_ALWAYS_INLINE uint32_t ZlixInflateReverse(uint32_t code, const uint16_t len)
{
    uint32_t a = code;
    a = ((a & 0xAAAA) >> 1) | ((a & 0x5555) << 1); /* 相隔1位交换，0xAAAA每4位为1010,0x5555每4位为0101 */
    a = ((a & 0xCCCC) >> 2) | ((a & 0x3333) << 2); /* 相隔2位交换，0xCCCC每4位为1100,0x3333每4位为0011 */
    a = ((a & 0xF0F0) >> 4) | ((a & 0x0F0F) << 4); /* 相隔4位交换，0xF0F0每8位为11110000,0x0F0F每4位为00001111 */
    a = ((a & 0xFF00) >> 8) | ((a & 0x00FF) << 8); /* 相隔8位交换，0xFF00前16位全为1、后16位全为0，0x00FF前16位全为0、后16位全为1 */
    return a >> (16 - len); /* 已交换后16位，改为交换后len位 */
}

ZLIX_STATIC ZLIX_ALWAYS_INLINE int ZlixCountTailZero64(uint64_t x)
{
#if (defined(__GNUC__) && (__GNUC__ >= 3))
    return __builtin_ctzll(x);
#elif defined(_MSC_VER) && (defined(_M_X64) || defined(_M_I86) && defined(__AVX2__))
    return _tzcnt_u64(x);
#elif defined(_MSC_VER) && defined(_WIN64)
    unsigned long ret;
    _BitScanForward64(&ret, x);
    return (int)ret;
#elif defined(_MSC_VER)
    unsigned long r;
    if (x & 0xffffffff) { /* 0xffffffff用来判断x在低32位是否有值 */
        _BitScanForward(&r, (unsigned long)x);
    } else {
        _BitScanForward(&r, x >> 32); /* 取高32位 */
        r += 32; /* 低32位均为0 */
    }

    return (int)r;
#else
    uint64_t val = x;
    int num = 0;
    if (x == 0) {
        return 0;
    }

    while ((val & 1) == 0) {
        val >>= 1;
        num++;
    }

    return num;
#endif
}

ZLIX_STATIC ZLIX_ALWAYS_INLINE int ZlixCountLeadZero64(uint64_t x)
{
#if (defined(__GNUC__) && (__GNUC__ >= 3))
    return __builtin_clzll(x);
#elif defined(_MSC_VER) && (defined(_M_X64) || defined(_M_I86) && defined(__AVX2__))
    return _lzcnt_u64(x);
#elif defined(_MSC_VER) && defined(_WIN64)
    unsigned long r;
    _BitScanReverse64(&r, x);
    return (int)r;
#elif defined(_MSC_VER)
    unsigned long r;
    if (x & 0xffffffff00000000ULL) { /* 0xffffffff00000000ULL用来判断x在高32位上是否有值 */
        _BitScanReverse(&r, (unsigned long)(x >> 32)); /* 取低32位 */
        r = 31 - r; /* 这里是31减前置1的位数 */
    } else {
        _BitScanReverse(&r, (unsigned long)x);
        r = 63 - r; /* 这里是63减前置1的位数 */
    }

    return (int)r;
#else
    int num = 0;
    uint64_t val = x;
    if (x == 0) {
        return 0;
    }

    while ((val & 0x8000000000000000ULL) == 0) { /* 0x8000000000000000ULL用来求val最高位（第31位） */
        val <<= 1;
        num++;
    }

    return num;
#endif
}

ZLIX_STATIC ZLIX_ALWAYS_INLINE int HighBit(uint32_t x)
{
#if (defined(__GNUC__) && (__GNUC__ >= 3))
    return (int)((uint32_t)(__builtin_clz(x)) ^ 31u); /* 获得前导0的个数后异或31就是前导1的位数 */
#elif defined(_MSC_VER)
    unsigned long r;
    _BitScanReverse(&r, x);
    return (int)r;
#else
    /* 二分法求高bit */
    int pos = 32;
    uint32_t val = x;

    if (!val) {
        return -1;
    }
    if (!(val & 0xFFFF0000)) { /* 0xFFFF0000检测val高16位是否全为0 */
        val <<= 16; /* 左移16位取低位 */
        pos -= 16;
    }
    if (!(val & 0xFF000000)) { /* 0xFF000000检测val高8位是否全为0 */
        val <<=  8; /* 左移8位取低位 */
        pos -= 8;
    }
    if (!(val & 0xF0000000)) { /* 0xF0000000检测val高4位是否全为0 */
        val <<=  4; /* 左移4位取低位 */
        pos -= 4;
    }
    if (!(val & 0xC0000000)) { /* 0xC0000000检测val高2位是否全为0 */
        val <<=  2; /* 左移2位取低位 */
        pos -= 2;
    }
    if (!(val & 0x80000000)) { /* 0x80000000检测val高1位是否为0 */
        val <<=  1;
        pos -= 1;
    }

    return pos - 1;
#endif
}

#ifdef __cplusplus
}
#endif

#endif
