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

#ifndef HTTP_CLIENT_WRAPPER_H
#define HTTP_CLIENT_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

#define http_info printf
#define http_err printf
#ifdef HTTP_DEBUG
#define http_debug printf
#endif

#ifndef HTTP_MIN
#define HTTP_MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif
#ifndef HTTP_MAX
#define HTTP_MAX(x, y) (((x) > (y)) ? (x) : (y))
#endif

int HttpTcpConnWrapper(HttpClient *client, const char *host);
int HttpTcpCloseWrapper(HttpClient *client);
int HttpTcpSendWrapper(HttpClient *client, const char *data, int length);
int HttpTcpRecvWrapper(HttpClient *client, char *buf, int buflen, int timeout_ms, int *p_read_len);
int HttpTcpSseRecv(HttpClient *client, char *buffer, int buflen, int *bytesRead);

#if CONFIG_HTTP_SECURE
int HttpSslConnWrapper(HttpClient *client, const char *host);
int HttpSslCloseWrapper(HttpClient *client);
int HttpSslSendWrapper(HttpClient *client, const char *data, size_t length);
int HttpSslRecvWrapper(HttpClient *client, char *buf, int buflen, int timeout_ms, int *p_read_len);
int HttpSslSseRecv(HttpClient *client, char *buffer, int buflen, int *bytesRead);
#endif

#ifdef __cplusplus
}
#endif

#endif  /* HTTP_CLIENT_WRAPPER_H */