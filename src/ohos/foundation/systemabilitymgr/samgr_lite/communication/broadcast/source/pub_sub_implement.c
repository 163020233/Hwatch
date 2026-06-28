/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
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

#include <log.h>
#include "pub_sub_implement.h"
#include "securec.h"
#include "ohos_errno.h"
#include "memory_adapter.h"
#include "thread_adapter.h"
static int AddTopic(IUnknown *iUnknown, const Topic *topic);
static int Subscribe(IUnknown *iUnknown, const Topic *topic, Consumer *consumer);
static Consumer *ModifyConsumer(IUnknown *iUnknown, const Topic *topic, Consumer *oldConsumer, Consumer *newConsumer);
static Consumer *Unsubscribe(IUnknown *iUnknown, const Topic *topic, const Consumer *consumer);
static BOOL Publish(IUnknown *iUnknown, const Topic *topic, uint8 *data, int16 len);
static void DefaultHandle(const Request *request, const Response *response);
static BOOL ImmediatelyPublish(PubSubFeature *feature, const Topic *topic, const Request *request);
static int32 SubscribeTopic(IUnknown *broadcastApi, const Topic topic, const Subscriber* subscriber);
static int32 UnsubscribeTopic(IUnknown *broadcastApi, const Topic topic, const Subscriber* subscriber);
static int32 GetTopicBitmap(IUnknown *broadcastApi, const Topic topic, uint32 *bitmaps, int16 size);
static int32 PublishTopic(IUnknown *broadcastApi, Request *msg);

static PubSubImplement g_pubSubImplement = {
    DEFAULT_IUNKNOWN_ENTRY_BEGIN,
    .subscriber.AddTopic = AddTopic,
    .subscriber.Subscribe = Subscribe,
    .subscriber.ModifyConsumer = ModifyConsumer,
    .subscriber.Unsubscribe = Unsubscribe,
    .provider.Publish = Publish,
    .SubscribeTopic = SubscribeTopic,
    .UnsubscribeTopic = UnsubscribeTopic,
    .GetTopicBitmap = GetTopicBitmap,
    .PublishTopic = PublishTopic,
    DEFAULT_IUNKNOWN_ENTRY_END,
    .feature = NULL
};

PubSubImplement *BCE_CreateInstance(Feature *feature)
{
    g_pubSubImplement.feature = (PubSubFeature *)feature;
    return &g_pubSubImplement;
}

#define INVALID_INDEX (-1)
#define array_size(x) (sizeof((x)) / sizeof((x)[0]))

/* 当前所有可用topic，不应该出现相同topic在不同位置 !!! 严格按照顺序填写，可以优化获取topic对应的索引 !!! */
static const TopicToCore topicToCoreArray[] = {
    {TOPIC_DEMO_ACORE_MSG, A_CORE_MASK, 1},
    {TOPIC_DEMO_ALL_MSG, A_CORE_MASK, 1},
    {TOPIC_EVENT_ALARM_RING, A_CORE_MASK, 1},
    {TOPIC_EVENT_UTCTIME_UPDATE, A_CORE_MASK, 1},
    {TOPIC_EVENT_NEXTDAY_UPDATE, A_CORE_MASK, 1},
    {TOPIC_EVENT_WEARSTATE_UPDATE, A_CORE_MASK, 1},
    {TOPIC_EVENT_STEPDATA_UPDATE, A_CORE_MASK, 1},
    {TOPIC_EVENT_SHUTDOWN, A_CORE_MASK, 1},
    {TOPIC_EVENT_HEARTRATE_UPDATA, A_CORE_MASK, 1},
    {TOPIC_EVENT_HFP_INCOMMING, A_CORE_MASK, 1},
    {TOPIC_EVENT_HFP_PHONE_ACTIVE, A_CORE_MASK, 1},
    {TOPIC_EVENT_HFP_WATCH_ACTIVE, A_CORE_MASK, 1},
    {TOPIC_EVENT_HFP_FINISHED, A_CORE_MASK, 1},
    {TOPIC_EVENT_HFP_OUTGOING, A_CORE_MASK, 1},
    {TOPIC_EVENT_HFP_PHONE_TO_WATCH, A_CORE_MASK, 1},
    {TOPIC_EVENT_HFP_WATCH_TO_PHONE, A_CORE_MASK, 1},
    {TOPIC_EVENT_BLOODOXYGEN_UPDATA, A_CORE_MASK, 1},
    {TOPIC_EVENT_SPORT_DISTANCE, A_CORE_MASK, 1},
    {TOPIC_EVENT_BT_CONNECTED, A_CORE_MASK, 1},
    {TOPIC_EVENT_BT_DISCONNECTED, A_CORE_MASK, 1},
    {TOPIC_EVENT_TWS_CARD_POP, A_CORE_MASK, 1},
    {TOPIC_EVENT_TWS_CARD_CONNECT, A_CORE_MASK, 1},
};

