/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: HeartModel.cpp
 * Create: 2021-09-22
 */

#include "heartrate/HeartModel.h"
#include "wearable_log.h"
#include "graphic_service.h"
#include "main/MainViewSample.h"
#include "heartrate/HeartRateView.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 佩戴检测结果 */
enum {
    SENSOR_WEAR_ON = 0,
    SENSOR_WEAR_OFF,
    SENSOR_WEAR_UNKNOW,
    SENSOR_WEAR_MAX
};

using namespace OHOS;
#define LEN 32

static void Notify(void *args, const Topic topic, Request* request);

Subscriber heartSubscirber = {
    .identity = nullptr,
    .args = nullptr,
    .Notify = Notify,
};

void HeartDataSubscribeTopic(Topic topic)
{
#if !defined(_WIN32)
    BroadcastApi *broadcastApi = nullptr;
    IUnknown *api = SAMGR_GetInstance()->GetFeatureApi(BROADCAST_SERVICE_NAME, BROADCAST_FEATURE_NAME);
    if (api == nullptr) {
        return;
    }
    int result = api->QueryInterface(api, DEFAULT_VERSION, (void **)&broadcastApi);
    if (result != 0 || broadcastApi == nullptr) {
        return;
    }
    int ret = broadcastApi->SubscribeTopic((IUnknown *)broadcastApi, topic, &heartSubscirber);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "HeartSubscribeTopic ret = %d, topic = %u", ret, topic);
    broadcastApi->Release((IUnknown *)broadcastApi);
#endif
}

void HearDatatUnsubscribeTopic(Topic topic)
{
#if !defined(_WIN32)
    BroadcastApi *broadcastApi = nullptr;
    IUnknown *api = SAMGR_GetInstance()->GetFeatureApi(BROADCAST_SERVICE_NAME, BROADCAST_FEATURE_NAME);
    if (api == nullptr) {
        return;
    }
    int result = api->QueryInterface(api, DEFAULT_VERSION, (void **)&broadcastApi);
    if (result != 0 || broadcastApi == nullptr) {
        return;
    }
    broadcastApi->UnsubscribeTopic((IUnknown *)broadcastApi, topic, &heartSubscirber);
    broadcastApi->Release((IUnknown *)broadcastApi);
#endif
}

static void Notify(void *args, const Topic topic, Request* request)
{
    UNUSED(args);
    UNUSED(topic);
    if (request == nullptr) {
        return;
    }
    if (request->msgId == TOPIC_EVENT_HEARTRATE_UPDATA) {
        HeartDatas *heartInfo = (HeartDatas*)malloc(sizeof(HeartDatas));
        if (heartInfo == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[Notify] malloc fail");
        }

        int rets = memcpy_s(heartInfo, sizeof(HeartDatas), (HeartInfo*)request->data, request->len);
        if (rets != EOK) {
            free(heartInfo);
            heartInfo = nullptr;
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "memcpy_s heartRateData fail！ ret = %d", rets);
            return;
        }

        OHOS::HeartModel::GetInstance()->HeartRatePros(heartInfo);

        free(heartInfo);
        heartInfo = nullptr;
    }
    if (request->msgId == TOPIC_EVENT_NEXTDAY_UPDATE) {
        OHOS::HeartModel::GetInstance()->ClearHeartData();
    }
    if (request->msgId == TOPIC_EVENT_UTCTIME_UPDATE) {
        OHOS::HeartModel::GetInstance()->ClearHeartData();
    }
    if (request->msgId == TOPIC_EVENT_WEARSTATE_UPDATE) {
        uint32 wearStatus = *((uint32*)request->data);
        if (wearStatus == SENSOR_WEAR_OFF) {
            OHOS::HeartModel::GetInstance()->SetWearStatus(false);
            OHOS::HeartModel::GetInstance()->SetHeartRatePerSec(0);
            if (OHOS::MainClockView::GetInstance() != nullptr) {
                OHOS::MainClockView::GetInstance()->RefreshHeartRate(0);
            }
        } else {
            OHOS::HeartModel::GetInstance()->SetWearStatus(true);
        }
    }
}

#ifdef __cplusplus
}
#endif

