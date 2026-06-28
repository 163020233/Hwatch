/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024. All rights reserved.
 *
 * Description: SLE CHBA NET DEVICE PUBLIC API module.
 */
#ifndef SLE_CHBA_MANAGER_H
#define SLE_CHBA_MANAGER_H

#include "sle_chba_transmition.h"
#include "sle_common.h"

typedef enum {
    CHBA_ROLE_AP, /*!< @if Eng In AP mode, the device functions as the center of a wireless network, manages STAs,
                  provides data relay, and supports connections to multiple devices. The SLE functions as the client.
                  @else   AP模式下，设备作为无线网络中心，管理STA，提供数据中继，支持多设备连接。sle作为client端 @endif */
    CHBA_ROLE_STA, /*!< @if Eng In STA mode, the device functions as a leaf node. The terminal device connects to a
                   wireless network for data transmission. This end functions as the server.
                   @else   STA模式下，为叶子节点。终端设备连接到无线网络，进行数据传输。sle作为server端 @endif */
} sle_chba_role_t;

/**
 * @if Eng
 * @brief Use this funtion to create chba.
 * @par   Use this funtion to create chba.
 * @param  [in] chba_role chba role
 * @param  [in] chba_mode chba mode
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @par Dependency:
 * @li  sle_common.h
 * @else
 * @brief  创建chba模块。
 * @par    创建chba模块。
 * @param  [in] chba_role 设备角色
 * @param  [in] chba_mode chba模式，是否开启achba
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @par 依赖:
 * @li  sle_common.h
 * @endif
 */
errcode_t sle_chba_netdev_create(uint8_t chba_role, uint8_t chba_mode);

/**
 * @if Eng
 * @brief Use this funtion to destroy chba.
 * @par   Use this funtion to destroy chba.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @par Dependency:
 * @li  sle_common.h
 * @else
 * @brief  销毁chba模块。
 * @par    销毁chba模块。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @par 依赖:
 * @li  sle_common.h
 * @endif
 */
errcode_t sle_chba_netdev_destroy(void);

/**
 * @if Eng
 * @brief Use this funtion to add chba link.
 * @par   Use this funtion to add chba link.
 * @param  [in] conn_id sle connection ID
 * @param  [in] remote_addr remote sle address
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @par Dependency:
 * @li  sle_common.h
 * @else
 * @brief  添加chba链路。
 * @par    添加chba链路。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @par 依赖:
 * @li  sle_common.h
 * @endif
 */
errcode_t sle_chba_netdev_add_link(uint16_t conn_id, const sle_addr_t *remote_addr);

/**
 * @if Eng
 * @brief Use this funtion to delete chba link.
 * @par   Use this funtion to delete chba link.
 * @param  [in] conn_id sle connection ID
 * @param  [in] remote_addr remote sle address
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @par Dependency:
 * @li  sle_common.h
 * @else
 * @brief  删除chba链路。
 * @par    删除chba链路。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @par 依赖:
 * @li  sle_common.h
 * @endif
 */
errcode_t sle_chba_netdev_del_link(uint16_t conn_id, const sle_addr_t *remote_addr);
#endif