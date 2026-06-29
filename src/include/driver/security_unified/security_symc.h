/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: the header file of aes API. \n
 * \n
 * Create: 2024-06-11
*/

#ifndef SECURITY_SYMC_H
#define SECURITY_SYMC_H

#include <stdbool.h>
#include "errcode.h"
#include "security_km.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @defgroup security_unified_symc Symc
 * @ingroup  drivers_driver_security_unified
 * @{
 */

typedef uintptr_t cipher_handle_t;

/**
 * @if Eng
 * @brief  compat symc alg.
 * @else
 * @brief  组合算法类型。
 * @endif
 */
#define uapi_compat_symc_alg(alg, work_mode, bit_width)   \
    ((((alg) & 0xFF) << 16) + (((work_mode) & 0Xff) << 8) + (bit_width))

/* AES-ECB. */
#define UAPI_DRV_SYMC_AES_ECB_ALG     0x10003
/* AES-CBC. */
#define UAPI_DRV_SYMC_AES_CBC_ALG     0x10103
/* AES-CTR. */
#define UAPI_DRV_SYMC_AES_CTR_ALG     0x10203
/* AES-OFB. */
#define UAPI_DRV_SYMC_AES_OFB_ALG     0x10303
/* AES-CFB1. */
#define UAPI_DRV_SYMC_AES_CFB1_ALG    0x10400
/* AES-CFB8. */
#define UAPI_DRV_SYMC_AES_CFB8_ALG    0x10401
/* AES-CFB128. */
#define UAPI_DRV_SYMC_AES_CFB128_ALG  0x10403
/* SM4-ECB. */
#define UAPI_DRV_SYMC_SM4_ECB_ALG     0x20003
/* SM4-CBC. */
#define UAPI_DRV_SYMC_SM4_CBC_ALG     0x20103
/* SM4-CTR. */
#define UAPI_DRV_SYMC_SM4_CTR_ALG     0x20203
/* SM4-OFB. */
#define UAPI_DRV_SYMC_SM4_OFB_ALG     0x20303
/* SM4-CFB1. */
#define UAPI_DRV_SYMC_SM4_CFB1_ALG    0x20400
/* SM4-CFB8. */
#define UAPI_DRV_SYMC_SM4_CFB8_ALG    0x20401
/* SM4-CFB128. */
#define UAPI_DRV_SYMC_SM4_CFB128_ALG  0x20403

/**
 * @if Eng
 * @brief Symmetrical encryption algorithm type
 * @note  TDES is not secure, and we advise not to use it.
 * @else
 * @brief 对称加密算法类型
 * @note  TDES不安全，不建议使用
 * @endif
 */
typedef enum {
    UAPI_DRV_CIPHER_SYMC_ALG_TDES = 0x0,
    UAPI_DRV_CIPHER_SYMC_ALG_AES = 0x1,
    UAPI_DRV_CIPHER_SYMC_ALG_SM4 = 0x2,
    UAPI_DRV_CIPHER_SYMC_ALG_LEA = 0x3,
    UAPI_DRV_CIPHER_SYMC_ALG_DMA = 0x4,
    UAPI_DRV_CIPHER_SYMC_ALG_MAX,
    UAPI_DRV_CIPHER_SYMC_ALG_INVALID = 0xffffffff,
} uapi_drv_cipher_symc_alg_t;

/**
 * @if Eng
 * @brief Symmetric encryption algorithm worke mode
 * @note  ECB is not secure, and we advise not to use it.
 * @else
 * @brief 对称加密算法工作模式
 * @note  ECB模式不安全，不建议使用
 * @endif
 */
typedef enum {
    UAPI_DRV_CIPHER_SYMC_WORK_MODE_ECB = 0x0,
    UAPI_DRV_CIPHER_SYMC_WORK_MODE_CBC,
    UAPI_DRV_CIPHER_SYMC_WORK_MODE_CTR,
    UAPI_DRV_CIPHER_SYMC_WORK_MODE_OFB,
    UAPI_DRV_CIPHER_SYMC_WORK_MODE_CFB,
    UAPI_DRV_CIPHER_SYMC_WORK_MODE_CCM,
    UAPI_DRV_CIPHER_SYMC_WORK_MODE_GCM,
    UAPI_DRV_CIPHER_SYMC_WORK_MODE_CBC_MAC,
    UAPI_DRV_CIPHER_SYMC_WORK_MODE_CMAC,
    UAPI_DRV_CIPHER_SYMC_WORK_MODE_MAX,
    UAPI_DRV_CIPHER_SYMC_WORK_MODE_INVALID = 0xffffffff,
} uapi_drv_cipher_symc_work_mode_t;

/**
 * @if Eng
 * @brief Symmetric encryption key bit width
 * @else
 * @brief 对称算法密钥位宽
 * @endif
 */
typedef enum {
    UAPI_DRV_CIPHER_SYMC_BIT_WIDTH_1BIT = 0x0,
    UAPI_DRV_CIPHER_SYMC_BIT_WIDTH_8BIT = 0x1,
    UAPI_DRV_CIPHER_SYMC_BIT_WIDTH_64BIT = 0x2,
    UAPI_DRV_CIPHER_SYMC_BIT_WIDTH_128BIT = 0x3,
    UAPI_DRV_CIPHER_SYMC_BIT_WIDTH_MAX,
    UAPI_DRV_CIPHER_SYMC_BIT_WIDTH_INVALID = 0xffffffff,
} uapi_drv_cipher_symc_bit_width_t;

/* SYMC(Single-Part). */
/**
 * @if Eng
 * @brief  Encrypt or Decrypt.
 * @param  [in]     alg Combine value. |<-1byte reserved->|<-1byte alg->|<-1byte work_mode->|<-1byte bit_width->|
 *                      Crypto alg, see @ref uapi_drv_cipher_symc_alg_t.
                        Crypto work mode， see @ref uapi_drv_cipher_symc_work_mode_t.
                        Crypto key bit width， see @ref uapi_drv_cipher_symc_bit_width_t.
 * @param  [in]     src Source data.
 * @param  [out]    dst Destination data.
 * @param  [in]     data_len Data length in byte. Return success if data_len is zero.
 * @param  [inout]  iv The input is the initial vector, and the output is the updated vector.
 * @param  [in]     key Symmetric key.
 * @param  [in]     key_len Length of symmetric key, in byte.
 * @param  [in]     keyslot_handle Handle for storing the key. The handle is created and destroyed by the KM module.
 *                  If not need please set as UAPI_DRV_INVALID_KEY_SLOT.
 * @param  [in]     is_encrypt Encrypted or not.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @note   There are two ways to set the key:
            1）Use the key/key_len combination. In this case, keyslot_handle is set to UAPI_DRV_INVALID_KEY_SLOT。.
            2）Use the keyslot_handle/key_len combination to pass in the keyslot handle created
                by the keyslot interface. In this case, the key is NULL.
 * @else
 * @brief  加解密。
 * @param  [in]     alg 组合值。 |<-1byte reserved->|<-1byte alg->|<-1byte work_mode->|<-1byte bit_width->|
                        加密算法，见 @ref uapi_drv_cipher_symc_alg_t 。
                        工作模式， 参考 @ref uapi_drv_cipher_symc_work_mode_t 。
                        密钥位宽， 参考 @ref uapi_drv_cipher_symc_bit_width_t 。
 * @param  [in]     src 源数据。
 * @param  [out]    dst 目的数据。
 * @param  [in]     data_len 数据长度，单位是 Byte。传入0，接口返回成功。
 * @param  [inout]  iv 输入为初始向量，输出为更新后的向量。
 * @param  [in]     key 对称密钥。
 * @param  [in]     key_len 对称密钥长度，单位是 Byte。
 * @param  [in]     keyslot_handle 用于保存key的句柄，通过 km 模块创建和销毁。
 *                  如果不需要，请设置为 UAPI_DRV_INVALID_KEY_SLOT。
 * @param  [in]     is_encrypt 是否加密。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @note   有两种方法设置密钥:
            1）使用 key/key_len 组合，在这种情况下，keyslot_handle 传入 UAPI_DRV_INVALID_KEY_SLOT。.
            2）使用 keyslot_handle/key_len 组合，keyslot 接口创建 keyslot 句柄传入，在这种情况下，key 传入为 NULL.
 * @endif
 */
errcode_t uapi_drv_cipher_symc_crypt(uint32_t alg, const uint8_t *src, uint8_t *dst, uint32_t data_len,
    uint8_t iv[16], const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle, bool is_encrypt
);

/* SYMC(Multi-Part). */
/**
 * @if Eng
 * @brief  Initialize the context for multi-part encryption or decryption.
 * @param  [in]   ctx Symc handle.
 * @param  [in]   alg Combine value. |<-1byte reserved->|<-1byte alg->|<-1byte work_mode->|<-1byte bit_width->|
 *                     Crypto alg, see @ref uapi_drv_cipher_symc_alg_t.
                       Crypto work mode， see @ref uapi_drv_cipher_symc_work_mode_t.
                       Crypto key bit width， see @ref uapi_drv_cipher_symc_bit_width_t.
 * @param  [in]   key Symmetric key.
 * @param  [in]   key_len Length of symmetric key, in byte.
 * @param  [in]   keyslot_handle Handle for storing the key. The handle is created and destroyed by the KM module.
 *                 If not need please set as UAPI_DRV_INVALID_KEY_SLOT.
 * @param  [in]   is_encrypt Encrypted or not.
 * @retval ERRCODE_SUCC Success.
 * @retval Other         Failure. For details, see @ref errcode_t
 * @note   There are two ways to set the key:
            1）Use the key/key_len combination. In this case, keyslot_handle is set to UAPI_DRV_INVALID_KEY_SLOT.
            2）Use the keyslot_handle/key_len combination to pass in the keyslot handle created
                by the keyslot interface. In this case, the key is NULL.
 * @else
 * @brief  初始化分段加解密的上下文句柄。
 * @param  [in]   ctx Symc 句柄。
 * @param  [in]   alg 组合值。 |<-1byte reserved->|<-1byte alg->|<-1byte work_mode->|<-1byte bit_width->|
                     加密算法，见 @ref uapi_drv_cipher_symc_alg_t 。
                     工作模式， 参考 @ref uapi_drv_cipher_symc_work_mode_t 。
                     密钥位宽， 参考 @ref uapi_drv_cipher_symc_bit_width_t 。
 * @param  [in]   key 对称密钥。
 * @param  [in]   key_len 对称密钥长度，单位是 Byte。
 * @param  [in]   keyslot_handle 用于保存key的句柄，通过 km 模块创建和销毁。
 *                 如果不需要，请设置为 UAPI_DRV_INVALID_KEY_SLOT。
 * @param  [in]   is_encrypt 是否加密。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other         失败，参考 @ref errcode_t 。
 * @note   有两种方法设置密钥:
            1）使用 key/key_len 组合，在这种情况下，keyslot_handle 传入 UAPI_DRV_INVALID_KEY_SLOT。
            2）使用 keyslot_handle/key_len 组合，keyslot 接口创建 keyslot 句柄传入，在这种情况下，key 传入为 NULL。
 * @endif
 */
errcode_t uapi_drv_cipher_symc_setup(cipher_handle_t *ctx, uint32_t alg,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle, bool is_encrypt);

/**
 * @if Eng
 * @brief  Multi-part encryption or decryption for ECB mode.
 * @param  [in]   ctx Symc handle.
 * @param  [in]   src Source data.
 * @param  [out]  dst Destination data.
 * @param  [in]   data_len Data length in byte. Return success if data_len is zero.
 * @retval ERRCODE_SUCC Success.
 * @retval Other         Failure. For details, see @ref errcode_t
 * @else
 * @brief  ECB模式下的分段加解密。
 * @param  [in]   ctx Symc 句柄。
 * @param  [in]   src 源数据。
 * @param  [out]  dst 目的数据。
 * @param  [in]   data_len 数据长度，单位是 Byte。传入0，接口返回成功。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other         失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_symc_ecb_update(cipher_handle_t ctx, const uint8_t *src, uint8_t *dst, uint32_t data_len);

/**
 * @if Eng
 * @brief  Multi-part encryption or decryption for CBC mode.
 * @param  [in]   ctx Symc handle.
 * @param  [in]   src Source data.
 * @param  [out]  dst Destination data.
 * @param  [in]   data_len Data length in byte. Return success if data_len is zero.
 * @param  [inout] iv The input is the initial vector, and the output is the updated vector.
 * @retval ERRCODE_SUCC Success.
 * @retval Other         Failure. For details, see @ref errcode_t
 * @else
 * @brief  CBC 模式下的分段加解密。
 * @param  [in]   ctx Symc 句柄。
 * @param  [in]   src 源数据。
 * @param  [out]  dst 目的数据。
 * @param  [in]   data_len 数据长度，单位是 Byte。传入0，接口返回成功。
 * @param  [inout] iv 输入为初始向量，输出为更新后的向量。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other         失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_symc_cbc_update(cipher_handle_t ctx, const uint8_t *src, uint8_t *dst, uint32_t data_len,
    uint8_t iv[16]);

/**
 * @if Eng
 * @brief  Multi-part encryption or decryption for CTR mode.
 * @param  [in]   ctx Symc handle.
 * @param  [in]   src Source data.
 * @param  [out]  dst Destination data.
 * @param  [in]   data_len Data length in byte. Return success if data_len is zero.
 * @param  [inout] nonce_counter The input is the initial 128-bit nonce and counter, and the output is the updated
                    128-bit nonce and counter.
 * @param  [inout] nc_off The offset in the current stream_block, for resuming within the current cipher stream.
                    It's value should be 0 at the start of the stream, and the output is the updated offset, it will be
                    maintained by the function.
 * @param  [out]  stream_block The output is the updated current stream_block.
 * @retval ERRCODE_SUCC Success.
 * @retval Other         Failure. For details, see @ref errcode_t
 * @else
 * @brief  CTR 模式下的分段加解密。
 * @param  [in]   ctx Symc 句柄。
 * @param  [in]   src 源数据。
 * @param  [out]  dst 目的数据。
 * @param  [in]   data_len 数据长度，单位是 Byte。传入0，接口返回成功。
 * @param  [inout] nonce_counter 输入为初始 128 位 nonce 和 counter，输出为更新后的 128 位 nonce 和 counter。
 * @param  [inout] nc_off 在当前 stream block 中的偏移量，用于在当前加密流中恢复计算。
                        它的值应该在流的开始时为 0，输出为更新的偏移量，函数将维护它。
 * @param  [out]  stream_block 输出为更新后的当前 stream_block。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other         失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_symc_ctr_update(cipher_handle_t ctx, const uint8_t *src, uint8_t *dst, uint32_t data_len,
    uint8_t nonce_counter[16], uint8_t *nc_off, uint8_t stream_block[16]);

/**
 * @if Eng
 * @brief  Multi-part encryption or decryption for OFB mode.
 * @param  [in]   ctx Symc handle.
 * @param  [in]   src Source data.
 * @param  [out]  dst Destination data.
 * @param  [in]   data_len Data length in byte. Return success if data_len is zero.
 * @param  [inout] iv The input is the initial vector, and the output is the updated vector.
 * @param  [inout] iv_off The offset in the current iv, for resuming within the current cipher stream.
 * @retval ERRCODE_SUCC Success.
 * @retval Other         Failure. For details, see @ref errcode_t
 * @else
 * @brief  OFB 模式下的分段加解密。
 * @param  [in]   ctx Symc 句柄。
 * @param  [in]   src 源数据。
 * @param  [out]  dst 目的数据。
 * @param  [in]   data_len 数据长度，单位是 Byte。传入0，接口返回成功。
 * @param  [inout] iv 输入为初始向量，输出为更新后的向量。
 * @param  [inout] iv_off 在当前 iv 中的偏移量，用于在当前加密流中恢复计算。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other         失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_symc_ofb_update(cipher_handle_t ctx, const uint8_t *src, uint8_t *dst, uint32_t data_len,
    uint8_t iv[16], uint8_t *iv_off);

/**
 * @if Eng
 * @brief  Multi-part encryption or decryption for CFB8 mode.
 * @param  [in]   ctx Symc handle.
 * @param  [in]   src Source data.
 * @param  [out]  dst Destination data.
 * @param  [in]   data_len Data length in byte. Return success if data_len is zero.
 * @param  [inout] iv The input is the initial vector, and the output is the updated vector.
 * @param  [inout] iv_off The offset in the current iv, for resuming within the current cipher stream.
 * @retval ERRCODE_SUCC Success.
 * @retval Other         Failure. For details, see @ref errcode_t
 * @else
 * @brief  CFB8 模式下的分段加解密。
 * @param  [in]   ctx Symc 句柄。
 * @param  [in]   src 源数据。
 * @param  [out]  dst 目的数据。
 * @param  [in]   data_len 数据长度，单位是 Byte。传入0，接口返回成功。
 * @param  [inout] iv 输入为初始向量，输出为更新后的向量。
 * @param  [inout] iv_off 在当前 iv 中的偏移量，用于在当前加密流中恢复计算。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other         失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_symc_cfb8_update(cipher_handle_t ctx, const uint8_t *src, uint8_t *dst, uint32_t data_len,
    uint8_t iv[16], uint8_t *iv_off);

/**
 * @if Eng
 * @brief  Multi-part encryption or decryption for CFB128 mode.
 * @param  [in]   ctx Symc handle.
 * @param  [in]   src Source data.
 * @param  [out]  dst Destination data.
 * @param  [in]   data_len Data length in byte. Return success if data_len is zero.
 * @param  [inout] iv The input is the initial vector, and the output is the updated vector.
 * @param  [inout] iv_off The offset in the current iv, for resuming within the current cipher stream.
 * @retval ERRCODE_SUCC Success.
 * @retval Other         Failure. For details, see @ref errcode_t
 * @else
 * @brief  CFB128 模式下的分段加解密。
 * @param  [in]   ctx Symc 句柄。
 * @param  [in]   src 源数据。
 * @param  [out]  dst 目的数据。
 * @param  [in]   data_len 数据长度，单位是 Byte。传入0，接口返回成功。
 * @param  [inout] iv 输入为初始向量，输出为更新后的向量。
 * @param  [inout] iv_off 在当前 iv 中的偏移量，用于在当前加密流中恢复计算。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other         失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_symc_cfb128_update(cipher_handle_t ctx, const uint8_t *src, uint8_t *dst, uint32_t data_len,
    uint8_t iv[16], uint8_t *iv_off);

/**
 * @if Eng
 * @brief  Destroy the handle.
 * @param  [in]   ctx The symc handle to be destroyed.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  销毁句柄。
 * @param  [in]   ctx 要销毁的 symc 句柄。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_symc_teardown(cipher_handle_t ctx);

/**
 * @if Eng
 * @brief  CCM encrypt and compute tag.
 * @param  [in]    alg Crypto Alg. see @ref uapi_drv_cipher_symc_alg_t.
 * @param  [in]    key Encryption key.
 * @param  [in]    key_len Encryption key length in byte. Only support 16,24,32.
 * @param  [in]    keyslot_handle The keyslot handle which store the encryption key.
 * @param  [in]    iv The initial vector.
 * @param  [in]    iv_len The initial vector lenght in byte. Only support 7~13.
 * @param  [in]    aad The additional authentication message. NULL is support.
 * @param  [in]    aad_len The additional authentication message length in byte.
 * @param  [in]    plain_text Input plain text.
 * @param  [out]   cipher_text Output cipher text.
 * @param  [in]    data_len data length in byte.
 *                  When iv_len is 13, data_len cannot exceed 2 ^ 16 - 1.
 *                  When iv_len is 12, data_len cannot exceed 2 ^ 24 - 1.
 * @param  [out]   tag Output tag.
 * @param  [in]    tag_len The expected tag length in byte. Only support 4, 6, 8, 10, 12, 14 or 16.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @note   There are two ways to set the key:
            1）Use the key/key_len combination. In this case, keyslot_handle is set to UAPI_DRV_INVALID_KEY_SLOT.
            2）Use the keyslot_handle/key_len combination to pass in the keyslot handle created
                by the keyslot interface. In this case, the key is NULL.
 * @else
 * @brief  CCM 加密并计算 tag.
 * @param  [in]    alg 加解密算法. 见 @ref uapi_drv_cipher_symc_alg_t 。
 * @param  [in]    key 加密密钥.
 * @param  [in]    key_len 加密密钥长度，单位是 Byte. 只支持 16,24,32.
 * @param  [in]    keyslot_handle 保存加密密钥的 keyslot 句柄.
 * @param  [in]    iv 初始向量
 * @param  [in]    iv_len 初始向量长度，单位是 Byte. 长度支持 7~13.
 * @param  [in]    aad 附加认证信息，支持传入为 NULL
 * @param  [in]    aad_len 附加认证信息，单位是 Byte.
 * @param  [in]    plain_text 输入的明文.
 * @param  [out]   cipher_text 输出的密文.
 * @param  [in]    data_len 数据长度，单位是 Byte.
 *                  iv_len 为 13 时, data_len 不能超过 2^16 - 1.
 *                  iv_len 为 12 时, data_len 不能超过 2^24 - 1.
 * @param  [out]   tag 输出的 tag
 * @param  [in]    tag_len 期望的 tag 长度，单位是 Byte. 只支持 4, 6, 8, 10, 12, 14 or 16.
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @note   有两种方法设置密钥:
            1）使用 key/key_len 组合，在这种情况下，keyslot_handle 传入 UAPI_DRV_INVALID_KEY_SLOT。
            2）使用 keyslot_handle/key_len 组合，keyslot 接口创建 keyslot 句柄传入，在这种情况下，key 传入为 NULL。
 * @endif
 */
errcode_t uapi_drv_cipher_symc_ccm_encrypt(uapi_drv_cipher_symc_alg_t alg,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *iv, uint32_t iv_len,
    const uint8_t *aad, uint32_t aad_len,
    const uint8_t *plain_text, uint8_t *cipher_text, uint32_t data_len,
    uint8_t *tag, uint32_t tag_len);

/**
 * @if Eng
 * @brief  CCM decrypt and verify tag.
 * @param  [in]    alg Crypto Alg. see @ref uapi_drv_cipher_symc_alg_t.
 * @param  [in]    key Decryption key.
 * @param  [in]    key_len Decryption key length in byte. Only support 16,24,32.
 * @param  [in]    keyslot_handle The keyslot handle which store the decryption key.
 * @param  [in]    iv The initial vector.
 * @param  [in]    iv_len The initial vector lenght in byte. Only support 7~13.
 * @param  [in]    aad The additional authentication message. NULL is support.
 * @param  [in]    aad_len The additional authentication message length in byte.
 * @param  [in]    cipher_text Input cipher text.
 * @param  [out]   plain_text Output plain text.
 * @param  [in]    data_len data length in byte.
 *                  When iv_len is 13, data_len cannot exceed 2 ^ 16 - 1.
 *                  When iv_len is 12, data_len cannot exceed 2 ^ 24 - 1.
 * @param  [in]    tag Input tag to be verified.
 * @param  [in]    tag_len The expected tag length in byte. Only support 4, 6, 8, 10, 12, 14 or 16.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @note   There are two ways to set the key:
            1）Use the key/key_len combination. In this case, keyslot_handle is set to UAPI_DRV_INVALID_KEY_SLOT.
            2）Use the keyslot_handle/key_len combination to pass in the keyslot handle created
                by the keyslot interface. In this case, the key is NULL.
 * @else
 * @brief  CCM 解密并校验 tag.
 * @param  [in]    alg 加解密算法. 见 @ref uapi_drv_cipher_symc_alg_t 。
 * @param  [in]    key 解密密钥.
 * @param  [in]    key_len 解密密钥长度，单位是 Byte. 只支持 16,24,32.
 * @param  [in]    keyslot_handle 保存解密密钥的 keyslot 句柄.
 * @param  [in]    iv 初始向量
 * @param  [in]    iv_len 初始向量长度，单位是 Byte. 长度支持 7~13.
 * @param  [in]    aad 附加认证信息，支持传入为 NULL
 * @param  [in]    aad_len 附加认证信息，单位是 Byte.
 * @param  [in]    cipher_text 输入的密文.
 * @param  [out]   plain_text 输出的明文.
 * @param  [in]    data_len 数据长度，单位是 Byte.
 *                  iv_len 为 13 时, data_len 不能超过 2^16 - 1.
 *                  iv_len 为 12 时, data_len 不能超过 2^24 - 1.
 * @param  [in]    tag 待校验的 tag.
 * @param  [in]    tag_len tag 长度，单位是 Byte. 只支持 4, 6, 8, 10, 12, 14 or 16.
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @note   有两种方法设置密钥:
            1）使用 key/key_len 组合，在这种情况下，keyslot_handle 传入 UAPI_DRV_INVALID_KEY_SLOT。
            2）使用 keyslot_handle/key_len 组合，keyslot 接口创建 keyslot 句柄传入，在这种情况下，key 传入为 NULL。
 * @endif
 */
errcode_t uapi_drv_cipher_symc_ccm_decrypt_verify(uapi_drv_cipher_symc_alg_t alg,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *iv, uint32_t iv_len,
    const uint8_t *aad, uint32_t aad_len,
    const uint8_t *cipher_text, uint8_t *plain_text, uint32_t data_len,
    const uint8_t *tag, uint32_t tag_len);


/**
 * @if Eng
 * @brief  Initialize the context for multi-part CCM encryption or decryption.
 * @param  [out]   ctx Created symc handle.
 * @param  [in]    alg Crypto Alg. see @ref uapi_drv_cipher_symc_alg_t.
 * @param  [in]    key Encryption key.
 * @param  [in]    key_len Encryption key length in byte. Only support 16,24,32.
 * @param  [in]    keyslot_handle The keyslot handle which store the encryption key.
 * @param  [in]    iv The initial vector.
 * @param  [in]    iv_len The initial vector lenght in byte. Only support 7~13.
 * @param  [in]    is_encrypt Encrypted or not.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @note   There are two ways to set the key:
            1）Use the key/key_len combination. In this case, keyslot_handle is set to UAPI_DRV_INVALID_KEY_SLOT.
            2）Use the keyslot_handle/key_len combination to pass in the keyslot handle created
                by the keyslot interface. In this case, the key is NULL.
 * @else
 * @brief  初始化 CCM 分段加解密的上下文句柄。
 * @param  [out]   ctx 创建的加解密句柄。
 * @param  [in]    alg 加解密算法. 见 @ref uapi_drv_cipher_symc_alg_t 。
 * @param  [in]    key 加密密钥.
 * @param  [in]    key_len 加密密钥长度，单位是 Byte. 只支持 16,24,32.
 * @param  [in]    keyslot_handle 保存加密密钥的 keyslot 句柄.
 * @param  [in]    iv 初始向量
 * @param  [in]    iv_len 初始向量长度，单位是 Byte. 长度支持 7~13.
 * @param  [in]    is_encrypt 是否加密。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @note   有两种方法设置密钥:
            1）使用 key/key_len 组合，在这种情况下，keyslot_handle 传入 UAPI_DRV_INVALID_KEY_SLOT。
            2）使用 keyslot_handle/key_len 组合，keyslot 接口创建 keyslot 句柄传入，在这种情况下，key 传入为 NULL。
 * @endif
 */
errcode_t uapi_drv_cipher_symc_ccm_setup(cipher_handle_t *ctx, uapi_drv_cipher_symc_alg_t alg,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *iv, uint32_t iv_len, bool is_encrypt);

/**
 * @if Eng
 * @brief  Set additional data length, message length and tag length for CCM.
 * @param  [in]    ctx symc handle.
 * @param  [in]    total_aad_len The total length of additional data in bytes.
 * @param  [in]    data_len The total length of plaintext or ciphertext in bytes.
 * @param  [in]    tag_len The length of the tag to generate in bytes. Only support 4, 6, 8, 10, 12, 14 or 16.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  为 CCM 模式设置附加数据长度，数据长度和 tag 长度。
 * @param  [in]    ctx symc handle.
 * @param  [in]    total_aad_len 附加数据的总长度，单位是字节。
 * @param  [in]    data_len 加密或解密数据的总长度，单位是字节。
 * @param  [in]    tag_len 生成的 tag 值的长度，单位是字节。 只支持 4, 6, 8, 10, 12, 14 或者 16。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_symc_ccm_set_lengths(cipher_handle_t ctx, uint32_t total_aad_len,
    uint32_t data_len, uint32_t tag_len);

/**
 * @if Eng
 * @brief  Add addition data for CCM. You may call this function zero, one or more times
 *                  to pass successive parts of the additional data.
 * @param  [in]    ctx symc handle.
 * @param  [in]    aad The additional data.
 * @param  [in]    aad_len The length of additional data in bytes for this times.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  为 CCM 增加附加数据。您可以零次、一次或多次调用此函数来传递分段的附加数据
 * @param  [in]    ctx symc handle.
 * @param  [in]    aad 附加数据。
 * @param  [in]    aad_len 此次附件数据的长度，单位是字节。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_symc_ccm_update_ad(cipher_handle_t ctx, const uint8_t *aad, uint32_t aad_len);

/**
 * @if Eng
 * @brief  Multi-part encryption or decryption for CCM mode.
 * @param  [in]   ctx Symc handle.
 * @param  [in]   src Source data.
 * @param  [out]  dst Destination data.
 * @param  [in]   data_len Data length in byte. Return success if data_len is zero.
 * @retval ERRCODE_SUCC Success.
 * @retval Other         Failure. For details, see @ref errcode_t
 * @else
 * @brief  CCM 模式下的分段加解密。
 * @param  [in]   ctx 加解密句柄。
 * @param  [in]   src 源数据。
 * @param  [out]  dst 目的数据。
 * @param  [in]   data_len 数据长度，单位是 Byte。传入0，接口返回成功。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other         失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_symc_ccm_update(cipher_handle_t ctx, const uint8_t *src, uint8_t *dst, uint32_t data_len);


/**
 * @if Eng
 * @brief Obtain tag for CCM.
 * @param  [in]   ctx Symc handle.
 * @param  [out]  tag Output tag.
 * @param  [in]   tag_len The length of the tag to generate in bytes. Only support 4, 6, 8, 10, 12, 14 or 16.
 * @retval ERRCODE_SUCC Success.
 * @retval Other         Failure. For details, see @ref errcode_t
 * @else
 * @brief  CCM 获取 tag 值。
 * @param  [in]   ctx 加解密句柄。
 * @param  [out]  tag 输出的 tag。
 * @param  [in]   tag_len 生成的 tag 值的长度，单位是字节。 只支持 4, 6, 8, 10, 12, 14 或者 16。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other         失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_symc_ccm_finish(cipher_handle_t ctx, uint8_t *tag, uint32_t tag_len);

/**
 * @if Eng
 * @brief  Destroy the handle for CCM.
 * @param  [in]   ctx The symc handle to be destroyed.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  CCM 销毁句柄。
 * @param  [in]   ctx 要销毁的 symc 句柄。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_symc_ccm_teardown(cipher_handle_t ctx);

/**
 * @if Eng
 * @brief  GCM encrypt and compute tag.
 * @param  [in]    alg Crypto Alg. see @ref uapi_drv_cipher_symc_alg_t.
 * @param  [in]    key Encryption key.
 * @param  [in]    key_len Encryption key length in byte. Only support 16,24,32.
 * @param  [in]    keyslot_handle The keyslot handle which store the encryption key.
 * @param  [in]    iv The initial vector. NULL is support.
 * @param  [in]    iv_len The initial vector lenght in byte. Any length supported.
 * @param  [in]    aad The additional authentication message. NULL is support.
 * @param  [in]    aad_len The additional authentication message length in byte.
 * @param  [in]    plain_text Input plain text.
 * @param  [out]   cipher_text Output cipher text.
 * @param  [in]    data_len data length in byte.
 * @param  [out]   tag Output tag.
 * @param  [in]    tag_len The expected tag length in byte.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @note   There are two ways to set the key:
            1）Use the key/key_len combination. In this case, keyslot_handle is set to UAPI_DRV_INVALID_KEY_SLOT.
            2）Use the keyslot_handle/key_len combination to pass in the keyslot handle created
                by the keyslot interface. In this case, the key is NULL.
 * @else
 * @brief  GCM 加密并计算 tag.
 * @param  [in]    alg 加解密算法. 见 @ref uapi_drv_cipher_symc_alg_t 。
 * @param  [in]    key 加密密钥.
 * @param  [in]    key_len 加密密钥长度，单位是 Byte. 只支持 16,24,32.
 * @param  [in]    keyslot_handle 保存加密密钥的 keyslot 句柄.
 * @param  [in]    iv 初始向量, 支持传入为 NULL.
 * @param  [in]    iv_len 初始向量长度，单位是 Byte. 支持任意长度.
 * @param  [in]    aad 附加认证信息，支持传入为 NULL
 * @param  [in]    aad_len 附加认证信息，单位是 Byte.
 * @param  [in]    plain_text 输入的明文.
 * @param  [out]   cipher_text 输出的密文.
 * @param  [in]    data_len 数据长度，单位是 Byte.
 * @param  [out]   tag 输出的 tag
 * @param  [in]    tag_len 期望的 tag 长度，单位是 Byte.
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @note   有两种方法设置密钥:
            1）使用 key/key_len 组合，在这种情况下，keyslot_handle 传入 UAPI_DRV_INVALID_KEY_SLOT。
            2）使用 keyslot_handle/key_len 组合，keyslot 接口创建 keyslot 句柄传入，在这种情况下，key 传入为 NULL。
 * @endif
 */
errcode_t uapi_drv_cipher_symc_gcm_encrypt(uapi_drv_cipher_symc_alg_t alg,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *iv, uint32_t iv_len,
    const uint8_t *aad, uint32_t aad_len,
    const uint8_t *plain_text, uint8_t *cipher_text, uint32_t data_len,
    uint8_t *tag, uint32_t tag_len);

/**
 * @if Eng
 * @brief  GCM decrypt and verify tag.
 * @param  [in]    alg Crypto Alg. see @ref uapi_drv_cipher_symc_alg_t.
 * @param  [in]    key Decryption key.
 * @param  [in]    key_len Decryption key length in byte. Only support 16,24,32.
 * @param  [in]    keyslot_handle The keyslot handle which store the decryption key.
 * @param  [in]    iv The initial vector. NULL is support.
 * @param  [in]    iv_len The initial vector lenght in byte. Any length supported.
 * @param  [in]    aad The additional authentication message. NULL is support.
 * @param  [in]    aad_len The additional authentication message length in byte.
 * @param  [in]    cipher_text Input cipher text.
 * @param  [out]   plain_text Output plain text.
 * @param  [in]    data_len data length in byte.
 * @param  [in]    tag Input tag to be verified.
 * @param  [in]    tag_len The tag length in byte.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @note   There are two ways to set the key:
            1）Use the key/key_len combination. In this case, keyslot_handle is set to UAPI_DRV_INVALID_KEY_SLOT.
            2）Use the keyslot_handle/key_len combination to pass in the keyslot handle created
                by the keyslot interface. In this case, the key is NULL.
 * @else
 * @brief  GCM 解密并校验 tag.
 * @param  [in]    alg 加解密算法. 见 @ref uapi_drv_cipher_symc_alg_t 。
 * @param  [in]    key 解密密钥.
 * @param  [in]    key_len 解密密钥长度，单位是 Byte. 只支持 16,24,32.
 * @param  [in]    keyslot_handle 保存解密密钥的 keyslot 句柄.
 * @param  [in]    iv 初始向量, 支持传入为 NULL.
 * @param  [in]    iv_len 初始向量长度，单位是 Byte. 支持任意长度.
 * @param  [in]    aad 附加认证信息，支持传入为 NULL.
 * @param  [in]    aad_len 附加认证信息，单位是 Byte.
 * @param  [in]    cipher_text 输入的密文.
 * @param  [out]   plain_text 输出的明文.
 * @param  [in]    data_len 数据长度，单位是 Byte.
 * @param  [in]   tag 待校验的 tag.
 * @param  [in]    tag_len tag 长度，单位是 Byte.
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @note   有两种方法设置密钥:
            1）使用 key/key_len 组合，在这种情况下，keyslot_handle 传入 UAPI_DRV_INVALID_KEY_SLOT。
            2）使用 keyslot_handle/key_len 组合，keyslot 接口创建 keyslot 句柄传入，在这种情况下，key 传入为 NULL。
 * @endif
 */
errcode_t uapi_drv_cipher_symc_gcm_decrypt_verify(uapi_drv_cipher_symc_alg_t alg,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *iv, uint32_t iv_len,
    const uint8_t *aad, uint32_t aad_len,
    const uint8_t *cipher_text, uint8_t *plain_text, uint32_t data_len,
    const uint8_t *tag, uint32_t tag_len);

/**
 * @if Eng
 * @brief  Initialize the context for multi-part GCM encryption or decryption.
 * @param  [out]   ctx Created symc handle.
 * @param  [in]    alg Crypto Alg. see @ref uapi_drv_cipher_symc_alg_t.
 * @param  [in]    key Encryption key.
 * @param  [in]    key_len Encryption key length in byte. Only support 16,24,32.
 * @param  [in]    keyslot_handle The keyslot handle which store the encryption key.
 * @param  [in]    iv The initial vector.
 * @param  [in]    iv_len The initial vector lenght in byte.
 * @param  [in]    is_encrypt Encrypted or not.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @note   There are two ways to set the key:
            1）Use the key/key_len combination. In this case, keyslot_handle is set to UAPI_DRV_INVALID_KEY_SLOT.
            2）Use the keyslot_handle/key_len combination to pass in the keyslot handle created
                by the keyslot interface. In this case, the key is NULL.
 * @else
 * @brief  初始化 GCM 分段加解密的上下文句柄。
 * @param  [out]   ctx 创建的加解密句柄。
 * @param  [in]    alg 加解密算法. 见 @ref uapi_drv_cipher_symc_alg_t 。
 * @param  [in]    key 加密密钥.
 * @param  [in]    key_len 加密密钥长度，单位是 Byte. 只支持 16,24,32.
 * @param  [in]    keyslot_handle 保存加密密钥的 keyslot 句柄.
 * @param  [in]    iv 初始向量
 * @param  [in]    iv_len 初始向量长度，单位是 Byte.
 * @param  [in]    is_encrypt 是否加密。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @note   有两种方法设置密钥:
            1）使用 key/key_len 组合，在这种情况下，keyslot_handle 传入 UAPI_DRV_INVALID_KEY_SLOT。
            2）使用 keyslot_handle/key_len 组合，keyslot 接口创建 keyslot 句柄传入，在这种情况下，key 传入为 NULL。
 * @endif
 */
errcode_t uapi_drv_cipher_symc_gcm_setup(cipher_handle_t *ctx, uapi_drv_cipher_symc_alg_t alg,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *iv, uint32_t iv_len, bool is_encrypt);

/**
 * @if Eng
 * @brief  Add addition data for GCM. You may call this function zero, one or more times
 *                  to pass successive parts of the additional data.
 * @param  [in]    ctx symc handle.
 * @param  [in]    aad The additional data.
 * @param  [in]    aad_len The length of additional data in bytes for this times.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  为 GCM 增加附加数据。您可以零次、一次或多次调用此函数来传递分段的附加数据。
 * @param  [in]    ctx symc handle.
 * @param  [in]    aad 附加数据。
 * @param  [in]    aad_len 此次附件数据的长度，单位是字节。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_symc_gcm_update_ad(cipher_handle_t ctx, const uint8_t *aad, uint32_t aad_len);

/**
 * @if Eng
 * @brief  Multi-part encryption or decryption for GCM mode.
 * @param  [in]   ctx Symc handle.
 * @param  [in]   src Source data.
 * @param  [out]  dst Destination data.
 * @param  [in]   data_len Data length in byte. Return success if data_len is zero.
 * @retval ERRCODE_SUCC Success.
 * @retval Other         Failure. For details, see @ref errcode_t
 * @else
 * @brief  GCM 模式下的分段加解密。
 * @param  [in]   ctx 加解密句柄。
 * @param  [in]   src 源数据。
 * @param  [out]  dst 目的数据。
 * @param  [in]   data_len 数据长度，单位是 Byte。传入0，接口返回成功。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other         失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_symc_gcm_update(cipher_handle_t ctx, const uint8_t *src, uint8_t *dst, uint32_t data_len);

/**
 * @if Eng
 * @brief Obtain tag for GCM.
 * @param  [in]   ctx Symc handle.
 * @param  [out]  tag Output tag.
 * @param  [in]   tag_len The length of tag to obtain in bytes.
 * @retval ERRCODE_SUCC Success.
 * @retval Other         Failure. For details, see @ref errcode_t
 * @else
 * @brief  GCM 获取 tag 值。
 * @param  [in]   ctx 加解密句柄。
 * @param  [out]  tag 输出的 tag。
 * @param  [in]   tag_len 获取的 tag 长度，单位是 Byte。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other         失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_symc_gcm_finish(cipher_handle_t ctx, uint8_t *tag, uint32_t tag_len);

/**
 * @if Eng
 * @brief  Destroy the handle for GCM.
 * @param  [in]   ctx The symc handle to be destroyed.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  GCM 销毁句柄。
 * @param  [in]   ctx 要销毁的 symc 句柄。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_symc_gcm_teardown(cipher_handle_t ctx);

/**
 * @if Eng
 * @brief  DMA Copy from src address to dst address.
 * @param  [in]   src_addr source data address.
 * @param  [in]   dst_addr destination data address.
 * @param  [in]   len data length in byte.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  DMA 拷贝从源数据地址到目的数据地址。
 * @param  [in]   src_addr 源数据地址
 * @param  [in]   dst_addr 目的数据地址
 * @param  [in]   len 数据长度，单位是 byte。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_symc_dma_copy(uint64_t src_addr, uint64_t dst_addr, uint32_t len);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  /* SECURITY_SYMC_H */