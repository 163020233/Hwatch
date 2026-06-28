/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 *
 * Description: SLE Service DIS server struct and func.
 */

#ifndef SSF_DIS_SERVER_H
#define SSF_DIS_SERVER_H

#include "stdint.h"
#include "errcode.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief   Manufacture information of DIS.
 * @par This function allows users to configure the manufacturer information in the
 * device information management service.
 * @attention NULL
 * @param   [in] manufacture_info Manufacture info.
 * @param   [in] len Length of manufacture info. Maximum is 21.
 * @retval error code, indicating whether the operation was successful.
 * @else
 * @brief   DIS服务端设置厂商信息。
 * @par 该函数用于配置设备信息中的厂商信息。
 * @attention 无
 * @param   [in] manufacture_info 厂商信息。
 * @param   [in] len 厂商信息长度。最大为21。
 * @retval 执行结果错误码，表示操作是否成功。
 */
errcode_t ssf_dis_set_manufacture_info(uint8_t *manufacture_info, uint8_t len);

/**
 * @if Eng
 * @brief   Device model information of DIS.
 * @par This function allows users to configure the device model information in the
 * device information management service.
 * @attention NULL
 * @param   [in] device_model_info Device model info.
 * @param   [in] len Length of device model info. Maximum is 21.
 * @retval error code, indicating whether the operation was successful.
 * @else
 * @brief  dis服务端设置设备型号信息。
 * @par 该函数用于配置设备信息中的设备型号信息。
 * @attention 无
 * @param [in] device_model_info 设备型号信息。
 * @param [in] len 设备型号信息长度。最大为21。
 * @retval 执行结果错误码，表示操作是否成功。
 */
errcode_t ssf_dis_set_device_model_info(uint8_t *device_model_info, uint8_t len);

/**
 * @if Eng
 * @brief   Device local name of DIS.
 * @par This function allows users to configure the device local name in the
 * device information management service.
 * @attention NULL
 * @param   [in] device_name Device local name.
 * @param   [in] len Length of device local name. Maximum is 20.
 * @retval error code, indicating whether the operation was successful.
 * @else
 * @brief  dis服务端设置设备本地名称。
 * @par 该函数用于配置设备信息中的设备本地名称。
 * @attention 无
 * @param [in] device_name 设备本地名称。
 * @param [in] len 设备本地名称长度。最大为20。
 * @retval 执行结果错误码，表示操作是否成功。
 */
errcode_t ssf_dis_set_device_name(uint8_t *device_name, uint8_t len);

/**
 * @if Eng
 * @brief   Device appearance of DIS.
 * @par This function allows users to configure the device appearance in the
 * device information management service.
 * @attention NULL
 * @param   [in] appearance_info Device appearance. Length of device appearance fixed at 3 bytes. 0x000101-0x0001FF is vendor-defined.
 * @retval error code, indicating whether the operation was successful.
 * @else
 * @brief  dis服务端设置设备外观信息。
 * @par 该函数用于配置设备信息中的设备外观信息。
 * @attention 无
 * @param [in] appearance_info 设备外观信息, 固定3字节。0x000101-0x0001FF为厂商自定义。
 * @retval 执行结果错误码，表示操作是否成功。
 */
errcode_t ssf_dis_set_device_appearance(uint32_t appearance_info);

#ifdef __cplusplus
}
#endif
#endif /* SSF_DIS_SERVER_H */