/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Secure Verify for image used in boot process.
 */

#include "boot_errcode.h"
#include "securec.h"
#include "unified_cipher_pke.h"
#include "security_init.h"
#include "security_hash.h"
#if defined(BUILD_APPLICATION_STANDARD) && !defined(BUILD_RECOVERY_IMAGE)
#include "ree_tee_comm.h"
#include "chips_porting_ioctl.h"
#include "pmp_config.h"
#endif
#include "sfc_v2.h"
#include "secure_verify_boot_check.h"
#include "secure_verify_boot_porting.h"

static const verify_table_item_t g_verify_table_registered[] = {
    {
        .image_type = BASE_IMAGE_TYPE_MCU, .erea_type = KEY_EREA_TYPE,
        .image_id = REE_APP_KEY_AREA_IMAGE_ID,
        .efuse_type = NOT_CHECK_VERSION
    },
    {
        .image_type = BASE_IMAGE_TYPE_MCU, .erea_type = CODE_INFO_TYPE,
        .image_id = REE_APP_CODE_INFO_IMAGE_ID,
        .efuse_type = NOT_CHECK_VERSION
    },
    {
        .image_type = BASE_IMAGE_TYPE_RECOVERY, .erea_type = KEY_EREA_TYPE,
        .image_id = RECOVERY_KEY_AREA_IMAGE_ID,
        .efuse_type = NOT_CHECK_VERSION
    },
    {
        .image_type = BASE_IMAGE_TYPE_RECOVERY, .erea_type = CODE_INFO_TYPE,
        .image_id = RECOVERY_CODE_INFO_IMAGE_ID,
        .efuse_type = NOT_CHECK_VERSION
    },
    {
        .image_type = BASE_IMAGE_TYPE_TEE_RUNTIME_FW, .erea_type = KEY_EREA_TYPE,
        .image_id = TEE_RUN_TIME_FW_KEY_AREA_IMAGE_ID,
        .efuse_type = EFUSE_TEE_VER_ID
    },
    {
        .image_type = BASE_IMAGE_TYPE_TEE_RUNTIME_FW, .erea_type = CODE_INFO_TYPE,
        .image_id = TEE_RUN_TIME_FW_AREA_INFO_IMAGE_ID,
        .efuse_type = EFUSE_TEE_VER_ID
    },
    {
        .image_type = BASE_IMAGE_TYPE_BT, .erea_type = KEY_EREA_TYPE,
        .image_id = BT_KEY_AREA_IMAGE_ID,
        .efuse_type = NOT_CHECK_VERSION
    },
    {
        .image_type = BASE_IMAGE_TYPE_BT, .erea_type = CODE_INFO_TYPE,
        .image_id = BT_CODE_INFO_IMAGE_ID,
        .efuse_type = NOT_CHECK_VERSION
    },
    {
        .image_type = BASE_IMAGE_TYPE_DSP, .erea_type = KEY_EREA_TYPE,
        .image_id = DSP_KEY_AREA_IMAGE_ID,
        .efuse_type = NOT_CHECK_VERSION
    },
    {
        .image_type = BASE_IMAGE_TYPE_DSP, .erea_type = CODE_INFO_TYPE,
        .image_id = DSP_CODE_INFO_IMAGE_ID,
        .efuse_type = NOT_CHECK_VERSION
    },
    {
        .image_type = BASE_IMAGE_TYPE_DSP1, .erea_type = CODE_INFO_TYPE,
        .image_id = DSP1_CODE_INFO_IMAGE_ID,
        .efuse_type = NOT_CHECK_VERSION
    },
    {
        .image_type = BASE_IMAGE_TYPE_DSP1, .erea_type = KEY_EREA_TYPE,
        .image_id = DSP1_KEY_AREA_IMAGE_ID,
        .efuse_type = NOT_CHECK_VERSION
    },
    {
        .image_type = BASE_IMAGE_TYPE_PARAMS, .erea_type = KEY_EREA_TYPE,
        .image_id = PARAMS_KEY_AREA_IMAGE_ID,
        .efuse_type = NOT_CHECK_VERSION
    },
    {
        .image_type = BASE_IMAGE_TYPE_PARAMS, .erea_type = CODE_INFO_TYPE,
        .image_id = PARAMS_AREA_INFO_IMAGE_ID,
        .efuse_type = NOT_CHECK_VERSION
    },
    {
        .image_type = BASE_IMAGE_TYPE_UPG_FLASHBOOT, .erea_type = KEY_EREA_TYPE,
        .image_id = REE_BOOT_KEY_AREA_IMAGE_ID,
        .efuse_type = EFUSE_BOOT_VER_ID
    },
    {
        .image_type = BASE_IMAGE_TYPE_UPG_FLASHBOOT, .erea_type = CODE_INFO_TYPE,
        .image_id = REE_BOOT_CODE_INFO_IMAGE_ID,
        .efuse_type = EFUSE_BOOT_VER_ID
    },
    {
        .image_type = BASE_IMAGE_TYPE_UPG_TEE_FW, .erea_type = KEY_EREA_TYPE,
        .image_id = TEE_RUN_TIME_FW_KEY_AREA_IMAGE_ID,
        .efuse_type = EFUSE_TEE_VER_ID
    },
    {
        .image_type = BASE_IMAGE_TYPE_UPG_TEE_FW, .erea_type = CODE_INFO_TYPE,
        .image_id = TEE_RUN_TIME_FW_AREA_INFO_IMAGE_ID,
        .efuse_type = EFUSE_TEE_VER_ID
    },
};

