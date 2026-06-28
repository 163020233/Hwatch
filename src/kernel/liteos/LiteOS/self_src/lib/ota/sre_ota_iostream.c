/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: 差分升级io数据流处理相关功能
 * Author: LibCK项目组
 * Create: 2025-04-12
 * History: 2025-04-12 LibCK项目组
 */
#include "sre_ota_internal.h"
#include "sre_err_external.h"

/*
 * 描述: 校验打开io数据流函数的入参
 */
OS_SEC_L4_TEXT U32 OsOtaIoStreamCheckPara(const struct OtaIoStreamPara *para)
{
    U32 ret;

    /* 校验io数据流的内存类型 */
    ret = OsOtaVerifyMemType(para->memType);
    if (ret != DRB_OK) {
        return ret;
    }

    /* 校验io数据流的内存大小 */
    if (para->size == 0) {
        return OS_ERRNO_OTA_SIZE_ZERO;
    }

    /* 校验io数据流的模式 */
    if (para->mode >= OS_OTA_IO_STREAM_MODE_BUTT) {
        return OS_ERRNO_OTA_IO_STREAM_MODE_INVALID;
    }

    return DRB_OK;
}

/*
 * 描述: 使用给定的模式mode打开io数据流
 */
OS_SEC_L4_TEXT IoStream *OsOtaOpenIoStream(const struct OtaIoStreamPara *para)
{
    U32 ret;
    IoStream *io;

    /* 校验入参 */
    if (para == NULL) {
        OS_REPORT_ERROR(OS_ERRNO_OTA_PTR_NULL);
        return NULL;
    }

    ret = OsOtaIoStreamCheckPara(para);
    if (ret != DRB_OK) {
        OS_REPORT_ERROR(ret);
        return NULL;
    }

    /* 申请io数据流控制块内存 */
    io = (IoStream *)OS_OTA_MEM_ALLOC(sizeof(IoStream));
    if (io == NULL) {
        OS_REPORT_ERROR(OS_ERRNO_OTA_MEM_ALLOC_FAIL);
        return NULL;
    }

    /* 初始化io数据流控制信息 */
    io->mode = para->mode;
    io->base = para->base;
    io->ptr = para->base;
    io->size = para->size;

    /* 获取内存类型信息 */
    ret = OsOtaGetMemTypeInfo(para->memType, &io->memTypeInfo);
    if (ret != DRB_OK) {
        OS_OTA_MEM_FREE(io);
        OS_REPORT_ERROR(ret);
        return NULL;
    }

    return io;
}

/*
 * 描述: 从给定io数据流读取数据到buf所指向的数组中
 */
OS_SEC_L4_TEXT U32 OsOtaReadIoStream(IoStream *stream, void *buf, size_t size, size_t number, size_t *readNum)
{
    size_t count = 0;
    size_t total;
    uintptr_t intSave;

    /* 校验入参 */
    if (stream == NULL || buf == NULL || readNum == NULL) {
        return OS_ERRNO_OTA_PTR_NULL;
    }

    if (size == 0 || number == 0) {
        return OS_ERRNO_OTA_SIZE_ZERO;
    }

    intSave = OsIntLock();
    /* 校验io数据流的模式 */
    if (!OS_OTA_IO_STREAM_MODE_IS_RD(stream->mode)) {
        OsIntRestore(intSave);
        return OS_ERRNO_OTA_IO_STREAM_NO_PERMISSION;
    }

    /* 计算可以从io数据流中读取的数据个数 */
    while ((count < number) && (stream->ptr + (count + 1U) * size <= stream->base + stream->size)) {
        count++;
    }

    /* 从io流中读取数据 */
    total = count * size;
    if (total == 0) {
        OsIntRestore(intSave);
        return OS_ERRNO_OTA_IO_STREAM_READ_FAIL;
    }

    if (stream->memTypeInfo.attr.directRead == 1U) {
        if (memcpy_s(buf, total, (const void *)stream->ptr, total) != EOK) {
            OsIntRestore(intSave);
            OS_GOTO_SYS_ERROR();
        }
    } else {
        if (stream->memTypeInfo.read(stream->ptr, buf, total) != 0) {
            OsIntRestore(intSave);
            return OS_ERRNO_OTA_READ_MEM_FAIL;
        }
    }

    stream->ptr += total;

    /* 返回实际读取到的数据个数 */
    *readNum = count;
    OsIntRestore(intSave);
    return DRB_OK;
}

/*
 * 描述: 把buf所指向的数组中的数据写入到给定io数据流中
 */
