/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description: Provide ux cmd bg \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-07-17， Create file. \n
 */
#include <securec.h>
#include "common_def.h"
#include "feature_config.h"
#include "ux_errcode.h"
#include "ux_common_log.h"
#include "ux_fwk_process.h"
#include "ux_event_define.h"
#include "ux_device_manager.h"
#include "ux_system_manager.h"
#include "ux_common_define.h"
#include "ux_utils.h"
#ifndef DEMO_WATCH
#include "audio_types.h"
#include "timer_manager.h"
#include "ux_api_bs.h"
#include "ux_api_fwk.h"
#include "ux_api_communication.h"
#include "ux_at_interface.h"
#include "ux_audio_voice.h"
#include "ux_audio_game.h"
#include "ux_bg_config.h"
#include "ux_bs_rpc.h"
#include "ux_bs_generic.h"
#include "ux_bs_sle.h"
#ifndef UT_TEST
#include "ux_bs_sle_livemic_pairing_service.h"
#endif
#include "ux_bs_property.h"
#include "ux_api_bs_connection.h"
#include "ux_bs_sle_wrap.h"
#include "ux_cmd_bs.h"

#endif

#ifndef DEMO_WATCH
errcode_t ux_cmd_set_protocol(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);

    errcode_t ret;
    if (data == NULL || data_len == 0) {
        ux_log_err("[ux][cmd_bs]Invalid parameter.\r\n");
        return UX_ERR_INVALID_PARAMETER;
    }
    switch (*data) {
        case UX_API_BS_PROTOCOL_BT:
            ret = ux_api_bs_set_protocol(UX_API_BS_PROTOCOL_BT);
            break;
        case UX_API_BS_PROTOCOL_SLE:
            ret = ux_api_bs_set_protocol(UX_API_BS_PROTOCOL_SLE);
            break;
        case UX_API_BS_PROTOCOL_COMBO:
            ret = ux_api_bs_set_protocol(UX_API_BS_PROTOCOL_COMBO);
            break;
        default:
            ux_log_err("[ux][cmd_bs]Invalid protocol.\r\n");
            return UX_ERR_BG_INVALID_ROLE;
    }
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][cmd_bs]ux_api_bs_set_protocol failed, ret: %d.\r\n", ret);
    }
    return ret;
}

errcode_t ux_cmd_get_protocol(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);

    if (data == NULL || data_len != sizeof(ux_api_bs_protocol_t)) {
        ux_log_err("[ux][cmd_bs]Invalid parameter.\r\n");
        return UX_ERR_INVALID_PARAMETER;
    }

    ux_api_bs_protocol_t protocol = ux_api_bs_get_protocol();
    errno_t err = memcpy_s(data, data_len, &protocol, sizeof(ux_api_bs_protocol_t));
    if (err != EOK) {
        ux_log_err("[ux][cmd][ux_cmd_get_protocol] memcpy_s failed");
        return ERRCODE_MEMCPY;
    }

    return UX_ERR_SUCCESS;
}

errcode_t ux_cmd_set_role(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);

    errcode_t ret;
    if (data == NULL || data_len == 0) {
        ux_log_err("[ux][cmd_bs]Invalid parameter.\r\n");
        return UX_ERR_INVALID_PARAMETER;
    }
    switch (*data) {
        case 0:
            ret = ux_bs_set_role(UX_DEVICE_ROLE_MASTER);
            break;
        case 1:
            ret = ux_bs_set_role(UX_DEVICE_ROLE_SLAVE);
            break;
        default:
            ux_log_err("[ux][cmd_bs]Invalid role.\r\n");
            return UX_ERR_BG_INVALID_ROLE;
    }
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][cmd_bs]ux_bs_set_role failed, ret: %d.\r\n", ret);
    }
    return ret;
}

errcode_t ux_cmd_get_role(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);

    if (data == NULL || data_len == 0) {
        ux_log_err("[ux][cmd_bs]Invalid parameter.\r\n");
        return UX_ERR_INVALID_PARAMETER;
    }

    ux_device_role_t role = UX_DEVICE_ROLE_MAX;
    errcode_t ret = ux_bs_get_role(&role);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][cmd_bs]ux_bs_get_role failed, ret: 0x%x.\r\n", ret);
        return ret;
    }
    errno_t err = memcpy_s(data, data_len, &role, sizeof(ux_device_role_t));
    if (err != EOK) {
        ux_log_err("[ux][cmd][ux_cmd_get_role] memcpy_s failed");
        return ERRCODE_MEMCPY;
    }

    return UX_ERR_SUCCESS;
}