/* 所有的topic，其对应的位图 */
static uint32 topicBitmapArray[BITMAP_ARRAY_SIZE] = {0};
/* 所有的topic，其订阅者信息 */
static Subscriber subscriberArray[MAX_BROADCAST_BITMAP_NUMBER] = {};

/* 获取对应核的个数 */
inline static int8 GetTopicCoreNum(uint16 coreMask)
{
    int8 count = 0;
    uint16 mask = coreMask;
    while (mask != 0) {
        mask &= (mask - 1); // 清除最低位的1
        ++count;
    }
    return count;
}

/* 通过topic查找对应索引，当前使用遍历方式。 !!! 后续topic与topicIndex相差10000，可以直接优化!!! */
inline static int16 GetTopicIndex(const TopicToCore *topicToCore, int16 size, Topic topic)
{
    for (int16 i = 0; i < size; ++i) {
        if (topicToCore[i].topic == topic) {
            return i;
        }
    }
    return INVALID_INDEX;
}

/* 通过target，查找器对应的索引。 !!!比较所有的对象，其中identity需要注意，仅比较指针!!! */
inline static int16 GetSubscriberIndex(const Subscriber* subscribers, int size, const Subscriber* target)
{
    for (int16 i = 0; i < size; ++i) {
        if (subscribers[i].identity == target->identity &&
            subscribers[i].args == target->args &&
            subscribers[i].Notify == target->Notify) {
            return i;
        }
    }
    return INVALID_INDEX;
}

/* 某个topic，设置其subscriber订阅 */
inline static void SetBitmap(uint32 *bitmap, int16 topicIndex, int16 subIndex)
{
    int16 offset = topicIndex * BITMAP_ARRAY_ENTRY_SIZE;
    offset += subIndex / BITMAP_INT32_SIZE;
    int16 mod = subIndex % BITMAP_INT32_SIZE;
    bitmap[offset] |= (0x1 << mod);
    HILOG_DEBUG(HILOG_MODULE_SAMGR, "topicIndex=%u, subIndex=%u, offset=%u, mod=%u, bitmap=0x%x",
        topicIndex, subIndex, offset, mod, bitmap[offset]);
}

/* 某个topic，清理其subscriber订阅 */
inline static void ClearBitmap(uint32 *bitmap, int16 topicIndex, int16 subIndex)
{
    int16 offset = topicIndex * BITMAP_ARRAY_ENTRY_SIZE;
    offset += subIndex / BITMAP_INT32_SIZE;
    int16 mod = subIndex % BITMAP_INT32_SIZE;
    bitmap[offset] &= ~(0x1 << mod);
    HILOG_DEBUG(HILOG_MODULE_SAMGR, "topicIndex=%u, subIndex=%u, offset=%u, mod=%u, bitmap=0x%x",
        topicIndex, subIndex, offset, mod, bitmap[offset]);
}

/* 某个topic，判断某个subscriber是否订阅 */
inline static BOOL GetBitmap(const uint32 *bitmap, int16 topicIndex, int16 subIndex)
{
    int16 offset = topicIndex * BITMAP_ARRAY_ENTRY_SIZE;
    offset += subIndex / BITMAP_INT32_SIZE;
    int16 mod = subIndex % BITMAP_INT32_SIZE;
    HILOG_DEBUG(HILOG_MODULE_SAMGR, "topicIndex=%u, subIndex=%u, offset=%u, mod=%u, bitmap=0x%x",
        topicIndex, subIndex, offset, mod, bitmap[offset]);
    return bitmap[offset] & (0x1 << mod);
}

