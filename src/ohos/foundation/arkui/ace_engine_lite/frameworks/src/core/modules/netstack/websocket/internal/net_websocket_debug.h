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

#ifndef __NET_WEBSOCKET_DEBUG_H__
#define __NET_WEBSOCKET_DEBUG_H__

#include <stdio.h>

#define WS_DBG_LEVEL WEBSOCKET_DBG_OFF

#define WS_INFO_LEVEL WEBSOCKET_DBG_ON

#define WS_ERR_LEVEL WEBSOCKET_DBG_ON

/** flag for WEBSOCKET_DEBUGF to enable that debug message */
#define WEBSOCKET_DBG_ON 0x80U
/** flag for WEBSOCKET_DEBUGF to disable that debug message */
#define WEBSOCKET_DBG_OFF 0x00U

#define WEBSOCKET_DEBUG

#ifdef WEBSOCKET_DEBUG
#define WS_PLATFORM_DIAG(x) \
    do {                    \
        printf x;           \
    } while (0)
#define WEBSOCKET_DEBUGF(debug, message)  \
    do {                                  \
        if (((debug)&WEBSOCKET_DBG_ON)) { \
            WS_PLATFORM_DIAG(message);    \
        }                                 \
    } while (0)

#else /* WEBSOCKET_DEBUG */
#define WEBSOCKET_DEBUGF(debug, message)
#endif /* WEBSOCKET_DEBUG */

#endif