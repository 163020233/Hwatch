/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: Provides pke driver pri header \n

 */
#ifndef SECURITY_PKE_PRI_H
#define SECURITY_PKE_PRI_H

#include "unified_cipher_pke.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @if Eng
 * @brief  ECC parameters
 * @else
 * @brief ECC参数结构体
 * @endif
 */
typedef struct {
    const uint8_t *p;        /*!< @if Eng Pointer to the prime modulus P (defines the finite field GF(P))
                                 @else   指向素数模数P的指针（定义有限域GF(P)） @endif */

    const uint8_t *a;        /*!< @if Eng Pointer to the coefficient A of the elliptic curve equation
                                 @else   指向椭圆曲线方程系数A的指针 @endif */

    const uint8_t *b;        /*!< @if Eng Pointer to the coefficient B of the elliptic curve equation
                                 @else   指向椭圆曲线方程系数B的指针 @endif */

    const uint8_t *gx;       /*!< @if Eng Pointer to the X-coordinate of the base point G
                                 @else   指向基点G的X坐标指针 @endif */

    const uint8_t *gy;       /*!< @if Eng Pointer to the Y-coordinate of the base point G
                                 @else   指向基点G的Y坐标指针 @endif */

    const uint8_t *n;        /*!< @if Eng Pointer to the order N of the base point G
                                 @else   指向基点G的阶N的指针 @endif */

    uint32_t h;              /*!< @if Eng Cofactor H of the elliptic curve group
                                 @else   椭圆曲线群的余因子H @endif */

    uapi_drv_cipher_pke_ecc_curve_type_t ecc_type;  /*!< @if Eng ECC curve type identifier
                                                        @else   ECC曲线类型标识符 @endif */

    uint32_t ksize;          /*!< @if Eng Size of the key in bytes
                                 @else   密钥字节长度 @endif */

    uint32_t key_valid_bit;  /*!< @if Eng Number of valid bits in the key
                                 @else   密钥中有效位数 @endif */
} uapi_drv_cipher_pke_ecc_param;

/**
 * @if Eng
 * @brief  Get elliptical Curve parameters.
 * @param  [in]    curve_type ECC Curve Type.
 * @param  [out]   ecc_param ECC Curve Parameters .
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  获取椭圆曲线参数。
 * @param  [in]    curve_type ECC曲线类型。
 * @param  [out]   ecc_param 输出椭圆曲线参数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_pke_get_ecc_param(uapi_drv_cipher_pke_ecc_curve_type_t curve_type,
    const uapi_drv_cipher_pke_ecc_param **ecc_param);

errcode_t uapi_drv_cipher_pke_mul_dot_add(uapi_drv_cipher_pke_ecc_curve_type_t curve_type,
    const uapi_drv_cipher_pke_data_t *a, const uapi_drv_cipher_pke_data_t *b,
    const uapi_drv_cipher_pke_ecc_point_t *p, const uapi_drv_cipher_pke_ecc_point_t *q,
    const uapi_drv_cipher_pke_ecc_point_t *r);

errcode_t uapi_drv_cipher_pke_mul_dot_sub(uapi_drv_cipher_pke_ecc_curve_type_t curve_type,
    const uapi_drv_cipher_pke_data_t *a, const uapi_drv_cipher_pke_data_t *b,
    const uapi_drv_cipher_pke_ecc_point_t *p, const uapi_drv_cipher_pke_ecc_point_t *q,
    const uapi_drv_cipher_pke_ecc_point_t *r);

errcode_t uapi_drv_cipher_pke_ecc_gen_key_sec(uapi_drv_cipher_pke_ecc_curve_type_t curve_type,
    const uapi_drv_cipher_pke_data_t *input_priv_key,
    const uapi_drv_cipher_pke_data_t *output_priv_key,
    const uapi_drv_cipher_pke_ecc_point_t *output_pub_key);

errcode_t uapi_drv_cipher_pke_ecdsa_sign_sec(uapi_drv_cipher_pke_ecc_curve_type_t curve_type,
    const uapi_drv_cipher_pke_data_t *priv_key,
    const uapi_drv_cipher_pke_data_t *hash,
    const uapi_drv_cipher_pke_ecc_sig_t *sig);

errcode_t uapi_drv_cipher_pke_ecc_gen_ecdh_key_sec(uapi_drv_cipher_pke_ecc_curve_type_t curve_type,
    const uapi_drv_cipher_pke_ecc_point_t *input_pub_key,
    const uapi_drv_cipher_pke_data_t *input_priv_key,
    const uapi_drv_cipher_pke_data_t *output_shared_key);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif