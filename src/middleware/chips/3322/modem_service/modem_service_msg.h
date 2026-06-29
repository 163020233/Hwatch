/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: modem_service_msg
 * Create: 2026
 */

#ifndef MODEM_SERVICE_MSG_HANDLER_H
#define MODEM_SERVICE_MSG_HANDLER_H

#include <stdint.h>
#include "errcode.h"
#include "osal_list.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int32_t (*msg_respond_callback_t)(uint8_t* data, size_t len);

typedef struct {
    msg_respond_callback_t handler;
    struct osal_list_head node;
} modem_service_msg_handler_t;

errcode_t uapi_modem_service_init(void);
errcode_t uapi_modem_service_register_handler(msg_respond_callback_t handler);
errcode_t uapi_modem_service_unregister_handler(msg_respond_callback_t handler);

#ifdef __cplusplus
}
#endif

#endif // MODEM_SERVICE_MSG_HANDLER_H
