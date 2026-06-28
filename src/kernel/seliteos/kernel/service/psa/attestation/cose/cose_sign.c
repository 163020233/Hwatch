/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: cose sign. \n
 *
 * History: \n
 * 2025-03-14, Create file. \n
 */

#include "cose_sign.h"
#include "dispatch_psa_attestation.h"
#include "security_hash.h"
#include "unified_cipher_pke.h"
#include "sec_storage.h"
#include "securec.h"

#include "qcbor/qcbor_encode.h"

#define COSE_HEADER_PARAM_ALG_LABEL 1

int32_t cose_sign_encode_parameters(struct cose_sign_ctx *me, QCBOREncodeContext *cbor_encode_ctx)
{
    QCBORError            qcbor_ret;
    QCBOREncodeContext    priv_cbor_encode_ctx;
    useful_buf            protected_parameters_buf_in = {me->protected_parameters, sizeof(me->protected_parameters)};

    /* add protected parameters */
    QCBOREncode_Init(&priv_cbor_encode_ctx, protected_parameters_buf_in);
    QCBOREncode_OpenMap(&priv_cbor_encode_ctx);
    QCBOREncode_AddInt64ToMapN(&priv_cbor_encode_ctx, COSE_HEADER_PARAM_ALG_LABEL, me->sign_algorithm);
    QCBOREncode_CloseMap(&priv_cbor_encode_ctx);
    qcbor_ret = QCBOREncode_Finish(&priv_cbor_encode_ctx, &me->protected_parameters_buf);
    attes_chk_return(qcbor_ret != QCBOR_SUCCESS, ERRCODE_FAIL, "QCBOREncode_Finish error!\n");
    QCBOREncode_AddBytes(cbor_encode_ctx, me->protected_parameters_buf);

    /* add unprotected parameters(if need) */
    return ERRCODE_SUCC;
}

static int32_t sha256_calcule(uint8_t *buf1, uint32_t buf1_len, uint8_t *buf2, uint32_t buf2_len, uint8_t *out,
    uint32_t *out_len)
{
    int32_t ret;
    hash_handle_t hash_ctx;

    ret = uapi_drv_cipher_sha256_start(&hash_ctx);
    attes_chk_return(ret != ERRCODE_SUCC, ret, "uapi_drv_cipher_sha256_start error\n");

    ret = uapi_drv_cipher_sha256_update(hash_ctx, buf1, buf1_len);
    attes_chk_goto(ret != ERRCODE_SUCC, destroy, "uapi_drv_cipher_sha256_update1 error\n");

    ret = uapi_drv_cipher_sha256_update(hash_ctx, buf2, buf2_len);
    attes_chk_goto(ret != ERRCODE_SUCC, destroy, "uapi_drv_cipher_sha256_update2 error\n");

    ret = uapi_drv_cipher_sha256_finish(hash_ctx, out);
    attes_chk_goto(ret != ERRCODE_SUCC, destroy, "uapi_drv_cipher_sha256_finish error\n");

    *out_len = SHA256_HASH_LEN;

destroy:
    (void)uapi_drv_cipher_sha_destroy(hash_ctx);
    return ret;
}