errcode_t ux_cmd_set_bs_name(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    if (data == NULL || data_len == 0) {
        return ERRCODE_INVALID_PARAM;
    }
    if (data_len >= DEVICE_NAME_LEN) {
        return ERRCODE_NOT_SUPPORT;
    }
#ifndef DEMO_WATCH
    /* BT_NAME is non_upgrade */
    errcode_t res = ux_api_bs_set_name(UX_API_BS_PROTOCOL_COMBO, (char*)data, data_len);
    if (res != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
#else
    return ux_api_bt_config(UX_API_BT_CONFIG_LOCAL_NAME, data, data_len);
#endif
}

errcode_t ux_cmd_get_bs_name(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    if (data == NULL || data_len == 0) {
        return ERRCODE_INVALID_PARAM;
    }

#ifndef DEMO_WATCH
    errcode_t ret = ux_api_bs_get_name(UX_API_BS_PROTOCOL_COMBO, (char*)data, data_len);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
#else
    return ux_api_bt_get_name(data, data_len);
#endif
}

errcode_t ux_cmd_set_sle_local_addr(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);

    errcode_t ret;
    if (data == NULL || data_len != UX_BS_ADDR_STR_LEN) {
        ux_log_err("[ux][cmd_bs]set sle addr Invalid parameter, data_len=%d.\r\n", data_len);
        return UX_ERR_INVALID_PARAMETER;
    }
    ret = ux_api_bs_set_addr(UX_API_BS_PROTOCOL_SLE, (char*)data, data_len);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][cmd_bs]ux_api_bs_set_addr failed, ret: %d.\r\n", ret);
    }
    return ret;
}

errcode_t ux_cmd_get_sle_local_addr(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);

    errcode_t ret;
    if (data == NULL || data_len != UX_BG_ADDR_LEN) {
        ux_log_err("[ux][cmd_bs]Invalid parameter.\r\n");
        return UX_ERR_INVALID_PARAMETER;
    }
    ret = ux_api_bs_get_addr(UX_API_BS_PROTOCOL_SLE, data, data_len);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][cmd_bs]ux_api_bs_get_addr failed, ret: 0x%x.\r\n", ret);
    }
    return ret;
}

errcode_t ux_cmd_sle_set_peer_addr(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);

    errcode_t ret;
    if (data == NULL || data_len != UX_BS_ADDR_STR_LEN) {
        ux_log_err("[ux][cmd_bs]Invalid parameter.\r\n");
        return UX_ERR_INVALID_PARAMETER;
    }
    ret = ux_api_bs_set_peer_addr(UX_API_BS_PROTOCOL_SLE, (char*)data, data_len);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][cmd_bs]ux_api_bs_set_peer_addr failed, ret: %d.\r\n", ret);
    }
    return ret;
}

errcode_t ux_cmd_sle_get_peer_addr(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);

    errcode_t ret;
    if (data == NULL || data_len != UX_BG_ADDR_LEN) {
        ux_log_err("[ux][cmd_bs]Invalid parameter.\r\n");
        return UX_ERR_INVALID_PARAMETER;
    }
    ret = ux_api_bs_get_peer_addr(UX_API_BS_PROTOCOL_SLE, data, data_len);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][cmd_bs]ux_api_bs_get_peer_addr failed, ret: %d.\r\n", ret);
    }
    return ret;
}

errcode_t ux_cmd_set_remote_whitelist_addr(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    unused(data);
    unused(data_len);
#if defined(MELODY_DEMO_BMIC_RX) || defined(MELODY_DEMO_DONGLE) || defined(MELODY_DEMO_SOUND_BOX) || \
    defined(MELODY_DEMO_SOUNDBAR)
    errcode_t ret;
    if (data == NULL || data_len != UX_BG_ADDR_LEN) {
        ux_log_err("[ux][cmd_bs]Invalid parameter.\r\n");
        return UX_ERR_INVALID_PARAMETER;
    }
    ret = ux_api_bs_set_whitelist_addr((const uint8_t *)data, data_len);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][cmd_bs]ux_api_bs_set_whitelist_addr failed, ret: 0x%x.\r\n", ret);
        return ret;
    }
#endif
    return UX_ERR_SUCCESS;
}

