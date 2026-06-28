/*
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies Co., Ltd..
 * Description: simulator mouse driver definition
 */

#include "tcxo.h"
#include "ux_api_bt.h"

uint64_t uapi_tcxo_get_ms(void)
{
    return 0;
}

uint64_t uapi_tcxo_get_us(void)
{
    return 0;
}

errcode_t ux_api_bt_enable(ux_api_bt_protocol_t protocol)
{
    return 0;
}

errcode_t ux_api_bt_disable(ux_api_bt_protocol_t protocol)
{
    return 0;
}

errcode_t ux_api_bt_start_discovery(ux_api_bt_role_t role, ux_api_bt_protocol_t protocol)
{
    return 0;
}

errcode_t ux_api_bt_stop_discovery(ux_api_bt_role_t role, ux_api_bt_protocol_t protocol)
{
    return 0;
}

errcode_t ux_api_bt_pair_device(const ux_api_bt_addr_t *addr, ux_api_bt_protocol_t protocol)
{
    return 0;
}

errcode_t ux_api_bt_connect(ux_api_bt_profile_t profile, const ux_api_bt_addr_t *addr)
{
    return 0;
}

errcode_t ux_api_bt_remove_device(const ux_api_bt_addr_t *addr, ux_api_bt_protocol_t protocol)
{
    return 0;
}

errcode_t ux_api_bt_disconnect(ux_api_bt_profile_t profile, const ux_api_bt_addr_t *addr)
{
    return 0;
}

errcode_t ux_api_bt_config(ux_api_bt_config_t config, void *param, uint16_t len)
{
    return 0;
}

errcode_t ux_api_bt_get_name(uint8_t *name, uint16_t len)
{
    return 0;
}

errcode_t ux_api_spp_service_send_data(uint16_t dev_id, uint8_t *data, uint16_t data_len)
{
    return 0;
}

void ux_api_spp_service_register_msg_handler(uint16_t dev_id, ux_api_spp_msg_handler handler)
{
    return;
}

errcode_t ux_api_bt_get_device_connection_addr(uint8_t *out_addr, uint16_t addr_len)
{
    return 0;
}

errcode_t ux_api_bt_get_profile_state(ux_api_bt_profile_t profile,
    const ux_api_bt_addr_t *addr, ux_api_bt_profile_state_t *profile_state)
{
    return 0;
}

errcode_t ux_api_bt_get_device_info_by_id(uint8_t device_id, ux_api_bt_device_info_t *device_info)
{
    return 0;
}

void ux_api_bt_switch_multiple_devices(bool enable)
{
    return;
}

bool ux_api_bt_is_multiple_device_open(void)
{
    return 0;
}
