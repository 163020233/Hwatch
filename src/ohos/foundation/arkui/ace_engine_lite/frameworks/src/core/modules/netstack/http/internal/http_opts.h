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

#ifndef HTTP_CLIENT_OPTS_H
#define HTTP_CLIENT_OPTS_H

#ifndef HTTPCLIENT_BASIC_AUTH_SIZE
#define HTTPCLIENT_BASIC_AUTH_SIZE     128
#endif

#ifndef HTTPCLIENT_CHUNK_BUF_SIZE
#define HTTPCLIENT_CHUNK_BUF_SIZE     (2048) // 1024
#endif

#ifndef HTTPCLIENT_SEND_BUF_SIZE
#define HTTPCLIENT_SEND_BUF_SIZE  2048 // 512
#endif

#ifndef HTTPCLIENT_MAX_HOST_LEN
#define HTTPCLIENT_MAX_HOST_LEN   128
#endif

#ifndef HTTPCLIENT_MAX_URL_LEN
#define HTTPCLIENT_MAX_URL_LEN    1024 // 512
#endif

#ifndef HTTPCLIENT_MAX_FILE_DATA_LEN
#define HTTPCLIENT_MAX_FILE_DATA_LEN 2048
#endif

#ifndef HTTPCLIENT_MAX_RECVDATA_WAIT_MS
#define HTTPCLIENT_MAX_RECVDATA_WAIT_MS 5000
#endif

#ifndef HTTP_SERVERPORT
#define HTTP_SERVERPORT   80
#endif

#ifndef HTTPS_SERVERPORT
#define HTTPS_SERVERPORT  443
#endif

#endif /* HTTP_CLIENT_OPTS_H */