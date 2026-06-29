/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: 差分升级epatch差分还原算法，通过old镜像和patch差分包生成new镜像
 * Author: LibCK项目组
 * Create: 2025-04-12
 * History: 2025-04-12 LibCK项目组
 */
#include "sre_ota_epatch_internal.h"
#include "../sre_ota_internal.h"
#include "drb_typedef.h"
#include "drb_mem.h"
#include "securec.h"

/*
 * 描述: 解析header信息
 */
OS_SEC_L4_TEXT static U32 OsOtaEpatchParseHeader(const struct OtaPara *para, struct OsOtaEpatchHeader *header)
{
    U32 ret;
    struct OtaIoStreamPara ioStreamPara;
    IoStream *io;
    U8 buffer[EPATCH_HEADER_LEN];
    U32 pos = 0;
    size_t res;

    /* 打开patch差分包文件 */
    ioStreamPara.mode = OS_OTA_IO_STREAM_RDONLY;
    ioStreamPara.memType = para->patchedMemType;
    ioStreamPara.base = para->patchedAddr;
    ioStreamPara.size = para->patchedSize;

    io = OsOtaOpenIoStream(&ioStreamPara);
    if (io == NULL) {
        return OS_ERRNO_OTA_IO_STREAM_OPEN_FAIL;
    }

    /* 读取header信息 */
    ret = OsOtaReadIoStream(io, buffer, 1U, EPATCH_HEADER_LEN, &res);
    if ((ret != DRB_OK) || (res != EPATCH_HEADER_LEN)) {
        ret = (ret != DRB_OK) ? ret : OS_ERRNO_OTA_IO_STREAM_READ_FAIL;
        goto EXIT;
    }

    /* 获取版本号 */
    if (memcpy_s(header->version, EPATCH_HEADER_VERSION_LEN, buffer, EPATCH_HEADER_VERSION_LEN) != EOK) {
        OS_GOTO_SYS_ERROR();
    }
    pos += EPATCH_HEADER_VERSION_LEN;

    /* 获取ctrl block大小 */
    header->ctrlBlockSize = (size_t)OsOtaByteArrayToInt(buffer + pos, EPATCH_HEADER_CTRL_BLOCK_LEN);
    pos += EPATCH_HEADER_CTRL_BLOCK_LEN;

    /* 获取diff block大小 */
    header->diffBlockSize = (size_t)OsOtaByteArrayToInt(buffer + pos, EPATCH_HEADER_DIFF_BLOCK_LEN);
    pos += EPATCH_HEADER_DIFF_BLOCK_LEN;

    /* 获取new镜像的大小 */
    header->newImageSize = (size_t)OsOtaByteArrayToInt(buffer + pos, EPATCH_HEADER_NEW_IMAGE_LEN);

EXIT:
    /* 关闭patch差分包文件 */
    (void)OsOtaCloseIoStream(io);
    return ret;
}

/*
 * 描述: 打开patch差分包中的ctrl\diff\extra block压缩流
 */
OS_SEC_L4_TEXT static U32 OsOtaEpatchOpenBlock(const struct OtaPara *para, U32 offset, CpreStream **stream)
{
    U32 ret;
    struct OtaIoStreamPara ioStreamPara;
    struct OtaCpreStreamPara cpreStreamPara;
    IoStream *io;
    CpreStream *cpre;

    /* 打开io数据流 */
    ioStreamPara.mode = OS_OTA_IO_STREAM_RDONLY;
    ioStreamPara.memType = para->patchedMemType;
    ioStreamPara.base = para->patchedAddr;
    ioStreamPara.size = para->patchedSize;

    io = OsOtaOpenIoStream(&ioStreamPara);
    if (io == NULL) {
        return OS_ERRNO_OTA_IO_STREAM_OPEN_FAIL;
    }

    /* 对io数据流进行偏移 */
    ret = OsOtaSeekIoStream(io, (long)offset, OS_OTA_IO_STREAM_SEEK_SET);
    if (ret != DRB_OK) {
        goto EXIT;
    }

    /* 打开压缩数据流 */
    cpreStreamPara.cpreArith = para->patchedCpreArith;
    cpreStreamPara.chunkCpreSize = para->patchedChunkCpreSize;
    cpreStreamPara.io = io;

    cpre = OsOtaOpenCpreStream(&cpreStreamPara);
    if (cpre == NULL) {
        ret = OS_ERRNO_OTA_CPRE_STREAM_OPEN_FAIL;
        goto EXIT;
    }

    *stream = cpre;

    return DRB_OK;

EXIT:
    /* 关闭io数据流 */
    (void)OsOtaCloseIoStream(io);
    return ret;
}

