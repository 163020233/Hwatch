/**
 * @file zlix_public_inner.h
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
 * <tr><td>2022-12-15  <td>0.1.0    <td>Anonym       <td>Initial Version
 * </table>
 * *******************************************************************************************
 */

#ifndef ZLIX_PUBLIC_INNER_H
#define ZLIX_PUBLIC_INNER_H

#include <string.h>
#include "drb_zlix.h"
#include "zlix_utils_inner.h"
#include "securec.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ZLIX_STATIC static
#define CONSTRUCTOR
#define DESTRUCTOR

/**
 * @ingroup zlix
 * 压缩水平的最大值（不可取到），即压缩水平范围为0 ~ 9。
 */
#define ZLIX_LEVEL_OPT 10

#if defined(__GNUC__) && ((__GNUC__) > 3) || ((__GNUC__ == 3) && (__GNUC_MINOR__ > 3))
/**
 * @ingroup zlix
 * 预测分支可能走向，帮助编译器优化编译，减少代码的分支预测失败时的可能开销。
 */
#define ZLIX_LIKELY(x) (__builtin_expect(!!(x), 1))

/**
 * @ingroup zlix
 * 预测分支不可能走向，帮助编译器优化编译，减少代码的分支预测失败时的可能开销。
 */
#define ZLIX_UNLIKELY(x) (__builtin_expect(!!(x), 0))
#else
#define ZLIX_LIKELY(x) (x)
#define ZLIX_UNLIKELY(x) (x)
#endif

/**
 * @ingroup zlix
 * @brief 强制内联
 */
#if defined(__GNUC__) && (__GNUC__ >= 4)
#define ZLIX_ALWAYS_INLINE inline __attribute__((always_inline))
#else
#define ZLIX_ALWAYS_INLINE inline
#endif

typedef unsigned int  ZLIX_BOOL;
#define ZLIX_FALSE   0
#define ZLIX_TRUE    1

/* 定义zlix压缩头格式 */
typedef enum {
    ZLIX_FORMAT_DEFLATE_RAW,                 /* deflate算法不加头 */
    ZLIX_FORMAT_DEFLATE_ZLIB,                /* deflate算法加zlib头 */
    ZLIX_FORMAT_DEFLATE_GZIP,                /* deflate算法加gzip头 */
    ZLIX_FORMAT_BUTT
} ZlixHeadFormat;

/* 定义zlix压缩状态信息 */
typedef enum {
    ZLIX_STATUS_INIT,                       /* 定义初始化状态 */
    ZLIX_STATUS_DEFLATE_ZLIB_HEAD,          /* 定义zlib头的状态 */
    ZLIX_STATUS_DEFLATE_GZIP_HEAD,          /* 定义gzip头的状态 */
    ZLIX_STATUS_ENCODE,                     /* 定义encode状态 */
    ZLIX_STATUS_FINISH,                     /* 定义结束状态 */
    ZLIX_STATUS_DEFLATE_GZIP_TAIL,          /* 定义gzip尾的状态 */
    ZLIX_STATUS_DEFLATE_ZLIB_TAIL,          /* 定义zlib尾的状态 */
    ZLIX_BUTT
} ZlixStatus;

typedef struct {
    const char *fileName;
    size_t fileNameLen;
} ZlixGzHeadInfo;

typedef struct ZlixDeflateContextInfo ZlixDeflateContext;

/* 资源初始化钩子 */
typedef int32_t (*ZlixStreamResInitFunc)(const ZlixConfigInfo *usrConfigInfo, ZlixStream *streamInfo);

/* 资源复位钩子 */
typedef int32_t (*ZlixStreamResResetFunc)(ZlixStream *streamInfo);

/* 资源去初始化钩子 */
typedef int32_t (*ZlixStreamResFiniFunc)(ZlixStream *streamInfo);

/* 流压缩/解压缩钩子 */
typedef int32_t (*ZlixStreamEncodeFunc)(int32_t flushType, ZlixStream *streamInfo, ZlixGzHeadInfo *gzHeadInfo);

typedef int32_t (*ZlixStreamDecodeFunc)(int32_t flushType, ZlixStream *streamInfo);

/* 流压缩/解压缩钩子 */
typedef size_t (*ZlixStreamBoundFunc)(ZlixStream *streamInfo, size_t srcLen);

/* zlix 流压缩和解压缩操作基础接口定义 */
typedef struct  TagZlixStreamOperFuncs {
    ZlixStreamResInitFunc encodeResInit;    /* 压缩资源初始化 */
    ZlixStreamResResetFunc encodeResReset;  /* 压缩资源复位 */
    ZlixStreamEncodeFunc encodeStream;      /* 流压缩处理 */
    ZlixStreamResFiniFunc encodeResFini;    /* 压缩资源去初始化 */
    ZlixStreamBoundFunc encodeBound;        /* 获取压缩后数据大小上限 */

    ZlixStreamResInitFunc decodeResInit;    /* 解压缩资源初始化 */
    ZlixStreamResResetFunc decodeResReset;  /* 解压缩资源复位 */
    ZlixStreamDecodeFunc decodeStream;      /* 流解压缩处理 */
    ZlixStreamResFiniFunc decodeResFini;    /* 解压缩资源去初始化 */
} ZlixStreamOperFuncs;

