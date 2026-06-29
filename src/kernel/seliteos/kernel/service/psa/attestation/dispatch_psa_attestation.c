/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: attestation api. \n
 *
 * History: \n
 * 2025-03-14, Create file. \n
 */

#include "dispatch_psa_attestation.h"
#include "attes_common.h"
#include "cose_sign.h"

#include "chips_porting.h"
#include "qcbor/qcbor_encode.h"
#include "sec_storage.h"
#include "securec.h"
#include "los_base.h"
#include "los_mem.h"
#include "pmp_mgr.h"
#include "memory_config_flash.h"
#include "memory_config_common.h"
#include "unified_cipher_pke.h"
#include "security_hash.h"
#include "trng.h"
#include "product.h"

#define IAT_NONCE                          10  /* EAT nonce */
#define IAT_INSTANCE_ID                    256 /* EAT ueid */
#define IAT_PROFILE_DEFINITION             265 /* EAT eat_profile */
#define IAT_BOOT_SEED                      268 /* EAT bootseed */
#define IAT_PLATFORM_STATE                 269 /* EAT platform state */
#define IAT_HARDWARE_VERSION               270 /* EAT hardware version */
#define IAT_REE_SW_VERSION                 271 /* EAT ree sw version */
#define IAT_TEE_SW_VERSION                 272 /* EAT tee sw version */
#define IAT_ARM_RANGE_BASE                 (2393)
#define IAT_CLIENT_ID                      (IAT_ARM_RANGE_BASE + 1)
#define IAT_SECURITY_LIFECYCLE             (IAT_ARM_RANGE_BASE + 2)
#define IAT_IMPLEMENTATION_ID              (IAT_ARM_RANGE_BASE + 3)
#define IAT_SW_COMPONENTS                  (IAT_ARM_RANGE_BASE + 6)
#define IAT_SW_COMPONENT_MEASUREMENT_TYPE  (1)
#define IAT_SW_COMPONENT_MEASUREMENT_VALUE (2)
/* Reserved                                (3) */
#define IAT_SW_COMPONENT_VERSION           (4)

#define INSTANCE_ID_LEN 32
#define IMPLEMENTATION_ID_LEN 32
#define BOOTSEED_LEN 8
#define SECURITY_LIFECYCLE_LEN 8

uint8_t g_bootseed[BOOTSEED_LEN];

uint32_t g_sign_alg = 0;

void psa_attes_get_bootseed(void)
{
    int32_t ret;

    ret = uapi_drv_cipher_trng_get_random_bytes(g_bootseed, BOOTSEED_LEN);
    if (ret != ERRCODE_SUCC) {
        PRINT_ERR("get attes bootseed failed, ret is %d\n", ret);
    }
}

static int32_t inner_attest_add_iat_nonce_claim(struct attest_token_encode_ctx *ctx, useful_buf *challenge)
{
    useful_bufc challeng_c = {challenge->ptr, challenge->len};

    QCBOREncode_AddBytesToMapN(&(ctx->cbor_enc_ctx), IAT_NONCE, challeng_c);
    return ERRCODE_SUCC;
}

static int32_t inner_attest_add_caller_id_claim(struct attest_token_encode_ctx *ctx)
{
    /* negative values represent callers from the NSPE and positive values represent callers from the SPE */
    QCBOREncode_AddInt64ToMapN(&(ctx->cbor_enc_ctx), IAT_CLIENT_ID, -1);
    return ERRCODE_SUCC;
}

