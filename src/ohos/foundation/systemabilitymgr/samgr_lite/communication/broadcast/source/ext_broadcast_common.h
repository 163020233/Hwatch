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

#ifndef EXT_BROADCAST_COMMON_H
#define EXT_BROADCAST_COMMON_H

#include "broadcast_interface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define CORE_MASK(core) (0x1 << (core))
#define PLATFORM_CORE_MASK (0x1 << 0)

typedef enum CoreMask {
    A_CORE_MASK = 0x1 << 0,
} CoreMask;

typedef struct TopicToCore {
    Topic topic;        /* 对应topic，从10000开始 */
    uint8 coreMask;     /* 对应topic，需要发往的核 */
    uint8 coreCount;    /* 需要发往的核个数 */
} TopicToCore;

typedef struct TopicToCoreTable {
    uint16 count;
    uint16 capSize;
    const TopicToCore *topics;
    uint32 *bitmap;
} TopicToCoreTable;

typedef struct SubscriberTable {
    uint16 count;
    uint16 capSize;
    Subscriber *subscribers;
} SubscriberTable;

typedef struct TopicSubscriberInfo {
    TopicToCoreTable topicTable;
    SubscriberTable subscriberTable;
    MutexId mutex;
} TopicSubscriberInfo;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif // LITE_PUB_SUB_IMPLEMENT_H
