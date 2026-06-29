/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: diag data store
 * This file should be changed only infrequently and with great care.
 */

#ifndef __DIAG_DATA_STORE_H__
#define __DIAG_DATA_STORE_H__

#include "stdbool.h"
#include "errcode.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_DIAG_ITEM_PAYLOAD_SIZE 500 // 定义单个数据项的最大长度

/* 存储元素封装 */
typedef struct {
    uint16_t         cmd_id; // 命令id
    bool             enable_ts; // 使能时间戳。开启后存储和发送数据时数据结构体开头会包含uint32时间戳信息
    uint32_t         ts; // 可选：数据写入时刻时间戳
    uint16_t         len;       // payload长度
    uint8_t          payload[]; // 实际数据
} diag_data_item_t;

// 写入diag存储时的上下文：指定目标存储索引和选项
typedef struct {
    uint8_t  idx;               // 目标存储索引
    bool enable_timestamp;      // 是否在写入时添加时间戳
    uint16_t reserved;
} diag_data_store_ctx_t;

/**
 * @brief 存储驱动的函数指针表/回调结构体
 */
typedef struct diag_data_store_driver_t {
    /**
     * @brief 初始化存储后端.
     * @param config (可选) 指向配置信息的指针.
     * @return 成功时返回ERRCODE_SUCC，否则返回相应的错误代码.
     */
    errcode_t (*init)(void *config);

    /**
     * @brief 反初始化存储后端.
     */
    void (*deinit)(void);

    /**
     * @brief 写入数据.
     * @param cmd_id 命令id.
     * @param data 要写入的数据.
     * @param len 数据长度.
     * @param ctx 数据写入上下文信息.
     * @return 成功时返回ERRCODE_SUCC，否则返回相应的错误代码.
     */
    errcode_t (*write)(uint16_t cmd_id, const void *data, uint32_t len, const diag_data_store_ctx_t *ctx);

    /**
     * @brief 读取数据 (Peek).
     * @param item 指向数据项的指针.
     * @param timeout_ms 读取等待时间.
     * @return 成功时返回ERRCODE_SUCC，否则返回相应的错误代码.
     */
    errcode_t (*read)(diag_data_item_t **item, uint32_t timeout_ms);

    /**
     * @brief 释放数据 (Consume).
     * @param item 指向数据项的指针.
     * @return 成功时返回ERRCODE_SUCC，否则返回相应的错误代码.
     */
    errcode_t (*release)(diag_data_item_t *item);

    /**
     * @brief 获取当前存储的数据量.
     * @return 存储的数据字节数.
     */
    uint32_t (*get_used_size)(void);
} diag_data_store_driver_t;

/**
 * @brief 注册非关键诊断数据存储驱动。
 *
 * 在调用任何其他数据存储API之前，必须先调用此函数。
 *
 * @param[in] driver 指向具有实现的驱动函数的结构的指针。
 * @return 成功时返回ERRCODE_SUCC，否则返回相应的错误代码。
 */
errcode_t diag_data_store_register_non_critical_driver(diag_data_store_driver_t *driver);

/**
 * @brief 注册关键诊断数据存储驱动。
 *
 * 在调用任何其他数据存储API之前，必须先调用此函数。
 *
 * @param[in] driver 指向具有实现的驱动函数的结构的指针。
 * @return 成功时返回ERRCODE_SUCC，否则返回相应的错误代码。
 */
errcode_t diag_data_store_register_critical_driver(diag_data_store_driver_t *driver);

/**
 * @brief 初始化诊断数据存储。
 *
 * @return 成功时返回ERRCODE_SUCC，否则返回相应的错误代码。
 */
errcode_t diag_data_store_init(void);

/**
 * @brief 反初始化诊断数据存储。
 */
void diag_data_store_deinit(void);

/**
 * @brief 向诊断数据存储中写入非关键数据。
 *
 * @param[in] cmd_id 命令Id。
 * @param[in] data 持有数据的缓冲区。
 * @param[in] len 要写入的数据长度。
 * @param[in] ctx 数据写入上下文信息.
 * @return 成功时返回ERRCODE_SUCC，否则返回相应的错误代码。
 */
errcode_t diag_data_store_non_critical_write(uint16_t cmd_id, void *data, uint32_t len,
                                             const diag_data_store_ctx_t *ctx);

/**
 * @brief 从诊断数据存储中读取非关键数据。
 *
 * 此函数读取数据而不将其移除。
 * 处理后，使用diag_data_store_non_critical_release()来移除数据。
 *
 * @param[in] item 指向数据项的指针。
 * @param[in] timeout 毫秒级超时。
 * @return 成功时返回ERRCODE_SUCC，否则返回相应的错误代码。
 */
errcode_t diag_data_store_non_critical_read(diag_data_item_t **item, uint32_t timeout);

/**
 * @brief 从诊断数据存储中释放（移除）非关键数据。
 *
 * 在成功处理（例如，通过BT发送）由diag_data_store_non_critical_read()读取的数据后，应调用此API。
 *
 * @param[in] item 指向数据项的指针。
 * @return 成功时返回ERRCODE_SUCC，否则返回相应的错误代码。
 */
errcode_t diag_data_store_non_critical_release(diag_data_item_t *item);


/**
 * @brief 向诊断数据存储中写入关键数据。
 *
 * @param[in] cmd_id 命令Id。
 * @param[in] data 持有数据的缓冲区。
 * @param[in] len 要写入的数据长度。
 * @param[in] ctx 数据写入上下文信息.
 * @return 成功时返回ERRCODE_SUCC，否则返回相应的错误代码。
 */
errcode_t diag_data_store_critical_write(uint16_t cmd_id, void *data, uint32_t len, const diag_data_store_ctx_t *ctx);

/**
 * @brief 从诊断数据存储中读取关键数据。
 *
 * 此函数读取数据而不将其移除。
 * 处理后，使用diag_data_store_critical_release()来移除数据。
 *
 * @param[in] item 指向数据项的指针。
 * @param[in] timeout 毫秒级超时。
 * @return 成功时返回ERRCODE_SUCC，否则返回相应的错误代码。
 */
errcode_t diag_data_store_critical_read(diag_data_item_t **item, uint32_t timeout);

/**
 * @brief 从诊断数据存储中释放（移除）关键数据。
 *
 * 在成功处理（例如，通过BT发送）由diag_data_store_critical_read()读取的数据后，应调用此API。
 *
 * @param[in] item 指向数据项的指针。
 * @return 成功时返回ERRCODE_SUCC，否则返回相应的错误代码。
 */
errcode_t diag_data_store_critical_release(diag_data_item_t *item);

#ifdef __cplusplus
}
#endif

#endif // __DIAG_DATA_STORE_H__
