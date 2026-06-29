/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef LWIP_WRAPPER_H
#define LWIP_WRAPPER_H

// 在包含lwip头文件之前，保存当前环境
#ifdef bind
#define LWIP_BIND_DEFINED
#endif

#ifdef connect
#define LWIP_CONNECT_DEFINED
#endif

#ifdef send
#define LWIP_SEND_DEFINED
#endif

#ifdef recv
#define LWIP_RECV_DEFINED
#endif

#ifdef close
#define LWIP_CLOSE_DEFINED
#endif

// 包含lwip头文件
#include "lwip/sockets.h"
#include "lwip/netdb.h"

// 取消冲突的宏定义
#undef bind
#undef connect
#undef send
#undef recv
#undef close
#undef socket
#undef accept
#undef listen
#undef select

// 恢复宏定义（如果需要）
#ifdef LWIP_BIND_DEFINED
#define bind lwip_bind
#endif

#ifdef LWIP_CONNECT_DEFINED
#define connect lwip_connect
#endif

#ifdef LWIP_SEND_DEFINED
#define send lwip_send
#endif

#ifdef LWIP_RECV_DEFINED
#define recv lwip_recv
#endif

#ifdef LWIP_CLOSE_DEFINED
#define close lwip_close
#endif

extern "C" {
// 这些函数在lwip中通常有声明，但为了确保，我们显式声明
int lwip_getsockopt(int s, int level, int optname, void *optval, socklen_t *optlen);
int lwip_setsockopt(int s, int level, int optname, const void *optval, socklen_t optlen);
int lwip_getsockname(int s, struct sockaddr *name, socklen_t *namelen);
int lwip_getpeername(int s, struct sockaddr *name, socklen_t *namelen);
}
#endif  // LWIP_WRAPPER_H