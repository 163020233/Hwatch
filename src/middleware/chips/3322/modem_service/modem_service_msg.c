/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: modem_service_msg
 * Create: 2026
 */

#include <stdio.h>
#include "securec.h"
#include "osal_list.h"
#include "soc_osal.h"
#include "modem_service_event.h"
#include "ril_interface.h"
#include "modem_service_msg.h"

#ifdef __cplusplus
extern "C" {
#endif

static OSAL_LIST_HEAD(msg_respond_handlers);
static osal_mutex g_msg_handler_mutex;

int32_t modem_service_msg_respond(uint8_t* data, size_t len)
{
    modem_service_msg_handler_t *cmd = NULL;

    if (osal_mutex_lock(&g_msg_handler_mutex) != OSAL_SUCCESS) {
        return ERRCODE_FAIL;
    }

    osal_list_for_each_entry(cmd, &msg_respond_handlers, node) {
        if (cmd->handler != NULL) {
            cmd->handler(data, len);
        }
    }

    osal_mutex_unlock(&g_msg_handler_mutex);

    return ERRCODE_SUCC;
}

errcode_t uapi_modem_service_init(void)
{
    if (osal_mutex_init(&g_msg_handler_mutex) != OSAL_SUCCESS) {
        return ERRCODE_FAIL;
    }

    ril_register_at_reply_callback(modem_service_msg_respond);
    uapi_modem_service_register_handler(modem_message_process);
    return ERRCODE_SUCC;
}

errcode_t uapi_modem_service_register_handler(msg_respond_callback_t handler)
{
    modem_service_msg_handler_t *cmd = NULL;
    if (handler == NULL) {
        return ERRCODE_INVALID_PARAM;
    }

    if (osal_mutex_lock(&g_msg_handler_mutex) != OSAL_SUCCESS) {
        return ERRCODE_FAIL;
    }
    osal_list_for_each_entry(cmd, &msg_respond_handlers, node) {
        if (cmd->handler == handler) {
            osal_mutex_unlock(&g_msg_handler_mutex);
            return ERRCODE_SUCC;
        }
    }
    osal_mutex_unlock(&g_msg_handler_mutex);

    modem_service_msg_handler_t *inst = (modem_service_msg_handler_t *)malloc(sizeof(modem_service_msg_handler_t));
    if (inst == NULL) {
        return ERRCODE_MALLOC;
    }
    memset_s(inst, sizeof(*inst), 0, sizeof(modem_service_msg_handler_t));
    inst->handler = handler;

    if (osal_mutex_lock(&g_msg_handler_mutex) != OSAL_SUCCESS) {
        free(inst);
        return ERRCODE_FAIL;
    }

    osal_list_add_tail(&inst->node, &msg_respond_handlers);

    osal_mutex_unlock(&g_msg_handler_mutex);

    return ERRCODE_SUCC;
}

errcode_t uapi_modem_service_unregister_handler(msg_respond_callback_t handler)
{
    errcode_t ret = ERRCODE_FAIL;
    modem_service_msg_handler_t *found = NULL;

    if (handler == NULL) {
        return ERRCODE_INVALID_PARAM;
    }

    if (osal_mutex_lock(&g_msg_handler_mutex) != OSAL_SUCCESS) {
        return ret;
    }

    osal_list_for_each_entry(found, &msg_respond_handlers, node) {
        if (found->handler == handler) {
            osal_list_del(&found->node);
            free(found);
            ret = ERRCODE_SUCC;
            break;
        }
    }

    osal_mutex_unlock(&g_msg_handler_mutex);

    return ret;
}

#ifdef __cplusplus
}
#endif