/* 获取位图最小位的索引 */
inline static int16 GetLowestBitIndex(uint32 bitmap)
{
    uint32 index = (bitmap - 1) & (~bitmap); /* eg: bitmap is 01010000, then the index is 00001111 */
    /* 每次一半一半的1加起来 */
    /* 1 : 2个为一个单位，一个单位分半后加起来 */
    index = (index & 0x55555555) + ((index >> 1) & 0x55555555);     /* 0x55555555: 5555 is 0101010101010101 */
    /* 2 : 4个为一个单位，一个单位分半后加起来 */
    index = (index & 0x33333333) + ((index >> 2) & 0x33333333);     /* 0x33333333: 3333 is 0011001100110011 */
    /* 4 : 8个为一个单位，一个单位分半后加起来 */
    index = (index & 0x0F0F0F0F) + ((index >> 4) & 0x0F0F0F0F);     /* 0x0F0F0F0F: 0f0F is 0000111100001111 */
    /* 8 : 16个为一个单位，一个单位分半后加起来 */
    index = (index & 0x00FF00FF) + ((index >> 8) & 0x00FF00FF);     /* 0x00FF00FF: 00FF is 0000000011111111 */
    /* 16 : 32个为一个单位，一个单位分半后加起来 */
    index = (index & 0x0000FFFF) + ((index >> 16) & 0x0000FFFF);    /* 0x0000FFFF: half + half */
    return (int16)index;
}

/* 主题需要核内通信 */
inline static BOOL IsInnerCore(CoreMask mask)
{
    return (mask & PLATFORM_CORE_MASK);
}

/* 核内发送本地消息,直接通过identify发送给对方服务 */
static BOOL SendLocalMessage(const Identity *id, Request *req)
{
    HILOG_DEBUG(HILOG_MODULE_SAMGR, "id=0x%p, serviceId=%u, featureId=%u, queueId=%u",
        id, id->serviceId, id->featureId, id->queueId);
    HILOG_DEBUG(HILOG_MODULE_SAMGR, "req=0x%p, msgId=%u, len=%u, data=0x%p",
        req, req->msgId, req->len, req->data);
    int32 ret = SAMGR_SendRequest(id, req, NULL);
    if (ret != OHOS_SUCCESS) {
        if (req->data != NULL) {
            free(req->data);
            req->data = NULL;
        }
        HILOG_ERROR(HILOG_MODULE_SAMGR, "SAMGR_SendRequest failed. ret=%d ", ret);
        return FALSE;
    }
    return TRUE;
}

/* 核内通信，将消息发送给通信服务转发，后者直接调用回调函数 */
static BOOL SendInnerMessage(const TopicSubscriberInfo *topicSubscriberInfo, int16 topicIdx, Request *req)
{
    HILOG_DEBUG(HILOG_MODULE_SAMGR, "topicIdx=%u, req=0x%p", topicIdx, req);
    const TopicToCoreTable *topicTable = &(topicSubscriberInfo->topicTable);
    Topic topic = topicTable->topics[topicIdx].topic;
    MUTEX_Lock(topicSubscriberInfo->mutex);
    /* 首先查看是否已经存在相同的结构 */
    int16 offset = topicIdx * BITMAP_ARRAY_ENTRY_SIZE;
    for (int16 i = 0; i < BITMAP_ARRAY_ENTRY_SIZE; ++i) {
        /* 使用优化的方式，遍历bitmap */
        uint32 bitmap = topicTable->bitmap[offset + i];
        while (bitmap != 0) {
            int16 subIdx = GetLowestBitIndex(bitmap);
            subIdx += i * BITMAP_INT32_SIZE;
            HILOG_DEBUG(HILOG_MODULE_SAMGR, "i=%d, bitmap=0x%x, subIdx=%u, topicIdx=%u",
                i, bitmap, subIdx, topicIdx);
            bitmap &= (bitmap - 1); /* 1：清除最低位bit */

            const Subscriber *subscriber = &topicSubscriberInfo->subscriberTable.subscribers[subIdx];
            /* 非空类型，说明注册的回调函数，直接调用对应回调函数 */
            if (subscriber->Notify != NULL) {
                HILOG_DEBUG(HILOG_MODULE_SAMGR, "subIdx=%u, topicIdx=%u, Notify=0x%p, args=0x%p",
                    subIdx, topicIdx, subscriber->Notify, subscriber->args);
                subscriber->Notify(subscriber->args, topic, req);
                continue;
            }

            /* 在当前核内给对应服务发送消息 */
            Request tmpReq = { .msgId =  req->msgId, .len = req->len, .msgValue = req->msgValue, .data = NULL, };
            if (tmpReq.len != 0) {
                tmpReq.data = malloc(tmpReq.len);
            }
            if (tmpReq.data != NULL) {
                (void)memcpy_s(tmpReq.data, tmpReq.len, req->data, req->len);
            }
            if (tmpReq.len != 0 && tmpReq.data == NULL) {
                    break;
            }
            SendLocalMessage(subscriber->identity, &tmpReq);
        }
    }
    if (req->data != NULL) {
        free(req->data);
        req->data = NULL;
    }
    MUTEX_Unlock(topicSubscriberInfo->mutex);
    return TRUE;
}

