/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: Secure Verify for Loaderboot and Flashboot
 *
 * Create: 2023-03-09
 */

#include "securec.h"
#include "boot_errcode.h"
#include "secure_verify_boot_porting.h"
#include "secure_verify_boot_check.h"
#include "secure_verify_boot.h"

#define MAINTENANCE_MODE_EN          0x2A13C812

#ifdef CONFIG_BOOT_SUPPORT_CFCT_IMAGE
#define SINGLE_AUTHORIZATION 0x3C7896E1

#define ROM_ROOT_PUBLIC_KEY_SEL_ZERO   0b0000
#define ROM_ROOT_PUBLIC_KEY_SEL_ONE   0b0001
#define ROM_ROOT_PUBLIC_KEY_SEL_TWO   0b0011
#define ROM_ROOT_PUBLIC_KEY_SEL_THREE   0b0111

errcode_t verify_cfct_key_area(cfct_key_area_t *key_area)
{
    errcode_t ret = ERRCODE_FAIL;
    uint8_t key_sel = 0;
    uint8_t key_n = 0;
    uint32_t rom_boot_key_addr = 0;

    ret = check_image_id(ROM_CFCT_BOOT_TYPE, CFCT_KEY_AREA_TYPE, key_area->image_id);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    ret = efuse_read_item(EFUSE_ROM_BOOT_PUB_KEY_SEL_ID, &key_sel, (uint16_t)sizeof(key_sel));
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    if (key_sel == 0) {
        key_sel = key_area->rom_boot_pub_key_sel_ext;
    }
    switch (key_sel) {
        case ROM_ROOT_PUBLIC_KEY_SEL_ZERO :
            key_n = 0; // key_sel 0
            break;
        case ROM_ROOT_PUBLIC_KEY_SEL_ONE :
            key_n = 1; // key_sel 1
            break;
        case ROM_ROOT_PUBLIC_KEY_SEL_TWO :
            key_n = 2; // key_sel 2
            break;
        case ROM_ROOT_PUBLIC_KEY_SEL_THREE :
            key_n = 3; // key_sel 3
            break;
        default :
            return ERRCODE_INVALID_PARAM;
    }
    rom_boot_key_addr = cfct_get_rom_boot_key_addr(key_n);
    ret = secure_authenticate((const uint8_t *)(uintptr_t)rom_boot_key_addr, (uint8_t *)key_area,
        KEY_AREA_STRUCTURE_LENGTH - SIG_LEN, key_area->sig_key_area, PUBLIC_KEY_LEN);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    ret = check_version(ROM_CFCT_BOOT_TYPE, key_area->cfct_key_version_ext, key_area->mask_cfct_key_version_ext);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    return ERRCODE_SUCC;
}

errcode_t verify_cfct_table_area(cfct_table_info_t *table_info, const uint8_t *public_key)
{
    errcode_t ret = ERRCODE_FAIL;
    uint8_t global_disable = 0;

    ret = check_image_id(ROM_CFCT_BOOT_TYPE, CFCT_TABLE_TYPE, table_info->image_id);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    if (table_info->authorization_mode == SINGLE_AUTHORIZATION) {
        ret = check_die_id((uint8_t *)table_info->auth_id, DIE_ID_LEN);
        if (ret != ERRCODE_SUCC) {
            return ret;
        }
        goto cfct_table_authenticate;
    }

    ret = efuse_read_item(EFUSE_CHIP_FEATURES_GLOBAL_DISABLE_ID, &global_disable, (uint16_t)sizeof(global_disable));
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    if (global_disable != 0) {
        return ERRCODE_NOT_SUPPORT;
    }

cfct_table_authenticate:
    if (table_info->data_length > UINT32_MAX - SIG_LEN - CFCT_TABLE_INFO_LEN) {
        return ERRCODE_BOOT_VERIFY_INVALID_CFCT_TABLE_LENGTH;
    }
    if (table_info->structure_length != SIG_LEN + table_info->data_length + CFCT_TABLE_INFO_LEN) {
        return ERRCODE_BOOT_VERIFY_INVALID_CFCT_TABLE_LENGTH;
    }

    ret = secure_authenticate(public_key, (uint8_t *)table_info, table_info->structure_length - SIG_LEN,
        (uint8_t *)table_info + table_info->structure_length - SIG_LEN, SIG_LEN);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    return ERRCODE_SUCC;
}
#endif

