/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: dev_haid_info.c header file
 * Author: AuthorNameMagicTag
 * Create: 2023-11-30
 */

#ifndef HAID_DEV_INFO_H
#define HAID_DEV_INFO_H

#include "haid_tlv.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* tlv_info查询回调 */
typedef uint16_t (*tlv_info_query_func_cb)(uint8_t *payload, uint16_t len, uint8_t type);

typedef struct {
    tlv_info_query_func_cb func; /* 各type的数据拼包处理函数  */
} protocol_tlv_info_query_map_t; /* 数组下标表示type */

typedef struct {
    protocol_tlv_info_query_map_t *map;   /* cmd id对应map指针 */
    uint8_t size;                          /* cmd id对应的type数量 */
} protocol_tlv_info_query_tab_t;

/* 5.3 一个cmd id对应一张map表，根据size即type数量注册每个map表 */
void uapi_register_haid_query_type_map(uint8_t cmd_id, uint16_t size);
/* 根据cmd id和type注册对应的处理函数 */
void uapi_register_haid_query_type_info(uint8_t cmd_id, uint8_t type, tlv_info_query_func_cb func);

protocol_tlv_info_query_map_t *uapi_get_haid_query_type_info(uint8_t cmd_id, uint8_t type);

void protocol_haid_query_init(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