/* 初始化topic */
int32 InitTopicSubscriberInfoImpl(TopicSubscriberInfo *topicSubscriberInfo)
{
    HILOG_INFO(HILOG_MODULE_SAMGR, "start!");
    topicSubscriberInfo->mutex = MUTEX_InitValue();
    topicSubscriberInfo->topicTable.count = array_size(topicToCoreArray);
    topicSubscriberInfo->topicTable.capSize = array_size(topicToCoreArray);
    topicSubscriberInfo->topicTable.topics = topicToCoreArray;
    topicSubscriberInfo->topicTable.bitmap = topicBitmapArray;
    topicSubscriberInfo->subscriberTable.count = 0;
    topicSubscriberInfo->subscriberTable.capSize = array_size(subscriberArray);
    topicSubscriberInfo->subscriberTable.subscribers = subscriberArray;
    for (uint16 i = 0; i < array_size(topicToCoreArray); ++i) {
        int8 coreCount = GetTopicCoreNum(topicToCoreArray[i].coreMask);
        if (topicToCoreArray[i].coreCount != coreCount) {
            HILOG_WARN(HILOG_MODULE_SAMGR, "coreNum invalid. topicToCoreArray[i].coreCount=%d, \
                          coreCount=%d", topicToCoreArray[i].coreCount, coreCount);
        }
    }
    return OHOS_SUCCESS;
}

/* 某个topic，注册监听者 */
int32 SubscribeTopicImpl(TopicSubscriberInfo *topicSubscriberInfo, Topic topic, const Subscriber *subscriber)
{
    HILOG_DEBUG(HILOG_MODULE_SAMGR, "start! topicSubscriberInfo=0x%p", topicSubscriberInfo);

    int32 ret = OHOS_FAILURE;
    TopicToCoreTable *topicTable = &(topicSubscriberInfo->topicTable);
    int16 topicIdx = GetTopicIndex(topicTable->topics, topicTable->count, topic);
    if (topicIdx == INVALID_INDEX) {
        HILOG_ERROR(HILOG_MODULE_SAMGR, "topic not exist! topic=%u", topic);
        return ret;
    }
    HILOG_DEBUG(HILOG_MODULE_SAMGR, "topic=%u, topicIdx=%u", topic, topicIdx);

    if (!(topicTable->topics[topicIdx].coreMask & PLATFORM_CORE_MASK)) {
        HILOG_ERROR(HILOG_MODULE_SAMGR, "topic not send current core! topic=%u, coreMask=0x%x, \
                      MASK=0x%x", topic, topicTable->topics[topicIdx].coreMask, PLATFORM_CORE_MASK);
        return ret;
    }

    const Identity *identity = subscriber->identity;
    if (identity != NULL) {
        HILOG_DEBUG(HILOG_MODULE_SAMGR, "serviceId=%u, featureId=%u, queueId=%u",
            identity->serviceId, identity->featureId, identity->queueId);
    }
    HILOG_DEBUG(HILOG_MODULE_SAMGR, "args=0x%p, notify=0x%p",
        subscriber->args, subscriber->Notify);

    SubscriberTable *subscriberTable = &(topicSubscriberInfo->subscriberTable);
    MUTEX_Lock(topicSubscriberInfo->mutex);
    do {
        /* 首先查看是否已经存在相同的结构 */
        int16 subIdx = GetSubscriberIndex(subscriberTable->subscribers, subscriberTable->count, subscriber);
        if (subIdx == INVALID_INDEX) {
            /* 所有的订阅者结构使用完 */
            if (subscriberTable->count >= subscriberTable->capSize) {
                HILOG_ERROR(HILOG_MODULE_SAMGR, "subscriber is enough! count=%u, capSize=%u",
                    subscriberTable->count, subscriberTable->capSize);
                break;
            }
            subscriberTable->subscribers[subscriberTable->count] = *subscriber;
            subIdx = subscriberTable->count;
            subscriberTable->count++;
        }
        HILOG_DEBUG(HILOG_MODULE_SAMGR, "topicIdx=%u, subIdx=%u", topicIdx, subIdx);
        if (!GetBitmap(topicTable->bitmap, topicIdx, subIdx)) {
            SetBitmap(topicTable->bitmap, topicIdx, subIdx);
        } else {
            HILOG_WARN(HILOG_MODULE_SAMGR, "already subscribe");
        }
        ret = OHOS_SUCCESS;
    } while (FALSE);
    MUTEX_Unlock(topicSubscriberInfo->mutex);
    return ret;
}