#ifdef PSA_ATTES_SHA384_SUPPORT
static int32_t sha384_calcule(uint8_t *buf1, uint32_t buf1_len, uint8_t *buf2, uint32_t buf2_len, uint8_t *out,
    uint32_t *out_len)
{
    int32_t ret;
    hash_handle_t hash_ctx;

    ret = uapi_drv_cipher_sha384_start(&hash_ctx);
    attes_chk_return(ret != ERRCODE_SUCC, ret, "uapi_drv_cipher_sha384_start error\n");

    ret = uapi_drv_cipher_sha384_update(hash_ctx, buf1, buf1_len);
    attes_chk_goto(ret != ERRCODE_SUCC, destroy, "uapi_drv_cipher_sha384_update1 error\n");

    ret = uapi_drv_cipher_sha384_update(hash_ctx, buf2, buf2_len);
    attes_chk_goto(ret != ERRCODE_SUCC, destroy, "uapi_drv_cipher_sha384_update2 error\n");

    ret = uapi_drv_cipher_sha384_finish(hash_ctx, out);
    attes_chk_goto(ret != ERRCODE_SUCC, destroy, "uapi_drv_cipher_sha384_finish error\n");

    *out_len = SHA384_HASH_LEN;

destroy:
    (void)uapi_drv_cipher_sha_destroy(hash_ctx);
    return ret;
}
#else
static int32_t sha384_calcule(uint8_t *buf1, uint32_t buf1_len, uint8_t *buf2, uint32_t buf2_len, uint8_t *out,
    uint32_t *out_len)
{
    attes_unused(buf1);
    attes_unused(buf1_len);
    attes_unused(buf2);
    attes_unused(buf2_len);
    attes_unused(out);
    attes_unused(out_len);
    return ERRCODE_NOT_SUPPORT;
}
#endif

#ifdef PSA_ATTES_SHA512_SUPPORT
static int32_t sha512_calcule(uint8_t *buf1, uint32_t buf1_len, uint8_t *buf2, uint32_t buf2_len, uint8_t *out,
    uint32_t *out_len)
{
    int32_t ret;
    hash_handle_t hash_ctx;

    ret = uapi_drv_cipher_sha512_start(&hash_ctx);
    attes_chk_return(ret != ERRCODE_SUCC, ret, "uapi_drv_cipher_sha512_start error\n");

    ret = uapi_drv_cipher_sha512_update(hash_ctx, buf1, buf1_len);
    attes_chk_goto(ret != ERRCODE_SUCC, destroy, "uapi_drv_cipher_sha512_update1 error\n");

    ret = uapi_drv_cipher_sha512_update(hash_ctx, buf2, buf2_len);
    attes_chk_goto(ret != ERRCODE_SUCC, destroy, "uapi_drv_cipher_sha512_update2 error\n");

    ret = uapi_drv_cipher_sha512_finish(hash_ctx, out);
    attes_chk_goto(ret != ERRCODE_SUCC, destroy, "uapi_drv_cipher_sha512_finish error\n");

    *out_len = SHA512_HASH_LEN;

destroy:
    (void)uapi_drv_cipher_sha_destroy(hash_ctx);
    return ret;
}
#else
static int32_t sha512_calcule(uint8_t *buf1, uint32_t buf1_len, uint8_t *buf2, uint32_t buf2_len, uint8_t *out,
    uint32_t *out_len)
{
    attes_unused(buf1);
    attes_unused(buf1_len);
    attes_unused(buf2);
    attes_unused(buf2_len);
    attes_unused(out);
    attes_unused(out_len);
    return ERRCODE_NOT_SUPPORT;
}
#endif

/*
 * Format of to-be-signed bytes used by create_tbs_hash().  This is
 * defined in COSE (RFC 8152) section 4.4. It is the input to the
 * hash.
 *
 * Sig_structure = [
 *    context : "Signature" / "Signature1" / "CounterSignature",
 *    body_protected : empty_or_serialized_map,
 *    ? sign_protected : empty_or_serialized_map, (not used for COSE_Sign1)
 *    external_aad : bstr,
 *    payload : bstr
 * ]
 *
 * body_protected refers to the protected parameters from the
 * main COSE_Sign1 structure. This is a little hard to
 * to understand in the spec.
 *
 * sign_protected is not used with COSE_Sign1 since there is no signer
 * chunk.
 *
 * external_aad allows external data to be covered by the hash, but is
 * not supported by this implementation.
 */

#define COSE_SIG_CONTEXT_STRING_SIGNATURE1 "Signature1"

/* opening the array + Signature1" + Overhead for encoding string + entire protected params +
    Empty bstr for absent external_aad + The max CBOR length encoding for start of payload */
