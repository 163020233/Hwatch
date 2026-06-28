/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: DateModel
 * Create: 2025-03-23
 */

#include "bts_br_gap.h"
#include "bts_spp.h"
#include "ux_api_bt.h"
#include "msg_center_customer.h"
#include "msg_center_device.h"
#include "ohos_timer.h"
#include "phone_msg_notification.h"
#include "date/DatePresenter.h"
#include "date/DateModel.h"

namespace OHOS {
#define BASE 10
static constexpr int16 INDEX_0 = 0;
static constexpr int16 INDEX_1 = 1;
static constexpr int16 INDEX_2 = 2;
static constexpr int16 INDEX_3 = 3;
static constexpr int16 INDEX_4 = 4;
static constexpr int16 INDEX_5 = 5;
static constexpr int16 DATE_SUM = 20;
static const char *DATE_FILE = DATE_PATH "/calendar.json";

DateModel::DateModel() {}

DateModel::~DateModel() {}

DateModel &DateModel::GetInstance()
{
    static DateModel instance;
    return instance;
}

bool DateModel::InitDateVec(std::string dateStr)
{
    int index = 0;
    listDate_.clear();
    if (GetConnectStatus() == 0) {
        listDate_.push_back({"无日程", "请连接手机，同步日程信息", "0", "0", true});
    } else {
        for (const auto &msg : keyValues_) {
            if (msg.first != dateStr) {
                continue;
            }
            for (const auto &item : msg.second) {
                listDate_.push_back(
                    {item.hour.c_str(), item.eventTitle.c_str(), item.address.c_str(), item.index.c_str(), false});
            }
        }

        if (listDate_.size() == 0) {
            listDate_.push_back({"无日程", "请前往手机日历创建日程", "0", "0", true});
        }
    }
    return true;
}

uint8_t DateModel::GetConnectStatus()
{
    uint8_t conn_status = 0;
#ifdef _WIN32
    conn_status = 1;
#else
    if (msg_center_get_trans_channel() == DIAG_FRAME_FID_BT_SPP) {
        conn_status = spp_is_connected(DIAG_SPP_DEFAULT_PORT) == true ? 1 : 0;
    } else if (msg_center_get_trans_channel() == DIAG_FRAME_FID_BLE_GATT) {
        conn_status = msg_center_get_ble_conn_state();
    }
#endif
    return conn_status;
}

bool DateModel::InitDate()
{
    keyValues_.clear();
    std::ifstream file(DATE_FILE);
    if (!file.is_open()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "cannot open file, %s\r\n", DATE_FILE);
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    if (content.empty()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "File %s is empty\r\n", DATE_FILE);
        return false;
    }

    cJSON *root = cJSON_Parse(content.c_str());
    if (root == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "root is NULL.\r\n");
        return false;
    }

    cJSON *dateSum = cJSON_GetObjectItemCaseSensitive(root, "dateSum");
    if (!cJSON_IsObject(dateSum)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Error observe:");
        cJSON_Delete(root);
        return false;
    }

    cJSON *item = cJSON_GetObjectItemCaseSensitive(dateSum, "count");
    if (item == nullptr || item->valuestring == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Error count");
        cJSON_Delete(root);
        return false;
    }
    int sum = static_cast<int>(strtol(item->valuestring, nullptr, BASE));
    for (int i = 0; i < sum; i++) {
        cJSON *date = cJSON_GetObjectItemCaseSensitive(dateSum, std::to_string(i).c_str());
        if (!cJSON_IsObject(date)) {
            cJSON_Delete(root);
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Error date");
            return false;
        }

        std::string time = "测试time";
        cJSON *timeItem = cJSON_GetObjectItemCaseSensitive(date, "time");
        if (!timeItem || !cJSON_IsString(timeItem)) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Error time: Field is missing or not a string");
            continue;
        } else {
            time = timeItem->valuestring;
        }

        std::string hour = "测试hour";
        cJSON *hourItem = cJSON_GetObjectItemCaseSensitive(date, "hour");
        if (!hourItem || !cJSON_IsString(hourItem)) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Error hour: Field is missing or not a string");
            continue;
        } else {
            hour = hourItem->valuestring;
        }

        std::string eventTitle = "测试eventTitle";
        cJSON *eventTitleItem = cJSON_GetObjectItemCaseSensitive(date, "eventTitle");
        if (!eventTitleItem || !cJSON_IsString(eventTitleItem)) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Error eventTitle: Field is missing or not a string");
            continue;
        } else {
            eventTitle = eventTitleItem->valuestring;
        }

        std::string address = "no location";
        cJSON *locationItem = cJSON_GetObjectItemCaseSensitive(date, "location");
        if (!locationItem || !cJSON_IsString(locationItem)) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Error location: Field is missing or not a string");
            continue;
        } else {
            address = locationItem->valuestring;
        }

        std::string startTime = "startTime";
        cJSON *startTimeItem = cJSON_GetObjectItemCaseSensitive(date, "startTime");
        if (!startTimeItem || !cJSON_IsString(startTimeItem)) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Error startTime: Field is missing or not a string");
            continue;
        } else {
            startTime = startTimeItem->valuestring;
        }

        std::string index = std::to_string(i);
        DateMsg item = {hour, eventTitle, address, index};
        if (CmpStartTime(startTime)) {
            if (keyValues_[time].size() < DATE_SUM) {
                keyValues_[time].push_back(item);
            } else {
                WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Date Sum: %d Is Over 20", keyValues_[time].size());
            }
        } else {
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "This calendar item is out of date!\r\n");
        }
    }
    cJSON_Delete(root);
    return true;
}

bool DateModel::CmpStartTime(std::string startTime)
{
    std::tm tm = {};
    std::istringstream ss(startTime);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M");
    if (ss.fail()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Error: Failed to parse time string");
        return false;
    }
    time_t inputTime = std::mktime(&tm);
    struct timeval now;
    gettimeofday(&now, nullptr);
    return inputTime >= now.tv_sec;
}

void DateModel::PostMsg(std::string content)
{
    GraphicService::GetInstance()->PostGraphicEvent(
        std::function<void()>([content]() { PhoneMsgView::GetInstance()->ShowDateMsg("日历", "日历通知", content); }));
}
}