/* 某个topic，清理对应监听者的位图，监听者数组不做处理 */
int32 UnsubscribeTopicsImpl(TopicSubscriberInfo *topicSubscriberInfo, Topic topic, const Subscriber *subscriber)
{
    HILOG_DEBUG(HILOG_MODULE_SAMGR, "start! topicSubscriberInfo=0x%p", topicSubscriberInfo);

    int32 ret = OHOS_FAILURE;
    TopicToCoreTable *topicTable = &(topicSubscriberInfo->topicTable);
    int16 topicIdx = GetTopicIndex(topicTable->topics, topicTable->count, topic);
    if (topicIdx == INVALID_INDEX) {
        HILOG_ERROR(HILOG_MODULE_SAMGR, "topic not exist! topic=%u", topic);
        return ret;
    }
    HILOG_DEBUG(HILOG_MODULE_SAMGR, "topic=%u, topicIdx=%u", topic, topicIdx);

    if (!(topicTable->topics[topicIdx].coreMask & PLATFORM_CORE_MASK)) {
        HILOG_ERROR(HILOG_MODULE_SAMGR, "topic not send current core! topic=%u, coreMask=0x%x, \
                      MASK=0x%x", topic, topicTable->topics[topicIdx].coreMask, PLATFORM_CORE_MASK);
        return ret;
    }

    const Identity *identity = subscriber->identity;
    if (identity != NULL) {
        HILOG_DEBUG(HILOG_MODULE_SAMGR, "serviceId=%u, featureId=%u, queueId=%u",
            identity->serviceId, identity->featureId, identity->queueId);
    }
    HILOG_DEBUG(HILOG_MODULE_SAMGR, "args=0x%p, notify=0x%p",
        subscriber->args, subscriber->Notify);

    SubscriberTable *subscriberTable = &(topicSubscriberInfo->subscriberTable);
    MUTEX_Lock(topicSubscriberInfo->mutex);
    do {
        /* 首先查看是否已经存在相同的结构 */
        int16 subIdx = GetSubscriberIndex(subscriberTable->subscribers, subscriberTable->count, subscriber);
        if (subIdx == INVALID_INDEX) {
            HILOG_ERROR(HILOG_MODULE_SAMGR, "subscriber not in array!");
            break;
        }
        HILOG_DEBUG(HILOG_MODULE_SAMGR, "topicIdx=%u, subIdx=%u", topicIdx, subIdx);
        if (GetBitmap(topicTable->bitmap, topicIdx, subIdx)) {
            /* 仅清零当前topic对应的subscribe，在subscribe数组中，subscriber数据仍然存在 */
            ClearBitmap(topicTable->bitmap, topicIdx, subIdx);
        } else {
            HILOG_WARN(HILOG_MODULE_SAMGR, "current not subscribe.");
        }
        ret = OHOS_SUCCESS;
    } while (FALSE);
    MUTEX_Unlock(topicSubscriberInfo->mutex);
    return ret;
}

