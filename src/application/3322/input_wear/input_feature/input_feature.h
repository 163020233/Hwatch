/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: button application header file \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-03-08, Create file. \n
 */
#ifndef INPUT_APP_H
#define INPUT_APP_H

#include "stdint.h"
#include "product.h"
#include "debug_print.h"

#define INPUT_DEV_NAME_LEN 15
#define APP_BOARD_MSG_COUNT 16
#define APP_BOARD_TASK_SIZE 0xA00
#define APP_BOARD_TASK_PRIO 5

#if !defined(SW_UART_DEBUG) && !defined(SW_RTT_DEBUG)
#define INPUT_FEATURE_PRINT_ERR(log_num, fmt, arg...) uapi_diag_error_log(0, fmt, ##arg)
#define INPUT_FEATURE_PRINT_WARN(log_num, fmt, arg...) uapi_diag_warning_log(0, fmt, ##arg)
#define INPUT_FEATURE_PRINT_INFO(log_num, fmt, arg...) uapi_diag_info_log(0, fmt, ##arg)
#ifdef WEARABLE_LOG_DEBUG
#define INPUT_FEATURE_PRINT_DEBUG(log_num, fmt, arg...) uapi_diag_debug_log(0, fmt, ##arg)
#else
#define INPUT_FEATURE_PRINT_DEBUG(log_num, fmt, arg...)
#endif
#else
#define INPUT_FEATURE_PRINT_ERR(log_num, fmt, arg...) PRINT("[INPUT E/] %s: " fmt "\r\n", __FUNCTION__, ##arg)
#define INPUT_FEATURE_PRINT_WARN(log_num, fmt, arg...) PRINT("[INPUT W/] %s: " fmt "\r\n", __FUNCTION__, ##arg)
#define INPUT_FEATURE_PRINT_INFO(log_num, fmt, arg...) PRINT("[INPUT I/] %s: " fmt "\r\n", __FUNCTION__, ##arg)
#ifdef WEARABLE_LOG_DEBUG
#define INPUT_FEATURE_PRINT_DEBUG(log_num, fmt, arg...) PRINT("[INPUT D/] %s: " fmt "\r\n", __FUNCTION__, ##arg)
#else
#define INPUT_FEATURE_PRINT_DEBUG(log_num, fmt, arg...)
#endif
#endif

typedef enum {
    APP_TOUCH,
    APP_KEY,
    APP_HEYBOARD,
    APP_MOUSE,
    APP_BUTTON,
    APP_CROWN,
    APP_ENCODER, /* *< Customized type of a specific function or event */
    APP_GESTURE,
    APP_USB_DEVICE,
    APP_INPUT_MAX,
} app_type_t;

typedef enum {
    APP_UI,
    APP_PMS,

    APP_SERVICE_MAX,
} app_notify_service_t;

/**
 * @brief Touch Panel事件类型枚举值
 *
 * TP数据上报时需要同时标识当前TP事件类型，比如按下、释放
 * 在standby mode上报的TP事件只包含当前事件的类型
 *
 */
typedef enum {
    /* * 手指触摸 */
    TP_PRESS = 0,
    /* * 手指抬起 */
    TP_RELEASE = 1,
    /* * 手指触摸后滑动 */
    TP_MOVE = 2,
    /* * 主表盘盖屏 */
    TP_COVER = 3,
    /* * 手指单击 */
    TP_SHORT_CLICK = 4,
    /* * 手指双击 */
    TP_DOUBLE_CLICK = 5,
    /* * 上滑 */
    TP_SLIDE_UP = 6,
    /* * 下滑 */
    TP_SLIDE_DOWN = 7,
    /* * 异常TP事件 */
    TP_INVALID,
} touch_event_type_t;

typedef struct {
    uint8_t event;     /* 给button使用 */
    uint8_t sub_event; /* 发给ux时需要填写 */
    uint8_t set_mode;
    uint8_t res[1];
    app_type_t app;
} app_comm_mail_t;

typedef struct {
    app_comm_mail_t comm;
    uint32_t data[8]; /* 用于传输输入事件的数据 */
    void *pri;
    void *config_buff;
    uint16_t config_len;
} common_notify_mail_t;

typedef struct {
    common_notify_mail_t mail;
} cmd_msg_t;