/*
 * 描述: 打开patch差分包
 */
OS_SEC_L4_TEXT U32 OsOtaEpatchOpenPatched(const struct OtaPara *para, struct OsOtaEpatchHeader *header,
    CpreStream **cpre)
{
    U32 ret;
    U32 pos = EPATCH_HEADER_LEN;

    /* 解析patch差分包header信息 */
    ret = OsOtaEpatchParseHeader(para, header);
    if (ret != DRB_OK) {
        return ret;
    }

    /* 打开ctrl block压缩数据流 */
    ret = OsOtaEpatchOpenBlock(para, pos, &cpre[EPATCH_COMPONENT_TYPE_CTRL_BLOCK]);
    if (ret != DRB_OK) {
        return ret;
    }

    pos += header->ctrlBlockSize;

    /* 打开diff block压缩数据流 */
    ret = OsOtaEpatchOpenBlock(para, pos, &cpre[EPATCH_COMPONENT_TYPE_DIFF_BLOCK]);
    if (ret != DRB_OK) {
        goto EXIT0;
    }

    pos += header->diffBlockSize;

    /* 打开extra block压缩数据流 */
    ret = OsOtaEpatchOpenBlock(para, pos, &cpre[EPATCH_COMPONENT_TYPE_EXTRA_BLOCK]);
    if (ret != DRB_OK) {
        goto EXIT1;
    }

    return DRB_OK;

EXIT1:
    /* 关闭diff block压缩数据流 */
    (void)OsOtaCloseCpreStream(cpre[EPATCH_COMPONENT_TYPE_DIFF_BLOCK]);

EXIT0:
    /* 关闭ctrl block压缩数据流 */
    (void)OsOtaCloseCpreStream(cpre[EPATCH_COMPONENT_TYPE_CTRL_BLOCK]);
    return ret;
}

/*
 * 描述: 打开old镜像
 */
OS_SEC_L4_TEXT U32 OsOtaEpatchOpenOldImage(const struct OtaPara *para, IoStream **old)
{
    struct OtaIoStreamPara ioStreamPara;

    /* 打开old镜像io数据流 */
    ioStreamPara.mode = OS_OTA_IO_STREAM_RDONLY;
    ioStreamPara.memType = para->oldImageMemType;
    ioStreamPara.base = para->oldImageAddr;
    ioStreamPara.size = para->oldImageSize;

    *old = OsOtaOpenIoStream(&ioStreamPara);
    if (*old == NULL) {
        return OS_ERRNO_OTA_IO_STREAM_OPEN_FAIL;
    }

    return DRB_OK;
}

/*
 * 描述: 打开new镜像io数据流
 */
OS_SEC_L4_TEXT U32 OsOtaEpatchOpenNewImage(const struct OtaPara *para, void **new)
{
    struct OtaIoStreamPara ioStreamPara;
    struct OtaCpreStreamPara cpreStreamPara;
    IoStream *newIo;

    ioStreamPara.mode = OS_OTA_IO_STREAM_WRONLY;
    ioStreamPara.memType = para->newImageMemType;
    ioStreamPara.base = para->newImageAddr;
    ioStreamPara.size = para->newImageMaxSize;

    newIo = OsOtaOpenIoStream(&ioStreamPara);
    if (newIo == NULL) {
        return OS_ERRNO_OTA_IO_STREAM_OPEN_FAIL;
    }

    /* 判断new镜像是否要压缩存储 */
    if (para->newImageIsCpreStore) {
        /* 打开new镜像的压缩数据流 */
        cpreStreamPara.cpreArith = para->newImageCpreArith;
        cpreStreamPara.chunkCpreSize = para->newImageChunkCpreSize;
        cpreStreamPara.io = newIo;

        *new = (void *)OsOtaOpenCpreStream(&cpreStreamPara);
        if (*new == NULL) {
            (void)OsOtaCloseIoStream(newIo);
            return OS_ERRNO_OTA_CPRE_STREAM_OPEN_FAIL;
        }
    } else {
        *new = (void *)newIo;
    }

    return DRB_OK;
}