int32 GetTopicBitmapsImpl(const TopicSubscriberInfo *topicSubscriberInfo, Topic topic, uint32 *bitmaps,
    int16 size)
{
    HILOG_DEBUG(HILOG_MODULE_SAMGR, "start! topicSubscriberInfo=0x%p", topicSubscriberInfo);
    int32 ret = -1;
    const TopicToCoreTable *topicTable = &(topicSubscriberInfo->topicTable);
    int16 topicIdx = GetTopicIndex(topicTable->topics, topicTable->count, topic);
    if (topicIdx == INVALID_INDEX) {
        HILOG_ERROR(HILOG_MODULE_SAMGR, "topic not exist! topic=%u", topic);
        return ret;
    }
    HILOG_DEBUG(HILOG_MODULE_SAMGR, "topic=%u, topicIdx=%u", topic, topicIdx);

    if (size < BITMAP_ARRAY_ENTRY_SIZE) {
        HILOG_ERROR(HILOG_MODULE_SAMGR, "bitmaps size not enough! size=%u, SIZE=%u",
            size, BITMAP_ARRAY_ENTRY_SIZE);
        return ret;
    }

    int16 offset = topicIdx * BITMAP_ARRAY_ENTRY_SIZE;
    uint16 bufSize = size * sizeof(int32);
    uint16 len = BITMAP_ARRAY_ENTRY_SIZE * sizeof(int32);
    MUTEX_Lock(topicSubscriberInfo->mutex);
    ret = memcpy_s(bitmaps, bufSize, &(topicTable->bitmap[offset]), len);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_SAMGR, "memcpy_s fail, ret = %d\r\n", ret);
    }
    MUTEX_Unlock(topicSubscriberInfo->mutex);
    ret = BITMAP_ARRAY_ENTRY_SIZE;
    return ret;
}

/* 通信服务发过来的消息， 处理过程中不需要释放。 如果需要转发时，需要复制消息(零拷贝) */
int32 HandleTopicMsgImpl(const TopicSubscriberInfo *topicSubscriberInfo, Request *req)
{
    HILOG_DEBUG(HILOG_MODULE_SAMGR, "start! topicSubscriberInfo=0x%p", topicSubscriberInfo);
    int32 ret = OHOS_FAILURE;
    if (req == NULL) {
        HILOG_ERROR(HILOG_MODULE_SAMGR, "invalid param!");
        return ret;
    }

    /* topic */
    const TopicToCoreTable *topicTable = &(topicSubscriberInfo->topicTable);
    int16 topicIdx = GetTopicIndex(topicTable->topics, topicTable->count, req->msgId);
    if (topicIdx == INVALID_INDEX) {
        HILOG_ERROR(HILOG_MODULE_SAMGR, "topic not exist! msgId=%u", req->msgId);
        return ret;
    }
    HILOG_DEBUG(HILOG_MODULE_SAMGR, "topic=%u, topicIdx=%u", req->msgId, topicIdx);

    SendInnerMessage(topicSubscriberInfo, topicIdx, req);
    return OHOS_SUCCESS;
}

/* 其他服务发直接发布消息， 处理消息后需要释放信息。核内零拷贝、核间需要复制消息 */
int32 PublishTopicMsgImpl(const TopicSubscriberInfo *topicSubscriberInfo, Request *req)
{
    HILOG_DEBUG(HILOG_MODULE_SAMGR, "start! topicSubscriberInfo=0x%p", topicSubscriberInfo);
    int32 ret = OHOS_FAILURE;
    if (req == NULL) {
        HILOG_ERROR(HILOG_MODULE_SAMGR, "invalid param!");
        return ret;
    }
    const TopicToCoreTable *topicTable = &(topicSubscriberInfo->topicTable);
    int16 topicIdx = GetTopicIndex(topicTable->topics, topicTable->count, req->msgId);
    if (topicIdx == INVALID_INDEX) {
        HILOG_ERROR(HILOG_MODULE_SAMGR, "topic not exist! msgId=%u", req->msgId);
        if (req->data != NULL) {
            free(req->data);
            req->data = NULL;
        }
        return ret;
    }
    HILOG_DEBUG(HILOG_MODULE_SAMGR, "topic=%u, topicIdx=%u", req->msgId, topicIdx);

    /* 消息将在SendInnerMessage或SendOuterMessage中释放，或发生出去 */
    uint16 coreMask = topicTable->topics[topicIdx].coreMask;
    BOOL isInnerCore = IsInnerCore(coreMask);
    /* topic需要核内通信 */
    if (isInnerCore) {
        SendInnerMessage(topicSubscriberInfo, topicIdx, req);
    }
    return OHOS_SUCCESS;
}


static int32 SubscribeTopic(IUnknown *broadcastApi, const Topic topic, const Subscriber* subscriber)
{
    PubSubImplement *broadcast =  GET_OBJECT(broadcastApi, PubSubImplement, iUnknown);
    return SubscribeTopicImpl(&(broadcast->feature->topicSubscriberInfo), topic, subscriber);
}

