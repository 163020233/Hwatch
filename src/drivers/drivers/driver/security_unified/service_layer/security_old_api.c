/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 *
 * Description: Provides old api adapt. \n
 */
#include "cipher.h"
#include "drv_hash.h"
#include "drv_symc.h"
#include "sl_common.h"

#include "crypto_drv_common.h"
#include "security_mac.h"
#include "security_km.h"
#include "km.h"

#define FILE_ID FILE_ID_SERVICE_LAYER_SECURITY_OLD_API

typedef struct {
    uapi_drv_cipher_symc_attr_t symc_attr;
    uapi_drv_cipher_symc_ctrl_t symc_ctrl;
    uint32_t keyslot_handle;
    uint8_t nc_off;
    uint8_t stream_block[CRYPTO_AES_BLOCK_SIZE_IN_BYTES];
    cipher_handle_t symc_ctx;
} uapi_symc_ctx_t;

errcode_t uapi_drv_cipher_symc_init(void)
{
    return ERRCODE_SUCC;
}

errcode_t uapi_drv_cipher_symc_deinit(void)
{
    return ERRCODE_SUCC;
}

errcode_t uapi_drv_cipher_symc_create(uint32_t *symc_handle, const uapi_drv_cipher_symc_attr_t *symc_attr)
{
    int32_t ret;
    uapi_symc_ctx_t *ctx = NULL;

    drv_err_chk_return(symc_handle == NULL, ERRCODE_INVALID_PARAM, "symc_handle is NULL\n");
    drv_err_chk_return(symc_attr == NULL, ERRCODE_INVALID_PARAM, "symc_attr is NULL\n");

    ctx = crypto_malloc_coherent(sizeof(uapi_symc_ctx_t));
    drv_err_chk_goto_with_ret(ret, ctx == NULL, exit, ERROR_MALLOC, "malloc failed\n");
    (void)memset_s(ctx, sizeof(uapi_symc_ctx_t), 0, sizeof(uapi_symc_ctx_t));

    ctx->symc_attr = *symc_attr;
    *symc_handle = (uintptr_t)ctx;
    ret = CRYPTO_SUCCESS;
exit:
    return crypto_sl_common_get_errcode(ret);
}

static void inner_symc_ctx_teardown(cipher_handle_t symc_ctx, uapi_drv_cipher_symc_work_mode_t work_mode)
{
    if (symc_ctx != 0) {
        if (work_mode == UAPI_DRV_CIPHER_SYMC_WORK_MODE_CCM) {
            (void)uapi_drv_cipher_symc_ccm_teardown(symc_ctx);
        } else if (work_mode == UAPI_DRV_CIPHER_SYMC_WORK_MODE_GCM) {
            (void)uapi_drv_cipher_symc_gcm_teardown(symc_ctx);
        } else {
            (void)uapi_drv_cipher_symc_teardown(symc_ctx);
        }
    }
}

errcode_t uapi_drv_cipher_symc_destroy(uint32_t symc_handle)
{
    uapi_symc_ctx_t *ctx = (uapi_symc_ctx_t *)(uintptr_t)symc_handle;

    drv_err_chk_return(ctx == NULL, ERRCODE_INVALID_PARAM, "symc_handle is NULL\n");

    inner_symc_ctx_teardown(ctx->symc_ctx, ctx->symc_attr.work_mode);
    (void)memset_s(ctx, sizeof(uapi_symc_ctx_t), 0, sizeof(uapi_symc_ctx_t));
    crypto_free_coherent(ctx);
    return ERRCODE_SUCC;
}