#define COSE_SIZE_OF_TBS (1 + sizeof(COSE_SIG_CONTEXT_STRING_SIGNATURE1) + 2 + \
    COSE_SIGN1_MAX_SIZE_PROTECTED_PARAMETERS + 1 + 9)

static int32_t create_hash(int32_t          sign_algorithm,
                           useful_bufc      *protected_parameters,
                           useful_bufc      *sign_payload,
                           useful_buf       *buffer_for_hash)
{
    int32_t                     ret;
    QCBORError                  qcbor_ret;
    QCBOREncodeContext          cbor_encode_ctx;
    UsefulBuf_MAKE_STACK_UB(    tbs_first_part_buf_in, COSE_SIZE_OF_TBS);
    useful_bufc                 tbs_first_part_buf_out;

    QCBOREncode_Init(&cbor_encode_ctx, tbs_first_part_buf_in);
    QCBOREncode_OpenArray(&cbor_encode_ctx);

    /* context */
    QCBOREncode_AddSZString(&cbor_encode_ctx, COSE_SIG_CONTEXT_STRING_SIGNATURE1);
    /* body_protected */
    QCBOREncode_AddBytes(&cbor_encode_ctx, *protected_parameters);
    /* external_aad. (not used) */
    QCBOREncode_AddBytes(&cbor_encode_ctx, NULLUsefulBufC);
    /* payload. (Fake payload is the type and length of the wrapping bstr. It gets hashed with the first part) */
    QCBOREncode_AddBytesLenOnly(&cbor_encode_ctx, *sign_payload);

    QCBOREncode_CloseArray(&cbor_encode_ctx);
    qcbor_ret = QCBOREncode_Finish(&cbor_encode_ctx, &tbs_first_part_buf_out);
    if (qcbor_ret) {
        /* Mainly means that the protected_parameters were too big which should never happen */
        attes_print("QCBOREncode_Finish error, qcbor_ret is %u\n", qcbor_ret);
        ret = ERRCODE_FAIL;
        goto Done;
    }

    if (sign_algorithm == COSE_ALGORITHM_ES256) {
        ret = sha256_calcule((uint8_t *)tbs_first_part_buf_out.ptr, tbs_first_part_buf_out.len,
            (uint8_t *)sign_payload->ptr, sign_payload->len, (uint8_t *)buffer_for_hash->ptr, &buffer_for_hash->len);
    } else if (sign_algorithm == COSE_ALGORITHM_ES384) {
        ret = sha384_calcule((uint8_t *)tbs_first_part_buf_out.ptr, tbs_first_part_buf_out.len,
            (uint8_t *)sign_payload->ptr, sign_payload->len, (uint8_t *)buffer_for_hash->ptr, &buffer_for_hash->len);
    } else {
        ret = sha512_calcule((uint8_t *)tbs_first_part_buf_out.ptr, tbs_first_part_buf_out.len,
            (uint8_t *)sign_payload->ptr, sign_payload->len, (uint8_t *)buffer_for_hash->ptr, &buffer_for_hash->len);
    }

Done:
    return ret;
}