static int32 UnsubscribeTopic(IUnknown *broadcastApi, const Topic topic, const Subscriber* subscriber)
{
    PubSubImplement *broadcast =  GET_OBJECT(broadcastApi, PubSubImplement, iUnknown);
    return UnsubscribeTopicsImpl(&(broadcast->feature->topicSubscriberInfo), topic, subscriber);
}

static int32 GetTopicBitmap(IUnknown *broadcastApi, const Topic topic, uint32 *bitmaps, int16 size)
{
    PubSubImplement *broadcast =  GET_OBJECT(broadcastApi, PubSubImplement, iUnknown);
    return GetTopicBitmapsImpl(&(broadcast->feature->topicSubscriberInfo), topic, bitmaps, size);
}

static int32 PublishTopic(IUnknown *broadcastApi, Request *msg)
{
    PubSubImplement *broadcast =  GET_OBJECT(broadcastApi, PubSubImplement, iUnknown);
    return PublishTopicMsgImpl(&(broadcast->feature->topicSubscriberInfo), msg);
}

static int AddTopic(IUnknown *iUnknown, const Topic *topic)
{
    if (iUnknown == NULL || topic == NULL) {
        return EC_INVALID;
    }

    PubSubImplement *broadcast = GET_OBJECT(iUnknown, PubSubImplement, iUnknown);
    if (broadcast->feature == NULL || broadcast->feature->GetRelation == NULL) {
        return EC_FAILURE;
    }

    if (broadcast->feature->GetRelation(broadcast->feature, topic) != NULL) {
        return EC_FAILURE;
    }

    Relation *head = &broadcast->feature->relations;
    Relation *newRelation = (Relation *)SAMGR_Malloc(sizeof(Relation));
    if (newRelation == NULL) {
        return EC_NOMEMORY;
    }
    newRelation->topic = *topic;
    newRelation->callbacks.consumer = NULL;
    UtilsListInit(&newRelation->callbacks.node);

    MUTEX_Lock(broadcast->feature->mutex);
    UtilsListAdd(&head->node, &(newRelation->node));
    MUTEX_Unlock(broadcast->feature->mutex);
    return EC_SUCCESS;
}

static int Subscribe(IUnknown *iUnknown, const Topic *topic, Consumer *consumer)
{
    if (iUnknown == NULL || topic == NULL || consumer == NULL) {
        return EC_INVALID;
    }

    PubSubImplement *broadcast = GET_OBJECT(iUnknown, PubSubImplement, iUnknown);
    if (broadcast->feature == NULL || broadcast->feature->GetRelation == NULL) {
        return EC_FAILURE;
    }

    Relation *relation = broadcast->feature->GetRelation(broadcast->feature, topic);
    if (relation == NULL) {
        return EC_FAILURE;
    }

    MUTEX_Lock(broadcast->feature->mutex);
    ConsumerNode *item = NULL;
    UTILS_DL_LIST_FOR_EACH_ENTRY(item, &relation->callbacks.node, ConsumerNode, node) {
        if (item->consumer->Equal(item->consumer, consumer)) {
            MUTEX_Unlock(broadcast->feature->mutex);
            return EC_ALREADY_SUBSCRIBED;
        }
    }
    MUTEX_Unlock(broadcast->feature->mutex);
    ConsumerNode *consumerNode = (ConsumerNode *)SAMGR_Malloc(sizeof(ConsumerNode));
    if (consumerNode == NULL) {
        return EC_NOMEMORY;
    }

    UtilsListInit(&consumerNode->node);
    consumerNode->consumer = consumer;
    MUTEX_Lock(broadcast->feature->mutex);
    ConsumerNode *head = &relation->callbacks;
    UtilsListAdd(&head->node, &consumerNode->node);
    MUTEX_Unlock(broadcast->feature->mutex);
    return EC_SUCCESS;
}

static Consumer *ModifyConsumer(IUnknown *iUnknown, const Topic *topic, Consumer *oldConsumer, Consumer *newConsumer)
{
    if (iUnknown == NULL || topic == NULL || oldConsumer == NULL || newConsumer == NULL) {
        return NULL;
    }

    PubSubImplement *broadcast = GET_OBJECT(iUnknown, PubSubImplement, iUnknown);
    if (broadcast->feature == NULL || broadcast->feature->GetRelation == NULL) {
        return NULL;
    }

    Relation *relation = broadcast->feature->GetRelation(broadcast->feature, topic);
    if (relation == NULL) {
        return NULL;
    }

    MUTEX_Lock(broadcast->feature->mutex);
    ConsumerNode *item = NULL;
    UTILS_DL_LIST_FOR_EACH_ENTRY(item, &relation->callbacks.node, ConsumerNode, node) {
        if (item->consumer->Equal(item->consumer, oldConsumer)) {
            Consumer *older = item->consumer;
            item->consumer = newConsumer;
            MUTEX_Unlock(broadcast->feature->mutex);
            return older;
        }
    }
    MUTEX_Unlock(broadcast->feature->mutex);
    return NULL;
}

