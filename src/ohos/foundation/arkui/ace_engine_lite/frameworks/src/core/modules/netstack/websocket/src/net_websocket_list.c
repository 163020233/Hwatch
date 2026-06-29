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

#include "net_websocket_type.h"
#include "net_websocket_list.h"

WsList *WsListCreate(void)
{
    return (WsList *)calloc(1, sizeof(WsList));
}

void WsListDelete(WsList *list)
{
    WsList *cur = list;
    while (cur) {
        WsList *prev = cur;
        cur = cur->next;
        free(prev);
    }
}

void WsListDeleteClean(WsList **list)
{
    if (list) {
        WsListDelete(*list);
        *list = NULL;
    }
}

int WsListAppend(WsList *list, WsNodeValue value)
{
    if (list == NULL) {
        return WEBSOCKET_NO_MEMORY;
    }
    WsList *cur = list;
    while (cur->next) {
        cur = cur->next;
    }
    cur->next = (WsNode *)calloc(1, sizeof(WsNode));
    if (cur->next == NULL) {
        return WEBSOCKET_NO_MEMORY;
    }
    cur->next->value = value;

    return WEBSOCKET_OK;
}
