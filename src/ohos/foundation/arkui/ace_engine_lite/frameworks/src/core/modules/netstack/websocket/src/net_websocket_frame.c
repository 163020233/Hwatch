/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "net_websocket_frame.h"
#include "net_websocket_debug.h"
#include "net_websocket_opts.h"
#include "endian.h"

#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

static void WsFrameParsePayload(
    const uint8_t *udata, uint32_t *expectedSizePtr, uint32_t *maskPosPtr, uint32_t *headerSizePtr)
{
    uint32_t expectedSize = *expectedSizePtr;
    uint32_t maskPos = *maskPosPtr;
    uint32_t headerSize = *headerSizePtr;
    uint32_t payload = udata[1] & 0x7f;

    switch (payload) {
        case 126: {          /* 126: ws payload */
            headerSize += 2; /* 2: offset */
            break;
        }
        case 127: {          /* 127: ws payload */
            headerSize += 8; /* 8: offset */
            break;
        }
        default:
            break;
    }

    switch (payload) {
        case 126: {                                    /* 126: ws payload */
            expectedSize |= ((uint32_t)udata[2]) << 8; /* 2,8: udata offset */
            expectedSize |= (uint32_t)udata[3];        /* 3: udata offset */
            maskPos = 4;                               /* 4: mask position */
            break;
        }
        case 127: {                                     /* 127: ws payload */
            expectedSize |= ((uint32_t)udata[6]) << 24; /* 6, 24: udata offset */
            expectedSize |= ((uint32_t)udata[7]) << 16; /* 7, 16: udata offset */
            expectedSize |= ((uint32_t)udata[8]) << 8;  /* 8: udata offset */
            expectedSize |= (uint32_t)udata[9];         /* 9: udata offset */
            maskPos = 10;                               /* 10: mask position */
            break;
        }
        default: {
            if (payload <= 125) { /* 125: payload */
                maskPos = 2;      /* 2: mask position */
                expectedSize = payload;
            }
            break;
        }
    }
    *headerSizePtr = headerSize;
    *maskPosPtr = maskPos;
    *expectedSizePtr = expectedSize;
}

static WsFrame *WsSetFrameData(WsFrame *frame, uint32_t expectedSize, const uint8_t *udata)
{
    const uint8_t *actualUdata = NULL;
    uint8_t *unmasked = NULL;
    size_t index = 0;
    if (frame->opcode == WS_OPCODE_CONNECTION_CLOSE || frame->opcode == WS_OPCODE_PONG) {
        return frame;
    }

    if (expectedSize > 0) {
        frame->data = malloc(expectedSize);
        if (frame->data == NULL) {
            goto Exit;
        }
        frame->length = expectedSize;
        actualUdata = udata + frame->headerSize;
        if (frame->isMasked) {
            unmasked = (uint8_t *)frame->data;
            for (index = 0; index < expectedSize; index++) {
                *unmasked = *actualUdata ^ frame->mask[index & 0x3];
                unmasked++;
                actualUdata++;
            }
        } else {
            if (memcpy_s(frame->data, expectedSize, actualUdata, expectedSize) != 0) {
                goto Exit;
            }
        }
    }
    return frame;
Exit:
    WsDeleteFrame(frame);
    return NULL;
}

WsFrame *WsFrameCreateWithRecvData(struct WebSocketClient *clientPtr, const uint8_t *data, const size_t length)
{
    if (data == NULL || length < 2) { /* 2: data length */
        return NULL;
    }
    const uint8_t *udata = (const uint8_t *)data;
    const WsOpcode opcode = (WsOpcode)(udata[0] & 0x0f);
    const uint32_t isFinished = (udata[0] >> 7) & 0x01;
    const uint32_t isMasked = (udata[1] >> 7) & 0x01;
    uint32_t headerSize = isMasked ? 6 : 2; /* 6, 2: header size */
    uint32_t expectedSize = 0;
    uint32_t maskPos = 0;
    WsFrame *frame = NULL;

    WsFrameParsePayload(udata, &expectedSize, &maskPos, &headerSize);
    if (length < headerSize) {
        return NULL;
    }
    frame = WsFrameCreate(clientPtr);
    if (frame == NULL) {
        return NULL;
    }
    frame->opcode = opcode;
    frame->isFinished = isFinished;
    frame->headerSize = (uint8_t)headerSize;

    if (isMasked) {
        frame->isMasked = true;
        if (memcpy_s(frame->mask, sizeof(frame->mask), &udata[maskPos], sizeof(frame->mask)) != 0) {
            WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("memcpy fail\n"));
            goto exit;
        }
    }
    return WsSetFrameData(frame, expectedSize, udata);
