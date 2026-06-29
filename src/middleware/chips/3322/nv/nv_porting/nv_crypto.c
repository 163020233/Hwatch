/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: NV crypto
 * Author: @CompanyNameTag
 * History:
 * 2025-07-10, Create file.
 */

#include <string.h>
#include "efuse.h"
#include "nv_porting.h"
#include "debug_print.h"
#include "security_hash.h"

#if (CONFIG_NV_SUPPORT_ENCRYPT == NV_YES)
#include "security_symc.h"
#include "storage_common.h"
#include "trng.h"
#include "security_km.h"

#define INVALID_HANDLE         (0xFFFFFFFF)
#define NV_HEADER_LENGTH       16
#define SALT_LEN               28
#define NV_KEY_OFFSET          4
#define NV_KEY_LENGTH          16
#define NV_KEY_OFFSET_LEN      (NV_KEY_LENGTH - NV_KEY_OFFSET)
#define NV_IV_LENGTH           16
#define NV_TAG_LENGTH          16
#define NV_DIE_ID_LENGTH_BYTES 16
#define NV_DIE_ID_LEN_WORDS    4
#define NV_DIE_ID_HIGH_32      4
#define NV_DIE_ID_LOW_32       0

typedef struct {
    uint32_t symc_handle;
    uint32_t keyslot_handle;
    uint8_t tag[NV_TAG_LENGTH];
    uint8_t iv[NV_IV_LENGTH];
} crypto_info_t;

typedef struct {
    uint8_t type;
    uint8_t upgrade;
    uint16_t key_id;
    uint16_t enc_key;
    uint16_t version;
    uint32_t rnd;
    uint32_t die_id;
} kv_crypto_info_t;

// make section_key or iv, its component:
// type || upgrade || key_id || enc_key || version || rnd || DIE_ID
static errcode_t make_crypto_component(uint8_t *crypto_component, const kv_key_header_t *header, uint32_t die_id)
{
    kv_crypto_info_t *kv_crypto_key_info = (kv_crypto_info_t *)(uintptr_t)crypto_component;
    kv_crypto_key_info->type = header->type;
    kv_crypto_key_info->upgrade = header->upgrade;
    kv_crypto_key_info->key_id = header->key_id;
    kv_crypto_key_info->enc_key = header->enc_key;
    kv_crypto_key_info->version = header->version;
    kv_crypto_key_info->rnd = header->rnd;
    kv_crypto_key_info->die_id = die_id;

    return ERRCODE_SUCC;
}

// salt component: section_key || ta_uuid;
static errcode_t get_salt(uint8_t *salt, uint32_t salt_len, uint8_t *die_id, const kv_key_header_t *header)
{
    errcode_t ret = ERRCODE_FAIL;
    uint8_t section_key[NV_KEY_LENGTH] = {0};

    // get section_key
    ret = make_crypto_component(section_key, header, die_id[NV_DIE_ID_HIGH_32]);
    storage_chk_goto(ret != ERRCODE_SUCC, ERR_EXIT, "make_crypto_component failed!\n");

    ret = (errcode_t)memcpy_s(salt, salt_len, section_key + NV_KEY_OFFSET, NV_KEY_OFFSET_LEN);
    storage_chk_goto(ret != EOK, ERR_EXIT, "memcpy_s failed!\n");

ERR_EXIT:
    (void)memset_s(section_key, sizeof(section_key), 0, sizeof(section_key));
    return ret;
}

// klad make data_key keyslot
static errcode_t create_keyslot(uint32_t *keyslot_handle, const kv_key_header_t *header, uint8_t *die_id)
{
    errcode_t ret = ERRCODE_FAIL;
    uint8_t salt[SALT_LEN] = {0};

    ret = uapi_drv_keyslot_setup(keyslot_handle, UAPI_DRV_KEYSLOT_TYPE_MCIPHER);
    storage_chk_return(ret != ERRCODE_SUCC, ret, "keyslot setup failed!\n");

    ret = get_salt(salt, SALT_LEN, die_id, header);
    storage_chk_goto(ret != ERRCODE_SUCC, error_keyslot_teardown, "get_salt failed!\n");

    /* 后续需要调整 UAPI_DRV_KEYSLOT_HARD_KEY_TYPE_USER_REE -> UAPI_DRV_KEYSLOT_HARD_KEY_TYPE_SEC_STORE */
    ret = uapi_drv_keyslot_set_hard_key(*keyslot_handle, UAPI_DRV_KEYSLOT_HARD_KEY_TYPE_USER_REE,
        UAPI_DRV_KLAD_ENGINE_AES, salt, sizeof(salt), 16);  // 16: 128-bit
    storage_chk_goto(ret != ERRCODE_SUCC, error_keyslot_teardown, "send hard key failed!\n");

    (void)memset_s(salt, SALT_LEN, 0, SALT_LEN);
    return ret;

error_keyslot_teardown:
    (void)uapi_drv_keyslot_teardown(*keyslot_handle);
    return ret;
}

