/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: LiteOS Remote Processor Queue Module Implementation
 * Author: Huawei LiteOS Team
 * Create: 2022-11-11
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --------------------------------------------------------------------------- */

#include "los_rpqueue_pri.h"

STATIC UINT16 OsRpqueueTxMagicGet(const LosRpqueueCB *queue)
{
    return queue->queueSize + queue->readLock + queue->writeEnd + queue->writePos + queue->writeStatus;
}

STATIC UINT16 OsRpqueueRxMagicGet(const LosRpqueueCB *queue)
{
    return queue->queueSize + queue->readPos;
}

STATIC UINT32 OsRpqueueMagicCheckInSend(const LosRpqueueCB *queue)
{
    if (queue->txMagic != OsRpqueueTxMagicGet(queue)) {
        return LOS_ERRNO_RPQUEUE_CB_DAMAGED;
    }
    if ((queue->dataError != OS_RPQUEUE_DATA_OK) && (queue->rxMagic != OsRpqueueRxMagicGet(queue))) {
        return LOS_ERRNO_RPQUEUE_CB_DAMAGED;
    }
    if (queue->readPos > queue->queueSize) {
        return LOS_ERRNO_RPQUEUE_CB_DAMAGED;
    }
    return LOS_OK;
}

STATIC VOID OsRpqueueReInit(LosRpqueueCB *queue, UINT16 queueSize)
{
    LosRpqueueCB queueInfo;

    (VOID)memset_s(&queueInfo, sizeof(queueInfo), 0, sizeof(queueInfo));
    queueInfo.queueSize = queueSize;
    queueInfo.writeEnd = queueSize;
    queueInfo.readPos = 0;
    queueInfo.writePos = 0;
    queueInfo.writeStatus = OS_RPQUEUE_WRITE_IDLE;
    queueInfo.readLock = OS_RPQUEUE_READ_UNLOCK;
    RPQUEUE_RX_MAGIC_GET(&queueInfo);
    RPQUEUE_TX_MAGIC_GET(&queueInfo);
    queueInfo.magic = OS_RPQUEUE_INFO_MAGIC;
    queueInfo.dataError = OS_RPQUEUE_DATA_OK;
    (VOID)memcpy_s(queue, sizeof(LosRpqueueCB), &queueInfo, sizeof(queueInfo));
}

STATIC VOID OsRpqueueInfoInit(LosRpqueueCB *queue, UINT32 size)
{
    OsRpqueueReInit(queue, (UINT16)(size - sizeof(LosRpqueueCB)));
}

UINT32 LOS_RpqueueInit(VOID *queue, UINT32 size, UINT32 option)
{
    LosRpqueueCB *queueInfo = (LosRpqueueCB *)RPQUEUE_ADDR_ALIGN(queue);
    UINT32 addrAlign = (UINTPTR)queueInfo - (UINTPTR)queue;
    UINT32 queueSize;
    LosRpqueueCB queueCpy;
    UINT32 intSave;

    (VOID)option;
    if (queueInfo == NULL) {
        return LOS_ERRNO_RPQUEUE_PTR_NULL;
    }
    if (addrAlign >= size) {
        return LOS_ERRNO_RPQUEUE_SIZE_TOO_SMALL;
    }
    queueSize = size - addrAlign;
    if (queueSize < (sizeof(LosRpqueueCB) + sizeof(LosRpqueueHeader))) {
        return LOS_ERRNO_RPQUEUE_SIZE_TOO_SMALL;
    }
    if (queueSize > (sizeof(LosRpqueueCB) + OS_RPQUEUE_SIZE_MAX)) {
        return LOS_ERRNO_RPQUEUE_SIZE_TOO_LARGE;
    }
    intSave = RPQUEUE_LOCK();
    if (queueInfo->magic == OS_RPQUEUE_INFO_MAGIC) {
        (VOID)memcpy_s(&queueCpy, sizeof(queueCpy), queueInfo, sizeof(LosRpqueueCB));
        if (queueCpy.rxMagic == OsRpqueueRxMagicGet(&queueCpy) &&
            queueCpy.txMagic == OsRpqueueTxMagicGet(&queueCpy)) {
            queueCpy.writeStatus = OS_RPQUEUE_WRITE_IDLE;
            queueInfo->writeStatus = OS_RPQUEUE_WRITE_IDLE; /* Reset writeStatus. */
            RPQUEUE_TX_MAGIC_GET(&queueCpy);
            queueInfo->txMagic = queueCpy.txMagic;
            RPQUEUE_UNLOCK(intSave);
            return LOS_ERRNO_RPQUEUE_INITED; /* Already inited, return directly. */
        }
    }
    OsRpqueueInfoInit(queueInfo, queueSize);
    RPQUEUE_UNLOCK(intSave);
    return LOS_OK;
}

