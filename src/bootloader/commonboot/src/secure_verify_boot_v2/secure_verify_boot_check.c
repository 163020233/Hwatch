/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: Secure Verify for image used in boot process.
 * Author: @CompanyNameTag
 * Create: 2023-03-09
 */

#include "boot_serial.h"
#include "securec.h"
#include "boot_errcode.h"
#include "efuse_porting.h"
#include "secure_verify_boot_porting.h"
#include "secure_verify_boot_check.h"

#define boot_array_size(arr)  (sizeof(arr) / sizeof((arr)[0]))
#define security_unused(x) (void)(x)

typedef struct {
    uint32_t trust_chain_idx;
    uint32_t image_id;
    uint32_t rootkey_efuse_idx;
    uint32_t rootkey_efuse_lock_idx;
    uint8_t root_key_status;
} rootkey_table_item_t;

static rootkey_table_item_t g_root_key_info_table[] = {
    {TRUST_CHAIN_OEM, REE_PUBLIC_KEY_IMAGE_ID, EFUSE_OEM_HASH_ROOT_PUBLIC_KEY_ID,
        EFUSE_OEM_HASH_ROOT_PUBLIC_KEY_LOCK_ID, ROOT_KEY_NOT_VERIFIED},
#ifdef CONFIG_SUPPORT_TRUST_CHAIN_MODULE
    {TRUST_CHAIN_MODULE, EFUSE_MODULE_HASH_ROOT_PUBLIC_KEY_ID, EFUSE_MODULE_HASH_ROOT_PUBLIC_KEY_LOCK_ID,
        ROOT_KEY_NOT_VERIFIED},
#endif
#ifdef CONFIG_SUPPORT_TRUST_CHAIN_TEE
    {TRUST_CHAIN_TEE, TEE_PUBLIC_KEY_IMAGE_ID, EFUSE_TEE_HASH_ROOT_PUBLIC_KEY_ID,
        EFUSE_TEE_HASH_ROOT_PUBLIC_KEY_LOCK_ID, ROOT_KEY_NOT_VERIFIED},
#endif
};

#define TRUST_CHAIN_MAX_NUM     (sizeof(g_root_key_info_table) / sizeof(rootkey_table_item_t))

static verify_table_item_t *g_verify_table = NULL;
static uint32_t g_verify_table_len = 0;
static secure_authenticate_func g_secure_authenticate = NULL;
static rom_hash_func g_rom_hash = NULL;
static rom_efuse_func g_rom_efuse = NULL;

void register_verify_info(const verify_table_item_t *verify_table,
    uint32_t verify_table_len, secure_authenticate_func authenticate_func, rom_hash_func hash_func,
    rom_efuse_func efuse_func)
{
    if (verify_table != NULL) {
        g_verify_table = (verify_table_item_t *)verify_table;
        g_verify_table_len = verify_table_len;
    }
    if (authenticate_func != NULL) {
        g_secure_authenticate = authenticate_func;
    }
    if (hash_func != NULL) {
        g_rom_hash = hash_func;
    }
    if (efuse_func != NULL) {
        g_rom_efuse = efuse_func;
    }
}

errcode_t init_rootkey_status(uint32_t trust_chain_type)
{
    uint32_t i = 0;

    for (; i < TRUST_CHAIN_MAX_NUM; i++) {
        if (g_root_key_info_table[i].trust_chain_idx == trust_chain_type) {
            g_root_key_info_table[i].root_key_status = ROOT_KEY_NOT_VERIFIED;
            break;
        }
    }
    if (i == TRUST_CHAIN_MAX_NUM) {
        return ERRCODE_BOOT_VERIFY_INVALID_ROOTKEY_TYPE;
    }

    return ERRCODE_SUCC;
}

errcode_t check_rootkey_id(uint32_t trust_chain_type, uint32_t image_id)
{
    uint32_t i = 0;

    for (; i < TRUST_CHAIN_MAX_NUM; i++) {
        if (g_root_key_info_table[i].trust_chain_idx == trust_chain_type &&
            image_id == g_root_key_info_table[i].image_id) {
            return ERRCODE_SUCC;
        }
    }

    return ERRCODE_BOOT_VERIFY_INVALID_ROOTKEY_TYPE;
}

errcode_t update_rootkey_status(uint32_t trust_chain_type)
{
    uint32_t i = 0;

    for (; i < TRUST_CHAIN_MAX_NUM; i++) {
        if (g_root_key_info_table[i].trust_chain_idx == trust_chain_type) {
            g_root_key_info_table[i].root_key_status = ROOT_KEY_VERIFIED;
            break;
        }
    }
    if (i == TRUST_CHAIN_MAX_NUM) {
        return ERRCODE_BOOT_VERIFY_INVALID_ROOTKEY_TYPE;
    }

    if (g_verify_table != NULL && g_secure_authenticate != NULL && g_rom_hash != NULL && g_rom_efuse != NULL) {
        return ERRCODE_SUCC;
    }

    return ERRCODE_BOOT_VERIFY_ROOT_KEY_UPDATE_STATUS_FAILED;
}