/* 内存操作基础接口 */
typedef struct TagZlixMemFuncs {
    ZlixMallocFunc zlixAlloc;                /* 内存申请钩子 */
    ZlixFreeFunc zlixFree;                   /* 内存释放钩子 */
} ZlixMemFuncs;

/* 内存操作基础接口 */
typedef struct TagZlixLabelMemFuncs {
    ZlixLabelMallocFunc zlixLabelAlloc;    /* 内存申请钩子 */
    ZlixLabelFreeFunc zlixLabelFree;       /* 内存释放钩子 */
} ZlixLabelMemFuncs;

/* 全局zlix流操作表 */
extern ZlixStreamOperFuncs g_zlixStreamOprTbl[];

extern ZlixMemFuncs g_zlixMemOper;

extern ZlixLabelMemFuncs g_zlixLabelMemOper;

#define ZLIXFILENAME ("")

void *ZlixDefaultLabelMallocFunc(uint32_t label, size_t size, const char *fileName, uint32_t line);
void ZlixDefaultLabelFreeFunc(uint32_t label, void *addr, const char *fileName, uint32_t line);

#define ZLIX_LABEL_MALLOC(zlixMemOper, label, size) \
    ((g_zlixLabelMemOper.zlixLabelAlloc != ZlixDefaultLabelMallocFunc) ? \
        g_zlixLabelMemOper.zlixLabelAlloc((label), (size), ZLIXFILENAME, __LINE__) : \
        (zlixMemOper).zlixAlloc((size), ZLIXFILENAME, __LINE__))

#define ZLIX_LABEL_FREE(zlixMemOper, label, addr) \
    ((g_zlixLabelMemOper.zlixLabelFree != ZlixDefaultLabelFreeFunc) ? \
        g_zlixLabelMemOper.zlixLabelFree((label), (addr), ZLIXFILENAME, __LINE__) : \
        (zlixMemOper).zlixFree((addr), ZLIXFILENAME, __LINE__))

static inline ZlixStreamOperFuncs *ZlixStreamOperTblGet(void)
{
    return g_zlixStreamOprTbl;
}

/* 默认内存操作函数 */
void *ZlixMalloc(size_t size, const char *fileName, uint32_t line);
void ZlixFree(void *addr, const char *fileName, uint32_t line);

/* 检查内存操作函数 */
int32_t ZlixMemFuncsCheck(const ZlixConfigInfo *usrCfgInfo, ZlixMemFuncs *zlixMemOper);

ZLIX_STATIC ZLIX_ALWAYS_INLINE int IsLE(void)
{
#if ((defined(__GNUC__) || defined(__clang__)) && defined(__BYTE_ORDER__))
    return (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) ? 1 : 0;
#else
    int n = 1;
    return *(char *)(&n);
#endif
}

/*
 * @ingroup zlix
 * @brief struct to read or write unaligned addr
 */
#if defined(_MSC_VER) || (defined(__INTEL_COMPILER) && defined(WIN32))
    typedef struct { int64_t v; } Unalign64;
    typedef struct { uint64_t v; } UnalignU64;
    typedef struct { uint32_t v; } UnalignU32;
    typedef struct { uint16_t v; } UnalignU16;
#else
    typedef struct { uint64_t v; } __attribute__((packed)) UnalignU64;
    typedef struct { uint32_t v; } __attribute__((packed)) UnalignU32;
    typedef struct { uint16_t v; } __attribute__((packed)) UnalignU16;
#endif

#define READ64BIT(ptr) (((const UnalignU64 *)(ptr))->v)
#define READ32BIT(ptr) (((const UnalignU32 *)(ptr))->v)
#define READ16BIT(ptr) (((const UnalignU16 *)(ptr))->v)
#define WRITE64BIT(ptr, val) (((UnalignU64 *)(ptr))->v = (val))
#define WRITE32BIT(ptr, val) (((UnalignU32 *)(ptr))->v = (val))
#define WRITE16BIT(ptr, val) (((UnalignU16 *)(ptr))->v = (val))

/**
 * @ingroup zlix
 * @brief 4字节值大小端转换
 * @param val   [IN] 需要转换的值
 * @retval 小端4字节值
 */
ZLIX_STATIC ZLIX_ALWAYS_INLINE uint32_t Swap32(uint32_t val)
{
#if (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 2)))
    return __builtin_bswap32(val);
#endif
    return ((0xff000000 & (val << 24)) | /* 左移24位和右移24位值互换，0xff000000表示24~31的位置 */
            (0x000000ff & (val >> 24)) | /* 0x000000ff表示0~7的位置，24~31位右移24位放到0~7位的位置 */
            (0x00ff0000 & (val <<  8)) | /* 高8位和低8位值互换, 0x00ff0000表示15~24的位置 */
            (0x0000ff00 & (val >>  8))); /* 0x0000ff00表示8~15的位置，高8位右移8位放到低8位的位置 */
}

