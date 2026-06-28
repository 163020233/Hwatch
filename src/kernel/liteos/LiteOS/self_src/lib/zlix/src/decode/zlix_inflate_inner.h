/**
 * @file zlix_inflate_inner.h
 * @copyright Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * @brief zlix inflate算法内部头文件
 * @details zlix inflate算法内部头文件，包括初始化、解压缩、去初始化等
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

#ifndef ZLIX_INFLATE_INNER_H
#define ZLIX_INFLATE_INNER_H

#include "drb_zlix.h"
#include "zlix_public_inner.h"
#include "zlix_deflate_public_inner.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 当轮inflate操作需要返回 */
#define ZLIX_INFLATE_LEAVE 1

/* 当轮inflate操作不需要返回 */
#define ZLIX_INFLATE_NOT_LEAVE 0

/* 字符/长度码表长度所需的最大值 */
#define ZLIX_ENOUGH_LENS 852

/* 距离码表长度所需的最大值 */
#define ZLIX_ENOUGH_DISTS 592

/* CCL一级码表阈值 */
#define ZLIX_CCL_THRESHOLD 7

/* 字符/长度一级码表阈值 */
#define ZLIX_LEN_THRESHOLD 9

/* 动态树距离一级码表阈值 */
#define ZLIX_DIST_THRESHOLD_DYN 6

/* 静态树距离一级码表阈值 */
#define ZLIX_DIST_THRESHOLD_FIX 5

typedef struct tagCodeInfo {
    uint16_t code;      /* 最长只有15的编码长度，2^15 */
    uint16_t val;       /* 区间值 */
    uint8_t codeLen;    /* 区间编码长度 */
} CODE_INFO;

/*  op == 0000 && exBits == 0, rangebase 指的是字符值
    op == 0000 && exBits != 0, exBits用于存储二级码表的层数，以一级码表的根节点为起始层数
    op == 0001, exBits用于存储extra bits个数，rangebase存储符号对应的区间基值, bits代表码长度
    op == 0110 && exBits == 0, 结束符
    op == 1000 非法值
*/
typedef struct tagcodeTableinfo {
    uint8_t bits;                /* 区间编码长度，二级码表的编码长度必须减掉一级码表的阈值 */
    uint8_t op : 4;              /* 具体含义参见上注释 */
    uint8_t exBits : 4;
    uint16_t rangebase;          /* 节点的基值 rangebase，结点存储二级码表信息时代表二级码表根节点所在的位置 */
} CODE_TABLE_INFO;

typedef enum {
    ZLIX_INFLATE_HEAD,       /* 等待输入：魔术字头 */
    ZLIX_INFLATE_FLAGS,
    ZLIX_INFLATE_TIME,
    ZLIX_INFLATE_OS,
    ZLIX_INFLATE_EXLEN,
    ZLIX_INFLATE_EXTRA,
    ZLIX_INFLATE_NAME,
    ZLIX_INFLATE_COMMENT,
    ZLIX_INFLATE_HCRC,
    ZLIX_INFLATE_DICTID,
    ZLIX_INFLATE_DICT,
    ZLIX_INFLATE_TYPE,       /* 等待输入： block类型头 */
    ZLIX_INFLATE_TYPEDO,     /* 等待输入： 同上，但是直接结束解压流程 */
    ZLIX_INFLATE_STORED,     /* 等待输入： stored大小 */
    ZLIX_INFLATE_COPY,       /* 等待输入或者输出来拷贝store块 */
    ZLIX_INFLATE_TABLELEN,   /* 等待输入：动态块的CL序列长度字段 */
    ZLIX_INFLATE_TABLECCL,   /* 等待输入：CCL序列字段 */
    ZLIX_INFLATE_CODECL,     /* 等待输入：CL1和CL2序列字段 */
    ZLIX_INFLATE_LEN_FAST,   /* 等待输入：长度/字符、块结束符字段 */
    ZLIX_INFLATE_LEN,        /* 等待输入：长度/字符、块结束符字段 */
    ZLIX_INFLATE_LENEXT,     /* 等待输入：长度的extra bits字段 */
    ZLIX_INFLATE_DIST,       /* 等待输入：距离字段 */
    ZLIX_INFLATE_DISTEXT,    /* 等待输入: 距离的extra bits字段 */
    ZLIX_INFLATE_MATCH,      /* 等待输出空间来拷贝匹配结果 */
    ZLIX_INFLATE_LIT,        /* 等待输出空间来写入字符 */
    ZLIX_INFLATE_CHECK,      /* 等待输入: 检查block块的最后32字节内容 */
    ZLIX_INFLATE_LENGTH,
    ZLIX_INFLATE_DONE,       /* 检查结束，停留在此状态直到reset */
    ZLIX_INFLATE_BAD,        /* 数据error，停留在此状态直到reset */
    ZLIX_INFLATE_MEM,        /* 内存相关error，停留在此状态直到reset */
} ZLIX_INFLATE_MODE;

