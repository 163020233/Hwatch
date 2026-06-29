/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: diag oam log
 * This file should be changed only infrequently and with great care.
 */
#include "diag_oam_log.h"
#include <stdarg.h>
#include "soc_diag_log.h"
#include "diag.h"
#include "diag_ind_src.h"
#include "dfx_feature_config.h"
#include "chip_core_definition.h"

#ifndef CORE_ID
#define CORE_ID CORE
#endif

#ifndef CONFIG_DFX_DIAG_LOG_USE_BUFFER
/* 为兼容旧项目，使用CONFIG_LOG_USE_DIAG_FRAME宏代替CONFIG_DFX_DIAG_LOG_USE_BUFFER宏 */
#ifdef CONFIG_LOG_USE_DIAG_FRAME
#define CONFIG_DFX_DIAG_LOG_USE_BUFFER              DFX_YES
#else
#define CONFIG_DFX_DIAG_LOG_USE_BUFFER              DFX_NO
#endif /* CONFIG_LOG_USE_DIAG_FRAME */
#endif

STATIC errcode_t oam_log_msg_print(uint32_t msg_id, uint32_t mod_id, uint8_t *buf, uint32_t size)
{
    uint32_t level = msg_id & OAM_LOG_LEVEL_MASK;
    /* does't check if the buffer is null. because when param count is 0,the buffer is null. */
#if (CONFIG_DFX_DIAG_LOG_USE_BUFFER == DFX_YES)
    return uapi_diag_report_sys_msg_to_buffer(mod_id, msg_id, buf, (uint16_t)size, (uint8_t)level);
#else
    return uapi_diag_report_sys_msg(mod_id, msg_id, buf, (uint16_t)size, (uint8_t)level);
#endif
}

errcode_t oam_log_print0_press_prv(uint32_t msg_id, uint32_t mod_id)
{
    return oam_log_msg_print(msg_id, mod_id, NULL, 0);
}

errcode_t oam_log_print1_press_prv(uint32_t msg_id, uint32_t mod_id, uint32_t param_1)
{
    zdiag_log_msg1_t msg;
    msg.data0 = param_1;
    return oam_log_msg_print(msg_id, mod_id, (uint8_t *)&msg, sizeof(zdiag_log_msg1_t));
}

errcode_t oam_log_print2_press_prv(uint32_t msg_id, uint32_t mod_id, uint32_t param_1, uint32_t param_2)
{
    zdiag_log_msg2_t msg;
    msg.data0 = param_1;
    msg.data1 = param_2;
    return oam_log_msg_print(msg_id, mod_id, (uint8_t *)&msg, sizeof(zdiag_log_msg2_t));
}

errcode_t oam_log_print3_press_prv(uint32_t msg_id, uint32_t mod_id, zdiag_log_msg3_t *olm)
{
    zdiag_log_msg3_t msg;
    msg.data0 = olm->data0;
    msg.data1 = olm->data1;
    msg.data2 = olm->data2;
    return oam_log_msg_print(msg_id, mod_id, (uint8_t *)&msg, sizeof(zdiag_log_msg3_t));
}

errcode_t oam_log_print4_press_prv(uint32_t msg_id, uint32_t mod_id, zdiag_log_msg4_t *olm)
{
    zdiag_log_msg4_t msg;
    msg.data0 = olm->data0;
    msg.data1 = olm->data1;
    msg.data2 = olm->data2;
    msg.data3 = olm->data3;
    return oam_log_msg_print(msg_id, mod_id, (uint8_t *)&msg, sizeof(zdiag_log_msg4_t));
}

errcode_t oam_log_print_alterable_press_prv(uint32_t msg_id, uint32_t mod_id, uint32_t param_num, ...)
{
    uint32_t log_oam_entry[OAM_LOG_PARAM_MAX_NUM];
    va_list args;
    uint32_t param_count;
    uint32_t i;

    param_count = ((param_num > OAM_LOG_PARAM_MAX_NUM) ? (OAM_LOG_PARAM_MAX_NUM) : param_num);
    va_start(args, param_num);
    for (i = 0; i < param_count; i++) {
        log_oam_entry[i] = (uint32_t)va_arg(args, uint32_t);
    }
    va_end(args);

    return oam_log_msg_print(msg_id, mod_id, (uint8_t *)log_oam_entry, param_count * sizeof(uint32_t));
}