typedef struct {
    app_type_t type;
    union {
        struct {
            int16_t x;
            int16_t y;
        };
        struct {
            int16_t validFlag;
            int16_t activationFlag;
            uint32_t pred;
        };
        uint16_t keyId;
        uint16_t buttonId;
        int16_t rotate;
        int16_t encoderDiff;
    };
    uint16_t state;
    uint32_t timestamp;
} input_dev_report_data_t;

/**
 * @brief 输入设备工作模式枚举值
 */
typedef enum {
    /* * 输入设备正常模式 */
    INPUT_DEV_NORMAL_MODE = 0,
    /* * 输入设备待机模式 */
    INPUT_DEV_STANDBY_MODE,
    /* * 输入设备休眠模式 */
    INPUT_DEV_SLEEP_MODE,
} input_dev_workmode_t;

/**
 * @brief 调用方法
 */
typedef enum {
    /* * 设置工作模式调用方式为异步调用 */
    INPUT_SET_MODE_ASYN = 0,
    /* * 设置工作模式调用方式为阻塞调用 */
    INPUT_SET_MODE_SYN,
} call_method_t;

/**
 * @brief TP设备的信息
 */
typedef struct {
    /* * TP芯片ID */
    uint32_t chip_id;
    /* * TP固件版本号 */
    uint32_t fw_version;
    /* * TP芯片PanelID */
    uint32_t chip_hw_id;
} touch_chip_info_t;

/**
 * @brief TP设备原始值信息内容
 */
typedef struct {
    /* * TP设备原始数据缓存地址 */
    uint8_t *raw_data_buf;
    /* * TP设备原始数据缓存地址大小 */
    uint16_t raw_data_bufsize;
    /* * TP设备原始数据输出长度 */
    uint16_t raw_data_outsize;
} touch_raw_data_info_t;

/**
 * @brief Crown设备的信息
 */
typedef struct {
    /* * Crown芯片ID */
    uint32_t chipId;
} crown_chip_info_t;

/**
 * @brief 输入设备的信息
 */
typedef union {
    /* * TP设备信息 */
    touch_chip_info_t touch_chip_info;
    /* * 表冠设备信息 */
    crown_chip_info_t crown_chip_info;
} input_dev_chip_info_t;

/**
 * @brief 输入设备的寄存器信息
 */
typedef struct {
    /* * 输入设备寄存器寄存器地址 */
    uint32_t reg_addr;
    /* * 输入设备寄存器寄存器值 */
    uint32_t reg_value;
} input_dev_reg_info_t;

typedef void (*input_report_event_cb)(input_dev_report_data_t *data);
typedef void (*service_callback_func)(common_notify_mail_t *notify_msg);

typedef struct {
    uint32_t (*input_dev_register_service_cb)(app_notify_service_t service, input_report_event_cb callback);
    uint32_t (*input_dev_unregister_service_cb)(app_notify_service_t service);
    uint32_t (*input_dev_check_status)(void);
    uint32_t (*input_dev_set_workmode)(input_dev_workmode_t mode);
    input_dev_workmode_t (*input_dev_get_workmode)(void);
    uint32_t (*input_dev_get_status)(uint32_t *dev_status);
    uint32_t (*input_dev_clear_status)(void);
    uint32_t (*input_dev_self_check)(void);
    uint32_t (*input_dev_get_info)(input_dev_chip_info_t *dev_chip_info);
    uint32_t (*input_dev_read_register)(input_dev_reg_info_t *dev_reg_info);
    uint32_t (*input_dev_write_register)(input_dev_reg_info_t *dev_reg_info);
    void (*input_power_control)(uint8_t power_state);
    uint32_t (*input_dev_init)(void);
    uint32_t (*input_dev_deinit)(void);
} input_devops_t;

typedef struct {
    uint8_t dev_name[INPUT_DEV_NAME_LEN];
    app_type_t dev_type;
    input_devops_t dev_ops;
    input_devops_t *next;
} input_dev_info_t;

uint32_t input_app_init(void);
uint64_t get_app_queue_id(void);

/**
 * @brief 输入事件回调函数
 */
uint32_t input_register_service_cb(app_type_t type, service_callback_func callback);
uint32_t input_set_workmode(app_type_t type, input_dev_workmode_t work_mode, call_method_t method);
#endif
