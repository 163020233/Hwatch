/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024. All rights reserved.
 * Description: SLE Device Manager, module.
 */

/**
 * @defgroup sle_device_manager Device Manager API
 * @ingroup  SLE
 * @{
 */

#ifndef SLE_DEVICE_MANAGER
#define SLE_DEVICE_MANAGER

#include <stdint.h>
#include "errcode.h"
#include "sle_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief Enum of SLE power status.
 * @else
 * @brief SLE上电状态枚举.
 * @endif
 */
typedef enum {
    SLE_POWER_ON_SUCCESS,       /*!< @if Eng SLE power on success.
                                     @else   SLE上电成功. @endif */
    SLE_POWER_ON_FAIL,          /*!< @if Eng SLE power on fail.
                                     @else   SLE上电失败. @endif */
} sle_power_on_status_t;

/**
 * @if Eng
 * @brief Enum of SLE enable/disable status.
 * @else
 * @brief SLE使能/去使能状态枚举.
 * @endif
 */
typedef enum {
    SLE_ENABLE_DISABLE_SUCCESS,       /*!< @if Eng SLE enable/disable success.
                                           @else   SLE使能/去使能成功. @endif */
    SLE_ENABLE_DISABLE_FAIL,          /*!< @if Eng SLE enable/disable fail.
                                           @else   SLE使能/去使能失败. @endif */
} sle_enable_disable_status_t;

/**
 * @if Eng
 * @brief Enum of SLE config type.
 * @else
 * @brief SLE配置类型枚举.
 * @endif
 */
typedef enum {
    SLE_CONFIG_KEYMISSING_PROCESS_METHOD,   /*!< @if Eng SLE set processing method when pair result is keymissing.
                                               { @ref sle_config_keymissing_param_t }
                                                 @else   SLE 配对结果为keymissing时处理方式.
                                               { @ref sle_config_keymissing_param_t }
                                                 @endif */
} sle_config_type_t;

/**
 * @if Eng
 * @brief  The process method when pair result is keymissing.
 * @else
 * @brief  Keymissing时处理方式。
 * @endif
 */
typedef struct {
    uint16_t conn_id;           /*!< @if Eng connection ID
                                     @else   连接ID @endif */
    uint8_t is_disconnect;      /*!< @if Eng Mark whether the link disconnect when
                                             pair result is keymissing, 0:no disconnect, 1: disconnect.
                                     @else  标记配对结果为keymissing时是否断连，0：不断连， 1：断连  @endif */
    uint32_t reserve;           /*!< @if Eng reserve, "0" for default value.
                                     @else   预留，默认值写0 @endif */
} sle_config_keymissing_param_t;

/**
 * @if Eng
 * @brief Callback invoked when device power on.
 * @par Callback invoked when device power on.
 * @attention 1.This function is called in SLE service context,should not be blocked or do long time waiting.
 * @attention 2.The memories of pointer are requested and freed by the SLE service automatically.
 * @param [in] status error code.
 * @retval void no return value.
 * @par Dependency:
 * @li  sle_common.h
 * @see sle_dev_manager_callbacks_t
 * @else
 * @brief  SLE设备上电。
 * @par    SLE设备上电。
 * @attention  1. 该回调函数运行于SLE service线程，不能阻塞或长时间等待。
 * @attention  2. 指针由SLE service申请内存，也由SLE service释放，回调中不应释放。
 * @param [in] status 执行结果错误码。
 * @retval 无返回值。
 * @par 依赖:
 * @li  sle_common.h
 * @see sle_dev_manager_callbacks_t
 * @endif
 */
typedef void (*sle_power_on_callback)(uint8_t status);

/**
 * @if Eng
 * @brief Callback invoked when SLE stack enable.
 * @par Callback invoked when SLE stack enable.
 * @attention 1.This function is called in SLE service context,should not be blocked or do long time waiting.
 * @attention 2.The memories of pointer are requested and freed by the SLE service automatically.
 * @param [in] status enable status, { @ref sle_enable_disable_status_t }.
 * @retval void no return value.
 * @par Dependency:
 * @li  sle_common.h
 * @see sle_dev_manager_callbacks_t
 * @else
 * @brief  SLE协议栈使能。
 * @par    SLE协议栈使能。
 * @attention  1. 该回调函数运行于SLE service线程，不能阻塞或长时间等待。
 * @attention  2. 指针由SLE service申请内存，也由SLE service释放，回调中不应释放。
 * @param [in] status 使能状态, { @ref sle_enable_disable_status_t }.
 * @retval 无返回值。
 * @par 依赖:
 * @li  sle_common.h
 * @see sle_dev_manager_callbacks_t
 * @endif
 */
typedef void (*sle_enable_callback)(uint8_t status);