static void destroy_keyslot(uint32_t keyslot_handle)
{
    (void)uapi_drv_keyslot_teardown(keyslot_handle);
    return;
}

/* 以下函数预留，可不实现 */
errcode_t nv_crypto_claim_aes(uint32_t *crypto_handle, const kv_key_header_t *header, bool is_encrypt)
{
    uint32_t symc_handle = INVALID_HANDLE;
    uint32_t keyslot_handle = INVALID_HANDLE;
    uint8_t die_id[NV_DIE_ID_LENGTH_BYTES] = {0};
    errcode_t ret;
    crypto_info_t *crypto_info = NULL;

    // /* 获取Die_id */
    ret = uapi_efuse_get_die_id((uint8_t *)die_id, NV_DIE_ID_LENGTH_BYTES);
    storage_chk_goto(ret != ERRCODE_SUCC, ERR_EXIT, "get_die_id failed!\n");

    ret = ERRCODE_MALLOC;
    crypto_info = (crypto_info_t *)kv_malloc(sizeof(crypto_info_t));
    storage_chk_goto(crypto_info == NULL, ERR_EXIT, "crypto_info malloc failed!\n");
    (void)memset_s(crypto_info, sizeof(crypto_info_t), 0, sizeof(crypto_info_t));

    /* 获取iv */
    ret = make_crypto_component(crypto_info->iv, header, die_id[NV_DIE_ID_LOW_32]);
    storage_chk_goto(ret != ERRCODE_SUCC, FREE_INFO, "make_crypto_component failed!\n");

    /* 获取密钥 */
    ret = create_keyslot(&keyslot_handle, header, die_id);
    storage_chk_goto(ret != ERRCODE_SUCC, FREE_INFO, "create_keyslot failed!\n");

    ret = uapi_drv_cipher_symc_gcm_setup(&symc_handle, UAPI_DRV_CIPHER_SYMC_ALG_AES,
        NULL, NV_KEY_LENGTH, keyslot_handle, crypto_info->iv, NV_IV_LENGTH, is_encrypt);
    storage_chk_goto(ret != ERRCODE_SUCC, FREE_INFO, "uapi_drv_cipher_symc_gcm_setup failed!\n");

    ret = uapi_drv_cipher_symc_gcm_update_ad(symc_handle, (uint8_t *)header, sizeof(kv_key_header_t));
    storage_chk_goto(ret != ERRCODE_SUCC, DESTORY_HANDLE, "uapi_drv_cipher_symc_gcm_update_ad failed!\n");
    crypto_info->symc_handle = symc_handle;
    crypto_info->keyslot_handle = keyslot_handle;
    *crypto_handle = (uintptr_t)(void *)crypto_info;
    ret = ERRCODE_SUCC;
    goto ERR_EXIT;
DESTORY_HANDLE:
    (void)uapi_drv_cipher_symc_gcm_teardown(symc_handle);
FREE_INFO:
    (void)memset_s(crypto_info, sizeof(crypto_info_t), 0, sizeof(crypto_info_t));
    (void)kv_free(crypto_info);
ERR_EXIT:
    (void)memset_s(die_id, sizeof(die_id), 0, sizeof(die_id));
    return ret;
}

/* 释放加解密通道 */
void nv_crypto_release_aes(uint32_t crypto_handle)
{
    crypto_info_t *crypto_info = (crypto_info_t *)(uintptr_t)crypto_handle;

    if (crypto_info != NULL) {
        uapi_drv_cipher_symc_gcm_teardown(crypto_info->symc_handle);
        destroy_keyslot(crypto_info->keyslot_handle);
        kv_free(crypto_info);
    }
}

/* NV数据加密 */
errcode_t nv_crypto_encode(uint32_t crypto_handle, const uintptr_t src, uintptr_t dest, uint32_t length)
{
    errcode_t ret = ERRCODE_FAIL;
    crypto_info_t *crypto_info = (crypto_info_t *)(uintptr_t)crypto_handle;

    if (src == 0 || dest == 0 || crypto_info == NULL) {
        return ERRCODE_NV_INVALID_PARAM;
    }

    ret = uapi_drv_cipher_symc_gcm_update(crypto_info->symc_handle, (uint8_t *)src, (uint8_t *)dest, length);
    storage_chk_return(ret != ERRCODE_SUCC, ERRCODE_FAIL, "uuapi_drv_cipher_symc_gcm_updatet failed!\n");

    return ERRCODE_SUCC;
}

