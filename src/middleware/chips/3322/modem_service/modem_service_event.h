/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: modem_service_event
 * Create: 2026
 */

#ifndef MODEM_SERVICE_EVENT_H
#define MODEM_SERVICE_EVENT_H

#include <stdint.h>
#include "errcode.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MODEM_EVENT_WIFI_INFO = 0,
    MODEM_EVENT_NET_RATE,
    MODEM_EVENT_MAX
} modem_event_type_t;

int32_t modem_message_process(uint8_t* data, size_t len);

#ifdef __cplusplus
}
#endif

#endif // MODEM_SERVICE_EVENT_H