static errcode_t drv_rom_cipher_sha256(const unsigned char *data, unsigned int data_length, unsigned char *hash,
    unsigned int hash_length)
{
    unused(hash_length);
    return uapi_drv_cipher_sha256(data, data_length, hash);
}

errcode_t get_verify_info(verify_table_item_t **out_table, uint32_t *out_table_len,
    secure_authenticate_func *authenticate_func, rom_hash_func *hash_func, rom_efuse_func *efuse_func)
{
    if (out_table == NULL || out_table_len == NULL || authenticate_func == NULL || hash_func == NULL ||
        efuse_func == NULL) {
        return ERRCODE_BOOT_VERIFY_PARAM_NULL;
    }
    *out_table = (verify_table_item_t *)g_verify_table_registered;
    *out_table_len = sizeof(g_verify_table_registered) / sizeof(verify_table_item_t);
    *authenticate_func = secure_verify_authenticate;
    *hash_func = drv_rom_cipher_sha256;
    *efuse_func = efuse_read_item;
    return ERRCODE_SUCC;
}

errcode_t secure_verify_authenticate(const uint8_t *key, const uint8_t *data, uint32_t data_length,
    const uint8_t *sign_buff, uint32_t sign_length)
{
    uint8_t hash_result[HASH_LEN];
    /* 32 is ecc bp256 key length */
    uapi_drv_cipher_pke_ecc_point_t input_pub_key = {.x = (uint8_t *)key, .y = (uint8_t *)(key + 32), .length = 32};
    uapi_drv_cipher_pke_data_t input_hash = {.data = hash_result, .length = HASH_LEN};
    /* 2: sign =  r + s */
    uapi_drv_cipher_pke_ecc_sig_t input_sig = {.r = (uint8_t *)sign_buff, .s = (uint8_t *)(sign_buff + sign_length / 2),
        .length = sign_length / 2};
    errcode_t ret = ERRCODE_FAIL;

    ret = uapi_drv_cipher_sha256(data, data_length, hash_result);
    if (ret != ERRCODE_SUCC) {
        PRINT("drv cipher sha fail\r\n");
        return ret;
    }

    ret = uapi_drv_cipher_pke_ecdsa_verify(
        UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC5639_P256, &input_pub_key, &input_hash, &input_sig);
    if (ret != ERRCODE_SUCC) {
        PRINT("drv cipher pke fail\r\n");
        return ret;
    }

    return ERRCODE_SUCC;
}

void secure_verify_init(void)
{
    uint32_t out_table_len = sizeof(g_verify_table_registered) / sizeof(verify_table_item_t);
    register_verify_info(g_verify_table_registered, out_table_len, secure_verify_authenticate, drv_rom_cipher_sha256,
        efuse_read_item);
    (void)update_rootkey_status(TRUST_CHAIN_OEM);
}

static errcode_t verify_tee_image_code_area(const image_code_info_t *code_info, const uint8_t *code_addr)
{
    errcode_t ret = ERRCODE_FAIL;
    hash_handle_t hash_handle;
    uint8_t hash_result[HASH_LEN];
    uint32_t total_code_length = code_info->code_area_len;
    /* first_buffer is tee code running on itcm */
    uint8_t *first_buffer = (uint8_t *)code_addr;
    uint32_t first_buffer_len = code_info->text_segment_size;
    /* second_buffer is tee code running on flash */
    uint8_t *second_buffer = (uint8_t *)(SELITEOS_FLASH_REGION_START + FLASH_SIGN_HEADER + first_buffer_len);
    uint32_t second_buffer_len = total_code_length - first_buffer_len;

    (void)memset_s(hash_result, sizeof(hash_result), 0, sizeof(hash_result));
    ret = uapi_drv_cipher_sha256_start(&hash_handle);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    ret = uapi_drv_cipher_sha256_update(hash_handle, first_buffer, first_buffer_len);
    if (ret != ERRCODE_SUCC) {
        goto error_exit;
    }

    ret = uapi_drv_cipher_sha256_update(hash_handle, second_buffer, second_buffer_len);
    if (ret != ERRCODE_SUCC) {
        goto error_exit;
    }

    ret = uapi_drv_cipher_sha256_finish(hash_handle, hash_result);
    if (ret != ERRCODE_SUCC) {
        goto error_exit;
    }

    secure_verify_boot_dump_data("hash_result", hash_result, HASH_LEN);
    secure_verify_boot_dump_data("code_area_hash", code_info->code_area_hash, HASH_LEN);
    if (memcmp(hash_result, code_info->code_area_hash, HASH_LEN) != 0) {
        ret = ERRCODE_BOOT_VERIFY_INVALID_HASH_RESULT;
    }

error_exit:
    (void)uapi_drv_cipher_sha_destroy(hash_handle);
    return ret;
}

