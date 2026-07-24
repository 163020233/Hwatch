/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: crc implementation.
 *
 * Create: 2025-04-23
 */

#include "hal_symc.h"
#include "crypto_drv_common.h"
#include "drv_symc.h"

#define FILE_ID FILE_ID_DRV_CODE_SYMC_DRV_SYMC_CRC

typedef struct {
    crypto_symc_work_mode crc_work_mode;
    uint32_t polynomial;
    uint32_t intial_value;
    uint32_t result_xor_value;
    bool is_input_reverse;
    bool is_output_reverse;
} crc_config;

#define SYMC_COMPAT_ERRNO(err_code)     DRV_COMPAT_ERRNO(ERROR_MODULE_SYMC, err_code)
#define symc_null_ptr_chk(ptr)   \
    drv_err_chk_return((ptr) == TD_NULL, SYMC_COMPAT_ERRNO(ERROR_PARAM_IS_NULL), #ptr" is NULL\n")

const crc_config crc_config_table[] = {
    {CRYPTO_SYMC_WORK_MODE_CRC16, 0x1021, 0, 0, false, false},    /* CRC-16/XMODEM */
    {CRYPTO_SYMC_WORK_MODE_CRC32, 0x04C11DB7, 0xFFFFFFFF, 0xFFFFFFFF, true, true},    /* CRC-32 */
};

void reverse_bits_in_byte(uint8_t input_data, uint8_t *out_data)
{
    uint8_t result = 0;
    for (int i = 0; i < CRYPTO_BITS_IN_BYTE; i++) {
        if ((input_data & (1 << i)) != 0) {
            result |= (1 << (CRYPTO_BITS_IN_BYTE - 1 - i));
        }
    }
    *out_data = result;
}

/* reverse the whole data by bits. */
int reverse_stream_bits(uint8_t *data, uint32_t data_len)
{
    uint8_t temp_val[CRC32_RESULT_SIZE] = {0};
    int32_t ret = memcpy_s(temp_val, CRC32_RESULT_SIZE, data, data_len);
    drv_err_chk_return(ret != EOK, SYMC_COMPAT_ERRNO(ERROR_MEMCPY_S), "memcpy_s failed!\r\n");
    /* reserve the whole result. */
    for (uint32_t i = 0; i < data_len; i++) {
        reverse_bits_in_byte(temp_val[i], &data[data_len - i - 1]);
        crypto_log_dbg("temp_val 0x%x, data 0x%x\r\n", temp_val[i], data[data_len - i - 1]);
    }
    return CRYPTO_SUCCESS;
}

int drv_symc_crc_get_initial_value(drv_crc_ctx_t *crc_ctx)
{
    int ret;
    uint32_t crc_iv[4] = {0};
    uint32_t i = 0;
    for (; i < sizeof(crc_config_table) / sizeof(crc_config); i++) {
        if (crc_ctx->mode == crc_config_table[i].crc_work_mode) {
            crc_iv[0] = crc_config_table[i].intial_value;
            crc_iv[1] = crc_config_table[i].polynomial;
            crc_iv[3] = crc_config_table[i].is_input_reverse;   /* 3: index 3, which store the input_reverse value. */
            crc_ctx->is_output_reverse = crc_config_table[i].is_output_reverse;
            crc_ctx->result_xor_value = crc_config_table[i].result_xor_value;
            break;
        }
    }

    drv_err_chk_return((i == sizeof(crc_config_table) / sizeof(crc_config)), SYMC_COMPAT_ERRNO(ERROR_INVALID_PARAM),
        "wrong crc mode!\r\n");

    ret = memcpy_s(crc_ctx->iv, sizeof(crc_ctx->iv), crc_iv, sizeof(crc_iv));
    drv_err_chk_return(ret != EOK, SYMC_COMPAT_ERRNO(ERROR_MEMCPY_S), "memcpy_s failed!\r\n");
    crypto_dump_data("iv", crc_ctx->iv, sizeof(crc_ctx->iv));

    return CRYPTO_SUCCESS;
}

int drv_symc_crc_post_process(const drv_crc_ctx_t *crc_ctx, unsigned char *out_data, unsigned int out_len)
{
    int32_t ret;
    uint8_t *xor_val = (uint8_t *)&(crc_ctx->result_xor_value); /* 4: crc result max is 4 bytes. */

    /* step1. reverse the output value */
    if (crc_ctx->is_output_reverse) {
        ret = reverse_stream_bits(out_data, out_len);
        drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "reverse stream bit failed\r\n");
    }
    crypto_dump_data("out_data", out_data, out_len);
    crypto_dump_data("xor_val", xor_val, sizeof(crc_ctx->result_xor_value));

    /* step2. result data xor. */
    for (int i = 0; i < (int)out_len; i++) {
        out_data[i] ^= xor_val[i];
    }
    crypto_dump_data("out_data", out_data, out_len);

    return CRYPTO_SUCCESS;
}

int drv_symc_crc_start(void **ctx, crypto_symc_work_mode mode)
{
    int ret;
    drv_crc_ctx_t *crc_ctx = NULL;
    symc_null_ptr_chk(ctx);
    drv_err_chk_return((mode != CRYPTO_SYMC_WORK_MODE_CRC16 && mode != CRYPTO_SYMC_WORK_MODE_CRC32),
        SYMC_COMPAT_ERRNO(ERROR_INVALID_PARAM), "wrong crc mode.\r\n");

    crc_ctx = (drv_crc_ctx_t *)crypto_malloc_coherent(sizeof(drv_crc_ctx_t));
    drv_err_chk_return(crc_ctx == NULL, SYMC_COMPAT_ERRNO(ERROR_MALLOC), "malloc failed\r\n");

    /* set initial value. */
    crc_ctx->mode = mode;
    crc_ctx->state_len = mode == CRYPTO_SYMC_WORK_MODE_CRC16 ? CRC16_RESULT_SIZE : CRC32_RESULT_SIZE;
    ret = drv_symc_crc_get_initial_value(crc_ctx);
    drv_err_chk_goto(ret != CRYPTO_SUCCESS, free_exit, "get crc initial iv failed!\r\n");

    *ctx = (void *)crc_ctx;
    return CRYPTO_SUCCESS;
free_exit:
    crypto_free_coherent(crc_ctx);
    return ret;
}