errcode_t oam_log_print_buff_press_prv(uint32_t msg_id, uint32_t mod_id, uint8_t *data, uint32_t data_size)
{
    return oam_log_msg_print(msg_id, mod_id, data, data_size);
}

/* 以下是为优化Codesize使用的函数 */
/*
 * 1、去掉mod_id参数，在不使用子模块的情况下，可使用这些函数。
 */
errcode_t oam_log_print0_press(uint32_t msg_id)
{
    uint32_t mod_id = (uint32_t)((msg_id >> OAM_LOG_MOD_ID_OFFSET) & OAM_LOG_MOD_ID_MASK);
    return oam_log_msg_print(msg_id, mod_id, NULL, 0);
}

errcode_t oam_log_print1_press(uint32_t msg_id, uint32_t param_1)
{
    zdiag_log_msg1_t msg;
    uint32_t mod_id = (uint32_t)((msg_id >> OAM_LOG_MOD_ID_OFFSET) & OAM_LOG_MOD_ID_MASK);
    msg.data0 = param_1;
    return oam_log_msg_print(msg_id, mod_id, (uint8_t *)&msg, sizeof(zdiag_log_msg1_t));
}

errcode_t oam_log_print2_press(uint32_t msg_id, uint32_t param_1, uint32_t param_2)
{
    zdiag_log_msg2_t msg;
    uint32_t mod_id = (uint32_t)((msg_id >> OAM_LOG_MOD_ID_OFFSET) & OAM_LOG_MOD_ID_MASK);
    msg.data0 = param_1;
    msg.data1 = param_2;
    return oam_log_msg_print(msg_id, mod_id, (uint8_t *)&msg, sizeof(zdiag_log_msg2_t));
}

errcode_t oam_log_print3_press(uint32_t msg_id, zdiag_log_msg3_t *olm)
{
    zdiag_log_msg3_t msg;
    uint32_t mod_id = (uint32_t)((msg_id >> OAM_LOG_MOD_ID_OFFSET) & OAM_LOG_MOD_ID_MASK);
    msg.data0 = olm->data0;
    msg.data1 = olm->data1;
    msg.data2 = olm->data2;
    return oam_log_msg_print(msg_id, mod_id, (uint8_t *)&msg, sizeof(zdiag_log_msg3_t));
}

errcode_t oam_log_print4_press(uint32_t msg_id, zdiag_log_msg4_t *olm)
{
    zdiag_log_msg4_t msg;
    uint32_t mod_id = (uint32_t)((msg_id >> OAM_LOG_MOD_ID_OFFSET) & OAM_LOG_MOD_ID_MASK);
    msg.data0 = olm->data0;
    msg.data1 = olm->data1;
    msg.data2 = olm->data2;
    msg.data3 = olm->data3;
    return oam_log_msg_print(msg_id, mod_id, (uint8_t *)&msg, sizeof(zdiag_log_msg4_t));
}

errcode_t oam_log_print_alterable_press(uint32_t msg_id, uint32_t param_num, ...)
{
    uint32_t mod_id = (uint32_t)((msg_id >> OAM_LOG_MOD_ID_OFFSET) & OAM_LOG_MOD_ID_MASK);
    uint32_t log_oam_entry[OAM_LOG_PARAM_MAX_NUM];
    va_list args;
    uint32_t i;
    uint8_t *buf_ptr;

    uint32_t param_count = ((param_num > OAM_LOG_PARAM_MAX_NUM) ? (OAM_LOG_PARAM_MAX_NUM) : param_num);

    va_start(args, param_num);
    for (i = 0; i < param_count; i++) {
        log_oam_entry[i] = (uint32_t)va_arg(args, uint32_t);
    }
    va_end(args);

    if (param_count > 0) {
        buf_ptr = (uint8_t *)log_oam_entry;
    } else {
        buf_ptr = NULL;
    }

    return oam_log_msg_print(msg_id, mod_id, buf_ptr, param_count * sizeof(uint32_t));
}

errcode_t oam_log_print_buff_press(uint32_t msg_id, uint8_t *data, uint32_t data_size)
{
    uint32_t mod_id = (uint32_t)((msg_id >> OAM_LOG_MOD_ID_OFFSET) & OAM_LOG_MOD_ID_MASK);
    return oam_log_msg_print(msg_id, mod_id, data, data_size);
}

