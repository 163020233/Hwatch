/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: 差分升级压缩数据流处理相关功能
 * Author: LibCK项目组
 * Create: 2025-04-12
 * History: 2025-04-12 LibCK项目组
 */
#include "sre_ota_internal.h"
#include "sre_err_external.h"

/*
 * 描述: 校验压缩算法
 */
OS_SEC_L4_TEXT U32 OsOtaVerifyCpreArith(U32 cpreArith)
{
    U32 idx;
    uintptr_t intSave = OsIntLock();

    for (idx = 0; idx < g_otaDesc.cpreArithMaxNum; idx++) {
        if (g_otaDesc.cpreArithInfo[idx].type == cpreArith) {
            OsIntRestore(intSave);
            return DRB_OK;
        }
    }

    /* 恢复中断 */
    OsIntRestore(intSave);

    /* 压缩算法未注册 */
    return OS_ERRNO_OTA_CPRE_ARITH_NO_REG;
}

/*
 * 描述: 注册压缩算法信息
 */
OS_SEC_L4_TEXT U32 OsOtaRegisterCpreArith(U32 cpreArith, const struct OtaRegCpreArithPara *para)
{
    U32 idx;
    U32 idle = OS_MAX_U32;
    uintptr_t intSave;

    /* 校验入参 */
    if (para == NULL || para->compress == NULL || para->decompress == NULL || para->compressBound == NULL) {
        return OS_ERRNO_OTA_PTR_NULL;
    }

    /* 关中断 */
    intSave = OsIntLock();

    for (idx = 0; idx < g_otaDesc.cpreArithMaxNum; idx++) {
        if (g_otaDesc.cpreArithInfo[idx].type == cpreArith) {
            /* 压缩算法已经注册过 */
            OsIntRestore(intSave);
            return OS_ERRNO_OTA_CPRE_ARITH_REPEAT_REG;
        }

        if (g_otaDesc.cpreArithInfo[idx].type == OS_OTA_CPRE_ARITH_INVALID && idle == OS_MAX_U32) {
            /* 找到空闲的索引 */
            idle = idx;
            break;
        }
    }

    if (idle != OS_MAX_U32) {
        /* 保存压缩算法信息 */
        g_otaDesc.cpreArithInfo[idle].type = cpreArith;
        g_otaDesc.cpreArithInfo[idle].compress = para->compress;
        g_otaDesc.cpreArithInfo[idle].decompress = para->decompress;
        g_otaDesc.cpreArithInfo[idle].compressBound = para->compressBound;

        OsIntRestore(intSave);
        return DRB_OK;
    }

    /* 恢复中断 */
    OsIntRestore(intSave);

    /* 压缩算法已注册满 */
    return OS_ERRNO_OTA_CPRE_ARITH_REG_FULL;
}

/*
 * 描述: 注册压缩算法信息
 */
OS_SEC_L4_TEXT U32 DRB_OtaRegisterCpreArith(U32 cpreArith, const struct OtaRegCpreArithPara *para)
{
    return OsOtaRegisterCpreArith(cpreArith, para);
}

/*
 * 描述: 通过压缩算法类型获取该压缩算法信息
 */
OS_SEC_L4_TEXT U32 OsOtaGetCpreArithInfo(U32 cpreArith, struct OsOtaCpreArithInfo *cpreArithInfo)
{
    U32 ret;
    U32 idx;
    uintptr_t intSave;

    if (cpreArithInfo == NULL) {
        return OS_ERRNO_OTA_PTR_NULL;
    }

    /* 校验压缩算法 */
    ret = OsOtaVerifyCpreArith(cpreArith);
    if (ret != DRB_OK) {
        return ret;
    }

    /* 关中断 */
    intSave = OsIntLock();

    for (idx = 0; idx < g_otaDesc.cpreArithMaxNum; idx++) {
        if (g_otaDesc.cpreArithInfo[idx].type == cpreArith) {
            /* 获取压缩算法信息 */
            cpreArithInfo->type = g_otaDesc.cpreArithInfo[idx].type;
            cpreArithInfo->compress = g_otaDesc.cpreArithInfo[idx].compress;
            cpreArithInfo->decompress = g_otaDesc.cpreArithInfo[idx].decompress;
            cpreArithInfo->compressBound = g_otaDesc.cpreArithInfo[idx].compressBound;

            OsIntRestore(intSave);
            return DRB_OK;
        }
    }

    /* 恢复中断 */
    OsIntRestore(intSave);

    /* 压缩算法未注册 */
    return OS_ERRNO_OTA_CPRE_ARITH_NO_REG;
}