typedef struct zlix_inflate_state {
    ZlixStream *stream;          /* 指向对应的压缩流结构的指针 */
    ZlixMemFuncs zlixMemOper;    /* 内存申请函数 */
    ZLIX_INFLATE_MODE mode;      /* 当前的解压状态 */
    int32_t error;               /* 错误码 */
    int32_t last;                /* 最后一个block时为true */
    ZlixEncodeStrategy strategy; /* 压缩策略 */
    uint32_t wrap;               /* 解压格式，bit 0置位代表zlib格式，bit 1置位代表gzip格式，bit 2置位代表需要合法性校验 */
    uint32_t check;              /* 校验值 */
    uint32_t flags;              /* gzip头标记位 (zlib时为0)，低8位表示CM（压缩方法），高8位表示FLG（标记位） */
    uint32_t wbits;              /* 窗口长度对的底数 */
    uint32_t wsize;              /* 为0时代表不使用滑窗 */
    uint32_t whave;              /* 窗口中可用字节数 */
    uint8_t *window;             /* 申请的滑窗内存地址 */

    uint32_t hold;               /* 输入bit缓冲区 */
    uint32_t bits;               /* 缓冲区中的bits数 */
    uint32_t readsofar;          /* 目前已有的输入 */
    uint32_t pending;            /* 目前已有的输出 */

    uint16_t length;             /* 需要拷贝的字符或者长度编码长度 */
    uint16_t offset;             /* 需要拷贝的字符的偏移距离 */
    uint16_t extra;              /* 需要的extra bits */

    uint16_t codenum;             /* CCL序列个数 */
    uint16_t lennum;              /* CL1序列个数 */
    uint16_t distnum;             /* CL2序列个数 */
    uint16_t have;                /* lens[]中的编码长度的个数 */
    uint16_t maxcodeLen;          /* 实际编码长度最大值 */
    uint16_t lthres;              /* CL1一级层数码表 */
    uint16_t dthres;              /* CL2一级层数码表 */

    uint8_t clArray[ZLIX_DYNAMIC_CL1_AND_CL2_LEN];
    CODE_TABLE_INFO lcodes[ZLIX_ENOUGH_LENS];       /* 码表空间 */
    CODE_TABLE_INFO dcodes[ZLIX_ENOUGH_DISTS];      /* 码表空间 */
} ZLIX_INFLATE_STATE;

typedef struct tagcodeTableConfig {
    CODE_TABLE_INFO *code;        // 序列数组地址
    uint16_t maxLen;              // 序列数组实际使用长度
    uint16_t base;                // 该码表有extra bit的最小区间值
    uint32_t threshold;           // 该码表的一级表的深度阈值
    const uint8_t *extraBits;     // 序列数组地址
    const uint16_t *rangeBaseVal; // 区间的基值数组
} CODE_TABLE_CINFIG;

/* 解压资源初始化 */
int32_t ZlixInflateInit(const ZlixConfigInfo *usrConfigInfo, ZlixStream *streamInfo);

/* 解压流程 */
int32_t ZlixInflate(int32_t flushType, ZlixStream *streamInfo);

/* 解压资源去初始化 */
int32_t ZlixInflateFini(ZlixStream *streamInfo);

void ZlixInflateStatusFuncReg(void);

#ifdef __cplusplus
}
#endif

#endif