exit:
    WsDeleteFrame(frame);
    return NULL;
}

static void WsFrameCreateHeader(WsFrame *f, uint8_t *header, uint32_t headerSize, const size_t length)
{
    if (length > WS_MAX_DATA_LENGTH ) {
        return;
    }
    const uint32_t size = (uint32_t)length;
    uint8_t fin = 0x80;
    if (f->isFinished == false) {
        fin = 0x00;
    }

    *header++ = fin | f->opcode;
    if (size < 126) { /* 126: ws payload */
        *header++ = (size & 0xff) | (f->isMasked ? 0x80 : 0);
        f->headerSize = 2;                          /* 2: ws header size */
    } else if (size < 65536) {                      /* 65536: size max */
        *header++ = 126 | (f->isMasked ? 0x80 : 0); /* 126, 0x80: ws payload, mask */
        *header++ = (size >> 8) & 0xff;             /* 8: bit */
        *header++ = size & 0xff;
        f->headerSize = 4; /* 4: ws header offset */
    } else {
        *header++ = 127 | (f->isMasked ? 0x80 : 0); /* 127, 0x80: ws payload, mask */

        *(uint32_t *)header = 0;
        header += 4; /* 4: header offset */

        *header++ = (size >> 24) & 0xff; /* 24: bit */
        *header++ = (size >> 16) & 0xff; /* 16: bit */
        *header++ = (size >> 8) & 0xff;  /* 8: bit */
        *header++ = size & 0xff;
        f->headerSize = 10; /* 10: header offset */
    }

    if (f->isMasked) {
        if (memcpy_s(header, headerSize - f->headerSize, f->mask, sizeof(f->mask))) {
            WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("memcpy fail\n"));
        }
        f->headerSize += sizeof(f->mask);
    }
}

static void WsFrameCreateBinHeader(
    WsFrame *f, uint8_t *header, uint32_t headerSize, const size_t length, WsBinary binType)
{
    const uint32_t size = (uint32_t)length;

    if (binType == WS_BINARY_START) {
        *header++ = 0x00 | f->opcode;
    } else if (binType == WS_BINARY_CONTINUE) {
        *header++ = 0x00 | f->opcode;
    } else if (binType == WS_BINARY_END) {
        *header++ = 0x80 | f->opcode; /* 0x80: ws header mask */
    } else {
        *header++ = 0x80 | f->opcode; /* 0x80: ws header mask */
    }

    if (size < 126) {                                         /* 126: ws payload */
        *header++ = (size & 0xff) | (f->isMasked ? 0x80 : 0); /* 0x80: ws mask */
        f->headerSize = 2;                                    /* 2: ws headersize */
    } else if (size < 65536) {                                /* 65536: 16 bit size max */
        *header++ = 126 | (f->isMasked ? 0x80 : 0);           /* 126, 0x80: ws payload, mask */
        *header++ = (size >> 8) & 0xff;                       /* 8: bits */
        *header++ = size & 0xff;
        f->headerSize = 4; /* 4: ws headersize */
    } else {
        *header++ = 127 | (f->isMasked ? 0x80 : 0); /* 127, 0x80: ws payload */

        *(uint32_t *)header = 0;
        header += 4; /* 4: ws headersize */

        *header++ = (size >> 24) & 0xff; /* 24: bits */
        *header++ = (size >> 16) & 0xff; /* 16: bits */
        *header++ = (size >> 8) & 0xff;  /* 8: bits */
        *header++ = size & 0xff;
        f->headerSize = 10; /* 10: headersize */
    }

    if (f->isMasked) {
        if (memcpy_s(header, headerSize - f->headerSize, f->mask, sizeof(f->mask)) != 0) {
            WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("memcpy fail\n"));
        }
        f->headerSize += sizeof(f->mask);
    }
}