errcode_t verify_image_key_area(image_type_t image_type,
    const image_key_area_t *key_area, const uint8_t *public_key)
{
    errcode_t ret = ERRCODE_FAIL;
    uint32_t image_owner = get_image_owner((uint32_t)image_type);

    /* check verify enable */
    ret = check_verify_enable(image_owner);
    if (ret == ERRCODE_NOT_SUPPORT) {
        return ERRCODE_SUCC;
    } else if (ret == ERRCODE_FAIL) {
        return ERRCODE_BOOT_VERIFY_CHECK_ENABLE_FAIL;
    }

    ret = check_rootkey_status(image_type);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    /* check image id */
    ret = check_image_id(image_type, KEY_EREA_TYPE, key_area->image_id);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    /* verify key area with rootkey */
    ret = secure_authenticate(public_key, (uint8_t *)key_area, KEY_AREA_STRUCTURE_LENGTH - SIG_LEN,
        key_area->sig_key_area, PUBLIC_KEY_LEN);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    /* check key version ext */
    ret = check_version(image_type, key_area->key_version_ext, key_area->mask_key_version_ext);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    /* check key msid */
    ret = check_msid(key_area->msid_ext, key_area->mask_msid_ext);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    /* check maintention mode */
    if (key_area->maintenance_mode == MAINTENANCE_MODE_EN) {
        ret = check_die_id((uint8_t *)key_area->die_id, DIE_ID_LEN);
        if (ret != ERRCODE_SUCC) {
            return ret;
        }
    }

    return ret;
}

errcode_t verify_image_code_info(image_type_t image_type,
    const image_code_info_t *code_info, const uint8_t *public_key)
{
    errcode_t ret = ERRCODE_FAIL;
    uint32_t image_owner = get_image_owner((uint32_t)image_type);

    /* check verify enable */
    ret = check_verify_enable(image_owner);
    if (ret == ERRCODE_NOT_SUPPORT) {
        return ERRCODE_SUCC;
    } else if (ret == ERRCODE_FAIL) {
        return ERRCODE_BOOT_VERIFY_CHECK_ENABLE_FAIL;
    }

    /* check image id */
    ret = check_image_id(image_type, CODE_INFO_TYPE, code_info->image_id);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    /* verify code info with external public key */
    ret = secure_authenticate(public_key, (uint8_t *)code_info, CODE_INFO_STRUCTURE_LENGTH - SIG_LEN - EXT_SIG_LEN,
        code_info->sig_code_info, PUBLIC_KEY_LEN);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    /* check version ext */
    ret = check_version(image_type, code_info->version_ext, code_info->mask_version_ext);
    if (ret != ERRCODE_SUCC) {
            return ret;
    }

    /* check msid */
    ret = check_msid(code_info->msid_ext, code_info->mask_msid_ext);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    return ret;
}

errcode_t verify_image_code_area(const image_code_info_t *code_info, const uint8_t *code_addr)
{
    uint32_t code_length = 0;
    uint8_t hash_result[HASH_LEN] = { 0 };
    errcode_t ret = ERRCODE_FAIL;

    /* calculate code area hash */
    code_length = code_info->code_area_len;
    ret = rom_hash((unsigned char *)(uintptr_t)code_addr, code_length, hash_result, HASH_LEN);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    secure_verify_boot_print("the code_area_len is %u", code_length);
    secure_verify_boot_dump_data("hash_result", hash_result, HASH_LEN);
    secure_verify_boot_dump_data("code_area_hash", code_info->code_area_hash, HASH_LEN);
    if (memcmp(hash_result, code_info->code_area_hash, HASH_LEN) != 0) {
        return ERRCODE_BOOT_VERIFY_INVALID_HASH_RESULT;
    }

    return ERRCODE_SUCC;
}

errcode_t verify_public_rootkey(uint32_t rootkey_buff_addr, uint32_t trust_chain_type)
{
    errcode_t ret = ERRCODE_FAIL;
    const root_public_key_area_t *rootkey_buff = (root_public_key_area_t *)(uintptr_t)rootkey_buff_addr;

    if (rootkey_buff == NULL) {
        return ERRCODE_BOOT_VERIFY_PARAM_NULL;
    }

    ret = init_rootkey_status(trust_chain_type);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    /* check verify enable */
    ret = check_verify_enable(trust_chain_type);
    if (ret == ERRCODE_NOT_SUPPORT) {
        return ERRCODE_SUCC;
    } else if (ret == ERRCODE_FAIL) {
        return ERRCODE_BOOT_VERIFY_CHECK_ENABLE_FAIL;
    }

    /* check rootkey_buff->image_id */
    ret = check_rootkey_id(trust_chain_type, rootkey_buff->image_id);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    /* check root_key hash. */
    ret = check_rootkey_hash(trust_chain_type, rootkey_buff);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    return update_rootkey_status(trust_chain_type);
}
