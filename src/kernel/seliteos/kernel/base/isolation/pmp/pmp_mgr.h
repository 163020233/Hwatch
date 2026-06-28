/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: pmp manager header for SELiteOS.
 */
#ifndef PMP_MGR_H
#define PMP_MGR_H

#include "los_base.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum {
    NORMAL_TYPE,
    REMOTE_CALL_TYPE,
    SWITCH_TYPE_MAX
} switch_to_ree_type_t;

void set_pmp_switch_ree_to_tee(switch_to_ree_type_t type);

void set_pmp_switch_tee_to_ree(switch_to_ree_type_t type);

#ifdef LOSCFG_SUPPORT_TA
void set_pmp_switch_kernel_to_usrbox(uint32_t usrbox_text_rodata_addr, uint32_t usrbox_text_rodata_size,
                                     uint32_t usrbox_data_addr, uint32_t usrbox_data_size);

void set_pmp_switch_usrbox_to_kernel(uint32_t usrbox_text_rodata_addr, uint32_t usrbox_text_rodata_size,
                                     uint32_t usrbox_data_addr, uint32_t usrbox_data_size);

void set_pmp_switch_usrbox_to_drvbox(uint32_t usrbox_text_rodata_addr, uint32_t usrbox_text_rodata_size);

void set_pmp_switch_drvbox_to_usrbox(uint32_t usrbox_text_rodata_addr, uint32_t usrbox_text_rodata_size);
#endif

uint32_t check_is_in_ree_alloc_shared_data(uintptr_t data_addr, uint32_t len);

uint32_t check_is_not_in_tee_range(uintptr_t data_addr, uint32_t len);

LITE_OS_SEC_TEXT_USRSPACE uint32_t check_data_range_valid(uintptr_t data_addr, uint32_t len);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PMP_MGR_H */