static int32_t inner_attest_add_instance_id_claim(struct attest_token_encode_ctx *ctx)
{
    int32_t ret;
    uint16_t sec_storage_return_len;
    uint8_t instance_id[INSTANCE_ID_LEN];
    useful_bufc claim_value;

    ret = uapi_sec_storage_read(INSTANCE_ID_SEC_STORAGE_ID, INSTANCE_ID_LEN, &sec_storage_return_len, instance_id);
    if (ret != ERRCODE_SUCC) {
        attes_print("uapi_sec_storage_read failed!\n");
        return ret;
    }

    if (sec_storage_return_len != INSTANCE_ID_LEN) {
        attes_print("sec_storage_read instance id length invalid!\n");
        return ERRCODE_FAIL;
    }

    claim_value.ptr = instance_id;
    claim_value.len = INSTANCE_ID_LEN;
    QCBOREncode_AddBytesToMapN(&(ctx->cbor_enc_ctx), IAT_INSTANCE_ID, claim_value);
    return ERRCODE_SUCC;
}

static int32_t inner_attest_add_implementation_id_claim(struct attest_token_encode_ctx *ctx)
{
    int32_t ret;
    uint8_t die_id[IMPLEMENTATION_ID_LEN] = {0};
    useful_bufc claim_value;

    ret = tee_efuse_get_die_id(die_id, IMPLEMENTATION_ID_LEN);
    if (ret != ERRCODE_SUCC) {
        attes_print("tee_efuse_get_die_id failed!\n");
        return ret;
    }

    claim_value.ptr = die_id;
    claim_value.len = IMPLEMENTATION_ID_LEN;
    QCBOREncode_AddBytesToMapN(&(ctx->cbor_enc_ctx), IAT_IMPLEMENTATION_ID, claim_value);
    return ret;
}

#define HISI_LIFECYCLE_EMPTY                    0x0000
#define HISI_LIFECYCLE_FACTORY                  0x0002
#define HISI_LIFECYCLE_PRODUCT                  0x0006
#define HISI_LIFECYCLE_DECOMMISSIONED           0x000e

#define PSA_LIFECYCLE_UNKNOWN                   0x0000
#define PSA_LIFECYCLE_ASSEMBLY_AND_TEST         0x1000
#define PSA_LIFECYCLE_PSA_ROT_PROVISIONING      0x2000
#define PSA_LIFECYCLE_SECURED                   0x3000
#define PSA_LIFECYCLE_NON_PSA_ROT_DEBUG         0x4000
#define PSA_LIFECYCLE_RECOVERABLE_PSA_ROT_DEBUG 0x5000
#define PSA_LIFECYCLE_DECOMMISSIONED            0x6000

typedef struct lifecycle {
    uint32_t hisi_lifecycle;
    uint32_t prot_lifecycle;
} lifecycle_t;

lifecycle_t g_lifecycle_table[] = {
    {HISI_LIFECYCLE_EMPTY, PSA_LIFECYCLE_ASSEMBLY_AND_TEST},
    {HISI_LIFECYCLE_FACTORY, PSA_LIFECYCLE_PSA_ROT_PROVISIONING},
    {HISI_LIFECYCLE_PRODUCT, PSA_LIFECYCLE_SECURED},
    {HISI_LIFECYCLE_DECOMMISSIONED, PSA_LIFECYCLE_DECOMMISSIONED},
};

static int32_t inner_attest_add_security_lifecycle_claim(struct attest_token_encode_ctx *ctx)
{
    int32_t ret;
    uint32_t i;
    uint64_t lifecycle = 0;
    uint32_t hisi_lifecycle = 0;
    uint32_t prot_lifecycle = PSA_LIFECYCLE_UNKNOWN;
    useful_bufc claim_value;

    /* get hisi lifecycle from efuse */
    ret = tee_efuse_get_lifecycle((uint8_t *)&hisi_lifecycle);
    if (ret != ERRCODE_SUCC) {
        PRINT_ERR("tee_efuse_get_lifecycle failed!\n");
        return ret;
    }

    /* get prot lifecycle */
    for (i = 0; i < sizeof(g_lifecycle_table) / sizeof(g_lifecycle_table[0]); i++) {
        if (g_lifecycle_table[i].hisi_lifecycle == hisi_lifecycle) {
            prot_lifecycle = g_lifecycle_table[i].prot_lifecycle;
            break;
        }
    }

    /* PSA lifecycle = prot_lifecycle[15:8] || hisi_lifecycle[7:0] */
    lifecycle = prot_lifecycle;
    lifecycle = (lifecycle << 32) | (hisi_lifecycle); /* 32: prot lifecycle is high 32 bits */

    claim_value.ptr = &lifecycle;
    claim_value.len = SECURITY_LIFECYCLE_LEN;
    QCBOREncode_AddBytesToMapN(&(ctx->cbor_enc_ctx), IAT_SECURITY_LIFECYCLE, claim_value);
    return ret;
}

