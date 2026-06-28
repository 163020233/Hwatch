/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: bt signaling test header file \n
 */
#ifndef BT_SIG_ATCMD_H
#define BT_SIG_ATCMD_H

#include "errcode.h"
#include "at.h"
#include "dut.h"
#include "bt_rf_test.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup dut_atcmd
 * @ingroup
 * @{
 */

typedef struct {
    bt_test_pkg_type bt_package_type;
    uint8_t local_channel;
    uint8_t pkg_type;
    uint16_t pkg_len;
    uint8_t ble_phy;
} bt_pkg_type_set_t;

#define CHANEL_FREQ_HIGH       2482
#define CHANEL_FREQ_LOW        2402
#define CHANEL_FREQ_INDEX_LOW  0
#define CHANEL_FREQ_INDEX_HIGH 78
#define BT_POW_HIGH            32768
#define BT_POW_LOW             (-32768)

errcode_t bt_sig_at_cmd_init(void);
void match_bt_addr(const char *para, bd_addr addr);
/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
