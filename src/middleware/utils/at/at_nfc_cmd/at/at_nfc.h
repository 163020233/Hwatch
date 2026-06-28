/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: At nfc header \n
 *
 * Date: 2025-09-19 \n
 */

#ifndef __AT_NFC_H__
#define __AT_NFC_H__

#include "td_base.h"
#include "at.h"
#include "nfc_defs.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    uint8_t nfcid[NFCID1_MAX_LEN];
    uint8_t nfcidLen;
    uint8_t sensRes[SENS_RES_LEN];
    uint8_t sak;
} NonContactParam;

enum nfc_status {
    DEVICE_IDLE,
    DEVICE_DISCOVERY,
    DEVICE_POLL_ACTIVE,
    DEVICE_OPEN,
};

enum mxx_card {
    NFC_MXX_CARD_BEGIN = 0,
    NFC_MXX_CARD_1 = NFC_MXX_CARD_BEGIN,
    NFC_MXX_CARD_2,
    NFC_MXX_CARD_END = NFC_MXX_CARD_2,
};

td_void los_at_nfc_cmd_register(td_void);

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif /* end of at_nfc.h */