/**
 * @if Eng
 * @brief Callback invoked when SLE stack disable.
 * @par Callback invoked when SLE stack disable.
 * @attention 1.This function is called in SLE service context,should not be blocked or do long time waiting.
 * @attention 2.The memories of pointer are requested and freed by the SLE service automatically.
 * @param [in] status disable status, { @ref sle_enable_disable_status_t }.
 * @retval void no return value.
 * @par Dependency:
 * @li  sle_common.h
 * @see sle_dev_manager_callbacks_t
 * @else
 * @brief  SLE协议栈去使能。
 * @par    SLE协议栈去使能。
 * @attention  1. 该回调函数运行于SLE service线程，不能阻塞或长时间等待。
 * @attention  2. 指针由SLE service申请内存，也由SLE service释放，回调中不应释放。
 * @param [in] status 去使能状态, { @ref sle_enable_disable_status_t }.
 * @retval 无返回值。
 * @par 依赖:
 * @li  sle_common.h
 * @see sle_dev_manager_callbacks_t
 * @endif
 */
typedef void (*sle_disable_callback)(uint8_t status);

/**
 * @if Eng
 * @brief Struct of SLE device announce callback function.
 * @else
 * @brief SLE 设备公开回调函数接口定义。
 * @endif
 */
typedef struct {
    sle_power_on_callback sle_power_on_cb;                  /*!< @if Eng SLE device power on callback.
                                                                 @else   SLE设备上电回调函数。 @endif */
    sle_enable_callback sle_enable_cb;                      /*!< @if Eng SLE stack enable callback.
                                                                 @else   SLE协议栈使能回调函数。 @endif */
    sle_disable_callback sle_disable_cb;                    /*!< @if Eng SLE stack disable callback.
                                                                 @else   SLE协议栈去使能回调函数。 @endif */
} sle_dev_manager_callbacks_t;

/**
 * @if Eng
 * @brief  Enable SLE stack.
 * @par Description: Enable SLE stack.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @par Depends:
 * @li sle_common.h
 * @else
 * @brief  使能SLE协议栈。
 * @par Description: 使能SLE协议栈。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @par 依赖：
 * @li sle_common.h
 * @endif
 */
errcode_t enable_sle(void);

/**
 * @if Eng
 * @brief  Disable SLE stack.
 * @par Description: Disable SLE stack.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @par Depends:
 * @li sle_common.h
 * @else
 * @brief  关闭SLE协议栈。
 * @par Description: 关闭SLE协议栈。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @par 依赖：
 * @li sle_common.h
 * @endif
 */
errcode_t disable_sle(void);

/**
 * @if Eng
 * @brief  Registering the Path for Storing SLE Device Persistence Information.
 * @par    Registering the Path for Storing SLE Device Persistence Information.
 * @attention Applicable to the Linux system, an encryption key must be set.
 * @param [in] path     file store path, including the null-terminated character.
 * @param [in] key      Key used for file persistence encryption. The key length is 16 bytes.
 * @param [in] key_len  Key length, fixed at 16.
 * @retval error code.
 * @else
 * @brief  注册SLE设备持久化信息存储路径。
 * @par    注册SLE设备持久化信息存储路径。
 * @attention 适用于linux系统，必须设置加密秘钥。
 * @param [in] path     文件持久化路径，包含结束符'\0'。
 * @param [in] key      用于文件持久化加密的秘钥, 秘钥长度16字节。
 * @param [in] key_len  秘钥长度，固定16。
 * @retval 执行结果错误码。
 * @endif
 */
errcode_t sle_dev_manager_register_file_path(const uint8_t *path, const uint8_t *key, uint8_t key_len);

/**
 * @if Eng
 * @brief  Device Management Settings Configuration function.
 * @par Description: Device Management Settings Configuration function.
 * @param [in] type Config type { @ref sle_config_type_t }.
 * @param [in] data Configuration parameters, corresponding configuration types:
                    SLE_CONFIG_KEYMISSING_PROCESS_METHOD: { @ref sle_config_keymissing_param_t }.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @par Depends:
 * @li sle_common.h
 * @else
 * @brief  SLE设备管理设置配置函数。
 * @par Description: SLE设备管理设置配置函数。
 * @param [in] type 配置类型 { @ref sle_config_type_t }。
 * @param [in] data 配置参数，对应配置类型：
                    SLE_CONFIG_KEYMISSING_PROCESS_METHOD: { @ref sle_config_keymissing_param_t }。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @par 依赖：
 * @li sle_common.h
 * @endif
 */
errcode_t sle_dev_manager_set_config(uint16_t type, void* data);

/**
 * @if Eng
 * @brief  Register SLE device manager callbacks.
 * @par Description: Register SLE device manager callbacks.
 * @param [in] func Callback function.
 * @retval error code.
 * @else
 * @brief  注册SLE设备管理回调函数。
 * @par Description: 注册SLE设备管理回调函数。
 * @param [in] func 回调函数。
 * @retval 执行结果错误码。
 * @endif
 */
errcode_t sle_dev_manager_register_callbacks(sle_dev_manager_callbacks_t *func);

#ifdef __cplusplus
}
#endif
#endif /* SLE_DEVICE_MANAGER */
/**
 * @}
 */
