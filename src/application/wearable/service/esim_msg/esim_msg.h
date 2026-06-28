/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Create Date : 2025-12
 */

#ifndef ESIM_MSG_H
#define ESIM_MSG_H

#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
// ESIM 基础信息
void uapi_esim_get_info(void);

// LPA 相关接口
uint32_t uapi_esim_mcu_set_active_code(uint8_t *para, uint32_t para_len);
uint32_t uapi_esim_mcu_set_iccid(uint8_t *para, uint32_t para_len);
void uapi_esim_init(void);
uint32_t uapi_esim_mcu_reset(void);
uint32_t uapi_esim_mcu_download(void);
uint32_t uapi_esim_mcu_notification(void);
uint32_t uapi_esim_mcu_disable_profile(uint8_t *para, uint32_t para_len);
uint32_t uapi_esim_mcu_enable_profile(uint8_t *para, uint32_t para_len);
uint32_t uapi_esim_mcu_delete_profile(void);
uint32_t uapi_esim_mcu_edit_profile(uint8_t *para, uint32_t para_len);
uint32_t uapi_esim_mcu_get_profile(uint8_t *para, uint32_t para_len);
uint32_t uapi_esim_mcu_get_profile_list(uint8_t *para, uint32_t para_len);

// 机卡绑定相关接口
uint32_t uapi_esim_mcu_get_status(void);
uint32_t uapi_esim_mcu_esim_write_ppk(void);
uint32_t uapi_esim_mcu_bind_card(void);
uint32_t uapi_esim_mcu_verify_card(void);
uint32_t uapi_esim_mcu_unlock_bind(void);
uint32_t uapi_esim_mcu_bind_main(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* ESIM_MSG_H */