/*
 * 描述: 校验打开压缩数据流函数的入参
 */
OS_SEC_L4_TEXT U32 OsOtaCpreStreamCheckPara(const struct OtaCpreStreamPara *para)
{
    U32 ret;

    /* 校验压缩算法 */
    ret = OsOtaVerifyCpreArith(para->cpreArith);
    if (ret != DRB_OK) {
        return ret;
    }

    /* 校验分块压缩大小 */
    if (para->chunkCpreSize == 0) {
        return OS_ERRNO_OTA_SIZE_ZERO;
    }

    /* 校验io数据流 */
    if (para->io == NULL) {
        return OS_ERRNO_OTA_PTR_NULL;
    }

    return DRB_OK;
}

/*
 * 描述: 使用给定的压缩算法打开压缩数据流
 */
OS_SEC_L4_TEXT CpreStream *OsOtaOpenCpreStream(const struct OtaCpreStreamPara *para)
{
    U32 ret;
    CpreStream *cpre;

    /* 校验入参 */
    if (para == NULL) {
        OS_REPORT_ERROR(OS_ERRNO_OTA_PTR_NULL);
        return NULL;
    }

    ret = OsOtaCpreStreamCheckPara(para);
    if (ret != DRB_OK) {
        OS_REPORT_ERROR(ret);
        return NULL;
    }

    /* 申请压缩数据流控制块内存 */
    cpre = (CpreStream *)OS_OTA_MEM_ALLOC(sizeof(CpreStream));
    if (cpre == NULL) {
        OS_REPORT_ERROR(OS_ERRNO_OTA_MEM_ALLOC_FAIL);
        return NULL;
    }

    /* 初始化压缩数据流控制信息 */
    cpre->io = para->io;
    cpre->pos = 0;
    cpre->size = 0;
    cpre->chunkCpreSize = para->chunkCpreSize;

    /* 获取压缩算法信息 */
    ret = OsOtaGetCpreArithInfo(para->cpreArith, &cpre->cpreArithInfo);
    if (ret != DRB_OK) {
        goto EXIT;
    }

    /* 申请压缩数据流缓冲区的内存 */
    cpre->buffer = OS_OTA_MEM_ALLOC(para->chunkCpreSize);
    if (cpre->buffer == NULL) {
        ret = OS_ERRNO_OTA_MEM_ALLOC_FAIL;
        goto EXIT;
    }

    return cpre;

EXIT:
    OS_OTA_MEM_FREE(cpre);
    OS_REPORT_ERROR(ret);
    return NULL;
}

/*
 * 描述: 读取分块压缩单个节点的数据
 */
OS_SEC_L4_TEXT U32 OsOtaReadChunkCpreNode(CpreStream *stream)
{
    U32 ret;
    struct OsOtaChunkCpreHeader header = {0};
    U8 data[OS_OTA_CHUNK_CPRE_HEADER_SIZE];
    void *buffer;
    size_t res;

    /* 读取分块压缩数据的头部信息 */
    ret = OsOtaReadIoStream(stream->io, data, 1U, OS_OTA_CHUNK_CPRE_HEADER_SIZE, &res);
    if ((ret != DRB_OK) || (res != OS_OTA_CHUNK_CPRE_HEADER_SIZE)) {
        return (ret != DRB_OK) ? ret : OS_ERRNO_OTA_IO_STREAM_READ_FAIL;
    }

    /* 获取当前分块压缩数据的大小 */
    header.len = (U32)OsOtaByteArrayToInt(data, OS_OTA_CHUNK_CPRE_HEADER_SIZE);

    /* 申请存放压缩数据的内存 */
    buffer = OS_OTA_MEM_ALLOC(header.len);
    if (buffer == NULL) {
        return OS_ERRNO_OTA_MEM_ALLOC_FAIL;
    }

    /* 从io数据流中读取压缩数据 */
    ret = OsOtaReadIoStream(stream->io, buffer, 1U, header.len, &res);
    if ((ret != DRB_OK) || (res != header.len)) {
        ret = (ret != DRB_OK) ? ret : OS_ERRNO_OTA_IO_STREAM_READ_FAIL;
        goto EXIT;
    }

    /* 将数据解压并放到缓冲区中 */
    stream->size = stream->chunkCpreSize;
    ret = stream->cpreArithInfo.decompress(buffer, header.len, stream->buffer, &stream->size);
    if (ret != DRB_OK) {
        ret = OS_ERRNO_OTA_DECOMPRESS_FAIL;
        goto EXIT;
    }

    stream->pos = 0;

EXIT:
    OS_OTA_MEM_FREE(buffer);
    return ret;
}