static Consumer *Unsubscribe(IUnknown *iUnknown, const Topic *topic, const Consumer *consumer)
{
    if (iUnknown == NULL || topic == NULL || consumer == NULL) {
        return NULL;
    }

    PubSubImplement *broadcast = GET_OBJECT(iUnknown, PubSubImplement, iUnknown);
    if (broadcast->feature == NULL || broadcast->feature->GetRelation == NULL) {
        return NULL;
    }

    Relation *relation = broadcast->feature->GetRelation(broadcast->feature, topic);
    if (relation == NULL) {
        return NULL;
    }
    MUTEX_Lock(broadcast->feature->mutex);
    ConsumerNode *item = NULL;
    UTILS_DL_LIST_FOR_EACH_ENTRY(item, &relation->callbacks.node, ConsumerNode, node) {
        if (item->consumer->Equal(item->consumer, consumer)) {
            UtilsListDelete(&item->node);
            break;
        }
    }
    MUTEX_Unlock(broadcast->feature->mutex);
    if (item == &relation->callbacks || item == NULL) {
        return NULL;
    }
    Consumer *oldConsumer = item->consumer;
    SAMGR_Free(item);
    return oldConsumer;
}

static BOOL Publish(IUnknown *iUnknown, const Topic *topic, uint8 *data, int16 len)
{
    PubSubImplement *broadcast = GET_OBJECT(iUnknown, PubSubImplement, iUnknown);
    PubSubFeature *feature = broadcast->feature;
    if (feature == NULL) {
        return FALSE;
    }

    Request request = {MSG_PUBLISH, 0, NULL, *(uint32 *)topic};
    if (data != NULL && len > 0) {
        request.data = (uint8 *)SAMGR_Malloc(len);
        if (request.data == NULL) {
            return FALSE;
        }
        request.len = len;
        // There is no problem, the request.data length is equal the input data length.
        (void)memcpy_s(request.data, request.len, data, len);
    }

    if (!ImmediatelyPublish(feature, topic, &request)) {
        (void)SAMGR_Free(request.data);
        request.data = NULL;
        request.len = 0;
        return FALSE;
    }
    return TRUE;
}

static BOOL ImmediatelyPublish(PubSubFeature *feature, const Topic *topic, const Request *request)
{
    if (feature->GetRelation == NULL) {
        return FALSE;
    }

    Relation *relation = feature->GetRelation(feature, topic);
    if (relation == NULL) {
        return FALSE;
    }

    if (UtilsListEmpty(&relation->callbacks.node)) {
        return FALSE;
    }

    BOOL needAync = FALSE;
    ConsumerNode *item = NULL;
    uint32 *token = NULL;
    MUTEX_Lock(feature->mutex);
    UTILS_DL_LIST_FOR_EACH_ENTRY(item, &relation->callbacks.node, ConsumerNode, node) {
        if (item->consumer->identity == NULL) {
            needAync = TRUE;
            continue;
        }

        Response response = {item->consumer, 0};
        int ret = SAMGR_SendSharedDirectRequest(item->consumer->identity, request, &response, &token, DefaultHandle);
        if (ret != EC_SUCCESS) {
            needAync = FALSE;
            break;
        }
    }
    if (needAync) {
        token = SAMGR_SendSharedRequest(&feature->identity, request, token, NULL);
    }
    MUTEX_Unlock(feature->mutex);
    return (token != NULL);
}

static void DefaultHandle(const Request *request, const Response *response)
{
    Consumer *consumer = (Consumer *)response->data;
    if (consumer == NULL || consumer->Notify == NULL || g_pubSubImplement.feature == NULL) {
        return;
    }

    // wait ImmediatelyPublish finished.
    MUTEX_Lock(g_pubSubImplement.feature->mutex);
    MUTEX_Unlock(g_pubSubImplement.feature->mutex);
    Topic topic = request->msgValue;
    consumer->Notify(consumer, &topic, request);
}
