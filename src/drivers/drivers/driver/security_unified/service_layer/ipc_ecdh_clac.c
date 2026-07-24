/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: BT BR/EDR Simple Secure Paring App core clac ecdh
 *
 * Author: BGTP
 *
 * Create: 2024-05-08
 */

#include "soc_osal.h"
#include "debug_print.h"
#include "print_config.h"
#include "securec.h"
#include "ipc.h"
#if defined(SUPPORT_IPC)
#include "ipc_actions.h"
#elif defined(IPC_NEW)
#include "ipc_porting.h"
#endif
#include "unified_cipher_pke.h"
#include "trng.h"
#include "ipc_ecdh_clac.h"

#define FILE_ID FILE_ID_SERVICE_LAYER_IPC_ECDH_CLAC

#define IPC_ECDH_SEND_TIMEOUT 500

#ifdef IPC_NEW
void ipc_new_spin_send_message(uint8_t *msg_buffer, uint32_t buffer_len)
{
    ipc_msg_info_t msg_info;
    msg_info.dst_core = CORES_BT_CORE;
    msg_info.priority = 1;
    msg_info.channel = 1;
    msg_info.msg_id = IPC_MSG_AP_BT_GET_ECDH;
    msg_info.buf_addr = msg_buffer;
    msg_info.buf_len = buffer_len;
    uapi_ipc_send_msg_async(&msg_info);
}
#endif

void uapi_drv_clac(ipc_ecdh_payload_t *param)
{
    uapi_drv_cipher_pke_data_t uapi_priv_key = {
        .length = param->curve_length,
        .data = param->priv_key.val,
    };
    uapi_drv_cipher_pke_ecc_point_t uapi_public_key = {
        .length = param->curve_length,
        .x = param->public_key.x,
        .y = param->public_key.y,
    };
    uapi_drv_cipher_pke_data_t uapi_dhkey = {
        .length = param->curve_length,
        .data = param->dh_key.val,
    };

    errcode_t status = 0;
    switch (param->ecdh_type) {
        case ECC_GEN_KEY :
            status = uapi_drv_cipher_pke_ecc_gen_key(param->curve_type, NULL, &uapi_priv_key, &uapi_public_key);
            break;
        case ECC_GEN_ECDH_KEY :
            status = uapi_drv_cipher_pke_ecc_gen_ecdh_key(param->curve_type, &uapi_public_key,
                &uapi_priv_key, &uapi_dhkey);
            break;
        case CHECK_DOT_ON_CURVE :
            status = uapi_drv_cipher_pke_check_dot_on_curve(param->curve_type, &uapi_public_key,
                &(param->on_curve));
            break;
        case GET_SEC_RAND :
            status = uapi_drv_cipher_trng_get_random(&(param->rand));
            break;
        default :
            break;
    }
    if (status != ERRCODE_SUCC) {
        oam_err_log("uapi_drv_clac: ecdh_type %d, status %x", param->ecdh_type, status);
    }
}

#ifdef SUPPORT_IPC
bool ipc_app_callback(ipc_action_t message, const volatile ipc_payload *payload_p, cores_t src, uint32_t id)
#elif defined(IPC_NEW)
void ipc_app_callback(uint8_t *payload_p, uint32_t payload_len)
#endif
{
#ifdef SUPPORT_IPC
    UNUSED(message);
    UNUSED(src);
    UNUSED(id);
    if (payload_p == NULL) { return true; }
#elif defined(IPC_NEW)
    UNUSED(payload_len);
    if (payload_p == NULL) { return; }
#endif

    ipc_ecdh_payload_t ipc_param;
    (void)memcpy_s((void *)&ipc_param, sizeof(ipc_ecdh_payload_t), (void *)payload_p, sizeof(ipc_ecdh_payload_t));

    uapi_drv_clac(&ipc_param);

#ifdef SUPPORT_IPC
    ipc_spin_send_message_timeout(CORES_BT_CORE, IPC_ACTION_AP_BT_GET_ECDH, (ipc_payload *)&ipc_param,
                                  sizeof(ipc_ecdh_payload_t), IPC_PRIORITY_HIGHEST, true, IPC_ECDH_SEND_TIMEOUT);
    return true;
#elif defined(IPC_NEW)
    ipc_new_spin_send_message((uint8_t *)&ipc_param, sizeof(ipc_ecdh_payload_t));
#endif
}

void ipc_acce_ecdh_init(void)
{
#ifdef SUPPORT_IPC
    ipc_register_handler(IPC_ACTION_BT_AP_ACCE_ECDH, ipc_app_callback);
#elif defined(IPC_NEW)
    ipc_rx_handler_info_t handler_info = {0};
    handler_info.msg_id = IPC_MSG_BT_AP_ACCE_ECDH;
    handler_info.cb = ipc_app_callback;
    uapi_ipc_register_rx_handler(&handler_info);
#endif
}
