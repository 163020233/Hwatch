/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 * Description: simulator source file
 * Create: 2025-10
 */

#include "bts_br_gap.h"
#include "bts_a2dp_source.h"
#include "bts_le_gap.h"
#include "bts_a2dp_sink.h"
#include "bts_spp.h"
#include "bts_gatt_server.h"

errcode_t gap_br_get_local_device_info(gap_local_device_info_t *dev_info)
{
    return 0;
}

errcode_t gap_br_get_paired_devices_list(gap_remote_device_info_t *dev_infos, uint8_t *number)
{
    return 0;
}

errcode_t gap_br_get_remote_device_info(const bd_addr_t *addr, gap_remote_device_info_t *dev_info)
{
    return 0;
}

errcode_t gap_read_remote_rssi_value(const bd_addr_t *addr)
{
    return 0;
}

errcode_t a2dp_src_connect(const bd_addr_t *bd_addr)
{
    return 0;
}

errcode_t gap_br_reply_acl_connect_request(const bd_addr_t *addr, bool accept, uint8_t reason)
{
    return 0;
}

errcode_t gap_ble_set_ctkd_enable(uint8_t ctkd_enable)
{
    return 0;
}

errcode_t a2dp_snk_start_accept(const bd_addr_t *bd_addr)
{
    return 0;
}

uint16_t spp_server_create(spp_create_socket_para_t *socket_para, const char *name, unsigned int len)
{
    return 0;
}

errcode_t gatts_add_service_sync(uint8_t server_id, bt_uuid_t *service_uuid, bool is_primary, uint16_t *handle)
{
    return 0;
}

errcode_t a2dp_src_register_callbacks(a2dp_src_callbacks_t *func)
{
    return 0;
}

errcode_t gap_register_callbacks(gap_callbacks_t *func)
{
    return 0;
}

errcode_t a2dp_snk_register_callbacks(a2dp_snk_callbacks_t *func)
{
    return 0;
}