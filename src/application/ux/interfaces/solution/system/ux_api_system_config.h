/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provide system config \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-08-06， Create file. \n
 */
#ifndef UX_API_SYSTEM_CONFIG_H
#define UX_API_SYSTEM_CONFIG_H
#include "stdbool.h"
#include "errcode.h"
#include "calendar.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SDK_VERSION_LEN_MAX  32                                  /**< SDK版本号最大长度 */
#define CHIP_VERSION_LEN_MAX 8                                   /**< 芯片版本号最大长度 */

#define VERSION_MASK_SDK              (1 << 0)                   /**< 0x01:支持SDK版本查询 */
#define VERSION_MASK_FLASHBOOT        (1 << 1)                   /**< 0x02:支持SSB版本查询 */
#define VERSION_MASK_RECOVERY         (1 << 2)                   /**< 0x04:支持Recovery版本查询 */
#define VERSION_MASK_APPLICATION      (1 << 3)                   /**< 0x08:支持App版本查询 */
#define VERSION_MASK_BT               (1 << 4)                   /**< 0x10:支持蓝牙版本查询 */
#define VERSION_MASK_CHIP             (1 << 5)                   /**< 0x20:支持芯片版本查询 */

/**
 * @brief 传输模式枚举
 */
typedef enum {
    UX_API_SYS_TRANSMIT_MODE_1R2T, /**< 2个发射器搭配1个接收器 */
    UX_API_SYS_TRANSMIT_MODE_1R4T, /**< 4个发射器搭配1个接收器 */
    UX_API_SYS_TRANSMIT_MODE_1R8T, /**< 8个发射器搭配1个接收器 */
    UX_API_SYS_TRANSMIT_MODE_MAX,  /**< 最大值 */
} ux_api_sys_transmit_mode_t;

/**
 * @brief 文件格式化回调函数类型定义
 */
typedef void (*ux_api_file_format_handler)(int result);

/**
 * @brief 版本信息结构体
 */
typedef struct {
    uint8_t version_mask;   /**< 版本查询支持掩码 */
    char sdk_version[SDK_VERSION_LEN_MAX];  /**< sdk版本号 */
    struct {
        uint32_t image_version;
        uint32_t key_version;
    } flashboot_image;   /**< flash boot镜像版本号以及对应key版本号 */
    struct {
        uint32_t image_version;
        uint32_t key_version;
    } recovery_image;    /**< recovery镜像版本号以及对应key版本号 */
    struct {
        uint32_t image_version;
        uint32_t key_version;
    } application_image; /**< application镜像版本号以及对应key版本号 */
    struct {
        uint32_t image_version;
        uint32_t key_version;
    } bt_image;          /**< bt镜像版本号以及对应key版本号 */
    uint8_t chip_version[CHIP_VERSION_LEN_MAX];  /**< 芯片版本号 */
    uint8_t reserved[32];                        /**< 保留字段 */
} ux_api_version_info_t;

/**
 * @brief 专用于获取sdk版本号
 * @param [out] sdk_version 用于存放读出sdk版本号的缓冲区指针
 * @param [in] sdk_version_len 缓冲区对应长度
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_get_sdk_version(char *sdk_version, uint8_t sdk_version_len);

/**
 * @brief 通过设备ID获取SDK版本号
 * @param [in] device_id 设备ID
 * @param [out] sdk_version SDK版本号缓冲区
 * @param [in] data_len 缓冲区长度
 * @return ERRCODE_SUCC 成功；Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_sys_get_sdk_version_by_id(uint16_t device_id, char* sdk_version, uint16_t data_len);

/**
 * @brief 用于批量获取系统版本信息
 * @param [out] version_info 用于存放读出系统信息的缓冲区指针
 * @param [in] version_info_size 缓冲区对应长度
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_get_all_version(ux_api_version_info_t *version_info, uint32_t version_info_size);

/**
 * @brief 执行文件格式化并调用回调函数
 * @param [in] handler 文件格式化完成后回调函数指针
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_sys_file_async_format(ux_api_file_format_handler handler);

/**
 * @brief 获取XTS运行标志
 * @return XTS运行标志；true 设置，false 不设置
 */
bool ux_api_sys_get_xts_flag(void);

/**
 * @brief 设置XTS运行标志并重启
 * @param [in] flag XTS运行标志；true 设置，false 不设置
 * @return ERRCODE_SUCC 成功；系统重启
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_sys_set_xts_flag(bool flag);

/**
 * @brief 设置直播麦克风模式
 * @param [in] transmit_mode 设置的直播麦克风模式，参考 @ref ux_api_sys_transmit_mode_t
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_sys_set_transmit_mode(ux_api_sys_transmit_mode_t transmit_mode);

/**
 * @brief 获取当前直播麦克风模式
 * @return 当前设置的直播麦克风模式，参考 @ref ux_api_sys_transmit_mode_t
 */
ux_api_sys_transmit_mode_t ux_api_sys_get_transmit_mode(void);

/**
 * @brief 设置系统语言
 * @param [in] language 语言编号
 * @return ERRCODE_SUCC 成功；Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_sys_set_language(uint8_t language);

/**
 * @brief 获取系统语言
 * @return 语言编号
 */
uint8_t ux_api_sys_get_language(void);

/**
 * @brief 设置系统时间
 * @param [in] calendar 时间结构体
 * @return ERRCODE_SUCC 成功；Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_sys_set_time(calendar_t *calendar);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