errcode_t ux_cmd_get_remote_whitelist_addr(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    unused(data);
    unused(data_len);
#if defined(MELODY_DEMO_BMIC_RX) || defined(MELODY_DEMO_DONGLE) || defined(MELODY_DEMO_SOUND_BOX) || \
    defined(MELODY_DEMO_SOUNDBAR)
    if (data == NULL || data_len != (sizeof(ux_api_bs_whitelist_node_t) * UX_API_BS_MAX_WHITELIST_NUM)) {
        ux_log_err("[ux][cmd_bs]Invalid parameter.\r\n");
        return UX_ERR_INVALID_PARAMETER;
    }
    errno_t err;
    errcode_t ret;
    ux_api_bs_whitelist_t whitelist = {0};
    ret = ux_api_bs_get_whitelist_addr(&whitelist);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][cmd_bs]ux_api_bs_get_whitelist_addr failed, ret: 0x%x.\r\n", ret);
        return ret;
    }
    err = memcpy_s(data, data_len, whitelist.whitelist_arr,
                   sizeof(ux_api_bs_whitelist_node_t) * UX_API_BS_MAX_WHITELIST_NUM);
    if (err != EOK) {
        ux_log_err("[ux][cmd_bs]memcpy_s failed\r\n");
        return UX_ERR_MEMCPY_FAILED;
    }
#endif
    return UX_ERR_SUCCESS;
}

errcode_t ux_cmd_clear_remote_whitelist_addr(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    unused(data);
    unused(data_len);
#if defined(MELODY_DEMO_BMIC_RX) || defined(MELODY_DEMO_DONGLE) || defined(MELODY_DEMO_SOUND_BOX) || \
    defined(MELODY_DEMO_SOUNDBAR)
    errcode_t ret;
    ret = ux_api_bs_clear_all_whitelist();
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][cmd_bs]ux_api_bs_clear_all_whitelist failed, ret: 0x%x.\r\n", ret);
        return ret;
    }
#endif
    return UX_ERR_SUCCESS;
}

static errcode_t start_pairing_sle(void)
{
    errcode_t ret = UX_ERR_SUCCESS;

#ifdef CONFIG_SUPPORT_BS_SLE_L3
    ret = ux_api_bs_start_discovery(UX_API_BS_PAIRING_MODE_DEFAULT);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][cmd_bs]ux_api_bs_start_discovery failed.\r\n");
    }
#elif defined(CONFIG_SUPPORT_BS_SLE_L4)
    ret = ux_api_bs_stop_audio_mode();
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][cmd_bs]ux_api_bs_stop_audio_mode failed.\r\n");
        return ret;
    }
    ret = ux_api_bs_stop_connect();
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][cmd_bs]ux_api_bs_stop_connect failed.\r\n");
        return ret;
    }
    ret = ux_api_bs_start_discovery(UX_API_BS_PAIRING_MODE_AIR);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][cmd_bs]ux_api_bs_start_discovery failed.\r\n");
    }
#endif
    return ret;
}

errcode_t ux_cmd_stop_connect(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    unused(data);
    unused(data_len);
    errcode_t ret = ERRCODE_SUCC;
#if defined(MELODY_DEMO_EARPHONE) || defined(MELODY_DEMO_SOUND_BOX) || defined(MELODY_DEMO_HA)
    ret = ux_api_bs_stop_connect();
#endif
    return ret;
}

errcode_t ux_cmd_start_connect(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    errcode_t ret = ERRCODE_SUCC;
#if defined(MELODY_DEMO_EARPHONE) || defined(MELODY_DEMO_SOUND_BOX) || defined(MELODY_DEMO_HA)
    uint32_t set_time = 0;
    if (data != NULL && data_len != 0) {
        char *p = NULL;
        char *time = strtok_r((char *)data, "\r", &p);
        if (time == NULL) {
            return ERRCODE_FAIL;
        }
        int32_t val = (int32_t)strtol(time, NULL, 0);
        if (val > 0) {
            set_time = (uint32_t)val;
        }
    }
    ret = ux_api_bs_start_connect(set_time);
    ux_log_info("[ux][cmd_bs]ux_cmd_start_connect set_time:%us.\r\n", set_time);
#else
    unused(data);
    unused(data_len);
#endif
    return ret;
}