errcode_t uapi_drv_cipher_symc_set_config(uint32_t symc_handle, const uapi_drv_cipher_symc_ctrl_t *symc_ctrl)
{
    uapi_symc_ctx_t *ctx = (uapi_symc_ctx_t *)(uintptr_t)symc_handle;

    drv_err_chk_return(ctx == NULL, ERRCODE_INVALID_PARAM, "symc_handle is NULL\n");
    drv_err_chk_return(symc_ctrl == NULL, ERRCODE_INVALID_PARAM, "symc_ctrl is NULL\n");

    inner_symc_ctx_teardown(ctx->symc_ctx, ctx->symc_ctrl.work_mode);
    ctx->symc_ctx = 0;
    ctx->nc_off = 0;
    (void)memset_s(ctx->stream_block, sizeof(ctx->stream_block), 0, sizeof(ctx->stream_block));

    ctx->symc_ctrl = *symc_ctrl;
    return ERRCODE_SUCC;
}

errcode_t uapi_drv_cipher_symc_get_config(uint32_t symc_handle, const uapi_drv_cipher_symc_ctrl_t *symc_ctrl)
{
    uapi_symc_ctx_t *ctx = (uapi_symc_ctx_t *)(uintptr_t)symc_handle;

    drv_err_chk_return(ctx == NULL, ERRCODE_INVALID_PARAM, "symc_handle is NULL\n");
    drv_err_chk_return(symc_ctrl == NULL, ERRCODE_INVALID_PARAM, "symc_ctrl is NULL\n");

    *(uapi_drv_cipher_symc_ctrl_t *)symc_ctrl = ctx->symc_ctrl;
    return ERRCODE_SUCC;
}

errcode_t uapi_drv_cipher_symc_attach(uint32_t symc_handle, uint32_t keyslot_handle)
{
    uapi_symc_ctx_t *ctx = (uapi_symc_ctx_t *)(uintptr_t)symc_handle;

    drv_err_chk_return(ctx == NULL, ERRCODE_INVALID_PARAM, "symc_handle is NULL\n");

    ctx->keyslot_handle = keyslot_handle;
    return ERRCODE_SUCC;
}

errcode_t uapi_drv_cipher_symc_detach(uint32_t symc_handle, uint32_t keyslot_handle)
{
    uapi_symc_ctx_t *ctx = (uapi_symc_ctx_t *)(uintptr_t)symc_handle;

    drv_err_chk_return(ctx == NULL, ERRCODE_INVALID_PARAM, "symc_handle is NULL\n");

    crypto_unused(keyslot_handle);
    ctx->keyslot_handle = 0;
    return ERRCODE_SUCC;
}

static errcode_t inner_symc_ccm_crypt(uapi_symc_ctx_t *ctx, const uint8_t *src,
    uint8_t *dst, uint32_t length, uint32_t key_len, bool is_encrypt)
{
    errcode_t ret;
    uapi_drv_cipher_symc_config_aes_ccm_gcm_t *ccm_gcm_param = NULL;

    ccm_gcm_param = (uapi_drv_cipher_symc_config_aes_ccm_gcm_t *)ctx->symc_ctrl.param;
    drv_err_chk_return(ccm_gcm_param == NULL, ERRCODE_INVALID_PARAM, "ccm_gcm_param is NULL\n");

    if (ctx->symc_ctx == 0) {
        ret = uapi_drv_cipher_symc_ccm_setup(&ctx->symc_ctx, ctx->symc_ctrl.symc_alg,
            NULL, key_len, ctx->keyslot_handle,
            ctx->symc_ctrl.iv, ctx->symc_ctrl.iv_length, is_encrypt);
        drv_err_chk_return(ret != ERRCODE_SUCC, ret, "ccm setup failed\n");
        /* teardown will be call in uapi_drv_cipher_symc_destroy. */
        ret = uapi_drv_cipher_symc_ccm_set_lengths(ctx->symc_ctx, ccm_gcm_param->aad_len, ccm_gcm_param->data_len,
            ccm_gcm_param->tag_len);
        drv_err_chk_return(ret != ERRCODE_SUCC, ret, "ccm set lengths failed\n");
        ret = uapi_drv_cipher_symc_ccm_update_ad(ctx->symc_ctx, ccm_gcm_param->aad_buf.virt_addr,
            ccm_gcm_param->aad_len);
        drv_err_chk_return(ret != ERRCODE_SUCC, ret, "ccm update ad failed\n");
    }
    return uapi_drv_cipher_symc_ccm_update(ctx->symc_ctx, src, dst, length);
}

