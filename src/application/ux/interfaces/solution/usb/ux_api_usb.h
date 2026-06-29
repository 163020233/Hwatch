/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: Provides ux usb solution \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-06-16， Create file. \n
 */
#ifndef UX_API_USB_H
#define UX_API_USB_H

#include <stdint.h>
#include <errcode.h>
#ifndef DEMO_WATCH
#include "feature_config.h"
#endif
#if defined(CONFIG_SUPPORT_USB_DEVICE)
#include "usb_drv.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @brief USB HID事件类型枚举
 */
typedef enum {
    UX_API_USB_HID_DEVICE_SET_VOL_INC,     /**< device设置音量+事件 */
    UX_API_USB_HID_DEVICE_SET_VOL_DEC,     /**< device设置音量-事件 */
    UX_API_USB_HID_DEVICE_SET_MUTE,        /**< device设置静音/解静音事件 */
    UX_API_USB_HID_DEVICE_SET_PLAY,        /**< device设置开始播放事件 */
    UX_API_USB_HID_DEVICE_SET_PAUSE,       /**< device设置停止播放事件 */
    UX_API_USB_HID_DEVICE_SET_PLAY_NEXT,   /**< Device下一曲 */
    UX_API_USB_HID_DEVICE_SET_PLAY_PRE,    /**< Device上一曲 */
    UX_API_USB_HID_DEVICE_SET_PLAY_CHANGE, /**< 播放切换 */
    UX_API_USB_HID_DEVICE_SET_HOOK_SWITCH, /**< Device通话接通或挂断 */
    UX_API_USB_HID_DEVICE_SET_PHONE_MUTE,  /**< Device通话静音 */
    UX_API_USB_HID_DEVICE_SET_CUSTOMER,    /**< device通过私有协议设置事件 */

    UX_API_USB_HID_HOST_SET_VOL_INC = 0x10,  /**< host设置音量+事件 */
    UX_API_USB_HID_HOST_SET_VOL_DEC,         /**< host设置音量-事件 */
    UX_API_USB_HID_HOST_SET_MUTE,            /**< host设置静音/解静音事件 */
    UX_API_USB_HID_HOST_SET_PLAY,            /**< host设置开始播放事件 */
    UX_API_USB_HID_HOST_SET_PAUSE,           /**< host设置停止播放事件 */

    UX_API_USB_HID_EVENT_MAX,                /**< 无效HID事件 */
} ux_api_usb_hid_event_t;

typedef enum {
    UX_API_USB_UAC_CHANNELS_2 = 2,        /**< UAC 2声道 */
    UX_API_USB_UAC_CHANNELS_4 = 4,        /**< UAC 4声道 */
    UX_API_USB_UAC_CHANNELS_8 = 8,        /**< UAC 8声道 */
    
    UX_API_USB_UAC_CHANNELS_MAX,           /**< 无效通道数 */
} ux_api_usb_uac_channels_t;

/**
 * @brief  释放hid消息申请内存回调函数类型
 * @param [in] data 待释放的数据指针
 */
typedef void (*ux_api_usb_release_msg_memory_handler)(uint8_t *data);

/**
* @brief 通过usb链路发送消息到主机端
* @param [in] event_type 事件类型 @ref ux_api_usb_hid_event_t。
* @param [in] data uint8_t* 类型 指向私有协议数据指针。
* @param [in] data_len 私有协议数据长度。
* @param [in] handler 释放HID消息内存的回调函数。
* @return errcode_t 返回操作的状态码，表示调用成功或失败的具体原因。
*/
errcode_t ux_api_usb_push_event_to_usb_service(uint8_t event_type, uint8_t *data, uint32_t data_len,
    ux_api_usb_release_msg_memory_handler handler);

/**
* @brief 设置UAC通道数量
* @param channels 需要设置的通道数量,支持设置2/4/8声道 @ref ux_api_usb_uac_channels_t
* @return errcode_t 返回操作的状态码，表示调用成功或失败的具体原因。
*/
errcode_t ux_api_usb_update_uac_channel_nums(ux_api_usb_uac_channels_t channels);

/**
* @brief 打开或者关闭UAC扬声器
* @param [in] enable true: 打开扬声器，false: 关闭扬声器
* @return errcode_t 返回操作的状态码，表示调用成功或失败的具体原因。
*/
errcode_t ux_api_usb_set_speaker_state(bool enable);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
