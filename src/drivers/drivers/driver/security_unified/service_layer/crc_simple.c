/**RSA
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: crc service header file.
 *
 * Create: 2025-05-07
 */

#include "crypto_symc_struct.h"
#include "crypto_drv_common.h"
#include "drv_symc.h"
#include "sl_common.h"
#include "security_crc.h"

#ifdef CONFIG_SYMC_SUPPORT_CRC16
errcode_t uapi_drv_cipher_crc16_compute(const uint8_t *data, uint32_t data_len, uint16_t *out_crc)
{
    crypto_service_preprocess(CRC_MODULE_ID);
    int32_t ret = drv_symc_crc_compute(CRYPTO_SYMC_WORK_MODE_CRC16, data, data_len, (unsigned char *)out_crc,
        sizeof(uint16_t));
    crypto_service_postprocess(CRC_MODULE_ID);

    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_crc16_start(crc_handle_t *ctx)
{
    crypto_service_preprocess(CRC_MODULE_ID);
    int32_t ret = drv_symc_crc_start((void **)ctx, CRYPTO_SYMC_WORK_MODE_CRC16);
    if (ret != CRYPTO_SUCCESS) {
        crypto_service_postprocess(CRC_MODULE_ID);
    }

    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_crc16_update(crc_handle_t ctx, const uint8_t *data, uint32_t data_len)
{
    int32_t ret = drv_symc_crc_update((void *)ctx, data, data_len);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_crc16_finish(crc_handle_t ctx, uint16_t *out_crc)
{
    int32_t ret = drv_symc_crc_finish((void *)ctx, (unsigned char *)out_crc, sizeof(uint16_t));
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_crc16_destroy(crc_handle_t ctx)
{
    int32_t ret = drv_symc_crc_destroy((void *)ctx);
    crypto_service_postprocess(CRC_MODULE_ID);

    return crypto_sl_common_get_errcode(ret);
}

#else
errcode_t uapi_drv_cipher_crc16_compute(const uint8_t *data, uint32_t data_len, uint16_t *out_crc)
{
    crypto_unused(data);
    crypto_unused(data_len);
    crypto_unused(out_crc);
    return ERRCODE_SECURITY_UNSUPPORT;
}

errcode_t uapi_drv_cipher_crc16_start(crc_handle_t *ctx)
{
    crypto_unused(ctx);
    return ERRCODE_SECURITY_UNSUPPORT;
}

errcode_t uapi_drv_cipher_crc16_update(crc_handle_t ctx, const uint8_t *data, uint32_t data_len)
{
    crypto_unused(ctx);
    crypto_unused(data);
    crypto_unused(data_len);
    return ERRCODE_SECURITY_UNSUPPORT;
}

errcode_t uapi_drv_cipher_crc16_finish(crc_handle_t ctx, uint16_t *out_crc)
{
    crypto_unused(ctx);
    crypto_unused(out_crc);
    return ERRCODE_SECURITY_UNSUPPORT;
}

errcode_t uapi_drv_cipher_crc16_destroy(crc_handle_t ctx)
{
    crypto_unused(ctx);
    return ERRCODE_SECURITY_UNSUPPORT;
}
#endif  /* CONFIG_SYMC_SUPPORT_CRC16 */

#ifdef CONFIG_SYMC_SUPPORT_CRC32
errcode_t uapi_drv_cipher_crc32_compute(const uint8_t *data, uint32_t data_len, uint32_t *out_crc)
{
    crypto_service_preprocess(CRC_MODULE_ID);
    int32_t ret = drv_symc_crc_compute(CRYPTO_SYMC_WORK_MODE_CRC32, data, data_len, (unsigned char *)out_crc,
        sizeof(uint32_t));
    crypto_service_postprocess(CRC_MODULE_ID);

    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_crc32_start(crc_handle_t *ctx)
{
    crypto_service_preprocess(CRC_MODULE_ID);
    int32_t ret = drv_symc_crc_start((void **)ctx, CRYPTO_SYMC_WORK_MODE_CRC32);
    if (ret != CRYPTO_SUCCESS) {
        crypto_service_postprocess(CRC_MODULE_ID);
    }

    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_crc32_update(crc_handle_t ctx, const uint8_t *data, uint32_t data_len)
{
    int32_t ret = drv_symc_crc_update((void *)ctx, data, data_len);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_crc32_finish(crc_handle_t ctx, uint32_t *out_crc)
{
    int32_t ret = drv_symc_crc_finish((void *)ctx, (unsigned char *)out_crc, sizeof(uint32_t));
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_crc32_destroy(crc_handle_t ctx)
{
    int32_t ret = drv_symc_crc_destroy((void *)ctx);
    crypto_service_postprocess(CRC_MODULE_ID);

    return crypto_sl_common_get_errcode(ret);
}

#else
errcode_t uapi_drv_cipher_crc32_compute(const uint8_t *data, uint32_t data_len, uint32_t *out_crc)
{
    crypto_unused(data);
    crypto_unused(data_len);
    crypto_unused(out_crc);
    return ERRCODE_SECURITY_UNSUPPORT;
}

errcode_t uapi_drv_cipher_crc32_start(crc_handle_t *ctx)
{
    crypto_unused(ctx);
    return ERRCODE_SECURITY_UNSUPPORT;
}

errcode_t uapi_drv_cipher_crc32_update(crc_handle_t ctx, const uint8_t *data, uint32_t data_len)
{
    crypto_unused(ctx);
    crypto_unused(data);
    crypto_unused(data_len);
    return ERRCODE_SECURITY_UNSUPPORT;
}

errcode_t uapi_drv_cipher_crc32_finish(crc_handle_t ctx, uint32_t *out_crc)
{
    crypto_unused(ctx);
    crypto_unused(out_crc);
    return ERRCODE_SECURITY_UNSUPPORT;
}

errcode_t uapi_drv_cipher_crc32_destroy(crc_handle_t ctx)
{
    crypto_unused(ctx);
    return ERRCODE_SECURITY_UNSUPPORT;
}
#endif  /* CONFIG_SYMC_SUPPORT_CRC32 */