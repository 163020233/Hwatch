/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: the header file of mac API. \n
 * \n
 * Create: 2024-11-13
*/

#ifndef SECURITY_MAC_H
#define SECURITY_MAC_H

#include "errcode.h"
#include "security_symc.h"
#include "security_km.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @defgroup security_unified_mac MAC
 * @ingroup  drivers_driver_security_unified
 * @{
 */

typedef uintptr_t mac_handle_t;

/* CBC-MAC(Single-Part). */
/**
 * @if Eng
 * @brief  CBC MAC compute.
 * @param  [in]     alg Crypto Alg. see @ref uapi_drv_cipher_symc_alg_t.
 * @param  [in]     key Symmetric key.
 * @param  [in]     key_len Length of symmetric key, in byte.
 * @param  [in]     keyslot_handle Handle for storing the key. The handle is created and destroyed by the KM module.
 *                  If not need please set as UAPI_DRV_INVALID_KEY_SLOT.
 * @param  [in]     data Message data.
 * @param  [in]     data_len The length of message data in bytes.
 * @param  [out]    out_mac Output mac.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @note   There are two ways to set the key:
            1）Use the key/key_len combination. In this case, keyslot_handle is set to UAPI_DRV_INVALID_KEY_SLOT。.
            2）Use the keyslot_handle/key_len combination to pass in the keyslot handle created
                by the keyslot interface. In this case, the key is NULL.
 * @else
 * @brief  CBC MAC 计算。
 * @param  [in]     alg 加解密算法. 见 @ref uapi_drv_cipher_symc_alg_t 。
 * @param  [in]     key 对称密钥。
 * @param  [in]     key_len 对称密钥长度，单位是 Byte。
 * @param  [in]     keyslot_handle 用于保存key的句柄，通过 km 模块创建和销毁。
 *                  如果不需要，请设置为 UAPI_DRV_INVALID_KEY_SLOT。
 * @param  [in]     data 消息数据。
 * @param  [in]     data_len 消息数据长度，单位是 Byte。
 * @param  [out]    out_mac 输出的 mac 值。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @note   有两种方法设置密钥:
            1）使用 key/key_len 组合，在这种情况下，keyslot_handle 传入 UAPI_DRV_INVALID_KEY_SLOT。.
            2）使用 keyslot_handle/key_len 组合，keyslot 接口创建 keyslot 句柄传入，在这种情况下，key 传入为 NULL.
 * @endif
 */
errcode_t uapi_drv_cipher_cbc_mac(uapi_drv_cipher_symc_alg_t alg,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *data, uint32_t data_len, uint8_t out_mac[16]);

/* CBC-MAC(Multi-Part). */
/**
 * @if Eng
 * @brief  Initialize the context for multi-part CBC MAC.
 * @param  [out]    ctx Created mac handle.
 * @param  [in]     alg Crypto Alg. see @ref uapi_drv_cipher_symc_alg_t.
 * @param  [in]     key Symmetric key.
 * @param  [in]     key_len Length of symmetric key, in byte.
 * @param  [in]     keyslot_handle Handle for storing the key. The handle is created and destroyed by the KM module.
 *                  If not need please set as UAPI_DRV_INVALID_KEY_SLOT.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @note   There are two ways to set the key:
            1）Use the key/key_len combination. In this case, keyslot_handle is set to UAPI_DRV_INVALID_KEY_SLOT。.
            2）Use the keyslot_handle/key_len combination to pass in the keyslot handle created
                by the keyslot interface. In this case, the key is NULL.
 * @else
 * @brief  初始化 CBC MAC 分段计算的上下文句柄。
 * @param  [out]    ctx 创建的 MAC 句柄。
 * @param  [in]     alg 加解密算法. 见 @ref uapi_drv_cipher_symc_alg_t 。
 * @param  [in]     key 对称密钥。
 * @param  [in]     key_len 对称密钥长度，单位是 Byte。
 * @param  [in]     keyslot_handle 用于保存key的句柄，通过 km 模块创建和销毁。
 *                  如果不需要，请设置为 UAPI_DRV_INVALID_KEY_SLOT。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @note   有两种方法设置密钥:
            1）使用 key/key_len 组合，在这种情况下，keyslot_handle 传入 UAPI_DRV_INVALID_KEY_SLOT。.
            2）使用 keyslot_handle/key_len 组合，keyslot 接口创建 keyslot 句柄传入，在这种情况下，key 传入为 NULL.
 * @endif
 */
errcode_t uapi_drv_cipher_cbc_mac_start(mac_handle_t *ctx,
    uapi_drv_cipher_symc_alg_t alg, const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle);

/**
 * @if Eng
 * @brief  CBC MAC multi-part compute.
 * @param  [in]   ctx MAC handle.
 * @param  [in]     data Message data.
 * @param  [in]     data_len The length of message data in bytes.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  CBC MAC 分段计算。
 * @param  [in]   ctx MAC 句柄。
 * @param  [in]     data 消息数据。
 * @param  [in]     data_len 消息数据长度，单位是 Byte。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_cbc_mac_update(mac_handle_t ctx, const uint8_t *data, uint32_t data_len);

/**
 * @if Eng
 * @brief  CBC MAC compute MAC.
 * @param  [in]   ctx MAC handle.
 * @param  [out]    out_mac Output mac.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  CBC MAC 计算 MAC 值。
 * @param  [in]   ctx MAC 句柄。
 * @param  [out]    out_mac 输出的 mac 值。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_cbc_mac_finish(mac_handle_t ctx, uint8_t out_mac[16]);

/**
 * @if Eng
 * @brief  Destroy MAC handle.
 * @param  [in]   ctx MAC handle.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  销毁 MAC 句柄。
 * @param  [in]   ctx MAC 句柄。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_cbc_mac_destroy(mac_handle_t ctx);


/* CMAC(Single-Part). */
/**
 * @if Eng
 * @brief  CMAC compute.
 * @param  [in]     alg Crypto Alg. see @ref uapi_drv_cipher_symc_alg_t. Support AES only.
 * @param  [in]     key Symmetric key.
 * @param  [in]     key_len Length of symmetric key, in byte.
 * @param  [in]     keyslot_handle Handle for storing the key. The handle is created and destroyed by the KM module.
 *                  If not need please set as UAPI_DRV_INVALID_KEY_SLOT.
 * @param  [in]     data Message data.
 * @param  [in]     data_len The length of message data in bytes. 0 byte is not support.
 * @param  [out]    out_mac Output mac.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @note   There are two ways to set the key:
            1）Use the key/key_len combination. In this case, keyslot_handle is set to UAPI_DRV_INVALID_KEY_SLOT。.
            2）Use the keyslot_handle/key_len combination to pass in the keyslot handle created
                by the keyslot interface. In this case, the key is NULL.
 * @else
 * @brief  CMAC 计算。
 * @param  [in]     alg 加解密算法. 见 @ref uapi_drv_cipher_symc_alg_t 。只支持 AES 算法.
 * @param  [in]     key 对称密钥。
 * @param  [in]     key_len 对称密钥长度，单位是 Byte。
 * @param  [in]     keyslot_handle 用于保存key的句柄，通过 km 模块创建和销毁。
 *                  如果不需要，请设置为 UAPI_DRV_INVALID_KEY_SLOT。
 * @param  [in]     data 消息数据。
 * @param  [in]     data_len 消息数据长度，单位是 Byte。CMAC 不支持传入长度为 0.
 * @param  [out]    out_mac 输出的 mac 值。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @note   有两种方法设置密钥:
            1）使用 key/key_len 组合，在这种情况下，keyslot_handle 传入 UAPI_DRV_INVALID_KEY_SLOT。.
            2）使用 keyslot_handle/key_len 组合，keyslot 接口创建 keyslot 句柄传入，在这种情况下，key 传入为 NULL.
 * @endif
 */
errcode_t uapi_drv_cipher_cmac(uapi_drv_cipher_symc_alg_t alg,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *data, uint32_t data_len, uint8_t out_mac[16]);

/* CMAC(Multi-Part). */
/**
 * @if Eng
 * @brief  Initialize the context for multi-part CMAC.
 * @param  [out]    ctx Created mac handle.
 * @param  [in]     alg Crypto Alg. see @ref uapi_drv_cipher_symc_alg_t.
 * @param  [in]     key Symmetric key.
 * @param  [in]     key_len Length of symmetric key, in byte.
 * @param  [in]     keyslot_handle Handle for storing the key. The handle is created and destroyed by the KM module.
 *                  If not need please set as UAPI_DRV_INVALID_KEY_SLOT.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @note   There are two ways to set the key:
            1）Use the key/key_len combination. In this case, keyslot_handle is set to UAPI_DRV_INVALID_KEY_SLOT。.
            2）Use the keyslot_handle/key_len combination to pass in the keyslot handle created
                by the keyslot interface. In this case, the key is NULL.
 * @else
 * @brief  初始化 CMAC 分段计算的上下文句柄。
 * @param  [out]    ctx 创建的 MAC 句柄。
 * @param  [in]     alg 加解密算法. 见 @ref uapi_drv_cipher_symc_alg_t 。
 * @param  [in]     key 对称密钥。
 * @param  [in]     key_len 对称密钥长度，单位是 Byte。
 * @param  [in]     keyslot_handle 用于保存key的句柄，通过 km 模块创建和销毁。
 *                  如果不需要，请设置为 UAPI_DRV_INVALID_KEY_SLOT。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @note   有两种方法设置密钥:
            1）使用 key/key_len 组合，在这种情况下，keyslot_handle 传入 UAPI_DRV_INVALID_KEY_SLOT。.
            2）使用 keyslot_handle/key_len 组合，keyslot 接口创建 keyslot 句柄传入，在这种情况下，key 传入为 NULL.
 * @endif
 */
errcode_t uapi_drv_cipher_cmac_start(mac_handle_t *ctx,
    uapi_drv_cipher_symc_alg_t alg, const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle);

/**
 * @if Eng
 * @brief  CMAC multi-part compute.
 * @param  [in]   ctx MAC handle.
 * @param  [in]     data Message data.
 * @param  [in]     data_len The length of message data in bytes.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  CMAC 分段计算。
 * @param  [in]   ctx MAC 句柄。
 * @param  [in]     data 消息数据。
 * @param  [in]     data_len 消息数据长度，单位是 Byte。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_cmac_update(mac_handle_t ctx, const uint8_t *data, uint32_t data_len);

/**
 * @if Eng
 * @brief  CMAC compute MAC.
 * @param  [in]   ctx MAC handle.
 * @param  [out]    out_mac Output mac.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @note   If the uapi_drv_cipher_cmac_update interface is not invoked or the value of data_len transferred by
 *              the uapi_drv_cipher_cmac_update interface is 0, return failure.
 * @else
 * @brief  CMAC 计算 MAC 值。
 * @param  [in]   ctx MAC 句柄。
 * @param  [out]    out_mac 输出的 mac 值。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @note   若未调用 uapi_drv_cipher_cmac_update 接口或调用 uapi_drv_cipher_cmac_update 接口传入的 data_len 为 0，
 *          则该接口返回失败。
 * @endif
 */
errcode_t uapi_drv_cipher_cmac_finish(mac_handle_t ctx, uint8_t out_mac[16]);

/**
 * @if Eng
 * @brief  Destroy MAC handle.
 * @param  [in]   ctx MAC handle.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  销毁 MAC 句柄。
 * @param  [in]   ctx MAC 句柄。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_cmac_destroy(mac_handle_t ctx);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  /* SECURITY_SYMC_H */