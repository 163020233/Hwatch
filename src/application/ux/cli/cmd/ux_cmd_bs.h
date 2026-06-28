/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provide ux cmd sle service \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-05-02， Create file. \n
 */
#ifndef UX_CMD_BS_H
#define UX_CMD_BS_H

#include <stdint.h>
#include "errcode.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

errcode_t ux_cmd_set_protocol(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_protocol(uint16_t device_id, uint8_t *data, uint16_t data_len);

errcode_t ux_cmd_set_role(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_role(uint16_t device_id, uint8_t *data, uint16_t data_len);

errcode_t ux_cmd_set_bs_name(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_bs_name(uint16_t device_id, uint8_t *data, uint16_t data_len);

errcode_t ux_cmd_set_sle_local_addr(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_sle_local_addr(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_sle_set_peer_addr(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_sle_get_peer_addr(uint16_t device_id, uint8_t *data, uint16_t data_len);

errcode_t ux_cmd_set_remote_whitelist_addr(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_remote_whitelist_addr(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_clear_remote_whitelist_addr(uint16_t device_id, uint8_t *data, uint16_t data_len);

errcode_t ux_cmd_start_pairing(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_start_whitelist_pairing(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_stop_pairing(uint16_t device_id, uint8_t *data, uint16_t data_len);

errcode_t ux_cmd_start_connect(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_stop_connect(uint16_t device_id, uint8_t *data, uint16_t data_len);

errcode_t ux_cmd_connect_to_device(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_disconnect_to_device(uint16_t device_id, uint8_t *data, uint16_t data_len);

errcode_t ux_cmd_get_bg_info(uint16_t device_id, uint8_t *data, uint16_t data_len);

errcode_t ux_cmd_bs_get_autorate_param(uint16_t device_id, uint8_t *data, uint16_t data_len);

errcode_t ux_cmd_bs_set_vendor_info(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_bs_get_vendor_info(uint16_t device_id, uint8_t *data, uint16_t data_len);

errcode_t ux_cmd_bs_sle_clear_pair_info(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_bs_sle_clear_pair_info_by_addr(uint16_t device_id, uint8_t *data, uint16_t data_len);

errcode_t ux_cmd_bs_sle_call_stream_create(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_bs_sle_call_stream_destroy(uint16_t device_id, uint8_t *data, uint16_t data_len);

errcode_t ux_cmd_bs_sle_music_stream_create(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_bs_sle_music_stream_destroy(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_bs_set_sle_cds_key(uint16_t device_id, uint8_t *data, uint16_t data_len);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
