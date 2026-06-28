/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: hal keyslot. \n
 */

#include "crypto_errno.h"
#include "crypto_drv_common.h"
#if defined(CRYPTO_ERROR_ENV) && (CRYPTO_ERROR_ENV == ERROR_ENV_SELITEOS)
#include "los_printf.h"
#endif
#include "hal_klad_reg.h"
#include "hal_rkp_reg.h"
#include "hal_keyslot_romable.h"

#if defined(CRYPTO_ERROR_ENV) && (CRYPTO_ERROR_ENV == ERROR_ENV_LITEOS)
void hal_keyslot_debug_default(void)
{
    printf("KL_KEY_SEC_CFG is 0x%x\r\n", km_reg_read(KL_KEY_SEC_CFG));
    printf("KL_KEY_ADDR is 0x%x\r\n", km_reg_read(KL_KEY_ADDR));
    printf("KL_KEY_CFG is 0x%x\r\n", km_reg_read(KL_KEY_CFG));

    printf("KL_COM_LOCK_INFO is 0x%x\r\n", km_reg_read(KL_COM_LOCK_INFO));
    printf("KL_COM_LOCK_STATUS is 0x%x\r\n", km_reg_read(KL_COM_LOCK_STATUS));
    printf("RKP_LOCK is 0x%x\r\n", km_reg_read(RKP_LOCK));
    printf("RKP_CMD_CFG is 0x%x\r\n", km_reg_read(RKP_CMD_CFG));
    printf("KL_COM_CTRL is 0x%x\r\n", km_reg_read(KL_COM_CTRL));
    printf("RKP_PBKDF2_RSLT_CRC is 0x%x\r\n", km_reg_read(RKP_PBKDF2_RSLT_CRC));
    printf("KL_CRC val is 0x%x\r\n", km_reg_read(KL_CRC));

    printf("KL_CLR_CTRL is 0x%x\r\n", km_reg_read(KL_CLR_CTRL));
    printf("KL_INT_RAW is 0x%x\r\n", km_reg_read(KL_INT_RAW));
    printf("KL_ERROR is 0x%x\r\n", km_reg_read(KL_ERROR));
    printf("KC_ERROR is 0x%x\r\n", km_reg_read(KC_ERROR));
    printf("KDF_ERROR is 0x%x\r\n", km_reg_read(KDF_ERROR));
}
#elif defined(CRYPTO_ERROR_ENV) && (CRYPTO_ERROR_ENV == ERROR_ENV_SELITEOS)
void hal_keyslot_debug_default(void)
{
    osal_printk("KL_KEY_SEC_CFG is 0x%x\r\n", km_reg_read(KL_KEY_SEC_CFG));
    osal_printk("KL_KEY_ADDR is 0x%x\r\n", km_reg_read(KL_KEY_ADDR));
    osal_printk("KL_KEY_CFG is 0x%x\r\n", km_reg_read(KL_KEY_CFG));

    osal_printk("KL_COM_LOCK_INFO is 0x%x\r\n", km_reg_read(KL_COM_LOCK_INFO));
    osal_printk("KL_COM_LOCK_STATUS is 0x%x\r\n", km_reg_read(KL_COM_LOCK_STATUS));
    osal_printk("RKP_LOCK is 0x%x\r\n", km_reg_read(RKP_LOCK));
    osal_printk("RKP_CMD_CFG is 0x%x\r\n", km_reg_read(RKP_CMD_CFG));
    osal_printk("KL_COM_CTRL is 0x%x\r\n", km_reg_read(KL_COM_CTRL));
    osal_printk("RKP_PBKDF2_RSLT_CRC is 0x%x\r\n", km_reg_read(RKP_PBKDF2_RSLT_CRC));

    osal_printk("KL_CLR_CTRL is 0x%x\r\n", km_reg_read(KL_CLR_CTRL));
    osal_printk("KL_INT_RAW is 0x%x\r\n", km_reg_read(KL_INT_RAW));
    osal_printk("KL_ERROR is 0x%x\r\n", km_reg_read(KL_ERROR));
    osal_printk("KC_ERROR is 0x%x\r\n", km_reg_read(KC_ERROR));
    osal_printk("KDF_ERROR is 0x%x\r\n", km_reg_read(KDF_ERROR));
}
#endif