STATIC errcode_t oam_log_optim_print_va(uint8_t sub_mod_id, uint8_t param_num, uint32_t msg_id, va_list args)
{
    uint32_t log_oam_entry[OAM_LOG_PARAM_MAX_NUM];
    uint8_t i;
    uint8_t mod_id = (uint8_t)((msg_id >> OAM_LOG_MOD_ID_OFFSET) & OAM_LOG_MOD_ID_MASK);
    uint32_t module_id = (uint32_t)oml_log_comb_mod_id(mod_id, sub_mod_id, CORE_ID);
    uint8_t param_count = ((param_num > OAM_LOG_PARAM_MAX_NUM) ? (OAM_LOG_PARAM_MAX_NUM) : param_num);

    for (i = 0; i < param_count; i++) {
        log_oam_entry[i] = (uint32_t)va_arg(args, uint32_t);
    }
    return oam_log_msg_print(msg_id, module_id, (uint8_t *)log_oam_entry, param_count * sizeof(uint32_t));
}

/* 无mod_id参数的变参接口 */
errcode_t oam_log_optim_no_mod_printx(uint8_t param_num, uint32_t msg_id, ...)
{
    errcode_t ret;
    va_list args;

    va_start(args, msg_id);
    ret = oam_log_optim_print_va(0, param_num, msg_id, args);
    va_end(args);
    return ret;
}

errcode_t oam_log_optim_no_mod_print0(uint32_t msg_id)
{
    uint8_t mod_id = (uint8_t)((msg_id >> OAM_LOG_MOD_ID_OFFSET) & OAM_LOG_MOD_ID_MASK);
    uint32_t module_id = (uint32_t)oml_log_comb_mod_id(mod_id, 0, CORE_ID);
    return oam_log_msg_print(msg_id, module_id, NULL, 0);
}

errcode_t oam_log_optim_no_mod_print1(uint32_t msg_id, ...)
{
    errcode_t ret;
    va_list args;

    va_start(args, msg_id);
    ret = oam_log_optim_print_va(0, 1, msg_id, args);
    va_end(args);
    return ret;
}

errcode_t oam_log_optim_no_mod_print2(uint32_t msg_id, ...)
{
    errcode_t ret;
    va_list args;

    va_start(args, msg_id);
    ret = oam_log_optim_print_va(0, 2, msg_id, args);  /* 2 parameters */
    va_end(args);
    return ret;
}

errcode_t oam_log_optim_no_mod_print3(uint32_t msg_id, ...)
{
    errcode_t ret;
    va_list args;

    va_start(args, msg_id);
    ret = oam_log_optim_print_va(0, 3, msg_id, args); /* 3 parameters */
    va_end(args);
    return ret;
}

errcode_t oam_log_optim_no_mod_print4(uint32_t msg_id, ...)
{
    errcode_t ret;
    va_list args;

    va_start(args, msg_id);
    ret = oam_log_optim_print_va(0, 4, msg_id, args); /* 4 parameters */
    va_end(args);
    return ret;
}

/*
 * 2、mod_id使用uint8_t类型，用于表示子模块
 */

/* 有子模块的定参接口 */
errcode_t oam_log_optim_fix_print0(uint8_t sub_mod_id, uint32_t msg_id)
{
    uint8_t mod_id = (uint8_t)((msg_id >> OAM_LOG_MOD_ID_OFFSET) & OAM_LOG_MOD_ID_MASK);
    uint32_t module_id = (uint32_t)oml_log_comb_mod_id(mod_id, sub_mod_id, CORE_ID);
    return oam_log_msg_print(msg_id, module_id, NULL, 0);
}

errcode_t oam_log_optim_fix_print1(uint8_t sub_mod_id, uint32_t msg_id, uint32_t param_1)
{
    zdiag_log_msg1_t msg;
    uint8_t mod_id = (uint8_t)((msg_id >> OAM_LOG_MOD_ID_OFFSET) & OAM_LOG_MOD_ID_MASK);
    uint32_t module_id = (uint32_t)oml_log_comb_mod_id(mod_id, sub_mod_id, CORE_ID);
    msg.data0 = param_1;
    return oam_log_msg_print(msg_id, module_id, (uint8_t *)&msg, sizeof(zdiag_log_msg1_t));
}