static int32_t inner_attest_add_boot_seed_claim(struct attest_token_encode_ctx *ctx)
{
    useful_bufc claim_value;

    claim_value.ptr = g_bootseed;
    claim_value.len = BOOTSEED_LEN;
    QCBOREncode_AddBytesToMapN(&(ctx->cbor_enc_ctx), IAT_BOOT_SEED, claim_value);
    return ERRCODE_SUCC;
}

#define PLATFORM_NORMAL_STATE   0
#define PLATFORM_ABNORMAL_STATE 1

static int32_t inner_attest_add_platform_state(struct attest_token_encode_ctx *ctx)
{
    useful_bufc platform_state;
    uint32_t state = PLATFORM_NORMAL_STATE;

    platform_state.ptr = &state;
    platform_state.len = sizeof(state);
    QCBOREncode_AddBytesToMapN(&(ctx->cbor_enc_ctx), IAT_PLATFORM_STATE, platform_state);
    return ERRCODE_SUCC;
}

#define CHIP_VERSION_LEN 6

static int32_t inner_attest_add_hardware_version_claim(struct attest_token_encode_ctx *ctx)
{
    int32_t ret;
    uint8_t buffer[CHIP_VERSION_LEN];
    useful_bufc chip_version;

    ret = tee_efuse_get_chip_version(buffer, sizeof(buffer));
    if (ret != ERRCODE_SUCC) {
        PRINT_ERR("tee_efuse_get_chip_version failed!\n");
        return ret;
    }

    chip_version.ptr = buffer;
    chip_version.len = sizeof(buffer);
    QCBOREncode_AddBytesToMapN(&(ctx->cbor_enc_ctx), IAT_HARDWARE_VERSION, chip_version);
    return ERRCODE_SUCC;
}

static int32_t inner_attest_add_ree_sw_version_claim(struct attest_token_encode_ctx *ctx)
{
    const char *version = APPLICATION_VERSION_STRING;
    useful_bufc ree_sw_version;

    ree_sw_version.ptr = version;
    ree_sw_version.len = strlen(version);
    QCBOREncode_AddBytesToMapN(&(ctx->cbor_enc_ctx), IAT_REE_SW_VERSION, ree_sw_version);
    return ERRCODE_SUCC;
}

static int32_t inner_attest_add_tee_sw_version_claim(struct attest_token_encode_ctx *ctx)
{
    const char *version = TEE_VERSION_STRING;
    useful_bufc tee_sw_version;

    tee_sw_version.ptr = version;
    tee_sw_version.len = strlen(version);
    QCBOREncode_AddBytesToMapN(&(ctx->cbor_enc_ctx), IAT_TEE_SW_VERSION, tee_sw_version);
    return ERRCODE_SUCC;
}

static void attest_encode_sw_component(QCBOREncodeContext *encode_ctx,
                                       const struct q_useful_buf_c *sw_version,
                                       const struct q_useful_buf_c *sw_type,
                                       const struct q_useful_buf_c *measurement)
{
    QCBOREncode_OpenMap(encode_ctx);

    /* Encode component version as text string. */
    QCBOREncode_AddTextToMapN(encode_ctx, IAT_SW_COMPONENT_VERSION, *sw_version);

    /* Encode software component type as text string. */
    QCBOREncode_AddTextToMapN(encode_ctx, IAT_SW_COMPONENT_MEASUREMENT_TYPE, *sw_type);

    /* Encode measurement value as byte string. */
    QCBOREncode_AddBytesToMapN(encode_ctx, IAT_SW_COMPONENT_MEASUREMENT_VALUE, *measurement);

    QCBOREncode_CloseMap(encode_ctx);
}

