/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: tlv api
 * Author: AuthorNameMagicTag
 * Create: 2023-02-20
 */

#ifndef CONN_TLV_API_H
#define CONN_TLV_API_H

#include "stdint.h"
#include "stdbool.h"
#include "diag_service.h"
#include "conn_tlv_err_code.h"

#define CONN_TLV_MAX_PAYLOAD_SIZE 2048
#define UINT32_BIT_WIDTH 32

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
typedef struct tlv_node {
    uint16_t len;
    uint8_t type; /* type:bit7 1:do not have child, 0:have child */
    uint8_t *value; /* type:bit7=1 not NULL */
    struct tlv_node *next;
} tlv_node_list_t;

bool tlv_is_have_child(uint8_t type);
uint8_t tlv_get_actual_type(uint8_t type);
bool tlv_calc_byte_cnt(uint32_t len, uint32_t *out_len);
void tlv_free_all_node_list(tlv_node_list_t *node);
int32_t tlv_fill_tl_byte(uint8_t *buf, int32_t buf_len, uint8_t type, uint16_t len);
int32_t tlv_fill_tlv_byte(uint8_t *buf, int32_t buf_len, uint8_t type, uint16_t len, const uint8_t *value);
int32_t tlv_trans_bytes_to_node_list(uint8_t *payload, uint32_t payload_len,
    tlv_node_list_t **tlv_node_list, uint32_t *tlv_byte_num);
int32_t tlv_trans_nodes_to_bytes(uint8_t *payload, uint32_t payload_len,
    tlv_node_list_t *node, uint32_t *tlv_byte_num);
diag_ser_data_t *tlv_build_inter_msg_info(diag_ser_header_t *header, uint32_t payload_len, bool frame_flag);
void tlv_send_err_code(diag_ser_data_t *inter_msg, tlv_app_op_code_t err_code);
bool tlv_parse_nodes(diag_ser_data_t *msg, tlv_node_list_t **node_list);
void tlv_send_byte_array(diag_ser_data_t *inter_msg, uint8_t *buf, uint32_t buf_len);
uint32_t tlv_parse_type_mask(tlv_node_list_t *node);
bool tlv_is_type_bit_set(uint32_t mask, uint8_t type);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif // CONN_TLV_API_H
