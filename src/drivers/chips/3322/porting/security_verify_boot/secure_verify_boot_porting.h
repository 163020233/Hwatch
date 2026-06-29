/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Secure Verify for image used in boot process.
 */

#ifndef SECURE_VERIFY_BOOT_PORTING_H
#define SECURE_VERIFY_BOOT_PORTING_H

#include "errcode.h"
#include "secure_verify_boot.h"

#define REE_PUBLIC_KEY_IMAGE_ID             0x4BA5C31E
#define TEE_PUBLIC_KEY_IMAGE_ID             0x4B96B41E
#define PROVISION_KEY_AREA_IMAGE_ID         0x4BD2F01E
#define PROVISION_CODE_INFO_IMAGE_ID        0x4BD2F02D
#define PARAMS_KEY_AREA_IMAGE_ID            0x4B87A51E
#define PARAMS_AREA_INFO_IMAGE_ID           0x4B87A52D
#define PARAMS_AREA_IMAGE_ID                0x4B87A54B
#define REE_BOOT_KEY_AREA_IMAGE_ID          0x4B1E3C1E
#define REE_BOOT_CODE_INFO_IMAGE_ID         0x4B1E3C2D
#define RECOVERY_KEY_AREA_IMAGE_ID          0x4B69871E
#define RECOVERY_CODE_INFO_IMAGE_ID         0x4B69872D
#define REE_APP_KEY_AREA_IMAGE_ID           0x4B0F2D1E
#define REE_APP_CODE_INFO_IMAGE_ID          0x4B0F2D2D
#define CHIP_FEATURE_CONFIG_TABLE_KEY_AREA_IMAGE_ID   0x4B78961E
#define CHIP_FEATURE_CONFIG_TABLE_IMAGE_ID  0x4B78962D
#define TEE_BOOT_KEY_AREA_IMAGE_ID          0x4BB4D21E
#define TEE_BOOT_CODE_INFO_IMAGE_ID         0x4BB4D22D
#define TEE_RUN_TIME_FW_KEY_AREA_IMAGE_ID   0x4BE10F1E
#define TEE_RUN_TIME_FW_AREA_INFO_IMAGE_ID  0x4BE10F2D
#define BT_KEY_AREA_IMAGE_ID                0x4BF01E1E
#define BT_CODE_INFO_IMAGE_ID               0x4BF01E2D
#define DSP_KEY_AREA_IMAGE_ID               0x5A87A51E
#define DSP_CODE_INFO_IMAGE_ID              0x5A87A52D
#define DSP1_KEY_AREA_IMAGE_ID              0x5A96B41E
#define DSP1_CODE_INFO_IMAGE_ID             0x5A96B42D

errcode_t get_verify_info(verify_table_item_t **out_table, uint32_t *out_table_len,
    secure_authenticate_func *authenticate_func, rom_hash_func *hash_func, rom_efuse_func *efuse_func);

errcode_t secure_verify_authenticate(const uint8_t *key, const uint8_t *data, uint32_t data_length,
    const uint8_t *sign_buff, uint32_t sign_length);

void secure_verify_init(void);

errcode_t secure_verify_image(image_type_t image_type, uint32_t key_area_addr, uint32_t verify_public_key_addr);

errcode_t secure_verify_get_board_version(uint32_t key_area_addr, uint32_t *key_ver, uint32_t *code_ver);

uint32_t get_tee_version(void);

#endif