sw_components_t g_sw_components[] = {
    {"BL", SSB_FLASH_REGION_START, 0, 0, {0}},
    {"App", APP_FLASH_REGION_START, 0, 0, {0}},
    {"TS", SELITEOS_FLASH_REGION_START, 0, 0, {0}},
};

static int32_t inner_attest_add_all_sw_components(struct attest_token_encode_ctx *ctx)
{
    uint32_t i;
    int32_t ret;
    useful_bufc sw_value_claim_value;
    useful_bufc sw_version_claim_value;
    useful_bufc sw_type_claim_value;

    QCBOREncode_OpenArrayInMapN(&(ctx->cbor_enc_ctx), IAT_SW_COMPONENTS);

    for (i = 0; i < psa_attes_array_size(g_sw_components); i++) {
        /* get version */
        g_sw_components[i].version = *(uint32_t *)(g_sw_components[i].flash_addr + CODE_VERSION_OFFSET);
        g_sw_components[i].version_mask = *(uint32_t *)(g_sw_components[i].flash_addr + CODE_VERSION_MASK_OFFSET);
        g_sw_components[i].version &= g_sw_components[i].version_mask;
        sw_version_claim_value.ptr = &g_sw_components[i].version;
        sw_version_claim_value.len = sizeof(g_sw_components[i].version);

        /* get type */
        sw_type_claim_value.ptr = g_sw_components[i].type;
        sw_type_claim_value.len = strlen(g_sw_components[i].type) + 1;

        /* get value */
        ret = memcpy_s(g_sw_components[i].hash, CODE_HASH_LEN,
            (uint8_t *)(g_sw_components[i].flash_addr + CODE_HASH_OFFSET), CODE_HASH_LEN);
        attes_chk_return(ret != EOK, ret, "memcpy_s failed!\n");
        sw_value_claim_value.ptr = g_sw_components[i].hash;
        sw_value_claim_value.len = CODE_HASH_LEN;

        /* encode */
        attest_encode_sw_component(&(ctx->cbor_enc_ctx), &sw_version_claim_value, &sw_type_claim_value,
            &sw_value_claim_value);
    }

    QCBOREncode_CloseArray(&(ctx->cbor_enc_ctx));
    return ERRCODE_SUCC;
}

static int32_t attest_token_encode_start(struct attest_token_encode_ctx *me, useful_buf *out_buf)
{
    int32_t ret;

    /* Spin up the CBOR encoder */
    QCBOREncode_Init(&(me->cbor_enc_ctx), *out_buf);
    QCBOREncode_OpenArray(&(me->cbor_enc_ctx));

    /* QCBOREncode_AddBytes: add protected && unprotected parameters to cbor_enc_ctx */
    ret = cose_sign_encode_parameters(&(me->signer_ctx), &(me->cbor_enc_ctx));
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    return ret;
}