/*
 * 描述: 解析ctrl block信息
 */
OS_SEC_L4_TEXT void OsOtaEpatchParseCtrlBlockUnit(const U8 *ctrlBlockAddr,
    struct OsOtaEpatchCtrlBlockUnit *ctrlBlockUnit)
{
    U32 pos = 0;

    /* 获取diff数据的大小 */
    ctrlBlockUnit->diffSize = (size_t)OsOtaByteArrayToInt(ctrlBlockAddr, EPATCH_CTRL_BLOCK_UNIT_DIFF_LEN);
    pos += EPATCH_CTRL_BLOCK_UNIT_DIFF_LEN;

    /* 获取extra数据的大小 */
    ctrlBlockUnit->extraSize = (size_t)OsOtaByteArrayToInt(ctrlBlockAddr + pos, EPATCH_CTRL_BLOCK_UNIT_EXTRA_LEN);
    pos += EPATCH_CTRL_BLOCK_UNIT_EXTRA_LEN;

    /* 获取old镜像的偏移量 */
    ctrlBlockUnit->oldOffset = (off_t)OsOtaByteArrayToInt(ctrlBlockAddr + pos, EPATCH_CTRL_BLOCK_UNIT_OFFSET_LEN);
}

/*
 * 描述: 处理diff单个节点数据
 */
OS_SEC_L4_TEXT U32 OsOtaEpatchProcDiffNode(CpreStream *patchCpre, U32 len, IoStream *old, bool isCpreStore, void *new)
{
    U32 ret;
    U32 idx;
    U8 *buffer;
    U8 *oldBuffer;
    size_t res;

    /* 申请存放diff数据的内存 */
    buffer = (U8 *)OS_OTA_MEM_ALLOC(len);
    if (buffer == NULL) {
        return OS_ERRNO_OTA_MEM_ALLOC_FAIL;
    }

    /* 从压缩数据流中读取diff数据 */
    ret = OsOtaReadCpreStream(patchCpre, buffer, len, &res);
    if ((ret != DRB_OK) || (res != len)) {
        ret = (ret != DRB_OK) ? ret : OS_ERRNO_OTA_CPRE_STREAM_READ_FAIL;
        goto EXIT0;
    }

    /* 申请存放old镜像的内存 */
    oldBuffer = (U8 *)OS_OTA_MEM_ALLOC(len);
    if (oldBuffer == NULL) {
        ret = OS_ERRNO_OTA_MEM_ALLOC_FAIL;
        goto EXIT0;
    }

    /* 从old镜像读取相应大小的数据 */
    ret = OsOtaReadIoStream(old, oldBuffer, 1U, len, &res);
    if ((ret != DRB_OK) || (res != len)) {
        ret = (ret != DRB_OK) ? ret : OS_ERRNO_OTA_IO_STREAM_READ_FAIL;
        goto EXIT1;
    }

    /* 生成new镜像数据 */
    for (idx = 0; idx < len; idx++) {
        buffer[idx] += oldBuffer[idx];
    }

    if (isCpreStore) {
        /* 将生成的new镜像压缩存储到内存 */
        ret = OsOtaWriteCpreStream((CpreStream *)new, buffer, len, &res);
        if ((ret != DRB_OK) || (res != len)) {
            ret = (ret != DRB_OK) ? ret : OS_ERRNO_OTA_CPRE_STREAM_WRITE_FAIL;
        }
    } else {
        /* 将new镜像写入到io数据流中 */
        ret = OsOtaWriteIoStream((IoStream *)new, buffer, len, 1U, &res);
        if ((ret != DRB_OK) || (res != 1U)) {
            ret = (ret != DRB_OK) ? ret : OS_ERRNO_OTA_IO_STREAM_WRITE_FAIL;
        }
    }

EXIT1:
    OS_OTA_MEM_FREE(oldBuffer);

EXIT0:
    OS_OTA_MEM_FREE(buffer);
    return ret;
}

/*
 * 描述: diff block数据处理，并将数据根据相应的方式写入内存
 */