OS_SEC_L4_TEXT U32 OsOtaWriteIoStream(IoStream *stream, const void *buf, size_t size, size_t number, size_t *writeNum)
{
    size_t count = 0;
    size_t total;
    uintptr_t intSave;

    /* 校验入参 */
    if (stream == NULL || buf == NULL || writeNum == NULL) {
        return OS_ERRNO_OTA_PTR_NULL;
    }

    if (size == 0 || number == 0) {
        return OS_ERRNO_OTA_SIZE_ZERO;
    }

    intSave = OsIntLock();
    /* 校验io数据流的模式 */
    if (!OS_OTA_IO_STREAM_MODE_IS_WR(stream->mode)) {
        OsIntRestore(intSave);
        return OS_ERRNO_OTA_IO_STREAM_NO_PERMISSION;
    }

    /* 计算可以往io数据流中写入的数据个数 */
    while ((count < number) && (stream->ptr + (count + 1U) * size <= stream->base + stream->size)) {
        count++;
    }

    total = count * size;
    if (total == 0) {
        OsIntRestore(intSave);
        return OS_ERRNO_OTA_IO_STREAM_WRITE_FAIL;
    }

    /* 往io数据流中写入数据 */
    if (stream->memTypeInfo.attr.directWrite == 1U) {
        if (memcpy_s((void *)stream->ptr, total, buf, total) != EOK) {
            OsIntRestore(intSave);
            OS_GOTO_SYS_ERROR();
        }
    } else {
        if (stream->memTypeInfo.write(stream->ptr, (const void *)buf, total) != 0) {
            OsIntRestore(intSave);
            return OS_ERRNO_OTA_WRITE_MEM_FAIL;
        }
    }

    stream->ptr += total;

    /* 返回实际写入成功的数据个数 */
    *writeNum = count;
    OsIntRestore(intSave);
    return DRB_OK;
}

/*
 * 描述: 置流stream的位置为给定的偏移offset，参数offset意味着从给定的whence位置偏移的字节数
 */
OS_SEC_L4_TEXT U32 OsOtaSeekIoStream(IoStream *stream, long offset, enum OtaIoStreamSeekType whence)
{
    uintptr_t intSave;

    /* 校验入参 */
    if (stream == NULL) {
        return OS_ERRNO_OTA_PTR_NULL;
    }

    if (whence >= OS_OTA_IO_STREAM_SEEK_BUTT) {
        return OS_ERRNO_OTA_IO_STREAM_SEEK_TYPE_INVALID;
    }

    /* 关中断 */
    intSave = OsIntLock();

    switch (whence) {
        /* 从io数据流的起始地址开始偏移 */
        case OS_OTA_IO_STREAM_SEEK_SET:
            if ((offset < 0) || ((size_t)offset > stream->size)) {
                goto EXIT;
            }

            stream->ptr = stream->base + (size_t)offset;
            break;

        /* 从io数据流当前位置开始偏移 */
        case OS_OTA_IO_STREAM_SEEK_CUR:
            if (((offset < 0) && ((stream->ptr + (size_t)offset) < stream->base)) ||
                ((offset > 0) && ((stream->ptr + (size_t)offset) > (stream->base + stream->size)))) {
                goto EXIT;
            }

            stream->ptr += (size_t)offset;
            break;

        /* 从io数据流结束地址开始偏移 */
        case OS_OTA_IO_STREAM_SEEK_END:
            if ((offset > 0) || ((size_t)(-offset) > stream->size)) {
                goto EXIT;
            }

            stream->ptr = stream->base + stream->size + (size_t)offset;
            break;

        default:
            goto EXIT;
    }

    /* 恢复中断 */
    OsIntRestore(intSave);
    return DRB_OK;

EXIT:
    OsIntRestore(intSave);
    return OS_ERRNO_OTA_IO_STREAM_SEEK_FAIL;
}

/*
 * 描述: 返回io数据流的当前文件位置
 */
OS_SEC_L4_TEXT U32 OsOtaTellIoStream(IoStream *stream, size_t *pos)
{
    uintptr_t intSave;

    /* 校验入参 */
    if (stream == NULL || pos == NULL) {
        return OS_ERRNO_OTA_PTR_NULL;
    }

    /* 关中断 */
    intSave = OsIntLock();

    /* 返回io数据流的当前位置，即文件当前大小 */
    *pos = stream->ptr - stream->base;

    /* 恢复中断 */
    OsIntRestore(intSave);
    return DRB_OK;
}

/*
 * 描述: 关闭io数据流
 */
OS_SEC_L4_TEXT U32 OsOtaCloseIoStream(IoStream *stream)
{
    uintptr_t intSave = OsIntLock();

    if (stream != NULL) {
        /* 释放io数据流的控制资源 */
        OS_OTA_MEM_FREE(stream);
    }

    /* 恢复中断 */
    OsIntRestore(intSave);
    return DRB_OK;
}