/*
 * 描述: 从给定压缩数据流读取数据到buf所指向的数组中
 */
OS_SEC_L4_TEXT U32 OsOtaReadCpreStream(CpreStream *stream, void *buf, size_t size, size_t *readNum)
{
    U32 ret = DRB_OK;
    off_t bufPos = 0;
    size_t remSize = size;
    U32 len;

    /* 校验入参 */
    if (stream == NULL || buf == NULL || readNum == NULL) {
        return OS_ERRNO_OTA_PTR_NULL;
    }

    if (size == 0) {
        return OS_ERRNO_OTA_SIZE_ZERO;
    }

    /* 校验io数据流的模式 */
    if (!OS_OTA_IO_STREAM_MODE_IS_RD(stream->io->mode)) {
        return OS_ERRNO_OTA_IO_STREAM_NO_PERMISSION;
    }

    while ((size_t)bufPos < size) {
        /* 压缩缓冲区中的数据读取完之后，从分块压缩节点中读取数据 */
        if (stream->size == 0) {
            ret = OsOtaReadChunkCpreNode(stream);
            if (ret != DRB_OK) {
                break;
            }
        }

        if (remSize <= stream->size) {
            len = remSize;
        } else {
            len = stream->size;
        }

        /* 从压缩数据流中读取数据 */
        if (memcpy_s((void *)((uintptr_t)buf + (size_t)bufPos), len,
            (const void *)((uintptr_t)stream->buffer + (size_t)stream->pos), len) != EOK) {
            OS_GOTO_SYS_ERROR();
        }
        stream->pos += (off_t)len;
        stream->size -= len;
        bufPos += (off_t)len;
        remSize -= (size_t)len;
    }

    /* 返回实际读取到的数据长度 */
    *readNum = (size_t)bufPos;
    return ret;
}

/*
 * 描述: 写入分块压缩单个节点的数据
 */
OS_SEC_L4_TEXT U32 OsOtaWriteChunkCpreNode(CpreStream *stream)
{
    U32 ret;
    struct OsOtaChunkCpreHeader header = {0};
    U8 data[OS_OTA_CHUNK_CPRE_HEADER_SIZE];
    void *buffer;
    size_t len;
    size_t res;

    len = (size_t)stream->pos;
    header.len = stream->cpreArithInfo.compressBound(len);

    /* 申请存放压缩数据的内存 */
    buffer = OS_OTA_MEM_ALLOC(header.len);
    if (buffer == NULL) {
        return OS_ERRNO_OTA_MEM_ALLOC_FAIL;
    }

    /* 压缩数据 */
    ret = stream->cpreArithInfo.compress(stream->buffer, len, buffer, &header.len);
    if (ret != DRB_OK) {
        ret = OS_ERRNO_OTA_COMPRESS_FAIL;
        goto EXIT;
    }

    OsOtaIntToByteArray(header.len, data);

    /* 写入分块压缩数据的头部信息 */
    ret = OsOtaWriteIoStream(stream->io, data, OS_OTA_CHUNK_CPRE_HEADER_SIZE, 1U, &res);
    if ((ret != DRB_OK) || (res != 1U)) {
        ret = (ret != DRB_OK) ? ret : OS_ERRNO_OTA_IO_STREAM_WRITE_FAIL;
        goto EXIT;
    }

    /* 将压缩数据写入io数据流中 */
    ret = OsOtaWriteIoStream(stream->io, buffer, header.len, 1U, &res);
    if ((ret != DRB_OK) || (res != 1U)) {
        ret = (ret != DRB_OK) ? ret : OS_ERRNO_OTA_IO_STREAM_WRITE_FAIL;
        goto EXIT;
    }

    stream->pos = 0;

EXIT:
    OS_OTA_MEM_FREE(buffer);
    return ret;
}