errcode_t oam_log_optim_fix_print2(uint8_t sub_mod_id, uint32_t msg_id, uint32_t param_1, uint32_t param_2)
{
    zdiag_log_msg2_t msg;
    uint8_t mod_id = (uint8_t)((msg_id >> OAM_LOG_MOD_ID_OFFSET) & OAM_LOG_MOD_ID_MASK);
    uint32_t module_id = (uint32_t)oml_log_comb_mod_id(mod_id, sub_mod_id, CORE_ID);
    msg.data0 = param_1;
    msg.data1 = param_2;
    return oam_log_msg_print(msg_id, module_id, (uint8_t *)&msg, sizeof(zdiag_log_msg2_t));
}

errcode_t oam_log_optim_fix_print3(uint8_t sub_mod_id, uint32_t msg_id, zdiag_log_msg3_t *olm)
{
    uint8_t mod_id = (uint8_t)((msg_id >> OAM_LOG_MOD_ID_OFFSET) & OAM_LOG_MOD_ID_MASK);
    uint32_t module_id = (uint32_t)oml_log_comb_mod_id(mod_id, sub_mod_id, CORE_ID);
    return oam_log_msg_print(msg_id, module_id, (uint8_t *)olm, sizeof(zdiag_log_msg3_t));
}

errcode_t oam_log_optim_fix_print4(uint8_t sub_mod_id, uint32_t msg_id, zdiag_log_msg4_t *olm)
{
    uint8_t mod_id = (uint8_t)((msg_id >> OAM_LOG_MOD_ID_OFFSET) & OAM_LOG_MOD_ID_MASK);
    uint32_t module_id = (uint32_t)oml_log_comb_mod_id(mod_id, sub_mod_id, CORE_ID);
    return oam_log_msg_print(msg_id, module_id, (uint8_t *)olm, sizeof(zdiag_log_msg4_t));
}

errcode_t oam_log_optim_fix_print_buff(uint8_t sub_mod_id, uint32_t msg_id, uint8_t *data, uint32_t data_size)
{
    uint8_t mod_id = (uint8_t)((msg_id >> OAM_LOG_MOD_ID_OFFSET) & OAM_LOG_MOD_ID_MASK);
    uint32_t module_id = (uint32_t)oml_log_comb_mod_id(mod_id, sub_mod_id, CORE_ID);
    return oam_log_msg_print(msg_id, module_id, data, data_size);
}

/* 有子模块的变参接口 */
errcode_t oam_log_optim_printx(uint8_t sub_mod_id, uint8_t param_num, uint32_t msg_id, ...)
{
    errcode_t ret;
    va_list args;

    va_start(args, msg_id);
    ret = oam_log_optim_print_va(sub_mod_id, param_num, msg_id, args);
    va_end(args);
    return ret;
}

errcode_t oam_log_optim_print0(uint8_t sub_mod_id, uint32_t msg_id)
{
    uint8_t mod_id = (uint8_t)((msg_id >> OAM_LOG_MOD_ID_OFFSET) & OAM_LOG_MOD_ID_MASK);
    uint32_t module_id = (uint32_t)oml_log_comb_mod_id(mod_id, sub_mod_id, CORE_ID);
    return oam_log_msg_print(msg_id, module_id, NULL, 0);
}

errcode_t oam_log_optim_print1(uint8_t sub_mod_id, uint32_t msg_id, ...)
{
    errcode_t ret;
    va_list args;

    va_start(args, msg_id);
    ret = oam_log_optim_print_va(sub_mod_id, 1, msg_id, args);
    va_end(args);
    return ret;
}

errcode_t oam_log_optim_print2(uint8_t sub_mod_id, uint32_t msg_id, ...)
{
    errcode_t ret;
    va_list args;

    va_start(args, msg_id);
    ret = oam_log_optim_print_va(sub_mod_id, 2, msg_id, args); /* 2 parameters */
    va_end(args);
    return ret;
}

errcode_t oam_log_optim_print3(uint8_t sub_mod_id, uint32_t msg_id, ...)
{
    errcode_t ret;
    va_list args;

    va_start(args, msg_id);
    ret = oam_log_optim_print_va(sub_mod_id, 3, msg_id, args); /* 3 parameters */
    va_end(args);
    return ret;
}

errcode_t oam_log_optim_print4(uint8_t sub_mod_id, uint32_t msg_id, ...)
{
    errcode_t ret;
    va_list args;

    va_start(args, msg_id);
    ret = oam_log_optim_print_va(sub_mod_id, 4, msg_id, args); /* 4 parameters */
    va_end(args);
    return ret;
}