static errcode_t inner_symc_gcm_crypt(uapi_symc_ctx_t *ctx, const uint8_t *src,
    uint8_t *dst, uint32_t length, uint32_t key_len, bool is_encrypt)
{
    errcode_t ret;
    uapi_drv_cipher_symc_config_aes_ccm_gcm_t *ccm_gcm_param = NULL;

    ccm_gcm_param = (uapi_drv_cipher_symc_config_aes_ccm_gcm_t *)ctx->symc_ctrl.param;
    drv_err_chk_return(ccm_gcm_param == NULL, ERRCODE_INVALID_PARAM, "ccm_gcm_param is NULL\n");

    if (ctx->symc_ctx == 0) {
        ret = uapi_drv_cipher_symc_gcm_setup(&ctx->symc_ctx, ctx->symc_ctrl.symc_alg,
            NULL, key_len, ctx->keyslot_handle,
            ctx->symc_ctrl.iv, ctx->symc_ctrl.iv_length, is_encrypt);
        drv_err_chk_return(ret != ERRCODE_SUCC, ret, "gcm setup failed\n");
        /* teardown will be call in uapi_drv_cipher_symc_destroy. */
        ret = uapi_drv_cipher_symc_gcm_update_ad(ctx->symc_ctx, ccm_gcm_param->aad_buf.virt_addr,
            ccm_gcm_param->aad_len);
        drv_err_chk_return(ret != ERRCODE_SUCC, ret, "gcm update ad failed\n");
    }
    return uapi_drv_cipher_symc_gcm_update(ctx->symc_ctx, src, dst, length);
}

static errcode_t inner_symc_crypt(uint32_t symc_handle, const uapi_drv_cipher_buf_attr_t *src_buf,
    const uapi_drv_cipher_buf_attr_t *dst_buf, uint32_t length, bool is_encrypt)
{
    errcode_t ret;
    uapi_symc_ctx_t *ctx = (uapi_symc_ctx_t *)(uintptr_t)symc_handle;
    uint32_t alg;
    uint32_t key_len = CRYPTO_128_KEY_LEN;  /* default */
    const uint8_t *src = NULL;
    uint8_t *dst = NULL;

    drv_err_chk_return(ctx == NULL, ERRCODE_INVALID_PARAM, "symc_handle is NULL\n");
    drv_err_chk_return(src_buf == NULL, ERRCODE_INVALID_PARAM, "src_buf is NULL\n");
    drv_err_chk_return(dst_buf == NULL, ERRCODE_INVALID_PARAM, "dst_buf is NULL\n");

    if (ctx->symc_ctrl.symc_key_length == UAPI_DRV_CIPHER_SYMC_KEY_192BIT) {
        key_len = CRYPTO_192_KEY_LEN;
    } else if (ctx->symc_ctrl.symc_key_length == UAPI_DRV_CIPHER_SYMC_KEY_256BIT) {
        key_len = CRYPTO_256_KEY_LEN;
    }
    alg = uapi_compat_symc_alg(ctx->symc_ctrl.symc_alg, ctx->symc_ctrl.work_mode, ctx->symc_ctrl.symc_bit_width);
    src = (const uint8_t *)(uintptr_t)src_buf->phys_addr;
    dst = (uint8_t *)(uintptr_t)dst_buf->phys_addr;

    if (ctx->symc_ctrl.work_mode == UAPI_DRV_CIPHER_SYMC_WORK_MODE_CCM) {
        return inner_symc_ccm_crypt(ctx, src, dst, length, key_len, is_encrypt);
    } else if (ctx->symc_ctrl.work_mode == UAPI_DRV_CIPHER_SYMC_WORK_MODE_GCM) {
        return inner_symc_gcm_crypt(ctx, src, dst, length, key_len, is_encrypt);
    } else if (ctx->symc_ctrl.work_mode == UAPI_DRV_CIPHER_SYMC_WORK_MODE_CTR) {
        if (ctx->symc_ctx == 0) {
            ret = uapi_drv_cipher_symc_setup(&ctx->symc_ctx, alg, NULL, key_len, ctx->keyslot_handle, is_encrypt);
            drv_err_chk_return(ret != ERRCODE_SUCC, ret, "setup failed\n");
        }
        return uapi_drv_cipher_symc_ctr_update(ctx->symc_ctx, src, dst, length,
            ctx->symc_ctrl.iv, &(ctx->nc_off), ctx->stream_block);
    } else {
        return uapi_drv_cipher_symc_crypt(alg, src, dst, length,
            ctx->symc_ctrl.iv, NULL, key_len, ctx->keyslot_handle, is_encrypt);
    }
}

errcode_t uapi_drv_cipher_symc_encrypt(uint32_t symc_handle, const uapi_drv_cipher_buf_attr_t *src_buf,
    const uapi_drv_cipher_buf_attr_t *dst_buf, uint32_t length)
{
    return inner_symc_crypt(symc_handle, src_buf, dst_buf, length, true);
}
errcode_t uapi_drv_cipher_symc_decrypt(uint32_t symc_handle, const uapi_drv_cipher_buf_attr_t *src_buf,
    const uapi_drv_cipher_buf_attr_t *dst_buf, uint32_t length)
{
    return inner_symc_crypt(symc_handle, src_buf, dst_buf, length, false);
}

errcode_t uapi_drv_cipher_symc_get_tag(uint32_t symc_handle, uint8_t *tag, uint32_t tag_length)
{
    uapi_symc_ctx_t *ctx = (uapi_symc_ctx_t *)(uintptr_t)symc_handle;

    drv_err_chk_return(ctx == NULL, ERRCODE_INVALID_PARAM, "symc_handle is NULL\n");
    drv_err_chk_return(tag == NULL, ERRCODE_INVALID_PARAM, "tag is NULL\n");
    drv_err_chk_return(tag_length > CRYPTO_AES_MAX_TAG_SIZE, ERRCODE_INVALID_PARAM, "tag_length is invalid\n");

    if (ctx->symc_ctrl.work_mode == UAPI_DRV_CIPHER_SYMC_WORK_MODE_CCM) {
        return uapi_drv_cipher_symc_ccm_finish(ctx->symc_ctx, tag, tag_length);
    } else if (ctx->symc_ctrl.work_mode == UAPI_DRV_CIPHER_SYMC_WORK_MODE_GCM) {
        return uapi_drv_cipher_symc_gcm_finish(ctx->symc_ctx, tag, tag_length);
    } else {
        return ERRCODE_SECURITY_UNEXPECTED;
    }
    return ERRCODE_SUCC;
}