/*
 * 描述: 把buf所指向的数组中的数据写入到给定压缩数据流中
 */
OS_SEC_L4_TEXT U32 OsOtaWriteCpreStream(CpreStream *stream, const void *buf, size_t size, size_t *writeNum)
{
    U32 ret = DRB_OK;
    off_t bufPos = 0;
    size_t remSize = size;
    size_t len;

    /* 校验入参 */
    if (stream == NULL || buf == NULL || writeNum == NULL) {
        return OS_ERRNO_OTA_PTR_NULL;
    }

    if (size == 0) {
        return OS_ERRNO_OTA_SIZE_ZERO;
    }

    /* 校验io数据流的模式 */
    if (!OS_OTA_IO_STREAM_MODE_IS_WR(stream->io->mode)) {
        return OS_ERRNO_OTA_IO_STREAM_NO_PERMISSION;
    }

    while ((size_t)bufPos < size) {
        if ((size_t)stream->pos + remSize <= stream->chunkCpreSize) {
            len = remSize;
        } else {
            len = stream->chunkCpreSize - (size_t)stream->pos;
        }

        /* 将数据放到压缩数据缓冲区中 */
        if (memcpy_s((void *)((uintptr_t)stream->buffer + (size_t)stream->pos), len,
            (const void *)((uintptr_t)buf + (size_t)bufPos), len) != EOK) {
            OS_GOTO_SYS_ERROR();
        }
        stream->pos += (off_t)len;
        bufPos += (off_t)len;
        remSize -= len;

        /* 压缩数据缓冲区满了之后，则压缩并写入io数据流 */
        if ((size_t)stream->pos == stream->chunkCpreSize) {
            ret = OsOtaWriteChunkCpreNode(stream);
            if (ret != DRB_OK) {
                break;
            }
        }
    }

    /* 返回实际写入成功的数据长度 */
    *writeNum = (size_t)bufPos;
    return ret;
}

/*
 * 描述: 把缓冲区中的数据回刷到io数据流中
 */
OS_SEC_L4_TEXT U32 OsOtaFlushCpreStream(CpreStream *stream)
{
    /* 校验入参 */
    if (stream == NULL) {
        return OS_ERRNO_OTA_PTR_NULL;
    }

    /* 校验io数据流的模式 */
    if (!OS_OTA_IO_STREAM_MODE_IS_WR(stream->io->mode)) {
        return OS_ERRNO_OTA_IO_STREAM_NO_PERMISSION;
    }

    /* 缓冲区的数据大小为0，则不需要回刷 */
    if (stream->pos == 0) {
        return DRB_OK;
    }

    return OsOtaWriteChunkCpreNode(stream);
}

/*
 * 描述: 关闭压缩数据流
 */
OS_SEC_L4_TEXT U32 OsOtaCloseCpreStream(CpreStream *stream)
{
    uintptr_t intSave = OsIntLock();

    if (stream != NULL) {
        /* 回刷缓冲区数据 */
        OsErrRecord(OsOtaFlushCpreStream(stream));

        /* 关闭io数据流 */
        OsErrRecord(OsOtaCloseIoStream(stream->io));

        /* 释放内存 */
        if (stream->buffer != NULL) {
            OS_OTA_MEM_FREE(stream->buffer);
        }
        OS_OTA_MEM_FREE(stream);
    }

    /* 恢复中断 */
    OsIntRestore(intSave);
    return DRB_OK;
}
