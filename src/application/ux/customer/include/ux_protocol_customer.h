/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: ux protocol customer header \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-06-28， Create file. \n
 */

#ifndef UX_PROTOCOL_CUSTOMER_H
#define UX_PROTOCOL_CUSTOMER_H

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
    /**
    * @brief  客制化协议初始化
    */
    void (*protocol_customer_init)(void);

    /**
    * @brief  客制化协议去初始化
    */
    void (*protocol_customer_deinit)(void);

    /**
    * @brief  客制化协议数据处理
    * @param[in] param: 传入的协议数据
    * @param[in] len: 传入的协议数据长度
    */
    void (*protocol_customer_msg_process)(const uint8_t *param, uint16_t length);
} ux_protocol_customer_funcs;

ux_protocol_customer_funcs *ux_protocol_get_instance(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