errcode_t secure_verify_image(image_type_t image_type, uint32_t key_area_addr, uint32_t verify_public_key_addr)
{
    errcode_t ret = ERRCODE_FAIL;
    uint8_t *verify_public_key = NULL;
    if (verify_public_key_addr == ROOT_PUBLIC_KEY_START) {
        verify_public_key = ((root_public_key_area_t *)(uintptr_t)verify_public_key_addr)->root_public_key;
    } else {
        verify_public_key = ((image_key_area_t *)(uintptr_t)verify_public_key_addr)->ext_public_key;
    }
    image_key_area_t *key_area = (image_key_area_t *)(uintptr_t)key_area_addr;
    image_code_info_t *code_info = NULL;
    uint8_t *code_addr = NULL;

    ret = check_verify_enable(get_image_owner(image_type));
    if (ret == ERRCODE_NOT_SUPPORT) {
        return ERRCODE_SUCC;
    } else if (ret != ERRCODE_SUCC) {
        return ret;
    }

    /* verify image key area */
    ret = verify_image_key_area(image_type, key_area, verify_public_key);
    if (ret != ERRCODE_SUCC) {
        secure_verify_boot_msg1("ka, ", ret);
        return ret;
    }
    /* verify image code info. */
    code_info = (image_code_info_t *)(uintptr_t)(key_area_addr + KEY_AREA_STRUCTURE_LENGTH);
    secure_verify_boot_dump_data("code_info", (uint8_t *)(uintptr_t)code_info, CODE_INFO_STRUCTURE_LENGTH);
    ret = verify_image_code_info(image_type, code_info, key_area->ext_public_key);
    if (ret != ERRCODE_SUCC) {
        secure_verify_boot_msg1("ci, ", ret);
        return ret;
    }
    /* verify image code area. */
    code_addr = (uint8_t *)(uintptr_t)((uintptr_t)code_info + CODE_INFO_STRUCTURE_LENGTH);
    secure_verify_boot_dump_data("code_addr", (uint8_t *)(uintptr_t)code_addr, 512);    /* 512: data for info check. */
    if (image_type == OEM_TEE_FIRMWARE_TYPE) {
        ret = verify_tee_image_code_area(code_info, code_addr);
    } else {
        ret = verify_image_code_area(code_info, code_addr);
    }
    if (ret != ERRCODE_SUCC) {
        secure_verify_boot_msg1("ca, ", ret);
        return ret;
    }

    return ret;
}

errcode_t secure_verify_get_board_version(uint32_t key_area_addr, uint32_t *key_ver, uint32_t *code_ver)
{
#if defined(BUILD_APPLICATION_STANDARD) && !defined(BUILD_RECOVERY_IMAGE)
    if (key_area_addr == SSB_FLASH_REGION_START || key_area_addr == SELITEOS_FLASH_REGION_START) {
        uint32_t cmd = (key_area_addr == SSB_FLASH_REGION_START) ? CMD_READ_SSB_KEY_CODE_VER_NR :
                                                                   CMD_READ_TEE_KEY_CODE_VER_NR;
        get_key_code_ver_arg_t local_arg = {.key_ver = key_ver, .code_ver = code_ver};
#ifdef CFG_DRIVERS_SPINOR
        pmp_norflash_config(1);
#endif
        errcode_t ret = ree_tee_comm_unify_entry(cmd, &local_arg);
#ifdef CFG_DRIVERS_SPINOR
        pmp_norflash_config(0);
#endif
        return ret;
    }
#endif

    image_key_area_t key_area = {0};
    errcode_t ret_val = uapi_sfc_read(SFC_ID_0, key_area_addr, (uint8_t *)(&key_area), sizeof(image_key_area_t));
    if (ret_val != ERRCODE_SUCC) {
        return ret_val;
    }
    *key_ver = key_area.key_version_ext;

    image_code_info_t code_info = {0};
    ret_val = uapi_sfc_read(SFC_ID_0, key_area_addr + KEY_AREA_STRUCTURE_LENGTH, (uint8_t *)(&code_info),
                            sizeof(image_code_info_t));
    if (ret_val != ERRCODE_SUCC) {
        return ret_val;
    }
    *code_ver = code_info.version_ext;
    return ret_val;
}

uint32_t get_tee_version(void)
{
#if defined(BUILD_APPLICATION_STANDARD) && !defined(BUILD_RECOVERY_IMAGE)
    uint32_t tee_ver;
    get_tee_ver_arg_t local_arg = {.version = &tee_ver};
    ree_tee_comm_unify_entry(CMD_READ_TEE_VER, &local_arg);
    return tee_ver;
#else
    return 0xFFFFFFFF;
#endif
}
