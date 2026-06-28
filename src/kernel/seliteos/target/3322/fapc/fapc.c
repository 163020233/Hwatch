/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 * Description: fapc source for SELiteOS.
 */
#include "chip_io.h"
#include "securec.h"
#include "hal_fapc.h"
#include "security_km.h"
#include "los_printf.h"
#include "fapc.h"

static fapc_image_info_t g_tee_image_info;

static const fapc_config_t g_fapc_config[] = {
    {
        FLASH_START,
        FLASH_START + FLASH_LENGTH,
        FAPC_TEE_MID,
        FAPC_TEE_MID,
    },
    {
        FLASH_START,
        SSB_FLASH_REGION_START,
        FAPC_TEE_MID | FAPC_REE_MID,
        FAPC_TEE_MID | FAPC_REE_MID
    },
    {
        RECOVERY_FLASH_REGION_START,
        (FLASH_START + FLASH_LENGTH),
        FAPC_TEE_MID | FAPC_REE_MID,
        FAPC_TEE_MID | FAPC_REE_MID,
    },
    {
        SELITEOS_FLASH_REGION_START,
        (SELITEOS_FLASH_REGION_START + SELITEOS_FLASH_REGION_LENGTH),
        FAPC_TEE_MID,
        FAPC_TEE_MID,
    },
};

static const fapc_lut0_config_t g_fapc_lut0_config[] = {
    { FAPC_TEE_MID, ERASE_CHIP_CMD } /* erase all chip */
};

static const fapc_lut1_config_t g_fapc_lut1_config[] = {
    { 16, ERASE_64K_CMD }, /* erase 16 blocks, block size is 4K */
    { 8, ERASE_32K_CMD }, /* erase 8 blocks, block size is 4K */
    { 1, ERASE_4K_CMD } /* erase 1 block, block size is 4K */
};

void tee_fapc_get_image_info(void)
{
    uint8_t *src = (uint8_t *)(uintptr_t)ENC_FLAG_ITCM_ADDR;
    uint8_t *dst = (uint8_t *)(uintptr_t)&g_tee_image_info;
    memcpy_s(dst, sizeof(fapc_image_info_t), src, sizeof(fapc_image_info_t));
}

void tee_fapc_init(void)
{
    uint32_t i;

    for (i = 0 ; i < sizeof(g_fapc_config) / sizeof(fapc_config_t); i++) {
        hal_fapc_set_region_addr(i, g_fapc_config[i].start_addr, g_fapc_config[i].end_addr);
        hal_fapc_set_region_permission(i, g_fapc_config[i].read_mid, g_fapc_config[i].write_mid);
        hal_fapc_set_region_mode(i, CRYPTO_FAPC_REGION_MODE_NORMAL);
        hal_fapc_region_enable(i, 1);
        hal_fapc_region_lock(i);
    }

    // REE cannot do CHIP_ERASE operation
    for (i = 0; i < sizeof(g_fapc_lut0_config) / sizeof(fapc_lut0_config_t); i++) {
        hal_fapc_set_cmd_lut0(i, g_fapc_lut0_config[i].allow_mid, g_fapc_lut0_config[i].instr);
        hal_fapc_set_cmd_lut0_enable(i, true);
    }

    // REE cannot do BLOCK_ERASE operation
    for (i = 0; i < sizeof(g_fapc_lut1_config) / sizeof(fapc_lut1_config_t); i++) {
        hal_fapc_set_cmd_lut1(i, g_fapc_lut1_config[i].block_cnt * BLOCK_SIZE - 1, g_fapc_lut1_config[i].instr, true);
        hal_fapc_set_cmd_lut1_enable(i, true);
    }

    hal_fapc_set_cmd_lut_lock();
}

static void tee_fapc_send_key(uint8_t *salt, uint32_t salt_len)
{
    uint32_t keyslot_handle;

    (void)uapi_drv_keyslot_setup(&keyslot_handle, UAPI_DRV_KEYSLOT_TYPE_FLASH);
    (void)uapi_drv_keyslot_set_hard_key(keyslot_handle, UAPI_DRV_KEYSLOT_HARD_KEY_TYPE_ODRK_TEE,
        UAPI_DRV_KLAD_ENGINE_AES, salt, salt_len, KEY_IV_LEN);
    (void)uapi_drv_keyslot_teardown(keyslot_handle);
}

void tee_flash_online_decryption_config(void)
{
    uint32_t region = SELITEOS_FAPC_REGION_NUM;
    uint32_t start_addr = SELITEOS_PROGRAM_ORIGIN;
    uint32_t end_addr = SELITEOS_FLASH_REGION_START + SELITEOS_FLASH_REGION_LENGTH;
    uint32_t read_mid = FAPC_TEE_MID;
    uint32_t write_mid = 0;

    if (g_tee_image_info.code_enc_flag != NON_ENC_FLAG) {
        /* send decryption key */
        tee_fapc_send_key(g_tee_image_info.salt, sizeof(g_tee_image_info.salt));

        /* set fapc to enable flash online decryption */
        hal_fapc_set_region_addr(region, start_addr, end_addr);
        hal_fapc_set_region_permission(region, read_mid, write_mid);
        hal_fapc_set_region_mode(region, CRYPTO_FAPC_REGION_MODE_DEC);
        hal_fapc_set_region_iv(CRYPTO_FAPC_IV_TYPE_TEE, g_tee_image_info.iv, sizeof(g_tee_image_info.iv));
        hal_fapc_set_region_iv_start_addr(region, start_addr);
        hal_fapc_region_enable(region, 1);
        hal_fapc_region_lock(region);
    }
}