STATIC UINT32 OsRpqueueInfoCheck(LosRpqueueCB *queue)
{
    if (queue == NULL) {
        return LOS_ERRNO_RPQUEUE_PTR_NULL;
    }
    if (queue->magic != OS_RPQUEUE_INFO_MAGIC) {
        return LOS_ERRNO_RPQUEUE_INVALID;
    }
    return LOS_OK;
}

STATIC UINT8 OsRpqueueHeaderMagicGet(const LosRpqueueHeader *header)
{
    UINT8 magic = 0;
    const UINT8 *data = &header->cmd;
    UINT16 len = OS_RPQUEUE_HEADER_CHECK_SIZE;

    while (len--) {
        magic ^= *data;
        data++;
    }

    return magic;
}

STATIC BOOL OsRpqueueRemainGet(LosRpqueueCB *queueCpy, INT32 sendSize, INT32 *queueRm)
{
    INT32 remain;

    remain = queueCpy->readPos - queueCpy->writePos;
    if (remain == 0) { /* Rpqueue empty and reset read position. */
        remain = queueCpy->queueSize + OS_RPQUEUE_EMPTY_BYTE; /* Added 1 byte virtual space behind of data. */
        if (queueCpy->readPos != 0) {
            queueCpy->readPos = 0;
            queueCpy->writePos = 0;
            *queueRm = remain;
            return TRUE; /* need to reset readPos */
        }
    } else if (remain < 0) {
        remain = (INT32)((queueCpy->queueSize - RPQUEUE_ADDR_ALIGN(queueCpy->writePos)) + OS_RPQUEUE_EMPTY_BYTE);
        if (remain < sendSize) {
            remain = queueCpy->readPos;
            if (remain > sendSize) {
                queueCpy->writeEnd = queueCpy->writePos; /* need to update writeEnd */
                queueCpy->writePos = 0;
                *queueRm = remain;
                return FALSE;
            }
        }
        queueCpy->writePos = RPQUEUE_ADDR_ALIGN(queueCpy->writePos);
    } else {
        queueCpy->writePos = RPQUEUE_ADDR_ALIGN(queueCpy->writePos);
        remain = queueCpy->readPos - queueCpy->writePos;
    }
    *queueRm = remain;

    return FALSE;
}