OS_SEC_L4_TEXT U32 OsOtaEpatchProcDiffBlock(CpreStream *patchCpre, U32 size, IoStream *old, bool isCpreStore, void *new)
{
    U32 ret = DRB_OK;
    long remSize = (long)size;
    size_t len;
    size_t pageSize;

    /* 获取内存页大小 */
    if (isCpreStore) {
        pageSize = ((CpreStream *)new)->io->memTypeInfo.pageSize;
    } else {
        pageSize = ((IoStream *)new)->memTypeInfo.pageSize;
    }

    while (remSize > 0) {
        if ((size_t)remSize <= pageSize) {
            len = (size_t)remSize;
        } else {
            len = pageSize;
        }

        /* 处理diff单个节点数据 */
        ret = OsOtaEpatchProcDiffNode(patchCpre, len, old, isCpreStore, new);
        if (ret != DRB_OK) {
            break;
        }

        remSize -= (long)len;
    }

    return ret;
}

/*
 * 描述: 处理单个extra节点数据
 */
OS_SEC_L4_TEXT U32 OsOtaEpatchProcExtraNode(CpreStream *patchCpre, U32 len, bool isCpreStore, void *new)
{
    U32 ret;
    void *buffer;
    size_t res;

    /* 申请存放extra数据的内存 */
    buffer = OS_OTA_MEM_ALLOC(len);
    if (buffer == NULL) {
        return OS_ERRNO_OTA_MEM_ALLOC_FAIL;
    }

    /* 从压缩数据流中读取extra数据 */
    ret = OsOtaReadCpreStream(patchCpre, buffer, len, &res);
    if ((ret != DRB_OK) || (res != len)) {
        ret = (ret != DRB_OK) ? ret : OS_ERRNO_OTA_CPRE_STREAM_READ_FAIL;
        goto EXIT;
    }

    if (isCpreStore) {
        /* 将生成的new镜像压缩存储到内存 */
        ret = OsOtaWriteCpreStream((CpreStream *)new, buffer, len, &res);
        if ((ret != DRB_OK) || (res != len)) {
            ret = (ret != DRB_OK) ? ret : OS_ERRNO_OTA_CPRE_STREAM_WRITE_FAIL;
        }
    } else {
        /* 将new镜像写入到io数据流中 */
        ret = OsOtaWriteIoStream((IoStream *)new, buffer, len, 1U, &res);
        if ((ret != DRB_OK) || (res != 1U)) {
            ret = (ret != DRB_OK) ? ret : OS_ERRNO_OTA_IO_STREAM_WRITE_FAIL;
        }
    }

EXIT:
    /* 释放内存 */
    OS_OTA_MEM_FREE(buffer);
    return ret;
}

/*
 * 描述: extra block数据处理，并将数据根据相应的方式写入内存
 */
OS_SEC_L4_TEXT U32 OsOtaEpatchProcExtraBlock(CpreStream *patchCpre, U32 size, bool isCpreStore, void *new)
{
    U32 ret = DRB_OK;
    long remSize = (long)size;
    size_t len;
    size_t pageSize;

    /* 获取内存页大小 */
    if (isCpreStore) {
        pageSize = ((CpreStream *)new)->io->memTypeInfo.pageSize;
    } else {
        pageSize = ((IoStream *)new)->memTypeInfo.pageSize;
    }

    while (remSize > 0) {
        if ((size_t)remSize <= pageSize) {
            len = (size_t)remSize;
        } else {
            len = pageSize;
        }

        /* 处理extra单个节点数据 */
        ret = OsOtaEpatchProcExtraNode(patchCpre, len, isCpreStore, new);
        if (ret != DRB_OK) {
            break;
        }

        remSize -= (long)len;
    }

    return ret;
}

/*
 * 描述: 处理epatch单个节点数据
 */
