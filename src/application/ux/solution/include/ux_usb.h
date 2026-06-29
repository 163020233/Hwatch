/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: hearing media api
 * Author: AuthorNameMagicTag
 * Create: 2023-02-28
 */

#ifndef UX_USB_H
#define UX_USB_H

#include "stdio.h"
#include "stdint.h"
#include "errcode.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* USB设备类型 */
typedef enum {
    UX_USB_MASS_STORAGE = 0,
    UX_USB_HID_UAC,

    UX_USB_MAX
} ux_usb_type_t;

typedef struct {
    uint32_t event;
    int32_t value;
} ux_usb_uac_event_t;

/**
* @brief  usb模块初始化操作
*/
void ux_usb_init(void);

/**
* @brief  usb驱动去初始化操作
*/
void ux_usb_deinit(void);

/**
* @brief  提供u盘挂载能力
* @return errcode_t 返回操作的状态码，表示调用成功或失败的具体原因
*/
errcode_t ux_usb_msc_open(void);

/**
* @brief  提供弹出u盘
* @return errcode_t 返回操作的状态码，表示调用成功或失败的具体原因
*/
errcode_t ux_usb_msc_close(void);

#if defined(CONFIG_ENABLE_USB_COMPOSITE) || defined(CONFIG_SUPPORT_USB_HID) || defined(UT_TEST)
/**
* @brief 提供打开HID设备能力
*/
errcode_t ux_usb_hid_open(void);

/**
* @brief 提供关闭HID设备能力
* @return errcode_t 返回操作的状态码，表示调用成功或失败的具体原因。
*/
errcode_t ux_usb_hid_close(void);

/**
* @brief 提供打开UAC设备能力
* @return errcode_t 返回操作的状态码，表示调用成功或失败的具体原因。
*/
errcode_t ux_usb_uac_open(void);

/**
* @brief 提供关闭UAC设备能力
* @return errcode_t 返回操作的状态码，表示调用成功或失败的具体原因。
*/
errcode_t ux_usb_uac_close(void);
#endif

/**
* @brief 打开USB设备
* @return errcode_t 返回操作的状态码，表示调用成功或失败的具体原因。
*/
errcode_t ux_usb_device_open(void);

/**
* @brief 关闭USB设备
*/
void ux_usb_device_close(void);

/**
 * @brief 释放USB消息内存
 * @param data 要释放的内存指针
 * @return 无
 */
void ux_usb_release_msg_memory(uint8_t *data);

/**
 * @brief 分配USB消息内存
 * @param size 需要分配的内存大小
 * @return 分配成功返回内存指针，否则返回NULL
 */
void *ux_usb_malloc_msg_memory(size_t size);

/**
 * @brief 设置USB设备类型
 * @param usb_dev_type 要设置的USB设备类型
 * @return 无
 */
void ux_usb_set_device_type(ux_usb_type_t usb_dev_type);

/**
 * @brief 获取USB设备类型
 * @param 无
 * @return 返回当前USB设备类型
 */
ux_usb_type_t ux_usb_get_device_type(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
