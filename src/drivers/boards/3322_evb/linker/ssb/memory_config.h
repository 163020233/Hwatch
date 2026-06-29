/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description:  Default memory configurations
 * Author: @CompanyNameTag
 * Create:  2023-10
 */
#ifndef MEMORY_CONFIG_H
#define MEMORY_CONFIG_H

#include "memory_config_common.h"
#include "memory_config_flash.h"

/* SSB based application code */
#define SSB_VECTORS_ORIGIN          (APP_ITCM_ORIGIN + FLASH_SIGN_HEADER)
#define SSB_VERSION_ORIGIN          (SSB_VECTORS_ORIGIN + SSB_VECTORS_LENGTH)
#define SSB_CHANGEID_ORIGIN         (SSB_VERSION_ORIGIN + SSB_VERSION_LENGTH)

/*  Keep same for flash and itcm before ITCM_ORIGIN  */
#define SSB_ITCM_ORIGIN             (align_up((SSB_CHANGEID_ORIGIN + SSB_CHANGEID_LENGTH), 64))
#define SSB_ITCM_END                (APP_ITCM_ORIGIN + APP_ITCM_LENGTH)
#define SSB_ITCM_LENGTH             (SSB_ITCM_END - SSB_ITCM_ORIGIN)

#define SSB_BOOT_RAM_ORIGIN         SSB_VECTORS_ORIGIN

/* stack total len 10k */
#define STACK_TOTAL_LEN             (SSB_USER_STACK_LEN + SSB_IRQ_STACK_LEN + SSB_EXCP_STACK_LEN)

/* stack for normal 5k */
#define SSB_USER_STACK_BASEADDR     APP_DTCM_ORIGIN
#define SSB_USER_STACK_LEN          0x1400
#define SSB_USER_STACK_LIMIT        (SSB_USER_STACK_BASEADDR + SSB_USER_STACK_LEN)

/* stack for irq 4k */
#define SSB_IRQ_STACK_BASEADDR      SSB_USER_STACK_LIMIT
#define SSB_IRQ_STACK_LEN           0x1000
#define SSB_IRQ_STACK_LIMIT         (SSB_IRQ_STACK_BASEADDR + SSB_IRQ_STACK_LEN)

/* stack for exception 1k */
#define SSB_EXCP_STACK_BASEADDR     SSB_IRQ_STACK_LIMIT
#define SSB_EXCP_STACK_LEN          0x400
#define SSB_NMI_STACK_LEN           0x400
#define EXCP_STACK_LIMIT            (SSB_EXCP_STACK_BASEADDR + SSB_EXCP_STACK_LEN)
#endif
