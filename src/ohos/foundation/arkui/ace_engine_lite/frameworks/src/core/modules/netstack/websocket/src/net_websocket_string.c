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

#include "net_websocket_string.h"
#include "securec.h"

char *WsStringCopy(const char *str)
{
    return str ? WsStringCopyLen(str, strlen(str)) : NULL;
}

char *WsStringCopyLen(const char *str, const size_t len)
{
    char *copy = (str && len > 0) ? (char *)malloc(len + 1) : NULL;
    if (copy != NULL) {
        if (memcpy_s(copy, len + 1, str, len) != 0) {
            free(copy);
            copy = NULL;
            return NULL;
        }
        copy[len] = 0;
        return copy;
    }
    return NULL;
}

void WsStringDelete(char *str)
{
    free(str);
}

void WsStringDeleteClean(char **str)
{
    if (str != NULL && *str != NULL) {
        free(*str);
        *str = NULL;
    }
}