static int32_t create_sign(int32_t sign_algorithm, useful_bufc  *hash_to_sign, useful_buf *signature)
{
    uapi_drv_cipher_pke_ecc_curve_type_t curve_type;
    uapi_drv_cipher_pke_data_t priv_key;
    uapi_drv_cipher_pke_data_t hash;
    uapi_drv_cipher_pke_ecc_sig_t sig;
    UsefulBuf_MAKE_STACK_UB(buffer_for_priv_key, COSE_KEY_MAX_SIZE);
    uint32_t key_len;
    uint32_t key_len_for_check;
    uint32_t hash_len;
    uint32_t sign_len;
    int32_t ret;

    if (sign_algorithm == COSE_ALGORITHM_ES256) {
        curve_type = UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC5639_P256;
        key_len = SHA256_HASH_LEN;
        hash_len = ECC_BP256_KEY_LEN;
        sign_len = ECC_BP256_SIGN_LEN;
    } else if (sign_algorithm == COSE_ALGORITHM_ES384) {
        curve_type = UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC5639_P384;
        key_len = SHA384_HASH_LEN;
        hash_len = ECC_BP384_KEY_LEN;
        sign_len = ECC_BP384_SIGN_LEN;
    } else {
        curve_type = UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC5639_P512;
        key_len = SHA512_HASH_LEN;
        hash_len = ECC_BP512_KEY_LEN;
        sign_len = ECC_BP512_SIGN_LEN;
    }
    signature->len = sign_len * 2; /* 2: r + s */
    key_len_for_check = key_len;

    /* get priv_key from secure store */
    ret = uapi_sec_storage_read(IAK_SEC_STORAGE_ID, buffer_for_priv_key.len, (uint16_t *)&key_len,
                                (uint8_t *)buffer_for_priv_key.ptr);
    if ((ret != ERRCODE_SUCC) || (key_len != key_len_for_check)) {
        attes_print("uapi_sec_storage_read failed!\n");
        goto clean_key;
    }

    priv_key.data = (uint8_t *)((uintptr_t)buffer_for_priv_key.ptr);
    priv_key.length = key_len;
    hash.data = (uint8_t *)hash_to_sign->ptr;
    hash.length = hash_len;
    sig.r = (uint8_t *)signature->ptr;
    sig.s = (uint8_t *)((uintptr_t)signature->ptr + sign_len);
    sig.length = sign_len;
    ret = uapi_drv_cipher_pke_ecdsa_sign(curve_type, &priv_key, &hash, &sig);

clean_key:
    memset_s(buffer_for_priv_key.ptr, buffer_for_priv_key.len, 0, buffer_for_priv_key.len);
    return ret;
}

int32_t cose_sign_encode_signature(struct cose_sign_ctx *me, QCBOREncodeContext *cbor_encode_ctx,
    useful_bufc *signed_payload)
{
    int32_t ret;
    QCBORError cbor_err;
    USEFUL_BUF_MAKE_STACK_UB(buffer_for_signature, COSE_MAX_SIG_SIZE);
    USEFUL_BUF_MAKE_STACK_UB(buffer_for_tbs_hash, COSE_MAX_HASH_SIZE);

    /* Check that there are no CBOR encoding errors before proceeding
     * with hashing and signing. This is not actually necessary as the
     * errors will be caught correctly later, but it does make it a
     * bit easier for the caller to debug problems.
     */
    cbor_err = QCBOREncode_GetErrorState(cbor_encode_ctx);
    if (cbor_err != QCBOR_SUCCESS) {
        attes_print("QCBOREncode_GetErrorState error, cbor_err is %u\n", cbor_err);
        ret = ERRCODE_FAIL;
        goto Done;
    }

    /* Create the hash of the to-be-signed bytes. Inputs to the
     * hash are the protected parameters, the payload that is
     * getting signed, the cose signature alg from which the hash
     * alg is determined. The sign_algorithm was checked in
     * dispatch_psa_attest_gen_key() so it doesn't need to be checked here.
     */
    ret = create_hash(me->sign_algorithm,
                      &me->protected_parameters_buf,
                      signed_payload,
                      &buffer_for_tbs_hash);
    if (ret != ERRCODE_SUCC) {
        attes_print("create_hash error\n");
        goto Done;
    }
    ret = create_sign(me->sign_algorithm,
                      (useful_bufc *)&buffer_for_tbs_hash,
                      &buffer_for_signature);
    if (ret != ERRCODE_SUCC) {
        attes_print("create_sign error\n");
        goto Done;
    }

    /* Add signature to CBOR and close out the array */
    QCBOREncode_AddBytes(cbor_encode_ctx, *(useful_bufc *)&buffer_for_signature);

    /* The layer above this must check for and handle CBOR encoding
     * errors CBOR encoding errors.  Some are detected at the start of
     * this function, but they cannot all be deteced there.
     */
Done:
    return ret;
}