errcode_t uapi_drv_cipher_mac_start(uint32_t *symc_handle, const uapi_drv_cipher_symc_mac_attr_t *mac_attr)
{
    int32_t ret;
    mac_handle_t mac_handle;
    uint32_t key_len = CRYPTO_128_KEY_LEN;  /* default */

    drv_err_chk_return(symc_handle == NULL, ERRCODE_INVALID_PARAM, "symc_handle is NULL\n");
    drv_err_chk_return(mac_attr == NULL, ERRCODE_INVALID_PARAM, "mac_attr is NULL\n");
    if (mac_attr->symc_key_length == UAPI_DRV_CIPHER_SYMC_KEY_192BIT) {
        key_len = CRYPTO_192_KEY_LEN;
    } else if (mac_attr->symc_key_length == UAPI_DRV_CIPHER_SYMC_KEY_256BIT) {
        key_len = CRYPTO_256_KEY_LEN;
    }

    crypto_service_preprocess(SYMC_MODULE_ID);
    ret = drv_symc_mac_start((void **)(&mac_handle), (crypto_symc_alg)mac_attr->symc_alg,
        (crypto_symc_work_mode)mac_attr->work_mode, NULL, key_len, mac_attr->keyslot_chn);
    if (ret != CRYPTO_SUCCESS) {
        crypto_service_postprocess(SYMC_MODULE_ID);
    }
    *symc_handle = mac_handle;
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_mac_update(uint32_t symc_handle, const uapi_drv_cipher_buf_attr_t *src_buf, uint32_t length)
{
    int32_t ret;
    drv_err_chk_return(src_buf == NULL, ERRCODE_INVALID_PARAM, "src_buf is NULL\n");

    ret = drv_symc_mac_update((void *)(uintptr_t)symc_handle, src_buf->virt_addr, length);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_mac_finish(uint32_t symc_handle, uint8_t *mac, uint32_t *mac_length)
{
    int32_t ret;

    drv_err_chk_return(mac_length == NULL, ERRCODE_INVALID_PARAM, "src_buf is NULL\n");

    ret = drv_symc_mac_finish((void *)(uintptr_t)symc_handle, mac);
    (void)drv_symc_mac_destroy((void *)(uintptr_t)symc_handle);
    crypto_service_postprocess(SYMC_MODULE_ID);

    *mac_length = CRYPTO_AES_BLOCK_SIZE_IN_BYTES;
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_hash_init(void)
{
    return ERRCODE_SUCC;
}

errcode_t uapi_drv_cipher_hash_deinit(void)
{
    return ERRCODE_SUCC;
}

#define INVALID_KEYSLOT_HANDLE  (0xFFFFFFFF)
errcode_t uapi_drv_cipher_hash_start(uint32_t *hash_handle, const uapi_drv_cipher_hash_attr_t *hash_attr)
{
    int32_t ret;

    drv_err_chk_return(hash_handle == NULL, ERRCODE_SECURITY_INVALID_PARAM, "hash_handle is NULL\n");
    drv_err_chk_return(hash_attr == NULL, ERRCODE_SECURITY_INVALID_PARAM, "hash_attr is NULL\n");

    if (crypto_hash_is_hmac(hash_attr->hash_type)) {
        return crypto_hmac_start(hash_handle, hash_attr->hash_type, hash_attr->key, hash_attr->key_len,
            INVALID_KEYSLOT_HANDLE);
    } else {
        crypto_service_preprocess(HASH_MODULE_ID);
        ret = drv_cipher_hash_compute_setup((void **)hash_handle);
        drv_err_chk_goto(ret != CRYPTO_SUCCESS, exit_post_process, "hash setup failed\r\n");

        ret = drv_cipher_hash_compute_start((void *)(uintptr_t)(*hash_handle), (crypto_hash_type)hash_attr->hash_type);
        drv_err_chk_goto(ret != ERRCODE_SUCC, exit_free, "hash start failed!\r\n");
    }
    return crypto_sl_common_get_errcode(ret);

exit_free:
    drv_cipher_hash_compute_teardown((void *)(uintptr_t)(*hash_handle));
exit_post_process:
    crypto_service_postprocess(HASH_MODULE_ID);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_hash_update(uint32_t hash_handle,  const uapi_drv_cipher_buf_attr_t *src_buf,
    const uint32_t len)
{
    crypto_hash_type *hash_type;

    drv_err_chk_return(hash_handle == 0, ERRCODE_SECURITY_INVALID_PARAM, "hash_handle is NULL\n");
    drv_err_chk_return(src_buf == NULL, ERRCODE_SECURITY_INVALID_PARAM, "src_buf is NULL\n");
    if (len == 0) {
        return ERRCODE_SUCC;
    }

    hash_type = (crypto_hash_type *)(uintptr_t)(hash_handle);
    if (crypto_hash_is_hmac(*hash_type)) {
        return crypto_hmac_update(hash_handle, src_buf->virt_addr, len);
    } else {
        return drv_cipher_hash_compute_update((void *)(uintptr_t)hash_handle, src_buf->virt_addr, len);
    }
}

errcode_t uapi_drv_cipher_hash_finish(uint32_t hash_handle, uint8_t *out, uint32_t *out_len)
{
    errcode_t ret;
    crypto_hash_type *hash_type;
    uint32_t hash_len;

    drv_err_chk_return(hash_handle == 0, ERRCODE_SECURITY_INVALID_PARAM, "hash_handle is NULL\n");
    drv_err_chk_return(out == NULL, ERRCODE_SECURITY_INVALID_PARAM, "out is NULL\n");
    drv_err_chk_return(out_len == NULL, ERRCODE_SECURITY_INVALID_PARAM, "out_len is NULL\n");

    hash_type = (crypto_hash_type *)(uintptr_t)(hash_handle);
    hash_len = crypto_hash_get_result_size(*hash_type) / CRYPTO_BITS_IN_BYTE;
    drv_err_chk_return(*out_len < hash_len, ERRCODE_SECURITY_INVALID_PARAM, "out_len is not enough\n");
    if (crypto_hash_is_hmac(*hash_type)) {
        ret = crypto_hmac_finish(hash_handle, out, hash_len);
        (void)crypto_hmac_destroy(hash_handle);
        *out_len = hash_len;
        return crypto_sl_common_get_errcode(ret);
    } else {
        ret = (errcode_t)drv_cipher_hash_compute_finish((void *)(uintptr_t)hash_handle, out, hash_len);
        (void)drv_cipher_hash_compute_teardown((void *)(uintptr_t)hash_handle);
        crypto_service_postprocess(HASH_MODULE_ID);
        *out_len = hash_len;
        return crypto_sl_common_get_errcode(ret);
    }
}

errcode_t uapi_drv_cipher_hash_get(uint32_t hash_handle, uapi_drv_cipher_hash_clone_ctx_t *hash_clone_ctx)
{
    int32_t ret;
    drv_hash_ctx *hash_ctx = NULL;

    drv_err_chk_return(hash_handle == 0, ERRCODE_SECURITY_INVALID_PARAM, "hash_handle is NULL\n");
    drv_err_chk_return(hash_clone_ctx == NULL, ERRCODE_SECURITY_INVALID_PARAM, "hash_clone_ctx is NULL\n");

    hash_ctx = (drv_hash_ctx *)(uintptr_t)hash_handle;
    if (crypto_hash_is_hmac(hash_ctx->hash_type)) {
        return crypto_sl_common_get_errcode(ERROR_UNSUPPORT);
    }

    ret = memcpy_s(hash_clone_ctx->tail, sizeof(hash_clone_ctx->tail), hash_ctx->buf, hash_ctx->tail_len);
    drv_err_chk_goto_with_ret(ret, ret != EOK, exit, ERROR_MEMCPY_S, "memcpy_s failed\n");

    ret = memcpy_s(hash_clone_ctx->length, sizeof(hash_clone_ctx->length), hash_ctx->length, sizeof(hash_ctx->length));
    drv_err_chk_goto_with_ret(ret, ret != EOK, exit, ERROR_MEMCPY_S, "memcpy_s failed\n");

    ret = memcpy_s(hash_clone_ctx->state, sizeof(hash_clone_ctx->state), hash_ctx->state, sizeof(hash_ctx->state));
    drv_err_chk_goto_with_ret(ret, ret != EOK, exit, ERROR_MEMCPY_S, "memcpy_s failed\n");

    hash_clone_ctx->tail_len = hash_ctx->tail_len;
    ret = CRYPTO_SUCCESS;
exit:
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_hash_set(uint32_t hash_handle, const uapi_drv_cipher_hash_clone_ctx_t *hash_clone_ctx)
{
    int32_t ret;
    drv_hash_ctx *hash_ctx = NULL;

    drv_err_chk_return(hash_handle == 0, ERRCODE_SECURITY_INVALID_PARAM, "hash_handle is NULL\n");
    drv_err_chk_return(hash_clone_ctx == NULL, ERRCODE_SECURITY_INVALID_PARAM, "hash_clone_ctx is NULL\n");

    hash_ctx = (drv_hash_ctx *)(uintptr_t)hash_handle;
    if (crypto_hash_is_hmac(hash_ctx->hash_type)) {
        return crypto_sl_common_get_errcode(ERROR_UNSUPPORT);
    }

    ret = memcpy_s(hash_ctx->buf, sizeof(hash_ctx->buf), hash_clone_ctx->tail, hash_clone_ctx->tail_len);
    drv_err_chk_goto_with_ret(ret, ret != EOK, exit, ERROR_MEMCPY_S, "memcpy_s failed\n");

    ret = memcpy_s(hash_ctx->length, sizeof(hash_ctx->length), hash_clone_ctx->length, sizeof(hash_clone_ctx->length));
    drv_err_chk_goto_with_ret(ret, ret != EOK, exit, ERROR_MEMCPY_S, "memcpy_s failed\n");

    ret = memcpy_s(hash_ctx->state, sizeof(hash_ctx->state), hash_clone_ctx->state, sizeof(hash_clone_ctx->state));
    drv_err_chk_goto_with_ret(ret, ret != EOK, exit, ERROR_MEMCPY_S, "memcpy_s failed\n");

    hash_ctx->tail_len = hash_clone_ctx->tail_len;
    ret = CRYPTO_SUCCESS;
exit:
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_hash_destroy(uint32_t hash_handle)
{
    crypto_unused(hash_handle);
    return ERRCODE_SUCC;
}

errcode_t uapi_drv_km_init(void)
{
    return ERRCODE_SUCC;
}

errcode_t uapi_drv_km_deinit(void)
{
    return ERRCODE_SUCC;
}

errcode_t uapi_drv_keyslot_create(uint32_t *keyslot_handle, uapi_drv_keyslot_type_t keyslot_type)
{
    return uapi_drv_keyslot_setup(keyslot_handle, keyslot_type);
}

errcode_t uapi_drv_keyslot_destroy(uint32_t keyslot_handle)
{
    return uapi_drv_keyslot_teardown(keyslot_handle);
}

typedef struct {
    uapi_drv_klad_dest_t klad_type;
    uint32_t keyslot_handle;
    uapi_drv_klad_attr_t attr;
} uapi_klad_ctx_t;

errcode_t uapi_drv_klad_create(uint32_t *klad_handle)
{
    int32_t ret;
    uapi_klad_ctx_t *ctx = NULL;

    drv_err_chk_return(klad_handle == NULL, ERRCODE_INVALID_PARAM, "klad_handle is NULL\n");

    ctx = crypto_malloc_coherent(sizeof(uapi_klad_ctx_t));
    drv_err_chk_goto_with_ret(ret, ctx == NULL, exit, ERROR_MALLOC, "malloc failed\n");

    *klad_handle = (uintptr_t)ctx;
    ret = CRYPTO_SUCCESS;
exit:
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_klad_destroy(uint32_t klad_handle)
{
    uapi_klad_ctx_t *ctx = (uapi_klad_ctx_t *)(uintptr_t)klad_handle;

    drv_err_chk_return(ctx == NULL, ERRCODE_INVALID_PARAM, "klad_handle is NULL\n");

    (void)memset_s(ctx, sizeof(uapi_klad_ctx_t), 0, sizeof(uapi_klad_ctx_t));
    crypto_free_coherent(ctx);
    return ERRCODE_SUCC;
}

errcode_t uapi_drv_klad_attach(uint32_t klad_handle, uapi_drv_klad_dest_t klad_type, uint32_t keyslot_handle)
{
    uapi_klad_ctx_t *ctx = (uapi_klad_ctx_t *)(uintptr_t)klad_handle;

    drv_err_chk_return(ctx == NULL, ERRCODE_INVALID_PARAM, "klad_handle is NULL\n");

    ctx->klad_type = klad_type;
    ctx->keyslot_handle = keyslot_handle;
    return ERRCODE_SUCC;
}

errcode_t uapi_drv_klad_detach(uint32_t klad_handle, uapi_drv_klad_dest_t klad_type, uint32_t keyslot_handle)
{
    uapi_klad_ctx_t *ctx = (uapi_klad_ctx_t *)(uintptr_t)klad_handle;

    drv_err_chk_return(ctx == NULL, ERRCODE_INVALID_PARAM, "klad_handle is NULL\n");
    crypto_unused(klad_type);
    crypto_unused(keyslot_handle);

    ctx->klad_type = UAPI_DRV_KLAD_DEST_MAX;
    ctx->keyslot_handle = 0;
    return ERRCODE_SUCC;
}

errcode_t uapi_drv_klad_set_attr(uint32_t klad_handle, const uapi_drv_klad_attr_t *attr)
{
    uapi_klad_ctx_t *ctx = (uapi_klad_ctx_t *)(uintptr_t)klad_handle;

    drv_err_chk_return(ctx == NULL, ERRCODE_INVALID_PARAM, "klad_handle is NULL\n");
    drv_err_chk_return(attr == NULL, ERRCODE_INVALID_PARAM, "attr is NULL\n");

    ctx->attr = *attr;
    return ERRCODE_SUCC;
}

errcode_t uapi_drv_klad_get_attr(uint32_t klad_handle, uapi_drv_klad_attr_t *attr)
{
    uapi_klad_ctx_t *ctx = (uapi_klad_ctx_t *)(uintptr_t)klad_handle;

    drv_err_chk_return(ctx == NULL, ERRCODE_INVALID_PARAM, "klad_handle is NULL\n");
    drv_err_chk_return(attr == NULL, ERRCODE_INVALID_PARAM, "attr is NULL\n");

    *attr = ctx->attr;
    return ERRCODE_SUCC;
}

errcode_t uapi_drv_klad_set_effective_key(uint32_t klad_handle, const uapi_drv_klad_effective_key_t *key)
{
    uapi_klad_ctx_t *ctx = (uapi_klad_ctx_t *)(uintptr_t)klad_handle;
    uint32_t key_len = CRYPTO_128_KEY_LEN;
    uapi_drv_keyslot_hard_key_type_t hard_key_type = UAPI_DRV_KEYSLOT_HARD_KEY_TYPE_USER_REE;

    drv_err_chk_return(ctx == NULL, ERRCODE_INVALID_PARAM, "klad_handle is NULL\n");
    drv_err_chk_return(key == NULL, ERRCODE_INVALID_PARAM, "key is NULL\n");
    if (key->key_size == UAPI_DRV_KLAD_KEY_SIZE_192BIT) {
        key_len = CRYPTO_192_KEY_LEN;
    } else if (key->key_size == UAPI_DRV_KLAD_KEY_SIZE_256BIT) {
        key_len = CRYPTO_256_KEY_LEN;
    }
    if (ctx->attr.klad_cfg.rootkey_type == UAPI_DRV_KDF_HARD_KEY_TYPE_RDRK_REE) {   /* for nv crypto. */
        hard_key_type = UAPI_DRV_KEYSLOT_HARD_KEY_TYPE_SEC_STORE;
    }

    return uapi_drv_keyslot_set_hard_key(ctx->keyslot_handle, hard_key_type,
        ctx->attr.key_cfg.engine, key->salt, key->salt_length, key_len);
}

errcode_t uapi_drv_klad_set_clear_key(uint32_t klad_handle, const uapi_drv_klad_clear_key_t *key)
{
    uapi_klad_ctx_t *ctx = (uapi_klad_ctx_t *)(uintptr_t)klad_handle;

    drv_err_chk_return(ctx == NULL, ERRCODE_INVALID_PARAM, "klad_handle is NULL\n");
    drv_err_chk_return(key == NULL, ERRCODE_INVALID_PARAM, "key is NULL\n");

    return uapi_drv_keyslot_set_clear_key(ctx->keyslot_handle, ctx->attr.key_cfg.engine, key->key, key->key_length);
}

errcode_t uapi_drv_kdf_update(uapi_drv_kdf_otp_key_t otp_key, uapi_drv_kdf_update_alg_t alg)
{
    crypto_unused(otp_key);
    crypto_unused(alg);

    return ERRCODE_SUCC;
}