/* NV数据解密 */
errcode_t nv_crypto_decode(uint32_t crypto_handle, const uintptr_t src, uintptr_t dest, uint32_t length)
{
    errcode_t ret = ERRCODE_FAIL;
    crypto_info_t *crypto_info = (crypto_info_t *)(uintptr_t)crypto_handle;

    if (src == 0 || dest == 0 || crypto_info == NULL) {
        return ERRCODE_NV_INVALID_PARAM;
    }

    ret = uapi_drv_cipher_symc_gcm_update(crypto_info->symc_handle, (uint8_t *)src, (uint8_t *)dest, length);
    storage_chk_return(ret != ERRCODE_SUCC, ERRCODE_FAIL, "uuapi_drv_cipher_symc_gcm_updatet failed!\n");

    return ERRCODE_SUCC;
}

/* 获取加密Tag */
errcode_t nv_crypto_get_tag(uint32_t crypto_handle, uint8_t *tag, uint32_t *tag_len)
{
    errcode_t ret = ERRCODE_FAIL;
    crypto_info_t *crypto_info = (crypto_info_t *)(uintptr_t)crypto_handle;

    if (tag == NULL || tag_len == NULL || *tag_len < NV_TAG_LENGTH || crypto_info == NULL) {
        return ERRCODE_NV_INVALID_PARAM;
    }

    /* get tag */
    ret = uapi_drv_cipher_symc_gcm_finish(crypto_info->symc_handle, tag, NV_TAG_LENGTH);
    storage_chk_return(ret != ERRCODE_SUCC, ERRCODE_FAIL, "get tag failed!\n");
    *tag_len = NV_TAG_LENGTH;

    return ERRCODE_SUCC;
}

/* 设置当前NV的Tag */
errcode_t nv_crypto_set_tag(uint32_t crypto_handle, uint8_t *tag, uint32_t tag_len)
{
    errcode_t ret;
    crypto_info_t *crypto_info = (crypto_info_t *)(uintptr_t)crypto_handle;

    if (tag == NULL || crypto_info == NULL) {
        return ERRCODE_NV_INVALID_PARAM;
    }

    ret = (errcode_t)memcpy_s(crypto_info->tag, NV_TAG_LENGTH, tag, tag_len);
    storage_chk_return(ret != ERRCODE_SUCC, ERRCODE_FAIL, "set tag failed!\n");

    return ret;
}

/* 校验Tag */
errcode_t nv_crypto_validate_tag(uint32_t crypto_handle)
{
    errcode_t ret = ERRCODE_SUCC;
    uint8_t tag[NV_TAG_LENGTH] = {0};
    crypto_info_t *crypto_info = (crypto_info_t *)(uintptr_t)crypto_handle;

    if (crypto_info == NULL) {
        return ERRCODE_NV_INVALID_PARAM;
    }
    ret = uapi_drv_cipher_symc_gcm_finish(crypto_info->symc_handle, tag, NV_TAG_LENGTH);
    storage_chk_return(ret != ERRCODE_SUCC, ERRCODE_FAIL, "get tag failed!\n");
    ret = (errcode_t)memcmp(tag, crypto_info->tag, NV_TAG_LENGTH);
    storage_chk_return(ret != 0, ERRCODE_FAIL, "validate failed!\n");

    return ret;
}

/* 获取随机数 */
void nv_crypto_generate_random(uint32_t *rnd)
{
    (void)uapi_drv_cipher_trng_get_random(rnd);
}
#endif /* #if (CONFIG_NV_SUPPORT_ENCRYPT == NV_YES) */

/* 以下函数为 nv upgrade crypto 相关实现 */
/* 启动HASH计算 */
errcode_t nv_crypto_start_hash(uint32_t *handle)
{
    errcode_t ret;
    ret = uapi_drv_cipher_sha256_start(handle);
    if (ret != ERRCODE_SUCC) {
        PRINT("[UPG] uapi_drv_cipher_sha256_start fail ret = 0x%x\r\n", ret);
        return ret;
    }
    return ERRCODE_SUCC;
}

/* 更新HASH */
errcode_t nv_crypto_update_hash(uint32_t handle, const uint8_t *src, uint32_t length)
{
    errcode_t ret = uapi_drv_cipher_sha256_update(handle, src, length);
    if (ret != ERRCODE_SUCC) {
        PRINT("[UPG] uapi_drv_cipher_sha256_update fail ret = 0x%x\r\n", ret);
        return ret;
    }
    return ERRCODE_SUCC;
}

/* 完成HASH计算并返回HASH值 */
errcode_t nv_crypto_complete_hash(uint32_t handle, uint8_t *hash)
{
    errcode_t ret;

    ret = uapi_drv_cipher_sha256_finish(handle, hash);
    if (ret != ERRCODE_SUCC) {
        uapi_drv_cipher_sha_destroy(handle);
        PRINT("[UPG] uapi_drv_cipher_sha256_finish failed, ret = 0x%x\n", ret);
        return ret;
    }

    ret = uapi_drv_cipher_sha_destroy(handle);
    if (ret != ERRCODE_SUCC) {
        PRINT("[UPG] uapi_drv_cipher_sha_destroy failed, ret = 0x%x\n", ret);
    }

    return ret;
}