namespace OHOS {
void HeartModel::ClearHeartData(void)
{
    pretime = 0;
    entryTimes = 0;
    aveCount = 0;
    preMaxValue = 0;
    preMaxX = NUM_1;
    preMinValue = HEART_RATE_LIMIT_UPPER;
    preMinX = NUM_1;
    countMax = 0;
    countMin = 0;
    healthDatas.heartRateMax.x = (uint16_t)0XFFFF;
    healthDatas.heartRateMax.y = 0;
    healthDatas.heartRateMin.x = (uint16_t)0XFFFF;
    healthDatas.heartRateMin.y = (uint16_t)0XFFFF;
    for (int i = 0; i < HEART_AVE_NUM; i++) {
        healthDatas.heartRateAvg[i].x = (uint16_t)0XFFFF;
        healthDatas.heartRateAvg[i].y = (uint16_t)0XFFFF;
    }
    if (HeartRateView::GetInstance() != nullptr) {
        HeartRateView::GetInstance()->ClearElectrocardiogram();
    }
    SetAveHeartRateCount(0);
    GraphicService::GetInstance()->PostGraphicEvent(std::bind(&HeartModel::RefreshHeartRateScreen, HeartModel::GetInstance(),
        healthDatas.heartRatePerSec, healthDatas.heartRateRest, healthDatas.heartRateMax.y, healthDatas.heartRateMin.y));
}

HeartModel::HeartModel()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "HeartModel::HeartModel()");
}

HeartModel::~HeartModel()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "HeartModel::~HeartModel()");
    HearDatatUnsubscribeTopic(TOPIC_EVENT_HEARTRATE_UPDATA);
    HearDatatUnsubscribeTopic(TOPIC_EVENT_NEXTDAY_UPDATE);
    HearDatatUnsubscribeTopic(TOPIC_EVENT_UTCTIME_UPDATE);
    HearDatatUnsubscribeTopic(TOPIC_EVENT_WEARSTATE_UPDATE);
}

HeartModel *HeartModel::GetInstance(void)
{
    static HeartModel heartModel;
    return &heartModel;
}

void HeartModel::Init(void)
{
    HeartDataSubscribeTopic(TOPIC_EVENT_HEARTRATE_UPDATA);
    HeartDataSubscribeTopic(TOPIC_EVENT_NEXTDAY_UPDATE);
    HeartDataSubscribeTopic(TOPIC_EVENT_UTCTIME_UPDATE);
    HeartDataSubscribeTopic(TOPIC_EVENT_WEARSTATE_UPDATE);
}

Point* HeartModel::GetAveHeartRate(void)
{
    return healthDatas.heartRateAvg;
}

void HeartModel::HandleMaxHeart(uint16 coord, uint16 value)
{
    healthDatas.heartRateMax.x = coord;
    healthDatas.heartRateMax.y = value;
    // 插入最大心率
    if (preMaxValue != healthDatas.heartRateMax.y && aveCount < HEART_AVE_NUM) {
        if (preMaxX == healthDatas.heartRateMax.x && countMax < HEART_AVE_NUM) {
            healthDatas.heartRateAvg[countMax].y = healthDatas.heartRateMax.y;
        } else if (aveCount > NUM_2 && (healthDatas.heartRateMax.x == healthDatas.heartRateAvg[aveCount - 1].x)) {
            healthDatas.heartRateAvg[aveCount - 1].y = healthDatas.heartRateMax.y;
        } else {
            healthDatas.heartRateAvg[aveCount] = healthDatas.heartRateMax;
            countMax = aveCount;
            aveCount++;
        }
        if (healthDatas.heartRateMax.x != 0) {
            if (healthDatas.heartRateAvg[0].x == 0 && healthDatas.heartRateAvg[1].x == 0) {
                for (int i = 1; i < (aveCount - 1); i++) {
                    healthDatas.heartRateAvg[i] = healthDatas.heartRateAvg[i + 1];
                }
                aveCount--;
                countMax--;
            }
        }
    }
    preMaxValue = healthDatas.heartRateMax.y;
    preMaxX = healthDatas.heartRateMax.x;
}

void HeartModel::HandleMinHeart(uint16 coord, uint16 value)
{
    // 最小心率
    healthDatas.heartRateMin.x = coord;
    healthDatas.heartRateMin.y = value;
    // 插入最小心率
    if (preMinValue != healthDatas.heartRateMin.y && aveCount < HEART_AVE_NUM) {
        if (preMinX == healthDatas.heartRateMin.x && countMin < HEART_AVE_NUM) {
            healthDatas.heartRateAvg[countMin].y = healthDatas.heartRateMin.y;
        } else if (aveCount > NUM_2 && (healthDatas.heartRateMin.x == healthDatas.heartRateAvg[aveCount - 1].x)) {
            healthDatas.heartRateAvg[aveCount - 1].y = healthDatas.heartRateMin.y;
        } else {
            healthDatas.heartRateAvg[aveCount] = healthDatas.heartRateMin;
            countMin = aveCount;
            aveCount++;
        }
        if (healthDatas.heartRateMin.x != 0) {
            if (healthDatas.heartRateAvg[0].x == 0 && healthDatas.heartRateAvg[1].x == 0) {
                for (int i = 0; i < (aveCount - 1); i++) {
                    healthDatas.heartRateAvg[i] = healthDatas.heartRateAvg[i + 1];
                }
                aveCount--;
                countMin--;
            }
        }
    }
    preMinValue = healthDatas.heartRateMin.y;
    preMinX = healthDatas.heartRateMin.x;
}

