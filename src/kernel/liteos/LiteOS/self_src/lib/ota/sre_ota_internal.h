/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: 差分升级模块内头文件
 * Author: LibCK项目组
 * Create: 2025-04-12
 * History: 2025-04-12 LibCK项目组
 */
#ifndef SRE_OTA_INTERNAL_H
#define SRE_OTA_INTERNAL_H

#include "drb_ota.h"
#include "drb_typedef.h"
#include "sre_attr_external.h"
#include "sre_cpu_external.h"
#include "sre_lib_external.h"
#include "sre_mem_external.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/* 非法的内存类型 */
#define OS_OTA_MEM_TYPE_INVALID 0xffffffff

/* 非法的压缩算法 */
#define OS_OTA_CPRE_ARITH_INVALID 0xffffffff

/* 判断io数据流模式是否是读模式 */
#define OS_OTA_IO_STREAM_MODE_IS_RD(mode) ((mode) == OS_OTA_IO_STREAM_RDONLY)

/* 判断io数据流模式是否是写模式 */
#define OS_OTA_IO_STREAM_MODE_IS_WR(mode) ((mode) == OS_OTA_IO_STREAM_WRONLY)

/* 分块压缩头部大小 */
#define OS_OTA_CHUNK_CPRE_HEADER_SIZE 4U

/* ota模块内存申请和释放函数 */
#define OS_OTA_MEM_ALLOC_ALIGN(len, align) OsMemAllocAlign((U32)OS_MID_OTA, g_otaDesc.memPtNo, (U32)(len), (align))
#define OS_OTA_MEM_ALLOC(len) OS_OTA_MEM_ALLOC_ALIGN(len, MEM_ADDR_ALIGN_004)
#define OS_OTA_MEM_FREE(addr) OsErrRecord(DRB_MemFree((U32)OS_MID_OTA, (addr)))

/*
 * @ingroup sre_ota
 * 内存类型信息的结构定义。
 *
 * 内存类型信息
 */
struct OsOtaMemTypeInfo {
    /* 内存类型 */
    U32 type;
    /* 内存属性 */
    union OtaMemAttr attr;
    /* 内存页大小 */
    size_t pageSize;
    /* 写内存函数 */
    OtaWriteMemFunc write;
    /* 读内存函数 */
    OtaReadMemFunc read;
};

/*
 * @ingroup sre_ota
 * 压缩算法信息的结构定义。
 *
 * 压缩算法信息
 */
struct OsOtaCpreArithInfo {
    /* 压缩算法类型 */
    U32 type;
    /* 压缩数据函数 */
    OtaCompressFunc compress;
    /* 解压数据函数 */
    OtaDecompressFunc decompress;
    /* 压缩大小上限函数 */
    OtaCompressBoundFunc compressBound;
};

/*
 * @ingroup drb_ota
 *
 * io数据流操作模式类型枚举定义
 */
enum OtaIoStreamMode {
    OS_OTA_IO_STREAM_RDONLY = 0x0, /* io数据流只读模式 */
    OS_OTA_IO_STREAM_WRONLY,       /* io数据流只写模式 */
    OS_OTA_IO_STREAM_MODE_BUTT     /* io数据流模式非法 */
};

/*
 * @ingroup drb_ota
 *
 * io数据流seek模式类型枚举定义
 */
enum OtaIoStreamSeekType {
    OS_OTA_IO_STREAM_SEEK_SET = 0, /* 从io数据流开始位置寻找 */
    OS_OTA_IO_STREAM_SEEK_CUR,     /* 从io数据流当前位置寻找 */
    OS_OTA_IO_STREAM_SEEK_END,     /* 从io数据流结束位置寻找 */
    OS_OTA_IO_STREAM_SEEK_BUTT     /* io数据流寻找位置非法 */
};

/*
 * @ingroup sre_ota
 * io数据流的结构定义。
 *
 * io数据流信息
 */
struct OtaIoStream {
    /* 打开模式 */
    enum OtaIoStreamMode mode;
    /* 内存基地址 */
    uintptr_t base;
    /* 当前的指针位置 */
    uintptr_t ptr;
    /* 内存大小 */
    size_t size;
    /* 内存类型信息 */
    struct OsOtaMemTypeInfo memTypeInfo;
};

/*
 * @ingroup sre_ota
 * 压缩数据帧头部的结构定义。
 *
 * 压缩数据帧头部信息
 */
struct OsOtaChunkCpreHeader {
    U32 len;
};

/*
 * @ingroup drb_ota
 * 打开io数据流函数的参数信息结构定义
 *
 * 传递打开io数据流函数的参数信息。
 */
struct OtaIoStreamPara {
    /* 打开模式 */
    enum OtaIoStreamMode mode;
    /* 内存类型 */
    U32 memType;
    /* 内存基地址 */
    uintptr_t base;
    /* 内存大小 */
    size_t size;
};

/*
 * @ingroup drb_ota
 * 打开压缩数据流函数的参数信息结构定义
 *
 * 传递打开压缩数据流函数的参数信息。
 */
struct OtaCpreStreamPara {
    /* 压缩算法 */
    U32 cpreArith;
    /* 分块压缩大小 */
    size_t chunkCpreSize;
    /* io数据流 */
    IoStream *io;
};

