/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 * Description: 3322 porting init API for SELiteOS.
 */

#include "security_init.h"
#include "rapc.h"
#include "fapc.h"
#include "pmp_mgr.h"
#include "pmp_cfg.h"
#include "uart_porting.h"
#include "tcxo.h"
#ifdef CONFIG_TEE_EFUSE_SUPPORT
#include "tee_efuse.h"
#endif
#ifdef PSA_ATTESTATION_SUPPORT
#include "attes_common.h"
#endif

void tee_pmp_init(void)
{
    set_pmp_usr_default();
    set_pmp_kernel_default();
}

void tee_rapc_init(void)
{
    rapc_set_tee_ctrl(true);
#ifdef TZPC_PROTECT_CONFIG
    tzpc_set_tee_ctrl(true);
#endif
}

void tee_driver_and_service_init(void)
{
    uapi_tcxo_init();

    /* init uart driver */
    sw_debug_uart_init(SW_UART_BAUDRATE);

    LOS_Printf("Welcome to SEliteOS!\r\n");

    /* init cipher driver */
    uapi_drv_cipher_env_init();

#ifdef CONFIG_TEE_EFUSE_SUPPORT
    /* init efuse driver */
    tee_efuse_init();
#endif
#ifdef PSA_ATTESTATION_SUPPORT
    /* init psa attestation bootseed */
    psa_attes_get_bootseed();
#endif
}

void tee_ready_to_ree(void)
{
    tee_fapc_get_image_info();
    set_pmp_switch_tee_to_ree(NORMAL_TYPE);
}
