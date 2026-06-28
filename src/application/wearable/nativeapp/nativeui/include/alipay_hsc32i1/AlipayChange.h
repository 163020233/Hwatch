/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: transmit header file
 * This file should be changed only infrequently and with great care.
 */
#ifndef ALIPAY_CHANGE_H
#define ALIPAY_CHANGE_H
#include "alipay_nfc_offline_pay.h"
#ifdef __cplusplus
extern "C" {
#endif
void AlipayChangePage(int status, int errcode);
void AlipaySetUrl(uint8_t *txData, uint16_t len);
#ifdef SUPPORT_ALIPAY_TAP
void AlipayCloseNFC(int value);
#endif
#ifdef __cplusplus
}
#endif
#endif