void  HeartModel::HeartRatePros(HeartDatas *heartInfo)
{
    uint16_t perSecHeartRate = 0;
    uint16_t restHeartRate = 0;

    // 每秒心率
    perSecHeartRate = heartInfo->perSec;
    if (perSecHeartRate > HEART_RATE_LIMIT_LOWER && perSecHeartRate <= HEART_RATE_LIMIT_UPPER) {
        healthDatas.heartRatePerSec = perSecHeartRate;
    }

    // 静息心率
    restHeartRate = heartInfo->rest;
    if (restHeartRate > HEART_RATE_LIMIT_LOWER && restHeartRate <= HEART_RATE_LIMIT_UPPER) {
        healthDatas.heartRateRest = restHeartRate;
    }
    HandleMinHeart(heartInfo->minX, heartInfo->minValue);
    HandleMaxHeart(heartInfo->maxX, heartInfo->maxValue);
    // 平均心率
    uint16_t count = aveCount + heartInfo->index;
    if (aveCount < HEART_AVE_NUM) {
        for (int i = aveCount; i < count; i++) {
            if (healthDatas.heartRateMin.x != heartInfo->value[i - aveCount].x && healthDatas.heartRateMax.x != heartInfo->value[i - aveCount].x) {
                healthDatas.heartRateAvg[aveCount].x = heartInfo->value[i - aveCount].x;
                healthDatas.heartRateAvg[aveCount].y = heartInfo->value[i - aveCount].y;
                aveCount++;
            }
        }
    }

    for (int i = 0; i < aveCount - 1; i++) {
        if (healthDatas.heartRateAvg[i].x == healthDatas.heartRateAvg[i + 1].x) {
            if (healthDatas.heartRateAvg[i].y != healthDatas.heartRateMin.y && healthDatas.heartRateAvg[i].y != healthDatas.heartRateMax.y) {
                for (int j = i; j < aveCount - 1; j++) {
                    healthDatas.heartRateAvg[j] = healthDatas.heartRateAvg[j + 1];
                }
                aveCount--;
            } else if (healthDatas.heartRateAvg[i + 1].y != healthDatas.heartRateMin.y && healthDatas.heartRateAvg[i + 1].y != healthDatas.heartRateMax.y) {
                for (int j = i + 1; j < aveCount - 1; j++) {
                    healthDatas.heartRateAvg[j] = healthDatas.heartRateAvg[j + 1];
                }
                aveCount--;
            }
        }
    }
    GraphicService::GetInstance()->PostGraphicEvent(std::bind(&HeartModel::RefreshHeartRateScreen, HeartModel::GetInstance(),
        healthDatas.heartRatePerSec, healthDatas.heartRateRest, healthDatas.heartRateMax.y, healthDatas.heartRateMin.y));
}

uint16_t HeartModel::GetAveHeartRateCount(void)
{
    return aveCount;
}

uint16_t HeartModel::GetHeartRateMax(void)
{
    return healthDatas.heartRateMax.y;
}

uint16_t HeartModel::GetHeartRateMin(void)
{
    return healthDatas.heartRateMin.y;
}

void HeartModel::SetHeartRatePerSec(uint32 sec)
{
    healthDatas.heartRatePerSec = sec;
    if (HeartRateView::GetInstance() == nullptr) {
        return;
    }
    HeartRateView::GetInstance()->RefreshHeartRatePerSec(healthDatas.heartRatePerSec);
}

void HeartModel::SetAveHeartRateCount(uint16_t value)
{
    aveCount = value;
}

void HeartModel::SetWearStatus(bool status)
{
    wearStatus = status;
}

bool HeartModel::GetWearStatus(void)
{
    return wearStatus;
}

void HeartModel::RefreshHeartRateScreen(uint16_t perSec, uint16_t rest, uint16_t maxRate, uint16_t minRate)
{
    UNUSED(rest);
    Point *rateData;
    int len = GetAveHeartRateCount();
    rateData = GetAveHeartRate();

    uint16_t currentViewId = NativeAbility::GetInstance().GetCurSliceId();
    if (currentViewId == VIEW_MAIN_SAMPLE && MainClockView::GetInstance() != nullptr) {
        MainClockView::GetInstance()->RefreshHeartRate(perSec);
    }

    if (MainViewSample::GetInstance() != nullptr && HeartRateView::GetInstance() != nullptr) {
        if (MainViewSample::GetInstance()->GetHorCurrentCard() == HEART_RATE) {
            HeartRateView::GetInstance()->SetHeartRateData(rateData, sizeof(Point) * len);
            HeartRateView::GetInstance()->RefreshElectrocardiogram();
            HeartRateView::GetInstance()->RefreshHeartRatePerSec(perSec);
            HeartRateView::GetInstance()->RefreshHeartRateMax(maxRate);
            HeartRateView::GetInstance()->RefreshHeartRateMin(minRate);
        }
    }
}
}

