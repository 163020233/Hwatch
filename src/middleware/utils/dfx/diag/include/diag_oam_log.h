/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: diag oam log
 * This file should be changed only infrequently and with great care.
 */
#ifndef DIAG_OAM_LOG_H
#define DIAG_OAM_LOG_H

#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define OAM_LOG_LEVEL_MASK              0x07
#define OAM_LOG_MOD_ID_MASK             0xF
#define OAM_LOG_MOD_ID_OFFSET           27
#ifndef OAM_LOG_PARAM_MAX_NUM
#define OAM_LOG_PARAM_MAX_NUM           10
#endif

#define oml_log_comb_mod_id(mod_id, sub_mod, core_id) \
    ((uint16_t)((uint16_t)(mod_id) | (((uint16_t)(sub_mod) & 0x1f) << 8) | ((uint16_t)(core_id) << 13)))

typedef struct {
    uint32_t data0;
} zdiag_log_msg1_t;

typedef struct {
    uint32_t data0;
    uint32_t data1;
} zdiag_log_msg2_t;

typedef struct {
    uint32_t data0;
    uint32_t data1;
    uint32_t data2;
} zdiag_log_msg3_t;

typedef struct {
    uint32_t data0;
    uint32_t data1;
    uint32_t data2;
    uint32_t data3;
} zdiag_log_msg4_t;

errcode_t oam_log_print0_press_prv(uint32_t msg_id, uint32_t mod_id);
errcode_t oam_log_print1_press_prv(uint32_t msg_id, uint32_t mod_id, uint32_t param_1);
errcode_t oam_log_print2_press_prv(uint32_t msg_id, uint32_t mod_id, uint32_t param_1, uint32_t param_2);
errcode_t oam_log_print3_press_prv(uint32_t msg_id, uint32_t mod_id, zdiag_log_msg3_t *olm);
errcode_t oam_log_print4_press_prv(uint32_t msg_id, uint32_t mod_id, zdiag_log_msg4_t *olm);
errcode_t oam_log_print_alterable_press_prv(uint32_t msg_id, uint32_t mod_id, uint32_t param_num, ...);
errcode_t oam_log_print_buff_press_prv(uint32_t msg_id, uint32_t mod_id, uint8_t *data, uint32_t data_size);

/* 无模块ID定参接口 */
errcode_t oam_log_print0_press(uint32_t msg_id);
errcode_t oam_log_print1_press(uint32_t msg_id, uint32_t param_1);
errcode_t oam_log_print2_press(uint32_t msg_id, uint32_t param_1, uint32_t param_2);
errcode_t oam_log_print3_press(uint32_t msg_id, zdiag_log_msg3_t *olm);
errcode_t oam_log_print4_press(uint32_t msg_id, zdiag_log_msg4_t *olm);
errcode_t oam_log_print_alterable_press(uint32_t msg_id, uint32_t param_num, ...);
errcode_t oam_log_print_buff_press(uint32_t msg_id, uint8_t *data, uint32_t data_size);

/* 无模块ID变参接口 */
errcode_t oam_log_optim_no_mod_printx(uint8_t param_num, uint32_t msg_id, ...);
errcode_t oam_log_optim_no_mod_print0(uint32_t msg_id);
errcode_t oam_log_optim_no_mod_print1(uint32_t msg_id, ...);
errcode_t oam_log_optim_no_mod_print2(uint32_t msg_id, ...);
errcode_t oam_log_optim_no_mod_print3(uint32_t msg_id, ...);
errcode_t oam_log_optim_no_mod_print4(uint32_t msg_id, ...);

/* 有子模块定参接口 */
errcode_t oam_log_optim_fix_print0(uint8_t sub_mod_id, uint32_t msg_id);
errcode_t oam_log_optim_fix_print1(uint8_t sub_mod_id, uint32_t msg_id, uint32_t param_1);
errcode_t oam_log_optim_fix_print2(uint8_t sub_mod_id, uint32_t msg_id, uint32_t param_1, uint32_t param_2);
errcode_t oam_log_optim_fix_print3(uint8_t sub_mod_id, uint32_t msg_id, zdiag_log_msg3_t *olm);
errcode_t oam_log_optim_fix_print4(uint8_t sub_mod_id, uint32_t msg_id, zdiag_log_msg4_t *olm);
errcode_t oam_log_optim_fix_print_buff(uint8_t sub_mod_id, uint32_t msg_id, uint8_t *data, uint32_t data_size);

/* 有子模块变参接口 */
errcode_t oam_log_optim_printx(uint8_t sub_mod_id, uint8_t param_num, uint32_t msg_id, ...);
errcode_t oam_log_optim_print0(uint8_t sub_mod_id, uint32_t msg_id);
errcode_t oam_log_optim_print1(uint8_t sub_mod_id, uint32_t msg_id, ...);
errcode_t oam_log_optim_print2(uint8_t sub_mod_id, uint32_t msg_id, ...);
errcode_t oam_log_optim_print3(uint8_t sub_mod_id, uint32_t msg_id, ...);
errcode_t oam_log_optim_print4(uint8_t sub_mod_id, uint32_t msg_id, ...);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* DIAG_OAM_LOG_H */