void WsFrameFillWithSendData(WsFrame *f, const uint8_t *data, const size_t length, bool isFinish)
{
    uint8_t header[16]; /* 16: header size */
    uint8_t *frame = NULL;
    uint8_t mask[4]; /* 4: mask length */
    size_t index = 0;

    f->isFinished = isFinish;
    WsFrameCreateHeader(f, header, sizeof(header), length);
    f->length = length + f->headerSize;
    f->data = malloc(f->length);
    if (f->data == NULL) {
        WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("malloc fail\n"));
        return;
    }
    frame = (uint8_t *)f->data;
    if (memcpy_s(frame, f->length, header, f->headerSize) != 0) {
        WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("memcpy_s fail\n"));
        free(f->data);
        f->data = NULL;
        return;
    }

    if (data == NULL) {
        return;
    }
    frame += f->headerSize;
    if (memcpy_s(frame, (f->length - f->headerSize), data, length) != 0) {
        WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("memcpy_s fail\n"));
        free(f->data);
        f->data = NULL;
        return;
    }

    if (f->isMasked) {
        if (memcpy_s(mask, sizeof(mask), &f->mask, sizeof(mask)) != 0) {
            WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("memcpy_s fail\n"));
            free(f->data);
            f->data = NULL;
            return;
        }
        for (index = 0; index < length; index++) {
            *frame = *frame ^ mask[index & 0x3]; /* 0x3: mask */
            frame++;
        }
    }
}

void WsFrameFillWithSendBinData(WsFrame *f, const uint8_t *data, const size_t length, WsBinary binType)
{
    uint8_t header[16]; /* 16: header size */
    uint8_t *frame = NULL;
    uint8_t mask[4]; /* 4: mask length */
    size_t index = 0;

    WsFrameCreateBinHeader(f, header, sizeof(header), length, binType);

    // check bin header data
    //   -----------------------------------------------
    //    F  |   |   |   |   |   |   |   |
    //    I  | 0 | 0 | 0 |    OPCODE     |
    //    N  |   |   |   |   |   |   |   |

    f->length = length + f->headerSize;
    f->data = malloc(f->length);
    if (f->data == NULL) {
        WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("malloc fail\n"));
        return;
    }
    frame = (uint8_t *)f->data;
    if (memcpy_s(frame, f->length, header, f->headerSize)) {
        WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("memcpy_s fail\n"));
        free(f->data);
        f->data = NULL;
        return;
    }

    f->isFinished = (binType == WS_BINARY_END);
    if (data == NULL) {
        free(f->data);
        f->data = NULL;
        return;
    }
    frame += f->headerSize;
    if (memcpy_s(frame, (f->length - f->headerSize), data, length) != 0) {
        WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("memcpy_s fail\n"));
        free(f->data);
        f->data = NULL;
        return;
    }

    if (f->isMasked) {
        if (memcpy_s(mask, sizeof(mask), f->mask, sizeof(mask)) != 0) {
            WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("memcpy_s fail\n"));
            free(f->data);
            f->data = NULL;
            return;
        }
        for (index = 0; index < length; index++) {
            *frame = *frame ^ mask[index & 0x3]; /* 0x3: mask */
            frame++;
        }
    }
}

void WsFrameCombineDatas(WsFrame *to, WsFrame *from)
{
    uint8_t *combData = (uint8_t *)malloc(to->length + from->length);
    uint8_t *originalCombData = combData;
    if (combData == NULL) {
        WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("malloc fail\n"));
        return;
    }

    if (to->data && to->length) {
        if (memcpy_s(combData, (to->length + from->length), to->data, to->length) != 0) {
            WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("memcpy_s fail\n"));
            free(originalCombData);
            combData = NULL;
            return;
        }
    }
    combData += to->length;
    if (from->data && from->length) {
        if (memcpy_s(combData, from->length, from->data, from->length) != 0) {
            WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("memcpy_s fail\n"));
            free(originalCombData);
            combData = NULL;
            return;
        }
    }
    if (to->data != NULL) {
        free(to->data);
    }
    to->data = originalCombData;
    to->length += from->length;
}

