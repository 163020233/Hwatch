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

#ifndef _NET_WS_FRAME_H__
#define _NET_WS_FRAME_H__

#include <stdint.h>
#include "net_websocket.h"
#include "net_websocket_priv.h"

#ifdef __cplusplus
extern "C" {
#endif
typedef enum {
    WS_OPCODE_CONTINUATION = 0x0,      // %x0 denotes a continuation frame
    WS_OPCODE_TEXT_FRAME = 0x1,        // %x1 denotes a text frame
    WS_OPCODE_BINARY_FRAME = 0x2,      // %x2 denotes a binary frame
    WS_OPCODE_CONNECTION_CLOSE = 0x8,  // %x8 denotes a connection close
    WS_OPCODE_PING = 0x9,              // %x9 denotes a ping
    WS_OPCODE_PONG = 0xA               // %xA denotes a pong
} WsOpcode;

typedef struct WsFrameStruct {
    uint8_t *data;
    size_t length;
    WsOpcode opcode;
    uint8_t mask[4]; /* 4: mask length */
    bool isMasked;
    bool isFinished;
    uint8_t headerSize;
} WsFrame;

size_t WsCheckRecvFrameSize(const uint8_t *data, const size_t data_size);

WsFrame *WsFrameCreateWithRecvData(struct WebSocketClient *clientPtr, const uint8_t *data, const size_t length);

void WsFrameFillWithSendBinData(WsFrame *frame, const uint8_t *data, const size_t length, WsBinary binType);

// data - should be null, and setted by newly created. 'data' & 'data_size' can be null
void WsFrameFillWithSendData(WsFrame *frame, const uint8_t *data, const size_t data_size, bool is_finish);

// combine datas of 2 frames. combined is 'to'
void WsFrameCombineDatas(WsFrame *to, WsFrame *from);

WsFrame *WsFrameCreate(struct WebSocketClient *clientPtr);

void WsDeleteFrame(WsFrame *frame);

void WsInitFrameRandom(struct WebSocketClient *clientPtr);

void WsFreeRandom(struct WebSocketClient *clientPtr);

#ifdef __cplusplus
}
#endif

#endif