errcode_t ux_cmd_start_pairing(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    unused(data);
    unused(data_len);

    errcode_t ret = UX_ERR_SUCCESS;
    ux_api_bs_protocol_t protocol = ux_api_bs_get_protocol();
    switch (protocol) {
        case UX_API_BS_PROTOCOL_BT:
            break;
        case UX_API_BS_PROTOCOL_SLE:
            ret = start_pairing_sle();
            break;
        default:
            ux_log_err("[ux][cmd_bs]Invalid protocol.\r\n");
            return UX_ERR_BG_INVALID_PROTOCOL;
    }
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][cmd_bs]ux_cmd_start_pairing failed, protocol: %d.\r\n", protocol);
        return ret;
    }
    return ret;
}

errcode_t ux_cmd_start_air_pairing(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    unused(data);
    unused(data_len);

    errcode_t ret = UX_ERR_SUCCESS;
#if defined(CONFIG_SUPPORT_BS_SLE_L4)
    ret = ux_api_bs_stop_audio_mode();
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][cmd_bs]ux_api_bs_stop_audio_mode failed.\r\n");
        return ret;
    }
    ret = ux_api_bs_stop_connect();
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][cmd_bs]ux_api_bs_stop_connect failed.\r\n");
        return ret;
    }
    ret = ux_api_bs_start_discovery(UX_API_BS_PAIRING_MODE_AIR);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][cmd_bs]ux_api_bs_start_discovery failed.\r\n");
        return ret;
    }
#elif defined CONFIG_SUPPORT_BS_SLE_L3
    ret = ux_api_bs_start_discovery(UX_API_BS_PAIRING_MODE_AIR);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][cmd_bs]ux_api_bs_start_discovery failed.\r\n");
        return ret;
    }
#endif
    return ret;
}

errcode_t ux_cmd_start_whitelist_pairing(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    unused(data);
    unused(data_len);

    errcode_t ret = UX_ERR_SUCCESS;
#if defined(CONFIG_SUPPORT_BS_SLE_L4)
    ret = ux_api_bs_stop_audio_mode();
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][cmd_bs]ux_api_bs_stop_audio_mode failed.\r\n");
        return ret;
    }
    ret = ux_api_bs_stop_connect();
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][cmd_bs]ux_api_bs_stop_connect failed.\r\n");
        return ret;
    }
    ret = ux_api_bs_start_discovery(UX_API_BS_PAIRING_MODE_WHITELIST);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][cmd_bs]ux_api_bs_start_discovery failed.\r\n");
        return ret;
    }
#elif defined CONFIG_SUPPORT_BS_SLE_L3
    ret = ux_api_bs_start_discovery(UX_API_BS_PAIRING_MODE_WHITELIST);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][cmd_bs]ux_api_bs_start_discovery failed.\r\n");
        return ret;
    }
#endif
    return ret;
}

errcode_t ux_cmd_stop_pairing(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    unused(data);
    unused(data_len);

    errcode_t ret = UX_ERR_SUCCESS;
    ux_api_bs_protocol_t protocol = ux_api_bs_get_protocol();
    switch (protocol) {
        case UX_API_BS_PROTOCOL_BT:
            break;
        case UX_API_BS_PROTOCOL_SLE:
#ifdef CONFIG_SUPPORT_BS_SLE_L4
            ret = ux_sle_stop_pairing();
            if (ret != UX_ERR_SUCCESS) {
                ux_log_err("[ux][cmd_bs]ux_sle_stop_pairing failed, ret: 0x%x.\r\n", ret);
            }
#elif defined(CONFIG_SUPPORT_BS_SLE_L3)
            ret = ux_bs_sle_stop_discovery();
            if (ret != UX_ERR_SUCCESS) {
                ux_log_err("[ux][cmd_bs]ux_bs_sle_stop_discovery failed, ret: 0x%x.\r\n", ret);
            }
#endif
            break;
        default:
            ux_log_err("[ux][cmd_bs]Invalid protocol.\r\n");
            return UX_ERR_BG_INVALID_PROTOCOL;
    }
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][cmd_bs]ux_cmd_stop_pairing failed, protocol: %d.\r\n", protocol);
    }
    return ret;
}

