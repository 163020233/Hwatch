/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: mbedtls alt bignum implemention.
 *
*/
#include "library/common.h"
#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_BIGNUM_ALT)
#include "bignum_alt.h"
#include "security_pke.h"
#include "mbedtls/error.h"
#include "mbedtls/platform.h"
#include "crypto_pke_struct.h"
#include "mbedtls_alt_common_def.h"
#include "securec.h"

#include "mbedtls_perf_utils.h"

#define BIGNUM_EXP_MOD_CHECK_CAN_DO     1
#define BIGNUM_HARDEN_ALIGNED_NUM       4

#define MAX_COMPARE(a, b)                   (a) > (b) ? (a) : (b)
#define CELL_ALIGNED(input, aligned_num)    (((input) + (aligned_num) - 1) / (aligned_num) * (aligned_num))

static void get_size_align(size_t size_n, size_t *size_align)
{
    size_t sizes[] = {
        MBEDTLS_ALT_ECP_LEN_192, MBEDTLS_ALT_ECP_LEN_256, MBEDTLS_ALT_ECP_LEN_384, MBEDTLS_ALT_ECP_LEN_448,
        MBEDTLS_ALT_ECP_LEN_512, MBEDTLS_ALT_ECP_LEN_1024, MBEDTLS_ALT_ECP_LEN_1536, MBEDTLS_ALT_ECP_LEN_2048,
        MBEDTLS_ALT_ECP_LEN_3072, MBEDTLS_ALT_ECP_LEN_4096
    };
    size_t sizes_count = sizeof(sizes) / sizeof(sizes[0]);

    for (size_t i = 0; i < sizes_count - 1; i++) {
        if (size_n <= sizes[i]) {
            *size_align = sizes[i];
            return;
        }
    }
    *size_align = sizes[sizes_count - 1];
}

static int check_exp_mod_harden_can_do(const mbedtls_mpi *A, const mbedtls_mpi *N)
{
    size_t size_a, size_n, size_aligned;
    size_a = CELL_ALIGNED(mbedtls_mpi_size(A), BIGNUM_HARDEN_ALIGNED_NUM);
    size_n = mbedtls_mpi_size(N);
    /* influence the right aligned_size for cal. */
    get_size_align(size_n, &size_aligned);
    if ((size_n > MBEDTLS_ALT_ECP_LEN_4096) || (size_a > 2 * size_aligned)) {   /* 2: hard can processed max(size_a) */
        mbedtls_printf("hardware not support");
        return 0;
    }
    return((size_aligned - size_n) <= 2);   // 2: 2-byte diff.
}

int mbedtls_mpi_exp_mod_alt(mbedtls_mpi *X, const mbedtls_mpi *A, const mbedtls_mpi *E, const mbedtls_mpi *N)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    uapi_drv_cipher_pke_data_t hw_data[4];    // 4: store A, E, N and result.
    size_t size_align = 0;
    size_t size_max = 0;
    const mbedtls_mpi *inputs[] = {A, E, N};  // X = A ^ E mod N ->  out = in ^ k mod n;
    unsigned char *buffer = NULL;
    MBEDTLS_PERF_START();

    ret = check_exp_mod_harden_can_do(A, N);
    mbedtls_harden_check_return(ret != BIGNUM_EXP_MOD_CHECK_CAN_DO, MBEDTLS_ERR_BIGNUM_HARDWARE_UNSUPPORT,
        "not support\r\n");

    get_size_align(mbedtls_mpi_size(N), &size_align);
    size_max = MAX_COMPARE(MAX_COMPARE(mbedtls_mpi_size(A), mbedtls_mpi_size(E)), size_align);
    size_max = CELL_ALIGNED(size_max, BIGNUM_HARDEN_ALIGNED_NUM);
    buffer = mbedtls_calloc(4, size_max); // 4: store A、E、N and result.
    mbedtls_harden_check_return(buffer == NULL, MBEDTLS_ERR_MPI_ALLOC_FAILED, "exp_mod_alt calloc fail!\n");
    (void)memset_s(buffer, 4 * size_max, 0, 4 * size_max);  /* 4: store A、E、N and result. */

    hw_data[0].length = CELL_ALIGNED(mbedtls_mpi_size(A), BIGNUM_HARDEN_ALIGNED_NUM);
    hw_data[1].length = CELL_ALIGNED(mbedtls_mpi_size(E), BIGNUM_HARDEN_ALIGNED_NUM);
    hw_data[2].length = size_align; /* 2: idx for n */
    for (unsigned int i = 0; i < 3; i++) {   // 3: for A, E and N.
        hw_data[i].data = buffer + i * size_max;
        MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(inputs[i], hw_data[i].data, hw_data[i].length));
    }
    hw_data[3].data = buffer + 3 * size_max; /* 3: idx for result */
    hw_data[3].length = size_align; /* 3: idx for result */
    /* step 1. if len(A) > len(N) && len(A) < len(2N), modular reduce first. */
    if (CELL_ALIGNED(mbedtls_mpi_size(A), BIGNUM_HARDEN_ALIGNED_NUM) > size_align) {
        /* 2: idx for n, 3: idx for result. */
        ret = uapi_drv_cipher_pke_mod(&hw_data[0], &hw_data[2], &hw_data[3]);
        mbedtls_harden_check_goto_with_ret(ret, ret != ERRCODE_SUCC, cleanup, MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED,
            "uapi_drv_cipher_pke_mod failed, ret = 0x%x!\n", ret);
        hw_data[0].data = hw_data[3].data;  /* 3: idx for result. */
        hw_data[0].length = hw_data[3].length;  /* 3: idx for result. */
    }
    /* 2: idx for n, 3: idx for result. */
    ret = uapi_drv_cipher_pke_exp_mod(&hw_data[2], &hw_data[1], &hw_data[0], &hw_data[3]);
    mbedtls_harden_check_goto_with_ret(ret, ret != ERRCODE_SUCC, cleanup, MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED,
        "uapi_drv_cipher_pke_exp_mod failed, ret = 0x%x!\n", ret);
    /* write result to X. */
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(X, hw_data[3].data, hw_data[3].length));    /* 3: result idx. */
    ret = 0;
cleanup:
    if (buffer != NULL) {
        mbedtls_platform_zeroize(buffer, 4 * size_max); /* 4: store A, E, N and result. */
        mbedtls_free(buffer);
    }

    MBEDTLS_PERF_BIGNUM_END(size_align);
    return ret;
}
#endif