errcode_t check_rootkey_status(image_type_t image_type)
{
    uint32_t image_owner = get_image_owner((uint32_t)image_type);
    uint32_t key_level = get_key_level((uint32_t)image_type);
    uint32_t i = 0;

    if (key_level != TRUST_IMAGE_LEVEL_ROOT) {
        return ERRCODE_SUCC;
    }

    for (; i < TRUST_CHAIN_MAX_NUM; i++) {
        if (g_root_key_info_table[i].trust_chain_idx == image_owner) {
            break;
        }
    }
    if (i == TRUST_CHAIN_MAX_NUM) {
        return ERRCODE_BOOT_VERIFY_INVALID_ROOTKEY_TYPE;
    }
    if (g_root_key_info_table[i].root_key_status != ROOT_KEY_VERIFIED) {
        return ERRCODE_BOOT_VERIFY_INVALID_ROOT_KEY;
    }

    return ERRCODE_SUCC;
}

errcode_t check_rootkey_hash(uint32_t trust_chain_type, const root_public_key_area_t *rootkey_buff)
{
    errcode_t ret = ERRCODE_FAIL;
    uint8_t efuse_hash_value[HASH_LEN];
    uint8_t efuse_hash_lock_value;
    uint8_t cal_hash_value[HASH_LEN];
    uint32_t i = 0;

    for (; i < boot_array_size(g_root_key_info_table); i++) {
        if (g_root_key_info_table[i].trust_chain_idx == trust_chain_type) {
            /* 1. read the hash_root_public_key lock value */
            ret = rom_efuse(g_root_key_info_table[i].rootkey_efuse_lock_idx, &efuse_hash_lock_value,
                (uint16_t)sizeof(efuse_hash_lock_value));
            if (ret != ERRCODE_SUCC) {
                return ERRCODE_BOOT_VERIFY_ROOT_KEY_HASH_LOCK_EFUSE_READ_FAIL;
            }

            /* 2. read the hash_root_public_key. sizeof(efuse_hash_value) = 32 */
            ret = rom_efuse(g_root_key_info_table[i].rootkey_efuse_idx, efuse_hash_value,
                (uint16_t)sizeof(efuse_hash_value));
            if (ret != ERRCODE_SUCC) {
                return ERRCODE_BOOT_VERIFY_ROOT_KEY_HASH_EFUSE_READ_FAIL;
            }
            break;
        }
    }
    if (i == boot_array_size(g_root_key_info_table)) {
        return ERRCODE_BOOT_VERIFY_GET_ROOT_KEY_EFUSE_ID_FAIL;
    }

    /* 3. check whether the hash_root_public_key bits are locked */
    if (efuse_hash_lock_value == 0) {
        return ERRCODE_BOOT_VERIFY_ROOT_KEY_HASH_EFUSE_IS_NOT_LOCKED;
    }

    /* 4. check whether the hash_root_public_key is whole zero */
    for (i = 0; i < HASH_LEN; i++) {
        if (efuse_hash_value[i] != 0) {
            break;
        }
    }
    if (i == HASH_LEN) {
        return ERRCODE_BOOT_VERIFY_ROOT_KEY_HASH_IS_ZERO;
    }

    /* 5. calculate rootkey hash */
    ret = rom_hash((unsigned char *)rootkey_buff, ROOT_PUBLIC_KEY_STRUCTURE_LENGTH, cal_hash_value, HASH_LEN);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    /* 6. check hash */
    if (memcmp(efuse_hash_value, cal_hash_value, HASH_LEN) != 0) {
        return ERRCODE_BOOT_VERIFY_INVALID_HASH_RESULT;
    }

    return ERRCODE_SUCC;
}

errcode_t check_image_id(image_type_t image_type, area_type_t erea_type, uint32_t image_id)
{
    uint32_t image_id_check = 0;
    uint32_t i = 0;
    uint32_t base_image_type = get_base_image_type((uint32_t)image_type);

    if (g_verify_table == NULL) {
        return ERRCODE_BOOT_VERIFY_TABLE_UNREGISTERED;
    }

    for (; i < g_verify_table_len; i++) {
        if (base_image_type == g_verify_table[i].image_type && erea_type == g_verify_table[i].erea_type) {
            image_id_check = g_verify_table[i].image_id;
            break;
        }
    }
    if (i == g_verify_table_len) {
        return ERRCODE_BOOT_VERIFY_INVALID_AREA_TYPE;
    }

    if (image_id != image_id_check) {
        return ERRCODE_BOOT_VERIFY_INVALID_IMAGE_ID;
    }

    return ERRCODE_SUCC;
}

