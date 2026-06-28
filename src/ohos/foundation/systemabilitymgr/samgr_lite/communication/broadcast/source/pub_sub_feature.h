/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#ifndef LITE_PUB_SUB_FEATURE_H
#define LITE_PUB_SUB_FEATURE_H

#include <stdbool.h>
#include "message.h"
#include "feature.h"
#include "common.h"
#include "broadcast_interface.h"
#include "ext_broadcast_common.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum {
    TOPIC_START_NUM = 10000,
    TOPIC_DEMO_ACORE_MSG = TOPIC_START_NUM,
    TOPIC_DEMO_ALL_MSG,
    TOPIC_EVENT_ALARM_RING,
    TOPIC_EVENT_UTCTIME_UPDATE,
    TOPIC_EVENT_NEXTDAY_UPDATE,
    TOPIC_EVENT_WEARSTATE_UPDATE,
    TOPIC_EVENT_STEPDATA_UPDATE,
    TOPIC_EVENT_SHUTDOWN,
    TOPIC_EVENT_HEARTRATE_UPDATA,
    TOPIC_EVENT_HFP_INCOMMING,
    TOPIC_EVENT_HFP_PHONE_ACTIVE,
    TOPIC_EVENT_HFP_WATCH_ACTIVE,
    TOPIC_EVENT_HFP_FINISHED,
    TOPIC_EVENT_HFP_OUTGOING,
    TOPIC_EVENT_HFP_PHONE_TO_WATCH,
    TOPIC_EVENT_HFP_WATCH_TO_PHONE,
    TOPIC_EVENT_BLOODOXYGEN_UPDATA,
    TOPIC_EVENT_SPORT_DISTANCE,
    TOPIC_EVENT_CALENDAR_NOTIFY,
    TOPIC_EVENT_BT_CONNECTED,     // 蓝牙连接完成消息
    TOPIC_EVENT_BT_DISCONNECTED,  // 蓝牙断连完成消息
    TOPIC_EVENT_TWS_CARD_POP,
    TOPIC_EVENT_TWS_CARD_CONNECT,
    TOPIC_END_NUM = 20000,
} TopicNum;

/* topic数量 */
#ifndef MAX_BROACCAST_TOPIC_NUMBER
#define MAX_BROACCAST_TOPIC_NUMBER 32
#endif

/* 每个topic 对应的bitmap长度 */
#ifndef MAX_BROADCAST_BITMAP_NUMBER
#define MAX_BROADCAST_BITMAP_NUMBER  64
#endif
#if (MAX_BROADCAST_BITMAP_NUMBER > 255)
#warning Not support subscriber more than 255!
#define MAX_BROADCAST_BITMAP_NUMBER  255
#endif
/* int32类型对应的位图长度 */
#define BITMAP_INT32_SIZE       32
/* 计算每个topic，bitmap需要多少int32. (采用四字节int32，相比int8，遍历时有优势) */
#define BITMAP_ARRAY_ENTRY_SIZE ((((MAX_BROADCAST_BITMAP_NUMBER) + 31) & ~31) >> 5)
/* 计算所有topic，对应int32数组长 */
#define BITMAP_ARRAY_SIZE       (BITMAP_ARRAY_ENTRY_SIZE * MAX_BROACCAST_TOPIC_NUMBER)

typedef struct UTILS_DL_LIST {
    struct UTILS_DL_LIST *pstPrev; /* < Current node's pointer to the previous node */
    struct UTILS_DL_LIST *pstNext; /* < Current node's pointer to the next node */
} UTILS_DL_LIST;

typedef struct ConsumerNode {
    UTILS_DL_LIST node;
    Consumer *consumer;
} ConsumerNode;

typedef struct Relation {
    UTILS_DL_LIST node;
    ConsumerNode callbacks;
    Topic topic;
} Relation;

#define UTILS_OFF_SET_OF(type, member) ((unsigned int)&((type *)0)->member)

#define UTILS_DL_LIST_ENTRY(item, type, member) \
    ((type *)(void *)((char *)(item) - UTILS_OFF_SET_OF(type, member)))

#define UTILS_DL_LIST_FOR_EACH_ENTRY(item, list, type, member)             \
    for ((item) = UTILS_DL_LIST_ENTRY((list)->pstNext, type, member);        \
         &(item)->member != (list);                                      \
         (item) = UTILS_DL_LIST_ENTRY((item)->member.pstNext, type, member))

static inline void UtilsListInit(UTILS_DL_LIST *list)
{
    list->pstNext = list;
    list->pstPrev = list;
}

static inline void UtilsListAdd(UTILS_DL_LIST *list, UTILS_DL_LIST *node)
{
    node->pstNext = list->pstNext;
    node->pstPrev = list;
    list->pstNext->pstPrev = node;
    list->pstNext = node;
}

static inline void UtilsListDelete(UTILS_DL_LIST *node)
{
    node->pstNext->pstPrev = node->pstPrev;
    node->pstPrev->pstNext = node->pstNext;
    node->pstNext = NULL;
    node->pstPrev = NULL;
}

static inline bool UtilsListEmpty(UTILS_DL_LIST *list)
{
    return (bool)(list->pstNext == list);
}
typedef struct PubSubFeature PubSubFeature;
struct PubSubFeature {
    INHERIT_FEATURE;
    Relation *(*GetRelation)(PubSubFeature *feature, const Topic *topic);
    MutexId mutex;
    Relation relations;
    Identity identity;
    TopicSubscriberInfo topicSubscriberInfo;
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif // LITE_PUB_SUB_FEATURE_H