static int32_t attest_token_encode_finish(struct attest_token_encode_ctx *me, useful_bufc *signed_payload,
    useful_buf *completed_token)
{
    int32_t ret;
    QCBORError qcbor_ret;

    /* do COSE_Sign1 */
    ret = cose_sign_encode_signature(&(me->signer_ctx), &(me->cbor_enc_ctx), signed_payload);
    if (ret != ERRCODE_SUCC) {
        attes_print("cose_sign_encode_signature error\n");
        return ret;
    }

    /* Finally close off the CBOR formatting and get the pointer and length of the resulting COSE_Sign */
    QCBOREncode_CloseArray(&(me->cbor_enc_ctx));
    qcbor_ret = QCBOREncode_Finish(&(me->cbor_enc_ctx), (UsefulBufC *)completed_token);
    if (qcbor_ret != QCBOR_SUCCESS) {
        attes_print("QCBOREncode_Finish error\n");
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

static int32_t inner_attest_create_token(useful_buf *challenge, useful_buf *token, useful_buf *completed_token)
{
    int32_t ret;
    useful_bufc signed_payload;
    struct attest_token_encode_ctx attest_token_ctx;

    attest_token_ctx.signer_ctx.sign_algorithm = g_sign_alg;

    /* create token: init CBOR, encode COSE headers (protected && unprotected parameters) */
    ret = attest_token_encode_start(&attest_token_ctx, token);
    attes_chk_return(ret != ERRCODE_SUCC, ret, "token encode start fail!");

    /* open map for body */
    QCBOREncode_BstrWrap(&attest_token_ctx.cbor_enc_ctx);
    QCBOREncode_OpenMap(&attest_token_ctx.cbor_enc_ctx);

    /* add token body(claim) */
    ret = inner_attest_add_iat_nonce_claim(&attest_token_ctx, challenge);  // 输入的随机数，至少32bit
    attes_chk_return(ret != ERRCODE_SUCC, ret, "inner_attest_add_iat_nonce_claim error!");
    ret = inner_attest_add_caller_id_claim(&attest_token_ctx); /* 负值表示来自NSPE的调用者，正值表示来自SPE的调用者 */
    attes_chk_return(ret != ERRCODE_SUCC, ret, "inner_attest_add_caller_id_claim error!");
    ret = inner_attest_add_instance_id_claim(&attest_token_ctx); /* NV */
    attes_chk_return(ret != ERRCODE_SUCC, ret, "inner_attest_add_instance_id_claim error!");
    ret = inner_attest_add_implementation_id_claim(&attest_token_ctx); /* efuse: DIEID */
    attes_chk_return(ret != ERRCODE_SUCC, ret, "inner_attest_add_implementation_id_claim error!");
    ret = inner_attest_add_security_lifecycle_claim(&attest_token_ctx); /* efuse: LifeCycle */
    attes_chk_return(ret != ERRCODE_SUCC, ret, "inner_attest_add_security_lifecycle_claim error!");
    ret = inner_attest_add_boot_seed_claim(&attest_token_ctx); /* 随机数 8 byte */
    attes_chk_return(ret != ERRCODE_SUCC, ret, "inner_attest_add_boot_seed_claim error!");
    ret = inner_attest_add_platform_state(&attest_token_ctx);
    attes_chk_return(ret != ERRCODE_SUCC, ret, "inner_attest_add_platform_state error!");
    ret = inner_attest_add_hardware_version_claim(&attest_token_ctx);
    attes_chk_return(ret != ERRCODE_SUCC, ret, "inner_attest_add_hardware_version_claim error!");
    ret = inner_attest_add_ree_sw_version_claim(&attest_token_ctx);
    attes_chk_return(ret != ERRCODE_SUCC, ret, "inner_attest_add_ree_sw_version_claim error!");
    ret = inner_attest_add_tee_sw_version_claim(&attest_token_ctx);
    attes_chk_return(ret != ERRCODE_SUCC, ret, "inner_attest_add_tee_sw_version_claim error!");

    ret = inner_attest_add_all_sw_components(&attest_token_ctx); /* BOOT & LiteOS & SELiteOS HASH */
    attes_chk_return(ret != ERRCODE_SUCC, ret, "inner_attest_add_all_sw_components error!");

    /* close map for body, get output signed_payload */
    QCBOREncode_CloseMap(&attest_token_ctx.cbor_enc_ctx);
    QCBOREncode_CloseBstrWrap(&attest_token_ctx.cbor_enc_ctx, &signed_payload);

    /* The payload used for calculating the hash does not include the first three encodings */
    signed_payload.ptr = (uint8_t *)((uintptr_t)signed_payload.ptr + 3);  /* 3: the first two encodings */
    signed_payload.len = signed_payload.len - 3;  /* 3: the first two encodings */

    /* get token: do signature, deinit CBOR and get CBOR output */
    ret = attest_token_encode_finish(&attest_token_ctx, &signed_payload, completed_token);
    attes_chk_return(ret != ERRCODE_SUCC, ret, "token encode finish fail!");

    return ERRCODE_SUCC;
}

#define CHALLENGE_SIZE_32_BYTE 32

int32_t dispatch_psa_attest_get_token(uint32_t cmd, void *arg)
{
    psa_attestation_get_token_arg_t *token_ctl = arg;
    useful_buf challenge;
    useful_buf token;
    useful_buf completed_token;
    uint32_t total_len;
    uint8_t *data_buffer;
    int32_t ret;

    /* check param valid */
    attes_chk_return(cmd != CMD_PSA_ATTESTATION_GET_TOKEN, ERRCODE_INVALID_PARAM, "cmd is invalid!\n");
    attes_chk_return((token_ctl->auth_challenge == NULL || token_ctl->token_buf == NULL ||
        token_ctl->token_size == NULL), PSA_ERROR_INVALID_ARGUMENT, "param is NULL!\n");
    attes_chk_return(token_ctl->challenge_size != CHALLENGE_SIZE_32_BYTE, PSA_ERROR_INVALID_ARGUMENT,
        "invalid challenge size!");
    attes_chk_return(token_ctl->token_buf_size == 0, PSA_ERROR_INVALID_ARGUMENT, "invalid token buffer size!");
    attes_chk_return(token_ctl->token_buf_size < token_ctl->challenge_size, PSA_ERROR_BUFFER_TOO_SMALL,
        "token buffer size too small!");

    /* check buffer valid */
    attes_chk_return(check_is_not_in_tee_range((uintptr_t)token_ctl->auth_challenge,
                                               token_ctl->challenge_size) != LOS_SUCCESS,
                     ERRCODE_INVALID_PARAM, "data range is invalid!\n");
    attes_chk_return(check_is_not_in_tee_range((uintptr_t)token_ctl->token_buf,
                                               token_ctl->token_buf_size) != LOS_SUCCESS,
                     ERRCODE_INVALID_PARAM, "data range is invalid!\n");
    attes_chk_return(check_is_not_in_tee_range((uintptr_t)token_ctl->token_size,
                                               sizeof(uint32_t)) != LOS_SUCCESS,
                     ERRCODE_INVALID_PARAM, "data range is invalid!\n");

    total_len = token_ctl->challenge_size + token_ctl->token_buf_size;
    data_buffer = los_kmalloc(total_len);
    attes_chk_return(data_buffer == NULL, ERRCODE_MALLOC, "los_kmalloc failed!\n");

    ret = memcpy_s(data_buffer, total_len, token_ctl->auth_challenge, token_ctl->challenge_size);
    attes_chk_goto(ret != EOK, free_buffer, "memcpy_s failed!\n");

    challenge.ptr = (void *)data_buffer;
    challenge.len = token_ctl->challenge_size;
    token.ptr = data_buffer + token_ctl->challenge_size;
    token.len = token_ctl->token_buf_size;

    /* create token */
    ret = inner_attest_create_token(&challenge, &token, &completed_token);
    attes_chk_goto(ret != ERRCODE_SUCC, free_buffer, "create token fail!");

    ret = memcpy_s(token_ctl->token_buf, token_ctl->token_buf_size, completed_token.ptr, completed_token.len);
    attes_chk_goto(ret != EOK, free_buffer, "memcpy_s failed!\n");

    *token_ctl->token_size = completed_token.len;

free_buffer:
    los_kfree(data_buffer);
    return ret;
}

int32_t dispatch_psa_attest_gen_key(uint32_t cmd, void *arg)
{
    psa_attestation_gen_key_arg_t *key_ctl = arg;
    uapi_drv_cipher_pke_ecc_curve_type_t curve_type;
    uapi_drv_cipher_pke_data_t output_priv_key;
    uapi_drv_cipher_pke_ecc_point_t output_pub_key;
    uint8_t private_key[COSE_KEY_MAX_SIZE];
    uint8_t public_key[COSE_KEY_MAX_SIZE * 2]; /* 2: x + y */
    uint8_t instance_id[INSTANCE_ID_LEN];
    uint32_t key_len;
    int32_t ret;

    /* check param valid */
    attes_chk_return(cmd != CMD_PSA_ATTESTATION_GEN_KEY, ERRCODE_INVALID_PARAM, "cmd is invalid!\n");
    attes_chk_return((key_ctl->alg == 0) || (key_ctl->alg >= COSE_ALGORITHM_MAX), ERRCODE_INVALID_PARAM,
        "sign algorithm invalid!\n");
    attes_chk_return(key_ctl->public_key == NULL || key_ctl->key_length == NULL, ERRCODE_INVALID_PARAM,
        "param is NULL!\n");

    /* check buffer valid */
    attes_chk_return(check_is_not_in_tee_range((uintptr_t)key_ctl->public_key, key_ctl->key_size) != LOS_SUCCESS,
        ERRCODE_INVALID_PARAM, "public_key data range is invalid!\n");
    attes_chk_return(check_is_not_in_tee_range((uintptr_t)key_ctl->key_length, sizeof(uint32_t)) != LOS_SUCCESS,
        ERRCODE_INVALID_PARAM, "key_length data range is invalid!\n");

    g_sign_alg = key_ctl->alg;

    if (key_ctl->alg == COSE_ALGORITHM_ES256) {
        curve_type = UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC5639_P256;
        key_len = ECC_BP256_KEY_LEN;
    } else if (key_ctl->alg == COSE_ALGORITHM_ES384) {
        curve_type = UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC5639_P384;
        key_len = ECC_BP384_KEY_LEN;
    } else {
        curve_type = UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC5639_P512;
        key_len = ECC_BP512_KEY_LEN;
    }

    /* gen key */
    output_priv_key.data = private_key;
    output_priv_key.length = key_len;
    output_pub_key.x = public_key;
    output_pub_key.y = public_key + key_len;
    output_pub_key.length = key_len;
    ret = uapi_drv_cipher_pke_ecc_gen_key(curve_type, NULL, &output_priv_key, &output_pub_key);
    attes_chk_return(ret != ERRCODE_SUCC, ret, "uapi_drv_cipher_pke_ecc_gen_key error!\n");

    /* store prive_key */
    ret = uapi_sec_storage_write(IAK_SEC_STORAGE_ID, private_key, key_len);
    attes_chk_goto(ret != ERRCODE_SUCC, clean_key, "uapi_sec_storage_write error!\n");

    /* calculate public_key hash, store instance_id */
    ret = uapi_drv_cipher_sha256(public_key, key_len * 2, instance_id); /* 2: x + y */
    attes_chk_goto(ret != ERRCODE_SUCC, clean_key, "uapi_drv_cipher_sha256 error!\n");

    ret = uapi_sec_storage_write(INSTANCE_ID_SEC_STORAGE_ID, instance_id, INSTANCE_ID_LEN);
    attes_chk_goto(ret != ERRCODE_SUCC, clean_key, "uapi_sec_storage_write error!\n");

    /* copy public_key */
    ret = memcpy_s(key_ctl->public_key, key_ctl->key_size, public_key, key_len * 2); /* 2: x + y */
    attes_chk_goto(ret != EOK, clean_key, "memcpy_s error!\n");

    *key_ctl->key_length = key_len * 2; /* 2: x + y */

clean_key:
    memset_s(private_key, COSE_KEY_MAX_SIZE, 0, COSE_KEY_MAX_SIZE);
    return ret;
}