int drv_symc_crc_update(void *ctx, const unsigned char *data, unsigned int data_len)
{
    int ret;
    drv_crc_ctx_t *crc_ctx = (drv_crc_ctx_t *)ctx;
    symc_null_ptr_chk(crc_ctx);

    ret = hal_cipher_symc_crypt(CRYPTO_SYMC_ALG_CRC, crc_ctx->mode, CRYPTO_SYMC_BIT_WIDTH_128BIT,
        crypto_get_phys_addr(data), 0, data_len, crc_ctx->iv, NULL, 0, 0, true);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "calculate crc failed!\r\n");

    return CRYPTO_SUCCESS;
}

int drv_symc_crc_finish(void *ctx, unsigned char *out_crc, unsigned int out_crc_len)
{
    int ret;
    drv_crc_ctx_t *crc_ctx = (drv_crc_ctx_t *)ctx;

    symc_null_ptr_chk(ctx);
    symc_null_ptr_chk(out_crc);
    drv_err_chk_return(crc_ctx->mode == CRYPTO_SYMC_WORK_MODE_CRC16 && out_crc_len < CRC16_RESULT_SIZE,
        SYMC_COMPAT_ERRNO(ERROR_INVALID_PARAM));
    drv_err_chk_return(crc_ctx->mode == CRYPTO_SYMC_WORK_MODE_CRC32 && out_crc_len < CRC32_RESULT_SIZE,
        SYMC_COMPAT_ERRNO(ERROR_INVALID_PARAM));

    /* step1. get result from state. */
    ret = memcpy_s(out_crc, out_crc_len, crc_ctx->iv, crc_ctx->state_len);
    drv_err_chk_return(ret != EOK, SYMC_COMPAT_ERRNO(ERROR_MEMCPY_S), "memcyp_s failed!\r\n");

    /* step2. post process. only last part need postprocess. */
    ret = drv_symc_crc_post_process(crc_ctx, out_crc, crc_ctx->state_len);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "post process failed!\r\n");

    return ret;
}

int drv_symc_crc_destroy(void *ctx)
{
    symc_null_ptr_chk(ctx);
    (void)memset_s(ctx, sizeof(drv_crc_ctx_t), 0, sizeof(drv_crc_ctx_t));
    crypto_free_coherent(ctx);
    return CRYPTO_SUCCESS;
}

int drv_symc_crc_compute(crypto_symc_work_mode mode, const unsigned char *data, unsigned int data_len,
    unsigned char *out_crc, unsigned int out_crc_len)
{
    int ret;
    drv_crc_ctx_t crc_ctx;

    symc_null_ptr_chk(out_crc);
    drv_err_chk_return(mode != CRYPTO_SYMC_WORK_MODE_CRC16 && mode != CRYPTO_SYMC_WORK_MODE_CRC32,
        SYMC_COMPAT_ERRNO(ERROR_INVALID_PARAM));
    drv_err_chk_return(mode == CRYPTO_SYMC_WORK_MODE_CRC16 && out_crc_len < CRC16_RESULT_SIZE,
        SYMC_COMPAT_ERRNO(ERROR_INVALID_PARAM));
    drv_err_chk_return(mode == CRYPTO_SYMC_WORK_MODE_CRC32 && out_crc_len < CRC32_RESULT_SIZE,
        SYMC_COMPAT_ERRNO(ERROR_INVALID_PARAM));

    /* step1. get intial config value. */
    crc_ctx.mode = mode;
    crc_ctx.state_len = mode == CRYPTO_SYMC_WORK_MODE_CRC16 ? CRC16_RESULT_SIZE : CRC32_RESULT_SIZE;
    ret = drv_symc_crc_get_initial_value(&crc_ctx);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "get crc iv failed!\r\n");

    /* step2. start calculate. */
    ret = hal_cipher_symc_crypt(CRYPTO_SYMC_ALG_CRC, mode, CRYPTO_SYMC_BIT_WIDTH_128BIT, crypto_get_phys_addr(data), 0,
        data_len, crc_ctx.iv, NULL, 0, 0, true);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "calculate crc failed!\r\n");
    crypto_dump_data("out iv", crc_ctx.iv, sizeof(crc_ctx.iv));

    /* step3. get result. */
    ret = memcpy_s(out_crc, out_crc_len, crc_ctx.iv, crc_ctx.state_len);
    drv_err_chk_return(ret != EOK, SYMC_COMPAT_ERRNO(ERROR_MEMCPY_S), "memcyp_s failed!\r\n");
    crypto_dump_data("out iv", crc_ctx.iv, crc_ctx.state_len);
    crypto_dump_data("out_crc", out_crc, crc_ctx.state_len);

    /* step4. post process. */
    ret = drv_symc_crc_post_process(&crc_ctx, out_crc, crc_ctx.state_len);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "post process failed!\r\n");
    crypto_dump_data("out_crc", out_crc, crc_ctx.state_len);

    return CRYPTO_SUCCESS;
}