STATIC UINT32 OsRpqueueSend(LosRpqueueCB *queue, const LosRpqueueHeader *header, const UINT8 *data)
{
    UINT8 *writeAddr = NULL;
    UINT32 ret = LOS_OK;
    UINT32 intSave;
    INT32 sendSize, queueRm;
    LosRpqueueCB queueCb;
    BOOL needRst;

    intSave = RPQUEUE_LOCK();
    (VOID)memcpy_s(&queueCb, sizeof(queueCb), queue, sizeof(LosRpqueueCB));
    ret = OsRpqueueMagicCheckInSend(&queueCb);
    if (ret != LOS_OK) {
        RPQUEUE_UNLOCK(intSave);
        return ret;
    }
    if (queue->dataError != OS_RPQUEUE_DATA_OK) {
        OsRpqueueReInit(queue, queueCb.queueSize);
        queueCb.writePos = 0;
        queueCb.readPos = 0;
    } else if (queue->writeStatus == OS_RPQUEUE_WRITE_BUSY) {
        RPQUEUE_UNLOCK(intSave);
        return LOS_ERRNO_RPQUEUE_WRITE_BUSY;
    } else {
        /* For MISRA-C */
    }

    queue->writeStatus = OS_RPQUEUE_WRITE_BUSY;
    RPQUEUE_TX_MAGIC_GET(queue); /* Write busy, allow direct use of shared memory. */
    RPQUEUE_UNLOCK(intSave);
    sendSize = (INT32)sizeof(LosRpqueueHeader) + header->len;
    needRst = OsRpqueueRemainGet(&queueCb, sendSize, &queueRm);
    if (queueRm > sendSize) { /* Must larger than sendSize. */
        writeAddr = (UINT8 *)((UINTPTR)(queue->queue) + queueCb.writePos);
        queueRm = queueCb.queueSize - queueCb.writePos;
        if ((memcpy_s(writeAddr, (UINT32)queueRm, header, sizeof(LosRpqueueHeader)) == EOK) &&
            (memcpy_s(writeAddr + sizeof(LosRpqueueHeader),
                (UINT32)queueRm - sizeof(LosRpqueueHeader), data, header->len) == EOK)) {
            queueCb.writePos += (UINT16)sendSize;  /* Get new writePos. */
        } else {
            ret = LOS_ERRNO_RPQUEUE_WRITE_ERROR; /* Needn't to take the previously operations back. */
        }
    } else {
        ret = LOS_ERRNO_RPQUEUE_BUF_NOT_ENOUGH;
    }
    intSave = RPQUEUE_LOCK();
    if (ret == LOS_OK) {
        if (queueCb.writePos > queueCb.writeEnd) {
            queueCb.writeEnd = queueCb.writePos;
        }
        if (needRst == TRUE) { /* The rpqueue is empty, need to reset rw pos */
            queue->readLock = OS_RPQUEUE_READ_LOCK;
            queue->readPos = queueCb.readPos;
            queue->rxMagic = OsRpqueueRxMagicGet(&queueCb);
            queue->writeEnd = queueCb.writeEnd;
            queue->writePos = queueCb.writePos;
            queue->readLock = OS_RPQUEUE_READ_UNLOCK;
        } else {
            queue->writeEnd = queueCb.writeEnd;
            queue->writePos = queueCb.writePos; /* Update writePos must after writeEnd. */
        }
        RPQUEUE_TX_MAGIC_GET(&queueCb);
    }
    queue->txMagic = queueCb.txMagic;
    queue->writeStatus = OS_RPQUEUE_WRITE_IDLE;
    RPQUEUE_UNLOCK(intSave);

    return ret;
}

UINT32 LOS_RpqueueMaxSendSizeGet(VOID *queue, UINT16 *maxSendSize)
{
    LosRpqueueCB *rpqueue = (LosRpqueueCB *)RPQUEUE_ADDR_ALIGN(queue);
    UINT32 ret;
    INT32 maxSize;
    UINT16 readPos;
    UINT16 writePos;

    if (maxSendSize == NULL) {
        return LOS_ERRNO_RPQUEUE_PTR_NULL;
    }
    ret = OsRpqueueInfoCheck(rpqueue);
    if (ret != LOS_OK) {
        return ret;
    }
    readPos = rpqueue->readPos;
    writePos = rpqueue->writePos;
    maxSize = readPos - writePos;
    writePos = RPQUEUE_ADDR_ALIGN(writePos);
    if (maxSize == 0) {
        maxSize = rpqueue->queueSize;
    } else if (maxSize < 0) {
        maxSize = rpqueue->queueSize - writePos; /* The last byte can be written. */
        if (maxSize < (readPos - OS_RPQUEUE_EMPTY_BYTE)) { /* The last byte can't be written. */
            maxSize = readPos - OS_RPQUEUE_EMPTY_BYTE;
        }
    } else {
        maxSize = (readPos - writePos) - OS_RPQUEUE_EMPTY_BYTE; /* The last byte can't be written. */
    }
    maxSize -= (INT32)sizeof(LosRpqueueHeader);
    if (maxSize < 0) {
        return LOS_ERRNO_RPQUEUE_BUF_NOT_ENOUGH;
    }
    *maxSendSize = (UINT16)maxSize;

    return ret;
}