void WsInitFrameRandom(struct WebSocketClient *clientPtr)
{
    if (clientPtr == NULL) {
        return;
    }

    mbedtls_entropy_init(&clientPtr->randomEntropy);
    mbedtls_ctr_drbg_init(&clientPtr->randomCtrDrbg);

    char pers[] = "random_identifier";
    int ret = mbedtls_ctr_drbg_seed(&clientPtr->randomCtrDrbg,
        mbedtls_entropy_func,
        &clientPtr->randomEntropy,
        (unsigned char *)pers,
        strlen(pers));
    if (ret != 0) {
        WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("mbedtls_ctr_drbg_seed() failed, value:-0x%x.", -ret));
    }
}

void WsFreeRandom(struct WebSocketClient *clientPtr)
{
    if (clientPtr == NULL) {
        return;
    }
    mbedtls_ctr_drbg_free(&clientPtr->randomCtrDrbg);
    mbedtls_entropy_free(&clientPtr->randomEntropy);
}

static int WsGetSecurityRandom(mbedtls_ctr_drbg_context *ctrDrbg, uint32_t *randomNum)
{
    int ret = mbedtls_ctr_drbg_random(ctrDrbg, (unsigned char *)randomNum, sizeof(uint32_t));
    if (ret != 0) {
        WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("mbedtls_ctr_drbg_random() failed, value:-0x%x.", -ret));
    }
    return ret;
}

WsFrame *WsFrameCreate(struct WebSocketClient *clientPtr)
{
    if (clientPtr == NULL) {
        return NULL;
    }
    WsFrame *frame = (WsFrame *)calloc(1, sizeof(WsFrame));
    if (frame == NULL) {
        return NULL;
    }

    union {
        uint32_t ui;
        uint8_t b[4]; /* 4: mask len */
    } maskUnion;

    int ret = WsGetSecurityRandom(&clientPtr->randomCtrDrbg, &maskUnion.ui);
    if (ret != 0) {
        free(frame);
        return NULL;
    } 
    if (memcpy_s(frame->mask, sizeof(frame->mask), maskUnion.b, sizeof(frame->mask))) {
        free(frame);
        return NULL;
    };
    return frame;
}

void WsDeleteFrame(WsFrame *frame)
{
    if (frame != NULL && frame->data != NULL) {
        free(frame->data);
        frame->data = NULL;
    }
    if (frame != NULL) {
        free(frame);
    }
}

size_t WsCheckRecvFrameSize(const uint8_t *data, const size_t length)
{
    if (data == NULL || length < 2) { /* 2: data length */
        return 0;
    }

    const uint8_t *udata = (const uint8_t *)data;
    const uint32_t isMasked = (udata[1] >> 7) & 0x01; /* 7: bits */
    const uint32_t payload = udata[1] & 0x7f;         /* 0x7f : mask */
    uint32_t headerSize = isMasked ? 6 : 2;           /* 6, 2: header size */

    size_t expectedSize = 0;

    switch (payload) {
        case 126:            /* 126: ws payload */
            headerSize += 2; /* 2: offset */
            break;
        case 127:            /* 127: ws payload */
            headerSize += 8; /* 8: offset */
            break;
        default:
            break;
    }

    if (length < headerSize) {
        return 0;
    }

    switch (payload) {
        case 126:                                      /* 126: ws payload */
            expectedSize |= ((uint32_t)udata[2]) << 8; /* 2,8: udata offset */
            expectedSize |= (uint32_t)udata[3];        /* 3: udata offset */
            break;

        case 127:                                       /* 127: ws payload */
            expectedSize |= be64toh(*(uint64_t *)(udata + 2)); /* 2: offset */
            break;

        default:
            if (payload <= 125) { /* 125: payload */
                expectedSize = payload;
            }
            break;
    }

    const uint32_t nPackSize = expectedSize + headerSize;
    return (nPackSize <= length) ? nPackSize : 0;
}