#ifdef CONFIG_BOOT_SUPPORT_CHECK_VERSION
static uint32_t count_bit_num(uint32_t input)
{
    uint32_t count; /* count accumulates the total bits set in input */
    uint32_t input_copy = input;

    for (count = 0; input_copy >= 1; count++) {
        input_copy &= input_copy - 1; /* clear the least significant bit set */
    }

    return count;
}
#endif

errcode_t check_version(image_type_t image_type, uint32_t version, uint32_t version_mask)
{
#ifdef CONFIG_BOOT_SUPPORT_CHECK_VERSION
    uint32_t efuse_version;
    uint32_t efuse_idx = 0;
    uint32_t ret = ERRCODE_FAIL;
    uint32_t i = 0;
    uint32_t base_image_type = get_base_image_type((uint32_t)image_type);

    if (g_verify_table == NULL) {
        return ERRCODE_BOOT_VERIFY_TABLE_UNREGISTERED;
    }

    for (; i < g_verify_table_len; i++) {
        if (base_image_type == g_verify_table[i].image_type) {
            efuse_idx = g_verify_table[i].efuse_type;
            break;
        }
    }
    if (i == g_verify_table_len) {
        return ERRCODE_BOOT_VERIFY_INVALID_IMAGE_TYPE;
    }
    if (efuse_idx == NOT_CHECK_VERSION) {
        /* for this type of image, no need to check version. */
        return ERRCODE_SUCC;
    }

    /* read otp version */
    /* sizeof(efuse_version) = 4 */
    ret = rom_efuse(efuse_idx, (uint8_t *)&efuse_version, (uint16_t)sizeof(efuse_version));
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_BOOT_VERIFY_CHECK_VERSION_EFUSE_READ_FAIL;
    }

    if (count_bit_num(version & version_mask) < count_bit_num(efuse_version & version_mask)) {
        return ERRCODE_BOOT_VERIFY_INVALID_VERSION;
    }
#else
    security_unused(image_type);
    security_unused(version);
    security_unused(version_mask);
#endif

    return ERRCODE_SUCC;
}

errcode_t check_msid(uint32_t msid, uint32_t msid_mask)
{
#ifdef CONFIG_BOOT_SUPPORT_CHECK_MSID
    uint32_t efuse_msid;
    uint32_t ret = ERRCODE_FAIL;

    /* sizeof(efuse_msid) = 4 */
    ret = rom_efuse(EFUSE_MSID_ID, (uint8_t *)&efuse_msid, (uint16_t)sizeof(efuse_msid));
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_BOOT_VERIFY_CHECK_MSID_EFUSE_READ_FAIL;
    }

    if ((msid & msid_mask) != (efuse_msid & msid_mask)) {
        return ERRCODE_BOOT_VERIFY_INVALID_MSID;
    }
#else
    security_unused(msid);
    security_unused(msid_mask);
#endif

    return ERRCODE_SUCC;
}

errcode_t check_die_id(uint8_t *die_id, uint32_t die_id_length)
{
    uint8_t efuse_die_id[DIE_ID_LEN];
    uint32_t ret = ERRCODE_FAIL;

    if (die_id_length != DIE_ID_LEN) {
        return ERRCODE_FAIL;
    }

    /* sizeof(efuse_die_id) = 16 */
    ret = rom_efuse(EFUSE_DIE_ID, (uint8_t *)efuse_die_id, (uint16_t)sizeof(efuse_die_id));
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_BOOT_VERIFY_CHECK_DIE_ID_EFUSE_READ_FAIL;
    }

    if (memcmp(efuse_die_id, die_id, die_id_length) != 0) {
        return ERRCODE_BOOT_VERIFY_INVALID_DIE_ID;
    }

    return ERRCODE_SUCC;
}

errcode_t secure_authenticate(const uint8_t *key, const uint8_t *data, uint32_t data_length,
    const uint8_t *sign_buff, uint32_t sign_length)
{
    if (g_secure_authenticate != NULL) {
        return g_secure_authenticate(key, data, data_length, sign_buff, sign_length);
    }
    return ERRCODE_BOOT_VERIFY_AUTHENTICATE_FUNC_NOT_REGISTERED;
}

errcode_t rom_hash(const unsigned char *data, unsigned int data_length, unsigned char *hash,
    unsigned int hash_length)
{
    if (g_rom_hash != NULL) {
        return g_rom_hash(data, data_length, hash, hash_length);
    }
    return ERRCODE_BOOT_VERIFY_HASH_FUNC_NOT_REGISTERED;
}

errcode_t rom_efuse(efuse_idx efuse_id, uint8_t *data, uint16_t data_len)
{
    if (g_rom_efuse != NULL) {
        return g_rom_efuse(efuse_id, data, data_len);
    }
    return ERRCODE_BOOT_VERIFY_EFUSE_FUNC_NOT_REGISTERED;
}