/*
 * @ingroup sre_ota
 * 压缩数据流的结构定义。
 *
 * 压缩数据流信息
 */
struct OtaCpreStream {
    /* io数据流 */
    IoStream *io;
    /* 压缩和解压数据缓冲区 */
    void *buffer;
    /* 压缩和解压数据缓冲区指针偏移 */
    off_t pos;
    /* 压缩和解压数据缓冲区当前大小 */
    size_t size;
    /* 压缩算法信息 */
    struct OsOtaCpreArithInfo cpreArithInfo;
    /* 分块压缩大小 */
    size_t chunkCpreSize;
};

/*
 * @ingroup sre_ota
 * ota模块描述的结构定义。
 *
 * ota模块描述
 */
struct OsOtaDesc {
    /* 最大支持的内存类型数目 */
    U32 memTypeMaxNum;
    /* 内存类型信息 */
    struct OsOtaMemTypeInfo *memTypeInfo;
    /* 最大支持的压缩算法数目 */
    U32 cpreArithMaxNum;
    /* 压缩算法信息 */
    struct OsOtaCpreArithInfo *cpreArithInfo;
    /* 使用的内存分区号 */
    U8 memPtNo;
    /* 预留空间，保证对齐 */
    U8 reserved[3];
    /* 使用的内存分区起始地址 */
    uintptr_t memPtAddr;
    /* 使用的内存分区大小 */
    size_t memPtSize;
};

/* ota模块描述 */
extern struct OsOtaDesc g_otaDesc;

/*
 * 描述: 将字节数组转成整数
 */
OS_SEC_ALW_INLINE INLINE int OsOtaByteArrayToInt(const U8 *byteArray, U32 size)
{
    int rstVal = 0;
    U32 idx = size - 1U;

    if (byteArray == NULL || size == 0) {
        return 0;
    }

    rstVal = (int)(byteArray[idx] & (U8)(~OS_BIT7_MASK));

    while (idx > 0) {
        rstVal = (rstVal * (int)(OS_MAX_U8 + 1U)) + (int)byteArray[idx - 1U];
        idx--;
    }

    if ((byteArray[size - 1U] & OS_BIT7_MASK) != 0) {
        rstVal = -rstVal;
    }

    return rstVal;
}

/*
 * 描述: 将整数转成字节数组
 */
OS_SEC_ALW_INLINE INLINE void OsOtaIntToByteArray(U32 intVal, U8 *byteArray)
{
    U32 idx = 0;

    if (byteArray == NULL) {
        return;
    }

    for (idx = 0; idx < sizeof(U32); idx++) {
        byteArray[idx] = (U8)(intVal >> (idx * OS_BITS_PER_BYTE));
    }

    return;
}

/*
 * 描述: 校验两块内存是否有重叠
 */
OS_SEC_ALW_INLINE INLINE bool OsOtaVerifyMemAddrOverlap(uintptr_t addr1, U32 size1, uintptr_t addr2, U32 size2)
{
    if ((addr1 == addr2) ||
        ((addr1 < addr2) && ((addr1 + size1) > addr2)) ||
        ((addr2 < addr1) && ((addr2 + size2) > addr1))) {
        return TRUE;
    }

    return FALSE;
}

/* 内存类型相关操作接口 */
extern U32 OsOtaVerifyMemType(U32 memType);
extern U32 OsOtaRegisterMemType(U32 memType, const struct OtaRegMemTypePara *para);
extern U32 OsOtaGetMemTypeInfo(U32 memType, struct OsOtaMemTypeInfo *memTypeInfo);

/* io数据流相关操作接口 */
extern IoStream *OsOtaOpenIoStream(const struct OtaIoStreamPara *para);
extern U32 OsOtaReadIoStream(IoStream *stream, void *buf, size_t size, size_t number, size_t *readNum);
extern U32 OsOtaWriteIoStream(IoStream *stream, const void *buf, size_t size, size_t number, size_t *writeNum);
extern U32 OsOtaSeekIoStream(IoStream *stream, long offset, enum OtaIoStreamSeekType whence);
extern U32 OsOtaTellIoStream(IoStream *stream, size_t *pos);
extern U32 OsOtaCloseIoStream(IoStream *stream);

/* 压缩数据流相关操作接口 */
extern U32 OsOtaVerifyCpreArith(U32 cpreArith);
extern U32 OsOtaRegisterCpreArith(U32 cpreArith, const struct OtaRegCpreArithPara *para);
extern U32 OsOtaGetCpreArithInfo(U32 cpreArith, struct OsOtaCpreArithInfo *cpreArithInfo);
extern CpreStream *OsOtaOpenCpreStream(const struct OtaCpreStreamPara *para);
extern U32 OsOtaReadCpreStream(CpreStream *stream, void *buf, size_t size, size_t *readNum);
extern U32 OsOtaWriteCpreStream(CpreStream *stream, const void *buf, size_t size, size_t *writeNum);
extern U32 OsOtaFlushCpreStream(CpreStream *stream);
extern U32 OsOtaCloseCpreStream(CpreStream *stream);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* SRE_OTA_INTERNAL_H */
