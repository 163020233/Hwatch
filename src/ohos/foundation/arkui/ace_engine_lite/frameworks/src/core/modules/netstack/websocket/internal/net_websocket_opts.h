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
 
#ifndef NET_WEBSOCKET_OPTS_H
#define NET_WEBSOCKET_OPTS_H

#define WEBSOCKET_RECV_BUFF_SIZE (8 * 1024)

#define WS_MAX_THRESH_VAL (8 * 1024) /* common TTS play size */

#define WS_MAX_CLIENT_NUM 4

#define WS_MAX_DATA_LENGTH (20 * 1024)

#define WSS_READ_TIMEOUT_MS 100

#define WS_THREAD_STACK_SIZE (6 * 1024)

#define WS_THREAD_SLEEP_MS 100

#define WS_IDLE_WAIT_MS OSAL_EVENT_FOREVER

#define WS_MONITOR_SLEEP_MS 50

#define WS_SELECT_TIMEOUT_S 60

#endif