UINT32 LOS_RpqueueSend(VOID *queue, const UINT8 *data, UINT16 len, const LosRpqueueSendCfg *cfg)
{
    UINT32 ret;
    UINT32 timeout;
    LosRpqueueHeader header;

    if (cfg == NULL || data == NULL) {
        return LOS_ERRNO_RPQUEUE_PTR_NULL;
    }
    queue = (VOID *)RPQUEUE_ADDR_ALIGN(queue);
    ret = OsRpqueueInfoCheck(queue);
    if (ret != LOS_OK) {
        return ret;
    }
    header.len = len;
    header.cmd = cfg->cmd;
    header.option = cfg->option;
    header.sequence = 0;
    header.reserved = 0;
    header.magic = OsRpqueueHeaderMagicGet(&header);
    timeout = cfg->timeout;
    if (timeout > 0) {
        do {
            ret = OsRpqueueSend(queue, &header, data);
            if (ret == LOS_OK) {
                break;
            }
            if (timeout > 0) {
                timeout--;
                (VOID)RPQUEUE_DELAY(1);
            } else {
                break;
            }
        } while ((ret == LOS_ERRNO_RPQUEUE_BUF_NOT_ENOUGH) || (ret == LOS_ERRNO_RPQUEUE_WRITE_BUSY));
    } else {
        ret = OsRpqueueSend(queue, &header, data);
    }

    return ret;
}

STATIC UINT32 OsRpqueueMsgRecv(LosRpqueueCB *queue, LosRpqueueCallback recvDataCb)
{
    LosRpqueueHeader header;
    LosRpqueueHeader *qHeader = NULL;
    UINT8 *readAddr = NULL;
    UINT16 readPos;
    LosRpqueueCB queueCb;

    if (queue->dataError != OS_RPQUEUE_DATA_OK) {
        return (queue->dataError == OS_RPQUEUE_HEADER_ERROR) ?
            LOS_ERRNO_RPQUEUE_HEADER_DAMAGED : LOS_ERRNO_RPQUEUE_CB_DAMAGED;
    }
    if (queue->readPos == queue->writePos) {
        return LOS_ERRNO_RPQUEUE_BUF_EMPTY;
    }
    if (queue->readLock == OS_RPQUEUE_READ_LOCK) {
        return LOS_ERRNO_RPQUEUE_READ_LOCK;
    }
    if (queue->readPos == queue->writePos) { /* Transmitter maybe in RW position resetting, need to check twice. */
        return LOS_ERRNO_RPQUEUE_BUF_EMPTY;
    }
    (VOID)memcpy_s(&queueCb, sizeof(queueCb), queue, sizeof(LosRpqueueCB));
    if (queueCb.rxMagic != OsRpqueueRxMagicGet(&queueCb)) {
        queue->dataError = OS_RPQUEUE_CB_ERROR;
        return LOS_ERRNO_RPQUEUE_CB_DAMAGED;
    }
    if (queueCb.readPos == queueCb.writeEnd) {
        queue->readPos = 0;
        queueCb.readPos = 0;
        readPos = 0;
    } else {
        readPos = RPQUEUE_ADDR_ALIGN(queueCb.readPos);
    }
    qHeader = (LosRpqueueHeader *)((UINTPTR)(queue->queue) + readPos);
    (VOID)memcpy_s(&header, sizeof(header), qHeader, sizeof(LosRpqueueHeader));
    if (header.magic != OsRpqueueHeaderMagicGet(&header)) {
        queue->rxMagic = OsRpqueueRxMagicGet(&queueCb);
        queue->dataError = OS_RPQUEUE_HEADER_ERROR;
        return LOS_ERRNO_RPQUEUE_HEADER_DAMAGED;
    }
    qHeader->magic = (UINT8)(~header.magic); /* After the data frame is received, it is damaged to prevent
                                                the data control block from being repeatedly received. */
    if (recvDataCb != NULL) {
        if (header.len > 0) {
            readAddr = qHeader->payload;
        }
        recvDataCb(header.cmd, readAddr, header.len); /* If header->len is 0 byte, set readAddr to NULL. */
    }
    queueCb.readPos = readPos + header.len + (UINT16)sizeof(LosRpqueueHeader);
    queue->readPos = queueCb.readPos;
    queue->rxMagic = OsRpqueueRxMagicGet(&queueCb);
    return LOS_OK;
}

UINT32 LOS_RpqueueRecv(VOID *queue, LosRpqueueCallback recvDataCb)
{
    UINT32 ret;
    LosRpqueueCB *queueInfo = (LosRpqueueCB *)RPQUEUE_ADDR_ALIGN(queue);

    ret = OsRpqueueInfoCheck(queueInfo);
    if (ret != LOS_OK) {
        return ret;
    }

    return OsRpqueueMsgRecv(queueInfo, recvDataCb);
}
