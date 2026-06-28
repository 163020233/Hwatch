/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: attestation common header. \n
 *
 * History: \n
 * 2025-03-14, Create file. \n
 */

#ifndef ATTES_COMMON_H
#define ATTES_COMMON_H


#include "qcbor/qcbor.h"
#include "los_base.h"

#ifdef __cplusplus
#define NULLUsefulBuf {NULL, 0}
#else
#define NULLUsefulBuf ((UsefulBuf) {NULL, 0})
#endif

#define attes_unused(x) (void)(x)

#define attes_print(...)

#define attes_chk_return(cond, ret, ...) do {      \
    if (cond) {                                  \
        attes_print(__VA_ARGS__);                 \
        return ret;                              \
    }                                            \
} while (0)

#define attes_chk_goto(cond, label, ...) do {      \
    if (cond) {                                  \
        attes_print(__VA_ARGS__);                 \
        goto label;                              \
    }                                            \
} while (0)

#define psa_attes_array_size(arr)  (sizeof(arr) / sizeof((arr)[0]))

#define  USEFUL_BUF_MAKE_STACK_UB(name, size) \
    uint8_t    __pBuf##name[(size)]; \
    useful_buf  name = {__pBuf##name, sizeof(__pBuf##name)}

#define COSE_ALGORITHM_ES256               1
#define COSE_ALGORITHM_ES384               2
#define COSE_ALGORITHM_ES512               3
#define COSE_ALGORITHM_MAX                 4

#define COSE_KEY_MAX_SIZE  64  /* size for secp512 */
#define COSE_MAX_SIG_SIZE  128 /* size for secp512 */
#define COSE_MAX_HASH_SIZE 64  /* size for sha512 */

#define SHA256_HASH_LEN    32
#define SHA384_HASH_LEN    48
#define SHA512_HASH_LEN    64
#define ECC_BP256_KEY_LEN  32
#define ECC_BP384_KEY_LEN  48
#define ECC_BP512_KEY_LEN  64
#define ECC_BP256_SIGN_LEN 32
#define ECC_BP384_SIGN_LEN 48
#define ECC_BP512_SIGN_LEN 64

#define IAK_SEC_STORAGE_ID          0x10
#define INSTANCE_ID_SEC_STORAGE_ID  0x11

typedef UsefulBuf useful_buf;
typedef UsefulBufC useful_bufc;

#define COSE_SIGN1_MAX_SIZE_PROTECTED_PARAMETERS (1+1+5+17)

struct cose_sign_ctx {
    int32_t                sign_algorithm;
    uint8_t                protected_parameters[COSE_SIGN1_MAX_SIZE_PROTECTED_PARAMETERS];
    useful_bufc            protected_parameters_buf; /* The encoded protected parameters */
};

struct attest_token_encode_ctx {
    QCBOREncodeContext      cbor_enc_ctx;
    struct cose_sign_ctx    signer_ctx;
};

void psa_attes_get_bootseed(void);

#define CODE_HASH_LEN 32
#define CODE_HASH_OFFSET (256 + 40)
#define CODE_VERSION_OFFSET (256 + 16)
#define CODE_VERSION_MASK_OFFSET (256 + 20)

typedef struct {
    const char *type;
    uint32_t flash_addr;
    uint32_t version;
    uint32_t version_mask;
    uint8_t hash[CODE_HASH_LEN];
} sw_components_t;

#endif
