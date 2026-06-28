/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: sha384 simple API functions file.
 *
 * Create: 2024-07-02
*/

#include "sl_common.h"
#include "drv_hash.h"
#include "crypto_drv_common.h"
#include "security_hash.h"

#ifdef CONFIG_HASH_SUPPORT_SHA384
errcode_t uapi_drv_cipher_sha384_start(hash_handle_t *ctx)
{
    int32_t ret;
    crypto_service_preprocess(HASH_MODULE_ID);
    ret = drv_cipher_hash_compute_setup((void **)ctx);
    crypto_chk_goto(ret != ERRCODE_SUCC, exit_post_process, "hash setup failed\r\n");
    ret = drv_cipher_hash_compute_start((void *)(*ctx), CRYPTO_HASH_TYPE_SHA384);
    crypto_chk_goto(ret != ERRCODE_SUCC, exit_free, "hash start failed!\r\n");
    return crypto_sl_common_get_errcode(ret);

exit_free:
    drv_cipher_hash_compute_teardown((void *)(*ctx));
exit_post_process:
    crypto_service_postprocess(HASH_MODULE_ID);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_sha384_update(hash_handle_t ctx, const uint8_t *buf, uint32_t len)
{
    int32_t ret = drv_cipher_hash_compute_update((void *)ctx, buf, len);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_sha384_finish(hash_handle_t ctx, uint8_t out[48])
{
    int32_t ret = drv_cipher_hash_compute_finish((void *)ctx, out, HASH_SHA384_RESULT_SIZE);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_sha384(const uint8_t *buf, uint32_t len, uint8_t out[48])
{
    crypto_service_preprocess(HASH_MODULE_ID);
    int32_t ret = drv_cipher_hash_compute(CRYPTO_HASH_TYPE_SHA384, buf, len, out,
        HASH_SHA384_RESULT_SIZE);
    crypto_service_postprocess(HASH_MODULE_ID);
    return crypto_sl_common_get_errcode(ret);
}
#else
errcode_t uapi_drv_cipher_sha384_start(hash_handle_t *ctx)
{
    crypto_unused(ctx);
    return ERRCODE_SECURITY_UNSUPPORT;
}

errcode_t uapi_drv_cipher_sha384_update(hash_handle_t ctx, const uint8_t *buf, uint32_t len)
{
    crypto_unused(ctx);
    crypto_unused(buf);
    crypto_unused(len);
    return ERRCODE_SECURITY_UNSUPPORT;
}

errcode_t uapi_drv_cipher_sha384_finish(hash_handle_t ctx, uint8_t out[48])
{
    crypto_unused(ctx);
    crypto_unused(out);
    return ERRCODE_SECURITY_UNSUPPORT;
}

errcode_t uapi_drv_cipher_sha384(const uint8_t *buf, uint32_t len, uint8_t out[48])
{
    crypto_unused(buf);
    crypto_unused(len);
    crypto_unused(out);
    return ERRCODE_SECURITY_UNSUPPORT;
}
#endif  /* CONFIG_HASH_SUPPORT_SHA384 */