OS_SEC_L4_TEXT U32 OsOtaEpatchProcNode(CpreStream **cpre, IoStream *old, bool isCpreStore, void *new, size_t *size)
{
    U32 ret;
    struct OsOtaEpatchCtrlBlockUnit ctrlBlockUnit;
    U8 buffer[EPATCH_CTRL_BLOCK_UNIT_SIZE];
    size_t res;

    /* 读取ctrl block数据 */
    ret = OsOtaReadCpreStream(cpre[EPATCH_COMPONENT_TYPE_CTRL_BLOCK], buffer, EPATCH_CTRL_BLOCK_UNIT_SIZE, &res);
    if ((ret != DRB_OK) || (res != EPATCH_CTRL_BLOCK_UNIT_SIZE)) {
        return (ret != DRB_OK) ? ret : OS_ERRNO_OTA_CPRE_STREAM_READ_FAIL;
    }

    /* 解析ctrl block数据 */
    OsOtaEpatchParseCtrlBlockUnit(buffer, &ctrlBlockUnit);

    /* 处理diff block数据 */
    ret = OsOtaEpatchProcDiffBlock(cpre[EPATCH_COMPONENT_TYPE_DIFF_BLOCK], ctrlBlockUnit.diffSize, old, isCpreStore,
        new);
    if (ret != DRB_OK) {
        return ret;
    }

    /* 更新生成的new镜像大小 */
    *size += ctrlBlockUnit.diffSize;

    /* 处理extra block信息 */
    ret = OsOtaEpatchProcExtraBlock(cpre[EPATCH_COMPONENT_TYPE_EXTRA_BLOCK], ctrlBlockUnit.extraSize, isCpreStore, new);
    if (ret != DRB_OK) {
        return ret;
    }

    /* 更新生成的new镜像大小 */
    *size += ctrlBlockUnit.extraSize;

    /* 设置old镜像的偏移 */
    return OsOtaSeekIoStream(old, ctrlBlockUnit.oldOffset, OS_OTA_IO_STREAM_SEEK_CUR);
}

/*
 * 描述: epatch通过旧镜像和差分包生成新镜像
 */
OS_SEC_L4_TEXT U32 OsOtaEpatch(const struct OtaPara *para, size_t *newImageSize)
{
    U32 ret;
    struct OsOtaEpatchHeader header;
    size_t currentSize = 0;
    CpreStream *cpre[EPATCH_COMPONENT_TYPE_NUM];
    IoStream *old;
    void *new;

    *newImageSize = 0;

    /* 打开patch差分包 */
    ret = OsOtaEpatchOpenPatched(para, &header, cpre);
    if (ret != DRB_OK) {
        return ret;
    }

    /* 打开old镜像 */
    ret = OsOtaEpatchOpenOldImage(para, &old);
    if (ret != DRB_OK) {
        goto EXIT0;
    }

    /* 打开new镜像 */
    ret = OsOtaEpatchOpenNewImage(para, &new);
    if (ret != DRB_OK) {
        goto EXIT1;
    }

    while (currentSize < header.newImageSize) {
        /* 处理epatch的单个节点数据 */
        ret = OsOtaEpatchProcNode(cpre, old, para->newImageIsCpreStore, new, &currentSize);
        if (ret != DRB_OK) {
            goto EXIT2;
        }
    }

    if (para->newImageIsCpreStore) {
        /* 将缓冲区的数据压缩并回刷到io数据流中 */
        ret = OsOtaFlushCpreStream((CpreStream *)new);
        if (ret != DRB_OK) {
            goto EXIT2;
        }

        /* 压缩存储之后的new镜像大小 */
        ret = OsOtaTellIoStream(((CpreStream *)new)->io, newImageSize);
    } else {
        /* new镜像大小 */
        *newImageSize = currentSize;
    }

EXIT2:
    /* 关闭new镜像 */
    if (para->newImageIsCpreStore) {
        (void)OsOtaCloseCpreStream((CpreStream *)new);
    } else {
        (void)OsOtaCloseIoStream((IoStream *)new);
    }

EXIT1:
    /* 关闭old镜像 */
    (void)OsOtaCloseIoStream(old);

EXIT0:
    /* 关闭patch差分包 */
    (void)OsOtaCloseCpreStream(cpre[EPATCH_COMPONENT_TYPE_CTRL_BLOCK]);
    (void)OsOtaCloseCpreStream(cpre[EPATCH_COMPONENT_TYPE_DIFF_BLOCK]);
    (void)OsOtaCloseCpreStream(cpre[EPATCH_COMPONENT_TYPE_EXTRA_BLOCK]);
    return ret;
}
