/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024. All rights reserved.
 * Description: mbedtls harden adapt internal header file.
 * Create: 2023-05-10
*/

#ifndef CMAC_ALT_H
#define CMAC_ALT_H

struct mbedtls_cmac_context_t
{
    /** The internal state of the CMAC algorithm.  */
    unsigned char       MBEDTLS_PRIVATE(state)[MBEDTLS_CIPHER_BLKSIZE_MAX];

    /** Unprocessed data - either data that was not block aligned and is still
     *  pending processing, or the final block. */
    unsigned char       MBEDTLS_PRIVATE(unprocessed_block)[MBEDTLS_CIPHER_BLKSIZE_MAX];

    /** The length of data pending processing. */
    size_t              MBEDTLS_PRIVATE(unprocessed_len);
};

#endif /* cmac_alt.h */