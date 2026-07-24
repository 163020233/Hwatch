/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: security_unified init implement file.
 *
 * Create: 2023-03-29
*/

#include "security_init.h"
#include "crypto_drv_common.h"
#include "crypto_drv_init.h"

#define FILE_ID FILE_ID_SERVICE_LAYER_SECURITY_INIT

void __attribute__((weak)) uapi_drv_cipher_env_init(void)
{
    crypto_drv_mod_init();
    drv_cipher_close_all_clk();
}

void uapi_drv_cipher_wait_func_enable_all(void)
{
    crypto_wait_func_config(TD_TRUE);
}

void uapi_drv_cipher_wait_func_disable_all(void)
{
    crypto_wait_func_config(TD_FALSE);
}

void uapi_drv_cipher_env_deinit(void)
{
    crypto_drv_mod_deinit();
}

void uapi_drv_cipher_env_resume(void)
{
    crypto_drv_mod_resume();
}

void uapi_drv_cipher_env_suspend(void)
{
    crypto_drv_mod_suspend();
}