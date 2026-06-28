/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description: Provide ux usb process \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-06-16， Create file. \n
 */
#ifndef USB_SERVICE_H
#define USB_SERVICE_H
#include "stdint.h"
#include "errcode.h"
#include "stdbool.h"
#include "ux_audio_stream.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
typedef void (*ux_usb_serivce_func)(const uint8_t *param, uint16_t length);

typedef enum {
    USB_CONNECT,
    USB_DISCONNECT,
    USB_UPDATE_PARAM,
    USB_UPDATE_UAC_CHANNELS,
    UAC_UPLINK_START,
    UAC_UPLINK_STOP,
    USB_SERVICE_INDEX_MAX,
} ux_usb_service_index_t;

typedef struct {
    uint8_t service_index;
    ux_usb_serivce_func func;
} ux_usb_serivce_map_t;

void ux_usb_set_uac_event_state(bool state);
bool ux_usb_get_uac_event_state(void);
void ux_usb_service_execute(uint8_t service_index, uint8_t *param, uint16_t length);
void ux_usb_activity_init(void);
void ux_usb_activity_deinit(void);
errcode_t ux_usb_task_init(void);
void ux_usb_task_deinit(void);
void usb_activity_uac_event_process(uint32_t event, int32_t value);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
