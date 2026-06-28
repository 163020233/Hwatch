/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

/**
 * @addtogroup PanSensor
 * @{
 *
 * @brief Provides standard open APIs for you to use common capabilities of sensors.
 *
 * For example, you can call these APIs to obtain sensor information,
 * subscribe to or unsubscribe from sensor data, enable or disable a sensor,
 * and set the sensor data reporting mode.
 *
 * @since 5
 */

/**
 * @file sensor_agent_type_ext.h
 *
 * @brief Defines the ext data used by the sensor agent to manage sensors.
 *
 * @since 5
 */

#ifndef SENSOR_AGENT_TYPE_EXT_H
#define SENSOR_AGENT_TYPE_EXT_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @brief 加速度传感器的数据结构
 */
typedef struct AccelData {
    /** 加速度X轴 */
    int16_t axisX;
    /** 加速度Y轴 */
    int16_t axisY;
    /** 加速度Z轴 */
    int16_t axisZ;
} AccelData;

/**
 * @brief 角速度传感器的数据结构
 */
typedef struct GyroData {
    /** 加速度X轴 */
    int16_t axisX;
    /** 加速度Y轴 */
    int16_t axisY;
    /** 加速度Z轴 */
    int16_t axisZ;
} GyroData;

/**
 * @brief 气压传感器的数据结构
 */
typedef struct PressureData {
    /** 气压计气压值 */
    uint32_t pressure;
    /** 气压计温度值 */
    uint32_t temperature;
    /** 气压计数据有效性 */
    uint32_t dataValid;
} PressureData;

/**
 * @brief Defines the sensor data.
 *
 * @since 5
 */
typedef struct SensorData {
    int64_t timestamp;     /**< Time when sensor data was reported */
    uint32_t batchCnt;     /**< Sensor batch cnt */
    uint8_t *data;         /**< Sensor data */
    uint32_t dataLen;      /**< Sensor data length */
} SensorData;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* SENSOR_AGENT_TYPE_EXT_H */
/** @} */