ZLIX_STATIC ZLIX_ALWAYS_INLINE int ReadLE32Bit(const void *addr, uint32_t *data)
{
    if (IsLE() != 0) {
        (*data) = READ32BIT(addr);
        return sizeof(*data);
    }

    (*data) = Swap32(READ32BIT(addr));
    return sizeof(*data);
}

/**
 * @ingroup zlix
 * @brief 2字节值大小端转换
 * @param val   [IN] 需要转换的值
 * @retval 小端2字节值
 */
ZLIX_STATIC ZLIX_ALWAYS_INLINE uint16_t Swap16(uint16_t val)
{
#if (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8)))
    return __builtin_bswap16(val);
#endif
    return ((0xff00 & (val << 0x8)) | (0x00ff & (val >> 0x8))); /* 0x8表示高8位和低8位值互换, 0xff00表示8~15的位置，0x00ff表示0~7的位置 */
}

ZLIX_STATIC ZLIX_ALWAYS_INLINE int ReadLE16Bit(const void *addr, uint16_t *out)
{
    if (IsLE() != 0) {
        (*out) = READ16BIT(addr);
        return sizeof(*out);
    }
    (*out) = Swap16(READ16BIT(addr));
    return sizeof(*out);
}

/**
 * @ingroup zlix
 * @brief 指令加速拷贝32byte内容，不区分大小端
 * @attention dst和src不能有重叠！否则结果出错
 * @param dst  [IN] 写入地址
 * @param src  [IN] 拷贝源数据地址
 * @retval
 */
ZLIX_STATIC ZLIX_ALWAYS_INLINE void Copy32Byte(void *dst, const void *src)
{
    WRITE64BIT(dst, READ64BIT(src));
    WRITE64BIT((uint8_t *)dst + 8, READ64BIT((const uint8_t *)src + 8));
    WRITE64BIT((uint8_t *)dst + 16, READ64BIT((const uint8_t *)src + 16));
    WRITE64BIT((uint8_t *)dst + 24, READ64BIT((const uint8_t *)src + 24));
}

/**
 * @ingroup zlix
 * @brief 指令加速拷贝16byte内容，不区分大小端
 * @attention dst和src不能有重叠！否则结果出错
 * @param dst  [IN] 写入地址
 * @param src  [IN] 拷贝源数据地址
 * @retval
 */
ZLIX_STATIC ZLIX_ALWAYS_INLINE void Copy16Byte(void *dst, const void *src)
{
    WRITE64BIT(dst, READ64BIT(src));
    WRITE64BIT((uint8_t *)dst + 8, READ64BIT((const uint8_t *)src + 8));
}

/**
 * @ingroup zlix
 * @brief 指令加速拷贝8byte内容，不区分大小端
 * @attention dst和src不能有重叠！否则结果出错
 * @param dst  [IN] 写入地址
 * @param src  [IN] 拷贝源数据地址
 * @retval
 */
ZLIX_STATIC ZLIX_ALWAYS_INLINE void Copy8Byte(void *dst, const void *src)
{
    WRITE64BIT(dst, READ64BIT(src));
}

ZLIX_STATIC ZLIX_ALWAYS_INLINE void ZlixWildCopy16(const uint8_t *srcPtr, uint8_t *dstPtr, const uint8_t *dstEnd)
{
    Copy16Byte(dstPtr, srcPtr);
    dstPtr += 16; /* 偏移16字节 */
    while (dstPtr < dstEnd) {
        srcPtr += 16; /* 偏移16字节 */
        Copy16Byte(dstPtr, srcPtr);
        dstPtr += 16; /* 偏移16字节 */
    };
}

#define ZLIX_CLEAR_OBJ(objPtr) (*(objPtr) = (typeof(*(objPtr))) {0})

int32_t ZlixStreamEncode2(int32_t flushType, ZlixStream *streamInfo, ZlixGzHeadInfo *gzHeadInfo);
#define ZLIX_1BYTES_MASK 0xFF

/* 将输入作为滑窗，可以看成输入内容一次性全部传入滑窗 */
ZLIX_STATIC ZLIX_ALWAYS_INLINE void ZlixDeflateWindowIsInput(
    ZlixStream *stream, ZLIX_BOOL needCrc, ZLIX_BOOL needAdler)
{
    if (stream->availIn != 0) {
        /* 将输入作为滑窗，可以看成输入内容一次性全部传入滑窗 */
        if (needCrc) {
            (void)ZlixUtilsCrc32Cal(stream->nextIn, stream->availIn, (uint32_t *)(&stream->adler));
        } else if (needAdler) {
            stream->adler = ZlixUtilsAdler32Cal(stream->adler, stream->nextIn, stream->availIn);
        }
        stream->nextIn += stream->availIn;
        stream->totalIn += stream->availIn;
        stream->availIn = 0;
    }
}

#ifdef __cplusplus
}
#endif

#endif