errcode_t ux_cmd_bs_get_autorate_param(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);

    if (data == NULL || data_len != UX_UINT32_BYTE_NUM) {
        ux_log_err("[ux][cmd_bs]Invalid parameter\r\n");
        return UX_ERR_INVALID_PARAMETER;
    }
    uint32_t rate;
    uint8_t tmp_buf[UX_UINT32_BYTE_NUM] = {0};
    errno_t err;

    rate = ux_api_bs_get_autorate_value();
    ux_utils_uint32_to_uint8(tmp_buf, UX_UINT32_BYTE_NUM, rate);
    err = memcpy_s(data, data_len, tmp_buf, UX_UINT32_BYTE_NUM);
    if (err != EOK) {
        ux_log_err("[ux][cmd_bs]memcpy_s failed\r\n");
        return UX_ERR_MEMCPY_FAILED;
    }
    return UX_ERR_SUCCESS;
}

errcode_t ux_cmd_bs_set_sle_cds_key(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    uint8_t cds_key[UX_API_BS_SLE_CDS_KEY_LEN] = {0};
    if (data != NULL) {
        if (data_len > UX_BS_SLE_CDS_KEY_INFO_LEN || (data_len & 0x1) != 0) {
            ux_log_err("[ux][cmd_bs]Invalid data_len:%u max_len:%u\r\n", data_len, UX_BS_SLE_CDS_KEY_INFO_LEN);
            return ERRCODE_INVALID_PARAM;
        }
        for (uint32_t i = 0; i < data_len / UX_CHAR_NUM && i < UX_BS_SLE_CDS_KEY_INFO_LEN; i++) {
            if ((ux_utils_hex_char_to_dec((char)data[UX_CHAR_NUM * i]) < 0) ||
                (ux_utils_hex_char_to_dec((char)data[UX_CHAR_NUM * i + 1]) < 0)) {
                ux_log_err("[ux][cmd_bs]ux_utils_hex_char_to_dec fail\r\n");
                return ERRCODE_INVALID_PARAM;
            }
            cds_key[i] = ((uint8_t)ux_utils_hex_char_to_dec((char)data[UX_CHAR_NUM * i]) * UX_BASE16 +
                (uint8_t)ux_utils_hex_char_to_dec((char)data[UX_CHAR_NUM * i + 1]));
        }
    }
    return ux_api_bs_config_sle_cds_key((const uint8_t *)cds_key, (uint16_t)UX_API_BS_SLE_CDS_KEY_LEN);
}

errcode_t ux_cmd_bs_set_vendor_info(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);

    if (data == NULL || data_len > UX_VENDOR_INFO_MAX_LEN || data_len == 0) {
        ux_log_err("[ux][cmd_bs]Invalid parameter\r\n");
        return UX_ERR_INVALID_PARAMETER;
    }
    errcode_t ret;
    ret = ux_api_bs_set_vendor_info((const uint8_t *)data, (uint8_t)data_len);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][cmd_bs]ux_api_bs_set_vendor_info failed.\r\n");
        return ret;
    }
    return ret;
}

errcode_t ux_cmd_bs_get_vendor_info(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);

    if (data == NULL || data_len != UX_VENDOR_INFO_MAX_LEN) {
        ux_log_err("[ux][cmd_bs]Invalid parameter\r\n");
        return UX_ERR_INVALID_PARAMETER;
    }
    errcode_t ret;
    ret = ux_api_bs_get_vendor_info(data, (uint8_t *)&data_len);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][cmd_bs]ux_api_bs_get_vendor_info failed.\r\n");
        return ret;
    }
    return ret;
}

errcode_t ux_cmd_bs_sle_clear_pair_info(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    unused(data);
    unused(data_len);

    errcode_t ret;
    ret = ux_api_bs_clear_pair_info();
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][cmd_bs]ux_api_bs_clear_pair_info failed.\r\n");
        return ret;
    }
    return ret;
}

errcode_t ux_cmd_bs_sle_clear_pair_info_by_addr(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);

    if (data == NULL || data_len != UX_BG_ADDR_LEN) {
        ux_log_err("[ux][cmd_bs]Invalid parameters\r\n");
        return UX_ERR_INVALID_PARAMETER;
    }

    errcode_t ret;
    ret = ux_api_bs_remove_pair_info(data, data_len);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][cmd_bs]ux_api_bs_remove_pair_info failed\r\n");
        return ret;
    }
    return ret;
}

errcode_t ux_cmd_bs_sle_call_stream_create(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    unused(data);
    unused(data_len);

#if defined(MELODY_DEMO_DONGLE) || defined(MELODY_DEMO_SOUNDBAR)
    errcode_t ret;
    ux_bs_obj_t obj;
    ux_audio_game_codec config = {0};
    config.sample_rate = SAMPLE_RATE_48000;
    config.channel_num = CHANNEL_NUM_1;
    config.sample_bit_width = SAMPLE_BIT_WIDTH_16;
    config.codec_format = CODEC_FORMAT_L2HC;
    // 需要根据UAC的上下行调用game的上下行
    ret = ux_audio_game_up_link_set_param(&config);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][cmd_bs] ux_audio_game_up_link_set_param failed.\r\n");
        return ret;
    }
    ret = ux_audio_game_up_link_start();
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][cmd_bs] ux_audio_game_up_link_start failed.\r\n");
        return ret;
    }

    ret = ux_device_mgr_get_obj(&obj);
    if (ret != ERRCODE_SUCC) {
        ux_log_err("[ux][cmd_bs] ux_device_mgr_get_obj failed.\r\n");
        return ret;
    }

    ret = ux_bs_sle_stream_create(obj, UX_BS_AUDIO_TYPE_UAC_UP);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][cmd_bs] ux_bs_sle_stream_create failed.\r\n");
        return ret;
    }
#endif
    return UX_ERR_SUCCESS;
}

errcode_t ux_cmd_bs_sle_call_stream_destroy(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    unused(data);
    unused(data_len);

#if defined(MELODY_DEMO_DONGLE) || defined(MELODY_DEMO_SOUNDBAR)
    errcode_t ret;
    ux_bs_obj_t obj;
    ret = ux_device_mgr_get_obj(&obj);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][cmd_bs] ux_device_mgr_get_obj failed.\r\n");
        return ret;
    }

    ret = ux_bs_sle_stream_destroy(obj, UX_BS_AUDIO_TYPE_UAC_UP);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][cmd_bs] ux_bs_sle_stream_destroy failed.\r\n");
        return ret;
    }
    ret = ux_audio_game_up_link_stop();
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][cmd_bs] ux_audio_game_up_link_stop failed.\r\n");
        return ret;
    }
#endif
    return UX_ERR_SUCCESS;
}

errcode_t ux_cmd_bs_sle_music_stream_create(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    unused(data);
    unused(data_len);

#if defined(MELODY_DEMO_DONGLE) || defined(MELODY_DEMO_SOUNDBAR)
    errcode_t ret;
    ux_bs_obj_t obj;
    ux_audio_game_codec config = {0};
    config.sample_rate = SAMPLE_RATE_48000;
    config.channel_num = CHANNEL_NUM_1;
    config.sample_bit_width = SAMPLE_BIT_WIDTH_16;
    config.codec_format = CODEC_FORMAT_L2HC;
    // 需要根据UAC的上下行调用game的上下行
    ret = ux_audio_game_down_link_set_param(&config);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][cmd_bs] ux_audio_game_down_link_set_param failed.\r\n");
        return ret;
    }
    ret = ux_audio_game_down_link_start();
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][cmd_bs] ux_audio_game_down_link_start failed.\r\n");
        return ret;
    }

    ret = ux_device_mgr_get_obj(&obj);
    if (ret != ERRCODE_SUCC) {
        ux_log_err("[ux][cmd_bs] ux_device_mgr_get_obj failed.\r\n");
        return ret;
    }

    ret = ux_bs_sle_stream_create(obj, UX_BS_AUDIO_TYPE_UAC_DOWN);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][cmd_bs] ux_bs_sle_stream_create failed.\r\n");
        return ret;
    }
#endif
    return UX_ERR_SUCCESS;
}

errcode_t ux_cmd_bs_sle_music_stream_destroy(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    unused(data);
    unused(data_len);

#if defined(MELODY_DEMO_DONGLE) || defined(MELODY_DEMO_SOUNDBAR)
    errcode_t ret;
    ux_bs_obj_t obj;
    ret = ux_device_mgr_get_obj(&obj);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][cmd_bs] ux_device_mgr_get_obj failed.\r\n");
        return ret;
    }

    ret = ux_bs_sle_stream_destroy(obj, UX_BS_AUDIO_TYPE_UAC_DOWN);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][cmd_bs] ux_bs_sle_stream_destroy failed.\r\n");
        return ret;
    }
    ret = ux_audio_game_down_link_stop();
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][cmd_bs] ux_audio_game_down_link_stop failed.\r\n");
        return ret;
    }
#endif
    return UX_ERR_SUCCESS;
}
#endif
